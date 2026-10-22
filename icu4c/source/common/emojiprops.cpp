// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.cpp
// created: 2021sep04 Markus W. Scherer

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucharstrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustringtrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "emojiprops.h"
#ಅನ್ತರ್ಭಾವ "ucln.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "uset_imp.h"

U_NAMESPACE_BEGIN

ನಾಮಾಕಾಶ {

EmojiProps *singleton = nullptr;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    delete singleton;
    singleton = nullptr;
    emojiInitOnce.reset();
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಶೂನ್ಯ U_CALLCONV initSingleton(UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    singleton = ನೂತನ EmojiProps(errorCode);
    ಯದಿ (singleton == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    } ಅನ್ಯಥಾ ಯದಿ (U_FAILURE(errorCode)) {
        delete singleton;
        singleton = nullptr;
    }
    ucln_common_registerCleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        ಧ್ರುವ UDataInfo &info, ಧ್ರುವ char16_t *dataFormat, uint8_t major, uint8_t minor) {
    ಪ್ರತ್ಯಾಗಮ
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

ಧ್ರುವ EmojiProps *
EmojiProps::getSingleton(UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    umtx_initOnce(emojiInitOnce, &initSingleton, errorCode);
    ಪ್ರತ್ಯಾಗಮ singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(ಶೂನ್ಯ * /*context*/, ಧ್ರುವ ವರ್ಣ * /*type*/, ಧ್ರುವ ವರ್ಣ * /*name*/,
                         ಧ್ರುವ UDataInfo *pInfo) {
    ಪ್ರತ್ಯಾಗಮ udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

ಶೂನ್ಯ
EmojiProps::load(UErrorCode &errorCode) {
    memory = udata_openChoice(nullptr, "icu", "uemoji", isAcceptable, ಏತತ್, &errorCode);
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    ಧ್ರುವ uint8_t* inBytes = static_cast<ಧ್ರುವ uint8_t*>(udata_getMemory(memory));
    ಧ್ರುವ int32_t* inIndexes = reinterpret_cast<ಧ್ರುವ int32_t*>(inBytes);
    int32_t indexesLength = inIndexes[IX_CPTRIE_OFFSET] / 4;
    ಯದಿ (indexesLength <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ಪ್ರತ್ಯಾಗಮ;
    }

    int32_t i = IX_CPTRIE_OFFSET;
    int32_t offset = inIndexes[i++];
    int32_t nextOffset = inIndexes[i];
    cpTrie = ucptrie_open_From_ದ್ವಿಮಾನ(UCPTRIE_TYPE_FAST, UCPTRIE_ಮೌಲ್ಯ_BITS_8,
                                    inBytes + offset, nextOffset - offset, nullptr, &errorCode);
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (i = IX_BASIC_EMOJI_TRIE_OFFSET; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET; ++i) {
        offset = inIndexes[i];
        nextOffset = inIndexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        ಧ್ರುವ char16_t* p = nextOffset > offset ? reinterpret_cast<ಧ್ರುವ char16_t*>(inBytes + offset) : nullptr;
        stringTries[getStringTrieIndex(i)] = p;
    }
}

ಶೂನ್ಯ
EmojiProps::addPropertyStarts(ಧ್ರುವ USetAdder *sa, UErrorCode & /*errorCode*/) ಧ್ರುವ {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    ಯಾವತ್ ((end = ucptrie_getRange(cpTrie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

UBool
EmojiProps::has_ದ್ವಿಮಾನ_Property(UChar32 c, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    ಧ್ರುವ EmojiProps *ep = getSingleton(errorCode);
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(errorCode) && ep->has_ದ್ವಿಮಾನ_Property_Impl(c, which);
}

UBool
EmojiProps::has_ದ್ವಿಮಾನ_Property_Impl(UChar32 c, UProperty which) ಧ್ರುವ {
    ಯದಿ (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    ಸ್ಥಿರ constexpr int8_t bitFlags[] = {
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
    ಯದಿ (bit < 0) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;  // not a property that we support in this function
    }
    uint8_t bits = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    ಪ್ರತ್ಯಾಗಮ (bits >> bit) & 1;
}

UBool
EmojiProps::has_ದ್ವಿಮಾನ_Property(ಧ್ರುವ char16_t *s, int32_t length, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    ಧ್ರುವ EmojiProps *ep = getSingleton(errorCode);
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(errorCode) && ep->has_ದ್ವಿಮಾನ_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_ದ್ವಿಮಾನ_Property_Impl(ಧ್ರುವ char16_t *s, int32_t length, UProperty which) ಧ್ರುವ {
    ಯದಿ (s == nullptr && length != 0) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    ಯದಿ (length <= 0 && (length == 0 || *s == 0)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    ಯದಿ (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    UProperty firstProp = which, lastProp = which;
    ಯದಿ (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    ಪ್ರತಿ (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        ಧ್ರುವ char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        ಯದಿ (trieUChars != nullptr) {
            UCharsTrie trie(trieUChars);
            UStringTrieResult result = trie.next(s, length);
            ಯದಿ (USTRINGTRIE_HAS_ಮೌಲ್ಯ(result)) {
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಶೂನ್ಯ
EmojiProps::addStrings(ಧ್ರುವ USetAdder *sa, UProperty which, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    ಯದಿ (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    UProperty firstProp = which, lastProp = which;
    ಯದಿ (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    ಪ್ರತಿ (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        ಧ್ರುವ char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        ಯದಿ (trieUChars != nullptr) {
            UCharsTrie::Iterator iter(trieUChars, 0, errorCode);
            ಯಾವತ್ (iter.next(errorCode)) {
                ಧ್ರುವ UnicodeString &s = iter.getString();
                sa->addString(sa->set, s.getBuffer(), s.length());
            }
        }
    }
}

U_NAMESPACE_END
