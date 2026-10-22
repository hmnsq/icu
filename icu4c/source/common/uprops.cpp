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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/ucptrie.h"
#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "unicode/unorm2.h"
#അന്തര്ഭാവ "unicode/uscript.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "emojiprops.h"
#അന്തര്ഭാവ "mutex.h"
#അന്തര്ഭാവ "normalizer2impl.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "ubidi_props.h"
#അന്തര്ഭാവ "uprops.h"
#അന്തര്ഭാവ "ucase.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "ulayout_props.h"
#അന്തര്ഭാവ "ustr_imp.h"

U_NAMESPACE_USE

// Unicode text layout properties data -----------------------------------------

നാമാകാശ {

icu::UInitOnce gLayoutInitOnce {};
UDataMemory *gLayoutMemory = nullptr;

UCPTrie *gInpcTrie = nullptr;  // Indic_Positional_Category
UCPTrie *gInscTrie = nullptr;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = nullptr;  // Vertical_Orientation

int32_t g_Max_Inpc_മൂല്യം = 0;
int32_t g_Max_Insc_മൂല്യം = 0;
int32_t g_Max_Vo_മൂല്യം = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(gLayoutMemory);
    gLayoutMemory = nullptr;

    ucptrie_close(gInpcTrie);
    gInpcTrie = nullptr;
    ucptrie_close(gInscTrie);
    gInscTrie = nullptr;
    ucptrie_close(gVoTrie);
    gVoTrie = nullptr;

    g_Max_Inpc_മൂല്യം = 0;
    g_Max_Insc_മൂല്യം = 0;
    g_Max_Vo_മൂല്യം = 0;

    gLayoutInitOnce.reset();
    തിരികെ സത്യം;
}

UBool U_CALLCONV
ulayout_isAcceptable(ശൂന്യം * /*context*/,
                     സ്ഥിരം അക്ഷരം * /* type */, സ്ഥിരം അക്ഷരം * /*name*/,
                     സ്ഥിരം UDataInfo *pInfo) {
    തിരികെ pInfo->size >= 20 &&
        pInfo->isBigEndian == U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily == U_CHARSET_FAMILY &&
        pInfo->dataFormat[0] == ULAYOUT_FMT_0 &&
        pInfo->dataFormat[1] == ULAYOUT_FMT_1 &&
        pInfo->dataFormat[2] == ULAYOUT_FMT_2 &&
        pInfo->dataFormat[3] == ULAYOUT_FMT_3 &&
        pInfo->formatVersion[0] == 1;
}

// UInitOnce singleton initialization function
ശൂന്യം U_CALLCONV ulayout_load(UErrorCode &errorCode) {
    gLayoutMemory = udata_openChoice(
        nullptr, ULAYOUT_DATA_TYPE, ULAYOUT_DATA_NAME,
        ulayout_isAcceptable, nullptr, &errorCode);
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }

    സ്ഥിരം uint8_t* inBytes = static_cast<സ്ഥിരം uint8_t*>(udata_getMemory(gLayoutMemory));
    സ്ഥിരം int32_t* inIndexes = reinterpret_cast<സ്ഥിരം int32_t*>(inBytes);
    int32_t indexesLength = inIndexes[ULAYOUT_IX_INDEXES_LENGTH];
    എങ്കിൽ (indexesLength < 12) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        തിരികെ;
    }
    int32_t offset = indexesLength * 4;
    int32_t മുകൾ = inIndexes[ULAYOUT_IX_INPC_TRIE_മുകൾ];
    int32_t trieSize = മുകൾ - offset;
    എങ്കിൽ (trieSize >= 16) {
        gInpcTrie = ucptrie_open_From_ദ്വിമാന(
            UCPTRIE_TYPE_ANY, UCPTRIE_മൂല്യം_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }
    offset = മുകൾ;
    മുകൾ = inIndexes[ULAYOUT_IX_INSC_TRIE_മുകൾ];
    trieSize = മുകൾ - offset;
    എങ്കിൽ (trieSize >= 16) {
        gInscTrie = ucptrie_open_From_ദ്വിമാന(
            UCPTRIE_TYPE_ANY, UCPTRIE_മൂല്യം_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }
    offset = മുകൾ;
    മുകൾ = inIndexes[ULAYOUT_IX_VO_TRIE_മുകൾ];
    trieSize = മുകൾ - offset;
    എങ്കിൽ (trieSize >= 16) {
        gVoTrie = ucptrie_open_From_ദ്വിമാന(
            UCPTRIE_TYPE_ANY, UCPTRIE_മൂല്യം_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }

    uint32_t max_മൂല്യങ്ങൾ = inIndexes[ULAYOUT_IX_MAX_മൂല്യങ്ങൾ];
    g_Max_Inpc_മൂല്യം = max_മൂല്യങ്ങൾ >> ULAYOUT_MAX_INPC_SHIFT;
    g_Max_Insc_മൂല്യം = (max_മൂല്യങ്ങൾ >> ULAYOUT_MAX_INSC_SHIFT) & 0xff;
    g_Max_Vo_മൂല്യം = (max_മൂല്യങ്ങൾ >> ULAYOUT_MAX_VO_SHIFT) & 0xff;

    ucln_common_registerCleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensureData(UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_load, errorCode);
    തിരികെ U_SUCCESS(errorCode);
}

UBool ulayout_ensureData() {
    UErrorCode errorCode = U_ZERO_ERROR;
    തിരികെ ulayout_ensureData(errorCode);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

ഘടന ദ്വിമാന_Property;

തരനിർവചനം UBool ദ്വിമാന_Property_Contains(സ്ഥിരം ദ്വിമാന_Property &prop, UChar32 c, UProperty which);

ഘടന ദ്വിമാന_Property {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    ദ്വിമാന_Property_Contains *contains;
};

സ്ഥിതം UBool defaultContains(സ്ഥിരം ദ്വിമാന_Property &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    തിരികെ (u_getUnicodeProperties(c, prop.column)&prop.mask)!=0;
}

സ്ഥിതം UBool case_ദ്വിമാന_Property_Contains(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty which) {
    തിരികെ ucase_has_ദ്വിമാന_Property(c, which);
}

സ്ഥിതം UBool isBidiControl(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ ubidi_isBidiControl(c);
}

സ്ഥിതം UBool isMirrored(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ ubidi_isMirrored(c);
}

സ്ഥിതം UBool isJoinControl(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ ubidi_isJoinControl(c);
}

#എങ്കിൽ UCONFIG_NO_NORMALIZATION
സ്ഥിതം UBool hasFullCompositionExclusion(സ്ഥിരം ദ്വിമാന_Property &, UChar32, UProperty) {
    തിരികെ അസത്യം;
}
#അല്ലെങ്കിൽ
സ്ഥിതം UBool hasFullCompositionExclusion(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UErrorCode errorCode=U_ZERO_ERROR;
    സ്ഥിരം Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    തിരികെ U_SUCCESS(errorCode) && impl->isCompNo(impl->getNorm16(c));
}
#endif

// UCHAR_NF*_INERT properties
#എങ്കിൽ UCONFIG_NO_NORMALIZATION
സ്ഥിതം UBool isNormInert(സ്ഥിരം ദ്വിമാന_Property &, UChar32, UProperty) {
    തിരികെ അസത്യം;
}
#അല്ലെങ്കിൽ
സ്ഥിതം UBool isNormInert(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty which) {
    UErrorCode errorCode=U_ZERO_ERROR;
    സ്ഥിരം Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalizationMode>(which - UCHAR_NFD_INERT + UNORM_NFD), errorCode);
    തിരികെ U_SUCCESS(errorCode) && norm2->isInert(c);
}
#endif

#എങ്കിൽ UCONFIG_NO_NORMALIZATION
സ്ഥിതം UBool changesWhenCasefolded(സ്ഥിരം ദ്വിമാന_Property &, UChar32, UProperty) {
    തിരികെ അസത്യം;
}
#അല്ലെങ്കിൽ
സ്ഥിതം UBool changesWhenCasefolded(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UErrorCode errorCode=U_ZERO_ERROR;
    സ്ഥിരം Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(errorCode);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ അസത്യം;
    }
    എങ്കിൽ(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        എങ്കിൽ(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } അല്ലെങ്കിൽ എങ്കിൽ(nfd.length()<=U16_MAX_LENGTH &&
                  nfd.length()==U16_LENGTH(c=nfd.char32At(0))
        ) {
            /* single supplementary code point */
        } അല്ലെങ്കിൽ {
            c=U_SENTINEL;
        }
    } അല്ലെങ്കിൽ എങ്കിൽ(c<0) {
        തിരികെ അസത്യം;  /* protect against bad input */
    }
    എങ്കിൽ(c>=0) {
        /* single code point */
        സ്ഥിരം char16_t *resultString;
        തിരികെ ucase_toFullFolding(c, &resultString, U_FOLD_CASE_DEFAULT) >= 0;
    } അല്ലെങ്കിൽ {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_LENGTH];
        int32_t destLength;
        destLength=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.getBuffer(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &errorCode);
        തിരികെ U_SUCCESS(errorCode) &&
                       0!=u_strCompare(nfd.getBuffer(), nfd.length(),
                                       dest, destLength, അസത്യം);
    }
}
#endif

#എങ്കിൽ UCONFIG_NO_NORMALIZATION
സ്ഥിതം UBool changesWhenNFKC_Casefolded(സ്ഥിരം ദ്വിമാന_Property &, UChar32, UProperty) {
    തിരികെ അസത്യം;
}
#അല്ലെങ്കിൽ
സ്ഥിതം UBool changesWhenNFKC_Casefolded(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    സ്ഥിരം Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(errorCode);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ അസത്യം;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        ReorderingBuffer buffer(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        എങ്കിൽ(buffer.init(5, errorCode)) {
            സ്ഥിരം char16_t *srcArray=src.getBuffer();
            kcf->compose(srcArray, srcArray+src.length(), അസത്യം,
                          സത്യം, buffer, errorCode);
        }
    }
    തിരികെ U_SUCCESS(errorCode) && dest!=src;
}
#endif

#എങ്കിൽ UCONFIG_NO_NORMALIZATION
സ്ഥിതം UBool isCanonSegmentStarter(സ്ഥിരം ദ്വിമാന_Property &, UChar32, UProperty) {
    തിരികെ അസത്യം;
}
#അല്ലെങ്കിൽ
സ്ഥിതം UBool isCanonSegmentStarter(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    സ്ഥിരം Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    തിരികെ
        U_SUCCESS(errorCode) && impl->ensureCanonIterData(errorCode) &&
        impl->isCanonSegmentStarter(c);
}
#endif

സ്ഥിതം UBool isPOSIX_alnum(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ u_isalnumPOSIX(c);
}

സ്ഥിതം UBool isPOSIX_blank(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ u_isblank(c);
}

സ്ഥിതം UBool isPOSIX_graph(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ u_isgraphPOSIX(c);
}

സ്ഥിതം UBool isPOSIX_print(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ u_isprintPOSIX(c);
}

സ്ഥിതം UBool isPOSIX_xdigit(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ u_isxdigit(c);
}

സ്ഥിതം UBool isRegionalIndicator(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    തിരികെ 0x1F1E6<=c && c<=0x1F1FF;
}

സ്ഥിതം UBool hasEmojiProperty(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty which) {
    തിരികെ EmojiProps::has_ദ്വിമാന_Property(c, which);
}

സ്ഥിതം UBool isIDSUnaryOperator(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // New in Unicode 15.1 for just two characters.
    തിരികെ 0x2FFE<=c && c<=0x2FFF;
}

/** Ranges (start/limit pairs) of ID_Compat_Math_Continue (only), from UCD PropList.txt. */
സ്ഥിതം constexpr UChar32 ID_COMPAT_MATH_CONTINUE[] = {
    0x00B2, 0x00B3 + 1,
    0x00B9, 0x00B9 + 1,
    0x2070, 0x2070 + 1,
    0x2074, 0x207E + 1,
    0x2080, 0x208E + 1
};

/** ID_Compat_Math_Start characters, from UCD PropList.txt. */
സ്ഥിതം constexpr UChar32 ID_COMPAT_MATH_START[] = {
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
സ്ഥിതം constexpr UChar32 MODIFIER_COMBINING_MARK[] = {
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

സ്ഥിതം UBool isIDCompatMathStart(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    എങ്കിൽ (c < ID_COMPAT_MATH_START[0]) { തിരികെ അസത്യം; }  // fastpath for common scripts
    വേണ്ടി (UChar32 startChar : ID_COMPAT_MATH_START) {
        എങ്കിൽ (c == startChar) { തിരികെ സത്യം; }
    }
    തിരികെ അസത്യം;
}

സ്ഥിതം UBool isIDCompatMathContinue(സ്ഥിരം ദ്വിമാന_Property &prop, UChar32 c, UProperty /*which*/) {
    വേണ്ടി (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        എങ്കിൽ (c < ID_COMPAT_MATH_CONTINUE[i]) { തിരികെ അസത്യം; }  // below range start
        എങ്കിൽ (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { തിരികെ സത്യം; }  // below range limit
    }
    തിരികെ isIDCompatMathStart(prop, c, UCHAR_ID_COMPAT_MATH_START);
}

സ്ഥിതം UBool isModifierCombiningMark(സ്ഥിരം ദ്വിമാന_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    വേണ്ടി (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        എങ്കിൽ (c < MODIFIER_COMBINING_MARK[i]) { തിരികെ അസത്യം; }  // below range start
        എങ്കിൽ (c < MODIFIER_COMBINING_MARK[i + 1]) { തിരികെ സത്യം; }  // below range limit
    }
    തിരികെ അസത്യം;
}

സ്ഥിതം സ്ഥിരം ദ്വിമാന_Property binProps[UCHAR_ദ്വിമാന_LIMIT]={
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
    { 1,                U_MASK(UPROPS_IDS_ദ്വിമാന_OPERATOR), defaultContains },
    { 1,                U_MASK(UPROPS_IDS_TRINARY_OPERATOR), defaultContains },
    { UPROPS_SRC_BIDI,  0, isJoinControl },
    { 1,                U_MASK(UPROPS_LOGICAL_ORDER_EXCEPTION), defaultContains },
    { UPROPS_SRC_CASE,  0, case_ദ്വിമാന_Property_Contains },  // UCHAR_LOWERCASE
    { 1,                U_MASK(UPROPS_MATH), defaultContains },
    { 1,                U_MASK(UPROPS_NONCHARACTER_CODE_POINT), defaultContains },
    { 1,                U_MASK(UPROPS_QUOTATION_MARK), defaultContains },
    { 1,                U_MASK(UPROPS_RADICAL), defaultContains },
    { UPROPS_SRC_CASE,  0, case_ദ്വിമാന_Property_Contains },  // UCHAR_SOFT_DOTTED
    { 1,                U_MASK(UPROPS_TERMINAL_PUNCTUATION), defaultContains },
    { 1,                U_MASK(UPROPS_UNIFIED_IDEOGRAPH), defaultContains },
    { UPROPS_SRC_CASE,  0, case_ദ്വിമാന_Property_Contains },  // UCHAR_UPPERCASE
    { 1,                U_MASK(UPROPS_WHITE_SPACE), defaultContains },
    { 1,                U_MASK(UPROPS_XID_CONTINUE), defaultContains },
    { 1,                U_MASK(UPROPS_XID_START), defaultContains },
    { UPROPS_SRC_CASE,  0, case_ദ്വിമാന_Property_Contains },  // UCHAR_CASE_SENSITIVE
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
    { UPROPS_SRC_CASE,  0, case_ദ്വിമാന_Property_Contains },  // UCHAR_CASED
    { UPROPS_SRC_CASE,  0, case_ദ്വിമാന_Property_Contains },  // UCHAR_CASE_IGNORABLE
    { UPROPS_SRC_CASE,  0, case_ദ്വിമാന_Property_Contains },  // UCHAR_CHANGES_WHEN_LOWERCASED
    { UPROPS_SRC_CASE,  0, case_ദ്വിമാന_Property_Contains },  // UCHAR_CHANGES_WHEN_UPPERCASED
    { UPROPS_SRC_CASE,  0, case_ദ്വിമാന_Property_Contains },  // UCHAR_CHANGES_WHEN_TITLECASED
    { UPROPS_SRC_CASE_AND_NORM,  0, changesWhenCasefolded },
    { UPROPS_SRC_CASE,  0, case_ദ്വിമാന_Property_Contains },  // UCHAR_CHANGES_WHEN_CASEMAPPED
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
u_has_ദ്വിമാന_Property(UChar32 c, UProperty which) {
    /* c is range-checked in the functions that are called from here */
    എങ്കിൽ(which<UCHAR_ദ്വിമാന_START || UCHAR_ദ്വിമാന_LIMIT<=which) {
        /* not a known binary property */
        തിരികെ അസത്യം;
    } അല്ലെങ്കിൽ {
        സ്ഥിരം ദ്വിമാന_Property &prop=binProps[which];
        തിരികെ prop.contains(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    തിരികെ u_has_ദ്വിമാന_Property(c, UCHAR_ID_START);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    തിരികെ u_has_ദ്വിമാന_Property(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_string_Has_ദ്വിമാന_Property(സ്ഥിരം char16_t *s, int32_t length, UProperty which) {
    എങ്കിൽ (s == nullptr && length != 0) { തിരികെ അസത്യം; }
    എങ്കിൽ (length == 1) {
        തിരികെ u_has_ദ്വിമാന_Property(s[0], which);  // single code point
    } അല്ലെങ്കിൽ എങ്കിൽ (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        എങ്കിൽ (length > 0 ? i == length : s[i] == 0) {
            തിരികെ u_has_ദ്വിമാന_Property(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    തിരികെ UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::has_ദ്വിമാന_Property(s, length, which);
}

ഘടന IntProperty;

തരനിർവചനം int32_t Int_Property_Get_മൂല്യം(സ്ഥിരം IntProperty &prop, UChar32 c, UProperty which);
തരനിർവചനം int32_t Int_Property_Get_Max_മൂല്യം(സ്ഥിരം IntProperty &prop, UProperty which);

ഘടന IntProperty {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t shift;  // =maxValue if getMaxValueFromShift() is used
    Int_Property_Get_മൂല്യം *get_മൂല്യം;
    Int_Property_Get_Max_മൂല്യം *get_Max_മൂല്യം;
};

സ്ഥിതം int32_t default_Get_മൂല്യം(സ്ഥിരം IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    തിരികെ static_cast<int32_t>(u_getUnicodeProperties(c, prop.column) & prop.mask) >> prop.shift;
}

സ്ഥിതം int32_t default_Get_Max_മൂല്യം(സ്ഥിരം IntProperty &prop, UProperty /*which*/) {
    തിരികെ (uprv_get_Max_മൂല്യങ്ങൾ(prop.column)&prop.mask)>>prop.shift;
}

സ്ഥിതം int32_t get_Max_മൂല്യം_From_Shift(സ്ഥിരം IntProperty &prop, UProperty /*which*/) {
    തിരികെ prop.shift;
}

സ്ഥിതം int32_t getBiDiClass(സ്ഥിരം IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ static_cast<int32_t>(u_charDirection(c));
}

സ്ഥിതം int32_t getBiDiPairedBracketType(സ്ഥിരം IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

സ്ഥിതം int32_t bi_Di_Get_Max_മൂല്യം(സ്ഥിരം IntProperty &/*prop*/, UProperty which) {
    തിരികെ ubidi_get_Max_മൂല്യം(which);
}

സ്ഥിതം int32_t getBlock(സ്ഥിരം IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ static_cast<int32_t>(ublock_getCode(c));
}

സ്ഥിതം int32_t block_Get_Max_മൂല്യം(സ്ഥിരം IntProperty &/*prop*/, UProperty /*which*/) {
    തിരികെ uprv_get_Max_മൂല്യങ്ങൾ(UPROPS_MAX_മൂല്യങ്ങൾ_OTHER_INDEX) & UPROPS_MAX_BLOCK;
}

#എങ്കിൽ UCONFIG_NO_NORMALIZATION
സ്ഥിതം int32_t getCombiningClass(സ്ഥിരം IntProperty &, UChar32, UProperty) {
    തിരികെ 0;
}
#അല്ലെങ്കിൽ
സ്ഥിതം int32_t getCombiningClass(സ്ഥിരം IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ u_getCombiningClass(c);
}
#endif

സ്ഥിതം int32_t getGeneralCategory(സ്ഥിരം IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ static_cast<int32_t>(u_charType(c));
}

സ്ഥിതം int32_t getJoiningGroup(സ്ഥിരം IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ ubidi_getJoiningGroup(c);
}

സ്ഥിതം int32_t getJoiningType(സ്ഥിരം IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ ubidi_getJoiningType(c);
}

സ്ഥിതം int32_t getNumericType(സ്ഥിരം IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_മൂല്യം(u_getMainProperties(c)));
    തിരികെ UPROPS_NTV_GET_TYPE(ntv);
}

സ്ഥിതം int32_t getScript(സ്ഥിരം IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    തിരികെ static_cast<int32_t>(uscript_getScript(c, &errorCode));
}

സ്ഥിതം int32_t script_Get_Max_മൂല്യം(സ്ഥിരം IntProperty &/*prop*/, UProperty /*which*/) {
    തിരികെ uprv_get_Max_മൂല്യങ്ങൾ(0)&UPROPS_MAX_SCRIPT;
}

/*
 * Map some of the Grapheme Cluster Break values to Hangul Syllable Types.
 * Hangul_Syllable_Type used to be fully redundant with a subset of Grapheme_Cluster_Break.
 *
 * Starting with Unicode 16, this is no longer true for HST=V vs. GCB=V in some cases:
 * Some Kirat Rai vowels are given GCB=V for proper grapheme clustering, but
 * they are of course not related to Hangul syllables.
 */
സ്ഥിതം സ്ഥിരം UHangulSyllableType gcbToHst[]={
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

സ്ഥിതം int32_t getHangulSyllableType(സ്ഥിരം IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Ignore supplementary code points: They all have HST=NA.
    // This is a simple way to handle the GCB!=hst cases since Unicode 16 (Kirat Rai vowels).
    എങ്കിൽ(c>0xffff) {
        തിരികെ U_HST_NOT_APPLICABLE;
    }
    /* see comments on gcbToHst[] above */
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_MASK) >> UPROPS_GCB_SHIFT;
    എങ്കിൽ(gcb<UPRV_LENGTHOF(gcbToHst)) {
        തിരികെ gcbToHst[gcb];
    } അല്ലെങ്കിൽ {
        തിരികെ U_HST_NOT_APPLICABLE;
    }
}

#എങ്കിൽ UCONFIG_NO_NORMALIZATION
സ്ഥിതം int32_t getNormQuickCheck(സ്ഥിരം IntProperty &, UChar32, UProperty) {
    തിരികെ 0;
}
#അല്ലെങ്കിൽ
സ്ഥിതം int32_t getNormQuickCheck(സ്ഥിരം IntProperty &/*prop*/, UChar32 c, UProperty which) {
    തിരികെ static_cast<int32_t>(unorm_getQuickCheck(c, static_cast<UNormalizationMode>(which - UCHAR_NFD_QUICK_CHECK + UNORM_NFD)));
}
#endif

#എങ്കിൽ UCONFIG_NO_NORMALIZATION
സ്ഥിതം int32_t getLeadCombiningClass(സ്ഥിരം IntProperty &, UChar32, UProperty) {
    തിരികെ 0;
}
#അല്ലെങ്കിൽ
സ്ഥിതം int32_t getLeadCombiningClass(സ്ഥിരം IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ unorm_getFCD16(c)>>8;
}
#endif

#എങ്കിൽ UCONFIG_NO_NORMALIZATION
സ്ഥിതം int32_t getTrailCombiningClass(സ്ഥിരം IntProperty &, UChar32, UProperty) {
    തിരികെ 0;
}
#അല്ലെങ്കിൽ
സ്ഥിതം int32_t getTrailCombiningClass(സ്ഥിരം IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    തിരികെ unorm_getFCD16(c)&0xff;
}
#endif

സ്ഥിതം int32_t getInPC(സ്ഥിരം IntProperty &, UChar32 c, UProperty) {
    തിരികെ ulayout_ensureData() && gInpcTrie != nullptr ? ucptrie_get(gInpcTrie, c) : 0;
}

സ്ഥിതം int32_t getInSC(സ്ഥിരം IntProperty &, UChar32 c, UProperty) {
    തിരികെ ulayout_ensureData() && gInscTrie != nullptr ? ucptrie_get(gInscTrie, c) : 0;
}

സ്ഥിതം int32_t getVo(സ്ഥിരം IntProperty &, UChar32 c, UProperty) {
    തിരികെ ulayout_ensureData() && gVoTrie != nullptr ? ucptrie_get(gVoTrie, c) : 0;
}

സ്ഥിതം int32_t layout_Get_Max_മൂല്യം(സ്ഥിരം IntProperty &/*prop*/, UProperty which) {
    എങ്കിൽ (!ulayout_ensureData()) { തിരികെ 0; }
    തിരഞ്ഞെടുപ്പ് (which) {
    അവസ്ഥ UCHAR_INDIC_POSITIONAL_CATEGORY:
        തിരികെ g_Max_Inpc_മൂല്യം;
    അവസ്ഥ UCHAR_INDIC_SYLLABIC_CATEGORY:
        തിരികെ g_Max_Insc_മൂല്യം;
    അവസ്ഥ UCHAR_VERTICAL_ORIENTATION:
        തിരികെ g_Max_Vo_മൂല്യം;
    സ്ഥിരസ്ഥിതി:
        തിരികെ 0;
    }
}

സ്ഥിതം int32_t get_IDStatus_മൂല്യം(സ്ഥിരം IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    തിരികെ value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_STATUS_ALLOWED : U_ID_STATUS_RESTRICTED;
}

സ്ഥിതം സ്ഥിരം IntProperty intProps[UCHAR_INT_LIMIT-UCHAR_INT_START]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, bi_Di_Get_Max_മൂല്യം },
    { UPROPS_SRC_BLOCK, 0, 0,                               getBlock, block_Get_Max_മൂല്യം },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, get_Max_മൂല്യം_From_Shift },
    { 2,                UPROPS_DT_MASK, 0,                  default_Get_മൂല്യം, default_Get_Max_മൂല്യം },
    { 0,                UPROPS_EA_MASK, UPROPS_EA_SHIFT,    default_Get_മൂല്യം, default_Get_Max_മൂല്യം },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_എണ്ണം) - 1, getGeneralCategory, get_Max_മൂല്യം_From_Shift },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningGroup, bi_Di_Get_Max_മൂല്യം },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, bi_Di_Get_Max_മൂല്യം },
    { 2,                UPROPS_LB_MASK, UPROPS_LB_SHIFT,    default_Get_മൂല്യം, default_Get_Max_മൂല്യം },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_എണ്ണം) - 1, getNumericType, get_Max_മൂല്യം_From_Shift },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, script_Get_Max_മൂല്യം },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_എണ്ണം) - 1, getHangulSyllableType, get_Max_മൂല്യം_From_Shift },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, get_Max_മൂല്യം_From_Shift },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, get_Max_മൂല്യം_From_Shift },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, get_Max_മൂല്യം_From_Shift },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, get_Max_മൂല്യം_From_Shift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, get_Max_മൂല്യം_From_Shift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, get_Max_മൂല്യം_From_Shift },
    { 2,                UPROPS_GCB_MASK, UPROPS_GCB_SHIFT,  default_Get_മൂല്യം, default_Get_Max_മൂല്യം },
    { 2,                UPROPS_SB_MASK, UPROPS_SB_SHIFT,    default_Get_മൂല്യം, default_Get_Max_മൂല്യം },
    { 2,                UPROPS_WB_MASK, UPROPS_WB_SHIFT,    default_Get_മൂല്യം, default_Get_Max_മൂല്യം },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, bi_Di_Get_Max_മൂല്യം },
    { UPROPS_SRC_INPC,  0, 0,                               getInPC, layout_Get_Max_മൂല്യം },
    { UPROPS_SRC_INSC,  0, 0,                               getInSC, layout_Get_Max_മൂല്യം },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layout_Get_Max_മൂല്യം },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_STATUS_ALLOWED), get_IDStatus_മൂല്യം, get_Max_മൂല്യം_From_Shift },
    { 0,                UPROPS_INCB_MASK, UPROPS_INCB_SHIFT,default_Get_മൂല്യം, default_Get_Max_മൂല്യം },
};

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_മൂല്യം(UChar32 c, UProperty which) {
    എങ്കിൽ(which<UCHAR_INT_START) {
        എങ്കിൽ(UCHAR_ദ്വിമാന_START<=which && which<UCHAR_ദ്വിമാന_LIMIT) {
            സ്ഥിരം ദ്വിമാന_Property &prop=binProps[which];
            തിരികെ prop.contains(prop, c, which);
        }
    } അല്ലെങ്കിൽ എങ്കിൽ(which<UCHAR_INT_LIMIT) {
        സ്ഥിരം IntProperty &prop=intProps[which-UCHAR_INT_START];
        തിരികെ prop.get_മൂല്യം(prop, c, which);
    } അല്ലെങ്കിൽ എങ്കിൽ(which==UCHAR_GENERAL_CATEGORY_MASK) {
        തിരികെ U_MASK(u_charType(c));
    }
    തിരികെ 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Min_മൂല്യം(UProperty /*which*/) {
    തിരികെ 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Max_മൂല്യം(UProperty which) {
    എങ്കിൽ(which<UCHAR_INT_START) {
        എങ്കിൽ(UCHAR_ദ്വിമാന_START<=which && which<UCHAR_ദ്വിമാന_LIMIT) {
            തിരികെ 1;  // maximum true for all binary properties
        }
    } അല്ലെങ്കിൽ എങ്കിൽ(which<UCHAR_INT_LIMIT) {
        സ്ഥിരം IntProperty &prop=intProps[which-UCHAR_INT_START];
        തിരികെ prop.get_Max_മൂല്യം(prop, which);
    }
    തിരികെ -1;  // undefined
}

U_CFUNC UPropertySource U_EXPORT2
uprops_getSource(UProperty which) {
    എങ്കിൽ(which<UCHAR_ദ്വിമാന_START) {
        തിരികെ UPROPS_SRC_NONE; /* undefined */
    } അല്ലെങ്കിൽ എങ്കിൽ(which<UCHAR_ദ്വിമാന_LIMIT) {
        സ്ഥിരം ദ്വിമാന_Property &prop=binProps[which];
        എങ്കിൽ(prop.mask!=0) {
            തിരികെ UPROPS_SRC_PROPSVEC;
        } അല്ലെങ്കിൽ {
            തിരികെ (UPropertySource)prop.column;
        }
    } അല്ലെങ്കിൽ എങ്കിൽ(which<UCHAR_INT_START) {
        തിരികെ UPROPS_SRC_NONE; /* undefined */
    } അല്ലെങ്കിൽ എങ്കിൽ(which<UCHAR_INT_LIMIT) {
        സ്ഥിരം IntProperty &prop=intProps[which-UCHAR_INT_START];
        എങ്കിൽ(prop.mask!=0) {
            തിരികെ UPROPS_SRC_PROPSVEC;
        } അല്ലെങ്കിൽ {
            തിരികെ (UPropertySource)prop.column;
        }
    } അല്ലെങ്കിൽ എങ്കിൽ(which<UCHAR_STRING_START) {
        തിരഞ്ഞെടുപ്പ്(which) {
        അവസ്ഥ UCHAR_GENERAL_CATEGORY_MASK:
        അവസ്ഥ UCHAR_NUMERIC_മൂല്യം:
            തിരികെ UPROPS_SRC_CHAR;

        സ്ഥിരസ്ഥിതി:
            തിരികെ UPROPS_SRC_NONE;
        }
    } അല്ലെങ്കിൽ എങ്കിൽ(which<UCHAR_STRING_LIMIT) {
        തിരഞ്ഞെടുപ്പ്(which) {
        അവസ്ഥ UCHAR_AGE:
            തിരികെ UPROPS_SRC_PROPSVEC;

        അവസ്ഥ UCHAR_BIDI_MIRRORING_GLYPH:
            തിരികെ UPROPS_SRC_BIDI;

        അവസ്ഥ UCHAR_CASE_FOLDING:
        അവസ്ഥ UCHAR_LOWERCASE_MAPPING:
        അവസ്ഥ UCHAR_SIMPLE_CASE_FOLDING:
        അവസ്ഥ UCHAR_SIMPLE_LOWERCASE_MAPPING:
        അവസ്ഥ UCHAR_SIMPLE_TITLECASE_MAPPING:
        അവസ്ഥ UCHAR_SIMPLE_UPPERCASE_MAPPING:
        അവസ്ഥ UCHAR_TITLECASE_MAPPING:
        അവസ്ഥ UCHAR_UPPERCASE_MAPPING:
            തിരികെ UPROPS_SRC_CASE;

        അവസ്ഥ UCHAR_ISO_COMMENT:
        അവസ്ഥ UCHAR_NAME:
        അവസ്ഥ UCHAR_UNICODE_1_NAME:
            തിരികെ UPROPS_SRC_NAMES;

        സ്ഥിരസ്ഥിതി:
            തിരികെ UPROPS_SRC_NONE;
        }
    } അല്ലെങ്കിൽ {
        തിരഞ്ഞെടുപ്പ്(which) {
        അവസ്ഥ UCHAR_SCRIPT_EXTENSIONS:
        അവസ്ഥ UCHAR_IDENTIFIER_TYPE:
            തിരികെ UPROPS_SRC_PROPSVEC;
        സ്ഥിരസ്ഥിതി:
            തിരികെ UPROPS_SRC_NONE; /* undefined */
        }
    }
}

U_CFUNC ശൂന്യം U_EXPORT2
uprops_addPropertyStarts(UPropertySource src, സ്ഥിരം USetAdder *sa, UErrorCode *pErrorCode) {
    എങ്കിൽ (U_FAILURE(*pErrorCode)) { തിരികെ; }
    എങ്കിൽ (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        വേണ്ടി (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->add(sa->set, c);
        }
        // single characters
        വേണ്ടി (UChar32 c : ID_COMPAT_MATH_START) {
            sa->add(sa->set, c);
            sa->add(sa->set, c + 1);
        }
        തിരികെ;
    }
    എങ്കിൽ (src == UPROPS_SRC_MCM) {
        // range limits
        വേണ്ടി (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->add(sa->set, c);
        }
        തിരികെ;
    }
    എങ്കിൽ (!ulayout_ensureData(*pErrorCode)) { തിരികെ; }
    സ്ഥിരം UCPTrie *trie;
    തിരഞ്ഞെടുപ്പ് (src) {
    അവസ്ഥ UPROPS_SRC_INPC:
        trie = gInpcTrie;
        നിർത്തുക;
    അവസ്ഥ UPROPS_SRC_INSC:
        trie = gInscTrie;
        നിർത്തുക;
    അവസ്ഥ UPROPS_SRC_VO:
        trie = gVoTrie;
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    എങ്കിൽ (trie == nullptr) {
        *pErrorCode = U_MISSING_RESOURCE_ERROR;
        തിരികെ;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    ആകുമ്പോൾ ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, nullptr)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

U_CAPI തർക്കം U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t typeIndex = type;  // also guards against negative type integers
    എങ്കിൽ (typeIndex >= UPRV_LENGTHOF(uprops_idTypeToEncoded)) {
        തിരികെ അസത്യം;
    }
    uint32_t encodedType = uprops_idTypeToEncoded[typeIndex];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    എങ്കിൽ ((encodedType & UPROPS_ID_TYPE_BIT) != 0) {
        തിരികെ value < UPROPS_ID_TYPE_FORBIDDEN && (value & encodedType) != 0;
    } അല്ലെങ്കിൽ {
        തിരികെ value == encodedType;
    }
}

നാമാകാശ {

ശൂന്യം maybeAppendType(uint32_t value, uint32_t bit, UIdentifierType t,
                     UIdentifierType *types, int32_t &length, int32_t capacity) {
    എങ്കിൽ ((value & bit) != 0) {
        എങ്കിൽ (length < capacity) {
            types[length] = t;
        }
        ++length;
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UErrorCode *pErrorCode) {
    എങ്കിൽ (U_FAILURE(*pErrorCode)) { തിരികെ 0; }
    എങ്കിൽ (capacity < 0 || (capacity > 0 && types == nullptr)) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    എങ്കിൽ ((value & UPROPS_ID_TYPE_FORBIDDEN) == UPROPS_ID_TYPE_FORBIDDEN ||
            value == UPROPS_ID_TYPE_NOT_CHARACTER) {
        // single value
        എങ്കിൽ (capacity > 0) {
            UIdentifierType t;
            തിരഞ്ഞെടുപ്പ് (value) {
                അവസ്ഥ UPROPS_ID_TYPE_NOT_CHARACTER: t = U_ID_TYPE_NOT_CHARACTER; നിർത്തുക;
                അവസ്ഥ UPROPS_ID_TYPE_DEPRECATED: t = U_ID_TYPE_DEPRECATED; നിർത്തുക;
                അവസ്ഥ UPROPS_ID_TYPE_DEFAULT_IGNORABLE: t = U_ID_TYPE_DEFAULT_IGNORABLE; നിർത്തുക;
                അവസ്ഥ UPROPS_ID_TYPE_NOT_NFKC: t = U_ID_TYPE_NOT_NFKC; നിർത്തുക;
                അവസ്ഥ UPROPS_ID_TYPE_INCLUSION: t = U_ID_TYPE_INCLUSION; നിർത്തുക;
                അവസ്ഥ UPROPS_ID_TYPE_RECOMMENDED: t = U_ID_TYPE_RECOMMENDED; നിർത്തുക;
                സ്ഥിരസ്ഥിതി:
                    *pErrorCode = U_INVALID_FORMAT_ERROR;
                    തിരികെ 0;
            }
            types[0] = t;
        } അല്ലെങ്കിൽ {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        തിരികെ 1;
    } അല്ലെങ്കിൽ {
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
        എങ്കിൽ (length >= capacity) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        തിരികെ length;
    }
}

#എങ്കിൽ !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_getFC_NFKC_Closure(UChar32 c, char16_t *dest, int32_t destCapacity, UErrorCode *pErrorCode) {
    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    സ്ഥിരം Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*pErrorCode);
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    സ്ഥിരം char16_t *folded1;
    int32_t folded1Length=ucase_toFullFolding(c, &folded1, U_FOLD_CASE_DEFAULT);
    എങ്കിൽ(folded1Length<0) {
        സ്ഥിരം Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        എങ്കിൽ(nfkcImpl->getCompQuickCheck(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            തിരികെ u_terminateUChars(dest, destCapacity, 0, pErrorCode);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.setTo(c);
    } അല്ലെങ്കിൽ {
        എങ്കിൽ(folded1Length>UCASE_MAX_STRING_LENGTH) {
            folded1String.setTo(folded1Length);
        } അല്ലെങ്കിൽ {
            folded1String.setTo(അസത്യം, folded1, folded1Length);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *pErrorCode);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *pErrorCode);
    // if (c != b) add the mapping from a to c
    എങ്കിൽ(U_FAILURE(*pErrorCode) || kc1==kc2) {
        തിരികെ u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    } അല്ലെങ്കിൽ {
        തിരികെ kc2.extract(dest, destCapacity, *pErrorCode);
    }
}

#endif
