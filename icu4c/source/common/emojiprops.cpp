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

EmojiProps *singleton = اشاره_تهی;
icu::UInitOnce emojiInitOnce {};

UBool U_CALLCONV emojiprops_cleanup() {
    حذف singleton;
    singleton = اشاره_تهی;
    emojiInitOnce.reset();
    أرجع درست;
}

فراغ U_CALLCONV initSingleton(UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع; }
    singleton = جديد EmojiProps(error_کد);
    إذا (singleton == اشاره_تهی) {
        error_کد = U_حافظه_تخصیص_ERROR;
    } وإلا إذا (U_FAILURE(error_کد)) {
        حذف singleton;
        singleton = اشاره_تهی;
    }
    ucln_common_ثبات_Cleanup(UCLN_COMMON_EMOJIPROPS, emojiprops_cleanup);
}

// TODO: turn this into a shared helper function
// Requires the major version to match, and then requires at least the minor version.
UBool udata_isAcceptableMajorMinor(
        ثابت UDataInfo &info, ثابت char16_t *داده_Format, uint8_t major, uint8_t minor) {
    أرجع
        info.size >= 20 &&
        info.isBigEndian == U_IS_BIG_ENDIAN &&
        info.charsetFamily == U_CHARSET_FAMILY &&
        info.داده_Format[0] == داده_Format[0] &&
        info.داده_Format[1] == داده_Format[1] &&
        info.داده_Format[2] == داده_Format[2] &&
        info.داده_Format[3] == داده_Format[3] &&
        info.format_نسخه[0] == major &&
        info.format_نسخه[1] >= minor;
}

}  // namespace

EmojiProps::~EmojiProps() {
    udata_close(حافظه);
    ucptrie_close(cpTrie);
}

ثابت EmojiProps *
EmojiProps::getSingleton(UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    umtx_initOnce(emojiInitOnce, &initSingleton, error_کد);
    أرجع singleton;
}

UBool U_CALLCONV
EmojiProps::isAcceptable(فراغ * /*context*/, ثابت حرف * /*type*/, ثابت حرف * /*name*/,
                         ثابت UDataInfo *pInfo) {
    أرجع udata_isAcceptableMajorMinor(*pInfo, u"Emoj", 1, 0);
}

فراغ
EmojiProps::load(UError_کد &error_کد) {
    حافظه = udata_openChoice(اشاره_تهی, "icu", "uemoji", isAcceptable, هذا, &error_کد);
    إذا (U_FAILURE(error_کد)) { أرجع; }
    ثابت uint8_t* درون_بایتها = static_cast<ثابت uint8_t*>(udata_get_حافظه(حافظه));
    ثابت int32_t* درون_Indexes = reinterpret_cast<ثابت int32_t*>(درون_بایتها);
    int32_t indexes_درازی = درون_Indexes[IX_CPTRIE_جابه_جایی] / 4;
    إذا (indexes_درازی <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_جابه_جایی) {
        error_کد = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        أرجع;
    }

    int32_t i = IX_CPTRIE_جابه_جایی;
    int32_t جابه_جایی = درون_Indexes[i++];
    int32_t بعدی_جابه_جایی = درون_Indexes[i];
    cpTrie = ucptrie_open_From_دودویی(UCPTRIE_TYPE_FAST, UCPTRIE_مقدار_بیتها_8,
                                    درون_بایتها + جابه_جایی, بعدی_جابه_جایی - جابه_جایی, اشاره_تهی, &error_کد);
    إذا (U_FAILURE(error_کد)) {
        أرجع;
    }

    لكل (i = IX_BASIC_EMOJI_TRIE_جابه_جایی; i <= IX_RGI_EMOJI_ZWJ_SEQUENCE_TRIE_جابه_جایی; ++i) {
        جابه_جایی = درون_Indexes[i];
        بعدی_جابه_جایی = درون_Indexes[i + 1];
        // Set/leave nullptr if there is no UCharsTrie.
        ثابت char16_t* p = بعدی_جابه_جایی > جابه_جایی ? reinterpret_cast<ثابت char16_t*>(درون_بایتها + جابه_جایی) : اشاره_تهی;
        stringTries[get_String_Trie_شاخص(i)] = p;
    }
}

فراغ
EmojiProps::جمع_کن_Property_Starts(ثابت USetAdder *sa, UError_کد & /*errorCode*/) ثابت {
    // Add the start code point of each same-value range of the trie.
    UChar32 شروع = 0, انتها;
    uint32_t value;
    طالما ((انتها = ucptrie_getRange(cpTrie, شروع, UCPMAP_RANGE_نرمال, 0,
                                   اشاره_تهی, اشاره_تهی, &value)) >= 0) {
        sa->جمع_کن(sa->مجموعه, شروع);
        شروع = انتها + 1;
    }
}

UBool
EmojiProps::has_دودویی_Property(UChar32 c, UProperty which) {
    UError_کد error_کد = U_ZERO_ERROR;
    ثابت EmojiProps *ep = getSingleton(error_کد);
    أرجع U_SUCCESS(error_کد) && ep->has_دودویی_Property_Impl(c, which);
}

UBool
EmojiProps::has_دودویی_Property_Impl(UChar32 c, UProperty which) ثابت {
    إذا (which < UCHAR_EMOJI || UCHAR_RGI_EMOJI < which) {
        أرجع خطأ;
    }
    // Note: UCHAR_REGIONAL_INDICATOR is a single, hardcoded range implemented elsewhere.
    ساكن constexpr int8_t بیت_Flags[] = {
        بیت_EMOJI,                  // UCHAR_EMOJI=57
        بیت_EMOJI_PRESENTATION,     // UCHAR_EMOJI_PRESENTATION=58
        بیت_EMOJI_MODIFIER,         // UCHAR_EMOJI_MODIFIER=59
        بیت_EMOJI_MODIFIER_پایه,    // UCHAR_EMOJI_MODIFIER_BASE=60
        بیت_EMOJI_COMPONENT,        // UCHAR_EMOJI_COMPONENT=61
        -1,                         // UCHAR_REGIONAL_INDICATOR=62
        -1,                         // UCHAR_PREPENDED_CONCATENATION_MARK=63
        بیت_EXTENDED_PICTOGRAPHIC,  // UCHAR_EXTENDED_PICTOGRAPHIC=64
        بیت_BASIC_EMOJI,            // UCHAR_BASIC_EMOJI=65
        -1,                         // UCHAR_EMOJI_KEYCAP_SEQUENCE=66
        -1,                         // UCHAR_RGI_EMOJI_MODIFIER_SEQUENCE=67
        -1,                         // UCHAR_RGI_EMOJI_FLAG_SEQUENCE=68
        -1,                         // UCHAR_RGI_EMOJI_TAG_SEQUENCE=69
        -1,                         // UCHAR_RGI_EMOJI_ZWJ_SEQUENCE=70
        بیت_BASIC_EMOJI,            // UCHAR_RGI_EMOJI=71
    };
    int32_t bit = بیت_Flags[which - UCHAR_EMOJI];
    إذا (bit < 0) {
        أرجع خطأ;  // not a property that we support in this function
    }
    uint8_t بیتها = UCPTRIE_FAST_GET(cpTrie, UCPTRIE_8, c);
    أرجع (بیتها >> bit) & 1;
}

UBool
EmojiProps::has_دودویی_Property(ثابت char16_t *s, int32_t length, UProperty which) {
    UError_کد error_کد = U_ZERO_ERROR;
    ثابت EmojiProps *ep = getSingleton(error_کد);
    أرجع U_SUCCESS(error_کد) && ep->has_دودویی_Property_Impl(s, length, which);
}

UBool
EmojiProps::has_دودویی_Property_Impl(ثابت char16_t *s, int32_t length, UProperty which) ثابت {
    إذا (s == اشاره_تهی && length != 0) { أرجع خطأ; }
    إذا (length <= 0 && (length == 0 || *s == 0)) { أرجع خطأ; }  // empty string
    // The caller should have delegated single code points to hasBinaryProperty(c, which).
    إذا (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        أرجع خطأ;
    }
    UProperty اولی_Prop = which, آخرین_Prop = which;
    إذا (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        اولی_Prop = UCHAR_BASIC_EMOJI;
        آخرین_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    لكل (int32_t prop = اولی_Prop; prop <= آخرین_Prop; ++prop) {
        ثابت char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        إذا (trieUChars != اشاره_تهی) {
            UCharsTrie trie(trieUChars);
            UString_Trie_نتیجه نتیجه = trie.next(s, length);
            إذا (USTRINGTRIE_HAS_مقدار(نتیجه)) {
                أرجع درست;
            }
        }
    }
    أرجع خطأ;
}

فراغ
EmojiProps::جمع_کن_رشتهها(ثابت USetAdder *sa, UProperty which, UError_کد &error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) { أرجع; }
    إذا (which < UCHAR_BASIC_EMOJI || UCHAR_RGI_EMOJI < which) {
        أرجع;
    }
    UProperty اولی_Prop = which, آخرین_Prop = which;
    إذا (which == UCHAR_RGI_EMOJI) {
        // RGI_Emoji is the union of the other emoji properties of strings.
        اولی_Prop = UCHAR_BASIC_EMOJI;
        آخرین_Prop = UCHAR_RGI_EMOJI_ZWJ_SEQUENCE;
    }
    لكل (int32_t prop = اولی_Prop; prop <= آخرین_Prop; ++prop) {
        ثابت char16_t *trieUChars = stringTries[prop - UCHAR_BASIC_EMOJI];
        إذا (trieUChars != اشاره_تهی) {
            UCharsTrie::Iterator iter(trieUChars, 0, error_کد);
            طالما (iter.next(error_کد)) {
                ثابت UnicodeString &s = iter.getString();
                sa->جمع_کن_String(sa->مجموعه, s.get_میانگیر(), s.length());
            }
        }
    }
}

U_NAMESPACE_انتها
