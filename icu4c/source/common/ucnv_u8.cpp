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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_CONVERSION

#അന്തര്ഭാവ "unicode/ucnv.h"
#അന്തര്ഭാവ "unicode/utf.h"
#അന്തര്ഭാവ "unicode/utf8.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ucnv_bld.h"
#അന്തര്ഭാവ "ucnv_cnv.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "ustr_imp.h"

/* Prototypes --------------------------------------------------------------- */

/* Keep these here to make finicky compilers happy */

U_CFUNC ശൂന്യം ucnv_fromUnicode_UTF8(UConverterFromUnicodeArgs *args,
                                           UErrorCode *err);
U_CFUNC ശൂന്യം ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverterFromUnicodeArgs *args,
                                                        UErrorCode *err);


/* UTF-8 -------------------------------------------------------------------- */

#define MAXIMUM_UCS2            0x0000FFFF

സ്ഥിതം സ്ഥിരം uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

സ്ഥിതം UBool hasCESU8Data(സ്ഥിരം UConverter *cnv)
{
#എങ്കിൽ UCONFIG_ONLY_HTML_CONVERSION
    തിരികെ അസത്യം;
#അല്ലെങ്കിൽ
    തിരികെ cnv->sharedData == &_CESU8Data;
#endif
}
U_CDECL_BEGIN
സ്ഥിതം ശൂന്യം  U_CALLCONV ucnv_toUnicode_UTF8 (UConverterToUnicodeArgs * args,
                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    സ്ഥിരം unsigned അക്ഷരം *mySource = (unsigned അക്ഷരം *) args->source;
    char16_t *myTarget = args->target;
    സ്ഥിരം unsigned അക്ഷരം *sourceLimit = (unsigned അക്ഷരം *) args->sourceLimit;
    സ്ഥിരം char16_t *targetLimit = args->targetLimit;
    unsigned അക്ഷരം *toUBytes = cnv->toUBytes;
    UBool isCESU8 = hasCESU8Data(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    എങ്കിൽ (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        പോകുക morebytes;
    }


    ആകുമ്പോൾ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        എങ്കിൽ (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
        }
        അല്ലെങ്കിൽ
        {
            /* store the first char */
            toUBytes[0] = (അക്ഷരം)ch;
            inBytes = U8_എണ്ണം_BYTES_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            ആകുമ്പോൾ (i < inBytes)
            {
                എങ്കിൽ (mySource < sourceLimit)
                {
                    toUBytes[i] = (അക്ഷരം) (ch2 = *mySource);
                    എങ്കിൽ (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        നിർത്തുക; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                അല്ലെങ്കിൽ
                {
                    /* stores a partially calculated target*/
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t) i;
                    പോകുക donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            എങ്കിൽ (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                എങ്കിൽ (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                }
                അല്ലെങ്കിൽ
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    എങ്കിൽ (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                    }
                    അല്ലെങ്കിൽ
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UCharErrorBuffer[0] = (char16_t) ch;
                        cnv->UCharErrorBufferLength = 1;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                        നിർത്തുക;
                    }
                }
            }
            അല്ലെങ്കിൽ
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                നിർത്തുക;
            }
        }
    }

donefornow:
    എങ്കിൽ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (സ്ഥിരം അക്ഷരം *) mySource;
}

സ്ഥിതം ശൂന്യം  U_CALLCONV ucnv_toUnicode_UTF8_OFFSETS_LOGIC (UConverterToUnicodeArgs * args,
                                                UErrorCode * err)
{
    UConverter *cnv = args->converter;
    സ്ഥിരം unsigned അക്ഷരം *mySource = (unsigned അക്ഷരം *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    int32_t offset_അക്കം = 0;
    സ്ഥിരം unsigned അക്ഷരം *sourceLimit = (unsigned അക്ഷരം *) args->sourceLimit;
    സ്ഥിരം char16_t *targetLimit = args->targetLimit;
    unsigned അക്ഷരം *toUBytes = cnv->toUBytes;
    UBool isCESU8 = hasCESU8Data(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    എങ്കിൽ (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        പോകുക morebytes;
    }

    ആകുമ്പോൾ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        എങ്കിൽ (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
            *(myOffsets++) = offset_അക്കം++;
        }
        അല്ലെങ്കിൽ
        {
            toUBytes[0] = (അക്ഷരം)ch;
            inBytes = U8_എണ്ണം_BYTES_NON_ASCII(ch);
            i = 1;

morebytes:
            ആകുമ്പോൾ (i < inBytes)
            {
                എങ്കിൽ (mySource < sourceLimit)
                {
                    toUBytes[i] = (അക്ഷരം) (ch2 = *mySource);
                    എങ്കിൽ (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        നിർത്തുക; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                അല്ലെങ്കിൽ
                {
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t)i;
                    പോകുക donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            എങ്കിൽ (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                എങ്കിൽ (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                    *(myOffsets++) = offset_അക്കം;
                }
                അല്ലെങ്കിൽ
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    *(myOffsets++) = offset_അക്കം;
                    ch = U16_TRAIL(ch);
                    എങ്കിൽ (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                        *(myOffsets++) = offset_അക്കം;
                    }
                    അല്ലെങ്കിൽ
                    {
                        cnv->UCharErrorBuffer[0] = (char16_t) ch;
                        cnv->UCharErrorBufferLength = 1;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
                offset_അക്കം += i;
            }
            അല്ലെങ്കിൽ
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                നിർത്തുക;
            }
        }
    }

donefornow:
    എങ്കിൽ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (സ്ഥിരം അക്ഷരം *) mySource;
    args->offsets = myOffsets;
}
U_CDECL_END

U_CFUNC ശൂന്യം  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverterFromUnicodeArgs * args,
                                    UErrorCode * err)
{
    UConverter *cnv = args->converter;
    സ്ഥിരം char16_t *mySource = args->source;
    സ്ഥിരം char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    സ്ഥിരം uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *tempPtr;
    UChar32 ch;
    uint8_t tempBuf[4];
    int32_t indexToWrite;
    UBool isNotCESU8 = !hasCESU8Data(cnv);

    എങ്കിൽ (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        പോകുക lowsurrogate;
    }

    ആകുമ്പോൾ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        എങ്കിൽ (ch < 0x80)        /* Single byte */
        {
            *(myTarget++) = (uint8_t) ch;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (ch < 0x800)  /* Double byte */
        {
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            എങ്കിൽ (myTarget < targetLimit)
            {
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            അല്ലെങ്കിൽ
            {
                cnv->charErrorBuffer[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->charErrorBufferLength = 1;
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        അല്ലെങ്കിൽ {
            /* Check for surrogates */
            എങ്കിൽ(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                എങ്കിൽ (mySource < sourceLimit) {
                    /* test both code units */
                    എങ്കിൽ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        /* exit this condition tree */
                    }
                    അല്ലെങ്കിൽ {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        നിർത്തുക;
                    }
                }
                അല്ലെങ്കിൽ {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    നിർത്തുക;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            tempPtr = (((targetLimit - myTarget) >= 4) ? myTarget : tempBuf);

            എങ്കിൽ (ch <= MAXIMUM_UCS2) {
                indexToWrite = 2;
                tempPtr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            അല്ലെങ്കിൽ {
                indexToWrite = 3;
                tempPtr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                tempPtr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            tempPtr[indexToWrite-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            tempPtr[indexToWrite] = (uint8_t) ((ch & 0x3f) | 0x80);

            എങ്കിൽ (tempPtr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (indexToWrite + 1);
            }
            അല്ലെങ്കിൽ {
                /* We might run out of room soon. Write it slowly. */
                വേണ്ടി (; tempPtr <= (tempBuf + indexToWrite); tempPtr++) {
                    എങ്കിൽ (myTarget < targetLimit) {
                        *(myTarget++) = *tempPtr;
                    }
                    അല്ലെങ്കിൽ {
                        cnv->charErrorBuffer[cnv->charErrorBufferLength++] = *tempPtr;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
            }
        }
    }

    എങ്കിൽ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (അക്ഷരം *) myTarget;
    args->source = mySource;
}

U_CFUNC ശൂന്യം  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverterFromUnicodeArgs * args,
                                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    സ്ഥിരം char16_t *mySource = args->source;
    int32_t *myOffsets = args->offsets;
    സ്ഥിരം char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    സ്ഥിരം uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *tempPtr;
    UChar32 ch;
    int32_t offset_അക്കം, nextSourceIndex;
    int32_t indexToWrite;
    uint8_t tempBuf[4];
    UBool isNotCESU8 = !hasCESU8Data(cnv);

    എങ്കിൽ (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        offset_അക്കം = -1;
        nextSourceIndex = 0;
        പോകുക lowsurrogate;
    } അല്ലെങ്കിൽ {
        offset_അക്കം = 0;
    }

    ആകുമ്പോൾ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        എങ്കിൽ (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = offset_അക്കം++;
            *(myTarget++) = (അക്ഷരം) ch;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = offset_അക്കം;
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            എങ്കിൽ (myTarget < targetLimit)
            {
                *(myOffsets++) = offset_അക്കം++;
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            അല്ലെങ്കിൽ
            {
                cnv->charErrorBuffer[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->charErrorBufferLength = 1;
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        അല്ലെങ്കിൽ
        /* Check for surrogates */
        {
            nextSourceIndex = offset_അക്കം + 1;

            എങ്കിൽ(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                എങ്കിൽ (mySource < sourceLimit) {
                    /* test both code units */
                    എങ്കിൽ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        ++nextSourceIndex;
                        /* exit this condition tree */
                    }
                    അല്ലെങ്കിൽ {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        നിർത്തുക;
                    }
                }
                അല്ലെങ്കിൽ {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    നിർത്തുക;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            tempPtr = (((targetLimit - myTarget) >= 4) ? myTarget : tempBuf);

            എങ്കിൽ (ch <= MAXIMUM_UCS2) {
                indexToWrite = 2;
                tempPtr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            അല്ലെങ്കിൽ {
                indexToWrite = 3;
                tempPtr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                tempPtr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            tempPtr[indexToWrite-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            tempPtr[indexToWrite] = (uint8_t) ((ch & 0x3f) | 0x80);

            എങ്കിൽ (tempPtr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (indexToWrite + 1);
                myOffsets[0] = offset_അക്കം;
                myOffsets[1] = offset_അക്കം;
                myOffsets[2] = offset_അക്കം;
                എങ്കിൽ (indexToWrite >= 3) {
                    myOffsets[3] = offset_അക്കം;
                }
                myOffsets += (indexToWrite + 1);
            }
            അല്ലെങ്കിൽ {
                /* We might run out of room soon. Write it slowly. */
                വേണ്ടി (; tempPtr <= (tempBuf + indexToWrite); tempPtr++) {
                    എങ്കിൽ (myTarget < targetLimit)
                    {
                        *(myOffsets++) = offset_അക്കം;
                        *(myTarget++) = *tempPtr;
                    }
                    അല്ലെങ്കിൽ
                    {
                        cnv->charErrorBuffer[cnv->charErrorBufferLength++] = *tempPtr;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
            }
            offset_അക്കം = nextSourceIndex;
        }
    }

    എങ്കിൽ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (അക്ഷരം *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

U_CDECL_BEGIN
സ്ഥിതം UChar32 U_CALLCONV ucnv_getNextUChar_UTF8(UConverterToUnicodeArgs *args,
                                               UErrorCode *err) {
    UConverter *cnv;
    സ്ഥിരം uint8_t *sourceInitial;
    സ്ഥിരം uint8_t *source;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = args->converter;
    sourceInitial = source = (സ്ഥിരം uint8_t *)args->source;
    എങ്കിൽ (source >= (സ്ഥിരം uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ 0xffff;
    }

    myByte = *(source++);
    എങ്കിൽ (U8_IS_SINGLE(myByte))
    {
        args->source = (സ്ഥിരം അക്ഷരം *)source;
        തിരികെ (UChar32)myByte;
    }

    uint16_t എണ്ണം_Trail_Bytes = U8_എണ്ണം_TRAIL_BYTES(myByte);
    എങ്കിൽ (എണ്ണം_Trail_Bytes == 0) {
        cnv->toUBytes[0] = myByte;
        cnv->toULength = 1;
        *err = U_ILLEGAL_CHAR_FOUND;
        args->source = (സ്ഥിരം അക്ഷരം *)source;
        തിരികെ 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    എങ്കിൽ (((സ്ഥിരം അക്ഷരം *)source + എണ്ണം_Trail_Bytes) > args->sourceLimit)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extraBytesToWrite = എണ്ണം_Trail_Bytes + 1;
        cnv->toUBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_FOUND;
        ആകുമ്പോൾ(source < (സ്ഥിരം uint8_t *)args->sourceLimit) {
            uint8_t b = *source;
            എങ്കിൽ(icu::UTF8::isValidTrail(myByte, b, i, extraBytesToWrite)) {
                cnv->toUBytes[i++] = b;
                ++source;
            } അല്ലെങ്കിൽ {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_FOUND;
                നിർത്തുക;
            }
        }
        cnv->toULength = i;
        args->source = (സ്ഥിരം അക്ഷരം *)source;
        തിരികെ 0xffff;
    }

    ch = myByte << 6;
    എങ്കിൽ(എണ്ണം_Trail_Bytes == 2) {
        uint8_t t1 = *source, t2;
        എങ്കിൽ(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source)) {
            args->source = (സ്ഥിരം അക്ഷരം *)(source + 1);
            തിരികെ (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } അല്ലെങ്കിൽ എങ്കിൽ(എണ്ണം_Trail_Bytes == 1) {
        uint8_t t1 = *source;
        എങ്കിൽ(U8_IS_TRAIL(t1)) {
            args->source = (സ്ഥിരം അക്ഷരം *)(source + 1);
            തിരികെ (ch + t1) - offsetsFromUTF8[2];
        }
    } അല്ലെങ്കിൽ {  // countTrailBytes == 3
        uint8_t t1 = *source, t2, t3;
        എങ്കിൽ(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source) &&
                U8_IS_TRAIL(t3 = *++source)) {
            args->source = (സ്ഥിരം അക്ഷരം *)(source + 1);
            തിരികെ (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    args->source = (സ്ഥിരം അക്ഷരം *)source;

    വേണ്ടി(i = 0; sourceInitial < source; ++i) {
        cnv->toUBytes[i] = *sourceInitial++;
    }
    cnv->toULength = i;
    *err = U_ILLEGAL_CHAR_FOUND;
    തിരികെ 0xffff;
} 
U_CDECL_END

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
സ്ഥിതം ശൂന്യം U_CALLCONV
ucnv_UTF8FromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                  UConverterToUnicodeArgs *pToUArgs,
                  UErrorCode *pErrorCode) {
    UConverter *utf8;
    സ്ഥിരം uint8_t *source, *sourceLimit;
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
    എങ്കിൽ(utf8->toULength > 0) {
        toULength=oldToULength=utf8->toULength;
        toULimit=(int8_t)utf8->mode;
        c=(UChar32)utf8->toUnicodeStatus;
    } അല്ലെങ്കിൽ {
        toULength=oldToULength=toULimit=0;
        c = 0;
    }

    count=(int32_t)(sourceLimit-source)+oldToULength;
    എങ്കിൽ(count<toULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } അല്ലെങ്കിൽ എങ്കിൽ(targetCapacity<toULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *pErrorCode=U_USING_DEFAULT_WARNING;
        തിരികെ;
    } അല്ലെങ്കിൽ {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        എങ്കിൽ(count>targetCapacity) {
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

    എങ്കിൽ(c!=0) {
        utf8->toUnicodeStatus=0;
        utf8->toULength=0;
        പോകുക moreBytes;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    ആകുമ്പോൾ(count>0) {
        b=*source++;
        എങ്കിൽ(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *target++=b;
            --count;
            തുടരുക;
        } അല്ലെങ്കിൽ {
            എങ്കിൽ(b>=0xe0) {
                എങ്കിൽ( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=source[0]) &&
                    U8_IS_TRAIL(t2=source[1])
                ) {
                    source+=2;
                    *target++=b;
                    *target++=t1;
                    *target++=t2;
                    count-=3;
                    തുടരുക;
                }
            } അല്ലെങ്കിൽ {
                എങ്കിൽ( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*source)
                ) {
                    ++source;
                    *target++=b;
                    *target++=t1;
                    count-=2;
                    തുടരുക;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            oldToULength=0;
            toULength=1;
            toULimit=U8_എണ്ണം_BYTES_NON_ASCII(b);
            c=b;
moreBytes:
            ആകുമ്പോൾ(toULength<toULimit) {
                എങ്കിൽ(source<sourceLimit) {
                    b=*source;
                    എങ്കിൽ(icu::UTF8::isValidTrail(c, b, toULength, toULimit)) {
                        ++source;
                        ++toULength;
                        c=(c<<6)+b;
                    } അല്ലെങ്കിൽ {
                        നിർത്തുക; /* sequence too short, stop with toULength<toULimit */
                    }
                } അല്ലെങ്കിൽ {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    source-=(toULength-oldToULength);
                    ആകുമ്പോൾ(oldToULength<toULength) {
                        utf8->toUBytes[oldToULength++]=*source++;
                    }
                    utf8->toUnicodeStatus=c;
                    utf8->toULength=toULength;
                    utf8->mode=toULimit;
                    pToUArgs->source=(അക്ഷരം *)source;
                    pFromUArgs->target=(അക്ഷരം *)target;
                    തിരികെ;
                }
            }

            എങ്കിൽ(toULength!=toULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                source-=(toULength-oldToULength);
                ആകുമ്പോൾ(oldToULength<toULength) {
                    utf8->toUBytes[oldToULength++]=*source++;
                }
                utf8->toULength=toULength;
                pToUArgs->source=(അക്ഷരം *)source;
                pFromUArgs->target=(അക്ഷരം *)target;
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                തിരികെ;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                വേണ്ടി(i=0; i<oldToULength; ++i) {
                    *target++=utf8->toUBytes[i];
                }
                source-=(toULength-oldToULength);
                വേണ്ടി(; i<toULength; ++i) {
                    *target++=*source++;
                }
                count-=toULength;
            }
        }
    }
    U_ASSERT(count>=0);

    എങ്കിൽ(U_SUCCESS(*pErrorCode) && source<sourceLimit) {
        എങ്കിൽ(target==(സ്ഥിരം uint8_t *)pFromUArgs->targetLimit) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        } അല്ലെങ്കിൽ {
            b=*source;
            toULimit=U8_എണ്ണം_BYTES(b);
            എങ്കിൽ(toULimit>(sourceLimit-source)) {
                /* collect a truncated byte sequence */
                toULength=0;
                c=b;
                വേണ്ടി(;;) {
                    utf8->toUBytes[toULength++]=b;
                    എങ്കിൽ(++source==sourceLimit) {
                        /* partial byte sequence at end of source */
                        utf8->toUnicodeStatus=c;
                        utf8->toULength=toULength;
                        utf8->mode=toULimit;
                        നിർത്തുക;
                    } അല്ലെങ്കിൽ എങ്കിൽ(!icu::UTF8::isValidTrail(c, b=*source, toULength, toULimit)) {
                        utf8->toULength=toULength;
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        നിർത്തുക;
                    }
                    c=(c<<6)+b;
                }
            } അല്ലെങ്കിൽ {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *pErrorCode=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    pToUArgs->source=(അക്ഷരം *)source;
    pFromUArgs->target=(അക്ഷരം *)target;
}

U_CDECL_END

/* UTF-8 converter data ----------------------------------------------------- */

സ്ഥിതം സ്ഥിരം UConverterImpl _UTF8Impl={
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
സ്ഥിതം സ്ഥിരം UConverterStaticData _UTF8StaticData={
    വലുപ്പം(UConverterStaticData),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,അസത്യം,അസത്യം,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


സ്ഥിരം UConverterSharedData _UTF8Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF8StaticData, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

സ്ഥിതം സ്ഥിരം UConverterImpl _CESU8Impl={
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

സ്ഥിതം സ്ഥിരം UConverterStaticData _CESU8StaticData={
    വലുപ്പം(UConverterStaticData),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,അസത്യം,അസത്യം,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


സ്ഥിരം UConverterSharedData _CESU8Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_CESU8StaticData, &_CESU8Impl);

#endif
