// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCDISTANCE_H__
#define __LOCDISTANCE_H__

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/bytestrie.h"
#അന്തര്ഭാവ "unicode/localematcher.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "lsr.h"

U_NAMESPACE_BEGIN

ഘടന LocaleDistanceData;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class LocaleDistance final : പൊതു UMemory {
പൊതു:
    സ്ഥിതം സ്ഥിരം LocaleDistance *getSingleton(UErrorCode &errorCode);

    സ്ഥിതം int32_t shiftDistance(int32_t distance) {
        തിരികെ distance << DISTANCE_SHIFT;
    }

    സ്ഥിതം int32_t getShiftedDistance(int32_t indexAndDistance) {
        തിരികെ indexAndDistance & DISTANCE_MASK;
    }

    സ്ഥിതം ഇരട്ട getDistanceDouble(int32_t indexAndDistance) {
        ഇരട്ട shiftedDistance = getShiftedDistance(indexAndDistance);
        തിരികെ shiftedDistance / (1 << DISTANCE_SHIFT);
    }

    സ്ഥിതം int32_t getDistanceFloor(int32_t indexAndDistance) {
        തിരികെ (indexAndDistance & DISTANCE_MASK) >> DISTANCE_SHIFT;
    }

    സ്ഥിതം int32_t getIndex(int32_t indexAndDistance) {
        // assert indexAndDistance >= 0;
        തിരികെ indexAndDistance >> INDEX_SHIFT;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t getBestIndexAndDistance(സ്ഥിരം LSR &desired,
                                    സ്ഥിരം LSR **supportedLSRs, int32_t supportedLSRsLength,
                                    int32_t shiftedThreshold,
                                    ULocMatchFavorSubtag favorSubtag,
                                    ULocMatchDirection direction) സ്ഥിരം;

    bool isParadigmLSR(സ്ഥിരം LSR &lsr) സ്ഥിരം;

    int32_t getDefaultScriptDistance() സ്ഥിരം {
        തിരികെ defaultScriptDistance;
    }

    int32_t getDefaultDemotionPerDesiredLocale() സ്ഥിരം {
        തിരികെ defaultDemotionPerDesiredLocale;
    }

സ്വകാര്യ:
    // The distance is shifted left to gain some fraction bits.
    സ്ഥിതം constexpr int32_t DISTANCE_SHIFT = 3;
    സ്ഥിതം constexpr int32_t DISTANCE_FRACTION_MASK = 7;
    // 7 bits for 0..100
    സ്ഥിതം constexpr int32_t DISTANCE_INT_SHIFT = 7;
    സ്ഥിതം constexpr int32_t INDEX_SHIFT = DISTANCE_INT_SHIFT + DISTANCE_SHIFT;
    സ്ഥിതം constexpr int32_t DISTANCE_MASK = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    സ്ഥിതം constexpr int32_t INDEX_NEG_1 = 0xfffffc00;

    LocaleDistance(സ്ഥിരം LocaleDistanceData &data, സ്ഥിരം LikelySubtags &likely);
    LocaleDistance(സ്ഥിരം LocaleDistance &other) = delete;
    LocaleDistance &operator=(സ്ഥിരം LocaleDistance &other) = delete;

    സ്ഥിതം ശൂന്യം initLocaleDistance(UErrorCode &errorCode);

    bool isMatch(സ്ഥിരം LSR &desired, സ്ഥിരം LSR &supported,
                 int32_t shiftedThreshold, ULocMatchFavorSubtag favorSubtag) സ്ഥിരം {
        സ്ഥിരം LSR *pSupp = &supported;
        തിരികെ getBestIndexAndDistance(
            desired, &pSupp, 1,
            shiftedThreshold, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    സ്ഥിതം int32_t getDesSuppScriptDistance(BytesTrie &iter, uint64_t startState,
                                            സ്ഥിരം അക്ഷരം *desired, സ്ഥിരം അക്ഷരം *supported);

    സ്ഥിതം int32_t getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        സ്ഥിരം അക്ഷരം *desiredPartitions, സ്ഥിരം അക്ഷരം *supportedPartitions,
        int32_t threshold);

    സ്ഥിതം int32_t getFallbackRegionDistance(BytesTrie &iter, uint64_t startState);

    സ്ഥിതം int32_t trieNext(BytesTrie &iter, സ്ഥിരം അക്ഷരം *s, bool want_മൂല്യം);

    സ്ഥിരം അക്ഷരം *partitionsForRegion(സ്ഥിരം LSR &lsr) സ്ഥിരം {
        // ill-formed region -> one non-matching string
        int32_t pIndex = regionToPartitionsIndex[lsr.regionIndex];
        തിരികെ partitionArrays[pIndex];
    }

    int32_t getDefaultRegionDistance() സ്ഥിരം {
        തിരികെ defaultRegionDistance;
    }

    സ്ഥിരം LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    BytesTrie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    സ്ഥിരം uint8_t *regionToPartitionsIndex;
    സ്ഥിരം അക്ഷരം **partitionArrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    സ്ഥിരം LSR *paradigmLSRs;
    int32_t paradigmLSRsLength;

    int32_t defaultLanguageDistance;
    int32_t defaultScriptDistance;
    int32_t defaultRegionDistance;
    int32_t minRegionDistance;
    int32_t defaultDemotionPerDesiredLocale;
};

U_NAMESPACE_END

#endif  // __LOCDISTANCE_H__
