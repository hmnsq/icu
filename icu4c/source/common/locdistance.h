// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCDISTANCE_H__
#define __LOCDISTANCE_H__

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/bytestrie.h"
#অন্তর্ভাব "unicode/localematcher.h"
#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "lsr.h"

U_NAMESPACE_BEGIN

সংরচনা LocaleDistanceData;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class LocaleDistance final : সার্বজনিক UMemory {
সার্বজনিক:
    স্থির ধ্রুব LocaleDistance *getSingleton(UErrorCode &errorCode);

    স্থির int32_t shiftDistance(int32_t distance) {
        প্রত্যাগম distance << DISTANCE_SHIFT;
    }

    স্থির int32_t getShiftedDistance(int32_t indexAndDistance) {
        প্রত্যাগম indexAndDistance & DISTANCE_MASK;
    }

    স্থির দ্বিগুণ getDistanceDouble(int32_t indexAndDistance) {
        দ্বিগুণ shiftedDistance = getShiftedDistance(indexAndDistance);
        প্রত্যাগম shiftedDistance / (1 << DISTANCE_SHIFT);
    }

    স্থির int32_t getDistanceFloor(int32_t indexAndDistance) {
        প্রত্যাগম (indexAndDistance & DISTANCE_MASK) >> DISTANCE_SHIFT;
    }

    স্থির int32_t getIndex(int32_t indexAndDistance) {
        // assert indexAndDistance >= 0;
        প্রত্যাগম indexAndDistance >> INDEX_SHIFT;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t getBestIndexAndDistance(ধ্রুব LSR &desired,
                                    ধ্রুব LSR **supportedLSRs, int32_t supportedLSRsLength,
                                    int32_t shiftedThreshold,
                                    ULocMatchFavorSubtag favorSubtag,
                                    ULocMatchDirection direction) ধ্রুব;

    bool isParadigmLSR(ধ্রুব LSR &lsr) ধ্রুব;

    int32_t getDefaultScriptDistance() ধ্রুব {
        প্রত্যাগম defaultScriptDistance;
    }

    int32_t getDefaultDemotionPerDesiredLocale() ধ্রুব {
        প্রত্যাগম defaultDemotionPerDesiredLocale;
    }

গুপ্ত:
    // The distance is shifted left to gain some fraction bits.
    স্থির constexpr int32_t DISTANCE_SHIFT = 3;
    স্থির constexpr int32_t DISTANCE_FRACTION_MASK = 7;
    // 7 bits for 0..100
    স্থির constexpr int32_t DISTANCE_INT_SHIFT = 7;
    স্থির constexpr int32_t INDEX_SHIFT = DISTANCE_INT_SHIFT + DISTANCE_SHIFT;
    স্থির constexpr int32_t DISTANCE_MASK = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    স্থির constexpr int32_t INDEX_NEG_1 = 0xfffffc00;

    LocaleDistance(ধ্রুব LocaleDistanceData &data, ধ্রুব LikelySubtags &likely);
    LocaleDistance(ধ্রুব LocaleDistance &other) = delete;
    LocaleDistance &operator=(ধ্রুব LocaleDistance &other) = delete;

    স্থির শূন্য initLocaleDistance(UErrorCode &errorCode);

    bool isMatch(ধ্রুব LSR &desired, ধ্রুব LSR &supported,
                 int32_t shiftedThreshold, ULocMatchFavorSubtag favorSubtag) ধ্রুব {
        ধ্রুব LSR *pSupp = &supported;
        প্রত্যাগম getBestIndexAndDistance(
            desired, &pSupp, 1,
            shiftedThreshold, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    স্থির int32_t getDesSuppScriptDistance(BytesTrie &iter, uint64_t startState,
                                            ধ্রুব বর্ণ *desired, ধ্রুব বর্ণ *supported);

    স্থির int32_t getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        ধ্রুব বর্ণ *desiredPartitions, ধ্রুব বর্ণ *supportedPartitions,
        int32_t threshold);

    স্থির int32_t getFallbackRegionDistance(BytesTrie &iter, uint64_t startState);

    স্থির int32_t trieNext(BytesTrie &iter, ধ্রুব বর্ণ *s, bool want_মান);

    ধ্রুব বর্ণ *partitionsForRegion(ধ্রুব LSR &lsr) ধ্রুব {
        // ill-formed region -> one non-matching string
        int32_t pIndex = regionToPartitionsIndex[lsr.regionIndex];
        প্রত্যাগম partitionArrays[pIndex];
    }

    int32_t getDefaultRegionDistance() ধ্রুব {
        প্রত্যাগম defaultRegionDistance;
    }

    ধ্রুব LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    BytesTrie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    ধ্রুব uint8_t *regionToPartitionsIndex;
    ধ্রুব বর্ণ **partitionArrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    ধ্রুব LSR *paradigmLSRs;
    int32_t paradigmLSRsLength;

    int32_t defaultLanguageDistance;
    int32_t defaultScriptDistance;
    int32_t defaultRegionDistance;
    int32_t minRegionDistance;
    int32_t defaultDemotionPerDesiredLocale;
};

U_NAMESPACE_END

#endif  // __LOCDISTANCE_H__
