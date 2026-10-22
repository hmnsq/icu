// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.cpp
// created: 2021sep04 Markus W. Scherer

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/uchar.h"
#அந்தர்பாவ "unicode/ucharstrie.h"
#அந்தர்பாவ "unicode/ucptrie.h"
#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "unicode/ustringtrie.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "emojiprops.h"
#அந்தர்பாவ "ucln.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "uset_imp.h"

U_NAMESPACE_BEGIN

நாமாகாஶ {

EmojiProps *singleton = nullptr;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    delete singleton;
    singleton = nullptr;
    emojiInitOnce.reset();
    திரும்பு உண்மை;
}

வெறுமை U_CALLCONV initSingleton(UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    singleton = புதிய EmojiProps(errorCode);
    எனில் (singleton == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    } இல்லையெனில் எனில் (U_FAILURE(errorCode)) {
        delete singleton;
        singleton = nullptr;
    }
    ucln_common_registerCleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        மாறிலி UDataInfo &info, மாறிலி char16_t *dataFormat, uint8_t major, uint8_t minor) {
    திரும்பு
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

மாறிலி EmojiProps *
EmojiProps::getSingleton(UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு nullptr; }
    umtx_initOnce(emojiInitOnce, &initSingleton, errorCode);
    திரும்பு singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(வெறுமை * /*context*/, மாறிலி எழுத்து * /*type*/, மாறிலி எழுத்து * /*name*/,
                         மாறிலி UDataInfo *pInfo) {
    திரும்பு udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

வெறுமை
EmojiProps::load(UErrorCode &errorCode) {
    memory = udata_openChoice(nullptr, "icu", "uemoji", isAcceptable, இது, &errorCode);
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    மாறிலி uint8_t* inBytes = static_cast<மாறிலி uint8_t*>(udata_getMemory(memory));
    மாறிலி int32_t* inIndexes = reinterpret_cast<மாறிலி int32_t*>(inBytes);
    int32_t indexesLength = inIndexes[IX_CPTRIE_OFFSET] / 4;
    எனில் (indexesLength <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        திரும்பு;
    }

    int32_t i = IX_CPTRIE_OFFSET;
    int32_t offset = inIndexes[i++];
    int32_t nextOffset = inIndexes[i];
    cpTrie = ucptrie_open_From_இருமம்(UCPTRIE_TYPE_FAST, UCPTRIE_மதிப்பு_BITS_8,
                                    inBytes + offset, nextOffset - offset, nullptr, &errorCode);
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு;
    }

    க்கு (i = IX_BASIC_EMOJI_TRIE_OFFSET; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET; ++i) {
        offset = inIndexes[i];
        nextOffset = inIndexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        மாறிலி char16_t* p = nextOffset > offset ? reinterpret_cast<மாறிலி char16_t*>(inBytes + offset) : nullptr;
        stringTries[getStringTrieIndex(i)] = p;
    }
}

வெறுமை
EmojiProps::addPropertyStarts(மாறிலி USetAdder *sa, UErrorCode & /*errorCode*/) மாறிலி {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    போது ((end = ucptrie_getRange(cpTrie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

UBool
EmojiProps::has_இருமம்_Property(UChar32 c, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    மாறிலி EmojiProps *ep = getSingleton(errorCode);
    திரும்பு U_SUCCESS(errorCode) && ep->has_இருமம்_Property_Impl(c, which);
}

UBool
EmojiProps::has_இருமம்_Property_Impl(UChar32 c, UProperty which) மாறிலி {
    எனில் (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        திரும்பு பொய்;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    நிலையான constexpr int8_t bitFlags[] = {
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
    எனில் (bit < 0) {
        திரும்பு பொய்;  // not a property that we support in this function
    }
    uint8_t bits = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    திரும்பு (bits >> bit) & 1;
}

UBool
EmojiProps::has_இருமம்_Property(மாறிலி char16_t *s, int32_t length, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    மாறிலி EmojiProps *ep = getSingleton(errorCode);
    திரும்பு U_SUCCESS(errorCode) && ep->has_இருமம்_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_இருமம்_Property_Impl(மாறிலி char16_t *s, int32_t length, UProperty which) மாறிலி {
    எனில் (s == nullptr && length != 0) { திரும்பு பொய்; }
    எனில் (length <= 0 && (length == 0 || *s == 0)) { திரும்பு பொய்; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    எனில் (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        திரும்பு பொய்;
    }
    UProperty firstProp = which, lastProp = which;
    எனில் (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    க்கு (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        மாறிலி char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        எனில் (trieUChars != nullptr) {
            UCharsTrie trie(trieUChars);
            UStringTrieResult result = trie.next(s, length);
            எனில் (USTRINGTRIE_HAS_மதிப்பு(result)) {
                திரும்பு உண்மை;
            }
        }
    }
    திரும்பு பொய்;
}

வெறுமை
EmojiProps::addStrings(மாறிலி USetAdder *sa, UProperty which, UErrorCode &errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    எனில் (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        திரும்பு;
    }
    UProperty firstProp = which, lastProp = which;
    எனில் (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    க்கு (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        மாறிலி char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        எனில் (trieUChars != nullptr) {
            UCharsTrie::Iterator iter(trieUChars, 0, errorCode);
            போது (iter.next(errorCode)) {
                மாறிலி UnicodeString &s = iter.getString();
                sa->addString(sa->set, s.getBuffer(), s.length());
            }
        }
    }
}

U_NAMESPACE_END
