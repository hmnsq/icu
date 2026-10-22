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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/unorm2.h"
#अन्तर्भाव "unicode/uscript.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "emojiprops.h"
#अन्तर्भाव "mutex.h"
#अन्तर्भाव "normalizer2impl.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "ubidi_props.h"
#अन्तर्भाव "uprops.h"
#अन्तर्भाव "ucase.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "ulayout_props.h"
#अन्तर्भाव "ustr_imp.h"

U_NAMESPACE_USE

// Unicode text layout properties data -----------------------------------------

नामाकाश {

icu::UInitOnce gLayoutInitOnce {};
UDataMemory *gLayoutMemory = nullptr;

UCPTrie *gInpcTrie = nullptr;  // Indic_Positional_Category
UCPTrie *gInscTrie = nullptr;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = nullptr;  // Vertical_Orientation

int32_t g_Max_Inpc_मान = 0;
int32_t g_Max_Insc_मान = 0;
int32_t g_Max_Vo_मान = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(gLayoutMemory);
    gLayoutMemory = nullptr;

    ucptrie_close(gInpcTrie);
    gInpcTrie = nullptr;
    ucptrie_close(gInscTrie);
    gInscTrie = nullptr;
    ucptrie_close(gVoTrie);
    gVoTrie = nullptr;

    g_Max_Inpc_मान = 0;
    g_Max_Insc_मान = 0;
    g_Max_Vo_मान = 0;

    gLayoutInitOnce.reset();
    फर्काऊ सत्य;
}

UBool U_CALLCONV
ulayout_isAcceptable(शून्य * /*context*/,
                     अचर वर्ण * /* type */, अचर वर्ण * /*name*/,
                     अचर UDataInfo *pInfo) {
    फर्काऊ pInfo->size >= 20 &&
        pInfo->isBigEndian == U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily == U_CHARSET_FAMILY &&
        pInfo->dataFormat[0] == ULAYOUT_FMT_0 &&
        pInfo->dataFormat[1] == ULAYOUT_FMT_1 &&
        pInfo->dataFormat[2] == ULAYOUT_FMT_2 &&
        pInfo->dataFormat[3] == ULAYOUT_FMT_3 &&
        pInfo->formatVersion[0] == 1;
}

// UInitOnce singleton initialization function
शून्य U_CALLCONV ulayout_load(UErrorCode &errorCode) {
    gLayoutMemory = udata_openChoice(
        nullptr, ULAYOUT_DATA_TYPE, ULAYOUT_DATA_NAME,
        ulayout_isAcceptable, nullptr, &errorCode);
    यदि (U_FAILURE(errorCode)) { फर्काऊ; }

    अचर uint8_t* inBytes = static_cast<अचर uint8_t*>(udata_getMemory(gLayoutMemory));
    अचर int32_t* inIndexes = reinterpret_cast<अचर int32_t*>(inBytes);
    int32_t indexesLength = inIndexes[ULAYOUT_IX_INDEXES_LENGTH];
    यदि (indexesLength < 12) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        फर्काऊ;
    }
    int32_t offset = indexesLength * 4;
    int32_t शीर्ष = inIndexes[ULAYOUT_IX_INPC_TRIE_शीर्ष];
    int32_t trieSize = शीर्ष - offset;
    यदि (trieSize >= 16) {
        gInpcTrie = ucptrie_open_From_द्विआधारी(
            UCPTRIE_TYPE_ANY, UCPTRIE_मान_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }
    offset = शीर्ष;
    शीर्ष = inIndexes[ULAYOUT_IX_INSC_TRIE_शीर्ष];
    trieSize = शीर्ष - offset;
    यदि (trieSize >= 16) {
        gInscTrie = ucptrie_open_From_द्विआधारी(
            UCPTRIE_TYPE_ANY, UCPTRIE_मान_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }
    offset = शीर्ष;
    शीर्ष = inIndexes[ULAYOUT_IX_VO_TRIE_शीर्ष];
    trieSize = शीर्ष - offset;
    यदि (trieSize >= 16) {
        gVoTrie = ucptrie_open_From_द्विआधारी(
            UCPTRIE_TYPE_ANY, UCPTRIE_मान_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }

    uint32_t max_मानहरू = inIndexes[ULAYOUT_IX_MAX_मानहरू];
    g_Max_Inpc_मान = max_मानहरू >> ULAYOUT_MAX_INPC_SHIFT;
    g_Max_Insc_मान = (max_मानहरू >> ULAYOUT_MAX_INSC_SHIFT) & 0xff;
    g_Max_Vo_मान = (max_मानहरू >> ULAYOUT_MAX_VO_SHIFT) & 0xff;

    ucln_common_registerCleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensureData(UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { फर्काऊ असत्य; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_load, errorCode);
    फर्काऊ U_SUCCESS(errorCode);
}

UBool ulayout_ensureData() {
    UErrorCode errorCode = U_ZERO_ERROR;
    फर्काऊ ulayout_ensureData(errorCode);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

संरचना द्विआधारी_Property;

प्रकारनिर्देश UBool द्विआधारी_Property_Contains(अचर द्विआधारी_Property &prop, UChar32 c, UProperty which);

संरचना द्विआधारी_Property {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    द्विआधारी_Property_Contains *contains;
};

स्थिर UBool defaultContains(अचर द्विआधारी_Property &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    फर्काऊ (u_getUnicodeProperties(c, prop.column)&prop.mask)!=0;
}

स्थिर UBool case_द्विआधारी_Property_Contains(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty which) {
    फर्काऊ ucase_has_द्विआधारी_Property(c, which);
}

स्थिर UBool isBidiControl(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ ubidi_isBidiControl(c);
}

स्थिर UBool isMirrored(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ ubidi_isMirrored(c);
}

स्थिर UBool isJoinControl(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ ubidi_isJoinControl(c);
}

#यदि UCONFIG_NO_NORMALIZATION
स्थिर UBool hasFullCompositionExclusion(अचर द्विआधारी_Property &, UChar32, UProperty) {
    फर्काऊ असत्य;
}
#अन्यथा
स्थिर UBool hasFullCompositionExclusion(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UErrorCode errorCode=U_ZERO_ERROR;
    अचर Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    फर्काऊ U_SUCCESS(errorCode) && impl->isCompNo(impl->getNorm16(c));
}
#endif

// UCHAR_NF*_INERT properties
#यदि UCONFIG_NO_NORMALIZATION
स्थिर UBool isNormInert(अचर द्विआधारी_Property &, UChar32, UProperty) {
    फर्काऊ असत्य;
}
#अन्यथा
स्थिर UBool isNormInert(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty which) {
    UErrorCode errorCode=U_ZERO_ERROR;
    अचर Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalizationMode>(which - UCHAR_NFD_INERT + UNORM_NFD), errorCode);
    फर्काऊ U_SUCCESS(errorCode) && norm2->isInert(c);
}
#endif

#यदि UCONFIG_NO_NORMALIZATION
स्थिर UBool changesWhenCasefolded(अचर द्विआधारी_Property &, UChar32, UProperty) {
    फर्काऊ असत्य;
}
#अन्यथा
स्थिर UBool changesWhenCasefolded(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UErrorCode errorCode=U_ZERO_ERROR;
    अचर Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(errorCode);
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ असत्य;
    }
    यदि(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        यदि(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } अन्यथा यदि(nfd.length()<=U16_MAX_LENGTH &&
                  nfd.length()==U16_LENGTH(c=nfd.char32At(0))
        ) {
            /* single supplementary code point */
        } अन्यथा {
            c=U_SENTINEL;
        }
    } अन्यथा यदि(c<0) {
        फर्काऊ असत्य;  /* protect against bad input */
    }
    यदि(c>=0) {
        /* single code point */
        अचर char16_t *resultString;
        फर्काऊ ucase_toFullFolding(c, &resultString, U_FOLD_CASE_DEFAULT) >= 0;
    } अन्यथा {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_LENGTH];
        int32_t destLength;
        destLength=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.getBuffer(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &errorCode);
        फर्काऊ U_SUCCESS(errorCode) &&
                       0!=u_strCompare(nfd.getBuffer(), nfd.length(),
                                       dest, destLength, असत्य);
    }
}
#endif

#यदि UCONFIG_NO_NORMALIZATION
स्थिर UBool changesWhenNFKC_Casefolded(अचर द्विआधारी_Property &, UChar32, UProperty) {
    फर्काऊ असत्य;
}
#अन्यथा
स्थिर UBool changesWhenNFKC_Casefolded(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    अचर Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(errorCode);
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ असत्य;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        ReorderingBuffer buffer(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        यदि(buffer.init(5, errorCode)) {
            अचर char16_t *srcArray=src.getBuffer();
            kcf->compose(srcArray, srcArray+src.length(), असत्य,
                          सत्य, buffer, errorCode);
        }
    }
    फर्काऊ U_SUCCESS(errorCode) && dest!=src;
}
#endif

#यदि UCONFIG_NO_NORMALIZATION
स्थिर UBool isCanonSegmentStarter(अचर द्विआधारी_Property &, UChar32, UProperty) {
    फर्काऊ असत्य;
}
#अन्यथा
स्थिर UBool isCanonSegmentStarter(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    अचर Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    फर्काऊ
        U_SUCCESS(errorCode) && impl->ensureCanonIterData(errorCode) &&
        impl->isCanonSegmentStarter(c);
}
#endif

स्थिर UBool isPOSIX_alnum(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ u_isalnumPOSIX(c);
}

स्थिर UBool isPOSIX_blank(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ u_isblank(c);
}

स्थिर UBool isPOSIX_graph(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ u_isgraphPOSIX(c);
}

स्थिर UBool isPOSIX_print(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ u_isprintPOSIX(c);
}

स्थिर UBool isPOSIX_xdigit(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ u_isxdigit(c);
}

स्थिर UBool isRegionalIndicator(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    फर्काऊ 0x1F1E6<=c && c<=0x1F1FF;
}

स्थिर UBool hasEmojiProperty(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty which) {
    फर्काऊ EmojiProps::has_द्विआधारी_Property(c, which);
}

स्थिर UBool isIDSUnaryOperator(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // New in Unicode 15.1 for just two characters.
    फर्काऊ 0x2FFE<=c && c<=0x2FFF;
}

/** Ranges (start/limit pairs) of ID_Compat_Math_Continue (only), from UCD PropList.txt. */
स्थिर constexpr UChar32 ID_COMPAT_MATH_CONTINUE[] = {
    0x00B2, 0x00B3 + 1,
    0x00B9, 0x00B9 + 1,
    0x2070, 0x2070 + 1,
    0x2074, 0x207E + 1,
    0x2080, 0x208E + 1
};

/** ID_Compat_Math_Start characters, from UCD PropList.txt. */
स्थिर constexpr UChar32 ID_COMPAT_MATH_START[] = {
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
स्थिर constexpr UChar32 MODIFIER_COMBINING_MARK[] = {
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

स्थिर UBool isIDCompatMathStart(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    यदि (c < ID_COMPAT_MATH_START[0]) { फर्काऊ असत्य; }  // fastpath for common scripts
    लागि (UChar32 startChar : ID_COMPAT_MATH_START) {
        यदि (c == startChar) { फर्काऊ सत्य; }
    }
    फर्काऊ असत्य;
}

स्थिर UBool isIDCompatMathContinue(अचर द्विआधारी_Property &prop, UChar32 c, UProperty /*which*/) {
    लागि (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        यदि (c < ID_COMPAT_MATH_CONTINUE[i]) { फर्काऊ असत्य; }  // below range start
        यदि (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { फर्काऊ सत्य; }  // below range limit
    }
    फर्काऊ isIDCompatMathStart(prop, c, UCHAR_ID_COMPAT_MATH_START);
}

स्थिर UBool isModifierCombiningMark(अचर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    लागि (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        यदि (c < MODIFIER_COMBINING_MARK[i]) { फर्काऊ असत्य; }  // below range start
        यदि (c < MODIFIER_COMBINING_MARK[i + 1]) { फर्काऊ सत्य; }  // below range limit
    }
    फर्काऊ असत्य;
}

स्थिर अचर द्विआधारी_Property binProps[UCHAR_द्विआधारी_LIMIT]={
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
    { 1,                U_MASK(UPROPS_IDS_द्विआधारी_OPERATOR), defaultContains },
    { 1,                U_MASK(UPROPS_IDS_TRINARY_OPERATOR), defaultContains },
    { UPROPS_SRC_BIDI,  0, isJoinControl },
    { 1,                U_MASK(UPROPS_LOGICAL_ORDER_EXCEPTION), defaultContains },
    { UPROPS_SRC_CASE,  0, case_द्विआधारी_Property_Contains },  // UCHAR_LOWERCASE
    { 1,                U_MASK(UPROPS_MATH), defaultContains },
    { 1,                U_MASK(UPROPS_NONCHARACTER_CODE_POINT), defaultContains },
    { 1,                U_MASK(UPROPS_QUOTATION_MARK), defaultContains },
    { 1,                U_MASK(UPROPS_RADICAL), defaultContains },
    { UPROPS_SRC_CASE,  0, case_द्विआधारी_Property_Contains },  // UCHAR_SOFT_DOTTED
    { 1,                U_MASK(UPROPS_TERMINAL_PUNCTUATION), defaultContains },
    { 1,                U_MASK(UPROPS_UNIFIED_IDEOGRAPH), defaultContains },
    { UPROPS_SRC_CASE,  0, case_द्विआधारी_Property_Contains },  // UCHAR_UPPERCASE
    { 1,                U_MASK(UPROPS_WHITE_SPACE), defaultContains },
    { 1,                U_MASK(UPROPS_XID_CONTINUE), defaultContains },
    { 1,                U_MASK(UPROPS_XID_START), defaultContains },
    { UPROPS_SRC_CASE,  0, case_द्विआधारी_Property_Contains },  // UCHAR_CASE_SENSITIVE
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
    { UPROPS_SRC_CASE,  0, case_द्विआधारी_Property_Contains },  // UCHAR_CASED
    { UPROPS_SRC_CASE,  0, case_द्विआधारी_Property_Contains },  // UCHAR_CASE_IGNORABLE
    { UPROPS_SRC_CASE,  0, case_द्विआधारी_Property_Contains },  // UCHAR_CHANGES_WHEN_LOWERCASED
    { UPROPS_SRC_CASE,  0, case_द्विआधारी_Property_Contains },  // UCHAR_CHANGES_WHEN_UPPERCASED
    { UPROPS_SRC_CASE,  0, case_द्विआधारी_Property_Contains },  // UCHAR_CHANGES_WHEN_TITLECASED
    { UPROPS_SRC_CASE_AND_NORM,  0, changesWhenCasefolded },
    { UPROPS_SRC_CASE,  0, case_द्विआधारी_Property_Contains },  // UCHAR_CHANGES_WHEN_CASEMAPPED
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
u_has_द्विआधारी_Property(UChar32 c, UProperty which) {
    /* c is range-checked in the functions that are called from here */
    यदि(which<UCHAR_द्विआधारी_START || UCHAR_द्विआधारी_LIMIT<=which) {
        /* not a known binary property */
        फर्काऊ असत्य;
    } अन्यथा {
        अचर द्विआधारी_Property &prop=binProps[which];
        फर्काऊ prop.contains(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    फर्काऊ u_has_द्विआधारी_Property(c, UCHAR_ID_START);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    फर्काऊ u_has_द्विआधारी_Property(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_string_Has_द्विआधारी_Property(अचर char16_t *s, int32_t length, UProperty which) {
    यदि (s == nullptr && length != 0) { फर्काऊ असत्य; }
    यदि (length == 1) {
        फर्काऊ u_has_द्विआधारी_Property(s[0], which);  // single code point
    } अन्यथा यदि (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        यदि (length > 0 ? i == length : s[i] == 0) {
            फर्काऊ u_has_द्विआधारी_Property(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    फर्काऊ UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::has_द्विआधारी_Property(s, length, which);
}

संरचना IntProperty;

प्रकारनिर्देश int32_t Int_Property_Get_मान(अचर IntProperty &prop, UChar32 c, UProperty which);
प्रकारनिर्देश int32_t Int_Property_Get_Max_मान(अचर IntProperty &prop, UProperty which);

संरचना IntProperty {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t shift;  // =maxValue if getMaxValueFromShift() is used
    Int_Property_Get_मान *get_मान;
    Int_Property_Get_Max_मान *get_Max_मान;
};

स्थिर int32_t default_Get_मान(अचर IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    फर्काऊ static_cast<int32_t>(u_getUnicodeProperties(c, prop.column) & prop.mask) >> prop.shift;
}

स्थिर int32_t default_Get_Max_मान(अचर IntProperty &prop, UProperty /*which*/) {
    फर्काऊ (uprv_get_Max_मानहरू(prop.column)&prop.mask)>>prop.shift;
}

स्थिर int32_t get_Max_मान_From_Shift(अचर IntProperty &prop, UProperty /*which*/) {
    फर्काऊ prop.shift;
}

स्थिर int32_t getBiDiClass(अचर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ static_cast<int32_t>(u_charDirection(c));
}

स्थिर int32_t getBiDiPairedBracketType(अचर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

स्थिर int32_t bi_Di_Get_Max_मान(अचर IntProperty &/*prop*/, UProperty which) {
    फर्काऊ ubidi_get_Max_मान(which);
}

स्थिर int32_t getBlock(अचर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ static_cast<int32_t>(ublock_getCode(c));
}

स्थिर int32_t block_Get_Max_मान(अचर IntProperty &/*prop*/, UProperty /*which*/) {
    फर्काऊ uprv_get_Max_मानहरू(UPROPS_MAX_मानहरू_OTHER_INDEX) & UPROPS_MAX_BLOCK;
}

#यदि UCONFIG_NO_NORMALIZATION
स्थिर int32_t getCombiningClass(अचर IntProperty &, UChar32, UProperty) {
    फर्काऊ 0;
}
#अन्यथा
स्थिर int32_t getCombiningClass(अचर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ u_getCombiningClass(c);
}
#endif

स्थिर int32_t getGeneralCategory(अचर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ static_cast<int32_t>(u_charType(c));
}

स्थिर int32_t getJoiningGroup(अचर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ ubidi_getJoiningGroup(c);
}

स्थिर int32_t getJoiningType(अचर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ ubidi_getJoiningType(c);
}

स्थिर int32_t getNumericType(अचर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_मान(u_getMainProperties(c)));
    फर्काऊ UPROPS_NTV_GET_TYPE(ntv);
}

स्थिर int32_t getScript(अचर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    फर्काऊ static_cast<int32_t>(uscript_getScript(c, &errorCode));
}

स्थिर int32_t script_Get_Max_मान(अचर IntProperty &/*prop*/, UProperty /*which*/) {
    फर्काऊ uprv_get_Max_मानहरू(0)&UPROPS_MAX_SCRIPT;
}

/*
 * Map some of the Grapheme Cluster Break values to Hangul Syllable Types.
 * Hangul_Syllable_Type used to be fully redundant with a subset of Grapheme_Cluster_Break.
 *
 * Starting with Unicode 16, this is no longer true for HST=V vs. GCB=V in some cases:
 * Some Kirat Rai vowels are given GCB=V for proper grapheme clustering, but
 * they are of course not related to Hangul syllables.
 */
स्थिर अचर UHangulSyllableType gcbToHst[]={
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

स्थिर int32_t getHangulSyllableType(अचर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Ignore supplementary code points: They all have HST=NA.
    // This is a simple way to handle the GCB!=hst cases since Unicode 16 (Kirat Rai vowels).
    यदि(c>0xffff) {
        फर्काऊ U_HST_NOT_APPLICABLE;
    }
    /* see comments on gcbToHst[] above */
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_MASK) >> UPROPS_GCB_SHIFT;
    यदि(gcb<UPRV_LENGTHOF(gcbToHst)) {
        फर्काऊ gcbToHst[gcb];
    } अन्यथा {
        फर्काऊ U_HST_NOT_APPLICABLE;
    }
}

#यदि UCONFIG_NO_NORMALIZATION
स्थिर int32_t getNormQuickCheck(अचर IntProperty &, UChar32, UProperty) {
    फर्काऊ 0;
}
#अन्यथा
स्थिर int32_t getNormQuickCheck(अचर IntProperty &/*prop*/, UChar32 c, UProperty which) {
    फर्काऊ static_cast<int32_t>(unorm_getQuickCheck(c, static_cast<UNormalizationMode>(which - UCHAR_NFD_QUICK_CHECK + UNORM_NFD)));
}
#endif

#यदि UCONFIG_NO_NORMALIZATION
स्थिर int32_t getLeadCombiningClass(अचर IntProperty &, UChar32, UProperty) {
    फर्काऊ 0;
}
#अन्यथा
स्थिर int32_t getLeadCombiningClass(अचर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ unorm_getFCD16(c)>>8;
}
#endif

#यदि UCONFIG_NO_NORMALIZATION
स्थिर int32_t getTrailCombiningClass(अचर IntProperty &, UChar32, UProperty) {
    फर्काऊ 0;
}
#अन्यथा
स्थिर int32_t getTrailCombiningClass(अचर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    फर्काऊ unorm_getFCD16(c)&0xff;
}
#endif

स्थिर int32_t getInPC(अचर IntProperty &, UChar32 c, UProperty) {
    फर्काऊ ulayout_ensureData() && gInpcTrie != nullptr ? ucptrie_get(gInpcTrie, c) : 0;
}

स्थिर int32_t getInSC(अचर IntProperty &, UChar32 c, UProperty) {
    फर्काऊ ulayout_ensureData() && gInscTrie != nullptr ? ucptrie_get(gInscTrie, c) : 0;
}

स्थिर int32_t getVo(अचर IntProperty &, UChar32 c, UProperty) {
    फर्काऊ ulayout_ensureData() && gVoTrie != nullptr ? ucptrie_get(gVoTrie, c) : 0;
}

स्थिर int32_t layout_Get_Max_मान(अचर IntProperty &/*prop*/, UProperty which) {
    यदि (!ulayout_ensureData()) { फर्काऊ 0; }
    विकल्प (which) {
    पक्ष UCHAR_INDIC_POSITIONAL_CATEGORY:
        फर्काऊ g_Max_Inpc_मान;
    पक्ष UCHAR_INDIC_SYLLABIC_CATEGORY:
        फर्काऊ g_Max_Insc_मान;
    पक्ष UCHAR_VERTICAL_ORIENTATION:
        फर्काऊ g_Max_Vo_मान;
    स्वभाव:
        फर्काऊ 0;
    }
}

स्थिर int32_t get_IDStatus_मान(अचर IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    फर्काऊ value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_STATUS_ALLOWED : U_ID_STATUS_RESTRICTED;
}

स्थिर अचर IntProperty intProps[UCHAR_INT_LIMIT-UCHAR_INT_START]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, bi_Di_Get_Max_मान },
    { UPROPS_SRC_BLOCK, 0, 0,                               getBlock, block_Get_Max_मान },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, get_Max_मान_From_Shift },
    { 2,                UPROPS_DT_MASK, 0,                  default_Get_मान, default_Get_Max_मान },
    { 0,                UPROPS_EA_MASK, UPROPS_EA_SHIFT,    default_Get_मान, default_Get_Max_मान },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_गन्ती) - 1, getGeneralCategory, get_Max_मान_From_Shift },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningGroup, bi_Di_Get_Max_मान },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, bi_Di_Get_Max_मान },
    { 2,                UPROPS_LB_MASK, UPROPS_LB_SHIFT,    default_Get_मान, default_Get_Max_मान },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_गन्ती) - 1, getNumericType, get_Max_मान_From_Shift },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, script_Get_Max_मान },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_गन्ती) - 1, getHangulSyllableType, get_Max_मान_From_Shift },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, get_Max_मान_From_Shift },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, get_Max_मान_From_Shift },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, get_Max_मान_From_Shift },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, get_Max_मान_From_Shift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, get_Max_मान_From_Shift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, get_Max_मान_From_Shift },
    { 2,                UPROPS_GCB_MASK, UPROPS_GCB_SHIFT,  default_Get_मान, default_Get_Max_मान },
    { 2,                UPROPS_SB_MASK, UPROPS_SB_SHIFT,    default_Get_मान, default_Get_Max_मान },
    { 2,                UPROPS_WB_MASK, UPROPS_WB_SHIFT,    default_Get_मान, default_Get_Max_मान },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, bi_Di_Get_Max_मान },
    { UPROPS_SRC_INPC,  0, 0,                               getInPC, layout_Get_Max_मान },
    { UPROPS_SRC_INSC,  0, 0,                               getInSC, layout_Get_Max_मान },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layout_Get_Max_मान },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_STATUS_ALLOWED), get_IDStatus_मान, get_Max_मान_From_Shift },
    { 0,                UPROPS_INCB_MASK, UPROPS_INCB_SHIFT,default_Get_मान, default_Get_Max_मान },
};

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_मान(UChar32 c, UProperty which) {
    यदि(which<UCHAR_INT_START) {
        यदि(UCHAR_द्विआधारी_START<=which && which<UCHAR_द्विआधारी_LIMIT) {
            अचर द्विआधारी_Property &prop=binProps[which];
            फर्काऊ prop.contains(prop, c, which);
        }
    } अन्यथा यदि(which<UCHAR_INT_LIMIT) {
        अचर IntProperty &prop=intProps[which-UCHAR_INT_START];
        फर्काऊ prop.get_मान(prop, c, which);
    } अन्यथा यदि(which==UCHAR_GENERAL_CATEGORY_MASK) {
        फर्काऊ U_MASK(u_charType(c));
    }
    फर्काऊ 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Min_मान(UProperty /*which*/) {
    फर्काऊ 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Max_मान(UProperty which) {
    यदि(which<UCHAR_INT_START) {
        यदि(UCHAR_द्विआधारी_START<=which && which<UCHAR_द्विआधारी_LIMIT) {
            फर्काऊ 1;  // maximum true for all binary properties
        }
    } अन्यथा यदि(which<UCHAR_INT_LIMIT) {
        अचर IntProperty &prop=intProps[which-UCHAR_INT_START];
        फर्काऊ prop.get_Max_मान(prop, which);
    }
    फर्काऊ -1;  // undefined
}

U_CFUNC UPropertySource U_EXPORT2
uprops_getSource(UProperty which) {
    यदि(which<UCHAR_द्विआधारी_START) {
        फर्काऊ UPROPS_SRC_NONE; /* undefined */
    } अन्यथा यदि(which<UCHAR_द्विआधारी_LIMIT) {
        अचर द्विआधारी_Property &prop=binProps[which];
        यदि(prop.mask!=0) {
            फर्काऊ UPROPS_SRC_PROPSVEC;
        } अन्यथा {
            फर्काऊ (UPropertySource)prop.column;
        }
    } अन्यथा यदि(which<UCHAR_INT_START) {
        फर्काऊ UPROPS_SRC_NONE; /* undefined */
    } अन्यथा यदि(which<UCHAR_INT_LIMIT) {
        अचर IntProperty &prop=intProps[which-UCHAR_INT_START];
        यदि(prop.mask!=0) {
            फर्काऊ UPROPS_SRC_PROPSVEC;
        } अन्यथा {
            फर्काऊ (UPropertySource)prop.column;
        }
    } अन्यथा यदि(which<UCHAR_STRING_START) {
        विकल्प(which) {
        पक्ष UCHAR_GENERAL_CATEGORY_MASK:
        पक्ष UCHAR_NUMERIC_मान:
            फर्काऊ UPROPS_SRC_CHAR;

        स्वभाव:
            फर्काऊ UPROPS_SRC_NONE;
        }
    } अन्यथा यदि(which<UCHAR_STRING_LIMIT) {
        विकल्प(which) {
        पक्ष UCHAR_AGE:
            फर्काऊ UPROPS_SRC_PROPSVEC;

        पक्ष UCHAR_BIDI_MIRRORING_GLYPH:
            फर्काऊ UPROPS_SRC_BIDI;

        पक्ष UCHAR_CASE_FOLDING:
        पक्ष UCHAR_LOWERCASE_MAPPING:
        पक्ष UCHAR_SIMPLE_CASE_FOLDING:
        पक्ष UCHAR_SIMPLE_LOWERCASE_MAPPING:
        पक्ष UCHAR_SIMPLE_TITLECASE_MAPPING:
        पक्ष UCHAR_SIMPLE_UPPERCASE_MAPPING:
        पक्ष UCHAR_TITLECASE_MAPPING:
        पक्ष UCHAR_UPPERCASE_MAPPING:
            फर्काऊ UPROPS_SRC_CASE;

        पक्ष UCHAR_ISO_COMMENT:
        पक्ष UCHAR_NAME:
        पक्ष UCHAR_UNICODE_1_NAME:
            फर्काऊ UPROPS_SRC_NAMES;

        स्वभाव:
            फर्काऊ UPROPS_SRC_NONE;
        }
    } अन्यथा {
        विकल्प(which) {
        पक्ष UCHAR_SCRIPT_EXTENSIONS:
        पक्ष UCHAR_IDENTIFIER_TYPE:
            फर्काऊ UPROPS_SRC_PROPSVEC;
        स्वभाव:
            फर्काऊ UPROPS_SRC_NONE; /* undefined */
        }
    }
}

U_CFUNC शून्य U_EXPORT2
uprops_addPropertyStarts(UPropertySource src, अचर USetAdder *sa, UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) { फर्काऊ; }
    यदि (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        लागि (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->add(sa->set, c);
        }
        // single characters
        लागि (UChar32 c : ID_COMPAT_MATH_START) {
            sa->add(sa->set, c);
            sa->add(sa->set, c + 1);
        }
        फर्काऊ;
    }
    यदि (src == UPROPS_SRC_MCM) {
        // range limits
        लागि (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->add(sa->set, c);
        }
        फर्काऊ;
    }
    यदि (!ulayout_ensureData(*pErrorCode)) { फर्काऊ; }
    अचर UCPTrie *trie;
    विकल्प (src) {
    पक्ष UPROPS_SRC_INPC:
        trie = gInpcTrie;
        रोक;
    पक्ष UPROPS_SRC_INSC:
        trie = gInscTrie;
        रोक;
    पक्ष UPROPS_SRC_VO:
        trie = gVoTrie;
        रोक;
    स्वभाव:
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }

    यदि (trie == nullptr) {
        *pErrorCode = U_MISSING_RESOURCE_ERROR;
        फर्काऊ;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    जबसम्म ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, nullptr)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

U_CAPI तार्किक U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t typeIndex = type;  // also guards against negative type integers
    यदि (typeIndex >= UPRV_LENGTHOF(uprops_idTypeToEncoded)) {
        फर्काऊ असत्य;
    }
    uint32_t encodedType = uprops_idTypeToEncoded[typeIndex];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    यदि ((encodedType & UPROPS_ID_TYPE_BIT) != 0) {
        फर्काऊ value < UPROPS_ID_TYPE_FORBIDDEN && (value & encodedType) != 0;
    } अन्यथा {
        फर्काऊ value == encodedType;
    }
}

नामाकाश {

शून्य maybeAppendType(uint32_t value, uint32_t bit, UIdentifierType t,
                     UIdentifierType *types, int32_t &length, int32_t capacity) {
    यदि ((value & bit) != 0) {
        यदि (length < capacity) {
            types[length] = t;
        }
        ++length;
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) { फर्काऊ 0; }
    यदि (capacity < 0 || (capacity > 0 && types == nullptr)) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    यदि ((value & UPROPS_ID_TYPE_FORBIDDEN) == UPROPS_ID_TYPE_FORBIDDEN ||
            value == UPROPS_ID_TYPE_NOT_CHARACTER) {
        // single value
        यदि (capacity > 0) {
            UIdentifierType t;
            विकल्प (value) {
                पक्ष UPROPS_ID_TYPE_NOT_CHARACTER: t = U_ID_TYPE_NOT_CHARACTER; रोक;
                पक्ष UPROPS_ID_TYPE_DEPRECATED: t = U_ID_TYPE_DEPRECATED; रोक;
                पक्ष UPROPS_ID_TYPE_DEFAULT_IGNORABLE: t = U_ID_TYPE_DEFAULT_IGNORABLE; रोक;
                पक्ष UPROPS_ID_TYPE_NOT_NFKC: t = U_ID_TYPE_NOT_NFKC; रोक;
                पक्ष UPROPS_ID_TYPE_INCLUSION: t = U_ID_TYPE_INCLUSION; रोक;
                पक्ष UPROPS_ID_TYPE_RECOMMENDED: t = U_ID_TYPE_RECOMMENDED; रोक;
                स्वभाव:
                    *pErrorCode = U_INVALID_FORMAT_ERROR;
                    फर्काऊ 0;
            }
            types[0] = t;
        } अन्यथा {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        फर्काऊ 1;
    } अन्यथा {
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
        यदि (length >= capacity) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        फर्काऊ length;
    }
}

#यदि !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_getFC_NFKC_Closure(UChar32 c, char16_t *dest, int32_t destCapacity, UErrorCode *pErrorCode) {
    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        फर्काऊ 0;
    }
    यदि(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    अचर Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*pErrorCode);
    यदि(U_FAILURE(*pErrorCode)) {
        फर्काऊ 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    अचर char16_t *folded1;
    int32_t folded1Length=ucase_toFullFolding(c, &folded1, U_FOLD_CASE_DEFAULT);
    यदि(folded1Length<0) {
        अचर Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        यदि(nfkcImpl->getCompQuickCheck(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            फर्काऊ u_terminateUChars(dest, destCapacity, 0, pErrorCode);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.setTo(c);
    } अन्यथा {
        यदि(folded1Length>UCASE_MAX_STRING_LENGTH) {
            folded1String.setTo(folded1Length);
        } अन्यथा {
            folded1String.setTo(असत्य, folded1, folded1Length);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *pErrorCode);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *pErrorCode);
    // if (c != b) add the mapping from a to c
    यदि(U_FAILURE(*pErrorCode) || kc1==kc2) {
        फर्काऊ u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    } अन्यथा {
        फर्काऊ kc2.extract(dest, destCapacity, *pErrorCode);
    }
}

#endif
