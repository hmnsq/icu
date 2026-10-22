// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCDISTANCE_H__
#define __LOCDISTANCE_H__

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/bytestrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/localematcher.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "lsr.h"

U_NAMESPACE_BEGIN

ସଂରଚନା LocaleDistanceData;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class LocaleDistance final : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
    ସ୍ଥିର ଅଚଳ LocaleDistance *getSingleton(UErrorCode &errorCode);

    ସ୍ଥିର int32_t shiftDistance(int32_t distance) {
        ଫେରନ୍ତୁ distance << DISTANCE_SHIFT;
    }

    ସ୍ଥିର int32_t getShiftedDistance(int32_t indexAndDistance) {
        ଫେରନ୍ତୁ indexAndDistance & DISTANCE_MASK;
    }

    ସ୍ଥିର ଦ୍ୱିଗୁଣ getDistanceDouble(int32_t indexAndDistance) {
        ଦ୍ୱିଗୁଣ shiftedDistance = getShiftedDistance(indexAndDistance);
        ଫେରନ୍ତୁ shiftedDistance / (1 << DISTANCE_SHIFT);
    }

    ସ୍ଥିର int32_t getDistanceFloor(int32_t indexAndDistance) {
        ଫେରନ୍ତୁ (indexAndDistance & DISTANCE_MASK) >> DISTANCE_SHIFT;
    }

    ସ୍ଥିର int32_t getIndex(int32_t indexAndDistance) {
        // assert indexAndDistance >= 0;
        ଫେରନ୍ତୁ indexAndDistance >> INDEX_SHIFT;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t getBestIndexAndDistance(ଅଚଳ LSR &desired,
                                    ଅଚଳ LSR **supportedLSRs, int32_t supportedLSRsLength,
                                    int32_t shiftedThreshold,
                                    ULocMatchFavorSubtag favorSubtag,
                                    ULocMatchDirection direction) ଅଚଳ;

    bool isParadigmLSR(ଅଚଳ LSR &lsr) ଅଚଳ;

    int32_t getDefaultScriptDistance() ଅଚଳ {
        ଫେରନ୍ତୁ defaultScriptDistance;
    }

    int32_t getDefaultDemotionPerDesiredLocale() ଅଚଳ {
        ଫେରନ୍ତୁ defaultDemotionPerDesiredLocale;
    }

ବ୍ୟକ୍ତିଗତ:
    // The distance is shifted left to gain some fraction bits.
    ସ୍ଥିର constexpr int32_t DISTANCE_SHIFT = 3;
    ସ୍ଥିର constexpr int32_t DISTANCE_FRACTION_MASK = 7;
    // 7 bits for 0..100
    ସ୍ଥିର constexpr int32_t DISTANCE_INT_SHIFT = 7;
    ସ୍ଥିର constexpr int32_t INDEX_SHIFT = DISTANCE_INT_SHIFT + DISTANCE_SHIFT;
    ସ୍ଥିର constexpr int32_t DISTANCE_MASK = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    ସ୍ଥିର constexpr int32_t INDEX_NEG_1 = 0xfffffc00;

    LocaleDistance(ଅଚଳ LocaleDistanceData &data, ଅଚଳ LikelySubtags &likely);
    LocaleDistance(ଅଚଳ LocaleDistance &other) = delete;
    LocaleDistance &operator=(ଅଚଳ LocaleDistance &other) = delete;

    ସ୍ଥିର ଶୂନ୍ୟ initLocaleDistance(UErrorCode &errorCode);

    bool isMatch(ଅଚଳ LSR &desired, ଅଚଳ LSR &supported,
                 int32_t shiftedThreshold, ULocMatchFavorSubtag favorSubtag) ଅଚଳ {
        ଅଚଳ LSR *pSupp = &supported;
        ଫେରନ୍ତୁ getBestIndexAndDistance(
            desired, &pSupp, 1,
            shiftedThreshold, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    ସ୍ଥିର int32_t getDesSuppScriptDistance(BytesTrie &iter, uint64_t startState,
                                            ଅଚଳ ଅକ୍ଷର *desired, ଅଚଳ ଅକ୍ଷର *supported);

    ସ୍ଥିର int32_t getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        ଅଚଳ ଅକ୍ଷର *desiredPartitions, ଅଚଳ ଅକ୍ଷର *supportedPartitions,
        int32_t threshold);

    ସ୍ଥିର int32_t getFallbackRegionDistance(BytesTrie &iter, uint64_t startState);

    ସ୍ଥିର int32_t trieNext(BytesTrie &iter, ଅଚଳ ଅକ୍ଷର *s, bool want_ମୂଲ୍ୟ);

    ଅଚଳ ଅକ୍ଷର *partitionsForRegion(ଅଚଳ LSR &lsr) ଅଚଳ {
        // ill-formed region -> one non-matching string
        int32_t pIndex = regionToPartitionsIndex[lsr.regionIndex];
        ଫେରନ୍ତୁ partitionArrays[pIndex];
    }

    int32_t getDefaultRegionDistance() ଅଚଳ {
        ଫେରନ୍ତୁ defaultRegionDistance;
    }

    ଅଚଳ LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    BytesTrie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    ଅଚଳ uint8_t *regionToPartitionsIndex;
    ଅଚଳ ଅକ୍ଷର **partitionArrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    ଅଚଳ LSR *paradigmLSRs;
    int32_t paradigmLSRsLength;

    int32_t defaultLanguageDistance;
    int32_t defaultScriptDistance;
    int32_t defaultRegionDistance;
    int32_t minRegionDistance;
    int32_t defaultDemotionPerDesiredLocale;
};

U_NAMESPACE_END

#endif  // __LOCDISTANCE_H__
