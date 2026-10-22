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

U_CFUNC فراغ ucnv_fromUnicode_UTF8(UConverter_From_Unicode_آرگومانها *آرگومانها_2,
                                           UError_کد *err);
U_CFUNC فراغ ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverter_From_Unicode_آرگومانها *آرگومانها_2,
                                                        UError_کد *err);


/* UTF-8 -------------------------------------------------------------------- */

#تعریف_کن بیشینه_UCS2            0x0000FFFF

ساكن ثابت uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

ساكن UBool has_CESU8_داده(ثابت UConverter *cnv)
{
#إذا UCONFIG_ONLY_HTML_CONVERSION
    أرجع خطأ;
#وإلا
    أرجع cnv->shared_داده == &CESU8_داده;
#پایان_اگر
}
U_CDECL_BEGIN
ساكن فراغ  U_CALLCONV ucnv_مقصد_Unicode_UTF8 (UConverter_مقصد_Unicode_آرگومانها * آرگومانها_2,
                                  UError_کد * err)
{
    UConverter *cnv = آرگومانها_2->converter;
    ثابت بیعلامت حرف *my_منبع = (بیعلامت حرف *) آرگومانها_2->منبع;
    char16_t *my_هدف = آرگومانها_2->هدف;
    ثابت بیعلامت حرف *منبع_حد = (بیعلامت حرف *) آرگومانها_2->منبع_حد;
    ثابت char16_t *هدف_حد = آرگومانها_2->هدف_حد;
    بیعلامت حرف *مقصد_UBytes = cnv->مقصد_UBytes;
    UBool isCESU8 = has_CESU8_داده(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, درون_بایتها;

    /* Restore size of current sequence */
    إذا (cnv->مقصد_ULength > 0 && my_هدف < هدف_حد)
    {
        درون_بایتها = cnv->نما;            /* restore # of bytes to consume */
        i = cnv->مقصد_ULength;             /* restore # of bytes consumed */
        cnv->مقصد_ULength = 0;

        ch = cnv->مقصد_Unicode_وضعیت;/*Stores the previously calculated ch from a previous call*/
        cnv->مقصد_Unicode_وضعیت = 0;
        اذهب morebytes;
    }


    طالما (my_منبع < منبع_حد && my_هدف < هدف_حد)
    {
        ch = *(my_منبع++);
        إذا (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(my_هدف++) = (char16_t) ch;
        }
        وإلا
        {
            /* store the first char */
            مقصد_UBytes[0] = (حرف)ch;
            درون_بایتها = U8_شمار_بایتها_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            طالما (i < درون_بایتها)
            {
                إذا (my_منبع < منبع_حد)
                {
                    مقصد_UBytes[i] = (حرف) (ch2 = *my_منبع);
                    إذا (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, درون_بایتها) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        اكسر; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++my_منبع;
                    i++;
                }
                وإلا
                {
                    /* stores a partially calculated target*/
                    cnv->مقصد_Unicode_وضعیت = ch;
                    cnv->نما = درون_بایتها;
                    cnv->مقصد_ULength = (int8_t) i;
                    اذهب donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            إذا (i == درون_بایتها && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[درون_بایتها];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                إذا (ch <= بیشینه_UCS2) 
                {
                    /* fits in 16 bits */
                    *(my_هدف++) = (char16_t) ch;
                }
                وإلا
                {
                    /* write out the surrogates */
                    *(my_هدف++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    إذا (my_هدف < هدف_حد)
                    {
                        *(my_هدف++) = (char16_t)ch;
                    }
                    وإلا
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UChar_Error_میانگیر[0] = (char16_t) ch;
                        cnv->UChar_Error_میانگیر_درازی = 1;
                        *err = U_میانگیر_سرریز_ERROR;
                        اكسر;
                    }
                }
            }
            وإلا
            {
                cnv->مقصد_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_یافت;
                اكسر;
            }
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

ساكن فراغ  U_CALLCONV ucnv_مقصد_Unicode_UTF8_OFFSETS_LOGIC (UConverter_مقصد_Unicode_آرگومانها * آرگومانها_2,
                                                UError_کد * err)
{
    UConverter *cnv = آرگومانها_2->converter;
    ثابت بیعلامت حرف *my_منبع = (بیعلامت حرف *) آرگومانها_2->منبع;
    char16_t *my_هدف = آرگومانها_2->هدف;
    int32_t *myOffsets = آرگومانها_2->offsets;
    int32_t جابه_جایی_رقم = 0;
    ثابت بیعلامت حرف *منبع_حد = (بیعلامت حرف *) آرگومانها_2->منبع_حد;
    ثابت char16_t *هدف_حد = آرگومانها_2->هدف_حد;
    بیعلامت حرف *مقصد_UBytes = cnv->مقصد_UBytes;
    UBool isCESU8 = has_CESU8_داده(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, درون_بایتها;

    /* Restore size of current sequence */
    إذا (cnv->مقصد_ULength > 0 && my_هدف < هدف_حد)
    {
        درون_بایتها = cnv->نما;            /* restore # of bytes to consume */
        i = cnv->مقصد_ULength;             /* restore # of bytes consumed */
        cnv->مقصد_ULength = 0;

        ch = cnv->مقصد_Unicode_وضعیت;/*Stores the previously calculated ch from a previous call*/
        cnv->مقصد_Unicode_وضعیت = 0;
        اذهب morebytes;
    }

    طالما (my_منبع < منبع_حد && my_هدف < هدف_حد)
    {
        ch = *(my_منبع++);
        إذا (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(my_هدف++) = (char16_t) ch;
            *(myOffsets++) = جابه_جایی_رقم++;
        }
        وإلا
        {
            مقصد_UBytes[0] = (حرف)ch;
            درون_بایتها = U8_شمار_بایتها_NON_ASCII(ch);
            i = 1;

morebytes:
            طالما (i < درون_بایتها)
            {
                إذا (my_منبع < منبع_حد)
                {
                    مقصد_UBytes[i] = (حرف) (ch2 = *my_منبع);
                    إذا (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, درون_بایتها) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        اكسر; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++my_منبع;
                    i++;
                }
                وإلا
                {
                    cnv->مقصد_Unicode_وضعیت = ch;
                    cnv->نما = درون_بایتها;
                    cnv->مقصد_ULength = (int8_t)i;
                    اذهب donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            إذا (i == درون_بایتها && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[درون_بایتها];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                إذا (ch <= بیشینه_UCS2) 
                {
                    /* fits in 16 bits */
                    *(my_هدف++) = (char16_t) ch;
                    *(myOffsets++) = جابه_جایی_رقم;
                }
                وإلا
                {
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
                        cnv->UChar_Error_میانگیر[0] = (char16_t) ch;
                        cnv->UChar_Error_میانگیر_درازی = 1;
                        *err = U_میانگیر_سرریز_ERROR;
                    }
                }
                جابه_جایی_رقم += i;
            }
            وإلا
            {
                cnv->مقصد_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_یافت;
                اكسر;
            }
        }
    }

donefornow:
    إذا (my_منبع < منبع_حد && my_هدف >= هدف_حد && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_میانگیر_سرریز_ERROR;
    }

    آرگومانها_2->هدف = my_هدف;
    آرگومانها_2->منبع = (ثابت حرف *) my_منبع;
    آرگومانها_2->offsets = myOffsets;
}
U_CDECL_انتها

U_CFUNC فراغ  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverter_From_Unicode_آرگومانها * آرگومانها_2,
                                    UError_کد * err)
{
    UConverter *cnv = آرگومانها_2->converter;
    ثابت char16_t *my_منبع = آرگومانها_2->منبع;
    ثابت char16_t *منبع_حد = آرگومانها_2->منبع_حد;
    uint8_t *my_هدف = (uint8_t *) آرگومانها_2->هدف;
    ثابت uint8_t *هدف_حد = (uint8_t *) آرگومانها_2->هدف_حد;
    uint8_t *موقت_Ptr;
    UChar32 ch;
    uint8_t موقت_Buf[4];
    int32_t شاخص_مقصد_Write;
    UBool isNotCESU8 = !has_CESU8_داده(cnv);

    إذا (cnv->fromUChar32 && my_هدف < هدف_حد)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        اذهب lowsurrogate;
    }

    طالما (my_منبع < منبع_حد && my_هدف < هدف_حد)
    {
        ch = *(my_منبع++);

        إذا (ch < 0x80)        /* Single byte */
        {
            *(my_هدف++) = (uint8_t) ch;
        }
        وإلا إذا (ch < 0x800)  /* Double byte */
        {
            *(my_هدف++) = (uint8_t) ((ch >> 6) | 0xc0);
            إذا (my_هدف < هدف_حد)
            {
                *(my_هدف++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            وإلا
            {
                cnv->char_Error_میانگیر[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_میانگیر_درازی = 1;
                *err = U_میانگیر_سرریز_ERROR;
            }
        }
        وإلا {
            /* Check for surrogates */
            إذا(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                إذا (my_منبع < منبع_حد) {
                    /* test both code units */
                    إذا(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*my_منبع)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *my_منبع);
                        ++my_منبع;
                        /* exit this condition tree */
                    }
                    وإلا {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_یافت;
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
            موقت_Ptr = (((هدف_حد - my_هدف) >= 4) ? my_هدف : موقت_Buf);

            إذا (ch <= بیشینه_UCS2) {
                شاخص_مقصد_Write = 2;
                موقت_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            وإلا {
                شاخص_مقصد_Write = 3;
                موقت_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                موقت_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            موقت_Ptr[شاخص_مقصد_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            موقت_Ptr[شاخص_مقصد_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            إذا (موقت_Ptr == my_هدف) {
                /* There was enough space to write the codepoint directly. */
                my_هدف += (شاخص_مقصد_Write + 1);
            }
            وإلا {
                /* We might run out of room soon. Write it slowly. */
                لكل (; موقت_Ptr <= (موقت_Buf + شاخص_مقصد_Write); موقت_Ptr++) {
                    إذا (my_هدف < هدف_حد) {
                        *(my_هدف++) = *موقت_Ptr;
                    }
                    وإلا {
                        cnv->char_Error_میانگیر[cnv->char_Error_میانگیر_درازی++] = *موقت_Ptr;
                        *err = U_میانگیر_سرریز_ERROR;
                    }
                }
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

U_CFUNC فراغ  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverter_From_Unicode_آرگومانها * آرگومانها_2,
                                                  UError_کد * err)
{
    UConverter *cnv = آرگومانها_2->converter;
    ثابت char16_t *my_منبع = آرگومانها_2->منبع;
    int32_t *myOffsets = آرگومانها_2->offsets;
    ثابت char16_t *منبع_حد = آرگومانها_2->منبع_حد;
    uint8_t *my_هدف = (uint8_t *) آرگومانها_2->هدف;
    ثابت uint8_t *هدف_حد = (uint8_t *) آرگومانها_2->هدف_حد;
    uint8_t *موقت_Ptr;
    UChar32 ch;
    int32_t جابه_جایی_رقم, بعدی_منبع_شاخص;
    int32_t شاخص_مقصد_Write;
    uint8_t موقت_Buf[4];
    UBool isNotCESU8 = !has_CESU8_داده(cnv);

    إذا (cnv->fromUChar32 && my_هدف < هدف_حد)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        جابه_جایی_رقم = -1;
        بعدی_منبع_شاخص = 0;
        اذهب lowsurrogate;
    } وإلا {
        جابه_جایی_رقم = 0;
    }

    طالما (my_منبع < منبع_حد && my_هدف < هدف_حد)
    {
        ch = *(my_منبع++);

        إذا (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = جابه_جایی_رقم++;
            *(my_هدف++) = (حرف) ch;
        }
        وإلا إذا (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = جابه_جایی_رقم;
            *(my_هدف++) = (uint8_t) ((ch >> 6) | 0xc0);
            إذا (my_هدف < هدف_حد)
            {
                *(myOffsets++) = جابه_جایی_رقم++;
                *(my_هدف++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            وإلا
            {
                cnv->char_Error_میانگیر[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_میانگیر_درازی = 1;
                *err = U_میانگیر_سرریز_ERROR;
            }
        }
        وإلا
        /* Check for surrogates */
        {
            بعدی_منبع_شاخص = جابه_جایی_رقم + 1;

            إذا(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                إذا (my_منبع < منبع_حد) {
                    /* test both code units */
                    إذا(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*my_منبع)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *my_منبع);
                        ++my_منبع;
                        ++بعدی_منبع_شاخص;
                        /* exit this condition tree */
                    }
                    وإلا {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_یافت;
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
            موقت_Ptr = (((هدف_حد - my_هدف) >= 4) ? my_هدف : موقت_Buf);

            إذا (ch <= بیشینه_UCS2) {
                شاخص_مقصد_Write = 2;
                موقت_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            وإلا {
                شاخص_مقصد_Write = 3;
                موقت_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                موقت_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            موقت_Ptr[شاخص_مقصد_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            موقت_Ptr[شاخص_مقصد_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            إذا (موقت_Ptr == my_هدف) {
                /* There was enough space to write the codepoint directly. */
                my_هدف += (شاخص_مقصد_Write + 1);
                myOffsets[0] = جابه_جایی_رقم;
                myOffsets[1] = جابه_جایی_رقم;
                myOffsets[2] = جابه_جایی_رقم;
                إذا (شاخص_مقصد_Write >= 3) {
                    myOffsets[3] = جابه_جایی_رقم;
                }
                myOffsets += (شاخص_مقصد_Write + 1);
            }
            وإلا {
                /* We might run out of room soon. Write it slowly. */
                لكل (; موقت_Ptr <= (موقت_Buf + شاخص_مقصد_Write); موقت_Ptr++) {
                    إذا (my_هدف < هدف_حد)
                    {
                        *(myOffsets++) = جابه_جایی_رقم;
                        *(my_هدف++) = *موقت_Ptr;
                    }
                    وإلا
                    {
                        cnv->char_Error_میانگیر[cnv->char_Error_میانگیر_درازی++] = *موقت_Ptr;
                        *err = U_میانگیر_سرریز_ERROR;
                    }
                }
            }
            جابه_جایی_رقم = بعدی_منبع_شاخص;
        }
    }

    إذا (my_منبع < منبع_حد && my_هدف >= هدف_حد && U_SUCCESS(*err))
    {
        *err = U_میانگیر_سرریز_ERROR;
    }

    آرگومانها_2->هدف = (حرف *) my_هدف;
    آرگومانها_2->منبع = my_منبع;
    آرگومانها_2->offsets = myOffsets;
}

U_CDECL_BEGIN
ساكن UChar32 U_CALLCONV ucnv_get_بعدی_UChar_UTF8(UConverter_مقصد_Unicode_آرگومانها *آرگومانها_2,
                                               UError_کد *err) {
    UConverter *cnv;
    ثابت uint8_t *منبع_Initial;
    ثابت uint8_t *منبع;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = آرگومانها_2->converter;
    منبع_Initial = منبع = (ثابت uint8_t *)آرگومانها_2->منبع;
    إذا (منبع >= (ثابت uint8_t *)آرگومانها_2->منبع_حد)
    {
        /* no input */
        *err = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    myByte = *(منبع++);
    إذا (U8_IS_SINGLE(myByte))
    {
        آرگومانها_2->منبع = (ثابت حرف *)منبع;
        أرجع (UChar32)myByte;
    }

    uint16_t شمار_Trail_بایتها = U8_شمار_TRAIL_بایتها(myByte);
    إذا (شمار_Trail_بایتها == 0) {
        cnv->مقصد_UBytes[0] = myByte;
        cnv->مقصد_ULength = 1;
        *err = U_ILLEGAL_CHAR_یافت;
        آرگومانها_2->منبع = (ثابت حرف *)منبع;
        أرجع 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    إذا (((ثابت حرف *)منبع + شمار_Trail_بایتها) > آرگومانها_2->منبع_حد)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extra_بایتها_مقصد_Write = شمار_Trail_بایتها + 1;
        cnv->مقصد_UBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_یافت;
        طالما(منبع < (ثابت uint8_t *)آرگومانها_2->منبع_حد) {
            uint8_t b = *منبع;
            إذا(icu::UTF8::isValidTrail(myByte, b, i, extra_بایتها_مقصد_Write)) {
                cnv->مقصد_UBytes[i++] = b;
                ++منبع;
            } وإلا {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_یافت;
                اكسر;
            }
        }
        cnv->مقصد_ULength = i;
        آرگومانها_2->منبع = (ثابت حرف *)منبع;
        أرجع 0xffff;
    }

    ch = myByte << 6;
    إذا(شمار_Trail_بایتها == 2) {
        uint8_t t1 = *منبع, t2;
        إذا(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++منبع)) {
            آرگومانها_2->منبع = (ثابت حرف *)(منبع + 1);
            أرجع (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } وإلا إذا(شمار_Trail_بایتها == 1) {
        uint8_t t1 = *منبع;
        إذا(U8_IS_TRAIL(t1)) {
            آرگومانها_2->منبع = (ثابت حرف *)(منبع + 1);
            أرجع (ch + t1) - offsetsFromUTF8[2];
        }
    } وإلا {  // countTrailBytes == 3
        uint8_t t1 = *منبع, t2, t3;
        إذا(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++منبع) &&
                U8_IS_TRAIL(t3 = *++منبع)) {
            آرگومانها_2->منبع = (ثابت حرف *)(منبع + 1);
            أرجع (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    آرگومانها_2->منبع = (ثابت حرف *)منبع;

    لكل(i = 0; منبع_Initial < منبع; ++i) {
        cnv->مقصد_UBytes[i] = *منبع_Initial++;
    }
    cnv->مقصد_ULength = i;
    *err = U_ILLEGAL_CHAR_یافت;
    أرجع 0xffff;
} 
U_CDECL_انتها

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
ساكن فراغ U_CALLCONV
ucnv_UTF8FromUTF8(UConverter_From_Unicode_آرگومانها *pFromUArgs,
                  UConverter_مقصد_Unicode_آرگومانها *p_مقصد_UArgs,
                  UError_کد *p_Error_کد) {
    UConverter *utf8;
    ثابت uint8_t *منبع, *منبع_حد;
    uint8_t *هدف;
    int32_t هدف_ظرفیت;
    int32_t count;

    int8_t old_مقصد_ULength, مقصد_ULength, مقصد_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    /* set up the local pointers */
    utf8=p_مقصد_UArgs->converter;
    منبع=(uint8_t *)p_مقصد_UArgs->منبع;
    منبع_حد=(uint8_t *)p_مقصد_UArgs->منبع_حد;
    هدف=(uint8_t *)pFromUArgs->هدف;
    هدف_ظرفیت=(int32_t)(pFromUArgs->هدف_حد-pFromUArgs->هدف);

    /* get the converter state from the UTF-8 UConverter */
    إذا(utf8->مقصد_ULength > 0) {
        مقصد_ULength=old_مقصد_ULength=utf8->مقصد_ULength;
        مقصد_ULimit=(int8_t)utf8->نما;
        c=(UChar32)utf8->مقصد_Unicode_وضعیت;
    } وإلا {
        مقصد_ULength=old_مقصد_ULength=مقصد_ULimit=0;
        c = 0;
    }

    count=(int32_t)(منبع_حد-منبع)+old_مقصد_ULength;
    إذا(count<مقصد_ULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } وإلا إذا(هدف_ظرفیت<مقصد_ULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *p_Error_کد=U_USING_DEFAULT_WARNING;
        أرجع;
    } وإلا {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        إذا(count>هدف_ظرفیت) {
            count=هدف_ظرفیت;
        }

        // The conversion loop checks count>0 only once per character.
        // If the buffer ends with a truncated sequence,
        // then we reduce the count to stop before that,
        // and collect the remaining bytes after the conversion loop.

        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length=count-مقصد_ULength;
        U8_TRUNCATE_IF_INCOMPLETE(منبع, 0, length);
        count=مقصد_ULength+length;
    }

    إذا(c!=0) {
        utf8->مقصد_Unicode_وضعیت=0;
        utf8->مقصد_ULength=0;
        اذهب بیشتر_بایتها;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    طالما(count>0) {
        b=*منبع++;
        إذا(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *هدف++=b;
            --count;
            استمر;
        } وإلا {
            إذا(b>=0xe0) {
                إذا( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=منبع[0]) &&
                    U8_IS_TRAIL(t2=منبع[1])
                ) {
                    منبع+=2;
                    *هدف++=b;
                    *هدف++=t1;
                    *هدف++=t2;
                    count-=3;
                    استمر;
                }
            } وإلا {
                إذا( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*منبع)
                ) {
                    ++منبع;
                    *هدف++=b;
                    *هدف++=t1;
                    count-=2;
                    استمر;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            old_مقصد_ULength=0;
            مقصد_ULength=1;
            مقصد_ULimit=U8_شمار_بایتها_NON_ASCII(b);
            c=b;
بیشتر_بایتها:
            طالما(مقصد_ULength<مقصد_ULimit) {
                إذا(منبع<منبع_حد) {
                    b=*منبع;
                    إذا(icu::UTF8::isValidTrail(c, b, مقصد_ULength, مقصد_ULimit)) {
                        ++منبع;
                        ++مقصد_ULength;
                        c=(c<<6)+b;
                    } وإلا {
                        اكسر; /* sequence too short, stop with toULength<toULimit */
                    }
                } وإلا {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    منبع-=(مقصد_ULength-old_مقصد_ULength);
                    طالما(old_مقصد_ULength<مقصد_ULength) {
                        utf8->مقصد_UBytes[old_مقصد_ULength++]=*منبع++;
                    }
                    utf8->مقصد_Unicode_وضعیت=c;
                    utf8->مقصد_ULength=مقصد_ULength;
                    utf8->نما=مقصد_ULimit;
                    p_مقصد_UArgs->منبع=(حرف *)منبع;
                    pFromUArgs->هدف=(حرف *)هدف;
                    أرجع;
                }
            }

            إذا(مقصد_ULength!=مقصد_ULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                منبع-=(مقصد_ULength-old_مقصد_ULength);
                طالما(old_مقصد_ULength<مقصد_ULength) {
                    utf8->مقصد_UBytes[old_مقصد_ULength++]=*منبع++;
                }
                utf8->مقصد_ULength=مقصد_ULength;
                p_مقصد_UArgs->منبع=(حرف *)منبع;
                pFromUArgs->هدف=(حرف *)هدف;
                *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                أرجع;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                لكل(i=0; i<old_مقصد_ULength; ++i) {
                    *هدف++=utf8->مقصد_UBytes[i];
                }
                منبع-=(مقصد_ULength-old_مقصد_ULength);
                لكل(; i<مقصد_ULength; ++i) {
                    *هدف++=*منبع++;
                }
                count-=مقصد_ULength;
            }
        }
    }
    U_ASSERT(count>=0);

    إذا(U_SUCCESS(*p_Error_کد) && منبع<منبع_حد) {
        إذا(هدف==(ثابت uint8_t *)pFromUArgs->هدف_حد) {
            *p_Error_کد=U_میانگیر_سرریز_ERROR;
        } وإلا {
            b=*منبع;
            مقصد_ULimit=U8_شمار_بایتها(b);
            إذا(مقصد_ULimit>(منبع_حد-منبع)) {
                /* collect a truncated byte sequence */
                مقصد_ULength=0;
                c=b;
                لكل(;;) {
                    utf8->مقصد_UBytes[مقصد_ULength++]=b;
                    إذا(++منبع==منبع_حد) {
                        /* partial byte sequence at end of source */
                        utf8->مقصد_Unicode_وضعیت=c;
                        utf8->مقصد_ULength=مقصد_ULength;
                        utf8->نما=مقصد_ULimit;
                        اكسر;
                    } وإلا إذا(!icu::UTF8::isValidTrail(c, b=*منبع, مقصد_ULength, مقصد_ULimit)) {
                        utf8->مقصد_ULength=مقصد_ULength;
                        *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                        اكسر;
                    }
                    c=(c<<6)+b;
                }
            } وإلا {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *p_Error_کد=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    p_مقصد_UArgs->منبع=(حرف *)منبع;
    pFromUArgs->هدف=(حرف *)هدف;
}

U_CDECL_انتها

/* UTF-8 converter data ----------------------------------------------------- */

ساكن ثابت UConverterImpl _UTF8Impl={
    UCNV_UTF8,

    اشاره_تهی,
    اشاره_تهی,

    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,

    ucnv_مقصد_Unicode_UTF8,
    ucnv_مقصد_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    ucnv_get_بعدی_UChar_UTF8,

    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
    ucnv_get_Non_Surrogate_Unicode_مجموعه,

    ucnv_UTF8FromUTF8,
    ucnv_UTF8FromUTF8
};

/* The 1208 CCSID refers to any version of Unicode of UTF-8 */
ساكن ثابت UConverter_Static_داده UTF8_Static_داده={
    حجم(UConverter_Static_داده),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,خطأ,خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_داده UTF8_داده=
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&UTF8_Static_داده, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

ساكن ثابت UConverterImpl _CESU8Impl={
    UCNV_CESU8,

    اشاره_تهی,
    اشاره_تهی,

    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,

    ucnv_مقصد_Unicode_UTF8,
    ucnv_مقصد_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    اشاره_تهی,

    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
    ucnv_get_Complete_Unicode_مجموعه,

    اشاره_تهی,
    اشاره_تهی
};

ساكن ثابت UConverter_Static_داده CESU8_Static_داده={
    حجم(UConverter_Static_داده),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,خطأ,خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_داده CESU8_داده=
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&CESU8_Static_داده, &_CESU8Impl);

#پایان_اگر
