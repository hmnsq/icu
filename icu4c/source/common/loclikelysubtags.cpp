// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#ಅನ್ತರ್ಭಾವ <utility>
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/localpointer.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "unicode/uscript.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "loclikelysubtags.h"
#ಅನ್ತರ್ಭಾವ "lsr.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "uinvchar.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "uniquecharstr.h"
#ಅನ್ತರ್ಭಾವ "uresdata.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"

U_NAMESPACE_BEGIN

ನಾಮಾಕಾಶ {

constexpr ವರ್ಣ PSEUDO_ACCENTS_PREFIX = '\'';  // -XA, -PSACCENT
constexpr ವರ್ಣ PSEUDO_BIDI_PREFIX = '+';  // -XB, -PSBIDI
constexpr ವರ್ಣ PSEUDO_CRACKED_PREFIX = ',';  // -XC, -PSCRACK

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

ಸಂರಚನಾ LikelySubtagsData {
    UResourceBundle *langInfoBundle = nullptr;
    UniqueCharStrings strings;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    ಧ್ರುವ uint8_t *trieBytes = nullptr;
    LSR *lsrs = nullptr;
    int32_t lsrsLength = 0;

    LocaleDistanceData distanceData;

    LikelySubtagsData(UErrorCode &errorCode) : strings(errorCode) {}

    ~LikelySubtagsData() {
        ures_close(langInfoBundle);
        delete[] lsrs;
    }

    ಶೂನ್ಯ load(UErrorCode &errorCode) {
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
        ರಾಶಿ_UResource_Bundle ರಾಶಿ_Temp_Bundle;
        Resource_Data_ಮೌಲ್ಯ value;
        ures_get_ಮೌಲ್ಯ_With_Fallback(langInfoBundle, "likely", ರಾಶಿ_Temp_Bundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        ಯದಿ (likelyTable.find_ಮೌಲ್ಯ("m49", value)) {
            m49Array = value.getArray(errorCode);
        } ಅನ್ಯಥಾ {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಯದಿ (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಯದಿ ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಯದಿ (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }

        ಯದಿ (!likelyTable.find_ಮೌಲ್ಯ("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        int32_t length;
        trieBytes = value.get_ದ್ವಿಮಾನ(length, errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_get_ಮೌಲ್ಯ_With_Fallback(langInfoBundle, "match", ರಾಶಿ_Temp_Bundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        ಯದಿ (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }

            ಯದಿ (matchTable.find_ಮೌಲ್ಯ("trie", value)) {
                distanceData.distanceTrieBytes = value.get_ದ್ವಿಮಾನ(length, errorCode);
                ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
            }

            ಯದಿ (matchTable.find_ಮೌಲ್ಯ("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.get_ದ್ವಿಮಾನ(length, errorCode);
                ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
                ಯದಿ (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    ಪ್ರತ್ಯಾಗಮ;
                }
            }

            ಯದಿ (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                ಪ್ರತ್ಯಾಗಮ;
            }
            ಯದಿ ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }

            ಯದಿ (matchTable.find_ಮೌಲ್ಯ("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
                ಯದಿ (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    ಪ್ರತ್ಯಾಗಮ;
                }
            }
        } ಅನ್ಯಥಾ ಯದಿ (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } ಅನ್ಯಥಾ {  // error other than missing resource
            errorCode = matchErrorCode;
            ಪ್ರತ್ಯಾಗಮ;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languagesLength / 2, errorCode);
        ಪ್ರತಿ (int32_t i = 0; i < languagesLength; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regionsLength / 2, errorCode);
        ಪ್ರತಿ (int32_t i = 0; i < regionsLength; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = ನೂತನ LSR[lsrsLength];
        ಯದಿ (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಪ್ರತಿ (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        ಯದಿ (partitionsLength > 0) {
            distanceData.partitions = static_cast<ಧ್ರುವ ವರ್ಣ **>(
                uprv_malloc(partitionsLength * ಪರಿಮಾಣ(ಧ್ರುವ ವರ್ಣ *)));
            ಯದಿ (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
            ಪ್ರತಿ (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        ಯದಿ (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = ನೂತನ LSR[distanceData.paradigmsLength];
            ಯದಿ (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
            ಪ್ರತಿ (int32_t i = 0, j = 0; i < paradigmSubtagsLength; i += 3, ++j) {
                paradigms[j] = LSR(strings.get(paradigmSubtagIndexes[i]),
                                   strings.get(paradigmSubtagIndexes[i + 1]),
                                   strings.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            distanceData.paradigms = paradigms;
        }
    }

ಗುಪ್ತ:
    ತಾರ್ಕಿಕ readStrings(ಧ್ರುವ ResourceTable &table, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, Resource_ಮೌಲ್ಯ &value,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
        ಯದಿ (table.find_ಮೌಲ್ಯ(ಕೀಲಿ, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
            length = stringArray.getSize();
            ಯದಿ (length == 0) { ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            ಯದಿ (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < length; ++i) {
                ಯದಿ (stringArray.get_ಮೌಲ್ಯ(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
                }
            }
        }
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    UnicodeString toLanguage(ಪೂರ್ಣಾಙ್ಕ encoded) {
        ಯದಿ (encoded == 0) {
            ಪ್ರತ್ಯಾಗಮ UNICODE_STRING_SIMPLE("");
        }
        ಯದಿ (encoded == 1) {
            ಪ್ರತ್ಯಾಗಮ UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        ವರ್ಣ lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        ಯದಿ (encoded / (27 * 27) == 0) {
            ಪ್ರತ್ಯಾಗಮ UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        ಪ್ರತ್ಯಾಗಮ UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(ಪೂರ್ಣಾಙ್ಕ encoded) {
        ಯದಿ (encoded == 0) {
            ಪ್ರತ್ಯಾಗಮ UNICODE_STRING_SIMPLE("");
        }
        ಯದಿ (encoded == 1) {
            ಪ್ರತ್ಯಾಗಮ UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        ಧ್ರುವ ವರ್ಣ* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        ಯದಿ (script == nullptr) {
            ಪ್ರತ್ಯಾಗಮ UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        ಪ್ರತ್ಯಾಗಮ UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(ಧ್ರುವ ResourceArray &m49Array, Resource_ಮೌಲ್ಯ &value, ಪೂರ್ಣಾಙ್ಕ index, UErrorCode &errorCode) {
        ಯದಿ (U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ UNICODE_STRING_SIMPLE("");
        }
        ಯದಿ (m49Array.get_ಮೌಲ್ಯ(index, value)) {
            ಪ್ರತ್ಯಾಗಮ value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        ಪ್ರತ್ಯಾಗಮ UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(ಧ್ರುವ ResourceArray& m49Array, Resource_ಮೌಲ್ಯ &value, ಪೂರ್ಣಾಙ್ಕ encoded, UErrorCode &errorCode) {
        ಯದಿ (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            ಪ್ರತ್ಯಾಗಮ UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        ಯದಿ (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            ಪ್ರತ್ಯಾಗಮ  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        ವರ್ಣ region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        ಪ್ರತ್ಯಾಗಮ UnicodeString(region, 2, US_INV);
    }

    ತಾರ್ಕಿಕ readLSREncodedStrings(ಧ್ರುವ ResourceTable &table, ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ, Resource_ಮೌಲ್ಯ &value, ಧ್ರುವ ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
        ಯದಿ (table.find_ಮೌಲ್ಯ(ಕೀಲಿ, value)) {
            ಧ್ರುವ int32_t* vectors = value.getIntVector(length, errorCode);
            ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
            ಯದಿ (length == 0) { ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            ಯದಿ (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.add_By_ಮೌಲ್ಯ(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.add_By_ಮೌಲ್ಯ(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.add_By_ಮೌಲ್ಯ(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
            }
            length *= 3;
        }
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
};

ನಾಮಾಕಾಶ {

LikelySubtags *gLikelySubtags = nullptr;
UVector *gMacroregions = nullptr;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    delete gLikelySubtags;
    gLikelySubtags = nullptr;
    delete gMacroregions;
    gMacroregions = nullptr;
    gInitOnce.reset();
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

constexpr ಧ್ರುವ char16_t* MACROREGION_HARDCODE[] = {
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
ಶೂನ್ಯ processMacroregionRange(ಧ್ರುವ UnicodeString& regionName, UVector* newMacroRegions, UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    ಯದಿ ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        ಯಾವತ್ ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(ನೂತನ UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } ಅನ್ಯಥಾ {
        LocalPointer<UnicodeString> newRegion(ನೂತನ UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#ಯದಿ U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    LocalPointer<UVector> newMacroRegions(ನೂತನ UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_get_By_ಕೀಲಿ(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_get_By_ಕೀಲಿ(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_get_By_ಕೀಲಿ(regionList.getAlias(),"macroregion",nullptr,&status));

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಯಾವತ್ (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
    }

    ಪ್ರತ್ಯಾಗಮ newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    LocalPointer<UVector> newMacroRegions(ನೂತನ UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಪ್ರತಿ (ಧ್ರುವ auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
    }

    ಪ್ರತ್ಯಾಗಮ newMacroRegions.orphan();
}

}  // namespace

ಶೂನ್ಯ U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    gLikelySubtags = ನೂತನ LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#ಯದಿ U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    ಯದಿ (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ಧ್ರುವ LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    ಪ್ರತ್ಯಾಗಮ gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#ಯದಿ U_DEBUG
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
    U_ASSERT(USTRINGTRIE_HAS_ಮೌಲ್ಯ(result));
    defaultLsrIndex = trie.get_ಮೌಲ್ಯ();
    trie.reset();

    ಪ್ರತಿ (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        ಯದಿ (result == USTRINGTRIE_NO_ಮೌಲ್ಯ) {
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

LSR LikelySubtags::makeMaximizedLsrFrom(ಧ್ರುವ Locale &locale,
                                         ತಾರ್ಕಿಕ return_ಒಳಹರಿವು_If_Unmatch,
                                         UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ {}; }
    ಯದಿ (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ {};
    }
    ಧ್ರುವ ವರ್ಣ *name = locale.getName();
    ಯದಿ (!return_ಒಳಹರಿವು_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        ಪ್ರತ್ಯಾಗಮ LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_ಒಳಹರಿವು_If_Unmatch, errorCode);

    ಯದಿ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        ಪ್ರತ್ಯಾಗಮ LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    ಪ್ರತ್ಯಾಗಮ max;
}

ನಾಮಾಕಾಶ {

ಧ್ರುವ ವರ್ಣ *getCanonical(ಧ್ರುವ CharStringMap &aliases, ಧ್ರುವ ವರ್ಣ *alias) {
    ಧ್ರುವ ವರ್ಣ *canonical = aliases.get(alias);
    ಪ್ರತ್ಯಾಗಮ canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(ಧ್ರುವ ವರ್ಣ *language, ಧ್ರುವ ವರ್ಣ *script, ಧ್ರುವ ವರ್ಣ *region,
                                     ಧ್ರುವ ವರ್ಣ *variant,
                                     ತಾರ್ಕಿಕ return_ಒಳಹರಿವು_If_Unmatch,
                                     UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    ಯದಿ (!return_ಒಳಹರಿವು_If_Unmatch) {
        ವರ್ಣ c1;
        ಯದಿ (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            ವಿಕಲ್ಪ (c1) {
            ಪಕ್ಷ 'A':
                ಪ್ರತ್ಯಾಗಮ LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            ಪಕ್ಷ 'B':
                ಪ್ರತ್ಯಾಗಮ LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            ಪಕ್ಷ 'C':
                ಪ್ರತ್ಯಾಗಮ LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            ಸ್ವಭಾವ:  // normal locale
                ಭಙ್ಗ;
            }
        }

        ಯದಿ (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            ಯದಿ (uprv_strcmp(variant, "PSACCENT") == 0) {
                ಪ್ರತ್ಯಾಗಮ LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } ಅನ್ಯಥಾ ಯದಿ (uprv_strcmp(variant, "PSBIDI") == 0) {
                ಪ್ರತ್ಯಾಗಮ LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } ಅನ್ಯಥಾ ಯದಿ (uprv_strcmp(variant, "PSCRACK") == 0) {
                ಪ್ರತ್ಯಾಗಮ LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    ಪ್ರತ್ಯಾಗಮ maximize(language, script, region, return_ಒಳಹರಿವು_If_Unmatch, errorCode);
}

LSR LikelySubtags::maximize(ಧ್ರುವ ವರ್ಣ *language, ಧ್ರುವ ವರ್ಣ *script, ಧ್ರುವ ವರ್ಣ *region,
                             ತಾರ್ಕಿಕ return_ಒಳಹರಿವು_If_Unmatch,
                             UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ {}; }
    ಪ್ರತ್ಯಾಗಮ maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_ಒಳಹರಿವು_If_Unmatch,
                    errorCode);
}

ತಾರ್ಕಿಕ LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    ಪ್ರತ್ಯಾಗಮ gMacroregions->contains((ಶೂನ್ಯ *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             ತಾರ್ಕಿಕ return_ಒಳಹರಿವು_If_Unmatch,
                             UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ {}; }
    ಯದಿ (language.compare("und") == 0) {
        language = "";
    }
    ಯದಿ (script.compare("Zzzz") == 0) {
        script = "";
    }
    ಯದಿ (region.compare("ZZ") == 0) {
        region = "";
    }
    ಯದಿ (!script.empty() && !region.empty() && !language.empty()) {
        ಪ್ರತ್ಯಾಗಮ LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    ತಾರ್ಕಿಕ retainLanguage = ಅಸತ್ಯ;
    ತಾರ್ಕಿಕ retainScript = ಅಸತ್ಯ;
    ತಾರ್ಕಿಕ retainRegion = ಅಸತ್ಯ;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    ಯದಿ (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } ಅನ್ಯಥಾ {
        value = trieNext(iter, language, 0);
    }
    ತಾರ್ಕಿಕ matchLanguage = (value >= 0);
    ತಾರ್ಕಿಕ matchScript = ಅಸತ್ಯ;
    ಯದಿ (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } ಅನ್ಯಥಾ {
        retainLanguage = ಸತ್ಯ;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    ಯದಿ (value >= 0 && !script.empty()) {
        matchScript = ಸತ್ಯ;
    }
    ಯದಿ (value > 0) {
        // Intermediate or final value from just language.
        ಯದಿ (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } ಅನ್ಯಥಾ {
        value = trieNext(iter, script, 0);
        ಯದಿ (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } ಅನ್ಯಥಾ {
            retainScript = ಸತ್ಯ;
            ಯದಿ (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } ಅನ್ಯಥಾ {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    ತಾರ್ಕಿಕ matchRegion = ಅಸತ್ಯ;
    ಯದಿ (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } ಅನ್ಯಥಾ {
        value = trieNext(iter, region, 0);
        ಯದಿ (value >= 0) {
            ಯದಿ (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = ಸತ್ಯ;
                matchRegion = ಸತ್ಯ;
            }
        } ಅನ್ಯಥಾ {
            retainRegion = ಸತ್ಯ;
            ಯದಿ (state == 0) {
                value = defaultLsrIndex;
            } ಅನ್ಯಥಾ {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                ಯದಿ (value < 0) {
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
                    ಯದಿ (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    ಯದಿ (return_ಒಳಹರಿವು_If_Unmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      ಪ್ರತ್ಯಾಗಮ LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    ಯದಿ (language.empty()) {
        language = StringPiece("und");
    }

    ಯದಿ (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        ಧ್ರುವ LSR &matched = lsrs[value];
        ಪ್ರತ್ಯಾಗಮ LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    ಯದಿ (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    ಯದಿ (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    ಯದಿ (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    ಪ್ರತ್ಯಾಗಮ LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(ಧ್ರುವ LSR &lsr, ಧ್ರುವ LSR &other, int32_t likelyInfo) ಧ್ರುವ {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    ಯದಿ (uprv_strcmp(lsr.language, other.language) != 0) {
        ಪ್ರತ್ಯಾಗಮ 0xfffffffc;  // negative, lsr not better than other
    }
    ಯದಿ (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        ಯದಿ (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } ಅನ್ಯಥಾ {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        ಧ್ರುವ LSR &likely = lsrs[index];
        ಯದಿ (uprv_strcmp(lsr.script, likely.script) == 0) {
            ಪ್ರತ್ಯಾಗಮ likelyInfo | 1;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ likelyInfo & ~1;
        }
    }
    ಯದಿ (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        ಯದಿ (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } ಅನ್ಯಥಾ {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        ಧ್ರುವ LSR &likely = lsrs[index];
        ಯದಿ (uprv_strcmp(lsr.region, likely.region) == 0) {
            ಪ್ರತ್ಯಾಗಮ likelyInfo | 1;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ likelyInfo & ~1;
        }
    }
    ಪ್ರತ್ಯಾಗಮ likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(ಧ್ರುವ ವರ್ಣ *language, ಧ್ರುವ ವರ್ಣ *script) ಧ್ರುವ {
    ಯದಿ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    ಯದಿ (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    ಯದಿ (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } ಅನ್ಯಥಾ {
        value = trieNext(iter, language, 0);
    }
    ಯದಿ (value >= 0) {
        state = iter.getState64();
    } ಅನ್ಯಥಾ {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    ಯದಿ (value > 0) {
        // Intermediate or final value from just language.
        ಯದಿ (value == SKIP_SCRIPT) {
            value = 0;
        }
    } ಅನ್ಯಥಾ {
        value = trieNext(iter, script, 0);
        ಯದಿ (value >= 0) {
            state = iter.getState64();
        } ಅನ್ಯಥಾ {
            ಯದಿ (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } ಅನ್ಯಥಾ {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    ಯದಿ (value > 0) {
        // Final value from just language or language+script.
    } ಅನ್ಯಥಾ {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    ಪ್ರತ್ಯಾಗಮ value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, ಧ್ರುವ ವರ್ಣ *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    ಯದಿ ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } ಅನ್ಯಥಾ {
        ಪ್ರತಿ (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            ಯದಿ (next != 0) {
                ಯದಿ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    ಪ್ರತ್ಯಾಗಮ -1;
                }
            } ಅನ್ಯಥಾ {
                // last character of this subtag
                result = iter.next(c | 0x80);
                ಭಙ್ಗ;
            }
            c = next;
        }
    }
    ವಿಕಲ್ಪ (result) {
    ಪಕ್ಷ USTRINGTRIE_NO_MATCH: ಪ್ರತ್ಯಾಗಮ -1;
    ಪಕ್ಷ USTRINGTRIE_NO_ಮೌಲ್ಯ: ಪ್ರತ್ಯಾಗಮ 0;
    ಪಕ್ಷ USTRINGTRIE_INTERMEDIATE_ಮೌಲ್ಯ:
        U_ASSERT(iter.get_ಮೌಲ್ಯ() == SKIP_SCRIPT);
        ಪ್ರತ್ಯಾಗಮ SKIP_SCRIPT;
    ಪಕ್ಷ USTRINGTRIE_FINAL_ಮೌಲ್ಯ: ಪ್ರತ್ಯಾಗಮ iter.get_ಮೌಲ್ಯ();
    ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    ಯದಿ (s.length() == i) {
        result = iter.next(u'*');
    } ಅನ್ಯಥಾ {
        c = s.data()[i];
        ಪ್ರತಿ (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            ಯದಿ (i+1 != s.length()) {
                ಯದಿ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    ಪ್ರತ್ಯಾಗಮ -1;
                }
                c = s.data()[++i];
            } ಅನ್ಯಥಾ {
                // last character of this subtag
                result = iter.next(c | 0x80);
                ಭಙ್ಗ;
            }
        }
    }
    ವಿಕಲ್ಪ (result) {
    ಪಕ್ಷ USTRINGTRIE_NO_MATCH: ಪ್ರತ್ಯಾಗಮ -1;
    ಪಕ್ಷ USTRINGTRIE_NO_ಮೌಲ್ಯ: ಪ್ರತ್ಯಾಗಮ 0;
    ಪಕ್ಷ USTRINGTRIE_INTERMEDIATE_ಮೌಲ್ಯ:
        U_ASSERT(iter.get_ಮೌಲ್ಯ() == SKIP_SCRIPT);
        ಪ್ರತ್ಯಾಗಮ SKIP_SCRIPT;
    ಪಕ್ಷ USTRINGTRIE_FINAL_ಮೌಲ್ಯ: ಪ್ರತ್ಯಾಗಮ iter.get_ಮೌಲ್ಯ();
    ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    ತಾರ್ಕಿಕ favorScript,
                                    UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ {}; }
    LSR max = maximize(language, script, region, ಸತ್ಯ, errorCode);
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ {}; }
    // If no match, return it.
    ಯದಿ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        ಪ್ರತ್ಯಾಗಮ LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", ಸತ್ಯ, errorCode);
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ {}; }
    ಯದಿ (test.isEquivalentTo(max)) {
        ಪ್ರತ್ಯಾಗಮ LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    ಯದಿ (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, ಸತ್ಯ, errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ {}; }
        ಯದಿ (test.isEquivalentTo(max)) {
            ಪ್ರತ್ಯಾಗಮ LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", ಸತ್ಯ, errorCode);
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ {}; }
    ಯದಿ (test.isEquivalentTo(max)) {
        ಪ್ರತ್ಯಾಗಮ LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    ಯದಿ (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, ಸತ್ಯ, errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ {}; }
        ಯದಿ (test.isEquivalentTo(max)) {
            ಪ್ರತ್ಯಾಗಮ LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    ಪ್ರತ್ಯಾಗಮ LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
