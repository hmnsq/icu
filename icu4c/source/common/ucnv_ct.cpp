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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "unicode/ucnv_err.h"
#अन्तर्भाव "unicode/ucnv_cb.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "ucnv_imp.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "ucnvmbcs.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cmemory.h"

प्रकारनिर्देश गणना {
    INVALID = -2,
    DO_अन्वेषणम् = -1,

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
    अङ्कः_सम्बन्धि_CONVERTERS = 20
} COMPOUND_TEXT_CONVERTERS;

#परिभाषय अन्वेषणम्_दीर्घता 12

स्थिर ध्रुव uint8_t escSeqCompoundText[अङ्कः_सम्बन्धि_CONVERTERS][5] = {
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

#परिभाषय ESC_START 0x1B

#परिभाषय isASCIIRange(codepoint) \
        ((codepoint == 0x0000) || (codepoint == 0x0009) || (codepoint == 0x000A) || \
         (codepoint >= 0x0020 && codepoint <= 0x007f) || (codepoint >= 0x00A0 && codepoint <= 0x00FF))

#परिभाषय isIBM915(codepoint) \
        ((codepoint >= 0x0401 && codepoint <= 0x045F) || (codepoint == 0x2116))

#परिभाषय isIBM916(codepoint) \
        ((codepoint >= 0x05D0 && codepoint <= 0x05EA) || (codepoint == 0x2017) || (codepoint == 0x203E))

#परिभाषय isCompoundS3(codepoint) \
        ((codepoint == 0x060C) || (codepoint == 0x061B) || (codepoint == 0x061F) || (codepoint >= 0x0621 && codepoint <= 0x063A) || \
         (codepoint >= 0x0640 && codepoint <= 0x0652) || (codepoint >= 0x0660 && codepoint <= 0x066D) || (codepoint == 0x200B) || \
         (codepoint >= 0x0FE70 && codepoint <= 0x0FE72) || (codepoint == 0x0FE74) || (codepoint >= 0x0FE76 && codepoint <= 0x0FEBE))

#परिभाषय isCompoundS2(codepoint) \
        ((codepoint == 0x02BC) || (codepoint == 0x02BD) || (codepoint >= 0x0384 && codepoint <= 0x03CE) || (codepoint == 0x2015))

#परिभाषय isIBM914(codepoint) \
        ((codepoint == 0x0100) || (codepoint == 0x0101) || (codepoint == 0x0112) || (codepoint == 0x0113) || (codepoint == 0x0116) || (codepoint == 0x0117) || \
         (codepoint == 0x0122) || (codepoint == 0x0123) || (codepoint >= 0x0128 && codepoint <= 0x012B) || (codepoint == 0x012E) || (codepoint == 0x012F) || \
         (codepoint >= 0x0136 && codepoint <= 0x0138) || (codepoint == 0x013B) || (codepoint == 0x013C) || (codepoint == 0x0145) || (codepoint ==  0x0146) || \
         (codepoint >= 0x014A && codepoint <= 0x014D) || (codepoint == 0x0156) || (codepoint == 0x0157) || (codepoint >= 0x0166 && codepoint <= 0x016B) || \
         (codepoint == 0x0172) || (codepoint == 0x0173))

#परिभाषय isIBM874(codepoint) \
        ((codepoint >= 0x0E01 && codepoint <= 0x0E3A) || (codepoint >= 0x0E3F && codepoint <= 0x0E5B))

#परिभाषय isIBM912(codepoint) \
        ((codepoint >= 0x0102 && codepoint <= 0x0107) || (codepoint >= 0x010C && codepoint <= 0x0111) || (codepoint >= 0x0118 && codepoint <= 0x011B) || \
         (codepoint == 0x0139) || (codepoint == 0x013A) || (codepoint == 0x013D) || (codepoint == 0x013E) || (codepoint >= 0x0141 && codepoint <= 0x0144) || \
         (codepoint == 0x0147) || (codepoint == 0x0147) || (codepoint == 0x0150) || (codepoint == 0x0151) || (codepoint == 0x0154) || (codepoint == 0x0155) || \
         (codepoint >= 0x0158 && codepoint <= 0x015B) || (codepoint == 0x015E) || (codepoint == 0x015F) || (codepoint >= 0x0160 && codepoint <= 0x0165) || \
         (codepoint == 0x016E) || (codepoint == 0x016F) || (codepoint == 0x0170) || (codepoint ==  0x0171) || (codepoint >= 0x0179 && codepoint <= 0x017E) || \
         (codepoint == 0x02C7) || (codepoint == 0x02D8) || (codepoint == 0x02D9) || (codepoint == 0x02DB) || (codepoint == 0x02DD))

#परिभाषय isIBM913(codepoint) \
        ((codepoint >= 0x0108 && codepoint <= 0x010B) || (codepoint == 0x011C) || \
         (codepoint == 0x011D) || (codepoint == 0x0120) || (codepoint == 0x0121) || \
         (codepoint >= 0x0124 && codepoint <= 0x0127) || (codepoint == 0x0134) || (codepoint == 0x0135) || \
         (codepoint == 0x015C) || (codepoint == 0x015D) || (codepoint == 0x016C) || (codepoint ==  0x016D))

#परिभाषय isCompoundS1(codepoint) \
        ((codepoint == 0x011E) || (codepoint == 0x011F) || (codepoint == 0x0130) || \
         (codepoint == 0x0131) || (codepoint >= 0x0218 && codepoint <= 0x021B))

#परिभाषय isISO8859_14(codepoint) \
        ((codepoint >= 0x0174 && codepoint <= 0x0177) || (codepoint == 0x1E0A) || \
         (codepoint == 0x1E0B) || (codepoint == 0x1E1E) || (codepoint == 0x1E1F) || \
         (codepoint == 0x1E40) || (codepoint == 0x1E41) || (codepoint == 0x1E56) || \
         (codepoint == 0x1E57) || (codepoint == 0x1E60) || (codepoint == 0x1E61) || \
         (codepoint == 0x1E6A) || (codepoint == 0x1E6B) || (codepoint == 0x1EF2) || \
         (codepoint == 0x1EF3) || (codepoint >= 0x1E80 && codepoint <= 0x1E85))

#परिभाषय isIBM923(codepoint) \
        ((codepoint == 0x0152) || (codepoint == 0x0153) || (codepoint == 0x0178) || (codepoint == 0x20AC))


प्रकारनिर्देश संरचना{
    UConverter_Shared_दत्त *myConverterArray[अङ्कः_सम्बन्धि_CONVERTERS];
    COMPOUND_TEXT_CONVERTERS state;
} UConverter_दत्त_Compound_Text;

/*********** Compound Text Converter Protos ***********/
U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
_CompoundTextOpen(UConverter *cnv, UConverter_आरोपय_तर्काः *p_तर्काः, UError_संहिता *error_संहिता);

स्थिर शून्य U_CALLCONV
 _CompoundTextClose(UConverter *converter);

स्थिर शून्य U_CALLCONV
_CompoundTextReset(UConverter *converter, UConverterResetChoice choice);

स्थिर ध्रुव वर्ण* U_CALLCONV
_CompoundTextgetName(ध्रुव UConverter* cnv);


स्थिर int32_t find_अग्र_Esc(ध्रुव वर्ण *source, ध्रुव वर्ण *source_सीमा) {
    int32_t length = static_cast<int32_t>(source_सीमा - source);
    int32_t i;
    प्रति (i = 1; i < length; i++) {
        यदि (*(source + i) == 0x1B) {
            प्रत्यागम i;
        }
    }

    प्रत्यागम length;
}

स्थिर COMPOUND_TEXT_CONVERTERS getState(पूर्णाङ्क codepoint) {
    COMPOUND_TEXT_CONVERTERS state = DO_अन्वेषणम्;

    यदि (isASCIIRange(codepoint)) {
        state = COMPOUND_TEXT_SINGLE_0;
    } अन्यथा यदि (isIBM912(codepoint)) {
        state = IBM_912;
    }अन्यथा यदि (isIBM913(codepoint)) {
        state = IBM_913;
    } अन्यथा यदि (isISO8859_14(codepoint)) {
        state = ISO_8859_14;
    } अन्यथा यदि (isIBM923(codepoint)) {
        state = IBM_923;
    } अन्यथा यदि (isIBM874(codepoint)) {
        state = IBM_874;
    } अन्यथा यदि (isIBM914(codepoint)) {
        state = IBM_914;
    } अन्यथा यदि (isCompoundS2(codepoint)) {
        state = COMPOUND_TEXT_SINGLE_2;
    } अन्यथा यदि (isCompoundS3(codepoint)) {
        state = COMPOUND_TEXT_SINGLE_3;
    } अन्यथा यदि (isIBM916(codepoint)) {
        state = IBM_916;
    } अन्यथा यदि (isIBM915(codepoint)) {
        state = IBM_915;
    } अन्यथा यदि (isCompoundS1(codepoint)) {
        state = COMPOUND_TEXT_SINGLE_1;
    }

    प्रत्यागम state;
}

स्थिर COMPOUND_TEXT_CONVERTERS findStateFromEscSeq(ध्रुव वर्ण* source, ध्रुव वर्ण* source_सीमा, ध्रुव uint8_t* गन्तव्यम्_UBytes_बफर, int32_t गन्तव्यम्_UBytes_बफर_दीर्घता, UError_संहिता *err) {
    COMPOUND_TEXT_CONVERTERS state = INVALID;
    UBool मेलनम्_प्राप्तम् = असत्य;
    int32_t i, n, विस्थापनम् = गन्तव्यम्_UBytes_बफर_दीर्घता;

    प्रति (i = 0; i < अङ्कः_सम्बन्धि_CONVERTERS; i++) {
        मेलनम्_प्राप्तम् = सत्य;
        प्रति (n = 0; escSeqCompoundText[i][n] != 0; n++) {
            यदि (n < गन्तव्यम्_UBytes_बफर_दीर्घता) {
                यदि (गन्तव्यम्_UBytes_बफर[n] != escSeqCompoundText[i][n]) {
                    मेलनम्_प्राप्तम् = असत्य;
                    भङ्ग;
                }
            } अन्यथा यदि ((source + (n - विस्थापनम्)) >= source_सीमा) {
                *err = U_TRUNCATED_CHAR_प्राप्तम्;
                मेलनम्_प्राप्तम् = असत्य;
                भङ्ग;
            } अन्यथा यदि (*(source + (n - विस्थापनम्)) != escSeqCompoundText[i][n]) {
                मेलनम्_प्राप्तम् = असत्य;
                भङ्ग;
            }
        }

        यदि (मेलनम्_प्राप्तम्) {
            भङ्ग;
        }
    }

    यदि (मेलनम्_प्राप्तम्) {
        state = (COMPOUND_TEXT_CONVERTERS)i;
    }

    प्रत्यागम state;
}

स्थिर शून्य U_CALLCONV
_CompoundTextOpen(UConverter *cnv, UConverter_आरोपय_तर्काः *p_तर्काः, UError_संहिता *error_संहिता){
    cnv->extraInfo = uprv_malloc (परिमाण (UConverter_दत्त_Compound_Text));
    यदि (cnv->extraInfo != शून्यसूचकः) {
        UConverter_दत्त_Compound_Text *my_Converter_दत्त = (UConverter_दत्त_Compound_Text *) cnv->extraInfo;

        UConverterNamePieces राशिः_Pieces;
        UConverter_आरोपय_तर्काः राशिः_तर्काः=UCNV_आरोपय_तर्काः_INITIALIZER;

        my_Converter_दत्त->myConverterArray[COMPOUND_TEXT_SINGLE_0] = शून्यसूचकः;
        my_Converter_दत्त->myConverterArray[COMPOUND_TEXT_SINGLE_1] = ucnv_आरोपय_Shared_दत्त("icu-internal-compound-s1", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[COMPOUND_TEXT_SINGLE_2] = ucnv_आरोपय_Shared_दत्त("icu-internal-compound-s2", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[COMPOUND_TEXT_SINGLE_3] = ucnv_आरोपय_Shared_दत्त("icu-internal-compound-s3", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[COMPOUND_TEXT_DOUBLE_1] = ucnv_आरोपय_Shared_दत्त("icu-internal-compound-d1", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[COMPOUND_TEXT_DOUBLE_2] = ucnv_आरोपय_Shared_दत्त("icu-internal-compound-d2", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[COMPOUND_TEXT_DOUBLE_3] = ucnv_आरोपय_Shared_दत्त("icu-internal-compound-d3", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[COMPOUND_TEXT_DOUBLE_4] = ucnv_आरोपय_Shared_दत्त("icu-internal-compound-d4", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[COMPOUND_TEXT_DOUBLE_5] = ucnv_आरोपय_Shared_दत्त("icu-internal-compound-d5", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[COMPOUND_TEXT_DOUBLE_6] = ucnv_आरोपय_Shared_दत्त("icu-internal-compound-d6", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[COMPOUND_TEXT_DOUBLE_7] = ucnv_आरोपय_Shared_दत्त("icu-internal-compound-d7", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[COMPOUND_TEXT_TRIPLE_DOUBLE] = ucnv_आरोपय_Shared_दत्त("icu-internal-compound-t", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);

        my_Converter_दत्त->myConverterArray[IBM_915] = ucnv_आरोपय_Shared_दत्त("ibm-915_P100-1995", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[IBM_916] = ucnv_आरोपय_Shared_दत्त("ibm-916_P100-1995", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[IBM_914] = ucnv_आरोपय_Shared_दत्त("ibm-914_P100-1995", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[IBM_874] = ucnv_आरोपय_Shared_दत्त("ibm-874_P100-1995", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[IBM_912] = ucnv_आरोपय_Shared_दत्त("ibm-912_P100-1995", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[IBM_913] = ucnv_आरोपय_Shared_दत्त("ibm-913_P100-2000", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[ISO_8859_14] = ucnv_आरोपय_Shared_दत्त("iso-8859_14-1998", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
        my_Converter_दत्त->myConverterArray[IBM_923] = ucnv_आरोपय_Shared_दत्त("ibm-923_P100-1998", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);

        यदि (U_FAILURE(*error_संहिता) || p_तर्काः->onlyTestIsLoadable) {
            _CompoundTextClose(cnv);
            प्रत्यागम;
        }

        my_Converter_दत्त->state = (COMPOUND_TEXT_CONVERTERS)0;
    } अन्यथा {
        *error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
    }
}


स्थिर शून्य U_CALLCONV
_CompoundTextClose(UConverter *converter) {
    UConverter_दत्त_Compound_Text* my_Converter_दत्त = (UConverter_दत्त_Compound_Text*)(converter->extraInfo);
    int32_t i;

    यदि (converter->extraInfo != शून्यसूचकः) {
        /*close the array of converter pointers and free the memory*/
        प्रति (i = 0; i < अङ्कः_सम्बन्धि_CONVERTERS; i++) {
            यदि (my_Converter_दत्त->myConverterArray[i] != शून्यसूचकः) {
                ucnv_unload_Shared_दत्त_If_Ready(my_Converter_दत्त->myConverterArray[i]);
            }
        }

        uprv_free(converter->extraInfo);
        converter->extraInfo = शून्यसूचकः;
    }
}

स्थिर शून्य U_CALLCONV
_CompoundTextReset(UConverter *converter, UConverterResetChoice choice) {
    (शून्य)converter;
    (शून्य)choice;
}

स्थिर ध्रुव वर्ण* U_CALLCONV
_CompoundTextgetName(ध्रुव UConverter* cnv){
    (शून्य)cnv;
    प्रत्यागम "x11-compound-text";
}

स्थिर शून्य U_CALLCONV
UConverter_fromUnicode_CompoundText_OFFSETS(UConverter_From_Unicode_तर्काः* तर्काः_2, UError_संहिता* err){
    UConverter *cnv = तर्काः_2->converter;
    uint8_t *target = (uint8_t *) तर्काः_2->target;
    ध्रुव uint8_t *target_सीमा = (ध्रुव uint8_t *) तर्काः_2->target_सीमा;
    ध्रुव char16_t* source = तर्काः_2->source;
    ध्रुव char16_t* source_सीमा = तर्काः_2->source_सीमा;
    /* int32_t* offsets = args->offsets; */
    UChar32 sourceChar;
    UBool useFallback = cnv->useFallback;
    uint8_t tmp_Target_बफर[7];
    int32_t tmp_Target_बफर_दीर्घता = 0;
    COMPOUND_TEXT_CONVERTERS currentState, tmpState;
    uint32_t p_मानम्;
    int32_t p_मानम्_दीर्घता = 0;
    int32_t i, n, j;

    UConverter_दत्त_Compound_Text *my_Converter_दत्त = (UConverter_दत्त_Compound_Text *) cnv->extraInfo;

    currentState = my_Converter_दत्त->state;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    यदि((sourceChar = cnv->fromUChar32)!=0 && target< target_सीमा) {
        गच्छ getTrail;
    }

    यावत्( source < source_सीमा){
        यदि(target < target_सीमा){

            sourceChar  = *(source++);
            /*check if the char is a First surrogate*/
             यदि(U16_IS_SURROGATE(sourceChar)) {
                यदि(U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    यदि(source < source_सीमा) {
                        /* test the following code unit */
                        char16_t trail = *source;
                        यदि(U16_IS_TRAIL(trail)) {
                            source++;
                            sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } अन्यथा {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_प्राप्तम्;
                            cnv->fromUChar32=sourceChar;
                            भङ्ग;
                        }
                    } अन्यथा {
                        /* no more input */
                        cnv->fromUChar32=sourceChar;
                        भङ्ग;
                    }
                } अन्यथा {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_प्राप्तम्;
                    cnv->fromUChar32=sourceChar;
                    भङ्ग;
                }
            }

             tmp_Target_बफर_दीर्घता = 0;
             tmpState = getState(sourceChar);

             यदि (tmpState != DO_अन्वेषणम् && currentState != tmpState) {
                 /* Get escape sequence if necessary */
                 currentState = tmpState;
                 प्रति (i = 0; escSeqCompoundText[currentState][i] != 0; i++) {
                     tmp_Target_बफर[tmp_Target_बफर_दीर्घता++] = escSeqCompoundText[currentState][i];
                 }
             }

             यदि (tmpState == DO_अन्वेषणम्) {
                 /* Test all available converters */
                 प्रति (i = 1; i < अन्वेषणम्_दीर्घता; i++) {
                     p_मानम्_दीर्घता = ucnv_MBCSFromUChar32(my_Converter_दत्त->myConverterArray[i], sourceChar, &p_मानम्, useFallback);
                     यदि (p_मानम्_दीर्घता > 0) {
                         tmpState = (COMPOUND_TEXT_CONVERTERS)i;
                         यदि (currentState != tmpState) {
                             currentState = tmpState;
                             प्रति (j = 0; escSeqCompoundText[currentState][j] != 0; j++) {
                                 tmp_Target_बफर[tmp_Target_बफर_दीर्घता++] = escSeqCompoundText[currentState][j];
                             }
                         }
                         प्रति (n = (p_मानम्_दीर्घता - 1); n >= 0; n--) {
                             tmp_Target_बफर[tmp_Target_बफर_दीर्घता++] = (uint8_t)(p_मानम् >> (n * 8));
                         }
                         भङ्ग;
                     }
                 }
             } अन्यथा यदि (tmpState == COMPOUND_TEXT_SINGLE_0) {
                 tmp_Target_बफर[tmp_Target_बफर_दीर्घता++] = (uint8_t)sourceChar;
             } अन्यथा {
                 p_मानम्_दीर्घता = ucnv_MBCSFromUChar32(my_Converter_दत्त->myConverterArray[currentState], sourceChar, &p_मानम्, useFallback);
                 यदि (p_मानम्_दीर्घता > 0) {
                     प्रति (n = (p_मानम्_दीर्घता - 1); n >= 0; n--) {
                         tmp_Target_बफर[tmp_Target_बफर_दीर्घता++] = (uint8_t)(p_मानम् >> (n * 8));
                     }
                 }
             }

             प्रति (i = 0; i < tmp_Target_बफर_दीर्घता; i++) {
                 यदि (target < target_सीमा) {
                     *target++ = tmp_Target_बफर[i];
                 } अन्यथा {
                     *err = U_बफर_अतिप्रवाहः_ERROR;
                     भङ्ग;
                 }
             }

             यदि (*err == U_बफर_अतिप्रवाहः_ERROR) {
                 प्रति (; i < tmp_Target_बफर_दीर्घता; i++) {
                     तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] = tmp_Target_बफर[i];
                 }
             }
        } अन्यथा {
            *err = U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }
    }

    /*save the state and return */
    my_Converter_दत्त->state = currentState;
    तर्काः_2->source = source;
    तर्काः_2->target = (वर्ण*)target;
}


स्थिर शून्य U_CALLCONV
UConverter_गन्तव्यम्_Unicode_Compound_Text_OFFSETS(UConverter_गन्तव्यम्_Unicode_तर्काः *तर्काः_2,
                                               UError_संहिता* err){
    ध्रुव वर्ण *mySource = (वर्ण *) तर्काः_2->source;
    char16_t *myTarget = तर्काः_2->target;
    ध्रुव वर्ण *my_Source_सीमा = तर्काः_2->source_सीमा;
    ध्रुव वर्ण *tmp_Source_सीमा = my_Source_सीमा;
    uint32_t mySourceChar = 0x0000;
    COMPOUND_TEXT_CONVERTERS currentState, tmpState;
    int32_t source_विस्थापनम् = 0;
    UConverter_दत्त_Compound_Text *my_Converter_दत्त = (UConverter_दत्त_Compound_Text *) तर्काः_2->converter->extraInfo;
    UConverter_Shared_दत्त* saved_Shared_दत्त = शून्यसूचकः;

    UConverter_गन्तव्यम्_Unicode_तर्काः sub_तर्काः;
    int32_t min_तर्काः_आकार;

    /* set up the subconverter arguments */
    यदि(तर्काः_2->size<परिमाण(UConverter_गन्तव्यम्_Unicode_तर्काः)) {
        min_तर्काः_आकार = तर्काः_2->size;
    } अन्यथा {
        min_तर्काः_आकार = (int32_t)परिमाण(UConverter_गन्तव्यम्_Unicode_तर्काः);
    }

    uprv_memcpy(&sub_तर्काः, तर्काः_2, min_तर्काः_आकार);
    sub_तर्काः.size = (uint16_t)min_तर्काः_आकार;

    currentState = tmpState =  my_Converter_दत्त->state;

    यावत्(mySource < my_Source_सीमा){
        यदि(myTarget < तर्काः_2->target_सीमा){
            यदि (तर्काः_2->converter->गन्तव्यम्_ULength > 0) {
                mySourceChar = तर्काः_2->converter->गन्तव्यम्_UBytes[0];
            } अन्यथा {
                mySourceChar = (uint8_t)*mySource;
            }

            यदि (mySourceChar == ESC_START) {
                tmpState = findStateFromEscSeq(mySource, my_Source_सीमा, तर्काः_2->converter->गन्तव्यम्_UBytes, तर्काः_2->converter->गन्तव्यम्_ULength, err);

                यदि (*err == U_TRUNCATED_CHAR_प्राप्तम्) {
                    प्रति (; mySource < my_Source_सीमा;) {
                        तर्काः_2->converter->गन्तव्यम्_UBytes[तर्काः_2->converter->गन्तव्यम्_ULength++] = *mySource++;
                    }
                    *err = U_ZERO_ERROR;
                    भङ्ग;
                } अन्यथा यदि (tmpState == INVALID) {
                    यदि (तर्काः_2->converter->गन्तव्यम्_ULength == 0) {
                        mySource++; /* skip over the 0x1b byte */
                    }
                    *err = U_ILLEGAL_CHAR_प्राप्तम्;
                    भङ्ग;
                }

                यदि (tmpState != currentState) {
                    currentState = tmpState;
                }

                source_विस्थापनम् = static_cast<int32_t>(uprv_strlen((वर्ण*)escSeqCompoundText[currentState]) - तर्काः_2->converter->गन्तव्यम्_ULength);

                mySource += source_विस्थापनम्;

                तर्काः_2->converter->गन्तव्यम्_ULength = 0;
            }

            यदि (currentState == COMPOUND_TEXT_SINGLE_0) {
                यावत् (mySource < my_Source_सीमा) {
                    यदि (*mySource == ESC_START) {
                        भङ्ग;
                    }
                    यदि (myTarget < तर्काः_2->target_सीमा) {
                        *myTarget++ = 0x00ff&(*mySource++);
                    } अन्यथा {
                        *err = U_बफर_अतिप्रवाहः_ERROR;
                        भङ्ग;
                    }
                }
            } अन्यथा यदि (mySource < my_Source_सीमा){
                source_विस्थापनम् = find_अग्र_Esc(mySource, my_Source_सीमा);

                tmp_Source_सीमा = mySource + source_विस्थापनम्;

                sub_तर्काः.source = mySource;
                sub_तर्काः.source_सीमा = tmp_Source_सीमा;
                sub_तर्काः.target = myTarget;
                saved_Shared_दत्त = sub_तर्काः.converter->shared_दत्त;
                sub_तर्काः.converter->shared_दत्त = my_Converter_दत्त->myConverterArray[currentState];

                ucnv_MBCSToUnicodeWithOffsets(&sub_तर्काः, err);

                sub_तर्काः.converter->shared_दत्त = saved_Shared_दत्त;

                mySource = sub_तर्काः.source;
                myTarget = sub_तर्काः.target;

                यदि (U_FAILURE(*err)) {
                    यदि(*err == U_बफर_अतिप्रवाहः_ERROR) {
                        यदि(sub_तर्काः.converter->UChar_Error_बफर_दीर्घता > 0) {
                            uprv_memcpy(तर्काः_2->converter->UChar_Error_बफर, sub_तर्काः.converter->UChar_Error_बफर,
                                        sub_तर्काः.converter->UChar_Error_बफर_दीर्घता);
                        }
                        तर्काः_2->converter->UChar_Error_बफर_दीर्घता=sub_तर्काः.converter->UChar_Error_बफर_दीर्घता;
                        sub_तर्काः.converter->UChar_Error_बफर_दीर्घता = 0;
                    }
                    भङ्ग;
                }
            }
        } अन्यथा {
            *err = U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }
    }
    my_Converter_दत्त->state = currentState;
    तर्काः_2->target = myTarget;
    तर्काः_2->source = mySource;
}

स्थिर शून्य U_CALLCONV
Compound_Text_Get_Unicode_समुच्चयः(ध्रुव UConverter *cnv,
                    ध्रुव USetAdder *sa,
                    UConverter_Unicode_समुच्चयः which,
                    UError_संहिता *p_Error_संहिता) {
    UConverter_दत्त_Compound_Text *my_Converter_दत्त = (UConverter_दत्त_Compound_Text *)cnv->extraInfo;
    int32_t i;

    प्रति (i = 1; i < अङ्कः_सम्बन्धि_CONVERTERS; i++) {
        ucnv_MBCSGet_Unicode_समुच्चयः_For_Unicode(my_Converter_दत्त->myConverterArray[i], sa, which, p_Error_संहिता);
    }
    sa->सङ्कलय(sa->समुच्चयः, 0x0000);
    sa->सङ्कलय(sa->समुच्चयः, 0x0009);
    sa->सङ्कलय(sa->समुच्चयः, 0x000A);
    sa->सङ्कलय_Range(sa->समुच्चयः, 0x0020, 0x007F);
    sa->सङ्कलय_Range(sa->समुच्चयः, 0x00A0, 0x00FF);
}
U_CDECL_अन्तः

स्थिर ध्रुव UConverterImpl _CompoundTextImpl = {

    UCNV_COMPOUND_TEXT,

    शून्यसूचकः,
    शून्यसूचकः,

    _CompoundTextOpen,
    _CompoundTextClose,
    _CompoundTextReset,

    UConverter_गन्तव्यम्_Unicode_Compound_Text_OFFSETS,
    UConverter_गन्तव्यम्_Unicode_Compound_Text_OFFSETS,
    UConverter_fromUnicode_CompoundText_OFFSETS,
    UConverter_fromUnicode_CompoundText_OFFSETS,
    शून्यसूचकः,

    शून्यसूचकः,
    _CompoundTextgetName,
    शून्यसूचकः,
    शून्यसूचकः,
    Compound_Text_Get_Unicode_समुच्चयः,
    शून्यसूचकः,
    शून्यसूचकः
};

स्थिर ध्रुव UConverter_Static_दत्त Compound_Text_Static_दत्त = {
    परिमाण(UConverter_Static_दत्त),
    "COMPOUND_TEXT",
    0,
    UCNV_IBM,
    UCNV_COMPOUND_TEXT,
    1,
    6,
    { 0xef, 0, 0, 0 },
    1,
    असत्य,
    असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};
ध्रुव UConverter_Shared_दत्त Compound_Text_दत्त =
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&Compound_Text_Static_दत्त, &_CompoundTextImpl);

#यदि_अन्तः /* #if !UCONFIG_NO_LEGACY_CONVERSION */
