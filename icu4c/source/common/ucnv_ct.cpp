// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2010-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  ucnv_ct.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010Dec09
*   created by: Michael Ow
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv_err.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv_cb.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "ucnv_imp.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "ucnvmbcs.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    INVALID = -2,
    DO_ಹುಡುಕು = -1,

    COMPOUND_ಪಠ್ಯ_SINGLE_0 = 0,
    COMPOUND_ಪಠ್ಯ_SINGLE_1 = 1,
    COMPOUND_ಪಠ್ಯ_SINGLE_2 = 2,
    COMPOUND_ಪಠ್ಯ_SINGLE_3 = 3,

    COMPOUND_ಪಠ್ಯ_DOUBLE_1 = 4,
    COMPOUND_ಪಠ್ಯ_DOUBLE_2 = 5,
    COMPOUND_ಪಠ್ಯ_DOUBLE_3 = 6,
    COMPOUND_ಪಠ್ಯ_DOUBLE_4 = 7,
    COMPOUND_ಪಠ್ಯ_DOUBLE_5 = 8,
    COMPOUND_ಪಠ್ಯ_DOUBLE_6 = 9,
    COMPOUND_ಪಠ್ಯ_DOUBLE_7 = 10,

    COMPOUND_ಪಠ್ಯ_TRIPLE_DOUBLE = 11,

    IBM_915 = 12,
    IBM_916 = 13,
    IBM_914 = 14,
    IBM_874 = 15,
    IBM_912 = 16,
    IBM_913 = 17,
    ISO_8859_14 = 18,
    IBM_923 = 19,
    ಅಂಕೆ_ದ_CONVERTERS = 20
} COMPOUND_ಪಠ್ಯ_CONVERTERS;

#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_ಉದ್ದಳತೆ 12

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t esc_Seq_Compound_ಪಠ್ಯ[ಅಂಕೆ_ದ_CONVERTERS][5] = {
    /* Single */
    { 0x1B, 0x2D, 0x41, 0, 0 },
    { 0x1B, 0x2D, 0x4D, 0, 0 },
    { 0x1B, 0x2D, 0x46, 0, 0 },
    { 0x1B, 0x2D, 0x47, 0, 0 },

    /* Double */
    { 0x1B, 0x24, 0x29, 0x41, 0 },
    { 0x1B, 0x24, 0x29, 0x42, 0 },
    { 0x1B, 0x24, 0x29, 0x43, 0 },
    { 0x1B, 0x24, 0x29, 0x44, 0 },
    { 0x1B, 0x24, 0x29, 0x47, 0 },
    { 0x1B, 0x24, 0x29, 0x48, 0 },
    { 0x1B, 0x24, 0x29, 0x49, 0 },

    /* Triple/Double */
    { 0x1B, 0x25, 0x47, 0, 0 },

    /*IBM-915*/
    { 0x1B, 0x2D, 0x4C, 0, 0 },
    /*IBM-916*/
    { 0x1B, 0x2D, 0x48, 0, 0 },
    /*IBM-914*/
    { 0x1B, 0x2D, 0x44, 0, 0 },
    /*IBM-874*/
    { 0x1B, 0x2D, 0x54, 0, 0 },
    /*IBM-912*/
    { 0x1B, 0x2D, 0x42, 0, 0 },
    /* IBM-913 */
    { 0x1B, 0x2D, 0x43, 0, 0 },
    /* ISO-8859_14 */
    { 0x1B, 0x2D, 0x5F, 0, 0 },
    /* IBM-923 */
    { 0x1B, 0x2D, 0x62, 0, 0 },
};

#ವ್ಯಾಖ್ಯಾನಿಸು ESC_ಆರಂಭ 0x1B

#ವ್ಯಾಖ್ಯಾನಿಸು isASCIIRange(codepoint) \
        ((codepoint == 0x0000) || (codepoint == 0x0009) || (codepoint == 0x000A) || \
         (codepoint >= 0x0020 && codepoint <= 0x007f) || (codepoint >= 0x00A0 && codepoint <= 0x00FF))

#ವ್ಯಾಖ್ಯಾನಿಸು isIBM915(codepoint) \
        ((codepoint >= 0x0401 && codepoint <= 0x045F) || (codepoint == 0x2116))

#ವ್ಯಾಖ್ಯಾನಿಸು isIBM916(codepoint) \
        ((codepoint >= 0x05D0 && codepoint <= 0x05EA) || (codepoint == 0x2017) || (codepoint == 0x203E))

#ವ್ಯಾಖ್ಯಾನಿಸು isCompoundS3(codepoint) \
        ((codepoint == 0x060C) || (codepoint == 0x061B) || (codepoint == 0x061F) || (codepoint >= 0x0621 && codepoint <= 0x063A) || \
         (codepoint >= 0x0640 && codepoint <= 0x0652) || (codepoint >= 0x0660 && codepoint <= 0x066D) || (codepoint == 0x200B) || \
         (codepoint >= 0x0FE70 && codepoint <= 0x0FE72) || (codepoint == 0x0FE74) || (codepoint >= 0x0FE76 && codepoint <= 0x0FEBE))

#ವ್ಯಾಖ್ಯಾನಿಸು isCompoundS2(codepoint) \
        ((codepoint == 0x02BC) || (codepoint == 0x02BD) || (codepoint >= 0x0384 && codepoint <= 0x03CE) || (codepoint == 0x2015))

#ವ್ಯಾಖ್ಯಾನಿಸು isIBM914(codepoint) \
        ((codepoint == 0x0100) || (codepoint == 0x0101) || (codepoint == 0x0112) || (codepoint == 0x0113) || (codepoint == 0x0116) || (codepoint == 0x0117) || \
         (codepoint == 0x0122) || (codepoint == 0x0123) || (codepoint >= 0x0128 && codepoint <= 0x012B) || (codepoint == 0x012E) || (codepoint == 0x012F) || \
         (codepoint >= 0x0136 && codepoint <= 0x0138) || (codepoint == 0x013B) || (codepoint == 0x013C) || (codepoint == 0x0145) || (codepoint ==  0x0146) || \
         (codepoint >= 0x014A && codepoint <= 0x014D) || (codepoint == 0x0156) || (codepoint == 0x0157) || (codepoint >= 0x0166 && codepoint <= 0x016B) || \
         (codepoint == 0x0172) || (codepoint == 0x0173))

#ವ್ಯಾಖ್ಯಾನಿಸು isIBM874(codepoint) \
        ((codepoint >= 0x0E01 && codepoint <= 0x0E3A) || (codepoint >= 0x0E3F && codepoint <= 0x0E5B))

#ವ್ಯಾಖ್ಯಾನಿಸು isIBM912(codepoint) \
        ((codepoint >= 0x0102 && codepoint <= 0x0107) || (codepoint >= 0x010C && codepoint <= 0x0111) || (codepoint >= 0x0118 && codepoint <= 0x011B) || \
         (codepoint == 0x0139) || (codepoint == 0x013A) || (codepoint == 0x013D) || (codepoint == 0x013E) || (codepoint >= 0x0141 && codepoint <= 0x0144) || \
         (codepoint == 0x0147) || (codepoint == 0x0147) || (codepoint == 0x0150) || (codepoint == 0x0151) || (codepoint == 0x0154) || (codepoint == 0x0155) || \
         (codepoint >= 0x0158 && codepoint <= 0x015B) || (codepoint == 0x015E) || (codepoint == 0x015F) || (codepoint >= 0x0160 && codepoint <= 0x0165) || \
         (codepoint == 0x016E) || (codepoint == 0x016F) || (codepoint == 0x0170) || (codepoint ==  0x0171) || (codepoint >= 0x0179 && codepoint <= 0x017E) || \
         (codepoint == 0x02C7) || (codepoint == 0x02D8) || (codepoint == 0x02D9) || (codepoint == 0x02DB) || (codepoint == 0x02DD))

#ವ್ಯಾಖ್ಯಾನಿಸು isIBM913(codepoint) \
        ((codepoint >= 0x0108 && codepoint <= 0x010B) || (codepoint == 0x011C) || \
         (codepoint == 0x011D) || (codepoint == 0x0120) || (codepoint == 0x0121) || \
         (codepoint >= 0x0124 && codepoint <= 0x0127) || (codepoint == 0x0134) || (codepoint == 0x0135) || \
         (codepoint == 0x015C) || (codepoint == 0x015D) || (codepoint == 0x016C) || (codepoint ==  0x016D))

#ವ್ಯಾಖ್ಯಾನಿಸು isCompoundS1(codepoint) \
        ((codepoint == 0x011E) || (codepoint == 0x011F) || (codepoint == 0x0130) || \
         (codepoint == 0x0131) || (codepoint >= 0x0218 && codepoint <= 0x021B))

#ವ್ಯಾಖ್ಯಾನಿಸು isISO8859_14(codepoint) \
        ((codepoint >= 0x0174 && codepoint <= 0x0177) || (codepoint == 0x1E0A) || \
         (codepoint == 0x1E0B) || (codepoint == 0x1E1E) || (codepoint == 0x1E1F) || \
         (codepoint == 0x1E40) || (codepoint == 0x1E41) || (codepoint == 0x1E56) || \
         (codepoint == 0x1E57) || (codepoint == 0x1E60) || (codepoint == 0x1E61) || \
         (codepoint == 0x1E6A) || (codepoint == 0x1E6B) || (codepoint == 0x1EF2) || \
         (codepoint == 0x1EF3) || (codepoint >= 0x1E80 && codepoint <= 0x1E85))

#ವ್ಯಾಖ್ಯಾನಿಸು isIBM923(codepoint) \
        ((codepoint == 0x0152) || (codepoint == 0x0153) || (codepoint == 0x0178) || (codepoint == 0x20AC))


ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ{
    UConverter_Shared_ದತ್ತಾಂಶ *myConverterArray[ಅಂಕೆ_ದ_CONVERTERS];
    COMPOUND_ಪಠ್ಯ_CONVERTERS state;
} UConverter_ದತ್ತಾಂಶ_Compound_ಪಠ್ಯ;

/*********** Compound Text Converter Protos ***********/
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
Compound_ಪಠ್ಯ_Open(UConverter *cnv, UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು, UError_ಸಂಕೇತ *error_ಸಂಕೇತ);

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
 Compound_ಪಠ್ಯ_Close(UConverter *converter);

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
Compound_ಪಠ್ಯ_Reset(UConverter *converter, UConverterResetChoice choice);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_CALLCONV
_CompoundTextgetName(ಸ್ಥಿರಾಂಕ UConverter* cnv);


ಸ್ಥಿರ int32_t ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_Esc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ_ಎಲ್ಲೆ) {
    int32_t length = static_cast<int32_t>(ಮೂಲ_ಎಲ್ಲೆ - ಮೂಲ);
    int32_t i;
    ಗಾಗಿ (i = 1; i < length; i++) {
        ಆದರೆ (*(ಮೂಲ + i) == 0x1B) {
            ಹಿಂತಿರುಗಿ i;
        }
    }

    ಹಿಂತಿರುಗಿ length;
}

ಸ್ಥಿರ COMPOUND_ಪಠ್ಯ_CONVERTERS getState(ಪೂರ್ಣಾಂಕ codepoint) {
    COMPOUND_ಪಠ್ಯ_CONVERTERS state = DO_ಹುಡುಕು;

    ಆದರೆ (isASCIIRange(codepoint)) {
        state = COMPOUND_ಪಠ್ಯ_SINGLE_0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isIBM912(codepoint)) {
        state = IBM_912;
    }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isIBM913(codepoint)) {
        state = IBM_913;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isISO8859_14(codepoint)) {
        state = ISO_8859_14;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isIBM923(codepoint)) {
        state = IBM_923;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isIBM874(codepoint)) {
        state = IBM_874;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isIBM914(codepoint)) {
        state = IBM_914;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isCompoundS2(codepoint)) {
        state = COMPOUND_ಪಠ್ಯ_SINGLE_2;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isCompoundS3(codepoint)) {
        state = COMPOUND_ಪಠ್ಯ_SINGLE_3;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isIBM916(codepoint)) {
        state = IBM_916;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isIBM915(codepoint)) {
        state = IBM_915;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isCompoundS1(codepoint)) {
        state = COMPOUND_ಪಠ್ಯ_SINGLE_1;
    }

    ಹಿಂತಿರುಗಿ state;
}

ಸ್ಥಿರ COMPOUND_ಪಠ್ಯ_CONVERTERS ಕಂಡುಹಿಡಿ_State_From_Esc_Seq(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮೂಲ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮೂಲ_ಎಲ್ಲೆ, ಸ್ಥಿರಾಂಕ uint8_t* ಗಮ್ಯ_UBytes_ಬಫರ್, int32_t ಗಮ್ಯ_UBytes_ಬಫರ್_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ *err) {
    COMPOUND_ಪಠ್ಯ_CONVERTERS state = INVALID;
    UBool ಹೊಂದಿಕೆ_ಸಿಕ್ಕಿತು = ಸುಳ್ಳು;
    int32_t i, n, ಸ್ಥಾನಾಂತರ = ಗಮ್ಯ_UBytes_ಬಫರ್_ಉದ್ದಳತೆ;

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_ದ_CONVERTERS; i++) {
        ಹೊಂದಿಕೆ_ಸಿಕ್ಕಿತು = ನಿಜ;
        ಗಾಗಿ (n = 0; esc_Seq_Compound_ಪಠ್ಯ[i][n] != 0; n++) {
            ಆದರೆ (n < ಗಮ್ಯ_UBytes_ಬಫರ್_ಉದ್ದಳತೆ) {
                ಆದರೆ (ಗಮ್ಯ_UBytes_ಬಫರ್[n] != esc_Seq_Compound_ಪಠ್ಯ[i][n]) {
                    ಹೊಂದಿಕೆ_ಸಿಕ್ಕಿತು = ಸುಳ್ಳು;
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ಮೂಲ + (n - ಸ್ಥಾನಾಂತರ)) >= ಮೂಲ_ಎಲ್ಲೆ) {
                *err = U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು;
                ಹೊಂದಿಕೆ_ಸಿಕ್ಕಿತು = ಸುಳ್ಳು;
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*(ಮೂಲ + (n - ಸ್ಥಾನಾಂತರ)) != esc_Seq_Compound_ಪಠ್ಯ[i][n]) {
                ಹೊಂದಿಕೆ_ಸಿಕ್ಕಿತು = ಸುಳ್ಳು;
                ನಿಲ್ಲಿಸು;
            }
        }

        ಆದರೆ (ಹೊಂದಿಕೆ_ಸಿಕ್ಕಿತು) {
            ನಿಲ್ಲಿಸು;
        }
    }

    ಆದರೆ (ಹೊಂದಿಕೆ_ಸಿಕ್ಕಿತು) {
        state = (COMPOUND_ಪಠ್ಯ_CONVERTERS)i;
    }

    ಹಿಂತಿರುಗಿ state;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
Compound_ಪಠ್ಯ_Open(UConverter *cnv, UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು, UError_ಸಂಕೇತ *error_ಸಂಕೇತ){
    cnv->extraInfo = uprv_malloc (ಗಾತ್ರ (UConverter_ದತ್ತಾಂಶ_Compound_ಪಠ್ಯ));
    ಆದರೆ (cnv->extraInfo != ಶೂನ್ಯಸೂಚಕ) {
        UConverter_ದತ್ತಾಂಶ_Compound_ಪಠ್ಯ *my_Converter_ದತ್ತಾಂಶ = (UConverter_ದತ್ತಾಂಶ_Compound_ಪಠ್ಯ *) cnv->extraInfo;

        UConverterNamePieces ರಾಶಿ_Pieces;
        UConverter_ತುಂಬು_ವಾದಗಳು ರಾಶಿ_ವಾದಗಳು=UCNV_ತುಂಬು_ವಾದಗಳು_INITIALIZER;

        my_Converter_ದತ್ತಾಂಶ->myConverterArray[COMPOUND_ಪಠ್ಯ_SINGLE_0] = ಶೂನ್ಯಸೂಚಕ;
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[COMPOUND_ಪಠ್ಯ_SINGLE_1] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("icu-internal-compound-s1", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[COMPOUND_ಪಠ್ಯ_SINGLE_2] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("icu-internal-compound-s2", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[COMPOUND_ಪಠ್ಯ_SINGLE_3] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("icu-internal-compound-s3", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[COMPOUND_ಪಠ್ಯ_DOUBLE_1] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("icu-internal-compound-d1", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[COMPOUND_ಪಠ್ಯ_DOUBLE_2] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("icu-internal-compound-d2", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[COMPOUND_ಪಠ್ಯ_DOUBLE_3] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("icu-internal-compound-d3", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[COMPOUND_ಪಠ್ಯ_DOUBLE_4] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("icu-internal-compound-d4", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[COMPOUND_ಪಠ್ಯ_DOUBLE_5] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("icu-internal-compound-d5", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[COMPOUND_ಪಠ್ಯ_DOUBLE_6] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("icu-internal-compound-d6", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[COMPOUND_ಪಠ್ಯ_DOUBLE_7] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("icu-internal-compound-d7", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[COMPOUND_ಪಠ್ಯ_TRIPLE_DOUBLE] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("icu-internal-compound-t", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);

        my_Converter_ದತ್ತಾಂಶ->myConverterArray[IBM_915] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("ibm-915_P100-1995", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[IBM_916] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("ibm-916_P100-1995", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[IBM_914] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("ibm-914_P100-1995", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[IBM_874] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("ibm-874_P100-1995", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[IBM_912] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("ibm-912_P100-1995", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[IBM_913] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("ibm-913_P100-2000", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[ISO_8859_14] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("iso-8859_14-1998", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
        my_Converter_ದತ್ತಾಂಶ->myConverterArray[IBM_923] = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("ibm-923_P100-1998", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);

        ಆದರೆ (U_FAILURE(*error_ಸಂಕೇತ) || p_ವಾದಗಳು->onlyTestIsLoadable) {
            Compound_ಪಠ್ಯ_Close(cnv);
            ಹಿಂತಿರುಗಿ;
        }

        my_Converter_ದತ್ತಾಂಶ->state = (COMPOUND_ಪಠ್ಯ_CONVERTERS)0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        *error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
}


ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
Compound_ಪಠ್ಯ_Close(UConverter *converter) {
    UConverter_ದತ್ತಾಂಶ_Compound_ಪಠ್ಯ* my_Converter_ದತ್ತಾಂಶ = (UConverter_ದತ್ತಾಂಶ_Compound_ಪಠ್ಯ*)(converter->extraInfo);
    int32_t i;

    ಆದರೆ (converter->extraInfo != ಶೂನ್ಯಸೂಚಕ) {
        /*close the array of converter pointers and free the memory*/
        ಗಾಗಿ (i = 0; i < ಅಂಕೆ_ದ_CONVERTERS; i++) {
            ಆದರೆ (my_Converter_ದತ್ತಾಂಶ->myConverterArray[i] != ಶೂನ್ಯಸೂಚಕ) {
                ucnv_unload_Shared_ದತ್ತಾಂಶ_If_Ready(my_Converter_ದತ್ತಾಂಶ->myConverterArray[i]);
            }
        }

        uprv_free(converter->extraInfo);
        converter->extraInfo = ಶೂನ್ಯಸೂಚಕ;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
Compound_ಪಠ್ಯ_Reset(UConverter *converter, UConverterResetChoice choice) {
    (ಶೂನ್ಯ)converter;
    (ಶೂನ್ಯ)choice;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_CALLCONV
_CompoundTextgetName(ಸ್ಥಿರಾಂಕ UConverter* cnv){
    (ಶೂನ್ಯ)cnv;
    ಹಿಂತಿರುಗಿ "x11-compound-text";
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
UConverter_from_Unicode_Compound_ಪಠ್ಯ_OFFSETS(UConverter_From_Unicode_ವಾದಗಳು* ವಾದಗಳು_2, UError_ಸಂಕೇತ* err){
    UConverter *cnv = ವಾದಗಳು_2->converter;
    uint8_t *ಗುರಿ = (uint8_t *) ವಾದಗಳು_2->ಗುರಿ;
    ಸ್ಥಿರಾಂಕ uint8_t *ಗುರಿ_ಎಲ್ಲೆ = (ಸ್ಥಿರಾಂಕ uint8_t *) ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ;
    ಸ್ಥಿರಾಂಕ char16_t* ಮೂಲ = ವಾದಗಳು_2->ಮೂಲ;
    ಸ್ಥಿರಾಂಕ char16_t* ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    /* int32_t* offsets = args->offsets; */
    UChar32 ಮೂಲ_Char;
    UBool useFallback = cnv->useFallback;
    uint8_t tmp_ಗುರಿ_ಬಫರ್[7];
    int32_t tmp_ಗುರಿ_ಬಫರ್_ಉದ್ದಳತೆ = 0;
    COMPOUND_ಪಠ್ಯ_CONVERTERS currentState, tmpState;
    uint32_t p_ಮೌಲ್ಯ;
    int32_t p_ಮೌಲ್ಯ_ಉದ್ದಳತೆ = 0;
    int32_t i, n, j;

    UConverter_ದತ್ತಾಂಶ_Compound_ಪಠ್ಯ *my_Converter_ದತ್ತಾಂಶ = (UConverter_ದತ್ತಾಂಶ_Compound_ಪಠ್ಯ *) cnv->extraInfo;

    currentState = my_Converter_ದತ್ತಾಂಶ->state;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    ಆದರೆ((ಮೂಲ_Char = cnv->fromUChar32)!=0 && ಗುರಿ< ಗುರಿ_ಎಲ್ಲೆ) {
        ಹೋಗು getTrail;
    }

    ಆಗ( ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ){
        ಆದರೆ(ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ){

            ಮೂಲ_Char  = *(ಮೂಲ++);
            /*check if the char is a First surrogate*/
             ಆದರೆ(U16_IS_SURROGATE(ಮೂಲ_Char)) {
                ಆದರೆ(U16_IS_SURROGATE_LEAD(ಮೂಲ_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    ಆದರೆ(ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ) {
                        /* test the following code unit */
                        char16_t trail = *ಮೂಲ;
                        ಆದರೆ(U16_IS_TRAIL(trail)) {
                            ಮೂಲ++;
                            ಮೂಲ_Char=U16_GET_SUPPLEMENTARY(ಮೂಲ_Char, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                            cnv->fromUChar32=ಮೂಲ_Char;
                            ನಿಲ್ಲಿಸು;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* no more input */
                        cnv->fromUChar32=ಮೂಲ_Char;
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    cnv->fromUChar32=ಮೂಲ_Char;
                    ನಿಲ್ಲಿಸು;
                }
            }

             tmp_ಗುರಿ_ಬಫರ್_ಉದ್ದಳತೆ = 0;
             tmpState = getState(ಮೂಲ_Char);

             ಆದರೆ (tmpState != DO_ಹುಡುಕು && currentState != tmpState) {
                 /* Get escape sequence if necessary */
                 currentState = tmpState;
                 ಗಾಗಿ (i = 0; esc_Seq_Compound_ಪಠ್ಯ[currentState][i] != 0; i++) {
                     tmp_ಗುರಿ_ಬಫರ್[tmp_ಗುರಿ_ಬಫರ್_ಉದ್ದಳತೆ++] = esc_Seq_Compound_ಪಠ್ಯ[currentState][i];
                 }
             }

             ಆದರೆ (tmpState == DO_ಹುಡುಕು) {
                 /* Test all available converters */
                 ಗಾಗಿ (i = 1; i < ಹುಡುಕು_ಉದ್ದಳತೆ; i++) {
                     p_ಮೌಲ್ಯ_ಉದ್ದಳತೆ = ucnv_MBCSFromUChar32(my_Converter_ದತ್ತಾಂಶ->myConverterArray[i], ಮೂಲ_Char, &p_ಮೌಲ್ಯ, useFallback);
                     ಆದರೆ (p_ಮೌಲ್ಯ_ಉದ್ದಳತೆ > 0) {
                         tmpState = (COMPOUND_ಪಠ್ಯ_CONVERTERS)i;
                         ಆದರೆ (currentState != tmpState) {
                             currentState = tmpState;
                             ಗಾಗಿ (j = 0; esc_Seq_Compound_ಪಠ್ಯ[currentState][j] != 0; j++) {
                                 tmp_ಗುರಿ_ಬಫರ್[tmp_ಗುರಿ_ಬಫರ್_ಉದ್ದಳತೆ++] = esc_Seq_Compound_ಪಠ್ಯ[currentState][j];
                             }
                         }
                         ಗಾಗಿ (n = (p_ಮೌಲ್ಯ_ಉದ್ದಳತೆ - 1); n >= 0; n--) {
                             tmp_ಗುರಿ_ಬಫರ್[tmp_ಗುರಿ_ಬಫರ್_ಉದ್ದಳತೆ++] = (uint8_t)(p_ಮೌಲ್ಯ >> (n * 8));
                         }
                         ನಿಲ್ಲಿಸು;
                     }
                 }
             } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tmpState == COMPOUND_ಪಠ್ಯ_SINGLE_0) {
                 tmp_ಗುರಿ_ಬಫರ್[tmp_ಗುರಿ_ಬಫರ್_ಉದ್ದಳತೆ++] = (uint8_t)ಮೂಲ_Char;
             } ಇಲ್ಲದಿದ್ದರೆ {
                 p_ಮೌಲ್ಯ_ಉದ್ದಳತೆ = ucnv_MBCSFromUChar32(my_Converter_ದತ್ತಾಂಶ->myConverterArray[currentState], ಮೂಲ_Char, &p_ಮೌಲ್ಯ, useFallback);
                 ಆದರೆ (p_ಮೌಲ್ಯ_ಉದ್ದಳತೆ > 0) {
                     ಗಾಗಿ (n = (p_ಮೌಲ್ಯ_ಉದ್ದಳತೆ - 1); n >= 0; n--) {
                         tmp_ಗುರಿ_ಬಫರ್[tmp_ಗುರಿ_ಬಫರ್_ಉದ್ದಳತೆ++] = (uint8_t)(p_ಮೌಲ್ಯ >> (n * 8));
                     }
                 }
             }

             ಗಾಗಿ (i = 0; i < tmp_ಗುರಿ_ಬಫರ್_ಉದ್ದಳತೆ; i++) {
                 ಆದರೆ (ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ) {
                     *ಗುರಿ++ = tmp_ಗುರಿ_ಬಫರ್[i];
                 } ಇಲ್ಲದಿದ್ದರೆ {
                     *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                     ನಿಲ್ಲಿಸು;
                 }
             }

             ಆದರೆ (*err == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
                 ಗಾಗಿ (; i < tmp_ಗುರಿ_ಬಫರ್_ಉದ್ದಳತೆ; i++) {
                     ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] = tmp_ಗುರಿ_ಬಫರ್[i];
                 }
             }
        } ಇಲ್ಲದಿದ್ದರೆ {
            *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
    }

    /*save the state and return */
    my_Converter_ದತ್ತಾಂಶ->state = currentState;
    ವಾದಗಳು_2->ಮೂಲ = ಮೂಲ;
    ವಾದಗಳು_2->ಗುರಿ = (ಅಕ್ಷರ*)ಗುರಿ;
}


ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
UConverter_ಗಮ್ಯ_Unicode_Compound_ಪಠ್ಯ_OFFSETS(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *ವಾದಗಳು_2,
                                               UError_ಸಂಕೇತ* err){
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *my_ಮೂಲ = (ಅಕ್ಷರ *) ವಾದಗಳು_2->ಮೂಲ;
    char16_t *my_ಗುರಿ = ವಾದಗಳು_2->ಗುರಿ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *my_ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tmp_ಮೂಲ_ಎಲ್ಲೆ = my_ಮೂಲ_ಎಲ್ಲೆ;
    uint32_t my_ಮೂಲ_Char = 0x0000;
    COMPOUND_ಪಠ್ಯ_CONVERTERS currentState, tmpState;
    int32_t ಮೂಲ_ಸ್ಥಾನಾಂತರ = 0;
    UConverter_ದತ್ತಾಂಶ_Compound_ಪಠ್ಯ *my_Converter_ದತ್ತಾಂಶ = (UConverter_ದತ್ತಾಂಶ_Compound_ಪಠ್ಯ *) ವಾದಗಳು_2->converter->extraInfo;
    UConverter_Shared_ದತ್ತಾಂಶ* saved_Shared_ದತ್ತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;

    UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು sub_ವಾದಗಳು;
    int32_t min_ವಾದಗಳು_ಗಾತ್ರ;

    /* set up the subconverter arguments */
    ಆದರೆ(ವಾದಗಳು_2->size<ಗಾತ್ರ(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು)) {
        min_ವಾದಗಳು_ಗಾತ್ರ = ವಾದಗಳು_2->size;
    } ಇಲ್ಲದಿದ್ದರೆ {
        min_ವಾದಗಳು_ಗಾತ್ರ = (int32_t)ಗಾತ್ರ(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು);
    }

    uprv_memcpy(&sub_ವಾದಗಳು, ವಾದಗಳು_2, min_ವಾದಗಳು_ಗಾತ್ರ);
    sub_ವಾದಗಳು.size = (uint16_t)min_ವಾದಗಳು_ಗಾತ್ರ;

    currentState = tmpState =  my_Converter_ದತ್ತಾಂಶ->state;

    ಆಗ(my_ಮೂಲ < my_ಮೂಲ_ಎಲ್ಲೆ){
        ಆದರೆ(my_ಗುರಿ < ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ){
            ಆದರೆ (ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength > 0) {
                my_ಮೂಲ_Char = ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[0];
            } ಇಲ್ಲದಿದ್ದರೆ {
                my_ಮೂಲ_Char = (uint8_t)*my_ಮೂಲ;
            }

            ಆದರೆ (my_ಮೂಲ_Char == ESC_ಆರಂಭ) {
                tmpState = ಕಂಡುಹಿಡಿ_State_From_Esc_Seq(my_ಮೂಲ, my_ಮೂಲ_ಎಲ್ಲೆ, ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes, ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength, err);

                ಆದರೆ (*err == U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು) {
                    ಗಾಗಿ (; my_ಮೂಲ < my_ಮೂಲ_ಎಲ್ಲೆ;) {
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength++] = *my_ಮೂಲ++;
                    }
                    *err = U_ZERO_ERROR;
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tmpState == INVALID) {
                    ಆದರೆ (ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength == 0) {
                        my_ಮೂಲ++; /* skip over the 0x1b byte */
                    }
                    *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    ನಿಲ್ಲಿಸು;
                }

                ಆದರೆ (tmpState != currentState) {
                    currentState = tmpState;
                }

                ಮೂಲ_ಸ್ಥಾನಾಂತರ = static_cast<int32_t>(uprv_strlen((ಅಕ್ಷರ*)esc_Seq_Compound_ಪಠ್ಯ[currentState]) - ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength);

                my_ಮೂಲ += ಮೂಲ_ಸ್ಥಾನಾಂತರ;

                ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 0;
            }

            ಆದರೆ (currentState == COMPOUND_ಪಠ್ಯ_SINGLE_0) {
                ಆಗ (my_ಮೂಲ < my_ಮೂಲ_ಎಲ್ಲೆ) {
                    ಆದರೆ (*my_ಮೂಲ == ESC_ಆರಂಭ) {
                        ನಿಲ್ಲಿಸು;
                    }
                    ಆದರೆ (my_ಗುರಿ < ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ) {
                        *my_ಗುರಿ++ = 0x00ff&(*my_ಮೂಲ++);
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                        ನಿಲ್ಲಿಸು;
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (my_ಮೂಲ < my_ಮೂಲ_ಎಲ್ಲೆ){
                ಮೂಲ_ಸ್ಥಾನಾಂತರ = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_Esc(my_ಮೂಲ, my_ಮೂಲ_ಎಲ್ಲೆ);

                tmp_ಮೂಲ_ಎಲ್ಲೆ = my_ಮೂಲ + ಮೂಲ_ಸ್ಥಾನಾಂತರ;

                sub_ವಾದಗಳು.ಮೂಲ = my_ಮೂಲ;
                sub_ವಾದಗಳು.ಮೂಲ_ಎಲ್ಲೆ = tmp_ಮೂಲ_ಎಲ್ಲೆ;
                sub_ವಾದಗಳು.ಗುರಿ = my_ಗುರಿ;
                saved_Shared_ದತ್ತಾಂಶ = sub_ವಾದಗಳು.converter->shared_ದತ್ತಾಂಶ;
                sub_ವಾದಗಳು.converter->shared_ದತ್ತಾಂಶ = my_Converter_ದತ್ತಾಂಶ->myConverterArray[currentState];

                ucnv_MBCSToUnicodeWithOffsets(&sub_ವಾದಗಳು, err);

                sub_ವಾದಗಳು.converter->shared_ದತ್ತಾಂಶ = saved_Shared_ದತ್ತಾಂಶ;

                my_ಮೂಲ = sub_ವಾದಗಳು.ಮೂಲ;
                my_ಗುರಿ = sub_ವಾದಗಳು.ಗುರಿ;

                ಆದರೆ (U_FAILURE(*err)) {
                    ಆದರೆ(*err == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
                        ಆದರೆ(sub_ವಾದಗಳು.converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ > 0) {
                            uprv_memcpy(ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್, sub_ವಾದಗಳು.converter->UChar_Error_ಬಫರ್,
                                        sub_ವಾದಗಳು.converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ);
                        }
                        ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=sub_ವಾದಗಳು.converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ;
                        sub_ವಾದಗಳು.converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ = 0;
                    }
                    ನಿಲ್ಲಿಸು;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
    }
    my_Converter_ದತ್ತಾಂಶ->state = currentState;
    ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
    ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
Compound_ಪಠ್ಯ_Get_Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UConverter *cnv,
                    ಸ್ಥಿರಾಂಕ USetAdder *sa,
                    UConverter_Unicode_ಗಣ which,
                    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter_ದತ್ತಾಂಶ_Compound_ಪಠ್ಯ *my_Converter_ದತ್ತಾಂಶ = (UConverter_ದತ್ತಾಂಶ_Compound_ಪಠ್ಯ *)cnv->extraInfo;
    int32_t i;

    ಗಾಗಿ (i = 1; i < ಅಂಕೆ_ದ_CONVERTERS; i++) {
        ucnv_MBCSGet_Unicode_ಗಣ_For_Unicode(my_Converter_ದತ್ತಾಂಶ->myConverterArray[i], sa, which, p_Error_ಸಂಕೇತ);
    }
    sa->ಕೂಡಿಸು(sa->ಗಣ, 0x0000);
    sa->ಕೂಡಿಸು(sa->ಗಣ, 0x0009);
    sa->ಕೂಡಿಸು(sa->ಗಣ, 0x000A);
    sa->ಕೂಡಿಸು_Range(sa->ಗಣ, 0x0020, 0x007F);
    sa->ಕೂಡಿಸು_Range(sa->ಗಣ, 0x00A0, 0x00FF);
}
U_CDECL_ಕೊನೆ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl Compound_ಪಠ್ಯ_Impl = {

    UCNV_COMPOUND_ಪಠ್ಯ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    Compound_ಪಠ್ಯ_Open,
    Compound_ಪಠ್ಯ_Close,
    Compound_ಪಠ್ಯ_Reset,

    UConverter_ಗಮ್ಯ_Unicode_Compound_ಪಠ್ಯ_OFFSETS,
    UConverter_ಗಮ್ಯ_Unicode_Compound_ಪಠ್ಯ_OFFSETS,
    UConverter_from_Unicode_Compound_ಪಠ್ಯ_OFFSETS,
    UConverter_from_Unicode_Compound_ಪಠ್ಯ_OFFSETS,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    _CompoundTextgetName,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    Compound_ಪಠ್ಯ_Get_Unicode_ಗಣ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ Compound_ಪಠ್ಯ_Static_ದತ್ತಾಂಶ = {
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "COMPOUND_TEXT",
    0,
    UCNV_IBM,
    UCNV_COMPOUND_ಪಠ್ಯ,
    1,
    6,
    { 0xef, 0, 0, 0 },
    1,
    ಸುಳ್ಳು,
    ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};
ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ Compound_ಪಠ್ಯ_ದತ್ತಾಂಶ =
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&Compound_ಪಠ್ಯ_Static_ದತ್ತಾಂಶ, &Compound_ಪಠ್ಯ_Impl);

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_LEGACY_CONVERSION */
