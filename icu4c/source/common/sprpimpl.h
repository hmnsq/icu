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

#که_نه_تعریف_شوی SPRPIMPL_H
#تعریف_کړه SPRPIMPL_H

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_IDNA

#ضمن "unicode/ustring.h"
#ضمن "unicode/parseerr.h"
#ضمن "unicode/usprep.h"
#ضمن "unicode/udata.h"
#ضمن "utrie.h"
#ضمن "udataswp.h"
#ضمن "ubidi_props.h"

#تعریف_کړه SPREP_معلومات_TYPE "spp"

تعداد UStringPrepType{
    USPREP_UNASSIGNED           = 0x0000 ,
    USPREP_MAP                  = 0x0001 ,
    USPREP_PROHIBITED           = 0x0002 , 
    USPREP_DELETE               = 0x0003 ,
    USPREP_TYPE_حد           = 0x0004  
};

تعريفنوع تعداد UStringPrepType UStringPrepType;

#که_تعریف_شوی USPREP_TYPE_نومونه_ARRAY
ساكن ثابت حرف* usprep_Type_نومونه[] ={
    "UNASSIGNED" ,          
    "MAP" , 
    "PROHIBITED" ,        
    "DELETE",
    "TYPE_LIMIT" 
};
#د_که_پای

تعداد{
    _SPREP_NORMALIZATION_ON = 0x0001,
    _SPREP_CHECK_BIDI_ON    = 0x0002
};

تعداد{
    SPREP_TYPE_پوله       = 0xFFF0,
    SPREP_MAX_شاخص_ارزښت      = 0x3FBF,   /*16139*/ 
    SPREP_MAX_شاخص_سر_اوږدتیا = 0x0003
};

/* indexes[] value names */
تعداد {
    SPREP_شاخص_TRIE_اندازه                  = 0, /* number of bytes in StringPrep trie */
    SPREP_شاخص_MAPPING_معلومات_اندازه          = 1, /* The array that contains the mapping   */
    SPREP_NORM_CORRECTNS_وروستی_UNI_نسخه  = 2, /* The index of Unicode version of last entry in NormalizationCorrections.txt */ 
    SPREP_ONE_UCHAR_MAPPING_شاخص_پیل    = 3, /* The starting index of 1 UChar mapping index in the mapping data array */
    SPREP_TWO_UCHARS_MAPPING_شاخص_پیل   = 4, /* The starting index of 2 UChars mapping index in the mapping data array */
    SPREP_THREE_UCHARS_MAPPING_شاخص_پیل = 5, /* The starting index of 3 UChars mapping index in the mapping data array */
    SPREP_FOUR_UCHARS_MAPPING_شاخص_پیل  = 6, /* The starting index of 4 UChars mapping index in the mapping data array */
    _SPREP_OPTIONS                          = 7, /* Bit set of options to turn on in the profile */
    SPREP_شاخص_سر=16                          /* changing this requires a new formatVersion */
};

تعريفنوع بنية UString_Prep_کلی UString_Prep_کلی;


بنية UString_Prep_کلی{
    حرف* name;
    حرف* لار;
};

بنية UStringPrepProfile{
    int32_t indexes[SPREP_شاخص_سر];
    UTrie sprepTrie;
    ثابت uint16_t* mapping_معلومات;
    UData_حافظه* sprep_معلومات;
    int32_t ref_شمېر;
    UBool is_معلومات_Loaded;
    UBool doNFKC;
    UBool checkBiDi;
};

/**
 * Helper function for populating the UParseError struct
 * @internal
 */
U_CAPI فراغ U_EXPORT2
uprv_syntaxError(ثابت UChar* قواعد, 
                 int32_t pos,
                 int32_t قواعد_Len,
                 UParseError* parseError);


/**
 * Swap StringPrep .spp profile data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
usprep_بدلون(ثابت UDataSwapper *ds,
            ثابت فراغ *دننه_معلومات, int32_t اوږدتیا, فراغ *بهر_معلومات,
            UError_کوډ *p_Error_کوډ);

#د_که_پای /* #if !UCONFIG_NO_IDNA */

#د_که_پای

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
