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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

#ವ್ಯಾಖ್ಯಾನಿಸು ಗರಿಷ್ಠ_UCS2            0x0000FFFF
#ವ್ಯಾಖ್ಯಾನಿಸು ಗರಿಷ್ಠ_UTF             0x0010FFFF
#ವ್ಯಾಖ್ಯಾನಿಸು HALF_ಸರಿಸು              10
#ವ್ಯಾಖ್ಯಾನಿಸು HALF_ತಳ               0x0010000
#ವ್ಯಾಖ್ಯಾನಿಸು HALF_ಮುಖವಾಡ               0x3FF
#ವ್ಯಾಖ್ಯಾನಿಸು SURROGATE_ಮೇಲಿನ_ಆರಂಭ    0xD800
#ವ್ಯಾಖ್ಯಾನಿಸು SURROGATE_ಕೆಳಗಿನ_ಆರಂಭ     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#ವ್ಯಾಖ್ಯಾನಿಸು SURROGATE_ಕೆಳಗಿನ_ತಳ      9216

ಎಣಿಕೆ {
    UCNV_NEED_ಗಮ್ಯ_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_ಗಮ್ಯ_Unicode_UTF32_BE(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು * ವಾದಗಳು_2,
                                UError_ಸಂಕೇತ * err)
{
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *my_ಮೂಲ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಮೂಲ;
    char16_t *my_ಗುರಿ = ವಾದಗಳು_2->ಗುರಿ;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಮೂಲ_ಎಲ್ಲೆ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಗಮ್ಯ_UBytes = ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    ಆದರೆ (ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength > 0 && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ) {
        i = ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength;       /* restore # of bytes consumed */
        ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 0;

        ch = ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ - 1;/*Stores the previously calculated ch from a previous call*/
        ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0;
        ಹೋಗು morebytes;
    }

    ಆಗ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ) {
        i = 0;
        ch = 0;
morebytes:
        ಆಗ (i < ಗಾತ್ರ(uint32_t)) {
            ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ) {
                ch = (ch << 8) | (uint8_t)(*my_ಮೂಲ);
                ಗಮ್ಯ_UBytes[i++] = (ಅಕ್ಷರ) *(my_ಮೂಲ++);
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = ch + 1;
                ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = (int8_t) i;
                ಹೋಗು donefornow;
            }
        }

        ಆದರೆ (ch <= ಗರಿಷ್ಠ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ಆದರೆ (ch <= ಗರಿಷ್ಠ_UCS2) 
            {
                /* fits in 16 bits */
                *(my_ಗುರಿ++) = (char16_t) ch;
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                /* write out the surrogates */
                *(my_ಗುರಿ++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                ಆದರೆ (my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ) {
                    *(my_ಗುರಿ++) = (char16_t)ch;
                }
                ಇಲ್ಲದಿದ್ದರೆ {
                    /* Put in overflow buffer (not handled here) */
                    ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್[0] = (char16_t) ch;
                    ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ = 1;
                    *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
            ನಿಲ್ಲಿಸು;
        }
    }

donefornow:
    ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ >= ಗುರಿ_ಎಲ್ಲೆ && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
    ವಾದಗಳು_2->ಮೂಲ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) my_ಮೂಲ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_ಗಮ್ಯ_Unicode_UTF32_BE_ಸ್ಥಾನಾಂತರ_LOGIC(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು * ವಾದಗಳು_2,
                                             UError_ಸಂಕೇತ * err)
{
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *my_ಮೂಲ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಮೂಲ;
    char16_t *my_ಗುರಿ = ವಾದಗಳು_2->ಗುರಿ;
    int32_t *myOffsets = ವಾದಗಳು_2->offsets;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಮೂಲ_ಎಲ್ಲೆ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಗಮ್ಯ_UBytes = ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes;
    uint32_t ch, i;
    int32_t ಸ್ಥಾನಾಂತರ_ಅಂಕೆ = 0;

    /* Restore state of current sequence */
    ಆದರೆ (ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength > 0 && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ) {
        i = ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength;       /* restore # of bytes consumed */
        ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 0;

        ch = ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ - 1;/*Stores the previously calculated ch from a previous call*/
        ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0;
        ಹೋಗು morebytes;
    }

    ಆಗ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ) {
        i = 0;
        ch = 0;
morebytes:
        ಆಗ (i < ಗಾತ್ರ(uint32_t)) {
            ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ) {
                ch = (ch << 8) | (uint8_t)(*my_ಮೂಲ);
                ಗಮ್ಯ_UBytes[i++] = (ಅಕ್ಷರ) *(my_ಮೂಲ++);
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = ch + 1;
                ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = (int8_t) i;
                ಹೋಗು donefornow;
            }
        }

        ಆದರೆ (ch <= ಗರಿಷ್ಠ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ಆದರೆ (ch <= ಗರಿಷ್ಠ_UCS2) {
                /* fits in 16 bits */
                *(my_ಗುರಿ++) = (char16_t) ch;
                *(myOffsets++) = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ;
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                /* write out the surrogates */
                *(my_ಗುರಿ++) = U16_LEAD(ch);
                *myOffsets++ = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ;
                ch = U16_TRAIL(ch);
                ಆದರೆ (my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
                {
                    *(my_ಗುರಿ++) = (char16_t)ch;
                    *(myOffsets++) = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ;
                }
                ಇಲ್ಲದಿದ್ದರೆ {
                    /* Put in overflow buffer (not handled here) */
                    ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್[0] = (char16_t) ch;
                    ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ = 1;
                    *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
            ನಿಲ್ಲಿಸು;
        }
        ಸ್ಥಾನಾಂತರ_ಅಂಕೆ += i;
    }

donefornow:
    ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ >= ಗುರಿ_ಎಲ್ಲೆ && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
    ವಾದಗಳು_2->ಮೂಲ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) my_ಮೂಲ;
    ವಾದಗಳು_2->offsets = myOffsets;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverter_From_Unicode_ವಾದಗಳು * ವಾದಗಳು_2,
                                  UError_ಸಂಕೇತ * err)
{
    ಸ್ಥಿರಾಂಕ char16_t *my_ಮೂಲ = ವಾದಗಳು_2->ಮೂಲ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *my_ಗುರಿ;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಗುರಿ_ಎಲ್ಲೆ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ;
    UChar32 ch, ch2;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ತಾತ್ಕಾಲಿಕ[ಗಾತ್ರ(uint32_t)];

    ಆದರೆ(my_ಮೂಲ >= ಮೂಲ_ಎಲ್ಲೆ) {
        /* no input, nothing to do */
        ಹಿಂತಿರುಗಿ;
    }

    /* write the BOM if necessary */
    ಆದರೆ(ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ==UCNV_NEED_ಗಮ್ಯ_WRITE_BOM) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ bom[]={ 0, 0, (ಅಕ್ಷರ)0xfeu, (ಅಕ್ಷರ)0xffu };
        ucnv_from_UWrite_ಬೈಟುಗಳು(ವಾದಗಳು_2->converter,
                             bom, 4,
                             &ವಾದಗಳು_2->ಗುರಿ, ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ,
                             &ವಾದಗಳು_2->offsets, -1,
                             err);
        ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ=0;
    }

    my_ಗುರಿ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಗುರಿ;
    ತಾತ್ಕಾಲಿಕ[0] = 0;

    ಆದರೆ (ವಾದಗಳು_2->converter->fromUChar32) {
        ch = ವಾದಗಳು_2->converter->fromUChar32;
        ವಾದಗಳು_2->converter->fromUChar32 = 0;
        ಹೋಗು lowsurogate;
    }

    ಆಗ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ) {
        ch = *(my_ಮೂಲ++);

        ಆದರೆ (U_IS_SURROGATE(ch)) {
            ಆದರೆ (U_IS_LEAD(ch)) {
lowsurogate:
                ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ) {
                    ch2 = *my_ಮೂಲ;
                    ಆದರೆ (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ಮೇಲಿನ_ಆರಂಭ) << HALF_ಸರಿಸು) + ch2 + SURROGATE_ಕೆಳಗಿನ_ತಳ;
                        my_ಮೂಲ++;
                    }
                    ಇಲ್ಲದಿದ್ದರೆ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        ವಾದಗಳು_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ನಿಲ್ಲಿಸು;
                    }
                }
                ಇಲ್ಲದಿದ್ದರೆ {
                    /* ran out of source */
                    ವಾದಗಳು_2->converter->fromUChar32 = ch;
                    ಆದರೆ (ವಾದಗಳು_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    }
                    ನಿಲ್ಲಿಸು;
                }
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                ವಾದಗಳು_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ನಿಲ್ಲಿಸು;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        ತಾತ್ಕಾಲಿಕ[1] = (uint8_t) (ch >> 16 & 0x1F);
        ತಾತ್ಕಾಲಿಕ[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        ತಾತ್ಕಾಲಿಕ[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        ಗಾಗಿ (ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write = 0; ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write <= ಗಾತ್ರ(uint32_t) - 1; ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write++) {
            ಆದರೆ (my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ) {
                *(my_ಗುರಿ++) = ತಾತ್ಕಾಲಿಕ[ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write];
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] = ತಾತ್ಕಾಲಿಕ[ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write];
                *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            }
        }
    }

    ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ >= ಗುರಿ_ಎಲ್ಲೆ && U_SUCCESS(*err)) {
        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    ವಾದಗಳು_2->ಗುರಿ = (ಅಕ್ಷರ *) my_ಗುರಿ;
    ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_from_Unicode_UTF32_BE_ಸ್ಥಾನಾಂತರ_LOGIC(UConverter_From_Unicode_ವಾದಗಳು * ವಾದಗಳು_2,
                                               UError_ಸಂಕೇತ * err)
{
    ಸ್ಥಿರಾಂಕ char16_t *my_ಮೂಲ = ವಾದಗಳು_2->ಮೂಲ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *my_ಗುರಿ;
    int32_t *myOffsets;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಗುರಿ_ಎಲ್ಲೆ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ;
    UChar32 ch, ch2;
    int32_t ಸ್ಥಾನಾಂತರ_ಅಂಕೆ = 0;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ತಾತ್ಕಾಲಿಕ[ಗಾತ್ರ(uint32_t)];

    ಆದರೆ(my_ಮೂಲ >= ಮೂಲ_ಎಲ್ಲೆ) {
        /* no input, nothing to do */
        ಹಿಂತಿರುಗಿ;
    }

    /* write the BOM if necessary */
    ಆದರೆ(ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ==UCNV_NEED_ಗಮ್ಯ_WRITE_BOM) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ bom[]={ 0, 0, (ಅಕ್ಷರ)0xfeu, (ಅಕ್ಷರ)0xffu };
        ucnv_from_UWrite_ಬೈಟುಗಳು(ವಾದಗಳು_2->converter,
                             bom, 4,
                             &ವಾದಗಳು_2->ಗುರಿ, ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ,
                             &ವಾದಗಳು_2->offsets, -1,
                             err);
        ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ=0;
    }

    my_ಗುರಿ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಗುರಿ;
    myOffsets = ವಾದಗಳು_2->offsets;
    ತಾತ್ಕಾಲಿಕ[0] = 0;

    ಆದರೆ (ವಾದಗಳು_2->converter->fromUChar32) {
        ch = ವಾದಗಳು_2->converter->fromUChar32;
        ವಾದಗಳು_2->converter->fromUChar32 = 0;
        ಹೋಗು lowsurogate;
    }

    ಆಗ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ) {
        ch = *(my_ಮೂಲ++);

        ಆದರೆ (U_IS_SURROGATE(ch)) {
            ಆದರೆ (U_IS_LEAD(ch)) {
lowsurogate:
                ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ) {
                    ch2 = *my_ಮೂಲ;
                    ಆದರೆ (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ಮೇಲಿನ_ಆರಂಭ) << HALF_ಸರಿಸು) + ch2 + SURROGATE_ಕೆಳಗಿನ_ತಳ;
                        my_ಮೂಲ++;
                    }
                    ಇಲ್ಲದಿದ್ದರೆ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        ವಾದಗಳು_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ನಿಲ್ಲಿಸು;
                    }
                }
                ಇಲ್ಲದಿದ್ದರೆ {
                    /* ran out of source */
                    ವಾದಗಳು_2->converter->fromUChar32 = ch;
                    ಆದರೆ (ವಾದಗಳು_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    }
                    ನಿಲ್ಲಿಸು;
                }
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                ವಾದಗಳು_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ನಿಲ್ಲಿಸು;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        ತಾತ್ಕಾಲಿಕ[1] = (uint8_t) (ch >> 16 & 0x1F);
        ತಾತ್ಕಾಲಿಕ[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        ತಾತ್ಕಾಲಿಕ[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        ಗಾಗಿ (ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write = 0; ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write <= ಗಾತ್ರ(uint32_t) - 1; ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write++) {
            ಆದರೆ (my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ) {
                *(my_ಗುರಿ++) = ತಾತ್ಕಾಲಿಕ[ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write];
                *(myOffsets++) = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ;
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] = ತಾತ್ಕಾಲಿಕ[ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write];
                *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            }
        }
        ಸ್ಥಾನಾಂತರ_ಅಂಕೆ = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ + 1 + (ತಾತ್ಕಾಲಿಕ[1] != 0);
    }

    ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ >= ಗುರಿ_ಎಲ್ಲೆ && U_SUCCESS(*err)) {
        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    ವಾದಗಳು_2->ಗುರಿ = (ಅಕ್ಷರ *) my_ಗುರಿ;
    ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
    ವಾದಗಳು_2->offsets = myOffsets;
}

ಸ್ಥಿರ UChar32 U_CALLCONV
T_UConverter_get_ಮುಂದಿನ_UChar_UTF32_BE(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು* ವಾದಗಳು_2,
                                   UError_ಸಂಕೇತ* err)
{
    ಸ್ಥಿರಾಂಕ uint8_t *my_ಮೂಲ;
    UChar32 myUChar;
    int32_t length;

    my_ಮೂಲ = (ಸ್ಥಿರಾಂಕ uint8_t *)ವಾದಗಳು_2->ಮೂಲ;
    ಆದರೆ (my_ಮೂಲ >= (ಸ್ಥಿರಾಂಕ uint8_t *)ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ)
    {
        /* no input */
        *err = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    length = (int32_t)((ಸ್ಥಿರಾಂಕ uint8_t *)ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ - my_ಮೂಲ);
    ಆದರೆ (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes, my_ಮೂಲ, length);
        ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = (int8_t)length;
        ವಾದಗಳು_2->ಮೂಲ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(my_ಮೂಲ + length);
        *err = U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು;
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)my_ಮೂಲ[0] << 24)
            | ((UChar32)my_ಮೂಲ[1] << 16)
            | ((UChar32)my_ಮೂಲ[2] << 8)
            | ((UChar32)my_ಮೂಲ[3]);

    ವಾದಗಳು_2->ಮೂಲ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(my_ಮೂಲ + 4);
    ಆದರೆ ((uint32_t)myUChar <= ಗರಿಷ್ಠ_UTF && !U_IS_SURROGATE(myUChar)) {
        ಹಿಂತಿರುಗಿ myUChar;
    }

    uprv_memcpy(ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes, my_ಮೂಲ, 4);
    ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 4;

    *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
    ಹಿಂತಿರುಗಿ 0xffff;
}
U_CDECL_ಕೊನೆ
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _UTF32BEImpl = {
    UCNV_UTF32_BigEndian,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    T_UConverter_ಗಮ್ಯ_Unicode_UTF32_BE,
    T_UConverter_ಗಮ್ಯ_Unicode_UTF32_BE_ಸ್ಥಾನಾಂತರ_LOGIC,
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_ಸ್ಥಾನಾಂತರ_LOGIC,
    T_UConverter_get_ಮುಂದಿನ_UChar_UTF32_BE,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_get_Non_Surrogate_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ UTF32_BEStatic_ದತ್ತಾಂಶ = {
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, ಸುಳ್ಳು, ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&UTF32_BEStatic_ದತ್ತಾಂಶ, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_ಗಮ್ಯ_Unicode_UTF32_LE(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು * ವಾದಗಳು_2,
                                UError_ಸಂಕೇತ * err)
{
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *my_ಮೂಲ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಮೂಲ;
    char16_t *my_ಗುರಿ = ವಾದಗಳು_2->ಗುರಿ;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಮೂಲ_ಎಲ್ಲೆ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಗಮ್ಯ_UBytes = ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    ಆದರೆ (ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength > 0 && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
    {
        i = ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength;       /* restore # of bytes consumed */
        ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ - 1;
        ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0;
        ಹೋಗು morebytes;
    }

    ಆಗ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
    {
        i = 0;
        ch = 0;
morebytes:
        ಆಗ (i < ಗಾತ್ರ(uint32_t))
        {
            ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ)
            {
                ch |= ((uint8_t)(*my_ಮೂಲ)) << (i * 8);
                ಗಮ್ಯ_UBytes[i++] = (ಅಕ್ಷರ) *(my_ಮೂಲ++);
            }
            ಇಲ್ಲದಿದ್ದರೆ
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = ch + 1;
                ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = (int8_t) i;
                ಹೋಗು donefornow;
            }
        }

        ಆದರೆ (ch <= ಗರಿಷ್ಠ_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ಆದರೆ (ch <= ಗರಿಷ್ಠ_UCS2) {
                /* fits in 16 bits */
                *(my_ಗುರಿ++) = (char16_t) ch;
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                /* write out the surrogates */
                *(my_ಗುರಿ++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                ಆದರೆ (my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ) {
                    *(my_ಗುರಿ++) = (char16_t)ch;
                }
                ಇಲ್ಲದಿದ್ದರೆ {
                    /* Put in overflow buffer (not handled here) */
                    ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್[0] = (char16_t) ch;
                    ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ = 1;
                    *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
            ನಿಲ್ಲಿಸು;
        }
    }

donefornow:
    ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ >= ಗುರಿ_ಎಲ್ಲೆ && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
    ವಾದಗಳು_2->ಮೂಲ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) my_ಮೂಲ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_ಗಮ್ಯ_Unicode_UTF32_LE_ಸ್ಥಾನಾಂತರ_LOGIC(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು * ವಾದಗಳು_2,
                                             UError_ಸಂಕೇತ * err)
{
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *my_ಮೂಲ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಮೂಲ;
    char16_t *my_ಗುರಿ = ವಾದಗಳು_2->ಗುರಿ;
    int32_t *myOffsets = ವಾದಗಳು_2->offsets;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಮೂಲ_ಎಲ್ಲೆ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಗಮ್ಯ_UBytes = ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes;
    uint32_t ch, i;
    int32_t ಸ್ಥಾನಾಂತರ_ಅಂಕೆ = 0;

    /* Restore state of current sequence */
    ಆದರೆ (ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength > 0 && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
    {
        i = ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength;       /* restore # of bytes consumed */
        ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ - 1;
        ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0;
        ಹೋಗು morebytes;
    }

    ಆಗ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
    {
        i = 0;
        ch = 0;
morebytes:
        ಆಗ (i < ಗಾತ್ರ(uint32_t))
        {
            ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ)
            {
                ch |= ((uint8_t)(*my_ಮೂಲ)) << (i * 8);
                ಗಮ್ಯ_UBytes[i++] = (ಅಕ್ಷರ) *(my_ಮೂಲ++);
            }
            ಇಲ್ಲದಿದ್ದರೆ
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = ch + 1;
                ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = (int8_t) i;
                ಹೋಗು donefornow;
            }
        }

        ಆದರೆ (ch <= ಗರಿಷ್ಠ_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            ಆದರೆ (ch <= ಗರಿಷ್ಠ_UCS2) 
            {
                /* fits in 16 bits */
                *(my_ಗುರಿ++) = (char16_t) ch;
                *(myOffsets++) = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ;
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                /* write out the surrogates */
                *(my_ಗುರಿ++) = U16_LEAD(ch);
                *(myOffsets++) = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ;
                ch = U16_TRAIL(ch);
                ಆದರೆ (my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
                {
                    *(my_ಗುರಿ++) = (char16_t)ch;
                    *(myOffsets++) = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ;
                }
                ಇಲ್ಲದಿದ್ದರೆ
                {
                    /* Put in overflow buffer (not handled here) */
                    ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್[0] = (char16_t) ch;
                    ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ = 1;
                    *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ
        {
            ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
            ನಿಲ್ಲಿಸು;
        }
        ಸ್ಥಾನಾಂತರ_ಅಂಕೆ += i;
    }

donefornow:
    ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ >= ಗುರಿ_ಎಲ್ಲೆ && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
    ವಾದಗಳು_2->ಮೂಲ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) my_ಮೂಲ;
    ವಾದಗಳು_2->offsets = myOffsets;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverter_From_Unicode_ವಾದಗಳು * ವಾದಗಳು_2,
                                  UError_ಸಂಕೇತ * err)
{
    ಸ್ಥಿರಾಂಕ char16_t *my_ಮೂಲ = ವಾದಗಳು_2->ಮೂಲ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *my_ಗುರಿ;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಗುರಿ_ಎಲ್ಲೆ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ;
    UChar32 ch, ch2;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ತಾತ್ಕಾಲಿಕ[ಗಾತ್ರ(uint32_t)];

    ಆದರೆ(my_ಮೂಲ >= ಮೂಲ_ಎಲ್ಲೆ) {
        /* no input, nothing to do */
        ಹಿಂತಿರುಗಿ;
    }

    /* write the BOM if necessary */
    ಆದರೆ(ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ==UCNV_NEED_ಗಮ್ಯ_WRITE_BOM) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ bom[]={ (ಅಕ್ಷರ)0xffu, (ಅಕ್ಷರ)0xfeu, 0, 0 };
        ucnv_from_UWrite_ಬೈಟುಗಳು(ವಾದಗಳು_2->converter,
                             bom, 4,
                             &ವಾದಗಳು_2->ಗುರಿ, ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ,
                             &ವಾದಗಳು_2->offsets, -1,
                             err);
        ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ=0;
    }

    my_ಗುರಿ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಗುರಿ;
    ತಾತ್ಕಾಲಿಕ[3] = 0;

    ಆದರೆ (ವಾದಗಳು_2->converter->fromUChar32)
    {
        ch = ವಾದಗಳು_2->converter->fromUChar32;
        ವಾದಗಳು_2->converter->fromUChar32 = 0;
        ಹೋಗು lowsurogate;
    }

    ಆಗ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
    {
        ch = *(my_ಮೂಲ++);

        ಆದರೆ (U16_IS_SURROGATE(ch)) {
            ಆದರೆ (U16_IS_LEAD(ch))
            {
lowsurogate:
                ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ)
                {
                    ch2 = *my_ಮೂಲ;
                    ಆದರೆ (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_ಮೇಲಿನ_ಆರಂಭ) << HALF_ಸರಿಸು) + ch2 + SURROGATE_ಕೆಳಗಿನ_ತಳ;
                        my_ಮೂಲ++;
                    }
                    ಇಲ್ಲದಿದ್ದರೆ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        ವಾದಗಳು_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ನಿಲ್ಲಿಸು;
                    }
                }
                ಇಲ್ಲದಿದ್ದರೆ {
                    /* ran out of source */
                    ವಾದಗಳು_2->converter->fromUChar32 = ch;
                    ಆದರೆ (ವಾದಗಳು_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    }
                    ನಿಲ್ಲಿಸು;
                }
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                ವಾದಗಳು_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ನಿಲ್ಲಿಸು;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        ತಾತ್ಕಾಲಿಕ[2] = (uint8_t) (ch >> 16 & 0x1F);
        ತಾತ್ಕಾಲಿಕ[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        ತಾತ್ಕಾಲಿಕ[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        ಗಾಗಿ (ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write = 0; ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write <= ಗಾತ್ರ(uint32_t) - 1; ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write++)
        {
            ಆದರೆ (my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
            {
                *(my_ಗುರಿ++) = ತಾತ್ಕಾಲಿಕ[ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write];
            }
            ಇಲ್ಲದಿದ್ದರೆ
            {
                ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] = ತಾತ್ಕಾಲಿಕ[ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write];
                *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            }
        }
    }

    ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ >= ಗುರಿ_ಎಲ್ಲೆ && U_SUCCESS(*err))
    {
        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    ವಾದಗಳು_2->ಗುರಿ = (ಅಕ್ಷರ *) my_ಗುರಿ;
    ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_from_Unicode_UTF32_LE_ಸ್ಥಾನಾಂತರ_LOGIC(UConverter_From_Unicode_ವಾದಗಳು * ವಾದಗಳು_2,
                                               UError_ಸಂಕೇತ * err)
{
    ಸ್ಥಿರಾಂಕ char16_t *my_ಮೂಲ = ವಾದಗಳು_2->ಮೂಲ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *my_ಗುರಿ;
    int32_t *myOffsets;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಗುರಿ_ಎಲ್ಲೆ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ;
    UChar32 ch, ch2;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ತಾತ್ಕಾಲಿಕ[ಗಾತ್ರ(uint32_t)];
    int32_t ಸ್ಥಾನಾಂತರ_ಅಂಕೆ = 0;

    ಆದರೆ(my_ಮೂಲ >= ಮೂಲ_ಎಲ್ಲೆ) {
        /* no input, nothing to do */
        ಹಿಂತಿರುಗಿ;
    }

    /* write the BOM if necessary */
    ಆದರೆ(ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ==UCNV_NEED_ಗಮ್ಯ_WRITE_BOM) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ bom[]={ (ಅಕ್ಷರ)0xffu, (ಅಕ್ಷರ)0xfeu, 0, 0 };
        ucnv_from_UWrite_ಬೈಟುಗಳು(ವಾದಗಳು_2->converter,
                             bom, 4,
                             &ವಾದಗಳು_2->ಗುರಿ, ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ,
                             &ವಾದಗಳು_2->offsets, -1,
                             err);
        ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ=0;
    }

    my_ಗುರಿ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಗುರಿ;
    myOffsets = ವಾದಗಳು_2->offsets;
    ತಾತ್ಕಾಲಿಕ[3] = 0;

    ಆದರೆ (ವಾದಗಳು_2->converter->fromUChar32)
    {
        ch = ವಾದಗಳು_2->converter->fromUChar32;
        ವಾದಗಳು_2->converter->fromUChar32 = 0;
        ಹೋಗು lowsurogate;
    }

    ಆಗ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
    {
        ch = *(my_ಮೂಲ++);

        ಆದರೆ (U16_IS_SURROGATE(ch)) {
            ಆದರೆ (U16_IS_LEAD(ch))
            {
lowsurogate:
                ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ)
                {
                    ch2 = *my_ಮೂಲ;
                    ಆದರೆ (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_ಮೇಲಿನ_ಆರಂಭ) << HALF_ಸರಿಸು) + ch2 + SURROGATE_ಕೆಳಗಿನ_ತಳ;
                        my_ಮೂಲ++;
                    }
                    ಇಲ್ಲದಿದ್ದರೆ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        ವಾದಗಳು_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ನಿಲ್ಲಿಸು;
                    }
                }
                ಇಲ್ಲದಿದ್ದರೆ {
                    /* ran out of source */
                    ವಾದಗಳು_2->converter->fromUChar32 = ch;
                    ಆದರೆ (ವಾದಗಳು_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    }
                    ನಿಲ್ಲಿಸು;
                }
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                ವಾದಗಳು_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ನಿಲ್ಲಿಸು;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        ತಾತ್ಕಾಲಿಕ[2] = (uint8_t) (ch >> 16 & 0x1F);
        ತಾತ್ಕಾಲಿಕ[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        ತಾತ್ಕಾಲಿಕ[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        ಗಾಗಿ (ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write = 0; ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write <= ಗಾತ್ರ(uint32_t) - 1; ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write++)
        {
            ಆದರೆ (my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
            {
                *(my_ಗುರಿ++) = ತಾತ್ಕಾಲಿಕ[ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write];
                *(myOffsets++) = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ;
            }
            ಇಲ್ಲದಿದ್ದರೆ
            {
                ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] = ತಾತ್ಕಾಲಿಕ[ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write];
                *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            }
        }
        ಸ್ಥಾನಾಂತರ_ಅಂಕೆ = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ + 1 + (ತಾತ್ಕಾಲಿಕ[2] != 0);
    }

    ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ >= ಗುರಿ_ಎಲ್ಲೆ && U_SUCCESS(*err))
    {
        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    ವಾದಗಳು_2->ಗುರಿ = (ಅಕ್ಷರ *) my_ಗುರಿ;
    ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
    ವಾದಗಳು_2->offsets = myOffsets;
}

ಸ್ಥಿರ UChar32 U_CALLCONV
T_UConverter_get_ಮುಂದಿನ_UChar_UTF32_LE(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು* ವಾದಗಳು_2,
                                   UError_ಸಂಕೇತ* err)
{
    ಸ್ಥಿರಾಂಕ uint8_t *my_ಮೂಲ;
    UChar32 myUChar;
    int32_t length;

    my_ಮೂಲ = (ಸ್ಥಿರಾಂಕ uint8_t *)ವಾದಗಳು_2->ಮೂಲ;
    ಆದರೆ (my_ಮೂಲ >= (ಸ್ಥಿರಾಂಕ uint8_t *)ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ)
    {
        /* no input */
        *err = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    length = (int32_t)((ಸ್ಥಿರಾಂಕ uint8_t *)ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ - my_ಮೂಲ);
    ಆದರೆ (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes, my_ಮೂಲ, length);
        ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = (int8_t)length;
        ವಾದಗಳು_2->ಮೂಲ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(my_ಮೂಲ + length);
        *err = U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು;
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)my_ಮೂಲ[3] << 24)
            | ((UChar32)my_ಮೂಲ[2] << 16)
            | ((UChar32)my_ಮೂಲ[1] << 8)
            | ((UChar32)my_ಮೂಲ[0]);

    ವಾದಗಳು_2->ಮೂಲ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(my_ಮೂಲ + 4);
    ಆದರೆ ((uint32_t)myUChar <= ಗರಿಷ್ಠ_UTF && !U_IS_SURROGATE(myUChar)) {
        ಹಿಂತಿರುಗಿ myUChar;
    }

    uprv_memcpy(ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes, my_ಮೂಲ, 4);
    ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 4;

    *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
    ಹಿಂತಿರುಗಿ 0xffff;
}
U_CDECL_ಕೊನೆ
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _UTF32LEImpl = {
    UCNV_UTF32_LittleEndian,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    T_UConverter_ಗಮ್ಯ_Unicode_UTF32_LE,
    T_UConverter_ಗಮ್ಯ_Unicode_UTF32_LE_ಸ್ಥಾನಾಂತರ_LOGIC,
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_ಸ್ಥಾನಾಂತರ_LOGIC,
    T_UConverter_get_ಮುಂದಿನ_UChar_UTF32_LE,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_get_Non_Surrogate_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ UTF32_LEStatic_ದತ್ತಾಂಶ = {
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, ಸುಳ್ಳು, ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ _UTF32LEData =
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&UTF32_LEStatic_ದತ್ತಾಂಶ, &_UTF32LEImpl);

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
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_UTF32Reset(UConverter *cnv, UConverterResetChoice choice) {
    ಆದರೆ(choice<=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->ಬಹುಲಕ=0;
    }
    ಆದರೆ(choice!=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->from_Unicode_ಸ್ಥಿತಿ=UCNV_NEED_ಗಮ್ಯ_WRITE_BOM;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು,
           UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    (ಶೂನ್ಯ)p_ವಾದಗಳು;
    (ಶೂನ್ಯ)p_Error_ಸಂಕೇತ;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ utf32BOM[8]={ 0, 0, (ಅಕ್ಷರ)0xfeu, (ಅಕ್ಷರ)0xffu, (ಅಕ್ಷರ)0xffu, (ಅಕ್ಷರ)0xfeu, 0, 0 };

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
UTF32_ಗಮ್ಯ_Unicode_With_Offsets(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                           UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv=p_ವಾದಗಳು->converter;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    int32_t *offsets=p_ವಾದಗಳು->offsets;

    int32_t state, ಸ್ಥಾನಾಂತರ_Delta;
    ಅಕ್ಷರ b;

    state=cnv->ಬಹುಲಕ;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    ಸ್ಥಾನಾಂತರ_Delta=0;

    ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
        ಆಯ್ಕೆ(state) {
        ಸ್ಥಿತಿ 0:
            b=*ಮೂಲ;
            ಆದರೆ(b==0) {
                state=1; /* could be 00 00 FE FF */
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b==(ಅಕ್ಷರ)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } ಇಲ್ಲದಿದ್ದರೆ {
                state=8; /* default to UTF-32BE */
                ಮುಂದುವರಿ;
            }
            ++ಮೂಲ;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 1:
        ಸ್ಥಿತಿ 2:
        ಸ್ಥಿತಿ 3:
        ಸ್ಥಿತಿ 5:
        ಸ್ಥಿತಿ 6:
        ಸ್ಥಿತಿ 7:
            ಆದರೆ(*ಮೂಲ==utf32BOM[state]) {
                ++state;
                ++ಮೂಲ;
                ಆದರೆ(state==4) {
                    state=8; /* detect UTF-32BE */
                    ಸ್ಥಾನಾಂತರ_Delta=(int32_t)(ಮೂಲ-p_ವಾದಗಳು->ಮೂಲ);
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(state==8) {
                    state=9; /* detect UTF-32LE */
                    ಸ್ಥಾನಾಂತರ_Delta=(int32_t)(ಮೂಲ-p_ವಾದಗಳು->ಮೂಲ);
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(ಮೂಲ-p_ವಾದಗಳು->ಮೂಲ); /* number of bytes from this buffer */

                /* reset the source */
                ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;

                ಆದರೆ(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } ಇಲ್ಲದಿದ್ದರೆ {
                    UBool oldFlush=p_ವಾದಗಳು->flush;

                    /* some of the bytes are from a previous buffer, replay those first */
                    p_ವಾದಗಳು->ಮೂಲ=utf32BOM+(state&4); /* select the correct BOM */
                    p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ+((state&3)-count); /* replay previous bytes */
                    p_ವಾದಗಳು->flush=ಸುಳ್ಳು; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_ಗಮ್ಯ_Unicode_UTF32_BE(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ=ಮೂಲ_ಎಲ್ಲೆ;
                    p_ವಾದಗಳು->flush=oldFlush;
                }
                state=8;
                ಮುಂದುವರಿ;
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 8:
            /* call UTF-32BE */
            p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
            ಆದರೆ(offsets==ಶೂನ್ಯಸೂಚಕ) {
                T_UConverter_ಗಮ್ಯ_Unicode_UTF32_BE(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                T_UConverter_ಗಮ್ಯ_Unicode_UTF32_BE_ಸ್ಥಾನಾಂತರ_LOGIC(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
            }
            ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 9:
            /* call UTF-32LE */
            p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
            ಆದರೆ(offsets==ಶೂನ್ಯಸೂಚಕ) {
                T_UConverter_ಗಮ್ಯ_Unicode_UTF32_LE(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                T_UConverter_ಗಮ್ಯ_Unicode_UTF32_LE_ಸ್ಥಾನಾಂತರ_LOGIC(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
            }
            ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ನಿಲ್ಲಿಸು; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ && ಸ್ಥಾನಾಂತರ_Delta!=0) {
        int32_t *offsets_ಎಲ್ಲೆ=p_ವಾದಗಳು->offsets;
        ಆಗ(offsets<offsets_ಎಲ್ಲೆ) {
            *offsets++ += ಸ್ಥಾನಾಂತರ_Delta;
        }
    }

    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;

    ಆದರೆ(ಮೂಲ==ಮೂಲ_ಎಲ್ಲೆ && p_ವಾದಗಳು->flush) {
        /* handle truncated input */
        ಆಯ್ಕೆ(state) {
        ಸ್ಥಿತಿ 0:
            ನಿಲ್ಲಿಸು; /* no input at all, nothing to do */
        ಸ್ಥಿತಿ 8:
            T_UConverter_ಗಮ್ಯ_Unicode_UTF32_BE(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 9:
            T_UConverter_ಗಮ್ಯ_Unicode_UTF32_LE(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            p_ವಾದಗಳು->ಮೂಲ=utf32BOM+(state&4); /* select the correct BOM */
            p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_ಗಮ್ಯ_Unicode_UTF32_BE(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
            p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
            p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ=ಮೂಲ_ಎಲ್ಲೆ;
            state=8;
            ನಿಲ್ಲಿಸು;
        }
    }

    cnv->ಬಹುಲಕ=state;
}

ಸ್ಥಿರ UChar32 U_CALLCONV
UTF32_Get_ಮುಂದಿನ_UChar(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆಯ್ಕೆ(p_ವಾದಗಳು->converter->ಬಹುಲಕ) {
    ಸ್ಥಿತಿ 8:
        ಹಿಂತಿರುಗಿ T_UConverter_get_ಮುಂದಿನ_UChar_UTF32_BE(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
    ಸ್ಥಿತಿ 9:
        ಹಿಂತಿರುಗಿ T_UConverter_get_ಮುಂದಿನ_UChar_UTF32_LE(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ UCNV_GET_ಮುಂದಿನ_UCHAR_USE_ಗಮ್ಯ_U;
    }
}
U_CDECL_ಕೊನೆ
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    _UTF32Open,
    ಶೂನ್ಯಸೂಚಕ,
    _UTF32Reset,

    UTF32_ಗಮ್ಯ_Unicode_With_Offsets,
    UTF32_ಗಮ್ಯ_Unicode_With_Offsets,
#ಆದರೆ U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_ಸ್ಥಾನಾಂತರ_LOGIC,
#ಇಲ್ಲದಿದ್ದರೆ
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_ಸ್ಥಾನಾಂತರ_LOGIC,
#ಆದರೆ_ಅಂತ್ಯ
    UTF32_Get_ಮುಂದಿನ_UChar,

    ಶೂನ್ಯಸೂಚಕ, /* ### TODO implement getStarters for all Unicode encodings?! */
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_get_Non_Surrogate_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};

/* The 1236 CCSID refers to any version of Unicode with a BOM sensitive endianness of UTF-32 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ UTF32_Static_ದತ್ತಾಂಶ = {
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#ಆದರೆ U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#ಇಲ್ಲದಿದ್ದರೆ
    { 0xfd, 0xff, 0, 0 }, 4,
#ಆದರೆ_ಅಂತ್ಯ
    ಸುಳ್ಳು, ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ UTF32_ದತ್ತಾಂಶ = 
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&UTF32_Static_ದತ್ತಾಂಶ, &_UTF32Impl);

#ಆದರೆ_ಅಂತ್ಯ
