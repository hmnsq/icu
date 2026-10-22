// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCLIKELYSUBTAGS_H__
#define __LOCLIKELYSUBTAGS_H__

#अन्तर्भाव <utility>
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestrie.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "charstrmap.h"
#अन्तर्भाव "lsr.h"

U_NAMESPACE_BEGIN

संरचना LikelySubtagsData;

संरचना LocaleDistanceData {
    LocaleDistanceData() = पूर्वनिर्धारित;
    LocaleDistanceData(LocaleDistanceData &&data);
    ~LocaleDistanceData();

    स्थिरांक uint8_t *distanceTrieBytes = nullptr;
    स्थिरांक uint8_t *regionToPartitions = nullptr;
    स्थिरांक अक्षर **partitions = nullptr;
    स्थिरांक LSR *paradigms = nullptr;
    int32_t paradigmsLength = 0;
    स्थिरांक int32_t *distances = nullptr;

खाजगी:
    LocaleDistanceData &चालक=(स्थिरांक LocaleDistanceData &) = delete;
};

class LikelySubtags final : सार्वजनिक UMemory {
सार्वजनिक:
    ~LikelySubtags();

    स्थिर constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    स्थिर स्थिरांक LikelySubtags *getSingleton(UErrorCode &errorCode);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(स्थिरांक Locale &locale,
                             bool return_आदान_If_Unmatch,
                             UErrorCode &errorCode) स्थिरांक;

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
    int32_t compareLikely(स्थिरांक LSR &lsr, स्थिरांक LSR &other, int32_t likelyInfo) स्थिरांक;

    LSR minimizeSubtags(StringPiece language, StringPiece script, StringPiece region,
                        bool favorScript,
                        UErrorCode &errorCode) स्थिरांक;

    // visible for LocaleDistance
    स्थिरांक LocaleDistanceData& getDistanceData() स्थिरांक U_LIFETIME_BOUND { परत distanceData; }

खाजगी:
    LikelySubtags(LikelySubtagsData &data);
    LikelySubtags(स्थिरांक LikelySubtags &other) = delete;
    LikelySubtags &चालक=(स्थिरांक LikelySubtags &other) = delete;

    स्थिर शून्य initLikelySubtags(UErrorCode &errorCode);

    LSR makeMaximizedLsr(स्थिरांक अक्षर *language, स्थिरांक अक्षर *script, स्थिरांक अक्षर *region,
                         स्थिरांक अक्षर *variant,
                         bool return_आदान_If_Unmatch,
                         UErrorCode &errorCode) स्थिरांक;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(स्थिरांक अक्षर *language, स्थिरांक अक्षर *script, स्थिरांक अक्षर *region,
                 bool return_आदान_If_Unmatch,
                 UErrorCode &errorCode) स्थिरांक;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_आदान_If_Unmatch,
                 UErrorCode &errorCode) स्थिरांक;

    int32_t getLikelyIndex(स्थिरांक अक्षर *language, स्थिरांक अक्षर *script) स्थिरांक;
    bool isMacroregion(StringPiece& region, UErrorCode &errorCode) स्थिरांक;

    स्थिर int32_t trieNext(BytesTrie &iter, स्थिरांक अक्षर *s, int32_t i);
    स्थिर int32_t trieNext(BytesTrie &iter, StringPiece s, int32_t i);

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
    स्थिरांक LSR *lsrs;
#जर U_DEBUG
    int32_t lsrsLength;
#endif

    // distance/matcher data: see comment in LikelySubtagsData::load()
    LocaleDistanceData distanceData;
};

U_NAMESPACE_END

#endif  // __LOCLIKELYSUBTAGS_H__
