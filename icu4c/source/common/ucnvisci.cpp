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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/ucnv_cb.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "uassert.h"

#నిర్వచించు UCNV_OPTIONS_సంస్కరణ_ముసుగు 0xf
#నిర్వచించు NUKTA               0x093c
#నిర్వచించు HALANT              0x094d
#నిర్వచించు ZWNJ                0x200c /* Zero Width Non Joiner */
#నిర్వచించు ZWJ                 0x200d /* Zero width Joiner */
#నిర్వచించు INVALID_CHAR        0xffff
#నిర్వచించు ATR                 0xEF   /* Attribute code */
#నిర్వచించు EXT                 0xF0   /* Extension code */
#నిర్వచించు DANDA               0x0964
#నిర్వచించు DOUBLE_DANDA        0x0965
#నిర్వచించు ISCII_NUKTA         0xE9
#నిర్వచించు ISCII_HALANT        0xE8
#నిర్వచించు ISCII_DANDA         0xEA
#నిర్వచించు ISCII_INV           0xD9
#నిర్వచించు ISCII_VOWEL_SIGN_E  0xE0
#నిర్వచించు INDIC_ఖండం_BEGIN   0x0900
#నిర్వచించు INDIC_ఖండం_చివర     0x0D7F
#నిర్వచించు INDIC_RANGE         (INDIC_ఖండం_చివర - INDIC_ఖండం_BEGIN)
#నిర్వచించు VOCALLIC_RR         0x0931
#నిర్వచించు LF                  0x0A
#నిర్వచించు ASCII_చివర           0xA0
#నిర్వచించు NO_CHAR_MARKER      0xFFFE
#నిర్వచించు TELUGU_DELTA        DELTA * TELUGU
#నిర్వచించు DEV_ABBR_SIGN       0x0970
#నిర్వచించు DEV_ANUDATTA        0x0952
#నిర్వచించు EXT_RANGE_BEGIN     0xA1
#నిర్వచించు EXT_RANGE_చివర       0xEE

#నిర్వచించు PNJ_DELTA           0x0100
#నిర్వచించు PNJ_BINDI           0x0A02
#నిర్వచించు PNJ_TIPPI           0x0A70
#నిర్వచించు PNJ_SIGN_VIRAMA     0x0A4D
#నిర్వచించు PNJ_ADHAK           0x0A71
#నిర్వచించు PNJ_HA              0x0A39
#నిర్వచించు PNJ_RRA             0x0A5C

రకనిర్వచనం గణన {
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
రకనిర్వచనం గణన {
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

రకనిర్వచనం గణన {
    DEV_ముసుగు =0x80,
    PNJ_ముసుగు =0x40,
    GJR_ముసుగు =0x20,
    ORI_ముసుగు =0x10,
    BNG_ముసుగు =0x08,
    KND_ముసుగు =0x04,
    MLM_ముసుగు =0x02,
    TML_ముసుగు =0x01,
    ZERO =0x00
}ముసుగు_Enum;

#నిర్వచించు ISCII_CNV_ఉపసర్గ "ISCII,version="

రకనిర్వచనం నిర్మాణం {
    char16_t context_Char_గమ్యం_Unicode;         /* previous Unicode codepoint for contextual analysis */
    char16_t contextCharFromUnicode;       /* previous Unicode codepoint for contextual analysis */
    uint16_t def_Delta_గమ్యం_Unicode;         /* delta for switching to default state when DEF is encountered  */
    uint16_t currentDeltaFromUnicode;   /* current delta in Indic block */
    uint16_t current_Delta_గమ్యం_Unicode;     /* current delta in Indic block */
    ముసుగు_Enum current_ముసుగు_From_Unicode;    /* mask for current state in toUnicode */
    ముసుగు_Enum current_ముసుగు_గమ్యం_Unicode;      /* mask for current state in toUnicode */
    ముసుగు_Enum def_ముసుగు_గమ్యం_Unicode;          /* mask for default state in toUnicode */
    UBool is_మొదటి_బఫరు;                /* boolean for fromUnicode to see if we need to announce the first script */
    UBool reset_గమ్యం_Default_గమ్యం_Unicode;      /* boolean for resetting to default delta and mask when a newline is encountered*/
    అక్షరం name[పరిమాణం(ISCII_CNV_ఉపసర్గ) + 1];
    UChar32 prev_గమ్యం_Unicode_స్థితి;        /* Hold the previous toUnicodeStatus. This is necessary because we may need to know the last two code points. */
} UConverter_దత్తాంశం_ISCII;

రకనిర్వచనం నిర్మాణం Lookup_దత్తాంశం_Struct {
    UniLang uniLang;
    ముసుగు_Enum ముసుగు_Enum_2;
    ISCIILang isciiLang;
} Lookup_దత్తాంశం_Struct;

స్థిర స్థిరరాశి Lookup_దత్తాంశం_Struct lookup_Initial_దత్తాంశం[]={
    { DEVANAGARI, DEV_ముసుగు,  DEV },
    { BENGALI,    BNG_ముసుగు,  BNG },
    { GURMUKHI,   PNJ_ముసుగు,  PNJ },
    { GUJARATI,   GJR_ముసుగు,  GJR },
    { ORIYA,      ORI_ముసుగు,  ORI },
    { TAMIL,      TML_ముసుగు,  TML },
    { TELUGU,     KND_ముసుగు,  TLG },
    { KANNADA,    KND_ముసుగు,  KND },
    { MALAYALAM,  MLM_ముసుగు,  MLM }
};

/*
 * For special handling of certain Gurmukhi characters.
 * Bit 0 (value 1): PNJ consonant
 * Bit 1 (value 2): PNJ Bindi Tippi
 */
స్థిర స్థిరరాశి uint8_t pnjMap[80] = {
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

స్థిర UBool
isPNJConsonant(UChar32 c) {
    ఒకవేళ (c < 0xa00 || 0xa50 <= c) {
        తిరిగి అబద్ధం;
    } లేకపోతే {
        తిరిగి pnjMap[c - 0xa00] & 1;
    }
}

స్థిర UBool
isPNJBindiTippi(UChar32 c) {
    ఒకవేళ (c < 0xa00 || 0xa50 <= c) {
        తిరిగి అబద్ధం;
    } లేకపోతే {
        తిరిగి pnjMap[c - 0xa00] >> 1;
    }
}
U_CDECL_BEGIN
స్థిర శూన్య  U_CALLCONV
_ISCIIOpen(UConverter *cnv, UConverter_ఎక్కించు_వాదనలు *p_వాదనలు, UError_సంకేతం *error_సంకేతం) {
    ఒకవేళ(p_వాదనలు->onlyTestIsLoadable) {
        తిరిగి;
    }

    cnv->extraInfo = uprv_malloc(పరిమాణం(UConverter_దత్తాంశం_ISCII));

    ఒకవేళ (cnv->extraInfo != శూన్యసూచిక) {
        int32_t len=0;
        UConverter_దత్తాంశం_ISCII *converter_దత్తాంశం=
                (UConverter_దత్తాంశం_ISCII *) cnv->extraInfo;
        converter_దత్తాంశం->context_Char_గమ్యం_Unicode=NO_CHAR_MARKER;
        cnv->గమ్యం_Unicode_స్థితి = missingCharMarker;
        converter_దత్తాంశం->contextCharFromUnicode=0x0000;
        converter_దత్తాంశం->reset_గమ్యం_Default_గమ్యం_Unicode=అబద్ధం;
        /* check if the version requested is supported */
        ఒకవేళ ((p_వాదనలు->options & UCNV_OPTIONS_సంస్కరణ_ముసుగు) < 9) {
            /* initialize state variables */
            converter_దత్తాంశం->currentDeltaFromUnicode
                    = converter_దత్తాంశం->current_Delta_గమ్యం_Unicode
                            = converter_దత్తాంశం->def_Delta_గమ్యం_Unicode = (uint16_t)(lookup_Initial_దత్తాంశం[p_వాదనలు->options & UCNV_OPTIONS_సంస్కరణ_ముసుగు].uniLang * DELTA);

            converter_దత్తాంశం->current_ముసుగు_From_Unicode
                    = converter_దత్తాంశం->current_ముసుగు_గమ్యం_Unicode
                            = converter_దత్తాంశం->def_ముసుగు_గమ్యం_Unicode = lookup_Initial_దత్తాంశం[p_వాదనలు->options & UCNV_OPTIONS_సంస్కరణ_ముసుగు].ముసుగు_Enum_2;
            
            converter_దత్తాంశం->is_మొదటి_బఫరు=నిజం;
            (శూన్య)uprv_strcpy(converter_దత్తాంశం->name, ISCII_CNV_ఉపసర్గ);
            len = (int32_t)uprv_strlen(converter_దత్తాంశం->name);
            converter_దత్తాంశం->name[len]= (అక్షరం)((p_వాదనలు->options & UCNV_OPTIONS_సంస్కరణ_ముసుగు) + '0');
            converter_దత్తాంశం->name[len+1]=0;
            
            converter_దత్తాంశం->prev_గమ్యం_Unicode_స్థితి = 0x0000;
        } లేకపోతే {
            uprv_free(cnv->extraInfo);
            cnv->extraInfo = శూన్యసూచిక;
            *error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        }

    } లేకపోతే {
        *error_సంకేతం =U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
}

స్థిర శూన్య U_CALLCONV
_ISCIIClose(UConverter *cnv) {
    ఒకవేళ (cnv->extraInfo!=శూన్యసూచిక) {
        ఒకవేళ (!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo=శూన్యసూచిక;
    }
}

స్థిర స్థిరరాశి అక్షరం*  U_CALLCONV
_ISCIIgetName(స్థిరరాశి UConverter* cnv) {
    ఒకవేళ (cnv->extraInfo) {
        UConverter_దత్తాంశం_ISCII* my_దత్తాంశం= (UConverter_దత్తాంశం_ISCII*)cnv->extraInfo;
        తిరిగి my_దత్తాంశం->name;
    }
    తిరిగి శూన్యసూచిక;
}

స్థిర శూన్య U_CALLCONV
_ISCIIReset(UConverter *cnv, UConverterResetChoice choice) {
    UConverter_దత్తాంశం_ISCII* దత్తాంశం =(UConverter_దత్తాంశం_ISCII *) (cnv->extraInfo);
    ఒకవేళ (choice<=UCNV_RESET_గమ్యం_UNICODE) {
        cnv->గమ్యం_Unicode_స్థితి = missingCharMarker;
        cnv->బహుళకం=0;
        దత్తాంశం->current_Delta_గమ్యం_Unicode=దత్తాంశం->def_Delta_గమ్యం_Unicode;
        దత్తాంశం->current_ముసుగు_గమ్యం_Unicode = దత్తాంశం->def_ముసుగు_గమ్యం_Unicode;
        దత్తాంశం->context_Char_గమ్యం_Unicode=NO_CHAR_MARKER;
        దత్తాంశం->prev_గమ్యం_Unicode_స్థితి = 0x0000;
    }
    ఒకవేళ (choice!=UCNV_RESET_గమ్యం_UNICODE) {
        cnv->fromUChar32=0x0000;
        దత్తాంశం->contextCharFromUnicode=0x00;
        దత్తాంశం->current_ముసుగు_From_Unicode=దత్తాంశం->def_ముసుగు_గమ్యం_Unicode;
        దత్తాంశం->currentDeltaFromUnicode=దత్తాంశం->def_Delta_గమ్యం_Unicode;
        దత్తాంశం->is_మొదటి_బఫరు=నిజం;
        దత్తాంశం->reset_గమ్యం_Default_గమ్యం_Unicode=అబద్ధం;
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

స్థిర స్థిరరాశి uint8_t validity_పట్టిక[128] = {
/* This state table is tool generated please do not edit unless you know exactly what you are doing */
/* Note: This table was edited to mirror the Windows XP implementation */
/*ISCII:Valid:Unicode */
/*0xa0 : 0x00: 0x900  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xa1 : 0xb8: 0x901  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + ZERO     + ZERO     + ZERO     ,
/*0xa2 : 0xfe: 0x902  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xa3 : 0xbf: 0x903  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0x00 : 0x00: 0x904  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xa4 : 0xff: 0x905  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xa5 : 0xff: 0x906  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xa6 : 0xff: 0x907  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xa7 : 0xff: 0x908  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xa8 : 0xff: 0x909  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xa9 : 0xff: 0x90a  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xaa : 0xfe: 0x90b  */ DEV_ముసుగు + ZERO     + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0x00 : 0x00: 0x90c  */ DEV_ముసుగు + ZERO     + ZERO     + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0xae : 0x80: 0x90d  */ DEV_ముసుగు + ZERO     + GJR_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xab : 0x87: 0x90e  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xac : 0xff: 0x90f  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xad : 0xff: 0x910  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xb2 : 0x80: 0x911  */ DEV_ముసుగు + ZERO     + GJR_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xaf : 0x87: 0x912  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xb0 : 0xff: 0x913  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xb1 : 0xff: 0x914  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xb3 : 0xff: 0x915  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xb4 : 0xfe: 0x916  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0xb5 : 0xfe: 0x917  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0xb6 : 0xfe: 0x918  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0xb7 : 0xff: 0x919  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xb8 : 0xff: 0x91a  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xb9 : 0xfe: 0x91b  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0xba : 0xff: 0x91c  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xbb : 0xfe: 0x91d  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0xbc : 0xff: 0x91e  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xbd : 0xff: 0x91f  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xbe : 0xfe: 0x920  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0xbf : 0xfe: 0x921  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0xc0 : 0xfe: 0x922  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0xc1 : 0xff: 0x923  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xc2 : 0xff: 0x924  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xc3 : 0xfe: 0x925  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0xc4 : 0xfe: 0x926  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0xc5 : 0xfe: 0x927  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0xc6 : 0xff: 0x928  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xc7 : 0x81: 0x929  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + TML_ముసుగు ,
/*0xc8 : 0xff: 0x92a  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xc9 : 0xfe: 0x92b  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0xca : 0xfe: 0x92c  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0xcb : 0xfe: 0x92d  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0xcc : 0xfe: 0x92e  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xcd : 0xff: 0x92f  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xcf : 0xff: 0x930  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xd0 : 0x87: 0x931  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + MLM_ముసుగు + TML_ముసుగు ,
/*0xd1 : 0xff: 0x932  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xd2 : 0xb7: 0x933  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + ZERO     + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xd3 : 0x83: 0x934  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + MLM_ముసుగు + TML_ముసుగు ,
/*0xd4 : 0xff: 0x935  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + ZERO     + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xd5 : 0xfe: 0x936  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0xd6 : 0xbf: 0x937  */ DEV_ముసుగు + ZERO     + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xd7 : 0xff: 0x938  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xd8 : 0xff: 0x939  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0x00 : 0x00: 0x93A  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x93B  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe9 : 0xda: 0x93c  */ DEV_ముసుగు + PNJ_ముసుగు + ZERO     + ORI_ముసుగు + BNG_ముసుగు + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x93d  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xda : 0xff: 0x93e  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xdb : 0xff: 0x93f  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xdc : 0xff: 0x940  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xdd : 0xff: 0x941  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xde : 0xff: 0x942  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xdf : 0xbe: 0x943  */ DEV_ముసుగు + ZERO     + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0x00 : 0x00: 0x944  */ DEV_ముసుగు + ZERO     + GJR_ముసుగు + ZERO     + BNG_ముసుగు + KND_ముసుగు + ZERO     + ZERO     ,
/*0xe3 : 0x80: 0x945  */ DEV_ముసుగు + ZERO     + GJR_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe0 : 0x87: 0x946  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xe1 : 0xff: 0x947  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xe2 : 0xff: 0x948  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xe7 : 0x80: 0x949  */ DEV_ముసుగు + ZERO     + GJR_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe4 : 0x87: 0x94a  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xe5 : 0xff: 0x94b  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xe6 : 0xff: 0x94c  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xe8 : 0xff: 0x94d  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xec : 0x00: 0x94e  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xed : 0x00: 0x94f  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x950  */ DEV_ముసుగు + ZERO     + GJR_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x951  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x952  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x953  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x954  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x955  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + KND_ముసుగు + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x956  */ ZERO     + ZERO     + ZERO     + ORI_ముసుగు + ZERO     + KND_ముసుగు + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x957  */ ZERO     + ZERO     + ZERO     + ORI_ముసుగు + BNG_ముసుగు + ZERO     + MLM_ముసుగు + ZERO     ,
/*0x00 : 0x00: 0x958  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x959  */ DEV_ముసుగు + PNJ_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95a  */ DEV_ముసుగు + PNJ_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95b  */ DEV_ముసుగు + PNJ_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95c  */ DEV_ముసుగు + PNJ_ముసుగు + ZERO     + ZERO     + BNG_ముసుగు + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95d  */ DEV_ముసుగు + ZERO     + ZERO     + ORI_ముసుగు + BNG_ముసుగు + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95e  */ DEV_ముసుగు + PNJ_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xce : 0x98: 0x95f  */ DEV_ముసుగు + ZERO     + ZERO     + ORI_ముసుగు + BNG_ముసుగు + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x960  */ DEV_ముసుగు + ZERO     + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0x00 : 0x00: 0x961  */ DEV_ముసుగు + ZERO     + ZERO     + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + ZERO     ,
/*0x00 : 0x00: 0x962  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + BNG_ముసుగు + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x963  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + BNG_ముసుగు + ZERO     + ZERO     + ZERO     ,
/*0xea : 0xf8: 0x964  */ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xeaea : 0x00: 0x965*/ DEV_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xf1 : 0xff: 0x966  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xf2 : 0xff: 0x967  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xf3 : 0xff: 0x968  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xf4 : 0xff: 0x969  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xf5 : 0xff: 0x96a  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xf6 : 0xff: 0x96b  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xf7 : 0xff: 0x96c  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xf8 : 0xff: 0x96d  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xf9 : 0xff: 0x96e  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0xfa : 0xff: 0x96f  */ DEV_ముసుగు + PNJ_ముసుగు + GJR_ముసుగు + ORI_ముసుగు + BNG_ముసుగు + KND_ముసుగు + MLM_ముసుగు + TML_ముసుగు ,
/*0x00 : 0x80: 0x970  */ DEV_ముసుగు + PNJ_ముసుగు + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*
 * The length of the array is 128 to provide values for 0x900..0x97f.
 * The last 15 entries for 0x971..0x97f of the validity table are all zero
 * because no Indic script uses such Unicode code points.
 */
/*0x00 : 0x00: 0x9yz  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO
};

స్థిర స్థిరరాశి uint16_t from_Unicode_పట్టిక[128]={
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
స్థిర స్థిరరాశి uint16_t గమ్యం_Unicode_పట్టిక[256]={
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

స్థిర స్థిరరాశి uint16_t vowelSignESpecialCases[][2]={
	{ 2 /*length of array*/    , 0      },
	{ 0xA4 , 0x0904 },
};

స్థిర స్థిరరాశి uint16_t nuktaSpecialCases[][2]={
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


#నిర్వచించు WRITE_గమ్యం_లక్ష్యం_FROM_U(వాదనలు_2,offsets,ఆధారం,లక్ష్యం,లక్ష్యం_హద్దు,లక్ష్యం_Byte_Unit,err) UPRV_ఖండం_MACRO_BEGIN { \
    int32_t స్థానభ్రంశం = (int32_t)(ఆధారం - వాదనలు_2->ఆధారం-1);                                        \
      /* write the targetUniChar  to target */                                                  \
    ఒకవేళ(లక్ష్యం < లక్ష్యం_హద్దు){                                                                   \
        ఒకవేళ(లక్ష్యం_Byte_Unit <= 0xFF){                                                             \
            *(లక్ష్యం)++ = (uint8_t)(లక్ష్యం_Byte_Unit);                                            \
            ఒకవేళ(offsets){                                                                        \
                *(offsets++) = స్థానభ్రంశం;                                                          \
            }                                                                                   \
        }లేకపోతే{                                                                                  \
            ఒకవేళ (లక్ష్యం_Byte_Unit > 0xFFFF) {                                                      \
                *(లక్ష్యం)++ = (uint8_t)(లక్ష్యం_Byte_Unit>>16);                                    \
                ఒకవేళ (offsets) {                                                                  \
                    --స్థానభ్రంశం;                                                                   \
                    *(offsets++) = స్థానభ్రంశం;                                                      \
                }                                                                               \
            }                                                                                   \
            ఒకవేళ (!(లక్ష్యం < లక్ష్యం_హద్దు)) {                                                      \
                వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] =    \
                                (uint8_t)(లక్ష్యం_Byte_Unit >> 8);                                 \
                వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] =    \
                                (uint8_t)లక్ష్యం_Byte_Unit;                                        \
                *err = U_బఫరు_అధికప్రవాహం_ERROR;                                                 \
            } లేకపోతే {                                                                            \
                *(లక్ష్యం)++ = (uint8_t)(లక్ష్యం_Byte_Unit>>8);                                     \
                ఒకవేళ(offsets){                                                                    \
                    *(offsets++) = స్థానభ్రంశం;                                                      \
                }                                                                               \
                ఒకవేళ(లక్ష్యం < లక్ష్యం_హద్దు){                                                       \
                    *(లక్ష్యం)++ = (uint8_t)  లక్ష్యం_Byte_Unit;                                    \
                    ఒకవేళ(offsets){                                                                \
                        *(offsets++) = స్థానభ్రంశం                            ;                      \
                    }                                                                           \
                }లేకపోతే{                                                                          \
                    వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] =\
                                (uint8_t) (లక్ష్యం_Byte_Unit);                                     \
                    *err = U_బఫరు_అధికప్రవాహం_ERROR;                                             \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
    }లేకపోతే{                                                                                      \
        ఒకవేళ (లక్ష్యం_Byte_Unit & 0xFF0000) {                                                        \
            వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] =        \
                        (uint8_t) (లక్ష్యం_Byte_Unit >>16);                                        \
        }                                                                                       \
        ఒకవేళ(లక్ష్యం_Byte_Unit & 0xFF00){                                                            \
            వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] =        \
                        (uint8_t) (లక్ష్యం_Byte_Unit >>8);                                         \
        }                                                                                       \
        వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] =            \
                        (uint8_t) (లక్ష్యం_Byte_Unit);                                             \
        *err = U_బఫరు_అధికప్రవాహం_ERROR;                                                         \
    }                                                                                           \
} UPRV_ఖండం_MACRO_చివర

/* Rules:
 *    Explicit Halant :
 *                      <HALANT> + <ZWNJ>
 *    Soft Halant :
 *                      <HALANT> + <ZWJ>
 */
స్థిర శూన్య U_CALLCONV
UConverter_fromUnicode_ISCII_OFFSETS_LOGIC(
        UConverter_From_Unicode_వాదనలు * వాదనలు_2, UError_సంకేతం * err) {
    స్థిరరాశి char16_t *ఆధారం = వాదనలు_2->ఆధారం;
    స్థిరరాశి char16_t *ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    గుర్తులేని అక్షరం *లక్ష్యం = (గుర్తులేని అక్షరం *) వాదనలు_2->లక్ష్యం;
    గుర్తులేని అక్షరం *లక్ష్యం_హద్దు = (గుర్తులేని అక్షరం *) వాదనలు_2->లక్ష్యం_హద్దు;
    int32_t* offsets = వాదనలు_2->offsets;
    uint32_t లక్ష్యం_Byte_Unit = 0x0000;
    UChar32 ఆధారం_Char = 0x0000;
    UChar32 తాత్కాలిక_Context_From_Unicode = 0x0000;    /* For special handling of the Gurmukhi script. */
    UConverter_దత్తాంశం_ISCII *converter_దత్తాంశం;
    uint16_t newDelta=0;
    uint16_t range = 0;
    UBool delta_మారింది = అబద్ధం;

    ఒకవేళ ((వాదనలు_2->converter == శూన్యసూచిక) || (వాదనలు_2->లక్ష్యం_హద్దు < వాదనలు_2->లక్ష్యం) || (వాదనలు_2->ఆధారం_హద్దు < వాదనలు_2->ఆధారం)) {
        *err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    /* initialize data */
    converter_దత్తాంశం=(UConverter_దత్తాంశం_ISCII*)వాదనలు_2->converter->extraInfo;
    newDelta=converter_దత్తాంశం->currentDeltaFromUnicode;
    range = (uint16_t)(newDelta/DELTA);

    ఒకవేళ ((ఆధారం_Char = వాదనలు_2->converter->fromUChar32)!=0) {
        వెళ్ళు getTrail;
    }

    /*writing the char to the output stream */
    అయితే (ఆధారం < ఆధారం_హద్దు) {
        /* Write the language code following LF only if LF is not the last character. */
        ఒకవేళ (వాదనలు_2->converter->from_Unicode_స్థితి == LF) {
            లక్ష్యం_Byte_Unit = ATR<<8;
            లక్ష్యం_Byte_Unit += (uint8_t) lookup_Initial_దత్తాంశం[range].isciiLang;
            వాదనలు_2->converter->from_Unicode_స్థితి = 0x0000;
            /* now append ATR and language code */
            WRITE_గమ్యం_లక్ష్యం_FROM_U(వాదనలు_2,offsets,ఆధారం,లక్ష్యం,లక్ష్యం_హద్దు,లక్ష్యం_Byte_Unit,err);
            ఒకవేళ (U_FAILURE(*err)) {
                ఆపు;
            }
        }
        
        ఆధారం_Char = *ఆధారం++;
        తాత్కాలిక_Context_From_Unicode = converter_దత్తాంశం->contextCharFromUnicode;
        
        లక్ష్యం_Byte_Unit = missingCharMarker;
        
        /*check if input is in ASCII and C0 control codes range*/
        ఒకవేళ (ఆధారం_Char <= ASCII_చివర) {
            వాదనలు_2->converter->from_Unicode_స్థితి = ఆధారం_Char;
            WRITE_గమ్యం_లక్ష్యం_FROM_U(వాదనలు_2,offsets,ఆధారం,లక్ష్యం,లక్ష్యం_హద్దు,ఆధారం_Char,err);
            ఒకవేళ (U_FAILURE(*err)) {
                ఆపు;
            }
            కొనసాగు;
        }
        ఎంపిక (ఆధారం_Char) {
        స్థితి ZWNJ:
            /* contextChar has HALANT */
            ఒకవేళ (converter_దత్తాంశం->contextCharFromUnicode) {
                converter_దత్తాంశం->contextCharFromUnicode = 0x00;
                లక్ష్యం_Byte_Unit = ISCII_HALANT;
            } లేకపోతే {
                /* consume ZWNJ and continue */
                converter_దత్తాంశం->contextCharFromUnicode = 0x00;
                కొనసాగు;
            }
            ఆపు;
        స్థితి ZWJ:
            /* contextChar has HALANT */
            ఒకవేళ (converter_దత్తాంశం->contextCharFromUnicode) {
                లక్ష్యం_Byte_Unit = ISCII_NUKTA;
            } లేకపోతే {
                లక్ష్యం_Byte_Unit =ISCII_INV;
            }
            converter_దత్తాంశం->contextCharFromUnicode = 0x00;
            ఆపు;
        అప్రమేయ:
            /* is the sourceChar in the INDIC_RANGE? */
            ఒకవేళ ((uint16_t)(INDIC_ఖండం_చివర-ఆధారం_Char) <= INDIC_RANGE) {
                /* Danda and Double Danda are valid in Northern scripts.. since Unicode
                 * does not include these codepoints in all Northern scrips we need to
                 * filter them out
                 */
                ఒకవేళ (ఆధారం_Char!= DANDA && ఆధారం_Char != DOUBLE_DANDA) {
                    /* find out to which block the souceChar belongs*/
                    range =(uint16_t)((ఆధారం_Char-INDIC_ఖండం_BEGIN)/DELTA);
                    newDelta =(uint16_t)(range*DELTA);

                    /* Now are we in the same block as the previous? */
                    ఒకవేళ (newDelta!= converter_దత్తాంశం->currentDeltaFromUnicode || converter_దత్తాంశం->is_మొదటి_బఫరు) {
                        converter_దత్తాంశం->currentDeltaFromUnicode = newDelta;
                        converter_దత్తాంశం->current_ముసుగు_From_Unicode = lookup_Initial_దత్తాంశం[range].ముసుగు_Enum_2;
                        delta_మారింది =నిజం;
                        converter_దత్తాంశం->is_మొదటి_బఫరు=అబద్ధం;
                    }
                    
                    ఒకవేళ (converter_దత్తాంశం->currentDeltaFromUnicode == PNJ_DELTA) { 
                        ఒకవేళ (ఆధారం_Char == PNJ_TIPPI) {
                            /* Make sure Tippi is converted to Bindi. */
                            ఆధారం_Char = PNJ_BINDI;
                        } లేకపోతే ఒకవేళ (ఆధారం_Char == PNJ_ADHAK) {
                            /* This is for consonant cluster handling. */
                            converter_దత్తాంశం->contextCharFromUnicode = PNJ_ADHAK;
                        }
                        
                    }
                    /* Normalize all Indic codepoints to Devanagari and map them to ISCII */
                    /* now subtract the new delta from sourceChar*/
                    ఆధారం_Char -= converter_దత్తాంశం->currentDeltaFromUnicode;
                }

                /* get the target byte unit */
                లక్ష్యం_Byte_Unit=from_Unicode_పట్టిక[(uint8_t)ఆధారం_Char];

                /* is the code point valid in current script? */
                ఒకవేళ ((validity_పట్టిక[(uint8_t)ఆధారం_Char] & converter_దత్తాంశం->current_ముసుగు_From_Unicode)==0) {
                    /* Vocallic RR is assigned in ISCII Telugu and Unicode */
                    ఒకవేళ (converter_దత్తాంశం->currentDeltaFromUnicode!=(TELUGU_DELTA) || ఆధారం_Char!=VOCALLIC_RR) {
                        లక్ష్యం_Byte_Unit=missingCharMarker;
                    }
                }

                ఒకవేళ (delta_మారింది) {
                    /* we are in a script block which is different than
                     * previous sourceChar's script block write ATR and language codes
                     */
                    uint32_t తాత్కాలిక=0;
                    తాత్కాలిక =(uint16_t)(ATR<<8);
                    తాత్కాలిక += (uint16_t)((uint8_t) lookup_Initial_దత్తాంశం[range].isciiLang);
                    /* reset */
                    delta_మారింది=అబద్ధం;
                    /* now append ATR and language code */
                    WRITE_గమ్యం_లక్ష్యం_FROM_U(వాదనలు_2,offsets,ఆధారం,లక్ష్యం,లక్ష్యం_హద్దు,తాత్కాలిక,err);
                    ఒకవేళ (U_FAILURE(*err)) {
                        ఆపు;
                    }
                }
                
                ఒకవేళ (converter_దత్తాంశం->currentDeltaFromUnicode == PNJ_DELTA && (ఆధారం_Char + PNJ_DELTA) == PNJ_ADHAK) {
                    కొనసాగు;
                }
            }
            /* reset context char */
            converter_దత్తాంశం->contextCharFromUnicode = 0x00;
            ఆపు;
        }
        ఒకవేళ (converter_దత్తాంశం->currentDeltaFromUnicode == PNJ_DELTA && తాత్కాలిక_Context_From_Unicode == PNJ_ADHAK && isPNJConsonant((ఆధారం_Char + PNJ_DELTA))) {
            /* If the previous codepoint is Adhak and the current codepoint is a consonant, the targetByteUnit should be C + Halant + C. */
            /* reset context char */
            converter_దత్తాంశం->contextCharFromUnicode = 0x0000;
            లక్ష్యం_Byte_Unit = లక్ష్యం_Byte_Unit << 16 | ISCII_HALANT << 8 | లక్ష్యం_Byte_Unit;
            /* write targetByteUnit to target */
            WRITE_గమ్యం_లక్ష్యం_FROM_U(వాదనలు_2, offsets, ఆధారం, లక్ష్యం, లక్ష్యం_హద్దు, లక్ష్యం_Byte_Unit,err);
            ఒకవేళ (U_FAILURE(*err)) {
                ఆపు;
            }
        } లేకపోతే ఒకవేళ (లక్ష్యం_Byte_Unit != missingCharMarker) {
            ఒకవేళ (లక్ష్యం_Byte_Unit==ISCII_HALANT) {
                converter_దత్తాంశం->contextCharFromUnicode = (char16_t)లక్ష్యం_Byte_Unit;
            }
            /* write targetByteUnit to target*/
            WRITE_గమ్యం_లక్ష్యం_FROM_U(వాదనలు_2,offsets,ఆధారం,లక్ష్యం,లక్ష్యం_హద్దు,లక్ష్యం_Byte_Unit,err);
            ఒకవేళ (U_FAILURE(*err)) {
                ఆపు;
            }
        } లేకపోతే {
            /* oops.. the code point is unassigned */
            /*check if the char is a First surrogate*/
            ఒకవేళ (U16_IS_SURROGATE(ఆధారం_Char)) {
                ఒకవేళ (U16_IS_SURROGATE_LEAD(ఆధారం_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    ఒకవేళ (ఆధారం < ఆధారం_హద్దు) {
                        /* test the following code unit */
                        char16_t trail= (*ఆధారం);
                        ఒకవేళ (U16_IS_TRAIL(trail)) {
                            ఆధారం++;
                            ఆధారం_Char=U16_GET_SUPPLEMENTARY(ఆధారం_Char, trail);
                            *err =U_INVALID_CHAR_దొరికింది;
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } లేకపోతే {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_దొరికింది;
                        }
                    } లేకపోతే {
                        /* no more input */
                        *err = U_ZERO_ERROR;
                    }
                } లేకపోతే {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_దొరికింది;
                }
            } లేకపోతే {
                /* callback(unassigned) for a BMP code point */
                *err = U_INVALID_CHAR_దొరికింది;
            }

            వాదనలు_2->converter->fromUChar32=ఆధారం_Char;
            ఆపు;
        }
    }/* end while(mySourceIndex<mySourceLength) */

    /*save the state and return */
    వాదనలు_2->ఆధారం = ఆధారం;
    వాదనలు_2->లక్ష్యం = (అక్షరం*)లక్ష్యం;
}

స్థిర స్థిరరాశి uint16_t lookup_పట్టిక[][2]={
    { ZERO,       ZERO     },     /*DEFAULT*/
    { ZERO,       ZERO     },     /*ROMAN*/
    { DEVANAGARI, DEV_ముసుగు },
    { BENGALI,    BNG_ముసుగు },
    { TAMIL,      TML_ముసుగు },
    { TELUGU,     KND_ముసుగు },
    { BENGALI,    BNG_ముసుగు },
    { ORIYA,      ORI_ముసుగు },
    { KANNADA,    KND_ముసుగు },
    { MALAYALAM,  MLM_ముసుగు },
    { GUJARATI,   GJR_ముసుగు },
    { GURMUKHI,   PNJ_ముసుగు }
};

#నిర్వచించు WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2,ఆధారం,లక్ష్యం,offsets,స్థానభ్రంశం,లక్ష్యం_Uni_Char,delta, err) UPRV_ఖండం_MACRO_BEGIN { \
    /* add offset to current Indic Block */                                              \
    ఒకవేళ(లక్ష్యం_Uni_Char>ASCII_చివర &&                                                        \
           లక్ష్యం_Uni_Char != ZWJ &&                                                       \
           లక్ష్యం_Uni_Char != ZWNJ &&                                                      \
           లక్ష్యం_Uni_Char != DANDA &&                                                     \
           లక్ష్యం_Uni_Char != DOUBLE_DANDA){                                               \
                                                                                         \
           లక్ష్యం_Uni_Char+=(uint16_t)(delta);                                             \
    }                                                                                    \
    /* now write the targetUniChar */                                                    \
    ఒకవేళ(లక్ష్యం<వాదనలు_2->లక్ష్యం_హద్దు){                                                        \
        *(లక్ష్యం)++ = (char16_t)లక్ష్యం_Uni_Char;                                              \
        ఒకవేళ(offsets){                                                                     \
            *(offsets)++ = (int32_t)(స్థానభ్రంశం);                                            \
        }                                                                                \
    }లేకపోతే{                                                                               \
        వాదనలు_2->converter->UChar_Error_బఫరు[వాదనలు_2->converter->UChar_Error_బఫరు_నిడివి++] =   \
            (char16_t)లక్ష్యం_Uni_Char;                                                        \
        *err = U_బఫరు_అధికప్రవాహం_ERROR;                                                  \
    }                                                                                    \
} UPRV_ఖండం_MACRO_చివర

#నిర్వచించు GET_MAPPING(ఆధారం_Char,లక్ష్యం_Uni_Char,దత్తాంశం) UPRV_ఖండం_MACRO_BEGIN {              \
    లక్ష్యం_Uni_Char = గమ్యం_Unicode_పట్టిక[(ఆధారం_Char)] ;                                       \
    /* is the code point valid in current script? */                                     \
    ఒకవేళ(ఆధారం_Char> ASCII_చివర &&                                                          \
            (validity_పట్టిక[(లక్ష్యం_Uni_Char & 0x7F)] & దత్తాంశం->current_ముసుగు_గమ్యం_Unicode)==0){    \
        /* Vocallic RR is assigned in ISCII Telugu and Unicode */                         \
        ఒకవేళ(దత్తాంశం->current_Delta_గమ్యం_Unicode!=(TELUGU_DELTA) ||                                \
                    లక్ష్యం_Uni_Char!=VOCALLIC_RR){                                         \
            లక్ష్యం_Uni_Char=missingCharMarker;                                             \
        }                                                                                \
    }                                                                                    \
} UPRV_ఖండం_MACRO_చివర

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

స్థిర శూన్య U_CALLCONV
UConverter_గమ్యం_Unicode_ISCII_OFFSETS_LOGIC(UConverter_గమ్యం_Unicode_వాదనలు *వాదనలు_2, UError_సంకేతం* err) {
    స్థిరరాశి అక్షరం *ఆధారం = ( అక్షరం *) వాదనలు_2->ఆధారం;
    char16_t *లక్ష్యం = వాదనలు_2->లక్ష్యం;
    స్థిరరాశి అక్షరం *ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    స్థిరరాశి char16_t* లక్ష్యం_హద్దు = వాదనలు_2->లక్ష్యం_హద్దు;
    uint32_t లక్ష్యం_Uni_Char = 0x0000;
    uint8_t ఆధారం_Char = 0x0000;
    UConverter_దత్తాంశం_ISCII* దత్తాంశం;
    UChar32* గమ్యం_Unicode_స్థితి=శూన్యసూచిక;
    UChar32 తాత్కాలిక_లక్ష్యం_Uni_Char = 0x0000;
    char16_t* context_Char_గమ్యం_Unicode= శూన్యసూచిక;
    UBool దొరికింది;
    పూర్ణాంకం i; 
    పూర్ణాంకం స్థానభ్రంశం = 0;

    ఒకవేళ ((వాదనలు_2->converter == శూన్యసూచిక) || (లక్ష్యం < వాదనలు_2->లక్ష్యం) || (ఆధారం < వాదనలు_2->ఆధారం)) {
        *err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    దత్తాంశం = (UConverter_దత్తాంశం_ISCII*)(వాదనలు_2->converter->extraInfo);
    context_Char_గమ్యం_Unicode = &దత్తాంశం->context_Char_గమ్యం_Unicode; /* contains previous ISCII codepoint visited */
    గమ్యం_Unicode_స్థితి = (UChar32*)&వాదనలు_2->converter->గమ్యం_Unicode_స్థితి;/* contains the mapping to Unicode of the above codepoint*/

    అయితే (U_SUCCESS(*err) && ఆధారం<ఆధారం_హద్దు) {

        లక్ష్యం_Uni_Char = missingCharMarker;

        ఒకవేళ (లక్ష్యం < లక్ష్యం_హద్దు) {
            ఆధారం_Char = (గుర్తులేని అక్షరం)*(ఆధారం)++;

            /* look at the post-context perform special processing */
            ఒకవేళ (*context_Char_గమ్యం_Unicode==ATR) {

                /* If we have ATR in *contextCharToUnicode then we need to change our
                 * state to the Indic Script specified by sourceChar
                 */

                /* check if the sourceChar is supported script range*/
                ఒకవేళ ((uint8_t)(PNJ-ఆధారం_Char)<=PNJ-DEV) {
                    దత్తాంశం->current_Delta_గమ్యం_Unicode = (uint16_t)(lookup_పట్టిక[ఆధారం_Char & 0x0F][0] * DELTA);
                    దత్తాంశం->current_ముసుగు_గమ్యం_Unicode = (ముసుగు_Enum)lookup_పట్టిక[ఆధారం_Char & 0x0F][1];
                } లేకపోతే ఒకవేళ (ఆధారం_Char==DEF) {
                    /* switch back to default */
                    దత్తాంశం->current_Delta_గమ్యం_Unicode = దత్తాంశం->def_Delta_గమ్యం_Unicode;
                    దత్తాంశం->current_ముసుగు_గమ్యం_Unicode = దత్తాంశం->def_ముసుగు_గమ్యం_Unicode;
                } లేకపోతే {
                    ఒకవేళ ((ఆధారం_Char >= 0x21 && ఆధారం_Char <= 0x3F)) {
                        /* these are display codes consume and continue */
                    } లేకపోతే {
                        *err =U_ILLEGAL_CHAR_దొరికింది;
                        /* reset */
                        *context_Char_గమ్యం_Unicode=NO_CHAR_MARKER;
                        వెళ్ళు CALLBACK;
                    }
                }

                /* reset */
                *context_Char_గమ్యం_Unicode=NO_CHAR_MARKER;

                కొనసాగు;

            } లేకపోతే ఒకవేళ (*context_Char_గమ్యం_Unicode==EXT) {
                /* check if sourceChar is in 0xA1-0xEE range */
                ఒకవేళ ((uint8_t) (EXT_RANGE_చివర - ఆధారం_Char) <= (EXT_RANGE_చివర - EXT_RANGE_BEGIN)) {
                    /* We currently support only Anudatta and Devanagari abbreviation sign */
                    ఒకవేళ (ఆధారం_Char==0xBF || ఆధారం_Char == 0xB8) {
                        లక్ష్యం_Uni_Char = (ఆధారం_Char==0xBF) ? DEV_ABBR_SIGN : DEV_ANUDATTA;
                        
                        /* find out if the mapping is valid in this state */
                        ఒకవేళ (validity_పట్టిక[(uint8_t)లక్ష్యం_Uni_Char] & దత్తాంశం->current_ముసుగు_గమ్యం_Unicode) {
                            *context_Char_గమ్యం_Unicode= NO_CHAR_MARKER;

                            /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                            ఒకవేళ (దత్తాంశం->prev_గమ్యం_Unicode_స్థితి) {
                                WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2,ఆధారం,లక్ష్యం,వాదనలు_2->offsets,(ఆధారం-వాదనలు_2->ఆధారం -1),దత్తాంశం->prev_గమ్యం_Unicode_స్థితి,0,err);
                                దత్తాంశం->prev_గమ్యం_Unicode_స్థితి = 0x0000;
                            }
                            /* write to target */
                            WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2,ఆధారం,లక్ష్యం,వాదనలు_2->offsets,(ఆధారం-వాదనలు_2->ఆధారం -2),లక్ష్యం_Uni_Char,దత్తాంశం->current_Delta_గమ్యం_Unicode,err);

                            కొనసాగు;
                        }
                    }
                    /* byte unit is unassigned */
                    లక్ష్యం_Uni_Char = missingCharMarker;
                    *err= U_INVALID_CHAR_దొరికింది;
                } లేకపోతే {
                    /* only 0xA1 - 0xEE are legal after EXT char */
                    *context_Char_గమ్యం_Unicode= NO_CHAR_MARKER;
                    *err = U_ILLEGAL_CHAR_దొరికింది;
                }
                వెళ్ళు CALLBACK;
            } లేకపోతే ఒకవేళ (*context_Char_గమ్యం_Unicode==ISCII_INV) {
                ఒకవేళ (ఆధారం_Char==ISCII_HALANT) {
                    లక్ష్యం_Uni_Char = 0x0020; /* replace with space according to Indic FAQ */
                } లేకపోతే {
                    లక్ష్యం_Uni_Char = ZWJ;
                }

                /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                ఒకవేళ (దత్తాంశం->prev_గమ్యం_Unicode_స్థితి) {
                    WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2,ఆధారం,లక్ష్యం,వాదనలు_2->offsets,(ఆధారం-వాదనలు_2->ఆధారం -1),దత్తాంశం->prev_గమ్యం_Unicode_స్థితి,0,err);
                    దత్తాంశం->prev_గమ్యం_Unicode_స్థితి = 0x0000;
                }
                /* write to target */
                WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2,ఆధారం,లక్ష్యం,వాదనలు_2->offsets,(ఆధారం-వాదనలు_2->ఆధారం -2),లక్ష్యం_Uni_Char,దత్తాంశం->current_Delta_గమ్యం_Unicode,err);
                /* reset */
                *context_Char_గమ్యం_Unicode=NO_CHAR_MARKER;
            }

            /* look at the pre-context and perform special processing */
            ఎంపిక (ఆధారం_Char) {
            స్థితి ISCII_INV:
            స్థితి EXT:
            స్థితి ATR:
                *context_Char_గమ్యం_Unicode = (char16_t)ఆధారం_Char;

                ఒకవేళ (*గమ్యం_Unicode_స్థితి != missingCharMarker) {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    ఒకవేళ (దత్తాంశం->prev_గమ్యం_Unicode_స్థితి) {
                        WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2,ఆధారం,లక్ష్యం,వాదనలు_2->offsets,(ఆధారం-వాదనలు_2->ఆధారం -1),దత్తాంశం->prev_గమ్యం_Unicode_స్థితి,0,err);
                        దత్తాంశం->prev_గమ్యం_Unicode_స్థితి = 0x0000;
                    }
                    WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2,ఆధారం,లక్ష్యం,వాదనలు_2->offsets,(ఆధారం-వాదనలు_2->ఆధారం -2),*గమ్యం_Unicode_స్థితి,దత్తాంశం->current_Delta_గమ్యం_Unicode,err);
                    *గమ్యం_Unicode_స్థితి = missingCharMarker;
                }
                కొనసాగు;
            స్థితి ISCII_DANDA:
                /* handle double danda*/
                ఒకవేళ (*context_Char_గమ్యం_Unicode== ISCII_DANDA) {
                    లక్ష్యం_Uni_Char = DOUBLE_DANDA;
                    /* clear the context */
                    *context_Char_గమ్యం_Unicode = NO_CHAR_MARKER;
                    *గమ్యం_Unicode_స్థితి = missingCharMarker;
                } లేకపోతే {
                    GET_MAPPING(ఆధారం_Char,లక్ష్యం_Uni_Char,దత్తాంశం);
                    *context_Char_గమ్యం_Unicode = ఆధారం_Char;
                }
                ఆపు;
            స్థితి ISCII_HALANT:
                /* handle explicit halant */
                ఒకవేళ (*context_Char_గమ్యం_Unicode == ISCII_HALANT) {
                    లక్ష్యం_Uni_Char = ZWNJ;
                    /* clear the context */
                    *context_Char_గమ్యం_Unicode = NO_CHAR_MARKER;
                } లేకపోతే {
                    GET_MAPPING(ఆధారం_Char,లక్ష్యం_Uni_Char,దత్తాంశం);
                    *context_Char_గమ్యం_Unicode = ఆధారం_Char;
                }
                ఆపు;
            స్థితి 0x0A:
            స్థితి 0x0D:
                దత్తాంశం->reset_గమ్యం_Default_గమ్యం_Unicode = నిజం;
                GET_MAPPING(ఆధారం_Char,లక్ష్యం_Uni_Char,దత్తాంశం)
                ;
                *context_Char_గమ్యం_Unicode = ఆధారం_Char;
                ఆపు;

            స్థితి ISCII_VOWEL_SIGN_E:
                i=1;
                దొరికింది=అబద్ధం;
                కొరకు (; i<vowelSignESpecialCases[0][0]; i++) {
                    U_ASSERT(i<UPRV_LENGTHOF(vowelSignESpecialCases));
                    ఒకవేళ (vowelSignESpecialCases[i][0]==(uint8_t)*context_Char_గమ్యం_Unicode) {
                        లక్ష్యం_Uni_Char=vowelSignESpecialCases[i][1];
                        దొరికింది=నిజం;
                        ఆపు;
                    }
                }
                ఒకవేళ (దొరికింది) {
                    /* find out if the mapping is valid in this state */
                    ఒకవేళ (validity_పట్టిక[(uint8_t)లక్ష్యం_Uni_Char] & దత్తాంశం->current_ముసుగు_గమ్యం_Unicode) {
                        /*targetUniChar += data->currentDeltaToUnicode ;*/
                        *context_Char_గమ్యం_Unicode= NO_CHAR_MARKER;
                        *గమ్యం_Unicode_స్థితి = missingCharMarker;
                        ఆపు;
                    }
                }
                GET_MAPPING(ఆధారం_Char,లక్ష్యం_Uni_Char,దత్తాంశం);
                *context_Char_గమ్యం_Unicode = ఆధారం_Char;
                ఆపు;

            స్థితి ISCII_NUKTA:
                /* handle soft halant */
                ఒకవేళ (*context_Char_గమ్యం_Unicode == ISCII_HALANT) {
                    లక్ష్యం_Uni_Char = ZWJ;
                    /* clear the context */
                    *context_Char_గమ్యం_Unicode = NO_CHAR_MARKER;
                    ఆపు;
                } లేకపోతే ఒకవేళ (దత్తాంశం->current_Delta_గమ్యం_Unicode == PNJ_DELTA && దత్తాంశం->context_Char_గమ్యం_Unicode == 0xc0) {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    ఒకవేళ (దత్తాంశం->prev_గమ్యం_Unicode_స్థితి) {
                        WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2,ఆధారం,లక్ష్యం,వాదనలు_2->offsets,(ఆధారం-వాదనలు_2->ఆధారం -1),దత్తాంశం->prev_గమ్యం_Unicode_స్థితి,0,err);
                        దత్తాంశం->prev_గమ్యం_Unicode_స్థితి = 0x0000;
                    }
                    /* We got here because ISCII_NUKTA was preceded by 0xc0 and we are converting Gurmukhi.
                     * In that case we must convert (0xc0 0xe9) to (\u0a5c\u0a4d\u0a39).
                     */
                    లక్ష్యం_Uni_Char = PNJ_RRA;
                    WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2, ఆధారం, లక్ష్యం, వాదనలు_2->offsets, (ఆధారం-వాదనలు_2->ఆధారం)-2, లక్ష్యం_Uni_Char, 0, err);
                    ఒకవేళ (U_SUCCESS(*err)) {
                        లక్ష్యం_Uni_Char = PNJ_SIGN_VIRAMA;
                        WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2, ఆధారం, లక్ష్యం, వాదనలు_2->offsets, (ఆధారం-వాదనలు_2->ఆధారం)-2, లక్ష్యం_Uni_Char, 0, err);
                        ఒకవేళ (U_SUCCESS(*err)) {
                            లక్ష్యం_Uni_Char = PNJ_HA;
                            WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2, ఆధారం, లక్ష్యం, వాదనలు_2->offsets, (ఆధారం-వాదనలు_2->ఆధారం)-2, లక్ష్యం_Uni_Char, 0, err);
                        } లేకపోతే {
                            వాదనలు_2->converter->UChar_Error_బఫరు[వాదనలు_2->converter->UChar_Error_బఫరు_నిడివి++]= PNJ_HA;
                        }
                    } లేకపోతే {
                        వాదనలు_2->converter->UChar_Error_బఫరు[వాదనలు_2->converter->UChar_Error_బఫరు_నిడివి++]= PNJ_SIGN_VIRAMA;
                        వాదనలు_2->converter->UChar_Error_బఫరు[వాదనలు_2->converter->UChar_Error_బఫరు_నిడివి++]= PNJ_HA;
                    }
                    *గమ్యం_Unicode_స్థితి = missingCharMarker;
                    దత్తాంశం->context_Char_గమ్యం_Unicode = NO_CHAR_MARKER;
                    కొనసాగు;
                } లేకపోతే {
                    /* try to handle <CHAR> + ISCII_NUKTA special mappings */
                    i=1;
                    దొరికింది =అబద్ధం;
                    కొరకు (; i<nuktaSpecialCases[0][0]; i++) {
                        ఒకవేళ (nuktaSpecialCases[i][0]==(uint8_t)
                                *context_Char_గమ్యం_Unicode) {
                            లక్ష్యం_Uni_Char=nuktaSpecialCases[i][1];
                            దొరికింది =నిజం;
                            ఆపు;
                        }
                    }
                    ఒకవేళ (దొరికింది) {
                        /* find out if the mapping is valid in this state */
                        ఒకవేళ (validity_పట్టిక[(uint8_t)లక్ష్యం_Uni_Char] & దత్తాంశం->current_ముసుగు_గమ్యం_Unicode) {
                            /*targetUniChar += data->currentDeltaToUnicode ;*/
                            *context_Char_గమ్యం_Unicode= NO_CHAR_MARKER;
                            *గమ్యం_Unicode_స్థితి = missingCharMarker;
                            ఒకవేళ (దత్తాంశం->current_Delta_గమ్యం_Unicode == PNJ_DELTA) {
                                /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                                ఒకవేళ (దత్తాంశం->prev_గమ్యం_Unicode_స్థితి) {
                                    WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2,ఆధారం,లక్ష్యం,వాదనలు_2->offsets,(ఆధారం-వాదనలు_2->ఆధారం -1),దత్తాంశం->prev_గమ్యం_Unicode_స్థితి,0,err);
                                    దత్తాంశం->prev_గమ్యం_Unicode_స్థితి = 0x0000;
                                }
                                WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2,ఆధారం,లక్ష్యం,వాదనలు_2->offsets,(ఆధారం-వాదనలు_2->ఆధారం -2),లక్ష్యం_Uni_Char,దత్తాంశం->current_Delta_గమ్యం_Unicode,err);
                                కొనసాగు;
                            }
                            ఆపు;
                        }
                        /* else fall through to default */
                    }
                    /* else fall through to default */
                    U_FALLTHROUGH;
                }
            అప్రమేయ:GET_MAPPING(ఆధారం_Char,లక్ష్యం_Uni_Char,దత్తాంశం)
                ;
                *context_Char_గమ్యం_Unicode = ఆధారం_Char;
                ఆపు;
            }

            ఒకవేళ (*గమ్యం_Unicode_స్థితి != missingCharMarker) {
                /* Check to make sure that consonant clusters are handled correct for Gurmukhi script. */
                ఒకవేళ (దత్తాంశం->current_Delta_గమ్యం_Unicode == PNJ_DELTA && దత్తాంశం->prev_గమ్యం_Unicode_స్థితి != 0 && isPNJConsonant(దత్తాంశం->prev_గమ్యం_Unicode_స్థితి) &&
                        (*గమ్యం_Unicode_స్థితి + PNJ_DELTA) == PNJ_SIGN_VIRAMA && ((UChar32)(లక్ష్యం_Uni_Char + PNJ_DELTA) == దత్తాంశం->prev_గమ్యం_Unicode_స్థితి)) {
                    /* Consonant clusters C + HALANT + C should be encoded as ADHAK + C */
                    స్థానభ్రంశం = (పూర్ణాంకం)(ఆధారం-వాదనలు_2->ఆధారం - 3);
                    తాత్కాలిక_లక్ష్యం_Uni_Char = PNJ_ADHAK; /* This is necessary to avoid some compiler warnings. */
                    WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2,ఆధారం,లక్ష్యం,వాదనలు_2->offsets,స్థానభ్రంశం,తాత్కాలిక_లక్ష్యం_Uni_Char,0,err);
                    WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2,ఆధారం,లక్ష్యం,వాదనలు_2->offsets,స్థానభ్రంశం,దత్తాంశం->prev_గమ్యం_Unicode_స్థితి,0,err);
                    దత్తాంశం->prev_గమ్యం_Unicode_స్థితి = 0x0000; /* reset the previous unicode code point */
                    *గమ్యం_Unicode_స్థితి = missingCharMarker;
                    కొనసాగు;
                } లేకపోతే {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    ఒకవేళ (దత్తాంశం->prev_గమ్యం_Unicode_స్థితి) {
                        WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2,ఆధారం,లక్ష్యం,వాదనలు_2->offsets,(ఆధారం-వాదనలు_2->ఆధారం -1),దత్తాంశం->prev_గమ్యం_Unicode_స్థితి,0,err);
                        దత్తాంశం->prev_గమ్యం_Unicode_స్థితి = 0x0000;
                    }
                    /* Check to make sure that Bindi and Tippi are handled correctly for Gurmukhi script. 
                     * If 0xA2 is preceded by a codepoint in the PNJ_BINDI_TIPPI_SET then the target codepoint should be Tippi instead of Bindi.
                     */
                    ఒకవేళ (దత్తాంశం->current_Delta_గమ్యం_Unicode == PNJ_DELTA && (లక్ష్యం_Uni_Char + PNJ_DELTA) == PNJ_BINDI && isPNJBindiTippi((*గమ్యం_Unicode_స్థితి + PNJ_DELTA))) {
                        లక్ష్యం_Uni_Char = PNJ_TIPPI - PNJ_DELTA;
                        WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2,ఆధారం,లక్ష్యం,వాదనలు_2->offsets,(ఆధారం-వాదనలు_2->ఆధారం -2),*గమ్యం_Unicode_స్థితి,PNJ_DELTA,err);
                    } లేకపోతే ఒకవేళ (దత్తాంశం->current_Delta_గమ్యం_Unicode == PNJ_DELTA && (లక్ష్యం_Uni_Char + PNJ_DELTA) == PNJ_SIGN_VIRAMA && isPNJConsonant((*గమ్యం_Unicode_స్థితి + PNJ_DELTA))) {
                        /* Store the current toUnicodeStatus code point for later handling of consonant cluster in Gurmukhi. */
                        దత్తాంశం->prev_గమ్యం_Unicode_స్థితి = *గమ్యం_Unicode_స్థితి + PNJ_DELTA;
                    } లేకపోతే {
                        /* write the previously mapped codepoint */
                        WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2,ఆధారం,లక్ష్యం,వాదనలు_2->offsets,(ఆధారం-వాదనలు_2->ఆధారం -2),*గమ్యం_Unicode_స్థితి,దత్తాంశం->current_Delta_గమ్యం_Unicode,err);
                    }
                }
                *గమ్యం_Unicode_స్థితి = missingCharMarker;
            }

            ఒకవేళ (లక్ష్యం_Uni_Char != missingCharMarker) {
                /* now save the targetUniChar for delayed write */
                *గమ్యం_Unicode_స్థితి = (char16_t) లక్ష్యం_Uni_Char;
                ఒకవేళ (దత్తాంశం->reset_గమ్యం_Default_గమ్యం_Unicode) {
                    దత్తాంశం->current_Delta_గమ్యం_Unicode = దత్తాంశం->def_Delta_గమ్యం_Unicode;
                    దత్తాంశం->current_ముసుగు_గమ్యం_Unicode = దత్తాంశం->def_ముసుగు_గమ్యం_Unicode;
                    దత్తాంశం->reset_గమ్యం_Default_గమ్యం_Unicode=అబద్ధం;
                }
            } లేకపోతే {

                /* we reach here only if targetUniChar == missingCharMarker
                 * so assign codes to reason and err
                 */
                *err = U_INVALID_CHAR_దొరికింది;
CALLBACK:
                వాదనలు_2->converter->గమ్యం_UBytes[0] = ఆధారం_Char;
                వాదనలు_2->converter->గమ్యం_ULength = 1;
                ఆపు;
            }

        } లేకపోతే {
            *err =U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }
    }

    ఒకవేళ (U_SUCCESS(*err) && వాదనలు_2->flush && ఆధారం == ఆధారం_హద్దు) {
        /* end of the input stream */
        UConverter *cnv = వాదనలు_2->converter;

        ఒకవేళ (*context_Char_గమ్యం_Unicode==ATR || *context_Char_గమ్యం_Unicode==EXT || *context_Char_గమ్యం_Unicode==ISCII_INV) {
            /* set toUBytes[] */
            cnv->గమ్యం_UBytes[0] = (uint8_t)*context_Char_గమ్యం_Unicode;
            cnv->గమ్యం_ULength = 1;

            /* avoid looping on truncated sequences */
            *context_Char_గమ్యం_Unicode = NO_CHAR_MARKER;
        } లేకపోతే {
            cnv->గమ్యం_ULength = 0;
        }

        ఒకవేళ (*గమ్యం_Unicode_స్థితి != missingCharMarker) {
            /* output a remaining target character */
            WRITE_గమ్యం_లక్ష్యం_గమ్యం_U(వాదనలు_2,ఆధారం,లక్ష్యం,వాదనలు_2->offsets,(ఆధారం - వాదనలు_2->ఆధారం -1),*గమ్యం_Unicode_స్థితి,దత్తాంశం->current_Delta_గమ్యం_Unicode,err);
            *గమ్యం_Unicode_స్థితి = missingCharMarker;
        }
    }

    వాదనలు_2->లక్ష్యం = లక్ష్యం;
    వాదనలు_2->ఆధారం = ఆధారం;
}

/* structure for SafeClone calculations */
నిర్మాణం cloneISCIIStruct {
    UConverter cnv;
    UConverter_దత్తాంశం_ISCII mydata;
};

స్థిర UConverter * U_CALLCONV
ISCII_సురక్షిత_Clone(స్థిరరాశి UConverter *cnv,
              శూన్య *దొంతర_బఫరు,
              int32_t *p_బఫరు_కొలత,
              UError_సంకేతం *స్థితి_2)
{
    నిర్మాణం cloneISCIIStruct * localClone;
    int32_t బఫరు_కొలత_Needed = పరిమాణం(నిర్మాణం cloneISCIIStruct);

    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ (*p_బఫరు_కొలత == 0) { /* 'preflighting' request - set needed size into *pBufferSize */
        *p_బఫరు_కొలత = బఫరు_కొలత_Needed;
        తిరిగి శూన్యసూచిక;
    }

    localClone = (నిర్మాణం cloneISCIIStruct *)దొంతర_బఫరు;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, పరిమాణం(UConverter_దత్తాంశం_ISCII));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = నిజం;

    తిరిగి &localClone->cnv;
}

స్థిర శూన్య U_CALLCONV
ISCIIGet_Unicode_సమితి(స్థిరరాశి UConverter *cnv,
                    స్థిరరాశి USetAdder *sa,
                    UConverter_Unicode_సమితి which,
                    UError_సంకేతం *p_Error_సంకేతం)
{
    (శూన్య)cnv;
    (శూన్య)which;
    (శూన్య)p_Error_సంకేతం;
    int32_t idx, script;
    uint8_t mask;

    /* Since all ISCII versions allow switching to other ISCII
    scripts, we add all roundtrippable characters to this set. */
    sa->కూడు_Range(sa->సమితి, 0, ASCII_చివర);
    కొరకు (script = DEVANAGARI; script <= MALAYALAM; script++) {
        mask = (uint8_t)(lookup_Initial_దత్తాంశం[script].ముసుగు_Enum_2);
        కొరకు (idx = 0; idx < DELTA; idx++) {
            /* added check for TELUGU character */
            ఒకవేళ ((validity_పట్టిక[idx] & mask) || (script==TELUGU && idx==0x31)) {
                sa->కూడు(sa->సమితి, idx + (script * DELTA) + INDIC_ఖండం_BEGIN);
            }
        }
    }
    sa->కూడు(sa->సమితి, DANDA);
    sa->కూడు(sa->సమితి, DOUBLE_DANDA);
    sa->కూడు(sa->సమితి, ZWNJ);
    sa->కూడు(sa->సమితి, ZWJ);
}
U_CDECL_చివర
స్థిర స్థిరరాశి UConverterImpl _ISCIIImpl={

    UCNV_ISCII,

    శూన్యసూచిక,
    శూన్యసూచిక,

    _ISCIIOpen,
    _ISCIIClose,
    _ISCIIReset,

    UConverter_గమ్యం_Unicode_ISCII_OFFSETS_LOGIC,
    UConverter_గమ్యం_Unicode_ISCII_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISCII_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISCII_OFFSETS_LOGIC,
    శూన్యసూచిక,

    శూన్యసూచిక,
    _ISCIIgetName,
    శూన్యసూచిక,
    ISCII_సురక్షిత_Clone,
    ISCIIGet_Unicode_సమితి,
    శూన్యసూచిక,
    శూన్యసూచిక
};

స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం ISCIIStatic_దత్తాంశం={
    పరిమాణం(UConverter_Static_దత్తాంశం),
        "ISCII",
         0,
         UCNV_IBM,
         UCNV_ISCII,
         1,
         4,
        { 0x1a, 0, 0, 0 },
        0x1,
        అబద్ధం,
        అబద్ధం,
        0x0,
        0x0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

స్థిరరాశి UConverter_Shared_దత్తాంశం _ISCIIData=
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&ISCIIStatic_దత్తాంశం, &_ISCIIImpl);

#అయితే_ముగింపు /* #if !UCONFIG_NO_LEGACY_CONVERSION */
