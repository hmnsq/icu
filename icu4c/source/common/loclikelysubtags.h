// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#جے_بنا_تعریف __LOCLIKELYSUBTAGS_H__
#تعریف_کرو __LOCLIKELYSUBTAGS_H__

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

بنية Likely_Subtags_انکڑے;

بنية Locale_فاصلہ_انکڑے {
    Locale_فاصلہ_انکڑے() = افتراضي;
    Locale_فاصلہ_انکڑے(Locale_فاصلہ_انکڑے &&انکڑے);
    ~Locale_فاصلہ_انکڑے();

    ثابت uint8_t *فاصلہ_Trie_بائٹاں = nullptr;
    ثابت uint8_t *region_منزل_Partitions = nullptr;
    ثابت حرف **partitions = nullptr;
    ثابت LSR *paradigms = nullptr;
    int32_t paradigms_لمائی = 0;
    ثابت int32_t *distances = nullptr;

خاص:
    Locale_فاصلہ_انکڑے &عامل=(ثابت Locale_فاصلہ_انکڑے &) = delete;
};

class LikelySubtags final : عام UMemory {
عام:
    ~LikelySubtags();

    ساكن constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    ساكن ثابت LikelySubtags *getSingleton(UError_کوڈ &error_کوڈ);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(ثابت Locale &locale,
                             bool return_داخلہ_If_Unmatch,
                             UError_کوڈ &error_کوڈ) ثابت;

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
                        UError_کوڈ &error_کوڈ) ثابت;

    // visible for LocaleDistance
    ثابت Locale_فاصلہ_انکڑے& get_فاصلہ_انکڑے() ثابت U_LIFETIME_BOUND { أرجع فاصلہ_انکڑے; }

خاص:
    LikelySubtags(Likely_Subtags_انکڑے &انکڑے);
    LikelySubtags(ثابت LikelySubtags &other) = delete;
    LikelySubtags &عامل=(ثابت LikelySubtags &other) = delete;

    ساكن فراغ initLikelySubtags(UError_کوڈ &error_کوڈ);

    LSR makeMaximizedLsr(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                         ثابت حرف *variant,
                         bool return_داخلہ_If_Unmatch,
                         UError_کوڈ &error_کوڈ) ثابت;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                 bool return_داخلہ_If_Unmatch,
                 UError_کوڈ &error_کوڈ) ثابت;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_داخلہ_If_Unmatch,
                 UError_کوڈ &error_کوڈ) ثابت;

    int32_t get_Likely_اشاریہ(ثابت حرف *language, ثابت حرف *script) ثابت;
    bool isMacroregion(StringPiece& region, UError_کوڈ &error_کوڈ) ثابت;

    ساكن int32_t trie_اگلا(بائٹاں_Trie &iter, ثابت حرف *s, int32_t i);
    ساكن int32_t trie_اگلا(بائٹاں_Trie &iter, StringPiece s, int32_t i);

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
    بائٹاں_Trie trie;
    uint64_t trieUndState;
    uint64_t trieUndZzzzState;
    int32_t default_Lsr_اشاریہ;
    uint64_t trie_پہلا_Letter_حالتاں[26];
    ثابت LSR *lsrs;
#إذا U_DEBUG
    int32_t lsrs_لمائی;
#جے_انت

    // distance/matcher data: see comment in LikelySubtagsData::load()
    Locale_فاصلہ_انکڑے فاصلہ_انکڑے;
};

U_NAMESPACE_سرا

#جے_انت  // __LOCLIKELYSUBTAGS_H__
