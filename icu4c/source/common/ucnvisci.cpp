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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#அந்தர்பாவ "unicode/ucnv.h"
#அந்தர்பாவ "unicode/ucnv_cb.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "ucnv_bld.h"
#அந்தர்பாவ "ucnv_cnv.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "uassert.h"

#define UCNV_OPTIONS_VERSION_MASK 0xf
#define NUKTA               0x093c
#define HALANT              0x094d
#define ZWNJ                0x200c /* Zero Width Non Joiner */
#define ZWJ                 0x200d /* Zero width Joiner */
#define INVALID_CHAR        0xffff
#define ATR                 0xEF   /* Attribute code */
#define EXT                 0xF0   /* Extension code */
#define DANDA               0x0964
#define DOUBLE_DANDA        0x0965
#define ISCII_NUKTA         0xE9
#define ISCII_HALANT        0xE8
#define ISCII_DANDA         0xEA
#define ISCII_INV           0xD9
#define ISCII_VOWEL_SIGN_E  0xE0
#define INDIC_BLOCK_BEGIN   0x0900
#define INDIC_BLOCK_END     0x0D7F
#define INDIC_RANGE         (INDIC_BLOCK_END - INDIC_BLOCK_BEGIN)
#define VOCALLIC_RR         0x0931
#define LF                  0x0A
#define ASCII_END           0xA0
#define NO_CHAR_MARKER      0xFFFE
#define TELUGU_DELTA        DELTA * TELUGU
#define DEV_ABBR_SIGN       0x0970
#define DEV_ANUDATTA        0x0952
#define EXT_RANGE_BEGIN     0xA1
#define EXT_RANGE_END       0xEE

#define PNJ_DELTA           0x0100
#define PNJ_BINDI           0x0A02
#define PNJ_TIPPI           0x0A70
#define PNJ_SIGN_VIRAMA     0x0A4D
#define PNJ_ADHAK           0x0A71
#define PNJ_HA              0x0A39
#define PNJ_RRA             0x0A5C

வகைவரை எண்ணிக்கை {
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
வகைவரை எண்ணிக்கை {
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

வகைவரை எண்ணிக்கை {
    DEV_MASK =0x80,
    PNJ_MASK =0x40,
    GJR_MASK =0x20,
    ORI_MASK =0x10,
    BNG_MASK =0x08,
    KND_MASK =0x04,
    MLM_MASK =0x02,
    TML_MASK =0x01,
    ZERO =0x00
}MaskEnum;

#define ISCII_CNV_PREFIX "ISCII,version="

வகைவரை கட்டமைப்பு {
    char16_t contextCharToUnicode;         /* previous Unicode codepoint for contextual analysis */
    char16_t contextCharFromUnicode;       /* previous Unicode codepoint for contextual analysis */
    uint16_t defDeltaToUnicode;         /* delta for switching to default state when DEF is encountered  */
    uint16_t currentDeltaFromUnicode;   /* current delta in Indic block */
    uint16_t currentDeltaToUnicode;     /* current delta in Indic block */
    MaskEnum currentMaskFromUnicode;    /* mask for current state in toUnicode */
    MaskEnum currentMaskToUnicode;      /* mask for current state in toUnicode */
    MaskEnum defMaskToUnicode;          /* mask for default state in toUnicode */
    UBool isFirstBuffer;                /* boolean for fromUnicode to see if we need to announce the first script */
    UBool resetToDefaultToUnicode;      /* boolean for resetting to default delta and mask when a newline is encountered*/
    எழுத்து name[அளவு(ISCII_CNV_PREFIX) + 1];
    UChar32 prevToUnicodeStatus;        /* Hold the previous toUnicodeStatus. This is necessary because we may need to know the last two code points. */
} UConverterDataISCII;

வகைவரை கட்டமைப்பு LookupDataStruct {
    UniLang uniLang;
    MaskEnum maskEnum;
    ISCIILang isciiLang;
} LookupDataStruct;

நிலையான மாறிலி LookupDataStruct lookupInitialData[]={
    { DEVANAGARI, DEV_MASK,  DEV },
    { BENGALI,    BNG_MASK,  BNG },
    { GURMUKHI,   PNJ_MASK,  PNJ },
    { GUJARATI,   GJR_MASK,  GJR },
    { ORIYA,      ORI_MASK,  ORI },
    { TAMIL,      TML_MASK,  TML },
    { TELUGU,     KND_MASK,  TLG },
    { KANNADA,    KND_MASK,  KND },
    { MALAYALAM,  MLM_MASK,  MLM }
};

/*
 * For special handling of certain Gurmukhi characters.
 * Bit 0 (value 1): PNJ consonant
 * Bit 1 (value 2): PNJ Bindi Tippi
 */
நிலையான மாறிலி uint8_t pnjMap[80] = {
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

நிலையான UBool
isPNJConsonant(UChar32 c) {
    எனில் (c < 0xa00 || 0xa50 <= c) {
        திரும்பு பொய்;
    } இல்லையெனில் {
        திரும்பு pnjMap[c - 0xa00] & 1;
    }
}

நிலையான UBool
isPNJBindiTippi(UChar32 c) {
    எனில் (c < 0xa00 || 0xa50 <= c) {
        திரும்பு பொய்;
    } இல்லையெனில் {
        திரும்பு pnjMap[c - 0xa00] >> 1;
    }
}
U_CDECL_BEGIN
நிலையான வெறுமை  U_CALLCONV
_ISCIIOpen(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode) {
    எனில்(pArgs->onlyTestIsLoadable) {
        திரும்பு;
    }

    cnv->extraInfo = uprv_malloc(அளவு(UConverterDataISCII));

    எனில் (cnv->extraInfo != nullptr) {
        int32_t len=0;
        UConverterDataISCII *converterData=
                (UConverterDataISCII *) cnv->extraInfo;
        converterData->contextCharToUnicode=NO_CHAR_MARKER;
        cnv->toUnicodeStatus = missingCharMarker;
        converterData->contextCharFromUnicode=0x0000;
        converterData->resetToDefaultToUnicode=பொய்;
        /* check if the version requested is supported */
        எனில் ((pArgs->options & UCNV_OPTIONS_VERSION_MASK) < 9) {
            /* initialize state variables */
            converterData->currentDeltaFromUnicode
                    = converterData->currentDeltaToUnicode
                            = converterData->defDeltaToUnicode = (uint16_t)(lookupInitialData[pArgs->options & UCNV_OPTIONS_VERSION_MASK].uniLang * DELTA);

            converterData->currentMaskFromUnicode
                    = converterData->currentMaskToUnicode
                            = converterData->defMaskToUnicode = lookupInitialData[pArgs->options & UCNV_OPTIONS_VERSION_MASK].maskEnum;
            
            converterData->isFirstBuffer=உண்மை;
            (வெறுமை)uprv_strcpy(converterData->name, ISCII_CNV_PREFIX);
            len = (int32_t)uprv_strlen(converterData->name);
            converterData->name[len]= (எழுத்து)((pArgs->options & UCNV_OPTIONS_VERSION_MASK) + '0');
            converterData->name[len+1]=0;
            
            converterData->prevToUnicodeStatus = 0x0000;
        } இல்லையெனில் {
            uprv_free(cnv->extraInfo);
            cnv->extraInfo = nullptr;
            *errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        }

    } இல்லையெனில் {
        *errorCode =U_MEMORY_ALLOCATION_ERROR;
    }
}

நிலையான வெறுமை U_CALLCONV
_ISCIIClose(UConverter *cnv) {
    எனில் (cnv->extraInfo!=nullptr) {
        எனில் (!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo=nullptr;
    }
}

நிலையான மாறிலி எழுத்து*  U_CALLCONV
_ISCIIgetName(மாறிலி UConverter* cnv) {
    எனில் (cnv->extraInfo) {
        UConverterDataISCII* myData= (UConverterDataISCII*)cnv->extraInfo;
        திரும்பு myData->name;
    }
    திரும்பு nullptr;
}

நிலையான வெறுமை U_CALLCONV
_ISCIIReset(UConverter *cnv, UConverterResetChoice choice) {
    UConverterDataISCII* data =(UConverterDataISCII *) (cnv->extraInfo);
    எனில் (choice<=UCNV_RESET_TO_UNICODE) {
        cnv->toUnicodeStatus = missingCharMarker;
        cnv->mode=0;
        data->currentDeltaToUnicode=data->defDeltaToUnicode;
        data->currentMaskToUnicode = data->defMaskToUnicode;
        data->contextCharToUnicode=NO_CHAR_MARKER;
        data->prevToUnicodeStatus = 0x0000;
    }
    எனில் (choice!=UCNV_RESET_TO_UNICODE) {
        cnv->fromUChar32=0x0000;
        data->contextCharFromUnicode=0x00;
        data->currentMaskFromUnicode=data->defMaskToUnicode;
        data->currentDeltaFromUnicode=data->defDeltaToUnicode;
        data->isFirstBuffer=உண்மை;
        data->resetToDefaultToUnicode=பொய்;
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

நிலையான மாறிலி uint8_t validityTable[128] = {
/* This state table is tool generated please do not edit unless you know exactly what you are doing */
/* Note: This table was edited to mirror the Windows XP implementation */
/*ISCII:Valid:Unicode */
/*0xa0 : 0x00: 0x900  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xa1 : 0xb8: 0x901  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + ZERO     + ZERO     + ZERO     ,
/*0xa2 : 0xfe: 0x902  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xa3 : 0xbf: 0x903  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0x00 : 0x00: 0x904  */ DEV_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xa4 : 0xff: 0x905  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xa5 : 0xff: 0x906  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xa6 : 0xff: 0x907  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xa7 : 0xff: 0x908  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xa8 : 0xff: 0x909  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xa9 : 0xff: 0x90a  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xaa : 0xfe: 0x90b  */ DEV_MASK + ZERO     + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0x00 : 0x00: 0x90c  */ DEV_MASK + ZERO     + ZERO     + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0xae : 0x80: 0x90d  */ DEV_MASK + ZERO     + GJR_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xab : 0x87: 0x90e  */ DEV_MASK + ZERO     + ZERO     + ZERO     + ZERO     + KND_MASK + MLM_MASK + TML_MASK ,
/*0xac : 0xff: 0x90f  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xad : 0xff: 0x910  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xb2 : 0x80: 0x911  */ DEV_MASK + ZERO     + GJR_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xaf : 0x87: 0x912  */ DEV_MASK + ZERO     + ZERO     + ZERO     + ZERO     + KND_MASK + MLM_MASK + TML_MASK ,
/*0xb0 : 0xff: 0x913  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xb1 : 0xff: 0x914  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xb3 : 0xff: 0x915  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xb4 : 0xfe: 0x916  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0xb5 : 0xfe: 0x917  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0xb6 : 0xfe: 0x918  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0xb7 : 0xff: 0x919  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xb8 : 0xff: 0x91a  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xb9 : 0xfe: 0x91b  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0xba : 0xff: 0x91c  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xbb : 0xfe: 0x91d  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0xbc : 0xff: 0x91e  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xbd : 0xff: 0x91f  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xbe : 0xfe: 0x920  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0xbf : 0xfe: 0x921  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0xc0 : 0xfe: 0x922  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0xc1 : 0xff: 0x923  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xc2 : 0xff: 0x924  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xc3 : 0xfe: 0x925  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0xc4 : 0xfe: 0x926  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0xc5 : 0xfe: 0x927  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0xc6 : 0xff: 0x928  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xc7 : 0x81: 0x929  */ DEV_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + TML_MASK ,
/*0xc8 : 0xff: 0x92a  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xc9 : 0xfe: 0x92b  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0xca : 0xfe: 0x92c  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0xcb : 0xfe: 0x92d  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0xcc : 0xfe: 0x92e  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xcd : 0xff: 0x92f  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xcf : 0xff: 0x930  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xd0 : 0x87: 0x931  */ DEV_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + MLM_MASK + TML_MASK ,
/*0xd1 : 0xff: 0x932  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xd2 : 0xb7: 0x933  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + ZERO     + KND_MASK + MLM_MASK + TML_MASK ,
/*0xd3 : 0x83: 0x934  */ DEV_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + MLM_MASK + TML_MASK ,
/*0xd4 : 0xff: 0x935  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + ZERO     + KND_MASK + MLM_MASK + TML_MASK ,
/*0xd5 : 0xfe: 0x936  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0xd6 : 0xbf: 0x937  */ DEV_MASK + ZERO     + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xd7 : 0xff: 0x938  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xd8 : 0xff: 0x939  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0x00 : 0x00: 0x93A  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x93B  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe9 : 0xda: 0x93c  */ DEV_MASK + PNJ_MASK + ZERO     + ORI_MASK + BNG_MASK + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x93d  */ DEV_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xda : 0xff: 0x93e  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xdb : 0xff: 0x93f  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xdc : 0xff: 0x940  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xdd : 0xff: 0x941  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xde : 0xff: 0x942  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xdf : 0xbe: 0x943  */ DEV_MASK + ZERO     + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0x00 : 0x00: 0x944  */ DEV_MASK + ZERO     + GJR_MASK + ZERO     + BNG_MASK + KND_MASK + ZERO     + ZERO     ,
/*0xe3 : 0x80: 0x945  */ DEV_MASK + ZERO     + GJR_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe0 : 0x87: 0x946  */ DEV_MASK + ZERO     + ZERO     + ZERO     + ZERO     + KND_MASK + MLM_MASK + TML_MASK ,
/*0xe1 : 0xff: 0x947  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xe2 : 0xff: 0x948  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xe7 : 0x80: 0x949  */ DEV_MASK + ZERO     + GJR_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe4 : 0x87: 0x94a  */ DEV_MASK + ZERO     + ZERO     + ZERO     + ZERO     + KND_MASK + MLM_MASK + TML_MASK ,
/*0xe5 : 0xff: 0x94b  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xe6 : 0xff: 0x94c  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xe8 : 0xff: 0x94d  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xec : 0x00: 0x94e  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xed : 0x00: 0x94f  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x950  */ DEV_MASK + ZERO     + GJR_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x951  */ DEV_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x952  */ DEV_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x953  */ DEV_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x954  */ DEV_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x955  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + KND_MASK + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x956  */ ZERO     + ZERO     + ZERO     + ORI_MASK + ZERO     + KND_MASK + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x957  */ ZERO     + ZERO     + ZERO     + ORI_MASK + BNG_MASK + ZERO     + MLM_MASK + ZERO     ,
/*0x00 : 0x00: 0x958  */ DEV_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x959  */ DEV_MASK + PNJ_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95a  */ DEV_MASK + PNJ_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95b  */ DEV_MASK + PNJ_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95c  */ DEV_MASK + PNJ_MASK + ZERO     + ZERO     + BNG_MASK + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95d  */ DEV_MASK + ZERO     + ZERO     + ORI_MASK + BNG_MASK + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95e  */ DEV_MASK + PNJ_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xce : 0x98: 0x95f  */ DEV_MASK + ZERO     + ZERO     + ORI_MASK + BNG_MASK + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x960  */ DEV_MASK + ZERO     + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0x00 : 0x00: 0x961  */ DEV_MASK + ZERO     + ZERO     + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + ZERO     ,
/*0x00 : 0x00: 0x962  */ DEV_MASK + ZERO     + ZERO     + ZERO     + BNG_MASK + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x963  */ DEV_MASK + ZERO     + ZERO     + ZERO     + BNG_MASK + ZERO     + ZERO     + ZERO     ,
/*0xea : 0xf8: 0x964  */ DEV_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xeaea : 0x00: 0x965*/ DEV_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xf1 : 0xff: 0x966  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xf2 : 0xff: 0x967  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xf3 : 0xff: 0x968  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xf4 : 0xff: 0x969  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xf5 : 0xff: 0x96a  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xf6 : 0xff: 0x96b  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xf7 : 0xff: 0x96c  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xf8 : 0xff: 0x96d  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xf9 : 0xff: 0x96e  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0xfa : 0xff: 0x96f  */ DEV_MASK + PNJ_MASK + GJR_MASK + ORI_MASK + BNG_MASK + KND_MASK + MLM_MASK + TML_MASK ,
/*0x00 : 0x80: 0x970  */ DEV_MASK + PNJ_MASK + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*
 * The length of the array is 128 to provide values for 0x900..0x97f.
 * The last 15 entries for 0x971..0x97f of the validity table are all zero
 * because no Indic script uses such Unicode code points.
 */
/*0x00 : 0x00: 0x9yz  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO
};

நிலையான மாறிலி uint16_t fromUnicodeTable[128]={
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
நிலையான மாறிலி uint16_t toUnicodeTable[256]={
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

நிலையான மாறிலி uint16_t vowelSignESpecialCases[][2]={
	{ 2 /*length of array*/    , 0      },
	{ 0xA4 , 0x0904 },
};

நிலையான மாறிலி uint16_t nuktaSpecialCases[][2]={
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


#define WRITE_TO_TARGET_FROM_U(args,offsets,source,target,targetLimit,targetByteUnit,err) UPRV_BLOCK_MACRO_BEGIN { \
    int32_t offset = (int32_t)(source - args->source-1);                                        \
      /* write the targetUniChar  to target */                                                  \
    எனில்(target < targetLimit){                                                                   \
        எனில்(targetByteUnit <= 0xFF){                                                             \
            *(target)++ = (uint8_t)(targetByteUnit);                                            \
            எனில்(offsets){                                                                        \
                *(offsets++) = offset;                                                          \
            }                                                                                   \
        }இல்லையெனில்{                                                                                  \
            எனில் (targetByteUnit > 0xFFFF) {                                                      \
                *(target)++ = (uint8_t)(targetByteUnit>>16);                                    \
                எனில் (offsets) {                                                                  \
                    --offset;                                                                   \
                    *(offsets++) = offset;                                                      \
                }                                                                               \
            }                                                                                   \
            எனில் (!(target < targetLimit)) {                                                      \
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] =    \
                                (uint8_t)(targetByteUnit >> 8);                                 \
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] =    \
                                (uint8_t)targetByteUnit;                                        \
                *err = U_BUFFER_OVERFLOW_ERROR;                                                 \
            } இல்லையெனில் {                                                                            \
                *(target)++ = (uint8_t)(targetByteUnit>>8);                                     \
                எனில்(offsets){                                                                    \
                    *(offsets++) = offset;                                                      \
                }                                                                               \
                எனில்(target < targetLimit){                                                       \
                    *(target)++ = (uint8_t)  targetByteUnit;                                    \
                    எனில்(offsets){                                                                \
                        *(offsets++) = offset                            ;                      \
                    }                                                                           \
                }இல்லையெனில்{                                                                          \
                    args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] =\
                                (uint8_t) (targetByteUnit);                                     \
                    *err = U_BUFFER_OVERFLOW_ERROR;                                             \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
    }இல்லையெனில்{                                                                                      \
        எனில் (targetByteUnit & 0xFF0000) {                                                        \
            args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] =        \
                        (uint8_t) (targetByteUnit >>16);                                        \
        }                                                                                       \
        எனில்(targetByteUnit & 0xFF00){                                                            \
            args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] =        \
                        (uint8_t) (targetByteUnit >>8);                                         \
        }                                                                                       \
        args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] =            \
                        (uint8_t) (targetByteUnit);                                             \
        *err = U_BUFFER_OVERFLOW_ERROR;                                                         \
    }                                                                                           \
} UPRV_BLOCK_MACRO_END

/* Rules:
 *    Explicit Halant :
 *                      <HALANT> + <ZWNJ>
 *    Soft Halant :
 *                      <HALANT> + <ZWJ>
 */
நிலையான வெறுமை U_CALLCONV
UConverter_fromUnicode_ISCII_OFFSETS_LOGIC(
        UConverterFromUnicodeArgs * args, UErrorCode * err) {
    மாறிலி char16_t *source = args->source;
    மாறிலி char16_t *sourceLimit = args->sourceLimit;
    unsigned எழுத்து *target = (unsigned எழுத்து *) args->target;
    unsigned எழுத்து *targetLimit = (unsigned எழுத்து *) args->targetLimit;
    int32_t* offsets = args->offsets;
    uint32_t targetByteUnit = 0x0000;
    UChar32 sourceChar = 0x0000;
    UChar32 tempContextFromUnicode = 0x0000;    /* For special handling of the Gurmukhi script. */
    UConverterDataISCII *converterData;
    uint16_t newDelta=0;
    uint16_t range = 0;
    UBool deltaChanged = பொய்;

    எனில் ((args->converter == nullptr) || (args->targetLimit < args->target) || (args->sourceLimit < args->source)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    /* initialize data */
    converterData=(UConverterDataISCII*)args->converter->extraInfo;
    newDelta=converterData->currentDeltaFromUnicode;
    range = (uint16_t)(newDelta/DELTA);

    எனில் ((sourceChar = args->converter->fromUChar32)!=0) {
        செல் getTrail;
    }

    /*writing the char to the output stream */
    போது (source < sourceLimit) {
        /* Write the language code following LF only if LF is not the last character. */
        எனில் (args->converter->fromUnicodeStatus == LF) {
            targetByteUnit = ATR<<8;
            targetByteUnit += (uint8_t) lookupInitialData[range].isciiLang;
            args->converter->fromUnicodeStatus = 0x0000;
            /* now append ATR and language code */
            WRITE_TO_TARGET_FROM_U(args,offsets,source,target,targetLimit,targetByteUnit,err);
            எனில் (U_FAILURE(*err)) {
                நிறுத்து;
            }
        }
        
        sourceChar = *source++;
        tempContextFromUnicode = converterData->contextCharFromUnicode;
        
        targetByteUnit = missingCharMarker;
        
        /*check if input is in ASCII and C0 control codes range*/
        எனில் (sourceChar <= ASCII_END) {
            args->converter->fromUnicodeStatus = sourceChar;
            WRITE_TO_TARGET_FROM_U(args,offsets,source,target,targetLimit,sourceChar,err);
            எனில் (U_FAILURE(*err)) {
                நிறுத்து;
            }
            தொடர்;
        }
        தேர்வு (sourceChar) {
        நிலை ZWNJ:
            /* contextChar has HALANT */
            எனில் (converterData->contextCharFromUnicode) {
                converterData->contextCharFromUnicode = 0x00;
                targetByteUnit = ISCII_HALANT;
            } இல்லையெனில் {
                /* consume ZWNJ and continue */
                converterData->contextCharFromUnicode = 0x00;
                தொடர்;
            }
            நிறுத்து;
        நிலை ZWJ:
            /* contextChar has HALANT */
            எனில் (converterData->contextCharFromUnicode) {
                targetByteUnit = ISCII_NUKTA;
            } இல்லையெனில் {
                targetByteUnit =ISCII_INV;
            }
            converterData->contextCharFromUnicode = 0x00;
            நிறுத்து;
        இயல்பு:
            /* is the sourceChar in the INDIC_RANGE? */
            எனில் ((uint16_t)(INDIC_BLOCK_END-sourceChar) <= INDIC_RANGE) {
                /* Danda and Double Danda are valid in Northern scripts.. since Unicode
                 * does not include these codepoints in all Northern scrips we need to
                 * filter them out
                 */
                எனில் (sourceChar!= DANDA && sourceChar != DOUBLE_DANDA) {
                    /* find out to which block the souceChar belongs*/
                    range =(uint16_t)((sourceChar-INDIC_BLOCK_BEGIN)/DELTA);
                    newDelta =(uint16_t)(range*DELTA);

                    /* Now are we in the same block as the previous? */
                    எனில் (newDelta!= converterData->currentDeltaFromUnicode || converterData->isFirstBuffer) {
                        converterData->currentDeltaFromUnicode = newDelta;
                        converterData->currentMaskFromUnicode = lookupInitialData[range].maskEnum;
                        deltaChanged =உண்மை;
                        converterData->isFirstBuffer=பொய்;
                    }
                    
                    எனில் (converterData->currentDeltaFromUnicode == PNJ_DELTA) { 
                        எனில் (sourceChar == PNJ_TIPPI) {
                            /* Make sure Tippi is converted to Bindi. */
                            sourceChar = PNJ_BINDI;
                        } இல்லையெனில் எனில் (sourceChar == PNJ_ADHAK) {
                            /* This is for consonant cluster handling. */
                            converterData->contextCharFromUnicode = PNJ_ADHAK;
                        }
                        
                    }
                    /* Normalize all Indic codepoints to Devanagari and map them to ISCII */
                    /* now subtract the new delta from sourceChar*/
                    sourceChar -= converterData->currentDeltaFromUnicode;
                }

                /* get the target byte unit */
                targetByteUnit=fromUnicodeTable[(uint8_t)sourceChar];

                /* is the code point valid in current script? */
                எனில் ((validityTable[(uint8_t)sourceChar] & converterData->currentMaskFromUnicode)==0) {
                    /* Vocallic RR is assigned in ISCII Telugu and Unicode */
                    எனில் (converterData->currentDeltaFromUnicode!=(TELUGU_DELTA) || sourceChar!=VOCALLIC_RR) {
                        targetByteUnit=missingCharMarker;
                    }
                }

                எனில் (deltaChanged) {
                    /* we are in a script block which is different than
                     * previous sourceChar's script block write ATR and language codes
                     */
                    uint32_t temp=0;
                    temp =(uint16_t)(ATR<<8);
                    temp += (uint16_t)((uint8_t) lookupInitialData[range].isciiLang);
                    /* reset */
                    deltaChanged=பொய்;
                    /* now append ATR and language code */
                    WRITE_TO_TARGET_FROM_U(args,offsets,source,target,targetLimit,temp,err);
                    எனில் (U_FAILURE(*err)) {
                        நிறுத்து;
                    }
                }
                
                எனில் (converterData->currentDeltaFromUnicode == PNJ_DELTA && (sourceChar + PNJ_DELTA) == PNJ_ADHAK) {
                    தொடர்;
                }
            }
            /* reset context char */
            converterData->contextCharFromUnicode = 0x00;
            நிறுத்து;
        }
        எனில் (converterData->currentDeltaFromUnicode == PNJ_DELTA && tempContextFromUnicode == PNJ_ADHAK && isPNJConsonant((sourceChar + PNJ_DELTA))) {
            /* If the previous codepoint is Adhak and the current codepoint is a consonant, the targetByteUnit should be C + Halant + C. */
            /* reset context char */
            converterData->contextCharFromUnicode = 0x0000;
            targetByteUnit = targetByteUnit << 16 | ISCII_HALANT << 8 | targetByteUnit;
            /* write targetByteUnit to target */
            WRITE_TO_TARGET_FROM_U(args, offsets, source, target, targetLimit, targetByteUnit,err);
            எனில் (U_FAILURE(*err)) {
                நிறுத்து;
            }
        } இல்லையெனில் எனில் (targetByteUnit != missingCharMarker) {
            எனில் (targetByteUnit==ISCII_HALANT) {
                converterData->contextCharFromUnicode = (char16_t)targetByteUnit;
            }
            /* write targetByteUnit to target*/
            WRITE_TO_TARGET_FROM_U(args,offsets,source,target,targetLimit,targetByteUnit,err);
            எனில் (U_FAILURE(*err)) {
                நிறுத்து;
            }
        } இல்லையெனில் {
            /* oops.. the code point is unassigned */
            /*check if the char is a First surrogate*/
            எனில் (U16_IS_SURROGATE(sourceChar)) {
                எனில் (U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    எனில் (source < sourceLimit) {
                        /* test the following code unit */
                        char16_t trail= (*source);
                        எனில் (U16_IS_TRAIL(trail)) {
                            source++;
                            sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                            *err =U_INVALID_CHAR_FOUND;
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } இல்லையெனில் {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_FOUND;
                        }
                    } இல்லையெனில் {
                        /* no more input */
                        *err = U_ZERO_ERROR;
                    }
                } இல்லையெனில் {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_FOUND;
                }
            } இல்லையெனில் {
                /* callback(unassigned) for a BMP code point */
                *err = U_INVALID_CHAR_FOUND;
            }

            args->converter->fromUChar32=sourceChar;
            நிறுத்து;
        }
    }/* end while(mySourceIndex<mySourceLength) */

    /*save the state and return */
    args->source = source;
    args->target = (எழுத்து*)target;
}

நிலையான மாறிலி uint16_t lookupTable[][2]={
    { ZERO,       ZERO     },     /*DEFAULT*/
    { ZERO,       ZERO     },     /*ROMAN*/
    { DEVANAGARI, DEV_MASK },
    { BENGALI,    BNG_MASK },
    { TAMIL,      TML_MASK },
    { TELUGU,     KND_MASK },
    { BENGALI,    BNG_MASK },
    { ORIYA,      ORI_MASK },
    { KANNADA,    KND_MASK },
    { MALAYALAM,  MLM_MASK },
    { GUJARATI,   GJR_MASK },
    { GURMUKHI,   PNJ_MASK }
};

#define WRITE_TO_TARGET_TO_U(args,source,target,offsets,offset,targetUniChar,delta, err) UPRV_BLOCK_MACRO_BEGIN { \
    /* add offset to current Indic Block */                                              \
    எனில்(targetUniChar>ASCII_END &&                                                        \
           targetUniChar != ZWJ &&                                                       \
           targetUniChar != ZWNJ &&                                                      \
           targetUniChar != DANDA &&                                                     \
           targetUniChar != DOUBLE_DANDA){                                               \
                                                                                         \
           targetUniChar+=(uint16_t)(delta);                                             \
    }                                                                                    \
    /* now write the targetUniChar */                                                    \
    எனில்(target<args->targetLimit){                                                        \
        *(target)++ = (char16_t)targetUniChar;                                              \
        எனில்(offsets){                                                                     \
            *(offsets)++ = (int32_t)(offset);                                            \
        }                                                                                \
    }இல்லையெனில்{                                                                               \
        args->converter->UCharErrorBuffer[args->converter->UCharErrorBufferLength++] =   \
            (char16_t)targetUniChar;                                                        \
        *err = U_BUFFER_OVERFLOW_ERROR;                                                  \
    }                                                                                    \
} UPRV_BLOCK_MACRO_END

#define GET_MAPPING(sourceChar,targetUniChar,data) UPRV_BLOCK_MACRO_BEGIN {              \
    targetUniChar = toUnicodeTable[(sourceChar)] ;                                       \
    /* is the code point valid in current script? */                                     \
    எனில்(sourceChar> ASCII_END &&                                                          \
            (validityTable[(targetUniChar & 0x7F)] & data->currentMaskToUnicode)==0){    \
        /* Vocallic RR is assigned in ISCII Telugu and Unicode */                         \
        எனில்(data->currentDeltaToUnicode!=(TELUGU_DELTA) ||                                \
                    targetUniChar!=VOCALLIC_RR){                                         \
            targetUniChar=missingCharMarker;                                             \
        }                                                                                \
    }                                                                                    \
} UPRV_BLOCK_MACRO_END

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

நிலையான வெறுமை U_CALLCONV
UConverter_toUnicode_ISCII_OFFSETS_LOGIC(UConverterToUnicodeArgs *args, UErrorCode* err) {
    மாறிலி எழுத்து *source = ( எழுத்து *) args->source;
    char16_t *target = args->target;
    மாறிலி எழுத்து *sourceLimit = args->sourceLimit;
    மாறிலி char16_t* targetLimit = args->targetLimit;
    uint32_t targetUniChar = 0x0000;
    uint8_t sourceChar = 0x0000;
    UConverterDataISCII* data;
    UChar32* toUnicodeStatus=nullptr;
    UChar32 tempTargetUniChar = 0x0000;
    char16_t* contextCharToUnicode= nullptr;
    UBool found;
    முழு i; 
    முழு offset = 0;

    எனில் ((args->converter == nullptr) || (target < args->target) || (source < args->source)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    data = (UConverterDataISCII*)(args->converter->extraInfo);
    contextCharToUnicode = &data->contextCharToUnicode; /* contains previous ISCII codepoint visited */
    toUnicodeStatus = (UChar32*)&args->converter->toUnicodeStatus;/* contains the mapping to Unicode of the above codepoint*/

    போது (U_SUCCESS(*err) && source<sourceLimit) {

        targetUniChar = missingCharMarker;

        எனில் (target < targetLimit) {
            sourceChar = (unsigned எழுத்து)*(source)++;

            /* look at the post-context perform special processing */
            எனில் (*contextCharToUnicode==ATR) {

                /* If we have ATR in *contextCharToUnicode then we need to change our
                 * state to the Indic Script specified by sourceChar
                 */

                /* check if the sourceChar is supported script range*/
                எனில் ((uint8_t)(PNJ-sourceChar)<=PNJ-DEV) {
                    data->currentDeltaToUnicode = (uint16_t)(lookupTable[sourceChar & 0x0F][0] * DELTA);
                    data->currentMaskToUnicode = (MaskEnum)lookupTable[sourceChar & 0x0F][1];
                } இல்லையெனில் எனில் (sourceChar==DEF) {
                    /* switch back to default */
                    data->currentDeltaToUnicode = data->defDeltaToUnicode;
                    data->currentMaskToUnicode = data->defMaskToUnicode;
                } இல்லையெனில் {
                    எனில் ((sourceChar >= 0x21 && sourceChar <= 0x3F)) {
                        /* these are display codes consume and continue */
                    } இல்லையெனில் {
                        *err =U_ILLEGAL_CHAR_FOUND;
                        /* reset */
                        *contextCharToUnicode=NO_CHAR_MARKER;
                        செல் CALLBACK;
                    }
                }

                /* reset */
                *contextCharToUnicode=NO_CHAR_MARKER;

                தொடர்;

            } இல்லையெனில் எனில் (*contextCharToUnicode==EXT) {
                /* check if sourceChar is in 0xA1-0xEE range */
                எனில் ((uint8_t) (EXT_RANGE_END - sourceChar) <= (EXT_RANGE_END - EXT_RANGE_BEGIN)) {
                    /* We currently support only Anudatta and Devanagari abbreviation sign */
                    எனில் (sourceChar==0xBF || sourceChar == 0xB8) {
                        targetUniChar = (sourceChar==0xBF) ? DEV_ABBR_SIGN : DEV_ANUDATTA;
                        
                        /* find out if the mapping is valid in this state */
                        எனில் (validityTable[(uint8_t)targetUniChar] & data->currentMaskToUnicode) {
                            *contextCharToUnicode= NO_CHAR_MARKER;

                            /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                            எனில் (data->prevToUnicodeStatus) {
                                WRITE_TO_TARGET_TO_U(args,source,target,args->offsets,(source-args->source -1),data->prevToUnicodeStatus,0,err);
                                data->prevToUnicodeStatus = 0x0000;
                            }
                            /* write to target */
                            WRITE_TO_TARGET_TO_U(args,source,target,args->offsets,(source-args->source -2),targetUniChar,data->currentDeltaToUnicode,err);

                            தொடர்;
                        }
                    }
                    /* byte unit is unassigned */
                    targetUniChar = missingCharMarker;
                    *err= U_INVALID_CHAR_FOUND;
                } இல்லையெனில் {
                    /* only 0xA1 - 0xEE are legal after EXT char */
                    *contextCharToUnicode= NO_CHAR_MARKER;
                    *err = U_ILLEGAL_CHAR_FOUND;
                }
                செல் CALLBACK;
            } இல்லையெனில் எனில் (*contextCharToUnicode==ISCII_INV) {
                எனில் (sourceChar==ISCII_HALANT) {
                    targetUniChar = 0x0020; /* replace with space according to Indic FAQ */
                } இல்லையெனில் {
                    targetUniChar = ZWJ;
                }

                /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                எனில் (data->prevToUnicodeStatus) {
                    WRITE_TO_TARGET_TO_U(args,source,target,args->offsets,(source-args->source -1),data->prevToUnicodeStatus,0,err);
                    data->prevToUnicodeStatus = 0x0000;
                }
                /* write to target */
                WRITE_TO_TARGET_TO_U(args,source,target,args->offsets,(source-args->source -2),targetUniChar,data->currentDeltaToUnicode,err);
                /* reset */
                *contextCharToUnicode=NO_CHAR_MARKER;
            }

            /* look at the pre-context and perform special processing */
            தேர்வு (sourceChar) {
            நிலை ISCII_INV:
            நிலை EXT:
            நிலை ATR:
                *contextCharToUnicode = (char16_t)sourceChar;

                எனில் (*toUnicodeStatus != missingCharMarker) {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    எனில் (data->prevToUnicodeStatus) {
                        WRITE_TO_TARGET_TO_U(args,source,target,args->offsets,(source-args->source -1),data->prevToUnicodeStatus,0,err);
                        data->prevToUnicodeStatus = 0x0000;
                    }
                    WRITE_TO_TARGET_TO_U(args,source,target,args->offsets,(source-args->source -2),*toUnicodeStatus,data->currentDeltaToUnicode,err);
                    *toUnicodeStatus = missingCharMarker;
                }
                தொடர்;
            நிலை ISCII_DANDA:
                /* handle double danda*/
                எனில் (*contextCharToUnicode== ISCII_DANDA) {
                    targetUniChar = DOUBLE_DANDA;
                    /* clear the context */
                    *contextCharToUnicode = NO_CHAR_MARKER;
                    *toUnicodeStatus = missingCharMarker;
                } இல்லையெனில் {
                    GET_MAPPING(sourceChar,targetUniChar,data);
                    *contextCharToUnicode = sourceChar;
                }
                நிறுத்து;
            நிலை ISCII_HALANT:
                /* handle explicit halant */
                எனில் (*contextCharToUnicode == ISCII_HALANT) {
                    targetUniChar = ZWNJ;
                    /* clear the context */
                    *contextCharToUnicode = NO_CHAR_MARKER;
                } இல்லையெனில் {
                    GET_MAPPING(sourceChar,targetUniChar,data);
                    *contextCharToUnicode = sourceChar;
                }
                நிறுத்து;
            நிலை 0x0A:
            நிலை 0x0D:
                data->resetToDefaultToUnicode = உண்மை;
                GET_MAPPING(sourceChar,targetUniChar,data)
                ;
                *contextCharToUnicode = sourceChar;
                நிறுத்து;

            நிலை ISCII_VOWEL_SIGN_E:
                i=1;
                found=பொய்;
                க்கு (; i<vowelSignESpecialCases[0][0]; i++) {
                    U_ASSERT(i<UPRV_LENGTHOF(vowelSignESpecialCases));
                    எனில் (vowelSignESpecialCases[i][0]==(uint8_t)*contextCharToUnicode) {
                        targetUniChar=vowelSignESpecialCases[i][1];
                        found=உண்மை;
                        நிறுத்து;
                    }
                }
                எனில் (found) {
                    /* find out if the mapping is valid in this state */
                    எனில் (validityTable[(uint8_t)targetUniChar] & data->currentMaskToUnicode) {
                        /*targetUniChar += data->currentDeltaToUnicode ;*/
                        *contextCharToUnicode= NO_CHAR_MARKER;
                        *toUnicodeStatus = missingCharMarker;
                        நிறுத்து;
                    }
                }
                GET_MAPPING(sourceChar,targetUniChar,data);
                *contextCharToUnicode = sourceChar;
                நிறுத்து;

            நிலை ISCII_NUKTA:
                /* handle soft halant */
                எனில் (*contextCharToUnicode == ISCII_HALANT) {
                    targetUniChar = ZWJ;
                    /* clear the context */
                    *contextCharToUnicode = NO_CHAR_MARKER;
                    நிறுத்து;
                } இல்லையெனில் எனில் (data->currentDeltaToUnicode == PNJ_DELTA && data->contextCharToUnicode == 0xc0) {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    எனில் (data->prevToUnicodeStatus) {
                        WRITE_TO_TARGET_TO_U(args,source,target,args->offsets,(source-args->source -1),data->prevToUnicodeStatus,0,err);
                        data->prevToUnicodeStatus = 0x0000;
                    }
                    /* We got here because ISCII_NUKTA was preceded by 0xc0 and we are converting Gurmukhi.
                     * In that case we must convert (0xc0 0xe9) to (\u0a5c\u0a4d\u0a39).
                     */
                    targetUniChar = PNJ_RRA;
                    WRITE_TO_TARGET_TO_U(args, source, target, args->offsets, (source-args->source)-2, targetUniChar, 0, err);
                    எனில் (U_SUCCESS(*err)) {
                        targetUniChar = PNJ_SIGN_VIRAMA;
                        WRITE_TO_TARGET_TO_U(args, source, target, args->offsets, (source-args->source)-2, targetUniChar, 0, err);
                        எனில் (U_SUCCESS(*err)) {
                            targetUniChar = PNJ_HA;
                            WRITE_TO_TARGET_TO_U(args, source, target, args->offsets, (source-args->source)-2, targetUniChar, 0, err);
                        } இல்லையெனில் {
                            args->converter->UCharErrorBuffer[args->converter->UCharErrorBufferLength++]= PNJ_HA;
                        }
                    } இல்லையெனில் {
                        args->converter->UCharErrorBuffer[args->converter->UCharErrorBufferLength++]= PNJ_SIGN_VIRAMA;
                        args->converter->UCharErrorBuffer[args->converter->UCharErrorBufferLength++]= PNJ_HA;
                    }
                    *toUnicodeStatus = missingCharMarker;
                    data->contextCharToUnicode = NO_CHAR_MARKER;
                    தொடர்;
                } இல்லையெனில் {
                    /* try to handle <CHAR> + ISCII_NUKTA special mappings */
                    i=1;
                    found =பொய்;
                    க்கு (; i<nuktaSpecialCases[0][0]; i++) {
                        எனில் (nuktaSpecialCases[i][0]==(uint8_t)
                                *contextCharToUnicode) {
                            targetUniChar=nuktaSpecialCases[i][1];
                            found =உண்மை;
                            நிறுத்து;
                        }
                    }
                    எனில் (found) {
                        /* find out if the mapping is valid in this state */
                        எனில் (validityTable[(uint8_t)targetUniChar] & data->currentMaskToUnicode) {
                            /*targetUniChar += data->currentDeltaToUnicode ;*/
                            *contextCharToUnicode= NO_CHAR_MARKER;
                            *toUnicodeStatus = missingCharMarker;
                            எனில் (data->currentDeltaToUnicode == PNJ_DELTA) {
                                /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                                எனில் (data->prevToUnicodeStatus) {
                                    WRITE_TO_TARGET_TO_U(args,source,target,args->offsets,(source-args->source -1),data->prevToUnicodeStatus,0,err);
                                    data->prevToUnicodeStatus = 0x0000;
                                }
                                WRITE_TO_TARGET_TO_U(args,source,target,args->offsets,(source-args->source -2),targetUniChar,data->currentDeltaToUnicode,err);
                                தொடர்;
                            }
                            நிறுத்து;
                        }
                        /* else fall through to default */
                    }
                    /* else fall through to default */
                    U_FALLTHROUGH;
                }
            இயல்பு:GET_MAPPING(sourceChar,targetUniChar,data)
                ;
                *contextCharToUnicode = sourceChar;
                நிறுத்து;
            }

            எனில் (*toUnicodeStatus != missingCharMarker) {
                /* Check to make sure that consonant clusters are handled correct for Gurmukhi script. */
                எனில் (data->currentDeltaToUnicode == PNJ_DELTA && data->prevToUnicodeStatus != 0 && isPNJConsonant(data->prevToUnicodeStatus) &&
                        (*toUnicodeStatus + PNJ_DELTA) == PNJ_SIGN_VIRAMA && ((UChar32)(targetUniChar + PNJ_DELTA) == data->prevToUnicodeStatus)) {
                    /* Consonant clusters C + HALANT + C should be encoded as ADHAK + C */
                    offset = (முழு)(source-args->source - 3);
                    tempTargetUniChar = PNJ_ADHAK; /* This is necessary to avoid some compiler warnings. */
                    WRITE_TO_TARGET_TO_U(args,source,target,args->offsets,offset,tempTargetUniChar,0,err);
                    WRITE_TO_TARGET_TO_U(args,source,target,args->offsets,offset,data->prevToUnicodeStatus,0,err);
                    data->prevToUnicodeStatus = 0x0000; /* reset the previous unicode code point */
                    *toUnicodeStatus = missingCharMarker;
                    தொடர்;
                } இல்லையெனில் {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    எனில் (data->prevToUnicodeStatus) {
                        WRITE_TO_TARGET_TO_U(args,source,target,args->offsets,(source-args->source -1),data->prevToUnicodeStatus,0,err);
                        data->prevToUnicodeStatus = 0x0000;
                    }
                    /* Check to make sure that Bindi and Tippi are handled correctly for Gurmukhi script. 
                     * If 0xA2 is preceded by a codepoint in the PNJ_BINDI_TIPPI_SET then the target codepoint should be Tippi instead of Bindi.
                     */
                    எனில் (data->currentDeltaToUnicode == PNJ_DELTA && (targetUniChar + PNJ_DELTA) == PNJ_BINDI && isPNJBindiTippi((*toUnicodeStatus + PNJ_DELTA))) {
                        targetUniChar = PNJ_TIPPI - PNJ_DELTA;
                        WRITE_TO_TARGET_TO_U(args,source,target,args->offsets,(source-args->source -2),*toUnicodeStatus,PNJ_DELTA,err);
                    } இல்லையெனில் எனில் (data->currentDeltaToUnicode == PNJ_DELTA && (targetUniChar + PNJ_DELTA) == PNJ_SIGN_VIRAMA && isPNJConsonant((*toUnicodeStatus + PNJ_DELTA))) {
                        /* Store the current toUnicodeStatus code point for later handling of consonant cluster in Gurmukhi. */
                        data->prevToUnicodeStatus = *toUnicodeStatus + PNJ_DELTA;
                    } இல்லையெனில் {
                        /* write the previously mapped codepoint */
                        WRITE_TO_TARGET_TO_U(args,source,target,args->offsets,(source-args->source -2),*toUnicodeStatus,data->currentDeltaToUnicode,err);
                    }
                }
                *toUnicodeStatus = missingCharMarker;
            }

            எனில் (targetUniChar != missingCharMarker) {
                /* now save the targetUniChar for delayed write */
                *toUnicodeStatus = (char16_t) targetUniChar;
                எனில் (data->resetToDefaultToUnicode) {
                    data->currentDeltaToUnicode = data->defDeltaToUnicode;
                    data->currentMaskToUnicode = data->defMaskToUnicode;
                    data->resetToDefaultToUnicode=பொய்;
                }
            } இல்லையெனில் {

                /* we reach here only if targetUniChar == missingCharMarker
                 * so assign codes to reason and err
                 */
                *err = U_INVALID_CHAR_FOUND;
CALLBACK:
                args->converter->toUBytes[0] = sourceChar;
                args->converter->toULength = 1;
                நிறுத்து;
            }

        } இல்லையெனில் {
            *err =U_BUFFER_OVERFLOW_ERROR;
            நிறுத்து;
        }
    }

    எனில் (U_SUCCESS(*err) && args->flush && source == sourceLimit) {
        /* end of the input stream */
        UConverter *cnv = args->converter;

        எனில் (*contextCharToUnicode==ATR || *contextCharToUnicode==EXT || *contextCharToUnicode==ISCII_INV) {
            /* set toUBytes[] */
            cnv->toUBytes[0] = (uint8_t)*contextCharToUnicode;
            cnv->toULength = 1;

            /* avoid looping on truncated sequences */
            *contextCharToUnicode = NO_CHAR_MARKER;
        } இல்லையெனில் {
            cnv->toULength = 0;
        }

        எனில் (*toUnicodeStatus != missingCharMarker) {
            /* output a remaining target character */
            WRITE_TO_TARGET_TO_U(args,source,target,args->offsets,(source - args->source -1),*toUnicodeStatus,data->currentDeltaToUnicode,err);
            *toUnicodeStatus = missingCharMarker;
        }
    }

    args->target = target;
    args->source = source;
}

/* structure for SafeClone calculations */
கட்டமைப்பு cloneISCIIStruct {
    UConverter cnv;
    UConverterDataISCII mydata;
};

நிலையான UConverter * U_CALLCONV
_ISCII_SafeClone(மாறிலி UConverter *cnv,
              வெறுமை *அடுக்கு_Buffer,
              int32_t *pBufferSize,
              UErrorCode *status)
{
    கட்டமைப்பு cloneISCIIStruct * localClone;
    int32_t bufferSizeNeeded = அளவு(கட்டமைப்பு cloneISCIIStruct);

    எனில் (U_FAILURE(*status)) {
        திரும்பு nullptr;
    }

    எனில் (*pBufferSize == 0) { /* 'preflighting' request - set needed size into *pBufferSize */
        *pBufferSize = bufferSizeNeeded;
        திரும்பு nullptr;
    }

    localClone = (கட்டமைப்பு cloneISCIIStruct *)அடுக்கு_Buffer;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, அளவு(UConverterDataISCII));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = உண்மை;

    திரும்பு &localClone->cnv;
}

நிலையான வெறுமை U_CALLCONV
_ISCIIGetUnicodeSet(மாறிலி UConverter *cnv,
                    மாறிலி USetAdder *sa,
                    UConverterUnicodeSet which,
                    UErrorCode *pErrorCode)
{
    (வெறுமை)cnv;
    (வெறுமை)which;
    (வெறுமை)pErrorCode;
    int32_t idx, script;
    uint8_t mask;

    /* Since all ISCII versions allow switching to other ISCII
    scripts, we add all roundtrippable characters to this set. */
    sa->addRange(sa->set, 0, ASCII_END);
    க்கு (script = DEVANAGARI; script <= MALAYALAM; script++) {
        mask = (uint8_t)(lookupInitialData[script].maskEnum);
        க்கு (idx = 0; idx < DELTA; idx++) {
            /* added check for TELUGU character */
            எனில் ((validityTable[idx] & mask) || (script==TELUGU && idx==0x31)) {
                sa->add(sa->set, idx + (script * DELTA) + INDIC_BLOCK_BEGIN);
            }
        }
    }
    sa->add(sa->set, DANDA);
    sa->add(sa->set, DOUBLE_DANDA);
    sa->add(sa->set, ZWNJ);
    sa->add(sa->set, ZWJ);
}
U_CDECL_END
நிலையான மாறிலி UConverterImpl _ISCIIImpl={

    UCNV_ISCII,

    nullptr,
    nullptr,

    _ISCIIOpen,
    _ISCIIClose,
    _ISCIIReset,

    UConverter_toUnicode_ISCII_OFFSETS_LOGIC,
    UConverter_toUnicode_ISCII_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISCII_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISCII_OFFSETS_LOGIC,
    nullptr,

    nullptr,
    _ISCIIgetName,
    nullptr,
    _ISCII_SafeClone,
    _ISCIIGetUnicodeSet,
    nullptr,
    nullptr
};

நிலையான மாறிலி UConverterStaticData _ISCIIStaticData={
    அளவு(UConverterStaticData),
        "ISCII",
         0,
         UCNV_IBM,
         UCNV_ISCII,
         1,
         4,
        { 0x1a, 0, 0, 0 },
        0x1,
        பொய்,
        பொய்,
        0x0,
        0x0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

மாறிலி UConverterSharedData _ISCIIData=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_ISCIIStaticData, &_ISCIIImpl);

#endif /* #if !UCONFIG_NO_LEGACY_CONVERSION */
