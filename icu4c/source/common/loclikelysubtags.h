// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCLIKELYSUBTAGS_H__
#define __LOCLIKELYSUBTAGS_H__

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

ಸಂರಚನಾ LikelySubtagsData;

ಸಂರಚನಾ LocaleDistanceData {
    LocaleDistanceData() = ಸ್ವಭಾವ;
    LocaleDistanceData(LocaleDistanceData &&data);
    ~LocaleDistanceData();

    ಧ್ರುವ uint8_t *distanceTrieBytes = nullptr;
    ಧ್ರುವ uint8_t *regionToPartitions = nullptr;
    ಧ್ರುವ ವರ್ಣ **partitions = nullptr;
    ಧ್ರುವ LSR *paradigms = nullptr;
    int32_t paradigmsLength = 0;
    ಧ್ರುವ int32_t *distances = nullptr;

ಗುಪ್ತ:
    LocaleDistanceData &operator=(ಧ್ರುವ LocaleDistanceData &) = delete;
};

class LikelySubtags final : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    ~LikelySubtags();

    ಸ್ಥಿರ constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    ಸ್ಥಿರ ಧ್ರುವ LikelySubtags *getSingleton(UErrorCode &errorCode);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(ಧ್ರುವ Locale &locale,
                             bool return_ಒಳಹರಿವು_If_Unmatch,
                             UErrorCode &errorCode) ಧ್ರುವ;

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
    int32_t compareLikely(ಧ್ರುವ LSR &lsr, ಧ್ರುವ LSR &other, int32_t likelyInfo) ಧ್ರುವ;

    LSR minimizeSubtags(StringPiece language, StringPiece script, StringPiece region,
                        bool favorScript,
                        UErrorCode &errorCode) ಧ್ರುವ;

    // visible for LocaleDistance
    ಧ್ರುವ LocaleDistanceData& getDistanceData() ಧ್ರುವ U_LIFETIME_BOUND { ಪ್ರತ್ಯಾಗಮ distanceData; }

ಗುಪ್ತ:
    LikelySubtags(LikelySubtagsData &data);
    LikelySubtags(ಧ್ರುವ LikelySubtags &other) = delete;
    LikelySubtags &operator=(ಧ್ರುವ LikelySubtags &other) = delete;

    ಸ್ಥಿರ ಶೂನ್ಯ initLikelySubtags(UErrorCode &errorCode);

    LSR makeMaximizedLsr(ಧ್ರುವ ವರ್ಣ *language, ಧ್ರುವ ವರ್ಣ *script, ಧ್ರುವ ವರ್ಣ *region,
                         ಧ್ರುವ ವರ್ಣ *variant,
                         bool return_ಒಳಹರಿವು_If_Unmatch,
                         UErrorCode &errorCode) ಧ್ರುವ;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(ಧ್ರುವ ವರ್ಣ *language, ಧ್ರುವ ವರ್ಣ *script, ಧ್ರುವ ವರ್ಣ *region,
                 bool return_ಒಳಹರಿವು_If_Unmatch,
                 UErrorCode &errorCode) ಧ್ರುವ;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_ಒಳಹರಿವು_If_Unmatch,
                 UErrorCode &errorCode) ಧ್ರುವ;

    int32_t getLikelyIndex(ಧ್ರುವ ವರ್ಣ *language, ಧ್ರುವ ವರ್ಣ *script) ಧ್ರುವ;
    bool isMacroregion(StringPiece& region, UErrorCode &errorCode) ಧ್ರುವ;

    ಸ್ಥಿರ int32_t trieNext(BytesTrie &iter, ಧ್ರುವ ವರ್ಣ *s, int32_t i);
    ಸ್ಥಿರ int32_t trieNext(BytesTrie &iter, StringPiece s, int32_t i);

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
    ಧ್ರುವ LSR *lsrs;
#ಯದಿ U_DEBUG
    int32_t lsrsLength;
#endif

    // distance/matcher data: see comment in LikelySubtagsData::load()
    LocaleDistanceData distanceData;
};

U_NAMESPACE_END

#endif  // __LOCLIKELYSUBTAGS_H__
