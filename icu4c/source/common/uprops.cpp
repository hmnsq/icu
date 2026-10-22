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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/uchar.h"
#அந்தர்பாவ "unicode/ucptrie.h"
#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "unicode/unorm2.h"
#அந்தர்பாவ "unicode/uscript.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "emojiprops.h"
#அந்தர்பாவ "mutex.h"
#அந்தர்பாவ "normalizer2impl.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "ubidi_props.h"
#அந்தர்பாவ "uprops.h"
#அந்தர்பாவ "ucase.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "ulayout_props.h"
#அந்தர்பாவ "ustr_imp.h"

U_NAMESPACE_USE

// Unicode text layout properties data -----------------------------------------

நாமாகாஶ {

icu::UInitOnce gLayoutInitOnce {};
UDataMemory *gLayoutMemory = nullptr;

UCPTrie *gInpcTrie = nullptr;  // Indic_Positional_Category
UCPTrie *gInscTrie = nullptr;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = nullptr;  // Vertical_Orientation

int32_t g_Max_Inpc_மதிப்பு = 0;
int32_t g_Max_Insc_மதிப்பு = 0;
int32_t g_Max_Vo_மதிப்பு = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(gLayoutMemory);
    gLayoutMemory = nullptr;

    ucptrie_close(gInpcTrie);
    gInpcTrie = nullptr;
    ucptrie_close(gInscTrie);
    gInscTrie = nullptr;
    ucptrie_close(gVoTrie);
    gVoTrie = nullptr;

    g_Max_Inpc_மதிப்பு = 0;
    g_Max_Insc_மதிப்பு = 0;
    g_Max_Vo_மதிப்பு = 0;

    gLayoutInitOnce.reset();
    திரும்பு உண்மை;
}

UBool U_CALLCONV
ulayout_isAcceptable(வெறுமை * /*context*/,
                     மாறிலி எழுத்து * /* type */, மாறிலி எழுத்து * /*name*/,
                     மாறிலி UDataInfo *pInfo) {
    திரும்பு pInfo->size >= 20 &&
        pInfo->isBigEndian == U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily == U_CHARSET_FAMILY &&
        pInfo->dataFormat[0] == ULAYOUT_FMT_0 &&
        pInfo->dataFormat[1] == ULAYOUT_FMT_1 &&
        pInfo->dataFormat[2] == ULAYOUT_FMT_2 &&
        pInfo->dataFormat[3] == ULAYOUT_FMT_3 &&
        pInfo->formatVersion[0] == 1;
}

// UInitOnce singleton initialization function
வெறுமை U_CALLCONV ulayout_load(UErrorCode &errorCode) {
    gLayoutMemory = udata_openChoice(
        nullptr, ULAYOUT_DATA_TYPE, ULAYOUT_DATA_NAME,
        ulayout_isAcceptable, nullptr, &errorCode);
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }

    மாறிலி uint8_t* inBytes = static_cast<மாறிலி uint8_t*>(udata_getMemory(gLayoutMemory));
    மாறிலி int32_t* inIndexes = reinterpret_cast<மாறிலி int32_t*>(inBytes);
    int32_t indexesLength = inIndexes[ULAYOUT_IX_INDEXES_LENGTH];
    எனில் (indexesLength < 12) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        திரும்பு;
    }
    int32_t offset = indexesLength * 4;
    int32_t உச்சி = inIndexes[ULAYOUT_IX_INPC_TRIE_உச்சி];
    int32_t trieSize = உச்சி - offset;
    எனில் (trieSize >= 16) {
        gInpcTrie = ucptrie_open_From_இருமம்(
            UCPTRIE_TYPE_ANY, UCPTRIE_மதிப்பு_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }
    offset = உச்சி;
    உச்சி = inIndexes[ULAYOUT_IX_INSC_TRIE_உச்சி];
    trieSize = உச்சி - offset;
    எனில் (trieSize >= 16) {
        gInscTrie = ucptrie_open_From_இருமம்(
            UCPTRIE_TYPE_ANY, UCPTRIE_மதிப்பு_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }
    offset = உச்சி;
    உச்சி = inIndexes[ULAYOUT_IX_VO_TRIE_உச்சி];
    trieSize = உச்சி - offset;
    எனில் (trieSize >= 16) {
        gVoTrie = ucptrie_open_From_இருமம்(
            UCPTRIE_TYPE_ANY, UCPTRIE_மதிப்பு_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }

    uint32_t max_மதிப்புகள் = inIndexes[ULAYOUT_IX_MAX_மதிப்புகள்];
    g_Max_Inpc_மதிப்பு = max_மதிப்புகள் >> ULAYOUT_MAX_INPC_SHIFT;
    g_Max_Insc_மதிப்பு = (max_மதிப்புகள் >> ULAYOUT_MAX_INSC_SHIFT) & 0xff;
    g_Max_Vo_மதிப்பு = (max_மதிப்புகள் >> ULAYOUT_MAX_VO_SHIFT) & 0xff;

    ucln_common_registerCleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensureData(UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_load, errorCode);
    திரும்பு U_SUCCESS(errorCode);
}

UBool ulayout_ensureData() {
    UErrorCode errorCode = U_ZERO_ERROR;
    திரும்பு ulayout_ensureData(errorCode);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

கட்டமைப்பு இருமம்_Property;

வகைவரை UBool இருமம்_Property_Contains(மாறிலி இருமம்_Property &prop, UChar32 c, UProperty which);

கட்டமைப்பு இருமம்_Property {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    இருமம்_Property_Contains *contains;
};

நிலையான UBool defaultContains(மாறிலி இருமம்_Property &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    திரும்பு (u_getUnicodeProperties(c, prop.column)&prop.mask)!=0;
}

நிலையான UBool case_இருமம்_Property_Contains(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty which) {
    திரும்பு ucase_has_இருமம்_Property(c, which);
}

நிலையான UBool isBidiControl(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு ubidi_isBidiControl(c);
}

நிலையான UBool isMirrored(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு ubidi_isMirrored(c);
}

நிலையான UBool isJoinControl(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு ubidi_isJoinControl(c);
}

#எனில் UCONFIG_NO_NORMALIZATION
நிலையான UBool hasFullCompositionExclusion(மாறிலி இருமம்_Property &, UChar32, UProperty) {
    திரும்பு பொய்;
}
#இல்லையெனில்
நிலையான UBool hasFullCompositionExclusion(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UErrorCode errorCode=U_ZERO_ERROR;
    மாறிலி Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    திரும்பு U_SUCCESS(errorCode) && impl->isCompNo(impl->getNorm16(c));
}
#endif

// UCHAR_NF*_INERT properties
#எனில் UCONFIG_NO_NORMALIZATION
நிலையான UBool isNormInert(மாறிலி இருமம்_Property &, UChar32, UProperty) {
    திரும்பு பொய்;
}
#இல்லையெனில்
நிலையான UBool isNormInert(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty which) {
    UErrorCode errorCode=U_ZERO_ERROR;
    மாறிலி Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalizationMode>(which - UCHAR_NFD_INERT + UNORM_NFD), errorCode);
    திரும்பு U_SUCCESS(errorCode) && norm2->isInert(c);
}
#endif

#எனில் UCONFIG_NO_NORMALIZATION
நிலையான UBool changesWhenCasefolded(மாறிலி இருமம்_Property &, UChar32, UProperty) {
    திரும்பு பொய்;
}
#இல்லையெனில்
நிலையான UBool changesWhenCasefolded(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UErrorCode errorCode=U_ZERO_ERROR;
    மாறிலி Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(errorCode);
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு பொய்;
    }
    எனில்(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        எனில்(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } இல்லையெனில் எனில்(nfd.length()<=U16_MAX_LENGTH &&
                  nfd.length()==U16_LENGTH(c=nfd.char32At(0))
        ) {
            /* single supplementary code point */
        } இல்லையெனில் {
            c=U_SENTINEL;
        }
    } இல்லையெனில் எனில்(c<0) {
        திரும்பு பொய்;  /* protect against bad input */
    }
    எனில்(c>=0) {
        /* single code point */
        மாறிலி char16_t *resultString;
        திரும்பு ucase_toFullFolding(c, &resultString, U_FOLD_CASE_DEFAULT) >= 0;
    } இல்லையெனில் {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_LENGTH];
        int32_t destLength;
        destLength=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.getBuffer(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &errorCode);
        திரும்பு U_SUCCESS(errorCode) &&
                       0!=u_strCompare(nfd.getBuffer(), nfd.length(),
                                       dest, destLength, பொய்);
    }
}
#endif

#எனில் UCONFIG_NO_NORMALIZATION
நிலையான UBool changesWhenNFKC_Casefolded(மாறிலி இருமம்_Property &, UChar32, UProperty) {
    திரும்பு பொய்;
}
#இல்லையெனில்
நிலையான UBool changesWhenNFKC_Casefolded(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    மாறிலி Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(errorCode);
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு பொய்;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        ReorderingBuffer buffer(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        எனில்(buffer.init(5, errorCode)) {
            மாறிலி char16_t *srcArray=src.getBuffer();
            kcf->compose(srcArray, srcArray+src.length(), பொய்,
                          உண்மை, buffer, errorCode);
        }
    }
    திரும்பு U_SUCCESS(errorCode) && dest!=src;
}
#endif

#எனில் UCONFIG_NO_NORMALIZATION
நிலையான UBool isCanonSegmentStarter(மாறிலி இருமம்_Property &, UChar32, UProperty) {
    திரும்பு பொய்;
}
#இல்லையெனில்
நிலையான UBool isCanonSegmentStarter(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    மாறிலி Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    திரும்பு
        U_SUCCESS(errorCode) && impl->ensureCanonIterData(errorCode) &&
        impl->isCanonSegmentStarter(c);
}
#endif

நிலையான UBool isPOSIX_alnum(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு u_isalnumPOSIX(c);
}

நிலையான UBool isPOSIX_blank(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு u_isblank(c);
}

நிலையான UBool isPOSIX_graph(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு u_isgraphPOSIX(c);
}

நிலையான UBool isPOSIX_print(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு u_isprintPOSIX(c);
}

நிலையான UBool isPOSIX_xdigit(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு u_isxdigit(c);
}

நிலையான UBool isRegionalIndicator(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    திரும்பு 0x1F1E6<=c && c<=0x1F1FF;
}

நிலையான UBool hasEmojiProperty(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty which) {
    திரும்பு EmojiProps::has_இருமம்_Property(c, which);
}

நிலையான UBool isIDSUnaryOperator(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // New in Unicode 15.1 for just two characters.
    திரும்பு 0x2FFE<=c && c<=0x2FFF;
}

/** Ranges (start/limit pairs) of ID_Compat_Math_Continue (only), from UCD PropList.txt. */
நிலையான constexpr UChar32 ID_COMPAT_MATH_CONTINUE[] = {
    0x00B2, 0x00B3 + 1,
    0x00B9, 0x00B9 + 1,
    0x2070, 0x2070 + 1,
    0x2074, 0x207E + 1,
    0x2080, 0x208E + 1
};

/** ID_Compat_Math_Start characters, from UCD PropList.txt. */
நிலையான constexpr UChar32 ID_COMPAT_MATH_START[] = {
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
நிலையான constexpr UChar32 MODIFIER_COMBINING_MARK[] = {
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

நிலையான UBool isIDCompatMathStart(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    எனில் (c < ID_COMPAT_MATH_START[0]) { திரும்பு பொய்; }  // fastpath for common scripts
    க்கு (UChar32 startChar : ID_COMPAT_MATH_START) {
        எனில் (c == startChar) { திரும்பு உண்மை; }
    }
    திரும்பு பொய்;
}

நிலையான UBool isIDCompatMathContinue(மாறிலி இருமம்_Property &prop, UChar32 c, UProperty /*which*/) {
    க்கு (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        எனில் (c < ID_COMPAT_MATH_CONTINUE[i]) { திரும்பு பொய்; }  // below range start
        எனில் (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { திரும்பு உண்மை; }  // below range limit
    }
    திரும்பு isIDCompatMathStart(prop, c, UCHAR_ID_COMPAT_MATH_START);
}

நிலையான UBool isModifierCombiningMark(மாறிலி இருமம்_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    க்கு (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        எனில் (c < MODIFIER_COMBINING_MARK[i]) { திரும்பு பொய்; }  // below range start
        எனில் (c < MODIFIER_COMBINING_MARK[i + 1]) { திரும்பு உண்மை; }  // below range limit
    }
    திரும்பு பொய்;
}

நிலையான மாறிலி இருமம்_Property binProps[UCHAR_இருமம்_LIMIT]={
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
    { 1,                U_MASK(UPROPS_IDS_இருமம்_OPERATOR), defaultContains },
    { 1,                U_MASK(UPROPS_IDS_TRINARY_OPERATOR), defaultContains },
    { UPROPS_SRC_BIDI,  0, isJoinControl },
    { 1,                U_MASK(UPROPS_LOGICAL_ORDER_EXCEPTION), defaultContains },
    { UPROPS_SRC_CASE,  0, case_இருமம்_Property_Contains },  // UCHAR_LOWERCASE
    { 1,                U_MASK(UPROPS_MATH), defaultContains },
    { 1,                U_MASK(UPROPS_NONCHARACTER_CODE_POINT), defaultContains },
    { 1,                U_MASK(UPROPS_QUOTATION_MARK), defaultContains },
    { 1,                U_MASK(UPROPS_RADICAL), defaultContains },
    { UPROPS_SRC_CASE,  0, case_இருமம்_Property_Contains },  // UCHAR_SOFT_DOTTED
    { 1,                U_MASK(UPROPS_TERMINAL_PUNCTUATION), defaultContains },
    { 1,                U_MASK(UPROPS_UNIFIED_IDEOGRAPH), defaultContains },
    { UPROPS_SRC_CASE,  0, case_இருமம்_Property_Contains },  // UCHAR_UPPERCASE
    { 1,                U_MASK(UPROPS_WHITE_SPACE), defaultContains },
    { 1,                U_MASK(UPROPS_XID_CONTINUE), defaultContains },
    { 1,                U_MASK(UPROPS_XID_START), defaultContains },
    { UPROPS_SRC_CASE,  0, case_இருமம்_Property_Contains },  // UCHAR_CASE_SENSITIVE
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
    { UPROPS_SRC_CASE,  0, case_இருமம்_Property_Contains },  // UCHAR_CASED
    { UPROPS_SRC_CASE,  0, case_இருமம்_Property_Contains },  // UCHAR_CASE_IGNORABLE
    { UPROPS_SRC_CASE,  0, case_இருமம்_Property_Contains },  // UCHAR_CHANGES_WHEN_LOWERCASED
    { UPROPS_SRC_CASE,  0, case_இருமம்_Property_Contains },  // UCHAR_CHANGES_WHEN_UPPERCASED
    { UPROPS_SRC_CASE,  0, case_இருமம்_Property_Contains },  // UCHAR_CHANGES_WHEN_TITLECASED
    { UPROPS_SRC_CASE_AND_NORM,  0, changesWhenCasefolded },
    { UPROPS_SRC_CASE,  0, case_இருமம்_Property_Contains },  // UCHAR_CHANGES_WHEN_CASEMAPPED
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
u_has_இருமம்_Property(UChar32 c, UProperty which) {
    /* c is range-checked in the functions that are called from here */
    எனில்(which<UCHAR_இருமம்_START || UCHAR_இருமம்_LIMIT<=which) {
        /* not a known binary property */
        திரும்பு பொய்;
    } இல்லையெனில் {
        மாறிலி இருமம்_Property &prop=binProps[which];
        திரும்பு prop.contains(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    திரும்பு u_has_இருமம்_Property(c, UCHAR_ID_START);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    திரும்பு u_has_இருமம்_Property(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_string_Has_இருமம்_Property(மாறிலி char16_t *s, int32_t length, UProperty which) {
    எனில் (s == nullptr && length != 0) { திரும்பு பொய்; }
    எனில் (length == 1) {
        திரும்பு u_has_இருமம்_Property(s[0], which);  // single code point
    } இல்லையெனில் எனில் (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        எனில் (length > 0 ? i == length : s[i] == 0) {
            திரும்பு u_has_இருமம்_Property(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    திரும்பு UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::has_இருமம்_Property(s, length, which);
}

கட்டமைப்பு IntProperty;

வகைவரை int32_t Int_Property_Get_மதிப்பு(மாறிலி IntProperty &prop, UChar32 c, UProperty which);
வகைவரை int32_t Int_Property_Get_Max_மதிப்பு(மாறிலி IntProperty &prop, UProperty which);

கட்டமைப்பு IntProperty {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t shift;  // =maxValue if getMaxValueFromShift() is used
    Int_Property_Get_மதிப்பு *get_மதிப்பு;
    Int_Property_Get_Max_மதிப்பு *get_Max_மதிப்பு;
};

நிலையான int32_t default_Get_மதிப்பு(மாறிலி IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    திரும்பு static_cast<int32_t>(u_getUnicodeProperties(c, prop.column) & prop.mask) >> prop.shift;
}

நிலையான int32_t default_Get_Max_மதிப்பு(மாறிலி IntProperty &prop, UProperty /*which*/) {
    திரும்பு (uprv_get_Max_மதிப்புகள்(prop.column)&prop.mask)>>prop.shift;
}

நிலையான int32_t get_Max_மதிப்பு_From_Shift(மாறிலி IntProperty &prop, UProperty /*which*/) {
    திரும்பு prop.shift;
}

நிலையான int32_t getBiDiClass(மாறிலி IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு static_cast<int32_t>(u_charDirection(c));
}

நிலையான int32_t getBiDiPairedBracketType(மாறிலி IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

நிலையான int32_t bi_Di_Get_Max_மதிப்பு(மாறிலி IntProperty &/*prop*/, UProperty which) {
    திரும்பு ubidi_get_Max_மதிப்பு(which);
}

நிலையான int32_t getBlock(மாறிலி IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு static_cast<int32_t>(ublock_getCode(c));
}

நிலையான int32_t block_Get_Max_மதிப்பு(மாறிலி IntProperty &/*prop*/, UProperty /*which*/) {
    திரும்பு uprv_get_Max_மதிப்புகள்(UPROPS_MAX_மதிப்புகள்_OTHER_INDEX) & UPROPS_MAX_BLOCK;
}

#எனில் UCONFIG_NO_NORMALIZATION
நிலையான int32_t getCombiningClass(மாறிலி IntProperty &, UChar32, UProperty) {
    திரும்பு 0;
}
#இல்லையெனில்
நிலையான int32_t getCombiningClass(மாறிலி IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு u_getCombiningClass(c);
}
#endif

நிலையான int32_t getGeneralCategory(மாறிலி IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு static_cast<int32_t>(u_charType(c));
}

நிலையான int32_t getJoiningGroup(மாறிலி IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு ubidi_getJoiningGroup(c);
}

நிலையான int32_t getJoiningType(மாறிலி IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு ubidi_getJoiningType(c);
}

நிலையான int32_t getNumericType(மாறிலி IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_மதிப்பு(u_getMainProperties(c)));
    திரும்பு UPROPS_NTV_GET_TYPE(ntv);
}

நிலையான int32_t getScript(மாறிலி IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    திரும்பு static_cast<int32_t>(uscript_getScript(c, &errorCode));
}

நிலையான int32_t script_Get_Max_மதிப்பு(மாறிலி IntProperty &/*prop*/, UProperty /*which*/) {
    திரும்பு uprv_get_Max_மதிப்புகள்(0)&UPROPS_MAX_SCRIPT;
}

/*
 * Map some of the Grapheme Cluster Break values to Hangul Syllable Types.
 * Hangul_Syllable_Type used to be fully redundant with a subset of Grapheme_Cluster_Break.
 *
 * Starting with Unicode 16, this is no longer true for HST=V vs. GCB=V in some cases:
 * Some Kirat Rai vowels are given GCB=V for proper grapheme clustering, but
 * they are of course not related to Hangul syllables.
 */
நிலையான மாறிலி UHangulSyllableType gcbToHst[]={
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

நிலையான int32_t getHangulSyllableType(மாறிலி IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Ignore supplementary code points: They all have HST=NA.
    // This is a simple way to handle the GCB!=hst cases since Unicode 16 (Kirat Rai vowels).
    எனில்(c>0xffff) {
        திரும்பு U_HST_NOT_APPLICABLE;
    }
    /* see comments on gcbToHst[] above */
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_MASK) >> UPROPS_GCB_SHIFT;
    எனில்(gcb<UPRV_LENGTHOF(gcbToHst)) {
        திரும்பு gcbToHst[gcb];
    } இல்லையெனில் {
        திரும்பு U_HST_NOT_APPLICABLE;
    }
}

#எனில் UCONFIG_NO_NORMALIZATION
நிலையான int32_t getNormQuickCheck(மாறிலி IntProperty &, UChar32, UProperty) {
    திரும்பு 0;
}
#இல்லையெனில்
நிலையான int32_t getNormQuickCheck(மாறிலி IntProperty &/*prop*/, UChar32 c, UProperty which) {
    திரும்பு static_cast<int32_t>(unorm_getQuickCheck(c, static_cast<UNormalizationMode>(which - UCHAR_NFD_QUICK_CHECK + UNORM_NFD)));
}
#endif

#எனில் UCONFIG_NO_NORMALIZATION
நிலையான int32_t getLeadCombiningClass(மாறிலி IntProperty &, UChar32, UProperty) {
    திரும்பு 0;
}
#இல்லையெனில்
நிலையான int32_t getLeadCombiningClass(மாறிலி IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு unorm_getFCD16(c)>>8;
}
#endif

#எனில் UCONFIG_NO_NORMALIZATION
நிலையான int32_t getTrailCombiningClass(மாறிலி IntProperty &, UChar32, UProperty) {
    திரும்பு 0;
}
#இல்லையெனில்
நிலையான int32_t getTrailCombiningClass(மாறிலி IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    திரும்பு unorm_getFCD16(c)&0xff;
}
#endif

நிலையான int32_t getInPC(மாறிலி IntProperty &, UChar32 c, UProperty) {
    திரும்பு ulayout_ensureData() && gInpcTrie != nullptr ? ucptrie_get(gInpcTrie, c) : 0;
}

நிலையான int32_t getInSC(மாறிலி IntProperty &, UChar32 c, UProperty) {
    திரும்பு ulayout_ensureData() && gInscTrie != nullptr ? ucptrie_get(gInscTrie, c) : 0;
}

நிலையான int32_t getVo(மாறிலி IntProperty &, UChar32 c, UProperty) {
    திரும்பு ulayout_ensureData() && gVoTrie != nullptr ? ucptrie_get(gVoTrie, c) : 0;
}

நிலையான int32_t layout_Get_Max_மதிப்பு(மாறிலி IntProperty &/*prop*/, UProperty which) {
    எனில் (!ulayout_ensureData()) { திரும்பு 0; }
    தேர்வு (which) {
    நிலை UCHAR_INDIC_POSITIONAL_CATEGORY:
        திரும்பு g_Max_Inpc_மதிப்பு;
    நிலை UCHAR_INDIC_SYLLABIC_CATEGORY:
        திரும்பு g_Max_Insc_மதிப்பு;
    நிலை UCHAR_VERTICAL_ORIENTATION:
        திரும்பு g_Max_Vo_மதிப்பு;
    இயல்பு:
        திரும்பு 0;
    }
}

நிலையான int32_t get_IDStatus_மதிப்பு(மாறிலி IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    திரும்பு value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_STATUS_ALLOWED : U_ID_STATUS_RESTRICTED;
}

நிலையான மாறிலி IntProperty intProps[UCHAR_INT_LIMIT-UCHAR_INT_START]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, bi_Di_Get_Max_மதிப்பு },
    { UPROPS_SRC_BLOCK, 0, 0,                               getBlock, block_Get_Max_மதிப்பு },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, get_Max_மதிப்பு_From_Shift },
    { 2,                UPROPS_DT_MASK, 0,                  default_Get_மதிப்பு, default_Get_Max_மதிப்பு },
    { 0,                UPROPS_EA_MASK, UPROPS_EA_SHIFT,    default_Get_மதிப்பு, default_Get_Max_மதிப்பு },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_மொத்தம்) - 1, getGeneralCategory, get_Max_மதிப்பு_From_Shift },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningGroup, bi_Di_Get_Max_மதிப்பு },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, bi_Di_Get_Max_மதிப்பு },
    { 2,                UPROPS_LB_MASK, UPROPS_LB_SHIFT,    default_Get_மதிப்பு, default_Get_Max_மதிப்பு },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_மொத்தம்) - 1, getNumericType, get_Max_மதிப்பு_From_Shift },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, script_Get_Max_மதிப்பு },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_மொத்தம்) - 1, getHangulSyllableType, get_Max_மதிப்பு_From_Shift },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, get_Max_மதிப்பு_From_Shift },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, get_Max_மதிப்பு_From_Shift },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, get_Max_மதிப்பு_From_Shift },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, get_Max_மதிப்பு_From_Shift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, get_Max_மதிப்பு_From_Shift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, get_Max_மதிப்பு_From_Shift },
    { 2,                UPROPS_GCB_MASK, UPROPS_GCB_SHIFT,  default_Get_மதிப்பு, default_Get_Max_மதிப்பு },
    { 2,                UPROPS_SB_MASK, UPROPS_SB_SHIFT,    default_Get_மதிப்பு, default_Get_Max_மதிப்பு },
    { 2,                UPROPS_WB_MASK, UPROPS_WB_SHIFT,    default_Get_மதிப்பு, default_Get_Max_மதிப்பு },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, bi_Di_Get_Max_மதிப்பு },
    { UPROPS_SRC_INPC,  0, 0,                               getInPC, layout_Get_Max_மதிப்பு },
    { UPROPS_SRC_INSC,  0, 0,                               getInSC, layout_Get_Max_மதிப்பு },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layout_Get_Max_மதிப்பு },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_STATUS_ALLOWED), get_IDStatus_மதிப்பு, get_Max_மதிப்பு_From_Shift },
    { 0,                UPROPS_INCB_MASK, UPROPS_INCB_SHIFT,default_Get_மதிப்பு, default_Get_Max_மதிப்பு },
};

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_மதிப்பு(UChar32 c, UProperty which) {
    எனில்(which<UCHAR_INT_START) {
        எனில்(UCHAR_இருமம்_START<=which && which<UCHAR_இருமம்_LIMIT) {
            மாறிலி இருமம்_Property &prop=binProps[which];
            திரும்பு prop.contains(prop, c, which);
        }
    } இல்லையெனில் எனில்(which<UCHAR_INT_LIMIT) {
        மாறிலி IntProperty &prop=intProps[which-UCHAR_INT_START];
        திரும்பு prop.get_மதிப்பு(prop, c, which);
    } இல்லையெனில் எனில்(which==UCHAR_GENERAL_CATEGORY_MASK) {
        திரும்பு U_MASK(u_charType(c));
    }
    திரும்பு 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Min_மதிப்பு(UProperty /*which*/) {
    திரும்பு 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Max_மதிப்பு(UProperty which) {
    எனில்(which<UCHAR_INT_START) {
        எனில்(UCHAR_இருமம்_START<=which && which<UCHAR_இருமம்_LIMIT) {
            திரும்பு 1;  // maximum true for all binary properties
        }
    } இல்லையெனில் எனில்(which<UCHAR_INT_LIMIT) {
        மாறிலி IntProperty &prop=intProps[which-UCHAR_INT_START];
        திரும்பு prop.get_Max_மதிப்பு(prop, which);
    }
    திரும்பு -1;  // undefined
}

U_CFUNC UPropertySource U_EXPORT2
uprops_getSource(UProperty which) {
    எனில்(which<UCHAR_இருமம்_START) {
        திரும்பு UPROPS_SRC_NONE; /* undefined */
    } இல்லையெனில் எனில்(which<UCHAR_இருமம்_LIMIT) {
        மாறிலி இருமம்_Property &prop=binProps[which];
        எனில்(prop.mask!=0) {
            திரும்பு UPROPS_SRC_PROPSVEC;
        } இல்லையெனில் {
            திரும்பு (UPropertySource)prop.column;
        }
    } இல்லையெனில் எனில்(which<UCHAR_INT_START) {
        திரும்பு UPROPS_SRC_NONE; /* undefined */
    } இல்லையெனில் எனில்(which<UCHAR_INT_LIMIT) {
        மாறிலி IntProperty &prop=intProps[which-UCHAR_INT_START];
        எனில்(prop.mask!=0) {
            திரும்பு UPROPS_SRC_PROPSVEC;
        } இல்லையெனில் {
            திரும்பு (UPropertySource)prop.column;
        }
    } இல்லையெனில் எனில்(which<UCHAR_STRING_START) {
        தேர்வு(which) {
        நிலை UCHAR_GENERAL_CATEGORY_MASK:
        நிலை UCHAR_NUMERIC_மதிப்பு:
            திரும்பு UPROPS_SRC_CHAR;

        இயல்பு:
            திரும்பு UPROPS_SRC_NONE;
        }
    } இல்லையெனில் எனில்(which<UCHAR_STRING_LIMIT) {
        தேர்வு(which) {
        நிலை UCHAR_AGE:
            திரும்பு UPROPS_SRC_PROPSVEC;

        நிலை UCHAR_BIDI_MIRRORING_GLYPH:
            திரும்பு UPROPS_SRC_BIDI;

        நிலை UCHAR_CASE_FOLDING:
        நிலை UCHAR_LOWERCASE_MAPPING:
        நிலை UCHAR_SIMPLE_CASE_FOLDING:
        நிலை UCHAR_SIMPLE_LOWERCASE_MAPPING:
        நிலை UCHAR_SIMPLE_TITLECASE_MAPPING:
        நிலை UCHAR_SIMPLE_UPPERCASE_MAPPING:
        நிலை UCHAR_TITLECASE_MAPPING:
        நிலை UCHAR_UPPERCASE_MAPPING:
            திரும்பு UPROPS_SRC_CASE;

        நிலை UCHAR_ISO_COMMENT:
        நிலை UCHAR_NAME:
        நிலை UCHAR_UNICODE_1_NAME:
            திரும்பு UPROPS_SRC_NAMES;

        இயல்பு:
            திரும்பு UPROPS_SRC_NONE;
        }
    } இல்லையெனில் {
        தேர்வு(which) {
        நிலை UCHAR_SCRIPT_EXTENSIONS:
        நிலை UCHAR_IDENTIFIER_TYPE:
            திரும்பு UPROPS_SRC_PROPSVEC;
        இயல்பு:
            திரும்பு UPROPS_SRC_NONE; /* undefined */
        }
    }
}

U_CFUNC வெறுமை U_EXPORT2
uprops_addPropertyStarts(UPropertySource src, மாறிலி USetAdder *sa, UErrorCode *pErrorCode) {
    எனில் (U_FAILURE(*pErrorCode)) { திரும்பு; }
    எனில் (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        க்கு (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->add(sa->set, c);
        }
        // single characters
        க்கு (UChar32 c : ID_COMPAT_MATH_START) {
            sa->add(sa->set, c);
            sa->add(sa->set, c + 1);
        }
        திரும்பு;
    }
    எனில் (src == UPROPS_SRC_MCM) {
        // range limits
        க்கு (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->add(sa->set, c);
        }
        திரும்பு;
    }
    எனில் (!ulayout_ensureData(*pErrorCode)) { திரும்பு; }
    மாறிலி UCPTrie *trie;
    தேர்வு (src) {
    நிலை UPROPS_SRC_INPC:
        trie = gInpcTrie;
        நிறுத்து;
    நிலை UPROPS_SRC_INSC:
        trie = gInscTrie;
        நிறுத்து;
    நிலை UPROPS_SRC_VO:
        trie = gVoTrie;
        நிறுத்து;
    இயல்பு:
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    எனில் (trie == nullptr) {
        *pErrorCode = U_MISSING_RESOURCE_ERROR;
        திரும்பு;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    போது ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, nullptr)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

U_CAPI தருக்க U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t typeIndex = type;  // also guards against negative type integers
    எனில் (typeIndex >= UPRV_LENGTHOF(uprops_idTypeToEncoded)) {
        திரும்பு பொய்;
    }
    uint32_t encodedType = uprops_idTypeToEncoded[typeIndex];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    எனில் ((encodedType & UPROPS_ID_TYPE_BIT) != 0) {
        திரும்பு value < UPROPS_ID_TYPE_FORBIDDEN && (value & encodedType) != 0;
    } இல்லையெனில் {
        திரும்பு value == encodedType;
    }
}

நாமாகாஶ {

வெறுமை maybeAppendType(uint32_t value, uint32_t bit, UIdentifierType t,
                     UIdentifierType *types, int32_t &length, int32_t capacity) {
    எனில் ((value & bit) != 0) {
        எனில் (length < capacity) {
            types[length] = t;
        }
        ++length;
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UErrorCode *pErrorCode) {
    எனில் (U_FAILURE(*pErrorCode)) { திரும்பு 0; }
    எனில் (capacity < 0 || (capacity > 0 && types == nullptr)) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    எனில் ((value & UPROPS_ID_TYPE_FORBIDDEN) == UPROPS_ID_TYPE_FORBIDDEN ||
            value == UPROPS_ID_TYPE_NOT_CHARACTER) {
        // single value
        எனில் (capacity > 0) {
            UIdentifierType t;
            தேர்வு (value) {
                நிலை UPROPS_ID_TYPE_NOT_CHARACTER: t = U_ID_TYPE_NOT_CHARACTER; நிறுத்து;
                நிலை UPROPS_ID_TYPE_DEPRECATED: t = U_ID_TYPE_DEPRECATED; நிறுத்து;
                நிலை UPROPS_ID_TYPE_DEFAULT_IGNORABLE: t = U_ID_TYPE_DEFAULT_IGNORABLE; நிறுத்து;
                நிலை UPROPS_ID_TYPE_NOT_NFKC: t = U_ID_TYPE_NOT_NFKC; நிறுத்து;
                நிலை UPROPS_ID_TYPE_INCLUSION: t = U_ID_TYPE_INCLUSION; நிறுத்து;
                நிலை UPROPS_ID_TYPE_RECOMMENDED: t = U_ID_TYPE_RECOMMENDED; நிறுத்து;
                இயல்பு:
                    *pErrorCode = U_INVALID_FORMAT_ERROR;
                    திரும்பு 0;
            }
            types[0] = t;
        } இல்லையெனில் {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        திரும்பு 1;
    } இல்லையெனில் {
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
        எனில் (length >= capacity) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        திரும்பு length;
    }
}

#எனில் !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_getFC_NFKC_Closure(UChar32 c, char16_t *dest, int32_t destCapacity, UErrorCode *pErrorCode) {
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    மாறிலி Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*pErrorCode);
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    மாறிலி char16_t *folded1;
    int32_t folded1Length=ucase_toFullFolding(c, &folded1, U_FOLD_CASE_DEFAULT);
    எனில்(folded1Length<0) {
        மாறிலி Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        எனில்(nfkcImpl->getCompQuickCheck(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            திரும்பு u_terminateUChars(dest, destCapacity, 0, pErrorCode);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.setTo(c);
    } இல்லையெனில் {
        எனில்(folded1Length>UCASE_MAX_STRING_LENGTH) {
            folded1String.setTo(folded1Length);
        } இல்லையெனில் {
            folded1String.setTo(பொய், folded1, folded1Length);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *pErrorCode);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *pErrorCode);
    // if (c != b) add the mapping from a to c
    எனில்(U_FAILURE(*pErrorCode) || kc1==kc2) {
        திரும்பு u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    } இல்லையெனில் {
        திரும்பு kc2.extract(dest, destCapacity, *pErrorCode);
    }
}

#endif
