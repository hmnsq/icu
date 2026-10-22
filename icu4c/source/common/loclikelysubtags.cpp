// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#ضمن <utility>
#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/localpointer.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/ures.h"
#ضمن "unicode/uscript.h"
#ضمن "charstr.h"
#ضمن "cstring.h"
#ضمن "loclikelysubtags.h"
#ضمن "lsr.h"
#ضمن "uassert.h"
#ضمن "ucln_cmn.h"
#ضمن "uhash.h"
#ضمن "uinvchar.h"
#ضمن "umutex.h"
#ضمن "uniquecharstr.h"
#ضمن "uresdata.h"
#ضمن "uresimp.h"
#ضمن "uvector.h"

U_NAMESPACE_BEGIN

نطاق {

constexpr حرف PSEUDO_ACCENTS_PREFIX = '\'';  // -XA, -PSACCENT
constexpr حرف PSEUDO_BIDI_PREFIX = '+';  // -XB, -PSBIDI
constexpr حرف PSEUDO_CRACKED_PREFIX = ',';  // -XC, -PSCRACK

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

بنية LikelySubtagsData {
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

    فراغ load(UErrorCode &errorCode) {
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
        StackUResourceBundle stackTempBundle;
        ResourceDataValue value;
        ures_getValueWithFallback(langInfoBundle, "likely", stackTempBundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        𐤀𐤌 (likelyTable.findValue("m49", value)) {
            m49Array = value.getArray(errorCode);
        } وإلا {
            errorCode = U_MISSING_RESOURCE_ERROR;
            أرجع;
        }
        𐤀𐤌 (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            أرجع;
        }
        𐤀𐤌 ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            أرجع;
        }
        𐤀𐤌 (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            أرجع;
        }

        𐤀𐤌 (!likelyTable.findValue("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            أرجع;
        }
        int32_t length;
        trieBytes = value.getBinary(length, errorCode);
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_getValueWithFallback(langInfoBundle, "match", stackTempBundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        𐤀𐤌 (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }

            𐤀𐤌 (matchTable.findValue("trie", value)) {
                distanceData.distanceTrieBytes = value.getBinary(length, errorCode);
                𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
            }

            𐤀𐤌 (matchTable.findValue("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.getBinary(length, errorCode);
                𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
                𐤀𐤌 (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    أرجع;
                }
            }

            𐤀𐤌 (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                أرجع;
            }
            𐤀𐤌 ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                أرجع;
            }

            𐤀𐤌 (matchTable.findValue("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
                𐤀𐤌 (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    أرجع;
                }
            }
        } وإلا 𐤀𐤌 (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } وإلا {  // error other than missing resource
            errorCode = matchErrorCode;
            أرجع;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languagesLength / 2, errorCode);
        لكل (int32_t i = 0; i < languagesLength; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regionsLength / 2, errorCode);
        لكل (int32_t i = 0; i < regionsLength; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = جديد LSR[lsrsLength];
        𐤀𐤌 (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            أرجع;
        }
        لكل (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        𐤀𐤌 (partitionsLength > 0) {
            distanceData.partitions = static_cast<ثابت حرف **>(
                uprv_malloc(partitionsLength * حجم(ثابت حرف *)));
            𐤀𐤌 (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                أرجع;
            }
            لكل (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        𐤀𐤌 (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = جديد LSR[distanceData.paradigmsLength];
            𐤀𐤌 (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                أرجع;
            }
            لكل (int32_t i = 0, j = 0; i < paradigmSubtagsLength; i += 3, ++j) {
                paradigms[j] = LSR(strings.get(paradigmSubtagIndexes[i]),
                                   strings.get(paradigmSubtagIndexes[i + 1]),
                                   strings.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            distanceData.paradigms = paradigms;
        }
    }

خاص:
    منطقي readStrings(ثابت ResourceTable &table, ثابت حرف *key, ResourceValue &value,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع خطأ; }
        𐤀𐤌 (table.findValue(key, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            𐤀𐤌 (U_FAILURE(errorCode)) { أرجع خطأ; }
            length = stringArray.getSize();
            𐤀𐤌 (length == 0) { أرجع true; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            𐤀𐤌 (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                أرجع خطأ;
            }
            لكل (صحيح i = 0; i < length; ++i) {
                𐤀𐤌 (stringArray.getValue(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع خطأ; }
                }
            }
        }
        أرجع true;
    }
    UnicodeString toLanguage(صحيح encoded) {
        𐤀𐤌 (encoded == 0) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        𐤀𐤌 (encoded == 1) {
            أرجع UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        حرف lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        𐤀𐤌 (encoded / (27 * 27) == 0) {
            أرجع UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        أرجع UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(صحيح encoded) {
        𐤀𐤌 (encoded == 0) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        𐤀𐤌 (encoded == 1) {
            أرجع UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        ثابت حرف* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        𐤀𐤌 (script == nullptr) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        أرجع UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(ثابت ResourceArray &m49Array, ResourceValue &value, صحيح index, UErrorCode &errorCode) {
        𐤀𐤌 (U_FAILURE(errorCode)) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        𐤀𐤌 (m49Array.getValue(index, value)) {
            أرجع value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        أرجع UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(ثابت ResourceArray& m49Array, ResourceValue &value, صحيح encoded, UErrorCode &errorCode) {
        𐤀𐤌 (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        𐤀𐤌 (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            أرجع  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        حرف region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        أرجع UnicodeString(region, 2, US_INV);
    }

    منطقي readLSREncodedStrings(ثابت ResourceTable &table, ثابت حرف* key, ResourceValue &value, ثابت ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع خطأ; }
        𐤀𐤌 (table.findValue(key, value)) {
            ثابت int32_t* vectors = value.getIntVector(length, errorCode);
            𐤀𐤌 (U_FAILURE(errorCode)) { أرجع خطأ; }
            𐤀𐤌 (length == 0) { أرجع true; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            𐤀𐤌 (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                أرجع خطأ;
            }
            لكل (صحيح i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.addByValue(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.addByValue(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.addByValue(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                𐤀𐤌 (U_FAILURE(errorCode)) { أرجع خطأ; }
            }
            length *= 3;
        }
        أرجع true;
    }
};

نطاق {

LikelySubtags *gLikelySubtags = nullptr;
UVector *gMacroregions = nullptr;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    delete gLikelySubtags;
    gLikelySubtags = nullptr;
    delete gMacroregions;
    gMacroregions = nullptr;
    gInitOnce.reset();
    أرجع true;
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
فراغ processMacroregionRange(ثابت UnicodeString& regionName, UVector* newMacroRegions, UErrorCode& status) {
    𐤀𐤌 (U_FAILURE(status)) { أرجع; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    𐤀𐤌 ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        طالما ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(جديد UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } وإلا {
        LocalPointer<UnicodeString> newRegion(جديد UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#𐤀𐤌 U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    𐤀𐤌 (U_FAILURE(status)) { أرجع nullptr; }
    LocalPointer<UVector> newMacroRegions(جديد UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_getByKey(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_getByKey(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_getByKey(regionList.getAlias(),"macroregion",nullptr,&status));

    𐤀𐤌 (U_FAILURE(status)) {
        أرجع nullptr;
    }

    طالما (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        𐤀𐤌 (U_FAILURE(status)) {
            أرجع nullptr;
        }
    }

    أرجع newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    𐤀𐤌 (U_FAILURE(status)) { أرجع nullptr; }
    LocalPointer<UVector> newMacroRegions(جديد UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    𐤀𐤌 (U_FAILURE(status)) {
        أرجع nullptr;
    }

    لكل (ثابت auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        𐤀𐤌 (U_FAILURE(status)) {
            أرجع nullptr;
        }
    }

    أرجع newMacroRegions.orphan();
}

}  // namespace

فراغ U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
    gLikelySubtags = جديد LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#𐤀𐤌 U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    𐤀𐤌 (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ثابت LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    أرجع gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#𐤀𐤌 U_DEBUG
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
    U_ASSERT(USTRINGTRIE_HAS_VALUE(result));
    defaultLsrIndex = trie.getValue();
    trie.reset();

    لكل (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        𐤀𐤌 (result == USTRINGTRIE_NO_VALUE) {
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
                                         منطقي returnInputIfUnmatch,
                                         UErrorCode &errorCode) ثابت {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع {}; }
    𐤀𐤌 (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع {};
    }
    ثابت حرف *name = locale.getName();
    𐤀𐤌 (!returnInputIfUnmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        أرجع LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), returnInputIfUnmatch, errorCode);

    𐤀𐤌 (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        أرجع LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    أرجع max;
}

نطاق {

ثابت حرف *getCanonical(ثابت CharStringMap &aliases, ثابت حرف *alias) {
    ثابت حرف *canonical = aliases.get(alias);
    أرجع canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                                     ثابت حرف *variant,
                                     منطقي returnInputIfUnmatch,
                                     UErrorCode &errorCode) ثابت {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    𐤀𐤌 (!returnInputIfUnmatch) {
        حرف c1;
        𐤀𐤌 (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            بدل (c1) {
            حالة 'A':
                أرجع LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            حالة 'B':
                أرجع LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            حالة 'C':
                أرجع LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            افتراضي:  // normal locale
                اكسر;
            }
        }

        𐤀𐤌 (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            𐤀𐤌 (uprv_strcmp(variant, "PSACCENT") == 0) {
                أرجع LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } وإلا 𐤀𐤌 (uprv_strcmp(variant, "PSBIDI") == 0) {
                أرجع LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } وإلا 𐤀𐤌 (uprv_strcmp(variant, "PSCRACK") == 0) {
                أرجع LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    أرجع maximize(language, script, region, returnInputIfUnmatch, errorCode);
}

LSR LikelySubtags::maximize(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                             منطقي returnInputIfUnmatch,
                             UErrorCode &errorCode) ثابت {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع {}; }
    أرجع maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    returnInputIfUnmatch,
                    errorCode);
}

منطقي LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) ثابت {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع خطأ; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع خطأ; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    أرجع gMacroregions->contains((فراغ *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             منطقي returnInputIfUnmatch,
                             UErrorCode &errorCode) ثابت {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع {}; }
    𐤀𐤌 (language.compare("und") == 0) {
        language = "";
    }
    𐤀𐤌 (script.compare("Zzzz") == 0) {
        script = "";
    }
    𐤀𐤌 (region.compare("ZZ") == 0) {
        region = "";
    }
    𐤀𐤌 (!script.empty() && !region.empty() && !language.empty()) {
        أرجع LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    منطقي retainLanguage = خطأ;
    منطقي retainScript = خطأ;
    منطقي retainRegion = خطأ;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    𐤀𐤌 (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } وإلا {
        value = trieNext(iter, language, 0);
    }
    منطقي matchLanguage = (value >= 0);
    منطقي matchScript = خطأ;
    𐤀𐤌 (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } وإلا {
        retainLanguage = true;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    𐤀𐤌 (value >= 0 && !script.empty()) {
        matchScript = true;
    }
    𐤀𐤌 (value > 0) {
        // Intermediate or final value from just language.
        𐤀𐤌 (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } وإلا {
        value = trieNext(iter, script, 0);
        𐤀𐤌 (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } وإلا {
            retainScript = true;
            𐤀𐤌 (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } وإلا {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    منطقي matchRegion = خطأ;
    𐤀𐤌 (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } وإلا {
        value = trieNext(iter, region, 0);
        𐤀𐤌 (value >= 0) {
            𐤀𐤌 (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = true;
                matchRegion = true;
            }
        } وإلا {
            retainRegion = true;
            𐤀𐤌 (state == 0) {
                value = defaultLsrIndex;
            } وإلا {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                𐤀𐤌 (value < 0) {
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
                    𐤀𐤌 (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    𐤀𐤌 (returnInputIfUnmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      أرجع LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    𐤀𐤌 (language.empty()) {
        language = StringPiece("und");
    }

    𐤀𐤌 (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        ثابت LSR &matched = lsrs[value];
        أرجع LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    𐤀𐤌 (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    𐤀𐤌 (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    𐤀𐤌 (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    أرجع LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(ثابت LSR &lsr, ثابت LSR &other, int32_t likelyInfo) ثابت {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    𐤀𐤌 (uprv_strcmp(lsr.language, other.language) != 0) {
        أرجع 0xfffffffc;  // negative, lsr not better than other
    }
    𐤀𐤌 (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        𐤀𐤌 (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } وإلا {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        ثابت LSR &likely = lsrs[index];
        𐤀𐤌 (uprv_strcmp(lsr.script, likely.script) == 0) {
            أرجع likelyInfo | 1;
        } وإلا {
            أرجع likelyInfo & ~1;
        }
    }
    𐤀𐤌 (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        𐤀𐤌 (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } وإلا {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        ثابت LSR &likely = lsrs[index];
        𐤀𐤌 (uprv_strcmp(lsr.region, likely.region) == 0) {
            أرجع likelyInfo | 1;
        } وإلا {
            أرجع likelyInfo & ~1;
        }
    }
    أرجع likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(ثابت حرف *language, ثابت حرف *script) ثابت {
    𐤀𐤌 (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    𐤀𐤌 (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    𐤀𐤌 (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } وإلا {
        value = trieNext(iter, language, 0);
    }
    𐤀𐤌 (value >= 0) {
        state = iter.getState64();
    } وإلا {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    𐤀𐤌 (value > 0) {
        // Intermediate or final value from just language.
        𐤀𐤌 (value == SKIP_SCRIPT) {
            value = 0;
        }
    } وإلا {
        value = trieNext(iter, script, 0);
        𐤀𐤌 (value >= 0) {
            state = iter.getState64();
        } وإلا {
            𐤀𐤌 (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } وإلا {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    𐤀𐤌 (value > 0) {
        // Final value from just language or language+script.
    } وإلا {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    أرجع value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, ثابت حرف *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    𐤀𐤌 ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } وإلا {
        لكل (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            𐤀𐤌 (next != 0) {
                𐤀𐤌 (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    أرجع -1;
                }
            } وإلا {
                // last character of this subtag
                result = iter.next(c | 0x80);
                اكسر;
            }
            c = next;
        }
    }
    بدل (result) {
    حالة USTRINGTRIE_NO_MATCH: أرجع -1;
    حالة USTRINGTRIE_NO_VALUE: أرجع 0;
    حالة USTRINGTRIE_INTERMEDIATE_VALUE:
        U_ASSERT(iter.getValue() == SKIP_SCRIPT);
        أرجع SKIP_SCRIPT;
    حالة USTRINGTRIE_FINAL_VALUE: أرجع iter.getValue();
    افتراضي: أرجع -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    𐤀𐤌 (s.length() == i) {
        result = iter.next(u'*');
    } وإلا {
        c = s.data()[i];
        لكل (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            𐤀𐤌 (i+1 != s.length()) {
                𐤀𐤌 (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    أرجع -1;
                }
                c = s.data()[++i];
            } وإلا {
                // last character of this subtag
                result = iter.next(c | 0x80);
                اكسر;
            }
        }
    }
    بدل (result) {
    حالة USTRINGTRIE_NO_MATCH: أرجع -1;
    حالة USTRINGTRIE_NO_VALUE: أرجع 0;
    حالة USTRINGTRIE_INTERMEDIATE_VALUE:
        U_ASSERT(iter.getValue() == SKIP_SCRIPT);
        أرجع SKIP_SCRIPT;
    حالة USTRINGTRIE_FINAL_VALUE: أرجع iter.getValue();
    افتراضي: أرجع -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    منطقي favorScript,
                                    UErrorCode &errorCode) ثابت {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع {}; }
    LSR max = maximize(language, script, region, true, errorCode);
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع {}; }
    // If no match, return it.
    𐤀𐤌 (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        أرجع LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", true, errorCode);
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع {}; }
    𐤀𐤌 (test.isEquivalentTo(max)) {
        أرجع LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    𐤀𐤌 (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, true, errorCode);
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع {}; }
        𐤀𐤌 (test.isEquivalentTo(max)) {
            أرجع LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", true, errorCode);
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع {}; }
    𐤀𐤌 (test.isEquivalentTo(max)) {
        أرجع LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    𐤀𐤌 (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, true, errorCode);
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع {}; }
        𐤀𐤌 (test.isEquivalentTo(max)) {
            أرجع LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    أرجع LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
