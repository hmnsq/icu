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
UDataMemory *gLayoutMemory = nullptr;

UCPTrie *gInpcTrie = nullptr;  // Indic_Positional_Category
UCPTrie *gInscTrie = nullptr;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = nullptr;  // Vertical_Orientation

int32_t gMaxInpcValue = 0;
int32_t gMaxInscValue = 0;
int32_t gMaxVoValue = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(gLayoutMemory);
    gLayoutMemory = nullptr;

    ucptrie_close(gInpcTrie);
    gInpcTrie = nullptr;
    ucptrie_close(gInscTrie);
    gInscTrie = nullptr;
    ucptrie_close(gVoTrie);
    gVoTrie = nullptr;

    gMaxInpcValue = 0;
    gMaxInscValue = 0;
    gMaxVoValue = 0;

    gLayoutInitOnce.reset();
    أرجع true;
}

UBool U_CALLCONV
ulayout_isAcceptable(فراغ * /*context*/,
                     ثابت حرف * /* type */, ثابت حرف * /*name*/,
                     ثابت UDataInfo *pInfo) {
    أرجع pInfo->size >= 20 &&
        pInfo->isBigEndian == U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily == U_CHARSET_FAMILY &&
        pInfo->dataFormat[0] == ULAYOUT_FMT_0 &&
        pInfo->dataFormat[1] == ULAYOUT_FMT_1 &&
        pInfo->dataFormat[2] == ULAYOUT_FMT_2 &&
        pInfo->dataFormat[3] == ULAYOUT_FMT_3 &&
        pInfo->formatVersion[0] == 1;
}

// UInitOnce singleton initialization function
فراغ U_CALLCONV ulayout_load(UErrorCode &errorCode) {
    gLayoutMemory = udata_openChoice(
        nullptr, ULAYOUT_DATA_TYPE, ULAYOUT_DATA_NAME,
        ulayout_isAcceptable, nullptr, &errorCode);
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }

    ثابت uint8_t* inBytes = static_cast<ثابت uint8_t*>(udata_getMemory(gLayoutMemory));
    ثابت int32_t* inIndexes = reinterpret_cast<ثابت int32_t*>(inBytes);
    int32_t indexesLength = inIndexes[ULAYOUT_IX_INDEXES_LENGTH];
    𐤀𐤌 (indexesLength < 12) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        أرجع;
    }
    int32_t offset = indexesLength * 4;
    int32_t top = inIndexes[ULAYOUT_IX_INPC_TRIE_TOP];
    int32_t trieSize = top - offset;
    𐤀𐤌 (trieSize >= 16) {
        gInpcTrie = ucptrie_openFromBinary(
            UCPTRIE_TYPE_ANY, UCPTRIE_VALUE_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }
    offset = top;
    top = inIndexes[ULAYOUT_IX_INSC_TRIE_TOP];
    trieSize = top - offset;
    𐤀𐤌 (trieSize >= 16) {
        gInscTrie = ucptrie_openFromBinary(
            UCPTRIE_TYPE_ANY, UCPTRIE_VALUE_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }
    offset = top;
    top = inIndexes[ULAYOUT_IX_VO_TRIE_TOP];
    trieSize = top - offset;
    𐤀𐤌 (trieSize >= 16) {
        gVoTrie = ucptrie_openFromBinary(
            UCPTRIE_TYPE_ANY, UCPTRIE_VALUE_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }

    uint32_t maxValues = inIndexes[ULAYOUT_IX_MAX_VALUES];
    gMaxInpcValue = maxValues >> ULAYOUT_MAX_INPC_SHIFT;
    gMaxInscValue = (maxValues >> ULAYOUT_MAX_INSC_SHIFT) & 0xff;
    gMaxVoValue = (maxValues >> ULAYOUT_MAX_VO_SHIFT) & 0xff;

    ucln_common_registerCleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensureData(UErrorCode &errorCode) {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع خطأ; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_load, errorCode);
    أرجع U_SUCCESS(errorCode);
}

UBool ulayout_ensureData() {
    UErrorCode errorCode = U_ZERO_ERROR;
    أرجع ulayout_ensureData(errorCode);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

بنية BinaryProperty;

تعريفنوع UBool BinaryPropertyContains(ثابت BinaryProperty &prop, UChar32 c, UProperty which);

بنية BinaryProperty {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    BinaryPropertyContains *contains;
};

ساكن UBool defaultContains(ثابت BinaryProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    أرجع (u_getUnicodeProperties(c, prop.column)&prop.mask)!=0;
}

ساكن UBool caseBinaryPropertyContains(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty which) {
    أرجع ucase_hasBinaryProperty(c, which);
}

ساكن UBool isBidiControl(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_isBidiControl(c);
}

ساكن UBool isMirrored(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_isMirrored(c);
}

ساكن UBool isJoinControl(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_isJoinControl(c);
}

#𐤀𐤌 UCONFIG_NO_NORMALIZATION
ساكن UBool hasFullCompositionExclusion(ثابت BinaryProperty &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool hasFullCompositionExclusion(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UErrorCode errorCode=U_ZERO_ERROR;
    ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    أرجع U_SUCCESS(errorCode) && impl->isCompNo(impl->getNorm16(c));
}
#endif

// UCHAR_NF*_INERT properties
#𐤀𐤌 UCONFIG_NO_NORMALIZATION
ساكن UBool isNormInert(ثابت BinaryProperty &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool isNormInert(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty which) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ثابت Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalizationMode>(which - UCHAR_NFD_INERT + UNORM_NFD), errorCode);
    أرجع U_SUCCESS(errorCode) && norm2->isInert(c);
}
#endif

#𐤀𐤌 UCONFIG_NO_NORMALIZATION
ساكن UBool changesWhenCasefolded(ثابت BinaryProperty &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool changesWhenCasefolded(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UErrorCode errorCode=U_ZERO_ERROR;
    ثابت Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(errorCode);
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع خطأ;
    }
    𐤀𐤌(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        𐤀𐤌(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } وإلا 𐤀𐤌(nfd.length()<=U16_MAX_LENGTH &&
                  nfd.length()==U16_LENGTH(c=nfd.char32At(0))
        ) {
            /* single supplementary code point */
        } وإلا {
            c=U_SENTINEL;
        }
    } وإلا 𐤀𐤌(c<0) {
        أرجع خطأ;  /* protect against bad input */
    }
    𐤀𐤌(c>=0) {
        /* single code point */
        ثابت char16_t *resultString;
        أرجع ucase_toFullFolding(c, &resultString, U_FOLD_CASE_DEFAULT) >= 0;
    } وإلا {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_LENGTH];
        int32_t destLength;
        destLength=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.getBuffer(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &errorCode);
        أرجع U_SUCCESS(errorCode) &&
                       0!=u_strCompare(nfd.getBuffer(), nfd.length(),
                                       dest, destLength, خطأ);
    }
}
#endif

#𐤀𐤌 UCONFIG_NO_NORMALIZATION
ساكن UBool changesWhenNFKC_Casefolded(ثابت BinaryProperty &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool changesWhenNFKC_Casefolded(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ثابت Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(errorCode);
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع خطأ;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        ReorderingBuffer buffer(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        𐤀𐤌(buffer.init(5, errorCode)) {
            ثابت char16_t *srcArray=src.getBuffer();
            kcf->compose(srcArray, srcArray+src.length(), خطأ,
                          true, buffer, errorCode);
        }
    }
    أرجع U_SUCCESS(errorCode) && dest!=src;
}
#endif

#𐤀𐤌 UCONFIG_NO_NORMALIZATION
ساكن UBool isCanonSegmentStarter(ثابت BinaryProperty &, UChar32, UProperty) {
    أرجع خطأ;
}
#وإلا
ساكن UBool isCanonSegmentStarter(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    أرجع
        U_SUCCESS(errorCode) && impl->ensureCanonIterData(errorCode) &&
        impl->isCanonSegmentStarter(c);
}
#endif

ساكن UBool isPOSIX_alnum(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isalnumPOSIX(c);
}

ساكن UBool isPOSIX_blank(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isblank(c);
}

ساكن UBool isPOSIX_graph(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isgraphPOSIX(c);
}

ساكن UBool isPOSIX_print(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isprintPOSIX(c);
}

ساكن UBool isPOSIX_xdigit(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_isxdigit(c);
}

ساكن UBool isRegionalIndicator(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    أرجع 0x1F1E6<=c && c<=0x1F1FF;
}

ساكن UBool hasEmojiProperty(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty which) {
    أرجع EmojiProps::hasBinaryProperty(c, which);
}

ساكن UBool isIDSUnaryOperator(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
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
ساكن constexpr UChar32 ID_COMPAT_MATH_START[] = {
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

ساكن UBool isIDCompatMathStart(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    𐤀𐤌 (c < ID_COMPAT_MATH_START[0]) { أرجع خطأ; }  // fastpath for common scripts
    لكل (UChar32 startChar : ID_COMPAT_MATH_START) {
        𐤀𐤌 (c == startChar) { أرجع true; }
    }
    أرجع خطأ;
}

ساكن UBool isIDCompatMathContinue(ثابت BinaryProperty &prop, UChar32 c, UProperty /*which*/) {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        𐤀𐤌 (c < ID_COMPAT_MATH_CONTINUE[i]) { أرجع خطأ; }  // below range start
        𐤀𐤌 (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { أرجع true; }  // below range limit
    }
    أرجع isIDCompatMathStart(prop, c, UCHAR_ID_COMPAT_MATH_START);
}

ساكن UBool isModifierCombiningMark(ثابت BinaryProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        𐤀𐤌 (c < MODIFIER_COMBINING_MARK[i]) { أرجع خطأ; }  // below range start
        𐤀𐤌 (c < MODIFIER_COMBINING_MARK[i + 1]) { أرجع true; }  // below range limit
    }
    أرجع خطأ;
}

ساكن ثابت BinaryProperty binProps[UCHAR_BINARY_LIMIT]={
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
    { 1,                U_MASK(UPROPS_IDS_BINARY_OPERATOR), defaultContains },
    { 1,                U_MASK(UPROPS_IDS_TRINARY_OPERATOR), defaultContains },
    { UPROPS_SRC_BIDI,  0, isJoinControl },
    { 1,                U_MASK(UPROPS_LOGICAL_ORDER_EXCEPTION), defaultContains },
    { UPROPS_SRC_CASE,  0, caseBinaryPropertyContains },  // UCHAR_LOWERCASE
    { 1,                U_MASK(UPROPS_MATH), defaultContains },
    { 1,                U_MASK(UPROPS_NONCHARACTER_CODE_POINT), defaultContains },
    { 1,                U_MASK(UPROPS_QUOTATION_MARK), defaultContains },
    { 1,                U_MASK(UPROPS_RADICAL), defaultContains },
    { UPROPS_SRC_CASE,  0, caseBinaryPropertyContains },  // UCHAR_SOFT_DOTTED
    { 1,                U_MASK(UPROPS_TERMINAL_PUNCTUATION), defaultContains },
    { 1,                U_MASK(UPROPS_UNIFIED_IDEOGRAPH), defaultContains },
    { UPROPS_SRC_CASE,  0, caseBinaryPropertyContains },  // UCHAR_UPPERCASE
    { 1,                U_MASK(UPROPS_WHITE_SPACE), defaultContains },
    { 1,                U_MASK(UPROPS_XID_CONTINUE), defaultContains },
    { 1,                U_MASK(UPROPS_XID_START), defaultContains },
    { UPROPS_SRC_CASE,  0, caseBinaryPropertyContains },  // UCHAR_CASE_SENSITIVE
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
    { UPROPS_SRC_CASE,  0, caseBinaryPropertyContains },  // UCHAR_CASED
    { UPROPS_SRC_CASE,  0, caseBinaryPropertyContains },  // UCHAR_CASE_IGNORABLE
    { UPROPS_SRC_CASE,  0, caseBinaryPropertyContains },  // UCHAR_CHANGES_WHEN_LOWERCASED
    { UPROPS_SRC_CASE,  0, caseBinaryPropertyContains },  // UCHAR_CHANGES_WHEN_UPPERCASED
    { UPROPS_SRC_CASE,  0, caseBinaryPropertyContains },  // UCHAR_CHANGES_WHEN_TITLECASED
    { UPROPS_SRC_CASE_AND_NORM,  0, changesWhenCasefolded },
    { UPROPS_SRC_CASE,  0, caseBinaryPropertyContains },  // UCHAR_CHANGES_WHEN_CASEMAPPED
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
u_hasBinaryProperty(UChar32 c, UProperty which) {
    /* c is range-checked in the functions that are called from here */
    𐤀𐤌(which<UCHAR_BINARY_START || UCHAR_BINARY_LIMIT<=which) {
        /* not a known binary property */
        أرجع خطأ;
    } وإلا {
        ثابت BinaryProperty &prop=binProps[which];
        أرجع prop.contains(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    أرجع u_hasBinaryProperty(c, UCHAR_ID_START);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    أرجع u_hasBinaryProperty(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_stringHasBinaryProperty(ثابت char16_t *s, int32_t length, UProperty which) {
    𐤀𐤌 (s == nullptr && length != 0) { أرجع خطأ; }
    𐤀𐤌 (length == 1) {
        أرجع u_hasBinaryProperty(s[0], which);  // single code point
    } وإلا 𐤀𐤌 (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        𐤀𐤌 (length > 0 ? i == length : s[i] == 0) {
            أرجع u_hasBinaryProperty(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    أرجع UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::hasBinaryProperty(s, length, which);
}

بنية IntProperty;

تعريفنوع int32_t IntPropertyGetValue(ثابت IntProperty &prop, UChar32 c, UProperty which);
تعريفنوع int32_t IntPropertyGetMaxValue(ثابت IntProperty &prop, UProperty which);

بنية IntProperty {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t shift;  // =maxValue if getMaxValueFromShift() is used
    IntPropertyGetValue *getValue;
    IntPropertyGetMaxValue *getMaxValue;
};

ساكن int32_t defaultGetValue(ثابت IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    أرجع static_cast<int32_t>(u_getUnicodeProperties(c, prop.column) & prop.mask) >> prop.shift;
}

ساكن int32_t defaultGetMaxValue(ثابت IntProperty &prop, UProperty /*which*/) {
    أرجع (uprv_getMaxValues(prop.column)&prop.mask)>>prop.shift;
}

ساكن int32_t getMaxValueFromShift(ثابت IntProperty &prop, UProperty /*which*/) {
    أرجع prop.shift;
}

ساكن int32_t getBiDiClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع static_cast<int32_t>(u_charDirection(c));
}

ساكن int32_t getBiDiPairedBracketType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

ساكن int32_t biDiGetMaxValue(ثابت IntProperty &/*prop*/, UProperty which) {
    أرجع ubidi_getMaxValue(which);
}

ساكن int32_t getBlock(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع static_cast<int32_t>(ublock_getCode(c));
}

ساكن int32_t blockGetMaxValue(ثابت IntProperty &/*prop*/, UProperty /*which*/) {
    أرجع uprv_getMaxValues(UPROPS_MAX_VALUES_OTHER_INDEX) & UPROPS_MAX_BLOCK;
}

#𐤀𐤌 UCONFIG_NO_NORMALIZATION
ساكن int32_t getCombiningClass(ثابت IntProperty &, UChar32, UProperty) {
    أرجع 0;
}
#وإلا
ساكن int32_t getCombiningClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع u_getCombiningClass(c);
}
#endif

ساكن int32_t getGeneralCategory(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع static_cast<int32_t>(u_charType(c));
}

ساكن int32_t getJoiningGroup(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_getJoiningGroup(c);
}

ساكن int32_t getJoiningType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع ubidi_getJoiningType(c);
}

ساكن int32_t getNumericType(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_VALUE(u_getMainProperties(c)));
    أرجع UPROPS_NTV_GET_TYPE(ntv);
}

ساكن int32_t getScript(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    أرجع static_cast<int32_t>(uscript_getScript(c, &errorCode));
}

ساكن int32_t scriptGetMaxValue(ثابت IntProperty &/*prop*/, UProperty /*which*/) {
    أرجع uprv_getMaxValues(0)&UPROPS_MAX_SCRIPT;
}

/*
 * Map some of the Grapheme Cluster Break values to Hangul Syllable Types.
 * Hangul_Syllable_Type used to be fully redundant with a subset of Grapheme_Cluster_Break.
 *
 * Starting with Unicode 16, this is no longer true for HST=V vs. GCB=V in some cases:
 * Some Kirat Rai vowels are given GCB=V for proper grapheme clustering, but
 * they are of course not related to Hangul syllables.
 */
ساكن ثابت UHangulSyllableType gcbToHst[]={
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
    𐤀𐤌(c>0xffff) {
        أرجع U_HST_NOT_APPLICABLE;
    }
    /* see comments on gcbToHst[] above */
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_MASK) >> UPROPS_GCB_SHIFT;
    𐤀𐤌(gcb<UPRV_LENGTHOF(gcbToHst)) {
        أرجع gcbToHst[gcb];
    } وإلا {
        أرجع U_HST_NOT_APPLICABLE;
    }
}

#𐤀𐤌 UCONFIG_NO_NORMALIZATION
ساكن int32_t getNormQuickCheck(ثابت IntProperty &, UChar32, UProperty) {
    أرجع 0;
}
#وإلا
ساكن int32_t getNormQuickCheck(ثابت IntProperty &/*prop*/, UChar32 c, UProperty which) {
    أرجع static_cast<int32_t>(unorm_getQuickCheck(c, static_cast<UNormalizationMode>(which - UCHAR_NFD_QUICK_CHECK + UNORM_NFD)));
}
#endif

#𐤀𐤌 UCONFIG_NO_NORMALIZATION
ساكن int32_t getLeadCombiningClass(ثابت IntProperty &, UChar32, UProperty) {
    أرجع 0;
}
#وإلا
ساكن int32_t getLeadCombiningClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع unorm_getFCD16(c)>>8;
}
#endif

#𐤀𐤌 UCONFIG_NO_NORMALIZATION
ساكن int32_t getTrailCombiningClass(ثابت IntProperty &, UChar32, UProperty) {
    أرجع 0;
}
#وإلا
ساكن int32_t getTrailCombiningClass(ثابت IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    أرجع unorm_getFCD16(c)&0xff;
}
#endif

ساكن int32_t getInPC(ثابت IntProperty &, UChar32 c, UProperty) {
    أرجع ulayout_ensureData() && gInpcTrie != nullptr ? ucptrie_get(gInpcTrie, c) : 0;
}

ساكن int32_t getInSC(ثابت IntProperty &, UChar32 c, UProperty) {
    أرجع ulayout_ensureData() && gInscTrie != nullptr ? ucptrie_get(gInscTrie, c) : 0;
}

ساكن int32_t getVo(ثابت IntProperty &, UChar32 c, UProperty) {
    أرجع ulayout_ensureData() && gVoTrie != nullptr ? ucptrie_get(gVoTrie, c) : 0;
}

ساكن int32_t layoutGetMaxValue(ثابت IntProperty &/*prop*/, UProperty which) {
    𐤀𐤌 (!ulayout_ensureData()) { أرجع 0; }
    بدل (which) {
    حالة UCHAR_INDIC_POSITIONAL_CATEGORY:
        أرجع gMaxInpcValue;
    حالة UCHAR_INDIC_SYLLABIC_CATEGORY:
        أرجع gMaxInscValue;
    حالة UCHAR_VERTICAL_ORIENTATION:
        أرجع gMaxVoValue;
    افتراضي:
        أرجع 0;
    }
}

ساكن int32_t getIDStatusValue(ثابت IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    أرجع value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_STATUS_ALLOWED : U_ID_STATUS_RESTRICTED;
}

ساكن ثابت IntProperty intProps[UCHAR_INT_LIMIT-UCHAR_INT_START]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, biDiGetMaxValue },
    { UPROPS_SRC_BLOCK, 0, 0,                               getBlock, blockGetMaxValue },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, getMaxValueFromShift },
    { 2,                UPROPS_DT_MASK, 0,                  defaultGetValue, defaultGetMaxValue },
    { 0,                UPROPS_EA_MASK, UPROPS_EA_SHIFT,    defaultGetValue, defaultGetMaxValue },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_COUNT) - 1, getGeneralCategory, getMaxValueFromShift },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningGroup, biDiGetMaxValue },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, biDiGetMaxValue },
    { 2,                UPROPS_LB_MASK, UPROPS_LB_SHIFT,    defaultGetValue, defaultGetMaxValue },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_COUNT) - 1, getNumericType, getMaxValueFromShift },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, scriptGetMaxValue },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_COUNT) - 1, getHangulSyllableType, getMaxValueFromShift },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, getMaxValueFromShift },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, getMaxValueFromShift },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, getMaxValueFromShift },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, getMaxValueFromShift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, getMaxValueFromShift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, getMaxValueFromShift },
    { 2,                UPROPS_GCB_MASK, UPROPS_GCB_SHIFT,  defaultGetValue, defaultGetMaxValue },
    { 2,                UPROPS_SB_MASK, UPROPS_SB_SHIFT,    defaultGetValue, defaultGetMaxValue },
    { 2,                UPROPS_WB_MASK, UPROPS_WB_SHIFT,    defaultGetValue, defaultGetMaxValue },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, biDiGetMaxValue },
    { UPROPS_SRC_INPC,  0, 0,                               getInPC, layoutGetMaxValue },
    { UPROPS_SRC_INSC,  0, 0,                               getInSC, layoutGetMaxValue },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layoutGetMaxValue },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_STATUS_ALLOWED), getIDStatusValue, getMaxValueFromShift },
    { 0,                UPROPS_INCB_MASK, UPROPS_INCB_SHIFT,defaultGetValue, defaultGetMaxValue },
};

U_CAPI int32_t U_EXPORT2
u_getIntPropertyValue(UChar32 c, UProperty which) {
    𐤀𐤌(which<UCHAR_INT_START) {
        𐤀𐤌(UCHAR_BINARY_START<=which && which<UCHAR_BINARY_LIMIT) {
            ثابت BinaryProperty &prop=binProps[which];
            أرجع prop.contains(prop, c, which);
        }
    } وإلا 𐤀𐤌(which<UCHAR_INT_LIMIT) {
        ثابت IntProperty &prop=intProps[which-UCHAR_INT_START];
        أرجع prop.getValue(prop, c, which);
    } وإلا 𐤀𐤌(which==UCHAR_GENERAL_CATEGORY_MASK) {
        أرجع U_MASK(u_charType(c));
    }
    أرجع 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_getIntPropertyMinValue(UProperty /*which*/) {
    أرجع 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_getIntPropertyMaxValue(UProperty which) {
    𐤀𐤌(which<UCHAR_INT_START) {
        𐤀𐤌(UCHAR_BINARY_START<=which && which<UCHAR_BINARY_LIMIT) {
            أرجع 1;  // maximum true for all binary properties
        }
    } وإلا 𐤀𐤌(which<UCHAR_INT_LIMIT) {
        ثابت IntProperty &prop=intProps[which-UCHAR_INT_START];
        أرجع prop.getMaxValue(prop, which);
    }
    أرجع -1;  // undefined
}

U_CFUNC UPropertySource U_EXPORT2
uprops_getSource(UProperty which) {
    𐤀𐤌(which<UCHAR_BINARY_START) {
        أرجع UPROPS_SRC_NONE; /* undefined */
    } وإلا 𐤀𐤌(which<UCHAR_BINARY_LIMIT) {
        ثابت BinaryProperty &prop=binProps[which];
        𐤀𐤌(prop.mask!=0) {
            أرجع UPROPS_SRC_PROPSVEC;
        } وإلا {
            أرجع (UPropertySource)prop.column;
        }
    } وإلا 𐤀𐤌(which<UCHAR_INT_START) {
        أرجع UPROPS_SRC_NONE; /* undefined */
    } وإلا 𐤀𐤌(which<UCHAR_INT_LIMIT) {
        ثابت IntProperty &prop=intProps[which-UCHAR_INT_START];
        𐤀𐤌(prop.mask!=0) {
            أرجع UPROPS_SRC_PROPSVEC;
        } وإلا {
            أرجع (UPropertySource)prop.column;
        }
    } وإلا 𐤀𐤌(which<UCHAR_STRING_START) {
        بدل(which) {
        حالة UCHAR_GENERAL_CATEGORY_MASK:
        حالة UCHAR_NUMERIC_VALUE:
            أرجع UPROPS_SRC_CHAR;

        افتراضي:
            أرجع UPROPS_SRC_NONE;
        }
    } وإلا 𐤀𐤌(which<UCHAR_STRING_LIMIT) {
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
            أرجع UPROPS_SRC_NAMES;

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
uprops_addPropertyStarts(UPropertySource src, ثابت USetAdder *sa, UErrorCode *pErrorCode) {
    𐤀𐤌 (U_FAILURE(*pErrorCode)) { أرجع; }
    𐤀𐤌 (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        لكل (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->add(sa->set, c);
        }
        // single characters
        لكل (UChar32 c : ID_COMPAT_MATH_START) {
            sa->add(sa->set, c);
            sa->add(sa->set, c + 1);
        }
        أرجع;
    }
    𐤀𐤌 (src == UPROPS_SRC_MCM) {
        // range limits
        لكل (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->add(sa->set, c);
        }
        أرجع;
    }
    𐤀𐤌 (!ulayout_ensureData(*pErrorCode)) { أرجع; }
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
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }

    𐤀𐤌 (trie == nullptr) {
        *pErrorCode = U_MISSING_RESOURCE_ERROR;
        أرجع;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    طالما ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, nullptr)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

U_CAPI منطقي U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t typeIndex = type;  // also guards against negative type integers
    𐤀𐤌 (typeIndex >= UPRV_LENGTHOF(uprops_idTypeToEncoded)) {
        أرجع خطأ;
    }
    uint32_t encodedType = uprops_idTypeToEncoded[typeIndex];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    𐤀𐤌 ((encodedType & UPROPS_ID_TYPE_BIT) != 0) {
        أرجع value < UPROPS_ID_TYPE_FORBIDDEN && (value & encodedType) != 0;
    } وإلا {
        أرجع value == encodedType;
    }
}

نطاق {

فراغ maybeAppendType(uint32_t value, uint32_t bit, UIdentifierType t,
                     UIdentifierType *types, int32_t &length, int32_t capacity) {
    𐤀𐤌 ((value & bit) != 0) {
        𐤀𐤌 (length < capacity) {
            types[length] = t;
        }
        ++length;
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UErrorCode *pErrorCode) {
    𐤀𐤌 (U_FAILURE(*pErrorCode)) { أرجع 0; }
    𐤀𐤌 (capacity < 0 || (capacity > 0 && types == nullptr)) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    𐤀𐤌 ((value & UPROPS_ID_TYPE_FORBIDDEN) == UPROPS_ID_TYPE_FORBIDDEN ||
            value == UPROPS_ID_TYPE_NOT_CHARACTER) {
        // single value
        𐤀𐤌 (capacity > 0) {
            UIdentifierType t;
            بدل (value) {
                حالة UPROPS_ID_TYPE_NOT_CHARACTER: t = U_ID_TYPE_NOT_CHARACTER; اكسر;
                حالة UPROPS_ID_TYPE_DEPRECATED: t = U_ID_TYPE_DEPRECATED; اكسر;
                حالة UPROPS_ID_TYPE_DEFAULT_IGNORABLE: t = U_ID_TYPE_DEFAULT_IGNORABLE; اكسر;
                حالة UPROPS_ID_TYPE_NOT_NFKC: t = U_ID_TYPE_NOT_NFKC; اكسر;
                حالة UPROPS_ID_TYPE_INCLUSION: t = U_ID_TYPE_INCLUSION; اكسر;
                حالة UPROPS_ID_TYPE_RECOMMENDED: t = U_ID_TYPE_RECOMMENDED; اكسر;
                افتراضي:
                    *pErrorCode = U_INVALID_FORMAT_ERROR;
                    أرجع 0;
            }
            types[0] = t;
        } وإلا {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
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
        𐤀𐤌 (length >= capacity) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        أرجع length;
    }
}

#𐤀𐤌 !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_getFC_NFKC_Closure(UChar32 c, char16_t *dest, int32_t destCapacity, UErrorCode *pErrorCode) {
    𐤀𐤌(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }
    𐤀𐤌(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    ثابت Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*pErrorCode);
    𐤀𐤌(U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    ثابت char16_t *folded1;
    int32_t folded1Length=ucase_toFullFolding(c, &folded1, U_FOLD_CASE_DEFAULT);
    𐤀𐤌(folded1Length<0) {
        ثابت Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        𐤀𐤌(nfkcImpl->getCompQuickCheck(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            أرجع u_terminateUChars(dest, destCapacity, 0, pErrorCode);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.setTo(c);
    } وإلا {
        𐤀𐤌(folded1Length>UCASE_MAX_STRING_LENGTH) {
            folded1String.setTo(folded1Length);
        } وإلا {
            folded1String.setTo(خطأ, folded1, folded1Length);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *pErrorCode);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *pErrorCode);
    // if (c != b) add the mapping from a to c
    𐤀𐤌(U_FAILURE(*pErrorCode) || kc1==kc2) {
        أرجع u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    } وإلا {
        أرجع kc2.extract(dest, destCapacity, *pErrorCode);
    }
}

#endif
