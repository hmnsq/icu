// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#include <utility>
#include "unicode/utypes.h"
#include "unicode/bytestrie.h"
#include "unicode/localpointer.h"
#include "unicode/locid.h"
#include "unicode/uobject.h"
#include "unicode/ures.h"
#include "unicode/uscript.h"
#include "charstr.h"
#include "cstring.h"
#include "loclikelysubtags.h"
#include "lsr.h"
#include "uassert.h"
#include "ucln_cmn.h"
#include "uhash.h"
#include "uinvchar.h"
#include "umutex.h"
#include "uniquecharstr.h"
#include "uresdata.h"
#include "uresimp.h"
#include "uvector.h"

U_NAMESPACE_BEGIN

namespace {

constexpr var_na PSEUDO_ACCENTS_PREFIX = '\'';  // -XA, -PSACCENT
constexpr var_na PSEUDO_BIDI_PREFIX = '+';  // -XB, -PSBIDI
constexpr var_na PSEUDO_CRACKED_PREFIX = ',';  // -XC, -PSCRACK

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

sa_nracanaa LikelySubtagsData {
    UResourceBundle *langInfoBundle = nullptr;
    UniqueCharStrings strings;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    const uint8_t *trieBytes = nullptr;
    LSR *lsrs = nullptr;
    int32_t lsrsLength = 0;

    LocaleDistanceData distanceData;

    LikelySubtagsData(UErrorCode &errorCode) : strings(errorCode) {}

    ~LikelySubtagsData() {
        ures_close(langInfoBundle);
        delete[] lsrs;
    }

    void load(UErrorCode &errorCode) {
        නම් (U_FAILURE(errorCode)) { ආපසු; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        නම් (U_FAILURE(errorCode)) { ආපසු; }
        StackUResourceBundle stackTempBundle;
        ResourceDataValue value;
        ures_getValueWithFallback(langInfoBundle, "likely", stackTempBundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        නම් (U_FAILURE(errorCode)) { ආපසු; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        නම් (likelyTable.findValue("m49", value)) {
            m49Array = value.getArray(errorCode);
        } නැතහොත් {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ආපසු;
        }
        නම් (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            ආපසු;
        }
        නම් ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            ආපසු;
        }
        නම් (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ආපසු;
        }

        නම් (!likelyTable.findValue("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ආපසු;
        }
        int32_t length;
        trieBytes = value.getBinary(length, errorCode);
        නම් (U_FAILURE(errorCode)) { ආපසු; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_getValueWithFallback(langInfoBundle, "match", stackTempBundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        නම් (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            නම් (U_FAILURE(errorCode)) { ආපසු; }

            නම් (matchTable.findValue("trie", value)) {
                distanceData.distanceTrieBytes = value.getBinary(length, errorCode);
                නම් (U_FAILURE(errorCode)) { ආපසු; }
            }

            නම් (matchTable.findValue("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.getBinary(length, errorCode);
                නම් (U_FAILURE(errorCode)) { ආපසු; }
                නම් (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    ආපසු;
                }
            }

            නම් (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                ආපසු;
            }
            නම් ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                ආපසු;
            }

            නම් (matchTable.findValue("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                නම් (U_FAILURE(errorCode)) { ආපසු; }
                නම් (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    ආපසු;
                }
            }
        } නැතහොත් නම් (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } නැතහොත් {  // error other than missing resource
            errorCode = matchErrorCode;
            ආපසු;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languagesLength / 2, errorCode);
        සඳහා (int32_t i = 0; i < languagesLength; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regionsLength / 2, errorCode);
        සඳහා (int32_t i = 0; i < regionsLength; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        නම් (U_FAILURE(errorCode)) { ආපසු; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = new LSR[lsrsLength];
        නම් (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ආපසු;
        }
        සඳහා (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        නම් (partitionsLength > 0) {
            distanceData.partitions = static_cast<const var_na **>(
                uprv_malloc(partitionsLength * parimaa_na(const var_na *)));
            නම් (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ආපසු;
            }
            සඳහා (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        නම් (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = new LSR[distanceData.paradigmsLength];
            නම් (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ආපසු;
            }
            සඳහා (int32_t i = 0, j = 0; i < paradigmSubtagsLength; i += 3, ++j) {
                paradigms[j] = LSR(strings.get(paradigmSubtagIndexes[i]),
                                   strings.get(paradigmSubtagIndexes[i + 1]),
                                   strings.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            distanceData.paradigms = paradigms;
        }
    }

private:
    bool readStrings(const ResourceTable &table, const var_na *key, ResourceValue &value,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        නම් (U_FAILURE(errorCode)) { ආපසු false; }
        නම් (table.findValue(key, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            නම් (U_FAILURE(errorCode)) { ආපසු false; }
            length = stringArray.getSize();
            නම් (length == 0) { ආපසු true; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            නම් (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ආපසු false;
            }
            සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < length; ++i) {
                නම් (stringArray.getValue(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    නම් (U_FAILURE(errorCode)) { ආපසු false; }
                }
            }
        }
        ආපසු true;
    }
    UnicodeString toLanguage(පූර්ණ_සංඛ්‍යා encoded) {
        නම් (encoded == 0) {
            ආපසු UNICODE_STRING_SIMPLE("");
        }
        නම් (encoded == 1) {
            ආපසු UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        var_na lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        නම් (encoded / (27 * 27) == 0) {
            ආපසු UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        ආපසු UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(පූර්ණ_සංඛ්‍යා encoded) {
        නම් (encoded == 0) {
            ආපසු UNICODE_STRING_SIMPLE("");
        }
        නම් (encoded == 1) {
            ආපසු UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        const var_na* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        නම් (script == nullptr) {
            ආපසු UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        ආපසු UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(const ResourceArray &m49Array, ResourceValue &value, පූර්ණ_සංඛ්‍යා index, UErrorCode &errorCode) {
        නම් (U_FAILURE(errorCode)) {
            ආපසු UNICODE_STRING_SIMPLE("");
        }
        නම් (m49Array.getValue(index, value)) {
            ආපසු value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        ආපසු UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(const ResourceArray& m49Array, ResourceValue &value, පූර්ණ_සංඛ්‍යා encoded, UErrorCode &errorCode) {
        නම් (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            ආපසු UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        නම් (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            ආපසු  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        var_na region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        ආපසු UnicodeString(region, 2, US_INV);
    }

    bool readLSREncodedStrings(const ResourceTable &table, const var_na* key, ResourceValue &value, const ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        නම් (U_FAILURE(errorCode)) { ආපසු false; }
        නම් (table.findValue(key, value)) {
            const int32_t* vectors = value.getIntVector(length, errorCode);
            නම් (U_FAILURE(errorCode)) { ආපසු false; }
            නම් (length == 0) { ආපසු true; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            නම් (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ආපසු false;
            }
            සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.addByValue(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.addByValue(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.addByValue(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                නම් (U_FAILURE(errorCode)) { ආපසු false; }
            }
            length *= 3;
        }
        ආපසු true;
    }
};

namespace {

LikelySubtags *gLikelySubtags = nullptr;
UVector *gMacroregions = nullptr;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    delete gLikelySubtags;
    gLikelySubtags = nullptr;
    delete gMacroregions;
    gMacroregions = nullptr;
    gInitOnce.reset();
    ආපසු true;
}

constexpr const char16_t* MACROREGION_HARDCODE[] = {
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
void processMacroregionRange(const UnicodeString& regionName, UVector* newMacroRegions, UErrorCode& status) {
    නම් (U_FAILURE(status)) { ආපසු; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    නම් ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        අතරතුර ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(new UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } නැතහොත් {
        LocalPointer<UnicodeString> newRegion(new UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#නම් U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    නම් (U_FAILURE(status)) { ආපසු nullptr; }
    LocalPointer<UVector> newMacroRegions(new UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_getByKey(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_getByKey(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_getByKey(regionList.getAlias(),"macroregion",nullptr,&status));

    නම් (U_FAILURE(status)) {
        ආපසු nullptr;
    }

    අතරතුර (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        නම් (U_FAILURE(status)) {
            ආපසු nullptr;
        }
    }

    ආපසු newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    නම් (U_FAILURE(status)) { ආපසු nullptr; }
    LocalPointer<UVector> newMacroRegions(new UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    නම් (U_FAILURE(status)) {
        ආපසු nullptr;
    }

    සඳහා (const auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        නම් (U_FAILURE(status)) {
            ආපසු nullptr;
        }
    }

    ආපසු newMacroRegions.orphan();
}

}  // namespace

void U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    නම් (U_FAILURE(errorCode)) { ආපසු; }
    gLikelySubtags = new LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#නම් U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    නම් (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ආපසු;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

const LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    නම් (U_FAILURE(errorCode)) { ආපසු nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    ආපසු gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#නම් U_DEBUG
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

    සඳහා (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        නම් (result == USTRINGTRIE_NO_VALUE) {
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

LSR LikelySubtags::makeMaximizedLsrFrom(const Locale &locale,
                                         bool returnInputIfUnmatch,
                                         UErrorCode &errorCode) const {
    නම් (U_FAILURE(errorCode)) { ආපසු {}; }
    නම් (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ආපසු {};
    }
    const var_na *name = locale.getName();
    නම් (!returnInputIfUnmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        ආපසු LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), returnInputIfUnmatch, errorCode);

    නම් (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        ආපසු LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    ආපසු max;
}

namespace {

const var_na *getCanonical(const CharStringMap &aliases, const var_na *alias) {
    const var_na *canonical = aliases.get(alias);
    ආපසු canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(const var_na *language, const var_na *script, const var_na *region,
                                     const var_na *variant,
                                     bool returnInputIfUnmatch,
                                     UErrorCode &errorCode) const {
    නම් (U_FAILURE(errorCode)) { ආපසු {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    නම් (!returnInputIfUnmatch) {
        var_na c1;
        නම් (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            switch (c1) {
            pak_sa 'A':
                ආපසු LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            pak_sa 'B':
                ආපසු LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            pak_sa 'C':
                ආපසු LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            default:  // normal locale
                bha_nga;
            }
        }

        නම් (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            නම් (uprv_strcmp(variant, "PSACCENT") == 0) {
                ආපසු LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } නැතහොත් නම් (uprv_strcmp(variant, "PSBIDI") == 0) {
                ආපසු LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } නැතහොත් නම් (uprv_strcmp(variant, "PSCRACK") == 0) {
                ආපසු LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    ආපසු maximize(language, script, region, returnInputIfUnmatch, errorCode);
}

LSR LikelySubtags::maximize(const var_na *language, const var_na *script, const var_na *region,
                             bool returnInputIfUnmatch,
                             UErrorCode &errorCode) const {
    නම් (U_FAILURE(errorCode)) { ආපසු {}; }
    ආපසු maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    returnInputIfUnmatch,
                    errorCode);
}

bool LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) const {
    නම් (U_FAILURE(errorCode)) { ආපසු false; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    නම් (U_FAILURE(errorCode)) { ආපසු false; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    ආපසු gMacroregions->contains((void *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             bool returnInputIfUnmatch,
                             UErrorCode &errorCode) const {
    නම් (U_FAILURE(errorCode)) { ආපසු {}; }
    නම් (language.compare("und") == 0) {
        language = "";
    }
    නම් (script.compare("Zzzz") == 0) {
        script = "";
    }
    නම් (region.compare("ZZ") == 0) {
        region = "";
    }
    නම් (!script.empty() && !region.empty() && !language.empty()) {
        ආපසු LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    bool retainLanguage = false;
    bool retainScript = false;
    bool retainRegion = false;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    නම් (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } නැතහොත් {
        value = trieNext(iter, language, 0);
    }
    bool matchLanguage = (value >= 0);
    bool matchScript = false;
    නම් (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } නැතහොත් {
        retainLanguage = true;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    නම් (value >= 0 && !script.empty()) {
        matchScript = true;
    }
    නම් (value > 0) {
        // Intermediate or final value from just language.
        නම් (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } නැතහොත් {
        value = trieNext(iter, script, 0);
        නම් (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } නැතහොත් {
            retainScript = true;
            නම් (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } නැතහොත් {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    bool matchRegion = false;
    නම් (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } නැතහොත් {
        value = trieNext(iter, region, 0);
        නම් (value >= 0) {
            නම් (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = true;
                matchRegion = true;
            }
        } නැතහොත් {
            retainRegion = true;
            නම් (state == 0) {
                value = defaultLsrIndex;
            } නැතහොත් {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                නම් (value < 0) {
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
                    නම් (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    නම් (returnInputIfUnmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      ආපසු LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    නම් (language.empty()) {
        language = StringPiece("und");
    }

    නම් (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        const LSR &matched = lsrs[value];
        ආපසු LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    නම් (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    නම් (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    නම් (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    ආපසු LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(const LSR &lsr, const LSR &other, int32_t likelyInfo) const {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    නම් (uprv_strcmp(lsr.language, other.language) != 0) {
        ආපසු 0xfffffffc;  // negative, lsr not better than other
    }
    නම් (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        නම් (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } නැතහොත් {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        const LSR &likely = lsrs[index];
        නම් (uprv_strcmp(lsr.script, likely.script) == 0) {
            ආපසු likelyInfo | 1;
        } නැතහොත් {
            ආපසු likelyInfo & ~1;
        }
    }
    නම් (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        නම් (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } නැතහොත් {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        const LSR &likely = lsrs[index];
        නම් (uprv_strcmp(lsr.region, likely.region) == 0) {
            ආපසු likelyInfo | 1;
        } නැතහොත් {
            ආපසු likelyInfo & ~1;
        }
    }
    ආපසු likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(const var_na *language, const var_na *script) const {
    නම් (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    නම් (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    නම් (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } නැතහොත් {
        value = trieNext(iter, language, 0);
    }
    නම් (value >= 0) {
        state = iter.getState64();
    } නැතහොත් {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    නම් (value > 0) {
        // Intermediate or final value from just language.
        නම් (value == SKIP_SCRIPT) {
            value = 0;
        }
    } නැතහොත් {
        value = trieNext(iter, script, 0);
        නම් (value >= 0) {
            state = iter.getState64();
        } නැතහොත් {
            නම් (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } නැතහොත් {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    නම් (value > 0) {
        // Final value from just language or language+script.
    } නැතහොත් {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    ආපසු value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, const var_na *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    නම් ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } නැතහොත් {
        සඳහා (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            නම් (next != 0) {
                නම් (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    ආපසු -1;
                }
            } නැතහොත් {
                // last character of this subtag
                result = iter.next(c | 0x80);
                bha_nga;
            }
            c = next;
        }
    }
    switch (result) {
    pak_sa USTRINGTRIE_NO_MATCH: ආපසු -1;
    pak_sa USTRINGTRIE_NO_VALUE: ආපසු 0;
    pak_sa USTRINGTRIE_INTERMEDIATE_VALUE:
        U_ASSERT(iter.getValue() == SKIP_SCRIPT);
        ආපසු SKIP_SCRIPT;
    pak_sa USTRINGTRIE_FINAL_VALUE: ආපසු iter.getValue();
    default: ආපසු -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    නම් (s.length() == i) {
        result = iter.next(u'*');
    } නැතහොත් {
        c = s.data()[i];
        සඳහා (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            නම් (i+1 != s.length()) {
                නම් (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    ආපසු -1;
                }
                c = s.data()[++i];
            } නැතහොත් {
                // last character of this subtag
                result = iter.next(c | 0x80);
                bha_nga;
            }
        }
    }
    switch (result) {
    pak_sa USTRINGTRIE_NO_MATCH: ආපසු -1;
    pak_sa USTRINGTRIE_NO_VALUE: ආපසු 0;
    pak_sa USTRINGTRIE_INTERMEDIATE_VALUE:
        U_ASSERT(iter.getValue() == SKIP_SCRIPT);
        ආපසු SKIP_SCRIPT;
    pak_sa USTRINGTRIE_FINAL_VALUE: ආපසු iter.getValue();
    default: ආපසු -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    bool favorScript,
                                    UErrorCode &errorCode) const {
    නම් (U_FAILURE(errorCode)) { ආපසු {}; }
    LSR max = maximize(language, script, region, true, errorCode);
    නම් (U_FAILURE(errorCode)) { ආපසු {}; }
    // If no match, return it.
    නම් (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        ආපසු LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", true, errorCode);
    නම් (U_FAILURE(errorCode)) { ආපසු {}; }
    නම් (test.isEquivalentTo(max)) {
        ආපසු LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    නම් (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, true, errorCode);
        නම් (U_FAILURE(errorCode)) { ආපසු {}; }
        නම් (test.isEquivalentTo(max)) {
            ආපසු LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", true, errorCode);
    නම් (U_FAILURE(errorCode)) { ආපසු {}; }
    නම් (test.isEquivalentTo(max)) {
        ආපසු LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    නම් (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, true, errorCode);
        නම් (U_FAILURE(errorCode)) { ආපසු {}; }
        නම් (test.isEquivalentTo(max)) {
            ආපසු LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    ආපසු LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
