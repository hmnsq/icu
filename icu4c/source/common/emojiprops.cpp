// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.cpp
// created: 2021sep04 Markus W. Scherer

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/ucharstrie.h"
#అన్తర్భావ "unicode/ucptrie.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "unicode/ustringtrie.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "emojiprops.h"
#అన్తర్భావ "ucln.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "uset_imp.h"

U_NAMESPACE_BEGIN

నామాకాశ {

EmojiProps *singleton = శూన్యసూచిక;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    తొలగించు singleton;
    singleton = శూన్యసూచిక;
    emojiInitOnce.reset();
    తిరిగి నిజం;
}

శూన్య U_CALLCONV initSingleton(UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    singleton = కొత్త EmojiProps(error_సంకేతం);
    ఒకవేళ (singleton == శూన్యసూచిక) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    } లేకపోతే ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తొలగించు singleton;
        singleton = శూన్యసూచిక;
    }
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        స్థిరరాశి UDataInfo &info, స్థిరరాశి char16_t *దత్తాంశం_Format, uint8_t major, uint8_t minor) {
    తిరిగి
        info.size >= 20 &&
        info.isBigEndian == U_IS_BIG_ENDIAN &&
        info.charsetFamily == U_CHARSET_FAMILY &&
        info.దత్తాంశం_Format[0] == దత్తాంశం_Format[0] &&
        info.దత్తాంశం_Format[1] == దత్తాంశం_Format[1] &&
        info.దత్తాంశం_Format[2] == దత్తాంశం_Format[2] &&
        info.దత్తాంశం_Format[3] == దత్తాంశం_Format[3] &&
        info.format_సంస్కరణ[0] == major &&
        info.format_సంస్కరణ[1] >= minor;
}

}  // namespace

EmojiProps::~EmojiProps() {
    udata_close(జ్ఞాపకం);
    ucptrie_close(cpTrie);
}

స్థిరరాశి EmojiProps *
EmojiProps::getSingleton(UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    umtx_initOnce(emojiInitOnce, &initSingleton, error_సంకేతం);
    తిరిగి singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(శూన్య * /*context*/, స్థిరరాశి అక్షరం * /*type*/, స్థిరరాశి అక్షరం * /*name*/,
                         స్థిరరాశి UDataInfo *pInfo) {
    తిరిగి udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

శూన్య
EmojiProps::load(UError_సంకేతం &error_సంకేతం) {
    జ్ఞాపకం = udata_openChoice(శూన్యసూచిక, "icu", "uemoji", isAcceptable, ఇది, &error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    స్థిరరాశి uint8_t* లోపల_బైట్లు = static_cast<స్థిరరాశి uint8_t*>(udata_get_జ్ఞాపకం(జ్ఞాపకం));
    స్థిరరాశి int32_t* లోపల_Indexes = reinterpret_cast<స్థిరరాశి int32_t*>(లోపల_బైట్లు);
    int32_t indexes_నిడివి = లోపల_Indexes[IX_CPTRIE_స్థానభ్రంశం] / 4;
    ఒకవేళ (indexes_నిడివి <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_స్థానభ్రంశం) {
        error_సంకేతం = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        తిరిగి;
    }

    int32_t i = IX_CPTRIE_స్థానభ్రంశం;
    int32_t స్థానభ్రంశం = లోపల_Indexes[i++];
    int32_t తదుపరి_స్థానభ్రంశం = లోపల_Indexes[i];
    cpTrie = ucptrie_open_From_ద్విమాన(UCPTRIE_TYPE_FAST, UCPTRIE_విలువ_బిట్లు_8,
                                    లోపల_బైట్లు + స్థానభ్రంశం, తదుపరి_స్థానభ్రంశం - స్థానభ్రంశం, శూన్యసూచిక, &error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }

    కొరకు (i = IX_BASIC_EMOJI_TRIE_స్థానభ్రంశం; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_స్థానభ్రంశం; ++i) {
        స్థానభ్రంశం = లోపల_Indexes[i];
        తదుపరి_స్థానభ్రంశం = లోపల_Indexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        స్థిరరాశి char16_t* p = తదుపరి_స్థానభ్రంశం > స్థానభ్రంశం ? reinterpret_cast<స్థిరరాశి char16_t*>(లోపల_బైట్లు + స్థానభ్రంశం) : శూన్యసూచిక;
        stringTries[get_String_Trie_సూచీ(i)] = p;
    }
}

శూన్య
EmojiProps::కూడు_Property_Starts(స్థిరరాశి USetAdder *sa, UError_సంకేతం & /*errorCode*/) స్థిరరాశి {
    // Add the start code point of each same-value range of the trie.
    UChar32 ప్రారంభం = 0, చివర;
    uint32_t value;
    అయితే ((చివర = ucptrie_getRange(cpTrie, ప్రారంభం, UCPMAP_RANGE_సాధారణ, 0,
                                   శూన్యసూచిక, శూన్యసూచిక, &value)) >= 0) {
        sa->కూడు(sa->సమితి, ప్రారంభం);
        ప్రారంభం = చివర + 1;
    }
}

UBool
EmojiProps::has_ద్విమాన_Property(UChar32 c, UProperty which) {
    UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
    స్థిరరాశి EmojiProps *ep = getSingleton(error_సంకేతం);
    తిరిగి U_SUCCESS(error_సంకేతం) && ep->has_ద్విమాన_Property_Impl(c, which);
}

UBool
EmojiProps::has_ద్విమాన_Property_Impl(UChar32 c, UProperty which) స్థిరరాశి {
    ఒకవేళ (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        తిరిగి అబద్ధం;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    స్థిర constexpr int8_t బిట్టు_Flags[] = {
        బిట్టు_EMOJI,                  // UCHAR_EMOJI=57
        బిట్టు_EMOJI_PRESENTATION,     // UCHAR_EMOJI_PRESENTATION=58
        బిట్టు_EMOJI_MODIFIER,         // UCHAR_EMOJI_MODIFIER=59
        బిట్టు_EMOJI_MODIFIER_పునాది,    // UCHAR_EMOJI_MODIFIER_BASE=60
        బిట్టు_EMOJI_COMPONENT,        // UCHAR_EMOJI_COMPONENT=61
        -1,                         // UCHAR_REGIONAL_INDICATOR=62
        -1,                         // UCHAR_PREPENDED_CONCATENATION_MARK=63
        బిట్టు_EXTENDED_PICTOGRAPHIC,  // UCHAR_EXTENDED_PICTOGRAPHIC=64
        బిట్టు_BASIC_EMOJI,            // UCHAR_BASIC_EMOJI=65
        -1,                         // UCHAR_EMOJI_KEYCAP_SEQUENCE=66
        -1,                         // UCHAR_RGI_EMOJI_MODIFIER_SEQUENCE=67
        -1,                         // UCHAR_RGI_EMOJI_FLAG_SEQUENCE=68
        -1,                         // UCHAR_RGI_EMOJI_TAG_SEQUENCE=69
        -1,                         // UCHAR_RGI_EMOJI_ZWJ_SEQUENCE=70
        బిట్టు_BASIC_EMOJI,            // UCHAR_RGI_EMOJI=71
    };
    int32_t bit = బిట్టు_Flags[which - UCHAR_EMOJI];
    ఒకవేళ (bit < 0) {
        తిరిగి అబద్ధం;  // not a property that we support in this function
    }
    uint8_t బిట్లు = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    తిరిగి (బిట్లు >> bit) & 1;
}

UBool
EmojiProps::has_ద్విమాన_Property(స్థిరరాశి char16_t *s, int32_t length, UProperty which) {
    UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
    స్థిరరాశి EmojiProps *ep = getSingleton(error_సంకేతం);
    తిరిగి U_SUCCESS(error_సంకేతం) && ep->has_ద్విమాన_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_ద్విమాన_Property_Impl(స్థిరరాశి char16_t *s, int32_t length, UProperty which) స్థిరరాశి {
    ఒకవేళ (s == శూన్యసూచిక && length != 0) { తిరిగి అబద్ధం; }
    ఒకవేళ (length <= 0 && (length == 0 || *s == 0)) { తిరిగి అబద్ధం; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    ఒకవేళ (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        తిరిగి అబద్ధం;
    }
    UProperty మొదటి_Prop = which, చివరి_Prop = which;
    ఒకవేళ (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        మొదటి_Prop = UCHAR_BASIC_EMOJI;
        చివరి_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    కొరకు (int32_t prop = మొదటి_Prop; prop <= చివరి_Prop; ++prop) {
        స్థిరరాశి char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        ఒకవేళ (trieUChars != శూన్యసూచిక) {
            UCharsTrie trie(trieUChars);
            UString_Trie_ఫలితం ఫలితం = trie.next(s, length);
            ఒకవేళ (USTRINGTRIE_HAS_విలువ(ఫలితం)) {
                తిరిగి నిజం;
            }
        }
    }
    తిరిగి అబద్ధం;
}

శూన్య
EmojiProps::కూడు_అక్షరశ్రేణులు(స్థిరరాశి USetAdder *sa, UProperty which, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    ఒకవేళ (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        తిరిగి;
    }
    UProperty మొదటి_Prop = which, చివరి_Prop = which;
    ఒకవేళ (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        మొదటి_Prop = UCHAR_BASIC_EMOJI;
        చివరి_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    కొరకు (int32_t prop = మొదటి_Prop; prop <= చివరి_Prop; ++prop) {
        స్థిరరాశి char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        ఒకవేళ (trieUChars != శూన్యసూచిక) {
            UCharsTrie::Iterator iter(trieUChars, 0, error_సంకేతం);
            అయితే (iter.next(error_సంకేతం)) {
                స్థిరరాశి UnicodeString &s = iter.getString();
                sa->కూడు_String(sa->సమితి, s.get_బఫరు(), s.length());
            }
        }
    }
}

U_NAMESPACE_చివర
