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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "unicode/ucnv_err.h"
#అన్తర్భావ "unicode/ucnv_cb.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "ucnv_imp.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "ucnvmbcs.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cmemory.h"

రకనిర్వచనం గణన {
    INVALID = -2,
    DO_శోధన = -1,

    COMPOUND_పాఠ్యం_SINGLE_0 = 0,
    COMPOUND_పాఠ్యం_SINGLE_1 = 1,
    COMPOUND_పాఠ్యం_SINGLE_2 = 2,
    COMPOUND_పాఠ్యం_SINGLE_3 = 3,

    COMPOUND_పాఠ్యం_DOUBLE_1 = 4,
    COMPOUND_పాఠ్యం_DOUBLE_2 = 5,
    COMPOUND_పాఠ్యం_DOUBLE_3 = 6,
    COMPOUND_పాఠ్యం_DOUBLE_4 = 7,
    COMPOUND_పాఠ్యం_DOUBLE_5 = 8,
    COMPOUND_పాఠ్యం_DOUBLE_6 = 9,
    COMPOUND_పాఠ్యం_DOUBLE_7 = 10,

    COMPOUND_పాఠ్యం_TRIPLE_DOUBLE = 11,

    IBM_915 = 12,
    IBM_916 = 13,
    IBM_914 = 14,
    IBM_874 = 15,
    IBM_912 = 16,
    IBM_913 = 17,
    ISO_8859_14 = 18,
    IBM_923 = 19,
    అంకె_యొక్క_CONVERTERS = 20
} COMPOUND_పాఠ్యం_CONVERTERS;

#నిర్వచించు శోధన_నిడివి 12

స్థిర స్థిరరాశి uint8_t esc_Seq_Compound_పాఠ్యం[అంకె_యొక్క_CONVERTERS][5] = {
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

#నిర్వచించు ESC_ప్రారంభం 0x1B

#నిర్వచించు isASCIIRange(codepoint) \
        ((codepoint == 0x0000) || (codepoint == 0x0009) || (codepoint == 0x000A) || \
         (codepoint >= 0x0020 && codepoint <= 0x007f) || (codepoint >= 0x00A0 && codepoint <= 0x00FF))

#నిర్వచించు isIBM915(codepoint) \
        ((codepoint >= 0x0401 && codepoint <= 0x045F) || (codepoint == 0x2116))

#నిర్వచించు isIBM916(codepoint) \
        ((codepoint >= 0x05D0 && codepoint <= 0x05EA) || (codepoint == 0x2017) || (codepoint == 0x203E))

#నిర్వచించు isCompoundS3(codepoint) \
        ((codepoint == 0x060C) || (codepoint == 0x061B) || (codepoint == 0x061F) || (codepoint >= 0x0621 && codepoint <= 0x063A) || \
         (codepoint >= 0x0640 && codepoint <= 0x0652) || (codepoint >= 0x0660 && codepoint <= 0x066D) || (codepoint == 0x200B) || \
         (codepoint >= 0x0FE70 && codepoint <= 0x0FE72) || (codepoint == 0x0FE74) || (codepoint >= 0x0FE76 && codepoint <= 0x0FEBE))

#నిర్వచించు isCompoundS2(codepoint) \
        ((codepoint == 0x02BC) || (codepoint == 0x02BD) || (codepoint >= 0x0384 && codepoint <= 0x03CE) || (codepoint == 0x2015))

#నిర్వచించు isIBM914(codepoint) \
        ((codepoint == 0x0100) || (codepoint == 0x0101) || (codepoint == 0x0112) || (codepoint == 0x0113) || (codepoint == 0x0116) || (codepoint == 0x0117) || \
         (codepoint == 0x0122) || (codepoint == 0x0123) || (codepoint >= 0x0128 && codepoint <= 0x012B) || (codepoint == 0x012E) || (codepoint == 0x012F) || \
         (codepoint >= 0x0136 && codepoint <= 0x0138) || (codepoint == 0x013B) || (codepoint == 0x013C) || (codepoint == 0x0145) || (codepoint ==  0x0146) || \
         (codepoint >= 0x014A && codepoint <= 0x014D) || (codepoint == 0x0156) || (codepoint == 0x0157) || (codepoint >= 0x0166 && codepoint <= 0x016B) || \
         (codepoint == 0x0172) || (codepoint == 0x0173))

#నిర్వచించు isIBM874(codepoint) \
        ((codepoint >= 0x0E01 && codepoint <= 0x0E3A) || (codepoint >= 0x0E3F && codepoint <= 0x0E5B))

#నిర్వచించు isIBM912(codepoint) \
        ((codepoint >= 0x0102 && codepoint <= 0x0107) || (codepoint >= 0x010C && codepoint <= 0x0111) || (codepoint >= 0x0118 && codepoint <= 0x011B) || \
         (codepoint == 0x0139) || (codepoint == 0x013A) || (codepoint == 0x013D) || (codepoint == 0x013E) || (codepoint >= 0x0141 && codepoint <= 0x0144) || \
         (codepoint == 0x0147) || (codepoint == 0x0147) || (codepoint == 0x0150) || (codepoint == 0x0151) || (codepoint == 0x0154) || (codepoint == 0x0155) || \
         (codepoint >= 0x0158 && codepoint <= 0x015B) || (codepoint == 0x015E) || (codepoint == 0x015F) || (codepoint >= 0x0160 && codepoint <= 0x0165) || \
         (codepoint == 0x016E) || (codepoint == 0x016F) || (codepoint == 0x0170) || (codepoint ==  0x0171) || (codepoint >= 0x0179 && codepoint <= 0x017E) || \
         (codepoint == 0x02C7) || (codepoint == 0x02D8) || (codepoint == 0x02D9) || (codepoint == 0x02DB) || (codepoint == 0x02DD))

#నిర్వచించు isIBM913(codepoint) \
        ((codepoint >= 0x0108 && codepoint <= 0x010B) || (codepoint == 0x011C) || \
         (codepoint == 0x011D) || (codepoint == 0x0120) || (codepoint == 0x0121) || \
         (codepoint >= 0x0124 && codepoint <= 0x0127) || (codepoint == 0x0134) || (codepoint == 0x0135) || \
         (codepoint == 0x015C) || (codepoint == 0x015D) || (codepoint == 0x016C) || (codepoint ==  0x016D))

#నిర్వచించు isCompoundS1(codepoint) \
        ((codepoint == 0x011E) || (codepoint == 0x011F) || (codepoint == 0x0130) || \
         (codepoint == 0x0131) || (codepoint >= 0x0218 && codepoint <= 0x021B))

#నిర్వచించు isISO8859_14(codepoint) \
        ((codepoint >= 0x0174 && codepoint <= 0x0177) || (codepoint == 0x1E0A) || \
         (codepoint == 0x1E0B) || (codepoint == 0x1E1E) || (codepoint == 0x1E1F) || \
         (codepoint == 0x1E40) || (codepoint == 0x1E41) || (codepoint == 0x1E56) || \
         (codepoint == 0x1E57) || (codepoint == 0x1E60) || (codepoint == 0x1E61) || \
         (codepoint == 0x1E6A) || (codepoint == 0x1E6B) || (codepoint == 0x1EF2) || \
         (codepoint == 0x1EF3) || (codepoint >= 0x1E80 && codepoint <= 0x1E85))

#నిర్వచించు isIBM923(codepoint) \
        ((codepoint == 0x0152) || (codepoint == 0x0153) || (codepoint == 0x0178) || (codepoint == 0x20AC))


రకనిర్వచనం నిర్మాణం{
    UConverter_Shared_దత్తాంశం *myConverterArray[అంకె_యొక్క_CONVERTERS];
    COMPOUND_పాఠ్యం_CONVERTERS state;
} UConverter_దత్తాంశం_Compound_పాఠ్యం;

/*********** Compound Text Converter Protos ***********/
U_CDECL_BEGIN
స్థిర శూన్య U_CALLCONV
Compound_పాఠ్యం_Open(UConverter *cnv, UConverter_ఎక్కించు_వాదనలు *p_వాదనలు, UError_సంకేతం *error_సంకేతం);

స్థిర శూన్య U_CALLCONV
 Compound_పాఠ్యం_Close(UConverter *converter);

స్థిర శూన్య U_CALLCONV
Compound_పాఠ్యం_Reset(UConverter *converter, UConverterResetChoice choice);

స్థిర స్థిరరాశి అక్షరం* U_CALLCONV
_CompoundTextgetName(స్థిరరాశి UConverter* cnv);


స్థిర int32_t కనుగొను_తదుపరి_Esc(స్థిరరాశి అక్షరం *ఆధారం, స్థిరరాశి అక్షరం *ఆధారం_హద్దు) {
    int32_t length = static_cast<int32_t>(ఆధారం_హద్దు - ఆధారం);
    int32_t i;
    కొరకు (i = 1; i < length; i++) {
        ఒకవేళ (*(ఆధారం + i) == 0x1B) {
            తిరిగి i;
        }
    }

    తిరిగి length;
}

స్థిర COMPOUND_పాఠ్యం_CONVERTERS getState(పూర్ణాంకం codepoint) {
    COMPOUND_పాఠ్యం_CONVERTERS state = DO_శోధన;

    ఒకవేళ (isASCIIRange(codepoint)) {
        state = COMPOUND_పాఠ్యం_SINGLE_0;
    } లేకపోతే ఒకవేళ (isIBM912(codepoint)) {
        state = IBM_912;
    }లేకపోతే ఒకవేళ (isIBM913(codepoint)) {
        state = IBM_913;
    } లేకపోతే ఒకవేళ (isISO8859_14(codepoint)) {
        state = ISO_8859_14;
    } లేకపోతే ఒకవేళ (isIBM923(codepoint)) {
        state = IBM_923;
    } లేకపోతే ఒకవేళ (isIBM874(codepoint)) {
        state = IBM_874;
    } లేకపోతే ఒకవేళ (isIBM914(codepoint)) {
        state = IBM_914;
    } లేకపోతే ఒకవేళ (isCompoundS2(codepoint)) {
        state = COMPOUND_పాఠ్యం_SINGLE_2;
    } లేకపోతే ఒకవేళ (isCompoundS3(codepoint)) {
        state = COMPOUND_పాఠ్యం_SINGLE_3;
    } లేకపోతే ఒకవేళ (isIBM916(codepoint)) {
        state = IBM_916;
    } లేకపోతే ఒకవేళ (isIBM915(codepoint)) {
        state = IBM_915;
    } లేకపోతే ఒకవేళ (isCompoundS1(codepoint)) {
        state = COMPOUND_పాఠ్యం_SINGLE_1;
    }

    తిరిగి state;
}

స్థిర COMPOUND_పాఠ్యం_CONVERTERS కనుగొను_State_From_Esc_Seq(స్థిరరాశి అక్షరం* ఆధారం, స్థిరరాశి అక్షరం* ఆధారం_హద్దు, స్థిరరాశి uint8_t* గమ్యం_UBytes_బఫరు, int32_t గమ్యం_UBytes_బఫరు_నిడివి, UError_సంకేతం *err) {
    COMPOUND_పాఠ్యం_CONVERTERS state = INVALID;
    UBool సరిపోలిక_దొరికింది = అబద్ధం;
    int32_t i, n, స్థానభ్రంశం = గమ్యం_UBytes_బఫరు_నిడివి;

    కొరకు (i = 0; i < అంకె_యొక్క_CONVERTERS; i++) {
        సరిపోలిక_దొరికింది = నిజం;
        కొరకు (n = 0; esc_Seq_Compound_పాఠ్యం[i][n] != 0; n++) {
            ఒకవేళ (n < గమ్యం_UBytes_బఫరు_నిడివి) {
                ఒకవేళ (గమ్యం_UBytes_బఫరు[n] != esc_Seq_Compound_పాఠ్యం[i][n]) {
                    సరిపోలిక_దొరికింది = అబద్ధం;
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ ((ఆధారం + (n - స్థానభ్రంశం)) >= ఆధారం_హద్దు) {
                *err = U_TRUNCATED_CHAR_దొరికింది;
                సరిపోలిక_దొరికింది = అబద్ధం;
                ఆపు;
            } లేకపోతే ఒకవేళ (*(ఆధారం + (n - స్థానభ్రంశం)) != esc_Seq_Compound_పాఠ్యం[i][n]) {
                సరిపోలిక_దొరికింది = అబద్ధం;
                ఆపు;
            }
        }

        ఒకవేళ (సరిపోలిక_దొరికింది) {
            ఆపు;
        }
    }

    ఒకవేళ (సరిపోలిక_దొరికింది) {
        state = (COMPOUND_పాఠ్యం_CONVERTERS)i;
    }

    తిరిగి state;
}

స్థిర శూన్య U_CALLCONV
Compound_పాఠ్యం_Open(UConverter *cnv, UConverter_ఎక్కించు_వాదనలు *p_వాదనలు, UError_సంకేతం *error_సంకేతం){
    cnv->extraInfo = uprv_malloc (పరిమాణం (UConverter_దత్తాంశం_Compound_పాఠ్యం));
    ఒకవేళ (cnv->extraInfo != శూన్యసూచిక) {
        UConverter_దత్తాంశం_Compound_పాఠ్యం *my_Converter_దత్తాంశం = (UConverter_దత్తాంశం_Compound_పాఠ్యం *) cnv->extraInfo;

        UConverterNamePieces దొంతర_Pieces;
        UConverter_ఎక్కించు_వాదనలు దొంతర_వాదనలు=UCNV_ఎక్కించు_వాదనలు_INITIALIZER;

        my_Converter_దత్తాంశం->myConverterArray[COMPOUND_పాఠ్యం_SINGLE_0] = శూన్యసూచిక;
        my_Converter_దత్తాంశం->myConverterArray[COMPOUND_పాఠ్యం_SINGLE_1] = ucnv_ఎక్కించు_Shared_దత్తాంశం("icu-internal-compound-s1", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[COMPOUND_పాఠ్యం_SINGLE_2] = ucnv_ఎక్కించు_Shared_దత్తాంశం("icu-internal-compound-s2", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[COMPOUND_పాఠ్యం_SINGLE_3] = ucnv_ఎక్కించు_Shared_దత్తాంశం("icu-internal-compound-s3", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[COMPOUND_పాఠ్యం_DOUBLE_1] = ucnv_ఎక్కించు_Shared_దత్తాంశం("icu-internal-compound-d1", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[COMPOUND_పాఠ్యం_DOUBLE_2] = ucnv_ఎక్కించు_Shared_దత్తాంశం("icu-internal-compound-d2", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[COMPOUND_పాఠ్యం_DOUBLE_3] = ucnv_ఎక్కించు_Shared_దత్తాంశం("icu-internal-compound-d3", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[COMPOUND_పాఠ్యం_DOUBLE_4] = ucnv_ఎక్కించు_Shared_దత్తాంశం("icu-internal-compound-d4", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[COMPOUND_పాఠ్యం_DOUBLE_5] = ucnv_ఎక్కించు_Shared_దత్తాంశం("icu-internal-compound-d5", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[COMPOUND_పాఠ్యం_DOUBLE_6] = ucnv_ఎక్కించు_Shared_దత్తాంశం("icu-internal-compound-d6", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[COMPOUND_పాఠ్యం_DOUBLE_7] = ucnv_ఎక్కించు_Shared_దత్తాంశం("icu-internal-compound-d7", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[COMPOUND_పాఠ్యం_TRIPLE_DOUBLE] = ucnv_ఎక్కించు_Shared_దత్తాంశం("icu-internal-compound-t", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);

        my_Converter_దత్తాంశం->myConverterArray[IBM_915] = ucnv_ఎక్కించు_Shared_దత్తాంశం("ibm-915_P100-1995", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[IBM_916] = ucnv_ఎక్కించు_Shared_దత్తాంశం("ibm-916_P100-1995", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[IBM_914] = ucnv_ఎక్కించు_Shared_దత్తాంశం("ibm-914_P100-1995", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[IBM_874] = ucnv_ఎక్కించు_Shared_దత్తాంశం("ibm-874_P100-1995", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[IBM_912] = ucnv_ఎక్కించు_Shared_దత్తాంశం("ibm-912_P100-1995", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[IBM_913] = ucnv_ఎక్కించు_Shared_దత్తాంశం("ibm-913_P100-2000", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[ISO_8859_14] = ucnv_ఎక్కించు_Shared_దత్తాంశం("iso-8859_14-1998", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
        my_Converter_దత్తాంశం->myConverterArray[IBM_923] = ucnv_ఎక్కించు_Shared_దత్తాంశం("ibm-923_P100-1998", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);

        ఒకవేళ (U_FAILURE(*error_సంకేతం) || p_వాదనలు->onlyTestIsLoadable) {
            Compound_పాఠ్యం_Close(cnv);
            తిరిగి;
        }

        my_Converter_దత్తాంశం->state = (COMPOUND_పాఠ్యం_CONVERTERS)0;
    } లేకపోతే {
        *error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
}


స్థిర శూన్య U_CALLCONV
Compound_పాఠ్యం_Close(UConverter *converter) {
    UConverter_దత్తాంశం_Compound_పాఠ్యం* my_Converter_దత్తాంశం = (UConverter_దత్తాంశం_Compound_పాఠ్యం*)(converter->extraInfo);
    int32_t i;

    ఒకవేళ (converter->extraInfo != శూన్యసూచిక) {
        /*close the array of converter pointers and free the memory*/
        కొరకు (i = 0; i < అంకె_యొక్క_CONVERTERS; i++) {
            ఒకవేళ (my_Converter_దత్తాంశం->myConverterArray[i] != శూన్యసూచిక) {
                ucnv_unload_Shared_దత్తాంశం_If_Ready(my_Converter_దత్తాంశం->myConverterArray[i]);
            }
        }

        uprv_free(converter->extraInfo);
        converter->extraInfo = శూన్యసూచిక;
    }
}

స్థిర శూన్య U_CALLCONV
Compound_పాఠ్యం_Reset(UConverter *converter, UConverterResetChoice choice) {
    (శూన్య)converter;
    (శూన్య)choice;
}

స్థిర స్థిరరాశి అక్షరం* U_CALLCONV
_CompoundTextgetName(స్థిరరాశి UConverter* cnv){
    (శూన్య)cnv;
    తిరిగి "x11-compound-text";
}

స్థిర శూన్య U_CALLCONV
UConverter_from_Unicode_Compound_పాఠ్యం_OFFSETS(UConverter_From_Unicode_వాదనలు* వాదనలు_2, UError_సంకేతం* err){
    UConverter *cnv = వాదనలు_2->converter;
    uint8_t *లక్ష్యం = (uint8_t *) వాదనలు_2->లక్ష్యం;
    స్థిరరాశి uint8_t *లక్ష్యం_హద్దు = (స్థిరరాశి uint8_t *) వాదనలు_2->లక్ష్యం_హద్దు;
    స్థిరరాశి char16_t* ఆధారం = వాదనలు_2->ఆధారం;
    స్థిరరాశి char16_t* ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    /* int32_t* offsets = args->offsets; */
    UChar32 ఆధారం_Char;
    UBool useFallback = cnv->useFallback;
    uint8_t tmp_లక్ష్యం_బఫరు[7];
    int32_t tmp_లక్ష్యం_బఫరు_నిడివి = 0;
    COMPOUND_పాఠ్యం_CONVERTERS currentState, tmpState;
    uint32_t p_విలువ;
    int32_t p_విలువ_నిడివి = 0;
    int32_t i, n, j;

    UConverter_దత్తాంశం_Compound_పాఠ్యం *my_Converter_దత్తాంశం = (UConverter_దత్తాంశం_Compound_పాఠ్యం *) cnv->extraInfo;

    currentState = my_Converter_దత్తాంశం->state;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    ఒకవేళ((ఆధారం_Char = cnv->fromUChar32)!=0 && లక్ష్యం< లక్ష్యం_హద్దు) {
        వెళ్ళు getTrail;
    }

    అయితే( ఆధారం < ఆధారం_హద్దు){
        ఒకవేళ(లక్ష్యం < లక్ష్యం_హద్దు){

            ఆధారం_Char  = *(ఆధారం++);
            /*check if the char is a First surrogate*/
             ఒకవేళ(U16_IS_SURROGATE(ఆధారం_Char)) {
                ఒకవేళ(U16_IS_SURROGATE_LEAD(ఆధారం_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    ఒకవేళ(ఆధారం < ఆధారం_హద్దు) {
                        /* test the following code unit */
                        char16_t trail = *ఆధారం;
                        ఒకవేళ(U16_IS_TRAIL(trail)) {
                            ఆధారం++;
                            ఆధారం_Char=U16_GET_SUPPLEMENTARY(ఆధారం_Char, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } లేకపోతే {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_దొరికింది;
                            cnv->fromUChar32=ఆధారం_Char;
                            ఆపు;
                        }
                    } లేకపోతే {
                        /* no more input */
                        cnv->fromUChar32=ఆధారం_Char;
                        ఆపు;
                    }
                } లేకపోతే {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_దొరికింది;
                    cnv->fromUChar32=ఆధారం_Char;
                    ఆపు;
                }
            }

             tmp_లక్ష్యం_బఫరు_నిడివి = 0;
             tmpState = getState(ఆధారం_Char);

             ఒకవేళ (tmpState != DO_శోధన && currentState != tmpState) {
                 /* Get escape sequence if necessary */
                 currentState = tmpState;
                 కొరకు (i = 0; esc_Seq_Compound_పాఠ్యం[currentState][i] != 0; i++) {
                     tmp_లక్ష్యం_బఫరు[tmp_లక్ష్యం_బఫరు_నిడివి++] = esc_Seq_Compound_పాఠ్యం[currentState][i];
                 }
             }

             ఒకవేళ (tmpState == DO_శోధన) {
                 /* Test all available converters */
                 కొరకు (i = 1; i < శోధన_నిడివి; i++) {
                     p_విలువ_నిడివి = ucnv_MBCSFromUChar32(my_Converter_దత్తాంశం->myConverterArray[i], ఆధారం_Char, &p_విలువ, useFallback);
                     ఒకవేళ (p_విలువ_నిడివి > 0) {
                         tmpState = (COMPOUND_పాఠ్యం_CONVERTERS)i;
                         ఒకవేళ (currentState != tmpState) {
                             currentState = tmpState;
                             కొరకు (j = 0; esc_Seq_Compound_పాఠ్యం[currentState][j] != 0; j++) {
                                 tmp_లక్ష్యం_బఫరు[tmp_లక్ష్యం_బఫరు_నిడివి++] = esc_Seq_Compound_పాఠ్యం[currentState][j];
                             }
                         }
                         కొరకు (n = (p_విలువ_నిడివి - 1); n >= 0; n--) {
                             tmp_లక్ష్యం_బఫరు[tmp_లక్ష్యం_బఫరు_నిడివి++] = (uint8_t)(p_విలువ >> (n * 8));
                         }
                         ఆపు;
                     }
                 }
             } లేకపోతే ఒకవేళ (tmpState == COMPOUND_పాఠ్యం_SINGLE_0) {
                 tmp_లక్ష్యం_బఫరు[tmp_లక్ష్యం_బఫరు_నిడివి++] = (uint8_t)ఆధారం_Char;
             } లేకపోతే {
                 p_విలువ_నిడివి = ucnv_MBCSFromUChar32(my_Converter_దత్తాంశం->myConverterArray[currentState], ఆధారం_Char, &p_విలువ, useFallback);
                 ఒకవేళ (p_విలువ_నిడివి > 0) {
                     కొరకు (n = (p_విలువ_నిడివి - 1); n >= 0; n--) {
                         tmp_లక్ష్యం_బఫరు[tmp_లక్ష్యం_బఫరు_నిడివి++] = (uint8_t)(p_విలువ >> (n * 8));
                     }
                 }
             }

             కొరకు (i = 0; i < tmp_లక్ష్యం_బఫరు_నిడివి; i++) {
                 ఒకవేళ (లక్ష్యం < లక్ష్యం_హద్దు) {
                     *లక్ష్యం++ = tmp_లక్ష్యం_బఫరు[i];
                 } లేకపోతే {
                     *err = U_బఫరు_అధికప్రవాహం_ERROR;
                     ఆపు;
                 }
             }

             ఒకవేళ (*err == U_బఫరు_అధికప్రవాహం_ERROR) {
                 కొరకు (; i < tmp_లక్ష్యం_బఫరు_నిడివి; i++) {
                     వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] = tmp_లక్ష్యం_బఫరు[i];
                 }
             }
        } లేకపోతే {
            *err = U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }
    }

    /*save the state and return */
    my_Converter_దత్తాంశం->state = currentState;
    వాదనలు_2->ఆధారం = ఆధారం;
    వాదనలు_2->లక్ష్యం = (అక్షరం*)లక్ష్యం;
}


స్థిర శూన్య U_CALLCONV
UConverter_గమ్యం_Unicode_Compound_పాఠ్యం_OFFSETS(UConverter_గమ్యం_Unicode_వాదనలు *వాదనలు_2,
                                               UError_సంకేతం* err){
    స్థిరరాశి అక్షరం *my_ఆధారం = (అక్షరం *) వాదనలు_2->ఆధారం;
    char16_t *my_లక్ష్యం = వాదనలు_2->లక్ష్యం;
    స్థిరరాశి అక్షరం *my_ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    స్థిరరాశి అక్షరం *tmp_ఆధారం_హద్దు = my_ఆధారం_హద్దు;
    uint32_t my_ఆధారం_Char = 0x0000;
    COMPOUND_పాఠ్యం_CONVERTERS currentState, tmpState;
    int32_t ఆధారం_స్థానభ్రంశం = 0;
    UConverter_దత్తాంశం_Compound_పాఠ్యం *my_Converter_దత్తాంశం = (UConverter_దత్తాంశం_Compound_పాఠ్యం *) వాదనలు_2->converter->extraInfo;
    UConverter_Shared_దత్తాంశం* saved_Shared_దత్తాంశం = శూన్యసూచిక;

    UConverter_గమ్యం_Unicode_వాదనలు sub_వాదనలు;
    int32_t min_వాదనలు_కొలత;

    /* set up the subconverter arguments */
    ఒకవేళ(వాదనలు_2->size<పరిమాణం(UConverter_గమ్యం_Unicode_వాదనలు)) {
        min_వాదనలు_కొలత = వాదనలు_2->size;
    } లేకపోతే {
        min_వాదనలు_కొలత = (int32_t)పరిమాణం(UConverter_గమ్యం_Unicode_వాదనలు);
    }

    uprv_memcpy(&sub_వాదనలు, వాదనలు_2, min_వాదనలు_కొలత);
    sub_వాదనలు.size = (uint16_t)min_వాదనలు_కొలత;

    currentState = tmpState =  my_Converter_దత్తాంశం->state;

    అయితే(my_ఆధారం < my_ఆధారం_హద్దు){
        ఒకవేళ(my_లక్ష్యం < వాదనలు_2->లక్ష్యం_హద్దు){
            ఒకవేళ (వాదనలు_2->converter->గమ్యం_ULength > 0) {
                my_ఆధారం_Char = వాదనలు_2->converter->గమ్యం_UBytes[0];
            } లేకపోతే {
                my_ఆధారం_Char = (uint8_t)*my_ఆధారం;
            }

            ఒకవేళ (my_ఆధారం_Char == ESC_ప్రారంభం) {
                tmpState = కనుగొను_State_From_Esc_Seq(my_ఆధారం, my_ఆధారం_హద్దు, వాదనలు_2->converter->గమ్యం_UBytes, వాదనలు_2->converter->గమ్యం_ULength, err);

                ఒకవేళ (*err == U_TRUNCATED_CHAR_దొరికింది) {
                    కొరకు (; my_ఆధారం < my_ఆధారం_హద్దు;) {
                        వాదనలు_2->converter->గమ్యం_UBytes[వాదనలు_2->converter->గమ్యం_ULength++] = *my_ఆధారం++;
                    }
                    *err = U_ZERO_ERROR;
                    ఆపు;
                } లేకపోతే ఒకవేళ (tmpState == INVALID) {
                    ఒకవేళ (వాదనలు_2->converter->గమ్యం_ULength == 0) {
                        my_ఆధారం++; /* skip over the 0x1b byte */
                    }
                    *err = U_ILLEGAL_CHAR_దొరికింది;
                    ఆపు;
                }

                ఒకవేళ (tmpState != currentState) {
                    currentState = tmpState;
                }

                ఆధారం_స్థానభ్రంశం = static_cast<int32_t>(uprv_strlen((అక్షరం*)esc_Seq_Compound_పాఠ్యం[currentState]) - వాదనలు_2->converter->గమ్యం_ULength);

                my_ఆధారం += ఆధారం_స్థానభ్రంశం;

                వాదనలు_2->converter->గమ్యం_ULength = 0;
            }

            ఒకవేళ (currentState == COMPOUND_పాఠ్యం_SINGLE_0) {
                అయితే (my_ఆధారం < my_ఆధారం_హద్దు) {
                    ఒకవేళ (*my_ఆధారం == ESC_ప్రారంభం) {
                        ఆపు;
                    }
                    ఒకవేళ (my_లక్ష్యం < వాదనలు_2->లక్ష్యం_హద్దు) {
                        *my_లక్ష్యం++ = 0x00ff&(*my_ఆధారం++);
                    } లేకపోతే {
                        *err = U_బఫరు_అధికప్రవాహం_ERROR;
                        ఆపు;
                    }
                }
            } లేకపోతే ఒకవేళ (my_ఆధారం < my_ఆధారం_హద్దు){
                ఆధారం_స్థానభ్రంశం = కనుగొను_తదుపరి_Esc(my_ఆధారం, my_ఆధారం_హద్దు);

                tmp_ఆధారం_హద్దు = my_ఆధారం + ఆధారం_స్థానభ్రంశం;

                sub_వాదనలు.ఆధారం = my_ఆధారం;
                sub_వాదనలు.ఆధారం_హద్దు = tmp_ఆధారం_హద్దు;
                sub_వాదనలు.లక్ష్యం = my_లక్ష్యం;
                saved_Shared_దత్తాంశం = sub_వాదనలు.converter->shared_దత్తాంశం;
                sub_వాదనలు.converter->shared_దత్తాంశం = my_Converter_దత్తాంశం->myConverterArray[currentState];

                ucnv_MBCSToUnicodeWithOffsets(&sub_వాదనలు, err);

                sub_వాదనలు.converter->shared_దత్తాంశం = saved_Shared_దత్తాంశం;

                my_ఆధారం = sub_వాదనలు.ఆధారం;
                my_లక్ష్యం = sub_వాదనలు.లక్ష్యం;

                ఒకవేళ (U_FAILURE(*err)) {
                    ఒకవేళ(*err == U_బఫరు_అధికప్రవాహం_ERROR) {
                        ఒకవేళ(sub_వాదనలు.converter->UChar_Error_బఫరు_నిడివి > 0) {
                            uprv_memcpy(వాదనలు_2->converter->UChar_Error_బఫరు, sub_వాదనలు.converter->UChar_Error_బఫరు,
                                        sub_వాదనలు.converter->UChar_Error_బఫరు_నిడివి);
                        }
                        వాదనలు_2->converter->UChar_Error_బఫరు_నిడివి=sub_వాదనలు.converter->UChar_Error_బఫరు_నిడివి;
                        sub_వాదనలు.converter->UChar_Error_బఫరు_నిడివి = 0;
                    }
                    ఆపు;
                }
            }
        } లేకపోతే {
            *err = U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }
    }
    my_Converter_దత్తాంశం->state = currentState;
    వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
    వాదనలు_2->ఆధారం = my_ఆధారం;
}

స్థిర శూన్య U_CALLCONV
Compound_పాఠ్యం_Get_Unicode_సమితి(స్థిరరాశి UConverter *cnv,
                    స్థిరరాశి USetAdder *sa,
                    UConverter_Unicode_సమితి which,
                    UError_సంకేతం *p_Error_సంకేతం) {
    UConverter_దత్తాంశం_Compound_పాఠ్యం *my_Converter_దత్తాంశం = (UConverter_దత్తాంశం_Compound_పాఠ్యం *)cnv->extraInfo;
    int32_t i;

    కొరకు (i = 1; i < అంకె_యొక్క_CONVERTERS; i++) {
        ucnv_MBCSGet_Unicode_సమితి_For_Unicode(my_Converter_దత్తాంశం->myConverterArray[i], sa, which, p_Error_సంకేతం);
    }
    sa->కూడు(sa->సమితి, 0x0000);
    sa->కూడు(sa->సమితి, 0x0009);
    sa->కూడు(sa->సమితి, 0x000A);
    sa->కూడు_Range(sa->సమితి, 0x0020, 0x007F);
    sa->కూడు_Range(sa->సమితి, 0x00A0, 0x00FF);
}
U_CDECL_చివర

స్థిర స్థిరరాశి UConverterImpl Compound_పాఠ్యం_Impl = {

    UCNV_COMPOUND_పాఠ్యం,

    శూన్యసూచిక,
    శూన్యసూచిక,

    Compound_పాఠ్యం_Open,
    Compound_పాఠ్యం_Close,
    Compound_పాఠ్యం_Reset,

    UConverter_గమ్యం_Unicode_Compound_పాఠ్యం_OFFSETS,
    UConverter_గమ్యం_Unicode_Compound_పాఠ్యం_OFFSETS,
    UConverter_from_Unicode_Compound_పాఠ్యం_OFFSETS,
    UConverter_from_Unicode_Compound_పాఠ్యం_OFFSETS,
    శూన్యసూచిక,

    శూన్యసూచిక,
    _CompoundTextgetName,
    శూన్యసూచిక,
    శూన్యసూచిక,
    Compound_పాఠ్యం_Get_Unicode_సమితి,
    శూన్యసూచిక,
    శూన్యసూచిక
};

స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం Compound_పాఠ్యం_Static_దత్తాంశం = {
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "COMPOUND_TEXT",
    0,
    UCNV_IBM,
    UCNV_COMPOUND_పాఠ్యం,
    1,
    6,
    { 0xef, 0, 0, 0 },
    1,
    అబద్ధం,
    అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};
స్థిరరాశి UConverter_Shared_దత్తాంశం Compound_పాఠ్యం_దత్తాంశం =
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&Compound_పాఠ్యం_Static_దత్తాంశం, &Compound_పాఠ్యం_Impl);

#అయితే_ముగింపు /* #if !UCONFIG_NO_LEGACY_CONVERSION */
