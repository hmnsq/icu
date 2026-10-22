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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵੱਧ_ਤੋਂ_ਵੱਧ_UCS2            0x0000FFFF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵੱਧ_ਤੋਂ_ਵੱਧ_UTF             0x0010FFFF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HALF_ਖਿਸਕਾਓ              10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HALF_ਅਧਾਰ               0x0010000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HALF_ਨਕਾਬ               0x3FF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SURROGATE_ਉੱਚਾ_ਸ਼ੁਰੂ    0xD800
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SURROGATE_ਨੀਵਾਂ_ਸ਼ੁਰੂ     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SURROGATE_ਨੀਵਾਂ_ਅਧਾਰ      9216

ਗਿਣਤੀ {
    UCNV_NEED_ਮੰਜ਼ਿਲ_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
T_UConverter_ਮੰਜ਼ਿਲ_Unicode_UTF32_BE(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ * ਦਲੀਲਾਂ_2,
                                UError_ਕੋਡ * err)
{
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *my_ਸਰੋਤ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    char16_t *my_ਨਿਸ਼ਾਨਾ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸਰੋਤ_ਹੱਦਬੰਦੀ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਮੰਜ਼ਿਲ_UBytes = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    ਜੇ (ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength > 0 && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        i = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength;       /* restore # of bytes consumed */
        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 0;

        ch = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ - 1;/*Stores the previously calculated ch from a previous call*/
        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0;
        ਜਾਓ morebytes;
    }

    ਜਦੋਂ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        i = 0;
        ch = 0;
morebytes:
        ਜਦੋਂ (i < ਆਕਾਰ(uint32_t)) {
            ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                ch = (ch << 8) | (uint8_t)(*my_ਸਰੋਤ);
                ਮੰਜ਼ਿਲ_UBytes[i++] = (ਅੱਖਰ) *(my_ਸਰੋਤ++);
            }
            ਨਹੀਂਤਾਂ {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = ch + 1;
                ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = (int8_t) i;
                ਜਾਓ donefornow;
            }
        }

        ਜੇ (ch <= ਵੱਧ_ਤੋਂ_ਵੱਧ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ਜੇ (ch <= ਵੱਧ_ਤੋਂ_ਵੱਧ_UCS2) 
            {
                /* fits in 16 bits */
                *(my_ਨਿਸ਼ਾਨਾ++) = (char16_t) ch;
            }
            ਨਹੀਂਤਾਂ {
                /* write out the surrogates */
                *(my_ਨਿਸ਼ਾਨਾ++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                ਜੇ (my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                    *(my_ਨਿਸ਼ਾਨਾ++) = (char16_t)ch;
                }
                ਨਹੀਂਤਾਂ {
                    /* Put in overflow buffer (not handled here) */
                    ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ[0] = (char16_t) ch;
                    ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ = 1;
                    *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    ਰੋਕੋ;
                }
            }
        }
        ਨਹੀਂਤਾਂ {
            ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
            ਰੋਕੋ;
        }
    }

donefornow:
    ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ >= ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = (ਅਚਲ ਅੱਖਰ *) my_ਸਰੋਤ;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
T_UConverter_ਮੰਜ਼ਿਲ_Unicode_UTF32_BE_ਵਿੱਥ_LOGIC(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ * ਦਲੀਲਾਂ_2,
                                             UError_ਕੋਡ * err)
{
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *my_ਸਰੋਤ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    char16_t *my_ਨਿਸ਼ਾਨਾ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    int32_t *myOffsets = ਦਲੀਲਾਂ_2->offsets;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸਰੋਤ_ਹੱਦਬੰਦੀ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਮੰਜ਼ਿਲ_UBytes = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes;
    uint32_t ch, i;
    int32_t ਵਿੱਥ_ਅੰਕ = 0;

    /* Restore state of current sequence */
    ਜੇ (ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength > 0 && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        i = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength;       /* restore # of bytes consumed */
        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 0;

        ch = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ - 1;/*Stores the previously calculated ch from a previous call*/
        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0;
        ਜਾਓ morebytes;
    }

    ਜਦੋਂ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        i = 0;
        ch = 0;
morebytes:
        ਜਦੋਂ (i < ਆਕਾਰ(uint32_t)) {
            ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                ch = (ch << 8) | (uint8_t)(*my_ਸਰੋਤ);
                ਮੰਜ਼ਿਲ_UBytes[i++] = (ਅੱਖਰ) *(my_ਸਰੋਤ++);
            }
            ਨਹੀਂਤਾਂ {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = ch + 1;
                ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = (int8_t) i;
                ਜਾਓ donefornow;
            }
        }

        ਜੇ (ch <= ਵੱਧ_ਤੋਂ_ਵੱਧ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ਜੇ (ch <= ਵੱਧ_ਤੋਂ_ਵੱਧ_UCS2) {
                /* fits in 16 bits */
                *(my_ਨਿਸ਼ਾਨਾ++) = (char16_t) ch;
                *(myOffsets++) = ਵਿੱਥ_ਅੰਕ;
            }
            ਨਹੀਂਤਾਂ {
                /* write out the surrogates */
                *(my_ਨਿਸ਼ਾਨਾ++) = U16_LEAD(ch);
                *myOffsets++ = ਵਿੱਥ_ਅੰਕ;
                ch = U16_TRAIL(ch);
                ਜੇ (my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
                {
                    *(my_ਨਿਸ਼ਾਨਾ++) = (char16_t)ch;
                    *(myOffsets++) = ਵਿੱਥ_ਅੰਕ;
                }
                ਨਹੀਂਤਾਂ {
                    /* Put in overflow buffer (not handled here) */
                    ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ[0] = (char16_t) ch;
                    ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ = 1;
                    *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    ਰੋਕੋ;
                }
            }
        }
        ਨਹੀਂਤਾਂ {
            ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
            ਰੋਕੋ;
        }
        ਵਿੱਥ_ਅੰਕ += i;
    }

donefornow:
    ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ >= ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = (ਅਚਲ ਅੱਖਰ *) my_ਸਰੋਤ;
    ਦਲੀਲਾਂ_2->offsets = myOffsets;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverter_From_Unicode_ਦਲੀਲਾਂ * ਦਲੀਲਾਂ_2,
                                  UError_ਕੋਡ * err)
{
    ਅਚਲ char16_t *my_ਸਰੋਤ = ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *my_ਨਿਸ਼ਾਨਾ;
    ਅਚਲ char16_t *ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    UChar32 ch, ch2;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਆਰਜ਼ੀ[ਆਕਾਰ(uint32_t)];

    ਜੇ(my_ਸਰੋਤ >= ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /* no input, nothing to do */
        ਵਾਪਸ;
    }

    /* write the BOM if necessary */
    ਜੇ(ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ==UCNV_NEED_ਮੰਜ਼ਿਲ_WRITE_BOM) {
        ਸਥਿਰ ਅਚਲ ਅੱਖਰ bom[]={ 0, 0, (ਅੱਖਰ)0xfeu, (ਅੱਖਰ)0xffu };
        ucnv_from_UWrite_ਬਾਈਟਾਂ(ਦਲੀਲਾਂ_2->converter,
                             bom, 4,
                             &ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ, ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                             &ਦਲੀਲਾਂ_2->offsets, -1,
                             err);
        ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ=0;
    }

    my_ਨਿਸ਼ਾਨਾ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    ਆਰਜ਼ੀ[0] = 0;

    ਜੇ (ਦਲੀਲਾਂ_2->converter->fromUChar32) {
        ch = ਦਲੀਲਾਂ_2->converter->fromUChar32;
        ਦਲੀਲਾਂ_2->converter->fromUChar32 = 0;
        ਜਾਓ lowsurogate;
    }

    ਜਦੋਂ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        ch = *(my_ਸਰੋਤ++);

        ਜੇ (U_IS_SURROGATE(ch)) {
            ਜੇ (U_IS_LEAD(ch)) {
lowsurogate:
                ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                    ch2 = *my_ਸਰੋਤ;
                    ਜੇ (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ਉੱਚਾ_ਸ਼ੁਰੂ) << HALF_ਖਿਸਕਾਓ) + ch2 + SURROGATE_ਨੀਵਾਂ_ਅਧਾਰ;
                        my_ਸਰੋਤ++;
                    }
                    ਨਹੀਂਤਾਂ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        ਦਲੀਲਾਂ_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਰੋਕੋ;
                    }
                }
                ਨਹੀਂਤਾਂ {
                    /* ran out of source */
                    ਦਲੀਲਾਂ_2->converter->fromUChar32 = ch;
                    ਜੇ (ਦਲੀਲਾਂ_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    }
                    ਰੋਕੋ;
                }
            }
            ਨਹੀਂਤਾਂ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                ਦਲੀਲਾਂ_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਰੋਕੋ;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        ਆਰਜ਼ੀ[1] = (uint8_t) (ch >> 16 & 0x1F);
        ਆਰਜ਼ੀ[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        ਆਰਜ਼ੀ[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        ਲਈ (ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write = 0; ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write <= ਆਕਾਰ(uint32_t) - 1; ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write++) {
            ਜੇ (my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                *(my_ਨਿਸ਼ਾਨਾ++) = ਆਰਜ਼ੀ[ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write];
            }
            ਨਹੀਂਤਾਂ {
                ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] = ਆਰਜ਼ੀ[ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write];
                *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
            }
        }
    }

    ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ >= ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ && U_SUCCESS(*err)) {
        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = (ਅੱਖਰ *) my_ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
T_UConverter_from_Unicode_UTF32_BE_ਵਿੱਥ_LOGIC(UConverter_From_Unicode_ਦਲੀਲਾਂ * ਦਲੀਲਾਂ_2,
                                               UError_ਕੋਡ * err)
{
    ਅਚਲ char16_t *my_ਸਰੋਤ = ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *my_ਨਿਸ਼ਾਨਾ;
    int32_t *myOffsets;
    ਅਚਲ char16_t *ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    UChar32 ch, ch2;
    int32_t ਵਿੱਥ_ਅੰਕ = 0;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਆਰਜ਼ੀ[ਆਕਾਰ(uint32_t)];

    ਜੇ(my_ਸਰੋਤ >= ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /* no input, nothing to do */
        ਵਾਪਸ;
    }

    /* write the BOM if necessary */
    ਜੇ(ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ==UCNV_NEED_ਮੰਜ਼ਿਲ_WRITE_BOM) {
        ਸਥਿਰ ਅਚਲ ਅੱਖਰ bom[]={ 0, 0, (ਅੱਖਰ)0xfeu, (ਅੱਖਰ)0xffu };
        ucnv_from_UWrite_ਬਾਈਟਾਂ(ਦਲੀਲਾਂ_2->converter,
                             bom, 4,
                             &ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ, ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                             &ਦਲੀਲਾਂ_2->offsets, -1,
                             err);
        ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ=0;
    }

    my_ਨਿਸ਼ਾਨਾ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    myOffsets = ਦਲੀਲਾਂ_2->offsets;
    ਆਰਜ਼ੀ[0] = 0;

    ਜੇ (ਦਲੀਲਾਂ_2->converter->fromUChar32) {
        ch = ਦਲੀਲਾਂ_2->converter->fromUChar32;
        ਦਲੀਲਾਂ_2->converter->fromUChar32 = 0;
        ਜਾਓ lowsurogate;
    }

    ਜਦੋਂ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        ch = *(my_ਸਰੋਤ++);

        ਜੇ (U_IS_SURROGATE(ch)) {
            ਜੇ (U_IS_LEAD(ch)) {
lowsurogate:
                ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                    ch2 = *my_ਸਰੋਤ;
                    ਜੇ (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ਉੱਚਾ_ਸ਼ੁਰੂ) << HALF_ਖਿਸਕਾਓ) + ch2 + SURROGATE_ਨੀਵਾਂ_ਅਧਾਰ;
                        my_ਸਰੋਤ++;
                    }
                    ਨਹੀਂਤਾਂ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        ਦਲੀਲਾਂ_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਰੋਕੋ;
                    }
                }
                ਨਹੀਂਤਾਂ {
                    /* ran out of source */
                    ਦਲੀਲਾਂ_2->converter->fromUChar32 = ch;
                    ਜੇ (ਦਲੀਲਾਂ_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    }
                    ਰੋਕੋ;
                }
            }
            ਨਹੀਂਤਾਂ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                ਦਲੀਲਾਂ_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਰੋਕੋ;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        ਆਰਜ਼ੀ[1] = (uint8_t) (ch >> 16 & 0x1F);
        ਆਰਜ਼ੀ[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        ਆਰਜ਼ੀ[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        ਲਈ (ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write = 0; ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write <= ਆਕਾਰ(uint32_t) - 1; ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write++) {
            ਜੇ (my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                *(my_ਨਿਸ਼ਾਨਾ++) = ਆਰਜ਼ੀ[ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write];
                *(myOffsets++) = ਵਿੱਥ_ਅੰਕ;
            }
            ਨਹੀਂਤਾਂ {
                ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] = ਆਰਜ਼ੀ[ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write];
                *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
            }
        }
        ਵਿੱਥ_ਅੰਕ = ਵਿੱਥ_ਅੰਕ + 1 + (ਆਰਜ਼ੀ[1] != 0);
    }

    ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ >= ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ && U_SUCCESS(*err)) {
        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = (ਅੱਖਰ *) my_ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
    ਦਲੀਲਾਂ_2->offsets = myOffsets;
}

ਸਥਿਰ UChar32 U_CALLCONV
T_UConverter_get_ਅਗਲਾ_UChar_UTF32_BE(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ* ਦਲੀਲਾਂ_2,
                                   UError_ਕੋਡ* err)
{
    ਅਚਲ uint8_t *my_ਸਰੋਤ;
    UChar32 myUChar;
    int32_t length;

    my_ਸਰੋਤ = (ਅਚਲ uint8_t *)ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    ਜੇ (my_ਸਰੋਤ >= (ਅਚਲ uint8_t *)ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ)
    {
        /* no input */
        *err = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0xffff;
    }

    length = (int32_t)((ਅਚਲ uint8_t *)ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ - my_ਸਰੋਤ);
    ਜੇ (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes, my_ਸਰੋਤ, length);
        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = (int8_t)length;
        ਦਲੀਲਾਂ_2->ਸਰੋਤ = (ਅਚਲ ਅੱਖਰ *)(my_ਸਰੋਤ + length);
        *err = U_TRUNCATED_CHAR_ਮਿਲਿਆ;
        ਵਾਪਸ 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)my_ਸਰੋਤ[0] << 24)
            | ((UChar32)my_ਸਰੋਤ[1] << 16)
            | ((UChar32)my_ਸਰੋਤ[2] << 8)
            | ((UChar32)my_ਸਰੋਤ[3]);

    ਦਲੀਲਾਂ_2->ਸਰੋਤ = (ਅਚਲ ਅੱਖਰ *)(my_ਸਰੋਤ + 4);
    ਜੇ ((uint32_t)myUChar <= ਵੱਧ_ਤੋਂ_ਵੱਧ_UTF && !U_IS_SURROGATE(myUChar)) {
        ਵਾਪਸ myUChar;
    }

    uprv_memcpy(ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes, my_ਸਰੋਤ, 4);
    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 4;

    *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
    ਵਾਪਸ 0xffff;
}
U_CDECL_ਸਿਰਾ
ਸਥਿਰ ਅਚਲ UConverterImpl _UTF32BEImpl = {
    UCNV_UTF32_BigEndian,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    T_UConverter_ਮੰਜ਼ਿਲ_Unicode_UTF32_BE,
    T_UConverter_ਮੰਜ਼ਿਲ_Unicode_UTF32_BE_ਵਿੱਥ_LOGIC,
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_ਵਿੱਥ_LOGIC,
    T_UConverter_get_ਅਗਲਾ_UChar_UTF32_BE,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_get_Non_Surrogate_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ UTF32_BEStatic_ਅੰਕੜੇ = {
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, ਝੂਠ, ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ਅਚਲ UConverter_Shared_ਅੰਕੜੇ _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&UTF32_BEStatic_ਅੰਕੜੇ, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
T_UConverter_ਮੰਜ਼ਿਲ_Unicode_UTF32_LE(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ * ਦਲੀਲਾਂ_2,
                                UError_ਕੋਡ * err)
{
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *my_ਸਰੋਤ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    char16_t *my_ਨਿਸ਼ਾਨਾ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸਰੋਤ_ਹੱਦਬੰਦੀ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਮੰਜ਼ਿਲ_UBytes = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    ਜੇ (ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength > 0 && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
    {
        i = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength;       /* restore # of bytes consumed */
        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ - 1;
        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0;
        ਜਾਓ morebytes;
    }

    ਜਦੋਂ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
    {
        i = 0;
        ch = 0;
morebytes:
        ਜਦੋਂ (i < ਆਕਾਰ(uint32_t))
        {
            ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ)
            {
                ch |= ((uint8_t)(*my_ਸਰੋਤ)) << (i * 8);
                ਮੰਜ਼ਿਲ_UBytes[i++] = (ਅੱਖਰ) *(my_ਸਰੋਤ++);
            }
            ਨਹੀਂਤਾਂ
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = ch + 1;
                ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = (int8_t) i;
                ਜਾਓ donefornow;
            }
        }

        ਜੇ (ch <= ਵੱਧ_ਤੋਂ_ਵੱਧ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ਜੇ (ch <= ਵੱਧ_ਤੋਂ_ਵੱਧ_UCS2) {
                /* fits in 16 bits */
                *(my_ਨਿਸ਼ਾਨਾ++) = (char16_t) ch;
            }
            ਨਹੀਂਤਾਂ {
                /* write out the surrogates */
                *(my_ਨਿਸ਼ਾਨਾ++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                ਜੇ (my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                    *(my_ਨਿਸ਼ਾਨਾ++) = (char16_t)ch;
                }
                ਨਹੀਂਤਾਂ {
                    /* Put in overflow buffer (not handled here) */
                    ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ[0] = (char16_t) ch;
                    ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ = 1;
                    *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    ਰੋਕੋ;
                }
            }
        }
        ਨਹੀਂਤਾਂ {
            ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
            ਰੋਕੋ;
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

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
T_UConverter_ਮੰਜ਼ਿਲ_Unicode_UTF32_LE_ਵਿੱਥ_LOGIC(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ * ਦਲੀਲਾਂ_2,
                                             UError_ਕੋਡ * err)
{
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *my_ਸਰੋਤ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    char16_t *my_ਨਿਸ਼ਾਨਾ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    int32_t *myOffsets = ਦਲੀਲਾਂ_2->offsets;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸਰੋਤ_ਹੱਦਬੰਦੀ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਮੰਜ਼ਿਲ_UBytes = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes;
    uint32_t ch, i;
    int32_t ਵਿੱਥ_ਅੰਕ = 0;

    /* Restore state of current sequence */
    ਜੇ (ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength > 0 && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
    {
        i = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength;       /* restore # of bytes consumed */
        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ - 1;
        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0;
        ਜਾਓ morebytes;
    }

    ਜਦੋਂ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
    {
        i = 0;
        ch = 0;
morebytes:
        ਜਦੋਂ (i < ਆਕਾਰ(uint32_t))
        {
            ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ)
            {
                ch |= ((uint8_t)(*my_ਸਰੋਤ)) << (i * 8);
                ਮੰਜ਼ਿਲ_UBytes[i++] = (ਅੱਖਰ) *(my_ਸਰੋਤ++);
            }
            ਨਹੀਂਤਾਂ
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = ch + 1;
                ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = (int8_t) i;
                ਜਾਓ donefornow;
            }
        }

        ਜੇ (ch <= ਵੱਧ_ਤੋਂ_ਵੱਧ_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ਜੇ (ch <= ਵੱਧ_ਤੋਂ_ਵੱਧ_UCS2) 
            {
                /* fits in 16 bits */
                *(my_ਨਿਸ਼ਾਨਾ++) = (char16_t) ch;
                *(myOffsets++) = ਵਿੱਥ_ਅੰਕ;
            }
            ਨਹੀਂਤਾਂ {
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
                    /* Put in overflow buffer (not handled here) */
                    ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ[0] = (char16_t) ch;
                    ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ = 1;
                    *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    ਰੋਕੋ;
                }
            }
        }
        ਨਹੀਂਤਾਂ
        {
            ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
            ਰੋਕੋ;
        }
        ਵਿੱਥ_ਅੰਕ += i;
    }

donefornow:
    ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ >= ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = (ਅਚਲ ਅੱਖਰ *) my_ਸਰੋਤ;
    ਦਲੀਲਾਂ_2->offsets = myOffsets;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverter_From_Unicode_ਦਲੀਲਾਂ * ਦਲੀਲਾਂ_2,
                                  UError_ਕੋਡ * err)
{
    ਅਚਲ char16_t *my_ਸਰੋਤ = ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *my_ਨਿਸ਼ਾਨਾ;
    ਅਚਲ char16_t *ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    UChar32 ch, ch2;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਆਰਜ਼ੀ[ਆਕਾਰ(uint32_t)];

    ਜੇ(my_ਸਰੋਤ >= ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /* no input, nothing to do */
        ਵਾਪਸ;
    }

    /* write the BOM if necessary */
    ਜੇ(ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ==UCNV_NEED_ਮੰਜ਼ਿਲ_WRITE_BOM) {
        ਸਥਿਰ ਅਚਲ ਅੱਖਰ bom[]={ (ਅੱਖਰ)0xffu, (ਅੱਖਰ)0xfeu, 0, 0 };
        ucnv_from_UWrite_ਬਾਈਟਾਂ(ਦਲੀਲਾਂ_2->converter,
                             bom, 4,
                             &ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ, ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                             &ਦਲੀਲਾਂ_2->offsets, -1,
                             err);
        ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ=0;
    }

    my_ਨਿਸ਼ਾਨਾ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    ਆਰਜ਼ੀ[3] = 0;

    ਜੇ (ਦਲੀਲਾਂ_2->converter->fromUChar32)
    {
        ch = ਦਲੀਲਾਂ_2->converter->fromUChar32;
        ਦਲੀਲਾਂ_2->converter->fromUChar32 = 0;
        ਜਾਓ lowsurogate;
    }

    ਜਦੋਂ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
    {
        ch = *(my_ਸਰੋਤ++);

        ਜੇ (U16_IS_SURROGATE(ch)) {
            ਜੇ (U16_IS_LEAD(ch))
            {
lowsurogate:
                ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ)
                {
                    ch2 = *my_ਸਰੋਤ;
                    ਜੇ (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ਉੱਚਾ_ਸ਼ੁਰੂ) << HALF_ਖਿਸਕਾਓ) + ch2 + SURROGATE_ਨੀਵਾਂ_ਅਧਾਰ;
                        my_ਸਰੋਤ++;
                    }
                    ਨਹੀਂਤਾਂ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        ਦਲੀਲਾਂ_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਰੋਕੋ;
                    }
                }
                ਨਹੀਂਤਾਂ {
                    /* ran out of source */
                    ਦਲੀਲਾਂ_2->converter->fromUChar32 = ch;
                    ਜੇ (ਦਲੀਲਾਂ_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    }
                    ਰੋਕੋ;
                }
            }
            ਨਹੀਂਤਾਂ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                ਦਲੀਲਾਂ_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਰੋਕੋ;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        ਆਰਜ਼ੀ[2] = (uint8_t) (ch >> 16 & 0x1F);
        ਆਰਜ਼ੀ[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        ਆਰਜ਼ੀ[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        ਲਈ (ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write = 0; ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write <= ਆਕਾਰ(uint32_t) - 1; ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write++)
        {
            ਜੇ (my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
            {
                *(my_ਨਿਸ਼ਾਨਾ++) = ਆਰਜ਼ੀ[ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write];
            }
            ਨਹੀਂਤਾਂ
            {
                ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] = ਆਰਜ਼ੀ[ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write];
                *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
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

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
T_UConverter_from_Unicode_UTF32_LE_ਵਿੱਥ_LOGIC(UConverter_From_Unicode_ਦਲੀਲਾਂ * ਦਲੀਲਾਂ_2,
                                               UError_ਕੋਡ * err)
{
    ਅਚਲ char16_t *my_ਸਰੋਤ = ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *my_ਨਿਸ਼ਾਨਾ;
    int32_t *myOffsets;
    ਅਚਲ char16_t *ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    UChar32 ch, ch2;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਆਰਜ਼ੀ[ਆਕਾਰ(uint32_t)];
    int32_t ਵਿੱਥ_ਅੰਕ = 0;

    ਜੇ(my_ਸਰੋਤ >= ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /* no input, nothing to do */
        ਵਾਪਸ;
    }

    /* write the BOM if necessary */
    ਜੇ(ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ==UCNV_NEED_ਮੰਜ਼ਿਲ_WRITE_BOM) {
        ਸਥਿਰ ਅਚਲ ਅੱਖਰ bom[]={ (ਅੱਖਰ)0xffu, (ਅੱਖਰ)0xfeu, 0, 0 };
        ucnv_from_UWrite_ਬਾਈਟਾਂ(ਦਲੀਲਾਂ_2->converter,
                             bom, 4,
                             &ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ, ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                             &ਦਲੀਲਾਂ_2->offsets, -1,
                             err);
        ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ=0;
    }

    my_ਨਿਸ਼ਾਨਾ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    myOffsets = ਦਲੀਲਾਂ_2->offsets;
    ਆਰਜ਼ੀ[3] = 0;

    ਜੇ (ਦਲੀਲਾਂ_2->converter->fromUChar32)
    {
        ch = ਦਲੀਲਾਂ_2->converter->fromUChar32;
        ਦਲੀਲਾਂ_2->converter->fromUChar32 = 0;
        ਜਾਓ lowsurogate;
    }

    ਜਦੋਂ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
    {
        ch = *(my_ਸਰੋਤ++);

        ਜੇ (U16_IS_SURROGATE(ch)) {
            ਜੇ (U16_IS_LEAD(ch))
            {
lowsurogate:
                ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ)
                {
                    ch2 = *my_ਸਰੋਤ;
                    ਜੇ (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_ਉੱਚਾ_ਸ਼ੁਰੂ) << HALF_ਖਿਸਕਾਓ) + ch2 + SURROGATE_ਨੀਵਾਂ_ਅਧਾਰ;
                        my_ਸਰੋਤ++;
                    }
                    ਨਹੀਂਤਾਂ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        ਦਲੀਲਾਂ_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਰੋਕੋ;
                    }
                }
                ਨਹੀਂਤਾਂ {
                    /* ran out of source */
                    ਦਲੀਲਾਂ_2->converter->fromUChar32 = ch;
                    ਜੇ (ਦਲੀਲਾਂ_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    }
                    ਰੋਕੋ;
                }
            }
            ਨਹੀਂਤਾਂ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                ਦਲੀਲਾਂ_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਰੋਕੋ;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        ਆਰਜ਼ੀ[2] = (uint8_t) (ch >> 16 & 0x1F);
        ਆਰਜ਼ੀ[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        ਆਰਜ਼ੀ[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        ਲਈ (ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write = 0; ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write <= ਆਕਾਰ(uint32_t) - 1; ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write++)
        {
            ਜੇ (my_ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
            {
                *(my_ਨਿਸ਼ਾਨਾ++) = ਆਰਜ਼ੀ[ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write];
                *(myOffsets++) = ਵਿੱਥ_ਅੰਕ;
            }
            ਨਹੀਂਤਾਂ
            {
                ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] = ਆਰਜ਼ੀ[ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_Write];
                *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
            }
        }
        ਵਿੱਥ_ਅੰਕ = ਵਿੱਥ_ਅੰਕ + 1 + (ਆਰਜ਼ੀ[2] != 0);
    }

    ਜੇ (my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ >= ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ && U_SUCCESS(*err))
    {
        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = (ਅੱਖਰ *) my_ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
    ਦਲੀਲਾਂ_2->offsets = myOffsets;
}

ਸਥਿਰ UChar32 U_CALLCONV
T_UConverter_get_ਅਗਲਾ_UChar_UTF32_LE(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ* ਦਲੀਲਾਂ_2,
                                   UError_ਕੋਡ* err)
{
    ਅਚਲ uint8_t *my_ਸਰੋਤ;
    UChar32 myUChar;
    int32_t length;

    my_ਸਰੋਤ = (ਅਚਲ uint8_t *)ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    ਜੇ (my_ਸਰੋਤ >= (ਅਚਲ uint8_t *)ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ)
    {
        /* no input */
        *err = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0xffff;
    }

    length = (int32_t)((ਅਚਲ uint8_t *)ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ - my_ਸਰੋਤ);
    ਜੇ (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes, my_ਸਰੋਤ, length);
        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = (int8_t)length;
        ਦਲੀਲਾਂ_2->ਸਰੋਤ = (ਅਚਲ ਅੱਖਰ *)(my_ਸਰੋਤ + length);
        *err = U_TRUNCATED_CHAR_ਮਿਲਿਆ;
        ਵਾਪਸ 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)my_ਸਰੋਤ[3] << 24)
            | ((UChar32)my_ਸਰੋਤ[2] << 16)
            | ((UChar32)my_ਸਰੋਤ[1] << 8)
            | ((UChar32)my_ਸਰੋਤ[0]);

    ਦਲੀਲਾਂ_2->ਸਰੋਤ = (ਅਚਲ ਅੱਖਰ *)(my_ਸਰੋਤ + 4);
    ਜੇ ((uint32_t)myUChar <= ਵੱਧ_ਤੋਂ_ਵੱਧ_UTF && !U_IS_SURROGATE(myUChar)) {
        ਵਾਪਸ myUChar;
    }

    uprv_memcpy(ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes, my_ਸਰੋਤ, 4);
    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 4;

    *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
    ਵਾਪਸ 0xffff;
}
U_CDECL_ਸਿਰਾ
ਸਥਿਰ ਅਚਲ UConverterImpl _UTF32LEImpl = {
    UCNV_UTF32_LittleEndian,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    T_UConverter_ਮੰਜ਼ਿਲ_Unicode_UTF32_LE,
    T_UConverter_ਮੰਜ਼ਿਲ_Unicode_UTF32_LE_ਵਿੱਥ_LOGIC,
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_ਵਿੱਥ_LOGIC,
    T_UConverter_get_ਅਗਲਾ_UChar_UTF32_LE,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_get_Non_Surrogate_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ UTF32_LEStatic_ਅੰਕੜੇ = {
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, ਝੂਠ, ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ਅਚਲ UConverter_Shared_ਅੰਕੜੇ _UTF32LEData =
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&UTF32_LEStatic_ਅੰਕੜੇ, &_UTF32LEImpl);

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
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_UTF32Reset(UConverter *cnv, UConverterResetChoice choice) {
    ਜੇ(choice<=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->ਬਹੁਲਕ=0;
    }
    ਜੇ(choice!=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->from_Unicode_ਸਥਿਤੀ=UCNV_NEED_ਮੰਜ਼ਿਲ_WRITE_BOM;
    }
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
           UError_ਕੋਡ *p_Error_ਕੋਡ) {
    (ਖਾਲੀ)p_ਦਲੀਲਾਂ;
    (ਖਾਲੀ)p_Error_ਕੋਡ;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ utf32BOM[8]={ 0, 0, (ਅੱਖਰ)0xfeu, (ਅੱਖਰ)0xffu, (ਅੱਖਰ)0xffu, (ਅੱਖਰ)0xfeu, 0, 0 };

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
UTF32_ਮੰਜ਼ਿਲ_Unicode_With_Offsets(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                           UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv=p_ਦਲੀਲਾਂ->converter;
    ਅਚਲ ਅੱਖਰ *ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਅਚਲ ਅੱਖਰ *ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    int32_t *offsets=p_ਦਲੀਲਾਂ->offsets;

    int32_t state, ਵਿੱਥ_Delta;
    ਅੱਖਰ b;

    state=cnv->ਬਹੁਲਕ;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    ਵਿੱਥ_Delta=0;

    ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && U_SUCCESS(*p_Error_ਕੋਡ)) {
        ਚੋਣ(state) {
        ਹਾਲਤ 0:
            b=*ਸਰੋਤ;
            ਜੇ(b==0) {
                state=1; /* could be 00 00 FE FF */
            } ਨਹੀਂਤਾਂ ਜੇ(b==(ਅੱਖਰ)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } ਨਹੀਂਤਾਂ {
                state=8; /* default to UTF-32BE */
                ਜਾਰੀ;
            }
            ++ਸਰੋਤ;
            ਰੋਕੋ;
        ਹਾਲਤ 1:
        ਹਾਲਤ 2:
        ਹਾਲਤ 3:
        ਹਾਲਤ 5:
        ਹਾਲਤ 6:
        ਹਾਲਤ 7:
            ਜੇ(*ਸਰੋਤ==utf32BOM[state]) {
                ++state;
                ++ਸਰੋਤ;
                ਜੇ(state==4) {
                    state=8; /* detect UTF-32BE */
                    ਵਿੱਥ_Delta=(int32_t)(ਸਰੋਤ-p_ਦਲੀਲਾਂ->ਸਰੋਤ);
                } ਨਹੀਂਤਾਂ ਜੇ(state==8) {
                    state=9; /* detect UTF-32LE */
                    ਵਿੱਥ_Delta=(int32_t)(ਸਰੋਤ-p_ਦਲੀਲਾਂ->ਸਰੋਤ);
                }
            } ਨਹੀਂਤਾਂ {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(ਸਰੋਤ-p_ਦਲੀਲਾਂ->ਸਰੋਤ); /* number of bytes from this buffer */

                /* reset the source */
                ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;

                ਜੇ(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } ਨਹੀਂਤਾਂ {
                    UBool oldFlush=p_ਦਲੀਲਾਂ->flush;

                    /* some of the bytes are from a previous buffer, replay those first */
                    p_ਦਲੀਲਾਂ->ਸਰੋਤ=utf32BOM+(state&4); /* select the correct BOM */
                    p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ+((state&3)-count); /* replay previous bytes */
                    p_ਦਲੀਲਾਂ->flush=ਝੂਠ; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_ਮੰਜ਼ਿਲ_Unicode_UTF32_BE(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ=ਸਰੋਤ_ਹੱਦਬੰਦੀ;
                    p_ਦਲੀਲਾਂ->flush=oldFlush;
                }
                state=8;
                ਜਾਰੀ;
            }
            ਰੋਕੋ;
        ਹਾਲਤ 8:
            /* call UTF-32BE */
            p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
            ਜੇ(offsets==ਖਾਲੀ_ਸੂਚਕ) {
                T_UConverter_ਮੰਜ਼ਿਲ_Unicode_UTF32_BE(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
            } ਨਹੀਂਤਾਂ {
                T_UConverter_ਮੰਜ਼ਿਲ_Unicode_UTF32_BE_ਵਿੱਥ_LOGIC(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
            }
            ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
            ਰੋਕੋ;
        ਹਾਲਤ 9:
            /* call UTF-32LE */
            p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
            ਜੇ(offsets==ਖਾਲੀ_ਸੂਚਕ) {
                T_UConverter_ਮੰਜ਼ਿਲ_Unicode_UTF32_LE(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
            } ਨਹੀਂਤਾਂ {
                T_UConverter_ਮੰਜ਼ਿਲ_Unicode_UTF32_LE_ਵਿੱਥ_LOGIC(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
            }
            ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
            ਰੋਕੋ;
        ਮੂਲ:
            ਰੋਕੋ; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ && ਵਿੱਥ_Delta!=0) {
        int32_t *offsets_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->offsets;
        ਜਦੋਂ(offsets<offsets_ਹੱਦਬੰਦੀ) {
            *offsets++ += ਵਿੱਥ_Delta;
        }
    }

    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;

    ਜੇ(ਸਰੋਤ==ਸਰੋਤ_ਹੱਦਬੰਦੀ && p_ਦਲੀਲਾਂ->flush) {
        /* handle truncated input */
        ਚੋਣ(state) {
        ਹਾਲਤ 0:
            ਰੋਕੋ; /* no input at all, nothing to do */
        ਹਾਲਤ 8:
            T_UConverter_ਮੰਜ਼ਿਲ_Unicode_UTF32_BE(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
            ਰੋਕੋ;
        ਹਾਲਤ 9:
            T_UConverter_ਮੰਜ਼ਿਲ_Unicode_UTF32_LE(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
            ਰੋਕੋ;
        ਮੂਲ:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            p_ਦਲੀਲਾਂ->ਸਰੋਤ=utf32BOM+(state&4); /* select the correct BOM */
            p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_ਮੰਜ਼ਿਲ_Unicode_UTF32_BE(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
            p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
            p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ=ਸਰੋਤ_ਹੱਦਬੰਦੀ;
            state=8;
            ਰੋਕੋ;
        }
    }

    cnv->ਬਹੁਲਕ=state;
}

ਸਥਿਰ UChar32 U_CALLCONV
UTF32_Get_ਅਗਲਾ_UChar(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                   UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਚੋਣ(p_ਦਲੀਲਾਂ->converter->ਬਹੁਲਕ) {
    ਹਾਲਤ 8:
        ਵਾਪਸ T_UConverter_get_ਅਗਲਾ_UChar_UTF32_BE(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
    ਹਾਲਤ 9:
        ਵਾਪਸ T_UConverter_get_ਅਗਲਾ_UChar_UTF32_LE(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
    ਮੂਲ:
        ਵਾਪਸ UCNV_GET_ਅਗਲਾ_UCHAR_USE_ਮੰਜ਼ਿਲ_U;
    }
}
U_CDECL_ਸਿਰਾ
ਸਥਿਰ ਅਚਲ UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    _UTF32Open,
    ਖਾਲੀ_ਸੂਚਕ,
    _UTF32Reset,

    UTF32_ਮੰਜ਼ਿਲ_Unicode_With_Offsets,
    UTF32_ਮੰਜ਼ਿਲ_Unicode_With_Offsets,
#ਜੇ U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_ਵਿੱਥ_LOGIC,
#ਨਹੀਂਤਾਂ
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_ਵਿੱਥ_LOGIC,
#ਜੇ_ਅੰਤ
    UTF32_Get_ਅਗਲਾ_UChar,

    ਖਾਲੀ_ਸੂਚਕ, /* ### TODO implement getStarters for all Unicode encodings?! */
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_get_Non_Surrogate_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};

/* The 1236 CCSID refers to any version of Unicode with a BOM sensitive endianness of UTF-32 */
ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ UTF32_Static_ਅੰਕੜੇ = {
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#ਜੇ U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#ਨਹੀਂਤਾਂ
    { 0xfd, 0xff, 0, 0 }, 4,
#ਜੇ_ਅੰਤ
    ਝੂਠ, ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ਅਚਲ UConverter_Shared_ਅੰਕੜੇ UTF32_ਅੰਕੜੇ = 
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&UTF32_Static_ਅੰਕੜੇ, &_UTF32Impl);

#ਜੇ_ਅੰਤ
