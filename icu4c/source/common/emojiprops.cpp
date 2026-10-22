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

EmojiProps *singleton = تش_اشاره;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    ړنګول singleton;
    singleton = تش_اشاره;
    emojiInitOnce.reset();
    أرجع ریښتیا;
}

فراغ U_CALLCONV initSingleton(UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
    singleton = جديد EmojiProps(error_کوډ);
    إذا (singleton == تش_اشاره) {
        error_کوډ = U_حافظه_ویش_ERROR;
    } وإلا إذا (U_FAILURE(error_کوډ)) {
        ړنګول singleton;
        singleton = تش_اشاره;
    }
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
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
        info.format_نسخه[0] == major &&
        info.format_نسخه[1] >= minor;
}

}  // namespace

EmojiProps::~EmojiProps() {
    udata_close(حافظه);
    ucptrie_close(cpTrie);
}

ثابت EmojiProps *
EmojiProps::getSingleton(UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    umtx_initOnce(emojiInitOnce, &initSingleton, error_کوډ);
    أرجع singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(فراغ * /*context*/, ثابت حرف * /*type*/, ثابت حرف * /*name*/,
                         ثابت UDataInfo *pInfo) {
    أرجع udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

فراغ
EmojiProps::load(UError_کوډ &error_کوډ) {
    حافظه = udata_openChoice(تش_اشاره, "icu", "uemoji", isAcceptable, هذا, &error_کوډ);
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
    ثابت uint8_t* دننه_بایټونه = static_cast<ثابت uint8_t*>(udata_get_حافظه(حافظه));
    ثابت int32_t* دننه_Indexes = reinterpret_cast<ثابت int32_t*>(دننه_بایټونه);
    int32_t indexes_اوږدتیا = دننه_Indexes[IX_CPTRIE_دننه_واټن] / 4;
    إذا (indexes_اوږدتیا <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_دننه_واټن) {
        error_کوډ = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        أرجع;
    }

    int32_t i = IX_CPTRIE_دننه_واټن;
    int32_t دننه_واټن = دننه_Indexes[i++];
    int32_t بل_دننه_واټن = دننه_Indexes[i];
    cpTrie = ucptrie_open_From_دوګونی(UCPTRIE_TYPE_FAST, UCPTRIE_ارزښت_بټونه_8,
                                    دننه_بایټونه + دننه_واټن, بل_دننه_واټن - دننه_واټن, تش_اشاره, &error_کوډ);
    إذا (U_FAILURE(error_کوډ)) {
        أرجع;
    }

    لكل (i = IX_BASIC_EMOJI_TRIE_دننه_واټن; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_دننه_واټن; ++i) {
        دننه_واټن = دننه_Indexes[i];
        بل_دننه_واټن = دننه_Indexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        ثابت char16_t* p = بل_دننه_واټن > دننه_واټن ? reinterpret_cast<ثابت char16_t*>(دننه_بایټونه + دننه_واټن) : تش_اشاره;
        stringTries[get_String_Trie_شاخص(i)] = p;
    }
}

فراغ
EmojiProps::جمع_کړه_Property_Starts(ثابت USetAdder *sa, UError_کوډ & /*errorCode*/) ثابت {
    // Add the start code point of each same-value range of the trie.
    UChar32 پیل = 0, پای_ټکی;
    uint32_t value;
    طالما ((پای_ټکی = ucptrie_getRange(cpTrie, پیل, UCPMAP_RANGE_نورمال, 0,
                                   تش_اشاره, تش_اشاره, &value)) >= 0) {
        sa->جمع_کړه(sa->سېټ, پیل);
        پیل = پای_ټکی + 1;
    }
}

UBool
EmojiProps::has_دوګونی_Property(UChar32 c, UProperty which) {
    UError_کوډ error_کوډ = U_ZERO_ERROR;
    ثابت EmojiProps *ep = getSingleton(error_کوډ);
    أرجع U_SUCCESS(error_کوډ) && ep->has_دوګونی_Property_Impl(c, which);
}

UBool
EmojiProps::has_دوګونی_Property_Impl(UChar32 c, UProperty which) ثابت {
    إذا (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        أرجع خطأ;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    ساكن constexpr int8_t بټ_Flags[] = {
        بټ_EMOJI,                  // UCHAR_EMOJI=57
        بټ_EMOJI_PRESENTATION,     // UCHAR_EMOJI_PRESENTATION=58
        بټ_EMOJI_MODIFIER,         // UCHAR_EMOJI_MODIFIER=59
        بټ_EMOJI_MODIFIER_بنسټ,    // UCHAR_EMOJI_MODIFIER_BASE=60
        بټ_EMOJI_COMPONENT,        // UCHAR_EMOJI_COMPONENT=61
        -1,                         // UCHAR_REGIONAL_INDICATOR=62
        -1,                         // UCHAR_PREPENDED_CONCATENATION_MARK=63
        بټ_EXTENDED_PICTOGRAPHIC,  // UCHAR_EXTENDED_PICTOGRAPHIC=64
        بټ_BASIC_EMOJI,            // UCHAR_BASIC_EMOJI=65
        -1,                         // UCHAR_EMOJI_KEYCAP_SEQUENCE=66
        -1,                         // UCHAR_RGI_EMOJI_MODIFIER_SEQUENCE=67
        -1,                         // UCHAR_RGI_EMOJI_FLAG_SEQUENCE=68
        -1,                         // UCHAR_RGI_EMOJI_TAG_SEQUENCE=69
        -1,                         // UCHAR_RGI_EMOJI_ZWJ_SEQUENCE=70
        بټ_BASIC_EMOJI,            // UCHAR_RGI_EMOJI=71
    };
    int32_t bit = بټ_Flags[which - UCHAR_EMOJI];
    إذا (bit < 0) {
        أرجع خطأ;  // not a property that we support in this function
    }
    uint8_t بټونه = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    أرجع (بټونه >> bit) & 1;
}

UBool
EmojiProps::has_دوګونی_Property(ثابت char16_t *s, int32_t length, UProperty which) {
    UError_کوډ error_کوډ = U_ZERO_ERROR;
    ثابت EmojiProps *ep = getSingleton(error_کوډ);
    أرجع U_SUCCESS(error_کوډ) && ep->has_دوګونی_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_دوګونی_Property_Impl(ثابت char16_t *s, int32_t length, UProperty which) ثابت {
    إذا (s == تش_اشاره && length != 0) { أرجع خطأ; }
    إذا (length <= 0 && (length == 0 || *s == 0)) { أرجع خطأ; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    إذا (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        أرجع خطأ;
    }
    UProperty لومړی_Prop = which, وروستی_Prop = which;
    إذا (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        لومړی_Prop = UCHAR_BASIC_EMOJI;
        وروستی_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    لكل (int32_t prop = لومړی_Prop; prop <= وروستی_Prop; ++prop) {
        ثابت char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        إذا (trieUChars != تش_اشاره) {
            UCharsTrie trie(trieUChars);
            UString_Trie_پایله پایله = trie.next(s, length);
            إذا (USTRINGTRIE_HAS_ارزښت(پایله)) {
                أرجع ریښتیا;
            }
        }
    }
    أرجع خطأ;
}

فراغ
EmojiProps::جمع_کړه_مزي_ګانې(ثابت USetAdder *sa, UProperty which, UError_کوډ &error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
    إذا (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        أرجع;
    }
    UProperty لومړی_Prop = which, وروستی_Prop = which;
    إذا (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        لومړی_Prop = UCHAR_BASIC_EMOJI;
        وروستی_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    لكل (int32_t prop = لومړی_Prop; prop <= وروستی_Prop; ++prop) {
        ثابت char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        إذا (trieUChars != تش_اشاره) {
            UCharsTrie::Iterator iter(trieUChars, 0, error_کوډ);
            طالما (iter.next(error_کوډ)) {
                ثابت UnicodeString &s = iter.getString();
                sa->جمع_کړه_String(sa->سېټ, s.get_بفر(), s.length());
            }
        }
    }
}

U_NAMESPACE_پای_ټکی
