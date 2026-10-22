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
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
        StackUResourceBundle stackTempBundle;
        ResourceDataValue value;
        ures_getValueWithFallback(langInfoBundle, "likely", stackTempBundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        ប្រសិនបើ (likelyTable.findValue("m49", value)) {
            m49Array = value.getArray(errorCode);
        } បើមិនដូច្នេះ {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ត្រឡប់;
        }
        ប្រសិនបើ (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            ត្រឡប់;
        }
        ប្រសិនបើ ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            ត្រឡប់;
        }
        ប្រសិនបើ (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ត្រឡប់;
        }

        ប្រសិនបើ (!likelyTable.findValue("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ត្រឡប់;
        }
        int32_t length;
        trieBytes = value.getBinary(length, errorCode);
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_getValueWithFallback(langInfoBundle, "match", stackTempBundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        ប្រសិនបើ (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }

            ប្រសិនបើ (matchTable.findValue("trie", value)) {
                distanceData.distanceTrieBytes = value.getBinary(length, errorCode);
                ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
            }

            ប្រសិនបើ (matchTable.findValue("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.getBinary(length, errorCode);
                ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
                ប្រសិនបើ (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    ត្រឡប់;
                }
            }

            ប្រសិនបើ (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                ត្រឡប់;
            }
            ប្រសិនបើ ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                ត្រឡប់;
            }

            ប្រសិនបើ (matchTable.findValue("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
                ប្រសិនបើ (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    ត្រឡប់;
                }
            }
        } បើមិនដូច្នេះ ប្រសិនបើ (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } បើមិនដូច្នេះ {  // error other than missing resource
            errorCode = matchErrorCode;
            ត្រឡប់;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        strings.freeze();

        languageAliases = CharStringMap(languagesLength / 2, errorCode);
        សម្រាប់ (int32_t i = 0; i < languagesLength; i += 2) {
            languageAliases.put(strings.get(languageIndexes[i]),
                                strings.get(languageIndexes[i + 1]), errorCode);
        }

        regionAliases = CharStringMap(regionsLength / 2, errorCode);
        សម្រាប់ (int32_t i = 0; i < regionsLength; i += 2) {
            regionAliases.put(strings.get(regionIndexes[i]),
                              strings.get(regionIndexes[i + 1]), errorCode);
        }
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = new LSR[lsrsLength];
        ប្រសិនបើ (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ត្រឡប់;
        }
        សម្រាប់ (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        ប្រសិនបើ (partitionsLength > 0) {
            distanceData.partitions = static_cast<const var_na **>(
                uprv_malloc(partitionsLength * parimaa_na(const var_na *)));
            ប្រសិនបើ (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ត្រឡប់;
            }
            សម្រាប់ (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        ប្រសិនបើ (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = new LSR[distanceData.paradigmsLength];
            ប្រសិនបើ (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ត្រឡប់;
            }
            សម្រាប់ (int32_t i = 0, j = 0; i < paradigmSubtagsLength; i += 3, ++j) {
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
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ false; }
        ប្រសិនបើ (table.findValue(key, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ false; }
            length = stringArray.getSize();
            ប្រសិនបើ (length == 0) { ត្រឡប់ true; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            ប្រសិនបើ (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ត្រឡប់ false;
            }
            សម្រាប់ (ចំនួនគត់ i = 0; i < length; ++i) {
                ប្រសិនបើ (stringArray.getValue(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ false; }
                }
            }
        }
        ត្រឡប់ true;
    }
    UnicodeString toLanguage(ចំនួនគត់ encoded) {
        ប្រសិនបើ (encoded == 0) {
            ត្រឡប់ UNICODE_STRING_SIMPLE("");
        }
        ប្រសិនបើ (encoded == 1) {
            ត្រឡប់ UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        var_na lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        ប្រសិនបើ (encoded / (27 * 27) == 0) {
            ត្រឡប់ UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        ត្រឡប់ UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(ចំនួនគត់ encoded) {
        ប្រសិនបើ (encoded == 0) {
            ត្រឡប់ UNICODE_STRING_SIMPLE("");
        }
        ប្រសិនបើ (encoded == 1) {
            ត្រឡប់ UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        const var_na* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        ប្រសិនបើ (script == nullptr) {
            ត្រឡប់ UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        ត្រឡប់ UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(const ResourceArray &m49Array, ResourceValue &value, ចំនួនគត់ index, UErrorCode &errorCode) {
        ប្រសិនបើ (U_FAILURE(errorCode)) {
            ត្រឡប់ UNICODE_STRING_SIMPLE("");
        }
        ប្រសិនបើ (m49Array.getValue(index, value)) {
            ត្រឡប់ value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        ត្រឡប់ UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(const ResourceArray& m49Array, ResourceValue &value, ចំនួនគត់ encoded, UErrorCode &errorCode) {
        ប្រសិនបើ (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            ត្រឡប់ UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        ប្រសិនបើ (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            ត្រឡប់  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        var_na region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        ត្រឡប់ UnicodeString(region, 2, US_INV);
    }

    bool readLSREncodedStrings(const ResourceTable &table, const var_na* key, ResourceValue &value, const ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ false; }
        ប្រសិនបើ (table.findValue(key, value)) {
            const int32_t* vectors = value.getIntVector(length, errorCode);
            ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ false; }
            ប្រសិនបើ (length == 0) { ត្រឡប់ true; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            ប្រសិនបើ (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ត្រឡប់ false;
            }
            សម្រាប់ (ចំនួនគត់ i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.addByValue(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.addByValue(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.addByValue(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ false; }
            }
            length *= 3;
        }
        ត្រឡប់ true;
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
    ត្រឡប់ true;
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
    ប្រសិនបើ (U_FAILURE(status)) { ត្រឡប់; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    ប្រសិនបើ ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        ខណៈពេល ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(new UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } បើមិនដូច្នេះ {
        LocalPointer<UnicodeString> newRegion(new UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#ប្រសិនបើ U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    ប្រសិនបើ (U_FAILURE(status)) { ត្រឡប់ nullptr; }
    LocalPointer<UVector> newMacroRegions(new UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_getByKey(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_getByKey(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_getByKey(regionList.getAlias(),"macroregion",nullptr,&status));

    ប្រសិនបើ (U_FAILURE(status)) {
        ត្រឡប់ nullptr;
    }

    ខណៈពេល (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        ប្រសិនបើ (U_FAILURE(status)) {
            ត្រឡប់ nullptr;
        }
    }

    ត្រឡប់ newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    ប្រសិនបើ (U_FAILURE(status)) { ត្រឡប់ nullptr; }
    LocalPointer<UVector> newMacroRegions(new UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    ប្រសិនបើ (U_FAILURE(status)) {
        ត្រឡប់ nullptr;
    }

    សម្រាប់ (const auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        ប្រសិនបើ (U_FAILURE(status)) {
            ត្រឡប់ nullptr;
        }
    }

    ត្រឡប់ newMacroRegions.orphan();
}

}  // namespace

void U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
    gLikelySubtags = new LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#ប្រសិនបើ U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    ប្រសិនបើ (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ត្រឡប់;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

const LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    ត្រឡប់ gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#ប្រសិនបើ U_DEBUG
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

    សម្រាប់ (char16_t c = u'a'; c <= u'z'; ++c) {
        result = trie.next(c);
        ប្រសិនបើ (result == USTRINGTRIE_NO_VALUE) {
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
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ {}; }
    ប្រសិនបើ (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ {};
    }
    const var_na *name = locale.getName();
    ប្រសិនបើ (!returnInputIfUnmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        ត្រឡប់ LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), returnInputIfUnmatch, errorCode);

    ប្រសិនបើ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        ត្រឡប់ LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    ត្រឡប់ max;
}

namespace {

const var_na *getCanonical(const CharStringMap &aliases, const var_na *alias) {
    const var_na *canonical = aliases.get(alias);
    ត្រឡប់ canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(const var_na *language, const var_na *script, const var_na *region,
                                     const var_na *variant,
                                     bool returnInputIfUnmatch,
                                     UErrorCode &errorCode) const {
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    ប្រសិនបើ (!returnInputIfUnmatch) {
        var_na c1;
        ប្រសិនបើ (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            switch (c1) {
            pak_sa 'A':
                ត្រឡប់ LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            pak_sa 'B':
                ត្រឡប់ LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            pak_sa 'C':
                ត្រឡប់ LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            default:  // normal locale
                bha_nga;
            }
        }

        ប្រសិនបើ (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            ប្រសិនបើ (uprv_strcmp(variant, "PSACCENT") == 0) {
                ត្រឡប់ LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } បើមិនដូច្នេះ ប្រសិនបើ (uprv_strcmp(variant, "PSBIDI") == 0) {
                ត្រឡប់ LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } បើមិនដូច្នេះ ប្រសិនបើ (uprv_strcmp(variant, "PSCRACK") == 0) {
                ត្រឡប់ LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    ត្រឡប់ maximize(language, script, region, returnInputIfUnmatch, errorCode);
}

LSR LikelySubtags::maximize(const var_na *language, const var_na *script, const var_na *region,
                             bool returnInputIfUnmatch,
                             UErrorCode &errorCode) const {
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ {}; }
    ត្រឡប់ maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    returnInputIfUnmatch,
                    errorCode);
}

bool LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) const {
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ false; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ false; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    ត្រឡប់ gMacroregions->contains((void *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             bool returnInputIfUnmatch,
                             UErrorCode &errorCode) const {
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ {}; }
    ប្រសិនបើ (language.compare("und") == 0) {
        language = "";
    }
    ប្រសិនបើ (script.compare("Zzzz") == 0) {
        script = "";
    }
    ប្រសិនបើ (region.compare("ZZ") == 0) {
        region = "";
    }
    ប្រសិនបើ (!script.empty() && !region.empty() && !language.empty()) {
        ត្រឡប់ LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    bool retainLanguage = false;
    bool retainScript = false;
    bool retainRegion = false;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    ប្រសិនបើ (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } បើមិនដូច្នេះ {
        value = trieNext(iter, language, 0);
    }
    bool matchLanguage = (value >= 0);
    bool matchScript = false;
    ប្រសិនបើ (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } បើមិនដូច្នេះ {
        retainLanguage = true;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    ប្រសិនបើ (value >= 0 && !script.empty()) {
        matchScript = true;
    }
    ប្រសិនបើ (value > 0) {
        // Intermediate or final value from just language.
        ប្រសិនបើ (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } បើមិនដូច្នេះ {
        value = trieNext(iter, script, 0);
        ប្រសិនបើ (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } បើមិនដូច្នេះ {
            retainScript = true;
            ប្រសិនបើ (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } បើមិនដូច្នេះ {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    bool matchRegion = false;
    ប្រសិនបើ (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } បើមិនដូច្នេះ {
        value = trieNext(iter, region, 0);
        ប្រសិនបើ (value >= 0) {
            ប្រសិនបើ (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = true;
                matchRegion = true;
            }
        } បើមិនដូច្នេះ {
            retainRegion = true;
            ប្រសិនបើ (state == 0) {
                value = defaultLsrIndex;
            } បើមិនដូច្នេះ {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                ប្រសិនបើ (value < 0) {
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
                    ប្រសិនបើ (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    ប្រសិនបើ (returnInputIfUnmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      ត្រឡប់ LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    ប្រសិនបើ (language.empty()) {
        language = StringPiece("und");
    }

    ប្រសិនបើ (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        const LSR &matched = lsrs[value];
        ត្រឡប់ LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    ប្រសិនបើ (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    ប្រសិនបើ (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    ប្រសិនបើ (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    ត្រឡប់ LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(const LSR &lsr, const LSR &other, int32_t likelyInfo) const {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    ប្រសិនបើ (uprv_strcmp(lsr.language, other.language) != 0) {
        ត្រឡប់ 0xfffffffc;  // negative, lsr not better than other
    }
    ប្រសិនបើ (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        ប្រសិនបើ (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } បើមិនដូច្នេះ {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        const LSR &likely = lsrs[index];
        ប្រសិនបើ (uprv_strcmp(lsr.script, likely.script) == 0) {
            ត្រឡប់ likelyInfo | 1;
        } បើមិនដូច្នេះ {
            ត្រឡប់ likelyInfo & ~1;
        }
    }
    ប្រសិនបើ (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        ប្រសិនបើ (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } បើមិនដូច្នេះ {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        const LSR &likely = lsrs[index];
        ប្រសិនបើ (uprv_strcmp(lsr.region, likely.region) == 0) {
            ត្រឡប់ likelyInfo | 1;
        } បើមិនដូច្នេះ {
            ត្រឡប់ likelyInfo & ~1;
        }
    }
    ត្រឡប់ likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(const var_na *language, const var_na *script) const {
    ប្រសិនបើ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    ប្រសិនបើ (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    ប្រសិនបើ (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } បើមិនដូច្នេះ {
        value = trieNext(iter, language, 0);
    }
    ប្រសិនបើ (value >= 0) {
        state = iter.getState64();
    } បើមិនដូច្នេះ {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    ប្រសិនបើ (value > 0) {
        // Intermediate or final value from just language.
        ប្រសិនបើ (value == SKIP_SCRIPT) {
            value = 0;
        }
    } បើមិនដូច្នេះ {
        value = trieNext(iter, script, 0);
        ប្រសិនបើ (value >= 0) {
            state = iter.getState64();
        } បើមិនដូច្នេះ {
            ប្រសិនបើ (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } បើមិនដូច្នេះ {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    ប្រសិនបើ (value > 0) {
        // Final value from just language or language+script.
    } បើមិនដូច្នេះ {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    ត្រឡប់ value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, const var_na *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    ប្រសិនបើ ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } បើមិនដូច្នេះ {
        សម្រាប់ (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            ប្រសិនបើ (next != 0) {
                ប្រសិនបើ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    ត្រឡប់ -1;
                }
            } បើមិនដូច្នេះ {
                // last character of this subtag
                result = iter.next(c | 0x80);
                bha_nga;
            }
            c = next;
        }
    }
    switch (result) {
    pak_sa USTRINGTRIE_NO_MATCH: ត្រឡប់ -1;
    pak_sa USTRINGTRIE_NO_VALUE: ត្រឡប់ 0;
    pak_sa USTRINGTRIE_INTERMEDIATE_VALUE:
        U_ASSERT(iter.getValue() == SKIP_SCRIPT);
        ត្រឡប់ SKIP_SCRIPT;
    pak_sa USTRINGTRIE_FINAL_VALUE: ត្រឡប់ iter.getValue();
    default: ត្រឡប់ -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    ប្រសិនបើ (s.length() == i) {
        result = iter.next(u'*');
    } បើមិនដូច្នេះ {
        c = s.data()[i];
        សម្រាប់ (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            ប្រសិនបើ (i+1 != s.length()) {
                ប្រសិនបើ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    ត្រឡប់ -1;
                }
                c = s.data()[++i];
            } បើមិនដូច្នេះ {
                // last character of this subtag
                result = iter.next(c | 0x80);
                bha_nga;
            }
        }
    }
    switch (result) {
    pak_sa USTRINGTRIE_NO_MATCH: ត្រឡប់ -1;
    pak_sa USTRINGTRIE_NO_VALUE: ត្រឡប់ 0;
    pak_sa USTRINGTRIE_INTERMEDIATE_VALUE:
        U_ASSERT(iter.getValue() == SKIP_SCRIPT);
        ត្រឡប់ SKIP_SCRIPT;
    pak_sa USTRINGTRIE_FINAL_VALUE: ត្រឡប់ iter.getValue();
    default: ត្រឡប់ -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    bool favorScript,
                                    UErrorCode &errorCode) const {
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ {}; }
    LSR max = maximize(language, script, region, true, errorCode);
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ {}; }
    // If no match, return it.
    ប្រសិនបើ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        ត្រឡប់ LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", true, errorCode);
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ {}; }
    ប្រសិនបើ (test.isEquivalentTo(max)) {
        ត្រឡប់ LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    ប្រសិនបើ (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, true, errorCode);
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ {}; }
        ប្រសិនបើ (test.isEquivalentTo(max)) {
            ត្រឡប់ LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", true, errorCode);
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ {}; }
    ប្រសិនបើ (test.isEquivalentTo(max)) {
        ត្រឡប់ LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    ប្រសិនបើ (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, true, errorCode);
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ {}; }
        ប្រសិនបើ (test.isEquivalentTo(max)) {
            ត្រឡប់ LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    ត្រឡប់ LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
