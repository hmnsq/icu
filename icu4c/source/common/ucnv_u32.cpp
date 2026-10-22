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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#समावेश "unicode/ucnv.h"
#समावेश "unicode/utf.h"
#समावेश "ucnv_bld.h"
#समावेश "ucnv_cnv.h"
#समावेश "cmemory.h"

#घोषणा MAXIMUM_UCS2            0x0000FFFF
#घोषणा MAXIMUM_UTF             0x0010FFFF
#घोषणा HALF_SHIFT              10
#घोषणा HALF_BASE               0x0010000
#घोषणा HALF_MASK               0x3FF
#घोषणा SURROGATE_ऊपरी_START    0xD800
#घोषणा SURROGATE_निचला_START     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#घोषणा SURROGATE_निचला_BASE      9216

क्रमागत {
    UCNV_NEED_TO_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
अटल व्योम U_CALLCONV
T_UConverter_toUnicode_UTF32_BE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    स्थिर अचिन्हित अक्षर *mySource = (अचिन्हित अक्षर *) args->source;
    char16_t *myTarget = args->target;
    स्थिर अचिन्हित अक्षर *sourceLimit = (अचिन्हित अक्षर *) args->sourceLimit;
    स्थिर char16_t *targetLimit = args->targetLimit;
    अचिन्हित अक्षर *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    अगर (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        जाओ morebytes;
    }

    जबतक (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        जबतक (i < माप(uint32_t)) {
            अगर (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (अक्षर) *(mySource++);
            }
            अन्यथा {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                जाओ donefornow;
            }
        }

        अगर (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            अगर (ch <= MAXIMUM_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            अन्यथा {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                अगर (myTarget < targetLimit) {
                    *(myTarget++) = (char16_t)ch;
                }
                अन्यथा {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UChar_Error_बफर[0] = (char16_t) ch;
                    args->converter->UChar_Error_बफर_लंबाई = 1;
                    *err = U_बफर_OVERFLOW_ERROR;
                    अवरोध;
                }
            }
        }
        अन्यथा {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            अवरोध;
        }
    }

donefornow:
    अगर (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_बफर_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (स्थिर अक्षर *) mySource;
}

अटल व्योम U_CALLCONV
T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    स्थिर अचिन्हित अक्षर *mySource = (अचिन्हित अक्षर *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    स्थिर अचिन्हित अक्षर *sourceLimit = (अचिन्हित अक्षर *) args->sourceLimit;
    स्थिर char16_t *targetLimit = args->targetLimit;
    अचिन्हित अक्षर *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_अंक = 0;

    /* Restore state of current sequence */
    अगर (args->converter->toULength > 0 && myTarget < targetLimit) {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        ch = args->converter->toUnicodeStatus - 1;/*Stores the previously calculated ch from a previous call*/
        args->converter->toUnicodeStatus = 0;
        जाओ morebytes;
    }

    जबतक (mySource < sourceLimit && myTarget < targetLimit) {
        i = 0;
        ch = 0;
morebytes:
        जबतक (i < माप(uint32_t)) {
            अगर (mySource < sourceLimit) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                toUBytes[i++] = (अक्षर) *(mySource++);
            }
            अन्यथा {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                जाओ donefornow;
            }
        }

        अगर (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            अगर (ch <= MAXIMUM_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = offset_अंक;
            }
            अन्यथा {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                *myOffsets++ = offset_अंक;
                ch = U16_TRAIL(ch);
                अगर (myTarget < targetLimit)
                {
                    *(myTarget++) = (char16_t)ch;
                    *(myOffsets++) = offset_अंक;
                }
                अन्यथा {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UChar_Error_बफर[0] = (char16_t) ch;
                    args->converter->UChar_Error_बफर_लंबाई = 1;
                    *err = U_बफर_OVERFLOW_ERROR;
                    अवरोध;
                }
            }
        }
        अन्यथा {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            अवरोध;
        }
        offset_अंक += i;
    }

donefornow:
    अगर (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_बफर_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (स्थिर अक्षर *) mySource;
    args->offsets = myOffsets;
}

अटल व्योम U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    स्थिर char16_t *mySource = args->source;
    अचिन्हित अक्षर *myTarget;
    स्थिर char16_t *sourceLimit = args->sourceLimit;
    स्थिर अचिन्हित अक्षर *targetLimit = (अचिन्हित अक्षर *) args->targetLimit;
    UChar32 ch, ch2;
    अचिन्हित पूर्णांक सूचकांक_To_Write;
    अचिन्हित अक्षर अस्थायी[माप(uint32_t)];

    अगर(mySource >= sourceLimit) {
        /* no input, nothing to do */
        वापस;
    }

    /* write the BOM if necessary */
    अगर(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        अटल स्थिर अक्षर bom[]={ 0, 0, (अक्षर)0xfeu, (अक्षर)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (अचिन्हित अक्षर *) args->target;
    अस्थायी[0] = 0;

    अगर (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        जाओ lowsurogate;
    }

    जबतक (mySource < sourceLimit && myTarget < targetLimit) {
        ch = *(mySource++);

        अगर (U_IS_SURROGATE(ch)) {
            अगर (U_IS_LEAD(ch)) {
lowsurogate:
                अगर (mySource < sourceLimit) {
                    ch2 = *mySource;
                    अगर (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ऊपरी_START) << HALF_SHIFT) + ch2 + SURROGATE_निचला_BASE;
                        mySource++;
                    }
                    अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        अवरोध;
                    }
                }
                अन्यथा {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    अगर (args->सफाई) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    अवरोध;
                }
            }
            अन्यथा {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                अवरोध;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        अस्थायी[1] = (uint8_t) (ch >> 16 & 0x1F);
        अस्थायी[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        अस्थायी[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        क्रम (सूचकांक_To_Write = 0; सूचकांक_To_Write <= माप(uint32_t) - 1; सूचकांक_To_Write++) {
            अगर (myTarget < targetLimit) {
                *(myTarget++) = अस्थायी[सूचकांक_To_Write];
            }
            अन्यथा {
                args->converter->char_Error_बफर[args->converter->char_Error_बफर_लंबाई++] = अस्थायी[सूचकांक_To_Write];
                *err = U_बफर_OVERFLOW_ERROR;
            }
        }
    }

    अगर (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_बफर_OVERFLOW_ERROR;
    }

    args->target = (अक्षर *) myTarget;
    args->source = mySource;
}

अटल व्योम U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    स्थिर char16_t *mySource = args->source;
    अचिन्हित अक्षर *myTarget;
    int32_t *myOffsets;
    स्थिर char16_t *sourceLimit = args->sourceLimit;
    स्थिर अचिन्हित अक्षर *targetLimit = (अचिन्हित अक्षर *) args->targetLimit;
    UChar32 ch, ch2;
    int32_t offset_अंक = 0;
    अचिन्हित पूर्णांक सूचकांक_To_Write;
    अचिन्हित अक्षर अस्थायी[माप(uint32_t)];

    अगर(mySource >= sourceLimit) {
        /* no input, nothing to do */
        वापस;
    }

    /* write the BOM if necessary */
    अगर(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        अटल स्थिर अक्षर bom[]={ 0, 0, (अक्षर)0xfeu, (अक्षर)0xffu };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (अचिन्हित अक्षर *) args->target;
    myOffsets = args->offsets;
    अस्थायी[0] = 0;

    अगर (args->converter->fromUChar32) {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        जाओ lowsurogate;
    }

    जबतक (mySource < sourceLimit && myTarget < targetLimit) {
        ch = *(mySource++);

        अगर (U_IS_SURROGATE(ch)) {
            अगर (U_IS_LEAD(ch)) {
lowsurogate:
                अगर (mySource < sourceLimit) {
                    ch2 = *mySource;
                    अगर (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ऊपरी_START) << HALF_SHIFT) + ch2 + SURROGATE_निचला_BASE;
                        mySource++;
                    }
                    अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        अवरोध;
                    }
                }
                अन्यथा {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    अगर (args->सफाई) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    अवरोध;
                }
            }
            अन्यथा {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                अवरोध;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        अस्थायी[1] = (uint8_t) (ch >> 16 & 0x1F);
        अस्थायी[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        अस्थायी[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        क्रम (सूचकांक_To_Write = 0; सूचकांक_To_Write <= माप(uint32_t) - 1; सूचकांक_To_Write++) {
            अगर (myTarget < targetLimit) {
                *(myTarget++) = अस्थायी[सूचकांक_To_Write];
                *(myOffsets++) = offset_अंक;
            }
            अन्यथा {
                args->converter->char_Error_बफर[args->converter->char_Error_बफर_लंबाई++] = अस्थायी[सूचकांक_To_Write];
                *err = U_बफर_OVERFLOW_ERROR;
            }
        }
        offset_अंक = offset_अंक + 1 + (अस्थायी[1] != 0);
    }

    अगर (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err)) {
        *err = U_बफर_OVERFLOW_ERROR;
    }

    args->target = (अक्षर *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

अटल UChar32 U_CALLCONV
T_UConverter_get_अग्र_UChar_UTF32_BE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    स्थिर uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (स्थिर uint8_t *)args->source;
    अगर (mySource >= (स्थिर uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0xffff;
    }

    length = (int32_t)((स्थिर uint8_t *)args->sourceLimit - mySource);
    अगर (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (स्थिर अक्षर *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        वापस 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[0] << 24)
            | ((UChar32)mySource[1] << 16)
            | ((UChar32)mySource[2] << 8)
            | ((UChar32)mySource[3]);

    args->source = (स्थिर अक्षर *)(mySource + 4);
    अगर ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        वापस myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    वापस 0xffff;
}
U_CDECL_END
अटल स्थिर UConverterImpl _UTF32BEImpl = {
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
    T_UConverter_get_अग्र_UChar_UTF32_BE,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
अटल स्थिर UConverter_Static_तथ्य UTF32_BEStatic_तथ्य = {
    माप(UConverter_Static_तथ्य),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, false, false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

स्थिर UConverter_Shared_तथ्य _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&UTF32_BEStatic_तथ्य, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
अटल व्योम U_CALLCONV
T_UConverter_toUnicode_UTF32_LE(UConverterToUnicodeArgs * args,
                                UErrorCode * err)
{
    स्थिर अचिन्हित अक्षर *mySource = (अचिन्हित अक्षर *) args->source;
    char16_t *myTarget = args->target;
    स्थिर अचिन्हित अक्षर *sourceLimit = (अचिन्हित अक्षर *) args->sourceLimit;
    स्थिर char16_t *targetLimit = args->targetLimit;
    अचिन्हित अक्षर *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    अगर (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        जाओ morebytes;
    }

    जबतक (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        जबतक (i < माप(uint32_t))
        {
            अगर (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (अक्षर) *(mySource++);
            }
            अन्यथा
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                जाओ donefornow;
            }
        }

        अगर (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            अगर (ch <= MAXIMUM_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            अन्यथा {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                अगर (myTarget < targetLimit) {
                    *(myTarget++) = (char16_t)ch;
                }
                अन्यथा {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UChar_Error_बफर[0] = (char16_t) ch;
                    args->converter->UChar_Error_बफर_लंबाई = 1;
                    *err = U_बफर_OVERFLOW_ERROR;
                    अवरोध;
                }
            }
        }
        अन्यथा {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            अवरोध;
        }
    }

donefornow:
    अगर (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_बफर_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (स्थिर अक्षर *) mySource;
}

अटल व्योम U_CALLCONV
T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(UConverterToUnicodeArgs * args,
                                             UErrorCode * err)
{
    स्थिर अचिन्हित अक्षर *mySource = (अचिन्हित अक्षर *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    स्थिर अचिन्हित अक्षर *sourceLimit = (अचिन्हित अक्षर *) args->sourceLimit;
    स्थिर char16_t *targetLimit = args->targetLimit;
    अचिन्हित अक्षर *toUBytes = args->converter->toUBytes;
    uint32_t ch, i;
    int32_t offset_अंक = 0;

    /* Restore state of current sequence */
    अगर (args->converter->toULength > 0 && myTarget < targetLimit)
    {
        i = args->converter->toULength;       /* restore # of bytes consumed */
        args->converter->toULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = args->converter->toUnicodeStatus - 1;
        args->converter->toUnicodeStatus = 0;
        जाओ morebytes;
    }

    जबतक (mySource < sourceLimit && myTarget < targetLimit)
    {
        i = 0;
        ch = 0;
morebytes:
        जबतक (i < माप(uint32_t))
        {
            अगर (mySource < sourceLimit)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                toUBytes[i++] = (अक्षर) *(mySource++);
            }
            अन्यथा
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                args->converter->toUnicodeStatus = ch + 1;
                args->converter->toULength = (int8_t) i;
                जाओ donefornow;
            }
        }

        अगर (ch <= MAXIMUM_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            अगर (ch <= MAXIMUM_UCS2) 
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
                अगर (myTarget < targetLimit)
                {
                    *(myTarget++) = (char16_t)ch;
                    *(myOffsets++) = offset_अंक;
                }
                अन्यथा
                {
                    /* Put in overflow buffer (not handled here) */
                    args->converter->UChar_Error_बफर[0] = (char16_t) ch;
                    args->converter->UChar_Error_बफर_लंबाई = 1;
                    *err = U_बफर_OVERFLOW_ERROR;
                    अवरोध;
                }
            }
        }
        अन्यथा
        {
            args->converter->toULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_FOUND;
            अवरोध;
        }
        offset_अंक += i;
    }

donefornow:
    अगर (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_बफर_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (स्थिर अक्षर *) mySource;
    args->offsets = myOffsets;
}

अटल व्योम U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverterFromUnicodeArgs * args,
                                  UErrorCode * err)
{
    स्थिर char16_t *mySource = args->source;
    अचिन्हित अक्षर *myTarget;
    स्थिर char16_t *sourceLimit = args->sourceLimit;
    स्थिर अचिन्हित अक्षर *targetLimit = (अचिन्हित अक्षर *) args->targetLimit;
    UChar32 ch, ch2;
    अचिन्हित पूर्णांक सूचकांक_To_Write;
    अचिन्हित अक्षर अस्थायी[माप(uint32_t)];

    अगर(mySource >= sourceLimit) {
        /* no input, nothing to do */
        वापस;
    }

    /* write the BOM if necessary */
    अगर(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        अटल स्थिर अक्षर bom[]={ (अक्षर)0xffu, (अक्षर)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (अचिन्हित अक्षर *) args->target;
    अस्थायी[3] = 0;

    अगर (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        जाओ lowsurogate;
    }

    जबतक (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        अगर (U16_IS_SURROGATE(ch)) {
            अगर (U16_IS_LEAD(ch))
            {
lowsurogate:
                अगर (mySource < sourceLimit)
                {
                    ch2 = *mySource;
                    अगर (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ऊपरी_START) << HALF_SHIFT) + ch2 + SURROGATE_निचला_BASE;
                        mySource++;
                    }
                    अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        अवरोध;
                    }
                }
                अन्यथा {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    अगर (args->सफाई) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    अवरोध;
                }
            }
            अन्यथा {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                अवरोध;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        अस्थायी[2] = (uint8_t) (ch >> 16 & 0x1F);
        अस्थायी[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        अस्थायी[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        क्रम (सूचकांक_To_Write = 0; सूचकांक_To_Write <= माप(uint32_t) - 1; सूचकांक_To_Write++)
        {
            अगर (myTarget < targetLimit)
            {
                *(myTarget++) = अस्थायी[सूचकांक_To_Write];
            }
            अन्यथा
            {
                args->converter->char_Error_बफर[args->converter->char_Error_बफर_लंबाई++] = अस्थायी[सूचकांक_To_Write];
                *err = U_बफर_OVERFLOW_ERROR;
            }
        }
    }

    अगर (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_बफर_OVERFLOW_ERROR;
    }

    args->target = (अक्षर *) myTarget;
    args->source = mySource;
}

अटल व्योम U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE_OFFSET_LOGIC(UConverterFromUnicodeArgs * args,
                                               UErrorCode * err)
{
    स्थिर char16_t *mySource = args->source;
    अचिन्हित अक्षर *myTarget;
    int32_t *myOffsets;
    स्थिर char16_t *sourceLimit = args->sourceLimit;
    स्थिर अचिन्हित अक्षर *targetLimit = (अचिन्हित अक्षर *) args->targetLimit;
    UChar32 ch, ch2;
    अचिन्हित पूर्णांक सूचकांक_To_Write;
    अचिन्हित अक्षर अस्थायी[माप(uint32_t)];
    int32_t offset_अंक = 0;

    अगर(mySource >= sourceLimit) {
        /* no input, nothing to do */
        वापस;
    }

    /* write the BOM if necessary */
    अगर(args->converter->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        अटल स्थिर अक्षर bom[]={ (अक्षर)0xffu, (अक्षर)0xfeu, 0, 0 };
        ucnv_fromUWriteBytes(args->converter,
                             bom, 4,
                             &args->target, args->targetLimit,
                             &args->offsets, -1,
                             err);
        args->converter->fromUnicodeStatus=0;
    }

    myTarget = (अचिन्हित अक्षर *) args->target;
    myOffsets = args->offsets;
    अस्थायी[3] = 0;

    अगर (args->converter->fromUChar32)
    {
        ch = args->converter->fromUChar32;
        args->converter->fromUChar32 = 0;
        जाओ lowsurogate;
    }

    जबतक (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        अगर (U16_IS_SURROGATE(ch)) {
            अगर (U16_IS_LEAD(ch))
            {
lowsurogate:
                अगर (mySource < sourceLimit)
                {
                    ch2 = *mySource;
                    अगर (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_ऊपरी_START) << HALF_SHIFT) + ch2 + SURROGATE_निचला_BASE;
                        mySource++;
                    }
                    अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        args->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        अवरोध;
                    }
                }
                अन्यथा {
                    /* ran out of source */
                    args->converter->fromUChar32 = ch;
                    अगर (args->सफाई) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_FOUND;
                    }
                    अवरोध;
                }
            }
            अन्यथा {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                args->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_FOUND;
                अवरोध;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        अस्थायी[2] = (uint8_t) (ch >> 16 & 0x1F);
        अस्थायी[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        अस्थायी[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        क्रम (सूचकांक_To_Write = 0; सूचकांक_To_Write <= माप(uint32_t) - 1; सूचकांक_To_Write++)
        {
            अगर (myTarget < targetLimit)
            {
                *(myTarget++) = अस्थायी[सूचकांक_To_Write];
                *(myOffsets++) = offset_अंक;
            }
            अन्यथा
            {
                args->converter->char_Error_बफर[args->converter->char_Error_बफर_लंबाई++] = अस्थायी[सूचकांक_To_Write];
                *err = U_बफर_OVERFLOW_ERROR;
            }
        }
        offset_अंक = offset_अंक + 1 + (अस्थायी[2] != 0);
    }

    अगर (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_बफर_OVERFLOW_ERROR;
    }

    args->target = (अक्षर *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

अटल UChar32 U_CALLCONV
T_UConverter_get_अग्र_UChar_UTF32_LE(UConverterToUnicodeArgs* args,
                                   UErrorCode* err)
{
    स्थिर uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (स्थिर uint8_t *)args->source;
    अगर (mySource >= (स्थिर uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0xffff;
    }

    length = (int32_t)((स्थिर uint8_t *)args->sourceLimit - mySource);
    अगर (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(args->converter->toUBytes, mySource, length);
        args->converter->toULength = (int8_t)length;
        args->source = (स्थिर अक्षर *)(mySource + length);
        *err = U_TRUNCATED_CHAR_FOUND;
        वापस 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[3] << 24)
            | ((UChar32)mySource[2] << 16)
            | ((UChar32)mySource[1] << 8)
            | ((UChar32)mySource[0]);

    args->source = (स्थिर अक्षर *)(mySource + 4);
    अगर ((uint32_t)myUChar <= MAXIMUM_UTF && !U_IS_SURROGATE(myUChar)) {
        वापस myUChar;
    }

    uprv_memcpy(args->converter->toUBytes, mySource, 4);
    args->converter->toULength = 4;

    *err = U_ILLEGAL_CHAR_FOUND;
    वापस 0xffff;
}
U_CDECL_END
अटल स्थिर UConverterImpl _UTF32LEImpl = {
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
    T_UConverter_get_अग्र_UChar_UTF32_LE,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
अटल स्थिर UConverter_Static_तथ्य UTF32_LEStatic_तथ्य = {
    माप(UConverter_Static_तथ्य),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, false, false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


स्थिर UConverter_Shared_तथ्य _UTF32LEData =
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&UTF32_LEStatic_तथ्य, &_UTF32LEImpl);

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
अटल व्योम U_CALLCONV
_UTF32Reset(UConverter *cnv, UConverterResetChoice choice) {
    अगर(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->mode=0;
    }
    अगर(choice!=UCNV_RESET_TO_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}

अटल व्योम U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverterLoadArgs *pArgs,
           UErrorCode *pErrorCode) {
    (व्योम)pArgs;
    (व्योम)pErrorCode;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

अटल स्थिर अक्षर utf32BOM[8]={ 0, 0, (अक्षर)0xfeu, (अक्षर)0xffu, (अक्षर)0xffu, (अक्षर)0xfeu, 0, 0 };

अटल व्योम U_CALLCONV
_UTF32ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    UConverter *cnv=pArgs->converter;
    स्थिर अक्षर *source=pArgs->source;
    स्थिर अक्षर *sourceLimit=pArgs->sourceLimit;
    int32_t *offsets=pArgs->offsets;

    int32_t state, offsetDelta;
    अक्षर b;

    state=cnv->mode;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    offsetDelta=0;

    जबतक(source<sourceLimit && U_SUCCESS(*pErrorCode)) {
        चयन(state) {
        हाल 0:
            b=*source;
            अगर(b==0) {
                state=1; /* could be 00 00 FE FF */
            } अन्यथा अगर(b==(अक्षर)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } अन्यथा {
                state=8; /* default to UTF-32BE */
                जारी;
            }
            ++source;
            अवरोध;
        हाल 1:
        हाल 2:
        हाल 3:
        हाल 5:
        हाल 6:
        हाल 7:
            अगर(*source==utf32BOM[state]) {
                ++state;
                ++source;
                अगर(state==4) {
                    state=8; /* detect UTF-32BE */
                    offsetDelta=(int32_t)(source-pArgs->source);
                } अन्यथा अगर(state==8) {
                    state=9; /* detect UTF-32LE */
                    offsetDelta=(int32_t)(source-pArgs->source);
                }
            } अन्यथा {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(source-pArgs->source); /* number of bytes from this buffer */

                /* reset the source */
                source=pArgs->source;

                अगर(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } अन्यथा {
                    UBool oldFlush=pArgs->सफाई;

                    /* some of the bytes are from a previous buffer, replay those first */
                    pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
                    pArgs->sourceLimit=pArgs->source+((state&3)-count); /* replay previous bytes */
                    pArgs->सफाई=false; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    pArgs->sourceLimit=sourceLimit;
                    pArgs->सफाई=oldFlush;
                }
                state=8;
                जारी;
            }
            अवरोध;
        हाल 8:
            /* call UTF-32BE */
            pArgs->source=source;
            अगर(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            } अन्यथा {
                T_UConverter_toUnicode_UTF32_BE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            अवरोध;
        हाल 9:
            /* call UTF-32LE */
            pArgs->source=source;
            अगर(offsets==nullptr) {
                T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            } अन्यथा {
                T_UConverter_toUnicode_UTF32_LE_OFFSET_LOGIC(pArgs, pErrorCode);
            }
            source=pArgs->source;
            अवरोध;
        शेष:
            अवरोध; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    अगर(offsets!=nullptr && offsetDelta!=0) {
        int32_t *offsetsLimit=pArgs->offsets;
        जबतक(offsets<offsetsLimit) {
            *offsets++ += offsetDelta;
        }
    }

    pArgs->source=source;

    अगर(source==sourceLimit && pArgs->सफाई) {
        /* handle truncated input */
        चयन(state) {
        हाल 0:
            अवरोध; /* no input at all, nothing to do */
        हाल 8:
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            अवरोध;
        हाल 9:
            T_UConverter_toUnicode_UTF32_LE(pArgs, pErrorCode);
            अवरोध;
        शेष:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            pArgs->source=utf32BOM+(state&4); /* select the correct BOM */
            pArgs->sourceLimit=pArgs->source+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_toUnicode_UTF32_BE(pArgs, pErrorCode);
            pArgs->source=source;
            pArgs->sourceLimit=sourceLimit;
            state=8;
            अवरोध;
        }
    }

    cnv->mode=state;
}

अटल UChar32 U_CALLCONV
UTF32_Get_अग्र_UChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    चयन(pArgs->converter->mode) {
    हाल 8:
        वापस T_UConverter_get_अग्र_UChar_UTF32_BE(pArgs, pErrorCode);
    हाल 9:
        वापस T_UConverter_get_अग्र_UChar_UTF32_LE(pArgs, pErrorCode);
    शेष:
        वापस UCNV_GET_अग्र_UCHAR_USE_TO_U;
    }
}
U_CDECL_END
अटल स्थिर UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    nullptr,
    nullptr,

    _UTF32Open,
    nullptr,
    _UTF32Reset,

    _UTF32ToUnicodeWithOffsets,
    _UTF32ToUnicodeWithOffsets,
#अगर U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_fromUnicode_UTF32_BE_OFFSET_LOGIC,
#अन्यथा
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_fromUnicode_UTF32_LE_OFFSET_LOGIC,
#पूर्ण_अगर
    UTF32_Get_अग्र_UChar,

    nullptr, /* ### TODO implement getStarters for all Unicode encodings?! */
    nullptr,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

/* The 1236 CCSID refers to any version of Unicode with a BOM sensitive endianness of UTF-32 */
अटल स्थिर UConverter_Static_तथ्य UTF32_Static_तथ्य = {
    माप(UConverter_Static_तथ्य),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#अगर U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#अन्यथा
    { 0xfd, 0xff, 0, 0 }, 4,
#पूर्ण_अगर
    false, false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

स्थिर UConverter_Shared_तथ्य UTF32_तथ्य = 
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&UTF32_Static_तथ्य, &_UTF32Impl);

#पूर्ण_अगर
