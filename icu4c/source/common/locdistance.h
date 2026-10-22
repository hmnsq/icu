// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#جے_بنا_تعریف __LOCDISTANCE_H__
#تعریف_کرو __LOCDISTANCE_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/localematcher.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/uobject.h"
#ضمن "lsr.h"

U_NAMESPACE_BEGIN

بنية Locale_فاصلہ_انکڑے;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class Locale_فاصلہ final : عام UMemory {
عام:
    ساكن ثابت Locale_فاصلہ *getSingleton(UError_کوڈ &error_کوڈ);

    ساكن int32_t کھسکاؤ_فاصلہ(int32_t فاصلہ) {
        أرجع فاصلہ << فاصلہ_کھسکاؤ;
    }

    ساكن int32_t get_Shifted_فاصلہ(int32_t اشاریہ_And_فاصلہ) {
        أرجع اشاریہ_And_فاصلہ & فاصلہ_نقاب;
    }

    ساكن مزدوج get_فاصلہ_Double(int32_t اشاریہ_And_فاصلہ) {
        مزدوج shifted_فاصلہ = get_Shifted_فاصلہ(اشاریہ_And_فاصلہ);
        أرجع shifted_فاصلہ / (1 << فاصلہ_کھسکاؤ);
    }

    ساكن int32_t get_فاصلہ_Floor(int32_t اشاریہ_And_فاصلہ) {
        أرجع (اشاریہ_And_فاصلہ & فاصلہ_نقاب) >> فاصلہ_کھسکاؤ;
    }

    ساكن int32_t get_اشاریہ(int32_t اشاریہ_And_فاصلہ) {
        // assert indexAndDistance >= 0;
        أرجع اشاریہ_And_فاصلہ >> اشاریہ_کھسکاؤ;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t get_بہترین_اشاریہ_And_فاصلہ(ثابت LSR &desired,
                                    ثابت LSR **supportedLSRs, int32_t supported_LSRs_لمائی,
                                    int32_t shifted_حد,
                                    ULoc_میل_Favor_Subtag favorSubtag,
                                    ULoc_میل_Direction direction) ثابت;

    bool isParadigmLSR(ثابت LSR &lsr) ثابت;

    int32_t get_Default_Script_فاصلہ() ثابت {
        أرجع default_Script_فاصلہ;
    }

    int32_t get_Default_Demotion_فی_Desired_Locale() ثابت {
        أرجع default_Demotion_فی_Desired_Locale;
    }

خاص:
    // The distance is shifted left to gain some fraction bits.
    ساكن constexpr int32_t فاصلہ_کھسکاؤ = 3;
    ساكن constexpr int32_t فاصلہ_FRACTION_نقاب = 7;
    // 7 bits for 0..100
    ساكن constexpr int32_t فاصلہ_INT_کھسکاؤ = 7;
    ساكن constexpr int32_t اشاریہ_کھسکاؤ = فاصلہ_INT_کھسکاؤ + فاصلہ_کھسکاؤ;
    ساكن constexpr int32_t فاصلہ_نقاب = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    ساكن constexpr int32_t اشاریہ_NEG_1 = 0xfffffc00;

    Locale_فاصلہ(ثابت Locale_فاصلہ_انکڑے &انکڑے, ثابت LikelySubtags &likely);
    Locale_فاصلہ(ثابت Locale_فاصلہ &other) = delete;
    Locale_فاصلہ &عامل=(ثابت Locale_فاصلہ &other) = delete;

    ساكن فراغ init_Locale_فاصلہ(UError_کوڈ &error_کوڈ);

    bool is_میل(ثابت LSR &desired, ثابت LSR &supported,
                 int32_t shifted_حد, ULoc_میل_Favor_Subtag favorSubtag) ثابت {
        ثابت LSR *pSupp = &supported;
        أرجع get_بہترین_اشاریہ_And_فاصلہ(
            desired, &pSupp, 1,
            shifted_حد, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    ساكن int32_t get_Des_Supp_Script_فاصلہ(بائٹاں_Trie &iter, uint64_t شروع_State,
                                            ثابت حرف *desired, ثابت حرف *supported);

    ساكن int32_t get_Region_Partitions_فاصلہ(
        بائٹاں_Trie &iter, uint64_t شروع_State,
        ثابت حرف *desiredPartitions, ثابت حرف *supportedPartitions,
        int32_t حد);

    ساكن int32_t get_Fallback_Region_فاصلہ(بائٹاں_Trie &iter, uint64_t شروع_State);

    ساكن int32_t trie_اگلا(بائٹاں_Trie &iter, ثابت حرف *s, bool want_قدر);

    ثابت حرف *partitionsForRegion(ثابت LSR &lsr) ثابت {
        // ill-formed region -> one non-matching string
        int32_t p_اشاریہ = region_منزل_Partitions_اشاریہ[lsr.region_اشاریہ];
        أرجع حصے_کرو_Arrays[p_اشاریہ];
    }

    int32_t get_Default_Region_فاصلہ() ثابت {
        أرجع default_Region_فاصلہ;
    }

    ثابت LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    بائٹاں_Trie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    ثابت uint8_t *region_منزل_Partitions_اشاریہ;
    ثابت حرف **حصے_کرو_Arrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    ثابت LSR *paradigmLSRs;
    int32_t paradigm_LSRs_لمائی;

    int32_t default_Language_فاصلہ;
    int32_t default_Script_فاصلہ;
    int32_t default_Region_فاصلہ;
    int32_t min_Region_فاصلہ;
    int32_t default_Demotion_فی_Desired_Locale;
};

U_NAMESPACE_سرا

#جے_انت  // __LOCDISTANCE_H__
