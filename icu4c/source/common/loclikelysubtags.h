// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#اگر_تعریف_نشده __LOCLIKELYSUBTAGS_H__
#تعریف_کن __LOCLIKELYSUBTAGS_H__

#ضمن <utility>
#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/ures.h"
#ضمن "charstrmap.h"
#ضمن "lsr.h"

U_NAMESPACE_BEGIN

بنية Likely_Subtags_داده;

بنية Locale_فاصله_داده {
    Locale_فاصله_داده() = افتراضي;
    Locale_فاصله_داده(Locale_فاصله_داده &&داده);
    ~Locale_فاصله_داده();

    ثابت uint8_t *فاصله_Trie_بایتها = nullptr;
    ثابت uint8_t *region_مقصد_Partitions = nullptr;
    ثابت حرف **partitions = nullptr;
    ثابت LSR *paradigms = nullptr;
    int32_t paradigms_درازی = 0;
    ثابت int32_t *distances = nullptr;

خاص:
    Locale_فاصله_داده &عملگر=(ثابت Locale_فاصله_داده &) = delete;
};

class LikelySubtags final : عام UMemory {
عام:
    ~LikelySubtags();

    ساكن constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    ساكن ثابت LikelySubtags *getSingleton(UError_کد &error_کد);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(ثابت Locale &locale,
                             bool return_درون_داد_If_Unmatch,
                             UError_کد &error_کد) ثابت;

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
                        UError_کد &error_کد) ثابت;

    // visible for LocaleDistance
    ثابت Locale_فاصله_داده& get_فاصله_داده() ثابت U_LIFETIME_BOUND { أرجع فاصله_داده; }

خاص:
    LikelySubtags(Likely_Subtags_داده &داده);
    LikelySubtags(ثابت LikelySubtags &other) = delete;
    LikelySubtags &عملگر=(ثابت LikelySubtags &other) = delete;

    ساكن فراغ initLikelySubtags(UError_کد &error_کد);

    LSR makeMaximizedLsr(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                         ثابت حرف *variant,
                         bool return_درون_داد_If_Unmatch,
                         UError_کد &error_کد) ثابت;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                 bool return_درون_داد_If_Unmatch,
                 UError_کد &error_کد) ثابت;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_درون_داد_If_Unmatch,
                 UError_کد &error_کد) ثابت;

    int32_t get_Likely_شاخص(ثابت حرف *language, ثابت حرف *script) ثابت;
    bool isMacroregion(StringPiece& region, UError_کد &error_کد) ثابت;

    ساكن int32_t trie_بعدی(بایتها_Trie &iter, ثابت حرف *s, int32_t i);
    ساكن int32_t trie_بعدی(بایتها_Trie &iter, StringPiece s, int32_t i);

    UResourceBundle *langInfoBundle;
    // We could store the strings by value, except that if there were few enough strings,
    // moving the contents could copy it to a different array,
    // invalidating the pointers stored in the maps.
    CharString *رشتهها;
    CharStringMap languageAliases;
    CharStringMap regionAliases;

    // The trie maps each lang+script+region (encoded in ASCII) to an index into lsrs.
    // There is also a trie value for each intermediate lang and lang+script.
    // '*' is used instead of "und", "Zzzz"/"" and "ZZ"/"".
    بایتها_Trie trie;
    uint64_t trieUndState;
    uint64_t trieUndZzzzState;
    int32_t default_Lsr_شاخص;
    uint64_t trie_اولی_Letter_حالتها[26];
    ثابت LSR *lsrs;
#إذا U_DEBUG
    int32_t lsrs_درازی;
#پایان_اگر

    // distance/matcher data: see comment in LikelySubtagsData::load()
    Locale_فاصله_داده فاصله_داده;
};

U_NAMESPACE_انتها

#پایان_اگر  // __LOCLIKELYSUBTAGS_H__
