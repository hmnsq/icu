// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#यदि_अपरिभाषितम् __EMOJIPROPS_H__
#परिभाषय __EMOJIPROPS_H__

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : सार्वजनिक UMemory {
सार्वजनिक:
    // @internal
    EmojiProps(UError_संहिता &error_संहिता) { आरोपय(error_संहिता); }
    ~EmojiProps();

    स्थिर ध्रुव EmojiProps *getSingleton(UError_संहिता &error_संहिता);
    स्थिर UBool has_द्विमान_Property(UChar32 c, UProperty which);
    स्थिर UBool has_द्विमान_Property(ध्रुव char16_t *s, int32_t दीर्घता, UProperty which);

    शून्य सङ्कलय_Property_Starts(ध्रुव USetAdder *sa, UError_संहिता &error_संहिता) ध्रुव;
    शून्य सङ्कलय_वाक्यानि(ध्रुव USetAdder *sa, UProperty which, UError_संहिता &error_संहिता) ध्रुव;

    गणना {
        // Byte offsets from the start of the data, after the generic header,
        // in ascending order.
        // UCPTrie=CodePointTrie, follows the indexes
        IX_CPTRIE_विस्थापनम्,
        IX_RESERVED1,
        IX_RESERVED2,
        IX_RESERVED3,

        // UCharsTrie=CharsTrie
        IX_BASIC_EMOJI_TRIE_विस्थापनम्,
        IX_EMOJI_KEYCAP_SEQUENCE_TRIE_विस्थापनम्,
        IX_RGI_EMOJI_MODIFIER_SEQUENCE_TRIE_विस्थापनम्,
        IX_RGI_EMOJI_FLAG_SEQUENCE_TRIE_विस्थापनम्,
        IX_RGI_EMOJI_चिह्नकम्_SEQUENCE_TRIE_विस्थापनम्,
        IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_विस्थापनम्,
        IX_RESERVED10,
        IX_RESERVED11,
        IX_RESERVED12,
        IX_योग_आकार,

        // Not initially byte offsets.
        IX_RESERVED14,
        IX_RESERVED15,
        IX_गणनम्  // 16
    };

    // Properties in the code point trie.
    गणना {
        // https://www.unicode.org/reports/tr51/#Emoji_Properties
        BIT_EMOJI,
        BIT_EMOJI_PRESENTATION,
        BIT_EMOJI_MODIFIER,
        BIT_EMOJI_MODIFIER_आधारः,
        BIT_EMOJI_COMPONENT,
        BIT_EXTENDED_PICTOGRAPHIC,
        // https://www.unicode.org/reports/tr51/#Emoji_Sets
        BIT_BASIC_EMOJI
    };

गुप्त:
    स्थिर UBool U_CALLCONV
    isAcceptable(शून्य *context, ध्रुव वर्ण *type, ध्रुव वर्ण *name, ध्रुव UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    स्थिर int32_t get_String_Trie_सूचकांक(int32_t i) {
        प्रत्यागम i - IX_BASIC_EMOJI_TRIE_विस्थापनम्;
    }

    शून्य आरोपय(UError_संहिता &error_संहिता);
    UBool has_द्विमान_Property_Impl(UChar32 c, UProperty which) ध्रुव;
    UBool has_द्विमान_Property_Impl(ध्रुव char16_t *s, int32_t दीर्घता, UProperty which) ध्रुव;

    UData_स्मृतिः *स्मृतिः = nullptr;
    UCPTrie *cpTrie = nullptr;
    ध्रुव char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_अन्तः

#यदि_अन्तः  // __EMOJIPROPS_H__
