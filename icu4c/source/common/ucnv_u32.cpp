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

#تعریف_کن بیشینه_UCS2            0x0000FFFF
#تعریف_کن بیشینه_UTF             0x0010FFFF
#تعریف_کن HALF_لغزش              10
#تعریف_کن HALF_پایه               0x0010000
#تعریف_کن HALF_ماسک               0x3FF
#تعریف_کن SURROGATE_بالا_شروع    0xD800
#تعریف_کن SURROGATE_کم_شروع     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#تعریف_کن SURROGATE_کم_پایه      9216

تعداد {
    UCNV_NEED_مقصد_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
T_UConverter_مقصد_Unicode_UTF32_BE(UConverter_مقصد_Unicode_آرگومانها * آرگومانها_2,
                                UError_کد * err)
{
    ثابت بیعلامت حرف *my_منبع = (بیعلامت حرف *) آرگومانها_2->منبع;
    char16_t *my_هدف = آرگومانها_2->هدف;
    ثابت بیعلامت حرف *منبع_حد = (بیعلامت حرف *) آرگومانها_2->منبع_حد;
    ثابت char16_t *هدف_حد = آرگومانها_2->هدف_حد;
    بیعلامت حرف *مقصد_UBytes = آرگومانها_2->converter->مقصد_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    إذا (آرگومانها_2->converter->مقصد_ULength > 0 && my_هدف < هدف_حد) {
        i = آرگومانها_2->converter->مقصد_ULength;       /* restore # of bytes consumed */
        آرگومانها_2->converter->مقصد_ULength = 0;

        ch = آرگومانها_2->converter->مقصد_Unicode_وضعیت - 1;/*Stores the previously calculated ch from a previous call*/
        آرگومانها_2->converter->مقصد_Unicode_وضعیت = 0;
        اذهب morebytes;
    }

    طالما (my_منبع < منبع_حد && my_هدف < هدف_حد) {
        i = 0;
        ch = 0;
morebytes:
        طالما (i < حجم(uint32_t)) {
            إذا (my_منبع < منبع_حد) {
                ch = (ch << 8) | (uint8_t)(*my_منبع);
                مقصد_UBytes[i++] = (حرف) *(my_منبع++);
            }
            وإلا {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                آرگومانها_2->converter->مقصد_Unicode_وضعیت = ch + 1;
                آرگومانها_2->converter->مقصد_ULength = (int8_t) i;
                اذهب donefornow;
            }
        }

        إذا (ch <= بیشینه_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            إذا (ch <= بیشینه_UCS2) 
            {
                /* fits in 16 bits */
                *(my_هدف++) = (char16_t) ch;
            }
            وإلا {
                /* write out the surrogates */
                *(my_هدف++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                إذا (my_هدف < هدف_حد) {
                    *(my_هدف++) = (char16_t)ch;
                }
                وإلا {
                    /* Put in overflow buffer (not handled here) */
                    آرگومانها_2->converter->UChar_Error_میانگیر[0] = (char16_t) ch;
                    آرگومانها_2->converter->UChar_Error_میانگیر_درازی = 1;
                    *err = U_میانگیر_سرریز_ERROR;
                    اكسر;
                }
            }
        }
        وإلا {
            آرگومانها_2->converter->مقصد_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_یافت;
            اكسر;
        }
    }

donefornow:
    إذا (my_منبع < منبع_حد && my_هدف >= هدف_حد && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_میانگیر_سرریز_ERROR;
    }

    آرگومانها_2->هدف = my_هدف;
    آرگومانها_2->منبع = (ثابت حرف *) my_منبع;
}

ساكن فراغ U_CALLCONV
T_UConverter_مقصد_Unicode_UTF32_BE_جابه_جایی_LOGIC(UConverter_مقصد_Unicode_آرگومانها * آرگومانها_2,
                                             UError_کد * err)
{
    ثابت بیعلامت حرف *my_منبع = (بیعلامت حرف *) آرگومانها_2->منبع;
    char16_t *my_هدف = آرگومانها_2->هدف;
    int32_t *myOffsets = آرگومانها_2->offsets;
    ثابت بیعلامت حرف *منبع_حد = (بیعلامت حرف *) آرگومانها_2->منبع_حد;
    ثابت char16_t *هدف_حد = آرگومانها_2->هدف_حد;
    بیعلامت حرف *مقصد_UBytes = آرگومانها_2->converter->مقصد_UBytes;
    uint32_t ch, i;
    int32_t جابه_جایی_رقم = 0;

    /* Restore state of current sequence */
    إذا (آرگومانها_2->converter->مقصد_ULength > 0 && my_هدف < هدف_حد) {
        i = آرگومانها_2->converter->مقصد_ULength;       /* restore # of bytes consumed */
        آرگومانها_2->converter->مقصد_ULength = 0;

        ch = آرگومانها_2->converter->مقصد_Unicode_وضعیت - 1;/*Stores the previously calculated ch from a previous call*/
        آرگومانها_2->converter->مقصد_Unicode_وضعیت = 0;
        اذهب morebytes;
    }

    طالما (my_منبع < منبع_حد && my_هدف < هدف_حد) {
        i = 0;
        ch = 0;
morebytes:
        طالما (i < حجم(uint32_t)) {
            إذا (my_منبع < منبع_حد) {
                ch = (ch << 8) | (uint8_t)(*my_منبع);
                مقصد_UBytes[i++] = (حرف) *(my_منبع++);
            }
            وإلا {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                آرگومانها_2->converter->مقصد_Unicode_وضعیت = ch + 1;
                آرگومانها_2->converter->مقصد_ULength = (int8_t) i;
                اذهب donefornow;
            }
        }

        إذا (ch <= بیشینه_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            إذا (ch <= بیشینه_UCS2) {
                /* fits in 16 bits */
                *(my_هدف++) = (char16_t) ch;
                *(myOffsets++) = جابه_جایی_رقم;
            }
            وإلا {
                /* write out the surrogates */
                *(my_هدف++) = U16_LEAD(ch);
                *myOffsets++ = جابه_جایی_رقم;
                ch = U16_TRAIL(ch);
                إذا (my_هدف < هدف_حد)
                {
                    *(my_هدف++) = (char16_t)ch;
                    *(myOffsets++) = جابه_جایی_رقم;
                }
                وإلا {
                    /* Put in overflow buffer (not handled here) */
                    آرگومانها_2->converter->UChar_Error_میانگیر[0] = (char16_t) ch;
                    آرگومانها_2->converter->UChar_Error_میانگیر_درازی = 1;
                    *err = U_میانگیر_سرریز_ERROR;
                    اكسر;
                }
            }
        }
        وإلا {
            آرگومانها_2->converter->مقصد_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_یافت;
            اكسر;
        }
        جابه_جایی_رقم += i;
    }

donefornow:
    إذا (my_منبع < منبع_حد && my_هدف >= هدف_حد && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_میانگیر_سرریز_ERROR;
    }

    آرگومانها_2->هدف = my_هدف;
    آرگومانها_2->منبع = (ثابت حرف *) my_منبع;
    آرگومانها_2->offsets = myOffsets;
}

ساكن فراغ U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverter_From_Unicode_آرگومانها * آرگومانها_2,
                                  UError_کد * err)
{
    ثابت char16_t *my_منبع = آرگومانها_2->منبع;
    بیعلامت حرف *my_هدف;
    ثابت char16_t *منبع_حد = آرگومانها_2->منبع_حد;
    ثابت بیعلامت حرف *هدف_حد = (بیعلامت حرف *) آرگومانها_2->هدف_حد;
    UChar32 ch, ch2;
    بیعلامت صحيح شاخص_مقصد_Write;
    بیعلامت حرف موقت[حجم(uint32_t)];

    إذا(my_منبع >= منبع_حد) {
        /* no input, nothing to do */
        أرجع;
    }

    /* write the BOM if necessary */
    إذا(آرگومانها_2->converter->from_Unicode_وضعیت==UCNV_NEED_مقصد_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ 0, 0, (حرف)0xfeu, (حرف)0xffu };
        ucnv_from_UWrite_بایتها(آرگومانها_2->converter,
                             bom, 4,
                             &آرگومانها_2->هدف, آرگومانها_2->هدف_حد,
                             &آرگومانها_2->offsets, -1,
                             err);
        آرگومانها_2->converter->from_Unicode_وضعیت=0;
    }

    my_هدف = (بیعلامت حرف *) آرگومانها_2->هدف;
    موقت[0] = 0;

    إذا (آرگومانها_2->converter->fromUChar32) {
        ch = آرگومانها_2->converter->fromUChar32;
        آرگومانها_2->converter->fromUChar32 = 0;
        اذهب lowsurogate;
    }

    طالما (my_منبع < منبع_حد && my_هدف < هدف_حد) {
        ch = *(my_منبع++);

        إذا (U_IS_SURROGATE(ch)) {
            إذا (U_IS_LEAD(ch)) {
lowsurogate:
                إذا (my_منبع < منبع_حد) {
                    ch2 = *my_منبع;
                    إذا (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_بالا_شروع) << HALF_لغزش) + ch2 + SURROGATE_کم_پایه;
                        my_منبع++;
                    }
                    وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        آرگومانها_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_یافت;
                        اكسر;
                    }
                }
                وإلا {
                    /* ran out of source */
                    آرگومانها_2->converter->fromUChar32 = ch;
                    إذا (آرگومانها_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_یافت;
                    }
                    اكسر;
                }
            }
            وإلا {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                آرگومانها_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_یافت;
                اكسر;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        موقت[1] = (uint8_t) (ch >> 16 & 0x1F);
        موقت[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        موقت[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        لكل (شاخص_مقصد_Write = 0; شاخص_مقصد_Write <= حجم(uint32_t) - 1; شاخص_مقصد_Write++) {
            إذا (my_هدف < هدف_حد) {
                *(my_هدف++) = موقت[شاخص_مقصد_Write];
            }
            وإلا {
                آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] = موقت[شاخص_مقصد_Write];
                *err = U_میانگیر_سرریز_ERROR;
            }
        }
    }

    إذا (my_منبع < منبع_حد && my_هدف >= هدف_حد && U_SUCCESS(*err)) {
        *err = U_میانگیر_سرریز_ERROR;
    }

    آرگومانها_2->هدف = (حرف *) my_هدف;
    آرگومانها_2->منبع = my_منبع;
}

ساكن فراغ U_CALLCONV
T_UConverter_from_Unicode_UTF32_BE_جابه_جایی_LOGIC(UConverter_From_Unicode_آرگومانها * آرگومانها_2,
                                               UError_کد * err)
{
    ثابت char16_t *my_منبع = آرگومانها_2->منبع;
    بیعلامت حرف *my_هدف;
    int32_t *myOffsets;
    ثابت char16_t *منبع_حد = آرگومانها_2->منبع_حد;
    ثابت بیعلامت حرف *هدف_حد = (بیعلامت حرف *) آرگومانها_2->هدف_حد;
    UChar32 ch, ch2;
    int32_t جابه_جایی_رقم = 0;
    بیعلامت صحيح شاخص_مقصد_Write;
    بیعلامت حرف موقت[حجم(uint32_t)];

    إذا(my_منبع >= منبع_حد) {
        /* no input, nothing to do */
        أرجع;
    }

    /* write the BOM if necessary */
    إذا(آرگومانها_2->converter->from_Unicode_وضعیت==UCNV_NEED_مقصد_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ 0, 0, (حرف)0xfeu, (حرف)0xffu };
        ucnv_from_UWrite_بایتها(آرگومانها_2->converter,
                             bom, 4,
                             &آرگومانها_2->هدف, آرگومانها_2->هدف_حد,
                             &آرگومانها_2->offsets, -1,
                             err);
        آرگومانها_2->converter->from_Unicode_وضعیت=0;
    }

    my_هدف = (بیعلامت حرف *) آرگومانها_2->هدف;
    myOffsets = آرگومانها_2->offsets;
    موقت[0] = 0;

    إذا (آرگومانها_2->converter->fromUChar32) {
        ch = آرگومانها_2->converter->fromUChar32;
        آرگومانها_2->converter->fromUChar32 = 0;
        اذهب lowsurogate;
    }

    طالما (my_منبع < منبع_حد && my_هدف < هدف_حد) {
        ch = *(my_منبع++);

        إذا (U_IS_SURROGATE(ch)) {
            إذا (U_IS_LEAD(ch)) {
lowsurogate:
                إذا (my_منبع < منبع_حد) {
                    ch2 = *my_منبع;
                    إذا (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_بالا_شروع) << HALF_لغزش) + ch2 + SURROGATE_کم_پایه;
                        my_منبع++;
                    }
                    وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        آرگومانها_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_یافت;
                        اكسر;
                    }
                }
                وإلا {
                    /* ran out of source */
                    آرگومانها_2->converter->fromUChar32 = ch;
                    إذا (آرگومانها_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_یافت;
                    }
                    اكسر;
                }
            }
            وإلا {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                آرگومانها_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_یافت;
                اكسر;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        موقت[1] = (uint8_t) (ch >> 16 & 0x1F);
        موقت[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        موقت[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        لكل (شاخص_مقصد_Write = 0; شاخص_مقصد_Write <= حجم(uint32_t) - 1; شاخص_مقصد_Write++) {
            إذا (my_هدف < هدف_حد) {
                *(my_هدف++) = موقت[شاخص_مقصد_Write];
                *(myOffsets++) = جابه_جایی_رقم;
            }
            وإلا {
                آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] = موقت[شاخص_مقصد_Write];
                *err = U_میانگیر_سرریز_ERROR;
            }
        }
        جابه_جایی_رقم = جابه_جایی_رقم + 1 + (موقت[1] != 0);
    }

    إذا (my_منبع < منبع_حد && my_هدف >= هدف_حد && U_SUCCESS(*err)) {
        *err = U_میانگیر_سرریز_ERROR;
    }

    آرگومانها_2->هدف = (حرف *) my_هدف;
    آرگومانها_2->منبع = my_منبع;
    آرگومانها_2->offsets = myOffsets;
}

ساكن UChar32 U_CALLCONV
T_UConverter_get_بعدی_UChar_UTF32_BE(UConverter_مقصد_Unicode_آرگومانها* آرگومانها_2,
                                   UError_کد* err)
{
    ثابت uint8_t *my_منبع;
    UChar32 myUChar;
    int32_t length;

    my_منبع = (ثابت uint8_t *)آرگومانها_2->منبع;
    إذا (my_منبع >= (ثابت uint8_t *)آرگومانها_2->منبع_حد)
    {
        /* no input */
        *err = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    length = (int32_t)((ثابت uint8_t *)آرگومانها_2->منبع_حد - my_منبع);
    إذا (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(آرگومانها_2->converter->مقصد_UBytes, my_منبع, length);
        آرگومانها_2->converter->مقصد_ULength = (int8_t)length;
        آرگومانها_2->منبع = (ثابت حرف *)(my_منبع + length);
        *err = U_TRUNCATED_CHAR_یافت;
        أرجع 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)my_منبع[0] << 24)
            | ((UChar32)my_منبع[1] << 16)
            | ((UChar32)my_منبع[2] << 8)
            | ((UChar32)my_منبع[3]);

    آرگومانها_2->منبع = (ثابت حرف *)(my_منبع + 4);
    إذا ((uint32_t)myUChar <= بیشینه_UTF && !U_IS_SURROGATE(myUChar)) {
        أرجع myUChar;
    }

    uprv_memcpy(آرگومانها_2->converter->مقصد_UBytes, my_منبع, 4);
    آرگومانها_2->converter->مقصد_ULength = 4;

    *err = U_ILLEGAL_CHAR_یافت;
    أرجع 0xffff;
}
U_CDECL_انتها
ساكن ثابت UConverterImpl _UTF32BEImpl = {
    UCNV_UTF32_BigEndian,

    اشاره_تهی,
    اشاره_تهی,

    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,

    T_UConverter_مقصد_Unicode_UTF32_BE,
    T_UConverter_مقصد_Unicode_UTF32_BE_جابه_جایی_LOGIC,
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_جابه_جایی_LOGIC,
    T_UConverter_get_بعدی_UChar_UTF32_BE,

    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
    ucnv_get_Non_Surrogate_Unicode_مجموعه,

    اشاره_تهی,
    اشاره_تهی
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
ساكن ثابت UConverter_Static_داده UTF32_BEStatic_داده = {
    حجم(UConverter_Static_داده),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_داده _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&UTF32_BEStatic_داده, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
T_UConverter_مقصد_Unicode_UTF32_LE(UConverter_مقصد_Unicode_آرگومانها * آرگومانها_2,
                                UError_کد * err)
{
    ثابت بیعلامت حرف *my_منبع = (بیعلامت حرف *) آرگومانها_2->منبع;
    char16_t *my_هدف = آرگومانها_2->هدف;
    ثابت بیعلامت حرف *منبع_حد = (بیعلامت حرف *) آرگومانها_2->منبع_حد;
    ثابت char16_t *هدف_حد = آرگومانها_2->هدف_حد;
    بیعلامت حرف *مقصد_UBytes = آرگومانها_2->converter->مقصد_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    إذا (آرگومانها_2->converter->مقصد_ULength > 0 && my_هدف < هدف_حد)
    {
        i = آرگومانها_2->converter->مقصد_ULength;       /* restore # of bytes consumed */
        آرگومانها_2->converter->مقصد_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = آرگومانها_2->converter->مقصد_Unicode_وضعیت - 1;
        آرگومانها_2->converter->مقصد_Unicode_وضعیت = 0;
        اذهب morebytes;
    }

    طالما (my_منبع < منبع_حد && my_هدف < هدف_حد)
    {
        i = 0;
        ch = 0;
morebytes:
        طالما (i < حجم(uint32_t))
        {
            إذا (my_منبع < منبع_حد)
            {
                ch |= ((uint8_t)(*my_منبع)) << (i * 8);
                مقصد_UBytes[i++] = (حرف) *(my_منبع++);
            }
            وإلا
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                آرگومانها_2->converter->مقصد_Unicode_وضعیت = ch + 1;
                آرگومانها_2->converter->مقصد_ULength = (int8_t) i;
                اذهب donefornow;
            }
        }

        إذا (ch <= بیشینه_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            إذا (ch <= بیشینه_UCS2) {
                /* fits in 16 bits */
                *(my_هدف++) = (char16_t) ch;
            }
            وإلا {
                /* write out the surrogates */
                *(my_هدف++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                إذا (my_هدف < هدف_حد) {
                    *(my_هدف++) = (char16_t)ch;
                }
                وإلا {
                    /* Put in overflow buffer (not handled here) */
                    آرگومانها_2->converter->UChar_Error_میانگیر[0] = (char16_t) ch;
                    آرگومانها_2->converter->UChar_Error_میانگیر_درازی = 1;
                    *err = U_میانگیر_سرریز_ERROR;
                    اكسر;
                }
            }
        }
        وإلا {
            آرگومانها_2->converter->مقصد_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_یافت;
            اكسر;
        }
    }

donefornow:
    إذا (my_منبع < منبع_حد && my_هدف >= هدف_حد && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_میانگیر_سرریز_ERROR;
    }

    آرگومانها_2->هدف = my_هدف;
    آرگومانها_2->منبع = (ثابت حرف *) my_منبع;
}

ساكن فراغ U_CALLCONV
T_UConverter_مقصد_Unicode_UTF32_LE_جابه_جایی_LOGIC(UConverter_مقصد_Unicode_آرگومانها * آرگومانها_2,
                                             UError_کد * err)
{
    ثابت بیعلامت حرف *my_منبع = (بیعلامت حرف *) آرگومانها_2->منبع;
    char16_t *my_هدف = آرگومانها_2->هدف;
    int32_t *myOffsets = آرگومانها_2->offsets;
    ثابت بیعلامت حرف *منبع_حد = (بیعلامت حرف *) آرگومانها_2->منبع_حد;
    ثابت char16_t *هدف_حد = آرگومانها_2->هدف_حد;
    بیعلامت حرف *مقصد_UBytes = آرگومانها_2->converter->مقصد_UBytes;
    uint32_t ch, i;
    int32_t جابه_جایی_رقم = 0;

    /* Restore state of current sequence */
    إذا (آرگومانها_2->converter->مقصد_ULength > 0 && my_هدف < هدف_حد)
    {
        i = آرگومانها_2->converter->مقصد_ULength;       /* restore # of bytes consumed */
        آرگومانها_2->converter->مقصد_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = آرگومانها_2->converter->مقصد_Unicode_وضعیت - 1;
        آرگومانها_2->converter->مقصد_Unicode_وضعیت = 0;
        اذهب morebytes;
    }

    طالما (my_منبع < منبع_حد && my_هدف < هدف_حد)
    {
        i = 0;
        ch = 0;
morebytes:
        طالما (i < حجم(uint32_t))
        {
            إذا (my_منبع < منبع_حد)
            {
                ch |= ((uint8_t)(*my_منبع)) << (i * 8);
                مقصد_UBytes[i++] = (حرف) *(my_منبع++);
            }
            وإلا
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                آرگومانها_2->converter->مقصد_Unicode_وضعیت = ch + 1;
                آرگومانها_2->converter->مقصد_ULength = (int8_t) i;
                اذهب donefornow;
            }
        }

        إذا (ch <= بیشینه_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            إذا (ch <= بیشینه_UCS2) 
            {
                /* fits in 16 bits */
                *(my_هدف++) = (char16_t) ch;
                *(myOffsets++) = جابه_جایی_رقم;
            }
            وإلا {
                /* write out the surrogates */
                *(my_هدف++) = U16_LEAD(ch);
                *(myOffsets++) = جابه_جایی_رقم;
                ch = U16_TRAIL(ch);
                إذا (my_هدف < هدف_حد)
                {
                    *(my_هدف++) = (char16_t)ch;
                    *(myOffsets++) = جابه_جایی_رقم;
                }
                وإلا
                {
                    /* Put in overflow buffer (not handled here) */
                    آرگومانها_2->converter->UChar_Error_میانگیر[0] = (char16_t) ch;
                    آرگومانها_2->converter->UChar_Error_میانگیر_درازی = 1;
                    *err = U_میانگیر_سرریز_ERROR;
                    اكسر;
                }
            }
        }
        وإلا
        {
            آرگومانها_2->converter->مقصد_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_یافت;
            اكسر;
        }
        جابه_جایی_رقم += i;
    }

donefornow:
    إذا (my_منبع < منبع_حد && my_هدف >= هدف_حد && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_میانگیر_سرریز_ERROR;
    }

    آرگومانها_2->هدف = my_هدف;
    آرگومانها_2->منبع = (ثابت حرف *) my_منبع;
    آرگومانها_2->offsets = myOffsets;
}

ساكن فراغ U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverter_From_Unicode_آرگومانها * آرگومانها_2,
                                  UError_کد * err)
{
    ثابت char16_t *my_منبع = آرگومانها_2->منبع;
    بیعلامت حرف *my_هدف;
    ثابت char16_t *منبع_حد = آرگومانها_2->منبع_حد;
    ثابت بیعلامت حرف *هدف_حد = (بیعلامت حرف *) آرگومانها_2->هدف_حد;
    UChar32 ch, ch2;
    بیعلامت صحيح شاخص_مقصد_Write;
    بیعلامت حرف موقت[حجم(uint32_t)];

    إذا(my_منبع >= منبع_حد) {
        /* no input, nothing to do */
        أرجع;
    }

    /* write the BOM if necessary */
    إذا(آرگومانها_2->converter->from_Unicode_وضعیت==UCNV_NEED_مقصد_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ (حرف)0xffu, (حرف)0xfeu, 0, 0 };
        ucnv_from_UWrite_بایتها(آرگومانها_2->converter,
                             bom, 4,
                             &آرگومانها_2->هدف, آرگومانها_2->هدف_حد,
                             &آرگومانها_2->offsets, -1,
                             err);
        آرگومانها_2->converter->from_Unicode_وضعیت=0;
    }

    my_هدف = (بیعلامت حرف *) آرگومانها_2->هدف;
    موقت[3] = 0;

    إذا (آرگومانها_2->converter->fromUChar32)
    {
        ch = آرگومانها_2->converter->fromUChar32;
        آرگومانها_2->converter->fromUChar32 = 0;
        اذهب lowsurogate;
    }

    طالما (my_منبع < منبع_حد && my_هدف < هدف_حد)
    {
        ch = *(my_منبع++);

        إذا (U16_IS_SURROGATE(ch)) {
            إذا (U16_IS_LEAD(ch))
            {
lowsurogate:
                إذا (my_منبع < منبع_حد)
                {
                    ch2 = *my_منبع;
                    إذا (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_بالا_شروع) << HALF_لغزش) + ch2 + SURROGATE_کم_پایه;
                        my_منبع++;
                    }
                    وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        آرگومانها_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_یافت;
                        اكسر;
                    }
                }
                وإلا {
                    /* ran out of source */
                    آرگومانها_2->converter->fromUChar32 = ch;
                    إذا (آرگومانها_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_یافت;
                    }
                    اكسر;
                }
            }
            وإلا {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                آرگومانها_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_یافت;
                اكسر;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        موقت[2] = (uint8_t) (ch >> 16 & 0x1F);
        موقت[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        موقت[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        لكل (شاخص_مقصد_Write = 0; شاخص_مقصد_Write <= حجم(uint32_t) - 1; شاخص_مقصد_Write++)
        {
            إذا (my_هدف < هدف_حد)
            {
                *(my_هدف++) = موقت[شاخص_مقصد_Write];
            }
            وإلا
            {
                آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] = موقت[شاخص_مقصد_Write];
                *err = U_میانگیر_سرریز_ERROR;
            }
        }
    }

    إذا (my_منبع < منبع_حد && my_هدف >= هدف_حد && U_SUCCESS(*err))
    {
        *err = U_میانگیر_سرریز_ERROR;
    }

    آرگومانها_2->هدف = (حرف *) my_هدف;
    آرگومانها_2->منبع = my_منبع;
}

ساكن فراغ U_CALLCONV
T_UConverter_from_Unicode_UTF32_LE_جابه_جایی_LOGIC(UConverter_From_Unicode_آرگومانها * آرگومانها_2,
                                               UError_کد * err)
{
    ثابت char16_t *my_منبع = آرگومانها_2->منبع;
    بیعلامت حرف *my_هدف;
    int32_t *myOffsets;
    ثابت char16_t *منبع_حد = آرگومانها_2->منبع_حد;
    ثابت بیعلامت حرف *هدف_حد = (بیعلامت حرف *) آرگومانها_2->هدف_حد;
    UChar32 ch, ch2;
    بیعلامت صحيح شاخص_مقصد_Write;
    بیعلامت حرف موقت[حجم(uint32_t)];
    int32_t جابه_جایی_رقم = 0;

    إذا(my_منبع >= منبع_حد) {
        /* no input, nothing to do */
        أرجع;
    }

    /* write the BOM if necessary */
    إذا(آرگومانها_2->converter->from_Unicode_وضعیت==UCNV_NEED_مقصد_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ (حرف)0xffu, (حرف)0xfeu, 0, 0 };
        ucnv_from_UWrite_بایتها(آرگومانها_2->converter,
                             bom, 4,
                             &آرگومانها_2->هدف, آرگومانها_2->هدف_حد,
                             &آرگومانها_2->offsets, -1,
                             err);
        آرگومانها_2->converter->from_Unicode_وضعیت=0;
    }

    my_هدف = (بیعلامت حرف *) آرگومانها_2->هدف;
    myOffsets = آرگومانها_2->offsets;
    موقت[3] = 0;

    إذا (آرگومانها_2->converter->fromUChar32)
    {
        ch = آرگومانها_2->converter->fromUChar32;
        آرگومانها_2->converter->fromUChar32 = 0;
        اذهب lowsurogate;
    }

    طالما (my_منبع < منبع_حد && my_هدف < هدف_حد)
    {
        ch = *(my_منبع++);

        إذا (U16_IS_SURROGATE(ch)) {
            إذا (U16_IS_LEAD(ch))
            {
lowsurogate:
                إذا (my_منبع < منبع_حد)
                {
                    ch2 = *my_منبع;
                    إذا (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_بالا_شروع) << HALF_لغزش) + ch2 + SURROGATE_کم_پایه;
                        my_منبع++;
                    }
                    وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        آرگومانها_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_یافت;
                        اكسر;
                    }
                }
                وإلا {
                    /* ran out of source */
                    آرگومانها_2->converter->fromUChar32 = ch;
                    إذا (آرگومانها_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_یافت;
                    }
                    اكسر;
                }
            }
            وإلا {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                آرگومانها_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_یافت;
                اكسر;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        موقت[2] = (uint8_t) (ch >> 16 & 0x1F);
        موقت[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        موقت[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        لكل (شاخص_مقصد_Write = 0; شاخص_مقصد_Write <= حجم(uint32_t) - 1; شاخص_مقصد_Write++)
        {
            إذا (my_هدف < هدف_حد)
            {
                *(my_هدف++) = موقت[شاخص_مقصد_Write];
                *(myOffsets++) = جابه_جایی_رقم;
            }
            وإلا
            {
                آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] = موقت[شاخص_مقصد_Write];
                *err = U_میانگیر_سرریز_ERROR;
            }
        }
        جابه_جایی_رقم = جابه_جایی_رقم + 1 + (موقت[2] != 0);
    }

    إذا (my_منبع < منبع_حد && my_هدف >= هدف_حد && U_SUCCESS(*err))
    {
        *err = U_میانگیر_سرریز_ERROR;
    }

    آرگومانها_2->هدف = (حرف *) my_هدف;
    آرگومانها_2->منبع = my_منبع;
    آرگومانها_2->offsets = myOffsets;
}

ساكن UChar32 U_CALLCONV
T_UConverter_get_بعدی_UChar_UTF32_LE(UConverter_مقصد_Unicode_آرگومانها* آرگومانها_2,
                                   UError_کد* err)
{
    ثابت uint8_t *my_منبع;
    UChar32 myUChar;
    int32_t length;

    my_منبع = (ثابت uint8_t *)آرگومانها_2->منبع;
    إذا (my_منبع >= (ثابت uint8_t *)آرگومانها_2->منبع_حد)
    {
        /* no input */
        *err = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    length = (int32_t)((ثابت uint8_t *)آرگومانها_2->منبع_حد - my_منبع);
    إذا (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(آرگومانها_2->converter->مقصد_UBytes, my_منبع, length);
        آرگومانها_2->converter->مقصد_ULength = (int8_t)length;
        آرگومانها_2->منبع = (ثابت حرف *)(my_منبع + length);
        *err = U_TRUNCATED_CHAR_یافت;
        أرجع 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)my_منبع[3] << 24)
            | ((UChar32)my_منبع[2] << 16)
            | ((UChar32)my_منبع[1] << 8)
            | ((UChar32)my_منبع[0]);

    آرگومانها_2->منبع = (ثابت حرف *)(my_منبع + 4);
    إذا ((uint32_t)myUChar <= بیشینه_UTF && !U_IS_SURROGATE(myUChar)) {
        أرجع myUChar;
    }

    uprv_memcpy(آرگومانها_2->converter->مقصد_UBytes, my_منبع, 4);
    آرگومانها_2->converter->مقصد_ULength = 4;

    *err = U_ILLEGAL_CHAR_یافت;
    أرجع 0xffff;
}
U_CDECL_انتها
ساكن ثابت UConverterImpl _UTF32LEImpl = {
    UCNV_UTF32_LittleEndian,

    اشاره_تهی,
    اشاره_تهی,

    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,

    T_UConverter_مقصد_Unicode_UTF32_LE,
    T_UConverter_مقصد_Unicode_UTF32_LE_جابه_جایی_LOGIC,
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_جابه_جایی_LOGIC,
    T_UConverter_get_بعدی_UChar_UTF32_LE,

    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
    ucnv_get_Non_Surrogate_Unicode_مجموعه,

    اشاره_تهی,
    اشاره_تهی
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
ساكن ثابت UConverter_Static_داده UTF32_LEStatic_داده = {
    حجم(UConverter_Static_داده),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_داده _UTF32LEData =
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&UTF32_LEStatic_داده, &_UTF32LEImpl);

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
    إذا(choice<=UCNV_RESET_مقصد_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->نما=0;
    }
    إذا(choice!=UCNV_RESET_مقصد_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->from_Unicode_وضعیت=UCNV_NEED_مقصد_WRITE_BOM;
    }
}

ساكن فراغ U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverter_بارگیری_آرگومانها *p_آرگومانها,
           UError_کد *p_Error_کد) {
    (فراغ)p_آرگومانها;
    (فراغ)p_Error_کد;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

ساكن ثابت حرف utf32BOM[8]={ 0, 0, (حرف)0xfeu, (حرف)0xffu, (حرف)0xffu, (حرف)0xfeu, 0, 0 };

ساكن فراغ U_CALLCONV
UTF32_مقصد_Unicode_With_Offsets(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                           UError_کد *p_Error_کد) {
    UConverter *cnv=p_آرگومانها->converter;
    ثابت حرف *منبع=p_آرگومانها->منبع;
    ثابت حرف *منبع_حد=p_آرگومانها->منبع_حد;
    int32_t *offsets=p_آرگومانها->offsets;

    int32_t state, جابه_جایی_Delta;
    حرف b;

    state=cnv->نما;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    جابه_جایی_Delta=0;

    طالما(منبع<منبع_حد && U_SUCCESS(*p_Error_کد)) {
        بدل(state) {
        حالة 0:
            b=*منبع;
            إذا(b==0) {
                state=1; /* could be 00 00 FE FF */
            } وإلا إذا(b==(حرف)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } وإلا {
                state=8; /* default to UTF-32BE */
                استمر;
            }
            ++منبع;
            اكسر;
        حالة 1:
        حالة 2:
        حالة 3:
        حالة 5:
        حالة 6:
        حالة 7:
            إذا(*منبع==utf32BOM[state]) {
                ++state;
                ++منبع;
                إذا(state==4) {
                    state=8; /* detect UTF-32BE */
                    جابه_جایی_Delta=(int32_t)(منبع-p_آرگومانها->منبع);
                } وإلا إذا(state==8) {
                    state=9; /* detect UTF-32LE */
                    جابه_جایی_Delta=(int32_t)(منبع-p_آرگومانها->منبع);
                }
            } وإلا {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(منبع-p_آرگومانها->منبع); /* number of bytes from this buffer */

                /* reset the source */
                منبع=p_آرگومانها->منبع;

                إذا(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } وإلا {
                    UBool oldFlush=p_آرگومانها->flush;

                    /* some of the bytes are from a previous buffer, replay those first */
                    p_آرگومانها->منبع=utf32BOM+(state&4); /* select the correct BOM */
                    p_آرگومانها->منبع_حد=p_آرگومانها->منبع+((state&3)-count); /* replay previous bytes */
                    p_آرگومانها->flush=خطأ; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_مقصد_Unicode_UTF32_BE(p_آرگومانها, p_Error_کد);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    p_آرگومانها->منبع_حد=منبع_حد;
                    p_آرگومانها->flush=oldFlush;
                }
                state=8;
                استمر;
            }
            اكسر;
        حالة 8:
            /* call UTF-32BE */
            p_آرگومانها->منبع=منبع;
            إذا(offsets==اشاره_تهی) {
                T_UConverter_مقصد_Unicode_UTF32_BE(p_آرگومانها, p_Error_کد);
            } وإلا {
                T_UConverter_مقصد_Unicode_UTF32_BE_جابه_جایی_LOGIC(p_آرگومانها, p_Error_کد);
            }
            منبع=p_آرگومانها->منبع;
            اكسر;
        حالة 9:
            /* call UTF-32LE */
            p_آرگومانها->منبع=منبع;
            إذا(offsets==اشاره_تهی) {
                T_UConverter_مقصد_Unicode_UTF32_LE(p_آرگومانها, p_Error_کد);
            } وإلا {
                T_UConverter_مقصد_Unicode_UTF32_LE_جابه_جایی_LOGIC(p_آرگومانها, p_Error_کد);
            }
            منبع=p_آرگومانها->منبع;
            اكسر;
        افتراضي:
            اكسر; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    إذا(offsets!=اشاره_تهی && جابه_جایی_Delta!=0) {
        int32_t *offsets_حد=p_آرگومانها->offsets;
        طالما(offsets<offsets_حد) {
            *offsets++ += جابه_جایی_Delta;
        }
    }

    p_آرگومانها->منبع=منبع;

    إذا(منبع==منبع_حد && p_آرگومانها->flush) {
        /* handle truncated input */
        بدل(state) {
        حالة 0:
            اكسر; /* no input at all, nothing to do */
        حالة 8:
            T_UConverter_مقصد_Unicode_UTF32_BE(p_آرگومانها, p_Error_کد);
            اكسر;
        حالة 9:
            T_UConverter_مقصد_Unicode_UTF32_LE(p_آرگومانها, p_Error_کد);
            اكسر;
        افتراضي:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            p_آرگومانها->منبع=utf32BOM+(state&4); /* select the correct BOM */
            p_آرگومانها->منبع_حد=p_آرگومانها->منبع+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_مقصد_Unicode_UTF32_BE(p_آرگومانها, p_Error_کد);
            p_آرگومانها->منبع=منبع;
            p_آرگومانها->منبع_حد=منبع_حد;
            state=8;
            اكسر;
        }
    }

    cnv->نما=state;
}

ساكن UChar32 U_CALLCONV
UTF32_Get_بعدی_UChar(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                   UError_کد *p_Error_کد) {
    بدل(p_آرگومانها->converter->نما) {
    حالة 8:
        أرجع T_UConverter_get_بعدی_UChar_UTF32_BE(p_آرگومانها, p_Error_کد);
    حالة 9:
        أرجع T_UConverter_get_بعدی_UChar_UTF32_LE(p_آرگومانها, p_Error_کد);
    افتراضي:
        أرجع UCNV_GET_بعدی_UCHAR_USE_مقصد_U;
    }
}
U_CDECL_انتها
ساكن ثابت UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    اشاره_تهی,
    اشاره_تهی,

    _UTF32Open,
    اشاره_تهی,
    _UTF32Reset,

    UTF32_مقصد_Unicode_With_Offsets,
    UTF32_مقصد_Unicode_With_Offsets,
#إذا U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_جابه_جایی_LOGIC,
#وإلا
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_جابه_جایی_LOGIC,
#پایان_اگر
    UTF32_Get_بعدی_UChar,

    اشاره_تهی, /* ### TODO implement getStarters for all Unicode encodings?! */
    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
    ucnv_get_Non_Surrogate_Unicode_مجموعه,

    اشاره_تهی,
    اشاره_تهی
};

/* The 1236 CCSID refers to any version of Unicode with a BOM sensitive endianness of UTF-32 */
ساكن ثابت UConverter_Static_داده UTF32_Static_داده = {
    حجم(UConverter_Static_داده),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#إذا U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#وإلا
    { 0xfd, 0xff, 0, 0 }, 4,
#پایان_اگر
    خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_داده UTF32_داده = 
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&UTF32_Static_داده, &_UTF32Impl);

#پایان_اگر
