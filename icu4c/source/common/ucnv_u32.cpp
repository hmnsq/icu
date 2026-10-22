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

#تعریف_کړه اعظمي_UCS2            0x0000FFFF
#تعریف_کړه اعظمي_UTF             0x0010FFFF
#تعریف_کړه HALF_ښويدنه              10
#تعریف_کړه HALF_بنسټ               0x0010000
#تعریف_کړه HALF_ماسک               0x3FF
#تعریف_کړه SURROGATE_لوړ_پیل    0xD800
#تعریف_کړه SURROGATE_ټیټ_پیل     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#تعریف_کړه SURROGATE_ټیټ_بنسټ      9216

تعداد {
    UCNV_NEED_منزل_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
T_UConverter_منزل_Unicode_UTF32_BE(UConverter_منزل_Unicode_دلیلونه * دلیلونه_2,
                                UError_کوډ * err)
{
    ثابت بینښه حرف *my_سرچینه = (بینښه حرف *) دلیلونه_2->سرچینه;
    char16_t *my_موخه = دلیلونه_2->موخه;
    ثابت بینښه حرف *سرچینه_حد = (بینښه حرف *) دلیلونه_2->سرچینه_حد;
    ثابت char16_t *موخه_حد = دلیلونه_2->موخه_حد;
    بینښه حرف *منزل_UBytes = دلیلونه_2->converter->منزل_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    إذا (دلیلونه_2->converter->منزل_ULength > 0 && my_موخه < موخه_حد) {
        i = دلیلونه_2->converter->منزل_ULength;       /* restore # of bytes consumed */
        دلیلونه_2->converter->منزل_ULength = 0;

        ch = دلیلونه_2->converter->منزل_Unicode_حالت - 1;/*Stores the previously calculated ch from a previous call*/
        دلیلونه_2->converter->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }

    طالما (my_سرچینه < سرچینه_حد && my_موخه < موخه_حد) {
        i = 0;
        ch = 0;
morebytes:
        طالما (i < حجم(uint32_t)) {
            إذا (my_سرچینه < سرچینه_حد) {
                ch = (ch << 8) | (uint8_t)(*my_سرچینه);
                منزل_UBytes[i++] = (حرف) *(my_سرچینه++);
            }
            وإلا {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                دلیلونه_2->converter->منزل_Unicode_حالت = ch + 1;
                دلیلونه_2->converter->منزل_ULength = (int8_t) i;
                اذهب donefornow;
            }
        }

        إذا (ch <= اعظمي_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            إذا (ch <= اعظمي_UCS2) 
            {
                /* fits in 16 bits */
                *(my_موخه++) = (char16_t) ch;
            }
            وإلا {
                /* write out the surrogates */
                *(my_موخه++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                إذا (my_موخه < موخه_حد) {
                    *(my_موخه++) = (char16_t)ch;
                }
                وإلا {
                    /* Put in overflow buffer (not handled here) */
                    دلیلونه_2->converter->UChar_Error_بفر[0] = (char16_t) ch;
                    دلیلونه_2->converter->UChar_Error_بفر_اوږدتیا = 1;
                    *err = U_بفر_ډکېدنه_ERROR;
                    اكسر;
                }
            }
        }
        وإلا {
            دلیلونه_2->converter->منزل_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_وموندل;
            اكسر;
        }
    }

donefornow:
    إذا (my_سرچینه < سرچینه_حد && my_موخه >= موخه_حد && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_بفر_ډکېدنه_ERROR;
    }

    دلیلونه_2->موخه = my_موخه;
    دلیلونه_2->سرچینه = (ثابت حرف *) my_سرچینه;
}

ساكن فراغ U_CALLCONV
T_UConverter_منزل_Unicode_UTF32_BE_دننه_واټن_LOGIC(UConverter_منزل_Unicode_دلیلونه * دلیلونه_2,
                                             UError_کوډ * err)
{
    ثابت بینښه حرف *my_سرچینه = (بینښه حرف *) دلیلونه_2->سرچینه;
    char16_t *my_موخه = دلیلونه_2->موخه;
    int32_t *myOffsets = دلیلونه_2->offsets;
    ثابت بینښه حرف *سرچینه_حد = (بینښه حرف *) دلیلونه_2->سرچینه_حد;
    ثابت char16_t *موخه_حد = دلیلونه_2->موخه_حد;
    بینښه حرف *منزل_UBytes = دلیلونه_2->converter->منزل_UBytes;
    uint32_t ch, i;
    int32_t دننه_واټن_عدد = 0;

    /* Restore state of current sequence */
    إذا (دلیلونه_2->converter->منزل_ULength > 0 && my_موخه < موخه_حد) {
        i = دلیلونه_2->converter->منزل_ULength;       /* restore # of bytes consumed */
        دلیلونه_2->converter->منزل_ULength = 0;

        ch = دلیلونه_2->converter->منزل_Unicode_حالت - 1;/*Stores the previously calculated ch from a previous call*/
        دلیلونه_2->converter->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }

    طالما (my_سرچینه < سرچینه_حد && my_موخه < موخه_حد) {
        i = 0;
        ch = 0;
morebytes:
        طالما (i < حجم(uint32_t)) {
            إذا (my_سرچینه < سرچینه_حد) {
                ch = (ch << 8) | (uint8_t)(*my_سرچینه);
                منزل_UBytes[i++] = (حرف) *(my_سرچینه++);
            }
            وإلا {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                دلیلونه_2->converter->منزل_Unicode_حالت = ch + 1;
                دلیلونه_2->converter->منزل_ULength = (int8_t) i;
                اذهب donefornow;
            }
        }

        إذا (ch <= اعظمي_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            إذا (ch <= اعظمي_UCS2) {
                /* fits in 16 bits */
                *(my_موخه++) = (char16_t) ch;
                *(myOffsets++) = دننه_واټن_عدد;
            }
            وإلا {
                /* write out the surrogates */
                *(my_موخه++) = U16_LEAD(ch);
                *myOffsets++ = دننه_واټن_عدد;
                ch = U16_TRAIL(ch);
                إذا (my_موخه < موخه_حد)
                {
                    *(my_موخه++) = (char16_t)ch;
                    *(myOffsets++) = دننه_واټن_عدد;
                }
                وإلا {
                    /* Put in overflow buffer (not handled here) */
                    دلیلونه_2->converter->UChar_Error_بفر[0] = (char16_t) ch;
                    دلیلونه_2->converter->UChar_Error_بفر_اوږدتیا = 1;
                    *err = U_بفر_ډکېدنه_ERROR;
                    اكسر;
                }
            }
        }
        وإلا {
            دلیلونه_2->converter->منزل_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_وموندل;
            اكسر;
        }
        دننه_واټن_عدد += i;
    }

donefornow:
    إذا (my_سرچینه < سرچینه_حد && my_موخه >= موخه_حد && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_بفر_ډکېدنه_ERROR;
    }

    دلیلونه_2->موخه = my_موخه;
    دلیلونه_2->سرچینه = (ثابت حرف *) my_سرچینه;
    دلیلونه_2->offsets = myOffsets;
}

ساكن فراغ U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverter_From_Unicode_دلیلونه * دلیلونه_2,
                                  UError_کوډ * err)
{
    ثابت char16_t *my_سرچینه = دلیلونه_2->سرچینه;
    بینښه حرف *my_موخه;
    ثابت char16_t *سرچینه_حد = دلیلونه_2->سرچینه_حد;
    ثابت بینښه حرف *موخه_حد = (بینښه حرف *) دلیلونه_2->موخه_حد;
    UChar32 ch, ch2;
    بینښه صحيح شاخص_منزل_Write;
    بینښه حرف لنډمهاله[حجم(uint32_t)];

    إذا(my_سرچینه >= سرچینه_حد) {
        /* no input, nothing to do */
        أرجع;
    }

    /* write the BOM if necessary */
    إذا(دلیلونه_2->converter->from_Unicode_حالت==UCNV_NEED_منزل_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ 0, 0, (حرف)0xfeu, (حرف)0xffu };
        ucnv_from_UWrite_بایټونه(دلیلونه_2->converter,
                             bom, 4,
                             &دلیلونه_2->موخه, دلیلونه_2->موخه_حد,
                             &دلیلونه_2->offsets, -1,
                             err);
        دلیلونه_2->converter->from_Unicode_حالت=0;
    }

    my_موخه = (بینښه حرف *) دلیلونه_2->موخه;
    لنډمهاله[0] = 0;

    إذا (دلیلونه_2->converter->fromUChar32) {
        ch = دلیلونه_2->converter->fromUChar32;
        دلیلونه_2->converter->fromUChar32 = 0;
        اذهب lowsurogate;
    }

    طالما (my_سرچینه < سرچینه_حد && my_موخه < موخه_حد) {
        ch = *(my_سرچینه++);

        إذا (U_IS_SURROGATE(ch)) {
            إذا (U_IS_LEAD(ch)) {
lowsurogate:
                إذا (my_سرچینه < سرچینه_حد) {
                    ch2 = *my_سرچینه;
                    إذا (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_لوړ_پیل) << HALF_ښويدنه) + ch2 + SURROGATE_ټیټ_بنسټ;
                        my_سرچینه++;
                    }
                    وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        دلیلونه_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_وموندل;
                        اكسر;
                    }
                }
                وإلا {
                    /* ran out of source */
                    دلیلونه_2->converter->fromUChar32 = ch;
                    إذا (دلیلونه_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_وموندل;
                    }
                    اكسر;
                }
            }
            وإلا {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                دلیلونه_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_وموندل;
                اكسر;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        لنډمهاله[1] = (uint8_t) (ch >> 16 & 0x1F);
        لنډمهاله[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        لنډمهاله[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        لكل (شاخص_منزل_Write = 0; شاخص_منزل_Write <= حجم(uint32_t) - 1; شاخص_منزل_Write++) {
            إذا (my_موخه < موخه_حد) {
                *(my_موخه++) = لنډمهاله[شاخص_منزل_Write];
            }
            وإلا {
                دلیلونه_2->converter->char_Error_بفر[دلیلونه_2->converter->char_Error_بفر_اوږدتیا++] = لنډمهاله[شاخص_منزل_Write];
                *err = U_بفر_ډکېدنه_ERROR;
            }
        }
    }

    إذا (my_سرچینه < سرچینه_حد && my_موخه >= موخه_حد && U_SUCCESS(*err)) {
        *err = U_بفر_ډکېدنه_ERROR;
    }

    دلیلونه_2->موخه = (حرف *) my_موخه;
    دلیلونه_2->سرچینه = my_سرچینه;
}

ساكن فراغ U_CALLCONV
T_UConverter_from_Unicode_UTF32_BE_دننه_واټن_LOGIC(UConverter_From_Unicode_دلیلونه * دلیلونه_2,
                                               UError_کوډ * err)
{
    ثابت char16_t *my_سرچینه = دلیلونه_2->سرچینه;
    بینښه حرف *my_موخه;
    int32_t *myOffsets;
    ثابت char16_t *سرچینه_حد = دلیلونه_2->سرچینه_حد;
    ثابت بینښه حرف *موخه_حد = (بینښه حرف *) دلیلونه_2->موخه_حد;
    UChar32 ch, ch2;
    int32_t دننه_واټن_عدد = 0;
    بینښه صحيح شاخص_منزل_Write;
    بینښه حرف لنډمهاله[حجم(uint32_t)];

    إذا(my_سرچینه >= سرچینه_حد) {
        /* no input, nothing to do */
        أرجع;
    }

    /* write the BOM if necessary */
    إذا(دلیلونه_2->converter->from_Unicode_حالت==UCNV_NEED_منزل_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ 0, 0, (حرف)0xfeu, (حرف)0xffu };
        ucnv_from_UWrite_بایټونه(دلیلونه_2->converter,
                             bom, 4,
                             &دلیلونه_2->موخه, دلیلونه_2->موخه_حد,
                             &دلیلونه_2->offsets, -1,
                             err);
        دلیلونه_2->converter->from_Unicode_حالت=0;
    }

    my_موخه = (بینښه حرف *) دلیلونه_2->موخه;
    myOffsets = دلیلونه_2->offsets;
    لنډمهاله[0] = 0;

    إذا (دلیلونه_2->converter->fromUChar32) {
        ch = دلیلونه_2->converter->fromUChar32;
        دلیلونه_2->converter->fromUChar32 = 0;
        اذهب lowsurogate;
    }

    طالما (my_سرچینه < سرچینه_حد && my_موخه < موخه_حد) {
        ch = *(my_سرچینه++);

        إذا (U_IS_SURROGATE(ch)) {
            إذا (U_IS_LEAD(ch)) {
lowsurogate:
                إذا (my_سرچینه < سرچینه_حد) {
                    ch2 = *my_سرچینه;
                    إذا (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_لوړ_پیل) << HALF_ښويدنه) + ch2 + SURROGATE_ټیټ_بنسټ;
                        my_سرچینه++;
                    }
                    وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        دلیلونه_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_وموندل;
                        اكسر;
                    }
                }
                وإلا {
                    /* ran out of source */
                    دلیلونه_2->converter->fromUChar32 = ch;
                    إذا (دلیلونه_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_وموندل;
                    }
                    اكسر;
                }
            }
            وإلا {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                دلیلونه_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_وموندل;
                اكسر;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        لنډمهاله[1] = (uint8_t) (ch >> 16 & 0x1F);
        لنډمهاله[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        لنډمهاله[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        لكل (شاخص_منزل_Write = 0; شاخص_منزل_Write <= حجم(uint32_t) - 1; شاخص_منزل_Write++) {
            إذا (my_موخه < موخه_حد) {
                *(my_موخه++) = لنډمهاله[شاخص_منزل_Write];
                *(myOffsets++) = دننه_واټن_عدد;
            }
            وإلا {
                دلیلونه_2->converter->char_Error_بفر[دلیلونه_2->converter->char_Error_بفر_اوږدتیا++] = لنډمهاله[شاخص_منزل_Write];
                *err = U_بفر_ډکېدنه_ERROR;
            }
        }
        دننه_واټن_عدد = دننه_واټن_عدد + 1 + (لنډمهاله[1] != 0);
    }

    إذا (my_سرچینه < سرچینه_حد && my_موخه >= موخه_حد && U_SUCCESS(*err)) {
        *err = U_بفر_ډکېدنه_ERROR;
    }

    دلیلونه_2->موخه = (حرف *) my_موخه;
    دلیلونه_2->سرچینه = my_سرچینه;
    دلیلونه_2->offsets = myOffsets;
}

ساكن UChar32 U_CALLCONV
T_UConverter_get_بل_UChar_UTF32_BE(UConverter_منزل_Unicode_دلیلونه* دلیلونه_2,
                                   UError_کوډ* err)
{
    ثابت uint8_t *my_سرچینه;
    UChar32 myUChar;
    int32_t length;

    my_سرچینه = (ثابت uint8_t *)دلیلونه_2->سرچینه;
    إذا (my_سرچینه >= (ثابت uint8_t *)دلیلونه_2->سرچینه_حد)
    {
        /* no input */
        *err = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    length = (int32_t)((ثابت uint8_t *)دلیلونه_2->سرچینه_حد - my_سرچینه);
    إذا (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(دلیلونه_2->converter->منزل_UBytes, my_سرچینه, length);
        دلیلونه_2->converter->منزل_ULength = (int8_t)length;
        دلیلونه_2->سرچینه = (ثابت حرف *)(my_سرچینه + length);
        *err = U_TRUNCATED_CHAR_وموندل;
        أرجع 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)my_سرچینه[0] << 24)
            | ((UChar32)my_سرچینه[1] << 16)
            | ((UChar32)my_سرچینه[2] << 8)
            | ((UChar32)my_سرچینه[3]);

    دلیلونه_2->سرچینه = (ثابت حرف *)(my_سرچینه + 4);
    إذا ((uint32_t)myUChar <= اعظمي_UTF && !U_IS_SURROGATE(myUChar)) {
        أرجع myUChar;
    }

    uprv_memcpy(دلیلونه_2->converter->منزل_UBytes, my_سرچینه, 4);
    دلیلونه_2->converter->منزل_ULength = 4;

    *err = U_ILLEGAL_CHAR_وموندل;
    أرجع 0xffff;
}
U_CDECL_پای_ټکی
ساكن ثابت UConverterImpl _UTF32BEImpl = {
    UCNV_UTF32_BigEndian,

    تش_اشاره,
    تش_اشاره,

    تش_اشاره,
    تش_اشاره,
    تش_اشاره,

    T_UConverter_منزل_Unicode_UTF32_BE,
    T_UConverter_منزل_Unicode_UTF32_BE_دننه_واټن_LOGIC,
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_دننه_واټن_LOGIC,
    T_UConverter_get_بل_UChar_UTF32_BE,

    تش_اشاره,
    تش_اشاره,
    تش_اشاره,
    تش_اشاره,
    ucnv_get_Non_Surrogate_Unicode_سېټ,

    تش_اشاره,
    تش_اشاره
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
ساكن ثابت UConverter_Static_معلومات UTF32_BEStatic_معلومات = {
    حجم(UConverter_Static_معلومات),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_معلومات _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_معلومات_INITIALIZER(&UTF32_BEStatic_معلومات, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
T_UConverter_منزل_Unicode_UTF32_LE(UConverter_منزل_Unicode_دلیلونه * دلیلونه_2,
                                UError_کوډ * err)
{
    ثابت بینښه حرف *my_سرچینه = (بینښه حرف *) دلیلونه_2->سرچینه;
    char16_t *my_موخه = دلیلونه_2->موخه;
    ثابت بینښه حرف *سرچینه_حد = (بینښه حرف *) دلیلونه_2->سرچینه_حد;
    ثابت char16_t *موخه_حد = دلیلونه_2->موخه_حد;
    بینښه حرف *منزل_UBytes = دلیلونه_2->converter->منزل_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    إذا (دلیلونه_2->converter->منزل_ULength > 0 && my_موخه < موخه_حد)
    {
        i = دلیلونه_2->converter->منزل_ULength;       /* restore # of bytes consumed */
        دلیلونه_2->converter->منزل_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = دلیلونه_2->converter->منزل_Unicode_حالت - 1;
        دلیلونه_2->converter->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }

    طالما (my_سرچینه < سرچینه_حد && my_موخه < موخه_حد)
    {
        i = 0;
        ch = 0;
morebytes:
        طالما (i < حجم(uint32_t))
        {
            إذا (my_سرچینه < سرچینه_حد)
            {
                ch |= ((uint8_t)(*my_سرچینه)) << (i * 8);
                منزل_UBytes[i++] = (حرف) *(my_سرچینه++);
            }
            وإلا
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                دلیلونه_2->converter->منزل_Unicode_حالت = ch + 1;
                دلیلونه_2->converter->منزل_ULength = (int8_t) i;
                اذهب donefornow;
            }
        }

        إذا (ch <= اعظمي_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            إذا (ch <= اعظمي_UCS2) {
                /* fits in 16 bits */
                *(my_موخه++) = (char16_t) ch;
            }
            وإلا {
                /* write out the surrogates */
                *(my_موخه++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                إذا (my_موخه < موخه_حد) {
                    *(my_موخه++) = (char16_t)ch;
                }
                وإلا {
                    /* Put in overflow buffer (not handled here) */
                    دلیلونه_2->converter->UChar_Error_بفر[0] = (char16_t) ch;
                    دلیلونه_2->converter->UChar_Error_بفر_اوږدتیا = 1;
                    *err = U_بفر_ډکېدنه_ERROR;
                    اكسر;
                }
            }
        }
        وإلا {
            دلیلونه_2->converter->منزل_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_وموندل;
            اكسر;
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

ساكن فراغ U_CALLCONV
T_UConverter_منزل_Unicode_UTF32_LE_دننه_واټن_LOGIC(UConverter_منزل_Unicode_دلیلونه * دلیلونه_2,
                                             UError_کوډ * err)
{
    ثابت بینښه حرف *my_سرچینه = (بینښه حرف *) دلیلونه_2->سرچینه;
    char16_t *my_موخه = دلیلونه_2->موخه;
    int32_t *myOffsets = دلیلونه_2->offsets;
    ثابت بینښه حرف *سرچینه_حد = (بینښه حرف *) دلیلونه_2->سرچینه_حد;
    ثابت char16_t *موخه_حد = دلیلونه_2->موخه_حد;
    بینښه حرف *منزل_UBytes = دلیلونه_2->converter->منزل_UBytes;
    uint32_t ch, i;
    int32_t دننه_واټن_عدد = 0;

    /* Restore state of current sequence */
    إذا (دلیلونه_2->converter->منزل_ULength > 0 && my_موخه < موخه_حد)
    {
        i = دلیلونه_2->converter->منزل_ULength;       /* restore # of bytes consumed */
        دلیلونه_2->converter->منزل_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = دلیلونه_2->converter->منزل_Unicode_حالت - 1;
        دلیلونه_2->converter->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }

    طالما (my_سرچینه < سرچینه_حد && my_موخه < موخه_حد)
    {
        i = 0;
        ch = 0;
morebytes:
        طالما (i < حجم(uint32_t))
        {
            إذا (my_سرچینه < سرچینه_حد)
            {
                ch |= ((uint8_t)(*my_سرچینه)) << (i * 8);
                منزل_UBytes[i++] = (حرف) *(my_سرچینه++);
            }
            وإلا
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                دلیلونه_2->converter->منزل_Unicode_حالت = ch + 1;
                دلیلونه_2->converter->منزل_ULength = (int8_t) i;
                اذهب donefornow;
            }
        }

        إذا (ch <= اعظمي_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            إذا (ch <= اعظمي_UCS2) 
            {
                /* fits in 16 bits */
                *(my_موخه++) = (char16_t) ch;
                *(myOffsets++) = دننه_واټن_عدد;
            }
            وإلا {
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
                    /* Put in overflow buffer (not handled here) */
                    دلیلونه_2->converter->UChar_Error_بفر[0] = (char16_t) ch;
                    دلیلونه_2->converter->UChar_Error_بفر_اوږدتیا = 1;
                    *err = U_بفر_ډکېدنه_ERROR;
                    اكسر;
                }
            }
        }
        وإلا
        {
            دلیلونه_2->converter->منزل_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_وموندل;
            اكسر;
        }
        دننه_واټن_عدد += i;
    }

donefornow:
    إذا (my_سرچینه < سرچینه_حد && my_موخه >= موخه_حد && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_بفر_ډکېدنه_ERROR;
    }

    دلیلونه_2->موخه = my_موخه;
    دلیلونه_2->سرچینه = (ثابت حرف *) my_سرچینه;
    دلیلونه_2->offsets = myOffsets;
}

ساكن فراغ U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverter_From_Unicode_دلیلونه * دلیلونه_2,
                                  UError_کوډ * err)
{
    ثابت char16_t *my_سرچینه = دلیلونه_2->سرچینه;
    بینښه حرف *my_موخه;
    ثابت char16_t *سرچینه_حد = دلیلونه_2->سرچینه_حد;
    ثابت بینښه حرف *موخه_حد = (بینښه حرف *) دلیلونه_2->موخه_حد;
    UChar32 ch, ch2;
    بینښه صحيح شاخص_منزل_Write;
    بینښه حرف لنډمهاله[حجم(uint32_t)];

    إذا(my_سرچینه >= سرچینه_حد) {
        /* no input, nothing to do */
        أرجع;
    }

    /* write the BOM if necessary */
    إذا(دلیلونه_2->converter->from_Unicode_حالت==UCNV_NEED_منزل_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ (حرف)0xffu, (حرف)0xfeu, 0, 0 };
        ucnv_from_UWrite_بایټونه(دلیلونه_2->converter,
                             bom, 4,
                             &دلیلونه_2->موخه, دلیلونه_2->موخه_حد,
                             &دلیلونه_2->offsets, -1,
                             err);
        دلیلونه_2->converter->from_Unicode_حالت=0;
    }

    my_موخه = (بینښه حرف *) دلیلونه_2->موخه;
    لنډمهاله[3] = 0;

    إذا (دلیلونه_2->converter->fromUChar32)
    {
        ch = دلیلونه_2->converter->fromUChar32;
        دلیلونه_2->converter->fromUChar32 = 0;
        اذهب lowsurogate;
    }

    طالما (my_سرچینه < سرچینه_حد && my_موخه < موخه_حد)
    {
        ch = *(my_سرچینه++);

        إذا (U16_IS_SURROGATE(ch)) {
            إذا (U16_IS_LEAD(ch))
            {
lowsurogate:
                إذا (my_سرچینه < سرچینه_حد)
                {
                    ch2 = *my_سرچینه;
                    إذا (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_لوړ_پیل) << HALF_ښويدنه) + ch2 + SURROGATE_ټیټ_بنسټ;
                        my_سرچینه++;
                    }
                    وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        دلیلونه_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_وموندل;
                        اكسر;
                    }
                }
                وإلا {
                    /* ran out of source */
                    دلیلونه_2->converter->fromUChar32 = ch;
                    إذا (دلیلونه_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_وموندل;
                    }
                    اكسر;
                }
            }
            وإلا {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                دلیلونه_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_وموندل;
                اكسر;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        لنډمهاله[2] = (uint8_t) (ch >> 16 & 0x1F);
        لنډمهاله[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        لنډمهاله[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        لكل (شاخص_منزل_Write = 0; شاخص_منزل_Write <= حجم(uint32_t) - 1; شاخص_منزل_Write++)
        {
            إذا (my_موخه < موخه_حد)
            {
                *(my_موخه++) = لنډمهاله[شاخص_منزل_Write];
            }
            وإلا
            {
                دلیلونه_2->converter->char_Error_بفر[دلیلونه_2->converter->char_Error_بفر_اوږدتیا++] = لنډمهاله[شاخص_منزل_Write];
                *err = U_بفر_ډکېدنه_ERROR;
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

ساكن فراغ U_CALLCONV
T_UConverter_from_Unicode_UTF32_LE_دننه_واټن_LOGIC(UConverter_From_Unicode_دلیلونه * دلیلونه_2,
                                               UError_کوډ * err)
{
    ثابت char16_t *my_سرچینه = دلیلونه_2->سرچینه;
    بینښه حرف *my_موخه;
    int32_t *myOffsets;
    ثابت char16_t *سرچینه_حد = دلیلونه_2->سرچینه_حد;
    ثابت بینښه حرف *موخه_حد = (بینښه حرف *) دلیلونه_2->موخه_حد;
    UChar32 ch, ch2;
    بینښه صحيح شاخص_منزل_Write;
    بینښه حرف لنډمهاله[حجم(uint32_t)];
    int32_t دننه_واټن_عدد = 0;

    إذا(my_سرچینه >= سرچینه_حد) {
        /* no input, nothing to do */
        أرجع;
    }

    /* write the BOM if necessary */
    إذا(دلیلونه_2->converter->from_Unicode_حالت==UCNV_NEED_منزل_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ (حرف)0xffu, (حرف)0xfeu, 0, 0 };
        ucnv_from_UWrite_بایټونه(دلیلونه_2->converter,
                             bom, 4,
                             &دلیلونه_2->موخه, دلیلونه_2->موخه_حد,
                             &دلیلونه_2->offsets, -1,
                             err);
        دلیلونه_2->converter->from_Unicode_حالت=0;
    }

    my_موخه = (بینښه حرف *) دلیلونه_2->موخه;
    myOffsets = دلیلونه_2->offsets;
    لنډمهاله[3] = 0;

    إذا (دلیلونه_2->converter->fromUChar32)
    {
        ch = دلیلونه_2->converter->fromUChar32;
        دلیلونه_2->converter->fromUChar32 = 0;
        اذهب lowsurogate;
    }

    طالما (my_سرچینه < سرچینه_حد && my_موخه < موخه_حد)
    {
        ch = *(my_سرچینه++);

        إذا (U16_IS_SURROGATE(ch)) {
            إذا (U16_IS_LEAD(ch))
            {
lowsurogate:
                إذا (my_سرچینه < سرچینه_حد)
                {
                    ch2 = *my_سرچینه;
                    إذا (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_لوړ_پیل) << HALF_ښويدنه) + ch2 + SURROGATE_ټیټ_بنسټ;
                        my_سرچینه++;
                    }
                    وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        دلیلونه_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_وموندل;
                        اكسر;
                    }
                }
                وإلا {
                    /* ran out of source */
                    دلیلونه_2->converter->fromUChar32 = ch;
                    إذا (دلیلونه_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_وموندل;
                    }
                    اكسر;
                }
            }
            وإلا {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                دلیلونه_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_وموندل;
                اكسر;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        لنډمهاله[2] = (uint8_t) (ch >> 16 & 0x1F);
        لنډمهاله[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        لنډمهاله[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        لكل (شاخص_منزل_Write = 0; شاخص_منزل_Write <= حجم(uint32_t) - 1; شاخص_منزل_Write++)
        {
            إذا (my_موخه < موخه_حد)
            {
                *(my_موخه++) = لنډمهاله[شاخص_منزل_Write];
                *(myOffsets++) = دننه_واټن_عدد;
            }
            وإلا
            {
                دلیلونه_2->converter->char_Error_بفر[دلیلونه_2->converter->char_Error_بفر_اوږدتیا++] = لنډمهاله[شاخص_منزل_Write];
                *err = U_بفر_ډکېدنه_ERROR;
            }
        }
        دننه_واټن_عدد = دننه_واټن_عدد + 1 + (لنډمهاله[2] != 0);
    }

    إذا (my_سرچینه < سرچینه_حد && my_موخه >= موخه_حد && U_SUCCESS(*err))
    {
        *err = U_بفر_ډکېدنه_ERROR;
    }

    دلیلونه_2->موخه = (حرف *) my_موخه;
    دلیلونه_2->سرچینه = my_سرچینه;
    دلیلونه_2->offsets = myOffsets;
}

ساكن UChar32 U_CALLCONV
T_UConverter_get_بل_UChar_UTF32_LE(UConverter_منزل_Unicode_دلیلونه* دلیلونه_2,
                                   UError_کوډ* err)
{
    ثابت uint8_t *my_سرچینه;
    UChar32 myUChar;
    int32_t length;

    my_سرچینه = (ثابت uint8_t *)دلیلونه_2->سرچینه;
    إذا (my_سرچینه >= (ثابت uint8_t *)دلیلونه_2->سرچینه_حد)
    {
        /* no input */
        *err = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    length = (int32_t)((ثابت uint8_t *)دلیلونه_2->سرچینه_حد - my_سرچینه);
    إذا (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(دلیلونه_2->converter->منزل_UBytes, my_سرچینه, length);
        دلیلونه_2->converter->منزل_ULength = (int8_t)length;
        دلیلونه_2->سرچینه = (ثابت حرف *)(my_سرچینه + length);
        *err = U_TRUNCATED_CHAR_وموندل;
        أرجع 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)my_سرچینه[3] << 24)
            | ((UChar32)my_سرچینه[2] << 16)
            | ((UChar32)my_سرچینه[1] << 8)
            | ((UChar32)my_سرچینه[0]);

    دلیلونه_2->سرچینه = (ثابت حرف *)(my_سرچینه + 4);
    إذا ((uint32_t)myUChar <= اعظمي_UTF && !U_IS_SURROGATE(myUChar)) {
        أرجع myUChar;
    }

    uprv_memcpy(دلیلونه_2->converter->منزل_UBytes, my_سرچینه, 4);
    دلیلونه_2->converter->منزل_ULength = 4;

    *err = U_ILLEGAL_CHAR_وموندل;
    أرجع 0xffff;
}
U_CDECL_پای_ټکی
ساكن ثابت UConverterImpl _UTF32LEImpl = {
    UCNV_UTF32_LittleEndian,

    تش_اشاره,
    تش_اشاره,

    تش_اشاره,
    تش_اشاره,
    تش_اشاره,

    T_UConverter_منزل_Unicode_UTF32_LE,
    T_UConverter_منزل_Unicode_UTF32_LE_دننه_واټن_LOGIC,
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_دننه_واټن_LOGIC,
    T_UConverter_get_بل_UChar_UTF32_LE,

    تش_اشاره,
    تش_اشاره,
    تش_اشاره,
    تش_اشاره,
    ucnv_get_Non_Surrogate_Unicode_سېټ,

    تش_اشاره,
    تش_اشاره
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
ساكن ثابت UConverter_Static_معلومات UTF32_LEStatic_معلومات = {
    حجم(UConverter_Static_معلومات),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_معلومات _UTF32LEData =
        UCNV_IMMUTABLE_SHARED_معلومات_INITIALIZER(&UTF32_LEStatic_معلومات, &_UTF32LEImpl);

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
        cnv->ډېرپېښ=0;
    }
    إذا(choice!=UCNV_RESET_منزل_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->from_Unicode_حالت=UCNV_NEED_منزل_WRITE_BOM;
    }
}

ساكن فراغ U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverter_بارول_دلیلونه *p_دلیلونه,
           UError_کوډ *p_Error_کوډ) {
    (فراغ)p_دلیلونه;
    (فراغ)p_Error_کوډ;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

ساكن ثابت حرف utf32BOM[8]={ 0, 0, (حرف)0xfeu, (حرف)0xffu, (حرف)0xffu, (حرف)0xfeu, 0, 0 };

ساكن فراغ U_CALLCONV
UTF32_منزل_Unicode_With_Offsets(UConverter_منزل_Unicode_دلیلونه *p_دلیلونه,
                           UError_کوډ *p_Error_کوډ) {
    UConverter *cnv=p_دلیلونه->converter;
    ثابت حرف *سرچینه=p_دلیلونه->سرچینه;
    ثابت حرف *سرچینه_حد=p_دلیلونه->سرچینه_حد;
    int32_t *offsets=p_دلیلونه->offsets;

    int32_t state, دننه_واټن_Delta;
    حرف b;

    state=cnv->ډېرپېښ;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    دننه_واټن_Delta=0;

    طالما(سرچینه<سرچینه_حد && U_SUCCESS(*p_Error_کوډ)) {
        بدل(state) {
        حالة 0:
            b=*سرچینه;
            إذا(b==0) {
                state=1; /* could be 00 00 FE FF */
            } وإلا إذا(b==(حرف)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } وإلا {
                state=8; /* default to UTF-32BE */
                استمر;
            }
            ++سرچینه;
            اكسر;
        حالة 1:
        حالة 2:
        حالة 3:
        حالة 5:
        حالة 6:
        حالة 7:
            إذا(*سرچینه==utf32BOM[state]) {
                ++state;
                ++سرچینه;
                إذا(state==4) {
                    state=8; /* detect UTF-32BE */
                    دننه_واټن_Delta=(int32_t)(سرچینه-p_دلیلونه->سرچینه);
                } وإلا إذا(state==8) {
                    state=9; /* detect UTF-32LE */
                    دننه_واټن_Delta=(int32_t)(سرچینه-p_دلیلونه->سرچینه);
                }
            } وإلا {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(سرچینه-p_دلیلونه->سرچینه); /* number of bytes from this buffer */

                /* reset the source */
                سرچینه=p_دلیلونه->سرچینه;

                إذا(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } وإلا {
                    UBool oldFlush=p_دلیلونه->flush;

                    /* some of the bytes are from a previous buffer, replay those first */
                    p_دلیلونه->سرچینه=utf32BOM+(state&4); /* select the correct BOM */
                    p_دلیلونه->سرچینه_حد=p_دلیلونه->سرچینه+((state&3)-count); /* replay previous bytes */
                    p_دلیلونه->flush=خطأ; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_منزل_Unicode_UTF32_BE(p_دلیلونه, p_Error_کوډ);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    p_دلیلونه->سرچینه_حد=سرچینه_حد;
                    p_دلیلونه->flush=oldFlush;
                }
                state=8;
                استمر;
            }
            اكسر;
        حالة 8:
            /* call UTF-32BE */
            p_دلیلونه->سرچینه=سرچینه;
            إذا(offsets==تش_اشاره) {
                T_UConverter_منزل_Unicode_UTF32_BE(p_دلیلونه, p_Error_کوډ);
            } وإلا {
                T_UConverter_منزل_Unicode_UTF32_BE_دننه_واټن_LOGIC(p_دلیلونه, p_Error_کوډ);
            }
            سرچینه=p_دلیلونه->سرچینه;
            اكسر;
        حالة 9:
            /* call UTF-32LE */
            p_دلیلونه->سرچینه=سرچینه;
            إذا(offsets==تش_اشاره) {
                T_UConverter_منزل_Unicode_UTF32_LE(p_دلیلونه, p_Error_کوډ);
            } وإلا {
                T_UConverter_منزل_Unicode_UTF32_LE_دننه_واټن_LOGIC(p_دلیلونه, p_Error_کوډ);
            }
            سرچینه=p_دلیلونه->سرچینه;
            اكسر;
        افتراضي:
            اكسر; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    إذا(offsets!=تش_اشاره && دننه_واټن_Delta!=0) {
        int32_t *offsets_حد=p_دلیلونه->offsets;
        طالما(offsets<offsets_حد) {
            *offsets++ += دننه_واټن_Delta;
        }
    }

    p_دلیلونه->سرچینه=سرچینه;

    إذا(سرچینه==سرچینه_حد && p_دلیلونه->flush) {
        /* handle truncated input */
        بدل(state) {
        حالة 0:
            اكسر; /* no input at all, nothing to do */
        حالة 8:
            T_UConverter_منزل_Unicode_UTF32_BE(p_دلیلونه, p_Error_کوډ);
            اكسر;
        حالة 9:
            T_UConverter_منزل_Unicode_UTF32_LE(p_دلیلونه, p_Error_کوډ);
            اكسر;
        افتراضي:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            p_دلیلونه->سرچینه=utf32BOM+(state&4); /* select the correct BOM */
            p_دلیلونه->سرچینه_حد=p_دلیلونه->سرچینه+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_منزل_Unicode_UTF32_BE(p_دلیلونه, p_Error_کوډ);
            p_دلیلونه->سرچینه=سرچینه;
            p_دلیلونه->سرچینه_حد=سرچینه_حد;
            state=8;
            اكسر;
        }
    }

    cnv->ډېرپېښ=state;
}

ساكن UChar32 U_CALLCONV
UTF32_Get_بل_UChar(UConverter_منزل_Unicode_دلیلونه *p_دلیلونه,
                   UError_کوډ *p_Error_کوډ) {
    بدل(p_دلیلونه->converter->ډېرپېښ) {
    حالة 8:
        أرجع T_UConverter_get_بل_UChar_UTF32_BE(p_دلیلونه, p_Error_کوډ);
    حالة 9:
        أرجع T_UConverter_get_بل_UChar_UTF32_LE(p_دلیلونه, p_Error_کوډ);
    افتراضي:
        أرجع UCNV_GET_بل_UCHAR_USE_منزل_U;
    }
}
U_CDECL_پای_ټکی
ساكن ثابت UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    تش_اشاره,
    تش_اشاره,

    _UTF32Open,
    تش_اشاره,
    _UTF32Reset,

    UTF32_منزل_Unicode_With_Offsets,
    UTF32_منزل_Unicode_With_Offsets,
#إذا U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_دننه_واټن_LOGIC,
#وإلا
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_دننه_واټن_LOGIC,
#د_که_پای
    UTF32_Get_بل_UChar,

    تش_اشاره, /* ### TODO implement getStarters for all Unicode encodings?! */
    تش_اشاره,
    تش_اشاره,
    تش_اشاره,
    ucnv_get_Non_Surrogate_Unicode_سېټ,

    تش_اشاره,
    تش_اشاره
};

/* The 1236 CCSID refers to any version of Unicode with a BOM sensitive endianness of UTF-32 */
ساكن ثابت UConverter_Static_معلومات UTF32_Static_معلومات = {
    حجم(UConverter_Static_معلومات),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#إذا U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#وإلا
    { 0xfd, 0xff, 0, 0 }, 4,
#د_که_پای
    خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_معلومات UTF32_معلومات = 
        UCNV_IMMUTABLE_SHARED_معلومات_INITIALIZER(&UTF32_Static_معلومات, &_UTF32Impl);

#د_که_پای
