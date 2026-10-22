// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#অন্তর্ভাব <utility>
#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/bytestrie.h"
#অন্তর্ভাব "unicode/localpointer.h"
#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "unicode/ures.h"
#অন্তর্ভাব "unicode/uscript.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "loclikelysubtags.h"
#অন্তর্ভাব "lsr.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "uinvchar.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "uniquecharstr.h"
#অন্তর্ভাব "uresdata.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "uvector.h"

U_NAMESPACE_BEGIN

নামাকাশ {

constexpr অক্ষর PSEUDO_ACCENTS_PREFIX = '\'';  // -XA, -PSACCENT
constexpr অক্ষর PSEUDO_BIDI_PREFIX = '+';  // -XB, -PSBIDI
constexpr অক্ষর PSEUDO_CRACKED_PREFIX = ',';  // -XC, -PSCRACK

}  // namespace

Locale_Distance_তথ্য::Locale_Distance_তথ্য(Locale_Distance_তথ্য &&তথ্য) :
        distanceTrieBytes(তথ্য.distanceTrieBytes),
        regionToPartitions(তথ্য.regionToPartitions),
        partitions(তথ্য.partitions),
        paradigms(তথ্য.paradigms), paradigms_দৈর্ঘ্য(তথ্য.paradigms_দৈর্ঘ্য),
        distances(তথ্য.distances) {
    তথ্য.partitions = nullptr;
    তথ্য.paradigms = nullptr;
}

Locale_Distance_তথ্য::~Locale_Distance_তথ্য() {
    uprv_free(partitions);
    delete[] paradigms;
}

কাঠামো Likely_Subtags_তথ্য {
    UResourceBundle *langInfoBundle = nullptr;
    UniqueCharStrings strings;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    ধ্রুবক uint8_t *trieBytes = nullptr;
    LSR *lsrs = nullptr;
    int32_t lsrs_দৈর্ঘ্য = 0;

    Locale_Distance_তথ্য distance_তথ্য;

    Likely_Subtags_তথ্য(UErrorCode &errorCode) : strings(errorCode) {}

    ~Likely_Subtags_তথ্য() {
        ures_close(langInfoBundle);
        delete[] lsrs;
    }

    শূন্য load(UErrorCode &errorCode) {
        যদি (U_FAILURE(errorCode)) { ফেরত; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        যদি (U_FAILURE(errorCode)) { ফেরত; }
        স্তূপ_UResource_Bundle স্তূপ_অস্থায়ী_Bundle;
        Resource_তথ্য_মান value;
        ures_get_মান_With_Fallback(langInfoBundle, "likely", স্তূপ_অস্থায়ী_Bundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        যদি (U_FAILURE(errorCode)) { ফেরত; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languages_দৈর্ঘ্য = 0, regions_দৈর্ঘ্য = 0, lsr_Subtags_দৈর্ঘ্য = 0;
        ResourceArray m49Array;
        যদি (likelyTable.find_মান("m49", value)) {
            m49Array = value.getArray(errorCode);
        } অন্যথা {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ফেরত;
        }
        যদি (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languages_দৈর্ঘ্য, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regions_দৈর্ঘ্য, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsr_Subtags_দৈর্ঘ্য, errorCode)) {
            ফেরত;
        }
        যদি ((languages_দৈর্ঘ্য & 1) != 0 ||
                (regions_দৈর্ঘ্য & 1) != 0 ||
                (lsr_Subtags_দৈর্ঘ্য % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            ফেরত;
        }
        যদি (lsr_Subtags_দৈর্ঘ্য == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ফেরত;
        }

        যদি (!likelyTable.find_মান("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ফেরত;
        }
        int32_t length;
        trieBytes = value.get_দ্বিমিক(length, errorCode);
        যদি (U_FAILURE(errorCode)) { ফেরত; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_get_মান_With_Fallback(langInfoBundle, "match", স্তূপ_অস্থায়ী_Bundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitions_দৈর্ঘ্য = 0, paradigm_Subtags_দৈর্ঘ্য = 0;
        যদি (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            যদি (U_FAILURE(errorCode)) { ফেরত; }

            যদি (matchTable.find_মান("trie", value)) {
                distance_তথ্য.distanceTrieBytes = value.get_দ্বিমিক(length, errorCode);
                যদি (U_FAILURE(errorCode)) { ফেরত; }
            }

            যদি (matchTable.find_মান("regionToPartitions", value)) {
                distance_তথ্য.regionToPartitions = value.get_দ্বিমিক(length, errorCode);
                যদি (U_FAILURE(errorCode)) { ফেরত; }
                যদি (length < LSR::REGION_সূচক_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    ফেরত;
                }
            }

            যদি (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitions_দৈর্ঘ্য, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigm_Subtags_দৈর্ঘ্য, errorCode)) {
                ফেরত;
            }
            যদি ((paradigm_Subtags_দৈর্ঘ্য % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                ফেরত;
            }

            যদি (matchTable.find_মান("distances", value)) {
                distance_তথ্য.distances = value.getIntVector(length, errorCode);
                যদি (U_FAILURE(errorCode)) { ফেরত; }
                যদি (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    ফেরত;
                }
            }
        } অন্যথা যদি (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } অন্যথা {  // error other than missing resource
            errorCode = matchErrorCode;
            ফেরত;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languages_দৈর্ঘ্য / 2, errorCode);
        জন্য (int32_t i = 0; i < languages_দৈর্ঘ্য; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regions_দৈর্ঘ্য / 2, errorCode);
        জন্য (int32_t i = 0; i < regions_দৈর্ঘ্য; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        যদি (U_FAILURE(errorCode)) { ফেরত; }

        lsrs_দৈর্ঘ্য = lsr_Subtags_দৈর্ঘ্য / 3;
        lsrs = নতুন LSR[lsrs_দৈর্ঘ্য];
        যদি (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
        }
        জন্য (int32_t i = 0, j = 0; i < lsr_Subtags_দৈর্ঘ্য; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        যদি (partitions_দৈর্ঘ্য > 0) {
            distance_তথ্য.partitions = static_cast<ধ্রুবক অক্ষর **>(
                uprv_malloc(partitions_দৈর্ঘ্য * আকার(ধ্রুবক অক্ষর *)));
            যদি (distance_তথ্য.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ফেরত;
            }
            জন্য (int32_t i = 0; i < partitions_দৈর্ঘ্য; ++i) {
                distance_তথ্য.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        যদি (paradigm_Subtags_দৈর্ঘ্য > 0) {
            distance_তথ্য.paradigms_দৈর্ঘ্য = paradigm_Subtags_দৈর্ঘ্য / 3;
            LSR *paradigms = নতুন LSR[distance_তথ্য.paradigms_দৈর্ঘ্য];
            যদি (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ফেরত;
            }
            জন্য (int32_t i = 0, j = 0; i < paradigm_Subtags_দৈর্ঘ্য; i += 3, ++j) {
                paradigms[j] = LSR(strings.get(paradigmSubtagIndexes[i]),
                                   strings.get(paradigmSubtagIndexes[i + 1]),
                                   strings.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            distance_তথ্য.paradigms = paradigms;
        }
    }

ব্যক্তিগত:
    যৌক্তিক readStrings(ধ্রুবক ResourceTable &table, ধ্রুবক অক্ষর *চাবি, Resource_মান &value,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
        যদি (table.find_মান(চাবি, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
            length = stringArray.get_আকার();
            যদি (length == 0) { ফেরত সত্য; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            যদি (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ফেরত মিথ্যা;
            }
            জন্য (পূর্ণসংখ্যা i = 0; i < length; ++i) {
                যদি (stringArray.get_মান(i, value)) {  // returns true because i < length
                    int32_t str_দৈর্ঘ্য = 0;
                    rawIndexes[i] = strings.add(value.getString(str_দৈর্ঘ্য, errorCode), errorCode);
                    যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
                }
            }
        }
        ফেরত সত্য;
    }
    UnicodeString toLanguage(পূর্ণসংখ্যা encoded) {
        যদি (encoded == 0) {
            ফেরত UNICODE_STRING_SIMPLE("");
        }
        যদি (encoded == 1) {
            ফেরত UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        অক্ষর lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        যদি (encoded / (27 * 27) == 0) {
            ফেরত UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        ফেরত UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(পূর্ণসংখ্যা encoded) {
        যদি (encoded == 0) {
            ফেরত UNICODE_STRING_SIMPLE("");
        }
        যদি (encoded == 1) {
            ফেরত UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        ধ্রুবক অক্ষর* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        যদি (script == nullptr) {
            ফেরত UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        ফেরত UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49_সূচক_To_Code(ধ্রুবক ResourceArray &m49Array, Resource_মান &value, পূর্ণসংখ্যা সূচক, UErrorCode &errorCode) {
        যদি (U_FAILURE(errorCode)) {
            ফেরত UNICODE_STRING_SIMPLE("");
        }
        যদি (m49Array.get_মান(সূচক, value)) {
            ফেরত value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        ফেরত UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(ধ্রুবক ResourceArray& m49Array, Resource_মান &value, পূর্ণসংখ্যা encoded, UErrorCode &errorCode) {
        যদি (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            ফেরত UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        যদি (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            ফেরত  m49_সূচক_To_Code(m49Array, value, encoded, errorCode);
        }
        অক্ষর region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        ফেরত UnicodeString(region, 2, US_INV);
    }

    যৌক্তিক readLSREncodedStrings(ধ্রুবক ResourceTable &table, ধ্রুবক অক্ষর* চাবি, Resource_মান &value, ধ্রুবক ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
        যদি (table.find_মান(চাবি, value)) {
            ধ্রুবক int32_t* vectors = value.getIntVector(length, errorCode);
            যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
            যদি (length == 0) { ফেরত সত্য; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            যদি (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ফেরত মিথ্যা;
            }
            জন্য (পূর্ণসংখ্যা i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.add_By_মান(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.add_By_মান(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.add_By_মান(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
            }
            length *= 3;
        }
        ফেরত সত্য;
    }
};

নামাকাশ {

LikelySubtags *gLikelySubtags = nullptr;
UVector *gMacroregions = nullptr;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    delete gLikelySubtags;
    gLikelySubtags = nullptr;
    delete gMacroregions;
    gMacroregions = nullptr;
    gInitOnce.reset();
    ফেরত সত্য;
}

constexpr ধ্রুবক char16_t* MACROREGION_HARDCODE[] = {
    u"001~3",
    u"005",
    u"009",
    u"011",
    u"013~5",
    u"017~9",
    u"021",
    u"029",
    u"030",
    u"034~5",
    u"039",
    u"053~4",
    u"057",
    u"061",
    u"142~3",
    u"145",
    u"150~1",
    u"154~5",
    u"202",
    u"419",
    u"EU",
    u"EZ",
    u"QO",
    u"UN",
};

constexpr char16_t RANGE_MARKER = 0x7E; /* '~' */
শূন্য processMacroregionRange(ধ্রুবক UnicodeString& regionName, UVector* newMacroRegions, UErrorCode& status) {
    যদি (U_FAILURE(status)) { ফেরত; }
    int32_t rangeMarkerLocation = regionName.সূচক_Of(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    যদি ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        যখন ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(নতুন UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } অন্যথা {
        LocalPointer<UnicodeString> newRegion(নতুন UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#যদি U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    যদি (U_FAILURE(status)) { ফেরত nullptr; }
    LocalPointer<UVector> newMacroRegions(নতুন UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplemental_তথ্য(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_get_By_চাবি(supplemental_তথ্য.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_get_By_চাবি(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_get_By_চাবি(regionList.getAlias(),"macroregion",nullptr,&status));

    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }

    যখন (ures_has_পরবর্তী(regionMacro.getAlias())) {
        UnicodeString regionName = ures_get_পরবর্তী_Unicode_String(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        যদি (U_FAILURE(status)) {
            ফেরত nullptr;
        }
    }

    ফেরত newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    যদি (U_FAILURE(status)) { ফেরত nullptr; }
    LocalPointer<UVector> newMacroRegions(নতুন UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }

    জন্য (ধ্রুবক auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        যদি (U_FAILURE(status)) {
            ফেরত nullptr;
        }
    }

    ফেরত newMacroRegions.orphan();
}

}  // namespace

শূন্য U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    Likely_Subtags_তথ্য তথ্য(errorCode);
    তথ্য.load(errorCode);
    যদি (U_FAILURE(errorCode)) { ফেরত; }
    gLikelySubtags = নতুন LikelySubtags(তথ্য);
    gMacroregions = getStaticMacroregions(errorCode);
#যদি U_DEBUG
    auto macroregions_From_তথ্য = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregions_From_তথ্য));
    delete macroregions_From_তথ্য;
#endif
    যদি (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ধ্রুবক LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ফেরত nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    ফেরত gLikelySubtags;
}

LikelySubtags::LikelySubtags(Likely_Subtags_তথ্য &তথ্য) :
        langInfoBundle(তথ্য.langInfoBundle),
        strings(তথ্য.strings.orphanCharStrings()),
        languageAliases(std::move(তথ্য.languageAliases)),
        regionAliases(std::move(তথ্য.regionAliases)),
        trie(তথ্য.trieBytes),
        lsrs(তথ্য.lsrs),
#যদি U_DEBUG
        lsrs_দৈর্ঘ্য(তথ্য.lsrs_দৈর্ঘ্য),
#endif // U_DEBUG
        distance_তথ্য(std::move(তথ্য.distance_তথ্য)) {
    তথ্য.langInfoBundle = nullptr;
    তথ্য.lsrs = nullptr;

    // Cache the result of looking up language="und" encoded as "*", and "und-Zzzz" ("**").
    UString_Trie_ফলাফল ফলাফল = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_পরবর্তী(ফলাফল));
    trieUndState = trie.getState64();
    ফলাফল = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_পরবর্তী(ফলাফল));
    trieUndZzzzState = trie.getState64();
    ফলাফল = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_মান(ফলাফল));
    default_Lsr_সূচক = trie.get_মান();
    trie.reset();

    জন্য (char16_t c = u'a'; c <= u'z'; ++c) {
        ফলাফল = trie.next(c);
        যদি (ফলাফল == USTRINGTRIE_NO_মান) {
            trieFirstLetterStates[c - u'a'] = trie.getState64();
        }
        trie.reset();
    }
}

LikelySubtags::~LikelySubtags() {
    ures_close(langInfoBundle);
    delete strings;
    delete[] lsrs;
}

LSR LikelySubtags::makeMaximizedLsrFrom(ধ্রুবক Locale &locale,
                                         যৌক্তিক return_প্রবেশ_If_Unmatch,
                                         UErrorCode &errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) { ফেরত {}; }
    যদি (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত {};
    }
    ধ্রুবক অক্ষর *name = locale.getName();
    যদি (!return_প্রবেশ_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        ফেরত LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_প্রবেশ_If_Unmatch, errorCode);

    যদি (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        ফেরত LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    ফেরত max;
}

নামাকাশ {

ধ্রুবক অক্ষর *getCanonical(ধ্রুবক CharStringMap &aliases, ধ্রুবক অক্ষর *alias) {
    ধ্রুবক অক্ষর *canonical = aliases.get(alias);
    ফেরত canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(ধ্রুবক অক্ষর *language, ধ্রুবক অক্ষর *script, ধ্রুবক অক্ষর *region,
                                     ধ্রুবক অক্ষর *variant,
                                     যৌক্তিক return_প্রবেশ_If_Unmatch,
                                     UErrorCode &errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) { ফেরত {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    যদি (!return_প্রবেশ_If_Unmatch) {
        অক্ষর c1;
        যদি (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            নির্বাচন (c1) {
            ক্ষেত্র 'A':
                ফেরত LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            ক্ষেত্র 'B':
                ফেরত LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            ক্ষেত্র 'C':
                ফেরত LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            পূর্বনির্ধারিত:  // normal locale
                বিরতি;
            }
        }

        যদি (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            যদি (uprv_strcmp(variant, "PSACCENT") == 0) {
                ফেরত LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } অন্যথা যদি (uprv_strcmp(variant, "PSBIDI") == 0) {
                ফেরত LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } অন্যথা যদি (uprv_strcmp(variant, "PSCRACK") == 0) {
                ফেরত LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    ফেরত maximize(language, script, region, return_প্রবেশ_If_Unmatch, errorCode);
}

LSR LikelySubtags::maximize(ধ্রুবক অক্ষর *language, ধ্রুবক অক্ষর *script, ধ্রুবক অক্ষর *region,
                             যৌক্তিক return_প্রবেশ_If_Unmatch,
                             UErrorCode &errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) { ফেরত {}; }
    ফেরত maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_প্রবেশ_If_Unmatch,
                    errorCode);
}

যৌক্তিক LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    ফেরত gMacroregions->contains((শূন্য *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             যৌক্তিক return_প্রবেশ_If_Unmatch,
                             UErrorCode &errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) { ফেরত {}; }
    যদি (language.compare("und") == 0) {
        language = "";
    }
    যদি (script.compare("Zzzz") == 0) {
        script = "";
    }
    যদি (region.compare("ZZ") == 0) {
        region = "";
    }
    যদি (!script.empty() && !region.empty() && !language.empty()) {
        ফেরত LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    যৌক্তিক retainLanguage = মিথ্যা;
    যৌক্তিক retainScript = মিথ্যা;
    যৌক্তিক retainRegion = মিথ্যা;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    যদি (0 <= (c0 = uprv_lowerOrdinal(language.তথ্য()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trie_পরবর্তী(iter.resetToState64(state), language, 1);
    } অন্যথা {
        value = trie_পরবর্তী(iter, language, 0);
    }
    যৌক্তিক matchLanguage = (value >= 0);
    যৌক্তিক matchScript = মিথ্যা;
    যদি (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } অন্যথা {
        retainLanguage = সত্য;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    যদি (value >= 0 && !script.empty()) {
        matchScript = সত্য;
    }
    যদি (value > 0) {
        // Intermediate or final value from just language.
        যদি (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } অন্যথা {
        value = trie_পরবর্তী(iter, script, 0);
        যদি (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } অন্যথা {
            retainScript = সত্য;
            যদি (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } অন্যথা {
                iter.resetToState64(state);
                value = trie_পরবর্তী(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    যৌক্তিক matchRegion = মিথ্যা;
    যদি (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } অন্যথা {
        value = trie_পরবর্তী(iter, region, 0);
        যদি (value >= 0) {
            যদি (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = সত্য;
                matchRegion = সত্য;
            }
        } অন্যথা {
            retainRegion = সত্য;
            যদি (state == 0) {
                value = default_Lsr_সূচক;
            } অন্যথা {
                iter.resetToState64(state);
                value = trie_পরবর্তী(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                যদি (value < 0) {
                    retainLanguage = !language.empty();
                    retainScript = !script.empty();
                    retainRegion = !region.empty();
                    // Fallback to und_$region =>
                    iter.resetToState64(trieUndState);  // "und" ("*")
                    value = trie_পরবর্তী(iter, "", 0);
                    U_ASSERT(value == 0);
                    int64_t trieUndEmptyState = iter.getState64();
                    value = trie_পরবর্তী(iter, region, 0);
                    // Fallback to und =>
                    যদি (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trie_পরবর্তী(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrs_দৈর্ঘ্য);
    যদি (return_প্রবেশ_If_Unmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      ফেরত LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    যদি (language.empty()) {
        language = StringPiece("und");
    }

    যদি (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        ধ্রুবক LSR &matched = lsrs[value];
        ফেরত LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    যদি (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    যদি (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    যদি (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    ফেরত LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(ধ্রুবক LSR &lsr, ধ্রুবক LSR &other, int32_t likelyInfo) ধ্রুবক {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    যদি (uprv_strcmp(lsr.language, other.language) != 0) {
        ফেরত 0xfffffffc;  // negative, lsr not better than other
    }
    যদি (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t সূচক;
        যদি (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            সূচক = likelyInfo >> 2;
        } অন্যথা {
            সূচক = get_Likely_সূচক(lsr.language, "");
            likelyInfo = সূচক << 2;
        }
        ধ্রুবক LSR &likely = lsrs[সূচক];
        যদি (uprv_strcmp(lsr.script, likely.script) == 0) {
            ফেরত likelyInfo | 1;
        } অন্যথা {
            ফেরত likelyInfo & ~1;
        }
    }
    যদি (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t সূচক;
        যদি (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            সূচক = likelyInfo >> 2;
        } অন্যথা {
            সূচক = get_Likely_সূচক(lsr.language, lsr.region);
            likelyInfo = (সূচক << 2) | 2;
        }
        ধ্রুবক LSR &likely = lsrs[সূচক];
        যদি (uprv_strcmp(lsr.region, likely.region) == 0) {
            ফেরত likelyInfo | 1;
        } অন্যথা {
            ফেরত likelyInfo & ~1;
        }
    }
    ফেরত likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::get_Likely_সূচক(ধ্রুবক অক্ষর *language, ধ্রুবক অক্ষর *script) ধ্রুবক {
    যদি (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    যদি (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    যদি (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trie_পরবর্তী(iter.resetToState64(state), language, 1);
    } অন্যথা {
        value = trie_পরবর্তী(iter, language, 0);
    }
    যদি (value >= 0) {
        state = iter.getState64();
    } অন্যথা {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    যদি (value > 0) {
        // Intermediate or final value from just language.
        যদি (value == SKIP_SCRIPT) {
            value = 0;
        }
    } অন্যথা {
        value = trie_পরবর্তী(iter, script, 0);
        যদি (value >= 0) {
            state = iter.getState64();
        } অন্যথা {
            যদি (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } অন্যথা {
                iter.resetToState64(state);
                value = trie_পরবর্তী(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    যদি (value > 0) {
        // Final value from just language or language+script.
    } অন্যথা {
        value = trie_পরবর্তী(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrs_দৈর্ঘ্য);
    ফেরত value;
}

int32_t LikelySubtags::trie_পরবর্তী(BytesTrie &iter, ধ্রুবক অক্ষর *s, int32_t i) {
    UString_Trie_ফলাফল ফলাফল;
    uint8_t c;
    যদি ((c = s[i]) == 0) {
        ফলাফল = iter.next(u'*');
    } অন্যথা {
        জন্য (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            যদি (next != 0) {
                যদি (!USTRINGTRIE_HAS_পরবর্তী(iter.next(c))) {
                    ফেরত -1;
                }
            } অন্যথা {
                // last character of this subtag
                ফলাফল = iter.next(c | 0x80);
                বিরতি;
            }
            c = next;
        }
    }
    নির্বাচন (ফলাফল) {
    ক্ষেত্র USTRINGTRIE_NO_MATCH: ফেরত -1;
    ক্ষেত্র USTRINGTRIE_NO_মান: ফেরত 0;
    ক্ষেত্র USTRINGTRIE_INTERMEDIATE_মান:
        U_ASSERT(iter.get_মান() == SKIP_SCRIPT);
        ফেরত SKIP_SCRIPT;
    ক্ষেত্র USTRINGTRIE_FINAL_মান: ফেরত iter.get_মান();
    পূর্বনির্ধারিত: ফেরত -1;
    }
}
int32_t LikelySubtags::trie_পরবর্তী(BytesTrie &iter, StringPiece s, int32_t i) {
    UString_Trie_ফলাফল ফলাফল;
    uint8_t c;
    যদি (s.length() == i) {
        ফলাফল = iter.next(u'*');
    } অন্যথা {
        c = s.তথ্য()[i];
        জন্য (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            যদি (i+1 != s.length()) {
                যদি (!USTRINGTRIE_HAS_পরবর্তী(iter.next(c))) {
                    ফেরত -1;
                }
                c = s.তথ্য()[++i];
            } অন্যথা {
                // last character of this subtag
                ফলাফল = iter.next(c | 0x80);
                বিরতি;
            }
        }
    }
    নির্বাচন (ফলাফল) {
    ক্ষেত্র USTRINGTRIE_NO_MATCH: ফেরত -1;
    ক্ষেত্র USTRINGTRIE_NO_মান: ফেরত 0;
    ক্ষেত্র USTRINGTRIE_INTERMEDIATE_মান:
        U_ASSERT(iter.get_মান() == SKIP_SCRIPT);
        ফেরত SKIP_SCRIPT;
    ক্ষেত্র USTRINGTRIE_FINAL_মান: ফেরত iter.get_মান();
    পূর্বনির্ধারিত: ফেরত -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    যৌক্তিক favorScript,
                                    UErrorCode &errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) { ফেরত {}; }
    LSR max = maximize(language, script, region, সত্য, errorCode);
    যদি (U_FAILURE(errorCode)) { ফেরত {}; }
    // If no match, return it.
    যদি (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        ফেরত LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", সত্য, errorCode);
    যদি (U_FAILURE(errorCode)) { ফেরত {}; }
    যদি (test.isEquivalentTo(max)) {
        ফেরত LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    যদি (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, সত্য, errorCode);
        যদি (U_FAILURE(errorCode)) { ফেরত {}; }
        যদি (test.isEquivalentTo(max)) {
            ফেরত LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", সত্য, errorCode);
    যদি (U_FAILURE(errorCode)) { ফেরত {}; }
    যদি (test.isEquivalentTo(max)) {
        ফেরত LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    যদি (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, সত্য, errorCode);
        যদি (U_FAILURE(errorCode)) { ফেরত {}; }
        যদি (test.isEquivalentTo(max)) {
            ফেরত LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    ফেরত LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
