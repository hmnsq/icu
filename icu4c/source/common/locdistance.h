// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#अगर_अघोषित __LOCDISTANCE_H__
#घोषणा __LOCDISTANCE_H__

#समावेश "unicode/utypes.h"
#समावेश "unicode/bytestrie.h"
#समावेश "unicode/localematcher.h"
#समावेश "unicode/locid.h"
#समावेश "unicode/uobject.h"
#समावेश "lsr.h"

U_NAMESPACE_BEGIN

काष्ठा Locale_Distance_तथ्य;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class LocaleDistance final : खुला UMemory {
खुला:
    अटल स्थिर LocaleDistance *getSingleton(UErrorCode &errorCode);

    अटल int32_t shiftDistance(int32_t distance) {
        वापस distance << DISTANCE_SHIFT;
    }

    अटल int32_t getShiftedDistance(int32_t सूचकांक_And_Distance) {
        वापस सूचकांक_And_Distance & DISTANCE_MASK;
    }

    अटल द्विगुन getDistanceDouble(int32_t सूचकांक_And_Distance) {
        द्विगुन shiftedDistance = getShiftedDistance(सूचकांक_And_Distance);
        वापस shiftedDistance / (1 << DISTANCE_SHIFT);
    }

    अटल int32_t getDistanceFloor(int32_t सूचकांक_And_Distance) {
        वापस (सूचकांक_And_Distance & DISTANCE_MASK) >> DISTANCE_SHIFT;
    }

    अटल int32_t get_सूचकांक(int32_t सूचकांक_And_Distance) {
        // assert indexAndDistance >= 0;
        वापस सूचकांक_And_Distance >> सूचकांक_SHIFT;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t get_Best_सूचकांक_And_Distance(स्थिर LSR &desired,
                                    स्थिर LSR **supportedLSRs, int32_t supported_LSRs_लंबाई,
                                    int32_t shiftedThreshold,
                                    ULocMatchFavorSubtag favorSubtag,
                                    ULocMatchDirection direction) स्थिर;

    bool isParadigmLSR(स्थिर LSR &lsr) स्थिर;

    int32_t getDefaultScriptDistance() स्थिर {
        वापस defaultScriptDistance;
    }

    int32_t getDefaultDemotionPerDesiredLocale() स्थिर {
        वापस defaultDemotionPerDesiredLocale;
    }

निजी:
    // The distance is shifted left to gain some fraction bits.
    अटल constexpr int32_t DISTANCE_SHIFT = 3;
    अटल constexpr int32_t DISTANCE_FRACTION_MASK = 7;
    // 7 bits for 0..100
    अटल constexpr int32_t DISTANCE_INT_SHIFT = 7;
    अटल constexpr int32_t सूचकांक_SHIFT = DISTANCE_INT_SHIFT + DISTANCE_SHIFT;
    अटल constexpr int32_t DISTANCE_MASK = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    अटल constexpr int32_t सूचकांक_NEG_1 = 0xfffffc00;

    LocaleDistance(स्थिर Locale_Distance_तथ्य &तथ्य, स्थिर LikelySubtags &likely);
    LocaleDistance(स्थिर LocaleDistance &other) = delete;
    LocaleDistance &चालक=(स्थिर LocaleDistance &other) = delete;

    अटल व्योम initLocaleDistance(UErrorCode &errorCode);

    bool isMatch(स्थिर LSR &desired, स्थिर LSR &supported,
                 int32_t shiftedThreshold, ULocMatchFavorSubtag favorSubtag) स्थिर {
        स्थिर LSR *pSupp = &supported;
        वापस get_Best_सूचकांक_And_Distance(
            desired, &pSupp, 1,
            shiftedThreshold, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    अटल int32_t getDesSuppScriptDistance(BytesTrie &iter, uint64_t startState,
                                            स्थिर अक्षर *desired, स्थिर अक्षर *supported);

    अटल int32_t getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        स्थिर अक्षर *desiredPartitions, स्थिर अक्षर *supportedPartitions,
        int32_t threshold);

    अटल int32_t getFallbackRegionDistance(BytesTrie &iter, uint64_t startState);

    अटल int32_t trie_अग्र(BytesTrie &iter, स्थिर अक्षर *s, bool want_मूल्य);

    स्थिर अक्षर *partitionsForRegion(स्थिर LSR &lsr) स्थिर {
        // ill-formed region -> one non-matching string
        int32_t p_सूचकांक = region_To_Partitions_सूचकांक[lsr.region_सूचकांक];
        वापस partitionArrays[p_सूचकांक];
    }

    int32_t getDefaultRegionDistance() स्थिर {
        वापस defaultRegionDistance;
    }

    स्थिर LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    BytesTrie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    स्थिर uint8_t *region_To_Partitions_सूचकांक;
    स्थिर अक्षर **partitionArrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    स्थिर LSR *paradigmLSRs;
    int32_t paradigm_LSRs_लंबाई;

    int32_t defaultLanguageDistance;
    int32_t defaultScriptDistance;
    int32_t defaultRegionDistance;
    int32_t minRegionDistance;
    int32_t defaultDemotionPerDesiredLocale;
};

U_NAMESPACE_END

#पूर्ण_अगर  // __LOCDISTANCE_H__
