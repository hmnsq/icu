// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCDISTANCE_H__
#define __LOCDISTANCE_H__

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/bytestrie.h"
#அந்தர்பாவ "unicode/localematcher.h"
#அந்தர்பாவ "unicode/locid.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "lsr.h"

U_NAMESPACE_BEGIN

கட்டமைப்பு LocaleDistanceData;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class LocaleDistance final : பொது UMemory {
பொது:
    நிலையான மாறிலி LocaleDistance *getSingleton(UErrorCode &errorCode);

    நிலையான int32_t shiftDistance(int32_t distance) {
        திரும்பு distance << DISTANCE_SHIFT;
    }

    நிலையான int32_t getShiftedDistance(int32_t indexAndDistance) {
        திரும்பு indexAndDistance & DISTANCE_MASK;
    }

    நிலையான இரட்டை getDistanceDouble(int32_t indexAndDistance) {
        இரட்டை shiftedDistance = getShiftedDistance(indexAndDistance);
        திரும்பு shiftedDistance / (1 << DISTANCE_SHIFT);
    }

    நிலையான int32_t getDistanceFloor(int32_t indexAndDistance) {
        திரும்பு (indexAndDistance & DISTANCE_MASK) >> DISTANCE_SHIFT;
    }

    நிலையான int32_t getIndex(int32_t indexAndDistance) {
        // assert indexAndDistance >= 0;
        திரும்பு indexAndDistance >> INDEX_SHIFT;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t getBestIndexAndDistance(மாறிலி LSR &desired,
                                    மாறிலி LSR **supportedLSRs, int32_t supportedLSRsLength,
                                    int32_t shiftedThreshold,
                                    ULocMatchFavorSubtag favorSubtag,
                                    ULocMatchDirection direction) மாறிலி;

    bool isParadigmLSR(மாறிலி LSR &lsr) மாறிலி;

    int32_t getDefaultScriptDistance() மாறிலி {
        திரும்பு defaultScriptDistance;
    }

    int32_t getDefaultDemotionPerDesiredLocale() மாறிலி {
        திரும்பு defaultDemotionPerDesiredLocale;
    }

தனிப்பட்ட:
    // The distance is shifted left to gain some fraction bits.
    நிலையான constexpr int32_t DISTANCE_SHIFT = 3;
    நிலையான constexpr int32_t DISTANCE_FRACTION_MASK = 7;
    // 7 bits for 0..100
    நிலையான constexpr int32_t DISTANCE_INT_SHIFT = 7;
    நிலையான constexpr int32_t INDEX_SHIFT = DISTANCE_INT_SHIFT + DISTANCE_SHIFT;
    நிலையான constexpr int32_t DISTANCE_MASK = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    நிலையான constexpr int32_t INDEX_NEG_1 = 0xfffffc00;

    LocaleDistance(மாறிலி LocaleDistanceData &data, மாறிலி LikelySubtags &likely);
    LocaleDistance(மாறிலி LocaleDistance &other) = delete;
    LocaleDistance &operator=(மாறிலி LocaleDistance &other) = delete;

    நிலையான வெறுமை initLocaleDistance(UErrorCode &errorCode);

    bool isMatch(மாறிலி LSR &desired, மாறிலி LSR &supported,
                 int32_t shiftedThreshold, ULocMatchFavorSubtag favorSubtag) மாறிலி {
        மாறிலி LSR *pSupp = &supported;
        திரும்பு getBestIndexAndDistance(
            desired, &pSupp, 1,
            shiftedThreshold, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    நிலையான int32_t getDesSuppScriptDistance(BytesTrie &iter, uint64_t startState,
                                            மாறிலி எழுத்து *desired, மாறிலி எழுத்து *supported);

    நிலையான int32_t getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        மாறிலி எழுத்து *desiredPartitions, மாறிலி எழுத்து *supportedPartitions,
        int32_t threshold);

    நிலையான int32_t getFallbackRegionDistance(BytesTrie &iter, uint64_t startState);

    நிலையான int32_t trieNext(BytesTrie &iter, மாறிலி எழுத்து *s, bool want_மதிப்பு);

    மாறிலி எழுத்து *partitionsForRegion(மாறிலி LSR &lsr) மாறிலி {
        // ill-formed region -> one non-matching string
        int32_t pIndex = regionToPartitionsIndex[lsr.regionIndex];
        திரும்பு partitionArrays[pIndex];
    }

    int32_t getDefaultRegionDistance() மாறிலி {
        திரும்பு defaultRegionDistance;
    }

    மாறிலி LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    BytesTrie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    மாறிலி uint8_t *regionToPartitionsIndex;
    மாறிலி எழுத்து **partitionArrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    மாறிலி LSR *paradigmLSRs;
    int32_t paradigmLSRsLength;

    int32_t defaultLanguageDistance;
    int32_t defaultScriptDistance;
    int32_t defaultRegionDistance;
    int32_t minRegionDistance;
    int32_t defaultDemotionPerDesiredLocale;
};

U_NAMESPACE_END

#endif  // __LOCDISTANCE_H__
