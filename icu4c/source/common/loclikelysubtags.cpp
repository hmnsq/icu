// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#شامل <utility>
#شامل "unicode/utypes.h"
#شامل "unicode/bytestrie.h"
#شامل "unicode/localpointer.h"
#شامل "unicode/locid.h"
#شامل "unicode/uobject.h"
#شامل "unicode/ures.h"
#شامل "unicode/uscript.h"
#شامل "charstr.h"
#شامل "cstring.h"
#شامل "loclikelysubtags.h"
#شامل "lsr.h"
#شامل "uassert.h"
#شامل "ucln_cmn.h"
#شامل "uhash.h"
#شامل "uinvchar.h"
#شامل "umutex.h"
#شامل "uniquecharstr.h"
#شامل "uresdata.h"
#شامل "uresimp.h"
#شامل "uvector.h"

U_NAMESPACE_BEGIN

فضاینام {

constexpr نویسه PSEUDO_ACCENTS_PREFIX = '\'';  // -XA, -PSACCENT
constexpr نویسه PSEUDO_BIDI_PREFIX = '+';  // -XB, -PSBIDI
constexpr نویسه PSEUDO_CRACKED_PREFIX = ',';  // -XC, -PSCRACK

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

ساختار LikelySubtagsData {
    UResourceBundle *langInfoBundle = nullptr;
    UniqueCharStrings strings;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    ثابت uint8_t *trieBytes = nullptr;
    LSR *lsrs = nullptr;
    int32_t lsrsLength = 0;

    LocaleDistanceData distanceData;

    LikelySubtagsData(UErrorCode &errorCode) : strings(errorCode) {}

    ~LikelySubtagsData() {
        ures_close(langInfoBundle);
        delete[] lsrs;
    }

    تهی load(UErrorCode &errorCode) {
        اگر (U_FAILURE(errorCode)) { بازگشت; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت; }
        پشته_UResource_Bundle پشته_Temp_Bundle;
        Resource_Data_مقدار value;
        ures_get_مقدار_With_Fallback(langInfoBundle, "likely", پشته_Temp_Bundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        اگر (likelyTable.find_مقدار("m49", value)) {
            m49Array = value.getArray(errorCode);
        } وگرنه {
            errorCode = U_MISSING_RESOURCE_ERROR;
            بازگشت;
        }
        اگر (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            بازگشت;
        }
        اگر ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            بازگشت;
        }
        اگر (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            بازگشت;
        }

        اگر (!likelyTable.find_مقدار("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            بازگشت;
        }
        int32_t length;
        trieBytes = value.get_دودویی(length, errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_get_مقدار_With_Fallback(langInfoBundle, "match", پشته_Temp_Bundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        اگر (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            اگر (U_FAILURE(errorCode)) { بازگشت; }

            اگر (matchTable.find_مقدار("trie", value)) {
                distanceData.distanceTrieBytes = value.get_دودویی(length, errorCode);
                اگر (U_FAILURE(errorCode)) { بازگشت; }
            }

            اگر (matchTable.find_مقدار("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.get_دودویی(length, errorCode);
                اگر (U_FAILURE(errorCode)) { بازگشت; }
                اگر (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    بازگشت;
                }
            }

            اگر (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                بازگشت;
            }
            اگر ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                بازگشت;
            }

            اگر (matchTable.find_مقدار("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                اگر (U_FAILURE(errorCode)) { بازگشت; }
                اگر (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    بازگشت;
                }
            }
        } وگرنه اگر (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } وگرنه {  // error other than missing resource
            errorCode = matchErrorCode;
            بازگشت;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languagesLength / 2, errorCode);
        برای (int32_t i = 0; i < languagesLength; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regionsLength / 2, errorCode);
        برای (int32_t i = 0; i < regionsLength; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        اگر (U_FAILURE(errorCode)) { بازگشت; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = جدید LSR[lsrsLength];
        اگر (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            بازگشت;
        }
        برای (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        اگر (partitionsLength > 0) {
            distanceData.partitions = static_cast<ثابت نویسه **>(
                uprv_malloc(partitionsLength * اندازه(ثابت نویسه *)));
            اگر (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                بازگشت;
            }
            برای (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        اگر (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = جدید LSR[distanceData.paradigmsLength];
            اگر (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                بازگشت;
            }
            برای (int32_t i = 0, j = 0; i < paradigmSubtagsLength; i += 3, ++j) {
                paradigms[j] = LSR(strings.get(paradigmSubtagIndexes[i]),
                                   strings.get(paradigmSubtagIndexes[i + 1]),
                                   strings.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            distanceData.paradigms = paradigms;
        }
    }

خصوصی:
    منطقی readStrings(ثابت ResourceTable &table, ثابت نویسه *کلید, Resource_مقدار &value,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        اگر (U_FAILURE(errorCode)) { بازگشت نادرست; }
        اگر (table.find_مقدار(کلید, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            اگر (U_FAILURE(errorCode)) { بازگشت نادرست; }
            length = stringArray.getSize();
            اگر (length == 0) { بازگشت درست; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            اگر (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                بازگشت نادرست;
            }
            برای (صحیح i = 0; i < length; ++i) {
                اگر (stringArray.get_مقدار(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    اگر (U_FAILURE(errorCode)) { بازگشت نادرست; }
                }
            }
        }
        بازگشت درست;
    }
    UnicodeString toLanguage(صحیح encoded) {
        اگر (encoded == 0) {
            بازگشت UNICODE_STRING_SIMPLE("");
        }
        اگر (encoded == 1) {
            بازگشت UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        نویسه lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        اگر (encoded / (27 * 27) == 0) {
            بازگشت UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        بازگشت UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(صحیح encoded) {
        اگر (encoded == 0) {
            بازگشت UNICODE_STRING_SIMPLE("");
        }
        اگر (encoded == 1) {
            بازگشت UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        ثابت نویسه* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        اگر (script == nullptr) {
            بازگشت UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        بازگشت UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(ثابت ResourceArray &m49Array, Resource_مقدار &value, صحیح index, UErrorCode &errorCode) {
        اگر (U_FAILURE(errorCode)) {
            بازگشت UNICODE_STRING_SIMPLE("");
        }
        اگر (m49Array.get_مقدار(index, value)) {
            بازگشت value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        بازگشت UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(ثابت ResourceArray& m49Array, Resource_مقدار &value, صحیح encoded, UErrorCode &errorCode) {
        اگر (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            بازگشت UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        اگر (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            بازگشت  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        نویسه region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        بازگشت UnicodeString(region, 2, US_INV);
    }

    منطقی readLSREncodedStrings(ثابت ResourceTable &table, ثابت نویسه* کلید, Resource_مقدار &value, ثابت ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        اگر (U_FAILURE(errorCode)) { بازگشت نادرست; }
        اگر (table.find_مقدار(کلید, value)) {
            ثابت int32_t* vectors = value.getIntVector(length, errorCode);
            اگر (U_FAILURE(errorCode)) { بازگشت نادرست; }
            اگر (length == 0) { بازگشت درست; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            اگر (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                بازگشت نادرست;
            }
            برای (صحیح i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.add_By_مقدار(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.add_By_مقدار(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.add_By_مقدار(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                اگر (U_FAILURE(errorCode)) { بازگشت نادرست; }
            }
            length *= 3;
        }
        بازگشت درست;
    }
};

فضاینام {

LikelySubtags *gLikelySubtags = nullptr;
UVector *gMacroregions = nullptr;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    delete gLikelySubtags;
    gLikelySubtags = nullptr;
    delete gMacroregions;
    gMacroregions = nullptr;
    gInitOnce.reset();
    بازگشت درست;
}

constexpr ثابت char16_t* MACROREGION_HARDCODE[] = {
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
تهی processMacroregionRange(ثابت UnicodeString& regionName, UVector* newMacroRegions, UErrorCode& status) {
    اگر (U_FAILURE(status)) { بازگشت; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    اگر ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        وقتی ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(جدید UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } وگرنه {
        LocalPointer<UnicodeString> newRegion(جدید UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#اگر U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    اگر (U_FAILURE(status)) { بازگشت nullptr; }
    LocalPointer<UVector> newMacroRegions(جدید UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_get_By_کلید(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_get_By_کلید(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_get_By_کلید(regionList.getAlias(),"macroregion",nullptr,&status));

    اگر (U_FAILURE(status)) {
        بازگشت nullptr;
    }

    وقتی (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        اگر (U_FAILURE(status)) {
            بازگشت nullptr;
        }
    }

    بازگشت newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    اگر (U_FAILURE(status)) { بازگشت nullptr; }
    LocalPointer<UVector> newMacroRegions(جدید UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    اگر (U_FAILURE(status)) {
        بازگشت nullptr;
    }

    برای (ثابت auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        اگر (U_FAILURE(status)) {
            بازگشت nullptr;
        }
    }

    بازگشت newMacroRegions.orphan();
}

}  // namespace

تهی U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    اگر (U_FAILURE(errorCode)) { بازگشت; }
    gLikelySubtags = جدید LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#اگر U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    اگر (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ثابت LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    بازگشت gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#اگر U_DEBUG
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
    U_ASSERT(USTRINGTRIE_HAS_مقدار(result));
    defaultLsrIndex = trie.get_مقدار();
    trie.reset();

    برای (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        اگر (result == USTRINGTRIE_NO_مقدار) {
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

LSR LikelySubtags::makeMaximizedLsrFrom(ثابت Locale &locale,
                                         منطقی return_درون_داد_If_Unmatch,
                                         UErrorCode &errorCode) ثابت {
    اگر (U_FAILURE(errorCode)) { بازگشت {}; }
    اگر (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت {};
    }
    ثابت نویسه *name = locale.getName();
    اگر (!return_درون_داد_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        بازگشت LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_درون_داد_If_Unmatch, errorCode);

    اگر (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        بازگشت LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    بازگشت max;
}

فضاینام {

ثابت نویسه *getCanonical(ثابت CharStringMap &aliases, ثابت نویسه *alias) {
    ثابت نویسه *canonical = aliases.get(alias);
    بازگشت canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(ثابت نویسه *language, ثابت نویسه *script, ثابت نویسه *region,
                                     ثابت نویسه *variant,
                                     منطقی return_درون_داد_If_Unmatch,
                                     UErrorCode &errorCode) ثابت {
    اگر (U_FAILURE(errorCode)) { بازگشت {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    اگر (!return_درون_داد_If_Unmatch) {
        نویسه c1;
        اگر (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            انتخاب (c1) {
            حالت 'A':
                بازگشت LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            حالت 'B':
                بازگشت LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            حالت 'C':
                بازگشت LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            پیشفرض:  // normal locale
                بشکن;
            }
        }

        اگر (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            اگر (uprv_strcmp(variant, "PSACCENT") == 0) {
                بازگشت LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } وگرنه اگر (uprv_strcmp(variant, "PSBIDI") == 0) {
                بازگشت LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } وگرنه اگر (uprv_strcmp(variant, "PSCRACK") == 0) {
                بازگشت LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    بازگشت maximize(language, script, region, return_درون_داد_If_Unmatch, errorCode);
}

LSR LikelySubtags::maximize(ثابت نویسه *language, ثابت نویسه *script, ثابت نویسه *region,
                             منطقی return_درون_داد_If_Unmatch,
                             UErrorCode &errorCode) ثابت {
    اگر (U_FAILURE(errorCode)) { بازگشت {}; }
    بازگشت maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_درون_داد_If_Unmatch,
                    errorCode);
}

منطقی LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) ثابت {
    اگر (U_FAILURE(errorCode)) { بازگشت نادرست; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    اگر (U_FAILURE(errorCode)) { بازگشت نادرست; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    بازگشت gMacroregions->contains((تهی *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             منطقی return_درون_داد_If_Unmatch,
                             UErrorCode &errorCode) ثابت {
    اگر (U_FAILURE(errorCode)) { بازگشت {}; }
    اگر (language.compare("und") == 0) {
        language = "";
    }
    اگر (script.compare("Zzzz") == 0) {
        script = "";
    }
    اگر (region.compare("ZZ") == 0) {
        region = "";
    }
    اگر (!script.empty() && !region.empty() && !language.empty()) {
        بازگشت LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    منطقی retainLanguage = نادرست;
    منطقی retainScript = نادرست;
    منطقی retainRegion = نادرست;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    اگر (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } وگرنه {
        value = trieNext(iter, language, 0);
    }
    منطقی matchLanguage = (value >= 0);
    منطقی matchScript = نادرست;
    اگر (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } وگرنه {
        retainLanguage = درست;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    اگر (value >= 0 && !script.empty()) {
        matchScript = درست;
    }
    اگر (value > 0) {
        // Intermediate or final value from just language.
        اگر (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } وگرنه {
        value = trieNext(iter, script, 0);
        اگر (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } وگرنه {
            retainScript = درست;
            اگر (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } وگرنه {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    منطقی matchRegion = نادرست;
    اگر (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } وگرنه {
        value = trieNext(iter, region, 0);
        اگر (value >= 0) {
            اگر (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = درست;
                matchRegion = درست;
            }
        } وگرنه {
            retainRegion = درست;
            اگر (state == 0) {
                value = defaultLsrIndex;
            } وگرنه {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                اگر (value < 0) {
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
                    اگر (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    اگر (return_درون_داد_If_Unmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      بازگشت LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    اگر (language.empty()) {
        language = StringPiece("und");
    }

    اگر (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        ثابت LSR &matched = lsrs[value];
        بازگشت LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    اگر (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    اگر (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    اگر (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    بازگشت LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(ثابت LSR &lsr, ثابت LSR &other, int32_t likelyInfo) ثابت {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    اگر (uprv_strcmp(lsr.language, other.language) != 0) {
        بازگشت 0xfffffffc;  // negative, lsr not better than other
    }
    اگر (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        اگر (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } وگرنه {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        ثابت LSR &likely = lsrs[index];
        اگر (uprv_strcmp(lsr.script, likely.script) == 0) {
            بازگشت likelyInfo | 1;
        } وگرنه {
            بازگشت likelyInfo & ~1;
        }
    }
    اگر (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        اگر (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } وگرنه {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        ثابت LSR &likely = lsrs[index];
        اگر (uprv_strcmp(lsr.region, likely.region) == 0) {
            بازگشت likelyInfo | 1;
        } وگرنه {
            بازگشت likelyInfo & ~1;
        }
    }
    بازگشت likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(ثابت نویسه *language, ثابت نویسه *script) ثابت {
    اگر (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    اگر (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    اگر (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } وگرنه {
        value = trieNext(iter, language, 0);
    }
    اگر (value >= 0) {
        state = iter.getState64();
    } وگرنه {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    اگر (value > 0) {
        // Intermediate or final value from just language.
        اگر (value == SKIP_SCRIPT) {
            value = 0;
        }
    } وگرنه {
        value = trieNext(iter, script, 0);
        اگر (value >= 0) {
            state = iter.getState64();
        } وگرنه {
            اگر (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } وگرنه {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    اگر (value > 0) {
        // Final value from just language or language+script.
    } وگرنه {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    بازگشت value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, ثابت نویسه *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    اگر ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } وگرنه {
        برای (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            اگر (next != 0) {
                اگر (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    بازگشت -1;
                }
            } وگرنه {
                // last character of this subtag
                result = iter.next(c | 0x80);
                بشکن;
            }
            c = next;
        }
    }
    انتخاب (result) {
    حالت USTRINGTRIE_NO_MATCH: بازگشت -1;
    حالت USTRINGTRIE_NO_مقدار: بازگشت 0;
    حالت USTRINGTRIE_INTERMEDIATE_مقدار:
        U_ASSERT(iter.get_مقدار() == SKIP_SCRIPT);
        بازگشت SKIP_SCRIPT;
    حالت USTRINGTRIE_FINAL_مقدار: بازگشت iter.get_مقدار();
    پیشفرض: بازگشت -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    اگر (s.length() == i) {
        result = iter.next(u'*');
    } وگرنه {
        c = s.data()[i];
        برای (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            اگر (i+1 != s.length()) {
                اگر (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    بازگشت -1;
                }
                c = s.data()[++i];
            } وگرنه {
                // last character of this subtag
                result = iter.next(c | 0x80);
                بشکن;
            }
        }
    }
    انتخاب (result) {
    حالت USTRINGTRIE_NO_MATCH: بازگشت -1;
    حالت USTRINGTRIE_NO_مقدار: بازگشت 0;
    حالت USTRINGTRIE_INTERMEDIATE_مقدار:
        U_ASSERT(iter.get_مقدار() == SKIP_SCRIPT);
        بازگشت SKIP_SCRIPT;
    حالت USTRINGTRIE_FINAL_مقدار: بازگشت iter.get_مقدار();
    پیشفرض: بازگشت -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    منطقی favorScript,
                                    UErrorCode &errorCode) ثابت {
    اگر (U_FAILURE(errorCode)) { بازگشت {}; }
    LSR max = maximize(language, script, region, درست, errorCode);
    اگر (U_FAILURE(errorCode)) { بازگشت {}; }
    // If no match, return it.
    اگر (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        بازگشت LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", درست, errorCode);
    اگر (U_FAILURE(errorCode)) { بازگشت {}; }
    اگر (test.isEquivalentTo(max)) {
        بازگشت LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    اگر (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, درست, errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت {}; }
        اگر (test.isEquivalentTo(max)) {
            بازگشت LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", درست, errorCode);
    اگر (U_FAILURE(errorCode)) { بازگشت {}; }
    اگر (test.isEquivalentTo(max)) {
        بازگشت LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    اگر (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, درست, errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت {}; }
        اگر (test.isEquivalentTo(max)) {
            بازگشت LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    بازگشت LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
