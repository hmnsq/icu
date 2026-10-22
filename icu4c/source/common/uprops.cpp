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
UData_स्मृतिः *g_Layout_स्मृतिः = शून्यसूचकः;

UCPTrie *gInpcTrie = शून्यसूचकः;  // Indic_Positional_Category
UCPTrie *gInscTrie = शून्यसूचकः;  // Indic_Syllabic_Category
UCPTrie *gVoTrie = शून्यसूचकः;  // Vertical_Orientation

int32_t g_Max_Inpc_मानम् = 0;
int32_t g_Max_Insc_मानम् = 0;
int32_t g_Max_Vo_मानम् = 0;

UBool U_CALLCONV uprops_cleanup() {
    udata_close(g_Layout_स्मृतिः);
    g_Layout_स्मृतिः = शून्यसूचकः;

    ucptrie_close(gInpcTrie);
    gInpcTrie = शून्यसूचकः;
    ucptrie_close(gInscTrie);
    gInscTrie = शून्यसूचकः;
    ucptrie_close(gVoTrie);
    gVoTrie = शून्यसूचकः;

    g_Max_Inpc_मानम् = 0;
    g_Max_Insc_मानम् = 0;
    g_Max_Vo_मानम् = 0;

    gLayoutInitOnce.reset();
    प्रत्यागम सत्य;
}

UBool U_CALLCONV
ulayout_isAcceptable(शून्य * /*context*/,
                     ध्रुव वर्ण * /* type */, ध्रुव वर्ण * /*name*/,
                     ध्रुव UDataInfo *pInfo) {
    प्रत्यागम pInfo->size >= 20 &&
        pInfo->isBigEndian == U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily == U_CHARSET_FAMILY &&
        pInfo->दत्त_Format[0] == ULAYOUT_FMT_0 &&
        pInfo->दत्त_Format[1] == ULAYOUT_FMT_1 &&
        pInfo->दत्त_Format[2] == ULAYOUT_FMT_2 &&
        pInfo->दत्त_Format[3] == ULAYOUT_FMT_3 &&
        pInfo->format_संस्करणम्[0] == 1;
}

// UInitOnce singleton initialization function
शून्य U_CALLCONV ulayout_आरोपय(UError_संहिता &error_संहिता) {
    g_Layout_स्मृतिः = udata_openChoice(
        शून्यसूचकः, ULAYOUT_दत्त_TYPE, ULAYOUT_दत्त_NAME,
        ulayout_isAcceptable, शून्यसूचकः, &error_संहिता);
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }

    ध्रुव uint8_t* अन्तर्_अष्टकानि = static_cast<ध्रुव uint8_t*>(udata_get_स्मृतिः(g_Layout_स्मृतिः));
    ध्रुव int32_t* अन्तर्_Indexes = reinterpret_cast<ध्रुव int32_t*>(अन्तर्_अष्टकानि);
    int32_t indexes_दीर्घता = अन्तर्_Indexes[ULAYOUT_IX_INDEXES_दीर्घता];
    यदि (indexes_दीर्घता < 12) {
        error_संहिता = U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        प्रत्यागम;
    }
    int32_t विस्थापनम् = indexes_दीर्घता * 4;
    int32_t शिखरम् = अन्तर्_Indexes[ULAYOUT_IX_INPC_TRIE_शिखरम्];
    int32_t trie_आकार = शिखरम् - विस्थापनम्;
    यदि (trie_आकार >= 16) {
        gInpcTrie = ucptrie_open_From_द्विमान(
            UCPTRIE_TYPE_ANY, UCPTRIE_मानम्_द्वयङ्काः_ANY,
            अन्तर्_अष्टकानि + विस्थापनम्, trie_आकार, शून्यसूचकः, &error_संहिता);
    }
    विस्थापनम् = शिखरम्;
    शिखरम् = अन्तर्_Indexes[ULAYOUT_IX_INSC_TRIE_शिखरम्];
    trie_आकार = शिखरम् - विस्थापनम्;
    यदि (trie_आकार >= 16) {
        gInscTrie = ucptrie_open_From_द्विमान(
            UCPTRIE_TYPE_ANY, UCPTRIE_मानम्_द्वयङ्काः_ANY,
            अन्तर्_अष्टकानि + विस्थापनम्, trie_आकार, शून्यसूचकः, &error_संहिता);
    }
    विस्थापनम् = शिखरम्;
    शिखरम् = अन्तर्_Indexes[ULAYOUT_IX_VO_TRIE_शिखरम्];
    trie_आकार = शिखरम् - विस्थापनम्;
    यदि (trie_आकार >= 16) {
        gVoTrie = ucptrie_open_From_द्विमान(
            UCPTRIE_TYPE_ANY, UCPTRIE_मानम्_द्वयङ्काः_ANY,
            अन्तर्_अष्टकानि + विस्थापनम्, trie_आकार, शून्यसूचकः, &error_संहिता);
    }

    uint32_t max_मानानि = अन्तर्_Indexes[ULAYOUT_IX_MAX_मानानि];
    g_Max_Inpc_मानम् = max_मानानि >> ULAYOUT_MAX_INPC_सरणम्;
    g_Max_Insc_मानम् = (max_मानानि >> ULAYOUT_MAX_INSC_सरणम्) & 0xff;
    g_Max_Vo_मानम् = (max_मानानि >> ULAYOUT_MAX_VO_सरणम्) & 0xff;

    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_UPROPS, uprops_cleanup);
}

UBool ulayout_ensure_दत्त(UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
    umtx_initOnce(gLayoutInitOnce, &ulayout_आरोपय, error_संहिता);
    प्रत्यागम U_SUCCESS(error_संहिता);
}

UBool ulayout_ensure_दत्त() {
    UError_संहिता error_संहिता = U_ZERO_ERROR;
    प्रत्यागम ulayout_ensure_दत्त(error_संहिता);
}

}  // namespace

/* general properties API functions ----------------------------------------- */

संरचना द्विमान_Property;

प्रकारनिर्देश UBool द्विमान_Property_अन्तर्भवति(ध्रुव द्विमान_Property &prop, UChar32 c, UProperty which);

संरचना द्विमान_Property {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    द्विमान_Property_अन्तर्भवति *अन्तर्भवति;
};

स्थिर UBool default_अन्तर्भवति(ध्रुव द्विमान_Property &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    प्रत्यागम (u_getUnicodeProperties(c, prop.column)&prop.mask)!=0;
}

स्थिर UBool case_द्विमान_Property_अन्तर्भवति(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty which) {
    प्रत्यागम ucase_has_द्विमान_Property(c, which);
}

स्थिर UBool isBidiControl(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम ubidi_isBidiControl(c);
}

स्थिर UBool isMirrored(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम ubidi_isMirrored(c);
}

स्थिर UBool isJoinControl(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम ubidi_isJoinControl(c);
}

#यदि UCONFIG_NO_NORMALIZATION
स्थिर UBool has_पूर्णम्_Composition_Exclusion(ध्रुव द्विमान_Property &, UChar32, UProperty) {
    प्रत्यागम असत्य;
}
#अन्यथा
स्थिर UBool has_पूर्णम्_Composition_Exclusion(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // By definition, Full_Composition_Exclusion is the same as NFC_QC=No.
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    ध्रुव Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_संहिता);
    प्रत्यागम U_SUCCESS(error_संहिता) && impl->isCompNo(impl->getNorm16(c));
}
#यदि_अन्तः

// UCHAR_NF*_INERT properties
#यदि UCONFIG_NO_NORMALIZATION
स्थिर UBool isNormInert(ध्रुव द्विमान_Property &, UChar32, UProperty) {
    प्रत्यागम असत्य;
}
#अन्यथा
स्थिर UBool isNormInert(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty which) {
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    ध्रुव Normalizer2 *norm2=Normalizer2Factory::getInstance(
        static_cast<UNormalization_बहुलकम्>(which - UCHAR_NFD_INERT + UNORM_NFD), error_संहिता);
    प्रत्यागम U_SUCCESS(error_संहिता) && norm2->isInert(c);
}
#यदि_अन्तः

#यदि UCONFIG_NO_NORMALIZATION
स्थिर UBool changesWhenCasefolded(ध्रुव द्विमान_Property &, UChar32, UProperty) {
    प्रत्यागम असत्य;
}
#अन्यथा
स्थिर UBool changesWhenCasefolded(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UnicodeString nfd;
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    ध्रुव Normalizer2 *nfcNorm2=Normalizer2::getNFCInstance(error_संहिता);
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम असत्य;
    }
    यदि(nfcNorm2->getDecomposition(c, nfd)) {
        /* c has a decomposition */
        यदि(nfd.length()==1) {
            c=nfd[0];  /* single BMP code point */
        } अन्यथा यदि(nfd.length()<=U16_MAX_दीर्घता &&
                  nfd.length()==U16_दीर्घता(c=nfd.char32At(0))
        ) {
            /* single supplementary code point */
        } अन्यथा {
            c=U_SENTINEL;
        }
    } अन्यथा यदि(c<0) {
        प्रत्यागम असत्य;  /* protect against bad input */
    }
    यदि(c>=0) {
        /* single code point */
        ध्रुव char16_t *परिणाम_String;
        प्रत्यागम ucase_गन्तव्यम्_पूर्णम्_Folding(c, &परिणाम_String, U_FOLD_CASE_DEFAULT) >= 0;
    } अन्यथा {
        /* guess some large but stack-friendly capacity */
        char16_t dest[2*UCASE_MAX_STRING_दीर्घता];
        int32_t dest_दीर्घता;
        dest_दीर्घता=u_strFoldCase(dest, UPRV_LENGTHOF(dest),
                                  nfd.get_बफर(), nfd.length(),
                                  U_FOLD_CASE_DEFAULT, &error_संहिता);
        प्रत्यागम U_SUCCESS(error_संहिता) &&
                       0!=u_strCompare(nfd.get_बफर(), nfd.length(),
                                       dest, dest_दीर्घता, असत्य);
    }
}
#यदि_अन्तः

#यदि UCONFIG_NO_NORMALIZATION
स्थिर UBool changesWhenNFKC_Casefolded(ध्रुव द्विमान_Property &, UChar32, UProperty) {
    प्रत्यागम असत्य;
}
#अन्यथा
स्थिर UBool changesWhenNFKC_Casefolded(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    ध्रुव Normalizer2Impl *kcf=Normalizer2Factory::getNFKC_CFImpl(error_संहिता);
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम असत्य;
    }
    UnicodeString src(c);
    UnicodeString dest;
    {
        // The ReorderingBuffer must be in a block because its destructor
        // needs to release dest's buffer before we look at its contents.
        Reordering_बफर बफर(*kcf, dest);
        // Small destCapacity for NFKC_CF(c).
        यदि(बफर.init(5, error_संहिता)) {
            ध्रुव char16_t *srcArray=src.get_बफर();
            kcf->compose(srcArray, srcArray+src.length(), असत्य,
                          सत्य, बफर, error_संहिता);
        }
    }
    प्रत्यागम U_SUCCESS(error_संहिता) && dest!=src;
}
#यदि_अन्तः

#यदि UCONFIG_NO_NORMALIZATION
स्थिर UBool isCanonSegmentStarter(ध्रुव द्विमान_Property &, UChar32, UProperty) {
    प्रत्यागम असत्य;
}
#अन्यथा
स्थिर UBool isCanonSegmentStarter(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    ध्रुव Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_संहिता);
    प्रत्यागम
        U_SUCCESS(error_संहिता) && impl->ensure_Canon_Iter_दत्त(error_संहिता) &&
        impl->isCanonSegmentStarter(c);
}
#यदि_अन्तः

स्थिर UBool isPOSIX_alnum(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम u_isalnumPOSIX(c);
}

स्थिर UBool isPOSIX_blank(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम u_isblank(c);
}

स्थिर UBool isPOSIX_graph(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम u_isgraphPOSIX(c);
}

स्थिर UBool isPOSIX_print(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम u_isprintPOSIX(c);
}

स्थिर UBool isPOSIX_xdigit(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम u_isxdigit(c);
}

स्थिर UBool isRegionalIndicator(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Property starts are a subset of lb=RI etc.
    प्रत्यागम 0x1F1E6<=c && c<=0x1F1FF;
}

स्थिर UBool hasEmojiProperty(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty which) {
    प्रत्यागम EmojiProps::has_द्विमान_Property(c, which);
}

स्थिर UBool isIDSUnaryOperator(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    // New in Unicode 15.1 for just two characters.
    प्रत्यागम 0x2FFE<=c && c<=0x2FFF;
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

स्थिर UBool isIDCompatMathStart(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    यदि (c < ID_COMPAT_MATH_START[0]) { प्रत्यागम असत्य; }  // fastpath for common scripts
    प्रति (UChar32 startChar : ID_COMPAT_MATH_START) {
        यदि (c == startChar) { प्रत्यागम सत्य; }
    }
    प्रत्यागम असत्य;
}

स्थिर UBool isIDCompatMathContinue(ध्रुव द्विमान_Property &prop, UChar32 c, UProperty /*which*/) {
    प्रति (int32_t i = 0; i < UPRV_LENGTHOF(ID_COMPAT_MATH_CONTINUE); i += 2) {
        यदि (c < ID_COMPAT_MATH_CONTINUE[i]) { प्रत्यागम असत्य; }  // below range start
        यदि (c < ID_COMPAT_MATH_CONTINUE[i + 1]) { प्रत्यागम सत्य; }  // below range limit
    }
    प्रत्यागम isIDCompatMathStart(prop, c, UCHAR_ID_COMPAT_MATH_START);
}

स्थिर UBool isModifierCombiningMark(ध्रुव द्विमान_Property &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रति (int32_t i = 0; i < UPRV_LENGTHOF(MODIFIER_COMBINING_MARK); i += 2) {
        यदि (c < MODIFIER_COMBINING_MARK[i]) { प्रत्यागम असत्य; }  // below range start
        यदि (c < MODIFIER_COMBINING_MARK[i + 1]) { प्रत्यागम सत्य; }  // below range limit
    }
    प्रत्यागम असत्य;
}

स्थिर ध्रुव द्विमान_Property binProps[UCHAR_द्विमान_सीमा]={
    /*
     * column and mask values for binary properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per binary UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { 1,                U_आवरणम्(UPROPS_ALPHABETIC), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_ASCII_HEX_DIGIT), default_अन्तर्भवति },
    { UPROPS_SRC_BIDI,  0, isBidiControl },
    { UPROPS_SRC_BIDI,  0, isMirrored },
    { 1,                U_आवरणम्(UPROPS_DASH), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_DEFAULT_IGNORABLE_संहिता_POINT), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_DEPRECATED), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_DIACRITIC), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_EXTENDER), default_अन्तर्भवति },
    { UPROPS_SRC_NFC,   0, has_पूर्णम्_Composition_Exclusion },
    { 1,                U_आवरणम्(UPROPS_GRAPHEME_आधारः), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_GRAPHEME_EXTEND), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_GRAPHEME_LINK), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_HEX_DIGIT), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_HYPHEN), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_ID_CONTINUE), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_ID_START), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_IDEOGRAPHIC), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_IDS_द्विमान_OPERATOR), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_IDS_TRINARY_OPERATOR), default_अन्तर्भवति },
    { UPROPS_SRC_BIDI,  0, isJoinControl },
    { 1,                U_आवरणम्(UPROPS_LOGICAL_क्रमः_EXCEPTION), default_अन्तर्भवति },
    { UPROPS_SRC_CASE,  0, case_द्विमान_Property_अन्तर्भवति },  // UCHAR_LOWERCASE
    { 1,                U_आवरणम्(UPROPS_MATH), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_NONCHARACTER_संहिता_POINT), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_QUOTATION_MARK), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_RADICAL), default_अन्तर्भवति },
    { UPROPS_SRC_CASE,  0, case_द्विमान_Property_अन्तर्भवति },  // UCHAR_SOFT_DOTTED
    { 1,                U_आवरणम्(UPROPS_TERMINAL_PUNCTUATION), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_UNIFIED_IDEOGRAPH), default_अन्तर्भवति },
    { UPROPS_SRC_CASE,  0, case_द्विमान_Property_अन्तर्भवति },  // UCHAR_UPPERCASE
    { 1,                U_आवरणम्(UPROPS_WHITE_SPACE), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_XID_CONTINUE), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_XID_START), default_अन्तर्भवति },
    { UPROPS_SRC_CASE,  0, case_द्विमान_Property_अन्तर्भवति },  // UCHAR_CASE_SENSITIVE
    { 1,                U_आवरणम्(UPROPS_S_पदांशः), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_VARIATION_SELECTOR), default_अन्तर्भवति },
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFD_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKD_INERT
    { UPROPS_SRC_NFC,   0, isNormInert },  // UCHAR_NFC_INERT
    { UPROPS_SRC_NFKC,  0, isNormInert },  // UCHAR_NFKC_INERT
    { UPROPS_SRC_NFC_CANON_ITER, 0, isCanonSegmentStarter },
    { 1,                U_आवरणम्(UPROPS_आदर्शः_SYNTAX), default_अन्तर्भवति },
    { 1,                U_आवरणम्(UPROPS_आदर्शः_WHITE_SPACE), default_अन्तर्भवति },
    { UPROPS_SRC_CHAR_AND_PROPSVEC,  0, isPOSIX_alnum },
    { UPROPS_SRC_CHAR,  0, isPOSIX_blank },
    { UPROPS_SRC_CHAR,  0, isPOSIX_graph },
    { UPROPS_SRC_CHAR,  0, isPOSIX_print },
    { UPROPS_SRC_CHAR,  0, isPOSIX_xdigit },
    { UPROPS_SRC_CASE,  0, case_द्विमान_Property_अन्तर्भवति },  // UCHAR_CASED
    { UPROPS_SRC_CASE,  0, case_द्विमान_Property_अन्तर्भवति },  // UCHAR_CASE_IGNORABLE
    { UPROPS_SRC_CASE,  0, case_द्विमान_Property_अन्तर्भवति },  // UCHAR_CHANGES_WHEN_LOWERCASED
    { UPROPS_SRC_CASE,  0, case_द्विमान_Property_अन्तर्भवति },  // UCHAR_CHANGES_WHEN_UPPERCASED
    { UPROPS_SRC_CASE,  0, case_द्विमान_Property_अन्तर्भवति },  // UCHAR_CHANGES_WHEN_TITLECASED
    { UPROPS_SRC_CASE_AND_NORM,  0, changesWhenCasefolded },
    { UPROPS_SRC_CASE,  0, case_द्विमान_Property_अन्तर्भवति },  // UCHAR_CHANGES_WHEN_CASEMAPPED
    { UPROPS_SRC_NFKC_CF, 0, changesWhenNFKC_Casefolded },
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_PRESENTATION
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_MODIFIER_BASE
    { UPROPS_SRC_EMOJI, 0, hasEmojiProperty },  // UCHAR_EMOJI_COMPONENT
    { 2,                0, isRegionalIndicator },
    { 1,                U_आवरणम्(UPROPS_PREPENDED_CONCATENATION_MARK), default_अन्तर्भवति },
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
u_has_द्विमान_Property(UChar32 c, UProperty which) {
    /* c is range-checked in the functions that are called from here */
    यदि(which<UCHAR_द्विमान_START || UCHAR_द्विमान_सीमा<=which) {
        /* not a known binary property */
        प्रत्यागम असत्य;
    } अन्यथा {
        ध्रुव द्विमान_Property &prop=binProps[which];
        प्रत्यागम prop.अन्तर्भवति(prop, c, which);
    }
}

/* Checks if the Unicode character can start a Unicode identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDStart(UChar32 c) {
    प्रत्यागम u_has_द्विमान_Property(c, UCHAR_ID_START);
}

/* Checks if the Unicode character can be a Unicode identifier part other than starting the
 identifier.*/
U_CAPI UBool U_EXPORT2
u_isIDPart(UChar32 c) {
    प्रत्यागम u_has_द्विमान_Property(c, UCHAR_ID_CONTINUE);
}

U_CAPI UBool U_EXPORT2
u_string_Has_द्विमान_Property(ध्रुव char16_t *s, int32_t length, UProperty which) {
    यदि (s == शून्यसूचकः && length != 0) { प्रत्यागम असत्य; }
    यदि (length == 1) {
        प्रत्यागम u_has_द्विमान_Property(s[0], which);  // single code point
    } अन्यथा यदि (length == 2 || (length < 0 && *s != 0)) {  // not empty string
        // first code point
        int32_t i = 0;
        UChar32 c;
        U16_अग्र(s, i, length, c);
        यदि (length > 0 ? i == length : s[i] == 0) {
            प्रत्यागम u_has_द्विमान_Property(c, which);  // single code point
        }
    }
    // Only call into EmojiProps for a relevant property,
    // so that we not unnecessarily try to load its data file.
    प्रत्यागम UCHAR_BASIC_EMOJI <= which && which <= UCHAR_RGI_EMOJI &&
        EmojiProps::has_द्विमान_Property(s, length, which);
}

संरचना IntProperty;

प्रकारनिर्देश int32_t Int_Property_Get_मानम्(ध्रुव IntProperty &prop, UChar32 c, UProperty which);
प्रकारनिर्देश int32_t Int_Property_Get_Max_मानम्(ध्रुव IntProperty &prop, UProperty which);

संरचना IntProperty {
    int32_t column;  // SRC_PROPSVEC column, or "source" if mask==0
    uint32_t mask;
    int32_t सरणम्;  // =maxValue if getMaxValueFromShift() is used
    Int_Property_Get_मानम् *get_मानम्;
    Int_Property_Get_Max_मानम् *get_Max_मानम्;
};

स्थिर int32_t default_Get_मानम्(ध्रुव IntProperty &prop, UChar32 c, UProperty /*which*/) {
    /* systematic, directly stored properties */
    प्रत्यागम static_cast<int32_t>(u_getUnicodeProperties(c, prop.column) & prop.mask) >> prop.सरणम्;
}

स्थिर int32_t default_Get_Max_मानम्(ध्रुव IntProperty &prop, UProperty /*which*/) {
    प्रत्यागम (uprv_get_Max_मानानि(prop.column)&prop.mask)>>prop.सरणम्;
}

स्थिर int32_t get_Max_मानम्_From_सरणम्(ध्रुव IntProperty &prop, UProperty /*which*/) {
    प्रत्यागम prop.सरणम्;
}

स्थिर int32_t getBiDiClass(ध्रुव IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम static_cast<int32_t>(u_charDirection(c));
}

स्थिर int32_t getBiDiPairedBracketType(ध्रुव IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम static_cast<int32_t>(ubidi_getPairedBracketType(c));
}

स्थिर int32_t bi_Di_Get_Max_मानम्(ध्रुव IntProperty &/*prop*/, UProperty which) {
    प्रत्यागम ubidi_get_Max_मानम्(which);
}

स्थिर int32_t get_खण्डम्(ध्रुव IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम static_cast<int32_t>(ublock_get_संहिता(c));
}

स्थिर int32_t खण्डम्_Get_Max_मानम्(ध्रुव IntProperty &/*prop*/, UProperty /*which*/) {
    प्रत्यागम uprv_get_Max_मानानि(UPROPS_MAX_मानानि_OTHER_सूचकांक) & UPROPS_MAX_खण्डम्;
}

#यदि UCONFIG_NO_NORMALIZATION
स्थिर int32_t getCombiningClass(ध्रुव IntProperty &, UChar32, UProperty) {
    प्रत्यागम 0;
}
#अन्यथा
स्थिर int32_t getCombiningClass(ध्रुव IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम u_getCombiningClass(c);
}
#यदि_अन्तः

स्थिर int32_t getGeneralCategory(ध्रुव IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम static_cast<int32_t>(u_charType(c));
}

स्थिर int32_t get_Joining_गणः(ध्रुव IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम ubidi_get_Joining_गणः(c);
}

स्थिर int32_t getJoiningType(ध्रुव IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम ubidi_getJoiningType(c);
}

स्थिर int32_t getNumericType(ध्रुव IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    int32_t ntv = static_cast<int32_t>(GET_NUMERIC_TYPE_मानम्(u_getMainProperties(c)));
    प्रत्यागम UPROPS_NTV_GET_TYPE(ntv);
}

स्थिर int32_t getScript(ध्रुव IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    प्रत्यागम static_cast<int32_t>(uscript_getScript(c, &error_संहिता));
}

स्थिर int32_t script_Get_Max_मानम्(ध्रुव IntProperty &/*prop*/, UProperty /*which*/) {
    प्रत्यागम uprv_get_Max_मानानि(0)&UPROPS_MAX_SCRIPT;
}

/*
 * Map some of the Grapheme Cluster Break values to Hangul Syllable Types.
 * Hangul_Syllable_Type used to be fully redundant with a subset of Grapheme_Cluster_Break.
 *
 * Starting with Unicode 16, this is no longer true for HST=V vs. GCB=V in some cases:
 * Some Kirat Rai vowels are given GCB=V for proper grapheme clustering, but
 * they are of course not related to Hangul syllables.
 */
स्थिर ध्रुव UHangulSyllableType gcb_गन्तव्यम्_Hst[]={
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

स्थिर int32_t getHangulSyllableType(ध्रुव IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    // Ignore supplementary code points: They all have HST=NA.
    // This is a simple way to handle the GCB!=hst cases since Unicode 16 (Kirat Rai vowels).
    यदि(c>0xffff) {
        प्रत्यागम U_HST_NOT_APPLICABLE;
    }
    /* see comments on gcbToHst[] above */
    int32_t gcb = static_cast<int32_t>(u_getUnicodeProperties(c, 2) & UPROPS_GCB_आवरणम्) >> UPROPS_GCB_सरणम्;
    यदि(gcb<UPRV_LENGTHOF(gcb_गन्तव्यम्_Hst)) {
        प्रत्यागम gcb_गन्तव्यम्_Hst[gcb];
    } अन्यथा {
        प्रत्यागम U_HST_NOT_APPLICABLE;
    }
}

#यदि UCONFIG_NO_NORMALIZATION
स्थिर int32_t get_Norm_शीघ्र_Check(ध्रुव IntProperty &, UChar32, UProperty) {
    प्रत्यागम 0;
}
#अन्यथा
स्थिर int32_t get_Norm_शीघ्र_Check(ध्रुव IntProperty &/*prop*/, UChar32 c, UProperty which) {
    प्रत्यागम static_cast<int32_t>(unorm_get_शीघ्र_Check(c, static_cast<UNormalization_बहुलकम्>(which - UCHAR_NFD_शीघ्र_CHECK + UNORM_NFD)));
}
#यदि_अन्तः

#यदि UCONFIG_NO_NORMALIZATION
स्थिर int32_t getLeadCombiningClass(ध्रुव IntProperty &, UChar32, UProperty) {
    प्रत्यागम 0;
}
#अन्यथा
स्थिर int32_t getLeadCombiningClass(ध्रुव IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम unorm_getFCD16(c)>>8;
}
#यदि_अन्तः

#यदि UCONFIG_NO_NORMALIZATION
स्थिर int32_t getTrailCombiningClass(ध्रुव IntProperty &, UChar32, UProperty) {
    प्रत्यागम 0;
}
#अन्यथा
स्थिर int32_t getTrailCombiningClass(ध्रुव IntProperty &/*prop*/, UChar32 c, UProperty /*which*/) {
    प्रत्यागम unorm_getFCD16(c)&0xff;
}
#यदि_अन्तः

स्थिर int32_t get_अन्तर्_PC(ध्रुव IntProperty &, UChar32 c, UProperty) {
    प्रत्यागम ulayout_ensure_दत्त() && gInpcTrie != शून्यसूचकः ? ucptrie_get(gInpcTrie, c) : 0;
}

स्थिर int32_t get_अन्तर्_SC(ध्रुव IntProperty &, UChar32 c, UProperty) {
    प्रत्यागम ulayout_ensure_दत्त() && gInscTrie != शून्यसूचकः ? ucptrie_get(gInscTrie, c) : 0;
}

स्थिर int32_t getVo(ध्रुव IntProperty &, UChar32 c, UProperty) {
    प्रत्यागम ulayout_ensure_दत्त() && gVoTrie != शून्यसूचकः ? ucptrie_get(gVoTrie, c) : 0;
}

स्थिर int32_t layout_Get_Max_मानम्(ध्रुव IntProperty &/*prop*/, UProperty which) {
    यदि (!ulayout_ensure_दत्त()) { प्रत्यागम 0; }
    विकल्प (which) {
    पक्ष UCHAR_INDIC_POSITIONAL_CATEGORY:
        प्रत्यागम g_Max_Inpc_मानम्;
    पक्ष UCHAR_INDIC_SYLLABIC_CATEGORY:
        प्रत्यागम g_Max_Insc_मानम्;
    पक्ष UCHAR_VERTICAL_ORIENTATION:
        प्रत्यागम g_Max_Vo_मानम्;
    स्वभाव:
        प्रत्यागम 0;
    }
}

स्थिर int32_t get_IDStatus_मानम्(ध्रुव IntProperty & /*prop*/, UChar32 c, UProperty /*which*/) {
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_सरणम्;
    प्रत्यागम value >= UPROPS_ID_TYPE_ALLOWED_MIN ? U_ID_स्थितिः_ALLOWED : U_ID_स्थितिः_RESTRICTED;
}

स्थिर ध्रुव IntProperty intProps[UCHAR_INT_सीमा-UCHAR_INT_START]={
    /*
     * column, mask and shift values for int-value properties from u_getUnicodeProperties().
     * Must be in order of corresponding UProperty,
     * and there must be exactly one entry per int UProperty.
     *
     * Properties with mask==0 are handled in code.
     * For them, column is the UPropertySource value.
     */
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiClass, bi_Di_Get_Max_मानम् },
    { UPROPS_SRC_खण्डम्, 0, 0,                               get_खण्डम्, खण्डम्_Get_Max_मानम् },
    { UPROPS_SRC_NFC,   0, 0xff,                            getCombiningClass, get_Max_मानम्_From_सरणम् },
    { 2,                UPROPS_DT_आवरणम्, 0,                  default_Get_मानम्, default_Get_Max_मानम् },
    { 0,                UPROPS_EA_आवरणम्, UPROPS_EA_सरणम्,    default_Get_मानम्, default_Get_Max_मानम् },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_CHAR_CATEGORY_गणनम्) - 1, getGeneralCategory, get_Max_मानम्_From_सरणम् },
    { UPROPS_SRC_BIDI,  0, 0,                               get_Joining_गणः, bi_Di_Get_Max_मानम् },
    { UPROPS_SRC_BIDI,  0, 0,                               getJoiningType, bi_Di_Get_Max_मानम् },
    { 2,                UPROPS_LB_आवरणम्, UPROPS_LB_सरणम्,    default_Get_मानम्, default_Get_Max_मानम् },
    { UPROPS_SRC_CHAR, 0, static_cast<int32_t>(U_NT_गणनम्) - 1, getNumericType, get_Max_मानम्_From_सरणम् },
    { UPROPS_SRC_PROPSVEC, 0, 0,                            getScript, script_Get_Max_मानम् },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_HST_गणनम्) - 1, getHangulSyllableType, get_Max_मानम्_From_सरणम् },
    // UCHAR_NFD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_YES), get_Norm_शीघ्र_Check, get_Max_मानम्_From_सरणम् },
    // UCHAR_NFKD_QUICK_CHECK: max=1=YES -- never "maybe", only "no" or "yes"
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_YES), get_Norm_शीघ्र_Check, get_Max_मानम्_From_सरणम् },
    // UCHAR_NFC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFC, 0, static_cast<int32_t>(UNORM_MAYBE), get_Norm_शीघ्र_Check, get_Max_मानम्_From_सरणम् },
    // UCHAR_NFKC_QUICK_CHECK: max=2=MAYBE
    { UPROPS_SRC_NFKC, 0, static_cast<int32_t>(UNORM_MAYBE), get_Norm_शीघ्र_Check, get_Max_मानम्_From_सरणम् },
    { UPROPS_SRC_NFC,   0, 0xff,                            getLeadCombiningClass, get_Max_मानम्_From_सरणम् },
    { UPROPS_SRC_NFC,   0, 0xff,                            getTrailCombiningClass, get_Max_मानम्_From_सरणम् },
    { 2,                UPROPS_GCB_आवरणम्, UPROPS_GCB_सरणम्,  default_Get_मानम्, default_Get_Max_मानम् },
    { 2,                UPROPS_SB_आवरणम्, UPROPS_SB_सरणम्,    default_Get_मानम्, default_Get_Max_मानम् },
    { 2,                UPROPS_WB_आवरणम्, UPROPS_WB_सरणम्,    default_Get_मानम्, default_Get_Max_मानम् },
    { UPROPS_SRC_BIDI,  0, 0,                               getBiDiPairedBracketType, bi_Di_Get_Max_मानम् },
    { UPROPS_SRC_INPC,  0, 0,                               get_अन्तर्_PC, layout_Get_Max_मानम् },
    { UPROPS_SRC_INSC,  0, 0,                               get_अन्तर्_SC, layout_Get_Max_मानम् },
    { UPROPS_SRC_VO,    0, 0,                               getVo, layout_Get_Max_मानम् },
    { UPROPS_SRC_PROPSVEC, 0, static_cast<int32_t>(U_ID_स्थितिः_ALLOWED), get_IDStatus_मानम्, get_Max_मानम्_From_सरणम् },
    { 0,                UPROPS_INCB_आवरणम्, UPROPS_INCB_सरणम्,default_Get_मानम्, default_Get_Max_मानम् },
};

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_मानम्(UChar32 c, UProperty which) {
    यदि(which<UCHAR_INT_START) {
        यदि(UCHAR_द्विमान_START<=which && which<UCHAR_द्विमान_सीमा) {
            ध्रुव द्विमान_Property &prop=binProps[which];
            प्रत्यागम prop.अन्तर्भवति(prop, c, which);
        }
    } अन्यथा यदि(which<UCHAR_INT_सीमा) {
        ध्रुव IntProperty &prop=intProps[which-UCHAR_INT_START];
        प्रत्यागम prop.get_मानम्(prop, c, which);
    } अन्यथा यदि(which==UCHAR_GENERAL_CATEGORY_आवरणम्) {
        प्रत्यागम U_आवरणम्(u_charType(c));
    }
    प्रत्यागम 0;  // undefined
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Min_मानम्(UProperty /*which*/) {
    प्रत्यागम 0; /* all binary/enum/int properties have a minimum value of 0 */
}

U_CAPI int32_t U_EXPORT2
u_get_Int_Property_Max_मानम्(UProperty which) {
    यदि(which<UCHAR_INT_START) {
        यदि(UCHAR_द्विमान_START<=which && which<UCHAR_द्विमान_सीमा) {
            प्रत्यागम 1;  // maximum true for all binary properties
        }
    } अन्यथा यदि(which<UCHAR_INT_सीमा) {
        ध्रुव IntProperty &prop=intProps[which-UCHAR_INT_START];
        प्रत्यागम prop.get_Max_मानम्(prop, which);
    }
    प्रत्यागम -1;  // undefined
}

U_CFUNC UPropertySource U_EXPORT2
uprops_getSource(UProperty which) {
    यदि(which<UCHAR_द्विमान_START) {
        प्रत्यागम UPROPS_SRC_NONE; /* undefined */
    } अन्यथा यदि(which<UCHAR_द्विमान_सीमा) {
        ध्रुव द्विमान_Property &prop=binProps[which];
        यदि(prop.mask!=0) {
            प्रत्यागम UPROPS_SRC_PROPSVEC;
        } अन्यथा {
            प्रत्यागम (UPropertySource)prop.column;
        }
    } अन्यथा यदि(which<UCHAR_INT_START) {
        प्रत्यागम UPROPS_SRC_NONE; /* undefined */
    } अन्यथा यदि(which<UCHAR_INT_सीमा) {
        ध्रुव IntProperty &prop=intProps[which-UCHAR_INT_START];
        यदि(prop.mask!=0) {
            प्रत्यागम UPROPS_SRC_PROPSVEC;
        } अन्यथा {
            प्रत्यागम (UPropertySource)prop.column;
        }
    } अन्यथा यदि(which<UCHAR_STRING_START) {
        विकल्प(which) {
        पक्ष UCHAR_GENERAL_CATEGORY_आवरणम्:
        पक्ष UCHAR_NUMERIC_मानम्:
            प्रत्यागम UPROPS_SRC_CHAR;

        स्वभाव:
            प्रत्यागम UPROPS_SRC_NONE;
        }
    } अन्यथा यदि(which<UCHAR_STRING_सीमा) {
        विकल्प(which) {
        पक्ष UCHAR_AGE:
            प्रत्यागम UPROPS_SRC_PROPSVEC;

        पक्ष UCHAR_BIDI_MIRRORING_GLYPH:
            प्रत्यागम UPROPS_SRC_BIDI;

        पक्ष UCHAR_CASE_FOLDING:
        पक्ष UCHAR_LOWERCASE_MAPPING:
        पक्ष UCHAR_SIMPLE_CASE_FOLDING:
        पक्ष UCHAR_SIMPLE_LOWERCASE_MAPPING:
        पक्ष UCHAR_SIMPLE_TITLECASE_MAPPING:
        पक्ष UCHAR_SIMPLE_UPPERCASE_MAPPING:
        पक्ष UCHAR_TITLECASE_MAPPING:
        पक्ष UCHAR_UPPERCASE_MAPPING:
            प्रत्यागम UPROPS_SRC_CASE;

        पक्ष UCHAR_ISO_COMMENT:
        पक्ष UCHAR_NAME:
        पक्ष UCHAR_UNICODE_1_NAME:
            प्रत्यागम UPROPS_SRC_नामानि;

        स्वभाव:
            प्रत्यागम UPROPS_SRC_NONE;
        }
    } अन्यथा {
        विकल्प(which) {
        पक्ष UCHAR_SCRIPT_EXTENSIONS:
        पक्ष UCHAR_IDENTIFIER_TYPE:
            प्रत्यागम UPROPS_SRC_PROPSVEC;
        स्वभाव:
            प्रत्यागम UPROPS_SRC_NONE; /* undefined */
        }
    }
}

U_CFUNC शून्य U_EXPORT2
uprops_सङ्कलय_Property_Starts(UPropertySource src, ध्रुव USetAdder *sa, UError_संहिता *p_Error_संहिता) {
    यदि (U_FAILURE(*p_Error_संहिता)) { प्रत्यागम; }
    यदि (src == UPROPS_SRC_ID_COMPAT_MATH) {
        // range limits
        प्रति (UChar32 c : ID_COMPAT_MATH_CONTINUE) {
            sa->सङ्कलय(sa->समुच्चयः, c);
        }
        // single characters
        प्रति (UChar32 c : ID_COMPAT_MATH_START) {
            sa->सङ्कलय(sa->समुच्चयः, c);
            sa->सङ्कलय(sa->समुच्चयः, c + 1);
        }
        प्रत्यागम;
    }
    यदि (src == UPROPS_SRC_MCM) {
        // range limits
        प्रति (UChar32 c : MODIFIER_COMBINING_MARK) {
            sa->सङ्कलय(sa->समुच्चयः, c);
        }
        प्रत्यागम;
    }
    यदि (!ulayout_ensure_दत्त(*p_Error_संहिता)) { प्रत्यागम; }
    ध्रुव UCPTrie *trie;
    विकल्प (src) {
    पक्ष UPROPS_SRC_INPC:
        trie = gInpcTrie;
        भङ्ग;
    पक्ष UPROPS_SRC_INSC:
        trie = gInscTrie;
        भङ्ग;
    पक्ष UPROPS_SRC_VO:
        trie = gVoTrie;
        भङ्ग;
    स्वभाव:
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    यदि (trie == शून्यसूचकः) {
        *p_Error_संहिता = U_MISSING_RESOURCE_ERROR;
        प्रत्यागम;
    }

    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, अन्तः;
    यावत् ((अन्तः = ucptrie_getRange(trie, start, UCPMAP_RANGE_प्रसामान्यम्, 0,
                                   शून्यसूचकः, शून्यसूचकः, शून्यसूचकः)) >= 0) {
        sa->सङ्कलय(sa->समुच्चयः, start);
        start = अन्तः + 1;
    }
}

U_CAPI तार्किक U_EXPORT2
u_hasIDType(UChar32 c, UIdentifierType type) {
    uint32_t type_सूचकांक = type;  // also guards against negative type integers
    यदि (type_सूचकांक >= UPRV_LENGTHOF(uprops_id_Type_गन्तव्यम्_Encoded)) {
        प्रत्यागम असत्य;
    }
    uint32_t encodedType = uprops_id_Type_गन्तव्यम्_Encoded[type_सूचकांक];
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_सरणम्;
    यदि ((encodedType & UPROPS_ID_TYPE_BIT) != 0) {
        प्रत्यागम value < UPROPS_ID_TYPE_FORBIDDEN && (value & encodedType) != 0;
    } अन्यथा {
        प्रत्यागम value == encodedType;
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
u_getIDTypes(UChar32 c, UIdentifierType *types, int32_t capacity, UError_संहिता *p_Error_संहिता) {
    यदि (U_FAILURE(*p_Error_संहिता)) { प्रत्यागम 0; }
    यदि (capacity < 0 || (capacity > 0 && types == शून्यसूचकः)) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    uint32_t value = u_getUnicodeProperties(c, 2) >> UPROPS_2_ID_TYPE_सरणम्;
    यदि ((value & UPROPS_ID_TYPE_FORBIDDEN) == UPROPS_ID_TYPE_FORBIDDEN ||
            value == UPROPS_ID_TYPE_NOT_CHARACTER) {
        // single value
        यदि (capacity > 0) {
            UIdentifierType t;
            विकल्प (value) {
                पक्ष UPROPS_ID_TYPE_NOT_CHARACTER: t = U_ID_TYPE_NOT_CHARACTER; भङ्ग;
                पक्ष UPROPS_ID_TYPE_DEPRECATED: t = U_ID_TYPE_DEPRECATED; भङ्ग;
                पक्ष UPROPS_ID_TYPE_DEFAULT_IGNORABLE: t = U_ID_TYPE_DEFAULT_IGNORABLE; भङ्ग;
                पक्ष UPROPS_ID_TYPE_NOT_NFKC: t = U_ID_TYPE_NOT_NFKC; भङ्ग;
                पक्ष UPROPS_ID_TYPE_INCLUSION: t = U_ID_TYPE_INCLUSION; भङ्ग;
                पक्ष UPROPS_ID_TYPE_RECOMMENDED: t = U_ID_TYPE_RECOMMENDED; भङ्ग;
                स्वभाव:
                    *p_Error_संहिता = U_INVALID_FORMAT_ERROR;
                    प्रत्यागम 0;
            }
            types[0] = t;
        } अन्यथा {
            *p_Error_संहिता = U_बफर_अतिप्रवाहः_ERROR;
        }
        प्रत्यागम 1;
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
            *p_Error_संहिता = U_बफर_अतिप्रवाहः_ERROR;
        }
        प्रत्यागम length;
    }
}

#यदि !UCONFIG_NO_NORMALIZATION

U_CAPI int32_t U_EXPORT2
u_get_FC_NFKC_संवृतिः(UChar32 c, char16_t *dest, int32_t dest_क्षमता, UError_संहिता *p_Error_संहिता) {
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(dest_क्षमता<0 || (dest==शून्यसूचकः && dest_क्षमता>0)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    // Compute the FC_NFKC_Closure on the fly:
    // We have the API for complete coverage of Unicode properties, although
    // this value by itself is not useful via API.
    // (What could be useful is a custom normalization table that combines
    // case folding and NFKC.)
    // For the derivation, see Unicode's DerivedNormalizationProps.txt.
    ध्रुव Normalizer2 *nfkc=Normalizer2::getNFKCInstance(*p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    // first: b = NFKC(Fold(a))
    UnicodeString folded1String;
    ध्रुव char16_t *folded1;
    int32_t folded1_दीर्घता=ucase_गन्तव्यम्_पूर्णम्_Folding(c, &folded1, U_FOLD_CASE_DEFAULT);
    यदि(folded1_दीर्घता<0) {
        ध्रुव Normalizer2Impl *nfkcImpl=Normalizer2Factory::getImpl(nfkc);
        यदि(nfkcImpl->get_Comp_शीघ्र_Check(nfkcImpl->getNorm16(c))!=UNORM_NO) {
            प्रत्यागम u_terminateUChars(dest, dest_क्षमता, 0, p_Error_संहिता);  // c does not change at all under CaseFolding+NFKC
        }
        folded1String.समुच्चयः_गन्तव्यम्(c);
    } अन्यथा {
        यदि(folded1_दीर्घता>UCASE_MAX_STRING_दीर्घता) {
            folded1String.समुच्चयः_गन्तव्यम्(folded1_दीर्घता);
        } अन्यथा {
            folded1String.समुच्चयः_गन्तव्यम्(असत्य, folded1, folded1_दीर्घता);
        }
    }
    UnicodeString kc1=nfkc->normalize(folded1String, *p_Error_संहिता);
    // second: c = NFKC(Fold(b))
    UnicodeString folded2String(kc1);
    UnicodeString kc2=nfkc->normalize(folded2String.foldCase(), *p_Error_संहिता);
    // if (c != b) add the mapping from a to c
    यदि(U_FAILURE(*p_Error_संहिता) || kc1==kc2) {
        प्रत्यागम u_terminateUChars(dest, dest_क्षमता, 0, p_Error_संहिता);
    } अन्यथा {
        प्रत्यागम kc2.extract(dest, dest_क्षमता, *p_Error_संहिता);
    }
}

#यदि_अन्तः
