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

#اگر_غیر_تعریف_شدہ SPRPIMPL_H
#تعریف_کرو SPRPIMPL_H

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_IDNA

#شامل "unicode/ustring.h"
#شامل "unicode/parseerr.h"
#شامل "unicode/usprep.h"
#شامل "unicode/udata.h"
#شامل "utrie.h"
#شامل "udataswp.h"
#شامل "ubidi_props.h"

#تعریف_کرو SPREP_ڈیٹا_TYPE "spp"

شمار UStringPrepType{
    USPREP_UNASSIGNED           = 0x0000 ,
    USPREP_MAP                  = 0x0001 ,
    USPREP_PROHIBITED           = 0x0002 , 
    USPREP_DELETE               = 0x0003 ,
    USPREP_TYPE_حد_بندی           = 0x0004  
};

قسمتعریف شمار UStringPrepType UStringPrepType;

#اگر_تعریف_شدہ USPREP_TYPE_نام_ARRAY
ساکن ثابت حرف* usprep_Type_نام[] ={
    "UNASSIGNED" ,          
    "MAP" , 
    "PROHIBITED" ,        
    "DELETE",
    "TYPE_LIMIT" 
};
#اگر_ختم

شمار{
    _SPREP_NORMALIZATION_ON = 0x0001,
    _SPREP_CHECK_BIDI_ON    = 0x0002
};

شمار{
    SPREP_TYPE_حد       = 0xFFF0,
    SPREP_MAX_اشاریہ_قدر      = 0x3FBF,   /*16139*/ 
    SPREP_MAX_اشاریہ_چوٹی_لمبائی = 0x0003
};

/* indexes[] value names */
شمار {
    SPREP_اشاریہ_TRIE_سائز                  = 0, /* number of bytes in StringPrep trie */
    SPREP_اشاریہ_MAPPING_ڈیٹا_سائز          = 1, /* The array that contains the mapping   */
    SPREP_NORM_CORRECTNS_آخری_UNI_نسخہ  = 2, /* The index of Unicode version of last entry in NormalizationCorrections.txt */ 
    SPREP_ONE_UCHAR_MAPPING_اشاریہ_آغاز    = 3, /* The starting index of 1 UChar mapping index in the mapping data array */
    SPREP_TWO_UCHARS_MAPPING_اشاریہ_آغاز   = 4, /* The starting index of 2 UChars mapping index in the mapping data array */
    SPREP_THREE_UCHARS_MAPPING_اشاریہ_آغاز = 5, /* The starting index of 3 UChars mapping index in the mapping data array */
    SPREP_FOUR_UCHARS_MAPPING_اشاریہ_آغاز  = 6, /* The starting index of 4 UChars mapping index in the mapping data array */
    _SPREP_OPTIONS                          = 7, /* Bit set of options to turn on in the profile */
    SPREP_اشاریہ_چوٹی=16                          /* changing this requires a new formatVersion */
};

قسمتعریف ساخت UString_Prep_کلید UString_Prep_کلید;


ساخت UString_Prep_کلید{
    حرف* name;
    حرف* راستہ;
};

ساخت UStringPrepProfile{
    int32_t indexes[SPREP_اشاریہ_چوٹی];
    UTrie sprepTrie;
    ثابت uint16_t* mapping_ڈیٹا;
    UData_حافظہ* sprep_ڈیٹا;
    int32_t ref_گنتی;
    UBool is_ڈیٹا_Loaded;
    UBool doNFKC;
    UBool checkBiDi;
};

/**
 * Helper function for populating the UParseError struct
 * @internal
 */
U_CAPI خالی U_EXPORT2
uprv_syntaxError(ثابت UChar* قواعد, 
                 int32_t pos,
                 int32_t قواعد_Len,
                 UParseError* parseError);


/**
 * Swap StringPrep .spp profile data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
usprep_تبادلہ(ثابت UDataSwapper *ds,
            ثابت خالی *اندر_ڈیٹا, int32_t لمبائی, خالی *برآمد_ڈیٹا,
            UError_رمز *p_Error_رمز);

#اگر_ختم /* #if !UCONFIG_NO_IDNA */

#اگر_ختم

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
