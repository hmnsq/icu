// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCDISTANCE_H__
#define __LOCDISTANCE_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/localematcher.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "lsr.h"

U_NAMESPACE_BEGIN

ಸಂರಚನಾ LocaleDistanceData;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class LocaleDistance final : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ ಧ್ರುವ LocaleDistance *getSingleton(UErrorCode &errorCode);

    ಸ್ಥಿರ int32_t shiftDistance(int32_t distance) {
        ಪ್ರತ್ಯಾಗಮ distance << DISTANCE_SHIFT;
    }

    ಸ್ಥಿರ int32_t getShiftedDistance(int32_t indexAndDistance) {
        ಪ್ರತ್ಯಾಗಮ indexAndDistance & DISTANCE_MASK;
    }

    ಸ್ಥಿರ ದ್ವಿಗುಣ getDistanceDouble(int32_t indexAndDistance) {
        ದ್ವಿಗುಣ shiftedDistance = getShiftedDistance(indexAndDistance);
        ಪ್ರತ್ಯಾಗಮ shiftedDistance / (1 << DISTANCE_SHIFT);
    }

    ಸ್ಥಿರ int32_t getDistanceFloor(int32_t indexAndDistance) {
        ಪ್ರತ್ಯಾಗಮ (indexAndDistance & DISTANCE_MASK) >> DISTANCE_SHIFT;
    }

    ಸ್ಥಿರ int32_t getIndex(int32_t indexAndDistance) {
        // assert indexAndDistance >= 0;
        ಪ್ರತ್ಯಾಗಮ indexAndDistance >> INDEX_SHIFT;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t getBestIndexAndDistance(ಧ್ರುವ LSR &desired,
                                    ಧ್ರುವ LSR **supportedLSRs, int32_t supportedLSRsLength,
                                    int32_t shiftedThreshold,
                                    ULocMatchFavorSubtag favorSubtag,
                                    ULocMatchDirection direction) ಧ್ರುವ;

    bool isParadigmLSR(ಧ್ರುವ LSR &lsr) ಧ್ರುವ;

    int32_t getDefaultScriptDistance() ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ defaultScriptDistance;
    }

    int32_t getDefaultDemotionPerDesiredLocale() ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ defaultDemotionPerDesiredLocale;
    }

ಗುಪ್ತ:
    // The distance is shifted left to gain some fraction bits.
    ಸ್ಥಿರ constexpr int32_t DISTANCE_SHIFT = 3;
    ಸ್ಥಿರ constexpr int32_t DISTANCE_FRACTION_MASK = 7;
    // 7 bits for 0..100
    ಸ್ಥಿರ constexpr int32_t DISTANCE_INT_SHIFT = 7;
    ಸ್ಥಿರ constexpr int32_t INDEX_SHIFT = DISTANCE_INT_SHIFT + DISTANCE_SHIFT;
    ಸ್ಥಿರ constexpr int32_t DISTANCE_MASK = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    ಸ್ಥಿರ constexpr int32_t INDEX_NEG_1 = 0xfffffc00;

    LocaleDistance(ಧ್ರುವ LocaleDistanceData &data, ಧ್ರುವ LikelySubtags &likely);
    LocaleDistance(ಧ್ರುವ LocaleDistance &other) = delete;
    LocaleDistance &operator=(ಧ್ರುವ LocaleDistance &other) = delete;

    ಸ್ಥಿರ ಶೂನ್ಯ initLocaleDistance(UErrorCode &errorCode);

    bool isMatch(ಧ್ರುವ LSR &desired, ಧ್ರುವ LSR &supported,
                 int32_t shiftedThreshold, ULocMatchFavorSubtag favorSubtag) ಧ್ರುವ {
        ಧ್ರುವ LSR *pSupp = &supported;
        ಪ್ರತ್ಯಾಗಮ getBestIndexAndDistance(
            desired, &pSupp, 1,
            shiftedThreshold, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    ಸ್ಥಿರ int32_t getDesSuppScriptDistance(BytesTrie &iter, uint64_t startState,
                                            ಧ್ರುವ ವರ್ಣ *desired, ಧ್ರುವ ವರ್ಣ *supported);

    ಸ್ಥಿರ int32_t getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        ಧ್ರುವ ವರ್ಣ *desiredPartitions, ಧ್ರುವ ವರ್ಣ *supportedPartitions,
        int32_t threshold);

    ಸ್ಥಿರ int32_t getFallbackRegionDistance(BytesTrie &iter, uint64_t startState);

    ಸ್ಥಿರ int32_t trieNext(BytesTrie &iter, ಧ್ರುವ ವರ್ಣ *s, bool want_ಮೌಲ್ಯ);

    ಧ್ರುವ ವರ್ಣ *partitionsForRegion(ಧ್ರುವ LSR &lsr) ಧ್ರುವ {
        // ill-formed region -> one non-matching string
        int32_t pIndex = regionToPartitionsIndex[lsr.regionIndex];
        ಪ್ರತ್ಯಾಗಮ partitionArrays[pIndex];
    }

    int32_t getDefaultRegionDistance() ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ defaultRegionDistance;
    }

    ಧ್ರುವ LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    BytesTrie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    ಧ್ರುವ uint8_t *regionToPartitionsIndex;
    ಧ್ರುವ ವರ್ಣ **partitionArrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    ಧ್ರುವ LSR *paradigmLSRs;
    int32_t paradigmLSRsLength;

    int32_t defaultLanguageDistance;
    int32_t defaultScriptDistance;
    int32_t defaultRegionDistance;
    int32_t minRegionDistance;
    int32_t defaultDemotionPerDesiredLocale;
};

U_NAMESPACE_END

#endif  // __LOCDISTANCE_H__
