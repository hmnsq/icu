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

    अचर uint8_t *distanceTrieBytes = nullptr;
    अचर uint8_t *regionToPartitions = nullptr;
    अचर वर्ण **partitions = nullptr;
    अचर LSR *paradigms = nullptr;
    int32_t paradigmsLength = 0;
    अचर int32_t *distances = nullptr;

गुप्त:
    LocaleDistanceData &चालक=(अचर LocaleDistanceData &) = delete;
};

class LikelySubtags final : सार्वजनिक UMemory {
सार्वजनिक:
    ~LikelySubtags();

    स्थिर constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    स्थिर अचर LikelySubtags *getSingleton(UErrorCode &errorCode);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(अचर Locale &locale,
                             bool return_आगत_If_Unmatch,
                             UErrorCode &errorCode) अचर;

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
    int32_t compareLikely(अचर LSR &lsr, अचर LSR &other, int32_t likelyInfo) अचर;

    LSR minimizeSubtags(StringPiece language, StringPiece script, StringPiece region,
                        bool favorScript,
                        UErrorCode &errorCode) अचर;

    // visible for LocaleDistance
    अचर LocaleDistanceData& getDistanceData() अचर U_LIFETIME_BOUND { फर्काऊ distanceData; }

गुप्त:
    LikelySubtags(LikelySubtagsData &data);
    LikelySubtags(अचर LikelySubtags &other) = delete;
    LikelySubtags &चालक=(अचर LikelySubtags &other) = delete;

    स्थिर शून्य initLikelySubtags(UErrorCode &errorCode);

    LSR makeMaximizedLsr(अचर वर्ण *language, अचर वर्ण *script, अचर वर्ण *region,
                         अचर वर्ण *variant,
                         bool return_आगत_If_Unmatch,
                         UErrorCode &errorCode) अचर;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(अचर वर्ण *language, अचर वर्ण *script, अचर वर्ण *region,
                 bool return_आगत_If_Unmatch,
                 UErrorCode &errorCode) अचर;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_आगत_If_Unmatch,
                 UErrorCode &errorCode) अचर;

    int32_t getLikelyIndex(अचर वर्ण *language, अचर वर्ण *script) अचर;
    bool isMacroregion(StringPiece& region, UErrorCode &errorCode) अचर;

    स्थिर int32_t trieNext(BytesTrie &iter, अचर वर्ण *s, int32_t i);
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
    अचर LSR *lsrs;
#यदि U_DEBUG
    int32_t lsrsLength;
#endif

    // distance/matcher data: see comment in LikelySubtagsData::load()
    LocaleDistanceData distanceData;
};

U_NAMESPACE_END

#endif  // __LOCLIKELYSUBTAGS_H__
