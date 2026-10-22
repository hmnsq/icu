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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#شامل "unicode/ucnv.h"
#شامل "unicode/uset.h"
#شامل "unicode/ucnv_err.h"
#شامل "unicode/ucnv_cb.h"
#شامل "unicode/utf16.h"
#شامل "ucnv_imp.h"
#شامل "ucnv_bld.h"
#شامل "ucnv_cnv.h"
#شامل "ucnvmbcs.h"
#شامل "cstring.h"
#شامل "cmemory.h"

قسمتعریف شمار {
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
    عدد_کا_CONVERTERS = 20
} COMPOUND_متن_CONVERTERS;

#تعریف_کرو تلاش_لمبائی 12

ساکن ثابت uint8_t esc_Seq_Compound_متن[عدد_کا_CONVERTERS][5] = {
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

#تعریف_کرو ESC_آغاز 0x1B

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


قسمتعریف ساخت{
    UConverter_Shared_ڈیٹا *myConverterArray[عدد_کا_CONVERTERS];
    COMPOUND_متن_CONVERTERS state;
} UConverter_ڈیٹا_Compound_متن;

/*********** Compound Text Converter Protos ***********/
U_CDECL_BEGIN
ساکن خالی U_CALLCONV
Compound_متن_Open(UConverter *cnv, UConverter_بھرو_دلائل *p_دلائل, UError_رمز *error_رمز);

ساکن خالی U_CALLCONV
 Compound_متن_Close(UConverter *converter);

ساکن خالی U_CALLCONV
Compound_متن_Reset(UConverter *converter, UConverterResetChoice choice);

ساکن ثابت حرف* U_CALLCONV
_CompoundTextgetName(ثابت UConverter* cnv);


ساکن int32_t ڈھونڈو_اگلا_Esc(ثابت حرف *ماخذ, ثابت حرف *ماخذ_حد_بندی) {
    int32_t length = static_cast<int32_t>(ماخذ_حد_بندی - ماخذ);
    int32_t i;
    برائے (i = 1; i < length; i++) {
        اگر (*(ماخذ + i) == 0x1B) {
            واپس i;
        }
    }

    واپس length;
}

ساکن COMPOUND_متن_CONVERTERS getState(صحیح codepoint) {
    COMPOUND_متن_CONVERTERS state = DO_تلاش;

    اگر (isASCIIRange(codepoint)) {
        state = COMPOUND_متن_SINGLE_0;
    } ورنہ اگر (isIBM912(codepoint)) {
        state = IBM_912;
    }ورنہ اگر (isIBM913(codepoint)) {
        state = IBM_913;
    } ورنہ اگر (isISO8859_14(codepoint)) {
        state = ISO_8859_14;
    } ورنہ اگر (isIBM923(codepoint)) {
        state = IBM_923;
    } ورنہ اگر (isIBM874(codepoint)) {
        state = IBM_874;
    } ورنہ اگر (isIBM914(codepoint)) {
        state = IBM_914;
    } ورنہ اگر (isCompoundS2(codepoint)) {
        state = COMPOUND_متن_SINGLE_2;
    } ورنہ اگر (isCompoundS3(codepoint)) {
        state = COMPOUND_متن_SINGLE_3;
    } ورنہ اگر (isIBM916(codepoint)) {
        state = IBM_916;
    } ورنہ اگر (isIBM915(codepoint)) {
        state = IBM_915;
    } ورنہ اگر (isCompoundS1(codepoint)) {
        state = COMPOUND_متن_SINGLE_1;
    }

    واپس state;
}

ساکن COMPOUND_متن_CONVERTERS ڈھونڈو_State_From_Esc_Seq(ثابت حرف* ماخذ, ثابت حرف* ماخذ_حد_بندی, ثابت uint8_t* مقصد_UBytes_بفر, int32_t مقصد_UBytes_بفر_لمبائی, UError_رمز *err) {
    COMPOUND_متن_CONVERTERS state = INVALID;
    UBool مطابقت_ملا = جھوٹ;
    int32_t i, n, انحراف = مقصد_UBytes_بفر_لمبائی;

    برائے (i = 0; i < عدد_کا_CONVERTERS; i++) {
        مطابقت_ملا = سچ;
        برائے (n = 0; esc_Seq_Compound_متن[i][n] != 0; n++) {
            اگر (n < مقصد_UBytes_بفر_لمبائی) {
                اگر (مقصد_UBytes_بفر[n] != esc_Seq_Compound_متن[i][n]) {
                    مطابقت_ملا = جھوٹ;
                    توڑو;
                }
            } ورنہ اگر ((ماخذ + (n - انحراف)) >= ماخذ_حد_بندی) {
                *err = U_TRUNCATED_CHAR_ملا;
                مطابقت_ملا = جھوٹ;
                توڑو;
            } ورنہ اگر (*(ماخذ + (n - انحراف)) != esc_Seq_Compound_متن[i][n]) {
                مطابقت_ملا = جھوٹ;
                توڑو;
            }
        }

        اگر (مطابقت_ملا) {
            توڑو;
        }
    }

    اگر (مطابقت_ملا) {
        state = (COMPOUND_متن_CONVERTERS)i;
    }

    واپس state;
}

ساکن خالی U_CALLCONV
Compound_متن_Open(UConverter *cnv, UConverter_بھرو_دلائل *p_دلائل, UError_رمز *error_رمز){
    cnv->extraInfo = uprv_malloc (حجم (UConverter_ڈیٹا_Compound_متن));
    اگر (cnv->extraInfo != خالی_اشاریہ) {
        UConverter_ڈیٹا_Compound_متن *my_Converter_ڈیٹا = (UConverter_ڈیٹا_Compound_متن *) cnv->extraInfo;

        UConverterNamePieces ڈھیر_Pieces;
        UConverter_بھرو_دلائل ڈھیر_دلائل=UCNV_بھرو_دلائل_INITIALIZER;

        my_Converter_ڈیٹا->myConverterArray[COMPOUND_متن_SINGLE_0] = خالی_اشاریہ;
        my_Converter_ڈیٹا->myConverterArray[COMPOUND_متن_SINGLE_1] = ucnv_بھرو_Shared_ڈیٹا("icu-internal-compound-s1", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[COMPOUND_متن_SINGLE_2] = ucnv_بھرو_Shared_ڈیٹا("icu-internal-compound-s2", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[COMPOUND_متن_SINGLE_3] = ucnv_بھرو_Shared_ڈیٹا("icu-internal-compound-s3", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[COMPOUND_متن_DOUBLE_1] = ucnv_بھرو_Shared_ڈیٹا("icu-internal-compound-d1", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[COMPOUND_متن_DOUBLE_2] = ucnv_بھرو_Shared_ڈیٹا("icu-internal-compound-d2", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[COMPOUND_متن_DOUBLE_3] = ucnv_بھرو_Shared_ڈیٹا("icu-internal-compound-d3", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[COMPOUND_متن_DOUBLE_4] = ucnv_بھرو_Shared_ڈیٹا("icu-internal-compound-d4", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[COMPOUND_متن_DOUBLE_5] = ucnv_بھرو_Shared_ڈیٹا("icu-internal-compound-d5", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[COMPOUND_متن_DOUBLE_6] = ucnv_بھرو_Shared_ڈیٹا("icu-internal-compound-d6", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[COMPOUND_متن_DOUBLE_7] = ucnv_بھرو_Shared_ڈیٹا("icu-internal-compound-d7", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[COMPOUND_متن_TRIPLE_DOUBLE] = ucnv_بھرو_Shared_ڈیٹا("icu-internal-compound-t", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);

        my_Converter_ڈیٹا->myConverterArray[IBM_915] = ucnv_بھرو_Shared_ڈیٹا("ibm-915_P100-1995", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[IBM_916] = ucnv_بھرو_Shared_ڈیٹا("ibm-916_P100-1995", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[IBM_914] = ucnv_بھرو_Shared_ڈیٹا("ibm-914_P100-1995", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[IBM_874] = ucnv_بھرو_Shared_ڈیٹا("ibm-874_P100-1995", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[IBM_912] = ucnv_بھرو_Shared_ڈیٹا("ibm-912_P100-1995", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[IBM_913] = ucnv_بھرو_Shared_ڈیٹا("ibm-913_P100-2000", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[ISO_8859_14] = ucnv_بھرو_Shared_ڈیٹا("iso-8859_14-1998", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);
        my_Converter_ڈیٹا->myConverterArray[IBM_923] = ucnv_بھرو_Shared_ڈیٹا("ibm-923_P100-1998", &ڈھیر_Pieces, &ڈھیر_دلائل, error_رمز);

        اگر (U_FAILURE(*error_رمز) || p_دلائل->onlyTestIsLoadable) {
            Compound_متن_Close(cnv);
            واپس;
        }

        my_Converter_ڈیٹا->state = (COMPOUND_متن_CONVERTERS)0;
    } ورنہ {
        *error_رمز = U_حافظہ_تخصیص_ERROR;
    }
}


ساکن خالی U_CALLCONV
Compound_متن_Close(UConverter *converter) {
    UConverter_ڈیٹا_Compound_متن* my_Converter_ڈیٹا = (UConverter_ڈیٹا_Compound_متن*)(converter->extraInfo);
    int32_t i;

    اگر (converter->extraInfo != خالی_اشاریہ) {
        /*close the array of converter pointers and free the memory*/
        برائے (i = 0; i < عدد_کا_CONVERTERS; i++) {
            اگر (my_Converter_ڈیٹا->myConverterArray[i] != خالی_اشاریہ) {
                ucnv_unload_Shared_ڈیٹا_If_Ready(my_Converter_ڈیٹا->myConverterArray[i]);
            }
        }

        uprv_free(converter->extraInfo);
        converter->extraInfo = خالی_اشاریہ;
    }
}

ساکن خالی U_CALLCONV
Compound_متن_Reset(UConverter *converter, UConverterResetChoice choice) {
    (خالی)converter;
    (خالی)choice;
}

ساکن ثابت حرف* U_CALLCONV
_CompoundTextgetName(ثابت UConverter* cnv){
    (خالی)cnv;
    واپس "x11-compound-text";
}

ساکن خالی U_CALLCONV
UConverter_from_Unicode_Compound_متن_OFFSETS(UConverter_From_Unicode_دلائل* دلائل_2, UError_رمز* err){
    UConverter *cnv = دلائل_2->converter;
    uint8_t *ہدف = (uint8_t *) دلائل_2->ہدف;
    ثابت uint8_t *ہدف_حد_بندی = (ثابت uint8_t *) دلائل_2->ہدف_حد_بندی;
    ثابت char16_t* ماخذ = دلائل_2->ماخذ;
    ثابت char16_t* ماخذ_حد_بندی = دلائل_2->ماخذ_حد_بندی;
    /* int32_t* offsets = args->offsets; */
    UChar32 ماخذ_Char;
    UBool useFallback = cnv->useFallback;
    uint8_t tmp_ہدف_بفر[7];
    int32_t tmp_ہدف_بفر_لمبائی = 0;
    COMPOUND_متن_CONVERTERS currentState, tmpState;
    uint32_t p_قدر;
    int32_t p_قدر_لمبائی = 0;
    int32_t i, n, j;

    UConverter_ڈیٹا_Compound_متن *my_Converter_ڈیٹا = (UConverter_ڈیٹا_Compound_متن *) cnv->extraInfo;

    currentState = my_Converter_ڈیٹا->state;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    اگر((ماخذ_Char = cnv->fromUChar32)!=0 && ہدف< ہدف_حد_بندی) {
        جاؤ getTrail;
    }

    جب( ماخذ < ماخذ_حد_بندی){
        اگر(ہدف < ہدف_حد_بندی){

            ماخذ_Char  = *(ماخذ++);
            /*check if the char is a First surrogate*/
             اگر(U16_IS_SURROGATE(ماخذ_Char)) {
                اگر(U16_IS_SURROGATE_LEAD(ماخذ_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    اگر(ماخذ < ماخذ_حد_بندی) {
                        /* test the following code unit */
                        char16_t trail = *ماخذ;
                        اگر(U16_IS_TRAIL(trail)) {
                            ماخذ++;
                            ماخذ_Char=U16_GET_SUPPLEMENTARY(ماخذ_Char, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } ورنہ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_ملا;
                            cnv->fromUChar32=ماخذ_Char;
                            توڑو;
                        }
                    } ورنہ {
                        /* no more input */
                        cnv->fromUChar32=ماخذ_Char;
                        توڑو;
                    }
                } ورنہ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_ملا;
                    cnv->fromUChar32=ماخذ_Char;
                    توڑو;
                }
            }

             tmp_ہدف_بفر_لمبائی = 0;
             tmpState = getState(ماخذ_Char);

             اگر (tmpState != DO_تلاش && currentState != tmpState) {
                 /* Get escape sequence if necessary */
                 currentState = tmpState;
                 برائے (i = 0; esc_Seq_Compound_متن[currentState][i] != 0; i++) {
                     tmp_ہدف_بفر[tmp_ہدف_بفر_لمبائی++] = esc_Seq_Compound_متن[currentState][i];
                 }
             }

             اگر (tmpState == DO_تلاش) {
                 /* Test all available converters */
                 برائے (i = 1; i < تلاش_لمبائی; i++) {
                     p_قدر_لمبائی = ucnv_MBCSFromUChar32(my_Converter_ڈیٹا->myConverterArray[i], ماخذ_Char, &p_قدر, useFallback);
                     اگر (p_قدر_لمبائی > 0) {
                         tmpState = (COMPOUND_متن_CONVERTERS)i;
                         اگر (currentState != tmpState) {
                             currentState = tmpState;
                             برائے (j = 0; esc_Seq_Compound_متن[currentState][j] != 0; j++) {
                                 tmp_ہدف_بفر[tmp_ہدف_بفر_لمبائی++] = esc_Seq_Compound_متن[currentState][j];
                             }
                         }
                         برائے (n = (p_قدر_لمبائی - 1); n >= 0; n--) {
                             tmp_ہدف_بفر[tmp_ہدف_بفر_لمبائی++] = (uint8_t)(p_قدر >> (n * 8));
                         }
                         توڑو;
                     }
                 }
             } ورنہ اگر (tmpState == COMPOUND_متن_SINGLE_0) {
                 tmp_ہدف_بفر[tmp_ہدف_بفر_لمبائی++] = (uint8_t)ماخذ_Char;
             } ورنہ {
                 p_قدر_لمبائی = ucnv_MBCSFromUChar32(my_Converter_ڈیٹا->myConverterArray[currentState], ماخذ_Char, &p_قدر, useFallback);
                 اگر (p_قدر_لمبائی > 0) {
                     برائے (n = (p_قدر_لمبائی - 1); n >= 0; n--) {
                         tmp_ہدف_بفر[tmp_ہدف_بفر_لمبائی++] = (uint8_t)(p_قدر >> (n * 8));
                     }
                 }
             }

             برائے (i = 0; i < tmp_ہدف_بفر_لمبائی; i++) {
                 اگر (ہدف < ہدف_حد_بندی) {
                     *ہدف++ = tmp_ہدف_بفر[i];
                 } ورنہ {
                     *err = U_بفر_حد_سے_تجاوز_ERROR;
                     توڑو;
                 }
             }

             اگر (*err == U_بفر_حد_سے_تجاوز_ERROR) {
                 برائے (; i < tmp_ہدف_بفر_لمبائی; i++) {
                     دلائل_2->converter->char_Error_بفر[دلائل_2->converter->char_Error_بفر_لمبائی++] = tmp_ہدف_بفر[i];
                 }
             }
        } ورنہ {
            *err = U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
        }
    }

    /*save the state and return */
    my_Converter_ڈیٹا->state = currentState;
    دلائل_2->ماخذ = ماخذ;
    دلائل_2->ہدف = (حرف*)ہدف;
}


ساکن خالی U_CALLCONV
UConverter_مقصد_Unicode_Compound_متن_OFFSETS(UConverter_مقصد_Unicode_دلائل *دلائل_2,
                                               UError_رمز* err){
    ثابت حرف *my_ماخذ = (حرف *) دلائل_2->ماخذ;
    char16_t *my_ہدف = دلائل_2->ہدف;
    ثابت حرف *my_ماخذ_حد_بندی = دلائل_2->ماخذ_حد_بندی;
    ثابت حرف *tmp_ماخذ_حد_بندی = my_ماخذ_حد_بندی;
    uint32_t my_ماخذ_Char = 0x0000;
    COMPOUND_متن_CONVERTERS currentState, tmpState;
    int32_t ماخذ_انحراف = 0;
    UConverter_ڈیٹا_Compound_متن *my_Converter_ڈیٹا = (UConverter_ڈیٹا_Compound_متن *) دلائل_2->converter->extraInfo;
    UConverter_Shared_ڈیٹا* saved_Shared_ڈیٹا = خالی_اشاریہ;

    UConverter_مقصد_Unicode_دلائل sub_دلائل;
    int32_t min_دلائل_سائز;

    /* set up the subconverter arguments */
    اگر(دلائل_2->size<حجم(UConverter_مقصد_Unicode_دلائل)) {
        min_دلائل_سائز = دلائل_2->size;
    } ورنہ {
        min_دلائل_سائز = (int32_t)حجم(UConverter_مقصد_Unicode_دلائل);
    }

    uprv_memcpy(&sub_دلائل, دلائل_2, min_دلائل_سائز);
    sub_دلائل.size = (uint16_t)min_دلائل_سائز;

    currentState = tmpState =  my_Converter_ڈیٹا->state;

    جب(my_ماخذ < my_ماخذ_حد_بندی){
        اگر(my_ہدف < دلائل_2->ہدف_حد_بندی){
            اگر (دلائل_2->converter->مقصد_ULength > 0) {
                my_ماخذ_Char = دلائل_2->converter->مقصد_UBytes[0];
            } ورنہ {
                my_ماخذ_Char = (uint8_t)*my_ماخذ;
            }

            اگر (my_ماخذ_Char == ESC_آغاز) {
                tmpState = ڈھونڈو_State_From_Esc_Seq(my_ماخذ, my_ماخذ_حد_بندی, دلائل_2->converter->مقصد_UBytes, دلائل_2->converter->مقصد_ULength, err);

                اگر (*err == U_TRUNCATED_CHAR_ملا) {
                    برائے (; my_ماخذ < my_ماخذ_حد_بندی;) {
                        دلائل_2->converter->مقصد_UBytes[دلائل_2->converter->مقصد_ULength++] = *my_ماخذ++;
                    }
                    *err = U_ZERO_ERROR;
                    توڑو;
                } ورنہ اگر (tmpState == INVALID) {
                    اگر (دلائل_2->converter->مقصد_ULength == 0) {
                        my_ماخذ++; /* skip over the 0x1b byte */
                    }
                    *err = U_ILLEGAL_CHAR_ملا;
                    توڑو;
                }

                اگر (tmpState != currentState) {
                    currentState = tmpState;
                }

                ماخذ_انحراف = static_cast<int32_t>(uprv_strlen((حرف*)esc_Seq_Compound_متن[currentState]) - دلائل_2->converter->مقصد_ULength);

                my_ماخذ += ماخذ_انحراف;

                دلائل_2->converter->مقصد_ULength = 0;
            }

            اگر (currentState == COMPOUND_متن_SINGLE_0) {
                جب (my_ماخذ < my_ماخذ_حد_بندی) {
                    اگر (*my_ماخذ == ESC_آغاز) {
                        توڑو;
                    }
                    اگر (my_ہدف < دلائل_2->ہدف_حد_بندی) {
                        *my_ہدف++ = 0x00ff&(*my_ماخذ++);
                    } ورنہ {
                        *err = U_بفر_حد_سے_تجاوز_ERROR;
                        توڑو;
                    }
                }
            } ورنہ اگر (my_ماخذ < my_ماخذ_حد_بندی){
                ماخذ_انحراف = ڈھونڈو_اگلا_Esc(my_ماخذ, my_ماخذ_حد_بندی);

                tmp_ماخذ_حد_بندی = my_ماخذ + ماخذ_انحراف;

                sub_دلائل.ماخذ = my_ماخذ;
                sub_دلائل.ماخذ_حد_بندی = tmp_ماخذ_حد_بندی;
                sub_دلائل.ہدف = my_ہدف;
                saved_Shared_ڈیٹا = sub_دلائل.converter->shared_ڈیٹا;
                sub_دلائل.converter->shared_ڈیٹا = my_Converter_ڈیٹا->myConverterArray[currentState];

                ucnv_MBCSToUnicodeWithOffsets(&sub_دلائل, err);

                sub_دلائل.converter->shared_ڈیٹا = saved_Shared_ڈیٹا;

                my_ماخذ = sub_دلائل.ماخذ;
                my_ہدف = sub_دلائل.ہدف;

                اگر (U_FAILURE(*err)) {
                    اگر(*err == U_بفر_حد_سے_تجاوز_ERROR) {
                        اگر(sub_دلائل.converter->UChar_Error_بفر_لمبائی > 0) {
                            uprv_memcpy(دلائل_2->converter->UChar_Error_بفر, sub_دلائل.converter->UChar_Error_بفر,
                                        sub_دلائل.converter->UChar_Error_بفر_لمبائی);
                        }
                        دلائل_2->converter->UChar_Error_بفر_لمبائی=sub_دلائل.converter->UChar_Error_بفر_لمبائی;
                        sub_دلائل.converter->UChar_Error_بفر_لمبائی = 0;
                    }
                    توڑو;
                }
            }
        } ورنہ {
            *err = U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
        }
    }
    my_Converter_ڈیٹا->state = currentState;
    دلائل_2->ہدف = my_ہدف;
    دلائل_2->ماخذ = my_ماخذ;
}

ساکن خالی U_CALLCONV
Compound_متن_Get_Unicode_سیٹ(ثابت UConverter *cnv,
                    ثابت USetAdder *sa,
                    UConverter_Unicode_سیٹ which,
                    UError_رمز *p_Error_رمز) {
    UConverter_ڈیٹا_Compound_متن *my_Converter_ڈیٹا = (UConverter_ڈیٹا_Compound_متن *)cnv->extraInfo;
    int32_t i;

    برائے (i = 1; i < عدد_کا_CONVERTERS; i++) {
        ucnv_MBCSGet_Unicode_سیٹ_For_Unicode(my_Converter_ڈیٹا->myConverterArray[i], sa, which, p_Error_رمز);
    }
    sa->جوڑو(sa->سیٹ, 0x0000);
    sa->جوڑو(sa->سیٹ, 0x0009);
    sa->جوڑو(sa->سیٹ, 0x000A);
    sa->جوڑو_Range(sa->سیٹ, 0x0020, 0x007F);
    sa->جوڑو_Range(sa->سیٹ, 0x00A0, 0x00FF);
}
U_CDECL_انتہا

ساکن ثابت UConverterImpl Compound_متن_Impl = {

    UCNV_COMPOUND_متن,

    خالی_اشاریہ,
    خالی_اشاریہ,

    Compound_متن_Open,
    Compound_متن_Close,
    Compound_متن_Reset,

    UConverter_مقصد_Unicode_Compound_متن_OFFSETS,
    UConverter_مقصد_Unicode_Compound_متن_OFFSETS,
    UConverter_from_Unicode_Compound_متن_OFFSETS,
    UConverter_from_Unicode_Compound_متن_OFFSETS,
    خالی_اشاریہ,

    خالی_اشاریہ,
    _CompoundTextgetName,
    خالی_اشاریہ,
    خالی_اشاریہ,
    Compound_متن_Get_Unicode_سیٹ,
    خالی_اشاریہ,
    خالی_اشاریہ
};

ساکن ثابت UConverter_Static_ڈیٹا Compound_متن_Static_ڈیٹا = {
    حجم(UConverter_Static_ڈیٹا),
    "COMPOUND_TEXT",
    0,
    UCNV_IBM,
    UCNV_COMPOUND_متن,
    1,
    6,
    { 0xef, 0, 0, 0 },
    1,
    جھوٹ,
    جھوٹ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};
ثابت UConverter_Shared_ڈیٹا Compound_متن_ڈیٹا =
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&Compound_متن_Static_ڈیٹا, &Compound_متن_Impl);

#اگر_ختم /* #if !UCONFIG_NO_LEGACY_CONVERSION */
