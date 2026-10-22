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

U_CFUNC فراغ ucnv_fromUnicode_UTF8(UConverter_From_Unicode_دليلون *دليلون_2,
                                           UError_ڪوڊ *err);
U_CFUNC فراغ ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverter_From_Unicode_دليلون *دليلون_2,
                                                        UError_ڪوڊ *err);


/* UTF-8 -------------------------------------------------------------------- */

#وصف_ڪريو وڌ_کان_وڌ_UCS2            0x0000FFFF

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
#جيڪڏهن_پورو
}
U_CDECL_BEGIN
ساكن فراغ  U_CALLCONV ucnv_منزل_Unicode_UTF8 (UConverter_منزل_Unicode_دليلون * دليلون_2,
                                  UError_ڪوڊ * err)
{
    UConverter *cnv = دليلون_2->converter;
    ثابت بنانشان حرف *my_ذريعو = (بنانشان حرف *) دليلون_2->ذريعو;
    char16_t *my_هدف = دليلون_2->هدف;
    ثابت بنانشان حرف *ذريعو_حد_بندي = (بنانشان حرف *) دليلون_2->ذريعو_حد_بندي;
    ثابت char16_t *هدف_حد_بندي = دليلون_2->هدف_حد_بندي;
    بنانشان حرف *منزل_UBytes = cnv->منزل_UBytes;
    UBool isCESU8 = has_CESU8_معلومات(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, اندريون_بائيٽون;

    /* Restore size of current sequence */
    إذا (cnv->منزل_ULength > 0 && my_هدف < هدف_حد_بندي)
    {
        اندريون_بائيٽون = cnv->گهڻائي;            /* restore # of bytes to consume */
        i = cnv->منزل_ULength;             /* restore # of bytes consumed */
        cnv->منزل_ULength = 0;

        ch = cnv->منزل_Unicode_حالت;/*Stores the previously calculated ch from a previous call*/
        cnv->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }


    طالما (my_ذريعو < ذريعو_حد_بندي && my_هدف < هدف_حد_بندي)
    {
        ch = *(my_ذريعو++);
        إذا (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(my_هدف++) = (char16_t) ch;
        }
        وإلا
        {
            /* store the first char */
            منزل_UBytes[0] = (حرف)ch;
            اندريون_بائيٽون = U8_ڳڻپ_بائيٽون_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            طالما (i < اندريون_بائيٽون)
            {
                إذا (my_ذريعو < ذريعو_حد_بندي)
                {
                    منزل_UBytes[i] = (حرف) (ch2 = *my_ذريعو);
                    إذا (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, اندريون_بائيٽون) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        اكسر; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++my_ذريعو;
                    i++;
                }
                وإلا
                {
                    /* stores a partially calculated target*/
                    cnv->منزل_Unicode_حالت = ch;
                    cnv->گهڻائي = اندريون_بائيٽون;
                    cnv->منزل_ULength = (int8_t) i;
                    اذهب donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            إذا (i == اندريون_بائيٽون && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[اندريون_بائيٽون];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                إذا (ch <= وڌ_کان_وڌ_UCS2) 
                {
                    /* fits in 16 bits */
                    *(my_هدف++) = (char16_t) ch;
                }
                وإلا
                {
                    /* write out the surrogates */
                    *(my_هدف++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    إذا (my_هدف < هدف_حد_بندي)
                    {
                        *(my_هدف++) = (char16_t)ch;
                    }
                    وإلا
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UChar_Error_بفر[0] = (char16_t) ch;
                        cnv->UChar_Error_بفر_ڊيگهائي = 1;
                        *err = U_بفر_حد_کان_مٿي_ERROR;
                        اكسر;
                    }
                }
            }
            وإلا
            {
                cnv->منزل_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_مليو;
                اكسر;
            }
        }
    }

donefornow:
    إذا (my_ذريعو < ذريعو_حد_بندي && my_هدف >= هدف_حد_بندي && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_بفر_حد_کان_مٿي_ERROR;
    }

    دليلون_2->هدف = my_هدف;
    دليلون_2->ذريعو = (ثابت حرف *) my_ذريعو;
}

ساكن فراغ  U_CALLCONV ucnv_منزل_Unicode_UTF8_OFFSETS_LOGIC (UConverter_منزل_Unicode_دليلون * دليلون_2,
                                                UError_ڪوڊ * err)
{
    UConverter *cnv = دليلون_2->converter;
    ثابت بنانشان حرف *my_ذريعو = (بنانشان حرف *) دليلون_2->ذريعو;
    char16_t *my_هدف = دليلون_2->هدف;
    int32_t *myOffsets = دليلون_2->offsets;
    int32_t وٿي_انگ = 0;
    ثابت بنانشان حرف *ذريعو_حد_بندي = (بنانشان حرف *) دليلون_2->ذريعو_حد_بندي;
    ثابت char16_t *هدف_حد_بندي = دليلون_2->هدف_حد_بندي;
    بنانشان حرف *منزل_UBytes = cnv->منزل_UBytes;
    UBool isCESU8 = has_CESU8_معلومات(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, اندريون_بائيٽون;

    /* Restore size of current sequence */
    إذا (cnv->منزل_ULength > 0 && my_هدف < هدف_حد_بندي)
    {
        اندريون_بائيٽون = cnv->گهڻائي;            /* restore # of bytes to consume */
        i = cnv->منزل_ULength;             /* restore # of bytes consumed */
        cnv->منزل_ULength = 0;

        ch = cnv->منزل_Unicode_حالت;/*Stores the previously calculated ch from a previous call*/
        cnv->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }

    طالما (my_ذريعو < ذريعو_حد_بندي && my_هدف < هدف_حد_بندي)
    {
        ch = *(my_ذريعو++);
        إذا (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(my_هدف++) = (char16_t) ch;
            *(myOffsets++) = وٿي_انگ++;
        }
        وإلا
        {
            منزل_UBytes[0] = (حرف)ch;
            اندريون_بائيٽون = U8_ڳڻپ_بائيٽون_NON_ASCII(ch);
            i = 1;

morebytes:
            طالما (i < اندريون_بائيٽون)
            {
                إذا (my_ذريعو < ذريعو_حد_بندي)
                {
                    منزل_UBytes[i] = (حرف) (ch2 = *my_ذريعو);
                    إذا (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, اندريون_بائيٽون) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        اكسر; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++my_ذريعو;
                    i++;
                }
                وإلا
                {
                    cnv->منزل_Unicode_حالت = ch;
                    cnv->گهڻائي = اندريون_بائيٽون;
                    cnv->منزل_ULength = (int8_t)i;
                    اذهب donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            إذا (i == اندريون_بائيٽون && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[اندريون_بائيٽون];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                إذا (ch <= وڌ_کان_وڌ_UCS2) 
                {
                    /* fits in 16 bits */
                    *(my_هدف++) = (char16_t) ch;
                    *(myOffsets++) = وٿي_انگ;
                }
                وإلا
                {
                    /* write out the surrogates */
                    *(my_هدف++) = U16_LEAD(ch);
                    *(myOffsets++) = وٿي_انگ;
                    ch = U16_TRAIL(ch);
                    إذا (my_هدف < هدف_حد_بندي)
                    {
                        *(my_هدف++) = (char16_t)ch;
                        *(myOffsets++) = وٿي_انگ;
                    }
                    وإلا
                    {
                        cnv->UChar_Error_بفر[0] = (char16_t) ch;
                        cnv->UChar_Error_بفر_ڊيگهائي = 1;
                        *err = U_بفر_حد_کان_مٿي_ERROR;
                    }
                }
                وٿي_انگ += i;
            }
            وإلا
            {
                cnv->منزل_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_مليو;
                اكسر;
            }
        }
    }

donefornow:
    إذا (my_ذريعو < ذريعو_حد_بندي && my_هدف >= هدف_حد_بندي && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_بفر_حد_کان_مٿي_ERROR;
    }

    دليلون_2->هدف = my_هدف;
    دليلون_2->ذريعو = (ثابت حرف *) my_ذريعو;
    دليلون_2->offsets = myOffsets;
}
U_CDECL_پڇاڙي

U_CFUNC فراغ  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverter_From_Unicode_دليلون * دليلون_2,
                                    UError_ڪوڊ * err)
{
    UConverter *cnv = دليلون_2->converter;
    ثابت char16_t *my_ذريعو = دليلون_2->ذريعو;
    ثابت char16_t *ذريعو_حد_بندي = دليلون_2->ذريعو_حد_بندي;
    uint8_t *my_هدف = (uint8_t *) دليلون_2->هدف;
    ثابت uint8_t *هدف_حد_بندي = (uint8_t *) دليلون_2->هدف_حد_بندي;
    uint8_t *عارضي_Ptr;
    UChar32 ch;
    uint8_t عارضي_Buf[4];
    int32_t اشاريو_منزل_Write;
    UBool isNotCESU8 = !has_CESU8_معلومات(cnv);

    إذا (cnv->fromUChar32 && my_هدف < هدف_حد_بندي)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        اذهب lowsurrogate;
    }

    طالما (my_ذريعو < ذريعو_حد_بندي && my_هدف < هدف_حد_بندي)
    {
        ch = *(my_ذريعو++);

        إذا (ch < 0x80)        /* Single byte */
        {
            *(my_هدف++) = (uint8_t) ch;
        }
        وإلا إذا (ch < 0x800)  /* Double byte */
        {
            *(my_هدف++) = (uint8_t) ((ch >> 6) | 0xc0);
            إذا (my_هدف < هدف_حد_بندي)
            {
                *(my_هدف++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            وإلا
            {
                cnv->char_Error_بفر[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_بفر_ڊيگهائي = 1;
                *err = U_بفر_حد_کان_مٿي_ERROR;
            }
        }
        وإلا {
            /* Check for surrogates */
            إذا(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                إذا (my_ذريعو < ذريعو_حد_بندي) {
                    /* test both code units */
                    إذا(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*my_ذريعو)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *my_ذريعو);
                        ++my_ذريعو;
                        /* exit this condition tree */
                    }
                    وإلا {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_مليو;
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
            عارضي_Ptr = (((هدف_حد_بندي - my_هدف) >= 4) ? my_هدف : عارضي_Buf);

            إذا (ch <= وڌ_کان_وڌ_UCS2) {
                اشاريو_منزل_Write = 2;
                عارضي_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            وإلا {
                اشاريو_منزل_Write = 3;
                عارضي_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                عارضي_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            عارضي_Ptr[اشاريو_منزل_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            عارضي_Ptr[اشاريو_منزل_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            إذا (عارضي_Ptr == my_هدف) {
                /* There was enough space to write the codepoint directly. */
                my_هدف += (اشاريو_منزل_Write + 1);
            }
            وإلا {
                /* We might run out of room soon. Write it slowly. */
                لكل (; عارضي_Ptr <= (عارضي_Buf + اشاريو_منزل_Write); عارضي_Ptr++) {
                    إذا (my_هدف < هدف_حد_بندي) {
                        *(my_هدف++) = *عارضي_Ptr;
                    }
                    وإلا {
                        cnv->char_Error_بفر[cnv->char_Error_بفر_ڊيگهائي++] = *عارضي_Ptr;
                        *err = U_بفر_حد_کان_مٿي_ERROR;
                    }
                }
            }
        }
    }

    إذا (my_ذريعو < ذريعو_حد_بندي && my_هدف >= هدف_حد_بندي && U_SUCCESS(*err))
    {
        *err = U_بفر_حد_کان_مٿي_ERROR;
    }

    دليلون_2->هدف = (حرف *) my_هدف;
    دليلون_2->ذريعو = my_ذريعو;
}

U_CFUNC فراغ  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverter_From_Unicode_دليلون * دليلون_2,
                                                  UError_ڪوڊ * err)
{
    UConverter *cnv = دليلون_2->converter;
    ثابت char16_t *my_ذريعو = دليلون_2->ذريعو;
    int32_t *myOffsets = دليلون_2->offsets;
    ثابت char16_t *ذريعو_حد_بندي = دليلون_2->ذريعو_حد_بندي;
    uint8_t *my_هدف = (uint8_t *) دليلون_2->هدف;
    ثابت uint8_t *هدف_حد_بندي = (uint8_t *) دليلون_2->هدف_حد_بندي;
    uint8_t *عارضي_Ptr;
    UChar32 ch;
    int32_t وٿي_انگ, ايندڙ_ذريعو_اشاريو;
    int32_t اشاريو_منزل_Write;
    uint8_t عارضي_Buf[4];
    UBool isNotCESU8 = !has_CESU8_معلومات(cnv);

    إذا (cnv->fromUChar32 && my_هدف < هدف_حد_بندي)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        وٿي_انگ = -1;
        ايندڙ_ذريعو_اشاريو = 0;
        اذهب lowsurrogate;
    } وإلا {
        وٿي_انگ = 0;
    }

    طالما (my_ذريعو < ذريعو_حد_بندي && my_هدف < هدف_حد_بندي)
    {
        ch = *(my_ذريعو++);

        إذا (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = وٿي_انگ++;
            *(my_هدف++) = (حرف) ch;
        }
        وإلا إذا (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = وٿي_انگ;
            *(my_هدف++) = (uint8_t) ((ch >> 6) | 0xc0);
            إذا (my_هدف < هدف_حد_بندي)
            {
                *(myOffsets++) = وٿي_انگ++;
                *(my_هدف++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            وإلا
            {
                cnv->char_Error_بفر[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_بفر_ڊيگهائي = 1;
                *err = U_بفر_حد_کان_مٿي_ERROR;
            }
        }
        وإلا
        /* Check for surrogates */
        {
            ايندڙ_ذريعو_اشاريو = وٿي_انگ + 1;

            إذا(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                إذا (my_ذريعو < ذريعو_حد_بندي) {
                    /* test both code units */
                    إذا(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*my_ذريعو)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *my_ذريعو);
                        ++my_ذريعو;
                        ++ايندڙ_ذريعو_اشاريو;
                        /* exit this condition tree */
                    }
                    وإلا {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_مليو;
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
            عارضي_Ptr = (((هدف_حد_بندي - my_هدف) >= 4) ? my_هدف : عارضي_Buf);

            إذا (ch <= وڌ_کان_وڌ_UCS2) {
                اشاريو_منزل_Write = 2;
                عارضي_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            وإلا {
                اشاريو_منزل_Write = 3;
                عارضي_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                عارضي_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            عارضي_Ptr[اشاريو_منزل_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            عارضي_Ptr[اشاريو_منزل_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            إذا (عارضي_Ptr == my_هدف) {
                /* There was enough space to write the codepoint directly. */
                my_هدف += (اشاريو_منزل_Write + 1);
                myOffsets[0] = وٿي_انگ;
                myOffsets[1] = وٿي_انگ;
                myOffsets[2] = وٿي_انگ;
                إذا (اشاريو_منزل_Write >= 3) {
                    myOffsets[3] = وٿي_انگ;
                }
                myOffsets += (اشاريو_منزل_Write + 1);
            }
            وإلا {
                /* We might run out of room soon. Write it slowly. */
                لكل (; عارضي_Ptr <= (عارضي_Buf + اشاريو_منزل_Write); عارضي_Ptr++) {
                    إذا (my_هدف < هدف_حد_بندي)
                    {
                        *(myOffsets++) = وٿي_انگ;
                        *(my_هدف++) = *عارضي_Ptr;
                    }
                    وإلا
                    {
                        cnv->char_Error_بفر[cnv->char_Error_بفر_ڊيگهائي++] = *عارضي_Ptr;
                        *err = U_بفر_حد_کان_مٿي_ERROR;
                    }
                }
            }
            وٿي_انگ = ايندڙ_ذريعو_اشاريو;
        }
    }

    إذا (my_ذريعو < ذريعو_حد_بندي && my_هدف >= هدف_حد_بندي && U_SUCCESS(*err))
    {
        *err = U_بفر_حد_کان_مٿي_ERROR;
    }

    دليلون_2->هدف = (حرف *) my_هدف;
    دليلون_2->ذريعو = my_ذريعو;
    دليلون_2->offsets = myOffsets;
}

U_CDECL_BEGIN
ساكن UChar32 U_CALLCONV ucnv_get_ايندڙ_UChar_UTF8(UConverter_منزل_Unicode_دليلون *دليلون_2,
                                               UError_ڪوڊ *err) {
    UConverter *cnv;
    ثابت uint8_t *ذريعو_Initial;
    ثابت uint8_t *ذريعو;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = دليلون_2->converter;
    ذريعو_Initial = ذريعو = (ثابت uint8_t *)دليلون_2->ذريعو;
    إذا (ذريعو >= (ثابت uint8_t *)دليلون_2->ذريعو_حد_بندي)
    {
        /* no input */
        *err = U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    myByte = *(ذريعو++);
    إذا (U8_IS_SINGLE(myByte))
    {
        دليلون_2->ذريعو = (ثابت حرف *)ذريعو;
        أرجع (UChar32)myByte;
    }

    uint16_t ڳڻپ_Trail_بائيٽون = U8_ڳڻپ_TRAIL_بائيٽون(myByte);
    إذا (ڳڻپ_Trail_بائيٽون == 0) {
        cnv->منزل_UBytes[0] = myByte;
        cnv->منزل_ULength = 1;
        *err = U_ILLEGAL_CHAR_مليو;
        دليلون_2->ذريعو = (ثابت حرف *)ذريعو;
        أرجع 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    إذا (((ثابت حرف *)ذريعو + ڳڻپ_Trail_بائيٽون) > دليلون_2->ذريعو_حد_بندي)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extra_بائيٽون_منزل_Write = ڳڻپ_Trail_بائيٽون + 1;
        cnv->منزل_UBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_مليو;
        طالما(ذريعو < (ثابت uint8_t *)دليلون_2->ذريعو_حد_بندي) {
            uint8_t b = *ذريعو;
            إذا(icu::UTF8::isValidTrail(myByte, b, i, extra_بائيٽون_منزل_Write)) {
                cnv->منزل_UBytes[i++] = b;
                ++ذريعو;
            } وإلا {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_مليو;
                اكسر;
            }
        }
        cnv->منزل_ULength = i;
        دليلون_2->ذريعو = (ثابت حرف *)ذريعو;
        أرجع 0xffff;
    }

    ch = myByte << 6;
    إذا(ڳڻپ_Trail_بائيٽون == 2) {
        uint8_t t1 = *ذريعو, t2;
        إذا(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++ذريعو)) {
            دليلون_2->ذريعو = (ثابت حرف *)(ذريعو + 1);
            أرجع (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } وإلا إذا(ڳڻپ_Trail_بائيٽون == 1) {
        uint8_t t1 = *ذريعو;
        إذا(U8_IS_TRAIL(t1)) {
            دليلون_2->ذريعو = (ثابت حرف *)(ذريعو + 1);
            أرجع (ch + t1) - offsetsFromUTF8[2];
        }
    } وإلا {  // countTrailBytes == 3
        uint8_t t1 = *ذريعو, t2, t3;
        إذا(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++ذريعو) &&
                U8_IS_TRAIL(t3 = *++ذريعو)) {
            دليلون_2->ذريعو = (ثابت حرف *)(ذريعو + 1);
            أرجع (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    دليلون_2->ذريعو = (ثابت حرف *)ذريعو;

    لكل(i = 0; ذريعو_Initial < ذريعو; ++i) {
        cnv->منزل_UBytes[i] = *ذريعو_Initial++;
    }
    cnv->منزل_ULength = i;
    *err = U_ILLEGAL_CHAR_مليو;
    أرجع 0xffff;
} 
U_CDECL_پڇاڙي

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
ساكن فراغ U_CALLCONV
ucnv_UTF8FromUTF8(UConverter_From_Unicode_دليلون *pFromUArgs,
                  UConverter_منزل_Unicode_دليلون *p_منزل_UArgs,
                  UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *utf8;
    ثابت uint8_t *ذريعو, *ذريعو_حد_بندي;
    uint8_t *هدف;
    int32_t هدف_صلاحيت;
    int32_t count;

    int8_t old_منزل_ULength, منزل_ULength, منزل_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    /* set up the local pointers */
    utf8=p_منزل_UArgs->converter;
    ذريعو=(uint8_t *)p_منزل_UArgs->ذريعو;
    ذريعو_حد_بندي=(uint8_t *)p_منزل_UArgs->ذريعو_حد_بندي;
    هدف=(uint8_t *)pFromUArgs->هدف;
    هدف_صلاحيت=(int32_t)(pFromUArgs->هدف_حد_بندي-pFromUArgs->هدف);

    /* get the converter state from the UTF-8 UConverter */
    إذا(utf8->منزل_ULength > 0) {
        منزل_ULength=old_منزل_ULength=utf8->منزل_ULength;
        منزل_ULimit=(int8_t)utf8->گهڻائي;
        c=(UChar32)utf8->منزل_Unicode_حالت;
    } وإلا {
        منزل_ULength=old_منزل_ULength=منزل_ULimit=0;
        c = 0;
    }

    count=(int32_t)(ذريعو_حد_بندي-ذريعو)+old_منزل_ULength;
    إذا(count<منزل_ULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } وإلا إذا(هدف_صلاحيت<منزل_ULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *p_Error_ڪوڊ=U_USING_DEFAULT_WARNING;
        أرجع;
    } وإلا {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        إذا(count>هدف_صلاحيت) {
            count=هدف_صلاحيت;
        }

        // The conversion loop checks count>0 only once per character.
        // If the buffer ends with a truncated sequence,
        // then we reduce the count to stop before that,
        // and collect the remaining bytes after the conversion loop.

        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length=count-منزل_ULength;
        U8_TRUNCATE_IF_INCOMPLETE(ذريعو, 0, length);
        count=منزل_ULength+length;
    }

    إذا(c!=0) {
        utf8->منزل_Unicode_حالت=0;
        utf8->منزل_ULength=0;
        اذهب وڌيڪ_بائيٽون;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    طالما(count>0) {
        b=*ذريعو++;
        إذا(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *هدف++=b;
            --count;
            استمر;
        } وإلا {
            إذا(b>=0xe0) {
                إذا( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=ذريعو[0]) &&
                    U8_IS_TRAIL(t2=ذريعو[1])
                ) {
                    ذريعو+=2;
                    *هدف++=b;
                    *هدف++=t1;
                    *هدف++=t2;
                    count-=3;
                    استمر;
                }
            } وإلا {
                إذا( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*ذريعو)
                ) {
                    ++ذريعو;
                    *هدف++=b;
                    *هدف++=t1;
                    count-=2;
                    استمر;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            old_منزل_ULength=0;
            منزل_ULength=1;
            منزل_ULimit=U8_ڳڻپ_بائيٽون_NON_ASCII(b);
            c=b;
وڌيڪ_بائيٽون:
            طالما(منزل_ULength<منزل_ULimit) {
                إذا(ذريعو<ذريعو_حد_بندي) {
                    b=*ذريعو;
                    إذا(icu::UTF8::isValidTrail(c, b, منزل_ULength, منزل_ULimit)) {
                        ++ذريعو;
                        ++منزل_ULength;
                        c=(c<<6)+b;
                    } وإلا {
                        اكسر; /* sequence too short, stop with toULength<toULimit */
                    }
                } وإلا {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    ذريعو-=(منزل_ULength-old_منزل_ULength);
                    طالما(old_منزل_ULength<منزل_ULength) {
                        utf8->منزل_UBytes[old_منزل_ULength++]=*ذريعو++;
                    }
                    utf8->منزل_Unicode_حالت=c;
                    utf8->منزل_ULength=منزل_ULength;
                    utf8->گهڻائي=منزل_ULimit;
                    p_منزل_UArgs->ذريعو=(حرف *)ذريعو;
                    pFromUArgs->هدف=(حرف *)هدف;
                    أرجع;
                }
            }

            إذا(منزل_ULength!=منزل_ULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                ذريعو-=(منزل_ULength-old_منزل_ULength);
                طالما(old_منزل_ULength<منزل_ULength) {
                    utf8->منزل_UBytes[old_منزل_ULength++]=*ذريعو++;
                }
                utf8->منزل_ULength=منزل_ULength;
                p_منزل_UArgs->ذريعو=(حرف *)ذريعو;
                pFromUArgs->هدف=(حرف *)هدف;
                *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                أرجع;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                لكل(i=0; i<old_منزل_ULength; ++i) {
                    *هدف++=utf8->منزل_UBytes[i];
                }
                ذريعو-=(منزل_ULength-old_منزل_ULength);
                لكل(; i<منزل_ULength; ++i) {
                    *هدف++=*ذريعو++;
                }
                count-=منزل_ULength;
            }
        }
    }
    U_ASSERT(count>=0);

    إذا(U_SUCCESS(*p_Error_ڪوڊ) && ذريعو<ذريعو_حد_بندي) {
        إذا(هدف==(ثابت uint8_t *)pFromUArgs->هدف_حد_بندي) {
            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
        } وإلا {
            b=*ذريعو;
            منزل_ULimit=U8_ڳڻپ_بائيٽون(b);
            إذا(منزل_ULimit>(ذريعو_حد_بندي-ذريعو)) {
                /* collect a truncated byte sequence */
                منزل_ULength=0;
                c=b;
                لكل(;;) {
                    utf8->منزل_UBytes[منزل_ULength++]=b;
                    إذا(++ذريعو==ذريعو_حد_بندي) {
                        /* partial byte sequence at end of source */
                        utf8->منزل_Unicode_حالت=c;
                        utf8->منزل_ULength=منزل_ULength;
                        utf8->گهڻائي=منزل_ULimit;
                        اكسر;
                    } وإلا إذا(!icu::UTF8::isValidTrail(c, b=*ذريعو, منزل_ULength, منزل_ULimit)) {
                        utf8->منزل_ULength=منزل_ULength;
                        *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                        اكسر;
                    }
                    c=(c<<6)+b;
                }
            } وإلا {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *p_Error_ڪوڊ=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    p_منزل_UArgs->ذريعو=(حرف *)ذريعو;
    pFromUArgs->هدف=(حرف *)هدف;
}

U_CDECL_پڇاڙي

/* UTF-8 converter data ----------------------------------------------------- */

ساكن ثابت UConverterImpl _UTF8Impl={
    UCNV_UTF8,

    خالي_اشارو,
    خالي_اشارو,

    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,

    ucnv_منزل_Unicode_UTF8,
    ucnv_منزل_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    ucnv_get_ايندڙ_UChar_UTF8,

    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,
    ucnv_get_Non_Surrogate_Unicode_سيٽ,

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

    خالي_اشارو,
    خالي_اشارو,

    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,

    ucnv_منزل_Unicode_UTF8,
    ucnv_منزل_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    خالي_اشارو,

    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,
    ucnv_get_Complete_Unicode_سيٽ,

    خالي_اشارو,
    خالي_اشارو
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

#جيڪڏهن_پورو
