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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv_cb.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_OPTIONS_ಆವೃತ್ತಿ_ಮುಖವಾಡ 0xf
#ವ್ಯಾಖ್ಯಾನಿಸು NUKTA               0x093c
#ವ್ಯಾಖ್ಯಾನಿಸು HALANT              0x094d
#ವ್ಯಾಖ್ಯಾನಿಸು ZWNJ                0x200c /* Zero Width Non Joiner */
#ವ್ಯಾಖ್ಯಾನಿಸು ZWJ                 0x200d /* Zero width Joiner */
#ವ್ಯಾಖ್ಯಾನಿಸು INVALID_CHAR        0xffff
#ವ್ಯಾಖ್ಯಾನಿಸು ATR                 0xEF   /* Attribute code */
#ವ್ಯಾಖ್ಯಾನಿಸು EXT                 0xF0   /* Extension code */
#ವ್ಯಾಖ್ಯಾನಿಸು DANDA               0x0964
#ವ್ಯಾಖ್ಯಾನಿಸು DOUBLE_DANDA        0x0965
#ವ್ಯಾಖ್ಯಾನಿಸು ISCII_NUKTA         0xE9
#ವ್ಯಾಖ್ಯಾನಿಸು ISCII_HALANT        0xE8
#ವ್ಯಾಖ್ಯಾನಿಸು ISCII_DANDA         0xEA
#ವ್ಯಾಖ್ಯಾನಿಸು ISCII_INV           0xD9
#ವ್ಯಾಖ್ಯಾನಿಸು ISCII_VOWEL_SIGN_E  0xE0
#ವ್ಯಾಖ್ಯಾನಿಸು INDIC_ಖಂಡ_BEGIN   0x0900
#ವ್ಯಾಖ್ಯಾನಿಸು INDIC_ಖಂಡ_ಕೊನೆ     0x0D7F
#ವ್ಯಾಖ್ಯಾನಿಸು INDIC_RANGE         (INDIC_ಖಂಡ_ಕೊನೆ - INDIC_ಖಂಡ_BEGIN)
#ವ್ಯಾಖ್ಯಾನಿಸು VOCALLIC_RR         0x0931
#ವ್ಯಾಖ್ಯಾನಿಸು LF                  0x0A
#ವ್ಯಾಖ್ಯಾನಿಸು ASCII_ಕೊನೆ           0xA0
#ವ್ಯಾಖ್ಯಾನಿಸು NO_CHAR_MARKER      0xFFFE
#ವ್ಯಾಖ್ಯಾನಿಸು TELUGU_DELTA        DELTA * TELUGU
#ವ್ಯಾಖ್ಯಾನಿಸು DEV_ABBR_SIGN       0x0970
#ವ್ಯಾಖ್ಯಾನಿಸು DEV_ANUDATTA        0x0952
#ವ್ಯಾಖ್ಯಾನಿಸು EXT_RANGE_BEGIN     0xA1
#ವ್ಯಾಖ್ಯಾನಿಸು EXT_RANGE_ಕೊನೆ       0xEE

#ವ್ಯಾಖ್ಯಾನಿಸು PNJ_DELTA           0x0100
#ವ್ಯಾಖ್ಯಾನಿಸು PNJ_BINDI           0x0A02
#ವ್ಯಾಖ್ಯಾನಿಸು PNJ_TIPPI           0x0A70
#ವ್ಯಾಖ್ಯಾನಿಸು PNJ_SIGN_VIRAMA     0x0A4D
#ವ್ಯಾಖ್ಯಾನಿಸು PNJ_ADHAK           0x0A71
#ವ್ಯಾಖ್ಯಾನಿಸು PNJ_HA              0x0A39
#ವ್ಯಾಖ್ಯಾನಿಸು PNJ_RRA             0x0A5C

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    DEV_ಮುಖವಾಡ =0x80,
    PNJ_ಮುಖವಾಡ =0x40,
    GJR_ಮುಖವಾಡ =0x20,
    ORI_ಮುಖವಾಡ =0x10,
    BNG_ಮುಖವಾಡ =0x08,
    KND_ಮುಖವಾಡ =0x04,
    MLM_ಮುಖವಾಡ =0x02,
    TML_ಮುಖವಾಡ =0x01,
    ZERO =0x00
}ಮುಖವಾಡ_Enum;

#ವ್ಯಾಖ್ಯಾನಿಸು ISCII_CNV_ಪೂರ್ವಪ್ರತ್ಯಯ "ISCII,version="

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char16_t context_Char_ಗಮ್ಯ_Unicode;         /* previous Unicode codepoint for contextual analysis */
    char16_t contextCharFromUnicode;       /* previous Unicode codepoint for contextual analysis */
    uint16_t def_Delta_ಗಮ್ಯ_Unicode;         /* delta for switching to default state when DEF is encountered  */
    uint16_t currentDeltaFromUnicode;   /* current delta in Indic block */
    uint16_t current_Delta_ಗಮ್ಯ_Unicode;     /* current delta in Indic block */
    ಮುಖವಾಡ_Enum current_ಮುಖವಾಡ_From_Unicode;    /* mask for current state in toUnicode */
    ಮುಖವಾಡ_Enum current_ಮುಖವಾಡ_ಗಮ್ಯ_Unicode;      /* mask for current state in toUnicode */
    ಮುಖವಾಡ_Enum def_ಮುಖವಾಡ_ಗಮ್ಯ_Unicode;          /* mask for default state in toUnicode */
    UBool is_ಮೊದಲ_ಬಫರ್;                /* boolean for fromUnicode to see if we need to announce the first script */
    UBool reset_ಗಮ್ಯ_Default_ಗಮ್ಯ_Unicode;      /* boolean for resetting to default delta and mask when a newline is encountered*/
    ಅಕ್ಷರ name[ಗಾತ್ರ(ISCII_CNV_ಪೂರ್ವಪ್ರತ್ಯಯ) + 1];
    UChar32 prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ;        /* Hold the previous toUnicodeStatus. This is necessary because we may need to know the last two code points. */
} UConverter_ದತ್ತಾಂಶ_ISCII;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Lookup_ದತ್ತಾಂಶ_Struct {
    UniLang uniLang;
    ಮುಖವಾಡ_Enum ಮುಖವಾಡ_Enum_2;
    ISCIILang isciiLang;
} Lookup_ದತ್ತಾಂಶ_Struct;

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Lookup_ದತ್ತಾಂಶ_Struct lookup_Initial_ದತ್ತಾಂಶ[]={
    { DEVANAGARI, DEV_ಮುಖವಾಡ,  DEV },
    { BENGALI,    BNG_ಮುಖವಾಡ,  BNG },
    { GURMUKHI,   PNJ_ಮುಖವಾಡ,  PNJ },
    { GUJARATI,   GJR_ಮುಖವಾಡ,  GJR },
    { ORIYA,      ORI_ಮುಖವಾಡ,  ORI },
    { TAMIL,      TML_ಮುಖವಾಡ,  TML },
    { TELUGU,     KND_ಮುಖವಾಡ,  TLG },
    { KANNADA,    KND_ಮುಖವಾಡ,  KND },
    { MALAYALAM,  MLM_ಮುಖವಾಡ,  MLM }
};

/*
 * For special handling of certain Gurmukhi characters.
 * Bit 0 (value 1): PNJ consonant
 * Bit 1 (value 2): PNJ Bindi Tippi
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t pnjMap[80] = {
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

ಸ್ಥಿರ UBool
isPNJConsonant(UChar32 c) {
    ಆದರೆ (c < 0xa00 || 0xa50 <= c) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ pnjMap[c - 0xa00] & 1;
    }
}

ಸ್ಥಿರ UBool
isPNJBindiTippi(UChar32 c) {
    ಆದರೆ (c < 0xa00 || 0xa50 <= c) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ pnjMap[c - 0xa00] >> 1;
    }
}
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_ISCIIOpen(UConverter *cnv, UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು, UError_ಸಂಕೇತ *error_ಸಂಕೇತ) {
    ಆದರೆ(p_ವಾದಗಳು->onlyTestIsLoadable) {
        ಹಿಂತಿರುಗಿ;
    }

    cnv->extraInfo = uprv_malloc(ಗಾತ್ರ(UConverter_ದತ್ತಾಂಶ_ISCII));

    ಆದರೆ (cnv->extraInfo != ಶೂನ್ಯಸೂಚಕ) {
        int32_t len=0;
        UConverter_ದತ್ತಾಂಶ_ISCII *converter_ದತ್ತಾಂಶ=
                (UConverter_ದತ್ತಾಂಶ_ISCII *) cnv->extraInfo;
        converter_ದತ್ತಾಂಶ->context_Char_ಗಮ್ಯ_Unicode=NO_CHAR_MARKER;
        cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = missingCharMarker;
        converter_ದತ್ತಾಂಶ->contextCharFromUnicode=0x0000;
        converter_ದತ್ತಾಂಶ->reset_ಗಮ್ಯ_Default_ಗಮ್ಯ_Unicode=ಸುಳ್ಳು;
        /* check if the version requested is supported */
        ಆದರೆ ((p_ವಾದಗಳು->options & UCNV_OPTIONS_ಆವೃತ್ತಿ_ಮುಖವಾಡ) < 9) {
            /* initialize state variables */
            converter_ದತ್ತಾಂಶ->currentDeltaFromUnicode
                    = converter_ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode
                            = converter_ದತ್ತಾಂಶ->def_Delta_ಗಮ್ಯ_Unicode = (uint16_t)(lookup_Initial_ದತ್ತಾಂಶ[p_ವಾದಗಳು->options & UCNV_OPTIONS_ಆವೃತ್ತಿ_ಮುಖವಾಡ].uniLang * DELTA);

            converter_ದತ್ತಾಂಶ->current_ಮುಖವಾಡ_From_Unicode
                    = converter_ದತ್ತಾಂಶ->current_ಮುಖವಾಡ_ಗಮ್ಯ_Unicode
                            = converter_ದತ್ತಾಂಶ->def_ಮುಖವಾಡ_ಗಮ್ಯ_Unicode = lookup_Initial_ದತ್ತಾಂಶ[p_ವಾದಗಳು->options & UCNV_OPTIONS_ಆವೃತ್ತಿ_ಮುಖವಾಡ].ಮುಖವಾಡ_Enum_2;
            
            converter_ದತ್ತಾಂಶ->is_ಮೊದಲ_ಬಫರ್=ನಿಜ;
            (ಶೂನ್ಯ)uprv_strcpy(converter_ದತ್ತಾಂಶ->name, ISCII_CNV_ಪೂರ್ವಪ್ರತ್ಯಯ);
            len = (int32_t)uprv_strlen(converter_ದತ್ತಾಂಶ->name);
            converter_ದತ್ತಾಂಶ->name[len]= (ಅಕ್ಷರ)((p_ವಾದಗಳು->options & UCNV_OPTIONS_ಆವೃತ್ತಿ_ಮುಖವಾಡ) + '0');
            converter_ದತ್ತಾಂಶ->name[len+1]=0;
            
            converter_ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0x0000;
        } ಇಲ್ಲದಿದ್ದರೆ {
            uprv_free(cnv->extraInfo);
            cnv->extraInfo = ಶೂನ್ಯಸೂಚಕ;
            *error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        }

    } ಇಲ್ಲದಿದ್ದರೆ {
        *error_ಸಂಕೇತ =U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_ISCIIClose(UConverter *cnv) {
    ಆದರೆ (cnv->extraInfo!=ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo=ಶೂನ್ಯಸೂಚಕ;
    }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*  U_CALLCONV
_ISCIIgetName(ಸ್ಥಿರಾಂಕ UConverter* cnv) {
    ಆದರೆ (cnv->extraInfo) {
        UConverter_ದತ್ತಾಂಶ_ISCII* my_ದತ್ತಾಂಶ= (UConverter_ದತ್ತಾಂಶ_ISCII*)cnv->extraInfo;
        ಹಿಂತಿರುಗಿ my_ದತ್ತಾಂಶ->name;
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_ISCIIReset(UConverter *cnv, UConverterResetChoice choice) {
    UConverter_ದತ್ತಾಂಶ_ISCII* ದತ್ತಾಂಶ =(UConverter_ದತ್ತಾಂಶ_ISCII *) (cnv->extraInfo);
    ಆದರೆ (choice<=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = missingCharMarker;
        cnv->ಬಹುಲಕ=0;
        ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode=ದತ್ತಾಂಶ->def_Delta_ಗಮ್ಯ_Unicode;
        ದತ್ತಾಂಶ->current_ಮುಖವಾಡ_ಗಮ್ಯ_Unicode = ದತ್ತಾಂಶ->def_ಮುಖವಾಡ_ಗಮ್ಯ_Unicode;
        ದತ್ತಾಂಶ->context_Char_ಗಮ್ಯ_Unicode=NO_CHAR_MARKER;
        ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0x0000;
    }
    ಆದರೆ (choice!=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        cnv->fromUChar32=0x0000;
        ದತ್ತಾಂಶ->contextCharFromUnicode=0x00;
        ದತ್ತಾಂಶ->current_ಮುಖವಾಡ_From_Unicode=ದತ್ತಾಂಶ->def_ಮುಖವಾಡ_ಗಮ್ಯ_Unicode;
        ದತ್ತಾಂಶ->currentDeltaFromUnicode=ದತ್ತಾಂಶ->def_Delta_ಗಮ್ಯ_Unicode;
        ದತ್ತಾಂಶ->is_ಮೊದಲ_ಬಫರ್=ನಿಜ;
        ದತ್ತಾಂಶ->reset_ಗಮ್ಯ_Default_ಗಮ್ಯ_Unicode=ಸುಳ್ಳು;
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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t validity_ಕೋಷ್ಟಕ[128] = {
/* This state table is tool generated please do not edit unless you know exactly what you are doing */
/* Note: This table was edited to mirror the Windows XP implementation */
/*ISCII:Valid:Unicode */
/*0xa0 : 0x00: 0x900  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xa1 : 0xb8: 0x901  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     ,
/*0xa2 : 0xfe: 0x902  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xa3 : 0xbf: 0x903  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0x00 : 0x00: 0x904  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xa4 : 0xff: 0x905  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xa5 : 0xff: 0x906  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xa6 : 0xff: 0x907  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xa7 : 0xff: 0x908  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xa8 : 0xff: 0x909  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xa9 : 0xff: 0x90a  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xaa : 0xfe: 0x90b  */ DEV_ಮುಖವಾಡ + ZERO     + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0x00 : 0x00: 0x90c  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0xae : 0x80: 0x90d  */ DEV_ಮುಖವಾಡ + ZERO     + GJR_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xab : 0x87: 0x90e  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xac : 0xff: 0x90f  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xad : 0xff: 0x910  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xb2 : 0x80: 0x911  */ DEV_ಮುಖವಾಡ + ZERO     + GJR_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xaf : 0x87: 0x912  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xb0 : 0xff: 0x913  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xb1 : 0xff: 0x914  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xb3 : 0xff: 0x915  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xb4 : 0xfe: 0x916  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0xb5 : 0xfe: 0x917  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0xb6 : 0xfe: 0x918  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0xb7 : 0xff: 0x919  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xb8 : 0xff: 0x91a  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xb9 : 0xfe: 0x91b  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0xba : 0xff: 0x91c  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xbb : 0xfe: 0x91d  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0xbc : 0xff: 0x91e  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xbd : 0xff: 0x91f  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xbe : 0xfe: 0x920  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0xbf : 0xfe: 0x921  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0xc0 : 0xfe: 0x922  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0xc1 : 0xff: 0x923  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xc2 : 0xff: 0x924  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xc3 : 0xfe: 0x925  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0xc4 : 0xfe: 0x926  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0xc5 : 0xfe: 0x927  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0xc6 : 0xff: 0x928  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xc7 : 0x81: 0x929  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + TML_ಮುಖವಾಡ ,
/*0xc8 : 0xff: 0x92a  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xc9 : 0xfe: 0x92b  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0xca : 0xfe: 0x92c  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0xcb : 0xfe: 0x92d  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0xcc : 0xfe: 0x92e  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xcd : 0xff: 0x92f  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xcf : 0xff: 0x930  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xd0 : 0x87: 0x931  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xd1 : 0xff: 0x932  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xd2 : 0xb7: 0x933  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + ZERO     + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xd3 : 0x83: 0x934  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xd4 : 0xff: 0x935  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + ZERO     + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xd5 : 0xfe: 0x936  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0xd6 : 0xbf: 0x937  */ DEV_ಮುಖವಾಡ + ZERO     + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xd7 : 0xff: 0x938  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xd8 : 0xff: 0x939  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0x00 : 0x00: 0x93A  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x93B  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe9 : 0xda: 0x93c  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + ZERO     + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x93d  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xda : 0xff: 0x93e  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xdb : 0xff: 0x93f  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xdc : 0xff: 0x940  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xdd : 0xff: 0x941  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xde : 0xff: 0x942  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xdf : 0xbe: 0x943  */ DEV_ಮುಖವಾಡ + ZERO     + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0x00 : 0x00: 0x944  */ DEV_ಮುಖವಾಡ + ZERO     + GJR_ಮುಖವಾಡ + ZERO     + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + ZERO     + ZERO     ,
/*0xe3 : 0x80: 0x945  */ DEV_ಮುಖವಾಡ + ZERO     + GJR_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe0 : 0x87: 0x946  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xe1 : 0xff: 0x947  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xe2 : 0xff: 0x948  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xe7 : 0x80: 0x949  */ DEV_ಮುಖವಾಡ + ZERO     + GJR_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe4 : 0x87: 0x94a  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xe5 : 0xff: 0x94b  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xe6 : 0xff: 0x94c  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xe8 : 0xff: 0x94d  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xec : 0x00: 0x94e  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xed : 0x00: 0x94f  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x950  */ DEV_ಮುಖವಾಡ + ZERO     + GJR_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x951  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x952  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x953  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x954  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x955  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + KND_ಮುಖವಾಡ + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x956  */ ZERO     + ZERO     + ZERO     + ORI_ಮುಖವಾಡ + ZERO     + KND_ಮುಖವಾಡ + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x957  */ ZERO     + ZERO     + ZERO     + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + ZERO     + MLM_ಮುಖವಾಡ + ZERO     ,
/*0x00 : 0x00: 0x958  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x959  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95a  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95b  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95c  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + ZERO     + ZERO     + BNG_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95d  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95e  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xce : 0x98: 0x95f  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x960  */ DEV_ಮುಖವಾಡ + ZERO     + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0x00 : 0x00: 0x961  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + ZERO     ,
/*0x00 : 0x00: 0x962  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + BNG_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x963  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + BNG_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     ,
/*0xea : 0xf8: 0x964  */ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xeaea : 0x00: 0x965*/ DEV_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xf1 : 0xff: 0x966  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xf2 : 0xff: 0x967  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xf3 : 0xff: 0x968  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xf4 : 0xff: 0x969  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xf5 : 0xff: 0x96a  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xf6 : 0xff: 0x96b  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xf7 : 0xff: 0x96c  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xf8 : 0xff: 0x96d  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xf9 : 0xff: 0x96e  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0xfa : 0xff: 0x96f  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + GJR_ಮುಖವಾಡ + ORI_ಮುಖವಾಡ + BNG_ಮುಖವಾಡ + KND_ಮುಖವಾಡ + MLM_ಮುಖವಾಡ + TML_ಮುಖವಾಡ ,
/*0x00 : 0x80: 0x970  */ DEV_ಮುಖವಾಡ + PNJ_ಮುಖವಾಡ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*
 * The length of the array is 128 to provide values for 0x900..0x97f.
 * The last 15 entries for 0x971..0x97f of the validity table are all zero
 * because no Indic script uses such Unicode code points.
 */
/*0x00 : 0x00: 0x9yz  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint16_t from_Unicode_ಕೋಷ್ಟಕ[128]={
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
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint16_t ಗಮ್ಯ_Unicode_ಕೋಷ್ಟಕ[256]={
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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint16_t vowelSignESpecialCases[][2]={
	{ 2 /*length of array*/    , 0      },
	{ 0xA4 , 0x0904 },
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint16_t nuktaSpecialCases[][2]={
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


#ವ್ಯಾಖ್ಯಾನಿಸು WRITE_ಗಮ್ಯ_ಗುರಿ_FROM_U(ವಾದಗಳು_2,offsets,ಮೂಲ,ಗುರಿ,ಗುರಿ_ಎಲ್ಲೆ,ಗುರಿ_Byte_Unit,err) UPRV_ಖಂಡ_MACRO_BEGIN { \
    int32_t ಸ್ಥಾನಾಂತರ = (int32_t)(ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ-1);                                        \
      /* write the targetUniChar  to target */                                                  \
    ಆದರೆ(ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ){                                                                   \
        ಆದರೆ(ಗುರಿ_Byte_Unit <= 0xFF){                                                             \
            *(ಗುರಿ)++ = (uint8_t)(ಗುರಿ_Byte_Unit);                                            \
            ಆದರೆ(offsets){                                                                        \
                *(offsets++) = ಸ್ಥಾನಾಂತರ;                                                          \
            }                                                                                   \
        }ಇಲ್ಲದಿದ್ದರೆ{                                                                                  \
            ಆದರೆ (ಗುರಿ_Byte_Unit > 0xFFFF) {                                                      \
                *(ಗುರಿ)++ = (uint8_t)(ಗುರಿ_Byte_Unit>>16);                                    \
                ಆದರೆ (offsets) {                                                                  \
                    --ಸ್ಥಾನಾಂತರ;                                                                   \
                    *(offsets++) = ಸ್ಥಾನಾಂತರ;                                                      \
                }                                                                               \
            }                                                                                   \
            ಆದರೆ (!(ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)) {                                                      \
                ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] =    \
                                (uint8_t)(ಗುರಿ_Byte_Unit >> 8);                                 \
                ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] =    \
                                (uint8_t)ಗುರಿ_Byte_Unit;                                        \
                *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;                                                 \
            } ಇಲ್ಲದಿದ್ದರೆ {                                                                            \
                *(ಗುರಿ)++ = (uint8_t)(ಗುರಿ_Byte_Unit>>8);                                     \
                ಆದರೆ(offsets){                                                                    \
                    *(offsets++) = ಸ್ಥಾನಾಂತರ;                                                      \
                }                                                                               \
                ಆದರೆ(ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ){                                                       \
                    *(ಗುರಿ)++ = (uint8_t)  ಗುರಿ_Byte_Unit;                                    \
                    ಆದರೆ(offsets){                                                                \
                        *(offsets++) = ಸ್ಥಾನಾಂತರ                            ;                      \
                    }                                                                           \
                }ಇಲ್ಲದಿದ್ದರೆ{                                                                          \
                    ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] =\
                                (uint8_t) (ಗುರಿ_Byte_Unit);                                     \
                    *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;                                             \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
    }ಇಲ್ಲದಿದ್ದರೆ{                                                                                      \
        ಆದರೆ (ಗುರಿ_Byte_Unit & 0xFF0000) {                                                        \
            ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] =        \
                        (uint8_t) (ಗುರಿ_Byte_Unit >>16);                                        \
        }                                                                                       \
        ಆದರೆ(ಗುರಿ_Byte_Unit & 0xFF00){                                                            \
            ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] =        \
                        (uint8_t) (ಗುರಿ_Byte_Unit >>8);                                         \
        }                                                                                       \
        ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] =            \
                        (uint8_t) (ಗುರಿ_Byte_Unit);                                             \
        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;                                                         \
    }                                                                                           \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ

/* Rules:
 *    Explicit Halant :
 *                      <HALANT> + <ZWNJ>
 *    Soft Halant :
 *                      <HALANT> + <ZWJ>
 */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
UConverter_fromUnicode_ISCII_OFFSETS_LOGIC(
        UConverter_From_Unicode_ವಾದಗಳು * ವಾದಗಳು_2, UError_ಸಂಕೇತ * err) {
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ = ವಾದಗಳು_2->ಮೂಲ;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಗುರಿ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಗುರಿ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಗುರಿ_ಎಲ್ಲೆ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ;
    int32_t* offsets = ವಾದಗಳು_2->offsets;
    uint32_t ಗುರಿ_Byte_Unit = 0x0000;
    UChar32 ಮೂಲ_Char = 0x0000;
    UChar32 ತಾತ್ಕಾಲಿಕ_Context_From_Unicode = 0x0000;    /* For special handling of the Gurmukhi script. */
    UConverter_ದತ್ತಾಂಶ_ISCII *converter_ದತ್ತಾಂಶ;
    uint16_t newDelta=0;
    uint16_t range = 0;
    UBool delta_ಬದಲಾಯಿತು = ಸುಳ್ಳು;

    ಆದರೆ ((ವಾದಗಳು_2->converter == ಶೂನ್ಯಸೂಚಕ) || (ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ < ವಾದಗಳು_2->ಗುರಿ) || (ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ < ವಾದಗಳು_2->ಮೂಲ)) {
        *err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    /* initialize data */
    converter_ದತ್ತಾಂಶ=(UConverter_ದತ್ತಾಂಶ_ISCII*)ವಾದಗಳು_2->converter->extraInfo;
    newDelta=converter_ದತ್ತಾಂಶ->currentDeltaFromUnicode;
    range = (uint16_t)(newDelta/DELTA);

    ಆದರೆ ((ಮೂಲ_Char = ವಾದಗಳು_2->converter->fromUChar32)!=0) {
        ಹೋಗು getTrail;
    }

    /*writing the char to the output stream */
    ಆಗ (ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ) {
        /* Write the language code following LF only if LF is not the last character. */
        ಆದರೆ (ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ == LF) {
            ಗುರಿ_Byte_Unit = ATR<<8;
            ಗುರಿ_Byte_Unit += (uint8_t) lookup_Initial_ದತ್ತಾಂಶ[range].isciiLang;
            ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ = 0x0000;
            /* now append ATR and language code */
            WRITE_ಗಮ್ಯ_ಗುರಿ_FROM_U(ವಾದಗಳು_2,offsets,ಮೂಲ,ಗುರಿ,ಗುರಿ_ಎಲ್ಲೆ,ಗುರಿ_Byte_Unit,err);
            ಆದರೆ (U_FAILURE(*err)) {
                ನಿಲ್ಲಿಸು;
            }
        }
        
        ಮೂಲ_Char = *ಮೂಲ++;
        ತಾತ್ಕಾಲಿಕ_Context_From_Unicode = converter_ದತ್ತಾಂಶ->contextCharFromUnicode;
        
        ಗುರಿ_Byte_Unit = missingCharMarker;
        
        /*check if input is in ASCII and C0 control codes range*/
        ಆದರೆ (ಮೂಲ_Char <= ASCII_ಕೊನೆ) {
            ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ = ಮೂಲ_Char;
            WRITE_ಗಮ್ಯ_ಗುರಿ_FROM_U(ವಾದಗಳು_2,offsets,ಮೂಲ,ಗುರಿ,ಗುರಿ_ಎಲ್ಲೆ,ಮೂಲ_Char,err);
            ಆದರೆ (U_FAILURE(*err)) {
                ನಿಲ್ಲಿಸು;
            }
            ಮುಂದುವರಿ;
        }
        ಆಯ್ಕೆ (ಮೂಲ_Char) {
        ಸ್ಥಿತಿ ZWNJ:
            /* contextChar has HALANT */
            ಆದರೆ (converter_ದತ್ತಾಂಶ->contextCharFromUnicode) {
                converter_ದತ್ತಾಂಶ->contextCharFromUnicode = 0x00;
                ಗುರಿ_Byte_Unit = ISCII_HALANT;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* consume ZWNJ and continue */
                converter_ದತ್ತಾಂಶ->contextCharFromUnicode = 0x00;
                ಮುಂದುವರಿ;
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ ZWJ:
            /* contextChar has HALANT */
            ಆದರೆ (converter_ದತ್ತಾಂಶ->contextCharFromUnicode) {
                ಗುರಿ_Byte_Unit = ISCII_NUKTA;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಗುರಿ_Byte_Unit =ISCII_INV;
            }
            converter_ದತ್ತಾಂಶ->contextCharFromUnicode = 0x00;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            /* is the sourceChar in the INDIC_RANGE? */
            ಆದರೆ ((uint16_t)(INDIC_ಖಂಡ_ಕೊನೆ-ಮೂಲ_Char) <= INDIC_RANGE) {
                /* Danda and Double Danda are valid in Northern scripts.. since Unicode
                 * does not include these codepoints in all Northern scrips we need to
                 * filter them out
                 */
                ಆದರೆ (ಮೂಲ_Char!= DANDA && ಮೂಲ_Char != DOUBLE_DANDA) {
                    /* find out to which block the souceChar belongs*/
                    range =(uint16_t)((ಮೂಲ_Char-INDIC_ಖಂಡ_BEGIN)/DELTA);
                    newDelta =(uint16_t)(range*DELTA);

                    /* Now are we in the same block as the previous? */
                    ಆದರೆ (newDelta!= converter_ದತ್ತಾಂಶ->currentDeltaFromUnicode || converter_ದತ್ತಾಂಶ->is_ಮೊದಲ_ಬಫರ್) {
                        converter_ದತ್ತಾಂಶ->currentDeltaFromUnicode = newDelta;
                        converter_ದತ್ತಾಂಶ->current_ಮುಖವಾಡ_From_Unicode = lookup_Initial_ದತ್ತಾಂಶ[range].ಮುಖವಾಡ_Enum_2;
                        delta_ಬದಲಾಯಿತು =ನಿಜ;
                        converter_ದತ್ತಾಂಶ->is_ಮೊದಲ_ಬಫರ್=ಸುಳ್ಳು;
                    }
                    
                    ಆದರೆ (converter_ದತ್ತಾಂಶ->currentDeltaFromUnicode == PNJ_DELTA) { 
                        ಆದರೆ (ಮೂಲ_Char == PNJ_TIPPI) {
                            /* Make sure Tippi is converted to Bindi. */
                            ಮೂಲ_Char = PNJ_BINDI;
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೂಲ_Char == PNJ_ADHAK) {
                            /* This is for consonant cluster handling. */
                            converter_ದತ್ತಾಂಶ->contextCharFromUnicode = PNJ_ADHAK;
                        }
                        
                    }
                    /* Normalize all Indic codepoints to Devanagari and map them to ISCII */
                    /* now subtract the new delta from sourceChar*/
                    ಮೂಲ_Char -= converter_ದತ್ತಾಂಶ->currentDeltaFromUnicode;
                }

                /* get the target byte unit */
                ಗುರಿ_Byte_Unit=from_Unicode_ಕೋಷ್ಟಕ[(uint8_t)ಮೂಲ_Char];

                /* is the code point valid in current script? */
                ಆದರೆ ((validity_ಕೋಷ್ಟಕ[(uint8_t)ಮೂಲ_Char] & converter_ದತ್ತಾಂಶ->current_ಮುಖವಾಡ_From_Unicode)==0) {
                    /* Vocallic RR is assigned in ISCII Telugu and Unicode */
                    ಆದರೆ (converter_ದತ್ತಾಂಶ->currentDeltaFromUnicode!=(TELUGU_DELTA) || ಮೂಲ_Char!=VOCALLIC_RR) {
                        ಗುರಿ_Byte_Unit=missingCharMarker;
                    }
                }

                ಆದರೆ (delta_ಬದಲಾಯಿತು) {
                    /* we are in a script block which is different than
                     * previous sourceChar's script block write ATR and language codes
                     */
                    uint32_t ತಾತ್ಕಾಲಿಕ=0;
                    ತಾತ್ಕಾಲಿಕ =(uint16_t)(ATR<<8);
                    ತಾತ್ಕಾಲಿಕ += (uint16_t)((uint8_t) lookup_Initial_ದತ್ತಾಂಶ[range].isciiLang);
                    /* reset */
                    delta_ಬದಲಾಯಿತು=ಸುಳ್ಳು;
                    /* now append ATR and language code */
                    WRITE_ಗಮ್ಯ_ಗುರಿ_FROM_U(ವಾದಗಳು_2,offsets,ಮೂಲ,ಗುರಿ,ಗುರಿ_ಎಲ್ಲೆ,ತಾತ್ಕಾಲಿಕ,err);
                    ಆದರೆ (U_FAILURE(*err)) {
                        ನಿಲ್ಲಿಸು;
                    }
                }
                
                ಆದರೆ (converter_ದತ್ತಾಂಶ->currentDeltaFromUnicode == PNJ_DELTA && (ಮೂಲ_Char + PNJ_DELTA) == PNJ_ADHAK) {
                    ಮುಂದುವರಿ;
                }
            }
            /* reset context char */
            converter_ದತ್ತಾಂಶ->contextCharFromUnicode = 0x00;
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ (converter_ದತ್ತಾಂಶ->currentDeltaFromUnicode == PNJ_DELTA && ತಾತ್ಕಾಲಿಕ_Context_From_Unicode == PNJ_ADHAK && isPNJConsonant((ಮೂಲ_Char + PNJ_DELTA))) {
            /* If the previous codepoint is Adhak and the current codepoint is a consonant, the targetByteUnit should be C + Halant + C. */
            /* reset context char */
            converter_ದತ್ತಾಂಶ->contextCharFromUnicode = 0x0000;
            ಗುರಿ_Byte_Unit = ಗುರಿ_Byte_Unit << 16 | ISCII_HALANT << 8 | ಗುರಿ_Byte_Unit;
            /* write targetByteUnit to target */
            WRITE_ಗಮ್ಯ_ಗುರಿ_FROM_U(ವಾದಗಳು_2, offsets, ಮೂಲ, ಗುರಿ, ಗುರಿ_ಎಲ್ಲೆ, ಗುರಿ_Byte_Unit,err);
            ಆದರೆ (U_FAILURE(*err)) {
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಗುರಿ_Byte_Unit != missingCharMarker) {
            ಆದರೆ (ಗುರಿ_Byte_Unit==ISCII_HALANT) {
                converter_ದತ್ತಾಂಶ->contextCharFromUnicode = (char16_t)ಗುರಿ_Byte_Unit;
            }
            /* write targetByteUnit to target*/
            WRITE_ಗಮ್ಯ_ಗುರಿ_FROM_U(ವಾದಗಳು_2,offsets,ಮೂಲ,ಗುರಿ,ಗುರಿ_ಎಲ್ಲೆ,ಗುರಿ_Byte_Unit,err);
            ಆದರೆ (U_FAILURE(*err)) {
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* oops.. the code point is unassigned */
            /*check if the char is a First surrogate*/
            ಆದರೆ (U16_IS_SURROGATE(ಮೂಲ_Char)) {
                ಆದರೆ (U16_IS_SURROGATE_LEAD(ಮೂಲ_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    ಆದರೆ (ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ) {
                        /* test the following code unit */
                        char16_t trail= (*ಮೂಲ);
                        ಆದರೆ (U16_IS_TRAIL(trail)) {
                            ಮೂಲ++;
                            ಮೂಲ_Char=U16_GET_SUPPLEMENTARY(ಮೂಲ_Char, trail);
                            *err =U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* no more input */
                        *err = U_ZERO_ERROR;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* callback(unassigned) for a BMP code point */
                *err = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
            }

            ವಾದಗಳು_2->converter->fromUChar32=ಮೂಲ_Char;
            ನಿಲ್ಲಿಸು;
        }
    }/* end while(mySourceIndex<mySourceLength) */

    /*save the state and return */
    ವಾದಗಳು_2->ಮೂಲ = ಮೂಲ;
    ವಾದಗಳು_2->ಗುರಿ = (ಅಕ್ಷರ*)ಗುರಿ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint16_t lookup_ಕೋಷ್ಟಕ[][2]={
    { ZERO,       ZERO     },     /*DEFAULT*/
    { ZERO,       ZERO     },     /*ROMAN*/
    { DEVANAGARI, DEV_ಮುಖವಾಡ },
    { BENGALI,    BNG_ಮುಖವಾಡ },
    { TAMIL,      TML_ಮುಖವಾಡ },
    { TELUGU,     KND_ಮುಖವಾಡ },
    { BENGALI,    BNG_ಮುಖವಾಡ },
    { ORIYA,      ORI_ಮುಖವಾಡ },
    { KANNADA,    KND_ಮುಖವಾಡ },
    { MALAYALAM,  MLM_ಮುಖವಾಡ },
    { GUJARATI,   GJR_ಮುಖವಾಡ },
    { GURMUKHI,   PNJ_ಮುಖವಾಡ }
};

#ವ್ಯಾಖ್ಯಾನಿಸು WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2,ಮೂಲ,ಗುರಿ,offsets,ಸ್ಥಾನಾಂತರ,ಗುರಿ_Uni_Char,delta, err) UPRV_ಖಂಡ_MACRO_BEGIN { \
    /* add offset to current Indic Block */                                              \
    ಆದರೆ(ಗುರಿ_Uni_Char>ASCII_ಕೊನೆ &&                                                        \
           ಗುರಿ_Uni_Char != ZWJ &&                                                       \
           ಗುರಿ_Uni_Char != ZWNJ &&                                                      \
           ಗುರಿ_Uni_Char != DANDA &&                                                     \
           ಗುರಿ_Uni_Char != DOUBLE_DANDA){                                               \
                                                                                         \
           ಗುರಿ_Uni_Char+=(uint16_t)(delta);                                             \
    }                                                                                    \
    /* now write the targetUniChar */                                                    \
    ಆದರೆ(ಗುರಿ<ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ){                                                        \
        *(ಗುರಿ)++ = (char16_t)ಗುರಿ_Uni_Char;                                              \
        ಆದರೆ(offsets){                                                                     \
            *(offsets)++ = (int32_t)(ಸ್ಥಾನಾಂತರ);                                            \
        }                                                                                \
    }ಇಲ್ಲದಿದ್ದರೆ{                                                                               \
        ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್[ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ++] =   \
            (char16_t)ಗುರಿ_Uni_Char;                                                        \
        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;                                                  \
    }                                                                                    \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ

#ವ್ಯಾಖ್ಯಾನಿಸು GET_MAPPING(ಮೂಲ_Char,ಗುರಿ_Uni_Char,ದತ್ತಾಂಶ) UPRV_ಖಂಡ_MACRO_BEGIN {              \
    ಗುರಿ_Uni_Char = ಗಮ್ಯ_Unicode_ಕೋಷ್ಟಕ[(ಮೂಲ_Char)] ;                                       \
    /* is the code point valid in current script? */                                     \
    ಆದರೆ(ಮೂಲ_Char> ASCII_ಕೊನೆ &&                                                          \
            (validity_ಕೋಷ್ಟಕ[(ಗುರಿ_Uni_Char & 0x7F)] & ದತ್ತಾಂಶ->current_ಮುಖವಾಡ_ಗಮ್ಯ_Unicode)==0){    \
        /* Vocallic RR is assigned in ISCII Telugu and Unicode */                         \
        ಆದರೆ(ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode!=(TELUGU_DELTA) ||                                \
                    ಗುರಿ_Uni_Char!=VOCALLIC_RR){                                         \
            ಗುರಿ_Uni_Char=missingCharMarker;                                             \
        }                                                                                \
    }                                                                                    \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ

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

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
UConverter_ಗಮ್ಯ_Unicode_ISCII_OFFSETS_LOGIC(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *ವಾದಗಳು_2, UError_ಸಂಕೇತ* err) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ = ( ಅಕ್ಷರ *) ವಾದಗಳು_2->ಮೂಲ;
    char16_t *ಗುರಿ = ವಾದಗಳು_2->ಗುರಿ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    ಸ್ಥಿರಾಂಕ char16_t* ಗುರಿ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ;
    uint32_t ಗುರಿ_Uni_Char = 0x0000;
    uint8_t ಮೂಲ_Char = 0x0000;
    UConverter_ದತ್ತಾಂಶ_ISCII* ದತ್ತಾಂಶ;
    UChar32* ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=ಶೂನ್ಯಸೂಚಕ;
    UChar32 ತಾತ್ಕಾಲಿಕ_ಗುರಿ_Uni_Char = 0x0000;
    char16_t* context_Char_ಗಮ್ಯ_Unicode= ಶೂನ್ಯಸೂಚಕ;
    UBool ಸಿಕ್ಕಿತು;
    ಪೂರ್ಣಾಂಕ i; 
    ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0;

    ಆದರೆ ((ವಾದಗಳು_2->converter == ಶೂನ್ಯಸೂಚಕ) || (ಗುರಿ < ವಾದಗಳು_2->ಗುರಿ) || (ಮೂಲ < ವಾದಗಳು_2->ಮೂಲ)) {
        *err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ದತ್ತಾಂಶ = (UConverter_ದತ್ತಾಂಶ_ISCII*)(ವಾದಗಳು_2->converter->extraInfo);
    context_Char_ಗಮ್ಯ_Unicode = &ದತ್ತಾಂಶ->context_Char_ಗಮ್ಯ_Unicode; /* contains previous ISCII codepoint visited */
    ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = (UChar32*)&ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ;/* contains the mapping to Unicode of the above codepoint*/

    ಆಗ (U_SUCCESS(*err) && ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {

        ಗುರಿ_Uni_Char = missingCharMarker;

        ಆದರೆ (ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ) {
            ಮೂಲ_Char = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*(ಮೂಲ)++;

            /* look at the post-context perform special processing */
            ಆದರೆ (*context_Char_ಗಮ್ಯ_Unicode==ATR) {

                /* If we have ATR in *contextCharToUnicode then we need to change our
                 * state to the Indic Script specified by sourceChar
                 */

                /* check if the sourceChar is supported script range*/
                ಆದರೆ ((uint8_t)(PNJ-ಮೂಲ_Char)<=PNJ-DEV) {
                    ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode = (uint16_t)(lookup_ಕೋಷ್ಟಕ[ಮೂಲ_Char & 0x0F][0] * DELTA);
                    ದತ್ತಾಂಶ->current_ಮುಖವಾಡ_ಗಮ್ಯ_Unicode = (ಮುಖವಾಡ_Enum)lookup_ಕೋಷ್ಟಕ[ಮೂಲ_Char & 0x0F][1];
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೂಲ_Char==DEF) {
                    /* switch back to default */
                    ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode = ದತ್ತಾಂಶ->def_Delta_ಗಮ್ಯ_Unicode;
                    ದತ್ತಾಂಶ->current_ಮುಖವಾಡ_ಗಮ್ಯ_Unicode = ದತ್ತಾಂಶ->def_ಮುಖವಾಡ_ಗಮ್ಯ_Unicode;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಆದರೆ ((ಮೂಲ_Char >= 0x21 && ಮೂಲ_Char <= 0x3F)) {
                        /* these are display codes consume and continue */
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        *err =U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        /* reset */
                        *context_Char_ಗಮ್ಯ_Unicode=NO_CHAR_MARKER;
                        ಹೋಗು CALLBACK;
                    }
                }

                /* reset */
                *context_Char_ಗಮ್ಯ_Unicode=NO_CHAR_MARKER;

                ಮುಂದುವರಿ;

            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*context_Char_ಗಮ್ಯ_Unicode==EXT) {
                /* check if sourceChar is in 0xA1-0xEE range */
                ಆದರೆ ((uint8_t) (EXT_RANGE_ಕೊನೆ - ಮೂಲ_Char) <= (EXT_RANGE_ಕೊನೆ - EXT_RANGE_BEGIN)) {
                    /* We currently support only Anudatta and Devanagari abbreviation sign */
                    ಆದರೆ (ಮೂಲ_Char==0xBF || ಮೂಲ_Char == 0xB8) {
                        ಗುರಿ_Uni_Char = (ಮೂಲ_Char==0xBF) ? DEV_ABBR_SIGN : DEV_ANUDATTA;
                        
                        /* find out if the mapping is valid in this state */
                        ಆದರೆ (validity_ಕೋಷ್ಟಕ[(uint8_t)ಗುರಿ_Uni_Char] & ದತ್ತಾಂಶ->current_ಮುಖವಾಡ_ಗಮ್ಯ_Unicode) {
                            *context_Char_ಗಮ್ಯ_Unicode= NO_CHAR_MARKER;

                            /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                            ಆದರೆ (ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ) {
                                WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2,ಮೂಲ,ಗುರಿ,ವಾದಗಳು_2->offsets,(ಮೂಲ-ವಾದಗಳು_2->ಮೂಲ -1),ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ,0,err);
                                ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0x0000;
                            }
                            /* write to target */
                            WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2,ಮೂಲ,ಗುರಿ,ವಾದಗಳು_2->offsets,(ಮೂಲ-ವಾದಗಳು_2->ಮೂಲ -2),ಗುರಿ_Uni_Char,ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode,err);

                            ಮುಂದುವರಿ;
                        }
                    }
                    /* byte unit is unassigned */
                    ಗುರಿ_Uni_Char = missingCharMarker;
                    *err= U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* only 0xA1 - 0xEE are legal after EXT char */
                    *context_Char_ಗಮ್ಯ_Unicode= NO_CHAR_MARKER;
                    *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                }
                ಹೋಗು CALLBACK;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*context_Char_ಗಮ್ಯ_Unicode==ISCII_INV) {
                ಆದರೆ (ಮೂಲ_Char==ISCII_HALANT) {
                    ಗುರಿ_Uni_Char = 0x0020; /* replace with space according to Indic FAQ */
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಗುರಿ_Uni_Char = ZWJ;
                }

                /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                ಆದರೆ (ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ) {
                    WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2,ಮೂಲ,ಗುರಿ,ವಾದಗಳು_2->offsets,(ಮೂಲ-ವಾದಗಳು_2->ಮೂಲ -1),ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ,0,err);
                    ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0x0000;
                }
                /* write to target */
                WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2,ಮೂಲ,ಗುರಿ,ವಾದಗಳು_2->offsets,(ಮೂಲ-ವಾದಗಳು_2->ಮೂಲ -2),ಗುರಿ_Uni_Char,ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode,err);
                /* reset */
                *context_Char_ಗಮ್ಯ_Unicode=NO_CHAR_MARKER;
            }

            /* look at the pre-context and perform special processing */
            ಆಯ್ಕೆ (ಮೂಲ_Char) {
            ಸ್ಥಿತಿ ISCII_INV:
            ಸ್ಥಿತಿ EXT:
            ಸ್ಥಿತಿ ATR:
                *context_Char_ಗಮ್ಯ_Unicode = (char16_t)ಮೂಲ_Char;

                ಆದರೆ (*ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ != missingCharMarker) {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    ಆದರೆ (ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ) {
                        WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2,ಮೂಲ,ಗುರಿ,ವಾದಗಳು_2->offsets,(ಮೂಲ-ವಾದಗಳು_2->ಮೂಲ -1),ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ,0,err);
                        ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0x0000;
                    }
                    WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2,ಮೂಲ,ಗುರಿ,ವಾದಗಳು_2->offsets,(ಮೂಲ-ವಾದಗಳು_2->ಮೂಲ -2),*ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ,ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode,err);
                    *ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = missingCharMarker;
                }
                ಮುಂದುವರಿ;
            ಸ್ಥಿತಿ ISCII_DANDA:
                /* handle double danda*/
                ಆದರೆ (*context_Char_ಗಮ್ಯ_Unicode== ISCII_DANDA) {
                    ಗುರಿ_Uni_Char = DOUBLE_DANDA;
                    /* clear the context */
                    *context_Char_ಗಮ್ಯ_Unicode = NO_CHAR_MARKER;
                    *ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = missingCharMarker;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    GET_MAPPING(ಮೂಲ_Char,ಗುರಿ_Uni_Char,ದತ್ತಾಂಶ);
                    *context_Char_ಗಮ್ಯ_Unicode = ಮೂಲ_Char;
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ ISCII_HALANT:
                /* handle explicit halant */
                ಆದರೆ (*context_Char_ಗಮ್ಯ_Unicode == ISCII_HALANT) {
                    ಗುರಿ_Uni_Char = ZWNJ;
                    /* clear the context */
                    *context_Char_ಗಮ್ಯ_Unicode = NO_CHAR_MARKER;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    GET_MAPPING(ಮೂಲ_Char,ಗುರಿ_Uni_Char,ದತ್ತಾಂಶ);
                    *context_Char_ಗಮ್ಯ_Unicode = ಮೂಲ_Char;
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ 0x0A:
            ಸ್ಥಿತಿ 0x0D:
                ದತ್ತಾಂಶ->reset_ಗಮ್ಯ_Default_ಗಮ್ಯ_Unicode = ನಿಜ;
                GET_MAPPING(ಮೂಲ_Char,ಗುರಿ_Uni_Char,ದತ್ತಾಂಶ)
                ;
                *context_Char_ಗಮ್ಯ_Unicode = ಮೂಲ_Char;
                ನಿಲ್ಲಿಸು;

            ಸ್ಥಿತಿ ISCII_VOWEL_SIGN_E:
                i=1;
                ಸಿಕ್ಕಿತು=ಸುಳ್ಳು;
                ಗಾಗಿ (; i<vowelSignESpecialCases[0][0]; i++) {
                    U_ASSERT(i<UPRV_LENGTHOF(vowelSignESpecialCases));
                    ಆದರೆ (vowelSignESpecialCases[i][0]==(uint8_t)*context_Char_ಗಮ್ಯ_Unicode) {
                        ಗುರಿ_Uni_Char=vowelSignESpecialCases[i][1];
                        ಸಿಕ್ಕಿತು=ನಿಜ;
                        ನಿಲ್ಲಿಸು;
                    }
                }
                ಆದರೆ (ಸಿಕ್ಕಿತು) {
                    /* find out if the mapping is valid in this state */
                    ಆದರೆ (validity_ಕೋಷ್ಟಕ[(uint8_t)ಗುರಿ_Uni_Char] & ದತ್ತಾಂಶ->current_ಮುಖವಾಡ_ಗಮ್ಯ_Unicode) {
                        /*targetUniChar += data->currentDeltaToUnicode ;*/
                        *context_Char_ಗಮ್ಯ_Unicode= NO_CHAR_MARKER;
                        *ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = missingCharMarker;
                        ನಿಲ್ಲಿಸು;
                    }
                }
                GET_MAPPING(ಮೂಲ_Char,ಗುರಿ_Uni_Char,ದತ್ತಾಂಶ);
                *context_Char_ಗಮ್ಯ_Unicode = ಮೂಲ_Char;
                ನಿಲ್ಲಿಸು;

            ಸ್ಥಿತಿ ISCII_NUKTA:
                /* handle soft halant */
                ಆದರೆ (*context_Char_ಗಮ್ಯ_Unicode == ISCII_HALANT) {
                    ಗುರಿ_Uni_Char = ZWJ;
                    /* clear the context */
                    *context_Char_ಗಮ್ಯ_Unicode = NO_CHAR_MARKER;
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode == PNJ_DELTA && ದತ್ತಾಂಶ->context_Char_ಗಮ್ಯ_Unicode == 0xc0) {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    ಆದರೆ (ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ) {
                        WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2,ಮೂಲ,ಗುರಿ,ವಾದಗಳು_2->offsets,(ಮೂಲ-ವಾದಗಳು_2->ಮೂಲ -1),ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ,0,err);
                        ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0x0000;
                    }
                    /* We got here because ISCII_NUKTA was preceded by 0xc0 and we are converting Gurmukhi.
                     * In that case we must convert (0xc0 0xe9) to (\u0a5c\u0a4d\u0a39).
                     */
                    ಗುರಿ_Uni_Char = PNJ_RRA;
                    WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2, ಮೂಲ, ಗುರಿ, ವಾದಗಳು_2->offsets, (ಮೂಲ-ವಾದಗಳು_2->ಮೂಲ)-2, ಗುರಿ_Uni_Char, 0, err);
                    ಆದರೆ (U_SUCCESS(*err)) {
                        ಗುರಿ_Uni_Char = PNJ_SIGN_VIRAMA;
                        WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2, ಮೂಲ, ಗುರಿ, ವಾದಗಳು_2->offsets, (ಮೂಲ-ವಾದಗಳು_2->ಮೂಲ)-2, ಗುರಿ_Uni_Char, 0, err);
                        ಆದರೆ (U_SUCCESS(*err)) {
                            ಗುರಿ_Uni_Char = PNJ_HA;
                            WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2, ಮೂಲ, ಗುರಿ, ವಾದಗಳು_2->offsets, (ಮೂಲ-ವಾದಗಳು_2->ಮೂಲ)-2, ಗುರಿ_Uni_Char, 0, err);
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್[ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ++]= PNJ_HA;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್[ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ++]= PNJ_SIGN_VIRAMA;
                        ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್[ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ++]= PNJ_HA;
                    }
                    *ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = missingCharMarker;
                    ದತ್ತಾಂಶ->context_Char_ಗಮ್ಯ_Unicode = NO_CHAR_MARKER;
                    ಮುಂದುವರಿ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* try to handle <CHAR> + ISCII_NUKTA special mappings */
                    i=1;
                    ಸಿಕ್ಕಿತು =ಸುಳ್ಳು;
                    ಗಾಗಿ (; i<nuktaSpecialCases[0][0]; i++) {
                        ಆದರೆ (nuktaSpecialCases[i][0]==(uint8_t)
                                *context_Char_ಗಮ್ಯ_Unicode) {
                            ಗುರಿ_Uni_Char=nuktaSpecialCases[i][1];
                            ಸಿಕ್ಕಿತು =ನಿಜ;
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                    ಆದರೆ (ಸಿಕ್ಕಿತು) {
                        /* find out if the mapping is valid in this state */
                        ಆದರೆ (validity_ಕೋಷ್ಟಕ[(uint8_t)ಗುರಿ_Uni_Char] & ದತ್ತಾಂಶ->current_ಮುಖವಾಡ_ಗಮ್ಯ_Unicode) {
                            /*targetUniChar += data->currentDeltaToUnicode ;*/
                            *context_Char_ಗಮ್ಯ_Unicode= NO_CHAR_MARKER;
                            *ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = missingCharMarker;
                            ಆದರೆ (ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode == PNJ_DELTA) {
                                /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                                ಆದರೆ (ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ) {
                                    WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2,ಮೂಲ,ಗುರಿ,ವಾದಗಳು_2->offsets,(ಮೂಲ-ವಾದಗಳು_2->ಮೂಲ -1),ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ,0,err);
                                    ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0x0000;
                                }
                                WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2,ಮೂಲ,ಗುರಿ,ವಾದಗಳು_2->offsets,(ಮೂಲ-ವಾದಗಳು_2->ಮೂಲ -2),ಗುರಿ_Uni_Char,ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode,err);
                                ಮುಂದುವರಿ;
                            }
                            ನಿಲ್ಲಿಸು;
                        }
                        /* else fall through to default */
                    }
                    /* else fall through to default */
                    U_FALLTHROUGH;
                }
            ಪೂರ್ವನಿಯೋಜಿತ:GET_MAPPING(ಮೂಲ_Char,ಗುರಿ_Uni_Char,ದತ್ತಾಂಶ)
                ;
                *context_Char_ಗಮ್ಯ_Unicode = ಮೂಲ_Char;
                ನಿಲ್ಲಿಸು;
            }

            ಆದರೆ (*ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ != missingCharMarker) {
                /* Check to make sure that consonant clusters are handled correct for Gurmukhi script. */
                ಆದರೆ (ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode == PNJ_DELTA && ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ != 0 && isPNJConsonant(ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ) &&
                        (*ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ + PNJ_DELTA) == PNJ_SIGN_VIRAMA && ((UChar32)(ಗುರಿ_Uni_Char + PNJ_DELTA) == ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ)) {
                    /* Consonant clusters C + HALANT + C should be encoded as ADHAK + C */
                    ಸ್ಥಾನಾಂತರ = (ಪೂರ್ಣಾಂಕ)(ಮೂಲ-ವಾದಗಳು_2->ಮೂಲ - 3);
                    ತಾತ್ಕಾಲಿಕ_ಗುರಿ_Uni_Char = PNJ_ADHAK; /* This is necessary to avoid some compiler warnings. */
                    WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2,ಮೂಲ,ಗುರಿ,ವಾದಗಳು_2->offsets,ಸ್ಥಾನಾಂತರ,ತಾತ್ಕಾಲಿಕ_ಗುರಿ_Uni_Char,0,err);
                    WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2,ಮೂಲ,ಗುರಿ,ವಾದಗಳು_2->offsets,ಸ್ಥಾನಾಂತರ,ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ,0,err);
                    ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0x0000; /* reset the previous unicode code point */
                    *ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = missingCharMarker;
                    ಮುಂದುವರಿ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    ಆದರೆ (ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ) {
                        WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2,ಮೂಲ,ಗುರಿ,ವಾದಗಳು_2->offsets,(ಮೂಲ-ವಾದಗಳು_2->ಮೂಲ -1),ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ,0,err);
                        ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0x0000;
                    }
                    /* Check to make sure that Bindi and Tippi are handled correctly for Gurmukhi script. 
                     * If 0xA2 is preceded by a codepoint in the PNJ_BINDI_TIPPI_SET then the target codepoint should be Tippi instead of Bindi.
                     */
                    ಆದರೆ (ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode == PNJ_DELTA && (ಗುರಿ_Uni_Char + PNJ_DELTA) == PNJ_BINDI && isPNJBindiTippi((*ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ + PNJ_DELTA))) {
                        ಗುರಿ_Uni_Char = PNJ_TIPPI - PNJ_DELTA;
                        WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2,ಮೂಲ,ಗುರಿ,ವಾದಗಳು_2->offsets,(ಮೂಲ-ವಾದಗಳು_2->ಮೂಲ -2),*ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ,PNJ_DELTA,err);
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode == PNJ_DELTA && (ಗುರಿ_Uni_Char + PNJ_DELTA) == PNJ_SIGN_VIRAMA && isPNJConsonant((*ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ + PNJ_DELTA))) {
                        /* Store the current toUnicodeStatus code point for later handling of consonant cluster in Gurmukhi. */
                        ದತ್ತಾಂಶ->prev_ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = *ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ + PNJ_DELTA;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* write the previously mapped codepoint */
                        WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2,ಮೂಲ,ಗುರಿ,ವಾದಗಳು_2->offsets,(ಮೂಲ-ವಾದಗಳು_2->ಮೂಲ -2),*ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ,ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode,err);
                    }
                }
                *ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = missingCharMarker;
            }

            ಆದರೆ (ಗುರಿ_Uni_Char != missingCharMarker) {
                /* now save the targetUniChar for delayed write */
                *ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = (char16_t) ಗುರಿ_Uni_Char;
                ಆದರೆ (ದತ್ತಾಂಶ->reset_ಗಮ್ಯ_Default_ಗಮ್ಯ_Unicode) {
                    ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode = ದತ್ತಾಂಶ->def_Delta_ಗಮ್ಯ_Unicode;
                    ದತ್ತಾಂಶ->current_ಮುಖವಾಡ_ಗಮ್ಯ_Unicode = ದತ್ತಾಂಶ->def_ಮುಖವಾಡ_ಗಮ್ಯ_Unicode;
                    ದತ್ತಾಂಶ->reset_ಗಮ್ಯ_Default_ಗಮ್ಯ_Unicode=ಸುಳ್ಳು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {

                /* we reach here only if targetUniChar == missingCharMarker
                 * so assign codes to reason and err
                 */
                *err = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
CALLBACK:
                ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[0] = ಮೂಲ_Char;
                ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 1;
                ನಿಲ್ಲಿಸು;
            }

        } ಇಲ್ಲದಿದ್ದರೆ {
            *err =U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
    }

    ಆದರೆ (U_SUCCESS(*err) && ವಾದಗಳು_2->flush && ಮೂಲ == ಮೂಲ_ಎಲ್ಲೆ) {
        /* end of the input stream */
        UConverter *cnv = ವಾದಗಳು_2->converter;

        ಆದರೆ (*context_Char_ಗಮ್ಯ_Unicode==ATR || *context_Char_ಗಮ್ಯ_Unicode==EXT || *context_Char_ಗಮ್ಯ_Unicode==ISCII_INV) {
            /* set toUBytes[] */
            cnv->ಗಮ್ಯ_UBytes[0] = (uint8_t)*context_Char_ಗಮ್ಯ_Unicode;
            cnv->ಗಮ್ಯ_ULength = 1;

            /* avoid looping on truncated sequences */
            *context_Char_ಗಮ್ಯ_Unicode = NO_CHAR_MARKER;
        } ಇಲ್ಲದಿದ್ದರೆ {
            cnv->ಗಮ್ಯ_ULength = 0;
        }

        ಆದರೆ (*ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ != missingCharMarker) {
            /* output a remaining target character */
            WRITE_ಗಮ್ಯ_ಗುರಿ_ಗಮ್ಯ_U(ವಾದಗಳು_2,ಮೂಲ,ಗುರಿ,ವಾದಗಳು_2->offsets,(ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ -1),*ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ,ದತ್ತಾಂಶ->current_Delta_ಗಮ್ಯ_Unicode,err);
            *ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = missingCharMarker;
        }
    }

    ವಾದಗಳು_2->ಗುರಿ = ಗುರಿ;
    ವಾದಗಳು_2->ಮೂಲ = ಮೂಲ;
}

/* structure for SafeClone calculations */
ರಚನೆ cloneISCIIStruct {
    UConverter cnv;
    UConverter_ದತ್ತಾಂಶ_ISCII mydata;
};

ಸ್ಥಿರ UConverter * U_CALLCONV
ISCII_ಸುರಕ್ಷಿತ_Clone(ಸ್ಥಿರಾಂಕ UConverter *cnv,
              ಶೂನ್ಯ *ರಾಶಿ_ಬಫರ್,
              int32_t *p_ಬಫರ್_ಗಾತ್ರ,
              UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ರಚನೆ cloneISCIIStruct * localClone;
    int32_t ಬಫರ್_ಗಾತ್ರ_Needed = ಗಾತ್ರ(ರಚನೆ cloneISCIIStruct);

    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ (*p_ಬಫರ್_ಗಾತ್ರ == 0) { /* 'preflighting' request - set needed size into *pBufferSize */
        *p_ಬಫರ್_ಗಾತ್ರ = ಬಫರ್_ಗಾತ್ರ_Needed;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    localClone = (ರಚನೆ cloneISCIIStruct *)ರಾಶಿ_ಬಫರ್;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, ಗಾತ್ರ(UConverter_ದತ್ತಾಂಶ_ISCII));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = ನಿಜ;

    ಹಿಂತಿರುಗಿ &localClone->cnv;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ISCIIGet_Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UConverter *cnv,
                    ಸ್ಥಿರಾಂಕ USetAdder *sa,
                    UConverter_Unicode_ಗಣ which,
                    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    (ಶೂನ್ಯ)cnv;
    (ಶೂನ್ಯ)which;
    (ಶೂನ್ಯ)p_Error_ಸಂಕೇತ;
    int32_t idx, script;
    uint8_t mask;

    /* Since all ISCII versions allow switching to other ISCII
    scripts, we add all roundtrippable characters to this set. */
    sa->ಕೂಡಿಸು_Range(sa->ಗಣ, 0, ASCII_ಕೊನೆ);
    ಗಾಗಿ (script = DEVANAGARI; script <= MALAYALAM; script++) {
        mask = (uint8_t)(lookup_Initial_ದತ್ತಾಂಶ[script].ಮುಖವಾಡ_Enum_2);
        ಗಾಗಿ (idx = 0; idx < DELTA; idx++) {
            /* added check for TELUGU character */
            ಆದರೆ ((validity_ಕೋಷ್ಟಕ[idx] & mask) || (script==TELUGU && idx==0x31)) {
                sa->ಕೂಡಿಸು(sa->ಗಣ, idx + (script * DELTA) + INDIC_ಖಂಡ_BEGIN);
            }
        }
    }
    sa->ಕೂಡಿಸು(sa->ಗಣ, DANDA);
    sa->ಕೂಡಿಸು(sa->ಗಣ, DOUBLE_DANDA);
    sa->ಕೂಡಿಸು(sa->ಗಣ, ZWNJ);
    sa->ಕೂಡಿಸು(sa->ಗಣ, ZWJ);
}
U_CDECL_ಕೊನೆ
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _ISCIIImpl={

    UCNV_ISCII,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    _ISCIIOpen,
    _ISCIIClose,
    _ISCIIReset,

    UConverter_ಗಮ್ಯ_Unicode_ISCII_OFFSETS_LOGIC,
    UConverter_ಗಮ್ಯ_Unicode_ISCII_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISCII_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISCII_OFFSETS_LOGIC,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    _ISCIIgetName,
    ಶೂನ್ಯಸೂಚಕ,
    ISCII_ಸುರಕ್ಷಿತ_Clone,
    ISCIIGet_Unicode_ಗಣ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ ISCIIStatic_ದತ್ತಾಂಶ={
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
        "ISCII",
         0,
         UCNV_IBM,
         UCNV_ISCII,
         1,
         4,
        { 0x1a, 0, 0, 0 },
        0x1,
        ಸುಳ್ಳು,
        ಸುಳ್ಳು,
        0x0,
        0x0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ _ISCIIData=
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&ISCIIStatic_ದತ್ತಾಂಶ, &_ISCIIImpl);

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_LEGACY_CONVERSION */
