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

কাঠামো Likely_Subtags_তথ্য;

কাঠামো Locale_Distance_তথ্য {
    Locale_Distance_তথ্য() = পূর্বনির্ধারিত;
    Locale_Distance_তথ্য(Locale_Distance_তথ্য &&তথ্য);
    ~Locale_Distance_তথ্য();

    ধ্রুবক uint8_t *distanceTrieBytes = nullptr;
    ধ্রুবক uint8_t *regionToPartitions = nullptr;
    ধ্রুবক অক্ষর **partitions = nullptr;
    ধ্রুবক LSR *paradigms = nullptr;
    int32_t paradigms_দৈর্ঘ্য = 0;
    ধ্রুবক int32_t *distances = nullptr;

ব্যক্তিগত:
    Locale_Distance_তথ্য &operator=(ধ্রুবক Locale_Distance_তথ্য &) = delete;
};

class LikelySubtags final : সর্বজনীন UMemory {
সর্বজনীন:
    ~LikelySubtags();

    স্থির constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    স্থির ধ্রুবক LikelySubtags *getSingleton(UErrorCode &errorCode);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(ধ্রুবক Locale &locale,
                             bool return_প্রবেশ_If_Unmatch,
                             UErrorCode &errorCode) ধ্রুবক;

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
    int32_t compareLikely(ধ্রুবক LSR &lsr, ধ্রুবক LSR &other, int32_t likelyInfo) ধ্রুবক;

    LSR minimizeSubtags(StringPiece language, StringPiece script, StringPiece region,
                        bool favorScript,
                        UErrorCode &errorCode) ধ্রুবক;

    // visible for LocaleDistance
    ধ্রুবক Locale_Distance_তথ্য& get_Distance_তথ্য() ধ্রুবক U_LIFETIME_BOUND { ফেরত distance_তথ্য; }

ব্যক্তিগত:
    LikelySubtags(Likely_Subtags_তথ্য &তথ্য);
    LikelySubtags(ধ্রুবক LikelySubtags &other) = delete;
    LikelySubtags &operator=(ধ্রুবক LikelySubtags &other) = delete;

    স্থির শূন্য initLikelySubtags(UErrorCode &errorCode);

    LSR makeMaximizedLsr(ধ্রুবক অক্ষর *language, ধ্রুবক অক্ষর *script, ধ্রুবক অক্ষর *region,
                         ধ্রুবক অক্ষর *variant,
                         bool return_প্রবেশ_If_Unmatch,
                         UErrorCode &errorCode) ধ্রুবক;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(ধ্রুবক অক্ষর *language, ধ্রুবক অক্ষর *script, ধ্রুবক অক্ষর *region,
                 bool return_প্রবেশ_If_Unmatch,
                 UErrorCode &errorCode) ধ্রুবক;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_প্রবেশ_If_Unmatch,
                 UErrorCode &errorCode) ধ্রুবক;

    int32_t get_Likely_সূচক(ধ্রুবক অক্ষর *language, ধ্রুবক অক্ষর *script) ধ্রুবক;
    bool isMacroregion(StringPiece& region, UErrorCode &errorCode) ধ্রুবক;

    স্থির int32_t trie_পরবর্তী(BytesTrie &iter, ধ্রুবক অক্ষর *s, int32_t i);
    স্থির int32_t trie_পরবর্তী(BytesTrie &iter, StringPiece s, int32_t i);

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
    int32_t default_Lsr_সূচক;
    uint64_t trieFirstLetterStates[26];
    ধ্রুবক LSR *lsrs;
#যদি U_DEBUG
    int32_t lsrs_দৈর্ঘ্য;
#endif

    // distance/matcher data: see comment in LikelySubtagsData::load()
    Locale_Distance_তথ্য distance_তথ্য;
};

U_NAMESPACE_END

#endif  // __LOCLIKELYSUBTAGS_H__
