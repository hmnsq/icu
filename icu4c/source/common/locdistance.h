// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __LOCDISTANCE_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __LOCDISTANCE_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/localematcher.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "lsr.h"

U_NAMESPACE_BEGIN

ರಚನೆ Locale_ದೂರ_ದತ್ತಾಂಶ;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class Locale_ದೂರ final : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Locale_ದೂರ *getSingleton(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    ಸ್ಥಿರ int32_t ಸರಿಸು_ದೂರ(int32_t ದೂರ) {
        ಹಿಂತಿರುಗಿ ದೂರ << ದೂರ_ಸರಿಸು;
    }

    ಸ್ಥಿರ int32_t get_Shifted_ದೂರ(int32_t ಸೂಚ್ಯಂಕ_And_ದೂರ) {
        ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ_And_ದೂರ & ದೂರ_ಮುಖವಾಡ;
    }

    ಸ್ಥಿರ ದ್ವಿಗುಣ get_ದೂರ_Double(int32_t ಸೂಚ್ಯಂಕ_And_ದೂರ) {
        ದ್ವಿಗುಣ shifted_ದೂರ = get_Shifted_ದೂರ(ಸೂಚ್ಯಂಕ_And_ದೂರ);
        ಹಿಂತಿರುಗಿ shifted_ದೂರ / (1 << ದೂರ_ಸರಿಸು);
    }

    ಸ್ಥಿರ int32_t get_ದೂರ_Floor(int32_t ಸೂಚ್ಯಂಕ_And_ದೂರ) {
        ಹಿಂತಿರುಗಿ (ಸೂಚ್ಯಂಕ_And_ದೂರ & ದೂರ_ಮುಖವಾಡ) >> ದೂರ_ಸರಿಸು;
    }

    ಸ್ಥಿರ int32_t get_ಸೂಚ್ಯಂಕ(int32_t ಸೂಚ್ಯಂಕ_And_ದೂರ) {
        // assert indexAndDistance >= 0;
        ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ_And_ದೂರ >> ಸೂಚ್ಯಂಕ_ಸರಿಸು;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t get_ಉತ್ತಮ_ಸೂಚ್ಯಂಕ_And_ದೂರ(ಸ್ಥಿರಾಂಕ LSR &desired,
                                    ಸ್ಥಿರಾಂಕ LSR **supportedLSRs, int32_t supported_LSRs_ಉದ್ದಳತೆ,
                                    int32_t shifted_ಮಿತಿ,
                                    ULoc_ಹೊಂದಿಕೆ_Favor_Subtag favorSubtag,
                                    ULoc_ಹೊಂದಿಕೆ_Direction direction) ಸ್ಥಿರಾಂಕ;

    bool isParadigmLSR(ಸ್ಥಿರಾಂಕ LSR &lsr) ಸ್ಥಿರಾಂಕ;

    int32_t get_Default_Script_ದೂರ() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ default_Script_ದೂರ;
    }

    int32_t get_Default_Demotion_ಪ್ರತಿ_Desired_Locale() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ default_Demotion_ಪ್ರತಿ_Desired_Locale;
    }

ಖಾಸಗಿ:
    // The distance is shifted left to gain some fraction bits.
    ಸ್ಥಿರ constexpr int32_t ದೂರ_ಸರಿಸು = 3;
    ಸ್ಥಿರ constexpr int32_t ದೂರ_FRACTION_ಮುಖವಾಡ = 7;
    // 7 bits for 0..100
    ಸ್ಥಿರ constexpr int32_t ದೂರ_INT_ಸರಿಸು = 7;
    ಸ್ಥಿರ constexpr int32_t ಸೂಚ್ಯಂಕ_ಸರಿಸು = ದೂರ_INT_ಸರಿಸು + ದೂರ_ಸರಿಸು;
    ಸ್ಥಿರ constexpr int32_t ದೂರ_ಮುಖವಾಡ = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    ಸ್ಥಿರ constexpr int32_t ಸೂಚ್ಯಂಕ_NEG_1 = 0xfffffc00;

    Locale_ದೂರ(ಸ್ಥಿರಾಂಕ Locale_ದೂರ_ದತ್ತಾಂಶ &ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ LikelySubtags &likely);
    Locale_ದೂರ(ಸ್ಥಿರಾಂಕ Locale_ದೂರ &other) = delete;
    Locale_ದೂರ &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Locale_ದೂರ &other) = delete;

    ಸ್ಥಿರ ಶೂನ್ಯ init_Locale_ದೂರ(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    bool is_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ LSR &desired, ಸ್ಥಿರಾಂಕ LSR &supported,
                 int32_t shifted_ಮಿತಿ, ULoc_ಹೊಂದಿಕೆ_Favor_Subtag favorSubtag) ಸ್ಥಿರಾಂಕ {
        ಸ್ಥಿರಾಂಕ LSR *pSupp = &supported;
        ಹಿಂತಿರುಗಿ get_ಉತ್ತಮ_ಸೂಚ್ಯಂಕ_And_ದೂರ(
            desired, &pSupp, 1,
            shifted_ಮಿತಿ, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    ಸ್ಥಿರ int32_t get_Des_Supp_Script_ದೂರ(ಬೈಟುಗಳು_Trie &iter, uint64_t ಆರಂಭ_State,
                                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *desired, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *supported);

    ಸ್ಥಿರ int32_t get_Region_Partitions_ದೂರ(
        ಬೈಟುಗಳು_Trie &iter, uint64_t ಆರಂಭ_State,
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *desiredPartitions, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *supportedPartitions,
        int32_t ಮಿತಿ);

    ಸ್ಥಿರ int32_t get_Fallback_Region_ದೂರ(ಬೈಟುಗಳು_Trie &iter, uint64_t ಆರಂಭ_State);

    ಸ್ಥಿರ int32_t trie_ಮುಂದಿನ(ಬೈಟುಗಳು_Trie &iter, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, bool want_ಮೌಲ್ಯ);

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *partitionsForRegion(ಸ್ಥಿರಾಂಕ LSR &lsr) ಸ್ಥಿರಾಂಕ {
        // ill-formed region -> one non-matching string
        int32_t p_ಸೂಚ್ಯಂಕ = region_ಗಮ್ಯ_Partitions_ಸೂಚ್ಯಂಕ[lsr.region_ಸೂಚ್ಯಂಕ];
        ಹಿಂತಿರುಗಿ ವಿಭಜನೆ_Arrays[p_ಸೂಚ್ಯಂಕ];
    }

    int32_t get_Default_Region_ದೂರ() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ default_Region_ದೂರ;
    }

    ಸ್ಥಿರಾಂಕ LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    ಬೈಟುಗಳು_Trie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    ಸ್ಥಿರಾಂಕ uint8_t *region_ಗಮ್ಯ_Partitions_ಸೂಚ್ಯಂಕ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ವಿಭಜನೆ_Arrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    ಸ್ಥಿರಾಂಕ LSR *paradigmLSRs;
    int32_t paradigm_LSRs_ಉದ್ದಳತೆ;

    int32_t default_Language_ದೂರ;
    int32_t default_Script_ದೂರ;
    int32_t default_Region_ದೂರ;
    int32_t min_Region_ದೂರ;
    int32_t default_Demotion_ಪ್ರತಿ_Desired_Locale;
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  // __LOCDISTANCE_H__
