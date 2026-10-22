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
        ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ; }
        langInfoBundle = ures_openDirect(nullptr, "langInfo", &errorCode);
        ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ; }
        StackUResourceBundle stackTempBundle;
        ResourceDataValue value;
        ures_getValueWithFallback(langInfoBundle, "likely", stackTempBundle.getAlias(),
                                  value, errorCode);
        ResourceTable likelyTable = value.getTable(errorCode);
        ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        LocalMemory<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languagesLength = 0, regionsLength = 0, lsrSubtagsLength = 0;
        ResourceArray m49Array;
        ᬬᬾᬦ᭄ (likelyTable.findValue("m49", value)) {
            m49Array = value.getArray(errorCode);
        } ᬮᬶᬬ {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ᬯᬮᬶ;
        }
        ᬬᬾᬦ᭄ (!readStrings(likelyTable, "languageAliases", value,
                         languageIndexes, languagesLength, errorCode) ||
                !readStrings(likelyTable, "regionAliases", value,
                             regionIndexes, regionsLength, errorCode) ||
                !readLSREncodedStrings(likelyTable, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsrSubtagsLength, errorCode)) {
            ᬯᬮᬶ;
        }
        ᬬᬾᬦ᭄ ((languagesLength & 1) != 0 ||
                (regionsLength & 1) != 0 ||
                (lsrSubtagsLength % 3) != 0) {
            errorCode = U_INVALID_FORMAT_ERROR;
            ᬯᬮᬶ;
        }
        ᬬᬾᬦ᭄ (lsrSubtagsLength == 0) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ᬯᬮᬶ;
        }

        ᬬᬾᬦ᭄ (!likelyTable.findValue("trie", value)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
            ᬯᬮᬶ;
        }
        int32_t length;
        trieBytes = value.getBinary(length, errorCode);
        ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UErrorCode matchErrorCode = U_ZERO_ERROR;
        ures_getValueWithFallback(langInfoBundle, "match", stackTempBundle.getAlias(),
                                  value, matchErrorCode);
        LocalMemory<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitionsLength = 0, paradigmSubtagsLength = 0;
        ᬬᬾᬦ᭄ (U_SUCCESS(matchErrorCode)) {
            ResourceTable matchTable = value.getTable(errorCode);
            ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ; }

            ᬬᬾᬦ᭄ (matchTable.findValue("trie", value)) {
                distanceData.distanceTrieBytes = value.getBinary(length, errorCode);
                ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ; }
            }

            ᬬᬾᬦ᭄ (matchTable.findValue("regionToPartitions", value)) {
                distanceData.regionToPartitions = value.getBinary(length, errorCode);
                ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ; }
                ᬬᬾᬦ᭄ (length < LSR::REGION_INDEX_LIMIT) {
                    errorCode = U_INVALID_FORMAT_ERROR;
                    ᬯᬮᬶ;
                }
            }

            ᬬᬾᬦ᭄ (!readStrings(matchTable, "partitions", value,
                             partitionIndexes, partitionsLength, errorCode) ||
                    !readLSREncodedStrings(matchTable, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigmSubtagsLength, errorCode)) {
                ᬯᬮᬶ;
            }
            ᬬᬾᬦ᭄ ((paradigmSubtagsLength % 3) != 0) {
                errorCode = U_INVALID_FORMAT_ERROR;
                ᬯᬮᬶ;
            }

            ᬬᬾᬦ᭄ (matchTable.findValue("distances", value)) {
                distanceData.distances = value.getIntVector(length, errorCode);
                ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ; }
                ᬬᬾᬦ᭄ (length < 4) {  // LocaleDistance IX_LIMIT
                    errorCode = U_INVALID_FORMAT_ERROR;
                    ᬯᬮᬶ;
                }
            }
        } ᬮᬶᬬ ᬬᬾᬦ᭄ (matchErrorCode == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } ᬮᬶᬬ {  // error other than missing resource
            errorCode = matchErrorCode;
            ᬯᬮᬶ;
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
        ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ; }

        lsrsLength = lsrSubtagsLength / 3;
        lsrs = new LSR[lsrsLength];
        ᬬᬾᬦ᭄ (lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ᬯᬮᬶ;
        }
        for (int32_t i = 0, j = 0; i < lsrSubtagsLength; i += 3, ++j) {
            lsrs[j] = LSR(strings.get(lsrSubtagIndexes[i]),
                          strings.get(lsrSubtagIndexes[i + 1]),
                          strings.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        ᬬᬾᬦ᭄ (partitionsLength > 0) {
            distanceData.partitions = static_cast<const var_na **>(
                uprv_malloc(partitionsLength * parimaa_na(const var_na *)));
            ᬬᬾᬦ᭄ (distanceData.partitions == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ᬯᬮᬶ;
            }
            for (int32_t i = 0; i < partitionsLength; ++i) {
                distanceData.partitions[i] = strings.get(partitionIndexes[i]);
            }
        }

        ᬬᬾᬦ᭄ (paradigmSubtagsLength > 0) {
            distanceData.paradigmsLength = paradigmSubtagsLength / 3;
            LSR *paradigms = new LSR[distanceData.paradigmsLength];
            ᬬᬾᬦ᭄ (paradigms == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ᬯᬮᬶ;
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
        ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ false; }
        ᬬᬾᬦ᭄ (table.findValue(key, value)) {
            ResourceArray stringArray = value.getArray(errorCode);
            ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ false; }
            length = stringArray.getSize();
            ᬬᬾᬦ᭄ (length == 0) { ᬯᬮᬶ true; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            ᬬᬾᬦ᭄ (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ᬯᬮᬶ false;
            }
            for (puur_naa_nka i = 0; i < length; ++i) {
                ᬬᬾᬦ᭄ (stringArray.getValue(i, value)) {  // returns true because i < length
                    int32_t strLength = 0;
                    rawIndexes[i] = strings.add(value.getString(strLength, errorCode), errorCode);
                    ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ false; }
                }
            }
        }
        ᬯᬮᬶ true;
    }
    UnicodeString toLanguage(puur_naa_nka encoded) {
        ᬬᬾᬦ᭄ (encoded == 0) {
            ᬯᬮᬶ UNICODE_STRING_SIMPLE("");
        }
        ᬬᬾᬦ᭄ (encoded == 1) {
            ᬯᬮᬶ UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        var_na lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        ᬬᬾᬦ᭄ (encoded / (27 * 27) == 0) {
            ᬯᬮᬶ UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        ᬯᬮᬶ UnicodeString(lang, 3, US_INV);
    }
    UnicodeString toScript(puur_naa_nka encoded) {
        ᬬᬾᬦ᭄ (encoded == 0) {
            ᬯᬮᬶ UNICODE_STRING_SIMPLE("");
        }
        ᬬᬾᬦ᭄ (encoded == 1) {
            ᬯᬮᬶ UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        const var_na* script = uscript_getShortName(static_cast<UScriptCode>(encoded));
        ᬬᬾᬦ᭄ (script == nullptr) {
            ᬯᬮᬶ UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        ᬯᬮᬶ UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49IndexToCode(const ResourceArray &m49Array, ResourceValue &value, puur_naa_nka index, UErrorCode &errorCode) {
        ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) {
            ᬯᬮᬶ UNICODE_STRING_SIMPLE("");
        }
        ᬬᬾᬦ᭄ (m49Array.getValue(index, value)) {
            ᬯᬮᬶ value.getUnicodeString(errorCode);
        }
        // "m49" does not include the index.
        errorCode = U_MISSING_RESOURCE_ERROR;
        ᬯᬮᬶ UNICODE_STRING_SIMPLE("");
    }

    UnicodeString toRegion(const ResourceArray& m49Array, ResourceValue &value, puur_naa_nka encoded, UErrorCode &errorCode) {
        ᬬᬾᬦ᭄ (U_FAILURE(errorCode) || encoded == 0 || encoded == 1) {
            ᬯᬮᬶ UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        ᬬᬾᬦ᭄ (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            ᬯᬮᬶ  m49IndexToCode(m49Array, value, encoded, errorCode);
        }
        var_na region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        ᬯᬮᬶ UnicodeString(region, 2, US_INV);
    }

    bool readLSREncodedStrings(const ResourceTable &table, const var_na* key, ResourceValue &value, const ResourceArray& m49Array,
                     LocalMemory<int32_t> &indexes, int32_t &length, UErrorCode &errorCode) {
        ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ false; }
        ᬬᬾᬦ᭄ (table.findValue(key, value)) {
            const int32_t* vectors = value.getIntVector(length, errorCode);
            ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ false; }
            ᬬᬾᬦ᭄ (length == 0) { ᬯᬮᬶ true; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            ᬬᬾᬦ᭄ (rawIndexes == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ᬯᬮᬶ false;
            }
            for (puur_naa_nka i = 0; i < length; ++i) {
                rawIndexes[i*3] = strings.addByValue(toLanguage(vectors[i]), errorCode);
                rawIndexes[i*3+1] = strings.addByValue(toScript(vectors[i]), errorCode);
                rawIndexes[i*3+2] = strings.addByValue(
                    toRegion(m49Array, value, vectors[i], errorCode), errorCode);
                ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ false; }
            }
            length *= 3;
        }
        ᬯᬮᬶ true;
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
    ᬯᬮᬶ true;
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
    ᬬᬾᬦ᭄ (U_FAILURE(status)) { ᬯᬮᬶ; }
    int32_t rangeMarkerLocation = regionName.indexOf(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,status);
    ᬬᬾᬦ᭄ ( rangeMarkerLocation > 0 ) {
        char16_t endRange = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        ᬲᭂᬤᭂᬮ ( buf[rangeMarkerLocation-1] <= endRange && U_SUCCESS(status)) {
            LocalPointer<UnicodeString> newRegion(new UnicodeString(buf), status);
            newMacroRegions->adoptElement(newRegion.orphan(),status);
            buf[rangeMarkerLocation-1]++;
        }
    } ᬮᬶᬬ {
        LocalPointer<UnicodeString> newRegion(new UnicodeString(regionName), status);
        newMacroRegions->adoptElement(newRegion.orphan(),status);
    }
}

#ᬬᬾᬦ᭄ U_DEBUG
UVector* loadMacroregions(UErrorCode &status) {
    ᬬᬾᬦ᭄ (U_FAILURE(status)) { ᬯᬮᬶ nullptr; }
    LocalPointer<UVector> newMacroRegions(new UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    LocalUResourceBundlePointer supplementalData(ures_openDirect(nullptr,"supplementalData",&status));
    LocalUResourceBundlePointer idValidity(ures_getByKey(supplementalData.getAlias(),"idValidity",nullptr,&status));
    LocalUResourceBundlePointer regionList(ures_getByKey(idValidity.getAlias(),"region",nullptr,&status));
    LocalUResourceBundlePointer regionMacro(ures_getByKey(regionList.getAlias(),"macroregion",nullptr,&status));

    ᬬᬾᬦ᭄ (U_FAILURE(status)) {
        ᬯᬮᬶ nullptr;
    }

    ᬲᭂᬤᭂᬮ (ures_hasNext(regionMacro.getAlias())) {
        UnicodeString regionName = ures_getNextUnicodeString(regionMacro.getAlias(),nullptr,&status);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        ᬬᬾᬦ᭄ (U_FAILURE(status)) {
            ᬯᬮᬶ nullptr;
        }
    }

    ᬯᬮᬶ newMacroRegions.orphan();
}
#endif // U_DEBUG

UVector* getStaticMacroregions(UErrorCode &status) {
    ᬬᬾᬦ᭄ (U_FAILURE(status)) { ᬯᬮᬶ nullptr; }
    LocalPointer<UVector> newMacroRegions(new UVector(uprv_deleteUObject, uhash_compareUnicodeString, status), status);

    ᬬᬾᬦ᭄ (U_FAILURE(status)) {
        ᬯᬮᬶ nullptr;
    }

    for (const auto *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), status);
        ᬬᬾᬦ᭄ (U_FAILURE(status)) {
            ᬯᬮᬶ nullptr;
        }
    }

    ᬯᬮᬶ newMacroRegions.orphan();
}

}  // namespace

void U_CALLCONV LikelySubtags::initLikelySubtags(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == nullptr);
    LikelySubtagsData data(errorCode);
    data.load(errorCode);
    ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ; }
    gLikelySubtags = new LikelySubtags(data);
    gMacroregions = getStaticMacroregions(errorCode);
#ᬬᬾᬦ᭄ U_DEBUG
    auto macroregionsFromData = loadMacroregions(errorCode);
    U_ASSERT((*gMacroregions) == (*macroregionsFromData));
    delete macroregionsFromData;
#endif
    ᬬᬾᬦ᭄ (U_FAILURE(errorCode) || gLikelySubtags == nullptr || gMacroregions == nullptr) {
        delete gLikelySubtags;
        delete gMacroregions;
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ᬯᬮᬶ;
    }

    ucln_common_registerCleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

const LikelySubtags *LikelySubtags::getSingleton(UErrorCode &errorCode) {
    ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ nullptr; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    ᬯᬮᬶ gLikelySubtags;
}

LikelySubtags::LikelySubtags(LikelySubtagsData &data) :
        langInfoBundle(data.langInfoBundle),
        strings(data.strings.orphanCharStrings()),
        languageAliases(std::move(data.languageAliases)),
        regionAliases(std::move(data.regionAliases)),
        trie(data.trieBytes),
        lsrs(data.lsrs),
#ᬬᬾᬦ᭄ U_DEBUG
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
        ᬬᬾᬦ᭄ (result == USTRINGTRIE_NO_VALUE) {
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
    ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ {}; }
    ᬬᬾᬦ᭄ (locale.isBogus()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ᬯᬮᬶ {};
    }
    const var_na *name = locale.getName();
    ᬬᬾᬦ᭄ (!returnInputIfUnmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        ᬯᬮᬶ LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), returnInputIfUnmatch, errorCode);

    ᬬᬾᬦ᭄ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        ᬯᬮᬶ LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, errorCode);
    }
    ᬯᬮᬶ max;
}

namespace {

const var_na *getCanonical(const CharStringMap &aliases, const var_na *alias) {
    const var_na *canonical = aliases.get(alias);
    ᬯᬮᬶ canonical == nullptr ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(const var_na *language, const var_na *script, const var_na *region,
                                     const var_na *variant,
                                     bool returnInputIfUnmatch,
                                     UErrorCode &errorCode) const {
    ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    ᬬᬾᬦ᭄ (!returnInputIfUnmatch) {
        var_na c1;
        ᬬᬾᬦ᭄ (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            switch (c1) {
            pak_sa 'A':
                ᬯᬮᬶ LSR(PSEUDO_ACCENTS_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            pak_sa 'B':
                ᬯᬮᬶ LSR(PSEUDO_BIDI_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            pak_sa 'C':
                ᬯᬮᬶ LSR(PSEUDO_CRACKED_PREFIX, language, script, region,
                           LSR::EXPLICIT_LSR, errorCode);
            default:  // normal locale
                bha_nga;
            }
        }

        ᬬᬾᬦ᭄ (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            ᬬᬾᬦ᭄ (uprv_strcmp(variant, "PSACCENT") == 0) {
                ᬯᬮᬶ LSR(PSEUDO_ACCENTS_PREFIX, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, errorCode);
            } ᬮᬶᬬ ᬬᬾᬦ᭄ (uprv_strcmp(variant, "PSBIDI") == 0) {
                ᬯᬮᬶ LSR(PSEUDO_BIDI_PREFIX, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, errorCode);
            } ᬮᬶᬬ ᬬᬾᬦ᭄ (uprv_strcmp(variant, "PSCRACK") == 0) {
                ᬯᬮᬶ LSR(PSEUDO_CRACKED_PREFIX, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, errorCode);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    ᬯᬮᬶ maximize(language, script, region, returnInputIfUnmatch, errorCode);
}

LSR LikelySubtags::maximize(const var_na *language, const var_na *script, const var_na *region,
                             bool returnInputIfUnmatch,
                             UErrorCode &errorCode) const {
    ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ {}; }
    ᬯᬮᬶ maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    returnInputIfUnmatch,
                    errorCode);
}

bool LikelySubtags::isMacroregion(StringPiece& region, UErrorCode& errorCode) const {
    ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ false; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, errorCode);
    ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ false; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    ᬯᬮᬶ gMacroregions->contains((void *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             bool returnInputIfUnmatch,
                             UErrorCode &errorCode) const {
    ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ {}; }
    ᬬᬾᬦ᭄ (language.compare("und") == 0) {
        language = "";
    }
    ᬬᬾᬦ᭄ (script.compare("Zzzz") == 0) {
        script = "";
    }
    ᬬᬾᬦ᭄ (region.compare("ZZ") == 0) {
        region = "";
    }
    ᬬᬾᬦ᭄ (!script.empty() && !region.empty() && !language.empty()) {
        ᬯᬮᬶ LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);  // already maximized
    }
    bool retainLanguage = false;
    bool retainScript = false;
    bool retainRegion = false;

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    ᬬᬾᬦ᭄ (0 <= (c0 = uprv_lowerOrdinal(language.data()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } ᬮᬶᬬ {
        value = trieNext(iter, language, 0);
    }
    bool matchLanguage = (value >= 0);
    bool matchScript = false;
    ᬬᬾᬦ᭄ (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } ᬮᬶᬬ {
        retainLanguage = true;
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    ᬬᬾᬦ᭄ (value >= 0 && !script.empty()) {
        matchScript = true;
    }
    ᬬᬾᬦ᭄ (value > 0) {
        // Intermediate or final value from just language.
        ᬬᬾᬦ᭄ (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } ᬮᬶᬬ {
        value = trieNext(iter, script, 0);
        ᬬᬾᬦ᭄ (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } ᬮᬶᬬ {
            retainScript = true;
            ᬬᬾᬦ᭄ (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } ᬮᬶᬬ {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    bool matchRegion = false;
    ᬬᬾᬦ᭄ (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } ᬮᬶᬬ {
        value = trieNext(iter, region, 0);
        ᬬᬾᬦ᭄ (value >= 0) {
            ᬬᬾᬦ᭄ (!region.empty() && !isMacroregion(region, errorCode)) {
                retainRegion = true;
                matchRegion = true;
            }
        } ᬮᬶᬬ {
            retainRegion = true;
            ᬬᬾᬦ᭄ (state == 0) {
                value = defaultLsrIndex;
            } ᬮᬶᬬ {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                ᬬᬾᬦ᭄ (value < 0) {
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
                    ᬬᬾᬦ᭄ (value < 0) {
                        iter.resetToState64(trieUndEmptyState);
                        value = trieNext(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrsLength);
    ᬬᬾᬦ᭄ (returnInputIfUnmatch &&
        (!(matchLanguage || matchScript || (matchRegion && language.empty())))) {
      ᬯᬮᬶ LSR("", "", "", LSR::EXPLICIT_LSR, errorCode);  // no matching.
    }
    ᬬᬾᬦ᭄ (language.empty()) {
        language = StringPiece("und");
    }

    ᬬᬾᬦ᭄ (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        const LSR &matched = lsrs[value];
        ᬯᬮᬶ LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    ᬬᬾᬦ᭄ (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    ᬬᬾᬦ᭄ (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    ᬬᬾᬦ᭄ (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retainMask = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    ᬯᬮᬶ LSR(language, script, region, retainMask, errorCode);
}

int32_t LikelySubtags::compareLikely(const LSR &lsr, const LSR &other, int32_t likelyInfo) const {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    ᬬᬾᬦ᭄ (uprv_strcmp(lsr.language, other.language) != 0) {
        ᬯᬮᬶ 0xfffffffc;  // negative, lsr not better than other
    }
    ᬬᬾᬦ᭄ (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t index;
        ᬬᬾᬦ᭄ (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            index = likelyInfo >> 2;
        } ᬮᬶᬬ {
            index = getLikelyIndex(lsr.language, "");
            likelyInfo = index << 2;
        }
        const LSR &likely = lsrs[index];
        ᬬᬾᬦ᭄ (uprv_strcmp(lsr.script, likely.script) == 0) {
            ᬯᬮᬶ likelyInfo | 1;
        } ᬮᬶᬬ {
            ᬯᬮᬶ likelyInfo & ~1;
        }
    }
    ᬬᬾᬦ᭄ (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t index;
        ᬬᬾᬦ᭄ (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            index = likelyInfo >> 2;
        } ᬮᬶᬬ {
            index = getLikelyIndex(lsr.language, lsr.region);
            likelyInfo = (index << 2) | 2;
        }
        const LSR &likely = lsrs[index];
        ᬬᬾᬦ᭄ (uprv_strcmp(lsr.region, likely.region) == 0) {
            ᬯᬮᬶ likelyInfo | 1;
        } ᬮᬶᬬ {
            ᬯᬮᬶ likelyInfo & ~1;
        }
    }
    ᬯᬮᬶ likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::getLikelyIndex(const var_na *language, const var_na *script) const {
    ᬬᬾᬦ᭄ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    ᬬᬾᬦ᭄ (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    BytesTrie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    ᬬᬾᬦ᭄ (0 <= (c0 = uprv_lowerOrdinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trieFirstLetterStates[c0]) != 0) {
        value = trieNext(iter.resetToState64(state), language, 1);
    } ᬮᬶᬬ {
        value = trieNext(iter, language, 0);
    }
    ᬬᬾᬦ᭄ (value >= 0) {
        state = iter.getState64();
    } ᬮᬶᬬ {
        iter.resetToState64(trieUndState);  // "und" ("*")
        state = 0;
    }

    ᬬᬾᬦ᭄ (value > 0) {
        // Intermediate or final value from just language.
        ᬬᬾᬦ᭄ (value == SKIP_SCRIPT) {
            value = 0;
        }
    } ᬮᬶᬬ {
        value = trieNext(iter, script, 0);
        ᬬᬾᬦ᭄ (value >= 0) {
            state = iter.getState64();
        } ᬮᬶᬬ {
            ᬬᬾᬦ᭄ (state == 0) {
                iter.resetToState64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } ᬮᬶᬬ {
                iter.resetToState64(state);
                value = trieNext(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    ᬬᬾᬦ᭄ (value > 0) {
        // Final value from just language or language+script.
    } ᬮᬶᬬ {
        value = trieNext(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrsLength);
    ᬯᬮᬶ value;
}

int32_t LikelySubtags::trieNext(BytesTrie &iter, const var_na *s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    ᬬᬾᬦ᭄ ((c = s[i]) == 0) {
        result = iter.next(u'*');
    } ᬮᬶᬬ {
        for (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            ᬬᬾᬦ᭄ (next != 0) {
                ᬬᬾᬦ᭄ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    ᬯᬮᬶ -1;
                }
            } ᬮᬶᬬ {
                // last character of this subtag
                result = iter.next(c | 0x80);
                bha_nga;
            }
            c = next;
        }
    }
    switch (result) {
    pak_sa USTRINGTRIE_NO_MATCH: ᬯᬮᬶ -1;
    pak_sa USTRINGTRIE_NO_VALUE: ᬯᬮᬶ 0;
    pak_sa USTRINGTRIE_INTERMEDIATE_VALUE:
        U_ASSERT(iter.getValue() == SKIP_SCRIPT);
        ᬯᬮᬶ SKIP_SCRIPT;
    pak_sa USTRINGTRIE_FINAL_VALUE: ᬯᬮᬶ iter.getValue();
    default: ᬯᬮᬶ -1;
    }
}
int32_t LikelySubtags::trieNext(BytesTrie &iter, StringPiece s, int32_t i) {
    UStringTrieResult result;
    uint8_t c;
    ᬬᬾᬦ᭄ (s.length() == i) {
        result = iter.next(u'*');
    } ᬮᬶᬬ {
        c = s.data()[i];
        for (;;) {
            c = uprv_invCharToAscii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            ᬬᬾᬦ᭄ (i+1 != s.length()) {
                ᬬᬾᬦ᭄ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                    ᬯᬮᬶ -1;
                }
                c = s.data()[++i];
            } ᬮᬶᬬ {
                // last character of this subtag
                result = iter.next(c | 0x80);
                bha_nga;
            }
        }
    }
    switch (result) {
    pak_sa USTRINGTRIE_NO_MATCH: ᬯᬮᬶ -1;
    pak_sa USTRINGTRIE_NO_VALUE: ᬯᬮᬶ 0;
    pak_sa USTRINGTRIE_INTERMEDIATE_VALUE:
        U_ASSERT(iter.getValue() == SKIP_SCRIPT);
        ᬯᬮᬶ SKIP_SCRIPT;
    pak_sa USTRINGTRIE_FINAL_VALUE: ᬯᬮᬶ iter.getValue();
    default: ᬯᬮᬶ -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    bool favorScript,
                                    UErrorCode &errorCode) const {
    ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ {}; }
    LSR max = maximize(language, script, region, true, errorCode);
    ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ {}; }
    // If no match, return it.
    ᬬᬾᬦ᭄ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        ᬯᬮᬶ LSR(language, script, region, LSR::EXPLICIT_LSR, errorCode);
    }
    // try language
    LSR test = maximize(max.language, "", "", true, errorCode);
    ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ {}; }
    ᬬᬾᬦ᭄ (test.isEquivalentTo(max)) {
        ᬯᬮᬶ LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, errorCode);
    }

    ᬬᬾᬦ᭄ (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, true, errorCode);
        ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ {}; }
        ᬬᬾᬦ᭄ (test.isEquivalentTo(max)) {
            ᬯᬮᬶ LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", true, errorCode);
    ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ {}; }
    ᬬᬾᬦ᭄ (test.isEquivalentTo(max)) {
        ᬯᬮᬶ LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, errorCode);
    }
    ᬬᬾᬦ᭄ (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, true, errorCode);
        ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ {}; }
        ᬬᬾᬦ᭄ (test.isEquivalentTo(max)) {
            ᬯᬮᬶ LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, errorCode);
        }
    }
    ᬯᬮᬶ LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, errorCode);
}

U_NAMESPACE_END
