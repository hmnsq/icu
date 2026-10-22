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

#अन्तर्भाव "unicode/utypes.h"

#जइ !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ustr_imp.h"

/* Prototypes --------------------------------------------------------------- */

/* Keep these here to make finicky compilers happy */

U_CFUNC शून्य ucnv_fromUnicode_UTF8(UConverterFromUnicodeArgs *args,
                                           UErrorCode *err);
U_CFUNC शून्य ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverterFromUnicodeArgs *args,
                                                        UErrorCode *err);


/* UTF-8 -------------------------------------------------------------------- */

#define MAXIMUM_UCS2            0x0000FFFF

स्थिर ध्रुव uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

स्थिर UBool hasCESU8Data(ध्रुव UConverter *cnv)
{
#जइ UCONFIG_ONLY_HTML_CONVERSION
    प्रत्यागम मिच्छ;
#अण्णहा
    प्रत्यागम cnv->sharedData == &_CESU8Data;
#endif
}
U_CDECL_BEGIN
स्थिर शून्य  U_CALLCONV ucnv_toUnicode_UTF8 (UConverterToUnicodeArgs * args,
                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    ध्रुव unsigned वर्ण *mySource = (unsigned वर्ण *) args->source;
    char16_t *myTarget = args->target;
    ध्रुव unsigned वर्ण *sourceLimit = (unsigned वर्ण *) args->sourceLimit;
    ध्रुव char16_t *targetLimit = args->targetLimit;
    unsigned वर्ण *toUBytes = cnv->toUBytes;
    UBool isCESU8 = hasCESU8Data(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    जइ (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        गच्छ morebytes;
    }


    जाव (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        जइ (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
        }
        अण्णहा
        {
            /* store the first char */
            toUBytes[0] = (वर्ण)ch;
            inBytes = U8_गणनम्_BYTES_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            जाव (i < inBytes)
            {
                जइ (mySource < sourceLimit)
                {
                    toUBytes[i] = (वर्ण) (ch2 = *mySource);
                    जइ (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        भङ्ग; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                अण्णहा
                {
                    /* stores a partially calculated target*/
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t) i;
                    गच्छ donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            जइ (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                जइ (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                }
                अण्णहा
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    जइ (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                    }
                    अण्णहा
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UCharErrorBuffer[0] = (char16_t) ch;
                        cnv->UCharErrorBufferLength = 1;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                        भङ्ग;
                    }
                }
            }
            अण्णहा
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                भङ्ग;
            }
        }
    }

donefornow:
    जइ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ध्रुव वर्ण *) mySource;
}

स्थिर शून्य  U_CALLCONV ucnv_toUnicode_UTF8_OFFSETS_LOGIC (UConverterToUnicodeArgs * args,
                                                UErrorCode * err)
{
    UConverter *cnv = args->converter;
    ध्रुव unsigned वर्ण *mySource = (unsigned वर्ण *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    int32_t offset_अङ्कः = 0;
    ध्रुव unsigned वर्ण *sourceLimit = (unsigned वर्ण *) args->sourceLimit;
    ध्रुव char16_t *targetLimit = args->targetLimit;
    unsigned वर्ण *toUBytes = cnv->toUBytes;
    UBool isCESU8 = hasCESU8Data(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    जइ (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        गच्छ morebytes;
    }

    जाव (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        जइ (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
            *(myOffsets++) = offset_अङ्कः++;
        }
        अण्णहा
        {
            toUBytes[0] = (वर्ण)ch;
            inBytes = U8_गणनम्_BYTES_NON_ASCII(ch);
            i = 1;

morebytes:
            जाव (i < inBytes)
            {
                जइ (mySource < sourceLimit)
                {
                    toUBytes[i] = (वर्ण) (ch2 = *mySource);
                    जइ (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        भङ्ग; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                अण्णहा
                {
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t)i;
                    गच्छ donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            जइ (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                जइ (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                    *(myOffsets++) = offset_अङ्कः;
                }
                अण्णहा
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    *(myOffsets++) = offset_अङ्कः;
                    ch = U16_TRAIL(ch);
                    जइ (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                        *(myOffsets++) = offset_अङ्कः;
                    }
                    अण्णहा
                    {
                        cnv->UCharErrorBuffer[0] = (char16_t) ch;
                        cnv->UCharErrorBufferLength = 1;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
                offset_अङ्कः += i;
            }
            अण्णहा
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                भङ्ग;
            }
        }
    }

donefornow:
    जइ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ध्रुव वर्ण *) mySource;
    args->offsets = myOffsets;
}
U_CDECL_END

U_CFUNC शून्य  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverterFromUnicodeArgs * args,
                                    UErrorCode * err)
{
    UConverter *cnv = args->converter;
    ध्रुव char16_t *mySource = args->source;
    ध्रुव char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    ध्रुव uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *tempPtr;
    UChar32 ch;
    uint8_t tempBuf[4];
    int32_t indexToWrite;
    UBool isNotCESU8 = !hasCESU8Data(cnv);

    जइ (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        गच्छ lowsurrogate;
    }

    जाव (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        जइ (ch < 0x80)        /* Single byte */
        {
            *(myTarget++) = (uint8_t) ch;
        }
        अण्णहा जइ (ch < 0x800)  /* Double byte */
        {
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            जइ (myTarget < targetLimit)
            {
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            अण्णहा
            {
                cnv->charErrorBuffer[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->charErrorBufferLength = 1;
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        अण्णहा {
            /* Check for surrogates */
            जइ(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                जइ (mySource < sourceLimit) {
                    /* test both code units */
                    जइ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        /* exit this condition tree */
                    }
                    अण्णहा {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        भङ्ग;
                    }
                }
                अण्णहा {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    भङ्ग;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            tempPtr = (((targetLimit - myTarget) >= 4) ? myTarget : tempBuf);

            जइ (ch <= MAXIMUM_UCS2) {
                indexToWrite = 2;
                tempPtr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            अण्णहा {
                indexToWrite = 3;
                tempPtr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                tempPtr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            tempPtr[indexToWrite-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            tempPtr[indexToWrite] = (uint8_t) ((ch & 0x3f) | 0x80);

            जइ (tempPtr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (indexToWrite + 1);
            }
            अण्णहा {
                /* We might run out of room soon. Write it slowly. */
                प्रति (; tempPtr <= (tempBuf + indexToWrite); tempPtr++) {
                    जइ (myTarget < targetLimit) {
                        *(myTarget++) = *tempPtr;
                    }
                    अण्णहा {
                        cnv->charErrorBuffer[cnv->charErrorBufferLength++] = *tempPtr;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
            }
        }
    }

    जइ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (वर्ण *) myTarget;
    args->source = mySource;
}

U_CFUNC शून्य  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverterFromUnicodeArgs * args,
                                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    ध्रुव char16_t *mySource = args->source;
    int32_t *myOffsets = args->offsets;
    ध्रुव char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    ध्रुव uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *tempPtr;
    UChar32 ch;
    int32_t offset_अङ्कः, nextSourceIndex;
    int32_t indexToWrite;
    uint8_t tempBuf[4];
    UBool isNotCESU8 = !hasCESU8Data(cnv);

    जइ (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        offset_अङ्कः = -1;
        nextSourceIndex = 0;
        गच्छ lowsurrogate;
    } अण्णहा {
        offset_अङ्कः = 0;
    }

    जाव (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        जइ (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = offset_अङ्कः++;
            *(myTarget++) = (वर्ण) ch;
        }
        अण्णहा जइ (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = offset_अङ्कः;
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            जइ (myTarget < targetLimit)
            {
                *(myOffsets++) = offset_अङ्कः++;
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            अण्णहा
            {
                cnv->charErrorBuffer[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->charErrorBufferLength = 1;
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        अण्णहा
        /* Check for surrogates */
        {
            nextSourceIndex = offset_अङ्कः + 1;

            जइ(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                जइ (mySource < sourceLimit) {
                    /* test both code units */
                    जइ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        ++nextSourceIndex;
                        /* exit this condition tree */
                    }
                    अण्णहा {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        भङ्ग;
                    }
                }
                अण्णहा {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    भङ्ग;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            tempPtr = (((targetLimit - myTarget) >= 4) ? myTarget : tempBuf);

            जइ (ch <= MAXIMUM_UCS2) {
                indexToWrite = 2;
                tempPtr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            अण्णहा {
                indexToWrite = 3;
                tempPtr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                tempPtr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            tempPtr[indexToWrite-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            tempPtr[indexToWrite] = (uint8_t) ((ch & 0x3f) | 0x80);

            जइ (tempPtr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (indexToWrite + 1);
                myOffsets[0] = offset_अङ्कः;
                myOffsets[1] = offset_अङ्कः;
                myOffsets[2] = offset_अङ्कः;
                जइ (indexToWrite >= 3) {
                    myOffsets[3] = offset_अङ्कः;
                }
                myOffsets += (indexToWrite + 1);
            }
            अण्णहा {
                /* We might run out of room soon. Write it slowly. */
                प्रति (; tempPtr <= (tempBuf + indexToWrite); tempPtr++) {
                    जइ (myTarget < targetLimit)
                    {
                        *(myOffsets++) = offset_अङ्कः;
                        *(myTarget++) = *tempPtr;
                    }
                    अण्णहा
                    {
                        cnv->charErrorBuffer[cnv->charErrorBufferLength++] = *tempPtr;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
            }
            offset_अङ्कः = nextSourceIndex;
        }
    }

    जइ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (वर्ण *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

U_CDECL_BEGIN
स्थिर UChar32 U_CALLCONV ucnv_getNextUChar_UTF8(UConverterToUnicodeArgs *args,
                                               UErrorCode *err) {
    UConverter *cnv;
    ध्रुव uint8_t *sourceInitial;
    ध्रुव uint8_t *source;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = args->converter;
    sourceInitial = source = (ध्रुव uint8_t *)args->source;
    जइ (source >= (ध्रुव uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0xffff;
    }

    myByte = *(source++);
    जइ (U8_IS_SINGLE(myByte))
    {
        args->source = (ध्रुव वर्ण *)source;
        प्रत्यागम (UChar32)myByte;
    }

    uint16_t गणनम्_Trail_Bytes = U8_गणनम्_TRAIL_BYTES(myByte);
    जइ (गणनम्_Trail_Bytes == 0) {
        cnv->toUBytes[0] = myByte;
        cnv->toULength = 1;
        *err = U_ILLEGAL_CHAR_FOUND;
        args->source = (ध्रुव वर्ण *)source;
        प्रत्यागम 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    जइ (((ध्रुव वर्ण *)source + गणनम्_Trail_Bytes) > args->sourceLimit)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extraBytesToWrite = गणनम्_Trail_Bytes + 1;
        cnv->toUBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_FOUND;
        जाव(source < (ध्रुव uint8_t *)args->sourceLimit) {
            uint8_t b = *source;
            जइ(icu::UTF8::isValidTrail(myByte, b, i, extraBytesToWrite)) {
                cnv->toUBytes[i++] = b;
                ++source;
            } अण्णहा {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_FOUND;
                भङ्ग;
            }
        }
        cnv->toULength = i;
        args->source = (ध्रुव वर्ण *)source;
        प्रत्यागम 0xffff;
    }

    ch = myByte << 6;
    जइ(गणनम्_Trail_Bytes == 2) {
        uint8_t t1 = *source, t2;
        जइ(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source)) {
            args->source = (ध्रुव वर्ण *)(source + 1);
            प्रत्यागम (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } अण्णहा जइ(गणनम्_Trail_Bytes == 1) {
        uint8_t t1 = *source;
        जइ(U8_IS_TRAIL(t1)) {
            args->source = (ध्रुव वर्ण *)(source + 1);
            प्रत्यागम (ch + t1) - offsetsFromUTF8[2];
        }
    } अण्णहा {  // countTrailBytes == 3
        uint8_t t1 = *source, t2, t3;
        जइ(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source) &&
                U8_IS_TRAIL(t3 = *++source)) {
            args->source = (ध्रुव वर्ण *)(source + 1);
            प्रत्यागम (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    args->source = (ध्रुव वर्ण *)source;

    प्रति(i = 0; sourceInitial < source; ++i) {
        cnv->toUBytes[i] = *sourceInitial++;
    }
    cnv->toULength = i;
    *err = U_ILLEGAL_CHAR_FOUND;
    प्रत्यागम 0xffff;
} 
U_CDECL_END

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
स्थिर शून्य U_CALLCONV
ucnv_UTF8FromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                  UConverterToUnicodeArgs *pToUArgs,
                  UErrorCode *pErrorCode) {
    UConverter *utf8;
    ध्रुव uint8_t *source, *sourceLimit;
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
    जइ(utf8->toULength > 0) {
        toULength=oldToULength=utf8->toULength;
        toULimit=(int8_t)utf8->mode;
        c=(UChar32)utf8->toUnicodeStatus;
    } अण्णहा {
        toULength=oldToULength=toULimit=0;
        c = 0;
    }

    count=(int32_t)(sourceLimit-source)+oldToULength;
    जइ(count<toULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } अण्णहा जइ(targetCapacity<toULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *pErrorCode=U_USING_DEFAULT_WARNING;
        प्रत्यागम;
    } अण्णहा {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        जइ(count>targetCapacity) {
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

    जइ(c!=0) {
        utf8->toUnicodeStatus=0;
        utf8->toULength=0;
        गच्छ moreBytes;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    जाव(count>0) {
        b=*source++;
        जइ(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *target++=b;
            --count;
            अनुवर्त;
        } अण्णहा {
            जइ(b>=0xe0) {
                जइ( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=source[0]) &&
                    U8_IS_TRAIL(t2=source[1])
                ) {
                    source+=2;
                    *target++=b;
                    *target++=t1;
                    *target++=t2;
                    count-=3;
                    अनुवर्त;
                }
            } अण्णहा {
                जइ( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*source)
                ) {
                    ++source;
                    *target++=b;
                    *target++=t1;
                    count-=2;
                    अनुवर्त;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            oldToULength=0;
            toULength=1;
            toULimit=U8_गणनम्_BYTES_NON_ASCII(b);
            c=b;
moreBytes:
            जाव(toULength<toULimit) {
                जइ(source<sourceLimit) {
                    b=*source;
                    जइ(icu::UTF8::isValidTrail(c, b, toULength, toULimit)) {
                        ++source;
                        ++toULength;
                        c=(c<<6)+b;
                    } अण्णहा {
                        भङ्ग; /* sequence too short, stop with toULength<toULimit */
                    }
                } अण्णहा {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    source-=(toULength-oldToULength);
                    जाव(oldToULength<toULength) {
                        utf8->toUBytes[oldToULength++]=*source++;
                    }
                    utf8->toUnicodeStatus=c;
                    utf8->toULength=toULength;
                    utf8->mode=toULimit;
                    pToUArgs->source=(वर्ण *)source;
                    pFromUArgs->target=(वर्ण *)target;
                    प्रत्यागम;
                }
            }

            जइ(toULength!=toULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                source-=(toULength-oldToULength);
                जाव(oldToULength<toULength) {
                    utf8->toUBytes[oldToULength++]=*source++;
                }
                utf8->toULength=toULength;
                pToUArgs->source=(वर्ण *)source;
                pFromUArgs->target=(वर्ण *)target;
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                प्रत्यागम;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                प्रति(i=0; i<oldToULength; ++i) {
                    *target++=utf8->toUBytes[i];
                }
                source-=(toULength-oldToULength);
                प्रति(; i<toULength; ++i) {
                    *target++=*source++;
                }
                count-=toULength;
            }
        }
    }
    U_ASSERT(count>=0);

    जइ(U_SUCCESS(*pErrorCode) && source<sourceLimit) {
        जइ(target==(ध्रुव uint8_t *)pFromUArgs->targetLimit) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        } अण्णहा {
            b=*source;
            toULimit=U8_गणनम्_BYTES(b);
            जइ(toULimit>(sourceLimit-source)) {
                /* collect a truncated byte sequence */
                toULength=0;
                c=b;
                प्रति(;;) {
                    utf8->toUBytes[toULength++]=b;
                    जइ(++source==sourceLimit) {
                        /* partial byte sequence at end of source */
                        utf8->toUnicodeStatus=c;
                        utf8->toULength=toULength;
                        utf8->mode=toULimit;
                        भङ्ग;
                    } अण्णहा जइ(!icu::UTF8::isValidTrail(c, b=*source, toULength, toULimit)) {
                        utf8->toULength=toULength;
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        भङ्ग;
                    }
                    c=(c<<6)+b;
                }
            } अण्णहा {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *pErrorCode=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    pToUArgs->source=(वर्ण *)source;
    pFromUArgs->target=(वर्ण *)target;
}

U_CDECL_END

/* UTF-8 converter data ----------------------------------------------------- */

स्थिर ध्रुव UConverterImpl _UTF8Impl={
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
स्थिर ध्रुव UConverterStaticData _UTF8StaticData={
    परिमाण(UConverterStaticData),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,मिच्छ,मिच्छ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ध्रुव UConverterSharedData _UTF8Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF8StaticData, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

स्थिर ध्रुव UConverterImpl _CESU8Impl={
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

स्थिर ध्रुव UConverterStaticData _CESU8StaticData={
    परिमाण(UConverterStaticData),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,मिच्छ,मिच्छ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ध्रुव UConverterSharedData _CESU8Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_CESU8StaticData, &_CESU8Impl);

#endif
