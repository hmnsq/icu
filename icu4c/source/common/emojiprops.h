// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#ifndef __EMOJIPROPS_H__
#define __EMOJIPROPS_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    // @internal
    EmojiProps(UErrorCode &errorCode) { load(errorCode); }
    ~EmojiProps();

    ಸ್ಥಿರ ಧ್ರುವ EmojiProps *getSingleton(UErrorCode &errorCode);
    ಸ್ಥಿರ UBool has_ದ್ವಿಮಾನ_Property(UChar32 c, UProperty which);
    ಸ್ಥಿರ UBool has_ದ್ವಿಮಾನ_Property(ಧ್ರುವ char16_t *s, int32_t length, UProperty which);

    ಶೂನ್ಯ addPropertyStarts(ಧ್ರುವ USetAdder *sa, UErrorCode &errorCode) ಧ್ರುವ;
    ಶೂನ್ಯ addStrings(ಧ್ರುವ USetAdder *sa, UProperty which, UErrorCode &errorCode) ಧ್ರುವ;

    ಗಣನಾ {
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
        IX_ಎಣಿಕೆ  // 16
    };

    // Properties in the code point trie.
    ಗಣನಾ {
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

ಗುಪ್ತ:
    ಸ್ಥಿರ UBool U_CALLCONV
    isAcceptable(ಶೂನ್ಯ *context, ಧ್ರುವ ವರ್ಣ *type, ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    ಸ್ಥಿರ int32_t getStringTrieIndex(int32_t i) {
        ಪ್ರತ್ಯಾಗಮ i - IX_BASIC_EMOJI_TRIE_OFFSET;
    }

    ಶೂನ್ಯ load(UErrorCode &errorCode);
    UBool has_ದ್ವಿಮಾನ_Property_Impl(UChar32 c, UProperty which) ಧ್ರುವ;
    UBool has_ದ್ವಿಮಾನ_Property_Impl(ಧ್ರುವ char16_t *s, int32_t length, UProperty which) ಧ್ರುವ;

    UDataMemory *memory = nullptr;
    UCPTrie *cpTrie = nullptr;
    ಧ್ರುವ char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_END

#endif  // __EMOJIPROPS_H__
