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
    ফেরত সত্য;
}

শূন্য U_CALLCONV initSingleton(UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ফেরত; }
    singleton = নতুন EmojiProps(errorCode);
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
        ধ্রুবক UDataInfo &info, ধ্রুবক char16_t *তথ্য_Format, uint8_t major, uint8_t minor) {
    ফেরত
        info.size >= 20 &&
        info.isBigEndian == U_IS_BIG_ENDIAN &&
        info.charsetFamily == U_CHARSET_FAMILY &&
        info.তথ্য_Format[0] == তথ্য_Format[0] &&
        info.তথ্য_Format[1] == তথ্য_Format[1] &&
        info.তথ্য_Format[2] == তথ্য_Format[2] &&
        info.তথ্য_Format[3] == তথ্য_Format[3] &&
        info.formatVersion[0] == major &&
        info.formatVersion[1] >= minor;
}

}  // namespace

EmojiProps::~EmojiProps() {
    udata_close(memory);
    ucptrie_close(cpTrie);
}

ধ্রুবক EmojiProps *
EmojiProps::getSingleton(UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ফেরত nullptr; }
    umtx_initOnce(emojiInitOnce, &initSingleton, errorCode);
    ফেরত singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(শূন্য * /*context*/, ধ্রুবক অক্ষর * /*type*/, ধ্রুবক অক্ষর * /*name*/,
                         ধ্রুবক UDataInfo *pInfo) {
    ফেরত udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

শূন্য
EmojiProps::load(UErrorCode &errorCode) {
    memory = udata_openChoice(nullptr, "icu", "uemoji", isAcceptable, এই, &errorCode);
    যদি (U_FAILURE(errorCode)) { ফেরত; }
    ধ্রুবক uint8_t* inBytes = static_cast<ধ্রুবক uint8_t*>(udata_getMemory(memory));
    ধ্রুবক int32_t* inIndexes = reinterpret_cast<ধ্রুবক int32_t*>(inBytes);
    int32_t indexes_দৈর্ঘ্য = inIndexes[IX_CPTRIE_OFFSET] / 4;
    যদি (indexes_দৈর্ঘ্য <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ফেরত;
    }

    int32_t i = IX_CPTRIE_OFFSET;
    int32_t offset = inIndexes[i++];
    int32_t পরবর্তী_Offset = inIndexes[i];
    cpTrie = ucptrie_open_From_দ্বিমিক(UCPTRIE_TYPE_FAST, UCPTRIE_মান_BITS_8,
                                    inBytes + offset, পরবর্তী_Offset - offset, nullptr, &errorCode);
    যদি (U_FAILURE(errorCode)) {
        ফেরত;
    }

    জন্য (i = IX_BASIC_EMOJI_TRIE_OFFSET; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET; ++i) {
        offset = inIndexes[i];
        পরবর্তী_Offset = inIndexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        ধ্রুবক char16_t* p = পরবর্তী_Offset > offset ? reinterpret_cast<ধ্রুবক char16_t*>(inBytes + offset) : nullptr;
        stringTries[get_String_Trie_সূচক(i)] = p;
    }
}

শূন্য
EmojiProps::addPropertyStarts(ধ্রুবক USetAdder *sa, UErrorCode & /*errorCode*/) ধ্রুবক {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    যখন ((end = ucptrie_getRange(cpTrie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

UBool
EmojiProps::has_দ্বিমিক_Property(UChar32 c, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    ধ্রুবক EmojiProps *ep = getSingleton(errorCode);
    ফেরত U_SUCCESS(errorCode) && ep->has_দ্বিমিক_Property_Impl(c, which);
}

UBool
EmojiProps::has_দ্বিমিক_Property_Impl(UChar32 c, UProperty which) ধ্রুবক {
    যদি (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        ফেরত মিথ্যা;
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
        ফেরত মিথ্যা;  // not a property that we support in this function
    }
    uint8_t bits = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    ফেরত (bits >> bit) & 1;
}

UBool
EmojiProps::has_দ্বিমিক_Property(ধ্রুবক char16_t *s, int32_t length, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    ধ্রুবক EmojiProps *ep = getSingleton(errorCode);
    ফেরত U_SUCCESS(errorCode) && ep->has_দ্বিমিক_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_দ্বিমিক_Property_Impl(ধ্রুবক char16_t *s, int32_t length, UProperty which) ধ্রুবক {
    যদি (s == nullptr && length != 0) { ফেরত মিথ্যা; }
    যদি (length <= 0 && (length == 0 || *s == 0)) { ফেরত মিথ্যা; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    যদি (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        ফেরত মিথ্যা;
    }
    UProperty firstProp = which, lastProp = which;
    যদি (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    জন্য (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        ধ্রুবক char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        যদি (trieUChars != nullptr) {
            UCharsTrie trie(trieUChars);
            UString_Trie_ফলাফল ফলাফল = trie.next(s, length);
            যদি (USTRINGTRIE_HAS_মান(ফলাফল)) {
                ফেরত সত্য;
            }
        }
    }
    ফেরত মিথ্যা;
}

শূন্য
EmojiProps::addStrings(ধ্রুবক USetAdder *sa, UProperty which, UErrorCode &errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) { ফেরত; }
    যদি (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        ফেরত;
    }
    UProperty firstProp = which, lastProp = which;
    যদি (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    জন্য (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        ধ্রুবক char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        যদি (trieUChars != nullptr) {
            UCharsTrie::Iterator iter(trieUChars, 0, errorCode);
            যখন (iter.next(errorCode)) {
                ধ্রুবক UnicodeString &s = iter.getString();
                sa->addString(sa->set, s.get_বাফার(), s.length());
            }
        }
    }
}

U_NAMESPACE_END
