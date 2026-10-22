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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION

#অন্তর্ভাব "unicode/ucnv.h"
#অন্তর্ভাব "unicode/utf.h"
#অন্তর্ভাব "unicode/utf8.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ucnv_bld.h"
#অন্তর্ভাব "ucnv_cnv.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "ustr_imp.h"

/* Prototypes --------------------------------------------------------------- */

/* Keep these here to make finicky compilers happy */

U_CFUNC শূন্য ucnv_fromUnicode_UTF8(UConverterFromUnicodeArgs *args,
                                           UErrorCode *err);
U_CFUNC শূন্য ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverterFromUnicodeArgs *args,
                                                        UErrorCode *err);


/* UTF-8 -------------------------------------------------------------------- */

#define MAXIMUM_UCS2            0x0000FFFF

স্থির ধ্রুবক uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

স্থির UBool has_CESU8_তথ্য(ধ্রুবক UConverter *cnv)
{
#যদি UCONFIG_ONLY_HTML_CONVERSION
    ফেরত মিথ্যা;
#অন্যথা
    ফেরত cnv->shared_তথ্য == &CESU8_তথ্য;
#endif
}
U_CDECL_BEGIN
স্থির শূন্য  U_CALLCONV ucnv_toUnicode_UTF8 (UConverterToUnicodeArgs * args,
                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    ধ্রুবক unsigned অক্ষর *mySource = (unsigned অক্ষর *) args->source;
    char16_t *myTarget = args->target;
    ধ্রুবক unsigned অক্ষর *sourceLimit = (unsigned অক্ষর *) args->sourceLimit;
    ধ্রুবক char16_t *targetLimit = args->targetLimit;
    unsigned অক্ষর *toUBytes = cnv->toUBytes;
    UBool isCESU8 = has_CESU8_তথ্য(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    যদি (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        যাও morebytes;
    }


    যখন (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        যদি (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
        }
        অন্যথা
        {
            /* store the first char */
            toUBytes[0] = (অক্ষর)ch;
            inBytes = U8_গুনতি_BYTES_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            যখন (i < inBytes)
            {
                যদি (mySource < sourceLimit)
                {
                    toUBytes[i] = (অক্ষর) (ch2 = *mySource);
                    যদি (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        বিরতি; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                অন্যথা
                {
                    /* stores a partially calculated target*/
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t) i;
                    যাও donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            যদি (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                যদি (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                }
                অন্যথা
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    যদি (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                    }
                    অন্যথা
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UChar_Error_বাফার[0] = (char16_t) ch;
                        cnv->UChar_Error_বাফার_দৈর্ঘ্য = 1;
                        *err = U_বাফার_OVERFLOW_ERROR;
                        বিরতি;
                    }
                }
            }
            অন্যথা
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                বিরতি;
            }
        }
    }

donefornow:
    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_বাফার_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ধ্রুবক অক্ষর *) mySource;
}

স্থির শূন্য  U_CALLCONV ucnv_toUnicode_UTF8_OFFSETS_LOGIC (UConverterToUnicodeArgs * args,
                                                UErrorCode * err)
{
    UConverter *cnv = args->converter;
    ধ্রুবক unsigned অক্ষর *mySource = (unsigned অক্ষর *) args->source;
    char16_t *myTarget = args->target;
    int32_t *myOffsets = args->offsets;
    int32_t offset_অঙ্ক = 0;
    ধ্রুবক unsigned অক্ষর *sourceLimit = (unsigned অক্ষর *) args->sourceLimit;
    ধ্রুবক char16_t *targetLimit = args->targetLimit;
    unsigned অক্ষর *toUBytes = cnv->toUBytes;
    UBool isCESU8 = has_CESU8_তথ্য(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, inBytes;

    /* Restore size of current sequence */
    যদি (cnv->toULength > 0 && myTarget < targetLimit)
    {
        inBytes = cnv->mode;            /* restore # of bytes to consume */
        i = cnv->toULength;             /* restore # of bytes consumed */
        cnv->toULength = 0;

        ch = cnv->toUnicodeStatus;/*Stores the previously calculated ch from a previous call*/
        cnv->toUnicodeStatus = 0;
        যাও morebytes;
    }

    যখন (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);
        যদি (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
            *(myOffsets++) = offset_অঙ্ক++;
        }
        অন্যথা
        {
            toUBytes[0] = (অক্ষর)ch;
            inBytes = U8_গুনতি_BYTES_NON_ASCII(ch);
            i = 1;

morebytes:
            যখন (i < inBytes)
            {
                যদি (mySource < sourceLimit)
                {
                    toUBytes[i] = (অক্ষর) (ch2 = *mySource);
                    যদি (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, inBytes) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        বিরতি; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                অন্যথা
                {
                    cnv->toUnicodeStatus = ch;
                    cnv->mode = inBytes;
                    cnv->toULength = (int8_t)i;
                    যাও donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            যদি (i == inBytes && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[inBytes];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                যদি (ch <= MAXIMUM_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                    *(myOffsets++) = offset_অঙ্ক;
                }
                অন্যথা
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    *(myOffsets++) = offset_অঙ্ক;
                    ch = U16_TRAIL(ch);
                    যদি (myTarget < targetLimit)
                    {
                        *(myTarget++) = (char16_t)ch;
                        *(myOffsets++) = offset_অঙ্ক;
                    }
                    অন্যথা
                    {
                        cnv->UChar_Error_বাফার[0] = (char16_t) ch;
                        cnv->UChar_Error_বাফার_দৈর্ঘ্য = 1;
                        *err = U_বাফার_OVERFLOW_ERROR;
                    }
                }
                offset_অঙ্ক += i;
            }
            অন্যথা
            {
                cnv->toULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_FOUND;
                বিরতি;
            }
        }
    }

donefornow:
    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_বাফার_OVERFLOW_ERROR;
    }

    args->target = myTarget;
    args->source = (ধ্রুবক অক্ষর *) mySource;
    args->offsets = myOffsets;
}
U_CDECL_END

U_CFUNC শূন্য  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverterFromUnicodeArgs * args,
                                    UErrorCode * err)
{
    UConverter *cnv = args->converter;
    ধ্রুবক char16_t *mySource = args->source;
    ধ্রুবক char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    ধ্রুবক uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *অস্থায়ী_Ptr;
    UChar32 ch;
    uint8_t অস্থায়ী_Buf[4];
    int32_t সূচক_To_Write;
    UBool isNotCESU8 = !has_CESU8_তথ্য(cnv);

    যদি (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        যাও lowsurrogate;
    }

    যখন (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        যদি (ch < 0x80)        /* Single byte */
        {
            *(myTarget++) = (uint8_t) ch;
        }
        অন্যথা যদি (ch < 0x800)  /* Double byte */
        {
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            যদি (myTarget < targetLimit)
            {
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            অন্যথা
            {
                cnv->char_Error_বাফার[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_বাফার_দৈর্ঘ্য = 1;
                *err = U_বাফার_OVERFLOW_ERROR;
            }
        }
        অন্যথা {
            /* Check for surrogates */
            যদি(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                যদি (mySource < sourceLimit) {
                    /* test both code units */
                    যদি(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        /* exit this condition tree */
                    }
                    অন্যথা {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        বিরতি;
                    }
                }
                অন্যথা {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    বিরতি;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            অস্থায়ী_Ptr = (((targetLimit - myTarget) >= 4) ? myTarget : অস্থায়ী_Buf);

            যদি (ch <= MAXIMUM_UCS2) {
                সূচক_To_Write = 2;
                অস্থায়ী_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            অন্যথা {
                সূচক_To_Write = 3;
                অস্থায়ী_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                অস্থায়ী_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            অস্থায়ী_Ptr[সূচক_To_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            অস্থায়ী_Ptr[সূচক_To_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            যদি (অস্থায়ী_Ptr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (সূচক_To_Write + 1);
            }
            অন্যথা {
                /* We might run out of room soon. Write it slowly. */
                জন্য (; অস্থায়ী_Ptr <= (অস্থায়ী_Buf + সূচক_To_Write); অস্থায়ী_Ptr++) {
                    যদি (myTarget < targetLimit) {
                        *(myTarget++) = *অস্থায়ী_Ptr;
                    }
                    অন্যথা {
                        cnv->char_Error_বাফার[cnv->char_Error_বাফার_দৈর্ঘ্য++] = *অস্থায়ী_Ptr;
                        *err = U_বাফার_OVERFLOW_ERROR;
                    }
                }
            }
        }
    }

    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_বাফার_OVERFLOW_ERROR;
    }

    args->target = (অক্ষর *) myTarget;
    args->source = mySource;
}

U_CFUNC শূন্য  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverterFromUnicodeArgs * args,
                                                  UErrorCode * err)
{
    UConverter *cnv = args->converter;
    ধ্রুবক char16_t *mySource = args->source;
    int32_t *myOffsets = args->offsets;
    ধ্রুবক char16_t *sourceLimit = args->sourceLimit;
    uint8_t *myTarget = (uint8_t *) args->target;
    ধ্রুবক uint8_t *targetLimit = (uint8_t *) args->targetLimit;
    uint8_t *অস্থায়ী_Ptr;
    UChar32 ch;
    int32_t offset_অঙ্ক, পরবর্তী_Source_সূচক;
    int32_t সূচক_To_Write;
    uint8_t অস্থায়ী_Buf[4];
    UBool isNotCESU8 = !has_CESU8_তথ্য(cnv);

    যদি (cnv->fromUChar32 && myTarget < targetLimit)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        offset_অঙ্ক = -1;
        পরবর্তী_Source_সূচক = 0;
        যাও lowsurrogate;
    } অন্যথা {
        offset_অঙ্ক = 0;
    }

    যখন (mySource < sourceLimit && myTarget < targetLimit)
    {
        ch = *(mySource++);

        যদি (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = offset_অঙ্ক++;
            *(myTarget++) = (অক্ষর) ch;
        }
        অন্যথা যদি (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = offset_অঙ্ক;
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            যদি (myTarget < targetLimit)
            {
                *(myOffsets++) = offset_অঙ্ক++;
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            অন্যথা
            {
                cnv->char_Error_বাফার[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_বাফার_দৈর্ঘ্য = 1;
                *err = U_বাফার_OVERFLOW_ERROR;
            }
        }
        অন্যথা
        /* Check for surrogates */
        {
            পরবর্তী_Source_সূচক = offset_অঙ্ক + 1;

            যদি(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                যদি (mySource < sourceLimit) {
                    /* test both code units */
                    যদি(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        ++পরবর্তী_Source_সূচক;
                        /* exit this condition tree */
                    }
                    অন্যথা {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_FOUND;
                        বিরতি;
                    }
                }
                অন্যথা {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    বিরতি;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            অস্থায়ী_Ptr = (((targetLimit - myTarget) >= 4) ? myTarget : অস্থায়ী_Buf);

            যদি (ch <= MAXIMUM_UCS2) {
                সূচক_To_Write = 2;
                অস্থায়ী_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            অন্যথা {
                সূচক_To_Write = 3;
                অস্থায়ী_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                অস্থায়ী_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            অস্থায়ী_Ptr[সূচক_To_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            অস্থায়ী_Ptr[সূচক_To_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            যদি (অস্থায়ী_Ptr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (সূচক_To_Write + 1);
                myOffsets[0] = offset_অঙ্ক;
                myOffsets[1] = offset_অঙ্ক;
                myOffsets[2] = offset_অঙ্ক;
                যদি (সূচক_To_Write >= 3) {
                    myOffsets[3] = offset_অঙ্ক;
                }
                myOffsets += (সূচক_To_Write + 1);
            }
            অন্যথা {
                /* We might run out of room soon. Write it slowly. */
                জন্য (; অস্থায়ী_Ptr <= (অস্থায়ী_Buf + সূচক_To_Write); অস্থায়ী_Ptr++) {
                    যদি (myTarget < targetLimit)
                    {
                        *(myOffsets++) = offset_অঙ্ক;
                        *(myTarget++) = *অস্থায়ী_Ptr;
                    }
                    অন্যথা
                    {
                        cnv->char_Error_বাফার[cnv->char_Error_বাফার_দৈর্ঘ্য++] = *অস্থায়ী_Ptr;
                        *err = U_বাফার_OVERFLOW_ERROR;
                    }
                }
            }
            offset_অঙ্ক = পরবর্তী_Source_সূচক;
        }
    }

    যদি (mySource < sourceLimit && myTarget >= targetLimit && U_SUCCESS(*err))
    {
        *err = U_বাফার_OVERFLOW_ERROR;
    }

    args->target = (অক্ষর *) myTarget;
    args->source = mySource;
    args->offsets = myOffsets;
}

U_CDECL_BEGIN
স্থির UChar32 U_CALLCONV ucnv_get_পরবর্তী_UChar_UTF8(UConverterToUnicodeArgs *args,
                                               UErrorCode *err) {
    UConverter *cnv;
    ধ্রুবক uint8_t *sourceInitial;
    ধ্রুবক uint8_t *source;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = args->converter;
    sourceInitial = source = (ধ্রুবক uint8_t *)args->source;
    যদি (source >= (ধ্রুবক uint8_t *)args->sourceLimit)
    {
        /* no input */
        *err = U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0xffff;
    }

    myByte = *(source++);
    যদি (U8_IS_SINGLE(myByte))
    {
        args->source = (ধ্রুবক অক্ষর *)source;
        ফেরত (UChar32)myByte;
    }

    uint16_t গুনতি_Trail_Bytes = U8_গুনতি_TRAIL_BYTES(myByte);
    যদি (গুনতি_Trail_Bytes == 0) {
        cnv->toUBytes[0] = myByte;
        cnv->toULength = 1;
        *err = U_ILLEGAL_CHAR_FOUND;
        args->source = (ধ্রুবক অক্ষর *)source;
        ফেরত 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    যদি (((ধ্রুবক অক্ষর *)source + গুনতি_Trail_Bytes) > args->sourceLimit)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extraBytesToWrite = গুনতি_Trail_Bytes + 1;
        cnv->toUBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_FOUND;
        যখন(source < (ধ্রুবক uint8_t *)args->sourceLimit) {
            uint8_t b = *source;
            যদি(icu::UTF8::isValidTrail(myByte, b, i, extraBytesToWrite)) {
                cnv->toUBytes[i++] = b;
                ++source;
            } অন্যথা {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_FOUND;
                বিরতি;
            }
        }
        cnv->toULength = i;
        args->source = (ধ্রুবক অক্ষর *)source;
        ফেরত 0xffff;
    }

    ch = myByte << 6;
    যদি(গুনতি_Trail_Bytes == 2) {
        uint8_t t1 = *source, t2;
        যদি(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source)) {
            args->source = (ধ্রুবক অক্ষর *)(source + 1);
            ফেরত (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } অন্যথা যদি(গুনতি_Trail_Bytes == 1) {
        uint8_t t1 = *source;
        যদি(U8_IS_TRAIL(t1)) {
            args->source = (ধ্রুবক অক্ষর *)(source + 1);
            ফেরত (ch + t1) - offsetsFromUTF8[2];
        }
    } অন্যথা {  // countTrailBytes == 3
        uint8_t t1 = *source, t2, t3;
        যদি(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source) &&
                U8_IS_TRAIL(t3 = *++source)) {
            args->source = (ধ্রুবক অক্ষর *)(source + 1);
            ফেরত (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    args->source = (ধ্রুবক অক্ষর *)source;

    জন্য(i = 0; sourceInitial < source; ++i) {
        cnv->toUBytes[i] = *sourceInitial++;
    }
    cnv->toULength = i;
    *err = U_ILLEGAL_CHAR_FOUND;
    ফেরত 0xffff;
} 
U_CDECL_END

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
স্থির শূন্য U_CALLCONV
ucnv_UTF8FromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                  UConverterToUnicodeArgs *pToUArgs,
                  UErrorCode *pErrorCode) {
    UConverter *utf8;
    ধ্রুবক uint8_t *source, *sourceLimit;
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
    যদি(utf8->toULength > 0) {
        toULength=oldToULength=utf8->toULength;
        toULimit=(int8_t)utf8->mode;
        c=(UChar32)utf8->toUnicodeStatus;
    } অন্যথা {
        toULength=oldToULength=toULimit=0;
        c = 0;
    }

    count=(int32_t)(sourceLimit-source)+oldToULength;
    যদি(count<toULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } অন্যথা যদি(targetCapacity<toULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *pErrorCode=U_USING_DEFAULT_WARNING;
        ফেরত;
    } অন্যথা {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        যদি(count>targetCapacity) {
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

    যদি(c!=0) {
        utf8->toUnicodeStatus=0;
        utf8->toULength=0;
        যাও moreBytes;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    যখন(count>0) {
        b=*source++;
        যদি(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *target++=b;
            --count;
            চালু;
        } অন্যথা {
            যদি(b>=0xe0) {
                যদি( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=source[0]) &&
                    U8_IS_TRAIL(t2=source[1])
                ) {
                    source+=2;
                    *target++=b;
                    *target++=t1;
                    *target++=t2;
                    count-=3;
                    চালু;
                }
            } অন্যথা {
                যদি( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*source)
                ) {
                    ++source;
                    *target++=b;
                    *target++=t1;
                    count-=2;
                    চালু;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            oldToULength=0;
            toULength=1;
            toULimit=U8_গুনতি_BYTES_NON_ASCII(b);
            c=b;
moreBytes:
            যখন(toULength<toULimit) {
                যদি(source<sourceLimit) {
                    b=*source;
                    যদি(icu::UTF8::isValidTrail(c, b, toULength, toULimit)) {
                        ++source;
                        ++toULength;
                        c=(c<<6)+b;
                    } অন্যথা {
                        বিরতি; /* sequence too short, stop with toULength<toULimit */
                    }
                } অন্যথা {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    source-=(toULength-oldToULength);
                    যখন(oldToULength<toULength) {
                        utf8->toUBytes[oldToULength++]=*source++;
                    }
                    utf8->toUnicodeStatus=c;
                    utf8->toULength=toULength;
                    utf8->mode=toULimit;
                    pToUArgs->source=(অক্ষর *)source;
                    pFromUArgs->target=(অক্ষর *)target;
                    ফেরত;
                }
            }

            যদি(toULength!=toULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                source-=(toULength-oldToULength);
                যখন(oldToULength<toULength) {
                    utf8->toUBytes[oldToULength++]=*source++;
                }
                utf8->toULength=toULength;
                pToUArgs->source=(অক্ষর *)source;
                pFromUArgs->target=(অক্ষর *)target;
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                ফেরত;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                জন্য(i=0; i<oldToULength; ++i) {
                    *target++=utf8->toUBytes[i];
                }
                source-=(toULength-oldToULength);
                জন্য(; i<toULength; ++i) {
                    *target++=*source++;
                }
                count-=toULength;
            }
        }
    }
    U_ASSERT(count>=0);

    যদি(U_SUCCESS(*pErrorCode) && source<sourceLimit) {
        যদি(target==(ধ্রুবক uint8_t *)pFromUArgs->targetLimit) {
            *pErrorCode=U_বাফার_OVERFLOW_ERROR;
        } অন্যথা {
            b=*source;
            toULimit=U8_গুনতি_BYTES(b);
            যদি(toULimit>(sourceLimit-source)) {
                /* collect a truncated byte sequence */
                toULength=0;
                c=b;
                জন্য(;;) {
                    utf8->toUBytes[toULength++]=b;
                    যদি(++source==sourceLimit) {
                        /* partial byte sequence at end of source */
                        utf8->toUnicodeStatus=c;
                        utf8->toULength=toULength;
                        utf8->mode=toULimit;
                        বিরতি;
                    } অন্যথা যদি(!icu::UTF8::isValidTrail(c, b=*source, toULength, toULimit)) {
                        utf8->toULength=toULength;
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        বিরতি;
                    }
                    c=(c<<6)+b;
                }
            } অন্যথা {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *pErrorCode=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    pToUArgs->source=(অক্ষর *)source;
    pFromUArgs->target=(অক্ষর *)target;
}

U_CDECL_END

/* UTF-8 converter data ----------------------------------------------------- */

স্থির ধ্রুবক UConverterImpl _UTF8Impl={
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
    ucnv_get_পরবর্তী_UChar_UTF8,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    ucnv_UTF8FromUTF8,
    ucnv_UTF8FromUTF8
};

/* The 1208 CCSID refers to any version of Unicode of UTF-8 */
স্থির ধ্রুবক UConverter_Static_তথ্য UTF8_Static_তথ্য={
    আকার(UConverter_Static_তথ্য),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,মিথ্যা,মিথ্যা,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ধ্রুবক UConverter_Shared_তথ্য UTF8_তথ্য=
        UCNV_IMMUTABLE_SHARED_তথ্য_INITIALIZER(&UTF8_Static_তথ্য, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

স্থির ধ্রুবক UConverterImpl _CESU8Impl={
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

স্থির ধ্রুবক UConverter_Static_তথ্য CESU8_Static_তথ্য={
    আকার(UConverter_Static_তথ্য),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,মিথ্যা,মিথ্যা,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ধ্রুবক UConverter_Shared_তথ্য CESU8_তথ্য=
        UCNV_IMMUTABLE_SHARED_তথ্য_INITIALIZER(&CESU8_Static_তথ্য, &_CESU8Impl);

#endif
