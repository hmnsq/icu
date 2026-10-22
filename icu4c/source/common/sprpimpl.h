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

#جے_بنا_تعریف SPRPIMPL_H
#تعریف_کرو SPRPIMPL_H

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_IDNA

#ضمن "unicode/ustring.h"
#ضمن "unicode/parseerr.h"
#ضمن "unicode/usprep.h"
#ضمن "unicode/udata.h"
#ضمن "utrie.h"
#ضمن "udataswp.h"
#ضمن "ubidi_props.h"

#تعریف_کرو SPREP_انکڑے_TYPE "spp"

تعداد UStringPrepType{
    USPREP_UNASSIGNED           = 0x0000 ,
    USPREP_MAP                  = 0x0001 ,
    USPREP_PROHIBITED           = 0x0002 , 
    USPREP_DELETE               = 0x0003 ,
    USPREP_TYPE_حد_بندی           = 0x0004  
};

تعريفنوع تعداد UStringPrepType UStringPrepType;

#جے_تعریف_شدہ USPREP_TYPE_ناں_ARRAY
ساكن ثابت حرف* usprep_Type_ناں[] ={
    "UNASSIGNED" ,          
    "MAP" , 
    "PROHIBITED" ,        
    "DELETE",
    "TYPE_LIMIT" 
};
#جے_انت

تعداد{
    _SPREP_NORMALIZATION_ON = 0x0001,
    _SPREP_CHECK_BIDI_ON    = 0x0002
};

تعداد{
    SPREP_TYPE_حد       = 0xFFF0,
    SPREP_MAX_اشاریہ_قدر      = 0x3FBF,   /*16139*/ 
    SPREP_MAX_اشاریہ_چوٹی_لمائی = 0x0003
};

/* indexes[] value names */
تعداد {
    SPREP_اشاریہ_TRIE_ناپ                  = 0, /* number of bytes in StringPrep trie */
    SPREP_اشاریہ_MAPPING_انکڑے_ناپ          = 1, /* The array that contains the mapping   */
    SPREP_NORM_CORRECTNS_آخری_UNI_نسخہ  = 2, /* The index of Unicode version of last entry in NormalizationCorrections.txt */ 
    SPREP_ONE_UCHAR_MAPPING_اشاریہ_شروع    = 3, /* The starting index of 1 UChar mapping index in the mapping data array */
    SPREP_TWO_UCHARS_MAPPING_اشاریہ_شروع   = 4, /* The starting index of 2 UChars mapping index in the mapping data array */
    SPREP_THREE_UCHARS_MAPPING_اشاریہ_شروع = 5, /* The starting index of 3 UChars mapping index in the mapping data array */
    SPREP_FOUR_UCHARS_MAPPING_اشاریہ_شروع  = 6, /* The starting index of 4 UChars mapping index in the mapping data array */
    _SPREP_OPTIONS                          = 7, /* Bit set of options to turn on in the profile */
    SPREP_اشاریہ_چوٹی=16                          /* changing this requires a new formatVersion */
};

تعريفنوع بنية UString_Prep_کلید UString_Prep_کلید;


بنية UString_Prep_کلید{
    حرف* name;
    حرف* رستہ;
};

بنية UStringPrepProfile{
    int32_t indexes[SPREP_اشاریہ_چوٹی];
    UTrie sprepTrie;
    ثابت uint16_t* mapping_انکڑے;
    UData_حافظہ* sprep_انکڑے;
    int32_t ref_گنتی;
    UBool is_انکڑے_Loaded;
    UBool doNFKC;
    UBool checkBiDi;
};

/**
 * Helper function for populating the UParseError struct
 * @internal
 */
U_CAPI فراغ U_EXPORT2
uprv_syntaxError(ثابت UChar* قاعدے, 
                 int32_t pos,
                 int32_t قاعدے_Len,
                 UParseError* parseError);


/**
 * Swap StringPrep .spp profile data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
usprep_وٹاندرا(ثابت UDataSwapper *ds,
            ثابت فراغ *وچ_انکڑے, int32_t لمائی, فراغ *باہرلا_انکڑے,
            UError_کوڈ *p_Error_کوڈ);

#جے_انت /* #if !UCONFIG_NO_IDNA */

#جے_انت

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
