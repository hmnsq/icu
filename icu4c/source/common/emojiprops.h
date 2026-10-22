// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __EMOJIPROPS_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __EMOJIPROPS_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    // @internal
    EmojiProps(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) { ತುಂಬು(error_ಸಂಕೇತ); }
    ~EmojiProps();

    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ EmojiProps *getSingleton(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    ಸ್ಥಿರ UBool has_ದ್ವಿಮಾನ_Property(UChar32 c, UProperty which);
    ಸ್ಥಿರ UBool has_ದ್ವಿಮಾನ_Property(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t ಉದ್ದಳತೆ, UProperty which);

    ಶೂನ್ಯ ಕೂಡಿಸು_Property_Starts(ಸ್ಥಿರಾಂಕ USetAdder *sa, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;
    ಶೂನ್ಯ ಕೂಡಿಸು_ಅಕ್ಷರಸರಣಿಗಳು(ಸ್ಥಿರಾಂಕ USetAdder *sa, UProperty which, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

    ಎಣಿಕೆ {
        // Byte offsets from the start of the data, after the generic header,
        // in ascending order.
        // UCPTrie=CodePointTrie, follows the indexes
        IX_CPTRIE_ಸ್ಥಾನಾಂತರ,
        IX_RESERVED1,
        IX_RESERVED2,
        IX_RESERVED3,

        // UCharsTrie=CharsTrie
        IX_BASIC_EMOJI_TRIE_ಸ್ಥಾನಾಂತರ,
        IX_EMOJI_KEYCAP_SEQUENCE_TRIE_ಸ್ಥಾನಾಂತರ,
        IX_RGI_EMOJI_MODIFIER_SEQUENCE_TRIE_ಸ್ಥಾನಾಂತರ,
        IX_RGI_EMOJI_FLAG_SEQUENCE_TRIE_ಸ್ಥಾನಾಂತರ,
        IX_RGI_EMOJI_ಟ್ಯಾಗ್_SEQUENCE_TRIE_ಸ್ಥಾನಾಂತರ,
        IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_ಸ್ಥಾನಾಂತರ,
        IX_RESERVED10,
        IX_RESERVED11,
        IX_RESERVED12,
        IX_ಒಟ್ಟು_ಗಾತ್ರ,

        // Not initially byte offsets.
        IX_RESERVED14,
        IX_RESERVED15,
        IX_ಎಣಿಕೆ  // 16
    };

    // Properties in the code point trie.
    ಎಣಿಕೆ {
        // https://www.unicode.org/reports/tr51/#Emoji_Properties
        ಬಿಟ್_EMOJI,
        ಬಿಟ್_EMOJI_PRESENTATION,
        ಬಿಟ್_EMOJI_MODIFIER,
        ಬಿಟ್_EMOJI_MODIFIER_ತಳ,
        ಬಿಟ್_EMOJI_COMPONENT,
        ಬಿಟ್_EXTENDED_PICTOGRAPHIC,
        // https://www.unicode.org/reports/tr51/#Emoji_Sets
        ಬಿಟ್_BASIC_EMOJI
    };

ಖಾಸಗಿ:
    ಸ್ಥಿರ UBool U_CALLCONV
    isAcceptable(ಶೂನ್ಯ *context, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    ಸ್ಥಿರ int32_t get_String_Trie_ಸೂಚ್ಯಂಕ(int32_t i) {
        ಹಿಂತಿರುಗಿ i - IX_BASIC_EMOJI_TRIE_ಸ್ಥಾನಾಂತರ;
    }

    ಶೂನ್ಯ ತುಂಬು(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    UBool has_ದ್ವಿಮಾನ_Property_Impl(UChar32 c, UProperty which) ಸ್ಥಿರಾಂಕ;
    UBool has_ದ್ವಿಮಾನ_Property_Impl(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t ಉದ್ದಳತೆ, UProperty which) ಸ್ಥಿರಾಂಕ;

    UData_ಸ್ಮರಣೆ *ಸ್ಮರಣೆ = nullptr;
    UCPTrie *cpTrie = nullptr;
    ಸ್ಥಿರಾಂಕ char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  // __EMOJIPROPS_H__
