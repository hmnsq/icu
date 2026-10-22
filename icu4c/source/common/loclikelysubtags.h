// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCLIKELYSUBTAGS_H__
#define __LOCLIKELYSUBTAGS_H__

#അന്തര്ഭാവ <utility>
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/bytestrie.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/stringpiece.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "unicode/ures.h"
#അന്തര്ഭാവ "charstrmap.h"
#അന്തര്ഭാവ "lsr.h"

U_NAMESPACE_BEGIN

ഘടന LikelySubtagsData;

ഘടന LocaleDistanceData {
    LocaleDistanceData() = സ്ഥിരസ്ഥിതി;
    LocaleDistanceData(LocaleDistanceData &&data);
    ~LocaleDistanceData();

    സ്ഥിരം uint8_t *distanceTrieBytes = nullptr;
    സ്ഥിരം uint8_t *regionToPartitions = nullptr;
    സ്ഥിരം അക്ഷരം **partitions = nullptr;
    സ്ഥിരം LSR *paradigms = nullptr;
    int32_t paradigmsLength = 0;
    സ്ഥിരം int32_t *distances = nullptr;

സ്വകാര്യ:
    LocaleDistanceData &operator=(സ്ഥിരം LocaleDistanceData &) = delete;
};

class LikelySubtags final : പൊതു UMemory {
പൊതു:
    ~LikelySubtags();

    സ്ഥിതം constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    സ്ഥിതം സ്ഥിരം LikelySubtags *getSingleton(UErrorCode &errorCode);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(സ്ഥിരം Locale &locale,
                             bool return_നിവേശം_If_Unmatch,
                             UErrorCode &errorCode) സ്ഥിരം;

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
    int32_t compareLikely(സ്ഥിരം LSR &lsr, സ്ഥിരം LSR &other, int32_t likelyInfo) സ്ഥിരം;

    LSR minimizeSubtags(StringPiece language, StringPiece script, StringPiece region,
                        bool favorScript,
                        UErrorCode &errorCode) സ്ഥിരം;

    // visible for LocaleDistance
    സ്ഥിരം LocaleDistanceData& getDistanceData() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ distanceData; }

സ്വകാര്യ:
    LikelySubtags(LikelySubtagsData &data);
    LikelySubtags(സ്ഥിരം LikelySubtags &other) = delete;
    LikelySubtags &operator=(സ്ഥിരം LikelySubtags &other) = delete;

    സ്ഥിതം ശൂന്യം initLikelySubtags(UErrorCode &errorCode);

    LSR makeMaximizedLsr(സ്ഥിരം അക്ഷരം *language, സ്ഥിരം അക്ഷരം *script, സ്ഥിരം അക്ഷരം *region,
                         സ്ഥിരം അക്ഷരം *variant,
                         bool return_നിവേശം_If_Unmatch,
                         UErrorCode &errorCode) സ്ഥിരം;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(സ്ഥിരം അക്ഷരം *language, സ്ഥിരം അക്ഷരം *script, സ്ഥിരം അക്ഷരം *region,
                 bool return_നിവേശം_If_Unmatch,
                 UErrorCode &errorCode) സ്ഥിരം;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_നിവേശം_If_Unmatch,
                 UErrorCode &errorCode) സ്ഥിരം;

    int32_t getLikelyIndex(സ്ഥിരം അക്ഷരം *language, സ്ഥിരം അക്ഷരം *script) സ്ഥിരം;
    bool isMacroregion(StringPiece& region, UErrorCode &errorCode) സ്ഥിരം;

    സ്ഥിതം int32_t trieNext(BytesTrie &iter, സ്ഥിരം അക്ഷരം *s, int32_t i);
    സ്ഥിതം int32_t trieNext(BytesTrie &iter, StringPiece s, int32_t i);

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
    സ്ഥിരം LSR *lsrs;
#എങ്കിൽ U_DEBUG
    int32_t lsrsLength;
#endif

    // distance/matcher data: see comment in LikelySubtagsData::load()
    LocaleDistanceData distanceData;
};

U_NAMESPACE_END

#endif  // __LOCLIKELYSUBTAGS_H__
