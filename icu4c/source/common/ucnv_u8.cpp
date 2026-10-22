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

#जर !UCONFIG_NO_CONVERSION

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

स्थिर स्थिरांक uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

स्थिर UBool hasCESU8Data(स्थिरांक UConverter *cnv)
{
#जर UCONFIG_ONLY_HTML_CONVERSION
    परत असत्य;
#अन्यथा
    परत cnv->sharedData == &_CESU8Data;
#endif
}
U_CDECL_BEGIN
स्थिर शून्य  U_CALLCONV ucnv_toUnicode_UTF8 (UConverterToUnicodeArgs * args,
                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    स्थिरांक अचिन्हित अक्षर *mySource = (अचिन्हित अक्षर *) args->source;
    char16_t *myTarget = args->target;
    स्थिरांक अचिन्हित अक्षर *sourceLimit = (अचिन्हित अक्षर *) args->sourceLimit;
    स्थिरांक char16_t *targetLimit = args->targetLimit;
    अचिन्हित अक्षर *toUBytes = cnv->toUBytes;
    UBool isCESU8 = hasCESU8Data(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    जर (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        जा morebytes;
    }


    पर्यंत (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        जर (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
        }
        अन्यथा
        {
            /* store the first char */
            toUBytes[0] = (अक्षर)ch;
            inBytes = U8_मोजणी_BYTES_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            पर्यंत (i < inBytes)
            {
                जर (mySource < sourceLimit)
                {
                    toUBytes[i] = (अक्षर) (ch2 = *mySource);
                    जर (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        थांबा; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                अन्यथा
                {
                    /* stores a partially calculated target*/
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t) i;
                    जा donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            जर (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                जर (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                }
                अन्यथा
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    जर (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                    }
                    अन्यथा
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UCharErrorBuffer[0] = (char16_t) ch;
                        cnv->UCharErrorBufferLength = 1;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                        थांबा;
                    }
                }
            }
            अन्यथा
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                थांबा;
            }
        }
    }

donefornow:
    जर (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (स्थिरांक अक्षर *) mySource;
}

स्थिर शून्य  U_CALLCONV ucnv_toUnicode_UTF8_OFFSETS_LOGIC (UConverterToUnicodeArgs * args,
                                                UErrorCode * err)
{
    UConverter *cnv = args->converter;
    स्थिरांक अचिन्हित अक्षर *mySource = (अचिन्हित अक्षर *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    int32_t offset_अंक = 0;
    स्थिरांक अचिन्हित अक्षर *sourceLimit = (अचिन्हित अक्षर *) args->sourceLimit;
    स्थिरांक char16_t *targetLimit = args->targetLimit;
    अचिन्हित अक्षर *toUBytes = cnv->toUBytes;
    UBool isCESU8 = hasCESU8Data(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    जर (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        जा morebytes;
    }

    पर्यंत (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        जर (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
            *(myOffsets++) = offset_अंक++;
        }
        अन्यथा
        {
            toUBytes[0] = (अक्षर)ch;
            inBytes = U8_मोजणी_BYTES_NON_ASCII(ch);
            i = 1;

morebytes:
            पर्यंत (i < inBytes)
            {
                जर (mySource < sourceLimit)
                {
                    toUBytes[i] = (अक्षर) (ch2 = *mySource);
                    जर (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        थांबा; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                अन्यथा
                {
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t)i;
                    जा donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            जर (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                जर (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                    *(myOffsets++) = offset_अंक;
                }
                अन्यथा
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    *(myOffsets++) = offset_अंक;
                    ch = U16_TRAIL(ch);
                    जर (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                        *(myOffsets++) = offset_अंक;
                    }
                    अन्यथा
                    {
                        cnv->UCharErrorBuffer[0] = (char16_t) ch;
                        cnv->UCharErrorBufferLength = 1;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
                offset_अंक += i;
            }
            अन्यथा
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                थांबा;
            }
        }
    }

donefornow:
    जर (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (स्थिरांक अक्षर *) mySource;
    args->offsets = myOffsets;
}
U_CDECL_END

U_CFUNC शून्य  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverterFromUnicodeArgs * args,
                                    UErrorCode * err)
{
    UConverter *cnv = args->converter;
    स्थिरांक char16_t *mySource = args->source;
    स्थिरांक char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    स्थिरांक uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *tempPtr;
    UChar32 ch;
    uint8_t tempBuf[4];
    int32_t indexToWrite;
    UBool isNotCESU8 = !hasCESU8Data(cnv);

    जर (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        जा lowsurrogate;
    }

    पर्यंत (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        जर (ch < 0x80)        /* Single byte */
        {
            *(myTarget++) = (uint8_t) ch;
        }
        अन्यथा जर (ch < 0x800)  /* Double byte */
        {
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            जर (myTarget < targetLimit)
            {
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            अन्यथा
            {
                cnv->charErrorBuffer[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->charErrorBufferLength = 1;
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        अन्यथा {
            /* Check for surrogates */
            जर(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                जर (mySource < sourceLimit) {
                    /* test both code units */
                    जर(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        /* exit this condition tree */
                    }
                    अन्यथा {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        थांबा;
                    }
                }
                अन्यथा {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    थांबा;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            tempPtr = (((targetLimit - myTarget) >= 4) ? myTarget : tempBuf);

            जर (ch <= MAXIMUM_UCS2) {
                indexToWrite = 2;
                tempPtr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            अन्यथा {
                indexToWrite = 3;
                tempPtr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                tempPtr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            tempPtr[indexToWrite-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            tempPtr[indexToWrite] = (uint8_t) ((ch & 0x3f) | 0x80);

            जर (tempPtr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (indexToWrite + 1);
            }
            अन्यथा {
                /* We might run out of room soon. Write it slowly. */
                साठी (; tempPtr <= (tempBuf + indexToWrite); tempPtr++) {
                    जर (myTarget < targetLimit) {
                        *(myTarget++) = *tempPtr;
                    }
                    अन्यथा {
                        cnv->charErrorBuffer[cnv->charErrorBufferLength++] = *tempPtr;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
            }
        }
    }

    जर (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (अक्षर *) myTarget;
    args->source = mySource;
}

U_CFUNC शून्य  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverterFromUnicodeArgs * args,
                                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    स्थिरांक char16_t *mySource = args->source;
    int32_t *myOffsets = args->offsets;
    स्थिरांक char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    स्थिरांक uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *tempPtr;
    UChar32 ch;
    int32_t offset_अंक, nextSourceIndex;
    int32_t indexToWrite;
    uint8_t tempBuf[4];
    UBool isNotCESU8 = !hasCESU8Data(cnv);

    जर (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        offset_अंक = -1;
        nextSourceIndex = 0;
        जा lowsurrogate;
    } अन्यथा {
        offset_अंक = 0;
    }

    पर्यंत (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        जर (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = offset_अंक++;
            *(myTarget++) = (अक्षर) ch;
        }
        अन्यथा जर (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = offset_अंक;
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            जर (myTarget < targetLimit)
            {
                *(myOffsets++) = offset_अंक++;
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            अन्यथा
            {
                cnv->charErrorBuffer[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->charErrorBufferLength = 1;
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        अन्यथा
        /* Check for surrogates */
        {
            nextSourceIndex = offset_अंक + 1;

            जर(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                जर (mySource < sourceLimit) {
                    /* test both code units */
                    जर(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        ++nextSourceIndex;
                        /* exit this condition tree */
                    }
                    अन्यथा {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        थांबा;
                    }
                }
                अन्यथा {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    थांबा;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            tempPtr = (((targetLimit - myTarget) >= 4) ? myTarget : tempBuf);

            जर (ch <= MAXIMUM_UCS2) {
                indexToWrite = 2;
                tempPtr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            अन्यथा {
                indexToWrite = 3;
                tempPtr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                tempPtr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            tempPtr[indexToWrite-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            tempPtr[indexToWrite] = (uint8_t) ((ch & 0x3f) | 0x80);

            जर (tempPtr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (indexToWrite + 1);
                myOffsets[0] = offset_अंक;
                myOffsets[1] = offset_अंक;
                myOffsets[2] = offset_अंक;
                जर (indexToWrite >= 3) {
                    myOffsets[3] = offset_अंक;
                }
                myOffsets += (indexToWrite + 1);
            }
            अन्यथा {
                /* We might run out of room soon. Write it slowly. */
                साठी (; tempPtr <= (tempBuf + indexToWrite); tempPtr++) {
                    जर (myTarget < targetLimit)
                    {
                        *(myOffsets++) = offset_अंक;
                        *(myTarget++) = *tempPtr;
                    }
                    अन्यथा
                    {
                        cnv->charErrorBuffer[cnv->charErrorBufferLength++] = *tempPtr;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }
            }
            offset_अंक = nextSourceIndex;
        }
    }

    जर (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (अक्षर *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

U_CDECL_BEGIN
स्थिर UChar32 U_CALLCONV ucnv_getNextUChar_UTF8(UConverterToUnicodeArgs *args,
                                               UErrorCode *err) {
    UConverter *cnv;
    स्थिरांक uint8_t *sourceInitial;
    स्थिरांक uint8_t *source;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = args->converter;
    sourceInitial = source = (स्थिरांक uint8_t *)args->source;
    जर (source >= (स्थिरांक uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        परत 0xffff;
    }

    myByte = *(source++);
    जर (U8_IS_SINGLE(myByte))
    {
        args->source = (स्थिरांक अक्षर *)source;
        परत (UChar32)myByte;
    }

    uint16_t मोजणी_Trail_Bytes = U8_मोजणी_TRAIL_BYTES(myByte);
    जर (मोजणी_Trail_Bytes == 0) {
        cnv->toUBytes[0] = myByte;
        cnv->toULength = 1;
        *err = U_ILLEGAL_CHAR_FOUND;
        args->source = (स्थिरांक अक्षर *)source;
        परत 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    जर (((स्थिरांक अक्षर *)source + मोजणी_Trail_Bytes) > args->sourceLimit)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extraBytesToWrite = मोजणी_Trail_Bytes + 1;
        cnv->toUBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_FOUND;
        पर्यंत(source < (स्थिरांक uint8_t *)args->sourceLimit) {
            uint8_t b = *source;
            जर(icu::UTF8::isValidTrail(myByte, b, i, extraBytesToWrite)) {
                cnv->toUBytes[i++] = b;
                ++source;
            } अन्यथा {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_FOUND;
                थांबा;
            }
        }
        cnv->toULength = i;
        args->source = (स्थिरांक अक्षर *)source;
        परत 0xffff;
    }

    ch = myByte << 6;
    जर(मोजणी_Trail_Bytes == 2) {
        uint8_t t1 = *source, t2;
        जर(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source)) {
            args->source = (स्थिरांक अक्षर *)(source + 1);
            परत (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } अन्यथा जर(मोजणी_Trail_Bytes == 1) {
        uint8_t t1 = *source;
        जर(U8_IS_TRAIL(t1)) {
            args->source = (स्थिरांक अक्षर *)(source + 1);
            परत (ch + t1) - offsetsFromUTF8[2];
        }
    } अन्यथा {  // countTrailBytes == 3
        uint8_t t1 = *source, t2, t3;
        जर(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source) &&
                U8_IS_TRAIL(t3 = *++source)) {
            args->source = (स्थिरांक अक्षर *)(source + 1);
            परत (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    args->source = (स्थिरांक अक्षर *)source;

    साठी(i = 0; sourceInitial < source; ++i) {
        cnv->toUBytes[i] = *sourceInitial++;
    }
    cnv->toULength = i;
    *err = U_ILLEGAL_CHAR_FOUND;
    परत 0xffff;
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
    स्थिरांक uint8_t *source, *sourceLimit;
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
    जर(utf8->toULength > 0) {
        toULength=oldToULength=utf8->toULength;
        toULimit=(int8_t)utf8->mode;
        c=(UChar32)utf8->toUnicodeStatus;
    } अन्यथा {
        toULength=oldToULength=toULimit=0;
        c = 0;
    }

    count=(int32_t)(sourceLimit-source)+oldToULength;
    जर(count<toULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } अन्यथा जर(targetCapacity<toULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *pErrorCode=U_USING_DEFAULT_WARNING;
        परत;
    } अन्यथा {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        जर(count>targetCapacity) {
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

    जर(c!=0) {
        utf8->toUnicodeStatus=0;
        utf8->toULength=0;
        जा moreBytes;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    पर्यंत(count>0) {
        b=*source++;
        जर(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *target++=b;
            --count;
            सुरूठेवा;
        } अन्यथा {
            जर(b>=0xe0) {
                जर( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=source[0]) &&
                    U8_IS_TRAIL(t2=source[1])
                ) {
                    source+=2;
                    *target++=b;
                    *target++=t1;
                    *target++=t2;
                    count-=3;
                    सुरूठेवा;
                }
            } अन्यथा {
                जर( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*source)
                ) {
                    ++source;
                    *target++=b;
                    *target++=t1;
                    count-=2;
                    सुरूठेवा;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            oldToULength=0;
            toULength=1;
            toULimit=U8_मोजणी_BYTES_NON_ASCII(b);
            c=b;
moreBytes:
            पर्यंत(toULength<toULimit) {
                जर(source<sourceLimit) {
                    b=*source;
                    जर(icu::UTF8::isValidTrail(c, b, toULength, toULimit)) {
                        ++source;
                        ++toULength;
                        c=(c<<6)+b;
                    } अन्यथा {
                        थांबा; /* sequence too short, stop with toULength<toULimit */
                    }
                } अन्यथा {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    source-=(toULength-oldToULength);
                    पर्यंत(oldToULength<toULength) {
                        utf8->toUBytes[oldToULength++]=*source++;
                    }
                    utf8->toUnicodeStatus=c;
                    utf8->toULength=toULength;
                    utf8->mode=toULimit;
                    pToUArgs->source=(अक्षर *)source;
                    pFromUArgs->target=(अक्षर *)target;
                    परत;
                }
            }

            जर(toULength!=toULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                source-=(toULength-oldToULength);
                पर्यंत(oldToULength<toULength) {
                    utf8->toUBytes[oldToULength++]=*source++;
                }
                utf8->toULength=toULength;
                pToUArgs->source=(अक्षर *)source;
                pFromUArgs->target=(अक्षर *)target;
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                परत;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                साठी(i=0; i<oldToULength; ++i) {
                    *target++=utf8->toUBytes[i];
                }
                source-=(toULength-oldToULength);
                साठी(; i<toULength; ++i) {
                    *target++=*source++;
                }
                count-=toULength;
            }
        }
    }
    U_ASSERT(count>=0);

    जर(U_SUCCESS(*pErrorCode) && source<sourceLimit) {
        जर(target==(स्थिरांक uint8_t *)pFromUArgs->targetLimit) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        } अन्यथा {
            b=*source;
            toULimit=U8_मोजणी_BYTES(b);
            जर(toULimit>(sourceLimit-source)) {
                /* collect a truncated byte sequence */
                toULength=0;
                c=b;
                साठी(;;) {
                    utf8->toUBytes[toULength++]=b;
                    जर(++source==sourceLimit) {
                        /* partial byte sequence at end of source */
                        utf8->toUnicodeStatus=c;
                        utf8->toULength=toULength;
                        utf8->mode=toULimit;
                        थांबा;
                    } अन्यथा जर(!icu::UTF8::isValidTrail(c, b=*source, toULength, toULimit)) {
                        utf8->toULength=toULength;
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        थांबा;
                    }
                    c=(c<<6)+b;
                }
            } अन्यथा {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *pErrorCode=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    pToUArgs->source=(अक्षर *)source;
    pFromUArgs->target=(अक्षर *)target;
}

U_CDECL_END

/* UTF-8 converter data ----------------------------------------------------- */

स्थिर स्थिरांक UConverterImpl _UTF8Impl={
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
स्थिर स्थिरांक UConverterStaticData _UTF8StaticData={
    आकार(UConverterStaticData),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,असत्य,असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


स्थिरांक UConverterSharedData _UTF8Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF8StaticData, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

स्थिर स्थिरांक UConverterImpl _CESU8Impl={
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

स्थिर स्थिरांक UConverterStaticData _CESU8StaticData={
    आकार(UConverterStaticData),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,असत्य,असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


स्थिरांक UConverterSharedData _CESU8Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_CESU8StaticData, &_CESU8Impl);

#endif
