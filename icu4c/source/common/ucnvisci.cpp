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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv_cb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_OPTIONS_ਸੰਸਕਰਣ_ਨਕਾਬ 0xf
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NUKTA               0x093c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HALANT              0x094d
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ZWNJ                0x200c /* Zero Width Non Joiner */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ZWJ                 0x200d /* Zero width Joiner */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INVALID_CHAR        0xffff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ATR                 0xEF   /* Attribute code */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXT                 0xF0   /* Extension code */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DANDA               0x0964
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOUBLE_DANDA        0x0965
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISCII_NUKTA         0xE9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISCII_HALANT        0xE8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISCII_DANDA         0xEA
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISCII_INV           0xD9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISCII_VOWEL_SIGN_E  0xE0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INDIC_ਖੰਡ_BEGIN   0x0900
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INDIC_ਖੰਡ_ਸਿਰਾ     0x0D7F
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INDIC_RANGE         (INDIC_ਖੰਡ_ਸਿਰਾ - INDIC_ਖੰਡ_BEGIN)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VOCALLIC_RR         0x0931
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LF                  0x0A
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASCII_ਸਿਰਾ           0xA0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NO_CHAR_MARKER      0xFFFE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TELUGU_DELTA        DELTA * TELUGU
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEV_ABBR_SIGN       0x0970
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEV_ANUDATTA        0x0952
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXT_RANGE_BEGIN     0xA1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXT_RANGE_ਸਿਰਾ       0xEE

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PNJ_DELTA           0x0100
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PNJ_BINDI           0x0A02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PNJ_TIPPI           0x0A70
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PNJ_SIGN_VIRAMA     0x0A4D
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PNJ_ADHAK           0x0A71
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PNJ_HA              0x0A39
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PNJ_RRA             0x0A5C

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
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

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    DEV_ਨਕਾਬ =0x80,
    PNJ_ਨਕਾਬ =0x40,
    GJR_ਨਕਾਬ =0x20,
    ORI_ਨਕਾਬ =0x10,
    BNG_ਨਕਾਬ =0x08,
    KND_ਨਕਾਬ =0x04,
    MLM_ਨਕਾਬ =0x02,
    TML_ਨਕਾਬ =0x01,
    ZERO =0x00
}ਨਕਾਬ_Enum;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISCII_CNV_ਅਗੇਤਰ "ISCII,version="

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    char16_t context_Char_ਮੰਜ਼ਿਲ_Unicode;         /* previous Unicode codepoint for contextual analysis */
    char16_t contextCharFromUnicode;       /* previous Unicode codepoint for contextual analysis */
    uint16_t def_Delta_ਮੰਜ਼ਿਲ_Unicode;         /* delta for switching to default state when DEF is encountered  */
    uint16_t currentDeltaFromUnicode;   /* current delta in Indic block */
    uint16_t current_Delta_ਮੰਜ਼ਿਲ_Unicode;     /* current delta in Indic block */
    ਨਕਾਬ_Enum current_ਨਕਾਬ_From_Unicode;    /* mask for current state in toUnicode */
    ਨਕਾਬ_Enum current_ਨਕਾਬ_ਮੰਜ਼ਿਲ_Unicode;      /* mask for current state in toUnicode */
    ਨਕਾਬ_Enum def_ਨਕਾਬ_ਮੰਜ਼ਿਲ_Unicode;          /* mask for default state in toUnicode */
    UBool is_ਪਹਿਲਾ_ਬਫਰ;                /* boolean for fromUnicode to see if we need to announce the first script */
    UBool reset_ਮੰਜ਼ਿਲ_Default_ਮੰਜ਼ਿਲ_Unicode;      /* boolean for resetting to default delta and mask when a newline is encountered*/
    ਅੱਖਰ name[ਆਕਾਰ(ISCII_CNV_ਅਗੇਤਰ) + 1];
    UChar32 prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ;        /* Hold the previous toUnicodeStatus. This is necessary because we may need to know the last two code points. */
} UConverter_ਅੰਕੜੇ_ISCII;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Lookup_ਅੰਕੜੇ_Struct {
    UniLang uniLang;
    ਨਕਾਬ_Enum ਨਕਾਬ_Enum_2;
    ISCIILang isciiLang;
} Lookup_ਅੰਕੜੇ_Struct;

ਸਥਿਰ ਅਚਲ Lookup_ਅੰਕੜੇ_Struct lookup_Initial_ਅੰਕੜੇ[]={
    { DEVANAGARI, DEV_ਨਕਾਬ,  DEV },
    { BENGALI,    BNG_ਨਕਾਬ,  BNG },
    { GURMUKHI,   PNJ_ਨਕਾਬ,  PNJ },
    { GUJARATI,   GJR_ਨਕਾਬ,  GJR },
    { ORIYA,      ORI_ਨਕਾਬ,  ORI },
    { TAMIL,      TML_ਨਕਾਬ,  TML },
    { TELUGU,     KND_ਨਕਾਬ,  TLG },
    { KANNADA,    KND_ਨਕਾਬ,  KND },
    { MALAYALAM,  MLM_ਨਕਾਬ,  MLM }
};

/*
 * For special handling of certain Gurmukhi characters.
 * Bit 0 (value 1): PNJ consonant
 * Bit 1 (value 2): PNJ Bindi Tippi
 */
ਸਥਿਰ ਅਚਲ uint8_t pnjMap[80] = {
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

ਸਥਿਰ UBool
isPNJConsonant(UChar32 c) {
    ਜੇ (c < 0xa00 || 0xa50 <= c) {
        ਵਾਪਸ ਝੂਠ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ pnjMap[c - 0xa00] & 1;
    }
}

ਸਥਿਰ UBool
isPNJBindiTippi(UChar32 c) {
    ਜੇ (c < 0xa00 || 0xa50 <= c) {
        ਵਾਪਸ ਝੂਠ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ pnjMap[c - 0xa00] >> 1;
    }
}
U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
_ISCIIOpen(UConverter *cnv, UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, UError_ਕੋਡ *error_ਕੋਡ) {
    ਜੇ(p_ਦਲੀਲਾਂ->onlyTestIsLoadable) {
        ਵਾਪਸ;
    }

    cnv->extraInfo = uprv_malloc(ਆਕਾਰ(UConverter_ਅੰਕੜੇ_ISCII));

    ਜੇ (cnv->extraInfo != ਖਾਲੀ_ਸੂਚਕ) {
        int32_t len=0;
        UConverter_ਅੰਕੜੇ_ISCII *converter_ਅੰਕੜੇ=
                (UConverter_ਅੰਕੜੇ_ISCII *) cnv->extraInfo;
        converter_ਅੰਕੜੇ->context_Char_ਮੰਜ਼ਿਲ_Unicode=NO_CHAR_MARKER;
        cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = missingCharMarker;
        converter_ਅੰਕੜੇ->contextCharFromUnicode=0x0000;
        converter_ਅੰਕੜੇ->reset_ਮੰਜ਼ਿਲ_Default_ਮੰਜ਼ਿਲ_Unicode=ਝੂਠ;
        /* check if the version requested is supported */
        ਜੇ ((p_ਦਲੀਲਾਂ->options & UCNV_OPTIONS_ਸੰਸਕਰਣ_ਨਕਾਬ) < 9) {
            /* initialize state variables */
            converter_ਅੰਕੜੇ->currentDeltaFromUnicode
                    = converter_ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode
                            = converter_ਅੰਕੜੇ->def_Delta_ਮੰਜ਼ਿਲ_Unicode = (uint16_t)(lookup_Initial_ਅੰਕੜੇ[p_ਦਲੀਲਾਂ->options & UCNV_OPTIONS_ਸੰਸਕਰਣ_ਨਕਾਬ].uniLang * DELTA);

            converter_ਅੰਕੜੇ->current_ਨਕਾਬ_From_Unicode
                    = converter_ਅੰਕੜੇ->current_ਨਕਾਬ_ਮੰਜ਼ਿਲ_Unicode
                            = converter_ਅੰਕੜੇ->def_ਨਕਾਬ_ਮੰਜ਼ਿਲ_Unicode = lookup_Initial_ਅੰਕੜੇ[p_ਦਲੀਲਾਂ->options & UCNV_OPTIONS_ਸੰਸਕਰਣ_ਨਕਾਬ].ਨਕਾਬ_Enum_2;
            
            converter_ਅੰਕੜੇ->is_ਪਹਿਲਾ_ਬਫਰ=ਸੱਚ;
            (ਖਾਲੀ)uprv_strcpy(converter_ਅੰਕੜੇ->name, ISCII_CNV_ਅਗੇਤਰ);
            len = (int32_t)uprv_strlen(converter_ਅੰਕੜੇ->name);
            converter_ਅੰਕੜੇ->name[len]= (ਅੱਖਰ)((p_ਦਲੀਲਾਂ->options & UCNV_OPTIONS_ਸੰਸਕਰਣ_ਨਕਾਬ) + '0');
            converter_ਅੰਕੜੇ->name[len+1]=0;
            
            converter_ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0x0000;
        } ਨਹੀਂਤਾਂ {
            uprv_free(cnv->extraInfo);
            cnv->extraInfo = ਖਾਲੀ_ਸੂਚਕ;
            *error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        }

    } ਨਹੀਂਤਾਂ {
        *error_ਕੋਡ =U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_ISCIIClose(UConverter *cnv) {
    ਜੇ (cnv->extraInfo!=ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo=ਖਾਲੀ_ਸੂਚਕ;
    }
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ*  U_CALLCONV
_ISCIIgetName(ਅਚਲ UConverter* cnv) {
    ਜੇ (cnv->extraInfo) {
        UConverter_ਅੰਕੜੇ_ISCII* my_ਅੰਕੜੇ= (UConverter_ਅੰਕੜੇ_ISCII*)cnv->extraInfo;
        ਵਾਪਸ my_ਅੰਕੜੇ->name;
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_ISCIIReset(UConverter *cnv, UConverterResetChoice choice) {
    UConverter_ਅੰਕੜੇ_ISCII* ਅੰਕੜੇ =(UConverter_ਅੰਕੜੇ_ISCII *) (cnv->extraInfo);
    ਜੇ (choice<=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = missingCharMarker;
        cnv->ਬਹੁਲਕ=0;
        ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode=ਅੰਕੜੇ->def_Delta_ਮੰਜ਼ਿਲ_Unicode;
        ਅੰਕੜੇ->current_ਨਕਾਬ_ਮੰਜ਼ਿਲ_Unicode = ਅੰਕੜੇ->def_ਨਕਾਬ_ਮੰਜ਼ਿਲ_Unicode;
        ਅੰਕੜੇ->context_Char_ਮੰਜ਼ਿਲ_Unicode=NO_CHAR_MARKER;
        ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0x0000;
    }
    ਜੇ (choice!=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        cnv->fromUChar32=0x0000;
        ਅੰਕੜੇ->contextCharFromUnicode=0x00;
        ਅੰਕੜੇ->current_ਨਕਾਬ_From_Unicode=ਅੰਕੜੇ->def_ਨਕਾਬ_ਮੰਜ਼ਿਲ_Unicode;
        ਅੰਕੜੇ->currentDeltaFromUnicode=ਅੰਕੜੇ->def_Delta_ਮੰਜ਼ਿਲ_Unicode;
        ਅੰਕੜੇ->is_ਪਹਿਲਾ_ਬਫਰ=ਸੱਚ;
        ਅੰਕੜੇ->reset_ਮੰਜ਼ਿਲ_Default_ਮੰਜ਼ਿਲ_Unicode=ਝੂਠ;
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

ਸਥਿਰ ਅਚਲ uint8_t validity_ਸਾਰਣੀ[128] = {
/* This state table is tool generated please do not edit unless you know exactly what you are doing */
/* Note: This table was edited to mirror the Windows XP implementation */
/*ISCII:Valid:Unicode */
/*0xa0 : 0x00: 0x900  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xa1 : 0xb8: 0x901  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + ZERO     + ZERO     + ZERO     ,
/*0xa2 : 0xfe: 0x902  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xa3 : 0xbf: 0x903  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0x00 : 0x00: 0x904  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xa4 : 0xff: 0x905  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xa5 : 0xff: 0x906  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xa6 : 0xff: 0x907  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xa7 : 0xff: 0x908  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xa8 : 0xff: 0x909  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xa9 : 0xff: 0x90a  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xaa : 0xfe: 0x90b  */ DEV_ਨਕਾਬ + ZERO     + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0x00 : 0x00: 0x90c  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0xae : 0x80: 0x90d  */ DEV_ਨਕਾਬ + ZERO     + GJR_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xab : 0x87: 0x90e  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xac : 0xff: 0x90f  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xad : 0xff: 0x910  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xb2 : 0x80: 0x911  */ DEV_ਨਕਾਬ + ZERO     + GJR_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xaf : 0x87: 0x912  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xb0 : 0xff: 0x913  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xb1 : 0xff: 0x914  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xb3 : 0xff: 0x915  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xb4 : 0xfe: 0x916  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0xb5 : 0xfe: 0x917  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0xb6 : 0xfe: 0x918  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0xb7 : 0xff: 0x919  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xb8 : 0xff: 0x91a  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xb9 : 0xfe: 0x91b  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0xba : 0xff: 0x91c  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xbb : 0xfe: 0x91d  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0xbc : 0xff: 0x91e  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xbd : 0xff: 0x91f  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xbe : 0xfe: 0x920  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0xbf : 0xfe: 0x921  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0xc0 : 0xfe: 0x922  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0xc1 : 0xff: 0x923  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xc2 : 0xff: 0x924  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xc3 : 0xfe: 0x925  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0xc4 : 0xfe: 0x926  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0xc5 : 0xfe: 0x927  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0xc6 : 0xff: 0x928  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xc7 : 0x81: 0x929  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + TML_ਨਕਾਬ ,
/*0xc8 : 0xff: 0x92a  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xc9 : 0xfe: 0x92b  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0xca : 0xfe: 0x92c  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0xcb : 0xfe: 0x92d  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0xcc : 0xfe: 0x92e  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xcd : 0xff: 0x92f  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xcf : 0xff: 0x930  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xd0 : 0x87: 0x931  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xd1 : 0xff: 0x932  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xd2 : 0xb7: 0x933  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + ZERO     + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xd3 : 0x83: 0x934  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xd4 : 0xff: 0x935  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + ZERO     + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xd5 : 0xfe: 0x936  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0xd6 : 0xbf: 0x937  */ DEV_ਨਕਾਬ + ZERO     + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xd7 : 0xff: 0x938  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xd8 : 0xff: 0x939  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0x00 : 0x00: 0x93A  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x93B  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe9 : 0xda: 0x93c  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + ZERO     + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x93d  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xda : 0xff: 0x93e  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xdb : 0xff: 0x93f  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xdc : 0xff: 0x940  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xdd : 0xff: 0x941  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xde : 0xff: 0x942  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xdf : 0xbe: 0x943  */ DEV_ਨਕਾਬ + ZERO     + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0x00 : 0x00: 0x944  */ DEV_ਨਕਾਬ + ZERO     + GJR_ਨਕਾਬ + ZERO     + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + ZERO     + ZERO     ,
/*0xe3 : 0x80: 0x945  */ DEV_ਨਕਾਬ + ZERO     + GJR_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe0 : 0x87: 0x946  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xe1 : 0xff: 0x947  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xe2 : 0xff: 0x948  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xe7 : 0x80: 0x949  */ DEV_ਨਕਾਬ + ZERO     + GJR_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xe4 : 0x87: 0x94a  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xe5 : 0xff: 0x94b  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xe6 : 0xff: 0x94c  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xe8 : 0xff: 0x94d  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xec : 0x00: 0x94e  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xed : 0x00: 0x94f  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x950  */ DEV_ਨਕਾਬ + ZERO     + GJR_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x951  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x952  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x953  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x954  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x955  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + KND_ਨਕਾਬ + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x956  */ ZERO     + ZERO     + ZERO     + ORI_ਨਕਾਬ + ZERO     + KND_ਨਕਾਬ + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x957  */ ZERO     + ZERO     + ZERO     + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + ZERO     + MLM_ਨਕਾਬ + ZERO     ,
/*0x00 : 0x00: 0x958  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x959  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95a  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95b  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95c  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + ZERO     + ZERO     + BNG_ਨਕਾਬ + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95d  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x95e  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xce : 0x98: 0x95f  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x960  */ DEV_ਨਕਾਬ + ZERO     + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0x00 : 0x00: 0x961  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + ZERO     ,
/*0x00 : 0x00: 0x962  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + BNG_ਨਕਾਬ + ZERO     + ZERO     + ZERO     ,
/*0x00 : 0x00: 0x963  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + BNG_ਨਕਾਬ + ZERO     + ZERO     + ZERO     ,
/*0xea : 0xf8: 0x964  */ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xeaea : 0x00: 0x965*/ DEV_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*0xf1 : 0xff: 0x966  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xf2 : 0xff: 0x967  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xf3 : 0xff: 0x968  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xf4 : 0xff: 0x969  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xf5 : 0xff: 0x96a  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xf6 : 0xff: 0x96b  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xf7 : 0xff: 0x96c  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xf8 : 0xff: 0x96d  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xf9 : 0xff: 0x96e  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0xfa : 0xff: 0x96f  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + GJR_ਨਕਾਬ + ORI_ਨਕਾਬ + BNG_ਨਕਾਬ + KND_ਨਕਾਬ + MLM_ਨਕਾਬ + TML_ਨਕਾਬ ,
/*0x00 : 0x80: 0x970  */ DEV_ਨਕਾਬ + PNJ_ਨਕਾਬ + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     ,
/*
 * The length of the array is 128 to provide values for 0x900..0x97f.
 * The last 15 entries for 0x971..0x97f of the validity table are all zero
 * because no Indic script uses such Unicode code points.
 */
/*0x00 : 0x00: 0x9yz  */ ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO     + ZERO
};

ਸਥਿਰ ਅਚਲ uint16_t from_Unicode_ਸਾਰਣੀ[128]={
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
ਸਥਿਰ ਅਚਲ uint16_t ਮੰਜ਼ਿਲ_Unicode_ਸਾਰਣੀ[256]={
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

ਸਥਿਰ ਅਚਲ uint16_t vowelSignESpecialCases[][2]={
	{ 2 /*length of array*/    , 0      },
	{ 0xA4 , 0x0904 },
};

ਸਥਿਰ ਅਚਲ uint16_t nuktaSpecialCases[][2]={
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


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_FROM_U(ਦਲੀਲਾਂ_2,offsets,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,ਨਿਸ਼ਾਨਾ_Byte_Unit,err) UPRV_ਖੰਡ_MACRO_BEGIN { \
    int32_t ਵਿੱਥ = (int32_t)(ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ-1);                                        \
      /* write the targetUniChar  to target */                                                  \
    ਜੇ(ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ){                                                                   \
        ਜੇ(ਨਿਸ਼ਾਨਾ_Byte_Unit <= 0xFF){                                                             \
            *(ਨਿਸ਼ਾਨਾ)++ = (uint8_t)(ਨਿਸ਼ਾਨਾ_Byte_Unit);                                            \
            ਜੇ(offsets){                                                                        \
                *(offsets++) = ਵਿੱਥ;                                                          \
            }                                                                                   \
        }ਨਹੀਂਤਾਂ{                                                                                  \
            ਜੇ (ਨਿਸ਼ਾਨਾ_Byte_Unit > 0xFFFF) {                                                      \
                *(ਨਿਸ਼ਾਨਾ)++ = (uint8_t)(ਨਿਸ਼ਾਨਾ_Byte_Unit>>16);                                    \
                ਜੇ (offsets) {                                                                  \
                    --ਵਿੱਥ;                                                                   \
                    *(offsets++) = ਵਿੱਥ;                                                      \
                }                                                                               \
            }                                                                                   \
            ਜੇ (!(ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)) {                                                      \
                ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] =    \
                                (uint8_t)(ਨਿਸ਼ਾਨਾ_Byte_Unit >> 8);                                 \
                ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] =    \
                                (uint8_t)ਨਿਸ਼ਾਨਾ_Byte_Unit;                                        \
                *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;                                                 \
            } ਨਹੀਂਤਾਂ {                                                                            \
                *(ਨਿਸ਼ਾਨਾ)++ = (uint8_t)(ਨਿਸ਼ਾਨਾ_Byte_Unit>>8);                                     \
                ਜੇ(offsets){                                                                    \
                    *(offsets++) = ਵਿੱਥ;                                                      \
                }                                                                               \
                ਜੇ(ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ){                                                       \
                    *(ਨਿਸ਼ਾਨਾ)++ = (uint8_t)  ਨਿਸ਼ਾਨਾ_Byte_Unit;                                    \
                    ਜੇ(offsets){                                                                \
                        *(offsets++) = ਵਿੱਥ                            ;                      \
                    }                                                                           \
                }ਨਹੀਂਤਾਂ{                                                                          \
                    ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] =\
                                (uint8_t) (ਨਿਸ਼ਾਨਾ_Byte_Unit);                                     \
                    *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;                                             \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
    }ਨਹੀਂਤਾਂ{                                                                                      \
        ਜੇ (ਨਿਸ਼ਾਨਾ_Byte_Unit & 0xFF0000) {                                                        \
            ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] =        \
                        (uint8_t) (ਨਿਸ਼ਾਨਾ_Byte_Unit >>16);                                        \
        }                                                                                       \
        ਜੇ(ਨਿਸ਼ਾਨਾ_Byte_Unit & 0xFF00){                                                            \
            ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] =        \
                        (uint8_t) (ਨਿਸ਼ਾਨਾ_Byte_Unit >>8);                                         \
        }                                                                                       \
        ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] =            \
                        (uint8_t) (ਨਿਸ਼ਾਨਾ_Byte_Unit);                                             \
        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;                                                         \
    }                                                                                           \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ

/* Rules:
 *    Explicit Halant :
 *                      <HALANT> + <ZWNJ>
 *    Soft Halant :
 *                      <HALANT> + <ZWJ>
 */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
UConverter_fromUnicode_ISCII_OFFSETS_LOGIC(
        UConverter_From_Unicode_ਦਲੀਲਾਂ * ਦਲੀਲਾਂ_2, UError_ਕੋਡ * err) {
    ਅਚਲ char16_t *ਸਰੋਤ = ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    ਅਚਲ char16_t *ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    int32_t* offsets = ਦਲੀਲਾਂ_2->offsets;
    uint32_t ਨਿਸ਼ਾਨਾ_Byte_Unit = 0x0000;
    UChar32 ਸਰੋਤ_Char = 0x0000;
    UChar32 ਆਰਜ਼ੀ_Context_From_Unicode = 0x0000;    /* For special handling of the Gurmukhi script. */
    UConverter_ਅੰਕੜੇ_ISCII *converter_ਅੰਕੜੇ;
    uint16_t newDelta=0;
    uint16_t range = 0;
    UBool delta_ਬਦਲਿਆ = ਝੂਠ;

    ਜੇ ((ਦਲੀਲਾਂ_2->converter == ਖਾਲੀ_ਸੂਚਕ) || (ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ < ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ) || (ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ < ਦਲੀਲਾਂ_2->ਸਰੋਤ)) {
        *err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    /* initialize data */
    converter_ਅੰਕੜੇ=(UConverter_ਅੰਕੜੇ_ISCII*)ਦਲੀਲਾਂ_2->converter->extraInfo;
    newDelta=converter_ਅੰਕੜੇ->currentDeltaFromUnicode;
    range = (uint16_t)(newDelta/DELTA);

    ਜੇ ((ਸਰੋਤ_Char = ਦਲੀਲਾਂ_2->converter->fromUChar32)!=0) {
        ਜਾਓ getTrail;
    }

    /*writing the char to the output stream */
    ਜਦੋਂ (ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /* Write the language code following LF only if LF is not the last character. */
        ਜੇ (ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ == LF) {
            ਨਿਸ਼ਾਨਾ_Byte_Unit = ATR<<8;
            ਨਿਸ਼ਾਨਾ_Byte_Unit += (uint8_t) lookup_Initial_ਅੰਕੜੇ[range].isciiLang;
            ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ = 0x0000;
            /* now append ATR and language code */
            WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_FROM_U(ਦਲੀਲਾਂ_2,offsets,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,ਨਿਸ਼ਾਨਾ_Byte_Unit,err);
            ਜੇ (U_FAILURE(*err)) {
                ਰੋਕੋ;
            }
        }
        
        ਸਰੋਤ_Char = *ਸਰੋਤ++;
        ਆਰਜ਼ੀ_Context_From_Unicode = converter_ਅੰਕੜੇ->contextCharFromUnicode;
        
        ਨਿਸ਼ਾਨਾ_Byte_Unit = missingCharMarker;
        
        /*check if input is in ASCII and C0 control codes range*/
        ਜੇ (ਸਰੋਤ_Char <= ASCII_ਸਿਰਾ) {
            ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ = ਸਰੋਤ_Char;
            WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_FROM_U(ਦਲੀਲਾਂ_2,offsets,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,ਸਰੋਤ_Char,err);
            ਜੇ (U_FAILURE(*err)) {
                ਰੋਕੋ;
            }
            ਜਾਰੀ;
        }
        ਚੋਣ (ਸਰੋਤ_Char) {
        ਹਾਲਤ ZWNJ:
            /* contextChar has HALANT */
            ਜੇ (converter_ਅੰਕੜੇ->contextCharFromUnicode) {
                converter_ਅੰਕੜੇ->contextCharFromUnicode = 0x00;
                ਨਿਸ਼ਾਨਾ_Byte_Unit = ISCII_HALANT;
            } ਨਹੀਂਤਾਂ {
                /* consume ZWNJ and continue */
                converter_ਅੰਕੜੇ->contextCharFromUnicode = 0x00;
                ਜਾਰੀ;
            }
            ਰੋਕੋ;
        ਹਾਲਤ ZWJ:
            /* contextChar has HALANT */
            ਜੇ (converter_ਅੰਕੜੇ->contextCharFromUnicode) {
                ਨਿਸ਼ਾਨਾ_Byte_Unit = ISCII_NUKTA;
            } ਨਹੀਂਤਾਂ {
                ਨਿਸ਼ਾਨਾ_Byte_Unit =ISCII_INV;
            }
            converter_ਅੰਕੜੇ->contextCharFromUnicode = 0x00;
            ਰੋਕੋ;
        ਮੂਲ:
            /* is the sourceChar in the INDIC_RANGE? */
            ਜੇ ((uint16_t)(INDIC_ਖੰਡ_ਸਿਰਾ-ਸਰੋਤ_Char) <= INDIC_RANGE) {
                /* Danda and Double Danda are valid in Northern scripts.. since Unicode
                 * does not include these codepoints in all Northern scrips we need to
                 * filter them out
                 */
                ਜੇ (ਸਰੋਤ_Char!= DANDA && ਸਰੋਤ_Char != DOUBLE_DANDA) {
                    /* find out to which block the souceChar belongs*/
                    range =(uint16_t)((ਸਰੋਤ_Char-INDIC_ਖੰਡ_BEGIN)/DELTA);
                    newDelta =(uint16_t)(range*DELTA);

                    /* Now are we in the same block as the previous? */
                    ਜੇ (newDelta!= converter_ਅੰਕੜੇ->currentDeltaFromUnicode || converter_ਅੰਕੜੇ->is_ਪਹਿਲਾ_ਬਫਰ) {
                        converter_ਅੰਕੜੇ->currentDeltaFromUnicode = newDelta;
                        converter_ਅੰਕੜੇ->current_ਨਕਾਬ_From_Unicode = lookup_Initial_ਅੰਕੜੇ[range].ਨਕਾਬ_Enum_2;
                        delta_ਬਦਲਿਆ =ਸੱਚ;
                        converter_ਅੰਕੜੇ->is_ਪਹਿਲਾ_ਬਫਰ=ਝੂਠ;
                    }
                    
                    ਜੇ (converter_ਅੰਕੜੇ->currentDeltaFromUnicode == PNJ_DELTA) { 
                        ਜੇ (ਸਰੋਤ_Char == PNJ_TIPPI) {
                            /* Make sure Tippi is converted to Bindi. */
                            ਸਰੋਤ_Char = PNJ_BINDI;
                        } ਨਹੀਂਤਾਂ ਜੇ (ਸਰੋਤ_Char == PNJ_ADHAK) {
                            /* This is for consonant cluster handling. */
                            converter_ਅੰਕੜੇ->contextCharFromUnicode = PNJ_ADHAK;
                        }
                        
                    }
                    /* Normalize all Indic codepoints to Devanagari and map them to ISCII */
                    /* now subtract the new delta from sourceChar*/
                    ਸਰੋਤ_Char -= converter_ਅੰਕੜੇ->currentDeltaFromUnicode;
                }

                /* get the target byte unit */
                ਨਿਸ਼ਾਨਾ_Byte_Unit=from_Unicode_ਸਾਰਣੀ[(uint8_t)ਸਰੋਤ_Char];

                /* is the code point valid in current script? */
                ਜੇ ((validity_ਸਾਰਣੀ[(uint8_t)ਸਰੋਤ_Char] & converter_ਅੰਕੜੇ->current_ਨਕਾਬ_From_Unicode)==0) {
                    /* Vocallic RR is assigned in ISCII Telugu and Unicode */
                    ਜੇ (converter_ਅੰਕੜੇ->currentDeltaFromUnicode!=(TELUGU_DELTA) || ਸਰੋਤ_Char!=VOCALLIC_RR) {
                        ਨਿਸ਼ਾਨਾ_Byte_Unit=missingCharMarker;
                    }
                }

                ਜੇ (delta_ਬਦਲਿਆ) {
                    /* we are in a script block which is different than
                     * previous sourceChar's script block write ATR and language codes
                     */
                    uint32_t ਆਰਜ਼ੀ=0;
                    ਆਰਜ਼ੀ =(uint16_t)(ATR<<8);
                    ਆਰਜ਼ੀ += (uint16_t)((uint8_t) lookup_Initial_ਅੰਕੜੇ[range].isciiLang);
                    /* reset */
                    delta_ਬਦਲਿਆ=ਝੂਠ;
                    /* now append ATR and language code */
                    WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_FROM_U(ਦਲੀਲਾਂ_2,offsets,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,ਆਰਜ਼ੀ,err);
                    ਜੇ (U_FAILURE(*err)) {
                        ਰੋਕੋ;
                    }
                }
                
                ਜੇ (converter_ਅੰਕੜੇ->currentDeltaFromUnicode == PNJ_DELTA && (ਸਰੋਤ_Char + PNJ_DELTA) == PNJ_ADHAK) {
                    ਜਾਰੀ;
                }
            }
            /* reset context char */
            converter_ਅੰਕੜੇ->contextCharFromUnicode = 0x00;
            ਰੋਕੋ;
        }
        ਜੇ (converter_ਅੰਕੜੇ->currentDeltaFromUnicode == PNJ_DELTA && ਆਰਜ਼ੀ_Context_From_Unicode == PNJ_ADHAK && isPNJConsonant((ਸਰੋਤ_Char + PNJ_DELTA))) {
            /* If the previous codepoint is Adhak and the current codepoint is a consonant, the targetByteUnit should be C + Halant + C. */
            /* reset context char */
            converter_ਅੰਕੜੇ->contextCharFromUnicode = 0x0000;
            ਨਿਸ਼ਾਨਾ_Byte_Unit = ਨਿਸ਼ਾਨਾ_Byte_Unit << 16 | ISCII_HALANT << 8 | ਨਿਸ਼ਾਨਾ_Byte_Unit;
            /* write targetByteUnit to target */
            WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_FROM_U(ਦਲੀਲਾਂ_2, offsets, ਸਰੋਤ, ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ, ਨਿਸ਼ਾਨਾ_Byte_Unit,err);
            ਜੇ (U_FAILURE(*err)) {
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ (ਨਿਸ਼ਾਨਾ_Byte_Unit != missingCharMarker) {
            ਜੇ (ਨਿਸ਼ਾਨਾ_Byte_Unit==ISCII_HALANT) {
                converter_ਅੰਕੜੇ->contextCharFromUnicode = (char16_t)ਨਿਸ਼ਾਨਾ_Byte_Unit;
            }
            /* write targetByteUnit to target*/
            WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_FROM_U(ਦਲੀਲਾਂ_2,offsets,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,ਨਿਸ਼ਾਨਾ_Byte_Unit,err);
            ਜੇ (U_FAILURE(*err)) {
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ {
            /* oops.. the code point is unassigned */
            /*check if the char is a First surrogate*/
            ਜੇ (U16_IS_SURROGATE(ਸਰੋਤ_Char)) {
                ਜੇ (U16_IS_SURROGATE_LEAD(ਸਰੋਤ_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    ਜੇ (ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                        /* test the following code unit */
                        char16_t trail= (*ਸਰੋਤ);
                        ਜੇ (U16_IS_TRAIL(trail)) {
                            ਸਰੋਤ++;
                            ਸਰੋਤ_Char=U16_GET_SUPPLEMENTARY(ਸਰੋਤ_Char, trail);
                            *err =U_INVALID_CHAR_ਮਿਲਿਆ;
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } ਨਹੀਂਤਾਂ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* no more input */
                        *err = U_ZERO_ERROR;
                    }
                } ਨਹੀਂਤਾਂ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                }
            } ਨਹੀਂਤਾਂ {
                /* callback(unassigned) for a BMP code point */
                *err = U_INVALID_CHAR_ਮਿਲਿਆ;
            }

            ਦਲੀਲਾਂ_2->converter->fromUChar32=ਸਰੋਤ_Char;
            ਰੋਕੋ;
        }
    }/* end while(mySourceIndex<mySourceLength) */

    /*save the state and return */
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = ਸਰੋਤ;
    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = (ਅੱਖਰ*)ਨਿਸ਼ਾਨਾ;
}

ਸਥਿਰ ਅਚਲ uint16_t lookup_ਸਾਰਣੀ[][2]={
    { ZERO,       ZERO     },     /*DEFAULT*/
    { ZERO,       ZERO     },     /*ROMAN*/
    { DEVANAGARI, DEV_ਨਕਾਬ },
    { BENGALI,    BNG_ਨਕਾਬ },
    { TAMIL,      TML_ਨਕਾਬ },
    { TELUGU,     KND_ਨਕਾਬ },
    { BENGALI,    BNG_ਨਕਾਬ },
    { ORIYA,      ORI_ਨਕਾਬ },
    { KANNADA,    KND_ਨਕਾਬ },
    { MALAYALAM,  MLM_ਨਕਾਬ },
    { GUJARATI,   GJR_ਨਕਾਬ },
    { GURMUKHI,   PNJ_ਨਕਾਬ }
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,offsets,ਵਿੱਥ,ਨਿਸ਼ਾਨਾ_Uni_Char,delta, err) UPRV_ਖੰਡ_MACRO_BEGIN { \
    /* add offset to current Indic Block */                                              \
    ਜੇ(ਨਿਸ਼ਾਨਾ_Uni_Char>ASCII_ਸਿਰਾ &&                                                        \
           ਨਿਸ਼ਾਨਾ_Uni_Char != ZWJ &&                                                       \
           ਨਿਸ਼ਾਨਾ_Uni_Char != ZWNJ &&                                                      \
           ਨਿਸ਼ਾਨਾ_Uni_Char != DANDA &&                                                     \
           ਨਿਸ਼ਾਨਾ_Uni_Char != DOUBLE_DANDA){                                               \
                                                                                         \
           ਨਿਸ਼ਾਨਾ_Uni_Char+=(uint16_t)(delta);                                             \
    }                                                                                    \
    /* now write the targetUniChar */                                                    \
    ਜੇ(ਨਿਸ਼ਾਨਾ<ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ){                                                        \
        *(ਨਿਸ਼ਾਨਾ)++ = (char16_t)ਨਿਸ਼ਾਨਾ_Uni_Char;                                              \
        ਜੇ(offsets){                                                                     \
            *(offsets)++ = (int32_t)(ਵਿੱਥ);                                            \
        }                                                                                \
    }ਨਹੀਂਤਾਂ{                                                                               \
        ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ++] =   \
            (char16_t)ਨਿਸ਼ਾਨਾ_Uni_Char;                                                        \
        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;                                                  \
    }                                                                                    \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_MAPPING(ਸਰੋਤ_Char,ਨਿਸ਼ਾਨਾ_Uni_Char,ਅੰਕੜੇ) UPRV_ਖੰਡ_MACRO_BEGIN {              \
    ਨਿਸ਼ਾਨਾ_Uni_Char = ਮੰਜ਼ਿਲ_Unicode_ਸਾਰਣੀ[(ਸਰੋਤ_Char)] ;                                       \
    /* is the code point valid in current script? */                                     \
    ਜੇ(ਸਰੋਤ_Char> ASCII_ਸਿਰਾ &&                                                          \
            (validity_ਸਾਰਣੀ[(ਨਿਸ਼ਾਨਾ_Uni_Char & 0x7F)] & ਅੰਕੜੇ->current_ਨਕਾਬ_ਮੰਜ਼ਿਲ_Unicode)==0){    \
        /* Vocallic RR is assigned in ISCII Telugu and Unicode */                         \
        ਜੇ(ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode!=(TELUGU_DELTA) ||                                \
                    ਨਿਸ਼ਾਨਾ_Uni_Char!=VOCALLIC_RR){                                         \
            ਨਿਸ਼ਾਨਾ_Uni_Char=missingCharMarker;                                             \
        }                                                                                \
    }                                                                                    \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ

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

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
UConverter_ਮੰਜ਼ਿਲ_Unicode_ISCII_OFFSETS_LOGIC(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2, UError_ਕੋਡ* err) {
    ਅਚਲ ਅੱਖਰ *ਸਰੋਤ = ( ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    char16_t *ਨਿਸ਼ਾਨਾ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    ਅਚਲ ਅੱਖਰ *ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਅਚਲ char16_t* ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    uint32_t ਨਿਸ਼ਾਨਾ_Uni_Char = 0x0000;
    uint8_t ਸਰੋਤ_Char = 0x0000;
    UConverter_ਅੰਕੜੇ_ISCII* ਅੰਕੜੇ;
    UChar32* ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=ਖਾਲੀ_ਸੂਚਕ;
    UChar32 ਆਰਜ਼ੀ_ਨਿਸ਼ਾਨਾ_Uni_Char = 0x0000;
    char16_t* context_Char_ਮੰਜ਼ਿਲ_Unicode= ਖਾਲੀ_ਸੂਚਕ;
    UBool ਮਿਲਿਆ;
    ਪੂਰਨ i; 
    ਪੂਰਨ ਵਿੱਥ = 0;

    ਜੇ ((ਦਲੀਲਾਂ_2->converter == ਖਾਲੀ_ਸੂਚਕ) || (ਨਿਸ਼ਾਨਾ < ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ) || (ਸਰੋਤ < ਦਲੀਲਾਂ_2->ਸਰੋਤ)) {
        *err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    ਅੰਕੜੇ = (UConverter_ਅੰਕੜੇ_ISCII*)(ਦਲੀਲਾਂ_2->converter->extraInfo);
    context_Char_ਮੰਜ਼ਿਲ_Unicode = &ਅੰਕੜੇ->context_Char_ਮੰਜ਼ਿਲ_Unicode; /* contains previous ISCII codepoint visited */
    ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = (UChar32*)&ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ;/* contains the mapping to Unicode of the above codepoint*/

    ਜਦੋਂ (U_SUCCESS(*err) && ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {

        ਨਿਸ਼ਾਨਾ_Uni_Char = missingCharMarker;

        ਜੇ (ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            ਸਰੋਤ_Char = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*(ਸਰੋਤ)++;

            /* look at the post-context perform special processing */
            ਜੇ (*context_Char_ਮੰਜ਼ਿਲ_Unicode==ATR) {

                /* If we have ATR in *contextCharToUnicode then we need to change our
                 * state to the Indic Script specified by sourceChar
                 */

                /* check if the sourceChar is supported script range*/
                ਜੇ ((uint8_t)(PNJ-ਸਰੋਤ_Char)<=PNJ-DEV) {
                    ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode = (uint16_t)(lookup_ਸਾਰਣੀ[ਸਰੋਤ_Char & 0x0F][0] * DELTA);
                    ਅੰਕੜੇ->current_ਨਕਾਬ_ਮੰਜ਼ਿਲ_Unicode = (ਨਕਾਬ_Enum)lookup_ਸਾਰਣੀ[ਸਰੋਤ_Char & 0x0F][1];
                } ਨਹੀਂਤਾਂ ਜੇ (ਸਰੋਤ_Char==DEF) {
                    /* switch back to default */
                    ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode = ਅੰਕੜੇ->def_Delta_ਮੰਜ਼ਿਲ_Unicode;
                    ਅੰਕੜੇ->current_ਨਕਾਬ_ਮੰਜ਼ਿਲ_Unicode = ਅੰਕੜੇ->def_ਨਕਾਬ_ਮੰਜ਼ਿਲ_Unicode;
                } ਨਹੀਂਤਾਂ {
                    ਜੇ ((ਸਰੋਤ_Char >= 0x21 && ਸਰੋਤ_Char <= 0x3F)) {
                        /* these are display codes consume and continue */
                    } ਨਹੀਂਤਾਂ {
                        *err =U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        /* reset */
                        *context_Char_ਮੰਜ਼ਿਲ_Unicode=NO_CHAR_MARKER;
                        ਜਾਓ CALLBACK;
                    }
                }

                /* reset */
                *context_Char_ਮੰਜ਼ਿਲ_Unicode=NO_CHAR_MARKER;

                ਜਾਰੀ;

            } ਨਹੀਂਤਾਂ ਜੇ (*context_Char_ਮੰਜ਼ਿਲ_Unicode==EXT) {
                /* check if sourceChar is in 0xA1-0xEE range */
                ਜੇ ((uint8_t) (EXT_RANGE_ਸਿਰਾ - ਸਰੋਤ_Char) <= (EXT_RANGE_ਸਿਰਾ - EXT_RANGE_BEGIN)) {
                    /* We currently support only Anudatta and Devanagari abbreviation sign */
                    ਜੇ (ਸਰੋਤ_Char==0xBF || ਸਰੋਤ_Char == 0xB8) {
                        ਨਿਸ਼ਾਨਾ_Uni_Char = (ਸਰੋਤ_Char==0xBF) ? DEV_ABBR_SIGN : DEV_ANUDATTA;
                        
                        /* find out if the mapping is valid in this state */
                        ਜੇ (validity_ਸਾਰਣੀ[(uint8_t)ਨਿਸ਼ਾਨਾ_Uni_Char] & ਅੰਕੜੇ->current_ਨਕਾਬ_ਮੰਜ਼ਿਲ_Unicode) {
                            *context_Char_ਮੰਜ਼ਿਲ_Unicode= NO_CHAR_MARKER;

                            /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                            ਜੇ (ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ) {
                                WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਦਲੀਲਾਂ_2->offsets,(ਸਰੋਤ-ਦਲੀਲਾਂ_2->ਸਰੋਤ -1),ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ,0,err);
                                ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0x0000;
                            }
                            /* write to target */
                            WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਦਲੀਲਾਂ_2->offsets,(ਸਰੋਤ-ਦਲੀਲਾਂ_2->ਸਰੋਤ -2),ਨਿਸ਼ਾਨਾ_Uni_Char,ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode,err);

                            ਜਾਰੀ;
                        }
                    }
                    /* byte unit is unassigned */
                    ਨਿਸ਼ਾਨਾ_Uni_Char = missingCharMarker;
                    *err= U_INVALID_CHAR_ਮਿਲਿਆ;
                } ਨਹੀਂਤਾਂ {
                    /* only 0xA1 - 0xEE are legal after EXT char */
                    *context_Char_ਮੰਜ਼ਿਲ_Unicode= NO_CHAR_MARKER;
                    *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                }
                ਜਾਓ CALLBACK;
            } ਨਹੀਂਤਾਂ ਜੇ (*context_Char_ਮੰਜ਼ਿਲ_Unicode==ISCII_INV) {
                ਜੇ (ਸਰੋਤ_Char==ISCII_HALANT) {
                    ਨਿਸ਼ਾਨਾ_Uni_Char = 0x0020; /* replace with space according to Indic FAQ */
                } ਨਹੀਂਤਾਂ {
                    ਨਿਸ਼ਾਨਾ_Uni_Char = ZWJ;
                }

                /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                ਜੇ (ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ) {
                    WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਦਲੀਲਾਂ_2->offsets,(ਸਰੋਤ-ਦਲੀਲਾਂ_2->ਸਰੋਤ -1),ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ,0,err);
                    ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0x0000;
                }
                /* write to target */
                WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਦਲੀਲਾਂ_2->offsets,(ਸਰੋਤ-ਦਲੀਲਾਂ_2->ਸਰੋਤ -2),ਨਿਸ਼ਾਨਾ_Uni_Char,ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode,err);
                /* reset */
                *context_Char_ਮੰਜ਼ਿਲ_Unicode=NO_CHAR_MARKER;
            }

            /* look at the pre-context and perform special processing */
            ਚੋਣ (ਸਰੋਤ_Char) {
            ਹਾਲਤ ISCII_INV:
            ਹਾਲਤ EXT:
            ਹਾਲਤ ATR:
                *context_Char_ਮੰਜ਼ਿਲ_Unicode = (char16_t)ਸਰੋਤ_Char;

                ਜੇ (*ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ != missingCharMarker) {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    ਜੇ (ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ) {
                        WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਦਲੀਲਾਂ_2->offsets,(ਸਰੋਤ-ਦਲੀਲਾਂ_2->ਸਰੋਤ -1),ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ,0,err);
                        ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0x0000;
                    }
                    WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਦਲੀਲਾਂ_2->offsets,(ਸਰੋਤ-ਦਲੀਲਾਂ_2->ਸਰੋਤ -2),*ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ,ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode,err);
                    *ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = missingCharMarker;
                }
                ਜਾਰੀ;
            ਹਾਲਤ ISCII_DANDA:
                /* handle double danda*/
                ਜੇ (*context_Char_ਮੰਜ਼ਿਲ_Unicode== ISCII_DANDA) {
                    ਨਿਸ਼ਾਨਾ_Uni_Char = DOUBLE_DANDA;
                    /* clear the context */
                    *context_Char_ਮੰਜ਼ਿਲ_Unicode = NO_CHAR_MARKER;
                    *ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = missingCharMarker;
                } ਨਹੀਂਤਾਂ {
                    GET_MAPPING(ਸਰੋਤ_Char,ਨਿਸ਼ਾਨਾ_Uni_Char,ਅੰਕੜੇ);
                    *context_Char_ਮੰਜ਼ਿਲ_Unicode = ਸਰੋਤ_Char;
                }
                ਰੋਕੋ;
            ਹਾਲਤ ISCII_HALANT:
                /* handle explicit halant */
                ਜੇ (*context_Char_ਮੰਜ਼ਿਲ_Unicode == ISCII_HALANT) {
                    ਨਿਸ਼ਾਨਾ_Uni_Char = ZWNJ;
                    /* clear the context */
                    *context_Char_ਮੰਜ਼ਿਲ_Unicode = NO_CHAR_MARKER;
                } ਨਹੀਂਤਾਂ {
                    GET_MAPPING(ਸਰੋਤ_Char,ਨਿਸ਼ਾਨਾ_Uni_Char,ਅੰਕੜੇ);
                    *context_Char_ਮੰਜ਼ਿਲ_Unicode = ਸਰੋਤ_Char;
                }
                ਰੋਕੋ;
            ਹਾਲਤ 0x0A:
            ਹਾਲਤ 0x0D:
                ਅੰਕੜੇ->reset_ਮੰਜ਼ਿਲ_Default_ਮੰਜ਼ਿਲ_Unicode = ਸੱਚ;
                GET_MAPPING(ਸਰੋਤ_Char,ਨਿਸ਼ਾਨਾ_Uni_Char,ਅੰਕੜੇ)
                ;
                *context_Char_ਮੰਜ਼ਿਲ_Unicode = ਸਰੋਤ_Char;
                ਰੋਕੋ;

            ਹਾਲਤ ISCII_VOWEL_SIGN_E:
                i=1;
                ਮਿਲਿਆ=ਝੂਠ;
                ਲਈ (; i<vowelSignESpecialCases[0][0]; i++) {
                    U_ASSERT(i<UPRV_LENGTHOF(vowelSignESpecialCases));
                    ਜੇ (vowelSignESpecialCases[i][0]==(uint8_t)*context_Char_ਮੰਜ਼ਿਲ_Unicode) {
                        ਨਿਸ਼ਾਨਾ_Uni_Char=vowelSignESpecialCases[i][1];
                        ਮਿਲਿਆ=ਸੱਚ;
                        ਰੋਕੋ;
                    }
                }
                ਜੇ (ਮਿਲਿਆ) {
                    /* find out if the mapping is valid in this state */
                    ਜੇ (validity_ਸਾਰਣੀ[(uint8_t)ਨਿਸ਼ਾਨਾ_Uni_Char] & ਅੰਕੜੇ->current_ਨਕਾਬ_ਮੰਜ਼ਿਲ_Unicode) {
                        /*targetUniChar += data->currentDeltaToUnicode ;*/
                        *context_Char_ਮੰਜ਼ਿਲ_Unicode= NO_CHAR_MARKER;
                        *ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = missingCharMarker;
                        ਰੋਕੋ;
                    }
                }
                GET_MAPPING(ਸਰੋਤ_Char,ਨਿਸ਼ਾਨਾ_Uni_Char,ਅੰਕੜੇ);
                *context_Char_ਮੰਜ਼ਿਲ_Unicode = ਸਰੋਤ_Char;
                ਰੋਕੋ;

            ਹਾਲਤ ISCII_NUKTA:
                /* handle soft halant */
                ਜੇ (*context_Char_ਮੰਜ਼ਿਲ_Unicode == ISCII_HALANT) {
                    ਨਿਸ਼ਾਨਾ_Uni_Char = ZWJ;
                    /* clear the context */
                    *context_Char_ਮੰਜ਼ਿਲ_Unicode = NO_CHAR_MARKER;
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ (ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode == PNJ_DELTA && ਅੰਕੜੇ->context_Char_ਮੰਜ਼ਿਲ_Unicode == 0xc0) {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    ਜੇ (ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ) {
                        WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਦਲੀਲਾਂ_2->offsets,(ਸਰੋਤ-ਦਲੀਲਾਂ_2->ਸਰੋਤ -1),ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ,0,err);
                        ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0x0000;
                    }
                    /* We got here because ISCII_NUKTA was preceded by 0xc0 and we are converting Gurmukhi.
                     * In that case we must convert (0xc0 0xe9) to (\u0a5c\u0a4d\u0a39).
                     */
                    ਨਿਸ਼ਾਨਾ_Uni_Char = PNJ_RRA;
                    WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2, ਸਰੋਤ, ਨਿਸ਼ਾਨਾ, ਦਲੀਲਾਂ_2->offsets, (ਸਰੋਤ-ਦਲੀਲਾਂ_2->ਸਰੋਤ)-2, ਨਿਸ਼ਾਨਾ_Uni_Char, 0, err);
                    ਜੇ (U_SUCCESS(*err)) {
                        ਨਿਸ਼ਾਨਾ_Uni_Char = PNJ_SIGN_VIRAMA;
                        WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2, ਸਰੋਤ, ਨਿਸ਼ਾਨਾ, ਦਲੀਲਾਂ_2->offsets, (ਸਰੋਤ-ਦਲੀਲਾਂ_2->ਸਰੋਤ)-2, ਨਿਸ਼ਾਨਾ_Uni_Char, 0, err);
                        ਜੇ (U_SUCCESS(*err)) {
                            ਨਿਸ਼ਾਨਾ_Uni_Char = PNJ_HA;
                            WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2, ਸਰੋਤ, ਨਿਸ਼ਾਨਾ, ਦਲੀਲਾਂ_2->offsets, (ਸਰੋਤ-ਦਲੀਲਾਂ_2->ਸਰੋਤ)-2, ਨਿਸ਼ਾਨਾ_Uni_Char, 0, err);
                        } ਨਹੀਂਤਾਂ {
                            ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ++]= PNJ_HA;
                        }
                    } ਨਹੀਂਤਾਂ {
                        ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ++]= PNJ_SIGN_VIRAMA;
                        ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ++]= PNJ_HA;
                    }
                    *ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = missingCharMarker;
                    ਅੰਕੜੇ->context_Char_ਮੰਜ਼ਿਲ_Unicode = NO_CHAR_MARKER;
                    ਜਾਰੀ;
                } ਨਹੀਂਤਾਂ {
                    /* try to handle <CHAR> + ISCII_NUKTA special mappings */
                    i=1;
                    ਮਿਲਿਆ =ਝੂਠ;
                    ਲਈ (; i<nuktaSpecialCases[0][0]; i++) {
                        ਜੇ (nuktaSpecialCases[i][0]==(uint8_t)
                                *context_Char_ਮੰਜ਼ਿਲ_Unicode) {
                            ਨਿਸ਼ਾਨਾ_Uni_Char=nuktaSpecialCases[i][1];
                            ਮਿਲਿਆ =ਸੱਚ;
                            ਰੋਕੋ;
                        }
                    }
                    ਜੇ (ਮਿਲਿਆ) {
                        /* find out if the mapping is valid in this state */
                        ਜੇ (validity_ਸਾਰਣੀ[(uint8_t)ਨਿਸ਼ਾਨਾ_Uni_Char] & ਅੰਕੜੇ->current_ਨਕਾਬ_ਮੰਜ਼ਿਲ_Unicode) {
                            /*targetUniChar += data->currentDeltaToUnicode ;*/
                            *context_Char_ਮੰਜ਼ਿਲ_Unicode= NO_CHAR_MARKER;
                            *ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = missingCharMarker;
                            ਜੇ (ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode == PNJ_DELTA) {
                                /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                                ਜੇ (ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ) {
                                    WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਦਲੀਲਾਂ_2->offsets,(ਸਰੋਤ-ਦਲੀਲਾਂ_2->ਸਰੋਤ -1),ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ,0,err);
                                    ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0x0000;
                                }
                                WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਦਲੀਲਾਂ_2->offsets,(ਸਰੋਤ-ਦਲੀਲਾਂ_2->ਸਰੋਤ -2),ਨਿਸ਼ਾਨਾ_Uni_Char,ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode,err);
                                ਜਾਰੀ;
                            }
                            ਰੋਕੋ;
                        }
                        /* else fall through to default */
                    }
                    /* else fall through to default */
                    U_FALLTHROUGH;
                }
            ਮੂਲ:GET_MAPPING(ਸਰੋਤ_Char,ਨਿਸ਼ਾਨਾ_Uni_Char,ਅੰਕੜੇ)
                ;
                *context_Char_ਮੰਜ਼ਿਲ_Unicode = ਸਰੋਤ_Char;
                ਰੋਕੋ;
            }

            ਜੇ (*ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ != missingCharMarker) {
                /* Check to make sure that consonant clusters are handled correct for Gurmukhi script. */
                ਜੇ (ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode == PNJ_DELTA && ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ != 0 && isPNJConsonant(ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ) &&
                        (*ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ + PNJ_DELTA) == PNJ_SIGN_VIRAMA && ((UChar32)(ਨਿਸ਼ਾਨਾ_Uni_Char + PNJ_DELTA) == ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ)) {
                    /* Consonant clusters C + HALANT + C should be encoded as ADHAK + C */
                    ਵਿੱਥ = (ਪੂਰਨ)(ਸਰੋਤ-ਦਲੀਲਾਂ_2->ਸਰੋਤ - 3);
                    ਆਰਜ਼ੀ_ਨਿਸ਼ਾਨਾ_Uni_Char = PNJ_ADHAK; /* This is necessary to avoid some compiler warnings. */
                    WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਦਲੀਲਾਂ_2->offsets,ਵਿੱਥ,ਆਰਜ਼ੀ_ਨਿਸ਼ਾਨਾ_Uni_Char,0,err);
                    WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਦਲੀਲਾਂ_2->offsets,ਵਿੱਥ,ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ,0,err);
                    ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0x0000; /* reset the previous unicode code point */
                    *ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = missingCharMarker;
                    ਜਾਰੀ;
                } ਨਹੀਂਤਾਂ {
                    /* Write the previous toUnicodeStatus, this was delayed to handle consonant clustering for Gurmukhi script. */
                    ਜੇ (ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ) {
                        WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਦਲੀਲਾਂ_2->offsets,(ਸਰੋਤ-ਦਲੀਲਾਂ_2->ਸਰੋਤ -1),ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ,0,err);
                        ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0x0000;
                    }
                    /* Check to make sure that Bindi and Tippi are handled correctly for Gurmukhi script. 
                     * If 0xA2 is preceded by a codepoint in the PNJ_BINDI_TIPPI_SET then the target codepoint should be Tippi instead of Bindi.
                     */
                    ਜੇ (ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode == PNJ_DELTA && (ਨਿਸ਼ਾਨਾ_Uni_Char + PNJ_DELTA) == PNJ_BINDI && isPNJBindiTippi((*ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ + PNJ_DELTA))) {
                        ਨਿਸ਼ਾਨਾ_Uni_Char = PNJ_TIPPI - PNJ_DELTA;
                        WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਦਲੀਲਾਂ_2->offsets,(ਸਰੋਤ-ਦਲੀਲਾਂ_2->ਸਰੋਤ -2),*ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ,PNJ_DELTA,err);
                    } ਨਹੀਂਤਾਂ ਜੇ (ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode == PNJ_DELTA && (ਨਿਸ਼ਾਨਾ_Uni_Char + PNJ_DELTA) == PNJ_SIGN_VIRAMA && isPNJConsonant((*ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ + PNJ_DELTA))) {
                        /* Store the current toUnicodeStatus code point for later handling of consonant cluster in Gurmukhi. */
                        ਅੰਕੜੇ->prev_ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = *ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ + PNJ_DELTA;
                    } ਨਹੀਂਤਾਂ {
                        /* write the previously mapped codepoint */
                        WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਦਲੀਲਾਂ_2->offsets,(ਸਰੋਤ-ਦਲੀਲਾਂ_2->ਸਰੋਤ -2),*ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ,ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode,err);
                    }
                }
                *ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = missingCharMarker;
            }

            ਜੇ (ਨਿਸ਼ਾਨਾ_Uni_Char != missingCharMarker) {
                /* now save the targetUniChar for delayed write */
                *ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = (char16_t) ਨਿਸ਼ਾਨਾ_Uni_Char;
                ਜੇ (ਅੰਕੜੇ->reset_ਮੰਜ਼ਿਲ_Default_ਮੰਜ਼ਿਲ_Unicode) {
                    ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode = ਅੰਕੜੇ->def_Delta_ਮੰਜ਼ਿਲ_Unicode;
                    ਅੰਕੜੇ->current_ਨਕਾਬ_ਮੰਜ਼ਿਲ_Unicode = ਅੰਕੜੇ->def_ਨਕਾਬ_ਮੰਜ਼ਿਲ_Unicode;
                    ਅੰਕੜੇ->reset_ਮੰਜ਼ਿਲ_Default_ਮੰਜ਼ਿਲ_Unicode=ਝੂਠ;
                }
            } ਨਹੀਂਤਾਂ {

                /* we reach here only if targetUniChar == missingCharMarker
                 * so assign codes to reason and err
                 */
                *err = U_INVALID_CHAR_ਮਿਲਿਆ;
CALLBACK:
                ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[0] = ਸਰੋਤ_Char;
                ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 1;
                ਰੋਕੋ;
            }

        } ਨਹੀਂਤਾਂ {
            *err =U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }
    }

    ਜੇ (U_SUCCESS(*err) && ਦਲੀਲਾਂ_2->flush && ਸਰੋਤ == ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /* end of the input stream */
        UConverter *cnv = ਦਲੀਲਾਂ_2->converter;

        ਜੇ (*context_Char_ਮੰਜ਼ਿਲ_Unicode==ATR || *context_Char_ਮੰਜ਼ਿਲ_Unicode==EXT || *context_Char_ਮੰਜ਼ਿਲ_Unicode==ISCII_INV) {
            /* set toUBytes[] */
            cnv->ਮੰਜ਼ਿਲ_UBytes[0] = (uint8_t)*context_Char_ਮੰਜ਼ਿਲ_Unicode;
            cnv->ਮੰਜ਼ਿਲ_ULength = 1;

            /* avoid looping on truncated sequences */
            *context_Char_ਮੰਜ਼ਿਲ_Unicode = NO_CHAR_MARKER;
        } ਨਹੀਂਤਾਂ {
            cnv->ਮੰਜ਼ਿਲ_ULength = 0;
        }

        ਜੇ (*ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ != missingCharMarker) {
            /* output a remaining target character */
            WRITE_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_U(ਦਲੀਲਾਂ_2,ਸਰੋਤ,ਨਿਸ਼ਾਨਾ,ਦਲੀਲਾਂ_2->offsets,(ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ -1),*ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ,ਅੰਕੜੇ->current_Delta_ਮੰਜ਼ਿਲ_Unicode,err);
            *ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = missingCharMarker;
        }
    }

    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = ਸਰੋਤ;
}

/* structure for SafeClone calculations */
ਬਣਤਰ cloneISCIIStruct {
    UConverter cnv;
    UConverter_ਅੰਕੜੇ_ISCII mydata;
};

ਸਥਿਰ UConverter * U_CALLCONV
ISCII_ਸੁਰੱਖਿਅਤ_Clone(ਅਚਲ UConverter *cnv,
              ਖਾਲੀ *ਢੇਰ_ਬਫਰ,
              int32_t *p_ਬਫਰ_ਮਾਪ,
              UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਬਣਤਰ cloneISCIIStruct * localClone;
    int32_t ਬਫਰ_ਮਾਪ_Needed = ਆਕਾਰ(ਬਣਤਰ cloneISCIIStruct);

    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ (*p_ਬਫਰ_ਮਾਪ == 0) { /* 'preflighting' request - set needed size into *pBufferSize */
        *p_ਬਫਰ_ਮਾਪ = ਬਫਰ_ਮਾਪ_Needed;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    localClone = (ਬਣਤਰ cloneISCIIStruct *)ਢੇਰ_ਬਫਰ;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, ਆਕਾਰ(UConverter_ਅੰਕੜੇ_ISCII));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = ਸੱਚ;

    ਵਾਪਸ &localClone->cnv;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ISCIIGet_Unicode_ਸੈੱਟ(ਅਚਲ UConverter *cnv,
                    ਅਚਲ USetAdder *sa,
                    UConverter_Unicode_ਸੈੱਟ which,
                    UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    (ਖਾਲੀ)cnv;
    (ਖਾਲੀ)which;
    (ਖਾਲੀ)p_Error_ਕੋਡ;
    int32_t idx, script;
    uint8_t mask;

    /* Since all ISCII versions allow switching to other ISCII
    scripts, we add all roundtrippable characters to this set. */
    sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, 0, ASCII_ਸਿਰਾ);
    ਲਈ (script = DEVANAGARI; script <= MALAYALAM; script++) {
        mask = (uint8_t)(lookup_Initial_ਅੰਕੜੇ[script].ਨਕਾਬ_Enum_2);
        ਲਈ (idx = 0; idx < DELTA; idx++) {
            /* added check for TELUGU character */
            ਜੇ ((validity_ਸਾਰਣੀ[idx] & mask) || (script==TELUGU && idx==0x31)) {
                sa->ਜੋੜੋ(sa->ਸੈੱਟ, idx + (script * DELTA) + INDIC_ਖੰਡ_BEGIN);
            }
        }
    }
    sa->ਜੋੜੋ(sa->ਸੈੱਟ, DANDA);
    sa->ਜੋੜੋ(sa->ਸੈੱਟ, DOUBLE_DANDA);
    sa->ਜੋੜੋ(sa->ਸੈੱਟ, ZWNJ);
    sa->ਜੋੜੋ(sa->ਸੈੱਟ, ZWJ);
}
U_CDECL_ਸਿਰਾ
ਸਥਿਰ ਅਚਲ UConverterImpl _ISCIIImpl={

    UCNV_ISCII,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    _ISCIIOpen,
    _ISCIIClose,
    _ISCIIReset,

    UConverter_ਮੰਜ਼ਿਲ_Unicode_ISCII_OFFSETS_LOGIC,
    UConverter_ਮੰਜ਼ਿਲ_Unicode_ISCII_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISCII_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISCII_OFFSETS_LOGIC,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    _ISCIIgetName,
    ਖਾਲੀ_ਸੂਚਕ,
    ISCII_ਸੁਰੱਖਿਅਤ_Clone,
    ISCIIGet_Unicode_ਸੈੱਟ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};

ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ ISCIIStatic_ਅੰਕੜੇ={
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
        "ISCII",
         0,
         UCNV_IBM,
         UCNV_ISCII,
         1,
         4,
        { 0x1a, 0, 0, 0 },
        0x1,
        ਝੂਠ,
        ਝੂਠ,
        0x0,
        0x0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

ਅਚਲ UConverter_Shared_ਅੰਕੜੇ _ISCIIData=
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&ISCIIStatic_ਅੰਕੜੇ, &_ISCIIImpl);

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_LEGACY_CONVERSION */
