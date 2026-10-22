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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

/* Prototypes --------------------------------------------------------------- */

/* Keep these here to make finicky compilers happy */

U_CFUNC ಶೂನ್ಯ ucnv_fromUnicode_UTF8(UConverterFromUnicodeArgs *args,
                                           UErrorCode *err);
U_CFUNC ಶೂನ್ಯ ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverterFromUnicodeArgs *args,
                                                        UErrorCode *err);


/* UTF-8 -------------------------------------------------------------------- */

#define MAXIMUM_UCS2            0x0000FFFF

ಸ್ಥಿರ ಧ್ರುವ uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

ಸ್ಥಿರ UBool hasCESU8Data(ಧ್ರುವ UConverter *cnv)
{
#ಯದಿ UCONFIG_ONLY_HTML_CONVERSION
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ cnv->sharedData == &_CESU8Data;
#endif
}
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV ucnv_toUnicode_UTF8 (UConverterToUnicodeArgs * args,
                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    ಧ್ರುವ unsigned ವರ್ಣ *mySource = (unsigned ವರ್ಣ *) args->source;
    char16_t *myTarget = args->target;
    ಧ್ರುವ unsigned ವರ್ಣ *sourceLimit = (unsigned ವರ್ಣ *) args->sourceLimit;
    ಧ್ರುವ char16_t *targetLimit = args->targetLimit;
    unsigned ವರ್ಣ *toUBytes = cnv->toUBytes;
    UBool isCESU8 = hasCESU8Data(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    ಯದಿ (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        ಗಚ್ಛ morebytes;
    }


    ಯಾವತ್ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        ಯದಿ (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
        }
        ಅನ್ಯಥಾ
        {
            /* store the first char */
            toUBytes[0] = (ವರ್ಣ)ch;
            inBytes = U8_ಎಣಿಕೆ_BYTES_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            ಯಾವತ್ (i < inBytes)
            {
                ಯದಿ (mySource < sourceLimit)
                {
                    toUBytes[i] = (ವರ್ಣ) (ch2 = *mySource);
                    ಯದಿ (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        ಭಙ್ಗ; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                ಅನ್ಯಥಾ
                {
                    /* stores a partially calculated target*/
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t) i;
                    ಗಚ್ಛ donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            ಯದಿ (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                ಯದಿ (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                }
                ಅನ್ಯಥಾ
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    ಯದಿ (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                    }
                    ಅನ್ಯಥಾ
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UCharErrorBuffer[0] = (char16_t) ch;
                        cnv->UCharErrorBufferLength = 1;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                        ಭಙ್ಗ;
                    }
                }
            }
            ಅನ್ಯಥಾ
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                ಭಙ್ಗ;
            }
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

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV ucnv_toUnicode_UTF8_OFFSETS_LOGIC (UConverterToUnicodeArgs * args,
                                                UErrorCode * err)
{
    UConverter *cnv = args->converter;
    ಧ್ರುವ unsigned ವರ್ಣ *mySource = (unsigned ವರ್ಣ *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    int32_t offset_ಅಂಕೆ = 0;
    ಧ್ರುವ unsigned ವರ್ಣ *sourceLimit = (unsigned ವರ್ಣ *) args->sourceLimit;
    ಧ್ರುವ char16_t *targetLimit = args->targetLimit;
    unsigned ವರ್ಣ *toUBytes = cnv->toUBytes;
    UBool isCESU8 = hasCESU8Data(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    ಯದಿ (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        ಗಚ್ಛ morebytes;
    }

    ಯಾವತ್ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        ಯದಿ (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
            *(myOffsets++) = offset_ಅಂಕೆ++;
        }
        ಅನ್ಯಥಾ
        {
            toUBytes[0] = (ವರ್ಣ)ch;
            inBytes = U8_ಎಣಿಕೆ_BYTES_NON_ASCII(ch);
            i = 1;

morebytes:
            ಯಾವತ್ (i < inBytes)
            {
                ಯದಿ (mySource < sourceLimit)
                {
                    toUBytes[i] = (ವರ್ಣ) (ch2 = *mySource);
                    ಯದಿ (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        ಭಙ್ಗ; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                ಅನ್ಯಥಾ
                {
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t)i;
                    ಗಚ್ಛ donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            ಯದಿ (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                ಯದಿ (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                    *(myOffsets++) = offset_ಅಂಕೆ;
                }
                ಅನ್ಯಥಾ
                {
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
                        cnv->UCharErrorBuffer[0] = (char16_t) ch;
                        cnv->UCharErrorBufferLength = 1;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
                offset_ಅಂಕೆ += i;
            }
            ಅನ್ಯಥಾ
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                ಭಙ್ಗ;
            }
        }
    }

donefornow:
    ಯದಿ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ಧ್ರುವ ವರ್ಣ *) mySource;
    args->offsets = myOffsets;
}
U_CDECL_END

U_CFUNC ಶೂನ್ಯ  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverterFromUnicodeArgs * args,
                                    UErrorCode * err)
{
    UConverter *cnv = args->converter;
    ಧ್ರುವ char16_t *mySource = args->source;
    ಧ್ರುವ char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    ಧ್ರುವ uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *tempPtr;
    UChar32 ch;
    uint8_t tempBuf[4];
    int32_t indexToWrite;
    UBool isNotCESU8 = !hasCESU8Data(cnv);

    ಯದಿ (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        ಗಚ್ಛ lowsurrogate;
    }

    ಯಾವತ್ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        ಯದಿ (ch < 0x80)        /* Single byte */
        {
            *(myTarget++) = (uint8_t) ch;
        }
        ಅನ್ಯಥಾ ಯದಿ (ch < 0x800)  /* Double byte */
        {
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            ಯದಿ (myTarget < targetLimit)
            {
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            ಅನ್ಯಥಾ
            {
                cnv->charErrorBuffer[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->charErrorBufferLength = 1;
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        ಅನ್ಯಥಾ {
            /* Check for surrogates */
            ಯದಿ(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                ಯದಿ (mySource < sourceLimit) {
                    /* test both code units */
                    ಯದಿ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        /* exit this condition tree */
                    }
                    ಅನ್ಯಥಾ {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        ಭಙ್ಗ;
                    }
                }
                ಅನ್ಯಥಾ {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    ಭಙ್ಗ;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            tempPtr = (((targetLimit - myTarget) >= 4) ? myTarget : tempBuf);

            ಯದಿ (ch <= MAXIMUM_UCS2) {
                indexToWrite = 2;
                tempPtr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            ಅನ್ಯಥಾ {
                indexToWrite = 3;
                tempPtr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                tempPtr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            tempPtr[indexToWrite-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            tempPtr[indexToWrite] = (uint8_t) ((ch & 0x3f) | 0x80);

            ಯದಿ (tempPtr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (indexToWrite + 1);
            }
            ಅನ್ಯಥಾ {
                /* We might run out of room soon. Write it slowly. */
                ಪ್ರತಿ (; tempPtr <= (tempBuf + indexToWrite); tempPtr++) {
                    ಯದಿ (myTarget < targetLimit) {
                        *(myTarget++) = *tempPtr;
                    }
                    ಅನ್ಯಥಾ {
                        cnv->charErrorBuffer[cnv->charErrorBufferLength++] = *tempPtr;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
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

U_CFUNC ಶೂನ್ಯ  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverterFromUnicodeArgs * args,
                                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    ಧ್ರುವ char16_t *mySource = args->source;
    int32_t *myOffsets = args->offsets;
    ಧ್ರುವ char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    ಧ್ರುವ uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *tempPtr;
    UChar32 ch;
    int32_t offset_ಅಂಕೆ, nextSourceIndex;
    int32_t indexToWrite;
    uint8_t tempBuf[4];
    UBool isNotCESU8 = !hasCESU8Data(cnv);

    ಯದಿ (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        offset_ಅಂಕೆ = -1;
        nextSourceIndex = 0;
        ಗಚ್ಛ lowsurrogate;
    } ಅನ್ಯಥಾ {
        offset_ಅಂಕೆ = 0;
    }

    ಯಾವತ್ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        ಯದಿ (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = offset_ಅಂಕೆ++;
            *(myTarget++) = (ವರ್ಣ) ch;
        }
        ಅನ್ಯಥಾ ಯದಿ (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = offset_ಅಂಕೆ;
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            ಯದಿ (myTarget < targetLimit)
            {
                *(myOffsets++) = offset_ಅಂಕೆ++;
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            ಅನ್ಯಥಾ
            {
                cnv->charErrorBuffer[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->charErrorBufferLength = 1;
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        ಅನ್ಯಥಾ
        /* Check for surrogates */
        {
            nextSourceIndex = offset_ಅಂಕೆ + 1;

            ಯದಿ(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                ಯದಿ (mySource < sourceLimit) {
                    /* test both code units */
                    ಯದಿ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        ++nextSourceIndex;
                        /* exit this condition tree */
                    }
                    ಅನ್ಯಥಾ {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        ಭಙ್ಗ;
                    }
                }
                ಅನ್ಯಥಾ {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    ಭಙ್ಗ;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            tempPtr = (((targetLimit - myTarget) >= 4) ? myTarget : tempBuf);

            ಯದಿ (ch <= MAXIMUM_UCS2) {
                indexToWrite = 2;
                tempPtr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            ಅನ್ಯಥಾ {
                indexToWrite = 3;
                tempPtr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                tempPtr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            tempPtr[indexToWrite-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            tempPtr[indexToWrite] = (uint8_t) ((ch & 0x3f) | 0x80);

            ಯದಿ (tempPtr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (indexToWrite + 1);
                myOffsets[0] = offset_ಅಂಕೆ;
                myOffsets[1] = offset_ಅಂಕೆ;
                myOffsets[2] = offset_ಅಂಕೆ;
                ಯದಿ (indexToWrite >= 3) {
                    myOffsets[3] = offset_ಅಂಕೆ;
                }
                myOffsets += (indexToWrite + 1);
            }
            ಅನ್ಯಥಾ {
                /* We might run out of room soon. Write it slowly. */
                ಪ್ರತಿ (; tempPtr <= (tempBuf + indexToWrite); tempPtr++) {
                    ಯದಿ (myTarget < targetLimit)
                    {
                        *(myOffsets++) = offset_ಅಂಕೆ;
                        *(myTarget++) = *tempPtr;
                    }
                    ಅನ್ಯಥಾ
                    {
                        cnv->charErrorBuffer[cnv->charErrorBufferLength++] = *tempPtr;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
            }
            offset_ಅಂಕೆ = nextSourceIndex;
        }
    }

    ಯದಿ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (ವರ್ಣ *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

U_CDECL_BEGIN
ಸ್ಥಿರ UChar32 U_CALLCONV ucnv_getNextUChar_UTF8(UConverterToUnicodeArgs *args,
                                               UErrorCode *err) {
    UConverter *cnv;
    ಧ್ರುವ uint8_t *sourceInitial;
    ಧ್ರುವ uint8_t *source;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = args->converter;
    sourceInitial = source = (ಧ್ರುವ uint8_t *)args->source;
    ಯದಿ (source >= (ಧ್ರುವ uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0xffff;
    }

    myByte = *(source++);
    ಯದಿ (U8_IS_SINGLE(myByte))
    {
        args->source = (ಧ್ರುವ ವರ್ಣ *)source;
        ಪ್ರತ್ಯಾಗಮ (UChar32)myByte;
    }

    uint16_t ಎಣಿಕೆ_Trail_Bytes = U8_ಎಣಿಕೆ_TRAIL_BYTES(myByte);
    ಯದಿ (ಎಣಿಕೆ_Trail_Bytes == 0) {
        cnv->toUBytes[0] = myByte;
        cnv->toULength = 1;
        *err = U_ILLEGAL_CHAR_FOUND;
        args->source = (ಧ್ರುವ ವರ್ಣ *)source;
        ಪ್ರತ್ಯಾಗಮ 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    ಯದಿ (((ಧ್ರುವ ವರ್ಣ *)source + ಎಣಿಕೆ_Trail_Bytes) > args->sourceLimit)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extraBytesToWrite = ಎಣಿಕೆ_Trail_Bytes + 1;
        cnv->toUBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_FOUND;
        ಯಾವತ್(source < (ಧ್ರುವ uint8_t *)args->sourceLimit) {
            uint8_t b = *source;
            ಯದಿ(icu::UTF8::isValidTrail(myByte, b, i, extraBytesToWrite)) {
                cnv->toUBytes[i++] = b;
                ++source;
            } ಅನ್ಯಥಾ {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_FOUND;
                ಭಙ್ಗ;
            }
        }
        cnv->toULength = i;
        args->source = (ಧ್ರುವ ವರ್ಣ *)source;
        ಪ್ರತ್ಯಾಗಮ 0xffff;
    }

    ch = myByte << 6;
    ಯದಿ(ಎಣಿಕೆ_Trail_Bytes == 2) {
        uint8_t t1 = *source, t2;
        ಯದಿ(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source)) {
            args->source = (ಧ್ರುವ ವರ್ಣ *)(source + 1);
            ಪ್ರತ್ಯಾಗಮ (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } ಅನ್ಯಥಾ ಯದಿ(ಎಣಿಕೆ_Trail_Bytes == 1) {
        uint8_t t1 = *source;
        ಯದಿ(U8_IS_TRAIL(t1)) {
            args->source = (ಧ್ರುವ ವರ್ಣ *)(source + 1);
            ಪ್ರತ್ಯಾಗಮ (ch + t1) - offsetsFromUTF8[2];
        }
    } ಅನ್ಯಥಾ {  // countTrailBytes == 3
        uint8_t t1 = *source, t2, t3;
        ಯದಿ(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source) &&
                U8_IS_TRAIL(t3 = *++source)) {
            args->source = (ಧ್ರುವ ವರ್ಣ *)(source + 1);
            ಪ್ರತ್ಯಾಗಮ (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    args->source = (ಧ್ರುವ ವರ್ಣ *)source;

    ಪ್ರತಿ(i = 0; sourceInitial < source; ++i) {
        cnv->toUBytes[i] = *sourceInitial++;
    }
    cnv->toULength = i;
    *err = U_ILLEGAL_CHAR_FOUND;
    ಪ್ರತ್ಯಾಗಮ 0xffff;
} 
U_CDECL_END

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_UTF8FromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                  UConverterToUnicodeArgs *pToUArgs,
                  UErrorCode *pErrorCode) {
    UConverter *utf8;
    ಧ್ರುವ uint8_t *source, *sourceLimit;
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
    ಯದಿ(utf8->toULength > 0) {
        toULength=oldToULength=utf8->toULength;
        toULimit=(int8_t)utf8->mode;
        c=(UChar32)utf8->toUnicodeStatus;
    } ಅನ್ಯಥಾ {
        toULength=oldToULength=toULimit=0;
        c = 0;
    }

    count=(int32_t)(sourceLimit-source)+oldToULength;
    ಯದಿ(count<toULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } ಅನ್ಯಥಾ ಯದಿ(targetCapacity<toULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *pErrorCode=U_USING_DEFAULT_WARNING;
        ಪ್ರತ್ಯಾಗಮ;
    } ಅನ್ಯಥಾ {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        ಯದಿ(count>targetCapacity) {
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

    ಯದಿ(c!=0) {
        utf8->toUnicodeStatus=0;
        utf8->toULength=0;
        ಗಚ್ಛ moreBytes;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    ಯಾವತ್(count>0) {
        b=*source++;
        ಯದಿ(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *target++=b;
            --count;
            ಅನುವರ್ತ;
        } ಅನ್ಯಥಾ {
            ಯದಿ(b>=0xe0) {
                ಯದಿ( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=source[0]) &&
                    U8_IS_TRAIL(t2=source[1])
                ) {
                    source+=2;
                    *target++=b;
                    *target++=t1;
                    *target++=t2;
                    count-=3;
                    ಅನುವರ್ತ;
                }
            } ಅನ್ಯಥಾ {
                ಯದಿ( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*source)
                ) {
                    ++source;
                    *target++=b;
                    *target++=t1;
                    count-=2;
                    ಅನುವರ್ತ;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            oldToULength=0;
            toULength=1;
            toULimit=U8_ಎಣಿಕೆ_BYTES_NON_ASCII(b);
            c=b;
moreBytes:
            ಯಾವತ್(toULength<toULimit) {
                ಯದಿ(source<sourceLimit) {
                    b=*source;
                    ಯದಿ(icu::UTF8::isValidTrail(c, b, toULength, toULimit)) {
                        ++source;
                        ++toULength;
                        c=(c<<6)+b;
                    } ಅನ್ಯಥಾ {
                        ಭಙ್ಗ; /* sequence too short, stop with toULength<toULimit */
                    }
                } ಅನ್ಯಥಾ {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    source-=(toULength-oldToULength);
                    ಯಾವತ್(oldToULength<toULength) {
                        utf8->toUBytes[oldToULength++]=*source++;
                    }
                    utf8->toUnicodeStatus=c;
                    utf8->toULength=toULength;
                    utf8->mode=toULimit;
                    pToUArgs->source=(ವರ್ಣ *)source;
                    pFromUArgs->target=(ವರ್ಣ *)target;
                    ಪ್ರತ್ಯಾಗಮ;
                }
            }

            ಯದಿ(toULength!=toULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                source-=(toULength-oldToULength);
                ಯಾವತ್(oldToULength<toULength) {
                    utf8->toUBytes[oldToULength++]=*source++;
                }
                utf8->toULength=toULength;
                pToUArgs->source=(ವರ್ಣ *)source;
                pFromUArgs->target=(ವರ್ಣ *)target;
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                ಪ್ರತ್ಯಾಗಮ;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                ಪ್ರತಿ(i=0; i<oldToULength; ++i) {
                    *target++=utf8->toUBytes[i];
                }
                source-=(toULength-oldToULength);
                ಪ್ರತಿ(; i<toULength; ++i) {
                    *target++=*source++;
                }
                count-=toULength;
            }
        }
    }
    U_ASSERT(count>=0);

    ಯದಿ(U_SUCCESS(*pErrorCode) && source<sourceLimit) {
        ಯದಿ(target==(ಧ್ರುವ uint8_t *)pFromUArgs->targetLimit) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        } ಅನ್ಯಥಾ {
            b=*source;
            toULimit=U8_ಎಣಿಕೆ_BYTES(b);
            ಯದಿ(toULimit>(sourceLimit-source)) {
                /* collect a truncated byte sequence */
                toULength=0;
                c=b;
                ಪ್ರತಿ(;;) {
                    utf8->toUBytes[toULength++]=b;
                    ಯದಿ(++source==sourceLimit) {
                        /* partial byte sequence at end of source */
                        utf8->toUnicodeStatus=c;
                        utf8->toULength=toULength;
                        utf8->mode=toULimit;
                        ಭಙ್ಗ;
                    } ಅನ್ಯಥಾ ಯದಿ(!icu::UTF8::isValidTrail(c, b=*source, toULength, toULimit)) {
                        utf8->toULength=toULength;
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        ಭಙ್ಗ;
                    }
                    c=(c<<6)+b;
                }
            } ಅನ್ಯಥಾ {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *pErrorCode=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    pToUArgs->source=(ವರ್ಣ *)source;
    pFromUArgs->target=(ವರ್ಣ *)target;
}

U_CDECL_END

/* UTF-8 converter data ----------------------------------------------------- */

ಸ್ಥಿರ ಧ್ರುವ UConverterImpl _UTF8Impl={
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
ಸ್ಥಿರ ಧ್ರುವ UConverterStaticData _UTF8StaticData={
    ಪರಿಮಾಣ(UConverterStaticData),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,ಅಸತ್ಯ,ಅಸತ್ಯ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ಧ್ರುವ UConverterSharedData _UTF8Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF8StaticData, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

ಸ್ಥಿರ ಧ್ರುವ UConverterImpl _CESU8Impl={
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

ಸ್ಥಿರ ಧ್ರುವ UConverterStaticData _CESU8StaticData={
    ಪರಿಮಾಣ(UConverterStaticData),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,ಅಸತ್ಯ,ಅಸತ್ಯ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ಧ್ರುವ UConverterSharedData _CESU8Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_CESU8StaticData, &_CESU8Impl);

#endif
