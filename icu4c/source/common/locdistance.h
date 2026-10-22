// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#اگر_تعریف_نشده __LOCDISTANCE_H__
#تعریف_کن __LOCDISTANCE_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/localematcher.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/uobject.h"
#ضمن "lsr.h"

U_NAMESPACE_BEGIN

بنية Locale_فاصله_داده;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class Locale_فاصله final : عام UMemory {
عام:
    ساكن ثابت Locale_فاصله *getSingleton(UError_کد &error_کد);

    ساكن int32_t لغزش_فاصله(int32_t فاصله) {
        أرجع فاصله << فاصله_لغزش;
    }

    ساكن int32_t get_Shifted_فاصله(int32_t شاخص_And_فاصله) {
        أرجع شاخص_And_فاصله & فاصله_ماسک;
    }

    ساكن مزدوج get_فاصله_Double(int32_t شاخص_And_فاصله) {
        مزدوج shifted_فاصله = get_Shifted_فاصله(شاخص_And_فاصله);
        أرجع shifted_فاصله / (1 << فاصله_لغزش);
    }

    ساكن int32_t get_فاصله_Floor(int32_t شاخص_And_فاصله) {
        أرجع (شاخص_And_فاصله & فاصله_ماسک) >> فاصله_لغزش;
    }

    ساكن int32_t get_شاخص(int32_t شاخص_And_فاصله) {
        // assert indexAndDistance >= 0;
        أرجع شاخص_And_فاصله >> شاخص_لغزش;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t get_بهترین_شاخص_And_فاصله(ثابت LSR &desired,
                                    ثابت LSR **supportedLSRs, int32_t supported_LSRs_درازی,
                                    int32_t shifted_آستانه,
                                    ULoc_تطابق_Favor_Subtag favorSubtag,
                                    ULoc_تطابق_Direction direction) ثابت;

    bool isParadigmLSR(ثابت LSR &lsr) ثابت;

    int32_t get_Default_Script_فاصله() ثابت {
        أرجع default_Script_فاصله;
    }

    int32_t get_Default_Demotion_در_هر_Desired_Locale() ثابت {
        أرجع default_Demotion_در_هر_Desired_Locale;
    }

خاص:
    // The distance is shifted left to gain some fraction bits.
    ساكن constexpr int32_t فاصله_لغزش = 3;
    ساكن constexpr int32_t فاصله_FRACTION_ماسک = 7;
    // 7 bits for 0..100
    ساكن constexpr int32_t فاصله_INT_لغزش = 7;
    ساكن constexpr int32_t شاخص_لغزش = فاصله_INT_لغزش + فاصله_لغزش;
    ساكن constexpr int32_t فاصله_ماسک = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    ساكن constexpr int32_t شاخص_NEG_1 = 0xfffffc00;

    Locale_فاصله(ثابت Locale_فاصله_داده &داده, ثابت LikelySubtags &likely);
    Locale_فاصله(ثابت Locale_فاصله &other) = delete;
    Locale_فاصله &عملگر=(ثابت Locale_فاصله &other) = delete;

    ساكن فراغ init_Locale_فاصله(UError_کد &error_کد);

    bool is_تطابق(ثابت LSR &desired, ثابت LSR &supported,
                 int32_t shifted_آستانه, ULoc_تطابق_Favor_Subtag favorSubtag) ثابت {
        ثابت LSR *pSupp = &supported;
        أرجع get_بهترین_شاخص_And_فاصله(
            desired, &pSupp, 1,
            shifted_آستانه, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    ساكن int32_t get_Des_Supp_Script_فاصله(بایتها_Trie &iter, uint64_t شروع_State,
                                            ثابت حرف *desired, ثابت حرف *supported);

    ساكن int32_t get_Region_Partitions_فاصله(
        بایتها_Trie &iter, uint64_t شروع_State,
        ثابت حرف *desiredPartitions, ثابت حرف *supportedPartitions,
        int32_t آستانه);

    ساكن int32_t get_Fallback_Region_فاصله(بایتها_Trie &iter, uint64_t شروع_State);

    ساكن int32_t trie_بعدی(بایتها_Trie &iter, ثابت حرف *s, bool want_مقدار);

    ثابت حرف *partitionsForRegion(ثابت LSR &lsr) ثابت {
        // ill-formed region -> one non-matching string
        int32_t p_شاخص = region_مقصد_Partitions_شاخص[lsr.region_شاخص];
        أرجع افراز_Arrays[p_شاخص];
    }

    int32_t get_Default_Region_فاصله() ثابت {
        أرجع default_Region_فاصله;
    }

    ثابت LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    بایتها_Trie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    ثابت uint8_t *region_مقصد_Partitions_شاخص;
    ثابت حرف **افراز_Arrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    ثابت LSR *paradigmLSRs;
    int32_t paradigm_LSRs_درازی;

    int32_t default_Language_فاصله;
    int32_t default_Script_فاصله;
    int32_t default_Region_فاصله;
    int32_t min_Region_فاصله;
    int32_t default_Demotion_در_هر_Desired_Locale;
};

U_NAMESPACE_انتها

#پایان_اگر  // __LOCDISTANCE_H__
