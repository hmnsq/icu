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

constexpr বর্ণ PSEUDO_ACCENTS_PREFIX = '\'';  // -XA, -PSACCENT
constexpr বর্ণ PSEUDO_BIDI_PREFIX = '+';  // -XB, -PSBIDI
constexpr বর্ণ PSEUDO_CRACKED_PREFIX = ',';  // -XC, -PSCRACK

}  // namespace

LocaleDistanceData::LocaleDistanceData(LocaleDistanceData &&data) :
        distanceTrieBytes(data.distanceTrieBytes),
        regionToPartitions(data.regionToPartitions),
        partitions(data.partitions),
        paradigms(data.paradigms), paradigmsLength(data.paradigmsLength),
        distances(data.distances) {
    data.partitions = nullptr;
    data.paradigms = nullptr;
}

LocaleDistanceData::~LocaleDistanceData() {
    uprv_free(partitions);
    delete[] paradigms;
}

সংরচনা LikelySubtagsData {
    UResourceBundle *langInfoBundle = nullptr;
    UniqueCharStrings strings;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    ধ্রুব uint8_t *trieBytes = nullptr;
    LSR *lsrs = nullptr;
    int32_t lsrsLength = 0;

    LocaleDistanceData distanceData;

    LikelySubtagsData(UErrorCode &errorCode) : strings(errorCode) {}

    ~LikelySubtagsData() {
        ures_close(langInfoBundle);
        delete[] lsrs;
    }

    শূন্য load(UErrorCode &errorCode) {
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
        স্তূপ_UResource_Bundle স্তূপ_Temp_Bundle;
        Resource_Data_মান value;
        ures_get_মান_With_Fallback(langInfoBundle, "likely", স্তূপ_Temp_Bundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        যদি (likelyTable.find_মান("m49", value)) {
            m49Array = value.getArray(errorCode);
        } অন্যথা {
            errorCode = U_MISSING_RESOURCE_ERROR;
            প্রত্যাগম;
        }
        যদি (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            প্রত্যাগম;
        }
        যদি ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            প্রত্যাগম;
        }
        যদি (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            প্রত্যাগম;
        }

        যদি (!likelyTable.find_মান("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            প্রত্যাগম;
        }
        int32_t length;
        trieBytes = value.get_দ্বিমিক(length, errorCode);
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_get_মান_With_Fallback(langInfoBundle, "match", স্তূপ_Temp_Bundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        যদি (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }

            যদি (matchTable.find_মান("trie", value)) {
                distanceData.distanceTrieBytes = value.get_দ্বিমিক(length, errorCode);
                যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
            }

            যদি (matchTable.find_মান("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.get_দ্বিমিক(length, errorCode);
                যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
                যদি (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    প্রত্যাগম;
                }
            }

            যদি (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                প্রত্যাগম;
            }
            যদি ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                প্রত্যাগম;
            }

            যদি (matchTable.find_মান("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
                যদি (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    প্রত্যাগম;
                }
            }
        } অন্যথা যদি (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } অন্যথা {  // error other than missing resource
            errorCode = matchErrorCode;
            প্রত্যাগম;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languagesLength / 2, errorCode);
        প্রতি (int32_t i = 0; i < languagesLength; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regionsLength / 2, errorCode);
        প্রতি (int32_t i = 0; i < regionsLength; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = নূতন LSR[lsrsLength];
        যদি (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম;
        }
        প্রতি (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        যদি (partitionsLength > 0) {
            distanceData.partitions = static_cast<ধ্রুব বর্ণ **>(
                uprv_malloc(partitionsLength * পরিমাণ(ধ্রুব বর্ণ *)));
            যদি (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                প্রত্যাগম;
            }
            প্রতি (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        যদি (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = নূতন LSR[distanceData.paradigmsLength];
            যদি (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                প্রত্যাগম;
            }
            প্রতি (int32_t i = 0, j = 0; i < paradigmSubtagsLength; i += 3, ++j) {
                paradigms[j] = LSR(strings.get(paradigmSubtagIndexes[i]),
                                   strings.get(paradigmSubtagIndexes[i + 1]),
                                   strings.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            distanceData.paradigms = paradigms;
        }
    }

গুপ্ত:
    তার্কিক readStrings(ধ্রুব ResourceTable &table, ধ্রুব বর্ণ *চাবি, Resource_মান &value,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
        যদি (table.find_মান(চাবি, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
            length = stringArray.getSize();
            যদি (length == 0) { প্রত্যাগম সত্য; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            যদি (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                প্রত্যাগম অসত্য;
            }
            প্রতি (পূর্ণাঙ্ক i = 0; i < length; ++i) {
                যদি (stringArray.get_মান(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
                }
            }
        }
        প্রত্যাগম সত্য;
    }
    UnicodeString toLanguage(পূর্ণাঙ্ক encoded) {
        যদি (encoded == 0) {
            প্রত্যাগম UNICODE_STRING_SIMPLE("");
        }
        যদি (encoded == 1) {
            প্রত্যাগম UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        বর্ণ lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        যদি (encoded / (27 * 27) == 0) {
            প্রত্যাগম UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        প্রত্যাগম UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(পূর্ণাঙ্ক encoded) {
        যদি (encoded == 0) {
            প্রত্যাগম UNICODE_STRING_SIMPLE("");
        }
        যদি (encoded == 1) {
            প্রত্যাগম UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        ধ্রুব বর্ণ* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        যদি (script == nullptr) {
            প্রত্যাগম UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        প্রত্যাগম UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(ধ্রুব ResourceArray &m49Array, Resource_মান &value, পূর্ণাঙ্ক index, UErrorCode &errorCode) {
        যদি (U_FAILURE(errorCode)) {
            প্রত্যাগম UNICODE_STRING_SIMPLE("");
        }
        যদি (m49Array.get_মান(index, value)) {
            প্রত্যাগম value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        প্রত্যাগম UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(ধ্রুব ResourceArray& m49Array, Resource_মান &value, পূর্ণাঙ্ক encoded, UErrorCode &errorCode) {
        যদি (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            প্রত্যাগম UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        যদি (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            প্রত্যাগম  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        বর্ণ region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        প্রত্যাগম UnicodeString(region, 2, US_INV);
    }

    তার্কিক readLSREncodedStrings(ধ্রুব ResourceTable &table, ধ্রুব বর্ণ* চাবি, Resource_মান &value, ধ্রুব ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
        যদি (table.find_মান(চাবি, value)) {
            ধ্রুব int32_t* vectors = value.getIntVector(length, errorCode);
            যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
            যদি (length == 0) { প্রত্যাগম সত্য; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            যদি (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                প্রত্যাগম অসত্য;
            }
            প্রতি (পূর্ণাঙ্ক i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.add_By_মান(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.add_By_মান(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.add_By_মান(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
            }
            length *= 3;
        }
        প্রত্যাগম সত্য;
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
    প্রত্যাগম সত্য;
}

constexpr ধ্রুব char16_t* MACROREGION_HARDCODE[] = {
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
শূন্য processMacroregionRange(ধ্রুব UnicodeString& regionName, UVector* newMacroRegions, UErrorCode& status) {
    যদি (U_FAILURE(status)) { প্রত্যাগম; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    যদি ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        যাবত্ ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(নূতন UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } অন্যথা {
        LocalPointer<UnicodeString> newRegion(নূতন UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#যদি U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    যদি (U_FAILURE(status)) { প্রত্যাগম nullptr; }
    LocalPointer<UVector> newMacroRegions(নূতন UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_get_By_চাবি(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_get_By_চাবি(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_get_By_চাবি(regionList.getAlias(),"macroregion",nullptr,&status));

    যদি (U_FAILURE(status)) {
        প্রত্যাগম nullptr;
    }

    যাবত্ (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        যদি (U_FAILURE(status)) {
            প্রত্যাগম nullptr;
        }
    }

    প্রত্যাগম newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    যদি (U_FAILURE(status)) { প্রত্যাগম nullptr; }
    LocalPointer<UVector> newMacroRegions(নূতন UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    যদি (U_FAILURE(status)) {
        প্রত্যাগম nullptr;
    }

    প্রতি (ধ্রুব auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        যদি (U_FAILURE(status)) {
            প্রত্যাগম nullptr;
        }
    }

    প্রত্যাগম newMacroRegions.orphan();
}

}  // namespace

শূন্য U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
    gLikelySubtags = নূতন LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#যদি U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    যদি (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ধ্রুব LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    প্রত্যাগম gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#যদি U_DEBUG
        lsrsLength(data.lsrsLength),
#endif // U_DEBUG
        distanceData(std::move(data.distanceData)) {
    data.langInfoBundle = nullptr;
    data.lsrs = nullptr;

    // Cache the result of looking up language="und" encoded as "*", and "und-Zzzz" ("**").
    UStringTrieResult result = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_NEXT(result));
    trieUndState = trie.getState64();
    result = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_NEXT(result));
    trieUndZzzzState = trie.getState64();
    result = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_মান(result));
    defaultLsrIndex = trie.get_মান();
    trie.reset();

    প্রতি (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        যদি (result == USTRINGTRIE_NO_মান) {
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

LSR LikelySubtags::makeMaximizedLsrFrom(ধ্রুব Locale &locale,
                                         তার্কিক return_প্রবেশ_If_Unmatch,
                                         UErrorCode &errorCode) ধ্রুব {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম {}; }
    যদি (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম {};
    }
    ধ্রুব বর্ণ *name = locale.getName();
    যদি (!return_প্রবেশ_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        প্রত্যাগম LSR(name, "", "", LSR::EXPLICIT_LSR);
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
        প্রত্যাগম LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    প্রত্যাগম max;
}

নামাকাশ {

ধ্রুব বর্ণ *getCanonical(ধ্রুব CharStringMap &aliases, ধ্রুব বর্ণ *alias) {
    ধ্রুব বর্ণ *canonical = aliases.get(alias);
    প্রত্যাগম canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(ধ্রুব বর্ণ *language, ধ্রুব বর্ণ *script, ধ্রুব বর্ণ *region,
                                     ধ্রুব বর্ণ *variant,
                                     তার্কিক return_প্রবেশ_If_Unmatch,
                                     UErrorCode &errorCode) ধ্রুব {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    যদি (!return_প্রবেশ_If_Unmatch) {
        বর্ণ c1;
        যদি (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            বিকল্প (c1) {
            পক্ষ 'A':
                প্রত্যাগম LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            পক্ষ 'B':
                প্রত্যাগম LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            পক্ষ 'C':
                প্রত্যাগম LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            স্বভাব:  // normal locale
                ভঙ্গ;
            }
        }

        যদি (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            যদি (uprv_strcmp(variant, "PSACCENT") == 0) {
                প্রত্যাগম LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } অন্যথা যদি (uprv_strcmp(variant, "PSBIDI") == 0) {
                প্রত্যাগম LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } অন্যথা যদি (uprv_strcmp(variant, "PSCRACK") == 0) {
                প্রত্যাগম LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    প্রত্যাগম maximize(language, script, region, return_প্রবেশ_If_Unmatch, errorCode);
}

LSR LikelySubtags::maximize(ধ্রুব বর্ণ *language, ধ্রুব বর্ণ *script, ধ্রুব বর্ণ *region,
                             তার্কিক return_প্রবেশ_If_Unmatch,
                             UErrorCode &errorCode) ধ্রুব {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম {}; }
    প্রত্যাগম maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_প্রবেশ_If_Unmatch,
                    errorCode);
}

তার্কিক LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) ধ্রুব {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    প্রত্যাগম gMacroregions->contains((শূন্য *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             তার্কিক return_প্রবেশ_If_Unmatch,
                             UErrorCode &errorCode) ধ্রুব {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম {}; }
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
        প্রত্যাগম LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    তার্কিক retainLanguage = অসত্য;
    তার্কিক retainScript = অসত্য;
    তার্কিক retainRegion = অসত্য;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    যদি (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } অন্যথা {
        value = trieNext(iter, language, 0);
    }
    তার্কিক matchLanguage = (value >= 0);
    তার্কিক matchScript = অসত্য;
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
        value = trieNext(iter, script, 0);
        যদি (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } অন্যথা {
            retainScript = সত্য;
            যদি (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } অন্যথা {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    তার্কিক matchRegion = অসত্য;
    যদি (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } অন্যথা {
        value = trieNext(iter, region, 0);
        যদি (value >= 0) {
            যদি (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = সত্য;
                matchRegion = সত্য;
            }
        } অন্যথা {
            retainRegion = সত্য;
            যদি (state == 0) {
                value = defaultLsrIndex;
            } অন্যথা {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                যদি (value < 0) {
                    retainLanguage = !language.empty();
                    retainScript = !script.empty();
                    retainRegion = !region.empty();
                    // Fallback to und_$region =>
                    iter.resetToState64(trieUndState);  // "und" ("*")
                    value = trieNext(iter, "", 0);
                    U_ASSERT(value == 0);
                    int64_t trieUndEmptyState = iter.getState64();
                    value = trieNext(iter, region, 0);
                    // Fallback to und =>
                    যদি (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    যদি (return_প্রবেশ_If_Unmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      প্রত্যাগম LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    যদি (language.empty()) {
        language = StringPiece("und");
    }

    যদি (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        ধ্রুব LSR &matched = lsrs[value];
        প্রত্যাগম LSR(matched.language, matched.script, matched.region, matched.flags);
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
    প্রত্যাগম LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(ধ্রুব LSR &lsr, ধ্রুব LSR &other, int32_t likelyInfo) ধ্রুব {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    যদি (uprv_strcmp(lsr.language, other.language) != 0) {
        প্রত্যাগম 0xfffffffc;  // negative, lsr not better than other
    }
    যদি (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        যদি (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } অন্যথা {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        ধ্রুব LSR &likely = lsrs[index];
        যদি (uprv_strcmp(lsr.script, likely.script) == 0) {
            প্রত্যাগম likelyInfo | 1;
        } অন্যথা {
            প্রত্যাগম likelyInfo & ~1;
        }
    }
    যদি (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        যদি (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } অন্যথা {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        ধ্রুব LSR &likely = lsrs[index];
        যদি (uprv_strcmp(lsr.region, likely.region) == 0) {
            প্রত্যাগম likelyInfo | 1;
        } অন্যথা {
            প্রত্যাগম likelyInfo & ~1;
        }
    }
    প্রত্যাগম likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(ধ্রুব বর্ণ *language, ধ্রুব বর্ণ *script) ধ্রুব {
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
        value = trieNext(iter.resetToState64(state), language, 1);
    } অন্যথা {
        value = trieNext(iter, language, 0);
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
        value = trieNext(iter, script, 0);
        যদি (value >= 0) {
            state = iter.getState64();
        } অন্যথা {
            যদি (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } অন্যথা {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    যদি (value > 0) {
        // Final value from just language or language+script.
    } অন্যথা {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    প্রত্যাগম value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, ধ্রুব বর্ণ *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    যদি ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } অন্যথা {
        প্রতি (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            যদি (next != 0) {
                যদি (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    প্রত্যাগম -1;
                }
            } অন্যথা {
                // last character of this subtag
                result = iter.next(c | 0x80);
                ভঙ্গ;
            }
            c = next;
        }
    }
    বিকল্প (result) {
    পক্ষ USTRINGTRIE_NO_MATCH: প্রত্যাগম -1;
    পক্ষ USTRINGTRIE_NO_মান: প্রত্যাগম 0;
    পক্ষ USTRINGTRIE_INTERMEDIATE_মান:
        U_ASSERT(iter.get_মান() == SKIP_SCRIPT);
        প্রত্যাগম SKIP_SCRIPT;
    পক্ষ USTRINGTRIE_FINAL_মান: প্রত্যাগম iter.get_মান();
    স্বভাব: প্রত্যাগম -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    যদি (s.length() == i) {
        result = iter.next(u'*');
    } অন্যথা {
        c = s.data()[i];
        প্রতি (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            যদি (i+1 != s.length()) {
                যদি (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    প্রত্যাগম -1;
                }
                c = s.data()[++i];
            } অন্যথা {
                // last character of this subtag
                result = iter.next(c | 0x80);
                ভঙ্গ;
            }
        }
    }
    বিকল্প (result) {
    পক্ষ USTRINGTRIE_NO_MATCH: প্রত্যাগম -1;
    পক্ষ USTRINGTRIE_NO_মান: প্রত্যাগম 0;
    পক্ষ USTRINGTRIE_INTERMEDIATE_মান:
        U_ASSERT(iter.get_মান() == SKIP_SCRIPT);
        প্রত্যাগম SKIP_SCRIPT;
    পক্ষ USTRINGTRIE_FINAL_মান: প্রত্যাগম iter.get_মান();
    স্বভাব: প্রত্যাগম -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    তার্কিক favorScript,
                                    UErrorCode &errorCode) ধ্রুব {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম {}; }
    LSR max = maximize(language, script, region, সত্য, errorCode);
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম {}; }
    // If no match, return it.
    যদি (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        প্রত্যাগম LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", সত্য, errorCode);
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম {}; }
    যদি (test.isEquivalentTo(max)) {
        প্রত্যাগম LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    যদি (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, সত্য, errorCode);
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম {}; }
        যদি (test.isEquivalentTo(max)) {
            প্রত্যাগম LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", সত্য, errorCode);
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম {}; }
    যদি (test.isEquivalentTo(max)) {
        প্রত্যাগম LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    যদি (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, সত্য, errorCode);
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম {}; }
        যদি (test.isEquivalentTo(max)) {
            প্রত্যাগম LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    প্রত্যাগম LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
