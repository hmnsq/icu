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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ଅନ୍ତର୍ଭାଵ "unicode/ucnv.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_bld.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_cnv.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"

#define MAXIMUM_UCS2            0x0000FFFF
#define MAXIMUM_UTF             0x0010FFFF
#define HALF_SHIFT              10
#define HALF_BASE               0x0010000
#define HALF_MASK               0x3FF
#define SURROGATE_ଉପର_START    0xD800
#define SURROGATE_ତଳ_START     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#define SURROGATE_ତଳ_BASE      9216

ଗଣନା {
    UCNV_NEED_TO_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
T_UConverter_toUnicode_UTF32_BE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    ଅଚଳ unsigned ଅକ୍ଷର *mySource = (unsigned ଅକ୍ଷର *) args->source;
    char16_t *myTarget = args->target;
    ଅଚଳ unsigned ଅକ୍ଷର *sourceLimit = (unsigned ଅକ୍ଷର *) args->sourceLimit;
    ଅଚଳ char16_t *targetLimit = args->targetLimit;
    unsigned ଅକ୍ଷର *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    ଯଦି (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        ଯାଅ morebytes;
    }

    ଯେତେବେଳେ (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        ଯେତେବେଳେ (i < ଆକାର(uint32_t)) {
            ଯଦି (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (ଅକ୍ଷର) *(mySource++);
            }
            ଅନ୍ୟଥା {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                ଯାଅ donefornow;
            }
        }

        ଯଦି (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ଯଦି (ch <= MAXIMUM_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            ଅନ୍ୟଥା {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                ଯଦି (myTarget < targetLimit) {
                    *(myTarget++) = (char16_t)ch;
                }
                ଅନ୍ୟଥା {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    ଭାଙ୍ଗ;
                }
            }
        }
        ଅନ୍ୟଥା {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            ଭାଙ୍ଗ;
        }
    }

donefornow:
    ଯଦି (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ଅଚଳ ଅକ୍ଷର *) mySource;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    ଅଚଳ unsigned ଅକ୍ଷର *mySource = (unsigned ଅକ୍ଷର *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    ଅଚଳ unsigned ଅକ୍ଷର *sourceLimit = (unsigned ଅକ୍ଷର *) args->sourceLimit;
    ଅଚଳ char16_t *targetLimit = args->targetLimit;
    unsigned ଅକ୍ଷର *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_ଅଙ୍କ = 0;

    /* Restore state of current sequence */
    ଯଦି (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        ଯାଅ morebytes;
    }

    ଯେତେବେଳେ (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        ଯେତେବେଳେ (i < ଆକାର(uint32_t)) {
            ଯଦି (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (ଅକ୍ଷର) *(mySource++);
            }
            ଅନ୍ୟଥା {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                ଯାଅ donefornow;
            }
        }

        ଯଦି (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ଯଦି (ch <= MAXIMUM_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = offset_ଅଙ୍କ;
            }
            ଅନ୍ୟଥା {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                *myOffsets++ = offset_ଅଙ୍କ;
                ch = U16_TRAIL(ch);
                ଯଦି (myTarget < targetLimit)
                {
                    *(myTarget++) = (char16_t)ch;
                    *(myOffsets++) = offset_ଅଙ୍କ;
                }
                ଅନ୍ୟଥା {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    ଭାଙ୍ଗ;
                }
            }
        }
        ଅନ୍ୟଥା {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            ଭାଙ୍ଗ;
        }
        offset_ଅଙ୍କ += i;
    }

donefornow:
    ଯଦି (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ଅଚଳ ଅକ୍ଷର *) mySource;
    args->offsets = myOffsets;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    ଅଚଳ char16_t *mySource = args->source;
    unsigned ଅକ୍ଷର *myTarget;
    ଅଚଳ char16_t *sourceLimit = args->sourceLimit;
    ଅଚଳ unsigned ଅକ୍ଷର *targetLimit = (unsigned ଅକ୍ଷର *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା indexToWrite;
    unsigned ଅକ୍ଷର temp[ଆକାର(uint32_t)];

    ଯଦି(mySource >= sourceLimit) {
        /* no input, nothing to do */
        ଫେରନ୍ତୁ;
    }

    /* write the BOM if necessary */
    ଯଦି(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        ସ୍ଥିର ଅଚଳ ଅକ୍ଷର bom[]={ 0, 0, (ଅକ୍ଷର)0xfeu, (ଅକ୍ଷର)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned ଅକ୍ଷର *) args->target;
    temp[0] = 0;

    ଯଦି (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        ଯାଅ lowsurogate;
    }

    ଯେତେବେଳେ (mySource < sourceLimit && myTarget < targetLimit) {
        ch = *(mySource++);

        ଯଦି (U_IS_SURROGATE(ch)) {
            ଯଦି (U_IS_LEAD(ch)) {
lowsurogate:
                ଯଦି (mySource < sourceLimit) {
                    ch2 = *mySource;
                    ଯଦି (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ଉପର_START) << HALF_SHIFT) + ch2 + SURROGATE_ତଳ_BASE;
                        mySource++;
                    }
                    ଅନ୍ୟଥା {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        ଭାଙ୍ଗ;
                    }
                }
                ଅନ୍ୟଥା {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    ଯଦି (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    ଭାଙ୍ଗ;
                }
            }
            ଅନ୍ୟଥା {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                ଭାଙ୍ଗ;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[1] = (uint8_t) (ch >> 16 & 0x1F);
        temp[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        ପାଇଁ (indexToWrite = 0; indexToWrite <= ଆକାର(uint32_t) - 1; indexToWrite++) {
            ଯଦି (myTarget < targetLimit) {
                *(myTarget++) = temp[indexToWrite];
            }
            ଅନ୍ୟଥା {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
    }

    ଯଦି (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (ଅକ୍ଷର *) myTarget;
    args->source = mySource;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    ଅଚଳ char16_t *mySource = args->source;
    unsigned ଅକ୍ଷର *myTarget;
    int32_t *myOffsets;
    ଅଚଳ char16_t *sourceLimit = args->sourceLimit;
    ଅଚଳ unsigned ଅକ୍ଷର *targetLimit = (unsigned ଅକ୍ଷର *) args->targetLimit;
    UChar32 ch, ch2;
    int32_t offset_ଅଙ୍କ = 0;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା indexToWrite;
    unsigned ଅକ୍ଷର temp[ଆକାର(uint32_t)];

    ଯଦି(mySource >= sourceLimit) {
        /* no input, nothing to do */
        ଫେରନ୍ତୁ;
    }

    /* write the BOM if necessary */
    ଯଦି(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        ସ୍ଥିର ଅଚଳ ଅକ୍ଷର bom[]={ 0, 0, (ଅକ୍ଷର)0xfeu, (ଅକ୍ଷର)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned ଅକ୍ଷର *) args->target;
    myOffsets = args->offsets;
    temp[0] = 0;

    ଯଦି (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        ଯାଅ lowsurogate;
    }

    ଯେତେବେଳେ (mySource < sourceLimit && myTarget < targetLimit) {
        ch = *(mySource++);

        ଯଦି (U_IS_SURROGATE(ch)) {
            ଯଦି (U_IS_LEAD(ch)) {
lowsurogate:
                ଯଦି (mySource < sourceLimit) {
                    ch2 = *mySource;
                    ଯଦି (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ଉପର_START) << HALF_SHIFT) + ch2 + SURROGATE_ତଳ_BASE;
                        mySource++;
                    }
                    ଅନ୍ୟଥା {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        ଭାଙ୍ଗ;
                    }
                }
                ଅନ୍ୟଥା {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    ଯଦି (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    ଭାଙ୍ଗ;
                }
            }
            ଅନ୍ୟଥା {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                ଭାଙ୍ଗ;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[1] = (uint8_t) (ch >> 16 & 0x1F);
        temp[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        ପାଇଁ (indexToWrite = 0; indexToWrite <= ଆକାର(uint32_t) - 1; indexToWrite++) {
            ଯଦି (myTarget < targetLimit) {
                *(myTarget++) = temp[indexToWrite];
                *(myOffsets++) = offset_ଅଙ୍କ;
            }
            ଅନ୍ୟଥା {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        offset_ଅଙ୍କ = offset_ଅଙ୍କ + 1 + (temp[1] != 0);
    }

    ଯଦି (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (ଅକ୍ଷର *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

ସ୍ଥିର UChar32 U_CALLCONV
T_UConverter_getNextUChar_UTF32_BE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    ଅଚଳ uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (ଅଚଳ uint8_t *)args->source;
    ଯଦି (mySource >= (ଅଚଳ uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0xffff;
    }

    length = (int32_t)((ଅଚଳ uint8_t *)args->sourceLimit - mySource);
    ଯଦି (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (ଅଚଳ ଅକ୍ଷର *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        ଫେରନ୍ତୁ 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[0] << 24)
            | ((UChar32)mySource[1] << 16)
            | ((UChar32)mySource[2] << 8)
            | ((UChar32)mySource[3]);

    args->source = (ଅଚଳ ଅକ୍ଷର *)(mySource + 4);
    ଯଦି ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        ଫେରନ୍ତୁ myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    ଫେରନ୍ତୁ 0xffff;
}
U_CDECL_END
ସ୍ଥିର ଅଚଳ UConverterImpl _UTF32BEImpl = {
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
ସ୍ଥିର ଅଚଳ UConverterStaticData _UTF32BEStaticData = {
    ଆକାର(UConverterStaticData),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, ମିଥ୍ୟା, ମିଥ୍ୟା,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ଅଚଳ UConverterSharedData _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF32BEStaticData, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
T_UConverter_toUnicode_UTF32_LE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    ଅଚଳ unsigned ଅକ୍ଷର *mySource = (unsigned ଅକ୍ଷର *) args->source;
    char16_t *myTarget = args->target;
    ଅଚଳ unsigned ଅକ୍ଷର *sourceLimit = (unsigned ଅକ୍ଷର *) args->sourceLimit;
    ଅଚଳ char16_t *targetLimit = args->targetLimit;
    unsigned ଅକ୍ଷର *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    ଯଦି (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        ଯାଅ morebytes;
    }

    ଯେତେବେଳେ (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        ଯେତେବେଳେ (i < ଆକାର(uint32_t))
        {
            ଯଦି (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (ଅକ୍ଷର) *(mySource++);
            }
            ଅନ୍ୟଥା
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                ଯାଅ donefornow;
            }
        }

        ଯଦି (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ଯଦି (ch <= MAXIMUM_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            ଅନ୍ୟଥା {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                ଯଦି (myTarget < targetLimit) {
                    *(myTarget++) = (char16_t)ch;
                }
                ଅନ୍ୟଥା {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    ଭାଙ୍ଗ;
                }
            }
        }
        ଅନ୍ୟଥା {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            ଭାଙ୍ଗ;
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

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    ଅଚଳ unsigned ଅକ୍ଷର *mySource = (unsigned ଅକ୍ଷର *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    ଅଚଳ unsigned ଅକ୍ଷର *sourceLimit = (unsigned ଅକ୍ଷର *) args->sourceLimit;
    ଅଚଳ char16_t *targetLimit = args->targetLimit;
    unsigned ଅକ୍ଷର *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_ଅଙ୍କ = 0;

    /* Restore state of current sequence */
    ଯଦି (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        ଯାଅ morebytes;
    }

    ଯେତେବେଳେ (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        ଯେତେବେଳେ (i < ଆକାର(uint32_t))
        {
            ଯଦି (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (ଅକ୍ଷର) *(mySource++);
            }
            ଅନ୍ୟଥା
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                ଯାଅ donefornow;
            }
        }

        ଯଦି (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ଯଦି (ch <= MAXIMUM_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = offset_ଅଙ୍କ;
            }
            ଅନ୍ୟଥା {
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
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    ଭାଙ୍ଗ;
                }
            }
        }
        ଅନ୍ୟଥା
        {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            ଭାଙ୍ଗ;
        }
        offset_ଅଙ୍କ += i;
    }

donefornow:
    ଯଦି (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ଅଚଳ ଅକ୍ଷର *) mySource;
    args->offsets = myOffsets;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    ଅଚଳ char16_t *mySource = args->source;
    unsigned ଅକ୍ଷର *myTarget;
    ଅଚଳ char16_t *sourceLimit = args->sourceLimit;
    ଅଚଳ unsigned ଅକ୍ଷର *targetLimit = (unsigned ଅକ୍ଷର *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା indexToWrite;
    unsigned ଅକ୍ଷର temp[ଆକାର(uint32_t)];

    ଯଦି(mySource >= sourceLimit) {
        /* no input, nothing to do */
        ଫେରନ୍ତୁ;
    }

    /* write the BOM if necessary */
    ଯଦି(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        ସ୍ଥିର ଅଚଳ ଅକ୍ଷର bom[]={ (ଅକ୍ଷର)0xffu, (ଅକ୍ଷର)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned ଅକ୍ଷର *) args->target;
    temp[3] = 0;

    ଯଦି (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        ଯାଅ lowsurogate;
    }

    ଯେତେବେଳେ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        ଯଦି (U16_IS_SURROGATE(ch)) {
            ଯଦି (U16_IS_LEAD(ch))
            {
lowsurogate:
                ଯଦି (mySource < sourceLimit)
                {
                    ch2 = *mySource;
                    ଯଦି (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ଉପର_START) << HALF_SHIFT) + ch2 + SURROGATE_ତଳ_BASE;
                        mySource++;
                    }
                    ଅନ୍ୟଥା {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        ଭାଙ୍ଗ;
                    }
                }
                ଅନ୍ୟଥା {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    ଯଦି (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    ଭାଙ୍ଗ;
                }
            }
            ଅନ୍ୟଥା {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                ଭାଙ୍ଗ;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[2] = (uint8_t) (ch >> 16 & 0x1F);
        temp[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        ପାଇଁ (indexToWrite = 0; indexToWrite <= ଆକାର(uint32_t) - 1; indexToWrite++)
        {
            ଯଦି (myTarget < targetLimit)
            {
                *(myTarget++) = temp[indexToWrite];
            }
            ଅନ୍ୟଥା
            {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
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

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    ଅଚଳ char16_t *mySource = args->source;
    unsigned ଅକ୍ଷର *myTarget;
    int32_t *myOffsets;
    ଅଚଳ char16_t *sourceLimit = args->sourceLimit;
    ଅଚଳ unsigned ଅକ୍ଷର *targetLimit = (unsigned ଅକ୍ଷର *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା indexToWrite;
    unsigned ଅକ୍ଷର temp[ଆକାର(uint32_t)];
    int32_t offset_ଅଙ୍କ = 0;

    ଯଦି(mySource >= sourceLimit) {
        /* no input, nothing to do */
        ଫେରନ୍ତୁ;
    }

    /* write the BOM if necessary */
    ଯଦି(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        ସ୍ଥିର ଅଚଳ ଅକ୍ଷର bom[]={ (ଅକ୍ଷର)0xffu, (ଅକ୍ଷର)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned ଅକ୍ଷର *) args->target;
    myOffsets = args->offsets;
    temp[3] = 0;

    ଯଦି (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        ଯାଅ lowsurogate;
    }

    ଯେତେବେଳେ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        ଯଦି (U16_IS_SURROGATE(ch)) {
            ଯଦି (U16_IS_LEAD(ch))
            {
lowsurogate:
                ଯଦି (mySource < sourceLimit)
                {
                    ch2 = *mySource;
                    ଯଦି (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_ଉପର_START) << HALF_SHIFT) + ch2 + SURROGATE_ତଳ_BASE;
                        mySource++;
                    }
                    ଅନ୍ୟଥା {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        ଭାଙ୍ଗ;
                    }
                }
                ଅନ୍ୟଥା {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    ଯଦି (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    ଭାଙ୍ଗ;
                }
            }
            ଅନ୍ୟଥା {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                ଭାଙ୍ଗ;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[2] = (uint8_t) (ch >> 16 & 0x1F);
        temp[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        ପାଇଁ (indexToWrite = 0; indexToWrite <= ଆକାର(uint32_t) - 1; indexToWrite++)
        {
            ଯଦି (myTarget < targetLimit)
            {
                *(myTarget++) = temp[indexToWrite];
                *(myOffsets++) = offset_ଅଙ୍କ;
            }
            ଅନ୍ୟଥା
            {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        offset_ଅଙ୍କ = offset_ଅଙ୍କ + 1 + (temp[2] != 0);
    }

    ଯଦି (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (ଅକ୍ଷର *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

ସ୍ଥିର UChar32 U_CALLCONV
T_UConverter_getNextUChar_UTF32_LE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    ଅଚଳ uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (ଅଚଳ uint8_t *)args->source;
    ଯଦି (mySource >= (ଅଚଳ uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0xffff;
    }

    length = (int32_t)((ଅଚଳ uint8_t *)args->sourceLimit - mySource);
    ଯଦି (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (ଅଚଳ ଅକ୍ଷର *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        ଫେରନ୍ତୁ 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[3] << 24)
            | ((UChar32)mySource[2] << 16)
            | ((UChar32)mySource[1] << 8)
            | ((UChar32)mySource[0]);

    args->source = (ଅଚଳ ଅକ୍ଷର *)(mySource + 4);
    ଯଦି ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        ଫେରନ୍ତୁ myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    ଫେରନ୍ତୁ 0xffff;
}
U_CDECL_END
ସ୍ଥିର ଅଚଳ UConverterImpl _UTF32LEImpl = {
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
ସ୍ଥିର ଅଚଳ UConverterStaticData _UTF32LEStaticData = {
    ଆକାର(UConverterStaticData),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, ମିଥ୍ୟା, ମିଥ୍ୟା,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ଅଚଳ UConverterSharedData _UTF32LEData =
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
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
_UTF32Reset(UConverter *cnv, UConverterResetChoice choice) {
    ଯଦି(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->mode=0;
    }
    ଯଦି(choice!=UCNV_RESET_TO_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverterLoadArgs *pArgs,
           UErrorCode *pErrorCode) {
    (ଶୂନ୍ୟ)pArgs;
    (ଶୂନ୍ୟ)pErrorCode;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର utf32BOM[8]={ 0, 0, (ଅକ୍ଷର)0xfeu, (ଅକ୍ଷର)0xffu, (ଅକ୍ଷର)0xffu, (ଅକ୍ଷର)0xfeu, 0, 0 };

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
_UTF32ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    UConverter *cnv=pArgs->converter;
    ଅଚଳ ଅକ୍ଷର *source=pArgs->source;
    ଅଚଳ ଅକ୍ଷର *sourceLimit=pArgs->sourceLimit;
    int32_t *offsets=pArgs->offsets;

    int32_t state, offsetDelta;
    ଅକ୍ଷର b;

    state=cnv->mode;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    offsetDelta=0;

    ଯେତେବେଳେ(source<sourceLimit && U_SUCCESS(*pErrorCode)) {
        ବାଛ(state) {
        ସ୍ଥିତି 0:
            b=*source;
            ଯଦି(b==0) {
                state=1; /* could be 00 00 FE FF */
            } ଅନ୍ୟଥା ଯଦି(b==(ଅକ୍ଷର)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } ଅନ୍ୟଥା {
                state=8; /* default to UTF-32BE */
                ଜାରି;
            }
            ++source;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି 1:
        ସ୍ଥିତି 2:
        ସ୍ଥିତି 3:
        ସ୍ଥିତି 5:
        ସ୍ଥିତି 6:
        ସ୍ଥିତି 7:
            ଯଦି(*source==utf32BOM[state]) {
                ++state;
                ++source;
                ଯଦି(state==4) {
                    state=8; /* detect UTF-32BE */
                    offsetDelta=(int32_t)(source-pArgs->source);
                } ଅନ୍ୟଥା ଯଦି(state==8) {
                    state=9; /* detect UTF-32LE */
                    offsetDelta=(int32_t)(source-pArgs->source);
                }
            } ଅନ୍ୟଥା {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(source-pArgs->source); /* number of bytes from this buffer */

                /* reset the source */
                source=pArgs->source;

                ଯଦି(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } ଅନ୍ୟଥା {
                    UBool oldFlush=pArgs->flush;

                    /* some of the bytes are from a previous buffer, replay those first */
                    pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
                    pArgs->sourceLimit=pArgs->source+((state&3)-count); /* replay previous bytes */
                    pArgs->flush=ମିଥ୍ୟା; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    pArgs->sourceLimit=sourceLimit;
                    pArgs->flush=oldFlush;
                }
                state=8;
                ଜାରି;
            }
            ଭାଙ୍ଗ;
        ସ୍ଥିତି 8:
            /* call UTF-32BE */
            pArgs->source=source;
            ଯଦି(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            } ଅନ୍ୟଥା {
                T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି 9:
            /* call UTF-32LE */
            pArgs->source=source;
            ଯଦି(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            } ଅନ୍ୟଥା {
                T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଭାଙ୍ଗ; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    ଯଦି(offsets!=nullptr && offsetDelta!=0) {
        int32_t *offsetsLimit=pArgs->offsets;
        ଯେତେବେଳେ(offsets<offsetsLimit) {
            *offsets++ += offsetDelta;
        }
    }

    pArgs->source=source;

    ଯଦି(source==sourceLimit && pArgs->flush) {
        /* handle truncated input */
        ବାଛ(state) {
        ସ୍ଥିତି 0:
            ଭାଙ୍ଗ; /* no input at all, nothing to do */
        ସ୍ଥିତି 8:
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି 9:
            T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
            pArgs->sourceLimit=pArgs->source+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            pArgs->source=source;
            pArgs->sourceLimit=sourceLimit;
            state=8;
            ଭାଙ୍ଗ;
        }
    }

    cnv->mode=state;
}

ସ୍ଥିର UChar32 U_CALLCONV
_UTF32GetNextUChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    ବାଛ(pArgs->converter->mode) {
    ସ୍ଥିତି 8:
        ଫେରନ୍ତୁ T_UConverter_getNextUChar_UTF32_BE(pArgs, pErrorCode);
    ସ୍ଥିତି 9:
        ଫେରନ୍ତୁ T_UConverter_getNextUChar_UTF32_LE(pArgs, pErrorCode);
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ UCNV_GET_NEXT_UCHAR_USE_TO_U;
    }
}
U_CDECL_END
ସ୍ଥିର ଅଚଳ UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    nullptr,
    nullptr,

    _UTF32Open,
    nullptr,
    _UTF32Reset,

    _UTF32ToUnicodeWithOffsets,
    _UTF32ToUnicodeWithOffsets,
#ଯଦି U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC,
#ଅନ୍ୟଥା
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
ସ୍ଥିର ଅଚଳ UConverterStaticData _UTF32StaticData = {
    ଆକାର(UConverterStaticData),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#ଯଦି U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#ଅନ୍ୟଥା
    { 0xfd, 0xff, 0, 0 }, 4,
#endif
    ମିଥ୍ୟା, ମିଥ୍ୟା,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ଅଚଳ UConverterSharedData _UTF32Data = 
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF32StaticData, &_UTF32Impl);

#endif
