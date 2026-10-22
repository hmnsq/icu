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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_CONVERSION

#ଅନ୍ତର୍ଭାଵ "unicode/ucnv.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf8.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_bld.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_cnv.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"

/* Prototypes --------------------------------------------------------------- */

/* Keep these here to make finicky compilers happy */

U_CFUNC ଶୂନ୍ୟ ucnv_fromUnicode_UTF8(UConverterFromUnicodeArgs *args,
                                           UErrorCode *err);
U_CFUNC ଶୂନ୍ୟ ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverterFromUnicodeArgs *args,
                                                        UErrorCode *err);


/* UTF-8 -------------------------------------------------------------------- */

#define MAXIMUM_UCS2            0x0000FFFF

ସ୍ଥିର ଅଚଳ uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

ସ୍ଥିର UBool hasCESU8Data(ଅଚଳ UConverter *cnv)
{
#ଯଦି UCONFIG_ONLY_HTML_CONVERSION
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ cnv->sharedData == &_CESU8Data;
#endif
}
U_CDECL_BEGIN
ସ୍ଥିର ଶୂନ୍ୟ  U_CALLCONV ucnv_toUnicode_UTF8 (UConverterToUnicodeArgs * args,
                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    ଅଚଳ unsigned ଅକ୍ଷର *mySource = (unsigned ଅକ୍ଷର *) args->source;
    char16_t *myTarget = args->target;
    ଅଚଳ unsigned ଅକ୍ଷର *sourceLimit = (unsigned ଅକ୍ଷର *) args->sourceLimit;
    ଅଚଳ char16_t *targetLimit = args->targetLimit;
    unsigned ଅକ୍ଷର *toUBytes = cnv->toUBytes;
    UBool isCESU8 = hasCESU8Data(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    ଯଦି (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        ଯାଅ morebytes;
    }


    ଯେତେବେଳେ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        ଯଦି (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
        }
        ଅନ୍ୟଥା
        {
            /* store the first char */
            toUBytes[0] = (ଅକ୍ଷର)ch;
            inBytes = U8_ଗଣନା_BYTES_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            ଯେତେବେଳେ (i < inBytes)
            {
                ଯଦି (mySource < sourceLimit)
                {
                    toUBytes[i] = (ଅକ୍ଷର) (ch2 = *mySource);
                    ଯଦି (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        ଭାଙ୍ଗ; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                ଅନ୍ୟଥା
                {
                    /* stores a partially calculated target*/
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t) i;
                    ଯାଅ donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            ଯଦି (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                ଯଦି (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                }
                ଅନ୍ୟଥା
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    ଯଦି (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                    }
                    ଅନ୍ୟଥା
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UCharErrorBuffer[0] = (char16_t) ch;
                        cnv->UCharErrorBufferLength = 1;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                        ଭାଙ୍ଗ;
                    }
                }
            }
            ଅନ୍ୟଥା
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                ଭାଙ୍ଗ;
            }
        }
    }

donefornow:
    ଯଦି (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ଅଚଳ ଅକ୍ଷର *) mySource;
}

ସ୍ଥିର ଶୂନ୍ୟ  U_CALLCONV ucnv_toUnicode_UTF8_OFFSETS_LOGIC (UConverterToUnicodeArgs * args,
                                                UErrorCode * err)
{
    UConverter *cnv = args->converter;
    ଅଚଳ unsigned ଅକ୍ଷର *mySource = (unsigned ଅକ୍ଷର *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    int32_t offset_ଅଙ୍କ = 0;
    ଅଚଳ unsigned ଅକ୍ଷର *sourceLimit = (unsigned ଅକ୍ଷର *) args->sourceLimit;
    ଅଚଳ char16_t *targetLimit = args->targetLimit;
    unsigned ଅକ୍ଷର *toUBytes = cnv->toUBytes;
    UBool isCESU8 = hasCESU8Data(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    ଯଦି (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        ଯାଅ morebytes;
    }

    ଯେତେବେଳେ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        ଯଦି (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
            *(myOffsets++) = offset_ଅଙ୍କ++;
        }
        ଅନ୍ୟଥା
        {
            toUBytes[0] = (ଅକ୍ଷର)ch;
            inBytes = U8_ଗଣନା_BYTES_NON_ASCII(ch);
            i = 1;

morebytes:
            ଯେତେବେଳେ (i < inBytes)
            {
                ଯଦି (mySource < sourceLimit)
                {
                    toUBytes[i] = (ଅକ୍ଷର) (ch2 = *mySource);
                    ଯଦି (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        ଭାଙ୍ଗ; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                ଅନ୍ୟଥା
                {
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t)i;
                    ଯାଅ donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            ଯଦି (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                ଯଦି (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                    *(myOffsets++) = offset_ଅଙ୍କ;
                }
                ଅନ୍ୟଥା
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    *(myOffsets++) = offset_ଅଙ୍କ;
                    ch = U16_TRAIL(ch);
                    ଯଦି (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                        *(myOffsets++) = offset_ଅଙ୍କ;
                    }
                    ଅନ୍ୟଥା
                    {
                        cnv->UCharErrorBuffer[0] = (char16_t) ch;
                        cnv->UCharErrorBufferLength = 1;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
                offset_ଅଙ୍କ += i;
            }
            ଅନ୍ୟଥା
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                ଭାଙ୍ଗ;
            }
        }
    }

donefornow:
    ଯଦି (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ଅଚଳ ଅକ୍ଷର *) mySource;
    args->offsets = myOffsets;
}
U_CDECL_END

U_CFUNC ଶୂନ୍ୟ  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverterFromUnicodeArgs * args,
                                    UErrorCode * err)
{
    UConverter *cnv = args->converter;
    ଅଚଳ char16_t *mySource = args->source;
    ଅଚଳ char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    ଅଚଳ uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *tempPtr;
    UChar32 ch;
    uint8_t tempBuf[4];
    int32_t indexToWrite;
    UBool isNotCESU8 = !hasCESU8Data(cnv);

    ଯଦି (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        ଯାଅ lowsurrogate;
    }

    ଯେତେବେଳେ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        ଯଦି (ch < 0x80)        /* Single byte */
        {
            *(myTarget++) = (uint8_t) ch;
        }
        ଅନ୍ୟଥା ଯଦି (ch < 0x800)  /* Double byte */
        {
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            ଯଦି (myTarget < targetLimit)
            {
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            ଅନ୍ୟଥା
            {
                cnv->charErrorBuffer[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->charErrorBufferLength = 1;
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        ଅନ୍ୟଥା {
            /* Check for surrogates */
            ଯଦି(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                ଯଦି (mySource < sourceLimit) {
                    /* test both code units */
                    ଯଦି(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        /* exit this condition tree */
                    }
                    ଅନ୍ୟଥା {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        ଭାଙ୍ଗ;
                    }
                }
                ଅନ୍ୟଥା {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    ଭାଙ୍ଗ;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            tempPtr = (((targetLimit - myTarget) >= 4) ? myTarget : tempBuf);

            ଯଦି (ch <= MAXIMUM_UCS2) {
                indexToWrite = 2;
                tempPtr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            ଅନ୍ୟଥା {
                indexToWrite = 3;
                tempPtr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                tempPtr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            tempPtr[indexToWrite-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            tempPtr[indexToWrite] = (uint8_t) ((ch & 0x3f) | 0x80);

            ଯଦି (tempPtr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (indexToWrite + 1);
            }
            ଅନ୍ୟଥା {
                /* We might run out of room soon. Write it slowly. */
                ପାଇଁ (; tempPtr <= (tempBuf + indexToWrite); tempPtr++) {
                    ଯଦି (myTarget < targetLimit) {
                        *(myTarget++) = *tempPtr;
                    }
                    ଅନ୍ୟଥା {
                        cnv->charErrorBuffer[cnv->charErrorBufferLength++] = *tempPtr;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
            }
        }
    }

    ଯଦି (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (ଅକ୍ଷର *) myTarget;
    args->source = mySource;
}

U_CFUNC ଶୂନ୍ୟ  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverterFromUnicodeArgs * args,
                                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    ଅଚଳ char16_t *mySource = args->source;
    int32_t *myOffsets = args->offsets;
    ଅଚଳ char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    ଅଚଳ uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *tempPtr;
    UChar32 ch;
    int32_t offset_ଅଙ୍କ, nextSourceIndex;
    int32_t indexToWrite;
    uint8_t tempBuf[4];
    UBool isNotCESU8 = !hasCESU8Data(cnv);

    ଯଦି (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        offset_ଅଙ୍କ = -1;
        nextSourceIndex = 0;
        ଯାଅ lowsurrogate;
    } ଅନ୍ୟଥା {
        offset_ଅଙ୍କ = 0;
    }

    ଯେତେବେଳେ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        ଯଦି (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = offset_ଅଙ୍କ++;
            *(myTarget++) = (ଅକ୍ଷର) ch;
        }
        ଅନ୍ୟଥା ଯଦି (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = offset_ଅଙ୍କ;
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            ଯଦି (myTarget < targetLimit)
            {
                *(myOffsets++) = offset_ଅଙ୍କ++;
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            ଅନ୍ୟଥା
            {
                cnv->charErrorBuffer[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->charErrorBufferLength = 1;
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        ଅନ୍ୟଥା
        /* Check for surrogates */
        {
            nextSourceIndex = offset_ଅଙ୍କ + 1;

            ଯଦି(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                ଯଦି (mySource < sourceLimit) {
                    /* test both code units */
                    ଯଦି(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        ++nextSourceIndex;
                        /* exit this condition tree */
                    }
                    ଅନ୍ୟଥା {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        ଭାଙ୍ଗ;
                    }
                }
                ଅନ୍ୟଥା {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    ଭାଙ୍ଗ;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            tempPtr = (((targetLimit - myTarget) >= 4) ? myTarget : tempBuf);

            ଯଦି (ch <= MAXIMUM_UCS2) {
                indexToWrite = 2;
                tempPtr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            ଅନ୍ୟଥା {
                indexToWrite = 3;
                tempPtr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                tempPtr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            tempPtr[indexToWrite-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            tempPtr[indexToWrite] = (uint8_t) ((ch & 0x3f) | 0x80);

            ଯଦି (tempPtr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (indexToWrite + 1);
                myOffsets[0] = offset_ଅଙ୍କ;
                myOffsets[1] = offset_ଅଙ୍କ;
                myOffsets[2] = offset_ଅଙ୍କ;
                ଯଦି (indexToWrite >= 3) {
                    myOffsets[3] = offset_ଅଙ୍କ;
                }
                myOffsets += (indexToWrite + 1);
            }
            ଅନ୍ୟଥା {
                /* We might run out of room soon. Write it slowly. */
                ପାଇଁ (; tempPtr <= (tempBuf + indexToWrite); tempPtr++) {
                    ଯଦି (myTarget < targetLimit)
                    {
                        *(myOffsets++) = offset_ଅଙ୍କ;
                        *(myTarget++) = *tempPtr;
                    }
                    ଅନ୍ୟଥା
                    {
                        cnv->charErrorBuffer[cnv->charErrorBufferLength++] = *tempPtr;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
            }
            offset_ଅଙ୍କ = nextSourceIndex;
        }
    }

    ଯଦି (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (ଅକ୍ଷର *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

U_CDECL_BEGIN
ସ୍ଥିର UChar32 U_CALLCONV ucnv_getNextUChar_UTF8(UConverterToUnicodeArgs *args,
                                               UErrorCode *err) {
    UConverter *cnv;
    ଅଚଳ uint8_t *sourceInitial;
    ଅଚଳ uint8_t *source;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = args->converter;
    sourceInitial = source = (ଅଚଳ uint8_t *)args->source;
    ଯଦି (source >= (ଅଚଳ uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0xffff;
    }

    myByte = *(source++);
    ଯଦି (U8_IS_SINGLE(myByte))
    {
        args->source = (ଅଚଳ ଅକ୍ଷର *)source;
        ଫେରନ୍ତୁ (UChar32)myByte;
    }

    uint16_t ଗଣନା_Trail_Bytes = U8_ଗଣନା_TRAIL_BYTES(myByte);
    ଯଦି (ଗଣନା_Trail_Bytes == 0) {
        cnv->toUBytes[0] = myByte;
        cnv->toULength = 1;
        *err = U_ILLEGAL_CHAR_FOUND;
        args->source = (ଅଚଳ ଅକ୍ଷର *)source;
        ଫେରନ୍ତୁ 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    ଯଦି (((ଅଚଳ ଅକ୍ଷର *)source + ଗଣନା_Trail_Bytes) > args->sourceLimit)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extraBytesToWrite = ଗଣନା_Trail_Bytes + 1;
        cnv->toUBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_FOUND;
        ଯେତେବେଳେ(source < (ଅଚଳ uint8_t *)args->sourceLimit) {
            uint8_t b = *source;
            ଯଦି(icu::UTF8::isValidTrail(myByte, b, i, extraBytesToWrite)) {
                cnv->toUBytes[i++] = b;
                ++source;
            } ଅନ୍ୟଥା {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_FOUND;
                ଭାଙ୍ଗ;
            }
        }
        cnv->toULength = i;
        args->source = (ଅଚଳ ଅକ୍ଷର *)source;
        ଫେରନ୍ତୁ 0xffff;
    }

    ch = myByte << 6;
    ଯଦି(ଗଣନା_Trail_Bytes == 2) {
        uint8_t t1 = *source, t2;
        ଯଦି(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source)) {
            args->source = (ଅଚଳ ଅକ୍ଷର *)(source + 1);
            ଫେରନ୍ତୁ (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } ଅନ୍ୟଥା ଯଦି(ଗଣନା_Trail_Bytes == 1) {
        uint8_t t1 = *source;
        ଯଦି(U8_IS_TRAIL(t1)) {
            args->source = (ଅଚଳ ଅକ୍ଷର *)(source + 1);
            ଫେରନ୍ତୁ (ch + t1) - offsetsFromUTF8[2];
        }
    } ଅନ୍ୟଥା {  // countTrailBytes == 3
        uint8_t t1 = *source, t2, t3;
        ଯଦି(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source) &&
                U8_IS_TRAIL(t3 = *++source)) {
            args->source = (ଅଚଳ ଅକ୍ଷର *)(source + 1);
            ଫେରନ୍ତୁ (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    args->source = (ଅଚଳ ଅକ୍ଷର *)source;

    ପାଇଁ(i = 0; sourceInitial < source; ++i) {
        cnv->toUBytes[i] = *sourceInitial++;
    }
    cnv->toULength = i;
    *err = U_ILLEGAL_CHAR_FOUND;
    ଫେରନ୍ତୁ 0xffff;
} 
U_CDECL_END

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
ucnv_UTF8FromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                  UConverterToUnicodeArgs *pToUArgs,
                  UErrorCode *pErrorCode) {
    UConverter *utf8;
    ଅଚଳ uint8_t *source, *sourceLimit;
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
    ଯଦି(utf8->toULength > 0) {
        toULength=oldToULength=utf8->toULength;
        toULimit=(int8_t)utf8->mode;
        c=(UChar32)utf8->toUnicodeStatus;
    } ଅନ୍ୟଥା {
        toULength=oldToULength=toULimit=0;
        c = 0;
    }

    count=(int32_t)(sourceLimit-source)+oldToULength;
    ଯଦି(count<toULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } ଅନ୍ୟଥା ଯଦି(targetCapacity<toULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *pErrorCode=U_USING_DEFAULT_WARNING;
        ଫେରନ୍ତୁ;
    } ଅନ୍ୟଥା {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        ଯଦି(count>targetCapacity) {
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

    ଯଦି(c!=0) {
        utf8->toUnicodeStatus=0;
        utf8->toULength=0;
        ଯାଅ moreBytes;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    ଯେତେବେଳେ(count>0) {
        b=*source++;
        ଯଦି(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *target++=b;
            --count;
            ଜାରି;
        } ଅନ୍ୟଥା {
            ଯଦି(b>=0xe0) {
                ଯଦି( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=source[0]) &&
                    U8_IS_TRAIL(t2=source[1])
                ) {
                    source+=2;
                    *target++=b;
                    *target++=t1;
                    *target++=t2;
                    count-=3;
                    ଜାରି;
                }
            } ଅନ୍ୟଥା {
                ଯଦି( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*source)
                ) {
                    ++source;
                    *target++=b;
                    *target++=t1;
                    count-=2;
                    ଜାରି;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            oldToULength=0;
            toULength=1;
            toULimit=U8_ଗଣନା_BYTES_NON_ASCII(b);
            c=b;
moreBytes:
            ଯେତେବେଳେ(toULength<toULimit) {
                ଯଦି(source<sourceLimit) {
                    b=*source;
                    ଯଦି(icu::UTF8::isValidTrail(c, b, toULength, toULimit)) {
                        ++source;
                        ++toULength;
                        c=(c<<6)+b;
                    } ଅନ୍ୟଥା {
                        ଭାଙ୍ଗ; /* sequence too short, stop with toULength<toULimit */
                    }
                } ଅନ୍ୟଥା {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    source-=(toULength-oldToULength);
                    ଯେତେବେଳେ(oldToULength<toULength) {
                        utf8->toUBytes[oldToULength++]=*source++;
                    }
                    utf8->toUnicodeStatus=c;
                    utf8->toULength=toULength;
                    utf8->mode=toULimit;
                    pToUArgs->source=(ଅକ୍ଷର *)source;
                    pFromUArgs->target=(ଅକ୍ଷର *)target;
                    ଫେରନ୍ତୁ;
                }
            }

            ଯଦି(toULength!=toULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                source-=(toULength-oldToULength);
                ଯେତେବେଳେ(oldToULength<toULength) {
                    utf8->toUBytes[oldToULength++]=*source++;
                }
                utf8->toULength=toULength;
                pToUArgs->source=(ଅକ୍ଷର *)source;
                pFromUArgs->target=(ଅକ୍ଷର *)target;
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                ଫେରନ୍ତୁ;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                ପାଇଁ(i=0; i<oldToULength; ++i) {
                    *target++=utf8->toUBytes[i];
                }
                source-=(toULength-oldToULength);
                ପାଇଁ(; i<toULength; ++i) {
                    *target++=*source++;
                }
                count-=toULength;
            }
        }
    }
    U_ASSERT(count>=0);

    ଯଦି(U_SUCCESS(*pErrorCode) && source<sourceLimit) {
        ଯଦି(target==(ଅଚଳ uint8_t *)pFromUArgs->targetLimit) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        } ଅନ୍ୟଥା {
            b=*source;
            toULimit=U8_ଗଣନା_BYTES(b);
            ଯଦି(toULimit>(sourceLimit-source)) {
                /* collect a truncated byte sequence */
                toULength=0;
                c=b;
                ପାଇଁ(;;) {
                    utf8->toUBytes[toULength++]=b;
                    ଯଦି(++source==sourceLimit) {
                        /* partial byte sequence at end of source */
                        utf8->toUnicodeStatus=c;
                        utf8->toULength=toULength;
                        utf8->mode=toULimit;
                        ଭାଙ୍ଗ;
                    } ଅନ୍ୟଥା ଯଦି(!icu::UTF8::isValidTrail(c, b=*source, toULength, toULimit)) {
                        utf8->toULength=toULength;
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        ଭାଙ୍ଗ;
                    }
                    c=(c<<6)+b;
                }
            } ଅନ୍ୟଥା {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *pErrorCode=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    pToUArgs->source=(ଅକ୍ଷର *)source;
    pFromUArgs->target=(ଅକ୍ଷର *)target;
}

U_CDECL_END

/* UTF-8 converter data ----------------------------------------------------- */

ସ୍ଥିର ଅଚଳ UConverterImpl _UTF8Impl={
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
ସ୍ଥିର ଅଚଳ UConverterStaticData _UTF8StaticData={
    ଆକାର(UConverterStaticData),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,ମିଥ୍ୟା,ମିଥ୍ୟା,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ଅଚଳ UConverterSharedData _UTF8Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF8StaticData, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

ସ୍ଥିର ଅଚଳ UConverterImpl _CESU8Impl={
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

ସ୍ଥିର ଅଚଳ UConverterStaticData _CESU8StaticData={
    ଆକାର(UConverterStaticData),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,ମିଥ୍ୟା,ମିଥ୍ୟା,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ଅଚଳ UConverterSharedData _CESU8Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_CESU8StaticData, &_CESU8Impl);

#endif
