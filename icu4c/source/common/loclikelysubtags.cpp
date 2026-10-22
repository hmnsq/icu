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
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
        राशिः_UResource_Bundle राशिः_Temp_Bundle;
        Resource_Data_मानम् value;
        ures_get_मानम्_With_Fallback(langInfoBundle, "likely", राशिः_Temp_Bundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        सचे (likelyTable.find_मानम्("m49", value)) {
            m49Array = value.getArray(errorCode);
        } अञ्ञथा {
            errorCode = U_MISSING_RESOURCE_ERROR;
            पटिनिवत्त;
        }
        सचे (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            पटिनिवत्त;
        }
        सचे ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            पटिनिवत्त;
        }
        सचे (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            पटिनिवत्त;
        }

        सचे (!likelyTable.find_मानम्("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            पटिनिवत्त;
        }
        int32_t length;
        trieBytes = value.get_द्विमान(length, errorCode);
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_get_मानम्_With_Fallback(langInfoBundle, "match", राशिः_Temp_Bundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        सचे (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }

            सचे (matchTable.find_मानम्("trie", value)) {
                distanceData.distanceTrieBytes = value.get_द्विमान(length, errorCode);
                सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
            }

            सचे (matchTable.find_मानम्("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.get_द्विमान(length, errorCode);
                सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
                सचे (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    पटिनिवत्त;
                }
            }

            सचे (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                पटिनिवत्त;
            }
            सचे ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                पटिनिवत्त;
            }

            सचे (matchTable.find_मानम्("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
                सचे (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    पटिनिवत्त;
                }
            }
        } अञ्ञथा सचे (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } अञ्ञथा {  // error other than missing resource
            errorCode = matchErrorCode;
            पटिनिवत्त;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languagesLength / 2, errorCode);
        अत्थाय (int32_t i = 0; i < languagesLength; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regionsLength / 2, errorCode);
        अत्थाय (int32_t i = 0; i < regionsLength; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = नूतन LSR[lsrsLength];
        सचे (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त;
        }
        अत्थाय (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        सचे (partitionsLength > 0) {
            distanceData.partitions = static_cast<ध्रुव वर्ण **>(
                uprv_malloc(partitionsLength * परिमाण(ध्रुव वर्ण *)));
            सचे (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त;
            }
            अत्थाय (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        सचे (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = नूतन LSR[distanceData.paradigmsLength];
            सचे (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त;
            }
            अत्थाय (int32_t i = 0, j = 0; i < paradigmSubtagsLength; i += 3, ++j) {
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
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त मिच्छा; }
        सचे (table.find_मानम्(कुञ्चिका, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            सचे (U_FAILURE(errorCode)) { पटिनिवत्त मिच्छा; }
            length = stringArray.getSize();
            सचे (length == 0) { पटिनिवत्त सच्च; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            सचे (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त मिच्छा;
            }
            अत्थाय (पूर्णाङ्क i = 0; i < length; ++i) {
                सचे (stringArray.get_मानम्(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    सचे (U_FAILURE(errorCode)) { पटिनिवत्त मिच्छा; }
                }
            }
        }
        पटिनिवत्त सच्च;
    }
    UnicodeString toLanguage(पूर्णाङ्क encoded) {
        सचे (encoded == 0) {
            पटिनिवत्त UNICODE_STRING_SIMPLE("");
        }
        सचे (encoded == 1) {
            पटिनिवत्त UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        वर्ण lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        सचे (encoded / (27 * 27) == 0) {
            पटिनिवत्त UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        पटिनिवत्त UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(पूर्णाङ्क encoded) {
        सचे (encoded == 0) {
            पटिनिवत्त UNICODE_STRING_SIMPLE("");
        }
        सचे (encoded == 1) {
            पटिनिवत्त UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        ध्रुव वर्ण* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        सचे (script == nullptr) {
            पटिनिवत्त UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        पटिनिवत्त UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(ध्रुव ResourceArray &m49Array, Resource_मानम् &value, पूर्णाङ्क index, UErrorCode &errorCode) {
        सचे (U_FAILURE(errorCode)) {
            पटिनिवत्त UNICODE_STRING_SIMPLE("");
        }
        सचे (m49Array.get_मानम्(index, value)) {
            पटिनिवत्त value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        पटिनिवत्त UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(ध्रुव ResourceArray& m49Array, Resource_मानम् &value, पूर्णाङ्क encoded, UErrorCode &errorCode) {
        सचे (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            पटिनिवत्त UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        सचे (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            पटिनिवत्त  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        वर्ण region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        पटिनिवत्त UnicodeString(region, 2, US_INV);
    }

    तार्किक readLSREncodedStrings(ध्रुव ResourceTable &table, ध्रुव वर्ण* कुञ्चिका, Resource_मानम् &value, ध्रुव ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त मिच्छा; }
        सचे (table.find_मानम्(कुञ्चिका, value)) {
            ध्रुव int32_t* vectors = value.getIntVector(length, errorCode);
            सचे (U_FAILURE(errorCode)) { पटिनिवत्त मिच्छा; }
            सचे (length == 0) { पटिनिवत्त सच्च; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            सचे (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त मिच्छा;
            }
            अत्थाय (पूर्णाङ्क i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.add_By_मानम्(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.add_By_मानम्(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.add_By_मानम्(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                सचे (U_FAILURE(errorCode)) { पटिनिवत्त मिच्छा; }
            }
            length *= 3;
        }
        पटिनिवत्त सच्च;
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
    पटिनिवत्त सच्च;
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
    सचे (U_FAILURE(status)) { पटिनिवत्त; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    सचे ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        याव ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(नूतन UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } अञ्ञथा {
        LocalPointer<UnicodeString> newRegion(नूतन UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#सचे U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    सचे (U_FAILURE(status)) { पटिनिवत्त nullptr; }
    LocalPointer<UVector> newMacroRegions(नूतन UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_get_By_कुञ्चिका(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_get_By_कुञ्चिका(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_get_By_कुञ्चिका(regionList.getAlias(),"macroregion",nullptr,&status));

    सचे (U_FAILURE(status)) {
        पटिनिवत्त nullptr;
    }

    याव (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        सचे (U_FAILURE(status)) {
            पटिनिवत्त nullptr;
        }
    }

    पटिनिवत्त newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    सचे (U_FAILURE(status)) { पटिनिवत्त nullptr; }
    LocalPointer<UVector> newMacroRegions(नूतन UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    सचे (U_FAILURE(status)) {
        पटिनिवत्त nullptr;
    }

    अत्थाय (ध्रुव auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        सचे (U_FAILURE(status)) {
            पटिनिवत्त nullptr;
        }
    }

    पटिनिवत्त newMacroRegions.orphan();
}

}  // namespace

शून्य U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
    gLikelySubtags = नूतन LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#सचे U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    सचे (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ध्रुव LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    पटिनिवत्त gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#सचे U_DEBUG
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

    अत्थाय (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        सचे (result == USTRINGTRIE_NO_मानम्) {
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
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त {}; }
    सचे (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त {};
    }
    ध्रुव वर्ण *name = locale.getName();
    सचे (!return_निवेशः_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        पटिनिवत्त LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_निवेशः_If_Unmatch, errorCode);

    सचे (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        पटिनिवत्त LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    पटिनिवत्त max;
}

नामाकाश {

ध्रुव वर्ण *getCanonical(ध्रुव CharStringMap &aliases, ध्रुव वर्ण *alias) {
    ध्रुव वर्ण *canonical = aliases.get(alias);
    पटिनिवत्त canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(ध्रुव वर्ण *language, ध्रुव वर्ण *script, ध्रुव वर्ण *region,
                                     ध्रुव वर्ण *variant,
                                     तार्किक return_निवेशः_If_Unmatch,
                                     UErrorCode &errorCode) ध्रुव {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    सचे (!return_निवेशः_If_Unmatch) {
        वर्ण c1;
        सचे (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            विकल्प (c1) {
            पक्ष 'A':
                पटिनिवत्त LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            पक्ष 'B':
                पटिनिवत्त LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            पक्ष 'C':
                पटिनिवत्त LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            स्वभाव:  // normal locale
                भङ्ग;
            }
        }

        सचे (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            सचे (uprv_strcmp(variant, "PSACCENT") == 0) {
                पटिनिवत्त LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } अञ्ञथा सचे (uprv_strcmp(variant, "PSBIDI") == 0) {
                पटिनिवत्त LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } अञ्ञथा सचे (uprv_strcmp(variant, "PSCRACK") == 0) {
                पटिनिवत्त LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    पटिनिवत्त maximize(language, script, region, return_निवेशः_If_Unmatch, errorCode);
}

LSR LikelySubtags::maximize(ध्रुव वर्ण *language, ध्रुव वर्ण *script, ध्रुव वर्ण *region,
                             तार्किक return_निवेशः_If_Unmatch,
                             UErrorCode &errorCode) ध्रुव {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त {}; }
    पटिनिवत्त maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_निवेशः_If_Unmatch,
                    errorCode);
}

तार्किक LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) ध्रुव {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त मिच्छा; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त मिच्छा; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    पटिनिवत्त gMacroregions->contains((शून्य *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             तार्किक return_निवेशः_If_Unmatch,
                             UErrorCode &errorCode) ध्रुव {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त {}; }
    सचे (language.compare("und") == 0) {
        language = "";
    }
    सचे (script.compare("Zzzz") == 0) {
        script = "";
    }
    सचे (region.compare("ZZ") == 0) {
        region = "";
    }
    सचे (!script.empty() && !region.empty() && !language.empty()) {
        पटिनिवत्त LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    तार्किक retainLanguage = मिच्छा;
    तार्किक retainScript = मिच्छा;
    तार्किक retainRegion = मिच्छा;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    सचे (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } अञ्ञथा {
        value = trieNext(iter, language, 0);
    }
    तार्किक matchLanguage = (value >= 0);
    तार्किक matchScript = मिच्छा;
    सचे (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } अञ्ञथा {
        retainLanguage = सच्च;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    सचे (value >= 0 && !script.empty()) {
        matchScript = सच्च;
    }
    सचे (value > 0) {
        // Intermediate or final value from just language.
        सचे (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } अञ्ञथा {
        value = trieNext(iter, script, 0);
        सचे (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } अञ्ञथा {
            retainScript = सच्च;
            सचे (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } अञ्ञथा {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    तार्किक matchRegion = मिच्छा;
    सचे (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } अञ्ञथा {
        value = trieNext(iter, region, 0);
        सचे (value >= 0) {
            सचे (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = सच्च;
                matchRegion = सच्च;
            }
        } अञ्ञथा {
            retainRegion = सच्च;
            सचे (state == 0) {
                value = defaultLsrIndex;
            } अञ्ञथा {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                सचे (value < 0) {
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
                    सचे (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    सचे (return_निवेशः_If_Unmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      पटिनिवत्त LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    सचे (language.empty()) {
        language = StringPiece("und");
    }

    सचे (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        ध्रुव LSR &matched = lsrs[value];
        पटिनिवत्त LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    सचे (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    सचे (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    सचे (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    पटिनिवत्त LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(ध्रुव LSR &lsr, ध्रुव LSR &other, int32_t likelyInfo) ध्रुव {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    सचे (uprv_strcmp(lsr.language, other.language) != 0) {
        पटिनिवत्त 0xfffffffc;  // negative, lsr not better than other
    }
    सचे (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        सचे (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } अञ्ञथा {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        ध्रुव LSR &likely = lsrs[index];
        सचे (uprv_strcmp(lsr.script, likely.script) == 0) {
            पटिनिवत्त likelyInfo | 1;
        } अञ्ञथा {
            पटिनिवत्त likelyInfo & ~1;
        }
    }
    सचे (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        सचे (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } अञ्ञथा {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        ध्रुव LSR &likely = lsrs[index];
        सचे (uprv_strcmp(lsr.region, likely.region) == 0) {
            पटिनिवत्त likelyInfo | 1;
        } अञ्ञथा {
            पटिनिवत्त likelyInfo & ~1;
        }
    }
    पटिनिवत्त likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(ध्रुव वर्ण *language, ध्रुव वर्ण *script) ध्रुव {
    सचे (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    सचे (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    सचे (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } अञ्ञथा {
        value = trieNext(iter, language, 0);
    }
    सचे (value >= 0) {
        state = iter.getState64();
    } अञ्ञथा {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    सचे (value > 0) {
        // Intermediate or final value from just language.
        सचे (value == SKIP_SCRIPT) {
            value = 0;
        }
    } अञ्ञथा {
        value = trieNext(iter, script, 0);
        सचे (value >= 0) {
            state = iter.getState64();
        } अञ्ञथा {
            सचे (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } अञ्ञथा {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    सचे (value > 0) {
        // Final value from just language or language+script.
    } अञ्ञथा {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    पटिनिवत्त value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, ध्रुव वर्ण *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    सचे ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } अञ्ञथा {
        अत्थाय (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            सचे (next != 0) {
                सचे (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    पटिनिवत्त -1;
                }
            } अञ्ञथा {
                // last character of this subtag
                result = iter.next(c | 0x80);
                भङ्ग;
            }
            c = next;
        }
    }
    विकल्प (result) {
    पक्ष USTRINGTRIE_NO_MATCH: पटिनिवत्त -1;
    पक्ष USTRINGTRIE_NO_मानम्: पटिनिवत्त 0;
    पक्ष USTRINGTRIE_INTERMEDIATE_मानम्:
        U_ASSERT(iter.get_मानम्() == SKIP_SCRIPT);
        पटिनिवत्त SKIP_SCRIPT;
    पक्ष USTRINGTRIE_FINAL_मानम्: पटिनिवत्त iter.get_मानम्();
    स्वभाव: पटिनिवत्त -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    सचे (s.length() == i) {
        result = iter.next(u'*');
    } अञ्ञथा {
        c = s.data()[i];
        अत्थाय (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            सचे (i+1 != s.length()) {
                सचे (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    पटिनिवत्त -1;
                }
                c = s.data()[++i];
            } अञ्ञथा {
                // last character of this subtag
                result = iter.next(c | 0x80);
                भङ्ग;
            }
        }
    }
    विकल्प (result) {
    पक्ष USTRINGTRIE_NO_MATCH: पटिनिवत्त -1;
    पक्ष USTRINGTRIE_NO_मानम्: पटिनिवत्त 0;
    पक्ष USTRINGTRIE_INTERMEDIATE_मानम्:
        U_ASSERT(iter.get_मानम्() == SKIP_SCRIPT);
        पटिनिवत्त SKIP_SCRIPT;
    पक्ष USTRINGTRIE_FINAL_मानम्: पटिनिवत्त iter.get_मानम्();
    स्वभाव: पटिनिवत्त -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    तार्किक favorScript,
                                    UErrorCode &errorCode) ध्रुव {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त {}; }
    LSR max = maximize(language, script, region, सच्च, errorCode);
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त {}; }
    // If no match, return it.
    सचे (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        पटिनिवत्त LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", सच्च, errorCode);
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त {}; }
    सचे (test.isEquivalentTo(max)) {
        पटिनिवत्त LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    सचे (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, सच्च, errorCode);
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त {}; }
        सचे (test.isEquivalentTo(max)) {
            पटिनिवत्त LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", सच्च, errorCode);
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त {}; }
    सचे (test.isEquivalentTo(max)) {
        पटिनिवत्त LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    सचे (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, सच्च, errorCode);
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त {}; }
        सचे (test.isEquivalentTo(max)) {
            पटिनिवत्त LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    पटिनिवत्त LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
