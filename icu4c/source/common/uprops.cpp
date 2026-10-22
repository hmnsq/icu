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
UData_يادگيري *g_Layout_يادگيري = خالي_اشارو;

UCPTrie *gInpcTrie = خالي_اشارو;  // Indic_Positional_Category
UCPTrie *gInscTrie = خالي_اشارو;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = خالي_اشارو;  // Vertical_Orientation

int32_t g_Max_Inpc_قدر = 0;
int32_t g_Max_Insc_قدر = 0;
int32_t g_Max_Vo_قدر = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(g_Layout_يادگيري);
    g_Layout_يادگيري = خالي_اشارو;

    ucptrie_close(gInpcTrie);
    gInpcTrie = خالي_اشارو;
    ucptrie_close(gInscTrie);
    gInscTrie = خالي_اشارو;
    ucptrie_close(gVoTrie);
    gVoTrie = خالي_اشارو;

    g_Max_Inpc_قدر = 0;
    g_Max_Insc_قدر = 0;
    g_Max_Vo_قدر = 0;

    gLayoutInitOnce.reset();
    أرجع سچ;
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
        pInfo->format_نسخو[0] == 1;
}

// UInitOnce singleton initialization function
فراغ U_CALLCONV ulayout_ڀريو(UError_ڪوڊ &error_ڪوڊ) {
    g_Layout_يادگيري = udata_openChoice(
        خالي_اشارو, ULAYOUT_معلومات_TYPE, ULAYOUT_معلومات_NAME,
        ulayout_isAcceptable, خالي_اشارو, &error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }

    ثابت uint8_t* اندريون_بائيٽون = static_cast<ثابت uint8_t*>(udata_get_يادگيري(g_Layout_يادگيري));
    ثابت int32_t* اندريون_Indexes = reinterpret_cast<ثابت int32_t*>(اندريون_بائيٽون);
    int32_t indexes_ڊيگهائي = اندريون_Indexes[ULAYOUT_IX_INDEXES_ڊيگهائي];
    إذا (indexes_ڊيگهائي < 12) {
        error_ڪوڊ = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        أرجع;
    }
    int32_t وٿي = indexes_ڊيگهائي * 4;
    int32_t چوٽي = اندريون_Indexes[ULAYOUT_IX_INPC_TRIE_چوٽي];
    int32_t trie_سائيز = چوٽي - وٿي;
    إذا (trie_سائيز >= 16) {
        gInpcTrie = ucptrie_open_From_ٻٽو(
            UCPTRIE_TYPE_ANY, UCPTRIE_قدر_بٽ_ANY,
            اندريون_بائيٽون + وٿي, trie_سائيز, خالي_اشارو, &error_ڪوڊ);
    }
    وٿي = چوٽي;
    چوٽي = اندريون_Indexes[ULAYOUT_IX_INSC_TRIE_چوٽي];
    trie_سائيز = چوٽي - وٿي;
    إذا (trie_سائيز >= 16) {
        gInscTrie = ucptrie_open_From_ٻٽو(
            UCPTRIE_TYPE_ANY, UCPTRIE_قدر_بٽ_ANY,
            اندريون_بائيٽون + وٿي, trie_سائيز, خالي_اشارو, &error_ڪوڊ);
    }
    وٿي = چوٽي;
    چوٽي = اندريون_Indexes[ULAYOUT_IX_VO_TRIE_چوٽي];
    trie_سائيز = چوٽي - وٿي;
    إذا (trie_سائيز >= 16) {
        gVoTrie = ucptrie_open_From_ٻٽو(
            UCPTRIE_TYPE_ANY, UCPTRIE_قدر_بٽ_ANY,
            اندريون_بائيٽون + وٿي, trie_سائيز, خالي_اشارو, &error_ڪوڊ);
    }

    uint32_t max_قدرون = اندريون_Indexes[ULAYOUT_IX_MAX_قدرون];
    g_Max_Inpc_قدر = max_قدرون >> ULAYOUT_MAX_INPC_سرڪايو;
    g_Max_Insc_قدر = (max_قدرون >> ULAYOUT_MAX_INSC_سرڪايو) & 0xff;
    g_Max_Vo_قدر = (max_قدرون >> ULAYOUT_MAX_VO_سرڪايو) & 0xff;

    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensure_معلومات(UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_ڀريو, error_ڪوڊ);
    أرجع U_SUCCESS(error_ڪوڊ);
}

UBool ulayout_ensure_معلومات() {
    UError_ڪوڊ error_ڪوڊ = U_ZERO_ERROR;
    أرجع ulayout_ensure_معلومات(error_ڪوڊ);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

بنية ٻٽو_Property;

تعريفنوع UBool ٻٽو_Property_شامل_آهي(ثابت ٻٽو_Property &prop, UChar32 c, UProperty which);

بنية ٻٽو_Property {
    int32_t ڪالم;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    ٻٽو_Property_شامل_آهي *شامل_آهي;
};

ساكن UBool default_شامل_آهي(ثابت ٻٽو_Property &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    أرجع (u_getUnicodeProperties(c, prop.ڪالم)&prop.mask)!=0;
}

ساكن UBool case_ٻٽو_Property_شامل_آهي(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty which) {
    أرجع ucase_has_ٻٽو_Property(c, which);
}

ساكن UBool isBidiControl(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_isBidiControl(c);
}

ساكن UBool isMirrored(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_isMirrored(c);
}

ساكن UBool isJoinControl(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_isJoinControl(c);
}

#إذا UCONFIG_NO_NORMALIZATION
ساكن UBool has_ڀريل_Composition_Exclusion(ثابت ٻٽو_Property &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool has_ڀريل_Composition_Exclusion(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_ڪوڊ);
    أرجع U_SUCCESS(error_ڪوڊ) && impl->isCompNo(impl->getNorm16(c));
}
#جيڪڏهن_پورو

// UCHAR_NF*_INERT properties
#إذا UCONFIG_NO_NORMALIZATION
ساكن UBool isNormInert(ثابت ٻٽو_Property &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool isNormInert(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty which) {
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    ثابت Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalization_گهڻائي>(which - UCHAR_NFD_INERT + UNORM_NFD), error_ڪوڊ);
    أرجع U_SUCCESS(error_ڪوڊ) && norm2->isInert(c);
}
#جيڪڏهن_پورو

#إذا UCONFIG_NO_NORMALIZATION
ساكن UBool changesWhenCasefolded(ثابت ٻٽو_Property &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool changesWhenCasefolded(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    ثابت Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(error_ڪوڊ);
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خطأ;
    }
    إذا(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        إذا(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } وإلا إذا(nfd.length()<=U16_MAX_ڊيگهائي &&
                  nfd.length()==U16_ڊيگهائي(c=nfd.char32At(0))
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
        ثابت char16_t *نتيجو_String;
        أرجع ucase_منزل_ڀريل_Folding(c, &نتيجو_String, U_FOLD_CASE_DEFAULT) >= 0;
    } وإلا {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_ڊيگهائي];
        int32_t dest_ڊيگهائي;
        dest_ڊيگهائي=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.get_بفر(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &error_ڪوڊ);
        أرجع U_SUCCESS(error_ڪوڊ) &&
                       0!=u_strCompare(nfd.get_بفر(), nfd.length(),
                                       dest, dest_ڊيگهائي, خطأ);
    }
}
#جيڪڏهن_پورو

#إذا UCONFIG_NO_NORMALIZATION
ساكن UBool changesWhenNFKC_Casefolded(ثابت ٻٽو_Property &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool changesWhenNFKC_Casefolded(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    ثابت Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(error_ڪوڊ);
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خطأ;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        Reordering_بفر بفر(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        إذا(بفر.init(5, error_ڪوڊ)) {
            ثابت char16_t *srcArray=src.get_بفر();
            kcf->compose(srcArray, srcArray+src.length(), خطأ,
                          سچ, بفر, error_ڪوڊ);
        }
    }
    أرجع U_SUCCESS(error_ڪوڊ) && dest!=src;
}
#جيڪڏهن_پورو

#إذا UCONFIG_NO_NORMALIZATION
ساكن UBool isCanonSegmentStarter(ثابت ٻٽو_Property &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool isCanonSegmentStarter(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_ڪوڊ);
    أرجع
        U_SUCCESS(error_ڪوڊ) && impl->ensure_Canon_Iter_معلومات(error_ڪوڊ) &&
        impl->isCanonSegmentStarter(c);
}
#جيڪڏهن_پورو

ساكن UBool isPOSIX_alnum(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isalnumPOSIX(c);
}

ساكن UBool isPOSIX_blank(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isblank(c);
}

ساكن UBool is_POSIX_گراف(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isgraphPOSIX(c);
}

ساكن UBool isPOSIX_print(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isprintPOSIX(c);
}

ساكن UBool isPOSIX_xdigit(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isxdigit(c);
}

ساكن UBool isRegionalIndicator(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    أرجع 0x1F1E6<=c && c<=0x1F1FF;
}

ساكن UBool hasEmojiProperty(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty which) {
    أرجع EmojiProps::has_ٻٽو_Property(c, which);
}

ساكن UBool isIDSUnaryOperator(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
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
ساكن constexpr UChar32 ID_COMPAT_MATH_شروعات[] = {
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

ساكن UBool is_IDCompat_Math_شروعات(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    إذا (c < ID_COMPAT_MATH_شروعات[0]) { أرجع خطأ; }  // fastpath for common scripts
    لكل (UChar32 شروعات_Char : ID_COMPAT_MATH_شروعات) {
        إذا (c == شروعات_Char) { أرجع سچ; }
    }
    أرجع خطأ;
}

ساكن UBool isIDCompatMathContinue(ثابت ٻٽو_Property &prop, UChar32 c, UProperty /*which*/) {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        إذا (c < ID_COMPAT_MATH_CONTINUE[i]) { أرجع خطأ; }  // below range start
        إذا (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { أرجع سچ; }  // below range limit
    }
    أرجع is_IDCompat_Math_شروعات(prop, c, UCHAR_ID_COMPAT_MATH_شروعات);
}

ساكن UBool isModifierCombiningMark(ثابت ٻٽو_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        إذا (c < MODIFIER_COMBINING_MARK[i]) { أرجع خطأ; }  // below range start
        إذا (c < MODIFIER_COMBINING_MARK[i + 1]) { أرجع سچ; }  // below range limit
    }
    أرجع خطأ;
}

ساكن ثابت ٻٽو_Property binProps[UCHAR_ٻٽو_حد_بندي]={
    /*
     * column and mask values for binary properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per binary UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { 1,                U_نقاب(UPROPS_ALPHABETIC), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_ASCII_HEX_DIGIT), default_شامل_آهي },
    { UPROPS_SRC_BIDI,  0, isBidiControl },
    { UPROPS_SRC_BIDI,  0, isMirrored },
    { 1,                U_نقاب(UPROPS_DASH), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_DEFAULT_IGNORABLE_ڪوڊ_نقطو), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_DEPRECATED), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_DIACRITIC), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_EXTENDER), default_شامل_آهي },
    { UPROPS_SRC_NFC,   0, has_ڀريل_Composition_Exclusion },
    { 1,                U_نقاب(UPROPS_GRAPHEME_بنياد), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_GRAPHEME_EXTEND), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_GRAPHEME_LINK), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_HEX_DIGIT), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_HYPHEN), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_ID_CONTINUE), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_ID_شروعات), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_IDEOGRAPHIC), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_IDS_ٻٽو_OPERATOR), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_IDS_TRINARY_OPERATOR), default_شامل_آهي },
    { UPROPS_SRC_BIDI,  0, isJoinControl },
    { 1,                U_نقاب(UPROPS_LOGICAL_ترتيب_EXCEPTION), default_شامل_آهي },
    { UPROPS_SRC_CASE,  0, case_ٻٽو_Property_شامل_آهي },  // UCHAR_LOWERCASE
    { 1,                U_نقاب(UPROPS_MATH), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_NONCHARACTER_ڪوڊ_نقطو), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_QUOTATION_MARK), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_RADICAL), default_شامل_آهي },
    { UPROPS_SRC_CASE,  0, case_ٻٽو_Property_شامل_آهي },  // UCHAR_SOFT_DOTTED
    { 1,                U_نقاب(UPROPS_TERMINAL_PUNCTUATION), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_UNIFIED_IDEOGRAPH), default_شامل_آهي },
    { UPROPS_SRC_CASE,  0, case_ٻٽو_Property_شامل_آهي },  // UCHAR_UPPERCASE
    { 1,                U_نقاب(UPROPS_WHITE_SPACE), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_XID_CONTINUE), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_XID_شروعات), default_شامل_آهي },
    { UPROPS_SRC_CASE,  0, case_ٻٽو_Property_شامل_آهي },  // UCHAR_CASE_SENSITIVE
    { 1,                U_نقاب(UPROPS_S_رڪن), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_VARIATION_SELECTOR), default_شامل_آهي },
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFD_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKD_INERT
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFC_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKC_INERT
    { UPROPS_SRC_NFC_CANON_ITER, 0, isCanonSegmentStarter },
    { 1,                U_نقاب(UPROPS_نمونو_SYNTAX), default_شامل_آهي },
    { 1,                U_نقاب(UPROPS_نمونو_WHITE_SPACE), default_شامل_آهي },
    { UPROPS_SRC_CHAR_AND_PROPSVEC,  0, isPOSIX_alnum },
    { UPROPS_SRC_CHAR,  0, isPOSIX_blank },
    { UPROPS_SRC_CHAR,  0, is_POSIX_گراف },
    { UPROPS_SRC_CHAR,  0, isPOSIX_print },
    { UPROPS_SRC_CHAR,  0, isPOSIX_xdigit },
    { UPROPS_SRC_CASE,  0, case_ٻٽو_Property_شامل_آهي },  // UCHAR_CASED
    { UPROPS_SRC_CASE,  0, case_ٻٽو_Property_شامل_آهي },  // UCHAR_CASE_IGNORABLE
    { UPROPS_SRC_CASE,  0, case_ٻٽو_Property_شامل_آهي },  // UCHAR_CHANGES_WHEN_LOWERCASED
    { UPROPS_SRC_CASE,  0, case_ٻٽو_Property_شامل_آهي },  // UCHAR_CHANGES_WHEN_UPPERCASED
    { UPROPS_SRC_CASE,  0, case_ٻٽو_Property_شامل_آهي },  // UCHAR_CHANGES_WHEN_TITLECASED
    { UPROPS_SRC_CASE_AND_NORM,  0, changesWhenCasefolded },
    { UPROPS_SRC_CASE,  0, case_ٻٽو_Property_شامل_آهي },  // UCHAR_CHANGES_WHEN_CASEMAPPED
    { UPROPS_SRC_NFKC_CF, 0, changesWhenNFKC_Casefolded },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_PRESENTATION
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER_BASE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_COMPONENT
    { 2,                0, isRegionalIndicator },
    { 1,                U_نقاب(UPROPS_PREPENDED_CONCATENATION_MARK), default_شامل_آهي },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EXTENDED_PICTOGRAPHIC
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_BASIC_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_KEYCAP_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_MODIFIER_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_FLAG_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_TAG_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_ZWJ_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI
    { UPROPS_SRC_IDSU, 0, isIDSUnaryOperator }, // UCHAR_IDS_UNARY_OPERATOR
    { UPROPS_SRC_ID_COMPAT_MATH, 0, is_IDCompat_Math_شروعات }, // UCHAR_ID_COMPAT_MATH_START
    { UPROPS_SRC_ID_COMPAT_MATH, 0, isIDCompatMathContinue }, // UCHAR_ID_COMPAT_MATH_CONTINUE
    { UPROPS_SRC_MCM, 0 , isModifierCombiningMark }, // UCHAR_MODIFIER_COMBINING_MARK
};

U_CAPI UBool U_EXPORT2
u_has_ٻٽو_Property(UChar32 c, UProperty which) {
    /* c is range-checked in the functions that are called from here */
    إذا(which<UCHAR_ٻٽو_شروعات || UCHAR_ٻٽو_حد_بندي<=which) {
        /* not a known binary property */
        أرجع خطأ;
    } وإلا {
        ثابت ٻٽو_Property &prop=binProps[which];
        أرجع prop.شامل_آهي(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    أرجع u_has_ٻٽو_Property(c, UCHAR_ID_شروعات);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    أرجع u_has_ٻٽو_Property(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_string_Has_ٻٽو_Property(ثابت char16_t *s, int32_t length, UProperty which) {
    إذا (s == خالي_اشارو && length != 0) { أرجع خطأ; }
    إذا (length == 1) {
        أرجع u_has_ٻٽو_Property(s[0], which);  // single code point
    } وإلا إذا (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_ايندڙ(s, i, length, c);
        إذا (length > 0 ? i == length : s[i] == 0) {
            أرجع u_has_ٻٽو_Property(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    أرجع UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::has_ٻٽو_Property(s, length, which);
}

بنية IntProperty;

تعريفنوع int32_t Int_Property_Get_قدر(ثابت IntProperty &prop, UChar32 c, UProperty which);
تعريفنوع int32_t Int_Property_Get_Max_قدر(ثابت IntProperty &prop, UProperty which);

بنية IntProperty {
    int32_t ڪالم;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t سرڪايو;  // =maxValue if getMaxValueFromShift() is used
    Int_Property_Get_قدر *get_قدر;
    Int_Property_Get_Max_قدر *get_Max_قدر;
};

ساكن int32_t default_Get_قدر(ثابت IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    أرجع static_cast<int32_t>(u_getUnicodeProperties(c, prop.ڪالم) & prop.mask) >> prop.سرڪايو;
}

ساكن int32_t default_Get_Max_قدر(ثابت IntProperty &prop, UProperty /*which*/) {
    أرجع (uprv_get_Max_قدرون(prop.ڪالم)&prop.mask)>>prop.سرڪايو;
}

ساكن int32_t get_Max_قدر_From_سرڪايو(ثابت IntProperty &prop, UProperty /*which*/) {
    أرجع prop.سرڪايو;
}

ساكن int32_t getBiDiClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع static_cast<int32_t>(u_charDirection(c));
}

ساكن int32_t getBiDiPairedBracketType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

ساكن int32_t bi_Di_Get_Max_قدر(ثابت IntProperty &/*prop*/, UProperty which) {
    أرجع ubidi_get_Max_قدر(which);
}

ساكن int32_t get_بلاڪ(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع static_cast<int32_t>(ublock_get_ڪوڊ(c));
}

ساكن int32_t بلاڪ_Get_Max_قدر(ثابت IntProperty &/*prop*/, UProperty /*which*/) {
    أرجع uprv_get_Max_قدرون(UPROPS_MAX_قدرون_OTHER_اشاريو) & UPROPS_MAX_بلاڪ;
}

#إذا UCONFIG_NO_NORMALIZATION
ساكن int32_t getCombiningClass(ثابت IntProperty &, UChar32, UProperty) {
    أرجع 0;
}
#وإلا
ساكن int32_t getCombiningClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_getCombiningClass(c);
}
#جيڪڏهن_پورو

ساكن int32_t getGeneralCategory(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع static_cast<int32_t>(u_charType(c));
}

ساكن int32_t get_Joining_گروهه(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_get_Joining_گروهه(c);
}

ساكن int32_t getJoiningType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_getJoiningType(c);
}

ساكن int32_t getNumericType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_قدر(u_getMainProperties(c)));
    أرجع UPROPS_NTV_GET_TYPE(ntv);
}

ساكن int32_t getScript(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    أرجع static_cast<int32_t>(uscript_getScript(c, &error_ڪوڊ));
}

ساكن int32_t script_Get_Max_قدر(ثابت IntProperty &/*prop*/, UProperty /*which*/) {
    أرجع uprv_get_Max_قدرون(0)&UPROPS_MAX_SCRIPT;
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
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_نقاب) >> UPROPS_GCB_سرڪايو;
    إذا(gcb<UPRV_LENGTHOF(gcb_منزل_Hst)) {
        أرجع gcb_منزل_Hst[gcb];
    } وإلا {
        أرجع U_HST_NOT_APPLICABLE;
    }
}

#إذا UCONFIG_NO_NORMALIZATION
ساكن int32_t get_Norm_تيز_Check(ثابت IntProperty &, UChar32, UProperty) {
    أرجع 0;
}
#وإلا
ساكن int32_t get_Norm_تيز_Check(ثابت IntProperty &/*prop*/, UChar32 c, UProperty which) {
    أرجع static_cast<int32_t>(unorm_get_تيز_Check(c, static_cast<UNormalization_گهڻائي>(which - UCHAR_NFD_تيز_CHECK + UNORM_NFD)));
}
#جيڪڏهن_پورو

#إذا UCONFIG_NO_NORMALIZATION
ساكن int32_t getLeadCombiningClass(ثابت IntProperty &, UChar32, UProperty) {
    أرجع 0;
}
#وإلا
ساكن int32_t getLeadCombiningClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع unorm_getFCD16(c)>>8;
}
#جيڪڏهن_پورو

#إذا UCONFIG_NO_NORMALIZATION
ساكن int32_t getTrailCombiningClass(ثابت IntProperty &, UChar32, UProperty) {
    أرجع 0;
}
#وإلا
ساكن int32_t getTrailCombiningClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع unorm_getFCD16(c)&0xff;
}
#جيڪڏهن_پورو

ساكن int32_t get_اندريون_PC(ثابت IntProperty &, UChar32 c, UProperty) {
    أرجع ulayout_ensure_معلومات() && gInpcTrie != خالي_اشارو ? ucptrie_get(gInpcTrie, c) : 0;
}

ساكن int32_t get_اندريون_SC(ثابت IntProperty &, UChar32 c, UProperty) {
    أرجع ulayout_ensure_معلومات() && gInscTrie != خالي_اشارو ? ucptrie_get(gInscTrie, c) : 0;
}

ساكن int32_t getVo(ثابت IntProperty &, UChar32 c, UProperty) {
    أرجع ulayout_ensure_معلومات() && gVoTrie != خالي_اشارو ? ucptrie_get(gVoTrie, c) : 0;
}

ساكن int32_t layout_Get_Max_قدر(ثابت IntProperty &/*prop*/, UProperty which) {
    إذا (!ulayout_ensure_معلومات()) { أرجع 0; }
    بدل (which) {
    حالة UCHAR_INDIC_POSITIONAL_CATEGORY:
        أرجع g_Max_Inpc_قدر;
    حالة UCHAR_INDIC_SYLLABIC_CATEGORY:
        أرجع g_Max_Insc_قدر;
    حالة UCHAR_VERTICAL_ORIENTATION:
        أرجع g_Max_Vo_قدر;
    افتراضي:
        أرجع 0;
    }
}

ساكن int32_t get_IDStatus_قدر(ثابت IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_سرڪايو;
    أرجع value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_حالت_ALLOWED : U_ID_حالت_RESTRICTED;
}

ساكن ثابت IntProperty intProps[UCHAR_INT_حد_بندي-UCHAR_INT_شروعات]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, bi_Di_Get_Max_قدر },
    { UPROPS_SRC_بلاڪ, 0, 0,                               get_بلاڪ, بلاڪ_Get_Max_قدر },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, get_Max_قدر_From_سرڪايو },
    { 2,                UPROPS_DT_نقاب, 0,                  default_Get_قدر, default_Get_Max_قدر },
    { 0,                UPROPS_EA_نقاب, UPROPS_EA_سرڪايو,    default_Get_قدر, default_Get_Max_قدر },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_ڳڻپ) - 1, getGeneralCategory, get_Max_قدر_From_سرڪايو },
    { UPROPS_SRC_BIDI,  0, 0,                               get_Joining_گروهه, bi_Di_Get_Max_قدر },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, bi_Di_Get_Max_قدر },
    { 2,                UPROPS_LB_نقاب, UPROPS_LB_سرڪايو,    default_Get_قدر, default_Get_Max_قدر },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_ڳڻپ) - 1, getNumericType, get_Max_قدر_From_سرڪايو },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, script_Get_Max_قدر },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_ڳڻپ) - 1, getHangulSyllableType, get_Max_قدر_From_سرڪايو },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), get_Norm_تيز_Check, get_Max_قدر_From_سرڪايو },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), get_Norm_تيز_Check, get_Max_قدر_From_سرڪايو },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), get_Norm_تيز_Check, get_Max_قدر_From_سرڪايو },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), get_Norm_تيز_Check, get_Max_قدر_From_سرڪايو },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, get_Max_قدر_From_سرڪايو },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, get_Max_قدر_From_سرڪايو },
    { 2,                UPROPS_GCB_نقاب, UPROPS_GCB_سرڪايو,  default_Get_قدر, default_Get_Max_قدر },
    { 2,                UPROPS_SB_نقاب, UPROPS_SB_سرڪايو,    default_Get_قدر, default_Get_Max_قدر },
    { 2,                UPROPS_WB_نقاب, UPROPS_WB_سرڪايو,    default_Get_قدر, default_Get_Max_قدر },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, bi_Di_Get_Max_قدر },
    { UPROPS_SRC_INPC,  0, 0,                               get_اندريون_PC, layout_Get_Max_قدر },
    { UPROPS_SRC_INSC,  0, 0,                               get_اندريون_SC, layout_Get_Max_قدر },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layout_Get_Max_قدر },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_حالت_ALLOWED), get_IDStatus_قدر, get_Max_قدر_From_سرڪايو },
    { 0,                UPROPS_INCB_نقاب, UPROPS_INCB_سرڪايو,default_Get_قدر, default_Get_Max_قدر },
};

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_قدر(UChar32 c, UProperty which) {
    إذا(which<UCHAR_INT_شروعات) {
        إذا(UCHAR_ٻٽو_شروعات<=which && which<UCHAR_ٻٽو_حد_بندي) {
            ثابت ٻٽو_Property &prop=binProps[which];
            أرجع prop.شامل_آهي(prop, c, which);
        }
    } وإلا إذا(which<UCHAR_INT_حد_بندي) {
        ثابت IntProperty &prop=intProps[which-UCHAR_INT_شروعات];
        أرجع prop.get_قدر(prop, c, which);
    } وإلا إذا(which==UCHAR_GENERAL_CATEGORY_نقاب) {
        أرجع U_نقاب(u_charType(c));
    }
    أرجع 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Min_قدر(UProperty /*which*/) {
    أرجع 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Max_قدر(UProperty which) {
    إذا(which<UCHAR_INT_شروعات) {
        إذا(UCHAR_ٻٽو_شروعات<=which && which<UCHAR_ٻٽو_حد_بندي) {
            أرجع 1;  // maximum true for all binary properties
        }
    } وإلا إذا(which<UCHAR_INT_حد_بندي) {
        ثابت IntProperty &prop=intProps[which-UCHAR_INT_شروعات];
        أرجع prop.get_Max_قدر(prop, which);
    }
    أرجع -1;  // undefined
}

U_CFUNC UProperty_ذريعو U_EXPORT2
uprops_get_ذريعو(UProperty which) {
    إذا(which<UCHAR_ٻٽو_شروعات) {
        أرجع UPROPS_SRC_NONE; /* undefined */
    } وإلا إذا(which<UCHAR_ٻٽو_حد_بندي) {
        ثابت ٻٽو_Property &prop=binProps[which];
        إذا(prop.mask!=0) {
            أرجع UPROPS_SRC_PROPSVEC;
        } وإلا {
            أرجع (UProperty_ذريعو)prop.ڪالم;
        }
    } وإلا إذا(which<UCHAR_INT_شروعات) {
        أرجع UPROPS_SRC_NONE; /* undefined */
    } وإلا إذا(which<UCHAR_INT_حد_بندي) {
        ثابت IntProperty &prop=intProps[which-UCHAR_INT_شروعات];
        إذا(prop.mask!=0) {
            أرجع UPROPS_SRC_PROPSVEC;
        } وإلا {
            أرجع (UProperty_ذريعو)prop.ڪالم;
        }
    } وإلا إذا(which<UCHAR_STRING_شروعات) {
        بدل(which) {
        حالة UCHAR_GENERAL_CATEGORY_نقاب:
        حالة UCHAR_NUMERIC_قدر:
            أرجع UPROPS_SRC_CHAR;

        افتراضي:
            أرجع UPROPS_SRC_NONE;
        }
    } وإلا إذا(which<UCHAR_STRING_حد_بندي) {
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
            أرجع UPROPS_SRC_نالا;

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
uprops_جوڙيو_Property_Starts(UProperty_ذريعو src, ثابت USetAdder *sa, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (U_FAILURE(*p_Error_ڪوڊ)) { أرجع; }
    إذا (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        لكل (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->جوڙيو(sa->سيٽ, c);
        }
        // single characters
        لكل (UChar32 c : ID_COMPAT_MATH_شروعات) {
            sa->جوڙيو(sa->سيٽ, c);
            sa->جوڙيو(sa->سيٽ, c + 1);
        }
        أرجع;
    }
    إذا (src == UPROPS_SRC_MCM) {
        // range limits
        لكل (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->جوڙيو(sa->سيٽ, c);
        }
        أرجع;
    }
    إذا (!ulayout_ensure_معلومات(*p_Error_ڪوڊ)) { أرجع; }
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
        *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    إذا (trie == خالي_اشارو) {
        *p_Error_ڪوڊ = U_MISSING_RESOURCE_ERROR;
        أرجع;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 شروعات = 0, پڇاڙي;
    طالما ((پڇاڙي = ucptrie_getRange(trie, شروعات, UCPMAP_RANGE_عام, 0,
                                   خالي_اشارو, خالي_اشارو, خالي_اشارو)) >= 0) {
        sa->جوڙيو(sa->سيٽ, شروعات);
        شروعات = پڇاڙي + 1;
    }
}

U_CAPI منطقي U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t type_اشاريو = type;  // also guards against negative type integers
    إذا (type_اشاريو >= UPRV_LENGTHOF(uprops_id_Type_منزل_Encoded)) {
        أرجع خطأ;
    }
    uint32_t encodedType = uprops_id_Type_منزل_Encoded[type_اشاريو];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_سرڪايو;
    إذا ((encodedType & UPROPS_ID_TYPE_ٻٽو_انگ) != 0) {
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
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (U_FAILURE(*p_Error_ڪوڊ)) { أرجع 0; }
    إذا (capacity < 0 || (capacity > 0 && types == خالي_اشارو)) {
        *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_سرڪايو;
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
                    *p_Error_ڪوڊ = U_INVALID_FORMAT_ERROR;
                    أرجع 0;
            }
            types[0] = t;
        } وإلا {
            *p_Error_ڪوڊ = U_بفر_حد_کان_مٿي_ERROR;
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
            *p_Error_ڪوڊ = U_بفر_حد_کان_مٿي_ERROR;
        }
        أرجع length;
    }
}

#إذا !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_get_FC_NFKC_گهيرو(UChar32 c, char16_t *dest, int32_t dest_صلاحيت, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(dest_صلاحيت<0 || (dest==خالي_اشارو && dest_صلاحيت>0)) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    ثابت Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*p_Error_ڪوڊ);
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    ثابت char16_t *folded1;
    int32_t folded1_ڊيگهائي=ucase_منزل_ڀريل_Folding(c, &folded1, U_FOLD_CASE_DEFAULT);
    إذا(folded1_ڊيگهائي<0) {
        ثابت Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        إذا(nfkcImpl->get_Comp_تيز_Check(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            أرجع u_terminateUChars(dest, dest_صلاحيت, 0, p_Error_ڪوڊ);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.سيٽ_منزل(c);
    } وإلا {
        إذا(folded1_ڊيگهائي>UCASE_MAX_STRING_ڊيگهائي) {
            folded1String.سيٽ_منزل(folded1_ڊيگهائي);
        } وإلا {
            folded1String.سيٽ_منزل(خطأ, folded1, folded1_ڊيگهائي);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *p_Error_ڪوڊ);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *p_Error_ڪوڊ);
    // if (c != b) add the mapping from a to c
    إذا(U_FAILURE(*p_Error_ڪوڊ) || kc1==kc2) {
        أرجع u_terminateUChars(dest, dest_صلاحيت, 0, p_Error_ڪوڊ);
    } وإلا {
        أرجع kc2.extract(dest, dest_صلاحيت, *p_Error_ڪوڊ);
    }
}

#جيڪڏهن_پورو
