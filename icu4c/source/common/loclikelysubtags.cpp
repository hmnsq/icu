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

constexpr char PSEUDO_ACCENTS_PREFIX = '\'';  // -XA, -PSACCENT
constexpr char PSEUDO_BIDI_PREFIX = '+';  // -XB, -PSBIDI
constexpr char PSEUDO_CRACKED_PREFIX = ',';  // -XC, -PSCRACK

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

struct LikelySubtagsData {
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
        만약 (U_FAILURE(errorCode)) { 반환; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        만약 (U_FAILURE(errorCode)) { 반환; }
        StackUResourceBundle stackTempBundle;
        ResourceDataValue value;
        ures_getValueWithFallback(langInfoBundle, "likely", stackTempBundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        만약 (U_FAILURE(errorCode)) { 반환; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        만약 (likelyTable.findValue("m49", value)) {
            m49Array = value.getArray(errorCode);
        } 아니면 {
            errorCode = U_MISSING_RESOURCE_ERROR;
            반환;
        }
        만약 (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            반환;
        }
        만약 ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            반환;
        }
        만약 (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            반환;
        }

        만약 (!likelyTable.findValue("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            반환;
        }
        int32_t length;
        trieBytes = value.getBinary(length, errorCode);
        만약 (U_FAILURE(errorCode)) { 반환; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_getValueWithFallback(langInfoBundle, "match", stackTempBundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        만약 (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            만약 (U_FAILURE(errorCode)) { 반환; }

            만약 (matchTable.findValue("trie", value)) {
                distanceData.distanceTrieBytes = value.getBinary(length, errorCode);
                만약 (U_FAILURE(errorCode)) { 반환; }
            }

            만약 (matchTable.findValue("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.getBinary(length, errorCode);
                만약 (U_FAILURE(errorCode)) { 반환; }
                만약 (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    반환;
                }
            }

            만약 (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                반환;
            }
            만약 ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                반환;
            }

            만약 (matchTable.findValue("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                만약 (U_FAILURE(errorCode)) { 반환; }
                만약 (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    반환;
                }
            }
        } 아니면 만약 (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } 아니면 {  // error other than missing resource
            errorCode = matchErrorCode;
            반환;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languagesLength / 2, errorCode);
        위해 (int32_t i = 0; i < languagesLength; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regionsLength / 2, errorCode);
        위해 (int32_t i = 0; i < regionsLength; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        만약 (U_FAILURE(errorCode)) { 반환; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = new LSR[lsrsLength];
        만약 (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            반환;
        }
        위해 (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        만약 (partitionsLength > 0) {
            distanceData.partitions = static_cast<const char **>(
                uprv_malloc(partitionsLength * sizeof(const char *)));
            만약 (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                반환;
            }
            위해 (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        만약 (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = new LSR[distanceData.paradigmsLength];
            만약 (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                반환;
            }
            위해 (int32_t i = 0, j = 0; i < paradigmSubtagsLength; i += 3, ++j) {
                paradigms[j] = LSR(strings.get(paradigmSubtagIndexes[i]),
                                   strings.get(paradigmSubtagIndexes[i + 1]),
                                   strings.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            distanceData.paradigms = paradigms;
        }
    }

private:
    bool readStrings(const ResourceTable &table, const char *key, ResourceValue &value,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        만약 (U_FAILURE(errorCode)) { 반환 false; }
        만약 (table.findValue(key, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            만약 (U_FAILURE(errorCode)) { 반환 false; }
            length = stringArray.getSize();
            만약 (length == 0) { 반환 true; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            만약 (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                반환 false;
            }
            위해 (정수 i = 0; i < length; ++i) {
                만약 (stringArray.getValue(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    만약 (U_FAILURE(errorCode)) { 반환 false; }
                }
            }
        }
        반환 true;
    }
    UnicodeString toLanguage(정수 encoded) {
        만약 (encoded == 0) {
            반환 UNICODE_STRING_SIMPLE("");
        }
        만약 (encoded == 1) {
            반환 UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        char lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        만약 (encoded / (27 * 27) == 0) {
            반환 UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        반환 UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(정수 encoded) {
        만약 (encoded == 0) {
            반환 UNICODE_STRING_SIMPLE("");
        }
        만약 (encoded == 1) {
            반환 UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        const char* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        만약 (script == nullptr) {
            반환 UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        반환 UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(const ResourceArray &m49Array, ResourceValue &value, 정수 index, UErrorCode &errorCode) {
        만약 (U_FAILURE(errorCode)) {
            반환 UNICODE_STRING_SIMPLE("");
        }
        만약 (m49Array.getValue(index, value)) {
            반환 value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        반환 UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(const ResourceArray& m49Array, ResourceValue &value, 정수 encoded, UErrorCode &errorCode) {
        만약 (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            반환 UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        만약 (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            반환  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        char region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        반환 UnicodeString(region, 2, US_INV);
    }

    bool readLSREncodedStrings(const ResourceTable &table, const char* key, ResourceValue &value, const ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        만약 (U_FAILURE(errorCode)) { 반환 false; }
        만약 (table.findValue(key, value)) {
            const int32_t* vectors = value.getIntVector(length, errorCode);
            만약 (U_FAILURE(errorCode)) { 반환 false; }
            만약 (length == 0) { 반환 true; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            만약 (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                반환 false;
            }
            위해 (정수 i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.addByValue(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.addByValue(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.addByValue(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                만약 (U_FAILURE(errorCode)) { 반환 false; }
            }
            length *= 3;
        }
        반환 true;
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
    반환 true;
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
    만약 (U_FAILURE(status)) { 반환; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    만약 ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        동안 ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(new UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } 아니면 {
        LocalPointer<UnicodeString> newRegion(new UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#만약 U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    만약 (U_FAILURE(status)) { 반환 nullptr; }
    LocalPointer<UVector> newMacroRegions(new UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_getByKey(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_getByKey(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_getByKey(regionList.getAlias(),"macroregion",nullptr,&status));

    만약 (U_FAILURE(status)) {
        반환 nullptr;
    }

    동안 (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        만약 (U_FAILURE(status)) {
            반환 nullptr;
        }
    }

    반환 newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    만약 (U_FAILURE(status)) { 반환 nullptr; }
    LocalPointer<UVector> newMacroRegions(new UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    만약 (U_FAILURE(status)) {
        반환 nullptr;
    }

    위해 (const auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        만약 (U_FAILURE(status)) {
            반환 nullptr;
        }
    }

    반환 newMacroRegions.orphan();
}

}  // namespace

void U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    만약 (U_FAILURE(errorCode)) { 반환; }
    gLikelySubtags = new LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#만약 U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    만약 (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        반환;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

const LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    만약 (U_FAILURE(errorCode)) { 반환 nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    반환 gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#만약 U_DEBUG
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

    위해 (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        만약 (result == USTRINGTRIE_NO_VALUE) {
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
    만약 (U_FAILURE(errorCode)) { 반환 {}; }
    만약 (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        반환 {};
    }
    const char *name = locale.getName();
    만약 (!returnInputIfUnmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        반환 LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), returnInputIfUnmatch, errorCode);

    만약 (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        반환 LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    반환 max;
}

namespace {

const char *getCanonical(const CharStringMap &aliases, const char *alias) {
    const char *canonical = aliases.get(alias);
    반환 canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(const char *language, const char *script, const char *region,
                                     const char *variant,
                                     bool returnInputIfUnmatch,
                                     UErrorCode &errorCode) const {
    만약 (U_FAILURE(errorCode)) { 반환 {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    만약 (!returnInputIfUnmatch) {
        char c1;
        만약 (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            switch (c1) {
            case 'A':
                반환 LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            case 'B':
                반환 LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            case 'C':
                반환 LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            default:  // normal locale
                break;
            }
        }

        만약 (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            만약 (uprv_strcmp(variant, "PSACCENT") == 0) {
                반환 LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } 아니면 만약 (uprv_strcmp(variant, "PSBIDI") == 0) {
                반환 LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } 아니면 만약 (uprv_strcmp(variant, "PSCRACK") == 0) {
                반환 LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    반환 maximize(language, script, region, returnInputIfUnmatch, errorCode);
}

LSR LikelySubtags::maximize(const char *language, const char *script, const char *region,
                             bool returnInputIfUnmatch,
                             UErrorCode &errorCode) const {
    만약 (U_FAILURE(errorCode)) { 반환 {}; }
    반환 maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    returnInputIfUnmatch,
                    errorCode);
}

bool LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) const {
    만약 (U_FAILURE(errorCode)) { 반환 false; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    만약 (U_FAILURE(errorCode)) { 반환 false; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    반환 gMacroregions->contains((void *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             bool returnInputIfUnmatch,
                             UErrorCode &errorCode) const {
    만약 (U_FAILURE(errorCode)) { 반환 {}; }
    만약 (language.compare("und") == 0) {
        language = "";
    }
    만약 (script.compare("Zzzz") == 0) {
        script = "";
    }
    만약 (region.compare("ZZ") == 0) {
        region = "";
    }
    만약 (!script.empty() && !region.empty() && !language.empty()) {
        반환 LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    bool retainLanguage = false;
    bool retainScript = false;
    bool retainRegion = false;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    만약 (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } 아니면 {
        value = trieNext(iter, language, 0);
    }
    bool matchLanguage = (value >= 0);
    bool matchScript = false;
    만약 (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } 아니면 {
        retainLanguage = true;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    만약 (value >= 0 && !script.empty()) {
        matchScript = true;
    }
    만약 (value > 0) {
        // Intermediate or final value from just language.
        만약 (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } 아니면 {
        value = trieNext(iter, script, 0);
        만약 (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } 아니면 {
            retainScript = true;
            만약 (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } 아니면 {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    bool matchRegion = false;
    만약 (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } 아니면 {
        value = trieNext(iter, region, 0);
        만약 (value >= 0) {
            만약 (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = true;
                matchRegion = true;
            }
        } 아니면 {
            retainRegion = true;
            만약 (state == 0) {
                value = defaultLsrIndex;
            } 아니면 {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                만약 (value < 0) {
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
                    만약 (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    만약 (returnInputIfUnmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      반환 LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    만약 (language.empty()) {
        language = StringPiece("und");
    }

    만약 (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        const LSR &matched = lsrs[value];
        반환 LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    만약 (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    만약 (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    만약 (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    반환 LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(const LSR &lsr, const LSR &other, int32_t likelyInfo) const {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    만약 (uprv_strcmp(lsr.language, other.language) != 0) {
        반환 0xfffffffc;  // negative, lsr not better than other
    }
    만약 (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        만약 (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } 아니면 {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        const LSR &likely = lsrs[index];
        만약 (uprv_strcmp(lsr.script, likely.script) == 0) {
            반환 likelyInfo | 1;
        } 아니면 {
            반환 likelyInfo & ~1;
        }
    }
    만약 (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        만약 (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } 아니면 {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        const LSR &likely = lsrs[index];
        만약 (uprv_strcmp(lsr.region, likely.region) == 0) {
            반환 likelyInfo | 1;
        } 아니면 {
            반환 likelyInfo & ~1;
        }
    }
    반환 likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(const char *language, const char *script) const {
    만약 (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    만약 (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    만약 (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } 아니면 {
        value = trieNext(iter, language, 0);
    }
    만약 (value >= 0) {
        state = iter.getState64();
    } 아니면 {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    만약 (value > 0) {
        // Intermediate or final value from just language.
        만약 (value == SKIP_SCRIPT) {
            value = 0;
        }
    } 아니면 {
        value = trieNext(iter, script, 0);
        만약 (value >= 0) {
            state = iter.getState64();
        } 아니면 {
            만약 (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } 아니면 {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    만약 (value > 0) {
        // Final value from just language or language+script.
    } 아니면 {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    반환 value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, const char *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    만약 ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } 아니면 {
        위해 (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            만약 (next != 0) {
                만약 (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    반환 -1;
                }
            } 아니면 {
                // last character of this subtag
                result = iter.next(c | 0x80);
                break;
            }
            c = next;
        }
    }
    switch (result) {
    case USTRINGTRIE_NO_MATCH: 반환 -1;
    case USTRINGTRIE_NO_VALUE: 반환 0;
    case USTRINGTRIE_INTERMEDIATE_VALUE:
        U_ASSERT(iter.getValue() == SKIP_SCRIPT);
        반환 SKIP_SCRIPT;
    case USTRINGTRIE_FINAL_VALUE: 반환 iter.getValue();
    default: 반환 -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    만약 (s.length() == i) {
        result = iter.next(u'*');
    } 아니면 {
        c = s.data()[i];
        위해 (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            만약 (i+1 != s.length()) {
                만약 (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    반환 -1;
                }
                c = s.data()[++i];
            } 아니면 {
                // last character of this subtag
                result = iter.next(c | 0x80);
                break;
            }
        }
    }
    switch (result) {
    case USTRINGTRIE_NO_MATCH: 반환 -1;
    case USTRINGTRIE_NO_VALUE: 반환 0;
    case USTRINGTRIE_INTERMEDIATE_VALUE:
        U_ASSERT(iter.getValue() == SKIP_SCRIPT);
        반환 SKIP_SCRIPT;
    case USTRINGTRIE_FINAL_VALUE: 반환 iter.getValue();
    default: 반환 -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    bool favorScript,
                                    UErrorCode &errorCode) const {
    만약 (U_FAILURE(errorCode)) { 반환 {}; }
    LSR max = maximize(language, script, region, true, errorCode);
    만약 (U_FAILURE(errorCode)) { 반환 {}; }
    // If no match, return it.
    만약 (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        반환 LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", true, errorCode);
    만약 (U_FAILURE(errorCode)) { 반환 {}; }
    만약 (test.isEquivalentTo(max)) {
        반환 LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    만약 (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, true, errorCode);
        만약 (U_FAILURE(errorCode)) { 반환 {}; }
        만약 (test.isEquivalentTo(max)) {
            반환 LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", true, errorCode);
    만약 (U_FAILURE(errorCode)) { 반환 {}; }
    만약 (test.isEquivalentTo(max)) {
        반환 LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    만약 (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, true, errorCode);
        만약 (U_FAILURE(errorCode)) { 반환 {}; }
        만약 (test.isEquivalentTo(max)) {
            반환 LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    반환 LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
