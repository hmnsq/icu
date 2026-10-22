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

#अन्तर्भाव "unicode/utypes.h"

#जइ !UCONFIG_NO_IDNA

#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/parseerr.h"
#अन्तर्भाव "unicode/usprep.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "utrie.h"
#अन्तर्भाव "udataswp.h"
#अन्तर्भाव "ubidi_props.h"

#define _SPREP_DATA_TYPE "spp"

गणना UStringPrepType{
    USPREP_UNASSIGNED           = 0x0000 ,
    USPREP_MAP                  = 0x0001 ,
    USPREP_PROHIBITED           = 0x0002 , 
    USPREP_DELETE               = 0x0003 ,
    USPREP_TYPE_LIMIT           = 0x0004  
};

प्रकारनिर्देश गणना UStringPrepType UStringPrepType;

#ifdef USPREP_TYPE_NAMES_ARRAY
स्थिर ध्रुव वर्ण* usprepTypeNames[] ={
    "UNASSIGNED" ,          
    "MAP" , 
    "PROHIBITED" ,        
    "DELETE",
    "TYPE_LIMIT" 
};
#endif

गणना{
    _SPREP_NORMALIZATION_ON = 0x0001,
    _SPREP_CHECK_BIDI_ON    = 0x0002
};

गणना{
    _SPREP_TYPE_THRESHOLD       = 0xFFF0,
    SPREP_MAX_INDEX_मानम्      = 0x3FBF,   /*16139*/ 
    SPREP_MAX_INDEX_शिखरम्_LENGTH = 0x0003
};

/* indexes[] value names */
गणना {
    _SPREP_INDEX_TRIE_SIZE                  = 0, /* number of bytes in StringPrep trie */
    _SPREP_INDEX_MAPPING_DATA_SIZE          = 1, /* The array that contains the mapping   */
    _SPREP_NORM_CORRECTNS_LAST_UNI_VERSION  = 2, /* The index of Unicode version of last entry in NormalizationCorrections.txt */ 
    _SPREP_ONE_UCHAR_MAPPING_INDEX_START    = 3, /* The starting index of 1 UChar mapping index in the mapping data array */
    _SPREP_TWO_UCHARS_MAPPING_INDEX_START   = 4, /* The starting index of 2 UChars mapping index in the mapping data array */
    _SPREP_THREE_UCHARS_MAPPING_INDEX_START = 5, /* The starting index of 3 UChars mapping index in the mapping data array */
    _SPREP_FOUR_UCHARS_MAPPING_INDEX_START  = 6, /* The starting index of 4 UChars mapping index in the mapping data array */
    _SPREP_OPTIONS                          = 7, /* Bit set of options to turn on in the profile */
    SPREP_INDEX_शिखरम्=16                          /* changing this requires a new formatVersion */
};

प्रकारनिर्देश संरचना UString_Prep_कुञ्चिका UString_Prep_कुञ्चिका;


संरचना UString_Prep_कुञ्चिका{
    वर्ण* name;
    वर्ण* path;
};

संरचना UStringPrepProfile{
    int32_t indexes[SPREP_INDEX_शिखरम्];
    UTrie sprepTrie;
    ध्रुव uint16_t* mappingData;
    UDataMemory* sprepData;
    int32_t ref_गणनम्;
    UBool isDataLoaded;
    UBool doNFKC;
    UBool checkBiDi;
};

/**
 * Helper function for populating the UParseError struct
 * @internal
 */
U_CAPI शून्य U_EXPORT2
uprv_syntaxError(ध्रुव UChar* rules, 
                 int32_t pos,
                 int32_t rulesLen,
                 UParseError* parseError);


/**
 * Swap StringPrep .spp profile data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
usprep_swap(ध्रुव UDataSwapper *ds,
            ध्रुव शून्य *inData, int32_t length, शून्य *outData,
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
