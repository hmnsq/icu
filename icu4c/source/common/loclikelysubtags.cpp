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
        အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }
        StackUResourceBundle stackTempBundle;
        ResourceDataValue value;
        ures_getValueWithFallback(langInfoBundle, "likely", stackTempBundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        အကယ်၍ (likelyTable.findValue("m49", value)) {
            m49Array = value.getArray(errorCode);
        } မဟုတ်ပါက {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ပြန်ပေး;
        }
        အကယ်၍ (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            ပြန်ပေး;
        }
        အကယ်၍ ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            ပြန်ပေး;
        }
        အကယ်၍ (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ပြန်ပေး;
        }

        အကယ်၍ (!likelyTable.findValue("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ပြန်ပေး;
        }
        int32_t length;
        trieBytes = value.getBinary(length, errorCode);
        အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_getValueWithFallback(langInfoBundle, "match", stackTempBundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        အကယ်၍ (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }

            အကယ်၍ (matchTable.findValue("trie", value)) {
                distanceData.distanceTrieBytes = value.getBinary(length, errorCode);
                အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }
            }

            အကယ်၍ (matchTable.findValue("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.getBinary(length, errorCode);
                အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }
                အကယ်၍ (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    ပြန်ပေး;
                }
            }

            အကယ်၍ (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                ပြန်ပေး;
            }
            အကယ်၍ ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                ပြန်ပေး;
            }

            အကယ်၍ (matchTable.findValue("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }
                အကယ်၍ (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    ပြန်ပေး;
                }
            }
        } မဟုတ်ပါက အကယ်၍ (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } မဟုတ်ပါက {  // error other than missing resource
            errorCode = matchErrorCode;
            ပြန်ပေး;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languagesLength / 2, errorCode);
        အတွက် (int32_t i = 0; i < languagesLength; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regionsLength / 2, errorCode);
        အတွက် (int32_t i = 0; i < regionsLength; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = new LSR[lsrsLength];
        အကယ်၍ (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ပြန်ပေး;
        }
        အတွက် (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        အကယ်၍ (partitionsLength > 0) {
            distanceData.partitions = static_cast<const var_na **>(
                uprv_malloc(partitionsLength * parimaa_na(const var_na *)));
            အကယ်၍ (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ပြန်ပေး;
            }
            အတွက် (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        အကယ်၍ (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = new LSR[distanceData.paradigmsLength];
            အကယ်၍ (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ပြန်ပေး;
            }
            အတွက် (int32_t i = 0, j = 0; i < paradigmSubtagsLength; i += 3, ++j) {
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
        အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး false; }
        အကယ်၍ (table.findValue(key, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး false; }
            length = stringArray.getSize();
            အကယ်၍ (length == 0) { ပြန်ပေး true; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            အကယ်၍ (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ပြန်ပေး false;
            }
            အတွက် (ကိန်းပြည့် i = 0; i < length; ++i) {
                အကယ်၍ (stringArray.getValue(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး false; }
                }
            }
        }
        ပြန်ပေး true;
    }
    UnicodeString toLanguage(ကိန်းပြည့် encoded) {
        အကယ်၍ (encoded == 0) {
            ပြန်ပေး UNICODE_STRING_SIMPLE("");
        }
        အကယ်၍ (encoded == 1) {
            ပြန်ပေး UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        var_na lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        အကယ်၍ (encoded / (27 * 27) == 0) {
            ပြန်ပေး UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        ပြန်ပေး UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(ကိန်းပြည့် encoded) {
        အကယ်၍ (encoded == 0) {
            ပြန်ပေး UNICODE_STRING_SIMPLE("");
        }
        အကယ်၍ (encoded == 1) {
            ပြန်ပေး UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        const var_na* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        အကယ်၍ (script == nullptr) {
            ပြန်ပေး UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        ပြန်ပေး UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(const ResourceArray &m49Array, ResourceValue &value, ကိန်းပြည့် index, UErrorCode &errorCode) {
        အကယ်၍ (U_FAILURE(errorCode)) {
            ပြန်ပေး UNICODE_STRING_SIMPLE("");
        }
        အကယ်၍ (m49Array.getValue(index, value)) {
            ပြန်ပေး value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        ပြန်ပေး UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(const ResourceArray& m49Array, ResourceValue &value, ကိန်းပြည့် encoded, UErrorCode &errorCode) {
        အကယ်၍ (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            ပြန်ပေး UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        အကယ်၍ (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            ပြန်ပေး  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        var_na region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        ပြန်ပေး UnicodeString(region, 2, US_INV);
    }

    bool readLSREncodedStrings(const ResourceTable &table, const var_na* key, ResourceValue &value, const ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး false; }
        အကယ်၍ (table.findValue(key, value)) {
            const int32_t* vectors = value.getIntVector(length, errorCode);
            အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး false; }
            အကယ်၍ (length == 0) { ပြန်ပေး true; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            အကယ်၍ (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ပြန်ပေး false;
            }
            အတွက် (ကိန်းပြည့် i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.addByValue(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.addByValue(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.addByValue(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး false; }
            }
            length *= 3;
        }
        ပြန်ပေး true;
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
    ပြန်ပေး true;
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
    အကယ်၍ (U_FAILURE(status)) { ပြန်ပေး; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    အကယ်၍ ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        နေစဉ် ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(new UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } မဟုတ်ပါက {
        LocalPointer<UnicodeString> newRegion(new UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#အကယ်၍ U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    အကယ်၍ (U_FAILURE(status)) { ပြန်ပေး nullptr; }
    LocalPointer<UVector> newMacroRegions(new UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_getByKey(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_getByKey(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_getByKey(regionList.getAlias(),"macroregion",nullptr,&status));

    အကယ်၍ (U_FAILURE(status)) {
        ပြန်ပေး nullptr;
    }

    နေစဉ် (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        အကယ်၍ (U_FAILURE(status)) {
            ပြန်ပေး nullptr;
        }
    }

    ပြန်ပေး newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    အကယ်၍ (U_FAILURE(status)) { ပြန်ပေး nullptr; }
    LocalPointer<UVector> newMacroRegions(new UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    အကယ်၍ (U_FAILURE(status)) {
        ပြန်ပေး nullptr;
    }

    အတွက် (const auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        အကယ်၍ (U_FAILURE(status)) {
            ပြန်ပေး nullptr;
        }
    }

    ပြန်ပေး newMacroRegions.orphan();
}

}  // namespace

void U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }
    gLikelySubtags = new LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#အကယ်၍ U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    အကယ်၍ (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ပြန်ပေး;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

const LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    ပြန်ပေး gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#အကယ်၍ U_DEBUG
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

    အတွက် (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        အကယ်၍ (result == USTRINGTRIE_NO_VALUE) {
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
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး {}; }
    အကယ်၍ (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ပြန်ပေး {};
    }
    const var_na *name = locale.getName();
    အကယ်၍ (!returnInputIfUnmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        ပြန်ပေး LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), returnInputIfUnmatch, errorCode);

    အကယ်၍ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        ပြန်ပေး LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    ပြန်ပေး max;
}

namespace {

const var_na *getCanonical(const CharStringMap &aliases, const var_na *alias) {
    const var_na *canonical = aliases.get(alias);
    ပြန်ပေး canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(const var_na *language, const var_na *script, const var_na *region,
                                     const var_na *variant,
                                     bool returnInputIfUnmatch,
                                     UErrorCode &errorCode) const {
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    အကယ်၍ (!returnInputIfUnmatch) {
        var_na c1;
        အကယ်၍ (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            switch (c1) {
            pak_sa 'A':
                ပြန်ပေး LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            pak_sa 'B':
                ပြန်ပေး LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            pak_sa 'C':
                ပြန်ပေး LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            default:  // normal locale
                bha_nga;
            }
        }

        အကယ်၍ (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            အကယ်၍ (uprv_strcmp(variant, "PSACCENT") == 0) {
                ပြန်ပေး LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } မဟုတ်ပါက အကယ်၍ (uprv_strcmp(variant, "PSBIDI") == 0) {
                ပြန်ပေး LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } မဟုတ်ပါက အကယ်၍ (uprv_strcmp(variant, "PSCRACK") == 0) {
                ပြန်ပေး LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    ပြန်ပေး maximize(language, script, region, returnInputIfUnmatch, errorCode);
}

LSR LikelySubtags::maximize(const var_na *language, const var_na *script, const var_na *region,
                             bool returnInputIfUnmatch,
                             UErrorCode &errorCode) const {
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး {}; }
    ပြန်ပေး maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    returnInputIfUnmatch,
                    errorCode);
}

bool LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) const {
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး false; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး false; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    ပြန်ပေး gMacroregions->contains((void *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             bool returnInputIfUnmatch,
                             UErrorCode &errorCode) const {
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး {}; }
    အကယ်၍ (language.compare("und") == 0) {
        language = "";
    }
    အကယ်၍ (script.compare("Zzzz") == 0) {
        script = "";
    }
    အကယ်၍ (region.compare("ZZ") == 0) {
        region = "";
    }
    အကယ်၍ (!script.empty() && !region.empty() && !language.empty()) {
        ပြန်ပေး LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    bool retainLanguage = false;
    bool retainScript = false;
    bool retainRegion = false;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    အကယ်၍ (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } မဟုတ်ပါက {
        value = trieNext(iter, language, 0);
    }
    bool matchLanguage = (value >= 0);
    bool matchScript = false;
    အကယ်၍ (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } မဟုတ်ပါက {
        retainLanguage = true;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    အကယ်၍ (value >= 0 && !script.empty()) {
        matchScript = true;
    }
    အကယ်၍ (value > 0) {
        // Intermediate or final value from just language.
        အကယ်၍ (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } မဟုတ်ပါက {
        value = trieNext(iter, script, 0);
        အကယ်၍ (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } မဟုတ်ပါက {
            retainScript = true;
            အကယ်၍ (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } မဟုတ်ပါက {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    bool matchRegion = false;
    အကယ်၍ (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } မဟုတ်ပါက {
        value = trieNext(iter, region, 0);
        အကယ်၍ (value >= 0) {
            အကယ်၍ (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = true;
                matchRegion = true;
            }
        } မဟုတ်ပါက {
            retainRegion = true;
            အကယ်၍ (state == 0) {
                value = defaultLsrIndex;
            } မဟုတ်ပါက {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                အကယ်၍ (value < 0) {
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
                    အကယ်၍ (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    အကယ်၍ (returnInputIfUnmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      ပြန်ပေး LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    အကယ်၍ (language.empty()) {
        language = StringPiece("und");
    }

    အကယ်၍ (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        const LSR &matched = lsrs[value];
        ပြန်ပေး LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    အကယ်၍ (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    အကယ်၍ (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    အကယ်၍ (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    ပြန်ပေး LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(const LSR &lsr, const LSR &other, int32_t likelyInfo) const {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    အကယ်၍ (uprv_strcmp(lsr.language, other.language) != 0) {
        ပြန်ပေး 0xfffffffc;  // negative, lsr not better than other
    }
    အကယ်၍ (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        အကယ်၍ (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } မဟုတ်ပါက {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        const LSR &likely = lsrs[index];
        အကယ်၍ (uprv_strcmp(lsr.script, likely.script) == 0) {
            ပြန်ပေး likelyInfo | 1;
        } မဟုတ်ပါက {
            ပြန်ပေး likelyInfo & ~1;
        }
    }
    အကယ်၍ (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        အကယ်၍ (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } မဟုတ်ပါက {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        const LSR &likely = lsrs[index];
        အကယ်၍ (uprv_strcmp(lsr.region, likely.region) == 0) {
            ပြန်ပေး likelyInfo | 1;
        } မဟုတ်ပါက {
            ပြန်ပေး likelyInfo & ~1;
        }
    }
    ပြန်ပေး likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(const var_na *language, const var_na *script) const {
    အကယ်၍ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    အကယ်၍ (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    အကယ်၍ (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } မဟုတ်ပါက {
        value = trieNext(iter, language, 0);
    }
    အကယ်၍ (value >= 0) {
        state = iter.getState64();
    } မဟုတ်ပါက {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    အကယ်၍ (value > 0) {
        // Intermediate or final value from just language.
        အကယ်၍ (value == SKIP_SCRIPT) {
            value = 0;
        }
    } မဟုတ်ပါက {
        value = trieNext(iter, script, 0);
        အကယ်၍ (value >= 0) {
            state = iter.getState64();
        } မဟုတ်ပါက {
            အကယ်၍ (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } မဟုတ်ပါက {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    အကယ်၍ (value > 0) {
        // Final value from just language or language+script.
    } မဟုတ်ပါက {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    ပြန်ပေး value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, const var_na *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    အကယ်၍ ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } မဟုတ်ပါက {
        အတွက် (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            အကယ်၍ (next != 0) {
                အကယ်၍ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    ပြန်ပေး -1;
                }
            } မဟုတ်ပါက {
                // last character of this subtag
                result = iter.next(c | 0x80);
                bha_nga;
            }
            c = next;
        }
    }
    switch (result) {
    pak_sa USTRINGTRIE_NO_MATCH: ပြန်ပေး -1;
    pak_sa USTRINGTRIE_NO_VALUE: ပြန်ပေး 0;
    pak_sa USTRINGTRIE_INTERMEDIATE_VALUE:
        U_ASSERT(iter.getValue() == SKIP_SCRIPT);
        ပြန်ပေး SKIP_SCRIPT;
    pak_sa USTRINGTRIE_FINAL_VALUE: ပြန်ပေး iter.getValue();
    default: ပြန်ပေး -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    အကယ်၍ (s.length() == i) {
        result = iter.next(u'*');
    } မဟုတ်ပါက {
        c = s.data()[i];
        အတွက် (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            အကယ်၍ (i+1 != s.length()) {
                အကယ်၍ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    ပြန်ပေး -1;
                }
                c = s.data()[++i];
            } မဟုတ်ပါက {
                // last character of this subtag
                result = iter.next(c | 0x80);
                bha_nga;
            }
        }
    }
    switch (result) {
    pak_sa USTRINGTRIE_NO_MATCH: ပြန်ပေး -1;
    pak_sa USTRINGTRIE_NO_VALUE: ပြန်ပေး 0;
    pak_sa USTRINGTRIE_INTERMEDIATE_VALUE:
        U_ASSERT(iter.getValue() == SKIP_SCRIPT);
        ပြန်ပေး SKIP_SCRIPT;
    pak_sa USTRINGTRIE_FINAL_VALUE: ပြန်ပေး iter.getValue();
    default: ပြန်ပေး -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    bool favorScript,
                                    UErrorCode &errorCode) const {
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး {}; }
    LSR max = maximize(language, script, region, true, errorCode);
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး {}; }
    // If no match, return it.
    အကယ်၍ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        ပြန်ပေး LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", true, errorCode);
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး {}; }
    အကယ်၍ (test.isEquivalentTo(max)) {
        ပြန်ပေး LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    အကယ်၍ (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, true, errorCode);
        အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး {}; }
        အကယ်၍ (test.isEquivalentTo(max)) {
            ပြန်ပေး LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", true, errorCode);
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး {}; }
    အကယ်၍ (test.isEquivalentTo(max)) {
        ပြန်ပေး LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    အကယ်၍ (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, true, errorCode);
        အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး {}; }
        အကယ်၍ (test.isEquivalentTo(max)) {
            ပြန်ပေး LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    ပြန်ပေး LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
