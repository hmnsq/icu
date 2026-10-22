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

EmojiProps *singleton = ಶೂನ್ಯಸೂಚಕ;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    ಅಳಿಸು singleton;
    singleton = ಶೂನ್ಯಸೂಚಕ;
    emojiInitOnce.reset();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ U_CALLCONV initSingleton(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    singleton = ಹೊಸ EmojiProps(error_ಸಂಕೇತ);
    ಆದರೆ (singleton == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಅಳಿಸು singleton;
        singleton = ಶೂನ್ಯಸೂಚಕ;
    }
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        ಸ್ಥಿರಾಂಕ UDataInfo &info, ಸ್ಥಿರಾಂಕ char16_t *ದತ್ತಾಂಶ_Format, uint8_t major, uint8_t minor) {
    ಹಿಂತಿರುಗಿ
        info.size >= 20 &&
        info.isBigEndian == U_IS_BIG_ENDIAN &&
        info.charsetFamily == U_CHARSET_FAMILY &&
        info.ದತ್ತಾಂಶ_Format[0] == ದತ್ತಾಂಶ_Format[0] &&
        info.ದತ್ತಾಂಶ_Format[1] == ದತ್ತಾಂಶ_Format[1] &&
        info.ದತ್ತಾಂಶ_Format[2] == ದತ್ತಾಂಶ_Format[2] &&
        info.ದತ್ತಾಂಶ_Format[3] == ದತ್ತಾಂಶ_Format[3] &&
        info.format_ಆವೃತ್ತಿ[0] == major &&
        info.format_ಆವೃತ್ತಿ[1] >= minor;
}

}  // namespace

EmojiProps::~EmojiProps() {
    udata_close(ಸ್ಮರಣೆ);
    ucptrie_close(cpTrie);
}

ಸ್ಥಿರಾಂಕ EmojiProps *
EmojiProps::getSingleton(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    umtx_initOnce(emojiInitOnce, &initSingleton, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(ಶೂನ್ಯ * /*context*/, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /*type*/, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /*name*/,
                         ಸ್ಥಿರಾಂಕ UDataInfo *pInfo) {
    ಹಿಂತಿರುಗಿ udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

ಶೂನ್ಯ
EmojiProps::load(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಸ್ಮರಣೆ = udata_openChoice(ಶೂನ್ಯಸೂಚಕ, "icu", "uemoji", isAcceptable, ಇದು, &error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    ಸ್ಥಿರಾಂಕ uint8_t* ಒಳ_ಬೈಟುಗಳು = static_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(udata_get_ಸ್ಮರಣೆ(ಸ್ಮರಣೆ));
    ಸ್ಥಿರಾಂಕ int32_t* ಒಳ_Indexes = reinterpret_cast<ಸ್ಥಿರಾಂಕ int32_t*>(ಒಳ_ಬೈಟುಗಳು);
    int32_t indexes_ಉದ್ದಳತೆ = ಒಳ_Indexes[IX_CPTRIE_ಸ್ಥಾನಾಂತರ] / 4;
    ಆದರೆ (indexes_ಉದ್ದಳತೆ <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_ಸ್ಥಾನಾಂತರ) {
        error_ಸಂಕೇತ = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ಹಿಂತಿರುಗಿ;
    }

    int32_t i = IX_CPTRIE_ಸ್ಥಾನಾಂತರ;
    int32_t ಸ್ಥಾನಾಂತರ = ಒಳ_Indexes[i++];
    int32_t ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ = ಒಳ_Indexes[i];
    cpTrie = ucptrie_open_From_ದ್ವಿಮಾನ(UCPTRIE_TYPE_FAST, UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8,
                                    ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ - ಸ್ಥಾನಾಂತರ, ಶೂನ್ಯಸೂಚಕ, &error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (i = IX_BASIC_EMOJI_TRIE_ಸ್ಥಾನಾಂತರ; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_ಸ್ಥಾನಾಂತರ; ++i) {
        ಸ್ಥಾನಾಂತರ = ಒಳ_Indexes[i];
        ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ = ಒಳ_Indexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        ಸ್ಥಿರಾಂಕ char16_t* p = ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ > ಸ್ಥಾನಾಂತರ ? reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t*>(ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ) : ಶೂನ್ಯಸೂಚಕ;
        stringTries[get_String_Trie_ಸೂಚ್ಯಂಕ(i)] = p;
    }
}

ಶೂನ್ಯ
EmojiProps::ಕೂಡಿಸು_Property_Starts(ಸ್ಥಿರಾಂಕ USetAdder *sa, UError_ಸಂಕೇತ & /*errorCode*/) ಸ್ಥಿರಾಂಕ {
    // Add the start code point of each same-value range of the trie.
    UChar32 ಆರಂಭ = 0, ಕೊನೆ;
    uint32_t value;
    ಆಗ ((ಕೊನೆ = ucptrie_getRange(cpTrie, ಆರಂಭ, UCPMAP_RANGE_ಸಾಮಾನ್ಯ, 0,
                                   ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, &value)) >= 0) {
        sa->ಕೂಡಿಸು(sa->ಗಣ, ಆರಂಭ);
        ಆರಂಭ = ಕೊನೆ + 1;
    }
}

UBool
EmojiProps::has_ದ್ವಿಮಾನ_Property(UChar32 c, UProperty which) {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;
    ಸ್ಥಿರಾಂಕ EmojiProps *ep = getSingleton(error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ U_SUCCESS(error_ಸಂಕೇತ) && ep->has_ದ್ವಿಮಾನ_Property_Impl(c, which);
}

UBool
EmojiProps::has_ದ್ವಿಮಾನ_Property_Impl(UChar32 c, UProperty which) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    ಸ್ಥಿರ constexpr int8_t ಬಿಟ್_Flags[] = {
        ಬಿಟ್_EMOJI,                  // UCHAR_EMOJI=57
        ಬಿಟ್_EMOJI_PRESENTATION,     // UCHAR_EMOJI_PRESENTATION=58
        ಬಿಟ್_EMOJI_MODIFIER,         // UCHAR_EMOJI_MODIFIER=59
        ಬಿಟ್_EMOJI_MODIFIER_ತಳ,    // UCHAR_EMOJI_MODIFIER_BASE=60
        ಬಿಟ್_EMOJI_COMPONENT,        // UCHAR_EMOJI_COMPONENT=61
        -1,                         // UCHAR_REGIONAL_INDICATOR=62
        -1,                         // UCHAR_PREPENDED_CONCATENATION_MARK=63
        ಬಿಟ್_EXTENDED_PICTOGRAPHIC,  // UCHAR_EXTENDED_PICTOGRAPHIC=64
        ಬಿಟ್_BASIC_EMOJI,            // UCHAR_BASIC_EMOJI=65
        -1,                         // UCHAR_EMOJI_KEYCAP_SEQUENCE=66
        -1,                         // UCHAR_RGI_EMOJI_MODIFIER_SEQUENCE=67
        -1,                         // UCHAR_RGI_EMOJI_FLAG_SEQUENCE=68
        -1,                         // UCHAR_RGI_EMOJI_TAG_SEQUENCE=69
        -1,                         // UCHAR_RGI_EMOJI_ZWJ_SEQUENCE=70
        ಬಿಟ್_BASIC_EMOJI,            // UCHAR_RGI_EMOJI=71
    };
    int32_t bit = ಬಿಟ್_Flags[which - UCHAR_EMOJI];
    ಆದರೆ (bit < 0) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;  // not a property that we support in this function
    }
    uint8_t ಬಿಟ್ಟುಗಳು = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    ಹಿಂತಿರುಗಿ (ಬಿಟ್ಟುಗಳು >> bit) & 1;
}

UBool
EmojiProps::has_ದ್ವಿಮಾನ_Property(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length, UProperty which) {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;
    ಸ್ಥಿರಾಂಕ EmojiProps *ep = getSingleton(error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ U_SUCCESS(error_ಸಂಕೇತ) && ep->has_ದ್ವಿಮಾನ_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_ದ್ವಿಮಾನ_Property_Impl(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length, UProperty which) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (s == ಶೂನ್ಯಸೂಚಕ && length != 0) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    ಆದರೆ (length <= 0 && (length == 0 || *s == 0)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    ಆದರೆ (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    UProperty ಮೊದಲ_Prop = which, ಕೊನೆಯ_Prop = which;
    ಆದರೆ (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        ಮೊದಲ_Prop = UCHAR_BASIC_EMOJI;
        ಕೊನೆಯ_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    ಗಾಗಿ (int32_t prop = ಮೊದಲ_Prop; prop <= ಕೊನೆಯ_Prop; ++prop) {
        ಸ್ಥಿರಾಂಕ char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        ಆದರೆ (trieUChars != ಶೂನ್ಯಸೂಚಕ) {
            UCharsTrie trie(trieUChars);
            UString_Trie_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ = trie.next(s, length);
            ಆದರೆ (USTRINGTRIE_HAS_ಮೌಲ್ಯ(ಫಲಿತಾಂಶ)) {
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ
EmojiProps::ಕೂಡಿಸು_ಅಕ್ಷರಸರಣಿಗಳು(ಸ್ಥಿರಾಂಕ USetAdder *sa, UProperty which, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    ಆದರೆ (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        ಹಿಂತಿರುಗಿ;
    }
    UProperty ಮೊದಲ_Prop = which, ಕೊನೆಯ_Prop = which;
    ಆದರೆ (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        ಮೊದಲ_Prop = UCHAR_BASIC_EMOJI;
        ಕೊನೆಯ_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    ಗಾಗಿ (int32_t prop = ಮೊದಲ_Prop; prop <= ಕೊನೆಯ_Prop; ++prop) {
        ಸ್ಥಿರಾಂಕ char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        ಆದರೆ (trieUChars != ಶೂನ್ಯಸೂಚಕ) {
            UCharsTrie::Iterator iter(trieUChars, 0, error_ಸಂಕೇತ);
            ಆಗ (iter.next(error_ಸಂಕೇತ)) {
                ಸ್ಥಿರಾಂಕ UnicodeString &s = iter.getString();
                sa->ಕೂಡಿಸು_String(sa->ಗಣ, s.get_ಬಫರ್(), s.length());
            }
        }
    }
}

U_NAMESPACE_ಕೊನೆ
