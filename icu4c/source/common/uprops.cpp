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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/unorm2.h"
#ಅನ್ತರ್ಭಾವ "unicode/uscript.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "emojiprops.h"
#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "normalizer2impl.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "ubidi_props.h"
#ಅನ್ತರ್ಭಾವ "uprops.h"
#ಅನ್ತರ್ಭಾವ "ucase.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "ulayout_props.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

U_NAMESPACE_USE

// Unicode text layout properties data -----------------------------------------

ನಾಮಾಕಾಶ {

icu::UInitOnce gLayoutInitOnce {};
UDataMemory *gLayoutMemory = nullptr;

UCPTrie *gInpcTrie = nullptr;  // Indic_Positional_Category
UCPTrie *gInscTrie = nullptr;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = nullptr;  // Vertical_Orientation

int32_t g_Max_Inpc_ಮೌಲ್ಯ = 0;
int32_t g_Max_Insc_ಮೌಲ್ಯ = 0;
int32_t g_Max_Vo_ಮೌಲ್ಯ = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(gLayoutMemory);
    gLayoutMemory = nullptr;

    ucptrie_close(gInpcTrie);
    gInpcTrie = nullptr;
    ucptrie_close(gInscTrie);
    gInscTrie = nullptr;
    ucptrie_close(gVoTrie);
    gVoTrie = nullptr;

    g_Max_Inpc_ಮೌಲ್ಯ = 0;
    g_Max_Insc_ಮೌಲ್ಯ = 0;
    g_Max_Vo_ಮೌಲ್ಯ = 0;

    gLayoutInitOnce.reset();
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

UBool U_CALLCONV
ulayout_isAcceptable(ಶೂನ್ಯ * /*context*/,
                     ಧ್ರುವ ವರ್ಣ * /* type */, ಧ್ರುವ ವರ್ಣ * /*name*/,
                     ಧ್ರುವ UDataInfo *pInfo) {
    ಪ್ರತ್ಯಾಗಮ pInfo->size >= 20 &&
        pInfo->isBigEndian == U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily == U_CHARSET_FAMILY &&
        pInfo->dataFormat[0] == ULAYOUT_FMT_0 &&
        pInfo->dataFormat[1] == ULAYOUT_FMT_1 &&
        pInfo->dataFormat[2] == ULAYOUT_FMT_2 &&
        pInfo->dataFormat[3] == ULAYOUT_FMT_3 &&
        pInfo->formatVersion[0] == 1;
}

// UInitOnce singleton initialization function
ಶೂನ್ಯ U_CALLCONV ulayout_load(UErrorCode &errorCode) {
    gLayoutMemory = udata_openChoice(
        nullptr, ULAYOUT_DATA_TYPE, ULAYOUT_DATA_NAME,
        ulayout_isAcceptable, nullptr, &errorCode);
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }

    ಧ್ರುವ uint8_t* inBytes = static_cast<ಧ್ರುವ uint8_t*>(udata_getMemory(gLayoutMemory));
    ಧ್ರುವ int32_t* inIndexes = reinterpret_cast<ಧ್ರುವ int32_t*>(inBytes);
    int32_t indexesLength = inIndexes[ULAYOUT_IX_INDEXES_LENGTH];
    ಯದಿ (indexesLength < 12) {
        errorCode = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ಪ್ರತ್ಯಾಗಮ;
    }
    int32_t offset = indexesLength * 4;
    int32_t ತುದಿ = inIndexes[ULAYOUT_IX_INPC_TRIE_ತುದಿ];
    int32_t trieSize = ತುದಿ - offset;
    ಯದಿ (trieSize >= 16) {
        gInpcTrie = ucptrie_open_From_ದ್ವಿಮಾನ(
            UCPTRIE_TYPE_ANY, UCPTRIE_ಮೌಲ್ಯ_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }
    offset = ತುದಿ;
    ತುದಿ = inIndexes[ULAYOUT_IX_INSC_TRIE_ತುದಿ];
    trieSize = ತುದಿ - offset;
    ಯದಿ (trieSize >= 16) {
        gInscTrie = ucptrie_open_From_ದ್ವಿಮಾನ(
            UCPTRIE_TYPE_ANY, UCPTRIE_ಮೌಲ್ಯ_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }
    offset = ತುದಿ;
    ತುದಿ = inIndexes[ULAYOUT_IX_VO_TRIE_ತುದಿ];
    trieSize = ತುದಿ - offset;
    ಯದಿ (trieSize >= 16) {
        gVoTrie = ucptrie_open_From_ದ್ವಿಮಾನ(
            UCPTRIE_TYPE_ANY, UCPTRIE_ಮೌಲ್ಯ_BITS_ANY,
            inBytes + offset, trieSize, nullptr, &errorCode);
    }

    uint32_t max_ಮೌಲ್ಯಗಳು = inIndexes[ULAYOUT_IX_MAX_ಮೌಲ್ಯಗಳು];
    g_Max_Inpc_ಮೌಲ್ಯ = max_ಮೌಲ್ಯಗಳು >> ULAYOUT_MAX_INPC_SHIFT;
    g_Max_Insc_ಮೌಲ್ಯ = (max_ಮೌಲ್ಯಗಳು >> ULAYOUT_MAX_INSC_SHIFT) & 0xff;
    g_Max_Vo_ಮೌಲ್ಯ = (max_ಮೌಲ್ಯಗಳು >> ULAYOUT_MAX_VO_SHIFT) & 0xff;

    ucln_common_registerCleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensureData(UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_load, errorCode);
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(errorCode);
}

UBool ulayout_ensureData() {
    UErrorCode errorCode = U_ZERO_ERROR;
    ಪ್ರತ್ಯಾಗಮ ulayout_ensureData(errorCode);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

ಸಂರಚನಾ ದ್ವಿಮಾನ_Property;

ಪ್ರಕಾರನಿರ್ದೇಶ UBool ದ್ವಿಮಾನ_Property_Contains(ಧ್ರುವ ದ್ವಿಮಾನ_Property &prop, UChar32 c, UProperty which);

ಸಂರಚನಾ ದ್ವಿಮಾನ_Property {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    ದ್ವಿಮಾನ_Property_Contains *contains;
};

ಸ್ಥಿರ UBool defaultContains(ಧ್ರುವ ದ್ವಿಮಾನ_Property &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    ಪ್ರತ್ಯಾಗಮ (u_getUnicodeProperties(c, prop.column)&prop.mask)!=0;
}

ಸ್ಥಿರ UBool case_ದ್ವಿಮಾನ_Property_Contains(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty which) {
    ಪ್ರತ್ಯಾಗಮ ucase_has_ದ್ವಿಮಾನ_Property(c, which);
}

ಸ್ಥಿರ UBool isBidiControl(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ ubidi_isBidiControl(c);
}

ಸ್ಥಿರ UBool isMirrored(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ ubidi_isMirrored(c);
}

ಸ್ಥಿರ UBool isJoinControl(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ ubidi_isJoinControl(c);
}

#ಯದಿ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ UBool hasFullCompositionExclusion(ಧ್ರುವ ದ್ವಿಮಾನ_Property &, UChar32, UProperty) {
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ UBool hasFullCompositionExclusion(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UErrorCode errorCode=U_ZERO_ERROR;
    ಧ್ರುವ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(errorCode) && impl->isCompNo(impl->getNorm16(c));
}
#endif

// UCHAR_NF*_INERT properties
#ಯದಿ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ UBool isNormInert(ಧ್ರುವ ದ್ವಿಮಾನ_Property &, UChar32, UProperty) {
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ UBool isNormInert(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty which) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ಧ್ರುವ Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalizationMode>(which - UCHAR_NFD_INERT + UNORM_NFD), errorCode);
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(errorCode) && norm2->isInert(c);
}
#endif

#ಯದಿ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ UBool changesWhenCasefolded(ಧ್ರುವ ದ್ವಿಮಾನ_Property &, UChar32, UProperty) {
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ UBool changesWhenCasefolded(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UErrorCode errorCode=U_ZERO_ERROR;
    ಧ್ರುವ Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(errorCode);
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        ಯದಿ(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } ಅನ್ಯಥಾ ಯದಿ(nfd.length()<=U16_MAX_LENGTH &&
                  nfd.length()==U16_LENGTH(c=nfd.char32At(0))
        ) {
            /* single supplementary code point */
        } ಅನ್ಯಥಾ {
            c=U_SENTINEL;
        }
    } ಅನ್ಯಥಾ ಯದಿ(c<0) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;  /* protect against bad input */
    }
    ಯದಿ(c>=0) {
        /* single code point */
        ಧ್ರುವ char16_t *resultString;
        ಪ್ರತ್ಯಾಗಮ ucase_toFullFolding(c, &resultString, U_FOLD_CASE_DEFAULT) >= 0;
    } ಅನ್ಯಥಾ {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_LENGTH];
        int32_t destLength;
        destLength=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.getBuffer(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &errorCode);
        ಪ್ರತ್ಯಾಗಮ U_SUCCESS(errorCode) &&
                       0!=u_strCompare(nfd.getBuffer(), nfd.length(),
                                       dest, destLength, ಅಸತ್ಯ);
    }
}
#endif

#ಯದಿ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ UBool changesWhenNFKC_Casefolded(ಧ್ರುವ ದ್ವಿಮಾನ_Property &, UChar32, UProperty) {
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ UBool changesWhenNFKC_Casefolded(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ಧ್ರುವ Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(errorCode);
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        ReorderingBuffer buffer(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        ಯದಿ(buffer.init(5, errorCode)) {
            ಧ್ರುವ char16_t *srcArray=src.getBuffer();
            kcf->compose(srcArray, srcArray+src.length(), ಅಸತ್ಯ,
                          ಸತ್ಯ, buffer, errorCode);
        }
    }
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(errorCode) && dest!=src;
}
#endif

#ಯದಿ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ UBool isCanonSegmentStarter(ಧ್ರುವ ದ್ವಿಮಾನ_Property &, UChar32, UProperty) {
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ UBool isCanonSegmentStarter(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ಧ್ರುವ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    ಪ್ರತ್ಯಾಗಮ
        U_SUCCESS(errorCode) && impl->ensureCanonIterData(errorCode) &&
        impl->isCanonSegmentStarter(c);
}
#endif

ಸ್ಥಿರ UBool isPOSIX_alnum(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ u_isalnumPOSIX(c);
}

ಸ್ಥಿರ UBool isPOSIX_blank(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ u_isblank(c);
}

ಸ್ಥಿರ UBool isPOSIX_graph(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ u_isgraphPOSIX(c);
}

ಸ್ಥಿರ UBool isPOSIX_print(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ u_isprintPOSIX(c);
}

ಸ್ಥಿರ UBool isPOSIX_xdigit(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ u_isxdigit(c);
}

ಸ್ಥಿರ UBool isRegionalIndicator(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    ಪ್ರತ್ಯಾಗಮ 0x1F1E6<=c && c<=0x1F1FF;
}

ಸ್ಥಿರ UBool hasEmojiProperty(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty which) {
    ಪ್ರತ್ಯಾಗಮ EmojiProps::has_ದ್ವಿಮಾನ_Property(c, which);
}

ಸ್ಥಿರ UBool isIDSUnaryOperator(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // New in Unicode 15.1 for just two characters.
    ಪ್ರತ್ಯಾಗಮ 0x2FFE<=c && c<=0x2FFF;
}

/** Ranges (start/limit pairs) of ID_Compat_Math_Continue (only), from UCD PropList.txt. */
ಸ್ಥಿರ constexpr UChar32 ID_COMPAT_MATH_CONTINUE[] = {
    0x00B2, 0x00B3 + 1,
    0x00B9, 0x00B9 + 1,
    0x2070, 0x2070 + 1,
    0x2074, 0x207E + 1,
    0x2080, 0x208E + 1
};

/** ID_Compat_Math_Start characters, from UCD PropList.txt. */
ಸ್ಥಿರ constexpr UChar32 ID_COMPAT_MATH_START[] = {
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
ಸ್ಥಿರ constexpr UChar32 MODIFIER_COMBINING_MARK[] = {
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

ಸ್ಥಿರ UBool isIDCompatMathStart(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಯದಿ (c < ID_COMPAT_MATH_START[0]) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }  // fastpath for common scripts
    ಪ್ರತಿ (UChar32 startChar : ID_COMPAT_MATH_START) {
        ಯದಿ (c == startChar) { ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಸ್ಥಿರ UBool isIDCompatMathContinue(ಧ್ರುವ ದ್ವಿಮಾನ_Property &prop, UChar32 c, UProperty /*which*/) {
    ಪ್ರತಿ (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        ಯದಿ (c < ID_COMPAT_MATH_CONTINUE[i]) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }  // below range start
        ಯದಿ (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; }  // below range limit
    }
    ಪ್ರತ್ಯಾಗಮ isIDCompatMathStart(prop, c, UCHAR_ID_COMPAT_MATH_START);
}

ಸ್ಥಿರ UBool isModifierCombiningMark(ಧ್ರುವ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತಿ (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        ಯದಿ (c < MODIFIER_COMBINING_MARK[i]) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }  // below range start
        ಯದಿ (c < MODIFIER_COMBINING_MARK[i + 1]) { ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; }  // below range limit
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಸ್ಥಿರ ಧ್ರುವ ದ್ವಿಮಾನ_Property binProps[UCHAR_ದ್ವಿಮಾನ_LIMIT]={
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
    { 1,                U_MASK(UPROPS_IDS_ದ್ವಿಮಾನ_OPERATOR), defaultContains },
    { 1,                U_MASK(UPROPS_IDS_TRINARY_OPERATOR), defaultContains },
    { UPROPS_SRC_BIDI,  0, isJoinControl },
    { 1,                U_MASK(UPROPS_LOGICAL_ORDER_EXCEPTION), defaultContains },
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_Contains },  // UCHAR_LOWERCASE
    { 1,                U_MASK(UPROPS_MATH), defaultContains },
    { 1,                U_MASK(UPROPS_NONCHARACTER_CODE_POINT), defaultContains },
    { 1,                U_MASK(UPROPS_QUOTATION_MARK), defaultContains },
    { 1,                U_MASK(UPROPS_RADICAL), defaultContains },
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_Contains },  // UCHAR_SOFT_DOTTED
    { 1,                U_MASK(UPROPS_TERMINAL_PUNCTUATION), defaultContains },
    { 1,                U_MASK(UPROPS_UNIFIED_IDEOGRAPH), defaultContains },
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_Contains },  // UCHAR_UPPERCASE
    { 1,                U_MASK(UPROPS_WHITE_SPACE), defaultContains },
    { 1,                U_MASK(UPROPS_XID_CONTINUE), defaultContains },
    { 1,                U_MASK(UPROPS_XID_START), defaultContains },
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_Contains },  // UCHAR_CASE_SENSITIVE
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
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_Contains },  // UCHAR_CASED
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_Contains },  // UCHAR_CASE_IGNORABLE
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_Contains },  // UCHAR_CHANGES_WHEN_LOWERCASED
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_Contains },  // UCHAR_CHANGES_WHEN_UPPERCASED
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_Contains },  // UCHAR_CHANGES_WHEN_TITLECASED
    { UPROPS_SRC_CASE_AND_NORM,  0, changesWhenCasefolded },
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_Contains },  // UCHAR_CHANGES_WHEN_CASEMAPPED
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
u_has_ದ್ವಿಮಾನ_Property(UChar32 c, UProperty which) {
    /* c is range-checked in the functions that are called from here */
    ಯದಿ(which<UCHAR_ದ್ವಿಮಾನ_START || UCHAR_ದ್ವಿಮಾನ_LIMIT<=which) {
        /* not a known binary property */
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    } ಅನ್ಯಥಾ {
        ಧ್ರುವ ದ್ವಿಮಾನ_Property &prop=binProps[which];
        ಪ್ರತ್ಯಾಗಮ prop.contains(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    ಪ್ರತ್ಯಾಗಮ u_has_ದ್ವಿಮಾನ_Property(c, UCHAR_ID_START);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    ಪ್ರತ್ಯಾಗಮ u_has_ದ್ವಿಮಾನ_Property(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_string_Has_ದ್ವಿಮಾನ_Property(ಧ್ರುವ char16_t *s, int32_t length, UProperty which) {
    ಯದಿ (s == nullptr && length != 0) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    ಯದಿ (length == 1) {
        ಪ್ರತ್ಯಾಗಮ u_has_ದ್ವಿಮಾನ_Property(s[0], which);  // single code point
    } ಅನ್ಯಥಾ ಯದಿ (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        ಯದಿ (length > 0 ? i == length : s[i] == 0) {
            ಪ್ರತ್ಯಾಗಮ u_has_ದ್ವಿಮಾನ_Property(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    ಪ್ರತ್ಯಾಗಮ UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::has_ದ್ವಿಮಾನ_Property(s, length, which);
}

ಸಂರಚನಾ IntProperty;

ಪ್ರಕಾರನಿರ್ದೇಶ int32_t Int_Property_Get_ಮೌಲ್ಯ(ಧ್ರುವ IntProperty &prop, UChar32 c, UProperty which);
ಪ್ರಕಾರನಿರ್ದೇಶ int32_t Int_Property_Get_Max_ಮೌಲ್ಯ(ಧ್ರುವ IntProperty &prop, UProperty which);

ಸಂರಚನಾ IntProperty {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t shift;  // =maxValue if getMaxValueFromShift() is used
    Int_Property_Get_ಮೌಲ್ಯ *get_ಮೌಲ್ಯ;
    Int_Property_Get_Max_ಮೌಲ್ಯ *get_Max_ಮೌಲ್ಯ;
};

ಸ್ಥಿರ int32_t default_Get_ಮೌಲ್ಯ(ಧ್ರುವ IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(u_getUnicodeProperties(c, prop.column) & prop.mask) >> prop.shift;
}

ಸ್ಥಿರ int32_t default_Get_Max_ಮೌಲ್ಯ(ಧ್ರುವ IntProperty &prop, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ (uprv_get_Max_ಮೌಲ್ಯಗಳು(prop.column)&prop.mask)>>prop.shift;
}

ಸ್ಥಿರ int32_t get_Max_ಮೌಲ್ಯ_From_Shift(ಧ್ರುವ IntProperty &prop, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ prop.shift;
}

ಸ್ಥಿರ int32_t getBiDiClass(ಧ್ರುವ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(u_charDirection(c));
}

ಸ್ಥಿರ int32_t getBiDiPairedBracketType(ಧ್ರುವ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

ಸ್ಥಿರ int32_t bi_Di_Get_Max_ಮೌಲ್ಯ(ಧ್ರುವ IntProperty &/*prop*/, UProperty which) {
    ಪ್ರತ್ಯಾಗಮ ubidi_get_Max_ಮೌಲ್ಯ(which);
}

ಸ್ಥಿರ int32_t getBlock(ಧ್ರುವ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(ublock_getCode(c));
}

ಸ್ಥಿರ int32_t block_Get_Max_ಮೌಲ್ಯ(ಧ್ರುವ IntProperty &/*prop*/, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ uprv_get_Max_ಮೌಲ್ಯಗಳು(UPROPS_MAX_ಮೌಲ್ಯಗಳು_OTHER_INDEX) & UPROPS_MAX_BLOCK;
}

#ಯದಿ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ int32_t getCombiningClass(ಧ್ರುವ IntProperty &, UChar32, UProperty) {
    ಪ್ರತ್ಯಾಗಮ 0;
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ int32_t getCombiningClass(ಧ್ರುವ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ u_getCombiningClass(c);
}
#endif

ಸ್ಥಿರ int32_t getGeneralCategory(ಧ್ರುವ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(u_charType(c));
}

ಸ್ಥಿರ int32_t getJoiningGroup(ಧ್ರುವ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ ubidi_getJoiningGroup(c);
}

ಸ್ಥಿರ int32_t getJoiningType(ಧ್ರುವ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ ubidi_getJoiningType(c);
}

ಸ್ಥಿರ int32_t getNumericType(ಧ್ರುವ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_ಮೌಲ್ಯ(u_getMainProperties(c)));
    ಪ್ರತ್ಯಾಗಮ UPROPS_NTV_GET_TYPE(ntv);
}

ಸ್ಥಿರ int32_t getScript(ಧ್ರುವ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(uscript_getScript(c, &errorCode));
}

ಸ್ಥಿರ int32_t script_Get_Max_ಮೌಲ್ಯ(ಧ್ರುವ IntProperty &/*prop*/, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ uprv_get_Max_ಮೌಲ್ಯಗಳು(0)&UPROPS_MAX_SCRIPT;
}

/*
 * Map some of the Grapheme Cluster Break values to Hangul Syllable Types.
 * Hangul_Syllable_Type used to be fully redundant with a subset of Grapheme_Cluster_Break.
 *
 * Starting with Unicode 16, this is no longer true for HST=V vs. GCB=V in some cases:
 * Some Kirat Rai vowels are given GCB=V for proper grapheme clustering, but
 * they are of course not related to Hangul syllables.
 */
ಸ್ಥಿರ ಧ್ರುವ UHangulSyllableType gcbToHst[]={
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

ಸ್ಥಿರ int32_t getHangulSyllableType(ಧ್ರುವ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Ignore supplementary code points: They all have HST=NA.
    // This is a simple way to handle the GCB!=hst cases since Unicode 16 (Kirat Rai vowels).
    ಯದಿ(c>0xffff) {
        ಪ್ರತ್ಯಾಗಮ U_HST_NOT_APPLICABLE;
    }
    /* see comments on gcbToHst[] above */
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_MASK) >> UPROPS_GCB_SHIFT;
    ಯದಿ(gcb<UPRV_LENGTHOF(gcbToHst)) {
        ಪ್ರತ್ಯಾಗಮ gcbToHst[gcb];
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ U_HST_NOT_APPLICABLE;
    }
}

#ಯದಿ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ int32_t getNormQuickCheck(ಧ್ರುವ IntProperty &, UChar32, UProperty) {
    ಪ್ರತ್ಯಾಗಮ 0;
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ int32_t getNormQuickCheck(ಧ್ರುವ IntProperty &/*prop*/, UChar32 c, UProperty which) {
    ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(unorm_getQuickCheck(c, static_cast<UNormalizationMode>(which - UCHAR_NFD_QUICK_CHECK + UNORM_NFD)));
}
#endif

#ಯದಿ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ int32_t getLeadCombiningClass(ಧ್ರುವ IntProperty &, UChar32, UProperty) {
    ಪ್ರತ್ಯಾಗಮ 0;
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ int32_t getLeadCombiningClass(ಧ್ರುವ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ unorm_getFCD16(c)>>8;
}
#endif

#ಯದಿ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ int32_t getTrailCombiningClass(ಧ್ರುವ IntProperty &, UChar32, UProperty) {
    ಪ್ರತ್ಯಾಗಮ 0;
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ int32_t getTrailCombiningClass(ಧ್ರುವ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ unorm_getFCD16(c)&0xff;
}
#endif

ಸ್ಥಿರ int32_t getInPC(ಧ್ರುವ IntProperty &, UChar32 c, UProperty) {
    ಪ್ರತ್ಯಾಗಮ ulayout_ensureData() && gInpcTrie != nullptr ? ucptrie_get(gInpcTrie, c) : 0;
}

ಸ್ಥಿರ int32_t getInSC(ಧ್ರುವ IntProperty &, UChar32 c, UProperty) {
    ಪ್ರತ್ಯಾಗಮ ulayout_ensureData() && gInscTrie != nullptr ? ucptrie_get(gInscTrie, c) : 0;
}

ಸ್ಥಿರ int32_t getVo(ಧ್ರುವ IntProperty &, UChar32 c, UProperty) {
    ಪ್ರತ್ಯಾಗಮ ulayout_ensureData() && gVoTrie != nullptr ? ucptrie_get(gVoTrie, c) : 0;
}

ಸ್ಥಿರ int32_t layout_Get_Max_ಮೌಲ್ಯ(ಧ್ರುವ IntProperty &/*prop*/, UProperty which) {
    ಯದಿ (!ulayout_ensureData()) { ಪ್ರತ್ಯಾಗಮ 0; }
    ವಿಕಲ್ಪ (which) {
    ಪಕ್ಷ UCHAR_INDIC_POSITIONAL_CATEGORY:
        ಪ್ರತ್ಯಾಗಮ g_Max_Inpc_ಮೌಲ್ಯ;
    ಪಕ್ಷ UCHAR_INDIC_SYLLABIC_CATEGORY:
        ಪ್ರತ್ಯಾಗಮ g_Max_Insc_ಮೌಲ್ಯ;
    ಪಕ್ಷ UCHAR_VERTICAL_ORIENTATION:
        ಪ್ರತ್ಯಾಗಮ g_Max_Vo_ಮೌಲ್ಯ;
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ 0;
    }
}

ಸ್ಥಿರ int32_t get_IDStatus_ಮೌಲ್ಯ(ಧ್ರುವ IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    ಪ್ರತ್ಯಾಗಮ value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_STATUS_ALLOWED : U_ID_STATUS_RESTRICTED;
}

ಸ್ಥಿರ ಧ್ರುವ IntProperty intProps[UCHAR_INT_LIMIT-UCHAR_INT_START]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, bi_Di_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_BLOCK, 0, 0,                               getBlock, block_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, get_Max_ಮೌಲ್ಯ_From_Shift },
    { 2,                UPROPS_DT_MASK, 0,                  default_Get_ಮೌಲ್ಯ, default_Get_Max_ಮೌಲ್ಯ },
    { 0,                UPROPS_EA_MASK, UPROPS_EA_SHIFT,    default_Get_ಮೌಲ್ಯ, default_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_ಎಣಿಕೆ) - 1, getGeneralCategory, get_Max_ಮೌಲ್ಯ_From_Shift },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningGroup, bi_Di_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, bi_Di_Get_Max_ಮೌಲ್ಯ },
    { 2,                UPROPS_LB_MASK, UPROPS_LB_SHIFT,    default_Get_ಮೌಲ್ಯ, default_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_ಎಣಿಕೆ) - 1, getNumericType, get_Max_ಮೌಲ್ಯ_From_Shift },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, script_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_ಎಣಿಕೆ) - 1, getHangulSyllableType, get_Max_ಮೌಲ್ಯ_From_Shift },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, get_Max_ಮೌಲ್ಯ_From_Shift },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), getNormQuickCheck, get_Max_ಮೌಲ್ಯ_From_Shift },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, get_Max_ಮೌಲ್ಯ_From_Shift },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), getNormQuickCheck, get_Max_ಮೌಲ್ಯ_From_Shift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, get_Max_ಮೌಲ್ಯ_From_Shift },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, get_Max_ಮೌಲ್ಯ_From_Shift },
    { 2,                UPROPS_GCB_MASK, UPROPS_GCB_SHIFT,  default_Get_ಮೌಲ್ಯ, default_Get_Max_ಮೌಲ್ಯ },
    { 2,                UPROPS_SB_MASK, UPROPS_SB_SHIFT,    default_Get_ಮೌಲ್ಯ, default_Get_Max_ಮೌಲ್ಯ },
    { 2,                UPROPS_WB_MASK, UPROPS_WB_SHIFT,    default_Get_ಮೌಲ್ಯ, default_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, bi_Di_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_INPC,  0, 0,                               getInPC, layout_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_INSC,  0, 0,                               getInSC, layout_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layout_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_STATUS_ALLOWED), get_IDStatus_ಮೌಲ್ಯ, get_Max_ಮೌಲ್ಯ_From_Shift },
    { 0,                UPROPS_INCB_MASK, UPROPS_INCB_SHIFT,default_Get_ಮೌಲ್ಯ, default_Get_Max_ಮೌಲ್ಯ },
};

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_ಮೌಲ್ಯ(UChar32 c, UProperty which) {
    ಯದಿ(which<UCHAR_INT_START) {
        ಯದಿ(UCHAR_ದ್ವಿಮಾನ_START<=which && which<UCHAR_ದ್ವಿಮಾನ_LIMIT) {
            ಧ್ರುವ ದ್ವಿಮಾನ_Property &prop=binProps[which];
            ಪ್ರತ್ಯಾಗಮ prop.contains(prop, c, which);
        }
    } ಅನ್ಯಥಾ ಯದಿ(which<UCHAR_INT_LIMIT) {
        ಧ್ರುವ IntProperty &prop=intProps[which-UCHAR_INT_START];
        ಪ್ರತ್ಯಾಗಮ prop.get_ಮೌಲ್ಯ(prop, c, which);
    } ಅನ್ಯಥಾ ಯದಿ(which==UCHAR_GENERAL_CATEGORY_MASK) {
        ಪ್ರತ್ಯಾಗಮ U_MASK(u_charType(c));
    }
    ಪ್ರತ್ಯಾಗಮ 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Min_ಮೌಲ್ಯ(UProperty /*which*/) {
    ಪ್ರತ್ಯಾಗಮ 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Max_ಮೌಲ್ಯ(UProperty which) {
    ಯದಿ(which<UCHAR_INT_START) {
        ಯದಿ(UCHAR_ದ್ವಿಮಾನ_START<=which && which<UCHAR_ದ್ವಿಮಾನ_LIMIT) {
            ಪ್ರತ್ಯಾಗಮ 1;  // maximum true for all binary properties
        }
    } ಅನ್ಯಥಾ ಯದಿ(which<UCHAR_INT_LIMIT) {
        ಧ್ರುವ IntProperty &prop=intProps[which-UCHAR_INT_START];
        ಪ್ರತ್ಯಾಗಮ prop.get_Max_ಮೌಲ್ಯ(prop, which);
    }
    ಪ್ರತ್ಯಾಗಮ -1;  // undefined
}

U_CFUNC UPropertySource U_EXPORT2
uprops_getSource(UProperty which) {
    ಯದಿ(which<UCHAR_ದ್ವಿಮಾನ_START) {
        ಪ್ರತ್ಯಾಗಮ UPROPS_SRC_NONE; /* undefined */
    } ಅನ್ಯಥಾ ಯದಿ(which<UCHAR_ದ್ವಿಮಾನ_LIMIT) {
        ಧ್ರುವ ದ್ವಿಮಾನ_Property &prop=binProps[which];
        ಯದಿ(prop.mask!=0) {
            ಪ್ರತ್ಯಾಗಮ UPROPS_SRC_PROPSVEC;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ (UPropertySource)prop.column;
        }
    } ಅನ್ಯಥಾ ಯದಿ(which<UCHAR_INT_START) {
        ಪ್ರತ್ಯಾಗಮ UPROPS_SRC_NONE; /* undefined */
    } ಅನ್ಯಥಾ ಯದಿ(which<UCHAR_INT_LIMIT) {
        ಧ್ರುವ IntProperty &prop=intProps[which-UCHAR_INT_START];
        ಯದಿ(prop.mask!=0) {
            ಪ್ರತ್ಯಾಗಮ UPROPS_SRC_PROPSVEC;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ (UPropertySource)prop.column;
        }
    } ಅನ್ಯಥಾ ಯದಿ(which<UCHAR_STRING_START) {
        ವಿಕಲ್ಪ(which) {
        ಪಕ್ಷ UCHAR_GENERAL_CATEGORY_MASK:
        ಪಕ್ಷ UCHAR_NUMERIC_ಮೌಲ್ಯ:
            ಪ್ರತ್ಯಾಗಮ UPROPS_SRC_CHAR;

        ಸ್ವಭಾವ:
            ಪ್ರತ್ಯಾಗಮ UPROPS_SRC_NONE;
        }
    } ಅನ್ಯಥಾ ಯದಿ(which<UCHAR_STRING_LIMIT) {
        ವಿಕಲ್ಪ(which) {
        ಪಕ್ಷ UCHAR_AGE:
            ಪ್ರತ್ಯಾಗಮ UPROPS_SRC_PROPSVEC;

        ಪಕ್ಷ UCHAR_BIDI_MIRRORING_GLYPH:
            ಪ್ರತ್ಯಾಗಮ UPROPS_SRC_BIDI;

        ಪಕ್ಷ UCHAR_CASE_FOLDING:
        ಪಕ್ಷ UCHAR_LOWERCASE_MAPPING:
        ಪಕ್ಷ UCHAR_SIMPLE_CASE_FOLDING:
        ಪಕ್ಷ UCHAR_SIMPLE_LOWERCASE_MAPPING:
        ಪಕ್ಷ UCHAR_SIMPLE_TITLECASE_MAPPING:
        ಪಕ್ಷ UCHAR_SIMPLE_UPPERCASE_MAPPING:
        ಪಕ್ಷ UCHAR_TITLECASE_MAPPING:
        ಪಕ್ಷ UCHAR_UPPERCASE_MAPPING:
            ಪ್ರತ್ಯಾಗಮ UPROPS_SRC_CASE;

        ಪಕ್ಷ UCHAR_ISO_COMMENT:
        ಪಕ್ಷ UCHAR_NAME:
        ಪಕ್ಷ UCHAR_UNICODE_1_NAME:
            ಪ್ರತ್ಯಾಗಮ UPROPS_SRC_NAMES;

        ಸ್ವಭಾವ:
            ಪ್ರತ್ಯಾಗಮ UPROPS_SRC_NONE;
        }
    } ಅನ್ಯಥಾ {
        ವಿಕಲ್ಪ(which) {
        ಪಕ್ಷ UCHAR_SCRIPT_EXTENSIONS:
        ಪಕ್ಷ UCHAR_IDENTIFIER_TYPE:
            ಪ್ರತ್ಯಾಗಮ UPROPS_SRC_PROPSVEC;
        ಸ್ವಭಾವ:
            ಪ್ರತ್ಯಾಗಮ UPROPS_SRC_NONE; /* undefined */
        }
    }
}

U_CFUNC ಶೂನ್ಯ U_EXPORT2
uprops_addPropertyStarts(UPropertySource src, ಧ್ರುವ USetAdder *sa, UErrorCode *pErrorCode) {
    ಯದಿ (U_FAILURE(*pErrorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    ಯದಿ (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        ಪ್ರತಿ (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->add(sa->set, c);
        }
        // single characters
        ಪ್ರತಿ (UChar32 c : ID_COMPAT_MATH_START) {
            sa->add(sa->set, c);
            sa->add(sa->set, c + 1);
        }
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (src == UPROPS_SRC_MCM) {
        // range limits
        ಪ್ರತಿ (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->add(sa->set, c);
        }
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (!ulayout_ensureData(*pErrorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    ಧ್ರುವ UCPTrie *trie;
    ವಿಕಲ್ಪ (src) {
    ಪಕ್ಷ UPROPS_SRC_INPC:
        trie = gInpcTrie;
        ಭಙ್ಗ;
    ಪಕ್ಷ UPROPS_SRC_INSC:
        trie = gInscTrie;
        ಭಙ್ಗ;
    ಪಕ್ಷ UPROPS_SRC_VO:
        trie = gVoTrie;
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (trie == nullptr) {
        *pErrorCode = U_MISSING_RESOURCE_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    ಯಾವತ್ ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, nullptr)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

U_CAPI ತಾರ್ಕಿಕ U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t typeIndex = type;  // also guards against negative type integers
    ಯದಿ (typeIndex >= UPRV_LENGTHOF(uprops_idTypeToEncoded)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    uint32_t encodedType = uprops_idTypeToEncoded[typeIndex];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    ಯದಿ ((encodedType & UPROPS_ID_TYPE_BIT) != 0) {
        ಪ್ರತ್ಯಾಗಮ value < UPROPS_ID_TYPE_FORBIDDEN && (value & encodedType) != 0;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ value == encodedType;
    }
}

ನಾಮಾಕಾಶ {

ಶೂನ್ಯ maybeAppendType(uint32_t value, uint32_t bit, UIdentifierType t,
                     UIdentifierType *types, int32_t &length, int32_t capacity) {
    ಯದಿ ((value & bit) != 0) {
        ಯದಿ (length < capacity) {
            types[length] = t;
        }
        ++length;
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UErrorCode *pErrorCode) {
    ಯದಿ (U_FAILURE(*pErrorCode)) { ಪ್ರತ್ಯಾಗಮ 0; }
    ಯದಿ (capacity < 0 || (capacity > 0 && types == nullptr)) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_SHIFT;
    ಯದಿ ((value & UPROPS_ID_TYPE_FORBIDDEN) == UPROPS_ID_TYPE_FORBIDDEN ||
            value == UPROPS_ID_TYPE_NOT_CHARACTER) {
        // single value
        ಯದಿ (capacity > 0) {
            UIdentifierType t;
            ವಿಕಲ್ಪ (value) {
                ಪಕ್ಷ UPROPS_ID_TYPE_NOT_CHARACTER: t = U_ID_TYPE_NOT_CHARACTER; ಭಙ್ಗ;
                ಪಕ್ಷ UPROPS_ID_TYPE_DEPRECATED: t = U_ID_TYPE_DEPRECATED; ಭಙ್ಗ;
                ಪಕ್ಷ UPROPS_ID_TYPE_DEFAULT_IGNORABLE: t = U_ID_TYPE_DEFAULT_IGNORABLE; ಭಙ್ಗ;
                ಪಕ್ಷ UPROPS_ID_TYPE_NOT_NFKC: t = U_ID_TYPE_NOT_NFKC; ಭಙ್ಗ;
                ಪಕ್ಷ UPROPS_ID_TYPE_INCLUSION: t = U_ID_TYPE_INCLUSION; ಭಙ್ಗ;
                ಪಕ್ಷ UPROPS_ID_TYPE_RECOMMENDED: t = U_ID_TYPE_RECOMMENDED; ಭಙ್ಗ;
                ಸ್ವಭಾವ:
                    *pErrorCode = U_INVALID_FORMAT_ERROR;
                    ಪ್ರತ್ಯಾಗಮ 0;
            }
            types[0] = t;
        } ಅನ್ಯಥಾ {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ 1;
    } ಅನ್ಯಥಾ {
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
        ಯದಿ (length >= capacity) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ length;
    }
}

#ಯದಿ !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_getFC_NFKC_Closure(UChar32 c, char16_t *dest, int32_t destCapacity, UErrorCode *pErrorCode) {
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    ಧ್ರುವ Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*pErrorCode);
    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    ಧ್ರುವ char16_t *folded1;
    int32_t folded1Length=ucase_toFullFolding(c, &folded1, U_FOLD_CASE_DEFAULT);
    ಯದಿ(folded1Length<0) {
        ಧ್ರುವ Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        ಯದಿ(nfkcImpl->getCompQuickCheck(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            ಪ್ರತ್ಯಾಗಮ u_terminateUChars(dest, destCapacity, 0, pErrorCode);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.setTo(c);
    } ಅನ್ಯಥಾ {
        ಯದಿ(folded1Length>UCASE_MAX_STRING_LENGTH) {
            folded1String.setTo(folded1Length);
        } ಅನ್ಯಥಾ {
            folded1String.setTo(ಅಸತ್ಯ, folded1, folded1Length);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *pErrorCode);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *pErrorCode);
    // if (c != b) add the mapping from a to c
    ಯದಿ(U_FAILURE(*pErrorCode) || kc1==kc2) {
        ಪ್ರತ್ಯಾಗಮ u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ kc2.extract(dest, destCapacity, *pErrorCode);
    }
}

#endif
