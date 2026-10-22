// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#અન્તર્ભાવ <utility>
#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/bytestrie.h"
#અન્તર્ભાવ "unicode/localpointer.h"
#અન્તર્ભાવ "unicode/locid.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "unicode/ures.h"
#અન્તર્ભાવ "unicode/uscript.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "loclikelysubtags.h"
#અન્તર્ભાવ "lsr.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "uhash.h"
#અન્તર્ભાવ "uinvchar.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "uniquecharstr.h"
#અન્તર્ભાવ "uresdata.h"
#અન્તર્ભાવ "uresimp.h"
#અન્તર્ભાવ "uvector.h"

U_NAMESPACE_BEGIN

નામાકાશ {

constexpr અક્ષર PSEUDO_ACCENTS_PREFIX = '\'';  // -XA, -PSACCENT
constexpr અક્ષર PSEUDO_BIDI_PREFIX = '+';  // -XB, -PSBIDI
constexpr અક્ષર PSEUDO_CRACKED_PREFIX = ',';  // -XC, -PSCRACK

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

સંરચના LikelySubtagsData {
    UResourceBundle *langInfoBundle = nullptr;
    UniqueCharStrings strings;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    અચલ uint8_t *trieBytes = nullptr;
    LSR *lsrs = nullptr;
    int32_t lsrsLength = 0;

    LocaleDistanceData distanceData;

    LikelySubtagsData(UErrorCode &errorCode) : strings(errorCode) {}

    ~LikelySubtagsData() {
        ures_close(langInfoBundle);
        delete[] lsrs;
    }

    શૂન્ય load(UErrorCode &errorCode) {
        જો (U_FAILURE(errorCode)) { પાછા; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        જો (U_FAILURE(errorCode)) { પાછા; }
        ઢગલો_UResource_Bundle ઢગલો_Temp_Bundle;
        Resource_Data_કિંમત value;
        ures_get_કિંમત_With_Fallback(langInfoBundle, "likely", ઢગલો_Temp_Bundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        જો (U_FAILURE(errorCode)) { પાછા; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        જો (likelyTable.find_કિંમત("m49", value)) {
            m49Array = value.getArray(errorCode);
        } નહીંતર {
            errorCode = U_MISSING_RESOURCE_ERROR;
            પાછા;
        }
        જો (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            પાછા;
        }
        જો ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            પાછા;
        }
        જો (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            પાછા;
        }

        જો (!likelyTable.find_કિંમત("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            પાછા;
        }
        int32_t length;
        trieBytes = value.get_દ્વિઅંકી(length, errorCode);
        જો (U_FAILURE(errorCode)) { પાછા; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_get_કિંમત_With_Fallback(langInfoBundle, "match", ઢગલો_Temp_Bundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        જો (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            જો (U_FAILURE(errorCode)) { પાછા; }

            જો (matchTable.find_કિંમત("trie", value)) {
                distanceData.distanceTrieBytes = value.get_દ્વિઅંકી(length, errorCode);
                જો (U_FAILURE(errorCode)) { પાછા; }
            }

            જો (matchTable.find_કિંમત("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.get_દ્વિઅંકી(length, errorCode);
                જો (U_FAILURE(errorCode)) { પાછા; }
                જો (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    પાછા;
                }
            }

            જો (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                પાછા;
            }
            જો ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                પાછા;
            }

            જો (matchTable.find_કિંમત("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                જો (U_FAILURE(errorCode)) { પાછા; }
                જો (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    પાછા;
                }
            }
        } નહીંતર જો (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } નહીંતર {  // error other than missing resource
            errorCode = matchErrorCode;
            પાછા;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languagesLength / 2, errorCode);
        માટે (int32_t i = 0; i < languagesLength; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regionsLength / 2, errorCode);
        માટે (int32_t i = 0; i < regionsLength; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        જો (U_FAILURE(errorCode)) { પાછા; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = નવું LSR[lsrsLength];
        જો (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            પાછા;
        }
        માટે (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        જો (partitionsLength > 0) {
            distanceData.partitions = static_cast<અચલ અક્ષર **>(
                uprv_malloc(partitionsLength * કદ(અચલ અક્ષર *)));
            જો (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                પાછા;
            }
            માટે (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        જો (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = નવું LSR[distanceData.paradigmsLength];
            જો (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                પાછા;
            }
            માટે (int32_t i = 0, j = 0; i < paradigmSubtagsLength; i += 3, ++j) {
                paradigms[j] = LSR(strings.get(paradigmSubtagIndexes[i]),
                                   strings.get(paradigmSubtagIndexes[i + 1]),
                                   strings.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            distanceData.paradigms = paradigms;
        }
    }

ખાનગી:
    તાર્કિક readStrings(અચલ ResourceTable &table, અચલ અક્ષર *ચાવી, Resource_કિંમત &value,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
        જો (table.find_કિંમત(ચાવી, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
            length = stringArray.getSize();
            જો (length == 0) { પાછા સાચું; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            જો (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                પાછા ખોટું;
            }
            માટે (પૂર્ણાંક i = 0; i < length; ++i) {
                જો (stringArray.get_કિંમત(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
                }
            }
        }
        પાછા સાચું;
    }
    UnicodeString toLanguage(પૂર્ણાંક encoded) {
        જો (encoded == 0) {
            પાછા UNICODE_STRING_SIMPLE("");
        }
        જો (encoded == 1) {
            પાછા UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        અક્ષર lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        જો (encoded / (27 * 27) == 0) {
            પાછા UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        પાછા UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(પૂર્ણાંક encoded) {
        જો (encoded == 0) {
            પાછા UNICODE_STRING_SIMPLE("");
        }
        જો (encoded == 1) {
            પાછા UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        અચલ અક્ષર* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        જો (script == nullptr) {
            પાછા UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        પાછા UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(અચલ ResourceArray &m49Array, Resource_કિંમત &value, પૂર્ણાંક index, UErrorCode &errorCode) {
        જો (U_FAILURE(errorCode)) {
            પાછા UNICODE_STRING_SIMPLE("");
        }
        જો (m49Array.get_કિંમત(index, value)) {
            પાછા value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        પાછા UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(અચલ ResourceArray& m49Array, Resource_કિંમત &value, પૂર્ણાંક encoded, UErrorCode &errorCode) {
        જો (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            પાછા UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        જો (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            પાછા  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        અક્ષર region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        પાછા UnicodeString(region, 2, US_INV);
    }

    તાર્કિક readLSREncodedStrings(અચલ ResourceTable &table, અચલ અક્ષર* ચાવી, Resource_કિંમત &value, અચલ ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
        જો (table.find_કિંમત(ચાવી, value)) {
            અચલ int32_t* vectors = value.getIntVector(length, errorCode);
            જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
            જો (length == 0) { પાછા સાચું; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            જો (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                પાછા ખોટું;
            }
            માટે (પૂર્ણાંક i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.add_By_કિંમત(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.add_By_કિંમત(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.add_By_કિંમત(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
            }
            length *= 3;
        }
        પાછા સાચું;
    }
};

નામાકાશ {

LikelySubtags *gLikelySubtags = nullptr;
UVector *gMacroregions = nullptr;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    delete gLikelySubtags;
    gLikelySubtags = nullptr;
    delete gMacroregions;
    gMacroregions = nullptr;
    gInitOnce.reset();
    પાછા સાચું;
}

constexpr અચલ char16_t* MACROREGION_HARDCODE[] = {
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
શૂન્ય processMacroregionRange(અચલ UnicodeString& regionName, UVector* newMacroRegions, UErrorCode& status) {
    જો (U_FAILURE(status)) { પાછા; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    જો ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        જ્યારે ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(નવું UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } નહીંતર {
        LocalPointer<UnicodeString> newRegion(નવું UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#જો U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    જો (U_FAILURE(status)) { પાછા nullptr; }
    LocalPointer<UVector> newMacroRegions(નવું UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_get_By_ચાવી(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_get_By_ચાવી(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_get_By_ચાવી(regionList.getAlias(),"macroregion",nullptr,&status));

    જો (U_FAILURE(status)) {
        પાછા nullptr;
    }

    જ્યારે (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        જો (U_FAILURE(status)) {
            પાછા nullptr;
        }
    }

    પાછા newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    જો (U_FAILURE(status)) { પાછા nullptr; }
    LocalPointer<UVector> newMacroRegions(નવું UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    જો (U_FAILURE(status)) {
        પાછા nullptr;
    }

    માટે (અચલ auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        જો (U_FAILURE(status)) {
            પાછા nullptr;
        }
    }

    પાછા newMacroRegions.orphan();
}

}  // namespace

શૂન્ય U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    જો (U_FAILURE(errorCode)) { પાછા; }
    gLikelySubtags = નવું LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#જો U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    જો (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

અચલ LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    પાછા gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#જો U_DEBUG
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
    U_ASSERT(USTRINGTRIE_HAS_કિંમત(result));
    defaultLsrIndex = trie.get_કિંમત();
    trie.reset();

    માટે (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        જો (result == USTRINGTRIE_NO_કિંમત) {
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

LSR LikelySubtags::makeMaximizedLsrFrom(અચલ Locale &locale,
                                         તાર્કિક return_આગત_If_Unmatch,
                                         UErrorCode &errorCode) અચલ {
    જો (U_FAILURE(errorCode)) { પાછા {}; }
    જો (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા {};
    }
    અચલ અક્ષર *name = locale.getName();
    જો (!return_આગત_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        પાછા LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_આગત_If_Unmatch, errorCode);

    જો (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        પાછા LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    પાછા max;
}

નામાકાશ {

અચલ અક્ષર *getCanonical(અચલ CharStringMap &aliases, અચલ અક્ષર *alias) {
    અચલ અક્ષર *canonical = aliases.get(alias);
    પાછા canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(અચલ અક્ષર *language, અચલ અક્ષર *script, અચલ અક્ષર *region,
                                     અચલ અક્ષર *variant,
                                     તાર્કિક return_આગત_If_Unmatch,
                                     UErrorCode &errorCode) અચલ {
    જો (U_FAILURE(errorCode)) { પાછા {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    જો (!return_આગત_If_Unmatch) {
        અક્ષર c1;
        જો (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            પસંદગી (c1) {
            સ્થિતિ 'A':
                પાછા LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            સ્થિતિ 'B':
                પાછા LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            સ્થિતિ 'C':
                પાછા LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            મૂળભૂત:  // normal locale
                તોડો;
            }
        }

        જો (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            જો (uprv_strcmp(variant, "PSACCENT") == 0) {
                પાછા LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } નહીંતર જો (uprv_strcmp(variant, "PSBIDI") == 0) {
                પાછા LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } નહીંતર જો (uprv_strcmp(variant, "PSCRACK") == 0) {
                પાછા LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    પાછા maximize(language, script, region, return_આગત_If_Unmatch, errorCode);
}

LSR LikelySubtags::maximize(અચલ અક્ષર *language, અચલ અક્ષર *script, અચલ અક્ષર *region,
                             તાર્કિક return_આગત_If_Unmatch,
                             UErrorCode &errorCode) અચલ {
    જો (U_FAILURE(errorCode)) { પાછા {}; }
    પાછા maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_આગત_If_Unmatch,
                    errorCode);
}

તાર્કિક LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) અચલ {
    જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    પાછા gMacroregions->contains((શૂન્ય *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             તાર્કિક return_આગત_If_Unmatch,
                             UErrorCode &errorCode) અચલ {
    જો (U_FAILURE(errorCode)) { પાછા {}; }
    જો (language.compare("und") == 0) {
        language = "";
    }
    જો (script.compare("Zzzz") == 0) {
        script = "";
    }
    જો (region.compare("ZZ") == 0) {
        region = "";
    }
    જો (!script.empty() && !region.empty() && !language.empty()) {
        પાછા LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    તાર્કિક retainLanguage = ખોટું;
    તાર્કિક retainScript = ખોટું;
    તાર્કિક retainRegion = ખોટું;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    જો (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } નહીંતર {
        value = trieNext(iter, language, 0);
    }
    તાર્કિક matchLanguage = (value >= 0);
    તાર્કિક matchScript = ખોટું;
    જો (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } નહીંતર {
        retainLanguage = સાચું;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    જો (value >= 0 && !script.empty()) {
        matchScript = સાચું;
    }
    જો (value > 0) {
        // Intermediate or final value from just language.
        જો (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } નહીંતર {
        value = trieNext(iter, script, 0);
        જો (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } નહીંતર {
            retainScript = સાચું;
            જો (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } નહીંતર {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    તાર્કિક matchRegion = ખોટું;
    જો (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } નહીંતર {
        value = trieNext(iter, region, 0);
        જો (value >= 0) {
            જો (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = સાચું;
                matchRegion = સાચું;
            }
        } નહીંતર {
            retainRegion = સાચું;
            જો (state == 0) {
                value = defaultLsrIndex;
            } નહીંતર {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                જો (value < 0) {
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
                    જો (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    જો (return_આગત_If_Unmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      પાછા LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    જો (language.empty()) {
        language = StringPiece("und");
    }

    જો (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        અચલ LSR &matched = lsrs[value];
        પાછા LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    જો (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    જો (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    જો (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    પાછા LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(અચલ LSR &lsr, અચલ LSR &other, int32_t likelyInfo) અચલ {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    જો (uprv_strcmp(lsr.language, other.language) != 0) {
        પાછા 0xfffffffc;  // negative, lsr not better than other
    }
    જો (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        જો (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } નહીંતર {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        અચલ LSR &likely = lsrs[index];
        જો (uprv_strcmp(lsr.script, likely.script) == 0) {
            પાછા likelyInfo | 1;
        } નહીંતર {
            પાછા likelyInfo & ~1;
        }
    }
    જો (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        જો (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } નહીંતર {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        અચલ LSR &likely = lsrs[index];
        જો (uprv_strcmp(lsr.region, likely.region) == 0) {
            પાછા likelyInfo | 1;
        } નહીંતર {
            પાછા likelyInfo & ~1;
        }
    }
    પાછા likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(અચલ અક્ષર *language, અચલ અક્ષર *script) અચલ {
    જો (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    જો (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    જો (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } નહીંતર {
        value = trieNext(iter, language, 0);
    }
    જો (value >= 0) {
        state = iter.getState64();
    } નહીંતર {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    જો (value > 0) {
        // Intermediate or final value from just language.
        જો (value == SKIP_SCRIPT) {
            value = 0;
        }
    } નહીંતર {
        value = trieNext(iter, script, 0);
        જો (value >= 0) {
            state = iter.getState64();
        } નહીંતર {
            જો (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } નહીંતર {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    જો (value > 0) {
        // Final value from just language or language+script.
    } નહીંતર {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    પાછા value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, અચલ અક્ષર *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    જો ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } નહીંતર {
        માટે (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            જો (next != 0) {
                જો (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    પાછા -1;
                }
            } નહીંતર {
                // last character of this subtag
                result = iter.next(c | 0x80);
                તોડો;
            }
            c = next;
        }
    }
    પસંદગી (result) {
    સ્થિતિ USTRINGTRIE_NO_MATCH: પાછા -1;
    સ્થિતિ USTRINGTRIE_NO_કિંમત: પાછા 0;
    સ્થિતિ USTRINGTRIE_INTERMEDIATE_કિંમત:
        U_ASSERT(iter.get_કિંમત() == SKIP_SCRIPT);
        પાછા SKIP_SCRIPT;
    સ્થિતિ USTRINGTRIE_FINAL_કિંમત: પાછા iter.get_કિંમત();
    મૂળભૂત: પાછા -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    જો (s.length() == i) {
        result = iter.next(u'*');
    } નહીંતર {
        c = s.data()[i];
        માટે (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            જો (i+1 != s.length()) {
                જો (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    પાછા -1;
                }
                c = s.data()[++i];
            } નહીંતર {
                // last character of this subtag
                result = iter.next(c | 0x80);
                તોડો;
            }
        }
    }
    પસંદગી (result) {
    સ્થિતિ USTRINGTRIE_NO_MATCH: પાછા -1;
    સ્થિતિ USTRINGTRIE_NO_કિંમત: પાછા 0;
    સ્થિતિ USTRINGTRIE_INTERMEDIATE_કિંમત:
        U_ASSERT(iter.get_કિંમત() == SKIP_SCRIPT);
        પાછા SKIP_SCRIPT;
    સ્થિતિ USTRINGTRIE_FINAL_કિંમત: પાછા iter.get_કિંમત();
    મૂળભૂત: પાછા -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    તાર્કિક favorScript,
                                    UErrorCode &errorCode) અચલ {
    જો (U_FAILURE(errorCode)) { પાછા {}; }
    LSR max = maximize(language, script, region, સાચું, errorCode);
    જો (U_FAILURE(errorCode)) { પાછા {}; }
    // If no match, return it.
    જો (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        પાછા LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", સાચું, errorCode);
    જો (U_FAILURE(errorCode)) { પાછા {}; }
    જો (test.isEquivalentTo(max)) {
        પાછા LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    જો (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, સાચું, errorCode);
        જો (U_FAILURE(errorCode)) { પાછા {}; }
        જો (test.isEquivalentTo(max)) {
            પાછા LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", સાચું, errorCode);
    જો (U_FAILURE(errorCode)) { પાછા {}; }
    જો (test.isEquivalentTo(max)) {
        પાછા LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    જો (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, સાચું, errorCode);
        જો (U_FAILURE(errorCode)) { પાછા {}; }
        જો (test.isEquivalentTo(max)) {
            પાછા LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    પાછા LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
