// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.cpp
// created: 2021sep04 Markus W. Scherer

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uchar.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucharstrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucptrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustringtrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "emojiprops.h"
#ଅନ୍ତର୍ଭାଵ "ucln.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "uset_imp.h"

U_NAMESPACE_BEGIN

ନାମାକାଶ {

EmojiProps *singleton = nullptr;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    delete singleton;
    singleton = nullptr;
    emojiInitOnce.reset();
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ U_CALLCONV initSingleton(UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    singleton = ନୂତନ EmojiProps(errorCode);
    ଯଦି (singleton == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    } ଅନ୍ୟଥା ଯଦି (U_FAILURE(errorCode)) {
        delete singleton;
        singleton = nullptr;
    }
    ucln_common_registerCleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        ଅଚଳ UDataInfo &info, ଅଚଳ char16_t *dataFormat, uint8_t major, uint8_t minor) {
    ଫେରନ୍ତୁ
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

ଅଚଳ EmojiProps *
EmojiProps::getSingleton(UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    umtx_initOnce(emojiInitOnce, &initSingleton, errorCode);
    ଫେରନ୍ତୁ singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(ଶୂନ୍ୟ * /*context*/, ଅଚଳ ଅକ୍ଷର * /*type*/, ଅଚଳ ଅକ୍ଷର * /*name*/,
                         ଅଚଳ UDataInfo *pInfo) {
    ଫେରନ୍ତୁ udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

ଶୂନ୍ୟ
EmojiProps::load(UErrorCode &errorCode) {
    memory = udata_openChoice(nullptr, "icu", "uemoji", isAcceptable, ଏହି, &errorCode);
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    ଅଚଳ uint8_t* inBytes = static_cast<ଅଚଳ uint8_t*>(udata_getMemory(memory));
    ଅଚଳ int32_t* inIndexes = reinterpret_cast<ଅଚଳ int32_t*>(inBytes);
    int32_t indexesLength = inIndexes[IX_CPTRIE_OFFSET] / 4;
    ଯଦି (indexesLength <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ଫେରନ୍ତୁ;
    }

    int32_t i = IX_CPTRIE_OFFSET;
    int32_t offset = inIndexes[i++];
    int32_t nextOffset = inIndexes[i];
    cpTrie = ucptrie_open_From_ଦ୍ୱିମିକ(UCPTRIE_TYPE_FAST, UCPTRIE_ମୂଲ୍ୟ_BITS_8,
                                    inBytes + offset, nextOffset - offset, nullptr, &errorCode);
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }

    ପାଇଁ (i = IX_BASIC_EMOJI_TRIE_OFFSET; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET; ++i) {
        offset = inIndexes[i];
        nextOffset = inIndexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        ଅଚଳ char16_t* p = nextOffset > offset ? reinterpret_cast<ଅଚଳ char16_t*>(inBytes + offset) : nullptr;
        stringTries[getStringTrieIndex(i)] = p;
    }
}

ଶୂନ୍ୟ
EmojiProps::addPropertyStarts(ଅଚଳ USetAdder *sa, UErrorCode & /*errorCode*/) ଅଚଳ {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    ଯେତେବେଳେ ((end = ucptrie_getRange(cpTrie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

UBool
EmojiProps::has_ଦ୍ୱିମିକ_Property(UChar32 c, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    ଅଚଳ EmojiProps *ep = getSingleton(errorCode);
    ଫେରନ୍ତୁ U_SUCCESS(errorCode) && ep->has_ଦ୍ୱିମିକ_Property_Impl(c, which);
}

UBool
EmojiProps::has_ଦ୍ୱିମିକ_Property_Impl(UChar32 c, UProperty which) ଅଚଳ {
    ଯଦି (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    ସ୍ଥିର constexpr int8_t bitFlags[] = {
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
    ଯଦି (bit < 0) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;  // not a property that we support in this function
    }
    uint8_t bits = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    ଫେରନ୍ତୁ (bits >> bit) & 1;
}

UBool
EmojiProps::has_ଦ୍ୱିମିକ_Property(ଅଚଳ char16_t *s, int32_t length, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    ଅଚଳ EmojiProps *ep = getSingleton(errorCode);
    ଫେରନ୍ତୁ U_SUCCESS(errorCode) && ep->has_ଦ୍ୱିମିକ_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_ଦ୍ୱିମିକ_Property_Impl(ଅଚଳ char16_t *s, int32_t length, UProperty which) ଅଚଳ {
    ଯଦି (s == nullptr && length != 0) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    ଯଦି (length <= 0 && (length == 0 || *s == 0)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    ଯଦି (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    UProperty firstProp = which, lastProp = which;
    ଯଦି (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    ପାଇଁ (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        ଅଚଳ char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        ଯଦି (trieUChars != nullptr) {
            UCharsTrie trie(trieUChars);
            UStringTrieResult result = trie.next(s, length);
            ଯଦି (USTRINGTRIE_HAS_ମୂଲ୍ୟ(result)) {
                ଫେରନ୍ତୁ ସତ୍ୟ;
            }
        }
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ
EmojiProps::addStrings(ଅଚଳ USetAdder *sa, UProperty which, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    ଯଦି (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        ଫେରନ୍ତୁ;
    }
    UProperty firstProp = which, lastProp = which;
    ଯଦି (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    ପାଇଁ (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        ଅଚଳ char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        ଯଦି (trieUChars != nullptr) {
            UCharsTrie::Iterator iter(trieUChars, 0, errorCode);
            ଯେତେବେଳେ (iter.next(errorCode)) {
                ଅଚଳ UnicodeString &s = iter.getString();
                sa->addString(sa->set, s.getBuffer(), s.length());
            }
        }
    }
}

U_NAMESPACE_END
