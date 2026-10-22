// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#ifndef __EMOJIPROPS_H__
#define __EMOJIPROPS_H__

#شامل "unicode/utypes.h"
#شامل "unicode/ucptrie.h"
#شامل "unicode/udata.h"
#شامل "unicode/uobject.h"
#شامل "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : عمومی UMemory {
عمومی:
    // @internal
    EmojiProps(UErrorCode &errorCode) { load(errorCode); }
    ~EmojiProps();

    ایستا ثابت EmojiProps *getSingleton(UErrorCode &errorCode);
    ایستا UBool has_دودویی_Property(UChar32 c, UProperty which);
    ایستا UBool has_دودویی_Property(ثابت char16_t *s, int32_t length, UProperty which);

    تهی addPropertyStarts(ثابت USetAdder *sa, UErrorCode &errorCode) ثابت;
    تهی addStrings(ثابت USetAdder *sa, UProperty which, UErrorCode &errorCode) ثابت;

    شمارش {
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
        IX_شمار  // 16
    };

    // Properties in the code point trie.
    شمارش {
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

خصوصی:
    ایستا UBool U_CALLCONV
    isAcceptable(تهی *context, ثابت نویسه *type, ثابت نویسه *name, ثابت UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    ایستا int32_t getStringTrieIndex(int32_t i) {
        بازگشت i - IX_BASIC_EMOJI_TRIE_OFFSET;
    }

    تهی load(UErrorCode &errorCode);
    UBool has_دودویی_Property_Impl(UChar32 c, UProperty which) ثابت;
    UBool has_دودویی_Property_Impl(ثابت char16_t *s, int32_t length, UProperty which) ثابت;

    UDataMemory *memory = nullptr;
    UCPTrie *cpTrie = nullptr;
    ثابت char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_END

#endif  // __EMOJIPROPS_H__
