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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_CONVERSION

#અન્તર્ભાવ "unicode/ucnv.h"
#અન્તર્ભાવ "unicode/utf.h"
#અન્તર્ભાવ "unicode/utf8.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ucnv_bld.h"
#અન્તર્ભાવ "ucnv_cnv.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "ustr_imp.h"

/* Prototypes --------------------------------------------------------------- */

/* Keep these here to make finicky compilers happy */

U_CFUNC શૂન્ય ucnv_fromUnicode_UTF8(UConverterFromUnicodeArgs *args,
                                           UErrorCode *err);
U_CFUNC શૂન્ય ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverterFromUnicodeArgs *args,
                                                        UErrorCode *err);


/* UTF-8 -------------------------------------------------------------------- */

#define MAXIMUM_UCS2            0x0000FFFF

સ્થિર અચલ uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

સ્થિર UBool hasCESU8Data(અચલ UConverter *cnv)
{
#જો UCONFIG_ONLY_HTML_CONVERSION
    પાછા ખોટું;
#નહીંતર
    પાછા cnv->sharedData == &_CESU8Data;
#endif
}
U_CDECL_BEGIN
સ્થિર શૂન્ય  U_CALLCONV ucnv_toUnicode_UTF8 (UConverterToUnicodeArgs * args,
                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    અચલ unsigned અક્ષર *mySource = (unsigned અક્ષર *) args->source;
    char16_t *myTarget = args->target;
    અચલ unsigned અક્ષર *sourceLimit = (unsigned અક્ષર *) args->sourceLimit;
    અચલ char16_t *targetLimit = args->targetLimit;
    unsigned અક્ષર *toUBytes = cnv->toUBytes;
    UBool isCESU8 = hasCESU8Data(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    જો (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        જાઓ morebytes;
    }


    જ્યારે (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        જો (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
        }
        નહીંતર
        {
            /* store the first char */
            toUBytes[0] = (અક્ષર)ch;
            inBytes = U8_ગણતરી_BYTES_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            જ્યારે (i < inBytes)
            {
                જો (mySource < sourceLimit)
                {
                    toUBytes[i] = (અક્ષર) (ch2 = *mySource);
                    જો (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        તોડો; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                નહીંતર
                {
                    /* stores a partially calculated target*/
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t) i;
                    જાઓ donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            જો (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                જો (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                }
                નહીંતર
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    જો (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                    }
                    નહીંતર
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UCharErrorBuffer[0] = (char16_t) ch;
                        cnv->UCharErrorBufferLength = 1;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                        તોડો;
                    }
                }
            }
            નહીંતર
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                તોડો;
            }
        }
    }

donefornow:
    જો (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (અચલ અક્ષર *) mySource;
}

સ્થિર શૂન્ય  U_CALLCONV ucnv_toUnicode_UTF8_OFFSETS_LOGIC (UConverterToUnicodeArgs * args,
                                                UErrorCode * err)
{
    UConverter *cnv = args->converter;
    અચલ unsigned અક્ષર *mySource = (unsigned અક્ષર *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    int32_t offset_અંક = 0;
    અચલ unsigned અક્ષર *sourceLimit = (unsigned અક્ષર *) args->sourceLimit;
    અચલ char16_t *targetLimit = args->targetLimit;
    unsigned અક્ષર *toUBytes = cnv->toUBytes;
    UBool isCESU8 = hasCESU8Data(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    જો (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        જાઓ morebytes;
    }

    જ્યારે (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        જો (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
            *(myOffsets++) = offset_અંક++;
        }
        નહીંતર
        {
            toUBytes[0] = (અક્ષર)ch;
            inBytes = U8_ગણતરી_BYTES_NON_ASCII(ch);
            i = 1;

morebytes:
            જ્યારે (i < inBytes)
            {
                જો (mySource < sourceLimit)
                {
                    toUBytes[i] = (અક્ષર) (ch2 = *mySource);
                    જો (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        તોડો; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                નહીંતર
                {
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t)i;
                    જાઓ donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            જો (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                જો (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                    *(myOffsets++) = offset_અંક;
                }
                નહીંતર
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    *(myOffsets++) = offset_અંક;
                    ch = U16_TRAIL(ch);
                    જો (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                        *(myOffsets++) = offset_અંક;
                    }
                    નહીંતર
                    {
                        cnv->UCharErrorBuffer[0] = (char16_t) ch;
                        cnv->UCharErrorBufferLength = 1;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
                offset_અંક += i;
            }
            નહીંતર
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                તોડો;
            }
        }
    }

donefornow:
    જો (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (અચલ અક્ષર *) mySource;
    args->offsets = myOffsets;
}
U_CDECL_END

U_CFUNC શૂન્ય  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverterFromUnicodeArgs * args,
                                    UErrorCode * err)
{
    UConverter *cnv = args->converter;
    અચલ char16_t *mySource = args->source;
    અચલ char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    અચલ uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *tempPtr;
    UChar32 ch;
    uint8_t tempBuf[4];
    int32_t indexToWrite;
    UBool isNotCESU8 = !hasCESU8Data(cnv);

    જો (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        જાઓ lowsurrogate;
    }

    જ્યારે (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        જો (ch < 0x80)        /* Single byte */
        {
            *(myTarget++) = (uint8_t) ch;
        }
        નહીંતર જો (ch < 0x800)  /* Double byte */
        {
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            જો (myTarget < targetLimit)
            {
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            નહીંતર
            {
                cnv->charErrorBuffer[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->charErrorBufferLength = 1;
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        નહીંતર {
            /* Check for surrogates */
            જો(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                જો (mySource < sourceLimit) {
                    /* test both code units */
                    જો(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        /* exit this condition tree */
                    }
                    નહીંતર {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        તોડો;
                    }
                }
                નહીંતર {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    તોડો;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            tempPtr = (((targetLimit - myTarget) >= 4) ? myTarget : tempBuf);

            જો (ch <= MAXIMUM_UCS2) {
                indexToWrite = 2;
                tempPtr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            નહીંતર {
                indexToWrite = 3;
                tempPtr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                tempPtr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            tempPtr[indexToWrite-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            tempPtr[indexToWrite] = (uint8_t) ((ch & 0x3f) | 0x80);

            જો (tempPtr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (indexToWrite + 1);
            }
            નહીંતર {
                /* We might run out of room soon. Write it slowly. */
                માટે (; tempPtr <= (tempBuf + indexToWrite); tempPtr++) {
                    જો (myTarget < targetLimit) {
                        *(myTarget++) = *tempPtr;
                    }
                    નહીંતર {
                        cnv->charErrorBuffer[cnv->charErrorBufferLength++] = *tempPtr;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
            }
        }
    }

    જો (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (અક્ષર *) myTarget;
    args->source = mySource;
}

U_CFUNC શૂન્ય  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverterFromUnicodeArgs * args,
                                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    અચલ char16_t *mySource = args->source;
    int32_t *myOffsets = args->offsets;
    અચલ char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    અચલ uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *tempPtr;
    UChar32 ch;
    int32_t offset_અંક, nextSourceIndex;
    int32_t indexToWrite;
    uint8_t tempBuf[4];
    UBool isNotCESU8 = !hasCESU8Data(cnv);

    જો (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        offset_અંક = -1;
        nextSourceIndex = 0;
        જાઓ lowsurrogate;
    } નહીંતર {
        offset_અંક = 0;
    }

    જ્યારે (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        જો (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = offset_અંક++;
            *(myTarget++) = (અક્ષર) ch;
        }
        નહીંતર જો (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = offset_અંક;
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            જો (myTarget < targetLimit)
            {
                *(myOffsets++) = offset_અંક++;
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            નહીંતર
            {
                cnv->charErrorBuffer[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->charErrorBufferLength = 1;
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        નહીંતર
        /* Check for surrogates */
        {
            nextSourceIndex = offset_અંક + 1;

            જો(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                જો (mySource < sourceLimit) {
                    /* test both code units */
                    જો(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        ++nextSourceIndex;
                        /* exit this condition tree */
                    }
                    નહીંતર {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        તોડો;
                    }
                }
                નહીંતર {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    તોડો;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            tempPtr = (((targetLimit - myTarget) >= 4) ? myTarget : tempBuf);

            જો (ch <= MAXIMUM_UCS2) {
                indexToWrite = 2;
                tempPtr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            નહીંતર {
                indexToWrite = 3;
                tempPtr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                tempPtr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            tempPtr[indexToWrite-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            tempPtr[indexToWrite] = (uint8_t) ((ch & 0x3f) | 0x80);

            જો (tempPtr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (indexToWrite + 1);
                myOffsets[0] = offset_અંક;
                myOffsets[1] = offset_અંક;
                myOffsets[2] = offset_અંક;
                જો (indexToWrite >= 3) {
                    myOffsets[3] = offset_અંક;
                }
                myOffsets += (indexToWrite + 1);
            }
            નહીંતર {
                /* We might run out of room soon. Write it slowly. */
                માટે (; tempPtr <= (tempBuf + indexToWrite); tempPtr++) {
                    જો (myTarget < targetLimit)
                    {
                        *(myOffsets++) = offset_અંક;
                        *(myTarget++) = *tempPtr;
                    }
                    નહીંતર
                    {
                        cnv->charErrorBuffer[cnv->charErrorBufferLength++] = *tempPtr;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
            }
            offset_અંક = nextSourceIndex;
        }
    }

    જો (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (અક્ષર *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

U_CDECL_BEGIN
સ્થિર UChar32 U_CALLCONV ucnv_getNextUChar_UTF8(UConverterToUnicodeArgs *args,
                                               UErrorCode *err) {
    UConverter *cnv;
    અચલ uint8_t *sourceInitial;
    અચલ uint8_t *source;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = args->converter;
    sourceInitial = source = (અચલ uint8_t *)args->source;
    જો (source >= (અચલ uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા 0xffff;
    }

    myByte = *(source++);
    જો (U8_IS_SINGLE(myByte))
    {
        args->source = (અચલ અક્ષર *)source;
        પાછા (UChar32)myByte;
    }

    uint16_t ગણતરી_Trail_Bytes = U8_ગણતરી_TRAIL_BYTES(myByte);
    જો (ગણતરી_Trail_Bytes == 0) {
        cnv->toUBytes[0] = myByte;
        cnv->toULength = 1;
        *err = U_ILLEGAL_CHAR_FOUND;
        args->source = (અચલ અક્ષર *)source;
        પાછા 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    જો (((અચલ અક્ષર *)source + ગણતરી_Trail_Bytes) > args->sourceLimit)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extraBytesToWrite = ગણતરી_Trail_Bytes + 1;
        cnv->toUBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_FOUND;
        જ્યારે(source < (અચલ uint8_t *)args->sourceLimit) {
            uint8_t b = *source;
            જો(icu::UTF8::isValidTrail(myByte, b, i, extraBytesToWrite)) {
                cnv->toUBytes[i++] = b;
                ++source;
            } નહીંતર {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_FOUND;
                તોડો;
            }
        }
        cnv->toULength = i;
        args->source = (અચલ અક્ષર *)source;
        પાછા 0xffff;
    }

    ch = myByte << 6;
    જો(ગણતરી_Trail_Bytes == 2) {
        uint8_t t1 = *source, t2;
        જો(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source)) {
            args->source = (અચલ અક્ષર *)(source + 1);
            પાછા (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } નહીંતર જો(ગણતરી_Trail_Bytes == 1) {
        uint8_t t1 = *source;
        જો(U8_IS_TRAIL(t1)) {
            args->source = (અચલ અક્ષર *)(source + 1);
            પાછા (ch + t1) - offsetsFromUTF8[2];
        }
    } નહીંતર {  // countTrailBytes == 3
        uint8_t t1 = *source, t2, t3;
        જો(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source) &&
                U8_IS_TRAIL(t3 = *++source)) {
            args->source = (અચલ અક્ષર *)(source + 1);
            પાછા (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    args->source = (અચલ અક્ષર *)source;

    માટે(i = 0; sourceInitial < source; ++i) {
        cnv->toUBytes[i] = *sourceInitial++;
    }
    cnv->toULength = i;
    *err = U_ILLEGAL_CHAR_FOUND;
    પાછા 0xffff;
} 
U_CDECL_END

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
સ્થિર શૂન્ય U_CALLCONV
ucnv_UTF8FromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                  UConverterToUnicodeArgs *pToUArgs,
                  UErrorCode *pErrorCode) {
    UConverter *utf8;
    અચલ uint8_t *source, *sourceLimit;
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
    જો(utf8->toULength > 0) {
        toULength=oldToULength=utf8->toULength;
        toULimit=(int8_t)utf8->mode;
        c=(UChar32)utf8->toUnicodeStatus;
    } નહીંતર {
        toULength=oldToULength=toULimit=0;
        c = 0;
    }

    count=(int32_t)(sourceLimit-source)+oldToULength;
    જો(count<toULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } નહીંતર જો(targetCapacity<toULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *pErrorCode=U_USING_DEFAULT_WARNING;
        પાછા;
    } નહીંતર {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        જો(count>targetCapacity) {
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

    જો(c!=0) {
        utf8->toUnicodeStatus=0;
        utf8->toULength=0;
        જાઓ moreBytes;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    જ્યારે(count>0) {
        b=*source++;
        જો(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *target++=b;
            --count;
            ચાલુ;
        } નહીંતર {
            જો(b>=0xe0) {
                જો( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=source[0]) &&
                    U8_IS_TRAIL(t2=source[1])
                ) {
                    source+=2;
                    *target++=b;
                    *target++=t1;
                    *target++=t2;
                    count-=3;
                    ચાલુ;
                }
            } નહીંતર {
                જો( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*source)
                ) {
                    ++source;
                    *target++=b;
                    *target++=t1;
                    count-=2;
                    ચાલુ;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            oldToULength=0;
            toULength=1;
            toULimit=U8_ગણતરી_BYTES_NON_ASCII(b);
            c=b;
moreBytes:
            જ્યારે(toULength<toULimit) {
                જો(source<sourceLimit) {
                    b=*source;
                    જો(icu::UTF8::isValidTrail(c, b, toULength, toULimit)) {
                        ++source;
                        ++toULength;
                        c=(c<<6)+b;
                    } નહીંતર {
                        તોડો; /* sequence too short, stop with toULength<toULimit */
                    }
                } નહીંતર {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    source-=(toULength-oldToULength);
                    જ્યારે(oldToULength<toULength) {
                        utf8->toUBytes[oldToULength++]=*source++;
                    }
                    utf8->toUnicodeStatus=c;
                    utf8->toULength=toULength;
                    utf8->mode=toULimit;
                    pToUArgs->source=(અક્ષર *)source;
                    pFromUArgs->target=(અક્ષર *)target;
                    પાછા;
                }
            }

            જો(toULength!=toULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                source-=(toULength-oldToULength);
                જ્યારે(oldToULength<toULength) {
                    utf8->toUBytes[oldToULength++]=*source++;
                }
                utf8->toULength=toULength;
                pToUArgs->source=(અક્ષર *)source;
                pFromUArgs->target=(અક્ષર *)target;
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                પાછા;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                માટે(i=0; i<oldToULength; ++i) {
                    *target++=utf8->toUBytes[i];
                }
                source-=(toULength-oldToULength);
                માટે(; i<toULength; ++i) {
                    *target++=*source++;
                }
                count-=toULength;
            }
        }
    }
    U_ASSERT(count>=0);

    જો(U_SUCCESS(*pErrorCode) && source<sourceLimit) {
        જો(target==(અચલ uint8_t *)pFromUArgs->targetLimit) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        } નહીંતર {
            b=*source;
            toULimit=U8_ગણતરી_BYTES(b);
            જો(toULimit>(sourceLimit-source)) {
                /* collect a truncated byte sequence */
                toULength=0;
                c=b;
                માટે(;;) {
                    utf8->toUBytes[toULength++]=b;
                    જો(++source==sourceLimit) {
                        /* partial byte sequence at end of source */
                        utf8->toUnicodeStatus=c;
                        utf8->toULength=toULength;
                        utf8->mode=toULimit;
                        તોડો;
                    } નહીંતર જો(!icu::UTF8::isValidTrail(c, b=*source, toULength, toULimit)) {
                        utf8->toULength=toULength;
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        તોડો;
                    }
                    c=(c<<6)+b;
                }
            } નહીંતર {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *pErrorCode=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    pToUArgs->source=(અક્ષર *)source;
    pFromUArgs->target=(અક્ષર *)target;
}

U_CDECL_END

/* UTF-8 converter data ----------------------------------------------------- */

સ્થિર અચલ UConverterImpl _UTF8Impl={
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
સ્થિર અચલ UConverterStaticData _UTF8StaticData={
    કદ(UConverterStaticData),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,ખોટું,ખોટું,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


અચલ UConverterSharedData _UTF8Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF8StaticData, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

સ્થિર અચલ UConverterImpl _CESU8Impl={
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

સ્થિર અચલ UConverterStaticData _CESU8StaticData={
    કદ(UConverterStaticData),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,ખોટું,ખોટું,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


અચલ UConverterSharedData _CESU8Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_CESU8StaticData, &_CESU8Impl);

#endif
