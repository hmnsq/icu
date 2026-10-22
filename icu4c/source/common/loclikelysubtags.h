// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#अगर_अघोषित __LOCLIKELYSUBTAGS_H__
#घोषणा __LOCLIKELYSUBTAGS_H__

#समावेश <utility>
#समावेश "unicode/utypes.h"
#समावेश "unicode/bytestrie.h"
#समावेश "unicode/locid.h"
#समावेश "unicode/stringpiece.h"
#समावेश "unicode/uobject.h"
#समावेश "unicode/ures.h"
#समावेश "charstrmap.h"
#समावेश "lsr.h"

U_NAMESPACE_BEGIN

काष्ठा Likely_Subtags_तथ्य;

काष्ठा Locale_Distance_तथ्य {
    Locale_Distance_तथ्य() = शेष;
    Locale_Distance_तथ्य(Locale_Distance_तथ्य &&तथ्य);
    ~Locale_Distance_तथ्य();

    स्थिर uint8_t *distanceTrieBytes = nullptr;
    स्थिर uint8_t *regionToPartitions = nullptr;
    स्थिर अक्षर **partitions = nullptr;
    स्थिर LSR *paradigms = nullptr;
    int32_t paradigms_लंबाई = 0;
    स्थिर int32_t *distances = nullptr;

निजी:
    Locale_Distance_तथ्य &चालक=(स्थिर Locale_Distance_तथ्य &) = delete;
};

class LikelySubtags final : खुला UMemory {
खुला:
    ~LikelySubtags();

    अटल constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    अटल स्थिर LikelySubtags *getSingleton(UErrorCode &errorCode);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(स्थिर Locale &locale,
                             bool return_निवेश_If_Unmatch,
                             UErrorCode &errorCode) स्थिर;

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
    int32_t compareLikely(स्थिर LSR &lsr, स्थिर LSR &other, int32_t likelyInfo) स्थिर;

    LSR minimizeSubtags(StringPiece language, StringPiece script, StringPiece region,
                        bool favorScript,
                        UErrorCode &errorCode) स्थिर;

    // visible for LocaleDistance
    स्थिर Locale_Distance_तथ्य& get_Distance_तथ्य() स्थिर U_LIFETIME_BOUND { वापस distance_तथ्य; }

निजी:
    LikelySubtags(Likely_Subtags_तथ्य &तथ्य);
    LikelySubtags(स्थिर LikelySubtags &other) = delete;
    LikelySubtags &चालक=(स्थिर LikelySubtags &other) = delete;

    अटल व्योम initLikelySubtags(UErrorCode &errorCode);

    LSR makeMaximizedLsr(स्थिर अक्षर *language, स्थिर अक्षर *script, स्थिर अक्षर *region,
                         स्थिर अक्षर *variant,
                         bool return_निवेश_If_Unmatch,
                         UErrorCode &errorCode) स्थिर;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(स्थिर अक्षर *language, स्थिर अक्षर *script, स्थिर अक्षर *region,
                 bool return_निवेश_If_Unmatch,
                 UErrorCode &errorCode) स्थिर;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_निवेश_If_Unmatch,
                 UErrorCode &errorCode) स्थिर;

    int32_t get_Likely_सूचकांक(स्थिर अक्षर *language, स्थिर अक्षर *script) स्थिर;
    bool isMacroregion(StringPiece& region, UErrorCode &errorCode) स्थिर;

    अटल int32_t trie_अग्र(BytesTrie &iter, स्थिर अक्षर *s, int32_t i);
    अटल int32_t trie_अग्र(BytesTrie &iter, StringPiece s, int32_t i);

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
    int32_t default_Lsr_सूचकांक;
    uint64_t trieFirstLetterStates[26];
    स्थिर LSR *lsrs;
#अगर U_DEBUG
    int32_t lsrs_लंबाई;
#पूर्ण_अगर

    // distance/matcher data: see comment in LikelySubtagsData::load()
    Locale_Distance_तथ्य distance_तथ्य;
};

U_NAMESPACE_END

#पूर्ण_अगर  // __LOCLIKELYSUBTAGS_H__
