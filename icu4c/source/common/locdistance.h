// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#اگر_غیر_تعریف_شدہ __LOCDISTANCE_H__
#تعریف_کرو __LOCDISTANCE_H__

#شامل "unicode/utypes.h"
#شامل "unicode/bytestrie.h"
#شامل "unicode/localematcher.h"
#شامل "unicode/locid.h"
#شامل "unicode/uobject.h"
#شامل "lsr.h"

U_NAMESPACE_BEGIN

ساخت Locale_فاصلہ_ڈیٹا;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class Locale_فاصلہ final : عوامی UMemory {
عوامی:
    ساکن ثابت Locale_فاصلہ *getSingleton(UError_رمز &error_رمز);

    ساکن int32_t سرکاؤ_فاصلہ(int32_t فاصلہ) {
        واپس فاصلہ << فاصلہ_سرکاؤ;
    }

    ساکن int32_t get_Shifted_فاصلہ(int32_t اشاریہ_And_فاصلہ) {
        واپس اشاریہ_And_فاصلہ & فاصلہ_نقاب;
    }

    ساکن دوہرا get_فاصلہ_Double(int32_t اشاریہ_And_فاصلہ) {
        دوہرا shifted_فاصلہ = get_Shifted_فاصلہ(اشاریہ_And_فاصلہ);
        واپس shifted_فاصلہ / (1 << فاصلہ_سرکاؤ);
    }

    ساکن int32_t get_فاصلہ_Floor(int32_t اشاریہ_And_فاصلہ) {
        واپس (اشاریہ_And_فاصلہ & فاصلہ_نقاب) >> فاصلہ_سرکاؤ;
    }

    ساکن int32_t get_اشاریہ(int32_t اشاریہ_And_فاصلہ) {
        // assert indexAndDistance >= 0;
        واپس اشاریہ_And_فاصلہ >> اشاریہ_سرکاؤ;
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
                                    ثابت LSR **supportedLSRs, int32_t supported_LSRs_لمبائی,
                                    int32_t shifted_حد,
                                    ULoc_مطابقت_Favor_Subtag favorSubtag,
                                    ULoc_مطابقت_Direction direction) ثابت;

    bool isParadigmLSR(ثابت LSR &lsr) ثابت;

    int32_t get_Default_Script_فاصلہ() ثابت {
        واپس default_Script_فاصلہ;
    }

    int32_t get_Default_Demotion_فی_Desired_Locale() ثابت {
        واپس default_Demotion_فی_Desired_Locale;
    }

نجی:
    // The distance is shifted left to gain some fraction bits.
    ساکن constexpr int32_t فاصلہ_سرکاؤ = 3;
    ساکن constexpr int32_t فاصلہ_FRACTION_نقاب = 7;
    // 7 bits for 0..100
    ساکن constexpr int32_t فاصلہ_INT_سرکاؤ = 7;
    ساکن constexpr int32_t اشاریہ_سرکاؤ = فاصلہ_INT_سرکاؤ + فاصلہ_سرکاؤ;
    ساکن constexpr int32_t فاصلہ_نقاب = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    ساکن constexpr int32_t اشاریہ_NEG_1 = 0xfffffc00;

    Locale_فاصلہ(ثابت Locale_فاصلہ_ڈیٹا &ڈیٹا, ثابت LikelySubtags &likely);
    Locale_فاصلہ(ثابت Locale_فاصلہ &other) = delete;
    Locale_فاصلہ &عامل=(ثابت Locale_فاصلہ &other) = delete;

    ساکن خالی init_Locale_فاصلہ(UError_رمز &error_رمز);

    bool is_مطابقت(ثابت LSR &desired, ثابت LSR &supported,
                 int32_t shifted_حد, ULoc_مطابقت_Favor_Subtag favorSubtag) ثابت {
        ثابت LSR *pSupp = &supported;
        واپس get_بہترین_اشاریہ_And_فاصلہ(
            desired, &pSupp, 1,
            shifted_حد, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    ساکن int32_t get_Des_Supp_Script_فاصلہ(بائٹیں_Trie &iter, uint64_t آغاز_State,
                                            ثابت حرف *desired, ثابت حرف *supported);

    ساکن int32_t get_Region_Partitions_فاصلہ(
        بائٹیں_Trie &iter, uint64_t آغاز_State,
        ثابت حرف *desiredPartitions, ثابت حرف *supportedPartitions,
        int32_t حد);

    ساکن int32_t get_Fallback_Region_فاصلہ(بائٹیں_Trie &iter, uint64_t آغاز_State);

    ساکن int32_t trie_اگلا(بائٹیں_Trie &iter, ثابت حرف *s, bool want_قدر);

    ثابت حرف *partitionsForRegion(ثابت LSR &lsr) ثابت {
        // ill-formed region -> one non-matching string
        int32_t p_اشاریہ = region_مقصد_Partitions_اشاریہ[lsr.region_اشاریہ];
        واپس تقسیم_Arrays[p_اشاریہ];
    }

    int32_t get_Default_Region_فاصلہ() ثابت {
        واپس default_Region_فاصلہ;
    }

    ثابت LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    بائٹیں_Trie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    ثابت uint8_t *region_مقصد_Partitions_اشاریہ;
    ثابت حرف **تقسیم_Arrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    ثابت LSR *paradigmLSRs;
    int32_t paradigm_LSRs_لمبائی;

    int32_t default_Language_فاصلہ;
    int32_t default_Script_فاصلہ;
    int32_t default_Region_فاصلہ;
    int32_t min_Region_فاصلہ;
    int32_t default_Demotion_فی_Desired_Locale;
};

U_NAMESPACE_انتہا

#اگر_ختم  // __LOCDISTANCE_H__
