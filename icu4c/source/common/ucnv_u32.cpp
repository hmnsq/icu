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
    ধ্রুব unsigned বর্ণ *mySource = (unsigned বর্ণ *) args->source;
    char16_t *myTarget = args->target;
    ধ্রুব unsigned বর্ণ *sourceLimit = (unsigned বর্ণ *) args->sourceLimit;
    ধ্রুব char16_t *targetLimit = args->targetLimit;
    unsigned বর্ণ *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    যদি (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        গচ্ছ morebytes;
    }

    যাবত্ (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        যাবত্ (i < পরিমাণ(uint32_t)) {
            যদি (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (বর্ণ) *(mySource++);
            }
            অন্যথা {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                গচ্ছ donefornow;
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
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    ভঙ্গ;
                }
            }
        }
        অন্যথা {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            ভঙ্গ;
        }
    }

donefornow:
    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ধ্রুব বর্ণ *) mySource;
}

স্থির শূন্য U_CALLCONV
T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    ধ্রুব unsigned বর্ণ *mySource = (unsigned বর্ণ *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    ধ্রুব unsigned বর্ণ *sourceLimit = (unsigned বর্ণ *) args->sourceLimit;
    ধ্রুব char16_t *targetLimit = args->targetLimit;
    unsigned বর্ণ *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_অঙ্ক = 0;

    /* Restore state of current sequence */
    যদি (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        গচ্ছ morebytes;
    }

    যাবত্ (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        যাবত্ (i < পরিমাণ(uint32_t)) {
            যদি (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (বর্ণ) *(mySource++);
            }
            অন্যথা {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                গচ্ছ donefornow;
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
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    ভঙ্গ;
                }
            }
        }
        অন্যথা {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            ভঙ্গ;
        }
        offset_অঙ্ক += i;
    }

donefornow:
    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ধ্রুব বর্ণ *) mySource;
    args->offsets = myOffsets;
}

স্থির শূন্য U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    ধ্রুব char16_t *mySource = args->source;
    unsigned বর্ণ *myTarget;
    ধ্রুব char16_t *sourceLimit = args->sourceLimit;
    ধ্রুব unsigned বর্ণ *targetLimit = (unsigned বর্ণ *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned পূর্ণাঙ্ক indexToWrite;
    unsigned বর্ণ temp[পরিমাণ(uint32_t)];

    যদি(mySource >= sourceLimit) {
        /* no input, nothing to do */
        প্রত্যাগম;
    }

    /* write the BOM if necessary */
    যদি(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        স্থির ধ্রুব বর্ণ bom[]={ 0, 0, (বর্ণ)0xfeu, (বর্ণ)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned বর্ণ *) args->target;
    temp[0] = 0;

    যদি (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        গচ্ছ lowsurogate;
    }

    যাবত্ (mySource < sourceLimit && myTarget < targetLimit) {
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
                        ভঙ্গ;
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
                    ভঙ্গ;
                }
            }
            অন্যথা {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                ভঙ্গ;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[1] = (uint8_t) (ch >> 16 & 0x1F);
        temp[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        প্রতি (indexToWrite = 0; indexToWrite <= পরিমাণ(uint32_t) - 1; indexToWrite++) {
            যদি (myTarget < targetLimit) {
                *(myTarget++) = temp[indexToWrite];
            }
            অন্যথা {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
    }

    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (বর্ণ *) myTarget;
    args->source = mySource;
}

স্থির শূন্য U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    ধ্রুব char16_t *mySource = args->source;
    unsigned বর্ণ *myTarget;
    int32_t *myOffsets;
    ধ্রুব char16_t *sourceLimit = args->sourceLimit;
    ধ্রুব unsigned বর্ণ *targetLimit = (unsigned বর্ণ *) args->targetLimit;
    UChar32 ch, ch2;
    int32_t offset_অঙ্ক = 0;
    unsigned পূর্ণাঙ্ক indexToWrite;
    unsigned বর্ণ temp[পরিমাণ(uint32_t)];

    যদি(mySource >= sourceLimit) {
        /* no input, nothing to do */
        প্রত্যাগম;
    }

    /* write the BOM if necessary */
    যদি(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        স্থির ধ্রুব বর্ণ bom[]={ 0, 0, (বর্ণ)0xfeu, (বর্ণ)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned বর্ণ *) args->target;
    myOffsets = args->offsets;
    temp[0] = 0;

    যদি (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        গচ্ছ lowsurogate;
    }

    যাবত্ (mySource < sourceLimit && myTarget < targetLimit) {
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
                        ভঙ্গ;
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
                    ভঙ্গ;
                }
            }
            অন্যথা {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                ভঙ্গ;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[1] = (uint8_t) (ch >> 16 & 0x1F);
        temp[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        প্রতি (indexToWrite = 0; indexToWrite <= পরিমাণ(uint32_t) - 1; indexToWrite++) {
            যদি (myTarget < targetLimit) {
                *(myTarget++) = temp[indexToWrite];
                *(myOffsets++) = offset_অঙ্ক;
            }
            অন্যথা {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        offset_অঙ্ক = offset_অঙ্ক + 1 + (temp[1] != 0);
    }

    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (বর্ণ *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

স্থির UChar32 U_CALLCONV
T_UConverter_getNextUChar_UTF32_BE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    ধ্রুব uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (ধ্রুব uint8_t *)args->source;
    যদি (mySource >= (ধ্রুব uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম 0xffff;
    }

    length = (int32_t)((ধ্রুব uint8_t *)args->sourceLimit - mySource);
    যদি (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (ধ্রুব বর্ণ *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        প্রত্যাগম 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[0] << 24)
            | ((UChar32)mySource[1] << 16)
            | ((UChar32)mySource[2] << 8)
            | ((UChar32)mySource[3]);

    args->source = (ধ্রুব বর্ণ *)(mySource + 4);
    যদি ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        প্রত্যাগম myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    প্রত্যাগম 0xffff;
}
U_CDECL_END
স্থির ধ্রুব UConverterImpl _UTF32BEImpl = {
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
    T_UConverter_getNextUChar_UTF32_BE,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
স্থির ধ্রুব UConverterStaticData _UTF32BEStaticData = {
    পরিমাণ(UConverterStaticData),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, অসত্য, অসত্য,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ধ্রুব UConverterSharedData _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF32BEStaticData, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
স্থির শূন্য U_CALLCONV
T_UConverter_toUnicode_UTF32_LE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    ধ্রুব unsigned বর্ণ *mySource = (unsigned বর্ণ *) args->source;
    char16_t *myTarget = args->target;
    ধ্রুব unsigned বর্ণ *sourceLimit = (unsigned বর্ণ *) args->sourceLimit;
    ধ্রুব char16_t *targetLimit = args->targetLimit;
    unsigned বর্ণ *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    যদি (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        গচ্ছ morebytes;
    }

    যাবত্ (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        যাবত্ (i < পরিমাণ(uint32_t))
        {
            যদি (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (বর্ণ) *(mySource++);
            }
            অন্যথা
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                গচ্ছ donefornow;
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
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    ভঙ্গ;
                }
            }
        }
        অন্যথা {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            ভঙ্গ;
        }
    }

donefornow:
    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ধ্রুব বর্ণ *) mySource;
}

স্থির শূন্য U_CALLCONV
T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    ধ্রুব unsigned বর্ণ *mySource = (unsigned বর্ণ *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    ধ্রুব unsigned বর্ণ *sourceLimit = (unsigned বর্ণ *) args->sourceLimit;
    ধ্রুব char16_t *targetLimit = args->targetLimit;
    unsigned বর্ণ *toUBytes = args->converter->toUBytes;
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
        গচ্ছ morebytes;
    }

    যাবত্ (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        যাবত্ (i < পরিমাণ(uint32_t))
        {
            যদি (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (বর্ণ) *(mySource++);
            }
            অন্যথা
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                গচ্ছ donefornow;
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
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    ভঙ্গ;
                }
            }
        }
        অন্যথা
        {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            ভঙ্গ;
        }
        offset_অঙ্ক += i;
    }

donefornow:
    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ধ্রুব বর্ণ *) mySource;
    args->offsets = myOffsets;
}

স্থির শূন্য U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    ধ্রুব char16_t *mySource = args->source;
    unsigned বর্ণ *myTarget;
    ধ্রুব char16_t *sourceLimit = args->sourceLimit;
    ধ্রুব unsigned বর্ণ *targetLimit = (unsigned বর্ণ *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned পূর্ণাঙ্ক indexToWrite;
    unsigned বর্ণ temp[পরিমাণ(uint32_t)];

    যদি(mySource >= sourceLimit) {
        /* no input, nothing to do */
        প্রত্যাগম;
    }

    /* write the BOM if necessary */
    যদি(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        স্থির ধ্রুব বর্ণ bom[]={ (বর্ণ)0xffu, (বর্ণ)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned বর্ণ *) args->target;
    temp[3] = 0;

    যদি (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        গচ্ছ lowsurogate;
    }

    যাবত্ (mySource < sourceLimit && myTarget < targetLimit)
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
                        ভঙ্গ;
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
                    ভঙ্গ;
                }
            }
            অন্যথা {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                ভঙ্গ;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[2] = (uint8_t) (ch >> 16 & 0x1F);
        temp[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        প্রতি (indexToWrite = 0; indexToWrite <= পরিমাণ(uint32_t) - 1; indexToWrite++)
        {
            যদি (myTarget < targetLimit)
            {
                *(myTarget++) = temp[indexToWrite];
            }
            অন্যথা
            {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
    }

    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (বর্ণ *) myTarget;
    args->source = mySource;
}

স্থির শূন্য U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    ধ্রুব char16_t *mySource = args->source;
    unsigned বর্ণ *myTarget;
    int32_t *myOffsets;
    ধ্রুব char16_t *sourceLimit = args->sourceLimit;
    ধ্রুব unsigned বর্ণ *targetLimit = (unsigned বর্ণ *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned পূর্ণাঙ্ক indexToWrite;
    unsigned বর্ণ temp[পরিমাণ(uint32_t)];
    int32_t offset_অঙ্ক = 0;

    যদি(mySource >= sourceLimit) {
        /* no input, nothing to do */
        প্রত্যাগম;
    }

    /* write the BOM if necessary */
    যদি(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        স্থির ধ্রুব বর্ণ bom[]={ (বর্ণ)0xffu, (বর্ণ)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned বর্ণ *) args->target;
    myOffsets = args->offsets;
    temp[3] = 0;

    যদি (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        গচ্ছ lowsurogate;
    }

    যাবত্ (mySource < sourceLimit && myTarget < targetLimit)
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
                        ভঙ্গ;
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
                    ভঙ্গ;
                }
            }
            অন্যথা {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                ভঙ্গ;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[2] = (uint8_t) (ch >> 16 & 0x1F);
        temp[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        প্রতি (indexToWrite = 0; indexToWrite <= পরিমাণ(uint32_t) - 1; indexToWrite++)
        {
            যদি (myTarget < targetLimit)
            {
                *(myTarget++) = temp[indexToWrite];
                *(myOffsets++) = offset_অঙ্ক;
            }
            অন্যথা
            {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        offset_অঙ্ক = offset_অঙ্ক + 1 + (temp[2] != 0);
    }

    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (বর্ণ *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

স্থির UChar32 U_CALLCONV
T_UConverter_getNextUChar_UTF32_LE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    ধ্রুব uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (ধ্রুব uint8_t *)args->source;
    যদি (mySource >= (ধ্রুব uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম 0xffff;
    }

    length = (int32_t)((ধ্রুব uint8_t *)args->sourceLimit - mySource);
    যদি (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (ধ্রুব বর্ণ *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        প্রত্যাগম 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[3] << 24)
            | ((UChar32)mySource[2] << 16)
            | ((UChar32)mySource[1] << 8)
            | ((UChar32)mySource[0]);

    args->source = (ধ্রুব বর্ণ *)(mySource + 4);
    যদি ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        প্রত্যাগম myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    প্রত্যাগম 0xffff;
}
U_CDECL_END
স্থির ধ্রুব UConverterImpl _UTF32LEImpl = {
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
    T_UConverter_getNextUChar_UTF32_LE,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
স্থির ধ্রুব UConverterStaticData _UTF32LEStaticData = {
    পরিমাণ(UConverterStaticData),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, অসত্য, অসত্য,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ধ্রুব UConverterSharedData _UTF32LEData =
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF32LEStaticData, &_UTF32LEImpl);

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

স্থির ধ্রুব বর্ণ utf32BOM[8]={ 0, 0, (বর্ণ)0xfeu, (বর্ণ)0xffu, (বর্ণ)0xffu, (বর্ণ)0xfeu, 0, 0 };

স্থির শূন্য U_CALLCONV
_UTF32ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    UConverter *cnv=pArgs->converter;
    ধ্রুব বর্ণ *source=pArgs->source;
    ধ্রুব বর্ণ *sourceLimit=pArgs->sourceLimit;
    int32_t *offsets=pArgs->offsets;

    int32_t state, offsetDelta;
    বর্ণ b;

    state=cnv->mode;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    offsetDelta=0;

    যাবত্(source<sourceLimit && U_SUCCESS(*pErrorCode)) {
        বিকল্প(state) {
        পক্ষ 0:
            b=*source;
            যদি(b==0) {
                state=1; /* could be 00 00 FE FF */
            } অন্যথা যদি(b==(বর্ণ)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } অন্যথা {
                state=8; /* default to UTF-32BE */
                অনুবর্ত;
            }
            ++source;
            ভঙ্গ;
        পক্ষ 1:
        পক্ষ 2:
        পক্ষ 3:
        পক্ষ 5:
        পক্ষ 6:
        পক্ষ 7:
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
                    pArgs->flush=অসত্য; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    pArgs->sourceLimit=sourceLimit;
                    pArgs->flush=oldFlush;
                }
                state=8;
                অনুবর্ত;
            }
            ভঙ্গ;
        পক্ষ 8:
            /* call UTF-32BE */
            pArgs->source=source;
            যদি(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            } অন্যথা {
                T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            ভঙ্গ;
        পক্ষ 9:
            /* call UTF-32LE */
            pArgs->source=source;
            যদি(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            } অন্যথা {
                T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            ভঙ্গ;
        স্বভাব:
            ভঙ্গ; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    যদি(offsets!=nullptr && offsetDelta!=0) {
        int32_t *offsetsLimit=pArgs->offsets;
        যাবত্(offsets<offsetsLimit) {
            *offsets++ += offsetDelta;
        }
    }

    pArgs->source=source;

    যদি(source==sourceLimit && pArgs->flush) {
        /* handle truncated input */
        বিকল্প(state) {
        পক্ষ 0:
            ভঙ্গ; /* no input at all, nothing to do */
        পক্ষ 8:
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            ভঙ্গ;
        পক্ষ 9:
            T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            ভঙ্গ;
        স্বভাব:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
            pArgs->sourceLimit=pArgs->source+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            pArgs->source=source;
            pArgs->sourceLimit=sourceLimit;
            state=8;
            ভঙ্গ;
        }
    }

    cnv->mode=state;
}

স্থির UChar32 U_CALLCONV
_UTF32GetNextUChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    বিকল্প(pArgs->converter->mode) {
    পক্ষ 8:
        প্রত্যাগম T_UConverter_getNextUChar_UTF32_BE(pArgs, pErrorCode);
    পক্ষ 9:
        প্রত্যাগম T_UConverter_getNextUChar_UTF32_LE(pArgs, pErrorCode);
    স্বভাব:
        প্রত্যাগম UCNV_GET_NEXT_UCHAR_USE_TO_U;
    }
}
U_CDECL_END
স্থির ধ্রুব UConverterImpl _UTF32Impl = {
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
    _UTF32GetNextUChar,

    nullptr, /* ### TODO implement getStarters for all Unicode encodings?! */
    nullptr,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

/* The 1236 CCSID refers to any version of Unicode with a BOM sensitive endianness of UTF-32 */
স্থির ধ্রুব UConverterStaticData _UTF32StaticData = {
    পরিমাণ(UConverterStaticData),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#যদি U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#অন্যথা
    { 0xfd, 0xff, 0, 0 }, 4,
#endif
    অসত্য, অসত্য,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ধ্রুব UConverterSharedData _UTF32Data = 
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF32StaticData, &_UTF32Impl);

#endif
