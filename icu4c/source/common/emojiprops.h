// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#اگر_تعریف_نشده __EMOJIPROPS_H__
#تعریف_کن __EMOJIPROPS_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/uobject.h"
#ضمن "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : عام UMemory {
عام:
    // @internal
    EmojiProps(UError_کد &error_کد) { بارگیری(error_کد); }
    ~EmojiProps();

    ساكن ثابت EmojiProps *getSingleton(UError_کد &error_کد);
    ساكن UBool has_دودویی_Property(UChar32 c, UProperty which);
    ساكن UBool has_دودویی_Property(ثابت char16_t *s, int32_t درازی, UProperty which);

    فراغ جمع_کن_Property_Starts(ثابت USetAdder *sa, UError_کد &error_کد) ثابت;
    فراغ جمع_کن_رشتهها(ثابت USetAdder *sa, UProperty which, UError_کد &error_کد) ثابت;

    تعداد {
        // Byte offsets from the start of the data, after the generic header,
        // in ascending order.
        // UCPTrie=CodePointTrie, follows the indexes
        IX_CPTRIE_جابه_جایی,
        IX_RESERVED1,
        IX_RESERVED2,
        IX_RESERVED3,

        // UCharsTrie=CharsTrie
        IX_BASIC_EMOJI_TRIE_جابه_جایی,
        IX_EMOJI_KEYCAP_SEQUENCE_TRIE_جابه_جایی,
        IX_RGI_EMOJI_MODIFIER_SEQUENCE_TRIE_جابه_جایی,
        IX_RGI_EMOJI_FLAG_SEQUENCE_TRIE_جابه_جایی,
        IX_RGI_EMOJI_برچسب_SEQUENCE_TRIE_جابه_جایی,
        IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_جابه_جایی,
        IX_RESERVED10,
        IX_RESERVED11,
        IX_RESERVED12,
        IX_مجموع_اندازه,

        // Not initially byte offsets.
        IX_RESERVED14,
        IX_RESERVED15,
        IX_شمار  // 16
    };

    // Properties in the code point trie.
    تعداد {
        // https://www.unicode.org/reports/tr51/#Emoji_Properties
        بیت_EMOJI,
        بیت_EMOJI_PRESENTATION,
        بیت_EMOJI_MODIFIER,
        بیت_EMOJI_MODIFIER_پایه,
        بیت_EMOJI_COMPONENT,
        بیت_EXTENDED_PICTOGRAPHIC,
        // https://www.unicode.org/reports/tr51/#Emoji_Sets
        بیت_BASIC_EMOJI
    };

خاص:
    ساكن UBool U_CALLCONV
    isAcceptable(فراغ *context, ثابت حرف *type, ثابت حرف *name, ثابت UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    ساكن int32_t get_String_Trie_شاخص(int32_t i) {
        أرجع i - IX_BASIC_EMOJI_TRIE_جابه_جایی;
    }

    فراغ بارگیری(UError_کد &error_کد);
    UBool has_دودویی_Property_Impl(UChar32 c, UProperty which) ثابت;
    UBool has_دودویی_Property_Impl(ثابت char16_t *s, int32_t درازی, UProperty which) ثابت;

    UData_حافظه *حافظه = nullptr;
    UCPTrie *cpTrie = nullptr;
    ثابت char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_انتها

#پایان_اگر  // __EMOJIPROPS_H__
