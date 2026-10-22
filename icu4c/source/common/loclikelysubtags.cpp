// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#अन्तर्भाव <utility>
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestrie.h"
#अन्तर्भाव "unicode/localpointer.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "unicode/uscript.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "loclikelysubtags.h"
#अन्तर्भाव "lsr.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "uinvchar.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "uniquecharstr.h"
#अन्तर्भाव "uresdata.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "uvector.h"

U_NAMESPACE_BEGIN

नामाकाश {

constexpr अक्षर PSEUDO_ACCENTS_PREFIX = '\'';  // -XA, -PSACCENT
constexpr अक्षर PSEUDO_BIDI_PREFIX = '+';  // -XB, -PSBIDI
constexpr अक्षर PSEUDO_CRACKED_PREFIX = ',';  // -XC, -PSCRACK

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

संरचना LikelySubtagsData {
    UResourceBundle *langInfoBundle = nullptr;
    UniqueCharStrings strings;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    स्थिरांक uint8_t *trieBytes = nullptr;
    LSR *lsrs = nullptr;
    int32_t lsrsLength = 0;

    LocaleDistanceData distanceData;

    LikelySubtagsData(UErrorCode &errorCode) : strings(errorCode) {}

    ~LikelySubtagsData() {
        ures_close(langInfoBundle);
        delete[] lsrs;
    }

    शून्य load(UErrorCode &errorCode) {
        जर (U_FAILURE(errorCode)) { परत; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        जर (U_FAILURE(errorCode)) { परत; }
        चळत_UResource_Bundle चळत_Temp_Bundle;
        Resource_Data_मूल्य value;
        ures_get_मूल्य_With_Fallback(langInfoBundle, "likely", चळत_Temp_Bundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        जर (U_FAILURE(errorCode)) { परत; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        जर (likelyTable.find_मूल्य("m49", value)) {
            m49Array = value.getArray(errorCode);
        } अन्यथा {
            errorCode = U_MISSING_RESOURCE_ERROR;
            परत;
        }
        जर (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            परत;
        }
        जर ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            परत;
        }
        जर (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            परत;
        }

        जर (!likelyTable.find_मूल्य("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            परत;
        }
        int32_t length;
        trieBytes = value.get_द्विमान(length, errorCode);
        जर (U_FAILURE(errorCode)) { परत; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_get_मूल्य_With_Fallback(langInfoBundle, "match", चळत_Temp_Bundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        जर (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            जर (U_FAILURE(errorCode)) { परत; }

            जर (matchTable.find_मूल्य("trie", value)) {
                distanceData.distanceTrieBytes = value.get_द्विमान(length, errorCode);
                जर (U_FAILURE(errorCode)) { परत; }
            }

            जर (matchTable.find_मूल्य("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.get_द्विमान(length, errorCode);
                जर (U_FAILURE(errorCode)) { परत; }
                जर (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    परत;
                }
            }

            जर (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                परत;
            }
            जर ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                परत;
            }

            जर (matchTable.find_मूल्य("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                जर (U_FAILURE(errorCode)) { परत; }
                जर (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    परत;
                }
            }
        } अन्यथा जर (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } अन्यथा {  // error other than missing resource
            errorCode = matchErrorCode;
            परत;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languagesLength / 2, errorCode);
        साठी (int32_t i = 0; i < languagesLength; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regionsLength / 2, errorCode);
        साठी (int32_t i = 0; i < regionsLength; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        जर (U_FAILURE(errorCode)) { परत; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = नवीन LSR[lsrsLength];
        जर (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            परत;
        }
        साठी (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        जर (partitionsLength > 0) {
            distanceData.partitions = static_cast<स्थिरांक अक्षर **>(
                uprv_malloc(partitionsLength * आकार(स्थिरांक अक्षर *)));
            जर (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                परत;
            }
            साठी (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        जर (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = नवीन LSR[distanceData.paradigmsLength];
            जर (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                परत;
            }
            साठी (int32_t i = 0, j = 0; i < paradigmSubtagsLength; i += 3, ++j) {
                paradigms[j] = LSR(strings.get(paradigmSubtagIndexes[i]),
                                   strings.get(paradigmSubtagIndexes[i + 1]),
                                   strings.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            distanceData.paradigms = paradigms;
        }
    }

खाजगी:
    तार्किक readStrings(स्थिरांक ResourceTable &table, स्थिरांक अक्षर *किल्ली, Resource_मूल्य &value,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        जर (U_FAILURE(errorCode)) { परत असत्य; }
        जर (table.find_मूल्य(किल्ली, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            जर (U_FAILURE(errorCode)) { परत असत्य; }
            length = stringArray.getSize();
            जर (length == 0) { परत सत्य; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            जर (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                परत असत्य;
            }
            साठी (पूर्णांक i = 0; i < length; ++i) {
                जर (stringArray.get_मूल्य(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    जर (U_FAILURE(errorCode)) { परत असत्य; }
                }
            }
        }
        परत सत्य;
    }
    UnicodeString toLanguage(पूर्णांक encoded) {
        जर (encoded == 0) {
            परत UNICODE_STRING_SIMPLE("");
        }
        जर (encoded == 1) {
            परत UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        अक्षर lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        जर (encoded / (27 * 27) == 0) {
            परत UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        परत UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(पूर्णांक encoded) {
        जर (encoded == 0) {
            परत UNICODE_STRING_SIMPLE("");
        }
        जर (encoded == 1) {
            परत UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        स्थिरांक अक्षर* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        जर (script == nullptr) {
            परत UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        परत UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(स्थिरांक ResourceArray &m49Array, Resource_मूल्य &value, पूर्णांक index, UErrorCode &errorCode) {
        जर (U_FAILURE(errorCode)) {
            परत UNICODE_STRING_SIMPLE("");
        }
        जर (m49Array.get_मूल्य(index, value)) {
            परत value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        परत UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(स्थिरांक ResourceArray& m49Array, Resource_मूल्य &value, पूर्णांक encoded, UErrorCode &errorCode) {
        जर (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            परत UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        जर (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            परत  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        अक्षर region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        परत UnicodeString(region, 2, US_INV);
    }

    तार्किक readLSREncodedStrings(स्थिरांक ResourceTable &table, स्थिरांक अक्षर* किल्ली, Resource_मूल्य &value, स्थिरांक ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        जर (U_FAILURE(errorCode)) { परत असत्य; }
        जर (table.find_मूल्य(किल्ली, value)) {
            स्थिरांक int32_t* vectors = value.getIntVector(length, errorCode);
            जर (U_FAILURE(errorCode)) { परत असत्य; }
            जर (length == 0) { परत सत्य; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            जर (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                परत असत्य;
            }
            साठी (पूर्णांक i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.add_By_मूल्य(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.add_By_मूल्य(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.add_By_मूल्य(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                जर (U_FAILURE(errorCode)) { परत असत्य; }
            }
            length *= 3;
        }
        परत सत्य;
    }
};

नामाकाश {

LikelySubtags *gLikelySubtags = nullptr;
UVector *gMacroregions = nullptr;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    delete gLikelySubtags;
    gLikelySubtags = nullptr;
    delete gMacroregions;
    gMacroregions = nullptr;
    gInitOnce.reset();
    परत सत्य;
}

constexpr स्थिरांक char16_t* MACROREGION_HARDCODE[] = {
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
शून्य processMacroregionRange(स्थिरांक UnicodeString& regionName, UVector* newMacroRegions, UErrorCode& status) {
    जर (U_FAILURE(status)) { परत; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    जर ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        पर्यंत ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(नवीन UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } अन्यथा {
        LocalPointer<UnicodeString> newRegion(नवीन UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#जर U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    जर (U_FAILURE(status)) { परत nullptr; }
    LocalPointer<UVector> newMacroRegions(नवीन UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_get_By_किल्ली(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_get_By_किल्ली(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_get_By_किल्ली(regionList.getAlias(),"macroregion",nullptr,&status));

    जर (U_FAILURE(status)) {
        परत nullptr;
    }

    पर्यंत (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        जर (U_FAILURE(status)) {
            परत nullptr;
        }
    }

    परत newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    जर (U_FAILURE(status)) { परत nullptr; }
    LocalPointer<UVector> newMacroRegions(नवीन UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    जर (U_FAILURE(status)) {
        परत nullptr;
    }

    साठी (स्थिरांक स्वतः *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        जर (U_FAILURE(status)) {
            परत nullptr;
        }
    }

    परत newMacroRegions.orphan();
}

}  // namespace

शून्य U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    जर (U_FAILURE(errorCode)) { परत; }
    gLikelySubtags = नवीन LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#जर U_DEBUG
    स्वतः macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    जर (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

स्थिरांक LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) { परत nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    परत gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#जर U_DEBUG
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
    U_ASSERT(USTRINGTRIE_HAS_मूल्य(result));
    defaultLsrIndex = trie.get_मूल्य();
    trie.reset();

    साठी (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        जर (result == USTRINGTRIE_NO_मूल्य) {
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

LSR LikelySubtags::makeMaximizedLsrFrom(स्थिरांक Locale &locale,
                                         तार्किक return_आदान_If_Unmatch,
                                         UErrorCode &errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) { परत {}; }
    जर (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत {};
    }
    स्थिरांक अक्षर *name = locale.getName();
    जर (!return_आदान_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        परत LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_आदान_If_Unmatch, errorCode);

    जर (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        परत LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    परत max;
}

नामाकाश {

स्थिरांक अक्षर *getCanonical(स्थिरांक CharStringMap &aliases, स्थिरांक अक्षर *alias) {
    स्थिरांक अक्षर *canonical = aliases.get(alias);
    परत canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(स्थिरांक अक्षर *language, स्थिरांक अक्षर *script, स्थिरांक अक्षर *region,
                                     स्थिरांक अक्षर *variant,
                                     तार्किक return_आदान_If_Unmatch,
                                     UErrorCode &errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) { परत {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    जर (!return_आदान_If_Unmatch) {
        अक्षर c1;
        जर (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            निवड (c1) {
            स्थिती 'A':
                परत LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            स्थिती 'B':
                परत LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            स्थिती 'C':
                परत LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            पूर्वनिर्धारित:  // normal locale
                थांबा;
            }
        }

        जर (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            जर (uprv_strcmp(variant, "PSACCENT") == 0) {
                परत LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } अन्यथा जर (uprv_strcmp(variant, "PSBIDI") == 0) {
                परत LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } अन्यथा जर (uprv_strcmp(variant, "PSCRACK") == 0) {
                परत LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    परत maximize(language, script, region, return_आदान_If_Unmatch, errorCode);
}

LSR LikelySubtags::maximize(स्थिरांक अक्षर *language, स्थिरांक अक्षर *script, स्थिरांक अक्षर *region,
                             तार्किक return_आदान_If_Unmatch,
                             UErrorCode &errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) { परत {}; }
    परत maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_आदान_If_Unmatch,
                    errorCode);
}

तार्किक LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) { परत असत्य; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    जर (U_FAILURE(errorCode)) { परत असत्य; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    परत gMacroregions->contains((शून्य *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             तार्किक return_आदान_If_Unmatch,
                             UErrorCode &errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) { परत {}; }
    जर (language.compare("und") == 0) {
        language = "";
    }
    जर (script.compare("Zzzz") == 0) {
        script = "";
    }
    जर (region.compare("ZZ") == 0) {
        region = "";
    }
    जर (!script.empty() && !region.empty() && !language.empty()) {
        परत LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    तार्किक retainLanguage = असत्य;
    तार्किक retainScript = असत्य;
    तार्किक retainRegion = असत्य;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    जर (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } अन्यथा {
        value = trieNext(iter, language, 0);
    }
    तार्किक matchLanguage = (value >= 0);
    तार्किक matchScript = असत्य;
    जर (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } अन्यथा {
        retainLanguage = सत्य;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    जर (value >= 0 && !script.empty()) {
        matchScript = सत्य;
    }
    जर (value > 0) {
        // Intermediate or final value from just language.
        जर (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } अन्यथा {
        value = trieNext(iter, script, 0);
        जर (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } अन्यथा {
            retainScript = सत्य;
            जर (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } अन्यथा {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    तार्किक matchRegion = असत्य;
    जर (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } अन्यथा {
        value = trieNext(iter, region, 0);
        जर (value >= 0) {
            जर (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = सत्य;
                matchRegion = सत्य;
            }
        } अन्यथा {
            retainRegion = सत्य;
            जर (state == 0) {
                value = defaultLsrIndex;
            } अन्यथा {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                जर (value < 0) {
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
                    जर (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    जर (return_आदान_If_Unmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      परत LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    जर (language.empty()) {
        language = StringPiece("und");
    }

    जर (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        स्थिरांक LSR &matched = lsrs[value];
        परत LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    जर (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    जर (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    जर (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    परत LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(स्थिरांक LSR &lsr, स्थिरांक LSR &other, int32_t likelyInfo) स्थिरांक {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    जर (uprv_strcmp(lsr.language, other.language) != 0) {
        परत 0xfffffffc;  // negative, lsr not better than other
    }
    जर (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        जर (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } अन्यथा {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        स्थिरांक LSR &likely = lsrs[index];
        जर (uprv_strcmp(lsr.script, likely.script) == 0) {
            परत likelyInfo | 1;
        } अन्यथा {
            परत likelyInfo & ~1;
        }
    }
    जर (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        जर (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } अन्यथा {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        स्थिरांक LSR &likely = lsrs[index];
        जर (uprv_strcmp(lsr.region, likely.region) == 0) {
            परत likelyInfo | 1;
        } अन्यथा {
            परत likelyInfo & ~1;
        }
    }
    परत likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(स्थिरांक अक्षर *language, स्थिरांक अक्षर *script) स्थिरांक {
    जर (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    जर (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    जर (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } अन्यथा {
        value = trieNext(iter, language, 0);
    }
    जर (value >= 0) {
        state = iter.getState64();
    } अन्यथा {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    जर (value > 0) {
        // Intermediate or final value from just language.
        जर (value == SKIP_SCRIPT) {
            value = 0;
        }
    } अन्यथा {
        value = trieNext(iter, script, 0);
        जर (value >= 0) {
            state = iter.getState64();
        } अन्यथा {
            जर (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } अन्यथा {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    जर (value > 0) {
        // Final value from just language or language+script.
    } अन्यथा {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    परत value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, स्थिरांक अक्षर *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    जर ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } अन्यथा {
        साठी (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            जर (next != 0) {
                जर (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    परत -1;
                }
            } अन्यथा {
                // last character of this subtag
                result = iter.next(c | 0x80);
                थांबा;
            }
            c = next;
        }
    }
    निवड (result) {
    स्थिती USTRINGTRIE_NO_MATCH: परत -1;
    स्थिती USTRINGTRIE_NO_मूल्य: परत 0;
    स्थिती USTRINGTRIE_INTERMEDIATE_मूल्य:
        U_ASSERT(iter.get_मूल्य() == SKIP_SCRIPT);
        परत SKIP_SCRIPT;
    स्थिती USTRINGTRIE_FINAL_मूल्य: परत iter.get_मूल्य();
    पूर्वनिर्धारित: परत -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    जर (s.length() == i) {
        result = iter.next(u'*');
    } अन्यथा {
        c = s.data()[i];
        साठी (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            जर (i+1 != s.length()) {
                जर (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    परत -1;
                }
                c = s.data()[++i];
            } अन्यथा {
                // last character of this subtag
                result = iter.next(c | 0x80);
                थांबा;
            }
        }
    }
    निवड (result) {
    स्थिती USTRINGTRIE_NO_MATCH: परत -1;
    स्थिती USTRINGTRIE_NO_मूल्य: परत 0;
    स्थिती USTRINGTRIE_INTERMEDIATE_मूल्य:
        U_ASSERT(iter.get_मूल्य() == SKIP_SCRIPT);
        परत SKIP_SCRIPT;
    स्थिती USTRINGTRIE_FINAL_मूल्य: परत iter.get_मूल्य();
    पूर्वनिर्धारित: परत -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    तार्किक favorScript,
                                    UErrorCode &errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) { परत {}; }
    LSR max = maximize(language, script, region, सत्य, errorCode);
    जर (U_FAILURE(errorCode)) { परत {}; }
    // If no match, return it.
    जर (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        परत LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", सत्य, errorCode);
    जर (U_FAILURE(errorCode)) { परत {}; }
    जर (test.isEquivalentTo(max)) {
        परत LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    जर (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, सत्य, errorCode);
        जर (U_FAILURE(errorCode)) { परत {}; }
        जर (test.isEquivalentTo(max)) {
            परत LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", सत्य, errorCode);
    जर (U_FAILURE(errorCode)) { परत {}; }
    जर (test.isEquivalentTo(max)) {
        परत LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    जर (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, सत्य, errorCode);
        जर (U_FAILURE(errorCode)) { परत {}; }
        जर (test.isEquivalentTo(max)) {
            परत LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    परत LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
