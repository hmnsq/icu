// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#నిర్వచితం_కాకపోతే __EMOJIPROPS_H__
#నిర్వచించు __EMOJIPROPS_H__

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/ucptrie.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : బహిరంగ UMemory {
బహిరంగ:
    // @internal
    EmojiProps(UError_సంకేతం &error_సంకేతం) { ఎక్కించు(error_సంకేతం); }
    ~EmojiProps();

    స్థిర స్థిరరాశి EmojiProps *getSingleton(UError_సంకేతం &error_సంకేతం);
    స్థిర UBool has_ద్విమాన_Property(UChar32 c, UProperty which);
    స్థిర UBool has_ద్విమాన_Property(స్థిరరాశి char16_t *s, int32_t నిడివి, UProperty which);

    శూన్య కూడు_Property_Starts(స్థిరరాశి USetAdder *sa, UError_సంకేతం &error_సంకేతం) స్థిరరాశి;
    శూన్య కూడు_అక్షరశ్రేణులు(స్థిరరాశి USetAdder *sa, UProperty which, UError_సంకేతం &error_సంకేతం) స్థిరరాశి;

    గణన {
        // Byte offsets from the start of the data, after the generic header,
        // in ascending order.
        // UCPTrie=CodePointTrie, follows the indexes
        IX_CPTRIE_స్థానభ్రంశం,
        IX_RESERVED1,
        IX_RESERVED2,
        IX_RESERVED3,

        // UCharsTrie=CharsTrie
        IX_BASIC_EMOJI_TRIE_స్థానభ్రంశం,
        IX_EMOJI_KEYCAP_SEQUENCE_TRIE_స్థానభ్రంశం,
        IX_RGI_EMOJI_MODIFIER_SEQUENCE_TRIE_స్థానభ్రంశం,
        IX_RGI_EMOJI_FLAG_SEQUENCE_TRIE_స్థానభ్రంశం,
        IX_RGI_EMOJI_ట్యాగ్_SEQUENCE_TRIE_స్థానభ్రంశం,
        IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_స్థానభ్రంశం,
        IX_RESERVED10,
        IX_RESERVED11,
        IX_RESERVED12,
        IX_మొత్తం_కొలత,

        // Not initially byte offsets.
        IX_RESERVED14,
        IX_RESERVED15,
        IX_లెక్క  // 16
    };

    // Properties in the code point trie.
    గణన {
        // https://www.unicode.org/reports/tr51/#Emoji_Properties
        బిట్టు_EMOJI,
        బిట్టు_EMOJI_PRESENTATION,
        బిట్టు_EMOJI_MODIFIER,
        బిట్టు_EMOJI_MODIFIER_పునాది,
        బిట్టు_EMOJI_COMPONENT,
        బిట్టు_EXTENDED_PICTOGRAPHIC,
        // https://www.unicode.org/reports/tr51/#Emoji_Sets
        బిట్టు_BASIC_EMOJI
    };

అంతరంగ:
    స్థిర UBool U_CALLCONV
    isAcceptable(శూన్య *context, స్థిరరాశి అక్షరం *type, స్థిరరాశి అక్షరం *name, స్థిరరాశి UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    స్థిర int32_t get_String_Trie_సూచీ(int32_t i) {
        తిరిగి i - IX_BASIC_EMOJI_TRIE_స్థానభ్రంశం;
    }

    శూన్య ఎక్కించు(UError_సంకేతం &error_సంకేతం);
    UBool has_ద్విమాన_Property_Impl(UChar32 c, UProperty which) స్థిరరాశి;
    UBool has_ద్విమాన_Property_Impl(స్థిరరాశి char16_t *s, int32_t నిడివి, UProperty which) స్థిరరాశి;

    UData_జ్ఞాపకం *జ్ఞాపకం = nullptr;
    UCPTrie *cpTrie = nullptr;
    స్థిరరాశి char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_చివర

#అయితే_ముగింపు  // __EMOJIPROPS_H__
