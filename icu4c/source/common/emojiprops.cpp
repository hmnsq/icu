// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.cpp
// created: 2021sep04 Markus W. Scherer

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/ucharstrie.h"
#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/ustringtrie.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "emojiprops.h"
#अन्तर्भाव "ucln.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "uset_imp.h"

U_NAMESPACE_BEGIN

नामाकाश {

EmojiProps *singleton = nullptr;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    delete singleton;
    singleton = nullptr;
    emojiInitOnce.reset();
    परत सत्य;
}

शून्य U_CALLCONV initSingleton(UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) { परत; }
    singleton = नवीन EmojiProps(errorCode);
    जर (singleton == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा जर (U_FAILURE(errorCode)) {
        delete singleton;
        singleton = nullptr;
    }
    ucln_common_registerCleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        स्थिरांक UDataInfo &info, स्थिरांक char16_t *dataFormat, uint8_t major, uint8_t minor) {
    परत
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

स्थिरांक EmojiProps *
EmojiProps::getSingleton(UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) { परत nullptr; }
    umtx_initOnce(emojiInitOnce, &initSingleton, errorCode);
    परत singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(शून्य * /*context*/, स्थिरांक अक्षर * /*type*/, स्थिरांक अक्षर * /*name*/,
                         स्थिरांक UDataInfo *pInfo) {
    परत udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

शून्य
EmojiProps::load(UErrorCode &errorCode) {
    memory = udata_openChoice(nullptr, "icu", "uemoji", isAcceptable, हे, &errorCode);
    जर (U_FAILURE(errorCode)) { परत; }
    स्थिरांक uint8_t* inBytes = static_cast<स्थिरांक uint8_t*>(udata_getMemory(memory));
    स्थिरांक int32_t* inIndexes = reinterpret_cast<स्थिरांक int32_t*>(inBytes);
    int32_t indexesLength = inIndexes[IX_CPTRIE_OFFSET] / 4;
    जर (indexesLength <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        परत;
    }

    int32_t i = IX_CPTRIE_OFFSET;
    int32_t offset = inIndexes[i++];
    int32_t nextOffset = inIndexes[i];
    cpTrie = ucptrie_open_From_द्विमान(UCPTRIE_TYPE_FAST, UCPTRIE_मूल्य_BITS_8,
                                    inBytes + offset, nextOffset - offset, nullptr, &errorCode);
    जर (U_FAILURE(errorCode)) {
        परत;
    }

    साठी (i = IX_BASIC_EMOJI_TRIE_OFFSET; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_OFFSET; ++i) {
        offset = inIndexes[i];
        nextOffset = inIndexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        स्थिरांक char16_t* p = nextOffset > offset ? reinterpret_cast<स्थिरांक char16_t*>(inBytes + offset) : nullptr;
        stringTries[getStringTrieIndex(i)] = p;
    }
}

शून्य
EmojiProps::addPropertyStarts(स्थिरांक USetAdder *sa, UErrorCode & /*errorCode*/) स्थिरांक {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    पर्यंत ((end = ucptrie_getRange(cpTrie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

UBool
EmojiProps::has_द्विमान_Property(UChar32 c, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    स्थिरांक EmojiProps *ep = getSingleton(errorCode);
    परत U_SUCCESS(errorCode) && ep->has_द्विमान_Property_Impl(c, which);
}

UBool
EmojiProps::has_द्विमान_Property_Impl(UChar32 c, UProperty which) स्थिरांक {
    जर (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        परत असत्य;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    स्थिर constexpr int8_t bitFlags[] = {
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
    जर (bit < 0) {
        परत असत्य;  // not a property that we support in this function
    }
    uint8_t bits = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    परत (bits >> bit) & 1;
}

UBool
EmojiProps::has_द्विमान_Property(स्थिरांक char16_t *s, int32_t length, UProperty which) {
    UErrorCode errorCode = U_ZERO_ERROR;
    स्थिरांक EmojiProps *ep = getSingleton(errorCode);
    परत U_SUCCESS(errorCode) && ep->has_द्विमान_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_द्विमान_Property_Impl(स्थिरांक char16_t *s, int32_t length, UProperty which) स्थिरांक {
    जर (s == nullptr && length != 0) { परत असत्य; }
    जर (length <= 0 && (length == 0 || *s == 0)) { परत असत्य; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    जर (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        परत असत्य;
    }
    UProperty firstProp = which, lastProp = which;
    जर (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    साठी (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        स्थिरांक char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        जर (trieUChars != nullptr) {
            UCharsTrie trie(trieUChars);
            UStringTrieResult result = trie.next(s, length);
            जर (USTRINGTRIE_HAS_मूल्य(result)) {
                परत सत्य;
            }
        }
    }
    परत असत्य;
}

शून्य
EmojiProps::addStrings(स्थिरांक USetAdder *sa, UProperty which, UErrorCode &errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) { परत; }
    जर (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        परत;
    }
    UProperty firstProp = which, lastProp = which;
    जर (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        lastProp = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    साठी (int32_t prop = firstProp; prop <= lastProp; ++prop) {
        स्थिरांक char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        जर (trieUChars != nullptr) {
            UCharsTrie::Iterator iter(trieUChars, 0, errorCode);
            पर्यंत (iter.next(errorCode)) {
                स्थिरांक UnicodeString &s = iter.getString();
                sa->addString(sa->set, s.getBuffer(), s.length());
            }
        }
    }
}

U_NAMESPACE_END
