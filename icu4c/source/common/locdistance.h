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

কাঠামো Locale_Distance_তথ্য;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class LocaleDistance final : সর্বজনীন UMemory {
সর্বজনীন:
    স্থির ধ্রুবক LocaleDistance *getSingleton(UErrorCode &errorCode);

    স্থির int32_t shiftDistance(int32_t distance) {
        ফেরত distance << DISTANCE_SHIFT;
    }

    স্থির int32_t getShiftedDistance(int32_t সূচক_And_Distance) {
        ফেরত সূচক_And_Distance & DISTANCE_MASK;
    }

    স্থির দ্বিগুণ getDistanceDouble(int32_t সূচক_And_Distance) {
        দ্বিগুণ shiftedDistance = getShiftedDistance(সূচক_And_Distance);
        ফেরত shiftedDistance / (1 << DISTANCE_SHIFT);
    }

    স্থির int32_t getDistanceFloor(int32_t সূচক_And_Distance) {
        ফেরত (সূচক_And_Distance & DISTANCE_MASK) >> DISTANCE_SHIFT;
    }

    স্থির int32_t get_সূচক(int32_t সূচক_And_Distance) {
        // assert indexAndDistance >= 0;
        ফেরত সূচক_And_Distance >> সূচক_SHIFT;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t get_Best_সূচক_And_Distance(ধ্রুবক LSR &desired,
                                    ধ্রুবক LSR **supportedLSRs, int32_t supported_LSRs_দৈর্ঘ্য,
                                    int32_t shiftedThreshold,
                                    ULocMatchFavorSubtag favorSubtag,
                                    ULocMatchDirection direction) ধ্রুবক;

    bool isParadigmLSR(ধ্রুবক LSR &lsr) ধ্রুবক;

    int32_t getDefaultScriptDistance() ধ্রুবক {
        ফেরত defaultScriptDistance;
    }

    int32_t getDefaultDemotionPerDesiredLocale() ধ্রুবক {
        ফেরত defaultDemotionPerDesiredLocale;
    }

ব্যক্তিগত:
    // The distance is shifted left to gain some fraction bits.
    স্থির constexpr int32_t DISTANCE_SHIFT = 3;
    স্থির constexpr int32_t DISTANCE_FRACTION_MASK = 7;
    // 7 bits for 0..100
    স্থির constexpr int32_t DISTANCE_INT_SHIFT = 7;
    স্থির constexpr int32_t সূচক_SHIFT = DISTANCE_INT_SHIFT + DISTANCE_SHIFT;
    স্থির constexpr int32_t DISTANCE_MASK = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    স্থির constexpr int32_t সূচক_NEG_1 = 0xfffffc00;

    LocaleDistance(ধ্রুবক Locale_Distance_তথ্য &তথ্য, ধ্রুবক LikelySubtags &likely);
    LocaleDistance(ধ্রুবক LocaleDistance &other) = delete;
    LocaleDistance &operator=(ধ্রুবক LocaleDistance &other) = delete;

    স্থির শূন্য initLocaleDistance(UErrorCode &errorCode);

    bool isMatch(ধ্রুবক LSR &desired, ধ্রুবক LSR &supported,
                 int32_t shiftedThreshold, ULocMatchFavorSubtag favorSubtag) ধ্রুবক {
        ধ্রুবক LSR *pSupp = &supported;
        ফেরত get_Best_সূচক_And_Distance(
            desired, &pSupp, 1,
            shiftedThreshold, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    স্থির int32_t getDesSuppScriptDistance(BytesTrie &iter, uint64_t startState,
                                            ধ্রুবক অক্ষর *desired, ধ্রুবক অক্ষর *supported);

    স্থির int32_t getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        ধ্রুবক অক্ষর *desiredPartitions, ধ্রুবক অক্ষর *supportedPartitions,
        int32_t threshold);

    স্থির int32_t getFallbackRegionDistance(BytesTrie &iter, uint64_t startState);

    স্থির int32_t trie_পরবর্তী(BytesTrie &iter, ধ্রুবক অক্ষর *s, bool want_মান);

    ধ্রুবক অক্ষর *partitionsForRegion(ধ্রুবক LSR &lsr) ধ্রুবক {
        // ill-formed region -> one non-matching string
        int32_t p_সূচক = region_To_Partitions_সূচক[lsr.region_সূচক];
        ফেরত partitionArrays[p_সূচক];
    }

    int32_t getDefaultRegionDistance() ধ্রুবক {
        ফেরত defaultRegionDistance;
    }

    ধ্রুবক LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    BytesTrie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    ধ্রুবক uint8_t *region_To_Partitions_সূচক;
    ধ্রুবক অক্ষর **partitionArrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    ধ্রুবক LSR *paradigmLSRs;
    int32_t paradigm_LSRs_দৈর্ঘ্য;

    int32_t defaultLanguageDistance;
    int32_t defaultScriptDistance;
    int32_t defaultRegionDistance;
    int32_t minRegionDistance;
    int32_t defaultDemotionPerDesiredLocale;
};

U_NAMESPACE_END

#endif  // __LOCDISTANCE_H__
