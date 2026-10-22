// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#அந்தர்பாவ <utility>
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/bytestrie.h"
#அந்தர்பாவ "unicode/localpointer.h"
#அந்தர்பாவ "unicode/locid.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "unicode/ures.h"
#அந்தர்பாவ "unicode/uscript.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "loclikelysubtags.h"
#அந்தர்பாவ "lsr.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "uinvchar.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "uniquecharstr.h"
#அந்தர்பாவ "uresdata.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "uvector.h"

U_NAMESPACE_BEGIN

நாமாகாஶ {

constexpr எழுத்து PSEUDO_ACCENTS_PREFIX = '\'';  // -XA, -PSACCENT
constexpr எழுத்து PSEUDO_BIDI_PREFIX = '+';  // -XB, -PSBIDI
constexpr எழுத்து PSEUDO_CRACKED_PREFIX = ',';  // -XC, -PSCRACK

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

கட்டமைப்பு LikelySubtagsData {
    UResourceBundle *langInfoBundle = nullptr;
    UniqueCharStrings strings;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    மாறிலி uint8_t *trieBytes = nullptr;
    LSR *lsrs = nullptr;
    int32_t lsrsLength = 0;

    LocaleDistanceData distanceData;

    LikelySubtagsData(UErrorCode &errorCode) : strings(errorCode) {}

    ~LikelySubtagsData() {
        ures_close(langInfoBundle);
        delete[] lsrs;
    }

    வெறுமை load(UErrorCode &errorCode) {
        எனில் (U_FAILURE(errorCode)) { திரும்பு; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        எனில் (U_FAILURE(errorCode)) { திரும்பு; }
        அடுக்கு_UResource_Bundle அடுக்கு_Temp_Bundle;
        Resource_Data_மதிப்பு value;
        ures_get_மதிப்பு_With_Fallback(langInfoBundle, "likely", அடுக்கு_Temp_Bundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        எனில் (U_FAILURE(errorCode)) { திரும்பு; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        எனில் (likelyTable.find_மதிப்பு("m49", value)) {
            m49Array = value.getArray(errorCode);
        } இல்லையெனில் {
            errorCode = U_MISSING_RESOURCE_ERROR;
            திரும்பு;
        }
        எனில் (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            திரும்பு;
        }
        எனில் ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            திரும்பு;
        }
        எனில் (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            திரும்பு;
        }

        எனில் (!likelyTable.find_மதிப்பு("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            திரும்பு;
        }
        int32_t length;
        trieBytes = value.get_இருமம்(length, errorCode);
        எனில் (U_FAILURE(errorCode)) { திரும்பு; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_get_மதிப்பு_With_Fallback(langInfoBundle, "match", அடுக்கு_Temp_Bundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        எனில் (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            எனில் (U_FAILURE(errorCode)) { திரும்பு; }

            எனில் (matchTable.find_மதிப்பு("trie", value)) {
                distanceData.distanceTrieBytes = value.get_இருமம்(length, errorCode);
                எனில் (U_FAILURE(errorCode)) { திரும்பு; }
            }

            எனில் (matchTable.find_மதிப்பு("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.get_இருமம்(length, errorCode);
                எனில் (U_FAILURE(errorCode)) { திரும்பு; }
                எனில் (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    திரும்பு;
                }
            }

            எனில் (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                திரும்பு;
            }
            எனில் ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                திரும்பு;
            }

            எனில் (matchTable.find_மதிப்பு("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                எனில் (U_FAILURE(errorCode)) { திரும்பு; }
                எனில் (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    திரும்பு;
                }
            }
        } இல்லையெனில் எனில் (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } இல்லையெனில் {  // error other than missing resource
            errorCode = matchErrorCode;
            திரும்பு;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languagesLength / 2, errorCode);
        க்கு (int32_t i = 0; i < languagesLength; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regionsLength / 2, errorCode);
        க்கு (int32_t i = 0; i < regionsLength; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        எனில் (U_FAILURE(errorCode)) { திரும்பு; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = புதிய LSR[lsrsLength];
        எனில் (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு;
        }
        க்கு (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        எனில் (partitionsLength > 0) {
            distanceData.partitions = static_cast<மாறிலி எழுத்து **>(
                uprv_malloc(partitionsLength * அளவு(மாறிலி எழுத்து *)));
            எனில் (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு;
            }
            க்கு (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        எனில் (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = புதிய LSR[distanceData.paradigmsLength];
            எனில் (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு;
            }
            க்கு (int32_t i = 0, j = 0; i < paradigmSubtagsLength; i += 3, ++j) {
                paradigms[j] = LSR(strings.get(paradigmSubtagIndexes[i]),
                                   strings.get(paradigmSubtagIndexes[i + 1]),
                                   strings.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            distanceData.paradigms = paradigms;
        }
    }

தனிப்பட்ட:
    தருக்க readStrings(மாறிலி ResourceTable &table, மாறிலி எழுத்து *சாவி, Resource_மதிப்பு &value,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
        எனில் (table.find_மதிப்பு(சாவி, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
            length = stringArray.getSize();
            எனில் (length == 0) { திரும்பு உண்மை; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            எனில் (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு பொய்;
            }
            க்கு (முழு i = 0; i < length; ++i) {
                எனில் (stringArray.get_மதிப்பு(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
                }
            }
        }
        திரும்பு உண்மை;
    }
    UnicodeString toLanguage(முழு encoded) {
        எனில் (encoded == 0) {
            திரும்பு UNICODE_STRING_SIMPLE("");
        }
        எனில் (encoded == 1) {
            திரும்பு UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        எழுத்து lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        எனில் (encoded / (27 * 27) == 0) {
            திரும்பு UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        திரும்பு UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(முழு encoded) {
        எனில் (encoded == 0) {
            திரும்பு UNICODE_STRING_SIMPLE("");
        }
        எனில் (encoded == 1) {
            திரும்பு UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        மாறிலி எழுத்து* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        எனில் (script == nullptr) {
            திரும்பு UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        திரும்பு UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(மாறிலி ResourceArray &m49Array, Resource_மதிப்பு &value, முழு index, UErrorCode &errorCode) {
        எனில் (U_FAILURE(errorCode)) {
            திரும்பு UNICODE_STRING_SIMPLE("");
        }
        எனில் (m49Array.get_மதிப்பு(index, value)) {
            திரும்பு value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        திரும்பு UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(மாறிலி ResourceArray& m49Array, Resource_மதிப்பு &value, முழு encoded, UErrorCode &errorCode) {
        எனில் (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            திரும்பு UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        எனில் (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            திரும்பு  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        எழுத்து region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        திரும்பு UnicodeString(region, 2, US_INV);
    }

    தருக்க readLSREncodedStrings(மாறிலி ResourceTable &table, மாறிலி எழுத்து* சாவி, Resource_மதிப்பு &value, மாறிலி ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
        எனில் (table.find_மதிப்பு(சாவி, value)) {
            மாறிலி int32_t* vectors = value.getIntVector(length, errorCode);
            எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
            எனில் (length == 0) { திரும்பு உண்மை; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            எனில் (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு பொய்;
            }
            க்கு (முழு i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.add_By_மதிப்பு(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.add_By_மதிப்பு(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.add_By_மதிப்பு(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
            }
            length *= 3;
        }
        திரும்பு உண்மை;
    }
};

நாமாகாஶ {

LikelySubtags *gLikelySubtags = nullptr;
UVector *gMacroregions = nullptr;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    delete gLikelySubtags;
    gLikelySubtags = nullptr;
    delete gMacroregions;
    gMacroregions = nullptr;
    gInitOnce.reset();
    திரும்பு உண்மை;
}

constexpr மாறிலி char16_t* MACROREGION_HARDCODE[] = {
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
வெறுமை processMacroregionRange(மாறிலி UnicodeString& regionName, UVector* newMacroRegions, UErrorCode& status) {
    எனில் (U_FAILURE(status)) { திரும்பு; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    எனில் ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        போது ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(புதிய UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } இல்லையெனில் {
        LocalPointer<UnicodeString> newRegion(புதிய UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#எனில் U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    எனில் (U_FAILURE(status)) { திரும்பு nullptr; }
    LocalPointer<UVector> newMacroRegions(புதிய UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_get_By_சாவி(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_get_By_சாவி(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_get_By_சாவி(regionList.getAlias(),"macroregion",nullptr,&status));

    எனில் (U_FAILURE(status)) {
        திரும்பு nullptr;
    }

    போது (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        எனில் (U_FAILURE(status)) {
            திரும்பு nullptr;
        }
    }

    திரும்பு newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    எனில் (U_FAILURE(status)) { திரும்பு nullptr; }
    LocalPointer<UVector> newMacroRegions(புதிய UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    எனில் (U_FAILURE(status)) {
        திரும்பு nullptr;
    }

    க்கு (மாறிலி auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        எனில் (U_FAILURE(status)) {
            திரும்பு nullptr;
        }
    }

    திரும்பு newMacroRegions.orphan();
}

}  // namespace

வெறுமை U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    gLikelySubtags = புதிய LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#எனில் U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    எனில் (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

மாறிலி LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    திரும்பு gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#எனில் U_DEBUG
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
    U_ASSERT(USTRINGTRIE_HAS_மதிப்பு(result));
    defaultLsrIndex = trie.get_மதிப்பு();
    trie.reset();

    க்கு (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        எனில் (result == USTRINGTRIE_NO_மதிப்பு) {
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

LSR LikelySubtags::makeMaximizedLsrFrom(மாறிலி Locale &locale,
                                         தருக்க return_உள்ளீடு_If_Unmatch,
                                         UErrorCode &errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) { திரும்பு {}; }
    எனில் (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு {};
    }
    மாறிலி எழுத்து *name = locale.getName();
    எனில் (!return_உள்ளீடு_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        திரும்பு LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_உள்ளீடு_If_Unmatch, errorCode);

    எனில் (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        திரும்பு LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    திரும்பு max;
}

நாமாகாஶ {

மாறிலி எழுத்து *getCanonical(மாறிலி CharStringMap &aliases, மாறிலி எழுத்து *alias) {
    மாறிலி எழுத்து *canonical = aliases.get(alias);
    திரும்பு canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(மாறிலி எழுத்து *language, மாறிலி எழுத்து *script, மாறிலி எழுத்து *region,
                                     மாறிலி எழுத்து *variant,
                                     தருக்க return_உள்ளீடு_If_Unmatch,
                                     UErrorCode &errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) { திரும்பு {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    எனில் (!return_உள்ளீடு_If_Unmatch) {
        எழுத்து c1;
        எனில் (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            தேர்வு (c1) {
            நிலை 'A':
                திரும்பு LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            நிலை 'B':
                திரும்பு LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            நிலை 'C':
                திரும்பு LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            இயல்பு:  // normal locale
                நிறுத்து;
            }
        }

        எனில் (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            எனில் (uprv_strcmp(variant, "PSACCENT") == 0) {
                திரும்பு LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } இல்லையெனில் எனில் (uprv_strcmp(variant, "PSBIDI") == 0) {
                திரும்பு LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } இல்லையெனில் எனில் (uprv_strcmp(variant, "PSCRACK") == 0) {
                திரும்பு LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    திரும்பு maximize(language, script, region, return_உள்ளீடு_If_Unmatch, errorCode);
}

LSR LikelySubtags::maximize(மாறிலி எழுத்து *language, மாறிலி எழுத்து *script, மாறிலி எழுத்து *region,
                             தருக்க return_உள்ளீடு_If_Unmatch,
                             UErrorCode &errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) { திரும்பு {}; }
    திரும்பு maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_உள்ளீடு_If_Unmatch,
                    errorCode);
}

தருக்க LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    திரும்பு gMacroregions->contains((வெறுமை *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             தருக்க return_உள்ளீடு_If_Unmatch,
                             UErrorCode &errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) { திரும்பு {}; }
    எனில் (language.compare("und") == 0) {
        language = "";
    }
    எனில் (script.compare("Zzzz") == 0) {
        script = "";
    }
    எனில் (region.compare("ZZ") == 0) {
        region = "";
    }
    எனில் (!script.empty() && !region.empty() && !language.empty()) {
        திரும்பு LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    தருக்க retainLanguage = பொய்;
    தருக்க retainScript = பொய்;
    தருக்க retainRegion = பொய்;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    எனில் (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } இல்லையெனில் {
        value = trieNext(iter, language, 0);
    }
    தருக்க matchLanguage = (value >= 0);
    தருக்க matchScript = பொய்;
    எனில் (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } இல்லையெனில் {
        retainLanguage = உண்மை;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    எனில் (value >= 0 && !script.empty()) {
        matchScript = உண்மை;
    }
    எனில் (value > 0) {
        // Intermediate or final value from just language.
        எனில் (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } இல்லையெனில் {
        value = trieNext(iter, script, 0);
        எனில் (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } இல்லையெனில் {
            retainScript = உண்மை;
            எனில் (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } இல்லையெனில் {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    தருக்க matchRegion = பொய்;
    எனில் (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } இல்லையெனில் {
        value = trieNext(iter, region, 0);
        எனில் (value >= 0) {
            எனில் (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = உண்மை;
                matchRegion = உண்மை;
            }
        } இல்லையெனில் {
            retainRegion = உண்மை;
            எனில் (state == 0) {
                value = defaultLsrIndex;
            } இல்லையெனில் {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                எனில் (value < 0) {
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
                    எனில் (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    எனில் (return_உள்ளீடு_If_Unmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      திரும்பு LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    எனில் (language.empty()) {
        language = StringPiece("und");
    }

    எனில் (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        மாறிலி LSR &matched = lsrs[value];
        திரும்பு LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    எனில் (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    எனில் (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    எனில் (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    திரும்பு LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(மாறிலி LSR &lsr, மாறிலி LSR &other, int32_t likelyInfo) மாறிலி {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    எனில் (uprv_strcmp(lsr.language, other.language) != 0) {
        திரும்பு 0xfffffffc;  // negative, lsr not better than other
    }
    எனில் (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        எனில் (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } இல்லையெனில் {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        மாறிலி LSR &likely = lsrs[index];
        எனில் (uprv_strcmp(lsr.script, likely.script) == 0) {
            திரும்பு likelyInfo | 1;
        } இல்லையெனில் {
            திரும்பு likelyInfo & ~1;
        }
    }
    எனில் (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        எனில் (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } இல்லையெனில் {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        மாறிலி LSR &likely = lsrs[index];
        எனில் (uprv_strcmp(lsr.region, likely.region) == 0) {
            திரும்பு likelyInfo | 1;
        } இல்லையெனில் {
            திரும்பு likelyInfo & ~1;
        }
    }
    திரும்பு likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(மாறிலி எழுத்து *language, மாறிலி எழுத்து *script) மாறிலி {
    எனில் (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    எனில் (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    எனில் (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } இல்லையெனில் {
        value = trieNext(iter, language, 0);
    }
    எனில் (value >= 0) {
        state = iter.getState64();
    } இல்லையெனில் {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    எனில் (value > 0) {
        // Intermediate or final value from just language.
        எனில் (value == SKIP_SCRIPT) {
            value = 0;
        }
    } இல்லையெனில் {
        value = trieNext(iter, script, 0);
        எனில் (value >= 0) {
            state = iter.getState64();
        } இல்லையெனில் {
            எனில் (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } இல்லையெனில் {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    எனில் (value > 0) {
        // Final value from just language or language+script.
    } இல்லையெனில் {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    திரும்பு value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, மாறிலி எழுத்து *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    எனில் ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } இல்லையெனில் {
        க்கு (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            எனில் (next != 0) {
                எனில் (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    திரும்பு -1;
                }
            } இல்லையெனில் {
                // last character of this subtag
                result = iter.next(c | 0x80);
                நிறுத்து;
            }
            c = next;
        }
    }
    தேர்வு (result) {
    நிலை USTRINGTRIE_NO_MATCH: திரும்பு -1;
    நிலை USTRINGTRIE_NO_மதிப்பு: திரும்பு 0;
    நிலை USTRINGTRIE_INTERMEDIATE_மதிப்பு:
        U_ASSERT(iter.get_மதிப்பு() == SKIP_SCRIPT);
        திரும்பு SKIP_SCRIPT;
    நிலை USTRINGTRIE_FINAL_மதிப்பு: திரும்பு iter.get_மதிப்பு();
    இயல்பு: திரும்பு -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    எனில் (s.length() == i) {
        result = iter.next(u'*');
    } இல்லையெனில் {
        c = s.data()[i];
        க்கு (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            எனில் (i+1 != s.length()) {
                எனில் (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    திரும்பு -1;
                }
                c = s.data()[++i];
            } இல்லையெனில் {
                // last character of this subtag
                result = iter.next(c | 0x80);
                நிறுத்து;
            }
        }
    }
    தேர்வு (result) {
    நிலை USTRINGTRIE_NO_MATCH: திரும்பு -1;
    நிலை USTRINGTRIE_NO_மதிப்பு: திரும்பு 0;
    நிலை USTRINGTRIE_INTERMEDIATE_மதிப்பு:
        U_ASSERT(iter.get_மதிப்பு() == SKIP_SCRIPT);
        திரும்பு SKIP_SCRIPT;
    நிலை USTRINGTRIE_FINAL_மதிப்பு: திரும்பு iter.get_மதிப்பு();
    இயல்பு: திரும்பு -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    தருக்க favorScript,
                                    UErrorCode &errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) { திரும்பு {}; }
    LSR max = maximize(language, script, region, உண்மை, errorCode);
    எனில் (U_FAILURE(errorCode)) { திரும்பு {}; }
    // If no match, return it.
    எனில் (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        திரும்பு LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", உண்மை, errorCode);
    எனில் (U_FAILURE(errorCode)) { திரும்பு {}; }
    எனில் (test.isEquivalentTo(max)) {
        திரும்பு LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    எனில் (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, உண்மை, errorCode);
        எனில் (U_FAILURE(errorCode)) { திரும்பு {}; }
        எனில் (test.isEquivalentTo(max)) {
            திரும்பு LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", உண்மை, errorCode);
    எனில் (U_FAILURE(errorCode)) { திரும்பு {}; }
    எனில் (test.isEquivalentTo(max)) {
        திரும்பு LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    எனில் (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, உண்மை, errorCode);
        எனில் (U_FAILURE(errorCode)) { திரும்பு {}; }
        எனில் (test.isEquivalentTo(max)) {
            திரும்பு LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    திரும்பு LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
