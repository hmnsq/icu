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

#جيڪڏهن_وصف_نه_ٿيل SPRPIMPL_H
#وصف_ڪريو SPRPIMPL_H

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_IDNA

#ضمن "unicode/ustring.h"
#ضمن "unicode/parseerr.h"
#ضمن "unicode/usprep.h"
#ضمن "unicode/udata.h"
#ضمن "utrie.h"
#ضمن "udataswp.h"
#ضمن "ubidi_props.h"

#وصف_ڪريو SPREP_معلومات_TYPE "spp"

تعداد UStringPrepType{
    USPREP_UNASSIGNED           = 0x0000 ,
    USPREP_MAP                  = 0x0001 ,
    USPREP_PROHIBITED           = 0x0002 , 
    USPREP_DELETE               = 0x0003 ,
    USPREP_TYPE_حد_بندي           = 0x0004  
};

تعريفنوع تعداد UStringPrepType UStringPrepType;

#جيڪڏهن_وصف_ٿيل USPREP_TYPE_نالا_ARRAY
ساكن ثابت حرف* usprep_Type_نالا[] ={
    "UNASSIGNED" ,          
    "MAP" , 
    "PROHIBITED" ,        
    "DELETE",
    "TYPE_LIMIT" 
};
#جيڪڏهن_پورو

تعداد{
    _SPREP_NORMALIZATION_ON = 0x0001,
    _SPREP_CHECK_BIDI_ON    = 0x0002
};

تعداد{
    SPREP_TYPE_حد       = 0xFFF0,
    SPREP_MAX_اشاريو_قدر      = 0x3FBF,   /*16139*/ 
    SPREP_MAX_اشاريو_چوٽي_ڊيگهائي = 0x0003
};

/* indexes[] value names */
تعداد {
    SPREP_اشاريو_TRIE_سائيز                  = 0, /* number of bytes in StringPrep trie */
    SPREP_اشاريو_MAPPING_معلومات_سائيز          = 1, /* The array that contains the mapping   */
    SPREP_NORM_CORRECTNS_آخري_UNI_نسخو  = 2, /* The index of Unicode version of last entry in NormalizationCorrections.txt */ 
    SPREP_ONE_UCHAR_MAPPING_اشاريو_شروعات    = 3, /* The starting index of 1 UChar mapping index in the mapping data array */
    SPREP_TWO_UCHARS_MAPPING_اشاريو_شروعات   = 4, /* The starting index of 2 UChars mapping index in the mapping data array */
    SPREP_THREE_UCHARS_MAPPING_اشاريو_شروعات = 5, /* The starting index of 3 UChars mapping index in the mapping data array */
    SPREP_FOUR_UCHARS_MAPPING_اشاريو_شروعات  = 6, /* The starting index of 4 UChars mapping index in the mapping data array */
    _SPREP_OPTIONS                          = 7, /* Bit set of options to turn on in the profile */
    SPREP_اشاريو_چوٽي=16                          /* changing this requires a new formatVersion */
};

تعريفنوع بنية UString_Prep_ڪنجي UString_Prep_ڪنجي;


بنية UString_Prep_ڪنجي{
    حرف* name;
    حرف* رستو;
};

بنية UStringPrepProfile{
    int32_t indexes[SPREP_اشاريو_چوٽي];
    UTrie sprepTrie;
    ثابت uint16_t* mapping_معلومات;
    UData_يادگيري* sprep_معلومات;
    int32_t ref_ڳڻپ;
    UBool is_معلومات_Loaded;
    UBool doNFKC;
    UBool checkBiDi;
};

/**
 * Helper function for populating the UParseError struct
 * @internal
 */
U_CAPI فراغ U_EXPORT2
uprv_syntaxError(ثابت UChar* قاعدا, 
                 int32_t pos,
                 int32_t قاعدا_Len,
                 UParseError* parseError);


/**
 * Swap StringPrep .spp profile data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
usprep_مٽا_سٽا(ثابت UDataSwapper *ds,
            ثابت فراغ *اندريون_معلومات, int32_t ڊيگهائي, فراغ *ٻاهريون_معلومات,
            UError_ڪوڊ *p_Error_ڪوڊ);

#جيڪڏهن_پورو /* #if !UCONFIG_NO_IDNA */

#جيڪڏهن_پورو

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
