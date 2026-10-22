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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ضمن "unicode/ucnv.h"
#ضمن "unicode/uset.h"
#ضمن "unicode/ucnv_err.h"
#ضمن "unicode/ucnv_cb.h"
#ضمن "unicode/utf16.h"
#ضمن "ucnv_imp.h"
#ضمن "ucnv_bld.h"
#ضمن "ucnv_cnv.h"
#ضمن "ucnvmbcs.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"

تعريفنوع تعداد {
    INVALID = -2,
    DO_تلاش = -1,

    COMPOUND_متن_SINGLE_0 = 0,
    COMPOUND_متن_SINGLE_1 = 1,
    COMPOUND_متن_SINGLE_2 = 2,
    COMPOUND_متن_SINGLE_3 = 3,

    COMPOUND_متن_DOUBLE_1 = 4,
    COMPOUND_متن_DOUBLE_2 = 5,
    COMPOUND_متن_DOUBLE_3 = 6,
    COMPOUND_متن_DOUBLE_4 = 7,
    COMPOUND_متن_DOUBLE_5 = 8,
    COMPOUND_متن_DOUBLE_6 = 9,
    COMPOUND_متن_DOUBLE_7 = 10,

    COMPOUND_متن_TRIPLE_DOUBLE = 11,

    IBM_915 = 12,
    IBM_916 = 13,
    IBM_914 = 14,
    IBM_874 = 15,
    IBM_912 = 16,
    IBM_913 = 17,
    ISO_8859_14 = 18,
    IBM_923 = 19,
    عدد_دا_CONVERTERS = 20
} COMPOUND_متن_CONVERTERS;

#تعریف_کرو تلاش_لمائی 12

ساكن ثابت uint8_t esc_Seq_Compound_متن[عدد_دا_CONVERTERS][5] = {
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

#تعریف_کرو ESC_شروع 0x1B

#تعریف_کرو isASCIIRange(codepoint) \
        ((codepoint == 0x0000) || (codepoint == 0x0009) || (codepoint == 0x000A) || \
         (codepoint >= 0x0020 && codepoint <= 0x007f) || (codepoint >= 0x00A0 && codepoint <= 0x00FF))

#تعریف_کرو isIBM915(codepoint) \
        ((codepoint >= 0x0401 && codepoint <= 0x045F) || (codepoint == 0x2116))

#تعریف_کرو isIBM916(codepoint) \
        ((codepoint >= 0x05D0 && codepoint <= 0x05EA) || (codepoint == 0x2017) || (codepoint == 0x203E))

#تعریف_کرو isCompoundS3(codepoint) \
        ((codepoint == 0x060C) || (codepoint == 0x061B) || (codepoint == 0x061F) || (codepoint >= 0x0621 && codepoint <= 0x063A) || \
         (codepoint >= 0x0640 && codepoint <= 0x0652) || (codepoint >= 0x0660 && codepoint <= 0x066D) || (codepoint == 0x200B) || \
         (codepoint >= 0x0FE70 && codepoint <= 0x0FE72) || (codepoint == 0x0FE74) || (codepoint >= 0x0FE76 && codepoint <= 0x0FEBE))

#تعریف_کرو isCompoundS2(codepoint) \
        ((codepoint == 0x02BC) || (codepoint == 0x02BD) || (codepoint >= 0x0384 && codepoint <= 0x03CE) || (codepoint == 0x2015))

#تعریف_کرو isIBM914(codepoint) \
        ((codepoint == 0x0100) || (codepoint == 0x0101) || (codepoint == 0x0112) || (codepoint == 0x0113) || (codepoint == 0x0116) || (codepoint == 0x0117) || \
         (codepoint == 0x0122) || (codepoint == 0x0123) || (codepoint >= 0x0128 && codepoint <= 0x012B) || (codepoint == 0x012E) || (codepoint == 0x012F) || \
         (codepoint >= 0x0136 && codepoint <= 0x0138) || (codepoint == 0x013B) || (codepoint == 0x013C) || (codepoint == 0x0145) || (codepoint ==  0x0146) || \
         (codepoint >= 0x014A && codepoint <= 0x014D) || (codepoint == 0x0156) || (codepoint == 0x0157) || (codepoint >= 0x0166 && codepoint <= 0x016B) || \
         (codepoint == 0x0172) || (codepoint == 0x0173))

#تعریف_کرو isIBM874(codepoint) \
        ((codepoint >= 0x0E01 && codepoint <= 0x0E3A) || (codepoint >= 0x0E3F && codepoint <= 0x0E5B))

#تعریف_کرو isIBM912(codepoint) \
        ((codepoint >= 0x0102 && codepoint <= 0x0107) || (codepoint >= 0x010C && codepoint <= 0x0111) || (codepoint >= 0x0118 && codepoint <= 0x011B) || \
         (codepoint == 0x0139) || (codepoint == 0x013A) || (codepoint == 0x013D) || (codepoint == 0x013E) || (codepoint >= 0x0141 && codepoint <= 0x0144) || \
         (codepoint == 0x0147) || (codepoint == 0x0147) || (codepoint == 0x0150) || (codepoint == 0x0151) || (codepoint == 0x0154) || (codepoint == 0x0155) || \
         (codepoint >= 0x0158 && codepoint <= 0x015B) || (codepoint == 0x015E) || (codepoint == 0x015F) || (codepoint >= 0x0160 && codepoint <= 0x0165) || \
         (codepoint == 0x016E) || (codepoint == 0x016F) || (codepoint == 0x0170) || (codepoint ==  0x0171) || (codepoint >= 0x0179 && codepoint <= 0x017E) || \
         (codepoint == 0x02C7) || (codepoint == 0x02D8) || (codepoint == 0x02D9) || (codepoint == 0x02DB) || (codepoint == 0x02DD))

#تعریف_کرو isIBM913(codepoint) \
        ((codepoint >= 0x0108 && codepoint <= 0x010B) || (codepoint == 0x011C) || \
         (codepoint == 0x011D) || (codepoint == 0x0120) || (codepoint == 0x0121) || \
         (codepoint >= 0x0124 && codepoint <= 0x0127) || (codepoint == 0x0134) || (codepoint == 0x0135) || \
         (codepoint == 0x015C) || (codepoint == 0x015D) || (codepoint == 0x016C) || (codepoint ==  0x016D))

#تعریف_کرو isCompoundS1(codepoint) \
        ((codepoint == 0x011E) || (codepoint == 0x011F) || (codepoint == 0x0130) || \
         (codepoint == 0x0131) || (codepoint >= 0x0218 && codepoint <= 0x021B))

#تعریف_کرو isISO8859_14(codepoint) \
        ((codepoint >= 0x0174 && codepoint <= 0x0177) || (codepoint == 0x1E0A) || \
         (codepoint == 0x1E0B) || (codepoint == 0x1E1E) || (codepoint == 0x1E1F) || \
         (codepoint == 0x1E40) || (codepoint == 0x1E41) || (codepoint == 0x1E56) || \
         (codepoint == 0x1E57) || (codepoint == 0x1E60) || (codepoint == 0x1E61) || \
         (codepoint == 0x1E6A) || (codepoint == 0x1E6B) || (codepoint == 0x1EF2) || \
         (codepoint == 0x1EF3) || (codepoint >= 0x1E80 && codepoint <= 0x1E85))

#تعریف_کرو isIBM923(codepoint) \
        ((codepoint == 0x0152) || (codepoint == 0x0153) || (codepoint == 0x0178) || (codepoint == 0x20AC))


تعريفنوع بنية{
    UConverter_Shared_انکڑے *myConverterArray[عدد_دا_CONVERTERS];
    COMPOUND_متن_CONVERTERS state;
} UConverter_انکڑے_Compound_متن;

/*********** Compound Text Converter Protos ***********/
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
Compound_متن_Open(UConverter *cnv, UConverter_لدو_دلیلاں *p_دلیلاں, UError_کوڈ *error_کوڈ);

ساكن فراغ U_CALLCONV
 Compound_متن_Close(UConverter *converter);

ساكن فراغ U_CALLCONV
Compound_متن_Reset(UConverter *converter, UConverterResetChoice choice);

ساكن ثابت حرف* U_CALLCONV
_CompoundTextgetName(ثابت UConverter* cnv);


ساكن int32_t لبھو_اگلا_Esc(ثابت حرف *سروت, ثابت حرف *سروت_حد_بندی) {
    int32_t length = static_cast<int32_t>(سروت_حد_بندی - سروت);
    int32_t i;
    لكل (i = 1; i < length; i++) {
        إذا (*(سروت + i) == 0x1B) {
            أرجع i;
        }
    }

    أرجع length;
}

ساكن COMPOUND_متن_CONVERTERS getState(صحيح codepoint) {
    COMPOUND_متن_CONVERTERS state = DO_تلاش;

    إذا (isASCIIRange(codepoint)) {
        state = COMPOUND_متن_SINGLE_0;
    } وإلا إذا (isIBM912(codepoint)) {
        state = IBM_912;
    }وإلا إذا (isIBM913(codepoint)) {
        state = IBM_913;
    } وإلا إذا (isISO8859_14(codepoint)) {
        state = ISO_8859_14;
    } وإلا إذا (isIBM923(codepoint)) {
        state = IBM_923;
    } وإلا إذا (isIBM874(codepoint)) {
        state = IBM_874;
    } وإلا إذا (isIBM914(codepoint)) {
        state = IBM_914;
    } وإلا إذا (isCompoundS2(codepoint)) {
        state = COMPOUND_متن_SINGLE_2;
    } وإلا إذا (isCompoundS3(codepoint)) {
        state = COMPOUND_متن_SINGLE_3;
    } وإلا إذا (isIBM916(codepoint)) {
        state = IBM_916;
    } وإلا إذا (isIBM915(codepoint)) {
        state = IBM_915;
    } وإلا إذا (isCompoundS1(codepoint)) {
        state = COMPOUND_متن_SINGLE_1;
    }

    أرجع state;
}

ساكن COMPOUND_متن_CONVERTERS لبھو_State_From_Esc_Seq(ثابت حرف* سروت, ثابت حرف* سروت_حد_بندی, ثابت uint8_t* منزل_UBytes_بفر, int32_t منزل_UBytes_بفر_لمائی, UError_کوڈ *err) {
    COMPOUND_متن_CONVERTERS state = INVALID;
    UBool میل_لبھیا = خطأ;
    int32_t i, n, وتھ = منزل_UBytes_بفر_لمائی;

    لكل (i = 0; i < عدد_دا_CONVERTERS; i++) {
        میل_لبھیا = سچ;
        لكل (n = 0; esc_Seq_Compound_متن[i][n] != 0; n++) {
            إذا (n < منزل_UBytes_بفر_لمائی) {
                إذا (منزل_UBytes_بفر[n] != esc_Seq_Compound_متن[i][n]) {
                    میل_لبھیا = خطأ;
                    اكسر;
                }
            } وإلا إذا ((سروت + (n - وتھ)) >= سروت_حد_بندی) {
                *err = U_TRUNCATED_CHAR_لبھیا;
                میل_لبھیا = خطأ;
                اكسر;
            } وإلا إذا (*(سروت + (n - وتھ)) != esc_Seq_Compound_متن[i][n]) {
                میل_لبھیا = خطأ;
                اكسر;
            }
        }

        إذا (میل_لبھیا) {
            اكسر;
        }
    }

    إذا (میل_لبھیا) {
        state = (COMPOUND_متن_CONVERTERS)i;
    }

    أرجع state;
}

ساكن فراغ U_CALLCONV
Compound_متن_Open(UConverter *cnv, UConverter_لدو_دلیلاں *p_دلیلاں, UError_کوڈ *error_کوڈ){
    cnv->extraInfo = uprv_malloc (حجم (UConverter_انکڑے_Compound_متن));
    إذا (cnv->extraInfo != خالی_سوچک) {
        UConverter_انکڑے_Compound_متن *my_Converter_انکڑے = (UConverter_انکڑے_Compound_متن *) cnv->extraInfo;

        UConverterNamePieces ڈھیر_Pieces;
        UConverter_لدو_دلیلاں ڈھیر_دلیلاں=UCNV_لدو_دلیلاں_INITIALIZER;

        my_Converter_انکڑے->myConverterArray[COMPOUND_متن_SINGLE_0] = خالی_سوچک;
        my_Converter_انکڑے->myConverterArray[COMPOUND_متن_SINGLE_1] = ucnv_لدو_Shared_انکڑے("icu-internal-compound-s1", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[COMPOUND_متن_SINGLE_2] = ucnv_لدو_Shared_انکڑے("icu-internal-compound-s2", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[COMPOUND_متن_SINGLE_3] = ucnv_لدو_Shared_انکڑے("icu-internal-compound-s3", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[COMPOUND_متن_DOUBLE_1] = ucnv_لدو_Shared_انکڑے("icu-internal-compound-d1", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[COMPOUND_متن_DOUBLE_2] = ucnv_لدو_Shared_انکڑے("icu-internal-compound-d2", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[COMPOUND_متن_DOUBLE_3] = ucnv_لدو_Shared_انکڑے("icu-internal-compound-d3", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[COMPOUND_متن_DOUBLE_4] = ucnv_لدو_Shared_انکڑے("icu-internal-compound-d4", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[COMPOUND_متن_DOUBLE_5] = ucnv_لدو_Shared_انکڑے("icu-internal-compound-d5", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[COMPOUND_متن_DOUBLE_6] = ucnv_لدو_Shared_انکڑے("icu-internal-compound-d6", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[COMPOUND_متن_DOUBLE_7] = ucnv_لدو_Shared_انکڑے("icu-internal-compound-d7", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[COMPOUND_متن_TRIPLE_DOUBLE] = ucnv_لدو_Shared_انکڑے("icu-internal-compound-t", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);

        my_Converter_انکڑے->myConverterArray[IBM_915] = ucnv_لدو_Shared_انکڑے("ibm-915_P100-1995", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[IBM_916] = ucnv_لدو_Shared_انکڑے("ibm-916_P100-1995", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[IBM_914] = ucnv_لدو_Shared_انکڑے("ibm-914_P100-1995", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[IBM_874] = ucnv_لدو_Shared_انکڑے("ibm-874_P100-1995", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[IBM_912] = ucnv_لدو_Shared_انکڑے("ibm-912_P100-1995", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[IBM_913] = ucnv_لدو_Shared_انکڑے("ibm-913_P100-2000", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[ISO_8859_14] = ucnv_لدو_Shared_انکڑے("iso-8859_14-1998", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);
        my_Converter_انکڑے->myConverterArray[IBM_923] = ucnv_لدو_Shared_انکڑے("ibm-923_P100-1998", &ڈھیر_Pieces, &ڈھیر_دلیلاں, error_کوڈ);

        إذا (U_FAILURE(*error_کوڈ) || p_دلیلاں->onlyTestIsLoadable) {
            Compound_متن_Close(cnv);
            أرجع;
        }

        my_Converter_انکڑے->state = (COMPOUND_متن_CONVERTERS)0;
    } وإلا {
        *error_کوڈ = U_حافظہ_ونڈ_ERROR;
    }
}


ساكن فراغ U_CALLCONV
Compound_متن_Close(UConverter *converter) {
    UConverter_انکڑے_Compound_متن* my_Converter_انکڑے = (UConverter_انکڑے_Compound_متن*)(converter->extraInfo);
    int32_t i;

    إذا (converter->extraInfo != خالی_سوچک) {
        /*close the array of converter pointers and free the memory*/
        لكل (i = 0; i < عدد_دا_CONVERTERS; i++) {
            إذا (my_Converter_انکڑے->myConverterArray[i] != خالی_سوچک) {
                ucnv_unload_Shared_انکڑے_If_Ready(my_Converter_انکڑے->myConverterArray[i]);
            }
        }

        uprv_free(converter->extraInfo);
        converter->extraInfo = خالی_سوچک;
    }
}

ساكن فراغ U_CALLCONV
Compound_متن_Reset(UConverter *converter, UConverterResetChoice choice) {
    (فراغ)converter;
    (فراغ)choice;
}

ساكن ثابت حرف* U_CALLCONV
_CompoundTextgetName(ثابت UConverter* cnv){
    (فراغ)cnv;
    أرجع "x11-compound-text";
}

ساكن فراغ U_CALLCONV
UConverter_from_Unicode_Compound_متن_OFFSETS(UConverter_From_Unicode_دلیلاں* دلیلاں_2, UError_کوڈ* err){
    UConverter *cnv = دلیلاں_2->converter;
    uint8_t *نشانہ = (uint8_t *) دلیلاں_2->نشانہ;
    ثابت uint8_t *نشانہ_حد_بندی = (ثابت uint8_t *) دلیلاں_2->نشانہ_حد_بندی;
    ثابت char16_t* سروت = دلیلاں_2->سروت;
    ثابت char16_t* سروت_حد_بندی = دلیلاں_2->سروت_حد_بندی;
    /* int32_t* offsets = args->offsets; */
    UChar32 سروت_Char;
    UBool useFallback = cnv->useFallback;
    uint8_t tmp_نشانہ_بفر[7];
    int32_t tmp_نشانہ_بفر_لمائی = 0;
    COMPOUND_متن_CONVERTERS currentState, tmpState;
    uint32_t p_قدر;
    int32_t p_قدر_لمائی = 0;
    int32_t i, n, j;

    UConverter_انکڑے_Compound_متن *my_Converter_انکڑے = (UConverter_انکڑے_Compound_متن *) cnv->extraInfo;

    currentState = my_Converter_انکڑے->state;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    إذا((سروت_Char = cnv->fromUChar32)!=0 && نشانہ< نشانہ_حد_بندی) {
        اذهب getTrail;
    }

    طالما( سروت < سروت_حد_بندی){
        إذا(نشانہ < نشانہ_حد_بندی){

            سروت_Char  = *(سروت++);
            /*check if the char is a First surrogate*/
             إذا(U16_IS_SURROGATE(سروت_Char)) {
                إذا(U16_IS_SURROGATE_LEAD(سروت_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    إذا(سروت < سروت_حد_بندی) {
                        /* test the following code unit */
                        char16_t trail = *سروت;
                        إذا(U16_IS_TRAIL(trail)) {
                            سروت++;
                            سروت_Char=U16_GET_SUPPLEMENTARY(سروت_Char, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_لبھیا;
                            cnv->fromUChar32=سروت_Char;
                            اكسر;
                        }
                    } وإلا {
                        /* no more input */
                        cnv->fromUChar32=سروت_Char;
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_لبھیا;
                    cnv->fromUChar32=سروت_Char;
                    اكسر;
                }
            }

             tmp_نشانہ_بفر_لمائی = 0;
             tmpState = getState(سروت_Char);

             إذا (tmpState != DO_تلاش && currentState != tmpState) {
                 /* Get escape sequence if necessary */
                 currentState = tmpState;
                 لكل (i = 0; esc_Seq_Compound_متن[currentState][i] != 0; i++) {
                     tmp_نشانہ_بفر[tmp_نشانہ_بفر_لمائی++] = esc_Seq_Compound_متن[currentState][i];
                 }
             }

             إذا (tmpState == DO_تلاش) {
                 /* Test all available converters */
                 لكل (i = 1; i < تلاش_لمائی; i++) {
                     p_قدر_لمائی = ucnv_MBCSFromUChar32(my_Converter_انکڑے->myConverterArray[i], سروت_Char, &p_قدر, useFallback);
                     إذا (p_قدر_لمائی > 0) {
                         tmpState = (COMPOUND_متن_CONVERTERS)i;
                         إذا (currentState != tmpState) {
                             currentState = tmpState;
                             لكل (j = 0; esc_Seq_Compound_متن[currentState][j] != 0; j++) {
                                 tmp_نشانہ_بفر[tmp_نشانہ_بفر_لمائی++] = esc_Seq_Compound_متن[currentState][j];
                             }
                         }
                         لكل (n = (p_قدر_لمائی - 1); n >= 0; n--) {
                             tmp_نشانہ_بفر[tmp_نشانہ_بفر_لمائی++] = (uint8_t)(p_قدر >> (n * 8));
                         }
                         اكسر;
                     }
                 }
             } وإلا إذا (tmpState == COMPOUND_متن_SINGLE_0) {
                 tmp_نشانہ_بفر[tmp_نشانہ_بفر_لمائی++] = (uint8_t)سروت_Char;
             } وإلا {
                 p_قدر_لمائی = ucnv_MBCSFromUChar32(my_Converter_انکڑے->myConverterArray[currentState], سروت_Char, &p_قدر, useFallback);
                 إذا (p_قدر_لمائی > 0) {
                     لكل (n = (p_قدر_لمائی - 1); n >= 0; n--) {
                         tmp_نشانہ_بفر[tmp_نشانہ_بفر_لمائی++] = (uint8_t)(p_قدر >> (n * 8));
                     }
                 }
             }

             لكل (i = 0; i < tmp_نشانہ_بفر_لمائی; i++) {
                 إذا (نشانہ < نشانہ_حد_بندی) {
                     *نشانہ++ = tmp_نشانہ_بفر[i];
                 } وإلا {
                     *err = U_بفر_حد_توں_ودھ_ERROR;
                     اكسر;
                 }
             }

             إذا (*err == U_بفر_حد_توں_ودھ_ERROR) {
                 لكل (; i < tmp_نشانہ_بفر_لمائی; i++) {
                     دلیلاں_2->converter->char_Error_بفر[دلیلاں_2->converter->char_Error_بفر_لمائی++] = tmp_نشانہ_بفر[i];
                 }
             }
        } وإلا {
            *err = U_بفر_حد_توں_ودھ_ERROR;
            اكسر;
        }
    }

    /*save the state and return */
    my_Converter_انکڑے->state = currentState;
    دلیلاں_2->سروت = سروت;
    دلیلاں_2->نشانہ = (حرف*)نشانہ;
}


ساكن فراغ U_CALLCONV
UConverter_منزل_Unicode_Compound_متن_OFFSETS(UConverter_منزل_Unicode_دلیلاں *دلیلاں_2,
                                               UError_کوڈ* err){
    ثابت حرف *my_سروت = (حرف *) دلیلاں_2->سروت;
    char16_t *my_نشانہ = دلیلاں_2->نشانہ;
    ثابت حرف *my_سروت_حد_بندی = دلیلاں_2->سروت_حد_بندی;
    ثابت حرف *tmp_سروت_حد_بندی = my_سروت_حد_بندی;
    uint32_t my_سروت_Char = 0x0000;
    COMPOUND_متن_CONVERTERS currentState, tmpState;
    int32_t سروت_وتھ = 0;
    UConverter_انکڑے_Compound_متن *my_Converter_انکڑے = (UConverter_انکڑے_Compound_متن *) دلیلاں_2->converter->extraInfo;
    UConverter_Shared_انکڑے* saved_Shared_انکڑے = خالی_سوچک;

    UConverter_منزل_Unicode_دلیلاں sub_دلیلاں;
    int32_t min_دلیلاں_ناپ;

    /* set up the subconverter arguments */
    إذا(دلیلاں_2->size<حجم(UConverter_منزل_Unicode_دلیلاں)) {
        min_دلیلاں_ناپ = دلیلاں_2->size;
    } وإلا {
        min_دلیلاں_ناپ = (int32_t)حجم(UConverter_منزل_Unicode_دلیلاں);
    }

    uprv_memcpy(&sub_دلیلاں, دلیلاں_2, min_دلیلاں_ناپ);
    sub_دلیلاں.size = (uint16_t)min_دلیلاں_ناپ;

    currentState = tmpState =  my_Converter_انکڑے->state;

    طالما(my_سروت < my_سروت_حد_بندی){
        إذا(my_نشانہ < دلیلاں_2->نشانہ_حد_بندی){
            إذا (دلیلاں_2->converter->منزل_ULength > 0) {
                my_سروت_Char = دلیلاں_2->converter->منزل_UBytes[0];
            } وإلا {
                my_سروت_Char = (uint8_t)*my_سروت;
            }

            إذا (my_سروت_Char == ESC_شروع) {
                tmpState = لبھو_State_From_Esc_Seq(my_سروت, my_سروت_حد_بندی, دلیلاں_2->converter->منزل_UBytes, دلیلاں_2->converter->منزل_ULength, err);

                إذا (*err == U_TRUNCATED_CHAR_لبھیا) {
                    لكل (; my_سروت < my_سروت_حد_بندی;) {
                        دلیلاں_2->converter->منزل_UBytes[دلیلاں_2->converter->منزل_ULength++] = *my_سروت++;
                    }
                    *err = U_ZERO_ERROR;
                    اكسر;
                } وإلا إذا (tmpState == INVALID) {
                    إذا (دلیلاں_2->converter->منزل_ULength == 0) {
                        my_سروت++; /* skip over the 0x1b byte */
                    }
                    *err = U_ILLEGAL_CHAR_لبھیا;
                    اكسر;
                }

                إذا (tmpState != currentState) {
                    currentState = tmpState;
                }

                سروت_وتھ = static_cast<int32_t>(uprv_strlen((حرف*)esc_Seq_Compound_متن[currentState]) - دلیلاں_2->converter->منزل_ULength);

                my_سروت += سروت_وتھ;

                دلیلاں_2->converter->منزل_ULength = 0;
            }

            إذا (currentState == COMPOUND_متن_SINGLE_0) {
                طالما (my_سروت < my_سروت_حد_بندی) {
                    إذا (*my_سروت == ESC_شروع) {
                        اكسر;
                    }
                    إذا (my_نشانہ < دلیلاں_2->نشانہ_حد_بندی) {
                        *my_نشانہ++ = 0x00ff&(*my_سروت++);
                    } وإلا {
                        *err = U_بفر_حد_توں_ودھ_ERROR;
                        اكسر;
                    }
                }
            } وإلا إذا (my_سروت < my_سروت_حد_بندی){
                سروت_وتھ = لبھو_اگلا_Esc(my_سروت, my_سروت_حد_بندی);

                tmp_سروت_حد_بندی = my_سروت + سروت_وتھ;

                sub_دلیلاں.سروت = my_سروت;
                sub_دلیلاں.سروت_حد_بندی = tmp_سروت_حد_بندی;
                sub_دلیلاں.نشانہ = my_نشانہ;
                saved_Shared_انکڑے = sub_دلیلاں.converter->shared_انکڑے;
                sub_دلیلاں.converter->shared_انکڑے = my_Converter_انکڑے->myConverterArray[currentState];

                ucnv_MBCSToUnicodeWithOffsets(&sub_دلیلاں, err);

                sub_دلیلاں.converter->shared_انکڑے = saved_Shared_انکڑے;

                my_سروت = sub_دلیلاں.سروت;
                my_نشانہ = sub_دلیلاں.نشانہ;

                إذا (U_FAILURE(*err)) {
                    إذا(*err == U_بفر_حد_توں_ودھ_ERROR) {
                        إذا(sub_دلیلاں.converter->UChar_Error_بفر_لمائی > 0) {
                            uprv_memcpy(دلیلاں_2->converter->UChar_Error_بفر, sub_دلیلاں.converter->UChar_Error_بفر,
                                        sub_دلیلاں.converter->UChar_Error_بفر_لمائی);
                        }
                        دلیلاں_2->converter->UChar_Error_بفر_لمائی=sub_دلیلاں.converter->UChar_Error_بفر_لمائی;
                        sub_دلیلاں.converter->UChar_Error_بفر_لمائی = 0;
                    }
                    اكسر;
                }
            }
        } وإلا {
            *err = U_بفر_حد_توں_ودھ_ERROR;
            اكسر;
        }
    }
    my_Converter_انکڑے->state = currentState;
    دلیلاں_2->نشانہ = my_نشانہ;
    دلیلاں_2->سروت = my_سروت;
}

ساكن فراغ U_CALLCONV
Compound_متن_Get_Unicode_سیٹ(ثابت UConverter *cnv,
                    ثابت USetAdder *sa,
                    UConverter_Unicode_سیٹ which,
                    UError_کوڈ *p_Error_کوڈ) {
    UConverter_انکڑے_Compound_متن *my_Converter_انکڑے = (UConverter_انکڑے_Compound_متن *)cnv->extraInfo;
    int32_t i;

    لكل (i = 1; i < عدد_دا_CONVERTERS; i++) {
        ucnv_MBCSGet_Unicode_سیٹ_For_Unicode(my_Converter_انکڑے->myConverterArray[i], sa, which, p_Error_کوڈ);
    }
    sa->جوڑو(sa->سیٹ, 0x0000);
    sa->جوڑو(sa->سیٹ, 0x0009);
    sa->جوڑو(sa->سیٹ, 0x000A);
    sa->جوڑو_Range(sa->سیٹ, 0x0020, 0x007F);
    sa->جوڑو_Range(sa->سیٹ, 0x00A0, 0x00FF);
}
U_CDECL_سرا

ساكن ثابت UConverterImpl Compound_متن_Impl = {

    UCNV_COMPOUND_متن,

    خالی_سوچک,
    خالی_سوچک,

    Compound_متن_Open,
    Compound_متن_Close,
    Compound_متن_Reset,

    UConverter_منزل_Unicode_Compound_متن_OFFSETS,
    UConverter_منزل_Unicode_Compound_متن_OFFSETS,
    UConverter_from_Unicode_Compound_متن_OFFSETS,
    UConverter_from_Unicode_Compound_متن_OFFSETS,
    خالی_سوچک,

    خالی_سوچک,
    _CompoundTextgetName,
    خالی_سوچک,
    خالی_سوچک,
    Compound_متن_Get_Unicode_سیٹ,
    خالی_سوچک,
    خالی_سوچک
};

ساكن ثابت UConverter_Static_انکڑے Compound_متن_Static_انکڑے = {
    حجم(UConverter_Static_انکڑے),
    "COMPOUND_TEXT",
    0,
    UCNV_IBM,
    UCNV_COMPOUND_متن,
    1,
    6,
    { 0xef, 0, 0, 0 },
    1,
    خطأ,
    خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};
ثابت UConverter_Shared_انکڑے Compound_متن_انکڑے =
        UCNV_IMMUTABLE_SHARED_انکڑے_INITIALIZER(&Compound_متن_Static_انکڑے, &Compound_متن_Impl);

#جے_انت /* #if !UCONFIG_NO_LEGACY_CONVERSION */
