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

#include "unicode/utypes.h"

#ប្រសិនបើ !UCONFIG_NO_CONVERSION

#include "unicode/ucnv.h"
#include "unicode/utf.h"
#include "unicode/utf8.h"
#include "unicode/utf16.h"
#include "uassert.h"
#include "ucnv_bld.h"
#include "ucnv_cnv.h"
#include "cmemory.h"
#include "ustr_imp.h"

/* Prototypes --------------------------------------------------------------- */

/* Keep these here to make finicky compilers happy */

U_CFUNC void ucnv_fromUnicode_UTF8(UConverterFromUnicodeArgs *args,
                                           UErrorCode *err);
U_CFUNC void ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverterFromUnicodeArgs *args,
                                                        UErrorCode *err);


/* UTF-8 -------------------------------------------------------------------- */

#define MAXIMUM_UCS2            0x0000FFFF

static const uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

static UBool hasCESU8Data(const UConverter *cnv)
{
#ប្រសិនបើ UCONFIG_ONLY_HTML_CONVERSION
    ត្រឡប់ false;
#បើមិនដូច្នេះ
    ត្រឡប់ cnv->sharedData == &_CESU8Data;
#endif
}
U_CDECL_BEGIN
static void  U_CALLCONV ucnv_toUnicode_UTF8 (UConverterToUnicodeArgs * args,
                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    const unsigned var_na *mySource = (unsigned var_na *) args->source;
    char16_t *myTarget = args->target;
    const unsigned var_na *sourceLimit = (unsigned var_na *) args->sourceLimit;
    const char16_t *targetLimit = args->targetLimit;
    unsigned var_na *toUBytes = cnv->toUBytes;
    UBool isCESU8 = hasCESU8Data(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    ប្រសិនបើ (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        goto morebytes;
    }


    ខណៈពេល (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        ប្រសិនបើ (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
        }
        បើមិនដូច្នេះ
        {
            /* store the first char */
            toUBytes[0] = (var_na)ch;
            inBytes = U8_COUNT_BYTES_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            ខណៈពេល (i < inBytes)
            {
                ប្រសិនបើ (mySource < sourceLimit)
                {
                    toUBytes[i] = (var_na) (ch2 = *mySource);
                    ប្រសិនបើ (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        bha_nga; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                បើមិនដូច្នេះ
                {
                    /* stores a partially calculated target*/
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t) i;
                    goto donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            ប្រសិនបើ (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                ប្រសិនបើ (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                }
                បើមិនដូច្នេះ
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    ប្រសិនបើ (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                    }
                    បើមិនដូច្នេះ
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UCharErrorBuffer[0] = (char16_t) ch;
                        cnv->UCharErrorBufferLength = 1;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                        bha_nga;
                    }
                }
            }
            បើមិនដូច្នេះ
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                bha_nga;
            }
        }
    }

donefornow:
    ប្រសិនបើ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (const var_na *) mySource;
}

static void  U_CALLCONV ucnv_toUnicode_UTF8_OFFSETS_LOGIC (UConverterToUnicodeArgs * args,
                                                UErrorCode * err)
{
    UConverter *cnv = args->converter;
    const unsigned var_na *mySource = (unsigned var_na *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    int32_t offsetNum = 0;
    const unsigned var_na *sourceLimit = (unsigned var_na *) args->sourceLimit;
    const char16_t *targetLimit = args->targetLimit;
    unsigned var_na *toUBytes = cnv->toUBytes;
    UBool isCESU8 = hasCESU8Data(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    ប្រសិនបើ (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        goto morebytes;
    }

    ខណៈពេល (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        ប្រសិនបើ (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
            *(myOffsets++) = offsetNum++;
        }
        បើមិនដូច្នេះ
        {
            toUBytes[0] = (var_na)ch;
            inBytes = U8_COUNT_BYTES_NON_ASCII(ch);
            i = 1;

morebytes:
            ខណៈពេល (i < inBytes)
            {
                ប្រសិនបើ (mySource < sourceLimit)
                {
                    toUBytes[i] = (var_na) (ch2 = *mySource);
                    ប្រសិនបើ (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        bha_nga; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                បើមិនដូច្នេះ
                {
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t)i;
                    goto donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            ប្រសិនបើ (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                ប្រសិនបើ (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                    *(myOffsets++) = offsetNum;
                }
                បើមិនដូច្នេះ
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    *(myOffsets++) = offsetNum;
                    ch = U16_TRAIL(ch);
                    ប្រសិនបើ (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                        *(myOffsets++) = offsetNum;
                    }
                    បើមិនដូច្នេះ
                    {
                        cnv->UCharErrorBuffer[0] = (char16_t) ch;
                        cnv->UCharErrorBufferLength = 1;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
                offsetNum += i;
            }
            បើមិនដូច្នេះ
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                bha_nga;
            }
        }
    }

donefornow:
    ប្រសិនបើ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (const var_na *) mySource;
    args->offsets = myOffsets;
}
U_CDECL_END

U_CFUNC void  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverterFromUnicodeArgs * args,
                                    UErrorCode * err)
{
    UConverter *cnv = args->converter;
    const char16_t *mySource = args->source;
    const char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    const uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *tempPtr;
    UChar32 ch;
    uint8_t tempBuf[4];
    int32_t indexToWrite;
    UBool isNotCESU8 = !hasCESU8Data(cnv);

    ប្រសិនបើ (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        goto lowsurrogate;
    }

    ខណៈពេល (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        ប្រសិនបើ (ch < 0x80)        /* Single byte */
        {
            *(myTarget++) = (uint8_t) ch;
        }
        បើមិនដូច្នេះ ប្រសិនបើ (ch < 0x800)  /* Double byte */
        {
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            ប្រសិនបើ (myTarget < targetLimit)
            {
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            បើមិនដូច្នេះ
            {
                cnv->charErrorBuffer[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->charErrorBufferLength = 1;
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        បើមិនដូច្នេះ {
            /* Check for surrogates */
            ប្រសិនបើ(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                ប្រសិនបើ (mySource < sourceLimit) {
                    /* test both code units */
                    ប្រសិនបើ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        /* exit this condition tree */
                    }
                    បើមិនដូច្នេះ {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        bha_nga;
                    }
                }
                បើមិនដូច្នេះ {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    bha_nga;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            tempPtr = (((targetLimit - myTarget) >= 4) ? myTarget : tempBuf);

            ប្រសិនបើ (ch <= MAXIMUM_UCS2) {
                indexToWrite = 2;
                tempPtr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            បើមិនដូច្នេះ {
                indexToWrite = 3;
                tempPtr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                tempPtr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            tempPtr[indexToWrite-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            tempPtr[indexToWrite] = (uint8_t) ((ch & 0x3f) | 0x80);

            ប្រសិនបើ (tempPtr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (indexToWrite + 1);
            }
            បើមិនដូច្នេះ {
                /* We might run out of room soon. Write it slowly. */
                សម្រាប់ (; tempPtr <= (tempBuf + indexToWrite); tempPtr++) {
                    ប្រសិនបើ (myTarget < targetLimit) {
                        *(myTarget++) = *tempPtr;
                    }
                    បើមិនដូច្នេះ {
                        cnv->charErrorBuffer[cnv->charErrorBufferLength++] = *tempPtr;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
            }
        }
    }

    ប្រសិនបើ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (var_na *) myTarget;
    args->source = mySource;
}

U_CFUNC void  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverterFromUnicodeArgs * args,
                                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    const char16_t *mySource = args->source;
    int32_t *myOffsets = args->offsets;
    const char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    const uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *tempPtr;
    UChar32 ch;
    int32_t offsetNum, nextSourceIndex;
    int32_t indexToWrite;
    uint8_t tempBuf[4];
    UBool isNotCESU8 = !hasCESU8Data(cnv);

    ប្រសិនបើ (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        offsetNum = -1;
        nextSourceIndex = 0;
        goto lowsurrogate;
    } បើមិនដូច្នេះ {
        offsetNum = 0;
    }

    ខណៈពេល (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        ប្រសិនបើ (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = offsetNum++;
            *(myTarget++) = (var_na) ch;
        }
        បើមិនដូច្នេះ ប្រសិនបើ (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = offsetNum;
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            ប្រសិនបើ (myTarget < targetLimit)
            {
                *(myOffsets++) = offsetNum++;
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            បើមិនដូច្នេះ
            {
                cnv->charErrorBuffer[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->charErrorBufferLength = 1;
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        បើមិនដូច្នេះ
        /* Check for surrogates */
        {
            nextSourceIndex = offsetNum + 1;

            ប្រសិនបើ(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                ប្រសិនបើ (mySource < sourceLimit) {
                    /* test both code units */
                    ប្រសិនបើ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        ++nextSourceIndex;
                        /* exit this condition tree */
                    }
                    បើមិនដូច្នេះ {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        bha_nga;
                    }
                }
                បើមិនដូច្នេះ {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    bha_nga;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            tempPtr = (((targetLimit - myTarget) >= 4) ? myTarget : tempBuf);

            ប្រសិនបើ (ch <= MAXIMUM_UCS2) {
                indexToWrite = 2;
                tempPtr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            បើមិនដូច្នេះ {
                indexToWrite = 3;
                tempPtr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                tempPtr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            tempPtr[indexToWrite-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            tempPtr[indexToWrite] = (uint8_t) ((ch & 0x3f) | 0x80);

            ប្រសិនបើ (tempPtr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (indexToWrite + 1);
                myOffsets[0] = offsetNum;
                myOffsets[1] = offsetNum;
                myOffsets[2] = offsetNum;
                ប្រសិនបើ (indexToWrite >= 3) {
                    myOffsets[3] = offsetNum;
                }
                myOffsets += (indexToWrite + 1);
            }
            បើមិនដូច្នេះ {
                /* We might run out of room soon. Write it slowly. */
                សម្រាប់ (; tempPtr <= (tempBuf + indexToWrite); tempPtr++) {
                    ប្រសិនបើ (myTarget < targetLimit)
                    {
                        *(myOffsets++) = offsetNum;
                        *(myTarget++) = *tempPtr;
                    }
                    បើមិនដូច្នេះ
                    {
                        cnv->charErrorBuffer[cnv->charErrorBufferLength++] = *tempPtr;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
            }
            offsetNum = nextSourceIndex;
        }
    }

    ប្រសិនបើ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (var_na *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

U_CDECL_BEGIN
static UChar32 U_CALLCONV ucnv_getNextUChar_UTF8(UConverterToUnicodeArgs *args,
                                               UErrorCode *err) {
    UConverter *cnv;
    const uint8_t *sourceInitial;
    const uint8_t *source;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = args->converter;
    sourceInitial = source = (const uint8_t *)args->source;
    ប្រសិនបើ (source >= (const uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        ត្រឡប់ 0xffff;
    }

    myByte = *(source++);
    ប្រសិនបើ (U8_IS_SINGLE(myByte))
    {
        args->source = (const var_na *)source;
        ត្រឡប់ (UChar32)myByte;
    }

    uint16_t countTrailBytes = U8_COUNT_TRAIL_BYTES(myByte);
    ប្រសិនបើ (countTrailBytes == 0) {
        cnv->toUBytes[0] = myByte;
        cnv->toULength = 1;
        *err = U_ILLEGAL_CHAR_FOUND;
        args->source = (const var_na *)source;
        ត្រឡប់ 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    ប្រសិនបើ (((const var_na *)source + countTrailBytes) > args->sourceLimit)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extraBytesToWrite = countTrailBytes + 1;
        cnv->toUBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_FOUND;
        ខណៈពេល(source < (const uint8_t *)args->sourceLimit) {
            uint8_t b = *source;
            ប្រសិនបើ(icu::UTF8::isValidTrail(myByte, b, i, extraBytesToWrite)) {
                cnv->toUBytes[i++] = b;
                ++source;
            } បើមិនដូច្នេះ {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_FOUND;
                bha_nga;
            }
        }
        cnv->toULength = i;
        args->source = (const var_na *)source;
        ត្រឡប់ 0xffff;
    }

    ch = myByte << 6;
    ប្រសិនបើ(countTrailBytes == 2) {
        uint8_t t1 = *source, t2;
        ប្រសិនបើ(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source)) {
            args->source = (const var_na *)(source + 1);
            ត្រឡប់ (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } បើមិនដូច្នេះ ប្រសិនបើ(countTrailBytes == 1) {
        uint8_t t1 = *source;
        ប្រសិនបើ(U8_IS_TRAIL(t1)) {
            args->source = (const var_na *)(source + 1);
            ត្រឡប់ (ch + t1) - offsetsFromUTF8[2];
        }
    } បើមិនដូច្នេះ {  // countTrailBytes == 3
        uint8_t t1 = *source, t2, t3;
        ប្រសិនបើ(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source) &&
                U8_IS_TRAIL(t3 = *++source)) {
            args->source = (const var_na *)(source + 1);
            ត្រឡប់ (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    args->source = (const var_na *)source;

    សម្រាប់(i = 0; sourceInitial < source; ++i) {
        cnv->toUBytes[i] = *sourceInitial++;
    }
    cnv->toULength = i;
    *err = U_ILLEGAL_CHAR_FOUND;
    ត្រឡប់ 0xffff;
} 
U_CDECL_END

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
static void U_CALLCONV
ucnv_UTF8FromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                  UConverterToUnicodeArgs *pToUArgs,
                  UErrorCode *pErrorCode) {
    UConverter *utf8;
    const uint8_t *source, *sourceLimit;
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
    ប្រសិនបើ(utf8->toULength > 0) {
        toULength=oldToULength=utf8->toULength;
        toULimit=(int8_t)utf8->mode;
        c=(UChar32)utf8->toUnicodeStatus;
    } បើមិនដូច្នេះ {
        toULength=oldToULength=toULimit=0;
        c = 0;
    }

    count=(int32_t)(sourceLimit-source)+oldToULength;
    ប្រសិនបើ(count<toULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } បើមិនដូច្នេះ ប្រសិនបើ(targetCapacity<toULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *pErrorCode=U_USING_DEFAULT_WARNING;
        ត្រឡប់;
    } បើមិនដូច្នេះ {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        ប្រសិនបើ(count>targetCapacity) {
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

    ប្រសិនបើ(c!=0) {
        utf8->toUnicodeStatus=0;
        utf8->toULength=0;
        goto moreBytes;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    ខណៈពេល(count>0) {
        b=*source++;
        ប្រសិនបើ(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *target++=b;
            --count;
            continue;
        } បើមិនដូច្នេះ {
            ប្រសិនបើ(b>=0xe0) {
                ប្រសិនបើ( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=source[0]) &&
                    U8_IS_TRAIL(t2=source[1])
                ) {
                    source+=2;
                    *target++=b;
                    *target++=t1;
                    *target++=t2;
                    count-=3;
                    continue;
                }
            } បើមិនដូច្នេះ {
                ប្រសិនបើ( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*source)
                ) {
                    ++source;
                    *target++=b;
                    *target++=t1;
                    count-=2;
                    continue;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            oldToULength=0;
            toULength=1;
            toULimit=U8_COUNT_BYTES_NON_ASCII(b);
            c=b;
moreBytes:
            ខណៈពេល(toULength<toULimit) {
                ប្រសិនបើ(source<sourceLimit) {
                    b=*source;
                    ប្រសិនបើ(icu::UTF8::isValidTrail(c, b, toULength, toULimit)) {
                        ++source;
                        ++toULength;
                        c=(c<<6)+b;
                    } បើមិនដូច្នេះ {
                        bha_nga; /* sequence too short, stop with toULength<toULimit */
                    }
                } បើមិនដូច្នេះ {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    source-=(toULength-oldToULength);
                    ខណៈពេល(oldToULength<toULength) {
                        utf8->toUBytes[oldToULength++]=*source++;
                    }
                    utf8->toUnicodeStatus=c;
                    utf8->toULength=toULength;
                    utf8->mode=toULimit;
                    pToUArgs->source=(var_na *)source;
                    pFromUArgs->target=(var_na *)target;
                    ត្រឡប់;
                }
            }

            ប្រសិនបើ(toULength!=toULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                source-=(toULength-oldToULength);
                ខណៈពេល(oldToULength<toULength) {
                    utf8->toUBytes[oldToULength++]=*source++;
                }
                utf8->toULength=toULength;
                pToUArgs->source=(var_na *)source;
                pFromUArgs->target=(var_na *)target;
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                ត្រឡប់;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                សម្រាប់(i=0; i<oldToULength; ++i) {
                    *target++=utf8->toUBytes[i];
                }
                source-=(toULength-oldToULength);
                សម្រាប់(; i<toULength; ++i) {
                    *target++=*source++;
                }
                count-=toULength;
            }
        }
    }
    U_ASSERT(count>=0);

    ប្រសិនបើ(U_SUCCESS(*pErrorCode) && source<sourceLimit) {
        ប្រសិនបើ(target==(const uint8_t *)pFromUArgs->targetLimit) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        } បើមិនដូច្នេះ {
            b=*source;
            toULimit=U8_COUNT_BYTES(b);
            ប្រសិនបើ(toULimit>(sourceLimit-source)) {
                /* collect a truncated byte sequence */
                toULength=0;
                c=b;
                សម្រាប់(;;) {
                    utf8->toUBytes[toULength++]=b;
                    ប្រសិនបើ(++source==sourceLimit) {
                        /* partial byte sequence at end of source */
                        utf8->toUnicodeStatus=c;
                        utf8->toULength=toULength;
                        utf8->mode=toULimit;
                        bha_nga;
                    } បើមិនដូច្នេះ ប្រសិនបើ(!icu::UTF8::isValidTrail(c, b=*source, toULength, toULimit)) {
                        utf8->toULength=toULength;
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        bha_nga;
                    }
                    c=(c<<6)+b;
                }
            } បើមិនដូច្នេះ {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *pErrorCode=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    pToUArgs->source=(var_na *)source;
    pFromUArgs->target=(var_na *)target;
}

U_CDECL_END

/* UTF-8 converter data ----------------------------------------------------- */

static const UConverterImpl _UTF8Impl={
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
static const UConverterStaticData _UTF8StaticData={
    parimaa_na(UConverterStaticData),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,false,false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


const UConverterSharedData _UTF8Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF8StaticData, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

static const UConverterImpl _CESU8Impl={
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

static const UConverterStaticData _CESU8StaticData={
    parimaa_na(UConverterStaticData),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,false,false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


const UConverterSharedData _CESU8Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_CESU8StaticData, &_CESU8Impl);

#endif
