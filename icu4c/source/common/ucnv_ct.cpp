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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#અન્તર્ભાવ "unicode/ucnv.h"
#અન્તર્ભાવ "unicode/uset.h"
#અન્તર્ભાવ "unicode/ucnv_err.h"
#અન્તર્ભાવ "unicode/ucnv_cb.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "ucnv_imp.h"
#અન્તર્ભાવ "ucnv_bld.h"
#અન્તર્ભાવ "ucnv_cnv.h"
#અન્તર્ભાવ "ucnvmbcs.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "cmemory.h"

પ્રકારવ્યાખ્યા ગણના {
    INVALID = -2,
    DO_શોધ = -1,

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
    અંક_OF_CONVERTERS = 20
} COMPOUND_TEXT_CONVERTERS;

#define શોધ_LENGTH 12

સ્થિર અચલ uint8_t escSeqCompoundText[અંક_OF_CONVERTERS][5] = {
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


પ્રકારવ્યાખ્યા સંરચના{
    UConverterSharedData *myConverterArray[અંક_OF_CONVERTERS];
    COMPOUND_TEXT_CONVERTERS state;
} UConverterDataCompoundText;

/*********** Compound Text Converter Protos ***********/
U_CDECL_BEGIN
સ્થિર શૂન્ય U_CALLCONV
_CompoundTextOpen(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode);

સ્થિર શૂન્ય U_CALLCONV
 _CompoundTextClose(UConverter *converter);

સ્થિર શૂન્ય U_CALLCONV
_CompoundTextReset(UConverter *converter, UConverterResetChoice choice);

સ્થિર અચલ અક્ષર* U_CALLCONV
_CompoundTextgetName(અચલ UConverter* cnv);


સ્થિર int32_t findNextEsc(અચલ અક્ષર *source, અચલ અક્ષર *sourceLimit) {
    int32_t length = static_cast<int32_t>(sourceLimit - source);
    int32_t i;
    માટે (i = 1; i < length; i++) {
        જો (*(source + i) == 0x1B) {
            પાછા i;
        }
    }

    પાછા length;
}

સ્થિર COMPOUND_TEXT_CONVERTERS getState(પૂર્ણાંક codepoint) {
    COMPOUND_TEXT_CONVERTERS state = DO_શોધ;

    જો (isASCIIRange(codepoint)) {
        state = COMPOUND_TEXT_SINGLE_0;
    } નહીંતર જો (isIBM912(codepoint)) {
        state = IBM_912;
    }નહીંતર જો (isIBM913(codepoint)) {
        state = IBM_913;
    } નહીંતર જો (isISO8859_14(codepoint)) {
        state = ISO_8859_14;
    } નહીંતર જો (isIBM923(codepoint)) {
        state = IBM_923;
    } નહીંતર જો (isIBM874(codepoint)) {
        state = IBM_874;
    } નહીંતર જો (isIBM914(codepoint)) {
        state = IBM_914;
    } નહીંતર જો (isCompoundS2(codepoint)) {
        state = COMPOUND_TEXT_SINGLE_2;
    } નહીંતર જો (isCompoundS3(codepoint)) {
        state = COMPOUND_TEXT_SINGLE_3;
    } નહીંતર જો (isIBM916(codepoint)) {
        state = IBM_916;
    } નહીંતર જો (isIBM915(codepoint)) {
        state = IBM_915;
    } નહીંતર જો (isCompoundS1(codepoint)) {
        state = COMPOUND_TEXT_SINGLE_1;
    }

    પાછા state;
}

સ્થિર COMPOUND_TEXT_CONVERTERS findStateFromEscSeq(અચલ અક્ષર* source, અચલ અક્ષર* sourceLimit, અચલ uint8_t* toUBytesBuffer, int32_t toUBytesBufferLength, UErrorCode *err) {
    COMPOUND_TEXT_CONVERTERS state = INVALID;
    UBool matchFound = ખોટું;
    int32_t i, n, offset = toUBytesBufferLength;

    માટે (i = 0; i < અંક_OF_CONVERTERS; i++) {
        matchFound = સાચું;
        માટે (n = 0; escSeqCompoundText[i][n] != 0; n++) {
            જો (n < toUBytesBufferLength) {
                જો (toUBytesBuffer[n] != escSeqCompoundText[i][n]) {
                    matchFound = ખોટું;
                    તોડો;
                }
            } નહીંતર જો ((source + (n - offset)) >= sourceLimit) {
                *err = U_TRUNCATED_CHAR_FOUND;
                matchFound = ખોટું;
                તોડો;
            } નહીંતર જો (*(source + (n - offset)) != escSeqCompoundText[i][n]) {
                matchFound = ખોટું;
                તોડો;
            }
        }

        જો (matchFound) {
            તોડો;
        }
    }

    જો (matchFound) {
        state = (COMPOUND_TEXT_CONVERTERS)i;
    }

    પાછા state;
}

સ્થિર શૂન્ય U_CALLCONV
_CompoundTextOpen(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode){
    cnv->extraInfo = uprv_malloc (કદ (UConverterDataCompoundText));
    જો (cnv->extraInfo != nullptr) {
        UConverterDataCompoundText *myConverterData = (UConverterDataCompoundText *) cnv->extraInfo;

        UConverterNamePieces ઢગલો_Pieces;
        UConverterLoadArgs ઢગલો_Args=UCNV_LOAD_ARGS_INITIALIZER;

        myConverterData->myConverterArray[COMPOUND_TEXT_SINGLE_0] = nullptr;
        myConverterData->myConverterArray[COMPOUND_TEXT_SINGLE_1] = ucnv_loadSharedData("icu-internal-compound-s1", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_SINGLE_2] = ucnv_loadSharedData("icu-internal-compound-s2", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_SINGLE_3] = ucnv_loadSharedData("icu-internal-compound-s3", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_DOUBLE_1] = ucnv_loadSharedData("icu-internal-compound-d1", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_DOUBLE_2] = ucnv_loadSharedData("icu-internal-compound-d2", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_DOUBLE_3] = ucnv_loadSharedData("icu-internal-compound-d3", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_DOUBLE_4] = ucnv_loadSharedData("icu-internal-compound-d4", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_DOUBLE_5] = ucnv_loadSharedData("icu-internal-compound-d5", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_DOUBLE_6] = ucnv_loadSharedData("icu-internal-compound-d6", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_DOUBLE_7] = ucnv_loadSharedData("icu-internal-compound-d7", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[COMPOUND_TEXT_TRIPLE_DOUBLE] = ucnv_loadSharedData("icu-internal-compound-t", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);

        myConverterData->myConverterArray[IBM_915] = ucnv_loadSharedData("ibm-915_P100-1995", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[IBM_916] = ucnv_loadSharedData("ibm-916_P100-1995", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[IBM_914] = ucnv_loadSharedData("ibm-914_P100-1995", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[IBM_874] = ucnv_loadSharedData("ibm-874_P100-1995", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[IBM_912] = ucnv_loadSharedData("ibm-912_P100-1995", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[IBM_913] = ucnv_loadSharedData("ibm-913_P100-2000", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[ISO_8859_14] = ucnv_loadSharedData("iso-8859_14-1998", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);
        myConverterData->myConverterArray[IBM_923] = ucnv_loadSharedData("ibm-923_P100-1998", &ઢગલો_Pieces, &ઢગલો_Args, errorCode);

        જો (U_FAILURE(*errorCode) || pArgs->onlyTestIsLoadable) {
            _CompoundTextClose(cnv);
            પાછા;
        }

        myConverterData->state = (COMPOUND_TEXT_CONVERTERS)0;
    } નહીંતર {
        *errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}


સ્થિર શૂન્ય U_CALLCONV
_CompoundTextClose(UConverter *converter) {
    UConverterDataCompoundText* myConverterData = (UConverterDataCompoundText*)(converter->extraInfo);
    int32_t i;

    જો (converter->extraInfo != nullptr) {
        /*close the array of converter pointers and free the memory*/
        માટે (i = 0; i < અંક_OF_CONVERTERS; i++) {
            જો (myConverterData->myConverterArray[i] != nullptr) {
                ucnv_unloadSharedDataIfReady(myConverterData->myConverterArray[i]);
            }
        }

        uprv_free(converter->extraInfo);
        converter->extraInfo = nullptr;
    }
}

સ્થિર શૂન્ય U_CALLCONV
_CompoundTextReset(UConverter *converter, UConverterResetChoice choice) {
    (શૂન્ય)converter;
    (શૂન્ય)choice;
}

સ્થિર અચલ અક્ષર* U_CALLCONV
_CompoundTextgetName(અચલ UConverter* cnv){
    (શૂન્ય)cnv;
    પાછા "x11-compound-text";
}

સ્થિર શૂન્ય U_CALLCONV
UConverter_fromUnicode_CompoundText_OFFSETS(UConverterFromUnicodeArgs* args, UErrorCode* err){
    UConverter *cnv = args->converter;
    uint8_t *target = (uint8_t *) args->target;
    અચલ uint8_t *targetLimit = (અચલ uint8_t *) args->targetLimit;
    અચલ char16_t* source = args->source;
    અચલ char16_t* sourceLimit = args->sourceLimit;
    /* int32_t* offsets = args->offsets; */
    UChar32 sourceChar;
    UBool useFallback = cnv->useFallback;
    uint8_t tmpTargetBuffer[7];
    int32_t tmpTargetBufferLength = 0;
    COMPOUND_TEXT_CONVERTERS currentState, tmpState;
    uint32_t p_કિંમત;
    int32_t p_કિંમત_Length = 0;
    int32_t i, n, j;

    UConverterDataCompoundText *myConverterData = (UConverterDataCompoundText *) cnv->extraInfo;

    currentState = myConverterData->state;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    જો((sourceChar = cnv->fromUChar32)!=0 && target< targetLimit) {
        જાઓ getTrail;
    }

    જ્યારે( source < sourceLimit){
        જો(target < targetLimit){

            sourceChar  = *(source++);
            /*check if the char is a First surrogate*/
             જો(U16_IS_SURROGATE(sourceChar)) {
                જો(U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    જો(source < sourceLimit) {
                        /* test the following code unit */
                        char16_t trail = *source;
                        જો(U16_IS_TRAIL(trail)) {
                            source++;
                            sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } નહીંતર {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_FOUND;
                            cnv->fromUChar32=sourceChar;
                            તોડો;
                        }
                    } નહીંતર {
                        /* no more input */
                        cnv->fromUChar32=sourceChar;
                        તોડો;
                    }
                } નહીંતર {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_FOUND;
                    cnv->fromUChar32=sourceChar;
                    તોડો;
                }
            }

             tmpTargetBufferLength = 0;
             tmpState = getState(sourceChar);

             જો (tmpState != DO_શોધ && currentState != tmpState) {
                 /* Get escape sequence if necessary */
                 currentState = tmpState;
                 માટે (i = 0; escSeqCompoundText[currentState][i] != 0; i++) {
                     tmpTargetBuffer[tmpTargetBufferLength++] = escSeqCompoundText[currentState][i];
                 }
             }

             જો (tmpState == DO_શોધ) {
                 /* Test all available converters */
                 માટે (i = 1; i < શોધ_LENGTH; i++) {
                     p_કિંમત_Length = ucnv_MBCSFromUChar32(myConverterData->myConverterArray[i], sourceChar, &p_કિંમત, useFallback);
                     જો (p_કિંમત_Length > 0) {
                         tmpState = (COMPOUND_TEXT_CONVERTERS)i;
                         જો (currentState != tmpState) {
                             currentState = tmpState;
                             માટે (j = 0; escSeqCompoundText[currentState][j] != 0; j++) {
                                 tmpTargetBuffer[tmpTargetBufferLength++] = escSeqCompoundText[currentState][j];
                             }
                         }
                         માટે (n = (p_કિંમત_Length - 1); n >= 0; n--) {
                             tmpTargetBuffer[tmpTargetBufferLength++] = (uint8_t)(p_કિંમત >> (n * 8));
                         }
                         તોડો;
                     }
                 }
             } નહીંતર જો (tmpState == COMPOUND_TEXT_SINGLE_0) {
                 tmpTargetBuffer[tmpTargetBufferLength++] = (uint8_t)sourceChar;
             } નહીંતર {
                 p_કિંમત_Length = ucnv_MBCSFromUChar32(myConverterData->myConverterArray[currentState], sourceChar, &p_કિંમત, useFallback);
                 જો (p_કિંમત_Length > 0) {
                     માટે (n = (p_કિંમત_Length - 1); n >= 0; n--) {
                         tmpTargetBuffer[tmpTargetBufferLength++] = (uint8_t)(p_કિંમત >> (n * 8));
                     }
                 }
             }

             માટે (i = 0; i < tmpTargetBufferLength; i++) {
                 જો (target < targetLimit) {
                     *target++ = tmpTargetBuffer[i];
                 } નહીંતર {
                     *err = U_BUFFER_OVERFLOW_ERROR;
                     તોડો;
                 }
             }

             જો (*err == U_BUFFER_OVERFLOW_ERROR) {
                 માટે (; i < tmpTargetBufferLength; i++) {
                     args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = tmpTargetBuffer[i];
                 }
             }
        } નહીંતર {
            *err = U_BUFFER_OVERFLOW_ERROR;
            તોડો;
        }
    }

    /*save the state and return */
    myConverterData->state = currentState;
    args->source = source;
    args->target = (અક્ષર*)target;
}


સ્થિર શૂન્ય U_CALLCONV
UConverter_toUnicode_CompoundText_OFFSETS(UConverterToUnicodeArgs *args,
                                               UErrorCode* err){
    અચલ અક્ષર *mySource = (અક્ષર *) args->source;
    char16_t *myTarget = args->target;
    અચલ અક્ષર *mySourceLimit = args->sourceLimit;
    અચલ અક્ષર *tmpSourceLimit = mySourceLimit;
    uint32_t mySourceChar = 0x0000;
    COMPOUND_TEXT_CONVERTERS currentState, tmpState;
    int32_t sourceOffset = 0;
    UConverterDataCompoundText *myConverterData = (UConverterDataCompoundText *) args->converter->extraInfo;
    UConverterSharedData* savedSharedData = nullptr;

    UConverterToUnicodeArgs subArgs;
    int32_t minArgsSize;

    /* set up the subconverter arguments */
    જો(args->size<કદ(UConverterToUnicodeArgs)) {
        minArgsSize = args->size;
    } નહીંતર {
        minArgsSize = (int32_t)કદ(UConverterToUnicodeArgs);
    }

    uprv_memcpy(&subArgs, args, minArgsSize);
    subArgs.size = (uint16_t)minArgsSize;

    currentState = tmpState =  myConverterData->state;

    જ્યારે(mySource < mySourceLimit){
        જો(myTarget < args->targetLimit){
            જો (args->converter->toULength > 0) {
                mySourceChar = args->converter->toUBytes[0];
            } નહીંતર {
                mySourceChar = (uint8_t)*mySource;
            }

            જો (mySourceChar == ESC_START) {
                tmpState = findStateFromEscSeq(mySource, mySourceLimit, args->converter->toUBytes, args->converter->toULength, err);

                જો (*err == U_TRUNCATED_CHAR_FOUND) {
                    માટે (; mySource < mySourceLimit;) {
                        args->converter->toUBytes[args->converter->toULength++] = *mySource++;
                    }
                    *err = U_ZERO_ERROR;
                    તોડો;
                } નહીંતર જો (tmpState == INVALID) {
                    જો (args->converter->toULength == 0) {
                        mySource++; /* skip over the 0x1b byte */
                    }
                    *err = U_ILLEGAL_CHAR_FOUND;
                    તોડો;
                }

                જો (tmpState != currentState) {
                    currentState = tmpState;
                }

                sourceOffset = static_cast<int32_t>(uprv_strlen((અક્ષર*)escSeqCompoundText[currentState]) - args->converter->toULength);

                mySource += sourceOffset;

                args->converter->toULength = 0;
            }

            જો (currentState == COMPOUND_TEXT_SINGLE_0) {
                જ્યારે (mySource < mySourceLimit) {
                    જો (*mySource == ESC_START) {
                        તોડો;
                    }
                    જો (myTarget < args->targetLimit) {
                        *myTarget++ = 0x00ff&(*mySource++);
                    } નહીંતર {
                        *err = U_BUFFER_OVERFLOW_ERROR;
                        તોડો;
                    }
                }
            } નહીંતર જો (mySource < mySourceLimit){
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

                જો (U_FAILURE(*err)) {
                    જો(*err == U_BUFFER_OVERFLOW_ERROR) {
                        જો(subArgs.converter->UCharErrorBufferLength > 0) {
                            uprv_memcpy(args->converter->UCharErrorBuffer, subArgs.converter->UCharErrorBuffer,
                                        subArgs.converter->UCharErrorBufferLength);
                        }
                        args->converter->UCharErrorBufferLength=subArgs.converter->UCharErrorBufferLength;
                        subArgs.converter->UCharErrorBufferLength = 0;
                    }
                    તોડો;
                }
            }
        } નહીંતર {
            *err = U_BUFFER_OVERFLOW_ERROR;
            તોડો;
        }
    }
    myConverterData->state = currentState;
    args->target = myTarget;
    args->source = mySource;
}

સ્થિર શૂન્ય U_CALLCONV
_CompoundText_GetUnicodeSet(અચલ UConverter *cnv,
                    અચલ USetAdder *sa,
                    UConverterUnicodeSet which,
                    UErrorCode *pErrorCode) {
    UConverterDataCompoundText *myConverterData = (UConverterDataCompoundText *)cnv->extraInfo;
    int32_t i;

    માટે (i = 1; i < અંક_OF_CONVERTERS; i++) {
        ucnv_MBCSGetUnicodeSetForUnicode(myConverterData->myConverterArray[i], sa, which, pErrorCode);
    }
    sa->add(sa->set, 0x0000);
    sa->add(sa->set, 0x0009);
    sa->add(sa->set, 0x000A);
    sa->addRange(sa->set, 0x0020, 0x007F);
    sa->addRange(sa->set, 0x00A0, 0x00FF);
}
U_CDECL_END

સ્થિર અચલ UConverterImpl _CompoundTextImpl = {

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

સ્થિર અચલ UConverterStaticData _CompoundTextStaticData = {
    કદ(UConverterStaticData),
    "COMPOUND_TEXT",
    0,
    UCNV_IBM,
    UCNV_COMPOUND_TEXT,
    1,
    6,
    { 0xef, 0, 0, 0 },
    1,
    ખોટું,
    ખોટું,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};
અચલ UConverterSharedData _CompoundTextData =
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_CompoundTextStaticData, &_CompoundTextImpl);

#endif /* #if !UCONFIG_NO_LEGACY_CONVERSION */
