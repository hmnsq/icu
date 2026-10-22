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

#ifndef SPRPIMPL_H
#define SPRPIMPL_H

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_IDNA

#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/parseerr.h"
#অন্তর্ভাব "unicode/usprep.h"
#অন্তর্ভাব "unicode/udata.h"
#অন্তর্ভাব "utrie.h"
#অন্তর্ভাব "udataswp.h"
#অন্তর্ভাব "ubidi_props.h"

#define SPREP_তথ্য_TYPE "spp"

গণনা UStringPrepType{
    USPREP_UNASSIGNED           = 0x0000 ,
    USPREP_MAP                  = 0x0001 ,
    USPREP_PROHIBITED           = 0x0002 , 
    USPREP_DELETE               = 0x0003 ,
    USPREP_TYPE_LIMIT           = 0x0004  
};

প্রকারলেখ গণনা UStringPrepType UStringPrepType;

#ifdef USPREP_TYPE_NAMES_ARRAY
স্থির ধ্রুবক অক্ষর* usprepTypeNames[] ={
    "UNASSIGNED" ,          
    "MAP" , 
    "PROHIBITED" ,        
    "DELETE",
    "TYPE_LIMIT" 
};
#endif

গণনা{
    _SPREP_NORMALIZATION_ON = 0x0001,
    _SPREP_CHECK_BIDI_ON    = 0x0002
};

গণনা{
    _SPREP_TYPE_THRESHOLD       = 0xFFF0,
    SPREP_MAX_সূচক_মান      = 0x3FBF,   /*16139*/ 
    SPREP_MAX_সূচক_শীর্ষ_দৈর্ঘ্য = 0x0003
};

/* indexes[] value names */
গণনা {
    SPREP_সূচক_TRIE_আকার                  = 0, /* number of bytes in StringPrep trie */
    SPREP_সূচক_MAPPING_তথ্য_আকার          = 1, /* The array that contains the mapping   */
    _SPREP_NORM_CORRECTNS_LAST_UNI_VERSION  = 2, /* The index of Unicode version of last entry in NormalizationCorrections.txt */ 
    SPREP_ONE_UCHAR_MAPPING_সূচক_START    = 3, /* The starting index of 1 UChar mapping index in the mapping data array */
    SPREP_TWO_UCHARS_MAPPING_সূচক_START   = 4, /* The starting index of 2 UChars mapping index in the mapping data array */
    SPREP_THREE_UCHARS_MAPPING_সূচক_START = 5, /* The starting index of 3 UChars mapping index in the mapping data array */
    SPREP_FOUR_UCHARS_MAPPING_সূচক_START  = 6, /* The starting index of 4 UChars mapping index in the mapping data array */
    _SPREP_OPTIONS                          = 7, /* Bit set of options to turn on in the profile */
    SPREP_সূচক_শীর্ষ=16                          /* changing this requires a new formatVersion */
};

প্রকারলেখ কাঠামো UString_Prep_চাবি UString_Prep_চাবি;


কাঠামো UString_Prep_চাবি{
    অক্ষর* name;
    অক্ষর* path;
};

কাঠামো UStringPrepProfile{
    int32_t indexes[SPREP_সূচক_শীর্ষ];
    UTrie sprepTrie;
    ধ্রুবক uint16_t* mapping_তথ্য;
    UDataMemory* sprep_তথ্য;
    int32_t ref_গুনতি;
    UBool is_তথ্য_Loaded;
    UBool doNFKC;
    UBool checkBiDi;
};

/**
 * Helper function for populating the UParseError struct
 * @internal
 */
U_CAPI শূন্য U_EXPORT2
uprv_syntaxError(ধ্রুবক UChar* rules, 
                 int32_t pos,
                 int32_t rulesLen,
                 UParseError* parseError);


/**
 * Swap StringPrep .spp profile data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
usprep_swap(ধ্রুবক UDataSwapper *ds,
            ধ্রুবক শূন্য *in_তথ্য, int32_t দৈর্ঘ্য, শূন্য *out_তথ্য,
            UErrorCode *pErrorCode);

#endif /* #if !UCONFIG_NO_IDNA */

#endif

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
