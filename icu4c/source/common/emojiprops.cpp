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

EmojiProps *singleton = शून्यसूचकः;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    विलोपय singleton;
    singleton = शून्यसूचकः;
    emojiInitOnce.reset();
    प्रत्यागम सत्य;
}

शून्य U_CALLCONV initSingleton(UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    singleton = नूतन EmojiProps(error_संहिता);
    यदि (singleton == शून्यसूचकः) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
    } अन्यथा यदि (U_FAILURE(error_संहिता)) {
        विलोपय singleton;
        singleton = शून्यसूचकः;
    }
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        ध्रुव UDataInfo &info, ध्रुव char16_t *दत्त_Format, uint8_t major, uint8_t minor) {
    प्रत्यागम
        info.size >= 20 &&
        info.isBigEndian == U_IS_BIG_ENDIAN &&
        info.charsetFamily == U_CHARSET_FAMILY &&
        info.दत्त_Format[0] == दत्त_Format[0] &&
        info.दत्त_Format[1] == दत्त_Format[1] &&
        info.दत्त_Format[2] == दत्त_Format[2] &&
        info.दत्त_Format[3] == दत्त_Format[3] &&
        info.format_संस्करणम्[0] == major &&
        info.format_संस्करणम्[1] >= minor;
}

}  // namespace

EmojiProps::~EmojiProps() {
    udata_close(स्मृतिः);
    ucptrie_close(cpTrie);
}

ध्रुव EmojiProps *
EmojiProps::getSingleton(UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    umtx_initOnce(emojiInitOnce, &initSingleton, error_संहिता);
    प्रत्यागम singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(शून्य * /*context*/, ध्रुव वर्ण * /*type*/, ध्रुव वर्ण * /*name*/,
                         ध्रुव UDataInfo *pInfo) {
    प्रत्यागम udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

शून्य
EmojiProps::load(UError_संहिता &error_संहिता) {
    स्मृतिः = udata_openChoice(शून्यसूचकः, "icu", "uemoji", isAcceptable, एतत्, &error_संहिता);
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    ध्रुव uint8_t* अन्तर्_अष्टकानि = static_cast<ध्रुव uint8_t*>(udata_get_स्मृतिः(स्मृतिः));
    ध्रुव int32_t* अन्तर्_Indexes = reinterpret_cast<ध्रुव int32_t*>(अन्तर्_अष्टकानि);
    int32_t indexes_दीर्घता = अन्तर्_Indexes[IX_CPTRIE_विस्थापनम्] / 4;
    यदि (indexes_दीर्घता <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_विस्थापनम्) {
        error_संहिता = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        प्रत्यागम;
    }

    int32_t i = IX_CPTRIE_विस्थापनम्;
    int32_t विस्थापनम् = अन्तर्_Indexes[i++];
    int32_t अग्र_विस्थापनम् = अन्तर्_Indexes[i];
    cpTrie = ucptrie_open_From_द्विमान(UCPTRIE_TYPE_FAST, UCPTRIE_मानम्_द्वयङ्काः_8,
                                    अन्तर्_अष्टकानि + विस्थापनम्, अग्र_विस्थापनम् - विस्थापनम्, शून्यसूचकः, &error_संहिता);
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }

    प्रति (i = IX_BASIC_EMOJI_TRIE_विस्थापनम्; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_विस्थापनम्; ++i) {
        विस्थापनम् = अन्तर्_Indexes[i];
        अग्र_विस्थापनम् = अन्तर्_Indexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        ध्रुव char16_t* p = अग्र_विस्थापनम् > विस्थापनम् ? reinterpret_cast<ध्रुव char16_t*>(अन्तर्_अष्टकानि + विस्थापनम्) : शून्यसूचकः;
        stringTries[get_String_Trie_सूचकांक(i)] = p;
    }
}

शून्य
EmojiProps::सङ्कलय_Property_Starts(ध्रुव USetAdder *sa, UError_संहिता & /*errorCode*/) ध्रुव {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, अन्तः;
    uint32_t value;
    यावत् ((अन्तः = ucptrie_getRange(cpTrie, start, UCPMAP_RANGE_प्रसामान्यम्, 0,
                                   शून्यसूचकः, शून्यसूचकः, &value)) >= 0) {
        sa->सङ्कलय(sa->समुच्चयः, start);
        start = अन्तः + 1;
    }
}

UBool
EmojiProps::has_द्विमान_Property(UChar32 c, UProperty which) {
    UError_संहिता error_संहिता = U_ZERO_ERROR;
    ध्रुव EmojiProps *ep = getSingleton(error_संहिता);
    प्रत्यागम U_SUCCESS(error_संहिता) && ep->has_द्विमान_Property_Impl(c, which);
}

UBool
EmojiProps::has_द्विमान_Property_Impl(UChar32 c, UProperty which) ध्रुव {
    यदि (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        प्रत्यागम असत्य;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    स्थिर constexpr int8_t bitFlags[] = {
        BIT_EMOJI,                  // UCHAR_EMOJI=57
        BIT_EMOJI_PRESENTATION,     // UCHAR_EMOJI_PRESENTATION=58
        BIT_EMOJI_MODIFIER,         // UCHAR_EMOJI_MODIFIER=59
        BIT_EMOJI_MODIFIER_आधारः,    // UCHAR_EMOJI_MODIFIER_BASE=60
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
    यदि (bit < 0) {
        प्रत्यागम असत्य;  // not a property that we support in this function
    }
    uint8_t द्वयङ्काः = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    प्रत्यागम (द्वयङ्काः >> bit) & 1;
}

UBool
EmojiProps::has_द्विमान_Property(ध्रुव char16_t *s, int32_t length, UProperty which) {
    UError_संहिता error_संहिता = U_ZERO_ERROR;
    ध्रुव EmojiProps *ep = getSingleton(error_संहिता);
    प्रत्यागम U_SUCCESS(error_संहिता) && ep->has_द्विमान_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_द्विमान_Property_Impl(ध्रुव char16_t *s, int32_t length, UProperty which) ध्रुव {
    यदि (s == शून्यसूचकः && length != 0) { प्रत्यागम असत्य; }
    यदि (length <= 0 && (length == 0 || *s == 0)) { प्रत्यागम असत्य; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    यदि (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        प्रत्यागम असत्य;
    }
    UProperty firstProp = which, चरमः_Prop = which;
    यदि (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        चरमः_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    प्रति (int32_t prop = firstProp; prop <= चरमः_Prop; ++prop) {
        ध्रुव char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        यदि (trieUChars != शून्यसूचकः) {
            UCharsTrie trie(trieUChars);
            UString_Trie_परिणाम परिणाम = trie.next(s, length);
            यदि (USTRINGTRIE_HAS_मानम्(परिणाम)) {
                प्रत्यागम सत्य;
            }
        }
    }
    प्रत्यागम असत्य;
}

शून्य
EmojiProps::सङ्कलय_वाक्यानि(ध्रुव USetAdder *sa, UProperty which, UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    यदि (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        प्रत्यागम;
    }
    UProperty firstProp = which, चरमः_Prop = which;
    यदि (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        firstProp = UCHAR_BASIC_EMOJI;
        चरमः_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    प्रति (int32_t prop = firstProp; prop <= चरमः_Prop; ++prop) {
        ध्रुव char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        यदि (trieUChars != शून्यसूचकः) {
            UCharsTrie::Iterator iter(trieUChars, 0, error_संहिता);
            यावत् (iter.next(error_संहिता)) {
                ध्रुव UnicodeString &s = iter.getString();
                sa->सङ्कलय_String(sa->समुच्चयः, s.get_बफर(), s.length());
            }
        }
    }
}

U_NAMESPACE_अन्तः
