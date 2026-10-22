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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#شامل "unicode/ucnv.h"
#شامل "unicode/ucnv_cb.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "ucnv_bld.h"
#شامل "ucnv_cnv.h"
#شامل "cstring.h"
#شامل "uassert.h"

#تعریف_کرو UCNV_OPTIONS_نسخہ_نقاب 0xf
#تعریف_کرو NUKTA               0x093c
#تعریف_کرو HALANT              0x094d
#تعریف_کرو ZWNJ                0x200c /* Zero Width Non Joiner */
#تعریف_کرو ZWJ                 0x200d /* Zero width Joiner */
#تعریف_کرو INVALID_CHAR        0xffff
#تعریف_کرو ATR                 0xEF   /* Attribute code */
#تعریف_کرو EXT                 0xF0   /* Extension code */
#تعریف_کرو DANDA               0x0964
#تعریف_کرو DOUBLE_DANDA        0x0965
#تعریف_کرو ISCII_NUKTA         0xE9
#تعریف_کرو ISCII_HALANT        0xE8
#تعریف_کرو ISCII_DANDA         0xEA
#تعریف_کرو ISCII_INV           0xD9
#تعریف_کرو ISCII_VOWEL_SIGN_E  0xE0
#تعریف_کرو INDIC_بلاک_BEGIN   0x0900
#تعریف_کرو INDIC_بلاک_انتہا     0x0D7F
#تعریف_کرو INDIC_RANGE         (INDIC_بلاک_انتہا - INDIC_بلاک_BEGIN)
#تعریف_کرو VOCALLIC_RR         0x0931
#تعریف_کرو LF                  0x0A
#تعریف_کرو ASCII_انتہا           0xA0
#تعریف_کرو NO_CHAR_MARKER      0xFFFE
#تعریف_کرو TELUGU_DELTA        DELTA * TELUGU
#تعریف_کرو DEV_ABBR_SIGN       0x0970
#تعریف_کرو DEV_ANUDATTA        0x0952
#تعریف_کرو EXT_RANGE_BEGIN     0xA1
#تعریف_کرو EXT_RANGE_انتہا       0xEE

#تعریف_کرو PNJ_DELTA           0x0100
#تعریف_کرو PNJ_BINDI           0x0A02
#تعریف_کرو PNJ_TIPPI           0x0A70
#تعریف_کرو PNJ_SIGN_VIRAMA     0x0A4D
#تعریف_کرو PNJ_ADHAK           0x0A71
#تعریف_کرو PNJ_HA              0x0A39
#تعریف_کرو PNJ_RRA             0x0A5C

قسمتعریف شمار {
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
قسمتعریف شمار {
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

قسمتعریف شمار {
    DEV_نقاب =0x80,
    PNJ_نقاب =0x40,
    GJR_نقاب =0x20,
    ORI_نقاب =0x10,
    BNG_نقاب =0x08,
    KND_نقاب =0x04,
    MLM_نقاب =0x02,
    TML_نقاب =0x01,
    ZERO =0x00
}نقاب_Enum;

#تعریف_کرو ISCII_CNV_سابقہ "ISCII,version="

قسمتعریف ساخت {
    char16_t context_Char_مقصد_Unicode;         /* previous Unicode codepoint for contextual analysis */
    char16_t contextCharFromUnicode;       /* previous Unicode codepoint for contextual analysis */
    uint16_t def_Delta_مقصد_Unicode;         /* delta for switching to default state when DEF is encountered  */
    uint16_t currentDeltaFromUnicode;   /* current delta in Indic block */
    uint16_t current_Delta_مقصد_Unicode;     /* current delta in Indic block */
    نقاب_Enum current_نقاب_From_Unicode;    /* mask for current state in toUnicode */
    نقاب_Enum current_نقاب_مقصد_Unicode;      /* mask for current state in toUnicode */
    نقاب_Enum def_نقاب_مقصد_Unicode;          /* mask for default state in toUnicode */
    UBool is_پہلا_بفر;                /* boolean for fromUnicode to see if we need to announce the first script */
    UBool reset_مقصد_Default_مقصد_Unicode;      /* boolean for resetting to default delta and mask when a newline is encountered*/
    حرف name[حجم(ISCII_CNV_سابقہ) + 1];
    UChar32 پچھلا_مقصد_Unicode_حالت;        /* Hold the previous toUnicodeStatus. This is necessary because we may need to know the last two code points. */
} UConverter_ڈیٹا_ISCII;

قسمتعریف ساخت Lookup_ڈیٹا_Struct {
    UniLang uniLang;
    نقاب_Enum نقاب_Enum_2;
    ISCIILang isciiLang;
} Lookup_ڈیٹا_Struct;

ساکن ثابت Lookup_ڈیٹا_Struct lookup_Initial_ڈیٹا[]={
    { DEVANAGARI, DEV_نقاب,  DEV },
    { BENGALI,    BNG_نقاب,  BNG },
    { GURMUKHI,   PNJ_نقاب,  PNJ },
    { GUJARATI,   GJR_نقاب,  GJR },
    { ORIYA,      ORI_نقاب,  ORI },
    { TAMIL,      TML_نقاب,  TML },
    { TELUGU,     KND_نقاب,  TLG },
    { KANNADA,    KND_نقاب,  KND },
    { MALAYALAM,  MLM_نقاب,  MLM }
};

/*
 * For special handling of certain Gurmukhi characters.
 * Bit 0 (value 1): PNJ consonant
 * Bit 1 (value 2): PNJ Bindi Tippi
 */
ساکن ثابت uint8_t pnjMap[80] = {
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

ساکن UBool
isPNJConsonant(UChar32 c) {
    اگر (c < 0xa00 || 0xa50 <= c) {
        واپس جھوٹ;
    } ورنہ {
        واپس pnjMap[c - 0xa00] & 1;
    }
}

ساکن UBool
isPNJBindiTippi(UChar32 c) {
    اگر (c < 0xa00 || 0xa50 <= c) {
        واپس جھوٹ;
    } ورنہ {
        واپس pnjMap[c - 0xa00] >> 1;
    }
}
U_CDECL_BEGIN
ساکن خالی  U_CALLCONV
_ISCIIOpen(UConverter *cnv, UConverter_بھرو_دلائل *p_دلائل, UError_رمز *error_رمز) {
    اگر(p_دلائل->onlyTestIsLoadable) {
        واپس;
    }

    cnv->extraInfo = uprv_malloc(حجم(UConverter_ڈیٹا_ISCII));

    اگر (cnv->extraInfo != خالی_اشاریہ) {
        int32_t len=0;
        UConverter_ڈیٹا_ISCII *converter_ڈیٹا=
                (UConverter_ڈیٹا_ISCII *) cnv->extraInfo;
        converter_ڈیٹا->context_Char_مقصد_Unicode=NO_CHAR_MARKER;
        cnv->مقصد_Unicode_حالت = missingCharMarker;
        converter_ڈیٹا->contextCharFromUnicode=0x0000;
        converter_ڈیٹا->reset_مقصد_Default_مقصد_Unicode=جھوٹ;
        /* check if the version requested is supported */
        اگر ((p_دلائل->options & UCNV_OPTIONS_نسخہ_نقاب) < 9) {
            /* initialize state variables */
            converter_ڈیٹا->currentDeltaFromUnicode
                    = converter_ڈیٹا->current_Delta_مقصد_Unicode
                            = converter_ڈیٹا->def_Delta_مقصد_Unicode = (uint16_t)(lookup_Initial_ڈیٹا[p_دلائل->options & UCNV_OPTIONS_نسخہ_نقاب].uniLang * DELTA);

            converter_ڈیٹا->current_نقاب_From_Unicode
                    = converter_ڈیٹا->current_نقاب_مقصد_Unicode
                            = converter_ڈیٹا->def_نقاب_مقصد_Unicode = lookup_Initial_ڈیٹا[p_دلائل->options & UCNV_OPTIONS_نسخہ_نقاب].نقاب_Enum_2;
            
            converter_ڈیٹا->is_پہلا_بفر=سچ;
            (خالی)uprv_strcpy(converter_ڈیٹا->name, ISCII_CNV_سابقہ);
            len = (int32_t)uprv_strlen(converter_ڈیٹا->name);
            converter_ڈیٹا->name[len]= (حرف)((p_دلائل->options & UCNV_OPTIONS_نسخہ_نقاب) + '0');
            converter_ڈیٹا->name[len+1]=0;
            
            converter_ڈیٹا->پچھلا_مقصد_Unicode_حالت = 0x0000;
        } ورنہ {
            uprv_free(cnv->extraInfo);
            cnv->extraInfo = خالی_اشاریہ;
            *error_رمز = U_ILLEGAL_دلیل_ERROR;
        }

    } ورنہ {
        *error_رمز =U_حافظہ_تخصیص_ERROR;
    }
}

ساکن خالی U_CALLCONV
_ISCIIClose(UConverter *cnv) {
    اگر (cnv->extraInfo!=خالی_اشاریہ) {
        اگر (!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo=خالی_اشاریہ;
    }
}

ساکن ثابت حرف*  U_CALLCONV
_ISCIIgetName(ثابت UConverter* cnv) {
    اگر (cnv->extraInfo) {
        UConverter_ڈیٹا_ISCII* my_ڈیٹا= (UConverter_ڈیٹا_ISCII*)cnv->extraInfo;
        واپس my_ڈیٹا->name;
    }
    واپس خالی_اشاریہ;
}

ساکن خالی U_CALLCONV
_ISCIIReset(UConverter *cnv, UConverterResetChoice choice) {
    UConverter_ڈیٹا_ISCII* ڈیٹا =(UConverter_ڈیٹا_ISCII *) (cnv->extraInfo);
    اگر (choice<=UCNV_RESET_مقصد_UNICODE) {
        cnv->مقصد_Unicode_حالت = missingCharMarker;
        cnv->منوال=0;
        ڈیٹا->current_Delta_مقصد_Unicode=ڈیٹا->def_Delta_مقصد_Unicode;
        ڈیٹا->current_نقاب_مقصد_Unicode = ڈیٹا->def_نقاب_مقصد_Unicode;
        ڈیٹا->context_Char_مقصد_Unicode=NO_CHAR_MARKER;
        ڈیٹا->پچھلا_مقصد_Unicode_حالت = 0x0000;
    }
    اگر (choice!=UCNV_RESET_مقصد_UNICODE) {
        cnv->fromUChar32=0x0000;
        ڈیٹا->contextCharFromUnicode=0x00;
        ڈیٹا->current_نقاب_From_Unicode=ڈیٹا->def_نقاب_مقصد_Unicode;
        ڈیٹا->currentDeltaFromUnicode=ڈیٹا->def_Delta_مقصد_Unicode;
        ڈیٹا->is_پہلا_بفر=سچ;
        ڈیٹا->reset_مقصد_Default_مقصد_Unicode=جھوٹ;
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

ساکن ثابت uint8_t validity_جدول[128] = {
/* This state table is tool generated please do not edit unless you know exactly what you are doing */
/* Note: This table was edited to mirror the Windows XP implementation */
/*ISCII:Valid:Unicode */
/*0xa0 : 0x00: 0x900  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xa1 : 0xb8: 0x901  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + ZERO     + ZERO     + ZERO     ,
/*0xa2 : 0xfe: 0x902  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xa3 : 0xbf: 0x903  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0x00 : 0x00: 0x904  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xa4 : 0xff: 0x905  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xa5 : 0xff: 0x906  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xa6 : 0xff: 0x907  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xa7 : 0xff: 0x908  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xa8 : 0xff: 0x909  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xa9 : 0xff: 0x90a  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xaa : 0xfe: 0x90b  */ DEV_نقاب + ZERO     + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0x00 : 0x00: 0x90c  */ DEV_نقاب + ZERO     + ZERO     + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0xae : 0x80: 0x90d  */ DEV_نقاب + ZERO     + GJR_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xab : 0x87: 0x90e  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xac : 0xff: 0x90f  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xad : 0xff: 0x910  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xb2 : 0x80: 0x911  */ DEV_نقاب + ZERO     + GJR_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xaf : 0x87: 0x912  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xb0 : 0xff: 0x913  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xb1 : 0xff: 0x914  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xb3 : 0xff: 0x915  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xb4 : 0xfe: 0x916  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0xb5 : 0xfe: 0x917  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0xb6 : 0xfe: 0x918  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0xb7 : 0xff: 0x919  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xb8 : 0xff: 0x91a  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xb9 : 0xfe: 0x91b  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0xba : 0xff: 0x91c  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xbb : 0xfe: 0x91d  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0xbc : 0xff: 0x91e  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xbd : 0xff: 0x91f  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xbe : 0xfe: 0x920  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0xbf : 0xfe: 0x921  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0xc0 : 0xfe: 0x922  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0xc1 : 0xff: 0x923  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xc2 : 0xff: 0x924  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xc3 : 0xfe: 0x925  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0xc4 : 0xfe: 0x926  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0xc5 : 0xfe: 0x927  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0xc6 : 0xff: 0x928  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xc7 : 0x81: 0x929  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + TML_نقاب ,
/*0xc8 : 0xff: 0x92a  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xc9 : 0xfe: 0x92b  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0xca : 0xfe: 0x92c  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0xcb : 0xfe: 0x92d  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0xcc : 0xfe: 0x92e  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xcd : 0xff: 0x92f  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xcf : 0xff: 0x930  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xd0 : 0x87: 0x931  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + MLM_نقاب + TML_نقاب ,
/*0xd1 : 0xff: 0x932  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xd2 : 0xb7: 0x933  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + ZERO     + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xd3 : 0x83: 0x934  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + MLM_نقاب + TML_نقاب ,
/*0xd4 : 0xff: 0x935  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + ZERO     + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xd5 : 0xfe: 0x936  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0xd6 : 0xbf: 0x937  */ DEV_نقاب + ZERO     + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xd7 : 0xff: 0x938  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xd8 : 0xff: 0x939  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0x00 : 0x00: 0x93A  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x93B  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe9 : 0xda: 0x93c  */ DEV_نقاب + PNJ_نقاب + ZERO     + ORI_نقاب + BNG_نقاب + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x93d  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xda : 0xff: 0x93e  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xdb : 0xff: 0x93f  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xdc : 0xff: 0x940  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xdd : 0xff: 0x941  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xde : 0xff: 0x942  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xdf : 0xbe: 0x943  */ DEV_نقاب + ZERO     + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0x00 : 0x00: 0x944  */ DEV_نقاب + ZERO     + GJR_نقاب + ZERO     + BNG_نقاب + KND_نقاب + ZERO     + ZERO     ,
/*0xe3 : 0x80: 0x945  */ DEV_نقاب + ZERO     + GJR_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe0 : 0x87: 0x946  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xe1 : 0xff: 0x947  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xe2 : 0xff: 0x948  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xe7 : 0x80: 0x949  */ DEV_نقاب + ZERO     + GJR_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe4 : 0x87: 0x94a  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xe5 : 0xff: 0x94b  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xe6 : 0xff: 0x94c  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xe8 : 0xff: 0x94d  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xec : 0x00: 0x94e  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xed : 0x00: 0x94f  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x950  */ DEV_نقاب + ZERO     + GJR_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x951  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x952  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x953  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x954  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x955  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + KND_نقاب + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x956  */ ZERO     + ZERO     + ZERO     + ORI_نقاب + ZERO     + KND_نقاب + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x957  */ ZERO     + ZERO     + ZERO     + ORI_نقاب + BNG_نقاب + ZERO     + MLM_نقاب + ZERO     ,
/*0x00 : 0x00: 0x958  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x959  */ DEV_نقاب + PNJ_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95a  */ DEV_نقاب + PNJ_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95b  */ DEV_نقاب + PNJ_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95c  */ DEV_نقاب + PNJ_نقاب + ZERO     + ZERO     + BNG_نقاب + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95d  */ DEV_نقاب + ZERO     + ZERO     + ORI_نقاب + BNG_نقاب + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95e  */ DEV_نقاب + PNJ_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xce : 0x98: 0x95f  */ DEV_نقاب + ZERO     + ZERO     + ORI_نقاب + BNG_نقاب + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x960  */ DEV_نقاب + ZERO     + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0x00 : 0x00: 0x961  */ DEV_نقاب + ZERO     + ZERO     + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + ZERO     ,
/*0x00 : 0x00: 0x962  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + BNG_نقاب + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x963  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + BNG_نقاب + ZERO     + ZERO     + ZERO     ,
/*0xea : 0xf8: 0x964  */ DEV_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xeaea : 0x00: 0x965*/ DEV_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xf1 : 0xff: 0x966  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xf2 : 0xff: 0x967  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xf3 : 0xff: 0x968  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xf4 : 0xff: 0x969  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xf5 : 0xff: 0x96a  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xf6 : 0xff: 0x96b  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xf7 : 0xff: 0x96c  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xf8 : 0xff: 0x96d  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xf9 : 0xff: 0x96e  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0xfa : 0xff: 0x96f  */ DEV_نقاب + PNJ_نقاب + GJR_نقاب + ORI_نقاب + BNG_نقاب + KND_نقاب + MLM_نقاب + TML_نقاب ,
/*0x00 : 0x80: 0x970  */ DEV_نقاب + PNJ_نقاب + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*
 * The length of the array is 128 to provide values for 0x900..0x97f.
 * The last 15 entries for 0x971..0x97f of the validity table are all zero
 * because no Indic script uses such Unicode code points.
 */
/*0x00 : 0x00: 0x9yz  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO
};

ساکن ثابت uint16_t from_Unicode_جدول[128]={
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
ساکن ثابت uint16_t مقصد_Unicode_جدول[256]={
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

ساکن ثابت uint16_t vowelSignESpecialCases[][2]={
	{ 2 /*length of array*/    , 0      },
	{ 0xA4 , 0x0904 },
};

ساکن ثابت uint16_t nuktaSpecialCases[][2]={
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


#تعریف_کرو WRITE_مقصد_ہدف_FROM_U(دلائل_2,offsets,ماخذ,ہدف,ہدف_حد_بندی,ہدف_Byte_Unit,err) UPRV_بلاک_MACRO_BEGIN { \
    int32_t انحراف = (int32_t)(ماخذ - دلائل_2->ماخذ-1);                                        \
      /* write the targetUniChar  to target */                                                  \
    اگر(ہدف < ہدف_حد_بندی){                                                                   \
        اگر(ہدف_Byte_Unit <= 0xFF){                                                             \
            *(ہدف)++ = (uint8_t)(ہدف_Byte_Unit);                                            \
            اگر(offsets){                                                                        \
                *(offsets++) = انحراف;                                                          \
            }                                                                                   \
        }ورنہ{                                                                                  \
            اگر (ہدف_Byte_Unit > 0xFFFF) {                                                      \
                *(ہدف)++ = (uint8_t)(ہدف_Byte_Unit>>16);                                    \
                اگر (offsets) {                                                                  \
                    --انحراف;                                                                   \
                    *(offsets++) = انحراف;                                                      \
                }                                                                               \
            }                                                                                   \
            اگر (!(ہدف < ہدف_حد_بندی)) {                                                      \
                دلائل_2->converter->char_Error_بفر[دلائل_2->converter->char_Error_بفر_لمبائی++] =    \
                                (uint8_t)(ہدف_Byte_Unit >> 8);                                 \
                دلائل_2->converter->char_Error_بفر[دلائل_2->converter->char_Error_بفر_لمبائی++] =    \
                                (uint8_t)ہدف_Byte_Unit;                                        \
                *err = U_بفر_حد_سے_تجاوز_ERROR;                                                 \
            } ورنہ {                                                                            \
                *(ہدف)++ = (uint8_t)(ہدف_Byte_Unit>>8);                                     \
                اگر(offsets){                                                                    \
                    *(offsets++) = انحراف;                                                      \
                }                                                                               \
                اگر(ہدف < ہدف_حد_بندی){                                                       \
                    *(ہدف)++ = (uint8_t)  ہدف_Byte_Unit;                                    \
                    اگر(offsets){                                                                \
                        *(offsets++) = انحراف                            ;                      \
                    }                                                                           \
                }ورنہ{                                                                          \
                    دلائل_2->converter->char_Error_بفر[دلائل_2->converter->char_Error_بفر_لمبائی++] =\
                                (uint8_t) (ہدف_Byte_Unit);                                     \
                    *err = U_بفر_حد_سے_تجاوز_ERROR;                                             \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
    }ورنہ{                                                                                      \
        اگر (ہدف_Byte_Unit & 0xFF0000) {                                                        \
            دلائل_2->converter->char_Error_بفر[دلائل_2->converter->char_Error_بفر_لمبائی++] =        \
                        (uint8_t) (ہدف_Byte_Unit >>16);                                        \
        }                                                                                       \
        اگر(ہدف_Byte_Unit & 0xFF00){                                                            \
            دلائل_2->converter->char_Error_بفر[دلائل_2->converter->char_Error_بفر_لمبائی++] =        \
                        (uint8_t) (ہدف_Byte_Unit >>8);                                         \
        }                                                                                       \
        دلائل_2->converter->char_Error_بفر[دلائل_2->converter->char_Error_بفر_لمبائی++] =            \
                        (uint8_t) (ہدف_Byte_Unit);                                             \
        *err = U_بفر_حد_سے_تجاوز_ERROR;                                                         \
    }                                                                                           \
} UPRV_بلاک_MACRO_انتہا

/* Rules:
 *    Explicit Halant :
 *                      <HALANT> + <ZWNJ>
 *    Soft Halant :
 *                      <HALANT> + <ZWJ>
 */
ساکن خالی U_CALLCONV
UConverter_fromUnicode_ISCII_OFFSETS_LOGIC(
        UConverter_From_Unicode_دلائل * دلائل_2, UError_رمز * err) {
    ثابت char16_t *ماخذ = دلائل_2->ماخذ;
    ثابت char16_t *ماخذ_حد_بندی = دلائل_2->ماخذ_حد_بندی;
    غیرعلامتی حرف *ہدف = (غیرعلامتی حرف *) دلائل_2->ہدف;
    غیرعلامتی حرف *ہدف_حد_بندی = (غیرعلامتی حرف *) دلائل_2->ہدف_حد_بندی;
    int32_t* offsets = دلائل_2->offsets;
    uint32_t ہدف_Byte_Unit = 0x0000;
    UChar32 ماخذ_Char = 0x0000;
    UChar32 عارضی_Context_From_Unicode = 0x0000;    /* For special handling of the Gurmukhi script. */
    UConverter_ڈیٹا_ISCII *converter_ڈیٹا;
    uint16_t newDelta=0;
    uint16_t range = 0;
    UBool delta_بدلا = جھوٹ;

    اگر ((دلائل_2->converter == خالی_اشاریہ) || (دلائل_2->ہدف_حد_بندی < دلائل_2->ہدف) || (دلائل_2->ماخذ_حد_بندی < دلائل_2->ماخذ)) {
        *err = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    /* initialize data */
    converter_ڈیٹا=(UConverter_ڈیٹا_ISCII*)دلائل_2->converter->extraInfo;
    newDelta=converter_ڈیٹا->currentDeltaFromUnicode;
    range = (uint16_t)(newDelta/DELTA);

    اگر ((ماخذ_Char = دلائل_2->converter->fromUChar32)!=0) {
        جاؤ getTrail;
    }

    /*writing the char to the output stream */
    جب (ماخذ < ماخذ_حد_بندی) {
        /* Write the language code following LF only if LF is not the last character. */
        اگر (دلائل_2->converter->from_Unicode_حالت == LF) {
            ہدف_Byte_Unit = ATR<<8;
            ہدف_Byte_Unit += (uint8_t) lookup_Initial_ڈیٹا[range].isciiLang;
            دلائل_2->converter->from_Unicode_حالت = 0x0000;
            /* now append ATR and language code */
            WRITE_مقصد_ہدف_FROM_U(دلائل_2,offsets,ماخذ,ہدف,ہدف_حد_بندی,ہدف_Byte_Unit,err);
            اگر (U_FAILURE(*err)) {
                توڑو;
            }
        }
        
        ماخذ_Char = *ماخذ++;
        عارضی_Context_From_Unicode = converter_ڈیٹا->contextCharFromUnicode;
        
        ہدف_Byte_Unit = missingCharMarker;
        
        /*check if input is in ASCII and C0 control codes range*/
        اگر (ماخذ_Char <= ASCII_انتہا) {
            دلائل_2->converter->from_Unicode_حالت = ماخذ_Char;
            WRITE_مقصد_ہدف_FROM_U(دلائل_2,offsets,ماخذ,ہدف,ہدف_حد_بندی,ماخذ_Char,err);
            اگر (U_FAILURE(*err)) {
                توڑو;
            }
            جاری;
        }
        انتخاب (ماخذ_Char) {
        حالت ZWNJ:
            /* contextChar has HALANT */
            اگر (converter_ڈیٹا->contextCharFromUnicode) {
                converter_ڈیٹا->contextCharFromUnicode = 0x00;
                ہدف_Byte_Unit = ISCII_HALANT;
            } ورنہ {
                /* consume ZWNJ and continue */
                converter_ڈیٹا->contextCharFromUnicode = 0x00;
                جاری;
            }
            توڑو;
        حالت ZWJ:
            /* contextChar has HALANT */
            اگر (converter_ڈیٹا->contextCharFromUnicode) {
                ہدف_Byte_Unit = ISCII_NUKTA;
            } ورنہ {
                ہدف_Byte_Unit =ISCII_INV;
            }
            converter_ڈیٹا->contextCharFromUnicode = 0x00;
            توڑو;
        طےشدہ:
            /* is the sourceChar in the INDIC_RANGE? */
            اگر ((uint16_t)(INDIC_بلاک_انتہا-ماخذ_Char) <= INDIC_RANGE) {
                /* Danda and Double Danda are valid in Northern scripts.. since Unicode
                 * does not include these codepoints in all Northern scrips we need to
                 * filter them out
                 */
                اگر (ماخذ_Char!= DANDA && ماخذ_Char != DOUBLE_DANDA) {
                    /* find out to which block the souceChar belongs*/
                    range =(uint16_t)((ماخذ_Char-INDIC_بلاک_BEGIN)/DELTA);
                    newDelta =(uint16_t)(range*DELTA);

                    /* Now are we in the same block as the previous? */
                    اگر (newDelta!= converter_ڈیٹا->currentDeltaFromUnicode || converter_ڈیٹا->is_پہلا_بفر) {
                        converter_ڈیٹا->currentDeltaFromUnicode = newDelta;
                        converter_ڈیٹا->current_نقاب_From_Unicode = lookup_Initial_ڈیٹا[range].نقاب_Enum_2;
                        delta_بدلا =سچ;
                        converter_ڈیٹا->is_پہلا_بفر=جھوٹ;
                    }
                    
                    اگر (converter_ڈیٹا->currentDeltaFromUnicode == PNJ_DELTA) { 
                        اگر (ماخذ_Char == PNJ_TIPPI) {
                            /* Make sure Tippi is converted to Bindi. */
                            ماخذ_Char = PNJ_BINDI;
                        } ورنہ اگر (ماخذ_Char == PNJ_ADHAK) {
                            /* This is for consonant cluster handling. */
                            converter_ڈیٹا->contextCharFromUnicode = PNJ_ADHAK;
                        }
                        
                    }
                    /* Normalize all Indic codepoints to Devanagari and map them to ISCII */
                    /* now subtract the new delta from sourceChar*/
                    ماخذ_Char -= converter_ڈیٹا->currentDeltaFromUnicode;
                }

                /* get the target byte unit */
                ہدف_Byte_Unit=from_Unicode_جدول[(uint8_t)ماخذ_Char];

                /* is the code point valid in current script? */
                اگر ((validity_جدول[(uint8_t)ماخذ_Char] & converter_ڈیٹا->current_نقاب_From_Unicode)==0) {
                    /* Vocallic RR is assigned in ISCII Telugu and Unicode */
                    اگر (converter_ڈیٹا->currentDeltaFromUnicode!=(TELUGU_DELTA) || ماخذ_Char!=VOCALLIC_RR) {
                        ہدف_Byte_Unit=missingCharMarker;
                    }
                }

                اگر (delta_بدلا) {
                    /* we are in a script block which is different than
                     * previous sourceChar's script block write ATR and language codes
                     */
                    uint32_t عارضی=0;
                    عارضی =(uint16_t)(ATR<<8);
                    عارضی += (uint16_t)((uint8_t) lookup_Initial_ڈیٹا[range].isciiLang);
                    /* reset */
                    delta_بدلا=جھوٹ;
                    /* now append ATR and language code */
                    WRITE_مقصد_ہدف_FROM_U(دلائل_2,offsets,ماخذ,ہدف,ہدف_حد_بندی,عارضی,err);
                    اگر (U_FAILURE(*err)) {
                        توڑو;
                    }
                }
                
                اگر (converter_ڈیٹا->currentDeltaFromUnicode == PNJ_DELTA && (ماخذ_Char + PNJ_DELTA) == PNJ_ADHAK) {
                    جاری;
                }
            }
            /* reset context char */
            converter_ڈیٹا->contextCharFromUnicode = 0x00;
            توڑو;
        }
        اگر (converter_ڈیٹا->currentDeltaFromUnicode == PNJ_DELTA && عارضی_Context_From_Unicode == PNJ_ADHAK && isPNJConsonant((ماخذ_Char + PNJ_DELTA))) {
            /* If the previous codepoint is Adhak and the current codepoint is a consonant, the targetByteUnit should be C + Halant + C. */
            /* reset context char */
            converter_ڈیٹا->contextCharFromUnicode = 0x0000;
            ہدف_Byte_Unit = ہدف_Byte_Unit << 16 | ISCII_HALANT << 8 | ہدف_Byte_Unit;
            /* write targetByteUnit to target */
            WRITE_مقصد_ہدف_FROM_U(دلائل_2, offsets, ماخذ, ہدف, ہدف_حد_بندی, ہدف_Byte_Unit,err);
            اگر (U_FAILURE(*err)) {
                توڑو;
            }
        } ورنہ اگر (ہدف_Byte_Unit != missingCharMarker) {
            اگر (ہدف_Byte_Unit==ISCII_HALANT) {
                converter_ڈیٹا->contextCharFromUnicode = (char16_t)ہدف_Byte_Unit;
            }
            /* write targetByteUnit to target*/
            WRITE_مقصد_ہدف_FROM_U(دلائل_2,offsets,ماخذ,ہدف,ہدف_حد_بندی,ہدف_Byte_Unit,err);
            اگر (U_FAILURE(*err)) {
                توڑو;
            }
        } ورنہ {
            /* oops.. the code point is unassigned */
            /*check if the char is a First surrogate*/
            اگر (U16_IS_SURROGATE(ماخذ_Char)) {
                اگر (U16_IS_SURROGATE_LEAD(ماخذ_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    اگر (ماخذ < ماخذ_حد_بندی) {
                        /* test the following code unit */
                        char16_t trail= (*ماخذ);
                        اگر (U16_IS_TRAIL(trail)) {
                            ماخذ++;
                            ماخذ_Char=U16_GET_SUPPLEMENTARY(ماخذ_Char, trail);
                            *err =U_INVALID_CHAR_ملا;
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } ورنہ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_ملا;
                        }
                    } ورنہ {
                        /* no more input */
                        *err = U_ZERO_ERROR;
                    }
                } ورنہ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_ملا;
                }
            } ورنہ {
                /* callback(unassigned) for a BMP code point */
                *err = U_INVALID_CHAR_ملا;
            }

            دلائل_2->converter->fromUChar32=ماخذ_Char;
            توڑو;
        }
    }/* end while(mySourceIndex<mySourceLength) */

    /*save the state and return */
    دلائل_2->ماخذ = ماخذ;
    دلائل_2->ہدف = (حرف*)ہدف;
}

ساکن ثابت uint16_t lookup_جدول[][2]={
    { ZERO,       ZERO     },     /*DEFAULT*/
    { ZERO,       ZERO     },     /*ROMAN*/
    { DEVANAGARI, DEV_نقاب },
    { BENGALI,    BNG_نقاب },
    { TAMIL,      TML_نقاب },
    { TELUGU,     KND_نقاب },
    { BENGALI,    BNG_نقاب },
    { ORIYA,      ORI_نقاب },
    { KANNADA,    KND_نقاب },
    { MALAYALAM,  MLM_نقاب },
    { GUJARATI,   GJR_نقاب },
    { GURMUKHI,   PNJ_نقاب }
};

#تعریف_کرو WRITE_مقصد_ہدف_مقصد_U(دلائل_2,ماخذ,ہدف,offsets,انحراف,ہدف_Uni_Char,delta, err) UPRV_بلاک_MACRO_BEGIN { \
    /* add offset to current Indic Block */                                              \
    اگر(ہدف_Uni_Char>ASCII_انتہا &&                                                        \
           ہدف_Uni_Char != ZWJ &&                                                       \
           ہدف_Uni_Char != ZWNJ &&                                                      \
           ہدف_Uni_Char != DANDA &&                                                     \
           ہدف_Uni_Char != DOUBLE_DANDA){                                               \
                                                                                         \
           ہدف_Uni_Char+=(uint16_t)(delta);                                             \
    }                                                                                    \
    /* now write the targetUniChar */                                                    \
    اگر(ہدف<دلائل_2->ہدف_حد_بندی){                                                        \
        *(ہدف)++ = (char16_t)ہدف_Uni_Char;                                              \
        اگر(offsets){                                                                     \
            *(offsets)++ = (int32_t)(انحراف);                                            \
        }                                                                                \
    }ورنہ{                                                                               \
        دلائل_2->converter->UChar_Error_بفر[دلائل_2->converter->UChar_Error_بفر_لمبائی++] =   \
            (char16_t)ہدف_Uni_Char;                                                        \
        *err = U_بفر_حد_سے_تجاوز_ERROR;                                                  \
    }                                                                                    \
} UPRV_بلاک_MACRO_انتہا

#تعریف_کرو GET_MAPPING(ماخذ_Char,ہدف_Uni_Char,ڈیٹا) UPRV_بلاک_MACRO_BEGIN {              \
    ہدف_Uni_Char = مقصد_Unicode_جدول[(ماخذ_Char)] ;                                       \
    /* is the code point valid in current script? */                                     \
    اگر(ماخذ_Char> ASCII_انتہا &&                                                          \
            (validity_جدول[(ہدف_Uni_Char & 0x7F)] & ڈیٹا->current_نقاب_مقصد_Unicode)==0){    \
        /* Vocallic RR is assigned in ISCII Telugu and Unicode */                         \
        اگر(ڈیٹا->current_Delta_مقصد_Unicode!=(TELUGU_DELTA) ||                                \
                    ہدف_Uni_Char!=VOCALLIC_RR){                                         \
            ہدف_Uni_Char=missingCharMarker;                                             \
        }                                                                                \
    }                                                                                    \
} UPRV_بلاک_MACRO_انتہا

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

ساکن خالی U_CALLCONV
UConverter_مقصد_Unicode_ISCII_OFFSETS_LOGIC(UConverter_مقصد_Unicode_دلائل *دلائل_2, UError_رمز* err) {
    ثابت حرف *ماخذ = ( حرف *) دلائل_2->ماخذ;
    char16_t *ہدف = دلائل_2->ہدف;
    ثابت حرف *ماخذ_حد_بندی = دلائل_2->ماخذ_حد_بندی;
    ثابت char16_t* ہدف_حد_بندی = دلائل_2->ہدف_حد_بندی;
    uint32_t ہدف_Uni_Char = 0x0000;
    uint8_t ماخذ_Char = 0x0000;
    UConverter_ڈیٹا_ISCII* ڈیٹا;
    UChar32* مقصد_Unicode_حالت=خالی_اشاریہ;
    UChar32 عارضی_ہدف_Uni_Char = 0x0000;
    char16_t* context_Char_مقصد_Unicode= خالی_اشاریہ;
    UBool ملا;
    صحیح i; 
    صحیح انحراف = 0;

    اگر ((دلائل_2->converter == خالی_اشاریہ) || (ہدف < دلائل_2->ہدف) || (ماخذ < دلائل_2->ماخذ)) {
        *err = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    ڈیٹا = (UConverter_ڈیٹا_ISCII*)(دلائل_2->converter->extraInfo);
    context_Char_مقصد_Unicode = &ڈیٹا->context_Char_مقصد_Unicode; /* contains previous ISCII codepoint visited */
    مقصد_Unicode_حالت = (UChar32*)&دلائل_2->converter->مقصد_Unicode_حالت;/* contains the mapping to Unicode of the above codepoint*/

    جب (U_SUCCESS(*err) && ماخذ<ماخذ_حد_بندی) {

        ہدف_Uni_Char = missingCharMarker;

        اگر (ہدف < ہدف_حد_بندی) {
            ماخذ_Char = (غیرعلامتی حرف)*(ماخذ)++;

            /* look at the post-context perform special processing */
            اگر (*context_Char_مقصد_Unicode==ATR) {

                /* If we have ATR in *contextCharToUnicode then we need to change our
                 * state to the Indic Script specified by sourceChar
                 */

                /* check if the sourceChar is supported script range*/
                اگر ((uint8_t)(PNJ-ماخذ_Char)<=PNJ-DEV) {
                    ڈیٹا->current_Delta_مقصد_Unicode = (uint16_t)(lookup_جدول[ماخذ_Char & 0x0F][0] * DELTA);
                    ڈیٹا->current_نقاب_مقصد_Unicode = (نقاب_Enum)lookup_جدول[ماخذ_Char & 0x0F][1];
                } ورنہ اگر (ماخذ_Char==DEF) {
                    /* switch back to default */
                    ڈیٹا->current_Delta_مقصد_Unicode = ڈیٹا->def_Delta_مقصد_Unicode;
                    ڈیٹا->current_نقاب_مقصد_Unicode = ڈیٹا->def_نقاب_مقصد_Unicode;
                } ورنہ {
                    اگر ((ماخذ_Char >= 0x21 && ماخذ_Char <= 0x3F)) {
                        /* these are display codes consume and continue */
                    } ورنہ {
                        *err =U_ILLEGAL_CHAR_ملا;
                        /* reset */
                        *context_Char_مقصد_Unicode=NO_CHAR_MARKER;
                        جاؤ CALLBACK;
                    }
                }

                /* reset */
                *context_Char_مقصد_Unicode=NO_CHAR_MARKER;

                جاری;

            } ورنہ اگر (*context_Char_مقصد_Unicode==EXT) {
                /* check if sourceChar is in 0xA1-0xEE range */
                اگر ((uint8_t) (EXT_RANGE_انتہا - ماخذ_Char) <= (EXT_RANGE_انتہا - EXT_RANGE_BEGIN)) {
                    /* We currently support only Anudatta and Devanagari abbreviation sign */
                    اگر (ماخذ_Char==0xBF || ماخذ_Char == 0xB8) {
                        ہدف_Uni_Char = (ماخذ_Char==0xBF) ? DEV_ABBR_SIGN : DEV_ANUDATTA;
                        
                        /* find out if the mapping is valid in this state */
                        اگر (validity_جدول[(uint8_t)ہدف_Uni_Char] & ڈیٹا->current_نقاب_مقصد_Unicode) {
                            *context_Char_مقصد_Unicode= NO_CHAR_MARKER;

                            /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                            اگر (ڈیٹا->پچھلا_مقصد_Unicode_حالت) {
                                WRITE_مقصد_ہدف_مقصد_U(دلائل_2,ماخذ,ہدف,دلائل_2->offsets,(ماخذ-دلائل_2->ماخذ -1),ڈیٹا->پچھلا_مقصد_Unicode_حالت,0,err);
                                ڈیٹا->پچھلا_مقصد_Unicode_حالت = 0x0000;
                            }
                            /* write to target */
                            WRITE_مقصد_ہدف_مقصد_U(دلائل_2,ماخذ,ہدف,دلائل_2->offsets,(ماخذ-دلائل_2->ماخذ -2),ہدف_Uni_Char,ڈیٹا->current_Delta_مقصد_Unicode,err);

                            جاری;
                        }
                    }
                    /* byte unit is unassigned */
                    ہدف_Uni_Char = missingCharMarker;
                    *err= U_INVALID_CHAR_ملا;
                } ورنہ {
                    /* only 0xA1 - 0xEE are legal after EXT char */
                    *context_Char_مقصد_Unicode= NO_CHAR_MARKER;
                    *err = U_ILLEGAL_CHAR_ملا;
                }
                جاؤ CALLBACK;
            } ورنہ اگر (*context_Char_مقصد_Unicode==ISCII_INV) {
                اگر (ماخذ_Char==ISCII_HALANT) {
                    ہدف_Uni_Char = 0x0020; /* replace with space according to Indic FAQ */
                } ورنہ {
                    ہدف_Uni_Char = ZWJ;
                }

                /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                اگر (ڈیٹا->پچھلا_مقصد_Unicode_حالت) {
                    WRITE_مقصد_ہدف_مقصد_U(دلائل_2,ماخذ,ہدف,دلائل_2->offsets,(ماخذ-دلائل_2->ماخذ -1),ڈیٹا->پچھلا_مقصد_Unicode_حالت,0,err);
                    ڈیٹا->پچھلا_مقصد_Unicode_حالت = 0x0000;
                }
                /* write to target */
                WRITE_مقصد_ہدف_مقصد_U(دلائل_2,ماخذ,ہدف,دلائل_2->offsets,(ماخذ-دلائل_2->ماخذ -2),ہدف_Uni_Char,ڈیٹا->current_Delta_مقصد_Unicode,err);
                /* reset */
                *context_Char_مقصد_Unicode=NO_CHAR_MARKER;
            }

            /* look at the pre-context and perform special processing */
            انتخاب (ماخذ_Char) {
            حالت ISCII_INV:
            حالت EXT:
            حالت ATR:
                *context_Char_مقصد_Unicode = (char16_t)ماخذ_Char;

                اگر (*مقصد_Unicode_حالت != missingCharMarker) {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    اگر (ڈیٹا->پچھلا_مقصد_Unicode_حالت) {
                        WRITE_مقصد_ہدف_مقصد_U(دلائل_2,ماخذ,ہدف,دلائل_2->offsets,(ماخذ-دلائل_2->ماخذ -1),ڈیٹا->پچھلا_مقصد_Unicode_حالت,0,err);
                        ڈیٹا->پچھلا_مقصد_Unicode_حالت = 0x0000;
                    }
                    WRITE_مقصد_ہدف_مقصد_U(دلائل_2,ماخذ,ہدف,دلائل_2->offsets,(ماخذ-دلائل_2->ماخذ -2),*مقصد_Unicode_حالت,ڈیٹا->current_Delta_مقصد_Unicode,err);
                    *مقصد_Unicode_حالت = missingCharMarker;
                }
                جاری;
            حالت ISCII_DANDA:
                /* handle double danda*/
                اگر (*context_Char_مقصد_Unicode== ISCII_DANDA) {
                    ہدف_Uni_Char = DOUBLE_DANDA;
                    /* clear the context */
                    *context_Char_مقصد_Unicode = NO_CHAR_MARKER;
                    *مقصد_Unicode_حالت = missingCharMarker;
                } ورنہ {
                    GET_MAPPING(ماخذ_Char,ہدف_Uni_Char,ڈیٹا);
                    *context_Char_مقصد_Unicode = ماخذ_Char;
                }
                توڑو;
            حالت ISCII_HALANT:
                /* handle explicit halant */
                اگر (*context_Char_مقصد_Unicode == ISCII_HALANT) {
                    ہدف_Uni_Char = ZWNJ;
                    /* clear the context */
                    *context_Char_مقصد_Unicode = NO_CHAR_MARKER;
                } ورنہ {
                    GET_MAPPING(ماخذ_Char,ہدف_Uni_Char,ڈیٹا);
                    *context_Char_مقصد_Unicode = ماخذ_Char;
                }
                توڑو;
            حالت 0x0A:
            حالت 0x0D:
                ڈیٹا->reset_مقصد_Default_مقصد_Unicode = سچ;
                GET_MAPPING(ماخذ_Char,ہدف_Uni_Char,ڈیٹا)
                ;
                *context_Char_مقصد_Unicode = ماخذ_Char;
                توڑو;

            حالت ISCII_VOWEL_SIGN_E:
                i=1;
                ملا=جھوٹ;
                برائے (; i<vowelSignESpecialCases[0][0]; i++) {
                    U_ASSERT(i<UPRV_LENGTHOF(vowelSignESpecialCases));
                    اگر (vowelSignESpecialCases[i][0]==(uint8_t)*context_Char_مقصد_Unicode) {
                        ہدف_Uni_Char=vowelSignESpecialCases[i][1];
                        ملا=سچ;
                        توڑو;
                    }
                }
                اگر (ملا) {
                    /* find out if the mapping is valid in this state */
                    اگر (validity_جدول[(uint8_t)ہدف_Uni_Char] & ڈیٹا->current_نقاب_مقصد_Unicode) {
                        /*targetUniChar += data->currentDeltaToUnicode ;*/
                        *context_Char_مقصد_Unicode= NO_CHAR_MARKER;
                        *مقصد_Unicode_حالت = missingCharMarker;
                        توڑو;
                    }
                }
                GET_MAPPING(ماخذ_Char,ہدف_Uni_Char,ڈیٹا);
                *context_Char_مقصد_Unicode = ماخذ_Char;
                توڑو;

            حالت ISCII_NUKTA:
                /* handle soft halant */
                اگر (*context_Char_مقصد_Unicode == ISCII_HALANT) {
                    ہدف_Uni_Char = ZWJ;
                    /* clear the context */
                    *context_Char_مقصد_Unicode = NO_CHAR_MARKER;
                    توڑو;
                } ورنہ اگر (ڈیٹا->current_Delta_مقصد_Unicode == PNJ_DELTA && ڈیٹا->context_Char_مقصد_Unicode == 0xc0) {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    اگر (ڈیٹا->پچھلا_مقصد_Unicode_حالت) {
                        WRITE_مقصد_ہدف_مقصد_U(دلائل_2,ماخذ,ہدف,دلائل_2->offsets,(ماخذ-دلائل_2->ماخذ -1),ڈیٹا->پچھلا_مقصد_Unicode_حالت,0,err);
                        ڈیٹا->پچھلا_مقصد_Unicode_حالت = 0x0000;
                    }
                    /* We got here because ISCII_NUKTA was preceded by 0xc0 and we are converting Gurmukhi.
                     * In that case we must convert (0xc0 0xe9) to (\u0a5c\u0a4d\u0a39).
                     */
                    ہدف_Uni_Char = PNJ_RRA;
                    WRITE_مقصد_ہدف_مقصد_U(دلائل_2, ماخذ, ہدف, دلائل_2->offsets, (ماخذ-دلائل_2->ماخذ)-2, ہدف_Uni_Char, 0, err);
                    اگر (U_SUCCESS(*err)) {
                        ہدف_Uni_Char = PNJ_SIGN_VIRAMA;
                        WRITE_مقصد_ہدف_مقصد_U(دلائل_2, ماخذ, ہدف, دلائل_2->offsets, (ماخذ-دلائل_2->ماخذ)-2, ہدف_Uni_Char, 0, err);
                        اگر (U_SUCCESS(*err)) {
                            ہدف_Uni_Char = PNJ_HA;
                            WRITE_مقصد_ہدف_مقصد_U(دلائل_2, ماخذ, ہدف, دلائل_2->offsets, (ماخذ-دلائل_2->ماخذ)-2, ہدف_Uni_Char, 0, err);
                        } ورنہ {
                            دلائل_2->converter->UChar_Error_بفر[دلائل_2->converter->UChar_Error_بفر_لمبائی++]= PNJ_HA;
                        }
                    } ورنہ {
                        دلائل_2->converter->UChar_Error_بفر[دلائل_2->converter->UChar_Error_بفر_لمبائی++]= PNJ_SIGN_VIRAMA;
                        دلائل_2->converter->UChar_Error_بفر[دلائل_2->converter->UChar_Error_بفر_لمبائی++]= PNJ_HA;
                    }
                    *مقصد_Unicode_حالت = missingCharMarker;
                    ڈیٹا->context_Char_مقصد_Unicode = NO_CHAR_MARKER;
                    جاری;
                } ورنہ {
                    /* try to handle <CHAR> + ISCII_NUKTA special mappings */
                    i=1;
                    ملا =جھوٹ;
                    برائے (; i<nuktaSpecialCases[0][0]; i++) {
                        اگر (nuktaSpecialCases[i][0]==(uint8_t)
                                *context_Char_مقصد_Unicode) {
                            ہدف_Uni_Char=nuktaSpecialCases[i][1];
                            ملا =سچ;
                            توڑو;
                        }
                    }
                    اگر (ملا) {
                        /* find out if the mapping is valid in this state */
                        اگر (validity_جدول[(uint8_t)ہدف_Uni_Char] & ڈیٹا->current_نقاب_مقصد_Unicode) {
                            /*targetUniChar += data->currentDeltaToUnicode ;*/
                            *context_Char_مقصد_Unicode= NO_CHAR_MARKER;
                            *مقصد_Unicode_حالت = missingCharMarker;
                            اگر (ڈیٹا->current_Delta_مقصد_Unicode == PNJ_DELTA) {
                                /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                                اگر (ڈیٹا->پچھلا_مقصد_Unicode_حالت) {
                                    WRITE_مقصد_ہدف_مقصد_U(دلائل_2,ماخذ,ہدف,دلائل_2->offsets,(ماخذ-دلائل_2->ماخذ -1),ڈیٹا->پچھلا_مقصد_Unicode_حالت,0,err);
                                    ڈیٹا->پچھلا_مقصد_Unicode_حالت = 0x0000;
                                }
                                WRITE_مقصد_ہدف_مقصد_U(دلائل_2,ماخذ,ہدف,دلائل_2->offsets,(ماخذ-دلائل_2->ماخذ -2),ہدف_Uni_Char,ڈیٹا->current_Delta_مقصد_Unicode,err);
                                جاری;
                            }
                            توڑو;
                        }
                        /* else fall through to default */
                    }
                    /* else fall through to default */
                    U_FALLTHROUGH;
                }
            طےشدہ:GET_MAPPING(ماخذ_Char,ہدف_Uni_Char,ڈیٹا)
                ;
                *context_Char_مقصد_Unicode = ماخذ_Char;
                توڑو;
            }

            اگر (*مقصد_Unicode_حالت != missingCharMarker) {
                /* Check to make sure that consonant clusters are handled correct for Gurmukhi script. */
                اگر (ڈیٹا->current_Delta_مقصد_Unicode == PNJ_DELTA && ڈیٹا->پچھلا_مقصد_Unicode_حالت != 0 && isPNJConsonant(ڈیٹا->پچھلا_مقصد_Unicode_حالت) &&
                        (*مقصد_Unicode_حالت + PNJ_DELTA) == PNJ_SIGN_VIRAMA && ((UChar32)(ہدف_Uni_Char + PNJ_DELTA) == ڈیٹا->پچھلا_مقصد_Unicode_حالت)) {
                    /* Consonant clusters C + HALANT + C should be encoded as ADHAK + C */
                    انحراف = (صحیح)(ماخذ-دلائل_2->ماخذ - 3);
                    عارضی_ہدف_Uni_Char = PNJ_ADHAK; /* This is necessary to avoid some compiler warnings. */
                    WRITE_مقصد_ہدف_مقصد_U(دلائل_2,ماخذ,ہدف,دلائل_2->offsets,انحراف,عارضی_ہدف_Uni_Char,0,err);
                    WRITE_مقصد_ہدف_مقصد_U(دلائل_2,ماخذ,ہدف,دلائل_2->offsets,انحراف,ڈیٹا->پچھلا_مقصد_Unicode_حالت,0,err);
                    ڈیٹا->پچھلا_مقصد_Unicode_حالت = 0x0000; /* reset the previous unicode code point */
                    *مقصد_Unicode_حالت = missingCharMarker;
                    جاری;
                } ورنہ {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    اگر (ڈیٹا->پچھلا_مقصد_Unicode_حالت) {
                        WRITE_مقصد_ہدف_مقصد_U(دلائل_2,ماخذ,ہدف,دلائل_2->offsets,(ماخذ-دلائل_2->ماخذ -1),ڈیٹا->پچھلا_مقصد_Unicode_حالت,0,err);
                        ڈیٹا->پچھلا_مقصد_Unicode_حالت = 0x0000;
                    }
                    /* Check to make sure that Bindi and Tippi are handled correctly for Gurmukhi script. 
                     * If 0xA2 is preceded by a codepoint in the PNJ_BINDI_TIPPI_SET then the target codepoint should be Tippi instead of Bindi.
                     */
                    اگر (ڈیٹا->current_Delta_مقصد_Unicode == PNJ_DELTA && (ہدف_Uni_Char + PNJ_DELTA) == PNJ_BINDI && isPNJBindiTippi((*مقصد_Unicode_حالت + PNJ_DELTA))) {
                        ہدف_Uni_Char = PNJ_TIPPI - PNJ_DELTA;
                        WRITE_مقصد_ہدف_مقصد_U(دلائل_2,ماخذ,ہدف,دلائل_2->offsets,(ماخذ-دلائل_2->ماخذ -2),*مقصد_Unicode_حالت,PNJ_DELTA,err);
                    } ورنہ اگر (ڈیٹا->current_Delta_مقصد_Unicode == PNJ_DELTA && (ہدف_Uni_Char + PNJ_DELTA) == PNJ_SIGN_VIRAMA && isPNJConsonant((*مقصد_Unicode_حالت + PNJ_DELTA))) {
                        /* Store the current toUnicodeStatus code point for later handling of consonant cluster in Gurmukhi. */
                        ڈیٹا->پچھلا_مقصد_Unicode_حالت = *مقصد_Unicode_حالت + PNJ_DELTA;
                    } ورنہ {
                        /* write the previously mapped codepoint */
                        WRITE_مقصد_ہدف_مقصد_U(دلائل_2,ماخذ,ہدف,دلائل_2->offsets,(ماخذ-دلائل_2->ماخذ -2),*مقصد_Unicode_حالت,ڈیٹا->current_Delta_مقصد_Unicode,err);
                    }
                }
                *مقصد_Unicode_حالت = missingCharMarker;
            }

            اگر (ہدف_Uni_Char != missingCharMarker) {
                /* now save the targetUniChar for delayed write */
                *مقصد_Unicode_حالت = (char16_t) ہدف_Uni_Char;
                اگر (ڈیٹا->reset_مقصد_Default_مقصد_Unicode) {
                    ڈیٹا->current_Delta_مقصد_Unicode = ڈیٹا->def_Delta_مقصد_Unicode;
                    ڈیٹا->current_نقاب_مقصد_Unicode = ڈیٹا->def_نقاب_مقصد_Unicode;
                    ڈیٹا->reset_مقصد_Default_مقصد_Unicode=جھوٹ;
                }
            } ورنہ {

                /* we reach here only if targetUniChar == missingCharMarker
                 * so assign codes to reason and err
                 */
                *err = U_INVALID_CHAR_ملا;
CALLBACK:
                دلائل_2->converter->مقصد_UBytes[0] = ماخذ_Char;
                دلائل_2->converter->مقصد_ULength = 1;
                توڑو;
            }

        } ورنہ {
            *err =U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
        }
    }

    اگر (U_SUCCESS(*err) && دلائل_2->flush && ماخذ == ماخذ_حد_بندی) {
        /* end of the input stream */
        UConverter *cnv = دلائل_2->converter;

        اگر (*context_Char_مقصد_Unicode==ATR || *context_Char_مقصد_Unicode==EXT || *context_Char_مقصد_Unicode==ISCII_INV) {
            /* set toUBytes[] */
            cnv->مقصد_UBytes[0] = (uint8_t)*context_Char_مقصد_Unicode;
            cnv->مقصد_ULength = 1;

            /* avoid looping on truncated sequences */
            *context_Char_مقصد_Unicode = NO_CHAR_MARKER;
        } ورنہ {
            cnv->مقصد_ULength = 0;
        }

        اگر (*مقصد_Unicode_حالت != missingCharMarker) {
            /* output a remaining target character */
            WRITE_مقصد_ہدف_مقصد_U(دلائل_2,ماخذ,ہدف,دلائل_2->offsets,(ماخذ - دلائل_2->ماخذ -1),*مقصد_Unicode_حالت,ڈیٹا->current_Delta_مقصد_Unicode,err);
            *مقصد_Unicode_حالت = missingCharMarker;
        }
    }

    دلائل_2->ہدف = ہدف;
    دلائل_2->ماخذ = ماخذ;
}

/* structure for SafeClone calculations */
ساخت cloneISCIIStruct {
    UConverter cnv;
    UConverter_ڈیٹا_ISCII mydata;
};

ساکن UConverter * U_CALLCONV
ISCII_محفوظ_Clone(ثابت UConverter *cnv,
              خالی *ڈھیر_بفر,
              int32_t *p_بفر_سائز,
              UError_رمز *حالت_2)
{
    ساخت cloneISCIIStruct * localClone;
    int32_t بفر_سائز_Needed = حجم(ساخت cloneISCIIStruct);

    اگر (U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }

    اگر (*p_بفر_سائز == 0) { /* 'preflighting' request - set needed size into *pBufferSize */
        *p_بفر_سائز = بفر_سائز_Needed;
        واپس خالی_اشاریہ;
    }

    localClone = (ساخت cloneISCIIStruct *)ڈھیر_بفر;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, حجم(UConverter_ڈیٹا_ISCII));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = سچ;

    واپس &localClone->cnv;
}

ساکن خالی U_CALLCONV
ISCIIGet_Unicode_سیٹ(ثابت UConverter *cnv,
                    ثابت USetAdder *sa,
                    UConverter_Unicode_سیٹ which,
                    UError_رمز *p_Error_رمز)
{
    (خالی)cnv;
    (خالی)which;
    (خالی)p_Error_رمز;
    int32_t idx, script;
    uint8_t mask;

    /* Since all ISCII versions allow switching to other ISCII
    scripts, we add all roundtrippable characters to this set. */
    sa->جوڑو_Range(sa->سیٹ, 0, ASCII_انتہا);
    برائے (script = DEVANAGARI; script <= MALAYALAM; script++) {
        mask = (uint8_t)(lookup_Initial_ڈیٹا[script].نقاب_Enum_2);
        برائے (idx = 0; idx < DELTA; idx++) {
            /* added check for TELUGU character */
            اگر ((validity_جدول[idx] & mask) || (script==TELUGU && idx==0x31)) {
                sa->جوڑو(sa->سیٹ, idx + (script * DELTA) + INDIC_بلاک_BEGIN);
            }
        }
    }
    sa->جوڑو(sa->سیٹ, DANDA);
    sa->جوڑو(sa->سیٹ, DOUBLE_DANDA);
    sa->جوڑو(sa->سیٹ, ZWNJ);
    sa->جوڑو(sa->سیٹ, ZWJ);
}
U_CDECL_انتہا
ساکن ثابت UConverterImpl _ISCIIImpl={

    UCNV_ISCII,

    خالی_اشاریہ,
    خالی_اشاریہ,

    _ISCIIOpen,
    _ISCIIClose,
    _ISCIIReset,

    UConverter_مقصد_Unicode_ISCII_OFFSETS_LOGIC,
    UConverter_مقصد_Unicode_ISCII_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISCII_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISCII_OFFSETS_LOGIC,
    خالی_اشاریہ,

    خالی_اشاریہ,
    _ISCIIgetName,
    خالی_اشاریہ,
    ISCII_محفوظ_Clone,
    ISCIIGet_Unicode_سیٹ,
    خالی_اشاریہ,
    خالی_اشاریہ
};

ساکن ثابت UConverter_Static_ڈیٹا ISCIIStatic_ڈیٹا={
    حجم(UConverter_Static_ڈیٹا),
        "ISCII",
         0,
         UCNV_IBM,
         UCNV_ISCII,
         1,
         4,
        { 0x1a, 0, 0, 0 },
        0x1,
        جھوٹ,
        جھوٹ,
        0x0,
        0x0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

ثابت UConverter_Shared_ڈیٹا _ISCIIData=
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&ISCIIStatic_ڈیٹا, &_ISCIIImpl);

#اگر_ختم /* #if !UCONFIG_NO_LEGACY_CONVERSION */
