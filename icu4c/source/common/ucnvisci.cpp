// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2000-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  ucnvisci.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2001JUN26
*   created by: Ram Viswanadha
*
*   Date        Name        Description
*   24/7/2001   Ram         Added support for EXT character handling
*/

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ضمن "unicode/ucnv.h"
#ضمن "unicode/ucnv_cb.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "ucnv_bld.h"
#ضمن "ucnv_cnv.h"
#ضمن "cstring.h"
#ضمن "uassert.h"

#تعریف_کن UCNV_OPTIONS_نسخه_ماسک 0xf
#تعریف_کن NUKTA               0x093c
#تعریف_کن HALANT              0x094d
#تعریف_کن ZWNJ                0x200c /* Zero Width Non Joiner */
#تعریف_کن ZWJ                 0x200d /* Zero width Joiner */
#تعریف_کن INVALID_CHAR        0xffff
#تعریف_کن ATR                 0xEF   /* Attribute code */
#تعریف_کن EXT                 0xF0   /* Extension code */
#تعریف_کن DANDA               0x0964
#تعریف_کن DOUBLE_DANDA        0x0965
#تعریف_کن ISCII_NUKTA         0xE9
#تعریف_کن ISCII_HALANT        0xE8
#تعریف_کن ISCII_DANDA         0xEA
#تعریف_کن ISCII_INV           0xD9
#تعریف_کن ISCII_VOWEL_SIGN_E  0xE0
#تعریف_کن INDIC_بلوک_BEGIN   0x0900
#تعریف_کن INDIC_بلوک_انتها     0x0D7F
#تعریف_کن INDIC_RANGE         (INDIC_بلوک_انتها - INDIC_بلوک_BEGIN)
#تعریف_کن VOCALLIC_RR         0x0931
#تعریف_کن LF                  0x0A
#تعریف_کن ASCII_انتها           0xA0
#تعریف_کن NO_CHAR_MARKER      0xFFFE
#تعریف_کن TELUGU_DELTA        DELTA * TELUGU
#تعریف_کن DEV_ABBR_SIGN       0x0970
#تعریف_کن DEV_ANUDATTA        0x0952
#تعریف_کن EXT_RANGE_BEGIN     0xA1
#تعریف_کن EXT_RANGE_انتها       0xEE

#تعریف_کن PNJ_DELTA           0x0100
#تعریف_کن PNJ_BINDI           0x0A02
#تعریف_کن PNJ_TIPPI           0x0A70
#تعریف_کن PNJ_SIGN_VIRAMA     0x0A4D
#تعریف_کن PNJ_ADHAK           0x0A71
#تعریف_کن PNJ_HA              0x0A39
#تعریف_کن PNJ_RRA             0x0A5C

تعريفنوع تعداد {
    DEVANAGARI =0,
    BENGALI,
    GURMUKHI,
    GUJARATI,
    ORIYA,
    TAMIL,
    TELUGU,
    KANNADA,
    MALAYALAM,
    DELTA=0x80
}UniLang;

/**
 * Enumeration for switching code pages if <ATR>+<one of below values>
 * is encountered
 */
تعريفنوع تعداد {
    DEF = 0x40,
    RMN = 0x41,
    DEV = 0x42,
    BNG = 0x43,
    TML = 0x44,
    TLG = 0x45,
    ASM = 0x46,
    ORI = 0x47,
    KND = 0x48,
    MLM = 0x49,
    GJR = 0x4A,
    PNJ = 0x4B,
    ARB = 0x71,
    PES = 0x72,
    URD = 0x73,
    SND = 0x74,
    KSM = 0x75,
    PST = 0x76
}ISCIILang;

تعريفنوع تعداد {
    DEV_ماسک =0x80,
    PNJ_ماسک =0x40,
    GJR_ماسک =0x20,
    ORI_ماسک =0x10,
    BNG_ماسک =0x08,
    KND_ماسک =0x04,
    MLM_ماسک =0x02,
    TML_ماسک =0x01,
    ZERO =0x00
}ماسک_Enum;

#تعریف_کن ISCII_CNV_پیشوند "ISCII,version="

تعريفنوع بنية {
    char16_t context_Char_مقصد_Unicode;         /* previous Unicode codepoint for contextual analysis */
    char16_t contextCharFromUnicode;       /* previous Unicode codepoint for contextual analysis */
    uint16_t def_Delta_مقصد_Unicode;         /* delta for switching to default state when DEF is encountered  */
    uint16_t currentDeltaFromUnicode;   /* current delta in Indic block */
    uint16_t current_Delta_مقصد_Unicode;     /* current delta in Indic block */
    ماسک_Enum current_ماسک_From_Unicode;    /* mask for current state in toUnicode */
    ماسک_Enum current_ماسک_مقصد_Unicode;      /* mask for current state in toUnicode */
    ماسک_Enum def_ماسک_مقصد_Unicode;          /* mask for default state in toUnicode */
    UBool is_اولی_میانگیر;                /* boolean for fromUnicode to see if we need to announce the first script */
    UBool reset_مقصد_Default_مقصد_Unicode;      /* boolean for resetting to default delta and mask when a newline is encountered*/
    حرف name[حجم(ISCII_CNV_پیشوند) + 1];
    UChar32 prev_مقصد_Unicode_وضعیت;        /* Hold the previous toUnicodeStatus. This is necessary because we may need to know the last two code points. */
} UConverter_داده_ISCII;

تعريفنوع بنية Lookup_داده_Struct {
    UniLang uniLang;
    ماسک_Enum ماسک_Enum_2;
    ISCIILang isciiLang;
} Lookup_داده_Struct;

ساكن ثابت Lookup_داده_Struct lookup_Initial_داده[]={
    { DEVANAGARI, DEV_ماسک,  DEV },
    { BENGALI,    BNG_ماسک,  BNG },
    { GURMUKHI,   PNJ_ماسک,  PNJ },
    { GUJARATI,   GJR_ماسک,  GJR },
    { ORIYA,      ORI_ماسک,  ORI },
    { TAMIL,      TML_ماسک,  TML },
    { TELUGU,     KND_ماسک,  TLG },
    { KANNADA,    KND_ماسک,  KND },
    { MALAYALAM,  MLM_ماسک,  MLM }
};

/*
 * For special handling of certain Gurmukhi characters.
 * Bit 0 (value 1): PNJ consonant
 * Bit 1 (value 2): PNJ Bindi Tippi
 */
ساكن ثابت uint8_t pnjMap[80] = {
    /* 0A00..0A0F */
    0, 0, 0, 0, 0, 2, 0, 2,  0, 0, 0, 0, 0, 0, 0, 0,
    /* 0A10..0A1F */
    0, 0, 0, 0, 0, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3,
    /* 0A20..0A2F */
    3, 3, 3, 3, 3, 3, 3, 3,  3, 0, 3, 3, 3, 3, 3, 3,
    /* 0A30..0A3F */
    3, 0, 0, 0, 0, 3, 3, 0,  3, 3, 0, 0, 0, 0, 0, 2,
    /* 0A40..0A4F */
    0, 2, 2, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0
};

ساكن UBool
isPNJConsonant(UChar32 c) {
    إذا (c < 0xa00 || 0xa50 <= c) {
        أرجع خطأ;
    } وإلا {
        أرجع pnjMap[c - 0xa00] & 1;
    }
}

ساكن UBool
isPNJBindiTippi(UChar32 c) {
    إذا (c < 0xa00 || 0xa50 <= c) {
        أرجع خطأ;
    } وإلا {
        أرجع pnjMap[c - 0xa00] >> 1;
    }
}
U_CDECL_BEGIN
ساكن فراغ  U_CALLCONV
_ISCIIOpen(UConverter *cnv, UConverter_بارگیری_آرگومانها *p_آرگومانها, UError_کد *error_کد) {
    إذا(p_آرگومانها->onlyTestIsLoadable) {
        أرجع;
    }

    cnv->extraInfo = uprv_malloc(حجم(UConverter_داده_ISCII));

    إذا (cnv->extraInfo != اشاره_تهی) {
        int32_t len=0;
        UConverter_داده_ISCII *converter_داده=
                (UConverter_داده_ISCII *) cnv->extraInfo;
        converter_داده->context_Char_مقصد_Unicode=NO_CHAR_MARKER;
        cnv->مقصد_Unicode_وضعیت = missingCharMarker;
        converter_داده->contextCharFromUnicode=0x0000;
        converter_داده->reset_مقصد_Default_مقصد_Unicode=خطأ;
        /* check if the version requested is supported */
        إذا ((p_آرگومانها->options & UCNV_OPTIONS_نسخه_ماسک) < 9) {
            /* initialize state variables */
            converter_داده->currentDeltaFromUnicode
                    = converter_داده->current_Delta_مقصد_Unicode
                            = converter_داده->def_Delta_مقصد_Unicode = (uint16_t)(lookup_Initial_داده[p_آرگومانها->options & UCNV_OPTIONS_نسخه_ماسک].uniLang * DELTA);

            converter_داده->current_ماسک_From_Unicode
                    = converter_داده->current_ماسک_مقصد_Unicode
                            = converter_داده->def_ماسک_مقصد_Unicode = lookup_Initial_داده[p_آرگومانها->options & UCNV_OPTIONS_نسخه_ماسک].ماسک_Enum_2;
            
            converter_داده->is_اولی_میانگیر=درست;
            (فراغ)uprv_strcpy(converter_داده->name, ISCII_CNV_پیشوند);
            len = (int32_t)uprv_strlen(converter_داده->name);
            converter_داده->name[len]= (حرف)((p_آرگومانها->options & UCNV_OPTIONS_نسخه_ماسک) + '0');
            converter_داده->name[len+1]=0;
            
            converter_داده->prev_مقصد_Unicode_وضعیت = 0x0000;
        } وإلا {
            uprv_free(cnv->extraInfo);
            cnv->extraInfo = اشاره_تهی;
            *error_کد = U_ILLEGAL_آرگومان_ERROR;
        }

    } وإلا {
        *error_کد =U_حافظه_تخصیص_ERROR;
    }
}

ساكن فراغ U_CALLCONV
_ISCIIClose(UConverter *cnv) {
    إذا (cnv->extraInfo!=اشاره_تهی) {
        إذا (!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo=اشاره_تهی;
    }
}

ساكن ثابت حرف*  U_CALLCONV
_ISCIIgetName(ثابت UConverter* cnv) {
    إذا (cnv->extraInfo) {
        UConverter_داده_ISCII* my_داده= (UConverter_داده_ISCII*)cnv->extraInfo;
        أرجع my_داده->name;
    }
    أرجع اشاره_تهی;
}

ساكن فراغ U_CALLCONV
_ISCIIReset(UConverter *cnv, UConverterResetChoice choice) {
    UConverter_داده_ISCII* داده =(UConverter_داده_ISCII *) (cnv->extraInfo);
    إذا (choice<=UCNV_RESET_مقصد_UNICODE) {
        cnv->مقصد_Unicode_وضعیت = missingCharMarker;
        cnv->نما=0;
        داده->current_Delta_مقصد_Unicode=داده->def_Delta_مقصد_Unicode;
        داده->current_ماسک_مقصد_Unicode = داده->def_ماسک_مقصد_Unicode;
        داده->context_Char_مقصد_Unicode=NO_CHAR_MARKER;
        داده->prev_مقصد_Unicode_وضعیت = 0x0000;
    }
    إذا (choice!=UCNV_RESET_مقصد_UNICODE) {
        cnv->fromUChar32=0x0000;
        داده->contextCharFromUnicode=0x00;
        داده->current_ماسک_From_Unicode=داده->def_ماسک_مقصد_Unicode;
        داده->currentDeltaFromUnicode=داده->def_Delta_مقصد_Unicode;
        داده->is_اولی_میانگیر=درست;
        داده->reset_مقصد_Default_مقصد_Unicode=خطأ;
    }
}

/**
 * The values in validity table are indexed by the lower bits of Unicode
 * range 0x0900 - 0x09ff. The values have a structure like:
 *       ---------------------------------------------------------------
 *      | DEV   | PNJ   | GJR   | ORI   | BNG   | TLG   | MLM   | TML   |
 *      |       |       |       |       | ASM   | KND   |       |       |
 *       ---------------------------------------------------------------
 * If a code point is valid in a particular script
 * then that bit is turned on
 *
 * Unicode does not distinguish between Bengali and Assamese so we use 1 bit for
 * to represent these languages
 *
 * Telugu and Kannada have same codepoints except for Vocallic_RR which we special case
 * and combine and use 1 bit to represent these languages.
 *
 * TODO: It is probably easier to understand and maintain to change this
 * to use uint16_t and give each of the 9 Unicode/script blocks its own bit.
 */

ساكن ثابت uint8_t validity_جدول[128] = {
/* This state table is tool generated please do not edit unless you know exactly what you are doing */
/* Note: This table was edited to mirror the Windows XP implementation */
/*ISCII:Valid:Unicode */
/*0xa0 : 0x00: 0x900  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xa1 : 0xb8: 0x901  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + ZERO     + ZERO     + ZERO     ,
/*0xa2 : 0xfe: 0x902  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xa3 : 0xbf: 0x903  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0x00 : 0x00: 0x904  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xa4 : 0xff: 0x905  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xa5 : 0xff: 0x906  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xa6 : 0xff: 0x907  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xa7 : 0xff: 0x908  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xa8 : 0xff: 0x909  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xa9 : 0xff: 0x90a  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xaa : 0xfe: 0x90b  */ DEV_ماسک + ZERO     + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0x00 : 0x00: 0x90c  */ DEV_ماسک + ZERO     + ZERO     + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0xae : 0x80: 0x90d  */ DEV_ماسک + ZERO     + GJR_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xab : 0x87: 0x90e  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xac : 0xff: 0x90f  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xad : 0xff: 0x910  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xb2 : 0x80: 0x911  */ DEV_ماسک + ZERO     + GJR_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xaf : 0x87: 0x912  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xb0 : 0xff: 0x913  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xb1 : 0xff: 0x914  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xb3 : 0xff: 0x915  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xb4 : 0xfe: 0x916  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0xb5 : 0xfe: 0x917  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0xb6 : 0xfe: 0x918  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0xb7 : 0xff: 0x919  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xb8 : 0xff: 0x91a  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xb9 : 0xfe: 0x91b  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0xba : 0xff: 0x91c  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xbb : 0xfe: 0x91d  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0xbc : 0xff: 0x91e  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xbd : 0xff: 0x91f  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xbe : 0xfe: 0x920  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0xbf : 0xfe: 0x921  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0xc0 : 0xfe: 0x922  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0xc1 : 0xff: 0x923  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xc2 : 0xff: 0x924  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xc3 : 0xfe: 0x925  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0xc4 : 0xfe: 0x926  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0xc5 : 0xfe: 0x927  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0xc6 : 0xff: 0x928  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xc7 : 0x81: 0x929  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + TML_ماسک ,
/*0xc8 : 0xff: 0x92a  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xc9 : 0xfe: 0x92b  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0xca : 0xfe: 0x92c  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0xcb : 0xfe: 0x92d  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0xcc : 0xfe: 0x92e  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xcd : 0xff: 0x92f  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xcf : 0xff: 0x930  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xd0 : 0x87: 0x931  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + MLM_ماسک + TML_ماسک ,
/*0xd1 : 0xff: 0x932  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xd2 : 0xb7: 0x933  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + ZERO     + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xd3 : 0x83: 0x934  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + MLM_ماسک + TML_ماسک ,
/*0xd4 : 0xff: 0x935  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + ZERO     + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xd5 : 0xfe: 0x936  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0xd6 : 0xbf: 0x937  */ DEV_ماسک + ZERO     + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xd7 : 0xff: 0x938  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xd8 : 0xff: 0x939  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0x00 : 0x00: 0x93A  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x93B  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe9 : 0xda: 0x93c  */ DEV_ماسک + PNJ_ماسک + ZERO     + ORI_ماسک + BNG_ماسک + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x93d  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xda : 0xff: 0x93e  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xdb : 0xff: 0x93f  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xdc : 0xff: 0x940  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xdd : 0xff: 0x941  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xde : 0xff: 0x942  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xdf : 0xbe: 0x943  */ DEV_ماسک + ZERO     + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0x00 : 0x00: 0x944  */ DEV_ماسک + ZERO     + GJR_ماسک + ZERO     + BNG_ماسک + KND_ماسک + ZERO     + ZERO     ,
/*0xe3 : 0x80: 0x945  */ DEV_ماسک + ZERO     + GJR_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe0 : 0x87: 0x946  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xe1 : 0xff: 0x947  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xe2 : 0xff: 0x948  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xe7 : 0x80: 0x949  */ DEV_ماسک + ZERO     + GJR_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe4 : 0x87: 0x94a  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xe5 : 0xff: 0x94b  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xe6 : 0xff: 0x94c  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xe8 : 0xff: 0x94d  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xec : 0x00: 0x94e  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xed : 0x00: 0x94f  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x950  */ DEV_ماسک + ZERO     + GJR_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x951  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x952  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x953  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x954  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x955  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + KND_ماسک + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x956  */ ZERO     + ZERO     + ZERO     + ORI_ماسک + ZERO     + KND_ماسک + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x957  */ ZERO     + ZERO     + ZERO     + ORI_ماسک + BNG_ماسک + ZERO     + MLM_ماسک + ZERO     ,
/*0x00 : 0x00: 0x958  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x959  */ DEV_ماسک + PNJ_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95a  */ DEV_ماسک + PNJ_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95b  */ DEV_ماسک + PNJ_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95c  */ DEV_ماسک + PNJ_ماسک + ZERO     + ZERO     + BNG_ماسک + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95d  */ DEV_ماسک + ZERO     + ZERO     + ORI_ماسک + BNG_ماسک + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95e  */ DEV_ماسک + PNJ_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xce : 0x98: 0x95f  */ DEV_ماسک + ZERO     + ZERO     + ORI_ماسک + BNG_ماسک + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x960  */ DEV_ماسک + ZERO     + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0x00 : 0x00: 0x961  */ DEV_ماسک + ZERO     + ZERO     + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + ZERO     ,
/*0x00 : 0x00: 0x962  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + BNG_ماسک + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x963  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + BNG_ماسک + ZERO     + ZERO     + ZERO     ,
/*0xea : 0xf8: 0x964  */ DEV_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xeaea : 0x00: 0x965*/ DEV_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xf1 : 0xff: 0x966  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xf2 : 0xff: 0x967  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xf3 : 0xff: 0x968  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xf4 : 0xff: 0x969  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xf5 : 0xff: 0x96a  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xf6 : 0xff: 0x96b  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xf7 : 0xff: 0x96c  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xf8 : 0xff: 0x96d  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xf9 : 0xff: 0x96e  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0xfa : 0xff: 0x96f  */ DEV_ماسک + PNJ_ماسک + GJR_ماسک + ORI_ماسک + BNG_ماسک + KND_ماسک + MLM_ماسک + TML_ماسک ,
/*0x00 : 0x80: 0x970  */ DEV_ماسک + PNJ_ماسک + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*
 * The length of the array is 128 to provide values for 0x900..0x97f.
 * The last 15 entries for 0x971..0x97f of the validity table are all zero
 * because no Indic script uses such Unicode code points.
 */
/*0x00 : 0x00: 0x9yz  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO
};

ساكن ثابت uint16_t from_Unicode_جدول[128]={
    0x00a0 ,/* 0x0900 */
    0x00a1 ,/* 0x0901 */
    0x00a2 ,/* 0x0902 */
    0x00a3 ,/* 0x0903 */
    0xa4e0 ,/* 0x0904 */
    0x00a4 ,/* 0x0905 */
    0x00a5 ,/* 0x0906 */
    0x00a6 ,/* 0x0907 */
    0x00a7 ,/* 0x0908 */
    0x00a8 ,/* 0x0909 */
    0x00a9 ,/* 0x090a */
    0x00aa ,/* 0x090b */
    0xA6E9 ,/* 0x090c */
    0x00ae ,/* 0x090d */
    0x00ab ,/* 0x090e */
    0x00ac ,/* 0x090f */
    0x00ad ,/* 0x0910 */
    0x00b2 ,/* 0x0911 */
    0x00af ,/* 0x0912 */
    0x00b0 ,/* 0x0913 */
    0x00b1 ,/* 0x0914 */
    0x00b3 ,/* 0x0915 */
    0x00b4 ,/* 0x0916 */
    0x00b5 ,/* 0x0917 */
    0x00b6 ,/* 0x0918 */
    0x00b7 ,/* 0x0919 */
    0x00b8 ,/* 0x091a */
    0x00b9 ,/* 0x091b */
    0x00ba ,/* 0x091c */
    0x00bb ,/* 0x091d */
    0x00bc ,/* 0x091e */
    0x00bd ,/* 0x091f */
    0x00be ,/* 0x0920 */
    0x00bf ,/* 0x0921 */
    0x00c0 ,/* 0x0922 */
    0x00c1 ,/* 0x0923 */
    0x00c2 ,/* 0x0924 */
    0x00c3 ,/* 0x0925 */
    0x00c4 ,/* 0x0926 */
    0x00c5 ,/* 0x0927 */
    0x00c6 ,/* 0x0928 */
    0x00c7 ,/* 0x0929 */
    0x00c8 ,/* 0x092a */
    0x00c9 ,/* 0x092b */
    0x00ca ,/* 0x092c */
    0x00cb ,/* 0x092d */
    0x00cc ,/* 0x092e */
    0x00cd ,/* 0x092f */
    0x00cf ,/* 0x0930 */
    0x00d0 ,/* 0x0931 */
    0x00d1 ,/* 0x0932 */
    0x00d2 ,/* 0x0933 */
    0x00d3 ,/* 0x0934 */
    0x00d4 ,/* 0x0935 */
    0x00d5 ,/* 0x0936 */
    0x00d6 ,/* 0x0937 */
    0x00d7 ,/* 0x0938 */
    0x00d8 ,/* 0x0939 */
    0xFFFF ,/* 0x093A */
    0xFFFF ,/* 0x093B */
    0x00e9 ,/* 0x093c */
    0xEAE9 ,/* 0x093d */
    0x00da ,/* 0x093e */
    0x00db ,/* 0x093f */
    0x00dc ,/* 0x0940 */
    0x00dd ,/* 0x0941 */
    0x00de ,/* 0x0942 */
    0x00df ,/* 0x0943 */
    0xDFE9 ,/* 0x0944 */
    0x00e3 ,/* 0x0945 */
    0x00e0 ,/* 0x0946 */
    0x00e1 ,/* 0x0947 */
    0x00e2 ,/* 0x0948 */
    0x00e7 ,/* 0x0949 */
    0x00e4 ,/* 0x094a */
    0x00e5 ,/* 0x094b */
    0x00e6 ,/* 0x094c */
    0x00e8 ,/* 0x094d */
    0x00ec ,/* 0x094e */
    0x00ed ,/* 0x094f */
    0xA1E9 ,/* 0x0950 */ /* OM Symbol */
    0xFFFF ,/* 0x0951 */
    0xF0B8 ,/* 0x0952 */
    0xFFFF ,/* 0x0953 */
    0xFFFF ,/* 0x0954 */
    0xFFFF ,/* 0x0955 */
    0xFFFF ,/* 0x0956 */
    0xFFFF ,/* 0x0957 */
    0xb3e9 ,/* 0x0958 */
    0xb4e9 ,/* 0x0959 */
    0xb5e9 ,/* 0x095a */
    0xbae9 ,/* 0x095b */
    0xbfe9 ,/* 0x095c */
    0xC0E9 ,/* 0x095d */
    0xc9e9 ,/* 0x095e */
    0x00ce ,/* 0x095f */
    0xAAe9 ,/* 0x0960 */
    0xA7E9 ,/* 0x0961 */
    0xDBE9 ,/* 0x0962 */
    0xDCE9 ,/* 0x0963 */
    0x00ea ,/* 0x0964 */
    0xeaea ,/* 0x0965 */
    0x00f1 ,/* 0x0966 */
    0x00f2 ,/* 0x0967 */
    0x00f3 ,/* 0x0968 */
    0x00f4 ,/* 0x0969 */
    0x00f5 ,/* 0x096a */
    0x00f6 ,/* 0x096b */
    0x00f7 ,/* 0x096c */
    0x00f8 ,/* 0x096d */
    0x00f9 ,/* 0x096e */
    0x00fa ,/* 0x096f */
    0xF0BF ,/* 0x0970 */
    0xFFFF ,/* 0x0971 */
    0xFFFF ,/* 0x0972 */
    0xFFFF ,/* 0x0973 */
    0xFFFF ,/* 0x0974 */
    0xFFFF ,/* 0x0975 */
    0xFFFF ,/* 0x0976 */
    0xFFFF ,/* 0x0977 */
    0xFFFF ,/* 0x0978 */
    0xFFFF ,/* 0x0979 */
    0xFFFF ,/* 0x097a */
    0xFFFF ,/* 0x097b */
    0xFFFF ,/* 0x097c */
    0xFFFF ,/* 0x097d */
    0xFFFF ,/* 0x097e */
    0xFFFF ,/* 0x097f */
};
ساكن ثابت uint16_t مقصد_Unicode_جدول[256]={
    0x0000,/* 0x00 */
    0x0001,/* 0x01 */
    0x0002,/* 0x02 */
    0x0003,/* 0x03 */
    0x0004,/* 0x04 */
    0x0005,/* 0x05 */
    0x0006,/* 0x06 */
    0x0007,/* 0x07 */
    0x0008,/* 0x08 */
    0x0009,/* 0x09 */
    0x000a,/* 0x0a */
    0x000b,/* 0x0b */
    0x000c,/* 0x0c */
    0x000d,/* 0x0d */
    0x000e,/* 0x0e */
    0x000f,/* 0x0f */
    0x0010,/* 0x10 */
    0x0011,/* 0x11 */
    0x0012,/* 0x12 */
    0x0013,/* 0x13 */
    0x0014,/* 0x14 */
    0x0015,/* 0x15 */
    0x0016,/* 0x16 */
    0x0017,/* 0x17 */
    0x0018,/* 0x18 */
    0x0019,/* 0x19 */
    0x001a,/* 0x1a */
    0x001b,/* 0x1b */
    0x001c,/* 0x1c */
    0x001d,/* 0x1d */
    0x001e,/* 0x1e */
    0x001f,/* 0x1f */
    0x0020,/* 0x20 */
    0x0021,/* 0x21 */
    0x0022,/* 0x22 */
    0x0023,/* 0x23 */
    0x0024,/* 0x24 */
    0x0025,/* 0x25 */
    0x0026,/* 0x26 */
    0x0027,/* 0x27 */
    0x0028,/* 0x28 */
    0x0029,/* 0x29 */
    0x002a,/* 0x2a */
    0x002b,/* 0x2b */
    0x002c,/* 0x2c */
    0x002d,/* 0x2d */
    0x002e,/* 0x2e */
    0x002f,/* 0x2f */
    0x0030,/* 0x30 */
    0x0031,/* 0x31 */
    0x0032,/* 0x32 */
    0x0033,/* 0x33 */
    0x0034,/* 0x34 */
    0x0035,/* 0x35 */
    0x0036,/* 0x36 */
    0x0037,/* 0x37 */
    0x0038,/* 0x38 */
    0x0039,/* 0x39 */
    0x003A,/* 0x3A */
    0x003B,/* 0x3B */
    0x003c,/* 0x3c */
    0x003d,/* 0x3d */
    0x003e,/* 0x3e */
    0x003f,/* 0x3f */
    0x0040,/* 0x40 */
    0x0041,/* 0x41 */
    0x0042,/* 0x42 */
    0x0043,/* 0x43 */
    0x0044,/* 0x44 */
    0x0045,/* 0x45 */
    0x0046,/* 0x46 */
    0x0047,/* 0x47 */
    0x0048,/* 0x48 */
    0x0049,/* 0x49 */
    0x004a,/* 0x4a */
    0x004b,/* 0x4b */
    0x004c,/* 0x4c */
    0x004d,/* 0x4d */
    0x004e,/* 0x4e */
    0x004f,/* 0x4f */
    0x0050,/* 0x50 */
    0x0051,/* 0x51 */
    0x0052,/* 0x52 */
    0x0053,/* 0x53 */
    0x0054,/* 0x54 */
    0x0055,/* 0x55 */
    0x0056,/* 0x56 */
    0x0057,/* 0x57 */
    0x0058,/* 0x58 */
    0x0059,/* 0x59 */
    0x005a,/* 0x5a */
    0x005b,/* 0x5b */
    0x005c,/* 0x5c */
    0x005d,/* 0x5d */
    0x005e,/* 0x5e */
    0x005f,/* 0x5f */
    0x0060,/* 0x60 */
    0x0061,/* 0x61 */
    0x0062,/* 0x62 */
    0x0063,/* 0x63 */
    0x0064,/* 0x64 */
    0x0065,/* 0x65 */
    0x0066,/* 0x66 */
    0x0067,/* 0x67 */
    0x0068,/* 0x68 */
    0x0069,/* 0x69 */
    0x006a,/* 0x6a */
    0x006b,/* 0x6b */
    0x006c,/* 0x6c */
    0x006d,/* 0x6d */
    0x006e,/* 0x6e */
    0x006f,/* 0x6f */
    0x0070,/* 0x70 */
    0x0071,/* 0x71 */
    0x0072,/* 0x72 */
    0x0073,/* 0x73 */
    0x0074,/* 0x74 */
    0x0075,/* 0x75 */
    0x0076,/* 0x76 */
    0x0077,/* 0x77 */
    0x0078,/* 0x78 */
    0x0079,/* 0x79 */
    0x007a,/* 0x7a */
    0x007b,/* 0x7b */
    0x007c,/* 0x7c */
    0x007d,/* 0x7d */
    0x007e,/* 0x7e */
    0x007f,/* 0x7f */
    0x0080,/* 0x80 */
    0x0081,/* 0x81 */
    0x0082,/* 0x82 */
    0x0083,/* 0x83 */
    0x0084,/* 0x84 */
    0x0085,/* 0x85 */
    0x0086,/* 0x86 */
    0x0087,/* 0x87 */
    0x0088,/* 0x88 */
    0x0089,/* 0x89 */
    0x008a,/* 0x8a */
    0x008b,/* 0x8b */
    0x008c,/* 0x8c */
    0x008d,/* 0x8d */
    0x008e,/* 0x8e */
    0x008f,/* 0x8f */
    0x0090,/* 0x90 */
    0x0091,/* 0x91 */
    0x0092,/* 0x92 */
    0x0093,/* 0x93 */
    0x0094,/* 0x94 */
    0x0095,/* 0x95 */
    0x0096,/* 0x96 */
    0x0097,/* 0x97 */
    0x0098,/* 0x98 */
    0x0099,/* 0x99 */
    0x009a,/* 0x9a */
    0x009b,/* 0x9b */
    0x009c,/* 0x9c */
    0x009d,/* 0x9d */
    0x009e,/* 0x9e */
    0x009f,/* 0x9f */
    0x00A0,/* 0xa0 */
    0x0901,/* 0xa1 */
    0x0902,/* 0xa2 */
    0x0903,/* 0xa3 */
    0x0905,/* 0xa4 */
    0x0906,/* 0xa5 */
    0x0907,/* 0xa6 */
    0x0908,/* 0xa7 */
    0x0909,/* 0xa8 */
    0x090a,/* 0xa9 */
    0x090b,/* 0xaa */
    0x090e,/* 0xab */
    0x090f,/* 0xac */
    0x0910,/* 0xad */
    0x090d,/* 0xae */
    0x0912,/* 0xaf */
    0x0913,/* 0xb0 */
    0x0914,/* 0xb1 */
    0x0911,/* 0xb2 */
    0x0915,/* 0xb3 */
    0x0916,/* 0xb4 */
    0x0917,/* 0xb5 */
    0x0918,/* 0xb6 */
    0x0919,/* 0xb7 */
    0x091a,/* 0xb8 */
    0x091b,/* 0xb9 */
    0x091c,/* 0xba */
    0x091d,/* 0xbb */
    0x091e,/* 0xbc */
    0x091f,/* 0xbd */
    0x0920,/* 0xbe */
    0x0921,/* 0xbf */
    0x0922,/* 0xc0 */
    0x0923,/* 0xc1 */
    0x0924,/* 0xc2 */
    0x0925,/* 0xc3 */
    0x0926,/* 0xc4 */
    0x0927,/* 0xc5 */
    0x0928,/* 0xc6 */
    0x0929,/* 0xc7 */
    0x092a,/* 0xc8 */
    0x092b,/* 0xc9 */
    0x092c,/* 0xca */
    0x092d,/* 0xcb */
    0x092e,/* 0xcc */
    0x092f,/* 0xcd */
    0x095f,/* 0xce */
    0x0930,/* 0xcf */
    0x0931,/* 0xd0 */
    0x0932,/* 0xd1 */
    0x0933,/* 0xd2 */
    0x0934,/* 0xd3 */
    0x0935,/* 0xd4 */
    0x0936,/* 0xd5 */
    0x0937,/* 0xd6 */
    0x0938,/* 0xd7 */
    0x0939,/* 0xd8 */
    0x200D,/* 0xd9 */
    0x093e,/* 0xda */
    0x093f,/* 0xdb */
    0x0940,/* 0xdc */
    0x0941,/* 0xdd */
    0x0942,/* 0xde */
    0x0943,/* 0xdf */
    0x0946,/* 0xe0 */
    0x0947,/* 0xe1 */
    0x0948,/* 0xe2 */
    0x0945,/* 0xe3 */
    0x094a,/* 0xe4 */
    0x094b,/* 0xe5 */
    0x094c,/* 0xe6 */
    0x0949,/* 0xe7 */
    0x094d,/* 0xe8 */
    0x093c,/* 0xe9 */
    0x0964,/* 0xea */
    0xFFFF,/* 0xeb */
    0xFFFF,/* 0xec */
    0xFFFF,/* 0xed */
    0xFFFF,/* 0xee */
    0xFFFF,/* 0xef */
    0xFFFF,/* 0xf0 */
    0x0966,/* 0xf1 */
    0x0967,/* 0xf2 */
    0x0968,/* 0xf3 */
    0x0969,/* 0xf4 */
    0x096a,/* 0xf5 */
    0x096b,/* 0xf6 */
    0x096c,/* 0xf7 */
    0x096d,/* 0xf8 */
    0x096e,/* 0xf9 */
    0x096f,/* 0xfa */
    0xFFFF,/* 0xfb */
    0xFFFF,/* 0xfc */
    0xFFFF,/* 0xfd */
    0xFFFF,/* 0xfe */
    0xFFFF /* 0xff */
};

ساكن ثابت uint16_t vowelSignESpecialCases[][2]={
	{ 2 /*length of array*/    , 0      },
	{ 0xA4 , 0x0904 },
};

ساكن ثابت uint16_t nuktaSpecialCases[][2]={
    { 16 /*length of array*/   , 0      },
    { 0xA6 , 0x090c },
    { 0xEA , 0x093D },
    { 0xDF , 0x0944 },
    { 0xA1 , 0x0950 },
    { 0xb3 , 0x0958 },
    { 0xb4 , 0x0959 },
    { 0xb5 , 0x095a },
    { 0xba , 0x095b },
    { 0xbf , 0x095c },
    { 0xC0 , 0x095d },
    { 0xc9 , 0x095e },
    { 0xAA , 0x0960 },
    { 0xA7 , 0x0961 },
    { 0xDB , 0x0962 },
    { 0xDC , 0x0963 },
};


#تعریف_کن WRITE_مقصد_هدف_FROM_U(آرگومانها_2,offsets,منبع,هدف,هدف_حد,هدف_Byte_Unit,err) UPRV_بلوک_MACRO_BEGIN { \
    int32_t جابه_جایی = (int32_t)(منبع - آرگومانها_2->منبع-1);                                        \
      /* write the targetUniChar  to target */                                                  \
    إذا(هدف < هدف_حد){                                                                   \
        إذا(هدف_Byte_Unit <= 0xFF){                                                             \
            *(هدف)++ = (uint8_t)(هدف_Byte_Unit);                                            \
            إذا(offsets){                                                                        \
                *(offsets++) = جابه_جایی;                                                          \
            }                                                                                   \
        }وإلا{                                                                                  \
            إذا (هدف_Byte_Unit > 0xFFFF) {                                                      \
                *(هدف)++ = (uint8_t)(هدف_Byte_Unit>>16);                                    \
                إذا (offsets) {                                                                  \
                    --جابه_جایی;                                                                   \
                    *(offsets++) = جابه_جایی;                                                      \
                }                                                                               \
            }                                                                                   \
            إذا (!(هدف < هدف_حد)) {                                                      \
                آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] =    \
                                (uint8_t)(هدف_Byte_Unit >> 8);                                 \
                آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] =    \
                                (uint8_t)هدف_Byte_Unit;                                        \
                *err = U_میانگیر_سرریز_ERROR;                                                 \
            } وإلا {                                                                            \
                *(هدف)++ = (uint8_t)(هدف_Byte_Unit>>8);                                     \
                إذا(offsets){                                                                    \
                    *(offsets++) = جابه_جایی;                                                      \
                }                                                                               \
                إذا(هدف < هدف_حد){                                                       \
                    *(هدف)++ = (uint8_t)  هدف_Byte_Unit;                                    \
                    إذا(offsets){                                                                \
                        *(offsets++) = جابه_جایی                            ;                      \
                    }                                                                           \
                }وإلا{                                                                          \
                    آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] =\
                                (uint8_t) (هدف_Byte_Unit);                                     \
                    *err = U_میانگیر_سرریز_ERROR;                                             \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
    }وإلا{                                                                                      \
        إذا (هدف_Byte_Unit & 0xFF0000) {                                                        \
            آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] =        \
                        (uint8_t) (هدف_Byte_Unit >>16);                                        \
        }                                                                                       \
        إذا(هدف_Byte_Unit & 0xFF00){                                                            \
            آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] =        \
                        (uint8_t) (هدف_Byte_Unit >>8);                                         \
        }                                                                                       \
        آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] =            \
                        (uint8_t) (هدف_Byte_Unit);                                             \
        *err = U_میانگیر_سرریز_ERROR;                                                         \
    }                                                                                           \
} UPRV_بلوک_MACRO_انتها

/* Rules:
 *    Explicit Halant :
 *                      <HALANT> + <ZWNJ>
 *    Soft Halant :
 *                      <HALANT> + <ZWJ>
 */
ساكن فراغ U_CALLCONV
UConverter_fromUnicode_ISCII_OFFSETS_LOGIC(
        UConverter_From_Unicode_آرگومانها * آرگومانها_2, UError_کد * err) {
    ثابت char16_t *منبع = آرگومانها_2->منبع;
    ثابت char16_t *منبع_حد = آرگومانها_2->منبع_حد;
    بیعلامت حرف *هدف = (بیعلامت حرف *) آرگومانها_2->هدف;
    بیعلامت حرف *هدف_حد = (بیعلامت حرف *) آرگومانها_2->هدف_حد;
    int32_t* offsets = آرگومانها_2->offsets;
    uint32_t هدف_Byte_Unit = 0x0000;
    UChar32 منبع_Char = 0x0000;
    UChar32 موقت_Context_From_Unicode = 0x0000;    /* For special handling of the Gurmukhi script. */
    UConverter_داده_ISCII *converter_داده;
    uint16_t newDelta=0;
    uint16_t range = 0;
    UBool delta_تغییریافته = خطأ;

    إذا ((آرگومانها_2->converter == اشاره_تهی) || (آرگومانها_2->هدف_حد < آرگومانها_2->هدف) || (آرگومانها_2->منبع_حد < آرگومانها_2->منبع)) {
        *err = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    /* initialize data */
    converter_داده=(UConverter_داده_ISCII*)آرگومانها_2->converter->extraInfo;
    newDelta=converter_داده->currentDeltaFromUnicode;
    range = (uint16_t)(newDelta/DELTA);

    إذا ((منبع_Char = آرگومانها_2->converter->fromUChar32)!=0) {
        اذهب getTrail;
    }

    /*writing the char to the output stream */
    طالما (منبع < منبع_حد) {
        /* Write the language code following LF only if LF is not the last character. */
        إذا (آرگومانها_2->converter->from_Unicode_وضعیت == LF) {
            هدف_Byte_Unit = ATR<<8;
            هدف_Byte_Unit += (uint8_t) lookup_Initial_داده[range].isciiLang;
            آرگومانها_2->converter->from_Unicode_وضعیت = 0x0000;
            /* now append ATR and language code */
            WRITE_مقصد_هدف_FROM_U(آرگومانها_2,offsets,منبع,هدف,هدف_حد,هدف_Byte_Unit,err);
            إذا (U_FAILURE(*err)) {
                اكسر;
            }
        }
        
        منبع_Char = *منبع++;
        موقت_Context_From_Unicode = converter_داده->contextCharFromUnicode;
        
        هدف_Byte_Unit = missingCharMarker;
        
        /*check if input is in ASCII and C0 control codes range*/
        إذا (منبع_Char <= ASCII_انتها) {
            آرگومانها_2->converter->from_Unicode_وضعیت = منبع_Char;
            WRITE_مقصد_هدف_FROM_U(آرگومانها_2,offsets,منبع,هدف,هدف_حد,منبع_Char,err);
            إذا (U_FAILURE(*err)) {
                اكسر;
            }
            استمر;
        }
        بدل (منبع_Char) {
        حالة ZWNJ:
            /* contextChar has HALANT */
            إذا (converter_داده->contextCharFromUnicode) {
                converter_داده->contextCharFromUnicode = 0x00;
                هدف_Byte_Unit = ISCII_HALANT;
            } وإلا {
                /* consume ZWNJ and continue */
                converter_داده->contextCharFromUnicode = 0x00;
                استمر;
            }
            اكسر;
        حالة ZWJ:
            /* contextChar has HALANT */
            إذا (converter_داده->contextCharFromUnicode) {
                هدف_Byte_Unit = ISCII_NUKTA;
            } وإلا {
                هدف_Byte_Unit =ISCII_INV;
            }
            converter_داده->contextCharFromUnicode = 0x00;
            اكسر;
        افتراضي:
            /* is the sourceChar in the INDIC_RANGE? */
            إذا ((uint16_t)(INDIC_بلوک_انتها-منبع_Char) <= INDIC_RANGE) {
                /* Danda and Double Danda are valid in Northern scripts.. since Unicode
                 * does not include these codepoints in all Northern scrips we need to
                 * filter them out
                 */
                إذا (منبع_Char!= DANDA && منبع_Char != DOUBLE_DANDA) {
                    /* find out to which block the souceChar belongs*/
                    range =(uint16_t)((منبع_Char-INDIC_بلوک_BEGIN)/DELTA);
                    newDelta =(uint16_t)(range*DELTA);

                    /* Now are we in the same block as the previous? */
                    إذا (newDelta!= converter_داده->currentDeltaFromUnicode || converter_داده->is_اولی_میانگیر) {
                        converter_داده->currentDeltaFromUnicode = newDelta;
                        converter_داده->current_ماسک_From_Unicode = lookup_Initial_داده[range].ماسک_Enum_2;
                        delta_تغییریافته =درست;
                        converter_داده->is_اولی_میانگیر=خطأ;
                    }
                    
                    إذا (converter_داده->currentDeltaFromUnicode == PNJ_DELTA) { 
                        إذا (منبع_Char == PNJ_TIPPI) {
                            /* Make sure Tippi is converted to Bindi. */
                            منبع_Char = PNJ_BINDI;
                        } وإلا إذا (منبع_Char == PNJ_ADHAK) {
                            /* This is for consonant cluster handling. */
                            converter_داده->contextCharFromUnicode = PNJ_ADHAK;
                        }
                        
                    }
                    /* Normalize all Indic codepoints to Devanagari and map them to ISCII */
                    /* now subtract the new delta from sourceChar*/
                    منبع_Char -= converter_داده->currentDeltaFromUnicode;
                }

                /* get the target byte unit */
                هدف_Byte_Unit=from_Unicode_جدول[(uint8_t)منبع_Char];

                /* is the code point valid in current script? */
                إذا ((validity_جدول[(uint8_t)منبع_Char] & converter_داده->current_ماسک_From_Unicode)==0) {
                    /* Vocallic RR is assigned in ISCII Telugu and Unicode */
                    إذا (converter_داده->currentDeltaFromUnicode!=(TELUGU_DELTA) || منبع_Char!=VOCALLIC_RR) {
                        هدف_Byte_Unit=missingCharMarker;
                    }
                }

                إذا (delta_تغییریافته) {
                    /* we are in a script block which is different than
                     * previous sourceChar's script block write ATR and language codes
                     */
                    uint32_t موقت=0;
                    موقت =(uint16_t)(ATR<<8);
                    موقت += (uint16_t)((uint8_t) lookup_Initial_داده[range].isciiLang);
                    /* reset */
                    delta_تغییریافته=خطأ;
                    /* now append ATR and language code */
                    WRITE_مقصد_هدف_FROM_U(آرگومانها_2,offsets,منبع,هدف,هدف_حد,موقت,err);
                    إذا (U_FAILURE(*err)) {
                        اكسر;
                    }
                }
                
                إذا (converter_داده->currentDeltaFromUnicode == PNJ_DELTA && (منبع_Char + PNJ_DELTA) == PNJ_ADHAK) {
                    استمر;
                }
            }
            /* reset context char */
            converter_داده->contextCharFromUnicode = 0x00;
            اكسر;
        }
        إذا (converter_داده->currentDeltaFromUnicode == PNJ_DELTA && موقت_Context_From_Unicode == PNJ_ADHAK && isPNJConsonant((منبع_Char + PNJ_DELTA))) {
            /* If the previous codepoint is Adhak and the current codepoint is a consonant, the targetByteUnit should be C + Halant + C. */
            /* reset context char */
            converter_داده->contextCharFromUnicode = 0x0000;
            هدف_Byte_Unit = هدف_Byte_Unit << 16 | ISCII_HALANT << 8 | هدف_Byte_Unit;
            /* write targetByteUnit to target */
            WRITE_مقصد_هدف_FROM_U(آرگومانها_2, offsets, منبع, هدف, هدف_حد, هدف_Byte_Unit,err);
            إذا (U_FAILURE(*err)) {
                اكسر;
            }
        } وإلا إذا (هدف_Byte_Unit != missingCharMarker) {
            إذا (هدف_Byte_Unit==ISCII_HALANT) {
                converter_داده->contextCharFromUnicode = (char16_t)هدف_Byte_Unit;
            }
            /* write targetByteUnit to target*/
            WRITE_مقصد_هدف_FROM_U(آرگومانها_2,offsets,منبع,هدف,هدف_حد,هدف_Byte_Unit,err);
            إذا (U_FAILURE(*err)) {
                اكسر;
            }
        } وإلا {
            /* oops.. the code point is unassigned */
            /*check if the char is a First surrogate*/
            إذا (U16_IS_SURROGATE(منبع_Char)) {
                إذا (U16_IS_SURROGATE_LEAD(منبع_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    إذا (منبع < منبع_حد) {
                        /* test the following code unit */
                        char16_t trail= (*منبع);
                        إذا (U16_IS_TRAIL(trail)) {
                            منبع++;
                            منبع_Char=U16_GET_SUPPLEMENTARY(منبع_Char, trail);
                            *err =U_INVALID_CHAR_یافت;
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_یافت;
                        }
                    } وإلا {
                        /* no more input */
                        *err = U_ZERO_ERROR;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_یافت;
                }
            } وإلا {
                /* callback(unassigned) for a BMP code point */
                *err = U_INVALID_CHAR_یافت;
            }

            آرگومانها_2->converter->fromUChar32=منبع_Char;
            اكسر;
        }
    }/* end while(mySourceIndex<mySourceLength) */

    /*save the state and return */
    آرگومانها_2->منبع = منبع;
    آرگومانها_2->هدف = (حرف*)هدف;
}

ساكن ثابت uint16_t lookup_جدول[][2]={
    { ZERO,       ZERO     },     /*DEFAULT*/
    { ZERO,       ZERO     },     /*ROMAN*/
    { DEVANAGARI, DEV_ماسک },
    { BENGALI,    BNG_ماسک },
    { TAMIL,      TML_ماسک },
    { TELUGU,     KND_ماسک },
    { BENGALI,    BNG_ماسک },
    { ORIYA,      ORI_ماسک },
    { KANNADA,    KND_ماسک },
    { MALAYALAM,  MLM_ماسک },
    { GUJARATI,   GJR_ماسک },
    { GURMUKHI,   PNJ_ماسک }
};

#تعریف_کن WRITE_مقصد_هدف_مقصد_U(آرگومانها_2,منبع,هدف,offsets,جابه_جایی,هدف_Uni_Char,delta, err) UPRV_بلوک_MACRO_BEGIN { \
    /* add offset to current Indic Block */                                              \
    إذا(هدف_Uni_Char>ASCII_انتها &&                                                        \
           هدف_Uni_Char != ZWJ &&                                                       \
           هدف_Uni_Char != ZWNJ &&                                                      \
           هدف_Uni_Char != DANDA &&                                                     \
           هدف_Uni_Char != DOUBLE_DANDA){                                               \
                                                                                         \
           هدف_Uni_Char+=(uint16_t)(delta);                                             \
    }                                                                                    \
    /* now write the targetUniChar */                                                    \
    إذا(هدف<آرگومانها_2->هدف_حد){                                                        \
        *(هدف)++ = (char16_t)هدف_Uni_Char;                                              \
        إذا(offsets){                                                                     \
            *(offsets)++ = (int32_t)(جابه_جایی);                                            \
        }                                                                                \
    }وإلا{                                                                               \
        آرگومانها_2->converter->UChar_Error_میانگیر[آرگومانها_2->converter->UChar_Error_میانگیر_درازی++] =   \
            (char16_t)هدف_Uni_Char;                                                        \
        *err = U_میانگیر_سرریز_ERROR;                                                  \
    }                                                                                    \
} UPRV_بلوک_MACRO_انتها

#تعریف_کن GET_MAPPING(منبع_Char,هدف_Uni_Char,داده) UPRV_بلوک_MACRO_BEGIN {              \
    هدف_Uni_Char = مقصد_Unicode_جدول[(منبع_Char)] ;                                       \
    /* is the code point valid in current script? */                                     \
    إذا(منبع_Char> ASCII_انتها &&                                                          \
            (validity_جدول[(هدف_Uni_Char & 0x7F)] & داده->current_ماسک_مقصد_Unicode)==0){    \
        /* Vocallic RR is assigned in ISCII Telugu and Unicode */                         \
        إذا(داده->current_Delta_مقصد_Unicode!=(TELUGU_DELTA) ||                                \
                    هدف_Uni_Char!=VOCALLIC_RR){                                         \
            هدف_Uni_Char=missingCharMarker;                                             \
        }                                                                                \
    }                                                                                    \
} UPRV_بلوک_MACRO_انتها

/***********
 *  Rules for ISCII to Unicode converter
 *  ISCII is stateful encoding. To convert ISCII bytes to Unicode,
 *  which has both precomposed and decomposed forms characters
 *  pre-context and post-context need to be considered.
 *
 *  Post context
 *  i)  ATR : Attribute code is used to declare the font and script switching.
 *      Currently we only switch scripts and font codes consumed without generating an error
 *  ii) EXT : Extension code is used to declare switching to Sanskrit and for obscure,
 *      obsolete characters
 *  Pre context
 *  i)  Halant: if preceded by a halant then it is a explicit halant
 *  ii) Nukta :
 *       a) if preceded by a halant then it is a soft halant
 *       b) if preceded by specific consonants and the ligatures have pre-composed
 *          characters in Unicode then convert to pre-composed characters
 *  iii) Danda: If Danda is preceded by a Danda then convert to Double Danda
 *
 */

ساكن فراغ U_CALLCONV
UConverter_مقصد_Unicode_ISCII_OFFSETS_LOGIC(UConverter_مقصد_Unicode_آرگومانها *آرگومانها_2, UError_کد* err) {
    ثابت حرف *منبع = ( حرف *) آرگومانها_2->منبع;
    char16_t *هدف = آرگومانها_2->هدف;
    ثابت حرف *منبع_حد = آرگومانها_2->منبع_حد;
    ثابت char16_t* هدف_حد = آرگومانها_2->هدف_حد;
    uint32_t هدف_Uni_Char = 0x0000;
    uint8_t منبع_Char = 0x0000;
    UConverter_داده_ISCII* داده;
    UChar32* مقصد_Unicode_وضعیت=اشاره_تهی;
    UChar32 موقت_هدف_Uni_Char = 0x0000;
    char16_t* context_Char_مقصد_Unicode= اشاره_تهی;
    UBool یافت;
    صحيح i; 
    صحيح جابه_جایی = 0;

    إذا ((آرگومانها_2->converter == اشاره_تهی) || (هدف < آرگومانها_2->هدف) || (منبع < آرگومانها_2->منبع)) {
        *err = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    داده = (UConverter_داده_ISCII*)(آرگومانها_2->converter->extraInfo);
    context_Char_مقصد_Unicode = &داده->context_Char_مقصد_Unicode; /* contains previous ISCII codepoint visited */
    مقصد_Unicode_وضعیت = (UChar32*)&آرگومانها_2->converter->مقصد_Unicode_وضعیت;/* contains the mapping to Unicode of the above codepoint*/

    طالما (U_SUCCESS(*err) && منبع<منبع_حد) {

        هدف_Uni_Char = missingCharMarker;

        إذا (هدف < هدف_حد) {
            منبع_Char = (بیعلامت حرف)*(منبع)++;

            /* look at the post-context perform special processing */
            إذا (*context_Char_مقصد_Unicode==ATR) {

                /* If we have ATR in *contextCharToUnicode then we need to change our
                 * state to the Indic Script specified by sourceChar
                 */

                /* check if the sourceChar is supported script range*/
                إذا ((uint8_t)(PNJ-منبع_Char)<=PNJ-DEV) {
                    داده->current_Delta_مقصد_Unicode = (uint16_t)(lookup_جدول[منبع_Char & 0x0F][0] * DELTA);
                    داده->current_ماسک_مقصد_Unicode = (ماسک_Enum)lookup_جدول[منبع_Char & 0x0F][1];
                } وإلا إذا (منبع_Char==DEF) {
                    /* switch back to default */
                    داده->current_Delta_مقصد_Unicode = داده->def_Delta_مقصد_Unicode;
                    داده->current_ماسک_مقصد_Unicode = داده->def_ماسک_مقصد_Unicode;
                } وإلا {
                    إذا ((منبع_Char >= 0x21 && منبع_Char <= 0x3F)) {
                        /* these are display codes consume and continue */
                    } وإلا {
                        *err =U_ILLEGAL_CHAR_یافت;
                        /* reset */
                        *context_Char_مقصد_Unicode=NO_CHAR_MARKER;
                        اذهب CALLBACK;
                    }
                }

                /* reset */
                *context_Char_مقصد_Unicode=NO_CHAR_MARKER;

                استمر;

            } وإلا إذا (*context_Char_مقصد_Unicode==EXT) {
                /* check if sourceChar is in 0xA1-0xEE range */
                إذا ((uint8_t) (EXT_RANGE_انتها - منبع_Char) <= (EXT_RANGE_انتها - EXT_RANGE_BEGIN)) {
                    /* We currently support only Anudatta and Devanagari abbreviation sign */
                    إذا (منبع_Char==0xBF || منبع_Char == 0xB8) {
                        هدف_Uni_Char = (منبع_Char==0xBF) ? DEV_ABBR_SIGN : DEV_ANUDATTA;
                        
                        /* find out if the mapping is valid in this state */
                        إذا (validity_جدول[(uint8_t)هدف_Uni_Char] & داده->current_ماسک_مقصد_Unicode) {
                            *context_Char_مقصد_Unicode= NO_CHAR_MARKER;

                            /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                            إذا (داده->prev_مقصد_Unicode_وضعیت) {
                                WRITE_مقصد_هدف_مقصد_U(آرگومانها_2,منبع,هدف,آرگومانها_2->offsets,(منبع-آرگومانها_2->منبع -1),داده->prev_مقصد_Unicode_وضعیت,0,err);
                                داده->prev_مقصد_Unicode_وضعیت = 0x0000;
                            }
                            /* write to target */
                            WRITE_مقصد_هدف_مقصد_U(آرگومانها_2,منبع,هدف,آرگومانها_2->offsets,(منبع-آرگومانها_2->منبع -2),هدف_Uni_Char,داده->current_Delta_مقصد_Unicode,err);

                            استمر;
                        }
                    }
                    /* byte unit is unassigned */
                    هدف_Uni_Char = missingCharMarker;
                    *err= U_INVALID_CHAR_یافت;
                } وإلا {
                    /* only 0xA1 - 0xEE are legal after EXT char */
                    *context_Char_مقصد_Unicode= NO_CHAR_MARKER;
                    *err = U_ILLEGAL_CHAR_یافت;
                }
                اذهب CALLBACK;
            } وإلا إذا (*context_Char_مقصد_Unicode==ISCII_INV) {
                إذا (منبع_Char==ISCII_HALANT) {
                    هدف_Uni_Char = 0x0020; /* replace with space according to Indic FAQ */
                } وإلا {
                    هدف_Uni_Char = ZWJ;
                }

                /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                إذا (داده->prev_مقصد_Unicode_وضعیت) {
                    WRITE_مقصد_هدف_مقصد_U(آرگومانها_2,منبع,هدف,آرگومانها_2->offsets,(منبع-آرگومانها_2->منبع -1),داده->prev_مقصد_Unicode_وضعیت,0,err);
                    داده->prev_مقصد_Unicode_وضعیت = 0x0000;
                }
                /* write to target */
                WRITE_مقصد_هدف_مقصد_U(آرگومانها_2,منبع,هدف,آرگومانها_2->offsets,(منبع-آرگومانها_2->منبع -2),هدف_Uni_Char,داده->current_Delta_مقصد_Unicode,err);
                /* reset */
                *context_Char_مقصد_Unicode=NO_CHAR_MARKER;
            }

            /* look at the pre-context and perform special processing */
            بدل (منبع_Char) {
            حالة ISCII_INV:
            حالة EXT:
            حالة ATR:
                *context_Char_مقصد_Unicode = (char16_t)منبع_Char;

                إذا (*مقصد_Unicode_وضعیت != missingCharMarker) {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    إذا (داده->prev_مقصد_Unicode_وضعیت) {
                        WRITE_مقصد_هدف_مقصد_U(آرگومانها_2,منبع,هدف,آرگومانها_2->offsets,(منبع-آرگومانها_2->منبع -1),داده->prev_مقصد_Unicode_وضعیت,0,err);
                        داده->prev_مقصد_Unicode_وضعیت = 0x0000;
                    }
                    WRITE_مقصد_هدف_مقصد_U(آرگومانها_2,منبع,هدف,آرگومانها_2->offsets,(منبع-آرگومانها_2->منبع -2),*مقصد_Unicode_وضعیت,داده->current_Delta_مقصد_Unicode,err);
                    *مقصد_Unicode_وضعیت = missingCharMarker;
                }
                استمر;
            حالة ISCII_DANDA:
                /* handle double danda*/
                إذا (*context_Char_مقصد_Unicode== ISCII_DANDA) {
                    هدف_Uni_Char = DOUBLE_DANDA;
                    /* clear the context */
                    *context_Char_مقصد_Unicode = NO_CHAR_MARKER;
                    *مقصد_Unicode_وضعیت = missingCharMarker;
                } وإلا {
                    GET_MAPPING(منبع_Char,هدف_Uni_Char,داده);
                    *context_Char_مقصد_Unicode = منبع_Char;
                }
                اكسر;
            حالة ISCII_HALANT:
                /* handle explicit halant */
                إذا (*context_Char_مقصد_Unicode == ISCII_HALANT) {
                    هدف_Uni_Char = ZWNJ;
                    /* clear the context */
                    *context_Char_مقصد_Unicode = NO_CHAR_MARKER;
                } وإلا {
                    GET_MAPPING(منبع_Char,هدف_Uni_Char,داده);
                    *context_Char_مقصد_Unicode = منبع_Char;
                }
                اكسر;
            حالة 0x0A:
            حالة 0x0D:
                داده->reset_مقصد_Default_مقصد_Unicode = درست;
                GET_MAPPING(منبع_Char,هدف_Uni_Char,داده)
                ;
                *context_Char_مقصد_Unicode = منبع_Char;
                اكسر;

            حالة ISCII_VOWEL_SIGN_E:
                i=1;
                یافت=خطأ;
                لكل (; i<vowelSignESpecialCases[0][0]; i++) {
                    U_ASSERT(i<UPRV_LENGTHOF(vowelSignESpecialCases));
                    إذا (vowelSignESpecialCases[i][0]==(uint8_t)*context_Char_مقصد_Unicode) {
                        هدف_Uni_Char=vowelSignESpecialCases[i][1];
                        یافت=درست;
                        اكسر;
                    }
                }
                إذا (یافت) {
                    /* find out if the mapping is valid in this state */
                    إذا (validity_جدول[(uint8_t)هدف_Uni_Char] & داده->current_ماسک_مقصد_Unicode) {
                        /*targetUniChar += data->currentDeltaToUnicode ;*/
                        *context_Char_مقصد_Unicode= NO_CHAR_MARKER;
                        *مقصد_Unicode_وضعیت = missingCharMarker;
                        اكسر;
                    }
                }
                GET_MAPPING(منبع_Char,هدف_Uni_Char,داده);
                *context_Char_مقصد_Unicode = منبع_Char;
                اكسر;

            حالة ISCII_NUKTA:
                /* handle soft halant */
                إذا (*context_Char_مقصد_Unicode == ISCII_HALANT) {
                    هدف_Uni_Char = ZWJ;
                    /* clear the context */
                    *context_Char_مقصد_Unicode = NO_CHAR_MARKER;
                    اكسر;
                } وإلا إذا (داده->current_Delta_مقصد_Unicode == PNJ_DELTA && داده->context_Char_مقصد_Unicode == 0xc0) {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    إذا (داده->prev_مقصد_Unicode_وضعیت) {
                        WRITE_مقصد_هدف_مقصد_U(آرگومانها_2,منبع,هدف,آرگومانها_2->offsets,(منبع-آرگومانها_2->منبع -1),داده->prev_مقصد_Unicode_وضعیت,0,err);
                        داده->prev_مقصد_Unicode_وضعیت = 0x0000;
                    }
                    /* We got here because ISCII_NUKTA was preceded by 0xc0 and we are converting Gurmukhi.
                     * In that case we must convert (0xc0 0xe9) to (\u0a5c\u0a4d\u0a39).
                     */
                    هدف_Uni_Char = PNJ_RRA;
                    WRITE_مقصد_هدف_مقصد_U(آرگومانها_2, منبع, هدف, آرگومانها_2->offsets, (منبع-آرگومانها_2->منبع)-2, هدف_Uni_Char, 0, err);
                    إذا (U_SUCCESS(*err)) {
                        هدف_Uni_Char = PNJ_SIGN_VIRAMA;
                        WRITE_مقصد_هدف_مقصد_U(آرگومانها_2, منبع, هدف, آرگومانها_2->offsets, (منبع-آرگومانها_2->منبع)-2, هدف_Uni_Char, 0, err);
                        إذا (U_SUCCESS(*err)) {
                            هدف_Uni_Char = PNJ_HA;
                            WRITE_مقصد_هدف_مقصد_U(آرگومانها_2, منبع, هدف, آرگومانها_2->offsets, (منبع-آرگومانها_2->منبع)-2, هدف_Uni_Char, 0, err);
                        } وإلا {
                            آرگومانها_2->converter->UChar_Error_میانگیر[آرگومانها_2->converter->UChar_Error_میانگیر_درازی++]= PNJ_HA;
                        }
                    } وإلا {
                        آرگومانها_2->converter->UChar_Error_میانگیر[آرگومانها_2->converter->UChar_Error_میانگیر_درازی++]= PNJ_SIGN_VIRAMA;
                        آرگومانها_2->converter->UChar_Error_میانگیر[آرگومانها_2->converter->UChar_Error_میانگیر_درازی++]= PNJ_HA;
                    }
                    *مقصد_Unicode_وضعیت = missingCharMarker;
                    داده->context_Char_مقصد_Unicode = NO_CHAR_MARKER;
                    استمر;
                } وإلا {
                    /* try to handle <CHAR> + ISCII_NUKTA special mappings */
                    i=1;
                    یافت =خطأ;
                    لكل (; i<nuktaSpecialCases[0][0]; i++) {
                        إذا (nuktaSpecialCases[i][0]==(uint8_t)
                                *context_Char_مقصد_Unicode) {
                            هدف_Uni_Char=nuktaSpecialCases[i][1];
                            یافت =درست;
                            اكسر;
                        }
                    }
                    إذا (یافت) {
                        /* find out if the mapping is valid in this state */
                        إذا (validity_جدول[(uint8_t)هدف_Uni_Char] & داده->current_ماسک_مقصد_Unicode) {
                            /*targetUniChar += data->currentDeltaToUnicode ;*/
                            *context_Char_مقصد_Unicode= NO_CHAR_MARKER;
                            *مقصد_Unicode_وضعیت = missingCharMarker;
                            إذا (داده->current_Delta_مقصد_Unicode == PNJ_DELTA) {
                                /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                                إذا (داده->prev_مقصد_Unicode_وضعیت) {
                                    WRITE_مقصد_هدف_مقصد_U(آرگومانها_2,منبع,هدف,آرگومانها_2->offsets,(منبع-آرگومانها_2->منبع -1),داده->prev_مقصد_Unicode_وضعیت,0,err);
                                    داده->prev_مقصد_Unicode_وضعیت = 0x0000;
                                }
                                WRITE_مقصد_هدف_مقصد_U(آرگومانها_2,منبع,هدف,آرگومانها_2->offsets,(منبع-آرگومانها_2->منبع -2),هدف_Uni_Char,داده->current_Delta_مقصد_Unicode,err);
                                استمر;
                            }
                            اكسر;
                        }
                        /* else fall through to default */
                    }
                    /* else fall through to default */
                    U_FALLTHROUGH;
                }
            افتراضي:GET_MAPPING(منبع_Char,هدف_Uni_Char,داده)
                ;
                *context_Char_مقصد_Unicode = منبع_Char;
                اكسر;
            }

            إذا (*مقصد_Unicode_وضعیت != missingCharMarker) {
                /* Check to make sure that consonant clusters are handled correct for Gurmukhi script. */
                إذا (داده->current_Delta_مقصد_Unicode == PNJ_DELTA && داده->prev_مقصد_Unicode_وضعیت != 0 && isPNJConsonant(داده->prev_مقصد_Unicode_وضعیت) &&
                        (*مقصد_Unicode_وضعیت + PNJ_DELTA) == PNJ_SIGN_VIRAMA && ((UChar32)(هدف_Uni_Char + PNJ_DELTA) == داده->prev_مقصد_Unicode_وضعیت)) {
                    /* Consonant clusters C + HALANT + C should be encoded as ADHAK + C */
                    جابه_جایی = (صحيح)(منبع-آرگومانها_2->منبع - 3);
                    موقت_هدف_Uni_Char = PNJ_ADHAK; /* This is necessary to avoid some compiler warnings. */
                    WRITE_مقصد_هدف_مقصد_U(آرگومانها_2,منبع,هدف,آرگومانها_2->offsets,جابه_جایی,موقت_هدف_Uni_Char,0,err);
                    WRITE_مقصد_هدف_مقصد_U(آرگومانها_2,منبع,هدف,آرگومانها_2->offsets,جابه_جایی,داده->prev_مقصد_Unicode_وضعیت,0,err);
                    داده->prev_مقصد_Unicode_وضعیت = 0x0000; /* reset the previous unicode code point */
                    *مقصد_Unicode_وضعیت = missingCharMarker;
                    استمر;
                } وإلا {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    إذا (داده->prev_مقصد_Unicode_وضعیت) {
                        WRITE_مقصد_هدف_مقصد_U(آرگومانها_2,منبع,هدف,آرگومانها_2->offsets,(منبع-آرگومانها_2->منبع -1),داده->prev_مقصد_Unicode_وضعیت,0,err);
                        داده->prev_مقصد_Unicode_وضعیت = 0x0000;
                    }
                    /* Check to make sure that Bindi and Tippi are handled correctly for Gurmukhi script. 
                     * If 0xA2 is preceded by a codepoint in the PNJ_BINDI_TIPPI_SET then the target codepoint should be Tippi instead of Bindi.
                     */
                    إذا (داده->current_Delta_مقصد_Unicode == PNJ_DELTA && (هدف_Uni_Char + PNJ_DELTA) == PNJ_BINDI && isPNJBindiTippi((*مقصد_Unicode_وضعیت + PNJ_DELTA))) {
                        هدف_Uni_Char = PNJ_TIPPI - PNJ_DELTA;
                        WRITE_مقصد_هدف_مقصد_U(آرگومانها_2,منبع,هدف,آرگومانها_2->offsets,(منبع-آرگومانها_2->منبع -2),*مقصد_Unicode_وضعیت,PNJ_DELTA,err);
                    } وإلا إذا (داده->current_Delta_مقصد_Unicode == PNJ_DELTA && (هدف_Uni_Char + PNJ_DELTA) == PNJ_SIGN_VIRAMA && isPNJConsonant((*مقصد_Unicode_وضعیت + PNJ_DELTA))) {
                        /* Store the current toUnicodeStatus code point for later handling of consonant cluster in Gurmukhi. */
                        داده->prev_مقصد_Unicode_وضعیت = *مقصد_Unicode_وضعیت + PNJ_DELTA;
                    } وإلا {
                        /* write the previously mapped codepoint */
                        WRITE_مقصد_هدف_مقصد_U(آرگومانها_2,منبع,هدف,آرگومانها_2->offsets,(منبع-آرگومانها_2->منبع -2),*مقصد_Unicode_وضعیت,داده->current_Delta_مقصد_Unicode,err);
                    }
                }
                *مقصد_Unicode_وضعیت = missingCharMarker;
            }

            إذا (هدف_Uni_Char != missingCharMarker) {
                /* now save the targetUniChar for delayed write */
                *مقصد_Unicode_وضعیت = (char16_t) هدف_Uni_Char;
                إذا (داده->reset_مقصد_Default_مقصد_Unicode) {
                    داده->current_Delta_مقصد_Unicode = داده->def_Delta_مقصد_Unicode;
                    داده->current_ماسک_مقصد_Unicode = داده->def_ماسک_مقصد_Unicode;
                    داده->reset_مقصد_Default_مقصد_Unicode=خطأ;
                }
            } وإلا {

                /* we reach here only if targetUniChar == missingCharMarker
                 * so assign codes to reason and err
                 */
                *err = U_INVALID_CHAR_یافت;
CALLBACK:
                آرگومانها_2->converter->مقصد_UBytes[0] = منبع_Char;
                آرگومانها_2->converter->مقصد_ULength = 1;
                اكسر;
            }

        } وإلا {
            *err =U_میانگیر_سرریز_ERROR;
            اكسر;
        }
    }

    إذا (U_SUCCESS(*err) && آرگومانها_2->flush && منبع == منبع_حد) {
        /* end of the input stream */
        UConverter *cnv = آرگومانها_2->converter;

        إذا (*context_Char_مقصد_Unicode==ATR || *context_Char_مقصد_Unicode==EXT || *context_Char_مقصد_Unicode==ISCII_INV) {
            /* set toUBytes[] */
            cnv->مقصد_UBytes[0] = (uint8_t)*context_Char_مقصد_Unicode;
            cnv->مقصد_ULength = 1;

            /* avoid looping on truncated sequences */
            *context_Char_مقصد_Unicode = NO_CHAR_MARKER;
        } وإلا {
            cnv->مقصد_ULength = 0;
        }

        إذا (*مقصد_Unicode_وضعیت != missingCharMarker) {
            /* output a remaining target character */
            WRITE_مقصد_هدف_مقصد_U(آرگومانها_2,منبع,هدف,آرگومانها_2->offsets,(منبع - آرگومانها_2->منبع -1),*مقصد_Unicode_وضعیت,داده->current_Delta_مقصد_Unicode,err);
            *مقصد_Unicode_وضعیت = missingCharMarker;
        }
    }

    آرگومانها_2->هدف = هدف;
    آرگومانها_2->منبع = منبع;
}

/* structure for SafeClone calculations */
بنية cloneISCIIStruct {
    UConverter cnv;
    UConverter_داده_ISCII mydata;
};

ساكن UConverter * U_CALLCONV
ISCII_امن_Clone(ثابت UConverter *cnv,
              فراغ *پشته_میانگیر,
              int32_t *p_میانگیر_اندازه,
              UError_کد *وضعیت)
{
    بنية cloneISCIIStruct * localClone;
    int32_t میانگیر_اندازه_Needed = حجم(بنية cloneISCIIStruct);

    إذا (U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }

    إذا (*p_میانگیر_اندازه == 0) { /* 'preflighting' request - set needed size into *pBufferSize */
        *p_میانگیر_اندازه = میانگیر_اندازه_Needed;
        أرجع اشاره_تهی;
    }

    localClone = (بنية cloneISCIIStruct *)پشته_میانگیر;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, حجم(UConverter_داده_ISCII));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = درست;

    أرجع &localClone->cnv;
}

ساكن فراغ U_CALLCONV
ISCIIGet_Unicode_مجموعه(ثابت UConverter *cnv,
                    ثابت USetAdder *sa,
                    UConverter_Unicode_مجموعه which,
                    UError_کد *p_Error_کد)
{
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_کد;
    int32_t idx, script;
    uint8_t mask;

    /* Since all ISCII versions allow switching to other ISCII
    scripts, we add all roundtrippable characters to this set. */
    sa->جمع_کن_Range(sa->مجموعه, 0, ASCII_انتها);
    لكل (script = DEVANAGARI; script <= MALAYALAM; script++) {
        mask = (uint8_t)(lookup_Initial_داده[script].ماسک_Enum_2);
        لكل (idx = 0; idx < DELTA; idx++) {
            /* added check for TELUGU character */
            إذا ((validity_جدول[idx] & mask) || (script==TELUGU && idx==0x31)) {
                sa->جمع_کن(sa->مجموعه, idx + (script * DELTA) + INDIC_بلوک_BEGIN);
            }
        }
    }
    sa->جمع_کن(sa->مجموعه, DANDA);
    sa->جمع_کن(sa->مجموعه, DOUBLE_DANDA);
    sa->جمع_کن(sa->مجموعه, ZWNJ);
    sa->جمع_کن(sa->مجموعه, ZWJ);
}
U_CDECL_انتها
ساكن ثابت UConverterImpl _ISCIIImpl={

    UCNV_ISCII,

    اشاره_تهی,
    اشاره_تهی,

    _ISCIIOpen,
    _ISCIIClose,
    _ISCIIReset,

    UConverter_مقصد_Unicode_ISCII_OFFSETS_LOGIC,
    UConverter_مقصد_Unicode_ISCII_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISCII_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISCII_OFFSETS_LOGIC,
    اشاره_تهی,

    اشاره_تهی,
    _ISCIIgetName,
    اشاره_تهی,
    ISCII_امن_Clone,
    ISCIIGet_Unicode_مجموعه,
    اشاره_تهی,
    اشاره_تهی
};

ساكن ثابت UConverter_Static_داده ISCIIStatic_داده={
    حجم(UConverter_Static_داده),
        "ISCII",
         0,
         UCNV_IBM,
         UCNV_ISCII,
         1,
         4,
        { 0x1a, 0, 0, 0 },
        0x1,
        خطأ,
        خطأ,
        0x0,
        0x0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

ثابت UConverter_Shared_داده _ISCIIData=
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&ISCIIStatic_داده, &_ISCIIImpl);

#پایان_اگر /* #if !UCONFIG_NO_LEGACY_CONVERSION */
