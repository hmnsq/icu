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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/ucnv_cb.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "uassert.h"

#परिभाषय UCNV_OPTIONS_संस्करणम्_आवरणम् 0xf
#परिभाषय NUKTA               0x093c
#परिभाषय HALANT              0x094d
#परिभाषय ZWNJ                0x200c /* Zero Width Non Joiner */
#परिभाषय ZWJ                 0x200d /* Zero width Joiner */
#परिभाषय INVALID_CHAR        0xffff
#परिभाषय ATR                 0xEF   /* Attribute code */
#परिभाषय EXT                 0xF0   /* Extension code */
#परिभाषय DANDA               0x0964
#परिभाषय DOUBLE_DANDA        0x0965
#परिभाषय ISCII_NUKTA         0xE9
#परिभाषय ISCII_HALANT        0xE8
#परिभाषय ISCII_DANDA         0xEA
#परिभाषय ISCII_INV           0xD9
#परिभाषय ISCII_VOWEL_SIGN_E  0xE0
#परिभाषय INDIC_खण्डम्_BEGIN   0x0900
#परिभाषय INDIC_खण्डम्_अन्तः     0x0D7F
#परिभाषय INDIC_RANGE         (INDIC_खण्डम्_अन्तः - INDIC_खण्डम्_BEGIN)
#परिभाषय VOCALLIC_RR         0x0931
#परिभाषय LF                  0x0A
#परिभाषय ASCII_अन्तः           0xA0
#परिभाषय NO_CHAR_MARKER      0xFFFE
#परिभाषय TELUGU_DELTA        DELTA * TELUGU
#परिभाषय DEV_ABBR_SIGN       0x0970
#परिभाषय DEV_ANUDATTA        0x0952
#परिभाषय EXT_RANGE_BEGIN     0xA1
#परिभाषय EXT_RANGE_अन्तः       0xEE

#परिभाषय PNJ_DELTA           0x0100
#परिभाषय PNJ_BINDI           0x0A02
#परिभाषय PNJ_TIPPI           0x0A70
#परिभाषय PNJ_SIGN_VIRAMA     0x0A4D
#परिभाषय PNJ_ADHAK           0x0A71
#परिभाषय PNJ_HA              0x0A39
#परिभाषय PNJ_RRA             0x0A5C

प्रकारनिर्देश गणना {
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
प्रकारनिर्देश गणना {
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

प्रकारनिर्देश गणना {
    DEV_आवरणम् =0x80,
    PNJ_आवरणम् =0x40,
    GJR_आवरणम् =0x20,
    ORI_आवरणम् =0x10,
    BNG_आवरणम् =0x08,
    KND_आवरणम् =0x04,
    MLM_आवरणम् =0x02,
    TML_आवरणम् =0x01,
    ZERO =0x00
}आवरणम्_Enum;

#परिभाषय ISCII_CNV_उपसर्गः "ISCII,version="

प्रकारनिर्देश संरचना {
    char16_t context_Char_गन्तव्यम्_Unicode;         /* previous Unicode codepoint for contextual analysis */
    char16_t contextCharFromUnicode;       /* previous Unicode codepoint for contextual analysis */
    uint16_t def_Delta_गन्तव्यम्_Unicode;         /* delta for switching to default state when DEF is encountered  */
    uint16_t currentDeltaFromUnicode;   /* current delta in Indic block */
    uint16_t current_Delta_गन्तव्यम्_Unicode;     /* current delta in Indic block */
    आवरणम्_Enum current_आवरणम्_From_Unicode;    /* mask for current state in toUnicode */
    आवरणम्_Enum current_आवरणम्_गन्तव्यम्_Unicode;      /* mask for current state in toUnicode */
    आवरणम्_Enum def_आवरणम्_गन्तव्यम्_Unicode;          /* mask for default state in toUnicode */
    UBool is_First_बफर;                /* boolean for fromUnicode to see if we need to announce the first script */
    UBool reset_गन्तव्यम्_Default_गन्तव्यम्_Unicode;      /* boolean for resetting to default delta and mask when a newline is encountered*/
    वर्ण name[परिमाण(ISCII_CNV_उपसर्गः) + 1];
    UChar32 पूर्व_गन्तव्यम्_Unicode_स्थितिः;        /* Hold the previous toUnicodeStatus. This is necessary because we may need to know the last two code points. */
} UConverter_दत्त_ISCII;

प्रकारनिर्देश संरचना Lookup_दत्त_Struct {
    UniLang uniLang;
    आवरणम्_Enum आवरणम्_Enum_2;
    ISCIILang isciiLang;
} Lookup_दत्त_Struct;

स्थिर ध्रुव Lookup_दत्त_Struct lookup_Initial_दत्त[]={
    { DEVANAGARI, DEV_आवरणम्,  DEV },
    { BENGALI,    BNG_आवरणम्,  BNG },
    { GURMUKHI,   PNJ_आवरणम्,  PNJ },
    { GUJARATI,   GJR_आवरणम्,  GJR },
    { ORIYA,      ORI_आवरणम्,  ORI },
    { TAMIL,      TML_आवरणम्,  TML },
    { TELUGU,     KND_आवरणम्,  TLG },
    { KANNADA,    KND_आवरणम्,  KND },
    { MALAYALAM,  MLM_आवरणम्,  MLM }
};

/*
 * For special handling of certain Gurmukhi characters.
 * Bit 0 (value 1): PNJ consonant
 * Bit 1 (value 2): PNJ Bindi Tippi
 */
स्थिर ध्रुव uint8_t pnjMap[80] = {
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

स्थिर UBool
isPNJConsonant(UChar32 c) {
    यदि (c < 0xa00 || 0xa50 <= c) {
        प्रत्यागम असत्य;
    } अन्यथा {
        प्रत्यागम pnjMap[c - 0xa00] & 1;
    }
}

स्थिर UBool
isPNJBindiTippi(UChar32 c) {
    यदि (c < 0xa00 || 0xa50 <= c) {
        प्रत्यागम असत्य;
    } अन्यथा {
        प्रत्यागम pnjMap[c - 0xa00] >> 1;
    }
}
U_CDECL_BEGIN
स्थिर शून्य  U_CALLCONV
_ISCIIOpen(UConverter *cnv, UConverter_आरोपय_तर्काः *p_तर्काः, UError_संहिता *error_संहिता) {
    यदि(p_तर्काः->onlyTestIsLoadable) {
        प्रत्यागम;
    }

    cnv->extraInfo = uprv_malloc(परिमाण(UConverter_दत्त_ISCII));

    यदि (cnv->extraInfo != शून्यसूचकः) {
        int32_t len=0;
        UConverter_दत्त_ISCII *converter_दत्त=
                (UConverter_दत्त_ISCII *) cnv->extraInfo;
        converter_दत्त->context_Char_गन्तव्यम्_Unicode=NO_CHAR_MARKER;
        cnv->गन्तव्यम्_Unicode_स्थितिः = missingCharMarker;
        converter_दत्त->contextCharFromUnicode=0x0000;
        converter_दत्त->reset_गन्तव्यम्_Default_गन्तव्यम्_Unicode=असत्य;
        /* check if the version requested is supported */
        यदि ((p_तर्काः->options & UCNV_OPTIONS_संस्करणम्_आवरणम्) < 9) {
            /* initialize state variables */
            converter_दत्त->currentDeltaFromUnicode
                    = converter_दत्त->current_Delta_गन्तव्यम्_Unicode
                            = converter_दत्त->def_Delta_गन्तव्यम्_Unicode = (uint16_t)(lookup_Initial_दत्त[p_तर्काः->options & UCNV_OPTIONS_संस्करणम्_आवरणम्].uniLang * DELTA);

            converter_दत्त->current_आवरणम्_From_Unicode
                    = converter_दत्त->current_आवरणम्_गन्तव्यम्_Unicode
                            = converter_दत्त->def_आवरणम्_गन्तव्यम्_Unicode = lookup_Initial_दत्त[p_तर्काः->options & UCNV_OPTIONS_संस्करणम्_आवरणम्].आवरणम्_Enum_2;
            
            converter_दत्त->is_First_बफर=सत्य;
            (शून्य)uprv_strcpy(converter_दत्त->name, ISCII_CNV_उपसर्गः);
            len = (int32_t)uprv_strlen(converter_दत्त->name);
            converter_दत्त->name[len]= (वर्ण)((p_तर्काः->options & UCNV_OPTIONS_संस्करणम्_आवरणम्) + '0');
            converter_दत्त->name[len+1]=0;
            
            converter_दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः = 0x0000;
        } अन्यथा {
            uprv_free(cnv->extraInfo);
            cnv->extraInfo = शून्यसूचकः;
            *error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        }

    } अन्यथा {
        *error_संहिता =U_स्मृतिः_आवण्टनम्_ERROR;
    }
}

स्थिर शून्य U_CALLCONV
_ISCIIClose(UConverter *cnv) {
    यदि (cnv->extraInfo!=शून्यसूचकः) {
        यदि (!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo=शून्यसूचकः;
    }
}

स्थिर ध्रुव वर्ण*  U_CALLCONV
_ISCIIgetName(ध्रुव UConverter* cnv) {
    यदि (cnv->extraInfo) {
        UConverter_दत्त_ISCII* my_दत्त= (UConverter_दत्त_ISCII*)cnv->extraInfo;
        प्रत्यागम my_दत्त->name;
    }
    प्रत्यागम शून्यसूचकः;
}

स्थिर शून्य U_CALLCONV
_ISCIIReset(UConverter *cnv, UConverterResetChoice choice) {
    UConverter_दत्त_ISCII* दत्त =(UConverter_दत्त_ISCII *) (cnv->extraInfo);
    यदि (choice<=UCNV_RESET_गन्तव्यम्_UNICODE) {
        cnv->गन्तव्यम्_Unicode_स्थितिः = missingCharMarker;
        cnv->बहुलकम्=0;
        दत्त->current_Delta_गन्तव्यम्_Unicode=दत्त->def_Delta_गन्तव्यम्_Unicode;
        दत्त->current_आवरणम्_गन्तव्यम्_Unicode = दत्त->def_आवरणम्_गन्तव्यम्_Unicode;
        दत्त->context_Char_गन्तव्यम्_Unicode=NO_CHAR_MARKER;
        दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः = 0x0000;
    }
    यदि (choice!=UCNV_RESET_गन्तव्यम्_UNICODE) {
        cnv->fromUChar32=0x0000;
        दत्त->contextCharFromUnicode=0x00;
        दत्त->current_आवरणम्_From_Unicode=दत्त->def_आवरणम्_गन्तव्यम्_Unicode;
        दत्त->currentDeltaFromUnicode=दत्त->def_Delta_गन्तव्यम्_Unicode;
        दत्त->is_First_बफर=सत्य;
        दत्त->reset_गन्तव्यम्_Default_गन्तव्यम्_Unicode=असत्य;
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

स्थिर ध्रुव uint8_t validity_सारणी[128] = {
/* This state table is tool generated please do not edit unless you know exactly what you are doing */
/* Note: This table was edited to mirror the Windows XP implementation */
/*ISCII:Valid:Unicode */
/*0xa0 : 0x00: 0x900  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xa1 : 0xb8: 0x901  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + ZERO     + ZERO     + ZERO     ,
/*0xa2 : 0xfe: 0x902  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xa3 : 0xbf: 0x903  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0x00 : 0x00: 0x904  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xa4 : 0xff: 0x905  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xa5 : 0xff: 0x906  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xa6 : 0xff: 0x907  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xa7 : 0xff: 0x908  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xa8 : 0xff: 0x909  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xa9 : 0xff: 0x90a  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xaa : 0xfe: 0x90b  */ DEV_आवरणम् + ZERO     + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0x00 : 0x00: 0x90c  */ DEV_आवरणम् + ZERO     + ZERO     + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0xae : 0x80: 0x90d  */ DEV_आवरणम् + ZERO     + GJR_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xab : 0x87: 0x90e  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xac : 0xff: 0x90f  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xad : 0xff: 0x910  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xb2 : 0x80: 0x911  */ DEV_आवरणम् + ZERO     + GJR_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xaf : 0x87: 0x912  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xb0 : 0xff: 0x913  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xb1 : 0xff: 0x914  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xb3 : 0xff: 0x915  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xb4 : 0xfe: 0x916  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0xb5 : 0xfe: 0x917  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0xb6 : 0xfe: 0x918  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0xb7 : 0xff: 0x919  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xb8 : 0xff: 0x91a  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xb9 : 0xfe: 0x91b  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0xba : 0xff: 0x91c  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xbb : 0xfe: 0x91d  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0xbc : 0xff: 0x91e  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xbd : 0xff: 0x91f  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xbe : 0xfe: 0x920  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0xbf : 0xfe: 0x921  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0xc0 : 0xfe: 0x922  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0xc1 : 0xff: 0x923  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xc2 : 0xff: 0x924  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xc3 : 0xfe: 0x925  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0xc4 : 0xfe: 0x926  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0xc5 : 0xfe: 0x927  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0xc6 : 0xff: 0x928  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xc7 : 0x81: 0x929  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + TML_आवरणम् ,
/*0xc8 : 0xff: 0x92a  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xc9 : 0xfe: 0x92b  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0xca : 0xfe: 0x92c  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0xcb : 0xfe: 0x92d  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0xcc : 0xfe: 0x92e  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xcd : 0xff: 0x92f  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xcf : 0xff: 0x930  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xd0 : 0x87: 0x931  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + MLM_आवरणम् + TML_आवरणम् ,
/*0xd1 : 0xff: 0x932  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xd2 : 0xb7: 0x933  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + ZERO     + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xd3 : 0x83: 0x934  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + MLM_आवरणम् + TML_आवरणम् ,
/*0xd4 : 0xff: 0x935  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + ZERO     + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xd5 : 0xfe: 0x936  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0xd6 : 0xbf: 0x937  */ DEV_आवरणम् + ZERO     + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xd7 : 0xff: 0x938  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xd8 : 0xff: 0x939  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0x00 : 0x00: 0x93A  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x93B  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe9 : 0xda: 0x93c  */ DEV_आवरणम् + PNJ_आवरणम् + ZERO     + ORI_आवरणम् + BNG_आवरणम् + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x93d  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xda : 0xff: 0x93e  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xdb : 0xff: 0x93f  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xdc : 0xff: 0x940  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xdd : 0xff: 0x941  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xde : 0xff: 0x942  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xdf : 0xbe: 0x943  */ DEV_आवरणम् + ZERO     + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0x00 : 0x00: 0x944  */ DEV_आवरणम् + ZERO     + GJR_आवरणम् + ZERO     + BNG_आवरणम् + KND_आवरणम् + ZERO     + ZERO     ,
/*0xe3 : 0x80: 0x945  */ DEV_आवरणम् + ZERO     + GJR_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe0 : 0x87: 0x946  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xe1 : 0xff: 0x947  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xe2 : 0xff: 0x948  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xe7 : 0x80: 0x949  */ DEV_आवरणम् + ZERO     + GJR_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe4 : 0x87: 0x94a  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xe5 : 0xff: 0x94b  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xe6 : 0xff: 0x94c  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xe8 : 0xff: 0x94d  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xec : 0x00: 0x94e  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xed : 0x00: 0x94f  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x950  */ DEV_आवरणम् + ZERO     + GJR_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x951  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x952  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x953  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x954  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x955  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + KND_आवरणम् + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x956  */ ZERO     + ZERO     + ZERO     + ORI_आवरणम् + ZERO     + KND_आवरणम् + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x957  */ ZERO     + ZERO     + ZERO     + ORI_आवरणम् + BNG_आवरणम् + ZERO     + MLM_आवरणम् + ZERO     ,
/*0x00 : 0x00: 0x958  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x959  */ DEV_आवरणम् + PNJ_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95a  */ DEV_आवरणम् + PNJ_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95b  */ DEV_आवरणम् + PNJ_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95c  */ DEV_आवरणम् + PNJ_आवरणम् + ZERO     + ZERO     + BNG_आवरणम् + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95d  */ DEV_आवरणम् + ZERO     + ZERO     + ORI_आवरणम् + BNG_आवरणम् + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95e  */ DEV_आवरणम् + PNJ_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xce : 0x98: 0x95f  */ DEV_आवरणम् + ZERO     + ZERO     + ORI_आवरणम् + BNG_आवरणम् + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x960  */ DEV_आवरणम् + ZERO     + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0x00 : 0x00: 0x961  */ DEV_आवरणम् + ZERO     + ZERO     + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + ZERO     ,
/*0x00 : 0x00: 0x962  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + BNG_आवरणम् + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x963  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + BNG_आवरणम् + ZERO     + ZERO     + ZERO     ,
/*0xea : 0xf8: 0x964  */ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xeaea : 0x00: 0x965*/ DEV_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xf1 : 0xff: 0x966  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xf2 : 0xff: 0x967  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xf3 : 0xff: 0x968  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xf4 : 0xff: 0x969  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xf5 : 0xff: 0x96a  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xf6 : 0xff: 0x96b  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xf7 : 0xff: 0x96c  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xf8 : 0xff: 0x96d  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xf9 : 0xff: 0x96e  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0xfa : 0xff: 0x96f  */ DEV_आवरणम् + PNJ_आवरणम् + GJR_आवरणम् + ORI_आवरणम् + BNG_आवरणम् + KND_आवरणम् + MLM_आवरणम् + TML_आवरणम् ,
/*0x00 : 0x80: 0x970  */ DEV_आवरणम् + PNJ_आवरणम् + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*
 * The length of the array is 128 to provide values for 0x900..0x97f.
 * The last 15 entries for 0x971..0x97f of the validity table are all zero
 * because no Indic script uses such Unicode code points.
 */
/*0x00 : 0x00: 0x9yz  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO
};

स्थिर ध्रुव uint16_t from_Unicode_सारणी[128]={
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
स्थिर ध्रुव uint16_t गन्तव्यम्_Unicode_सारणी[256]={
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

स्थिर ध्रुव uint16_t vowelSignESpecialCases[][2]={
	{ 2 /*length of array*/    , 0      },
	{ 0xA4 , 0x0904 },
};

स्थिर ध्रुव uint16_t nuktaSpecialCases[][2]={
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


#परिभाषय WRITE_गन्तव्यम्_TARGET_FROM_U(तर्काः_2,offsets,source,target,target_सीमा,targetByteUnit,err) UPRV_खण्डम्_MACRO_BEGIN { \
    int32_t विस्थापनम् = (int32_t)(source - तर्काः_2->source-1);                                        \
      /* write the targetUniChar  to target */                                                  \
    यदि(target < target_सीमा){                                                                   \
        यदि(targetByteUnit <= 0xFF){                                                             \
            *(target)++ = (uint8_t)(targetByteUnit);                                            \
            यदि(offsets){                                                                        \
                *(offsets++) = विस्थापनम्;                                                          \
            }                                                                                   \
        }अन्यथा{                                                                                  \
            यदि (targetByteUnit > 0xFFFF) {                                                      \
                *(target)++ = (uint8_t)(targetByteUnit>>16);                                    \
                यदि (offsets) {                                                                  \
                    --विस्थापनम्;                                                                   \
                    *(offsets++) = विस्थापनम्;                                                      \
                }                                                                               \
            }                                                                                   \
            यदि (!(target < target_सीमा)) {                                                      \
                तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] =    \
                                (uint8_t)(targetByteUnit >> 8);                                 \
                तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] =    \
                                (uint8_t)targetByteUnit;                                        \
                *err = U_बफर_अतिप्रवाहः_ERROR;                                                 \
            } अन्यथा {                                                                            \
                *(target)++ = (uint8_t)(targetByteUnit>>8);                                     \
                यदि(offsets){                                                                    \
                    *(offsets++) = विस्थापनम्;                                                      \
                }                                                                               \
                यदि(target < target_सीमा){                                                       \
                    *(target)++ = (uint8_t)  targetByteUnit;                                    \
                    यदि(offsets){                                                                \
                        *(offsets++) = विस्थापनम्                            ;                      \
                    }                                                                           \
                }अन्यथा{                                                                          \
                    तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] =\
                                (uint8_t) (targetByteUnit);                                     \
                    *err = U_बफर_अतिप्रवाहः_ERROR;                                             \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
    }अन्यथा{                                                                                      \
        यदि (targetByteUnit & 0xFF0000) {                                                        \
            तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] =        \
                        (uint8_t) (targetByteUnit >>16);                                        \
        }                                                                                       \
        यदि(targetByteUnit & 0xFF00){                                                            \
            तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] =        \
                        (uint8_t) (targetByteUnit >>8);                                         \
        }                                                                                       \
        तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] =            \
                        (uint8_t) (targetByteUnit);                                             \
        *err = U_बफर_अतिप्रवाहः_ERROR;                                                         \
    }                                                                                           \
} UPRV_खण्डम्_MACRO_अन्तः

/* Rules:
 *    Explicit Halant :
 *                      <HALANT> + <ZWNJ>
 *    Soft Halant :
 *                      <HALANT> + <ZWJ>
 */
स्थिर शून्य U_CALLCONV
UConverter_fromUnicode_ISCII_OFFSETS_LOGIC(
        UConverter_From_Unicode_तर्काः * तर्काः_2, UError_संहिता * err) {
    ध्रुव char16_t *source = तर्काः_2->source;
    ध्रुव char16_t *source_सीमा = तर्काः_2->source_सीमा;
    अचिह्नित वर्ण *target = (अचिह्नित वर्ण *) तर्काः_2->target;
    अचिह्नित वर्ण *target_सीमा = (अचिह्नित वर्ण *) तर्काः_2->target_सीमा;
    int32_t* offsets = तर्काः_2->offsets;
    uint32_t targetByteUnit = 0x0000;
    UChar32 sourceChar = 0x0000;
    UChar32 अस्थायी_Context_From_Unicode = 0x0000;    /* For special handling of the Gurmukhi script. */
    UConverter_दत्त_ISCII *converter_दत्त;
    uint16_t newDelta=0;
    uint16_t range = 0;
    UBool delta_परिवर्तितम् = असत्य;

    यदि ((तर्काः_2->converter == शून्यसूचकः) || (तर्काः_2->target_सीमा < तर्काः_2->target) || (तर्काः_2->source_सीमा < तर्काः_2->source)) {
        *err = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    /* initialize data */
    converter_दत्त=(UConverter_दत्त_ISCII*)तर्काः_2->converter->extraInfo;
    newDelta=converter_दत्त->currentDeltaFromUnicode;
    range = (uint16_t)(newDelta/DELTA);

    यदि ((sourceChar = तर्काः_2->converter->fromUChar32)!=0) {
        गच्छ getTrail;
    }

    /*writing the char to the output stream */
    यावत् (source < source_सीमा) {
        /* Write the language code following LF only if LF is not the last character. */
        यदि (तर्काः_2->converter->from_Unicode_स्थितिः == LF) {
            targetByteUnit = ATR<<8;
            targetByteUnit += (uint8_t) lookup_Initial_दत्त[range].isciiLang;
            तर्काः_2->converter->from_Unicode_स्थितिः = 0x0000;
            /* now append ATR and language code */
            WRITE_गन्तव्यम्_TARGET_FROM_U(तर्काः_2,offsets,source,target,target_सीमा,targetByteUnit,err);
            यदि (U_FAILURE(*err)) {
                भङ्ग;
            }
        }
        
        sourceChar = *source++;
        अस्थायी_Context_From_Unicode = converter_दत्त->contextCharFromUnicode;
        
        targetByteUnit = missingCharMarker;
        
        /*check if input is in ASCII and C0 control codes range*/
        यदि (sourceChar <= ASCII_अन्तः) {
            तर्काः_2->converter->from_Unicode_स्थितिः = sourceChar;
            WRITE_गन्तव्यम्_TARGET_FROM_U(तर्काः_2,offsets,source,target,target_सीमा,sourceChar,err);
            यदि (U_FAILURE(*err)) {
                भङ्ग;
            }
            अनुवर्त;
        }
        विकल्प (sourceChar) {
        पक्ष ZWNJ:
            /* contextChar has HALANT */
            यदि (converter_दत्त->contextCharFromUnicode) {
                converter_दत्त->contextCharFromUnicode = 0x00;
                targetByteUnit = ISCII_HALANT;
            } अन्यथा {
                /* consume ZWNJ and continue */
                converter_दत्त->contextCharFromUnicode = 0x00;
                अनुवर्त;
            }
            भङ्ग;
        पक्ष ZWJ:
            /* contextChar has HALANT */
            यदि (converter_दत्त->contextCharFromUnicode) {
                targetByteUnit = ISCII_NUKTA;
            } अन्यथा {
                targetByteUnit =ISCII_INV;
            }
            converter_दत्त->contextCharFromUnicode = 0x00;
            भङ्ग;
        स्वभाव:
            /* is the sourceChar in the INDIC_RANGE? */
            यदि ((uint16_t)(INDIC_खण्डम्_अन्तः-sourceChar) <= INDIC_RANGE) {
                /* Danda and Double Danda are valid in Northern scripts.. since Unicode
                 * does not include these codepoints in all Northern scrips we need to
                 * filter them out
                 */
                यदि (sourceChar!= DANDA && sourceChar != DOUBLE_DANDA) {
                    /* find out to which block the souceChar belongs*/
                    range =(uint16_t)((sourceChar-INDIC_खण्डम्_BEGIN)/DELTA);
                    newDelta =(uint16_t)(range*DELTA);

                    /* Now are we in the same block as the previous? */
                    यदि (newDelta!= converter_दत्त->currentDeltaFromUnicode || converter_दत्त->is_First_बफर) {
                        converter_दत्त->currentDeltaFromUnicode = newDelta;
                        converter_दत्त->current_आवरणम्_From_Unicode = lookup_Initial_दत्त[range].आवरणम्_Enum_2;
                        delta_परिवर्तितम् =सत्य;
                        converter_दत्त->is_First_बफर=असत्य;
                    }
                    
                    यदि (converter_दत्त->currentDeltaFromUnicode == PNJ_DELTA) { 
                        यदि (sourceChar == PNJ_TIPPI) {
                            /* Make sure Tippi is converted to Bindi. */
                            sourceChar = PNJ_BINDI;
                        } अन्यथा यदि (sourceChar == PNJ_ADHAK) {
                            /* This is for consonant cluster handling. */
                            converter_दत्त->contextCharFromUnicode = PNJ_ADHAK;
                        }
                        
                    }
                    /* Normalize all Indic codepoints to Devanagari and map them to ISCII */
                    /* now subtract the new delta from sourceChar*/
                    sourceChar -= converter_दत्त->currentDeltaFromUnicode;
                }

                /* get the target byte unit */
                targetByteUnit=from_Unicode_सारणी[(uint8_t)sourceChar];

                /* is the code point valid in current script? */
                यदि ((validity_सारणी[(uint8_t)sourceChar] & converter_दत्त->current_आवरणम्_From_Unicode)==0) {
                    /* Vocallic RR is assigned in ISCII Telugu and Unicode */
                    यदि (converter_दत्त->currentDeltaFromUnicode!=(TELUGU_DELTA) || sourceChar!=VOCALLIC_RR) {
                        targetByteUnit=missingCharMarker;
                    }
                }

                यदि (delta_परिवर्तितम्) {
                    /* we are in a script block which is different than
                     * previous sourceChar's script block write ATR and language codes
                     */
                    uint32_t अस्थायी=0;
                    अस्थायी =(uint16_t)(ATR<<8);
                    अस्थायी += (uint16_t)((uint8_t) lookup_Initial_दत्त[range].isciiLang);
                    /* reset */
                    delta_परिवर्तितम्=असत्य;
                    /* now append ATR and language code */
                    WRITE_गन्तव्यम्_TARGET_FROM_U(तर्काः_2,offsets,source,target,target_सीमा,अस्थायी,err);
                    यदि (U_FAILURE(*err)) {
                        भङ्ग;
                    }
                }
                
                यदि (converter_दत्त->currentDeltaFromUnicode == PNJ_DELTA && (sourceChar + PNJ_DELTA) == PNJ_ADHAK) {
                    अनुवर्त;
                }
            }
            /* reset context char */
            converter_दत्त->contextCharFromUnicode = 0x00;
            भङ्ग;
        }
        यदि (converter_दत्त->currentDeltaFromUnicode == PNJ_DELTA && अस्थायी_Context_From_Unicode == PNJ_ADHAK && isPNJConsonant((sourceChar + PNJ_DELTA))) {
            /* If the previous codepoint is Adhak and the current codepoint is a consonant, the targetByteUnit should be C + Halant + C. */
            /* reset context char */
            converter_दत्त->contextCharFromUnicode = 0x0000;
            targetByteUnit = targetByteUnit << 16 | ISCII_HALANT << 8 | targetByteUnit;
            /* write targetByteUnit to target */
            WRITE_गन्तव्यम्_TARGET_FROM_U(तर्काः_2, offsets, source, target, target_सीमा, targetByteUnit,err);
            यदि (U_FAILURE(*err)) {
                भङ्ग;
            }
        } अन्यथा यदि (targetByteUnit != missingCharMarker) {
            यदि (targetByteUnit==ISCII_HALANT) {
                converter_दत्त->contextCharFromUnicode = (char16_t)targetByteUnit;
            }
            /* write targetByteUnit to target*/
            WRITE_गन्तव्यम्_TARGET_FROM_U(तर्काः_2,offsets,source,target,target_सीमा,targetByteUnit,err);
            यदि (U_FAILURE(*err)) {
                भङ्ग;
            }
        } अन्यथा {
            /* oops.. the code point is unassigned */
            /*check if the char is a First surrogate*/
            यदि (U16_IS_SURROGATE(sourceChar)) {
                यदि (U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    यदि (source < source_सीमा) {
                        /* test the following code unit */
                        char16_t trail= (*source);
                        यदि (U16_IS_TRAIL(trail)) {
                            source++;
                            sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                            *err =U_INVALID_CHAR_प्राप्तम्;
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } अन्यथा {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_प्राप्तम्;
                        }
                    } अन्यथा {
                        /* no more input */
                        *err = U_ZERO_ERROR;
                    }
                } अन्यथा {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_प्राप्तम्;
                }
            } अन्यथा {
                /* callback(unassigned) for a BMP code point */
                *err = U_INVALID_CHAR_प्राप्तम्;
            }

            तर्काः_2->converter->fromUChar32=sourceChar;
            भङ्ग;
        }
    }/* end while(mySourceIndex<mySourceLength) */

    /*save the state and return */
    तर्काः_2->source = source;
    तर्काः_2->target = (वर्ण*)target;
}

स्थिर ध्रुव uint16_t lookup_सारणी[][2]={
    { ZERO,       ZERO     },     /*DEFAULT*/
    { ZERO,       ZERO     },     /*ROMAN*/
    { DEVANAGARI, DEV_आवरणम् },
    { BENGALI,    BNG_आवरणम् },
    { TAMIL,      TML_आवरणम् },
    { TELUGU,     KND_आवरणम् },
    { BENGALI,    BNG_आवरणम् },
    { ORIYA,      ORI_आवरणम् },
    { KANNADA,    KND_आवरणम् },
    { MALAYALAM,  MLM_आवरणम् },
    { GUJARATI,   GJR_आवरणम् },
    { GURMUKHI,   PNJ_आवरणम् }
};

#परिभाषय WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2,source,target,offsets,विस्थापनम्,targetUniChar,delta, err) UPRV_खण्डम्_MACRO_BEGIN { \
    /* add offset to current Indic Block */                                              \
    यदि(targetUniChar>ASCII_अन्तः &&                                                        \
           targetUniChar != ZWJ &&                                                       \
           targetUniChar != ZWNJ &&                                                      \
           targetUniChar != DANDA &&                                                     \
           targetUniChar != DOUBLE_DANDA){                                               \
                                                                                         \
           targetUniChar+=(uint16_t)(delta);                                             \
    }                                                                                    \
    /* now write the targetUniChar */                                                    \
    यदि(target<तर्काः_2->target_सीमा){                                                        \
        *(target)++ = (char16_t)targetUniChar;                                              \
        यदि(offsets){                                                                     \
            *(offsets)++ = (int32_t)(विस्थापनम्);                                            \
        }                                                                                \
    }अन्यथा{                                                                               \
        तर्काः_2->converter->UChar_Error_बफर[तर्काः_2->converter->UChar_Error_बफर_दीर्घता++] =   \
            (char16_t)targetUniChar;                                                        \
        *err = U_बफर_अतिप्रवाहः_ERROR;                                                  \
    }                                                                                    \
} UPRV_खण्डम्_MACRO_अन्तः

#परिभाषय GET_MAPPING(sourceChar,targetUniChar,दत्त) UPRV_खण्डम्_MACRO_BEGIN {              \
    targetUniChar = गन्तव्यम्_Unicode_सारणी[(sourceChar)] ;                                       \
    /* is the code point valid in current script? */                                     \
    यदि(sourceChar> ASCII_अन्तः &&                                                          \
            (validity_सारणी[(targetUniChar & 0x7F)] & दत्त->current_आवरणम्_गन्तव्यम्_Unicode)==0){    \
        /* Vocallic RR is assigned in ISCII Telugu and Unicode */                         \
        यदि(दत्त->current_Delta_गन्तव्यम्_Unicode!=(TELUGU_DELTA) ||                                \
                    targetUniChar!=VOCALLIC_RR){                                         \
            targetUniChar=missingCharMarker;                                             \
        }                                                                                \
    }                                                                                    \
} UPRV_खण्डम्_MACRO_अन्तः

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

स्थिर शून्य U_CALLCONV
UConverter_गन्तव्यम्_Unicode_ISCII_OFFSETS_LOGIC(UConverter_गन्तव्यम्_Unicode_तर्काः *तर्काः_2, UError_संहिता* err) {
    ध्रुव वर्ण *source = ( वर्ण *) तर्काः_2->source;
    char16_t *target = तर्काः_2->target;
    ध्रुव वर्ण *source_सीमा = तर्काः_2->source_सीमा;
    ध्रुव char16_t* target_सीमा = तर्काः_2->target_सीमा;
    uint32_t targetUniChar = 0x0000;
    uint8_t sourceChar = 0x0000;
    UConverter_दत्त_ISCII* दत्त;
    UChar32* गन्तव्यम्_Unicode_स्थितिः=शून्यसूचकः;
    UChar32 अस्थायी_Target_Uni_Char = 0x0000;
    char16_t* context_Char_गन्तव्यम्_Unicode= शून्यसूचकः;
    UBool प्राप्तम्;
    पूर्णाङ्क i; 
    पूर्णाङ्क विस्थापनम् = 0;

    यदि ((तर्काः_2->converter == शून्यसूचकः) || (target < तर्काः_2->target) || (source < तर्काः_2->source)) {
        *err = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    दत्त = (UConverter_दत्त_ISCII*)(तर्काः_2->converter->extraInfo);
    context_Char_गन्तव्यम्_Unicode = &दत्त->context_Char_गन्तव्यम्_Unicode; /* contains previous ISCII codepoint visited */
    गन्तव्यम्_Unicode_स्थितिः = (UChar32*)&तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः;/* contains the mapping to Unicode of the above codepoint*/

    यावत् (U_SUCCESS(*err) && source<source_सीमा) {

        targetUniChar = missingCharMarker;

        यदि (target < target_सीमा) {
            sourceChar = (अचिह्नित वर्ण)*(source)++;

            /* look at the post-context perform special processing */
            यदि (*context_Char_गन्तव्यम्_Unicode==ATR) {

                /* If we have ATR in *contextCharToUnicode then we need to change our
                 * state to the Indic Script specified by sourceChar
                 */

                /* check if the sourceChar is supported script range*/
                यदि ((uint8_t)(PNJ-sourceChar)<=PNJ-DEV) {
                    दत्त->current_Delta_गन्तव्यम्_Unicode = (uint16_t)(lookup_सारणी[sourceChar & 0x0F][0] * DELTA);
                    दत्त->current_आवरणम्_गन्तव्यम्_Unicode = (आवरणम्_Enum)lookup_सारणी[sourceChar & 0x0F][1];
                } अन्यथा यदि (sourceChar==DEF) {
                    /* switch back to default */
                    दत्त->current_Delta_गन्तव्यम्_Unicode = दत्त->def_Delta_गन्तव्यम्_Unicode;
                    दत्त->current_आवरणम्_गन्तव्यम्_Unicode = दत्त->def_आवरणम्_गन्तव्यम्_Unicode;
                } अन्यथा {
                    यदि ((sourceChar >= 0x21 && sourceChar <= 0x3F)) {
                        /* these are display codes consume and continue */
                    } अन्यथा {
                        *err =U_ILLEGAL_CHAR_प्राप्तम्;
                        /* reset */
                        *context_Char_गन्तव्यम्_Unicode=NO_CHAR_MARKER;
                        गच्छ CALLBACK;
                    }
                }

                /* reset */
                *context_Char_गन्तव्यम्_Unicode=NO_CHAR_MARKER;

                अनुवर्त;

            } अन्यथा यदि (*context_Char_गन्तव्यम्_Unicode==EXT) {
                /* check if sourceChar is in 0xA1-0xEE range */
                यदि ((uint8_t) (EXT_RANGE_अन्तः - sourceChar) <= (EXT_RANGE_अन्तः - EXT_RANGE_BEGIN)) {
                    /* We currently support only Anudatta and Devanagari abbreviation sign */
                    यदि (sourceChar==0xBF || sourceChar == 0xB8) {
                        targetUniChar = (sourceChar==0xBF) ? DEV_ABBR_SIGN : DEV_ANUDATTA;
                        
                        /* find out if the mapping is valid in this state */
                        यदि (validity_सारणी[(uint8_t)targetUniChar] & दत्त->current_आवरणम्_गन्तव्यम्_Unicode) {
                            *context_Char_गन्तव्यम्_Unicode= NO_CHAR_MARKER;

                            /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                            यदि (दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः) {
                                WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2,source,target,तर्काः_2->offsets,(source-तर्काः_2->source -1),दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः,0,err);
                                दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः = 0x0000;
                            }
                            /* write to target */
                            WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2,source,target,तर्काः_2->offsets,(source-तर्काः_2->source -2),targetUniChar,दत्त->current_Delta_गन्तव्यम्_Unicode,err);

                            अनुवर्त;
                        }
                    }
                    /* byte unit is unassigned */
                    targetUniChar = missingCharMarker;
                    *err= U_INVALID_CHAR_प्राप्तम्;
                } अन्यथा {
                    /* only 0xA1 - 0xEE are legal after EXT char */
                    *context_Char_गन्तव्यम्_Unicode= NO_CHAR_MARKER;
                    *err = U_ILLEGAL_CHAR_प्राप्तम्;
                }
                गच्छ CALLBACK;
            } अन्यथा यदि (*context_Char_गन्तव्यम्_Unicode==ISCII_INV) {
                यदि (sourceChar==ISCII_HALANT) {
                    targetUniChar = 0x0020; /* replace with space according to Indic FAQ */
                } अन्यथा {
                    targetUniChar = ZWJ;
                }

                /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                यदि (दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः) {
                    WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2,source,target,तर्काः_2->offsets,(source-तर्काः_2->source -1),दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः,0,err);
                    दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः = 0x0000;
                }
                /* write to target */
                WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2,source,target,तर्काः_2->offsets,(source-तर्काः_2->source -2),targetUniChar,दत्त->current_Delta_गन्तव्यम्_Unicode,err);
                /* reset */
                *context_Char_गन्तव्यम्_Unicode=NO_CHAR_MARKER;
            }

            /* look at the pre-context and perform special processing */
            विकल्प (sourceChar) {
            पक्ष ISCII_INV:
            पक्ष EXT:
            पक्ष ATR:
                *context_Char_गन्तव्यम्_Unicode = (char16_t)sourceChar;

                यदि (*गन्तव्यम्_Unicode_स्थितिः != missingCharMarker) {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    यदि (दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः) {
                        WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2,source,target,तर्काः_2->offsets,(source-तर्काः_2->source -1),दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः,0,err);
                        दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः = 0x0000;
                    }
                    WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2,source,target,तर्काः_2->offsets,(source-तर्काः_2->source -2),*गन्तव्यम्_Unicode_स्थितिः,दत्त->current_Delta_गन्तव्यम्_Unicode,err);
                    *गन्तव्यम्_Unicode_स्थितिः = missingCharMarker;
                }
                अनुवर्त;
            पक्ष ISCII_DANDA:
                /* handle double danda*/
                यदि (*context_Char_गन्तव्यम्_Unicode== ISCII_DANDA) {
                    targetUniChar = DOUBLE_DANDA;
                    /* clear the context */
                    *context_Char_गन्तव्यम्_Unicode = NO_CHAR_MARKER;
                    *गन्तव्यम्_Unicode_स्थितिः = missingCharMarker;
                } अन्यथा {
                    GET_MAPPING(sourceChar,targetUniChar,दत्त);
                    *context_Char_गन्तव्यम्_Unicode = sourceChar;
                }
                भङ्ग;
            पक्ष ISCII_HALANT:
                /* handle explicit halant */
                यदि (*context_Char_गन्तव्यम्_Unicode == ISCII_HALANT) {
                    targetUniChar = ZWNJ;
                    /* clear the context */
                    *context_Char_गन्तव्यम्_Unicode = NO_CHAR_MARKER;
                } अन्यथा {
                    GET_MAPPING(sourceChar,targetUniChar,दत्त);
                    *context_Char_गन्तव्यम्_Unicode = sourceChar;
                }
                भङ्ग;
            पक्ष 0x0A:
            पक्ष 0x0D:
                दत्त->reset_गन्तव्यम्_Default_गन्तव्यम्_Unicode = सत्य;
                GET_MAPPING(sourceChar,targetUniChar,दत्त)
                ;
                *context_Char_गन्तव्यम्_Unicode = sourceChar;
                भङ्ग;

            पक्ष ISCII_VOWEL_SIGN_E:
                i=1;
                प्राप्तम्=असत्य;
                प्रति (; i<vowelSignESpecialCases[0][0]; i++) {
                    U_ASSERT(i<UPRV_LENGTHOF(vowelSignESpecialCases));
                    यदि (vowelSignESpecialCases[i][0]==(uint8_t)*context_Char_गन्तव्यम्_Unicode) {
                        targetUniChar=vowelSignESpecialCases[i][1];
                        प्राप्तम्=सत्य;
                        भङ्ग;
                    }
                }
                यदि (प्राप्तम्) {
                    /* find out if the mapping is valid in this state */
                    यदि (validity_सारणी[(uint8_t)targetUniChar] & दत्त->current_आवरणम्_गन्तव्यम्_Unicode) {
                        /*targetUniChar += data->currentDeltaToUnicode ;*/
                        *context_Char_गन्तव्यम्_Unicode= NO_CHAR_MARKER;
                        *गन्तव्यम्_Unicode_स्थितिः = missingCharMarker;
                        भङ्ग;
                    }
                }
                GET_MAPPING(sourceChar,targetUniChar,दत्त);
                *context_Char_गन्तव्यम्_Unicode = sourceChar;
                भङ्ग;

            पक्ष ISCII_NUKTA:
                /* handle soft halant */
                यदि (*context_Char_गन्तव्यम्_Unicode == ISCII_HALANT) {
                    targetUniChar = ZWJ;
                    /* clear the context */
                    *context_Char_गन्तव्यम्_Unicode = NO_CHAR_MARKER;
                    भङ्ग;
                } अन्यथा यदि (दत्त->current_Delta_गन्तव्यम्_Unicode == PNJ_DELTA && दत्त->context_Char_गन्तव्यम्_Unicode == 0xc0) {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    यदि (दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः) {
                        WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2,source,target,तर्काः_2->offsets,(source-तर्काः_2->source -1),दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः,0,err);
                        दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः = 0x0000;
                    }
                    /* We got here because ISCII_NUKTA was preceded by 0xc0 and we are converting Gurmukhi.
                     * In that case we must convert (0xc0 0xe9) to (\u0a5c\u0a4d\u0a39).
                     */
                    targetUniChar = PNJ_RRA;
                    WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2, source, target, तर्काः_2->offsets, (source-तर्काः_2->source)-2, targetUniChar, 0, err);
                    यदि (U_SUCCESS(*err)) {
                        targetUniChar = PNJ_SIGN_VIRAMA;
                        WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2, source, target, तर्काः_2->offsets, (source-तर्काः_2->source)-2, targetUniChar, 0, err);
                        यदि (U_SUCCESS(*err)) {
                            targetUniChar = PNJ_HA;
                            WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2, source, target, तर्काः_2->offsets, (source-तर्काः_2->source)-2, targetUniChar, 0, err);
                        } अन्यथा {
                            तर्काः_2->converter->UChar_Error_बफर[तर्काः_2->converter->UChar_Error_बफर_दीर्घता++]= PNJ_HA;
                        }
                    } अन्यथा {
                        तर्काः_2->converter->UChar_Error_बफर[तर्काः_2->converter->UChar_Error_बफर_दीर्घता++]= PNJ_SIGN_VIRAMA;
                        तर्काः_2->converter->UChar_Error_बफर[तर्काः_2->converter->UChar_Error_बफर_दीर्घता++]= PNJ_HA;
                    }
                    *गन्तव्यम्_Unicode_स्थितिः = missingCharMarker;
                    दत्त->context_Char_गन्तव्यम्_Unicode = NO_CHAR_MARKER;
                    अनुवर्त;
                } अन्यथा {
                    /* try to handle <CHAR> + ISCII_NUKTA special mappings */
                    i=1;
                    प्राप्तम् =असत्य;
                    प्रति (; i<nuktaSpecialCases[0][0]; i++) {
                        यदि (nuktaSpecialCases[i][0]==(uint8_t)
                                *context_Char_गन्तव्यम्_Unicode) {
                            targetUniChar=nuktaSpecialCases[i][1];
                            प्राप्तम् =सत्य;
                            भङ्ग;
                        }
                    }
                    यदि (प्राप्तम्) {
                        /* find out if the mapping is valid in this state */
                        यदि (validity_सारणी[(uint8_t)targetUniChar] & दत्त->current_आवरणम्_गन्तव्यम्_Unicode) {
                            /*targetUniChar += data->currentDeltaToUnicode ;*/
                            *context_Char_गन्तव्यम्_Unicode= NO_CHAR_MARKER;
                            *गन्तव्यम्_Unicode_स्थितिः = missingCharMarker;
                            यदि (दत्त->current_Delta_गन्तव्यम्_Unicode == PNJ_DELTA) {
                                /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                                यदि (दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः) {
                                    WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2,source,target,तर्काः_2->offsets,(source-तर्काः_2->source -1),दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः,0,err);
                                    दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः = 0x0000;
                                }
                                WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2,source,target,तर्काः_2->offsets,(source-तर्काः_2->source -2),targetUniChar,दत्त->current_Delta_गन्तव्यम्_Unicode,err);
                                अनुवर्त;
                            }
                            भङ्ग;
                        }
                        /* else fall through to default */
                    }
                    /* else fall through to default */
                    U_FALLTHROUGH;
                }
            स्वभाव:GET_MAPPING(sourceChar,targetUniChar,दत्त)
                ;
                *context_Char_गन्तव्यम्_Unicode = sourceChar;
                भङ्ग;
            }

            यदि (*गन्तव्यम्_Unicode_स्थितिः != missingCharMarker) {
                /* Check to make sure that consonant clusters are handled correct for Gurmukhi script. */
                यदि (दत्त->current_Delta_गन्तव्यम्_Unicode == PNJ_DELTA && दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः != 0 && isPNJConsonant(दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः) &&
                        (*गन्तव्यम्_Unicode_स्थितिः + PNJ_DELTA) == PNJ_SIGN_VIRAMA && ((UChar32)(targetUniChar + PNJ_DELTA) == दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः)) {
                    /* Consonant clusters C + HALANT + C should be encoded as ADHAK + C */
                    विस्थापनम् = (पूर्णाङ्क)(source-तर्काः_2->source - 3);
                    अस्थायी_Target_Uni_Char = PNJ_ADHAK; /* This is necessary to avoid some compiler warnings. */
                    WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2,source,target,तर्काः_2->offsets,विस्थापनम्,अस्थायी_Target_Uni_Char,0,err);
                    WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2,source,target,तर्काः_2->offsets,विस्थापनम्,दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः,0,err);
                    दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः = 0x0000; /* reset the previous unicode code point */
                    *गन्तव्यम्_Unicode_स्थितिः = missingCharMarker;
                    अनुवर्त;
                } अन्यथा {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    यदि (दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः) {
                        WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2,source,target,तर्काः_2->offsets,(source-तर्काः_2->source -1),दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः,0,err);
                        दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः = 0x0000;
                    }
                    /* Check to make sure that Bindi and Tippi are handled correctly for Gurmukhi script. 
                     * If 0xA2 is preceded by a codepoint in the PNJ_BINDI_TIPPI_SET then the target codepoint should be Tippi instead of Bindi.
                     */
                    यदि (दत्त->current_Delta_गन्तव्यम्_Unicode == PNJ_DELTA && (targetUniChar + PNJ_DELTA) == PNJ_BINDI && isPNJBindiTippi((*गन्तव्यम्_Unicode_स्थितिः + PNJ_DELTA))) {
                        targetUniChar = PNJ_TIPPI - PNJ_DELTA;
                        WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2,source,target,तर्काः_2->offsets,(source-तर्काः_2->source -2),*गन्तव्यम्_Unicode_स्थितिः,PNJ_DELTA,err);
                    } अन्यथा यदि (दत्त->current_Delta_गन्तव्यम्_Unicode == PNJ_DELTA && (targetUniChar + PNJ_DELTA) == PNJ_SIGN_VIRAMA && isPNJConsonant((*गन्तव्यम्_Unicode_स्थितिः + PNJ_DELTA))) {
                        /* Store the current toUnicodeStatus code point for later handling of consonant cluster in Gurmukhi. */
                        दत्त->पूर्व_गन्तव्यम्_Unicode_स्थितिः = *गन्तव्यम्_Unicode_स्थितिः + PNJ_DELTA;
                    } अन्यथा {
                        /* write the previously mapped codepoint */
                        WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2,source,target,तर्काः_2->offsets,(source-तर्काः_2->source -2),*गन्तव्यम्_Unicode_स्थितिः,दत्त->current_Delta_गन्तव्यम्_Unicode,err);
                    }
                }
                *गन्तव्यम्_Unicode_स्थितिः = missingCharMarker;
            }

            यदि (targetUniChar != missingCharMarker) {
                /* now save the targetUniChar for delayed write */
                *गन्तव्यम्_Unicode_स्थितिः = (char16_t) targetUniChar;
                यदि (दत्त->reset_गन्तव्यम्_Default_गन्तव्यम्_Unicode) {
                    दत्त->current_Delta_गन्तव्यम्_Unicode = दत्त->def_Delta_गन्तव्यम्_Unicode;
                    दत्त->current_आवरणम्_गन्तव्यम्_Unicode = दत्त->def_आवरणम्_गन्तव्यम्_Unicode;
                    दत्त->reset_गन्तव्यम्_Default_गन्तव्यम्_Unicode=असत्य;
                }
            } अन्यथा {

                /* we reach here only if targetUniChar == missingCharMarker
                 * so assign codes to reason and err
                 */
                *err = U_INVALID_CHAR_प्राप्तम्;
CALLBACK:
                तर्काः_2->converter->गन्तव्यम्_UBytes[0] = sourceChar;
                तर्काः_2->converter->गन्तव्यम्_ULength = 1;
                भङ्ग;
            }

        } अन्यथा {
            *err =U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }
    }

    यदि (U_SUCCESS(*err) && तर्काः_2->flush && source == source_सीमा) {
        /* end of the input stream */
        UConverter *cnv = तर्काः_2->converter;

        यदि (*context_Char_गन्तव्यम्_Unicode==ATR || *context_Char_गन्तव्यम्_Unicode==EXT || *context_Char_गन्तव्यम्_Unicode==ISCII_INV) {
            /* set toUBytes[] */
            cnv->गन्तव्यम्_UBytes[0] = (uint8_t)*context_Char_गन्तव्यम्_Unicode;
            cnv->गन्तव्यम्_ULength = 1;

            /* avoid looping on truncated sequences */
            *context_Char_गन्तव्यम्_Unicode = NO_CHAR_MARKER;
        } अन्यथा {
            cnv->गन्तव्यम्_ULength = 0;
        }

        यदि (*गन्तव्यम्_Unicode_स्थितिः != missingCharMarker) {
            /* output a remaining target character */
            WRITE_गन्तव्यम्_TARGET_गन्तव्यम्_U(तर्काः_2,source,target,तर्काः_2->offsets,(source - तर्काः_2->source -1),*गन्तव्यम्_Unicode_स्थितिः,दत्त->current_Delta_गन्तव्यम्_Unicode,err);
            *गन्तव्यम्_Unicode_स्थितिः = missingCharMarker;
        }
    }

    तर्काः_2->target = target;
    तर्काः_2->source = source;
}

/* structure for SafeClone calculations */
संरचना cloneISCIIStruct {
    UConverter cnv;
    UConverter_दत्त_ISCII mydata;
};

स्थिर UConverter * U_CALLCONV
_ISCII_SafeClone(ध्रुव UConverter *cnv,
              शून्य *राशिः_बफर,
              int32_t *p_बफर_आकार,
              UError_संहिता *स्थितिः)
{
    संरचना cloneISCIIStruct * localClone;
    int32_t बफर_आकार_Needed = परिमाण(संरचना cloneISCIIStruct);

    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    यदि (*p_बफर_आकार == 0) { /* 'preflighting' request - set needed size into *pBufferSize */
        *p_बफर_आकार = बफर_आकार_Needed;
        प्रत्यागम शून्यसूचकः;
    }

    localClone = (संरचना cloneISCIIStruct *)राशिः_बफर;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, परिमाण(UConverter_दत्त_ISCII));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = सत्य;

    प्रत्यागम &localClone->cnv;
}

स्थिर शून्य U_CALLCONV
ISCIIGet_Unicode_समुच्चयः(ध्रुव UConverter *cnv,
                    ध्रुव USetAdder *sa,
                    UConverter_Unicode_समुच्चयः which,
                    UError_संहिता *p_Error_संहिता)
{
    (शून्य)cnv;
    (शून्य)which;
    (शून्य)p_Error_संहिता;
    int32_t idx, script;
    uint8_t mask;

    /* Since all ISCII versions allow switching to other ISCII
    scripts, we add all roundtrippable characters to this set. */
    sa->सङ्कलय_Range(sa->समुच्चयः, 0, ASCII_अन्तः);
    प्रति (script = DEVANAGARI; script <= MALAYALAM; script++) {
        mask = (uint8_t)(lookup_Initial_दत्त[script].आवरणम्_Enum_2);
        प्रति (idx = 0; idx < DELTA; idx++) {
            /* added check for TELUGU character */
            यदि ((validity_सारणी[idx] & mask) || (script==TELUGU && idx==0x31)) {
                sa->सङ्कलय(sa->समुच्चयः, idx + (script * DELTA) + INDIC_खण्डम्_BEGIN);
            }
        }
    }
    sa->सङ्कलय(sa->समुच्चयः, DANDA);
    sa->सङ्कलय(sa->समुच्चयः, DOUBLE_DANDA);
    sa->सङ्कलय(sa->समुच्चयः, ZWNJ);
    sa->सङ्कलय(sa->समुच्चयः, ZWJ);
}
U_CDECL_अन्तः
स्थिर ध्रुव UConverterImpl _ISCIIImpl={

    UCNV_ISCII,

    शून्यसूचकः,
    शून्यसूचकः,

    _ISCIIOpen,
    _ISCIIClose,
    _ISCIIReset,

    UConverter_गन्तव्यम्_Unicode_ISCII_OFFSETS_LOGIC,
    UConverter_गन्तव्यम्_Unicode_ISCII_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISCII_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISCII_OFFSETS_LOGIC,
    शून्यसूचकः,

    शून्यसूचकः,
    _ISCIIgetName,
    शून्यसूचकः,
    _ISCII_SafeClone,
    ISCIIGet_Unicode_समुच्चयः,
    शून्यसूचकः,
    शून्यसूचकः
};

स्थिर ध्रुव UConverter_Static_दत्त ISCIIStatic_दत्त={
    परिमाण(UConverter_Static_दत्त),
        "ISCII",
         0,
         UCNV_IBM,
         UCNV_ISCII,
         1,
         4,
        { 0x1a, 0, 0, 0 },
        0x1,
        असत्य,
        असत्य,
        0x0,
        0x0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

ध्रुव UConverter_Shared_दत्त _ISCIIData=
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&ISCIIStatic_दत्त, &_ISCIIImpl);

#यदि_अन्तः /* #if !UCONFIG_NO_LEGACY_CONVERSION */
