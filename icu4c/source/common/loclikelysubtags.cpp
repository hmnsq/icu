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
        ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }
        StackUResourceBundle stackTempBundle;
        ResourceDataValue value;
        ures_getValueWithFallback(langInfoBundle, "likely", stackTempBundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        ᨀᨘ (likelyTable.findValue("m49", value)) {
            m49Array = value.getArray(errorCode);
        } ᨀᨘᨁ {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ᨀᨘᨒ;
        }
        ᨀᨘ (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            ᨀᨘᨒ;
        }
        ᨀᨘ ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            ᨀᨘᨒ;
        }
        ᨀᨘ (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ᨀᨘᨒ;
        }

        ᨀᨘ (!likelyTable.findValue("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ᨀᨘᨒ;
        }
        int32_t length;
        trieBytes = value.getBinary(length, errorCode);
        ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_getValueWithFallback(langInfoBundle, "match", stackTempBundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        ᨀᨘ (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }

            ᨀᨘ (matchTable.findValue("trie", value)) {
                distanceData.distanceTrieBytes = value.getBinary(length, errorCode);
                ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }
            }

            ᨀᨘ (matchTable.findValue("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.getBinary(length, errorCode);
                ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }
                ᨀᨘ (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    ᨀᨘᨒ;
                }
            }

            ᨀᨘ (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                ᨀᨘᨒ;
            }
            ᨀᨘ ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                ᨀᨘᨒ;
            }

            ᨀᨘ (matchTable.findValue("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }
                ᨀᨘ (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    ᨀᨘᨒ;
                }
            }
        } ᨀᨘᨁ ᨀᨘ (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } ᨀᨘᨁ {  // error other than missing resource
            errorCode = matchErrorCode;
            ᨀᨘᨒ;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languagesLength / 2, errorCode);
        for (int32_t i = 0; i < languagesLength; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regionsLength / 2, errorCode);
        for (int32_t i = 0; i < regionsLength; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = new LSR[lsrsLength];
        ᨀᨘ (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ᨀᨘᨒ;
        }
        for (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        ᨀᨘ (partitionsLength > 0) {
            distanceData.partitions = static_cast<const var_na **>(
                uprv_malloc(partitionsLength * parimaa_na(const var_na *)));
            ᨀᨘ (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ᨀᨘᨒ;
            }
            for (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        ᨀᨘ (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = new LSR[distanceData.paradigmsLength];
            ᨀᨘ (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ᨀᨘᨒ;
            }
            for (int32_t i = 0, j = 0; i < paradigmSubtagsLength; i += 3, ++j) {
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
        ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ false; }
        ᨀᨘ (table.findValue(key, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ false; }
            length = stringArray.getSize();
            ᨀᨘ (length == 0) { ᨀᨘᨒ true; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            ᨀᨘ (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ᨀᨘᨒ false;
            }
            for (puur_naa_nka i = 0; i < length; ++i) {
                ᨀᨘ (stringArray.getValue(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ false; }
                }
            }
        }
        ᨀᨘᨒ true;
    }
    UnicodeString toLanguage(puur_naa_nka encoded) {
        ᨀᨘ (encoded == 0) {
            ᨀᨘᨒ UNICODE_STRING_SIMPLE("");
        }
        ᨀᨘ (encoded == 1) {
            ᨀᨘᨒ UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        var_na lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        ᨀᨘ (encoded / (27 * 27) == 0) {
            ᨀᨘᨒ UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        ᨀᨘᨒ UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(puur_naa_nka encoded) {
        ᨀᨘ (encoded == 0) {
            ᨀᨘᨒ UNICODE_STRING_SIMPLE("");
        }
        ᨀᨘ (encoded == 1) {
            ᨀᨘᨒ UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        const var_na* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        ᨀᨘ (script == nullptr) {
            ᨀᨘᨒ UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        ᨀᨘᨒ UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(const ResourceArray &m49Array, ResourceValue &value, puur_naa_nka index, UErrorCode &errorCode) {
        ᨀᨘ (U_FAILURE(errorCode)) {
            ᨀᨘᨒ UNICODE_STRING_SIMPLE("");
        }
        ᨀᨘ (m49Array.getValue(index, value)) {
            ᨀᨘᨒ value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        ᨀᨘᨒ UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(const ResourceArray& m49Array, ResourceValue &value, puur_naa_nka encoded, UErrorCode &errorCode) {
        ᨀᨘ (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            ᨀᨘᨒ UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        ᨀᨘ (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            ᨀᨘᨒ  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        var_na region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        ᨀᨘᨒ UnicodeString(region, 2, US_INV);
    }

    bool readLSREncodedStrings(const ResourceTable &table, const var_na* key, ResourceValue &value, const ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ false; }
        ᨀᨘ (table.findValue(key, value)) {
            const int32_t* vectors = value.getIntVector(length, errorCode);
            ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ false; }
            ᨀᨘ (length == 0) { ᨀᨘᨒ true; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            ᨀᨘ (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ᨀᨘᨒ false;
            }
            for (puur_naa_nka i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.addByValue(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.addByValue(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.addByValue(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ false; }
            }
            length *= 3;
        }
        ᨀᨘᨒ true;
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
    ᨀᨘᨒ true;
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
    ᨀᨘ (U_FAILURE(status)) { ᨀᨘᨒ; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    ᨀᨘ ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        ᨀᨛ ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(new UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } ᨀᨘᨁ {
        LocalPointer<UnicodeString> newRegion(new UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#ᨀᨘ U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    ᨀᨘ (U_FAILURE(status)) { ᨀᨘᨒ nullptr; }
    LocalPointer<UVector> newMacroRegions(new UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_getByKey(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_getByKey(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_getByKey(regionList.getAlias(),"macroregion",nullptr,&status));

    ᨀᨘ (U_FAILURE(status)) {
        ᨀᨘᨒ nullptr;
    }

    ᨀᨛ (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        ᨀᨘ (U_FAILURE(status)) {
            ᨀᨘᨒ nullptr;
        }
    }

    ᨀᨘᨒ newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    ᨀᨘ (U_FAILURE(status)) { ᨀᨘᨒ nullptr; }
    LocalPointer<UVector> newMacroRegions(new UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    ᨀᨘ (U_FAILURE(status)) {
        ᨀᨘᨒ nullptr;
    }

    for (const auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        ᨀᨘ (U_FAILURE(status)) {
            ᨀᨘᨒ nullptr;
        }
    }

    ᨀᨘᨒ newMacroRegions.orphan();
}

}  // namespace

void U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }
    gLikelySubtags = new LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#ᨀᨘ U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    ᨀᨘ (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ᨀᨘᨒ;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

const LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    ᨀᨘᨒ gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#ᨀᨘ U_DEBUG
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

    for (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        ᨀᨘ (result == USTRINGTRIE_NO_VALUE) {
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
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ {}; }
    ᨀᨘ (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ᨀᨘᨒ {};
    }
    const var_na *name = locale.getName();
    ᨀᨘ (!returnInputIfUnmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        ᨀᨘᨒ LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), returnInputIfUnmatch, errorCode);

    ᨀᨘ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        ᨀᨘᨒ LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    ᨀᨘᨒ max;
}

namespace {

const var_na *getCanonical(const CharStringMap &aliases, const var_na *alias) {
    const var_na *canonical = aliases.get(alias);
    ᨀᨘᨒ canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(const var_na *language, const var_na *script, const var_na *region,
                                     const var_na *variant,
                                     bool returnInputIfUnmatch,
                                     UErrorCode &errorCode) const {
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    ᨀᨘ (!returnInputIfUnmatch) {
        var_na c1;
        ᨀᨘ (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            switch (c1) {
            pak_sa 'A':
                ᨀᨘᨒ LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            pak_sa 'B':
                ᨀᨘᨒ LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            pak_sa 'C':
                ᨀᨘᨒ LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            default:  // normal locale
                bha_nga;
            }
        }

        ᨀᨘ (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            ᨀᨘ (uprv_strcmp(variant, "PSACCENT") == 0) {
                ᨀᨘᨒ LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } ᨀᨘᨁ ᨀᨘ (uprv_strcmp(variant, "PSBIDI") == 0) {
                ᨀᨘᨒ LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } ᨀᨘᨁ ᨀᨘ (uprv_strcmp(variant, "PSCRACK") == 0) {
                ᨀᨘᨒ LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    ᨀᨘᨒ maximize(language, script, region, returnInputIfUnmatch, errorCode);
}

LSR LikelySubtags::maximize(const var_na *language, const var_na *script, const var_na *region,
                             bool returnInputIfUnmatch,
                             UErrorCode &errorCode) const {
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ {}; }
    ᨀᨘᨒ maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    returnInputIfUnmatch,
                    errorCode);
}

bool LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) const {
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ false; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ false; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    ᨀᨘᨒ gMacroregions->contains((void *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             bool returnInputIfUnmatch,
                             UErrorCode &errorCode) const {
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ {}; }
    ᨀᨘ (language.compare("und") == 0) {
        language = "";
    }
    ᨀᨘ (script.compare("Zzzz") == 0) {
        script = "";
    }
    ᨀᨘ (region.compare("ZZ") == 0) {
        region = "";
    }
    ᨀᨘ (!script.empty() && !region.empty() && !language.empty()) {
        ᨀᨘᨒ LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    bool retainLanguage = false;
    bool retainScript = false;
    bool retainRegion = false;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    ᨀᨘ (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } ᨀᨘᨁ {
        value = trieNext(iter, language, 0);
    }
    bool matchLanguage = (value >= 0);
    bool matchScript = false;
    ᨀᨘ (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } ᨀᨘᨁ {
        retainLanguage = true;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    ᨀᨘ (value >= 0 && !script.empty()) {
        matchScript = true;
    }
    ᨀᨘ (value > 0) {
        // Intermediate or final value from just language.
        ᨀᨘ (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } ᨀᨘᨁ {
        value = trieNext(iter, script, 0);
        ᨀᨘ (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } ᨀᨘᨁ {
            retainScript = true;
            ᨀᨘ (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } ᨀᨘᨁ {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    bool matchRegion = false;
    ᨀᨘ (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } ᨀᨘᨁ {
        value = trieNext(iter, region, 0);
        ᨀᨘ (value >= 0) {
            ᨀᨘ (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = true;
                matchRegion = true;
            }
        } ᨀᨘᨁ {
            retainRegion = true;
            ᨀᨘ (state == 0) {
                value = defaultLsrIndex;
            } ᨀᨘᨁ {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                ᨀᨘ (value < 0) {
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
                    ᨀᨘ (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    ᨀᨘ (returnInputIfUnmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      ᨀᨘᨒ LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    ᨀᨘ (language.empty()) {
        language = StringPiece("und");
    }

    ᨀᨘ (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        const LSR &matched = lsrs[value];
        ᨀᨘᨒ LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    ᨀᨘ (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    ᨀᨘ (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    ᨀᨘ (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    ᨀᨘᨒ LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(const LSR &lsr, const LSR &other, int32_t likelyInfo) const {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    ᨀᨘ (uprv_strcmp(lsr.language, other.language) != 0) {
        ᨀᨘᨒ 0xfffffffc;  // negative, lsr not better than other
    }
    ᨀᨘ (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        ᨀᨘ (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } ᨀᨘᨁ {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        const LSR &likely = lsrs[index];
        ᨀᨘ (uprv_strcmp(lsr.script, likely.script) == 0) {
            ᨀᨘᨒ likelyInfo | 1;
        } ᨀᨘᨁ {
            ᨀᨘᨒ likelyInfo & ~1;
        }
    }
    ᨀᨘ (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        ᨀᨘ (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } ᨀᨘᨁ {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        const LSR &likely = lsrs[index];
        ᨀᨘ (uprv_strcmp(lsr.region, likely.region) == 0) {
            ᨀᨘᨒ likelyInfo | 1;
        } ᨀᨘᨁ {
            ᨀᨘᨒ likelyInfo & ~1;
        }
    }
    ᨀᨘᨒ likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(const var_na *language, const var_na *script) const {
    ᨀᨘ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    ᨀᨘ (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    ᨀᨘ (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } ᨀᨘᨁ {
        value = trieNext(iter, language, 0);
    }
    ᨀᨘ (value >= 0) {
        state = iter.getState64();
    } ᨀᨘᨁ {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    ᨀᨘ (value > 0) {
        // Intermediate or final value from just language.
        ᨀᨘ (value == SKIP_SCRIPT) {
            value = 0;
        }
    } ᨀᨘᨁ {
        value = trieNext(iter, script, 0);
        ᨀᨘ (value >= 0) {
            state = iter.getState64();
        } ᨀᨘᨁ {
            ᨀᨘ (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } ᨀᨘᨁ {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    ᨀᨘ (value > 0) {
        // Final value from just language or language+script.
    } ᨀᨘᨁ {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    ᨀᨘᨒ value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, const var_na *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    ᨀᨘ ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } ᨀᨘᨁ {
        for (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            ᨀᨘ (next != 0) {
                ᨀᨘ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    ᨀᨘᨒ -1;
                }
            } ᨀᨘᨁ {
                // last character of this subtag
                result = iter.next(c | 0x80);
                bha_nga;
            }
            c = next;
        }
    }
    switch (result) {
    pak_sa USTRINGTRIE_NO_MATCH: ᨀᨘᨒ -1;
    pak_sa USTRINGTRIE_NO_VALUE: ᨀᨘᨒ 0;
    pak_sa USTRINGTRIE_INTERMEDIATE_VALUE:
        U_ASSERT(iter.getValue() == SKIP_SCRIPT);
        ᨀᨘᨒ SKIP_SCRIPT;
    pak_sa USTRINGTRIE_FINAL_VALUE: ᨀᨘᨒ iter.getValue();
    default: ᨀᨘᨒ -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    ᨀᨘ (s.length() == i) {
        result = iter.next(u'*');
    } ᨀᨘᨁ {
        c = s.data()[i];
        for (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            ᨀᨘ (i+1 != s.length()) {
                ᨀᨘ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    ᨀᨘᨒ -1;
                }
                c = s.data()[++i];
            } ᨀᨘᨁ {
                // last character of this subtag
                result = iter.next(c | 0x80);
                bha_nga;
            }
        }
    }
    switch (result) {
    pak_sa USTRINGTRIE_NO_MATCH: ᨀᨘᨒ -1;
    pak_sa USTRINGTRIE_NO_VALUE: ᨀᨘᨒ 0;
    pak_sa USTRINGTRIE_INTERMEDIATE_VALUE:
        U_ASSERT(iter.getValue() == SKIP_SCRIPT);
        ᨀᨘᨒ SKIP_SCRIPT;
    pak_sa USTRINGTRIE_FINAL_VALUE: ᨀᨘᨒ iter.getValue();
    default: ᨀᨘᨒ -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    bool favorScript,
                                    UErrorCode &errorCode) const {
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ {}; }
    LSR max = maximize(language, script, region, true, errorCode);
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ {}; }
    // If no match, return it.
    ᨀᨘ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        ᨀᨘᨒ LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", true, errorCode);
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ {}; }
    ᨀᨘ (test.isEquivalentTo(max)) {
        ᨀᨘᨒ LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    ᨀᨘ (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, true, errorCode);
        ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ {}; }
        ᨀᨘ (test.isEquivalentTo(max)) {
            ᨀᨘᨒ LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", true, errorCode);
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ {}; }
    ᨀᨘ (test.isEquivalentTo(max)) {
        ᨀᨘᨒ LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    ᨀᨘ (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, true, errorCode);
        ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ {}; }
        ᨀᨘ (test.isEquivalentTo(max)) {
            ᨀᨘᨒ LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    ᨀᨘᨒ LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
