// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#ifndef __EMOJIPROPS_H__
#define __EMOJIPROPS_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/uobject.h"
#ضمن "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : عام UMemory {
عام:
    // @internal
    EmojiProps(UErrorCode &errorCode) { load(errorCode); }
    ~EmojiProps();

    ساكن ثابت EmojiProps *getSingleton(UErrorCode &errorCode);
    ساكن UBool has_نبيو_Property(UChar32 c, UProperty which);
    ساكن UBool has_نبيو_Property(ثابت char16_t *s, int32_t length, UProperty which);

    فراغ addPropertyStarts(ثابت USetAdder *sa, UErrorCode &errorCode) ثابت;
    فراغ addStrings(ثابت USetAdder *sa, UProperty which, UErrorCode &errorCode) ثابت;

    تعداد {
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
        IX_كدي  // 16
    };

    // Properties in the code point trie.
    تعداد {
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

خاص:
    ساكن UBool U_CALLCONV
    isAcceptable(فراغ *context, ثابت حرف *type, ثابت حرف *name, ثابت UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    ساكن int32_t getStringTrieIndex(int32_t i) {
        أرجع i - IX_BASIC_EMOJI_TRIE_OFFSET;
    }

    فراغ load(UErrorCode &errorCode);
    UBool has_نبيو_Property_Impl(UChar32 c, UProperty which) ثابت;
    UBool has_نبيو_Property_Impl(ثابت char16_t *s, int32_t length, UProperty which) ثابت;

    UDataMemory *memory = nullptr;
    UCPTrie *cpTrie = nullptr;
    ثابت char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_END

#endif  // __EMOJIPROPS_H__
