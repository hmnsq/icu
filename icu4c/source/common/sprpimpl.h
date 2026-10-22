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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_IDNA

#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/parseerr.h"
#അന്തര്ഭാവ "unicode/usprep.h"
#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "utrie.h"
#അന്തര്ഭാവ "udataswp.h"
#അന്തര്ഭാവ "ubidi_props.h"

#define _SPREP_DATA_TYPE "spp"

എണ്ണൽ UStringPrepType{
    USPREP_UNASSIGNED           = 0x0000 ,
    USPREP_MAP                  = 0x0001 ,
    USPREP_PROHIBITED           = 0x0002 , 
    USPREP_DELETE               = 0x0003 ,
    USPREP_TYPE_LIMIT           = 0x0004  
};

തരനിർവചനം എണ്ണൽ UStringPrepType UStringPrepType;

#ifdef USPREP_TYPE_NAMES_ARRAY
സ്ഥിതം സ്ഥിരം അക്ഷരം* usprepTypeNames[] ={
    "UNASSIGNED" ,          
    "MAP" , 
    "PROHIBITED" ,        
    "DELETE",
    "TYPE_LIMIT" 
};
#endif

എണ്ണൽ{
    _SPREP_NORMALIZATION_ON = 0x0001,
    _SPREP_CHECK_BIDI_ON    = 0x0002
};

എണ്ണൽ{
    _SPREP_TYPE_THRESHOLD       = 0xFFF0,
    SPREP_MAX_INDEX_മൂല്യം      = 0x3FBF,   /*16139*/ 
    SPREP_MAX_INDEX_മുകൾ_LENGTH = 0x0003
};

/* indexes[] value names */
എണ്ണൽ {
    _SPREP_INDEX_TRIE_SIZE                  = 0, /* number of bytes in StringPrep trie */
    _SPREP_INDEX_MAPPING_DATA_SIZE          = 1, /* The array that contains the mapping   */
    _SPREP_NORM_CORRECTNS_LAST_UNI_VERSION  = 2, /* The index of Unicode version of last entry in NormalizationCorrections.txt */ 
    _SPREP_ONE_UCHAR_MAPPING_INDEX_START    = 3, /* The starting index of 1 UChar mapping index in the mapping data array */
    _SPREP_TWO_UCHARS_MAPPING_INDEX_START   = 4, /* The starting index of 2 UChars mapping index in the mapping data array */
    _SPREP_THREE_UCHARS_MAPPING_INDEX_START = 5, /* The starting index of 3 UChars mapping index in the mapping data array */
    _SPREP_FOUR_UCHARS_MAPPING_INDEX_START  = 6, /* The starting index of 4 UChars mapping index in the mapping data array */
    _SPREP_OPTIONS                          = 7, /* Bit set of options to turn on in the profile */
    SPREP_INDEX_മുകൾ=16                          /* changing this requires a new formatVersion */
};

തരനിർവചനം ഘടന UString_Prep_താക്കോൽ UString_Prep_താക്കോൽ;


ഘടന UString_Prep_താക്കോൽ{
    അക്ഷരം* name;
    അക്ഷരം* path;
};

ഘടന UStringPrepProfile{
    int32_t indexes[SPREP_INDEX_മുകൾ];
    UTrie sprepTrie;
    സ്ഥിരം uint16_t* mappingData;
    UDataMemory* sprepData;
    int32_t ref_എണ്ണം;
    UBool isDataLoaded;
    UBool doNFKC;
    UBool checkBiDi;
};

/**
 * Helper function for populating the UParseError struct
 * @internal
 */
U_CAPI ശൂന്യം U_EXPORT2
uprv_syntaxError(സ്ഥിരം UChar* rules, 
                 int32_t pos,
                 int32_t rulesLen,
                 UParseError* parseError);


/**
 * Swap StringPrep .spp profile data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
usprep_swap(സ്ഥിരം UDataSwapper *ds,
            സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
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
