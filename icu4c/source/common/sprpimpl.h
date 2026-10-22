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

#यदि_अपरिभाषितम् SPRPIMPL_H
#परिभाषय SPRPIMPL_H

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_IDNA

#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/parseerr.h"
#अन्तर्भाव "unicode/usprep.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "utrie.h"
#अन्तर्भाव "udataswp.h"
#अन्तर्भाव "ubidi_props.h"

#परिभाषय SPREP_दत्त_TYPE "spp"

गणना UStringPrepType{
    USPREP_UNASSIGNED           = 0x0000 ,
    USPREP_MAP                  = 0x0001 ,
    USPREP_PROHIBITED           = 0x0002 , 
    USPREP_DELETE               = 0x0003 ,
    USPREP_TYPE_सीमा           = 0x0004  
};

प्रकारनिर्देश गणना UStringPrepType UStringPrepType;

#यदि_परिभाषितम् USPREP_TYPE_नामानि_ARRAY
स्थिर ध्रुव वर्ण* usprep_Type_नामानि[] ={
    "UNASSIGNED" ,          
    "MAP" , 
    "PROHIBITED" ,        
    "DELETE",
    "TYPE_LIMIT" 
};
#यदि_अन्तः

गणना{
    _SPREP_NORMALIZATION_ON = 0x0001,
    _SPREP_CHECK_BIDI_ON    = 0x0002
};

गणना{
    SPREP_TYPE_देहली       = 0xFFF0,
    SPREP_MAX_सूचकांक_मानम्      = 0x3FBF,   /*16139*/ 
    SPREP_MAX_सूचकांक_शिखरम्_दीर्घता = 0x0003
};

/* indexes[] value names */
गणना {
    SPREP_सूचकांक_TRIE_आकार                  = 0, /* number of bytes in StringPrep trie */
    SPREP_सूचकांक_MAPPING_दत्त_आकार          = 1, /* The array that contains the mapping   */
    SPREP_NORM_CORRECTNS_चरमः_UNI_संस्करणम्  = 2, /* The index of Unicode version of last entry in NormalizationCorrections.txt */ 
    SPREP_ONE_UCHAR_MAPPING_सूचकांक_START    = 3, /* The starting index of 1 UChar mapping index in the mapping data array */
    SPREP_TWO_UCHARS_MAPPING_सूचकांक_START   = 4, /* The starting index of 2 UChars mapping index in the mapping data array */
    SPREP_THREE_UCHARS_MAPPING_सूचकांक_START = 5, /* The starting index of 3 UChars mapping index in the mapping data array */
    SPREP_FOUR_UCHARS_MAPPING_सूचकांक_START  = 6, /* The starting index of 4 UChars mapping index in the mapping data array */
    _SPREP_OPTIONS                          = 7, /* Bit set of options to turn on in the profile */
    SPREP_सूचकांक_शिखरम्=16                          /* changing this requires a new formatVersion */
};

प्रकारनिर्देश संरचना UString_Prep_कुञ्चिका UString_Prep_कुञ्चिका;


संरचना UString_Prep_कुञ्चिका{
    वर्ण* name;
    वर्ण* मार्गः;
};

संरचना UStringPrepProfile{
    int32_t indexes[SPREP_सूचकांक_शिखरम्];
    UTrie sprepTrie;
    ध्रुव uint16_t* mapping_दत्त;
    UData_स्मृतिः* sprep_दत्त;
    int32_t ref_गणनम्;
    UBool is_दत्त_Loaded;
    UBool doNFKC;
    UBool checkBiDi;
};

/**
 * Helper function for populating the UParseError struct
 * @internal
 */
U_CAPI शून्य U_EXPORT2
uprv_syntaxError(ध्रुव UChar* नियमाः, 
                 int32_t pos,
                 int32_t नियमाः_Len,
                 UParseError* parseError);


/**
 * Swap StringPrep .spp profile data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
usprep_swap(ध्रुव UDataSwapper *ds,
            ध्रुव शून्य *अन्तर्_दत्त, int32_t दीर्घता, शून्य *बहिर्गमनम्_दत्त,
            UError_संहिता *p_Error_संहिता);

#यदि_अन्तः /* #if !UCONFIG_NO_IDNA */

#यदि_अन्तः

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
