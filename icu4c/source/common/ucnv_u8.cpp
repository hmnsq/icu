// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*  
**********************************************************************
*   Copyright (C) 2002-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  ucnv_u8.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002jul01
*   created by: Markus W. Scherer
*
*   UTF-8 converter implementation. Used to be in ucnv_utf.c.
*
*   Also, CESU-8 implementation, see UTR 26.
*   The CESU-8 converter uses all the same functions as the
*   UTF-8 converter, with a branch for converting supplementary code points.
*/

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/utf.h"
#అన్తర్భావ "unicode/utf8.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "ustr_imp.h"

/* Prototypes --------------------------------------------------------------- */

/* Keep these here to make finicky compilers happy */

U_CFUNC శూన్య ucnv_fromUnicode_UTF8(UConverter_From_Unicode_వాదనలు *వాదనలు_2,
                                           UError_సంకేతం *err);
U_CFUNC శూన్య ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverter_From_Unicode_వాదనలు *వాదనలు_2,
                                                        UError_సంకేతం *err);


/* UTF-8 -------------------------------------------------------------------- */

#నిర్వచించు గరిష్ఠ_UCS2            0x0000FFFF

స్థిర స్థిరరాశి uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

స్థిర UBool has_CESU8_దత్తాంశం(స్థిరరాశి UConverter *cnv)
{
#ఒకవేళ UCONFIG_ONLY_HTML_CONVERSION
    తిరిగి అబద్ధం;
#లేకపోతే
    తిరిగి cnv->shared_దత్తాంశం == &CESU8_దత్తాంశం;
#అయితే_ముగింపు
}
U_CDECL_BEGIN
స్థిర శూన్య  U_CALLCONV ucnv_గమ్యం_Unicode_UTF8 (UConverter_గమ్యం_Unicode_వాదనలు * వాదనలు_2,
                                  UError_సంకేతం * err)
{
    UConverter *cnv = వాదనలు_2->converter;
    స్థిరరాశి గుర్తులేని అక్షరం *my_ఆధారం = (గుర్తులేని అక్షరం *) వాదనలు_2->ఆధారం;
    char16_t *my_లక్ష్యం = వాదనలు_2->లక్ష్యం;
    స్థిరరాశి గుర్తులేని అక్షరం *ఆధారం_హద్దు = (గుర్తులేని అక్షరం *) వాదనలు_2->ఆధారం_హద్దు;
    స్థిరరాశి char16_t *లక్ష్యం_హద్దు = వాదనలు_2->లక్ష్యం_హద్దు;
    గుర్తులేని అక్షరం *గమ్యం_UBytes = cnv->గమ్యం_UBytes;
    UBool isCESU8 = has_CESU8_దత్తాంశం(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, లోపల_బైట్లు;

    /* Restore size of current sequence */
    ఒకవేళ (cnv->గమ్యం_ULength > 0 && my_లక్ష్యం < లక్ష్యం_హద్దు)
    {
        లోపల_బైట్లు = cnv->బహుళకం;            /* restore # of bytes to consume */
        i = cnv->గమ్యం_ULength;             /* restore # of bytes consumed */
        cnv->గమ్యం_ULength = 0;

        ch = cnv->గమ్యం_Unicode_స్థితి;/*Stores the previously calculated ch from a previous call*/
        cnv->గమ్యం_Unicode_స్థితి = 0;
        వెళ్ళు morebytes;
    }


    అయితే (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం < లక్ష్యం_హద్దు)
    {
        ch = *(my_ఆధారం++);
        ఒకవేళ (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(my_లక్ష్యం++) = (char16_t) ch;
        }
        లేకపోతే
        {
            /* store the first char */
            గమ్యం_UBytes[0] = (అక్షరం)ch;
            లోపల_బైట్లు = U8_లెక్క_బైట్లు_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            అయితే (i < లోపల_బైట్లు)
            {
                ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు)
                {
                    గమ్యం_UBytes[i] = (అక్షరం) (ch2 = *my_ఆధారం);
                    ఒకవేళ (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, లోపల_బైట్లు) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        ఆపు; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++my_ఆధారం;
                    i++;
                }
                లేకపోతే
                {
                    /* stores a partially calculated target*/
                    cnv->గమ్యం_Unicode_స్థితి = ch;
                    cnv->బహుళకం = లోపల_బైట్లు;
                    cnv->గమ్యం_ULength = (int8_t) i;
                    వెళ్ళు donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            ఒకవేళ (i == లోపల_బైట్లు && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[లోపల_బైట్లు];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                ఒకవేళ (ch <= గరిష్ఠ_UCS2) 
                {
                    /* fits in 16 bits */
                    *(my_లక్ష్యం++) = (char16_t) ch;
                }
                లేకపోతే
                {
                    /* write out the surrogates */
                    *(my_లక్ష్యం++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    ఒకవేళ (my_లక్ష్యం < లక్ష్యం_హద్దు)
                    {
                        *(my_లక్ష్యం++) = (char16_t)ch;
                    }
                    లేకపోతే
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UChar_Error_బఫరు[0] = (char16_t) ch;
                        cnv->UChar_Error_బఫరు_నిడివి = 1;
                        *err = U_బఫరు_అధికప్రవాహం_ERROR;
                        ఆపు;
                    }
                }
            }
            లేకపోతే
            {
                cnv->గమ్యం_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_దొరికింది;
                ఆపు;
            }
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

స్థిర శూన్య  U_CALLCONV ucnv_గమ్యం_Unicode_UTF8_OFFSETS_LOGIC (UConverter_గమ్యం_Unicode_వాదనలు * వాదనలు_2,
                                                UError_సంకేతం * err)
{
    UConverter *cnv = వాదనలు_2->converter;
    స్థిరరాశి గుర్తులేని అక్షరం *my_ఆధారం = (గుర్తులేని అక్షరం *) వాదనలు_2->ఆధారం;
    char16_t *my_లక్ష్యం = వాదనలు_2->లక్ష్యం;
    int32_t *myOffsets = వాదనలు_2->offsets;
    int32_t స్థానభ్రంశం_అంకె = 0;
    స్థిరరాశి గుర్తులేని అక్షరం *ఆధారం_హద్దు = (గుర్తులేని అక్షరం *) వాదనలు_2->ఆధారం_హద్దు;
    స్థిరరాశి char16_t *లక్ష్యం_హద్దు = వాదనలు_2->లక్ష్యం_హద్దు;
    గుర్తులేని అక్షరం *గమ్యం_UBytes = cnv->గమ్యం_UBytes;
    UBool isCESU8 = has_CESU8_దత్తాంశం(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, లోపల_బైట్లు;

    /* Restore size of current sequence */
    ఒకవేళ (cnv->గమ్యం_ULength > 0 && my_లక్ష్యం < లక్ష్యం_హద్దు)
    {
        లోపల_బైట్లు = cnv->బహుళకం;            /* restore # of bytes to consume */
        i = cnv->గమ్యం_ULength;             /* restore # of bytes consumed */
        cnv->గమ్యం_ULength = 0;

        ch = cnv->గమ్యం_Unicode_స్థితి;/*Stores the previously calculated ch from a previous call*/
        cnv->గమ్యం_Unicode_స్థితి = 0;
        వెళ్ళు morebytes;
    }

    అయితే (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం < లక్ష్యం_హద్దు)
    {
        ch = *(my_ఆధారం++);
        ఒకవేళ (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(my_లక్ష్యం++) = (char16_t) ch;
            *(myOffsets++) = స్థానభ్రంశం_అంకె++;
        }
        లేకపోతే
        {
            గమ్యం_UBytes[0] = (అక్షరం)ch;
            లోపల_బైట్లు = U8_లెక్క_బైట్లు_NON_ASCII(ch);
            i = 1;

morebytes:
            అయితే (i < లోపల_బైట్లు)
            {
                ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు)
                {
                    గమ్యం_UBytes[i] = (అక్షరం) (ch2 = *my_ఆధారం);
                    ఒకవేళ (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, లోపల_బైట్లు) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        ఆపు; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++my_ఆధారం;
                    i++;
                }
                లేకపోతే
                {
                    cnv->గమ్యం_Unicode_స్థితి = ch;
                    cnv->బహుళకం = లోపల_బైట్లు;
                    cnv->గమ్యం_ULength = (int8_t)i;
                    వెళ్ళు donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            ఒకవేళ (i == లోపల_బైట్లు && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[లోపల_బైట్లు];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                ఒకవేళ (ch <= గరిష్ఠ_UCS2) 
                {
                    /* fits in 16 bits */
                    *(my_లక్ష్యం++) = (char16_t) ch;
                    *(myOffsets++) = స్థానభ్రంశం_అంకె;
                }
                లేకపోతే
                {
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
                        cnv->UChar_Error_బఫరు[0] = (char16_t) ch;
                        cnv->UChar_Error_బఫరు_నిడివి = 1;
                        *err = U_బఫరు_అధికప్రవాహం_ERROR;
                    }
                }
                స్థానభ్రంశం_అంకె += i;
            }
            లేకపోతే
            {
                cnv->గమ్యం_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_దొరికింది;
                ఆపు;
            }
        }
    }

donefornow:
    ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం >= లక్ష్యం_హద్దు && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_బఫరు_అధికప్రవాహం_ERROR;
    }

    వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
    వాదనలు_2->ఆధారం = (స్థిరరాశి అక్షరం *) my_ఆధారం;
    వాదనలు_2->offsets = myOffsets;
}
U_CDECL_చివర

U_CFUNC శూన్య  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverter_From_Unicode_వాదనలు * వాదనలు_2,
                                    UError_సంకేతం * err)
{
    UConverter *cnv = వాదనలు_2->converter;
    స్థిరరాశి char16_t *my_ఆధారం = వాదనలు_2->ఆధారం;
    స్థిరరాశి char16_t *ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    uint8_t *my_లక్ష్యం = (uint8_t *) వాదనలు_2->లక్ష్యం;
    స్థిరరాశి uint8_t *లక్ష్యం_హద్దు = (uint8_t *) వాదనలు_2->లక్ష్యం_హద్దు;
    uint8_t *తాత్కాలిక_Ptr;
    UChar32 ch;
    uint8_t తాత్కాలిక_Buf[4];
    int32_t సూచీ_గమ్యం_Write;
    UBool isNotCESU8 = !has_CESU8_దత్తాంశం(cnv);

    ఒకవేళ (cnv->fromUChar32 && my_లక్ష్యం < లక్ష్యం_హద్దు)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        వెళ్ళు lowsurrogate;
    }

    అయితే (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం < లక్ష్యం_హద్దు)
    {
        ch = *(my_ఆధారం++);

        ఒకవేళ (ch < 0x80)        /* Single byte */
        {
            *(my_లక్ష్యం++) = (uint8_t) ch;
        }
        లేకపోతే ఒకవేళ (ch < 0x800)  /* Double byte */
        {
            *(my_లక్ష్యం++) = (uint8_t) ((ch >> 6) | 0xc0);
            ఒకవేళ (my_లక్ష్యం < లక్ష్యం_హద్దు)
            {
                *(my_లక్ష్యం++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            లేకపోతే
            {
                cnv->char_Error_బఫరు[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_బఫరు_నిడివి = 1;
                *err = U_బఫరు_అధికప్రవాహం_ERROR;
            }
        }
        లేకపోతే {
            /* Check for surrogates */
            ఒకవేళ(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు) {
                    /* test both code units */
                    ఒకవేళ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*my_ఆధారం)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *my_ఆధారం);
                        ++my_ఆధారం;
                        /* exit this condition tree */
                    }
                    లేకపోతే {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_దొరికింది;
                        ఆపు;
                    }
                }
                లేకపోతే {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    ఆపు;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            తాత్కాలిక_Ptr = (((లక్ష్యం_హద్దు - my_లక్ష్యం) >= 4) ? my_లక్ష్యం : తాత్కాలిక_Buf);

            ఒకవేళ (ch <= గరిష్ఠ_UCS2) {
                సూచీ_గమ్యం_Write = 2;
                తాత్కాలిక_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            లేకపోతే {
                సూచీ_గమ్యం_Write = 3;
                తాత్కాలిక_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                తాత్కాలిక_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            తాత్కాలిక_Ptr[సూచీ_గమ్యం_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            తాత్కాలిక_Ptr[సూచీ_గమ్యం_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            ఒకవేళ (తాత్కాలిక_Ptr == my_లక్ష్యం) {
                /* There was enough space to write the codepoint directly. */
                my_లక్ష్యం += (సూచీ_గమ్యం_Write + 1);
            }
            లేకపోతే {
                /* We might run out of room soon. Write it slowly. */
                కొరకు (; తాత్కాలిక_Ptr <= (తాత్కాలిక_Buf + సూచీ_గమ్యం_Write); తాత్కాలిక_Ptr++) {
                    ఒకవేళ (my_లక్ష్యం < లక్ష్యం_హద్దు) {
                        *(my_లక్ష్యం++) = *తాత్కాలిక_Ptr;
                    }
                    లేకపోతే {
                        cnv->char_Error_బఫరు[cnv->char_Error_బఫరు_నిడివి++] = *తాత్కాలిక_Ptr;
                        *err = U_బఫరు_అధికప్రవాహం_ERROR;
                    }
                }
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

U_CFUNC శూన్య  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverter_From_Unicode_వాదనలు * వాదనలు_2,
                                                  UError_సంకేతం * err)
{
    UConverter *cnv = వాదనలు_2->converter;
    స్థిరరాశి char16_t *my_ఆధారం = వాదనలు_2->ఆధారం;
    int32_t *myOffsets = వాదనలు_2->offsets;
    స్థిరరాశి char16_t *ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    uint8_t *my_లక్ష్యం = (uint8_t *) వాదనలు_2->లక్ష్యం;
    స్థిరరాశి uint8_t *లక్ష్యం_హద్దు = (uint8_t *) వాదనలు_2->లక్ష్యం_హద్దు;
    uint8_t *తాత్కాలిక_Ptr;
    UChar32 ch;
    int32_t స్థానభ్రంశం_అంకె, తదుపరి_ఆధారం_సూచీ;
    int32_t సూచీ_గమ్యం_Write;
    uint8_t తాత్కాలిక_Buf[4];
    UBool isNotCESU8 = !has_CESU8_దత్తాంశం(cnv);

    ఒకవేళ (cnv->fromUChar32 && my_లక్ష్యం < లక్ష్యం_హద్దు)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        స్థానభ్రంశం_అంకె = -1;
        తదుపరి_ఆధారం_సూచీ = 0;
        వెళ్ళు lowsurrogate;
    } లేకపోతే {
        స్థానభ్రంశం_అంకె = 0;
    }

    అయితే (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం < లక్ష్యం_హద్దు)
    {
        ch = *(my_ఆధారం++);

        ఒకవేళ (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = స్థానభ్రంశం_అంకె++;
            *(my_లక్ష్యం++) = (అక్షరం) ch;
        }
        లేకపోతే ఒకవేళ (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = స్థానభ్రంశం_అంకె;
            *(my_లక్ష్యం++) = (uint8_t) ((ch >> 6) | 0xc0);
            ఒకవేళ (my_లక్ష్యం < లక్ష్యం_హద్దు)
            {
                *(myOffsets++) = స్థానభ్రంశం_అంకె++;
                *(my_లక్ష్యం++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            లేకపోతే
            {
                cnv->char_Error_బఫరు[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_బఫరు_నిడివి = 1;
                *err = U_బఫరు_అధికప్రవాహం_ERROR;
            }
        }
        లేకపోతే
        /* Check for surrogates */
        {
            తదుపరి_ఆధారం_సూచీ = స్థానభ్రంశం_అంకె + 1;

            ఒకవేళ(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు) {
                    /* test both code units */
                    ఒకవేళ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*my_ఆధారం)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *my_ఆధారం);
                        ++my_ఆధారం;
                        ++తదుపరి_ఆధారం_సూచీ;
                        /* exit this condition tree */
                    }
                    లేకపోతే {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_దొరికింది;
                        ఆపు;
                    }
                }
                లేకపోతే {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    ఆపు;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            తాత్కాలిక_Ptr = (((లక్ష్యం_హద్దు - my_లక్ష్యం) >= 4) ? my_లక్ష్యం : తాత్కాలిక_Buf);

            ఒకవేళ (ch <= గరిష్ఠ_UCS2) {
                సూచీ_గమ్యం_Write = 2;
                తాత్కాలిక_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            లేకపోతే {
                సూచీ_గమ్యం_Write = 3;
                తాత్కాలిక_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                తాత్కాలిక_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            తాత్కాలిక_Ptr[సూచీ_గమ్యం_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            తాత్కాలిక_Ptr[సూచీ_గమ్యం_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            ఒకవేళ (తాత్కాలిక_Ptr == my_లక్ష్యం) {
                /* There was enough space to write the codepoint directly. */
                my_లక్ష్యం += (సూచీ_గమ్యం_Write + 1);
                myOffsets[0] = స్థానభ్రంశం_అంకె;
                myOffsets[1] = స్థానభ్రంశం_అంకె;
                myOffsets[2] = స్థానభ్రంశం_అంకె;
                ఒకవేళ (సూచీ_గమ్యం_Write >= 3) {
                    myOffsets[3] = స్థానభ్రంశం_అంకె;
                }
                myOffsets += (సూచీ_గమ్యం_Write + 1);
            }
            లేకపోతే {
                /* We might run out of room soon. Write it slowly. */
                కొరకు (; తాత్కాలిక_Ptr <= (తాత్కాలిక_Buf + సూచీ_గమ్యం_Write); తాత్కాలిక_Ptr++) {
                    ఒకవేళ (my_లక్ష్యం < లక్ష్యం_హద్దు)
                    {
                        *(myOffsets++) = స్థానభ్రంశం_అంకె;
                        *(my_లక్ష్యం++) = *తాత్కాలిక_Ptr;
                    }
                    లేకపోతే
                    {
                        cnv->char_Error_బఫరు[cnv->char_Error_బఫరు_నిడివి++] = *తాత్కాలిక_Ptr;
                        *err = U_బఫరు_అధికప్రవాహం_ERROR;
                    }
                }
            }
            స్థానభ్రంశం_అంకె = తదుపరి_ఆధారం_సూచీ;
        }
    }

    ఒకవేళ (my_ఆధారం < ఆధారం_హద్దు && my_లక్ష్యం >= లక్ష్యం_హద్దు && U_SUCCESS(*err))
    {
        *err = U_బఫరు_అధికప్రవాహం_ERROR;
    }

    వాదనలు_2->లక్ష్యం = (అక్షరం *) my_లక్ష్యం;
    వాదనలు_2->ఆధారం = my_ఆధారం;
    వాదనలు_2->offsets = myOffsets;
}

U_CDECL_BEGIN
స్థిర UChar32 U_CALLCONV ucnv_get_తదుపరి_UChar_UTF8(UConverter_గమ్యం_Unicode_వాదనలు *వాదనలు_2,
                                               UError_సంకేతం *err) {
    UConverter *cnv;
    స్థిరరాశి uint8_t *ఆధారం_Initial;
    స్థిరరాశి uint8_t *ఆధారం;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = వాదనలు_2->converter;
    ఆధారం_Initial = ఆధారం = (స్థిరరాశి uint8_t *)వాదనలు_2->ఆధారం;
    ఒకవేళ (ఆధారం >= (స్థిరరాశి uint8_t *)వాదనలు_2->ఆధారం_హద్దు)
    {
        /* no input */
        *err = U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0xffff;
    }

    myByte = *(ఆధారం++);
    ఒకవేళ (U8_IS_SINGLE(myByte))
    {
        వాదనలు_2->ఆధారం = (స్థిరరాశి అక్షరం *)ఆధారం;
        తిరిగి (UChar32)myByte;
    }

    uint16_t లెక్క_Trail_బైట్లు = U8_లెక్క_TRAIL_బైట్లు(myByte);
    ఒకవేళ (లెక్క_Trail_బైట్లు == 0) {
        cnv->గమ్యం_UBytes[0] = myByte;
        cnv->గమ్యం_ULength = 1;
        *err = U_ILLEGAL_CHAR_దొరికింది;
        వాదనలు_2->ఆధారం = (స్థిరరాశి అక్షరం *)ఆధారం;
        తిరిగి 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    ఒకవేళ (((స్థిరరాశి అక్షరం *)ఆధారం + లెక్క_Trail_బైట్లు) > వాదనలు_2->ఆధారం_హద్దు)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extra_బైట్లు_గమ్యం_Write = లెక్క_Trail_బైట్లు + 1;
        cnv->గమ్యం_UBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_దొరికింది;
        అయితే(ఆధారం < (స్థిరరాశి uint8_t *)వాదనలు_2->ఆధారం_హద్దు) {
            uint8_t b = *ఆధారం;
            ఒకవేళ(icu::UTF8::isValidTrail(myByte, b, i, extra_బైట్లు_గమ్యం_Write)) {
                cnv->గమ్యం_UBytes[i++] = b;
                ++ఆధారం;
            } లేకపోతే {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_దొరికింది;
                ఆపు;
            }
        }
        cnv->గమ్యం_ULength = i;
        వాదనలు_2->ఆధారం = (స్థిరరాశి అక్షరం *)ఆధారం;
        తిరిగి 0xffff;
    }

    ch = myByte << 6;
    ఒకవేళ(లెక్క_Trail_బైట్లు == 2) {
        uint8_t t1 = *ఆధారం, t2;
        ఒకవేళ(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++ఆధారం)) {
            వాదనలు_2->ఆధారం = (స్థిరరాశి అక్షరం *)(ఆధారం + 1);
            తిరిగి (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } లేకపోతే ఒకవేళ(లెక్క_Trail_బైట్లు == 1) {
        uint8_t t1 = *ఆధారం;
        ఒకవేళ(U8_IS_TRAIL(t1)) {
            వాదనలు_2->ఆధారం = (స్థిరరాశి అక్షరం *)(ఆధారం + 1);
            తిరిగి (ch + t1) - offsetsFromUTF8[2];
        }
    } లేకపోతే {  // countTrailBytes == 3
        uint8_t t1 = *ఆధారం, t2, t3;
        ఒకవేళ(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++ఆధారం) &&
                U8_IS_TRAIL(t3 = *++ఆధారం)) {
            వాదనలు_2->ఆధారం = (స్థిరరాశి అక్షరం *)(ఆధారం + 1);
            తిరిగి (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    వాదనలు_2->ఆధారం = (స్థిరరాశి అక్షరం *)ఆధారం;

    కొరకు(i = 0; ఆధారం_Initial < ఆధారం; ++i) {
        cnv->గమ్యం_UBytes[i] = *ఆధారం_Initial++;
    }
    cnv->గమ్యం_ULength = i;
    *err = U_ILLEGAL_CHAR_దొరికింది;
    తిరిగి 0xffff;
} 
U_CDECL_చివర

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
స్థిర శూన్య U_CALLCONV
ucnv_UTF8FromUTF8(UConverter_From_Unicode_వాదనలు *pFromUArgs,
                  UConverter_గమ్యం_Unicode_వాదనలు *p_గమ్యం_UArgs,
                  UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *utf8;
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు;
    uint8_t *లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం;
    int32_t count;

    int8_t old_గమ్యం_ULength, గమ్యం_ULength, గమ్యం_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    /* set up the local pointers */
    utf8=p_గమ్యం_UArgs->converter;
    ఆధారం=(uint8_t *)p_గమ్యం_UArgs->ఆధారం;
    ఆధారం_హద్దు=(uint8_t *)p_గమ్యం_UArgs->ఆధారం_హద్దు;
    లక్ష్యం=(uint8_t *)pFromUArgs->లక్ష్యం;
    లక్ష్యం_సామర్థ్యం=(int32_t)(pFromUArgs->లక్ష్యం_హద్దు-pFromUArgs->లక్ష్యం);

    /* get the converter state from the UTF-8 UConverter */
    ఒకవేళ(utf8->గమ్యం_ULength > 0) {
        గమ్యం_ULength=old_గమ్యం_ULength=utf8->గమ్యం_ULength;
        గమ్యం_ULimit=(int8_t)utf8->బహుళకం;
        c=(UChar32)utf8->గమ్యం_Unicode_స్థితి;
    } లేకపోతే {
        గమ్యం_ULength=old_గమ్యం_ULength=గమ్యం_ULimit=0;
        c = 0;
    }

    count=(int32_t)(ఆధారం_హద్దు-ఆధారం)+old_గమ్యం_ULength;
    ఒకవేళ(count<గమ్యం_ULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } లేకపోతే ఒకవేళ(లక్ష్యం_సామర్థ్యం<గమ్యం_ULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *p_Error_సంకేతం=U_USING_DEFAULT_WARNING;
        తిరిగి;
    } లేకపోతే {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        ఒకవేళ(count>లక్ష్యం_సామర్థ్యం) {
            count=లక్ష్యం_సామర్థ్యం;
        }

        // The conversion loop checks count>0 only once per character.
        // If the buffer ends with a truncated sequence,
        // then we reduce the count to stop before that,
        // and collect the remaining bytes after the conversion loop.

        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length=count-గమ్యం_ULength;
        U8_TRUNCATE_IF_INCOMPLETE(ఆధారం, 0, length);
        count=గమ్యం_ULength+length;
    }

    ఒకవేళ(c!=0) {
        utf8->గమ్యం_Unicode_స్థితి=0;
        utf8->గమ్యం_ULength=0;
        వెళ్ళు మరిన్ని_బైట్లు;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    అయితే(count>0) {
        b=*ఆధారం++;
        ఒకవేళ(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *లక్ష్యం++=b;
            --count;
            కొనసాగు;
        } లేకపోతే {
            ఒకవేళ(b>=0xe0) {
                ఒకవేళ( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=ఆధారం[0]) &&
                    U8_IS_TRAIL(t2=ఆధారం[1])
                ) {
                    ఆధారం+=2;
                    *లక్ష్యం++=b;
                    *లక్ష్యం++=t1;
                    *లక్ష్యం++=t2;
                    count-=3;
                    కొనసాగు;
                }
            } లేకపోతే {
                ఒకవేళ( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*ఆధారం)
                ) {
                    ++ఆధారం;
                    *లక్ష్యం++=b;
                    *లక్ష్యం++=t1;
                    count-=2;
                    కొనసాగు;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            old_గమ్యం_ULength=0;
            గమ్యం_ULength=1;
            గమ్యం_ULimit=U8_లెక్క_బైట్లు_NON_ASCII(b);
            c=b;
మరిన్ని_బైట్లు:
            అయితే(గమ్యం_ULength<గమ్యం_ULimit) {
                ఒకవేళ(ఆధారం<ఆధారం_హద్దు) {
                    b=*ఆధారం;
                    ఒకవేళ(icu::UTF8::isValidTrail(c, b, గమ్యం_ULength, గమ్యం_ULimit)) {
                        ++ఆధారం;
                        ++గమ్యం_ULength;
                        c=(c<<6)+b;
                    } లేకపోతే {
                        ఆపు; /* sequence too short, stop with toULength<toULimit */
                    }
                } లేకపోతే {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    ఆధారం-=(గమ్యం_ULength-old_గమ్యం_ULength);
                    అయితే(old_గమ్యం_ULength<గమ్యం_ULength) {
                        utf8->గమ్యం_UBytes[old_గమ్యం_ULength++]=*ఆధారం++;
                    }
                    utf8->గమ్యం_Unicode_స్థితి=c;
                    utf8->గమ్యం_ULength=గమ్యం_ULength;
                    utf8->బహుళకం=గమ్యం_ULimit;
                    p_గమ్యం_UArgs->ఆధారం=(అక్షరం *)ఆధారం;
                    pFromUArgs->లక్ష్యం=(అక్షరం *)లక్ష్యం;
                    తిరిగి;
                }
            }

            ఒకవేళ(గమ్యం_ULength!=గమ్యం_ULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                ఆధారం-=(గమ్యం_ULength-old_గమ్యం_ULength);
                అయితే(old_గమ్యం_ULength<గమ్యం_ULength) {
                    utf8->గమ్యం_UBytes[old_గమ్యం_ULength++]=*ఆధారం++;
                }
                utf8->గమ్యం_ULength=గమ్యం_ULength;
                p_గమ్యం_UArgs->ఆధారం=(అక్షరం *)ఆధారం;
                pFromUArgs->లక్ష్యం=(అక్షరం *)లక్ష్యం;
                *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                తిరిగి;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                కొరకు(i=0; i<old_గమ్యం_ULength; ++i) {
                    *లక్ష్యం++=utf8->గమ్యం_UBytes[i];
                }
                ఆధారం-=(గమ్యం_ULength-old_గమ్యం_ULength);
                కొరకు(; i<గమ్యం_ULength; ++i) {
                    *లక్ష్యం++=*ఆధారం++;
                }
                count-=గమ్యం_ULength;
            }
        }
    }
    U_ASSERT(count>=0);

    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం) && ఆధారం<ఆధారం_హద్దు) {
        ఒకవేళ(లక్ష్యం==(స్థిరరాశి uint8_t *)pFromUArgs->లక్ష్యం_హద్దు) {
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
        } లేకపోతే {
            b=*ఆధారం;
            గమ్యం_ULimit=U8_లెక్క_బైట్లు(b);
            ఒకవేళ(గమ్యం_ULimit>(ఆధారం_హద్దు-ఆధారం)) {
                /* collect a truncated byte sequence */
                గమ్యం_ULength=0;
                c=b;
                కొరకు(;;) {
                    utf8->గమ్యం_UBytes[గమ్యం_ULength++]=b;
                    ఒకవేళ(++ఆధారం==ఆధారం_హద్దు) {
                        /* partial byte sequence at end of source */
                        utf8->గమ్యం_Unicode_స్థితి=c;
                        utf8->గమ్యం_ULength=గమ్యం_ULength;
                        utf8->బహుళకం=గమ్యం_ULimit;
                        ఆపు;
                    } లేకపోతే ఒకవేళ(!icu::UTF8::isValidTrail(c, b=*ఆధారం, గమ్యం_ULength, గమ్యం_ULimit)) {
                        utf8->గమ్యం_ULength=గమ్యం_ULength;
                        *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                        ఆపు;
                    }
                    c=(c<<6)+b;
                }
            } లేకపోతే {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *p_Error_సంకేతం=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    p_గమ్యం_UArgs->ఆధారం=(అక్షరం *)ఆధారం;
    pFromUArgs->లక్ష్యం=(అక్షరం *)లక్ష్యం;
}

U_CDECL_చివర

/* UTF-8 converter data ----------------------------------------------------- */

స్థిర స్థిరరాశి UConverterImpl _UTF8Impl={
    UCNV_UTF8,

    శూన్యసూచిక,
    శూన్యసూచిక,

    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,

    ucnv_గమ్యం_Unicode_UTF8,
    ucnv_గమ్యం_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    ucnv_get_తదుపరి_UChar_UTF8,

    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
    ucnv_get_Non_Surrogate_Unicode_సమితి,

    ucnv_UTF8FromUTF8,
    ucnv_UTF8FromUTF8
};

/* The 1208 CCSID refers to any version of Unicode of UTF-8 */
స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం UTF8_Static_దత్తాంశం={
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,అబద్ధం,అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


స్థిరరాశి UConverter_Shared_దత్తాంశం UTF8_దత్తాంశం=
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&UTF8_Static_దత్తాంశం, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

స్థిర స్థిరరాశి UConverterImpl _CESU8Impl={
    UCNV_CESU8,

    శూన్యసూచిక,
    శూన్యసూచిక,

    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,

    ucnv_గమ్యం_Unicode_UTF8,
    ucnv_గమ్యం_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    శూన్యసూచిక,

    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
    ucnv_get_Complete_Unicode_సమితి,

    శూన్యసూచిక,
    శూన్యసూచిక
};

స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం CESU8_Static_దత్తాంశం={
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,అబద్ధం,అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


స్థిరరాశి UConverter_Shared_దత్తాంశం CESU8_దత్తాంశం=
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&CESU8_Static_దత్తాంశం, &_CESU8Impl);

#అయితే_ముగింపు
