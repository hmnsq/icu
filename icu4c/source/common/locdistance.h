// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#جيڪڏهن_وصف_نه_ٿيل __LOCDISTANCE_H__
#وصف_ڪريو __LOCDISTANCE_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/localematcher.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/uobject.h"
#ضمن "lsr.h"

U_NAMESPACE_BEGIN

بنية Locale_مفاصلو_معلومات;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class Locale_مفاصلو final : عام UMemory {
عام:
    ساكن ثابت Locale_مفاصلو *getSingleton(UError_ڪوڊ &error_ڪوڊ);

    ساكن int32_t سرڪايو_مفاصلو(int32_t مفاصلو) {
        أرجع مفاصلو << مفاصلو_سرڪايو;
    }

    ساكن int32_t get_Shifted_مفاصلو(int32_t اشاريو_And_مفاصلو) {
        أرجع اشاريو_And_مفاصلو & مفاصلو_نقاب;
    }

    ساكن مزدوج get_مفاصلو_Double(int32_t اشاريو_And_مفاصلو) {
        مزدوج shifted_مفاصلو = get_Shifted_مفاصلو(اشاريو_And_مفاصلو);
        أرجع shifted_مفاصلو / (1 << مفاصلو_سرڪايو);
    }

    ساكن int32_t get_مفاصلو_Floor(int32_t اشاريو_And_مفاصلو) {
        أرجع (اشاريو_And_مفاصلو & مفاصلو_نقاب) >> مفاصلو_سرڪايو;
    }

    ساكن int32_t get_اشاريو(int32_t اشاريو_And_مفاصلو) {
        // assert indexAndDistance >= 0;
        أرجع اشاريو_And_مفاصلو >> اشاريو_سرڪايو;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t get_بهترين_اشاريو_And_مفاصلو(ثابت LSR &desired,
                                    ثابت LSR **supportedLSRs, int32_t supported_LSRs_ڊيگهائي,
                                    int32_t shifted_حد,
                                    ULoc_ميلاپ_Favor_Subtag favorSubtag,
                                    ULoc_ميلاپ_Direction direction) ثابت;

    bool isParadigmLSR(ثابت LSR &lsr) ثابت;

    int32_t get_Default_Script_مفاصلو() ثابت {
        أرجع default_Script_مفاصلو;
    }

    int32_t get_Default_Demotion_في_Desired_Locale() ثابت {
        أرجع default_Demotion_في_Desired_Locale;
    }

خاص:
    // The distance is shifted left to gain some fraction bits.
    ساكن constexpr int32_t مفاصلو_سرڪايو = 3;
    ساكن constexpr int32_t مفاصلو_FRACTION_نقاب = 7;
    // 7 bits for 0..100
    ساكن constexpr int32_t مفاصلو_INT_سرڪايو = 7;
    ساكن constexpr int32_t اشاريو_سرڪايو = مفاصلو_INT_سرڪايو + مفاصلو_سرڪايو;
    ساكن constexpr int32_t مفاصلو_نقاب = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    ساكن constexpr int32_t اشاريو_NEG_1 = 0xfffffc00;

    Locale_مفاصلو(ثابت Locale_مفاصلو_معلومات &معلومات, ثابت LikelySubtags &likely);
    Locale_مفاصلو(ثابت Locale_مفاصلو &other) = delete;
    Locale_مفاصلو &عامل=(ثابت Locale_مفاصلو &other) = delete;

    ساكن فراغ init_Locale_مفاصلو(UError_ڪوڊ &error_ڪوڊ);

    bool is_ميلاپ(ثابت LSR &desired, ثابت LSR &supported,
                 int32_t shifted_حد, ULoc_ميلاپ_Favor_Subtag favorSubtag) ثابت {
        ثابت LSR *pSupp = &supported;
        أرجع get_بهترين_اشاريو_And_مفاصلو(
            desired, &pSupp, 1,
            shifted_حد, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    ساكن int32_t get_Des_Supp_Script_مفاصلو(بائيٽون_Trie &iter, uint64_t شروعات_State,
                                            ثابت حرف *desired, ثابت حرف *supported);

    ساكن int32_t get_Region_Partitions_مفاصلو(
        بائيٽون_Trie &iter, uint64_t شروعات_State,
        ثابت حرف *desiredPartitions, ثابت حرف *supportedPartitions,
        int32_t حد);

    ساكن int32_t get_Fallback_Region_مفاصلو(بائيٽون_Trie &iter, uint64_t شروعات_State);

    ساكن int32_t trie_ايندڙ(بائيٽون_Trie &iter, ثابت حرف *s, bool want_قدر);

    ثابت حرف *partitionsForRegion(ثابت LSR &lsr) ثابت {
        // ill-formed region -> one non-matching string
        int32_t p_اشاريو = region_منزل_Partitions_اشاريو[lsr.region_اشاريو];
        أرجع حصا_ڪريو_Arrays[p_اشاريو];
    }

    int32_t get_Default_Region_مفاصلو() ثابت {
        أرجع default_Region_مفاصلو;
    }

    ثابت LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    بائيٽون_Trie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    ثابت uint8_t *region_منزل_Partitions_اشاريو;
    ثابت حرف **حصا_ڪريو_Arrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    ثابت LSR *paradigmLSRs;
    int32_t paradigm_LSRs_ڊيگهائي;

    int32_t default_Language_مفاصلو;
    int32_t default_Script_مفاصلو;
    int32_t default_Region_مفاصلو;
    int32_t min_Region_مفاصلو;
    int32_t default_Demotion_في_Desired_Locale;
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو  // __LOCDISTANCE_H__
