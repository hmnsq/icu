// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#ଅନ୍ତର୍ଭାଵ <utility>
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/bytestrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/localpointer.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uscript.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "loclikelysubtags.h"
#ଅନ୍ତର୍ଭାଵ "lsr.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "uinvchar.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "uniquecharstr.h"
#ଅନ୍ତର୍ଭାଵ "uresdata.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"

U_NAMESPACE_BEGIN

ନାମାକାଶ {

constexpr ଅକ୍ଷର PSEUDO_ACCENTS_PREFIX = '\'';  // -XA, -PSACCENT
constexpr ଅକ୍ଷର PSEUDO_BIDI_PREFIX = '+';  // -XB, -PSBIDI
constexpr ଅକ୍ଷର PSEUDO_CRACKED_PREFIX = ',';  // -XC, -PSCRACK

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

ସଂରଚନା LikelySubtagsData {
    UResourceBundle *langInfoBundle = nullptr;
    UniqueCharStrings strings;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    ଅଚଳ uint8_t *trieBytes = nullptr;
    LSR *lsrs = nullptr;
    int32_t lsrsLength = 0;

    LocaleDistanceData distanceData;

    LikelySubtagsData(UErrorCode &errorCode) : strings(errorCode) {}

    ~LikelySubtagsData() {
        ures_close(langInfoBundle);
        delete[] lsrs;
    }

    ଶୂନ୍ୟ load(UErrorCode &errorCode) {
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
        ଗଦା_UResource_Bundle ଗଦା_Temp_Bundle;
        Resource_Data_ମୂଲ୍ୟ value;
        ures_get_ମୂଲ୍ୟ_With_Fallback(langInfoBundle, "likely", ଗଦା_Temp_Bundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        ଯଦି (likelyTable.find_ମୂଲ୍ୟ("m49", value)) {
            m49Array = value.getArray(errorCode);
        } ଅନ୍ୟଥା {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ଫେରନ୍ତୁ;
        }
        ଯଦି (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            ଫେରନ୍ତୁ;
        }
        ଯଦି ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            ଫେରନ୍ତୁ;
        }
        ଯଦି (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ଫେରନ୍ତୁ;
        }

        ଯଦି (!likelyTable.find_ମୂଲ୍ୟ("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ଫେରନ୍ତୁ;
        }
        int32_t length;
        trieBytes = value.get_ଦ୍ୱିମିକ(length, errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_get_ମୂଲ୍ୟ_With_Fallback(langInfoBundle, "match", ଗଦା_Temp_Bundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        ଯଦି (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }

            ଯଦି (matchTable.find_ମୂଲ୍ୟ("trie", value)) {
                distanceData.distanceTrieBytes = value.get_ଦ୍ୱିମିକ(length, errorCode);
                ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
            }

            ଯଦି (matchTable.find_ମୂଲ୍ୟ("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.get_ଦ୍ୱିମିକ(length, errorCode);
                ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
                ଯଦି (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    ଫେରନ୍ତୁ;
                }
            }

            ଯଦି (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                ଫେରନ୍ତୁ;
            }
            ଯଦି ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                ଫେରନ୍ତୁ;
            }

            ଯଦି (matchTable.find_ମୂଲ୍ୟ("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
                ଯଦି (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    ଫେରନ୍ତୁ;
                }
            }
        } ଅନ୍ୟଥା ଯଦି (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } ଅନ୍ୟଥା {  // error other than missing resource
            errorCode = matchErrorCode;
            ଫେରନ୍ତୁ;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languagesLength / 2, errorCode);
        ପାଇଁ (int32_t i = 0; i < languagesLength; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regionsLength / 2, errorCode);
        ପାଇଁ (int32_t i = 0; i < regionsLength; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = ନୂତନ LSR[lsrsLength];
        ଯଦି (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ;
        }
        ପାଇଁ (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        ଯଦି (partitionsLength > 0) {
            distanceData.partitions = static_cast<ଅଚଳ ଅକ୍ଷର **>(
                uprv_malloc(partitionsLength * ଆକାର(ଅଚଳ ଅକ୍ଷର *)));
            ଯଦି (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ;
            }
            ପାଇଁ (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        ଯଦି (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = ନୂତନ LSR[distanceData.paradigmsLength];
            ଯଦି (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ;
            }
            ପାଇଁ (int32_t i = 0, j = 0; i < paradigmSubtagsLength; i += 3, ++j) {
                paradigms[j] = LSR(strings.get(paradigmSubtagIndexes[i]),
                                   strings.get(paradigmSubtagIndexes[i + 1]),
                                   strings.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            distanceData.paradigms = paradigms;
        }
    }

ବ୍ୟକ୍ତିଗତ:
    ତାର୍କିକ readStrings(ଅଚଳ ResourceTable &table, ଅଚଳ ଅକ୍ଷର *ଚାବି, Resource_ମୂଲ୍ୟ &value,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
        ଯଦି (table.find_ମୂଲ୍ୟ(ଚାବି, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
            length = stringArray.getSize();
            ଯଦି (length == 0) { ଫେରନ୍ତୁ ସତ୍ୟ; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            ଯଦି (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < length; ++i) {
                ଯଦି (stringArray.get_ମୂଲ୍ୟ(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
                }
            }
        }
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    UnicodeString toLanguage(ପୂର୍ଣ୍ଣସଂଖ୍ୟା encoded) {
        ଯଦି (encoded == 0) {
            ଫେରନ୍ତୁ UNICODE_STRING_SIMPLE("");
        }
        ଯଦି (encoded == 1) {
            ଫେରନ୍ତୁ UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        ଅକ୍ଷର lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        ଯଦି (encoded / (27 * 27) == 0) {
            ଫେରନ୍ତୁ UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        ଫେରନ୍ତୁ UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(ପୂର୍ଣ୍ଣସଂଖ୍ୟା encoded) {
        ଯଦି (encoded == 0) {
            ଫେରନ୍ତୁ UNICODE_STRING_SIMPLE("");
        }
        ଯଦି (encoded == 1) {
            ଫେରନ୍ତୁ UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        ଅଚଳ ଅକ୍ଷର* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        ଯଦି (script == nullptr) {
            ଫେରନ୍ତୁ UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        ଫେରନ୍ତୁ UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(ଅଚଳ ResourceArray &m49Array, Resource_ମୂଲ୍ୟ &value, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index, UErrorCode &errorCode) {
        ଯଦି (U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ UNICODE_STRING_SIMPLE("");
        }
        ଯଦି (m49Array.get_ମୂଲ୍ୟ(index, value)) {
            ଫେରନ୍ତୁ value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        ଫେରନ୍ତୁ UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(ଅଚଳ ResourceArray& m49Array, Resource_ମୂଲ୍ୟ &value, ପୂର୍ଣ୍ଣସଂଖ୍ୟା encoded, UErrorCode &errorCode) {
        ଯଦି (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            ଫେରନ୍ତୁ UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        ଯଦି (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            ଫେରନ୍ତୁ  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        ଅକ୍ଷର region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        ଫେରନ୍ତୁ UnicodeString(region, 2, US_INV);
    }

    ତାର୍କିକ readLSREncodedStrings(ଅଚଳ ResourceTable &table, ଅଚଳ ଅକ୍ଷର* ଚାବି, Resource_ମୂଲ୍ୟ &value, ଅଚଳ ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
        ଯଦି (table.find_ମୂଲ୍ୟ(ଚାବି, value)) {
            ଅଚଳ int32_t* vectors = value.getIntVector(length, errorCode);
            ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
            ଯଦି (length == 0) { ଫେରନ୍ତୁ ସତ୍ୟ; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            ଯଦି (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.add_By_ମୂଲ୍ୟ(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.add_By_ମୂଲ୍ୟ(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.add_By_ମୂଲ୍ୟ(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
            }
            length *= 3;
        }
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
};

ନାମାକାଶ {

LikelySubtags *gLikelySubtags = nullptr;
UVector *gMacroregions = nullptr;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    delete gLikelySubtags;
    gLikelySubtags = nullptr;
    delete gMacroregions;
    gMacroregions = nullptr;
    gInitOnce.reset();
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

constexpr ଅଚଳ char16_t* MACROREGION_HARDCODE[] = {
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
ଶୂନ୍ୟ processMacroregionRange(ଅଚଳ UnicodeString& regionName, UVector* newMacroRegions, UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    ଯଦି ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        ଯେତେବେଳେ ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(ନୂତନ UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } ଅନ୍ୟଥା {
        LocalPointer<UnicodeString> newRegion(ନୂତନ UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#ଯଦି U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ nullptr; }
    LocalPointer<UVector> newMacroRegions(ନୂତନ UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_get_By_ଚାବି(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_get_By_ଚାବି(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_get_By_ଚାବି(regionList.getAlias(),"macroregion",nullptr,&status));

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    ଯେତେବେଳେ (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ nullptr;
        }
    }

    ଫେରନ୍ତୁ newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ nullptr; }
    LocalPointer<UVector> newMacroRegions(ନୂତନ UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    ପାଇଁ (ଅଚଳ auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ nullptr;
        }
    }

    ଫେରନ୍ତୁ newMacroRegions.orphan();
}

}  // namespace

ଶୂନ୍ୟ U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    gLikelySubtags = ନୂତନ LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#ଯଦି U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    ଯଦି (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ଅଚଳ LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    ଫେରନ୍ତୁ gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#ଯଦି U_DEBUG
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
    U_ASSERT(USTRINGTRIE_HAS_ମୂଲ୍ୟ(result));
    defaultLsrIndex = trie.get_ମୂଲ୍ୟ();
    trie.reset();

    ପାଇଁ (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        ଯଦି (result == USTRINGTRIE_NO_ମୂଲ୍ୟ) {
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

LSR LikelySubtags::makeMaximizedLsrFrom(ଅଚଳ Locale &locale,
                                         ତାର୍କିକ return_ନିବେଶ_If_Unmatch,
                                         UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ {}; }
    ଯଦି (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ {};
    }
    ଅଚଳ ଅକ୍ଷର *name = locale.getName();
    ଯଦି (!return_ନିବେଶ_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        ଫେରନ୍ତୁ LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_ନିବେଶ_If_Unmatch, errorCode);

    ଯଦି (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        ଫେରନ୍ତୁ LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    ଫେରନ୍ତୁ max;
}

ନାମାକାଶ {

ଅଚଳ ଅକ୍ଷର *getCanonical(ଅଚଳ CharStringMap &aliases, ଅଚଳ ଅକ୍ଷର *alias) {
    ଅଚଳ ଅକ୍ଷର *canonical = aliases.get(alias);
    ଫେରନ୍ତୁ canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(ଅଚଳ ଅକ୍ଷର *language, ଅଚଳ ଅକ୍ଷର *script, ଅଚଳ ଅକ୍ଷର *region,
                                     ଅଚଳ ଅକ୍ଷର *variant,
                                     ତାର୍କିକ return_ନିବେଶ_If_Unmatch,
                                     UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    ଯଦି (!return_ନିବେଶ_If_Unmatch) {
        ଅକ୍ଷର c1;
        ଯଦି (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            ବାଛ (c1) {
            ସ୍ଥିତି 'A':
                ଫେରନ୍ତୁ LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            ସ୍ଥିତି 'B':
                ଫେରନ୍ତୁ LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            ସ୍ଥିତି 'C':
                ଫେରନ୍ତୁ LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  // normal locale
                ଭାଙ୍ଗ;
            }
        }

        ଯଦି (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            ଯଦି (uprv_strcmp(variant, "PSACCENT") == 0) {
                ଫେରନ୍ତୁ LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } ଅନ୍ୟଥା ଯଦି (uprv_strcmp(variant, "PSBIDI") == 0) {
                ଫେରନ୍ତୁ LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } ଅନ୍ୟଥା ଯଦି (uprv_strcmp(variant, "PSCRACK") == 0) {
                ଫେରନ୍ତୁ LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    ଫେରନ୍ତୁ maximize(language, script, region, return_ନିବେଶ_If_Unmatch, errorCode);
}

LSR LikelySubtags::maximize(ଅଚଳ ଅକ୍ଷର *language, ଅଚଳ ଅକ୍ଷର *script, ଅଚଳ ଅକ୍ଷର *region,
                             ତାର୍କିକ return_ନିବେଶ_If_Unmatch,
                             UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ {}; }
    ଫେରନ୍ତୁ maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_ନିବେଶ_If_Unmatch,
                    errorCode);
}

ତାର୍କିକ LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    ଫେରନ୍ତୁ gMacroregions->contains((ଶୂନ୍ୟ *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             ତାର୍କିକ return_ନିବେଶ_If_Unmatch,
                             UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ {}; }
    ଯଦି (language.compare("und") == 0) {
        language = "";
    }
    ଯଦି (script.compare("Zzzz") == 0) {
        script = "";
    }
    ଯଦି (region.compare("ZZ") == 0) {
        region = "";
    }
    ଯଦି (!script.empty() && !region.empty() && !language.empty()) {
        ଫେରନ୍ତୁ LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    ତାର୍କିକ retainLanguage = ମିଥ୍ୟା;
    ତାର୍କିକ retainScript = ମିଥ୍ୟା;
    ତାର୍କିକ retainRegion = ମିଥ୍ୟା;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    ଯଦି (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } ଅନ୍ୟଥା {
        value = trieNext(iter, language, 0);
    }
    ତାର୍କିକ matchLanguage = (value >= 0);
    ତାର୍କିକ matchScript = ମିଥ୍ୟା;
    ଯଦି (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } ଅନ୍ୟଥା {
        retainLanguage = ସତ୍ୟ;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    ଯଦି (value >= 0 && !script.empty()) {
        matchScript = ସତ୍ୟ;
    }
    ଯଦି (value > 0) {
        // Intermediate or final value from just language.
        ଯଦି (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } ଅନ୍ୟଥା {
        value = trieNext(iter, script, 0);
        ଯଦି (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } ଅନ୍ୟଥା {
            retainScript = ସତ୍ୟ;
            ଯଦି (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } ଅନ୍ୟଥା {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    ତାର୍କିକ matchRegion = ମିଥ୍ୟା;
    ଯଦି (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } ଅନ୍ୟଥା {
        value = trieNext(iter, region, 0);
        ଯଦି (value >= 0) {
            ଯଦି (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = ସତ୍ୟ;
                matchRegion = ସତ୍ୟ;
            }
        } ଅନ୍ୟଥା {
            retainRegion = ସତ୍ୟ;
            ଯଦି (state == 0) {
                value = defaultLsrIndex;
            } ଅନ୍ୟଥା {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                ଯଦି (value < 0) {
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
                    ଯଦି (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    ଯଦି (return_ନିବେଶ_If_Unmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      ଫେରନ୍ତୁ LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    ଯଦି (language.empty()) {
        language = StringPiece("und");
    }

    ଯଦି (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        ଅଚଳ LSR &matched = lsrs[value];
        ଫେରନ୍ତୁ LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    ଯଦି (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    ଯଦି (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    ଯଦି (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    ଫେରନ୍ତୁ LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(ଅଚଳ LSR &lsr, ଅଚଳ LSR &other, int32_t likelyInfo) ଅଚଳ {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    ଯଦି (uprv_strcmp(lsr.language, other.language) != 0) {
        ଫେରନ୍ତୁ 0xfffffffc;  // negative, lsr not better than other
    }
    ଯଦି (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        ଯଦି (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } ଅନ୍ୟଥା {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        ଅଚଳ LSR &likely = lsrs[index];
        ଯଦି (uprv_strcmp(lsr.script, likely.script) == 0) {
            ଫେରନ୍ତୁ likelyInfo | 1;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ likelyInfo & ~1;
        }
    }
    ଯଦି (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        ଯଦି (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } ଅନ୍ୟଥା {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        ଅଚଳ LSR &likely = lsrs[index];
        ଯଦି (uprv_strcmp(lsr.region, likely.region) == 0) {
            ଫେରନ୍ତୁ likelyInfo | 1;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ likelyInfo & ~1;
        }
    }
    ଫେରନ୍ତୁ likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(ଅଚଳ ଅକ୍ଷର *language, ଅଚଳ ଅକ୍ଷର *script) ଅଚଳ {
    ଯଦି (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    ଯଦି (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    ଯଦି (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } ଅନ୍ୟଥା {
        value = trieNext(iter, language, 0);
    }
    ଯଦି (value >= 0) {
        state = iter.getState64();
    } ଅନ୍ୟଥା {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    ଯଦି (value > 0) {
        // Intermediate or final value from just language.
        ଯଦି (value == SKIP_SCRIPT) {
            value = 0;
        }
    } ଅନ୍ୟଥା {
        value = trieNext(iter, script, 0);
        ଯଦି (value >= 0) {
            state = iter.getState64();
        } ଅନ୍ୟଥା {
            ଯଦି (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } ଅନ୍ୟଥା {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    ଯଦି (value > 0) {
        // Final value from just language or language+script.
    } ଅନ୍ୟଥା {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    ଫେରନ୍ତୁ value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, ଅଚଳ ଅକ୍ଷର *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    ଯଦି ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } ଅନ୍ୟଥା {
        ପାଇଁ (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            ଯଦି (next != 0) {
                ଯଦି (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    ଫେରନ୍ତୁ -1;
                }
            } ଅନ୍ୟଥା {
                // last character of this subtag
                result = iter.next(c | 0x80);
                ଭାଙ୍ଗ;
            }
            c = next;
        }
    }
    ବାଛ (result) {
    ସ୍ଥିତି USTRINGTRIE_NO_MATCH: ଫେରନ୍ତୁ -1;
    ସ୍ଥିତି USTRINGTRIE_NO_ମୂଲ୍ୟ: ଫେରନ୍ତୁ 0;
    ସ୍ଥିତି USTRINGTRIE_INTERMEDIATE_ମୂଲ୍ୟ:
        U_ASSERT(iter.get_ମୂଲ୍ୟ() == SKIP_SCRIPT);
        ଫେରନ୍ତୁ SKIP_SCRIPT;
    ସ୍ଥିତି USTRINGTRIE_FINAL_ମୂଲ୍ୟ: ଫେରନ୍ତୁ iter.get_ମୂଲ୍ୟ();
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    ଯଦି (s.length() == i) {
        result = iter.next(u'*');
    } ଅନ୍ୟଥା {
        c = s.data()[i];
        ପାଇଁ (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            ଯଦି (i+1 != s.length()) {
                ଯଦି (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    ଫେରନ୍ତୁ -1;
                }
                c = s.data()[++i];
            } ଅନ୍ୟଥା {
                // last character of this subtag
                result = iter.next(c | 0x80);
                ଭାଙ୍ଗ;
            }
        }
    }
    ବାଛ (result) {
    ସ୍ଥିତି USTRINGTRIE_NO_MATCH: ଫେରନ୍ତୁ -1;
    ସ୍ଥିତି USTRINGTRIE_NO_ମୂଲ୍ୟ: ଫେରନ୍ତୁ 0;
    ସ୍ଥିତି USTRINGTRIE_INTERMEDIATE_ମୂଲ୍ୟ:
        U_ASSERT(iter.get_ମୂଲ୍ୟ() == SKIP_SCRIPT);
        ଫେରନ୍ତୁ SKIP_SCRIPT;
    ସ୍ଥିତି USTRINGTRIE_FINAL_ମୂଲ୍ୟ: ଫେରନ୍ତୁ iter.get_ମୂଲ୍ୟ();
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    ତାର୍କିକ favorScript,
                                    UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ {}; }
    LSR max = maximize(language, script, region, ସତ୍ୟ, errorCode);
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ {}; }
    // If no match, return it.
    ଯଦି (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        ଫେରନ୍ତୁ LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", ସତ୍ୟ, errorCode);
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ {}; }
    ଯଦି (test.isEquivalentTo(max)) {
        ଫେରନ୍ତୁ LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    ଯଦି (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, ସତ୍ୟ, errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ {}; }
        ଯଦି (test.isEquivalentTo(max)) {
            ଫେରନ୍ତୁ LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", ସତ୍ୟ, errorCode);
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ {}; }
    ଯଦି (test.isEquivalentTo(max)) {
        ଫେରନ୍ତୁ LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    ଯଦି (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, ସତ୍ୟ, errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ {}; }
        ଯଦି (test.isEquivalentTo(max)) {
            ଫେରନ୍ତୁ LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    ଫେରନ୍ତୁ LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
