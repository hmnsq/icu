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

U_CFUNC فراغ ucnv_fromUnicode_UTF8(UConverter_From_Unicode_دلیلاں *دلیلاں_2,
                                           UError_کوڈ *err);
U_CFUNC فراغ ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverter_From_Unicode_دلیلاں *دلیلاں_2,
                                                        UError_کوڈ *err);


/* UTF-8 -------------------------------------------------------------------- */

#تعریف_کرو ودھ_توں_ودھ_UCS2            0x0000FFFF

ساكن ثابت uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

ساكن UBool has_CESU8_انکڑے(ثابت UConverter *cnv)
{
#إذا UCONFIG_ONLY_HTML_CONVERSION
    أرجع خطأ;
#وإلا
    أرجع cnv->shared_انکڑے == &CESU8_انکڑے;
#جے_انت
}
U_CDECL_BEGIN
ساكن فراغ  U_CALLCONV ucnv_منزل_Unicode_UTF8 (UConverter_منزل_Unicode_دلیلاں * دلیلاں_2,
                                  UError_کوڈ * err)
{
    UConverter *cnv = دلیلاں_2->converter;
    ثابت بنانشانی حرف *my_سروت = (بنانشانی حرف *) دلیلاں_2->سروت;
    char16_t *my_نشانہ = دلیلاں_2->نشانہ;
    ثابت بنانشانی حرف *سروت_حد_بندی = (بنانشانی حرف *) دلیلاں_2->سروت_حد_بندی;
    ثابت char16_t *نشانہ_حد_بندی = دلیلاں_2->نشانہ_حد_بندی;
    بنانشانی حرف *منزل_UBytes = cnv->منزل_UBytes;
    UBool isCESU8 = has_CESU8_انکڑے(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, وچ_بائٹاں;

    /* Restore size of current sequence */
    إذا (cnv->منزل_ULength > 0 && my_نشانہ < نشانہ_حد_بندی)
    {
        وچ_بائٹاں = cnv->منوال;            /* restore # of bytes to consume */
        i = cnv->منزل_ULength;             /* restore # of bytes consumed */
        cnv->منزل_ULength = 0;

        ch = cnv->منزل_Unicode_حالت;/*Stores the previously calculated ch from a previous call*/
        cnv->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }


    طالما (my_سروت < سروت_حد_بندی && my_نشانہ < نشانہ_حد_بندی)
    {
        ch = *(my_سروت++);
        إذا (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(my_نشانہ++) = (char16_t) ch;
        }
        وإلا
        {
            /* store the first char */
            منزل_UBytes[0] = (حرف)ch;
            وچ_بائٹاں = U8_گنتی_بائٹاں_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            طالما (i < وچ_بائٹاں)
            {
                إذا (my_سروت < سروت_حد_بندی)
                {
                    منزل_UBytes[i] = (حرف) (ch2 = *my_سروت);
                    إذا (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, وچ_بائٹاں) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        اكسر; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++my_سروت;
                    i++;
                }
                وإلا
                {
                    /* stores a partially calculated target*/
                    cnv->منزل_Unicode_حالت = ch;
                    cnv->منوال = وچ_بائٹاں;
                    cnv->منزل_ULength = (int8_t) i;
                    اذهب donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            إذا (i == وچ_بائٹاں && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[وچ_بائٹاں];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                إذا (ch <= ودھ_توں_ودھ_UCS2) 
                {
                    /* fits in 16 bits */
                    *(my_نشانہ++) = (char16_t) ch;
                }
                وإلا
                {
                    /* write out the surrogates */
                    *(my_نشانہ++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    إذا (my_نشانہ < نشانہ_حد_بندی)
                    {
                        *(my_نشانہ++) = (char16_t)ch;
                    }
                    وإلا
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UChar_Error_بفر[0] = (char16_t) ch;
                        cnv->UChar_Error_بفر_لمائی = 1;
                        *err = U_بفر_حد_توں_ودھ_ERROR;
                        اكسر;
                    }
                }
            }
            وإلا
            {
                cnv->منزل_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_لبھیا;
                اكسر;
            }
        }
    }

donefornow:
    إذا (my_سروت < سروت_حد_بندی && my_نشانہ >= نشانہ_حد_بندی && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_بفر_حد_توں_ودھ_ERROR;
    }

    دلیلاں_2->نشانہ = my_نشانہ;
    دلیلاں_2->سروت = (ثابت حرف *) my_سروت;
}

ساكن فراغ  U_CALLCONV ucnv_منزل_Unicode_UTF8_OFFSETS_LOGIC (UConverter_منزل_Unicode_دلیلاں * دلیلاں_2,
                                                UError_کوڈ * err)
{
    UConverter *cnv = دلیلاں_2->converter;
    ثابت بنانشانی حرف *my_سروت = (بنانشانی حرف *) دلیلاں_2->سروت;
    char16_t *my_نشانہ = دلیلاں_2->نشانہ;
    int32_t *myOffsets = دلیلاں_2->offsets;
    int32_t وتھ_عدد = 0;
    ثابت بنانشانی حرف *سروت_حد_بندی = (بنانشانی حرف *) دلیلاں_2->سروت_حد_بندی;
    ثابت char16_t *نشانہ_حد_بندی = دلیلاں_2->نشانہ_حد_بندی;
    بنانشانی حرف *منزل_UBytes = cnv->منزل_UBytes;
    UBool isCESU8 = has_CESU8_انکڑے(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, وچ_بائٹاں;

    /* Restore size of current sequence */
    إذا (cnv->منزل_ULength > 0 && my_نشانہ < نشانہ_حد_بندی)
    {
        وچ_بائٹاں = cnv->منوال;            /* restore # of bytes to consume */
        i = cnv->منزل_ULength;             /* restore # of bytes consumed */
        cnv->منزل_ULength = 0;

        ch = cnv->منزل_Unicode_حالت;/*Stores the previously calculated ch from a previous call*/
        cnv->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }

    طالما (my_سروت < سروت_حد_بندی && my_نشانہ < نشانہ_حد_بندی)
    {
        ch = *(my_سروت++);
        إذا (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(my_نشانہ++) = (char16_t) ch;
            *(myOffsets++) = وتھ_عدد++;
        }
        وإلا
        {
            منزل_UBytes[0] = (حرف)ch;
            وچ_بائٹاں = U8_گنتی_بائٹاں_NON_ASCII(ch);
            i = 1;

morebytes:
            طالما (i < وچ_بائٹاں)
            {
                إذا (my_سروت < سروت_حد_بندی)
                {
                    منزل_UBytes[i] = (حرف) (ch2 = *my_سروت);
                    إذا (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, وچ_بائٹاں) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        اكسر; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++my_سروت;
                    i++;
                }
                وإلا
                {
                    cnv->منزل_Unicode_حالت = ch;
                    cnv->منوال = وچ_بائٹاں;
                    cnv->منزل_ULength = (int8_t)i;
                    اذهب donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            إذا (i == وچ_بائٹاں && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[وچ_بائٹاں];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                إذا (ch <= ودھ_توں_ودھ_UCS2) 
                {
                    /* fits in 16 bits */
                    *(my_نشانہ++) = (char16_t) ch;
                    *(myOffsets++) = وتھ_عدد;
                }
                وإلا
                {
                    /* write out the surrogates */
                    *(my_نشانہ++) = U16_LEAD(ch);
                    *(myOffsets++) = وتھ_عدد;
                    ch = U16_TRAIL(ch);
                    إذا (my_نشانہ < نشانہ_حد_بندی)
                    {
                        *(my_نشانہ++) = (char16_t)ch;
                        *(myOffsets++) = وتھ_عدد;
                    }
                    وإلا
                    {
                        cnv->UChar_Error_بفر[0] = (char16_t) ch;
                        cnv->UChar_Error_بفر_لمائی = 1;
                        *err = U_بفر_حد_توں_ودھ_ERROR;
                    }
                }
                وتھ_عدد += i;
            }
            وإلا
            {
                cnv->منزل_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_لبھیا;
                اكسر;
            }
        }
    }

donefornow:
    إذا (my_سروت < سروت_حد_بندی && my_نشانہ >= نشانہ_حد_بندی && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_بفر_حد_توں_ودھ_ERROR;
    }

    دلیلاں_2->نشانہ = my_نشانہ;
    دلیلاں_2->سروت = (ثابت حرف *) my_سروت;
    دلیلاں_2->offsets = myOffsets;
}
U_CDECL_سرا

U_CFUNC فراغ  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverter_From_Unicode_دلیلاں * دلیلاں_2,
                                    UError_کوڈ * err)
{
    UConverter *cnv = دلیلاں_2->converter;
    ثابت char16_t *my_سروت = دلیلاں_2->سروت;
    ثابت char16_t *سروت_حد_بندی = دلیلاں_2->سروت_حد_بندی;
    uint8_t *my_نشانہ = (uint8_t *) دلیلاں_2->نشانہ;
    ثابت uint8_t *نشانہ_حد_بندی = (uint8_t *) دلیلاں_2->نشانہ_حد_بندی;
    uint8_t *عارضی_Ptr;
    UChar32 ch;
    uint8_t عارضی_Buf[4];
    int32_t اشاریہ_منزل_Write;
    UBool isNotCESU8 = !has_CESU8_انکڑے(cnv);

    إذا (cnv->fromUChar32 && my_نشانہ < نشانہ_حد_بندی)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        اذهب lowsurrogate;
    }

    طالما (my_سروت < سروت_حد_بندی && my_نشانہ < نشانہ_حد_بندی)
    {
        ch = *(my_سروت++);

        إذا (ch < 0x80)        /* Single byte */
        {
            *(my_نشانہ++) = (uint8_t) ch;
        }
        وإلا إذا (ch < 0x800)  /* Double byte */
        {
            *(my_نشانہ++) = (uint8_t) ((ch >> 6) | 0xc0);
            إذا (my_نشانہ < نشانہ_حد_بندی)
            {
                *(my_نشانہ++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            وإلا
            {
                cnv->char_Error_بفر[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_بفر_لمائی = 1;
                *err = U_بفر_حد_توں_ودھ_ERROR;
            }
        }
        وإلا {
            /* Check for surrogates */
            إذا(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                إذا (my_سروت < سروت_حد_بندی) {
                    /* test both code units */
                    إذا(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*my_سروت)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *my_سروت);
                        ++my_سروت;
                        /* exit this condition tree */
                    }
                    وإلا {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_لبھیا;
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
            عارضی_Ptr = (((نشانہ_حد_بندی - my_نشانہ) >= 4) ? my_نشانہ : عارضی_Buf);

            إذا (ch <= ودھ_توں_ودھ_UCS2) {
                اشاریہ_منزل_Write = 2;
                عارضی_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            وإلا {
                اشاریہ_منزل_Write = 3;
                عارضی_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                عارضی_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            عارضی_Ptr[اشاریہ_منزل_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            عارضی_Ptr[اشاریہ_منزل_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            إذا (عارضی_Ptr == my_نشانہ) {
                /* There was enough space to write the codepoint directly. */
                my_نشانہ += (اشاریہ_منزل_Write + 1);
            }
            وإلا {
                /* We might run out of room soon. Write it slowly. */
                لكل (; عارضی_Ptr <= (عارضی_Buf + اشاریہ_منزل_Write); عارضی_Ptr++) {
                    إذا (my_نشانہ < نشانہ_حد_بندی) {
                        *(my_نشانہ++) = *عارضی_Ptr;
                    }
                    وإلا {
                        cnv->char_Error_بفر[cnv->char_Error_بفر_لمائی++] = *عارضی_Ptr;
                        *err = U_بفر_حد_توں_ودھ_ERROR;
                    }
                }
            }
        }
    }

    إذا (my_سروت < سروت_حد_بندی && my_نشانہ >= نشانہ_حد_بندی && U_SUCCESS(*err))
    {
        *err = U_بفر_حد_توں_ودھ_ERROR;
    }

    دلیلاں_2->نشانہ = (حرف *) my_نشانہ;
    دلیلاں_2->سروت = my_سروت;
}

U_CFUNC فراغ  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverter_From_Unicode_دلیلاں * دلیلاں_2,
                                                  UError_کوڈ * err)
{
    UConverter *cnv = دلیلاں_2->converter;
    ثابت char16_t *my_سروت = دلیلاں_2->سروت;
    int32_t *myOffsets = دلیلاں_2->offsets;
    ثابت char16_t *سروت_حد_بندی = دلیلاں_2->سروت_حد_بندی;
    uint8_t *my_نشانہ = (uint8_t *) دلیلاں_2->نشانہ;
    ثابت uint8_t *نشانہ_حد_بندی = (uint8_t *) دلیلاں_2->نشانہ_حد_بندی;
    uint8_t *عارضی_Ptr;
    UChar32 ch;
    int32_t وتھ_عدد, اگلا_سروت_اشاریہ;
    int32_t اشاریہ_منزل_Write;
    uint8_t عارضی_Buf[4];
    UBool isNotCESU8 = !has_CESU8_انکڑے(cnv);

    إذا (cnv->fromUChar32 && my_نشانہ < نشانہ_حد_بندی)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        وتھ_عدد = -1;
        اگلا_سروت_اشاریہ = 0;
        اذهب lowsurrogate;
    } وإلا {
        وتھ_عدد = 0;
    }

    طالما (my_سروت < سروت_حد_بندی && my_نشانہ < نشانہ_حد_بندی)
    {
        ch = *(my_سروت++);

        إذا (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = وتھ_عدد++;
            *(my_نشانہ++) = (حرف) ch;
        }
        وإلا إذا (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = وتھ_عدد;
            *(my_نشانہ++) = (uint8_t) ((ch >> 6) | 0xc0);
            إذا (my_نشانہ < نشانہ_حد_بندی)
            {
                *(myOffsets++) = وتھ_عدد++;
                *(my_نشانہ++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            وإلا
            {
                cnv->char_Error_بفر[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_بفر_لمائی = 1;
                *err = U_بفر_حد_توں_ودھ_ERROR;
            }
        }
        وإلا
        /* Check for surrogates */
        {
            اگلا_سروت_اشاریہ = وتھ_عدد + 1;

            إذا(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                إذا (my_سروت < سروت_حد_بندی) {
                    /* test both code units */
                    إذا(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*my_سروت)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *my_سروت);
                        ++my_سروت;
                        ++اگلا_سروت_اشاریہ;
                        /* exit this condition tree */
                    }
                    وإلا {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_لبھیا;
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
            عارضی_Ptr = (((نشانہ_حد_بندی - my_نشانہ) >= 4) ? my_نشانہ : عارضی_Buf);

            إذا (ch <= ودھ_توں_ودھ_UCS2) {
                اشاریہ_منزل_Write = 2;
                عارضی_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            وإلا {
                اشاریہ_منزل_Write = 3;
                عارضی_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                عارضی_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            عارضی_Ptr[اشاریہ_منزل_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            عارضی_Ptr[اشاریہ_منزل_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            إذا (عارضی_Ptr == my_نشانہ) {
                /* There was enough space to write the codepoint directly. */
                my_نشانہ += (اشاریہ_منزل_Write + 1);
                myOffsets[0] = وتھ_عدد;
                myOffsets[1] = وتھ_عدد;
                myOffsets[2] = وتھ_عدد;
                إذا (اشاریہ_منزل_Write >= 3) {
                    myOffsets[3] = وتھ_عدد;
                }
                myOffsets += (اشاریہ_منزل_Write + 1);
            }
            وإلا {
                /* We might run out of room soon. Write it slowly. */
                لكل (; عارضی_Ptr <= (عارضی_Buf + اشاریہ_منزل_Write); عارضی_Ptr++) {
                    إذا (my_نشانہ < نشانہ_حد_بندی)
                    {
                        *(myOffsets++) = وتھ_عدد;
                        *(my_نشانہ++) = *عارضی_Ptr;
                    }
                    وإلا
                    {
                        cnv->char_Error_بفر[cnv->char_Error_بفر_لمائی++] = *عارضی_Ptr;
                        *err = U_بفر_حد_توں_ودھ_ERROR;
                    }
                }
            }
            وتھ_عدد = اگلا_سروت_اشاریہ;
        }
    }

    إذا (my_سروت < سروت_حد_بندی && my_نشانہ >= نشانہ_حد_بندی && U_SUCCESS(*err))
    {
        *err = U_بفر_حد_توں_ودھ_ERROR;
    }

    دلیلاں_2->نشانہ = (حرف *) my_نشانہ;
    دلیلاں_2->سروت = my_سروت;
    دلیلاں_2->offsets = myOffsets;
}

U_CDECL_BEGIN
ساكن UChar32 U_CALLCONV ucnv_get_اگلا_UChar_UTF8(UConverter_منزل_Unicode_دلیلاں *دلیلاں_2,
                                               UError_کوڈ *err) {
    UConverter *cnv;
    ثابت uint8_t *سروت_Initial;
    ثابت uint8_t *سروت;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = دلیلاں_2->converter;
    سروت_Initial = سروت = (ثابت uint8_t *)دلیلاں_2->سروت;
    إذا (سروت >= (ثابت uint8_t *)دلیلاں_2->سروت_حد_بندی)
    {
        /* no input */
        *err = U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    myByte = *(سروت++);
    إذا (U8_IS_SINGLE(myByte))
    {
        دلیلاں_2->سروت = (ثابت حرف *)سروت;
        أرجع (UChar32)myByte;
    }

    uint16_t گنتی_Trail_بائٹاں = U8_گنتی_TRAIL_بائٹاں(myByte);
    إذا (گنتی_Trail_بائٹاں == 0) {
        cnv->منزل_UBytes[0] = myByte;
        cnv->منزل_ULength = 1;
        *err = U_ILLEGAL_CHAR_لبھیا;
        دلیلاں_2->سروت = (ثابت حرف *)سروت;
        أرجع 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    إذا (((ثابت حرف *)سروت + گنتی_Trail_بائٹاں) > دلیلاں_2->سروت_حد_بندی)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extra_بائٹاں_منزل_Write = گنتی_Trail_بائٹاں + 1;
        cnv->منزل_UBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_لبھیا;
        طالما(سروت < (ثابت uint8_t *)دلیلاں_2->سروت_حد_بندی) {
            uint8_t b = *سروت;
            إذا(icu::UTF8::isValidTrail(myByte, b, i, extra_بائٹاں_منزل_Write)) {
                cnv->منزل_UBytes[i++] = b;
                ++سروت;
            } وإلا {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_لبھیا;
                اكسر;
            }
        }
        cnv->منزل_ULength = i;
        دلیلاں_2->سروت = (ثابت حرف *)سروت;
        أرجع 0xffff;
    }

    ch = myByte << 6;
    إذا(گنتی_Trail_بائٹاں == 2) {
        uint8_t t1 = *سروت, t2;
        إذا(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++سروت)) {
            دلیلاں_2->سروت = (ثابت حرف *)(سروت + 1);
            أرجع (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } وإلا إذا(گنتی_Trail_بائٹاں == 1) {
        uint8_t t1 = *سروت;
        إذا(U8_IS_TRAIL(t1)) {
            دلیلاں_2->سروت = (ثابت حرف *)(سروت + 1);
            أرجع (ch + t1) - offsetsFromUTF8[2];
        }
    } وإلا {  // countTrailBytes == 3
        uint8_t t1 = *سروت, t2, t3;
        إذا(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++سروت) &&
                U8_IS_TRAIL(t3 = *++سروت)) {
            دلیلاں_2->سروت = (ثابت حرف *)(سروت + 1);
            أرجع (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    دلیلاں_2->سروت = (ثابت حرف *)سروت;

    لكل(i = 0; سروت_Initial < سروت; ++i) {
        cnv->منزل_UBytes[i] = *سروت_Initial++;
    }
    cnv->منزل_ULength = i;
    *err = U_ILLEGAL_CHAR_لبھیا;
    أرجع 0xffff;
} 
U_CDECL_سرا

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
ساكن فراغ U_CALLCONV
ucnv_UTF8FromUTF8(UConverter_From_Unicode_دلیلاں *pFromUArgs,
                  UConverter_منزل_Unicode_دلیلاں *p_منزل_UArgs,
                  UError_کوڈ *p_Error_کوڈ) {
    UConverter *utf8;
    ثابت uint8_t *سروت, *سروت_حد_بندی;
    uint8_t *نشانہ;
    int32_t نشانہ_سمرتھا;
    int32_t count;

    int8_t old_منزل_ULength, منزل_ULength, منزل_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    /* set up the local pointers */
    utf8=p_منزل_UArgs->converter;
    سروت=(uint8_t *)p_منزل_UArgs->سروت;
    سروت_حد_بندی=(uint8_t *)p_منزل_UArgs->سروت_حد_بندی;
    نشانہ=(uint8_t *)pFromUArgs->نشانہ;
    نشانہ_سمرتھا=(int32_t)(pFromUArgs->نشانہ_حد_بندی-pFromUArgs->نشانہ);

    /* get the converter state from the UTF-8 UConverter */
    إذا(utf8->منزل_ULength > 0) {
        منزل_ULength=old_منزل_ULength=utf8->منزل_ULength;
        منزل_ULimit=(int8_t)utf8->منوال;
        c=(UChar32)utf8->منزل_Unicode_حالت;
    } وإلا {
        منزل_ULength=old_منزل_ULength=منزل_ULimit=0;
        c = 0;
    }

    count=(int32_t)(سروت_حد_بندی-سروت)+old_منزل_ULength;
    إذا(count<منزل_ULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } وإلا إذا(نشانہ_سمرتھا<منزل_ULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *p_Error_کوڈ=U_USING_DEFAULT_WARNING;
        أرجع;
    } وإلا {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        إذا(count>نشانہ_سمرتھا) {
            count=نشانہ_سمرتھا;
        }

        // The conversion loop checks count>0 only once per character.
        // If the buffer ends with a truncated sequence,
        // then we reduce the count to stop before that,
        // and collect the remaining bytes after the conversion loop.

        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length=count-منزل_ULength;
        U8_TRUNCATE_IF_INCOMPLETE(سروت, 0, length);
        count=منزل_ULength+length;
    }

    إذا(c!=0) {
        utf8->منزل_Unicode_حالت=0;
        utf8->منزل_ULength=0;
        اذهب ہور_بائٹاں;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    طالما(count>0) {
        b=*سروت++;
        إذا(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *نشانہ++=b;
            --count;
            استمر;
        } وإلا {
            إذا(b>=0xe0) {
                إذا( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=سروت[0]) &&
                    U8_IS_TRAIL(t2=سروت[1])
                ) {
                    سروت+=2;
                    *نشانہ++=b;
                    *نشانہ++=t1;
                    *نشانہ++=t2;
                    count-=3;
                    استمر;
                }
            } وإلا {
                إذا( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*سروت)
                ) {
                    ++سروت;
                    *نشانہ++=b;
                    *نشانہ++=t1;
                    count-=2;
                    استمر;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            old_منزل_ULength=0;
            منزل_ULength=1;
            منزل_ULimit=U8_گنتی_بائٹاں_NON_ASCII(b);
            c=b;
ہور_بائٹاں:
            طالما(منزل_ULength<منزل_ULimit) {
                إذا(سروت<سروت_حد_بندی) {
                    b=*سروت;
                    إذا(icu::UTF8::isValidTrail(c, b, منزل_ULength, منزل_ULimit)) {
                        ++سروت;
                        ++منزل_ULength;
                        c=(c<<6)+b;
                    } وإلا {
                        اكسر; /* sequence too short, stop with toULength<toULimit */
                    }
                } وإلا {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    سروت-=(منزل_ULength-old_منزل_ULength);
                    طالما(old_منزل_ULength<منزل_ULength) {
                        utf8->منزل_UBytes[old_منزل_ULength++]=*سروت++;
                    }
                    utf8->منزل_Unicode_حالت=c;
                    utf8->منزل_ULength=منزل_ULength;
                    utf8->منوال=منزل_ULimit;
                    p_منزل_UArgs->سروت=(حرف *)سروت;
                    pFromUArgs->نشانہ=(حرف *)نشانہ;
                    أرجع;
                }
            }

            إذا(منزل_ULength!=منزل_ULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                سروت-=(منزل_ULength-old_منزل_ULength);
                طالما(old_منزل_ULength<منزل_ULength) {
                    utf8->منزل_UBytes[old_منزل_ULength++]=*سروت++;
                }
                utf8->منزل_ULength=منزل_ULength;
                p_منزل_UArgs->سروت=(حرف *)سروت;
                pFromUArgs->نشانہ=(حرف *)نشانہ;
                *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
                أرجع;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                لكل(i=0; i<old_منزل_ULength; ++i) {
                    *نشانہ++=utf8->منزل_UBytes[i];
                }
                سروت-=(منزل_ULength-old_منزل_ULength);
                لكل(; i<منزل_ULength; ++i) {
                    *نشانہ++=*سروت++;
                }
                count-=منزل_ULength;
            }
        }
    }
    U_ASSERT(count>=0);

    إذا(U_SUCCESS(*p_Error_کوڈ) && سروت<سروت_حد_بندی) {
        إذا(نشانہ==(ثابت uint8_t *)pFromUArgs->نشانہ_حد_بندی) {
            *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
        } وإلا {
            b=*سروت;
            منزل_ULimit=U8_گنتی_بائٹاں(b);
            إذا(منزل_ULimit>(سروت_حد_بندی-سروت)) {
                /* collect a truncated byte sequence */
                منزل_ULength=0;
                c=b;
                لكل(;;) {
                    utf8->منزل_UBytes[منزل_ULength++]=b;
                    إذا(++سروت==سروت_حد_بندی) {
                        /* partial byte sequence at end of source */
                        utf8->منزل_Unicode_حالت=c;
                        utf8->منزل_ULength=منزل_ULength;
                        utf8->منوال=منزل_ULimit;
                        اكسر;
                    } وإلا إذا(!icu::UTF8::isValidTrail(c, b=*سروت, منزل_ULength, منزل_ULimit)) {
                        utf8->منزل_ULength=منزل_ULength;
                        *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
                        اكسر;
                    }
                    c=(c<<6)+b;
                }
            } وإلا {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *p_Error_کوڈ=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    p_منزل_UArgs->سروت=(حرف *)سروت;
    pFromUArgs->نشانہ=(حرف *)نشانہ;
}

U_CDECL_سرا

/* UTF-8 converter data ----------------------------------------------------- */

ساكن ثابت UConverterImpl _UTF8Impl={
    UCNV_UTF8,

    خالی_سوچک,
    خالی_سوچک,

    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,

    ucnv_منزل_Unicode_UTF8,
    ucnv_منزل_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    ucnv_get_اگلا_UChar_UTF8,

    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,
    ucnv_get_Non_Surrogate_Unicode_سیٹ,

    ucnv_UTF8FromUTF8,
    ucnv_UTF8FromUTF8
};

/* The 1208 CCSID refers to any version of Unicode of UTF-8 */
ساكن ثابت UConverter_Static_انکڑے UTF8_Static_انکڑے={
    حجم(UConverter_Static_انکڑے),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,خطأ,خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_انکڑے UTF8_انکڑے=
        UCNV_IMMUTABLE_SHARED_انکڑے_INITIALIZER(&UTF8_Static_انکڑے, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

ساكن ثابت UConverterImpl _CESU8Impl={
    UCNV_CESU8,

    خالی_سوچک,
    خالی_سوچک,

    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,

    ucnv_منزل_Unicode_UTF8,
    ucnv_منزل_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    خالی_سوچک,

    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,
    ucnv_get_Complete_Unicode_سیٹ,

    خالی_سوچک,
    خالی_سوچک
};

ساكن ثابت UConverter_Static_انکڑے CESU8_Static_انکڑے={
    حجم(UConverter_Static_انکڑے),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,خطأ,خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_انکڑے CESU8_انکڑے=
        UCNV_IMMUTABLE_SHARED_انکڑے_INITIALIZER(&CESU8_Static_انکڑے, &_CESU8Impl);

#جے_انت
