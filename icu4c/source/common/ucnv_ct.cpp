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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#समावेश "unicode/ucnv.h"
#समावेश "unicode/uset.h"
#समावेश "unicode/ucnv_err.h"
#समावेश "unicode/ucnv_cb.h"
#समावेश "unicode/utf16.h"
#समावेश "ucnv_imp.h"
#समावेश "ucnv_bld.h"
#समावेश "ucnv_cnv.h"
#समावेश "ucnvmbcs.h"
#समावेश "cstring.h"
#समावेश "cmemory.h"

प्रकार क्रमागत {
    INVALID = -2,
    DO_खोज = -1,

    COMPOUND_TEXT_SINGLE_0 = 0,
    COMPOUND_TEXT_SINGLE_1 = 1,
    COMPOUND_TEXT_SINGLE_2 = 2,
    COMPOUND_TEXT_SINGLE_3 = 3,

    COMPOUND_TEXT_DOUBLE_1 = 4,
    COMPOUND_TEXT_DOUBLE_2 = 5,
    COMPOUND_TEXT_DOUBLE_3 = 6,
    COMPOUND_TEXT_DOUBLE_4 = 7,
    COMPOUND_TEXT_DOUBLE_5 = 8,
    COMPOUND_TEXT_DOUBLE_6 = 9,
    COMPOUND_TEXT_DOUBLE_7 = 10,

    COMPOUND_TEXT_TRIPLE_DOUBLE = 11,

    IBM_915 = 12,
    IBM_916 = 13,
    IBM_914 = 14,
    IBM_874 = 15,
    IBM_912 = 16,
    IBM_913 = 17,
    ISO_8859_14 = 18,
    IBM_923 = 19,
    अंक_OF_CONVERTERS = 20
} COMPOUND_TEXT_CONVERTERS;

#घोषणा खोज_लंबाई 12

अटल स्थिर uint8_t escSeqCompoundText[अंक_OF_CONVERTERS][5] = {
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

#घोषणा ESC_START 0x1B

#घोषणा isASCIIRange(codepoint) \
        ((codepoint == 0x0000) || (codepoint == 0x0009) || (codepoint == 0x000A) || \
         (codepoint >= 0x0020 && codepoint <= 0x007f) || (codepoint >= 0x00A0 && codepoint <= 0x00FF))

#घोषणा isIBM915(codepoint) \
        ((codepoint >= 0x0401 && codepoint <= 0x045F) || (codepoint == 0x2116))

#घोषणा isIBM916(codepoint) \
        ((codepoint >= 0x05D0 && codepoint <= 0x05EA) || (codepoint == 0x2017) || (codepoint == 0x203E))

#घोषणा isCompoundS3(codepoint) \
        ((codepoint == 0x060C) || (codepoint == 0x061B) || (codepoint == 0x061F) || (codepoint >= 0x0621 && codepoint <= 0x063A) || \
         (codepoint >= 0x0640 && codepoint <= 0x0652) || (codepoint >= 0x0660 && codepoint <= 0x066D) || (codepoint == 0x200B) || \
         (codepoint >= 0x0FE70 && codepoint <= 0x0FE72) || (codepoint == 0x0FE74) || (codepoint >= 0x0FE76 && codepoint <= 0x0FEBE))

#घोषणा isCompoundS2(codepoint) \
        ((codepoint == 0x02BC) || (codepoint == 0x02BD) || (codepoint >= 0x0384 && codepoint <= 0x03CE) || (codepoint == 0x2015))

#घोषणा isIBM914(codepoint) \
        ((codepoint == 0x0100) || (codepoint == 0x0101) || (codepoint == 0x0112) || (codepoint == 0x0113) || (codepoint == 0x0116) || (codepoint == 0x0117) || \
         (codepoint == 0x0122) || (codepoint == 0x0123) || (codepoint >= 0x0128 && codepoint <= 0x012B) || (codepoint == 0x012E) || (codepoint == 0x012F) || \
         (codepoint >= 0x0136 && codepoint <= 0x0138) || (codepoint == 0x013B) || (codepoint == 0x013C) || (codepoint == 0x0145) || (codepoint ==  0x0146) || \
         (codepoint >= 0x014A && codepoint <= 0x014D) || (codepoint == 0x0156) || (codepoint == 0x0157) || (codepoint >= 0x0166 && codepoint <= 0x016B) || \
         (codepoint == 0x0172) || (codepoint == 0x0173))

#घोषणा isIBM874(codepoint) \
        ((codepoint >= 0x0E01 && codepoint <= 0x0E3A) || (codepoint >= 0x0E3F && codepoint <= 0x0E5B))

#घोषणा isIBM912(codepoint) \
        ((codepoint >= 0x0102 && codepoint <= 0x0107) || (codepoint >= 0x010C && codepoint <= 0x0111) || (codepoint >= 0x0118 && codepoint <= 0x011B) || \
         (codepoint == 0x0139) || (codepoint == 0x013A) || (codepoint == 0x013D) || (codepoint == 0x013E) || (codepoint >= 0x0141 && codepoint <= 0x0144) || \
         (codepoint == 0x0147) || (codepoint == 0x0147) || (codepoint == 0x0150) || (codepoint == 0x0151) || (codepoint == 0x0154) || (codepoint == 0x0155) || \
         (codepoint >= 0x0158 && codepoint <= 0x015B) || (codepoint == 0x015E) || (codepoint == 0x015F) || (codepoint >= 0x0160 && codepoint <= 0x0165) || \
         (codepoint == 0x016E) || (codepoint == 0x016F) || (codepoint == 0x0170) || (codepoint ==  0x0171) || (codepoint >= 0x0179 && codepoint <= 0x017E) || \
         (codepoint == 0x02C7) || (codepoint == 0x02D8) || (codepoint == 0x02D9) || (codepoint == 0x02DB) || (codepoint == 0x02DD))

#घोषणा isIBM913(codepoint) \
        ((codepoint >= 0x0108 && codepoint <= 0x010B) || (codepoint == 0x011C) || \
         (codepoint == 0x011D) || (codepoint == 0x0120) || (codepoint == 0x0121) || \
         (codepoint >= 0x0124 && codepoint <= 0x0127) || (codepoint == 0x0134) || (codepoint == 0x0135) || \
         (codepoint == 0x015C) || (codepoint == 0x015D) || (codepoint == 0x016C) || (codepoint ==  0x016D))

#घोषणा isCompoundS1(codepoint) \
        ((codepoint == 0x011E) || (codepoint == 0x011F) || (codepoint == 0x0130) || \
         (codepoint == 0x0131) || (codepoint >= 0x0218 && codepoint <= 0x021B))

#घोषणा isISO8859_14(codepoint) \
        ((codepoint >= 0x0174 && codepoint <= 0x0177) || (codepoint == 0x1E0A) || \
         (codepoint == 0x1E0B) || (codepoint == 0x1E1E) || (codepoint == 0x1E1F) || \
         (codepoint == 0x1E40) || (codepoint == 0x1E41) || (codepoint == 0x1E56) || \
         (codepoint == 0x1E57) || (codepoint == 0x1E60) || (codepoint == 0x1E61) || \
         (codepoint == 0x1E6A) || (codepoint == 0x1E6B) || (codepoint == 0x1EF2) || \
         (codepoint == 0x1EF3) || (codepoint >= 0x1E80 && codepoint <= 0x1E85))

#घोषणा isIBM923(codepoint) \
        ((codepoint == 0x0152) || (codepoint == 0x0153) || (codepoint == 0x0178) || (codepoint == 0x20AC))


प्रकार काष्ठा{
    UConverter_Shared_तथ्य *myConverterArray[अंक_OF_CONVERTERS];
    COMPOUND_TEXT_CONVERTERS state;
} UConverter_तथ्य_Compound_Text;

/*********** Compound Text Converter Protos ***********/
U_CDECL_BEGIN
अटल व्योम U_CALLCONV
_CompoundTextOpen(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode);

अटल व्योम U_CALLCONV
 _CompoundTextClose(UConverter *converter);

अटल व्योम U_CALLCONV
_CompoundTextReset(UConverter *converter, UConverterResetChoice choice);

अटल स्थिर अक्षर* U_CALLCONV
_CompoundTextgetName(स्थिर UConverter* cnv);


अटल int32_t find_अग्र_Esc(स्थिर अक्षर *source, स्थिर अक्षर *sourceLimit) {
    int32_t length = static_cast<int32_t>(sourceLimit - source);
    int32_t i;
    क्रम (i = 1; i < length; i++) {
        अगर (*(source + i) == 0x1B) {
            वापस i;
        }
    }

    वापस length;
}

अटल COMPOUND_TEXT_CONVERTERS getState(पूर्णांक codepoint) {
    COMPOUND_TEXT_CONVERTERS state = DO_खोज;

    अगर (isASCIIRange(codepoint)) {
        state = COMPOUND_TEXT_SINGLE_0;
    } अन्यथा अगर (isIBM912(codepoint)) {
        state = IBM_912;
    }अन्यथा अगर (isIBM913(codepoint)) {
        state = IBM_913;
    } अन्यथा अगर (isISO8859_14(codepoint)) {
        state = ISO_8859_14;
    } अन्यथा अगर (isIBM923(codepoint)) {
        state = IBM_923;
    } अन्यथा अगर (isIBM874(codepoint)) {
        state = IBM_874;
    } अन्यथा अगर (isIBM914(codepoint)) {
        state = IBM_914;
    } अन्यथा अगर (isCompoundS2(codepoint)) {
        state = COMPOUND_TEXT_SINGLE_2;
    } अन्यथा अगर (isCompoundS3(codepoint)) {
        state = COMPOUND_TEXT_SINGLE_3;
    } अन्यथा अगर (isIBM916(codepoint)) {
        state = IBM_916;
    } अन्यथा अगर (isIBM915(codepoint)) {
        state = IBM_915;
    } अन्यथा अगर (isCompoundS1(codepoint)) {
        state = COMPOUND_TEXT_SINGLE_1;
    }

    वापस state;
}

अटल COMPOUND_TEXT_CONVERTERS findStateFromEscSeq(स्थिर अक्षर* source, स्थिर अक्षर* sourceLimit, स्थिर uint8_t* to_UBytes_बफर, int32_t to_UBytes_बफर_लंबाई, UErrorCode *err) {
    COMPOUND_TEXT_CONVERTERS state = INVALID;
    UBool matchFound = false;
    int32_t i, n, offset = to_UBytes_बफर_लंबाई;

    क्रम (i = 0; i < अंक_OF_CONVERTERS; i++) {
        matchFound = true;
        क्रम (n = 0; escSeqCompoundText[i][n] != 0; n++) {
            अगर (n < to_UBytes_बफर_लंबाई) {
                अगर (to_UBytes_बफर[n] != escSeqCompoundText[i][n]) {
                    matchFound = false;
                    अवरोध;
                }
            } अन्यथा अगर ((source + (n - offset)) >= sourceLimit) {
                *err = U_TRUNCATED_CHAR_FOUND;
                matchFound = false;
                अवरोध;
            } अन्यथा अगर (*(source + (n - offset)) != escSeqCompoundText[i][n]) {
                matchFound = false;
                अवरोध;
            }
        }

        अगर (matchFound) {
            अवरोध;
        }
    }

    अगर (matchFound) {
        state = (COMPOUND_TEXT_CONVERTERS)i;
    }

    वापस state;
}

अटल व्योम U_CALLCONV
_CompoundTextOpen(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode){
    cnv->extraInfo = uprv_malloc (माप (UConverter_तथ्य_Compound_Text));
    अगर (cnv->extraInfo != nullptr) {
        UConverter_तथ्य_Compound_Text *my_Converter_तथ्य = (UConverter_तथ्य_Compound_Text *) cnv->extraInfo;

        UConverterNamePieces ढेर_Pieces;
        UConverterLoadArgs ढेर_Args=UCNV_LOAD_ARGS_INITIALIZER;

        my_Converter_तथ्य->myConverterArray[COMPOUND_TEXT_SINGLE_0] = nullptr;
        my_Converter_तथ्य->myConverterArray[COMPOUND_TEXT_SINGLE_1] = ucnv_load_Shared_तथ्य("icu-internal-compound-s1", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[COMPOUND_TEXT_SINGLE_2] = ucnv_load_Shared_तथ्य("icu-internal-compound-s2", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[COMPOUND_TEXT_SINGLE_3] = ucnv_load_Shared_तथ्य("icu-internal-compound-s3", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[COMPOUND_TEXT_DOUBLE_1] = ucnv_load_Shared_तथ्य("icu-internal-compound-d1", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[COMPOUND_TEXT_DOUBLE_2] = ucnv_load_Shared_तथ्य("icu-internal-compound-d2", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[COMPOUND_TEXT_DOUBLE_3] = ucnv_load_Shared_तथ्य("icu-internal-compound-d3", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[COMPOUND_TEXT_DOUBLE_4] = ucnv_load_Shared_तथ्य("icu-internal-compound-d4", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[COMPOUND_TEXT_DOUBLE_5] = ucnv_load_Shared_तथ्य("icu-internal-compound-d5", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[COMPOUND_TEXT_DOUBLE_6] = ucnv_load_Shared_तथ्य("icu-internal-compound-d6", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[COMPOUND_TEXT_DOUBLE_7] = ucnv_load_Shared_तथ्य("icu-internal-compound-d7", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[COMPOUND_TEXT_TRIPLE_DOUBLE] = ucnv_load_Shared_तथ्य("icu-internal-compound-t", &ढेर_Pieces, &ढेर_Args, errorCode);

        my_Converter_तथ्य->myConverterArray[IBM_915] = ucnv_load_Shared_तथ्य("ibm-915_P100-1995", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[IBM_916] = ucnv_load_Shared_तथ्य("ibm-916_P100-1995", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[IBM_914] = ucnv_load_Shared_तथ्य("ibm-914_P100-1995", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[IBM_874] = ucnv_load_Shared_तथ्य("ibm-874_P100-1995", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[IBM_912] = ucnv_load_Shared_तथ्य("ibm-912_P100-1995", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[IBM_913] = ucnv_load_Shared_तथ्य("ibm-913_P100-2000", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[ISO_8859_14] = ucnv_load_Shared_तथ्य("iso-8859_14-1998", &ढेर_Pieces, &ढेर_Args, errorCode);
        my_Converter_तथ्य->myConverterArray[IBM_923] = ucnv_load_Shared_तथ्य("ibm-923_P100-1998", &ढेर_Pieces, &ढेर_Args, errorCode);

        अगर (U_FAILURE(*errorCode) || pArgs->onlyTestIsLoadable) {
            _CompoundTextClose(cnv);
            वापस;
        }

        my_Converter_तथ्य->state = (COMPOUND_TEXT_CONVERTERS)0;
    } अन्यथा {
        *errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}


अटल व्योम U_CALLCONV
_CompoundTextClose(UConverter *converter) {
    UConverter_तथ्य_Compound_Text* my_Converter_तथ्य = (UConverter_तथ्य_Compound_Text*)(converter->extraInfo);
    int32_t i;

    अगर (converter->extraInfo != nullptr) {
        /*close the array of converter pointers and free the memory*/
        क्रम (i = 0; i < अंक_OF_CONVERTERS; i++) {
            अगर (my_Converter_तथ्य->myConverterArray[i] != nullptr) {
                ucnv_unload_Shared_तथ्य_If_Ready(my_Converter_तथ्य->myConverterArray[i]);
            }
        }

        uprv_free(converter->extraInfo);
        converter->extraInfo = nullptr;
    }
}

अटल व्योम U_CALLCONV
_CompoundTextReset(UConverter *converter, UConverterResetChoice choice) {
    (व्योम)converter;
    (व्योम)choice;
}

अटल स्थिर अक्षर* U_CALLCONV
_CompoundTextgetName(स्थिर UConverter* cnv){
    (व्योम)cnv;
    वापस "x11-compound-text";
}

अटल व्योम U_CALLCONV
UConverter_fromUnicode_CompoundText_OFFSETS(UConverterFromUnicodeArgs* args, UErrorCode* err){
    UConverter *cnv = args->converter;
    uint8_t *target = (uint8_t *) args->target;
    स्थिर uint8_t *targetLimit = (स्थिर uint8_t *) args->targetLimit;
    स्थिर char16_t* source = args->source;
    स्थिर char16_t* sourceLimit = args->sourceLimit;
    /* int32_t* offsets = args->offsets; */
    UChar32 sourceChar;
    UBool useFallback = cnv->useFallback;
    uint8_t tmp_Target_बफर[7];
    int32_t tmp_Target_बफर_लंबाई = 0;
    COMPOUND_TEXT_CONVERTERS currentState, tmpState;
    uint32_t p_मूल्य;
    int32_t p_मूल्य_लंबाई = 0;
    int32_t i, n, j;

    UConverter_तथ्य_Compound_Text *my_Converter_तथ्य = (UConverter_तथ्य_Compound_Text *) cnv->extraInfo;

    currentState = my_Converter_तथ्य->state;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    अगर((sourceChar = cnv->fromUChar32)!=0 && target< targetLimit) {
        जाओ getTrail;
    }

    जबतक( source < sourceLimit){
        अगर(target < targetLimit){

            sourceChar  = *(source++);
            /*check if the char is a First surrogate*/
             अगर(U16_IS_SURROGATE(sourceChar)) {
                अगर(U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    अगर(source < sourceLimit) {
                        /* test the following code unit */
                        char16_t trail = *source;
                        अगर(U16_IS_TRAIL(trail)) {
                            source++;
                            sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } अन्यथा {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_FOUND;
                            cnv->fromUChar32=sourceChar;
                            अवरोध;
                        }
                    } अन्यथा {
                        /* no more input */
                        cnv->fromUChar32=sourceChar;
                        अवरोध;
                    }
                } अन्यथा {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_FOUND;
                    cnv->fromUChar32=sourceChar;
                    अवरोध;
                }
            }

             tmp_Target_बफर_लंबाई = 0;
             tmpState = getState(sourceChar);

             अगर (tmpState != DO_खोज && currentState != tmpState) {
                 /* Get escape sequence if necessary */
                 currentState = tmpState;
                 क्रम (i = 0; escSeqCompoundText[currentState][i] != 0; i++) {
                     tmp_Target_बफर[tmp_Target_बफर_लंबाई++] = escSeqCompoundText[currentState][i];
                 }
             }

             अगर (tmpState == DO_खोज) {
                 /* Test all available converters */
                 क्रम (i = 1; i < खोज_लंबाई; i++) {
                     p_मूल्य_लंबाई = ucnv_MBCSFromUChar32(my_Converter_तथ्य->myConverterArray[i], sourceChar, &p_मूल्य, useFallback);
                     अगर (p_मूल्य_लंबाई > 0) {
                         tmpState = (COMPOUND_TEXT_CONVERTERS)i;
                         अगर (currentState != tmpState) {
                             currentState = tmpState;
                             क्रम (j = 0; escSeqCompoundText[currentState][j] != 0; j++) {
                                 tmp_Target_बफर[tmp_Target_बफर_लंबाई++] = escSeqCompoundText[currentState][j];
                             }
                         }
                         क्रम (n = (p_मूल्य_लंबाई - 1); n >= 0; n--) {
                             tmp_Target_बफर[tmp_Target_बफर_लंबाई++] = (uint8_t)(p_मूल्य >> (n * 8));
                         }
                         अवरोध;
                     }
                 }
             } अन्यथा अगर (tmpState == COMPOUND_TEXT_SINGLE_0) {
                 tmp_Target_बफर[tmp_Target_बफर_लंबाई++] = (uint8_t)sourceChar;
             } अन्यथा {
                 p_मूल्य_लंबाई = ucnv_MBCSFromUChar32(my_Converter_तथ्य->myConverterArray[currentState], sourceChar, &p_मूल्य, useFallback);
                 अगर (p_मूल्य_लंबाई > 0) {
                     क्रम (n = (p_मूल्य_लंबाई - 1); n >= 0; n--) {
                         tmp_Target_बफर[tmp_Target_बफर_लंबाई++] = (uint8_t)(p_मूल्य >> (n * 8));
                     }
                 }
             }

             क्रम (i = 0; i < tmp_Target_बफर_लंबाई; i++) {
                 अगर (target < targetLimit) {
                     *target++ = tmp_Target_बफर[i];
                 } अन्यथा {
                     *err = U_बफर_OVERFLOW_ERROR;
                     अवरोध;
                 }
             }

             अगर (*err == U_बफर_OVERFLOW_ERROR) {
                 क्रम (; i < tmp_Target_बफर_लंबाई; i++) {
                     args->converter->char_Error_बफर[args->converter->char_Error_बफर_लंबाई++] = tmp_Target_बफर[i];
                 }
             }
        } अन्यथा {
            *err = U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }
    }

    /*save the state and return */
    my_Converter_तथ्य->state = currentState;
    args->source = source;
    args->target = (अक्षर*)target;
}


अटल व्योम U_CALLCONV
UConverter_toUnicode_CompoundText_OFFSETS(UConverterToUnicodeArgs *args,
                                               UErrorCode* err){
    स्थिर अक्षर *mySource = (अक्षर *) args->source;
    char16_t *myTarget = args->target;
    स्थिर अक्षर *mySourceLimit = args->sourceLimit;
    स्थिर अक्षर *tmpSourceLimit = mySourceLimit;
    uint32_t mySourceChar = 0x0000;
    COMPOUND_TEXT_CONVERTERS currentState, tmpState;
    int32_t sourceOffset = 0;
    UConverter_तथ्य_Compound_Text *my_Converter_तथ्य = (UConverter_तथ्य_Compound_Text *) args->converter->extraInfo;
    UConverter_Shared_तथ्य* saved_Shared_तथ्य = nullptr;

    UConverterToUnicodeArgs subArgs;
    int32_t min_Args_आकार;

    /* set up the subconverter arguments */
    अगर(args->size<माप(UConverterToUnicodeArgs)) {
        min_Args_आकार = args->size;
    } अन्यथा {
        min_Args_आकार = (int32_t)माप(UConverterToUnicodeArgs);
    }

    uprv_memcpy(&subArgs, args, min_Args_आकार);
    subArgs.size = (uint16_t)min_Args_आकार;

    currentState = tmpState =  my_Converter_तथ्य->state;

    जबतक(mySource < mySourceLimit){
        अगर(myTarget < args->targetLimit){
            अगर (args->converter->toULength > 0) {
                mySourceChar = args->converter->toUBytes[0];
            } अन्यथा {
                mySourceChar = (uint8_t)*mySource;
            }

            अगर (mySourceChar == ESC_START) {
                tmpState = findStateFromEscSeq(mySource, mySourceLimit, args->converter->toUBytes, args->converter->toULength, err);

                अगर (*err == U_TRUNCATED_CHAR_FOUND) {
                    क्रम (; mySource < mySourceLimit;) {
                        args->converter->toUBytes[args->converter->toULength++] = *mySource++;
                    }
                    *err = U_ZERO_ERROR;
                    अवरोध;
                } अन्यथा अगर (tmpState == INVALID) {
                    अगर (args->converter->toULength == 0) {
                        mySource++; /* skip over the 0x1b byte */
                    }
                    *err = U_ILLEGAL_CHAR_FOUND;
                    अवरोध;
                }

                अगर (tmpState != currentState) {
                    currentState = tmpState;
                }

                sourceOffset = static_cast<int32_t>(uprv_strlen((अक्षर*)escSeqCompoundText[currentState]) - args->converter->toULength);

                mySource += sourceOffset;

                args->converter->toULength = 0;
            }

            अगर (currentState == COMPOUND_TEXT_SINGLE_0) {
                जबतक (mySource < mySourceLimit) {
                    अगर (*mySource == ESC_START) {
                        अवरोध;
                    }
                    अगर (myTarget < args->targetLimit) {
                        *myTarget++ = 0x00ff&(*mySource++);
                    } अन्यथा {
                        *err = U_बफर_OVERFLOW_ERROR;
                        अवरोध;
                    }
                }
            } अन्यथा अगर (mySource < mySourceLimit){
                sourceOffset = find_अग्र_Esc(mySource, mySourceLimit);

                tmpSourceLimit = mySource + sourceOffset;

                subArgs.source = mySource;
                subArgs.sourceLimit = tmpSourceLimit;
                subArgs.target = myTarget;
                saved_Shared_तथ्य = subArgs.converter->shared_तथ्य;
                subArgs.converter->shared_तथ्य = my_Converter_तथ्य->myConverterArray[currentState];

                ucnv_MBCSToUnicodeWithOffsets(&subArgs, err);

                subArgs.converter->shared_तथ्य = saved_Shared_तथ्य;

                mySource = subArgs.source;
                myTarget = subArgs.target;

                अगर (U_FAILURE(*err)) {
                    अगर(*err == U_बफर_OVERFLOW_ERROR) {
                        अगर(subArgs.converter->UChar_Error_बफर_लंबाई > 0) {
                            uprv_memcpy(args->converter->UChar_Error_बफर, subArgs.converter->UChar_Error_बफर,
                                        subArgs.converter->UChar_Error_बफर_लंबाई);
                        }
                        args->converter->UChar_Error_बफर_लंबाई=subArgs.converter->UChar_Error_बफर_लंबाई;
                        subArgs.converter->UChar_Error_बफर_लंबाई = 0;
                    }
                    अवरोध;
                }
            }
        } अन्यथा {
            *err = U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }
    }
    my_Converter_तथ्य->state = currentState;
    args->target = myTarget;
    args->source = mySource;
}

अटल व्योम U_CALLCONV
_CompoundText_GetUnicodeSet(स्थिर UConverter *cnv,
                    स्थिर USetAdder *sa,
                    UConverterUnicodeSet which,
                    UErrorCode *pErrorCode) {
    UConverter_तथ्य_Compound_Text *my_Converter_तथ्य = (UConverter_तथ्य_Compound_Text *)cnv->extraInfo;
    int32_t i;

    क्रम (i = 1; i < अंक_OF_CONVERTERS; i++) {
        ucnv_MBCSGetUnicodeSetForUnicode(my_Converter_तथ्य->myConverterArray[i], sa, which, pErrorCode);
    }
    sa->add(sa->set, 0x0000);
    sa->add(sa->set, 0x0009);
    sa->add(sa->set, 0x000A);
    sa->addRange(sa->set, 0x0020, 0x007F);
    sa->addRange(sa->set, 0x00A0, 0x00FF);
}
U_CDECL_END

अटल स्थिर UConverterImpl _CompoundTextImpl = {

    UCNV_COMPOUND_TEXT,

    nullptr,
    nullptr,

    _CompoundTextOpen,
    _CompoundTextClose,
    _CompoundTextReset,

    UConverter_toUnicode_CompoundText_OFFSETS,
    UConverter_toUnicode_CompoundText_OFFSETS,
    UConverter_fromUnicode_CompoundText_OFFSETS,
    UConverter_fromUnicode_CompoundText_OFFSETS,
    nullptr,

    nullptr,
    _CompoundTextgetName,
    nullptr,
    nullptr,
    _CompoundText_GetUnicodeSet,
    nullptr,
    nullptr
};

अटल स्थिर UConverter_Static_तथ्य Compound_Text_Static_तथ्य = {
    माप(UConverter_Static_तथ्य),
    "COMPOUND_TEXT",
    0,
    UCNV_IBM,
    UCNV_COMPOUND_TEXT,
    1,
    6,
    { 0xef, 0, 0, 0 },
    1,
    false,
    false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};
स्थिर UConverter_Shared_तथ्य Compound_Text_तथ्य =
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&Compound_Text_Static_तथ्य, &_CompoundTextImpl);

#पूर्ण_अगर /* #if !UCONFIG_NO_LEGACY_CONVERSION */
