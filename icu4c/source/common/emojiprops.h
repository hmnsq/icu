// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#اگر_غیر_تعریف_شدہ __EMOJIPROPS_H__
#تعریف_کرو __EMOJIPROPS_H__

#شامل "unicode/utypes.h"
#شامل "unicode/ucptrie.h"
#شامل "unicode/udata.h"
#شامل "unicode/uobject.h"
#شامل "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : عوامی UMemory {
عوامی:
    // @internal
    EmojiProps(UError_رمز &error_رمز) { بھرو(error_رمز); }
    ~EmojiProps();

    ساکن ثابت EmojiProps *getSingleton(UError_رمز &error_رمز);
    ساکن UBool has_ثنائی_Property(UChar32 c, UProperty which);
    ساکن UBool has_ثنائی_Property(ثابت char16_t *s, int32_t لمبائی, UProperty which);

    خالی جوڑو_Property_Starts(ثابت USetAdder *sa, UError_رمز &error_رمز) ثابت;
    خالی جوڑو_لڑیاں(ثابت USetAdder *sa, UProperty which, UError_رمز &error_رمز) ثابت;

    شمار {
        // Byte offsets from the start of the data, after the generic header,
        // in ascending order.
        // UCPTrie=CodePointTrie, follows the indexes
        IX_CPTRIE_انحراف,
        IX_RESERVED1,
        IX_RESERVED2,
        IX_RESERVED3,

        // UCharsTrie=CharsTrie
        IX_BASIC_EMOJI_TRIE_انحراف,
        IX_EMOJI_KEYCAP_SEQUENCE_TRIE_انحراف,
        IX_RGI_EMOJI_MODIFIER_SEQUENCE_TRIE_انحراف,
        IX_RGI_EMOJI_FLAG_SEQUENCE_TRIE_انحراف,
        IX_RGI_EMOJI_لیبل_SEQUENCE_TRIE_انحراف,
        IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_انحراف,
        IX_RESERVED10,
        IX_RESERVED11,
        IX_RESERVED12,
        IX_کل_سائز,

        // Not initially byte offsets.
        IX_RESERVED14,
        IX_RESERVED15,
        IX_گنتی  // 16
    };

    // Properties in the code point trie.
    شمار {
        // https://www.unicode.org/reports/tr51/#Emoji_Properties
        ثنائی_ہندسہ_EMOJI,
        ثنائی_ہندسہ_EMOJI_PRESENTATION,
        ثنائی_ہندسہ_EMOJI_MODIFIER,
        ثنائی_ہندسہ_EMOJI_MODIFIER_بنیاد,
        ثنائی_ہندسہ_EMOJI_COMPONENT,
        ثنائی_ہندسہ_EXTENDED_PICTOGRAPHIC,
        // https://www.unicode.org/reports/tr51/#Emoji_Sets
        ثنائی_ہندسہ_BASIC_EMOJI
    };

نجی:
    ساکن UBool U_CALLCONV
    isAcceptable(خالی *context, ثابت حرف *type, ثابت حرف *name, ثابت UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    ساکن int32_t get_String_Trie_اشاریہ(int32_t i) {
        واپس i - IX_BASIC_EMOJI_TRIE_انحراف;
    }

    خالی بھرو(UError_رمز &error_رمز);
    UBool has_ثنائی_Property_Impl(UChar32 c, UProperty which) ثابت;
    UBool has_ثنائی_Property_Impl(ثابت char16_t *s, int32_t لمبائی, UProperty which) ثابت;

    UData_حافظہ *حافظہ = nullptr;
    UCPTrie *cpTrie = nullptr;
    ثابت char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_انتہا

#اگر_ختم  // __EMOJIPROPS_H__
