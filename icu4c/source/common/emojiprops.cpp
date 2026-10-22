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

EmojiProps *singleton = خالي_اشارو;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    ميٽيو singleton;
    singleton = خالي_اشارو;
    emojiInitOnce.reset();
    أرجع سچ;
}

فراغ U_CALLCONV initSingleton(UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    singleton = جديد EmojiProps(error_ڪوڊ);
    إذا (singleton == خالي_اشارو) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
    } وإلا إذا (U_FAILURE(error_ڪوڊ)) {
        ميٽيو singleton;
        singleton = خالي_اشارو;
    }
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        ثابت UDataInfo &info, ثابت char16_t *معلومات_Format, uint8_t major, uint8_t minor) {
    أرجع
        info.size >= 20 &&
        info.isBigEndian == U_IS_BIG_ENDIAN &&
        info.charsetFamily == U_CHARSET_FAMILY &&
        info.معلومات_Format[0] == معلومات_Format[0] &&
        info.معلومات_Format[1] == معلومات_Format[1] &&
        info.معلومات_Format[2] == معلومات_Format[2] &&
        info.معلومات_Format[3] == معلومات_Format[3] &&
        info.format_نسخو[0] == major &&
        info.format_نسخو[1] >= minor;
}

}  // namespace

EmojiProps::~EmojiProps() {
    udata_close(يادگيري);
    ucptrie_close(cpTrie);
}

ثابت EmojiProps *
EmojiProps::getSingleton(UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    umtx_initOnce(emojiInitOnce, &initSingleton, error_ڪوڊ);
    أرجع singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(فراغ * /*context*/, ثابت حرف * /*type*/, ثابت حرف * /*name*/,
                         ثابت UDataInfo *pInfo) {
    أرجع udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

فراغ
EmojiProps::load(UError_ڪوڊ &error_ڪوڊ) {
    يادگيري = udata_openChoice(خالي_اشارو, "icu", "uemoji", isAcceptable, هذا, &error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    ثابت uint8_t* اندريون_بائيٽون = static_cast<ثابت uint8_t*>(udata_get_يادگيري(يادگيري));
    ثابت int32_t* اندريون_Indexes = reinterpret_cast<ثابت int32_t*>(اندريون_بائيٽون);
    int32_t indexes_ڊيگهائي = اندريون_Indexes[IX_CPTRIE_وٿي] / 4;
    إذا (indexes_ڊيگهائي <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_وٿي) {
        error_ڪوڊ = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        أرجع;
    }

    int32_t i = IX_CPTRIE_وٿي;
    int32_t وٿي = اندريون_Indexes[i++];
    int32_t ايندڙ_وٿي = اندريون_Indexes[i];
    cpTrie = ucptrie_open_From_ٻٽو(UCPTRIE_TYPE_FAST, UCPTRIE_قدر_بٽ_8,
                                    اندريون_بائيٽون + وٿي, ايندڙ_وٿي - وٿي, خالي_اشارو, &error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }

    لكل (i = IX_BASIC_EMOJI_TRIE_وٿي; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_وٿي; ++i) {
        وٿي = اندريون_Indexes[i];
        ايندڙ_وٿي = اندريون_Indexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        ثابت char16_t* p = ايندڙ_وٿي > وٿي ? reinterpret_cast<ثابت char16_t*>(اندريون_بائيٽون + وٿي) : خالي_اشارو;
        stringTries[get_String_Trie_اشاريو(i)] = p;
    }
}

فراغ
EmojiProps::جوڙيو_Property_Starts(ثابت USetAdder *sa, UError_ڪوڊ & /*errorCode*/) ثابت {
    // Add the start code point of each same-value range of the trie.
    UChar32 شروعات = 0, پڇاڙي;
    uint32_t value;
    طالما ((پڇاڙي = ucptrie_getRange(cpTrie, شروعات, UCPMAP_RANGE_عام, 0,
                                   خالي_اشارو, خالي_اشارو, &value)) >= 0) {
        sa->جوڙيو(sa->سيٽ, شروعات);
        شروعات = پڇاڙي + 1;
    }
}

UBool
EmojiProps::has_ٻٽو_Property(UChar32 c, UProperty which) {
    UError_ڪوڊ error_ڪوڊ = U_ZERO_ERROR;
    ثابت EmojiProps *ep = getSingleton(error_ڪوڊ);
    أرجع U_SUCCESS(error_ڪوڊ) && ep->has_ٻٽو_Property_Impl(c, which);
}

UBool
EmojiProps::has_ٻٽو_Property_Impl(UChar32 c, UProperty which) ثابت {
    إذا (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        أرجع خطأ;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    ساكن constexpr int8_t ٻٽو_انگ_Flags[] = {
        ٻٽو_انگ_EMOJI,                  // UCHAR_EMOJI=57
        ٻٽو_انگ_EMOJI_PRESENTATION,     // UCHAR_EMOJI_PRESENTATION=58
        ٻٽو_انگ_EMOJI_MODIFIER,         // UCHAR_EMOJI_MODIFIER=59
        ٻٽو_انگ_EMOJI_MODIFIER_بنياد,    // UCHAR_EMOJI_MODIFIER_BASE=60
        ٻٽو_انگ_EMOJI_COMPONENT,        // UCHAR_EMOJI_COMPONENT=61
        -1,                         // UCHAR_REGIONAL_INDICATOR=62
        -1,                         // UCHAR_PREPENDED_CONCATENATION_MARK=63
        ٻٽو_انگ_EXTENDED_PICTOGRAPHIC,  // UCHAR_EXTENDED_PICTOGRAPHIC=64
        ٻٽو_انگ_BASIC_EMOJI,            // UCHAR_BASIC_EMOJI=65
        -1,                         // UCHAR_EMOJI_KEYCAP_SEQUENCE=66
        -1,                         // UCHAR_RGI_EMOJI_MODIFIER_SEQUENCE=67
        -1,                         // UCHAR_RGI_EMOJI_FLAG_SEQUENCE=68
        -1,                         // UCHAR_RGI_EMOJI_TAG_SEQUENCE=69
        -1,                         // UCHAR_RGI_EMOJI_ZWJ_SEQUENCE=70
        ٻٽو_انگ_BASIC_EMOJI,            // UCHAR_RGI_EMOJI=71
    };
    int32_t bit = ٻٽو_انگ_Flags[which - UCHAR_EMOJI];
    إذا (bit < 0) {
        أرجع خطأ;  // not a property that we support in this function
    }
    uint8_t بٽ = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    أرجع (بٽ >> bit) & 1;
}

UBool
EmojiProps::has_ٻٽو_Property(ثابت char16_t *s, int32_t length, UProperty which) {
    UError_ڪوڊ error_ڪوڊ = U_ZERO_ERROR;
    ثابت EmojiProps *ep = getSingleton(error_ڪوڊ);
    أرجع U_SUCCESS(error_ڪوڊ) && ep->has_ٻٽو_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_ٻٽو_Property_Impl(ثابت char16_t *s, int32_t length, UProperty which) ثابت {
    إذا (s == خالي_اشارو && length != 0) { أرجع خطأ; }
    إذا (length <= 0 && (length == 0 || *s == 0)) { أرجع خطأ; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    إذا (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        أرجع خطأ;
    }
    UProperty پهريون_Prop = which, آخري_Prop = which;
    إذا (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        پهريون_Prop = UCHAR_BASIC_EMOJI;
        آخري_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    لكل (int32_t prop = پهريون_Prop; prop <= آخري_Prop; ++prop) {
        ثابت char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        إذا (trieUChars != خالي_اشارو) {
            UCharsTrie trie(trieUChars);
            UString_Trie_نتيجو نتيجو = trie.next(s, length);
            إذا (USTRINGTRIE_HAS_قدر(نتيجو)) {
                أرجع سچ;
            }
        }
    }
    أرجع خطأ;
}

فراغ
EmojiProps::جوڙيو_لڙيون(ثابت USetAdder *sa, UProperty which, UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    إذا (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        أرجع;
    }
    UProperty پهريون_Prop = which, آخري_Prop = which;
    إذا (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        پهريون_Prop = UCHAR_BASIC_EMOJI;
        آخري_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    لكل (int32_t prop = پهريون_Prop; prop <= آخري_Prop; ++prop) {
        ثابت char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        إذا (trieUChars != خالي_اشارو) {
            UCharsTrie::Iterator iter(trieUChars, 0, error_ڪوڊ);
            طالما (iter.next(error_ڪوڊ)) {
                ثابت UnicodeString &s = iter.getString();
                sa->جوڙيو_String(sa->سيٽ, s.get_بفر(), s.length());
            }
        }
    }
}

U_NAMESPACE_پڇاڙي
