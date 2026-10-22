// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.cpp
// created: 2021sep04 Markus W. Scherer

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/uchar.h"
#অন্তর্ভাব "unicode/ucharstrie.h"
#অন্তর্ভাব "unicode/ucptrie.h"
#অন্তর্ভাব "unicode/udata.h"
#অন্তর্ভাব "unicode/ustringtrie.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "emojiprops.h"
#অন্তর্ভাব "ucln.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "uset_imp.h"

U_NAMESPACE_BEGIN

নামাকাশ {

EmojiProps *singleton = nullptr;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    delete singleton;
    singleton = nullptr;
    emojiInitOnce.reset();
    প্রত্যাগম সত্য;
}

শূন্য U_CALLCONV initSingleton(UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
    singleton = নূতন EmojiProps(errorCode);
    যদি (singleton == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    } অন্যথা যদি (U_FAILURE(errorCode)) {
        delete singleton;
        singleton = nullptr;
    }
    ucln_common_registerCleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        ধ্রুব UDataInfo &info, ধ্রুব char16_t *dataFormat, uint8_t major, uint8_t minor) {
    প্রত্যাগম
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

ধ্রুব EmojiProps *
EmojiProps::getSingleton(UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম nullptr; }
    umtx_initOnce(emojiInitOnce, &initSingleton, errorCode);
    প্রত্যাগম singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(শূন্য * /*context*/, ধ্রুব বর্ণ * /*type*/, ধ্রুব বর্ণ * /*name*/,
                         ধ্রুব UDataInfo *pInfo) {
    প্রত্যাগম udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

শূন্য
EmojiProps::load(UErrorCode &errorCode) {
    memory = udata_openChoice(nullptr, "icu", "uemoji", isAcceptable, এতত্, &errorCode);
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
    ধ্রুব uint8_t* inBytes = static_cast<ধ্রুব uint8_t*>(udata_getMemory(memory));
    ধ্রুব int32_t* inIndexes = reinterpret_cast<ধ্রুব int32_t*>(inBytes);
    int32_t indexesLength = inIndexes[IX_CPTRIE_OFFSET] / 4;
    যদি (indexesLength <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        প্রত্যাগম;
    }

    int32_t i = IX_CPTRIE_OFFSET;
    int32_t offset = inIndexes[i++];
    int32_t nextOffset = inIndexes[i];
    cpTrie = ucptrie_open_From_দ্বিমিক(UCPTRIE_TYPE_FAST, UCPTRIE_মান_BITS_8,
                                    inBytes + offset, nextOffset - offset, nullptr, &errorCode);
    যদি (U_FAILURE(errorCode)) {
        প্রত্যাগম;
    }

    প্রতি (i = IX_BASIC_EMOJI_TRIE_OFFSET; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET; ++i) {
        offset = inIndexes[i];
        nextOffset = inIndexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        ধ্রুব char16_t* p = nextOffset > offset ? reinterpret_cast<ধ্রুব char16_t*>(inBytes + offset) : nullptr;
        stringTries[getStringTrieIndex(i)] = p;
    }
}

শূন্য
EmojiProps::addPropertyStarts(ধ্রুব USetAdder *sa, UErrorCode & /*errorCode*/) ধ্রুব {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    যাবত্ ((end = ucptrie_getRange(cpTrie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

UBool
EmojiProps::has_দ্বিমিক_Property(UChar32 c, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    ধ্রুব EmojiProps *ep = getSingleton(errorCode);
    প্রত্যাগম U_SUCCESS(errorCode) && ep->has_দ্বিমিক_Property_Impl(c, which);
}

UBool
EmojiProps::has_দ্বিমিক_Property_Impl(UChar32 c, UProperty which) ধ্রুব {
    যদি (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        প্রত্যাগম অসত্য;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    স্থির constexpr int8_t bitFlags[] = {
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
    যদি (bit < 0) {
        প্রত্যাগম অসত্য;  // not a property that we support in this function
    }
    uint8_t bits = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    প্রত্যাগম (bits >> bit) & 1;
}

UBool
EmojiProps::has_দ্বিমিক_Property(ধ্রুব char16_t *s, int32_t length, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    ধ্রুব EmojiProps *ep = getSingleton(errorCode);
    প্রত্যাগম U_SUCCESS(errorCode) && ep->has_দ্বিমিক_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_দ্বিমিক_Property_Impl(ধ্রুব char16_t *s, int32_t length, UProperty which) ধ্রুব {
    যদি (s == nullptr && length != 0) { প্রত্যাগম অসত্য; }
    যদি (length <= 0 && (length == 0 || *s == 0)) { প্রত্যাগম অসত্য; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    যদি (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        প্রত্যাগম অসত্য;
    }
    UProperty firstProp = which, lastProp = which;
    যদি (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    প্রতি (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        ধ্রুব char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        যদি (trieUChars != nullptr) {
            UCharsTrie trie(trieUChars);
            UStringTrieResult result = trie.next(s, length);
            যদি (USTRINGTRIE_HAS_মান(result)) {
                প্রত্যাগম সত্য;
            }
        }
    }
    প্রত্যাগম অসত্য;
}

শূন্য
EmojiProps::addStrings(ধ্রুব USetAdder *sa, UProperty which, UErrorCode &errorCode) ধ্রুব {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
    যদি (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        প্রত্যাগম;
    }
    UProperty firstProp = which, lastProp = which;
    যদি (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    প্রতি (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        ধ্রুব char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        যদি (trieUChars != nullptr) {
            UCharsTrie::Iterator iter(trieUChars, 0, errorCode);
            যাবত্ (iter.next(errorCode)) {
                ধ্রুব UnicodeString &s = iter.getString();
                sa->addString(sa->set, s.getBuffer(), s.length());
            }
        }
    }
}

U_NAMESPACE_END
