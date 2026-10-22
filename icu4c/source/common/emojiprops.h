// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#ifndef __EMOJIPROPS_H__
#define __EMOJIPROPS_H__

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/ucptrie.h"
#અન્તર્ભાવ "unicode/udata.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : જાહેર UMemory {
જાહેર:
    // @internal
    EmojiProps(UErrorCode &errorCode) { load(errorCode); }
    ~EmojiProps();

    સ્થિર અચલ EmojiProps *getSingleton(UErrorCode &errorCode);
    સ્થિર UBool has_દ્વિઅંકી_Property(UChar32 c, UProperty which);
    સ્થિર UBool has_દ્વિઅંકી_Property(અચલ char16_t *s, int32_t length, UProperty which);

    શૂન્ય addPropertyStarts(અચલ USetAdder *sa, UErrorCode &errorCode) અચલ;
    શૂન્ય addStrings(અચલ USetAdder *sa, UProperty which, UErrorCode &errorCode) અચલ;

    ગણના {
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
        IX_ગણતરી  // 16
    };

    // Properties in the code point trie.
    ગણના {
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

ખાનગી:
    સ્થિર UBool U_CALLCONV
    isAcceptable(શૂન્ય *context, અચલ અક્ષર *type, અચલ અક્ષર *name, અચલ UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    સ્થિર int32_t getStringTrieIndex(int32_t i) {
        પાછા i - IX_BASIC_EMOJI_TRIE_OFFSET;
    }

    શૂન્ય load(UErrorCode &errorCode);
    UBool has_દ્વિઅંકી_Property_Impl(UChar32 c, UProperty which) અચલ;
    UBool has_દ્વિઅંકી_Property_Impl(અચલ char16_t *s, int32_t length, UProperty which) અચલ;

    UDataMemory *memory = nullptr;
    UCPTrie *cpTrie = nullptr;
    અચલ char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_END

#endif  // __EMOJIPROPS_H__
