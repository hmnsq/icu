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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#അന്തര്ഭാവ "unicode/ucnv.h"
#അന്തര്ഭാവ "unicode/utf.h"
#അന്തര്ഭാവ "ucnv_bld.h"
#അന്തര്ഭാവ "ucnv_cnv.h"
#അന്തര്ഭാവ "cmemory.h"

#define MAXIMUM_UCS2            0x0000FFFF
#define MAXIMUM_UTF             0x0010FFFF
#define HALF_SHIFT              10
#define HALF_BASE               0x0010000
#define HALF_MASK               0x3FF
#define SURROGATE_മുകളിലെ_START    0xD800
#define SURROGATE_താഴെയുള്ള_START     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#define SURROGATE_താഴെയുള്ള_BASE      9216

എണ്ണൽ {
    UCNV_NEED_TO_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
സ്ഥിതം ശൂന്യം U_CALLCONV
T_UConverter_toUnicode_UTF32_BE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    സ്ഥിരം unsigned അക്ഷരം *mySource = (unsigned അക്ഷരം *) args->source;
    char16_t *myTarget = args->target;
    സ്ഥിരം unsigned അക്ഷരം *sourceLimit = (unsigned അക്ഷരം *) args->sourceLimit;
    സ്ഥിരം char16_t *targetLimit = args->targetLimit;
    unsigned അക്ഷരം *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    എങ്കിൽ (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        പോകുക morebytes;
    }

    ആകുമ്പോൾ (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        ആകുമ്പോൾ (i < വലുപ്പം(uint32_t)) {
            എങ്കിൽ (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (അക്ഷരം) *(mySource++);
            }
            അല്ലെങ്കിൽ {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                പോകുക donefornow;
            }
        }

        എങ്കിൽ (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            എങ്കിൽ (ch <= MAXIMUM_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            അല്ലെങ്കിൽ {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                എങ്കിൽ (myTarget < targetLimit) {
                    *(myTarget++) = (char16_t)ch;
                }
                അല്ലെങ്കിൽ {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    നിർത്തുക;
                }
            }
        }
        അല്ലെങ്കിൽ {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            നിർത്തുക;
        }
    }

donefornow:
    എങ്കിൽ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (സ്ഥിരം അക്ഷരം *) mySource;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    സ്ഥിരം unsigned അക്ഷരം *mySource = (unsigned അക്ഷരം *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    സ്ഥിരം unsigned അക്ഷരം *sourceLimit = (unsigned അക്ഷരം *) args->sourceLimit;
    സ്ഥിരം char16_t *targetLimit = args->targetLimit;
    unsigned അക്ഷരം *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_അക്കം = 0;

    /* Restore state of current sequence */
    എങ്കിൽ (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        പോകുക morebytes;
    }

    ആകുമ്പോൾ (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        ആകുമ്പോൾ (i < വലുപ്പം(uint32_t)) {
            എങ്കിൽ (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (അക്ഷരം) *(mySource++);
            }
            അല്ലെങ്കിൽ {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                പോകുക donefornow;
            }
        }

        എങ്കിൽ (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            എങ്കിൽ (ch <= MAXIMUM_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = offset_അക്കം;
            }
            അല്ലെങ്കിൽ {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                *myOffsets++ = offset_അക്കം;
                ch = U16_TRAIL(ch);
                എങ്കിൽ (myTarget < targetLimit)
                {
                    *(myTarget++) = (char16_t)ch;
                    *(myOffsets++) = offset_അക്കം;
                }
                അല്ലെങ്കിൽ {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    നിർത്തുക;
                }
            }
        }
        അല്ലെങ്കിൽ {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            നിർത്തുക;
        }
        offset_അക്കം += i;
    }

donefornow:
    എങ്കിൽ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (സ്ഥിരം അക്ഷരം *) mySource;
    args->offsets = myOffsets;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    സ്ഥിരം char16_t *mySource = args->source;
    unsigned അക്ഷരം *myTarget;
    സ്ഥിരം char16_t *sourceLimit = args->sourceLimit;
    സ്ഥിരം unsigned അക്ഷരം *targetLimit = (unsigned അക്ഷരം *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned പൂർണ്ണസംഖ്യ indexToWrite;
    unsigned അക്ഷരം temp[വലുപ്പം(uint32_t)];

    എങ്കിൽ(mySource >= sourceLimit) {
        /* no input, nothing to do */
        തിരികെ;
    }

    /* write the BOM if necessary */
    എങ്കിൽ(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        സ്ഥിതം സ്ഥിരം അക്ഷരം bom[]={ 0, 0, (അക്ഷരം)0xfeu, (അക്ഷരം)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned അക്ഷരം *) args->target;
    temp[0] = 0;

    എങ്കിൽ (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        പോകുക lowsurogate;
    }

    ആകുമ്പോൾ (mySource < sourceLimit && myTarget < targetLimit) {
        ch = *(mySource++);

        എങ്കിൽ (U_IS_SURROGATE(ch)) {
            എങ്കിൽ (U_IS_LEAD(ch)) {
lowsurogate:
                എങ്കിൽ (mySource < sourceLimit) {
                    ch2 = *mySource;
                    എങ്കിൽ (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_മുകളിലെ_START) << HALF_SHIFT) + ch2 + SURROGATE_താഴെയുള്ള_BASE;
                        mySource++;
                    }
                    അല്ലെങ്കിൽ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        നിർത്തുക;
                    }
                }
                അല്ലെങ്കിൽ {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    എങ്കിൽ (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    നിർത്തുക;
                }
            }
            അല്ലെങ്കിൽ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                നിർത്തുക;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[1] = (uint8_t) (ch >> 16 & 0x1F);
        temp[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        വേണ്ടി (indexToWrite = 0; indexToWrite <= വലുപ്പം(uint32_t) - 1; indexToWrite++) {
            എങ്കിൽ (myTarget < targetLimit) {
                *(myTarget++) = temp[indexToWrite];
            }
            അല്ലെങ്കിൽ {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
    }

    എങ്കിൽ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (അക്ഷരം *) myTarget;
    args->source = mySource;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    സ്ഥിരം char16_t *mySource = args->source;
    unsigned അക്ഷരം *myTarget;
    int32_t *myOffsets;
    സ്ഥിരം char16_t *sourceLimit = args->sourceLimit;
    സ്ഥിരം unsigned അക്ഷരം *targetLimit = (unsigned അക്ഷരം *) args->targetLimit;
    UChar32 ch, ch2;
    int32_t offset_അക്കം = 0;
    unsigned പൂർണ്ണസംഖ്യ indexToWrite;
    unsigned അക്ഷരം temp[വലുപ്പം(uint32_t)];

    എങ്കിൽ(mySource >= sourceLimit) {
        /* no input, nothing to do */
        തിരികെ;
    }

    /* write the BOM if necessary */
    എങ്കിൽ(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        സ്ഥിതം സ്ഥിരം അക്ഷരം bom[]={ 0, 0, (അക്ഷരം)0xfeu, (അക്ഷരം)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned അക്ഷരം *) args->target;
    myOffsets = args->offsets;
    temp[0] = 0;

    എങ്കിൽ (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        പോകുക lowsurogate;
    }

    ആകുമ്പോൾ (mySource < sourceLimit && myTarget < targetLimit) {
        ch = *(mySource++);

        എങ്കിൽ (U_IS_SURROGATE(ch)) {
            എങ്കിൽ (U_IS_LEAD(ch)) {
lowsurogate:
                എങ്കിൽ (mySource < sourceLimit) {
                    ch2 = *mySource;
                    എങ്കിൽ (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_മുകളിലെ_START) << HALF_SHIFT) + ch2 + SURROGATE_താഴെയുള്ള_BASE;
                        mySource++;
                    }
                    അല്ലെങ്കിൽ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        നിർത്തുക;
                    }
                }
                അല്ലെങ്കിൽ {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    എങ്കിൽ (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    നിർത്തുക;
                }
            }
            അല്ലെങ്കിൽ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                നിർത്തുക;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[1] = (uint8_t) (ch >> 16 & 0x1F);
        temp[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        വേണ്ടി (indexToWrite = 0; indexToWrite <= വലുപ്പം(uint32_t) - 1; indexToWrite++) {
            എങ്കിൽ (myTarget < targetLimit) {
                *(myTarget++) = temp[indexToWrite];
                *(myOffsets++) = offset_അക്കം;
            }
            അല്ലെങ്കിൽ {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        offset_അക്കം = offset_അക്കം + 1 + (temp[1] != 0);
    }

    എങ്കിൽ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (അക്ഷരം *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

സ്ഥിതം UChar32 U_CALLCONV
T_UConverter_getNextUChar_UTF32_BE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    സ്ഥിരം uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (സ്ഥിരം uint8_t *)args->source;
    എങ്കിൽ (mySource >= (സ്ഥിരം uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ 0xffff;
    }

    length = (int32_t)((സ്ഥിരം uint8_t *)args->sourceLimit - mySource);
    എങ്കിൽ (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (സ്ഥിരം അക്ഷരം *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        തിരികെ 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[0] << 24)
            | ((UChar32)mySource[1] << 16)
            | ((UChar32)mySource[2] << 8)
            | ((UChar32)mySource[3]);

    args->source = (സ്ഥിരം അക്ഷരം *)(mySource + 4);
    എങ്കിൽ ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        തിരികെ myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    തിരികെ 0xffff;
}
U_CDECL_END
സ്ഥിതം സ്ഥിരം UConverterImpl _UTF32BEImpl = {
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
സ്ഥിതം സ്ഥിരം UConverterStaticData _UTF32BEStaticData = {
    വലുപ്പം(UConverterStaticData),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, അസത്യം, അസത്യം,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

സ്ഥിരം UConverterSharedData _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF32BEStaticData, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
സ്ഥിതം ശൂന്യം U_CALLCONV
T_UConverter_toUnicode_UTF32_LE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    സ്ഥിരം unsigned അക്ഷരം *mySource = (unsigned അക്ഷരം *) args->source;
    char16_t *myTarget = args->target;
    സ്ഥിരം unsigned അക്ഷരം *sourceLimit = (unsigned അക്ഷരം *) args->sourceLimit;
    സ്ഥിരം char16_t *targetLimit = args->targetLimit;
    unsigned അക്ഷരം *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    എങ്കിൽ (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        പോകുക morebytes;
    }

    ആകുമ്പോൾ (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        ആകുമ്പോൾ (i < വലുപ്പം(uint32_t))
        {
            എങ്കിൽ (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (അക്ഷരം) *(mySource++);
            }
            അല്ലെങ്കിൽ
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                പോകുക donefornow;
            }
        }

        എങ്കിൽ (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            എങ്കിൽ (ch <= MAXIMUM_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            അല്ലെങ്കിൽ {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                എങ്കിൽ (myTarget < targetLimit) {
                    *(myTarget++) = (char16_t)ch;
                }
                അല്ലെങ്കിൽ {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    നിർത്തുക;
                }
            }
        }
        അല്ലെങ്കിൽ {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            നിർത്തുക;
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

സ്ഥിതം ശൂന്യം U_CALLCONV
T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    സ്ഥിരം unsigned അക്ഷരം *mySource = (unsigned അക്ഷരം *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    സ്ഥിരം unsigned അക്ഷരം *sourceLimit = (unsigned അക്ഷരം *) args->sourceLimit;
    സ്ഥിരം char16_t *targetLimit = args->targetLimit;
    unsigned അക്ഷരം *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_അക്കം = 0;

    /* Restore state of current sequence */
    എങ്കിൽ (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        പോകുക morebytes;
    }

    ആകുമ്പോൾ (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        ആകുമ്പോൾ (i < വലുപ്പം(uint32_t))
        {
            എങ്കിൽ (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (അക്ഷരം) *(mySource++);
            }
            അല്ലെങ്കിൽ
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                പോകുക donefornow;
            }
        }

        എങ്കിൽ (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            എങ്കിൽ (ch <= MAXIMUM_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = offset_അക്കം;
            }
            അല്ലെങ്കിൽ {
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
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UCharErrorBuffer[0] = (char16_t) ch;
                    args->converter->UCharErrorBufferLength = 1;
                    *err = U_BUFFER_OVERFLOW_ERROR;
                    നിർത്തുക;
                }
            }
        }
        അല്ലെങ്കിൽ
        {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            നിർത്തുക;
        }
        offset_അക്കം += i;
    }

donefornow:
    എങ്കിൽ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (സ്ഥിരം അക്ഷരം *) mySource;
    args->offsets = myOffsets;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    സ്ഥിരം char16_t *mySource = args->source;
    unsigned അക്ഷരം *myTarget;
    സ്ഥിരം char16_t *sourceLimit = args->sourceLimit;
    സ്ഥിരം unsigned അക്ഷരം *targetLimit = (unsigned അക്ഷരം *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned പൂർണ്ണസംഖ്യ indexToWrite;
    unsigned അക്ഷരം temp[വലുപ്പം(uint32_t)];

    എങ്കിൽ(mySource >= sourceLimit) {
        /* no input, nothing to do */
        തിരികെ;
    }

    /* write the BOM if necessary */
    എങ്കിൽ(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        സ്ഥിതം സ്ഥിരം അക്ഷരം bom[]={ (അക്ഷരം)0xffu, (അക്ഷരം)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned അക്ഷരം *) args->target;
    temp[3] = 0;

    എങ്കിൽ (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        പോകുക lowsurogate;
    }

    ആകുമ്പോൾ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        എങ്കിൽ (U16_IS_SURROGATE(ch)) {
            എങ്കിൽ (U16_IS_LEAD(ch))
            {
lowsurogate:
                എങ്കിൽ (mySource < sourceLimit)
                {
                    ch2 = *mySource;
                    എങ്കിൽ (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_മുകളിലെ_START) << HALF_SHIFT) + ch2 + SURROGATE_താഴെയുള്ള_BASE;
                        mySource++;
                    }
                    അല്ലെങ്കിൽ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        നിർത്തുക;
                    }
                }
                അല്ലെങ്കിൽ {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    എങ്കിൽ (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    നിർത്തുക;
                }
            }
            അല്ലെങ്കിൽ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                നിർത്തുക;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[2] = (uint8_t) (ch >> 16 & 0x1F);
        temp[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        വേണ്ടി (indexToWrite = 0; indexToWrite <= വലുപ്പം(uint32_t) - 1; indexToWrite++)
        {
            എങ്കിൽ (myTarget < targetLimit)
            {
                *(myTarget++) = temp[indexToWrite];
            }
            അല്ലെങ്കിൽ
            {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
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

സ്ഥിതം ശൂന്യം U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    സ്ഥിരം char16_t *mySource = args->source;
    unsigned അക്ഷരം *myTarget;
    int32_t *myOffsets;
    സ്ഥിരം char16_t *sourceLimit = args->sourceLimit;
    സ്ഥിരം unsigned അക്ഷരം *targetLimit = (unsigned അക്ഷരം *) args->targetLimit;
    UChar32 ch, ch2;
    unsigned പൂർണ്ണസംഖ്യ indexToWrite;
    unsigned അക്ഷരം temp[വലുപ്പം(uint32_t)];
    int32_t offset_അക്കം = 0;

    എങ്കിൽ(mySource >= sourceLimit) {
        /* no input, nothing to do */
        തിരികെ;
    }

    /* write the BOM if necessary */
    എങ്കിൽ(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        സ്ഥിതം സ്ഥിരം അക്ഷരം bom[]={ (അക്ഷരം)0xffu, (അക്ഷരം)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (unsigned അക്ഷരം *) args->target;
    myOffsets = args->offsets;
    temp[3] = 0;

    എങ്കിൽ (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        പോകുക lowsurogate;
    }

    ആകുമ്പോൾ (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        എങ്കിൽ (U16_IS_SURROGATE(ch)) {
            എങ്കിൽ (U16_IS_LEAD(ch))
            {
lowsurogate:
                എങ്കിൽ (mySource < sourceLimit)
                {
                    ch2 = *mySource;
                    എങ്കിൽ (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_മുകളിലെ_START) << HALF_SHIFT) + ch2 + SURROGATE_താഴെയുള്ള_BASE;
                        mySource++;
                    }
                    അല്ലെങ്കിൽ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        നിർത്തുക;
                    }
                }
                അല്ലെങ്കിൽ {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    എങ്കിൽ (args->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    നിർത്തുക;
                }
            }
            അല്ലെങ്കിൽ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                നിർത്തുക;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        temp[2] = (uint8_t) (ch >> 16 & 0x1F);
        temp[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        temp[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        വേണ്ടി (indexToWrite = 0; indexToWrite <= വലുപ്പം(uint32_t) - 1; indexToWrite++)
        {
            എങ്കിൽ (myTarget < targetLimit)
            {
                *(myTarget++) = temp[indexToWrite];
                *(myOffsets++) = offset_അക്കം;
            }
            അല്ലെങ്കിൽ
            {
                args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = temp[indexToWrite];
                *err = U_BUFFER_OVERFLOW_ERROR;
            }
        }
        offset_അക്കം = offset_അക്കം + 1 + (temp[2] != 0);
    }

    എങ്കിൽ (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_BUFFER_OVERFLOW_ERROR;
    }

    args->target = (അക്ഷരം *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

സ്ഥിതം UChar32 U_CALLCONV
T_UConverter_getNextUChar_UTF32_LE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    സ്ഥിരം uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (സ്ഥിരം uint8_t *)args->source;
    എങ്കിൽ (mySource >= (സ്ഥിരം uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ 0xffff;
    }

    length = (int32_t)((സ്ഥിരം uint8_t *)args->sourceLimit - mySource);
    എങ്കിൽ (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (സ്ഥിരം അക്ഷരം *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        തിരികെ 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[3] << 24)
            | ((UChar32)mySource[2] << 16)
            | ((UChar32)mySource[1] << 8)
            | ((UChar32)mySource[0]);

    args->source = (സ്ഥിരം അക്ഷരം *)(mySource + 4);
    എങ്കിൽ ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        തിരികെ myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    തിരികെ 0xffff;
}
U_CDECL_END
സ്ഥിതം സ്ഥിരം UConverterImpl _UTF32LEImpl = {
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
സ്ഥിതം സ്ഥിരം UConverterStaticData _UTF32LEStaticData = {
    വലുപ്പം(UConverterStaticData),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, അസത്യം, അസത്യം,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


സ്ഥിരം UConverterSharedData _UTF32LEData =
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
സ്ഥിതം ശൂന്യം U_CALLCONV
_UTF32Reset(UConverter *cnv, UConverterResetChoice choice) {
    എങ്കിൽ(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->mode=0;
    }
    എങ്കിൽ(choice!=UCNV_RESET_TO_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}

സ്ഥിതം ശൂന്യം U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverterLoadArgs *pArgs,
           UErrorCode *pErrorCode) {
    (ശൂന്യം)pArgs;
    (ശൂന്യം)pErrorCode;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

സ്ഥിതം സ്ഥിരം അക്ഷരം utf32BOM[8]={ 0, 0, (അക്ഷരം)0xfeu, (അക്ഷരം)0xffu, (അക്ഷരം)0xffu, (അക്ഷരം)0xfeu, 0, 0 };

സ്ഥിതം ശൂന്യം U_CALLCONV
_UTF32ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    UConverter *cnv=pArgs->converter;
    സ്ഥിരം അക്ഷരം *source=pArgs->source;
    സ്ഥിരം അക്ഷരം *sourceLimit=pArgs->sourceLimit;
    int32_t *offsets=pArgs->offsets;

    int32_t state, offsetDelta;
    അക്ഷരം b;

    state=cnv->mode;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    offsetDelta=0;

    ആകുമ്പോൾ(source<sourceLimit && U_SUCCESS(*pErrorCode)) {
        തിരഞ്ഞെടുപ്പ്(state) {
        അവസ്ഥ 0:
            b=*source;
            എങ്കിൽ(b==0) {
                state=1; /* could be 00 00 FE FF */
            } അല്ലെങ്കിൽ എങ്കിൽ(b==(അക്ഷരം)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } അല്ലെങ്കിൽ {
                state=8; /* default to UTF-32BE */
                തുടരുക;
            }
            ++source;
            നിർത്തുക;
        അവസ്ഥ 1:
        അവസ്ഥ 2:
        അവസ്ഥ 3:
        അവസ്ഥ 5:
        അവസ്ഥ 6:
        അവസ്ഥ 7:
            എങ്കിൽ(*source==utf32BOM[state]) {
                ++state;
                ++source;
                എങ്കിൽ(state==4) {
                    state=8; /* detect UTF-32BE */
                    offsetDelta=(int32_t)(source-pArgs->source);
                } അല്ലെങ്കിൽ എങ്കിൽ(state==8) {
                    state=9; /* detect UTF-32LE */
                    offsetDelta=(int32_t)(source-pArgs->source);
                }
            } അല്ലെങ്കിൽ {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(source-pArgs->source); /* number of bytes from this buffer */

                /* reset the source */
                source=pArgs->source;

                എങ്കിൽ(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } അല്ലെങ്കിൽ {
                    UBool oldFlush=pArgs->flush;

                    /* some of the bytes are from a previous buffer, replay those first */
                    pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
                    pArgs->sourceLimit=pArgs->source+((state&3)-count); /* replay previous bytes */
                    pArgs->flush=അസത്യം; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    pArgs->sourceLimit=sourceLimit;
                    pArgs->flush=oldFlush;
                }
                state=8;
                തുടരുക;
            }
            നിർത്തുക;
        അവസ്ഥ 8:
            /* call UTF-32BE */
            pArgs->source=source;
            എങ്കിൽ(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            } അല്ലെങ്കിൽ {
                T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            നിർത്തുക;
        അവസ്ഥ 9:
            /* call UTF-32LE */
            pArgs->source=source;
            എങ്കിൽ(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            } അല്ലെങ്കിൽ {
                T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            നിർത്തുക; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    എങ്കിൽ(offsets!=nullptr && offsetDelta!=0) {
        int32_t *offsetsLimit=pArgs->offsets;
        ആകുമ്പോൾ(offsets<offsetsLimit) {
            *offsets++ += offsetDelta;
        }
    }

    pArgs->source=source;

    എങ്കിൽ(source==sourceLimit && pArgs->flush) {
        /* handle truncated input */
        തിരഞ്ഞെടുപ്പ്(state) {
        അവസ്ഥ 0:
            നിർത്തുക; /* no input at all, nothing to do */
        അവസ്ഥ 8:
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            നിർത്തുക;
        അവസ്ഥ 9:
            T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
            pArgs->sourceLimit=pArgs->source+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            pArgs->source=source;
            pArgs->sourceLimit=sourceLimit;
            state=8;
            നിർത്തുക;
        }
    }

    cnv->mode=state;
}

സ്ഥിതം UChar32 U_CALLCONV
_UTF32GetNextUChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    തിരഞ്ഞെടുപ്പ്(pArgs->converter->mode) {
    അവസ്ഥ 8:
        തിരികെ T_UConverter_getNextUChar_UTF32_BE(pArgs, pErrorCode);
    അവസ്ഥ 9:
        തിരികെ T_UConverter_getNextUChar_UTF32_LE(pArgs, pErrorCode);
    സ്ഥിരസ്ഥിതി:
        തിരികെ UCNV_GET_NEXT_UCHAR_USE_TO_U;
    }
}
U_CDECL_END
സ്ഥിതം സ്ഥിരം UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    nullptr,
    nullptr,

    _UTF32Open,
    nullptr,
    _UTF32Reset,

    _UTF32ToUnicodeWithOffsets,
    _UTF32ToUnicodeWithOffsets,
#എങ്കിൽ U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC,
#അല്ലെങ്കിൽ
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
സ്ഥിതം സ്ഥിരം UConverterStaticData _UTF32StaticData = {
    വലുപ്പം(UConverterStaticData),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#എങ്കിൽ U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#അല്ലെങ്കിൽ
    { 0xfd, 0xff, 0, 0 }, 4,
#endif
    അസത്യം, അസത്യം,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

സ്ഥിരം UConverterSharedData _UTF32Data = 
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF32StaticData, &_UTF32Impl);

#endif
