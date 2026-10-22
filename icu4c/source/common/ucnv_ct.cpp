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
    DO_ڳولا = -1,

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
    انگ_جو_CONVERTERS = 20
} COMPOUND_متن_CONVERTERS;

#وصف_ڪريو ڳولا_ڊيگهائي 12

ساكن ثابت uint8_t esc_Seq_Compound_متن[انگ_جو_CONVERTERS][5] = {
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

#وصف_ڪريو ESC_شروعات 0x1B

#وصف_ڪريو isASCIIRange(codepoint) \
        ((codepoint == 0x0000) || (codepoint == 0x0009) || (codepoint == 0x000A) || \
         (codepoint >= 0x0020 && codepoint <= 0x007f) || (codepoint >= 0x00A0 && codepoint <= 0x00FF))

#وصف_ڪريو isIBM915(codepoint) \
        ((codepoint >= 0x0401 && codepoint <= 0x045F) || (codepoint == 0x2116))

#وصف_ڪريو isIBM916(codepoint) \
        ((codepoint >= 0x05D0 && codepoint <= 0x05EA) || (codepoint == 0x2017) || (codepoint == 0x203E))

#وصف_ڪريو isCompoundS3(codepoint) \
        ((codepoint == 0x060C) || (codepoint == 0x061B) || (codepoint == 0x061F) || (codepoint >= 0x0621 && codepoint <= 0x063A) || \
         (codepoint >= 0x0640 && codepoint <= 0x0652) || (codepoint >= 0x0660 && codepoint <= 0x066D) || (codepoint == 0x200B) || \
         (codepoint >= 0x0FE70 && codepoint <= 0x0FE72) || (codepoint == 0x0FE74) || (codepoint >= 0x0FE76 && codepoint <= 0x0FEBE))

#وصف_ڪريو isCompoundS2(codepoint) \
        ((codepoint == 0x02BC) || (codepoint == 0x02BD) || (codepoint >= 0x0384 && codepoint <= 0x03CE) || (codepoint == 0x2015))

#وصف_ڪريو isIBM914(codepoint) \
        ((codepoint == 0x0100) || (codepoint == 0x0101) || (codepoint == 0x0112) || (codepoint == 0x0113) || (codepoint == 0x0116) || (codepoint == 0x0117) || \
         (codepoint == 0x0122) || (codepoint == 0x0123) || (codepoint >= 0x0128 && codepoint <= 0x012B) || (codepoint == 0x012E) || (codepoint == 0x012F) || \
         (codepoint >= 0x0136 && codepoint <= 0x0138) || (codepoint == 0x013B) || (codepoint == 0x013C) || (codepoint == 0x0145) || (codepoint ==  0x0146) || \
         (codepoint >= 0x014A && codepoint <= 0x014D) || (codepoint == 0x0156) || (codepoint == 0x0157) || (codepoint >= 0x0166 && codepoint <= 0x016B) || \
         (codepoint == 0x0172) || (codepoint == 0x0173))

#وصف_ڪريو isIBM874(codepoint) \
        ((codepoint >= 0x0E01 && codepoint <= 0x0E3A) || (codepoint >= 0x0E3F && codepoint <= 0x0E5B))

#وصف_ڪريو isIBM912(codepoint) \
        ((codepoint >= 0x0102 && codepoint <= 0x0107) || (codepoint >= 0x010C && codepoint <= 0x0111) || (codepoint >= 0x0118 && codepoint <= 0x011B) || \
         (codepoint == 0x0139) || (codepoint == 0x013A) || (codepoint == 0x013D) || (codepoint == 0x013E) || (codepoint >= 0x0141 && codepoint <= 0x0144) || \
         (codepoint == 0x0147) || (codepoint == 0x0147) || (codepoint == 0x0150) || (codepoint == 0x0151) || (codepoint == 0x0154) || (codepoint == 0x0155) || \
         (codepoint >= 0x0158 && codepoint <= 0x015B) || (codepoint == 0x015E) || (codepoint == 0x015F) || (codepoint >= 0x0160 && codepoint <= 0x0165) || \
         (codepoint == 0x016E) || (codepoint == 0x016F) || (codepoint == 0x0170) || (codepoint ==  0x0171) || (codepoint >= 0x0179 && codepoint <= 0x017E) || \
         (codepoint == 0x02C7) || (codepoint == 0x02D8) || (codepoint == 0x02D9) || (codepoint == 0x02DB) || (codepoint == 0x02DD))

#وصف_ڪريو isIBM913(codepoint) \
        ((codepoint >= 0x0108 && codepoint <= 0x010B) || (codepoint == 0x011C) || \
         (codepoint == 0x011D) || (codepoint == 0x0120) || (codepoint == 0x0121) || \
         (codepoint >= 0x0124 && codepoint <= 0x0127) || (codepoint == 0x0134) || (codepoint == 0x0135) || \
         (codepoint == 0x015C) || (codepoint == 0x015D) || (codepoint == 0x016C) || (codepoint ==  0x016D))

#وصف_ڪريو isCompoundS1(codepoint) \
        ((codepoint == 0x011E) || (codepoint == 0x011F) || (codepoint == 0x0130) || \
         (codepoint == 0x0131) || (codepoint >= 0x0218 && codepoint <= 0x021B))

#وصف_ڪريو isISO8859_14(codepoint) \
        ((codepoint >= 0x0174 && codepoint <= 0x0177) || (codepoint == 0x1E0A) || \
         (codepoint == 0x1E0B) || (codepoint == 0x1E1E) || (codepoint == 0x1E1F) || \
         (codepoint == 0x1E40) || (codepoint == 0x1E41) || (codepoint == 0x1E56) || \
         (codepoint == 0x1E57) || (codepoint == 0x1E60) || (codepoint == 0x1E61) || \
         (codepoint == 0x1E6A) || (codepoint == 0x1E6B) || (codepoint == 0x1EF2) || \
         (codepoint == 0x1EF3) || (codepoint >= 0x1E80 && codepoint <= 0x1E85))

#وصف_ڪريو isIBM923(codepoint) \
        ((codepoint == 0x0152) || (codepoint == 0x0153) || (codepoint == 0x0178) || (codepoint == 0x20AC))


تعريفنوع بنية{
    UConverter_Shared_معلومات *myConverterArray[انگ_جو_CONVERTERS];
    COMPOUND_متن_CONVERTERS state;
} UConverter_معلومات_Compound_متن;

/*********** Compound Text Converter Protos ***********/
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
Compound_متن_Open(UConverter *cnv, UConverter_ڀريو_دليلون *p_دليلون, UError_ڪوڊ *error_ڪوڊ);

ساكن فراغ U_CALLCONV
 Compound_متن_Close(UConverter *converter);

ساكن فراغ U_CALLCONV
Compound_متن_Reset(UConverter *converter, UConverterResetChoice choice);

ساكن ثابت حرف* U_CALLCONV
_CompoundTextgetName(ثابت UConverter* cnv);


ساكن int32_t ڳوليو_ايندڙ_Esc(ثابت حرف *ذريعو, ثابت حرف *ذريعو_حد_بندي) {
    int32_t length = static_cast<int32_t>(ذريعو_حد_بندي - ذريعو);
    int32_t i;
    لكل (i = 1; i < length; i++) {
        إذا (*(ذريعو + i) == 0x1B) {
            أرجع i;
        }
    }

    أرجع length;
}

ساكن COMPOUND_متن_CONVERTERS getState(صحيح codepoint) {
    COMPOUND_متن_CONVERTERS state = DO_ڳولا;

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

ساكن COMPOUND_متن_CONVERTERS ڳوليو_State_From_Esc_Seq(ثابت حرف* ذريعو, ثابت حرف* ذريعو_حد_بندي, ثابت uint8_t* منزل_UBytes_بفر, int32_t منزل_UBytes_بفر_ڊيگهائي, UError_ڪوڊ *err) {
    COMPOUND_متن_CONVERTERS state = INVALID;
    UBool ميلاپ_مليو = خطأ;
    int32_t i, n, وٿي = منزل_UBytes_بفر_ڊيگهائي;

    لكل (i = 0; i < انگ_جو_CONVERTERS; i++) {
        ميلاپ_مليو = سچ;
        لكل (n = 0; esc_Seq_Compound_متن[i][n] != 0; n++) {
            إذا (n < منزل_UBytes_بفر_ڊيگهائي) {
                إذا (منزل_UBytes_بفر[n] != esc_Seq_Compound_متن[i][n]) {
                    ميلاپ_مليو = خطأ;
                    اكسر;
                }
            } وإلا إذا ((ذريعو + (n - وٿي)) >= ذريعو_حد_بندي) {
                *err = U_TRUNCATED_CHAR_مليو;
                ميلاپ_مليو = خطأ;
                اكسر;
            } وإلا إذا (*(ذريعو + (n - وٿي)) != esc_Seq_Compound_متن[i][n]) {
                ميلاپ_مليو = خطأ;
                اكسر;
            }
        }

        إذا (ميلاپ_مليو) {
            اكسر;
        }
    }

    إذا (ميلاپ_مليو) {
        state = (COMPOUND_متن_CONVERTERS)i;
    }

    أرجع state;
}

ساكن فراغ U_CALLCONV
Compound_متن_Open(UConverter *cnv, UConverter_ڀريو_دليلون *p_دليلون, UError_ڪوڊ *error_ڪوڊ){
    cnv->extraInfo = uprv_malloc (حجم (UConverter_معلومات_Compound_متن));
    إذا (cnv->extraInfo != خالي_اشارو) {
        UConverter_معلومات_Compound_متن *my_Converter_معلومات = (UConverter_معلومات_Compound_متن *) cnv->extraInfo;

        UConverterNamePieces ڍير_Pieces;
        UConverter_ڀريو_دليلون ڍير_دليلون=UCNV_ڀريو_دليلون_INITIALIZER;

        my_Converter_معلومات->myConverterArray[COMPOUND_متن_SINGLE_0] = خالي_اشارو;
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_SINGLE_1] = ucnv_ڀريو_Shared_معلومات("icu-internal-compound-s1", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_SINGLE_2] = ucnv_ڀريو_Shared_معلومات("icu-internal-compound-s2", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_SINGLE_3] = ucnv_ڀريو_Shared_معلومات("icu-internal-compound-s3", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_DOUBLE_1] = ucnv_ڀريو_Shared_معلومات("icu-internal-compound-d1", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_DOUBLE_2] = ucnv_ڀريو_Shared_معلومات("icu-internal-compound-d2", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_DOUBLE_3] = ucnv_ڀريو_Shared_معلومات("icu-internal-compound-d3", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_DOUBLE_4] = ucnv_ڀريو_Shared_معلومات("icu-internal-compound-d4", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_DOUBLE_5] = ucnv_ڀريو_Shared_معلومات("icu-internal-compound-d5", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_DOUBLE_6] = ucnv_ڀريو_Shared_معلومات("icu-internal-compound-d6", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_DOUBLE_7] = ucnv_ڀريو_Shared_معلومات("icu-internal-compound-d7", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_TRIPLE_DOUBLE] = ucnv_ڀريو_Shared_معلومات("icu-internal-compound-t", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);

        my_Converter_معلومات->myConverterArray[IBM_915] = ucnv_ڀريو_Shared_معلومات("ibm-915_P100-1995", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[IBM_916] = ucnv_ڀريو_Shared_معلومات("ibm-916_P100-1995", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[IBM_914] = ucnv_ڀريو_Shared_معلومات("ibm-914_P100-1995", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[IBM_874] = ucnv_ڀريو_Shared_معلومات("ibm-874_P100-1995", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[IBM_912] = ucnv_ڀريو_Shared_معلومات("ibm-912_P100-1995", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[IBM_913] = ucnv_ڀريو_Shared_معلومات("ibm-913_P100-2000", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[ISO_8859_14] = ucnv_ڀريو_Shared_معلومات("iso-8859_14-1998", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);
        my_Converter_معلومات->myConverterArray[IBM_923] = ucnv_ڀريو_Shared_معلومات("ibm-923_P100-1998", &ڍير_Pieces, &ڍير_دليلون, error_ڪوڊ);

        إذا (U_FAILURE(*error_ڪوڊ) || p_دليلون->onlyTestIsLoadable) {
            Compound_متن_Close(cnv);
            أرجع;
        }

        my_Converter_معلومات->state = (COMPOUND_متن_CONVERTERS)0;
    } وإلا {
        *error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
    }
}


ساكن فراغ U_CALLCONV
Compound_متن_Close(UConverter *converter) {
    UConverter_معلومات_Compound_متن* my_Converter_معلومات = (UConverter_معلومات_Compound_متن*)(converter->extraInfo);
    int32_t i;

    إذا (converter->extraInfo != خالي_اشارو) {
        /*close the array of converter pointers and free the memory*/
        لكل (i = 0; i < انگ_جو_CONVERTERS; i++) {
            إذا (my_Converter_معلومات->myConverterArray[i] != خالي_اشارو) {
                ucnv_unload_Shared_معلومات_If_Ready(my_Converter_معلومات->myConverterArray[i]);
            }
        }

        uprv_free(converter->extraInfo);
        converter->extraInfo = خالي_اشارو;
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
UConverter_from_Unicode_Compound_متن_OFFSETS(UConverter_From_Unicode_دليلون* دليلون_2, UError_ڪوڊ* err){
    UConverter *cnv = دليلون_2->converter;
    uint8_t *هدف = (uint8_t *) دليلون_2->هدف;
    ثابت uint8_t *هدف_حد_بندي = (ثابت uint8_t *) دليلون_2->هدف_حد_بندي;
    ثابت char16_t* ذريعو = دليلون_2->ذريعو;
    ثابت char16_t* ذريعو_حد_بندي = دليلون_2->ذريعو_حد_بندي;
    /* int32_t* offsets = args->offsets; */
    UChar32 ذريعو_Char;
    UBool useFallback = cnv->useFallback;
    uint8_t tmp_هدف_بفر[7];
    int32_t tmp_هدف_بفر_ڊيگهائي = 0;
    COMPOUND_متن_CONVERTERS currentState, tmpState;
    uint32_t p_قدر;
    int32_t p_قدر_ڊيگهائي = 0;
    int32_t i, n, j;

    UConverter_معلومات_Compound_متن *my_Converter_معلومات = (UConverter_معلومات_Compound_متن *) cnv->extraInfo;

    currentState = my_Converter_معلومات->state;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    إذا((ذريعو_Char = cnv->fromUChar32)!=0 && هدف< هدف_حد_بندي) {
        اذهب getTrail;
    }

    طالما( ذريعو < ذريعو_حد_بندي){
        إذا(هدف < هدف_حد_بندي){

            ذريعو_Char  = *(ذريعو++);
            /*check if the char is a First surrogate*/
             إذا(U16_IS_SURROGATE(ذريعو_Char)) {
                إذا(U16_IS_SURROGATE_LEAD(ذريعو_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    إذا(ذريعو < ذريعو_حد_بندي) {
                        /* test the following code unit */
                        char16_t trail = *ذريعو;
                        إذا(U16_IS_TRAIL(trail)) {
                            ذريعو++;
                            ذريعو_Char=U16_GET_SUPPLEMENTARY(ذريعو_Char, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_مليو;
                            cnv->fromUChar32=ذريعو_Char;
                            اكسر;
                        }
                    } وإلا {
                        /* no more input */
                        cnv->fromUChar32=ذريعو_Char;
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_مليو;
                    cnv->fromUChar32=ذريعو_Char;
                    اكسر;
                }
            }

             tmp_هدف_بفر_ڊيگهائي = 0;
             tmpState = getState(ذريعو_Char);

             إذا (tmpState != DO_ڳولا && currentState != tmpState) {
                 /* Get escape sequence if necessary */
                 currentState = tmpState;
                 لكل (i = 0; esc_Seq_Compound_متن[currentState][i] != 0; i++) {
                     tmp_هدف_بفر[tmp_هدف_بفر_ڊيگهائي++] = esc_Seq_Compound_متن[currentState][i];
                 }
             }

             إذا (tmpState == DO_ڳولا) {
                 /* Test all available converters */
                 لكل (i = 1; i < ڳولا_ڊيگهائي; i++) {
                     p_قدر_ڊيگهائي = ucnv_MBCSFromUChar32(my_Converter_معلومات->myConverterArray[i], ذريعو_Char, &p_قدر, useFallback);
                     إذا (p_قدر_ڊيگهائي > 0) {
                         tmpState = (COMPOUND_متن_CONVERTERS)i;
                         إذا (currentState != tmpState) {
                             currentState = tmpState;
                             لكل (j = 0; esc_Seq_Compound_متن[currentState][j] != 0; j++) {
                                 tmp_هدف_بفر[tmp_هدف_بفر_ڊيگهائي++] = esc_Seq_Compound_متن[currentState][j];
                             }
                         }
                         لكل (n = (p_قدر_ڊيگهائي - 1); n >= 0; n--) {
                             tmp_هدف_بفر[tmp_هدف_بفر_ڊيگهائي++] = (uint8_t)(p_قدر >> (n * 8));
                         }
                         اكسر;
                     }
                 }
             } وإلا إذا (tmpState == COMPOUND_متن_SINGLE_0) {
                 tmp_هدف_بفر[tmp_هدف_بفر_ڊيگهائي++] = (uint8_t)ذريعو_Char;
             } وإلا {
                 p_قدر_ڊيگهائي = ucnv_MBCSFromUChar32(my_Converter_معلومات->myConverterArray[currentState], ذريعو_Char, &p_قدر, useFallback);
                 إذا (p_قدر_ڊيگهائي > 0) {
                     لكل (n = (p_قدر_ڊيگهائي - 1); n >= 0; n--) {
                         tmp_هدف_بفر[tmp_هدف_بفر_ڊيگهائي++] = (uint8_t)(p_قدر >> (n * 8));
                     }
                 }
             }

             لكل (i = 0; i < tmp_هدف_بفر_ڊيگهائي; i++) {
                 إذا (هدف < هدف_حد_بندي) {
                     *هدف++ = tmp_هدف_بفر[i];
                 } وإلا {
                     *err = U_بفر_حد_کان_مٿي_ERROR;
                     اكسر;
                 }
             }

             إذا (*err == U_بفر_حد_کان_مٿي_ERROR) {
                 لكل (; i < tmp_هدف_بفر_ڊيگهائي; i++) {
                     دليلون_2->converter->char_Error_بفر[دليلون_2->converter->char_Error_بفر_ڊيگهائي++] = tmp_هدف_بفر[i];
                 }
             }
        } وإلا {
            *err = U_بفر_حد_کان_مٿي_ERROR;
            اكسر;
        }
    }

    /*save the state and return */
    my_Converter_معلومات->state = currentState;
    دليلون_2->ذريعو = ذريعو;
    دليلون_2->هدف = (حرف*)هدف;
}


ساكن فراغ U_CALLCONV
UConverter_منزل_Unicode_Compound_متن_OFFSETS(UConverter_منزل_Unicode_دليلون *دليلون_2,
                                               UError_ڪوڊ* err){
    ثابت حرف *my_ذريعو = (حرف *) دليلون_2->ذريعو;
    char16_t *my_هدف = دليلون_2->هدف;
    ثابت حرف *my_ذريعو_حد_بندي = دليلون_2->ذريعو_حد_بندي;
    ثابت حرف *tmp_ذريعو_حد_بندي = my_ذريعو_حد_بندي;
    uint32_t my_ذريعو_Char = 0x0000;
    COMPOUND_متن_CONVERTERS currentState, tmpState;
    int32_t ذريعو_وٿي = 0;
    UConverter_معلومات_Compound_متن *my_Converter_معلومات = (UConverter_معلومات_Compound_متن *) دليلون_2->converter->extraInfo;
    UConverter_Shared_معلومات* saved_Shared_معلومات = خالي_اشارو;

    UConverter_منزل_Unicode_دليلون sub_دليلون;
    int32_t min_دليلون_سائيز;

    /* set up the subconverter arguments */
    إذا(دليلون_2->size<حجم(UConverter_منزل_Unicode_دليلون)) {
        min_دليلون_سائيز = دليلون_2->size;
    } وإلا {
        min_دليلون_سائيز = (int32_t)حجم(UConverter_منزل_Unicode_دليلون);
    }

    uprv_memcpy(&sub_دليلون, دليلون_2, min_دليلون_سائيز);
    sub_دليلون.size = (uint16_t)min_دليلون_سائيز;

    currentState = tmpState =  my_Converter_معلومات->state;

    طالما(my_ذريعو < my_ذريعو_حد_بندي){
        إذا(my_هدف < دليلون_2->هدف_حد_بندي){
            إذا (دليلون_2->converter->منزل_ULength > 0) {
                my_ذريعو_Char = دليلون_2->converter->منزل_UBytes[0];
            } وإلا {
                my_ذريعو_Char = (uint8_t)*my_ذريعو;
            }

            إذا (my_ذريعو_Char == ESC_شروعات) {
                tmpState = ڳوليو_State_From_Esc_Seq(my_ذريعو, my_ذريعو_حد_بندي, دليلون_2->converter->منزل_UBytes, دليلون_2->converter->منزل_ULength, err);

                إذا (*err == U_TRUNCATED_CHAR_مليو) {
                    لكل (; my_ذريعو < my_ذريعو_حد_بندي;) {
                        دليلون_2->converter->منزل_UBytes[دليلون_2->converter->منزل_ULength++] = *my_ذريعو++;
                    }
                    *err = U_ZERO_ERROR;
                    اكسر;
                } وإلا إذا (tmpState == INVALID) {
                    إذا (دليلون_2->converter->منزل_ULength == 0) {
                        my_ذريعو++; /* skip over the 0x1b byte */
                    }
                    *err = U_ILLEGAL_CHAR_مليو;
                    اكسر;
                }

                إذا (tmpState != currentState) {
                    currentState = tmpState;
                }

                ذريعو_وٿي = static_cast<int32_t>(uprv_strlen((حرف*)esc_Seq_Compound_متن[currentState]) - دليلون_2->converter->منزل_ULength);

                my_ذريعو += ذريعو_وٿي;

                دليلون_2->converter->منزل_ULength = 0;
            }

            إذا (currentState == COMPOUND_متن_SINGLE_0) {
                طالما (my_ذريعو < my_ذريعو_حد_بندي) {
                    إذا (*my_ذريعو == ESC_شروعات) {
                        اكسر;
                    }
                    إذا (my_هدف < دليلون_2->هدف_حد_بندي) {
                        *my_هدف++ = 0x00ff&(*my_ذريعو++);
                    } وإلا {
                        *err = U_بفر_حد_کان_مٿي_ERROR;
                        اكسر;
                    }
                }
            } وإلا إذا (my_ذريعو < my_ذريعو_حد_بندي){
                ذريعو_وٿي = ڳوليو_ايندڙ_Esc(my_ذريعو, my_ذريعو_حد_بندي);

                tmp_ذريعو_حد_بندي = my_ذريعو + ذريعو_وٿي;

                sub_دليلون.ذريعو = my_ذريعو;
                sub_دليلون.ذريعو_حد_بندي = tmp_ذريعو_حد_بندي;
                sub_دليلون.هدف = my_هدف;
                saved_Shared_معلومات = sub_دليلون.converter->shared_معلومات;
                sub_دليلون.converter->shared_معلومات = my_Converter_معلومات->myConverterArray[currentState];

                ucnv_MBCSToUnicodeWithOffsets(&sub_دليلون, err);

                sub_دليلون.converter->shared_معلومات = saved_Shared_معلومات;

                my_ذريعو = sub_دليلون.ذريعو;
                my_هدف = sub_دليلون.هدف;

                إذا (U_FAILURE(*err)) {
                    إذا(*err == U_بفر_حد_کان_مٿي_ERROR) {
                        إذا(sub_دليلون.converter->UChar_Error_بفر_ڊيگهائي > 0) {
                            uprv_memcpy(دليلون_2->converter->UChar_Error_بفر, sub_دليلون.converter->UChar_Error_بفر,
                                        sub_دليلون.converter->UChar_Error_بفر_ڊيگهائي);
                        }
                        دليلون_2->converter->UChar_Error_بفر_ڊيگهائي=sub_دليلون.converter->UChar_Error_بفر_ڊيگهائي;
                        sub_دليلون.converter->UChar_Error_بفر_ڊيگهائي = 0;
                    }
                    اكسر;
                }
            }
        } وإلا {
            *err = U_بفر_حد_کان_مٿي_ERROR;
            اكسر;
        }
    }
    my_Converter_معلومات->state = currentState;
    دليلون_2->هدف = my_هدف;
    دليلون_2->ذريعو = my_ذريعو;
}

ساكن فراغ U_CALLCONV
Compound_متن_Get_Unicode_سيٽ(ثابت UConverter *cnv,
                    ثابت USetAdder *sa,
                    UConverter_Unicode_سيٽ which,
                    UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter_معلومات_Compound_متن *my_Converter_معلومات = (UConverter_معلومات_Compound_متن *)cnv->extraInfo;
    int32_t i;

    لكل (i = 1; i < انگ_جو_CONVERTERS; i++) {
        ucnv_MBCSGet_Unicode_سيٽ_For_Unicode(my_Converter_معلومات->myConverterArray[i], sa, which, p_Error_ڪوڊ);
    }
    sa->جوڙيو(sa->سيٽ, 0x0000);
    sa->جوڙيو(sa->سيٽ, 0x0009);
    sa->جوڙيو(sa->سيٽ, 0x000A);
    sa->جوڙيو_Range(sa->سيٽ, 0x0020, 0x007F);
    sa->جوڙيو_Range(sa->سيٽ, 0x00A0, 0x00FF);
}
U_CDECL_پڇاڙي

ساكن ثابت UConverterImpl Compound_متن_Impl = {

    UCNV_COMPOUND_متن,

    خالي_اشارو,
    خالي_اشارو,

    Compound_متن_Open,
    Compound_متن_Close,
    Compound_متن_Reset,

    UConverter_منزل_Unicode_Compound_متن_OFFSETS,
    UConverter_منزل_Unicode_Compound_متن_OFFSETS,
    UConverter_from_Unicode_Compound_متن_OFFSETS,
    UConverter_from_Unicode_Compound_متن_OFFSETS,
    خالي_اشارو,

    خالي_اشارو,
    _CompoundTextgetName,
    خالي_اشارو,
    خالي_اشارو,
    Compound_متن_Get_Unicode_سيٽ,
    خالي_اشارو,
    خالي_اشارو
};

ساكن ثابت UConverter_Static_معلومات Compound_متن_Static_معلومات = {
    حجم(UConverter_Static_معلومات),
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
ثابت UConverter_Shared_معلومات Compound_متن_معلومات =
        UCNV_IMMUTABLE_SHARED_معلومات_INITIALIZER(&Compound_متن_Static_معلومات, &Compound_متن_Impl);

#جيڪڏهن_پورو /* #if !UCONFIG_NO_LEGACY_CONVERSION */
