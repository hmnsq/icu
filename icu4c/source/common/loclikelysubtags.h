// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#اگر_غیر_تعریف_شدہ __LOCLIKELYSUBTAGS_H__
#تعریف_کرو __LOCLIKELYSUBTAGS_H__

#شامل <utility>
#شامل "unicode/utypes.h"
#شامل "unicode/bytestrie.h"
#شامل "unicode/locid.h"
#شامل "unicode/stringpiece.h"
#شامل "unicode/uobject.h"
#شامل "unicode/ures.h"
#شامل "charstrmap.h"
#شامل "lsr.h"

U_NAMESPACE_BEGIN

ساخت Likely_Subtags_ڈیٹا;

ساخت Locale_فاصلہ_ڈیٹا {
    Locale_فاصلہ_ڈیٹا() = طےشدہ;
    Locale_فاصلہ_ڈیٹا(Locale_فاصلہ_ڈیٹا &&ڈیٹا);
    ~Locale_فاصلہ_ڈیٹا();

    ثابت uint8_t *فاصلہ_Trie_بائٹیں = nullptr;
    ثابت uint8_t *region_مقصد_Partitions = nullptr;
    ثابت حرف **partitions = nullptr;
    ثابت LSR *paradigms = nullptr;
    int32_t paradigms_لمبائی = 0;
    ثابت int32_t *distances = nullptr;

نجی:
    Locale_فاصلہ_ڈیٹا &عامل=(ثابت Locale_فاصلہ_ڈیٹا &) = delete;
};

class LikelySubtags final : عوامی UMemory {
عوامی:
    ~LikelySubtags();

    ساکن constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    ساکن ثابت LikelySubtags *getSingleton(UError_رمز &error_رمز);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(ثابت Locale &locale,
                             bool return_داخلہ_If_Unmatch,
                             UError_رمز &error_رمز) ثابت;

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
    int32_t compareLikely(ثابت LSR &lsr, ثابت LSR &other, int32_t likelyInfo) ثابت;

    LSR minimizeSubtags(StringPiece language, StringPiece script, StringPiece region,
                        bool favorScript,
                        UError_رمز &error_رمز) ثابت;

    // visible for LocaleDistance
    ثابت Locale_فاصلہ_ڈیٹا& get_فاصلہ_ڈیٹا() ثابت U_LIFETIME_BOUND { واپس فاصلہ_ڈیٹا; }

نجی:
    LikelySubtags(Likely_Subtags_ڈیٹا &ڈیٹا);
    LikelySubtags(ثابت LikelySubtags &other) = delete;
    LikelySubtags &عامل=(ثابت LikelySubtags &other) = delete;

    ساکن خالی initLikelySubtags(UError_رمز &error_رمز);

    LSR makeMaximizedLsr(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                         ثابت حرف *variant,
                         bool return_داخلہ_If_Unmatch,
                         UError_رمز &error_رمز) ثابت;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                 bool return_داخلہ_If_Unmatch,
                 UError_رمز &error_رمز) ثابت;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_داخلہ_If_Unmatch,
                 UError_رمز &error_رمز) ثابت;

    int32_t get_Likely_اشاریہ(ثابت حرف *language, ثابت حرف *script) ثابت;
    bool isMacroregion(StringPiece& region, UError_رمز &error_رمز) ثابت;

    ساکن int32_t trie_اگلا(بائٹیں_Trie &iter, ثابت حرف *s, int32_t i);
    ساکن int32_t trie_اگلا(بائٹیں_Trie &iter, StringPiece s, int32_t i);

    UResourceBundle *langInfoBundle;
    // We could store the strings by value, except that if there were few enough strings,
    // moving the contents could copy it to a different array,
    // invalidating the pointers stored in the maps.
    CharString *لڑیاں;
    CharStringMap languageAliases;
    CharStringMap regionAliases;

    // The trie maps each lang+script+region (encoded in ASCII) to an index into lsrs.
    // There is also a trie value for each intermediate lang and lang+script.
    // '*' is used instead of "und", "Zzzz"/"" and "ZZ"/"".
    بائٹیں_Trie trie;
    uint64_t trieUndState;
    uint64_t trieUndZzzzState;
    int32_t default_Lsr_اشاریہ;
    uint64_t trie_پہلا_Letter_حالتیں[26];
    ثابت LSR *lsrs;
#اگر U_DEBUG
    int32_t lsrs_لمبائی;
#اگر_ختم

    // distance/matcher data: see comment in LikelySubtagsData::load()
    Locale_فاصلہ_ڈیٹا فاصلہ_ڈیٹا;
};

U_NAMESPACE_انتہا

#اگر_ختم  // __LOCLIKELYSUBTAGS_H__
