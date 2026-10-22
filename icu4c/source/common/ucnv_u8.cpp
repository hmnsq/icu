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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن "unicode/ucnv.h"
#ضمن "unicode/utf.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "uassert.h"
#ضمن "ucnv_bld.h"
#ضمن "ucnv_cnv.h"
#ضمن "cmemory.h"
#ضمن "ustr_imp.h"

/* Prototypes --------------------------------------------------------------- */

/* Keep these here to make finicky compilers happy */

U_CFUNC فراغ ucnv_fromUnicode_UTF8(UConverter_From_Unicode_دلیلونه *دلیلونه_2,
                                           UError_کوډ *err);
U_CFUNC فراغ ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverter_From_Unicode_دلیلونه *دلیلونه_2,
                                                        UError_کوډ *err);


/* UTF-8 -------------------------------------------------------------------- */

#تعریف_کړه اعظمي_UCS2            0x0000FFFF

ساكن ثابت uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

ساكن UBool has_CESU8_معلومات(ثابت UConverter *cnv)
{
#إذا UCONFIG_ONLY_HTML_CONVERSION
    أرجع خطأ;
#وإلا
    أرجع cnv->shared_معلومات == &CESU8_معلومات;
#د_که_پای
}
U_CDECL_BEGIN
ساكن فراغ  U_CALLCONV ucnv_منزل_Unicode_UTF8 (UConverter_منزل_Unicode_دلیلونه * دلیلونه_2,
                                  UError_کوډ * err)
{
    UConverter *cnv = دلیلونه_2->converter;
    ثابت بینښه حرف *my_سرچینه = (بینښه حرف *) دلیلونه_2->سرچینه;
    char16_t *my_موخه = دلیلونه_2->موخه;
    ثابت بینښه حرف *سرچینه_حد = (بینښه حرف *) دلیلونه_2->سرچینه_حد;
    ثابت char16_t *موخه_حد = دلیلونه_2->موخه_حد;
    بینښه حرف *منزل_UBytes = cnv->منزل_UBytes;
    UBool isCESU8 = has_CESU8_معلومات(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, دننه_بایټونه;

    /* Restore size of current sequence */
    إذا (cnv->منزل_ULength > 0 && my_موخه < موخه_حد)
    {
        دننه_بایټونه = cnv->ډېرپېښ;            /* restore # of bytes to consume */
        i = cnv->منزل_ULength;             /* restore # of bytes consumed */
        cnv->منزل_ULength = 0;

        ch = cnv->منزل_Unicode_حالت;/*Stores the previously calculated ch from a previous call*/
        cnv->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }


    طالما (my_سرچینه < سرچینه_حد && my_موخه < موخه_حد)
    {
        ch = *(my_سرچینه++);
        إذا (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(my_موخه++) = (char16_t) ch;
        }
        وإلا
        {
            /* store the first char */
            منزل_UBytes[0] = (حرف)ch;
            دننه_بایټونه = U8_شمېر_بایټونه_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            طالما (i < دننه_بایټونه)
            {
                إذا (my_سرچینه < سرچینه_حد)
                {
                    منزل_UBytes[i] = (حرف) (ch2 = *my_سرچینه);
                    إذا (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, دننه_بایټونه) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        اكسر; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++my_سرچینه;
                    i++;
                }
                وإلا
                {
                    /* stores a partially calculated target*/
                    cnv->منزل_Unicode_حالت = ch;
                    cnv->ډېرپېښ = دننه_بایټونه;
                    cnv->منزل_ULength = (int8_t) i;
                    اذهب donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            إذا (i == دننه_بایټونه && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[دننه_بایټونه];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                إذا (ch <= اعظمي_UCS2) 
                {
                    /* fits in 16 bits */
                    *(my_موخه++) = (char16_t) ch;
                }
                وإلا
                {
                    /* write out the surrogates */
                    *(my_موخه++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    إذا (my_موخه < موخه_حد)
                    {
                        *(my_موخه++) = (char16_t)ch;
                    }
                    وإلا
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UChar_Error_بفر[0] = (char16_t) ch;
                        cnv->UChar_Error_بفر_اوږدتیا = 1;
                        *err = U_بفر_ډکېدنه_ERROR;
                        اكسر;
                    }
                }
            }
            وإلا
            {
                cnv->منزل_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_وموندل;
                اكسر;
            }
        }
    }

donefornow:
    إذا (my_سرچینه < سرچینه_حد && my_موخه >= موخه_حد && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_بفر_ډکېدنه_ERROR;
    }

    دلیلونه_2->موخه = my_موخه;
    دلیلونه_2->سرچینه = (ثابت حرف *) my_سرچینه;
}

ساكن فراغ  U_CALLCONV ucnv_منزل_Unicode_UTF8_OFFSETS_LOGIC (UConverter_منزل_Unicode_دلیلونه * دلیلونه_2,
                                                UError_کوډ * err)
{
    UConverter *cnv = دلیلونه_2->converter;
    ثابت بینښه حرف *my_سرچینه = (بینښه حرف *) دلیلونه_2->سرچینه;
    char16_t *my_موخه = دلیلونه_2->موخه;
    int32_t *myOffsets = دلیلونه_2->offsets;
    int32_t دننه_واټن_عدد = 0;
    ثابت بینښه حرف *سرچینه_حد = (بینښه حرف *) دلیلونه_2->سرچینه_حد;
    ثابت char16_t *موخه_حد = دلیلونه_2->موخه_حد;
    بینښه حرف *منزل_UBytes = cnv->منزل_UBytes;
    UBool isCESU8 = has_CESU8_معلومات(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, دننه_بایټونه;

    /* Restore size of current sequence */
    إذا (cnv->منزل_ULength > 0 && my_موخه < موخه_حد)
    {
        دننه_بایټونه = cnv->ډېرپېښ;            /* restore # of bytes to consume */
        i = cnv->منزل_ULength;             /* restore # of bytes consumed */
        cnv->منزل_ULength = 0;

        ch = cnv->منزل_Unicode_حالت;/*Stores the previously calculated ch from a previous call*/
        cnv->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }

    طالما (my_سرچینه < سرچینه_حد && my_موخه < موخه_حد)
    {
        ch = *(my_سرچینه++);
        إذا (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(my_موخه++) = (char16_t) ch;
            *(myOffsets++) = دننه_واټن_عدد++;
        }
        وإلا
        {
            منزل_UBytes[0] = (حرف)ch;
            دننه_بایټونه = U8_شمېر_بایټونه_NON_ASCII(ch);
            i = 1;

morebytes:
            طالما (i < دننه_بایټونه)
            {
                إذا (my_سرچینه < سرچینه_حد)
                {
                    منزل_UBytes[i] = (حرف) (ch2 = *my_سرچینه);
                    إذا (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, دننه_بایټونه) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        اكسر; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++my_سرچینه;
                    i++;
                }
                وإلا
                {
                    cnv->منزل_Unicode_حالت = ch;
                    cnv->ډېرپېښ = دننه_بایټونه;
                    cnv->منزل_ULength = (int8_t)i;
                    اذهب donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            إذا (i == دننه_بایټونه && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[دننه_بایټونه];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                إذا (ch <= اعظمي_UCS2) 
                {
                    /* fits in 16 bits */
                    *(my_موخه++) = (char16_t) ch;
                    *(myOffsets++) = دننه_واټن_عدد;
                }
                وإلا
                {
                    /* write out the surrogates */
                    *(my_موخه++) = U16_LEAD(ch);
                    *(myOffsets++) = دننه_واټن_عدد;
                    ch = U16_TRAIL(ch);
                    إذا (my_موخه < موخه_حد)
                    {
                        *(my_موخه++) = (char16_t)ch;
                        *(myOffsets++) = دننه_واټن_عدد;
                    }
                    وإلا
                    {
                        cnv->UChar_Error_بفر[0] = (char16_t) ch;
                        cnv->UChar_Error_بفر_اوږدتیا = 1;
                        *err = U_بفر_ډکېدنه_ERROR;
                    }
                }
                دننه_واټن_عدد += i;
            }
            وإلا
            {
                cnv->منزل_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_وموندل;
                اكسر;
            }
        }
    }

donefornow:
    إذا (my_سرچینه < سرچینه_حد && my_موخه >= موخه_حد && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_بفر_ډکېدنه_ERROR;
    }

    دلیلونه_2->موخه = my_موخه;
    دلیلونه_2->سرچینه = (ثابت حرف *) my_سرچینه;
    دلیلونه_2->offsets = myOffsets;
}
U_CDECL_پای_ټکی

U_CFUNC فراغ  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverter_From_Unicode_دلیلونه * دلیلونه_2,
                                    UError_کوډ * err)
{
    UConverter *cnv = دلیلونه_2->converter;
    ثابت char16_t *my_سرچینه = دلیلونه_2->سرچینه;
    ثابت char16_t *سرچینه_حد = دلیلونه_2->سرچینه_حد;
    uint8_t *my_موخه = (uint8_t *) دلیلونه_2->موخه;
    ثابت uint8_t *موخه_حد = (uint8_t *) دلیلونه_2->موخه_حد;
    uint8_t *لنډمهاله_Ptr;
    UChar32 ch;
    uint8_t لنډمهاله_Buf[4];
    int32_t شاخص_منزل_Write;
    UBool isNotCESU8 = !has_CESU8_معلومات(cnv);

    إذا (cnv->fromUChar32 && my_موخه < موخه_حد)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        اذهب lowsurrogate;
    }

    طالما (my_سرچینه < سرچینه_حد && my_موخه < موخه_حد)
    {
        ch = *(my_سرچینه++);

        إذا (ch < 0x80)        /* Single byte */
        {
            *(my_موخه++) = (uint8_t) ch;
        }
        وإلا إذا (ch < 0x800)  /* Double byte */
        {
            *(my_موخه++) = (uint8_t) ((ch >> 6) | 0xc0);
            إذا (my_موخه < موخه_حد)
            {
                *(my_موخه++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            وإلا
            {
                cnv->char_Error_بفر[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_بفر_اوږدتیا = 1;
                *err = U_بفر_ډکېدنه_ERROR;
            }
        }
        وإلا {
            /* Check for surrogates */
            إذا(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                إذا (my_سرچینه < سرچینه_حد) {
                    /* test both code units */
                    إذا(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*my_سرچینه)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *my_سرچینه);
                        ++my_سرچینه;
                        /* exit this condition tree */
                    }
                    وإلا {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_وموندل;
                        اكسر;
                    }
                }
                وإلا {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    اكسر;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            لنډمهاله_Ptr = (((موخه_حد - my_موخه) >= 4) ? my_موخه : لنډمهاله_Buf);

            إذا (ch <= اعظمي_UCS2) {
                شاخص_منزل_Write = 2;
                لنډمهاله_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            وإلا {
                شاخص_منزل_Write = 3;
                لنډمهاله_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                لنډمهاله_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            لنډمهاله_Ptr[شاخص_منزل_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            لنډمهاله_Ptr[شاخص_منزل_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            إذا (لنډمهاله_Ptr == my_موخه) {
                /* There was enough space to write the codepoint directly. */
                my_موخه += (شاخص_منزل_Write + 1);
            }
            وإلا {
                /* We might run out of room soon. Write it slowly. */
                لكل (; لنډمهاله_Ptr <= (لنډمهاله_Buf + شاخص_منزل_Write); لنډمهاله_Ptr++) {
                    إذا (my_موخه < موخه_حد) {
                        *(my_موخه++) = *لنډمهاله_Ptr;
                    }
                    وإلا {
                        cnv->char_Error_بفر[cnv->char_Error_بفر_اوږدتیا++] = *لنډمهاله_Ptr;
                        *err = U_بفر_ډکېدنه_ERROR;
                    }
                }
            }
        }
    }

    إذا (my_سرچینه < سرچینه_حد && my_موخه >= موخه_حد && U_SUCCESS(*err))
    {
        *err = U_بفر_ډکېدنه_ERROR;
    }

    دلیلونه_2->موخه = (حرف *) my_موخه;
    دلیلونه_2->سرچینه = my_سرچینه;
}

U_CFUNC فراغ  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverter_From_Unicode_دلیلونه * دلیلونه_2,
                                                  UError_کوډ * err)
{
    UConverter *cnv = دلیلونه_2->converter;
    ثابت char16_t *my_سرچینه = دلیلونه_2->سرچینه;
    int32_t *myOffsets = دلیلونه_2->offsets;
    ثابت char16_t *سرچینه_حد = دلیلونه_2->سرچینه_حد;
    uint8_t *my_موخه = (uint8_t *) دلیلونه_2->موخه;
    ثابت uint8_t *موخه_حد = (uint8_t *) دلیلونه_2->موخه_حد;
    uint8_t *لنډمهاله_Ptr;
    UChar32 ch;
    int32_t دننه_واټن_عدد, بل_سرچینه_شاخص;
    int32_t شاخص_منزل_Write;
    uint8_t لنډمهاله_Buf[4];
    UBool isNotCESU8 = !has_CESU8_معلومات(cnv);

    إذا (cnv->fromUChar32 && my_موخه < موخه_حد)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        دننه_واټن_عدد = -1;
        بل_سرچینه_شاخص = 0;
        اذهب lowsurrogate;
    } وإلا {
        دننه_واټن_عدد = 0;
    }

    طالما (my_سرچینه < سرچینه_حد && my_موخه < موخه_حد)
    {
        ch = *(my_سرچینه++);

        إذا (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = دننه_واټن_عدد++;
            *(my_موخه++) = (حرف) ch;
        }
        وإلا إذا (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = دننه_واټن_عدد;
            *(my_موخه++) = (uint8_t) ((ch >> 6) | 0xc0);
            إذا (my_موخه < موخه_حد)
            {
                *(myOffsets++) = دننه_واټن_عدد++;
                *(my_موخه++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            وإلا
            {
                cnv->char_Error_بفر[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_بفر_اوږدتیا = 1;
                *err = U_بفر_ډکېدنه_ERROR;
            }
        }
        وإلا
        /* Check for surrogates */
        {
            بل_سرچینه_شاخص = دننه_واټن_عدد + 1;

            إذا(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                إذا (my_سرچینه < سرچینه_حد) {
                    /* test both code units */
                    إذا(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*my_سرچینه)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *my_سرچینه);
                        ++my_سرچینه;
                        ++بل_سرچینه_شاخص;
                        /* exit this condition tree */
                    }
                    وإلا {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_وموندل;
                        اكسر;
                    }
                }
                وإلا {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    اكسر;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            لنډمهاله_Ptr = (((موخه_حد - my_موخه) >= 4) ? my_موخه : لنډمهاله_Buf);

            إذا (ch <= اعظمي_UCS2) {
                شاخص_منزل_Write = 2;
                لنډمهاله_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            وإلا {
                شاخص_منزل_Write = 3;
                لنډمهاله_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                لنډمهاله_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            لنډمهاله_Ptr[شاخص_منزل_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            لنډمهاله_Ptr[شاخص_منزل_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            إذا (لنډمهاله_Ptr == my_موخه) {
                /* There was enough space to write the codepoint directly. */
                my_موخه += (شاخص_منزل_Write + 1);
                myOffsets[0] = دننه_واټن_عدد;
                myOffsets[1] = دننه_واټن_عدد;
                myOffsets[2] = دننه_واټن_عدد;
                إذا (شاخص_منزل_Write >= 3) {
                    myOffsets[3] = دننه_واټن_عدد;
                }
                myOffsets += (شاخص_منزل_Write + 1);
            }
            وإلا {
                /* We might run out of room soon. Write it slowly. */
                لكل (; لنډمهاله_Ptr <= (لنډمهاله_Buf + شاخص_منزل_Write); لنډمهاله_Ptr++) {
                    إذا (my_موخه < موخه_حد)
                    {
                        *(myOffsets++) = دننه_واټن_عدد;
                        *(my_موخه++) = *لنډمهاله_Ptr;
                    }
                    وإلا
                    {
                        cnv->char_Error_بفر[cnv->char_Error_بفر_اوږدتیا++] = *لنډمهاله_Ptr;
                        *err = U_بفر_ډکېدنه_ERROR;
                    }
                }
            }
            دننه_واټن_عدد = بل_سرچینه_شاخص;
        }
    }

    إذا (my_سرچینه < سرچینه_حد && my_موخه >= موخه_حد && U_SUCCESS(*err))
    {
        *err = U_بفر_ډکېدنه_ERROR;
    }

    دلیلونه_2->موخه = (حرف *) my_موخه;
    دلیلونه_2->سرچینه = my_سرچینه;
    دلیلونه_2->offsets = myOffsets;
}

U_CDECL_BEGIN
ساكن UChar32 U_CALLCONV ucnv_get_بل_UChar_UTF8(UConverter_منزل_Unicode_دلیلونه *دلیلونه_2,
                                               UError_کوډ *err) {
    UConverter *cnv;
    ثابت uint8_t *سرچینه_Initial;
    ثابت uint8_t *سرچینه;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = دلیلونه_2->converter;
    سرچینه_Initial = سرچینه = (ثابت uint8_t *)دلیلونه_2->سرچینه;
    إذا (سرچینه >= (ثابت uint8_t *)دلیلونه_2->سرچینه_حد)
    {
        /* no input */
        *err = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    myByte = *(سرچینه++);
    إذا (U8_IS_SINGLE(myByte))
    {
        دلیلونه_2->سرچینه = (ثابت حرف *)سرچینه;
        أرجع (UChar32)myByte;
    }

    uint16_t شمېر_Trail_بایټونه = U8_شمېر_TRAIL_بایټونه(myByte);
    إذا (شمېر_Trail_بایټونه == 0) {
        cnv->منزل_UBytes[0] = myByte;
        cnv->منزل_ULength = 1;
        *err = U_ILLEGAL_CHAR_وموندل;
        دلیلونه_2->سرچینه = (ثابت حرف *)سرچینه;
        أرجع 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    إذا (((ثابت حرف *)سرچینه + شمېر_Trail_بایټونه) > دلیلونه_2->سرچینه_حد)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extra_بایټونه_منزل_Write = شمېر_Trail_بایټونه + 1;
        cnv->منزل_UBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_وموندل;
        طالما(سرچینه < (ثابت uint8_t *)دلیلونه_2->سرچینه_حد) {
            uint8_t b = *سرچینه;
            إذا(icu::UTF8::isValidTrail(myByte, b, i, extra_بایټونه_منزل_Write)) {
                cnv->منزل_UBytes[i++] = b;
                ++سرچینه;
            } وإلا {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_وموندل;
                اكسر;
            }
        }
        cnv->منزل_ULength = i;
        دلیلونه_2->سرچینه = (ثابت حرف *)سرچینه;
        أرجع 0xffff;
    }

    ch = myByte << 6;
    إذا(شمېر_Trail_بایټونه == 2) {
        uint8_t t1 = *سرچینه, t2;
        إذا(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++سرچینه)) {
            دلیلونه_2->سرچینه = (ثابت حرف *)(سرچینه + 1);
            أرجع (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } وإلا إذا(شمېر_Trail_بایټونه == 1) {
        uint8_t t1 = *سرچینه;
        إذا(U8_IS_TRAIL(t1)) {
            دلیلونه_2->سرچینه = (ثابت حرف *)(سرچینه + 1);
            أرجع (ch + t1) - offsetsFromUTF8[2];
        }
    } وإلا {  // countTrailBytes == 3
        uint8_t t1 = *سرچینه, t2, t3;
        إذا(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++سرچینه) &&
                U8_IS_TRAIL(t3 = *++سرچینه)) {
            دلیلونه_2->سرچینه = (ثابت حرف *)(سرچینه + 1);
            أرجع (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    دلیلونه_2->سرچینه = (ثابت حرف *)سرچینه;

    لكل(i = 0; سرچینه_Initial < سرچینه; ++i) {
        cnv->منزل_UBytes[i] = *سرچینه_Initial++;
    }
    cnv->منزل_ULength = i;
    *err = U_ILLEGAL_CHAR_وموندل;
    أرجع 0xffff;
} 
U_CDECL_پای_ټکی

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
ساكن فراغ U_CALLCONV
ucnv_UTF8FromUTF8(UConverter_From_Unicode_دلیلونه *pFromUArgs,
                  UConverter_منزل_Unicode_دلیلونه *p_منزل_UArgs,
                  UError_کوډ *p_Error_کوډ) {
    UConverter *utf8;
    ثابت uint8_t *سرچینه, *سرچینه_حد;
    uint8_t *موخه;
    int32_t موخه_ظرفیت;
    int32_t count;

    int8_t old_منزل_ULength, منزل_ULength, منزل_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    /* set up the local pointers */
    utf8=p_منزل_UArgs->converter;
    سرچینه=(uint8_t *)p_منزل_UArgs->سرچینه;
    سرچینه_حد=(uint8_t *)p_منزل_UArgs->سرچینه_حد;
    موخه=(uint8_t *)pFromUArgs->موخه;
    موخه_ظرفیت=(int32_t)(pFromUArgs->موخه_حد-pFromUArgs->موخه);

    /* get the converter state from the UTF-8 UConverter */
    إذا(utf8->منزل_ULength > 0) {
        منزل_ULength=old_منزل_ULength=utf8->منزل_ULength;
        منزل_ULimit=(int8_t)utf8->ډېرپېښ;
        c=(UChar32)utf8->منزل_Unicode_حالت;
    } وإلا {
        منزل_ULength=old_منزل_ULength=منزل_ULimit=0;
        c = 0;
    }

    count=(int32_t)(سرچینه_حد-سرچینه)+old_منزل_ULength;
    إذا(count<منزل_ULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } وإلا إذا(موخه_ظرفیت<منزل_ULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *p_Error_کوډ=U_USING_DEFAULT_WARNING;
        أرجع;
    } وإلا {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        إذا(count>موخه_ظرفیت) {
            count=موخه_ظرفیت;
        }

        // The conversion loop checks count>0 only once per character.
        // If the buffer ends with a truncated sequence,
        // then we reduce the count to stop before that,
        // and collect the remaining bytes after the conversion loop.

        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length=count-منزل_ULength;
        U8_TRUNCATE_IF_INCOMPLETE(سرچینه, 0, length);
        count=منزل_ULength+length;
    }

    إذا(c!=0) {
        utf8->منزل_Unicode_حالت=0;
        utf8->منزل_ULength=0;
        اذهب نور_بایټونه;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    طالما(count>0) {
        b=*سرچینه++;
        إذا(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *موخه++=b;
            --count;
            استمر;
        } وإلا {
            إذا(b>=0xe0) {
                إذا( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=سرچینه[0]) &&
                    U8_IS_TRAIL(t2=سرچینه[1])
                ) {
                    سرچینه+=2;
                    *موخه++=b;
                    *موخه++=t1;
                    *موخه++=t2;
                    count-=3;
                    استمر;
                }
            } وإلا {
                إذا( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*سرچینه)
                ) {
                    ++سرچینه;
                    *موخه++=b;
                    *موخه++=t1;
                    count-=2;
                    استمر;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            old_منزل_ULength=0;
            منزل_ULength=1;
            منزل_ULimit=U8_شمېر_بایټونه_NON_ASCII(b);
            c=b;
نور_بایټونه:
            طالما(منزل_ULength<منزل_ULimit) {
                إذا(سرچینه<سرچینه_حد) {
                    b=*سرچینه;
                    إذا(icu::UTF8::isValidTrail(c, b, منزل_ULength, منزل_ULimit)) {
                        ++سرچینه;
                        ++منزل_ULength;
                        c=(c<<6)+b;
                    } وإلا {
                        اكسر; /* sequence too short, stop with toULength<toULimit */
                    }
                } وإلا {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    سرچینه-=(منزل_ULength-old_منزل_ULength);
                    طالما(old_منزل_ULength<منزل_ULength) {
                        utf8->منزل_UBytes[old_منزل_ULength++]=*سرچینه++;
                    }
                    utf8->منزل_Unicode_حالت=c;
                    utf8->منزل_ULength=منزل_ULength;
                    utf8->ډېرپېښ=منزل_ULimit;
                    p_منزل_UArgs->سرچینه=(حرف *)سرچینه;
                    pFromUArgs->موخه=(حرف *)موخه;
                    أرجع;
                }
            }

            إذا(منزل_ULength!=منزل_ULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                سرچینه-=(منزل_ULength-old_منزل_ULength);
                طالما(old_منزل_ULength<منزل_ULength) {
                    utf8->منزل_UBytes[old_منزل_ULength++]=*سرچینه++;
                }
                utf8->منزل_ULength=منزل_ULength;
                p_منزل_UArgs->سرچینه=(حرف *)سرچینه;
                pFromUArgs->موخه=(حرف *)موخه;
                *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                أرجع;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                لكل(i=0; i<old_منزل_ULength; ++i) {
                    *موخه++=utf8->منزل_UBytes[i];
                }
                سرچینه-=(منزل_ULength-old_منزل_ULength);
                لكل(; i<منزل_ULength; ++i) {
                    *موخه++=*سرچینه++;
                }
                count-=منزل_ULength;
            }
        }
    }
    U_ASSERT(count>=0);

    إذا(U_SUCCESS(*p_Error_کوډ) && سرچینه<سرچینه_حد) {
        إذا(موخه==(ثابت uint8_t *)pFromUArgs->موخه_حد) {
            *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
        } وإلا {
            b=*سرچینه;
            منزل_ULimit=U8_شمېر_بایټونه(b);
            إذا(منزل_ULimit>(سرچینه_حد-سرچینه)) {
                /* collect a truncated byte sequence */
                منزل_ULength=0;
                c=b;
                لكل(;;) {
                    utf8->منزل_UBytes[منزل_ULength++]=b;
                    إذا(++سرچینه==سرچینه_حد) {
                        /* partial byte sequence at end of source */
                        utf8->منزل_Unicode_حالت=c;
                        utf8->منزل_ULength=منزل_ULength;
                        utf8->ډېرپېښ=منزل_ULimit;
                        اكسر;
                    } وإلا إذا(!icu::UTF8::isValidTrail(c, b=*سرچینه, منزل_ULength, منزل_ULimit)) {
                        utf8->منزل_ULength=منزل_ULength;
                        *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                        اكسر;
                    }
                    c=(c<<6)+b;
                }
            } وإلا {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *p_Error_کوډ=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    p_منزل_UArgs->سرچینه=(حرف *)سرچینه;
    pFromUArgs->موخه=(حرف *)موخه;
}

U_CDECL_پای_ټکی

/* UTF-8 converter data ----------------------------------------------------- */

ساكن ثابت UConverterImpl _UTF8Impl={
    UCNV_UTF8,

    تش_اشاره,
    تش_اشاره,

    تش_اشاره,
    تش_اشاره,
    تش_اشاره,

    ucnv_منزل_Unicode_UTF8,
    ucnv_منزل_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    ucnv_get_بل_UChar_UTF8,

    تش_اشاره,
    تش_اشاره,
    تش_اشاره,
    تش_اشاره,
    ucnv_get_Non_Surrogate_Unicode_سېټ,

    ucnv_UTF8FromUTF8,
    ucnv_UTF8FromUTF8
};

/* The 1208 CCSID refers to any version of Unicode of UTF-8 */
ساكن ثابت UConverter_Static_معلومات UTF8_Static_معلومات={
    حجم(UConverter_Static_معلومات),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,خطأ,خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_معلومات UTF8_معلومات=
        UCNV_IMMUTABLE_SHARED_معلومات_INITIALIZER(&UTF8_Static_معلومات, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

ساكن ثابت UConverterImpl _CESU8Impl={
    UCNV_CESU8,

    تش_اشاره,
    تش_اشاره,

    تش_اشاره,
    تش_اشاره,
    تش_اشاره,

    ucnv_منزل_Unicode_UTF8,
    ucnv_منزل_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    تش_اشاره,

    تش_اشاره,
    تش_اشاره,
    تش_اشاره,
    تش_اشاره,
    ucnv_get_Complete_Unicode_سېټ,

    تش_اشاره,
    تش_اشاره
};

ساكن ثابت UConverter_Static_معلومات CESU8_Static_معلومات={
    حجم(UConverter_Static_معلومات),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,خطأ,خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_معلومات CESU8_معلومات=
        UCNV_IMMUTABLE_SHARED_معلومات_INITIALIZER(&CESU8_Static_معلومات, &_CESU8Impl);

#د_که_پای
