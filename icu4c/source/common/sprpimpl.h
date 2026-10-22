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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SPRPIMPL_H
#ವ್ಯಾಖ್ಯಾನಿಸು SPRPIMPL_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_IDNA

#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/parseerr.h"
#ಅನ್ತರ್ಭಾವ "unicode/usprep.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "utrie.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"
#ಅನ್ತರ್ಭಾವ "ubidi_props.h"

#ವ್ಯಾಖ್ಯಾನಿಸು SPREP_ದತ್ತಾಂಶ_TYPE "spp"

ಎಣಿಕೆ UStringPrepType{
    USPREP_UNASSIGNED           = 0x0000 ,
    USPREP_MAP                  = 0x0001 ,
    USPREP_PROHIBITED           = 0x0002 , 
    USPREP_DELETE               = 0x0003 ,
    USPREP_TYPE_ಎಲ್ಲೆ           = 0x0004  
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UStringPrepType UStringPrepType;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USPREP_TYPE_ಹೆಸರುಗಳು_ARRAY
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* usprep_Type_ಹೆಸರುಗಳು[] ={
    "UNASSIGNED" ,          
    "MAP" , 
    "PROHIBITED" ,        
    "DELETE",
    "TYPE_LIMIT" 
};
#ಆದರೆ_ಅಂತ್ಯ

ಎಣಿಕೆ{
    _SPREP_NORMALIZATION_ON = 0x0001,
    _SPREP_CHECK_BIDI_ON    = 0x0002
};

ಎಣಿಕೆ{
    SPREP_TYPE_ಮಿತಿ       = 0xFFF0,
    SPREP_MAX_ಸೂಚ್ಯಂಕ_ಮೌಲ್ಯ      = 0x3FBF,   /*16139*/ 
    SPREP_MAX_ಸೂಚ್ಯಂಕ_ತುದಿ_ಉದ್ದಳತೆ = 0x0003
};

/* indexes[] value names */
ಎಣಿಕೆ {
    SPREP_ಸೂಚ್ಯಂಕ_TRIE_ಗಾತ್ರ                  = 0, /* number of bytes in StringPrep trie */
    SPREP_ಸೂಚ್ಯಂಕ_MAPPING_ದತ್ತಾಂಶ_ಗಾತ್ರ          = 1, /* The array that contains the mapping   */
    SPREP_NORM_CORRECTNS_ಕೊನೆಯ_UNI_ಆವೃತ್ತಿ  = 2, /* The index of Unicode version of last entry in NormalizationCorrections.txt */ 
    SPREP_ONE_UCHAR_MAPPING_ಸೂಚ್ಯಂಕ_ಆರಂಭ    = 3, /* The starting index of 1 UChar mapping index in the mapping data array */
    SPREP_TWO_UCHARS_MAPPING_ಸೂಚ್ಯಂಕ_ಆರಂಭ   = 4, /* The starting index of 2 UChars mapping index in the mapping data array */
    SPREP_THREE_UCHARS_MAPPING_ಸೂಚ್ಯಂಕ_ಆರಂಭ = 5, /* The starting index of 3 UChars mapping index in the mapping data array */
    SPREP_FOUR_UCHARS_MAPPING_ಸೂಚ್ಯಂಕ_ಆರಂಭ  = 6, /* The starting index of 4 UChars mapping index in the mapping data array */
    _SPREP_OPTIONS                          = 7, /* Bit set of options to turn on in the profile */
    SPREP_ಸೂಚ್ಯಂಕ_ತುದಿ=16                          /* changing this requires a new formatVersion */
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UString_Prep_ಕೀಲಿ UString_Prep_ಕೀಲಿ;


ರಚನೆ UString_Prep_ಕೀಲಿ{
    ಅಕ್ಷರ* name;
    ಅಕ್ಷರ* ಮಾರ್ಗ;
};

ರಚನೆ UStringPrepProfile{
    int32_t indexes[SPREP_ಸೂಚ್ಯಂಕ_ತುದಿ];
    UTrie sprepTrie;
    ಸ್ಥಿರಾಂಕ uint16_t* mapping_ದತ್ತಾಂಶ;
    UData_ಸ್ಮರಣೆ* sprep_ದತ್ತಾಂಶ;
    int32_t ref_ಎಣಿಕೆ;
    UBool is_ದತ್ತಾಂಶ_Loaded;
    UBool doNFKC;
    UBool checkBiDi;
};

/**
 * Helper function for populating the UParseError struct
 * @internal
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
uprv_syntaxError(ಸ್ಥಿರಾಂಕ UChar* ನಿಯಮಗಳು, 
                 int32_t pos,
                 int32_t ನಿಯಮಗಳು_Len,
                 UParseError* parseError);


/**
 * Swap StringPrep .spp profile data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
usprep_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
            ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t ಉದ್ದಳತೆ, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
            UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_IDNA */

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
