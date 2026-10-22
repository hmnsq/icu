// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCLIKELYSUBTAGS_H__
#define __LOCLIKELYSUBTAGS_H__

#شامل <utility>
#شامل "unicode/utypes.h"
#شامل "unicode/bytestrie.h"
#شامل "unicode/locid.h"
#شامل "unicode/stringpiece.h"
#شامل "unicode/uobject.h"
#شامل "unicode/ures.h"
#شامل "charstrmap.h"
#شامل "lsr.h"

U_NAMESPACE_BEGIN

ساختار LikelySubtagsData;

ساختار LocaleDistanceData {
    LocaleDistanceData() = پیشفرض;
    LocaleDistanceData(LocaleDistanceData &&data);
    ~LocaleDistanceData();

    ثابت uint8_t *distanceTrieBytes = nullptr;
    ثابت uint8_t *regionToPartitions = nullptr;
    ثابت نویسه **partitions = nullptr;
    ثابت LSR *paradigms = nullptr;
    int32_t paradigmsLength = 0;
    ثابت int32_t *distances = nullptr;

خصوصی:
    LocaleDistanceData &operator=(ثابت LocaleDistanceData &) = delete;
};

class LikelySubtags final : عمومی UMemory {
عمومی:
    ~LikelySubtags();

    ایستا constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    ایستا ثابت LikelySubtags *getSingleton(UErrorCode &errorCode);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(ثابت Locale &locale,
                             bool return_درون_داد_If_Unmatch,
                             UErrorCode &errorCode) ثابت;

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
                        UErrorCode &errorCode) ثابت;

    // visible for LocaleDistance
    ثابت LocaleDistanceData& getDistanceData() ثابت U_LIFETIME_BOUND { بازگشت distanceData; }

خصوصی:
    LikelySubtags(LikelySubtagsData &data);
    LikelySubtags(ثابت LikelySubtags &other) = delete;
    LikelySubtags &operator=(ثابت LikelySubtags &other) = delete;

    ایستا تهی initLikelySubtags(UErrorCode &errorCode);

    LSR makeMaximizedLsr(ثابت نویسه *language, ثابت نویسه *script, ثابت نویسه *region,
                         ثابت نویسه *variant,
                         bool return_درون_داد_If_Unmatch,
                         UErrorCode &errorCode) ثابت;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(ثابت نویسه *language, ثابت نویسه *script, ثابت نویسه *region,
                 bool return_درون_داد_If_Unmatch,
                 UErrorCode &errorCode) ثابت;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_درون_داد_If_Unmatch,
                 UErrorCode &errorCode) ثابت;

    int32_t getLikelyIndex(ثابت نویسه *language, ثابت نویسه *script) ثابت;
    bool isMacroregion(StringPiece& region, UErrorCode &errorCode) ثابت;

    ایستا int32_t trieNext(BytesTrie &iter, ثابت نویسه *s, int32_t i);
    ایستا int32_t trieNext(BytesTrie &iter, StringPiece s, int32_t i);

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
    ثابت LSR *lsrs;
#اگر U_DEBUG
    int32_t lsrsLength;
#endif

    // distance/matcher data: see comment in LikelySubtagsData::load()
    LocaleDistanceData distanceData;
};

U_NAMESPACE_END

#endif  // __LOCLIKELYSUBTAGS_H__
