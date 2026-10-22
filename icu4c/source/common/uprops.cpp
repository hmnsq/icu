// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uprops.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002feb24
*   created by: Markus W. Scherer
*
*   Implementations for mostly non-core Unicode character properties
*   stored in uprops.icu.
*
*   With the APIs implemented here, almost all properties files and
*   their associated implementation files are used from this file,
*   including those for normalization and case mappings.
*/

#شامل "unicode/utypes.h"
#شامل "unicode/uchar.h"
#شامل "unicode/ucptrie.h"
#شامل "unicode/udata.h"
#شامل "unicode/unorm2.h"
#شامل "unicode/uscript.h"
#شامل "unicode/ustring.h"
#شامل "unicode/utf16.h"
#شامل "cstring.h"
#شامل "emojiprops.h"
#شامل "mutex.h"
#شامل "normalizer2impl.h"
#شامل "umutex.h"
#شامل "ubidi_props.h"
#شامل "uprops.h"
#شامل "ucase.h"
#شامل "ucln_cmn.h"
#شامل "ulayout_props.h"
#شامل "ustr_imp.h"

U_NAMESPACE_USE

// Unicode text layout properties data -----------------------------------------

نامخلا {

icu::UInitOnce gLayoutInitOnce {};
UData_حافظہ *g_Layout_حافظہ = خالی_اشاریہ;

UCPTrie *gInpcTrie = خالی_اشاریہ;  // Indic_Positional_Category
UCPTrie *gInscTrie = خالی_اشاریہ;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = خالی_اشاریہ;  // Vertical_Orientation

int32_t g_Max_Inpc_قدر = 0;
int32_t g_Max_Insc_قدر = 0;
int32_t g_Max_Vo_قدر = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(g_Layout_حافظہ);
    g_Layout_حافظہ = خالی_اشاریہ;

    ucptrie_close(gInpcTrie);
    gInpcTrie = خالی_اشاریہ;
    ucptrie_close(gInscTrie);
    gInscTrie = خالی_اشاریہ;
    ucptrie_close(gVoTrie);
    gVoTrie = خالی_اشاریہ;

    g_Max_Inpc_قدر = 0;
    g_Max_Insc_قدر = 0;
    g_Max_Vo_قدر = 0;

    gLayoutInitOnce.reset();
    واپس سچ;
}

UBool U_CALLCONV
ulayout_isAcceptable(خالی * /*context*/,
                     ثابت حرف * /* type */, ثابت حرف * /*name*/,
                     ثابت UDataInfo *pInfo) {
    واپس pInfo->size >= 20 &&
        pInfo->isBigEndian == U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily == U_CHARSET_FAMILY &&
        pInfo->ڈیٹا_Format[0] == ULAYOUT_FMT_0 &&
        pInfo->ڈیٹا_Format[1] == ULAYOUT_FMT_1 &&
        pInfo->ڈیٹا_Format[2] == ULAYOUT_FMT_2 &&
        pInfo->ڈیٹا_Format[3] == ULAYOUT_FMT_3 &&
        pInfo->format_نسخہ[0] == 1;
}

// UInitOnce singleton initialization function
خالی U_CALLCONV ulayout_بھرو(UError_رمز &error_رمز) {
    g_Layout_حافظہ = udata_openChoice(
        خالی_اشاریہ, ULAYOUT_ڈیٹا_TYPE, ULAYOUT_ڈیٹا_NAME,
        ulayout_isAcceptable, خالی_اشاریہ, &error_رمز);
    اگر (U_FAILURE(error_رمز)) { واپس; }

    ثابت uint8_t* اندر_بائٹیں = static_cast<ثابت uint8_t*>(udata_get_حافظہ(g_Layout_حافظہ));
    ثابت int32_t* اندر_Indexes = reinterpret_cast<ثابت int32_t*>(اندر_بائٹیں);
    int32_t indexes_لمبائی = اندر_Indexes[ULAYOUT_IX_INDEXES_لمبائی];
    اگر (indexes_لمبائی < 12) {
        error_رمز = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        واپس;
    }
    int32_t انحراف = indexes_لمبائی * 4;
    int32_t چوٹی = اندر_Indexes[ULAYOUT_IX_INPC_TRIE_چوٹی];
    int32_t trie_سائز = چوٹی - انحراف;
    اگر (trie_سائز >= 16) {
        gInpcTrie = ucptrie_open_From_ثنائی(
            UCPTRIE_TYPE_ANY, UCPTRIE_قدر_بٹ_ANY,
            اندر_بائٹیں + انحراف, trie_سائز, خالی_اشاریہ, &error_رمز);
    }
    انحراف = چوٹی;
    چوٹی = اندر_Indexes[ULAYOUT_IX_INSC_TRIE_چوٹی];
    trie_سائز = چوٹی - انحراف;
    اگر (trie_سائز >= 16) {
        gInscTrie = ucptrie_open_From_ثنائی(
            UCPTRIE_TYPE_ANY, UCPTRIE_قدر_بٹ_ANY,
            اندر_بائٹیں + انحراف, trie_سائز, خالی_اشاریہ, &error_رمز);
    }
    انحراف = چوٹی;
    چوٹی = اندر_Indexes[ULAYOUT_IX_VO_TRIE_چوٹی];
    trie_سائز = چوٹی - انحراف;
    اگر (trie_سائز >= 16) {
        gVoTrie = ucptrie_open_From_ثنائی(
            UCPTRIE_TYPE_ANY, UCPTRIE_قدر_بٹ_ANY,
            اندر_بائٹیں + انحراف, trie_سائز, خالی_اشاریہ, &error_رمز);
    }

    uint32_t max_اقدار = اندر_Indexes[ULAYOUT_IX_MAX_اقدار];
    g_Max_Inpc_قدر = max_اقدار >> ULAYOUT_MAX_INPC_سرکاؤ;
    g_Max_Insc_قدر = (max_اقدار >> ULAYOUT_MAX_INSC_سرکاؤ) & 0xff;
    g_Max_Vo_قدر = (max_اقدار >> ULAYOUT_MAX_VO_سرکاؤ) & 0xff;

    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensure_ڈیٹا(UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_بھرو, error_رمز);
    واپس U_SUCCESS(error_رمز);
}

UBool ulayout_ensure_ڈیٹا() {
    UError_رمز error_رمز = U_ZERO_ERROR;
    واپس ulayout_ensure_ڈیٹا(error_رمز);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

ساخت ثنائی_Property;

قسمتعریف UBool ثنائی_Property_شامل_ہے(ثابت ثنائی_Property &prop, UChar32 c, UProperty which);

ساخت ثنائی_Property {
    int32_t عمود;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    ثنائی_Property_شامل_ہے *شامل_ہے;
};

ساکن UBool default_شامل_ہے(ثابت ثنائی_Property &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    واپس (u_getUnicodeProperties(c, prop.عمود)&prop.mask)!=0;
}

ساکن UBool case_ثنائی_Property_شامل_ہے(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty which) {
    واپس ucase_has_ثنائی_Property(c, which);
}

ساکن UBool isBidiControl(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس ubidi_isBidiControl(c);
}

ساکن UBool isMirrored(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس ubidi_isMirrored(c);
}

ساکن UBool isJoinControl(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس ubidi_isJoinControl(c);
}

#اگر UCONFIG_NO_NORMALIZATION
ساکن UBool has_بھرا_Composition_Exclusion(ثابت ثنائی_Property &, UChar32, UProperty) {
    واپس جھوٹ;
}
#ورنہ
ساکن UBool has_بھرا_Composition_Exclusion(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UError_رمز error_رمز=U_ZERO_ERROR;
    ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_رمز);
    واپس U_SUCCESS(error_رمز) && impl->isCompNo(impl->getNorm16(c));
}
#اگر_ختم

// UCHAR_NF*_INERT properties
#اگر UCONFIG_NO_NORMALIZATION
ساکن UBool isNormInert(ثابت ثنائی_Property &, UChar32, UProperty) {
    واپس جھوٹ;
}
#ورنہ
ساکن UBool isNormInert(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty which) {
    UError_رمز error_رمز=U_ZERO_ERROR;
    ثابت Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalization_منوال>(which - UCHAR_NFD_INERT + UNORM_NFD), error_رمز);
    واپس U_SUCCESS(error_رمز) && norm2->isInert(c);
}
#اگر_ختم

#اگر UCONFIG_NO_NORMALIZATION
ساکن UBool changesWhenCasefolded(ثابت ثنائی_Property &, UChar32, UProperty) {
    واپس جھوٹ;
}
#ورنہ
ساکن UBool changesWhenCasefolded(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UError_رمز error_رمز=U_ZERO_ERROR;
    ثابت Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(error_رمز);
    اگر(U_FAILURE(error_رمز)) {
        واپس جھوٹ;
    }
    اگر(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        اگر(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } ورنہ اگر(nfd.length()<=U16_MAX_لمبائی &&
                  nfd.length()==U16_لمبائی(c=nfd.char32At(0))
        ) {
            /* single supplementary code point */
        } ورنہ {
            c=U_SENTINEL;
        }
    } ورنہ اگر(c<0) {
        واپس جھوٹ;  /* protect against bad input */
    }
    اگر(c>=0) {
        /* single code point */
        ثابت char16_t *نتیجہ_String;
        واپس ucase_مقصد_بھرا_Folding(c, &نتیجہ_String, U_FOLD_CASE_DEFAULT) >= 0;
    } ورنہ {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_لمبائی];
        int32_t dest_لمبائی;
        dest_لمبائی=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.get_بفر(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &error_رمز);
        واپس U_SUCCESS(error_رمز) &&
                       0!=u_strCompare(nfd.get_بفر(), nfd.length(),
                                       dest, dest_لمبائی, جھوٹ);
    }
}
#اگر_ختم

#اگر UCONFIG_NO_NORMALIZATION
ساکن UBool changesWhenNFKC_Casefolded(ثابت ثنائی_Property &, UChar32, UProperty) {
    واپس جھوٹ;
}
#ورنہ
ساکن UBool changesWhenNFKC_Casefolded(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_رمز error_رمز=U_ZERO_ERROR;
    ثابت Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(error_رمز);
    اگر(U_FAILURE(error_رمز)) {
        واپس جھوٹ;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        Reordering_بفر بفر(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        اگر(بفر.init(5, error_رمز)) {
            ثابت char16_t *srcArray=src.get_بفر();
            kcf->compose(srcArray, srcArray+src.length(), جھوٹ,
                          سچ, بفر, error_رمز);
        }
    }
    واپس U_SUCCESS(error_رمز) && dest!=src;
}
#اگر_ختم

#اگر UCONFIG_NO_NORMALIZATION
ساکن UBool isCanonSegmentStarter(ثابت ثنائی_Property &, UChar32, UProperty) {
    واپس جھوٹ;
}
#ورنہ
ساکن UBool isCanonSegmentStarter(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_رمز error_رمز=U_ZERO_ERROR;
    ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_رمز);
    واپس
        U_SUCCESS(error_رمز) && impl->ensure_Canon_Iter_ڈیٹا(error_رمز) &&
        impl->isCanonSegmentStarter(c);
}
#اگر_ختم

ساکن UBool isPOSIX_alnum(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس u_isalnumPOSIX(c);
}

ساکن UBool isPOSIX_blank(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس u_isblank(c);
}

ساکن UBool is_POSIX_گراف(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس u_isgraphPOSIX(c);
}

ساکن UBool isPOSIX_print(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس u_isprintPOSIX(c);
}

ساکن UBool isPOSIX_xdigit(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس u_isxdigit(c);
}

ساکن UBool isRegionalIndicator(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    واپس 0x1F1E6<=c && c<=0x1F1FF;
}

ساکن UBool hasEmojiProperty(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty which) {
    واپس EmojiProps::has_ثنائی_Property(c, which);
}

ساکن UBool isIDSUnaryOperator(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // New in Unicode 15.1 for just two characters.
    واپس 0x2FFE<=c && c<=0x2FFF;
}

/** Ranges (start/limit pairs) of ID_Compat_Math_Continue (only), from UCD PropList.txt. */
ساکن constexpr UChar32 ID_COMPAT_MATH_CONTINUE[] = {
    0x00B2, 0x00B3 + 1,
    0x00B9, 0x00B9 + 1,
    0x2070, 0x2070 + 1,
    0x2074, 0x207E + 1,
    0x2080, 0x208E + 1
};

/** ID_Compat_Math_Start characters, from UCD PropList.txt. */
ساکن constexpr UChar32 ID_COMPAT_MATH_آغاز[] = {
    0x2202,
    0x2207,
    0x221E,
    0x1D6C1,
    0x1D6DB,
    0x1D6FB,
    0x1D715,
    0x1D735,
    0x1D74F,
    0x1D76F,
    0x1D789,
    0x1D7A9,
    0x1D7C3
};

/** Ranges (start/limit pairs) of Modifier_Combining_Mark (only), from UCD PropList.txt. */
ساکن constexpr UChar32 MODIFIER_COMBINING_MARK[] = {
    0x0654, 0x0655 + 1,
    0x0658, 0x0658 + 1, // U+0658
    0x06DC, 0x06DC + 1, // U+06DC
    0x06E3, 0x06E3 + 1, // U+06E3
    0x06E7, 0x06E8 + 1,
    0x08CA, 0x08CB + 1,
    0x08CD, 0x08CF + 1,
    0x08D3, 0x08D3 + 1, // U+08D3
    0x08F3, 0x08F3 + 1, // U+08F3
    0x10EF4, 0x10EF4 + 1,
    0x10EF6, 0x10EF6 + 1,
    0x10EF9, 0x10EF9 + 1,
};

ساکن UBool is_IDCompat_Math_آغاز(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    اگر (c < ID_COMPAT_MATH_آغاز[0]) { واپس جھوٹ; }  // fastpath for common scripts
    برائے (UChar32 آغاز_Char : ID_COMPAT_MATH_آغاز) {
        اگر (c == آغاز_Char) { واپس سچ; }
    }
    واپس جھوٹ;
}

ساکن UBool isIDCompatMathContinue(ثابت ثنائی_Property &prop, UChar32 c, UProperty /*which*/) {
    برائے (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        اگر (c < ID_COMPAT_MATH_CONTINUE[i]) { واپس جھوٹ; }  // below range start
        اگر (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { واپس سچ; }  // below range limit
    }
    واپس is_IDCompat_Math_آغاز(prop, c, UCHAR_ID_COMPAT_MATH_آغاز);
}

ساکن UBool isModifierCombiningMark(ثابت ثنائی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    برائے (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        اگر (c < MODIFIER_COMBINING_MARK[i]) { واپس جھوٹ; }  // below range start
        اگر (c < MODIFIER_COMBINING_MARK[i + 1]) { واپس سچ; }  // below range limit
    }
    واپس جھوٹ;
}

ساکن ثابت ثنائی_Property binProps[UCHAR_ثنائی_حد_بندی]={
    /*
     * column and mask values for binary properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per binary UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { 1,                U_نقاب(UPROPS_ALPHABETIC), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_ASCII_HEX_DIGIT), default_شامل_ہے },
    { UPROPS_SRC_BIDI,  0, isBidiControl },
    { UPROPS_SRC_BIDI,  0, isMirrored },
    { 1,                U_نقاب(UPROPS_DASH), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_DEFAULT_IGNORABLE_رمز_نقطہ), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_DEPRECATED), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_DIACRITIC), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_EXTENDER), default_شامل_ہے },
    { UPROPS_SRC_NFC,   0, has_بھرا_Composition_Exclusion },
    { 1,                U_نقاب(UPROPS_GRAPHEME_بنیاد), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_GRAPHEME_EXTEND), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_GRAPHEME_LINK), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_HEX_DIGIT), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_HYPHEN), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_ID_CONTINUE), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_ID_آغاز), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_IDEOGRAPHIC), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_IDS_ثنائی_OPERATOR), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_IDS_TRINARY_OPERATOR), default_شامل_ہے },
    { UPROPS_SRC_BIDI,  0, isJoinControl },
    { 1,                U_نقاب(UPROPS_LOGICAL_ترتیب_EXCEPTION), default_شامل_ہے },
    { UPROPS_SRC_CASE,  0, case_ثنائی_Property_شامل_ہے },  // UCHAR_LOWERCASE
    { 1,                U_نقاب(UPROPS_MATH), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_NONCHARACTER_رمز_نقطہ), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_QUOTATION_MARK), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_RADICAL), default_شامل_ہے },
    { UPROPS_SRC_CASE,  0, case_ثنائی_Property_شامل_ہے },  // UCHAR_SOFT_DOTTED
    { 1,                U_نقاب(UPROPS_TERMINAL_PUNCTUATION), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_UNIFIED_IDEOGRAPH), default_شامل_ہے },
    { UPROPS_SRC_CASE,  0, case_ثنائی_Property_شامل_ہے },  // UCHAR_UPPERCASE
    { 1,                U_نقاب(UPROPS_WHITE_SPACE), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_XID_CONTINUE), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_XID_آغاز), default_شامل_ہے },
    { UPROPS_SRC_CASE,  0, case_ثنائی_Property_شامل_ہے },  // UCHAR_CASE_SENSITIVE
    { 1,                U_نقاب(UPROPS_S_رکن), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_VARIATION_SELECTOR), default_شامل_ہے },
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFD_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKD_INERT
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFC_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKC_INERT
    { UPROPS_SRC_NFC_CANON_ITER, 0, isCanonSegmentStarter },
    { 1,                U_نقاب(UPROPS_نمونہ_SYNTAX), default_شامل_ہے },
    { 1,                U_نقاب(UPROPS_نمونہ_WHITE_SPACE), default_شامل_ہے },
    { UPROPS_SRC_CHAR_AND_PROPSVEC,  0, isPOSIX_alnum },
    { UPROPS_SRC_CHAR,  0, isPOSIX_blank },
    { UPROPS_SRC_CHAR,  0, is_POSIX_گراف },
    { UPROPS_SRC_CHAR,  0, isPOSIX_print },
    { UPROPS_SRC_CHAR,  0, isPOSIX_xdigit },
    { UPROPS_SRC_CASE,  0, case_ثنائی_Property_شامل_ہے },  // UCHAR_CASED
    { UPROPS_SRC_CASE,  0, case_ثنائی_Property_شامل_ہے },  // UCHAR_CASE_IGNORABLE
    { UPROPS_SRC_CASE,  0, case_ثنائی_Property_شامل_ہے },  // UCHAR_CHANGES_WHEN_LOWERCASED
    { UPROPS_SRC_CASE,  0, case_ثنائی_Property_شامل_ہے },  // UCHAR_CHANGES_WHEN_UPPERCASED
    { UPROPS_SRC_CASE,  0, case_ثنائی_Property_شامل_ہے },  // UCHAR_CHANGES_WHEN_TITLECASED
    { UPROPS_SRC_CASE_AND_NORM,  0, changesWhenCasefolded },
    { UPROPS_SRC_CASE,  0, case_ثنائی_Property_شامل_ہے },  // UCHAR_CHANGES_WHEN_CASEMAPPED
    { UPROPS_SRC_NFKC_CF, 0, changesWhenNFKC_Casefolded },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_PRESENTATION
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER_BASE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_COMPONENT
    { 2,                0, isRegionalIndicator },
    { 1,                U_نقاب(UPROPS_PREPENDED_CONCATENATION_MARK), default_شامل_ہے },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EXTENDED_PICTOGRAPHIC
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_BASIC_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_KEYCAP_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_MODIFIER_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_FLAG_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_TAG_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_ZWJ_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI
    { UPROPS_SRC_IDSU, 0, isIDSUnaryOperator }, // UCHAR_IDS_UNARY_OPERATOR
    { UPROPS_SRC_ID_COMPAT_MATH, 0, is_IDCompat_Math_آغاز }, // UCHAR_ID_COMPAT_MATH_START
    { UPROPS_SRC_ID_COMPAT_MATH, 0, isIDCompatMathContinue }, // UCHAR_ID_COMPAT_MATH_CONTINUE
    { UPROPS_SRC_MCM, 0 , isModifierCombiningMark }, // UCHAR_MODIFIER_COMBINING_MARK
};

U_CAPI UBool U_EXPORT2
u_has_ثنائی_Property(UChar32 c, UProperty which) {
    /* c is range-checked in the functions that are called from here */
    اگر(which<UCHAR_ثنائی_آغاز || UCHAR_ثنائی_حد_بندی<=which) {
        /* not a known binary property */
        واپس جھوٹ;
    } ورنہ {
        ثابت ثنائی_Property &prop=binProps[which];
        واپس prop.شامل_ہے(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    واپس u_has_ثنائی_Property(c, UCHAR_ID_آغاز);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    واپس u_has_ثنائی_Property(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_string_Has_ثنائی_Property(ثابت char16_t *s, int32_t length, UProperty which) {
    اگر (s == خالی_اشاریہ && length != 0) { واپس جھوٹ; }
    اگر (length == 1) {
        واپس u_has_ثنائی_Property(s[0], which);  // single code point
    } ورنہ اگر (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_اگلا(s, i, length, c);
        اگر (length > 0 ? i == length : s[i] == 0) {
            واپس u_has_ثنائی_Property(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    واپس UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::has_ثنائی_Property(s, length, which);
}

ساخت IntProperty;

قسمتعریف int32_t Int_Property_Get_قدر(ثابت IntProperty &prop, UChar32 c, UProperty which);
قسمتعریف int32_t Int_Property_Get_Max_قدر(ثابت IntProperty &prop, UProperty which);

ساخت IntProperty {
    int32_t عمود;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t سرکاؤ;  // =maxValue if getMaxValueFromShift() is used
    Int_Property_Get_قدر *get_قدر;
    Int_Property_Get_Max_قدر *get_Max_قدر;
};

ساکن int32_t default_Get_قدر(ثابت IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    واپس static_cast<int32_t>(u_getUnicodeProperties(c, prop.عمود) & prop.mask) >> prop.سرکاؤ;
}

ساکن int32_t default_Get_Max_قدر(ثابت IntProperty &prop, UProperty /*which*/) {
    واپس (uprv_get_Max_اقدار(prop.عمود)&prop.mask)>>prop.سرکاؤ;
}

ساکن int32_t get_Max_قدر_From_سرکاؤ(ثابت IntProperty &prop, UProperty /*which*/) {
    واپس prop.سرکاؤ;
}

ساکن int32_t getBiDiClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس static_cast<int32_t>(u_charDirection(c));
}

ساکن int32_t getBiDiPairedBracketType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

ساکن int32_t bi_Di_Get_Max_قدر(ثابت IntProperty &/*prop*/, UProperty which) {
    واپس ubidi_get_Max_قدر(which);
}

ساکن int32_t get_بلاک(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس static_cast<int32_t>(ublock_get_رمز(c));
}

ساکن int32_t بلاک_Get_Max_قدر(ثابت IntProperty &/*prop*/, UProperty /*which*/) {
    واپس uprv_get_Max_اقدار(UPROPS_MAX_اقدار_OTHER_اشاریہ) & UPROPS_MAX_بلاک;
}

#اگر UCONFIG_NO_NORMALIZATION
ساکن int32_t getCombiningClass(ثابت IntProperty &, UChar32, UProperty) {
    واپس 0;
}
#ورنہ
ساکن int32_t getCombiningClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس u_getCombiningClass(c);
}
#اگر_ختم

ساکن int32_t getGeneralCategory(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس static_cast<int32_t>(u_charType(c));
}

ساکن int32_t get_Joining_گروہ(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس ubidi_get_Joining_گروہ(c);
}

ساکن int32_t getJoiningType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس ubidi_getJoiningType(c);
}

ساکن int32_t getNumericType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_قدر(u_getMainProperties(c)));
    واپس UPROPS_NTV_GET_TYPE(ntv);
}

ساکن int32_t getScript(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_رمز error_رمز=U_ZERO_ERROR;
    واپس static_cast<int32_t>(uscript_getScript(c, &error_رمز));
}

ساکن int32_t script_Get_Max_قدر(ثابت IntProperty &/*prop*/, UProperty /*which*/) {
    واپس uprv_get_Max_اقدار(0)&UPROPS_MAX_SCRIPT;
}

/*
 * Map some of the Grapheme Cluster Break values to Hangul Syllable Types.
 * Hangul_Syllable_Type used to be fully redundant with a subset of Grapheme_Cluster_Break.
 *
 * Starting with Unicode 16, this is no longer true for HST=V vs. GCB=V in some cases:
 * Some Kirat Rai vowels are given GCB=V for proper grapheme clustering, but
 * they are of course not related to Hangul syllables.
 */
ساکن ثابت UHangulSyllableType gcb_مقصد_Hst[]={
    U_HST_NOT_APPLICABLE,   /* U_GCB_OTHER */
    U_HST_NOT_APPLICABLE,   /* U_GCB_CONTROL */
    U_HST_NOT_APPLICABLE,   /* U_GCB_CR */
    U_HST_NOT_APPLICABLE,   /* U_GCB_EXTEND */
    U_HST_LEADING_JAMO,     /* U_GCB_L */
    U_HST_NOT_APPLICABLE,   /* U_GCB_LF */
    U_HST_LV_SYLLABLE,      /* U_GCB_LV */
    U_HST_LVT_SYLLABLE,     /* U_GCB_LVT */
    U_HST_TRAILING_JAMO,    /* U_GCB_T */
    U_HST_VOWEL_JAMO        /* U_GCB_V */
    /*
     * Omit GCB values beyond what we need for hst.
     * The code below checks for the array length.
     */
};

ساکن int32_t getHangulSyllableType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Ignore supplementary code points: They all have HST=NA.
    // This is a simple way to handle the GCB!=hst cases since Unicode 16 (Kirat Rai vowels).
    اگر(c>0xffff) {
        واپس U_HST_NOT_APPLICABLE;
    }
    /* see comments on gcbToHst[] above */
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_نقاب) >> UPROPS_GCB_سرکاؤ;
    اگر(gcb<UPRV_LENGTHOF(gcb_مقصد_Hst)) {
        واپس gcb_مقصد_Hst[gcb];
    } ورنہ {
        واپس U_HST_NOT_APPLICABLE;
    }
}

#اگر UCONFIG_NO_NORMALIZATION
ساکن int32_t get_Norm_تیز_Check(ثابت IntProperty &, UChar32, UProperty) {
    واپس 0;
}
#ورنہ
ساکن int32_t get_Norm_تیز_Check(ثابت IntProperty &/*prop*/, UChar32 c, UProperty which) {
    واپس static_cast<int32_t>(unorm_get_تیز_Check(c, static_cast<UNormalization_منوال>(which - UCHAR_NFD_تیز_CHECK + UNORM_NFD)));
}
#اگر_ختم

#اگر UCONFIG_NO_NORMALIZATION
ساکن int32_t getLeadCombiningClass(ثابت IntProperty &, UChar32, UProperty) {
    واپس 0;
}
#ورنہ
ساکن int32_t getLeadCombiningClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس unorm_getFCD16(c)>>8;
}
#اگر_ختم

#اگر UCONFIG_NO_NORMALIZATION
ساکن int32_t getTrailCombiningClass(ثابت IntProperty &, UChar32, UProperty) {
    واپس 0;
}
#ورنہ
ساکن int32_t getTrailCombiningClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    واپس unorm_getFCD16(c)&0xff;
}
#اگر_ختم

ساکن int32_t get_اندر_PC(ثابت IntProperty &, UChar32 c, UProperty) {
    واپس ulayout_ensure_ڈیٹا() && gInpcTrie != خالی_اشاریہ ? ucptrie_get(gInpcTrie, c) : 0;
}

ساکن int32_t get_اندر_SC(ثابت IntProperty &, UChar32 c, UProperty) {
    واپس ulayout_ensure_ڈیٹا() && gInscTrie != خالی_اشاریہ ? ucptrie_get(gInscTrie, c) : 0;
}

ساکن int32_t getVo(ثابت IntProperty &, UChar32 c, UProperty) {
    واپس ulayout_ensure_ڈیٹا() && gVoTrie != خالی_اشاریہ ? ucptrie_get(gVoTrie, c) : 0;
}

ساکن int32_t layout_Get_Max_قدر(ثابت IntProperty &/*prop*/, UProperty which) {
    اگر (!ulayout_ensure_ڈیٹا()) { واپس 0; }
    انتخاب (which) {
    حالت UCHAR_INDIC_POSITIONAL_CATEGORY:
        واپس g_Max_Inpc_قدر;
    حالت UCHAR_INDIC_SYLLABIC_CATEGORY:
        واپس g_Max_Insc_قدر;
    حالت UCHAR_VERTICAL_ORIENTATION:
        واپس g_Max_Vo_قدر;
    طےشدہ:
        واپس 0;
    }
}

ساکن int32_t get_IDStatus_قدر(ثابت IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_سرکاؤ;
    واپس value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_حالت_ALLOWED : U_ID_حالت_RESTRICTED;
}

ساکن ثابت IntProperty intProps[UCHAR_INT_حد_بندی-UCHAR_INT_آغاز]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, bi_Di_Get_Max_قدر },
    { UPROPS_SRC_بلاک, 0, 0,                               get_بلاک, بلاک_Get_Max_قدر },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, get_Max_قدر_From_سرکاؤ },
    { 2,                UPROPS_DT_نقاب, 0,                  default_Get_قدر, default_Get_Max_قدر },
    { 0,                UPROPS_EA_نقاب, UPROPS_EA_سرکاؤ,    default_Get_قدر, default_Get_Max_قدر },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_گنتی) - 1, getGeneralCategory, get_Max_قدر_From_سرکاؤ },
    { UPROPS_SRC_BIDI,  0, 0,                               get_Joining_گروہ, bi_Di_Get_Max_قدر },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, bi_Di_Get_Max_قدر },
    { 2,                UPROPS_LB_نقاب, UPROPS_LB_سرکاؤ,    default_Get_قدر, default_Get_Max_قدر },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_گنتی) - 1, getNumericType, get_Max_قدر_From_سرکاؤ },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, script_Get_Max_قدر },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_گنتی) - 1, getHangulSyllableType, get_Max_قدر_From_سرکاؤ },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), get_Norm_تیز_Check, get_Max_قدر_From_سرکاؤ },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), get_Norm_تیز_Check, get_Max_قدر_From_سرکاؤ },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), get_Norm_تیز_Check, get_Max_قدر_From_سرکاؤ },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), get_Norm_تیز_Check, get_Max_قدر_From_سرکاؤ },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, get_Max_قدر_From_سرکاؤ },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, get_Max_قدر_From_سرکاؤ },
    { 2,                UPROPS_GCB_نقاب, UPROPS_GCB_سرکاؤ,  default_Get_قدر, default_Get_Max_قدر },
    { 2,                UPROPS_SB_نقاب, UPROPS_SB_سرکاؤ,    default_Get_قدر, default_Get_Max_قدر },
    { 2,                UPROPS_WB_نقاب, UPROPS_WB_سرکاؤ,    default_Get_قدر, default_Get_Max_قدر },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, bi_Di_Get_Max_قدر },
    { UPROPS_SRC_INPC,  0, 0,                               get_اندر_PC, layout_Get_Max_قدر },
    { UPROPS_SRC_INSC,  0, 0,                               get_اندر_SC, layout_Get_Max_قدر },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layout_Get_Max_قدر },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_حالت_ALLOWED), get_IDStatus_قدر, get_Max_قدر_From_سرکاؤ },
    { 0,                UPROPS_INCB_نقاب, UPROPS_INCB_سرکاؤ,default_Get_قدر, default_Get_Max_قدر },
};

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_قدر(UChar32 c, UProperty which) {
    اگر(which<UCHAR_INT_آغاز) {
        اگر(UCHAR_ثنائی_آغاز<=which && which<UCHAR_ثنائی_حد_بندی) {
            ثابت ثنائی_Property &prop=binProps[which];
            واپس prop.شامل_ہے(prop, c, which);
        }
    } ورنہ اگر(which<UCHAR_INT_حد_بندی) {
        ثابت IntProperty &prop=intProps[which-UCHAR_INT_آغاز];
        واپس prop.get_قدر(prop, c, which);
    } ورنہ اگر(which==UCHAR_GENERAL_CATEGORY_نقاب) {
        واپس U_نقاب(u_charType(c));
    }
    واپس 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Min_قدر(UProperty /*which*/) {
    واپس 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Max_قدر(UProperty which) {
    اگر(which<UCHAR_INT_آغاز) {
        اگر(UCHAR_ثنائی_آغاز<=which && which<UCHAR_ثنائی_حد_بندی) {
            واپس 1;  // maximum true for all binary properties
        }
    } ورنہ اگر(which<UCHAR_INT_حد_بندی) {
        ثابت IntProperty &prop=intProps[which-UCHAR_INT_آغاز];
        واپس prop.get_Max_قدر(prop, which);
    }
    واپس -1;  // undefined
}

U_CFUNC UProperty_ماخذ U_EXPORT2
uprops_get_ماخذ(UProperty which) {
    اگر(which<UCHAR_ثنائی_آغاز) {
        واپس UPROPS_SRC_NONE; /* undefined */
    } ورنہ اگر(which<UCHAR_ثنائی_حد_بندی) {
        ثابت ثنائی_Property &prop=binProps[which];
        اگر(prop.mask!=0) {
            واپس UPROPS_SRC_PROPSVEC;
        } ورنہ {
            واپس (UProperty_ماخذ)prop.عمود;
        }
    } ورنہ اگر(which<UCHAR_INT_آغاز) {
        واپس UPROPS_SRC_NONE; /* undefined */
    } ورنہ اگر(which<UCHAR_INT_حد_بندی) {
        ثابت IntProperty &prop=intProps[which-UCHAR_INT_آغاز];
        اگر(prop.mask!=0) {
            واپس UPROPS_SRC_PROPSVEC;
        } ورنہ {
            واپس (UProperty_ماخذ)prop.عمود;
        }
    } ورنہ اگر(which<UCHAR_STRING_آغاز) {
        انتخاب(which) {
        حالت UCHAR_GENERAL_CATEGORY_نقاب:
        حالت UCHAR_NUMERIC_قدر:
            واپس UPROPS_SRC_CHAR;

        طےشدہ:
            واپس UPROPS_SRC_NONE;
        }
    } ورنہ اگر(which<UCHAR_STRING_حد_بندی) {
        انتخاب(which) {
        حالت UCHAR_AGE:
            واپس UPROPS_SRC_PROPSVEC;

        حالت UCHAR_BIDI_MIRRORING_GLYPH:
            واپس UPROPS_SRC_BIDI;

        حالت UCHAR_CASE_FOLDING:
        حالت UCHAR_LOWERCASE_MAPPING:
        حالت UCHAR_SIMPLE_CASE_FOLDING:
        حالت UCHAR_SIMPLE_LOWERCASE_MAPPING:
        حالت UCHAR_SIMPLE_TITLECASE_MAPPING:
        حالت UCHAR_SIMPLE_UPPERCASE_MAPPING:
        حالت UCHAR_TITLECASE_MAPPING:
        حالت UCHAR_UPPERCASE_MAPPING:
            واپس UPROPS_SRC_CASE;

        حالت UCHAR_ISO_COMMENT:
        حالت UCHAR_NAME:
        حالت UCHAR_UNICODE_1_NAME:
            واپس UPROPS_SRC_نام;

        طےشدہ:
            واپس UPROPS_SRC_NONE;
        }
    } ورنہ {
        انتخاب(which) {
        حالت UCHAR_SCRIPT_EXTENSIONS:
        حالت UCHAR_IDENTIFIER_TYPE:
            واپس UPROPS_SRC_PROPSVEC;
        طےشدہ:
            واپس UPROPS_SRC_NONE; /* undefined */
        }
    }
}

U_CFUNC خالی U_EXPORT2
uprops_جوڑو_Property_Starts(UProperty_ماخذ src, ثابت USetAdder *sa, UError_رمز *p_Error_رمز) {
    اگر (U_FAILURE(*p_Error_رمز)) { واپس; }
    اگر (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        برائے (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->جوڑو(sa->سیٹ, c);
        }
        // single characters
        برائے (UChar32 c : ID_COMPAT_MATH_آغاز) {
            sa->جوڑو(sa->سیٹ, c);
            sa->جوڑو(sa->سیٹ, c + 1);
        }
        واپس;
    }
    اگر (src == UPROPS_SRC_MCM) {
        // range limits
        برائے (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->جوڑو(sa->سیٹ, c);
        }
        واپس;
    }
    اگر (!ulayout_ensure_ڈیٹا(*p_Error_رمز)) { واپس; }
    ثابت UCPTrie *trie;
    انتخاب (src) {
    حالت UPROPS_SRC_INPC:
        trie = gInpcTrie;
        توڑو;
    حالت UPROPS_SRC_INSC:
        trie = gInscTrie;
        توڑو;
    حالت UPROPS_SRC_VO:
        trie = gVoTrie;
        توڑو;
    طےشدہ:
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    اگر (trie == خالی_اشاریہ) {
        *p_Error_رمز = U_MISSING_RESOURCE_ERROR;
        واپس;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 آغاز = 0, انتہا;
    جب ((انتہا = ucptrie_getRange(trie, آغاز, UCPMAP_RANGE_معمول, 0,
                                   خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ)) >= 0) {
        sa->جوڑو(sa->سیٹ, آغاز);
        آغاز = انتہا + 1;
    }
}

U_CAPI منطقی U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t type_اشاریہ = type;  // also guards against negative type integers
    اگر (type_اشاریہ >= UPRV_LENGTHOF(uprops_id_Type_مقصد_Encoded)) {
        واپس جھوٹ;
    }
    uint32_t encodedType = uprops_id_Type_مقصد_Encoded[type_اشاریہ];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_سرکاؤ;
    اگر ((encodedType & UPROPS_ID_TYPE_ثنائی_ہندسہ) != 0) {
        واپس value < UPROPS_ID_TYPE_FORBIDDEN && (value & encodedType) != 0;
    } ورنہ {
        واپس value == encodedType;
    }
}

نامخلا {

خالی maybeAppendType(uint32_t value, uint32_t bit, UIdentifierType t,
                     UIdentifierType *types, int32_t &length, int32_t capacity) {
    اگر ((value & bit) != 0) {
        اگر (length < capacity) {
            types[length] = t;
        }
        ++length;
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UError_رمز *p_Error_رمز) {
    اگر (U_FAILURE(*p_Error_رمز)) { واپس 0; }
    اگر (capacity < 0 || (capacity > 0 && types == خالی_اشاریہ)) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_سرکاؤ;
    اگر ((value & UPROPS_ID_TYPE_FORBIDDEN) == UPROPS_ID_TYPE_FORBIDDEN ||
            value == UPROPS_ID_TYPE_NOT_CHARACTER) {
        // single value
        اگر (capacity > 0) {
            UIdentifierType t;
            انتخاب (value) {
                حالت UPROPS_ID_TYPE_NOT_CHARACTER: t = U_ID_TYPE_NOT_CHARACTER; توڑو;
                حالت UPROPS_ID_TYPE_DEPRECATED: t = U_ID_TYPE_DEPRECATED; توڑو;
                حالت UPROPS_ID_TYPE_DEFAULT_IGNORABLE: t = U_ID_TYPE_DEFAULT_IGNORABLE; توڑو;
                حالت UPROPS_ID_TYPE_NOT_NFKC: t = U_ID_TYPE_NOT_NFKC; توڑو;
                حالت UPROPS_ID_TYPE_INCLUSION: t = U_ID_TYPE_INCLUSION; توڑو;
                حالت UPROPS_ID_TYPE_RECOMMENDED: t = U_ID_TYPE_RECOMMENDED; توڑو;
                طےشدہ:
                    *p_Error_رمز = U_INVALID_FORMAT_ERROR;
                    واپس 0;
            }
            types[0] = t;
        } ورنہ {
            *p_Error_رمز = U_بفر_حد_سے_تجاوز_ERROR;
        }
        واپس 1;
    } ورنہ {
        // one or more combinable bits
        int32_t length = 0;
        maybeAppendType(value, UPROPS_ID_TYPE_NOT_XID, U_ID_TYPE_NOT_XID,
                        types, length, capacity);
        maybeAppendType(value, UPROPS_ID_TYPE_EXCLUSION, U_ID_TYPE_EXCLUSION,
                        types, length, capacity);
        maybeAppendType(value, UPROPS_ID_TYPE_OBSOLETE, U_ID_TYPE_OBSOLETE,
                        types, length, capacity);
        maybeAppendType(value, UPROPS_ID_TYPE_TECHNICAL, U_ID_TYPE_TECHNICAL,
                        types, length, capacity);
        maybeAppendType(value, UPROPS_ID_TYPE_UNCOMMON_USE, U_ID_TYPE_UNCOMMON_USE,
                        types, length, capacity);
        maybeAppendType(value, UPROPS_ID_TYPE_LIMITED_USE, U_ID_TYPE_LIMITED_USE,
                        types, length, capacity);
        اگر (length >= capacity) {
            *p_Error_رمز = U_بفر_حد_سے_تجاوز_ERROR;
        }
        واپس length;
    }
}

#اگر !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_get_FC_NFKC_احاطہ(UChar32 c, char16_t *dest, int32_t dest_استعداد, UError_رمز *p_Error_رمز) {
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر(dest_استعداد<0 || (dest==خالی_اشاریہ && dest_استعداد>0)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    ثابت Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    ثابت char16_t *folded1;
    int32_t folded1_لمبائی=ucase_مقصد_بھرا_Folding(c, &folded1, U_FOLD_CASE_DEFAULT);
    اگر(folded1_لمبائی<0) {
        ثابت Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        اگر(nfkcImpl->get_Comp_تیز_Check(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            واپس u_terminateUChars(dest, dest_استعداد, 0, p_Error_رمز);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.سیٹ_مقصد(c);
    } ورنہ {
        اگر(folded1_لمبائی>UCASE_MAX_STRING_لمبائی) {
            folded1String.سیٹ_مقصد(folded1_لمبائی);
        } ورنہ {
            folded1String.سیٹ_مقصد(جھوٹ, folded1, folded1_لمبائی);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *p_Error_رمز);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *p_Error_رمز);
    // if (c != b) add the mapping from a to c
    اگر(U_FAILURE(*p_Error_رمز) || kc1==kc2) {
        واپس u_terminateUChars(dest, dest_استعداد, 0, p_Error_رمز);
    } ورنہ {
        واپس kc2.extract(dest, dest_استعداد, *p_Error_رمز);
    }
}

#اگر_ختم
