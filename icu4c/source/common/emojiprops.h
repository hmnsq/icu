// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.h
// created: 2021sep03 Markus W. Scherer

#ifndef __EMOJIPROPS_H__
#define __EMOJIPROPS_H__

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucptrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "uset_imp.h"

U_NAMESPACE_BEGIN

class EmojiProps : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
    // @internal
    EmojiProps(UErrorCode &errorCode) { load(errorCode); }
    ~EmojiProps();

    ସ୍ଥିର ଅଚଳ EmojiProps *getSingleton(UErrorCode &errorCode);
    ସ୍ଥିର UBool has_ଦ୍ୱିମିକ_Property(UChar32 c, UProperty which);
    ସ୍ଥିର UBool has_ଦ୍ୱିମିକ_Property(ଅଚଳ char16_t *s, int32_t length, UProperty which);

    ଶୂନ୍ୟ addPropertyStarts(ଅଚଳ USetAdder *sa, UErrorCode &errorCode) ଅଚଳ;
    ଶୂନ୍ୟ addStrings(ଅଚଳ USetAdder *sa, UProperty which, UErrorCode &errorCode) ଅଚଳ;

    ଗଣନା {
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
        IX_ଗଣନା  // 16
    };

    // Properties in the code point trie.
    ଗଣନା {
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

ବ୍ୟକ୍ତିଗତ:
    ସ୍ଥିର UBool U_CALLCONV
    isAcceptable(ଶୂନ୍ୟ *context, ଅଚଳ ଅକ୍ଷର *type, ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ UDataInfo *pInfo);
    /** Input i: One of the IX_..._TRIE_OFFSET indexes into the data file indexes[] array. */
    ସ୍ଥିର int32_t getStringTrieIndex(int32_t i) {
        ଫେରନ୍ତୁ i - IX_BASIC_EMOJI_TRIE_OFFSET;
    }

    ଶୂନ୍ୟ load(UErrorCode &errorCode);
    UBool has_ଦ୍ୱିମିକ_Property_Impl(UChar32 c, UProperty which) ଅଚଳ;
    UBool has_ଦ୍ୱିମିକ_Property_Impl(ଅଚଳ char16_t *s, int32_t length, UProperty which) ଅଚଳ;

    UDataMemory *memory = nullptr;
    UCPTrie *cpTrie = nullptr;
    ଅଚଳ char16_t *stringTries[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

U_NAMESPACE_END

#endif  // __EMOJIPROPS_H__
