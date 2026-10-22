// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#جے_بنا_تعریف __EMOJIPROPS_H__
#تعریف_کرو __EMOJIPROPS_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/uobject.h"
#ضمن "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : عام UMemory {
عام:
    // @internal
    EmojiProps(UError_کوڈ &error_کوڈ) { لدو(error_کوڈ); }
    ~EmojiProps();

    ساكن ثابت EmojiProps *getSingleton(UError_کوڈ &error_کوڈ);
    ساكن UBool has_ثنائی_Property(UChar32 c, UProperty which);
    ساكن UBool has_ثنائی_Property(ثابت char16_t *s, int32_t لمائی, UProperty which);

    فراغ جوڑو_Property_Starts(ثابت USetAdder *sa, UError_کوڈ &error_کوڈ) ثابت;
    فراغ جوڑو_لڑیاں(ثابت USetAdder *sa, UProperty which, UError_کوڈ &error_کوڈ) ثابت;

    تعداد {
        // Byte offsets from the start of the data, after the generic header,
        // in ascending order.
        // UCPTrie=CodePointTrie, follows the indexes
        IX_CPTRIE_وتھ,
        IX_RESERVED1,
        IX_RESERVED2,
        IX_RESERVED3,

        // UCharsTrie=CharsTrie
        IX_BASIC_EMOJI_TRIE_وتھ,
        IX_EMOJI_KEYCAP_SEQUENCE_TRIE_وتھ,
        IX_RGI_EMOJI_MODIFIER_SEQUENCE_TRIE_وتھ,
        IX_RGI_EMOJI_FLAG_SEQUENCE_TRIE_وتھ,
        IX_RGI_EMOJI_ٹیگ_SEQUENCE_TRIE_وتھ,
        IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_وتھ,
        IX_RESERVED10,
        IX_RESERVED11,
        IX_RESERVED12,
        IX_کل_ناپ,

        // Not initially byte offsets.
        IX_RESERVED14,
        IX_RESERVED15,
        IX_گنتی  // 16
    };

    // Properties in the code point trie.
    تعداد {
        // https://www.unicode.org/reports/tr51/#Emoji_Properties
        ثنائی_ہندسہ_EMOJI,
        ثنائی_ہندسہ_EMOJI_PRESENTATION,
        ثنائی_ہندسہ_EMOJI_MODIFIER,
        ثنائی_ہندسہ_EMOJI_MODIFIER_ادھار,
        ثنائی_ہندسہ_EMOJI_COMPONENT,
        ثنائی_ہندسہ_EXTENDED_PICTOGRAPHIC,
        // https://www.unicode.org/reports/tr51/#Emoji_Sets
        ثنائی_ہندسہ_BASIC_EMOJI
    };

خاص:
    ساكن UBool U_CALLCONV
    isAcceptable(فراغ *context, ثابت حرف *type, ثابت حرف *name, ثابت UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    ساكن int32_t get_String_Trie_اشاریہ(int32_t i) {
        أرجع i - IX_BASIC_EMOJI_TRIE_وتھ;
    }

    فراغ لدو(UError_کوڈ &error_کوڈ);
    UBool has_ثنائی_Property_Impl(UChar32 c, UProperty which) ثابت;
    UBool has_ثنائی_Property_Impl(ثابت char16_t *s, int32_t لمائی, UProperty which) ثابت;

    UData_حافظہ *حافظہ = nullptr;
    UCPTrie *cpTrie = nullptr;
    ثابت char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_سرا

#جے_انت  // __EMOJIPROPS_H__
