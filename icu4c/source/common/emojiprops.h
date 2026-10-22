// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#ifndef __EMOJIPROPS_H__
#define __EMOJIPROPS_H__

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : सार्वजनिक UMemory {
सार्वजनिक:
    // @internal
    EmojiProps(UErrorCode &errorCode) { load(errorCode); }
    ~EmojiProps();

    स्थिर स्थिरांक EmojiProps *getSingleton(UErrorCode &errorCode);
    स्थिर UBool has_द्विमान_Property(UChar32 c, UProperty which);
    स्थिर UBool has_द्विमान_Property(स्थिरांक char16_t *s, int32_t length, UProperty which);

    शून्य addPropertyStarts(स्थिरांक USetAdder *sa, UErrorCode &errorCode) स्थिरांक;
    शून्य addStrings(स्थिरांक USetAdder *sa, UProperty which, UErrorCode &errorCode) स्थिरांक;

    गणना {
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
        IX_मोजणी  // 16
    };

    // Properties in the code point trie.
    गणना {
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

खाजगी:
    स्थिर UBool U_CALLCONV
    isAcceptable(शून्य *context, स्थिरांक अक्षर *type, स्थिरांक अक्षर *name, स्थिरांक UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    स्थिर int32_t getStringTrieIndex(int32_t i) {
        परत i - IX_BASIC_EMOJI_TRIE_OFFSET;
    }

    शून्य load(UErrorCode &errorCode);
    UBool has_द्विमान_Property_Impl(UChar32 c, UProperty which) स्थिरांक;
    UBool has_द्विमान_Property_Impl(स्थिरांक char16_t *s, int32_t length, UProperty which) स्थिरांक;

    UDataMemory *memory = nullptr;
    UCPTrie *cpTrie = nullptr;
    स्थिरांक char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_END

#endif  // __EMOJIPROPS_H__
