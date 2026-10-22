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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#অন্তর্ভাব "unicode/ucnv.h"
#অন্তর্ভাব "unicode/utf.h"
#অন্তর্ভাব "ucnv_bld.h"
#অন্তর্ভাব "ucnv_cnv.h"
#অন্তর্ভাব "cmemory.h"

#define MAXIMUM_UCS2            0x0000FFFF
#define MAXIMUM_UTF             0x0010FFFF
#define HALF_SHIFT              10
#define HALF_BASE               0x0010000
#define HALF_MASK               0x3FF
#define SURROGATE_উপরের_START    0xD800
#define SURROGATE_নিচের_START     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#define SURROGATE_নিচের_BASE      9216

গণনা {
    UCNV_NEED_TO_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
স্থির শূন্য U_CALLCONV
T_UConverter_toUnicode_UTF32_BE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    ধ্রুবক unsigned অক্ষর *mySource = (unsigned অক্ষর *) args->source;
    char16_t *myTarget = args->target;
    ধ্রুবক unsigned অক্ষর *sourceLimit = (unsigned অক্ষর *) args->sourceLimit;
    ধ্রুবক char16_t *targetLimit = args->targetLimit;
    unsigned অক্ষর *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    যদি (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        যাও morebytes;
    }

    যখন (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        যখন (i < আকার(uint32_t)) {
            যদি (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (অক্ষর) *(mySource++);
            }
            অন্যথা {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                যাও donefornow;
            }
        }

        যদি (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            যদি (ch <= MAXIMUM_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            অন্যথা {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                যদি (myTarget < targetLimit) {
                    *(myTarget++) = (char16_t)ch;
                }
                অন্যথা {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UChar_Error_বাফার[0] = (char16_t) ch;
                    args->converter->UChar_Error_বাফার_দৈর্ঘ্য = 1;
                    *err = U_বাফার_OVERFLOW_ERROR;
                    বিরতি;
                }
            }
        }
        অন্যথা {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            বিরতি;
        }
    }

donefornow:
    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_বাফার_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ধ্রুবক অক্ষর *) mySource;
}

স্থির শূন্য U_CALLCONV
T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    ধ্রুবক unsigned অক্ষর *mySource = (unsigned অক্ষর *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    ধ্রুবক unsigned অক্ষর *sourceLimit = (unsigned অক্ষর *) args->sourceLimit;
    ধ্রুবক char16_t *targetLimit = args->targetLimit;
    unsigned অক্ষর *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_অঙ্ক = 0;

    /* Restore state of current sequence */
    যদি (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        যাও morebytes;
    }

    যখন (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        যখন (i < আকার(uint32_t)) {
            যদি (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (অক্ষর) *(mySource++);
            }
            অন্যথা {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                যাও donefornow;
            }
        }

        যদি (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            যদি (ch <= MAXIMUM_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = offset_অঙ্ক;
            }
            অন্যথা {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                *myOffsets++ = offset_অঙ্ক;
                ch = U16_TRAIL(ch);
                যদি (myTarget < targetLimit)
                {
                    *(myTarget++) = (char16_t)ch;
                    *(myOffsets++) = offset_অঙ্ক;
                }
                অন্যথা {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UChar_Error_বাফার[0] = (char16_t) ch;
                    args->converter->UChar_Error_বাফার_দৈর্ঘ্য = 1;
                    *err = U_বাফার_OVERFLOW_ERROR;
                    বিরতি;
                }
            }
        }
        অন্যথা {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            বিরতি;
        }
        offset_অঙ্ক += i;
    }

donefornow:
    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_বাফার_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ধ্রুবক অক্ষর *) mySource;
    args->offsets = myOffsets;
}

স্থির শূন্য U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    ধ্রুবক char16_t *mySource = args->source;
    unsigned অক্ষর *myTarget;
    ধ্রুবক char16_t *sourceLimit = args->sourceLimit;
    ধ্রুবক unsigned অক্ষর *targetLimit = (unsigned অক্ষর *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned পূর্ণসংখ্যা সূচক_To_Write;
    unsigned অক্ষর অস্থায়ী[আকার(uint32_t)];

    যদি(mySource >= sourceLimit) {
        /* no input, nothing to do */
        ফেরত;
    }

    /* write the BOM if necessary */
    যদি(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        স্থির ধ্রুবক অক্ষর bom[]={ 0, 0, (অক্ষর)0xfeu, (অক্ষর)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned অক্ষর *) args->target;
    অস্থায়ী[0] = 0;

    যদি (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        যাও lowsurogate;
    }

    যখন (mySource < sourceLimit && myTarget < targetLimit) {
        ch = *(mySource++);

        যদি (U_IS_SURROGATE(ch)) {
            যদি (U_IS_LEAD(ch)) {
lowsurogate:
                যদি (mySource < sourceLimit) {
                    ch2 = *mySource;
                    যদি (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_উপরের_START) << HALF_SHIFT) + ch2 + SURROGATE_নিচের_BASE;
                        mySource++;
                    }
                    অন্যথা {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        বিরতি;
                    }
                }
                অন্যথা {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    যদি (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    বিরতি;
                }
            }
            অন্যথা {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                বিরতি;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        অস্থায়ী[1] = (uint8_t) (ch >> 16 & 0x1F);
        অস্থায়ী[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        অস্থায়ী[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        জন্য (সূচক_To_Write = 0; সূচক_To_Write <= আকার(uint32_t) - 1; সূচক_To_Write++) {
            যদি (myTarget < targetLimit) {
                *(myTarget++) = অস্থায়ী[সূচক_To_Write];
            }
            অন্যথা {
                args->converter->char_Error_বাফার[args->converter->char_Error_বাফার_দৈর্ঘ্য++] = অস্থায়ী[সূচক_To_Write];
                *err = U_বাফার_OVERFLOW_ERROR;
            }
        }
    }

    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_বাফার_OVERFLOW_ERROR;
    }

    args->target = (অক্ষর *) myTarget;
    args->source = mySource;
}

স্থির শূন্য U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    ধ্রুবক char16_t *mySource = args->source;
    unsigned অক্ষর *myTarget;
    int32_t *myOffsets;
    ধ্রুবক char16_t *sourceLimit = args->sourceLimit;
    ধ্রুবক unsigned অক্ষর *targetLimit = (unsigned অক্ষর *) args->targetLimit;
    UChar32 ch, ch2;
    int32_t offset_অঙ্ক = 0;
    unsigned পূর্ণসংখ্যা সূচক_To_Write;
    unsigned অক্ষর অস্থায়ী[আকার(uint32_t)];

    যদি(mySource >= sourceLimit) {
        /* no input, nothing to do */
        ফেরত;
    }

    /* write the BOM if necessary */
    যদি(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        স্থির ধ্রুবক অক্ষর bom[]={ 0, 0, (অক্ষর)0xfeu, (অক্ষর)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned অক্ষর *) args->target;
    myOffsets = args->offsets;
    অস্থায়ী[0] = 0;

    যদি (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        যাও lowsurogate;
    }

    যখন (mySource < sourceLimit && myTarget < targetLimit) {
        ch = *(mySource++);

        যদি (U_IS_SURROGATE(ch)) {
            যদি (U_IS_LEAD(ch)) {
lowsurogate:
                যদি (mySource < sourceLimit) {
                    ch2 = *mySource;
                    যদি (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_উপরের_START) << HALF_SHIFT) + ch2 + SURROGATE_নিচের_BASE;
                        mySource++;
                    }
                    অন্যথা {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        বিরতি;
                    }
                }
                অন্যথা {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    যদি (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    বিরতি;
                }
            }
            অন্যথা {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                বিরতি;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        অস্থায়ী[1] = (uint8_t) (ch >> 16 & 0x1F);
        অস্থায়ী[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        অস্থায়ী[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        জন্য (সূচক_To_Write = 0; সূচক_To_Write <= আকার(uint32_t) - 1; সূচক_To_Write++) {
            যদি (myTarget < targetLimit) {
                *(myTarget++) = অস্থায়ী[সূচক_To_Write];
                *(myOffsets++) = offset_অঙ্ক;
            }
            অন্যথা {
                args->converter->char_Error_বাফার[args->converter->char_Error_বাফার_দৈর্ঘ্য++] = অস্থায়ী[সূচক_To_Write];
                *err = U_বাফার_OVERFLOW_ERROR;
            }
        }
        offset_অঙ্ক = offset_অঙ্ক + 1 + (অস্থায়ী[1] != 0);
    }

    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_বাফার_OVERFLOW_ERROR;
    }

    args->target = (অক্ষর *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

স্থির UChar32 U_CALLCONV
T_UConverter_get_পরবর্তী_UChar_UTF32_BE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    ধ্রুবক uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (ধ্রুবক uint8_t *)args->source;
    যদি (mySource >= (ধ্রুবক uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0xffff;
    }

    length = (int32_t)((ধ্রুবক uint8_t *)args->sourceLimit - mySource);
    যদি (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (ধ্রুবক অক্ষর *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        ফেরত 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[0] << 24)
            | ((UChar32)mySource[1] << 16)
            | ((UChar32)mySource[2] << 8)
            | ((UChar32)mySource[3]);

    args->source = (ধ্রুবক অক্ষর *)(mySource + 4);
    যদি ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        ফেরত myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    ফেরত 0xffff;
}
U_CDECL_END
স্থির ধ্রুবক UConverterImpl _UTF32BEImpl = {
    UCNV_UTF32_BigEndian,

    nullptr,
    nullptr,

    nullptr,
    nullptr,
    nullptr,

    T_UConverter_toUnicode_UTF32_BE,
    T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC,
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC,
    T_UConverter_get_পরবর্তী_UChar_UTF32_BE,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
স্থির ধ্রুবক UConverter_Static_তথ্য UTF32_BEStatic_তথ্য = {
    আকার(UConverter_Static_তথ্য),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, মিথ্যা, মিথ্যা,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ধ্রুবক UConverter_Shared_তথ্য _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_তথ্য_INITIALIZER(&UTF32_BEStatic_তথ্য, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
স্থির শূন্য U_CALLCONV
T_UConverter_toUnicode_UTF32_LE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    ধ্রুবক unsigned অক্ষর *mySource = (unsigned অক্ষর *) args->source;
    char16_t *myTarget = args->target;
    ধ্রুবক unsigned অক্ষর *sourceLimit = (unsigned অক্ষর *) args->sourceLimit;
    ধ্রুবক char16_t *targetLimit = args->targetLimit;
    unsigned অক্ষর *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    যদি (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        যাও morebytes;
    }

    যখন (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        যখন (i < আকার(uint32_t))
        {
            যদি (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (অক্ষর) *(mySource++);
            }
            অন্যথা
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                যাও donefornow;
            }
        }

        যদি (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            যদি (ch <= MAXIMUM_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            অন্যথা {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                যদি (myTarget < targetLimit) {
                    *(myTarget++) = (char16_t)ch;
                }
                অন্যথা {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UChar_Error_বাফার[0] = (char16_t) ch;
                    args->converter->UChar_Error_বাফার_দৈর্ঘ্য = 1;
                    *err = U_বাফার_OVERFLOW_ERROR;
                    বিরতি;
                }
            }
        }
        অন্যথা {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            বিরতি;
        }
    }

donefornow:
    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_বাফার_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ধ্রুবক অক্ষর *) mySource;
}

স্থির শূন্য U_CALLCONV
T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    ধ্রুবক unsigned অক্ষর *mySource = (unsigned অক্ষর *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    ধ্রুবক unsigned অক্ষর *sourceLimit = (unsigned অক্ষর *) args->sourceLimit;
    ধ্রুবক char16_t *targetLimit = args->targetLimit;
    unsigned অক্ষর *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_অঙ্ক = 0;

    /* Restore state of current sequence */
    যদি (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        যাও morebytes;
    }

    যখন (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        যখন (i < আকার(uint32_t))
        {
            যদি (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (অক্ষর) *(mySource++);
            }
            অন্যথা
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                যাও donefornow;
            }
        }

        যদি (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            যদি (ch <= MAXIMUM_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = offset_অঙ্ক;
            }
            অন্যথা {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                *(myOffsets++) = offset_অঙ্ক;
                ch = U16_TRAIL(ch);
                যদি (myTarget < targetLimit)
                {
                    *(myTarget++) = (char16_t)ch;
                    *(myOffsets++) = offset_অঙ্ক;
                }
                অন্যথা
                {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UChar_Error_বাফার[0] = (char16_t) ch;
                    args->converter->UChar_Error_বাফার_দৈর্ঘ্য = 1;
                    *err = U_বাফার_OVERFLOW_ERROR;
                    বিরতি;
                }
            }
        }
        অন্যথা
        {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            বিরতি;
        }
        offset_অঙ্ক += i;
    }

donefornow:
    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_বাফার_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ধ্রুবক অক্ষর *) mySource;
    args->offsets = myOffsets;
}

স্থির শূন্য U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    ধ্রুবক char16_t *mySource = args->source;
    unsigned অক্ষর *myTarget;
    ধ্রুবক char16_t *sourceLimit = args->sourceLimit;
    ধ্রুবক unsigned অক্ষর *targetLimit = (unsigned অক্ষর *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned পূর্ণসংখ্যা সূচক_To_Write;
    unsigned অক্ষর অস্থায়ী[আকার(uint32_t)];

    যদি(mySource >= sourceLimit) {
        /* no input, nothing to do */
        ফেরত;
    }

    /* write the BOM if necessary */
    যদি(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        স্থির ধ্রুবক অক্ষর bom[]={ (অক্ষর)0xffu, (অক্ষর)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned অক্ষর *) args->target;
    অস্থায়ী[3] = 0;

    যদি (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        যাও lowsurogate;
    }

    যখন (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        যদি (U16_IS_SURROGATE(ch)) {
            যদি (U16_IS_LEAD(ch))
            {
lowsurogate:
                যদি (mySource < sourceLimit)
                {
                    ch2 = *mySource;
                    যদি (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_উপরের_START) << HALF_SHIFT) + ch2 + SURROGATE_নিচের_BASE;
                        mySource++;
                    }
                    অন্যথা {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        বিরতি;
                    }
                }
                অন্যথা {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    যদি (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    বিরতি;
                }
            }
            অন্যথা {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                বিরতি;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        অস্থায়ী[2] = (uint8_t) (ch >> 16 & 0x1F);
        অস্থায়ী[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        অস্থায়ী[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        জন্য (সূচক_To_Write = 0; সূচক_To_Write <= আকার(uint32_t) - 1; সূচক_To_Write++)
        {
            যদি (myTarget < targetLimit)
            {
                *(myTarget++) = অস্থায়ী[সূচক_To_Write];
            }
            অন্যথা
            {
                args->converter->char_Error_বাফার[args->converter->char_Error_বাফার_দৈর্ঘ্য++] = অস্থায়ী[সূচক_To_Write];
                *err = U_বাফার_OVERFLOW_ERROR;
            }
        }
    }

    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_বাফার_OVERFLOW_ERROR;
    }

    args->target = (অক্ষর *) myTarget;
    args->source = mySource;
}

স্থির শূন্য U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    ধ্রুবক char16_t *mySource = args->source;
    unsigned অক্ষর *myTarget;
    int32_t *myOffsets;
    ধ্রুবক char16_t *sourceLimit = args->sourceLimit;
    ধ্রুবক unsigned অক্ষর *targetLimit = (unsigned অক্ষর *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned পূর্ণসংখ্যা সূচক_To_Write;
    unsigned অক্ষর অস্থায়ী[আকার(uint32_t)];
    int32_t offset_অঙ্ক = 0;

    যদি(mySource >= sourceLimit) {
        /* no input, nothing to do */
        ফেরত;
    }

    /* write the BOM if necessary */
    যদি(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        স্থির ধ্রুবক অক্ষর bom[]={ (অক্ষর)0xffu, (অক্ষর)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned অক্ষর *) args->target;
    myOffsets = args->offsets;
    অস্থায়ী[3] = 0;

    যদি (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        যাও lowsurogate;
    }

    যখন (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        যদি (U16_IS_SURROGATE(ch)) {
            যদি (U16_IS_LEAD(ch))
            {
lowsurogate:
                যদি (mySource < sourceLimit)
                {
                    ch2 = *mySource;
                    যদি (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_উপরের_START) << HALF_SHIFT) + ch2 + SURROGATE_নিচের_BASE;
                        mySource++;
                    }
                    অন্যথা {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        বিরতি;
                    }
                }
                অন্যথা {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    যদি (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    বিরতি;
                }
            }
            অন্যথা {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                বিরতি;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        অস্থায়ী[2] = (uint8_t) (ch >> 16 & 0x1F);
        অস্থায়ী[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        অস্থায়ী[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        জন্য (সূচক_To_Write = 0; সূচক_To_Write <= আকার(uint32_t) - 1; সূচক_To_Write++)
        {
            যদি (myTarget < targetLimit)
            {
                *(myTarget++) = অস্থায়ী[সূচক_To_Write];
                *(myOffsets++) = offset_অঙ্ক;
            }
            অন্যথা
            {
                args->converter->char_Error_বাফার[args->converter->char_Error_বাফার_দৈর্ঘ্য++] = অস্থায়ী[সূচক_To_Write];
                *err = U_বাফার_OVERFLOW_ERROR;
            }
        }
        offset_অঙ্ক = offset_অঙ্ক + 1 + (অস্থায়ী[2] != 0);
    }

    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_বাফার_OVERFLOW_ERROR;
    }

    args->target = (অক্ষর *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

স্থির UChar32 U_CALLCONV
T_UConverter_get_পরবর্তী_UChar_UTF32_LE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    ধ্রুবক uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (ধ্রুবক uint8_t *)args->source;
    যদি (mySource >= (ধ্রুবক uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0xffff;
    }

    length = (int32_t)((ধ্রুবক uint8_t *)args->sourceLimit - mySource);
    যদি (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (ধ্রুবক অক্ষর *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        ফেরত 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[3] << 24)
            | ((UChar32)mySource[2] << 16)
            | ((UChar32)mySource[1] << 8)
            | ((UChar32)mySource[0]);

    args->source = (ধ্রুবক অক্ষর *)(mySource + 4);
    যদি ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        ফেরত myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    ফেরত 0xffff;
}
U_CDECL_END
স্থির ধ্রুবক UConverterImpl _UTF32LEImpl = {
    UCNV_UTF32_LittleEndian,

    nullptr,
    nullptr,

    nullptr,
    nullptr,
    nullptr,

    T_UConverter_toUnicode_UTF32_LE,
    T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC,
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_fromUnicode_UTF32_LE_OFFSET_LOGIC,
    T_UConverter_get_পরবর্তী_UChar_UTF32_LE,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
স্থির ধ্রুবক UConverter_Static_তথ্য UTF32_LEStatic_তথ্য = {
    আকার(UConverter_Static_তথ্য),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, মিথ্যা, মিথ্যা,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ধ্রুবক UConverter_Shared_তথ্য _UTF32LEData =
        UCNV_IMMUTABLE_SHARED_তথ্য_INITIALIZER(&UTF32_LEStatic_তথ্য, &_UTF32LEImpl);

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
স্থির শূন্য U_CALLCONV
_UTF32Reset(UConverter *cnv, UConverterResetChoice choice) {
    যদি(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->mode=0;
    }
    যদি(choice!=UCNV_RESET_TO_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}

স্থির শূন্য U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverterLoadArgs *pArgs,
           UErrorCode *pErrorCode) {
    (শূন্য)pArgs;
    (শূন্য)pErrorCode;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

স্থির ধ্রুবক অক্ষর utf32BOM[8]={ 0, 0, (অক্ষর)0xfeu, (অক্ষর)0xffu, (অক্ষর)0xffu, (অক্ষর)0xfeu, 0, 0 };

স্থির শূন্য U_CALLCONV
_UTF32ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    UConverter *cnv=pArgs->converter;
    ধ্রুবক অক্ষর *source=pArgs->source;
    ধ্রুবক অক্ষর *sourceLimit=pArgs->sourceLimit;
    int32_t *offsets=pArgs->offsets;

    int32_t state, offsetDelta;
    অক্ষর b;

    state=cnv->mode;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    offsetDelta=0;

    যখন(source<sourceLimit && U_SUCCESS(*pErrorCode)) {
        নির্বাচন(state) {
        ক্ষেত্র 0:
            b=*source;
            যদি(b==0) {
                state=1; /* could be 00 00 FE FF */
            } অন্যথা যদি(b==(অক্ষর)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } অন্যথা {
                state=8; /* default to UTF-32BE */
                চালু;
            }
            ++source;
            বিরতি;
        ক্ষেত্র 1:
        ক্ষেত্র 2:
        ক্ষেত্র 3:
        ক্ষেত্র 5:
        ক্ষেত্র 6:
        ক্ষেত্র 7:
            যদি(*source==utf32BOM[state]) {
                ++state;
                ++source;
                যদি(state==4) {
                    state=8; /* detect UTF-32BE */
                    offsetDelta=(int32_t)(source-pArgs->source);
                } অন্যথা যদি(state==8) {
                    state=9; /* detect UTF-32LE */
                    offsetDelta=(int32_t)(source-pArgs->source);
                }
            } অন্যথা {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(source-pArgs->source); /* number of bytes from this buffer */

                /* reset the source */
                source=pArgs->source;

                যদি(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } অন্যথা {
                    UBool oldFlush=pArgs->flush;

                    /* some of the bytes are from a previous buffer, replay those first */
                    pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
                    pArgs->sourceLimit=pArgs->source+((state&3)-count); /* replay previous bytes */
                    pArgs->flush=মিথ্যা; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    pArgs->sourceLimit=sourceLimit;
                    pArgs->flush=oldFlush;
                }
                state=8;
                চালু;
            }
            বিরতি;
        ক্ষেত্র 8:
            /* call UTF-32BE */
            pArgs->source=source;
            যদি(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            } অন্যথা {
                T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            বিরতি;
        ক্ষেত্র 9:
            /* call UTF-32LE */
            pArgs->source=source;
            যদি(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            } অন্যথা {
                T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            বিরতি;
        পূর্বনির্ধারিত:
            বিরতি; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    যদি(offsets!=nullptr && offsetDelta!=0) {
        int32_t *offsetsLimit=pArgs->offsets;
        যখন(offsets<offsetsLimit) {
            *offsets++ += offsetDelta;
        }
    }

    pArgs->source=source;

    যদি(source==sourceLimit && pArgs->flush) {
        /* handle truncated input */
        নির্বাচন(state) {
        ক্ষেত্র 0:
            বিরতি; /* no input at all, nothing to do */
        ক্ষেত্র 8:
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            বিরতি;
        ক্ষেত্র 9:
            T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            বিরতি;
        পূর্বনির্ধারিত:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
            pArgs->sourceLimit=pArgs->source+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            pArgs->source=source;
            pArgs->sourceLimit=sourceLimit;
            state=8;
            বিরতি;
        }
    }

    cnv->mode=state;
}

স্থির UChar32 U_CALLCONV
UTF32_Get_পরবর্তী_UChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    নির্বাচন(pArgs->converter->mode) {
    ক্ষেত্র 8:
        ফেরত T_UConverter_get_পরবর্তী_UChar_UTF32_BE(pArgs, pErrorCode);
    ক্ষেত্র 9:
        ফেরত T_UConverter_get_পরবর্তী_UChar_UTF32_LE(pArgs, pErrorCode);
    পূর্বনির্ধারিত:
        ফেরত UCNV_GET_পরবর্তী_UCHAR_USE_TO_U;
    }
}
U_CDECL_END
স্থির ধ্রুবক UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    nullptr,
    nullptr,

    _UTF32Open,
    nullptr,
    _UTF32Reset,

    _UTF32ToUnicodeWithOffsets,
    _UTF32ToUnicodeWithOffsets,
#যদি U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC,
#অন্যথা
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_fromUnicode_UTF32_LE_OFFSET_LOGIC,
#endif
    UTF32_Get_পরবর্তী_UChar,

    nullptr, /* ### TODO implement getStarters for all Unicode encodings?! */
    nullptr,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

/* The 1236 CCSID refers to any version of Unicode with a BOM sensitive endianness of UTF-32 */
স্থির ধ্রুবক UConverter_Static_তথ্য UTF32_Static_তথ্য = {
    আকার(UConverter_Static_তথ্য),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#যদি U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#অন্যথা
    { 0xfd, 0xff, 0, 0 }, 4,
#endif
    মিথ্যা, মিথ্যা,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ধ্রুবক UConverter_Shared_তথ্য UTF32_তথ্য = 
        UCNV_IMMUTABLE_SHARED_তথ্য_INITIALIZER(&UTF32_Static_তথ্য, &_UTF32Impl);

#endif
