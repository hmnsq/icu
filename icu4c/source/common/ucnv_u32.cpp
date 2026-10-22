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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#شامل "unicode/ucnv.h"
#شامل "unicode/utf.h"
#شامل "ucnv_bld.h"
#شامل "ucnv_cnv.h"
#شامل "cmemory.h"

#define MAXIMUM_UCS2            0x0000FFFF
#define MAXIMUM_UTF             0x0010FFFF
#define HALF_SHIFT              10
#define HALF_BASE               0x0010000
#define HALF_MASK               0x3FF
#define SURROGATE_بالا_START    0xD800
#define SURROGATE_کم_START     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#define SURROGATE_کم_BASE      9216

شمارش {
    UCNV_NEED_TO_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ایستا تهی U_CALLCONV
T_UConverter_toUnicode_UTF32_BE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    ثابت unsigned نویسه *mySource = (unsigned نویسه *) args->source;
    char16_t *myTarget = args->target;
    ثابت unsigned نویسه *sourceLimit = (unsigned نویسه *) args->sourceLimit;
    ثابت char16_t *targetLimit = args->targetLimit;
    unsigned نویسه *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    اگر (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        برو morebytes;
    }

    وقتی (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        وقتی (i < اندازه(uint32_t)) {
            اگر (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (نویسه) *(mySource++);
            }
            وگرنه {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                برو donefornow;
            }
        }

        اگر (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            اگر (ch <= MAXIMUM_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            وگرنه {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                اگر (myTarget < targetLimit) {
                    *(myTarget++) = (char16_t)ch;
                }
                وگرنه {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    بشکن;
                }
            }
        }
        وگرنه {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            بشکن;
        }
    }

donefornow:
    اگر (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ثابت نویسه *) mySource;
}

ایستا تهی U_CALLCONV
T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    ثابت unsigned نویسه *mySource = (unsigned نویسه *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    ثابت unsigned نویسه *sourceLimit = (unsigned نویسه *) args->sourceLimit;
    ثابت char16_t *targetLimit = args->targetLimit;
    unsigned نویسه *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_رقم = 0;

    /* Restore state of current sequence */
    اگر (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        برو morebytes;
    }

    وقتی (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        وقتی (i < اندازه(uint32_t)) {
            اگر (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (نویسه) *(mySource++);
            }
            وگرنه {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                برو donefornow;
            }
        }

        اگر (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            اگر (ch <= MAXIMUM_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = offset_رقم;
            }
            وگرنه {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                *myOffsets++ = offset_رقم;
                ch = U16_TRAIL(ch);
                اگر (myTarget < targetLimit)
                {
                    *(myTarget++) = (char16_t)ch;
                    *(myOffsets++) = offset_رقم;
                }
                وگرنه {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    بشکن;
                }
            }
        }
        وگرنه {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            بشکن;
        }
        offset_رقم += i;
    }

donefornow:
    اگر (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ثابت نویسه *) mySource;
    args->offsets = myOffsets;
}

ایستا تهی U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    ثابت char16_t *mySource = args->source;
    unsigned نویسه *myTarget;
    ثابت char16_t *sourceLimit = args->sourceLimit;
    ثابت unsigned نویسه *targetLimit = (unsigned نویسه *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned صحیح indexToWrite;
    unsigned نویسه temp[اندازه(uint32_t)];

    اگر(mySource >= sourceLimit) {
        /* no input, nothing to do */
        بازگشت;
    }

    /* write the BOM if necessary */
    اگر(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        ایستا ثابت نویسه bom[]={ 0, 0, (نویسه)0xfeu, (نویسه)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned نویسه *) args->target;
    temp[0] = 0;

    اگر (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        برو lowsurogate;
    }

    وقتی (mySource < sourceLimit && myTarget < targetLimit) {
        ch = *(mySource++);

        اگر (U_IS_SURROGATE(ch)) {
            اگر (U_IS_LEAD(ch)) {
lowsurogate:
                اگر (mySource < sourceLimit) {
                    ch2 = *mySource;
                    اگر (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_بالا_START) << HALF_SHIFT) + ch2 + SURROGATE_کم_BASE;
                        mySource++;
                    }
                    وگرنه {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        بشکن;
                    }
                }
                وگرنه {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    اگر (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    بشکن;
                }
            }
            وگرنه {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                بشکن;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[1] = (uint8_t) (ch >> 16 & 0x1F);
        temp[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        برای (indexToWrite = 0; indexToWrite <= اندازه(uint32_t) - 1; indexToWrite++) {
            اگر (myTarget < targetLimit) {
                *(myTarget++) = temp[indexToWrite];
            }
            وگرنه {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
    }

    اگر (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (نویسه *) myTarget;
    args->source = mySource;
}

ایستا تهی U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    ثابت char16_t *mySource = args->source;
    unsigned نویسه *myTarget;
    int32_t *myOffsets;
    ثابت char16_t *sourceLimit = args->sourceLimit;
    ثابت unsigned نویسه *targetLimit = (unsigned نویسه *) args->targetLimit;
    UChar32 ch, ch2;
    int32_t offset_رقم = 0;
    unsigned صحیح indexToWrite;
    unsigned نویسه temp[اندازه(uint32_t)];

    اگر(mySource >= sourceLimit) {
        /* no input, nothing to do */
        بازگشت;
    }

    /* write the BOM if necessary */
    اگر(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        ایستا ثابت نویسه bom[]={ 0, 0, (نویسه)0xfeu, (نویسه)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned نویسه *) args->target;
    myOffsets = args->offsets;
    temp[0] = 0;

    اگر (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        برو lowsurogate;
    }

    وقتی (mySource < sourceLimit && myTarget < targetLimit) {
        ch = *(mySource++);

        اگر (U_IS_SURROGATE(ch)) {
            اگر (U_IS_LEAD(ch)) {
lowsurogate:
                اگر (mySource < sourceLimit) {
                    ch2 = *mySource;
                    اگر (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_بالا_START) << HALF_SHIFT) + ch2 + SURROGATE_کم_BASE;
                        mySource++;
                    }
                    وگرنه {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        بشکن;
                    }
                }
                وگرنه {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    اگر (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    بشکن;
                }
            }
            وگرنه {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                بشکن;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[1] = (uint8_t) (ch >> 16 & 0x1F);
        temp[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        برای (indexToWrite = 0; indexToWrite <= اندازه(uint32_t) - 1; indexToWrite++) {
            اگر (myTarget < targetLimit) {
                *(myTarget++) = temp[indexToWrite];
                *(myOffsets++) = offset_رقم;
            }
            وگرنه {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        offset_رقم = offset_رقم + 1 + (temp[1] != 0);
    }

    اگر (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (نویسه *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

ایستا UChar32 U_CALLCONV
T_UConverter_getNextUChar_UTF32_BE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    ثابت uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (ثابت uint8_t *)args->source;
    اگر (mySource >= (ثابت uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت 0xffff;
    }

    length = (int32_t)((ثابت uint8_t *)args->sourceLimit - mySource);
    اگر (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (ثابت نویسه *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        بازگشت 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[0] << 24)
            | ((UChar32)mySource[1] << 16)
            | ((UChar32)mySource[2] << 8)
            | ((UChar32)mySource[3]);

    args->source = (ثابت نویسه *)(mySource + 4);
    اگر ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        بازگشت myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    بازگشت 0xffff;
}
U_CDECL_END
ایستا ثابت UConverterImpl _UTF32BEImpl = {
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
ایستا ثابت UConverterStaticData _UTF32BEStaticData = {
    اندازه(UConverterStaticData),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, نادرست, نادرست,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverterSharedData _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF32BEStaticData, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
ایستا تهی U_CALLCONV
T_UConverter_toUnicode_UTF32_LE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    ثابت unsigned نویسه *mySource = (unsigned نویسه *) args->source;
    char16_t *myTarget = args->target;
    ثابت unsigned نویسه *sourceLimit = (unsigned نویسه *) args->sourceLimit;
    ثابت char16_t *targetLimit = args->targetLimit;
    unsigned نویسه *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    اگر (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        برو morebytes;
    }

    وقتی (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        وقتی (i < اندازه(uint32_t))
        {
            اگر (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (نویسه) *(mySource++);
            }
            وگرنه
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                برو donefornow;
            }
        }

        اگر (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            اگر (ch <= MAXIMUM_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            وگرنه {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                اگر (myTarget < targetLimit) {
                    *(myTarget++) = (char16_t)ch;
                }
                وگرنه {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    بشکن;
                }
            }
        }
        وگرنه {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            بشکن;
        }
    }

donefornow:
    اگر (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ثابت نویسه *) mySource;
}

ایستا تهی U_CALLCONV
T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    ثابت unsigned نویسه *mySource = (unsigned نویسه *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    ثابت unsigned نویسه *sourceLimit = (unsigned نویسه *) args->sourceLimit;
    ثابت char16_t *targetLimit = args->targetLimit;
    unsigned نویسه *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_رقم = 0;

    /* Restore state of current sequence */
    اگر (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        برو morebytes;
    }

    وقتی (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        وقتی (i < اندازه(uint32_t))
        {
            اگر (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (نویسه) *(mySource++);
            }
            وگرنه
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                برو donefornow;
            }
        }

        اگر (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            اگر (ch <= MAXIMUM_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = offset_رقم;
            }
            وگرنه {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                *(myOffsets++) = offset_رقم;
                ch = U16_TRAIL(ch);
                اگر (myTarget < targetLimit)
                {
                    *(myTarget++) = (char16_t)ch;
                    *(myOffsets++) = offset_رقم;
                }
                وگرنه
                {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    بشکن;
                }
            }
        }
        وگرنه
        {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            بشکن;
        }
        offset_رقم += i;
    }

donefornow:
    اگر (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ثابت نویسه *) mySource;
    args->offsets = myOffsets;
}

ایستا تهی U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    ثابت char16_t *mySource = args->source;
    unsigned نویسه *myTarget;
    ثابت char16_t *sourceLimit = args->sourceLimit;
    ثابت unsigned نویسه *targetLimit = (unsigned نویسه *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned صحیح indexToWrite;
    unsigned نویسه temp[اندازه(uint32_t)];

    اگر(mySource >= sourceLimit) {
        /* no input, nothing to do */
        بازگشت;
    }

    /* write the BOM if necessary */
    اگر(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        ایستا ثابت نویسه bom[]={ (نویسه)0xffu, (نویسه)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned نویسه *) args->target;
    temp[3] = 0;

    اگر (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        برو lowsurogate;
    }

    وقتی (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        اگر (U16_IS_SURROGATE(ch)) {
            اگر (U16_IS_LEAD(ch))
            {
lowsurogate:
                اگر (mySource < sourceLimit)
                {
                    ch2 = *mySource;
                    اگر (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_بالا_START) << HALF_SHIFT) + ch2 + SURROGATE_کم_BASE;
                        mySource++;
                    }
                    وگرنه {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        بشکن;
                    }
                }
                وگرنه {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    اگر (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    بشکن;
                }
            }
            وگرنه {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                بشکن;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[2] = (uint8_t) (ch >> 16 & 0x1F);
        temp[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        برای (indexToWrite = 0; indexToWrite <= اندازه(uint32_t) - 1; indexToWrite++)
        {
            اگر (myTarget < targetLimit)
            {
                *(myTarget++) = temp[indexToWrite];
            }
            وگرنه
            {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
    }

    اگر (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (نویسه *) myTarget;
    args->source = mySource;
}

ایستا تهی U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    ثابت char16_t *mySource = args->source;
    unsigned نویسه *myTarget;
    int32_t *myOffsets;
    ثابت char16_t *sourceLimit = args->sourceLimit;
    ثابت unsigned نویسه *targetLimit = (unsigned نویسه *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned صحیح indexToWrite;
    unsigned نویسه temp[اندازه(uint32_t)];
    int32_t offset_رقم = 0;

    اگر(mySource >= sourceLimit) {
        /* no input, nothing to do */
        بازگشت;
    }

    /* write the BOM if necessary */
    اگر(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        ایستا ثابت نویسه bom[]={ (نویسه)0xffu, (نویسه)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned نویسه *) args->target;
    myOffsets = args->offsets;
    temp[3] = 0;

    اگر (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        برو lowsurogate;
    }

    وقتی (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        اگر (U16_IS_SURROGATE(ch)) {
            اگر (U16_IS_LEAD(ch))
            {
lowsurogate:
                اگر (mySource < sourceLimit)
                {
                    ch2 = *mySource;
                    اگر (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_بالا_START) << HALF_SHIFT) + ch2 + SURROGATE_کم_BASE;
                        mySource++;
                    }
                    وگرنه {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        بشکن;
                    }
                }
                وگرنه {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    اگر (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    بشکن;
                }
            }
            وگرنه {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                بشکن;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[2] = (uint8_t) (ch >> 16 & 0x1F);
        temp[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        برای (indexToWrite = 0; indexToWrite <= اندازه(uint32_t) - 1; indexToWrite++)
        {
            اگر (myTarget < targetLimit)
            {
                *(myTarget++) = temp[indexToWrite];
                *(myOffsets++) = offset_رقم;
            }
            وگرنه
            {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        offset_رقم = offset_رقم + 1 + (temp[2] != 0);
    }

    اگر (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (نویسه *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

ایستا UChar32 U_CALLCONV
T_UConverter_getNextUChar_UTF32_LE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    ثابت uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (ثابت uint8_t *)args->source;
    اگر (mySource >= (ثابت uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت 0xffff;
    }

    length = (int32_t)((ثابت uint8_t *)args->sourceLimit - mySource);
    اگر (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (ثابت نویسه *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        بازگشت 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[3] << 24)
            | ((UChar32)mySource[2] << 16)
            | ((UChar32)mySource[1] << 8)
            | ((UChar32)mySource[0]);

    args->source = (ثابت نویسه *)(mySource + 4);
    اگر ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        بازگشت myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    بازگشت 0xffff;
}
U_CDECL_END
ایستا ثابت UConverterImpl _UTF32LEImpl = {
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
ایستا ثابت UConverterStaticData _UTF32LEStaticData = {
    اندازه(UConverterStaticData),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, نادرست, نادرست,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverterSharedData _UTF32LEData =
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
ایستا تهی U_CALLCONV
_UTF32Reset(UConverter *cnv, UConverterResetChoice choice) {
    اگر(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->mode=0;
    }
    اگر(choice!=UCNV_RESET_TO_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}

ایستا تهی U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverterLoadArgs *pArgs,
           UErrorCode *pErrorCode) {
    (تهی)pArgs;
    (تهی)pErrorCode;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

ایستا ثابت نویسه utf32BOM[8]={ 0, 0, (نویسه)0xfeu, (نویسه)0xffu, (نویسه)0xffu, (نویسه)0xfeu, 0, 0 };

ایستا تهی U_CALLCONV
_UTF32ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    UConverter *cnv=pArgs->converter;
    ثابت نویسه *source=pArgs->source;
    ثابت نویسه *sourceLimit=pArgs->sourceLimit;
    int32_t *offsets=pArgs->offsets;

    int32_t state, offsetDelta;
    نویسه b;

    state=cnv->mode;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    offsetDelta=0;

    وقتی(source<sourceLimit && U_SUCCESS(*pErrorCode)) {
        انتخاب(state) {
        حالت 0:
            b=*source;
            اگر(b==0) {
                state=1; /* could be 00 00 FE FF */
            } وگرنه اگر(b==(نویسه)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } وگرنه {
                state=8; /* default to UTF-32BE */
                ادامه;
            }
            ++source;
            بشکن;
        حالت 1:
        حالت 2:
        حالت 3:
        حالت 5:
        حالت 6:
        حالت 7:
            اگر(*source==utf32BOM[state]) {
                ++state;
                ++source;
                اگر(state==4) {
                    state=8; /* detect UTF-32BE */
                    offsetDelta=(int32_t)(source-pArgs->source);
                } وگرنه اگر(state==8) {
                    state=9; /* detect UTF-32LE */
                    offsetDelta=(int32_t)(source-pArgs->source);
                }
            } وگرنه {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(source-pArgs->source); /* number of bytes from this buffer */

                /* reset the source */
                source=pArgs->source;

                اگر(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } وگرنه {
                    UBool oldFlush=pArgs->flush;

                    /* some of the bytes are from a previous buffer, replay those first */
                    pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
                    pArgs->sourceLimit=pArgs->source+((state&3)-count); /* replay previous bytes */
                    pArgs->flush=نادرست; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    pArgs->sourceLimit=sourceLimit;
                    pArgs->flush=oldFlush;
                }
                state=8;
                ادامه;
            }
            بشکن;
        حالت 8:
            /* call UTF-32BE */
            pArgs->source=source;
            اگر(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            } وگرنه {
                T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            بشکن;
        حالت 9:
            /* call UTF-32LE */
            pArgs->source=source;
            اگر(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            } وگرنه {
                T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            بشکن;
        پیشفرض:
            بشکن; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    اگر(offsets!=nullptr && offsetDelta!=0) {
        int32_t *offsetsLimit=pArgs->offsets;
        وقتی(offsets<offsetsLimit) {
            *offsets++ += offsetDelta;
        }
    }

    pArgs->source=source;

    اگر(source==sourceLimit && pArgs->flush) {
        /* handle truncated input */
        انتخاب(state) {
        حالت 0:
            بشکن; /* no input at all, nothing to do */
        حالت 8:
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            بشکن;
        حالت 9:
            T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            بشکن;
        پیشفرض:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
            pArgs->sourceLimit=pArgs->source+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            pArgs->source=source;
            pArgs->sourceLimit=sourceLimit;
            state=8;
            بشکن;
        }
    }

    cnv->mode=state;
}

ایستا UChar32 U_CALLCONV
_UTF32GetNextUChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    انتخاب(pArgs->converter->mode) {
    حالت 8:
        بازگشت T_UConverter_getNextUChar_UTF32_BE(pArgs, pErrorCode);
    حالت 9:
        بازگشت T_UConverter_getNextUChar_UTF32_LE(pArgs, pErrorCode);
    پیشفرض:
        بازگشت UCNV_GET_NEXT_UCHAR_USE_TO_U;
    }
}
U_CDECL_END
ایستا ثابت UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    nullptr,
    nullptr,

    _UTF32Open,
    nullptr,
    _UTF32Reset,

    _UTF32ToUnicodeWithOffsets,
    _UTF32ToUnicodeWithOffsets,
#اگر U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC,
#وگرنه
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
ایستا ثابت UConverterStaticData _UTF32StaticData = {
    اندازه(UConverterStaticData),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#اگر U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#وگرنه
    { 0xfd, 0xff, 0, 0 }, 4,
#endif
    نادرست, نادرست,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverterSharedData _UTF32Data = 
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF32StaticData, &_UTF32Impl);

#endif
