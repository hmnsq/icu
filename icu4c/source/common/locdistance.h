// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#यदि_अपरिभाषितम् __LOCDISTANCE_H__
#परिभाषय __LOCDISTANCE_H__

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestrie.h"
#अन्तर्भाव "unicode/localematcher.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "lsr.h"

U_NAMESPACE_BEGIN

संरचना Locale_Distance_दत्त;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class LocaleDistance final : सार्वजनिक UMemory {
सार्वजनिक:
    स्थिर ध्रुव LocaleDistance *getSingleton(UError_संहिता &error_संहिता);

    स्थिर int32_t सरणम्_Distance(int32_t distance) {
        प्रत्यागम distance << DISTANCE_सरणम्;
    }

    स्थिर int32_t getShiftedDistance(int32_t सूचकांक_And_Distance) {
        प्रत्यागम सूचकांक_And_Distance & DISTANCE_आवरणम्;
    }

    स्थिर द्विगुण getDistanceDouble(int32_t सूचकांक_And_Distance) {
        द्विगुण shiftedDistance = getShiftedDistance(सूचकांक_And_Distance);
        प्रत्यागम shiftedDistance / (1 << DISTANCE_सरणम्);
    }

    स्थिर int32_t getDistanceFloor(int32_t सूचकांक_And_Distance) {
        प्रत्यागम (सूचकांक_And_Distance & DISTANCE_आवरणम्) >> DISTANCE_सरणम्;
    }

    स्थिर int32_t get_सूचकांक(int32_t सूचकांक_And_Distance) {
        // assert indexAndDistance >= 0;
        प्रत्यागम सूचकांक_And_Distance >> सूचकांक_सरणम्;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t get_Best_सूचकांक_And_Distance(ध्रुव LSR &desired,
                                    ध्रुव LSR **supportedLSRs, int32_t supported_LSRs_दीर्घता,
                                    int32_t shifted_देहली,
                                    ULoc_मेलनम्_Favor_Subtag favorSubtag,
                                    ULoc_मेलनम्_Direction direction) ध्रुव;

    bool isParadigmLSR(ध्रुव LSR &lsr) ध्रुव;

    int32_t getDefaultScriptDistance() ध्रुव {
        प्रत्यागम defaultScriptDistance;
    }

    int32_t get_Default_Demotion_प्रत्येकम्_Desired_Locale() ध्रुव {
        प्रत्यागम default_Demotion_प्रत्येकम्_Desired_Locale;
    }

गुप्त:
    // The distance is shifted left to gain some fraction bits.
    स्थिर constexpr int32_t DISTANCE_सरणम् = 3;
    स्थिर constexpr int32_t DISTANCE_FRACTION_आवरणम् = 7;
    // 7 bits for 0..100
    स्थिर constexpr int32_t DISTANCE_INT_सरणम् = 7;
    स्थिर constexpr int32_t सूचकांक_सरणम् = DISTANCE_INT_सरणम् + DISTANCE_सरणम्;
    स्थिर constexpr int32_t DISTANCE_आवरणम् = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    स्थिर constexpr int32_t सूचकांक_NEG_1 = 0xfffffc00;

    LocaleDistance(ध्रुव Locale_Distance_दत्त &दत्त, ध्रुव LikelySubtags &likely);
    LocaleDistance(ध्रुव LocaleDistance &other) = delete;
    LocaleDistance &संक्रियकः=(ध्रुव LocaleDistance &other) = delete;

    स्थिर शून्य initLocaleDistance(UError_संहिता &error_संहिता);

    bool is_मेलनम्(ध्रुव LSR &desired, ध्रुव LSR &supported,
                 int32_t shifted_देहली, ULoc_मेलनम्_Favor_Subtag favorSubtag) ध्रुव {
        ध्रुव LSR *pSupp = &supported;
        प्रत्यागम get_Best_सूचकांक_And_Distance(
            desired, &pSupp, 1,
            shifted_देहली, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    स्थिर int32_t getDesSuppScriptDistance(अष्टकानि_Trie &iter, uint64_t startState,
                                            ध्रुव वर्ण *desired, ध्रुव वर्ण *supported);

    स्थिर int32_t getRegionPartitionsDistance(
        अष्टकानि_Trie &iter, uint64_t startState,
        ध्रुव वर्ण *desiredPartitions, ध्रुव वर्ण *supportedPartitions,
        int32_t देहली);

    स्थिर int32_t getFallbackRegionDistance(अष्टकानि_Trie &iter, uint64_t startState);

    स्थिर int32_t trie_अग्र(अष्टकानि_Trie &iter, ध्रुव वर्ण *s, bool want_मानम्);

    ध्रुव वर्ण *partitionsForRegion(ध्रुव LSR &lsr) ध्रुव {
        // ill-formed region -> one non-matching string
        int32_t p_सूचकांक = region_गन्तव्यम्_Partitions_सूचकांक[lsr.region_सूचकांक];
        प्रत्यागम partitionArrays[p_सूचकांक];
    }

    int32_t getDefaultRegionDistance() ध्रुव {
        प्रत्यागम defaultRegionDistance;
    }

    ध्रुव LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    अष्टकानि_Trie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    ध्रुव uint8_t *region_गन्तव्यम्_Partitions_सूचकांक;
    ध्रुव वर्ण **partitionArrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    ध्रुव LSR *paradigmLSRs;
    int32_t paradigm_LSRs_दीर्घता;

    int32_t defaultLanguageDistance;
    int32_t defaultScriptDistance;
    int32_t defaultRegionDistance;
    int32_t minRegionDistance;
    int32_t default_Demotion_प्रत्येकम्_Desired_Locale;
};

U_NAMESPACE_अन्तः

#यदि_अन्तः  // __LOCDISTANCE_H__
