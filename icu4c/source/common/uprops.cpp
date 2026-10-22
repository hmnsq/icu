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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/uchar.h"
#અન્તર્ભાવ "unicode/ucptrie.h"
#અન્તર્ભાવ "unicode/udata.h"
#અન્તર્ભાવ "unicode/unorm2.h"
#અન્તર્ભાવ "unicode/uscript.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "emojiprops.h"
#અન્તર્ભાવ "mutex.h"
#અન્તર્ભાવ "normalizer2impl.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "ubidi_props.h"
#અન્તર્ભાવ "uprops.h"
#અન્તર્ભાવ "ucase.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "ulayout_props.h"
#અન્તર્ભાવ "ustr_imp.h"

U_NAMESPACE_USE

// Unicode text layout properties data -----------------------------------------

નામાકાશ {

icu::UInitOnce gLayoutInitOnce {};
UDataMemory *gLayoutMemory = nullptr;

UCPTrie *gInpcTrie = nullptr;  // Indic_Positional_Category
UCPTrie *gInscTrie = nullptr;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = nullptr;  // Vertical_Orientation

int32_t g_Max_Inpc_કિંમત = 0;
int32_t g_Max_Insc_કિંમત = 0;
int32_t g_Max_Vo_કિંમત = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(gLayoutMemory);
    gLayoutMemory = nullptr;

    ucptrie_close(gInpcTrie);
    gInpcTrie = nullptr;
    ucptrie_close(gInscTrie);
    gInscTrie = nullptr;
    ucptrie_close(gVoTrie);
    gVoTrie = nullptr;

    g_Max_Inpc_કિંમત = 0;
    g_Max_Insc_કિંમત = 0;
    g_Max_Vo_કિંમત = 0;

    gLayoutInitOnce.reset();
    પાછા સાચું;
}

UBool U_CALLCONV
ulayout_isAcceptable(શૂન્ય * /*context*/,
                     અચલ અક્ષર * /* type */, અચલ અક્ષર * /*name*/,
                     અચલ UDataInfo *pInfo) {
    પાછા pInfo->size >= 20 &&
        pInfo->isBigEndian == U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily == U_CHARSET_FAMILY &&
        pInfo->dataFormat[0] == ULAYOUT_FMT_0 &&
        pInfo->dataFormat[1] == ULAYOUT_FMT_1 &&
        pInfo->dataFormat[2] == ULAYOUT_FMT_2 &&
        pInfo->dataFormat[3] == ULAYOUT_FMT_3 &&
        pInfo->formatVersion[0] == 1;
}

// UInitOnce singleton initialization function
શૂન્ય U_CALLCONV ulayout_load(UErrorCode &errorCode) {
    gLayoutMemory = udata_openChoice(
        nullptr, ULAYOUT_DATA_TYPE, ULAYOUT_DATA_NAME,
        ulayout_isAcceptable, nullptr, &errorCode);
    જો (U_FAILURE(errorCode)) { પાછા; }

    અચલ uint8_t* inBytes = static_cast<અચલ uint8_t*>(udata_getMemory(gLayoutMemory));
    અચલ int32_t* inIndexes = reinterpret_cast<અચલ int32_t*>(inBytes);
    int32_t indexesLength = inIndexes[ULAYOUT_IX_INDEXES_LENGTH];
    જો (indexesLength < 12) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        પાછા;
    }
    int32_t offset = indexesLength * 4;
    int32_t ટોચ = inIndexes[ULAYOUT_IX_INPC_TRIE_ટોચ];
    int32_t trieSize = ટોચ - offset;
    જો (trieSize >= 16) {
        gInpcTrie = ucptrie_open_From_દ્વિઅંકી(
            UCPTRIE_TYPE_ANY, UCPTRIE_કિંમત_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }
    offset = ટોચ;
    ટોચ = inIndexes[ULAYOUT_IX_INSC_TRIE_ટોચ];
    trieSize = ટોચ - offset;
    જો (trieSize >= 16) {
        gInscTrie = ucptrie_open_From_દ્વિઅંકી(
            UCPTRIE_TYPE_ANY, UCPTRIE_કિંમત_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }
    offset = ટોચ;
    ટોચ = inIndexes[ULAYOUT_IX_VO_TRIE_ટોચ];
    trieSize = ટોચ - offset;
    જો (trieSize >= 16) {
        gVoTrie = ucptrie_open_From_દ્વિઅંકી(
            UCPTRIE_TYPE_ANY, UCPTRIE_કિંમત_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }

    uint32_t max_કિંમતો = inIndexes[ULAYOUT_IX_MAX_કિંમતો];
    g_Max_Inpc_કિંમત = max_કિંમતો >> ULAYOUT_MAX_INPC_SHIFT;
    g_Max_Insc_કિંમત = (max_કિંમતો >> ULAYOUT_MAX_INSC_SHIFT) & 0xff;
    g_Max_Vo_કિંમત = (max_કિંમતો >> ULAYOUT_MAX_VO_SHIFT) & 0xff;

    ucln_common_registerCleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensureData(UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_load, errorCode);
    પાછા U_SUCCESS(errorCode);
}

UBool ulayout_ensureData() {
    UErrorCode errorCode = U_ZERO_ERROR;
    પાછા ulayout_ensureData(errorCode);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

સંરચના દ્વિઅંકી_Property;

પ્રકારવ્યાખ્યા UBool દ્વિઅંકી_Property_Contains(અચલ દ્વિઅંકી_Property &prop, UChar32 c, UProperty which);

સંરચના દ્વિઅંકી_Property {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    દ્વિઅંકી_Property_Contains *contains;
};

સ્થિર UBool defaultContains(અચલ દ્વિઅંકી_Property &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    પાછા (u_getUnicodeProperties(c, prop.column)&prop.mask)!=0;
}

સ્થિર UBool case_દ્વિઅંકી_Property_Contains(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty which) {
    પાછા ucase_has_દ્વિઅંકી_Property(c, which);
}

સ્થિર UBool isBidiControl(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા ubidi_isBidiControl(c);
}

સ્થિર UBool isMirrored(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા ubidi_isMirrored(c);
}

સ્થિર UBool isJoinControl(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા ubidi_isJoinControl(c);
}

#જો UCONFIG_NO_NORMALIZATION
સ્થિર UBool hasFullCompositionExclusion(અચલ દ્વિઅંકી_Property &, UChar32, UProperty) {
    પાછા ખોટું;
}
#નહીંતર
સ્થિર UBool hasFullCompositionExclusion(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UErrorCode errorCode=U_ZERO_ERROR;
    અચલ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    પાછા U_SUCCESS(errorCode) && impl->isCompNo(impl->getNorm16(c));
}
#endif

// UCHAR_NF*_INERT properties
#જો UCONFIG_NO_NORMALIZATION
સ્થિર UBool isNormInert(અચલ દ્વિઅંકી_Property &, UChar32, UProperty) {
    પાછા ખોટું;
}
#નહીંતર
સ્થિર UBool isNormInert(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty which) {
    UErrorCode errorCode=U_ZERO_ERROR;
    અચલ Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalizationMode>(which - UCHAR_NFD_INERT + UNORM_NFD), errorCode);
    પાછા U_SUCCESS(errorCode) && norm2->isInert(c);
}
#endif

#જો UCONFIG_NO_NORMALIZATION
સ્થિર UBool changesWhenCasefolded(અચલ દ્વિઅંકી_Property &, UChar32, UProperty) {
    પાછા ખોટું;
}
#નહીંતર
સ્થિર UBool changesWhenCasefolded(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UErrorCode errorCode=U_ZERO_ERROR;
    અચલ Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(errorCode);
    જો(U_FAILURE(errorCode)) {
        પાછા ખોટું;
    }
    જો(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        જો(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } નહીંતર જો(nfd.length()<=U16_MAX_LENGTH &&
                  nfd.length()==U16_LENGTH(c=nfd.char32At(0))
        ) {
            /* single supplementary code point */
        } નહીંતર {
            c=U_SENTINEL;
        }
    } નહીંતર જો(c<0) {
        પાછા ખોટું;  /* protect against bad input */
    }
    જો(c>=0) {
        /* single code point */
        અચલ char16_t *resultString;
        પાછા ucase_toFullFolding(c, &resultString, U_FOLD_CASE_DEFAULT) >= 0;
    } નહીંતર {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_LENGTH];
        int32_t destLength;
        destLength=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.getBuffer(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &errorCode);
        પાછા U_SUCCESS(errorCode) &&
                       0!=u_strCompare(nfd.getBuffer(), nfd.length(),
                                       dest, destLength, ખોટું);
    }
}
#endif

#જો UCONFIG_NO_NORMALIZATION
સ્થિર UBool changesWhenNFKC_Casefolded(અચલ દ્વિઅંકી_Property &, UChar32, UProperty) {
    પાછા ખોટું;
}
#નહીંતર
સ્થિર UBool changesWhenNFKC_Casefolded(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    અચલ Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(errorCode);
    જો(U_FAILURE(errorCode)) {
        પાછા ખોટું;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        ReorderingBuffer buffer(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        જો(buffer.init(5, errorCode)) {
            અચલ char16_t *srcArray=src.getBuffer();
            kcf->compose(srcArray, srcArray+src.length(), ખોટું,
                          સાચું, buffer, errorCode);
        }
    }
    પાછા U_SUCCESS(errorCode) && dest!=src;
}
#endif

#જો UCONFIG_NO_NORMALIZATION
સ્થિર UBool isCanonSegmentStarter(અચલ દ્વિઅંકી_Property &, UChar32, UProperty) {
    પાછા ખોટું;
}
#નહીંતર
સ્થિર UBool isCanonSegmentStarter(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    અચલ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    પાછા
        U_SUCCESS(errorCode) && impl->ensureCanonIterData(errorCode) &&
        impl->isCanonSegmentStarter(c);
}
#endif

સ્થિર UBool isPOSIX_alnum(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા u_isalnumPOSIX(c);
}

સ્થિર UBool isPOSIX_blank(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા u_isblank(c);
}

સ્થિર UBool isPOSIX_graph(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા u_isgraphPOSIX(c);
}

સ્થિર UBool isPOSIX_print(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા u_isprintPOSIX(c);
}

સ્થિર UBool isPOSIX_xdigit(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા u_isxdigit(c);
}

સ્થિર UBool isRegionalIndicator(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    પાછા 0x1F1E6<=c && c<=0x1F1FF;
}

સ્થિર UBool hasEmojiProperty(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty which) {
    પાછા EmojiProps::has_દ્વિઅંકી_Property(c, which);
}

સ્થિર UBool isIDSUnaryOperator(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // New in Unicode 15.1 for just two characters.
    પાછા 0x2FFE<=c && c<=0x2FFF;
}

/** Ranges (start/limit pairs) of ID_Compat_Math_Continue (only), from UCD PropList.txt. */
સ્થિર constexpr UChar32 ID_COMPAT_MATH_CONTINUE[] = {
    0x00B2, 0x00B3 + 1,
    0x00B9, 0x00B9 + 1,
    0x2070, 0x2070 + 1,
    0x2074, 0x207E + 1,
    0x2080, 0x208E + 1
};

/** ID_Compat_Math_Start characters, from UCD PropList.txt. */
સ્થિર constexpr UChar32 ID_COMPAT_MATH_START[] = {
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
સ્થિર constexpr UChar32 MODIFIER_COMBINING_MARK[] = {
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

સ્થિર UBool isIDCompatMathStart(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    જો (c < ID_COMPAT_MATH_START[0]) { પાછા ખોટું; }  // fastpath for common scripts
    માટે (UChar32 startChar : ID_COMPAT_MATH_START) {
        જો (c == startChar) { પાછા સાચું; }
    }
    પાછા ખોટું;
}

સ્થિર UBool isIDCompatMathContinue(અચલ દ્વિઅંકી_Property &prop, UChar32 c, UProperty /*which*/) {
    માટે (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        જો (c < ID_COMPAT_MATH_CONTINUE[i]) { પાછા ખોટું; }  // below range start
        જો (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { પાછા સાચું; }  // below range limit
    }
    પાછા isIDCompatMathStart(prop, c, UCHAR_ID_COMPAT_MATH_START);
}

સ્થિર UBool isModifierCombiningMark(અચલ દ્વિઅંકી_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    માટે (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        જો (c < MODIFIER_COMBINING_MARK[i]) { પાછા ખોટું; }  // below range start
        જો (c < MODIFIER_COMBINING_MARK[i + 1]) { પાછા સાચું; }  // below range limit
    }
    પાછા ખોટું;
}

સ્થિર અચલ દ્વિઅંકી_Property binProps[UCHAR_દ્વિઅંકી_LIMIT]={
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
    { 1,                U_MASK(UPROPS_IDS_દ્વિઅંકી_OPERATOR), defaultContains },
    { 1,                U_MASK(UPROPS_IDS_TRINARY_OPERATOR), defaultContains },
    { UPROPS_SRC_BIDI,  0, isJoinControl },
    { 1,                U_MASK(UPROPS_LOGICAL_ORDER_EXCEPTION), defaultContains },
    { UPROPS_SRC_CASE,  0, case_દ્વિઅંકી_Property_Contains },  // UCHAR_LOWERCASE
    { 1,                U_MASK(UPROPS_MATH), defaultContains },
    { 1,                U_MASK(UPROPS_NONCHARACTER_CODE_POINT), defaultContains },
    { 1,                U_MASK(UPROPS_QUOTATION_MARK), defaultContains },
    { 1,                U_MASK(UPROPS_RADICAL), defaultContains },
    { UPROPS_SRC_CASE,  0, case_દ્વિઅંકી_Property_Contains },  // UCHAR_SOFT_DOTTED
    { 1,                U_MASK(UPROPS_TERMINAL_PUNCTUATION), defaultContains },
    { 1,                U_MASK(UPROPS_UNIFIED_IDEOGRAPH), defaultContains },
    { UPROPS_SRC_CASE,  0, case_દ્વિઅંકી_Property_Contains },  // UCHAR_UPPERCASE
    { 1,                U_MASK(UPROPS_WHITE_SPACE), defaultContains },
    { 1,                U_MASK(UPROPS_XID_CONTINUE), defaultContains },
    { 1,                U_MASK(UPROPS_XID_START), defaultContains },
    { UPROPS_SRC_CASE,  0, case_દ્વિઅંકી_Property_Contains },  // UCHAR_CASE_SENSITIVE
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
    { UPROPS_SRC_CASE,  0, case_દ્વિઅંકી_Property_Contains },  // UCHAR_CASED
    { UPROPS_SRC_CASE,  0, case_દ્વિઅંકી_Property_Contains },  // UCHAR_CASE_IGNORABLE
    { UPROPS_SRC_CASE,  0, case_દ્વિઅંકી_Property_Contains },  // UCHAR_CHANGES_WHEN_LOWERCASED
    { UPROPS_SRC_CASE,  0, case_દ્વિઅંકી_Property_Contains },  // UCHAR_CHANGES_WHEN_UPPERCASED
    { UPROPS_SRC_CASE,  0, case_દ્વિઅંકી_Property_Contains },  // UCHAR_CHANGES_WHEN_TITLECASED
    { UPROPS_SRC_CASE_AND_NORM,  0, changesWhenCasefolded },
    { UPROPS_SRC_CASE,  0, case_દ્વિઅંકી_Property_Contains },  // UCHAR_CHANGES_WHEN_CASEMAPPED
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
u_has_દ્વિઅંકી_Property(UChar32 c, UProperty which) {
    /* c is range-checked in the functions that are called from here */
    જો(which<UCHAR_દ્વિઅંકી_START || UCHAR_દ્વિઅંકી_LIMIT<=which) {
        /* not a known binary property */
        પાછા ખોટું;
    } નહીંતર {
        અચલ દ્વિઅંકી_Property &prop=binProps[which];
        પાછા prop.contains(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    પાછા u_has_દ્વિઅંકી_Property(c, UCHAR_ID_START);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    પાછા u_has_દ્વિઅંકી_Property(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_string_Has_દ્વિઅંકી_Property(અચલ char16_t *s, int32_t length, UProperty which) {
    જો (s == nullptr && length != 0) { પાછા ખોટું; }
    જો (length == 1) {
        પાછા u_has_દ્વિઅંકી_Property(s[0], which);  // single code point
    } નહીંતર જો (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        જો (length > 0 ? i == length : s[i] == 0) {
            પાછા u_has_દ્વિઅંકી_Property(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    પાછા UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::has_દ્વિઅંકી_Property(s, length, which);
}

સંરચના IntProperty;

પ્રકારવ્યાખ્યા int32_t Int_Property_Get_કિંમત(અચલ IntProperty &prop, UChar32 c, UProperty which);
પ્રકારવ્યાખ્યા int32_t Int_Property_Get_Max_કિંમત(અચલ IntProperty &prop, UProperty which);

સંરચના IntProperty {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t shift;  // =maxValue if getMaxValueFromShift() is used
    Int_Property_Get_કિંમત *get_કિંમત;
    Int_Property_Get_Max_કિંમત *get_Max_કિંમત;
};

સ્થિર int32_t default_Get_કિંમત(અચલ IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    પાછા static_cast<int32_t>(u_getUnicodeProperties(c, prop.column) & prop.mask) >> prop.shift;
}

સ્થિર int32_t default_Get_Max_કિંમત(અચલ IntProperty &prop, UProperty /*which*/) {
    પાછા (uprv_get_Max_કિંમતો(prop.column)&prop.mask)>>prop.shift;
}

સ્થિર int32_t get_Max_કિંમત_From_Shift(અચલ IntProperty &prop, UProperty /*which*/) {
    પાછા prop.shift;
}

સ્થિર int32_t getBiDiClass(અચલ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા static_cast<int32_t>(u_charDirection(c));
}

સ્થિર int32_t getBiDiPairedBracketType(અચલ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

સ્થિર int32_t bi_Di_Get_Max_કિંમત(અચલ IntProperty &/*prop*/, UProperty which) {
    પાછા ubidi_get_Max_કિંમત(which);
}

સ્થિર int32_t getBlock(અચલ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા static_cast<int32_t>(ublock_getCode(c));
}

સ્થિર int32_t block_Get_Max_કિંમત(અચલ IntProperty &/*prop*/, UProperty /*which*/) {
    પાછા uprv_get_Max_કિંમતો(UPROPS_MAX_કિંમતો_OTHER_INDEX) & UPROPS_MAX_BLOCK;
}

#જો UCONFIG_NO_NORMALIZATION
સ્થિર int32_t getCombiningClass(અચલ IntProperty &, UChar32, UProperty) {
    પાછા 0;
}
#નહીંતર
સ્થિર int32_t getCombiningClass(અચલ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા u_getCombiningClass(c);
}
#endif

સ્થિર int32_t getGeneralCategory(અચલ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા static_cast<int32_t>(u_charType(c));
}

સ્થિર int32_t getJoiningGroup(અચલ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા ubidi_getJoiningGroup(c);
}

સ્થિર int32_t getJoiningType(અચલ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા ubidi_getJoiningType(c);
}

સ્થિર int32_t getNumericType(અચલ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_કિંમત(u_getMainProperties(c)));
    પાછા UPROPS_NTV_GET_TYPE(ntv);
}

સ્થિર int32_t getScript(અચલ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    પાછા static_cast<int32_t>(uscript_getScript(c, &errorCode));
}

સ્થિર int32_t script_Get_Max_કિંમત(અચલ IntProperty &/*prop*/, UProperty /*which*/) {
    પાછા uprv_get_Max_કિંમતો(0)&UPROPS_MAX_SCRIPT;
}

/*
 * Map some of the Grapheme Cluster Break values to Hangul Syllable Types.
 * Hangul_Syllable_Type used to be fully redundant with a subset of Grapheme_Cluster_Break.
 *
 * Starting with Unicode 16, this is no longer true for HST=V vs. GCB=V in some cases:
 * Some Kirat Rai vowels are given GCB=V for proper grapheme clustering, but
 * they are of course not related to Hangul syllables.
 */
સ્થિર અચલ UHangulSyllableType gcbToHst[]={
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

સ્થિર int32_t getHangulSyllableType(અચલ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Ignore supplementary code points: They all have HST=NA.
    // This is a simple way to handle the GCB!=hst cases since Unicode 16 (Kirat Rai vowels).
    જો(c>0xffff) {
        પાછા U_HST_NOT_APPLICABLE;
    }
    /* see comments on gcbToHst[] above */
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_MASK) >> UPROPS_GCB_SHIFT;
    જો(gcb<UPRV_LENGTHOF(gcbToHst)) {
        પાછા gcbToHst[gcb];
    } નહીંતર {
        પાછા U_HST_NOT_APPLICABLE;
    }
}

#જો UCONFIG_NO_NORMALIZATION
સ્થિર int32_t getNormQuickCheck(અચલ IntProperty &, UChar32, UProperty) {
    પાછા 0;
}
#નહીંતર
સ્થિર int32_t getNormQuickCheck(અચલ IntProperty &/*prop*/, UChar32 c, UProperty which) {
    પાછા static_cast<int32_t>(unorm_getQuickCheck(c, static_cast<UNormalizationMode>(which - UCHAR_NFD_QUICK_CHECK + UNORM_NFD)));
}
#endif

#જો UCONFIG_NO_NORMALIZATION
સ્થિર int32_t getLeadCombiningClass(અચલ IntProperty &, UChar32, UProperty) {
    પાછા 0;
}
#નહીંતર
સ્થિર int32_t getLeadCombiningClass(અચલ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા unorm_getFCD16(c)>>8;
}
#endif

#જો UCONFIG_NO_NORMALIZATION
સ્થિર int32_t getTrailCombiningClass(અચલ IntProperty &, UChar32, UProperty) {
    પાછા 0;
}
#નહીંતર
સ્થિર int32_t getTrailCombiningClass(અચલ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    પાછા unorm_getFCD16(c)&0xff;
}
#endif

સ્થિર int32_t getInPC(અચલ IntProperty &, UChar32 c, UProperty) {
    પાછા ulayout_ensureData() && gInpcTrie != nullptr ? ucptrie_get(gInpcTrie, c) : 0;
}

સ્થિર int32_t getInSC(અચલ IntProperty &, UChar32 c, UProperty) {
    પાછા ulayout_ensureData() && gInscTrie != nullptr ? ucptrie_get(gInscTrie, c) : 0;
}

સ્થિર int32_t getVo(અચલ IntProperty &, UChar32 c, UProperty) {
    પાછા ulayout_ensureData() && gVoTrie != nullptr ? ucptrie_get(gVoTrie, c) : 0;
}

સ્થિર int32_t layout_Get_Max_કિંમત(અચલ IntProperty &/*prop*/, UProperty which) {
    જો (!ulayout_ensureData()) { પાછા 0; }
    પસંદગી (which) {
    સ્થિતિ UCHAR_INDIC_POSITIONAL_CATEGORY:
        પાછા g_Max_Inpc_કિંમત;
    સ્થિતિ UCHAR_INDIC_SYLLABIC_CATEGORY:
        પાછા g_Max_Insc_કિંમત;
    સ્થિતિ UCHAR_VERTICAL_ORIENTATION:
        પાછા g_Max_Vo_કિંમત;
    મૂળભૂત:
        પાછા 0;
    }
}

સ્થિર int32_t get_IDStatus_કિંમત(અચલ IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    પાછા value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_STATUS_ALLOWED : U_ID_STATUS_RESTRICTED;
}

સ્થિર અચલ IntProperty intProps[UCHAR_INT_LIMIT-UCHAR_INT_START]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, bi_Di_Get_Max_કિંમત },
    { UPROPS_SRC_BLOCK, 0, 0,                               getBlock, block_Get_Max_કિંમત },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, get_Max_કિંમત_From_Shift },
    { 2,                UPROPS_DT_MASK, 0,                  default_Get_કિંમત, default_Get_Max_કિંમત },
    { 0,                UPROPS_EA_MASK, UPROPS_EA_SHIFT,    default_Get_કિંમત, default_Get_Max_કિંમત },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_ગણતરી) - 1, getGeneralCategory, get_Max_કિંમત_From_Shift },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningGroup, bi_Di_Get_Max_કિંમત },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, bi_Di_Get_Max_કિંમત },
    { 2,                UPROPS_LB_MASK, UPROPS_LB_SHIFT,    default_Get_કિંમત, default_Get_Max_કિંમત },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_ગણતરી) - 1, getNumericType, get_Max_કિંમત_From_Shift },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, script_Get_Max_કિંમત },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_ગણતરી) - 1, getHangulSyllableType, get_Max_કિંમત_From_Shift },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, get_Max_કિંમત_From_Shift },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, get_Max_કિંમત_From_Shift },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, get_Max_કિંમત_From_Shift },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, get_Max_કિંમત_From_Shift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, get_Max_કિંમત_From_Shift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, get_Max_કિંમત_From_Shift },
    { 2,                UPROPS_GCB_MASK, UPROPS_GCB_SHIFT,  default_Get_કિંમત, default_Get_Max_કિંમત },
    { 2,                UPROPS_SB_MASK, UPROPS_SB_SHIFT,    default_Get_કિંમત, default_Get_Max_કિંમત },
    { 2,                UPROPS_WB_MASK, UPROPS_WB_SHIFT,    default_Get_કિંમત, default_Get_Max_કિંમત },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, bi_Di_Get_Max_કિંમત },
    { UPROPS_SRC_INPC,  0, 0,                               getInPC, layout_Get_Max_કિંમત },
    { UPROPS_SRC_INSC,  0, 0,                               getInSC, layout_Get_Max_કિંમત },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layout_Get_Max_કિંમત },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_STATUS_ALLOWED), get_IDStatus_કિંમત, get_Max_કિંમત_From_Shift },
    { 0,                UPROPS_INCB_MASK, UPROPS_INCB_SHIFT,default_Get_કિંમત, default_Get_Max_કિંમત },
};

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_કિંમત(UChar32 c, UProperty which) {
    જો(which<UCHAR_INT_START) {
        જો(UCHAR_દ્વિઅંકી_START<=which && which<UCHAR_દ્વિઅંકી_LIMIT) {
            અચલ દ્વિઅંકી_Property &prop=binProps[which];
            પાછા prop.contains(prop, c, which);
        }
    } નહીંતર જો(which<UCHAR_INT_LIMIT) {
        અચલ IntProperty &prop=intProps[which-UCHAR_INT_START];
        પાછા prop.get_કિંમત(prop, c, which);
    } નહીંતર જો(which==UCHAR_GENERAL_CATEGORY_MASK) {
        પાછા U_MASK(u_charType(c));
    }
    પાછા 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Min_કિંમત(UProperty /*which*/) {
    પાછા 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Max_કિંમત(UProperty which) {
    જો(which<UCHAR_INT_START) {
        જો(UCHAR_દ્વિઅંકી_START<=which && which<UCHAR_દ્વિઅંકી_LIMIT) {
            પાછા 1;  // maximum true for all binary properties
        }
    } નહીંતર જો(which<UCHAR_INT_LIMIT) {
        અચલ IntProperty &prop=intProps[which-UCHAR_INT_START];
        પાછા prop.get_Max_કિંમત(prop, which);
    }
    પાછા -1;  // undefined
}

U_CFUNC UPropertySource U_EXPORT2
uprops_getSource(UProperty which) {
    જો(which<UCHAR_દ્વિઅંકી_START) {
        પાછા UPROPS_SRC_NONE; /* undefined */
    } નહીંતર જો(which<UCHAR_દ્વિઅંકી_LIMIT) {
        અચલ દ્વિઅંકી_Property &prop=binProps[which];
        જો(prop.mask!=0) {
            પાછા UPROPS_SRC_PROPSVEC;
        } નહીંતર {
            પાછા (UPropertySource)prop.column;
        }
    } નહીંતર જો(which<UCHAR_INT_START) {
        પાછા UPROPS_SRC_NONE; /* undefined */
    } નહીંતર જો(which<UCHAR_INT_LIMIT) {
        અચલ IntProperty &prop=intProps[which-UCHAR_INT_START];
        જો(prop.mask!=0) {
            પાછા UPROPS_SRC_PROPSVEC;
        } નહીંતર {
            પાછા (UPropertySource)prop.column;
        }
    } નહીંતર જો(which<UCHAR_STRING_START) {
        પસંદગી(which) {
        સ્થિતિ UCHAR_GENERAL_CATEGORY_MASK:
        સ્થિતિ UCHAR_NUMERIC_કિંમત:
            પાછા UPROPS_SRC_CHAR;

        મૂળભૂત:
            પાછા UPROPS_SRC_NONE;
        }
    } નહીંતર જો(which<UCHAR_STRING_LIMIT) {
        પસંદગી(which) {
        સ્થિતિ UCHAR_AGE:
            પાછા UPROPS_SRC_PROPSVEC;

        સ્થિતિ UCHAR_BIDI_MIRRORING_GLYPH:
            પાછા UPROPS_SRC_BIDI;

        સ્થિતિ UCHAR_CASE_FOLDING:
        સ્થિતિ UCHAR_LOWERCASE_MAPPING:
        સ્થિતિ UCHAR_SIMPLE_CASE_FOLDING:
        સ્થિતિ UCHAR_SIMPLE_LOWERCASE_MAPPING:
        સ્થિતિ UCHAR_SIMPLE_TITLECASE_MAPPING:
        સ્થિતિ UCHAR_SIMPLE_UPPERCASE_MAPPING:
        સ્થિતિ UCHAR_TITLECASE_MAPPING:
        સ્થિતિ UCHAR_UPPERCASE_MAPPING:
            પાછા UPROPS_SRC_CASE;

        સ્થિતિ UCHAR_ISO_COMMENT:
        સ્થિતિ UCHAR_NAME:
        સ્થિતિ UCHAR_UNICODE_1_NAME:
            પાછા UPROPS_SRC_NAMES;

        મૂળભૂત:
            પાછા UPROPS_SRC_NONE;
        }
    } નહીંતર {
        પસંદગી(which) {
        સ્થિતિ UCHAR_SCRIPT_EXTENSIONS:
        સ્થિતિ UCHAR_IDENTIFIER_TYPE:
            પાછા UPROPS_SRC_PROPSVEC;
        મૂળભૂત:
            પાછા UPROPS_SRC_NONE; /* undefined */
        }
    }
}

U_CFUNC શૂન્ય U_EXPORT2
uprops_addPropertyStarts(UPropertySource src, અચલ USetAdder *sa, UErrorCode *pErrorCode) {
    જો (U_FAILURE(*pErrorCode)) { પાછા; }
    જો (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        માટે (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->add(sa->set, c);
        }
        // single characters
        માટે (UChar32 c : ID_COMPAT_MATH_START) {
            sa->add(sa->set, c);
            sa->add(sa->set, c + 1);
        }
        પાછા;
    }
    જો (src == UPROPS_SRC_MCM) {
        // range limits
        માટે (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->add(sa->set, c);
        }
        પાછા;
    }
    જો (!ulayout_ensureData(*pErrorCode)) { પાછા; }
    અચલ UCPTrie *trie;
    પસંદગી (src) {
    સ્થિતિ UPROPS_SRC_INPC:
        trie = gInpcTrie;
        તોડો;
    સ્થિતિ UPROPS_SRC_INSC:
        trie = gInscTrie;
        તોડો;
    સ્થિતિ UPROPS_SRC_VO:
        trie = gVoTrie;
        તોડો;
    મૂળભૂત:
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

    જો (trie == nullptr) {
        *pErrorCode = U_MISSING_RESOURCE_ERROR;
        પાછા;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    જ્યારે ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, nullptr)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

U_CAPI તાર્કિક U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t typeIndex = type;  // also guards against negative type integers
    જો (typeIndex >= UPRV_LENGTHOF(uprops_idTypeToEncoded)) {
        પાછા ખોટું;
    }
    uint32_t encodedType = uprops_idTypeToEncoded[typeIndex];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    જો ((encodedType & UPROPS_ID_TYPE_BIT) != 0) {
        પાછા value < UPROPS_ID_TYPE_FORBIDDEN && (value & encodedType) != 0;
    } નહીંતર {
        પાછા value == encodedType;
    }
}

નામાકાશ {

શૂન્ય maybeAppendType(uint32_t value, uint32_t bit, UIdentifierType t,
                     UIdentifierType *types, int32_t &length, int32_t capacity) {
    જો ((value & bit) != 0) {
        જો (length < capacity) {
            types[length] = t;
        }
        ++length;
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UErrorCode *pErrorCode) {
    જો (U_FAILURE(*pErrorCode)) { પાછા 0; }
    જો (capacity < 0 || (capacity > 0 && types == nullptr)) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    જો ((value & UPROPS_ID_TYPE_FORBIDDEN) == UPROPS_ID_TYPE_FORBIDDEN ||
            value == UPROPS_ID_TYPE_NOT_CHARACTER) {
        // single value
        જો (capacity > 0) {
            UIdentifierType t;
            પસંદગી (value) {
                સ્થિતિ UPROPS_ID_TYPE_NOT_CHARACTER: t = U_ID_TYPE_NOT_CHARACTER; તોડો;
                સ્થિતિ UPROPS_ID_TYPE_DEPRECATED: t = U_ID_TYPE_DEPRECATED; તોડો;
                સ્થિતિ UPROPS_ID_TYPE_DEFAULT_IGNORABLE: t = U_ID_TYPE_DEFAULT_IGNORABLE; તોડો;
                સ્થિતિ UPROPS_ID_TYPE_NOT_NFKC: t = U_ID_TYPE_NOT_NFKC; તોડો;
                સ્થિતિ UPROPS_ID_TYPE_INCLUSION: t = U_ID_TYPE_INCLUSION; તોડો;
                સ્થિતિ UPROPS_ID_TYPE_RECOMMENDED: t = U_ID_TYPE_RECOMMENDED; તોડો;
                મૂળભૂત:
                    *pErrorCode = U_INVALID_FORMAT_ERROR;
                    પાછા 0;
            }
            types[0] = t;
        } નહીંતર {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        પાછા 1;
    } નહીંતર {
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
        જો (length >= capacity) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        પાછા length;
    }
}

#જો !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_getFC_NFKC_Closure(UChar32 c, char16_t *dest, int32_t destCapacity, UErrorCode *pErrorCode) {
    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    જો(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    અચલ Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*pErrorCode);
    જો(U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    અચલ char16_t *folded1;
    int32_t folded1Length=ucase_toFullFolding(c, &folded1, U_FOLD_CASE_DEFAULT);
    જો(folded1Length<0) {
        અચલ Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        જો(nfkcImpl->getCompQuickCheck(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            પાછા u_terminateUChars(dest, destCapacity, 0, pErrorCode);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.setTo(c);
    } નહીંતર {
        જો(folded1Length>UCASE_MAX_STRING_LENGTH) {
            folded1String.setTo(folded1Length);
        } નહીંતર {
            folded1String.setTo(ખોટું, folded1, folded1Length);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *pErrorCode);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *pErrorCode);
    // if (c != b) add the mapping from a to c
    જો(U_FAILURE(*pErrorCode) || kc1==kc2) {
        પાછા u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    } નહીંતર {
        પાછા kc2.extract(dest, destCapacity, *pErrorCode);
    }
}

#endif
