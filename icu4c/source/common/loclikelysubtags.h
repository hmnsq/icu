// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCLIKELYSUBTAGS_H__
#define __LOCLIKELYSUBTAGS_H__

#অন্তর্ভাব <utility>
#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/bytestrie.h"
#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/stringpiece.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "unicode/ures.h"
#অন্তর্ভাব "charstrmap.h"
#অন্তর্ভাব "lsr.h"

U_NAMESPACE_BEGIN

সংরচনা LikelySubtagsData;

সংরচনা LocaleDistanceData {
    LocaleDistanceData() = স্বভাব;
    LocaleDistanceData(LocaleDistanceData &&data);
    ~LocaleDistanceData();

    ধ্রুব uint8_t *distanceTrieBytes = nullptr;
    ধ্রুব uint8_t *regionToPartitions = nullptr;
    ধ্রুব বর্ণ **partitions = nullptr;
    ধ্রুব LSR *paradigms = nullptr;
    int32_t paradigmsLength = 0;
    ধ্রুব int32_t *distances = nullptr;

গুপ্ত:
    LocaleDistanceData &operator=(ধ্রুব LocaleDistanceData &) = delete;
};

class LikelySubtags final : সার্বজনিক UMemory {
সার্বজনিক:
    ~LikelySubtags();

    স্থির constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    স্থির ধ্রুব LikelySubtags *getSingleton(UErrorCode &errorCode);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(ধ্রুব Locale &locale,
                             bool return_প্রবেশ_If_Unmatch,
                             UErrorCode &errorCode) ধ্রুব;

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
    int32_t compareLikely(ধ্রুব LSR &lsr, ধ্রুব LSR &other, int32_t likelyInfo) ধ্রুব;

    LSR minimizeSubtags(StringPiece language, StringPiece script, StringPiece region,
                        bool favorScript,
                        UErrorCode &errorCode) ধ্রুব;

    // visible for LocaleDistance
    ধ্রুব LocaleDistanceData& getDistanceData() ধ্রুব U_LIFETIME_BOUND { প্রত্যাগম distanceData; }

গুপ্ত:
    LikelySubtags(LikelySubtagsData &data);
    LikelySubtags(ধ্রুব LikelySubtags &other) = delete;
    LikelySubtags &operator=(ধ্রুব LikelySubtags &other) = delete;

    স্থির শূন্য initLikelySubtags(UErrorCode &errorCode);

    LSR makeMaximizedLsr(ধ্রুব বর্ণ *language, ধ্রুব বর্ণ *script, ধ্রুব বর্ণ *region,
                         ধ্রুব বর্ণ *variant,
                         bool return_প্রবেশ_If_Unmatch,
                         UErrorCode &errorCode) ধ্রুব;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(ধ্রুব বর্ণ *language, ধ্রুব বর্ণ *script, ধ্রুব বর্ণ *region,
                 bool return_প্রবেশ_If_Unmatch,
                 UErrorCode &errorCode) ধ্রুব;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_প্রবেশ_If_Unmatch,
                 UErrorCode &errorCode) ধ্রুব;

    int32_t getLikelyIndex(ধ্রুব বর্ণ *language, ধ্রুব বর্ণ *script) ধ্রুব;
    bool isMacroregion(StringPiece& region, UErrorCode &errorCode) ধ্রুব;

    স্থির int32_t trieNext(BytesTrie &iter, ধ্রুব বর্ণ *s, int32_t i);
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
    ধ্রুব LSR *lsrs;
#যদি U_DEBUG
    int32_t lsrsLength;
#endif

    // distance/matcher data: see comment in LikelySubtagsData::load()
    LocaleDistanceData distanceData;
};

U_NAMESPACE_END

#endif  // __LOCLIKELYSUBTAGS_H__
