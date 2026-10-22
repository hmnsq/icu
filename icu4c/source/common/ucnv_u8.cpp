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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_CONVERSION

#அந்தர்பாவ "unicode/ucnv.h"
#அந்தர்பாவ "unicode/utf.h"
#அந்தர்பாவ "unicode/utf8.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ucnv_bld.h"
#அந்தர்பாவ "ucnv_cnv.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "ustr_imp.h"

/* Prototypes --------------------------------------------------------------- */

/* Keep these here to make finicky compilers happy */

U_CFUNC வெறுமை ucnv_fromUnicode_UTF8(UConverterFromUnicodeArgs *args,
                                           UErrorCode *err);
U_CFUNC வெறுமை ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverterFromUnicodeArgs *args,
                                                        UErrorCode *err);


/* UTF-8 -------------------------------------------------------------------- */

#define MAXIMUM_UCS2            0x0000FFFF

நிலையான மாறிலி uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

நிலையான UBool hasCESU8Data(மாறிலி UConverter *cnv)
{
#எனில் UCONFIG_ONLY_HTML_CONVERSION
    திரும்பு பொய்;
#இல்லையெனில்
    திரும்பு cnv->sharedData == &_CESU8Data;
#endif
}
U_CDECL_BEGIN
நிலையான வெறுமை  U_CALLCONV ucnv_toUnicode_UTF8 (UConverterToUnicodeArgs * args,
                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    மாறிலி unsigned எழுத்து *mySource = (unsigned எழுத்து *) args->source;
    char16_t *myTarget = args->target;
    மாறிலி unsigned எழுத்து *sourceLimit = (unsigned எழுத்து *) args->sourceLimit;
    மாறிலி char16_t *targetLimit = args->targetLimit;
    unsigned எழுத்து *toUBytes = cnv->toUBytes;
    UBool isCESU8 = hasCESU8Data(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    எனில் (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        செல் morebytes;
    }


    போது (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        எனில் (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
        }
        இல்லையெனில்
        {
            /* store the first char */
            toUBytes[0] = (எழுத்து)ch;
            inBytes = U8_மொத்தம்_BYTES_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            போது (i < inBytes)
            {
                எனில் (mySource < sourceLimit)
                {
                    toUBytes[i] = (எழுத்து) (ch2 = *mySource);
                    எனில் (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        நிறுத்து; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                இல்லையெனில்
                {
                    /* stores a partially calculated target*/
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t) i;
                    செல் donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            எனில் (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                எனில் (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                }
                இல்லையெனில்
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    எனில் (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                    }
                    இல்லையெனில்
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UCharErrorBuffer[0] = (char16_t) ch;
                        cnv->UCharErrorBufferLength = 1;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                        நிறுத்து;
                    }
                }
            }
            இல்லையெனில்
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                நிறுத்து;
            }
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

நிலையான வெறுமை  U_CALLCONV ucnv_toUnicode_UTF8_OFFSETS_LOGIC (UConverterToUnicodeArgs * args,
                                                UErrorCode * err)
{
    UConverter *cnv = args->converter;
    மாறிலி unsigned எழுத்து *mySource = (unsigned எழுத்து *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    int32_t offset_இலக்கம் = 0;
    மாறிலி unsigned எழுத்து *sourceLimit = (unsigned எழுத்து *) args->sourceLimit;
    மாறிலி char16_t *targetLimit = args->targetLimit;
    unsigned எழுத்து *toUBytes = cnv->toUBytes;
    UBool isCESU8 = hasCESU8Data(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    எனில் (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        செல் morebytes;
    }

    போது (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        எனில் (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
            *(myOffsets++) = offset_இலக்கம்++;
        }
        இல்லையெனில்
        {
            toUBytes[0] = (எழுத்து)ch;
            inBytes = U8_மொத்தம்_BYTES_NON_ASCII(ch);
            i = 1;

morebytes:
            போது (i < inBytes)
            {
                எனில் (mySource < sourceLimit)
                {
                    toUBytes[i] = (எழுத்து) (ch2 = *mySource);
                    எனில் (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        நிறுத்து; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                இல்லையெனில்
                {
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t)i;
                    செல் donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            எனில் (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                எனில் (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                    *(myOffsets++) = offset_இலக்கம்;
                }
                இல்லையெனில்
                {
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
                        cnv->UCharErrorBuffer[0] = (char16_t) ch;
                        cnv->UCharErrorBufferLength = 1;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
                offset_இலக்கம் += i;
            }
            இல்லையெனில்
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                நிறுத்து;
            }
        }
    }

donefornow:
    எனில் (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (மாறிலி எழுத்து *) mySource;
    args->offsets = myOffsets;
}
U_CDECL_END

U_CFUNC வெறுமை  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverterFromUnicodeArgs * args,
                                    UErrorCode * err)
{
    UConverter *cnv = args->converter;
    மாறிலி char16_t *mySource = args->source;
    மாறிலி char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    மாறிலி uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *tempPtr;
    UChar32 ch;
    uint8_t tempBuf[4];
    int32_t indexToWrite;
    UBool isNotCESU8 = !hasCESU8Data(cnv);

    எனில் (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        செல் lowsurrogate;
    }

    போது (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        எனில் (ch < 0x80)        /* Single byte */
        {
            *(myTarget++) = (uint8_t) ch;
        }
        இல்லையெனில் எனில் (ch < 0x800)  /* Double byte */
        {
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            எனில் (myTarget < targetLimit)
            {
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            இல்லையெனில்
            {
                cnv->charErrorBuffer[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->charErrorBufferLength = 1;
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        இல்லையெனில் {
            /* Check for surrogates */
            எனில்(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                எனில் (mySource < sourceLimit) {
                    /* test both code units */
                    எனில்(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        /* exit this condition tree */
                    }
                    இல்லையெனில் {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        நிறுத்து;
                    }
                }
                இல்லையெனில் {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    நிறுத்து;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            tempPtr = (((targetLimit - myTarget) >= 4) ? myTarget : tempBuf);

            எனில் (ch <= MAXIMUM_UCS2) {
                indexToWrite = 2;
                tempPtr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            இல்லையெனில் {
                indexToWrite = 3;
                tempPtr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                tempPtr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            tempPtr[indexToWrite-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            tempPtr[indexToWrite] = (uint8_t) ((ch & 0x3f) | 0x80);

            எனில் (tempPtr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (indexToWrite + 1);
            }
            இல்லையெனில் {
                /* We might run out of room soon. Write it slowly. */
                க்கு (; tempPtr <= (tempBuf + indexToWrite); tempPtr++) {
                    எனில் (myTarget < targetLimit) {
                        *(myTarget++) = *tempPtr;
                    }
                    இல்லையெனில் {
                        cnv->charErrorBuffer[cnv->charErrorBufferLength++] = *tempPtr;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
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

U_CFUNC வெறுமை  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverterFromUnicodeArgs * args,
                                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    மாறிலி char16_t *mySource = args->source;
    int32_t *myOffsets = args->offsets;
    மாறிலி char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    மாறிலி uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *tempPtr;
    UChar32 ch;
    int32_t offset_இலக்கம், nextSourceIndex;
    int32_t indexToWrite;
    uint8_t tempBuf[4];
    UBool isNotCESU8 = !hasCESU8Data(cnv);

    எனில் (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        offset_இலக்கம் = -1;
        nextSourceIndex = 0;
        செல் lowsurrogate;
    } இல்லையெனில் {
        offset_இலக்கம் = 0;
    }

    போது (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        எனில் (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = offset_இலக்கம்++;
            *(myTarget++) = (எழுத்து) ch;
        }
        இல்லையெனில் எனில் (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = offset_இலக்கம்;
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            எனில் (myTarget < targetLimit)
            {
                *(myOffsets++) = offset_இலக்கம்++;
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            இல்லையெனில்
            {
                cnv->charErrorBuffer[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->charErrorBufferLength = 1;
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        இல்லையெனில்
        /* Check for surrogates */
        {
            nextSourceIndex = offset_இலக்கம் + 1;

            எனில்(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                எனில் (mySource < sourceLimit) {
                    /* test both code units */
                    எனில்(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        ++nextSourceIndex;
                        /* exit this condition tree */
                    }
                    இல்லையெனில் {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        நிறுத்து;
                    }
                }
                இல்லையெனில் {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    நிறுத்து;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            tempPtr = (((targetLimit - myTarget) >= 4) ? myTarget : tempBuf);

            எனில் (ch <= MAXIMUM_UCS2) {
                indexToWrite = 2;
                tempPtr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            இல்லையெனில் {
                indexToWrite = 3;
                tempPtr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                tempPtr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            tempPtr[indexToWrite-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            tempPtr[indexToWrite] = (uint8_t) ((ch & 0x3f) | 0x80);

            எனில் (tempPtr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (indexToWrite + 1);
                myOffsets[0] = offset_இலக்கம்;
                myOffsets[1] = offset_இலக்கம்;
                myOffsets[2] = offset_இலக்கம்;
                எனில் (indexToWrite >= 3) {
                    myOffsets[3] = offset_இலக்கம்;
                }
                myOffsets += (indexToWrite + 1);
            }
            இல்லையெனில் {
                /* We might run out of room soon. Write it slowly. */
                க்கு (; tempPtr <= (tempBuf + indexToWrite); tempPtr++) {
                    எனில் (myTarget < targetLimit)
                    {
                        *(myOffsets++) = offset_இலக்கம்;
                        *(myTarget++) = *tempPtr;
                    }
                    இல்லையெனில்
                    {
                        cnv->charErrorBuffer[cnv->charErrorBufferLength++] = *tempPtr;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
            }
            offset_இலக்கம் = nextSourceIndex;
        }
    }

    எனில் (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (எழுத்து *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

U_CDECL_BEGIN
நிலையான UChar32 U_CALLCONV ucnv_getNextUChar_UTF8(UConverterToUnicodeArgs *args,
                                               UErrorCode *err) {
    UConverter *cnv;
    மாறிலி uint8_t *sourceInitial;
    மாறிலி uint8_t *source;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = args->converter;
    sourceInitial = source = (மாறிலி uint8_t *)args->source;
    எனில் (source >= (மாறிலி uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0xffff;
    }

    myByte = *(source++);
    எனில் (U8_IS_SINGLE(myByte))
    {
        args->source = (மாறிலி எழுத்து *)source;
        திரும்பு (UChar32)myByte;
    }

    uint16_t மொத்தம்_Trail_Bytes = U8_மொத்தம்_TRAIL_BYTES(myByte);
    எனில் (மொத்தம்_Trail_Bytes == 0) {
        cnv->toUBytes[0] = myByte;
        cnv->toULength = 1;
        *err = U_ILLEGAL_CHAR_FOUND;
        args->source = (மாறிலி எழுத்து *)source;
        திரும்பு 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    எனில் (((மாறிலி எழுத்து *)source + மொத்தம்_Trail_Bytes) > args->sourceLimit)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extraBytesToWrite = மொத்தம்_Trail_Bytes + 1;
        cnv->toUBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_FOUND;
        போது(source < (மாறிலி uint8_t *)args->sourceLimit) {
            uint8_t b = *source;
            எனில்(icu::UTF8::isValidTrail(myByte, b, i, extraBytesToWrite)) {
                cnv->toUBytes[i++] = b;
                ++source;
            } இல்லையெனில் {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_FOUND;
                நிறுத்து;
            }
        }
        cnv->toULength = i;
        args->source = (மாறிலி எழுத்து *)source;
        திரும்பு 0xffff;
    }

    ch = myByte << 6;
    எனில்(மொத்தம்_Trail_Bytes == 2) {
        uint8_t t1 = *source, t2;
        எனில்(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source)) {
            args->source = (மாறிலி எழுத்து *)(source + 1);
            திரும்பு (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } இல்லையெனில் எனில்(மொத்தம்_Trail_Bytes == 1) {
        uint8_t t1 = *source;
        எனில்(U8_IS_TRAIL(t1)) {
            args->source = (மாறிலி எழுத்து *)(source + 1);
            திரும்பு (ch + t1) - offsetsFromUTF8[2];
        }
    } இல்லையெனில் {  // countTrailBytes == 3
        uint8_t t1 = *source, t2, t3;
        எனில்(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source) &&
                U8_IS_TRAIL(t3 = *++source)) {
            args->source = (மாறிலி எழுத்து *)(source + 1);
            திரும்பு (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    args->source = (மாறிலி எழுத்து *)source;

    க்கு(i = 0; sourceInitial < source; ++i) {
        cnv->toUBytes[i] = *sourceInitial++;
    }
    cnv->toULength = i;
    *err = U_ILLEGAL_CHAR_FOUND;
    திரும்பு 0xffff;
} 
U_CDECL_END

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
நிலையான வெறுமை U_CALLCONV
ucnv_UTF8FromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                  UConverterToUnicodeArgs *pToUArgs,
                  UErrorCode *pErrorCode) {
    UConverter *utf8;
    மாறிலி uint8_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity;
    int32_t count;

    int8_t oldToULength, toULength, toULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    /* set up the local pointers */
    utf8=pToUArgs->converter;
    source=(uint8_t *)pToUArgs->source;
    sourceLimit=(uint8_t *)pToUArgs->sourceLimit;
    target=(uint8_t *)pFromUArgs->target;
    targetCapacity=(int32_t)(pFromUArgs->targetLimit-pFromUArgs->target);

    /* get the converter state from the UTF-8 UConverter */
    எனில்(utf8->toULength > 0) {
        toULength=oldToULength=utf8->toULength;
        toULimit=(int8_t)utf8->mode;
        c=(UChar32)utf8->toUnicodeStatus;
    } இல்லையெனில் {
        toULength=oldToULength=toULimit=0;
        c = 0;
    }

    count=(int32_t)(sourceLimit-source)+oldToULength;
    எனில்(count<toULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } இல்லையெனில் எனில்(targetCapacity<toULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *pErrorCode=U_USING_DEFAULT_WARNING;
        திரும்பு;
    } இல்லையெனில் {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        எனில்(count>targetCapacity) {
            count=targetCapacity;
        }

        // The conversion loop checks count>0 only once per character.
        // If the buffer ends with a truncated sequence,
        // then we reduce the count to stop before that,
        // and collect the remaining bytes after the conversion loop.

        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length=count-toULength;
        U8_TRUNCATE_IF_INCOMPLETE(source, 0, length);
        count=toULength+length;
    }

    எனில்(c!=0) {
        utf8->toUnicodeStatus=0;
        utf8->toULength=0;
        செல் moreBytes;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    போது(count>0) {
        b=*source++;
        எனில்(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *target++=b;
            --count;
            தொடர்;
        } இல்லையெனில் {
            எனில்(b>=0xe0) {
                எனில்( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=source[0]) &&
                    U8_IS_TRAIL(t2=source[1])
                ) {
                    source+=2;
                    *target++=b;
                    *target++=t1;
                    *target++=t2;
                    count-=3;
                    தொடர்;
                }
            } இல்லையெனில் {
                எனில்( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*source)
                ) {
                    ++source;
                    *target++=b;
                    *target++=t1;
                    count-=2;
                    தொடர்;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            oldToULength=0;
            toULength=1;
            toULimit=U8_மொத்தம்_BYTES_NON_ASCII(b);
            c=b;
moreBytes:
            போது(toULength<toULimit) {
                எனில்(source<sourceLimit) {
                    b=*source;
                    எனில்(icu::UTF8::isValidTrail(c, b, toULength, toULimit)) {
                        ++source;
                        ++toULength;
                        c=(c<<6)+b;
                    } இல்லையெனில் {
                        நிறுத்து; /* sequence too short, stop with toULength<toULimit */
                    }
                } இல்லையெனில் {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    source-=(toULength-oldToULength);
                    போது(oldToULength<toULength) {
                        utf8->toUBytes[oldToULength++]=*source++;
                    }
                    utf8->toUnicodeStatus=c;
                    utf8->toULength=toULength;
                    utf8->mode=toULimit;
                    pToUArgs->source=(எழுத்து *)source;
                    pFromUArgs->target=(எழுத்து *)target;
                    திரும்பு;
                }
            }

            எனில்(toULength!=toULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                source-=(toULength-oldToULength);
                போது(oldToULength<toULength) {
                    utf8->toUBytes[oldToULength++]=*source++;
                }
                utf8->toULength=toULength;
                pToUArgs->source=(எழுத்து *)source;
                pFromUArgs->target=(எழுத்து *)target;
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                திரும்பு;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                க்கு(i=0; i<oldToULength; ++i) {
                    *target++=utf8->toUBytes[i];
                }
                source-=(toULength-oldToULength);
                க்கு(; i<toULength; ++i) {
                    *target++=*source++;
                }
                count-=toULength;
            }
        }
    }
    U_ASSERT(count>=0);

    எனில்(U_SUCCESS(*pErrorCode) && source<sourceLimit) {
        எனில்(target==(மாறிலி uint8_t *)pFromUArgs->targetLimit) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        } இல்லையெனில் {
            b=*source;
            toULimit=U8_மொத்தம்_BYTES(b);
            எனில்(toULimit>(sourceLimit-source)) {
                /* collect a truncated byte sequence */
                toULength=0;
                c=b;
                க்கு(;;) {
                    utf8->toUBytes[toULength++]=b;
                    எனில்(++source==sourceLimit) {
                        /* partial byte sequence at end of source */
                        utf8->toUnicodeStatus=c;
                        utf8->toULength=toULength;
                        utf8->mode=toULimit;
                        நிறுத்து;
                    } இல்லையெனில் எனில்(!icu::UTF8::isValidTrail(c, b=*source, toULength, toULimit)) {
                        utf8->toULength=toULength;
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        நிறுத்து;
                    }
                    c=(c<<6)+b;
                }
            } இல்லையெனில் {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *pErrorCode=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    pToUArgs->source=(எழுத்து *)source;
    pFromUArgs->target=(எழுத்து *)target;
}

U_CDECL_END

/* UTF-8 converter data ----------------------------------------------------- */

நிலையான மாறிலி UConverterImpl _UTF8Impl={
    UCNV_UTF8,

    nullptr,
    nullptr,

    nullptr,
    nullptr,
    nullptr,

    ucnv_toUnicode_UTF8,
    ucnv_toUnicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    ucnv_getNextUChar_UTF8,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    ucnv_UTF8FromUTF8,
    ucnv_UTF8FromUTF8
};

/* The 1208 CCSID refers to any version of Unicode of UTF-8 */
நிலையான மாறிலி UConverterStaticData _UTF8StaticData={
    அளவு(UConverterStaticData),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,பொய்,பொய்,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


மாறிலி UConverterSharedData _UTF8Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF8StaticData, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

நிலையான மாறிலி UConverterImpl _CESU8Impl={
    UCNV_CESU8,

    nullptr,
    nullptr,

    nullptr,
    nullptr,
    nullptr,

    ucnv_toUnicode_UTF8,
    ucnv_toUnicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    nullptr,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    ucnv_getCompleteUnicodeSet,

    nullptr,
    nullptr
};

நிலையான மாறிலி UConverterStaticData _CESU8StaticData={
    அளவு(UConverterStaticData),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,பொய்,பொய்,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


மாறிலி UConverterSharedData _CESU8Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_CESU8StaticData, &_CESU8Impl);

#endif
