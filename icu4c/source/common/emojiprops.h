// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#ifndef __EMOJIPROPS_H__
#define __EMOJIPROPS_H__

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/ucptrie.h"
#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : പൊതു UMemory {
പൊതു:
    // @internal
    EmojiProps(UErrorCode &errorCode) { load(errorCode); }
    ~EmojiProps();

    സ്ഥിതം സ്ഥിരം EmojiProps *getSingleton(UErrorCode &errorCode);
    സ്ഥിതം UBool has_ദ്വിമാന_Property(UChar32 c, UProperty which);
    സ്ഥിതം UBool has_ദ്വിമാന_Property(സ്ഥിരം char16_t *s, int32_t length, UProperty which);

    ശൂന്യം addPropertyStarts(സ്ഥിരം USetAdder *sa, UErrorCode &errorCode) സ്ഥിരം;
    ശൂന്യം addStrings(സ്ഥിരം USetAdder *sa, UProperty which, UErrorCode &errorCode) സ്ഥിരം;

    എണ്ണൽ {
        // Byte offsets from the start of the data, after the generic header,
        // in ascending order.
        // UCPTrie=CodePointTrie, follows the indexes
        IX_CPTRIE_OFFSET,
        IX_RESERVED1,
        IX_RESERVED2,
        IX_RESERVED3,

        // UCharsTrie=CharsTrie
        IX_BASIC_EMOJI_TRIE_OFFSET,
        IX_EMOJI_KEYCAP_SEQUENCE_TRIE_OFFSET,
        IX_RGI_EMOJI_MODIFIER_SEQUENCE_TRIE_OFFSET,
        IX_RGI_EMOJI_FLAG_SEQUENCE_TRIE_OFFSET,
        IX_RGI_EMOJI_TAG_SEQUENCE_TRIE_OFFSET,
        IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET,
        IX_RESERVED10,
        IX_RESERVED11,
        IX_RESERVED12,
        IX_TOTAL_SIZE,

        // Not initially byte offsets.
        IX_RESERVED14,
        IX_RESERVED15,
        IX_എണ്ണം  // 16
    };

    // Properties in the code point trie.
    എണ്ണൽ {
        // https://www.unicode.org/reports/tr51/#Emoji_Properties
        BIT_EMOJI,
        BIT_EMOJI_PRESENTATION,
        BIT_EMOJI_MODIFIER,
        BIT_EMOJI_MODIFIER_BASE,
        BIT_EMOJI_COMPONENT,
        BIT_EXTENDED_PICTOGRAPHIC,
        // https://www.unicode.org/reports/tr51/#Emoji_Sets
        BIT_BASIC_EMOJI
    };

സ്വകാര്യ:
    സ്ഥിതം UBool U_CALLCONV
    isAcceptable(ശൂന്യം *context, സ്ഥിരം അക്ഷരം *type, സ്ഥിരം അക്ഷരം *name, സ്ഥിരം UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    സ്ഥിതം int32_t getStringTrieIndex(int32_t i) {
        തിരികെ i - IX_BASIC_EMOJI_TRIE_OFFSET;
    }

    ശൂന്യം load(UErrorCode &errorCode);
    UBool has_ദ്വിമാന_Property_Impl(UChar32 c, UProperty which) സ്ഥിരം;
    UBool has_ദ്വിമാന_Property_Impl(സ്ഥിരം char16_t *s, int32_t length, UProperty which) സ്ഥിരം;

    UDataMemory *memory = nullptr;
    UCPTrie *cpTrie = nullptr;
    സ്ഥിരം char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_END

#endif  // __EMOJIPROPS_H__
