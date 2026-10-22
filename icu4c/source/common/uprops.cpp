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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uchar.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucptrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unorm2.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uscript.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "emojiprops.h"
#ଅନ୍ତର୍ଭାଵ "mutex.h"
#ଅନ୍ତର୍ଭାଵ "normalizer2impl.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "ubidi_props.h"
#ଅନ୍ତର୍ଭାଵ "uprops.h"
#ଅନ୍ତର୍ଭାଵ "ucase.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "ulayout_props.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"

U_NAMESPACE_USE

// Unicode text layout properties data -----------------------------------------

ନାମାକାଶ {

icu::UInitOnce gLayoutInitOnce {};
UDataMemory *gLayoutMemory = nullptr;

UCPTrie *gInpcTrie = nullptr;  // Indic_Positional_Category
UCPTrie *gInscTrie = nullptr;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = nullptr;  // Vertical_Orientation

int32_t g_Max_Inpc_ମୂଲ୍ୟ = 0;
int32_t g_Max_Insc_ମୂଲ୍ୟ = 0;
int32_t g_Max_Vo_ମୂଲ୍ୟ = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(gLayoutMemory);
    gLayoutMemory = nullptr;

    ucptrie_close(gInpcTrie);
    gInpcTrie = nullptr;
    ucptrie_close(gInscTrie);
    gInscTrie = nullptr;
    ucptrie_close(gVoTrie);
    gVoTrie = nullptr;

    g_Max_Inpc_ମୂଲ୍ୟ = 0;
    g_Max_Insc_ମୂଲ୍ୟ = 0;
    g_Max_Vo_ମୂଲ୍ୟ = 0;

    gLayoutInitOnce.reset();
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

UBool U_CALLCONV
ulayout_isAcceptable(ଶୂନ୍ୟ * /*context*/,
                     ଅଚଳ ଅକ୍ଷର * /* type */, ଅଚଳ ଅକ୍ଷର * /*name*/,
                     ଅଚଳ UDataInfo *pInfo) {
    ଫେରନ୍ତୁ pInfo->size >= 20 &&
        pInfo->isBigEndian == U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily == U_CHARSET_FAMILY &&
        pInfo->dataFormat[0] == ULAYOUT_FMT_0 &&
        pInfo->dataFormat[1] == ULAYOUT_FMT_1 &&
        pInfo->dataFormat[2] == ULAYOUT_FMT_2 &&
        pInfo->dataFormat[3] == ULAYOUT_FMT_3 &&
        pInfo->formatVersion[0] == 1;
}

// UInitOnce singleton initialization function
ଶୂନ୍ୟ U_CALLCONV ulayout_load(UErrorCode &errorCode) {
    gLayoutMemory = udata_openChoice(
        nullptr, ULAYOUT_DATA_TYPE, ULAYOUT_DATA_NAME,
        ulayout_isAcceptable, nullptr, &errorCode);
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }

    ଅଚଳ uint8_t* inBytes = static_cast<ଅଚଳ uint8_t*>(udata_getMemory(gLayoutMemory));
    ଅଚଳ int32_t* inIndexes = reinterpret_cast<ଅଚଳ int32_t*>(inBytes);
    int32_t indexesLength = inIndexes[ULAYOUT_IX_INDEXES_LENGTH];
    ଯଦି (indexesLength < 12) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ଫେରନ୍ତୁ;
    }
    int32_t offset = indexesLength * 4;
    int32_t ଶୀର୍ଷ = inIndexes[ULAYOUT_IX_INPC_TRIE_ଶୀର୍ଷ];
    int32_t trieSize = ଶୀର୍ଷ - offset;
    ଯଦି (trieSize >= 16) {
        gInpcTrie = ucptrie_open_From_ଦ୍ୱିମିକ(
            UCPTRIE_TYPE_ANY, UCPTRIE_ମୂଲ୍ୟ_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }
    offset = ଶୀର୍ଷ;
    ଶୀର୍ଷ = inIndexes[ULAYOUT_IX_INSC_TRIE_ଶୀର୍ଷ];
    trieSize = ଶୀର୍ଷ - offset;
    ଯଦି (trieSize >= 16) {
        gInscTrie = ucptrie_open_From_ଦ୍ୱିମିକ(
            UCPTRIE_TYPE_ANY, UCPTRIE_ମୂଲ୍ୟ_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }
    offset = ଶୀର୍ଷ;
    ଶୀର୍ଷ = inIndexes[ULAYOUT_IX_VO_TRIE_ଶୀର୍ଷ];
    trieSize = ଶୀର୍ଷ - offset;
    ଯଦି (trieSize >= 16) {
        gVoTrie = ucptrie_open_From_ଦ୍ୱିମିକ(
            UCPTRIE_TYPE_ANY, UCPTRIE_ମୂଲ୍ୟ_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }

    uint32_t max_ମୂଲ୍ୟଗୁଡ଼ିକ = inIndexes[ULAYOUT_IX_MAX_ମୂଲ୍ୟଗୁଡ଼ିକ];
    g_Max_Inpc_ମୂଲ୍ୟ = max_ମୂଲ୍ୟଗୁଡ଼ିକ >> ULAYOUT_MAX_INPC_SHIFT;
    g_Max_Insc_ମୂଲ୍ୟ = (max_ମୂଲ୍ୟଗୁଡ଼ିକ >> ULAYOUT_MAX_INSC_SHIFT) & 0xff;
    g_Max_Vo_ମୂଲ୍ୟ = (max_ମୂଲ୍ୟଗୁଡ଼ିକ >> ULAYOUT_MAX_VO_SHIFT) & 0xff;

    ucln_common_registerCleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensureData(UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_load, errorCode);
    ଫେରନ୍ତୁ U_SUCCESS(errorCode);
}

UBool ulayout_ensureData() {
    UErrorCode errorCode = U_ZERO_ERROR;
    ଫେରନ୍ତୁ ulayout_ensureData(errorCode);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

ସଂରଚନା ଦ୍ୱିମିକ_Property;

ପ୍ରକାରବ୍ୟାଖ୍ୟା UBool ଦ୍ୱିମିକ_Property_Contains(ଅଚଳ ଦ୍ୱିମିକ_Property &prop, UChar32 c, UProperty which);

ସଂରଚନା ଦ୍ୱିମିକ_Property {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    ଦ୍ୱିମିକ_Property_Contains *contains;
};

ସ୍ଥିର UBool defaultContains(ଅଚଳ ଦ୍ୱିମିକ_Property &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    ଫେରନ୍ତୁ (u_getUnicodeProperties(c, prop.column)&prop.mask)!=0;
}

ସ୍ଥିର UBool case_ଦ୍ୱିମିକ_Property_Contains(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty which) {
    ଫେରନ୍ତୁ ucase_has_ଦ୍ୱିମିକ_Property(c, which);
}

ସ୍ଥିର UBool isBidiControl(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ ubidi_isBidiControl(c);
}

ସ୍ଥିର UBool isMirrored(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ ubidi_isMirrored(c);
}

ସ୍ଥିର UBool isJoinControl(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ ubidi_isJoinControl(c);
}

#ଯଦି UCONFIG_NO_NORMALIZATION
ସ୍ଥିର UBool hasFullCompositionExclusion(ଅଚଳ ଦ୍ୱିମିକ_Property &, UChar32, UProperty) {
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}
#ଅନ୍ୟଥା
ସ୍ଥିର UBool hasFullCompositionExclusion(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UErrorCode errorCode=U_ZERO_ERROR;
    ଅଚଳ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    ଫେରନ୍ତୁ U_SUCCESS(errorCode) && impl->isCompNo(impl->getNorm16(c));
}
#endif

// UCHAR_NF*_INERT properties
#ଯଦି UCONFIG_NO_NORMALIZATION
ସ୍ଥିର UBool isNormInert(ଅଚଳ ଦ୍ୱିମିକ_Property &, UChar32, UProperty) {
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}
#ଅନ୍ୟଥା
ସ୍ଥିର UBool isNormInert(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty which) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ଅଚଳ Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalizationMode>(which - UCHAR_NFD_INERT + UNORM_NFD), errorCode);
    ଫେରନ୍ତୁ U_SUCCESS(errorCode) && norm2->isInert(c);
}
#endif

#ଯଦି UCONFIG_NO_NORMALIZATION
ସ୍ଥିର UBool changesWhenCasefolded(ଅଚଳ ଦ୍ୱିମିକ_Property &, UChar32, UProperty) {
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}
#ଅନ୍ୟଥା
ସ୍ଥିର UBool changesWhenCasefolded(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UErrorCode errorCode=U_ZERO_ERROR;
    ଅଚଳ Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(errorCode);
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        ଯଦି(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } ଅନ୍ୟଥା ଯଦି(nfd.length()<=U16_MAX_LENGTH &&
                  nfd.length()==U16_LENGTH(c=nfd.char32At(0))
        ) {
            /* single supplementary code point */
        } ଅନ୍ୟଥା {
            c=U_SENTINEL;
        }
    } ଅନ୍ୟଥା ଯଦି(c<0) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;  /* protect against bad input */
    }
    ଯଦି(c>=0) {
        /* single code point */
        ଅଚଳ char16_t *resultString;
        ଫେରନ୍ତୁ ucase_toFullFolding(c, &resultString, U_FOLD_CASE_DEFAULT) >= 0;
    } ଅନ୍ୟଥା {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_LENGTH];
        int32_t destLength;
        destLength=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.getBuffer(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &errorCode);
        ଫେରନ୍ତୁ U_SUCCESS(errorCode) &&
                       0!=u_strCompare(nfd.getBuffer(), nfd.length(),
                                       dest, destLength, ମିଥ୍ୟା);
    }
}
#endif

#ଯଦି UCONFIG_NO_NORMALIZATION
ସ୍ଥିର UBool changesWhenNFKC_Casefolded(ଅଚଳ ଦ୍ୱିମିକ_Property &, UChar32, UProperty) {
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}
#ଅନ୍ୟଥା
ସ୍ଥିର UBool changesWhenNFKC_Casefolded(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ଅଚଳ Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(errorCode);
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        ReorderingBuffer buffer(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        ଯଦି(buffer.init(5, errorCode)) {
            ଅଚଳ char16_t *srcArray=src.getBuffer();
            kcf->compose(srcArray, srcArray+src.length(), ମିଥ୍ୟା,
                          ସତ୍ୟ, buffer, errorCode);
        }
    }
    ଫେରନ୍ତୁ U_SUCCESS(errorCode) && dest!=src;
}
#endif

#ଯଦି UCONFIG_NO_NORMALIZATION
ସ୍ଥିର UBool isCanonSegmentStarter(ଅଚଳ ଦ୍ୱିମିକ_Property &, UChar32, UProperty) {
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}
#ଅନ୍ୟଥା
ସ୍ଥିର UBool isCanonSegmentStarter(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ଅଚଳ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    ଫେରନ୍ତୁ
        U_SUCCESS(errorCode) && impl->ensureCanonIterData(errorCode) &&
        impl->isCanonSegmentStarter(c);
}
#endif

ସ୍ଥିର UBool isPOSIX_alnum(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ u_isalnumPOSIX(c);
}

ସ୍ଥିର UBool isPOSIX_blank(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ u_isblank(c);
}

ସ୍ଥିର UBool isPOSIX_graph(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ u_isgraphPOSIX(c);
}

ସ୍ଥିର UBool isPOSIX_print(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ u_isprintPOSIX(c);
}

ସ୍ଥିର UBool isPOSIX_xdigit(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ u_isxdigit(c);
}

ସ୍ଥିର UBool isRegionalIndicator(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    ଫେରନ୍ତୁ 0x1F1E6<=c && c<=0x1F1FF;
}

ସ୍ଥିର UBool hasEmojiProperty(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty which) {
    ଫେରନ୍ତୁ EmojiProps::has_ଦ୍ୱିମିକ_Property(c, which);
}

ସ୍ଥିର UBool isIDSUnaryOperator(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // New in Unicode 15.1 for just two characters.
    ଫେରନ୍ତୁ 0x2FFE<=c && c<=0x2FFF;
}

/** Ranges (start/limit pairs) of ID_Compat_Math_Continue (only), from UCD PropList.txt. */
ସ୍ଥିର constexpr UChar32 ID_COMPAT_MATH_CONTINUE[] = {
    0x00B2, 0x00B3 + 1,
    0x00B9, 0x00B9 + 1,
    0x2070, 0x2070 + 1,
    0x2074, 0x207E + 1,
    0x2080, 0x208E + 1
};

/** ID_Compat_Math_Start characters, from UCD PropList.txt. */
ସ୍ଥିର constexpr UChar32 ID_COMPAT_MATH_START[] = {
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
ସ୍ଥିର constexpr UChar32 MODIFIER_COMBINING_MARK[] = {
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

ସ୍ଥିର UBool isIDCompatMathStart(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଯଦି (c < ID_COMPAT_MATH_START[0]) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }  // fastpath for common scripts
    ପାଇଁ (UChar32 startChar : ID_COMPAT_MATH_START) {
        ଯଦି (c == startChar) { ଫେରନ୍ତୁ ସତ୍ୟ; }
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ସ୍ଥିର UBool isIDCompatMathContinue(ଅଚଳ ଦ୍ୱିମିକ_Property &prop, UChar32 c, UProperty /*which*/) {
    ପାଇଁ (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        ଯଦି (c < ID_COMPAT_MATH_CONTINUE[i]) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }  // below range start
        ଯଦି (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { ଫେରନ୍ତୁ ସତ୍ୟ; }  // below range limit
    }
    ଫେରନ୍ତୁ isIDCompatMathStart(prop, c, UCHAR_ID_COMPAT_MATH_START);
}

ସ୍ଥିର UBool isModifierCombiningMark(ଅଚଳ ଦ୍ୱିମିକ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ପାଇଁ (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        ଯଦି (c < MODIFIER_COMBINING_MARK[i]) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }  // below range start
        ଯଦି (c < MODIFIER_COMBINING_MARK[i + 1]) { ଫେରନ୍ତୁ ସତ୍ୟ; }  // below range limit
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ସ୍ଥିର ଅଚଳ ଦ୍ୱିମିକ_Property binProps[UCHAR_ଦ୍ୱିମିକ_LIMIT]={
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
    { 1,                U_MASK(UPROPS_IDS_ଦ୍ୱିମିକ_OPERATOR), defaultContains },
    { 1,                U_MASK(UPROPS_IDS_TRINARY_OPERATOR), defaultContains },
    { UPROPS_SRC_BIDI,  0, isJoinControl },
    { 1,                U_MASK(UPROPS_LOGICAL_ORDER_EXCEPTION), defaultContains },
    { UPROPS_SRC_CASE,  0, case_ଦ୍ୱିମିକ_Property_Contains },  // UCHAR_LOWERCASE
    { 1,                U_MASK(UPROPS_MATH), defaultContains },
    { 1,                U_MASK(UPROPS_NONCHARACTER_CODE_POINT), defaultContains },
    { 1,                U_MASK(UPROPS_QUOTATION_MARK), defaultContains },
    { 1,                U_MASK(UPROPS_RADICAL), defaultContains },
    { UPROPS_SRC_CASE,  0, case_ଦ୍ୱିମିକ_Property_Contains },  // UCHAR_SOFT_DOTTED
    { 1,                U_MASK(UPROPS_TERMINAL_PUNCTUATION), defaultContains },
    { 1,                U_MASK(UPROPS_UNIFIED_IDEOGRAPH), defaultContains },
    { UPROPS_SRC_CASE,  0, case_ଦ୍ୱିମିକ_Property_Contains },  // UCHAR_UPPERCASE
    { 1,                U_MASK(UPROPS_WHITE_SPACE), defaultContains },
    { 1,                U_MASK(UPROPS_XID_CONTINUE), defaultContains },
    { 1,                U_MASK(UPROPS_XID_START), defaultContains },
    { UPROPS_SRC_CASE,  0, case_ଦ୍ୱିମିକ_Property_Contains },  // UCHAR_CASE_SENSITIVE
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
    { UPROPS_SRC_CASE,  0, case_ଦ୍ୱିମିକ_Property_Contains },  // UCHAR_CASED
    { UPROPS_SRC_CASE,  0, case_ଦ୍ୱିମିକ_Property_Contains },  // UCHAR_CASE_IGNORABLE
    { UPROPS_SRC_CASE,  0, case_ଦ୍ୱିମିକ_Property_Contains },  // UCHAR_CHANGES_WHEN_LOWERCASED
    { UPROPS_SRC_CASE,  0, case_ଦ୍ୱିମିକ_Property_Contains },  // UCHAR_CHANGES_WHEN_UPPERCASED
    { UPROPS_SRC_CASE,  0, case_ଦ୍ୱିମିକ_Property_Contains },  // UCHAR_CHANGES_WHEN_TITLECASED
    { UPROPS_SRC_CASE_AND_NORM,  0, changesWhenCasefolded },
    { UPROPS_SRC_CASE,  0, case_ଦ୍ୱିମିକ_Property_Contains },  // UCHAR_CHANGES_WHEN_CASEMAPPED
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
u_has_ଦ୍ୱିମିକ_Property(UChar32 c, UProperty which) {
    /* c is range-checked in the functions that are called from here */
    ଯଦି(which<UCHAR_ଦ୍ୱିମିକ_START || UCHAR_ଦ୍ୱିମିକ_LIMIT<=which) {
        /* not a known binary property */
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    } ଅନ୍ୟଥା {
        ଅଚଳ ଦ୍ୱିମିକ_Property &prop=binProps[which];
        ଫେରନ୍ତୁ prop.contains(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    ଫେରନ୍ତୁ u_has_ଦ୍ୱିମିକ_Property(c, UCHAR_ID_START);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    ଫେରନ୍ତୁ u_has_ଦ୍ୱିମିକ_Property(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_string_Has_ଦ୍ୱିମିକ_Property(ଅଚଳ char16_t *s, int32_t length, UProperty which) {
    ଯଦି (s == nullptr && length != 0) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    ଯଦି (length == 1) {
        ଫେରନ୍ତୁ u_has_ଦ୍ୱିମିକ_Property(s[0], which);  // single code point
    } ଅନ୍ୟଥା ଯଦି (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        ଯଦି (length > 0 ? i == length : s[i] == 0) {
            ଫେରନ୍ତୁ u_has_ଦ୍ୱିମିକ_Property(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    ଫେରନ୍ତୁ UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::has_ଦ୍ୱିମିକ_Property(s, length, which);
}

ସଂରଚନା IntProperty;

ପ୍ରକାରବ୍ୟାଖ୍ୟା int32_t Int_Property_Get_ମୂଲ୍ୟ(ଅଚଳ IntProperty &prop, UChar32 c, UProperty which);
ପ୍ରକାରବ୍ୟାଖ୍ୟା int32_t Int_Property_Get_Max_ମୂଲ୍ୟ(ଅଚଳ IntProperty &prop, UProperty which);

ସଂରଚନା IntProperty {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t shift;  // =maxValue if getMaxValueFromShift() is used
    Int_Property_Get_ମୂଲ୍ୟ *get_ମୂଲ୍ୟ;
    Int_Property_Get_Max_ମୂଲ୍ୟ *get_Max_ମୂଲ୍ୟ;
};

ସ୍ଥିର int32_t default_Get_ମୂଲ୍ୟ(ଅଚଳ IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    ଫେରନ୍ତୁ static_cast<int32_t>(u_getUnicodeProperties(c, prop.column) & prop.mask) >> prop.shift;
}

ସ୍ଥିର int32_t default_Get_Max_ମୂଲ୍ୟ(ଅଚଳ IntProperty &prop, UProperty /*which*/) {
    ଫେରନ୍ତୁ (uprv_get_Max_ମୂଲ୍ୟଗୁଡ଼ିକ(prop.column)&prop.mask)>>prop.shift;
}

ସ୍ଥିର int32_t get_Max_ମୂଲ୍ୟ_From_Shift(ଅଚଳ IntProperty &prop, UProperty /*which*/) {
    ଫେରନ୍ତୁ prop.shift;
}

ସ୍ଥିର int32_t getBiDiClass(ଅଚଳ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ static_cast<int32_t>(u_charDirection(c));
}

ସ୍ଥିର int32_t getBiDiPairedBracketType(ଅଚଳ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

ସ୍ଥିର int32_t bi_Di_Get_Max_ମୂଲ୍ୟ(ଅଚଳ IntProperty &/*prop*/, UProperty which) {
    ଫେରନ୍ତୁ ubidi_get_Max_ମୂଲ୍ୟ(which);
}

ସ୍ଥିର int32_t getBlock(ଅଚଳ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ static_cast<int32_t>(ublock_getCode(c));
}

ସ୍ଥିର int32_t block_Get_Max_ମୂଲ୍ୟ(ଅଚଳ IntProperty &/*prop*/, UProperty /*which*/) {
    ଫେରନ୍ତୁ uprv_get_Max_ମୂଲ୍ୟଗୁଡ଼ିକ(UPROPS_MAX_ମୂଲ୍ୟଗୁଡ଼ିକ_OTHER_INDEX) & UPROPS_MAX_BLOCK;
}

#ଯଦି UCONFIG_NO_NORMALIZATION
ସ୍ଥିର int32_t getCombiningClass(ଅଚଳ IntProperty &, UChar32, UProperty) {
    ଫେରନ୍ତୁ 0;
}
#ଅନ୍ୟଥା
ସ୍ଥିର int32_t getCombiningClass(ଅଚଳ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ u_getCombiningClass(c);
}
#endif

ସ୍ଥିର int32_t getGeneralCategory(ଅଚଳ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ static_cast<int32_t>(u_charType(c));
}

ସ୍ଥିର int32_t getJoiningGroup(ଅଚଳ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ ubidi_getJoiningGroup(c);
}

ସ୍ଥିର int32_t getJoiningType(ଅଚଳ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ ubidi_getJoiningType(c);
}

ସ୍ଥିର int32_t getNumericType(ଅଚଳ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_ମୂଲ୍ୟ(u_getMainProperties(c)));
    ଫେରନ୍ତୁ UPROPS_NTV_GET_TYPE(ntv);
}

ସ୍ଥିର int32_t getScript(ଅଚଳ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ଫେରନ୍ତୁ static_cast<int32_t>(uscript_getScript(c, &errorCode));
}

ସ୍ଥିର int32_t script_Get_Max_ମୂଲ୍ୟ(ଅଚଳ IntProperty &/*prop*/, UProperty /*which*/) {
    ଫେରନ୍ତୁ uprv_get_Max_ମୂଲ୍ୟଗୁଡ଼ିକ(0)&UPROPS_MAX_SCRIPT;
}

/*
 * Map some of the Grapheme Cluster Break values to Hangul Syllable Types.
 * Hangul_Syllable_Type used to be fully redundant with a subset of Grapheme_Cluster_Break.
 *
 * Starting with Unicode 16, this is no longer true for HST=V vs. GCB=V in some cases:
 * Some Kirat Rai vowels are given GCB=V for proper grapheme clustering, but
 * they are of course not related to Hangul syllables.
 */
ସ୍ଥିର ଅଚଳ UHangulSyllableType gcbToHst[]={
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

ସ୍ଥିର int32_t getHangulSyllableType(ଅଚଳ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Ignore supplementary code points: They all have HST=NA.
    // This is a simple way to handle the GCB!=hst cases since Unicode 16 (Kirat Rai vowels).
    ଯଦି(c>0xffff) {
        ଫେରନ୍ତୁ U_HST_NOT_APPLICABLE;
    }
    /* see comments on gcbToHst[] above */
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_MASK) >> UPROPS_GCB_SHIFT;
    ଯଦି(gcb<UPRV_LENGTHOF(gcbToHst)) {
        ଫେରନ୍ତୁ gcbToHst[gcb];
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ U_HST_NOT_APPLICABLE;
    }
}

#ଯଦି UCONFIG_NO_NORMALIZATION
ସ୍ଥିର int32_t getNormQuickCheck(ଅଚଳ IntProperty &, UChar32, UProperty) {
    ଫେରନ୍ତୁ 0;
}
#ଅନ୍ୟଥା
ସ୍ଥିର int32_t getNormQuickCheck(ଅଚଳ IntProperty &/*prop*/, UChar32 c, UProperty which) {
    ଫେରନ୍ତୁ static_cast<int32_t>(unorm_getQuickCheck(c, static_cast<UNormalizationMode>(which - UCHAR_NFD_QUICK_CHECK + UNORM_NFD)));
}
#endif

#ଯଦି UCONFIG_NO_NORMALIZATION
ସ୍ଥିର int32_t getLeadCombiningClass(ଅଚଳ IntProperty &, UChar32, UProperty) {
    ଫେରନ୍ତୁ 0;
}
#ଅନ୍ୟଥା
ସ୍ଥିର int32_t getLeadCombiningClass(ଅଚଳ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ unorm_getFCD16(c)>>8;
}
#endif

#ଯଦି UCONFIG_NO_NORMALIZATION
ସ୍ଥିର int32_t getTrailCombiningClass(ଅଚଳ IntProperty &, UChar32, UProperty) {
    ଫେରନ୍ତୁ 0;
}
#ଅନ୍ୟଥା
ସ୍ଥିର int32_t getTrailCombiningClass(ଅଚଳ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ଫେରନ୍ତୁ unorm_getFCD16(c)&0xff;
}
#endif

ସ୍ଥିର int32_t getInPC(ଅଚଳ IntProperty &, UChar32 c, UProperty) {
    ଫେରନ୍ତୁ ulayout_ensureData() && gInpcTrie != nullptr ? ucptrie_get(gInpcTrie, c) : 0;
}

ସ୍ଥିର int32_t getInSC(ଅଚଳ IntProperty &, UChar32 c, UProperty) {
    ଫେରନ୍ତୁ ulayout_ensureData() && gInscTrie != nullptr ? ucptrie_get(gInscTrie, c) : 0;
}

ସ୍ଥିର int32_t getVo(ଅଚଳ IntProperty &, UChar32 c, UProperty) {
    ଫେରନ୍ତୁ ulayout_ensureData() && gVoTrie != nullptr ? ucptrie_get(gVoTrie, c) : 0;
}

ସ୍ଥିର int32_t layout_Get_Max_ମୂଲ୍ୟ(ଅଚଳ IntProperty &/*prop*/, UProperty which) {
    ଯଦି (!ulayout_ensureData()) { ଫେରନ୍ତୁ 0; }
    ବାଛ (which) {
    ସ୍ଥିତି UCHAR_INDIC_POSITIONAL_CATEGORY:
        ଫେରନ୍ତୁ g_Max_Inpc_ମୂଲ୍ୟ;
    ସ୍ଥିତି UCHAR_INDIC_SYLLABIC_CATEGORY:
        ଫେରନ୍ତୁ g_Max_Insc_ମୂଲ୍ୟ;
    ସ୍ଥିତି UCHAR_VERTICAL_ORIENTATION:
        ଫେରନ୍ତୁ g_Max_Vo_ମୂଲ୍ୟ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ 0;
    }
}

ସ୍ଥିର int32_t get_IDStatus_ମୂଲ୍ୟ(ଅଚଳ IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    ଫେରନ୍ତୁ value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_STATUS_ALLOWED : U_ID_STATUS_RESTRICTED;
}

ସ୍ଥିର ଅଚଳ IntProperty intProps[UCHAR_INT_LIMIT-UCHAR_INT_START]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, bi_Di_Get_Max_ମୂଲ୍ୟ },
    { UPROPS_SRC_BLOCK, 0, 0,                               getBlock, block_Get_Max_ମୂଲ୍ୟ },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, get_Max_ମୂଲ୍ୟ_From_Shift },
    { 2,                UPROPS_DT_MASK, 0,                  default_Get_ମୂଲ୍ୟ, default_Get_Max_ମୂଲ୍ୟ },
    { 0,                UPROPS_EA_MASK, UPROPS_EA_SHIFT,    default_Get_ମୂଲ୍ୟ, default_Get_Max_ମୂଲ୍ୟ },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_ଗଣନା) - 1, getGeneralCategory, get_Max_ମୂଲ୍ୟ_From_Shift },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningGroup, bi_Di_Get_Max_ମୂଲ୍ୟ },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, bi_Di_Get_Max_ମୂଲ୍ୟ },
    { 2,                UPROPS_LB_MASK, UPROPS_LB_SHIFT,    default_Get_ମୂଲ୍ୟ, default_Get_Max_ମୂଲ୍ୟ },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_ଗଣନା) - 1, getNumericType, get_Max_ମୂଲ୍ୟ_From_Shift },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, script_Get_Max_ମୂଲ୍ୟ },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_ଗଣନା) - 1, getHangulSyllableType, get_Max_ମୂଲ୍ୟ_From_Shift },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, get_Max_ମୂଲ୍ୟ_From_Shift },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, get_Max_ମୂଲ୍ୟ_From_Shift },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, get_Max_ମୂଲ୍ୟ_From_Shift },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, get_Max_ମୂଲ୍ୟ_From_Shift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, get_Max_ମୂଲ୍ୟ_From_Shift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, get_Max_ମୂଲ୍ୟ_From_Shift },
    { 2,                UPROPS_GCB_MASK, UPROPS_GCB_SHIFT,  default_Get_ମୂଲ୍ୟ, default_Get_Max_ମୂଲ୍ୟ },
    { 2,                UPROPS_SB_MASK, UPROPS_SB_SHIFT,    default_Get_ମୂଲ୍ୟ, default_Get_Max_ମୂଲ୍ୟ },
    { 2,                UPROPS_WB_MASK, UPROPS_WB_SHIFT,    default_Get_ମୂଲ୍ୟ, default_Get_Max_ମୂଲ୍ୟ },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, bi_Di_Get_Max_ମୂଲ୍ୟ },
    { UPROPS_SRC_INPC,  0, 0,                               getInPC, layout_Get_Max_ମୂଲ୍ୟ },
    { UPROPS_SRC_INSC,  0, 0,                               getInSC, layout_Get_Max_ମୂଲ୍ୟ },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layout_Get_Max_ମୂଲ୍ୟ },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_STATUS_ALLOWED), get_IDStatus_ମୂଲ୍ୟ, get_Max_ମୂଲ୍ୟ_From_Shift },
    { 0,                UPROPS_INCB_MASK, UPROPS_INCB_SHIFT,default_Get_ମୂଲ୍ୟ, default_Get_Max_ମୂଲ୍ୟ },
};

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_ମୂଲ୍ୟ(UChar32 c, UProperty which) {
    ଯଦି(which<UCHAR_INT_START) {
        ଯଦି(UCHAR_ଦ୍ୱିମିକ_START<=which && which<UCHAR_ଦ୍ୱିମିକ_LIMIT) {
            ଅଚଳ ଦ୍ୱିମିକ_Property &prop=binProps[which];
            ଫେରନ୍ତୁ prop.contains(prop, c, which);
        }
    } ଅନ୍ୟଥା ଯଦି(which<UCHAR_INT_LIMIT) {
        ଅଚଳ IntProperty &prop=intProps[which-UCHAR_INT_START];
        ଫେରନ୍ତୁ prop.get_ମୂଲ୍ୟ(prop, c, which);
    } ଅନ୍ୟଥା ଯଦି(which==UCHAR_GENERAL_CATEGORY_MASK) {
        ଫେରନ୍ତୁ U_MASK(u_charType(c));
    }
    ଫେରନ୍ତୁ 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Min_ମୂଲ୍ୟ(UProperty /*which*/) {
    ଫେରନ୍ତୁ 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Max_ମୂଲ୍ୟ(UProperty which) {
    ଯଦି(which<UCHAR_INT_START) {
        ଯଦି(UCHAR_ଦ୍ୱିମିକ_START<=which && which<UCHAR_ଦ୍ୱିମିକ_LIMIT) {
            ଫେରନ୍ତୁ 1;  // maximum true for all binary properties
        }
    } ଅନ୍ୟଥା ଯଦି(which<UCHAR_INT_LIMIT) {
        ଅଚଳ IntProperty &prop=intProps[which-UCHAR_INT_START];
        ଫେରନ୍ତୁ prop.get_Max_ମୂଲ୍ୟ(prop, which);
    }
    ଫେରନ୍ତୁ -1;  // undefined
}

U_CFUNC UPropertySource U_EXPORT2
uprops_getSource(UProperty which) {
    ଯଦି(which<UCHAR_ଦ୍ୱିମିକ_START) {
        ଫେରନ୍ତୁ UPROPS_SRC_NONE; /* undefined */
    } ଅନ୍ୟଥା ଯଦି(which<UCHAR_ଦ୍ୱିମିକ_LIMIT) {
        ଅଚଳ ଦ୍ୱିମିକ_Property &prop=binProps[which];
        ଯଦି(prop.mask!=0) {
            ଫେରନ୍ତୁ UPROPS_SRC_PROPSVEC;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ (UPropertySource)prop.column;
        }
    } ଅନ୍ୟଥା ଯଦି(which<UCHAR_INT_START) {
        ଫେରନ୍ତୁ UPROPS_SRC_NONE; /* undefined */
    } ଅନ୍ୟଥା ଯଦି(which<UCHAR_INT_LIMIT) {
        ଅଚଳ IntProperty &prop=intProps[which-UCHAR_INT_START];
        ଯଦି(prop.mask!=0) {
            ଫେରନ୍ତୁ UPROPS_SRC_PROPSVEC;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ (UPropertySource)prop.column;
        }
    } ଅନ୍ୟଥା ଯଦି(which<UCHAR_STRING_START) {
        ବାଛ(which) {
        ସ୍ଥିତି UCHAR_GENERAL_CATEGORY_MASK:
        ସ୍ଥିତି UCHAR_NUMERIC_ମୂଲ୍ୟ:
            ଫେରନ୍ତୁ UPROPS_SRC_CHAR;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଫେରନ୍ତୁ UPROPS_SRC_NONE;
        }
    } ଅନ୍ୟଥା ଯଦି(which<UCHAR_STRING_LIMIT) {
        ବାଛ(which) {
        ସ୍ଥିତି UCHAR_AGE:
            ଫେରନ୍ତୁ UPROPS_SRC_PROPSVEC;

        ସ୍ଥିତି UCHAR_BIDI_MIRRORING_GLYPH:
            ଫେରନ୍ତୁ UPROPS_SRC_BIDI;

        ସ୍ଥିତି UCHAR_CASE_FOLDING:
        ସ୍ଥିତି UCHAR_LOWERCASE_MAPPING:
        ସ୍ଥିତି UCHAR_SIMPLE_CASE_FOLDING:
        ସ୍ଥିତି UCHAR_SIMPLE_LOWERCASE_MAPPING:
        ସ୍ଥିତି UCHAR_SIMPLE_TITLECASE_MAPPING:
        ସ୍ଥିତି UCHAR_SIMPLE_UPPERCASE_MAPPING:
        ସ୍ଥିତି UCHAR_TITLECASE_MAPPING:
        ସ୍ଥିତି UCHAR_UPPERCASE_MAPPING:
            ଫେରନ୍ତୁ UPROPS_SRC_CASE;

        ସ୍ଥିତି UCHAR_ISO_COMMENT:
        ସ୍ଥିତି UCHAR_NAME:
        ସ୍ଥିତି UCHAR_UNICODE_1_NAME:
            ଫେରନ୍ତୁ UPROPS_SRC_NAMES;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଫେରନ୍ତୁ UPROPS_SRC_NONE;
        }
    } ଅନ୍ୟଥା {
        ବାଛ(which) {
        ସ୍ଥିତି UCHAR_SCRIPT_EXTENSIONS:
        ସ୍ଥିତି UCHAR_IDENTIFIER_TYPE:
            ଫେରନ୍ତୁ UPROPS_SRC_PROPSVEC;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଫେରନ୍ତୁ UPROPS_SRC_NONE; /* undefined */
        }
    }
}

U_CFUNC ଶୂନ୍ୟ U_EXPORT2
uprops_addPropertyStarts(UPropertySource src, ଅଚଳ USetAdder *sa, UErrorCode *pErrorCode) {
    ଯଦି (U_FAILURE(*pErrorCode)) { ଫେରନ୍ତୁ; }
    ଯଦି (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        ପାଇଁ (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->add(sa->set, c);
        }
        // single characters
        ପାଇଁ (UChar32 c : ID_COMPAT_MATH_START) {
            sa->add(sa->set, c);
            sa->add(sa->set, c + 1);
        }
        ଫେରନ୍ତୁ;
    }
    ଯଦି (src == UPROPS_SRC_MCM) {
        // range limits
        ପାଇଁ (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->add(sa->set, c);
        }
        ଫେରନ୍ତୁ;
    }
    ଯଦି (!ulayout_ensureData(*pErrorCode)) { ଫେରନ୍ତୁ; }
    ଅଚଳ UCPTrie *trie;
    ବାଛ (src) {
    ସ୍ଥିତି UPROPS_SRC_INPC:
        trie = gInpcTrie;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UPROPS_SRC_INSC:
        trie = gInscTrie;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UPROPS_SRC_VO:
        trie = gVoTrie;
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    ଯଦି (trie == nullptr) {
        *pErrorCode = U_MISSING_RESOURCE_ERROR;
        ଫେରନ୍ତୁ;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    ଯେତେବେଳେ ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, nullptr)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

U_CAPI ତାର୍କିକ U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t typeIndex = type;  // also guards against negative type integers
    ଯଦି (typeIndex >= UPRV_LENGTHOF(uprops_idTypeToEncoded)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    uint32_t encodedType = uprops_idTypeToEncoded[typeIndex];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    ଯଦି ((encodedType & UPROPS_ID_TYPE_BIT) != 0) {
        ଫେରନ୍ତୁ value < UPROPS_ID_TYPE_FORBIDDEN && (value & encodedType) != 0;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ value == encodedType;
    }
}

ନାମାକାଶ {

ଶୂନ୍ୟ maybeAppendType(uint32_t value, uint32_t bit, UIdentifierType t,
                     UIdentifierType *types, int32_t &length, int32_t capacity) {
    ଯଦି ((value & bit) != 0) {
        ଯଦି (length < capacity) {
            types[length] = t;
        }
        ++length;
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UErrorCode *pErrorCode) {
    ଯଦି (U_FAILURE(*pErrorCode)) { ଫେରନ୍ତୁ 0; }
    ଯଦି (capacity < 0 || (capacity > 0 && types == nullptr)) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    ଯଦି ((value & UPROPS_ID_TYPE_FORBIDDEN) == UPROPS_ID_TYPE_FORBIDDEN ||
            value == UPROPS_ID_TYPE_NOT_CHARACTER) {
        // single value
        ଯଦି (capacity > 0) {
            UIdentifierType t;
            ବାଛ (value) {
                ସ୍ଥିତି UPROPS_ID_TYPE_NOT_CHARACTER: t = U_ID_TYPE_NOT_CHARACTER; ଭାଙ୍ଗ;
                ସ୍ଥିତି UPROPS_ID_TYPE_DEPRECATED: t = U_ID_TYPE_DEPRECATED; ଭାଙ୍ଗ;
                ସ୍ଥିତି UPROPS_ID_TYPE_DEFAULT_IGNORABLE: t = U_ID_TYPE_DEFAULT_IGNORABLE; ଭାଙ୍ଗ;
                ସ୍ଥିତି UPROPS_ID_TYPE_NOT_NFKC: t = U_ID_TYPE_NOT_NFKC; ଭାଙ୍ଗ;
                ସ୍ଥିତି UPROPS_ID_TYPE_INCLUSION: t = U_ID_TYPE_INCLUSION; ଭାଙ୍ଗ;
                ସ୍ଥିତି UPROPS_ID_TYPE_RECOMMENDED: t = U_ID_TYPE_RECOMMENDED; ଭାଙ୍ଗ;
                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                    *pErrorCode = U_INVALID_FORMAT_ERROR;
                    ଫେରନ୍ତୁ 0;
            }
            types[0] = t;
        } ଅନ୍ୟଥା {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        ଫେରନ୍ତୁ 1;
    } ଅନ୍ୟଥା {
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
        ଯଦି (length >= capacity) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        ଫେରନ୍ତୁ length;
    }
}

#ଯଦି !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_getFC_NFKC_Closure(UChar32 c, char16_t *dest, int32_t destCapacity, UErrorCode *pErrorCode) {
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    ଅଚଳ Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*pErrorCode);
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    ଅଚଳ char16_t *folded1;
    int32_t folded1Length=ucase_toFullFolding(c, &folded1, U_FOLD_CASE_DEFAULT);
    ଯଦି(folded1Length<0) {
        ଅଚଳ Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        ଯଦି(nfkcImpl->getCompQuickCheck(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            ଫେରନ୍ତୁ u_terminateUChars(dest, destCapacity, 0, pErrorCode);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.setTo(c);
    } ଅନ୍ୟଥା {
        ଯଦି(folded1Length>UCASE_MAX_STRING_LENGTH) {
            folded1String.setTo(folded1Length);
        } ଅନ୍ୟଥା {
            folded1String.setTo(ମିଥ୍ୟା, folded1, folded1Length);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *pErrorCode);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *pErrorCode);
    // if (c != b) add the mapping from a to c
    ଯଦି(U_FAILURE(*pErrorCode) || kc1==kc2) {
        ଫେରନ୍ତୁ u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ kc2.extract(dest, destCapacity, *pErrorCode);
    }
}

#endif
