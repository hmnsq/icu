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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#அந்தர்பாவ "unicode/ucnv.h"
#அந்தர்பாவ "unicode/uset.h"
#அந்தர்பாவ "unicode/ucnv_err.h"
#அந்தர்பாவ "unicode/ucnv_cb.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "ucnv_imp.h"
#அந்தர்பாவ "ucnv_bld.h"
#அந்தர்பாவ "ucnv_cnv.h"
#அந்தர்பாவ "ucnvmbcs.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "cmemory.h"

வகைவரை எண்ணிக்கை {
    INVALID = -2,
    DO_தேடல் = -1,

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
    இலக்கம்_OF_CONVERTERS = 20
} COMPOUND_TEXT_CONVERTERS;

#define தேடல்_LENGTH 12

நிலையான மாறிலி uint8_t escSeqCompoundText[இலக்கம்_OF_CONVERTERS][5] = {
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

#define ESC_START 0x1B

#define isASCIIRange(codepoint) \
        ((codepoint == 0x0000) || (codepoint == 0x0009) || (codepoint == 0x000A) || \
         (codepoint >= 0x0020 && codepoint <= 0x007f) || (codepoint >= 0x00A0 && codepoint <= 0x00FF))

#define isIBM915(codepoint) \
        ((codepoint >= 0x0401 && codepoint <= 0x045F) || (codepoint == 0x2116))

#define isIBM916(codepoint) \
        ((codepoint >= 0x05D0 && codepoint <= 0x05EA) || (codepoint == 0x2017) || (codepoint == 0x203E))

#define isCompoundS3(codepoint) \
        ((codepoint == 0x060C) || (codepoint == 0x061B) || (codepoint == 0x061F) || (codepoint >= 0x0621 && codepoint <= 0x063A) || \
         (codepoint >= 0x0640 && codepoint <= 0x0652) || (codepoint >= 0x0660 && codepoint <= 0x066D) || (codepoint == 0x200B) || \
         (codepoint >= 0x0FE70 && codepoint <= 0x0FE72) || (codepoint == 0x0FE74) || (codepoint >= 0x0FE76 && codepoint <= 0x0FEBE))

#define isCompoundS2(codepoint) \
        ((codepoint == 0x02BC) || (codepoint == 0x02BD) || (codepoint >= 0x0384 && codepoint <= 0x03CE) || (codepoint == 0x2015))

#define isIBM914(codepoint) \
        ((codepoint == 0x0100) || (codepoint == 0x0101) || (codepoint == 0x0112) || (codepoint == 0x0113) || (codepoint == 0x0116) || (codepoint == 0x0117) || \
         (codepoint == 0x0122) || (codepoint == 0x0123) || (codepoint >= 0x0128 && codepoint <= 0x012B) || (codepoint == 0x012E) || (codepoint == 0x012F) || \
         (codepoint >= 0x0136 && codepoint <= 0x0138) || (codepoint == 0x013B) || (codepoint == 0x013C) || (codepoint == 0x0145) || (codepoint ==  0x0146) || \
         (codepoint >= 0x014A && codepoint <= 0x014D) || (codepoint == 0x0156) || (codepoint == 0x0157) || (codepoint >= 0x0166 && codepoint <= 0x016B) || \
         (codepoint == 0x0172) || (codepoint == 0x0173))

#define isIBM874(codepoint) \
        ((codepoint >= 0x0E01 && codepoint <= 0x0E3A) || (codepoint >= 0x0E3F && codepoint <= 0x0E5B))

#define isIBM912(codepoint) \
        ((codepoint >= 0x0102 && codepoint <= 0x0107) || (codepoint >= 0x010C && codepoint <= 0x0111) || (codepoint >= 0x0118 && codepoint <= 0x011B) || \
         (codepoint == 0x0139) || (codepoint == 0x013A) || (codepoint == 0x013D) || (codepoint == 0x013E) || (codepoint >= 0x0141 && codepoint <= 0x0144) || \
         (codepoint == 0x0147) || (codepoint == 0x0147) || (codepoint == 0x0150) || (codepoint == 0x0151) || (codepoint == 0x0154) || (codepoint == 0x0155) || \
         (codepoint >= 0x0158 && codepoint <= 0x015B) || (codepoint == 0x015E) || (codepoint == 0x015F) || (codepoint >= 0x0160 && codepoint <= 0x0165) || \
         (codepoint == 0x016E) || (codepoint == 0x016F) || (codepoint == 0x0170) || (codepoint ==  0x0171) || (codepoint >= 0x0179 && codepoint <= 0x017E) || \
         (codepoint == 0x02C7) || (codepoint == 0x02D8) || (codepoint == 0x02D9) || (codepoint == 0x02DB) || (codepoint == 0x02DD))

#define isIBM913(codepoint) \
        ((codepoint >= 0x0108 && codepoint <= 0x010B) || (codepoint == 0x011C) || \
         (codepoint == 0x011D) || (codepoint == 0x0120) || (codepoint == 0x0121) || \
         (codepoint >= 0x0124 && codepoint <= 0x0127) || (codepoint == 0x0134) || (codepoint == 0x0135) || \
         (codepoint == 0x015C) || (codepoint == 0x015D) || (codepoint == 0x016C) || (codepoint ==  0x016D))

#define isCompoundS1(codepoint) \
        ((codepoint == 0x011E) || (codepoint == 0x011F) || (codepoint == 0x0130) || \
         (codepoint == 0x0131) || (codepoint >= 0x0218 && codepoint <= 0x021B))

#define isISO8859_14(codepoint) \
        ((codepoint >= 0x0174 && codepoint <= 0x0177) || (codepoint == 0x1E0A) || \
         (codepoint == 0x1E0B) || (codepoint == 0x1E1E) || (codepoint == 0x1E1F) || \
         (codepoint == 0x1E40) || (codepoint == 0x1E41) || (codepoint == 0x1E56) || \
         (codepoint == 0x1E57) || (codepoint == 0x1E60) || (codepoint == 0x1E61) || \
         (codepoint == 0x1E6A) || (codepoint == 0x1E6B) || (codepoint == 0x1EF2) || \
         (codepoint == 0x1EF3) || (codepoint >= 0x1E80 && codepoint <= 0x1E85))

#define isIBM923(codepoint) \
        ((codepoint == 0x0152) || (codepoint == 0x0153) || (codepoint == 0x0178) || (codepoint == 0x20AC))


வகைவரை கட்டமைப்பு{
    UConverterSharedData *myConverterArray[இலக்கம்_OF_CONVERTERS];
    COMPOUND_TEXT_CONVERTERS state;
} UConverterDataCompoundText;

/*********** Compound Text Converter Protos ***********/
U_CDECL_BEGIN
நிலையான வெறுமை U_CALLCONV
_CompoundTextOpen(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode);

நிலையான வெறுமை U_CALLCONV
 _CompoundTextClose(UConverter *converter);

நிலையான வெறுமை U_CALLCONV
_CompoundTextReset(UConverter *converter, UConverterResetChoice choice);

நிலையான மாறிலி எழுத்து* U_CALLCONV
_CompoundTextgetName(மாறிலி UConverter* cnv);


நிலையான int32_t findNextEsc(மாறிலி எழுத்து *source, மாறிலி எழுத்து *sourceLimit) {
    int32_t length = static_cast<int32_t>(sourceLimit - source);
    int32_t i;
    க்கு (i = 1; i < length; i++) {
        எனில் (*(source + i) == 0x1B) {
            திரும்பு i;
        }
    }

    திரும்பு length;
}

நிலையான COMPOUND_TEXT_CONVERTERS getState(முழு codepoint) {
    COMPOUND_TEXT_CONVERTERS state = DO_தேடல்;

    எனில் (isASCIIRange(codepoint)) {
        state = COMPOUND_TEXT_SINGLE_0;
    } இல்லையெனில் எனில் (isIBM912(codepoint)) {
        state = IBM_912;
    }இல்லையெனில் எனில் (isIBM913(codepoint)) {
        state = IBM_913;
    } இல்லையெனில் எனில் (isISO8859_14(codepoint)) {
        state = ISO_8859_14;
    } இல்லையெனில் எனில் (isIBM923(codepoint)) {
        state = IBM_923;
    } இல்லையெனில் எனில் (isIBM874(codepoint)) {
        state = IBM_874;
    } இல்லையெனில் எனில் (isIBM914(codepoint)) {
        state = IBM_914;
    } இல்லையெனில் எனில் (isCompoundS2(codepoint)) {
        state = COMPOUND_TEXT_SINGLE_2;
    } இல்லையெனில் எனில் (isCompoundS3(codepoint)) {
        state = COMPOUND_TEXT_SINGLE_3;
    } இல்லையெனில் எனில் (isIBM916(codepoint)) {
        state = IBM_916;
    } இல்லையெனில் எனில் (isIBM915(codepoint)) {
        state = IBM_915;
    } இல்லையெனில் எனில் (isCompoundS1(codepoint)) {
        state = COMPOUND_TEXT_SINGLE_1;
    }

    திரும்பு state;
}

நிலையான COMPOUND_TEXT_CONVERTERS findStateFromEscSeq(மாறிலி எழுத்து* source, மாறிலி எழுத்து* sourceLimit, மாறிலி uint8_t* toUBytesBuffer, int32_t toUBytesBufferLength, UErrorCode *err) {
    COMPOUND_TEXT_CONVERTERS state = INVALID;
    UBool matchFound = பொய்;
    int32_t i, n, offset = toUBytesBufferLength;

    க்கு (i = 0; i < இலக்கம்_OF_CONVERTERS; i++) {
        matchFound = உண்மை;
        க்கு (n = 0; escSeqCompoundText[i][n] != 0; n++) {
            எனில் (n < toUBytesBufferLength) {
                எனில் (toUBytesBuffer[n] != escSeqCompoundText[i][n]) {
                    matchFound = பொய்;
                    நிறுத்து;
                }
            } இல்லையெனில் எனில் ((source + (n - offset)) >= sourceLimit) {
                *err = U_TRUNCATED_CHAR_FOUND;
                matchFound = பொய்;
                நிறுத்து;
            } இல்லையெனில் எனில் (*(source + (n - offset)) != escSeqCompoundText[i][n]) {
                matchFound = பொய்;
                நிறுத்து;
            }
        }

        எனில் (matchFound) {
            நிறுத்து;
        }
    }

    எனில் (matchFound) {
        state = (COMPOUND_TEXT_CONVERTERS)i;
    }

    திரும்பு state;
}

நிலையான வெறுமை U_CALLCONV
_CompoundTextOpen(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode){
    cnv->extraInfo = uprv_malloc (அளவு (UConverterDataCompoundText));
    எனில் (cnv->extraInfo != nullptr) {
        UConverterDataCompoundText *myConverterData = (UConverterDataCompoundText *) cnv->extraInfo;

        UConverterNamePieces அடுக்கு_Pieces;
        UConverterLoadArgs அடுக்கு_Args=UCNV_LOAD_ARGS_INITIALIZER;

        myConverterData->myConverterArray[COMPOUND_TEXT_SINGLE_0] = nullptr;
        myConverterData->myConverterArray[COMPOUND_TEXT_SINGLE_1] = ucnv_loadSharedData("icu-internal-compound-s1", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_SINGLE_2] = ucnv_loadSharedData("icu-internal-compound-s2", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_SINGLE_3] = ucnv_loadSharedData("icu-internal-compound-s3", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_DOUBLE_1] = ucnv_loadSharedData("icu-internal-compound-d1", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_DOUBLE_2] = ucnv_loadSharedData("icu-internal-compound-d2", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_DOUBLE_3] = ucnv_loadSharedData("icu-internal-compound-d3", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_DOUBLE_4] = ucnv_loadSharedData("icu-internal-compound-d4", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_DOUBLE_5] = ucnv_loadSharedData("icu-internal-compound-d5", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_DOUBLE_6] = ucnv_loadSharedData("icu-internal-compound-d6", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_DOUBLE_7] = ucnv_loadSharedData("icu-internal-compound-d7", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_TRIPLE_DOUBLE] = ucnv_loadSharedData("icu-internal-compound-t", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);

        myConverterData->myConverterArray[IBM_915] = ucnv_loadSharedData("ibm-915_P100-1995", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[IBM_916] = ucnv_loadSharedData("ibm-916_P100-1995", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[IBM_914] = ucnv_loadSharedData("ibm-914_P100-1995", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[IBM_874] = ucnv_loadSharedData("ibm-874_P100-1995", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[IBM_912] = ucnv_loadSharedData("ibm-912_P100-1995", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[IBM_913] = ucnv_loadSharedData("ibm-913_P100-2000", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[ISO_8859_14] = ucnv_loadSharedData("iso-8859_14-1998", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);
        myConverterData->myConverterArray[IBM_923] = ucnv_loadSharedData("ibm-923_P100-1998", &அடுக்கு_Pieces, &அடுக்கு_Args, errorCode);

        எனில் (U_FAILURE(*errorCode) || pArgs->onlyTestIsLoadable) {
            _CompoundTextClose(cnv);
            திரும்பு;
        }

        myConverterData->state = (COMPOUND_TEXT_CONVERTERS)0;
    } இல்லையெனில் {
        *errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}


நிலையான வெறுமை U_CALLCONV
_CompoundTextClose(UConverter *converter) {
    UConverterDataCompoundText* myConverterData = (UConverterDataCompoundText*)(converter->extraInfo);
    int32_t i;

    எனில் (converter->extraInfo != nullptr) {
        /*close the array of converter pointers and free the memory*/
        க்கு (i = 0; i < இலக்கம்_OF_CONVERTERS; i++) {
            எனில் (myConverterData->myConverterArray[i] != nullptr) {
                ucnv_unloadSharedDataIfReady(myConverterData->myConverterArray[i]);
            }
        }

        uprv_free(converter->extraInfo);
        converter->extraInfo = nullptr;
    }
}

நிலையான வெறுமை U_CALLCONV
_CompoundTextReset(UConverter *converter, UConverterResetChoice choice) {
    (வெறுமை)converter;
    (வெறுமை)choice;
}

நிலையான மாறிலி எழுத்து* U_CALLCONV
_CompoundTextgetName(மாறிலி UConverter* cnv){
    (வெறுமை)cnv;
    திரும்பு "x11-compound-text";
}

நிலையான வெறுமை U_CALLCONV
UConverter_fromUnicode_CompoundText_OFFSETS(UConverterFromUnicodeArgs* args, UErrorCode* err){
    UConverter *cnv = args->converter;
    uint8_t *target = (uint8_t *) args->target;
    மாறிலி uint8_t *targetLimit = (மாறிலி uint8_t *) args->targetLimit;
    மாறிலி char16_t* source = args->source;
    மாறிலி char16_t* sourceLimit = args->sourceLimit;
    /* int32_t* offsets = args->offsets; */
    UChar32 sourceChar;
    UBool useFallback = cnv->useFallback;
    uint8_t tmpTargetBuffer[7];
    int32_t tmpTargetBufferLength = 0;
    COMPOUND_TEXT_CONVERTERS currentState, tmpState;
    uint32_t p_மதிப்பு;
    int32_t p_மதிப்பு_Length = 0;
    int32_t i, n, j;

    UConverterDataCompoundText *myConverterData = (UConverterDataCompoundText *) cnv->extraInfo;

    currentState = myConverterData->state;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    எனில்((sourceChar = cnv->fromUChar32)!=0 && target< targetLimit) {
        செல் getTrail;
    }

    போது( source < sourceLimit){
        எனில்(target < targetLimit){

            sourceChar  = *(source++);
            /*check if the char is a First surrogate*/
             எனில்(U16_IS_SURROGATE(sourceChar)) {
                எனில்(U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    எனில்(source < sourceLimit) {
                        /* test the following code unit */
                        char16_t trail = *source;
                        எனில்(U16_IS_TRAIL(trail)) {
                            source++;
                            sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } இல்லையெனில் {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_FOUND;
                            cnv->fromUChar32=sourceChar;
                            நிறுத்து;
                        }
                    } இல்லையெனில் {
                        /* no more input */
                        cnv->fromUChar32=sourceChar;
                        நிறுத்து;
                    }
                } இல்லையெனில் {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_FOUND;
                    cnv->fromUChar32=sourceChar;
                    நிறுத்து;
                }
            }

             tmpTargetBufferLength = 0;
             tmpState = getState(sourceChar);

             எனில் (tmpState != DO_தேடல் && currentState != tmpState) {
                 /* Get escape sequence if necessary */
                 currentState = tmpState;
                 க்கு (i = 0; escSeqCompoundText[currentState][i] != 0; i++) {
                     tmpTargetBuffer[tmpTargetBufferLength++] = escSeqCompoundText[currentState][i];
                 }
             }

             எனில் (tmpState == DO_தேடல்) {
                 /* Test all available converters */
                 க்கு (i = 1; i < தேடல்_LENGTH; i++) {
                     p_மதிப்பு_Length = ucnv_MBCSFromUChar32(myConverterData->myConverterArray[i], sourceChar, &p_மதிப்பு, useFallback);
                     எனில் (p_மதிப்பு_Length > 0) {
                         tmpState = (COMPOUND_TEXT_CONVERTERS)i;
                         எனில் (currentState != tmpState) {
                             currentState = tmpState;
                             க்கு (j = 0; escSeqCompoundText[currentState][j] != 0; j++) {
                                 tmpTargetBuffer[tmpTargetBufferLength++] = escSeqCompoundText[currentState][j];
                             }
                         }
                         க்கு (n = (p_மதிப்பு_Length - 1); n >= 0; n--) {
                             tmpTargetBuffer[tmpTargetBufferLength++] = (uint8_t)(p_மதிப்பு >> (n * 8));
                         }
                         நிறுத்து;
                     }
                 }
             } இல்லையெனில் எனில் (tmpState == COMPOUND_TEXT_SINGLE_0) {
                 tmpTargetBuffer[tmpTargetBufferLength++] = (uint8_t)sourceChar;
             } இல்லையெனில் {
                 p_மதிப்பு_Length = ucnv_MBCSFromUChar32(myConverterData->myConverterArray[currentState], sourceChar, &p_மதிப்பு, useFallback);
                 எனில் (p_மதிப்பு_Length > 0) {
                     க்கு (n = (p_மதிப்பு_Length - 1); n >= 0; n--) {
                         tmpTargetBuffer[tmpTargetBufferLength++] = (uint8_t)(p_மதிப்பு >> (n * 8));
                     }
                 }
             }

             க்கு (i = 0; i < tmpTargetBufferLength; i++) {
                 எனில் (target < targetLimit) {
                     *target++ = tmpTargetBuffer[i];
                 } இல்லையெனில் {
                     *err = U_BUFFER_OVERFLOW_ERROR;
                     நிறுத்து;
                 }
             }

             எனில் (*err == U_BUFFER_OVERFLOW_ERROR) {
                 க்கு (; i < tmpTargetBufferLength; i++) {
                     args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = tmpTargetBuffer[i];
                 }
             }
        } இல்லையெனில் {
            *err = U_BUFFER_OVERFLOW_ERROR;
            நிறுத்து;
        }
    }

    /*save the state and return */
    myConverterData->state = currentState;
    args->source = source;
    args->target = (எழுத்து*)target;
}


நிலையான வெறுமை U_CALLCONV
UConverter_toUnicode_CompoundText_OFFSETS(UConverterToUnicodeArgs *args,
                                               UErrorCode* err){
    மாறிலி எழுத்து *mySource = (எழுத்து *) args->source;
    char16_t *myTarget = args->target;
    மாறிலி எழுத்து *mySourceLimit = args->sourceLimit;
    மாறிலி எழுத்து *tmpSourceLimit = mySourceLimit;
    uint32_t mySourceChar = 0x0000;
    COMPOUND_TEXT_CONVERTERS currentState, tmpState;
    int32_t sourceOffset = 0;
    UConverterDataCompoundText *myConverterData = (UConverterDataCompoundText *) args->converter->extraInfo;
    UConverterSharedData* savedSharedData = nullptr;

    UConverterToUnicodeArgs subArgs;
    int32_t minArgsSize;

    /* set up the subconverter arguments */
    எனில்(args->size<அளவு(UConverterToUnicodeArgs)) {
        minArgsSize = args->size;
    } இல்லையெனில் {
        minArgsSize = (int32_t)அளவு(UConverterToUnicodeArgs);
    }

    uprv_memcpy(&subArgs, args, minArgsSize);
    subArgs.size = (uint16_t)minArgsSize;

    currentState = tmpState =  myConverterData->state;

    போது(mySource < mySourceLimit){
        எனில்(myTarget < args->targetLimit){
            எனில் (args->converter->toULength > 0) {
                mySourceChar = args->converter->toUBytes[0];
            } இல்லையெனில் {
                mySourceChar = (uint8_t)*mySource;
            }

            எனில் (mySourceChar == ESC_START) {
                tmpState = findStateFromEscSeq(mySource, mySourceLimit, args->converter->toUBytes, args->converter->toULength, err);

                எனில் (*err == U_TRUNCATED_CHAR_FOUND) {
                    க்கு (; mySource < mySourceLimit;) {
                        args->converter->toUBytes[args->converter->toULength++] = *mySource++;
                    }
                    *err = U_ZERO_ERROR;
                    நிறுத்து;
                } இல்லையெனில் எனில் (tmpState == INVALID) {
                    எனில் (args->converter->toULength == 0) {
                        mySource++; /* skip over the 0x1b byte */
                    }
                    *err = U_ILLEGAL_CHAR_FOUND;
                    நிறுத்து;
                }

                எனில் (tmpState != currentState) {
                    currentState = tmpState;
                }

                sourceOffset = static_cast<int32_t>(uprv_strlen((எழுத்து*)escSeqCompoundText[currentState]) - args->converter->toULength);

                mySource += sourceOffset;

                args->converter->toULength = 0;
            }

            எனில் (currentState == COMPOUND_TEXT_SINGLE_0) {
                போது (mySource < mySourceLimit) {
                    எனில் (*mySource == ESC_START) {
                        நிறுத்து;
                    }
                    எனில் (myTarget < args->targetLimit) {
                        *myTarget++ = 0x00ff&(*mySource++);
                    } இல்லையெனில் {
                        *err = U_BUFFER_OVERFLOW_ERROR;
                        நிறுத்து;
                    }
                }
            } இல்லையெனில் எனில் (mySource < mySourceLimit){
                sourceOffset = findNextEsc(mySource, mySourceLimit);

                tmpSourceLimit = mySource + sourceOffset;

                subArgs.source = mySource;
                subArgs.sourceLimit = tmpSourceLimit;
                subArgs.target = myTarget;
                savedSharedData = subArgs.converter->sharedData;
                subArgs.converter->sharedData = myConverterData->myConverterArray[currentState];

                ucnv_MBCSToUnicodeWithOffsets(&subArgs, err);

                subArgs.converter->sharedData = savedSharedData;

                mySource = subArgs.source;
                myTarget = subArgs.target;

                எனில் (U_FAILURE(*err)) {
                    எனில்(*err == U_BUFFER_OVERFLOW_ERROR) {
                        எனில்(subArgs.converter->UCharErrorBufferLength > 0) {
                            uprv_memcpy(args->converter->UCharErrorBuffer, subArgs.converter->UCharErrorBuffer,
                                        subArgs.converter->UCharErrorBufferLength);
                        }
                        args->converter->UCharErrorBufferLength=subArgs.converter->UCharErrorBufferLength;
                        subArgs.converter->UCharErrorBufferLength = 0;
                    }
                    நிறுத்து;
                }
            }
        } இல்லையெனில் {
            *err = U_BUFFER_OVERFLOW_ERROR;
            நிறுத்து;
        }
    }
    myConverterData->state = currentState;
    args->target = myTarget;
    args->source = mySource;
}

நிலையான வெறுமை U_CALLCONV
_CompoundText_GetUnicodeSet(மாறிலி UConverter *cnv,
                    மாறிலி USetAdder *sa,
                    UConverterUnicodeSet which,
                    UErrorCode *pErrorCode) {
    UConverterDataCompoundText *myConverterData = (UConverterDataCompoundText *)cnv->extraInfo;
    int32_t i;

    க்கு (i = 1; i < இலக்கம்_OF_CONVERTERS; i++) {
        ucnv_MBCSGetUnicodeSetForUnicode(myConverterData->myConverterArray[i], sa, which, pErrorCode);
    }
    sa->add(sa->set, 0x0000);
    sa->add(sa->set, 0x0009);
    sa->add(sa->set, 0x000A);
    sa->addRange(sa->set, 0x0020, 0x007F);
    sa->addRange(sa->set, 0x00A0, 0x00FF);
}
U_CDECL_END

நிலையான மாறிலி UConverterImpl _CompoundTextImpl = {

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

நிலையான மாறிலி UConverterStaticData _CompoundTextStaticData = {
    அளவு(UConverterStaticData),
    "COMPOUND_TEXT",
    0,
    UCNV_IBM,
    UCNV_COMPOUND_TEXT,
    1,
    6,
    { 0xef, 0, 0, 0 },
    1,
    பொய்,
    பொய்,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};
மாறிலி UConverterSharedData _CompoundTextData =
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_CompoundTextStaticData, &_CompoundTextImpl);

#endif /* #if !UCONFIG_NO_LEGACY_CONVERSION */
