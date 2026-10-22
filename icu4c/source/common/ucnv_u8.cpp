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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION

#شامل "unicode/ucnv.h"
#شامل "unicode/utf.h"
#شامل "unicode/utf8.h"
#شامل "unicode/utf16.h"
#شامل "uassert.h"
#شامل "ucnv_bld.h"
#شامل "ucnv_cnv.h"
#شامل "cmemory.h"
#شامل "ustr_imp.h"

/* Prototypes --------------------------------------------------------------- */

/* Keep these here to make finicky compilers happy */

U_CFUNC خالی ucnv_fromUnicode_UTF8(UConverter_From_Unicode_دلائل *دلائل_2,
                                           UError_رمز *err);
U_CFUNC خالی ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverter_From_Unicode_دلائل *دلائل_2,
                                                        UError_رمز *err);


/* UTF-8 -------------------------------------------------------------------- */

#تعریف_کرو زیادہ_سے_زیادہ_UCS2            0x0000FFFF

ساکن ثابت uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

ساکن UBool has_CESU8_ڈیٹا(ثابت UConverter *cnv)
{
#اگر UCONFIG_ONLY_HTML_CONVERSION
    واپس جھوٹ;
#ورنہ
    واپس cnv->shared_ڈیٹا == &CESU8_ڈیٹا;
#اگر_ختم
}
U_CDECL_BEGIN
ساکن خالی  U_CALLCONV ucnv_مقصد_Unicode_UTF8 (UConverter_مقصد_Unicode_دلائل * دلائل_2,
                                  UError_رمز * err)
{
    UConverter *cnv = دلائل_2->converter;
    ثابت غیرعلامتی حرف *my_ماخذ = (غیرعلامتی حرف *) دلائل_2->ماخذ;
    char16_t *my_ہدف = دلائل_2->ہدف;
    ثابت غیرعلامتی حرف *ماخذ_حد_بندی = (غیرعلامتی حرف *) دلائل_2->ماخذ_حد_بندی;
    ثابت char16_t *ہدف_حد_بندی = دلائل_2->ہدف_حد_بندی;
    غیرعلامتی حرف *مقصد_UBytes = cnv->مقصد_UBytes;
    UBool isCESU8 = has_CESU8_ڈیٹا(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, اندر_بائٹیں;

    /* Restore size of current sequence */
    اگر (cnv->مقصد_ULength > 0 && my_ہدف < ہدف_حد_بندی)
    {
        اندر_بائٹیں = cnv->منوال;            /* restore # of bytes to consume */
        i = cnv->مقصد_ULength;             /* restore # of bytes consumed */
        cnv->مقصد_ULength = 0;

        ch = cnv->مقصد_Unicode_حالت;/*Stores the previously calculated ch from a previous call*/
        cnv->مقصد_Unicode_حالت = 0;
        جاؤ morebytes;
    }


    جب (my_ماخذ < ماخذ_حد_بندی && my_ہدف < ہدف_حد_بندی)
    {
        ch = *(my_ماخذ++);
        اگر (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(my_ہدف++) = (char16_t) ch;
        }
        ورنہ
        {
            /* store the first char */
            مقصد_UBytes[0] = (حرف)ch;
            اندر_بائٹیں = U8_گنتی_بائٹیں_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            جب (i < اندر_بائٹیں)
            {
                اگر (my_ماخذ < ماخذ_حد_بندی)
                {
                    مقصد_UBytes[i] = (حرف) (ch2 = *my_ماخذ);
                    اگر (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, اندر_بائٹیں) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        توڑو; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++my_ماخذ;
                    i++;
                }
                ورنہ
                {
                    /* stores a partially calculated target*/
                    cnv->مقصد_Unicode_حالت = ch;
                    cnv->منوال = اندر_بائٹیں;
                    cnv->مقصد_ULength = (int8_t) i;
                    جاؤ donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            اگر (i == اندر_بائٹیں && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[اندر_بائٹیں];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                اگر (ch <= زیادہ_سے_زیادہ_UCS2) 
                {
                    /* fits in 16 bits */
                    *(my_ہدف++) = (char16_t) ch;
                }
                ورنہ
                {
                    /* write out the surrogates */
                    *(my_ہدف++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    اگر (my_ہدف < ہدف_حد_بندی)
                    {
                        *(my_ہدف++) = (char16_t)ch;
                    }
                    ورنہ
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UChar_Error_بفر[0] = (char16_t) ch;
                        cnv->UChar_Error_بفر_لمبائی = 1;
                        *err = U_بفر_حد_سے_تجاوز_ERROR;
                        توڑو;
                    }
                }
            }
            ورنہ
            {
                cnv->مقصد_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_ملا;
                توڑو;
            }
        }
    }

donefornow:
    اگر (my_ماخذ < ماخذ_حد_بندی && my_ہدف >= ہدف_حد_بندی && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_بفر_حد_سے_تجاوز_ERROR;
    }

    دلائل_2->ہدف = my_ہدف;
    دلائل_2->ماخذ = (ثابت حرف *) my_ماخذ;
}

ساکن خالی  U_CALLCONV ucnv_مقصد_Unicode_UTF8_OFFSETS_LOGIC (UConverter_مقصد_Unicode_دلائل * دلائل_2,
                                                UError_رمز * err)
{
    UConverter *cnv = دلائل_2->converter;
    ثابت غیرعلامتی حرف *my_ماخذ = (غیرعلامتی حرف *) دلائل_2->ماخذ;
    char16_t *my_ہدف = دلائل_2->ہدف;
    int32_t *myOffsets = دلائل_2->offsets;
    int32_t انحراف_عدد = 0;
    ثابت غیرعلامتی حرف *ماخذ_حد_بندی = (غیرعلامتی حرف *) دلائل_2->ماخذ_حد_بندی;
    ثابت char16_t *ہدف_حد_بندی = دلائل_2->ہدف_حد_بندی;
    غیرعلامتی حرف *مقصد_UBytes = cnv->مقصد_UBytes;
    UBool isCESU8 = has_CESU8_ڈیٹا(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, اندر_بائٹیں;

    /* Restore size of current sequence */
    اگر (cnv->مقصد_ULength > 0 && my_ہدف < ہدف_حد_بندی)
    {
        اندر_بائٹیں = cnv->منوال;            /* restore # of bytes to consume */
        i = cnv->مقصد_ULength;             /* restore # of bytes consumed */
        cnv->مقصد_ULength = 0;

        ch = cnv->مقصد_Unicode_حالت;/*Stores the previously calculated ch from a previous call*/
        cnv->مقصد_Unicode_حالت = 0;
        جاؤ morebytes;
    }

    جب (my_ماخذ < ماخذ_حد_بندی && my_ہدف < ہدف_حد_بندی)
    {
        ch = *(my_ماخذ++);
        اگر (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(my_ہدف++) = (char16_t) ch;
            *(myOffsets++) = انحراف_عدد++;
        }
        ورنہ
        {
            مقصد_UBytes[0] = (حرف)ch;
            اندر_بائٹیں = U8_گنتی_بائٹیں_NON_ASCII(ch);
            i = 1;

morebytes:
            جب (i < اندر_بائٹیں)
            {
                اگر (my_ماخذ < ماخذ_حد_بندی)
                {
                    مقصد_UBytes[i] = (حرف) (ch2 = *my_ماخذ);
                    اگر (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, اندر_بائٹیں) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        توڑو; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++my_ماخذ;
                    i++;
                }
                ورنہ
                {
                    cnv->مقصد_Unicode_حالت = ch;
                    cnv->منوال = اندر_بائٹیں;
                    cnv->مقصد_ULength = (int8_t)i;
                    جاؤ donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            اگر (i == اندر_بائٹیں && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[اندر_بائٹیں];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                اگر (ch <= زیادہ_سے_زیادہ_UCS2) 
                {
                    /* fits in 16 bits */
                    *(my_ہدف++) = (char16_t) ch;
                    *(myOffsets++) = انحراف_عدد;
                }
                ورنہ
                {
                    /* write out the surrogates */
                    *(my_ہدف++) = U16_LEAD(ch);
                    *(myOffsets++) = انحراف_عدد;
                    ch = U16_TRAIL(ch);
                    اگر (my_ہدف < ہدف_حد_بندی)
                    {
                        *(my_ہدف++) = (char16_t)ch;
                        *(myOffsets++) = انحراف_عدد;
                    }
                    ورنہ
                    {
                        cnv->UChar_Error_بفر[0] = (char16_t) ch;
                        cnv->UChar_Error_بفر_لمبائی = 1;
                        *err = U_بفر_حد_سے_تجاوز_ERROR;
                    }
                }
                انحراف_عدد += i;
            }
            ورنہ
            {
                cnv->مقصد_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_ملا;
                توڑو;
            }
        }
    }

donefornow:
    اگر (my_ماخذ < ماخذ_حد_بندی && my_ہدف >= ہدف_حد_بندی && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_بفر_حد_سے_تجاوز_ERROR;
    }

    دلائل_2->ہدف = my_ہدف;
    دلائل_2->ماخذ = (ثابت حرف *) my_ماخذ;
    دلائل_2->offsets = myOffsets;
}
U_CDECL_انتہا

U_CFUNC خالی  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverter_From_Unicode_دلائل * دلائل_2,
                                    UError_رمز * err)
{
    UConverter *cnv = دلائل_2->converter;
    ثابت char16_t *my_ماخذ = دلائل_2->ماخذ;
    ثابت char16_t *ماخذ_حد_بندی = دلائل_2->ماخذ_حد_بندی;
    uint8_t *my_ہدف = (uint8_t *) دلائل_2->ہدف;
    ثابت uint8_t *ہدف_حد_بندی = (uint8_t *) دلائل_2->ہدف_حد_بندی;
    uint8_t *عارضی_Ptr;
    UChar32 ch;
    uint8_t عارضی_Buf[4];
    int32_t اشاریہ_مقصد_Write;
    UBool isNotCESU8 = !has_CESU8_ڈیٹا(cnv);

    اگر (cnv->fromUChar32 && my_ہدف < ہدف_حد_بندی)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        جاؤ lowsurrogate;
    }

    جب (my_ماخذ < ماخذ_حد_بندی && my_ہدف < ہدف_حد_بندی)
    {
        ch = *(my_ماخذ++);

        اگر (ch < 0x80)        /* Single byte */
        {
            *(my_ہدف++) = (uint8_t) ch;
        }
        ورنہ اگر (ch < 0x800)  /* Double byte */
        {
            *(my_ہدف++) = (uint8_t) ((ch >> 6) | 0xc0);
            اگر (my_ہدف < ہدف_حد_بندی)
            {
                *(my_ہدف++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            ورنہ
            {
                cnv->char_Error_بفر[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_بفر_لمبائی = 1;
                *err = U_بفر_حد_سے_تجاوز_ERROR;
            }
        }
        ورنہ {
            /* Check for surrogates */
            اگر(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                اگر (my_ماخذ < ماخذ_حد_بندی) {
                    /* test both code units */
                    اگر(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*my_ماخذ)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *my_ماخذ);
                        ++my_ماخذ;
                        /* exit this condition tree */
                    }
                    ورنہ {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ملا;
                        توڑو;
                    }
                }
                ورنہ {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    توڑو;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            عارضی_Ptr = (((ہدف_حد_بندی - my_ہدف) >= 4) ? my_ہدف : عارضی_Buf);

            اگر (ch <= زیادہ_سے_زیادہ_UCS2) {
                اشاریہ_مقصد_Write = 2;
                عارضی_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            ورنہ {
                اشاریہ_مقصد_Write = 3;
                عارضی_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                عارضی_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            عارضی_Ptr[اشاریہ_مقصد_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            عارضی_Ptr[اشاریہ_مقصد_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            اگر (عارضی_Ptr == my_ہدف) {
                /* There was enough space to write the codepoint directly. */
                my_ہدف += (اشاریہ_مقصد_Write + 1);
            }
            ورنہ {
                /* We might run out of room soon. Write it slowly. */
                برائے (; عارضی_Ptr <= (عارضی_Buf + اشاریہ_مقصد_Write); عارضی_Ptr++) {
                    اگر (my_ہدف < ہدف_حد_بندی) {
                        *(my_ہدف++) = *عارضی_Ptr;
                    }
                    ورنہ {
                        cnv->char_Error_بفر[cnv->char_Error_بفر_لمبائی++] = *عارضی_Ptr;
                        *err = U_بفر_حد_سے_تجاوز_ERROR;
                    }
                }
            }
        }
    }

    اگر (my_ماخذ < ماخذ_حد_بندی && my_ہدف >= ہدف_حد_بندی && U_SUCCESS(*err))
    {
        *err = U_بفر_حد_سے_تجاوز_ERROR;
    }

    دلائل_2->ہدف = (حرف *) my_ہدف;
    دلائل_2->ماخذ = my_ماخذ;
}

U_CFUNC خالی  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverter_From_Unicode_دلائل * دلائل_2,
                                                  UError_رمز * err)
{
    UConverter *cnv = دلائل_2->converter;
    ثابت char16_t *my_ماخذ = دلائل_2->ماخذ;
    int32_t *myOffsets = دلائل_2->offsets;
    ثابت char16_t *ماخذ_حد_بندی = دلائل_2->ماخذ_حد_بندی;
    uint8_t *my_ہدف = (uint8_t *) دلائل_2->ہدف;
    ثابت uint8_t *ہدف_حد_بندی = (uint8_t *) دلائل_2->ہدف_حد_بندی;
    uint8_t *عارضی_Ptr;
    UChar32 ch;
    int32_t انحراف_عدد, اگلا_ماخذ_اشاریہ;
    int32_t اشاریہ_مقصد_Write;
    uint8_t عارضی_Buf[4];
    UBool isNotCESU8 = !has_CESU8_ڈیٹا(cnv);

    اگر (cnv->fromUChar32 && my_ہدف < ہدف_حد_بندی)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        انحراف_عدد = -1;
        اگلا_ماخذ_اشاریہ = 0;
        جاؤ lowsurrogate;
    } ورنہ {
        انحراف_عدد = 0;
    }

    جب (my_ماخذ < ماخذ_حد_بندی && my_ہدف < ہدف_حد_بندی)
    {
        ch = *(my_ماخذ++);

        اگر (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = انحراف_عدد++;
            *(my_ہدف++) = (حرف) ch;
        }
        ورنہ اگر (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = انحراف_عدد;
            *(my_ہدف++) = (uint8_t) ((ch >> 6) | 0xc0);
            اگر (my_ہدف < ہدف_حد_بندی)
            {
                *(myOffsets++) = انحراف_عدد++;
                *(my_ہدف++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            ورنہ
            {
                cnv->char_Error_بفر[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_بفر_لمبائی = 1;
                *err = U_بفر_حد_سے_تجاوز_ERROR;
            }
        }
        ورنہ
        /* Check for surrogates */
        {
            اگلا_ماخذ_اشاریہ = انحراف_عدد + 1;

            اگر(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                اگر (my_ماخذ < ماخذ_حد_بندی) {
                    /* test both code units */
                    اگر(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*my_ماخذ)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *my_ماخذ);
                        ++my_ماخذ;
                        ++اگلا_ماخذ_اشاریہ;
                        /* exit this condition tree */
                    }
                    ورنہ {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ملا;
                        توڑو;
                    }
                }
                ورنہ {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    توڑو;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            عارضی_Ptr = (((ہدف_حد_بندی - my_ہدف) >= 4) ? my_ہدف : عارضی_Buf);

            اگر (ch <= زیادہ_سے_زیادہ_UCS2) {
                اشاریہ_مقصد_Write = 2;
                عارضی_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            ورنہ {
                اشاریہ_مقصد_Write = 3;
                عارضی_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                عارضی_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            عارضی_Ptr[اشاریہ_مقصد_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            عارضی_Ptr[اشاریہ_مقصد_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            اگر (عارضی_Ptr == my_ہدف) {
                /* There was enough space to write the codepoint directly. */
                my_ہدف += (اشاریہ_مقصد_Write + 1);
                myOffsets[0] = انحراف_عدد;
                myOffsets[1] = انحراف_عدد;
                myOffsets[2] = انحراف_عدد;
                اگر (اشاریہ_مقصد_Write >= 3) {
                    myOffsets[3] = انحراف_عدد;
                }
                myOffsets += (اشاریہ_مقصد_Write + 1);
            }
            ورنہ {
                /* We might run out of room soon. Write it slowly. */
                برائے (; عارضی_Ptr <= (عارضی_Buf + اشاریہ_مقصد_Write); عارضی_Ptr++) {
                    اگر (my_ہدف < ہدف_حد_بندی)
                    {
                        *(myOffsets++) = انحراف_عدد;
                        *(my_ہدف++) = *عارضی_Ptr;
                    }
                    ورنہ
                    {
                        cnv->char_Error_بفر[cnv->char_Error_بفر_لمبائی++] = *عارضی_Ptr;
                        *err = U_بفر_حد_سے_تجاوز_ERROR;
                    }
                }
            }
            انحراف_عدد = اگلا_ماخذ_اشاریہ;
        }
    }

    اگر (my_ماخذ < ماخذ_حد_بندی && my_ہدف >= ہدف_حد_بندی && U_SUCCESS(*err))
    {
        *err = U_بفر_حد_سے_تجاوز_ERROR;
    }

    دلائل_2->ہدف = (حرف *) my_ہدف;
    دلائل_2->ماخذ = my_ماخذ;
    دلائل_2->offsets = myOffsets;
}

U_CDECL_BEGIN
ساکن UChar32 U_CALLCONV ucnv_get_اگلا_UChar_UTF8(UConverter_مقصد_Unicode_دلائل *دلائل_2,
                                               UError_رمز *err) {
    UConverter *cnv;
    ثابت uint8_t *ماخذ_Initial;
    ثابت uint8_t *ماخذ;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = دلائل_2->converter;
    ماخذ_Initial = ماخذ = (ثابت uint8_t *)دلائل_2->ماخذ;
    اگر (ماخذ >= (ثابت uint8_t *)دلائل_2->ماخذ_حد_بندی)
    {
        /* no input */
        *err = U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0xffff;
    }

    myByte = *(ماخذ++);
    اگر (U8_IS_SINGLE(myByte))
    {
        دلائل_2->ماخذ = (ثابت حرف *)ماخذ;
        واپس (UChar32)myByte;
    }

    uint16_t گنتی_Trail_بائٹیں = U8_گنتی_TRAIL_بائٹیں(myByte);
    اگر (گنتی_Trail_بائٹیں == 0) {
        cnv->مقصد_UBytes[0] = myByte;
        cnv->مقصد_ULength = 1;
        *err = U_ILLEGAL_CHAR_ملا;
        دلائل_2->ماخذ = (ثابت حرف *)ماخذ;
        واپس 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    اگر (((ثابت حرف *)ماخذ + گنتی_Trail_بائٹیں) > دلائل_2->ماخذ_حد_بندی)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extra_بائٹیں_مقصد_Write = گنتی_Trail_بائٹیں + 1;
        cnv->مقصد_UBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_ملا;
        جب(ماخذ < (ثابت uint8_t *)دلائل_2->ماخذ_حد_بندی) {
            uint8_t b = *ماخذ;
            اگر(icu::UTF8::isValidTrail(myByte, b, i, extra_بائٹیں_مقصد_Write)) {
                cnv->مقصد_UBytes[i++] = b;
                ++ماخذ;
            } ورنہ {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_ملا;
                توڑو;
            }
        }
        cnv->مقصد_ULength = i;
        دلائل_2->ماخذ = (ثابت حرف *)ماخذ;
        واپس 0xffff;
    }

    ch = myByte << 6;
    اگر(گنتی_Trail_بائٹیں == 2) {
        uint8_t t1 = *ماخذ, t2;
        اگر(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++ماخذ)) {
            دلائل_2->ماخذ = (ثابت حرف *)(ماخذ + 1);
            واپس (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } ورنہ اگر(گنتی_Trail_بائٹیں == 1) {
        uint8_t t1 = *ماخذ;
        اگر(U8_IS_TRAIL(t1)) {
            دلائل_2->ماخذ = (ثابت حرف *)(ماخذ + 1);
            واپس (ch + t1) - offsetsFromUTF8[2];
        }
    } ورنہ {  // countTrailBytes == 3
        uint8_t t1 = *ماخذ, t2, t3;
        اگر(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++ماخذ) &&
                U8_IS_TRAIL(t3 = *++ماخذ)) {
            دلائل_2->ماخذ = (ثابت حرف *)(ماخذ + 1);
            واپس (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    دلائل_2->ماخذ = (ثابت حرف *)ماخذ;

    برائے(i = 0; ماخذ_Initial < ماخذ; ++i) {
        cnv->مقصد_UBytes[i] = *ماخذ_Initial++;
    }
    cnv->مقصد_ULength = i;
    *err = U_ILLEGAL_CHAR_ملا;
    واپس 0xffff;
} 
U_CDECL_انتہا

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
ساکن خالی U_CALLCONV
ucnv_UTF8FromUTF8(UConverter_From_Unicode_دلائل *pFromUArgs,
                  UConverter_مقصد_Unicode_دلائل *p_مقصد_UArgs,
                  UError_رمز *p_Error_رمز) {
    UConverter *utf8;
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی;
    uint8_t *ہدف;
    int32_t ہدف_استعداد;
    int32_t count;

    int8_t old_مقصد_ULength, مقصد_ULength, مقصد_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    /* set up the local pointers */
    utf8=p_مقصد_UArgs->converter;
    ماخذ=(uint8_t *)p_مقصد_UArgs->ماخذ;
    ماخذ_حد_بندی=(uint8_t *)p_مقصد_UArgs->ماخذ_حد_بندی;
    ہدف=(uint8_t *)pFromUArgs->ہدف;
    ہدف_استعداد=(int32_t)(pFromUArgs->ہدف_حد_بندی-pFromUArgs->ہدف);

    /* get the converter state from the UTF-8 UConverter */
    اگر(utf8->مقصد_ULength > 0) {
        مقصد_ULength=old_مقصد_ULength=utf8->مقصد_ULength;
        مقصد_ULimit=(int8_t)utf8->منوال;
        c=(UChar32)utf8->مقصد_Unicode_حالت;
    } ورنہ {
        مقصد_ULength=old_مقصد_ULength=مقصد_ULimit=0;
        c = 0;
    }

    count=(int32_t)(ماخذ_حد_بندی-ماخذ)+old_مقصد_ULength;
    اگر(count<مقصد_ULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } ورنہ اگر(ہدف_استعداد<مقصد_ULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *p_Error_رمز=U_USING_DEFAULT_WARNING;
        واپس;
    } ورنہ {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        اگر(count>ہدف_استعداد) {
            count=ہدف_استعداد;
        }

        // The conversion loop checks count>0 only once per character.
        // If the buffer ends with a truncated sequence,
        // then we reduce the count to stop before that,
        // and collect the remaining bytes after the conversion loop.

        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length=count-مقصد_ULength;
        U8_TRUNCATE_IF_INCOMPLETE(ماخذ, 0, length);
        count=مقصد_ULength+length;
    }

    اگر(c!=0) {
        utf8->مقصد_Unicode_حالت=0;
        utf8->مقصد_ULength=0;
        جاؤ مزید_بائٹیں;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    جب(count>0) {
        b=*ماخذ++;
        اگر(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *ہدف++=b;
            --count;
            جاری;
        } ورنہ {
            اگر(b>=0xe0) {
                اگر( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=ماخذ[0]) &&
                    U8_IS_TRAIL(t2=ماخذ[1])
                ) {
                    ماخذ+=2;
                    *ہدف++=b;
                    *ہدف++=t1;
                    *ہدف++=t2;
                    count-=3;
                    جاری;
                }
            } ورنہ {
                اگر( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*ماخذ)
                ) {
                    ++ماخذ;
                    *ہدف++=b;
                    *ہدف++=t1;
                    count-=2;
                    جاری;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            old_مقصد_ULength=0;
            مقصد_ULength=1;
            مقصد_ULimit=U8_گنتی_بائٹیں_NON_ASCII(b);
            c=b;
مزید_بائٹیں:
            جب(مقصد_ULength<مقصد_ULimit) {
                اگر(ماخذ<ماخذ_حد_بندی) {
                    b=*ماخذ;
                    اگر(icu::UTF8::isValidTrail(c, b, مقصد_ULength, مقصد_ULimit)) {
                        ++ماخذ;
                        ++مقصد_ULength;
                        c=(c<<6)+b;
                    } ورنہ {
                        توڑو; /* sequence too short, stop with toULength<toULimit */
                    }
                } ورنہ {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    ماخذ-=(مقصد_ULength-old_مقصد_ULength);
                    جب(old_مقصد_ULength<مقصد_ULength) {
                        utf8->مقصد_UBytes[old_مقصد_ULength++]=*ماخذ++;
                    }
                    utf8->مقصد_Unicode_حالت=c;
                    utf8->مقصد_ULength=مقصد_ULength;
                    utf8->منوال=مقصد_ULimit;
                    p_مقصد_UArgs->ماخذ=(حرف *)ماخذ;
                    pFromUArgs->ہدف=(حرف *)ہدف;
                    واپس;
                }
            }

            اگر(مقصد_ULength!=مقصد_ULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                ماخذ-=(مقصد_ULength-old_مقصد_ULength);
                جب(old_مقصد_ULength<مقصد_ULength) {
                    utf8->مقصد_UBytes[old_مقصد_ULength++]=*ماخذ++;
                }
                utf8->مقصد_ULength=مقصد_ULength;
                p_مقصد_UArgs->ماخذ=(حرف *)ماخذ;
                pFromUArgs->ہدف=(حرف *)ہدف;
                *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                واپس;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                برائے(i=0; i<old_مقصد_ULength; ++i) {
                    *ہدف++=utf8->مقصد_UBytes[i];
                }
                ماخذ-=(مقصد_ULength-old_مقصد_ULength);
                برائے(; i<مقصد_ULength; ++i) {
                    *ہدف++=*ماخذ++;
                }
                count-=مقصد_ULength;
            }
        }
    }
    U_ASSERT(count>=0);

    اگر(U_SUCCESS(*p_Error_رمز) && ماخذ<ماخذ_حد_بندی) {
        اگر(ہدف==(ثابت uint8_t *)pFromUArgs->ہدف_حد_بندی) {
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
        } ورنہ {
            b=*ماخذ;
            مقصد_ULimit=U8_گنتی_بائٹیں(b);
            اگر(مقصد_ULimit>(ماخذ_حد_بندی-ماخذ)) {
                /* collect a truncated byte sequence */
                مقصد_ULength=0;
                c=b;
                برائے(;;) {
                    utf8->مقصد_UBytes[مقصد_ULength++]=b;
                    اگر(++ماخذ==ماخذ_حد_بندی) {
                        /* partial byte sequence at end of source */
                        utf8->مقصد_Unicode_حالت=c;
                        utf8->مقصد_ULength=مقصد_ULength;
                        utf8->منوال=مقصد_ULimit;
                        توڑو;
                    } ورنہ اگر(!icu::UTF8::isValidTrail(c, b=*ماخذ, مقصد_ULength, مقصد_ULimit)) {
                        utf8->مقصد_ULength=مقصد_ULength;
                        *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                        توڑو;
                    }
                    c=(c<<6)+b;
                }
            } ورنہ {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *p_Error_رمز=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    p_مقصد_UArgs->ماخذ=(حرف *)ماخذ;
    pFromUArgs->ہدف=(حرف *)ہدف;
}

U_CDECL_انتہا

/* UTF-8 converter data ----------------------------------------------------- */

ساکن ثابت UConverterImpl _UTF8Impl={
    UCNV_UTF8,

    خالی_اشاریہ,
    خالی_اشاریہ,

    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,

    ucnv_مقصد_Unicode_UTF8,
    ucnv_مقصد_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    ucnv_get_اگلا_UChar_UTF8,

    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,
    ucnv_get_Non_Surrogate_Unicode_سیٹ,

    ucnv_UTF8FromUTF8,
    ucnv_UTF8FromUTF8
};

/* The 1208 CCSID refers to any version of Unicode of UTF-8 */
ساکن ثابت UConverter_Static_ڈیٹا UTF8_Static_ڈیٹا={
    حجم(UConverter_Static_ڈیٹا),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,جھوٹ,جھوٹ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_ڈیٹا UTF8_ڈیٹا=
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&UTF8_Static_ڈیٹا, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

ساکن ثابت UConverterImpl _CESU8Impl={
    UCNV_CESU8,

    خالی_اشاریہ,
    خالی_اشاریہ,

    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,

    ucnv_مقصد_Unicode_UTF8,
    ucnv_مقصد_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    خالی_اشاریہ,

    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,
    ucnv_get_Complete_Unicode_سیٹ,

    خالی_اشاریہ,
    خالی_اشاریہ
};

ساکن ثابت UConverter_Static_ڈیٹا CESU8_Static_ڈیٹا={
    حجم(UConverter_Static_ڈیٹا),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,جھوٹ,جھوٹ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_ڈیٹا CESU8_ڈیٹا=
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&CESU8_Static_ڈیٹا, &_CESU8Impl);

#اگر_ختم
