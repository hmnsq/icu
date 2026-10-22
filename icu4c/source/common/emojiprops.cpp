// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.cpp
// created: 2021sep04 Markus W. Scherer

#समावेश "unicode/utypes.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/ucharstrie.h"
#समावेश "unicode/ucptrie.h"
#समावेश "unicode/udata.h"
#समावेश "unicode/ustringtrie.h"
#समावेश "unicode/utf16.h"
#समावेश "emojiprops.h"
#समावेश "ucln.h"
#समावेश "ucln_cmn.h"
#समावेश "umutex.h"
#समावेश "uset_imp.h"

U_NAMESPACE_BEGIN

namespace {

EmojiProps *singleton = nullptr;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    मिटाओ singleton;
    singleton = nullptr;
    emojiInitOnce.reset();
    वापस true;
}

व्योम U_CALLCONV initSingleton(UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस; }
    singleton = नया EmojiProps(errorCode);
    अगर (singleton == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा अगर (U_FAILURE(errorCode)) {
        मिटाओ singleton;
        singleton = nullptr;
    }
    ucln_common_registerCleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        स्थिर UDataInfo &info, स्थिर char16_t *तथ्य_Format, uint8_t major, uint8_t minor) {
    वापस
        info.size >= 20 &&
        info.isBigEndian == U_IS_BIG_ENDIAN &&
        info.charsetFamily == U_CHARSET_FAMILY &&
        info.तथ्य_Format[0] == तथ्य_Format[0] &&
        info.तथ्य_Format[1] == तथ्य_Format[1] &&
        info.तथ्य_Format[2] == तथ्य_Format[2] &&
        info.तथ्य_Format[3] == तथ्य_Format[3] &&
        info.formatVersion[0] == major &&
        info.formatVersion[1] >= minor;
}

}  // namespace

EmojiProps::~EmojiProps() {
    udata_close(memory);
    ucptrie_close(cpTrie);
}

स्थिर EmojiProps *
EmojiProps::getSingleton(UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस nullptr; }
    umtx_initOnce(emojiInitOnce, &initSingleton, errorCode);
    वापस singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(व्योम * /*context*/, स्थिर अक्षर * /*type*/, स्थिर अक्षर * /*name*/,
                         स्थिर UDataInfo *pInfo) {
    वापस udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

व्योम
EmojiProps::load(UErrorCode &errorCode) {
    memory = udata_openChoice(nullptr, "icu", "uemoji", isAcceptable, यह, &errorCode);
    अगर (U_FAILURE(errorCode)) { वापस; }
    स्थिर uint8_t* inBytes = static_cast<स्थिर uint8_t*>(udata_getMemory(memory));
    स्थिर int32_t* inIndexes = reinterpret_cast<स्थिर int32_t*>(inBytes);
    int32_t indexes_लंबाई = inIndexes[IX_CPTRIE_OFFSET] / 4;
    अगर (indexes_लंबाई <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        वापस;
    }

    int32_t i = IX_CPTRIE_OFFSET;
    int32_t offset = inIndexes[i++];
    int32_t अग्र_Offset = inIndexes[i];
    cpTrie = ucptrie_open_From_द्विआधारी(UCPTRIE_TYPE_FAST, UCPTRIE_मूल्य_BITS_8,
                                    inBytes + offset, अग्र_Offset - offset, nullptr, &errorCode);
    अगर (U_FAILURE(errorCode)) {
        वापस;
    }

    क्रम (i = IX_BASIC_EMOJI_TRIE_OFFSET; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET; ++i) {
        offset = inIndexes[i];
        अग्र_Offset = inIndexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        स्थिर char16_t* p = अग्र_Offset > offset ? reinterpret_cast<स्थिर char16_t*>(inBytes + offset) : nullptr;
        stringTries[get_String_Trie_सूचकांक(i)] = p;
    }
}

व्योम
EmojiProps::addPropertyStarts(स्थिर USetAdder *sa, UErrorCode & /*errorCode*/) स्थिर {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    जबतक ((end = ucptrie_getRange(cpTrie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

UBool
EmojiProps::has_द्विआधारी_Property(UChar32 c, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    स्थिर EmojiProps *ep = getSingleton(errorCode);
    वापस U_SUCCESS(errorCode) && ep->has_द्विआधारी_Property_Impl(c, which);
}

UBool
EmojiProps::has_द्विआधारी_Property_Impl(UChar32 c, UProperty which) स्थिर {
    अगर (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        वापस false;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    अटल constexpr int8_t bitFlags[] = {
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
    अगर (bit < 0) {
        वापस false;  // not a property that we support in this function
    }
    uint8_t bits = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    वापस (bits >> bit) & 1;
}

UBool
EmojiProps::has_द्विआधारी_Property(स्थिर char16_t *s, int32_t length, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    स्थिर EmojiProps *ep = getSingleton(errorCode);
    वापस U_SUCCESS(errorCode) && ep->has_द्विआधारी_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_द्विआधारी_Property_Impl(स्थिर char16_t *s, int32_t length, UProperty which) स्थिर {
    अगर (s == nullptr && length != 0) { वापस false; }
    अगर (length <= 0 && (length == 0 || *s == 0)) { वापस false; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    अगर (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        वापस false;
    }
    UProperty firstProp = which, lastProp = which;
    अगर (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    क्रम (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        स्थिर char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        अगर (trieUChars != nullptr) {
            UCharsTrie trie(trieUChars);
            UString_Trie_परिणाम परिणाम = trie.next(s, length);
            अगर (USTRINGTRIE_HAS_मूल्य(परिणाम)) {
                वापस true;
            }
        }
    }
    वापस false;
}

व्योम
EmojiProps::addStrings(स्थिर USetAdder *sa, UProperty which, UErrorCode &errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) { वापस; }
    अगर (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        वापस;
    }
    UProperty firstProp = which, lastProp = which;
    अगर (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    क्रम (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        स्थिर char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        अगर (trieUChars != nullptr) {
            UCharsTrie::Iterator iter(trieUChars, 0, errorCode);
            जबतक (iter.next(errorCode)) {
                स्थिर UnicodeString &s = iter.getString();
                sa->addString(sa->set, s.get_बफर(), s.length());
            }
        }
    }
}

U_NAMESPACE_END
