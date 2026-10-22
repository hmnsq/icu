// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*  
**********************************************************************
*   Copyright (C) 2002-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  ucnv_u32.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002jul01
*   created by: Markus W. Scherer
*
*   UTF-32 converter implementation. Used to be in ucnv_utf.c.
*/

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/utf.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "cmemory.h"

#నిర్వచించు గరిష్ఠ_UCS2            0x0000FFFF
#నిర్వచించు గరిష్ఠ_UTF             0x0010FFFF
#నిర్వచించు HALF_జరుపు              10
#నిర్వచించు HALF_పునాది               0x0010000
#నిర్వచించు HALF_ముసుగు               0x3FF
#నిర్వచించు SURROGATE_ఎగువ_ప్రారంభం    0xD800
#నిర్వచించు SURROGATE_దిగువ_ప్రారంభం     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#నిర్వచించు SURROGATE_దిగువ_పునాది      9216

గణన {
    UCNV_NEED_గమ్యం_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
స్థిర శూన్య U_CALLCONV
T_UConverter_గమ్యం_Unicode_UTF32_BE(UConverter_గమ్యం_Unicode_వాదనలు * వాదనలు_2,
                                UError_సంకేతం * err)
{
    స్థిరరాశి గుర్తులేని అక్షరం *my_ఆధారం = (గుర్తులేని అక్షరం *) వాదనలు_2->ఆధారం;
    char16_t *my_లక్ష్యం = వాదనలు_2->లక్ష్యం;
    స్థిరరాశి గుర్తులేని అక్షరం *ఆధారం_హద్దు = (గుర్తులేని అక్షరం *) వాదనలు_2->ఆధారం_హద్దు;
    స్థిరరాశి char16_t *లక్ష్యం_హద్దు = వాదనలు_2->లక్ష్యం_హద్దు;
    గుర్తులేని అక్షరం *గమ్యం_UBytes = వాదనలు_2->converter->గమ్యం_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    ఒకవేళ (వాదనలు_2->converter->గమ్యం_ULength > 0 && my_లక్ష్యం < లక్ష్యం_హద్దు) {
        i = వాదనలు_2->converter->గమ్యం_ULength;       /* restore # of bytes consumed */
        వాదనలు_2->converter->గమ్యం_ULength = 0;

        ch = వాదనలు_2->converter->గమ్యం_Unicode_స్థితి - 1;/*Stores the previously calculated ch from a previous call*/
        వాదనలు_2->converter->గమ్యం_Unicode_స్థితి = 0;
        వెళ్ళు morebytes;
    }

    అయితే (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం < లక్ష్యం_హద్దు) {
        i = 0;
        ch = 0;
morebytes:
        అయితే (i < పరిమాణం(uint32_t)) {
            ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు) {
                ch = (ch << 8) | (uint8_t)(*my_ఆధారం);
                గమ్యం_UBytes[i++] = (అక్షరం) *(my_ఆధారం++);
            }
            లేకపోతే {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                వాదనలు_2->converter->గమ్యం_Unicode_స్థితి = ch + 1;
                వాదనలు_2->converter->గమ్యం_ULength = (int8_t) i;
                వెళ్ళు donefornow;
            }
        }

        ఒకవేళ (ch <= గరిష్ఠ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ఒకవేళ (ch <= గరిష్ఠ_UCS2) 
            {
                /* fits in 16 bits */
                *(my_లక్ష్యం++) = (char16_t) ch;
            }
            లేకపోతే {
                /* write out the surrogates */
                *(my_లక్ష్యం++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                ఒకవేళ (my_లక్ష్యం < లక్ష్యం_హద్దు) {
                    *(my_లక్ష్యం++) = (char16_t)ch;
                }
                లేకపోతే {
                    /* Put in overflow buffer (not handled here) */
                    వాదనలు_2->converter->UChar_Error_బఫరు[0] = (char16_t) ch;
                    వాదనలు_2->converter->UChar_Error_బఫరు_నిడివి = 1;
                    *err = U_బఫరు_అధికప్రవాహం_ERROR;
                    ఆపు;
                }
            }
        }
        లేకపోతే {
            వాదనలు_2->converter->గమ్యం_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_దొరికింది;
            ఆపు;
        }
    }

donefornow:
    ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం >= లక్ష్యం_హద్దు && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_బఫరు_అధికప్రవాహం_ERROR;
    }

    వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
    వాదనలు_2->ఆధారం = (స్థిరరాశి అక్షరం *) my_ఆధారం;
}

స్థిర శూన్య U_CALLCONV
T_UConverter_గమ్యం_Unicode_UTF32_BE_స్థానభ్రంశం_LOGIC(UConverter_గమ్యం_Unicode_వాదనలు * వాదనలు_2,
                                             UError_సంకేతం * err)
{
    స్థిరరాశి గుర్తులేని అక్షరం *my_ఆధారం = (గుర్తులేని అక్షరం *) వాదనలు_2->ఆధారం;
    char16_t *my_లక్ష్యం = వాదనలు_2->లక్ష్యం;
    int32_t *myOffsets = వాదనలు_2->offsets;
    స్థిరరాశి గుర్తులేని అక్షరం *ఆధారం_హద్దు = (గుర్తులేని అక్షరం *) వాదనలు_2->ఆధారం_హద్దు;
    స్థిరరాశి char16_t *లక్ష్యం_హద్దు = వాదనలు_2->లక్ష్యం_హద్దు;
    గుర్తులేని అక్షరం *గమ్యం_UBytes = వాదనలు_2->converter->గమ్యం_UBytes;
    uint32_t ch, i;
    int32_t స్థానభ్రంశం_అంకె = 0;

    /* Restore state of current sequence */
    ఒకవేళ (వాదనలు_2->converter->గమ్యం_ULength > 0 && my_లక్ష్యం < లక్ష్యం_హద్దు) {
        i = వాదనలు_2->converter->గమ్యం_ULength;       /* restore # of bytes consumed */
        వాదనలు_2->converter->గమ్యం_ULength = 0;

        ch = వాదనలు_2->converter->గమ్యం_Unicode_స్థితి - 1;/*Stores the previously calculated ch from a previous call*/
        వాదనలు_2->converter->గమ్యం_Unicode_స్థితి = 0;
        వెళ్ళు morebytes;
    }

    అయితే (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం < లక్ష్యం_హద్దు) {
        i = 0;
        ch = 0;
morebytes:
        అయితే (i < పరిమాణం(uint32_t)) {
            ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు) {
                ch = (ch << 8) | (uint8_t)(*my_ఆధారం);
                గమ్యం_UBytes[i++] = (అక్షరం) *(my_ఆధారం++);
            }
            లేకపోతే {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                వాదనలు_2->converter->గమ్యం_Unicode_స్థితి = ch + 1;
                వాదనలు_2->converter->గమ్యం_ULength = (int8_t) i;
                వెళ్ళు donefornow;
            }
        }

        ఒకవేళ (ch <= గరిష్ఠ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ఒకవేళ (ch <= గరిష్ఠ_UCS2) {
                /* fits in 16 bits */
                *(my_లక్ష్యం++) = (char16_t) ch;
                *(myOffsets++) = స్థానభ్రంశం_అంకె;
            }
            లేకపోతే {
                /* write out the surrogates */
                *(my_లక్ష్యం++) = U16_LEAD(ch);
                *myOffsets++ = స్థానభ్రంశం_అంకె;
                ch = U16_TRAIL(ch);
                ఒకవేళ (my_లక్ష్యం < లక్ష్యం_హద్దు)
                {
                    *(my_లక్ష్యం++) = (char16_t)ch;
                    *(myOffsets++) = స్థానభ్రంశం_అంకె;
                }
                లేకపోతే {
                    /* Put in overflow buffer (not handled here) */
                    వాదనలు_2->converter->UChar_Error_బఫరు[0] = (char16_t) ch;
                    వాదనలు_2->converter->UChar_Error_బఫరు_నిడివి = 1;
                    *err = U_బఫరు_అధికప్రవాహం_ERROR;
                    ఆపు;
                }
            }
        }
        లేకపోతే {
            వాదనలు_2->converter->గమ్యం_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_దొరికింది;
            ఆపు;
        }
        స్థానభ్రంశం_అంకె += i;
    }

donefornow:
    ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం >= లక్ష్యం_హద్దు && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_బఫరు_అధికప్రవాహం_ERROR;
    }

    వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
    వాదనలు_2->ఆధారం = (స్థిరరాశి అక్షరం *) my_ఆధారం;
    వాదనలు_2->offsets = myOffsets;
}

స్థిర శూన్య U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverter_From_Unicode_వాదనలు * వాదనలు_2,
                                  UError_సంకేతం * err)
{
    స్థిరరాశి char16_t *my_ఆధారం = వాదనలు_2->ఆధారం;
    గుర్తులేని అక్షరం *my_లక్ష్యం;
    స్థిరరాశి char16_t *ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    స్థిరరాశి గుర్తులేని అక్షరం *లక్ష్యం_హద్దు = (గుర్తులేని అక్షరం *) వాదనలు_2->లక్ష్యం_హద్దు;
    UChar32 ch, ch2;
    గుర్తులేని పూర్ణాంకం సూచీ_గమ్యం_Write;
    గుర్తులేని అక్షరం తాత్కాలిక[పరిమాణం(uint32_t)];

    ఒకవేళ(my_ఆధారం >= ఆధారం_హద్దు) {
        /* no input, nothing to do */
        తిరిగి;
    }

    /* write the BOM if necessary */
    ఒకవేళ(వాదనలు_2->converter->from_Unicode_స్థితి==UCNV_NEED_గమ్యం_WRITE_BOM) {
        స్థిర స్థిరరాశి అక్షరం bom[]={ 0, 0, (అక్షరం)0xfeu, (అక్షరం)0xffu };
        ucnv_from_UWrite_బైట్లు(వాదనలు_2->converter,
                             bom, 4,
                             &వాదనలు_2->లక్ష్యం, వాదనలు_2->లక్ష్యం_హద్దు,
                             &వాదనలు_2->offsets, -1,
                             err);
        వాదనలు_2->converter->from_Unicode_స్థితి=0;
    }

    my_లక్ష్యం = (గుర్తులేని అక్షరం *) వాదనలు_2->లక్ష్యం;
    తాత్కాలిక[0] = 0;

    ఒకవేళ (వాదనలు_2->converter->fromUChar32) {
        ch = వాదనలు_2->converter->fromUChar32;
        వాదనలు_2->converter->fromUChar32 = 0;
        వెళ్ళు lowsurogate;
    }

    అయితే (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం < లక్ష్యం_హద్దు) {
        ch = *(my_ఆధారం++);

        ఒకవేళ (U_IS_SURROGATE(ch)) {
            ఒకవేళ (U_IS_LEAD(ch)) {
lowsurogate:
                ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు) {
                    ch2 = *my_ఆధారం;
                    ఒకవేళ (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ఎగువ_ప్రారంభం) << HALF_జరుపు) + ch2 + SURROGATE_దిగువ_పునాది;
                        my_ఆధారం++;
                    }
                    లేకపోతే {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        వాదనలు_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_దొరికింది;
                        ఆపు;
                    }
                }
                లేకపోతే {
                    /* ran out of source */
                    వాదనలు_2->converter->fromUChar32 = ch;
                    ఒకవేళ (వాదనలు_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_దొరికింది;
                    }
                    ఆపు;
                }
            }
            లేకపోతే {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                వాదనలు_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_దొరికింది;
                ఆపు;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        తాత్కాలిక[1] = (uint8_t) (ch >> 16 & 0x1F);
        తాత్కాలిక[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        తాత్కాలిక[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        కొరకు (సూచీ_గమ్యం_Write = 0; సూచీ_గమ్యం_Write <= పరిమాణం(uint32_t) - 1; సూచీ_గమ్యం_Write++) {
            ఒకవేళ (my_లక్ష్యం < లక్ష్యం_హద్దు) {
                *(my_లక్ష్యం++) = తాత్కాలిక[సూచీ_గమ్యం_Write];
            }
            లేకపోతే {
                వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] = తాత్కాలిక[సూచీ_గమ్యం_Write];
                *err = U_బఫరు_అధికప్రవాహం_ERROR;
            }
        }
    }

    ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం >= లక్ష్యం_హద్దు && U_SUCCESS(*err)) {
        *err = U_బఫరు_అధికప్రవాహం_ERROR;
    }

    వాదనలు_2->లక్ష్యం = (అక్షరం *) my_లక్ష్యం;
    వాదనలు_2->ఆధారం = my_ఆధారం;
}

స్థిర శూన్య U_CALLCONV
T_UConverter_from_Unicode_UTF32_BE_స్థానభ్రంశం_LOGIC(UConverter_From_Unicode_వాదనలు * వాదనలు_2,
                                               UError_సంకేతం * err)
{
    స్థిరరాశి char16_t *my_ఆధారం = వాదనలు_2->ఆధారం;
    గుర్తులేని అక్షరం *my_లక్ష్యం;
    int32_t *myOffsets;
    స్థిరరాశి char16_t *ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    స్థిరరాశి గుర్తులేని అక్షరం *లక్ష్యం_హద్దు = (గుర్తులేని అక్షరం *) వాదనలు_2->లక్ష్యం_హద్దు;
    UChar32 ch, ch2;
    int32_t స్థానభ్రంశం_అంకె = 0;
    గుర్తులేని పూర్ణాంకం సూచీ_గమ్యం_Write;
    గుర్తులేని అక్షరం తాత్కాలిక[పరిమాణం(uint32_t)];

    ఒకవేళ(my_ఆధారం >= ఆధారం_హద్దు) {
        /* no input, nothing to do */
        తిరిగి;
    }

    /* write the BOM if necessary */
    ఒకవేళ(వాదనలు_2->converter->from_Unicode_స్థితి==UCNV_NEED_గమ్యం_WRITE_BOM) {
        స్థిర స్థిరరాశి అక్షరం bom[]={ 0, 0, (అక్షరం)0xfeu, (అక్షరం)0xffu };
        ucnv_from_UWrite_బైట్లు(వాదనలు_2->converter,
                             bom, 4,
                             &వాదనలు_2->లక్ష్యం, వాదనలు_2->లక్ష్యం_హద్దు,
                             &వాదనలు_2->offsets, -1,
                             err);
        వాదనలు_2->converter->from_Unicode_స్థితి=0;
    }

    my_లక్ష్యం = (గుర్తులేని అక్షరం *) వాదనలు_2->లక్ష్యం;
    myOffsets = వాదనలు_2->offsets;
    తాత్కాలిక[0] = 0;

    ఒకవేళ (వాదనలు_2->converter->fromUChar32) {
        ch = వాదనలు_2->converter->fromUChar32;
        వాదనలు_2->converter->fromUChar32 = 0;
        వెళ్ళు lowsurogate;
    }

    అయితే (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం < లక్ష్యం_హద్దు) {
        ch = *(my_ఆధారం++);

        ఒకవేళ (U_IS_SURROGATE(ch)) {
            ఒకవేళ (U_IS_LEAD(ch)) {
lowsurogate:
                ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు) {
                    ch2 = *my_ఆధారం;
                    ఒకవేళ (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ఎగువ_ప్రారంభం) << HALF_జరుపు) + ch2 + SURROGATE_దిగువ_పునాది;
                        my_ఆధారం++;
                    }
                    లేకపోతే {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        వాదనలు_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_దొరికింది;
                        ఆపు;
                    }
                }
                లేకపోతే {
                    /* ran out of source */
                    వాదనలు_2->converter->fromUChar32 = ch;
                    ఒకవేళ (వాదనలు_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_దొరికింది;
                    }
                    ఆపు;
                }
            }
            లేకపోతే {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                వాదనలు_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_దొరికింది;
                ఆపు;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        తాత్కాలిక[1] = (uint8_t) (ch >> 16 & 0x1F);
        తాత్కాలిక[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        తాత్కాలిక[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        కొరకు (సూచీ_గమ్యం_Write = 0; సూచీ_గమ్యం_Write <= పరిమాణం(uint32_t) - 1; సూచీ_గమ్యం_Write++) {
            ఒకవేళ (my_లక్ష్యం < లక్ష్యం_హద్దు) {
                *(my_లక్ష్యం++) = తాత్కాలిక[సూచీ_గమ్యం_Write];
                *(myOffsets++) = స్థానభ్రంశం_అంకె;
            }
            లేకపోతే {
                వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] = తాత్కాలిక[సూచీ_గమ్యం_Write];
                *err = U_బఫరు_అధికప్రవాహం_ERROR;
            }
        }
        స్థానభ్రంశం_అంకె = స్థానభ్రంశం_అంకె + 1 + (తాత్కాలిక[1] != 0);
    }

    ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం >= లక్ష్యం_హద్దు && U_SUCCESS(*err)) {
        *err = U_బఫరు_అధికప్రవాహం_ERROR;
    }

    వాదనలు_2->లక్ష్యం = (అక్షరం *) my_లక్ష్యం;
    వాదనలు_2->ఆధారం = my_ఆధారం;
    వాదనలు_2->offsets = myOffsets;
}

స్థిర UChar32 U_CALLCONV
T_UConverter_get_తదుపరి_UChar_UTF32_BE(UConverter_గమ్యం_Unicode_వాదనలు* వాదనలు_2,
                                   UError_సంకేతం* err)
{
    స్థిరరాశి uint8_t *my_ఆధారం;
    UChar32 myUChar;
    int32_t length;

    my_ఆధారం = (స్థిరరాశి uint8_t *)వాదనలు_2->ఆధారం;
    ఒకవేళ (my_ఆధారం >= (స్థిరరాశి uint8_t *)వాదనలు_2->ఆధారం_హద్దు)
    {
        /* no input */
        *err = U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0xffff;
    }

    length = (int32_t)((స్థిరరాశి uint8_t *)వాదనలు_2->ఆధారం_హద్దు - my_ఆధారం);
    ఒకవేళ (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(వాదనలు_2->converter->గమ్యం_UBytes, my_ఆధారం, length);
        వాదనలు_2->converter->గమ్యం_ULength = (int8_t)length;
        వాదనలు_2->ఆధారం = (స్థిరరాశి అక్షరం *)(my_ఆధారం + length);
        *err = U_TRUNCATED_CHAR_దొరికింది;
        తిరిగి 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)my_ఆధారం[0] << 24)
            | ((UChar32)my_ఆధారం[1] << 16)
            | ((UChar32)my_ఆధారం[2] << 8)
            | ((UChar32)my_ఆధారం[3]);

    వాదనలు_2->ఆధారం = (స్థిరరాశి అక్షరం *)(my_ఆధారం + 4);
    ఒకవేళ ((uint32_t)myUChar <= గరిష్ఠ_UTF && !U_IS_SURROGATE(myUChar)) {
        తిరిగి myUChar;
    }

    uprv_memcpy(వాదనలు_2->converter->గమ్యం_UBytes, my_ఆధారం, 4);
    వాదనలు_2->converter->గమ్యం_ULength = 4;

    *err = U_ILLEGAL_CHAR_దొరికింది;
    తిరిగి 0xffff;
}
U_CDECL_చివర
స్థిర స్థిరరాశి UConverterImpl _UTF32BEImpl = {
    UCNV_UTF32_BigEndian,

    శూన్యసూచిక,
    శూన్యసూచిక,

    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,

    T_UConverter_గమ్యం_Unicode_UTF32_BE,
    T_UConverter_గమ్యం_Unicode_UTF32_BE_స్థానభ్రంశం_LOGIC,
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_స్థానభ్రంశం_LOGIC,
    T_UConverter_get_తదుపరి_UChar_UTF32_BE,

    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
    ucnv_get_Non_Surrogate_Unicode_సమితి,

    శూన్యసూచిక,
    శూన్యసూచిక
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం UTF32_BEStatic_దత్తాంశం = {
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, అబద్ధం, అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

స్థిరరాశి UConverter_Shared_దత్తాంశం _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&UTF32_BEStatic_దత్తాంశం, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
స్థిర శూన్య U_CALLCONV
T_UConverter_గమ్యం_Unicode_UTF32_LE(UConverter_గమ్యం_Unicode_వాదనలు * వాదనలు_2,
                                UError_సంకేతం * err)
{
    స్థిరరాశి గుర్తులేని అక్షరం *my_ఆధారం = (గుర్తులేని అక్షరం *) వాదనలు_2->ఆధారం;
    char16_t *my_లక్ష్యం = వాదనలు_2->లక్ష్యం;
    స్థిరరాశి గుర్తులేని అక్షరం *ఆధారం_హద్దు = (గుర్తులేని అక్షరం *) వాదనలు_2->ఆధారం_హద్దు;
    స్థిరరాశి char16_t *లక్ష్యం_హద్దు = వాదనలు_2->లక్ష్యం_హద్దు;
    గుర్తులేని అక్షరం *గమ్యం_UBytes = వాదనలు_2->converter->గమ్యం_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    ఒకవేళ (వాదనలు_2->converter->గమ్యం_ULength > 0 && my_లక్ష్యం < లక్ష్యం_హద్దు)
    {
        i = వాదనలు_2->converter->గమ్యం_ULength;       /* restore # of bytes consumed */
        వాదనలు_2->converter->గమ్యం_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = వాదనలు_2->converter->గమ్యం_Unicode_స్థితి - 1;
        వాదనలు_2->converter->గమ్యం_Unicode_స్థితి = 0;
        వెళ్ళు morebytes;
    }

    అయితే (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం < లక్ష్యం_హద్దు)
    {
        i = 0;
        ch = 0;
morebytes:
        అయితే (i < పరిమాణం(uint32_t))
        {
            ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు)
            {
                ch |= ((uint8_t)(*my_ఆధారం)) << (i * 8);
                గమ్యం_UBytes[i++] = (అక్షరం) *(my_ఆధారం++);
            }
            లేకపోతే
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                వాదనలు_2->converter->గమ్యం_Unicode_స్థితి = ch + 1;
                వాదనలు_2->converter->గమ్యం_ULength = (int8_t) i;
                వెళ్ళు donefornow;
            }
        }

        ఒకవేళ (ch <= గరిష్ఠ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ఒకవేళ (ch <= గరిష్ఠ_UCS2) {
                /* fits in 16 bits */
                *(my_లక్ష్యం++) = (char16_t) ch;
            }
            లేకపోతే {
                /* write out the surrogates */
                *(my_లక్ష్యం++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                ఒకవేళ (my_లక్ష్యం < లక్ష్యం_హద్దు) {
                    *(my_లక్ష్యం++) = (char16_t)ch;
                }
                లేకపోతే {
                    /* Put in overflow buffer (not handled here) */
                    వాదనలు_2->converter->UChar_Error_బఫరు[0] = (char16_t) ch;
                    వాదనలు_2->converter->UChar_Error_బఫరు_నిడివి = 1;
                    *err = U_బఫరు_అధికప్రవాహం_ERROR;
                    ఆపు;
                }
            }
        }
        లేకపోతే {
            వాదనలు_2->converter->గమ్యం_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_దొరికింది;
            ఆపు;
        }
    }

donefornow:
    ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం >= లక్ష్యం_హద్దు && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_బఫరు_అధికప్రవాహం_ERROR;
    }

    వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
    వాదనలు_2->ఆధారం = (స్థిరరాశి అక్షరం *) my_ఆధారం;
}

స్థిర శూన్య U_CALLCONV
T_UConverter_గమ్యం_Unicode_UTF32_LE_స్థానభ్రంశం_LOGIC(UConverter_గమ్యం_Unicode_వాదనలు * వాదనలు_2,
                                             UError_సంకేతం * err)
{
    స్థిరరాశి గుర్తులేని అక్షరం *my_ఆధారం = (గుర్తులేని అక్షరం *) వాదనలు_2->ఆధారం;
    char16_t *my_లక్ష్యం = వాదనలు_2->లక్ష్యం;
    int32_t *myOffsets = వాదనలు_2->offsets;
    స్థిరరాశి గుర్తులేని అక్షరం *ఆధారం_హద్దు = (గుర్తులేని అక్షరం *) వాదనలు_2->ఆధారం_హద్దు;
    స్థిరరాశి char16_t *లక్ష్యం_హద్దు = వాదనలు_2->లక్ష్యం_హద్దు;
    గుర్తులేని అక్షరం *గమ్యం_UBytes = వాదనలు_2->converter->గమ్యం_UBytes;
    uint32_t ch, i;
    int32_t స్థానభ్రంశం_అంకె = 0;

    /* Restore state of current sequence */
    ఒకవేళ (వాదనలు_2->converter->గమ్యం_ULength > 0 && my_లక్ష్యం < లక్ష్యం_హద్దు)
    {
        i = వాదనలు_2->converter->గమ్యం_ULength;       /* restore # of bytes consumed */
        వాదనలు_2->converter->గమ్యం_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = వాదనలు_2->converter->గమ్యం_Unicode_స్థితి - 1;
        వాదనలు_2->converter->గమ్యం_Unicode_స్థితి = 0;
        వెళ్ళు morebytes;
    }

    అయితే (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం < లక్ష్యం_హద్దు)
    {
        i = 0;
        ch = 0;
morebytes:
        అయితే (i < పరిమాణం(uint32_t))
        {
            ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు)
            {
                ch |= ((uint8_t)(*my_ఆధారం)) << (i * 8);
                గమ్యం_UBytes[i++] = (అక్షరం) *(my_ఆధారం++);
            }
            లేకపోతే
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                వాదనలు_2->converter->గమ్యం_Unicode_స్థితి = ch + 1;
                వాదనలు_2->converter->గమ్యం_ULength = (int8_t) i;
                వెళ్ళు donefornow;
            }
        }

        ఒకవేళ (ch <= గరిష్ఠ_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ఒకవేళ (ch <= గరిష్ఠ_UCS2) 
            {
                /* fits in 16 bits */
                *(my_లక్ష్యం++) = (char16_t) ch;
                *(myOffsets++) = స్థానభ్రంశం_అంకె;
            }
            లేకపోతే {
                /* write out the surrogates */
                *(my_లక్ష్యం++) = U16_LEAD(ch);
                *(myOffsets++) = స్థానభ్రంశం_అంకె;
                ch = U16_TRAIL(ch);
                ఒకవేళ (my_లక్ష్యం < లక్ష్యం_హద్దు)
                {
                    *(my_లక్ష్యం++) = (char16_t)ch;
                    *(myOffsets++) = స్థానభ్రంశం_అంకె;
                }
                లేకపోతే
                {
                    /* Put in overflow buffer (not handled here) */
                    వాదనలు_2->converter->UChar_Error_బఫరు[0] = (char16_t) ch;
                    వాదనలు_2->converter->UChar_Error_బఫరు_నిడివి = 1;
                    *err = U_బఫరు_అధికప్రవాహం_ERROR;
                    ఆపు;
                }
            }
        }
        లేకపోతే
        {
            వాదనలు_2->converter->గమ్యం_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_దొరికింది;
            ఆపు;
        }
        స్థానభ్రంశం_అంకె += i;
    }

donefornow:
    ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం >= లక్ష్యం_హద్దు && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_బఫరు_అధికప్రవాహం_ERROR;
    }

    వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
    వాదనలు_2->ఆధారం = (స్థిరరాశి అక్షరం *) my_ఆధారం;
    వాదనలు_2->offsets = myOffsets;
}

స్థిర శూన్య U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverter_From_Unicode_వాదనలు * వాదనలు_2,
                                  UError_సంకేతం * err)
{
    స్థిరరాశి char16_t *my_ఆధారం = వాదనలు_2->ఆధారం;
    గుర్తులేని అక్షరం *my_లక్ష్యం;
    స్థిరరాశి char16_t *ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    స్థిరరాశి గుర్తులేని అక్షరం *లక్ష్యం_హద్దు = (గుర్తులేని అక్షరం *) వాదనలు_2->లక్ష్యం_హద్దు;
    UChar32 ch, ch2;
    గుర్తులేని పూర్ణాంకం సూచీ_గమ్యం_Write;
    గుర్తులేని అక్షరం తాత్కాలిక[పరిమాణం(uint32_t)];

    ఒకవేళ(my_ఆధారం >= ఆధారం_హద్దు) {
        /* no input, nothing to do */
        తిరిగి;
    }

    /* write the BOM if necessary */
    ఒకవేళ(వాదనలు_2->converter->from_Unicode_స్థితి==UCNV_NEED_గమ్యం_WRITE_BOM) {
        స్థిర స్థిరరాశి అక్షరం bom[]={ (అక్షరం)0xffu, (అక్షరం)0xfeu, 0, 0 };
        ucnv_from_UWrite_బైట్లు(వాదనలు_2->converter,
                             bom, 4,
                             &వాదనలు_2->లక్ష్యం, వాదనలు_2->లక్ష్యం_హద్దు,
                             &వాదనలు_2->offsets, -1,
                             err);
        వాదనలు_2->converter->from_Unicode_స్థితి=0;
    }

    my_లక్ష్యం = (గుర్తులేని అక్షరం *) వాదనలు_2->లక్ష్యం;
    తాత్కాలిక[3] = 0;

    ఒకవేళ (వాదనలు_2->converter->fromUChar32)
    {
        ch = వాదనలు_2->converter->fromUChar32;
        వాదనలు_2->converter->fromUChar32 = 0;
        వెళ్ళు lowsurogate;
    }

    అయితే (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం < లక్ష్యం_హద్దు)
    {
        ch = *(my_ఆధారం++);

        ఒకవేళ (U16_IS_SURROGATE(ch)) {
            ఒకవేళ (U16_IS_LEAD(ch))
            {
lowsurogate:
                ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు)
                {
                    ch2 = *my_ఆధారం;
                    ఒకవేళ (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ఎగువ_ప్రారంభం) << HALF_జరుపు) + ch2 + SURROGATE_దిగువ_పునాది;
                        my_ఆధారం++;
                    }
                    లేకపోతే {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        వాదనలు_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_దొరికింది;
                        ఆపు;
                    }
                }
                లేకపోతే {
                    /* ran out of source */
                    వాదనలు_2->converter->fromUChar32 = ch;
                    ఒకవేళ (వాదనలు_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_దొరికింది;
                    }
                    ఆపు;
                }
            }
            లేకపోతే {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                వాదనలు_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_దొరికింది;
                ఆపు;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        తాత్కాలిక[2] = (uint8_t) (ch >> 16 & 0x1F);
        తాత్కాలిక[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        తాత్కాలిక[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        కొరకు (సూచీ_గమ్యం_Write = 0; సూచీ_గమ్యం_Write <= పరిమాణం(uint32_t) - 1; సూచీ_గమ్యం_Write++)
        {
            ఒకవేళ (my_లక్ష్యం < లక్ష్యం_హద్దు)
            {
                *(my_లక్ష్యం++) = తాత్కాలిక[సూచీ_గమ్యం_Write];
            }
            లేకపోతే
            {
                వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] = తాత్కాలిక[సూచీ_గమ్యం_Write];
                *err = U_బఫరు_అధికప్రవాహం_ERROR;
            }
        }
    }

    ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం >= లక్ష్యం_హద్దు && U_SUCCESS(*err))
    {
        *err = U_బఫరు_అధికప్రవాహం_ERROR;
    }

    వాదనలు_2->లక్ష్యం = (అక్షరం *) my_లక్ష్యం;
    వాదనలు_2->ఆధారం = my_ఆధారం;
}

స్థిర శూన్య U_CALLCONV
T_UConverter_from_Unicode_UTF32_LE_స్థానభ్రంశం_LOGIC(UConverter_From_Unicode_వాదనలు * వాదనలు_2,
                                               UError_సంకేతం * err)
{
    స్థిరరాశి char16_t *my_ఆధారం = వాదనలు_2->ఆధారం;
    గుర్తులేని అక్షరం *my_లక్ష్యం;
    int32_t *myOffsets;
    స్థిరరాశి char16_t *ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    స్థిరరాశి గుర్తులేని అక్షరం *లక్ష్యం_హద్దు = (గుర్తులేని అక్షరం *) వాదనలు_2->లక్ష్యం_హద్దు;
    UChar32 ch, ch2;
    గుర్తులేని పూర్ణాంకం సూచీ_గమ్యం_Write;
    గుర్తులేని అక్షరం తాత్కాలిక[పరిమాణం(uint32_t)];
    int32_t స్థానభ్రంశం_అంకె = 0;

    ఒకవేళ(my_ఆధారం >= ఆధారం_హద్దు) {
        /* no input, nothing to do */
        తిరిగి;
    }

    /* write the BOM if necessary */
    ఒకవేళ(వాదనలు_2->converter->from_Unicode_స్థితి==UCNV_NEED_గమ్యం_WRITE_BOM) {
        స్థిర స్థిరరాశి అక్షరం bom[]={ (అక్షరం)0xffu, (అక్షరం)0xfeu, 0, 0 };
        ucnv_from_UWrite_బైట్లు(వాదనలు_2->converter,
                             bom, 4,
                             &వాదనలు_2->లక్ష్యం, వాదనలు_2->లక్ష్యం_హద్దు,
                             &వాదనలు_2->offsets, -1,
                             err);
        వాదనలు_2->converter->from_Unicode_స్థితి=0;
    }

    my_లక్ష్యం = (గుర్తులేని అక్షరం *) వాదనలు_2->లక్ష్యం;
    myOffsets = వాదనలు_2->offsets;
    తాత్కాలిక[3] = 0;

    ఒకవేళ (వాదనలు_2->converter->fromUChar32)
    {
        ch = వాదనలు_2->converter->fromUChar32;
        వాదనలు_2->converter->fromUChar32 = 0;
        వెళ్ళు lowsurogate;
    }

    అయితే (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం < లక్ష్యం_హద్దు)
    {
        ch = *(my_ఆధారం++);

        ఒకవేళ (U16_IS_SURROGATE(ch)) {
            ఒకవేళ (U16_IS_LEAD(ch))
            {
lowsurogate:
                ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు)
                {
                    ch2 = *my_ఆధారం;
                    ఒకవేళ (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_ఎగువ_ప్రారంభం) << HALF_జరుపు) + ch2 + SURROGATE_దిగువ_పునాది;
                        my_ఆధారం++;
                    }
                    లేకపోతే {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        వాదనలు_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_దొరికింది;
                        ఆపు;
                    }
                }
                లేకపోతే {
                    /* ran out of source */
                    వాదనలు_2->converter->fromUChar32 = ch;
                    ఒకవేళ (వాదనలు_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_దొరికింది;
                    }
                    ఆపు;
                }
            }
            లేకపోతే {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                వాదనలు_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_దొరికింది;
                ఆపు;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        తాత్కాలిక[2] = (uint8_t) (ch >> 16 & 0x1F);
        తాత్కాలిక[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        తాత్కాలిక[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        కొరకు (సూచీ_గమ్యం_Write = 0; సూచీ_గమ్యం_Write <= పరిమాణం(uint32_t) - 1; సూచీ_గమ్యం_Write++)
        {
            ఒకవేళ (my_లక్ష్యం < లక్ష్యం_హద్దు)
            {
                *(my_లక్ష్యం++) = తాత్కాలిక[సూచీ_గమ్యం_Write];
                *(myOffsets++) = స్థానభ్రంశం_అంకె;
            }
            లేకపోతే
            {
                వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] = తాత్కాలిక[సూచీ_గమ్యం_Write];
                *err = U_బఫరు_అధికప్రవాహం_ERROR;
            }
        }
        స్థానభ్రంశం_అంకె = స్థానభ్రంశం_అంకె + 1 + (తాత్కాలిక[2] != 0);
    }

    ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం >= లక్ష్యం_హద్దు && U_SUCCESS(*err))
    {
        *err = U_బఫరు_అధికప్రవాహం_ERROR;
    }

    వాదనలు_2->లక్ష్యం = (అక్షరం *) my_లక్ష్యం;
    వాదనలు_2->ఆధారం = my_ఆధారం;
    వాదనలు_2->offsets = myOffsets;
}

స్థిర UChar32 U_CALLCONV
T_UConverter_get_తదుపరి_UChar_UTF32_LE(UConverter_గమ్యం_Unicode_వాదనలు* వాదనలు_2,
                                   UError_సంకేతం* err)
{
    స్థిరరాశి uint8_t *my_ఆధారం;
    UChar32 myUChar;
    int32_t length;

    my_ఆధారం = (స్థిరరాశి uint8_t *)వాదనలు_2->ఆధారం;
    ఒకవేళ (my_ఆధారం >= (స్థిరరాశి uint8_t *)వాదనలు_2->ఆధారం_హద్దు)
    {
        /* no input */
        *err = U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0xffff;
    }

    length = (int32_t)((స్థిరరాశి uint8_t *)వాదనలు_2->ఆధారం_హద్దు - my_ఆధారం);
    ఒకవేళ (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(వాదనలు_2->converter->గమ్యం_UBytes, my_ఆధారం, length);
        వాదనలు_2->converter->గమ్యం_ULength = (int8_t)length;
        వాదనలు_2->ఆధారం = (స్థిరరాశి అక్షరం *)(my_ఆధారం + length);
        *err = U_TRUNCATED_CHAR_దొరికింది;
        తిరిగి 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)my_ఆధారం[3] << 24)
            | ((UChar32)my_ఆధారం[2] << 16)
            | ((UChar32)my_ఆధారం[1] << 8)
            | ((UChar32)my_ఆధారం[0]);

    వాదనలు_2->ఆధారం = (స్థిరరాశి అక్షరం *)(my_ఆధారం + 4);
    ఒకవేళ ((uint32_t)myUChar <= గరిష్ఠ_UTF && !U_IS_SURROGATE(myUChar)) {
        తిరిగి myUChar;
    }

    uprv_memcpy(వాదనలు_2->converter->గమ్యం_UBytes, my_ఆధారం, 4);
    వాదనలు_2->converter->గమ్యం_ULength = 4;

    *err = U_ILLEGAL_CHAR_దొరికింది;
    తిరిగి 0xffff;
}
U_CDECL_చివర
స్థిర స్థిరరాశి UConverterImpl _UTF32LEImpl = {
    UCNV_UTF32_LittleEndian,

    శూన్యసూచిక,
    శూన్యసూచిక,

    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,

    T_UConverter_గమ్యం_Unicode_UTF32_LE,
    T_UConverter_గమ్యం_Unicode_UTF32_LE_స్థానభ్రంశం_LOGIC,
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_స్థానభ్రంశం_LOGIC,
    T_UConverter_get_తదుపరి_UChar_UTF32_LE,

    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
    ucnv_get_Non_Surrogate_Unicode_సమితి,

    శూన్యసూచిక,
    శూన్యసూచిక
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం UTF32_LEStatic_దత్తాంశం = {
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, అబద్ధం, అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


స్థిరరాశి UConverter_Shared_దత్తాంశం _UTF32LEData =
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&UTF32_LEStatic_దత్తాంశం, &_UTF32LEImpl);

/* UTF-32 (Detect BOM) ------------------------------------------------------ */

/*
 * Detect a BOM at the beginning of the stream and select UTF-32BE or UTF-32LE
 * accordingly.
 *
 * State values:
 * 0    initial state
 * 1    saw 00
 * 2    saw 00 00
 * 3    saw 00 00 FE
 * 4    -
 * 5    saw FF
 * 6    saw FF FE
 * 7    saw FF FE 00
 * 8    UTF-32BE mode
 * 9    UTF-32LE mode
 *
 * During detection: state&3==number of matching bytes so far.
 *
 * On output, emit U+FEFF as the first code point.
 */
U_CDECL_BEGIN
స్థిర శూన్య U_CALLCONV
_UTF32Reset(UConverter *cnv, UConverterResetChoice choice) {
    ఒకవేళ(choice<=UCNV_RESET_గమ్యం_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->బహుళకం=0;
    }
    ఒకవేళ(choice!=UCNV_RESET_గమ్యం_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->from_Unicode_స్థితి=UCNV_NEED_గమ్యం_WRITE_BOM;
    }
}

స్థిర శూన్య U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverter_ఎక్కించు_వాదనలు *p_వాదనలు,
           UError_సంకేతం *p_Error_సంకేతం) {
    (శూన్య)p_వాదనలు;
    (శూన్య)p_Error_సంకేతం;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

స్థిర స్థిరరాశి అక్షరం utf32BOM[8]={ 0, 0, (అక్షరం)0xfeu, (అక్షరం)0xffu, (అక్షరం)0xffu, (అక్షరం)0xfeu, 0, 0 };

స్థిర శూన్య U_CALLCONV
UTF32_గమ్యం_Unicode_With_Offsets(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                           UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv=p_వాదనలు->converter;
    స్థిరరాశి అక్షరం *ఆధారం=p_వాదనలు->ఆధారం;
    స్థిరరాశి అక్షరం *ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
    int32_t *offsets=p_వాదనలు->offsets;

    int32_t state, స్థానభ్రంశం_Delta;
    అక్షరం b;

    state=cnv->బహుళకం;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    స్థానభ్రంశం_Delta=0;

    అయితే(ఆధారం<ఆధారం_హద్దు && U_SUCCESS(*p_Error_సంకేతం)) {
        ఎంపిక(state) {
        స్థితి 0:
            b=*ఆధారం;
            ఒకవేళ(b==0) {
                state=1; /* could be 00 00 FE FF */
            } లేకపోతే ఒకవేళ(b==(అక్షరం)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } లేకపోతే {
                state=8; /* default to UTF-32BE */
                కొనసాగు;
            }
            ++ఆధారం;
            ఆపు;
        స్థితి 1:
        స్థితి 2:
        స్థితి 3:
        స్థితి 5:
        స్థితి 6:
        స్థితి 7:
            ఒకవేళ(*ఆధారం==utf32BOM[state]) {
                ++state;
                ++ఆధారం;
                ఒకవేళ(state==4) {
                    state=8; /* detect UTF-32BE */
                    స్థానభ్రంశం_Delta=(int32_t)(ఆధారం-p_వాదనలు->ఆధారం);
                } లేకపోతే ఒకవేళ(state==8) {
                    state=9; /* detect UTF-32LE */
                    స్థానభ్రంశం_Delta=(int32_t)(ఆధారం-p_వాదనలు->ఆధారం);
                }
            } లేకపోతే {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(ఆధారం-p_వాదనలు->ఆధారం); /* number of bytes from this buffer */

                /* reset the source */
                ఆధారం=p_వాదనలు->ఆధారం;

                ఒకవేళ(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } లేకపోతే {
                    UBool oldFlush=p_వాదనలు->flush;

                    /* some of the bytes are from a previous buffer, replay those first */
                    p_వాదనలు->ఆధారం=utf32BOM+(state&4); /* select the correct BOM */
                    p_వాదనలు->ఆధారం_హద్దు=p_వాదనలు->ఆధారం+((state&3)-count); /* replay previous bytes */
                    p_వాదనలు->flush=అబద్ధం; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_గమ్యం_Unicode_UTF32_BE(p_వాదనలు, p_Error_సంకేతం);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    p_వాదనలు->ఆధారం_హద్దు=ఆధారం_హద్దు;
                    p_వాదనలు->flush=oldFlush;
                }
                state=8;
                కొనసాగు;
            }
            ఆపు;
        స్థితి 8:
            /* call UTF-32BE */
            p_వాదనలు->ఆధారం=ఆధారం;
            ఒకవేళ(offsets==శూన్యసూచిక) {
                T_UConverter_గమ్యం_Unicode_UTF32_BE(p_వాదనలు, p_Error_సంకేతం);
            } లేకపోతే {
                T_UConverter_గమ్యం_Unicode_UTF32_BE_స్థానభ్రంశం_LOGIC(p_వాదనలు, p_Error_సంకేతం);
            }
            ఆధారం=p_వాదనలు->ఆధారం;
            ఆపు;
        స్థితి 9:
            /* call UTF-32LE */
            p_వాదనలు->ఆధారం=ఆధారం;
            ఒకవేళ(offsets==శూన్యసూచిక) {
                T_UConverter_గమ్యం_Unicode_UTF32_LE(p_వాదనలు, p_Error_సంకేతం);
            } లేకపోతే {
                T_UConverter_గమ్యం_Unicode_UTF32_LE_స్థానభ్రంశం_LOGIC(p_వాదనలు, p_Error_సంకేతం);
            }
            ఆధారం=p_వాదనలు->ఆధారం;
            ఆపు;
        అప్రమేయ:
            ఆపు; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    ఒకవేళ(offsets!=శూన్యసూచిక && స్థానభ్రంశం_Delta!=0) {
        int32_t *offsets_హద్దు=p_వాదనలు->offsets;
        అయితే(offsets<offsets_హద్దు) {
            *offsets++ += స్థానభ్రంశం_Delta;
        }
    }

    p_వాదనలు->ఆధారం=ఆధారం;

    ఒకవేళ(ఆధారం==ఆధారం_హద్దు && p_వాదనలు->flush) {
        /* handle truncated input */
        ఎంపిక(state) {
        స్థితి 0:
            ఆపు; /* no input at all, nothing to do */
        స్థితి 8:
            T_UConverter_గమ్యం_Unicode_UTF32_BE(p_వాదనలు, p_Error_సంకేతం);
            ఆపు;
        స్థితి 9:
            T_UConverter_గమ్యం_Unicode_UTF32_LE(p_వాదనలు, p_Error_సంకేతం);
            ఆపు;
        అప్రమేయ:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            p_వాదనలు->ఆధారం=utf32BOM+(state&4); /* select the correct BOM */
            p_వాదనలు->ఆధారం_హద్దు=p_వాదనలు->ఆధారం+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_గమ్యం_Unicode_UTF32_BE(p_వాదనలు, p_Error_సంకేతం);
            p_వాదనలు->ఆధారం=ఆధారం;
            p_వాదనలు->ఆధారం_హద్దు=ఆధారం_హద్దు;
            state=8;
            ఆపు;
        }
    }

    cnv->బహుళకం=state;
}

స్థిర UChar32 U_CALLCONV
UTF32_Get_తదుపరి_UChar(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                   UError_సంకేతం *p_Error_సంకేతం) {
    ఎంపిక(p_వాదనలు->converter->బహుళకం) {
    స్థితి 8:
        తిరిగి T_UConverter_get_తదుపరి_UChar_UTF32_BE(p_వాదనలు, p_Error_సంకేతం);
    స్థితి 9:
        తిరిగి T_UConverter_get_తదుపరి_UChar_UTF32_LE(p_వాదనలు, p_Error_సంకేతం);
    అప్రమేయ:
        తిరిగి UCNV_GET_తదుపరి_UCHAR_USE_గమ్యం_U;
    }
}
U_CDECL_చివర
స్థిర స్థిరరాశి UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    శూన్యసూచిక,
    శూన్యసూచిక,

    _UTF32Open,
    శూన్యసూచిక,
    _UTF32Reset,

    UTF32_గమ్యం_Unicode_With_Offsets,
    UTF32_గమ్యం_Unicode_With_Offsets,
#ఒకవేళ U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_స్థానభ్రంశం_LOGIC,
#లేకపోతే
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_స్థానభ్రంశం_LOGIC,
#అయితే_ముగింపు
    UTF32_Get_తదుపరి_UChar,

    శూన్యసూచిక, /* ### TODO implement getStarters for all Unicode encodings?! */
    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
    ucnv_get_Non_Surrogate_Unicode_సమితి,

    శూన్యసూచిక,
    శూన్యసూచిక
};

/* The 1236 CCSID refers to any version of Unicode with a BOM sensitive endianness of UTF-32 */
స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం UTF32_Static_దత్తాంశం = {
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#ఒకవేళ U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#లేకపోతే
    { 0xfd, 0xff, 0, 0 }, 4,
#అయితే_ముగింపు
    అబద్ధం, అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

స్థిరరాశి UConverter_Shared_దత్తాంశం UTF32_దత్తాంశం = 
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&UTF32_Static_దత్తాంశం, &_UTF32Impl);

#అయితే_ముగింపు
