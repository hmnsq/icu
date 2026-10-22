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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/uchar.h"
#অন্তর্ভাৱ "unicode/ucptrie.h"
#অন্তর্ভাৱ "unicode/udata.h"
#অন্তর্ভাৱ "unicode/unorm2.h"
#অন্তর্ভাৱ "unicode/uscript.h"
#অন্তর্ভাৱ "unicode/ustring.h"
#অন্তর্ভাৱ "unicode/utf16.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "emojiprops.h"
#অন্তর্ভাৱ "mutex.h"
#অন্তর্ভাৱ "normalizer2impl.h"
#অন্তর্ভাৱ "umutex.h"
#অন্তর্ভাৱ "ubidi_props.h"
#অন্তর্ভাৱ "uprops.h"
#অন্তর্ভাৱ "ucase.h"
#অন্তর্ভাৱ "ucln_cmn.h"
#অন্তর্ভাৱ "ulayout_props.h"
#অন্তর্ভাৱ "ustr_imp.h"

U_NAMESPACE_USE

// Unicode text layout properties data -----------------------------------------

নামাকাশ {

icu::UInitOnce gLayoutInitOnce {};
UDataMemory *gLayoutMemory = nullptr;

UCPTrie *gInpcTrie = nullptr;  // Indic_Positional_Category
UCPTrie *gInscTrie = nullptr;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = nullptr;  // Vertical_Orientation

int32_t g_Max_Inpc_মান = 0;
int32_t g_Max_Insc_মান = 0;
int32_t g_Max_Vo_মান = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(gLayoutMemory);
    gLayoutMemory = nullptr;

    ucptrie_close(gInpcTrie);
    gInpcTrie = nullptr;
    ucptrie_close(gInscTrie);
    gInscTrie = nullptr;
    ucptrie_close(gVoTrie);
    gVoTrie = nullptr;

    g_Max_Inpc_মান = 0;
    g_Max_Insc_মান = 0;
    g_Max_Vo_মান = 0;

    gLayoutInitOnce.reset();
    ঘূৰাই সঁচা;
}

UBool U_CALLCONV
ulayout_isAcceptable(শূন্য * /*context*/,
                     ধ্রুৱ ৱর্ণ * /* type */, ধ্রুৱ ৱর্ণ * /*name*/,
                     ধ্রুৱ UDataInfo *pInfo) {
    ঘূৰাই pInfo->size >= 20 &&
        pInfo->isBigEndian == U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily == U_CHARSET_FAMILY &&
        pInfo->dataFormat[0] == ULAYOUT_FMT_0 &&
        pInfo->dataFormat[1] == ULAYOUT_FMT_1 &&
        pInfo->dataFormat[2] == ULAYOUT_FMT_2 &&
        pInfo->dataFormat[3] == ULAYOUT_FMT_3 &&
        pInfo->formatVersion[0] == 1;
}

// UInitOnce singleton initialization function
শূন্য U_CALLCONV ulayout_load(UErrorCode &errorCode) {
    gLayoutMemory = udata_openChoice(
        nullptr, ULAYOUT_DATA_TYPE, ULAYOUT_DATA_NAME,
        ulayout_isAcceptable, nullptr, &errorCode);
    যদি (U_FAILURE(errorCode)) { ঘূৰাই; }

    ধ্রুৱ uint8_t* inBytes = static_cast<ধ্রুৱ uint8_t*>(udata_getMemory(gLayoutMemory));
    ধ্রুৱ int32_t* inIndexes = reinterpret_cast<ধ্রুৱ int32_t*>(inBytes);
    int32_t indexesLength = inIndexes[ULAYOUT_IX_INDEXES_LENGTH];
    যদি (indexesLength < 12) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ঘূৰাই;
    }
    int32_t offset = indexesLength * 4;
    int32_t শীৰ্ষ = inIndexes[ULAYOUT_IX_INPC_TRIE_শীৰ্ষ];
    int32_t trieSize = শীৰ্ষ - offset;
    যদি (trieSize >= 16) {
        gInpcTrie = ucptrie_open_From_দ্বিমিক(
            UCPTRIE_TYPE_ANY, UCPTRIE_মান_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }
    offset = শীৰ্ষ;
    শীৰ্ষ = inIndexes[ULAYOUT_IX_INSC_TRIE_শীৰ্ষ];
    trieSize = শীৰ্ষ - offset;
    যদি (trieSize >= 16) {
        gInscTrie = ucptrie_open_From_দ্বিমিক(
            UCPTRIE_TYPE_ANY, UCPTRIE_মান_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }
    offset = শীৰ্ষ;
    শীৰ্ষ = inIndexes[ULAYOUT_IX_VO_TRIE_শীৰ্ষ];
    trieSize = শীৰ্ষ - offset;
    যদি (trieSize >= 16) {
        gVoTrie = ucptrie_open_From_দ্বিমিক(
            UCPTRIE_TYPE_ANY, UCPTRIE_মান_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }

    uint32_t max_মানসমূহ = inIndexes[ULAYOUT_IX_MAX_মানসমূহ];
    g_Max_Inpc_মান = max_মানসমূহ >> ULAYOUT_MAX_INPC_SHIFT;
    g_Max_Insc_মান = (max_মানসমূহ >> ULAYOUT_MAX_INSC_SHIFT) & 0xff;
    g_Max_Vo_মান = (max_মানসমূহ >> ULAYOUT_MAX_VO_SHIFT) & 0xff;

    ucln_common_registerCleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensureData(UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই মিছা; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_load, errorCode);
    ঘূৰাই U_SUCCESS(errorCode);
}

UBool ulayout_ensureData() {
    UErrorCode errorCode = U_ZERO_ERROR;
    ঘূৰাই ulayout_ensureData(errorCode);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

সংরচনা দ্বিমিক_Property;

প্রকারনির্দেশ UBool দ্বিমিক_Property_Contains(ধ্রুৱ দ্বিমিক_Property &prop, UChar32 c, UProperty which);

সংরচনা দ্বিমিক_Property {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    দ্বিমিক_Property_Contains *contains;
};

স্থির UBool defaultContains(ধ্রুৱ দ্বিমিক_Property &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    ঘূৰাই (u_getUnicodeProperties(c, prop.column)&prop.mask)!=0;
}

স্থির UBool case_দ্বিমিক_Property_Contains(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty which) {
    ঘূৰাই ucase_has_দ্বিমিক_Property(c, which);
}

স্থির UBool isBidiControl(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই ubidi_isBidiControl(c);
}

স্থির UBool isMirrored(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই ubidi_isMirrored(c);
}

স্থির UBool isJoinControl(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই ubidi_isJoinControl(c);
}

#যদি UCONFIG_NO_NORMALIZATION
স্থির UBool hasFullCompositionExclusion(ধ্রুৱ দ্বিমিক_Property &, UChar32, UProperty) {
    ঘূৰাই মিছা;
}
#অন্যথা
স্থির UBool hasFullCompositionExclusion(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UErrorCode errorCode=U_ZERO_ERROR;
    ধ্রুৱ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    ঘূৰাই U_SUCCESS(errorCode) && impl->isCompNo(impl->getNorm16(c));
}
#endif

// UCHAR_NF*_INERT properties
#যদি UCONFIG_NO_NORMALIZATION
স্থির UBool isNormInert(ধ্রুৱ দ্বিমিক_Property &, UChar32, UProperty) {
    ঘূৰাই মিছা;
}
#অন্যথা
স্থির UBool isNormInert(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty which) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ধ্রুৱ Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalizationMode>(which - UCHAR_NFD_INERT + UNORM_NFD), errorCode);
    ঘূৰাই U_SUCCESS(errorCode) && norm2->isInert(c);
}
#endif

#যদি UCONFIG_NO_NORMALIZATION
স্থির UBool changesWhenCasefolded(ধ্রুৱ দ্বিমিক_Property &, UChar32, UProperty) {
    ঘূৰাই মিছা;
}
#অন্যথা
স্থির UBool changesWhenCasefolded(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UErrorCode errorCode=U_ZERO_ERROR;
    ধ্রুৱ Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(errorCode);
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই মিছা;
    }
    যদি(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        যদি(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } অন্যথা যদি(nfd.length()<=U16_MAX_LENGTH &&
                  nfd.length()==U16_LENGTH(c=nfd.char32At(0))
        ) {
            /* single supplementary code point */
        } অন্যথা {
            c=U_SENTINEL;
        }
    } অন্যথা যদি(c<0) {
        ঘূৰাই মিছা;  /* protect against bad input */
    }
    যদি(c>=0) {
        /* single code point */
        ধ্রুৱ char16_t *resultString;
        ঘূৰাই ucase_toFullFolding(c, &resultString, U_FOLD_CASE_DEFAULT) >= 0;
    } অন্যথা {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_LENGTH];
        int32_t destLength;
        destLength=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.getBuffer(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &errorCode);
        ঘূৰাই U_SUCCESS(errorCode) &&
                       0!=u_strCompare(nfd.getBuffer(), nfd.length(),
                                       dest, destLength, মিছা);
    }
}
#endif

#যদি UCONFIG_NO_NORMALIZATION
স্থির UBool changesWhenNFKC_Casefolded(ধ্রুৱ দ্বিমিক_Property &, UChar32, UProperty) {
    ঘূৰাই মিছা;
}
#অন্যথা
স্থির UBool changesWhenNFKC_Casefolded(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ধ্রুৱ Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(errorCode);
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই মিছা;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        ReorderingBuffer buffer(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        যদি(buffer.init(5, errorCode)) {
            ধ্রুৱ char16_t *srcArray=src.getBuffer();
            kcf->compose(srcArray, srcArray+src.length(), মিছা,
                          সঁচা, buffer, errorCode);
        }
    }
    ঘূৰাই U_SUCCESS(errorCode) && dest!=src;
}
#endif

#যদি UCONFIG_NO_NORMALIZATION
স্থির UBool isCanonSegmentStarter(ধ্রুৱ দ্বিমিক_Property &, UChar32, UProperty) {
    ঘূৰাই মিছা;
}
#অন্যথা
স্থির UBool isCanonSegmentStarter(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ধ্রুৱ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    ঘূৰাই
        U_SUCCESS(errorCode) && impl->ensureCanonIterData(errorCode) &&
        impl->isCanonSegmentStarter(c);
}
#endif

স্থির UBool isPOSIX_alnum(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই u_isalnumPOSIX(c);
}

স্থির UBool isPOSIX_blank(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই u_isblank(c);
}

স্থির UBool isPOSIX_graph(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই u_isgraphPOSIX(c);
}

স্থির UBool isPOSIX_print(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই u_isprintPOSIX(c);
}

স্থির UBool isPOSIX_xdigit(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই u_isxdigit(c);
}

স্থির UBool isRegionalIndicator(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    ঘূৰাই 0x1F1E6<=c && c<=0x1F1FF;
}

স্থির UBool hasEmojiProperty(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty which) {
    ঘূৰাই EmojiProps::has_দ্বিমিক_Property(c, which);
}

স্থির UBool isIDSUnaryOperator(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // New in Unicode 15.1 for just two characters.
    ঘূৰাই 0x2FFE<=c && c<=0x2FFF;
}

/** Ranges (start/limit pairs) of ID_Compat_Math_Continue (only), from UCD PropList.txt. */
স্থির constexpr UChar32 ID_COMPAT_MATH_CONTINUE[] = {
    0x00B2, 0x00B3 + 1,
    0x00B9, 0x00B9 + 1,
    0x2070, 0x2070 + 1,
    0x2074, 0x207E + 1,
    0x2080, 0x208E + 1
};

/** ID_Compat_Math_Start characters, from UCD PropList.txt. */
স্থির constexpr UChar32 ID_COMPAT_MATH_START[] = {
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
স্থির constexpr UChar32 MODIFIER_COMBINING_MARK[] = {
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

স্থির UBool isIDCompatMathStart(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    যদি (c < ID_COMPAT_MATH_START[0]) { ঘূৰাই মিছা; }  // fastpath for common scripts
    বাবে (UChar32 startChar : ID_COMPAT_MATH_START) {
        যদি (c == startChar) { ঘূৰাই সঁচা; }
    }
    ঘূৰাই মিছা;
}

স্থির UBool isIDCompatMathContinue(ধ্রুৱ দ্বিমিক_Property &prop, UChar32 c, UProperty /*which*/) {
    বাবে (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        যদি (c < ID_COMPAT_MATH_CONTINUE[i]) { ঘূৰাই মিছা; }  // below range start
        যদি (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { ঘূৰাই সঁচা; }  // below range limit
    }
    ঘূৰাই isIDCompatMathStart(prop, c, UCHAR_ID_COMPAT_MATH_START);
}

স্থির UBool isModifierCombiningMark(ধ্রুৱ দ্বিমিক_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    বাবে (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        যদি (c < MODIFIER_COMBINING_MARK[i]) { ঘূৰাই মিছা; }  // below range start
        যদি (c < MODIFIER_COMBINING_MARK[i + 1]) { ঘূৰাই সঁচা; }  // below range limit
    }
    ঘূৰাই মিছা;
}

স্থির ধ্রুৱ দ্বিমিক_Property binProps[UCHAR_দ্বিমিক_LIMIT]={
    /*
     * column and mask values for binary properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per binary UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { 1,                U_MASK(UPROPS_ALPHABETIC), defaultContains },
    { 1,                U_MASK(UPROPS_ASCII_HEX_DIGIT), defaultContains },
    { UPROPS_SRC_BIDI,  0, isBidiControl },
    { UPROPS_SRC_BIDI,  0, isMirrored },
    { 1,                U_MASK(UPROPS_DASH), defaultContains },
    { 1,                U_MASK(UPROPS_DEFAULT_IGNORABLE_CODE_POINT), defaultContains },
    { 1,                U_MASK(UPROPS_DEPRECATED), defaultContains },
    { 1,                U_MASK(UPROPS_DIACRITIC), defaultContains },
    { 1,                U_MASK(UPROPS_EXTENDER), defaultContains },
    { UPROPS_SRC_NFC,   0, hasFullCompositionExclusion },
    { 1,                U_MASK(UPROPS_GRAPHEME_BASE), defaultContains },
    { 1,                U_MASK(UPROPS_GRAPHEME_EXTEND), defaultContains },
    { 1,                U_MASK(UPROPS_GRAPHEME_LINK), defaultContains },
    { 1,                U_MASK(UPROPS_HEX_DIGIT), defaultContains },
    { 1,                U_MASK(UPROPS_HYPHEN), defaultContains },
    { 1,                U_MASK(UPROPS_ID_CONTINUE), defaultContains },
    { 1,                U_MASK(UPROPS_ID_START), defaultContains },
    { 1,                U_MASK(UPROPS_IDEOGRAPHIC), defaultContains },
    { 1,                U_MASK(UPROPS_IDS_দ্বিমিক_OPERATOR), defaultContains },
    { 1,                U_MASK(UPROPS_IDS_TRINARY_OPERATOR), defaultContains },
    { UPROPS_SRC_BIDI,  0, isJoinControl },
    { 1,                U_MASK(UPROPS_LOGICAL_ORDER_EXCEPTION), defaultContains },
    { UPROPS_SRC_CASE,  0, case_দ্বিমিক_Property_Contains },  // UCHAR_LOWERCASE
    { 1,                U_MASK(UPROPS_MATH), defaultContains },
    { 1,                U_MASK(UPROPS_NONCHARACTER_CODE_POINT), defaultContains },
    { 1,                U_MASK(UPROPS_QUOTATION_MARK), defaultContains },
    { 1,                U_MASK(UPROPS_RADICAL), defaultContains },
    { UPROPS_SRC_CASE,  0, case_দ্বিমিক_Property_Contains },  // UCHAR_SOFT_DOTTED
    { 1,                U_MASK(UPROPS_TERMINAL_PUNCTUATION), defaultContains },
    { 1,                U_MASK(UPROPS_UNIFIED_IDEOGRAPH), defaultContains },
    { UPROPS_SRC_CASE,  0, case_দ্বিমিক_Property_Contains },  // UCHAR_UPPERCASE
    { 1,                U_MASK(UPROPS_WHITE_SPACE), defaultContains },
    { 1,                U_MASK(UPROPS_XID_CONTINUE), defaultContains },
    { 1,                U_MASK(UPROPS_XID_START), defaultContains },
    { UPROPS_SRC_CASE,  0, case_দ্বিমিক_Property_Contains },  // UCHAR_CASE_SENSITIVE
    { 1,                U_MASK(UPROPS_S_TERM), defaultContains },
    { 1,                U_MASK(UPROPS_VARIATION_SELECTOR), defaultContains },
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFD_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKD_INERT
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFC_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKC_INERT
    { UPROPS_SRC_NFC_CANON_ITER, 0, isCanonSegmentStarter },
    { 1,                U_MASK(UPROPS_PATTERN_SYNTAX), defaultContains },
    { 1,                U_MASK(UPROPS_PATTERN_WHITE_SPACE), defaultContains },
    { UPROPS_SRC_CHAR_AND_PROPSVEC,  0, isPOSIX_alnum },
    { UPROPS_SRC_CHAR,  0, isPOSIX_blank },
    { UPROPS_SRC_CHAR,  0, isPOSIX_graph },
    { UPROPS_SRC_CHAR,  0, isPOSIX_print },
    { UPROPS_SRC_CHAR,  0, isPOSIX_xdigit },
    { UPROPS_SRC_CASE,  0, case_দ্বিমিক_Property_Contains },  // UCHAR_CASED
    { UPROPS_SRC_CASE,  0, case_দ্বিমিক_Property_Contains },  // UCHAR_CASE_IGNORABLE
    { UPROPS_SRC_CASE,  0, case_দ্বিমিক_Property_Contains },  // UCHAR_CHANGES_WHEN_LOWERCASED
    { UPROPS_SRC_CASE,  0, case_দ্বিমিক_Property_Contains },  // UCHAR_CHANGES_WHEN_UPPERCASED
    { UPROPS_SRC_CASE,  0, case_দ্বিমিক_Property_Contains },  // UCHAR_CHANGES_WHEN_TITLECASED
    { UPROPS_SRC_CASE_AND_NORM,  0, changesWhenCasefolded },
    { UPROPS_SRC_CASE,  0, case_দ্বিমিক_Property_Contains },  // UCHAR_CHANGES_WHEN_CASEMAPPED
    { UPROPS_SRC_NFKC_CF, 0, changesWhenNFKC_Casefolded },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_PRESENTATION
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER_BASE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_COMPONENT
    { 2,                0, isRegionalIndicator },
    { 1,                U_MASK(UPROPS_PREPENDED_CONCATENATION_MARK), defaultContains },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EXTENDED_PICTOGRAPHIC
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_BASIC_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_KEYCAP_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_MODIFIER_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_FLAG_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_TAG_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_ZWJ_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI
    { UPROPS_SRC_IDSU, 0, isIDSUnaryOperator }, // UCHAR_IDS_UNARY_OPERATOR
    { UPROPS_SRC_ID_COMPAT_MATH, 0, isIDCompatMathStart }, // UCHAR_ID_COMPAT_MATH_START
    { UPROPS_SRC_ID_COMPAT_MATH, 0, isIDCompatMathContinue }, // UCHAR_ID_COMPAT_MATH_CONTINUE
    { UPROPS_SRC_MCM, 0 , isModifierCombiningMark }, // UCHAR_MODIFIER_COMBINING_MARK
};

U_CAPI UBool U_EXPORT2
u_has_দ্বিমিক_Property(UChar32 c, UProperty which) {
    /* c is range-checked in the functions that are called from here */
    যদি(which<UCHAR_দ্বিমিক_START || UCHAR_দ্বিমিক_LIMIT<=which) {
        /* not a known binary property */
        ঘূৰাই মিছা;
    } অন্যথা {
        ধ্রুৱ দ্বিমিক_Property &prop=binProps[which];
        ঘূৰাই prop.contains(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    ঘূৰাই u_has_দ্বিমিক_Property(c, UCHAR_ID_START);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    ঘূৰাই u_has_দ্বিমিক_Property(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_string_Has_দ্বিমিক_Property(ধ্রুৱ char16_t *s, int32_t length, UProperty which) {
    যদি (s == nullptr && length != 0) { ঘূৰাই মিছা; }
    যদি (length == 1) {
        ঘূৰাই u_has_দ্বিমিক_Property(s[0], which);  // single code point
    } অন্যথা যদি (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        যদি (length > 0 ? i == length : s[i] == 0) {
            ঘূৰাই u_has_দ্বিমিক_Property(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    ঘূৰাই UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::has_দ্বিমিক_Property(s, length, which);
}

সংরচনা IntProperty;

প্রকারনির্দেশ int32_t Int_Property_Get_মান(ধ্রুৱ IntProperty &prop, UChar32 c, UProperty which);
প্রকারনির্দেশ int32_t Int_Property_Get_Max_মান(ধ্রুৱ IntProperty &prop, UProperty which);

সংরচনা IntProperty {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t shift;  // =maxValue if getMaxValueFromShift() is used
    Int_Property_Get_মান *get_মান;
    Int_Property_Get_Max_মান *get_Max_মান;
};

স্থির int32_t default_Get_মান(ধ্রুৱ IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    ঘূৰাই static_cast<int32_t>(u_getUnicodeProperties(c, prop.column) & prop.mask) >> prop.shift;
}

স্থির int32_t default_Get_Max_মান(ধ্রুৱ IntProperty &prop, UProperty /*which*/) {
    ঘূৰাই (uprv_get_Max_মানসমূহ(prop.column)&prop.mask)>>prop.shift;
}

স্থির int32_t get_Max_মান_From_Shift(ধ্রুৱ IntProperty &prop, UProperty /*which*/) {
    ঘূৰাই prop.shift;
}

স্থির int32_t getBiDiClass(ধ্রুৱ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই static_cast<int32_t>(u_charDirection(c));
}

স্থির int32_t getBiDiPairedBracketType(ধ্রুৱ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

স্থির int32_t bi_Di_Get_Max_মান(ধ্রুৱ IntProperty &/*prop*/, UProperty which) {
    ঘূৰাই ubidi_get_Max_মান(which);
}

স্থির int32_t getBlock(ধ্রুৱ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই static_cast<int32_t>(ublock_getCode(c));
}

স্থির int32_t block_Get_Max_মান(ধ্রুৱ IntProperty &/*prop*/, UProperty /*which*/) {
    ঘূৰাই uprv_get_Max_মানসমূহ(UPROPS_MAX_মানসমূহ_OTHER_INDEX) & UPROPS_MAX_BLOCK;
}

#যদি UCONFIG_NO_NORMALIZATION
স্থির int32_t getCombiningClass(ধ্রুৱ IntProperty &, UChar32, UProperty) {
    ঘূৰাই 0;
}
#অন্যথা
স্থির int32_t getCombiningClass(ধ্রুৱ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই u_getCombiningClass(c);
}
#endif

স্থির int32_t getGeneralCategory(ধ্রুৱ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই static_cast<int32_t>(u_charType(c));
}

স্থির int32_t getJoiningGroup(ধ্রুৱ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই ubidi_getJoiningGroup(c);
}

স্থির int32_t getJoiningType(ধ্রুৱ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই ubidi_getJoiningType(c);
}

স্থির int32_t getNumericType(ধ্রুৱ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_মান(u_getMainProperties(c)));
    ঘূৰাই UPROPS_NTV_GET_TYPE(ntv);
}

স্থির int32_t getScript(ধ্রুৱ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ঘূৰাই static_cast<int32_t>(uscript_getScript(c, &errorCode));
}

স্থির int32_t script_Get_Max_মান(ধ্রুৱ IntProperty &/*prop*/, UProperty /*which*/) {
    ঘূৰাই uprv_get_Max_মানসমূহ(0)&UPROPS_MAX_SCRIPT;
}

/*
 * Map some of the Grapheme Cluster Break values to Hangul Syllable Types.
 * Hangul_Syllable_Type used to be fully redundant with a subset of Grapheme_Cluster_Break.
 *
 * Starting with Unicode 16, this is no longer true for HST=V vs. GCB=V in some cases:
 * Some Kirat Rai vowels are given GCB=V for proper grapheme clustering, but
 * they are of course not related to Hangul syllables.
 */
স্থির ধ্রুৱ UHangulSyllableType gcbToHst[]={
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

স্থির int32_t getHangulSyllableType(ধ্রুৱ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Ignore supplementary code points: They all have HST=NA.
    // This is a simple way to handle the GCB!=hst cases since Unicode 16 (Kirat Rai vowels).
    যদি(c>0xffff) {
        ঘূৰাই U_HST_NOT_APPLICABLE;
    }
    /* see comments on gcbToHst[] above */
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_MASK) >> UPROPS_GCB_SHIFT;
    যদি(gcb<UPRV_LENGTHOF(gcbToHst)) {
        ঘূৰাই gcbToHst[gcb];
    } অন্যথা {
        ঘূৰাই U_HST_NOT_APPLICABLE;
    }
}

#যদি UCONFIG_NO_NORMALIZATION
স্থির int32_t getNormQuickCheck(ধ্রুৱ IntProperty &, UChar32, UProperty) {
    ঘূৰাই 0;
}
#অন্যথা
স্থির int32_t getNormQuickCheck(ধ্রুৱ IntProperty &/*prop*/, UChar32 c, UProperty which) {
    ঘূৰাই static_cast<int32_t>(unorm_getQuickCheck(c, static_cast<UNormalizationMode>(which - UCHAR_NFD_QUICK_CHECK + UNORM_NFD)));
}
#endif

#যদি UCONFIG_NO_NORMALIZATION
স্থির int32_t getLeadCombiningClass(ধ্রুৱ IntProperty &, UChar32, UProperty) {
    ঘূৰাই 0;
}
#অন্যথা
স্থির int32_t getLeadCombiningClass(ধ্রুৱ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই unorm_getFCD16(c)>>8;
}
#endif

#যদি UCONFIG_NO_NORMALIZATION
স্থির int32_t getTrailCombiningClass(ধ্রুৱ IntProperty &, UChar32, UProperty) {
    ঘূৰাই 0;
}
#অন্যথা
স্থির int32_t getTrailCombiningClass(ধ্রুৱ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ঘূৰাই unorm_getFCD16(c)&0xff;
}
#endif

স্থির int32_t getInPC(ধ্রুৱ IntProperty &, UChar32 c, UProperty) {
    ঘূৰাই ulayout_ensureData() && gInpcTrie != nullptr ? ucptrie_get(gInpcTrie, c) : 0;
}

স্থির int32_t getInSC(ধ্রুৱ IntProperty &, UChar32 c, UProperty) {
    ঘূৰাই ulayout_ensureData() && gInscTrie != nullptr ? ucptrie_get(gInscTrie, c) : 0;
}

স্থির int32_t getVo(ধ্রুৱ IntProperty &, UChar32 c, UProperty) {
    ঘূৰাই ulayout_ensureData() && gVoTrie != nullptr ? ucptrie_get(gVoTrie, c) : 0;
}

স্থির int32_t layout_Get_Max_মান(ধ্রুৱ IntProperty &/*prop*/, UProperty which) {
    যদি (!ulayout_ensureData()) { ঘূৰাই 0; }
    ৱিকল্প (which) {
    পক্ষ UCHAR_INDIC_POSITIONAL_CATEGORY:
        ঘূৰাই g_Max_Inpc_মান;
    পক্ষ UCHAR_INDIC_SYLLABIC_CATEGORY:
        ঘূৰাই g_Max_Insc_মান;
    পক্ষ UCHAR_VERTICAL_ORIENTATION:
        ঘূৰাই g_Max_Vo_মান;
    স্ৱভাৱ:
        ঘূৰাই 0;
    }
}

স্থির int32_t get_IDStatus_মান(ধ্রুৱ IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    ঘূৰাই value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_STATUS_ALLOWED : U_ID_STATUS_RESTRICTED;
}

স্থির ধ্রুৱ IntProperty intProps[UCHAR_INT_LIMIT-UCHAR_INT_START]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, bi_Di_Get_Max_মান },
    { UPROPS_SRC_BLOCK, 0, 0,                               getBlock, block_Get_Max_মান },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, get_Max_মান_From_Shift },
    { 2,                UPROPS_DT_MASK, 0,                  default_Get_মান, default_Get_Max_মান },
    { 0,                UPROPS_EA_MASK, UPROPS_EA_SHIFT,    default_Get_মান, default_Get_Max_মান },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_গুনতি) - 1, getGeneralCategory, get_Max_মান_From_Shift },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningGroup, bi_Di_Get_Max_মান },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, bi_Di_Get_Max_মান },
    { 2,                UPROPS_LB_MASK, UPROPS_LB_SHIFT,    default_Get_মান, default_Get_Max_মান },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_গুনতি) - 1, getNumericType, get_Max_মান_From_Shift },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, script_Get_Max_মান },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_গুনতি) - 1, getHangulSyllableType, get_Max_মান_From_Shift },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, get_Max_মান_From_Shift },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, get_Max_মান_From_Shift },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, get_Max_মান_From_Shift },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, get_Max_মান_From_Shift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, get_Max_মান_From_Shift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, get_Max_মান_From_Shift },
    { 2,                UPROPS_GCB_MASK, UPROPS_GCB_SHIFT,  default_Get_মান, default_Get_Max_মান },
    { 2,                UPROPS_SB_MASK, UPROPS_SB_SHIFT,    default_Get_মান, default_Get_Max_মান },
    { 2,                UPROPS_WB_MASK, UPROPS_WB_SHIFT,    default_Get_মান, default_Get_Max_মান },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, bi_Di_Get_Max_মান },
    { UPROPS_SRC_INPC,  0, 0,                               getInPC, layout_Get_Max_মান },
    { UPROPS_SRC_INSC,  0, 0,                               getInSC, layout_Get_Max_মান },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layout_Get_Max_মান },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_STATUS_ALLOWED), get_IDStatus_মান, get_Max_মান_From_Shift },
    { 0,                UPROPS_INCB_MASK, UPROPS_INCB_SHIFT,default_Get_মান, default_Get_Max_মান },
};

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_মান(UChar32 c, UProperty which) {
    যদি(which<UCHAR_INT_START) {
        যদি(UCHAR_দ্বিমিক_START<=which && which<UCHAR_দ্বিমিক_LIMIT) {
            ধ্রুৱ দ্বিমিক_Property &prop=binProps[which];
            ঘূৰাই prop.contains(prop, c, which);
        }
    } অন্যথা যদি(which<UCHAR_INT_LIMIT) {
        ধ্রুৱ IntProperty &prop=intProps[which-UCHAR_INT_START];
        ঘূৰাই prop.get_মান(prop, c, which);
    } অন্যথা যদি(which==UCHAR_GENERAL_CATEGORY_MASK) {
        ঘূৰাই U_MASK(u_charType(c));
    }
    ঘূৰাই 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Min_মান(UProperty /*which*/) {
    ঘূৰাই 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Max_মান(UProperty which) {
    যদি(which<UCHAR_INT_START) {
        যদি(UCHAR_দ্বিমিক_START<=which && which<UCHAR_দ্বিমিক_LIMIT) {
            ঘূৰাই 1;  // maximum true for all binary properties
        }
    } অন্যথা যদি(which<UCHAR_INT_LIMIT) {
        ধ্রুৱ IntProperty &prop=intProps[which-UCHAR_INT_START];
        ঘূৰাই prop.get_Max_মান(prop, which);
    }
    ঘূৰাই -1;  // undefined
}

U_CFUNC UPropertySource U_EXPORT2
uprops_getSource(UProperty which) {
    যদি(which<UCHAR_দ্বিমিক_START) {
        ঘূৰাই UPROPS_SRC_NONE; /* undefined */
    } অন্যথা যদি(which<UCHAR_দ্বিমিক_LIMIT) {
        ধ্রুৱ দ্বিমিক_Property &prop=binProps[which];
        যদি(prop.mask!=0) {
            ঘূৰাই UPROPS_SRC_PROPSVEC;
        } অন্যথা {
            ঘূৰাই (UPropertySource)prop.column;
        }
    } অন্যথা যদি(which<UCHAR_INT_START) {
        ঘূৰাই UPROPS_SRC_NONE; /* undefined */
    } অন্যথা যদি(which<UCHAR_INT_LIMIT) {
        ধ্রুৱ IntProperty &prop=intProps[which-UCHAR_INT_START];
        যদি(prop.mask!=0) {
            ঘূৰাই UPROPS_SRC_PROPSVEC;
        } অন্যথা {
            ঘূৰাই (UPropertySource)prop.column;
        }
    } অন্যথা যদি(which<UCHAR_STRING_START) {
        ৱিকল্প(which) {
        পক্ষ UCHAR_GENERAL_CATEGORY_MASK:
        পক্ষ UCHAR_NUMERIC_মান:
            ঘূৰাই UPROPS_SRC_CHAR;

        স্ৱভাৱ:
            ঘূৰাই UPROPS_SRC_NONE;
        }
    } অন্যথা যদি(which<UCHAR_STRING_LIMIT) {
        ৱিকল্প(which) {
        পক্ষ UCHAR_AGE:
            ঘূৰাই UPROPS_SRC_PROPSVEC;

        পক্ষ UCHAR_BIDI_MIRRORING_GLYPH:
            ঘূৰাই UPROPS_SRC_BIDI;

        পক্ষ UCHAR_CASE_FOLDING:
        পক্ষ UCHAR_LOWERCASE_MAPPING:
        পক্ষ UCHAR_SIMPLE_CASE_FOLDING:
        পক্ষ UCHAR_SIMPLE_LOWERCASE_MAPPING:
        পক্ষ UCHAR_SIMPLE_TITLECASE_MAPPING:
        পক্ষ UCHAR_SIMPLE_UPPERCASE_MAPPING:
        পক্ষ UCHAR_TITLECASE_MAPPING:
        পক্ষ UCHAR_UPPERCASE_MAPPING:
            ঘূৰাই UPROPS_SRC_CASE;

        পক্ষ UCHAR_ISO_COMMENT:
        পক্ষ UCHAR_NAME:
        পক্ষ UCHAR_UNICODE_1_NAME:
            ঘূৰাই UPROPS_SRC_NAMES;

        স্ৱভাৱ:
            ঘূৰাই UPROPS_SRC_NONE;
        }
    } অন্যথা {
        ৱিকল্প(which) {
        পক্ষ UCHAR_SCRIPT_EXTENSIONS:
        পক্ষ UCHAR_IDENTIFIER_TYPE:
            ঘূৰাই UPROPS_SRC_PROPSVEC;
        স্ৱভাৱ:
            ঘূৰাই UPROPS_SRC_NONE; /* undefined */
        }
    }
}

U_CFUNC শূন্য U_EXPORT2
uprops_addPropertyStarts(UPropertySource src, ধ্রুৱ USetAdder *sa, UErrorCode *pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) { ঘূৰাই; }
    যদি (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        বাবে (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->add(sa->set, c);
        }
        // single characters
        বাবে (UChar32 c : ID_COMPAT_MATH_START) {
            sa->add(sa->set, c);
            sa->add(sa->set, c + 1);
        }
        ঘূৰাই;
    }
    যদি (src == UPROPS_SRC_MCM) {
        // range limits
        বাবে (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->add(sa->set, c);
        }
        ঘূৰাই;
    }
    যদি (!ulayout_ensureData(*pErrorCode)) { ঘূৰাই; }
    ধ্রুৱ UCPTrie *trie;
    ৱিকল্প (src) {
    পক্ষ UPROPS_SRC_INPC:
        trie = gInpcTrie;
        ভঙ্গ;
    পক্ষ UPROPS_SRC_INSC:
        trie = gInscTrie;
        ভঙ্গ;
    পক্ষ UPROPS_SRC_VO:
        trie = gVoTrie;
        ভঙ্গ;
    স্ৱভাৱ:
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
    }

    যদি (trie == nullptr) {
        *pErrorCode = U_MISSING_RESOURCE_ERROR;
        ঘূৰাই;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    যেতিয়ালৈ ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, nullptr)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

U_CAPI তার্কিক U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t typeIndex = type;  // also guards against negative type integers
    যদি (typeIndex >= UPRV_LENGTHOF(uprops_idTypeToEncoded)) {
        ঘূৰাই মিছা;
    }
    uint32_t encodedType = uprops_idTypeToEncoded[typeIndex];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    যদি ((encodedType & UPROPS_ID_TYPE_BIT) != 0) {
        ঘূৰাই value < UPROPS_ID_TYPE_FORBIDDEN && (value & encodedType) != 0;
    } অন্যথা {
        ঘূৰাই value == encodedType;
    }
}

নামাকাশ {

শূন্য maybeAppendType(uint32_t value, uint32_t bit, UIdentifierType t,
                     UIdentifierType *types, int32_t &length, int32_t capacity) {
    যদি ((value & bit) != 0) {
        যদি (length < capacity) {
            types[length] = t;
        }
        ++length;
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UErrorCode *pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) { ঘূৰাই 0; }
    যদি (capacity < 0 || (capacity > 0 && types == nullptr)) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    যদি ((value & UPROPS_ID_TYPE_FORBIDDEN) == UPROPS_ID_TYPE_FORBIDDEN ||
            value == UPROPS_ID_TYPE_NOT_CHARACTER) {
        // single value
        যদি (capacity > 0) {
            UIdentifierType t;
            ৱিকল্প (value) {
                পক্ষ UPROPS_ID_TYPE_NOT_CHARACTER: t = U_ID_TYPE_NOT_CHARACTER; ভঙ্গ;
                পক্ষ UPROPS_ID_TYPE_DEPRECATED: t = U_ID_TYPE_DEPRECATED; ভঙ্গ;
                পক্ষ UPROPS_ID_TYPE_DEFAULT_IGNORABLE: t = U_ID_TYPE_DEFAULT_IGNORABLE; ভঙ্গ;
                পক্ষ UPROPS_ID_TYPE_NOT_NFKC: t = U_ID_TYPE_NOT_NFKC; ভঙ্গ;
                পক্ষ UPROPS_ID_TYPE_INCLUSION: t = U_ID_TYPE_INCLUSION; ভঙ্গ;
                পক্ষ UPROPS_ID_TYPE_RECOMMENDED: t = U_ID_TYPE_RECOMMENDED; ভঙ্গ;
                স্ৱভাৱ:
                    *pErrorCode = U_INVALID_FORMAT_ERROR;
                    ঘূৰাই 0;
            }
            types[0] = t;
        } অন্যথা {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        ঘূৰাই 1;
    } অন্যথা {
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
        যদি (length >= capacity) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        ঘূৰাই length;
    }
}

#যদি !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_getFC_NFKC_Closure(UChar32 c, char16_t *dest, int32_t destCapacity, UErrorCode *pErrorCode) {
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }
    যদি(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    ধ্রুৱ Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*pErrorCode);
    যদি(U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    ধ্রুৱ char16_t *folded1;
    int32_t folded1Length=ucase_toFullFolding(c, &folded1, U_FOLD_CASE_DEFAULT);
    যদি(folded1Length<0) {
        ধ্রুৱ Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        যদি(nfkcImpl->getCompQuickCheck(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            ঘূৰাই u_terminateUChars(dest, destCapacity, 0, pErrorCode);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.setTo(c);
    } অন্যথা {
        যদি(folded1Length>UCASE_MAX_STRING_LENGTH) {
            folded1String.setTo(folded1Length);
        } অন্যথা {
            folded1String.setTo(মিছা, folded1, folded1Length);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *pErrorCode);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *pErrorCode);
    // if (c != b) add the mapping from a to c
    যদি(U_FAILURE(*pErrorCode) || kc1==kc2) {
        ঘূৰাই u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    } অন্যথা {
        ঘূৰাই kc2.extract(dest, destCapacity, *pErrorCode);
    }
}

#endif
