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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv_err.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv_cb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnvmbcs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    INVALID = -2,
    DO_ਖੋਜ = -1,

    COMPOUND_ਪਾਠ_SINGLE_0 = 0,
    COMPOUND_ਪਾਠ_SINGLE_1 = 1,
    COMPOUND_ਪਾਠ_SINGLE_2 = 2,
    COMPOUND_ਪਾਠ_SINGLE_3 = 3,

    COMPOUND_ਪਾਠ_DOUBLE_1 = 4,
    COMPOUND_ਪਾਠ_DOUBLE_2 = 5,
    COMPOUND_ਪਾਠ_DOUBLE_3 = 6,
    COMPOUND_ਪਾਠ_DOUBLE_4 = 7,
    COMPOUND_ਪਾਠ_DOUBLE_5 = 8,
    COMPOUND_ਪਾਠ_DOUBLE_6 = 9,
    COMPOUND_ਪਾਠ_DOUBLE_7 = 10,

    COMPOUND_ਪਾਠ_TRIPLE_DOUBLE = 11,

    IBM_915 = 12,
    IBM_916 = 13,
    IBM_914 = 14,
    IBM_874 = 15,
    IBM_912 = 16,
    IBM_913 = 17,
    ISO_8859_14 = 18,
    IBM_923 = 19,
    ਅੰਕ_ਦਾ_CONVERTERS = 20
} COMPOUND_ਪਾਠ_CONVERTERS;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_ਲੰਮਾਈ 12

ਸਥਿਰ ਅਚਲ uint8_t esc_Seq_Compound_ਪਾਠ[ਅੰਕ_ਦਾ_CONVERTERS][5] = {
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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ESC_ਸ਼ੁਰੂ 0x1B

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isASCIIRange(codepoint) \
        ((codepoint == 0x0000) || (codepoint == 0x0009) || (codepoint == 0x000A) || \
         (codepoint >= 0x0020 && codepoint <= 0x007f) || (codepoint >= 0x00A0 && codepoint <= 0x00FF))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isIBM915(codepoint) \
        ((codepoint >= 0x0401 && codepoint <= 0x045F) || (codepoint == 0x2116))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isIBM916(codepoint) \
        ((codepoint >= 0x05D0 && codepoint <= 0x05EA) || (codepoint == 0x2017) || (codepoint == 0x203E))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isCompoundS3(codepoint) \
        ((codepoint == 0x060C) || (codepoint == 0x061B) || (codepoint == 0x061F) || (codepoint >= 0x0621 && codepoint <= 0x063A) || \
         (codepoint >= 0x0640 && codepoint <= 0x0652) || (codepoint >= 0x0660 && codepoint <= 0x066D) || (codepoint == 0x200B) || \
         (codepoint >= 0x0FE70 && codepoint <= 0x0FE72) || (codepoint == 0x0FE74) || (codepoint >= 0x0FE76 && codepoint <= 0x0FEBE))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isCompoundS2(codepoint) \
        ((codepoint == 0x02BC) || (codepoint == 0x02BD) || (codepoint >= 0x0384 && codepoint <= 0x03CE) || (codepoint == 0x2015))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isIBM914(codepoint) \
        ((codepoint == 0x0100) || (codepoint == 0x0101) || (codepoint == 0x0112) || (codepoint == 0x0113) || (codepoint == 0x0116) || (codepoint == 0x0117) || \
         (codepoint == 0x0122) || (codepoint == 0x0123) || (codepoint >= 0x0128 && codepoint <= 0x012B) || (codepoint == 0x012E) || (codepoint == 0x012F) || \
         (codepoint >= 0x0136 && codepoint <= 0x0138) || (codepoint == 0x013B) || (codepoint == 0x013C) || (codepoint == 0x0145) || (codepoint ==  0x0146) || \
         (codepoint >= 0x014A && codepoint <= 0x014D) || (codepoint == 0x0156) || (codepoint == 0x0157) || (codepoint >= 0x0166 && codepoint <= 0x016B) || \
         (codepoint == 0x0172) || (codepoint == 0x0173))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isIBM874(codepoint) \
        ((codepoint >= 0x0E01 && codepoint <= 0x0E3A) || (codepoint >= 0x0E3F && codepoint <= 0x0E5B))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isIBM912(codepoint) \
        ((codepoint >= 0x0102 && codepoint <= 0x0107) || (codepoint >= 0x010C && codepoint <= 0x0111) || (codepoint >= 0x0118 && codepoint <= 0x011B) || \
         (codepoint == 0x0139) || (codepoint == 0x013A) || (codepoint == 0x013D) || (codepoint == 0x013E) || (codepoint >= 0x0141 && codepoint <= 0x0144) || \
         (codepoint == 0x0147) || (codepoint == 0x0147) || (codepoint == 0x0150) || (codepoint == 0x0151) || (codepoint == 0x0154) || (codepoint == 0x0155) || \
         (codepoint >= 0x0158 && codepoint <= 0x015B) || (codepoint == 0x015E) || (codepoint == 0x015F) || (codepoint >= 0x0160 && codepoint <= 0x0165) || \
         (codepoint == 0x016E) || (codepoint == 0x016F) || (codepoint == 0x0170) || (codepoint ==  0x0171) || (codepoint >= 0x0179 && codepoint <= 0x017E) || \
         (codepoint == 0x02C7) || (codepoint == 0x02D8) || (codepoint == 0x02D9) || (codepoint == 0x02DB) || (codepoint == 0x02DD))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isIBM913(codepoint) \
        ((codepoint >= 0x0108 && codepoint <= 0x010B) || (codepoint == 0x011C) || \
         (codepoint == 0x011D) || (codepoint == 0x0120) || (codepoint == 0x0121) || \
         (codepoint >= 0x0124 && codepoint <= 0x0127) || (codepoint == 0x0134) || (codepoint == 0x0135) || \
         (codepoint == 0x015C) || (codepoint == 0x015D) || (codepoint == 0x016C) || (codepoint ==  0x016D))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isCompoundS1(codepoint) \
        ((codepoint == 0x011E) || (codepoint == 0x011F) || (codepoint == 0x0130) || \
         (codepoint == 0x0131) || (codepoint >= 0x0218 && codepoint <= 0x021B))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isISO8859_14(codepoint) \
        ((codepoint >= 0x0174 && codepoint <= 0x0177) || (codepoint == 0x1E0A) || \
         (codepoint == 0x1E0B) || (codepoint == 0x1E1E) || (codepoint == 0x1E1F) || \
         (codepoint == 0x1E40) || (codepoint == 0x1E41) || (codepoint == 0x1E56) || \
         (codepoint == 0x1E57) || (codepoint == 0x1E60) || (codepoint == 0x1E61) || \
         (codepoint == 0x1E6A) || (codepoint == 0x1E6B) || (codepoint == 0x1EF2) || \
         (codepoint == 0x1EF3) || (codepoint >= 0x1E80 && codepoint <= 0x1E85))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isIBM923(codepoint) \
        ((codepoint == 0x0152) || (codepoint == 0x0153) || (codepoint == 0x0178) || (codepoint == 0x20AC))


ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ{
    UConverter_Shared_ਅੰਕੜੇ *myConverterArray[ਅੰਕ_ਦਾ_CONVERTERS];
    COMPOUND_ਪਾਠ_CONVERTERS state;
} UConverter_ਅੰਕੜੇ_Compound_ਪਾਠ;

/*********** Compound Text Converter Protos ***********/
U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
Compound_ਪਾਠ_Open(UConverter *cnv, UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, UError_ਕੋਡ *error_ਕੋਡ);

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
 Compound_ਪਾਠ_Close(UConverter *converter);

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
Compound_ਪਾਠ_Reset(UConverter *converter, UConverterResetChoice choice);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* U_CALLCONV
_CompoundTextgetName(ਅਚਲ UConverter* cnv);


ਸਥਿਰ int32_t ਲੱਭੋ_ਅਗਲਾ_Esc(ਅਚਲ ਅੱਖਰ *ਸਰੋਤ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
    int32_t length = static_cast<int32_t>(ਸਰੋਤ_ਹੱਦਬੰਦੀ - ਸਰੋਤ);
    int32_t i;
    ਲਈ (i = 1; i < length; i++) {
        ਜੇ (*(ਸਰੋਤ + i) == 0x1B) {
            ਵਾਪਸ i;
        }
    }

    ਵਾਪਸ length;
}

ਸਥਿਰ COMPOUND_ਪਾਠ_CONVERTERS getState(ਪੂਰਨ codepoint) {
    COMPOUND_ਪਾਠ_CONVERTERS state = DO_ਖੋਜ;

    ਜੇ (isASCIIRange(codepoint)) {
        state = COMPOUND_ਪਾਠ_SINGLE_0;
    } ਨਹੀਂਤਾਂ ਜੇ (isIBM912(codepoint)) {
        state = IBM_912;
    }ਨਹੀਂਤਾਂ ਜੇ (isIBM913(codepoint)) {
        state = IBM_913;
    } ਨਹੀਂਤਾਂ ਜੇ (isISO8859_14(codepoint)) {
        state = ISO_8859_14;
    } ਨਹੀਂਤਾਂ ਜੇ (isIBM923(codepoint)) {
        state = IBM_923;
    } ਨਹੀਂਤਾਂ ਜੇ (isIBM874(codepoint)) {
        state = IBM_874;
    } ਨਹੀਂਤਾਂ ਜੇ (isIBM914(codepoint)) {
        state = IBM_914;
    } ਨਹੀਂਤਾਂ ਜੇ (isCompoundS2(codepoint)) {
        state = COMPOUND_ਪਾਠ_SINGLE_2;
    } ਨਹੀਂਤਾਂ ਜੇ (isCompoundS3(codepoint)) {
        state = COMPOUND_ਪਾਠ_SINGLE_3;
    } ਨਹੀਂਤਾਂ ਜੇ (isIBM916(codepoint)) {
        state = IBM_916;
    } ਨਹੀਂਤਾਂ ਜੇ (isIBM915(codepoint)) {
        state = IBM_915;
    } ਨਹੀਂਤਾਂ ਜੇ (isCompoundS1(codepoint)) {
        state = COMPOUND_ਪਾਠ_SINGLE_1;
    }

    ਵਾਪਸ state;
}

ਸਥਿਰ COMPOUND_ਪਾਠ_CONVERTERS ਲੱਭੋ_State_From_Esc_Seq(ਅਚਲ ਅੱਖਰ* ਸਰੋਤ, ਅਚਲ ਅੱਖਰ* ਸਰੋਤ_ਹੱਦਬੰਦੀ, ਅਚਲ uint8_t* ਮੰਜ਼ਿਲ_UBytes_ਬਫਰ, int32_t ਮੰਜ਼ਿਲ_UBytes_ਬਫਰ_ਲੰਮਾਈ, UError_ਕੋਡ *err) {
    COMPOUND_ਪਾਠ_CONVERTERS state = INVALID;
    UBool ਮੇਲ_ਮਿਲਿਆ = ਝੂਠ;
    int32_t i, n, ਵਿੱਥ = ਮੰਜ਼ਿਲ_UBytes_ਬਫਰ_ਲੰਮਾਈ;

    ਲਈ (i = 0; i < ਅੰਕ_ਦਾ_CONVERTERS; i++) {
        ਮੇਲ_ਮਿਲਿਆ = ਸੱਚ;
        ਲਈ (n = 0; esc_Seq_Compound_ਪਾਠ[i][n] != 0; n++) {
            ਜੇ (n < ਮੰਜ਼ਿਲ_UBytes_ਬਫਰ_ਲੰਮਾਈ) {
                ਜੇ (ਮੰਜ਼ਿਲ_UBytes_ਬਫਰ[n] != esc_Seq_Compound_ਪਾਠ[i][n]) {
                    ਮੇਲ_ਮਿਲਿਆ = ਝੂਠ;
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ ((ਸਰੋਤ + (n - ਵਿੱਥ)) >= ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                *err = U_TRUNCATED_CHAR_ਮਿਲਿਆ;
                ਮੇਲ_ਮਿਲਿਆ = ਝੂਠ;
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ ਜੇ (*(ਸਰੋਤ + (n - ਵਿੱਥ)) != esc_Seq_Compound_ਪਾਠ[i][n]) {
                ਮੇਲ_ਮਿਲਿਆ = ਝੂਠ;
                ਰੋਕੋ;
            }
        }

        ਜੇ (ਮੇਲ_ਮਿਲਿਆ) {
            ਰੋਕੋ;
        }
    }

    ਜੇ (ਮੇਲ_ਮਿਲਿਆ) {
        state = (COMPOUND_ਪਾਠ_CONVERTERS)i;
    }

    ਵਾਪਸ state;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
Compound_ਪਾਠ_Open(UConverter *cnv, UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, UError_ਕੋਡ *error_ਕੋਡ){
    cnv->extraInfo = uprv_malloc (ਆਕਾਰ (UConverter_ਅੰਕੜੇ_Compound_ਪਾਠ));
    ਜੇ (cnv->extraInfo != ਖਾਲੀ_ਸੂਚਕ) {
        UConverter_ਅੰਕੜੇ_Compound_ਪਾਠ *my_Converter_ਅੰਕੜੇ = (UConverter_ਅੰਕੜੇ_Compound_ਪਾਠ *) cnv->extraInfo;

        UConverterNamePieces ਢੇਰ_Pieces;
        UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ ਢੇਰ_ਦਲੀਲਾਂ=UCNV_ਲੱਦੋ_ਦਲੀਲਾਂ_INITIALIZER;

        my_Converter_ਅੰਕੜੇ->myConverterArray[COMPOUND_ਪਾਠ_SINGLE_0] = ਖਾਲੀ_ਸੂਚਕ;
        my_Converter_ਅੰਕੜੇ->myConverterArray[COMPOUND_ਪਾਠ_SINGLE_1] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("icu-internal-compound-s1", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[COMPOUND_ਪਾਠ_SINGLE_2] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("icu-internal-compound-s2", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[COMPOUND_ਪਾਠ_SINGLE_3] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("icu-internal-compound-s3", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[COMPOUND_ਪਾਠ_DOUBLE_1] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("icu-internal-compound-d1", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[COMPOUND_ਪਾਠ_DOUBLE_2] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("icu-internal-compound-d2", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[COMPOUND_ਪਾਠ_DOUBLE_3] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("icu-internal-compound-d3", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[COMPOUND_ਪਾਠ_DOUBLE_4] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("icu-internal-compound-d4", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[COMPOUND_ਪਾਠ_DOUBLE_5] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("icu-internal-compound-d5", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[COMPOUND_ਪਾਠ_DOUBLE_6] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("icu-internal-compound-d6", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[COMPOUND_ਪਾਠ_DOUBLE_7] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("icu-internal-compound-d7", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[COMPOUND_ਪਾਠ_TRIPLE_DOUBLE] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("icu-internal-compound-t", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);

        my_Converter_ਅੰਕੜੇ->myConverterArray[IBM_915] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("ibm-915_P100-1995", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[IBM_916] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("ibm-916_P100-1995", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[IBM_914] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("ibm-914_P100-1995", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[IBM_874] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("ibm-874_P100-1995", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[IBM_912] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("ibm-912_P100-1995", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[IBM_913] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("ibm-913_P100-2000", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[ISO_8859_14] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("iso-8859_14-1998", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
        my_Converter_ਅੰਕੜੇ->myConverterArray[IBM_923] = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("ibm-923_P100-1998", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);

        ਜੇ (U_FAILURE(*error_ਕੋਡ) || p_ਦਲੀਲਾਂ->onlyTestIsLoadable) {
            Compound_ਪਾਠ_Close(cnv);
            ਵਾਪਸ;
        }

        my_Converter_ਅੰਕੜੇ->state = (COMPOUND_ਪਾਠ_CONVERTERS)0;
    } ਨਹੀਂਤਾਂ {
        *error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
}


ਸਥਿਰ ਖਾਲੀ U_CALLCONV
Compound_ਪਾਠ_Close(UConverter *converter) {
    UConverter_ਅੰਕੜੇ_Compound_ਪਾਠ* my_Converter_ਅੰਕੜੇ = (UConverter_ਅੰਕੜੇ_Compound_ਪਾਠ*)(converter->extraInfo);
    int32_t i;

    ਜੇ (converter->extraInfo != ਖਾਲੀ_ਸੂਚਕ) {
        /*close the array of converter pointers and free the memory*/
        ਲਈ (i = 0; i < ਅੰਕ_ਦਾ_CONVERTERS; i++) {
            ਜੇ (my_Converter_ਅੰਕੜੇ->myConverterArray[i] != ਖਾਲੀ_ਸੂਚਕ) {
                ucnv_unload_Shared_ਅੰਕੜੇ_If_Ready(my_Converter_ਅੰਕੜੇ->myConverterArray[i]);
            }
        }

        uprv_free(converter->extraInfo);
        converter->extraInfo = ਖਾਲੀ_ਸੂਚਕ;
    }
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
Compound_ਪਾਠ_Reset(UConverter *converter, UConverterResetChoice choice) {
    (ਖਾਲੀ)converter;
    (ਖਾਲੀ)choice;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* U_CALLCONV
_CompoundTextgetName(ਅਚਲ UConverter* cnv){
    (ਖਾਲੀ)cnv;
    ਵਾਪਸ "x11-compound-text";
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
UConverter_from_Unicode_Compound_ਪਾਠ_OFFSETS(UConverter_From_Unicode_ਦਲੀਲਾਂ* ਦਲੀਲਾਂ_2, UError_ਕੋਡ* err){
    UConverter *cnv = ਦਲੀਲਾਂ_2->converter;
    uint8_t *ਨਿਸ਼ਾਨਾ = (uint8_t *) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    ਅਚਲ uint8_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = (ਅਚਲ uint8_t *) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    ਅਚਲ char16_t* ਸਰੋਤ = ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    ਅਚਲ char16_t* ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    /* int32_t* offsets = args->offsets; */
    UChar32 ਸਰੋਤ_Char;
    UBool useFallback = cnv->useFallback;
    uint8_t tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ[7];
    int32_t tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ_ਲੰਮਾਈ = 0;
    COMPOUND_ਪਾਠ_CONVERTERS currentState, tmpState;
    uint32_t p_ਮੁੱਲ;
    int32_t p_ਮੁੱਲ_ਲੰਮਾਈ = 0;
    int32_t i, n, j;

    UConverter_ਅੰਕੜੇ_Compound_ਪਾਠ *my_Converter_ਅੰਕੜੇ = (UConverter_ਅੰਕੜੇ_Compound_ਪਾਠ *) cnv->extraInfo;

    currentState = my_Converter_ਅੰਕੜੇ->state;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    ਜੇ((ਸਰੋਤ_Char = cnv->fromUChar32)!=0 && ਨਿਸ਼ਾਨਾ< ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        ਜਾਓ getTrail;
    }

    ਜਦੋਂ( ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ){
        ਜੇ(ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ){

            ਸਰੋਤ_Char  = *(ਸਰੋਤ++);
            /*check if the char is a First surrogate*/
             ਜੇ(U16_IS_SURROGATE(ਸਰੋਤ_Char)) {
                ਜੇ(U16_IS_SURROGATE_LEAD(ਸਰੋਤ_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    ਜੇ(ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                        /* test the following code unit */
                        char16_t trail = *ਸਰੋਤ;
                        ਜੇ(U16_IS_TRAIL(trail)) {
                            ਸਰੋਤ++;
                            ਸਰੋਤ_Char=U16_GET_SUPPLEMENTARY(ਸਰੋਤ_Char, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } ਨਹੀਂਤਾਂ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                            cnv->fromUChar32=ਸਰੋਤ_Char;
                            ਰੋਕੋ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* no more input */
                        cnv->fromUChar32=ਸਰੋਤ_Char;
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    cnv->fromUChar32=ਸਰੋਤ_Char;
                    ਰੋਕੋ;
                }
            }

             tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ_ਲੰਮਾਈ = 0;
             tmpState = getState(ਸਰੋਤ_Char);

             ਜੇ (tmpState != DO_ਖੋਜ && currentState != tmpState) {
                 /* Get escape sequence if necessary */
                 currentState = tmpState;
                 ਲਈ (i = 0; esc_Seq_Compound_ਪਾਠ[currentState][i] != 0; i++) {
                     tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ[tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ_ਲੰਮਾਈ++] = esc_Seq_Compound_ਪਾਠ[currentState][i];
                 }
             }

             ਜੇ (tmpState == DO_ਖੋਜ) {
                 /* Test all available converters */
                 ਲਈ (i = 1; i < ਖੋਜ_ਲੰਮਾਈ; i++) {
                     p_ਮੁੱਲ_ਲੰਮਾਈ = ucnv_MBCSFromUChar32(my_Converter_ਅੰਕੜੇ->myConverterArray[i], ਸਰੋਤ_Char, &p_ਮੁੱਲ, useFallback);
                     ਜੇ (p_ਮੁੱਲ_ਲੰਮਾਈ > 0) {
                         tmpState = (COMPOUND_ਪਾਠ_CONVERTERS)i;
                         ਜੇ (currentState != tmpState) {
                             currentState = tmpState;
                             ਲਈ (j = 0; esc_Seq_Compound_ਪਾਠ[currentState][j] != 0; j++) {
                                 tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ[tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ_ਲੰਮਾਈ++] = esc_Seq_Compound_ਪਾਠ[currentState][j];
                             }
                         }
                         ਲਈ (n = (p_ਮੁੱਲ_ਲੰਮਾਈ - 1); n >= 0; n--) {
                             tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ[tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ_ਲੰਮਾਈ++] = (uint8_t)(p_ਮੁੱਲ >> (n * 8));
                         }
                         ਰੋਕੋ;
                     }
                 }
             } ਨਹੀਂਤਾਂ ਜੇ (tmpState == COMPOUND_ਪਾਠ_SINGLE_0) {
                 tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ[tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ_ਲੰਮਾਈ++] = (uint8_t)ਸਰੋਤ_Char;
             } ਨਹੀਂਤਾਂ {
                 p_ਮੁੱਲ_ਲੰਮਾਈ = ucnv_MBCSFromUChar32(my_Converter_ਅੰਕੜੇ->myConverterArray[currentState], ਸਰੋਤ_Char, &p_ਮੁੱਲ, useFallback);
                 ਜੇ (p_ਮੁੱਲ_ਲੰਮਾਈ > 0) {
                     ਲਈ (n = (p_ਮੁੱਲ_ਲੰਮਾਈ - 1); n >= 0; n--) {
                         tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ[tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ_ਲੰਮਾਈ++] = (uint8_t)(p_ਮੁੱਲ >> (n * 8));
                     }
                 }
             }

             ਲਈ (i = 0; i < tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ_ਲੰਮਾਈ; i++) {
                 ਜੇ (ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                     *ਨਿਸ਼ਾਨਾ++ = tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ[i];
                 } ਨਹੀਂਤਾਂ {
                     *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                     ਰੋਕੋ;
                 }
             }

             ਜੇ (*err == U_ਬਫਰ_ਉਛਾਲ_ERROR) {
                 ਲਈ (; i < tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ_ਲੰਮਾਈ; i++) {
                     ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] = tmp_ਨਿਸ਼ਾਨਾ_ਬਫਰ[i];
                 }
             }
        } ਨਹੀਂਤਾਂ {
            *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }
    }

    /*save the state and return */
    my_Converter_ਅੰਕੜੇ->state = currentState;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = ਸਰੋਤ;
    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = (ਅੱਖਰ*)ਨਿਸ਼ਾਨਾ;
}


ਸਥਿਰ ਖਾਲੀ U_CALLCONV
UConverter_ਮੰਜ਼ਿਲ_Unicode_Compound_ਪਾਠ_OFFSETS(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2,
                                               UError_ਕੋਡ* err){
    ਅਚਲ ਅੱਖਰ *my_ਸਰੋਤ = (ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    char16_t *my_ਨਿਸ਼ਾਨਾ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    ਅਚਲ ਅੱਖਰ *my_ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਅਚਲ ਅੱਖਰ *tmp_ਸਰੋਤ_ਹੱਦਬੰਦੀ = my_ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint32_t my_ਸਰੋਤ_Char = 0x0000;
    COMPOUND_ਪਾਠ_CONVERTERS currentState, tmpState;
    int32_t ਸਰੋਤ_ਵਿੱਥ = 0;
    UConverter_ਅੰਕੜੇ_Compound_ਪਾਠ *my_Converter_ਅੰਕੜੇ = (UConverter_ਅੰਕੜੇ_Compound_ਪਾਠ *) ਦਲੀਲਾਂ_2->converter->extraInfo;
    UConverter_Shared_ਅੰਕੜੇ* saved_Shared_ਅੰਕੜੇ = ਖਾਲੀ_ਸੂਚਕ;

    UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ sub_ਦਲੀਲਾਂ;
    int32_t min_ਦਲੀਲਾਂ_ਮਾਪ;

    /* set up the subconverter arguments */
    ਜੇ(ਦਲੀਲਾਂ_2->size<ਆਕਾਰ(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ)) {
        min_ਦਲੀਲਾਂ_ਮਾਪ = ਦਲੀਲਾਂ_2->size;
    } ਨਹੀਂਤਾਂ {
        min_ਦਲੀਲਾਂ_ਮਾਪ = (int32_t)ਆਕਾਰ(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ);
    }

    uprv_memcpy(&sub_ਦਲੀਲਾਂ, ਦਲੀਲਾਂ_2, min_ਦਲੀਲਾਂ_ਮਾਪ);
    sub_ਦਲੀਲਾਂ.size = (uint16_t)min_ਦਲੀਲਾਂ_ਮਾਪ;

    currentState = tmpState =  my_Converter_ਅੰਕੜੇ->state;

    ਜਦੋਂ(my_ਸਰੋਤ < my_ਸਰੋਤ_ਹੱਦਬੰਦੀ){
        ਜੇ(my_ਨਿਸ਼ਾਨਾ < ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ){
            ਜੇ (ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength > 0) {
                my_ਸਰੋਤ_Char = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[0];
            } ਨਹੀਂਤਾਂ {
                my_ਸਰੋਤ_Char = (uint8_t)*my_ਸਰੋਤ;
            }

            ਜੇ (my_ਸਰੋਤ_Char == ESC_ਸ਼ੁਰੂ) {
                tmpState = ਲੱਭੋ_State_From_Esc_Seq(my_ਸਰੋਤ, my_ਸਰੋਤ_ਹੱਦਬੰਦੀ, ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes, ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength, err);

                ਜੇ (*err == U_TRUNCATED_CHAR_ਮਿਲਿਆ) {
                    ਲਈ (; my_ਸਰੋਤ < my_ਸਰੋਤ_ਹੱਦਬੰਦੀ;) {
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength++] = *my_ਸਰੋਤ++;
                    }
                    *err = U_ZERO_ERROR;
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ (tmpState == INVALID) {
                    ਜੇ (ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength == 0) {
                        my_ਸਰੋਤ++; /* skip over the 0x1b byte */
                    }
                    *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    ਰੋਕੋ;
                }

                ਜੇ (tmpState != currentState) {
                    currentState = tmpState;
                }

                ਸਰੋਤ_ਵਿੱਥ = static_cast<int32_t>(uprv_strlen((ਅੱਖਰ*)esc_Seq_Compound_ਪਾਠ[currentState]) - ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength);

                my_ਸਰੋਤ += ਸਰੋਤ_ਵਿੱਥ;

                ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 0;
            }

            ਜੇ (currentState == COMPOUND_ਪਾਠ_SINGLE_0) {
                ਜਦੋਂ (my_ਸਰੋਤ < my_ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                    ਜੇ (*my_ਸਰੋਤ == ESC_ਸ਼ੁਰੂ) {
                        ਰੋਕੋ;
                    }
                    ਜੇ (my_ਨਿਸ਼ਾਨਾ < ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                        *my_ਨਿਸ਼ਾਨਾ++ = 0x00ff&(*my_ਸਰੋਤ++);
                    } ਨਹੀਂਤਾਂ {
                        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                        ਰੋਕੋ;
                    }
                }
            } ਨਹੀਂਤਾਂ ਜੇ (my_ਸਰੋਤ < my_ਸਰੋਤ_ਹੱਦਬੰਦੀ){
                ਸਰੋਤ_ਵਿੱਥ = ਲੱਭੋ_ਅਗਲਾ_Esc(my_ਸਰੋਤ, my_ਸਰੋਤ_ਹੱਦਬੰਦੀ);

                tmp_ਸਰੋਤ_ਹੱਦਬੰਦੀ = my_ਸਰੋਤ + ਸਰੋਤ_ਵਿੱਥ;

                sub_ਦਲੀਲਾਂ.ਸਰੋਤ = my_ਸਰੋਤ;
                sub_ਦਲੀਲਾਂ.ਸਰੋਤ_ਹੱਦਬੰਦੀ = tmp_ਸਰੋਤ_ਹੱਦਬੰਦੀ;
                sub_ਦਲੀਲਾਂ.ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
                saved_Shared_ਅੰਕੜੇ = sub_ਦਲੀਲਾਂ.converter->shared_ਅੰਕੜੇ;
                sub_ਦਲੀਲਾਂ.converter->shared_ਅੰਕੜੇ = my_Converter_ਅੰਕੜੇ->myConverterArray[currentState];

                ucnv_MBCSToUnicodeWithOffsets(&sub_ਦਲੀਲਾਂ, err);

                sub_ਦਲੀਲਾਂ.converter->shared_ਅੰਕੜੇ = saved_Shared_ਅੰਕੜੇ;

                my_ਸਰੋਤ = sub_ਦਲੀਲਾਂ.ਸਰੋਤ;
                my_ਨਿਸ਼ਾਨਾ = sub_ਦਲੀਲਾਂ.ਨਿਸ਼ਾਨਾ;

                ਜੇ (U_FAILURE(*err)) {
                    ਜੇ(*err == U_ਬਫਰ_ਉਛਾਲ_ERROR) {
                        ਜੇ(sub_ਦਲੀਲਾਂ.converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ > 0) {
                            uprv_memcpy(ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ, sub_ਦਲੀਲਾਂ.converter->UChar_Error_ਬਫਰ,
                                        sub_ਦਲੀਲਾਂ.converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ);
                        }
                        ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=sub_ਦਲੀਲਾਂ.converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ;
                        sub_ਦਲੀਲਾਂ.converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ = 0;
                    }
                    ਰੋਕੋ;
                }
            }
        } ਨਹੀਂਤਾਂ {
            *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }
    }
    my_Converter_ਅੰਕੜੇ->state = currentState;
    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
Compound_ਪਾਠ_Get_Unicode_ਸੈੱਟ(ਅਚਲ UConverter *cnv,
                    ਅਚਲ USetAdder *sa,
                    UConverter_Unicode_ਸੈੱਟ which,
                    UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter_ਅੰਕੜੇ_Compound_ਪਾਠ *my_Converter_ਅੰਕੜੇ = (UConverter_ਅੰਕੜੇ_Compound_ਪਾਠ *)cnv->extraInfo;
    int32_t i;

    ਲਈ (i = 1; i < ਅੰਕ_ਦਾ_CONVERTERS; i++) {
        ucnv_MBCSGet_Unicode_ਸੈੱਟ_For_Unicode(my_Converter_ਅੰਕੜੇ->myConverterArray[i], sa, which, p_Error_ਕੋਡ);
    }
    sa->ਜੋੜੋ(sa->ਸੈੱਟ, 0x0000);
    sa->ਜੋੜੋ(sa->ਸੈੱਟ, 0x0009);
    sa->ਜੋੜੋ(sa->ਸੈੱਟ, 0x000A);
    sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, 0x0020, 0x007F);
    sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, 0x00A0, 0x00FF);
}
U_CDECL_ਸਿਰਾ

ਸਥਿਰ ਅਚਲ UConverterImpl Compound_ਪਾਠ_Impl = {

    UCNV_COMPOUND_ਪਾਠ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    Compound_ਪਾਠ_Open,
    Compound_ਪਾਠ_Close,
    Compound_ਪਾਠ_Reset,

    UConverter_ਮੰਜ਼ਿਲ_Unicode_Compound_ਪਾਠ_OFFSETS,
    UConverter_ਮੰਜ਼ਿਲ_Unicode_Compound_ਪਾਠ_OFFSETS,
    UConverter_from_Unicode_Compound_ਪਾਠ_OFFSETS,
    UConverter_from_Unicode_Compound_ਪਾਠ_OFFSETS,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    _CompoundTextgetName,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    Compound_ਪਾਠ_Get_Unicode_ਸੈੱਟ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};

ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ Compound_ਪਾਠ_Static_ਅੰਕੜੇ = {
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "COMPOUND_TEXT",
    0,
    UCNV_IBM,
    UCNV_COMPOUND_ਪਾਠ,
    1,
    6,
    { 0xef, 0, 0, 0 },
    1,
    ਝੂਠ,
    ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};
ਅਚਲ UConverter_Shared_ਅੰਕੜੇ Compound_ਪਾਠ_ਅੰਕੜੇ =
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&Compound_ਪਾਠ_Static_ਅੰਕੜੇ, &Compound_ਪਾਠ_Impl);

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_LEGACY_CONVERSION */
