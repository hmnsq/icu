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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ضمن "unicode/ucnv.h"
#ضمن "unicode/utf.h"
#ضمن "ucnv_bld.h"
#ضمن "ucnv_cnv.h"
#ضمن "cmemory.h"

#تعریف_کرو ودھ_توں_ودھ_UCS2            0x0000FFFF
#تعریف_کرو ودھ_توں_ودھ_UTF             0x0010FFFF
#تعریف_کرو HALF_کھسکاؤ              10
#تعریف_کرو HALF_ادھار               0x0010000
#تعریف_کرو HALF_نقاب               0x3FF
#تعریف_کرو SURROGATE_بلند_شروع    0xD800
#تعریف_کرو SURROGATE_کم_شروع     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#تعریف_کرو SURROGATE_کم_ادھار      9216

تعداد {
    UCNV_NEED_منزل_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
T_UConverter_منزل_Unicode_UTF32_BE(UConverter_منزل_Unicode_دلیلاں * دلیلاں_2,
                                UError_کوڈ * err)
{
    ثابت بنانشانی حرف *my_سروت = (بنانشانی حرف *) دلیلاں_2->سروت;
    char16_t *my_نشانہ = دلیلاں_2->نشانہ;
    ثابت بنانشانی حرف *سروت_حد_بندی = (بنانشانی حرف *) دلیلاں_2->سروت_حد_بندی;
    ثابت char16_t *نشانہ_حد_بندی = دلیلاں_2->نشانہ_حد_بندی;
    بنانشانی حرف *منزل_UBytes = دلیلاں_2->converter->منزل_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    إذا (دلیلاں_2->converter->منزل_ULength > 0 && my_نشانہ < نشانہ_حد_بندی) {
        i = دلیلاں_2->converter->منزل_ULength;       /* restore # of bytes consumed */
        دلیلاں_2->converter->منزل_ULength = 0;

        ch = دلیلاں_2->converter->منزل_Unicode_حالت - 1;/*Stores the previously calculated ch from a previous call*/
        دلیلاں_2->converter->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }

    طالما (my_سروت < سروت_حد_بندی && my_نشانہ < نشانہ_حد_بندی) {
        i = 0;
        ch = 0;
morebytes:
        طالما (i < حجم(uint32_t)) {
            إذا (my_سروت < سروت_حد_بندی) {
                ch = (ch << 8) | (uint8_t)(*my_سروت);
                منزل_UBytes[i++] = (حرف) *(my_سروت++);
            }
            وإلا {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                دلیلاں_2->converter->منزل_Unicode_حالت = ch + 1;
                دلیلاں_2->converter->منزل_ULength = (int8_t) i;
                اذهب donefornow;
            }
        }

        إذا (ch <= ودھ_توں_ودھ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            إذا (ch <= ودھ_توں_ودھ_UCS2) 
            {
                /* fits in 16 bits */
                *(my_نشانہ++) = (char16_t) ch;
            }
            وإلا {
                /* write out the surrogates */
                *(my_نشانہ++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                إذا (my_نشانہ < نشانہ_حد_بندی) {
                    *(my_نشانہ++) = (char16_t)ch;
                }
                وإلا {
                    /* Put in overflow buffer (not handled here) */
                    دلیلاں_2->converter->UChar_Error_بفر[0] = (char16_t) ch;
                    دلیلاں_2->converter->UChar_Error_بفر_لمائی = 1;
                    *err = U_بفر_حد_توں_ودھ_ERROR;
                    اكسر;
                }
            }
        }
        وإلا {
            دلیلاں_2->converter->منزل_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_لبھیا;
            اكسر;
        }
    }

donefornow:
    إذا (my_سروت < سروت_حد_بندی && my_نشانہ >= نشانہ_حد_بندی && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_بفر_حد_توں_ودھ_ERROR;
    }

    دلیلاں_2->نشانہ = my_نشانہ;
    دلیلاں_2->سروت = (ثابت حرف *) my_سروت;
}

ساكن فراغ U_CALLCONV
T_UConverter_منزل_Unicode_UTF32_BE_وتھ_LOGIC(UConverter_منزل_Unicode_دلیلاں * دلیلاں_2,
                                             UError_کوڈ * err)
{
    ثابت بنانشانی حرف *my_سروت = (بنانشانی حرف *) دلیلاں_2->سروت;
    char16_t *my_نشانہ = دلیلاں_2->نشانہ;
    int32_t *myOffsets = دلیلاں_2->offsets;
    ثابت بنانشانی حرف *سروت_حد_بندی = (بنانشانی حرف *) دلیلاں_2->سروت_حد_بندی;
    ثابت char16_t *نشانہ_حد_بندی = دلیلاں_2->نشانہ_حد_بندی;
    بنانشانی حرف *منزل_UBytes = دلیلاں_2->converter->منزل_UBytes;
    uint32_t ch, i;
    int32_t وتھ_عدد = 0;

    /* Restore state of current sequence */
    إذا (دلیلاں_2->converter->منزل_ULength > 0 && my_نشانہ < نشانہ_حد_بندی) {
        i = دلیلاں_2->converter->منزل_ULength;       /* restore # of bytes consumed */
        دلیلاں_2->converter->منزل_ULength = 0;

        ch = دلیلاں_2->converter->منزل_Unicode_حالت - 1;/*Stores the previously calculated ch from a previous call*/
        دلیلاں_2->converter->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }

    طالما (my_سروت < سروت_حد_بندی && my_نشانہ < نشانہ_حد_بندی) {
        i = 0;
        ch = 0;
morebytes:
        طالما (i < حجم(uint32_t)) {
            إذا (my_سروت < سروت_حد_بندی) {
                ch = (ch << 8) | (uint8_t)(*my_سروت);
                منزل_UBytes[i++] = (حرف) *(my_سروت++);
            }
            وإلا {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                دلیلاں_2->converter->منزل_Unicode_حالت = ch + 1;
                دلیلاں_2->converter->منزل_ULength = (int8_t) i;
                اذهب donefornow;
            }
        }

        إذا (ch <= ودھ_توں_ودھ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            إذا (ch <= ودھ_توں_ودھ_UCS2) {
                /* fits in 16 bits */
                *(my_نشانہ++) = (char16_t) ch;
                *(myOffsets++) = وتھ_عدد;
            }
            وإلا {
                /* write out the surrogates */
                *(my_نشانہ++) = U16_LEAD(ch);
                *myOffsets++ = وتھ_عدد;
                ch = U16_TRAIL(ch);
                إذا (my_نشانہ < نشانہ_حد_بندی)
                {
                    *(my_نشانہ++) = (char16_t)ch;
                    *(myOffsets++) = وتھ_عدد;
                }
                وإلا {
                    /* Put in overflow buffer (not handled here) */
                    دلیلاں_2->converter->UChar_Error_بفر[0] = (char16_t) ch;
                    دلیلاں_2->converter->UChar_Error_بفر_لمائی = 1;
                    *err = U_بفر_حد_توں_ودھ_ERROR;
                    اكسر;
                }
            }
        }
        وإلا {
            دلیلاں_2->converter->منزل_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_لبھیا;
            اكسر;
        }
        وتھ_عدد += i;
    }

donefornow:
    إذا (my_سروت < سروت_حد_بندی && my_نشانہ >= نشانہ_حد_بندی && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_بفر_حد_توں_ودھ_ERROR;
    }

    دلیلاں_2->نشانہ = my_نشانہ;
    دلیلاں_2->سروت = (ثابت حرف *) my_سروت;
    دلیلاں_2->offsets = myOffsets;
}

ساكن فراغ U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverter_From_Unicode_دلیلاں * دلیلاں_2,
                                  UError_کوڈ * err)
{
    ثابت char16_t *my_سروت = دلیلاں_2->سروت;
    بنانشانی حرف *my_نشانہ;
    ثابت char16_t *سروت_حد_بندی = دلیلاں_2->سروت_حد_بندی;
    ثابت بنانشانی حرف *نشانہ_حد_بندی = (بنانشانی حرف *) دلیلاں_2->نشانہ_حد_بندی;
    UChar32 ch, ch2;
    بنانشانی صحيح اشاریہ_منزل_Write;
    بنانشانی حرف عارضی[حجم(uint32_t)];

    إذا(my_سروت >= سروت_حد_بندی) {
        /* no input, nothing to do */
        أرجع;
    }

    /* write the BOM if necessary */
    إذا(دلیلاں_2->converter->from_Unicode_حالت==UCNV_NEED_منزل_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ 0, 0, (حرف)0xfeu, (حرف)0xffu };
        ucnv_from_UWrite_بائٹاں(دلیلاں_2->converter,
                             bom, 4,
                             &دلیلاں_2->نشانہ, دلیلاں_2->نشانہ_حد_بندی,
                             &دلیلاں_2->offsets, -1,
                             err);
        دلیلاں_2->converter->from_Unicode_حالت=0;
    }

    my_نشانہ = (بنانشانی حرف *) دلیلاں_2->نشانہ;
    عارضی[0] = 0;

    إذا (دلیلاں_2->converter->fromUChar32) {
        ch = دلیلاں_2->converter->fromUChar32;
        دلیلاں_2->converter->fromUChar32 = 0;
        اذهب lowsurogate;
    }

    طالما (my_سروت < سروت_حد_بندی && my_نشانہ < نشانہ_حد_بندی) {
        ch = *(my_سروت++);

        إذا (U_IS_SURROGATE(ch)) {
            إذا (U_IS_LEAD(ch)) {
lowsurogate:
                إذا (my_سروت < سروت_حد_بندی) {
                    ch2 = *my_سروت;
                    إذا (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_بلند_شروع) << HALF_کھسکاؤ) + ch2 + SURROGATE_کم_ادھار;
                        my_سروت++;
                    }
                    وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        دلیلاں_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_لبھیا;
                        اكسر;
                    }
                }
                وإلا {
                    /* ran out of source */
                    دلیلاں_2->converter->fromUChar32 = ch;
                    إذا (دلیلاں_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_لبھیا;
                    }
                    اكسر;
                }
            }
            وإلا {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                دلیلاں_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_لبھیا;
                اكسر;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        عارضی[1] = (uint8_t) (ch >> 16 & 0x1F);
        عارضی[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        عارضی[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        لكل (اشاریہ_منزل_Write = 0; اشاریہ_منزل_Write <= حجم(uint32_t) - 1; اشاریہ_منزل_Write++) {
            إذا (my_نشانہ < نشانہ_حد_بندی) {
                *(my_نشانہ++) = عارضی[اشاریہ_منزل_Write];
            }
            وإلا {
                دلیلاں_2->converter->char_Error_بفر[دلیلاں_2->converter->char_Error_بفر_لمائی++] = عارضی[اشاریہ_منزل_Write];
                *err = U_بفر_حد_توں_ودھ_ERROR;
            }
        }
    }

    إذا (my_سروت < سروت_حد_بندی && my_نشانہ >= نشانہ_حد_بندی && U_SUCCESS(*err)) {
        *err = U_بفر_حد_توں_ودھ_ERROR;
    }

    دلیلاں_2->نشانہ = (حرف *) my_نشانہ;
    دلیلاں_2->سروت = my_سروت;
}

ساكن فراغ U_CALLCONV
T_UConverter_from_Unicode_UTF32_BE_وتھ_LOGIC(UConverter_From_Unicode_دلیلاں * دلیلاں_2,
                                               UError_کوڈ * err)
{
    ثابت char16_t *my_سروت = دلیلاں_2->سروت;
    بنانشانی حرف *my_نشانہ;
    int32_t *myOffsets;
    ثابت char16_t *سروت_حد_بندی = دلیلاں_2->سروت_حد_بندی;
    ثابت بنانشانی حرف *نشانہ_حد_بندی = (بنانشانی حرف *) دلیلاں_2->نشانہ_حد_بندی;
    UChar32 ch, ch2;
    int32_t وتھ_عدد = 0;
    بنانشانی صحيح اشاریہ_منزل_Write;
    بنانشانی حرف عارضی[حجم(uint32_t)];

    إذا(my_سروت >= سروت_حد_بندی) {
        /* no input, nothing to do */
        أرجع;
    }

    /* write the BOM if necessary */
    إذا(دلیلاں_2->converter->from_Unicode_حالت==UCNV_NEED_منزل_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ 0, 0, (حرف)0xfeu, (حرف)0xffu };
        ucnv_from_UWrite_بائٹاں(دلیلاں_2->converter,
                             bom, 4,
                             &دلیلاں_2->نشانہ, دلیلاں_2->نشانہ_حد_بندی,
                             &دلیلاں_2->offsets, -1,
                             err);
        دلیلاں_2->converter->from_Unicode_حالت=0;
    }

    my_نشانہ = (بنانشانی حرف *) دلیلاں_2->نشانہ;
    myOffsets = دلیلاں_2->offsets;
    عارضی[0] = 0;

    إذا (دلیلاں_2->converter->fromUChar32) {
        ch = دلیلاں_2->converter->fromUChar32;
        دلیلاں_2->converter->fromUChar32 = 0;
        اذهب lowsurogate;
    }

    طالما (my_سروت < سروت_حد_بندی && my_نشانہ < نشانہ_حد_بندی) {
        ch = *(my_سروت++);

        إذا (U_IS_SURROGATE(ch)) {
            إذا (U_IS_LEAD(ch)) {
lowsurogate:
                إذا (my_سروت < سروت_حد_بندی) {
                    ch2 = *my_سروت;
                    إذا (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_بلند_شروع) << HALF_کھسکاؤ) + ch2 + SURROGATE_کم_ادھار;
                        my_سروت++;
                    }
                    وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        دلیلاں_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_لبھیا;
                        اكسر;
                    }
                }
                وإلا {
                    /* ran out of source */
                    دلیلاں_2->converter->fromUChar32 = ch;
                    إذا (دلیلاں_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_لبھیا;
                    }
                    اكسر;
                }
            }
            وإلا {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                دلیلاں_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_لبھیا;
                اكسر;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        عارضی[1] = (uint8_t) (ch >> 16 & 0x1F);
        عارضی[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        عارضی[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        لكل (اشاریہ_منزل_Write = 0; اشاریہ_منزل_Write <= حجم(uint32_t) - 1; اشاریہ_منزل_Write++) {
            إذا (my_نشانہ < نشانہ_حد_بندی) {
                *(my_نشانہ++) = عارضی[اشاریہ_منزل_Write];
                *(myOffsets++) = وتھ_عدد;
            }
            وإلا {
                دلیلاں_2->converter->char_Error_بفر[دلیلاں_2->converter->char_Error_بفر_لمائی++] = عارضی[اشاریہ_منزل_Write];
                *err = U_بفر_حد_توں_ودھ_ERROR;
            }
        }
        وتھ_عدد = وتھ_عدد + 1 + (عارضی[1] != 0);
    }

    إذا (my_سروت < سروت_حد_بندی && my_نشانہ >= نشانہ_حد_بندی && U_SUCCESS(*err)) {
        *err = U_بفر_حد_توں_ودھ_ERROR;
    }

    دلیلاں_2->نشانہ = (حرف *) my_نشانہ;
    دلیلاں_2->سروت = my_سروت;
    دلیلاں_2->offsets = myOffsets;
}

ساكن UChar32 U_CALLCONV
T_UConverter_get_اگلا_UChar_UTF32_BE(UConverter_منزل_Unicode_دلیلاں* دلیلاں_2,
                                   UError_کوڈ* err)
{
    ثابت uint8_t *my_سروت;
    UChar32 myUChar;
    int32_t length;

    my_سروت = (ثابت uint8_t *)دلیلاں_2->سروت;
    إذا (my_سروت >= (ثابت uint8_t *)دلیلاں_2->سروت_حد_بندی)
    {
        /* no input */
        *err = U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    length = (int32_t)((ثابت uint8_t *)دلیلاں_2->سروت_حد_بندی - my_سروت);
    إذا (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(دلیلاں_2->converter->منزل_UBytes, my_سروت, length);
        دلیلاں_2->converter->منزل_ULength = (int8_t)length;
        دلیلاں_2->سروت = (ثابت حرف *)(my_سروت + length);
        *err = U_TRUNCATED_CHAR_لبھیا;
        أرجع 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)my_سروت[0] << 24)
            | ((UChar32)my_سروت[1] << 16)
            | ((UChar32)my_سروت[2] << 8)
            | ((UChar32)my_سروت[3]);

    دلیلاں_2->سروت = (ثابت حرف *)(my_سروت + 4);
    إذا ((uint32_t)myUChar <= ودھ_توں_ودھ_UTF && !U_IS_SURROGATE(myUChar)) {
        أرجع myUChar;
    }

    uprv_memcpy(دلیلاں_2->converter->منزل_UBytes, my_سروت, 4);
    دلیلاں_2->converter->منزل_ULength = 4;

    *err = U_ILLEGAL_CHAR_لبھیا;
    أرجع 0xffff;
}
U_CDECL_سرا
ساكن ثابت UConverterImpl _UTF32BEImpl = {
    UCNV_UTF32_BigEndian,

    خالی_سوچک,
    خالی_سوچک,

    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,

    T_UConverter_منزل_Unicode_UTF32_BE,
    T_UConverter_منزل_Unicode_UTF32_BE_وتھ_LOGIC,
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_وتھ_LOGIC,
    T_UConverter_get_اگلا_UChar_UTF32_BE,

    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,
    ucnv_get_Non_Surrogate_Unicode_سیٹ,

    خالی_سوچک,
    خالی_سوچک
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
ساكن ثابت UConverter_Static_انکڑے UTF32_BEStatic_انکڑے = {
    حجم(UConverter_Static_انکڑے),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_انکڑے _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_انکڑے_INITIALIZER(&UTF32_BEStatic_انکڑے, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
T_UConverter_منزل_Unicode_UTF32_LE(UConverter_منزل_Unicode_دلیلاں * دلیلاں_2,
                                UError_کوڈ * err)
{
    ثابت بنانشانی حرف *my_سروت = (بنانشانی حرف *) دلیلاں_2->سروت;
    char16_t *my_نشانہ = دلیلاں_2->نشانہ;
    ثابت بنانشانی حرف *سروت_حد_بندی = (بنانشانی حرف *) دلیلاں_2->سروت_حد_بندی;
    ثابت char16_t *نشانہ_حد_بندی = دلیلاں_2->نشانہ_حد_بندی;
    بنانشانی حرف *منزل_UBytes = دلیلاں_2->converter->منزل_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    إذا (دلیلاں_2->converter->منزل_ULength > 0 && my_نشانہ < نشانہ_حد_بندی)
    {
        i = دلیلاں_2->converter->منزل_ULength;       /* restore # of bytes consumed */
        دلیلاں_2->converter->منزل_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = دلیلاں_2->converter->منزل_Unicode_حالت - 1;
        دلیلاں_2->converter->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }

    طالما (my_سروت < سروت_حد_بندی && my_نشانہ < نشانہ_حد_بندی)
    {
        i = 0;
        ch = 0;
morebytes:
        طالما (i < حجم(uint32_t))
        {
            إذا (my_سروت < سروت_حد_بندی)
            {
                ch |= ((uint8_t)(*my_سروت)) << (i * 8);
                منزل_UBytes[i++] = (حرف) *(my_سروت++);
            }
            وإلا
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                دلیلاں_2->converter->منزل_Unicode_حالت = ch + 1;
                دلیلاں_2->converter->منزل_ULength = (int8_t) i;
                اذهب donefornow;
            }
        }

        إذا (ch <= ودھ_توں_ودھ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            إذا (ch <= ودھ_توں_ودھ_UCS2) {
                /* fits in 16 bits */
                *(my_نشانہ++) = (char16_t) ch;
            }
            وإلا {
                /* write out the surrogates */
                *(my_نشانہ++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                إذا (my_نشانہ < نشانہ_حد_بندی) {
                    *(my_نشانہ++) = (char16_t)ch;
                }
                وإلا {
                    /* Put in overflow buffer (not handled here) */
                    دلیلاں_2->converter->UChar_Error_بفر[0] = (char16_t) ch;
                    دلیلاں_2->converter->UChar_Error_بفر_لمائی = 1;
                    *err = U_بفر_حد_توں_ودھ_ERROR;
                    اكسر;
                }
            }
        }
        وإلا {
            دلیلاں_2->converter->منزل_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_لبھیا;
            اكسر;
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

ساكن فراغ U_CALLCONV
T_UConverter_منزل_Unicode_UTF32_LE_وتھ_LOGIC(UConverter_منزل_Unicode_دلیلاں * دلیلاں_2,
                                             UError_کوڈ * err)
{
    ثابت بنانشانی حرف *my_سروت = (بنانشانی حرف *) دلیلاں_2->سروت;
    char16_t *my_نشانہ = دلیلاں_2->نشانہ;
    int32_t *myOffsets = دلیلاں_2->offsets;
    ثابت بنانشانی حرف *سروت_حد_بندی = (بنانشانی حرف *) دلیلاں_2->سروت_حد_بندی;
    ثابت char16_t *نشانہ_حد_بندی = دلیلاں_2->نشانہ_حد_بندی;
    بنانشانی حرف *منزل_UBytes = دلیلاں_2->converter->منزل_UBytes;
    uint32_t ch, i;
    int32_t وتھ_عدد = 0;

    /* Restore state of current sequence */
    إذا (دلیلاں_2->converter->منزل_ULength > 0 && my_نشانہ < نشانہ_حد_بندی)
    {
        i = دلیلاں_2->converter->منزل_ULength;       /* restore # of bytes consumed */
        دلیلاں_2->converter->منزل_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = دلیلاں_2->converter->منزل_Unicode_حالت - 1;
        دلیلاں_2->converter->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }

    طالما (my_سروت < سروت_حد_بندی && my_نشانہ < نشانہ_حد_بندی)
    {
        i = 0;
        ch = 0;
morebytes:
        طالما (i < حجم(uint32_t))
        {
            إذا (my_سروت < سروت_حد_بندی)
            {
                ch |= ((uint8_t)(*my_سروت)) << (i * 8);
                منزل_UBytes[i++] = (حرف) *(my_سروت++);
            }
            وإلا
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                دلیلاں_2->converter->منزل_Unicode_حالت = ch + 1;
                دلیلاں_2->converter->منزل_ULength = (int8_t) i;
                اذهب donefornow;
            }
        }

        إذا (ch <= ودھ_توں_ودھ_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            إذا (ch <= ودھ_توں_ودھ_UCS2) 
            {
                /* fits in 16 bits */
                *(my_نشانہ++) = (char16_t) ch;
                *(myOffsets++) = وتھ_عدد;
            }
            وإلا {
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
                    /* Put in overflow buffer (not handled here) */
                    دلیلاں_2->converter->UChar_Error_بفر[0] = (char16_t) ch;
                    دلیلاں_2->converter->UChar_Error_بفر_لمائی = 1;
                    *err = U_بفر_حد_توں_ودھ_ERROR;
                    اكسر;
                }
            }
        }
        وإلا
        {
            دلیلاں_2->converter->منزل_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_لبھیا;
            اكسر;
        }
        وتھ_عدد += i;
    }

donefornow:
    إذا (my_سروت < سروت_حد_بندی && my_نشانہ >= نشانہ_حد_بندی && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_بفر_حد_توں_ودھ_ERROR;
    }

    دلیلاں_2->نشانہ = my_نشانہ;
    دلیلاں_2->سروت = (ثابت حرف *) my_سروت;
    دلیلاں_2->offsets = myOffsets;
}

ساكن فراغ U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverter_From_Unicode_دلیلاں * دلیلاں_2,
                                  UError_کوڈ * err)
{
    ثابت char16_t *my_سروت = دلیلاں_2->سروت;
    بنانشانی حرف *my_نشانہ;
    ثابت char16_t *سروت_حد_بندی = دلیلاں_2->سروت_حد_بندی;
    ثابت بنانشانی حرف *نشانہ_حد_بندی = (بنانشانی حرف *) دلیلاں_2->نشانہ_حد_بندی;
    UChar32 ch, ch2;
    بنانشانی صحيح اشاریہ_منزل_Write;
    بنانشانی حرف عارضی[حجم(uint32_t)];

    إذا(my_سروت >= سروت_حد_بندی) {
        /* no input, nothing to do */
        أرجع;
    }

    /* write the BOM if necessary */
    إذا(دلیلاں_2->converter->from_Unicode_حالت==UCNV_NEED_منزل_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ (حرف)0xffu, (حرف)0xfeu, 0, 0 };
        ucnv_from_UWrite_بائٹاں(دلیلاں_2->converter,
                             bom, 4,
                             &دلیلاں_2->نشانہ, دلیلاں_2->نشانہ_حد_بندی,
                             &دلیلاں_2->offsets, -1,
                             err);
        دلیلاں_2->converter->from_Unicode_حالت=0;
    }

    my_نشانہ = (بنانشانی حرف *) دلیلاں_2->نشانہ;
    عارضی[3] = 0;

    إذا (دلیلاں_2->converter->fromUChar32)
    {
        ch = دلیلاں_2->converter->fromUChar32;
        دلیلاں_2->converter->fromUChar32 = 0;
        اذهب lowsurogate;
    }

    طالما (my_سروت < سروت_حد_بندی && my_نشانہ < نشانہ_حد_بندی)
    {
        ch = *(my_سروت++);

        إذا (U16_IS_SURROGATE(ch)) {
            إذا (U16_IS_LEAD(ch))
            {
lowsurogate:
                إذا (my_سروت < سروت_حد_بندی)
                {
                    ch2 = *my_سروت;
                    إذا (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_بلند_شروع) << HALF_کھسکاؤ) + ch2 + SURROGATE_کم_ادھار;
                        my_سروت++;
                    }
                    وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        دلیلاں_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_لبھیا;
                        اكسر;
                    }
                }
                وإلا {
                    /* ran out of source */
                    دلیلاں_2->converter->fromUChar32 = ch;
                    إذا (دلیلاں_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_لبھیا;
                    }
                    اكسر;
                }
            }
            وإلا {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                دلیلاں_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_لبھیا;
                اكسر;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        عارضی[2] = (uint8_t) (ch >> 16 & 0x1F);
        عارضی[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        عارضی[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        لكل (اشاریہ_منزل_Write = 0; اشاریہ_منزل_Write <= حجم(uint32_t) - 1; اشاریہ_منزل_Write++)
        {
            إذا (my_نشانہ < نشانہ_حد_بندی)
            {
                *(my_نشانہ++) = عارضی[اشاریہ_منزل_Write];
            }
            وإلا
            {
                دلیلاں_2->converter->char_Error_بفر[دلیلاں_2->converter->char_Error_بفر_لمائی++] = عارضی[اشاریہ_منزل_Write];
                *err = U_بفر_حد_توں_ودھ_ERROR;
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

ساكن فراغ U_CALLCONV
T_UConverter_from_Unicode_UTF32_LE_وتھ_LOGIC(UConverter_From_Unicode_دلیلاں * دلیلاں_2,
                                               UError_کوڈ * err)
{
    ثابت char16_t *my_سروت = دلیلاں_2->سروت;
    بنانشانی حرف *my_نشانہ;
    int32_t *myOffsets;
    ثابت char16_t *سروت_حد_بندی = دلیلاں_2->سروت_حد_بندی;
    ثابت بنانشانی حرف *نشانہ_حد_بندی = (بنانشانی حرف *) دلیلاں_2->نشانہ_حد_بندی;
    UChar32 ch, ch2;
    بنانشانی صحيح اشاریہ_منزل_Write;
    بنانشانی حرف عارضی[حجم(uint32_t)];
    int32_t وتھ_عدد = 0;

    إذا(my_سروت >= سروت_حد_بندی) {
        /* no input, nothing to do */
        أرجع;
    }

    /* write the BOM if necessary */
    إذا(دلیلاں_2->converter->from_Unicode_حالت==UCNV_NEED_منزل_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ (حرف)0xffu, (حرف)0xfeu, 0, 0 };
        ucnv_from_UWrite_بائٹاں(دلیلاں_2->converter,
                             bom, 4,
                             &دلیلاں_2->نشانہ, دلیلاں_2->نشانہ_حد_بندی,
                             &دلیلاں_2->offsets, -1,
                             err);
        دلیلاں_2->converter->from_Unicode_حالت=0;
    }

    my_نشانہ = (بنانشانی حرف *) دلیلاں_2->نشانہ;
    myOffsets = دلیلاں_2->offsets;
    عارضی[3] = 0;

    إذا (دلیلاں_2->converter->fromUChar32)
    {
        ch = دلیلاں_2->converter->fromUChar32;
        دلیلاں_2->converter->fromUChar32 = 0;
        اذهب lowsurogate;
    }

    طالما (my_سروت < سروت_حد_بندی && my_نشانہ < نشانہ_حد_بندی)
    {
        ch = *(my_سروت++);

        إذا (U16_IS_SURROGATE(ch)) {
            إذا (U16_IS_LEAD(ch))
            {
lowsurogate:
                إذا (my_سروت < سروت_حد_بندی)
                {
                    ch2 = *my_سروت;
                    إذا (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_بلند_شروع) << HALF_کھسکاؤ) + ch2 + SURROGATE_کم_ادھار;
                        my_سروت++;
                    }
                    وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        دلیلاں_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_لبھیا;
                        اكسر;
                    }
                }
                وإلا {
                    /* ran out of source */
                    دلیلاں_2->converter->fromUChar32 = ch;
                    إذا (دلیلاں_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_لبھیا;
                    }
                    اكسر;
                }
            }
            وإلا {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                دلیلاں_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_لبھیا;
                اكسر;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        عارضی[2] = (uint8_t) (ch >> 16 & 0x1F);
        عارضی[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        عارضی[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        لكل (اشاریہ_منزل_Write = 0; اشاریہ_منزل_Write <= حجم(uint32_t) - 1; اشاریہ_منزل_Write++)
        {
            إذا (my_نشانہ < نشانہ_حد_بندی)
            {
                *(my_نشانہ++) = عارضی[اشاریہ_منزل_Write];
                *(myOffsets++) = وتھ_عدد;
            }
            وإلا
            {
                دلیلاں_2->converter->char_Error_بفر[دلیلاں_2->converter->char_Error_بفر_لمائی++] = عارضی[اشاریہ_منزل_Write];
                *err = U_بفر_حد_توں_ودھ_ERROR;
            }
        }
        وتھ_عدد = وتھ_عدد + 1 + (عارضی[2] != 0);
    }

    إذا (my_سروت < سروت_حد_بندی && my_نشانہ >= نشانہ_حد_بندی && U_SUCCESS(*err))
    {
        *err = U_بفر_حد_توں_ودھ_ERROR;
    }

    دلیلاں_2->نشانہ = (حرف *) my_نشانہ;
    دلیلاں_2->سروت = my_سروت;
    دلیلاں_2->offsets = myOffsets;
}

ساكن UChar32 U_CALLCONV
T_UConverter_get_اگلا_UChar_UTF32_LE(UConverter_منزل_Unicode_دلیلاں* دلیلاں_2,
                                   UError_کوڈ* err)
{
    ثابت uint8_t *my_سروت;
    UChar32 myUChar;
    int32_t length;

    my_سروت = (ثابت uint8_t *)دلیلاں_2->سروت;
    إذا (my_سروت >= (ثابت uint8_t *)دلیلاں_2->سروت_حد_بندی)
    {
        /* no input */
        *err = U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    length = (int32_t)((ثابت uint8_t *)دلیلاں_2->سروت_حد_بندی - my_سروت);
    إذا (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(دلیلاں_2->converter->منزل_UBytes, my_سروت, length);
        دلیلاں_2->converter->منزل_ULength = (int8_t)length;
        دلیلاں_2->سروت = (ثابت حرف *)(my_سروت + length);
        *err = U_TRUNCATED_CHAR_لبھیا;
        أرجع 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)my_سروت[3] << 24)
            | ((UChar32)my_سروت[2] << 16)
            | ((UChar32)my_سروت[1] << 8)
            | ((UChar32)my_سروت[0]);

    دلیلاں_2->سروت = (ثابت حرف *)(my_سروت + 4);
    إذا ((uint32_t)myUChar <= ودھ_توں_ودھ_UTF && !U_IS_SURROGATE(myUChar)) {
        أرجع myUChar;
    }

    uprv_memcpy(دلیلاں_2->converter->منزل_UBytes, my_سروت, 4);
    دلیلاں_2->converter->منزل_ULength = 4;

    *err = U_ILLEGAL_CHAR_لبھیا;
    أرجع 0xffff;
}
U_CDECL_سرا
ساكن ثابت UConverterImpl _UTF32LEImpl = {
    UCNV_UTF32_LittleEndian,

    خالی_سوچک,
    خالی_سوچک,

    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,

    T_UConverter_منزل_Unicode_UTF32_LE,
    T_UConverter_منزل_Unicode_UTF32_LE_وتھ_LOGIC,
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_وتھ_LOGIC,
    T_UConverter_get_اگلا_UChar_UTF32_LE,

    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,
    ucnv_get_Non_Surrogate_Unicode_سیٹ,

    خالی_سوچک,
    خالی_سوچک
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
ساكن ثابت UConverter_Static_انکڑے UTF32_LEStatic_انکڑے = {
    حجم(UConverter_Static_انکڑے),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_انکڑے _UTF32LEData =
        UCNV_IMMUTABLE_SHARED_انکڑے_INITIALIZER(&UTF32_LEStatic_انکڑے, &_UTF32LEImpl);

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
ساكن فراغ U_CALLCONV
_UTF32Reset(UConverter *cnv, UConverterResetChoice choice) {
    إذا(choice<=UCNV_RESET_منزل_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->منوال=0;
    }
    إذا(choice!=UCNV_RESET_منزل_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->from_Unicode_حالت=UCNV_NEED_منزل_WRITE_BOM;
    }
}

ساكن فراغ U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverter_لدو_دلیلاں *p_دلیلاں,
           UError_کوڈ *p_Error_کوڈ) {
    (فراغ)p_دلیلاں;
    (فراغ)p_Error_کوڈ;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

ساكن ثابت حرف utf32BOM[8]={ 0, 0, (حرف)0xfeu, (حرف)0xffu, (حرف)0xffu, (حرف)0xfeu, 0, 0 };

ساكن فراغ U_CALLCONV
UTF32_منزل_Unicode_With_Offsets(UConverter_منزل_Unicode_دلیلاں *p_دلیلاں,
                           UError_کوڈ *p_Error_کوڈ) {
    UConverter *cnv=p_دلیلاں->converter;
    ثابت حرف *سروت=p_دلیلاں->سروت;
    ثابت حرف *سروت_حد_بندی=p_دلیلاں->سروت_حد_بندی;
    int32_t *offsets=p_دلیلاں->offsets;

    int32_t state, وتھ_Delta;
    حرف b;

    state=cnv->منوال;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    وتھ_Delta=0;

    طالما(سروت<سروت_حد_بندی && U_SUCCESS(*p_Error_کوڈ)) {
        بدل(state) {
        حالة 0:
            b=*سروت;
            إذا(b==0) {
                state=1; /* could be 00 00 FE FF */
            } وإلا إذا(b==(حرف)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } وإلا {
                state=8; /* default to UTF-32BE */
                استمر;
            }
            ++سروت;
            اكسر;
        حالة 1:
        حالة 2:
        حالة 3:
        حالة 5:
        حالة 6:
        حالة 7:
            إذا(*سروت==utf32BOM[state]) {
                ++state;
                ++سروت;
                إذا(state==4) {
                    state=8; /* detect UTF-32BE */
                    وتھ_Delta=(int32_t)(سروت-p_دلیلاں->سروت);
                } وإلا إذا(state==8) {
                    state=9; /* detect UTF-32LE */
                    وتھ_Delta=(int32_t)(سروت-p_دلیلاں->سروت);
                }
            } وإلا {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(سروت-p_دلیلاں->سروت); /* number of bytes from this buffer */

                /* reset the source */
                سروت=p_دلیلاں->سروت;

                إذا(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } وإلا {
                    UBool oldFlush=p_دلیلاں->flush;

                    /* some of the bytes are from a previous buffer, replay those first */
                    p_دلیلاں->سروت=utf32BOM+(state&4); /* select the correct BOM */
                    p_دلیلاں->سروت_حد_بندی=p_دلیلاں->سروت+((state&3)-count); /* replay previous bytes */
                    p_دلیلاں->flush=خطأ; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_منزل_Unicode_UTF32_BE(p_دلیلاں, p_Error_کوڈ);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    p_دلیلاں->سروت_حد_بندی=سروت_حد_بندی;
                    p_دلیلاں->flush=oldFlush;
                }
                state=8;
                استمر;
            }
            اكسر;
        حالة 8:
            /* call UTF-32BE */
            p_دلیلاں->سروت=سروت;
            إذا(offsets==خالی_سوچک) {
                T_UConverter_منزل_Unicode_UTF32_BE(p_دلیلاں, p_Error_کوڈ);
            } وإلا {
                T_UConverter_منزل_Unicode_UTF32_BE_وتھ_LOGIC(p_دلیلاں, p_Error_کوڈ);
            }
            سروت=p_دلیلاں->سروت;
            اكسر;
        حالة 9:
            /* call UTF-32LE */
            p_دلیلاں->سروت=سروت;
            إذا(offsets==خالی_سوچک) {
                T_UConverter_منزل_Unicode_UTF32_LE(p_دلیلاں, p_Error_کوڈ);
            } وإلا {
                T_UConverter_منزل_Unicode_UTF32_LE_وتھ_LOGIC(p_دلیلاں, p_Error_کوڈ);
            }
            سروت=p_دلیلاں->سروت;
            اكسر;
        افتراضي:
            اكسر; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    إذا(offsets!=خالی_سوچک && وتھ_Delta!=0) {
        int32_t *offsets_حد_بندی=p_دلیلاں->offsets;
        طالما(offsets<offsets_حد_بندی) {
            *offsets++ += وتھ_Delta;
        }
    }

    p_دلیلاں->سروت=سروت;

    إذا(سروت==سروت_حد_بندی && p_دلیلاں->flush) {
        /* handle truncated input */
        بدل(state) {
        حالة 0:
            اكسر; /* no input at all, nothing to do */
        حالة 8:
            T_UConverter_منزل_Unicode_UTF32_BE(p_دلیلاں, p_Error_کوڈ);
            اكسر;
        حالة 9:
            T_UConverter_منزل_Unicode_UTF32_LE(p_دلیلاں, p_Error_کوڈ);
            اكسر;
        افتراضي:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            p_دلیلاں->سروت=utf32BOM+(state&4); /* select the correct BOM */
            p_دلیلاں->سروت_حد_بندی=p_دلیلاں->سروت+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_منزل_Unicode_UTF32_BE(p_دلیلاں, p_Error_کوڈ);
            p_دلیلاں->سروت=سروت;
            p_دلیلاں->سروت_حد_بندی=سروت_حد_بندی;
            state=8;
            اكسر;
        }
    }

    cnv->منوال=state;
}

ساكن UChar32 U_CALLCONV
UTF32_Get_اگلا_UChar(UConverter_منزل_Unicode_دلیلاں *p_دلیلاں,
                   UError_کوڈ *p_Error_کوڈ) {
    بدل(p_دلیلاں->converter->منوال) {
    حالة 8:
        أرجع T_UConverter_get_اگلا_UChar_UTF32_BE(p_دلیلاں, p_Error_کوڈ);
    حالة 9:
        أرجع T_UConverter_get_اگلا_UChar_UTF32_LE(p_دلیلاں, p_Error_کوڈ);
    افتراضي:
        أرجع UCNV_GET_اگلا_UCHAR_USE_منزل_U;
    }
}
U_CDECL_سرا
ساكن ثابت UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    خالی_سوچک,
    خالی_سوچک,

    _UTF32Open,
    خالی_سوچک,
    _UTF32Reset,

    UTF32_منزل_Unicode_With_Offsets,
    UTF32_منزل_Unicode_With_Offsets,
#إذا U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_وتھ_LOGIC,
#وإلا
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_وتھ_LOGIC,
#جے_انت
    UTF32_Get_اگلا_UChar,

    خالی_سوچک, /* ### TODO implement getStarters for all Unicode encodings?! */
    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,
    ucnv_get_Non_Surrogate_Unicode_سیٹ,

    خالی_سوچک,
    خالی_سوچک
};

/* The 1236 CCSID refers to any version of Unicode with a BOM sensitive endianness of UTF-32 */
ساكن ثابت UConverter_Static_انکڑے UTF32_Static_انکڑے = {
    حجم(UConverter_Static_انکڑے),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#إذا U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#وإلا
    { 0xfd, 0xff, 0, 0 }, 4,
#جے_انت
    خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_انکڑے UTF32_انکڑے = 
        UCNV_IMMUTABLE_SHARED_انکڑے_INITIALIZER(&UTF32_Static_انکڑے, &_UTF32Impl);

#جے_انت
