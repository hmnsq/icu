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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "cmemory.h"

#define MAXIMUM_UCS2            0x0000FFFF
#define MAXIMUM_UTF             0x0010FFFF
#define HALF_SHIFT              10
#define HALF_BASE               0x0010000
#define HALF_MASK               0x3FF
#define SURROGATE_ऊपरी_START    0xD800
#define SURROGATE_निचला_START     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#define SURROGATE_निचला_BASE      9216

गणना {
    UCNV_NEED_TO_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
T_UConverter_toUnicode_UTF32_BE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    ध्रुव unsigned वर्ण *mySource = (unsigned वर्ण *) args->source;
    char16_t *myTarget = args->target;
    ध्रुव unsigned वर्ण *sourceLimit = (unsigned वर्ण *) args->sourceLimit;
    ध्रुव char16_t *targetLimit = args->targetLimit;
    unsigned वर्ण *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    यदि (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        गच्छ morebytes;
    }

    यावत् (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        यावत् (i < परिमाण(uint32_t)) {
            यदि (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (वर्ण) *(mySource++);
            }
            अन्यथा {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                गच्छ donefornow;
            }
        }

        यदि (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            यदि (ch <= MAXIMUM_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            अन्यथा {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                यदि (myTarget < targetLimit) {
                    *(myTarget++) = (char16_t)ch;
                }
                अन्यथा {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    भङ्ग;
                }
            }
        }
        अन्यथा {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            भङ्ग;
        }
    }

donefornow:
    यदि (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ध्रुव वर्ण *) mySource;
}

स्थिर शून्य U_CALLCONV
T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    ध्रुव unsigned वर्ण *mySource = (unsigned वर्ण *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    ध्रुव unsigned वर्ण *sourceLimit = (unsigned वर्ण *) args->sourceLimit;
    ध्रुव char16_t *targetLimit = args->targetLimit;
    unsigned वर्ण *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_अंक = 0;

    /* Restore state of current sequence */
    यदि (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        गच्छ morebytes;
    }

    यावत् (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        यावत् (i < परिमाण(uint32_t)) {
            यदि (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (वर्ण) *(mySource++);
            }
            अन्यथा {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                गच्छ donefornow;
            }
        }

        यदि (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            यदि (ch <= MAXIMUM_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = offset_अंक;
            }
            अन्यथा {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                *myOffsets++ = offset_अंक;
                ch = U16_TRAIL(ch);
                यदि (myTarget < targetLimit)
                {
                    *(myTarget++) = (char16_t)ch;
                    *(myOffsets++) = offset_अंक;
                }
                अन्यथा {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    भङ्ग;
                }
            }
        }
        अन्यथा {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            भङ्ग;
        }
        offset_अंक += i;
    }

donefornow:
    यदि (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ध्रुव वर्ण *) mySource;
    args->offsets = myOffsets;
}

स्थिर शून्य U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    ध्रुव char16_t *mySource = args->source;
    unsigned वर्ण *myTarget;
    ध्रुव char16_t *sourceLimit = args->sourceLimit;
    ध्रुव unsigned वर्ण *targetLimit = (unsigned वर्ण *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned पूर्णाङ्क indexToWrite;
    unsigned वर्ण temp[परिमाण(uint32_t)];

    यदि(mySource >= sourceLimit) {
        /* no input, nothing to do */
        प्रत्यागम;
    }

    /* write the BOM if necessary */
    यदि(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        स्थिर ध्रुव वर्ण bom[]={ 0, 0, (वर्ण)0xfeu, (वर्ण)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned वर्ण *) args->target;
    temp[0] = 0;

    यदि (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        गच्छ lowsurogate;
    }

    यावत् (mySource < sourceLimit && myTarget < targetLimit) {
        ch = *(mySource++);

        यदि (U_IS_SURROGATE(ch)) {
            यदि (U_IS_LEAD(ch)) {
lowsurogate:
                यदि (mySource < sourceLimit) {
                    ch2 = *mySource;
                    यदि (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ऊपरी_START) << HALF_SHIFT) + ch2 + SURROGATE_निचला_BASE;
                        mySource++;
                    }
                    अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        भङ्ग;
                    }
                }
                अन्यथा {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    यदि (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    भङ्ग;
                }
            }
            अन्यथा {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                भङ्ग;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[1] = (uint8_t) (ch >> 16 & 0x1F);
        temp[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        प्रति (indexToWrite = 0; indexToWrite <= परिमाण(uint32_t) - 1; indexToWrite++) {
            यदि (myTarget < targetLimit) {
                *(myTarget++) = temp[indexToWrite];
            }
            अन्यथा {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
    }

    यदि (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (वर्ण *) myTarget;
    args->source = mySource;
}

स्थिर शून्य U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    ध्रुव char16_t *mySource = args->source;
    unsigned वर्ण *myTarget;
    int32_t *myOffsets;
    ध्रुव char16_t *sourceLimit = args->sourceLimit;
    ध्रुव unsigned वर्ण *targetLimit = (unsigned वर्ण *) args->targetLimit;
    UChar32 ch, ch2;
    int32_t offset_अंक = 0;
    unsigned पूर्णाङ्क indexToWrite;
    unsigned वर्ण temp[परिमाण(uint32_t)];

    यदि(mySource >= sourceLimit) {
        /* no input, nothing to do */
        प्रत्यागम;
    }

    /* write the BOM if necessary */
    यदि(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        स्थिर ध्रुव वर्ण bom[]={ 0, 0, (वर्ण)0xfeu, (वर्ण)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned वर्ण *) args->target;
    myOffsets = args->offsets;
    temp[0] = 0;

    यदि (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        गच्छ lowsurogate;
    }

    यावत् (mySource < sourceLimit && myTarget < targetLimit) {
        ch = *(mySource++);

        यदि (U_IS_SURROGATE(ch)) {
            यदि (U_IS_LEAD(ch)) {
lowsurogate:
                यदि (mySource < sourceLimit) {
                    ch2 = *mySource;
                    यदि (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ऊपरी_START) << HALF_SHIFT) + ch2 + SURROGATE_निचला_BASE;
                        mySource++;
                    }
                    अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        भङ्ग;
                    }
                }
                अन्यथा {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    यदि (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    भङ्ग;
                }
            }
            अन्यथा {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                भङ्ग;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[1] = (uint8_t) (ch >> 16 & 0x1F);
        temp[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        प्रति (indexToWrite = 0; indexToWrite <= परिमाण(uint32_t) - 1; indexToWrite++) {
            यदि (myTarget < targetLimit) {
                *(myTarget++) = temp[indexToWrite];
                *(myOffsets++) = offset_अंक;
            }
            अन्यथा {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        offset_अंक = offset_अंक + 1 + (temp[1] != 0);
    }

    यदि (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (वर्ण *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

स्थिर UChar32 U_CALLCONV
T_UConverter_getNextUChar_UTF32_BE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    ध्रुव uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (ध्रुव uint8_t *)args->source;
    यदि (mySource >= (ध्रुव uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0xffff;
    }

    length = (int32_t)((ध्रुव uint8_t *)args->sourceLimit - mySource);
    यदि (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (ध्रुव वर्ण *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        प्रत्यागम 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[0] << 24)
            | ((UChar32)mySource[1] << 16)
            | ((UChar32)mySource[2] << 8)
            | ((UChar32)mySource[3]);

    args->source = (ध्रुव वर्ण *)(mySource + 4);
    यदि ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        प्रत्यागम myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    प्रत्यागम 0xffff;
}
U_CDECL_END
स्थिर ध्रुव UConverterImpl _UTF32BEImpl = {
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
स्थिर ध्रुव UConverterStaticData _UTF32BEStaticData = {
    परिमाण(UConverterStaticData),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, असत्य, असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ध्रुव UConverterSharedData _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF32BEStaticData, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
T_UConverter_toUnicode_UTF32_LE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    ध्रुव unsigned वर्ण *mySource = (unsigned वर्ण *) args->source;
    char16_t *myTarget = args->target;
    ध्रुव unsigned वर्ण *sourceLimit = (unsigned वर्ण *) args->sourceLimit;
    ध्रुव char16_t *targetLimit = args->targetLimit;
    unsigned वर्ण *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    यदि (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        गच्छ morebytes;
    }

    यावत् (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        यावत् (i < परिमाण(uint32_t))
        {
            यदि (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (वर्ण) *(mySource++);
            }
            अन्यथा
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                गच्छ donefornow;
            }
        }

        यदि (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            यदि (ch <= MAXIMUM_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            अन्यथा {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                यदि (myTarget < targetLimit) {
                    *(myTarget++) = (char16_t)ch;
                }
                अन्यथा {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    भङ्ग;
                }
            }
        }
        अन्यथा {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            भङ्ग;
        }
    }

donefornow:
    यदि (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ध्रुव वर्ण *) mySource;
}

स्थिर शून्य U_CALLCONV
T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    ध्रुव unsigned वर्ण *mySource = (unsigned वर्ण *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    ध्रुव unsigned वर्ण *sourceLimit = (unsigned वर्ण *) args->sourceLimit;
    ध्रुव char16_t *targetLimit = args->targetLimit;
    unsigned वर्ण *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_अंक = 0;

    /* Restore state of current sequence */
    यदि (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        गच्छ morebytes;
    }

    यावत् (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        यावत् (i < परिमाण(uint32_t))
        {
            यदि (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (वर्ण) *(mySource++);
            }
            अन्यथा
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                गच्छ donefornow;
            }
        }

        यदि (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            यदि (ch <= MAXIMUM_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = offset_अंक;
            }
            अन्यथा {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                *(myOffsets++) = offset_अंक;
                ch = U16_TRAIL(ch);
                यदि (myTarget < targetLimit)
                {
                    *(myTarget++) = (char16_t)ch;
                    *(myOffsets++) = offset_अंक;
                }
                अन्यथा
                {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    भङ्ग;
                }
            }
        }
        अन्यथा
        {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            भङ्ग;
        }
        offset_अंक += i;
    }

donefornow:
    यदि (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ध्रुव वर्ण *) mySource;
    args->offsets = myOffsets;
}

स्थिर शून्य U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    ध्रुव char16_t *mySource = args->source;
    unsigned वर्ण *myTarget;
    ध्रुव char16_t *sourceLimit = args->sourceLimit;
    ध्रुव unsigned वर्ण *targetLimit = (unsigned वर्ण *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned पूर्णाङ्क indexToWrite;
    unsigned वर्ण temp[परिमाण(uint32_t)];

    यदि(mySource >= sourceLimit) {
        /* no input, nothing to do */
        प्रत्यागम;
    }

    /* write the BOM if necessary */
    यदि(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        स्थिर ध्रुव वर्ण bom[]={ (वर्ण)0xffu, (वर्ण)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned वर्ण *) args->target;
    temp[3] = 0;

    यदि (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        गच्छ lowsurogate;
    }

    यावत् (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        यदि (U16_IS_SURROGATE(ch)) {
            यदि (U16_IS_LEAD(ch))
            {
lowsurogate:
                यदि (mySource < sourceLimit)
                {
                    ch2 = *mySource;
                    यदि (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ऊपरी_START) << HALF_SHIFT) + ch2 + SURROGATE_निचला_BASE;
                        mySource++;
                    }
                    अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        भङ्ग;
                    }
                }
                अन्यथा {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    यदि (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    भङ्ग;
                }
            }
            अन्यथा {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                भङ्ग;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[2] = (uint8_t) (ch >> 16 & 0x1F);
        temp[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        प्रति (indexToWrite = 0; indexToWrite <= परिमाण(uint32_t) - 1; indexToWrite++)
        {
            यदि (myTarget < targetLimit)
            {
                *(myTarget++) = temp[indexToWrite];
            }
            अन्यथा
            {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
    }

    यदि (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (वर्ण *) myTarget;
    args->source = mySource;
}

स्थिर शून्य U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    ध्रुव char16_t *mySource = args->source;
    unsigned वर्ण *myTarget;
    int32_t *myOffsets;
    ध्रुव char16_t *sourceLimit = args->sourceLimit;
    ध्रुव unsigned वर्ण *targetLimit = (unsigned वर्ण *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned पूर्णाङ्क indexToWrite;
    unsigned वर्ण temp[परिमाण(uint32_t)];
    int32_t offset_अंक = 0;

    यदि(mySource >= sourceLimit) {
        /* no input, nothing to do */
        प्रत्यागम;
    }

    /* write the BOM if necessary */
    यदि(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        स्थिर ध्रुव वर्ण bom[]={ (वर्ण)0xffu, (वर्ण)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned वर्ण *) args->target;
    myOffsets = args->offsets;
    temp[3] = 0;

    यदि (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        गच्छ lowsurogate;
    }

    यावत् (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        यदि (U16_IS_SURROGATE(ch)) {
            यदि (U16_IS_LEAD(ch))
            {
lowsurogate:
                यदि (mySource < sourceLimit)
                {
                    ch2 = *mySource;
                    यदि (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_ऊपरी_START) << HALF_SHIFT) + ch2 + SURROGATE_निचला_BASE;
                        mySource++;
                    }
                    अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        भङ्ग;
                    }
                }
                अन्यथा {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    यदि (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    भङ्ग;
                }
            }
            अन्यथा {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                भङ्ग;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[2] = (uint8_t) (ch >> 16 & 0x1F);
        temp[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        प्रति (indexToWrite = 0; indexToWrite <= परिमाण(uint32_t) - 1; indexToWrite++)
        {
            यदि (myTarget < targetLimit)
            {
                *(myTarget++) = temp[indexToWrite];
                *(myOffsets++) = offset_अंक;
            }
            अन्यथा
            {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        offset_अंक = offset_अंक + 1 + (temp[2] != 0);
    }

    यदि (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (वर्ण *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

स्थिर UChar32 U_CALLCONV
T_UConverter_getNextUChar_UTF32_LE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    ध्रुव uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (ध्रुव uint8_t *)args->source;
    यदि (mySource >= (ध्रुव uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0xffff;
    }

    length = (int32_t)((ध्रुव uint8_t *)args->sourceLimit - mySource);
    यदि (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (ध्रुव वर्ण *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        प्रत्यागम 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[3] << 24)
            | ((UChar32)mySource[2] << 16)
            | ((UChar32)mySource[1] << 8)
            | ((UChar32)mySource[0]);

    args->source = (ध्रुव वर्ण *)(mySource + 4);
    यदि ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        प्रत्यागम myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    प्रत्यागम 0xffff;
}
U_CDECL_END
स्थिर ध्रुव UConverterImpl _UTF32LEImpl = {
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
स्थिर ध्रुव UConverterStaticData _UTF32LEStaticData = {
    परिमाण(UConverterStaticData),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, असत्य, असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ध्रुव UConverterSharedData _UTF32LEData =
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
स्थिर शून्य U_CALLCONV
_UTF32Reset(UConverter *cnv, UConverterResetChoice choice) {
    यदि(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->mode=0;
    }
    यदि(choice!=UCNV_RESET_TO_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}

स्थिर शून्य U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverterLoadArgs *pArgs,
           UErrorCode *pErrorCode) {
    (शून्य)pArgs;
    (शून्य)pErrorCode;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

स्थिर ध्रुव वर्ण utf32BOM[8]={ 0, 0, (वर्ण)0xfeu, (वर्ण)0xffu, (वर्ण)0xffu, (वर्ण)0xfeu, 0, 0 };

स्थिर शून्य U_CALLCONV
_UTF32ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    UConverter *cnv=pArgs->converter;
    ध्रुव वर्ण *source=pArgs->source;
    ध्रुव वर्ण *sourceLimit=pArgs->sourceLimit;
    int32_t *offsets=pArgs->offsets;

    int32_t state, offsetDelta;
    वर्ण b;

    state=cnv->mode;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    offsetDelta=0;

    यावत्(source<sourceLimit && U_SUCCESS(*pErrorCode)) {
        विकल्प(state) {
        पक्ष 0:
            b=*source;
            यदि(b==0) {
                state=1; /* could be 00 00 FE FF */
            } अन्यथा यदि(b==(वर्ण)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } अन्यथा {
                state=8; /* default to UTF-32BE */
                अनुवर्त;
            }
            ++source;
            भङ्ग;
        पक्ष 1:
        पक्ष 2:
        पक्ष 3:
        पक्ष 5:
        पक्ष 6:
        पक्ष 7:
            यदि(*source==utf32BOM[state]) {
                ++state;
                ++source;
                यदि(state==4) {
                    state=8; /* detect UTF-32BE */
                    offsetDelta=(int32_t)(source-pArgs->source);
                } अन्यथा यदि(state==8) {
                    state=9; /* detect UTF-32LE */
                    offsetDelta=(int32_t)(source-pArgs->source);
                }
            } अन्यथा {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(source-pArgs->source); /* number of bytes from this buffer */

                /* reset the source */
                source=pArgs->source;

                यदि(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } अन्यथा {
                    UBool oldFlush=pArgs->flush;

                    /* some of the bytes are from a previous buffer, replay those first */
                    pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
                    pArgs->sourceLimit=pArgs->source+((state&3)-count); /* replay previous bytes */
                    pArgs->flush=असत्य; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    pArgs->sourceLimit=sourceLimit;
                    pArgs->flush=oldFlush;
                }
                state=8;
                अनुवर्त;
            }
            भङ्ग;
        पक्ष 8:
            /* call UTF-32BE */
            pArgs->source=source;
            यदि(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            } अन्यथा {
                T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            भङ्ग;
        पक्ष 9:
            /* call UTF-32LE */
            pArgs->source=source;
            यदि(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            } अन्यथा {
                T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            भङ्ग;
        स्वभाव:
            भङ्ग; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    यदि(offsets!=nullptr && offsetDelta!=0) {
        int32_t *offsetsLimit=pArgs->offsets;
        यावत्(offsets<offsetsLimit) {
            *offsets++ += offsetDelta;
        }
    }

    pArgs->source=source;

    यदि(source==sourceLimit && pArgs->flush) {
        /* handle truncated input */
        विकल्प(state) {
        पक्ष 0:
            भङ्ग; /* no input at all, nothing to do */
        पक्ष 8:
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            भङ्ग;
        पक्ष 9:
            T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            भङ्ग;
        स्वभाव:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
            pArgs->sourceLimit=pArgs->source+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            pArgs->source=source;
            pArgs->sourceLimit=sourceLimit;
            state=8;
            भङ्ग;
        }
    }

    cnv->mode=state;
}

स्थिर UChar32 U_CALLCONV
_UTF32GetNextUChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    विकल्प(pArgs->converter->mode) {
    पक्ष 8:
        प्रत्यागम T_UConverter_getNextUChar_UTF32_BE(pArgs, pErrorCode);
    पक्ष 9:
        प्रत्यागम T_UConverter_getNextUChar_UTF32_LE(pArgs, pErrorCode);
    स्वभाव:
        प्रत्यागम UCNV_GET_NEXT_UCHAR_USE_TO_U;
    }
}
U_CDECL_END
स्थिर ध्रुव UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    nullptr,
    nullptr,

    _UTF32Open,
    nullptr,
    _UTF32Reset,

    _UTF32ToUnicodeWithOffsets,
    _UTF32ToUnicodeWithOffsets,
#यदि U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC,
#अन्यथा
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
स्थिर ध्रुव UConverterStaticData _UTF32StaticData = {
    परिमाण(UConverterStaticData),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#यदि U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#अन्यथा
    { 0xfd, 0xff, 0, 0 }, 4,
#endif
    असत्य, असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ध्रुव UConverterSharedData _UTF32Data = 
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF32StaticData, &_UTF32Impl);

#endif
