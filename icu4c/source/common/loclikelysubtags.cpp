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
        यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
        ढेर_UResource_Bundle ढेर_Temp_Bundle;
        Resource_Data_मूल्य value;
        ures_get_मूल्य_With_Fallback(langInfoBundle, "likely", ढेर_Temp_Bundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        यदि (U_FAILURE(errorCode)) { प्रत्यागम; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        यदि (likelyTable.find_मूल्य("m49", value)) {
            m49Array = value.getArray(errorCode);
        } अन्यथा {
            errorCode = U_MISSING_RESOURCE_ERROR;
            प्रत्यागम;
        }
        यदि (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            प्रत्यागम;
        }
        यदि ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            प्रत्यागम;
        }
        यदि (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            प्रत्यागम;
        }

        यदि (!likelyTable.find_मूल्य("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            प्रत्यागम;
        }
        int32_t length;
        trieBytes = value.get_द्विआधारी(length, errorCode);
        यदि (U_FAILURE(errorCode)) { प्रत्यागम; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_get_मूल्य_With_Fallback(langInfoBundle, "match", ढेर_Temp_Bundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        यदि (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            यदि (U_FAILURE(errorCode)) { प्रत्यागम; }

            यदि (matchTable.find_मूल्य("trie", value)) {
                distanceData.distanceTrieBytes = value.get_द्विआधारी(length, errorCode);
                यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
            }

            यदि (matchTable.find_मूल्य("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.get_द्विआधारी(length, errorCode);
                यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
                यदि (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    प्रत्यागम;
                }
            }

            यदि (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                प्रत्यागम;
            }
            यदि ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                प्रत्यागम;
            }

            यदि (matchTable.find_मूल्य("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
                यदि (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    प्रत्यागम;
                }
            }
        } अन्यथा यदि (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } अन्यथा {  // error other than missing resource
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
        यदि (U_FAILURE(errorCode)) { प्रत्यागम; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = नूतन LSR[lsrsLength];
        यदि (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम;
        }
        प्रति (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        यदि (partitionsLength > 0) {
            distanceData.partitions = static_cast<ध्रुव वर्ण **>(
                uprv_malloc(partitionsLength * परिमाण(ध्रुव वर्ण *)));
            यदि (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम;
            }
            प्रति (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        यदि (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = नूतन LSR[distanceData.paradigmsLength];
            यदि (paradigms == nullptr) {
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
    तार्किक readStrings(ध्रुव ResourceTable &table, ध्रुव वर्ण *कुंजी, Resource_मूल्य &value,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        यदि (U_FAILURE(errorCode)) { प्रत्यागम असत्य; }
        यदि (table.find_मूल्य(कुंजी, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            यदि (U_FAILURE(errorCode)) { प्रत्यागम असत्य; }
            length = stringArray.getSize();
            यदि (length == 0) { प्रत्यागम सत्य; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            यदि (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम असत्य;
            }
            प्रति (पूर्णाङ्क i = 0; i < length; ++i) {
                यदि (stringArray.get_मूल्य(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    यदि (U_FAILURE(errorCode)) { प्रत्यागम असत्य; }
                }
            }
        }
        प्रत्यागम सत्य;
    }
    UnicodeString toLanguage(पूर्णाङ्क encoded) {
        यदि (encoded == 0) {
            प्रत्यागम UNICODE_STRING_SIMPLE("");
        }
        यदि (encoded == 1) {
            प्रत्यागम UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        वर्ण lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        यदि (encoded / (27 * 27) == 0) {
            प्रत्यागम UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        प्रत्यागम UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(पूर्णाङ्क encoded) {
        यदि (encoded == 0) {
            प्रत्यागम UNICODE_STRING_SIMPLE("");
        }
        यदि (encoded == 1) {
            प्रत्यागम UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        ध्रुव वर्ण* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        यदि (script == nullptr) {
            प्रत्यागम UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        प्रत्यागम UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(ध्रुव ResourceArray &m49Array, Resource_मूल्य &value, पूर्णाङ्क index, UErrorCode &errorCode) {
        यदि (U_FAILURE(errorCode)) {
            प्रत्यागम UNICODE_STRING_SIMPLE("");
        }
        यदि (m49Array.get_मूल्य(index, value)) {
            प्रत्यागम value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        प्रत्यागम UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(ध्रुव ResourceArray& m49Array, Resource_मूल्य &value, पूर्णाङ्क encoded, UErrorCode &errorCode) {
        यदि (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            प्रत्यागम UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        यदि (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            प्रत्यागम  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        वर्ण region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        प्रत्यागम UnicodeString(region, 2, US_INV);
    }

    तार्किक readLSREncodedStrings(ध्रुव ResourceTable &table, ध्रुव वर्ण* कुंजी, Resource_मूल्य &value, ध्रुव ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        यदि (U_FAILURE(errorCode)) { प्रत्यागम असत्य; }
        यदि (table.find_मूल्य(कुंजी, value)) {
            ध्रुव int32_t* vectors = value.getIntVector(length, errorCode);
            यदि (U_FAILURE(errorCode)) { प्रत्यागम असत्य; }
            यदि (length == 0) { प्रत्यागम सत्य; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            यदि (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम असत्य;
            }
            प्रति (पूर्णाङ्क i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.add_By_मूल्य(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.add_By_मूल्य(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.add_By_मूल्य(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                यदि (U_FAILURE(errorCode)) { प्रत्यागम असत्य; }
            }
            length *= 3;
        }
        प्रत्यागम सत्य;
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
    प्रत्यागम सत्य;
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
    यदि (U_FAILURE(status)) { प्रत्यागम; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    यदि ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        यावत् ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(नूतन UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } अन्यथा {
        LocalPointer<UnicodeString> newRegion(नूतन UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#यदि U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    यदि (U_FAILURE(status)) { प्रत्यागम nullptr; }
    LocalPointer<UVector> newMacroRegions(नूतन UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_get_By_कुंजी(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_get_By_कुंजी(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_get_By_कुंजी(regionList.getAlias(),"macroregion",nullptr,&status));

    यदि (U_FAILURE(status)) {
        प्रत्यागम nullptr;
    }

    यावत् (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        यदि (U_FAILURE(status)) {
            प्रत्यागम nullptr;
        }
    }

    प्रत्यागम newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    यदि (U_FAILURE(status)) { प्रत्यागम nullptr; }
    LocalPointer<UVector> newMacroRegions(नूतन UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    यदि (U_FAILURE(status)) {
        प्रत्यागम nullptr;
    }

    प्रति (ध्रुव auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        यदि (U_FAILURE(status)) {
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
    यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
    gLikelySubtags = नूतन LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#यदि U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    यदि (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ध्रुव LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम nullptr; }
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
#यदि U_DEBUG
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

    प्रति (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        यदि (result == USTRINGTRIE_NO_मूल्य) {
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
                                         तार्किक return_निवेश_If_Unmatch,
                                         UErrorCode &errorCode) ध्रुव {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम {}; }
    यदि (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम {};
    }
    ध्रुव वर्ण *name = locale.getName();
    यदि (!return_निवेश_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        प्रत्यागम LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_निवेश_If_Unmatch, errorCode);

    यदि (uprv_strlen(max.language) == 0 &&
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
                                     तार्किक return_निवेश_If_Unmatch,
                                     UErrorCode &errorCode) ध्रुव {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    यदि (!return_निवेश_If_Unmatch) {
        वर्ण c1;
        यदि (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
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

        यदि (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            यदि (uprv_strcmp(variant, "PSACCENT") == 0) {
                प्रत्यागम LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } अन्यथा यदि (uprv_strcmp(variant, "PSBIDI") == 0) {
                प्रत्यागम LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } अन्यथा यदि (uprv_strcmp(variant, "PSCRACK") == 0) {
                प्रत्यागम LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    प्रत्यागम maximize(language, script, region, return_निवेश_If_Unmatch, errorCode);
}

LSR LikelySubtags::maximize(ध्रुव वर्ण *language, ध्रुव वर्ण *script, ध्रुव वर्ण *region,
                             तार्किक return_निवेश_If_Unmatch,
                             UErrorCode &errorCode) ध्रुव {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम {}; }
    प्रत्यागम maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_निवेश_If_Unmatch,
                    errorCode);
}

तार्किक LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) ध्रुव {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम असत्य; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    यदि (U_FAILURE(errorCode)) { प्रत्यागम असत्य; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    प्रत्यागम gMacroregions->contains((शून्य *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             तार्किक return_निवेश_If_Unmatch,
                             UErrorCode &errorCode) ध्रुव {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम {}; }
    यदि (language.compare("und") == 0) {
        language = "";
    }
    यदि (script.compare("Zzzz") == 0) {
        script = "";
    }
    यदि (region.compare("ZZ") == 0) {
        region = "";
    }
    यदि (!script.empty() && !region.empty() && !language.empty()) {
        प्रत्यागम LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    तार्किक retainLanguage = असत्य;
    तार्किक retainScript = असत्य;
    तार्किक retainRegion = असत्य;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    यदि (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } अन्यथा {
        value = trieNext(iter, language, 0);
    }
    तार्किक matchLanguage = (value >= 0);
    तार्किक matchScript = असत्य;
    यदि (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } अन्यथा {
        retainLanguage = सत्य;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    यदि (value >= 0 && !script.empty()) {
        matchScript = सत्य;
    }
    यदि (value > 0) {
        // Intermediate or final value from just language.
        यदि (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } अन्यथा {
        value = trieNext(iter, script, 0);
        यदि (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } अन्यथा {
            retainScript = सत्य;
            यदि (state == 0) {
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
    यदि (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } अन्यथा {
        value = trieNext(iter, region, 0);
        यदि (value >= 0) {
            यदि (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = सत्य;
                matchRegion = सत्य;
            }
        } अन्यथा {
            retainRegion = सत्य;
            यदि (state == 0) {
                value = defaultLsrIndex;
            } अन्यथा {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                यदि (value < 0) {
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
                    यदि (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    यदि (return_निवेश_If_Unmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      प्रत्यागम LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    यदि (language.empty()) {
        language = StringPiece("und");
    }

    यदि (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        ध्रुव LSR &matched = lsrs[value];
        प्रत्यागम LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    यदि (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    यदि (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    यदि (!retainRegion) {
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
    यदि (uprv_strcmp(lsr.language, other.language) != 0) {
        प्रत्यागम 0xfffffffc;  // negative, lsr not better than other
    }
    यदि (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        यदि (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } अन्यथा {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        ध्रुव LSR &likely = lsrs[index];
        यदि (uprv_strcmp(lsr.script, likely.script) == 0) {
            प्रत्यागम likelyInfo | 1;
        } अन्यथा {
            प्रत्यागम likelyInfo & ~1;
        }
    }
    यदि (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        यदि (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } अन्यथा {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        ध्रुव LSR &likely = lsrs[index];
        यदि (uprv_strcmp(lsr.region, likely.region) == 0) {
            प्रत्यागम likelyInfo | 1;
        } अन्यथा {
            प्रत्यागम likelyInfo & ~1;
        }
    }
    प्रत्यागम likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(ध्रुव वर्ण *language, ध्रुव वर्ण *script) ध्रुव {
    यदि (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    यदि (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    यदि (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } अन्यथा {
        value = trieNext(iter, language, 0);
    }
    यदि (value >= 0) {
        state = iter.getState64();
    } अन्यथा {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    यदि (value > 0) {
        // Intermediate or final value from just language.
        यदि (value == SKIP_SCRIPT) {
            value = 0;
        }
    } अन्यथा {
        value = trieNext(iter, script, 0);
        यदि (value >= 0) {
            state = iter.getState64();
        } अन्यथा {
            यदि (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } अन्यथा {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    यदि (value > 0) {
        // Final value from just language or language+script.
    } अन्यथा {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    प्रत्यागम value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, ध्रुव वर्ण *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    यदि ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } अन्यथा {
        प्रति (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            यदि (next != 0) {
                यदि (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    प्रत्यागम -1;
                }
            } अन्यथा {
                // last character of this subtag
                result = iter.next(c | 0x80);
                भङ्ग;
            }
            c = next;
        }
    }
    विकल्प (result) {
    पक्ष USTRINGTRIE_NO_MATCH: प्रत्यागम -1;
    पक्ष USTRINGTRIE_NO_मूल्य: प्रत्यागम 0;
    पक्ष USTRINGTRIE_INTERMEDIATE_मूल्य:
        U_ASSERT(iter.get_मूल्य() == SKIP_SCRIPT);
        प्रत्यागम SKIP_SCRIPT;
    पक्ष USTRINGTRIE_FINAL_मूल्य: प्रत्यागम iter.get_मूल्य();
    स्वभाव: प्रत्यागम -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    यदि (s.length() == i) {
        result = iter.next(u'*');
    } अन्यथा {
        c = s.data()[i];
        प्रति (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            यदि (i+1 != s.length()) {
                यदि (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    प्रत्यागम -1;
                }
                c = s.data()[++i];
            } अन्यथा {
                // last character of this subtag
                result = iter.next(c | 0x80);
                भङ्ग;
            }
        }
    }
    विकल्प (result) {
    पक्ष USTRINGTRIE_NO_MATCH: प्रत्यागम -1;
    पक्ष USTRINGTRIE_NO_मूल्य: प्रत्यागम 0;
    पक्ष USTRINGTRIE_INTERMEDIATE_मूल्य:
        U_ASSERT(iter.get_मूल्य() == SKIP_SCRIPT);
        प्रत्यागम SKIP_SCRIPT;
    पक्ष USTRINGTRIE_FINAL_मूल्य: प्रत्यागम iter.get_मूल्य();
    स्वभाव: प्रत्यागम -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    तार्किक favorScript,
                                    UErrorCode &errorCode) ध्रुव {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम {}; }
    LSR max = maximize(language, script, region, सत्य, errorCode);
    यदि (U_FAILURE(errorCode)) { प्रत्यागम {}; }
    // If no match, return it.
    यदि (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        प्रत्यागम LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", सत्य, errorCode);
    यदि (U_FAILURE(errorCode)) { प्रत्यागम {}; }
    यदि (test.isEquivalentTo(max)) {
        प्रत्यागम LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    यदि (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, सत्य, errorCode);
        यदि (U_FAILURE(errorCode)) { प्रत्यागम {}; }
        यदि (test.isEquivalentTo(max)) {
            प्रत्यागम LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", सत्य, errorCode);
    यदि (U_FAILURE(errorCode)) { प्रत्यागम {}; }
    यदि (test.isEquivalentTo(max)) {
        प्रत्यागम LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    यदि (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, सत्य, errorCode);
        यदि (U_FAILURE(errorCode)) { प्रत्यागम {}; }
        यदि (test.isEquivalentTo(max)) {
            प्रत्यागम LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    प्रत्यागम LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
