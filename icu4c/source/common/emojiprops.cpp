// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.cpp
// created: 2021sep04 Markus W. Scherer

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucharstrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustringtrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "emojiprops.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uset_imp.h"

U_NAMESPACE_BEGIN

ਨਾਮਾਕਾਸ਼ {

EmojiProps *singleton = ਖਾਲੀ_ਸੂਚਕ;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    ਮਿਟਾਓ singleton;
    singleton = ਖਾਲੀ_ਸੂਚਕ;
    emojiInitOnce.reset();
    ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ U_CALLCONV initSingleton(UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    singleton = ਨਵਾਂ EmojiProps(error_ਕੋਡ);
    ਜੇ (singleton == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    } ਨਹੀਂਤਾਂ ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਮਿਟਾਓ singleton;
        singleton = ਖਾਲੀ_ਸੂਚਕ;
    }
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        ਅਚਲ UDataInfo &info, ਅਚਲ char16_t *ਅੰਕੜੇ_Format, uint8_t major, uint8_t minor) {
    ਵਾਪਸ
        info.size >= 20 &&
        info.isBigEndian == U_IS_BIG_ENDIAN &&
        info.charsetFamily == U_CHARSET_FAMILY &&
        info.ਅੰਕੜੇ_Format[0] == ਅੰਕੜੇ_Format[0] &&
        info.ਅੰਕੜੇ_Format[1] == ਅੰਕੜੇ_Format[1] &&
        info.ਅੰਕੜੇ_Format[2] == ਅੰਕੜੇ_Format[2] &&
        info.ਅੰਕੜੇ_Format[3] == ਅੰਕੜੇ_Format[3] &&
        info.format_ਸੰਸਕਰਣ[0] == major &&
        info.format_ਸੰਸਕਰਣ[1] >= minor;
}

}  // namespace

EmojiProps::~EmojiProps() {
    udata_close(ਸਮ੍ਰਿਤੀ);
    ucptrie_close(cpTrie);
}

ਅਚਲ EmojiProps *
EmojiProps::getSingleton(UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    umtx_initOnce(emojiInitOnce, &initSingleton, error_ਕੋਡ);
    ਵਾਪਸ singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(ਖਾਲੀ * /*context*/, ਅਚਲ ਅੱਖਰ * /*type*/, ਅਚਲ ਅੱਖਰ * /*name*/,
                         ਅਚਲ UDataInfo *pInfo) {
    ਵਾਪਸ udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

ਖਾਲੀ
EmojiProps::load(UError_ਕੋਡ &error_ਕੋਡ) {
    ਸਮ੍ਰਿਤੀ = udata_openChoice(ਖਾਲੀ_ਸੂਚਕ, "icu", "uemoji", isAcceptable, ਇਹ, &error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    ਅਚਲ uint8_t* ਵਿੱਚ_ਬਾਈਟਾਂ = static_cast<ਅਚਲ uint8_t*>(udata_get_ਸਮ੍ਰਿਤੀ(ਸਮ੍ਰਿਤੀ));
    ਅਚਲ int32_t* ਵਿੱਚ_Indexes = reinterpret_cast<ਅਚਲ int32_t*>(ਵਿੱਚ_ਬਾਈਟਾਂ);
    int32_t indexes_ਲੰਮਾਈ = ਵਿੱਚ_Indexes[IX_CPTRIE_ਵਿੱਥ] / 4;
    ਜੇ (indexes_ਲੰਮਾਈ <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_ਵਿੱਥ) {
        error_ਕੋਡ = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ਵਾਪਸ;
    }

    int32_t i = IX_CPTRIE_ਵਿੱਥ;
    int32_t ਵਿੱਥ = ਵਿੱਚ_Indexes[i++];
    int32_t ਅਗਲਾ_ਵਿੱਥ = ਵਿੱਚ_Indexes[i];
    cpTrie = ucptrie_open_From_ਦੁਆਧਾਰੀ(UCPTRIE_TYPE_FAST, UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8,
                                    ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, ਅਗਲਾ_ਵਿੱਥ - ਵਿੱਥ, ਖਾਲੀ_ਸੂਚਕ, &error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    ਲਈ (i = IX_BASIC_EMOJI_TRIE_ਵਿੱਥ; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_ਵਿੱਥ; ++i) {
        ਵਿੱਥ = ਵਿੱਚ_Indexes[i];
        ਅਗਲਾ_ਵਿੱਥ = ਵਿੱਚ_Indexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        ਅਚਲ char16_t* p = ਅਗਲਾ_ਵਿੱਥ > ਵਿੱਥ ? reinterpret_cast<ਅਚਲ char16_t*>(ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ) : ਖਾਲੀ_ਸੂਚਕ;
        stringTries[get_String_Trie_ਸੂਚਕਾਂਕ(i)] = p;
    }
}

ਖਾਲੀ
EmojiProps::ਜੋੜੋ_Property_Starts(ਅਚਲ USetAdder *sa, UError_ਕੋਡ & /*errorCode*/) ਅਚਲ {
    // Add the start code point of each same-value range of the trie.
    UChar32 ਸ਼ੁਰੂ = 0, ਸਿਰਾ;
    uint32_t value;
    ਜਦੋਂ ((ਸਿਰਾ = ucptrie_getRange(cpTrie, ਸ਼ੁਰੂ, UCPMAP_RANGE_ਸਾਧਾਰਨ, 0,
                                   ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, &value)) >= 0) {
        sa->ਜੋੜੋ(sa->ਸੈੱਟ, ਸ਼ੁਰੂ);
        ਸ਼ੁਰੂ = ਸਿਰਾ + 1;
    }
}

UBool
EmojiProps::has_ਦੁਆਧਾਰੀ_Property(UChar32 c, UProperty which) {
    UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;
    ਅਚਲ EmojiProps *ep = getSingleton(error_ਕੋਡ);
    ਵਾਪਸ U_SUCCESS(error_ਕੋਡ) && ep->has_ਦੁਆਧਾਰੀ_Property_Impl(c, which);
}

UBool
EmojiProps::has_ਦੁਆਧਾਰੀ_Property_Impl(UChar32 c, UProperty which) ਅਚਲ {
    ਜੇ (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        ਵਾਪਸ ਝੂਠ;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    ਸਥਿਰ constexpr int8_t ਦੋਆਧਾਰੀ_ਅੰਕ_Flags[] = {
        ਦੋਆਧਾਰੀ_ਅੰਕ_EMOJI,                  // UCHAR_EMOJI=57
        ਦੋਆਧਾਰੀ_ਅੰਕ_EMOJI_PRESENTATION,     // UCHAR_EMOJI_PRESENTATION=58
        ਦੋਆਧਾਰੀ_ਅੰਕ_EMOJI_MODIFIER,         // UCHAR_EMOJI_MODIFIER=59
        ਦੋਆਧਾਰੀ_ਅੰਕ_EMOJI_MODIFIER_ਅਧਾਰ,    // UCHAR_EMOJI_MODIFIER_BASE=60
        ਦੋਆਧਾਰੀ_ਅੰਕ_EMOJI_COMPONENT,        // UCHAR_EMOJI_COMPONENT=61
        -1,                         // UCHAR_REGIONAL_INDICATOR=62
        -1,                         // UCHAR_PREPENDED_CONCATENATION_MARK=63
        ਦੋਆਧਾਰੀ_ਅੰਕ_EXTENDED_PICTOGRAPHIC,  // UCHAR_EXTENDED_PICTOGRAPHIC=64
        ਦੋਆਧਾਰੀ_ਅੰਕ_BASIC_EMOJI,            // UCHAR_BASIC_EMOJI=65
        -1,                         // UCHAR_EMOJI_KEYCAP_SEQUENCE=66
        -1,                         // UCHAR_RGI_EMOJI_MODIFIER_SEQUENCE=67
        -1,                         // UCHAR_RGI_EMOJI_FLAG_SEQUENCE=68
        -1,                         // UCHAR_RGI_EMOJI_TAG_SEQUENCE=69
        -1,                         // UCHAR_RGI_EMOJI_ZWJ_SEQUENCE=70
        ਦੋਆਧਾਰੀ_ਅੰਕ_BASIC_EMOJI,            // UCHAR_RGI_EMOJI=71
    };
    int32_t bit = ਦੋਆਧਾਰੀ_ਅੰਕ_Flags[which - UCHAR_EMOJI];
    ਜੇ (bit < 0) {
        ਵਾਪਸ ਝੂਠ;  // not a property that we support in this function
    }
    uint8_t ਬਿੱਟ = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    ਵਾਪਸ (ਬਿੱਟ >> bit) & 1;
}

UBool
EmojiProps::has_ਦੁਆਧਾਰੀ_Property(ਅਚਲ char16_t *s, int32_t length, UProperty which) {
    UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;
    ਅਚਲ EmojiProps *ep = getSingleton(error_ਕੋਡ);
    ਵਾਪਸ U_SUCCESS(error_ਕੋਡ) && ep->has_ਦੁਆਧਾਰੀ_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_ਦੁਆਧਾਰੀ_Property_Impl(ਅਚਲ char16_t *s, int32_t length, UProperty which) ਅਚਲ {
    ਜੇ (s == ਖਾਲੀ_ਸੂਚਕ && length != 0) { ਵਾਪਸ ਝੂਠ; }
    ਜੇ (length <= 0 && (length == 0 || *s == 0)) { ਵਾਪਸ ਝੂਠ; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    ਜੇ (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        ਵਾਪਸ ਝੂਠ;
    }
    UProperty ਪਹਿਲਾ_Prop = which, ਆਖਰੀ_Prop = which;
    ਜੇ (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        ਪਹਿਲਾ_Prop = UCHAR_BASIC_EMOJI;
        ਆਖਰੀ_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    ਲਈ (int32_t prop = ਪਹਿਲਾ_Prop; prop <= ਆਖਰੀ_Prop; ++prop) {
        ਅਚਲ char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        ਜੇ (trieUChars != ਖਾਲੀ_ਸੂਚਕ) {
            UCharsTrie trie(trieUChars);
            UString_Trie_ਨਤੀਜਾ ਨਤੀਜਾ = trie.next(s, length);
            ਜੇ (USTRINGTRIE_HAS_ਮੁੱਲ(ਨਤੀਜਾ)) {
                ਵਾਪਸ ਸੱਚ;
            }
        }
    }
    ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ
EmojiProps::ਜੋੜੋ_ਲੜੀਆਂ(ਅਚਲ USetAdder *sa, UProperty which, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    ਜੇ (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        ਵਾਪਸ;
    }
    UProperty ਪਹਿਲਾ_Prop = which, ਆਖਰੀ_Prop = which;
    ਜੇ (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        ਪਹਿਲਾ_Prop = UCHAR_BASIC_EMOJI;
        ਆਖਰੀ_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    ਲਈ (int32_t prop = ਪਹਿਲਾ_Prop; prop <= ਆਖਰੀ_Prop; ++prop) {
        ਅਚਲ char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        ਜੇ (trieUChars != ਖਾਲੀ_ਸੂਚਕ) {
            UCharsTrie::Iterator iter(trieUChars, 0, error_ਕੋਡ);
            ਜਦੋਂ (iter.next(error_ਕੋਡ)) {
                ਅਚਲ UnicodeString &s = iter.getString();
                sa->ਜੋੜੋ_String(sa->ਸੈੱਟ, s.get_ਬਫਰ(), s.length());
            }
        }
    }
}

U_NAMESPACE_ਸਿਰਾ
