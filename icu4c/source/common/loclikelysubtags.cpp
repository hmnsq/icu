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

constexpr वर्ण PSEUDO_ACCENTS_PREFIX = '\'';  // -XA, -PSACCENT
constexpr वर्ण PSEUDO_BIDI_PREFIX = '+';  // -XB, -PSBIDI
constexpr वर्ण PSEUDO_CRACKED_PREFIX = ',';  // -XC, -PSCRACK

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
    ध्रुव uint8_t *trieBytes = nullptr;
    LSR *lsrs = nullptr;
    int32_t lsrsLength = 0;

    LocaleDistanceData distanceData;

    LikelySubtagsData(UErrorCode &errorCode) : strings(errorCode) {}

    ~LikelySubtagsData() {
        ures_close(langInfoBundle);
        delete[] lsrs;
    }

    शून्य load(UErrorCode &errorCode) {
        जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
        राशिः_UResource_Bundle राशिः_Temp_Bundle;
        Resource_Data_मानम् value;
        ures_get_मानम्_With_Fallback(langInfoBundle, "likely", राशिः_Temp_Bundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        जइ (U_FAILURE(errorCode)) { प्रत्यागम; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        जइ (likelyTable.find_मानम्("m49", value)) {
            m49Array = value.getArray(errorCode);
        } अण्णहा {
            errorCode = U_MISSING_RESOURCE_ERROR;
            प्रत्यागम;
        }
        जइ (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            प्रत्यागम;
        }
        जइ ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            प्रत्यागम;
        }
        जइ (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            प्रत्यागम;
        }

        जइ (!likelyTable.find_मानम्("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            प्रत्यागम;
        }
        int32_t length;
        trieBytes = value.get_द्विमान(length, errorCode);
        जइ (U_FAILURE(errorCode)) { प्रत्यागम; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_get_मानम्_With_Fallback(langInfoBundle, "match", राशिः_Temp_Bundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        जइ (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            जइ (U_FAILURE(errorCode)) { प्रत्यागम; }

            जइ (matchTable.find_मानम्("trie", value)) {
                distanceData.distanceTrieBytes = value.get_द्विमान(length, errorCode);
                जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
            }

            जइ (matchTable.find_मानम्("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.get_द्विमान(length, errorCode);
                जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
                जइ (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    प्रत्यागम;
                }
            }

            जइ (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                प्रत्यागम;
            }
            जइ ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                प्रत्यागम;
            }

            जइ (matchTable.find_मानम्("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
                जइ (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    प्रत्यागम;
                }
            }
        } अण्णहा जइ (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } अण्णहा {  // error other than missing resource
            errorCode = matchErrorCode;
            प्रत्यागम;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languagesLength / 2, errorCode);
        प्रति (int32_t i = 0; i < languagesLength; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regionsLength / 2, errorCode);
        प्रति (int32_t i = 0; i < regionsLength; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        जइ (U_FAILURE(errorCode)) { प्रत्यागम; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = नूतन LSR[lsrsLength];
        जइ (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम;
        }
        प्रति (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        जइ (partitionsLength > 0) {
            distanceData.partitions = static_cast<ध्रुव वर्ण **>(
                uprv_malloc(partitionsLength * परिमाण(ध्रुव वर्ण *)));
            जइ (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम;
            }
            प्रति (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        जइ (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = नूतन LSR[distanceData.paradigmsLength];
            जइ (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम;
            }
            प्रति (int32_t i = 0, j = 0; i < paradigmSubtagsLength; i += 3, ++j) {
                paradigms[j] = LSR(strings.get(paradigmSubtagIndexes[i]),
                                   strings.get(paradigmSubtagIndexes[i + 1]),
                                   strings.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            distanceData.paradigms = paradigms;
        }
    }

गुप्त:
    तार्किक readStrings(ध्रुव ResourceTable &table, ध्रुव वर्ण *कुञ्चिका, Resource_मानम् &value,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        जइ (U_FAILURE(errorCode)) { प्रत्यागम मिच्छ; }
        जइ (table.find_मानम्(कुञ्चिका, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            जइ (U_FAILURE(errorCode)) { प्रत्यागम मिच्छ; }
            length = stringArray.getSize();
            जइ (length == 0) { प्रत्यागम सच्च; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            जइ (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम मिच्छ;
            }
            प्रति (पूर्णाङ्क i = 0; i < length; ++i) {
                जइ (stringArray.get_मानम्(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    जइ (U_FAILURE(errorCode)) { प्रत्यागम मिच्छ; }
                }
            }
        }
        प्रत्यागम सच्च;
    }
    UnicodeString toLanguage(पूर्णाङ्क encoded) {
        जइ (encoded == 0) {
            प्रत्यागम UNICODE_STRING_SIMPLE("");
        }
        जइ (encoded == 1) {
            प्रत्यागम UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        वर्ण lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        जइ (encoded / (27 * 27) == 0) {
            प्रत्यागम UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        प्रत्यागम UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(पूर्णाङ्क encoded) {
        जइ (encoded == 0) {
            प्रत्यागम UNICODE_STRING_SIMPLE("");
        }
        जइ (encoded == 1) {
            प्रत्यागम UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        ध्रुव वर्ण* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        जइ (script == nullptr) {
            प्रत्यागम UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        प्रत्यागम UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(ध्रुव ResourceArray &m49Array, Resource_मानम् &value, पूर्णाङ्क index, UErrorCode &errorCode) {
        जइ (U_FAILURE(errorCode)) {
            प्रत्यागम UNICODE_STRING_SIMPLE("");
        }
        जइ (m49Array.get_मानम्(index, value)) {
            प्रत्यागम value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        प्रत्यागम UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(ध्रुव ResourceArray& m49Array, Resource_मानम् &value, पूर्णाङ्क encoded, UErrorCode &errorCode) {
        जइ (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            प्रत्यागम UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        जइ (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            प्रत्यागम  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        वर्ण region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        प्रत्यागम UnicodeString(region, 2, US_INV);
    }

    तार्किक readLSREncodedStrings(ध्रुव ResourceTable &table, ध्रुव वर्ण* कुञ्चिका, Resource_मानम् &value, ध्रुव ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        जइ (U_FAILURE(errorCode)) { प्रत्यागम मिच्छ; }
        जइ (table.find_मानम्(कुञ्चिका, value)) {
            ध्रुव int32_t* vectors = value.getIntVector(length, errorCode);
            जइ (U_FAILURE(errorCode)) { प्रत्यागम मिच्छ; }
            जइ (length == 0) { प्रत्यागम सच्च; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            जइ (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम मिच्छ;
            }
            प्रति (पूर्णाङ्क i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.add_By_मानम्(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.add_By_मानम्(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.add_By_मानम्(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                जइ (U_FAILURE(errorCode)) { प्रत्यागम मिच्छ; }
            }
            length *= 3;
        }
        प्रत्यागम सच्च;
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
    प्रत्यागम सच्च;
}

constexpr ध्रुव char16_t* MACROREGION_HARDCODE[] = {
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
शून्य processMacroregionRange(ध्रुव UnicodeString& regionName, UVector* newMacroRegions, UErrorCode& status) {
    जइ (U_FAILURE(status)) { प्रत्यागम; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    जइ ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        जाव ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(नूतन UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } अण्णहा {
        LocalPointer<UnicodeString> newRegion(नूतन UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#जइ U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    जइ (U_FAILURE(status)) { प्रत्यागम nullptr; }
    LocalPointer<UVector> newMacroRegions(नूतन UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_get_By_कुञ्चिका(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_get_By_कुञ्चिका(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_get_By_कुञ्चिका(regionList.getAlias(),"macroregion",nullptr,&status));

    जइ (U_FAILURE(status)) {
        प्रत्यागम nullptr;
    }

    जाव (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        जइ (U_FAILURE(status)) {
            प्रत्यागम nullptr;
        }
    }

    प्रत्यागम newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    जइ (U_FAILURE(status)) { प्रत्यागम nullptr; }
    LocalPointer<UVector> newMacroRegions(नूतन UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    जइ (U_FAILURE(status)) {
        प्रत्यागम nullptr;
    }

    प्रति (ध्रुव auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        जइ (U_FAILURE(status)) {
            प्रत्यागम nullptr;
        }
    }

    प्रत्यागम newMacroRegions.orphan();
}

}  // namespace

शून्य U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
    gLikelySubtags = नूतन LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#जइ U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    जइ (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ध्रुव LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    प्रत्यागम gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#जइ U_DEBUG
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
    U_ASSERT(USTRINGTRIE_HAS_मानम्(result));
    defaultLsrIndex = trie.get_मानम्();
    trie.reset();

    प्रति (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        जइ (result == USTRINGTRIE_NO_मानम्) {
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

LSR LikelySubtags::makeMaximizedLsrFrom(ध्रुव Locale &locale,
                                         तार्किक return_निवेशः_If_Unmatch,
                                         UErrorCode &errorCode) ध्रुव {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम {}; }
    जइ (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम {};
    }
    ध्रुव वर्ण *name = locale.getName();
    जइ (!return_निवेशः_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        प्रत्यागम LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_निवेशः_If_Unmatch, errorCode);

    जइ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        प्रत्यागम LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    प्रत्यागम max;
}

नामाकाश {

ध्रुव वर्ण *getCanonical(ध्रुव CharStringMap &aliases, ध्रुव वर्ण *alias) {
    ध्रुव वर्ण *canonical = aliases.get(alias);
    प्रत्यागम canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(ध्रुव वर्ण *language, ध्रुव वर्ण *script, ध्रुव वर्ण *region,
                                     ध्रुव वर्ण *variant,
                                     तार्किक return_निवेशः_If_Unmatch,
                                     UErrorCode &errorCode) ध्रुव {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    जइ (!return_निवेशः_If_Unmatch) {
        वर्ण c1;
        जइ (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            विकल्प (c1) {
            पक्ष 'A':
                प्रत्यागम LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            पक्ष 'B':
                प्रत्यागम LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            पक्ष 'C':
                प्रत्यागम LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            स्वभाव:  // normal locale
                भङ्ग;
            }
        }

        जइ (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            जइ (uprv_strcmp(variant, "PSACCENT") == 0) {
                प्रत्यागम LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } अण्णहा जइ (uprv_strcmp(variant, "PSBIDI") == 0) {
                प्रत्यागम LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } अण्णहा जइ (uprv_strcmp(variant, "PSCRACK") == 0) {
                प्रत्यागम LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    प्रत्यागम maximize(language, script, region, return_निवेशः_If_Unmatch, errorCode);
}

LSR LikelySubtags::maximize(ध्रुव वर्ण *language, ध्रुव वर्ण *script, ध्रुव वर्ण *region,
                             तार्किक return_निवेशः_If_Unmatch,
                             UErrorCode &errorCode) ध्रुव {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम {}; }
    प्रत्यागम maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_निवेशः_If_Unmatch,
                    errorCode);
}

तार्किक LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) ध्रुव {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम मिच्छ; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    जइ (U_FAILURE(errorCode)) { प्रत्यागम मिच्छ; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    प्रत्यागम gMacroregions->contains((शून्य *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             तार्किक return_निवेशः_If_Unmatch,
                             UErrorCode &errorCode) ध्रुव {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम {}; }
    जइ (language.compare("und") == 0) {
        language = "";
    }
    जइ (script.compare("Zzzz") == 0) {
        script = "";
    }
    जइ (region.compare("ZZ") == 0) {
        region = "";
    }
    जइ (!script.empty() && !region.empty() && !language.empty()) {
        प्रत्यागम LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    तार्किक retainLanguage = मिच्छ;
    तार्किक retainScript = मिच्छ;
    तार्किक retainRegion = मिच्छ;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    जइ (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } अण्णहा {
        value = trieNext(iter, language, 0);
    }
    तार्किक matchLanguage = (value >= 0);
    तार्किक matchScript = मिच्छ;
    जइ (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } अण्णहा {
        retainLanguage = सच्च;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    जइ (value >= 0 && !script.empty()) {
        matchScript = सच्च;
    }
    जइ (value > 0) {
        // Intermediate or final value from just language.
        जइ (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } अण्णहा {
        value = trieNext(iter, script, 0);
        जइ (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } अण्णहा {
            retainScript = सच्च;
            जइ (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } अण्णहा {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    तार्किक matchRegion = मिच्छ;
    जइ (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } अण्णहा {
        value = trieNext(iter, region, 0);
        जइ (value >= 0) {
            जइ (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = सच्च;
                matchRegion = सच्च;
            }
        } अण्णहा {
            retainRegion = सच्च;
            जइ (state == 0) {
                value = defaultLsrIndex;
            } अण्णहा {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                जइ (value < 0) {
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
                    जइ (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    जइ (return_निवेशः_If_Unmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      प्रत्यागम LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    जइ (language.empty()) {
        language = StringPiece("und");
    }

    जइ (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        ध्रुव LSR &matched = lsrs[value];
        प्रत्यागम LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    जइ (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    जइ (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    जइ (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    प्रत्यागम LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(ध्रुव LSR &lsr, ध्रुव LSR &other, int32_t likelyInfo) ध्रुव {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    जइ (uprv_strcmp(lsr.language, other.language) != 0) {
        प्रत्यागम 0xfffffffc;  // negative, lsr not better than other
    }
    जइ (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        जइ (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } अण्णहा {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        ध्रुव LSR &likely = lsrs[index];
        जइ (uprv_strcmp(lsr.script, likely.script) == 0) {
            प्रत्यागम likelyInfo | 1;
        } अण्णहा {
            प्रत्यागम likelyInfo & ~1;
        }
    }
    जइ (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        जइ (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } अण्णहा {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        ध्रुव LSR &likely = lsrs[index];
        जइ (uprv_strcmp(lsr.region, likely.region) == 0) {
            प्रत्यागम likelyInfo | 1;
        } अण्णहा {
            प्रत्यागम likelyInfo & ~1;
        }
    }
    प्रत्यागम likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(ध्रुव वर्ण *language, ध्रुव वर्ण *script) ध्रुव {
    जइ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    जइ (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    जइ (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } अण्णहा {
        value = trieNext(iter, language, 0);
    }
    जइ (value >= 0) {
        state = iter.getState64();
    } अण्णहा {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    जइ (value > 0) {
        // Intermediate or final value from just language.
        जइ (value == SKIP_SCRIPT) {
            value = 0;
        }
    } अण्णहा {
        value = trieNext(iter, script, 0);
        जइ (value >= 0) {
            state = iter.getState64();
        } अण्णहा {
            जइ (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } अण्णहा {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    जइ (value > 0) {
        // Final value from just language or language+script.
    } अण्णहा {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    प्रत्यागम value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, ध्रुव वर्ण *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    जइ ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } अण्णहा {
        प्रति (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            जइ (next != 0) {
                जइ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    प्रत्यागम -1;
                }
            } अण्णहा {
                // last character of this subtag
                result = iter.next(c | 0x80);
                भङ्ग;
            }
            c = next;
        }
    }
    विकल्प (result) {
    पक्ष USTRINGTRIE_NO_MATCH: प्रत्यागम -1;
    पक्ष USTRINGTRIE_NO_मानम्: प्रत्यागम 0;
    पक्ष USTRINGTRIE_INTERMEDIATE_मानम्:
        U_ASSERT(iter.get_मानम्() == SKIP_SCRIPT);
        प्रत्यागम SKIP_SCRIPT;
    पक्ष USTRINGTRIE_FINAL_मानम्: प्रत्यागम iter.get_मानम्();
    स्वभाव: प्रत्यागम -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    जइ (s.length() == i) {
        result = iter.next(u'*');
    } अण्णहा {
        c = s.data()[i];
        प्रति (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            जइ (i+1 != s.length()) {
                जइ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    प्रत्यागम -1;
                }
                c = s.data()[++i];
            } अण्णहा {
                // last character of this subtag
                result = iter.next(c | 0x80);
                भङ्ग;
            }
        }
    }
    विकल्प (result) {
    पक्ष USTRINGTRIE_NO_MATCH: प्रत्यागम -1;
    पक्ष USTRINGTRIE_NO_मानम्: प्रत्यागम 0;
    पक्ष USTRINGTRIE_INTERMEDIATE_मानम्:
        U_ASSERT(iter.get_मानम्() == SKIP_SCRIPT);
        प्रत्यागम SKIP_SCRIPT;
    पक्ष USTRINGTRIE_FINAL_मानम्: प्रत्यागम iter.get_मानम्();
    स्वभाव: प्रत्यागम -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    तार्किक favorScript,
                                    UErrorCode &errorCode) ध्रुव {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम {}; }
    LSR max = maximize(language, script, region, सच्च, errorCode);
    जइ (U_FAILURE(errorCode)) { प्रत्यागम {}; }
    // If no match, return it.
    जइ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        प्रत्यागम LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", सच्च, errorCode);
    जइ (U_FAILURE(errorCode)) { प्रत्यागम {}; }
    जइ (test.isEquivalentTo(max)) {
        प्रत्यागम LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    जइ (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, सच्च, errorCode);
        जइ (U_FAILURE(errorCode)) { प्रत्यागम {}; }
        जइ (test.isEquivalentTo(max)) {
            प्रत्यागम LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", सच्च, errorCode);
    जइ (U_FAILURE(errorCode)) { प्रत्यागम {}; }
    जइ (test.isEquivalentTo(max)) {
        प्रत्यागम LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    जइ (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, सच्च, errorCode);
        जइ (U_FAILURE(errorCode)) { प्रत्यागम {}; }
        जइ (test.isEquivalentTo(max)) {
            प्रत्यागम LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    प्रत्यागम LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
