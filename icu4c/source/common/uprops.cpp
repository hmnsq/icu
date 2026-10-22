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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/ucptrie.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "unicode/unorm2.h"
#అన్తర్భావ "unicode/uscript.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "emojiprops.h"
#అన్తర్భావ "mutex.h"
#అన్తర్భావ "normalizer2impl.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "ubidi_props.h"
#అన్తర్భావ "uprops.h"
#అన్తర్భావ "ucase.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "ulayout_props.h"
#అన్తర్భావ "ustr_imp.h"

U_NAMESPACE_USE

// Unicode text layout properties data -----------------------------------------

నామాకాశ {

icu::UInitOnce gLayoutInitOnce {};
UData_జ్ఞాపకం *g_Layout_జ్ఞాపకం = శూన్యసూచిక;

UCPTrie *gInpcTrie = శూన్యసూచిక;  // Indic_Positional_Category
UCPTrie *gInscTrie = శూన్యసూచిక;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = శూన్యసూచిక;  // Vertical_Orientation

int32_t g_Max_Inpc_విలువ = 0;
int32_t g_Max_Insc_విలువ = 0;
int32_t g_Max_Vo_విలువ = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(g_Layout_జ్ఞాపకం);
    g_Layout_జ్ఞాపకం = శూన్యసూచిక;

    ucptrie_close(gInpcTrie);
    gInpcTrie = శూన్యసూచిక;
    ucptrie_close(gInscTrie);
    gInscTrie = శూన్యసూచిక;
    ucptrie_close(gVoTrie);
    gVoTrie = శూన్యసూచిక;

    g_Max_Inpc_విలువ = 0;
    g_Max_Insc_విలువ = 0;
    g_Max_Vo_విలువ = 0;

    gLayoutInitOnce.reset();
    తిరిగి నిజం;
}

UBool U_CALLCONV
ulayout_isAcceptable(శూన్య * /*context*/,
                     స్థిరరాశి అక్షరం * /* type */, స్థిరరాశి అక్షరం * /*name*/,
                     స్థిరరాశి UDataInfo *pInfo) {
    తిరిగి pInfo->size >= 20 &&
        pInfo->isBigEndian == U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily == U_CHARSET_FAMILY &&
        pInfo->దత్తాంశం_Format[0] == ULAYOUT_FMT_0 &&
        pInfo->దత్తాంశం_Format[1] == ULAYOUT_FMT_1 &&
        pInfo->దత్తాంశం_Format[2] == ULAYOUT_FMT_2 &&
        pInfo->దత్తాంశం_Format[3] == ULAYOUT_FMT_3 &&
        pInfo->format_సంస్కరణ[0] == 1;
}

// UInitOnce singleton initialization function
శూన్య U_CALLCONV ulayout_ఎక్కించు(UError_సంకేతం &error_సంకేతం) {
    g_Layout_జ్ఞాపకం = udata_openChoice(
        శూన్యసూచిక, ULAYOUT_దత్తాంశం_TYPE, ULAYOUT_దత్తాంశం_NAME,
        ulayout_isAcceptable, శూన్యసూచిక, &error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }

    స్థిరరాశి uint8_t* లోపల_బైట్లు = static_cast<స్థిరరాశి uint8_t*>(udata_get_జ్ఞాపకం(g_Layout_జ్ఞాపకం));
    స్థిరరాశి int32_t* లోపల_Indexes = reinterpret_cast<స్థిరరాశి int32_t*>(లోపల_బైట్లు);
    int32_t indexes_నిడివి = లోపల_Indexes[ULAYOUT_IX_INDEXES_నిడివి];
    ఒకవేళ (indexes_నిడివి < 12) {
        error_సంకేతం = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        తిరిగి;
    }
    int32_t స్థానభ్రంశం = indexes_నిడివి * 4;
    int32_t శిఖరం = లోపల_Indexes[ULAYOUT_IX_INPC_TRIE_శిఖరం];
    int32_t trie_కొలత = శిఖరం - స్థానభ్రంశం;
    ఒకవేళ (trie_కొలత >= 16) {
        gInpcTrie = ucptrie_open_From_ద్విమాన(
            UCPTRIE_TYPE_ANY, UCPTRIE_విలువ_బిట్లు_ANY,
            లోపల_బైట్లు + స్థానభ్రంశం, trie_కొలత, శూన్యసూచిక, &error_సంకేతం);
    }
    స్థానభ్రంశం = శిఖరం;
    శిఖరం = లోపల_Indexes[ULAYOUT_IX_INSC_TRIE_శిఖరం];
    trie_కొలత = శిఖరం - స్థానభ్రంశం;
    ఒకవేళ (trie_కొలత >= 16) {
        gInscTrie = ucptrie_open_From_ద్విమాన(
            UCPTRIE_TYPE_ANY, UCPTRIE_విలువ_బిట్లు_ANY,
            లోపల_బైట్లు + స్థానభ్రంశం, trie_కొలత, శూన్యసూచిక, &error_సంకేతం);
    }
    స్థానభ్రంశం = శిఖరం;
    శిఖరం = లోపల_Indexes[ULAYOUT_IX_VO_TRIE_శిఖరం];
    trie_కొలత = శిఖరం - స్థానభ్రంశం;
    ఒకవేళ (trie_కొలత >= 16) {
        gVoTrie = ucptrie_open_From_ద్విమాన(
            UCPTRIE_TYPE_ANY, UCPTRIE_విలువ_బిట్లు_ANY,
            లోపల_బైట్లు + స్థానభ్రంశం, trie_కొలత, శూన్యసూచిక, &error_సంకేతం);
    }

    uint32_t max_విలువలు = లోపల_Indexes[ULAYOUT_IX_MAX_విలువలు];
    g_Max_Inpc_విలువ = max_విలువలు >> ULAYOUT_MAX_INPC_జరుపు;
    g_Max_Insc_విలువ = (max_విలువలు >> ULAYOUT_MAX_INSC_జరుపు) & 0xff;
    g_Max_Vo_విలువ = (max_విలువలు >> ULAYOUT_MAX_VO_జరుపు) & 0xff;

    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensure_దత్తాంశం(UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_ఎక్కించు, error_సంకేతం);
    తిరిగి U_SUCCESS(error_సంకేతం);
}

UBool ulayout_ensure_దత్తాంశం() {
    UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
    తిరిగి ulayout_ensure_దత్తాంశం(error_సంకేతం);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

నిర్మాణం ద్విమాన_Property;

రకనిర్వచనం UBool ద్విమాన_Property_కలిగియుంది(స్థిరరాశి ద్విమాన_Property &prop, UChar32 c, UProperty which);

నిర్మాణం ద్విమాన_Property {
    int32_t స్తంభం;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    ద్విమాన_Property_కలిగియుంది *కలిగియుంది;
};

స్థిర UBool default_కలిగియుంది(స్థిరరాశి ద్విమాన_Property &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    తిరిగి (u_getUnicodeProperties(c, prop.స్తంభం)&prop.mask)!=0;
}

స్థిర UBool case_ద్విమాన_Property_కలిగియుంది(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty which) {
    తిరిగి ucase_has_ద్విమాన_Property(c, which);
}

స్థిర UBool isBidiControl(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి ubidi_isBidiControl(c);
}

స్థిర UBool isMirrored(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి ubidi_isMirrored(c);
}

స్థిర UBool isJoinControl(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి ubidi_isJoinControl(c);
}

#ఒకవేళ UCONFIG_NO_NORMALIZATION
స్థిర UBool has_నిండిన_Composition_Exclusion(స్థిరరాశి ద్విమాన_Property &, UChar32, UProperty) {
    తిరిగి అబద్ధం;
}
#లేకపోతే
స్థిర UBool has_నిండిన_Composition_Exclusion(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    స్థిరరాశి Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_సంకేతం);
    తిరిగి U_SUCCESS(error_సంకేతం) && impl->isCompNo(impl->getNorm16(c));
}
#అయితే_ముగింపు

// UCHAR_NF*_INERT properties
#ఒకవేళ UCONFIG_NO_NORMALIZATION
స్థిర UBool isNormInert(స్థిరరాశి ద్విమాన_Property &, UChar32, UProperty) {
    తిరిగి అబద్ధం;
}
#లేకపోతే
స్థిర UBool isNormInert(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty which) {
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    స్థిరరాశి Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalization_బహుళకం>(which - UCHAR_NFD_INERT + UNORM_NFD), error_సంకేతం);
    తిరిగి U_SUCCESS(error_సంకేతం) && norm2->isInert(c);
}
#అయితే_ముగింపు

#ఒకవేళ UCONFIG_NO_NORMALIZATION
స్థిర UBool changesWhenCasefolded(స్థిరరాశి ద్విమాన_Property &, UChar32, UProperty) {
    తిరిగి అబద్ధం;
}
#లేకపోతే
స్థిర UBool changesWhenCasefolded(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    స్థిరరాశి Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(error_సంకేతం);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        ఒకవేళ(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } లేకపోతే ఒకవేళ(nfd.length()<=U16_MAX_నిడివి &&
                  nfd.length()==U16_నిడివి(c=nfd.char32At(0))
        ) {
            /* single supplementary code point */
        } లేకపోతే {
            c=U_SENTINEL;
        }
    } లేకపోతే ఒకవేళ(c<0) {
        తిరిగి అబద్ధం;  /* protect against bad input */
    }
    ఒకవేళ(c>=0) {
        /* single code point */
        స్థిరరాశి char16_t *ఫలితం_String;
        తిరిగి ucase_గమ్యం_నిండిన_Folding(c, &ఫలితం_String, U_FOLD_CASE_DEFAULT) >= 0;
    } లేకపోతే {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_నిడివి];
        int32_t dest_నిడివి;
        dest_నిడివి=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.get_బఫరు(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &error_సంకేతం);
        తిరిగి U_SUCCESS(error_సంకేతం) &&
                       0!=u_strCompare(nfd.get_బఫరు(), nfd.length(),
                                       dest, dest_నిడివి, అబద్ధం);
    }
}
#అయితే_ముగింపు

#ఒకవేళ UCONFIG_NO_NORMALIZATION
స్థిర UBool changesWhenNFKC_Casefolded(స్థిరరాశి ద్విమాన_Property &, UChar32, UProperty) {
    తిరిగి అబద్ధం;
}
#లేకపోతే
స్థిర UBool changesWhenNFKC_Casefolded(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    స్థిరరాశి Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(error_సంకేతం);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        Reordering_బఫరు బఫరు(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        ఒకవేళ(బఫరు.init(5, error_సంకేతం)) {
            స్థిరరాశి char16_t *srcArray=src.get_బఫరు();
            kcf->compose(srcArray, srcArray+src.length(), అబద్ధం,
                          నిజం, బఫరు, error_సంకేతం);
        }
    }
    తిరిగి U_SUCCESS(error_సంకేతం) && dest!=src;
}
#అయితే_ముగింపు

#ఒకవేళ UCONFIG_NO_NORMALIZATION
స్థిర UBool isCanonSegmentStarter(స్థిరరాశి ద్విమాన_Property &, UChar32, UProperty) {
    తిరిగి అబద్ధం;
}
#లేకపోతే
స్థిర UBool isCanonSegmentStarter(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    స్థిరరాశి Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_సంకేతం);
    తిరిగి
        U_SUCCESS(error_సంకేతం) && impl->ensure_Canon_Iter_దత్తాంశం(error_సంకేతం) &&
        impl->isCanonSegmentStarter(c);
}
#అయితే_ముగింపు

స్థిర UBool isPOSIX_alnum(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి u_isalnumPOSIX(c);
}

స్థిర UBool isPOSIX_blank(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి u_isblank(c);
}

స్థిర UBool is_POSIX_లేఖాచిత్రం(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి u_isgraphPOSIX(c);
}

స్థిర UBool isPOSIX_print(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి u_isprintPOSIX(c);
}

స్థిర UBool isPOSIX_xdigit(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి u_isxdigit(c);
}

స్థిర UBool isRegionalIndicator(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    తిరిగి 0x1F1E6<=c && c<=0x1F1FF;
}

స్థిర UBool hasEmojiProperty(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty which) {
    తిరిగి EmojiProps::has_ద్విమాన_Property(c, which);
}

స్థిర UBool isIDSUnaryOperator(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // New in Unicode 15.1 for just two characters.
    తిరిగి 0x2FFE<=c && c<=0x2FFF;
}

/** Ranges (start/limit pairs) of ID_Compat_Math_Continue (only), from UCD PropList.txt. */
స్థిర constexpr UChar32 ID_COMPAT_MATH_CONTINUE[] = {
    0x00B2, 0x00B3 + 1,
    0x00B9, 0x00B9 + 1,
    0x2070, 0x2070 + 1,
    0x2074, 0x207E + 1,
    0x2080, 0x208E + 1
};

/** ID_Compat_Math_Start characters, from UCD PropList.txt. */
స్థిర constexpr UChar32 ID_COMPAT_MATH_ప్రారంభం[] = {
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
స్థిర constexpr UChar32 MODIFIER_COMBINING_MARK[] = {
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

స్థిర UBool is_IDCompat_Math_ప్రారంభం(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ఒకవేళ (c < ID_COMPAT_MATH_ప్రారంభం[0]) { తిరిగి అబద్ధం; }  // fastpath for common scripts
    కొరకు (UChar32 ప్రారంభం_Char : ID_COMPAT_MATH_ప్రారంభం) {
        ఒకవేళ (c == ప్రారంభం_Char) { తిరిగి నిజం; }
    }
    తిరిగి అబద్ధం;
}

స్థిర UBool isIDCompatMathContinue(స్థిరరాశి ద్విమాన_Property &prop, UChar32 c, UProperty /*which*/) {
    కొరకు (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        ఒకవేళ (c < ID_COMPAT_MATH_CONTINUE[i]) { తిరిగి అబద్ధం; }  // below range start
        ఒకవేళ (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { తిరిగి నిజం; }  // below range limit
    }
    తిరిగి is_IDCompat_Math_ప్రారంభం(prop, c, UCHAR_ID_COMPAT_MATH_ప్రారంభం);
}

స్థిర UBool isModifierCombiningMark(స్థిరరాశి ద్విమాన_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    కొరకు (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        ఒకవేళ (c < MODIFIER_COMBINING_MARK[i]) { తిరిగి అబద్ధం; }  // below range start
        ఒకవేళ (c < MODIFIER_COMBINING_MARK[i + 1]) { తిరిగి నిజం; }  // below range limit
    }
    తిరిగి అబద్ధం;
}

స్థిర స్థిరరాశి ద్విమాన_Property binProps[UCHAR_ద్విమాన_హద్దు]={
    /*
     * column and mask values for binary properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per binary UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { 1,                U_ముసుగు(UPROPS_ALPHABETIC), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_ASCII_HEX_DIGIT), default_కలిగియుంది },
    { UPROPS_SRC_BIDI,  0, isBidiControl },
    { UPROPS_SRC_BIDI,  0, isMirrored },
    { 1,                U_ముసుగు(UPROPS_DASH), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_DEFAULT_IGNORABLE_సంకేతం_బిందువు), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_DEPRECATED), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_DIACRITIC), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_EXTENDER), default_కలిగియుంది },
    { UPROPS_SRC_NFC,   0, has_నిండిన_Composition_Exclusion },
    { 1,                U_ముసుగు(UPROPS_GRAPHEME_పునాది), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_GRAPHEME_EXTEND), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_GRAPHEME_LINK), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_HEX_DIGIT), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_HYPHEN), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_ID_CONTINUE), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_ID_ప్రారంభం), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_IDEOGRAPHIC), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_IDS_ద్విమాన_OPERATOR), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_IDS_TRINARY_OPERATOR), default_కలిగియుంది },
    { UPROPS_SRC_BIDI,  0, isJoinControl },
    { 1,                U_ముసుగు(UPROPS_LOGICAL_క్రమం_EXCEPTION), default_కలిగియుంది },
    { UPROPS_SRC_CASE,  0, case_ద్విమాన_Property_కలిగియుంది },  // UCHAR_LOWERCASE
    { 1,                U_ముసుగు(UPROPS_MATH), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_NONCHARACTER_సంకేతం_బిందువు), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_QUOTATION_MARK), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_RADICAL), default_కలిగియుంది },
    { UPROPS_SRC_CASE,  0, case_ద్విమాన_Property_కలిగియుంది },  // UCHAR_SOFT_DOTTED
    { 1,                U_ముసుగు(UPROPS_TERMINAL_PUNCTUATION), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_UNIFIED_IDEOGRAPH), default_కలిగియుంది },
    { UPROPS_SRC_CASE,  0, case_ద్విమాన_Property_కలిగియుంది },  // UCHAR_UPPERCASE
    { 1,                U_ముసుగు(UPROPS_WHITE_SPACE), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_XID_CONTINUE), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_XID_ప్రారంభం), default_కలిగియుంది },
    { UPROPS_SRC_CASE,  0, case_ద్విమాన_Property_కలిగియుంది },  // UCHAR_CASE_SENSITIVE
    { 1,                U_ముసుగు(UPROPS_S_పదం), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_VARIATION_SELECTOR), default_కలిగియుంది },
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFD_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKD_INERT
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFC_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKC_INERT
    { UPROPS_SRC_NFC_CANON_ITER, 0, isCanonSegmentStarter },
    { 1,                U_ముసుగు(UPROPS_నమూనా_SYNTAX), default_కలిగియుంది },
    { 1,                U_ముసుగు(UPROPS_నమూనా_WHITE_SPACE), default_కలిగియుంది },
    { UPROPS_SRC_CHAR_AND_PROPSVEC,  0, isPOSIX_alnum },
    { UPROPS_SRC_CHAR,  0, isPOSIX_blank },
    { UPROPS_SRC_CHAR,  0, is_POSIX_లేఖాచిత్రం },
    { UPROPS_SRC_CHAR,  0, isPOSIX_print },
    { UPROPS_SRC_CHAR,  0, isPOSIX_xdigit },
    { UPROPS_SRC_CASE,  0, case_ద్విమాన_Property_కలిగియుంది },  // UCHAR_CASED
    { UPROPS_SRC_CASE,  0, case_ద్విమాన_Property_కలిగియుంది },  // UCHAR_CASE_IGNORABLE
    { UPROPS_SRC_CASE,  0, case_ద్విమాన_Property_కలిగియుంది },  // UCHAR_CHANGES_WHEN_LOWERCASED
    { UPROPS_SRC_CASE,  0, case_ద్విమాన_Property_కలిగియుంది },  // UCHAR_CHANGES_WHEN_UPPERCASED
    { UPROPS_SRC_CASE,  0, case_ద్విమాన_Property_కలిగియుంది },  // UCHAR_CHANGES_WHEN_TITLECASED
    { UPROPS_SRC_CASE_AND_NORM,  0, changesWhenCasefolded },
    { UPROPS_SRC_CASE,  0, case_ద్విమాన_Property_కలిగియుంది },  // UCHAR_CHANGES_WHEN_CASEMAPPED
    { UPROPS_SRC_NFKC_CF, 0, changesWhenNFKC_Casefolded },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_PRESENTATION
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER_BASE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_COMPONENT
    { 2,                0, isRegionalIndicator },
    { 1,                U_ముసుగు(UPROPS_PREPENDED_CONCATENATION_MARK), default_కలిగియుంది },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EXTENDED_PICTOGRAPHIC
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_BASIC_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_KEYCAP_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_MODIFIER_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_FLAG_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_TAG_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_ZWJ_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI
    { UPROPS_SRC_IDSU, 0, isIDSUnaryOperator }, // UCHAR_IDS_UNARY_OPERATOR
    { UPROPS_SRC_ID_COMPAT_MATH, 0, is_IDCompat_Math_ప్రారంభం }, // UCHAR_ID_COMPAT_MATH_START
    { UPROPS_SRC_ID_COMPAT_MATH, 0, isIDCompatMathContinue }, // UCHAR_ID_COMPAT_MATH_CONTINUE
    { UPROPS_SRC_MCM, 0 , isModifierCombiningMark }, // UCHAR_MODIFIER_COMBINING_MARK
};

U_CAPI UBool U_EXPORT2
u_has_ద్విమాన_Property(UChar32 c, UProperty which) {
    /* c is range-checked in the functions that are called from here */
    ఒకవేళ(which<UCHAR_ద్విమాన_ప్రారంభం || UCHAR_ద్విమాన_హద్దు<=which) {
        /* not a known binary property */
        తిరిగి అబద్ధం;
    } లేకపోతే {
        స్థిరరాశి ద్విమాన_Property &prop=binProps[which];
        తిరిగి prop.కలిగియుంది(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    తిరిగి u_has_ద్విమాన_Property(c, UCHAR_ID_ప్రారంభం);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    తిరిగి u_has_ద్విమాన_Property(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_string_Has_ద్విమాన_Property(స్థిరరాశి char16_t *s, int32_t length, UProperty which) {
    ఒకవేళ (s == శూన్యసూచిక && length != 0) { తిరిగి అబద్ధం; }
    ఒకవేళ (length == 1) {
        తిరిగి u_has_ద్విమాన_Property(s[0], which);  // single code point
    } లేకపోతే ఒకవేళ (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_తదుపరి(s, i, length, c);
        ఒకవేళ (length > 0 ? i == length : s[i] == 0) {
            తిరిగి u_has_ద్విమాన_Property(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    తిరిగి UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::has_ద్విమాన_Property(s, length, which);
}

నిర్మాణం IntProperty;

రకనిర్వచనం int32_t Int_Property_Get_విలువ(స్థిరరాశి IntProperty &prop, UChar32 c, UProperty which);
రకనిర్వచనం int32_t Int_Property_Get_Max_విలువ(స్థిరరాశి IntProperty &prop, UProperty which);

నిర్మాణం IntProperty {
    int32_t స్తంభం;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t జరుపు;  // =maxValue if getMaxValueFromShift() is used
    Int_Property_Get_విలువ *get_విలువ;
    Int_Property_Get_Max_విలువ *get_Max_విలువ;
};

స్థిర int32_t default_Get_విలువ(స్థిరరాశి IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    తిరిగి static_cast<int32_t>(u_getUnicodeProperties(c, prop.స్తంభం) & prop.mask) >> prop.జరుపు;
}

స్థిర int32_t default_Get_Max_విలువ(స్థిరరాశి IntProperty &prop, UProperty /*which*/) {
    తిరిగి (uprv_get_Max_విలువలు(prop.స్తంభం)&prop.mask)>>prop.జరుపు;
}

స్థిర int32_t get_Max_విలువ_From_జరుపు(స్థిరరాశి IntProperty &prop, UProperty /*which*/) {
    తిరిగి prop.జరుపు;
}

స్థిర int32_t getBiDiClass(స్థిరరాశి IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి static_cast<int32_t>(u_charDirection(c));
}

స్థిర int32_t getBiDiPairedBracketType(స్థిరరాశి IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

స్థిర int32_t bi_Di_Get_Max_విలువ(స్థిరరాశి IntProperty &/*prop*/, UProperty which) {
    తిరిగి ubidi_get_Max_విలువ(which);
}

స్థిర int32_t get_ఖండం(స్థిరరాశి IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి static_cast<int32_t>(ublock_get_సంకేతం(c));
}

స్థిర int32_t ఖండం_Get_Max_విలువ(స్థిరరాశి IntProperty &/*prop*/, UProperty /*which*/) {
    తిరిగి uprv_get_Max_విలువలు(UPROPS_MAX_విలువలు_OTHER_సూచీ) & UPROPS_MAX_ఖండం;
}

#ఒకవేళ UCONFIG_NO_NORMALIZATION
స్థిర int32_t getCombiningClass(స్థిరరాశి IntProperty &, UChar32, UProperty) {
    తిరిగి 0;
}
#లేకపోతే
స్థిర int32_t getCombiningClass(స్థిరరాశి IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి u_getCombiningClass(c);
}
#అయితే_ముగింపు

స్థిర int32_t getGeneralCategory(స్థిరరాశి IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి static_cast<int32_t>(u_charType(c));
}

స్థిర int32_t get_Joining_సమూహం(స్థిరరాశి IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి ubidi_get_Joining_సమూహం(c);
}

స్థిర int32_t getJoiningType(స్థిరరాశి IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి ubidi_getJoiningType(c);
}

స్థిర int32_t getNumericType(స్థిరరాశి IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_విలువ(u_getMainProperties(c)));
    తిరిగి UPROPS_NTV_GET_TYPE(ntv);
}

స్థిర int32_t getScript(స్థిరరాశి IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    తిరిగి static_cast<int32_t>(uscript_getScript(c, &error_సంకేతం));
}

స్థిర int32_t script_Get_Max_విలువ(స్థిరరాశి IntProperty &/*prop*/, UProperty /*which*/) {
    తిరిగి uprv_get_Max_విలువలు(0)&UPROPS_MAX_SCRIPT;
}

/*
 * Map some of the Grapheme Cluster Break values to Hangul Syllable Types.
 * Hangul_Syllable_Type used to be fully redundant with a subset of Grapheme_Cluster_Break.
 *
 * Starting with Unicode 16, this is no longer true for HST=V vs. GCB=V in some cases:
 * Some Kirat Rai vowels are given GCB=V for proper grapheme clustering, but
 * they are of course not related to Hangul syllables.
 */
స్థిర స్థిరరాశి UHangulSyllableType gcb_గమ్యం_Hst[]={
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

స్థిర int32_t getHangulSyllableType(స్థిరరాశి IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Ignore supplementary code points: They all have HST=NA.
    // This is a simple way to handle the GCB!=hst cases since Unicode 16 (Kirat Rai vowels).
    ఒకవేళ(c>0xffff) {
        తిరిగి U_HST_NOT_APPLICABLE;
    }
    /* see comments on gcbToHst[] above */
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_ముసుగు) >> UPROPS_GCB_జరుపు;
    ఒకవేళ(gcb<UPRV_LENGTHOF(gcb_గమ్యం_Hst)) {
        తిరిగి gcb_గమ్యం_Hst[gcb];
    } లేకపోతే {
        తిరిగి U_HST_NOT_APPLICABLE;
    }
}

#ఒకవేళ UCONFIG_NO_NORMALIZATION
స్థిర int32_t get_Norm_శీఘ్ర_Check(స్థిరరాశి IntProperty &, UChar32, UProperty) {
    తిరిగి 0;
}
#లేకపోతే
స్థిర int32_t get_Norm_శీఘ్ర_Check(స్థిరరాశి IntProperty &/*prop*/, UChar32 c, UProperty which) {
    తిరిగి static_cast<int32_t>(unorm_get_శీఘ్ర_Check(c, static_cast<UNormalization_బహుళకం>(which - UCHAR_NFD_శీఘ్ర_CHECK + UNORM_NFD)));
}
#అయితే_ముగింపు

#ఒకవేళ UCONFIG_NO_NORMALIZATION
స్థిర int32_t getLeadCombiningClass(స్థిరరాశి IntProperty &, UChar32, UProperty) {
    తిరిగి 0;
}
#లేకపోతే
స్థిర int32_t getLeadCombiningClass(స్థిరరాశి IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి unorm_getFCD16(c)>>8;
}
#అయితే_ముగింపు

#ఒకవేళ UCONFIG_NO_NORMALIZATION
స్థిర int32_t getTrailCombiningClass(స్థిరరాశి IntProperty &, UChar32, UProperty) {
    తిరిగి 0;
}
#లేకపోతే
స్థిర int32_t getTrailCombiningClass(స్థిరరాశి IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    తిరిగి unorm_getFCD16(c)&0xff;
}
#అయితే_ముగింపు

స్థిర int32_t get_లోపల_PC(స్థిరరాశి IntProperty &, UChar32 c, UProperty) {
    తిరిగి ulayout_ensure_దత్తాంశం() && gInpcTrie != శూన్యసూచిక ? ucptrie_get(gInpcTrie, c) : 0;
}

స్థిర int32_t get_లోపల_SC(స్థిరరాశి IntProperty &, UChar32 c, UProperty) {
    తిరిగి ulayout_ensure_దత్తాంశం() && gInscTrie != శూన్యసూచిక ? ucptrie_get(gInscTrie, c) : 0;
}

స్థిర int32_t getVo(స్థిరరాశి IntProperty &, UChar32 c, UProperty) {
    తిరిగి ulayout_ensure_దత్తాంశం() && gVoTrie != శూన్యసూచిక ? ucptrie_get(gVoTrie, c) : 0;
}

స్థిర int32_t layout_Get_Max_విలువ(స్థిరరాశి IntProperty &/*prop*/, UProperty which) {
    ఒకవేళ (!ulayout_ensure_దత్తాంశం()) { తిరిగి 0; }
    ఎంపిక (which) {
    స్థితి UCHAR_INDIC_POSITIONAL_CATEGORY:
        తిరిగి g_Max_Inpc_విలువ;
    స్థితి UCHAR_INDIC_SYLLABIC_CATEGORY:
        తిరిగి g_Max_Insc_విలువ;
    స్థితి UCHAR_VERTICAL_ORIENTATION:
        తిరిగి g_Max_Vo_విలువ;
    అప్రమేయ:
        తిరిగి 0;
    }
}

స్థిర int32_t get_IDStatus_విలువ(స్థిరరాశి IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_జరుపు;
    తిరిగి value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_స్థితి_ALLOWED : U_ID_స్థితి_RESTRICTED;
}

స్థిర స్థిరరాశి IntProperty intProps[UCHAR_INT_హద్దు-UCHAR_INT_ప్రారంభం]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, bi_Di_Get_Max_విలువ },
    { UPROPS_SRC_ఖండం, 0, 0,                               get_ఖండం, ఖండం_Get_Max_విలువ },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, get_Max_విలువ_From_జరుపు },
    { 2,                UPROPS_DT_ముసుగు, 0,                  default_Get_విలువ, default_Get_Max_విలువ },
    { 0,                UPROPS_EA_ముసుగు, UPROPS_EA_జరుపు,    default_Get_విలువ, default_Get_Max_విలువ },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_లెక్క) - 1, getGeneralCategory, get_Max_విలువ_From_జరుపు },
    { UPROPS_SRC_BIDI,  0, 0,                               get_Joining_సమూహం, bi_Di_Get_Max_విలువ },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, bi_Di_Get_Max_విలువ },
    { 2,                UPROPS_LB_ముసుగు, UPROPS_LB_జరుపు,    default_Get_విలువ, default_Get_Max_విలువ },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_లెక్క) - 1, getNumericType, get_Max_విలువ_From_జరుపు },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, script_Get_Max_విలువ },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_లెక్క) - 1, getHangulSyllableType, get_Max_విలువ_From_జరుపు },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), get_Norm_శీఘ్ర_Check, get_Max_విలువ_From_జరుపు },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), get_Norm_శీఘ్ర_Check, get_Max_విలువ_From_జరుపు },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), get_Norm_శీఘ్ర_Check, get_Max_విలువ_From_జరుపు },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), get_Norm_శీఘ్ర_Check, get_Max_విలువ_From_జరుపు },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, get_Max_విలువ_From_జరుపు },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, get_Max_విలువ_From_జరుపు },
    { 2,                UPROPS_GCB_ముసుగు, UPROPS_GCB_జరుపు,  default_Get_విలువ, default_Get_Max_విలువ },
    { 2,                UPROPS_SB_ముసుగు, UPROPS_SB_జరుపు,    default_Get_విలువ, default_Get_Max_విలువ },
    { 2,                UPROPS_WB_ముసుగు, UPROPS_WB_జరుపు,    default_Get_విలువ, default_Get_Max_విలువ },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, bi_Di_Get_Max_విలువ },
    { UPROPS_SRC_INPC,  0, 0,                               get_లోపల_PC, layout_Get_Max_విలువ },
    { UPROPS_SRC_INSC,  0, 0,                               get_లోపల_SC, layout_Get_Max_విలువ },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layout_Get_Max_విలువ },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_స్థితి_ALLOWED), get_IDStatus_విలువ, get_Max_విలువ_From_జరుపు },
    { 0,                UPROPS_INCB_ముసుగు, UPROPS_INCB_జరుపు,default_Get_విలువ, default_Get_Max_విలువ },
};

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_విలువ(UChar32 c, UProperty which) {
    ఒకవేళ(which<UCHAR_INT_ప్రారంభం) {
        ఒకవేళ(UCHAR_ద్విమాన_ప్రారంభం<=which && which<UCHAR_ద్విమాన_హద్దు) {
            స్థిరరాశి ద్విమాన_Property &prop=binProps[which];
            తిరిగి prop.కలిగియుంది(prop, c, which);
        }
    } లేకపోతే ఒకవేళ(which<UCHAR_INT_హద్దు) {
        స్థిరరాశి IntProperty &prop=intProps[which-UCHAR_INT_ప్రారంభం];
        తిరిగి prop.get_విలువ(prop, c, which);
    } లేకపోతే ఒకవేళ(which==UCHAR_GENERAL_CATEGORY_ముసుగు) {
        తిరిగి U_ముసుగు(u_charType(c));
    }
    తిరిగి 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Min_విలువ(UProperty /*which*/) {
    తిరిగి 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Max_విలువ(UProperty which) {
    ఒకవేళ(which<UCHAR_INT_ప్రారంభం) {
        ఒకవేళ(UCHAR_ద్విమాన_ప్రారంభం<=which && which<UCHAR_ద్విమాన_హద్దు) {
            తిరిగి 1;  // maximum true for all binary properties
        }
    } లేకపోతే ఒకవేళ(which<UCHAR_INT_హద్దు) {
        స్థిరరాశి IntProperty &prop=intProps[which-UCHAR_INT_ప్రారంభం];
        తిరిగి prop.get_Max_విలువ(prop, which);
    }
    తిరిగి -1;  // undefined
}

U_CFUNC UProperty_ఆధారం U_EXPORT2
uprops_get_ఆధారం(UProperty which) {
    ఒకవేళ(which<UCHAR_ద్విమాన_ప్రారంభం) {
        తిరిగి UPROPS_SRC_NONE; /* undefined */
    } లేకపోతే ఒకవేళ(which<UCHAR_ద్విమాన_హద్దు) {
        స్థిరరాశి ద్విమాన_Property &prop=binProps[which];
        ఒకవేళ(prop.mask!=0) {
            తిరిగి UPROPS_SRC_PROPSVEC;
        } లేకపోతే {
            తిరిగి (UProperty_ఆధారం)prop.స్తంభం;
        }
    } లేకపోతే ఒకవేళ(which<UCHAR_INT_ప్రారంభం) {
        తిరిగి UPROPS_SRC_NONE; /* undefined */
    } లేకపోతే ఒకవేళ(which<UCHAR_INT_హద్దు) {
        స్థిరరాశి IntProperty &prop=intProps[which-UCHAR_INT_ప్రారంభం];
        ఒకవేళ(prop.mask!=0) {
            తిరిగి UPROPS_SRC_PROPSVEC;
        } లేకపోతే {
            తిరిగి (UProperty_ఆధారం)prop.స్తంభం;
        }
    } లేకపోతే ఒకవేళ(which<UCHAR_STRING_ప్రారంభం) {
        ఎంపిక(which) {
        స్థితి UCHAR_GENERAL_CATEGORY_ముసుగు:
        స్థితి UCHAR_NUMERIC_విలువ:
            తిరిగి UPROPS_SRC_CHAR;

        అప్రమేయ:
            తిరిగి UPROPS_SRC_NONE;
        }
    } లేకపోతే ఒకవేళ(which<UCHAR_STRING_హద్దు) {
        ఎంపిక(which) {
        స్థితి UCHAR_AGE:
            తిరిగి UPROPS_SRC_PROPSVEC;

        స్థితి UCHAR_BIDI_MIRRORING_GLYPH:
            తిరిగి UPROPS_SRC_BIDI;

        స్థితి UCHAR_CASE_FOLDING:
        స్థితి UCHAR_LOWERCASE_MAPPING:
        స్థితి UCHAR_SIMPLE_CASE_FOLDING:
        స్థితి UCHAR_SIMPLE_LOWERCASE_MAPPING:
        స్థితి UCHAR_SIMPLE_TITLECASE_MAPPING:
        స్థితి UCHAR_SIMPLE_UPPERCASE_MAPPING:
        స్థితి UCHAR_TITLECASE_MAPPING:
        స్థితి UCHAR_UPPERCASE_MAPPING:
            తిరిగి UPROPS_SRC_CASE;

        స్థితి UCHAR_ISO_COMMENT:
        స్థితి UCHAR_NAME:
        స్థితి UCHAR_UNICODE_1_NAME:
            తిరిగి UPROPS_SRC_పేర్లు;

        అప్రమేయ:
            తిరిగి UPROPS_SRC_NONE;
        }
    } లేకపోతే {
        ఎంపిక(which) {
        స్థితి UCHAR_SCRIPT_EXTENSIONS:
        స్థితి UCHAR_IDENTIFIER_TYPE:
            తిరిగి UPROPS_SRC_PROPSVEC;
        అప్రమేయ:
            తిరిగి UPROPS_SRC_NONE; /* undefined */
        }
    }
}

U_CFUNC శూన్య U_EXPORT2
uprops_కూడు_Property_Starts(UProperty_ఆధారం src, స్థిరరాశి USetAdder *sa, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) { తిరిగి; }
    ఒకవేళ (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        కొరకు (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->కూడు(sa->సమితి, c);
        }
        // single characters
        కొరకు (UChar32 c : ID_COMPAT_MATH_ప్రారంభం) {
            sa->కూడు(sa->సమితి, c);
            sa->కూడు(sa->సమితి, c + 1);
        }
        తిరిగి;
    }
    ఒకవేళ (src == UPROPS_SRC_MCM) {
        // range limits
        కొరకు (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->కూడు(sa->సమితి, c);
        }
        తిరిగి;
    }
    ఒకవేళ (!ulayout_ensure_దత్తాంశం(*p_Error_సంకేతం)) { తిరిగి; }
    స్థిరరాశి UCPTrie *trie;
    ఎంపిక (src) {
    స్థితి UPROPS_SRC_INPC:
        trie = gInpcTrie;
        ఆపు;
    స్థితి UPROPS_SRC_INSC:
        trie = gInscTrie;
        ఆపు;
    స్థితి UPROPS_SRC_VO:
        trie = gVoTrie;
        ఆపు;
    అప్రమేయ:
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    ఒకవేళ (trie == శూన్యసూచిక) {
        *p_Error_సంకేతం = U_MISSING_RESOURCE_ERROR;
        తిరిగి;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 ప్రారంభం = 0, చివర;
    అయితే ((చివర = ucptrie_getRange(trie, ప్రారంభం, UCPMAP_RANGE_సాధారణ, 0,
                                   శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక)) >= 0) {
        sa->కూడు(sa->సమితి, ప్రారంభం);
        ప్రారంభం = చివర + 1;
    }
}

U_CAPI తార్కిక U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t type_సూచీ = type;  // also guards against negative type integers
    ఒకవేళ (type_సూచీ >= UPRV_LENGTHOF(uprops_id_Type_గమ్యం_Encoded)) {
        తిరిగి అబద్ధం;
    }
    uint32_t encodedType = uprops_id_Type_గమ్యం_Encoded[type_సూచీ];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_జరుపు;
    ఒకవేళ ((encodedType & UPROPS_ID_TYPE_బిట్టు) != 0) {
        తిరిగి value < UPROPS_ID_TYPE_FORBIDDEN && (value & encodedType) != 0;
    } లేకపోతే {
        తిరిగి value == encodedType;
    }
}

నామాకాశ {

శూన్య maybeAppendType(uint32_t value, uint32_t bit, UIdentifierType t,
                     UIdentifierType *types, int32_t &length, int32_t capacity) {
    ఒకవేళ ((value & bit) != 0) {
        ఒకవేళ (length < capacity) {
            types[length] = t;
        }
        ++length;
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) { తిరిగి 0; }
    ఒకవేళ (capacity < 0 || (capacity > 0 && types == శూన్యసూచిక)) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_జరుపు;
    ఒకవేళ ((value & UPROPS_ID_TYPE_FORBIDDEN) == UPROPS_ID_TYPE_FORBIDDEN ||
            value == UPROPS_ID_TYPE_NOT_CHARACTER) {
        // single value
        ఒకవేళ (capacity > 0) {
            UIdentifierType t;
            ఎంపిక (value) {
                స్థితి UPROPS_ID_TYPE_NOT_CHARACTER: t = U_ID_TYPE_NOT_CHARACTER; ఆపు;
                స్థితి UPROPS_ID_TYPE_DEPRECATED: t = U_ID_TYPE_DEPRECATED; ఆపు;
                స్థితి UPROPS_ID_TYPE_DEFAULT_IGNORABLE: t = U_ID_TYPE_DEFAULT_IGNORABLE; ఆపు;
                స్థితి UPROPS_ID_TYPE_NOT_NFKC: t = U_ID_TYPE_NOT_NFKC; ఆపు;
                స్థితి UPROPS_ID_TYPE_INCLUSION: t = U_ID_TYPE_INCLUSION; ఆపు;
                స్థితి UPROPS_ID_TYPE_RECOMMENDED: t = U_ID_TYPE_RECOMMENDED; ఆపు;
                అప్రమేయ:
                    *p_Error_సంకేతం = U_INVALID_FORMAT_ERROR;
                    తిరిగి 0;
            }
            types[0] = t;
        } లేకపోతే {
            *p_Error_సంకేతం = U_బఫరు_అధికప్రవాహం_ERROR;
        }
        తిరిగి 1;
    } లేకపోతే {
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
        ఒకవేళ (length >= capacity) {
            *p_Error_సంకేతం = U_బఫరు_అధికప్రవాహం_ERROR;
        }
        తిరిగి length;
    }
}

#ఒకవేళ !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_get_FC_NFKC_సంవృతం(UChar32 c, char16_t *dest, int32_t dest_సామర్థ్యం, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(dest_సామర్థ్యం<0 || (dest==శూన్యసూచిక && dest_సామర్థ్యం>0)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    స్థిరరాశి Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    స్థిరరాశి char16_t *folded1;
    int32_t folded1_నిడివి=ucase_గమ్యం_నిండిన_Folding(c, &folded1, U_FOLD_CASE_DEFAULT);
    ఒకవేళ(folded1_నిడివి<0) {
        స్థిరరాశి Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        ఒకవేళ(nfkcImpl->get_Comp_శీఘ్ర_Check(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, 0, p_Error_సంకేతం);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.సమితి_గమ్యం(c);
    } లేకపోతే {
        ఒకవేళ(folded1_నిడివి>UCASE_MAX_STRING_నిడివి) {
            folded1String.సమితి_గమ్యం(folded1_నిడివి);
        } లేకపోతే {
            folded1String.సమితి_గమ్యం(అబద్ధం, folded1, folded1_నిడివి);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *p_Error_సంకేతం);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *p_Error_సంకేతం);
    // if (c != b) add the mapping from a to c
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం) || kc1==kc2) {
        తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, 0, p_Error_సంకేతం);
    } లేకపోతే {
        తిరిగి kc2.extract(dest, dest_సామర్థ్యం, *p_Error_సంకేతం);
    }
}

#అయితే_ముగింపు
