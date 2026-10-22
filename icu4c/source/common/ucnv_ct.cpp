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
    DO_جستجو = -1,

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
    رقم_از_CONVERTERS = 20
} COMPOUND_متن_CONVERTERS;

#تعریف_کن جستجو_درازی 12

ساكن ثابت uint8_t esc_Seq_Compound_متن[رقم_از_CONVERTERS][5] = {
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

#تعریف_کن ESC_شروع 0x1B

#تعریف_کن isASCIIRange(codepoint) \
        ((codepoint == 0x0000) || (codepoint == 0x0009) || (codepoint == 0x000A) || \
         (codepoint >= 0x0020 && codepoint <= 0x007f) || (codepoint >= 0x00A0 && codepoint <= 0x00FF))

#تعریف_کن isIBM915(codepoint) \
        ((codepoint >= 0x0401 && codepoint <= 0x045F) || (codepoint == 0x2116))

#تعریف_کن isIBM916(codepoint) \
        ((codepoint >= 0x05D0 && codepoint <= 0x05EA) || (codepoint == 0x2017) || (codepoint == 0x203E))

#تعریف_کن isCompoundS3(codepoint) \
        ((codepoint == 0x060C) || (codepoint == 0x061B) || (codepoint == 0x061F) || (codepoint >= 0x0621 && codepoint <= 0x063A) || \
         (codepoint >= 0x0640 && codepoint <= 0x0652) || (codepoint >= 0x0660 && codepoint <= 0x066D) || (codepoint == 0x200B) || \
         (codepoint >= 0x0FE70 && codepoint <= 0x0FE72) || (codepoint == 0x0FE74) || (codepoint >= 0x0FE76 && codepoint <= 0x0FEBE))

#تعریف_کن isCompoundS2(codepoint) \
        ((codepoint == 0x02BC) || (codepoint == 0x02BD) || (codepoint >= 0x0384 && codepoint <= 0x03CE) || (codepoint == 0x2015))

#تعریف_کن isIBM914(codepoint) \
        ((codepoint == 0x0100) || (codepoint == 0x0101) || (codepoint == 0x0112) || (codepoint == 0x0113) || (codepoint == 0x0116) || (codepoint == 0x0117) || \
         (codepoint == 0x0122) || (codepoint == 0x0123) || (codepoint >= 0x0128 && codepoint <= 0x012B) || (codepoint == 0x012E) || (codepoint == 0x012F) || \
         (codepoint >= 0x0136 && codepoint <= 0x0138) || (codepoint == 0x013B) || (codepoint == 0x013C) || (codepoint == 0x0145) || (codepoint ==  0x0146) || \
         (codepoint >= 0x014A && codepoint <= 0x014D) || (codepoint == 0x0156) || (codepoint == 0x0157) || (codepoint >= 0x0166 && codepoint <= 0x016B) || \
         (codepoint == 0x0172) || (codepoint == 0x0173))

#تعریف_کن isIBM874(codepoint) \
        ((codepoint >= 0x0E01 && codepoint <= 0x0E3A) || (codepoint >= 0x0E3F && codepoint <= 0x0E5B))

#تعریف_کن isIBM912(codepoint) \
        ((codepoint >= 0x0102 && codepoint <= 0x0107) || (codepoint >= 0x010C && codepoint <= 0x0111) || (codepoint >= 0x0118 && codepoint <= 0x011B) || \
         (codepoint == 0x0139) || (codepoint == 0x013A) || (codepoint == 0x013D) || (codepoint == 0x013E) || (codepoint >= 0x0141 && codepoint <= 0x0144) || \
         (codepoint == 0x0147) || (codepoint == 0x0147) || (codepoint == 0x0150) || (codepoint == 0x0151) || (codepoint == 0x0154) || (codepoint == 0x0155) || \
         (codepoint >= 0x0158 && codepoint <= 0x015B) || (codepoint == 0x015E) || (codepoint == 0x015F) || (codepoint >= 0x0160 && codepoint <= 0x0165) || \
         (codepoint == 0x016E) || (codepoint == 0x016F) || (codepoint == 0x0170) || (codepoint ==  0x0171) || (codepoint >= 0x0179 && codepoint <= 0x017E) || \
         (codepoint == 0x02C7) || (codepoint == 0x02D8) || (codepoint == 0x02D9) || (codepoint == 0x02DB) || (codepoint == 0x02DD))

#تعریف_کن isIBM913(codepoint) \
        ((codepoint >= 0x0108 && codepoint <= 0x010B) || (codepoint == 0x011C) || \
         (codepoint == 0x011D) || (codepoint == 0x0120) || (codepoint == 0x0121) || \
         (codepoint >= 0x0124 && codepoint <= 0x0127) || (codepoint == 0x0134) || (codepoint == 0x0135) || \
         (codepoint == 0x015C) || (codepoint == 0x015D) || (codepoint == 0x016C) || (codepoint ==  0x016D))

#تعریف_کن isCompoundS1(codepoint) \
        ((codepoint == 0x011E) || (codepoint == 0x011F) || (codepoint == 0x0130) || \
         (codepoint == 0x0131) || (codepoint >= 0x0218 && codepoint <= 0x021B))

#تعریف_کن isISO8859_14(codepoint) \
        ((codepoint >= 0x0174 && codepoint <= 0x0177) || (codepoint == 0x1E0A) || \
         (codepoint == 0x1E0B) || (codepoint == 0x1E1E) || (codepoint == 0x1E1F) || \
         (codepoint == 0x1E40) || (codepoint == 0x1E41) || (codepoint == 0x1E56) || \
         (codepoint == 0x1E57) || (codepoint == 0x1E60) || (codepoint == 0x1E61) || \
         (codepoint == 0x1E6A) || (codepoint == 0x1E6B) || (codepoint == 0x1EF2) || \
         (codepoint == 0x1EF3) || (codepoint >= 0x1E80 && codepoint <= 0x1E85))

#تعریف_کن isIBM923(codepoint) \
        ((codepoint == 0x0152) || (codepoint == 0x0153) || (codepoint == 0x0178) || (codepoint == 0x20AC))


تعريفنوع بنية{
    UConverter_Shared_داده *myConverterArray[رقم_از_CONVERTERS];
    COMPOUND_متن_CONVERTERS state;
} UConverter_داده_Compound_متن;

/*********** Compound Text Converter Protos ***********/
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
Compound_متن_Open(UConverter *cnv, UConverter_بارگیری_آرگومانها *p_آرگومانها, UError_کد *error_کد);

ساكن فراغ U_CALLCONV
 Compound_متن_Close(UConverter *converter);

ساكن فراغ U_CALLCONV
Compound_متن_Reset(UConverter *converter, UConverterResetChoice choice);

ساكن ثابت حرف* U_CALLCONV
_CompoundTextgetName(ثابت UConverter* cnv);


ساكن int32_t پیدا_کن_بعدی_Esc(ثابت حرف *منبع, ثابت حرف *منبع_حد) {
    int32_t length = static_cast<int32_t>(منبع_حد - منبع);
    int32_t i;
    لكل (i = 1; i < length; i++) {
        إذا (*(منبع + i) == 0x1B) {
            أرجع i;
        }
    }

    أرجع length;
}

ساكن COMPOUND_متن_CONVERTERS getState(صحيح codepoint) {
    COMPOUND_متن_CONVERTERS state = DO_جستجو;

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

ساكن COMPOUND_متن_CONVERTERS پیدا_کن_State_From_Esc_Seq(ثابت حرف* منبع, ثابت حرف* منبع_حد, ثابت uint8_t* مقصد_UBytes_میانگیر, int32_t مقصد_UBytes_میانگیر_درازی, UError_کد *err) {
    COMPOUND_متن_CONVERTERS state = INVALID;
    UBool تطابق_یافت = خطأ;
    int32_t i, n, جابه_جایی = مقصد_UBytes_میانگیر_درازی;

    لكل (i = 0; i < رقم_از_CONVERTERS; i++) {
        تطابق_یافت = درست;
        لكل (n = 0; esc_Seq_Compound_متن[i][n] != 0; n++) {
            إذا (n < مقصد_UBytes_میانگیر_درازی) {
                إذا (مقصد_UBytes_میانگیر[n] != esc_Seq_Compound_متن[i][n]) {
                    تطابق_یافت = خطأ;
                    اكسر;
                }
            } وإلا إذا ((منبع + (n - جابه_جایی)) >= منبع_حد) {
                *err = U_TRUNCATED_CHAR_یافت;
                تطابق_یافت = خطأ;
                اكسر;
            } وإلا إذا (*(منبع + (n - جابه_جایی)) != esc_Seq_Compound_متن[i][n]) {
                تطابق_یافت = خطأ;
                اكسر;
            }
        }

        إذا (تطابق_یافت) {
            اكسر;
        }
    }

    إذا (تطابق_یافت) {
        state = (COMPOUND_متن_CONVERTERS)i;
    }

    أرجع state;
}

ساكن فراغ U_CALLCONV
Compound_متن_Open(UConverter *cnv, UConverter_بارگیری_آرگومانها *p_آرگومانها, UError_کد *error_کد){
    cnv->extraInfo = uprv_malloc (حجم (UConverter_داده_Compound_متن));
    إذا (cnv->extraInfo != اشاره_تهی) {
        UConverter_داده_Compound_متن *my_Converter_داده = (UConverter_داده_Compound_متن *) cnv->extraInfo;

        UConverterNamePieces پشته_Pieces;
        UConverter_بارگیری_آرگومانها پشته_آرگومانها=UCNV_بارگیری_آرگومانها_INITIALIZER;

        my_Converter_داده->myConverterArray[COMPOUND_متن_SINGLE_0] = اشاره_تهی;
        my_Converter_داده->myConverterArray[COMPOUND_متن_SINGLE_1] = ucnv_بارگیری_Shared_داده("icu-internal-compound-s1", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[COMPOUND_متن_SINGLE_2] = ucnv_بارگیری_Shared_داده("icu-internal-compound-s2", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[COMPOUND_متن_SINGLE_3] = ucnv_بارگیری_Shared_داده("icu-internal-compound-s3", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[COMPOUND_متن_DOUBLE_1] = ucnv_بارگیری_Shared_داده("icu-internal-compound-d1", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[COMPOUND_متن_DOUBLE_2] = ucnv_بارگیری_Shared_داده("icu-internal-compound-d2", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[COMPOUND_متن_DOUBLE_3] = ucnv_بارگیری_Shared_داده("icu-internal-compound-d3", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[COMPOUND_متن_DOUBLE_4] = ucnv_بارگیری_Shared_داده("icu-internal-compound-d4", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[COMPOUND_متن_DOUBLE_5] = ucnv_بارگیری_Shared_داده("icu-internal-compound-d5", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[COMPOUND_متن_DOUBLE_6] = ucnv_بارگیری_Shared_داده("icu-internal-compound-d6", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[COMPOUND_متن_DOUBLE_7] = ucnv_بارگیری_Shared_داده("icu-internal-compound-d7", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[COMPOUND_متن_TRIPLE_DOUBLE] = ucnv_بارگیری_Shared_داده("icu-internal-compound-t", &پشته_Pieces, &پشته_آرگومانها, error_کد);

        my_Converter_داده->myConverterArray[IBM_915] = ucnv_بارگیری_Shared_داده("ibm-915_P100-1995", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[IBM_916] = ucnv_بارگیری_Shared_داده("ibm-916_P100-1995", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[IBM_914] = ucnv_بارگیری_Shared_داده("ibm-914_P100-1995", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[IBM_874] = ucnv_بارگیری_Shared_داده("ibm-874_P100-1995", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[IBM_912] = ucnv_بارگیری_Shared_داده("ibm-912_P100-1995", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[IBM_913] = ucnv_بارگیری_Shared_داده("ibm-913_P100-2000", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[ISO_8859_14] = ucnv_بارگیری_Shared_داده("iso-8859_14-1998", &پشته_Pieces, &پشته_آرگومانها, error_کد);
        my_Converter_داده->myConverterArray[IBM_923] = ucnv_بارگیری_Shared_داده("ibm-923_P100-1998", &پشته_Pieces, &پشته_آرگومانها, error_کد);

        إذا (U_FAILURE(*error_کد) || p_آرگومانها->onlyTestIsLoadable) {
            Compound_متن_Close(cnv);
            أرجع;
        }

        my_Converter_داده->state = (COMPOUND_متن_CONVERTERS)0;
    } وإلا {
        *error_کد = U_حافظه_تخصیص_ERROR;
    }
}


ساكن فراغ U_CALLCONV
Compound_متن_Close(UConverter *converter) {
    UConverter_داده_Compound_متن* my_Converter_داده = (UConverter_داده_Compound_متن*)(converter->extraInfo);
    int32_t i;

    إذا (converter->extraInfo != اشاره_تهی) {
        /*close the array of converter pointers and free the memory*/
        لكل (i = 0; i < رقم_از_CONVERTERS; i++) {
            إذا (my_Converter_داده->myConverterArray[i] != اشاره_تهی) {
                ucnv_unload_Shared_داده_If_Ready(my_Converter_داده->myConverterArray[i]);
            }
        }

        uprv_free(converter->extraInfo);
        converter->extraInfo = اشاره_تهی;
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
UConverter_from_Unicode_Compound_متن_OFFSETS(UConverter_From_Unicode_آرگومانها* آرگومانها_2, UError_کد* err){
    UConverter *cnv = آرگومانها_2->converter;
    uint8_t *هدف = (uint8_t *) آرگومانها_2->هدف;
    ثابت uint8_t *هدف_حد = (ثابت uint8_t *) آرگومانها_2->هدف_حد;
    ثابت char16_t* منبع = آرگومانها_2->منبع;
    ثابت char16_t* منبع_حد = آرگومانها_2->منبع_حد;
    /* int32_t* offsets = args->offsets; */
    UChar32 منبع_Char;
    UBool useFallback = cnv->useFallback;
    uint8_t tmp_هدف_میانگیر[7];
    int32_t tmp_هدف_میانگیر_درازی = 0;
    COMPOUND_متن_CONVERTERS currentState, tmpState;
    uint32_t p_مقدار;
    int32_t p_مقدار_درازی = 0;
    int32_t i, n, j;

    UConverter_داده_Compound_متن *my_Converter_داده = (UConverter_داده_Compound_متن *) cnv->extraInfo;

    currentState = my_Converter_داده->state;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    إذا((منبع_Char = cnv->fromUChar32)!=0 && هدف< هدف_حد) {
        اذهب getTrail;
    }

    طالما( منبع < منبع_حد){
        إذا(هدف < هدف_حد){

            منبع_Char  = *(منبع++);
            /*check if the char is a First surrogate*/
             إذا(U16_IS_SURROGATE(منبع_Char)) {
                إذا(U16_IS_SURROGATE_LEAD(منبع_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    إذا(منبع < منبع_حد) {
                        /* test the following code unit */
                        char16_t trail = *منبع;
                        إذا(U16_IS_TRAIL(trail)) {
                            منبع++;
                            منبع_Char=U16_GET_SUPPLEMENTARY(منبع_Char, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_یافت;
                            cnv->fromUChar32=منبع_Char;
                            اكسر;
                        }
                    } وإلا {
                        /* no more input */
                        cnv->fromUChar32=منبع_Char;
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_یافت;
                    cnv->fromUChar32=منبع_Char;
                    اكسر;
                }
            }

             tmp_هدف_میانگیر_درازی = 0;
             tmpState = getState(منبع_Char);

             إذا (tmpState != DO_جستجو && currentState != tmpState) {
                 /* Get escape sequence if necessary */
                 currentState = tmpState;
                 لكل (i = 0; esc_Seq_Compound_متن[currentState][i] != 0; i++) {
                     tmp_هدف_میانگیر[tmp_هدف_میانگیر_درازی++] = esc_Seq_Compound_متن[currentState][i];
                 }
             }

             إذا (tmpState == DO_جستجو) {
                 /* Test all available converters */
                 لكل (i = 1; i < جستجو_درازی; i++) {
                     p_مقدار_درازی = ucnv_MBCSFromUChar32(my_Converter_داده->myConverterArray[i], منبع_Char, &p_مقدار, useFallback);
                     إذا (p_مقدار_درازی > 0) {
                         tmpState = (COMPOUND_متن_CONVERTERS)i;
                         إذا (currentState != tmpState) {
                             currentState = tmpState;
                             لكل (j = 0; esc_Seq_Compound_متن[currentState][j] != 0; j++) {
                                 tmp_هدف_میانگیر[tmp_هدف_میانگیر_درازی++] = esc_Seq_Compound_متن[currentState][j];
                             }
                         }
                         لكل (n = (p_مقدار_درازی - 1); n >= 0; n--) {
                             tmp_هدف_میانگیر[tmp_هدف_میانگیر_درازی++] = (uint8_t)(p_مقدار >> (n * 8));
                         }
                         اكسر;
                     }
                 }
             } وإلا إذا (tmpState == COMPOUND_متن_SINGLE_0) {
                 tmp_هدف_میانگیر[tmp_هدف_میانگیر_درازی++] = (uint8_t)منبع_Char;
             } وإلا {
                 p_مقدار_درازی = ucnv_MBCSFromUChar32(my_Converter_داده->myConverterArray[currentState], منبع_Char, &p_مقدار, useFallback);
                 إذا (p_مقدار_درازی > 0) {
                     لكل (n = (p_مقدار_درازی - 1); n >= 0; n--) {
                         tmp_هدف_میانگیر[tmp_هدف_میانگیر_درازی++] = (uint8_t)(p_مقدار >> (n * 8));
                     }
                 }
             }

             لكل (i = 0; i < tmp_هدف_میانگیر_درازی; i++) {
                 إذا (هدف < هدف_حد) {
                     *هدف++ = tmp_هدف_میانگیر[i];
                 } وإلا {
                     *err = U_میانگیر_سرریز_ERROR;
                     اكسر;
                 }
             }

             إذا (*err == U_میانگیر_سرریز_ERROR) {
                 لكل (; i < tmp_هدف_میانگیر_درازی; i++) {
                     آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] = tmp_هدف_میانگیر[i];
                 }
             }
        } وإلا {
            *err = U_میانگیر_سرریز_ERROR;
            اكسر;
        }
    }

    /*save the state and return */
    my_Converter_داده->state = currentState;
    آرگومانها_2->منبع = منبع;
    آرگومانها_2->هدف = (حرف*)هدف;
}


ساكن فراغ U_CALLCONV
UConverter_مقصد_Unicode_Compound_متن_OFFSETS(UConverter_مقصد_Unicode_آرگومانها *آرگومانها_2,
                                               UError_کد* err){
    ثابت حرف *my_منبع = (حرف *) آرگومانها_2->منبع;
    char16_t *my_هدف = آرگومانها_2->هدف;
    ثابت حرف *my_منبع_حد = آرگومانها_2->منبع_حد;
    ثابت حرف *tmp_منبع_حد = my_منبع_حد;
    uint32_t my_منبع_Char = 0x0000;
    COMPOUND_متن_CONVERTERS currentState, tmpState;
    int32_t منبع_جابه_جایی = 0;
    UConverter_داده_Compound_متن *my_Converter_داده = (UConverter_داده_Compound_متن *) آرگومانها_2->converter->extraInfo;
    UConverter_Shared_داده* saved_Shared_داده = اشاره_تهی;

    UConverter_مقصد_Unicode_آرگومانها sub_آرگومانها;
    int32_t min_آرگومانها_اندازه;

    /* set up the subconverter arguments */
    إذا(آرگومانها_2->size<حجم(UConverter_مقصد_Unicode_آرگومانها)) {
        min_آرگومانها_اندازه = آرگومانها_2->size;
    } وإلا {
        min_آرگومانها_اندازه = (int32_t)حجم(UConverter_مقصد_Unicode_آرگومانها);
    }

    uprv_memcpy(&sub_آرگومانها, آرگومانها_2, min_آرگومانها_اندازه);
    sub_آرگومانها.size = (uint16_t)min_آرگومانها_اندازه;

    currentState = tmpState =  my_Converter_داده->state;

    طالما(my_منبع < my_منبع_حد){
        إذا(my_هدف < آرگومانها_2->هدف_حد){
            إذا (آرگومانها_2->converter->مقصد_ULength > 0) {
                my_منبع_Char = آرگومانها_2->converter->مقصد_UBytes[0];
            } وإلا {
                my_منبع_Char = (uint8_t)*my_منبع;
            }

            إذا (my_منبع_Char == ESC_شروع) {
                tmpState = پیدا_کن_State_From_Esc_Seq(my_منبع, my_منبع_حد, آرگومانها_2->converter->مقصد_UBytes, آرگومانها_2->converter->مقصد_ULength, err);

                إذا (*err == U_TRUNCATED_CHAR_یافت) {
                    لكل (; my_منبع < my_منبع_حد;) {
                        آرگومانها_2->converter->مقصد_UBytes[آرگومانها_2->converter->مقصد_ULength++] = *my_منبع++;
                    }
                    *err = U_ZERO_ERROR;
                    اكسر;
                } وإلا إذا (tmpState == INVALID) {
                    إذا (آرگومانها_2->converter->مقصد_ULength == 0) {
                        my_منبع++; /* skip over the 0x1b byte */
                    }
                    *err = U_ILLEGAL_CHAR_یافت;
                    اكسر;
                }

                إذا (tmpState != currentState) {
                    currentState = tmpState;
                }

                منبع_جابه_جایی = static_cast<int32_t>(uprv_strlen((حرف*)esc_Seq_Compound_متن[currentState]) - آرگومانها_2->converter->مقصد_ULength);

                my_منبع += منبع_جابه_جایی;

                آرگومانها_2->converter->مقصد_ULength = 0;
            }

            إذا (currentState == COMPOUND_متن_SINGLE_0) {
                طالما (my_منبع < my_منبع_حد) {
                    إذا (*my_منبع == ESC_شروع) {
                        اكسر;
                    }
                    إذا (my_هدف < آرگومانها_2->هدف_حد) {
                        *my_هدف++ = 0x00ff&(*my_منبع++);
                    } وإلا {
                        *err = U_میانگیر_سرریز_ERROR;
                        اكسر;
                    }
                }
            } وإلا إذا (my_منبع < my_منبع_حد){
                منبع_جابه_جایی = پیدا_کن_بعدی_Esc(my_منبع, my_منبع_حد);

                tmp_منبع_حد = my_منبع + منبع_جابه_جایی;

                sub_آرگومانها.منبع = my_منبع;
                sub_آرگومانها.منبع_حد = tmp_منبع_حد;
                sub_آرگومانها.هدف = my_هدف;
                saved_Shared_داده = sub_آرگومانها.converter->shared_داده;
                sub_آرگومانها.converter->shared_داده = my_Converter_داده->myConverterArray[currentState];

                ucnv_MBCSToUnicodeWithOffsets(&sub_آرگومانها, err);

                sub_آرگومانها.converter->shared_داده = saved_Shared_داده;

                my_منبع = sub_آرگومانها.منبع;
                my_هدف = sub_آرگومانها.هدف;

                إذا (U_FAILURE(*err)) {
                    إذا(*err == U_میانگیر_سرریز_ERROR) {
                        إذا(sub_آرگومانها.converter->UChar_Error_میانگیر_درازی > 0) {
                            uprv_memcpy(آرگومانها_2->converter->UChar_Error_میانگیر, sub_آرگومانها.converter->UChar_Error_میانگیر,
                                        sub_آرگومانها.converter->UChar_Error_میانگیر_درازی);
                        }
                        آرگومانها_2->converter->UChar_Error_میانگیر_درازی=sub_آرگومانها.converter->UChar_Error_میانگیر_درازی;
                        sub_آرگومانها.converter->UChar_Error_میانگیر_درازی = 0;
                    }
                    اكسر;
                }
            }
        } وإلا {
            *err = U_میانگیر_سرریز_ERROR;
            اكسر;
        }
    }
    my_Converter_داده->state = currentState;
    آرگومانها_2->هدف = my_هدف;
    آرگومانها_2->منبع = my_منبع;
}

ساكن فراغ U_CALLCONV
Compound_متن_Get_Unicode_مجموعه(ثابت UConverter *cnv,
                    ثابت USetAdder *sa,
                    UConverter_Unicode_مجموعه which,
                    UError_کد *p_Error_کد) {
    UConverter_داده_Compound_متن *my_Converter_داده = (UConverter_داده_Compound_متن *)cnv->extraInfo;
    int32_t i;

    لكل (i = 1; i < رقم_از_CONVERTERS; i++) {
        ucnv_MBCSGet_Unicode_مجموعه_For_Unicode(my_Converter_داده->myConverterArray[i], sa, which, p_Error_کد);
    }
    sa->جمع_کن(sa->مجموعه, 0x0000);
    sa->جمع_کن(sa->مجموعه, 0x0009);
    sa->جمع_کن(sa->مجموعه, 0x000A);
    sa->جمع_کن_Range(sa->مجموعه, 0x0020, 0x007F);
    sa->جمع_کن_Range(sa->مجموعه, 0x00A0, 0x00FF);
}
U_CDECL_انتها

ساكن ثابت UConverterImpl Compound_متن_Impl = {

    UCNV_COMPOUND_متن,

    اشاره_تهی,
    اشاره_تهی,

    Compound_متن_Open,
    Compound_متن_Close,
    Compound_متن_Reset,

    UConverter_مقصد_Unicode_Compound_متن_OFFSETS,
    UConverter_مقصد_Unicode_Compound_متن_OFFSETS,
    UConverter_from_Unicode_Compound_متن_OFFSETS,
    UConverter_from_Unicode_Compound_متن_OFFSETS,
    اشاره_تهی,

    اشاره_تهی,
    _CompoundTextgetName,
    اشاره_تهی,
    اشاره_تهی,
    Compound_متن_Get_Unicode_مجموعه,
    اشاره_تهی,
    اشاره_تهی
};

ساكن ثابت UConverter_Static_داده Compound_متن_Static_داده = {
    حجم(UConverter_Static_داده),
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
ثابت UConverter_Shared_داده Compound_متن_داده =
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&Compound_متن_Static_داده, &Compound_متن_Impl);

#پایان_اگر /* #if !UCONFIG_NO_LEGACY_CONVERSION */
