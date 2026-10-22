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

#अगर_अघोषित SPRPIMPL_H
#घोषणा SPRPIMPL_H

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_IDNA

#समावेश "unicode/ustring.h"
#समावेश "unicode/parseerr.h"
#समावेश "unicode/usprep.h"
#समावेश "unicode/udata.h"
#समावेश "utrie.h"
#समावेश "udataswp.h"
#समावेश "ubidi_props.h"

#घोषणा SPREP_तथ्य_TYPE "spp"

क्रमागत UStringPrepType{
    USPREP_UNASSIGNED           = 0x0000 ,
    USPREP_MAP                  = 0x0001 ,
    USPREP_PROHIBITED           = 0x0002 , 
    USPREP_DELETE               = 0x0003 ,
    USPREP_TYPE_LIMIT           = 0x0004  
};

प्रकार क्रमागत UStringPrepType UStringPrepType;

#अगर_घोषित USPREP_TYPE_NAMES_ARRAY
अटल स्थिर अक्षर* usprepTypeNames[] ={
    "UNASSIGNED" ,          
    "MAP" , 
    "PROHIBITED" ,        
    "DELETE",
    "TYPE_LIMIT" 
};
#पूर्ण_अगर

क्रमागत{
    _SPREP_NORMALIZATION_ON = 0x0001,
    _SPREP_CHECK_BIDI_ON    = 0x0002
};

क्रमागत{
    _SPREP_TYPE_THRESHOLD       = 0xFFF0,
    SPREP_MAX_सूचकांक_मूल्य      = 0x3FBF,   /*16139*/ 
    SPREP_MAX_सूचकांक_शीर्ष_लंबाई = 0x0003
};

/* indexes[] value names */
क्रमागत {
    SPREP_सूचकांक_TRIE_आकार                  = 0, /* number of bytes in StringPrep trie */
    SPREP_सूचकांक_MAPPING_तथ्य_आकार          = 1, /* The array that contains the mapping   */
    _SPREP_NORM_CORRECTNS_LAST_UNI_VERSION  = 2, /* The index of Unicode version of last entry in NormalizationCorrections.txt */ 
    SPREP_ONE_UCHAR_MAPPING_सूचकांक_START    = 3, /* The starting index of 1 UChar mapping index in the mapping data array */
    SPREP_TWO_UCHARS_MAPPING_सूचकांक_START   = 4, /* The starting index of 2 UChars mapping index in the mapping data array */
    SPREP_THREE_UCHARS_MAPPING_सूचकांक_START = 5, /* The starting index of 3 UChars mapping index in the mapping data array */
    SPREP_FOUR_UCHARS_MAPPING_सूचकांक_START  = 6, /* The starting index of 4 UChars mapping index in the mapping data array */
    _SPREP_OPTIONS                          = 7, /* Bit set of options to turn on in the profile */
    SPREP_सूचकांक_शीर्ष=16                          /* changing this requires a new formatVersion */
};

प्रकार काष्ठा UString_Prep_कुंजी UString_Prep_कुंजी;


काष्ठा UString_Prep_कुंजी{
    अक्षर* name;
    अक्षर* path;
};

काष्ठा UStringPrepProfile{
    int32_t indexes[SPREP_सूचकांक_शीर्ष];
    UTrie sprepTrie;
    स्थिर uint16_t* mapping_तथ्य;
    UDataMemory* sprep_तथ्य;
    int32_t ref_गिनती;
    UBool is_तथ्य_Loaded;
    UBool doNFKC;
    UBool checkBiDi;
};

/**
 * Helper function for populating the UParseError struct
 * @internal
 */
U_CAPI व्योम U_EXPORT2
uprv_syntaxError(स्थिर UChar* rules, 
                 int32_t pos,
                 int32_t rulesLen,
                 UParseError* parseError);


/**
 * Swap StringPrep .spp profile data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
usprep_swap(स्थिर UDataSwapper *ds,
            स्थिर व्योम *in_तथ्य, int32_t लंबाई, व्योम *out_तथ्य,
            UErrorCode *pErrorCode);

#पूर्ण_अगर /* #if !UCONFIG_NO_IDNA */

#पूर्ण_अगर

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
