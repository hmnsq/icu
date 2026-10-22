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
    स्थिर स्थिरांक LocaleDistance *getSingleton(UErrorCode &errorCode);

    स्थिर int32_t shiftDistance(int32_t distance) {
        परत distance << DISTANCE_SHIFT;
    }

    स्थिर int32_t getShiftedDistance(int32_t indexAndDistance) {
        परत indexAndDistance & DISTANCE_MASK;
    }

    स्थिर द्विगुण getDistanceDouble(int32_t indexAndDistance) {
        द्विगुण shiftedDistance = getShiftedDistance(indexAndDistance);
        परत shiftedDistance / (1 << DISTANCE_SHIFT);
    }

    स्थिर int32_t getDistanceFloor(int32_t indexAndDistance) {
        परत (indexAndDistance & DISTANCE_MASK) >> DISTANCE_SHIFT;
    }

    स्थिर int32_t getIndex(int32_t indexAndDistance) {
        // assert indexAndDistance >= 0;
        परत indexAndDistance >> INDEX_SHIFT;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t getBestIndexAndDistance(स्थिरांक LSR &desired,
                                    स्थिरांक LSR **supportedLSRs, int32_t supportedLSRsLength,
                                    int32_t shiftedThreshold,
                                    ULocMatchFavorSubtag favorSubtag,
                                    ULocMatchDirection direction) स्थिरांक;

    bool isParadigmLSR(स्थिरांक LSR &lsr) स्थिरांक;

    int32_t getDefaultScriptDistance() स्थिरांक {
        परत defaultScriptDistance;
    }

    int32_t getDefaultDemotionPerDesiredLocale() स्थिरांक {
        परत defaultDemotionPerDesiredLocale;
    }

खाजगी:
    // The distance is shifted left to gain some fraction bits.
    स्थिर constexpr int32_t DISTANCE_SHIFT = 3;
    स्थिर constexpr int32_t DISTANCE_FRACTION_MASK = 7;
    // 7 bits for 0..100
    स्थिर constexpr int32_t DISTANCE_INT_SHIFT = 7;
    स्थिर constexpr int32_t INDEX_SHIFT = DISTANCE_INT_SHIFT + DISTANCE_SHIFT;
    स्थिर constexpr int32_t DISTANCE_MASK = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    स्थिर constexpr int32_t INDEX_NEG_1 = 0xfffffc00;

    LocaleDistance(स्थिरांक LocaleDistanceData &data, स्थिरांक LikelySubtags &likely);
    LocaleDistance(स्थिरांक LocaleDistance &other) = delete;
    LocaleDistance &चालक=(स्थिरांक LocaleDistance &other) = delete;

    स्थिर शून्य initLocaleDistance(UErrorCode &errorCode);

    bool isMatch(स्थिरांक LSR &desired, स्थिरांक LSR &supported,
                 int32_t shiftedThreshold, ULocMatchFavorSubtag favorSubtag) स्थिरांक {
        स्थिरांक LSR *pSupp = &supported;
        परत getBestIndexAndDistance(
            desired, &pSupp, 1,
            shiftedThreshold, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    स्थिर int32_t getDesSuppScriptDistance(BytesTrie &iter, uint64_t startState,
                                            स्थिरांक अक्षर *desired, स्थिरांक अक्षर *supported);

    स्थिर int32_t getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        स्थिरांक अक्षर *desiredPartitions, स्थिरांक अक्षर *supportedPartitions,
        int32_t threshold);

    स्थिर int32_t getFallbackRegionDistance(BytesTrie &iter, uint64_t startState);

    स्थिर int32_t trieNext(BytesTrie &iter, स्थिरांक अक्षर *s, bool want_मूल्य);

    स्थिरांक अक्षर *partitionsForRegion(स्थिरांक LSR &lsr) स्थिरांक {
        // ill-formed region -> one non-matching string
        int32_t pIndex = regionToPartitionsIndex[lsr.regionIndex];
        परत partitionArrays[pIndex];
    }

    int32_t getDefaultRegionDistance() स्थिरांक {
        परत defaultRegionDistance;
    }

    स्थिरांक LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    BytesTrie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    स्थिरांक uint8_t *regionToPartitionsIndex;
    स्थिरांक अक्षर **partitionArrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    स्थिरांक LSR *paradigmLSRs;
    int32_t paradigmLSRsLength;

    int32_t defaultLanguageDistance;
    int32_t defaultScriptDistance;
    int32_t defaultRegionDistance;
    int32_t minRegionDistance;
    int32_t defaultDemotionPerDesiredLocale;
};

U_NAMESPACE_END

#endif  // __LOCDISTANCE_H__
