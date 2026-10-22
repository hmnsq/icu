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
    LocaleDistanceData() = स्वभाव;
    LocaleDistanceData(LocaleDistanceData &&data);
    ~LocaleDistanceData();

    ध्रुव uint8_t *distanceTrieBytes = nullptr;
    ध्रुव uint8_t *regionToPartitions = nullptr;
    ध्रुव वर्ण **partitions = nullptr;
    ध्रुव LSR *paradigms = nullptr;
    int32_t paradigmsLength = 0;
    ध्रुव int32_t *distances = nullptr;

गुप्त:
    LocaleDistanceData &चालक=(ध्रुव LocaleDistanceData &) = delete;
};

class LikelySubtags final : सार्वजनिक UMemory {
सार्वजनिक:
    ~LikelySubtags();

    स्थिर constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    स्थिर ध्रुव LikelySubtags *getSingleton(UErrorCode &errorCode);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(ध्रुव Locale &locale,
                             bool return_निवेश_If_Unmatch,
                             UErrorCode &errorCode) ध्रुव;

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
    int32_t compareLikely(ध्रुव LSR &lsr, ध्रुव LSR &other, int32_t likelyInfo) ध्रुव;

    LSR minimizeSubtags(StringPiece language, StringPiece script, StringPiece region,
                        bool favorScript,
                        UErrorCode &errorCode) ध्रुव;

    // visible for LocaleDistance
    ध्रुव LocaleDistanceData& getDistanceData() ध्रुव U_LIFETIME_BOUND { प्रत्यागम distanceData; }

गुप्त:
    LikelySubtags(LikelySubtagsData &data);
    LikelySubtags(ध्रुव LikelySubtags &other) = delete;
    LikelySubtags &चालक=(ध्रुव LikelySubtags &other) = delete;

    स्थिर शून्य initLikelySubtags(UErrorCode &errorCode);

    LSR makeMaximizedLsr(ध्रुव वर्ण *language, ध्रुव वर्ण *script, ध्रुव वर्ण *region,
                         ध्रुव वर्ण *variant,
                         bool return_निवेश_If_Unmatch,
                         UErrorCode &errorCode) ध्रुव;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(ध्रुव वर्ण *language, ध्रुव वर्ण *script, ध्रुव वर्ण *region,
                 bool return_निवेश_If_Unmatch,
                 UErrorCode &errorCode) ध्रुव;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_निवेश_If_Unmatch,
                 UErrorCode &errorCode) ध्रुव;

    int32_t getLikelyIndex(ध्रुव वर्ण *language, ध्रुव वर्ण *script) ध्रुव;
    bool isMacroregion(StringPiece& region, UErrorCode &errorCode) ध्रुव;

    स्थिर int32_t trieNext(BytesTrie &iter, ध्रुव वर्ण *s, int32_t i);
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
    ध्रुव LSR *lsrs;
#यदि U_DEBUG
    int32_t lsrsLength;
#endif

    // distance/matcher data: see comment in LikelySubtagsData::load()
    LocaleDistanceData distanceData;
};

U_NAMESPACE_END

#endif  // __LOCLIKELYSUBTAGS_H__
