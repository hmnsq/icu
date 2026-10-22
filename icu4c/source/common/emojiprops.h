// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#ifndef __EMOJIPROPS_H__
#define __EMOJIPROPS_H__

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/ucptrie.h"
#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : பொது UMemory {
பொது:
    // @internal
    EmojiProps(UErrorCode &errorCode) { load(errorCode); }
    ~EmojiProps();

    நிலையான மாறிலி EmojiProps *getSingleton(UErrorCode &errorCode);
    நிலையான UBool has_இருமம்_Property(UChar32 c, UProperty which);
    நிலையான UBool has_இருமம்_Property(மாறிலி char16_t *s, int32_t length, UProperty which);

    வெறுமை addPropertyStarts(மாறிலி USetAdder *sa, UErrorCode &errorCode) மாறிலி;
    வெறுமை addStrings(மாறிலி USetAdder *sa, UProperty which, UErrorCode &errorCode) மாறிலி;

    எண்ணிக்கை {
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
        IX_மொத்தம்  // 16
    };

    // Properties in the code point trie.
    எண்ணிக்கை {
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

தனிப்பட்ட:
    நிலையான UBool U_CALLCONV
    isAcceptable(வெறுமை *context, மாறிலி எழுத்து *type, மாறிலி எழுத்து *name, மாறிலி UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    நிலையான int32_t getStringTrieIndex(int32_t i) {
        திரும்பு i - IX_BASIC_EMOJI_TRIE_OFFSET;
    }

    வெறுமை load(UErrorCode &errorCode);
    UBool has_இருமம்_Property_Impl(UChar32 c, UProperty which) மாறிலி;
    UBool has_இருமம்_Property_Impl(மாறிலி char16_t *s, int32_t length, UProperty which) மாறிலி;

    UDataMemory *memory = nullptr;
    UCPTrie *cpTrie = nullptr;
    மாறிலி char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_END

#endif  // __EMOJIPROPS_H__
