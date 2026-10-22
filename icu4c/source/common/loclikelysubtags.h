// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#جيڪڏهن_وصف_نه_ٿيل __LOCLIKELYSUBTAGS_H__
#وصف_ڪريو __LOCLIKELYSUBTAGS_H__

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

بنية Likely_Subtags_معلومات;

بنية Locale_مفاصلو_معلومات {
    Locale_مفاصلو_معلومات() = افتراضي;
    Locale_مفاصلو_معلومات(Locale_مفاصلو_معلومات &&معلومات);
    ~Locale_مفاصلو_معلومات();

    ثابت uint8_t *مفاصلو_Trie_بائيٽون = nullptr;
    ثابت uint8_t *region_منزل_Partitions = nullptr;
    ثابت حرف **partitions = nullptr;
    ثابت LSR *paradigms = nullptr;
    int32_t paradigms_ڊيگهائي = 0;
    ثابت int32_t *distances = nullptr;

خاص:
    Locale_مفاصلو_معلومات &عامل=(ثابت Locale_مفاصلو_معلومات &) = delete;
};

class LikelySubtags final : عام UMemory {
عام:
    ~LikelySubtags();

    ساكن constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    ساكن ثابت LikelySubtags *getSingleton(UError_ڪوڊ &error_ڪوڊ);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(ثابت Locale &locale,
                             bool return_داخلا_If_Unmatch,
                             UError_ڪوڊ &error_ڪوڊ) ثابت;

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
                        UError_ڪوڊ &error_ڪوڊ) ثابت;

    // visible for LocaleDistance
    ثابت Locale_مفاصلو_معلومات& get_مفاصلو_معلومات() ثابت U_LIFETIME_BOUND { أرجع مفاصلو_معلومات; }

خاص:
    LikelySubtags(Likely_Subtags_معلومات &معلومات);
    LikelySubtags(ثابت LikelySubtags &other) = delete;
    LikelySubtags &عامل=(ثابت LikelySubtags &other) = delete;

    ساكن فراغ initLikelySubtags(UError_ڪوڊ &error_ڪوڊ);

    LSR makeMaximizedLsr(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                         ثابت حرف *variant,
                         bool return_داخلا_If_Unmatch,
                         UError_ڪوڊ &error_ڪوڊ) ثابت;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                 bool return_داخلا_If_Unmatch,
                 UError_ڪوڊ &error_ڪوڊ) ثابت;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_داخلا_If_Unmatch,
                 UError_ڪوڊ &error_ڪوڊ) ثابت;

    int32_t get_Likely_اشاريو(ثابت حرف *language, ثابت حرف *script) ثابت;
    bool isMacroregion(StringPiece& region, UError_ڪوڊ &error_ڪوڊ) ثابت;

    ساكن int32_t trie_ايندڙ(بائيٽون_Trie &iter, ثابت حرف *s, int32_t i);
    ساكن int32_t trie_ايندڙ(بائيٽون_Trie &iter, StringPiece s, int32_t i);

    UResourceBundle *langInfoBundle;
    // We could store the strings by value, except that if there were few enough strings,
    // moving the contents could copy it to a different array,
    // invalidating the pointers stored in the maps.
    CharString *لڙيون;
    CharStringMap languageAliases;
    CharStringMap regionAliases;

    // The trie maps each lang+script+region (encoded in ASCII) to an index into lsrs.
    // There is also a trie value for each intermediate lang and lang+script.
    // '*' is used instead of "und", "Zzzz"/"" and "ZZ"/"".
    بائيٽون_Trie trie;
    uint64_t trieUndState;
    uint64_t trieUndZzzzState;
    int32_t default_Lsr_اشاريو;
    uint64_t trie_پهريون_Letter_حالتون[26];
    ثابت LSR *lsrs;
#إذا U_DEBUG
    int32_t lsrs_ڊيگهائي;
#جيڪڏهن_پورو

    // distance/matcher data: see comment in LikelySubtagsData::load()
    Locale_مفاصلو_معلومات مفاصلو_معلومات;
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو  // __LOCLIKELYSUBTAGS_H__
