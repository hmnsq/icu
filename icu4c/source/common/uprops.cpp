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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unorm2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uscript.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "emojiprops.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "normalizer2impl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubidi_props.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uprops.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ulayout_props.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

U_NAMESPACE_USE

// Unicode text layout properties data -----------------------------------------

ਨਾਮਾਕਾਸ਼ {

icu::UInitOnce gLayoutInitOnce {};
UData_ਸਮ੍ਰਿਤੀ *g_Layout_ਸਮ੍ਰਿਤੀ = ਖਾਲੀ_ਸੂਚਕ;

UCPTrie *gInpcTrie = ਖਾਲੀ_ਸੂਚਕ;  // Indic_Positional_Category
UCPTrie *gInscTrie = ਖਾਲੀ_ਸੂਚਕ;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = ਖਾਲੀ_ਸੂਚਕ;  // Vertical_Orientation

int32_t g_Max_Inpc_ਮੁੱਲ = 0;
int32_t g_Max_Insc_ਮੁੱਲ = 0;
int32_t g_Max_Vo_ਮੁੱਲ = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(g_Layout_ਸਮ੍ਰਿਤੀ);
    g_Layout_ਸਮ੍ਰਿਤੀ = ਖਾਲੀ_ਸੂਚਕ;

    ucptrie_close(gInpcTrie);
    gInpcTrie = ਖਾਲੀ_ਸੂਚਕ;
    ucptrie_close(gInscTrie);
    gInscTrie = ਖਾਲੀ_ਸੂਚਕ;
    ucptrie_close(gVoTrie);
    gVoTrie = ਖਾਲੀ_ਸੂਚਕ;

    g_Max_Inpc_ਮੁੱਲ = 0;
    g_Max_Insc_ਮੁੱਲ = 0;
    g_Max_Vo_ਮੁੱਲ = 0;

    gLayoutInitOnce.reset();
    ਵਾਪਸ ਸੱਚ;
}

UBool U_CALLCONV
ulayout_isAcceptable(ਖਾਲੀ * /*context*/,
                     ਅਚਲ ਅੱਖਰ * /* type */, ਅਚਲ ਅੱਖਰ * /*name*/,
                     ਅਚਲ UDataInfo *pInfo) {
    ਵਾਪਸ pInfo->size >= 20 &&
        pInfo->isBigEndian == U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily == U_CHARSET_FAMILY &&
        pInfo->ਅੰਕੜੇ_Format[0] == ULAYOUT_FMT_0 &&
        pInfo->ਅੰਕੜੇ_Format[1] == ULAYOUT_FMT_1 &&
        pInfo->ਅੰਕੜੇ_Format[2] == ULAYOUT_FMT_2 &&
        pInfo->ਅੰਕੜੇ_Format[3] == ULAYOUT_FMT_3 &&
        pInfo->format_ਸੰਸਕਰਣ[0] == 1;
}

// UInitOnce singleton initialization function
ਖਾਲੀ U_CALLCONV ulayout_ਲੱਦੋ(UError_ਕੋਡ &error_ਕੋਡ) {
    g_Layout_ਸਮ੍ਰਿਤੀ = udata_openChoice(
        ਖਾਲੀ_ਸੂਚਕ, ULAYOUT_ਅੰਕੜੇ_TYPE, ULAYOUT_ਅੰਕੜੇ_NAME,
        ulayout_isAcceptable, ਖਾਲੀ_ਸੂਚਕ, &error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }

    ਅਚਲ uint8_t* ਵਿੱਚ_ਬਾਈਟਾਂ = static_cast<ਅਚਲ uint8_t*>(udata_get_ਸਮ੍ਰਿਤੀ(g_Layout_ਸਮ੍ਰਿਤੀ));
    ਅਚਲ int32_t* ਵਿੱਚ_Indexes = reinterpret_cast<ਅਚਲ int32_t*>(ਵਿੱਚ_ਬਾਈਟਾਂ);
    int32_t indexes_ਲੰਮਾਈ = ਵਿੱਚ_Indexes[ULAYOUT_IX_INDEXES_ਲੰਮਾਈ];
    ਜੇ (indexes_ਲੰਮਾਈ < 12) {
        error_ਕੋਡ = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ਵਾਪਸ;
    }
    int32_t ਵਿੱਥ = indexes_ਲੰਮਾਈ * 4;
    int32_t ਸਿਖਰ = ਵਿੱਚ_Indexes[ULAYOUT_IX_INPC_TRIE_ਸਿਖਰ];
    int32_t trie_ਮਾਪ = ਸਿਖਰ - ਵਿੱਥ;
    ਜੇ (trie_ਮਾਪ >= 16) {
        gInpcTrie = ucptrie_open_From_ਦੁਆਧਾਰੀ(
            UCPTRIE_TYPE_ANY, UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_ANY,
            ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, trie_ਮਾਪ, ਖਾਲੀ_ਸੂਚਕ, &error_ਕੋਡ);
    }
    ਵਿੱਥ = ਸਿਖਰ;
    ਸਿਖਰ = ਵਿੱਚ_Indexes[ULAYOUT_IX_INSC_TRIE_ਸਿਖਰ];
    trie_ਮਾਪ = ਸਿਖਰ - ਵਿੱਥ;
    ਜੇ (trie_ਮਾਪ >= 16) {
        gInscTrie = ucptrie_open_From_ਦੁਆਧਾਰੀ(
            UCPTRIE_TYPE_ANY, UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_ANY,
            ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, trie_ਮਾਪ, ਖਾਲੀ_ਸੂਚਕ, &error_ਕੋਡ);
    }
    ਵਿੱਥ = ਸਿਖਰ;
    ਸਿਖਰ = ਵਿੱਚ_Indexes[ULAYOUT_IX_VO_TRIE_ਸਿਖਰ];
    trie_ਮਾਪ = ਸਿਖਰ - ਵਿੱਥ;
    ਜੇ (trie_ਮਾਪ >= 16) {
        gVoTrie = ucptrie_open_From_ਦੁਆਧਾਰੀ(
            UCPTRIE_TYPE_ANY, UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_ANY,
            ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, trie_ਮਾਪ, ਖਾਲੀ_ਸੂਚਕ, &error_ਕੋਡ);
    }

    uint32_t max_ਮੁੱਲਾਂ = ਵਿੱਚ_Indexes[ULAYOUT_IX_MAX_ਮੁੱਲਾਂ];
    g_Max_Inpc_ਮੁੱਲ = max_ਮੁੱਲਾਂ >> ULAYOUT_MAX_INPC_ਖਿਸਕਾਓ;
    g_Max_Insc_ਮੁੱਲ = (max_ਮੁੱਲਾਂ >> ULAYOUT_MAX_INSC_ਖਿਸਕਾਓ) & 0xff;
    g_Max_Vo_ਮੁੱਲ = (max_ਮੁੱਲਾਂ >> ULAYOUT_MAX_VO_ਖਿਸਕਾਓ) & 0xff;

    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensure_ਅੰਕੜੇ(UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_ਲੱਦੋ, error_ਕੋਡ);
    ਵਾਪਸ U_SUCCESS(error_ਕੋਡ);
}

UBool ulayout_ensure_ਅੰਕੜੇ() {
    UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;
    ਵਾਪਸ ulayout_ensure_ਅੰਕੜੇ(error_ਕੋਡ);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

ਬਣਤਰ ਦੁਆਧਾਰੀ_Property;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ UBool ਦੁਆਧਾਰੀ_Property_ਸ਼ਾਮਲ_ਹੈ(ਅਚਲ ਦੁਆਧਾਰੀ_Property &prop, UChar32 c, UProperty which);

ਬਣਤਰ ਦੁਆਧਾਰੀ_Property {
    int32_t ਕਾਲਮ;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    ਦੁਆਧਾਰੀ_Property_ਸ਼ਾਮਲ_ਹੈ *ਸ਼ਾਮਲ_ਹੈ;
};

ਸਥਿਰ UBool default_ਸ਼ਾਮਲ_ਹੈ(ਅਚਲ ਦੁਆਧਾਰੀ_Property &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    ਵਾਪਸ (u_getUnicodeProperties(c, prop.ਕਾਲਮ)&prop.mask)!=0;
}

ਸਥਿਰ UBool case_ਦੁਆਧਾਰੀ_Property_ਸ਼ਾਮਲ_ਹੈ(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty which) {
    ਵਾਪਸ ucase_has_ਦੁਆਧਾਰੀ_Property(c, which);
}

ਸਥਿਰ UBool isBidiControl(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ ubidi_isBidiControl(c);
}

ਸਥਿਰ UBool isMirrored(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ ubidi_isMirrored(c);
}

ਸਥਿਰ UBool isJoinControl(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ ubidi_isJoinControl(c);
}

#ਜੇ UCONFIG_NO_NORMALIZATION
ਸਥਿਰ UBool has_ਭਰਿਆ_Composition_Exclusion(ਅਚਲ ਦੁਆਧਾਰੀ_Property &, UChar32, UProperty) {
    ਵਾਪਸ ਝੂਠ;
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ UBool has_ਭਰਿਆ_Composition_Exclusion(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    ਅਚਲ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_ਕੋਡ);
    ਵਾਪਸ U_SUCCESS(error_ਕੋਡ) && impl->isCompNo(impl->getNorm16(c));
}
#ਜੇ_ਅੰਤ

// UCHAR_NF*_INERT properties
#ਜੇ UCONFIG_NO_NORMALIZATION
ਸਥਿਰ UBool isNormInert(ਅਚਲ ਦੁਆਧਾਰੀ_Property &, UChar32, UProperty) {
    ਵਾਪਸ ਝੂਠ;
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ UBool isNormInert(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty which) {
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    ਅਚਲ Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalization_ਬਹੁਲਕ>(which - UCHAR_NFD_INERT + UNORM_NFD), error_ਕੋਡ);
    ਵਾਪਸ U_SUCCESS(error_ਕੋਡ) && norm2->isInert(c);
}
#ਜੇ_ਅੰਤ

#ਜੇ UCONFIG_NO_NORMALIZATION
ਸਥਿਰ UBool changesWhenCasefolded(ਅਚਲ ਦੁਆਧਾਰੀ_Property &, UChar32, UProperty) {
    ਵਾਪਸ ਝੂਠ;
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ UBool changesWhenCasefolded(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    ਅਚਲ Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(error_ਕੋਡ);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        ਜੇ(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } ਨਹੀਂਤਾਂ ਜੇ(nfd.length()<=U16_MAX_ਲੰਮਾਈ &&
                  nfd.length()==U16_ਲੰਮਾਈ(c=nfd.char32At(0))
        ) {
            /* single supplementary code point */
        } ਨਹੀਂਤਾਂ {
            c=U_SENTINEL;
        }
    } ਨਹੀਂਤਾਂ ਜੇ(c<0) {
        ਵਾਪਸ ਝੂਠ;  /* protect against bad input */
    }
    ਜੇ(c>=0) {
        /* single code point */
        ਅਚਲ char16_t *ਨਤੀਜਾ_String;
        ਵਾਪਸ ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_Folding(c, &ਨਤੀਜਾ_String, U_FOLD_CASE_DEFAULT) >= 0;
    } ਨਹੀਂਤਾਂ {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_ਲੰਮਾਈ];
        int32_t dest_ਲੰਮਾਈ;
        dest_ਲੰਮਾਈ=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.get_ਬਫਰ(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &error_ਕੋਡ);
        ਵਾਪਸ U_SUCCESS(error_ਕੋਡ) &&
                       0!=u_strCompare(nfd.get_ਬਫਰ(), nfd.length(),
                                       dest, dest_ਲੰਮਾਈ, ਝੂਠ);
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ UCONFIG_NO_NORMALIZATION
ਸਥਿਰ UBool changesWhenNFKC_Casefolded(ਅਚਲ ਦੁਆਧਾਰੀ_Property &, UChar32, UProperty) {
    ਵਾਪਸ ਝੂਠ;
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ UBool changesWhenNFKC_Casefolded(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    ਅਚਲ Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(error_ਕੋਡ);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        Reordering_ਬਫਰ ਬਫਰ(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        ਜੇ(ਬਫਰ.init(5, error_ਕੋਡ)) {
            ਅਚਲ char16_t *srcArray=src.get_ਬਫਰ();
            kcf->compose(srcArray, srcArray+src.length(), ਝੂਠ,
                          ਸੱਚ, ਬਫਰ, error_ਕੋਡ);
        }
    }
    ਵਾਪਸ U_SUCCESS(error_ਕੋਡ) && dest!=src;
}
#ਜੇ_ਅੰਤ

#ਜੇ UCONFIG_NO_NORMALIZATION
ਸਥਿਰ UBool isCanonSegmentStarter(ਅਚਲ ਦੁਆਧਾਰੀ_Property &, UChar32, UProperty) {
    ਵਾਪਸ ਝੂਠ;
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ UBool isCanonSegmentStarter(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    ਅਚਲ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_ਕੋਡ);
    ਵਾਪਸ
        U_SUCCESS(error_ਕੋਡ) && impl->ensure_Canon_Iter_ਅੰਕੜੇ(error_ਕੋਡ) &&
        impl->isCanonSegmentStarter(c);
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ UBool isPOSIX_alnum(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ u_isalnumPOSIX(c);
}

ਸਥਿਰ UBool isPOSIX_blank(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ u_isblank(c);
}

ਸਥਿਰ UBool is_POSIX_ਗ੍ਰਾਫ(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ u_isgraphPOSIX(c);
}

ਸਥਿਰ UBool isPOSIX_print(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ u_isprintPOSIX(c);
}

ਸਥਿਰ UBool isPOSIX_xdigit(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ u_isxdigit(c);
}

ਸਥਿਰ UBool isRegionalIndicator(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    ਵਾਪਸ 0x1F1E6<=c && c<=0x1F1FF;
}

ਸਥਿਰ UBool hasEmojiProperty(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty which) {
    ਵਾਪਸ EmojiProps::has_ਦੁਆਧਾਰੀ_Property(c, which);
}

ਸਥਿਰ UBool isIDSUnaryOperator(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // New in Unicode 15.1 for just two characters.
    ਵਾਪਸ 0x2FFE<=c && c<=0x2FFF;
}

/** Ranges (start/limit pairs) of ID_Compat_Math_Continue (only), from UCD PropList.txt. */
ਸਥਿਰ constexpr UChar32 ID_COMPAT_MATH_CONTINUE[] = {
    0x00B2, 0x00B3 + 1,
    0x00B9, 0x00B9 + 1,
    0x2070, 0x2070 + 1,
    0x2074, 0x207E + 1,
    0x2080, 0x208E + 1
};

/** ID_Compat_Math_Start characters, from UCD PropList.txt. */
ਸਥਿਰ constexpr UChar32 ID_COMPAT_MATH_ਸ਼ੁਰੂ[] = {
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
ਸਥਿਰ constexpr UChar32 MODIFIER_COMBINING_MARK[] = {
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

ਸਥਿਰ UBool is_IDCompat_Math_ਸ਼ੁਰੂ(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਜੇ (c < ID_COMPAT_MATH_ਸ਼ੁਰੂ[0]) { ਵਾਪਸ ਝੂਠ; }  // fastpath for common scripts
    ਲਈ (UChar32 ਸ਼ੁਰੂ_Char : ID_COMPAT_MATH_ਸ਼ੁਰੂ) {
        ਜੇ (c == ਸ਼ੁਰੂ_Char) { ਵਾਪਸ ਸੱਚ; }
    }
    ਵਾਪਸ ਝੂਠ;
}

ਸਥਿਰ UBool isIDCompatMathContinue(ਅਚਲ ਦੁਆਧਾਰੀ_Property &prop, UChar32 c, UProperty /*which*/) {
    ਲਈ (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        ਜੇ (c < ID_COMPAT_MATH_CONTINUE[i]) { ਵਾਪਸ ਝੂਠ; }  // below range start
        ਜੇ (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { ਵਾਪਸ ਸੱਚ; }  // below range limit
    }
    ਵਾਪਸ is_IDCompat_Math_ਸ਼ੁਰੂ(prop, c, UCHAR_ID_COMPAT_MATH_ਸ਼ੁਰੂ);
}

ਸਥਿਰ UBool isModifierCombiningMark(ਅਚਲ ਦੁਆਧਾਰੀ_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਲਈ (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        ਜੇ (c < MODIFIER_COMBINING_MARK[i]) { ਵਾਪਸ ਝੂਠ; }  // below range start
        ਜੇ (c < MODIFIER_COMBINING_MARK[i + 1]) { ਵਾਪਸ ਸੱਚ; }  // below range limit
    }
    ਵਾਪਸ ਝੂਠ;
}

ਸਥਿਰ ਅਚਲ ਦੁਆਧਾਰੀ_Property binProps[UCHAR_ਦੁਆਧਾਰੀ_ਹੱਦਬੰਦੀ]={
    /*
     * column and mask values for binary properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per binary UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { 1,                U_ਨਕਾਬ(UPROPS_ALPHABETIC), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_ASCII_HEX_DIGIT), default_ਸ਼ਾਮਲ_ਹੈ },
    { UPROPS_SRC_BIDI,  0, isBidiControl },
    { UPROPS_SRC_BIDI,  0, isMirrored },
    { 1,                U_ਨਕਾਬ(UPROPS_DASH), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_DEFAULT_IGNORABLE_ਕੋਡ_ਬਿੰਦੂ), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_DEPRECATED), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_DIACRITIC), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_EXTENDER), default_ਸ਼ਾਮਲ_ਹੈ },
    { UPROPS_SRC_NFC,   0, has_ਭਰਿਆ_Composition_Exclusion },
    { 1,                U_ਨਕਾਬ(UPROPS_GRAPHEME_ਅਧਾਰ), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_GRAPHEME_EXTEND), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_GRAPHEME_LINK), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_HEX_DIGIT), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_HYPHEN), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_ID_CONTINUE), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_ID_ਸ਼ੁਰੂ), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_IDEOGRAPHIC), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_IDS_ਦੁਆਧਾਰੀ_OPERATOR), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_IDS_TRINARY_OPERATOR), default_ਸ਼ਾਮਲ_ਹੈ },
    { UPROPS_SRC_BIDI,  0, isJoinControl },
    { 1,                U_ਨਕਾਬ(UPROPS_LOGICAL_ਕ੍ਰਮ_EXCEPTION), default_ਸ਼ਾਮਲ_ਹੈ },
    { UPROPS_SRC_CASE,  0, case_ਦੁਆਧਾਰੀ_Property_ਸ਼ਾਮਲ_ਹੈ },  // UCHAR_LOWERCASE
    { 1,                U_ਨਕਾਬ(UPROPS_MATH), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_NONCHARACTER_ਕੋਡ_ਬਿੰਦੂ), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_QUOTATION_MARK), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_RADICAL), default_ਸ਼ਾਮਲ_ਹੈ },
    { UPROPS_SRC_CASE,  0, case_ਦੁਆਧਾਰੀ_Property_ਸ਼ਾਮਲ_ਹੈ },  // UCHAR_SOFT_DOTTED
    { 1,                U_ਨਕਾਬ(UPROPS_TERMINAL_PUNCTUATION), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_UNIFIED_IDEOGRAPH), default_ਸ਼ਾਮਲ_ਹੈ },
    { UPROPS_SRC_CASE,  0, case_ਦੁਆਧਾਰੀ_Property_ਸ਼ਾਮਲ_ਹੈ },  // UCHAR_UPPERCASE
    { 1,                U_ਨਕਾਬ(UPROPS_WHITE_SPACE), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_XID_CONTINUE), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_XID_ਸ਼ੁਰੂ), default_ਸ਼ਾਮਲ_ਹੈ },
    { UPROPS_SRC_CASE,  0, case_ਦੁਆਧਾਰੀ_Property_ਸ਼ਾਮਲ_ਹੈ },  // UCHAR_CASE_SENSITIVE
    { 1,                U_ਨਕਾਬ(UPROPS_S_ਪਦ), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_VARIATION_SELECTOR), default_ਸ਼ਾਮਲ_ਹੈ },
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFD_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKD_INERT
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFC_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKC_INERT
    { UPROPS_SRC_NFC_CANON_ITER, 0, isCanonSegmentStarter },
    { 1,                U_ਨਕਾਬ(UPROPS_ਨਮੂਨਾ_SYNTAX), default_ਸ਼ਾਮਲ_ਹੈ },
    { 1,                U_ਨਕਾਬ(UPROPS_ਨਮੂਨਾ_WHITE_SPACE), default_ਸ਼ਾਮਲ_ਹੈ },
    { UPROPS_SRC_CHAR_AND_PROPSVEC,  0, isPOSIX_alnum },
    { UPROPS_SRC_CHAR,  0, isPOSIX_blank },
    { UPROPS_SRC_CHAR,  0, is_POSIX_ਗ੍ਰਾਫ },
    { UPROPS_SRC_CHAR,  0, isPOSIX_print },
    { UPROPS_SRC_CHAR,  0, isPOSIX_xdigit },
    { UPROPS_SRC_CASE,  0, case_ਦੁਆਧਾਰੀ_Property_ਸ਼ਾਮਲ_ਹੈ },  // UCHAR_CASED
    { UPROPS_SRC_CASE,  0, case_ਦੁਆਧਾਰੀ_Property_ਸ਼ਾਮਲ_ਹੈ },  // UCHAR_CASE_IGNORABLE
    { UPROPS_SRC_CASE,  0, case_ਦੁਆਧਾਰੀ_Property_ਸ਼ਾਮਲ_ਹੈ },  // UCHAR_CHANGES_WHEN_LOWERCASED
    { UPROPS_SRC_CASE,  0, case_ਦੁਆਧਾਰੀ_Property_ਸ਼ਾਮਲ_ਹੈ },  // UCHAR_CHANGES_WHEN_UPPERCASED
    { UPROPS_SRC_CASE,  0, case_ਦੁਆਧਾਰੀ_Property_ਸ਼ਾਮਲ_ਹੈ },  // UCHAR_CHANGES_WHEN_TITLECASED
    { UPROPS_SRC_CASE_AND_NORM,  0, changesWhenCasefolded },
    { UPROPS_SRC_CASE,  0, case_ਦੁਆਧਾਰੀ_Property_ਸ਼ਾਮਲ_ਹੈ },  // UCHAR_CHANGES_WHEN_CASEMAPPED
    { UPROPS_SRC_NFKC_CF, 0, changesWhenNFKC_Casefolded },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_PRESENTATION
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER_BASE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_COMPONENT
    { 2,                0, isRegionalIndicator },
    { 1,                U_ਨਕਾਬ(UPROPS_PREPENDED_CONCATENATION_MARK), default_ਸ਼ਾਮਲ_ਹੈ },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EXTENDED_PICTOGRAPHIC
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_BASIC_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_KEYCAP_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_MODIFIER_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_FLAG_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_TAG_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI_ZWJ_SEQUENCE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_RGI_EMOJI
    { UPROPS_SRC_IDSU, 0, isIDSUnaryOperator }, // UCHAR_IDS_UNARY_OPERATOR
    { UPROPS_SRC_ID_COMPAT_MATH, 0, is_IDCompat_Math_ਸ਼ੁਰੂ }, // UCHAR_ID_COMPAT_MATH_START
    { UPROPS_SRC_ID_COMPAT_MATH, 0, isIDCompatMathContinue }, // UCHAR_ID_COMPAT_MATH_CONTINUE
    { UPROPS_SRC_MCM, 0 , isModifierCombiningMark }, // UCHAR_MODIFIER_COMBINING_MARK
};

U_CAPI UBool U_EXPORT2
u_has_ਦੁਆਧਾਰੀ_Property(UChar32 c, UProperty which) {
    /* c is range-checked in the functions that are called from here */
    ਜੇ(which<UCHAR_ਦੁਆਧਾਰੀ_ਸ਼ੁਰੂ || UCHAR_ਦੁਆਧਾਰੀ_ਹੱਦਬੰਦੀ<=which) {
        /* not a known binary property */
        ਵਾਪਸ ਝੂਠ;
    } ਨਹੀਂਤਾਂ {
        ਅਚਲ ਦੁਆਧਾਰੀ_Property &prop=binProps[which];
        ਵਾਪਸ prop.ਸ਼ਾਮਲ_ਹੈ(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    ਵਾਪਸ u_has_ਦੁਆਧਾਰੀ_Property(c, UCHAR_ID_ਸ਼ੁਰੂ);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    ਵਾਪਸ u_has_ਦੁਆਧਾਰੀ_Property(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_string_Has_ਦੁਆਧਾਰੀ_Property(ਅਚਲ char16_t *s, int32_t length, UProperty which) {
    ਜੇ (s == ਖਾਲੀ_ਸੂਚਕ && length != 0) { ਵਾਪਸ ਝੂਠ; }
    ਜੇ (length == 1) {
        ਵਾਪਸ u_has_ਦੁਆਧਾਰੀ_Property(s[0], which);  // single code point
    } ਨਹੀਂਤਾਂ ਜੇ (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_ਅਗਲਾ(s, i, length, c);
        ਜੇ (length > 0 ? i == length : s[i] == 0) {
            ਵਾਪਸ u_has_ਦੁਆਧਾਰੀ_Property(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    ਵਾਪਸ UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::has_ਦੁਆਧਾਰੀ_Property(s, length, which);
}

ਬਣਤਰ IntProperty;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ int32_t Int_Property_Get_ਮੁੱਲ(ਅਚਲ IntProperty &prop, UChar32 c, UProperty which);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ int32_t Int_Property_Get_Max_ਮੁੱਲ(ਅਚਲ IntProperty &prop, UProperty which);

ਬਣਤਰ IntProperty {
    int32_t ਕਾਲਮ;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t ਖਿਸਕਾਓ;  // =maxValue if getMaxValueFromShift() is used
    Int_Property_Get_ਮੁੱਲ *get_ਮੁੱਲ;
    Int_Property_Get_Max_ਮੁੱਲ *get_Max_ਮੁੱਲ;
};

ਸਥਿਰ int32_t default_Get_ਮੁੱਲ(ਅਚਲ IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    ਵਾਪਸ static_cast<int32_t>(u_getUnicodeProperties(c, prop.ਕਾਲਮ) & prop.mask) >> prop.ਖਿਸਕਾਓ;
}

ਸਥਿਰ int32_t default_Get_Max_ਮੁੱਲ(ਅਚਲ IntProperty &prop, UProperty /*which*/) {
    ਵਾਪਸ (uprv_get_Max_ਮੁੱਲਾਂ(prop.ਕਾਲਮ)&prop.mask)>>prop.ਖਿਸਕਾਓ;
}

ਸਥਿਰ int32_t get_Max_ਮੁੱਲ_From_ਖਿਸਕਾਓ(ਅਚਲ IntProperty &prop, UProperty /*which*/) {
    ਵਾਪਸ prop.ਖਿਸਕਾਓ;
}

ਸਥਿਰ int32_t getBiDiClass(ਅਚਲ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ static_cast<int32_t>(u_charDirection(c));
}

ਸਥਿਰ int32_t getBiDiPairedBracketType(ਅਚਲ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

ਸਥਿਰ int32_t bi_Di_Get_Max_ਮੁੱਲ(ਅਚਲ IntProperty &/*prop*/, UProperty which) {
    ਵਾਪਸ ubidi_get_Max_ਮੁੱਲ(which);
}

ਸਥਿਰ int32_t get_ਖੰਡ(ਅਚਲ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ static_cast<int32_t>(ublock_get_ਕੋਡ(c));
}

ਸਥਿਰ int32_t ਖੰਡ_Get_Max_ਮੁੱਲ(ਅਚਲ IntProperty &/*prop*/, UProperty /*which*/) {
    ਵਾਪਸ uprv_get_Max_ਮੁੱਲਾਂ(UPROPS_MAX_ਮੁੱਲਾਂ_OTHER_ਸੂਚਕਾਂਕ) & UPROPS_MAX_ਖੰਡ;
}

#ਜੇ UCONFIG_NO_NORMALIZATION
ਸਥਿਰ int32_t getCombiningClass(ਅਚਲ IntProperty &, UChar32, UProperty) {
    ਵਾਪਸ 0;
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ int32_t getCombiningClass(ਅਚਲ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ u_getCombiningClass(c);
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ int32_t getGeneralCategory(ਅਚਲ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ static_cast<int32_t>(u_charType(c));
}

ਸਥਿਰ int32_t get_Joining_ਸਮੂਹ(ਅਚਲ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ ubidi_get_Joining_ਸਮੂਹ(c);
}

ਸਥਿਰ int32_t getJoiningType(ਅਚਲ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ ubidi_getJoiningType(c);
}

ਸਥਿਰ int32_t getNumericType(ਅਚਲ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_ਮੁੱਲ(u_getMainProperties(c)));
    ਵਾਪਸ UPROPS_NTV_GET_TYPE(ntv);
}

ਸਥਿਰ int32_t getScript(ਅਚਲ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    ਵਾਪਸ static_cast<int32_t>(uscript_getScript(c, &error_ਕੋਡ));
}

ਸਥਿਰ int32_t script_Get_Max_ਮੁੱਲ(ਅਚਲ IntProperty &/*prop*/, UProperty /*which*/) {
    ਵਾਪਸ uprv_get_Max_ਮੁੱਲਾਂ(0)&UPROPS_MAX_SCRIPT;
}

/*
 * Map some of the Grapheme Cluster Break values to Hangul Syllable Types.
 * Hangul_Syllable_Type used to be fully redundant with a subset of Grapheme_Cluster_Break.
 *
 * Starting with Unicode 16, this is no longer true for HST=V vs. GCB=V in some cases:
 * Some Kirat Rai vowels are given GCB=V for proper grapheme clustering, but
 * they are of course not related to Hangul syllables.
 */
ਸਥਿਰ ਅਚਲ UHangulSyllableType gcb_ਮੰਜ਼ਿਲ_Hst[]={
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

ਸਥਿਰ int32_t getHangulSyllableType(ਅਚਲ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Ignore supplementary code points: They all have HST=NA.
    // This is a simple way to handle the GCB!=hst cases since Unicode 16 (Kirat Rai vowels).
    ਜੇ(c>0xffff) {
        ਵਾਪਸ U_HST_NOT_APPLICABLE;
    }
    /* see comments on gcbToHst[] above */
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_ਨਕਾਬ) >> UPROPS_GCB_ਖਿਸਕਾਓ;
    ਜੇ(gcb<UPRV_LENGTHOF(gcb_ਮੰਜ਼ਿਲ_Hst)) {
        ਵਾਪਸ gcb_ਮੰਜ਼ਿਲ_Hst[gcb];
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ U_HST_NOT_APPLICABLE;
    }
}

#ਜੇ UCONFIG_NO_NORMALIZATION
ਸਥਿਰ int32_t get_Norm_ਤੇਜ਼_Check(ਅਚਲ IntProperty &, UChar32, UProperty) {
    ਵਾਪਸ 0;
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ int32_t get_Norm_ਤੇਜ਼_Check(ਅਚਲ IntProperty &/*prop*/, UChar32 c, UProperty which) {
    ਵਾਪਸ static_cast<int32_t>(unorm_get_ਤੇਜ਼_Check(c, static_cast<UNormalization_ਬਹੁਲਕ>(which - UCHAR_NFD_ਤੇਜ਼_CHECK + UNORM_NFD)));
}
#ਜੇ_ਅੰਤ

#ਜੇ UCONFIG_NO_NORMALIZATION
ਸਥਿਰ int32_t getLeadCombiningClass(ਅਚਲ IntProperty &, UChar32, UProperty) {
    ਵਾਪਸ 0;
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ int32_t getLeadCombiningClass(ਅਚਲ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ unorm_getFCD16(c)>>8;
}
#ਜੇ_ਅੰਤ

#ਜੇ UCONFIG_NO_NORMALIZATION
ਸਥਿਰ int32_t getTrailCombiningClass(ਅਚਲ IntProperty &, UChar32, UProperty) {
    ਵਾਪਸ 0;
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ int32_t getTrailCombiningClass(ਅਚਲ IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    ਵਾਪਸ unorm_getFCD16(c)&0xff;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ int32_t get_ਵਿੱਚ_PC(ਅਚਲ IntProperty &, UChar32 c, UProperty) {
    ਵਾਪਸ ulayout_ensure_ਅੰਕੜੇ() && gInpcTrie != ਖਾਲੀ_ਸੂਚਕ ? ucptrie_get(gInpcTrie, c) : 0;
}

ਸਥਿਰ int32_t get_ਵਿੱਚ_SC(ਅਚਲ IntProperty &, UChar32 c, UProperty) {
    ਵਾਪਸ ulayout_ensure_ਅੰਕੜੇ() && gInscTrie != ਖਾਲੀ_ਸੂਚਕ ? ucptrie_get(gInscTrie, c) : 0;
}

ਸਥਿਰ int32_t getVo(ਅਚਲ IntProperty &, UChar32 c, UProperty) {
    ਵਾਪਸ ulayout_ensure_ਅੰਕੜੇ() && gVoTrie != ਖਾਲੀ_ਸੂਚਕ ? ucptrie_get(gVoTrie, c) : 0;
}

ਸਥਿਰ int32_t layout_Get_Max_ਮੁੱਲ(ਅਚਲ IntProperty &/*prop*/, UProperty which) {
    ਜੇ (!ulayout_ensure_ਅੰਕੜੇ()) { ਵਾਪਸ 0; }
    ਚੋਣ (which) {
    ਹਾਲਤ UCHAR_INDIC_POSITIONAL_CATEGORY:
        ਵਾਪਸ g_Max_Inpc_ਮੁੱਲ;
    ਹਾਲਤ UCHAR_INDIC_SYLLABIC_CATEGORY:
        ਵਾਪਸ g_Max_Insc_ਮੁੱਲ;
    ਹਾਲਤ UCHAR_VERTICAL_ORIENTATION:
        ਵਾਪਸ g_Max_Vo_ਮੁੱਲ;
    ਮੂਲ:
        ਵਾਪਸ 0;
    }
}

ਸਥਿਰ int32_t get_IDStatus_ਮੁੱਲ(ਅਚਲ IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_ਖਿਸਕਾਓ;
    ਵਾਪਸ value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_ਸਥਿਤੀ_ALLOWED : U_ID_ਸਥਿਤੀ_RESTRICTED;
}

ਸਥਿਰ ਅਚਲ IntProperty intProps[UCHAR_INT_ਹੱਦਬੰਦੀ-UCHAR_INT_ਸ਼ੁਰੂ]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, bi_Di_Get_Max_ਮੁੱਲ },
    { UPROPS_SRC_ਖੰਡ, 0, 0,                               get_ਖੰਡ, ਖੰਡ_Get_Max_ਮੁੱਲ },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, get_Max_ਮੁੱਲ_From_ਖਿਸਕਾਓ },
    { 2,                UPROPS_DT_ਨਕਾਬ, 0,                  default_Get_ਮੁੱਲ, default_Get_Max_ਮੁੱਲ },
    { 0,                UPROPS_EA_ਨਕਾਬ, UPROPS_EA_ਖਿਸਕਾਓ,    default_Get_ਮੁੱਲ, default_Get_Max_ਮੁੱਲ },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_ਗਿਣਤੀ) - 1, getGeneralCategory, get_Max_ਮੁੱਲ_From_ਖਿਸਕਾਓ },
    { UPROPS_SRC_BIDI,  0, 0,                               get_Joining_ਸਮੂਹ, bi_Di_Get_Max_ਮੁੱਲ },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, bi_Di_Get_Max_ਮੁੱਲ },
    { 2,                UPROPS_LB_ਨਕਾਬ, UPROPS_LB_ਖਿਸਕਾਓ,    default_Get_ਮੁੱਲ, default_Get_Max_ਮੁੱਲ },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_ਗਿਣਤੀ) - 1, getNumericType, get_Max_ਮੁੱਲ_From_ਖਿਸਕਾਓ },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, script_Get_Max_ਮੁੱਲ },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_ਗਿਣਤੀ) - 1, getHangulSyllableType, get_Max_ਮੁੱਲ_From_ਖਿਸਕਾਓ },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), get_Norm_ਤੇਜ਼_Check, get_Max_ਮੁੱਲ_From_ਖਿਸਕਾਓ },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), get_Norm_ਤੇਜ਼_Check, get_Max_ਮੁੱਲ_From_ਖਿਸਕਾਓ },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), get_Norm_ਤੇਜ਼_Check, get_Max_ਮੁੱਲ_From_ਖਿਸਕਾਓ },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), get_Norm_ਤੇਜ਼_Check, get_Max_ਮੁੱਲ_From_ਖਿਸਕਾਓ },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, get_Max_ਮੁੱਲ_From_ਖਿਸਕਾਓ },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, get_Max_ਮੁੱਲ_From_ਖਿਸਕਾਓ },
    { 2,                UPROPS_GCB_ਨਕਾਬ, UPROPS_GCB_ਖਿਸਕਾਓ,  default_Get_ਮੁੱਲ, default_Get_Max_ਮੁੱਲ },
    { 2,                UPROPS_SB_ਨਕਾਬ, UPROPS_SB_ਖਿਸਕਾਓ,    default_Get_ਮੁੱਲ, default_Get_Max_ਮੁੱਲ },
    { 2,                UPROPS_WB_ਨਕਾਬ, UPROPS_WB_ਖਿਸਕਾਓ,    default_Get_ਮੁੱਲ, default_Get_Max_ਮੁੱਲ },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, bi_Di_Get_Max_ਮੁੱਲ },
    { UPROPS_SRC_INPC,  0, 0,                               get_ਵਿੱਚ_PC, layout_Get_Max_ਮੁੱਲ },
    { UPROPS_SRC_INSC,  0, 0,                               get_ਵਿੱਚ_SC, layout_Get_Max_ਮੁੱਲ },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layout_Get_Max_ਮੁੱਲ },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_ਸਥਿਤੀ_ALLOWED), get_IDStatus_ਮੁੱਲ, get_Max_ਮੁੱਲ_From_ਖਿਸਕਾਓ },
    { 0,                UPROPS_INCB_ਨਕਾਬ, UPROPS_INCB_ਖਿਸਕਾਓ,default_Get_ਮੁੱਲ, default_Get_Max_ਮੁੱਲ },
};

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_ਮੁੱਲ(UChar32 c, UProperty which) {
    ਜੇ(which<UCHAR_INT_ਸ਼ੁਰੂ) {
        ਜੇ(UCHAR_ਦੁਆਧਾਰੀ_ਸ਼ੁਰੂ<=which && which<UCHAR_ਦੁਆਧਾਰੀ_ਹੱਦਬੰਦੀ) {
            ਅਚਲ ਦੁਆਧਾਰੀ_Property &prop=binProps[which];
            ਵਾਪਸ prop.ਸ਼ਾਮਲ_ਹੈ(prop, c, which);
        }
    } ਨਹੀਂਤਾਂ ਜੇ(which<UCHAR_INT_ਹੱਦਬੰਦੀ) {
        ਅਚਲ IntProperty &prop=intProps[which-UCHAR_INT_ਸ਼ੁਰੂ];
        ਵਾਪਸ prop.get_ਮੁੱਲ(prop, c, which);
    } ਨਹੀਂਤਾਂ ਜੇ(which==UCHAR_GENERAL_CATEGORY_ਨਕਾਬ) {
        ਵਾਪਸ U_ਨਕਾਬ(u_charType(c));
    }
    ਵਾਪਸ 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Min_ਮੁੱਲ(UProperty /*which*/) {
    ਵਾਪਸ 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Max_ਮੁੱਲ(UProperty which) {
    ਜੇ(which<UCHAR_INT_ਸ਼ੁਰੂ) {
        ਜੇ(UCHAR_ਦੁਆਧਾਰੀ_ਸ਼ੁਰੂ<=which && which<UCHAR_ਦੁਆਧਾਰੀ_ਹੱਦਬੰਦੀ) {
            ਵਾਪਸ 1;  // maximum true for all binary properties
        }
    } ਨਹੀਂਤਾਂ ਜੇ(which<UCHAR_INT_ਹੱਦਬੰਦੀ) {
        ਅਚਲ IntProperty &prop=intProps[which-UCHAR_INT_ਸ਼ੁਰੂ];
        ਵਾਪਸ prop.get_Max_ਮੁੱਲ(prop, which);
    }
    ਵਾਪਸ -1;  // undefined
}

U_CFUNC UProperty_ਸਰੋਤ U_EXPORT2
uprops_get_ਸਰੋਤ(UProperty which) {
    ਜੇ(which<UCHAR_ਦੁਆਧਾਰੀ_ਸ਼ੁਰੂ) {
        ਵਾਪਸ UPROPS_SRC_NONE; /* undefined */
    } ਨਹੀਂਤਾਂ ਜੇ(which<UCHAR_ਦੁਆਧਾਰੀ_ਹੱਦਬੰਦੀ) {
        ਅਚਲ ਦੁਆਧਾਰੀ_Property &prop=binProps[which];
        ਜੇ(prop.mask!=0) {
            ਵਾਪਸ UPROPS_SRC_PROPSVEC;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ (UProperty_ਸਰੋਤ)prop.ਕਾਲਮ;
        }
    } ਨਹੀਂਤਾਂ ਜੇ(which<UCHAR_INT_ਸ਼ੁਰੂ) {
        ਵਾਪਸ UPROPS_SRC_NONE; /* undefined */
    } ਨਹੀਂਤਾਂ ਜੇ(which<UCHAR_INT_ਹੱਦਬੰਦੀ) {
        ਅਚਲ IntProperty &prop=intProps[which-UCHAR_INT_ਸ਼ੁਰੂ];
        ਜੇ(prop.mask!=0) {
            ਵਾਪਸ UPROPS_SRC_PROPSVEC;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ (UProperty_ਸਰੋਤ)prop.ਕਾਲਮ;
        }
    } ਨਹੀਂਤਾਂ ਜੇ(which<UCHAR_STRING_ਸ਼ੁਰੂ) {
        ਚੋਣ(which) {
        ਹਾਲਤ UCHAR_GENERAL_CATEGORY_ਨਕਾਬ:
        ਹਾਲਤ UCHAR_NUMERIC_ਮੁੱਲ:
            ਵਾਪਸ UPROPS_SRC_CHAR;

        ਮੂਲ:
            ਵਾਪਸ UPROPS_SRC_NONE;
        }
    } ਨਹੀਂਤਾਂ ਜੇ(which<UCHAR_STRING_ਹੱਦਬੰਦੀ) {
        ਚੋਣ(which) {
        ਹਾਲਤ UCHAR_AGE:
            ਵਾਪਸ UPROPS_SRC_PROPSVEC;

        ਹਾਲਤ UCHAR_BIDI_MIRRORING_GLYPH:
            ਵਾਪਸ UPROPS_SRC_BIDI;

        ਹਾਲਤ UCHAR_CASE_FOLDING:
        ਹਾਲਤ UCHAR_LOWERCASE_MAPPING:
        ਹਾਲਤ UCHAR_SIMPLE_CASE_FOLDING:
        ਹਾਲਤ UCHAR_SIMPLE_LOWERCASE_MAPPING:
        ਹਾਲਤ UCHAR_SIMPLE_TITLECASE_MAPPING:
        ਹਾਲਤ UCHAR_SIMPLE_UPPERCASE_MAPPING:
        ਹਾਲਤ UCHAR_TITLECASE_MAPPING:
        ਹਾਲਤ UCHAR_UPPERCASE_MAPPING:
            ਵਾਪਸ UPROPS_SRC_CASE;

        ਹਾਲਤ UCHAR_ISO_COMMENT:
        ਹਾਲਤ UCHAR_NAME:
        ਹਾਲਤ UCHAR_UNICODE_1_NAME:
            ਵਾਪਸ UPROPS_SRC_ਨਾਂ;

        ਮੂਲ:
            ਵਾਪਸ UPROPS_SRC_NONE;
        }
    } ਨਹੀਂਤਾਂ {
        ਚੋਣ(which) {
        ਹਾਲਤ UCHAR_SCRIPT_EXTENSIONS:
        ਹਾਲਤ UCHAR_IDENTIFIER_TYPE:
            ਵਾਪਸ UPROPS_SRC_PROPSVEC;
        ਮੂਲ:
            ਵਾਪਸ UPROPS_SRC_NONE; /* undefined */
        }
    }
}

U_CFUNC ਖਾਲੀ U_EXPORT2
uprops_ਜੋੜੋ_Property_Starts(UProperty_ਸਰੋਤ src, ਅਚਲ USetAdder *sa, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) { ਵਾਪਸ; }
    ਜੇ (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        ਲਈ (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->ਜੋੜੋ(sa->ਸੈੱਟ, c);
        }
        // single characters
        ਲਈ (UChar32 c : ID_COMPAT_MATH_ਸ਼ੁਰੂ) {
            sa->ਜੋੜੋ(sa->ਸੈੱਟ, c);
            sa->ਜੋੜੋ(sa->ਸੈੱਟ, c + 1);
        }
        ਵਾਪਸ;
    }
    ਜੇ (src == UPROPS_SRC_MCM) {
        // range limits
        ਲਈ (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->ਜੋੜੋ(sa->ਸੈੱਟ, c);
        }
        ਵਾਪਸ;
    }
    ਜੇ (!ulayout_ensure_ਅੰਕੜੇ(*p_Error_ਕੋਡ)) { ਵਾਪਸ; }
    ਅਚਲ UCPTrie *trie;
    ਚੋਣ (src) {
    ਹਾਲਤ UPROPS_SRC_INPC:
        trie = gInpcTrie;
        ਰੋਕੋ;
    ਹਾਲਤ UPROPS_SRC_INSC:
        trie = gInscTrie;
        ਰੋਕੋ;
    ਹਾਲਤ UPROPS_SRC_VO:
        trie = gVoTrie;
        ਰੋਕੋ;
    ਮੂਲ:
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    ਜੇ (trie == ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ = U_MISSING_RESOURCE_ERROR;
        ਵਾਪਸ;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 ਸ਼ੁਰੂ = 0, ਸਿਰਾ;
    ਜਦੋਂ ((ਸਿਰਾ = ucptrie_getRange(trie, ਸ਼ੁਰੂ, UCPMAP_RANGE_ਸਾਧਾਰਨ, 0,
                                   ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ)) >= 0) {
        sa->ਜੋੜੋ(sa->ਸੈੱਟ, ਸ਼ੁਰੂ);
        ਸ਼ੁਰੂ = ਸਿਰਾ + 1;
    }
}

U_CAPI ਤਾਰਕਿਕ U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t type_ਸੂਚਕਾਂਕ = type;  // also guards against negative type integers
    ਜੇ (type_ਸੂਚਕਾਂਕ >= UPRV_LENGTHOF(uprops_id_Type_ਮੰਜ਼ਿਲ_Encoded)) {
        ਵਾਪਸ ਝੂਠ;
    }
    uint32_t encodedType = uprops_id_Type_ਮੰਜ਼ਿਲ_Encoded[type_ਸੂਚਕਾਂਕ];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_ਖਿਸਕਾਓ;
    ਜੇ ((encodedType & UPROPS_ID_TYPE_ਦੋਆਧਾਰੀ_ਅੰਕ) != 0) {
        ਵਾਪਸ value < UPROPS_ID_TYPE_FORBIDDEN && (value & encodedType) != 0;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ value == encodedType;
    }
}

ਨਾਮਾਕਾਸ਼ {

ਖਾਲੀ maybeAppendType(uint32_t value, uint32_t bit, UIdentifierType t,
                     UIdentifierType *types, int32_t &length, int32_t capacity) {
    ਜੇ ((value & bit) != 0) {
        ਜੇ (length < capacity) {
            types[length] = t;
        }
        ++length;
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) { ਵਾਪਸ 0; }
    ਜੇ (capacity < 0 || (capacity > 0 && types == ਖਾਲੀ_ਸੂਚਕ)) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_ਖਿਸਕਾਓ;
    ਜੇ ((value & UPROPS_ID_TYPE_FORBIDDEN) == UPROPS_ID_TYPE_FORBIDDEN ||
            value == UPROPS_ID_TYPE_NOT_CHARACTER) {
        // single value
        ਜੇ (capacity > 0) {
            UIdentifierType t;
            ਚੋਣ (value) {
                ਹਾਲਤ UPROPS_ID_TYPE_NOT_CHARACTER: t = U_ID_TYPE_NOT_CHARACTER; ਰੋਕੋ;
                ਹਾਲਤ UPROPS_ID_TYPE_DEPRECATED: t = U_ID_TYPE_DEPRECATED; ਰੋਕੋ;
                ਹਾਲਤ UPROPS_ID_TYPE_DEFAULT_IGNORABLE: t = U_ID_TYPE_DEFAULT_IGNORABLE; ਰੋਕੋ;
                ਹਾਲਤ UPROPS_ID_TYPE_NOT_NFKC: t = U_ID_TYPE_NOT_NFKC; ਰੋਕੋ;
                ਹਾਲਤ UPROPS_ID_TYPE_INCLUSION: t = U_ID_TYPE_INCLUSION; ਰੋਕੋ;
                ਹਾਲਤ UPROPS_ID_TYPE_RECOMMENDED: t = U_ID_TYPE_RECOMMENDED; ਰੋਕੋ;
                ਮੂਲ:
                    *p_Error_ਕੋਡ = U_INVALID_FORMAT_ERROR;
                    ਵਾਪਸ 0;
            }
            types[0] = t;
        } ਨਹੀਂਤਾਂ {
            *p_Error_ਕੋਡ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        }
        ਵਾਪਸ 1;
    } ਨਹੀਂਤਾਂ {
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
        ਜੇ (length >= capacity) {
            *p_Error_ਕੋਡ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        }
        ਵਾਪਸ length;
    }
}

#ਜੇ !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_get_FC_NFKC_ਘੇਰਾ(UChar32 c, char16_t *dest, int32_t dest_ਸਮਰੱਥਾ, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(dest_ਸਮਰੱਥਾ<0 || (dest==ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ>0)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    ਅਚਲ Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    ਅਚਲ char16_t *folded1;
    int32_t folded1_ਲੰਮਾਈ=ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_Folding(c, &folded1, U_FOLD_CASE_DEFAULT);
    ਜੇ(folded1_ਲੰਮਾਈ<0) {
        ਅਚਲ Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        ਜੇ(nfkcImpl->get_Comp_ਤੇਜ਼_Check(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, 0, p_Error_ਕੋਡ);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.ਸੈੱਟ_ਮੰਜ਼ਿਲ(c);
    } ਨਹੀਂਤਾਂ {
        ਜੇ(folded1_ਲੰਮਾਈ>UCASE_MAX_STRING_ਲੰਮਾਈ) {
            folded1String.ਸੈੱਟ_ਮੰਜ਼ਿਲ(folded1_ਲੰਮਾਈ);
        } ਨਹੀਂਤਾਂ {
            folded1String.ਸੈੱਟ_ਮੰਜ਼ਿਲ(ਝੂਠ, folded1, folded1_ਲੰਮਾਈ);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *p_Error_ਕੋਡ);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *p_Error_ਕੋਡ);
    // if (c != b) add the mapping from a to c
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ) || kc1==kc2) {
        ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, 0, p_Error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ kc2.extract(dest, dest_ਸਮਰੱਥਾ, *p_Error_ਕੋਡ);
    }
}

#ਜੇ_ਅੰਤ
