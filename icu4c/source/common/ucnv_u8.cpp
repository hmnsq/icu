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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION

#समावेश "unicode/ucnv.h"
#समावेश "unicode/utf.h"
#समावेश "unicode/utf8.h"
#समावेश "unicode/utf16.h"
#समावेश "uassert.h"
#समावेश "ucnv_bld.h"
#समावेश "ucnv_cnv.h"
#समावेश "cmemory.h"
#समावेश "ustr_imp.h"

/* Prototypes --------------------------------------------------------------- */

/* Keep these here to make finicky compilers happy */

U_CFUNC व्योम ucnv_fromUnicode_UTF8(UConverterFromUnicodeArgs *args,
                                           UErrorCode *err);
U_CFUNC व्योम ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverterFromUnicodeArgs *args,
                                                        UErrorCode *err);


/* UTF-8 -------------------------------------------------------------------- */

#घोषणा MAXIMUM_UCS2            0x0000FFFF

अटल स्थिर uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

अटल UBool has_CESU8_तथ्य(स्थिर UConverter *cnv)
{
#अगर UCONFIG_ONLY_HTML_CONVERSION
    वापस false;
#अन्यथा
    वापस cnv->shared_तथ्य == &CESU8_तथ्य;
#पूर्ण_अगर
}
U_CDECL_BEGIN
अटल व्योम  U_CALLCONV ucnv_toUnicode_UTF8 (UConverterToUnicodeArgs * args,
                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    स्थिर अचिन्हित अक्षर *mySource = (अचिन्हित अक्षर *) args->source;
    char16_t *myTarget = args->target;
    स्थिर अचिन्हित अक्षर *sourceLimit = (अचिन्हित अक्षर *) args->sourceLimit;
    स्थिर char16_t *targetLimit = args->targetLimit;
    अचिन्हित अक्षर *toUBytes = cnv->toUBytes;
    UBool isCESU8 = has_CESU8_तथ्य(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    अगर (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        जाओ morebytes;
    }


    जबतक (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        अगर (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
        }
        अन्यथा
        {
            /* store the first char */
            toUBytes[0] = (अक्षर)ch;
            inBytes = U8_गिनती_BYTES_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            जबतक (i < inBytes)
            {
                अगर (mySource < sourceLimit)
                {
                    toUBytes[i] = (अक्षर) (ch2 = *mySource);
                    अगर (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        अवरोध; /* i < inBytes */
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
                    जाओ donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            अगर (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                अगर (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                }
                अन्यथा
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    अगर (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                    }
                    अन्यथा
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UChar_Error_बफर[0] = (char16_t) ch;
                        cnv->UChar_Error_बफर_लंबाई = 1;
                        *err = U_बफर_OVERFLOW_ERROR;
                        अवरोध;
                    }
                }
            }
            अन्यथा
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                अवरोध;
            }
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

अटल व्योम  U_CALLCONV ucnv_toUnicode_UTF8_OFFSETS_LOGIC (UConverterToUnicodeArgs * args,
                                                UErrorCode * err)
{
    UConverter *cnv = args->converter;
    स्थिर अचिन्हित अक्षर *mySource = (अचिन्हित अक्षर *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    int32_t offset_अंक = 0;
    स्थिर अचिन्हित अक्षर *sourceLimit = (अचिन्हित अक्षर *) args->sourceLimit;
    स्थिर char16_t *targetLimit = args->targetLimit;
    अचिन्हित अक्षर *toUBytes = cnv->toUBytes;
    UBool isCESU8 = has_CESU8_तथ्य(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    अगर (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        जाओ morebytes;
    }

    जबतक (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        अगर (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
            *(myOffsets++) = offset_अंक++;
        }
        अन्यथा
        {
            toUBytes[0] = (अक्षर)ch;
            inBytes = U8_गिनती_BYTES_NON_ASCII(ch);
            i = 1;

morebytes:
            जबतक (i < inBytes)
            {
                अगर (mySource < sourceLimit)
                {
                    toUBytes[i] = (अक्षर) (ch2 = *mySource);
                    अगर (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        अवरोध; /* i < inBytes */
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
                    जाओ donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            अगर (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                अगर (ch <= MAXIMUM_UCS2) 
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
                    अगर (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                        *(myOffsets++) = offset_अंक;
                    }
                    अन्यथा
                    {
                        cnv->UChar_Error_बफर[0] = (char16_t) ch;
                        cnv->UChar_Error_बफर_लंबाई = 1;
                        *err = U_बफर_OVERFLOW_ERROR;
                    }
                }
                offset_अंक += i;
            }
            अन्यथा
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                अवरोध;
            }
        }
    }

donefornow:
    अगर (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_बफर_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (स्थिर अक्षर *) mySource;
    args->offsets = myOffsets;
}
U_CDECL_END

U_CFUNC व्योम  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverterFromUnicodeArgs * args,
                                    UErrorCode * err)
{
    UConverter *cnv = args->converter;
    स्थिर char16_t *mySource = args->source;
    स्थिर char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    स्थिर uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *अस्थायी_Ptr;
    UChar32 ch;
    uint8_t अस्थायी_Buf[4];
    int32_t सूचकांक_To_Write;
    UBool isNotCESU8 = !has_CESU8_तथ्य(cnv);

    अगर (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        जाओ lowsurrogate;
    }

    जबतक (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        अगर (ch < 0x80)        /* Single byte */
        {
            *(myTarget++) = (uint8_t) ch;
        }
        अन्यथा अगर (ch < 0x800)  /* Double byte */
        {
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            अगर (myTarget < targetLimit)
            {
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            अन्यथा
            {
                cnv->char_Error_बफर[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_बफर_लंबाई = 1;
                *err = U_बफर_OVERFLOW_ERROR;
            }
        }
        अन्यथा {
            /* Check for surrogates */
            अगर(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                अगर (mySource < sourceLimit) {
                    /* test both code units */
                    अगर(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
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
                        अवरोध;
                    }
                }
                अन्यथा {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    अवरोध;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            अस्थायी_Ptr = (((targetLimit - myTarget) >= 4) ? myTarget : अस्थायी_Buf);

            अगर (ch <= MAXIMUM_UCS2) {
                सूचकांक_To_Write = 2;
                अस्थायी_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            अन्यथा {
                सूचकांक_To_Write = 3;
                अस्थायी_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                अस्थायी_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            अस्थायी_Ptr[सूचकांक_To_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            अस्थायी_Ptr[सूचकांक_To_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            अगर (अस्थायी_Ptr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (सूचकांक_To_Write + 1);
            }
            अन्यथा {
                /* We might run out of room soon. Write it slowly. */
                क्रम (; अस्थायी_Ptr <= (अस्थायी_Buf + सूचकांक_To_Write); अस्थायी_Ptr++) {
                    अगर (myTarget < targetLimit) {
                        *(myTarget++) = *अस्थायी_Ptr;
                    }
                    अन्यथा {
                        cnv->char_Error_बफर[cnv->char_Error_बफर_लंबाई++] = *अस्थायी_Ptr;
                        *err = U_बफर_OVERFLOW_ERROR;
                    }
                }
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

U_CFUNC व्योम  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverterFromUnicodeArgs * args,
                                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    स्थिर char16_t *mySource = args->source;
    int32_t *myOffsets = args->offsets;
    स्थिर char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    स्थिर uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *अस्थायी_Ptr;
    UChar32 ch;
    int32_t offset_अंक, अग्र_Source_सूचकांक;
    int32_t सूचकांक_To_Write;
    uint8_t अस्थायी_Buf[4];
    UBool isNotCESU8 = !has_CESU8_तथ्य(cnv);

    अगर (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        offset_अंक = -1;
        अग्र_Source_सूचकांक = 0;
        जाओ lowsurrogate;
    } अन्यथा {
        offset_अंक = 0;
    }

    जबतक (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        अगर (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = offset_अंक++;
            *(myTarget++) = (अक्षर) ch;
        }
        अन्यथा अगर (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = offset_अंक;
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            अगर (myTarget < targetLimit)
            {
                *(myOffsets++) = offset_अंक++;
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            अन्यथा
            {
                cnv->char_Error_बफर[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_बफर_लंबाई = 1;
                *err = U_बफर_OVERFLOW_ERROR;
            }
        }
        अन्यथा
        /* Check for surrogates */
        {
            अग्र_Source_सूचकांक = offset_अंक + 1;

            अगर(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                अगर (mySource < sourceLimit) {
                    /* test both code units */
                    अगर(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        ++अग्र_Source_सूचकांक;
                        /* exit this condition tree */
                    }
                    अन्यथा {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        अवरोध;
                    }
                }
                अन्यथा {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    अवरोध;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            अस्थायी_Ptr = (((targetLimit - myTarget) >= 4) ? myTarget : अस्थायी_Buf);

            अगर (ch <= MAXIMUM_UCS2) {
                सूचकांक_To_Write = 2;
                अस्थायी_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            अन्यथा {
                सूचकांक_To_Write = 3;
                अस्थायी_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                अस्थायी_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            अस्थायी_Ptr[सूचकांक_To_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            अस्थायी_Ptr[सूचकांक_To_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            अगर (अस्थायी_Ptr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (सूचकांक_To_Write + 1);
                myOffsets[0] = offset_अंक;
                myOffsets[1] = offset_अंक;
                myOffsets[2] = offset_अंक;
                अगर (सूचकांक_To_Write >= 3) {
                    myOffsets[3] = offset_अंक;
                }
                myOffsets += (सूचकांक_To_Write + 1);
            }
            अन्यथा {
                /* We might run out of room soon. Write it slowly. */
                क्रम (; अस्थायी_Ptr <= (अस्थायी_Buf + सूचकांक_To_Write); अस्थायी_Ptr++) {
                    अगर (myTarget < targetLimit)
                    {
                        *(myOffsets++) = offset_अंक;
                        *(myTarget++) = *अस्थायी_Ptr;
                    }
                    अन्यथा
                    {
                        cnv->char_Error_बफर[cnv->char_Error_बफर_लंबाई++] = *अस्थायी_Ptr;
                        *err = U_बफर_OVERFLOW_ERROR;
                    }
                }
            }
            offset_अंक = अग्र_Source_सूचकांक;
        }
    }

    अगर (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_बफर_OVERFLOW_ERROR;
    }

    args->target = (अक्षर *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

U_CDECL_BEGIN
अटल UChar32 U_CALLCONV ucnv_get_अग्र_UChar_UTF8(UConverterToUnicodeArgs *args,
                                               UErrorCode *err) {
    UConverter *cnv;
    स्थिर uint8_t *sourceInitial;
    स्थिर uint8_t *source;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = args->converter;
    sourceInitial = source = (स्थिर uint8_t *)args->source;
    अगर (source >= (स्थिर uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0xffff;
    }

    myByte = *(source++);
    अगर (U8_IS_SINGLE(myByte))
    {
        args->source = (स्थिर अक्षर *)source;
        वापस (UChar32)myByte;
    }

    uint16_t गिनती_Trail_Bytes = U8_गिनती_TRAIL_BYTES(myByte);
    अगर (गिनती_Trail_Bytes == 0) {
        cnv->toUBytes[0] = myByte;
        cnv->toULength = 1;
        *err = U_ILLEGAL_CHAR_FOUND;
        args->source = (स्थिर अक्षर *)source;
        वापस 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    अगर (((स्थिर अक्षर *)source + गिनती_Trail_Bytes) > args->sourceLimit)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extraBytesToWrite = गिनती_Trail_Bytes + 1;
        cnv->toUBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_FOUND;
        जबतक(source < (स्थिर uint8_t *)args->sourceLimit) {
            uint8_t b = *source;
            अगर(icu::UTF8::isValidTrail(myByte, b, i, extraBytesToWrite)) {
                cnv->toUBytes[i++] = b;
                ++source;
            } अन्यथा {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_FOUND;
                अवरोध;
            }
        }
        cnv->toULength = i;
        args->source = (स्थिर अक्षर *)source;
        वापस 0xffff;
    }

    ch = myByte << 6;
    अगर(गिनती_Trail_Bytes == 2) {
        uint8_t t1 = *source, t2;
        अगर(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source)) {
            args->source = (स्थिर अक्षर *)(source + 1);
            वापस (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } अन्यथा अगर(गिनती_Trail_Bytes == 1) {
        uint8_t t1 = *source;
        अगर(U8_IS_TRAIL(t1)) {
            args->source = (स्थिर अक्षर *)(source + 1);
            वापस (ch + t1) - offsetsFromUTF8[2];
        }
    } अन्यथा {  // countTrailBytes == 3
        uint8_t t1 = *source, t2, t3;
        अगर(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source) &&
                U8_IS_TRAIL(t3 = *++source)) {
            args->source = (स्थिर अक्षर *)(source + 1);
            वापस (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    args->source = (स्थिर अक्षर *)source;

    क्रम(i = 0; sourceInitial < source; ++i) {
        cnv->toUBytes[i] = *sourceInitial++;
    }
    cnv->toULength = i;
    *err = U_ILLEGAL_CHAR_FOUND;
    वापस 0xffff;
} 
U_CDECL_END

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
अटल व्योम U_CALLCONV
ucnv_UTF8FromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                  UConverterToUnicodeArgs *pToUArgs,
                  UErrorCode *pErrorCode) {
    UConverter *utf8;
    स्थिर uint8_t *source, *sourceLimit;
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
    अगर(utf8->toULength > 0) {
        toULength=oldToULength=utf8->toULength;
        toULimit=(int8_t)utf8->mode;
        c=(UChar32)utf8->toUnicodeStatus;
    } अन्यथा {
        toULength=oldToULength=toULimit=0;
        c = 0;
    }

    count=(int32_t)(sourceLimit-source)+oldToULength;
    अगर(count<toULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } अन्यथा अगर(targetCapacity<toULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *pErrorCode=U_USING_DEFAULT_WARNING;
        वापस;
    } अन्यथा {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        अगर(count>targetCapacity) {
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

    अगर(c!=0) {
        utf8->toUnicodeStatus=0;
        utf8->toULength=0;
        जाओ moreBytes;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    जबतक(count>0) {
        b=*source++;
        अगर(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *target++=b;
            --count;
            जारी;
        } अन्यथा {
            अगर(b>=0xe0) {
                अगर( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=source[0]) &&
                    U8_IS_TRAIL(t2=source[1])
                ) {
                    source+=2;
                    *target++=b;
                    *target++=t1;
                    *target++=t2;
                    count-=3;
                    जारी;
                }
            } अन्यथा {
                अगर( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*source)
                ) {
                    ++source;
                    *target++=b;
                    *target++=t1;
                    count-=2;
                    जारी;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            oldToULength=0;
            toULength=1;
            toULimit=U8_गिनती_BYTES_NON_ASCII(b);
            c=b;
moreBytes:
            जबतक(toULength<toULimit) {
                अगर(source<sourceLimit) {
                    b=*source;
                    अगर(icu::UTF8::isValidTrail(c, b, toULength, toULimit)) {
                        ++source;
                        ++toULength;
                        c=(c<<6)+b;
                    } अन्यथा {
                        अवरोध; /* sequence too short, stop with toULength<toULimit */
                    }
                } अन्यथा {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    source-=(toULength-oldToULength);
                    जबतक(oldToULength<toULength) {
                        utf8->toUBytes[oldToULength++]=*source++;
                    }
                    utf8->toUnicodeStatus=c;
                    utf8->toULength=toULength;
                    utf8->mode=toULimit;
                    pToUArgs->source=(अक्षर *)source;
                    pFromUArgs->target=(अक्षर *)target;
                    वापस;
                }
            }

            अगर(toULength!=toULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                source-=(toULength-oldToULength);
                जबतक(oldToULength<toULength) {
                    utf8->toUBytes[oldToULength++]=*source++;
                }
                utf8->toULength=toULength;
                pToUArgs->source=(अक्षर *)source;
                pFromUArgs->target=(अक्षर *)target;
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                वापस;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                क्रम(i=0; i<oldToULength; ++i) {
                    *target++=utf8->toUBytes[i];
                }
                source-=(toULength-oldToULength);
                क्रम(; i<toULength; ++i) {
                    *target++=*source++;
                }
                count-=toULength;
            }
        }
    }
    U_ASSERT(count>=0);

    अगर(U_SUCCESS(*pErrorCode) && source<sourceLimit) {
        अगर(target==(स्थिर uint8_t *)pFromUArgs->targetLimit) {
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
        } अन्यथा {
            b=*source;
            toULimit=U8_गिनती_BYTES(b);
            अगर(toULimit>(sourceLimit-source)) {
                /* collect a truncated byte sequence */
                toULength=0;
                c=b;
                क्रम(;;) {
                    utf8->toUBytes[toULength++]=b;
                    अगर(++source==sourceLimit) {
                        /* partial byte sequence at end of source */
                        utf8->toUnicodeStatus=c;
                        utf8->toULength=toULength;
                        utf8->mode=toULimit;
                        अवरोध;
                    } अन्यथा अगर(!icu::UTF8::isValidTrail(c, b=*source, toULength, toULimit)) {
                        utf8->toULength=toULength;
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        अवरोध;
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

अटल स्थिर UConverterImpl _UTF8Impl={
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
    ucnv_get_अग्र_UChar_UTF8,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    ucnv_UTF8FromUTF8,
    ucnv_UTF8FromUTF8
};

/* The 1208 CCSID refers to any version of Unicode of UTF-8 */
अटल स्थिर UConverter_Static_तथ्य UTF8_Static_तथ्य={
    माप(UConverter_Static_तथ्य),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,false,false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


स्थिर UConverter_Shared_तथ्य UTF8_तथ्य=
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&UTF8_Static_तथ्य, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

अटल स्थिर UConverterImpl _CESU8Impl={
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

अटल स्थिर UConverter_Static_तथ्य CESU8_Static_तथ्य={
    माप(UConverter_Static_तथ्य),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,false,false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


स्थिर UConverter_Shared_तथ्य CESU8_तथ्य=
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&CESU8_Static_तथ्य, &_CESU8Impl);

#पूर्ण_अगर
