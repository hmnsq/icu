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

#समावेश "unicode/utypes.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/ucptrie.h"
#समावेश "unicode/udata.h"
#समावेश "unicode/unorm2.h"
#समावेश "unicode/uscript.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/utf16.h"
#समावेश "cstring.h"
#समावेश "emojiprops.h"
#समावेश "mutex.h"
#समावेश "normalizer2impl.h"
#समावेश "umutex.h"
#समावेश "ubidi_props.h"
#समावेश "uprops.h"
#समावेश "ucase.h"
#समावेश "ucln_cmn.h"
#समावेश "ulayout_props.h"
#समावेश "ustr_imp.h"

U_NAMESPACE_USE

// Unicode text layout properties data -----------------------------------------

namespace {

icu::UInitOnce gLayoutInitOnce {};
UDataMemory *gLayoutMemory = nullptr;

UCPTrie *gInpcTrie = nullptr;  // Indic_Positional_Category
UCPTrie *gInscTrie = nullptr;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = nullptr;  // Vertical_Orientation

int32_t g_Max_Inpc_मूल्य = 0;
int32_t g_Max_Insc_मूल्य = 0;
int32_t g_Max_Vo_मूल्य = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(gLayoutMemory);
    gLayoutMemory = nullptr;

    ucptrie_close(gInpcTrie);
    gInpcTrie = nullptr;
    ucptrie_close(gInscTrie);
    gInscTrie = nullptr;
    ucptrie_close(gVoTrie);
    gVoTrie = nullptr;

    g_Max_Inpc_मूल्य = 0;
    g_Max_Insc_मूल्य = 0;
    g_Max_Vo_मूल्य = 0;

    gLayoutInitOnce.reset();
    वापस true;
}

UBool U_CALLCONV
ulayout_isAcceptable(व्योम * /*context*/,
                     स्थिर अक्षर * /* type */, स्थिर अक्षर * /*name*/,
                     स्थिर UDataInfo *pInfo) {
    वापस pInfo->size >= 20 &&
        pInfo->isBigEndian == U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily == U_CHARSET_FAMILY &&
        pInfo->तथ्य_Format[0] == ULAYOUT_FMT_0 &&
        pInfo->तथ्य_Format[1] == ULAYOUT_FMT_1 &&
        pInfo->तथ्य_Format[2] == ULAYOUT_FMT_2 &&
        pInfo->तथ्य_Format[3] == ULAYOUT_FMT_3 &&
        pInfo->formatVersion[0] == 1;
}

// UInitOnce singleton initialization function
व्योम U_CALLCONV ulayout_load(UErrorCode &errorCode) {
    gLayoutMemory = udata_openChoice(
        nullptr, ULAYOUT_तथ्य_TYPE, ULAYOUT_तथ्य_NAME,
        ulayout_isAcceptable, nullptr, &errorCode);
    अगर (U_FAILURE(errorCode)) { वापस; }

    स्थिर uint8_t* inBytes = static_cast<स्थिर uint8_t*>(udata_getMemory(gLayoutMemory));
    स्थिर int32_t* inIndexes = reinterpret_cast<स्थिर int32_t*>(inBytes);
    int32_t indexes_लंबाई = inIndexes[ULAYOUT_IX_INDEXES_लंबाई];
    अगर (indexes_लंबाई < 12) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        वापस;
    }
    int32_t offset = indexes_लंबाई * 4;
    int32_t शीर्ष_2 = inIndexes[ULAYOUT_IX_INPC_TRIE_शीर्ष];
    int32_t trie_आकार = शीर्ष_2 - offset;
    अगर (trie_आकार >= 16) {
        gInpcTrie = ucptrie_open_From_द्विआधारी(
            UCPTRIE_TYPE_ANY, UCPTRIE_मूल्य_BITS_ANY,
            inBytes + offset, trie_आकार, nullptr, &errorCode);
    }
    offset = शीर्ष_2;
    शीर्ष_2 = inIndexes[ULAYOUT_IX_INSC_TRIE_शीर्ष];
    trie_आकार = शीर्ष_2 - offset;
    अगर (trie_आकार >= 16) {
        gInscTrie = ucptrie_open_From_द्विआधारी(
            UCPTRIE_TYPE_ANY, UCPTRIE_मूल्य_BITS_ANY,
            inBytes + offset, trie_आकार, nullptr, &errorCode);
    }
    offset = शीर्ष_2;
    शीर्ष_2 = inIndexes[ULAYOUT_IX_VO_TRIE_शीर्ष];
    trie_आकार = शीर्ष_2 - offset;
    अगर (trie_आकार >= 16) {
        gVoTrie = ucptrie_open_From_द्विआधारी(
            UCPTRIE_TYPE_ANY, UCPTRIE_मूल्य_BITS_ANY,
            inBytes + offset, trie_आकार, nullptr, &errorCode);
    }

    uint32_t max_मूल्यावली = inIndexes[ULAYOUT_IX_MAX_मूल्यावली];
    g_Max_Inpc_मूल्य = max_मूल्यावली >> ULAYOUT_MAX_INPC_SHIFT;
    g_Max_Insc_मूल्य = (max_मूल्यावली >> ULAYOUT_MAX_INSC_SHIFT) & 0xff;
    g_Max_Vo_मूल्य = (max_मूल्यावली >> ULAYOUT_MAX_VO_SHIFT) & 0xff;

    ucln_common_registerCleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensure_तथ्य(UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस false; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_load, errorCode);
    वापस U_SUCCESS(errorCode);
}

UBool ulayout_ensure_तथ्य() {
    UErrorCode errorCode = U_ZERO_ERROR;
    वापस ulayout_ensure_तथ्य(errorCode);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

काष्ठा द्विआधारी_Property;

प्रकार UBool द्विआधारी_Property_Contains(स्थिर द्विआधारी_Property &prop, UChar32 c, UProperty which);

काष्ठा द्विआधारी_Property {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    द्विआधारी_Property_Contains *contains;
};

अटल UBool defaultContains(स्थिर द्विआधारी_Property &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    वापस (u_getUnicodeProperties(c, prop.column)&prop.mask)!=0;
}

अटल UBool case_द्विआधारी_Property_Contains(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty which) {
    वापस ucase_has_द्विआधारी_Property(c, which);
}

अटल UBool isBidiControl(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस ubidi_isBidiControl(c);
}

अटल UBool isMirrored(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस ubidi_isMirrored(c);
}

अटल UBool isJoinControl(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस ubidi_isJoinControl(c);
}

#अगर UCONFIG_NO_NORMALIZATION
अटल UBool hasFullCompositionExclusion(स्थिर द्विआधारी_Property &, UChar32, UProperty) {
    वापस false;
}
#अन्यथा
अटल UBool hasFullCompositionExclusion(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UErrorCode errorCode=U_ZERO_ERROR;
    स्थिर Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    वापस U_SUCCESS(errorCode) && impl->isCompNo(impl->getNorm16(c));
}
#पूर्ण_अगर

// UCHAR_NF*_INERT properties
#अगर UCONFIG_NO_NORMALIZATION
अटल UBool isNormInert(स्थिर द्विआधारी_Property &, UChar32, UProperty) {
    वापस false;
}
#अन्यथा
अटल UBool isNormInert(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty which) {
    UErrorCode errorCode=U_ZERO_ERROR;
    स्थिर Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalizationMode>(which - UCHAR_NFD_INERT + UNORM_NFD), errorCode);
    वापस U_SUCCESS(errorCode) && norm2->isInert(c);
}
#पूर्ण_अगर

#अगर UCONFIG_NO_NORMALIZATION
अटल UBool changesWhenCasefolded(स्थिर द्विआधारी_Property &, UChar32, UProperty) {
    वापस false;
}
#अन्यथा
अटल UBool changesWhenCasefolded(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UErrorCode errorCode=U_ZERO_ERROR;
    स्थिर Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(errorCode);
    अगर(U_FAILURE(errorCode)) {
        वापस false;
    }
    अगर(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        अगर(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } अन्यथा अगर(nfd.length()<=U16_MAX_लंबाई &&
                  nfd.length()==U16_लंबाई(c=nfd.char32At(0))
        ) {
            /* single supplementary code point */
        } अन्यथा {
            c=U_SENTINEL;
        }
    } अन्यथा अगर(c<0) {
        वापस false;  /* protect against bad input */
    }
    अगर(c>=0) {
        /* single code point */
        स्थिर char16_t *परिणाम_String;
        वापस ucase_toFullFolding(c, &परिणाम_String, U_FOLD_CASE_DEFAULT) >= 0;
    } अन्यथा {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_लंबाई];
        int32_t dest_लंबाई;
        dest_लंबाई=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.get_बफर(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &errorCode);
        वापस U_SUCCESS(errorCode) &&
                       0!=u_strCompare(nfd.get_बफर(), nfd.length(),
                                       dest, dest_लंबाई, false);
    }
}
#पूर्ण_अगर

#अगर UCONFIG_NO_NORMALIZATION
अटल UBool changesWhenNFKC_Casefolded(स्थिर द्विआधारी_Property &, UChar32, UProperty) {
    वापस false;
}
#अन्यथा
अटल UBool changesWhenNFKC_Casefolded(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    स्थिर Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(errorCode);
    अगर(U_FAILURE(errorCode)) {
        वापस false;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        Reordering_बफर बफर(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        अगर(बफर.init(5, errorCode)) {
            स्थिर char16_t *srcArray=src.get_बफर();
            kcf->compose(srcArray, srcArray+src.length(), false,
                          true, बफर, errorCode);
        }
    }
    वापस U_SUCCESS(errorCode) && dest!=src;
}
#पूर्ण_अगर

#अगर UCONFIG_NO_NORMALIZATION
अटल UBool isCanonSegmentStarter(स्थिर द्विआधारी_Property &, UChar32, UProperty) {
    वापस false;
}
#अन्यथा
अटल UBool isCanonSegmentStarter(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    स्थिर Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    वापस
        U_SUCCESS(errorCode) && impl->ensure_Canon_Iter_तथ्य(errorCode) &&
        impl->isCanonSegmentStarter(c);
}
#पूर्ण_अगर

अटल UBool isPOSIX_alnum(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस u_isalnumPOSIX(c);
}

अटल UBool isPOSIX_blank(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस u_isblank(c);
}

अटल UBool isPOSIX_graph(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस u_isgraphPOSIX(c);
}

अटल UBool isPOSIX_print(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस u_isprintPOSIX(c);
}

अटल UBool isPOSIX_xdigit(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस u_isxdigit(c);
}

अटल UBool isRegionalIndicator(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    वापस 0x1F1E6<=c && c<=0x1F1FF;
}

अटल UBool hasEmojiProperty(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty which) {
    वापस EmojiProps::has_द्विआधारी_Property(c, which);
}

अटल UBool isIDSUnaryOperator(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // New in Unicode 15.1 for just two characters.
    वापस 0x2FFE<=c && c<=0x2FFF;
}

/** Ranges (start/limit pairs) of ID_Compat_Math_Continue (only), from UCD PropList.txt. */
अटल constexpr UChar32 ID_COMPAT_MATH_CONTINUE[] = {
    0x00B2, 0x00B3 + 1,
    0x00B9, 0x00B9 + 1,
    0x2070, 0x2070 + 1,
    0x2074, 0x207E + 1,
    0x2080, 0x208E + 1
};

/** ID_Compat_Math_Start characters, from UCD PropList.txt. */
अटल constexpr UChar32 ID_COMPAT_MATH_START[] = {
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
अटल constexpr UChar32 MODIFIER_COMBINING_MARK[] = {
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

अटल UBool isIDCompatMathStart(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    अगर (c < ID_COMPAT_MATH_START[0]) { वापस false; }  // fastpath for common scripts
    क्रम (UChar32 startChar : ID_COMPAT_MATH_START) {
        अगर (c == startChar) { वापस true; }
    }
    वापस false;
}

अटल UBool isIDCompatMathContinue(स्थिर द्विआधारी_Property &prop, UChar32 c, UProperty /*which*/) {
    क्रम (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        अगर (c < ID_COMPAT_MATH_CONTINUE[i]) { वापस false; }  // below range start
        अगर (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { वापस true; }  // below range limit
    }
    वापस isIDCompatMathStart(prop, c, UCHAR_ID_COMPAT_MATH_START);
}

अटल UBool isModifierCombiningMark(स्थिर द्विआधारी_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    क्रम (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        अगर (c < MODIFIER_COMBINING_MARK[i]) { वापस false; }  // below range start
        अगर (c < MODIFIER_COMBINING_MARK[i + 1]) { वापस true; }  // below range limit
    }
    वापस false;
}

अटल स्थिर द्विआधारी_Property binProps[UCHAR_द्विआधारी_LIMIT]={
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
    अगर(which<UCHAR_द्विआधारी_START || UCHAR_द्विआधारी_LIMIT<=which) {
        /* not a known binary property */
        वापस false;
    } अन्यथा {
        स्थिर द्विआधारी_Property &prop=binProps[which];
        वापस prop.contains(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    वापस u_has_द्विआधारी_Property(c, UCHAR_ID_START);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    वापस u_has_द्विआधारी_Property(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_string_Has_द्विआधारी_Property(स्थिर char16_t *s, int32_t length, UProperty which) {
    अगर (s == nullptr && length != 0) { वापस false; }
    अगर (length == 1) {
        वापस u_has_द्विआधारी_Property(s[0], which);  // single code point
    } अन्यथा अगर (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_अग्र(s, i, length, c);
        अगर (length > 0 ? i == length : s[i] == 0) {
            वापस u_has_द्विआधारी_Property(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    वापस UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::has_द्विआधारी_Property(s, length, which);
}

काष्ठा IntProperty;

प्रकार int32_t Int_Property_Get_मूल्य(स्थिर IntProperty &prop, UChar32 c, UProperty which);
प्रकार int32_t Int_Property_Get_Max_मूल्य(स्थिर IntProperty &prop, UProperty which);

काष्ठा IntProperty {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t shift;  // =maxValue if getMaxValueFromShift() is used
    Int_Property_Get_मूल्य *get_मूल्य;
    Int_Property_Get_Max_मूल्य *get_Max_मूल्य;
};

अटल int32_t default_Get_मूल्य(स्थिर IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    वापस static_cast<int32_t>(u_getUnicodeProperties(c, prop.column) & prop.mask) >> prop.shift;
}

अटल int32_t default_Get_Max_मूल्य(स्थिर IntProperty &prop, UProperty /*which*/) {
    वापस (uprv_get_Max_मूल्यावली(prop.column)&prop.mask)>>prop.shift;
}

अटल int32_t get_Max_मूल्य_From_Shift(स्थिर IntProperty &prop, UProperty /*which*/) {
    वापस prop.shift;
}

अटल int32_t getBiDiClass(स्थिर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस static_cast<int32_t>(u_charDirection(c));
}

अटल int32_t getBiDiPairedBracketType(स्थिर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

अटल int32_t bi_Di_Get_Max_मूल्य(स्थिर IntProperty &/*prop*/, UProperty which) {
    वापस ubidi_get_Max_मूल्य(which);
}

अटल int32_t getBlock(स्थिर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस static_cast<int32_t>(ublock_getCode(c));
}

अटल int32_t block_Get_Max_मूल्य(स्थिर IntProperty &/*prop*/, UProperty /*which*/) {
    वापस uprv_get_Max_मूल्यावली(UPROPS_MAX_मूल्यावली_OTHER_सूचकांक) & UPROPS_MAX_BLOCK;
}

#अगर UCONFIG_NO_NORMALIZATION
अटल int32_t getCombiningClass(स्थिर IntProperty &, UChar32, UProperty) {
    वापस 0;
}
#अन्यथा
अटल int32_t getCombiningClass(स्थिर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस u_getCombiningClass(c);
}
#पूर्ण_अगर

अटल int32_t getGeneralCategory(स्थिर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस static_cast<int32_t>(u_charType(c));
}

अटल int32_t getJoiningGroup(स्थिर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस ubidi_getJoiningGroup(c);
}

अटल int32_t getJoiningType(स्थिर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस ubidi_getJoiningType(c);
}

अटल int32_t getNumericType(स्थिर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_मूल्य(u_getMainProperties(c)));
    वापस UPROPS_NTV_GET_TYPE(ntv);
}

अटल int32_t getScript(स्थिर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    वापस static_cast<int32_t>(uscript_getScript(c, &errorCode));
}

अटल int32_t script_Get_Max_मूल्य(स्थिर IntProperty &/*prop*/, UProperty /*which*/) {
    वापस uprv_get_Max_मूल्यावली(0)&UPROPS_MAX_SCRIPT;
}

/*
 * Map some of the Grapheme Cluster Break values to Hangul Syllable Types.
 * Hangul_Syllable_Type used to be fully redundant with a subset of Grapheme_Cluster_Break.
 *
 * Starting with Unicode 16, this is no longer true for HST=V vs. GCB=V in some cases:
 * Some Kirat Rai vowels are given GCB=V for proper grapheme clustering, but
 * they are of course not related to Hangul syllables.
 */
अटल स्थिर UHangulSyllableType gcbToHst[]={
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

अटल int32_t getHangulSyllableType(स्थिर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Ignore supplementary code points: They all have HST=NA.
    // This is a simple way to handle the GCB!=hst cases since Unicode 16 (Kirat Rai vowels).
    अगर(c>0xffff) {
        वापस U_HST_NOT_APPLICABLE;
    }
    /* see comments on gcbToHst[] above */
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_MASK) >> UPROPS_GCB_SHIFT;
    अगर(gcb<UPRV_LENGTHOF(gcbToHst)) {
        वापस gcbToHst[gcb];
    } अन्यथा {
        वापस U_HST_NOT_APPLICABLE;
    }
}

#अगर UCONFIG_NO_NORMALIZATION
अटल int32_t getNormQuickCheck(स्थिर IntProperty &, UChar32, UProperty) {
    वापस 0;
}
#अन्यथा
अटल int32_t getNormQuickCheck(स्थिर IntProperty &/*prop*/, UChar32 c, UProperty which) {
    वापस static_cast<int32_t>(unorm_getQuickCheck(c, static_cast<UNormalizationMode>(which - UCHAR_NFD_QUICK_CHECK + UNORM_NFD)));
}
#पूर्ण_अगर

#अगर UCONFIG_NO_NORMALIZATION
अटल int32_t getLeadCombiningClass(स्थिर IntProperty &, UChar32, UProperty) {
    वापस 0;
}
#अन्यथा
अटल int32_t getLeadCombiningClass(स्थिर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस unorm_getFCD16(c)>>8;
}
#पूर्ण_अगर

#अगर UCONFIG_NO_NORMALIZATION
अटल int32_t getTrailCombiningClass(स्थिर IntProperty &, UChar32, UProperty) {
    वापस 0;
}
#अन्यथा
अटल int32_t getTrailCombiningClass(स्थिर IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    वापस unorm_getFCD16(c)&0xff;
}
#पूर्ण_अगर

अटल int32_t getInPC(स्थिर IntProperty &, UChar32 c, UProperty) {
    वापस ulayout_ensure_तथ्य() && gInpcTrie != nullptr ? ucptrie_get(gInpcTrie, c) : 0;
}

अटल int32_t getInSC(स्थिर IntProperty &, UChar32 c, UProperty) {
    वापस ulayout_ensure_तथ्य() && gInscTrie != nullptr ? ucptrie_get(gInscTrie, c) : 0;
}

अटल int32_t getVo(स्थिर IntProperty &, UChar32 c, UProperty) {
    वापस ulayout_ensure_तथ्य() && gVoTrie != nullptr ? ucptrie_get(gVoTrie, c) : 0;
}

अटल int32_t layout_Get_Max_मूल्य(स्थिर IntProperty &/*prop*/, UProperty which) {
    अगर (!ulayout_ensure_तथ्य()) { वापस 0; }
    चयन (which) {
    हाल UCHAR_INDIC_POSITIONAL_CATEGORY:
        वापस g_Max_Inpc_मूल्य;
    हाल UCHAR_INDIC_SYLLABIC_CATEGORY:
        वापस g_Max_Insc_मूल्य;
    हाल UCHAR_VERTICAL_ORIENTATION:
        वापस g_Max_Vo_मूल्य;
    शेष:
        वापस 0;
    }
}

अटल int32_t get_IDStatus_मूल्य(स्थिर IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    वापस value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_STATUS_ALLOWED : U_ID_STATUS_RESTRICTED;
}

अटल स्थिर IntProperty intProps[UCHAR_INT_LIMIT-UCHAR_INT_START]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, bi_Di_Get_Max_मूल्य },
    { UPROPS_SRC_BLOCK, 0, 0,                               getBlock, block_Get_Max_मूल्य },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, get_Max_मूल्य_From_Shift },
    { 2,                UPROPS_DT_MASK, 0,                  default_Get_मूल्य, default_Get_Max_मूल्य },
    { 0,                UPROPS_EA_MASK, UPROPS_EA_SHIFT,    default_Get_मूल्य, default_Get_Max_मूल्य },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_गिनती) - 1, getGeneralCategory, get_Max_मूल्य_From_Shift },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningGroup, bi_Di_Get_Max_मूल्य },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, bi_Di_Get_Max_मूल्य },
    { 2,                UPROPS_LB_MASK, UPROPS_LB_SHIFT,    default_Get_मूल्य, default_Get_Max_मूल्य },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_गिनती) - 1, getNumericType, get_Max_मूल्य_From_Shift },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, script_Get_Max_मूल्य },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_गिनती) - 1, getHangulSyllableType, get_Max_मूल्य_From_Shift },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, get_Max_मूल्य_From_Shift },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, get_Max_मूल्य_From_Shift },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, get_Max_मूल्य_From_Shift },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, get_Max_मूल्य_From_Shift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, get_Max_मूल्य_From_Shift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, get_Max_मूल्य_From_Shift },
    { 2,                UPROPS_GCB_MASK, UPROPS_GCB_SHIFT,  default_Get_मूल्य, default_Get_Max_मूल्य },
    { 2,                UPROPS_SB_MASK, UPROPS_SB_SHIFT,    default_Get_मूल्य, default_Get_Max_मूल्य },
    { 2,                UPROPS_WB_MASK, UPROPS_WB_SHIFT,    default_Get_मूल्य, default_Get_Max_मूल्य },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, bi_Di_Get_Max_मूल्य },
    { UPROPS_SRC_INPC,  0, 0,                               getInPC, layout_Get_Max_मूल्य },
    { UPROPS_SRC_INSC,  0, 0,                               getInSC, layout_Get_Max_मूल्य },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layout_Get_Max_मूल्य },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_STATUS_ALLOWED), get_IDStatus_मूल्य, get_Max_मूल्य_From_Shift },
    { 0,                UPROPS_INCB_MASK, UPROPS_INCB_SHIFT,default_Get_मूल्य, default_Get_Max_मूल्य },
};

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_मूल्य(UChar32 c, UProperty which) {
    अगर(which<UCHAR_INT_START) {
        अगर(UCHAR_द्विआधारी_START<=which && which<UCHAR_द्विआधारी_LIMIT) {
            स्थिर द्विआधारी_Property &prop=binProps[which];
            वापस prop.contains(prop, c, which);
        }
    } अन्यथा अगर(which<UCHAR_INT_LIMIT) {
        स्थिर IntProperty &prop=intProps[which-UCHAR_INT_START];
        वापस prop.get_मूल्य(prop, c, which);
    } अन्यथा अगर(which==UCHAR_GENERAL_CATEGORY_MASK) {
        वापस U_MASK(u_charType(c));
    }
    वापस 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Min_मूल्य(UProperty /*which*/) {
    वापस 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Max_मूल्य(UProperty which) {
    अगर(which<UCHAR_INT_START) {
        अगर(UCHAR_द्विआधारी_START<=which && which<UCHAR_द्विआधारी_LIMIT) {
            वापस 1;  // maximum true for all binary properties
        }
    } अन्यथा अगर(which<UCHAR_INT_LIMIT) {
        स्थिर IntProperty &prop=intProps[which-UCHAR_INT_START];
        वापस prop.get_Max_मूल्य(prop, which);
    }
    वापस -1;  // undefined
}

U_CFUNC UPropertySource U_EXPORT2
uprops_getSource(UProperty which) {
    अगर(which<UCHAR_द्विआधारी_START) {
        वापस UPROPS_SRC_NONE; /* undefined */
    } अन्यथा अगर(which<UCHAR_द्विआधारी_LIMIT) {
        स्थिर द्विआधारी_Property &prop=binProps[which];
        अगर(prop.mask!=0) {
            वापस UPROPS_SRC_PROPSVEC;
        } अन्यथा {
            वापस (UPropertySource)prop.column;
        }
    } अन्यथा अगर(which<UCHAR_INT_START) {
        वापस UPROPS_SRC_NONE; /* undefined */
    } अन्यथा अगर(which<UCHAR_INT_LIMIT) {
        स्थिर IntProperty &prop=intProps[which-UCHAR_INT_START];
        अगर(prop.mask!=0) {
            वापस UPROPS_SRC_PROPSVEC;
        } अन्यथा {
            वापस (UPropertySource)prop.column;
        }
    } अन्यथा अगर(which<UCHAR_STRING_START) {
        चयन(which) {
        हाल UCHAR_GENERAL_CATEGORY_MASK:
        हाल UCHAR_NUMERIC_मूल्य:
            वापस UPROPS_SRC_CHAR;

        शेष:
            वापस UPROPS_SRC_NONE;
        }
    } अन्यथा अगर(which<UCHAR_STRING_LIMIT) {
        चयन(which) {
        हाल UCHAR_AGE:
            वापस UPROPS_SRC_PROPSVEC;

        हाल UCHAR_BIDI_MIRRORING_GLYPH:
            वापस UPROPS_SRC_BIDI;

        हाल UCHAR_CASE_FOLDING:
        हाल UCHAR_LOWERCASE_MAPPING:
        हाल UCHAR_SIMPLE_CASE_FOLDING:
        हाल UCHAR_SIMPLE_LOWERCASE_MAPPING:
        हाल UCHAR_SIMPLE_TITLECASE_MAPPING:
        हाल UCHAR_SIMPLE_UPPERCASE_MAPPING:
        हाल UCHAR_TITLECASE_MAPPING:
        हाल UCHAR_UPPERCASE_MAPPING:
            वापस UPROPS_SRC_CASE;

        हाल UCHAR_ISO_COMMENT:
        हाल UCHAR_NAME:
        हाल UCHAR_UNICODE_1_NAME:
            वापस UPROPS_SRC_NAMES;

        शेष:
            वापस UPROPS_SRC_NONE;
        }
    } अन्यथा {
        चयन(which) {
        हाल UCHAR_SCRIPT_EXTENSIONS:
        हाल UCHAR_IDENTIFIER_TYPE:
            वापस UPROPS_SRC_PROPSVEC;
        शेष:
            वापस UPROPS_SRC_NONE; /* undefined */
        }
    }
}

U_CFUNC व्योम U_EXPORT2
uprops_addPropertyStarts(UPropertySource src, स्थिर USetAdder *sa, UErrorCode *pErrorCode) {
    अगर (U_FAILURE(*pErrorCode)) { वापस; }
    अगर (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        क्रम (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->add(sa->set, c);
        }
        // single characters
        क्रम (UChar32 c : ID_COMPAT_MATH_START) {
            sa->add(sa->set, c);
            sa->add(sa->set, c + 1);
        }
        वापस;
    }
    अगर (src == UPROPS_SRC_MCM) {
        // range limits
        क्रम (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->add(sa->set, c);
        }
        वापस;
    }
    अगर (!ulayout_ensure_तथ्य(*pErrorCode)) { वापस; }
    स्थिर UCPTrie *trie;
    चयन (src) {
    हाल UPROPS_SRC_INPC:
        trie = gInpcTrie;
        अवरोध;
    हाल UPROPS_SRC_INSC:
        trie = gInscTrie;
        अवरोध;
    हाल UPROPS_SRC_VO:
        trie = gVoTrie;
        अवरोध;
    शेष:
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    अगर (trie == nullptr) {
        *pErrorCode = U_MISSING_RESOURCE_ERROR;
        वापस;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    जबतक ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, nullptr)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

U_CAPI bool U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t type_सूचकांक = type;  // also guards against negative type integers
    अगर (type_सूचकांक >= UPRV_LENGTHOF(uprops_idTypeToEncoded)) {
        वापस false;
    }
    uint32_t encodedType = uprops_idTypeToEncoded[type_सूचकांक];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    अगर ((encodedType & UPROPS_ID_TYPE_BIT) != 0) {
        वापस value < UPROPS_ID_TYPE_FORBIDDEN && (value & encodedType) != 0;
    } अन्यथा {
        वापस value == encodedType;
    }
}

namespace {

व्योम maybeAppendType(uint32_t value, uint32_t bit, UIdentifierType t,
                     UIdentifierType *types, int32_t &length, int32_t capacity) {
    अगर ((value & bit) != 0) {
        अगर (length < capacity) {
            types[length] = t;
        }
        ++length;
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UErrorCode *pErrorCode) {
    अगर (U_FAILURE(*pErrorCode)) { वापस 0; }
    अगर (capacity < 0 || (capacity > 0 && types == nullptr)) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    अगर ((value & UPROPS_ID_TYPE_FORBIDDEN) == UPROPS_ID_TYPE_FORBIDDEN ||
            value == UPROPS_ID_TYPE_NOT_CHARACTER) {
        // single value
        अगर (capacity > 0) {
            UIdentifierType t;
            चयन (value) {
                हाल UPROPS_ID_TYPE_NOT_CHARACTER: t = U_ID_TYPE_NOT_CHARACTER; अवरोध;
                हाल UPROPS_ID_TYPE_DEPRECATED: t = U_ID_TYPE_DEPRECATED; अवरोध;
                हाल UPROPS_ID_TYPE_DEFAULT_IGNORABLE: t = U_ID_TYPE_DEFAULT_IGNORABLE; अवरोध;
                हाल UPROPS_ID_TYPE_NOT_NFKC: t = U_ID_TYPE_NOT_NFKC; अवरोध;
                हाल UPROPS_ID_TYPE_INCLUSION: t = U_ID_TYPE_INCLUSION; अवरोध;
                हाल UPROPS_ID_TYPE_RECOMMENDED: t = U_ID_TYPE_RECOMMENDED; अवरोध;
                शेष:
                    *pErrorCode = U_INVALID_FORMAT_ERROR;
                    वापस 0;
            }
            types[0] = t;
        } अन्यथा {
            *pErrorCode = U_बफर_OVERFLOW_ERROR;
        }
        वापस 1;
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
        अगर (length >= capacity) {
            *pErrorCode = U_बफर_OVERFLOW_ERROR;
        }
        वापस length;
    }
}

#अगर !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_getFC_NFKC_Closure(UChar32 c, char16_t *dest, int32_t destCapacity, UErrorCode *pErrorCode) {
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    स्थिर Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    स्थिर char16_t *folded1;
    int32_t folded1_लंबाई=ucase_toFullFolding(c, &folded1, U_FOLD_CASE_DEFAULT);
    अगर(folded1_लंबाई<0) {
        स्थिर Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        अगर(nfkcImpl->getCompQuickCheck(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            वापस u_terminateUChars(dest, destCapacity, 0, pErrorCode);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.setTo(c);
    } अन्यथा {
        अगर(folded1_लंबाई>UCASE_MAX_STRING_लंबाई) {
            folded1String.setTo(folded1_लंबाई);
        } अन्यथा {
            folded1String.setTo(false, folded1, folded1_लंबाई);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *pErrorCode);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *pErrorCode);
    // if (c != b) add the mapping from a to c
    अगर(U_FAILURE(*pErrorCode) || kc1==kc2) {
        वापस u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    } अन्यथा {
        वापस kc2.extract(dest, destCapacity, *pErrorCode);
    }
}

#पूर्ण_अगर
