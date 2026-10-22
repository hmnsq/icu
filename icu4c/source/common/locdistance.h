// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#که_نه_تعریف_شوی __LOCDISTANCE_H__
#تعریف_کړه __LOCDISTANCE_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/localematcher.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/uobject.h"
#ضمن "lsr.h"

U_NAMESPACE_BEGIN

بنية Locale_واټن_معلومات;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class Locale_واټن final : عام UMemory {
عام:
    ساكن ثابت Locale_واټن *getSingleton(UError_کوډ &error_کوډ);

    ساكن int32_t ښويدنه_واټن(int32_t واټن) {
        أرجع واټن << واټن_ښويدنه;
    }

    ساكن int32_t get_Shifted_واټن(int32_t شاخص_And_واټن) {
        أرجع شاخص_And_واټن & واټن_ماسک;
    }

    ساكن مزدوج get_واټن_Double(int32_t شاخص_And_واټن) {
        مزدوج shifted_واټن = get_Shifted_واټن(شاخص_And_واټن);
        أرجع shifted_واټن / (1 << واټن_ښويدنه);
    }

    ساكن int32_t get_واټن_Floor(int32_t شاخص_And_واټن) {
        أرجع (شاخص_And_واټن & واټن_ماسک) >> واټن_ښويدنه;
    }

    ساكن int32_t get_شاخص(int32_t شاخص_And_واټن) {
        // assert indexAndDistance >= 0;
        أرجع شاخص_And_واټن >> شاخص_ښويدنه;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t get_غوره_شاخص_And_واټن(ثابت LSR &desired,
                                    ثابت LSR **supportedLSRs, int32_t supported_LSRs_اوږدتیا,
                                    int32_t shifted_پوله,
                                    ULoc_سمون_Favor_Subtag favorSubtag,
                                    ULoc_سمون_Direction direction) ثابت;

    bool isParadigmLSR(ثابت LSR &lsr) ثابت;

    int32_t get_Default_Script_واټن() ثابت {
        أرجع default_Script_واټن;
    }

    int32_t get_Default_Demotion_فی_Desired_Locale() ثابت {
        أرجع default_Demotion_فی_Desired_Locale;
    }

خاص:
    // The distance is shifted left to gain some fraction bits.
    ساكن constexpr int32_t واټن_ښويدنه = 3;
    ساكن constexpr int32_t واټن_FRACTION_ماسک = 7;
    // 7 bits for 0..100
    ساكن constexpr int32_t واټن_INT_ښويدنه = 7;
    ساكن constexpr int32_t شاخص_ښويدنه = واټن_INT_ښويدنه + واټن_ښويدنه;
    ساكن constexpr int32_t واټن_ماسک = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    ساكن constexpr int32_t شاخص_NEG_1 = 0xfffffc00;

    Locale_واټن(ثابت Locale_واټن_معلومات &معلومات, ثابت LikelySubtags &likely);
    Locale_واټن(ثابت Locale_واټن &other) = delete;
    Locale_واټن &چلوونکی=(ثابت Locale_واټن &other) = delete;

    ساكن فراغ init_Locale_واټن(UError_کوډ &error_کوډ);

    bool is_سمون(ثابت LSR &desired, ثابت LSR &supported,
                 int32_t shifted_پوله, ULoc_سمون_Favor_Subtag favorSubtag) ثابت {
        ثابت LSR *pSupp = &supported;
        أرجع get_غوره_شاخص_And_واټن(
            desired, &pSupp, 1,
            shifted_پوله, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    ساكن int32_t get_Des_Supp_Script_واټن(بایټونه_Trie &iter, uint64_t پیل_State,
                                            ثابت حرف *desired, ثابت حرف *supported);

    ساكن int32_t get_Region_Partitions_واټن(
        بایټونه_Trie &iter, uint64_t پیل_State,
        ثابت حرف *desiredPartitions, ثابت حرف *supportedPartitions,
        int32_t پوله);

    ساكن int32_t get_Fallback_Region_واټن(بایټونه_Trie &iter, uint64_t پیل_State);

    ساكن int32_t trie_بل(بایټونه_Trie &iter, ثابت حرف *s, bool want_ارزښت);

    ثابت حرف *partitionsForRegion(ثابت LSR &lsr) ثابت {
        // ill-formed region -> one non-matching string
        int32_t p_شاخص = region_منزل_Partitions_شاخص[lsr.region_شاخص];
        أرجع برخه_کول_Arrays[p_شاخص];
    }

    int32_t get_Default_Region_واټن() ثابت {
        أرجع default_Region_واټن;
    }

    ثابت LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    بایټونه_Trie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    ثابت uint8_t *region_منزل_Partitions_شاخص;
    ثابت حرف **برخه_کول_Arrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    ثابت LSR *paradigmLSRs;
    int32_t paradigm_LSRs_اوږدتیا;

    int32_t default_Language_واټن;
    int32_t default_Script_واټن;
    int32_t default_Region_واټن;
    int32_t min_Region_واټن;
    int32_t default_Demotion_فی_Desired_Locale;
};

U_NAMESPACE_پای_ټکی

#د_که_پای  // __LOCDISTANCE_H__
