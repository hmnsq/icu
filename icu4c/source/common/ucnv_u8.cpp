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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

/* Prototypes --------------------------------------------------------------- */

/* Keep these here to make finicky compilers happy */

U_CFUNC ਖਾਲੀ ucnv_fromUnicode_UTF8(UConverter_From_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2,
                                           UError_ਕੋਡ *err);
U_CFUNC ਖਾਲੀ ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverter_From_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2,
                                                        UError_ਕੋਡ *err);


/* UTF-8 -------------------------------------------------------------------- */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵੱਧ_ਤੋਂ_ਵੱਧ_UCS2            0x0000FFFF

ਸਥਿਰ ਅਚਲ uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

ਸਥਿਰ UBool has_CESU8_ਅੰਕੜੇ(ਅਚਲ UConverter *cnv)
{
#ਜੇ UCONFIG_ONLY_HTML_CONVERSION
    ਵਾਪਸ ਝੂਠ;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ cnv->shared_ਅੰਕੜੇ == &CESU8_ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ
}
U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ  U_CALLCONV ucnv_ਮੰਜ਼ਿਲ_Unicode_UTF8 (UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ * ਦਲੀਲਾਂ_2,
                                  UError_ਕੋਡ * err)
{
    UConverter *cnv = ਦਲੀਲਾਂ_2->converter;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *my_ਸਰੋਤ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    char16_t *my_ਨਿਸ਼ਾਨਾ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸਰੋਤ_ਹੱਦਬੰਦੀ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਮੰਜ਼ਿਲ_UBytes = cnv->ਮੰਜ਼ਿਲ_UBytes;
    UBool isCESU8 = has_CESU8_ਅੰਕੜੇ(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, ਵਿੱਚ_ਬਾਈਟਾਂ;

    /* Restore size of current sequence */
    ਜੇ (cnv->ਮੰਜ਼ਿਲ_ULength > 0 && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
    {
        ਵਿੱਚ_ਬਾਈਟਾਂ = cnv->ਬਹੁਲਕ;            /* restore # of bytes to consume */
        i = cnv->ਮੰਜ਼ਿਲ_ULength;             /* restore # of bytes consumed */
        cnv->ਮੰਜ਼ਿਲ_ULength = 0;

        ch = cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ;/*Stores the previously calculated ch from a previous call*/
        cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0;
        ਜਾਓ morebytes;
    }


    ਜਦੋਂ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
    {
        ch = *(my_ਸਰੋਤ++);
        ਜੇ (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(my_ਨਿਸ਼ਾਨਾ++) = (char16_t) ch;
        }
        ਨਹੀਂਤਾਂ
        {
            /* store the first char */
            ਮੰਜ਼ਿਲ_UBytes[0] = (ਅੱਖਰ)ch;
            ਵਿੱਚ_ਬਾਈਟਾਂ = U8_ਗਿਣਤੀ_ਬਾਈਟਾਂ_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            ਜਦੋਂ (i < ਵਿੱਚ_ਬਾਈਟਾਂ)
            {
                ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ)
                {
                    ਮੰਜ਼ਿਲ_UBytes[i] = (ਅੱਖਰ) (ch2 = *my_ਸਰੋਤ);
                    ਜੇ (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, ਵਿੱਚ_ਬਾਈਟਾਂ) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        ਰੋਕੋ; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++my_ਸਰੋਤ;
                    i++;
                }
                ਨਹੀਂਤਾਂ
                {
                    /* stores a partially calculated target*/
                    cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = ch;
                    cnv->ਬਹੁਲਕ = ਵਿੱਚ_ਬਾਈਟਾਂ;
                    cnv->ਮੰਜ਼ਿਲ_ULength = (int8_t) i;
                    ਜਾਓ donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            ਜੇ (i == ਵਿੱਚ_ਬਾਈਟਾਂ && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[ਵਿੱਚ_ਬਾਈਟਾਂ];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                ਜੇ (ch <= ਵੱਧ_ਤੋਂ_ਵੱਧ_UCS2) 
                {
                    /* fits in 16 bits */
                    *(my_ਨਿਸ਼ਾਨਾ++) = (char16_t) ch;
                }
                ਨਹੀਂਤਾਂ
                {
                    /* write out the surrogates */
                    *(my_ਨਿਸ਼ਾਨਾ++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    ਜੇ (my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
                    {
                        *(my_ਨਿਸ਼ਾਨਾ++) = (char16_t)ch;
                    }
                    ਨਹੀਂਤਾਂ
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UChar_Error_ਬਫਰ[0] = (char16_t) ch;
                        cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ = 1;
                        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                        ਰੋਕੋ;
                    }
                }
            }
            ਨਹੀਂਤਾਂ
            {
                cnv->ਮੰਜ਼ਿਲ_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਰੋਕੋ;
            }
        }
    }

donefornow:
    ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ >= ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = (ਅਚਲ ਅੱਖਰ *) my_ਸਰੋਤ;
}

ਸਥਿਰ ਖਾਲੀ  U_CALLCONV ucnv_ਮੰਜ਼ਿਲ_Unicode_UTF8_OFFSETS_LOGIC (UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ * ਦਲੀਲਾਂ_2,
                                                UError_ਕੋਡ * err)
{
    UConverter *cnv = ਦਲੀਲਾਂ_2->converter;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *my_ਸਰੋਤ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    char16_t *my_ਨਿਸ਼ਾਨਾ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    int32_t *myOffsets = ਦਲੀਲਾਂ_2->offsets;
    int32_t ਵਿੱਥ_ਅੰਕ = 0;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸਰੋਤ_ਹੱਦਬੰਦੀ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਮੰਜ਼ਿਲ_UBytes = cnv->ਮੰਜ਼ਿਲ_UBytes;
    UBool isCESU8 = has_CESU8_ਅੰਕੜੇ(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, ਵਿੱਚ_ਬਾਈਟਾਂ;

    /* Restore size of current sequence */
    ਜੇ (cnv->ਮੰਜ਼ਿਲ_ULength > 0 && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
    {
        ਵਿੱਚ_ਬਾਈਟਾਂ = cnv->ਬਹੁਲਕ;            /* restore # of bytes to consume */
        i = cnv->ਮੰਜ਼ਿਲ_ULength;             /* restore # of bytes consumed */
        cnv->ਮੰਜ਼ਿਲ_ULength = 0;

        ch = cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ;/*Stores the previously calculated ch from a previous call*/
        cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0;
        ਜਾਓ morebytes;
    }

    ਜਦੋਂ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
    {
        ch = *(my_ਸਰੋਤ++);
        ਜੇ (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(my_ਨਿਸ਼ਾਨਾ++) = (char16_t) ch;
            *(myOffsets++) = ਵਿੱਥ_ਅੰਕ++;
        }
        ਨਹੀਂਤਾਂ
        {
            ਮੰਜ਼ਿਲ_UBytes[0] = (ਅੱਖਰ)ch;
            ਵਿੱਚ_ਬਾਈਟਾਂ = U8_ਗਿਣਤੀ_ਬਾਈਟਾਂ_NON_ASCII(ch);
            i = 1;

morebytes:
            ਜਦੋਂ (i < ਵਿੱਚ_ਬਾਈਟਾਂ)
            {
                ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ)
                {
                    ਮੰਜ਼ਿਲ_UBytes[i] = (ਅੱਖਰ) (ch2 = *my_ਸਰੋਤ);
                    ਜੇ (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, ਵਿੱਚ_ਬਾਈਟਾਂ) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        ਰੋਕੋ; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++my_ਸਰੋਤ;
                    i++;
                }
                ਨਹੀਂਤਾਂ
                {
                    cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = ch;
                    cnv->ਬਹੁਲਕ = ਵਿੱਚ_ਬਾਈਟਾਂ;
                    cnv->ਮੰਜ਼ਿਲ_ULength = (int8_t)i;
                    ਜਾਓ donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            ਜੇ (i == ਵਿੱਚ_ਬਾਈਟਾਂ && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[ਵਿੱਚ_ਬਾਈਟਾਂ];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                ਜੇ (ch <= ਵੱਧ_ਤੋਂ_ਵੱਧ_UCS2) 
                {
                    /* fits in 16 bits */
                    *(my_ਨਿਸ਼ਾਨਾ++) = (char16_t) ch;
                    *(myOffsets++) = ਵਿੱਥ_ਅੰਕ;
                }
                ਨਹੀਂਤਾਂ
                {
                    /* write out the surrogates */
                    *(my_ਨਿਸ਼ਾਨਾ++) = U16_LEAD(ch);
                    *(myOffsets++) = ਵਿੱਥ_ਅੰਕ;
                    ch = U16_TRAIL(ch);
                    ਜੇ (my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
                    {
                        *(my_ਨਿਸ਼ਾਨਾ++) = (char16_t)ch;
                        *(myOffsets++) = ਵਿੱਥ_ਅੰਕ;
                    }
                    ਨਹੀਂਤਾਂ
                    {
                        cnv->UChar_Error_ਬਫਰ[0] = (char16_t) ch;
                        cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ = 1;
                        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    }
                }
                ਵਿੱਥ_ਅੰਕ += i;
            }
            ਨਹੀਂਤਾਂ
            {
                cnv->ਮੰਜ਼ਿਲ_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਰੋਕੋ;
            }
        }
    }

donefornow:
    ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ >= ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = (ਅਚਲ ਅੱਖਰ *) my_ਸਰੋਤ;
    ਦਲੀਲਾਂ_2->offsets = myOffsets;
}
U_CDECL_ਸਿਰਾ

U_CFUNC ਖਾਲੀ  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverter_From_Unicode_ਦਲੀਲਾਂ * ਦਲੀਲਾਂ_2,
                                    UError_ਕੋਡ * err)
{
    UConverter *cnv = ਦਲੀਲਾਂ_2->converter;
    ਅਚਲ char16_t *my_ਸਰੋਤ = ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    ਅਚਲ char16_t *ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *my_ਨਿਸ਼ਾਨਾ = (uint8_t *) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    ਅਚਲ uint8_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = (uint8_t *) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    uint8_t *ਆਰਜ਼ੀ_Ptr;
    UChar32 ch;
    uint8_t ਆਰਜ਼ੀ_Buf[4];
    int32_t ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write;
    UBool isNotCESU8 = !has_CESU8_ਅੰਕੜੇ(cnv);

    ਜੇ (cnv->fromUChar32 && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        ਜਾਓ lowsurrogate;
    }

    ਜਦੋਂ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
    {
        ch = *(my_ਸਰੋਤ++);

        ਜੇ (ch < 0x80)        /* Single byte */
        {
            *(my_ਨਿਸ਼ਾਨਾ++) = (uint8_t) ch;
        }
        ਨਹੀਂਤਾਂ ਜੇ (ch < 0x800)  /* Double byte */
        {
            *(my_ਨਿਸ਼ਾਨਾ++) = (uint8_t) ((ch >> 6) | 0xc0);
            ਜੇ (my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
            {
                *(my_ਨਿਸ਼ਾਨਾ++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            ਨਹੀਂਤਾਂ
            {
                cnv->char_Error_ਬਫਰ[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ = 1;
                *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
            }
        }
        ਨਹੀਂਤਾਂ {
            /* Check for surrogates */
            ਜੇ(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                    /* test both code units */
                    ਜੇ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*my_ਸਰੋਤ)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *my_ਸਰੋਤ);
                        ++my_ਸਰੋਤ;
                        /* exit this condition tree */
                    }
                    ਨਹੀਂਤਾਂ {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਰੋਕੋ;
                    }
                }
                ਨਹੀਂਤਾਂ {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    ਰੋਕੋ;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            ਆਰਜ਼ੀ_Ptr = (((ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - my_ਨਿਸ਼ਾਨਾ) >= 4) ? my_ਨਿਸ਼ਾਨਾ : ਆਰਜ਼ੀ_Buf);

            ਜੇ (ch <= ਵੱਧ_ਤੋਂ_ਵੱਧ_UCS2) {
                ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write = 2;
                ਆਰਜ਼ੀ_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            ਨਹੀਂਤਾਂ {
                ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write = 3;
                ਆਰਜ਼ੀ_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                ਆਰਜ਼ੀ_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            ਆਰਜ਼ੀ_Ptr[ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            ਆਰਜ਼ੀ_Ptr[ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            ਜੇ (ਆਰਜ਼ੀ_Ptr == my_ਨਿਸ਼ਾਨਾ) {
                /* There was enough space to write the codepoint directly. */
                my_ਨਿਸ਼ਾਨਾ += (ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write + 1);
            }
            ਨਹੀਂਤਾਂ {
                /* We might run out of room soon. Write it slowly. */
                ਲਈ (; ਆਰਜ਼ੀ_Ptr <= (ਆਰਜ਼ੀ_Buf + ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write); ਆਰਜ਼ੀ_Ptr++) {
                    ਜੇ (my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                        *(my_ਨਿਸ਼ਾਨਾ++) = *ਆਰਜ਼ੀ_Ptr;
                    }
                    ਨਹੀਂਤਾਂ {
                        cnv->char_Error_ਬਫਰ[cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ++] = *ਆਰਜ਼ੀ_Ptr;
                        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    }
                }
            }
        }
    }

    ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ >= ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ && U_SUCCESS(*err))
    {
        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = (ਅੱਖਰ *) my_ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
}

U_CFUNC ਖਾਲੀ  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverter_From_Unicode_ਦਲੀਲਾਂ * ਦਲੀਲਾਂ_2,
                                                  UError_ਕੋਡ * err)
{
    UConverter *cnv = ਦਲੀਲਾਂ_2->converter;
    ਅਚਲ char16_t *my_ਸਰੋਤ = ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    int32_t *myOffsets = ਦਲੀਲਾਂ_2->offsets;
    ਅਚਲ char16_t *ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *my_ਨਿਸ਼ਾਨਾ = (uint8_t *) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    ਅਚਲ uint8_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = (uint8_t *) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    uint8_t *ਆਰਜ਼ੀ_Ptr;
    UChar32 ch;
    int32_t ਵਿੱਥ_ਅੰਕ, ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
    int32_t ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write;
    uint8_t ਆਰਜ਼ੀ_Buf[4];
    UBool isNotCESU8 = !has_CESU8_ਅੰਕੜੇ(cnv);

    ਜੇ (cnv->fromUChar32 && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        ਵਿੱਥ_ਅੰਕ = -1;
        ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ = 0;
        ਜਾਓ lowsurrogate;
    } ਨਹੀਂਤਾਂ {
        ਵਿੱਥ_ਅੰਕ = 0;
    }

    ਜਦੋਂ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
    {
        ch = *(my_ਸਰੋਤ++);

        ਜੇ (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = ਵਿੱਥ_ਅੰਕ++;
            *(my_ਨਿਸ਼ਾਨਾ++) = (ਅੱਖਰ) ch;
        }
        ਨਹੀਂਤਾਂ ਜੇ (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = ਵਿੱਥ_ਅੰਕ;
            *(my_ਨਿਸ਼ਾਨਾ++) = (uint8_t) ((ch >> 6) | 0xc0);
            ਜੇ (my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
            {
                *(myOffsets++) = ਵਿੱਥ_ਅੰਕ++;
                *(my_ਨਿਸ਼ਾਨਾ++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            ਨਹੀਂਤਾਂ
            {
                cnv->char_Error_ਬਫਰ[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ = 1;
                *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
            }
        }
        ਨਹੀਂਤਾਂ
        /* Check for surrogates */
        {
            ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ = ਵਿੱਥ_ਅੰਕ + 1;

            ਜੇ(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                    /* test both code units */
                    ਜੇ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*my_ਸਰੋਤ)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *my_ਸਰੋਤ);
                        ++my_ਸਰੋਤ;
                        ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        /* exit this condition tree */
                    }
                    ਨਹੀਂਤਾਂ {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਰੋਕੋ;
                    }
                }
                ਨਹੀਂਤਾਂ {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    ਰੋਕੋ;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            ਆਰਜ਼ੀ_Ptr = (((ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - my_ਨਿਸ਼ਾਨਾ) >= 4) ? my_ਨਿਸ਼ਾਨਾ : ਆਰਜ਼ੀ_Buf);

            ਜੇ (ch <= ਵੱਧ_ਤੋਂ_ਵੱਧ_UCS2) {
                ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write = 2;
                ਆਰਜ਼ੀ_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            ਨਹੀਂਤਾਂ {
                ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write = 3;
                ਆਰਜ਼ੀ_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                ਆਰਜ਼ੀ_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            ਆਰਜ਼ੀ_Ptr[ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            ਆਰਜ਼ੀ_Ptr[ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            ਜੇ (ਆਰਜ਼ੀ_Ptr == my_ਨਿਸ਼ਾਨਾ) {
                /* There was enough space to write the codepoint directly. */
                my_ਨਿਸ਼ਾਨਾ += (ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write + 1);
                myOffsets[0] = ਵਿੱਥ_ਅੰਕ;
                myOffsets[1] = ਵਿੱਥ_ਅੰਕ;
                myOffsets[2] = ਵਿੱਥ_ਅੰਕ;
                ਜੇ (ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write >= 3) {
                    myOffsets[3] = ਵਿੱਥ_ਅੰਕ;
                }
                myOffsets += (ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write + 1);
            }
            ਨਹੀਂਤਾਂ {
                /* We might run out of room soon. Write it slowly. */
                ਲਈ (; ਆਰਜ਼ੀ_Ptr <= (ਆਰਜ਼ੀ_Buf + ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write); ਆਰਜ਼ੀ_Ptr++) {
                    ਜੇ (my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
                    {
                        *(myOffsets++) = ਵਿੱਥ_ਅੰਕ;
                        *(my_ਨਿਸ਼ਾਨਾ++) = *ਆਰਜ਼ੀ_Ptr;
                    }
                    ਨਹੀਂਤਾਂ
                    {
                        cnv->char_Error_ਬਫਰ[cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ++] = *ਆਰਜ਼ੀ_Ptr;
                        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    }
                }
            }
            ਵਿੱਥ_ਅੰਕ = ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
        }
    }

    ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ >= ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ && U_SUCCESS(*err))
    {
        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = (ਅੱਖਰ *) my_ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
    ਦਲੀਲਾਂ_2->offsets = myOffsets;
}

U_CDECL_BEGIN
ਸਥਿਰ UChar32 U_CALLCONV ucnv_get_ਅਗਲਾ_UChar_UTF8(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2,
                                               UError_ਕੋਡ *err) {
    UConverter *cnv;
    ਅਚਲ uint8_t *ਸਰੋਤ_Initial;
    ਅਚਲ uint8_t *ਸਰੋਤ;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = ਦਲੀਲਾਂ_2->converter;
    ਸਰੋਤ_Initial = ਸਰੋਤ = (ਅਚਲ uint8_t *)ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    ਜੇ (ਸਰੋਤ >= (ਅਚਲ uint8_t *)ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ)
    {
        /* no input */
        *err = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0xffff;
    }

    myByte = *(ਸਰੋਤ++);
    ਜੇ (U8_IS_SINGLE(myByte))
    {
        ਦਲੀਲਾਂ_2->ਸਰੋਤ = (ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
        ਵਾਪਸ (UChar32)myByte;
    }

    uint16_t ਗਿਣਤੀ_Trail_ਬਾਈਟਾਂ = U8_ਗਿਣਤੀ_TRAIL_ਬਾਈਟਾਂ(myByte);
    ਜੇ (ਗਿਣਤੀ_Trail_ਬਾਈਟਾਂ == 0) {
        cnv->ਮੰਜ਼ਿਲ_UBytes[0] = myByte;
        cnv->ਮੰਜ਼ਿਲ_ULength = 1;
        *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
        ਦਲੀਲਾਂ_2->ਸਰੋਤ = (ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
        ਵਾਪਸ 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    ਜੇ (((ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ + ਗਿਣਤੀ_Trail_ਬਾਈਟਾਂ) > ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extra_ਬਾਈਟਾਂ_ਮੰਜ਼ਿਲ_Write = ਗਿਣਤੀ_Trail_ਬਾਈਟਾਂ + 1;
        cnv->ਮੰਜ਼ਿਲ_UBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_ਮਿਲਿਆ;
        ਜਦੋਂ(ਸਰੋਤ < (ਅਚਲ uint8_t *)ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            uint8_t b = *ਸਰੋਤ;
            ਜੇ(icu::UTF8::isValidTrail(myByte, b, i, extra_ਬਾਈਟਾਂ_ਮੰਜ਼ਿਲ_Write)) {
                cnv->ਮੰਜ਼ਿਲ_UBytes[i++] = b;
                ++ਸਰੋਤ;
            } ਨਹੀਂਤਾਂ {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਰੋਕੋ;
            }
        }
        cnv->ਮੰਜ਼ਿਲ_ULength = i;
        ਦਲੀਲਾਂ_2->ਸਰੋਤ = (ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
        ਵਾਪਸ 0xffff;
    }

    ch = myByte << 6;
    ਜੇ(ਗਿਣਤੀ_Trail_ਬਾਈਟਾਂ == 2) {
        uint8_t t1 = *ਸਰੋਤ, t2;
        ਜੇ(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++ਸਰੋਤ)) {
            ਦਲੀਲਾਂ_2->ਸਰੋਤ = (ਅਚਲ ਅੱਖਰ *)(ਸਰੋਤ + 1);
            ਵਾਪਸ (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } ਨਹੀਂਤਾਂ ਜੇ(ਗਿਣਤੀ_Trail_ਬਾਈਟਾਂ == 1) {
        uint8_t t1 = *ਸਰੋਤ;
        ਜੇ(U8_IS_TRAIL(t1)) {
            ਦਲੀਲਾਂ_2->ਸਰੋਤ = (ਅਚਲ ਅੱਖਰ *)(ਸਰੋਤ + 1);
            ਵਾਪਸ (ch + t1) - offsetsFromUTF8[2];
        }
    } ਨਹੀਂਤਾਂ {  // countTrailBytes == 3
        uint8_t t1 = *ਸਰੋਤ, t2, t3;
        ਜੇ(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++ਸਰੋਤ) &&
                U8_IS_TRAIL(t3 = *++ਸਰੋਤ)) {
            ਦਲੀਲਾਂ_2->ਸਰੋਤ = (ਅਚਲ ਅੱਖਰ *)(ਸਰੋਤ + 1);
            ਵਾਪਸ (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = (ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;

    ਲਈ(i = 0; ਸਰੋਤ_Initial < ਸਰੋਤ; ++i) {
        cnv->ਮੰਜ਼ਿਲ_UBytes[i] = *ਸਰੋਤ_Initial++;
    }
    cnv->ਮੰਜ਼ਿਲ_ULength = i;
    *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
    ਵਾਪਸ 0xffff;
} 
U_CDECL_ਸਿਰਾ

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_UTF8FromUTF8(UConverter_From_Unicode_ਦਲੀਲਾਂ *pFromUArgs,
                  UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਮੰਜ਼ਿਲ_UArgs,
                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *utf8;
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
    int32_t count;

    int8_t old_ਮੰਜ਼ਿਲ_ULength, ਮੰਜ਼ਿਲ_ULength, ਮੰਜ਼ਿਲ_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    /* set up the local pointers */
    utf8=p_ਮੰਜ਼ਿਲ_UArgs->converter;
    ਸਰੋਤ=(uint8_t *)p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=(uint8_t *)p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ=(uint8_t *)pFromUArgs->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(int32_t)(pFromUArgs->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-pFromUArgs->ਨਿਸ਼ਾਨਾ);

    /* get the converter state from the UTF-8 UConverter */
    ਜੇ(utf8->ਮੰਜ਼ਿਲ_ULength > 0) {
        ਮੰਜ਼ਿਲ_ULength=old_ਮੰਜ਼ਿਲ_ULength=utf8->ਮੰਜ਼ਿਲ_ULength;
        ਮੰਜ਼ਿਲ_ULimit=(int8_t)utf8->ਬਹੁਲਕ;
        c=(UChar32)utf8->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ;
    } ਨਹੀਂਤਾਂ {
        ਮੰਜ਼ਿਲ_ULength=old_ਮੰਜ਼ਿਲ_ULength=ਮੰਜ਼ਿਲ_ULimit=0;
        c = 0;
    }

    count=(int32_t)(ਸਰੋਤ_ਹੱਦਬੰਦੀ-ਸਰੋਤ)+old_ਮੰਜ਼ਿਲ_ULength;
    ਜੇ(count<ਮੰਜ਼ਿਲ_ULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } ਨਹੀਂਤਾਂ ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ<ਮੰਜ਼ਿਲ_ULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *p_Error_ਕੋਡ=U_USING_DEFAULT_WARNING;
        ਵਾਪਸ;
    } ਨਹੀਂਤਾਂ {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        ਜੇ(count>ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
            count=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
        }

        // The conversion loop checks count>0 only once per character.
        // If the buffer ends with a truncated sequence,
        // then we reduce the count to stop before that,
        // and collect the remaining bytes after the conversion loop.

        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length=count-ਮੰਜ਼ਿਲ_ULength;
        U8_TRUNCATE_IF_INCOMPLETE(ਸਰੋਤ, 0, length);
        count=ਮੰਜ਼ਿਲ_ULength+length;
    }

    ਜੇ(c!=0) {
        utf8->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=0;
        utf8->ਮੰਜ਼ਿਲ_ULength=0;
        ਜਾਓ ਹੋਰ_ਬਾਈਟਾਂ;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    ਜਦੋਂ(count>0) {
        b=*ਸਰੋਤ++;
        ਜੇ(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *ਨਿਸ਼ਾਨਾ++=b;
            --count;
            ਜਾਰੀ;
        } ਨਹੀਂਤਾਂ {
            ਜੇ(b>=0xe0) {
                ਜੇ( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=ਸਰੋਤ[0]) &&
                    U8_IS_TRAIL(t2=ਸਰੋਤ[1])
                ) {
                    ਸਰੋਤ+=2;
                    *ਨਿਸ਼ਾਨਾ++=b;
                    *ਨਿਸ਼ਾਨਾ++=t1;
                    *ਨਿਸ਼ਾਨਾ++=t2;
                    count-=3;
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ {
                ਜੇ( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*ਸਰੋਤ)
                ) {
                    ++ਸਰੋਤ;
                    *ਨਿਸ਼ਾਨਾ++=b;
                    *ਨਿਸ਼ਾਨਾ++=t1;
                    count-=2;
                    ਜਾਰੀ;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            old_ਮੰਜ਼ਿਲ_ULength=0;
            ਮੰਜ਼ਿਲ_ULength=1;
            ਮੰਜ਼ਿਲ_ULimit=U8_ਗਿਣਤੀ_ਬਾਈਟਾਂ_NON_ASCII(b);
            c=b;
ਹੋਰ_ਬਾਈਟਾਂ:
            ਜਦੋਂ(ਮੰਜ਼ਿਲ_ULength<ਮੰਜ਼ਿਲ_ULimit) {
                ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                    b=*ਸਰੋਤ;
                    ਜੇ(icu::UTF8::isValidTrail(c, b, ਮੰਜ਼ਿਲ_ULength, ਮੰਜ਼ਿਲ_ULimit)) {
                        ++ਸਰੋਤ;
                        ++ਮੰਜ਼ਿਲ_ULength;
                        c=(c<<6)+b;
                    } ਨਹੀਂਤਾਂ {
                        ਰੋਕੋ; /* sequence too short, stop with toULength<toULimit */
                    }
                } ਨਹੀਂਤਾਂ {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    ਸਰੋਤ-=(ਮੰਜ਼ਿਲ_ULength-old_ਮੰਜ਼ਿਲ_ULength);
                    ਜਦੋਂ(old_ਮੰਜ਼ਿਲ_ULength<ਮੰਜ਼ਿਲ_ULength) {
                        utf8->ਮੰਜ਼ਿਲ_UBytes[old_ਮੰਜ਼ਿਲ_ULength++]=*ਸਰੋਤ++;
                    }
                    utf8->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=c;
                    utf8->ਮੰਜ਼ਿਲ_ULength=ਮੰਜ਼ਿਲ_ULength;
                    utf8->ਬਹੁਲਕ=ਮੰਜ਼ਿਲ_ULimit;
                    p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ=(ਅੱਖਰ *)ਸਰੋਤ;
                    pFromUArgs->ਨਿਸ਼ਾਨਾ=(ਅੱਖਰ *)ਨਿਸ਼ਾਨਾ;
                    ਵਾਪਸ;
                }
            }

            ਜੇ(ਮੰਜ਼ਿਲ_ULength!=ਮੰਜ਼ਿਲ_ULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                ਸਰੋਤ-=(ਮੰਜ਼ਿਲ_ULength-old_ਮੰਜ਼ਿਲ_ULength);
                ਜਦੋਂ(old_ਮੰਜ਼ਿਲ_ULength<ਮੰਜ਼ਿਲ_ULength) {
                    utf8->ਮੰਜ਼ਿਲ_UBytes[old_ਮੰਜ਼ਿਲ_ULength++]=*ਸਰੋਤ++;
                }
                utf8->ਮੰਜ਼ਿਲ_ULength=ਮੰਜ਼ਿਲ_ULength;
                p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ=(ਅੱਖਰ *)ਸਰੋਤ;
                pFromUArgs->ਨਿਸ਼ਾਨਾ=(ਅੱਖਰ *)ਨਿਸ਼ਾਨਾ;
                *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਵਾਪਸ;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                ਲਈ(i=0; i<old_ਮੰਜ਼ਿਲ_ULength; ++i) {
                    *ਨਿਸ਼ਾਨਾ++=utf8->ਮੰਜ਼ਿਲ_UBytes[i];
                }
                ਸਰੋਤ-=(ਮੰਜ਼ਿਲ_ULength-old_ਮੰਜ਼ਿਲ_ULength);
                ਲਈ(; i<ਮੰਜ਼ਿਲ_ULength; ++i) {
                    *ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
                }
                count-=ਮੰਜ਼ਿਲ_ULength;
            }
        }
    }
    U_ASSERT(count>=0);

    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ) && ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        ਜੇ(ਨਿਸ਼ਾਨਾ==(ਅਚਲ uint8_t *)pFromUArgs->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
        } ਨਹੀਂਤਾਂ {
            b=*ਸਰੋਤ;
            ਮੰਜ਼ਿਲ_ULimit=U8_ਗਿਣਤੀ_ਬਾਈਟਾਂ(b);
            ਜੇ(ਮੰਜ਼ਿਲ_ULimit>(ਸਰੋਤ_ਹੱਦਬੰਦੀ-ਸਰੋਤ)) {
                /* collect a truncated byte sequence */
                ਮੰਜ਼ਿਲ_ULength=0;
                c=b;
                ਲਈ(;;) {
                    utf8->ਮੰਜ਼ਿਲ_UBytes[ਮੰਜ਼ਿਲ_ULength++]=b;
                    ਜੇ(++ਸਰੋਤ==ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                        /* partial byte sequence at end of source */
                        utf8->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=c;
                        utf8->ਮੰਜ਼ਿਲ_ULength=ਮੰਜ਼ਿਲ_ULength;
                        utf8->ਬਹੁਲਕ=ਮੰਜ਼ਿਲ_ULimit;
                        ਰੋਕੋ;
                    } ਨਹੀਂਤਾਂ ਜੇ(!icu::UTF8::isValidTrail(c, b=*ਸਰੋਤ, ਮੰਜ਼ਿਲ_ULength, ਮੰਜ਼ਿਲ_ULimit)) {
                        utf8->ਮੰਜ਼ਿਲ_ULength=ਮੰਜ਼ਿਲ_ULength;
                        *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਰੋਕੋ;
                    }
                    c=(c<<6)+b;
                }
            } ਨਹੀਂਤਾਂ {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *p_Error_ਕੋਡ=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ=(ਅੱਖਰ *)ਸਰੋਤ;
    pFromUArgs->ਨਿਸ਼ਾਨਾ=(ਅੱਖਰ *)ਨਿਸ਼ਾਨਾ;
}

U_CDECL_ਸਿਰਾ

/* UTF-8 converter data ----------------------------------------------------- */

ਸਥਿਰ ਅਚਲ UConverterImpl _UTF8Impl={
    UCNV_UTF8,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    ucnv_ਮੰਜ਼ਿਲ_Unicode_UTF8,
    ucnv_ਮੰਜ਼ਿਲ_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    ucnv_get_ਅਗਲਾ_UChar_UTF8,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_get_Non_Surrogate_Unicode_ਸੈੱਟ,

    ucnv_UTF8FromUTF8,
    ucnv_UTF8FromUTF8
};

/* The 1208 CCSID refers to any version of Unicode of UTF-8 */
ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ UTF8_Static_ਅੰਕੜੇ={
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,ਝੂਠ,ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ਅਚਲ UConverter_Shared_ਅੰਕੜੇ UTF8_ਅੰਕੜੇ=
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&UTF8_Static_ਅੰਕੜੇ, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

ਸਥਿਰ ਅਚਲ UConverterImpl _CESU8Impl={
    UCNV_CESU8,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    ucnv_ਮੰਜ਼ਿਲ_Unicode_UTF8,
    ucnv_ਮੰਜ਼ਿਲ_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_get_Complete_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};

ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ CESU8_Static_ਅੰਕੜੇ={
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,ਝੂਠ,ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ਅਚਲ UConverter_Shared_ਅੰਕੜੇ CESU8_ਅੰਕੜੇ=
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&CESU8_Static_ਅੰਕੜੇ, &_CESU8Impl);

#ਜੇ_ਅੰਤ
