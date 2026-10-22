// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCLIKELYSUBTAGS_H__
#define __LOCLIKELYSUBTAGS_H__

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

بنية LikelySubtagsData;

بنية LocaleDistanceData {
    LocaleDistanceData() = افتراضي;
    LocaleDistanceData(LocaleDistanceData &&data);
    ~LocaleDistanceData();

    ثابت uint8_t *distanceTrieBytes = nullptr;
    ثابت uint8_t *regionToPartitions = nullptr;
    ثابت حرف **partitions = nullptr;
    ثابت LSR *paradigms = nullptr;
    int32_t paradigmsLength = 0;
    ثابت int32_t *distances = nullptr;

خاص:
    LocaleDistanceData &operator=(ثابت LocaleDistanceData &) = delete;
};

class LikelySubtags final : عام UMemory {
عام:
    ~LikelySubtags();

    ساكن constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    ساكن ثابت LikelySubtags *getSingleton(UErrorCode &errorCode);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(ثابت Locale &locale,
                             bool return_ماسوقن_If_Unmatch,
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
    ثابت LocaleDistanceData& getDistanceData() ثابت U_LIFETIME_BOUND { أرجع distanceData; }

خاص:
    LikelySubtags(LikelySubtagsData &data);
    LikelySubtags(ثابت LikelySubtags &other) = delete;
    LikelySubtags &operator=(ثابت LikelySubtags &other) = delete;

    ساكن فراغ initLikelySubtags(UErrorCode &errorCode);

    LSR makeMaximizedLsr(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                         ثابت حرف *variant,
                         bool return_ماسوقن_If_Unmatch,
                         UErrorCode &errorCode) ثابت;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                 bool return_ماسوقن_If_Unmatch,
                 UErrorCode &errorCode) ثابت;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_ماسوقن_If_Unmatch,
                 UErrorCode &errorCode) ثابت;

    int32_t getLikelyIndex(ثابت حرف *language, ثابت حرف *script) ثابت;
    bool isMacroregion(StringPiece& region, UErrorCode &errorCode) ثابت;

    ساكن int32_t trieNext(BytesTrie &iter, ثابت حرف *s, int32_t i);
    ساكن int32_t trieNext(BytesTrie &iter, StringPiece s, int32_t i);

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
#إذا U_DEBUG
    int32_t lsrsLength;
#endif

    // distance/matcher data: see comment in LikelySubtagsData::load()
    LocaleDistanceData distanceData;
};

U_NAMESPACE_END

#endif  // __LOCLIKELYSUBTAGS_H__
