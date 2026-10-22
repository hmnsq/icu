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

#وصف_ڪريو وڌ_کان_وڌ_UCS2            0x0000FFFF
#وصف_ڪريو وڌ_کان_وڌ_UTF             0x0010FFFF
#وصف_ڪريو HALF_سرڪايو              10
#وصف_ڪريو HALF_بنياد               0x0010000
#وصف_ڪريو HALF_نقاب               0x3FF
#وصف_ڪريو SURROGATE_مٿاهون_شروعات    0xD800
#وصف_ڪريو SURROGATE_هيٺيون_شروعات     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#وصف_ڪريو SURROGATE_هيٺيون_بنياد      9216

تعداد {
    UCNV_NEED_منزل_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
T_UConverter_منزل_Unicode_UTF32_BE(UConverter_منزل_Unicode_دليلون * دليلون_2,
                                UError_ڪوڊ * err)
{
    ثابت بنانشان حرف *my_ذريعو = (بنانشان حرف *) دليلون_2->ذريعو;
    char16_t *my_هدف = دليلون_2->هدف;
    ثابت بنانشان حرف *ذريعو_حد_بندي = (بنانشان حرف *) دليلون_2->ذريعو_حد_بندي;
    ثابت char16_t *هدف_حد_بندي = دليلون_2->هدف_حد_بندي;
    بنانشان حرف *منزل_UBytes = دليلون_2->converter->منزل_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    إذا (دليلون_2->converter->منزل_ULength > 0 && my_هدف < هدف_حد_بندي) {
        i = دليلون_2->converter->منزل_ULength;       /* restore # of bytes consumed */
        دليلون_2->converter->منزل_ULength = 0;

        ch = دليلون_2->converter->منزل_Unicode_حالت - 1;/*Stores the previously calculated ch from a previous call*/
        دليلون_2->converter->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }

    طالما (my_ذريعو < ذريعو_حد_بندي && my_هدف < هدف_حد_بندي) {
        i = 0;
        ch = 0;
morebytes:
        طالما (i < حجم(uint32_t)) {
            إذا (my_ذريعو < ذريعو_حد_بندي) {
                ch = (ch << 8) | (uint8_t)(*my_ذريعو);
                منزل_UBytes[i++] = (حرف) *(my_ذريعو++);
            }
            وإلا {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                دليلون_2->converter->منزل_Unicode_حالت = ch + 1;
                دليلون_2->converter->منزل_ULength = (int8_t) i;
                اذهب donefornow;
            }
        }

        إذا (ch <= وڌ_کان_وڌ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            إذا (ch <= وڌ_کان_وڌ_UCS2) 
            {
                /* fits in 16 bits */
                *(my_هدف++) = (char16_t) ch;
            }
            وإلا {
                /* write out the surrogates */
                *(my_هدف++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                إذا (my_هدف < هدف_حد_بندي) {
                    *(my_هدف++) = (char16_t)ch;
                }
                وإلا {
                    /* Put in overflow buffer (not handled here) */
                    دليلون_2->converter->UChar_Error_بفر[0] = (char16_t) ch;
                    دليلون_2->converter->UChar_Error_بفر_ڊيگهائي = 1;
                    *err = U_بفر_حد_کان_مٿي_ERROR;
                    اكسر;
                }
            }
        }
        وإلا {
            دليلون_2->converter->منزل_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_مليو;
            اكسر;
        }
    }

donefornow:
    إذا (my_ذريعو < ذريعو_حد_بندي && my_هدف >= هدف_حد_بندي && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_بفر_حد_کان_مٿي_ERROR;
    }

    دليلون_2->هدف = my_هدف;
    دليلون_2->ذريعو = (ثابت حرف *) my_ذريعو;
}

ساكن فراغ U_CALLCONV
T_UConverter_منزل_Unicode_UTF32_BE_وٿي_LOGIC(UConverter_منزل_Unicode_دليلون * دليلون_2,
                                             UError_ڪوڊ * err)
{
    ثابت بنانشان حرف *my_ذريعو = (بنانشان حرف *) دليلون_2->ذريعو;
    char16_t *my_هدف = دليلون_2->هدف;
    int32_t *myOffsets = دليلون_2->offsets;
    ثابت بنانشان حرف *ذريعو_حد_بندي = (بنانشان حرف *) دليلون_2->ذريعو_حد_بندي;
    ثابت char16_t *هدف_حد_بندي = دليلون_2->هدف_حد_بندي;
    بنانشان حرف *منزل_UBytes = دليلون_2->converter->منزل_UBytes;
    uint32_t ch, i;
    int32_t وٿي_انگ = 0;

    /* Restore state of current sequence */
    إذا (دليلون_2->converter->منزل_ULength > 0 && my_هدف < هدف_حد_بندي) {
        i = دليلون_2->converter->منزل_ULength;       /* restore # of bytes consumed */
        دليلون_2->converter->منزل_ULength = 0;

        ch = دليلون_2->converter->منزل_Unicode_حالت - 1;/*Stores the previously calculated ch from a previous call*/
        دليلون_2->converter->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }

    طالما (my_ذريعو < ذريعو_حد_بندي && my_هدف < هدف_حد_بندي) {
        i = 0;
        ch = 0;
morebytes:
        طالما (i < حجم(uint32_t)) {
            إذا (my_ذريعو < ذريعو_حد_بندي) {
                ch = (ch << 8) | (uint8_t)(*my_ذريعو);
                منزل_UBytes[i++] = (حرف) *(my_ذريعو++);
            }
            وإلا {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                دليلون_2->converter->منزل_Unicode_حالت = ch + 1;
                دليلون_2->converter->منزل_ULength = (int8_t) i;
                اذهب donefornow;
            }
        }

        إذا (ch <= وڌ_کان_وڌ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            إذا (ch <= وڌ_کان_وڌ_UCS2) {
                /* fits in 16 bits */
                *(my_هدف++) = (char16_t) ch;
                *(myOffsets++) = وٿي_انگ;
            }
            وإلا {
                /* write out the surrogates */
                *(my_هدف++) = U16_LEAD(ch);
                *myOffsets++ = وٿي_انگ;
                ch = U16_TRAIL(ch);
                إذا (my_هدف < هدف_حد_بندي)
                {
                    *(my_هدف++) = (char16_t)ch;
                    *(myOffsets++) = وٿي_انگ;
                }
                وإلا {
                    /* Put in overflow buffer (not handled here) */
                    دليلون_2->converter->UChar_Error_بفر[0] = (char16_t) ch;
                    دليلون_2->converter->UChar_Error_بفر_ڊيگهائي = 1;
                    *err = U_بفر_حد_کان_مٿي_ERROR;
                    اكسر;
                }
            }
        }
        وإلا {
            دليلون_2->converter->منزل_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_مليو;
            اكسر;
        }
        وٿي_انگ += i;
    }

donefornow:
    إذا (my_ذريعو < ذريعو_حد_بندي && my_هدف >= هدف_حد_بندي && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_بفر_حد_کان_مٿي_ERROR;
    }

    دليلون_2->هدف = my_هدف;
    دليلون_2->ذريعو = (ثابت حرف *) my_ذريعو;
    دليلون_2->offsets = myOffsets;
}

ساكن فراغ U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverter_From_Unicode_دليلون * دليلون_2,
                                  UError_ڪوڊ * err)
{
    ثابت char16_t *my_ذريعو = دليلون_2->ذريعو;
    بنانشان حرف *my_هدف;
    ثابت char16_t *ذريعو_حد_بندي = دليلون_2->ذريعو_حد_بندي;
    ثابت بنانشان حرف *هدف_حد_بندي = (بنانشان حرف *) دليلون_2->هدف_حد_بندي;
    UChar32 ch, ch2;
    بنانشان صحيح اشاريو_منزل_Write;
    بنانشان حرف عارضي[حجم(uint32_t)];

    إذا(my_ذريعو >= ذريعو_حد_بندي) {
        /* no input, nothing to do */
        أرجع;
    }

    /* write the BOM if necessary */
    إذا(دليلون_2->converter->from_Unicode_حالت==UCNV_NEED_منزل_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ 0, 0, (حرف)0xfeu, (حرف)0xffu };
        ucnv_from_UWrite_بائيٽون(دليلون_2->converter,
                             bom, 4,
                             &دليلون_2->هدف, دليلون_2->هدف_حد_بندي,
                             &دليلون_2->offsets, -1,
                             err);
        دليلون_2->converter->from_Unicode_حالت=0;
    }

    my_هدف = (بنانشان حرف *) دليلون_2->هدف;
    عارضي[0] = 0;

    إذا (دليلون_2->converter->fromUChar32) {
        ch = دليلون_2->converter->fromUChar32;
        دليلون_2->converter->fromUChar32 = 0;
        اذهب lowsurogate;
    }

    طالما (my_ذريعو < ذريعو_حد_بندي && my_هدف < هدف_حد_بندي) {
        ch = *(my_ذريعو++);

        إذا (U_IS_SURROGATE(ch)) {
            إذا (U_IS_LEAD(ch)) {
lowsurogate:
                إذا (my_ذريعو < ذريعو_حد_بندي) {
                    ch2 = *my_ذريعو;
                    إذا (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_مٿاهون_شروعات) << HALF_سرڪايو) + ch2 + SURROGATE_هيٺيون_بنياد;
                        my_ذريعو++;
                    }
                    وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        دليلون_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_مليو;
                        اكسر;
                    }
                }
                وإلا {
                    /* ran out of source */
                    دليلون_2->converter->fromUChar32 = ch;
                    إذا (دليلون_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_مليو;
                    }
                    اكسر;
                }
            }
            وإلا {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                دليلون_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_مليو;
                اكسر;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        عارضي[1] = (uint8_t) (ch >> 16 & 0x1F);
        عارضي[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        عارضي[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        لكل (اشاريو_منزل_Write = 0; اشاريو_منزل_Write <= حجم(uint32_t) - 1; اشاريو_منزل_Write++) {
            إذا (my_هدف < هدف_حد_بندي) {
                *(my_هدف++) = عارضي[اشاريو_منزل_Write];
            }
            وإلا {
                دليلون_2->converter->char_Error_بفر[دليلون_2->converter->char_Error_بفر_ڊيگهائي++] = عارضي[اشاريو_منزل_Write];
                *err = U_بفر_حد_کان_مٿي_ERROR;
            }
        }
    }

    إذا (my_ذريعو < ذريعو_حد_بندي && my_هدف >= هدف_حد_بندي && U_SUCCESS(*err)) {
        *err = U_بفر_حد_کان_مٿي_ERROR;
    }

    دليلون_2->هدف = (حرف *) my_هدف;
    دليلون_2->ذريعو = my_ذريعو;
}

ساكن فراغ U_CALLCONV
T_UConverter_from_Unicode_UTF32_BE_وٿي_LOGIC(UConverter_From_Unicode_دليلون * دليلون_2,
                                               UError_ڪوڊ * err)
{
    ثابت char16_t *my_ذريعو = دليلون_2->ذريعو;
    بنانشان حرف *my_هدف;
    int32_t *myOffsets;
    ثابت char16_t *ذريعو_حد_بندي = دليلون_2->ذريعو_حد_بندي;
    ثابت بنانشان حرف *هدف_حد_بندي = (بنانشان حرف *) دليلون_2->هدف_حد_بندي;
    UChar32 ch, ch2;
    int32_t وٿي_انگ = 0;
    بنانشان صحيح اشاريو_منزل_Write;
    بنانشان حرف عارضي[حجم(uint32_t)];

    إذا(my_ذريعو >= ذريعو_حد_بندي) {
        /* no input, nothing to do */
        أرجع;
    }

    /* write the BOM if necessary */
    إذا(دليلون_2->converter->from_Unicode_حالت==UCNV_NEED_منزل_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ 0, 0, (حرف)0xfeu, (حرف)0xffu };
        ucnv_from_UWrite_بائيٽون(دليلون_2->converter,
                             bom, 4,
                             &دليلون_2->هدف, دليلون_2->هدف_حد_بندي,
                             &دليلون_2->offsets, -1,
                             err);
        دليلون_2->converter->from_Unicode_حالت=0;
    }

    my_هدف = (بنانشان حرف *) دليلون_2->هدف;
    myOffsets = دليلون_2->offsets;
    عارضي[0] = 0;

    إذا (دليلون_2->converter->fromUChar32) {
        ch = دليلون_2->converter->fromUChar32;
        دليلون_2->converter->fromUChar32 = 0;
        اذهب lowsurogate;
    }

    طالما (my_ذريعو < ذريعو_حد_بندي && my_هدف < هدف_حد_بندي) {
        ch = *(my_ذريعو++);

        إذا (U_IS_SURROGATE(ch)) {
            إذا (U_IS_LEAD(ch)) {
lowsurogate:
                إذا (my_ذريعو < ذريعو_حد_بندي) {
                    ch2 = *my_ذريعو;
                    إذا (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_مٿاهون_شروعات) << HALF_سرڪايو) + ch2 + SURROGATE_هيٺيون_بنياد;
                        my_ذريعو++;
                    }
                    وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        دليلون_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_مليو;
                        اكسر;
                    }
                }
                وإلا {
                    /* ran out of source */
                    دليلون_2->converter->fromUChar32 = ch;
                    إذا (دليلون_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_مليو;
                    }
                    اكسر;
                }
            }
            وإلا {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                دليلون_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_مليو;
                اكسر;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        عارضي[1] = (uint8_t) (ch >> 16 & 0x1F);
        عارضي[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        عارضي[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        لكل (اشاريو_منزل_Write = 0; اشاريو_منزل_Write <= حجم(uint32_t) - 1; اشاريو_منزل_Write++) {
            إذا (my_هدف < هدف_حد_بندي) {
                *(my_هدف++) = عارضي[اشاريو_منزل_Write];
                *(myOffsets++) = وٿي_انگ;
            }
            وإلا {
                دليلون_2->converter->char_Error_بفر[دليلون_2->converter->char_Error_بفر_ڊيگهائي++] = عارضي[اشاريو_منزل_Write];
                *err = U_بفر_حد_کان_مٿي_ERROR;
            }
        }
        وٿي_انگ = وٿي_انگ + 1 + (عارضي[1] != 0);
    }

    إذا (my_ذريعو < ذريعو_حد_بندي && my_هدف >= هدف_حد_بندي && U_SUCCESS(*err)) {
        *err = U_بفر_حد_کان_مٿي_ERROR;
    }

    دليلون_2->هدف = (حرف *) my_هدف;
    دليلون_2->ذريعو = my_ذريعو;
    دليلون_2->offsets = myOffsets;
}

ساكن UChar32 U_CALLCONV
T_UConverter_get_ايندڙ_UChar_UTF32_BE(UConverter_منزل_Unicode_دليلون* دليلون_2,
                                   UError_ڪوڊ* err)
{
    ثابت uint8_t *my_ذريعو;
    UChar32 myUChar;
    int32_t length;

    my_ذريعو = (ثابت uint8_t *)دليلون_2->ذريعو;
    إذا (my_ذريعو >= (ثابت uint8_t *)دليلون_2->ذريعو_حد_بندي)
    {
        /* no input */
        *err = U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    length = (int32_t)((ثابت uint8_t *)دليلون_2->ذريعو_حد_بندي - my_ذريعو);
    إذا (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(دليلون_2->converter->منزل_UBytes, my_ذريعو, length);
        دليلون_2->converter->منزل_ULength = (int8_t)length;
        دليلون_2->ذريعو = (ثابت حرف *)(my_ذريعو + length);
        *err = U_TRUNCATED_CHAR_مليو;
        أرجع 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)my_ذريعو[0] << 24)
            | ((UChar32)my_ذريعو[1] << 16)
            | ((UChar32)my_ذريعو[2] << 8)
            | ((UChar32)my_ذريعو[3]);

    دليلون_2->ذريعو = (ثابت حرف *)(my_ذريعو + 4);
    إذا ((uint32_t)myUChar <= وڌ_کان_وڌ_UTF && !U_IS_SURROGATE(myUChar)) {
        أرجع myUChar;
    }

    uprv_memcpy(دليلون_2->converter->منزل_UBytes, my_ذريعو, 4);
    دليلون_2->converter->منزل_ULength = 4;

    *err = U_ILLEGAL_CHAR_مليو;
    أرجع 0xffff;
}
U_CDECL_پڇاڙي
ساكن ثابت UConverterImpl _UTF32BEImpl = {
    UCNV_UTF32_BigEndian,

    خالي_اشارو,
    خالي_اشارو,

    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,

    T_UConverter_منزل_Unicode_UTF32_BE,
    T_UConverter_منزل_Unicode_UTF32_BE_وٿي_LOGIC,
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_وٿي_LOGIC,
    T_UConverter_get_ايندڙ_UChar_UTF32_BE,

    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,
    ucnv_get_Non_Surrogate_Unicode_سيٽ,

    خالي_اشارو,
    خالي_اشارو
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
T_UConverter_منزل_Unicode_UTF32_LE(UConverter_منزل_Unicode_دليلون * دليلون_2,
                                UError_ڪوڊ * err)
{
    ثابت بنانشان حرف *my_ذريعو = (بنانشان حرف *) دليلون_2->ذريعو;
    char16_t *my_هدف = دليلون_2->هدف;
    ثابت بنانشان حرف *ذريعو_حد_بندي = (بنانشان حرف *) دليلون_2->ذريعو_حد_بندي;
    ثابت char16_t *هدف_حد_بندي = دليلون_2->هدف_حد_بندي;
    بنانشان حرف *منزل_UBytes = دليلون_2->converter->منزل_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    إذا (دليلون_2->converter->منزل_ULength > 0 && my_هدف < هدف_حد_بندي)
    {
        i = دليلون_2->converter->منزل_ULength;       /* restore # of bytes consumed */
        دليلون_2->converter->منزل_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = دليلون_2->converter->منزل_Unicode_حالت - 1;
        دليلون_2->converter->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }

    طالما (my_ذريعو < ذريعو_حد_بندي && my_هدف < هدف_حد_بندي)
    {
        i = 0;
        ch = 0;
morebytes:
        طالما (i < حجم(uint32_t))
        {
            إذا (my_ذريعو < ذريعو_حد_بندي)
            {
                ch |= ((uint8_t)(*my_ذريعو)) << (i * 8);
                منزل_UBytes[i++] = (حرف) *(my_ذريعو++);
            }
            وإلا
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                دليلون_2->converter->منزل_Unicode_حالت = ch + 1;
                دليلون_2->converter->منزل_ULength = (int8_t) i;
                اذهب donefornow;
            }
        }

        إذا (ch <= وڌ_کان_وڌ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            إذا (ch <= وڌ_کان_وڌ_UCS2) {
                /* fits in 16 bits */
                *(my_هدف++) = (char16_t) ch;
            }
            وإلا {
                /* write out the surrogates */
                *(my_هدف++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                إذا (my_هدف < هدف_حد_بندي) {
                    *(my_هدف++) = (char16_t)ch;
                }
                وإلا {
                    /* Put in overflow buffer (not handled here) */
                    دليلون_2->converter->UChar_Error_بفر[0] = (char16_t) ch;
                    دليلون_2->converter->UChar_Error_بفر_ڊيگهائي = 1;
                    *err = U_بفر_حد_کان_مٿي_ERROR;
                    اكسر;
                }
            }
        }
        وإلا {
            دليلون_2->converter->منزل_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_مليو;
            اكسر;
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

ساكن فراغ U_CALLCONV
T_UConverter_منزل_Unicode_UTF32_LE_وٿي_LOGIC(UConverter_منزل_Unicode_دليلون * دليلون_2,
                                             UError_ڪوڊ * err)
{
    ثابت بنانشان حرف *my_ذريعو = (بنانشان حرف *) دليلون_2->ذريعو;
    char16_t *my_هدف = دليلون_2->هدف;
    int32_t *myOffsets = دليلون_2->offsets;
    ثابت بنانشان حرف *ذريعو_حد_بندي = (بنانشان حرف *) دليلون_2->ذريعو_حد_بندي;
    ثابت char16_t *هدف_حد_بندي = دليلون_2->هدف_حد_بندي;
    بنانشان حرف *منزل_UBytes = دليلون_2->converter->منزل_UBytes;
    uint32_t ch, i;
    int32_t وٿي_انگ = 0;

    /* Restore state of current sequence */
    إذا (دليلون_2->converter->منزل_ULength > 0 && my_هدف < هدف_حد_بندي)
    {
        i = دليلون_2->converter->منزل_ULength;       /* restore # of bytes consumed */
        دليلون_2->converter->منزل_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = دليلون_2->converter->منزل_Unicode_حالت - 1;
        دليلون_2->converter->منزل_Unicode_حالت = 0;
        اذهب morebytes;
    }

    طالما (my_ذريعو < ذريعو_حد_بندي && my_هدف < هدف_حد_بندي)
    {
        i = 0;
        ch = 0;
morebytes:
        طالما (i < حجم(uint32_t))
        {
            إذا (my_ذريعو < ذريعو_حد_بندي)
            {
                ch |= ((uint8_t)(*my_ذريعو)) << (i * 8);
                منزل_UBytes[i++] = (حرف) *(my_ذريعو++);
            }
            وإلا
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                دليلون_2->converter->منزل_Unicode_حالت = ch + 1;
                دليلون_2->converter->منزل_ULength = (int8_t) i;
                اذهب donefornow;
            }
        }

        إذا (ch <= وڌ_کان_وڌ_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            إذا (ch <= وڌ_کان_وڌ_UCS2) 
            {
                /* fits in 16 bits */
                *(my_هدف++) = (char16_t) ch;
                *(myOffsets++) = وٿي_انگ;
            }
            وإلا {
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
                    /* Put in overflow buffer (not handled here) */
                    دليلون_2->converter->UChar_Error_بفر[0] = (char16_t) ch;
                    دليلون_2->converter->UChar_Error_بفر_ڊيگهائي = 1;
                    *err = U_بفر_حد_کان_مٿي_ERROR;
                    اكسر;
                }
            }
        }
        وإلا
        {
            دليلون_2->converter->منزل_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_مليو;
            اكسر;
        }
        وٿي_انگ += i;
    }

donefornow:
    إذا (my_ذريعو < ذريعو_حد_بندي && my_هدف >= هدف_حد_بندي && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_بفر_حد_کان_مٿي_ERROR;
    }

    دليلون_2->هدف = my_هدف;
    دليلون_2->ذريعو = (ثابت حرف *) my_ذريعو;
    دليلون_2->offsets = myOffsets;
}

ساكن فراغ U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverter_From_Unicode_دليلون * دليلون_2,
                                  UError_ڪوڊ * err)
{
    ثابت char16_t *my_ذريعو = دليلون_2->ذريعو;
    بنانشان حرف *my_هدف;
    ثابت char16_t *ذريعو_حد_بندي = دليلون_2->ذريعو_حد_بندي;
    ثابت بنانشان حرف *هدف_حد_بندي = (بنانشان حرف *) دليلون_2->هدف_حد_بندي;
    UChar32 ch, ch2;
    بنانشان صحيح اشاريو_منزل_Write;
    بنانشان حرف عارضي[حجم(uint32_t)];

    إذا(my_ذريعو >= ذريعو_حد_بندي) {
        /* no input, nothing to do */
        أرجع;
    }

    /* write the BOM if necessary */
    إذا(دليلون_2->converter->from_Unicode_حالت==UCNV_NEED_منزل_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ (حرف)0xffu, (حرف)0xfeu, 0, 0 };
        ucnv_from_UWrite_بائيٽون(دليلون_2->converter,
                             bom, 4,
                             &دليلون_2->هدف, دليلون_2->هدف_حد_بندي,
                             &دليلون_2->offsets, -1,
                             err);
        دليلون_2->converter->from_Unicode_حالت=0;
    }

    my_هدف = (بنانشان حرف *) دليلون_2->هدف;
    عارضي[3] = 0;

    إذا (دليلون_2->converter->fromUChar32)
    {
        ch = دليلون_2->converter->fromUChar32;
        دليلون_2->converter->fromUChar32 = 0;
        اذهب lowsurogate;
    }

    طالما (my_ذريعو < ذريعو_حد_بندي && my_هدف < هدف_حد_بندي)
    {
        ch = *(my_ذريعو++);

        إذا (U16_IS_SURROGATE(ch)) {
            إذا (U16_IS_LEAD(ch))
            {
lowsurogate:
                إذا (my_ذريعو < ذريعو_حد_بندي)
                {
                    ch2 = *my_ذريعو;
                    إذا (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_مٿاهون_شروعات) << HALF_سرڪايو) + ch2 + SURROGATE_هيٺيون_بنياد;
                        my_ذريعو++;
                    }
                    وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        دليلون_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_مليو;
                        اكسر;
                    }
                }
                وإلا {
                    /* ran out of source */
                    دليلون_2->converter->fromUChar32 = ch;
                    إذا (دليلون_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_مليو;
                    }
                    اكسر;
                }
            }
            وإلا {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                دليلون_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_مليو;
                اكسر;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        عارضي[2] = (uint8_t) (ch >> 16 & 0x1F);
        عارضي[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        عارضي[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        لكل (اشاريو_منزل_Write = 0; اشاريو_منزل_Write <= حجم(uint32_t) - 1; اشاريو_منزل_Write++)
        {
            إذا (my_هدف < هدف_حد_بندي)
            {
                *(my_هدف++) = عارضي[اشاريو_منزل_Write];
            }
            وإلا
            {
                دليلون_2->converter->char_Error_بفر[دليلون_2->converter->char_Error_بفر_ڊيگهائي++] = عارضي[اشاريو_منزل_Write];
                *err = U_بفر_حد_کان_مٿي_ERROR;
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

ساكن فراغ U_CALLCONV
T_UConverter_from_Unicode_UTF32_LE_وٿي_LOGIC(UConverter_From_Unicode_دليلون * دليلون_2,
                                               UError_ڪوڊ * err)
{
    ثابت char16_t *my_ذريعو = دليلون_2->ذريعو;
    بنانشان حرف *my_هدف;
    int32_t *myOffsets;
    ثابت char16_t *ذريعو_حد_بندي = دليلون_2->ذريعو_حد_بندي;
    ثابت بنانشان حرف *هدف_حد_بندي = (بنانشان حرف *) دليلون_2->هدف_حد_بندي;
    UChar32 ch, ch2;
    بنانشان صحيح اشاريو_منزل_Write;
    بنانشان حرف عارضي[حجم(uint32_t)];
    int32_t وٿي_انگ = 0;

    إذا(my_ذريعو >= ذريعو_حد_بندي) {
        /* no input, nothing to do */
        أرجع;
    }

    /* write the BOM if necessary */
    إذا(دليلون_2->converter->from_Unicode_حالت==UCNV_NEED_منزل_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ (حرف)0xffu, (حرف)0xfeu, 0, 0 };
        ucnv_from_UWrite_بائيٽون(دليلون_2->converter,
                             bom, 4,
                             &دليلون_2->هدف, دليلون_2->هدف_حد_بندي,
                             &دليلون_2->offsets, -1,
                             err);
        دليلون_2->converter->from_Unicode_حالت=0;
    }

    my_هدف = (بنانشان حرف *) دليلون_2->هدف;
    myOffsets = دليلون_2->offsets;
    عارضي[3] = 0;

    إذا (دليلون_2->converter->fromUChar32)
    {
        ch = دليلون_2->converter->fromUChar32;
        دليلون_2->converter->fromUChar32 = 0;
        اذهب lowsurogate;
    }

    طالما (my_ذريعو < ذريعو_حد_بندي && my_هدف < هدف_حد_بندي)
    {
        ch = *(my_ذريعو++);

        إذا (U16_IS_SURROGATE(ch)) {
            إذا (U16_IS_LEAD(ch))
            {
lowsurogate:
                إذا (my_ذريعو < ذريعو_حد_بندي)
                {
                    ch2 = *my_ذريعو;
                    إذا (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_مٿاهون_شروعات) << HALF_سرڪايو) + ch2 + SURROGATE_هيٺيون_بنياد;
                        my_ذريعو++;
                    }
                    وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        دليلون_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_مليو;
                        اكسر;
                    }
                }
                وإلا {
                    /* ran out of source */
                    دليلون_2->converter->fromUChar32 = ch;
                    إذا (دليلون_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_مليو;
                    }
                    اكسر;
                }
            }
            وإلا {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                دليلون_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_مليو;
                اكسر;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        عارضي[2] = (uint8_t) (ch >> 16 & 0x1F);
        عارضي[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        عارضي[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        لكل (اشاريو_منزل_Write = 0; اشاريو_منزل_Write <= حجم(uint32_t) - 1; اشاريو_منزل_Write++)
        {
            إذا (my_هدف < هدف_حد_بندي)
            {
                *(my_هدف++) = عارضي[اشاريو_منزل_Write];
                *(myOffsets++) = وٿي_انگ;
            }
            وإلا
            {
                دليلون_2->converter->char_Error_بفر[دليلون_2->converter->char_Error_بفر_ڊيگهائي++] = عارضي[اشاريو_منزل_Write];
                *err = U_بفر_حد_کان_مٿي_ERROR;
            }
        }
        وٿي_انگ = وٿي_انگ + 1 + (عارضي[2] != 0);
    }

    إذا (my_ذريعو < ذريعو_حد_بندي && my_هدف >= هدف_حد_بندي && U_SUCCESS(*err))
    {
        *err = U_بفر_حد_کان_مٿي_ERROR;
    }

    دليلون_2->هدف = (حرف *) my_هدف;
    دليلون_2->ذريعو = my_ذريعو;
    دليلون_2->offsets = myOffsets;
}

ساكن UChar32 U_CALLCONV
T_UConverter_get_ايندڙ_UChar_UTF32_LE(UConverter_منزل_Unicode_دليلون* دليلون_2,
                                   UError_ڪوڊ* err)
{
    ثابت uint8_t *my_ذريعو;
    UChar32 myUChar;
    int32_t length;

    my_ذريعو = (ثابت uint8_t *)دليلون_2->ذريعو;
    إذا (my_ذريعو >= (ثابت uint8_t *)دليلون_2->ذريعو_حد_بندي)
    {
        /* no input */
        *err = U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    length = (int32_t)((ثابت uint8_t *)دليلون_2->ذريعو_حد_بندي - my_ذريعو);
    إذا (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(دليلون_2->converter->منزل_UBytes, my_ذريعو, length);
        دليلون_2->converter->منزل_ULength = (int8_t)length;
        دليلون_2->ذريعو = (ثابت حرف *)(my_ذريعو + length);
        *err = U_TRUNCATED_CHAR_مليو;
        أرجع 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)my_ذريعو[3] << 24)
            | ((UChar32)my_ذريعو[2] << 16)
            | ((UChar32)my_ذريعو[1] << 8)
            | ((UChar32)my_ذريعو[0]);

    دليلون_2->ذريعو = (ثابت حرف *)(my_ذريعو + 4);
    إذا ((uint32_t)myUChar <= وڌ_کان_وڌ_UTF && !U_IS_SURROGATE(myUChar)) {
        أرجع myUChar;
    }

    uprv_memcpy(دليلون_2->converter->منزل_UBytes, my_ذريعو, 4);
    دليلون_2->converter->منزل_ULength = 4;

    *err = U_ILLEGAL_CHAR_مليو;
    أرجع 0xffff;
}
U_CDECL_پڇاڙي
ساكن ثابت UConverterImpl _UTF32LEImpl = {
    UCNV_UTF32_LittleEndian,

    خالي_اشارو,
    خالي_اشارو,

    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,

    T_UConverter_منزل_Unicode_UTF32_LE,
    T_UConverter_منزل_Unicode_UTF32_LE_وٿي_LOGIC,
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_وٿي_LOGIC,
    T_UConverter_get_ايندڙ_UChar_UTF32_LE,

    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,
    ucnv_get_Non_Surrogate_Unicode_سيٽ,

    خالي_اشارو,
    خالي_اشارو
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
        cnv->گهڻائي=0;
    }
    إذا(choice!=UCNV_RESET_منزل_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->from_Unicode_حالت=UCNV_NEED_منزل_WRITE_BOM;
    }
}

ساكن فراغ U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverter_ڀريو_دليلون *p_دليلون,
           UError_ڪوڊ *p_Error_ڪوڊ) {
    (فراغ)p_دليلون;
    (فراغ)p_Error_ڪوڊ;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

ساكن ثابت حرف utf32BOM[8]={ 0, 0, (حرف)0xfeu, (حرف)0xffu, (حرف)0xffu, (حرف)0xfeu, 0, 0 };

ساكن فراغ U_CALLCONV
UTF32_منزل_Unicode_With_Offsets(UConverter_منزل_Unicode_دليلون *p_دليلون,
                           UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv=p_دليلون->converter;
    ثابت حرف *ذريعو=p_دليلون->ذريعو;
    ثابت حرف *ذريعو_حد_بندي=p_دليلون->ذريعو_حد_بندي;
    int32_t *offsets=p_دليلون->offsets;

    int32_t state, وٿي_Delta;
    حرف b;

    state=cnv->گهڻائي;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    وٿي_Delta=0;

    طالما(ذريعو<ذريعو_حد_بندي && U_SUCCESS(*p_Error_ڪوڊ)) {
        بدل(state) {
        حالة 0:
            b=*ذريعو;
            إذا(b==0) {
                state=1; /* could be 00 00 FE FF */
            } وإلا إذا(b==(حرف)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } وإلا {
                state=8; /* default to UTF-32BE */
                استمر;
            }
            ++ذريعو;
            اكسر;
        حالة 1:
        حالة 2:
        حالة 3:
        حالة 5:
        حالة 6:
        حالة 7:
            إذا(*ذريعو==utf32BOM[state]) {
                ++state;
                ++ذريعو;
                إذا(state==4) {
                    state=8; /* detect UTF-32BE */
                    وٿي_Delta=(int32_t)(ذريعو-p_دليلون->ذريعو);
                } وإلا إذا(state==8) {
                    state=9; /* detect UTF-32LE */
                    وٿي_Delta=(int32_t)(ذريعو-p_دليلون->ذريعو);
                }
            } وإلا {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(ذريعو-p_دليلون->ذريعو); /* number of bytes from this buffer */

                /* reset the source */
                ذريعو=p_دليلون->ذريعو;

                إذا(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } وإلا {
                    UBool oldFlush=p_دليلون->flush;

                    /* some of the bytes are from a previous buffer, replay those first */
                    p_دليلون->ذريعو=utf32BOM+(state&4); /* select the correct BOM */
                    p_دليلون->ذريعو_حد_بندي=p_دليلون->ذريعو+((state&3)-count); /* replay previous bytes */
                    p_دليلون->flush=خطأ; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_منزل_Unicode_UTF32_BE(p_دليلون, p_Error_ڪوڊ);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    p_دليلون->ذريعو_حد_بندي=ذريعو_حد_بندي;
                    p_دليلون->flush=oldFlush;
                }
                state=8;
                استمر;
            }
            اكسر;
        حالة 8:
            /* call UTF-32BE */
            p_دليلون->ذريعو=ذريعو;
            إذا(offsets==خالي_اشارو) {
                T_UConverter_منزل_Unicode_UTF32_BE(p_دليلون, p_Error_ڪوڊ);
            } وإلا {
                T_UConverter_منزل_Unicode_UTF32_BE_وٿي_LOGIC(p_دليلون, p_Error_ڪوڊ);
            }
            ذريعو=p_دليلون->ذريعو;
            اكسر;
        حالة 9:
            /* call UTF-32LE */
            p_دليلون->ذريعو=ذريعو;
            إذا(offsets==خالي_اشارو) {
                T_UConverter_منزل_Unicode_UTF32_LE(p_دليلون, p_Error_ڪوڊ);
            } وإلا {
                T_UConverter_منزل_Unicode_UTF32_LE_وٿي_LOGIC(p_دليلون, p_Error_ڪوڊ);
            }
            ذريعو=p_دليلون->ذريعو;
            اكسر;
        افتراضي:
            اكسر; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    إذا(offsets!=خالي_اشارو && وٿي_Delta!=0) {
        int32_t *offsets_حد_بندي=p_دليلون->offsets;
        طالما(offsets<offsets_حد_بندي) {
            *offsets++ += وٿي_Delta;
        }
    }

    p_دليلون->ذريعو=ذريعو;

    إذا(ذريعو==ذريعو_حد_بندي && p_دليلون->flush) {
        /* handle truncated input */
        بدل(state) {
        حالة 0:
            اكسر; /* no input at all, nothing to do */
        حالة 8:
            T_UConverter_منزل_Unicode_UTF32_BE(p_دليلون, p_Error_ڪوڊ);
            اكسر;
        حالة 9:
            T_UConverter_منزل_Unicode_UTF32_LE(p_دليلون, p_Error_ڪوڊ);
            اكسر;
        افتراضي:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            p_دليلون->ذريعو=utf32BOM+(state&4); /* select the correct BOM */
            p_دليلون->ذريعو_حد_بندي=p_دليلون->ذريعو+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_منزل_Unicode_UTF32_BE(p_دليلون, p_Error_ڪوڊ);
            p_دليلون->ذريعو=ذريعو;
            p_دليلون->ذريعو_حد_بندي=ذريعو_حد_بندي;
            state=8;
            اكسر;
        }
    }

    cnv->گهڻائي=state;
}

ساكن UChar32 U_CALLCONV
UTF32_Get_ايندڙ_UChar(UConverter_منزل_Unicode_دليلون *p_دليلون,
                   UError_ڪوڊ *p_Error_ڪوڊ) {
    بدل(p_دليلون->converter->گهڻائي) {
    حالة 8:
        أرجع T_UConverter_get_ايندڙ_UChar_UTF32_BE(p_دليلون, p_Error_ڪوڊ);
    حالة 9:
        أرجع T_UConverter_get_ايندڙ_UChar_UTF32_LE(p_دليلون, p_Error_ڪوڊ);
    افتراضي:
        أرجع UCNV_GET_ايندڙ_UCHAR_USE_منزل_U;
    }
}
U_CDECL_پڇاڙي
ساكن ثابت UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    خالي_اشارو,
    خالي_اشارو,

    _UTF32Open,
    خالي_اشارو,
    _UTF32Reset,

    UTF32_منزل_Unicode_With_Offsets,
    UTF32_منزل_Unicode_With_Offsets,
#إذا U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_وٿي_LOGIC,
#وإلا
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_وٿي_LOGIC,
#جيڪڏهن_پورو
    UTF32_Get_ايندڙ_UChar,

    خالي_اشارو, /* ### TODO implement getStarters for all Unicode encodings?! */
    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,
    ucnv_get_Non_Surrogate_Unicode_سيٽ,

    خالي_اشارو,
    خالي_اشارو
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
#جيڪڏهن_پورو
    خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_معلومات UTF32_معلومات = 
        UCNV_IMMUTABLE_SHARED_معلومات_INITIALIZER(&UTF32_Static_معلومات, &_UTF32Impl);

#جيڪڏهن_پورو
