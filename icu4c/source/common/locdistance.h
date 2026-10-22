// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCDISTANCE_H__
#define __LOCDISTANCE_H__

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestrie.h"
#अन्तर्भाव "unicode/localematcher.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "lsr.h"

U_NAMESPACE_BEGIN

संरचना LocaleDistanceData;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class LocaleDistance final : सार्वजनिक UMemory {
सार्वजनिक:
    स्थिर ध्रुव LocaleDistance *getSingleton(UErrorCode &errorCode);

    स्थिर int32_t shiftDistance(int32_t distance) {
        प्रत्यागम distance << DISTANCE_SHIFT;
    }

    स्थिर int32_t getShiftedDistance(int32_t indexAndDistance) {
        प्रत्यागम indexAndDistance & DISTANCE_MASK;
    }

    स्थिर द्विगुण getDistanceDouble(int32_t indexAndDistance) {
        द्विगुण shiftedDistance = getShiftedDistance(indexAndDistance);
        प्रत्यागम shiftedDistance / (1 << DISTANCE_SHIFT);
    }

    स्थिर int32_t getDistanceFloor(int32_t indexAndDistance) {
        प्रत्यागम (indexAndDistance & DISTANCE_MASK) >> DISTANCE_SHIFT;
    }

    स्थिर int32_t getIndex(int32_t indexAndDistance) {
        // assert indexAndDistance >= 0;
        प्रत्यागम indexAndDistance >> INDEX_SHIFT;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t getBestIndexAndDistance(ध्रुव LSR &desired,
                                    ध्रुव LSR **supportedLSRs, int32_t supportedLSRsLength,
                                    int32_t shiftedThreshold,
                                    ULocMatchFavorSubtag favorSubtag,
                                    ULocMatchDirection direction) ध्रुव;

    bool isParadigmLSR(ध्रुव LSR &lsr) ध्रुव;

    int32_t getDefaultScriptDistance() ध्रुव {
        प्रत्यागम defaultScriptDistance;
    }

    int32_t getDefaultDemotionPerDesiredLocale() ध्रुव {
        प्रत्यागम defaultDemotionPerDesiredLocale;
    }

गुप्त:
    // The distance is shifted left to gain some fraction bits.
    स्थिर constexpr int32_t DISTANCE_SHIFT = 3;
    स्थिर constexpr int32_t DISTANCE_FRACTION_MASK = 7;
    // 7 bits for 0..100
    स्थिर constexpr int32_t DISTANCE_INT_SHIFT = 7;
    स्थिर constexpr int32_t INDEX_SHIFT = DISTANCE_INT_SHIFT + DISTANCE_SHIFT;
    स्थिर constexpr int32_t DISTANCE_MASK = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    स्थिर constexpr int32_t INDEX_NEG_1 = 0xfffffc00;

    LocaleDistance(ध्रुव LocaleDistanceData &data, ध्रुव LikelySubtags &likely);
    LocaleDistance(ध्रुव LocaleDistance &other) = delete;
    LocaleDistance &चालक=(ध्रुव LocaleDistance &other) = delete;

    स्थिर शून्य initLocaleDistance(UErrorCode &errorCode);

    bool isMatch(ध्रुव LSR &desired, ध्रुव LSR &supported,
                 int32_t shiftedThreshold, ULocMatchFavorSubtag favorSubtag) ध्रुव {
        ध्रुव LSR *pSupp = &supported;
        प्रत्यागम getBestIndexAndDistance(
            desired, &pSupp, 1,
            shiftedThreshold, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    स्थिर int32_t getDesSuppScriptDistance(BytesTrie &iter, uint64_t startState,
                                            ध्रुव वर्ण *desired, ध्रुव वर्ण *supported);

    स्थिर int32_t getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        ध्रुव वर्ण *desiredPartitions, ध्रुव वर्ण *supportedPartitions,
        int32_t threshold);

    स्थिर int32_t getFallbackRegionDistance(BytesTrie &iter, uint64_t startState);

    स्थिर int32_t trieNext(BytesTrie &iter, ध्रुव वर्ण *s, bool want_मूल्य);

    ध्रुव वर्ण *partitionsForRegion(ध्रुव LSR &lsr) ध्रुव {
        // ill-formed region -> one non-matching string
        int32_t pIndex = regionToPartitionsIndex[lsr.regionIndex];
        प्रत्यागम partitionArrays[pIndex];
    }

    int32_t getDefaultRegionDistance() ध्रुव {
        प्रत्यागम defaultRegionDistance;
    }

    ध्रुव LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    BytesTrie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    ध्रुव uint8_t *regionToPartitionsIndex;
    ध्रुव वर्ण **partitionArrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    ध्रुव LSR *paradigmLSRs;
    int32_t paradigmLSRsLength;

    int32_t defaultLanguageDistance;
    int32_t defaultScriptDistance;
    int32_t defaultRegionDistance;
    int32_t minRegionDistance;
    int32_t defaultDemotionPerDesiredLocale;
};

U_NAMESPACE_END

#endif  // __LOCDISTANCE_H__
