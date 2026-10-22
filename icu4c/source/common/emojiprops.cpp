// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.cpp
// created: 2021sep04 Markus W. Scherer

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/ucharstrie.h"
#അന്തര്ഭാവ "unicode/ucptrie.h"
#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "unicode/ustringtrie.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "emojiprops.h"
#അന്തര്ഭാവ "ucln.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "uset_imp.h"

U_NAMESPACE_BEGIN

നാമാകാശ {

EmojiProps *singleton = nullptr;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    delete singleton;
    singleton = nullptr;
    emojiInitOnce.reset();
    തിരികെ സത്യം;
}

ശൂന്യം U_CALLCONV initSingleton(UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
    singleton = പുതിയ EmojiProps(errorCode);
    എങ്കിൽ (singleton == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    } അല്ലെങ്കിൽ എങ്കിൽ (U_FAILURE(errorCode)) {
        delete singleton;
        singleton = nullptr;
    }
    ucln_common_registerCleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        സ്ഥിരം UDataInfo &info, സ്ഥിരം char16_t *dataFormat, uint8_t major, uint8_t minor) {
    തിരികെ
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

സ്ഥിരം EmojiProps *
EmojiProps::getSingleton(UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ nullptr; }
    umtx_initOnce(emojiInitOnce, &initSingleton, errorCode);
    തിരികെ singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(ശൂന്യം * /*context*/, സ്ഥിരം അക്ഷരം * /*type*/, സ്ഥിരം അക്ഷരം * /*name*/,
                         സ്ഥിരം UDataInfo *pInfo) {
    തിരികെ udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

ശൂന്യം
EmojiProps::load(UErrorCode &errorCode) {
    memory = udata_openChoice(nullptr, "icu", "uemoji", isAcceptable, ഇത്, &errorCode);
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
    സ്ഥിരം uint8_t* inBytes = static_cast<സ്ഥിരം uint8_t*>(udata_getMemory(memory));
    സ്ഥിരം int32_t* inIndexes = reinterpret_cast<സ്ഥിരം int32_t*>(inBytes);
    int32_t indexesLength = inIndexes[IX_CPTRIE_OFFSET] / 4;
    എങ്കിൽ (indexesLength <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        തിരികെ;
    }

    int32_t i = IX_CPTRIE_OFFSET;
    int32_t offset = inIndexes[i++];
    int32_t nextOffset = inIndexes[i];
    cpTrie = ucptrie_open_From_ദ്വിമാന(UCPTRIE_TYPE_FAST, UCPTRIE_മൂല്യം_BITS_8,
                                    inBytes + offset, nextOffset - offset, nullptr, &errorCode);
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ;
    }

    വേണ്ടി (i = IX_BASIC_EMOJI_TRIE_OFFSET; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET; ++i) {
        offset = inIndexes[i];
        nextOffset = inIndexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        സ്ഥിരം char16_t* p = nextOffset > offset ? reinterpret_cast<സ്ഥിരം char16_t*>(inBytes + offset) : nullptr;
        stringTries[getStringTrieIndex(i)] = p;
    }
}

ശൂന്യം
EmojiProps::addPropertyStarts(സ്ഥിരം USetAdder *sa, UErrorCode & /*errorCode*/) സ്ഥിരം {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    ആകുമ്പോൾ ((end = ucptrie_getRange(cpTrie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

UBool
EmojiProps::has_ദ്വിമാന_Property(UChar32 c, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    സ്ഥിരം EmojiProps *ep = getSingleton(errorCode);
    തിരികെ U_SUCCESS(errorCode) && ep->has_ദ്വിമാന_Property_Impl(c, which);
}

UBool
EmojiProps::has_ദ്വിമാന_Property_Impl(UChar32 c, UProperty which) സ്ഥിരം {
    എങ്കിൽ (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        തിരികെ അസത്യം;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    സ്ഥിതം constexpr int8_t bitFlags[] = {
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
    എങ്കിൽ (bit < 0) {
        തിരികെ അസത്യം;  // not a property that we support in this function
    }
    uint8_t bits = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    തിരികെ (bits >> bit) & 1;
}

UBool
EmojiProps::has_ദ്വിമാന_Property(സ്ഥിരം char16_t *s, int32_t length, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    സ്ഥിരം EmojiProps *ep = getSingleton(errorCode);
    തിരികെ U_SUCCESS(errorCode) && ep->has_ദ്വിമാന_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_ദ്വിമാന_Property_Impl(സ്ഥിരം char16_t *s, int32_t length, UProperty which) സ്ഥിരം {
    എങ്കിൽ (s == nullptr && length != 0) { തിരികെ അസത്യം; }
    എങ്കിൽ (length <= 0 && (length == 0 || *s == 0)) { തിരികെ അസത്യം; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    എങ്കിൽ (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        തിരികെ അസത്യം;
    }
    UProperty firstProp = which, lastProp = which;
    എങ്കിൽ (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    വേണ്ടി (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        സ്ഥിരം char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        എങ്കിൽ (trieUChars != nullptr) {
            UCharsTrie trie(trieUChars);
            UStringTrieResult result = trie.next(s, length);
            എങ്കിൽ (USTRINGTRIE_HAS_മൂല്യം(result)) {
                തിരികെ സത്യം;
            }
        }
    }
    തിരികെ അസത്യം;
}

ശൂന്യം
EmojiProps::addStrings(സ്ഥിരം USetAdder *sa, UProperty which, UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
    എങ്കിൽ (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        തിരികെ;
    }
    UProperty firstProp = which, lastProp = which;
    എങ്കിൽ (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    വേണ്ടി (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        സ്ഥിരം char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        എങ്കിൽ (trieUChars != nullptr) {
            UCharsTrie::Iterator iter(trieUChars, 0, errorCode);
            ആകുമ്പോൾ (iter.next(errorCode)) {
                സ്ഥിരം UnicodeString &s = iter.getString();
                sa->addString(sa->set, s.getBuffer(), s.length());
            }
        }
    }
}

U_NAMESPACE_END
