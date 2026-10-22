// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCLIKELYSUBTAGS_H__
#define __LOCLIKELYSUBTAGS_H__

#અન્તર્ભાવ <utility>
#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/bytestrie.h"
#અન્તર્ભાવ "unicode/locid.h"
#અન્તર્ભાવ "unicode/stringpiece.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "unicode/ures.h"
#અન્તર્ભાવ "charstrmap.h"
#અન્તર્ભાવ "lsr.h"

U_NAMESPACE_BEGIN

સંરચના LikelySubtagsData;

સંરચના LocaleDistanceData {
    LocaleDistanceData() = મૂળભૂત;
    LocaleDistanceData(LocaleDistanceData &&data);
    ~LocaleDistanceData();

    અચલ uint8_t *distanceTrieBytes = nullptr;
    અચલ uint8_t *regionToPartitions = nullptr;
    અચલ અક્ષર **partitions = nullptr;
    અચલ LSR *paradigms = nullptr;
    int32_t paradigmsLength = 0;
    અચલ int32_t *distances = nullptr;

ખાનગી:
    LocaleDistanceData &operator=(અચલ LocaleDistanceData &) = delete;
};

class LikelySubtags final : જાહેર UMemory {
જાહેર:
    ~LikelySubtags();

    સ્થિર constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    સ્થિર અચલ LikelySubtags *getSingleton(UErrorCode &errorCode);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(અચલ Locale &locale,
                             bool return_આગત_If_Unmatch,
                             UErrorCode &errorCode) અચલ;

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
    int32_t compareLikely(અચલ LSR &lsr, અચલ LSR &other, int32_t likelyInfo) અચલ;

    LSR minimizeSubtags(StringPiece language, StringPiece script, StringPiece region,
                        bool favorScript,
                        UErrorCode &errorCode) અચલ;

    // visible for LocaleDistance
    અચલ LocaleDistanceData& getDistanceData() અચલ U_LIFETIME_BOUND { પાછા distanceData; }

ખાનગી:
    LikelySubtags(LikelySubtagsData &data);
    LikelySubtags(અચલ LikelySubtags &other) = delete;
    LikelySubtags &operator=(અચલ LikelySubtags &other) = delete;

    સ્થિર શૂન્ય initLikelySubtags(UErrorCode &errorCode);

    LSR makeMaximizedLsr(અચલ અક્ષર *language, અચલ અક્ષર *script, અચલ અક્ષર *region,
                         અચલ અક્ષર *variant,
                         bool return_આગત_If_Unmatch,
                         UErrorCode &errorCode) અચલ;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(અચલ અક્ષર *language, અચલ અક્ષર *script, અચલ અક્ષર *region,
                 bool return_આગત_If_Unmatch,
                 UErrorCode &errorCode) અચલ;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_આગત_If_Unmatch,
                 UErrorCode &errorCode) અચલ;

    int32_t getLikelyIndex(અચલ અક્ષર *language, અચલ અક્ષર *script) અચલ;
    bool isMacroregion(StringPiece& region, UErrorCode &errorCode) અચલ;

    સ્થિર int32_t trieNext(BytesTrie &iter, અચલ અક્ષર *s, int32_t i);
    સ્થિર int32_t trieNext(BytesTrie &iter, StringPiece s, int32_t i);

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
    અચલ LSR *lsrs;
#જો U_DEBUG
    int32_t lsrsLength;
#endif

    // distance/matcher data: see comment in LikelySubtagsData::load()
    LocaleDistanceData distanceData;
};

U_NAMESPACE_END

#endif  // __LOCLIKELYSUBTAGS_H__
