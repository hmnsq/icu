// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __LOCLIKELYSUBTAGS_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __LOCLIKELYSUBTAGS_H__

#ಅನ್ತರ್ಭಾವ <utility>
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "charstrmap.h"
#ಅನ್ತರ್ಭಾವ "lsr.h"

U_NAMESPACE_BEGIN

ರಚನೆ Likely_Subtags_ದತ್ತಾಂಶ;

ರಚನೆ Locale_ದೂರ_ದತ್ತಾಂಶ {
    Locale_ದೂರ_ದತ್ತಾಂಶ() = ಪೂರ್ವನಿಯೋಜಿತ;
    Locale_ದೂರ_ದತ್ತಾಂಶ(Locale_ದೂರ_ದತ್ತಾಂಶ &&ದತ್ತಾಂಶ);
    ~Locale_ದೂರ_ದತ್ತಾಂಶ();

    ಸ್ಥಿರಾಂಕ uint8_t *ದೂರ_Trie_ಬೈಟುಗಳು = nullptr;
    ಸ್ಥಿರಾಂಕ uint8_t *region_ಗಮ್ಯ_Partitions = nullptr;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **partitions = nullptr;
    ಸ್ಥಿರಾಂಕ LSR *paradigms = nullptr;
    int32_t paradigms_ಉದ್ದಳತೆ = 0;
    ಸ್ಥಿರಾಂಕ int32_t *distances = nullptr;

ಖಾಸಗಿ:
    Locale_ದೂರ_ದತ್ತಾಂಶ &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Locale_ದೂರ_ದತ್ತಾಂಶ &) = delete;
};

class LikelySubtags final : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    ~LikelySubtags();

    ಸ್ಥಿರ constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ LikelySubtags *getSingleton(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(ಸ್ಥಿರಾಂಕ Locale &locale,
                             bool return_ಒಳಹರಿವು_If_Unmatch,
                             UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

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
    int32_t compareLikely(ಸ್ಥಿರಾಂಕ LSR &lsr, ಸ್ಥಿರಾಂಕ LSR &other, int32_t likelyInfo) ಸ್ಥಿರಾಂಕ;

    LSR minimizeSubtags(StringPiece language, StringPiece script, StringPiece region,
                        bool favorScript,
                        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

    // visible for LocaleDistance
    ಸ್ಥಿರಾಂಕ Locale_ದೂರ_ದತ್ತಾಂಶ& get_ದೂರ_ದತ್ತಾಂಶ() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ ದೂರ_ದತ್ತಾಂಶ; }

ಖಾಸಗಿ:
    LikelySubtags(Likely_Subtags_ದತ್ತಾಂಶ &ದತ್ತಾಂಶ);
    LikelySubtags(ಸ್ಥಿರಾಂಕ LikelySubtags &other) = delete;
    LikelySubtags &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ LikelySubtags &other) = delete;

    ಸ್ಥಿರ ಶೂನ್ಯ initLikelySubtags(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    LSR makeMaximizedLsr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *language, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *script, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *region,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variant,
                         bool return_ಒಳಹರಿವು_If_Unmatch,
                         UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *language, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *script, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *region,
                 bool return_ಒಳಹರಿವು_If_Unmatch,
                 UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_ಒಳಹರಿವು_If_Unmatch,
                 UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

    int32_t get_Likely_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *language, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *script) ಸ್ಥಿರಾಂಕ;
    bool isMacroregion(StringPiece& region, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

    ಸ್ಥಿರ int32_t trie_ಮುಂದಿನ(ಬೈಟುಗಳು_Trie &iter, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int32_t i);
    ಸ್ಥಿರ int32_t trie_ಮುಂದಿನ(ಬೈಟುಗಳು_Trie &iter, StringPiece s, int32_t i);

    UResourceBundle *langInfoBundle;
    // We could store the strings by value, except that if there were few enough strings,
    // moving the contents could copy it to a different array,
    // invalidating the pointers stored in the maps.
    CharString *ಅಕ್ಷರಸರಣಿಗಳು;
    CharStringMap languageAliases;
    CharStringMap regionAliases;

    // The trie maps each lang+script+region (encoded in ASCII) to an index into lsrs.
    // There is also a trie value for each intermediate lang and lang+script.
    // '*' is used instead of "und", "Zzzz"/"" and "ZZ"/"".
    ಬೈಟುಗಳು_Trie trie;
    uint64_t trieUndState;
    uint64_t trieUndZzzzState;
    int32_t default_Lsr_ಸೂಚ್ಯಂಕ;
    uint64_t trie_ಮೊದಲ_Letter_ಸ್ಥಿತಿಗಳು[26];
    ಸ್ಥಿರಾಂಕ LSR *lsrs;
#ಆದರೆ U_DEBUG
    int32_t lsrs_ಉದ್ದಳತೆ;
#ಆದರೆ_ಅಂತ್ಯ

    // distance/matcher data: see comment in LikelySubtagsData::load()
    Locale_ದೂರ_ದತ್ತಾಂಶ ದೂರ_ದತ್ತಾಂಶ;
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  // __LOCLIKELYSUBTAGS_H__
