// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCLIKELYSUBTAGS_H__
#define __LOCLIKELYSUBTAGS_H__

#অন্তর্ভাৱ <utility>
#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/bytestrie.h"
#অন্তর্ভাৱ "unicode/locid.h"
#অন্তর্ভাৱ "unicode/stringpiece.h"
#অন্তর্ভাৱ "unicode/uobject.h"
#অন্তর্ভাৱ "unicode/ures.h"
#অন্তর্ভাৱ "charstrmap.h"
#অন্তর্ভাৱ "lsr.h"

U_NAMESPACE_BEGIN

সংরচনা LikelySubtagsData;

সংরচনা LocaleDistanceData {
    LocaleDistanceData() = স্ৱভাৱ;
    LocaleDistanceData(LocaleDistanceData &&data);
    ~LocaleDistanceData();

    ধ্রুৱ uint8_t *distanceTrieBytes = nullptr;
    ধ্রুৱ uint8_t *regionToPartitions = nullptr;
    ধ্রুৱ ৱর্ণ **partitions = nullptr;
    ধ্রুৱ LSR *paradigms = nullptr;
    int32_t paradigmsLength = 0;
    ধ্রুৱ int32_t *distances = nullptr;

গুপ্ত:
    LocaleDistanceData &operator=(ধ্রুৱ LocaleDistanceData &) = delete;
};

class LikelySubtags final : সার্ৱজনিক UMemory {
সার্ৱজনিক:
    ~LikelySubtags();

    স্থির constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    স্থির ধ্রুৱ LikelySubtags *getSingleton(UErrorCode &errorCode);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(ধ্রুৱ Locale &locale,
                             bool return_প্ৰবেশ_If_Unmatch,
                             UErrorCode &errorCode) ধ্রুৱ;

    /**
     * Tests whether lsr is "more likely" than other.
     * For example, fr-Latn-FR is more likely than fr-Latn-CH because
     * FR is the default region for fr-Latn.
     *
     * The likelyInfo caches lookup information between calls.
     * The return value is an updated likelyInfo value,
     * with bit 0 set if lsr is "more likely".
     * The initial value of likelyInfo must be negative.
     */
    int32_t compareLikely(ধ্রুৱ LSR &lsr, ধ্রুৱ LSR &other, int32_t likelyInfo) ধ্রুৱ;

    LSR minimizeSubtags(StringPiece language, StringPiece script, StringPiece region,
                        bool favorScript,
                        UErrorCode &errorCode) ধ্রুৱ;

    // visible for LocaleDistance
    ধ্রুৱ LocaleDistanceData& getDistanceData() ধ্রুৱ U_LIFETIME_BOUND { ঘূৰাই distanceData; }

গুপ্ত:
    LikelySubtags(LikelySubtagsData &data);
    LikelySubtags(ধ্রুৱ LikelySubtags &other) = delete;
    LikelySubtags &operator=(ধ্রুৱ LikelySubtags &other) = delete;

    স্থির শূন্য initLikelySubtags(UErrorCode &errorCode);

    LSR makeMaximizedLsr(ধ্রুৱ ৱর্ণ *language, ধ্রুৱ ৱর্ণ *script, ধ্রুৱ ৱর্ণ *region,
                         ধ্রুৱ ৱর্ণ *variant,
                         bool return_প্ৰবেশ_If_Unmatch,
                         UErrorCode &errorCode) ধ্রুৱ;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(ধ্রুৱ ৱর্ণ *language, ধ্রুৱ ৱর্ণ *script, ধ্রুৱ ৱর্ণ *region,
                 bool return_প্ৰবেশ_If_Unmatch,
                 UErrorCode &errorCode) ধ্রুৱ;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_প্ৰবেশ_If_Unmatch,
                 UErrorCode &errorCode) ধ্রুৱ;

    int32_t getLikelyIndex(ধ্রুৱ ৱর্ণ *language, ধ্রুৱ ৱর্ণ *script) ধ্রুৱ;
    bool isMacroregion(StringPiece& region, UErrorCode &errorCode) ধ্রুৱ;

    স্থির int32_t trieNext(BytesTrie &iter, ধ্রুৱ ৱর্ণ *s, int32_t i);
    স্থির int32_t trieNext(BytesTrie &iter, StringPiece s, int32_t i);

    UResourceBundle *langInfoBundle;
    // We could store the strings by value, except that if there were few enough strings,
    // moving the contents could copy it to a different array,
    // invalidating the pointers stored in the maps.
    CharString *strings;
    CharStringMap languageAliases;
    CharStringMap regionAliases;

    // The trie maps each lang+script+region (encoded in ASCII) to an index into lsrs.
    // There is also a trie value for each intermediate lang and lang+script.
    // '*' is used instead of "und", "Zzzz"/"" and "ZZ"/"".
    BytesTrie trie;
    uint64_t trieUndState;
    uint64_t trieUndZzzzState;
    int32_t defaultLsrIndex;
    uint64_t trieFirstLetterStates[26];
    ধ্রুৱ LSR *lsrs;
#যদি U_DEBUG
    int32_t lsrsLength;
#endif

    // distance/matcher data: see comment in LikelySubtagsData::load()
    LocaleDistanceData distanceData;
};

U_NAMESPACE_END

#endif  // __LOCLIKELYSUBTAGS_H__
