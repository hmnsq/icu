// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCLIKELYSUBTAGS_H__
#define __LOCLIKELYSUBTAGS_H__

#ଅନ୍ତର୍ଭାଵ <utility>
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/bytestrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringpiece.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"
#ଅନ୍ତର୍ଭାଵ "charstrmap.h"
#ଅନ୍ତର୍ଭାଵ "lsr.h"

U_NAMESPACE_BEGIN

ସଂରଚନା LikelySubtagsData;

ସଂରଚନା LocaleDistanceData {
    LocaleDistanceData() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
    LocaleDistanceData(LocaleDistanceData &&data);
    ~LocaleDistanceData();

    ଅଚଳ uint8_t *distanceTrieBytes = nullptr;
    ଅଚଳ uint8_t *regionToPartitions = nullptr;
    ଅଚଳ ଅକ୍ଷର **partitions = nullptr;
    ଅଚଳ LSR *paradigms = nullptr;
    int32_t paradigmsLength = 0;
    ଅଚଳ int32_t *distances = nullptr;

ବ୍ୟକ୍ତିଗତ:
    LocaleDistanceData &operator=(ଅଚଳ LocaleDistanceData &) = delete;
};

class LikelySubtags final : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
    ~LikelySubtags();

    ସ୍ଥିର constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    ସ୍ଥିର ଅଚଳ LikelySubtags *getSingleton(UErrorCode &errorCode);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(ଅଚଳ Locale &locale,
                             bool return_ନିବେଶ_If_Unmatch,
                             UErrorCode &errorCode) ଅଚଳ;

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
    int32_t compareLikely(ଅଚଳ LSR &lsr, ଅଚଳ LSR &other, int32_t likelyInfo) ଅଚଳ;

    LSR minimizeSubtags(StringPiece language, StringPiece script, StringPiece region,
                        bool favorScript,
                        UErrorCode &errorCode) ଅଚଳ;

    // visible for LocaleDistance
    ଅଚଳ LocaleDistanceData& getDistanceData() ଅଚଳ U_LIFETIME_BOUND { ଫେରନ୍ତୁ distanceData; }

ବ୍ୟକ୍ତିଗତ:
    LikelySubtags(LikelySubtagsData &data);
    LikelySubtags(ଅଚଳ LikelySubtags &other) = delete;
    LikelySubtags &operator=(ଅଚଳ LikelySubtags &other) = delete;

    ସ୍ଥିର ଶୂନ୍ୟ initLikelySubtags(UErrorCode &errorCode);

    LSR makeMaximizedLsr(ଅଚଳ ଅକ୍ଷର *language, ଅଚଳ ଅକ୍ଷର *script, ଅଚଳ ଅକ୍ଷର *region,
                         ଅଚଳ ଅକ୍ଷର *variant,
                         bool return_ନିବେଶ_If_Unmatch,
                         UErrorCode &errorCode) ଅଚଳ;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(ଅଚଳ ଅକ୍ଷର *language, ଅଚଳ ଅକ୍ଷର *script, ଅଚଳ ଅକ୍ଷର *region,
                 bool return_ନିବେଶ_If_Unmatch,
                 UErrorCode &errorCode) ଅଚଳ;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_ନିବେଶ_If_Unmatch,
                 UErrorCode &errorCode) ଅଚଳ;

    int32_t getLikelyIndex(ଅଚଳ ଅକ୍ଷର *language, ଅଚଳ ଅକ୍ଷର *script) ଅଚଳ;
    bool isMacroregion(StringPiece& region, UErrorCode &errorCode) ଅଚଳ;

    ସ୍ଥିର int32_t trieNext(BytesTrie &iter, ଅଚଳ ଅକ୍ଷର *s, int32_t i);
    ସ୍ଥିର int32_t trieNext(BytesTrie &iter, StringPiece s, int32_t i);

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
    ଅଚଳ LSR *lsrs;
#ଯଦି U_DEBUG
    int32_t lsrsLength;
#endif

    // distance/matcher data: see comment in LikelySubtagsData::load()
    LocaleDistanceData distanceData;
};

U_NAMESPACE_END

#endif  // __LOCLIKELYSUBTAGS_H__
