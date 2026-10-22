// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#ifndef __LOCLIKELYSUBTAGS_H__
#define __LOCLIKELYSUBTAGS_H__

#அந்தர்பாவ <utility>
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/bytestrie.h"
#அந்தர்பாவ "unicode/locid.h"
#அந்தர்பாவ "unicode/stringpiece.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "unicode/ures.h"
#அந்தர்பாவ "charstrmap.h"
#அந்தர்பாவ "lsr.h"

U_NAMESPACE_BEGIN

கட்டமைப்பு LikelySubtagsData;

கட்டமைப்பு LocaleDistanceData {
    LocaleDistanceData() = இயல்பு;
    LocaleDistanceData(LocaleDistanceData &&data);
    ~LocaleDistanceData();

    மாறிலி uint8_t *distanceTrieBytes = nullptr;
    மாறிலி uint8_t *regionToPartitions = nullptr;
    மாறிலி எழுத்து **partitions = nullptr;
    மாறிலி LSR *paradigms = nullptr;
    int32_t paradigmsLength = 0;
    மாறிலி int32_t *distances = nullptr;

தனிப்பட்ட:
    LocaleDistanceData &operator=(மாறிலி LocaleDistanceData &) = delete;
};

class LikelySubtags final : பொது UMemory {
பொது:
    ~LikelySubtags();

    நிலையான constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    நிலையான மாறிலி LikelySubtags *getSingleton(UErrorCode &errorCode);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(மாறிலி Locale &locale,
                             bool return_உள்ளீடு_If_Unmatch,
                             UErrorCode &errorCode) மாறிலி;

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
    int32_t compareLikely(மாறிலி LSR &lsr, மாறிலி LSR &other, int32_t likelyInfo) மாறிலி;

    LSR minimizeSubtags(StringPiece language, StringPiece script, StringPiece region,
                        bool favorScript,
                        UErrorCode &errorCode) மாறிலி;

    // visible for LocaleDistance
    மாறிலி LocaleDistanceData& getDistanceData() மாறிலி U_LIFETIME_BOUND { திரும்பு distanceData; }

தனிப்பட்ட:
    LikelySubtags(LikelySubtagsData &data);
    LikelySubtags(மாறிலி LikelySubtags &other) = delete;
    LikelySubtags &operator=(மாறிலி LikelySubtags &other) = delete;

    நிலையான வெறுமை initLikelySubtags(UErrorCode &errorCode);

    LSR makeMaximizedLsr(மாறிலி எழுத்து *language, மாறிலி எழுத்து *script, மாறிலி எழுத்து *region,
                         மாறிலி எழுத்து *variant,
                         bool return_உள்ளீடு_If_Unmatch,
                         UErrorCode &errorCode) மாறிலி;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(மாறிலி எழுத்து *language, மாறிலி எழுத்து *script, மாறிலி எழுத்து *region,
                 bool return_உள்ளீடு_If_Unmatch,
                 UErrorCode &errorCode) மாறிலி;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_உள்ளீடு_If_Unmatch,
                 UErrorCode &errorCode) மாறிலி;

    int32_t getLikelyIndex(மாறிலி எழுத்து *language, மாறிலி எழுத்து *script) மாறிலி;
    bool isMacroregion(StringPiece& region, UErrorCode &errorCode) மாறிலி;

    நிலையான int32_t trieNext(BytesTrie &iter, மாறிலி எழுத்து *s, int32_t i);
    நிலையான int32_t trieNext(BytesTrie &iter, StringPiece s, int32_t i);

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
    மாறிலி LSR *lsrs;
#எனில் U_DEBUG
    int32_t lsrsLength;
#endif

    // distance/matcher data: see comment in LikelySubtagsData::load()
    LocaleDistanceData distanceData;
};

U_NAMESPACE_END

#endif  // __LOCLIKELYSUBTAGS_H__
