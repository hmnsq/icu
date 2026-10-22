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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

#define MAXIMUM_UCS2            0x0000FFFF
#define MAXIMUM_UTF             0x0010FFFF
#define HALF_SHIFT              10
#define HALF_BASE               0x0010000
#define HALF_MASK               0x3FF
#define SURROGATE_ಮೇಲಿನ_START    0xD800
#define SURROGATE_ಕೆಳಗಿನ_START     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#define SURROGATE_ಕೆಳಗಿನ_BASE      9216

ಗಣನಾ {
    UCNV_NEED_TO_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_toUnicode_UTF32_BE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    ಧ್ರುವ unsigned ವರ್ಣ *mySource = (unsigned ವರ್ಣ *) args->source;
    char16_t *myTarget = args->target;
    ಧ್ರುವ unsigned ವರ್ಣ *sourceLimit = (unsigned ವರ್ಣ *) args->sourceLimit;
    ಧ್ರುವ char16_t *targetLimit = args->targetLimit;
    unsigned ವರ್ಣ *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    ಯದಿ (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        ಗಚ್ಛ morebytes;
    }

    ಯಾವತ್ (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        ಯಾವತ್ (i < ಪರಿಮಾಣ(uint32_t)) {
            ಯದಿ (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (ವರ್ಣ) *(mySource++);
            }
            ಅನ್ಯಥಾ {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                ಗಚ್ಛ donefornow;
            }
        }

        ಯದಿ (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ಯದಿ (ch <= MAXIMUM_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            ಅನ್ಯಥಾ {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                ಯದಿ (myTarget < targetLimit) {
                    *(myTarget++) = (char16_t)ch;
                }
                ಅನ್ಯಥಾ {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    ಭಙ್ಗ;
                }
            }
        }
        ಅನ್ಯಥಾ {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            ಭಙ್ಗ;
        }
    }

donefornow:
    ಯದಿ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ಧ್ರುವ ವರ್ಣ *) mySource;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    ಧ್ರುವ unsigned ವರ್ಣ *mySource = (unsigned ವರ್ಣ *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    ಧ್ರುವ unsigned ವರ್ಣ *sourceLimit = (unsigned ವರ್ಣ *) args->sourceLimit;
    ಧ್ರುವ char16_t *targetLimit = args->targetLimit;
    unsigned ವರ್ಣ *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_ಅಂಕೆ = 0;

    /* Restore state of current sequence */
    ಯದಿ (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        ಗಚ್ಛ morebytes;
    }

    ಯಾವತ್ (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        ಯಾವತ್ (i < ಪರಿಮಾಣ(uint32_t)) {
            ಯದಿ (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (ವರ್ಣ) *(mySource++);
            }
            ಅನ್ಯಥಾ {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                ಗಚ್ಛ donefornow;
            }
        }

        ಯದಿ (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ಯದಿ (ch <= MAXIMUM_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = offset_ಅಂಕೆ;
            }
            ಅನ್ಯಥಾ {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                *myOffsets++ = offset_ಅಂಕೆ;
                ch = U16_TRAIL(ch);
                ಯದಿ (myTarget < targetLimit)
                {
                    *(myTarget++) = (char16_t)ch;
                    *(myOffsets++) = offset_ಅಂಕೆ;
                }
                ಅನ್ಯಥಾ {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    ಭಙ್ಗ;
                }
            }
        }
        ಅನ್ಯಥಾ {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            ಭಙ್ಗ;
        }
        offset_ಅಂಕೆ += i;
    }

donefornow:
    ಯದಿ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ಧ್ರುವ ವರ್ಣ *) mySource;
    args->offsets = myOffsets;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    ಧ್ರುವ char16_t *mySource = args->source;
    unsigned ವರ್ಣ *myTarget;
    ಧ್ರುವ char16_t *sourceLimit = args->sourceLimit;
    ಧ್ರುವ unsigned ವರ್ಣ *targetLimit = (unsigned ವರ್ಣ *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned ಪೂರ್ಣಾಙ್ಕ indexToWrite;
    unsigned ವರ್ಣ temp[ಪರಿಮಾಣ(uint32_t)];

    ಯದಿ(mySource >= sourceLimit) {
        /* no input, nothing to do */
        ಪ್ರತ್ಯಾಗಮ;
    }

    /* write the BOM if necessary */
    ಯದಿ(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ bom[]={ 0, 0, (ವರ್ಣ)0xfeu, (ವರ್ಣ)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned ವರ್ಣ *) args->target;
    temp[0] = 0;

    ಯದಿ (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        ಗಚ್ಛ lowsurogate;
    }

    ಯಾವತ್ (mySource < sourceLimit && myTarget < targetLimit) {
        ch = *(mySource++);

        ಯದಿ (U_IS_SURROGATE(ch)) {
            ಯದಿ (U_IS_LEAD(ch)) {
lowsurogate:
                ಯದಿ (mySource < sourceLimit) {
                    ch2 = *mySource;
                    ಯದಿ (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ಮೇಲಿನ_START) << HALF_SHIFT) + ch2 + SURROGATE_ಕೆಳಗಿನ_BASE;
                        mySource++;
                    }
                    ಅನ್ಯಥಾ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        ಭಙ್ಗ;
                    }
                }
                ಅನ್ಯಥಾ {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    ಯದಿ (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    ಭಙ್ಗ;
                }
            }
            ಅನ್ಯಥಾ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                ಭಙ್ಗ;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[1] = (uint8_t) (ch >> 16 & 0x1F);
        temp[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        ಪ್ರತಿ (indexToWrite = 0; indexToWrite <= ಪರಿಮಾಣ(uint32_t) - 1; indexToWrite++) {
            ಯದಿ (myTarget < targetLimit) {
                *(myTarget++) = temp[indexToWrite];
            }
            ಅನ್ಯಥಾ {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
    }

    ಯದಿ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (ವರ್ಣ *) myTarget;
    args->source = mySource;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    ಧ್ರುವ char16_t *mySource = args->source;
    unsigned ವರ್ಣ *myTarget;
    int32_t *myOffsets;
    ಧ್ರುವ char16_t *sourceLimit = args->sourceLimit;
    ಧ್ರುವ unsigned ವರ್ಣ *targetLimit = (unsigned ವರ್ಣ *) args->targetLimit;
    UChar32 ch, ch2;
    int32_t offset_ಅಂಕೆ = 0;
    unsigned ಪೂರ್ಣಾಙ್ಕ indexToWrite;
    unsigned ವರ್ಣ temp[ಪರಿಮಾಣ(uint32_t)];

    ಯದಿ(mySource >= sourceLimit) {
        /* no input, nothing to do */
        ಪ್ರತ್ಯಾಗಮ;
    }

    /* write the BOM if necessary */
    ಯದಿ(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ bom[]={ 0, 0, (ವರ್ಣ)0xfeu, (ವರ್ಣ)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned ವರ್ಣ *) args->target;
    myOffsets = args->offsets;
    temp[0] = 0;

    ಯದಿ (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        ಗಚ್ಛ lowsurogate;
    }

    ಯಾವತ್ (mySource < sourceLimit && myTarget < targetLimit) {
        ch = *(mySource++);

        ಯದಿ (U_IS_SURROGATE(ch)) {
            ಯದಿ (U_IS_LEAD(ch)) {
lowsurogate:
                ಯದಿ (mySource < sourceLimit) {
                    ch2 = *mySource;
                    ಯದಿ (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ಮೇಲಿನ_START) << HALF_SHIFT) + ch2 + SURROGATE_ಕೆಳಗಿನ_BASE;
                        mySource++;
                    }
                    ಅನ್ಯಥಾ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        ಭಙ್ಗ;
                    }
                }
                ಅನ್ಯಥಾ {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    ಯದಿ (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    ಭಙ್ಗ;
                }
            }
            ಅನ್ಯಥಾ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                ಭಙ್ಗ;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[1] = (uint8_t) (ch >> 16 & 0x1F);
        temp[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        ಪ್ರತಿ (indexToWrite = 0; indexToWrite <= ಪರಿಮಾಣ(uint32_t) - 1; indexToWrite++) {
            ಯದಿ (myTarget < targetLimit) {
                *(myTarget++) = temp[indexToWrite];
                *(myOffsets++) = offset_ಅಂಕೆ;
            }
            ಅನ್ಯಥಾ {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        offset_ಅಂಕೆ = offset_ಅಂಕೆ + 1 + (temp[1] != 0);
    }

    ಯದಿ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (ವರ್ಣ *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

ಸ್ಥಿರ UChar32 U_CALLCONV
T_UConverter_getNextUChar_UTF32_BE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    ಧ್ರುವ uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (ಧ್ರುವ uint8_t *)args->source;
    ಯದಿ (mySource >= (ಧ್ರುವ uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0xffff;
    }

    length = (int32_t)((ಧ್ರುವ uint8_t *)args->sourceLimit - mySource);
    ಯದಿ (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (ಧ್ರುವ ವರ್ಣ *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        ಪ್ರತ್ಯಾಗಮ 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[0] << 24)
            | ((UChar32)mySource[1] << 16)
            | ((UChar32)mySource[2] << 8)
            | ((UChar32)mySource[3]);

    args->source = (ಧ್ರುವ ವರ್ಣ *)(mySource + 4);
    ಯದಿ ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        ಪ್ರತ್ಯಾಗಮ myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    ಪ್ರತ್ಯಾಗಮ 0xffff;
}
U_CDECL_END
ಸ್ಥಿರ ಧ್ರುವ UConverterImpl _UTF32BEImpl = {
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
ಸ್ಥಿರ ಧ್ರುವ UConverterStaticData _UTF32BEStaticData = {
    ಪರಿಮಾಣ(UConverterStaticData),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, ಅಸತ್ಯ, ಅಸತ್ಯ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ಧ್ರುವ UConverterSharedData _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF32BEStaticData, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_toUnicode_UTF32_LE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    ಧ್ರುವ unsigned ವರ್ಣ *mySource = (unsigned ವರ್ಣ *) args->source;
    char16_t *myTarget = args->target;
    ಧ್ರುವ unsigned ವರ್ಣ *sourceLimit = (unsigned ವರ್ಣ *) args->sourceLimit;
    ಧ್ರುವ char16_t *targetLimit = args->targetLimit;
    unsigned ವರ್ಣ *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    ಯದಿ (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        ಗಚ್ಛ morebytes;
    }

    ಯಾವತ್ (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        ಯಾವತ್ (i < ಪರಿಮಾಣ(uint32_t))
        {
            ಯದಿ (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (ವರ್ಣ) *(mySource++);
            }
            ಅನ್ಯಥಾ
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                ಗಚ್ಛ donefornow;
            }
        }

        ಯದಿ (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ಯದಿ (ch <= MAXIMUM_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            ಅನ್ಯಥಾ {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                ಯದಿ (myTarget < targetLimit) {
                    *(myTarget++) = (char16_t)ch;
                }
                ಅನ್ಯಥಾ {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    ಭಙ್ಗ;
                }
            }
        }
        ಅನ್ಯಥಾ {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            ಭಙ್ಗ;
        }
    }

donefornow:
    ಯದಿ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ಧ್ರುವ ವರ್ಣ *) mySource;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    ಧ್ರುವ unsigned ವರ್ಣ *mySource = (unsigned ವರ್ಣ *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    ಧ್ರುವ unsigned ವರ್ಣ *sourceLimit = (unsigned ವರ್ಣ *) args->sourceLimit;
    ಧ್ರುವ char16_t *targetLimit = args->targetLimit;
    unsigned ವರ್ಣ *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_ಅಂಕೆ = 0;

    /* Restore state of current sequence */
    ಯದಿ (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        ಗಚ್ಛ morebytes;
    }

    ಯಾವತ್ (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        ಯಾವತ್ (i < ಪರಿಮಾಣ(uint32_t))
        {
            ಯದಿ (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (ವರ್ಣ) *(mySource++);
            }
            ಅನ್ಯಥಾ
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                ಗಚ್ಛ donefornow;
            }
        }

        ಯದಿ (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ಯದಿ (ch <= MAXIMUM_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = offset_ಅಂಕೆ;
            }
            ಅನ್ಯಥಾ {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                *(myOffsets++) = offset_ಅಂಕೆ;
                ch = U16_TRAIL(ch);
                ಯದಿ (myTarget < targetLimit)
                {
                    *(myTarget++) = (char16_t)ch;
                    *(myOffsets++) = offset_ಅಂಕೆ;
                }
                ಅನ್ಯಥಾ
                {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    ಭಙ್ಗ;
                }
            }
        }
        ಅನ್ಯಥಾ
        {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            ಭಙ್ಗ;
        }
        offset_ಅಂಕೆ += i;
    }

donefornow:
    ಯದಿ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ಧ್ರುವ ವರ್ಣ *) mySource;
    args->offsets = myOffsets;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    ಧ್ರುವ char16_t *mySource = args->source;
    unsigned ವರ್ಣ *myTarget;
    ಧ್ರುವ char16_t *sourceLimit = args->sourceLimit;
    ಧ್ರುವ unsigned ವರ್ಣ *targetLimit = (unsigned ವರ್ಣ *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned ಪೂರ್ಣಾಙ್ಕ indexToWrite;
    unsigned ವರ್ಣ temp[ಪರಿಮಾಣ(uint32_t)];

    ಯದಿ(mySource >= sourceLimit) {
        /* no input, nothing to do */
        ಪ್ರತ್ಯಾಗಮ;
    }

    /* write the BOM if necessary */
    ಯದಿ(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ bom[]={ (ವರ್ಣ)0xffu, (ವರ್ಣ)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned ವರ್ಣ *) args->target;
    temp[3] = 0;

    ಯದಿ (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        ಗಚ್ಛ lowsurogate;
    }

    ಯಾವತ್ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        ಯದಿ (U16_IS_SURROGATE(ch)) {
            ಯದಿ (U16_IS_LEAD(ch))
            {
lowsurogate:
                ಯದಿ (mySource < sourceLimit)
                {
                    ch2 = *mySource;
                    ಯದಿ (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ಮೇಲಿನ_START) << HALF_SHIFT) + ch2 + SURROGATE_ಕೆಳಗಿನ_BASE;
                        mySource++;
                    }
                    ಅನ್ಯಥಾ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        ಭಙ್ಗ;
                    }
                }
                ಅನ್ಯಥಾ {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    ಯದಿ (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    ಭಙ್ಗ;
                }
            }
            ಅನ್ಯಥಾ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                ಭಙ್ಗ;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[2] = (uint8_t) (ch >> 16 & 0x1F);
        temp[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        ಪ್ರತಿ (indexToWrite = 0; indexToWrite <= ಪರಿಮಾಣ(uint32_t) - 1; indexToWrite++)
        {
            ಯದಿ (myTarget < targetLimit)
            {
                *(myTarget++) = temp[indexToWrite];
            }
            ಅನ್ಯಥಾ
            {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
    }

    ಯದಿ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (ವರ್ಣ *) myTarget;
    args->source = mySource;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    ಧ್ರುವ char16_t *mySource = args->source;
    unsigned ವರ್ಣ *myTarget;
    int32_t *myOffsets;
    ಧ್ರುವ char16_t *sourceLimit = args->sourceLimit;
    ಧ್ರುವ unsigned ವರ್ಣ *targetLimit = (unsigned ವರ್ಣ *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned ಪೂರ್ಣಾಙ್ಕ indexToWrite;
    unsigned ವರ್ಣ temp[ಪರಿಮಾಣ(uint32_t)];
    int32_t offset_ಅಂಕೆ = 0;

    ಯದಿ(mySource >= sourceLimit) {
        /* no input, nothing to do */
        ಪ್ರತ್ಯಾಗಮ;
    }

    /* write the BOM if necessary */
    ಯದಿ(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ bom[]={ (ವರ್ಣ)0xffu, (ವರ್ಣ)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned ವರ್ಣ *) args->target;
    myOffsets = args->offsets;
    temp[3] = 0;

    ಯದಿ (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        ಗಚ್ಛ lowsurogate;
    }

    ಯಾವತ್ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        ಯದಿ (U16_IS_SURROGATE(ch)) {
            ಯದಿ (U16_IS_LEAD(ch))
            {
lowsurogate:
                ಯದಿ (mySource < sourceLimit)
                {
                    ch2 = *mySource;
                    ಯದಿ (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_ಮೇಲಿನ_START) << HALF_SHIFT) + ch2 + SURROGATE_ಕೆಳಗಿನ_BASE;
                        mySource++;
                    }
                    ಅನ್ಯಥಾ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        ಭಙ್ಗ;
                    }
                }
                ಅನ್ಯಥಾ {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    ಯದಿ (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    ಭಙ್ಗ;
                }
            }
            ಅನ್ಯಥಾ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                ಭಙ್ಗ;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[2] = (uint8_t) (ch >> 16 & 0x1F);
        temp[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        ಪ್ರತಿ (indexToWrite = 0; indexToWrite <= ಪರಿಮಾಣ(uint32_t) - 1; indexToWrite++)
        {
            ಯದಿ (myTarget < targetLimit)
            {
                *(myTarget++) = temp[indexToWrite];
                *(myOffsets++) = offset_ಅಂಕೆ;
            }
            ಅನ್ಯಥಾ
            {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        offset_ಅಂಕೆ = offset_ಅಂಕೆ + 1 + (temp[2] != 0);
    }

    ಯದಿ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (ವರ್ಣ *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

ಸ್ಥಿರ UChar32 U_CALLCONV
T_UConverter_getNextUChar_UTF32_LE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    ಧ್ರುವ uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (ಧ್ರುವ uint8_t *)args->source;
    ಯದಿ (mySource >= (ಧ್ರುವ uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0xffff;
    }

    length = (int32_t)((ಧ್ರುವ uint8_t *)args->sourceLimit - mySource);
    ಯದಿ (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (ಧ್ರುವ ವರ್ಣ *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        ಪ್ರತ್ಯಾಗಮ 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[3] << 24)
            | ((UChar32)mySource[2] << 16)
            | ((UChar32)mySource[1] << 8)
            | ((UChar32)mySource[0]);

    args->source = (ಧ್ರುವ ವರ್ಣ *)(mySource + 4);
    ಯದಿ ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        ಪ್ರತ್ಯಾಗಮ myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    ಪ್ರತ್ಯಾಗಮ 0xffff;
}
U_CDECL_END
ಸ್ಥಿರ ಧ್ರುವ UConverterImpl _UTF32LEImpl = {
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
ಸ್ಥಿರ ಧ್ರುವ UConverterStaticData _UTF32LEStaticData = {
    ಪರಿಮಾಣ(UConverterStaticData),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, ಅಸತ್ಯ, ಅಸತ್ಯ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ಧ್ರುವ UConverterSharedData _UTF32LEData =
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
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_UTF32Reset(UConverter *cnv, UConverterResetChoice choice) {
    ಯದಿ(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->mode=0;
    }
    ಯದಿ(choice!=UCNV_RESET_TO_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverterLoadArgs *pArgs,
           UErrorCode *pErrorCode) {
    (ಶೂನ್ಯ)pArgs;
    (ಶೂನ್ಯ)pErrorCode;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ utf32BOM[8]={ 0, 0, (ವರ್ಣ)0xfeu, (ವರ್ಣ)0xffu, (ವರ್ಣ)0xffu, (ವರ್ಣ)0xfeu, 0, 0 };

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_UTF32ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    UConverter *cnv=pArgs->converter;
    ಧ್ರುವ ವರ್ಣ *source=pArgs->source;
    ಧ್ರುವ ವರ್ಣ *sourceLimit=pArgs->sourceLimit;
    int32_t *offsets=pArgs->offsets;

    int32_t state, offsetDelta;
    ವರ್ಣ b;

    state=cnv->mode;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    offsetDelta=0;

    ಯಾವತ್(source<sourceLimit && U_SUCCESS(*pErrorCode)) {
        ವಿಕಲ್ಪ(state) {
        ಪಕ್ಷ 0:
            b=*source;
            ಯದಿ(b==0) {
                state=1; /* could be 00 00 FE FF */
            } ಅನ್ಯಥಾ ಯದಿ(b==(ವರ್ಣ)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } ಅನ್ಯಥಾ {
                state=8; /* default to UTF-32BE */
                ಅನುವರ್ತ;
            }
            ++source;
            ಭಙ್ಗ;
        ಪಕ್ಷ 1:
        ಪಕ್ಷ 2:
        ಪಕ್ಷ 3:
        ಪಕ್ಷ 5:
        ಪಕ್ಷ 6:
        ಪಕ್ಷ 7:
            ಯದಿ(*source==utf32BOM[state]) {
                ++state;
                ++source;
                ಯದಿ(state==4) {
                    state=8; /* detect UTF-32BE */
                    offsetDelta=(int32_t)(source-pArgs->source);
                } ಅನ್ಯಥಾ ಯದಿ(state==8) {
                    state=9; /* detect UTF-32LE */
                    offsetDelta=(int32_t)(source-pArgs->source);
                }
            } ಅನ್ಯಥಾ {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(source-pArgs->source); /* number of bytes from this buffer */

                /* reset the source */
                source=pArgs->source;

                ಯದಿ(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } ಅನ್ಯಥಾ {
                    UBool oldFlush=pArgs->flush;

                    /* some of the bytes are from a previous buffer, replay those first */
                    pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
                    pArgs->sourceLimit=pArgs->source+((state&3)-count); /* replay previous bytes */
                    pArgs->flush=ಅಸತ್ಯ; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    pArgs->sourceLimit=sourceLimit;
                    pArgs->flush=oldFlush;
                }
                state=8;
                ಅನುವರ್ತ;
            }
            ಭಙ್ಗ;
        ಪಕ್ಷ 8:
            /* call UTF-32BE */
            pArgs->source=source;
            ಯದಿ(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            } ಅನ್ಯಥಾ {
                T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            ಭಙ್ಗ;
        ಪಕ್ಷ 9:
            /* call UTF-32LE */
            pArgs->source=source;
            ಯದಿ(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            } ಅನ್ಯಥಾ {
                T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            ಭಙ್ಗ; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    ಯದಿ(offsets!=nullptr && offsetDelta!=0) {
        int32_t *offsetsLimit=pArgs->offsets;
        ಯಾವತ್(offsets<offsetsLimit) {
            *offsets++ += offsetDelta;
        }
    }

    pArgs->source=source;

    ಯದಿ(source==sourceLimit && pArgs->flush) {
        /* handle truncated input */
        ವಿಕಲ್ಪ(state) {
        ಪಕ್ಷ 0:
            ಭಙ್ಗ; /* no input at all, nothing to do */
        ಪಕ್ಷ 8:
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            ಭಙ್ಗ;
        ಪಕ್ಷ 9:
            T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
            pArgs->sourceLimit=pArgs->source+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            pArgs->source=source;
            pArgs->sourceLimit=sourceLimit;
            state=8;
            ಭಙ್ಗ;
        }
    }

    cnv->mode=state;
}

ಸ್ಥಿರ UChar32 U_CALLCONV
_UTF32GetNextUChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    ವಿಕಲ್ಪ(pArgs->converter->mode) {
    ಪಕ್ಷ 8:
        ಪ್ರತ್ಯಾಗಮ T_UConverter_getNextUChar_UTF32_BE(pArgs, pErrorCode);
    ಪಕ್ಷ 9:
        ಪ್ರತ್ಯಾಗಮ T_UConverter_getNextUChar_UTF32_LE(pArgs, pErrorCode);
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ UCNV_GET_NEXT_UCHAR_USE_TO_U;
    }
}
U_CDECL_END
ಸ್ಥಿರ ಧ್ರುವ UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    nullptr,
    nullptr,

    _UTF32Open,
    nullptr,
    _UTF32Reset,

    _UTF32ToUnicodeWithOffsets,
    _UTF32ToUnicodeWithOffsets,
#ಯದಿ U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC,
#ಅನ್ಯಥಾ
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
ಸ್ಥಿರ ಧ್ರುವ UConverterStaticData _UTF32StaticData = {
    ಪರಿಮಾಣ(UConverterStaticData),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#ಯದಿ U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#ಅನ್ಯಥಾ
    { 0xfd, 0xff, 0, 0 }, 4,
#endif
    ಅಸತ್ಯ, ಅಸತ್ಯ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ಧ್ರುವ UConverterSharedData _UTF32Data = 
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF32StaticData, &_UTF32Impl);

#endif
