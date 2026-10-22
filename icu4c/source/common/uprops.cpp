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
UData_حافظه *g_Layout_حافظه = تش_اشاره;

UCPTrie *gInpcTrie = تش_اشاره;  // Indic_Positional_Category
UCPTrie *gInscTrie = تش_اشاره;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = تش_اشاره;  // Vertical_Orientation

int32_t g_Max_Inpc_ارزښت = 0;
int32_t g_Max_Insc_ارزښت = 0;
int32_t g_Max_Vo_ارزښت = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(g_Layout_حافظه);
    g_Layout_حافظه = تش_اشاره;

    ucptrie_close(gInpcTrie);
    gInpcTrie = تش_اشاره;
    ucptrie_close(gInscTrie);
    gInscTrie = تش_اشاره;
    ucptrie_close(gVoTrie);
    gVoTrie = تش_اشاره;

    g_Max_Inpc_ارزښت = 0;
    g_Max_Insc_ارزښت = 0;
    g_Max_Vo_ارزښت = 0;

    gLayoutInitOnce.reset();
    أرجع ریښتیا;
}

UBool U_CALLCONV
ulayout_isAcceptable(فراغ * /*context*/,
                     ثابت حرف * /* type */, ثابت حرف * /*name*/,
                     ثابت UDataInfo *pInfo) {
    أرجع pInfo->size >= 20 &&
        pInfo->isBigEndian == U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily == U_CHARSET_FAMILY &&
        pInfo->معلومات_Format[0] == ULAYOUT_FMT_0 &&
        pInfo->معلومات_Format[1] == ULAYOUT_FMT_1 &&
        pInfo->معلومات_Format[2] == ULAYOUT_FMT_2 &&
        pInfo->معلومات_Format[3] == ULAYOUT_FMT_3 &&
        pInfo->format_نسخه[0] == 1;
}

// UInitOnce singleton initialization function
فراغ U_CALLCONV ulayout_بارول(UError_کوډ &error_کوډ) {
    g_Layout_حافظه = udata_openChoice(
        تش_اشاره, ULAYOUT_معلومات_TYPE, ULAYOUT_معلومات_NAME,
        ulayout_isAcceptable, تش_اشاره, &error_کوډ);
    إذا (U_FAILURE(error_کوډ)) { أرجع; }

    ثابت uint8_t* دننه_بایټونه = static_cast<ثابت uint8_t*>(udata_get_حافظه(g_Layout_حافظه));
    ثابت int32_t* دننه_Indexes = reinterpret_cast<ثابت int32_t*>(دننه_بایټونه);
    int32_t indexes_اوږدتیا = دننه_Indexes[ULAYOUT_IX_INDEXES_اوږدتیا];
    إذا (indexes_اوږدتیا < 12) {
        error_کوډ = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        أرجع;
    }
    int32_t دننه_واټن = indexes_اوږدتیا * 4;
    int32_t سر_2 = دننه_Indexes[ULAYOUT_IX_INPC_TRIE_سر];
    int32_t trie_اندازه = سر_2 - دننه_واټن;
    إذا (trie_اندازه >= 16) {
        gInpcTrie = ucptrie_open_From_دوګونی(
            UCPTRIE_TYPE_ANY, UCPTRIE_ارزښت_بټونه_ANY,
            دننه_بایټونه + دننه_واټن, trie_اندازه, تش_اشاره, &error_کوډ);
    }
    دننه_واټن = سر_2;
    سر_2 = دننه_Indexes[ULAYOUT_IX_INSC_TRIE_سر];
    trie_اندازه = سر_2 - دننه_واټن;
    إذا (trie_اندازه >= 16) {
        gInscTrie = ucptrie_open_From_دوګونی(
            UCPTRIE_TYPE_ANY, UCPTRIE_ارزښت_بټونه_ANY,
            دننه_بایټونه + دننه_واټن, trie_اندازه, تش_اشاره, &error_کوډ);
    }
    دننه_واټن = سر_2;
    سر_2 = دننه_Indexes[ULAYOUT_IX_VO_TRIE_سر];
    trie_اندازه = سر_2 - دننه_واټن;
    إذا (trie_اندازه >= 16) {
        gVoTrie = ucptrie_open_From_دوګونی(
            UCPTRIE_TYPE_ANY, UCPTRIE_ارزښت_بټونه_ANY,
            دننه_بایټونه + دننه_واټن, trie_اندازه, تش_اشاره, &error_کوډ);
    }

    uint32_t max_ارزښتونه = دننه_Indexes[ULAYOUT_IX_MAX_ارزښتونه];
    g_Max_Inpc_ارزښت = max_ارزښتونه >> ULAYOUT_MAX_INPC_ښويدنه;
    g_Max_Insc_ارزښت = (max_ارزښتونه >> ULAYOUT_MAX_INSC_ښويدنه) & 0xff;
    g_Max_Vo_ارزښت = (max_ارزښتونه >> ULAYOUT_MAX_VO_ښويدنه) & 0xff;

    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensure_معلومات(UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_بارول, error_کوډ);
    أرجع U_SUCCESS(error_کوډ);
}

UBool ulayout_ensure_معلومات() {
    UError_کوډ error_کوډ = U_ZERO_ERROR;
    أرجع ulayout_ensure_معلومات(error_کوډ);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

بنية دوګونی_Property;

تعريفنوع UBool دوګونی_Property_لري(ثابت دوګونی_Property &prop, UChar32 c, UProperty which);

بنية دوګونی_Property {
    int32_t ستنه;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    دوګونی_Property_لري *لري;
};

ساكن UBool default_لري(ثابت دوګونی_Property &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    أرجع (u_getUnicodeProperties(c, prop.ستنه)&prop.mask)!=0;
}

ساكن UBool case_دوګونی_Property_لري(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty which) {
    أرجع ucase_has_دوګونی_Property(c, which);
}

ساكن UBool isBidiControl(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_isBidiControl(c);
}

ساكن UBool isMirrored(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_isMirrored(c);
}

ساكن UBool isJoinControl(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_isJoinControl(c);
}

#إذا UCONFIG_NO_NORMALIZATION
ساكن UBool has_ډک_Composition_Exclusion(ثابت دوګونی_Property &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool has_ډک_Composition_Exclusion(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_کوډ);
    أرجع U_SUCCESS(error_کوډ) && impl->isCompNo(impl->getNorm16(c));
}
#د_که_پای

// UCHAR_NF*_INERT properties
#إذا UCONFIG_NO_NORMALIZATION
ساكن UBool isNormInert(ثابت دوګونی_Property &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool isNormInert(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty which) {
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    ثابت Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalization_ډېرپېښ>(which - UCHAR_NFD_INERT + UNORM_NFD), error_کوډ);
    أرجع U_SUCCESS(error_کوډ) && norm2->isInert(c);
}
#د_که_پای

#إذا UCONFIG_NO_NORMALIZATION
ساكن UBool changesWhenCasefolded(ثابت دوګونی_Property &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool changesWhenCasefolded(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    ثابت Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(error_کوډ);
    إذا(U_FAILURE(error_کوډ)) {
        أرجع خطأ;
    }
    إذا(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        إذا(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } وإلا إذا(nfd.length()<=U16_MAX_اوږدتیا &&
                  nfd.length()==U16_اوږدتیا(c=nfd.char32At(0))
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
        ثابت char16_t *پایله_String;
        أرجع ucase_منزل_ډک_Folding(c, &پایله_String, U_FOLD_CASE_DEFAULT) >= 0;
    } وإلا {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_اوږدتیا];
        int32_t dest_اوږدتیا;
        dest_اوږدتیا=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.get_بفر(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &error_کوډ);
        أرجع U_SUCCESS(error_کوډ) &&
                       0!=u_strCompare(nfd.get_بفر(), nfd.length(),
                                       dest, dest_اوږدتیا, خطأ);
    }
}
#د_که_پای

#إذا UCONFIG_NO_NORMALIZATION
ساكن UBool changesWhenNFKC_Casefolded(ثابت دوګونی_Property &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool changesWhenNFKC_Casefolded(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    ثابت Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(error_کوډ);
    إذا(U_FAILURE(error_کوډ)) {
        أرجع خطأ;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        Reordering_بفر بفر(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        إذا(بفر.init(5, error_کوډ)) {
            ثابت char16_t *srcArray=src.get_بفر();
            kcf->compose(srcArray, srcArray+src.length(), خطأ,
                          ریښتیا, بفر, error_کوډ);
        }
    }
    أرجع U_SUCCESS(error_کوډ) && dest!=src;
}
#د_که_پای

#إذا UCONFIG_NO_NORMALIZATION
ساكن UBool isCanonSegmentStarter(ثابت دوګونی_Property &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool isCanonSegmentStarter(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_کوډ);
    أرجع
        U_SUCCESS(error_کوډ) && impl->ensure_Canon_Iter_معلومات(error_کوډ) &&
        impl->isCanonSegmentStarter(c);
}
#د_که_پای

ساكن UBool isPOSIX_alnum(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isalnumPOSIX(c);
}

ساكن UBool isPOSIX_blank(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isblank(c);
}

ساكن UBool is_POSIX_ګراف(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isgraphPOSIX(c);
}

ساكن UBool isPOSIX_print(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isprintPOSIX(c);
}

ساكن UBool isPOSIX_xdigit(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isxdigit(c);
}

ساكن UBool isRegionalIndicator(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    أرجع 0x1F1E6<=c && c<=0x1F1FF;
}

ساكن UBool hasEmojiProperty(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty which) {
    أرجع EmojiProps::has_دوګونی_Property(c, which);
}

ساكن UBool isIDSUnaryOperator(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
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
ساكن constexpr UChar32 ID_COMPAT_MATH_پیل[] = {
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

ساكن UBool is_IDCompat_Math_پیل(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    إذا (c < ID_COMPAT_MATH_پیل[0]) { أرجع خطأ; }  // fastpath for common scripts
    لكل (UChar32 پیل_Char : ID_COMPAT_MATH_پیل) {
        إذا (c == پیل_Char) { أرجع ریښتیا; }
    }
    أرجع خطأ;
}

ساكن UBool isIDCompatMathContinue(ثابت دوګونی_Property &prop, UChar32 c, UProperty /*which*/) {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        إذا (c < ID_COMPAT_MATH_CONTINUE[i]) { أرجع خطأ; }  // below range start
        إذا (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { أرجع ریښتیا; }  // below range limit
    }
    أرجع is_IDCompat_Math_پیل(prop, c, UCHAR_ID_COMPAT_MATH_پیل);
}

ساكن UBool isModifierCombiningMark(ثابت دوګونی_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        إذا (c < MODIFIER_COMBINING_MARK[i]) { أرجع خطأ; }  // below range start
        إذا (c < MODIFIER_COMBINING_MARK[i + 1]) { أرجع ریښتیا; }  // below range limit
    }
    أرجع خطأ;
}

ساكن ثابت دوګونی_Property binProps[UCHAR_دوګونی_حد]={
    /*
     * column and mask values for binary properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per binary UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { 1,                U_ماسک(UPROPS_ALPHABETIC), default_لري },
    { 1,                U_ماسک(UPROPS_ASCII_HEX_DIGIT), default_لري },
    { UPROPS_SRC_BIDI,  0, isBidiControl },
    { UPROPS_SRC_BIDI,  0, isMirrored },
    { 1,                U_ماسک(UPROPS_DASH), default_لري },
    { 1,                U_ماسک(UPROPS_DEFAULT_IGNORABLE_کوډ_ټکی), default_لري },
    { 1,                U_ماسک(UPROPS_DEPRECATED), default_لري },
    { 1,                U_ماسک(UPROPS_DIACRITIC), default_لري },
    { 1,                U_ماسک(UPROPS_EXTENDER), default_لري },
    { UPROPS_SRC_NFC,   0, has_ډک_Composition_Exclusion },
    { 1,                U_ماسک(UPROPS_GRAPHEME_بنسټ), default_لري },
    { 1,                U_ماسک(UPROPS_GRAPHEME_EXTEND), default_لري },
    { 1,                U_ماسک(UPROPS_GRAPHEME_LINK), default_لري },
    { 1,                U_ماسک(UPROPS_HEX_DIGIT), default_لري },
    { 1,                U_ماسک(UPROPS_HYPHEN), default_لري },
    { 1,                U_ماسک(UPROPS_ID_CONTINUE), default_لري },
    { 1,                U_ماسک(UPROPS_ID_پیل), default_لري },
    { 1,                U_ماسک(UPROPS_IDEOGRAPHIC), default_لري },
    { 1,                U_ماسک(UPROPS_IDS_دوګونی_OPERATOR), default_لري },
    { 1,                U_ماسک(UPROPS_IDS_TRINARY_OPERATOR), default_لري },
    { UPROPS_SRC_BIDI,  0, isJoinControl },
    { 1,                U_ماسک(UPROPS_LOGICAL_ترتیب_EXCEPTION), default_لري },
    { UPROPS_SRC_CASE,  0, case_دوګونی_Property_لري },  // UCHAR_LOWERCASE
    { 1,                U_ماسک(UPROPS_MATH), default_لري },
    { 1,                U_ماسک(UPROPS_NONCHARACTER_کوډ_ټکی), default_لري },
    { 1,                U_ماسک(UPROPS_QUOTATION_MARK), default_لري },
    { 1,                U_ماسک(UPROPS_RADICAL), default_لري },
    { UPROPS_SRC_CASE,  0, case_دوګونی_Property_لري },  // UCHAR_SOFT_DOTTED
    { 1,                U_ماسک(UPROPS_TERMINAL_PUNCTUATION), default_لري },
    { 1,                U_ماسک(UPROPS_UNIFIED_IDEOGRAPH), default_لري },
    { UPROPS_SRC_CASE,  0, case_دوګونی_Property_لري },  // UCHAR_UPPERCASE
    { 1,                U_ماسک(UPROPS_WHITE_SPACE), default_لري },
    { 1,                U_ماسک(UPROPS_XID_CONTINUE), default_لري },
    { 1,                U_ماسک(UPROPS_XID_پیل), default_لري },
    { UPROPS_SRC_CASE,  0, case_دوګونی_Property_لري },  // UCHAR_CASE_SENSITIVE
    { 1,                U_ماسک(UPROPS_S_غړی), default_لري },
    { 1,                U_ماسک(UPROPS_VARIATION_SELECTOR), default_لري },
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFD_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKD_INERT
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFC_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKC_INERT
    { UPROPS_SRC_NFC_CANON_ITER, 0, isCanonSegmentStarter },
    { 1,                U_ماسک(UPROPS_نمونه_SYNTAX), default_لري },
    { 1,                U_ماسک(UPROPS_نمونه_WHITE_SPACE), default_لري },
    { UPROPS_SRC_CHAR_AND_PROPSVEC,  0, isPOSIX_alnum },
    { UPROPS_SRC_CHAR,  0, isPOSIX_blank },
    { UPROPS_SRC_CHAR,  0, is_POSIX_ګراف },
    { UPROPS_SRC_CHAR,  0, isPOSIX_print },
    { UPROPS_SRC_CHAR,  0, isPOSIX_xdigit },
    { UPROPS_SRC_CASE,  0, case_دوګونی_Property_لري },  // UCHAR_CASED
    { UPROPS_SRC_CASE,  0, case_دوګونی_Property_لري },  // UCHAR_CASE_IGNORABLE
    { UPROPS_SRC_CASE,  0, case_دوګونی_Property_لري },  // UCHAR_CHANGES_WHEN_LOWERCASED
    { UPROPS_SRC_CASE,  0, case_دوګونی_Property_لري },  // UCHAR_CHANGES_WHEN_UPPERCASED
    { UPROPS_SRC_CASE,  0, case_دوګونی_Property_لري },  // UCHAR_CHANGES_WHEN_TITLECASED
    { UPROPS_SRC_CASE_AND_NORM,  0, changesWhenCasefolded },
    { UPROPS_SRC_CASE,  0, case_دوګونی_Property_لري },  // UCHAR_CHANGES_WHEN_CASEMAPPED
    { UPROPS_SRC_NFKC_CF, 0, changesWhenNFKC_Casefolded },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_PRESENTATION
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER_BASE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_COMPONENT
    { 2,                0, isRegionalIndicator },
    { 1,                U_ماسک(UPROPS_PREPENDED_CONCATENATION_MARK), default_لري },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EXTENDED_PICTOGRAPHIC
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_BASIC_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_KEYCAP_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_MODIFIER_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_FLAG_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_TAG_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_ZWJ_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI
    { UPROPS_SRC_IDSU, 0, isIDSUnaryOperator }, // UCHAR_IDS_UNARY_OPERATOR
    { UPROPS_SRC_ID_COMPAT_MATH, 0, is_IDCompat_Math_پیل }, // UCHAR_ID_COMPAT_MATH_START
    { UPROPS_SRC_ID_COMPAT_MATH, 0, isIDCompatMathContinue }, // UCHAR_ID_COMPAT_MATH_CONTINUE
    { UPROPS_SRC_MCM, 0 , isModifierCombiningMark }, // UCHAR_MODIFIER_COMBINING_MARK
};

U_CAPI UBool U_EXPORT2
u_has_دوګونی_Property(UChar32 c, UProperty which) {
    /* c is range-checked in the functions that are called from here */
    إذا(which<UCHAR_دوګونی_پیل || UCHAR_دوګونی_حد<=which) {
        /* not a known binary property */
        أرجع خطأ;
    } وإلا {
        ثابت دوګونی_Property &prop=binProps[which];
        أرجع prop.لري(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    أرجع u_has_دوګونی_Property(c, UCHAR_ID_پیل);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    أرجع u_has_دوګونی_Property(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_string_Has_دوګونی_Property(ثابت char16_t *s, int32_t length, UProperty which) {
    إذا (s == تش_اشاره && length != 0) { أرجع خطأ; }
    إذا (length == 1) {
        أرجع u_has_دوګونی_Property(s[0], which);  // single code point
    } وإلا إذا (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_بل(s, i, length, c);
        إذا (length > 0 ? i == length : s[i] == 0) {
            أرجع u_has_دوګونی_Property(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    أرجع UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::has_دوګونی_Property(s, length, which);
}

بنية IntProperty;

تعريفنوع int32_t Int_Property_Get_ارزښت(ثابت IntProperty &prop, UChar32 c, UProperty which);
تعريفنوع int32_t Int_Property_Get_Max_ارزښت(ثابت IntProperty &prop, UProperty which);

بنية IntProperty {
    int32_t ستنه;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t ښويدنه;  // =maxValue if getMaxValueFromShift() is used
    Int_Property_Get_ارزښت *get_ارزښت;
    Int_Property_Get_Max_ارزښت *get_Max_ارزښت;
};

ساكن int32_t default_Get_ارزښت(ثابت IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    أرجع static_cast<int32_t>(u_getUnicodeProperties(c, prop.ستنه) & prop.mask) >> prop.ښويدنه;
}

ساكن int32_t default_Get_Max_ارزښت(ثابت IntProperty &prop, UProperty /*which*/) {
    أرجع (uprv_get_Max_ارزښتونه(prop.ستنه)&prop.mask)>>prop.ښويدنه;
}

ساكن int32_t get_Max_ارزښت_From_ښويدنه(ثابت IntProperty &prop, UProperty /*which*/) {
    أرجع prop.ښويدنه;
}

ساكن int32_t getBiDiClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع static_cast<int32_t>(u_charDirection(c));
}

ساكن int32_t getBiDiPairedBracketType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

ساكن int32_t bi_Di_Get_Max_ارزښت(ثابت IntProperty &/*prop*/, UProperty which) {
    أرجع ubidi_get_Max_ارزښت(which);
}

ساكن int32_t get_بلاک(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع static_cast<int32_t>(ublock_get_کوډ(c));
}

ساكن int32_t بلاک_Get_Max_ارزښت(ثابت IntProperty &/*prop*/, UProperty /*which*/) {
    أرجع uprv_get_Max_ارزښتونه(UPROPS_MAX_ارزښتونه_OTHER_شاخص) & UPROPS_MAX_بلاک;
}

#إذا UCONFIG_NO_NORMALIZATION
ساكن int32_t getCombiningClass(ثابت IntProperty &, UChar32, UProperty) {
    أرجع 0;
}
#وإلا
ساكن int32_t getCombiningClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_getCombiningClass(c);
}
#د_که_پای

ساكن int32_t getGeneralCategory(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع static_cast<int32_t>(u_charType(c));
}

ساكن int32_t get_Joining_ډله(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_get_Joining_ډله(c);
}

ساكن int32_t getJoiningType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_getJoiningType(c);
}

ساكن int32_t getNumericType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_ارزښت(u_getMainProperties(c)));
    أرجع UPROPS_NTV_GET_TYPE(ntv);
}

ساكن int32_t getScript(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    أرجع static_cast<int32_t>(uscript_getScript(c, &error_کوډ));
}

ساكن int32_t script_Get_Max_ارزښت(ثابت IntProperty &/*prop*/, UProperty /*which*/) {
    أرجع uprv_get_Max_ارزښتونه(0)&UPROPS_MAX_SCRIPT;
}

/*
 * Map some of the Grapheme Cluster Break values to Hangul Syllable Types.
 * Hangul_Syllable_Type used to be fully redundant with a subset of Grapheme_Cluster_Break.
 *
 * Starting with Unicode 16, this is no longer true for HST=V vs. GCB=V in some cases:
 * Some Kirat Rai vowels are given GCB=V for proper grapheme clustering, but
 * they are of course not related to Hangul syllables.
 */
ساكن ثابت UHangulSyllableType gcb_منزل_Hst[]={
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
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_ماسک) >> UPROPS_GCB_ښويدنه;
    إذا(gcb<UPRV_LENGTHOF(gcb_منزل_Hst)) {
        أرجع gcb_منزل_Hst[gcb];
    } وإلا {
        أرجع U_HST_NOT_APPLICABLE;
    }
}

#إذا UCONFIG_NO_NORMALIZATION
ساكن int32_t get_Norm_چټک_Check(ثابت IntProperty &, UChar32, UProperty) {
    أرجع 0;
}
#وإلا
ساكن int32_t get_Norm_چټک_Check(ثابت IntProperty &/*prop*/, UChar32 c, UProperty which) {
    أرجع static_cast<int32_t>(unorm_get_چټک_Check(c, static_cast<UNormalization_ډېرپېښ>(which - UCHAR_NFD_چټک_CHECK + UNORM_NFD)));
}
#د_که_پای

#إذا UCONFIG_NO_NORMALIZATION
ساكن int32_t getLeadCombiningClass(ثابت IntProperty &, UChar32, UProperty) {
    أرجع 0;
}
#وإلا
ساكن int32_t getLeadCombiningClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع unorm_getFCD16(c)>>8;
}
#د_که_پای

#إذا UCONFIG_NO_NORMALIZATION
ساكن int32_t getTrailCombiningClass(ثابت IntProperty &, UChar32, UProperty) {
    أرجع 0;
}
#وإلا
ساكن int32_t getTrailCombiningClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع unorm_getFCD16(c)&0xff;
}
#د_که_پای

ساكن int32_t get_دننه_PC(ثابت IntProperty &, UChar32 c, UProperty) {
    أرجع ulayout_ensure_معلومات() && gInpcTrie != تش_اشاره ? ucptrie_get(gInpcTrie, c) : 0;
}

ساكن int32_t get_دننه_SC(ثابت IntProperty &, UChar32 c, UProperty) {
    أرجع ulayout_ensure_معلومات() && gInscTrie != تش_اشاره ? ucptrie_get(gInscTrie, c) : 0;
}

ساكن int32_t getVo(ثابت IntProperty &, UChar32 c, UProperty) {
    أرجع ulayout_ensure_معلومات() && gVoTrie != تش_اشاره ? ucptrie_get(gVoTrie, c) : 0;
}

ساكن int32_t layout_Get_Max_ارزښت(ثابت IntProperty &/*prop*/, UProperty which) {
    إذا (!ulayout_ensure_معلومات()) { أرجع 0; }
    بدل (which) {
    حالة UCHAR_INDIC_POSITIONAL_CATEGORY:
        أرجع g_Max_Inpc_ارزښت;
    حالة UCHAR_INDIC_SYLLABIC_CATEGORY:
        أرجع g_Max_Insc_ارزښت;
    حالة UCHAR_VERTICAL_ORIENTATION:
        أرجع g_Max_Vo_ارزښت;
    افتراضي:
        أرجع 0;
    }
}

ساكن int32_t get_IDStatus_ارزښت(ثابت IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_ښويدنه;
    أرجع value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_حالت_ALLOWED : U_ID_حالت_RESTRICTED;
}

ساكن ثابت IntProperty intProps[UCHAR_INT_حد-UCHAR_INT_پیل]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, bi_Di_Get_Max_ارزښت },
    { UPROPS_SRC_بلاک, 0, 0,                               get_بلاک, بلاک_Get_Max_ارزښت },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, get_Max_ارزښت_From_ښويدنه },
    { 2,                UPROPS_DT_ماسک, 0,                  default_Get_ارزښت, default_Get_Max_ارزښت },
    { 0,                UPROPS_EA_ماسک, UPROPS_EA_ښويدنه,    default_Get_ارزښت, default_Get_Max_ارزښت },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_شمېر) - 1, getGeneralCategory, get_Max_ارزښت_From_ښويدنه },
    { UPROPS_SRC_BIDI,  0, 0,                               get_Joining_ډله, bi_Di_Get_Max_ارزښت },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, bi_Di_Get_Max_ارزښت },
    { 2,                UPROPS_LB_ماسک, UPROPS_LB_ښويدنه,    default_Get_ارزښت, default_Get_Max_ارزښت },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_شمېر) - 1, getNumericType, get_Max_ارزښت_From_ښويدنه },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, script_Get_Max_ارزښت },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_شمېر) - 1, getHangulSyllableType, get_Max_ارزښت_From_ښويدنه },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), get_Norm_چټک_Check, get_Max_ارزښت_From_ښويدنه },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), get_Norm_چټک_Check, get_Max_ارزښت_From_ښويدنه },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), get_Norm_چټک_Check, get_Max_ارزښت_From_ښويدنه },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), get_Norm_چټک_Check, get_Max_ارزښت_From_ښويدنه },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, get_Max_ارزښت_From_ښويدنه },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, get_Max_ارزښت_From_ښويدنه },
    { 2,                UPROPS_GCB_ماسک, UPROPS_GCB_ښويدنه,  default_Get_ارزښت, default_Get_Max_ارزښت },
    { 2,                UPROPS_SB_ماسک, UPROPS_SB_ښويدنه,    default_Get_ارزښت, default_Get_Max_ارزښت },
    { 2,                UPROPS_WB_ماسک, UPROPS_WB_ښويدنه,    default_Get_ارزښت, default_Get_Max_ارزښت },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, bi_Di_Get_Max_ارزښت },
    { UPROPS_SRC_INPC,  0, 0,                               get_دننه_PC, layout_Get_Max_ارزښت },
    { UPROPS_SRC_INSC,  0, 0,                               get_دننه_SC, layout_Get_Max_ارزښت },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layout_Get_Max_ارزښت },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_حالت_ALLOWED), get_IDStatus_ارزښت, get_Max_ارزښت_From_ښويدنه },
    { 0,                UPROPS_INCB_ماسک, UPROPS_INCB_ښويدنه,default_Get_ارزښت, default_Get_Max_ارزښت },
};

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_ارزښت(UChar32 c, UProperty which) {
    إذا(which<UCHAR_INT_پیل) {
        إذا(UCHAR_دوګونی_پیل<=which && which<UCHAR_دوګونی_حد) {
            ثابت دوګونی_Property &prop=binProps[which];
            أرجع prop.لري(prop, c, which);
        }
    } وإلا إذا(which<UCHAR_INT_حد) {
        ثابت IntProperty &prop=intProps[which-UCHAR_INT_پیل];
        أرجع prop.get_ارزښت(prop, c, which);
    } وإلا إذا(which==UCHAR_GENERAL_CATEGORY_ماسک) {
        أرجع U_ماسک(u_charType(c));
    }
    أرجع 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Min_ارزښت(UProperty /*which*/) {
    أرجع 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Max_ارزښت(UProperty which) {
    إذا(which<UCHAR_INT_پیل) {
        إذا(UCHAR_دوګونی_پیل<=which && which<UCHAR_دوګونی_حد) {
            أرجع 1;  // maximum true for all binary properties
        }
    } وإلا إذا(which<UCHAR_INT_حد) {
        ثابت IntProperty &prop=intProps[which-UCHAR_INT_پیل];
        أرجع prop.get_Max_ارزښت(prop, which);
    }
    أرجع -1;  // undefined
}

U_CFUNC UProperty_سرچینه U_EXPORT2
uprops_get_سرچینه(UProperty which) {
    إذا(which<UCHAR_دوګونی_پیل) {
        أرجع UPROPS_SRC_NONE; /* undefined */
    } وإلا إذا(which<UCHAR_دوګونی_حد) {
        ثابت دوګونی_Property &prop=binProps[which];
        إذا(prop.mask!=0) {
            أرجع UPROPS_SRC_PROPSVEC;
        } وإلا {
            أرجع (UProperty_سرچینه)prop.ستنه;
        }
    } وإلا إذا(which<UCHAR_INT_پیل) {
        أرجع UPROPS_SRC_NONE; /* undefined */
    } وإلا إذا(which<UCHAR_INT_حد) {
        ثابت IntProperty &prop=intProps[which-UCHAR_INT_پیل];
        إذا(prop.mask!=0) {
            أرجع UPROPS_SRC_PROPSVEC;
        } وإلا {
            أرجع (UProperty_سرچینه)prop.ستنه;
        }
    } وإلا إذا(which<UCHAR_STRING_پیل) {
        بدل(which) {
        حالة UCHAR_GENERAL_CATEGORY_ماسک:
        حالة UCHAR_NUMERIC_ارزښت:
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
            أرجع UPROPS_SRC_نومونه;

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
uprops_جمع_کړه_Property_Starts(UProperty_سرچینه src, ثابت USetAdder *sa, UError_کوډ *p_Error_کوډ) {
    إذا (U_FAILURE(*p_Error_کوډ)) { أرجع; }
    إذا (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        لكل (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->جمع_کړه(sa->سېټ, c);
        }
        // single characters
        لكل (UChar32 c : ID_COMPAT_MATH_پیل) {
            sa->جمع_کړه(sa->سېټ, c);
            sa->جمع_کړه(sa->سېټ, c + 1);
        }
        أرجع;
    }
    إذا (src == UPROPS_SRC_MCM) {
        // range limits
        لكل (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->جمع_کړه(sa->سېټ, c);
        }
        أرجع;
    }
    إذا (!ulayout_ensure_معلومات(*p_Error_کوډ)) { أرجع; }
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
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    إذا (trie == تش_اشاره) {
        *p_Error_کوډ = U_MISSING_RESOURCE_ERROR;
        أرجع;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 پیل = 0, پای_ټکی;
    طالما ((پای_ټکی = ucptrie_getRange(trie, پیل, UCPMAP_RANGE_نورمال, 0,
                                   تش_اشاره, تش_اشاره, تش_اشاره)) >= 0) {
        sa->جمع_کړه(sa->سېټ, پیل);
        پیل = پای_ټکی + 1;
    }
}

U_CAPI منطقي U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t type_شاخص = type;  // also guards against negative type integers
    إذا (type_شاخص >= UPRV_LENGTHOF(uprops_id_Type_منزل_Encoded)) {
        أرجع خطأ;
    }
    uint32_t encodedType = uprops_id_Type_منزل_Encoded[type_شاخص];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_ښويدنه;
    إذا ((encodedType & UPROPS_ID_TYPE_بټ) != 0) {
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
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UError_کوډ *p_Error_کوډ) {
    إذا (U_FAILURE(*p_Error_کوډ)) { أرجع 0; }
    إذا (capacity < 0 || (capacity > 0 && types == تش_اشاره)) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_ښويدنه;
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
                    *p_Error_کوډ = U_INVALID_FORMAT_ERROR;
                    أرجع 0;
            }
            types[0] = t;
        } وإلا {
            *p_Error_کوډ = U_بفر_ډکېدنه_ERROR;
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
            *p_Error_کوډ = U_بفر_ډکېدنه_ERROR;
        }
        أرجع length;
    }
}

#إذا !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_get_FC_NFKC_تړون(UChar32 c, char16_t *dest, int32_t dest_ظرفیت, UError_کوډ *p_Error_کوډ) {
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا(dest_ظرفیت<0 || (dest==تش_اشاره && dest_ظرفیت>0)) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    ثابت Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*p_Error_کوډ);
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    ثابت char16_t *folded1;
    int32_t folded1_اوږدتیا=ucase_منزل_ډک_Folding(c, &folded1, U_FOLD_CASE_DEFAULT);
    إذا(folded1_اوږدتیا<0) {
        ثابت Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        إذا(nfkcImpl->get_Comp_چټک_Check(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            أرجع u_terminateUChars(dest, dest_ظرفیت, 0, p_Error_کوډ);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.سېټ_منزل(c);
    } وإلا {
        إذا(folded1_اوږدتیا>UCASE_MAX_STRING_اوږدتیا) {
            folded1String.سېټ_منزل(folded1_اوږدتیا);
        } وإلا {
            folded1String.سېټ_منزل(خطأ, folded1, folded1_اوږدتیا);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *p_Error_کوډ);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *p_Error_کوډ);
    // if (c != b) add the mapping from a to c
    إذا(U_FAILURE(*p_Error_کوډ) || kc1==kc2) {
        أرجع u_terminateUChars(dest, dest_ظرفیت, 0, p_Error_کوډ);
    } وإلا {
        أرجع kc2.extract(dest, dest_ظرفیت, *p_Error_کوډ);
    }
}

#د_که_پای
