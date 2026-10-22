// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.cpp
// created: 2021sep04 Markus W. Scherer

#ضمن "unicode/utypes.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/ucharstrie.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/ustringtrie.h"
#ضمن "unicode/utf16.h"
#ضمن "emojiprops.h"
#ضمن "ucln.h"
#ضمن "ucln_cmn.h"
#ضمن "umutex.h"
#ضمن "uset_imp.h"

U_NAMESPACE_BEGIN

نطاق {

EmojiProps *singleton = nullptr;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    delete singleton;
    singleton = nullptr;
    emojiInitOnce.reset();
    أرجع true;
}

فراغ U_CALLCONV initSingleton(UErrorCode &errorCode) {
    إذا (U_FAILURE(errorCode)) { أرجع; }
    singleton = جديد EmojiProps(errorCode);
    إذا (singleton == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    } وإلا إذا (U_FAILURE(errorCode)) {
        delete singleton;
        singleton = nullptr;
    }
    ucln_common_registerCleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        ثابت UDataInfo &info, ثابت char16_t *dataFormat, uint8_t major, uint8_t minor) {
    أرجع
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

ثابت EmojiProps *
EmojiProps::getSingleton(UErrorCode &errorCode) {
    إذا (U_FAILURE(errorCode)) { أرجع nullptr; }
    umtx_initOnce(emojiInitOnce, &initSingleton, errorCode);
    أرجع singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(فراغ * /*context*/, ثابت حرف * /*type*/, ثابت حرف * /*name*/,
                         ثابت UDataInfo *pInfo) {
    أرجع udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

فراغ
EmojiProps::load(UErrorCode &errorCode) {
    memory = udata_openChoice(nullptr, "icu", "uemoji", isAcceptable, هذا, &errorCode);
    إذا (U_FAILURE(errorCode)) { أرجع; }
    ثابت uint8_t* inBytes = static_cast<ثابت uint8_t*>(udata_getMemory(memory));
    ثابت int32_t* inIndexes = reinterpret_cast<ثابت int32_t*>(inBytes);
    int32_t indexesLength = inIndexes[IX_CPTRIE_OFFSET] / 4;
    إذا (indexesLength <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        أرجع;
    }

    int32_t i = IX_CPTRIE_OFFSET;
    int32_t offset = inIndexes[i++];
    int32_t nextOffset = inIndexes[i];
    cpTrie = ucptrie_open_From_ثنائی(UCPTRIE_TYPE_FAST, UCPTRIE_قدر_BITS_8,
                                    inBytes + offset, nextOffset - offset, nullptr, &errorCode);
    إذا (U_FAILURE(errorCode)) {
        أرجع;
    }

    لكل (i = IX_BASIC_EMOJI_TRIE_OFFSET; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET; ++i) {
        offset = inIndexes[i];
        nextOffset = inIndexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        ثابت char16_t* p = nextOffset > offset ? reinterpret_cast<ثابت char16_t*>(inBytes + offset) : nullptr;
        stringTries[getStringTrieIndex(i)] = p;
    }
}

فراغ
EmojiProps::addPropertyStarts(ثابت USetAdder *sa, UErrorCode & /*errorCode*/) ثابت {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    طالما ((end = ucptrie_getRange(cpTrie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

UBool
EmojiProps::has_ثنائی_Property(UChar32 c, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    ثابت EmojiProps *ep = getSingleton(errorCode);
    أرجع U_SUCCESS(errorCode) && ep->has_ثنائی_Property_Impl(c, which);
}

UBool
EmojiProps::has_ثنائی_Property_Impl(UChar32 c, UProperty which) ثابت {
    إذا (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        أرجع خطأ;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    ساكن constexpr int8_t bitFlags[] = {
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
    إذا (bit < 0) {
        أرجع خطأ;  // not a property that we support in this function
    }
    uint8_t bits = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    أرجع (bits >> bit) & 1;
}

UBool
EmojiProps::has_ثنائی_Property(ثابت char16_t *s, int32_t length, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    ثابت EmojiProps *ep = getSingleton(errorCode);
    أرجع U_SUCCESS(errorCode) && ep->has_ثنائی_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_ثنائی_Property_Impl(ثابت char16_t *s, int32_t length, UProperty which) ثابت {
    إذا (s == nullptr && length != 0) { أرجع خطأ; }
    إذا (length <= 0 && (length == 0 || *s == 0)) { أرجع خطأ; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    إذا (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        أرجع خطأ;
    }
    UProperty firstProp = which, lastProp = which;
    إذا (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    لكل (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        ثابت char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        إذا (trieUChars != nullptr) {
            UCharsTrie trie(trieUChars);
            UStringTrieResult result = trie.next(s, length);
            إذا (USTRINGTRIE_HAS_قدر(result)) {
                أرجع true;
            }
        }
    }
    أرجع خطأ;
}

فراغ
EmojiProps::addStrings(ثابت USetAdder *sa, UProperty which, UErrorCode &errorCode) ثابت {
    إذا (U_FAILURE(errorCode)) { أرجع; }
    إذا (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        أرجع;
    }
    UProperty firstProp = which, lastProp = which;
    إذا (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    لكل (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        ثابت char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        إذا (trieUChars != nullptr) {
            UCharsTrie::Iterator iter(trieUChars, 0, errorCode);
            طالما (iter.next(errorCode)) {
                ثابت UnicodeString &s = iter.getString();
                sa->addString(sa->set, s.getBuffer(), s.length());
            }
        }
    }
}

U_NAMESPACE_END
