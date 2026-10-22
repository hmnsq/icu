// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// emojiprops.cpp
// created: 2021sep04 Markus W. Scherer

#شامل "unicode/utypes.h"
#شامل "unicode/uchar.h"
#شامل "unicode/ucharstrie.h"
#شامل "unicode/ucptrie.h"
#شامل "unicode/udata.h"
#شامل "unicode/ustringtrie.h"
#شامل "unicode/utf16.h"
#شامل "emojiprops.h"
#شامل "ucln.h"
#شامل "ucln_cmn.h"
#شامل "umutex.h"
#شامل "uset_imp.h"

U_NAMESPACE_BEGIN

نامخلا {

EmojiProps *singleton = خالی_اشاریہ;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    حذف singleton;
    singleton = خالی_اشاریہ;
    emojiInitOnce.reset();
    واپس سچ;
}

خالی U_CALLCONV initSingleton(UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس; }
    singleton = نیا EmojiProps(error_رمز);
    اگر (singleton == خالی_اشاریہ) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
    } ورنہ اگر (U_FAILURE(error_رمز)) {
        حذف singleton;
        singleton = خالی_اشاریہ;
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        ثابت UDataInfo &info, ثابت char16_t *ڈیٹا_Format, uint8_t major, uint8_t minor) {
    واپس
        info.size >= 20 &&
        info.isBigEndian == U_IS_BIG_ENDIAN &&
        info.charsetFamily == U_CHARSET_FAMILY &&
        info.ڈیٹا_Format[0] == ڈیٹا_Format[0] &&
        info.ڈیٹا_Format[1] == ڈیٹا_Format[1] &&
        info.ڈیٹا_Format[2] == ڈیٹا_Format[2] &&
        info.ڈیٹا_Format[3] == ڈیٹا_Format[3] &&
        info.format_نسخہ[0] == major &&
        info.format_نسخہ[1] >= minor;
}

}  // namespace

EmojiProps::~EmojiProps() {
    udata_close(حافظہ);
    ucptrie_close(cpTrie);
}

ثابت EmojiProps *
EmojiProps::getSingleton(UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    umtx_initOnce(emojiInitOnce, &initSingleton, error_رمز);
    واپس singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(خالی * /*context*/, ثابت حرف * /*type*/, ثابت حرف * /*name*/,
                         ثابت UDataInfo *pInfo) {
    واپس udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

خالی
EmojiProps::load(UError_رمز &error_رمز) {
    حافظہ = udata_openChoice(خالی_اشاریہ, "icu", "uemoji", isAcceptable, یہ, &error_رمز);
    اگر (U_FAILURE(error_رمز)) { واپس; }
    ثابت uint8_t* اندر_بائٹیں = static_cast<ثابت uint8_t*>(udata_get_حافظہ(حافظہ));
    ثابت int32_t* اندر_Indexes = reinterpret_cast<ثابت int32_t*>(اندر_بائٹیں);
    int32_t indexes_لمبائی = اندر_Indexes[IX_CPTRIE_انحراف] / 4;
    اگر (indexes_لمبائی <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_انحراف) {
        error_رمز = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        واپس;
    }

    int32_t i = IX_CPTRIE_انحراف;
    int32_t انحراف = اندر_Indexes[i++];
    int32_t اگلا_انحراف = اندر_Indexes[i];
    cpTrie = ucptrie_open_From_ثنائی(UCPTRIE_TYPE_FAST, UCPTRIE_قدر_بٹ_8,
                                    اندر_بائٹیں + انحراف, اگلا_انحراف - انحراف, خالی_اشاریہ, &error_رمز);
    اگر (U_FAILURE(error_رمز)) {
        واپس;
    }

    برائے (i = IX_BASIC_EMOJI_TRIE_انحراف; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_انحراف; ++i) {
        انحراف = اندر_Indexes[i];
        اگلا_انحراف = اندر_Indexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        ثابت char16_t* p = اگلا_انحراف > انحراف ? reinterpret_cast<ثابت char16_t*>(اندر_بائٹیں + انحراف) : خالی_اشاریہ;
        stringTries[get_String_Trie_اشاریہ(i)] = p;
    }
}

خالی
EmojiProps::جوڑو_Property_Starts(ثابت USetAdder *sa, UError_رمز & /*errorCode*/) ثابت {
    // Add the start code point of each same-value range of the trie.
    UChar32 آغاز = 0, انتہا;
    uint32_t value;
    جب ((انتہا = ucptrie_getRange(cpTrie, آغاز, UCPMAP_RANGE_معمول, 0,
                                   خالی_اشاریہ, خالی_اشاریہ, &value)) >= 0) {
        sa->جوڑو(sa->سیٹ, آغاز);
        آغاز = انتہا + 1;
    }
}

UBool
EmojiProps::has_ثنائی_Property(UChar32 c, UProperty which) {
    UError_رمز error_رمز = U_ZERO_ERROR;
    ثابت EmojiProps *ep = getSingleton(error_رمز);
    واپس U_SUCCESS(error_رمز) && ep->has_ثنائی_Property_Impl(c, which);
}

UBool
EmojiProps::has_ثنائی_Property_Impl(UChar32 c, UProperty which) ثابت {
    اگر (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        واپس جھوٹ;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    ساکن constexpr int8_t ثنائی_ہندسہ_Flags[] = {
        ثنائی_ہندسہ_EMOJI,                  // UCHAR_EMOJI=57
        ثنائی_ہندسہ_EMOJI_PRESENTATION,     // UCHAR_EMOJI_PRESENTATION=58
        ثنائی_ہندسہ_EMOJI_MODIFIER,         // UCHAR_EMOJI_MODIFIER=59
        ثنائی_ہندسہ_EMOJI_MODIFIER_بنیاد,    // UCHAR_EMOJI_MODIFIER_BASE=60
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
    اگر (bit < 0) {
        واپس جھوٹ;  // not a property that we support in this function
    }
    uint8_t بٹ = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    واپس (بٹ >> bit) & 1;
}

UBool
EmojiProps::has_ثنائی_Property(ثابت char16_t *s, int32_t length, UProperty which) {
    UError_رمز error_رمز = U_ZERO_ERROR;
    ثابت EmojiProps *ep = getSingleton(error_رمز);
    واپس U_SUCCESS(error_رمز) && ep->has_ثنائی_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_ثنائی_Property_Impl(ثابت char16_t *s, int32_t length, UProperty which) ثابت {
    اگر (s == خالی_اشاریہ && length != 0) { واپس جھوٹ; }
    اگر (length <= 0 && (length == 0 || *s == 0)) { واپس جھوٹ; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    اگر (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        واپس جھوٹ;
    }
    UProperty پہلا_Prop = which, آخری_Prop = which;
    اگر (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        پہلا_Prop = UCHAR_BASIC_EMOJI;
        آخری_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    برائے (int32_t prop = پہلا_Prop; prop <= آخری_Prop; ++prop) {
        ثابت char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        اگر (trieUChars != خالی_اشاریہ) {
            UCharsTrie trie(trieUChars);
            UString_Trie_نتیجہ نتیجہ = trie.next(s, length);
            اگر (USTRINGTRIE_HAS_قدر(نتیجہ)) {
                واپس سچ;
            }
        }
    }
    واپس جھوٹ;
}

خالی
EmojiProps::جوڑو_لڑیاں(ثابت USetAdder *sa, UProperty which, UError_رمز &error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) { واپس; }
    اگر (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        واپس;
    }
    UProperty پہلا_Prop = which, آخری_Prop = which;
    اگر (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        پہلا_Prop = UCHAR_BASIC_EMOJI;
        آخری_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    برائے (int32_t prop = پہلا_Prop; prop <= آخری_Prop; ++prop) {
        ثابت char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        اگر (trieUChars != خالی_اشاریہ) {
            UCharsTrie::Iterator iter(trieUChars, 0, error_رمز);
            جب (iter.next(error_رمز)) {
                ثابت UnicodeString &s = iter.getString();
                sa->جوڑو_String(sa->سیٹ, s.get_بفر(), s.length());
            }
        }
    }
}

U_NAMESPACE_انتہا
