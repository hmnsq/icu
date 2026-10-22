// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.cpp
// created: 2021sep04 Markus W. Scherer

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/uchar.h"
#અન્તર્ભાવ "unicode/ucharstrie.h"
#અન્તર્ભાવ "unicode/ucptrie.h"
#અન્તર્ભાવ "unicode/udata.h"
#અન્તર્ભાવ "unicode/ustringtrie.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "emojiprops.h"
#અન્તર્ભાવ "ucln.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "uset_imp.h"

U_NAMESPACE_BEGIN

નામાકાશ {

EmojiProps *singleton = nullptr;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    delete singleton;
    singleton = nullptr;
    emojiInitOnce.reset();
    પાછા સાચું;
}

શૂન્ય U_CALLCONV initSingleton(UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા; }
    singleton = નવું EmojiProps(errorCode);
    જો (singleton == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    } નહીંતર જો (U_FAILURE(errorCode)) {
        delete singleton;
        singleton = nullptr;
    }
    ucln_common_registerCleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        અચલ UDataInfo &info, અચલ char16_t *dataFormat, uint8_t major, uint8_t minor) {
    પાછા
        info.size >= 20 &&
        info.isBigEndian == U_IS_BIG_ENDIAN &&
        info.charsetFamily == U_CHARSET_FAMILY &&
        info.dataFormat[0] == dataFormat[0] &&
        info.dataFormat[1] == dataFormat[1] &&
        info.dataFormat[2] == dataFormat[2] &&
        info.dataFormat[3] == dataFormat[3] &&
        info.formatVersion[0] == major &&
        info.formatVersion[1] >= minor;
}

}  // namespace

EmojiProps::~EmojiProps() {
    udata_close(memory);
    ucptrie_close(cpTrie);
}

અચલ EmojiProps *
EmojiProps::getSingleton(UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા nullptr; }
    umtx_initOnce(emojiInitOnce, &initSingleton, errorCode);
    પાછા singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(શૂન્ય * /*context*/, અચલ અક્ષર * /*type*/, અચલ અક્ષર * /*name*/,
                         અચલ UDataInfo *pInfo) {
    પાછા udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

શૂન્ય
EmojiProps::load(UErrorCode &errorCode) {
    memory = udata_openChoice(nullptr, "icu", "uemoji", isAcceptable, આ, &errorCode);
    જો (U_FAILURE(errorCode)) { પાછા; }
    અચલ uint8_t* inBytes = static_cast<અચલ uint8_t*>(udata_getMemory(memory));
    અચલ int32_t* inIndexes = reinterpret_cast<અચલ int32_t*>(inBytes);
    int32_t indexesLength = inIndexes[IX_CPTRIE_OFFSET] / 4;
    જો (indexesLength <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        પાછા;
    }

    int32_t i = IX_CPTRIE_OFFSET;
    int32_t offset = inIndexes[i++];
    int32_t nextOffset = inIndexes[i];
    cpTrie = ucptrie_open_From_દ્વિઅંકી(UCPTRIE_TYPE_FAST, UCPTRIE_કિંમત_BITS_8,
                                    inBytes + offset, nextOffset - offset, nullptr, &errorCode);
    જો (U_FAILURE(errorCode)) {
        પાછા;
    }

    માટે (i = IX_BASIC_EMOJI_TRIE_OFFSET; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET; ++i) {
        offset = inIndexes[i];
        nextOffset = inIndexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        અચલ char16_t* p = nextOffset > offset ? reinterpret_cast<અચલ char16_t*>(inBytes + offset) : nullptr;
        stringTries[getStringTrieIndex(i)] = p;
    }
}

શૂન્ય
EmojiProps::addPropertyStarts(અચલ USetAdder *sa, UErrorCode & /*errorCode*/) અચલ {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    જ્યારે ((end = ucptrie_getRange(cpTrie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

UBool
EmojiProps::has_દ્વિઅંકી_Property(UChar32 c, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    અચલ EmojiProps *ep = getSingleton(errorCode);
    પાછા U_SUCCESS(errorCode) && ep->has_દ્વિઅંકી_Property_Impl(c, which);
}

UBool
EmojiProps::has_દ્વિઅંકી_Property_Impl(UChar32 c, UProperty which) અચલ {
    જો (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        પાછા ખોટું;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    સ્થિર constexpr int8_t bitFlags[] = {
        BIT_EMOJI,                  // UCHAR_EMOJI=57
        BIT_EMOJI_PRESENTATION,     // UCHAR_EMOJI_PRESENTATION=58
        BIT_EMOJI_MODIFIER,         // UCHAR_EMOJI_MODIFIER=59
        BIT_EMOJI_MODIFIER_BASE,    // UCHAR_EMOJI_MODIFIER_BASE=60
        BIT_EMOJI_COMPONENT,        // UCHAR_EMOJI_COMPONENT=61
        -1,                         // UCHAR_REGIONAL_INDICATOR=62
        -1,                         // UCHAR_PREPENDED_CONCATENATION_MARK=63
        BIT_EXTENDED_PICTOGRAPHIC,  // UCHAR_EXTENDED_PICTOGRAPHIC=64
        BIT_BASIC_EMOJI,            // UCHAR_BASIC_EMOJI=65
        -1,                         // UCHAR_EMOJI_KEYCAP_SEQUENCE=66
        -1,                         // UCHAR_RGI_EMOJI_MODIFIER_SEQUENCE=67
        -1,                         // UCHAR_RGI_EMOJI_FLAG_SEQUENCE=68
        -1,                         // UCHAR_RGI_EMOJI_TAG_SEQUENCE=69
        -1,                         // UCHAR_RGI_EMOJI_ZWJ_SEQUENCE=70
        BIT_BASIC_EMOJI,            // UCHAR_RGI_EMOJI=71
    };
    int32_t bit = bitFlags[which - UCHAR_EMOJI];
    જો (bit < 0) {
        પાછા ખોટું;  // not a property that we support in this function
    }
    uint8_t bits = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    પાછા (bits >> bit) & 1;
}

UBool
EmojiProps::has_દ્વિઅંકી_Property(અચલ char16_t *s, int32_t length, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    અચલ EmojiProps *ep = getSingleton(errorCode);
    પાછા U_SUCCESS(errorCode) && ep->has_દ્વિઅંકી_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_દ્વિઅંકી_Property_Impl(અચલ char16_t *s, int32_t length, UProperty which) અચલ {
    જો (s == nullptr && length != 0) { પાછા ખોટું; }
    જો (length <= 0 && (length == 0 || *s == 0)) { પાછા ખોટું; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    જો (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        પાછા ખોટું;
    }
    UProperty firstProp = which, lastProp = which;
    જો (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    માટે (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        અચલ char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        જો (trieUChars != nullptr) {
            UCharsTrie trie(trieUChars);
            UStringTrieResult result = trie.next(s, length);
            જો (USTRINGTRIE_HAS_કિંમત(result)) {
                પાછા સાચું;
            }
        }
    }
    પાછા ખોટું;
}

શૂન્ય
EmojiProps::addStrings(અચલ USetAdder *sa, UProperty which, UErrorCode &errorCode) અચલ {
    જો (U_FAILURE(errorCode)) { પાછા; }
    જો (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        પાછા;
    }
    UProperty firstProp = which, lastProp = which;
    જો (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    માટે (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        અચલ char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        જો (trieUChars != nullptr) {
            UCharsTrie::Iterator iter(trieUChars, 0, errorCode);
            જ્યારે (iter.next(errorCode)) {
                અચલ UnicodeString &s = iter.getString();
                sa->addString(sa->set, s.getBuffer(), s.length());
            }
        }
    }
}

U_NAMESPACE_END
