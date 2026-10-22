// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __EMOJIPROPS_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __EMOJIPROPS_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    // @internal
    EmojiProps(UError_ਕੋਡ &error_ਕੋਡ) { ਲੱਦੋ(error_ਕੋਡ); }
    ~EmojiProps();

    ਸਥਿਰ ਅਚਲ EmojiProps *getSingleton(UError_ਕੋਡ &error_ਕੋਡ);
    ਸਥਿਰ UBool has_ਦੁਆਧਾਰੀ_Property(UChar32 c, UProperty which);
    ਸਥਿਰ UBool has_ਦੁਆਧਾਰੀ_Property(ਅਚਲ char16_t *s, int32_t ਲੰਮਾਈ, UProperty which);

    ਖਾਲੀ ਜੋੜੋ_Property_Starts(ਅਚਲ USetAdder *sa, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ;
    ਖਾਲੀ ਜੋੜੋ_ਲੜੀਆਂ(ਅਚਲ USetAdder *sa, UProperty which, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ;

    ਗਿਣਤੀ {
        // Byte offsets from the start of the data, after the generic header,
        // in ascending order.
        // UCPTrie=CodePointTrie, follows the indexes
        IX_CPTRIE_ਵਿੱਥ,
        IX_RESERVED1,
        IX_RESERVED2,
        IX_RESERVED3,

        // UCharsTrie=CharsTrie
        IX_BASIC_EMOJI_TRIE_ਵਿੱਥ,
        IX_EMOJI_KEYCAP_SEQUENCE_TRIE_ਵਿੱਥ,
        IX_RGI_EMOJI_MODIFIER_SEQUENCE_TRIE_ਵਿੱਥ,
        IX_RGI_EMOJI_FLAG_SEQUENCE_TRIE_ਵਿੱਥ,
        IX_RGI_EMOJI_ਟੈਗ_SEQUENCE_TRIE_ਵਿੱਥ,
        IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_ਵਿੱਥ,
        IX_RESERVED10,
        IX_RESERVED11,
        IX_RESERVED12,
        IX_ਕੁੱਲ_ਮਾਪ,

        // Not initially byte offsets.
        IX_RESERVED14,
        IX_RESERVED15,
        IX_ਗਿਣਤੀ  // 16
    };

    // Properties in the code point trie.
    ਗਿਣਤੀ {
        // https://www.unicode.org/reports/tr51/#Emoji_Properties
        ਦੋਆਧਾਰੀ_ਅੰਕ_EMOJI,
        ਦੋਆਧਾਰੀ_ਅੰਕ_EMOJI_PRESENTATION,
        ਦੋਆਧਾਰੀ_ਅੰਕ_EMOJI_MODIFIER,
        ਦੋਆਧਾਰੀ_ਅੰਕ_EMOJI_MODIFIER_ਅਧਾਰ,
        ਦੋਆਧਾਰੀ_ਅੰਕ_EMOJI_COMPONENT,
        ਦੋਆਧਾਰੀ_ਅੰਕ_EXTENDED_PICTOGRAPHIC,
        // https://www.unicode.org/reports/tr51/#Emoji_Sets
        ਦੋਆਧਾਰੀ_ਅੰਕ_BASIC_EMOJI
    };

ਨਿਜੀ:
    ਸਥਿਰ UBool U_CALLCONV
    isAcceptable(ਖਾਲੀ *context, ਅਚਲ ਅੱਖਰ *type, ਅਚਲ ਅੱਖਰ *name, ਅਚਲ UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    ਸਥਿਰ int32_t get_String_Trie_ਸੂਚਕਾਂਕ(int32_t i) {
        ਵਾਪਸ i - IX_BASIC_EMOJI_TRIE_ਵਿੱਥ;
    }

    ਖਾਲੀ ਲੱਦੋ(UError_ਕੋਡ &error_ਕੋਡ);
    UBool has_ਦੁਆਧਾਰੀ_Property_Impl(UChar32 c, UProperty which) ਅਚਲ;
    UBool has_ਦੁਆਧਾਰੀ_Property_Impl(ਅਚਲ char16_t *s, int32_t ਲੰਮਾਈ, UProperty which) ਅਚਲ;

    UData_ਸਮ੍ਰਿਤੀ *ਸਮ੍ਰਿਤੀ = nullptr;
    UCPTrie *cpTrie = nullptr;
    ਅਚਲ char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  // __EMOJIPROPS_H__
