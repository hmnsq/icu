// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#جيڪڏهن_وصف_نه_ٿيل __EMOJIPROPS_H__
#وصف_ڪريو __EMOJIPROPS_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/uobject.h"
#ضمن "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : عام UMemory {
عام:
    // @internal
    EmojiProps(UError_ڪوڊ &error_ڪوڊ) { ڀريو(error_ڪوڊ); }
    ~EmojiProps();

    ساكن ثابت EmojiProps *getSingleton(UError_ڪوڊ &error_ڪوڊ);
    ساكن UBool has_ٻٽو_Property(UChar32 c, UProperty which);
    ساكن UBool has_ٻٽو_Property(ثابت char16_t *s, int32_t ڊيگهائي, UProperty which);

    فراغ جوڙيو_Property_Starts(ثابت USetAdder *sa, UError_ڪوڊ &error_ڪوڊ) ثابت;
    فراغ جوڙيو_لڙيون(ثابت USetAdder *sa, UProperty which, UError_ڪوڊ &error_ڪوڊ) ثابت;

    تعداد {
        // Byte offsets from the start of the data, after the generic header,
        // in ascending order.
        // UCPTrie=CodePointTrie, follows the indexes
        IX_CPTRIE_وٿي,
        IX_RESERVED1,
        IX_RESERVED2,
        IX_RESERVED3,

        // UCharsTrie=CharsTrie
        IX_BASIC_EMOJI_TRIE_وٿي,
        IX_EMOJI_KEYCAP_SEQUENCE_TRIE_وٿي,
        IX_RGI_EMOJI_MODIFIER_SEQUENCE_TRIE_وٿي,
        IX_RGI_EMOJI_FLAG_SEQUENCE_TRIE_وٿي,
        IX_RGI_EMOJI_ٽيگ_SEQUENCE_TRIE_وٿي,
        IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_وٿي,
        IX_RESERVED10,
        IX_RESERVED11,
        IX_RESERVED12,
        IX_ڪل_سائيز,

        // Not initially byte offsets.
        IX_RESERVED14,
        IX_RESERVED15,
        IX_ڳڻپ  // 16
    };

    // Properties in the code point trie.
    تعداد {
        // https://www.unicode.org/reports/tr51/#Emoji_Properties
        ٻٽو_انگ_EMOJI,
        ٻٽو_انگ_EMOJI_PRESENTATION,
        ٻٽو_انگ_EMOJI_MODIFIER,
        ٻٽو_انگ_EMOJI_MODIFIER_بنياد,
        ٻٽو_انگ_EMOJI_COMPONENT,
        ٻٽو_انگ_EXTENDED_PICTOGRAPHIC,
        // https://www.unicode.org/reports/tr51/#Emoji_Sets
        ٻٽو_انگ_BASIC_EMOJI
    };

خاص:
    ساكن UBool U_CALLCONV
    isAcceptable(فراغ *context, ثابت حرف *type, ثابت حرف *name, ثابت UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    ساكن int32_t get_String_Trie_اشاريو(int32_t i) {
        أرجع i - IX_BASIC_EMOJI_TRIE_وٿي;
    }

    فراغ ڀريو(UError_ڪوڊ &error_ڪوڊ);
    UBool has_ٻٽو_Property_Impl(UChar32 c, UProperty which) ثابت;
    UBool has_ٻٽو_Property_Impl(ثابت char16_t *s, int32_t ڊيگهائي, UProperty which) ثابت;

    UData_يادگيري *يادگيري = nullptr;
    UCPTrie *cpTrie = nullptr;
    ثابت char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو  // __EMOJIPROPS_H__
