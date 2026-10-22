// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#که_نه_تعریف_شوی __EMOJIPROPS_H__
#تعریف_کړه __EMOJIPROPS_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/uobject.h"
#ضمن "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : عام UMemory {
عام:
    // @internal
    EmojiProps(UError_کوډ &error_کوډ) { بارول(error_کوډ); }
    ~EmojiProps();

    ساكن ثابت EmojiProps *getSingleton(UError_کوډ &error_کوډ);
    ساكن UBool has_دوګونی_Property(UChar32 c, UProperty which);
    ساكن UBool has_دوګونی_Property(ثابت char16_t *s, int32_t اوږدتیا, UProperty which);

    فراغ جمع_کړه_Property_Starts(ثابت USetAdder *sa, UError_کوډ &error_کوډ) ثابت;
    فراغ جمع_کړه_مزي_ګانې(ثابت USetAdder *sa, UProperty which, UError_کوډ &error_کوډ) ثابت;

    تعداد {
        // Byte offsets from the start of the data, after the generic header,
        // in ascending order.
        // UCPTrie=CodePointTrie, follows the indexes
        IX_CPTRIE_دننه_واټن,
        IX_RESERVED1,
        IX_RESERVED2,
        IX_RESERVED3,

        // UCharsTrie=CharsTrie
        IX_BASIC_EMOJI_TRIE_دننه_واټن,
        IX_EMOJI_KEYCAP_SEQUENCE_TRIE_دننه_واټن,
        IX_RGI_EMOJI_MODIFIER_SEQUENCE_TRIE_دننه_واټن,
        IX_RGI_EMOJI_FLAG_SEQUENCE_TRIE_دننه_واټن,
        IX_RGI_EMOJI_نښان_SEQUENCE_TRIE_دننه_واټن,
        IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_دننه_واټن,
        IX_RESERVED10,
        IX_RESERVED11,
        IX_RESERVED12,
        IX_ټول_اندازه,

        // Not initially byte offsets.
        IX_RESERVED14,
        IX_RESERVED15,
        IX_شمېر  // 16
    };

    // Properties in the code point trie.
    تعداد {
        // https://www.unicode.org/reports/tr51/#Emoji_Properties
        بټ_EMOJI,
        بټ_EMOJI_PRESENTATION,
        بټ_EMOJI_MODIFIER,
        بټ_EMOJI_MODIFIER_بنسټ,
        بټ_EMOJI_COMPONENT,
        بټ_EXTENDED_PICTOGRAPHIC,
        // https://www.unicode.org/reports/tr51/#Emoji_Sets
        بټ_BASIC_EMOJI
    };

خاص:
    ساكن UBool U_CALLCONV
    isAcceptable(فراغ *context, ثابت حرف *type, ثابت حرف *name, ثابت UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    ساكن int32_t get_String_Trie_شاخص(int32_t i) {
        أرجع i - IX_BASIC_EMOJI_TRIE_دننه_واټن;
    }

    فراغ بارول(UError_کوډ &error_کوډ);
    UBool has_دوګونی_Property_Impl(UChar32 c, UProperty which) ثابت;
    UBool has_دوګونی_Property_Impl(ثابت char16_t *s, int32_t اوږدتیا, UProperty which) ثابت;

    UData_حافظه *حافظه = nullptr;
    UCPTrie *cpTrie = nullptr;
    ثابت char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_پای_ټکی

#د_که_پای  // __EMOJIPROPS_H__
