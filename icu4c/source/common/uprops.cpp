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

#ضمن "unicode/utypes.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/unorm2.h"
#ضمن "unicode/uscript.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/utf16.h"
#ضمن "cstring.h"
#ضمن "emojiprops.h"
#ضمن "mutex.h"
#ضمن "normalizer2impl.h"
#ضمن "umutex.h"
#ضمن "ubidi_props.h"
#ضمن "uprops.h"
#ضمن "ucase.h"
#ضمن "ucln_cmn.h"
#ضمن "ulayout_props.h"
#ضمن "ustr_imp.h"

U_NAMESPACE_USE

// Unicode text layout properties data -----------------------------------------

نطاق {

icu::UInitOnce gLayoutInitOnce {};
UData_حافظه *g_Layout_حافظه = اشاره_تهی;

UCPTrie *gInpcTrie = اشاره_تهی;  // Indic_Positional_Category
UCPTrie *gInscTrie = اشاره_تهی;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = اشاره_تهی;  // Vertical_Orientation

int32_t g_Max_Inpc_مقدار = 0;
int32_t g_Max_Insc_مقدار = 0;
int32_t g_Max_Vo_مقدار = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(g_Layout_حافظه);
    g_Layout_حافظه = اشاره_تهی;

    ucptrie_close(gInpcTrie);
    gInpcTrie = اشاره_تهی;
    ucptrie_close(gInscTrie);
    gInscTrie = اشاره_تهی;
    ucptrie_close(gVoTrie);
    gVoTrie = اشاره_تهی;

    g_Max_Inpc_مقدار = 0;
    g_Max_Insc_مقدار = 0;
    g_Max_Vo_مقدار = 0;

    gLayoutInitOnce.reset();
    أرجع درست;
}

UBool U_CALLCONV
ulayout_isAcceptable(فراغ * /*context*/,
                     ثابت حرف * /* type */, ثابت حرف * /*name*/,
                     ثابت UDataInfo *pInfo) {
    أرجع pInfo->size >= 20 &&
        pInfo->isBigEndian == U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily == U_CHARSET_FAMILY &&
        pInfo->داده_Format[0] == ULAYOUT_FMT_0 &&
        pInfo->داده_Format[1] == ULAYOUT_FMT_1 &&
        pInfo->داده_Format[2] == ULAYOUT_FMT_2 &&
        pInfo->داده_Format[3] == ULAYOUT_FMT_3 &&
        pInfo->format_نسخه[0] == 1;
}

// UInitOnce singleton initialization function
فراغ U_CALLCONV ulayout_بارگیری(UError_کد &error_کد) {
    g_Layout_حافظه = udata_openChoice(
        اشاره_تهی, ULAYOUT_داده_TYPE, ULAYOUT_داده_NAME,
        ulayout_isAcceptable, اشاره_تهی, &error_کد);
    إذا (U_FAILURE(error_کد)) { أرجع; }

    ثابت uint8_t* درون_بایتها = static_cast<ثابت uint8_t*>(udata_get_حافظه(g_Layout_حافظه));
    ثابت int32_t* درون_Indexes = reinterpret_cast<ثابت int32_t*>(درون_بایتها);
    int32_t indexes_درازی = درون_Indexes[ULAYOUT_IX_INDEXES_درازی];
    إذا (indexes_درازی < 12) {
        error_کد = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        أرجع;
    }
    int32_t جابه_جایی = indexes_درازی * 4;
    int32_t بالاترین = درون_Indexes[ULAYOUT_IX_INPC_TRIE_بالاترین];
    int32_t trie_اندازه = بالاترین - جابه_جایی;
    إذا (trie_اندازه >= 16) {
        gInpcTrie = ucptrie_open_From_دودویی(
            UCPTRIE_TYPE_ANY, UCPTRIE_مقدار_بیتها_ANY,
            درون_بایتها + جابه_جایی, trie_اندازه, اشاره_تهی, &error_کد);
    }
    جابه_جایی = بالاترین;
    بالاترین = درون_Indexes[ULAYOUT_IX_INSC_TRIE_بالاترین];
    trie_اندازه = بالاترین - جابه_جایی;
    إذا (trie_اندازه >= 16) {
        gInscTrie = ucptrie_open_From_دودویی(
            UCPTRIE_TYPE_ANY, UCPTRIE_مقدار_بیتها_ANY,
            درون_بایتها + جابه_جایی, trie_اندازه, اشاره_تهی, &error_کد);
    }
    جابه_جایی = بالاترین;
    بالاترین = درون_Indexes[ULAYOUT_IX_VO_TRIE_بالاترین];
    trie_اندازه = بالاترین - جابه_جایی;
    إذا (trie_اندازه >= 16) {
        gVoTrie = ucptrie_open_From_دودویی(
            UCPTRIE_TYPE_ANY, UCPTRIE_مقدار_بیتها_ANY,
            درون_بایتها + جابه_جایی, trie_اندازه, اشاره_تهی, &error_کد);
    }

    uint32_t max_مقادیر = درون_Indexes[ULAYOUT_IX_MAX_مقادیر];
    g_Max_Inpc_مقدار = max_مقادیر >> ULAYOUT_MAX_INPC_لغزش;
    g_Max_Insc_مقدار = (max_مقادیر >> ULAYOUT_MAX_INSC_لغزش) & 0xff;
    g_Max_Vo_مقدار = (max_مقادیر >> ULAYOUT_MAX_VO_لغزش) & 0xff;

    ucln_common_ثبات_Cleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensure_داده(UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_بارگیری, error_کد);
    أرجع U_SUCCESS(error_کد);
}

UBool ulayout_ensure_داده() {
    UError_کد error_کد = U_ZERO_ERROR;
    أرجع ulayout_ensure_داده(error_کد);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

بنية دودویی_Property;

تعريفنوع UBool دودویی_Property_شامل_است(ثابت دودویی_Property &prop, UChar32 c, UProperty which);

بنية دودویی_Property {
    int32_t ستونی;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    دودویی_Property_شامل_است *شامل_است;
};

ساكن UBool default_شامل_است(ثابت دودویی_Property &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    أرجع (u_getUnicodeProperties(c, prop.ستونی)&prop.mask)!=0;
}

ساكن UBool case_دودویی_Property_شامل_است(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty which) {
    أرجع ucase_has_دودویی_Property(c, which);
}

ساكن UBool isBidiControl(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_isBidiControl(c);
}

ساكن UBool isMirrored(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_isMirrored(c);
}

ساكن UBool isJoinControl(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_isJoinControl(c);
}

#إذا UCONFIG_NO_NORMALIZATION
ساكن UBool has_پر_Composition_Exclusion(ثابت دودویی_Property &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool has_پر_Composition_Exclusion(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UError_کد error_کد=U_ZERO_ERROR;
    ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_کد);
    أرجع U_SUCCESS(error_کد) && impl->isCompNo(impl->getNorm16(c));
}
#پایان_اگر

// UCHAR_NF*_INERT properties
#إذا UCONFIG_NO_NORMALIZATION
ساكن UBool isNormInert(ثابت دودویی_Property &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool isNormInert(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty which) {
    UError_کد error_کد=U_ZERO_ERROR;
    ثابت Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalization_نما>(which - UCHAR_NFD_INERT + UNORM_NFD), error_کد);
    أرجع U_SUCCESS(error_کد) && norm2->isInert(c);
}
#پایان_اگر

#إذا UCONFIG_NO_NORMALIZATION
ساكن UBool changesWhenCasefolded(ثابت دودویی_Property &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool changesWhenCasefolded(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UError_کد error_کد=U_ZERO_ERROR;
    ثابت Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(error_کد);
    إذا(U_FAILURE(error_کد)) {
        أرجع خطأ;
    }
    إذا(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        إذا(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } وإلا إذا(nfd.length()<=U16_MAX_درازی &&
                  nfd.length()==U16_درازی(c=nfd.char32At(0))
        ) {
            /* single supplementary code point */
        } وإلا {
            c=U_SENTINEL;
        }
    } وإلا إذا(c<0) {
        أرجع خطأ;  /* protect against bad input */
    }
    إذا(c>=0) {
        /* single code point */
        ثابت char16_t *نتیجه_String;
        أرجع ucase_مقصد_پر_Folding(c, &نتیجه_String, U_FOLD_CASE_DEFAULT) >= 0;
    } وإلا {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_درازی];
        int32_t dest_درازی;
        dest_درازی=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.get_میانگیر(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &error_کد);
        أرجع U_SUCCESS(error_کد) &&
                       0!=u_strCompare(nfd.get_میانگیر(), nfd.length(),
                                       dest, dest_درازی, خطأ);
    }
}
#پایان_اگر

#إذا UCONFIG_NO_NORMALIZATION
ساكن UBool changesWhenNFKC_Casefolded(ثابت دودویی_Property &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool changesWhenNFKC_Casefolded(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_کد error_کد=U_ZERO_ERROR;
    ثابت Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(error_کد);
    إذا(U_FAILURE(error_کد)) {
        أرجع خطأ;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        Reordering_میانگیر میانگیر(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        إذا(میانگیر.init(5, error_کد)) {
            ثابت char16_t *srcArray=src.get_میانگیر();
            kcf->compose(srcArray, srcArray+src.length(), خطأ,
                          درست, میانگیر, error_کد);
        }
    }
    أرجع U_SUCCESS(error_کد) && dest!=src;
}
#پایان_اگر

#إذا UCONFIG_NO_NORMALIZATION
ساكن UBool isCanonSegmentStarter(ثابت دودویی_Property &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool isCanonSegmentStarter(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_کد error_کد=U_ZERO_ERROR;
    ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_کد);
    أرجع
        U_SUCCESS(error_کد) && impl->ensure_Canon_Iter_داده(error_کد) &&
        impl->isCanonSegmentStarter(c);
}
#پایان_اگر

ساكن UBool isPOSIX_alnum(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isalnumPOSIX(c);
}

ساكن UBool isPOSIX_blank(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isblank(c);
}

ساكن UBool is_POSIX_گراف(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isgraphPOSIX(c);
}

ساكن UBool isPOSIX_print(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isprintPOSIX(c);
}

ساكن UBool isPOSIX_xdigit(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isxdigit(c);
}

ساكن UBool isRegionalIndicator(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    أرجع 0x1F1E6<=c && c<=0x1F1FF;
}

ساكن UBool hasEmojiProperty(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty which) {
    أرجع EmojiProps::has_دودویی_Property(c, which);
}

ساكن UBool isIDSUnaryOperator(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // New in Unicode 15.1 for just two characters.
    أرجع 0x2FFE<=c && c<=0x2FFF;
}

/** Ranges (start/limit pairs) of ID_Compat_Math_Continue (only), from UCD PropList.txt. */
ساكن constexpr UChar32 ID_COMPAT_MATH_CONTINUE[] = {
    0x00B2, 0x00B3 + 1,
    0x00B9, 0x00B9 + 1,
    0x2070, 0x2070 + 1,
    0x2074, 0x207E + 1,
    0x2080, 0x208E + 1
};

/** ID_Compat_Math_Start characters, from UCD PropList.txt. */
ساكن constexpr UChar32 ID_COMPAT_MATH_شروع[] = {
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
ساكن constexpr UChar32 MODIFIER_COMBINING_MARK[] = {
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

ساكن UBool is_IDCompat_Math_شروع(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    إذا (c < ID_COMPAT_MATH_شروع[0]) { أرجع خطأ; }  // fastpath for common scripts
    لكل (UChar32 شروع_Char : ID_COMPAT_MATH_شروع) {
        إذا (c == شروع_Char) { أرجع درست; }
    }
    أرجع خطأ;
}

ساكن UBool isIDCompatMathContinue(ثابت دودویی_Property &prop, UChar32 c, UProperty /*which*/) {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        إذا (c < ID_COMPAT_MATH_CONTINUE[i]) { أرجع خطأ; }  // below range start
        إذا (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { أرجع درست; }  // below range limit
    }
    أرجع is_IDCompat_Math_شروع(prop, c, UCHAR_ID_COMPAT_MATH_شروع);
}

ساكن UBool isModifierCombiningMark(ثابت دودویی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        إذا (c < MODIFIER_COMBINING_MARK[i]) { أرجع خطأ; }  // below range start
        إذا (c < MODIFIER_COMBINING_MARK[i + 1]) { أرجع درست; }  // below range limit
    }
    أرجع خطأ;
}

ساكن ثابت دودویی_Property binProps[UCHAR_دودویی_حد]={
    /*
     * column and mask values for binary properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per binary UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { 1,                U_ماسک(UPROPS_ALPHABETIC), default_شامل_است },
    { 1,                U_ماسک(UPROPS_ASCII_HEX_DIGIT), default_شامل_است },
    { UPROPS_SRC_BIDI,  0, isBidiControl },
    { UPROPS_SRC_BIDI,  0, isMirrored },
    { 1,                U_ماسک(UPROPS_DASH), default_شامل_است },
    { 1,                U_ماسک(UPROPS_DEFAULT_IGNORABLE_کد_نقطه), default_شامل_است },
    { 1,                U_ماسک(UPROPS_DEPRECATED), default_شامل_است },
    { 1,                U_ماسک(UPROPS_DIACRITIC), default_شامل_است },
    { 1,                U_ماسک(UPROPS_EXTENDER), default_شامل_است },
    { UPROPS_SRC_NFC,   0, has_پر_Composition_Exclusion },
    { 1,                U_ماسک(UPROPS_GRAPHEME_پایه), default_شامل_است },
    { 1,                U_ماسک(UPROPS_GRAPHEME_EXTEND), default_شامل_است },
    { 1,                U_ماسک(UPROPS_GRAPHEME_LINK), default_شامل_است },
    { 1,                U_ماسک(UPROPS_HEX_DIGIT), default_شامل_است },
    { 1,                U_ماسک(UPROPS_HYPHEN), default_شامل_است },
    { 1,                U_ماسک(UPROPS_ID_CONTINUE), default_شامل_است },
    { 1,                U_ماسک(UPROPS_ID_شروع), default_شامل_است },
    { 1,                U_ماسک(UPROPS_IDEOGRAPHIC), default_شامل_است },
    { 1,                U_ماسک(UPROPS_IDS_دودویی_OPERATOR), default_شامل_است },
    { 1,                U_ماسک(UPROPS_IDS_TRINARY_OPERATOR), default_شامل_است },
    { UPROPS_SRC_BIDI,  0, isJoinControl },
    { 1,                U_ماسک(UPROPS_LOGICAL_ترتیب_EXCEPTION), default_شامل_است },
    { UPROPS_SRC_CASE,  0, case_دودویی_Property_شامل_است },  // UCHAR_LOWERCASE
    { 1,                U_ماسک(UPROPS_MATH), default_شامل_است },
    { 1,                U_ماسک(UPROPS_NONCHARACTER_کد_نقطه), default_شامل_است },
    { 1,                U_ماسک(UPROPS_QUOTATION_MARK), default_شامل_است },
    { 1,                U_ماسک(UPROPS_RADICAL), default_شامل_است },
    { UPROPS_SRC_CASE,  0, case_دودویی_Property_شامل_است },  // UCHAR_SOFT_DOTTED
    { 1,                U_ماسک(UPROPS_TERMINAL_PUNCTUATION), default_شامل_است },
    { 1,                U_ماسک(UPROPS_UNIFIED_IDEOGRAPH), default_شامل_است },
    { UPROPS_SRC_CASE,  0, case_دودویی_Property_شامل_است },  // UCHAR_UPPERCASE
    { 1,                U_ماسک(UPROPS_WHITE_SPACE), default_شامل_است },
    { 1,                U_ماسک(UPROPS_XID_CONTINUE), default_شامل_است },
    { 1,                U_ماسک(UPROPS_XID_شروع), default_شامل_است },
    { UPROPS_SRC_CASE,  0, case_دودویی_Property_شامل_است },  // UCHAR_CASE_SENSITIVE
    { 1,                U_ماسک(UPROPS_S_جمله), default_شامل_است },
    { 1,                U_ماسک(UPROPS_VARIATION_SELECTOR), default_شامل_است },
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFD_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKD_INERT
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFC_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKC_INERT
    { UPROPS_SRC_NFC_CANON_ITER, 0, isCanonSegmentStarter },
    { 1,                U_ماسک(UPROPS_الگو_SYNTAX), default_شامل_است },
    { 1,                U_ماسک(UPROPS_الگو_WHITE_SPACE), default_شامل_است },
    { UPROPS_SRC_CHAR_AND_PROPSVEC,  0, isPOSIX_alnum },
    { UPROPS_SRC_CHAR,  0, isPOSIX_blank },
    { UPROPS_SRC_CHAR,  0, is_POSIX_گراف },
    { UPROPS_SRC_CHAR,  0, isPOSIX_print },
    { UPROPS_SRC_CHAR,  0, isPOSIX_xdigit },
    { UPROPS_SRC_CASE,  0, case_دودویی_Property_شامل_است },  // UCHAR_CASED
    { UPROPS_SRC_CASE,  0, case_دودویی_Property_شامل_است },  // UCHAR_CASE_IGNORABLE
    { UPROPS_SRC_CASE,  0, case_دودویی_Property_شامل_است },  // UCHAR_CHANGES_WHEN_LOWERCASED
    { UPROPS_SRC_CASE,  0, case_دودویی_Property_شامل_است },  // UCHAR_CHANGES_WHEN_UPPERCASED
    { UPROPS_SRC_CASE,  0, case_دودویی_Property_شامل_است },  // UCHAR_CHANGES_WHEN_TITLECASED
    { UPROPS_SRC_CASE_AND_NORM,  0, changesWhenCasefolded },
    { UPROPS_SRC_CASE,  0, case_دودویی_Property_شامل_است },  // UCHAR_CHANGES_WHEN_CASEMAPPED
    { UPROPS_SRC_NFKC_CF, 0, changesWhenNFKC_Casefolded },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_PRESENTATION
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER_BASE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_COMPONENT
    { 2,                0, isRegionalIndicator },
    { 1,                U_ماسک(UPROPS_PREPENDED_CONCATENATION_MARK), default_شامل_است },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EXTENDED_PICTOGRAPHIC
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_BASIC_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_KEYCAP_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_MODIFIER_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_FLAG_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_TAG_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_ZWJ_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI
    { UPROPS_SRC_IDSU, 0, isIDSUnaryOperator }, // UCHAR_IDS_UNARY_OPERATOR
    { UPROPS_SRC_ID_COMPAT_MATH, 0, is_IDCompat_Math_شروع }, // UCHAR_ID_COMPAT_MATH_START
    { UPROPS_SRC_ID_COMPAT_MATH, 0, isIDCompatMathContinue }, // UCHAR_ID_COMPAT_MATH_CONTINUE
    { UPROPS_SRC_MCM, 0 , isModifierCombiningMark }, // UCHAR_MODIFIER_COMBINING_MARK
};

U_CAPI UBool U_EXPORT2
u_has_دودویی_Property(UChar32 c, UProperty which) {
    /* c is range-checked in the functions that are called from here */
    إذا(which<UCHAR_دودویی_شروع || UCHAR_دودویی_حد<=which) {
        /* not a known binary property */
        أرجع خطأ;
    } وإلا {
        ثابت دودویی_Property &prop=binProps[which];
        أرجع prop.شامل_است(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    أرجع u_has_دودویی_Property(c, UCHAR_ID_شروع);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    أرجع u_has_دودویی_Property(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_string_Has_دودویی_Property(ثابت char16_t *s, int32_t length, UProperty which) {
    إذا (s == اشاره_تهی && length != 0) { أرجع خطأ; }
    إذا (length == 1) {
        أرجع u_has_دودویی_Property(s[0], which);  // single code point
    } وإلا إذا (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_بعدی(s, i, length, c);
        إذا (length > 0 ? i == length : s[i] == 0) {
            أرجع u_has_دودویی_Property(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    أرجع UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::has_دودویی_Property(s, length, which);
}

بنية IntProperty;

تعريفنوع int32_t Int_Property_Get_مقدار(ثابت IntProperty &prop, UChar32 c, UProperty which);
تعريفنوع int32_t Int_Property_Get_Max_مقدار(ثابت IntProperty &prop, UProperty which);

بنية IntProperty {
    int32_t ستونی;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t لغزش;  // =maxValue if getMaxValueFromShift() is used
    Int_Property_Get_مقدار *get_مقدار;
    Int_Property_Get_Max_مقدار *get_Max_مقدار;
};

ساكن int32_t default_Get_مقدار(ثابت IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    أرجع static_cast<int32_t>(u_getUnicodeProperties(c, prop.ستونی) & prop.mask) >> prop.لغزش;
}

ساكن int32_t default_Get_Max_مقدار(ثابت IntProperty &prop, UProperty /*which*/) {
    أرجع (uprv_get_Max_مقادیر(prop.ستونی)&prop.mask)>>prop.لغزش;
}

ساكن int32_t get_Max_مقدار_From_لغزش(ثابت IntProperty &prop, UProperty /*which*/) {
    أرجع prop.لغزش;
}

ساكن int32_t getBiDiClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع static_cast<int32_t>(u_charDirection(c));
}

ساكن int32_t getBiDiPairedBracketType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

ساكن int32_t bi_Di_Get_Max_مقدار(ثابت IntProperty &/*prop*/, UProperty which) {
    أرجع ubidi_get_Max_مقدار(which);
}

ساكن int32_t get_بلوک(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع static_cast<int32_t>(ublock_get_کد(c));
}

ساكن int32_t بلوک_Get_Max_مقدار(ثابت IntProperty &/*prop*/, UProperty /*which*/) {
    أرجع uprv_get_Max_مقادیر(UPROPS_MAX_مقادیر_OTHER_شاخص) & UPROPS_MAX_بلوک;
}

#إذا UCONFIG_NO_NORMALIZATION
ساكن int32_t getCombiningClass(ثابت IntProperty &, UChar32, UProperty) {
    أرجع 0;
}
#وإلا
ساكن int32_t getCombiningClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_getCombiningClass(c);
}
#پایان_اگر

ساكن int32_t getGeneralCategory(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع static_cast<int32_t>(u_charType(c));
}

ساكن int32_t get_Joining_گروه(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_get_Joining_گروه(c);
}

ساكن int32_t getJoiningType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_getJoiningType(c);
}

ساكن int32_t getNumericType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_مقدار(u_getMainProperties(c)));
    أرجع UPROPS_NTV_GET_TYPE(ntv);
}

ساكن int32_t getScript(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_کد error_کد=U_ZERO_ERROR;
    أرجع static_cast<int32_t>(uscript_getScript(c, &error_کد));
}

ساكن int32_t script_Get_Max_مقدار(ثابت IntProperty &/*prop*/, UProperty /*which*/) {
    أرجع uprv_get_Max_مقادیر(0)&UPROPS_MAX_SCRIPT;
}

/*
 * Map some of the Grapheme Cluster Break values to Hangul Syllable Types.
 * Hangul_Syllable_Type used to be fully redundant with a subset of Grapheme_Cluster_Break.
 *
 * Starting with Unicode 16, this is no longer true for HST=V vs. GCB=V in some cases:
 * Some Kirat Rai vowels are given GCB=V for proper grapheme clustering, but
 * they are of course not related to Hangul syllables.
 */
ساكن ثابت UHangulSyllableType gcb_مقصد_Hst[]={
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

ساكن int32_t getHangulSyllableType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Ignore supplementary code points: They all have HST=NA.
    // This is a simple way to handle the GCB!=hst cases since Unicode 16 (Kirat Rai vowels).
    إذا(c>0xffff) {
        أرجع U_HST_NOT_APPLICABLE;
    }
    /* see comments on gcbToHst[] above */
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_ماسک) >> UPROPS_GCB_لغزش;
    إذا(gcb<UPRV_LENGTHOF(gcb_مقصد_Hst)) {
        أرجع gcb_مقصد_Hst[gcb];
    } وإلا {
        أرجع U_HST_NOT_APPLICABLE;
    }
}

#إذا UCONFIG_NO_NORMALIZATION
ساكن int32_t get_Norm_سریع_Check(ثابت IntProperty &, UChar32, UProperty) {
    أرجع 0;
}
#وإلا
ساكن int32_t get_Norm_سریع_Check(ثابت IntProperty &/*prop*/, UChar32 c, UProperty which) {
    أرجع static_cast<int32_t>(unorm_get_سریع_Check(c, static_cast<UNormalization_نما>(which - UCHAR_NFD_سریع_CHECK + UNORM_NFD)));
}
#پایان_اگر

#إذا UCONFIG_NO_NORMALIZATION
ساكن int32_t getLeadCombiningClass(ثابت IntProperty &, UChar32, UProperty) {
    أرجع 0;
}
#وإلا
ساكن int32_t getLeadCombiningClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع unorm_getFCD16(c)>>8;
}
#پایان_اگر

#إذا UCONFIG_NO_NORMALIZATION
ساكن int32_t getTrailCombiningClass(ثابت IntProperty &, UChar32, UProperty) {
    أرجع 0;
}
#وإلا
ساكن int32_t getTrailCombiningClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع unorm_getFCD16(c)&0xff;
}
#پایان_اگر

ساكن int32_t get_درون_PC(ثابت IntProperty &, UChar32 c, UProperty) {
    أرجع ulayout_ensure_داده() && gInpcTrie != اشاره_تهی ? ucptrie_get(gInpcTrie, c) : 0;
}

ساكن int32_t get_درون_SC(ثابت IntProperty &, UChar32 c, UProperty) {
    أرجع ulayout_ensure_داده() && gInscTrie != اشاره_تهی ? ucptrie_get(gInscTrie, c) : 0;
}

ساكن int32_t getVo(ثابت IntProperty &, UChar32 c, UProperty) {
    أرجع ulayout_ensure_داده() && gVoTrie != اشاره_تهی ? ucptrie_get(gVoTrie, c) : 0;
}

ساكن int32_t layout_Get_Max_مقدار(ثابت IntProperty &/*prop*/, UProperty which) {
    إذا (!ulayout_ensure_داده()) { أرجع 0; }
    بدل (which) {
    حالة UCHAR_INDIC_POSITIONAL_CATEGORY:
        أرجع g_Max_Inpc_مقدار;
    حالة UCHAR_INDIC_SYLLABIC_CATEGORY:
        أرجع g_Max_Insc_مقدار;
    حالة UCHAR_VERTICAL_ORIENTATION:
        أرجع g_Max_Vo_مقدار;
    افتراضي:
        أرجع 0;
    }
}

ساكن int32_t get_IDStatus_مقدار(ثابت IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_لغزش;
    أرجع value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_وضعیت_ALLOWED : U_ID_وضعیت_RESTRICTED;
}

ساكن ثابت IntProperty intProps[UCHAR_INT_حد-UCHAR_INT_شروع]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, bi_Di_Get_Max_مقدار },
    { UPROPS_SRC_بلوک, 0, 0,                               get_بلوک, بلوک_Get_Max_مقدار },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, get_Max_مقدار_From_لغزش },
    { 2,                UPROPS_DT_ماسک, 0,                  default_Get_مقدار, default_Get_Max_مقدار },
    { 0,                UPROPS_EA_ماسک, UPROPS_EA_لغزش,    default_Get_مقدار, default_Get_Max_مقدار },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_شمار) - 1, getGeneralCategory, get_Max_مقدار_From_لغزش },
    { UPROPS_SRC_BIDI,  0, 0,                               get_Joining_گروه, bi_Di_Get_Max_مقدار },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, bi_Di_Get_Max_مقدار },
    { 2,                UPROPS_LB_ماسک, UPROPS_LB_لغزش,    default_Get_مقدار, default_Get_Max_مقدار },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_شمار) - 1, getNumericType, get_Max_مقدار_From_لغزش },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, script_Get_Max_مقدار },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_شمار) - 1, getHangulSyllableType, get_Max_مقدار_From_لغزش },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), get_Norm_سریع_Check, get_Max_مقدار_From_لغزش },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), get_Norm_سریع_Check, get_Max_مقدار_From_لغزش },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), get_Norm_سریع_Check, get_Max_مقدار_From_لغزش },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), get_Norm_سریع_Check, get_Max_مقدار_From_لغزش },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, get_Max_مقدار_From_لغزش },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, get_Max_مقدار_From_لغزش },
    { 2,                UPROPS_GCB_ماسک, UPROPS_GCB_لغزش,  default_Get_مقدار, default_Get_Max_مقدار },
    { 2,                UPROPS_SB_ماسک, UPROPS_SB_لغزش,    default_Get_مقدار, default_Get_Max_مقدار },
    { 2,                UPROPS_WB_ماسک, UPROPS_WB_لغزش,    default_Get_مقدار, default_Get_Max_مقدار },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, bi_Di_Get_Max_مقدار },
    { UPROPS_SRC_INPC,  0, 0,                               get_درون_PC, layout_Get_Max_مقدار },
    { UPROPS_SRC_INSC,  0, 0,                               get_درون_SC, layout_Get_Max_مقدار },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layout_Get_Max_مقدار },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_وضعیت_ALLOWED), get_IDStatus_مقدار, get_Max_مقدار_From_لغزش },
    { 0,                UPROPS_INCB_ماسک, UPROPS_INCB_لغزش,default_Get_مقدار, default_Get_Max_مقدار },
};

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_مقدار(UChar32 c, UProperty which) {
    إذا(which<UCHAR_INT_شروع) {
        إذا(UCHAR_دودویی_شروع<=which && which<UCHAR_دودویی_حد) {
            ثابت دودویی_Property &prop=binProps[which];
            أرجع prop.شامل_است(prop, c, which);
        }
    } وإلا إذا(which<UCHAR_INT_حد) {
        ثابت IntProperty &prop=intProps[which-UCHAR_INT_شروع];
        أرجع prop.get_مقدار(prop, c, which);
    } وإلا إذا(which==UCHAR_GENERAL_CATEGORY_ماسک) {
        أرجع U_ماسک(u_charType(c));
    }
    أرجع 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Min_مقدار(UProperty /*which*/) {
    أرجع 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Max_مقدار(UProperty which) {
    إذا(which<UCHAR_INT_شروع) {
        إذا(UCHAR_دودویی_شروع<=which && which<UCHAR_دودویی_حد) {
            أرجع 1;  // maximum true for all binary properties
        }
    } وإلا إذا(which<UCHAR_INT_حد) {
        ثابت IntProperty &prop=intProps[which-UCHAR_INT_شروع];
        أرجع prop.get_Max_مقدار(prop, which);
    }
    أرجع -1;  // undefined
}

U_CFUNC UProperty_منبع U_EXPORT2
uprops_get_منبع(UProperty which) {
    إذا(which<UCHAR_دودویی_شروع) {
        أرجع UPROPS_SRC_NONE; /* undefined */
    } وإلا إذا(which<UCHAR_دودویی_حد) {
        ثابت دودویی_Property &prop=binProps[which];
        إذا(prop.mask!=0) {
            أرجع UPROPS_SRC_PROPSVEC;
        } وإلا {
            أرجع (UProperty_منبع)prop.ستونی;
        }
    } وإلا إذا(which<UCHAR_INT_شروع) {
        أرجع UPROPS_SRC_NONE; /* undefined */
    } وإلا إذا(which<UCHAR_INT_حد) {
        ثابت IntProperty &prop=intProps[which-UCHAR_INT_شروع];
        إذا(prop.mask!=0) {
            أرجع UPROPS_SRC_PROPSVEC;
        } وإلا {
            أرجع (UProperty_منبع)prop.ستونی;
        }
    } وإلا إذا(which<UCHAR_STRING_شروع) {
        بدل(which) {
        حالة UCHAR_GENERAL_CATEGORY_ماسک:
        حالة UCHAR_NUMERIC_مقدار:
            أرجع UPROPS_SRC_CHAR;

        افتراضي:
            أرجع UPROPS_SRC_NONE;
        }
    } وإلا إذا(which<UCHAR_STRING_حد) {
        بدل(which) {
        حالة UCHAR_AGE:
            أرجع UPROPS_SRC_PROPSVEC;

        حالة UCHAR_BIDI_MIRRORING_GLYPH:
            أرجع UPROPS_SRC_BIDI;

        حالة UCHAR_CASE_FOLDING:
        حالة UCHAR_LOWERCASE_MAPPING:
        حالة UCHAR_SIMPLE_CASE_FOLDING:
        حالة UCHAR_SIMPLE_LOWERCASE_MAPPING:
        حالة UCHAR_SIMPLE_TITLECASE_MAPPING:
        حالة UCHAR_SIMPLE_UPPERCASE_MAPPING:
        حالة UCHAR_TITLECASE_MAPPING:
        حالة UCHAR_UPPERCASE_MAPPING:
            أرجع UPROPS_SRC_CASE;

        حالة UCHAR_ISO_COMMENT:
        حالة UCHAR_NAME:
        حالة UCHAR_UNICODE_1_NAME:
            أرجع UPROPS_SRC_نامها;

        افتراضي:
            أرجع UPROPS_SRC_NONE;
        }
    } وإلا {
        بدل(which) {
        حالة UCHAR_SCRIPT_EXTENSIONS:
        حالة UCHAR_IDENTIFIER_TYPE:
            أرجع UPROPS_SRC_PROPSVEC;
        افتراضي:
            أرجع UPROPS_SRC_NONE; /* undefined */
        }
    }
}

U_CFUNC فراغ U_EXPORT2
uprops_جمع_کن_Property_Starts(UProperty_منبع src, ثابت USetAdder *sa, UError_کد *p_Error_کد) {
    إذا (U_FAILURE(*p_Error_کد)) { أرجع; }
    إذا (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        لكل (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->جمع_کن(sa->مجموعه, c);
        }
        // single characters
        لكل (UChar32 c : ID_COMPAT_MATH_شروع) {
            sa->جمع_کن(sa->مجموعه, c);
            sa->جمع_کن(sa->مجموعه, c + 1);
        }
        أرجع;
    }
    إذا (src == UPROPS_SRC_MCM) {
        // range limits
        لكل (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->جمع_کن(sa->مجموعه, c);
        }
        أرجع;
    }
    إذا (!ulayout_ensure_داده(*p_Error_کد)) { أرجع; }
    ثابت UCPTrie *trie;
    بدل (src) {
    حالة UPROPS_SRC_INPC:
        trie = gInpcTrie;
        اكسر;
    حالة UPROPS_SRC_INSC:
        trie = gInscTrie;
        اكسر;
    حالة UPROPS_SRC_VO:
        trie = gVoTrie;
        اكسر;
    افتراضي:
        *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    إذا (trie == اشاره_تهی) {
        *p_Error_کد = U_MISSING_RESOURCE_ERROR;
        أرجع;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 شروع = 0, انتها;
    طالما ((انتها = ucptrie_getRange(trie, شروع, UCPMAP_RANGE_نرمال, 0,
                                   اشاره_تهی, اشاره_تهی, اشاره_تهی)) >= 0) {
        sa->جمع_کن(sa->مجموعه, شروع);
        شروع = انتها + 1;
    }
}

U_CAPI منطقي U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t type_شاخص = type;  // also guards against negative type integers
    إذا (type_شاخص >= UPRV_LENGTHOF(uprops_id_Type_مقصد_Encoded)) {
        أرجع خطأ;
    }
    uint32_t encodedType = uprops_id_Type_مقصد_Encoded[type_شاخص];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_لغزش;
    إذا ((encodedType & UPROPS_ID_TYPE_بیت) != 0) {
        أرجع value < UPROPS_ID_TYPE_FORBIDDEN && (value & encodedType) != 0;
    } وإلا {
        أرجع value == encodedType;
    }
}

نطاق {

فراغ maybeAppendType(uint32_t value, uint32_t bit, UIdentifierType t,
                     UIdentifierType *types, int32_t &length, int32_t capacity) {
    إذا ((value & bit) != 0) {
        إذا (length < capacity) {
            types[length] = t;
        }
        ++length;
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UError_کد *p_Error_کد) {
    إذا (U_FAILURE(*p_Error_کد)) { أرجع 0; }
    إذا (capacity < 0 || (capacity > 0 && types == اشاره_تهی)) {
        *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_لغزش;
    إذا ((value & UPROPS_ID_TYPE_FORBIDDEN) == UPROPS_ID_TYPE_FORBIDDEN ||
            value == UPROPS_ID_TYPE_NOT_CHARACTER) {
        // single value
        إذا (capacity > 0) {
            UIdentifierType t;
            بدل (value) {
                حالة UPROPS_ID_TYPE_NOT_CHARACTER: t = U_ID_TYPE_NOT_CHARACTER; اكسر;
                حالة UPROPS_ID_TYPE_DEPRECATED: t = U_ID_TYPE_DEPRECATED; اكسر;
                حالة UPROPS_ID_TYPE_DEFAULT_IGNORABLE: t = U_ID_TYPE_DEFAULT_IGNORABLE; اكسر;
                حالة UPROPS_ID_TYPE_NOT_NFKC: t = U_ID_TYPE_NOT_NFKC; اكسر;
                حالة UPROPS_ID_TYPE_INCLUSION: t = U_ID_TYPE_INCLUSION; اكسر;
                حالة UPROPS_ID_TYPE_RECOMMENDED: t = U_ID_TYPE_RECOMMENDED; اكسر;
                افتراضي:
                    *p_Error_کد = U_INVALID_FORMAT_ERROR;
                    أرجع 0;
            }
            types[0] = t;
        } وإلا {
            *p_Error_کد = U_میانگیر_سرریز_ERROR;
        }
        أرجع 1;
    } وإلا {
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
        إذا (length >= capacity) {
            *p_Error_کد = U_میانگیر_سرریز_ERROR;
        }
        أرجع length;
    }
}

#إذا !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_get_FC_NFKC_بستگی(UChar32 c, char16_t *dest, int32_t dest_ظرفیت, UError_کد *p_Error_کد) {
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(dest_ظرفیت<0 || (dest==اشاره_تهی && dest_ظرفیت>0)) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    ثابت Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*p_Error_کد);
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    ثابت char16_t *folded1;
    int32_t folded1_درازی=ucase_مقصد_پر_Folding(c, &folded1, U_FOLD_CASE_DEFAULT);
    إذا(folded1_درازی<0) {
        ثابت Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        إذا(nfkcImpl->get_Comp_سریع_Check(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            أرجع u_terminateUChars(dest, dest_ظرفیت, 0, p_Error_کد);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.مجموعه_مقصد(c);
    } وإلا {
        إذا(folded1_درازی>UCASE_MAX_STRING_درازی) {
            folded1String.مجموعه_مقصد(folded1_درازی);
        } وإلا {
            folded1String.مجموعه_مقصد(خطأ, folded1, folded1_درازی);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *p_Error_کد);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *p_Error_کد);
    // if (c != b) add the mapping from a to c
    إذا(U_FAILURE(*p_Error_کد) || kc1==kc2) {
        أرجع u_terminateUChars(dest, dest_ظرفیت, 0, p_Error_کد);
    } وإلا {
        أرجع kc2.extract(dest, dest_ظرفیت, *p_Error_کد);
    }
}

#پایان_اگر
