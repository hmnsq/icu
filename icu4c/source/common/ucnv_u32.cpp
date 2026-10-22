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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#شامل "unicode/ucnv.h"
#شامل "unicode/utf.h"
#شامل "ucnv_bld.h"
#شامل "ucnv_cnv.h"
#شامل "cmemory.h"

#تعریف_کرو زیادہ_سے_زیادہ_UCS2            0x0000FFFF
#تعریف_کرو زیادہ_سے_زیادہ_UTF             0x0010FFFF
#تعریف_کرو HALF_سرکاؤ              10
#تعریف_کرو HALF_بنیاد               0x0010000
#تعریف_کرو HALF_نقاب               0x3FF
#تعریف_کرو SURROGATE_بلند_آغاز    0xD800
#تعریف_کرو SURROGATE_کم_آغاز     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#تعریف_کرو SURROGATE_کم_بنیاد      9216

شمار {
    UCNV_NEED_مقصد_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ساکن خالی U_CALLCONV
T_UConverter_مقصد_Unicode_UTF32_BE(UConverter_مقصد_Unicode_دلائل * دلائل_2,
                                UError_رمز * err)
{
    ثابت غیرعلامتی حرف *my_ماخذ = (غیرعلامتی حرف *) دلائل_2->ماخذ;
    char16_t *my_ہدف = دلائل_2->ہدف;
    ثابت غیرعلامتی حرف *ماخذ_حد_بندی = (غیرعلامتی حرف *) دلائل_2->ماخذ_حد_بندی;
    ثابت char16_t *ہدف_حد_بندی = دلائل_2->ہدف_حد_بندی;
    غیرعلامتی حرف *مقصد_UBytes = دلائل_2->converter->مقصد_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    اگر (دلائل_2->converter->مقصد_ULength > 0 && my_ہدف < ہدف_حد_بندی) {
        i = دلائل_2->converter->مقصد_ULength;       /* restore # of bytes consumed */
        دلائل_2->converter->مقصد_ULength = 0;

        ch = دلائل_2->converter->مقصد_Unicode_حالت - 1;/*Stores the previously calculated ch from a previous call*/
        دلائل_2->converter->مقصد_Unicode_حالت = 0;
        جاؤ morebytes;
    }

    جب (my_ماخذ < ماخذ_حد_بندی && my_ہدف < ہدف_حد_بندی) {
        i = 0;
        ch = 0;
morebytes:
        جب (i < حجم(uint32_t)) {
            اگر (my_ماخذ < ماخذ_حد_بندی) {
                ch = (ch << 8) | (uint8_t)(*my_ماخذ);
                مقصد_UBytes[i++] = (حرف) *(my_ماخذ++);
            }
            ورنہ {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                دلائل_2->converter->مقصد_Unicode_حالت = ch + 1;
                دلائل_2->converter->مقصد_ULength = (int8_t) i;
                جاؤ donefornow;
            }
        }

        اگر (ch <= زیادہ_سے_زیادہ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            اگر (ch <= زیادہ_سے_زیادہ_UCS2) 
            {
                /* fits in 16 bits */
                *(my_ہدف++) = (char16_t) ch;
            }
            ورنہ {
                /* write out the surrogates */
                *(my_ہدف++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                اگر (my_ہدف < ہدف_حد_بندی) {
                    *(my_ہدف++) = (char16_t)ch;
                }
                ورنہ {
                    /* Put in overflow buffer (not handled here) */
                    دلائل_2->converter->UChar_Error_بفر[0] = (char16_t) ch;
                    دلائل_2->converter->UChar_Error_بفر_لمبائی = 1;
                    *err = U_بفر_حد_سے_تجاوز_ERROR;
                    توڑو;
                }
            }
        }
        ورنہ {
            دلائل_2->converter->مقصد_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_ملا;
            توڑو;
        }
    }

donefornow:
    اگر (my_ماخذ < ماخذ_حد_بندی && my_ہدف >= ہدف_حد_بندی && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_بفر_حد_سے_تجاوز_ERROR;
    }

    دلائل_2->ہدف = my_ہدف;
    دلائل_2->ماخذ = (ثابت حرف *) my_ماخذ;
}

ساکن خالی U_CALLCONV
T_UConverter_مقصد_Unicode_UTF32_BE_انحراف_LOGIC(UConverter_مقصد_Unicode_دلائل * دلائل_2,
                                             UError_رمز * err)
{
    ثابت غیرعلامتی حرف *my_ماخذ = (غیرعلامتی حرف *) دلائل_2->ماخذ;
    char16_t *my_ہدف = دلائل_2->ہدف;
    int32_t *myOffsets = دلائل_2->offsets;
    ثابت غیرعلامتی حرف *ماخذ_حد_بندی = (غیرعلامتی حرف *) دلائل_2->ماخذ_حد_بندی;
    ثابت char16_t *ہدف_حد_بندی = دلائل_2->ہدف_حد_بندی;
    غیرعلامتی حرف *مقصد_UBytes = دلائل_2->converter->مقصد_UBytes;
    uint32_t ch, i;
    int32_t انحراف_عدد = 0;

    /* Restore state of current sequence */
    اگر (دلائل_2->converter->مقصد_ULength > 0 && my_ہدف < ہدف_حد_بندی) {
        i = دلائل_2->converter->مقصد_ULength;       /* restore # of bytes consumed */
        دلائل_2->converter->مقصد_ULength = 0;

        ch = دلائل_2->converter->مقصد_Unicode_حالت - 1;/*Stores the previously calculated ch from a previous call*/
        دلائل_2->converter->مقصد_Unicode_حالت = 0;
        جاؤ morebytes;
    }

    جب (my_ماخذ < ماخذ_حد_بندی && my_ہدف < ہدف_حد_بندی) {
        i = 0;
        ch = 0;
morebytes:
        جب (i < حجم(uint32_t)) {
            اگر (my_ماخذ < ماخذ_حد_بندی) {
                ch = (ch << 8) | (uint8_t)(*my_ماخذ);
                مقصد_UBytes[i++] = (حرف) *(my_ماخذ++);
            }
            ورنہ {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                دلائل_2->converter->مقصد_Unicode_حالت = ch + 1;
                دلائل_2->converter->مقصد_ULength = (int8_t) i;
                جاؤ donefornow;
            }
        }

        اگر (ch <= زیادہ_سے_زیادہ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            اگر (ch <= زیادہ_سے_زیادہ_UCS2) {
                /* fits in 16 bits */
                *(my_ہدف++) = (char16_t) ch;
                *(myOffsets++) = انحراف_عدد;
            }
            ورنہ {
                /* write out the surrogates */
                *(my_ہدف++) = U16_LEAD(ch);
                *myOffsets++ = انحراف_عدد;
                ch = U16_TRAIL(ch);
                اگر (my_ہدف < ہدف_حد_بندی)
                {
                    *(my_ہدف++) = (char16_t)ch;
                    *(myOffsets++) = انحراف_عدد;
                }
                ورنہ {
                    /* Put in overflow buffer (not handled here) */
                    دلائل_2->converter->UChar_Error_بفر[0] = (char16_t) ch;
                    دلائل_2->converter->UChar_Error_بفر_لمبائی = 1;
                    *err = U_بفر_حد_سے_تجاوز_ERROR;
                    توڑو;
                }
            }
        }
        ورنہ {
            دلائل_2->converter->مقصد_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_ملا;
            توڑو;
        }
        انحراف_عدد += i;
    }

donefornow:
    اگر (my_ماخذ < ماخذ_حد_بندی && my_ہدف >= ہدف_حد_بندی && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_بفر_حد_سے_تجاوز_ERROR;
    }

    دلائل_2->ہدف = my_ہدف;
    دلائل_2->ماخذ = (ثابت حرف *) my_ماخذ;
    دلائل_2->offsets = myOffsets;
}

ساکن خالی U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverter_From_Unicode_دلائل * دلائل_2,
                                  UError_رمز * err)
{
    ثابت char16_t *my_ماخذ = دلائل_2->ماخذ;
    غیرعلامتی حرف *my_ہدف;
    ثابت char16_t *ماخذ_حد_بندی = دلائل_2->ماخذ_حد_بندی;
    ثابت غیرعلامتی حرف *ہدف_حد_بندی = (غیرعلامتی حرف *) دلائل_2->ہدف_حد_بندی;
    UChar32 ch, ch2;
    غیرعلامتی صحیح اشاریہ_مقصد_Write;
    غیرعلامتی حرف عارضی[حجم(uint32_t)];

    اگر(my_ماخذ >= ماخذ_حد_بندی) {
        /* no input, nothing to do */
        واپس;
    }

    /* write the BOM if necessary */
    اگر(دلائل_2->converter->from_Unicode_حالت==UCNV_NEED_مقصد_WRITE_BOM) {
        ساکن ثابت حرف bom[]={ 0, 0, (حرف)0xfeu, (حرف)0xffu };
        ucnv_from_UWrite_بائٹیں(دلائل_2->converter,
                             bom, 4,
                             &دلائل_2->ہدف, دلائل_2->ہدف_حد_بندی,
                             &دلائل_2->offsets, -1,
                             err);
        دلائل_2->converter->from_Unicode_حالت=0;
    }

    my_ہدف = (غیرعلامتی حرف *) دلائل_2->ہدف;
    عارضی[0] = 0;

    اگر (دلائل_2->converter->fromUChar32) {
        ch = دلائل_2->converter->fromUChar32;
        دلائل_2->converter->fromUChar32 = 0;
        جاؤ lowsurogate;
    }

    جب (my_ماخذ < ماخذ_حد_بندی && my_ہدف < ہدف_حد_بندی) {
        ch = *(my_ماخذ++);

        اگر (U_IS_SURROGATE(ch)) {
            اگر (U_IS_LEAD(ch)) {
lowsurogate:
                اگر (my_ماخذ < ماخذ_حد_بندی) {
                    ch2 = *my_ماخذ;
                    اگر (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_بلند_آغاز) << HALF_سرکاؤ) + ch2 + SURROGATE_کم_بنیاد;
                        my_ماخذ++;
                    }
                    ورنہ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        دلائل_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ملا;
                        توڑو;
                    }
                }
                ورنہ {
                    /* ran out of source */
                    دلائل_2->converter->fromUChar32 = ch;
                    اگر (دلائل_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_ملا;
                    }
                    توڑو;
                }
            }
            ورنہ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                دلائل_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_ملا;
                توڑو;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        عارضی[1] = (uint8_t) (ch >> 16 & 0x1F);
        عارضی[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        عارضی[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        برائے (اشاریہ_مقصد_Write = 0; اشاریہ_مقصد_Write <= حجم(uint32_t) - 1; اشاریہ_مقصد_Write++) {
            اگر (my_ہدف < ہدف_حد_بندی) {
                *(my_ہدف++) = عارضی[اشاریہ_مقصد_Write];
            }
            ورنہ {
                دلائل_2->converter->char_Error_بفر[دلائل_2->converter->char_Error_بفر_لمبائی++] = عارضی[اشاریہ_مقصد_Write];
                *err = U_بفر_حد_سے_تجاوز_ERROR;
            }
        }
    }

    اگر (my_ماخذ < ماخذ_حد_بندی && my_ہدف >= ہدف_حد_بندی && U_SUCCESS(*err)) {
        *err = U_بفر_حد_سے_تجاوز_ERROR;
    }

    دلائل_2->ہدف = (حرف *) my_ہدف;
    دلائل_2->ماخذ = my_ماخذ;
}

ساکن خالی U_CALLCONV
T_UConverter_from_Unicode_UTF32_BE_انحراف_LOGIC(UConverter_From_Unicode_دلائل * دلائل_2,
                                               UError_رمز * err)
{
    ثابت char16_t *my_ماخذ = دلائل_2->ماخذ;
    غیرعلامتی حرف *my_ہدف;
    int32_t *myOffsets;
    ثابت char16_t *ماخذ_حد_بندی = دلائل_2->ماخذ_حد_بندی;
    ثابت غیرعلامتی حرف *ہدف_حد_بندی = (غیرعلامتی حرف *) دلائل_2->ہدف_حد_بندی;
    UChar32 ch, ch2;
    int32_t انحراف_عدد = 0;
    غیرعلامتی صحیح اشاریہ_مقصد_Write;
    غیرعلامتی حرف عارضی[حجم(uint32_t)];

    اگر(my_ماخذ >= ماخذ_حد_بندی) {
        /* no input, nothing to do */
        واپس;
    }

    /* write the BOM if necessary */
    اگر(دلائل_2->converter->from_Unicode_حالت==UCNV_NEED_مقصد_WRITE_BOM) {
        ساکن ثابت حرف bom[]={ 0, 0, (حرف)0xfeu, (حرف)0xffu };
        ucnv_from_UWrite_بائٹیں(دلائل_2->converter,
                             bom, 4,
                             &دلائل_2->ہدف, دلائل_2->ہدف_حد_بندی,
                             &دلائل_2->offsets, -1,
                             err);
        دلائل_2->converter->from_Unicode_حالت=0;
    }

    my_ہدف = (غیرعلامتی حرف *) دلائل_2->ہدف;
    myOffsets = دلائل_2->offsets;
    عارضی[0] = 0;

    اگر (دلائل_2->converter->fromUChar32) {
        ch = دلائل_2->converter->fromUChar32;
        دلائل_2->converter->fromUChar32 = 0;
        جاؤ lowsurogate;
    }

    جب (my_ماخذ < ماخذ_حد_بندی && my_ہدف < ہدف_حد_بندی) {
        ch = *(my_ماخذ++);

        اگر (U_IS_SURROGATE(ch)) {
            اگر (U_IS_LEAD(ch)) {
lowsurogate:
                اگر (my_ماخذ < ماخذ_حد_بندی) {
                    ch2 = *my_ماخذ;
                    اگر (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_بلند_آغاز) << HALF_سرکاؤ) + ch2 + SURROGATE_کم_بنیاد;
                        my_ماخذ++;
                    }
                    ورنہ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        دلائل_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ملا;
                        توڑو;
                    }
                }
                ورنہ {
                    /* ran out of source */
                    دلائل_2->converter->fromUChar32 = ch;
                    اگر (دلائل_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_ملا;
                    }
                    توڑو;
                }
            }
            ورنہ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                دلائل_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_ملا;
                توڑو;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        عارضی[1] = (uint8_t) (ch >> 16 & 0x1F);
        عارضی[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        عارضی[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        برائے (اشاریہ_مقصد_Write = 0; اشاریہ_مقصد_Write <= حجم(uint32_t) - 1; اشاریہ_مقصد_Write++) {
            اگر (my_ہدف < ہدف_حد_بندی) {
                *(my_ہدف++) = عارضی[اشاریہ_مقصد_Write];
                *(myOffsets++) = انحراف_عدد;
            }
            ورنہ {
                دلائل_2->converter->char_Error_بفر[دلائل_2->converter->char_Error_بفر_لمبائی++] = عارضی[اشاریہ_مقصد_Write];
                *err = U_بفر_حد_سے_تجاوز_ERROR;
            }
        }
        انحراف_عدد = انحراف_عدد + 1 + (عارضی[1] != 0);
    }

    اگر (my_ماخذ < ماخذ_حد_بندی && my_ہدف >= ہدف_حد_بندی && U_SUCCESS(*err)) {
        *err = U_بفر_حد_سے_تجاوز_ERROR;
    }

    دلائل_2->ہدف = (حرف *) my_ہدف;
    دلائل_2->ماخذ = my_ماخذ;
    دلائل_2->offsets = myOffsets;
}

ساکن UChar32 U_CALLCONV
T_UConverter_get_اگلا_UChar_UTF32_BE(UConverter_مقصد_Unicode_دلائل* دلائل_2,
                                   UError_رمز* err)
{
    ثابت uint8_t *my_ماخذ;
    UChar32 myUChar;
    int32_t length;

    my_ماخذ = (ثابت uint8_t *)دلائل_2->ماخذ;
    اگر (my_ماخذ >= (ثابت uint8_t *)دلائل_2->ماخذ_حد_بندی)
    {
        /* no input */
        *err = U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0xffff;
    }

    length = (int32_t)((ثابت uint8_t *)دلائل_2->ماخذ_حد_بندی - my_ماخذ);
    اگر (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(دلائل_2->converter->مقصد_UBytes, my_ماخذ, length);
        دلائل_2->converter->مقصد_ULength = (int8_t)length;
        دلائل_2->ماخذ = (ثابت حرف *)(my_ماخذ + length);
        *err = U_TRUNCATED_CHAR_ملا;
        واپس 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)my_ماخذ[0] << 24)
            | ((UChar32)my_ماخذ[1] << 16)
            | ((UChar32)my_ماخذ[2] << 8)
            | ((UChar32)my_ماخذ[3]);

    دلائل_2->ماخذ = (ثابت حرف *)(my_ماخذ + 4);
    اگر ((uint32_t)myUChar <= زیادہ_سے_زیادہ_UTF && !U_IS_SURROGATE(myUChar)) {
        واپس myUChar;
    }

    uprv_memcpy(دلائل_2->converter->مقصد_UBytes, my_ماخذ, 4);
    دلائل_2->converter->مقصد_ULength = 4;

    *err = U_ILLEGAL_CHAR_ملا;
    واپس 0xffff;
}
U_CDECL_انتہا
ساکن ثابت UConverterImpl _UTF32BEImpl = {
    UCNV_UTF32_BigEndian,

    خالی_اشاریہ,
    خالی_اشاریہ,

    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,

    T_UConverter_مقصد_Unicode_UTF32_BE,
    T_UConverter_مقصد_Unicode_UTF32_BE_انحراف_LOGIC,
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_انحراف_LOGIC,
    T_UConverter_get_اگلا_UChar_UTF32_BE,

    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,
    ucnv_get_Non_Surrogate_Unicode_سیٹ,

    خالی_اشاریہ,
    خالی_اشاریہ
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
ساکن ثابت UConverter_Static_ڈیٹا UTF32_BEStatic_ڈیٹا = {
    حجم(UConverter_Static_ڈیٹا),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, جھوٹ, جھوٹ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_ڈیٹا _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&UTF32_BEStatic_ڈیٹا, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
ساکن خالی U_CALLCONV
T_UConverter_مقصد_Unicode_UTF32_LE(UConverter_مقصد_Unicode_دلائل * دلائل_2,
                                UError_رمز * err)
{
    ثابت غیرعلامتی حرف *my_ماخذ = (غیرعلامتی حرف *) دلائل_2->ماخذ;
    char16_t *my_ہدف = دلائل_2->ہدف;
    ثابت غیرعلامتی حرف *ماخذ_حد_بندی = (غیرعلامتی حرف *) دلائل_2->ماخذ_حد_بندی;
    ثابت char16_t *ہدف_حد_بندی = دلائل_2->ہدف_حد_بندی;
    غیرعلامتی حرف *مقصد_UBytes = دلائل_2->converter->مقصد_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    اگر (دلائل_2->converter->مقصد_ULength > 0 && my_ہدف < ہدف_حد_بندی)
    {
        i = دلائل_2->converter->مقصد_ULength;       /* restore # of bytes consumed */
        دلائل_2->converter->مقصد_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = دلائل_2->converter->مقصد_Unicode_حالت - 1;
        دلائل_2->converter->مقصد_Unicode_حالت = 0;
        جاؤ morebytes;
    }

    جب (my_ماخذ < ماخذ_حد_بندی && my_ہدف < ہدف_حد_بندی)
    {
        i = 0;
        ch = 0;
morebytes:
        جب (i < حجم(uint32_t))
        {
            اگر (my_ماخذ < ماخذ_حد_بندی)
            {
                ch |= ((uint8_t)(*my_ماخذ)) << (i * 8);
                مقصد_UBytes[i++] = (حرف) *(my_ماخذ++);
            }
            ورنہ
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                دلائل_2->converter->مقصد_Unicode_حالت = ch + 1;
                دلائل_2->converter->مقصد_ULength = (int8_t) i;
                جاؤ donefornow;
            }
        }

        اگر (ch <= زیادہ_سے_زیادہ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            اگر (ch <= زیادہ_سے_زیادہ_UCS2) {
                /* fits in 16 bits */
                *(my_ہدف++) = (char16_t) ch;
            }
            ورنہ {
                /* write out the surrogates */
                *(my_ہدف++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                اگر (my_ہدف < ہدف_حد_بندی) {
                    *(my_ہدف++) = (char16_t)ch;
                }
                ورنہ {
                    /* Put in overflow buffer (not handled here) */
                    دلائل_2->converter->UChar_Error_بفر[0] = (char16_t) ch;
                    دلائل_2->converter->UChar_Error_بفر_لمبائی = 1;
                    *err = U_بفر_حد_سے_تجاوز_ERROR;
                    توڑو;
                }
            }
        }
        ورنہ {
            دلائل_2->converter->مقصد_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_ملا;
            توڑو;
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

ساکن خالی U_CALLCONV
T_UConverter_مقصد_Unicode_UTF32_LE_انحراف_LOGIC(UConverter_مقصد_Unicode_دلائل * دلائل_2,
                                             UError_رمز * err)
{
    ثابت غیرعلامتی حرف *my_ماخذ = (غیرعلامتی حرف *) دلائل_2->ماخذ;
    char16_t *my_ہدف = دلائل_2->ہدف;
    int32_t *myOffsets = دلائل_2->offsets;
    ثابت غیرعلامتی حرف *ماخذ_حد_بندی = (غیرعلامتی حرف *) دلائل_2->ماخذ_حد_بندی;
    ثابت char16_t *ہدف_حد_بندی = دلائل_2->ہدف_حد_بندی;
    غیرعلامتی حرف *مقصد_UBytes = دلائل_2->converter->مقصد_UBytes;
    uint32_t ch, i;
    int32_t انحراف_عدد = 0;

    /* Restore state of current sequence */
    اگر (دلائل_2->converter->مقصد_ULength > 0 && my_ہدف < ہدف_حد_بندی)
    {
        i = دلائل_2->converter->مقصد_ULength;       /* restore # of bytes consumed */
        دلائل_2->converter->مقصد_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = دلائل_2->converter->مقصد_Unicode_حالت - 1;
        دلائل_2->converter->مقصد_Unicode_حالت = 0;
        جاؤ morebytes;
    }

    جب (my_ماخذ < ماخذ_حد_بندی && my_ہدف < ہدف_حد_بندی)
    {
        i = 0;
        ch = 0;
morebytes:
        جب (i < حجم(uint32_t))
        {
            اگر (my_ماخذ < ماخذ_حد_بندی)
            {
                ch |= ((uint8_t)(*my_ماخذ)) << (i * 8);
                مقصد_UBytes[i++] = (حرف) *(my_ماخذ++);
            }
            ورنہ
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                دلائل_2->converter->مقصد_Unicode_حالت = ch + 1;
                دلائل_2->converter->مقصد_ULength = (int8_t) i;
                جاؤ donefornow;
            }
        }

        اگر (ch <= زیادہ_سے_زیادہ_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            اگر (ch <= زیادہ_سے_زیادہ_UCS2) 
            {
                /* fits in 16 bits */
                *(my_ہدف++) = (char16_t) ch;
                *(myOffsets++) = انحراف_عدد;
            }
            ورنہ {
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
                    /* Put in overflow buffer (not handled here) */
                    دلائل_2->converter->UChar_Error_بفر[0] = (char16_t) ch;
                    دلائل_2->converter->UChar_Error_بفر_لمبائی = 1;
                    *err = U_بفر_حد_سے_تجاوز_ERROR;
                    توڑو;
                }
            }
        }
        ورنہ
        {
            دلائل_2->converter->مقصد_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_ملا;
            توڑو;
        }
        انحراف_عدد += i;
    }

donefornow:
    اگر (my_ماخذ < ماخذ_حد_بندی && my_ہدف >= ہدف_حد_بندی && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_بفر_حد_سے_تجاوز_ERROR;
    }

    دلائل_2->ہدف = my_ہدف;
    دلائل_2->ماخذ = (ثابت حرف *) my_ماخذ;
    دلائل_2->offsets = myOffsets;
}

ساکن خالی U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverter_From_Unicode_دلائل * دلائل_2,
                                  UError_رمز * err)
{
    ثابت char16_t *my_ماخذ = دلائل_2->ماخذ;
    غیرعلامتی حرف *my_ہدف;
    ثابت char16_t *ماخذ_حد_بندی = دلائل_2->ماخذ_حد_بندی;
    ثابت غیرعلامتی حرف *ہدف_حد_بندی = (غیرعلامتی حرف *) دلائل_2->ہدف_حد_بندی;
    UChar32 ch, ch2;
    غیرعلامتی صحیح اشاریہ_مقصد_Write;
    غیرعلامتی حرف عارضی[حجم(uint32_t)];

    اگر(my_ماخذ >= ماخذ_حد_بندی) {
        /* no input, nothing to do */
        واپس;
    }

    /* write the BOM if necessary */
    اگر(دلائل_2->converter->from_Unicode_حالت==UCNV_NEED_مقصد_WRITE_BOM) {
        ساکن ثابت حرف bom[]={ (حرف)0xffu, (حرف)0xfeu, 0, 0 };
        ucnv_from_UWrite_بائٹیں(دلائل_2->converter,
                             bom, 4,
                             &دلائل_2->ہدف, دلائل_2->ہدف_حد_بندی,
                             &دلائل_2->offsets, -1,
                             err);
        دلائل_2->converter->from_Unicode_حالت=0;
    }

    my_ہدف = (غیرعلامتی حرف *) دلائل_2->ہدف;
    عارضی[3] = 0;

    اگر (دلائل_2->converter->fromUChar32)
    {
        ch = دلائل_2->converter->fromUChar32;
        دلائل_2->converter->fromUChar32 = 0;
        جاؤ lowsurogate;
    }

    جب (my_ماخذ < ماخذ_حد_بندی && my_ہدف < ہدف_حد_بندی)
    {
        ch = *(my_ماخذ++);

        اگر (U16_IS_SURROGATE(ch)) {
            اگر (U16_IS_LEAD(ch))
            {
lowsurogate:
                اگر (my_ماخذ < ماخذ_حد_بندی)
                {
                    ch2 = *my_ماخذ;
                    اگر (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_بلند_آغاز) << HALF_سرکاؤ) + ch2 + SURROGATE_کم_بنیاد;
                        my_ماخذ++;
                    }
                    ورنہ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        دلائل_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ملا;
                        توڑو;
                    }
                }
                ورنہ {
                    /* ran out of source */
                    دلائل_2->converter->fromUChar32 = ch;
                    اگر (دلائل_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_ملا;
                    }
                    توڑو;
                }
            }
            ورنہ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                دلائل_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_ملا;
                توڑو;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        عارضی[2] = (uint8_t) (ch >> 16 & 0x1F);
        عارضی[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        عارضی[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        برائے (اشاریہ_مقصد_Write = 0; اشاریہ_مقصد_Write <= حجم(uint32_t) - 1; اشاریہ_مقصد_Write++)
        {
            اگر (my_ہدف < ہدف_حد_بندی)
            {
                *(my_ہدف++) = عارضی[اشاریہ_مقصد_Write];
            }
            ورنہ
            {
                دلائل_2->converter->char_Error_بفر[دلائل_2->converter->char_Error_بفر_لمبائی++] = عارضی[اشاریہ_مقصد_Write];
                *err = U_بفر_حد_سے_تجاوز_ERROR;
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

ساکن خالی U_CALLCONV
T_UConverter_from_Unicode_UTF32_LE_انحراف_LOGIC(UConverter_From_Unicode_دلائل * دلائل_2,
                                               UError_رمز * err)
{
    ثابت char16_t *my_ماخذ = دلائل_2->ماخذ;
    غیرعلامتی حرف *my_ہدف;
    int32_t *myOffsets;
    ثابت char16_t *ماخذ_حد_بندی = دلائل_2->ماخذ_حد_بندی;
    ثابت غیرعلامتی حرف *ہدف_حد_بندی = (غیرعلامتی حرف *) دلائل_2->ہدف_حد_بندی;
    UChar32 ch, ch2;
    غیرعلامتی صحیح اشاریہ_مقصد_Write;
    غیرعلامتی حرف عارضی[حجم(uint32_t)];
    int32_t انحراف_عدد = 0;

    اگر(my_ماخذ >= ماخذ_حد_بندی) {
        /* no input, nothing to do */
        واپس;
    }

    /* write the BOM if necessary */
    اگر(دلائل_2->converter->from_Unicode_حالت==UCNV_NEED_مقصد_WRITE_BOM) {
        ساکن ثابت حرف bom[]={ (حرف)0xffu, (حرف)0xfeu, 0, 0 };
        ucnv_from_UWrite_بائٹیں(دلائل_2->converter,
                             bom, 4,
                             &دلائل_2->ہدف, دلائل_2->ہدف_حد_بندی,
                             &دلائل_2->offsets, -1,
                             err);
        دلائل_2->converter->from_Unicode_حالت=0;
    }

    my_ہدف = (غیرعلامتی حرف *) دلائل_2->ہدف;
    myOffsets = دلائل_2->offsets;
    عارضی[3] = 0;

    اگر (دلائل_2->converter->fromUChar32)
    {
        ch = دلائل_2->converter->fromUChar32;
        دلائل_2->converter->fromUChar32 = 0;
        جاؤ lowsurogate;
    }

    جب (my_ماخذ < ماخذ_حد_بندی && my_ہدف < ہدف_حد_بندی)
    {
        ch = *(my_ماخذ++);

        اگر (U16_IS_SURROGATE(ch)) {
            اگر (U16_IS_LEAD(ch))
            {
lowsurogate:
                اگر (my_ماخذ < ماخذ_حد_بندی)
                {
                    ch2 = *my_ماخذ;
                    اگر (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_بلند_آغاز) << HALF_سرکاؤ) + ch2 + SURROGATE_کم_بنیاد;
                        my_ماخذ++;
                    }
                    ورنہ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        دلائل_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ملا;
                        توڑو;
                    }
                }
                ورنہ {
                    /* ran out of source */
                    دلائل_2->converter->fromUChar32 = ch;
                    اگر (دلائل_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_ملا;
                    }
                    توڑو;
                }
            }
            ورنہ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                دلائل_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_ملا;
                توڑو;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        عارضی[2] = (uint8_t) (ch >> 16 & 0x1F);
        عارضی[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        عارضی[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        برائے (اشاریہ_مقصد_Write = 0; اشاریہ_مقصد_Write <= حجم(uint32_t) - 1; اشاریہ_مقصد_Write++)
        {
            اگر (my_ہدف < ہدف_حد_بندی)
            {
                *(my_ہدف++) = عارضی[اشاریہ_مقصد_Write];
                *(myOffsets++) = انحراف_عدد;
            }
            ورنہ
            {
                دلائل_2->converter->char_Error_بفر[دلائل_2->converter->char_Error_بفر_لمبائی++] = عارضی[اشاریہ_مقصد_Write];
                *err = U_بفر_حد_سے_تجاوز_ERROR;
            }
        }
        انحراف_عدد = انحراف_عدد + 1 + (عارضی[2] != 0);
    }

    اگر (my_ماخذ < ماخذ_حد_بندی && my_ہدف >= ہدف_حد_بندی && U_SUCCESS(*err))
    {
        *err = U_بفر_حد_سے_تجاوز_ERROR;
    }

    دلائل_2->ہدف = (حرف *) my_ہدف;
    دلائل_2->ماخذ = my_ماخذ;
    دلائل_2->offsets = myOffsets;
}

ساکن UChar32 U_CALLCONV
T_UConverter_get_اگلا_UChar_UTF32_LE(UConverter_مقصد_Unicode_دلائل* دلائل_2,
                                   UError_رمز* err)
{
    ثابت uint8_t *my_ماخذ;
    UChar32 myUChar;
    int32_t length;

    my_ماخذ = (ثابت uint8_t *)دلائل_2->ماخذ;
    اگر (my_ماخذ >= (ثابت uint8_t *)دلائل_2->ماخذ_حد_بندی)
    {
        /* no input */
        *err = U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0xffff;
    }

    length = (int32_t)((ثابت uint8_t *)دلائل_2->ماخذ_حد_بندی - my_ماخذ);
    اگر (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(دلائل_2->converter->مقصد_UBytes, my_ماخذ, length);
        دلائل_2->converter->مقصد_ULength = (int8_t)length;
        دلائل_2->ماخذ = (ثابت حرف *)(my_ماخذ + length);
        *err = U_TRUNCATED_CHAR_ملا;
        واپس 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)my_ماخذ[3] << 24)
            | ((UChar32)my_ماخذ[2] << 16)
            | ((UChar32)my_ماخذ[1] << 8)
            | ((UChar32)my_ماخذ[0]);

    دلائل_2->ماخذ = (ثابت حرف *)(my_ماخذ + 4);
    اگر ((uint32_t)myUChar <= زیادہ_سے_زیادہ_UTF && !U_IS_SURROGATE(myUChar)) {
        واپس myUChar;
    }

    uprv_memcpy(دلائل_2->converter->مقصد_UBytes, my_ماخذ, 4);
    دلائل_2->converter->مقصد_ULength = 4;

    *err = U_ILLEGAL_CHAR_ملا;
    واپس 0xffff;
}
U_CDECL_انتہا
ساکن ثابت UConverterImpl _UTF32LEImpl = {
    UCNV_UTF32_LittleEndian,

    خالی_اشاریہ,
    خالی_اشاریہ,

    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,

    T_UConverter_مقصد_Unicode_UTF32_LE,
    T_UConverter_مقصد_Unicode_UTF32_LE_انحراف_LOGIC,
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_انحراف_LOGIC,
    T_UConverter_get_اگلا_UChar_UTF32_LE,

    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,
    ucnv_get_Non_Surrogate_Unicode_سیٹ,

    خالی_اشاریہ,
    خالی_اشاریہ
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
ساکن ثابت UConverter_Static_ڈیٹا UTF32_LEStatic_ڈیٹا = {
    حجم(UConverter_Static_ڈیٹا),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, جھوٹ, جھوٹ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_ڈیٹا _UTF32LEData =
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&UTF32_LEStatic_ڈیٹا, &_UTF32LEImpl);

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
ساکن خالی U_CALLCONV
_UTF32Reset(UConverter *cnv, UConverterResetChoice choice) {
    اگر(choice<=UCNV_RESET_مقصد_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->منوال=0;
    }
    اگر(choice!=UCNV_RESET_مقصد_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->from_Unicode_حالت=UCNV_NEED_مقصد_WRITE_BOM;
    }
}

ساکن خالی U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverter_بھرو_دلائل *p_دلائل,
           UError_رمز *p_Error_رمز) {
    (خالی)p_دلائل;
    (خالی)p_Error_رمز;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

ساکن ثابت حرف utf32BOM[8]={ 0, 0, (حرف)0xfeu, (حرف)0xffu, (حرف)0xffu, (حرف)0xfeu, 0, 0 };

ساکن خالی U_CALLCONV
UTF32_مقصد_Unicode_With_Offsets(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                           UError_رمز *p_Error_رمز) {
    UConverter *cnv=p_دلائل->converter;
    ثابت حرف *ماخذ=p_دلائل->ماخذ;
    ثابت حرف *ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
    int32_t *offsets=p_دلائل->offsets;

    int32_t state, انحراف_Delta;
    حرف b;

    state=cnv->منوال;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    انحراف_Delta=0;

    جب(ماخذ<ماخذ_حد_بندی && U_SUCCESS(*p_Error_رمز)) {
        انتخاب(state) {
        حالت 0:
            b=*ماخذ;
            اگر(b==0) {
                state=1; /* could be 00 00 FE FF */
            } ورنہ اگر(b==(حرف)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } ورنہ {
                state=8; /* default to UTF-32BE */
                جاری;
            }
            ++ماخذ;
            توڑو;
        حالت 1:
        حالت 2:
        حالت 3:
        حالت 5:
        حالت 6:
        حالت 7:
            اگر(*ماخذ==utf32BOM[state]) {
                ++state;
                ++ماخذ;
                اگر(state==4) {
                    state=8; /* detect UTF-32BE */
                    انحراف_Delta=(int32_t)(ماخذ-p_دلائل->ماخذ);
                } ورنہ اگر(state==8) {
                    state=9; /* detect UTF-32LE */
                    انحراف_Delta=(int32_t)(ماخذ-p_دلائل->ماخذ);
                }
            } ورنہ {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(ماخذ-p_دلائل->ماخذ); /* number of bytes from this buffer */

                /* reset the source */
                ماخذ=p_دلائل->ماخذ;

                اگر(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } ورنہ {
                    UBool oldFlush=p_دلائل->flush;

                    /* some of the bytes are from a previous buffer, replay those first */
                    p_دلائل->ماخذ=utf32BOM+(state&4); /* select the correct BOM */
                    p_دلائل->ماخذ_حد_بندی=p_دلائل->ماخذ+((state&3)-count); /* replay previous bytes */
                    p_دلائل->flush=جھوٹ; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_مقصد_Unicode_UTF32_BE(p_دلائل, p_Error_رمز);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    p_دلائل->ماخذ_حد_بندی=ماخذ_حد_بندی;
                    p_دلائل->flush=oldFlush;
                }
                state=8;
                جاری;
            }
            توڑو;
        حالت 8:
            /* call UTF-32BE */
            p_دلائل->ماخذ=ماخذ;
            اگر(offsets==خالی_اشاریہ) {
                T_UConverter_مقصد_Unicode_UTF32_BE(p_دلائل, p_Error_رمز);
            } ورنہ {
                T_UConverter_مقصد_Unicode_UTF32_BE_انحراف_LOGIC(p_دلائل, p_Error_رمز);
            }
            ماخذ=p_دلائل->ماخذ;
            توڑو;
        حالت 9:
            /* call UTF-32LE */
            p_دلائل->ماخذ=ماخذ;
            اگر(offsets==خالی_اشاریہ) {
                T_UConverter_مقصد_Unicode_UTF32_LE(p_دلائل, p_Error_رمز);
            } ورنہ {
                T_UConverter_مقصد_Unicode_UTF32_LE_انحراف_LOGIC(p_دلائل, p_Error_رمز);
            }
            ماخذ=p_دلائل->ماخذ;
            توڑو;
        طےشدہ:
            توڑو; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    اگر(offsets!=خالی_اشاریہ && انحراف_Delta!=0) {
        int32_t *offsets_حد_بندی=p_دلائل->offsets;
        جب(offsets<offsets_حد_بندی) {
            *offsets++ += انحراف_Delta;
        }
    }

    p_دلائل->ماخذ=ماخذ;

    اگر(ماخذ==ماخذ_حد_بندی && p_دلائل->flush) {
        /* handle truncated input */
        انتخاب(state) {
        حالت 0:
            توڑو; /* no input at all, nothing to do */
        حالت 8:
            T_UConverter_مقصد_Unicode_UTF32_BE(p_دلائل, p_Error_رمز);
            توڑو;
        حالت 9:
            T_UConverter_مقصد_Unicode_UTF32_LE(p_دلائل, p_Error_رمز);
            توڑو;
        طےشدہ:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            p_دلائل->ماخذ=utf32BOM+(state&4); /* select the correct BOM */
            p_دلائل->ماخذ_حد_بندی=p_دلائل->ماخذ+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_مقصد_Unicode_UTF32_BE(p_دلائل, p_Error_رمز);
            p_دلائل->ماخذ=ماخذ;
            p_دلائل->ماخذ_حد_بندی=ماخذ_حد_بندی;
            state=8;
            توڑو;
        }
    }

    cnv->منوال=state;
}

ساکن UChar32 U_CALLCONV
UTF32_Get_اگلا_UChar(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                   UError_رمز *p_Error_رمز) {
    انتخاب(p_دلائل->converter->منوال) {
    حالت 8:
        واپس T_UConverter_get_اگلا_UChar_UTF32_BE(p_دلائل, p_Error_رمز);
    حالت 9:
        واپس T_UConverter_get_اگلا_UChar_UTF32_LE(p_دلائل, p_Error_رمز);
    طےشدہ:
        واپس UCNV_GET_اگلا_UCHAR_USE_مقصد_U;
    }
}
U_CDECL_انتہا
ساکن ثابت UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    خالی_اشاریہ,
    خالی_اشاریہ,

    _UTF32Open,
    خالی_اشاریہ,
    _UTF32Reset,

    UTF32_مقصد_Unicode_With_Offsets,
    UTF32_مقصد_Unicode_With_Offsets,
#اگر U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_انحراف_LOGIC,
#ورنہ
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_انحراف_LOGIC,
#اگر_ختم
    UTF32_Get_اگلا_UChar,

    خالی_اشاریہ, /* ### TODO implement getStarters for all Unicode encodings?! */
    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,
    ucnv_get_Non_Surrogate_Unicode_سیٹ,

    خالی_اشاریہ,
    خالی_اشاریہ
};

/* The 1236 CCSID refers to any version of Unicode with a BOM sensitive endianness of UTF-32 */
ساکن ثابت UConverter_Static_ڈیٹا UTF32_Static_ڈیٹا = {
    حجم(UConverter_Static_ڈیٹا),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#اگر U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#ورنہ
    { 0xfd, 0xff, 0, 0 }, 4,
#اگر_ختم
    جھوٹ, جھوٹ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_ڈیٹا UTF32_ڈیٹا = 
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&UTF32_Static_ڈیٹا, &_UTF32Impl);

#اگر_ختم
