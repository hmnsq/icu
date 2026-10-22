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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#அந்தர்பாவ "unicode/ucnv.h"
#அந்தர்பாவ "unicode/utf.h"
#அந்தர்பாவ "ucnv_bld.h"
#அந்தர்பாவ "ucnv_cnv.h"
#அந்தர்பாவ "cmemory.h"

#define MAXIMUM_UCS2            0x0000FFFF
#define MAXIMUM_UTF             0x0010FFFF
#define HALF_SHIFT              10
#define HALF_BASE               0x0010000
#define HALF_MASK               0x3FF
#define SURROGATE_உயர்வு_START    0xD800
#define SURROGATE_கீழ்_START     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#define SURROGATE_கீழ்_BASE      9216

எண்ணிக்கை {
    UCNV_NEED_TO_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
நிலையான வெறுமை U_CALLCONV
T_UConverter_toUnicode_UTF32_BE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    மாறிலி unsigned எழுத்து *mySource = (unsigned எழுத்து *) args->source;
    char16_t *myTarget = args->target;
    மாறிலி unsigned எழுத்து *sourceLimit = (unsigned எழுத்து *) args->sourceLimit;
    மாறிலி char16_t *targetLimit = args->targetLimit;
    unsigned எழுத்து *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    எனில் (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        செல் morebytes;
    }

    போது (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        போது (i < அளவு(uint32_t)) {
            எனில் (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (எழுத்து) *(mySource++);
            }
            இல்லையெனில் {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                செல் donefornow;
            }
        }

        எனில் (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            எனில் (ch <= MAXIMUM_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            இல்லையெனில் {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                எனில் (myTarget < targetLimit) {
                    *(myTarget++) = (char16_t)ch;
                }
                இல்லையெனில் {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    நிறுத்து;
                }
            }
        }
        இல்லையெனில் {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            நிறுத்து;
        }
    }

donefornow:
    எனில் (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (மாறிலி எழுத்து *) mySource;
}

நிலையான வெறுமை U_CALLCONV
T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    மாறிலி unsigned எழுத்து *mySource = (unsigned எழுத்து *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    மாறிலி unsigned எழுத்து *sourceLimit = (unsigned எழுத்து *) args->sourceLimit;
    மாறிலி char16_t *targetLimit = args->targetLimit;
    unsigned எழுத்து *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_இலக்கம் = 0;

    /* Restore state of current sequence */
    எனில் (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        செல் morebytes;
    }

    போது (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        போது (i < அளவு(uint32_t)) {
            எனில் (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (எழுத்து) *(mySource++);
            }
            இல்லையெனில் {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                செல் donefornow;
            }
        }

        எனில் (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            எனில் (ch <= MAXIMUM_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = offset_இலக்கம்;
            }
            இல்லையெனில் {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                *myOffsets++ = offset_இலக்கம்;
                ch = U16_TRAIL(ch);
                எனில் (myTarget < targetLimit)
                {
                    *(myTarget++) = (char16_t)ch;
                    *(myOffsets++) = offset_இலக்கம்;
                }
                இல்லையெனில் {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    நிறுத்து;
                }
            }
        }
        இல்லையெனில் {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            நிறுத்து;
        }
        offset_இலக்கம் += i;
    }

donefornow:
    எனில் (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (மாறிலி எழுத்து *) mySource;
    args->offsets = myOffsets;
}

நிலையான வெறுமை U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    மாறிலி char16_t *mySource = args->source;
    unsigned எழுத்து *myTarget;
    மாறிலி char16_t *sourceLimit = args->sourceLimit;
    மாறிலி unsigned எழுத்து *targetLimit = (unsigned எழுத்து *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned முழு indexToWrite;
    unsigned எழுத்து temp[அளவு(uint32_t)];

    எனில்(mySource >= sourceLimit) {
        /* no input, nothing to do */
        திரும்பு;
    }

    /* write the BOM if necessary */
    எனில்(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        நிலையான மாறிலி எழுத்து bom[]={ 0, 0, (எழுத்து)0xfeu, (எழுத்து)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned எழுத்து *) args->target;
    temp[0] = 0;

    எனில் (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        செல் lowsurogate;
    }

    போது (mySource < sourceLimit && myTarget < targetLimit) {
        ch = *(mySource++);

        எனில் (U_IS_SURROGATE(ch)) {
            எனில் (U_IS_LEAD(ch)) {
lowsurogate:
                எனில் (mySource < sourceLimit) {
                    ch2 = *mySource;
                    எனில் (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_உயர்வு_START) << HALF_SHIFT) + ch2 + SURROGATE_கீழ்_BASE;
                        mySource++;
                    }
                    இல்லையெனில் {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        நிறுத்து;
                    }
                }
                இல்லையெனில் {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    எனில் (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    நிறுத்து;
                }
            }
            இல்லையெனில் {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                நிறுத்து;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[1] = (uint8_t) (ch >> 16 & 0x1F);
        temp[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        க்கு (indexToWrite = 0; indexToWrite <= அளவு(uint32_t) - 1; indexToWrite++) {
            எனில் (myTarget < targetLimit) {
                *(myTarget++) = temp[indexToWrite];
            }
            இல்லையெனில் {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
    }

    எனில் (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (எழுத்து *) myTarget;
    args->source = mySource;
}

நிலையான வெறுமை U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    மாறிலி char16_t *mySource = args->source;
    unsigned எழுத்து *myTarget;
    int32_t *myOffsets;
    மாறிலி char16_t *sourceLimit = args->sourceLimit;
    மாறிலி unsigned எழுத்து *targetLimit = (unsigned எழுத்து *) args->targetLimit;
    UChar32 ch, ch2;
    int32_t offset_இலக்கம் = 0;
    unsigned முழு indexToWrite;
    unsigned எழுத்து temp[அளவு(uint32_t)];

    எனில்(mySource >= sourceLimit) {
        /* no input, nothing to do */
        திரும்பு;
    }

    /* write the BOM if necessary */
    எனில்(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        நிலையான மாறிலி எழுத்து bom[]={ 0, 0, (எழுத்து)0xfeu, (எழுத்து)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned எழுத்து *) args->target;
    myOffsets = args->offsets;
    temp[0] = 0;

    எனில் (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        செல் lowsurogate;
    }

    போது (mySource < sourceLimit && myTarget < targetLimit) {
        ch = *(mySource++);

        எனில் (U_IS_SURROGATE(ch)) {
            எனில் (U_IS_LEAD(ch)) {
lowsurogate:
                எனில் (mySource < sourceLimit) {
                    ch2 = *mySource;
                    எனில் (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_உயர்வு_START) << HALF_SHIFT) + ch2 + SURROGATE_கீழ்_BASE;
                        mySource++;
                    }
                    இல்லையெனில் {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        நிறுத்து;
                    }
                }
                இல்லையெனில் {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    எனில் (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    நிறுத்து;
                }
            }
            இல்லையெனில் {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                நிறுத்து;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[1] = (uint8_t) (ch >> 16 & 0x1F);
        temp[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        க்கு (indexToWrite = 0; indexToWrite <= அளவு(uint32_t) - 1; indexToWrite++) {
            எனில் (myTarget < targetLimit) {
                *(myTarget++) = temp[indexToWrite];
                *(myOffsets++) = offset_இலக்கம்;
            }
            இல்லையெனில் {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        offset_இலக்கம் = offset_இலக்கம் + 1 + (temp[1] != 0);
    }

    எனில் (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (எழுத்து *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

நிலையான UChar32 U_CALLCONV
T_UConverter_getNextUChar_UTF32_BE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    மாறிலி uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (மாறிலி uint8_t *)args->source;
    எனில் (mySource >= (மாறிலி uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0xffff;
    }

    length = (int32_t)((மாறிலி uint8_t *)args->sourceLimit - mySource);
    எனில் (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (மாறிலி எழுத்து *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        திரும்பு 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[0] << 24)
            | ((UChar32)mySource[1] << 16)
            | ((UChar32)mySource[2] << 8)
            | ((UChar32)mySource[3]);

    args->source = (மாறிலி எழுத்து *)(mySource + 4);
    எனில் ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        திரும்பு myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    திரும்பு 0xffff;
}
U_CDECL_END
நிலையான மாறிலி UConverterImpl _UTF32BEImpl = {
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
நிலையான மாறிலி UConverterStaticData _UTF32BEStaticData = {
    அளவு(UConverterStaticData),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, பொய், பொய்,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

மாறிலி UConverterSharedData _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF32BEStaticData, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
நிலையான வெறுமை U_CALLCONV
T_UConverter_toUnicode_UTF32_LE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    மாறிலி unsigned எழுத்து *mySource = (unsigned எழுத்து *) args->source;
    char16_t *myTarget = args->target;
    மாறிலி unsigned எழுத்து *sourceLimit = (unsigned எழுத்து *) args->sourceLimit;
    மாறிலி char16_t *targetLimit = args->targetLimit;
    unsigned எழுத்து *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    எனில் (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        செல் morebytes;
    }

    போது (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        போது (i < அளவு(uint32_t))
        {
            எனில் (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (எழுத்து) *(mySource++);
            }
            இல்லையெனில்
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                செல் donefornow;
            }
        }

        எனில் (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            எனில் (ch <= MAXIMUM_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            இல்லையெனில் {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                எனில் (myTarget < targetLimit) {
                    *(myTarget++) = (char16_t)ch;
                }
                இல்லையெனில் {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    நிறுத்து;
                }
            }
        }
        இல்லையெனில் {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            நிறுத்து;
        }
    }

donefornow:
    எனில் (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (மாறிலி எழுத்து *) mySource;
}

நிலையான வெறுமை U_CALLCONV
T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    மாறிலி unsigned எழுத்து *mySource = (unsigned எழுத்து *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    மாறிலி unsigned எழுத்து *sourceLimit = (unsigned எழுத்து *) args->sourceLimit;
    மாறிலி char16_t *targetLimit = args->targetLimit;
    unsigned எழுத்து *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_இலக்கம் = 0;

    /* Restore state of current sequence */
    எனில் (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        செல் morebytes;
    }

    போது (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        போது (i < அளவு(uint32_t))
        {
            எனில் (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (எழுத்து) *(mySource++);
            }
            இல்லையெனில்
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                செல் donefornow;
            }
        }

        எனில் (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            எனில் (ch <= MAXIMUM_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = offset_இலக்கம்;
            }
            இல்லையெனில் {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                *(myOffsets++) = offset_இலக்கம்;
                ch = U16_TRAIL(ch);
                எனில் (myTarget < targetLimit)
                {
                    *(myTarget++) = (char16_t)ch;
                    *(myOffsets++) = offset_இலக்கம்;
                }
                இல்லையெனில்
                {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    நிறுத்து;
                }
            }
        }
        இல்லையெனில்
        {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            நிறுத்து;
        }
        offset_இலக்கம் += i;
    }

donefornow:
    எனில் (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (மாறிலி எழுத்து *) mySource;
    args->offsets = myOffsets;
}

நிலையான வெறுமை U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    மாறிலி char16_t *mySource = args->source;
    unsigned எழுத்து *myTarget;
    மாறிலி char16_t *sourceLimit = args->sourceLimit;
    மாறிலி unsigned எழுத்து *targetLimit = (unsigned எழுத்து *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned முழு indexToWrite;
    unsigned எழுத்து temp[அளவு(uint32_t)];

    எனில்(mySource >= sourceLimit) {
        /* no input, nothing to do */
        திரும்பு;
    }

    /* write the BOM if necessary */
    எனில்(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        நிலையான மாறிலி எழுத்து bom[]={ (எழுத்து)0xffu, (எழுத்து)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned எழுத்து *) args->target;
    temp[3] = 0;

    எனில் (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        செல் lowsurogate;
    }

    போது (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        எனில் (U16_IS_SURROGATE(ch)) {
            எனில் (U16_IS_LEAD(ch))
            {
lowsurogate:
                எனில் (mySource < sourceLimit)
                {
                    ch2 = *mySource;
                    எனில் (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_உயர்வு_START) << HALF_SHIFT) + ch2 + SURROGATE_கீழ்_BASE;
                        mySource++;
                    }
                    இல்லையெனில் {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        நிறுத்து;
                    }
                }
                இல்லையெனில் {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    எனில் (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    நிறுத்து;
                }
            }
            இல்லையெனில் {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                நிறுத்து;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[2] = (uint8_t) (ch >> 16 & 0x1F);
        temp[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        க்கு (indexToWrite = 0; indexToWrite <= அளவு(uint32_t) - 1; indexToWrite++)
        {
            எனில் (myTarget < targetLimit)
            {
                *(myTarget++) = temp[indexToWrite];
            }
            இல்லையெனில்
            {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
    }

    எனில் (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (எழுத்து *) myTarget;
    args->source = mySource;
}

நிலையான வெறுமை U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    மாறிலி char16_t *mySource = args->source;
    unsigned எழுத்து *myTarget;
    int32_t *myOffsets;
    மாறிலி char16_t *sourceLimit = args->sourceLimit;
    மாறிலி unsigned எழுத்து *targetLimit = (unsigned எழுத்து *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned முழு indexToWrite;
    unsigned எழுத்து temp[அளவு(uint32_t)];
    int32_t offset_இலக்கம் = 0;

    எனில்(mySource >= sourceLimit) {
        /* no input, nothing to do */
        திரும்பு;
    }

    /* write the BOM if necessary */
    எனில்(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        நிலையான மாறிலி எழுத்து bom[]={ (எழுத்து)0xffu, (எழுத்து)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned எழுத்து *) args->target;
    myOffsets = args->offsets;
    temp[3] = 0;

    எனில் (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        செல் lowsurogate;
    }

    போது (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        எனில் (U16_IS_SURROGATE(ch)) {
            எனில் (U16_IS_LEAD(ch))
            {
lowsurogate:
                எனில் (mySource < sourceLimit)
                {
                    ch2 = *mySource;
                    எனில் (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_உயர்வு_START) << HALF_SHIFT) + ch2 + SURROGATE_கீழ்_BASE;
                        mySource++;
                    }
                    இல்லையெனில் {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        நிறுத்து;
                    }
                }
                இல்லையெனில் {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    எனில் (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    நிறுத்து;
                }
            }
            இல்லையெனில் {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                நிறுத்து;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[2] = (uint8_t) (ch >> 16 & 0x1F);
        temp[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        க்கு (indexToWrite = 0; indexToWrite <= அளவு(uint32_t) - 1; indexToWrite++)
        {
            எனில் (myTarget < targetLimit)
            {
                *(myTarget++) = temp[indexToWrite];
                *(myOffsets++) = offset_இலக்கம்;
            }
            இல்லையெனில்
            {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        offset_இலக்கம் = offset_இலக்கம் + 1 + (temp[2] != 0);
    }

    எனில் (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (எழுத்து *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

நிலையான UChar32 U_CALLCONV
T_UConverter_getNextUChar_UTF32_LE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    மாறிலி uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (மாறிலி uint8_t *)args->source;
    எனில் (mySource >= (மாறிலி uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0xffff;
    }

    length = (int32_t)((மாறிலி uint8_t *)args->sourceLimit - mySource);
    எனில் (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (மாறிலி எழுத்து *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        திரும்பு 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[3] << 24)
            | ((UChar32)mySource[2] << 16)
            | ((UChar32)mySource[1] << 8)
            | ((UChar32)mySource[0]);

    args->source = (மாறிலி எழுத்து *)(mySource + 4);
    எனில் ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        திரும்பு myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    திரும்பு 0xffff;
}
U_CDECL_END
நிலையான மாறிலி UConverterImpl _UTF32LEImpl = {
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
நிலையான மாறிலி UConverterStaticData _UTF32LEStaticData = {
    அளவு(UConverterStaticData),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, பொய், பொய்,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


மாறிலி UConverterSharedData _UTF32LEData =
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
நிலையான வெறுமை U_CALLCONV
_UTF32Reset(UConverter *cnv, UConverterResetChoice choice) {
    எனில்(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->mode=0;
    }
    எனில்(choice!=UCNV_RESET_TO_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}

நிலையான வெறுமை U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverterLoadArgs *pArgs,
           UErrorCode *pErrorCode) {
    (வெறுமை)pArgs;
    (வெறுமை)pErrorCode;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

நிலையான மாறிலி எழுத்து utf32BOM[8]={ 0, 0, (எழுத்து)0xfeu, (எழுத்து)0xffu, (எழுத்து)0xffu, (எழுத்து)0xfeu, 0, 0 };

நிலையான வெறுமை U_CALLCONV
_UTF32ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    UConverter *cnv=pArgs->converter;
    மாறிலி எழுத்து *source=pArgs->source;
    மாறிலி எழுத்து *sourceLimit=pArgs->sourceLimit;
    int32_t *offsets=pArgs->offsets;

    int32_t state, offsetDelta;
    எழுத்து b;

    state=cnv->mode;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    offsetDelta=0;

    போது(source<sourceLimit && U_SUCCESS(*pErrorCode)) {
        தேர்வு(state) {
        நிலை 0:
            b=*source;
            எனில்(b==0) {
                state=1; /* could be 00 00 FE FF */
            } இல்லையெனில் எனில்(b==(எழுத்து)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } இல்லையெனில் {
                state=8; /* default to UTF-32BE */
                தொடர்;
            }
            ++source;
            நிறுத்து;
        நிலை 1:
        நிலை 2:
        நிலை 3:
        நிலை 5:
        நிலை 6:
        நிலை 7:
            எனில்(*source==utf32BOM[state]) {
                ++state;
                ++source;
                எனில்(state==4) {
                    state=8; /* detect UTF-32BE */
                    offsetDelta=(int32_t)(source-pArgs->source);
                } இல்லையெனில் எனில்(state==8) {
                    state=9; /* detect UTF-32LE */
                    offsetDelta=(int32_t)(source-pArgs->source);
                }
            } இல்லையெனில் {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(source-pArgs->source); /* number of bytes from this buffer */

                /* reset the source */
                source=pArgs->source;

                எனில்(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } இல்லையெனில் {
                    UBool oldFlush=pArgs->flush;

                    /* some of the bytes are from a previous buffer, replay those first */
                    pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
                    pArgs->sourceLimit=pArgs->source+((state&3)-count); /* replay previous bytes */
                    pArgs->flush=பொய்; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    pArgs->sourceLimit=sourceLimit;
                    pArgs->flush=oldFlush;
                }
                state=8;
                தொடர்;
            }
            நிறுத்து;
        நிலை 8:
            /* call UTF-32BE */
            pArgs->source=source;
            எனில்(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            } இல்லையெனில் {
                T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            நிறுத்து;
        நிலை 9:
            /* call UTF-32LE */
            pArgs->source=source;
            எனில்(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            } இல்லையெனில் {
                T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            நிறுத்து;
        இயல்பு:
            நிறுத்து; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    எனில்(offsets!=nullptr && offsetDelta!=0) {
        int32_t *offsetsLimit=pArgs->offsets;
        போது(offsets<offsetsLimit) {
            *offsets++ += offsetDelta;
        }
    }

    pArgs->source=source;

    எனில்(source==sourceLimit && pArgs->flush) {
        /* handle truncated input */
        தேர்வு(state) {
        நிலை 0:
            நிறுத்து; /* no input at all, nothing to do */
        நிலை 8:
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            நிறுத்து;
        நிலை 9:
            T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            நிறுத்து;
        இயல்பு:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
            pArgs->sourceLimit=pArgs->source+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            pArgs->source=source;
            pArgs->sourceLimit=sourceLimit;
            state=8;
            நிறுத்து;
        }
    }

    cnv->mode=state;
}

நிலையான UChar32 U_CALLCONV
_UTF32GetNextUChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    தேர்வு(pArgs->converter->mode) {
    நிலை 8:
        திரும்பு T_UConverter_getNextUChar_UTF32_BE(pArgs, pErrorCode);
    நிலை 9:
        திரும்பு T_UConverter_getNextUChar_UTF32_LE(pArgs, pErrorCode);
    இயல்பு:
        திரும்பு UCNV_GET_NEXT_UCHAR_USE_TO_U;
    }
}
U_CDECL_END
நிலையான மாறிலி UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    nullptr,
    nullptr,

    _UTF32Open,
    nullptr,
    _UTF32Reset,

    _UTF32ToUnicodeWithOffsets,
    _UTF32ToUnicodeWithOffsets,
#எனில் U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC,
#இல்லையெனில்
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
நிலையான மாறிலி UConverterStaticData _UTF32StaticData = {
    அளவு(UConverterStaticData),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#எனில் U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#இல்லையெனில்
    { 0xfd, 0xff, 0, 0 }, 4,
#endif
    பொய், பொய்,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

மாறிலி UConverterSharedData _UTF32Data = 
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF32StaticData, &_UTF32Impl);

#endif
