// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#ifndef __EMOJIPROPS_H__
#define __EMOJIPROPS_H__

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/ucptrie.h"
#অন্তর্ভাৱ "unicode/udata.h"
#অন্তর্ভাৱ "unicode/uobject.h"
#অন্তর্ভাৱ "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : সার্ৱজনিক UMemory {
সার্ৱজনিক:
    // @internal
    EmojiProps(UErrorCode &errorCode) { load(errorCode); }
    ~EmojiProps();

    স্থির ধ্রুৱ EmojiProps *getSingleton(UErrorCode &errorCode);
    স্থির UBool has_দ্বিমিক_Property(UChar32 c, UProperty which);
    স্থির UBool has_দ্বিমিক_Property(ধ্রুৱ char16_t *s, int32_t length, UProperty which);

    শূন্য addPropertyStarts(ধ্রুৱ USetAdder *sa, UErrorCode &errorCode) ধ্রুৱ;
    শূন্য addStrings(ধ্রুৱ USetAdder *sa, UProperty which, UErrorCode &errorCode) ধ্রুৱ;

    গণনা {
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
        IX_গুনতি  // 16
    };

    // Properties in the code point trie.
    গণনা {
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

গুপ্ত:
    স্থির UBool U_CALLCONV
    isAcceptable(শূন্য *context, ধ্রুৱ ৱর্ণ *type, ধ্রুৱ ৱর্ণ *name, ধ্রুৱ UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    স্থির int32_t getStringTrieIndex(int32_t i) {
        ঘূৰাই i - IX_BASIC_EMOJI_TRIE_OFFSET;
    }

    শূন্য load(UErrorCode &errorCode);
    UBool has_দ্বিমিক_Property_Impl(UChar32 c, UProperty which) ধ্রুৱ;
    UBool has_দ্বিমিক_Property_Impl(ধ্রুৱ char16_t *s, int32_t length, UProperty which) ধ্রুৱ;

    UDataMemory *memory = nullptr;
    UCPTrie *cpTrie = nullptr;
    ধ্রুৱ char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_END

#endif  // __EMOJIPROPS_H__
