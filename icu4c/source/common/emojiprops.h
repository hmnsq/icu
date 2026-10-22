// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#अगर_अघोषित __EMOJIPROPS_H__
#घोषणा __EMOJIPROPS_H__

#समावेश "unicode/utypes.h"
#समावेश "unicode/ucptrie.h"
#समावेश "unicode/udata.h"
#समावेश "unicode/uobject.h"
#समावेश "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : खुला UMemory {
खुला:
    // @internal
    EmojiProps(UErrorCode &errorCode) { load(errorCode); }
    ~EmojiProps();

    अटल स्थिर EmojiProps *getSingleton(UErrorCode &errorCode);
    अटल UBool has_द्विआधारी_Property(UChar32 c, UProperty which);
    अटल UBool has_द्विआधारी_Property(स्थिर char16_t *s, int32_t लंबाई, UProperty which);

    व्योम addPropertyStarts(स्थिर USetAdder *sa, UErrorCode &errorCode) स्थिर;
    व्योम addStrings(स्थिर USetAdder *sa, UProperty which, UErrorCode &errorCode) स्थिर;

    क्रमागत {
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
        IX_योग_आकार,

        // Not initially byte offsets.
        IX_RESERVED14,
        IX_RESERVED15,
        IX_गिनती  // 16
    };

    // Properties in the code point trie.
    क्रमागत {
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

निजी:
    अटल UBool U_CALLCONV
    isAcceptable(व्योम *context, स्थिर अक्षर *type, स्थिर अक्षर *name, स्थिर UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    अटल int32_t get_String_Trie_सूचकांक(int32_t i) {
        वापस i - IX_BASIC_EMOJI_TRIE_OFFSET;
    }

    व्योम load(UErrorCode &errorCode);
    UBool has_द्विआधारी_Property_Impl(UChar32 c, UProperty which) स्थिर;
    UBool has_द्विआधारी_Property_Impl(स्थिर char16_t *s, int32_t लंबाई, UProperty which) स्थिर;

    UDataMemory *memory = nullptr;
    UCPTrie *cpTrie = nullptr;
    स्थिर char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_END

#पूर्ण_अगर  // __EMOJIPROPS_H__
