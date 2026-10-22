// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#यदि_अपरिभाषितम् __LOCLIKELYSUBTAGS_H__
#परिभाषय __LOCLIKELYSUBTAGS_H__

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

संरचना Likely_Subtags_दत्त;

संरचना Locale_Distance_दत्त {
    Locale_Distance_दत्त() = स्वभाव;
    Locale_Distance_दत्त(Locale_Distance_दत्त &&दत्त);
    ~Locale_Distance_दत्त();

    ध्रुव uint8_t *distance_Trie_अष्टकानि = nullptr;
    ध्रुव uint8_t *region_गन्तव्यम्_Partitions = nullptr;
    ध्रुव वर्ण **partitions = nullptr;
    ध्रुव LSR *paradigms = nullptr;
    int32_t paradigms_दीर्घता = 0;
    ध्रुव int32_t *distances = nullptr;

गुप्त:
    Locale_Distance_दत्त &संक्रियकः=(ध्रुव Locale_Distance_दत्त &) = delete;
};

class LikelySubtags final : सार्वजनिक UMemory {
सार्वजनिक:
    ~LikelySubtags();

    स्थिर constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    स्थिर ध्रुव LikelySubtags *getSingleton(UError_संहिता &error_संहिता);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(ध्रुव Locale &locale,
                             bool return_निवेशः_If_Unmatch,
                             UError_संहिता &error_संहिता) ध्रुव;

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
                        UError_संहिता &error_संहिता) ध्रुव;

    // visible for LocaleDistance
    ध्रुव Locale_Distance_दत्त& get_Distance_दत्त() ध्रुव U_LIFETIME_BOUND { प्रत्यागम distance_दत्त; }

गुप्त:
    LikelySubtags(Likely_Subtags_दत्त &दत्त);
    LikelySubtags(ध्रुव LikelySubtags &other) = delete;
    LikelySubtags &संक्रियकः=(ध्रुव LikelySubtags &other) = delete;

    स्थिर शून्य initLikelySubtags(UError_संहिता &error_संहिता);

    LSR makeMaximizedLsr(ध्रुव वर्ण *language, ध्रुव वर्ण *script, ध्रुव वर्ण *region,
                         ध्रुव वर्ण *variant,
                         bool return_निवेशः_If_Unmatch,
                         UError_संहिता &error_संहिता) ध्रुव;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(ध्रुव वर्ण *language, ध्रुव वर्ण *script, ध्रुव वर्ण *region,
                 bool return_निवेशः_If_Unmatch,
                 UError_संहिता &error_संहिता) ध्रुव;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_निवेशः_If_Unmatch,
                 UError_संहिता &error_संहिता) ध्रुव;

    int32_t get_Likely_सूचकांक(ध्रुव वर्ण *language, ध्रुव वर्ण *script) ध्रुव;
    bool isMacroregion(StringPiece& region, UError_संहिता &error_संहिता) ध्रुव;

    स्थिर int32_t trie_अग्र(अष्टकानि_Trie &iter, ध्रुव वर्ण *s, int32_t i);
    स्थिर int32_t trie_अग्र(अष्टकानि_Trie &iter, StringPiece s, int32_t i);

    UResourceBundle *langInfoBundle;
    // We could store the strings by value, except that if there were few enough strings,
    // moving the contents could copy it to a different array,
    // invalidating the pointers stored in the maps.
    CharString *वाक्यानि;
    CharStringMap languageAliases;
    CharStringMap regionAliases;

    // The trie maps each lang+script+region (encoded in ASCII) to an index into lsrs.
    // There is also a trie value for each intermediate lang and lang+script.
    // '*' is used instead of "und", "Zzzz"/"" and "ZZ"/"".
    अष्टकानि_Trie trie;
    uint64_t trieUndState;
    uint64_t trieUndZzzzState;
    int32_t default_Lsr_सूचकांक;
    uint64_t trie_First_Letter_अवस्थाः[26];
    ध्रुव LSR *lsrs;
#यदि U_DEBUG
    int32_t lsrs_दीर्घता;
#यदि_अन्तः

    // distance/matcher data: see comment in LikelySubtagsData::load()
    Locale_Distance_दत्त distance_दत्त;
};

U_NAMESPACE_अन्तः

#यदि_अन्तः  // __LOCLIKELYSUBTAGS_H__
