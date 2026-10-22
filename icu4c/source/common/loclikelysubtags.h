// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __LOCLIKELYSUBTAGS_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __LOCLIKELYSUBTAGS_H__

#ਅਨ੍ਤਰ੍ਭਾਵ <utility>
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringpiece.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstrmap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lsr.h"

U_NAMESPACE_BEGIN

ਬਣਤਰ Likely_Subtags_ਅੰਕੜੇ;

ਬਣਤਰ Locale_ਦੂਰੀ_ਅੰਕੜੇ {
    Locale_ਦੂਰੀ_ਅੰਕੜੇ() = ਮੂਲ;
    Locale_ਦੂਰੀ_ਅੰਕੜੇ(Locale_ਦੂਰੀ_ਅੰਕੜੇ &&ਅੰਕੜੇ);
    ~Locale_ਦੂਰੀ_ਅੰਕੜੇ();

    ਅਚਲ uint8_t *ਦੂਰੀ_Trie_ਬਾਈਟਾਂ = nullptr;
    ਅਚਲ uint8_t *region_ਮੰਜ਼ਿਲ_Partitions = nullptr;
    ਅਚਲ ਅੱਖਰ **partitions = nullptr;
    ਅਚਲ LSR *paradigms = nullptr;
    int32_t paradigms_ਲੰਮਾਈ = 0;
    ਅਚਲ int32_t *distances = nullptr;

ਨਿਜੀ:
    Locale_ਦੂਰੀ_ਅੰਕੜੇ &ਸੰਚਾਲਕ=(ਅਚਲ Locale_ਦੂਰੀ_ਅੰਕੜੇ &) = delete;
};

class LikelySubtags final : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    ~LikelySubtags();

    ਸਥਿਰ constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    ਸਥਿਰ ਅਚਲ LikelySubtags *getSingleton(UError_ਕੋਡ &error_ਕੋਡ);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(ਅਚਲ Locale &locale,
                             bool return_ਨਿਵੇਸ਼_If_Unmatch,
                             UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ;

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
    int32_t compareLikely(ਅਚਲ LSR &lsr, ਅਚਲ LSR &other, int32_t likelyInfo) ਅਚਲ;

    LSR minimizeSubtags(StringPiece language, StringPiece script, StringPiece region,
                        bool favorScript,
                        UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ;

    // visible for LocaleDistance
    ਅਚਲ Locale_ਦੂਰੀ_ਅੰਕੜੇ& get_ਦੂਰੀ_ਅੰਕੜੇ() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ ਦੂਰੀ_ਅੰਕੜੇ; }

ਨਿਜੀ:
    LikelySubtags(Likely_Subtags_ਅੰਕੜੇ &ਅੰਕੜੇ);
    LikelySubtags(ਅਚਲ LikelySubtags &other) = delete;
    LikelySubtags &ਸੰਚਾਲਕ=(ਅਚਲ LikelySubtags &other) = delete;

    ਸਥਿਰ ਖਾਲੀ initLikelySubtags(UError_ਕੋਡ &error_ਕੋਡ);

    LSR makeMaximizedLsr(ਅਚਲ ਅੱਖਰ *language, ਅਚਲ ਅੱਖਰ *script, ਅਚਲ ਅੱਖਰ *region,
                         ਅਚਲ ਅੱਖਰ *variant,
                         bool return_ਨਿਵੇਸ਼_If_Unmatch,
                         UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(ਅਚਲ ਅੱਖਰ *language, ਅਚਲ ਅੱਖਰ *script, ਅਚਲ ਅੱਖਰ *region,
                 bool return_ਨਿਵੇਸ਼_If_Unmatch,
                 UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_ਨਿਵੇਸ਼_If_Unmatch,
                 UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ;

    int32_t get_Likely_ਸੂਚਕਾਂਕ(ਅਚਲ ਅੱਖਰ *language, ਅਚਲ ਅੱਖਰ *script) ਅਚਲ;
    bool isMacroregion(StringPiece& region, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ;

    ਸਥਿਰ int32_t trie_ਅਗਲਾ(ਬਾਈਟਾਂ_Trie &iter, ਅਚਲ ਅੱਖਰ *s, int32_t i);
    ਸਥਿਰ int32_t trie_ਅਗਲਾ(ਬਾਈਟਾਂ_Trie &iter, StringPiece s, int32_t i);

    UResourceBundle *langInfoBundle;
    // We could store the strings by value, except that if there were few enough strings,
    // moving the contents could copy it to a different array,
    // invalidating the pointers stored in the maps.
    CharString *ਲੜੀਆਂ;
    CharStringMap languageAliases;
    CharStringMap regionAliases;

    // The trie maps each lang+script+region (encoded in ASCII) to an index into lsrs.
    // There is also a trie value for each intermediate lang and lang+script.
    // '*' is used instead of "und", "Zzzz"/"" and "ZZ"/"".
    ਬਾਈਟਾਂ_Trie trie;
    uint64_t trieUndState;
    uint64_t trieUndZzzzState;
    int32_t default_Lsr_ਸੂਚਕਾਂਕ;
    uint64_t trie_ਪਹਿਲਾ_Letter_ਹਾਲਤਾਂ[26];
    ਅਚਲ LSR *lsrs;
#ਜੇ U_DEBUG
    int32_t lsrs_ਲੰਮਾਈ;
#ਜੇ_ਅੰਤ

    // distance/matcher data: see comment in LikelySubtagsData::load()
    Locale_ਦੂਰੀ_ਅੰਕੜੇ ਦੂਰੀ_ਅੰਕੜੇ;
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  // __LOCLIKELYSUBTAGS_H__
