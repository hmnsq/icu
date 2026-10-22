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
UData_ಸ್ಮರಣೆ *g_Layout_ಸ್ಮರಣೆ = ಶೂನ್ಯಸೂಚಕ;

UCPTrie *gInpcTrie = ಶೂನ್ಯಸೂಚಕ;  // Indic_Positional_Category
UCPTrie *gInscTrie = ಶೂನ್ಯಸೂಚಕ;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = ಶೂನ್ಯಸೂಚಕ;  // Vertical_Orientation

int32_t g_Max_Inpc_ಮೌಲ್ಯ = 0;
int32_t g_Max_Insc_ಮೌಲ್ಯ = 0;
int32_t g_Max_Vo_ಮೌಲ್ಯ = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(g_Layout_ಸ್ಮರಣೆ);
    g_Layout_ಸ್ಮರಣೆ = ಶೂನ್ಯಸೂಚಕ;

    ucptrie_close(gInpcTrie);
    gInpcTrie = ಶೂನ್ಯಸೂಚಕ;
    ucptrie_close(gInscTrie);
    gInscTrie = ಶೂನ್ಯಸೂಚಕ;
    ucptrie_close(gVoTrie);
    gVoTrie = ಶೂನ್ಯಸೂಚಕ;

    g_Max_Inpc_ಮೌಲ್ಯ = 0;
    g_Max_Insc_ಮೌಲ್ಯ = 0;
    g_Max_Vo_ಮೌಲ್ಯ = 0;

    gLayoutInitOnce.reset();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

UBool U_CALLCONV
ulayout_isAcceptable(ಶೂನ್ಯ * /*context*/,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /* type */, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /*name*/,
                     ಸ್ಥಿರಾಂಕ UDataInfo *pInfo) {
    ಹಿಂತಿರುಗಿ pInfo->size >= 20 &&
        pInfo->isBigEndian == U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily == U_CHARSET_FAMILY &&
        pInfo->ದತ್ತಾಂಶ_Format[0] == ULAYOUT_FMT_0 &&
        pInfo->ದತ್ತಾಂಶ_Format[1] == ULAYOUT_FMT_1 &&
        pInfo->ದತ್ತಾಂಶ_Format[2] == ULAYOUT_FMT_2 &&
        pInfo->ದತ್ತಾಂಶ_Format[3] == ULAYOUT_FMT_3 &&
        pInfo->format_ಆವೃತ್ತಿ[0] == 1;
}

// UInitOnce singleton initialization function
ಶೂನ್ಯ U_CALLCONV ulayout_ತುಂಬು(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    g_Layout_ಸ್ಮರಣೆ = udata_openChoice(
        ಶೂನ್ಯಸೂಚಕ, ULAYOUT_ದತ್ತಾಂಶ_TYPE, ULAYOUT_ದತ್ತಾಂಶ_NAME,
        ulayout_isAcceptable, ಶೂನ್ಯಸೂಚಕ, &error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }

    ಸ್ಥಿರಾಂಕ uint8_t* ಒಳ_ಬೈಟುಗಳು = static_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(udata_get_ಸ್ಮರಣೆ(g_Layout_ಸ್ಮರಣೆ));
    ಸ್ಥಿರಾಂಕ int32_t* ಒಳ_Indexes = reinterpret_cast<ಸ್ಥಿರಾಂಕ int32_t*>(ಒಳ_ಬೈಟುಗಳು);
    int32_t indexes_ಉದ್ದಳತೆ = ಒಳ_Indexes[ULAYOUT_IX_INDEXES_ಉದ್ದಳತೆ];
    ಆದರೆ (indexes_ಉದ್ದಳತೆ < 12) {
        error_ಸಂಕೇತ = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ಹಿಂತಿರುಗಿ;
    }
    int32_t ಸ್ಥಾನಾಂತರ = indexes_ಉದ್ದಳತೆ * 4;
    int32_t ತುದಿ = ಒಳ_Indexes[ULAYOUT_IX_INPC_TRIE_ತುದಿ];
    int32_t trie_ಗಾತ್ರ = ತುದಿ - ಸ್ಥಾನಾಂತರ;
    ಆದರೆ (trie_ಗಾತ್ರ >= 16) {
        gInpcTrie = ucptrie_open_From_ದ್ವಿಮಾನ(
            UCPTRIE_TYPE_ANY, UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_ANY,
            ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, trie_ಗಾತ್ರ, ಶೂನ್ಯಸೂಚಕ, &error_ಸಂಕೇತ);
    }
    ಸ್ಥಾನಾಂತರ = ತುದಿ;
    ತುದಿ = ಒಳ_Indexes[ULAYOUT_IX_INSC_TRIE_ತುದಿ];
    trie_ಗಾತ್ರ = ತುದಿ - ಸ್ಥಾನಾಂತರ;
    ಆದರೆ (trie_ಗಾತ್ರ >= 16) {
        gInscTrie = ucptrie_open_From_ದ್ವಿಮಾನ(
            UCPTRIE_TYPE_ANY, UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_ANY,
            ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, trie_ಗಾತ್ರ, ಶೂನ್ಯಸೂಚಕ, &error_ಸಂಕೇತ);
    }
    ಸ್ಥಾನಾಂತರ = ತುದಿ;
    ತುದಿ = ಒಳ_Indexes[ULAYOUT_IX_VO_TRIE_ತುದಿ];
    trie_ಗಾತ್ರ = ತುದಿ - ಸ್ಥಾನಾಂತರ;
    ಆದರೆ (trie_ಗಾತ್ರ >= 16) {
        gVoTrie = ucptrie_open_From_ದ್ವಿಮಾನ(
            UCPTRIE_TYPE_ANY, UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_ANY,
            ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, trie_ಗಾತ್ರ, ಶೂನ್ಯಸೂಚಕ, &error_ಸಂಕೇತ);
    }

    uint32_t max_ಮೌಲ್ಯಗಳು = ಒಳ_Indexes[ULAYOUT_IX_MAX_ಮೌಲ್ಯಗಳು];
    g_Max_Inpc_ಮೌಲ್ಯ = max_ಮೌಲ್ಯಗಳು >> ULAYOUT_MAX_INPC_ಸರಿಸು;
    g_Max_Insc_ಮೌಲ್ಯ = (max_ಮೌಲ್ಯಗಳು >> ULAYOUT_MAX_INSC_ಸರಿಸು) & 0xff;
    g_Max_Vo_ಮೌಲ್ಯ = (max_ಮೌಲ್ಯಗಳು >> ULAYOUT_MAX_VO_ಸರಿಸು) & 0xff;

    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensure_ದತ್ತಾಂಶ(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_ತುಂಬು, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ U_SUCCESS(error_ಸಂಕೇತ);
}

UBool ulayout_ensure_ದತ್ತಾಂಶ() {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;
    ಹಿಂತಿರುಗಿ ulayout_ensure_ದತ್ತಾಂಶ(error_ಸಂಕೇತ);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

ರಚನೆ ದ್ವಿಮಾನ_Property;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ UBool ದ್ವಿಮಾನ_Property_ಒಳಗೊಂಡಿದೆ(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &prop, UChar32 c, UProperty which);

ರಚನೆ ದ್ವಿಮಾನ_Property {
    int32_t ಸ್ತಂಭ;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    ದ್ವಿಮಾನ_Property_ಒಳಗೊಂಡಿದೆ *ಒಳಗೊಂಡಿದೆ;
};

ಸ್ಥಿರ UBool default_ಒಳಗೊಂಡಿದೆ(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    ಹಿಂತಿರುಗಿ (u_getUnicodeProperties(c, prop.ಸ್ತಂಭ)&prop.mask)!=0;
}

ಸ್ಥಿರ UBool case_ದ್ವಿಮಾನ_Property_ಒಳಗೊಂಡಿದೆ(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty which) {
    ಹಿಂತಿರುಗಿ ucase_has_ದ್ವಿಮಾನ_Property(c, which);
}

ಸ್ಥಿರ UBool isBidiControl(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ ubidi_isBidiControl(c);
}

ಸ್ಥಿರ UBool isMirrored(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ ubidi_isMirrored(c);
}

ಸ್ಥಿರ UBool isJoinControl(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ ubidi_isJoinControl(c);
}

#ಆದರೆ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ UBool has_ತುಂಬಿದ_Composition_Exclusion(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &, UChar32, UProperty) {
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ UBool has_ತುಂಬಿದ_Composition_Exclusion(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    ಸ್ಥಿರಾಂಕ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ U_SUCCESS(error_ಸಂಕೇತ) && impl->isCompNo(impl->getNorm16(c));
}
#ಆದರೆ_ಅಂತ್ಯ

// UCHAR_NF*_INERT properties
#ಆದರೆ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ UBool isNormInert(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &, UChar32, UProperty) {
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ UBool isNormInert(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty which) {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    ಸ್ಥಿರಾಂಕ Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalization_ಬಹುಲಕ>(which - UCHAR_NFD_INERT + UNORM_NFD), error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ U_SUCCESS(error_ಸಂಕೇತ) && norm2->isInert(c);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ UBool changesWhenCasefolded(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &, UChar32, UProperty) {
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ UBool changesWhenCasefolded(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    ಸ್ಥಿರಾಂಕ Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        ಆದರೆ(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nfd.length()<=U16_MAX_ಉದ್ದಳತೆ &&
                  nfd.length()==U16_ಉದ್ದಳತೆ(c=nfd.char32At(0))
        ) {
            /* single supplementary code point */
        } ಇಲ್ಲದಿದ್ದರೆ {
            c=U_SENTINEL;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<0) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;  /* protect against bad input */
    }
    ಆದರೆ(c>=0) {
        /* single code point */
        ಸ್ಥಿರಾಂಕ char16_t *ಫಲಿತಾಂಶ_String;
        ಹಿಂತಿರುಗಿ ucase_ಗಮ್ಯ_ತುಂಬಿದ_Folding(c, &ಫಲಿತಾಂಶ_String, U_FOLD_CASE_DEFAULT) >= 0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_ಉದ್ದಳತೆ];
        int32_t dest_ಉದ್ದಳತೆ;
        dest_ಉದ್ದಳತೆ=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.get_ಬಫರ್(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &error_ಸಂಕೇತ);
        ಹಿಂತಿರುಗಿ U_SUCCESS(error_ಸಂಕೇತ) &&
                       0!=u_strCompare(nfd.get_ಬಫರ್(), nfd.length(),
                                       dest, dest_ಉದ್ದಳತೆ, ಸುಳ್ಳು);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ UBool changesWhenNFKC_Casefolded(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &, UChar32, UProperty) {
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ UBool changesWhenNFKC_Casefolded(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    ಸ್ಥಿರಾಂಕ Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        Reordering_ಬಫರ್ ಬಫರ್(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        ಆದರೆ(ಬಫರ್.init(5, error_ಸಂಕೇತ)) {
            ಸ್ಥಿರಾಂಕ char16_t *srcArray=src.get_ಬಫರ್();
            kcf->compose(srcArray, srcArray+src.length(), ಸುಳ್ಳು,
                          ನಿಜ, ಬಫರ್, error_ಸಂಕೇತ);
        }
    }
    ಹಿಂತಿರುಗಿ U_SUCCESS(error_ಸಂಕೇತ) && dest!=src;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ UBool isCanonSegmentStarter(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &, UChar32, UProperty) {
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ UBool isCanonSegmentStarter(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    ಸ್ಥಿರಾಂಕ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ
        U_SUCCESS(error_ಸಂಕೇತ) && impl->ensure_Canon_Iter_ದತ್ತಾಂಶ(error_ಸಂಕೇತ) &&
        impl->isCanonSegmentStarter(c);
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ UBool isPOSIX_alnum(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ u_isalnumPOSIX(c);
}

ಸ್ಥಿರ UBool isPOSIX_blank(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ u_isblank(c);
}

ಸ್ಥಿರ UBool is_POSIX_ನಕ್ಷೆ(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ u_isgraphPOSIX(c);
}

ಸ್ಥಿರ UBool isPOSIX_print(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ u_isprintPOSIX(c);
}

ಸ್ಥಿರ UBool isPOSIX_xdigit(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ u_isxdigit(c);
}

ಸ್ಥಿರ UBool isRegionalIndicator(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    ಹಿಂತಿರುಗಿ 0x1F1E6<=c && c<=0x1F1FF;
}

ಸ್ಥಿರ UBool hasEmojiProperty(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty which) {
    ಹಿಂತಿರುಗಿ EmojiProps::has_ದ್ವಿಮಾನ_Property(c, which);
}

ಸ್ಥಿರ UBool isIDSUnaryOperator(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // New in Unicode 15.1 for just two characters.
    ಹಿಂತಿರುಗಿ 0x2FFE<=c && c<=0x2FFF;
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
ಸ್ಥಿರ constexpr UChar32 ID_COMPAT_MATH_ಆರಂಭ[] = {
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

ಸ್ಥಿರ UBool is_IDCompat_Math_ಆರಂಭ(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಆದರೆ (c < ID_COMPAT_MATH_ಆರಂಭ[0]) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }  // fastpath for common scripts
    ಗಾಗಿ (UChar32 ಆರಂಭ_Char : ID_COMPAT_MATH_ಆರಂಭ) {
        ಆದರೆ (c == ಆರಂಭ_Char) { ಹಿಂತಿರುಗಿ ನಿಜ; }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಸ್ಥಿರ UBool isIDCompatMathContinue(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &prop, UChar32 c, UProperty /*which*/) {
    ಗಾಗಿ (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        ಆದರೆ (c < ID_COMPAT_MATH_CONTINUE[i]) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }  // below range start
        ಆದರೆ (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { ಹಿಂತಿರುಗಿ ನಿಜ; }  // below range limit
    }
    ಹಿಂತಿರುಗಿ is_IDCompat_Math_ಆರಂಭ(prop, c, UCHAR_ID_COMPAT_MATH_ಆರಂಭ);
}

ಸ್ಥಿರ UBool isModifierCombiningMark(ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಗಾಗಿ (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        ಆದರೆ (c < MODIFIER_COMBINING_MARK[i]) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }  // below range start
        ಆದರೆ (c < MODIFIER_COMBINING_MARK[i + 1]) { ಹಿಂತಿರುಗಿ ನಿಜ; }  // below range limit
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property binProps[UCHAR_ದ್ವಿಮಾನ_ಎಲ್ಲೆ]={
    /*
     * column and mask values for binary properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per binary UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { 1,                U_ಮುಖವಾಡ(UPROPS_ALPHABETIC), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_ASCII_HEX_DIGIT), default_ಒಳಗೊಂಡಿದೆ },
    { UPROPS_SRC_BIDI,  0, isBidiControl },
    { UPROPS_SRC_BIDI,  0, isMirrored },
    { 1,                U_ಮುಖವಾಡ(UPROPS_DASH), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_DEFAULT_IGNORABLE_ಸಂಕೇತ_ಬಿಂದು), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_DEPRECATED), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_DIACRITIC), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_EXTENDER), default_ಒಳಗೊಂಡಿದೆ },
    { UPROPS_SRC_NFC,   0, has_ತುಂಬಿದ_Composition_Exclusion },
    { 1,                U_ಮುಖವಾಡ(UPROPS_GRAPHEME_ತಳ), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_GRAPHEME_EXTEND), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_GRAPHEME_LINK), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_HEX_DIGIT), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_HYPHEN), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_ID_CONTINUE), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_ID_ಆರಂಭ), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_IDEOGRAPHIC), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_IDS_ದ್ವಿಮಾನ_OPERATOR), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_IDS_TRINARY_OPERATOR), default_ಒಳಗೊಂಡಿದೆ },
    { UPROPS_SRC_BIDI,  0, isJoinControl },
    { 1,                U_ಮುಖವಾಡ(UPROPS_LOGICAL_ಕ್ರಮ_EXCEPTION), default_ಒಳಗೊಂಡಿದೆ },
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_ಒಳಗೊಂಡಿದೆ },  // UCHAR_LOWERCASE
    { 1,                U_ಮುಖವಾಡ(UPROPS_MATH), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_NONCHARACTER_ಸಂಕೇತ_ಬಿಂದು), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_QUOTATION_MARK), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_RADICAL), default_ಒಳಗೊಂಡಿದೆ },
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_ಒಳಗೊಂಡಿದೆ },  // UCHAR_SOFT_DOTTED
    { 1,                U_ಮುಖವಾಡ(UPROPS_TERMINAL_PUNCTUATION), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_UNIFIED_IDEOGRAPH), default_ಒಳಗೊಂಡಿದೆ },
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_ಒಳಗೊಂಡಿದೆ },  // UCHAR_UPPERCASE
    { 1,                U_ಮುಖವಾಡ(UPROPS_WHITE_SPACE), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_XID_CONTINUE), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_XID_ಆರಂಭ), default_ಒಳಗೊಂಡಿದೆ },
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_ಒಳಗೊಂಡಿದೆ },  // UCHAR_CASE_SENSITIVE
    { 1,                U_ಮುಖವಾಡ(UPROPS_S_ಪದ), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_VARIATION_SELECTOR), default_ಒಳಗೊಂಡಿದೆ },
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFD_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKD_INERT
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFC_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKC_INERT
    { UPROPS_SRC_NFC_CANON_ITER, 0, isCanonSegmentStarter },
    { 1,                U_ಮುಖವಾಡ(UPROPS_ನಮೂನೆ_SYNTAX), default_ಒಳಗೊಂಡಿದೆ },
    { 1,                U_ಮುಖವಾಡ(UPROPS_ನಮೂನೆ_WHITE_SPACE), default_ಒಳಗೊಂಡಿದೆ },
    { UPROPS_SRC_CHAR_AND_PROPSVEC,  0, isPOSIX_alnum },
    { UPROPS_SRC_CHAR,  0, isPOSIX_blank },
    { UPROPS_SRC_CHAR,  0, is_POSIX_ನಕ್ಷೆ },
    { UPROPS_SRC_CHAR,  0, isPOSIX_print },
    { UPROPS_SRC_CHAR,  0, isPOSIX_xdigit },
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_ಒಳಗೊಂಡಿದೆ },  // UCHAR_CASED
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_ಒಳಗೊಂಡಿದೆ },  // UCHAR_CASE_IGNORABLE
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_ಒಳಗೊಂಡಿದೆ },  // UCHAR_CHANGES_WHEN_LOWERCASED
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_ಒಳಗೊಂಡಿದೆ },  // UCHAR_CHANGES_WHEN_UPPERCASED
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_ಒಳಗೊಂಡಿದೆ },  // UCHAR_CHANGES_WHEN_TITLECASED
    { UPROPS_SRC_CASE_AND_NORM,  0, changesWhenCasefolded },
    { UPROPS_SRC_CASE,  0, case_ದ್ವಿಮಾನ_Property_ಒಳಗೊಂಡಿದೆ },  // UCHAR_CHANGES_WHEN_CASEMAPPED
    { UPROPS_SRC_NFKC_CF, 0, changesWhenNFKC_Casefolded },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_PRESENTATION
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER_BASE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_COMPONENT
    { 2,                0, isRegionalIndicator },
    { 1,                U_ಮುಖವಾಡ(UPROPS_PREPENDED_CONCATENATION_MARK), default_ಒಳಗೊಂಡಿದೆ },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EXTENDED_PICTOGRAPHIC
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_BASIC_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_KEYCAP_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_MODIFIER_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_FLAG_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_TAG_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_ZWJ_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI
    { UPROPS_SRC_IDSU, 0, isIDSUnaryOperator }, // UCHAR_IDS_UNARY_OPERATOR
    { UPROPS_SRC_ID_COMPAT_MATH, 0, is_IDCompat_Math_ಆರಂಭ }, // UCHAR_ID_COMPAT_MATH_START
    { UPROPS_SRC_ID_COMPAT_MATH, 0, isIDCompatMathContinue }, // UCHAR_ID_COMPAT_MATH_CONTINUE
    { UPROPS_SRC_MCM, 0 , isModifierCombiningMark }, // UCHAR_MODIFIER_COMBINING_MARK
};

U_CAPI UBool U_EXPORT2
u_has_ದ್ವಿಮಾನ_Property(UChar32 c, UProperty which) {
    /* c is range-checked in the functions that are called from here */
    ಆದರೆ(which<UCHAR_ದ್ವಿಮಾನ_ಆರಂಭ || UCHAR_ದ್ವಿಮಾನ_ಎಲ್ಲೆ<=which) {
        /* not a known binary property */
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &prop=binProps[which];
        ಹಿಂತಿರುಗಿ prop.ಒಳಗೊಂಡಿದೆ(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    ಹಿಂತಿರುಗಿ u_has_ದ್ವಿಮಾನ_Property(c, UCHAR_ID_ಆರಂಭ);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    ಹಿಂತಿರುಗಿ u_has_ದ್ವಿಮಾನ_Property(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_string_Has_ದ್ವಿಮಾನ_Property(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length, UProperty which) {
    ಆದರೆ (s == ಶೂನ್ಯಸೂಚಕ && length != 0) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    ಆದರೆ (length == 1) {
        ಹಿಂತಿರುಗಿ u_has_ದ್ವಿಮಾನ_Property(s[0], which);  // single code point
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_ಮುಂದಿನ(s, i, length, c);
        ಆದರೆ (length > 0 ? i == length : s[i] == 0) {
            ಹಿಂತಿರುಗಿ u_has_ದ್ವಿಮಾನ_Property(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    ಹಿಂತಿರುಗಿ UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::has_ದ್ವಿಮಾನ_Property(s, length, which);
}

ರಚನೆ IntProperty;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ int32_t Int_Property_Get_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ IntProperty &prop, UChar32 c, UProperty which);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ int32_t Int_Property_Get_Max_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ IntProperty &prop, UProperty which);

ರಚನೆ IntProperty {
    int32_t ಸ್ತಂಭ;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t ಸರಿಸು;  // =maxValue if getMaxValueFromShift() is used
    Int_Property_Get_ಮೌಲ್ಯ *get_ಮೌಲ್ಯ;
    Int_Property_Get_Max_ಮೌಲ್ಯ *get_Max_ಮೌಲ್ಯ;
};

ಸ್ಥಿರ int32_t default_Get_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(u_getUnicodeProperties(c, prop.ಸ್ತಂಭ) & prop.mask) >> prop.ಸರಿಸು;
}

ಸ್ಥಿರ int32_t default_Get_Max_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ IntProperty &prop, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ (uprv_get_Max_ಮೌಲ್ಯಗಳು(prop.ಸ್ತಂಭ)&prop.mask)>>prop.ಸರಿಸು;
}

ಸ್ಥಿರ int32_t get_Max_ಮೌಲ್ಯ_From_ಸರಿಸು(ಸ್ಥಿರಾಂಕ IntProperty &prop, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ prop.ಸರಿಸು;
}

ಸ್ಥಿರ int32_t getBiDiClass(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(u_charDirection(c));
}

ಸ್ಥಿರ int32_t getBiDiPairedBracketType(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

ಸ್ಥಿರ int32_t bi_Di_Get_Max_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UProperty which) {
    ಹಿಂತಿರುಗಿ ubidi_get_Max_ಮೌಲ್ಯ(which);
}

ಸ್ಥಿರ int32_t get_ಖಂಡ(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(ublock_get_ಸಂಕೇತ(c));
}

ಸ್ಥಿರ int32_t ಖಂಡ_Get_Max_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ uprv_get_Max_ಮೌಲ್ಯಗಳು(UPROPS_MAX_ಮೌಲ್ಯಗಳು_OTHER_ಸೂಚ್ಯಂಕ) & UPROPS_MAX_ಖಂಡ;
}

#ಆದರೆ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ int32_t getCombiningClass(ಸ್ಥಿರಾಂಕ IntProperty &, UChar32, UProperty) {
    ಹಿಂತಿರುಗಿ 0;
}
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ int32_t getCombiningClass(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ u_getCombiningClass(c);
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ int32_t getGeneralCategory(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(u_charType(c));
}

ಸ್ಥಿರ int32_t get_Joining_ಗುಂಪು(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ ubidi_get_Joining_ಗುಂಪು(c);
}

ಸ್ಥಿರ int32_t getJoiningType(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ ubidi_getJoiningType(c);
}

ಸ್ಥಿರ int32_t getNumericType(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_ಮೌಲ್ಯ(u_getMainProperties(c)));
    ಹಿಂತಿರುಗಿ UPROPS_NTV_GET_TYPE(ntv);
}

ಸ್ಥಿರ int32_t getScript(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(uscript_getScript(c, &error_ಸಂಕೇತ));
}

ಸ್ಥಿರ int32_t script_Get_Max_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ uprv_get_Max_ಮೌಲ್ಯಗಳು(0)&UPROPS_MAX_SCRIPT;
}

/*
 * Map some of the Grapheme Cluster Break values to Hangul Syllable Types.
 * Hangul_Syllable_Type used to be fully redundant with a subset of Grapheme_Cluster_Break.
 *
 * Starting with Unicode 16, this is no longer true for HST=V vs. GCB=V in some cases:
 * Some Kirat Rai vowels are given GCB=V for proper grapheme clustering, but
 * they are of course not related to Hangul syllables.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UHangulSyllableType gcb_ಗಮ್ಯ_Hst[]={
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

ಸ್ಥಿರ int32_t getHangulSyllableType(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Ignore supplementary code points: They all have HST=NA.
    // This is a simple way to handle the GCB!=hst cases since Unicode 16 (Kirat Rai vowels).
    ಆದರೆ(c>0xffff) {
        ಹಿಂತಿರುಗಿ U_HST_NOT_APPLICABLE;
    }
    /* see comments on gcbToHst[] above */
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_ಮುಖವಾಡ) >> UPROPS_GCB_ಸರಿಸು;
    ಆದರೆ(gcb<UPRV_LENGTHOF(gcb_ಗಮ್ಯ_Hst)) {
        ಹಿಂತಿರುಗಿ gcb_ಗಮ್ಯ_Hst[gcb];
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_HST_NOT_APPLICABLE;
    }
}

#ಆದರೆ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ int32_t get_Norm_ಶೀಘ್ರ_Check(ಸ್ಥಿರಾಂಕ IntProperty &, UChar32, UProperty) {
    ಹಿಂತಿರುಗಿ 0;
}
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ int32_t get_Norm_ಶೀಘ್ರ_Check(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UChar32 c, UProperty which) {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(unorm_get_ಶೀಘ್ರ_Check(c, static_cast<UNormalization_ಬಹುಲಕ>(which - UCHAR_NFD_ಶೀಘ್ರ_CHECK + UNORM_NFD)));
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ int32_t getLeadCombiningClass(ಸ್ಥಿರಾಂಕ IntProperty &, UChar32, UProperty) {
    ಹಿಂತಿರುಗಿ 0;
}
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ int32_t getLeadCombiningClass(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ unorm_getFCD16(c)>>8;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ UCONFIG_NO_NORMALIZATION
ಸ್ಥಿರ int32_t getTrailCombiningClass(ಸ್ಥಿರಾಂಕ IntProperty &, UChar32, UProperty) {
    ಹಿಂತಿರುಗಿ 0;
}
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ int32_t getTrailCombiningClass(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ unorm_getFCD16(c)&0xff;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ int32_t get_ಒಳ_PC(ಸ್ಥಿರಾಂಕ IntProperty &, UChar32 c, UProperty) {
    ಹಿಂತಿರುಗಿ ulayout_ensure_ದತ್ತಾಂಶ() && gInpcTrie != ಶೂನ್ಯಸೂಚಕ ? ucptrie_get(gInpcTrie, c) : 0;
}

ಸ್ಥಿರ int32_t get_ಒಳ_SC(ಸ್ಥಿರಾಂಕ IntProperty &, UChar32 c, UProperty) {
    ಹಿಂತಿರುಗಿ ulayout_ensure_ದತ್ತಾಂಶ() && gInscTrie != ಶೂನ್ಯಸೂಚಕ ? ucptrie_get(gInscTrie, c) : 0;
}

ಸ್ಥಿರ int32_t getVo(ಸ್ಥಿರಾಂಕ IntProperty &, UChar32 c, UProperty) {
    ಹಿಂತಿರುಗಿ ulayout_ensure_ದತ್ತಾಂಶ() && gVoTrie != ಶೂನ್ಯಸೂಚಕ ? ucptrie_get(gVoTrie, c) : 0;
}

ಸ್ಥಿರ int32_t layout_Get_Max_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ IntProperty &/*prop*/, UProperty which) {
    ಆದರೆ (!ulayout_ensure_ದತ್ತಾಂಶ()) { ಹಿಂತಿರುಗಿ 0; }
    ಆಯ್ಕೆ (which) {
    ಸ್ಥಿತಿ UCHAR_INDIC_POSITIONAL_CATEGORY:
        ಹಿಂತಿರುಗಿ g_Max_Inpc_ಮೌಲ್ಯ;
    ಸ್ಥಿತಿ UCHAR_INDIC_SYLLABIC_CATEGORY:
        ಹಿಂತಿರುಗಿ g_Max_Insc_ಮೌಲ್ಯ;
    ಸ್ಥಿತಿ UCHAR_VERTICAL_ORIENTATION:
        ಹಿಂತಿರುಗಿ g_Max_Vo_ಮೌಲ್ಯ;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ 0;
    }
}

ಸ್ಥಿರ int32_t get_IDStatus_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_ಸರಿಸು;
    ಹಿಂತಿರುಗಿ value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_ಸ್ಥಿತಿ_ALLOWED : U_ID_ಸ್ಥಿತಿ_RESTRICTED;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ IntProperty intProps[UCHAR_INT_ಎಲ್ಲೆ-UCHAR_INT_ಆರಂಭ]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, bi_Di_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_ಖಂಡ, 0, 0,                               get_ಖಂಡ, ಖಂಡ_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, get_Max_ಮೌಲ್ಯ_From_ಸರಿಸು },
    { 2,                UPROPS_DT_ಮುಖವಾಡ, 0,                  default_Get_ಮೌಲ್ಯ, default_Get_Max_ಮೌಲ್ಯ },
    { 0,                UPROPS_EA_ಮುಖವಾಡ, UPROPS_EA_ಸರಿಸು,    default_Get_ಮೌಲ್ಯ, default_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_ಎಣಿಕೆ) - 1, getGeneralCategory, get_Max_ಮೌಲ್ಯ_From_ಸರಿಸು },
    { UPROPS_SRC_BIDI,  0, 0,                               get_Joining_ಗುಂಪು, bi_Di_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, bi_Di_Get_Max_ಮೌಲ್ಯ },
    { 2,                UPROPS_LB_ಮುಖವಾಡ, UPROPS_LB_ಸರಿಸು,    default_Get_ಮೌಲ್ಯ, default_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_ಎಣಿಕೆ) - 1, getNumericType, get_Max_ಮೌಲ್ಯ_From_ಸರಿಸು },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, script_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_ಎಣಿಕೆ) - 1, getHangulSyllableType, get_Max_ಮೌಲ್ಯ_From_ಸರಿಸು },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), get_Norm_ಶೀಘ್ರ_Check, get_Max_ಮೌಲ್ಯ_From_ಸರಿಸು },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), get_Norm_ಶೀಘ್ರ_Check, get_Max_ಮೌಲ್ಯ_From_ಸರಿಸು },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), get_Norm_ಶೀಘ್ರ_Check, get_Max_ಮೌಲ್ಯ_From_ಸರಿಸು },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), get_Norm_ಶೀಘ್ರ_Check, get_Max_ಮೌಲ್ಯ_From_ಸರಿಸು },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, get_Max_ಮೌಲ್ಯ_From_ಸರಿಸು },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, get_Max_ಮೌಲ್ಯ_From_ಸರಿಸು },
    { 2,                UPROPS_GCB_ಮುಖವಾಡ, UPROPS_GCB_ಸರಿಸು,  default_Get_ಮೌಲ್ಯ, default_Get_Max_ಮೌಲ್ಯ },
    { 2,                UPROPS_SB_ಮುಖವಾಡ, UPROPS_SB_ಸರಿಸು,    default_Get_ಮೌಲ್ಯ, default_Get_Max_ಮೌಲ್ಯ },
    { 2,                UPROPS_WB_ಮುಖವಾಡ, UPROPS_WB_ಸರಿಸು,    default_Get_ಮೌಲ್ಯ, default_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, bi_Di_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_INPC,  0, 0,                               get_ಒಳ_PC, layout_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_INSC,  0, 0,                               get_ಒಳ_SC, layout_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layout_Get_Max_ಮೌಲ್ಯ },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_ಸ್ಥಿತಿ_ALLOWED), get_IDStatus_ಮೌಲ್ಯ, get_Max_ಮೌಲ್ಯ_From_ಸರಿಸು },
    { 0,                UPROPS_INCB_ಮುಖವಾಡ, UPROPS_INCB_ಸರಿಸು,default_Get_ಮೌಲ್ಯ, default_Get_Max_ಮೌಲ್ಯ },
};

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_ಮೌಲ್ಯ(UChar32 c, UProperty which) {
    ಆದರೆ(which<UCHAR_INT_ಆರಂಭ) {
        ಆದರೆ(UCHAR_ದ್ವಿಮಾನ_ಆರಂಭ<=which && which<UCHAR_ದ್ವಿಮಾನ_ಎಲ್ಲೆ) {
            ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &prop=binProps[which];
            ಹಿಂತಿರುಗಿ prop.ಒಳಗೊಂಡಿದೆ(prop, c, which);
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(which<UCHAR_INT_ಎಲ್ಲೆ) {
        ಸ್ಥಿರಾಂಕ IntProperty &prop=intProps[which-UCHAR_INT_ಆರಂಭ];
        ಹಿಂತಿರುಗಿ prop.get_ಮೌಲ್ಯ(prop, c, which);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(which==UCHAR_GENERAL_CATEGORY_ಮುಖವಾಡ) {
        ಹಿಂತಿರುಗಿ U_ಮುಖವಾಡ(u_charType(c));
    }
    ಹಿಂತಿರುಗಿ 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Min_ಮೌಲ್ಯ(UProperty /*which*/) {
    ಹಿಂತಿರುಗಿ 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Max_ಮೌಲ್ಯ(UProperty which) {
    ಆದರೆ(which<UCHAR_INT_ಆರಂಭ) {
        ಆದರೆ(UCHAR_ದ್ವಿಮಾನ_ಆರಂಭ<=which && which<UCHAR_ದ್ವಿಮಾನ_ಎಲ್ಲೆ) {
            ಹಿಂತಿರುಗಿ 1;  // maximum true for all binary properties
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(which<UCHAR_INT_ಎಲ್ಲೆ) {
        ಸ್ಥಿರಾಂಕ IntProperty &prop=intProps[which-UCHAR_INT_ಆರಂಭ];
        ಹಿಂತಿರುಗಿ prop.get_Max_ಮೌಲ್ಯ(prop, which);
    }
    ಹಿಂತಿರುಗಿ -1;  // undefined
}

U_CFUNC UProperty_ಮೂಲ U_EXPORT2
uprops_get_ಮೂಲ(UProperty which) {
    ಆದರೆ(which<UCHAR_ದ್ವಿಮಾನ_ಆರಂಭ) {
        ಹಿಂತಿರುಗಿ UPROPS_SRC_NONE; /* undefined */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(which<UCHAR_ದ್ವಿಮಾನ_ಎಲ್ಲೆ) {
        ಸ್ಥಿರಾಂಕ ದ್ವಿಮಾನ_Property &prop=binProps[which];
        ಆದರೆ(prop.mask!=0) {
            ಹಿಂತಿರುಗಿ UPROPS_SRC_PROPSVEC;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ (UProperty_ಮೂಲ)prop.ಸ್ತಂಭ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(which<UCHAR_INT_ಆರಂಭ) {
        ಹಿಂತಿರುಗಿ UPROPS_SRC_NONE; /* undefined */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(which<UCHAR_INT_ಎಲ್ಲೆ) {
        ಸ್ಥಿರಾಂಕ IntProperty &prop=intProps[which-UCHAR_INT_ಆರಂಭ];
        ಆದರೆ(prop.mask!=0) {
            ಹಿಂತಿರುಗಿ UPROPS_SRC_PROPSVEC;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ (UProperty_ಮೂಲ)prop.ಸ್ತಂಭ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(which<UCHAR_STRING_ಆರಂಭ) {
        ಆಯ್ಕೆ(which) {
        ಸ್ಥಿತಿ UCHAR_GENERAL_CATEGORY_ಮುಖವಾಡ:
        ಸ್ಥಿತಿ UCHAR_NUMERIC_ಮೌಲ್ಯ:
            ಹಿಂತಿರುಗಿ UPROPS_SRC_CHAR;

        ಪೂರ್ವನಿಯೋಜಿತ:
            ಹಿಂತಿರುಗಿ UPROPS_SRC_NONE;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(which<UCHAR_STRING_ಎಲ್ಲೆ) {
        ಆಯ್ಕೆ(which) {
        ಸ್ಥಿತಿ UCHAR_AGE:
            ಹಿಂತಿರುಗಿ UPROPS_SRC_PROPSVEC;

        ಸ್ಥಿತಿ UCHAR_BIDI_MIRRORING_GLYPH:
            ಹಿಂತಿರುಗಿ UPROPS_SRC_BIDI;

        ಸ್ಥಿತಿ UCHAR_CASE_FOLDING:
        ಸ್ಥಿತಿ UCHAR_LOWERCASE_MAPPING:
        ಸ್ಥಿತಿ UCHAR_SIMPLE_CASE_FOLDING:
        ಸ್ಥಿತಿ UCHAR_SIMPLE_LOWERCASE_MAPPING:
        ಸ್ಥಿತಿ UCHAR_SIMPLE_TITLECASE_MAPPING:
        ಸ್ಥಿತಿ UCHAR_SIMPLE_UPPERCASE_MAPPING:
        ಸ್ಥಿತಿ UCHAR_TITLECASE_MAPPING:
        ಸ್ಥಿತಿ UCHAR_UPPERCASE_MAPPING:
            ಹಿಂತಿರುಗಿ UPROPS_SRC_CASE;

        ಸ್ಥಿತಿ UCHAR_ISO_COMMENT:
        ಸ್ಥಿತಿ UCHAR_NAME:
        ಸ್ಥಿತಿ UCHAR_UNICODE_1_NAME:
            ಹಿಂತಿರುಗಿ UPROPS_SRC_ಹೆಸರುಗಳು;

        ಪೂರ್ವನಿಯೋಜಿತ:
            ಹಿಂತಿರುಗಿ UPROPS_SRC_NONE;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆಯ್ಕೆ(which) {
        ಸ್ಥಿತಿ UCHAR_SCRIPT_EXTENSIONS:
        ಸ್ಥಿತಿ UCHAR_IDENTIFIER_TYPE:
            ಹಿಂತಿರುಗಿ UPROPS_SRC_PROPSVEC;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಹಿಂತಿರುಗಿ UPROPS_SRC_NONE; /* undefined */
        }
    }
}

U_CFUNC ಶೂನ್ಯ U_EXPORT2
uprops_ಕೂಡಿಸು_Property_Starts(UProperty_ಮೂಲ src, ಸ್ಥಿರಾಂಕ USetAdder *sa, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    ಆದರೆ (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        ಗಾಗಿ (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->ಕೂಡಿಸು(sa->ಗಣ, c);
        }
        // single characters
        ಗಾಗಿ (UChar32 c : ID_COMPAT_MATH_ಆರಂಭ) {
            sa->ಕೂಡಿಸು(sa->ಗಣ, c);
            sa->ಕೂಡಿಸು(sa->ಗಣ, c + 1);
        }
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (src == UPROPS_SRC_MCM) {
        // range limits
        ಗಾಗಿ (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->ಕೂಡಿಸು(sa->ಗಣ, c);
        }
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (!ulayout_ensure_ದತ್ತಾಂಶ(*p_Error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    ಸ್ಥಿರಾಂಕ UCPTrie *trie;
    ಆಯ್ಕೆ (src) {
    ಸ್ಥಿತಿ UPROPS_SRC_INPC:
        trie = gInpcTrie;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UPROPS_SRC_INSC:
        trie = gInscTrie;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UPROPS_SRC_VO:
        trie = gVoTrie;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (trie == ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ = U_MISSING_RESOURCE_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 ಆರಂಭ = 0, ಕೊನೆ;
    ಆಗ ((ಕೊನೆ = ucptrie_getRange(trie, ಆರಂಭ, UCPMAP_RANGE_ಸಾಮಾನ್ಯ, 0,
                                   ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ)) >= 0) {
        sa->ಕೂಡಿಸು(sa->ಗಣ, ಆರಂಭ);
        ಆರಂಭ = ಕೊನೆ + 1;
    }
}

U_CAPI ತಾರ್ಕಿಕ U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t type_ಸೂಚ್ಯಂಕ = type;  // also guards against negative type integers
    ಆದರೆ (type_ಸೂಚ್ಯಂಕ >= UPRV_LENGTHOF(uprops_id_Type_ಗಮ್ಯ_Encoded)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    uint32_t encodedType = uprops_id_Type_ಗಮ್ಯ_Encoded[type_ಸೂಚ್ಯಂಕ];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_ಸರಿಸು;
    ಆದರೆ ((encodedType & UPROPS_ID_TYPE_ಬಿಟ್) != 0) {
        ಹಿಂತಿರುಗಿ value < UPROPS_ID_TYPE_FORBIDDEN && (value & encodedType) != 0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ value == encodedType;
    }
}

ನಾಮಾಕಾಶ {

ಶೂನ್ಯ maybeAppendType(uint32_t value, uint32_t bit, UIdentifierType t,
                     UIdentifierType *types, int32_t &length, int32_t capacity) {
    ಆದರೆ ((value & bit) != 0) {
        ಆದರೆ (length < capacity) {
            types[length] = t;
        }
        ++length;
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ 0; }
    ಆದರೆ (capacity < 0 || (capacity > 0 && types == ಶೂನ್ಯಸೂಚಕ)) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_ಸರಿಸು;
    ಆದರೆ ((value & UPROPS_ID_TYPE_FORBIDDEN) == UPROPS_ID_TYPE_FORBIDDEN ||
            value == UPROPS_ID_TYPE_NOT_CHARACTER) {
        // single value
        ಆದರೆ (capacity > 0) {
            UIdentifierType t;
            ಆಯ್ಕೆ (value) {
                ಸ್ಥಿತಿ UPROPS_ID_TYPE_NOT_CHARACTER: t = U_ID_TYPE_NOT_CHARACTER; ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ UPROPS_ID_TYPE_DEPRECATED: t = U_ID_TYPE_DEPRECATED; ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ UPROPS_ID_TYPE_DEFAULT_IGNORABLE: t = U_ID_TYPE_DEFAULT_IGNORABLE; ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ UPROPS_ID_TYPE_NOT_NFKC: t = U_ID_TYPE_NOT_NFKC; ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ UPROPS_ID_TYPE_INCLUSION: t = U_ID_TYPE_INCLUSION; ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ UPROPS_ID_TYPE_RECOMMENDED: t = U_ID_TYPE_RECOMMENDED; ನಿಲ್ಲಿಸು;
                ಪೂರ್ವನಿಯೋಜಿತ:
                    *p_Error_ಸಂಕೇತ = U_INVALID_FORMAT_ERROR;
                    ಹಿಂತಿರುಗಿ 0;
            }
            types[0] = t;
        } ಇಲ್ಲದಿದ್ದರೆ {
            *p_Error_ಸಂಕೇತ = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        }
        ಹಿಂತಿರುಗಿ 1;
    } ಇಲ್ಲದಿದ್ದರೆ {
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
        ಆದರೆ (length >= capacity) {
            *p_Error_ಸಂಕೇತ = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        }
        ಹಿಂತಿರುಗಿ length;
    }
}

#ಆದರೆ !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_get_FC_NFKC_ಆವರಣ(UChar32 c, char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ<0 || (dest==ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ>0)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    ಸ್ಥಿರಾಂಕ Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    ಸ್ಥಿರಾಂಕ char16_t *folded1;
    int32_t folded1_ಉದ್ದಳತೆ=ucase_ಗಮ್ಯ_ತುಂಬಿದ_Folding(c, &folded1, U_FOLD_CASE_DEFAULT);
    ಆದರೆ(folded1_ಉದ್ದಳತೆ<0) {
        ಸ್ಥಿರಾಂಕ Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        ಆದರೆ(nfkcImpl->get_Comp_ಶೀಘ್ರ_Check(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, 0, p_Error_ಸಂಕೇತ);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.ಗಣ_ಗಮ್ಯ(c);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(folded1_ಉದ್ದಳತೆ>UCASE_MAX_STRING_ಉದ್ದಳತೆ) {
            folded1String.ಗಣ_ಗಮ್ಯ(folded1_ಉದ್ದಳತೆ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            folded1String.ಗಣ_ಗಮ್ಯ(ಸುಳ್ಳು, folded1, folded1_ಉದ್ದಳತೆ);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *p_Error_ಸಂಕೇತ);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *p_Error_ಸಂಕೇತ);
    // if (c != b) add the mapping from a to c
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ) || kc1==kc2) {
        ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, 0, p_Error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ kc2.extract(dest, dest_ಸಾಮರ್ಥ್ಯ, *p_Error_ಸಂಕೇತ);
    }
}

#ಆದರೆ_ಅಂತ್ಯ
