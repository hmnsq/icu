// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __LOCDISTANCE_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __LOCDISTANCE_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/localematcher.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lsr.h"

U_NAMESPACE_BEGIN

ਬਣਤਰ Locale_ਦੂਰੀ_ਅੰਕੜੇ;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class Locale_ਦੂਰੀ final : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    ਸਥਿਰ ਅਚਲ Locale_ਦੂਰੀ *getSingleton(UError_ਕੋਡ &error_ਕੋਡ);

    ਸਥਿਰ int32_t ਖਿਸਕਾਓ_ਦੂਰੀ(int32_t ਦੂਰੀ) {
        ਵਾਪਸ ਦੂਰੀ << ਦੂਰੀ_ਖਿਸਕਾਓ;
    }

    ਸਥਿਰ int32_t get_Shifted_ਦੂਰੀ(int32_t ਸੂਚਕਾਂਕ_And_ਦੂਰੀ) {
        ਵਾਪਸ ਸੂਚਕਾਂਕ_And_ਦੂਰੀ & ਦੂਰੀ_ਨਕਾਬ;
    }

    ਸਥਿਰ ਦੁੱਗਣਾ get_ਦੂਰੀ_Double(int32_t ਸੂਚਕਾਂਕ_And_ਦੂਰੀ) {
        ਦੁੱਗਣਾ shifted_ਦੂਰੀ = get_Shifted_ਦੂਰੀ(ਸੂਚਕਾਂਕ_And_ਦੂਰੀ);
        ਵਾਪਸ shifted_ਦੂਰੀ / (1 << ਦੂਰੀ_ਖਿਸਕਾਓ);
    }

    ਸਥਿਰ int32_t get_ਦੂਰੀ_Floor(int32_t ਸੂਚਕਾਂਕ_And_ਦੂਰੀ) {
        ਵਾਪਸ (ਸੂਚਕਾਂਕ_And_ਦੂਰੀ & ਦੂਰੀ_ਨਕਾਬ) >> ਦੂਰੀ_ਖਿਸਕਾਓ;
    }

    ਸਥਿਰ int32_t get_ਸੂਚਕਾਂਕ(int32_t ਸੂਚਕਾਂਕ_And_ਦੂਰੀ) {
        // assert indexAndDistance >= 0;
        ਵਾਪਸ ਸੂਚਕਾਂਕ_And_ਦੂਰੀ >> ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t get_ਉੱਤਮ_ਸੂਚਕਾਂਕ_And_ਦੂਰੀ(ਅਚਲ LSR &desired,
                                    ਅਚਲ LSR **supportedLSRs, int32_t supported_LSRs_ਲੰਮਾਈ,
                                    int32_t shifted_ਹੱਦ,
                                    ULoc_ਮੇਲ_Favor_Subtag favorSubtag,
                                    ULoc_ਮੇਲ_Direction direction) ਅਚਲ;

    bool isParadigmLSR(ਅਚਲ LSR &lsr) ਅਚਲ;

    int32_t get_Default_Script_ਦੂਰੀ() ਅਚਲ {
        ਵਾਪਸ default_Script_ਦੂਰੀ;
    }

    int32_t get_Default_Demotion_ਪ੍ਰਤੀ_Desired_Locale() ਅਚਲ {
        ਵਾਪਸ default_Demotion_ਪ੍ਰਤੀ_Desired_Locale;
    }

ਨਿਜੀ:
    // The distance is shifted left to gain some fraction bits.
    ਸਥਿਰ constexpr int32_t ਦੂਰੀ_ਖਿਸਕਾਓ = 3;
    ਸਥਿਰ constexpr int32_t ਦੂਰੀ_FRACTION_ਨਕਾਬ = 7;
    // 7 bits for 0..100
    ਸਥਿਰ constexpr int32_t ਦੂਰੀ_INT_ਖਿਸਕਾਓ = 7;
    ਸਥਿਰ constexpr int32_t ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ = ਦੂਰੀ_INT_ਖਿਸਕਾਓ + ਦੂਰੀ_ਖਿਸਕਾਓ;
    ਸਥਿਰ constexpr int32_t ਦੂਰੀ_ਨਕਾਬ = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    ਸਥਿਰ constexpr int32_t ਸੂਚਕਾਂਕ_NEG_1 = 0xfffffc00;

    Locale_ਦੂਰੀ(ਅਚਲ Locale_ਦੂਰੀ_ਅੰਕੜੇ &ਅੰਕੜੇ, ਅਚਲ LikelySubtags &likely);
    Locale_ਦੂਰੀ(ਅਚਲ Locale_ਦੂਰੀ &other) = delete;
    Locale_ਦੂਰੀ &ਸੰਚਾਲਕ=(ਅਚਲ Locale_ਦੂਰੀ &other) = delete;

    ਸਥਿਰ ਖਾਲੀ init_Locale_ਦੂਰੀ(UError_ਕੋਡ &error_ਕੋਡ);

    bool is_ਮੇਲ(ਅਚਲ LSR &desired, ਅਚਲ LSR &supported,
                 int32_t shifted_ਹੱਦ, ULoc_ਮੇਲ_Favor_Subtag favorSubtag) ਅਚਲ {
        ਅਚਲ LSR *pSupp = &supported;
        ਵਾਪਸ get_ਉੱਤਮ_ਸੂਚਕਾਂਕ_And_ਦੂਰੀ(
            desired, &pSupp, 1,
            shifted_ਹੱਦ, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    ਸਥਿਰ int32_t get_Des_Supp_Script_ਦੂਰੀ(ਬਾਈਟਾਂ_Trie &iter, uint64_t ਸ਼ੁਰੂ_State,
                                            ਅਚਲ ਅੱਖਰ *desired, ਅਚਲ ਅੱਖਰ *supported);

    ਸਥਿਰ int32_t get_Region_Partitions_ਦੂਰੀ(
        ਬਾਈਟਾਂ_Trie &iter, uint64_t ਸ਼ੁਰੂ_State,
        ਅਚਲ ਅੱਖਰ *desiredPartitions, ਅਚਲ ਅੱਖਰ *supportedPartitions,
        int32_t ਹੱਦ);

    ਸਥਿਰ int32_t get_Fallback_Region_ਦੂਰੀ(ਬਾਈਟਾਂ_Trie &iter, uint64_t ਸ਼ੁਰੂ_State);

    ਸਥਿਰ int32_t trie_ਅਗਲਾ(ਬਾਈਟਾਂ_Trie &iter, ਅਚਲ ਅੱਖਰ *s, bool want_ਮੁੱਲ);

    ਅਚਲ ਅੱਖਰ *partitionsForRegion(ਅਚਲ LSR &lsr) ਅਚਲ {
        // ill-formed region -> one non-matching string
        int32_t p_ਸੂਚਕਾਂਕ = region_ਮੰਜ਼ਿਲ_Partitions_ਸੂਚਕਾਂਕ[lsr.region_ਸੂਚਕਾਂਕ];
        ਵਾਪਸ ਹਿੱਸੇ_ਕਰੋ_Arrays[p_ਸੂਚਕਾਂਕ];
    }

    int32_t get_Default_Region_ਦੂਰੀ() ਅਚਲ {
        ਵਾਪਸ default_Region_ਦੂਰੀ;
    }

    ਅਚਲ LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    ਬਾਈਟਾਂ_Trie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    ਅਚਲ uint8_t *region_ਮੰਜ਼ਿਲ_Partitions_ਸੂਚਕਾਂਕ;
    ਅਚਲ ਅੱਖਰ **ਹਿੱਸੇ_ਕਰੋ_Arrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    ਅਚਲ LSR *paradigmLSRs;
    int32_t paradigm_LSRs_ਲੰਮਾਈ;

    int32_t default_Language_ਦੂਰੀ;
    int32_t default_Script_ਦੂਰੀ;
    int32_t default_Region_ਦੂਰੀ;
    int32_t min_Region_ਦੂਰੀ;
    int32_t default_Demotion_ਪ੍ਰਤੀ_Desired_Locale;
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  // __LOCDISTANCE_H__
