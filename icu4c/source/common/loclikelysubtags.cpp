// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#അന്തര്ഭാവ <utility>
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/bytestrie.h"
#അന്തര്ഭാവ "unicode/localpointer.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "unicode/ures.h"
#അന്തര്ഭാവ "unicode/uscript.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "loclikelysubtags.h"
#അന്തര്ഭാവ "lsr.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "uinvchar.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "uniquecharstr.h"
#അന്തര്ഭാവ "uresdata.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "uvector.h"

U_NAMESPACE_BEGIN

നാമാകാശ {

constexpr അക്ഷരം PSEUDO_ACCENTS_PREFIX = '\'';  // -XA, -PSACCENT
constexpr അക്ഷരം PSEUDO_BIDI_PREFIX = '+';  // -XB, -PSBIDI
constexpr അക്ഷരം PSEUDO_CRACKED_PREFIX = ',';  // -XC, -PSCRACK

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

ഘടന LikelySubtagsData {
    UResourceBundle *langInfoBundle = nullptr;
    UniqueCharStrings strings;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    സ്ഥിരം uint8_t *trieBytes = nullptr;
    LSR *lsrs = nullptr;
    int32_t lsrsLength = 0;

    LocaleDistanceData distanceData;

    LikelySubtagsData(UErrorCode &errorCode) : strings(errorCode) {}

    ~LikelySubtagsData() {
        ures_close(langInfoBundle);
        delete[] lsrs;
    }

    ശൂന്യം load(UErrorCode &errorCode) {
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
        അട്ടി_UResource_Bundle അട്ടി_Temp_Bundle;
        Resource_Data_മൂല്യം value;
        ures_get_മൂല്യം_With_Fallback(langInfoBundle, "likely", അട്ടി_Temp_Bundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        എങ്കിൽ (likelyTable.find_മൂല്യം("m49", value)) {
            m49Array = value.getArray(errorCode);
        } അല്ലെങ്കിൽ {
            errorCode = U_MISSING_RESOURCE_ERROR;
            തിരികെ;
        }
        എങ്കിൽ (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            തിരികെ;
        }
        എങ്കിൽ ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            തിരികെ;
        }
        എങ്കിൽ (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            തിരികെ;
        }

        എങ്കിൽ (!likelyTable.find_മൂല്യം("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            തിരികെ;
        }
        int32_t length;
        trieBytes = value.get_ദ്വിമാന(length, errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_get_മൂല്യം_With_Fallback(langInfoBundle, "match", അട്ടി_Temp_Bundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        എങ്കിൽ (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }

            എങ്കിൽ (matchTable.find_മൂല്യം("trie", value)) {
                distanceData.distanceTrieBytes = value.get_ദ്വിമാന(length, errorCode);
                എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
            }

            എങ്കിൽ (matchTable.find_മൂല്യം("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.get_ദ്വിമാന(length, errorCode);
                എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
                എങ്കിൽ (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    തിരികെ;
                }
            }

            എങ്കിൽ (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                തിരികെ;
            }
            എങ്കിൽ ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                തിരികെ;
            }

            എങ്കിൽ (matchTable.find_മൂല്യം("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
                എങ്കിൽ (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    തിരികെ;
                }
            }
        } അല്ലെങ്കിൽ എങ്കിൽ (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } അല്ലെങ്കിൽ {  // error other than missing resource
            errorCode = matchErrorCode;
            തിരികെ;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languagesLength / 2, errorCode);
        വേണ്ടി (int32_t i = 0; i < languagesLength; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regionsLength / 2, errorCode);
        വേണ്ടി (int32_t i = 0; i < regionsLength; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = പുതിയ LSR[lsrsLength];
        എങ്കിൽ (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ;
        }
        വേണ്ടി (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        എങ്കിൽ (partitionsLength > 0) {
            distanceData.partitions = static_cast<സ്ഥിരം അക്ഷരം **>(
                uprv_malloc(partitionsLength * വലുപ്പം(സ്ഥിരം അക്ഷരം *)));
            എങ്കിൽ (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ;
            }
            വേണ്ടി (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        എങ്കിൽ (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = പുതിയ LSR[distanceData.paradigmsLength];
            എങ്കിൽ (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ;
            }
            വേണ്ടി (int32_t i = 0, j = 0; i < paradigmSubtagsLength; i += 3, ++j) {
                paradigms[j] = LSR(strings.get(paradigmSubtagIndexes[i]),
                                   strings.get(paradigmSubtagIndexes[i + 1]),
                                   strings.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            distanceData.paradigms = paradigms;
        }
    }

സ്വകാര്യ:
    തർക്കം readStrings(സ്ഥിരം ResourceTable &table, സ്ഥിരം അക്ഷരം *താക്കോൽ, Resource_മൂല്യം &value,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
        എങ്കിൽ (table.find_മൂല്യം(താക്കോൽ, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
            length = stringArray.getSize();
            എങ്കിൽ (length == 0) { തിരികെ സത്യം; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            എങ്കിൽ (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ അസത്യം;
            }
            വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < length; ++i) {
                എങ്കിൽ (stringArray.get_മൂല്യം(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
                }
            }
        }
        തിരികെ സത്യം;
    }
    UnicodeString toLanguage(പൂർണ്ണസംഖ്യ encoded) {
        എങ്കിൽ (encoded == 0) {
            തിരികെ UNICODE_STRING_SIMPLE("");
        }
        എങ്കിൽ (encoded == 1) {
            തിരികെ UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        അക്ഷരം lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        എങ്കിൽ (encoded / (27 * 27) == 0) {
            തിരികെ UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        തിരികെ UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(പൂർണ്ണസംഖ്യ encoded) {
        എങ്കിൽ (encoded == 0) {
            തിരികെ UNICODE_STRING_SIMPLE("");
        }
        എങ്കിൽ (encoded == 1) {
            തിരികെ UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        സ്ഥിരം അക്ഷരം* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        എങ്കിൽ (script == nullptr) {
            തിരികെ UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        തിരികെ UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(സ്ഥിരം ResourceArray &m49Array, Resource_മൂല്യം &value, പൂർണ്ണസംഖ്യ index, UErrorCode &errorCode) {
        എങ്കിൽ (U_FAILURE(errorCode)) {
            തിരികെ UNICODE_STRING_SIMPLE("");
        }
        എങ്കിൽ (m49Array.get_മൂല്യം(index, value)) {
            തിരികെ value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        തിരികെ UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(സ്ഥിരം ResourceArray& m49Array, Resource_മൂല്യം &value, പൂർണ്ണസംഖ്യ encoded, UErrorCode &errorCode) {
        എങ്കിൽ (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            തിരികെ UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        എങ്കിൽ (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            തിരികെ  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        അക്ഷരം region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        തിരികെ UnicodeString(region, 2, US_INV);
    }

    തർക്കം readLSREncodedStrings(സ്ഥിരം ResourceTable &table, സ്ഥിരം അക്ഷരം* താക്കോൽ, Resource_മൂല്യം &value, സ്ഥിരം ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
        എങ്കിൽ (table.find_മൂല്യം(താക്കോൽ, value)) {
            സ്ഥിരം int32_t* vectors = value.getIntVector(length, errorCode);
            എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
            എങ്കിൽ (length == 0) { തിരികെ സത്യം; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            എങ്കിൽ (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ അസത്യം;
            }
            വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.add_By_മൂല്യം(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.add_By_മൂല്യം(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.add_By_മൂല്യം(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
            }
            length *= 3;
        }
        തിരികെ സത്യം;
    }
};

നാമാകാശ {

LikelySubtags *gLikelySubtags = nullptr;
UVector *gMacroregions = nullptr;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    delete gLikelySubtags;
    gLikelySubtags = nullptr;
    delete gMacroregions;
    gMacroregions = nullptr;
    gInitOnce.reset();
    തിരികെ സത്യം;
}

constexpr സ്ഥിരം char16_t* MACROREGION_HARDCODE[] = {
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
ശൂന്യം processMacroregionRange(സ്ഥിരം UnicodeString& regionName, UVector* newMacroRegions, UErrorCode& status) {
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    എങ്കിൽ ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        ആകുമ്പോൾ ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(പുതിയ UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } അല്ലെങ്കിൽ {
        LocalPointer<UnicodeString> newRegion(പുതിയ UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#എങ്കിൽ U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ nullptr; }
    LocalPointer<UVector> newMacroRegions(പുതിയ UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_get_By_താക്കോൽ(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_get_By_താക്കോൽ(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_get_By_താക്കോൽ(regionList.getAlias(),"macroregion",nullptr,&status));

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }

    ആകുമ്പോൾ (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ nullptr;
        }
    }

    തിരികെ newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ nullptr; }
    LocalPointer<UVector> newMacroRegions(പുതിയ UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }

    വേണ്ടി (സ്ഥിരം auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ nullptr;
        }
    }

    തിരികെ newMacroRegions.orphan();
}

}  // namespace

ശൂന്യം U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
    gLikelySubtags = പുതിയ LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#എങ്കിൽ U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    എങ്കിൽ (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

സ്ഥിരം LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    തിരികെ gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#എങ്കിൽ U_DEBUG
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
    U_ASSERT(USTRINGTRIE_HAS_മൂല്യം(result));
    defaultLsrIndex = trie.get_മൂല്യം();
    trie.reset();

    വേണ്ടി (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        എങ്കിൽ (result == USTRINGTRIE_NO_മൂല്യം) {
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

LSR LikelySubtags::makeMaximizedLsrFrom(സ്ഥിരം Locale &locale,
                                         തർക്കം return_നിവേശം_If_Unmatch,
                                         UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ {}; }
    എങ്കിൽ (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ {};
    }
    സ്ഥിരം അക്ഷരം *name = locale.getName();
    എങ്കിൽ (!return_നിവേശം_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        തിരികെ LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_നിവേശം_If_Unmatch, errorCode);

    എങ്കിൽ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        തിരികെ LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    തിരികെ max;
}

നാമാകാശ {

സ്ഥിരം അക്ഷരം *getCanonical(സ്ഥിരം CharStringMap &aliases, സ്ഥിരം അക്ഷരം *alias) {
    സ്ഥിരം അക്ഷരം *canonical = aliases.get(alias);
    തിരികെ canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(സ്ഥിരം അക്ഷരം *language, സ്ഥിരം അക്ഷരം *script, സ്ഥിരം അക്ഷരം *region,
                                     സ്ഥിരം അക്ഷരം *variant,
                                     തർക്കം return_നിവേശം_If_Unmatch,
                                     UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    എങ്കിൽ (!return_നിവേശം_If_Unmatch) {
        അക്ഷരം c1;
        എങ്കിൽ (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            തിരഞ്ഞെടുപ്പ് (c1) {
            അവസ്ഥ 'A':
                തിരികെ LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            അവസ്ഥ 'B':
                തിരികെ LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            അവസ്ഥ 'C':
                തിരികെ LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            സ്ഥിരസ്ഥിതി:  // normal locale
                നിർത്തുക;
            }
        }

        എങ്കിൽ (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            എങ്കിൽ (uprv_strcmp(variant, "PSACCENT") == 0) {
                തിരികെ LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(variant, "PSBIDI") == 0) {
                തിരികെ LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(variant, "PSCRACK") == 0) {
                തിരികെ LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    തിരികെ maximize(language, script, region, return_നിവേശം_If_Unmatch, errorCode);
}

LSR LikelySubtags::maximize(സ്ഥിരം അക്ഷരം *language, സ്ഥിരം അക്ഷരം *script, സ്ഥിരം അക്ഷരം *region,
                             തർക്കം return_നിവേശം_If_Unmatch,
                             UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ {}; }
    തിരികെ maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_നിവേശം_If_Unmatch,
                    errorCode);
}

തർക്കം LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    തിരികെ gMacroregions->contains((ശൂന്യം *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             തർക്കം return_നിവേശം_If_Unmatch,
                             UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ {}; }
    എങ്കിൽ (language.compare("und") == 0) {
        language = "";
    }
    എങ്കിൽ (script.compare("Zzzz") == 0) {
        script = "";
    }
    എങ്കിൽ (region.compare("ZZ") == 0) {
        region = "";
    }
    എങ്കിൽ (!script.empty() && !region.empty() && !language.empty()) {
        തിരികെ LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    തർക്കം retainLanguage = അസത്യം;
    തർക്കം retainScript = അസത്യം;
    തർക്കം retainRegion = അസത്യം;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    എങ്കിൽ (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } അല്ലെങ്കിൽ {
        value = trieNext(iter, language, 0);
    }
    തർക്കം matchLanguage = (value >= 0);
    തർക്കം matchScript = അസത്യം;
    എങ്കിൽ (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } അല്ലെങ്കിൽ {
        retainLanguage = സത്യം;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    എങ്കിൽ (value >= 0 && !script.empty()) {
        matchScript = സത്യം;
    }
    എങ്കിൽ (value > 0) {
        // Intermediate or final value from just language.
        എങ്കിൽ (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } അല്ലെങ്കിൽ {
        value = trieNext(iter, script, 0);
        എങ്കിൽ (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } അല്ലെങ്കിൽ {
            retainScript = സത്യം;
            എങ്കിൽ (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } അല്ലെങ്കിൽ {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    തർക്കം matchRegion = അസത്യം;
    എങ്കിൽ (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } അല്ലെങ്കിൽ {
        value = trieNext(iter, region, 0);
        എങ്കിൽ (value >= 0) {
            എങ്കിൽ (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = സത്യം;
                matchRegion = സത്യം;
            }
        } അല്ലെങ്കിൽ {
            retainRegion = സത്യം;
            എങ്കിൽ (state == 0) {
                value = defaultLsrIndex;
            } അല്ലെങ്കിൽ {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                എങ്കിൽ (value < 0) {
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
                    എങ്കിൽ (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    എങ്കിൽ (return_നിവേശം_If_Unmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      തിരികെ LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    എങ്കിൽ (language.empty()) {
        language = StringPiece("und");
    }

    എങ്കിൽ (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        സ്ഥിരം LSR &matched = lsrs[value];
        തിരികെ LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    എങ്കിൽ (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    എങ്കിൽ (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    എങ്കിൽ (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    തിരികെ LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(സ്ഥിരം LSR &lsr, സ്ഥിരം LSR &other, int32_t likelyInfo) സ്ഥിരം {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    എങ്കിൽ (uprv_strcmp(lsr.language, other.language) != 0) {
        തിരികെ 0xfffffffc;  // negative, lsr not better than other
    }
    എങ്കിൽ (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        എങ്കിൽ (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } അല്ലെങ്കിൽ {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        സ്ഥിരം LSR &likely = lsrs[index];
        എങ്കിൽ (uprv_strcmp(lsr.script, likely.script) == 0) {
            തിരികെ likelyInfo | 1;
        } അല്ലെങ്കിൽ {
            തിരികെ likelyInfo & ~1;
        }
    }
    എങ്കിൽ (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        എങ്കിൽ (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } അല്ലെങ്കിൽ {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        സ്ഥിരം LSR &likely = lsrs[index];
        എങ്കിൽ (uprv_strcmp(lsr.region, likely.region) == 0) {
            തിരികെ likelyInfo | 1;
        } അല്ലെങ്കിൽ {
            തിരികെ likelyInfo & ~1;
        }
    }
    തിരികെ likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(സ്ഥിരം അക്ഷരം *language, സ്ഥിരം അക്ഷരം *script) സ്ഥിരം {
    എങ്കിൽ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    എങ്കിൽ (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    എങ്കിൽ (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } അല്ലെങ്കിൽ {
        value = trieNext(iter, language, 0);
    }
    എങ്കിൽ (value >= 0) {
        state = iter.getState64();
    } അല്ലെങ്കിൽ {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    എങ്കിൽ (value > 0) {
        // Intermediate or final value from just language.
        എങ്കിൽ (value == SKIP_SCRIPT) {
            value = 0;
        }
    } അല്ലെങ്കിൽ {
        value = trieNext(iter, script, 0);
        എങ്കിൽ (value >= 0) {
            state = iter.getState64();
        } അല്ലെങ്കിൽ {
            എങ്കിൽ (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } അല്ലെങ്കിൽ {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    എങ്കിൽ (value > 0) {
        // Final value from just language or language+script.
    } അല്ലെങ്കിൽ {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    തിരികെ value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, സ്ഥിരം അക്ഷരം *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    എങ്കിൽ ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } അല്ലെങ്കിൽ {
        വേണ്ടി (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            എങ്കിൽ (next != 0) {
                എങ്കിൽ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    തിരികെ -1;
                }
            } അല്ലെങ്കിൽ {
                // last character of this subtag
                result = iter.next(c | 0x80);
                നിർത്തുക;
            }
            c = next;
        }
    }
    തിരഞ്ഞെടുപ്പ് (result) {
    അവസ്ഥ USTRINGTRIE_NO_MATCH: തിരികെ -1;
    അവസ്ഥ USTRINGTRIE_NO_മൂല്യം: തിരികെ 0;
    അവസ്ഥ USTRINGTRIE_INTERMEDIATE_മൂല്യം:
        U_ASSERT(iter.get_മൂല്യം() == SKIP_SCRIPT);
        തിരികെ SKIP_SCRIPT;
    അവസ്ഥ USTRINGTRIE_FINAL_മൂല്യം: തിരികെ iter.get_മൂല്യം();
    സ്ഥിരസ്ഥിതി: തിരികെ -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    എങ്കിൽ (s.length() == i) {
        result = iter.next(u'*');
    } അല്ലെങ്കിൽ {
        c = s.data()[i];
        വേണ്ടി (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            എങ്കിൽ (i+1 != s.length()) {
                എങ്കിൽ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    തിരികെ -1;
                }
                c = s.data()[++i];
            } അല്ലെങ്കിൽ {
                // last character of this subtag
                result = iter.next(c | 0x80);
                നിർത്തുക;
            }
        }
    }
    തിരഞ്ഞെടുപ്പ് (result) {
    അവസ്ഥ USTRINGTRIE_NO_MATCH: തിരികെ -1;
    അവസ്ഥ USTRINGTRIE_NO_മൂല്യം: തിരികെ 0;
    അവസ്ഥ USTRINGTRIE_INTERMEDIATE_മൂല്യം:
        U_ASSERT(iter.get_മൂല്യം() == SKIP_SCRIPT);
        തിരികെ SKIP_SCRIPT;
    അവസ്ഥ USTRINGTRIE_FINAL_മൂല്യം: തിരികെ iter.get_മൂല്യം();
    സ്ഥിരസ്ഥിതി: തിരികെ -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    തർക്കം favorScript,
                                    UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ {}; }
    LSR max = maximize(language, script, region, സത്യം, errorCode);
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ {}; }
    // If no match, return it.
    എങ്കിൽ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        തിരികെ LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", സത്യം, errorCode);
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ {}; }
    എങ്കിൽ (test.isEquivalentTo(max)) {
        തിരികെ LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    എങ്കിൽ (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, സത്യം, errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ {}; }
        എങ്കിൽ (test.isEquivalentTo(max)) {
            തിരികെ LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", സത്യം, errorCode);
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ {}; }
    എങ്കിൽ (test.isEquivalentTo(max)) {
        തിരികെ LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    എങ്കിൽ (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, സത്യം, errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ {}; }
        എങ്കിൽ (test.isEquivalentTo(max)) {
            തിരികെ LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    തിരികെ LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
