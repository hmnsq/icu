// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCDISTANCE_H__
#define __LOCDISTANCE_H__

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/bytestrie.h"
#અન્તર્ભાવ "unicode/localematcher.h"
#અન્તર્ભાવ "unicode/locid.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "lsr.h"

U_NAMESPACE_BEGIN

સંરચના LocaleDistanceData;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class LocaleDistance final : જાહેર UMemory {
જાહેર:
    સ્થિર અચલ LocaleDistance *getSingleton(UErrorCode &errorCode);

    સ્થિર int32_t shiftDistance(int32_t distance) {
        પાછા distance << DISTANCE_SHIFT;
    }

    સ્થિર int32_t getShiftedDistance(int32_t indexAndDistance) {
        પાછા indexAndDistance & DISTANCE_MASK;
    }

    સ્થિર દ્વિગુણ getDistanceDouble(int32_t indexAndDistance) {
        દ્વિગુણ shiftedDistance = getShiftedDistance(indexAndDistance);
        પાછા shiftedDistance / (1 << DISTANCE_SHIFT);
    }

    સ્થિર int32_t getDistanceFloor(int32_t indexAndDistance) {
        પાછા (indexAndDistance & DISTANCE_MASK) >> DISTANCE_SHIFT;
    }

    સ્થિર int32_t getIndex(int32_t indexAndDistance) {
        // assert indexAndDistance >= 0;
        પાછા indexAndDistance >> INDEX_SHIFT;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t getBestIndexAndDistance(અચલ LSR &desired,
                                    અચલ LSR **supportedLSRs, int32_t supportedLSRsLength,
                                    int32_t shiftedThreshold,
                                    ULocMatchFavorSubtag favorSubtag,
                                    ULocMatchDirection direction) અચલ;

    bool isParadigmLSR(અચલ LSR &lsr) અચલ;

    int32_t getDefaultScriptDistance() અચલ {
        પાછા defaultScriptDistance;
    }

    int32_t getDefaultDemotionPerDesiredLocale() અચલ {
        પાછા defaultDemotionPerDesiredLocale;
    }

ખાનગી:
    // The distance is shifted left to gain some fraction bits.
    સ્થિર constexpr int32_t DISTANCE_SHIFT = 3;
    સ્થિર constexpr int32_t DISTANCE_FRACTION_MASK = 7;
    // 7 bits for 0..100
    સ્થિર constexpr int32_t DISTANCE_INT_SHIFT = 7;
    સ્થિર constexpr int32_t INDEX_SHIFT = DISTANCE_INT_SHIFT + DISTANCE_SHIFT;
    સ્થિર constexpr int32_t DISTANCE_MASK = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    સ્થિર constexpr int32_t INDEX_NEG_1 = 0xfffffc00;

    LocaleDistance(અચલ LocaleDistanceData &data, અચલ LikelySubtags &likely);
    LocaleDistance(અચલ LocaleDistance &other) = delete;
    LocaleDistance &operator=(અચલ LocaleDistance &other) = delete;

    સ્થિર શૂન્ય initLocaleDistance(UErrorCode &errorCode);

    bool isMatch(અચલ LSR &desired, અચલ LSR &supported,
                 int32_t shiftedThreshold, ULocMatchFavorSubtag favorSubtag) અચલ {
        અચલ LSR *pSupp = &supported;
        પાછા getBestIndexAndDistance(
            desired, &pSupp, 1,
            shiftedThreshold, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    સ્થિર int32_t getDesSuppScriptDistance(BytesTrie &iter, uint64_t startState,
                                            અચલ અક્ષર *desired, અચલ અક્ષર *supported);

    સ્થિર int32_t getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        અચલ અક્ષર *desiredPartitions, અચલ અક્ષર *supportedPartitions,
        int32_t threshold);

    સ્થિર int32_t getFallbackRegionDistance(BytesTrie &iter, uint64_t startState);

    સ્થિર int32_t trieNext(BytesTrie &iter, અચલ અક્ષર *s, bool want_કિંમત);

    અચલ અક્ષર *partitionsForRegion(અચલ LSR &lsr) અચલ {
        // ill-formed region -> one non-matching string
        int32_t pIndex = regionToPartitionsIndex[lsr.regionIndex];
        પાછા partitionArrays[pIndex];
    }

    int32_t getDefaultRegionDistance() અચલ {
        પાછા defaultRegionDistance;
    }

    અચલ LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    BytesTrie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    અચલ uint8_t *regionToPartitionsIndex;
    અચલ અક્ષર **partitionArrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    અચલ LSR *paradigmLSRs;
    int32_t paradigmLSRsLength;

    int32_t defaultLanguageDistance;
    int32_t defaultScriptDistance;
    int32_t defaultRegionDistance;
    int32_t minRegionDistance;
    int32_t defaultDemotionPerDesiredLocale;
};

U_NAMESPACE_END

#endif  // __LOCDISTANCE_H__
