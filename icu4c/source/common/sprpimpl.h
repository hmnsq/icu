// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 *
 *   Copyright (C) 2003-2006, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 *
 *******************************************************************************
 *   file name:  sprpimpl.h
 *   encoding:   UTF-8
 *   tab size:   8 (not used)
 *   indentation:4
 *
 *   created on: 2003feb1
 *   created by: Ram Viswanadha
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SPRPIMPL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPRPIMPL_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_IDNA

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/parseerr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/usprep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubidi_props.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPREP_ਅੰਕੜੇ_TYPE "spp"

ਗਿਣਤੀ UStringPrepType{
    USPREP_UNASSIGNED           = 0x0000 ,
    USPREP_MAP                  = 0x0001 ,
    USPREP_PROHIBITED           = 0x0002 , 
    USPREP_DELETE               = 0x0003 ,
    USPREP_TYPE_ਹੱਦਬੰਦੀ           = 0x0004  
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UStringPrepType UStringPrepType;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USPREP_TYPE_ਨਾਂ_ARRAY
ਸਥਿਰ ਅਚਲ ਅੱਖਰ* usprep_Type_ਨਾਂ[] ={
    "UNASSIGNED" ,          
    "MAP" , 
    "PROHIBITED" ,        
    "DELETE",
    "TYPE_LIMIT" 
};
#ਜੇ_ਅੰਤ

ਗਿਣਤੀ{
    _SPREP_NORMALIZATION_ON = 0x0001,
    _SPREP_CHECK_BIDI_ON    = 0x0002
};

ਗਿਣਤੀ{
    SPREP_TYPE_ਹੱਦ       = 0xFFF0,
    SPREP_MAX_ਸੂਚਕਾਂਕ_ਮੁੱਲ      = 0x3FBF,   /*16139*/ 
    SPREP_MAX_ਸੂਚਕਾਂਕ_ਸਿਖਰ_ਲੰਮਾਈ = 0x0003
};

/* indexes[] value names */
ਗਿਣਤੀ {
    SPREP_ਸੂਚਕਾਂਕ_TRIE_ਮਾਪ                  = 0, /* number of bytes in StringPrep trie */
    SPREP_ਸੂਚਕਾਂਕ_MAPPING_ਅੰਕੜੇ_ਮਾਪ          = 1, /* The array that contains the mapping   */
    SPREP_NORM_CORRECTNS_ਆਖਰੀ_UNI_ਸੰਸਕਰਣ  = 2, /* The index of Unicode version of last entry in NormalizationCorrections.txt */ 
    SPREP_ONE_UCHAR_MAPPING_ਸੂਚਕਾਂਕ_ਸ਼ੁਰੂ    = 3, /* The starting index of 1 UChar mapping index in the mapping data array */
    SPREP_TWO_UCHARS_MAPPING_ਸੂਚਕਾਂਕ_ਸ਼ੁਰੂ   = 4, /* The starting index of 2 UChars mapping index in the mapping data array */
    SPREP_THREE_UCHARS_MAPPING_ਸੂਚਕਾਂਕ_ਸ਼ੁਰੂ = 5, /* The starting index of 3 UChars mapping index in the mapping data array */
    SPREP_FOUR_UCHARS_MAPPING_ਸੂਚਕਾਂਕ_ਸ਼ੁਰੂ  = 6, /* The starting index of 4 UChars mapping index in the mapping data array */
    _SPREP_OPTIONS                          = 7, /* Bit set of options to turn on in the profile */
    SPREP_ਸੂਚਕਾਂਕ_ਸਿਖਰ=16                          /* changing this requires a new formatVersion */
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UString_Prep_ਕੁੰਜੀ UString_Prep_ਕੁੰਜੀ;


ਬਣਤਰ UString_Prep_ਕੁੰਜੀ{
    ਅੱਖਰ* name;
    ਅੱਖਰ* ਰਸਤਾ;
};

ਬਣਤਰ UStringPrepProfile{
    int32_t indexes[SPREP_ਸੂਚਕਾਂਕ_ਸਿਖਰ];
    UTrie sprepTrie;
    ਅਚਲ uint16_t* mapping_ਅੰਕੜੇ;
    UData_ਸਮ੍ਰਿਤੀ* sprep_ਅੰਕੜੇ;
    int32_t ref_ਗਿਣਤੀ;
    UBool is_ਅੰਕੜੇ_Loaded;
    UBool doNFKC;
    UBool checkBiDi;
};

/**
 * Helper function for populating the UParseError struct
 * @internal
 */
U_CAPI ਖਾਲੀ U_EXPORT2
uprv_syntaxError(ਅਚਲ UChar* ਨਿਯਮਾਂ, 
                 int32_t pos,
                 int32_t ਨਿਯਮਾਂ_Len,
                 UParseError* parseError);


/**
 * Swap StringPrep .spp profile data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
usprep_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds,
            ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
            UError_ਕੋਡ *p_Error_ਕੋਡ);

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_IDNA */

#ਜੇ_ਅੰਤ

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
