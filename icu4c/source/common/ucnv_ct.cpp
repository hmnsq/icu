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
    DO_پلټنه = -1,

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
    عدد_د_CONVERTERS = 20
} COMPOUND_متن_CONVERTERS;

#تعریف_کړه پلټنه_اوږدتیا 12

ساكن ثابت uint8_t esc_Seq_Compound_متن[عدد_د_CONVERTERS][5] = {
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

#تعریف_کړه ESC_پیل 0x1B

#تعریف_کړه isASCIIRange(codepoint) \
        ((codepoint == 0x0000) || (codepoint == 0x0009) || (codepoint == 0x000A) || \
         (codepoint >= 0x0020 && codepoint <= 0x007f) || (codepoint >= 0x00A0 && codepoint <= 0x00FF))

#تعریف_کړه isIBM915(codepoint) \
        ((codepoint >= 0x0401 && codepoint <= 0x045F) || (codepoint == 0x2116))

#تعریف_کړه isIBM916(codepoint) \
        ((codepoint >= 0x05D0 && codepoint <= 0x05EA) || (codepoint == 0x2017) || (codepoint == 0x203E))

#تعریف_کړه isCompoundS3(codepoint) \
        ((codepoint == 0x060C) || (codepoint == 0x061B) || (codepoint == 0x061F) || (codepoint >= 0x0621 && codepoint <= 0x063A) || \
         (codepoint >= 0x0640 && codepoint <= 0x0652) || (codepoint >= 0x0660 && codepoint <= 0x066D) || (codepoint == 0x200B) || \
         (codepoint >= 0x0FE70 && codepoint <= 0x0FE72) || (codepoint == 0x0FE74) || (codepoint >= 0x0FE76 && codepoint <= 0x0FEBE))

#تعریف_کړه isCompoundS2(codepoint) \
        ((codepoint == 0x02BC) || (codepoint == 0x02BD) || (codepoint >= 0x0384 && codepoint <= 0x03CE) || (codepoint == 0x2015))

#تعریف_کړه isIBM914(codepoint) \
        ((codepoint == 0x0100) || (codepoint == 0x0101) || (codepoint == 0x0112) || (codepoint == 0x0113) || (codepoint == 0x0116) || (codepoint == 0x0117) || \
         (codepoint == 0x0122) || (codepoint == 0x0123) || (codepoint >= 0x0128 && codepoint <= 0x012B) || (codepoint == 0x012E) || (codepoint == 0x012F) || \
         (codepoint >= 0x0136 && codepoint <= 0x0138) || (codepoint == 0x013B) || (codepoint == 0x013C) || (codepoint == 0x0145) || (codepoint ==  0x0146) || \
         (codepoint >= 0x014A && codepoint <= 0x014D) || (codepoint == 0x0156) || (codepoint == 0x0157) || (codepoint >= 0x0166 && codepoint <= 0x016B) || \
         (codepoint == 0x0172) || (codepoint == 0x0173))

#تعریف_کړه isIBM874(codepoint) \
        ((codepoint >= 0x0E01 && codepoint <= 0x0E3A) || (codepoint >= 0x0E3F && codepoint <= 0x0E5B))

#تعریف_کړه isIBM912(codepoint) \
        ((codepoint >= 0x0102 && codepoint <= 0x0107) || (codepoint >= 0x010C && codepoint <= 0x0111) || (codepoint >= 0x0118 && codepoint <= 0x011B) || \
         (codepoint == 0x0139) || (codepoint == 0x013A) || (codepoint == 0x013D) || (codepoint == 0x013E) || (codepoint >= 0x0141 && codepoint <= 0x0144) || \
         (codepoint == 0x0147) || (codepoint == 0x0147) || (codepoint == 0x0150) || (codepoint == 0x0151) || (codepoint == 0x0154) || (codepoint == 0x0155) || \
         (codepoint >= 0x0158 && codepoint <= 0x015B) || (codepoint == 0x015E) || (codepoint == 0x015F) || (codepoint >= 0x0160 && codepoint <= 0x0165) || \
         (codepoint == 0x016E) || (codepoint == 0x016F) || (codepoint == 0x0170) || (codepoint ==  0x0171) || (codepoint >= 0x0179 && codepoint <= 0x017E) || \
         (codepoint == 0x02C7) || (codepoint == 0x02D8) || (codepoint == 0x02D9) || (codepoint == 0x02DB) || (codepoint == 0x02DD))

#تعریف_کړه isIBM913(codepoint) \
        ((codepoint >= 0x0108 && codepoint <= 0x010B) || (codepoint == 0x011C) || \
         (codepoint == 0x011D) || (codepoint == 0x0120) || (codepoint == 0x0121) || \
         (codepoint >= 0x0124 && codepoint <= 0x0127) || (codepoint == 0x0134) || (codepoint == 0x0135) || \
         (codepoint == 0x015C) || (codepoint == 0x015D) || (codepoint == 0x016C) || (codepoint ==  0x016D))

#تعریف_کړه isCompoundS1(codepoint) \
        ((codepoint == 0x011E) || (codepoint == 0x011F) || (codepoint == 0x0130) || \
         (codepoint == 0x0131) || (codepoint >= 0x0218 && codepoint <= 0x021B))

#تعریف_کړه isISO8859_14(codepoint) \
        ((codepoint >= 0x0174 && codepoint <= 0x0177) || (codepoint == 0x1E0A) || \
         (codepoint == 0x1E0B) || (codepoint == 0x1E1E) || (codepoint == 0x1E1F) || \
         (codepoint == 0x1E40) || (codepoint == 0x1E41) || (codepoint == 0x1E56) || \
         (codepoint == 0x1E57) || (codepoint == 0x1E60) || (codepoint == 0x1E61) || \
         (codepoint == 0x1E6A) || (codepoint == 0x1E6B) || (codepoint == 0x1EF2) || \
         (codepoint == 0x1EF3) || (codepoint >= 0x1E80 && codepoint <= 0x1E85))

#تعریف_کړه isIBM923(codepoint) \
        ((codepoint == 0x0152) || (codepoint == 0x0153) || (codepoint == 0x0178) || (codepoint == 0x20AC))


تعريفنوع بنية{
    UConverter_Shared_معلومات *myConverterArray[عدد_د_CONVERTERS];
    COMPOUND_متن_CONVERTERS state;
} UConverter_معلومات_Compound_متن;

/*********** Compound Text Converter Protos ***********/
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
Compound_متن_Open(UConverter *cnv, UConverter_بارول_دلیلونه *p_دلیلونه, UError_کوډ *error_کوډ);

ساكن فراغ U_CALLCONV
 Compound_متن_Close(UConverter *converter);

ساكن فراغ U_CALLCONV
Compound_متن_Reset(UConverter *converter, UConverterResetChoice choice);

ساكن ثابت حرف* U_CALLCONV
_CompoundTextgetName(ثابت UConverter* cnv);


ساكن int32_t پیدا_کړه_بل_Esc(ثابت حرف *سرچینه, ثابت حرف *سرچینه_حد) {
    int32_t length = static_cast<int32_t>(سرچینه_حد - سرچینه);
    int32_t i;
    لكل (i = 1; i < length; i++) {
        إذا (*(سرچینه + i) == 0x1B) {
            أرجع i;
        }
    }

    أرجع length;
}

ساكن COMPOUND_متن_CONVERTERS getState(صحيح codepoint) {
    COMPOUND_متن_CONVERTERS state = DO_پلټنه;

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

ساكن COMPOUND_متن_CONVERTERS پیدا_کړه_State_From_Esc_Seq(ثابت حرف* سرچینه, ثابت حرف* سرچینه_حد, ثابت uint8_t* منزل_UBytes_بفر, int32_t منزل_UBytes_بفر_اوږدتیا, UError_کوډ *err) {
    COMPOUND_متن_CONVERTERS state = INVALID;
    UBool سمون_وموندل = خطأ;
    int32_t i, n, دننه_واټن = منزل_UBytes_بفر_اوږدتیا;

    لكل (i = 0; i < عدد_د_CONVERTERS; i++) {
        سمون_وموندل = ریښتیا;
        لكل (n = 0; esc_Seq_Compound_متن[i][n] != 0; n++) {
            إذا (n < منزل_UBytes_بفر_اوږدتیا) {
                إذا (منزل_UBytes_بفر[n] != esc_Seq_Compound_متن[i][n]) {
                    سمون_وموندل = خطأ;
                    اكسر;
                }
            } وإلا إذا ((سرچینه + (n - دننه_واټن)) >= سرچینه_حد) {
                *err = U_TRUNCATED_CHAR_وموندل;
                سمون_وموندل = خطأ;
                اكسر;
            } وإلا إذا (*(سرچینه + (n - دننه_واټن)) != esc_Seq_Compound_متن[i][n]) {
                سمون_وموندل = خطأ;
                اكسر;
            }
        }

        إذا (سمون_وموندل) {
            اكسر;
        }
    }

    إذا (سمون_وموندل) {
        state = (COMPOUND_متن_CONVERTERS)i;
    }

    أرجع state;
}

ساكن فراغ U_CALLCONV
Compound_متن_Open(UConverter *cnv, UConverter_بارول_دلیلونه *p_دلیلونه, UError_کوډ *error_کوډ){
    cnv->extraInfo = uprv_malloc (حجم (UConverter_معلومات_Compound_متن));
    إذا (cnv->extraInfo != تش_اشاره) {
        UConverter_معلومات_Compound_متن *my_Converter_معلومات = (UConverter_معلومات_Compound_متن *) cnv->extraInfo;

        UConverterNamePieces ډېری_Pieces;
        UConverter_بارول_دلیلونه ډېری_دلیلونه=UCNV_بارول_دلیلونه_INITIALIZER;

        my_Converter_معلومات->myConverterArray[COMPOUND_متن_SINGLE_0] = تش_اشاره;
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_SINGLE_1] = ucnv_بارول_Shared_معلومات("icu-internal-compound-s1", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_SINGLE_2] = ucnv_بارول_Shared_معلومات("icu-internal-compound-s2", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_SINGLE_3] = ucnv_بارول_Shared_معلومات("icu-internal-compound-s3", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_DOUBLE_1] = ucnv_بارول_Shared_معلومات("icu-internal-compound-d1", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_DOUBLE_2] = ucnv_بارول_Shared_معلومات("icu-internal-compound-d2", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_DOUBLE_3] = ucnv_بارول_Shared_معلومات("icu-internal-compound-d3", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_DOUBLE_4] = ucnv_بارول_Shared_معلومات("icu-internal-compound-d4", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_DOUBLE_5] = ucnv_بارول_Shared_معلومات("icu-internal-compound-d5", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_DOUBLE_6] = ucnv_بارول_Shared_معلومات("icu-internal-compound-d6", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_DOUBLE_7] = ucnv_بارول_Shared_معلومات("icu-internal-compound-d7", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[COMPOUND_متن_TRIPLE_DOUBLE] = ucnv_بارول_Shared_معلومات("icu-internal-compound-t", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);

        my_Converter_معلومات->myConverterArray[IBM_915] = ucnv_بارول_Shared_معلومات("ibm-915_P100-1995", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[IBM_916] = ucnv_بارول_Shared_معلومات("ibm-916_P100-1995", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[IBM_914] = ucnv_بارول_Shared_معلومات("ibm-914_P100-1995", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[IBM_874] = ucnv_بارول_Shared_معلومات("ibm-874_P100-1995", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[IBM_912] = ucnv_بارول_Shared_معلومات("ibm-912_P100-1995", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[IBM_913] = ucnv_بارول_Shared_معلومات("ibm-913_P100-2000", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[ISO_8859_14] = ucnv_بارول_Shared_معلومات("iso-8859_14-1998", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);
        my_Converter_معلومات->myConverterArray[IBM_923] = ucnv_بارول_Shared_معلومات("ibm-923_P100-1998", &ډېری_Pieces, &ډېری_دلیلونه, error_کوډ);

        إذا (U_FAILURE(*error_کوډ) || p_دلیلونه->onlyTestIsLoadable) {
            Compound_متن_Close(cnv);
            أرجع;
        }

        my_Converter_معلومات->state = (COMPOUND_متن_CONVERTERS)0;
    } وإلا {
        *error_کوډ = U_حافظه_ویش_ERROR;
    }
}


ساكن فراغ U_CALLCONV
Compound_متن_Close(UConverter *converter) {
    UConverter_معلومات_Compound_متن* my_Converter_معلومات = (UConverter_معلومات_Compound_متن*)(converter->extraInfo);
    int32_t i;

    إذا (converter->extraInfo != تش_اشاره) {
        /*close the array of converter pointers and free the memory*/
        لكل (i = 0; i < عدد_د_CONVERTERS; i++) {
            إذا (my_Converter_معلومات->myConverterArray[i] != تش_اشاره) {
                ucnv_unload_Shared_معلومات_If_Ready(my_Converter_معلومات->myConverterArray[i]);
            }
        }

        uprv_free(converter->extraInfo);
        converter->extraInfo = تش_اشاره;
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
UConverter_from_Unicode_Compound_متن_OFFSETS(UConverter_From_Unicode_دلیلونه* دلیلونه_2, UError_کوډ* err){
    UConverter *cnv = دلیلونه_2->converter;
    uint8_t *موخه = (uint8_t *) دلیلونه_2->موخه;
    ثابت uint8_t *موخه_حد = (ثابت uint8_t *) دلیلونه_2->موخه_حد;
    ثابت char16_t* سرچینه = دلیلونه_2->سرچینه;
    ثابت char16_t* سرچینه_حد = دلیلونه_2->سرچینه_حد;
    /* int32_t* offsets = args->offsets; */
    UChar32 سرچینه_Char;
    UBool useFallback = cnv->useFallback;
    uint8_t tmp_موخه_بفر[7];
    int32_t tmp_موخه_بفر_اوږدتیا = 0;
    COMPOUND_متن_CONVERTERS currentState, tmpState;
    uint32_t p_ارزښت;
    int32_t p_ارزښت_اوږدتیا = 0;
    int32_t i, n, j;

    UConverter_معلومات_Compound_متن *my_Converter_معلومات = (UConverter_معلومات_Compound_متن *) cnv->extraInfo;

    currentState = my_Converter_معلومات->state;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    إذا((سرچینه_Char = cnv->fromUChar32)!=0 && موخه< موخه_حد) {
        اذهب getTrail;
    }

    طالما( سرچینه < سرچینه_حد){
        إذا(موخه < موخه_حد){

            سرچینه_Char  = *(سرچینه++);
            /*check if the char is a First surrogate*/
             إذا(U16_IS_SURROGATE(سرچینه_Char)) {
                إذا(U16_IS_SURROGATE_LEAD(سرچینه_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    إذا(سرچینه < سرچینه_حد) {
                        /* test the following code unit */
                        char16_t trail = *سرچینه;
                        إذا(U16_IS_TRAIL(trail)) {
                            سرچینه++;
                            سرچینه_Char=U16_GET_SUPPLEMENTARY(سرچینه_Char, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_وموندل;
                            cnv->fromUChar32=سرچینه_Char;
                            اكسر;
                        }
                    } وإلا {
                        /* no more input */
                        cnv->fromUChar32=سرچینه_Char;
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_وموندل;
                    cnv->fromUChar32=سرچینه_Char;
                    اكسر;
                }
            }

             tmp_موخه_بفر_اوږدتیا = 0;
             tmpState = getState(سرچینه_Char);

             إذا (tmpState != DO_پلټنه && currentState != tmpState) {
                 /* Get escape sequence if necessary */
                 currentState = tmpState;
                 لكل (i = 0; esc_Seq_Compound_متن[currentState][i] != 0; i++) {
                     tmp_موخه_بفر[tmp_موخه_بفر_اوږدتیا++] = esc_Seq_Compound_متن[currentState][i];
                 }
             }

             إذا (tmpState == DO_پلټنه) {
                 /* Test all available converters */
                 لكل (i = 1; i < پلټنه_اوږدتیا; i++) {
                     p_ارزښت_اوږدتیا = ucnv_MBCSFromUChar32(my_Converter_معلومات->myConverterArray[i], سرچینه_Char, &p_ارزښت, useFallback);
                     إذا (p_ارزښت_اوږدتیا > 0) {
                         tmpState = (COMPOUND_متن_CONVERTERS)i;
                         إذا (currentState != tmpState) {
                             currentState = tmpState;
                             لكل (j = 0; esc_Seq_Compound_متن[currentState][j] != 0; j++) {
                                 tmp_موخه_بفر[tmp_موخه_بفر_اوږدتیا++] = esc_Seq_Compound_متن[currentState][j];
                             }
                         }
                         لكل (n = (p_ارزښت_اوږدتیا - 1); n >= 0; n--) {
                             tmp_موخه_بفر[tmp_موخه_بفر_اوږدتیا++] = (uint8_t)(p_ارزښت >> (n * 8));
                         }
                         اكسر;
                     }
                 }
             } وإلا إذا (tmpState == COMPOUND_متن_SINGLE_0) {
                 tmp_موخه_بفر[tmp_موخه_بفر_اوږدتیا++] = (uint8_t)سرچینه_Char;
             } وإلا {
                 p_ارزښت_اوږدتیا = ucnv_MBCSFromUChar32(my_Converter_معلومات->myConverterArray[currentState], سرچینه_Char, &p_ارزښت, useFallback);
                 إذا (p_ارزښت_اوږدتیا > 0) {
                     لكل (n = (p_ارزښت_اوږدتیا - 1); n >= 0; n--) {
                         tmp_موخه_بفر[tmp_موخه_بفر_اوږدتیا++] = (uint8_t)(p_ارزښت >> (n * 8));
                     }
                 }
             }

             لكل (i = 0; i < tmp_موخه_بفر_اوږدتیا; i++) {
                 إذا (موخه < موخه_حد) {
                     *موخه++ = tmp_موخه_بفر[i];
                 } وإلا {
                     *err = U_بفر_ډکېدنه_ERROR;
                     اكسر;
                 }
             }

             إذا (*err == U_بفر_ډکېدنه_ERROR) {
                 لكل (; i < tmp_موخه_بفر_اوږدتیا; i++) {
                     دلیلونه_2->converter->char_Error_بفر[دلیلونه_2->converter->char_Error_بفر_اوږدتیا++] = tmp_موخه_بفر[i];
                 }
             }
        } وإلا {
            *err = U_بفر_ډکېدنه_ERROR;
            اكسر;
        }
    }

    /*save the state and return */
    my_Converter_معلومات->state = currentState;
    دلیلونه_2->سرچینه = سرچینه;
    دلیلونه_2->موخه = (حرف*)موخه;
}


ساكن فراغ U_CALLCONV
UConverter_منزل_Unicode_Compound_متن_OFFSETS(UConverter_منزل_Unicode_دلیلونه *دلیلونه_2,
                                               UError_کوډ* err){
    ثابت حرف *my_سرچینه = (حرف *) دلیلونه_2->سرچینه;
    char16_t *my_موخه = دلیلونه_2->موخه;
    ثابت حرف *my_سرچینه_حد = دلیلونه_2->سرچینه_حد;
    ثابت حرف *tmp_سرچینه_حد = my_سرچینه_حد;
    uint32_t my_سرچینه_Char = 0x0000;
    COMPOUND_متن_CONVERTERS currentState, tmpState;
    int32_t سرچینه_دننه_واټن = 0;
    UConverter_معلومات_Compound_متن *my_Converter_معلومات = (UConverter_معلومات_Compound_متن *) دلیلونه_2->converter->extraInfo;
    UConverter_Shared_معلومات* saved_Shared_معلومات = تش_اشاره;

    UConverter_منزل_Unicode_دلیلونه sub_دلیلونه;
    int32_t min_دلیلونه_اندازه;

    /* set up the subconverter arguments */
    إذا(دلیلونه_2->size<حجم(UConverter_منزل_Unicode_دلیلونه)) {
        min_دلیلونه_اندازه = دلیلونه_2->size;
    } وإلا {
        min_دلیلونه_اندازه = (int32_t)حجم(UConverter_منزل_Unicode_دلیلونه);
    }

    uprv_memcpy(&sub_دلیلونه, دلیلونه_2, min_دلیلونه_اندازه);
    sub_دلیلونه.size = (uint16_t)min_دلیلونه_اندازه;

    currentState = tmpState =  my_Converter_معلومات->state;

    طالما(my_سرچینه < my_سرچینه_حد){
        إذا(my_موخه < دلیلونه_2->موخه_حد){
            إذا (دلیلونه_2->converter->منزل_ULength > 0) {
                my_سرچینه_Char = دلیلونه_2->converter->منزل_UBytes[0];
            } وإلا {
                my_سرچینه_Char = (uint8_t)*my_سرچینه;
            }

            إذا (my_سرچینه_Char == ESC_پیل) {
                tmpState = پیدا_کړه_State_From_Esc_Seq(my_سرچینه, my_سرچینه_حد, دلیلونه_2->converter->منزل_UBytes, دلیلونه_2->converter->منزل_ULength, err);

                إذا (*err == U_TRUNCATED_CHAR_وموندل) {
                    لكل (; my_سرچینه < my_سرچینه_حد;) {
                        دلیلونه_2->converter->منزل_UBytes[دلیلونه_2->converter->منزل_ULength++] = *my_سرچینه++;
                    }
                    *err = U_ZERO_ERROR;
                    اكسر;
                } وإلا إذا (tmpState == INVALID) {
                    إذا (دلیلونه_2->converter->منزل_ULength == 0) {
                        my_سرچینه++; /* skip over the 0x1b byte */
                    }
                    *err = U_ILLEGAL_CHAR_وموندل;
                    اكسر;
                }

                إذا (tmpState != currentState) {
                    currentState = tmpState;
                }

                سرچینه_دننه_واټن = static_cast<int32_t>(uprv_strlen((حرف*)esc_Seq_Compound_متن[currentState]) - دلیلونه_2->converter->منزل_ULength);

                my_سرچینه += سرچینه_دننه_واټن;

                دلیلونه_2->converter->منزل_ULength = 0;
            }

            إذا (currentState == COMPOUND_متن_SINGLE_0) {
                طالما (my_سرچینه < my_سرچینه_حد) {
                    إذا (*my_سرچینه == ESC_پیل) {
                        اكسر;
                    }
                    إذا (my_موخه < دلیلونه_2->موخه_حد) {
                        *my_موخه++ = 0x00ff&(*my_سرچینه++);
                    } وإلا {
                        *err = U_بفر_ډکېدنه_ERROR;
                        اكسر;
                    }
                }
            } وإلا إذا (my_سرچینه < my_سرچینه_حد){
                سرچینه_دننه_واټن = پیدا_کړه_بل_Esc(my_سرچینه, my_سرچینه_حد);

                tmp_سرچینه_حد = my_سرچینه + سرچینه_دننه_واټن;

                sub_دلیلونه.سرچینه = my_سرچینه;
                sub_دلیلونه.سرچینه_حد = tmp_سرچینه_حد;
                sub_دلیلونه.موخه = my_موخه;
                saved_Shared_معلومات = sub_دلیلونه.converter->shared_معلومات;
                sub_دلیلونه.converter->shared_معلومات = my_Converter_معلومات->myConverterArray[currentState];

                ucnv_MBCSToUnicodeWithOffsets(&sub_دلیلونه, err);

                sub_دلیلونه.converter->shared_معلومات = saved_Shared_معلومات;

                my_سرچینه = sub_دلیلونه.سرچینه;
                my_موخه = sub_دلیلونه.موخه;

                إذا (U_FAILURE(*err)) {
                    إذا(*err == U_بفر_ډکېدنه_ERROR) {
                        إذا(sub_دلیلونه.converter->UChar_Error_بفر_اوږدتیا > 0) {
                            uprv_memcpy(دلیلونه_2->converter->UChar_Error_بفر, sub_دلیلونه.converter->UChar_Error_بفر,
                                        sub_دلیلونه.converter->UChar_Error_بفر_اوږدتیا);
                        }
                        دلیلونه_2->converter->UChar_Error_بفر_اوږدتیا=sub_دلیلونه.converter->UChar_Error_بفر_اوږدتیا;
                        sub_دلیلونه.converter->UChar_Error_بفر_اوږدتیا = 0;
                    }
                    اكسر;
                }
            }
        } وإلا {
            *err = U_بفر_ډکېدنه_ERROR;
            اكسر;
        }
    }
    my_Converter_معلومات->state = currentState;
    دلیلونه_2->موخه = my_موخه;
    دلیلونه_2->سرچینه = my_سرچینه;
}

ساكن فراغ U_CALLCONV
Compound_متن_Get_Unicode_سېټ(ثابت UConverter *cnv,
                    ثابت USetAdder *sa,
                    UConverter_Unicode_سېټ which,
                    UError_کوډ *p_Error_کوډ) {
    UConverter_معلومات_Compound_متن *my_Converter_معلومات = (UConverter_معلومات_Compound_متن *)cnv->extraInfo;
    int32_t i;

    لكل (i = 1; i < عدد_د_CONVERTERS; i++) {
        ucnv_MBCSGet_Unicode_سېټ_For_Unicode(my_Converter_معلومات->myConverterArray[i], sa, which, p_Error_کوډ);
    }
    sa->جمع_کړه(sa->سېټ, 0x0000);
    sa->جمع_کړه(sa->سېټ, 0x0009);
    sa->جمع_کړه(sa->سېټ, 0x000A);
    sa->جمع_کړه_Range(sa->سېټ, 0x0020, 0x007F);
    sa->جمع_کړه_Range(sa->سېټ, 0x00A0, 0x00FF);
}
U_CDECL_پای_ټکی

ساكن ثابت UConverterImpl Compound_متن_Impl = {

    UCNV_COMPOUND_متن,

    تش_اشاره,
    تش_اشاره,

    Compound_متن_Open,
    Compound_متن_Close,
    Compound_متن_Reset,

    UConverter_منزل_Unicode_Compound_متن_OFFSETS,
    UConverter_منزل_Unicode_Compound_متن_OFFSETS,
    UConverter_from_Unicode_Compound_متن_OFFSETS,
    UConverter_from_Unicode_Compound_متن_OFFSETS,
    تش_اشاره,

    تش_اشاره,
    _CompoundTextgetName,
    تش_اشاره,
    تش_اشاره,
    Compound_متن_Get_Unicode_سېټ,
    تش_اشاره,
    تش_اشاره
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

#د_که_پای /* #if !UCONFIG_NO_LEGACY_CONVERSION */
