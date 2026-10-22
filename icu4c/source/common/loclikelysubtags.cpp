// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#समावेश <utility>
#समावेश "unicode/utypes.h"
#समावेश "unicode/bytestrie.h"
#समावेश "unicode/localpointer.h"
#समावेश "unicode/locid.h"
#समावेश "unicode/uobject.h"
#समावेश "unicode/ures.h"
#समावेश "unicode/uscript.h"
#समावेश "charstr.h"
#समावेश "cstring.h"
#समावेश "loclikelysubtags.h"
#समावेश "lsr.h"
#समावेश "uassert.h"
#समावेश "ucln_cmn.h"
#समावेश "uhash.h"
#समावेश "uinvchar.h"
#समावेश "umutex.h"
#समावेश "uniquecharstr.h"
#समावेश "uresdata.h"
#समावेश "uresimp.h"
#समावेश "uvector.h"

U_NAMESPACE_BEGIN

namespace {

constexpr अक्षर PSEUDO_ACCENTS_PREFIX = '\'';  // -XA, -PSACCENT
constexpr अक्षर PSEUDO_BIDI_PREFIX = '+';  // -XB, -PSBIDI
constexpr अक्षर PSEUDO_CRACKED_PREFIX = ',';  // -XC, -PSCRACK

}  // namespace

Locale_Distance_तथ्य::Locale_Distance_तथ्य(Locale_Distance_तथ्य &&तथ्य) :
        distanceTrieBytes(तथ्य.distanceTrieBytes),
        regionToPartitions(तथ्य.regionToPartitions),
        partitions(तथ्य.partitions),
        paradigms(तथ्य.paradigms), paradigms_लंबाई(तथ्य.paradigms_लंबाई),
        distances(तथ्य.distances) {
    तथ्य.partitions = nullptr;
    तथ्य.paradigms = nullptr;
}

Locale_Distance_तथ्य::~Locale_Distance_तथ्य() {
    uprv_free(partitions);
    मिटाओ[] paradigms;
}

काष्ठा Likely_Subtags_तथ्य {
    UResourceBundle *langInfoBundle = nullptr;
    UniqueCharStrings strings;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    स्थिर uint8_t *trieBytes = nullptr;
    LSR *lsrs = nullptr;
    int32_t lsrs_लंबाई = 0;

    Locale_Distance_तथ्य distance_तथ्य;

    Likely_Subtags_तथ्य(UErrorCode &errorCode) : strings(errorCode) {}

    ~Likely_Subtags_तथ्य() {
        ures_close(langInfoBundle);
        मिटाओ[] lsrs;
    }

    व्योम load(UErrorCode &errorCode) {
        अगर (U_FAILURE(errorCode)) { वापस; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        अगर (U_FAILURE(errorCode)) { वापस; }
        ढेर_UResource_Bundle ढेर_अस्थायी_Bundle;
        Resource_तथ्य_मूल्य value;
        ures_get_मूल्य_With_Fallback(langInfoBundle, "likely", ढेर_अस्थायी_Bundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        अगर (U_FAILURE(errorCode)) { वापस; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languages_लंबाई = 0, regions_लंबाई = 0, lsr_Subtags_लंबाई = 0;
        ResourceArray m49Array;
        अगर (likelyTable.find_मूल्य("m49", value)) {
            m49Array = value.getArray(errorCode);
        } अन्यथा {
            errorCode = U_MISSING_RESOURCE_ERROR;
            वापस;
        }
        अगर (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languages_लंबाई, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regions_लंबाई, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsr_Subtags_लंबाई, errorCode)) {
            वापस;
        }
        अगर ((languages_लंबाई & 1) != 0 ||
                (regions_लंबाई & 1) != 0 ||
                (lsr_Subtags_लंबाई % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            वापस;
        }
        अगर (lsr_Subtags_लंबाई == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            वापस;
        }

        अगर (!likelyTable.find_मूल्य("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            वापस;
        }
        int32_t length;
        trieBytes = value.get_द्विआधारी(length, errorCode);
        अगर (U_FAILURE(errorCode)) { वापस; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_get_मूल्य_With_Fallback(langInfoBundle, "match", ढेर_अस्थायी_Bundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitions_लंबाई = 0, paradigm_Subtags_लंबाई = 0;
        अगर (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            अगर (U_FAILURE(errorCode)) { वापस; }

            अगर (matchTable.find_मूल्य("trie", value)) {
                distance_तथ्य.distanceTrieBytes = value.get_द्विआधारी(length, errorCode);
                अगर (U_FAILURE(errorCode)) { वापस; }
            }

            अगर (matchTable.find_मूल्य("regionToPartitions", value)) {
                distance_तथ्य.regionToPartitions = value.get_द्विआधारी(length, errorCode);
                अगर (U_FAILURE(errorCode)) { वापस; }
                अगर (length < LSR::REGION_सूचकांक_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    वापस;
                }
            }

            अगर (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitions_लंबाई, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigm_Subtags_लंबाई, errorCode)) {
                वापस;
            }
            अगर ((paradigm_Subtags_लंबाई % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                वापस;
            }

            अगर (matchTable.find_मूल्य("distances", value)) {
                distance_तथ्य.distances = value.getIntVector(length, errorCode);
                अगर (U_FAILURE(errorCode)) { वापस; }
                अगर (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    वापस;
                }
            }
        } अन्यथा अगर (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } अन्यथा {  // error other than missing resource
            errorCode = matchErrorCode;
            वापस;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languages_लंबाई / 2, errorCode);
        क्रम (int32_t i = 0; i < languages_लंबाई; i += 2) {
            languageAliases.दो(strings.लो(languageIndexes[i]),
                                strings.लो(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regions_लंबाई / 2, errorCode);
        क्रम (int32_t i = 0; i < regions_लंबाई; i += 2) {
            regionAliases.दो(strings.लो(regionIndexes[i]),
                              strings.लो(regionIndexes[i + 1]), errorCode);
        }
        अगर (U_FAILURE(errorCode)) { वापस; }

        lsrs_लंबाई = lsr_Subtags_लंबाई / 3;
        lsrs = नया LSR[lsrs_लंबाई];
        अगर (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }
        क्रम (int32_t i = 0, j = 0; i < lsr_Subtags_लंबाई; i += 3, ++j) {
            lsrs[j] = LSR(strings.लो(lsrSubtagIndexes[i]),
                          strings.लो(lsrSubtagIndexes[i + 1]),
                          strings.लो(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        अगर (partitions_लंबाई > 0) {
            distance_तथ्य.partitions = static_cast<स्थिर अक्षर **>(
                uprv_malloc(partitions_लंबाई * माप(स्थिर अक्षर *)));
            अगर (distance_तथ्य.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                वापस;
            }
            क्रम (int32_t i = 0; i < partitions_लंबाई; ++i) {
                distance_तथ्य.partitions[i] = strings.लो(partitionIndexes[i]);
            }
        }

        अगर (paradigm_Subtags_लंबाई > 0) {
            distance_तथ्य.paradigms_लंबाई = paradigm_Subtags_लंबाई / 3;
            LSR *paradigms = नया LSR[distance_तथ्य.paradigms_लंबाई];
            अगर (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                वापस;
            }
            क्रम (int32_t i = 0, j = 0; i < paradigm_Subtags_लंबाई; i += 3, ++j) {
                paradigms[j] = LSR(strings.लो(paradigmSubtagIndexes[i]),
                                   strings.लो(paradigmSubtagIndexes[i + 1]),
                                   strings.लो(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            distance_तथ्य.paradigms = paradigms;
        }
    }

निजी:
    bool readStrings(स्थिर ResourceTable &table, स्थिर अक्षर *कुंजी, Resource_मूल्य &value,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        अगर (U_FAILURE(errorCode)) { वापस false; }
        अगर (table.find_मूल्य(कुंजी, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            अगर (U_FAILURE(errorCode)) { वापस false; }
            length = stringArray.get_आकार();
            अगर (length == 0) { वापस true; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            अगर (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                वापस false;
            }
            क्रम (पूर्णांक i = 0; i < length; ++i) {
                अगर (stringArray.get_मूल्य(i, value)) {  // returns true because i < length
                    int32_t str_लंबाई = 0;
                    rawIndexes[i] = strings.add(value.getString(str_लंबाई, errorCode), errorCode);
                    अगर (U_FAILURE(errorCode)) { वापस false; }
                }
            }
        }
        वापस true;
    }
    UnicodeString toLanguage(पूर्णांक encoded) {
        अगर (encoded == 0) {
            वापस UNICODE_STRING_SIMPLE("");
        }
        अगर (encoded == 1) {
            वापस UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        अक्षर lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        अगर (encoded / (27 * 27) == 0) {
            वापस UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        वापस UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(पूर्णांक encoded) {
        अगर (encoded == 0) {
            वापस UNICODE_STRING_SIMPLE("");
        }
        अगर (encoded == 1) {
            वापस UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        स्थिर अक्षर* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        अगर (script == nullptr) {
            वापस UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        वापस UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49_सूचकांक_To_Code(स्थिर ResourceArray &m49Array, Resource_मूल्य &value, पूर्णांक सूचकांक, UErrorCode &errorCode) {
        अगर (U_FAILURE(errorCode)) {
            वापस UNICODE_STRING_SIMPLE("");
        }
        अगर (m49Array.get_मूल्य(सूचकांक, value)) {
            वापस value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        वापस UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(स्थिर ResourceArray& m49Array, Resource_मूल्य &value, पूर्णांक encoded, UErrorCode &errorCode) {
        अगर (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            वापस UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        अगर (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            वापस  m49_सूचकांक_To_Code(m49Array, value, encoded, errorCode);
        }
        अक्षर region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        वापस UnicodeString(region, 2, US_INV);
    }

    bool readLSREncodedStrings(स्थिर ResourceTable &table, स्थिर अक्षर* कुंजी, Resource_मूल्य &value, स्थिर ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        अगर (U_FAILURE(errorCode)) { वापस false; }
        अगर (table.find_मूल्य(कुंजी, value)) {
            स्थिर int32_t* vectors = value.getIntVector(length, errorCode);
            अगर (U_FAILURE(errorCode)) { वापस false; }
            अगर (length == 0) { वापस true; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            अगर (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                वापस false;
            }
            क्रम (पूर्णांक i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.add_By_मूल्य(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.add_By_मूल्य(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.add_By_मूल्य(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                अगर (U_FAILURE(errorCode)) { वापस false; }
            }
            length *= 3;
        }
        वापस true;
    }
};

namespace {

LikelySubtags *gLikelySubtags = nullptr;
UVector *gMacroregions = nullptr;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    मिटाओ gLikelySubtags;
    gLikelySubtags = nullptr;
    मिटाओ gMacroregions;
    gMacroregions = nullptr;
    gInitOnce.reset();
    वापस true;
}

constexpr स्थिर char16_t* MACROREGION_HARDCODE[] = {
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
व्योम processMacroregionRange(स्थिर UnicodeString& regionName, UVector* newMacroRegions, UErrorCode& status) {
    अगर (U_FAILURE(status)) { वापस; }
    int32_t rangeMarkerLocation = regionName.सूचकांक_Of(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    अगर ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        जबतक ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(नया UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } अन्यथा {
        LocalPointer<UnicodeString> newRegion(नया UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#अगर U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    अगर (U_FAILURE(status)) { वापस nullptr; }
    LocalPointer<UVector> newMacroRegions(नया UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplemental_तथ्य(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_get_By_कुंजी(supplemental_तथ्य.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_get_By_कुंजी(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_get_By_कुंजी(regionList.getAlias(),"macroregion",nullptr,&status));

    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }

    जबतक (ures_has_अग्र(regionMacro.getAlias())) {
        UnicodeString regionName = ures_get_अग्र_Unicode_String(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        अगर (U_FAILURE(status)) {
            वापस nullptr;
        }
    }

    वापस newMacroRegions.orphan();
}
#पूर्ण_अगर // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    अगर (U_FAILURE(status)) { वापस nullptr; }
    LocalPointer<UVector> newMacroRegions(नया UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }

    क्रम (स्थिर स्वतः *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        अगर (U_FAILURE(status)) {
            वापस nullptr;
        }
    }

    वापस newMacroRegions.orphan();
}

}  // namespace

व्योम U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    Likely_Subtags_तथ्य तथ्य(errorCode);
    तथ्य.load(errorCode);
    अगर (U_FAILURE(errorCode)) { वापस; }
    gLikelySubtags = नया LikelySubtags(तथ्य);
    gMacroregions = getStaticMacroregions(errorCode);
#अगर U_DEBUG
    स्वतः macroregions_From_तथ्य = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregions_From_तथ्य));
    मिटाओ macroregions_From_तथ्य;
#पूर्ण_अगर
    अगर (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        मिटाओ gLikelySubtags;
        मिटाओ gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

स्थिर LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    वापस gLikelySubtags;
}

LikelySubtags::LikelySubtags(Likely_Subtags_तथ्य &तथ्य) :
        langInfoBundle(तथ्य.langInfoBundle),
        strings(तथ्य.strings.orphanCharStrings()),
        languageAliases(std::move(तथ्य.languageAliases)),
        regionAliases(std::move(तथ्य.regionAliases)),
        trie(तथ्य.trieBytes),
        lsrs(तथ्य.lsrs),
#अगर U_DEBUG
        lsrs_लंबाई(तथ्य.lsrs_लंबाई),
#पूर्ण_अगर // U_DEBUG
        distance_तथ्य(std::move(तथ्य.distance_तथ्य)) {
    तथ्य.langInfoBundle = nullptr;
    तथ्य.lsrs = nullptr;

    // Cache the result of looking up language="und" encoded as "*", and "und-Zzzz" ("**").
    UString_Trie_परिणाम परिणाम = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_अग्र(परिणाम));
    trieUndState = trie.getState64();
    परिणाम = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_अग्र(परिणाम));
    trieUndZzzzState = trie.getState64();
    परिणाम = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_मूल्य(परिणाम));
    default_Lsr_सूचकांक = trie.get_मूल्य();
    trie.reset();

    क्रम (char16_t c = u'a'; c <= u'z'; ++c) {
        परिणाम = trie.next(c);
        अगर (परिणाम == USTRINGTRIE_NO_मूल्य) {
            trieFirstLetterStates[c - u'a'] = trie.getState64();
        }
        trie.reset();
    }
}

LikelySubtags::~LikelySubtags() {
    ures_close(langInfoBundle);
    मिटाओ strings;
    मिटाओ[] lsrs;
}

LSR LikelySubtags::makeMaximizedLsrFrom(स्थिर Locale &locale,
                                         bool return_निवेश_If_Unmatch,
                                         UErrorCode &errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) { वापस {}; }
    अगर (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस {};
    }
    स्थिर अक्षर *name = locale.getName();
    अगर (!return_निवेश_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        वापस LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_निवेश_If_Unmatch, errorCode);

    अगर (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        वापस LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    वापस max;
}

namespace {

स्थिर अक्षर *getCanonical(स्थिर CharStringMap &aliases, स्थिर अक्षर *alias) {
    स्थिर अक्षर *canonical = aliases.लो(alias);
    वापस canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(स्थिर अक्षर *language, स्थिर अक्षर *script, स्थिर अक्षर *region,
                                     स्थिर अक्षर *variant,
                                     bool return_निवेश_If_Unmatch,
                                     UErrorCode &errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) { वापस {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    अगर (!return_निवेश_If_Unmatch) {
        अक्षर c1;
        अगर (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            चयन (c1) {
            हाल 'A':
                वापस LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            हाल 'B':
                वापस LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            हाल 'C':
                वापस LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            शेष:  // normal locale
                अवरोध;
            }
        }

        अगर (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            अगर (uprv_strcmp(variant, "PSACCENT") == 0) {
                वापस LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } अन्यथा अगर (uprv_strcmp(variant, "PSBIDI") == 0) {
                वापस LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } अन्यथा अगर (uprv_strcmp(variant, "PSCRACK") == 0) {
                वापस LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    वापस maximize(language, script, region, return_निवेश_If_Unmatch, errorCode);
}

LSR LikelySubtags::maximize(स्थिर अक्षर *language, स्थिर अक्षर *script, स्थिर अक्षर *region,
                             bool return_निवेश_If_Unmatch,
                             UErrorCode &errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) { वापस {}; }
    वापस maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_निवेश_If_Unmatch,
                    errorCode);
}

bool LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) { वापस false; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    अगर (U_FAILURE(errorCode)) { वापस false; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    वापस gMacroregions->contains((व्योम *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             bool return_निवेश_If_Unmatch,
                             UErrorCode &errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) { वापस {}; }
    अगर (language.compare("und") == 0) {
        language = "";
    }
    अगर (script.compare("Zzzz") == 0) {
        script = "";
    }
    अगर (region.compare("ZZ") == 0) {
        region = "";
    }
    अगर (!script.empty() && !region.empty() && !language.empty()) {
        वापस LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    bool retainLanguage = false;
    bool retainScript = false;
    bool retainRegion = false;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    अगर (0 <= (c0 = uprv_lowerOrdinal(language.तथ्य()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trie_अग्र(iter.resetToState64(state), language, 1);
    } अन्यथा {
        value = trie_अग्र(iter, language, 0);
    }
    bool matchLanguage = (value >= 0);
    bool matchScript = false;
    अगर (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } अन्यथा {
        retainLanguage = true;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    अगर (value >= 0 && !script.empty()) {
        matchScript = true;
    }
    अगर (value > 0) {
        // Intermediate or final value from just language.
        अगर (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } अन्यथा {
        value = trie_अग्र(iter, script, 0);
        अगर (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } अन्यथा {
            retainScript = true;
            अगर (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } अन्यथा {
                iter.resetToState64(state);
                value = trie_अग्र(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    bool matchRegion = false;
    अगर (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } अन्यथा {
        value = trie_अग्र(iter, region, 0);
        अगर (value >= 0) {
            अगर (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = true;
                matchRegion = true;
            }
        } अन्यथा {
            retainRegion = true;
            अगर (state == 0) {
                value = default_Lsr_सूचकांक;
            } अन्यथा {
                iter.resetToState64(state);
                value = trie_अग्र(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                अगर (value < 0) {
                    retainLanguage = !language.empty();
                    retainScript = !script.empty();
                    retainRegion = !region.empty();
                    // Fallback to und_$region =>
                    iter.resetToState64(trieUndState);  // "und" ("*")
                    value = trie_अग्र(iter, "", 0);
                    U_ASSERT(value == 0);
                    int64_t trieUndEmptyState = iter.getState64();
                    value = trie_अग्र(iter, region, 0);
                    // Fallback to und =>
                    अगर (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trie_अग्र(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrs_लंबाई);
    अगर (return_निवेश_If_Unmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      वापस LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    अगर (language.empty()) {
        language = StringPiece("und");
    }

    अगर (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        स्थिर LSR &matched = lsrs[value];
        वापस LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    अगर (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    अगर (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    अगर (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    वापस LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(स्थिर LSR &lsr, स्थिर LSR &other, int32_t likelyInfo) स्थिर {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    अगर (uprv_strcmp(lsr.language, other.language) != 0) {
        वापस 0xfffffffc;  // negative, lsr not better than other
    }
    अगर (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t सूचकांक;
        अगर (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            सूचकांक = likelyInfo >> 2;
        } अन्यथा {
            सूचकांक = get_Likely_सूचकांक(lsr.language, "");
            likelyInfo = सूचकांक << 2;
        }
        स्थिर LSR &likely = lsrs[सूचकांक];
        अगर (uprv_strcmp(lsr.script, likely.script) == 0) {
            वापस likelyInfo | 1;
        } अन्यथा {
            वापस likelyInfo & ~1;
        }
    }
    अगर (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t सूचकांक;
        अगर (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            सूचकांक = likelyInfo >> 2;
        } अन्यथा {
            सूचकांक = get_Likely_सूचकांक(lsr.language, lsr.region);
            likelyInfo = (सूचकांक << 2) | 2;
        }
        स्थिर LSR &likely = lsrs[सूचकांक];
        अगर (uprv_strcmp(lsr.region, likely.region) == 0) {
            वापस likelyInfo | 1;
        } अन्यथा {
            वापस likelyInfo & ~1;
        }
    }
    वापस likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::get_Likely_सूचकांक(स्थिर अक्षर *language, स्थिर अक्षर *script) स्थिर {
    अगर (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    अगर (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    अगर (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trie_अग्र(iter.resetToState64(state), language, 1);
    } अन्यथा {
        value = trie_अग्र(iter, language, 0);
    }
    अगर (value >= 0) {
        state = iter.getState64();
    } अन्यथा {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    अगर (value > 0) {
        // Intermediate or final value from just language.
        अगर (value == SKIP_SCRIPT) {
            value = 0;
        }
    } अन्यथा {
        value = trie_अग्र(iter, script, 0);
        अगर (value >= 0) {
            state = iter.getState64();
        } अन्यथा {
            अगर (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } अन्यथा {
                iter.resetToState64(state);
                value = trie_अग्र(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    अगर (value > 0) {
        // Final value from just language or language+script.
    } अन्यथा {
        value = trie_अग्र(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrs_लंबाई);
    वापस value;
}

int32_t LikelySubtags::trie_अग्र(BytesTrie &iter, स्थिर अक्षर *s, int32_t i) {
    UString_Trie_परिणाम परिणाम;
    uint8_t c;
    अगर ((c = s[i]) == 0) {
        परिणाम = iter.next(u'*');
    } अन्यथा {
        क्रम (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            अगर (next != 0) {
                अगर (!USTRINGTRIE_HAS_अग्र(iter.next(c))) {
                    वापस -1;
                }
            } अन्यथा {
                // last character of this subtag
                परिणाम = iter.next(c | 0x80);
                अवरोध;
            }
            c = next;
        }
    }
    चयन (परिणाम) {
    हाल USTRINGTRIE_NO_MATCH: वापस -1;
    हाल USTRINGTRIE_NO_मूल्य: वापस 0;
    हाल USTRINGTRIE_INTERMEDIATE_मूल्य:
        U_ASSERT(iter.get_मूल्य() == SKIP_SCRIPT);
        वापस SKIP_SCRIPT;
    हाल USTRINGTRIE_FINAL_मूल्य: वापस iter.get_मूल्य();
    शेष: वापस -1;
    }
}
int32_t LikelySubtags::trie_अग्र(BytesTrie &iter, StringPiece s, int32_t i) {
    UString_Trie_परिणाम परिणाम;
    uint8_t c;
    अगर (s.length() == i) {
        परिणाम = iter.next(u'*');
    } अन्यथा {
        c = s.तथ्य()[i];
        क्रम (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            अगर (i+1 != s.length()) {
                अगर (!USTRINGTRIE_HAS_अग्र(iter.next(c))) {
                    वापस -1;
                }
                c = s.तथ्य()[++i];
            } अन्यथा {
                // last character of this subtag
                परिणाम = iter.next(c | 0x80);
                अवरोध;
            }
        }
    }
    चयन (परिणाम) {
    हाल USTRINGTRIE_NO_MATCH: वापस -1;
    हाल USTRINGTRIE_NO_मूल्य: वापस 0;
    हाल USTRINGTRIE_INTERMEDIATE_मूल्य:
        U_ASSERT(iter.get_मूल्य() == SKIP_SCRIPT);
        वापस SKIP_SCRIPT;
    हाल USTRINGTRIE_FINAL_मूल्य: वापस iter.get_मूल्य();
    शेष: वापस -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    bool favorScript,
                                    UErrorCode &errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) { वापस {}; }
    LSR max = maximize(language, script, region, true, errorCode);
    अगर (U_FAILURE(errorCode)) { वापस {}; }
    // If no match, return it.
    अगर (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        वापस LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", true, errorCode);
    अगर (U_FAILURE(errorCode)) { वापस {}; }
    अगर (test.isEquivalentTo(max)) {
        वापस LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    अगर (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, true, errorCode);
        अगर (U_FAILURE(errorCode)) { वापस {}; }
        अगर (test.isEquivalentTo(max)) {
            वापस LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", true, errorCode);
    अगर (U_FAILURE(errorCode)) { वापस {}; }
    अगर (test.isEquivalentTo(max)) {
        वापस LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    अगर (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, true, errorCode);
        अगर (U_FAILURE(errorCode)) { वापस {}; }
        अगर (test.isEquivalentTo(max)) {
            वापस LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    वापस LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
