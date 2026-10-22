// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.cpp
// created: 2021sep04 Markus W. Scherer

#ضمن "unicode/utypes.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/ucharstrie.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/ustringtrie.h"
#ضمن "unicode/utf16.h"
#ضمن "emojiprops.h"
#ضمن "ucln.h"
#ضمن "ucln_cmn.h"
#ضمن "umutex.h"
#ضمن "uset_imp.h"

U_NAMESPACE_BEGIN

نطاق {

EmojiProps *singleton = خالی_سوچک;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    مٹاؤ singleton;
    singleton = خالی_سوچک;
    emojiInitOnce.reset();
    أرجع سچ;
}

فراغ U_CALLCONV initSingleton(UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    singleton = جديد EmojiProps(error_کوڈ);
    إذا (singleton == خالی_سوچک) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
    } وإلا إذا (U_FAILURE(error_کوڈ)) {
        مٹاؤ singleton;
        singleton = خالی_سوچک;
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        ثابت UDataInfo &info, ثابت char16_t *انکڑے_Format, uint8_t major, uint8_t minor) {
    أرجع
        info.size >= 20 &&
        info.isBigEndian == U_IS_BIG_ENDIAN &&
        info.charsetFamily == U_CHARSET_FAMILY &&
        info.انکڑے_Format[0] == انکڑے_Format[0] &&
        info.انکڑے_Format[1] == انکڑے_Format[1] &&
        info.انکڑے_Format[2] == انکڑے_Format[2] &&
        info.انکڑے_Format[3] == انکڑے_Format[3] &&
        info.format_نسخہ[0] == major &&
        info.format_نسخہ[1] >= minor;
}

}  // namespace

EmojiProps::~EmojiProps() {
    udata_close(حافظہ);
    ucptrie_close(cpTrie);
}

ثابت EmojiProps *
EmojiProps::getSingleton(UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    umtx_initOnce(emojiInitOnce, &initSingleton, error_کوڈ);
    أرجع singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(فراغ * /*context*/, ثابت حرف * /*type*/, ثابت حرف * /*name*/,
                         ثابت UDataInfo *pInfo) {
    أرجع udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

فراغ
EmojiProps::load(UError_کوڈ &error_کوڈ) {
    حافظہ = udata_openChoice(خالی_سوچک, "icu", "uemoji", isAcceptable, هذا, &error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    ثابت uint8_t* وچ_بائٹاں = static_cast<ثابت uint8_t*>(udata_get_حافظہ(حافظہ));
    ثابت int32_t* وچ_Indexes = reinterpret_cast<ثابت int32_t*>(وچ_بائٹاں);
    int32_t indexes_لمائی = وچ_Indexes[IX_CPTRIE_وتھ] / 4;
    إذا (indexes_لمائی <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_وتھ) {
        error_کوڈ = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        أرجع;
    }

    int32_t i = IX_CPTRIE_وتھ;
    int32_t وتھ = وچ_Indexes[i++];
    int32_t اگلا_وتھ = وچ_Indexes[i];
    cpTrie = ucptrie_open_From_ثنائی(UCPTRIE_TYPE_FAST, UCPTRIE_قدر_بٹ_8,
                                    وچ_بائٹاں + وتھ, اگلا_وتھ - وتھ, خالی_سوچک, &error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع;
    }

    لكل (i = IX_BASIC_EMOJI_TRIE_وتھ; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_وتھ; ++i) {
        وتھ = وچ_Indexes[i];
        اگلا_وتھ = وچ_Indexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        ثابت char16_t* p = اگلا_وتھ > وتھ ? reinterpret_cast<ثابت char16_t*>(وچ_بائٹاں + وتھ) : خالی_سوچک;
        stringTries[get_String_Trie_اشاریہ(i)] = p;
    }
}

فراغ
EmojiProps::جوڑو_Property_Starts(ثابت USetAdder *sa, UError_کوڈ & /*errorCode*/) ثابت {
    // Add the start code point of each same-value range of the trie.
    UChar32 شروع = 0, سرا;
    uint32_t value;
    طالما ((سرا = ucptrie_getRange(cpTrie, شروع, UCPMAP_RANGE_عام, 0,
                                   خالی_سوچک, خالی_سوچک, &value)) >= 0) {
        sa->جوڑو(sa->سیٹ, شروع);
        شروع = سرا + 1;
    }
}

UBool
EmojiProps::has_ثنائی_Property(UChar32 c, UProperty which) {
    UError_کوڈ error_کوڈ = U_ZERO_ERROR;
    ثابت EmojiProps *ep = getSingleton(error_کوڈ);
    أرجع U_SUCCESS(error_کوڈ) && ep->has_ثنائی_Property_Impl(c, which);
}

UBool
EmojiProps::has_ثنائی_Property_Impl(UChar32 c, UProperty which) ثابت {
    إذا (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        أرجع خطأ;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    ساكن constexpr int8_t ثنائی_ہندسہ_Flags[] = {
        ثنائی_ہندسہ_EMOJI,                  // UCHAR_EMOJI=57
        ثنائی_ہندسہ_EMOJI_PRESENTATION,     // UCHAR_EMOJI_PRESENTATION=58
        ثنائی_ہندسہ_EMOJI_MODIFIER,         // UCHAR_EMOJI_MODIFIER=59
        ثنائی_ہندسہ_EMOJI_MODIFIER_ادھار,    // UCHAR_EMOJI_MODIFIER_BASE=60
        ثنائی_ہندسہ_EMOJI_COMPONENT,        // UCHAR_EMOJI_COMPONENT=61
        -1,                         // UCHAR_REGIONAL_INDICATOR=62
        -1,                         // UCHAR_PREPENDED_CONCATENATION_MARK=63
        ثنائی_ہندسہ_EXTENDED_PICTOGRAPHIC,  // UCHAR_EXTENDED_PICTOGRAPHIC=64
        ثنائی_ہندسہ_BASIC_EMOJI,            // UCHAR_BASIC_EMOJI=65
        -1,                         // UCHAR_EMOJI_KEYCAP_SEQUENCE=66
        -1,                         // UCHAR_RGI_EMOJI_MODIFIER_SEQUENCE=67
        -1,                         // UCHAR_RGI_EMOJI_FLAG_SEQUENCE=68
        -1,                         // UCHAR_RGI_EMOJI_TAG_SEQUENCE=69
        -1,                         // UCHAR_RGI_EMOJI_ZWJ_SEQUENCE=70
        ثنائی_ہندسہ_BASIC_EMOJI,            // UCHAR_RGI_EMOJI=71
    };
    int32_t bit = ثنائی_ہندسہ_Flags[which - UCHAR_EMOJI];
    إذا (bit < 0) {
        أرجع خطأ;  // not a property that we support in this function
    }
    uint8_t بٹ = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    أرجع (بٹ >> bit) & 1;
}

UBool
EmojiProps::has_ثنائی_Property(ثابت char16_t *s, int32_t length, UProperty which) {
    UError_کوڈ error_کوڈ = U_ZERO_ERROR;
    ثابت EmojiProps *ep = getSingleton(error_کوڈ);
    أرجع U_SUCCESS(error_کوڈ) && ep->has_ثنائی_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_ثنائی_Property_Impl(ثابت char16_t *s, int32_t length, UProperty which) ثابت {
    إذا (s == خالی_سوچک && length != 0) { أرجع خطأ; }
    إذا (length <= 0 && (length == 0 || *s == 0)) { أرجع خطأ; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    إذا (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        أرجع خطأ;
    }
    UProperty پہلا_Prop = which, آخری_Prop = which;
    إذا (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        پہلا_Prop = UCHAR_BASIC_EMOJI;
        آخری_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    لكل (int32_t prop = پہلا_Prop; prop <= آخری_Prop; ++prop) {
        ثابت char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        إذا (trieUChars != خالی_سوچک) {
            UCharsTrie trie(trieUChars);
            UString_Trie_نتیجہ نتیجہ = trie.next(s, length);
            إذا (USTRINGTRIE_HAS_قدر(نتیجہ)) {
                أرجع سچ;
            }
        }
    }
    أرجع خطأ;
}

فراغ
EmojiProps::جوڑو_لڑیاں(ثابت USetAdder *sa, UProperty which, UError_کوڈ &error_کوڈ) ثابت {
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    إذا (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        أرجع;
    }
    UProperty پہلا_Prop = which, آخری_Prop = which;
    إذا (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        پہلا_Prop = UCHAR_BASIC_EMOJI;
        آخری_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    لكل (int32_t prop = پہلا_Prop; prop <= آخری_Prop; ++prop) {
        ثابت char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        إذا (trieUChars != خالی_سوچک) {
            UCharsTrie::Iterator iter(trieUChars, 0, error_کوڈ);
            طالما (iter.next(error_کوڈ)) {
                ثابت UnicodeString &s = iter.getString();
                sa->جوڑو_String(sa->سیٹ, s.get_بفر(), s.length());
            }
        }
    }
}

U_NAMESPACE_سرا
