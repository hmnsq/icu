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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

/* Prototypes --------------------------------------------------------------- */

/* Keep these here to make finicky compilers happy */

U_CFUNC ಶೂನ್ಯ ucnv_fromUnicode_UTF8(UConverter_From_Unicode_ವಾದಗಳು *ವಾದಗಳು_2,
                                           UError_ಸಂಕೇತ *err);
U_CFUNC ಶೂನ್ಯ ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverter_From_Unicode_ವಾದಗಳು *ವಾದಗಳು_2,
                                                        UError_ಸಂಕೇತ *err);


/* UTF-8 -------------------------------------------------------------------- */

#ವ್ಯಾಖ್ಯಾನಿಸು ಗರಿಷ್ಠ_UCS2            0x0000FFFF

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

ಸ್ಥಿರ UBool has_CESU8_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ UConverter *cnv)
{
#ಆದರೆ UCONFIG_ONLY_HTML_CONVERSION
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ cnv->shared_ದತ್ತಾಂಶ == &CESU8_ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ
}
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV ucnv_ಗಮ್ಯ_Unicode_UTF8 (UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು * ವಾದಗಳು_2,
                                  UError_ಸಂಕೇತ * err)
{
    UConverter *cnv = ವಾದಗಳು_2->converter;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *my_ಮೂಲ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಮೂಲ;
    char16_t *my_ಗುರಿ = ವಾದಗಳು_2->ಗುರಿ;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಮೂಲ_ಎಲ್ಲೆ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಗಮ್ಯ_UBytes = cnv->ಗಮ್ಯ_UBytes;
    UBool isCESU8 = has_CESU8_ದತ್ತಾಂಶ(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, ಒಳ_ಬೈಟುಗಳು;

    /* Restore size of current sequence */
    ಆದರೆ (cnv->ಗಮ್ಯ_ULength > 0 && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
    {
        ಒಳ_ಬೈಟುಗಳು = cnv->ಬಹುಲಕ;            /* restore # of bytes to consume */
        i = cnv->ಗಮ್ಯ_ULength;             /* restore # of bytes consumed */
        cnv->ಗಮ್ಯ_ULength = 0;

        ch = cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ;/*Stores the previously calculated ch from a previous call*/
        cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0;
        ಹೋಗು morebytes;
    }


    ಆಗ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
    {
        ch = *(my_ಮೂಲ++);
        ಆದರೆ (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(my_ಗುರಿ++) = (char16_t) ch;
        }
        ಇಲ್ಲದಿದ್ದರೆ
        {
            /* store the first char */
            ಗಮ್ಯ_UBytes[0] = (ಅಕ್ಷರ)ch;
            ಒಳ_ಬೈಟುಗಳು = U8_ಎಣಿಕೆ_ಬೈಟುಗಳು_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            ಆಗ (i < ಒಳ_ಬೈಟುಗಳು)
            {
                ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ)
                {
                    ಗಮ್ಯ_UBytes[i] = (ಅಕ್ಷರ) (ch2 = *my_ಮೂಲ);
                    ಆದರೆ (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, ಒಳ_ಬೈಟುಗಳು) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        ನಿಲ್ಲಿಸು; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++my_ಮೂಲ;
                    i++;
                }
                ಇಲ್ಲದಿದ್ದರೆ
                {
                    /* stores a partially calculated target*/
                    cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = ch;
                    cnv->ಬಹುಲಕ = ಒಳ_ಬೈಟುಗಳು;
                    cnv->ಗಮ್ಯ_ULength = (int8_t) i;
                    ಹೋಗು donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            ಆದರೆ (i == ಒಳ_ಬೈಟುಗಳು && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[ಒಳ_ಬೈಟುಗಳು];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                ಆದರೆ (ch <= ಗರಿಷ್ಠ_UCS2) 
                {
                    /* fits in 16 bits */
                    *(my_ಗುರಿ++) = (char16_t) ch;
                }
                ಇಲ್ಲದಿದ್ದರೆ
                {
                    /* write out the surrogates */
                    *(my_ಗುರಿ++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    ಆದರೆ (my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
                    {
                        *(my_ಗುರಿ++) = (char16_t)ch;
                    }
                    ಇಲ್ಲದಿದ್ದರೆ
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UChar_Error_ಬಫರ್[0] = (char16_t) ch;
                        cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ = 1;
                        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                        ನಿಲ್ಲಿಸು;
                    }
                }
            }
            ಇಲ್ಲದಿದ್ದರೆ
            {
                cnv->ಗಮ್ಯ_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ನಿಲ್ಲಿಸು;
            }
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

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV ucnv_ಗಮ್ಯ_Unicode_UTF8_OFFSETS_LOGIC (UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು * ವಾದಗಳು_2,
                                                UError_ಸಂಕೇತ * err)
{
    UConverter *cnv = ವಾದಗಳು_2->converter;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *my_ಮೂಲ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಮೂಲ;
    char16_t *my_ಗುರಿ = ವಾದಗಳು_2->ಗುರಿ;
    int32_t *myOffsets = ವಾದಗಳು_2->offsets;
    int32_t ಸ್ಥಾನಾಂತರ_ಅಂಕೆ = 0;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಮೂಲ_ಎಲ್ಲೆ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಗಮ್ಯ_UBytes = cnv->ಗಮ್ಯ_UBytes;
    UBool isCESU8 = has_CESU8_ದತ್ತಾಂಶ(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, ಒಳ_ಬೈಟುಗಳು;

    /* Restore size of current sequence */
    ಆದರೆ (cnv->ಗಮ್ಯ_ULength > 0 && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
    {
        ಒಳ_ಬೈಟುಗಳು = cnv->ಬಹುಲಕ;            /* restore # of bytes to consume */
        i = cnv->ಗಮ್ಯ_ULength;             /* restore # of bytes consumed */
        cnv->ಗಮ್ಯ_ULength = 0;

        ch = cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ;/*Stores the previously calculated ch from a previous call*/
        cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0;
        ಹೋಗು morebytes;
    }

    ಆಗ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
    {
        ch = *(my_ಮೂಲ++);
        ಆದರೆ (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(my_ಗುರಿ++) = (char16_t) ch;
            *(myOffsets++) = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ++;
        }
        ಇಲ್ಲದಿದ್ದರೆ
        {
            ಗಮ್ಯ_UBytes[0] = (ಅಕ್ಷರ)ch;
            ಒಳ_ಬೈಟುಗಳು = U8_ಎಣಿಕೆ_ಬೈಟುಗಳು_NON_ASCII(ch);
            i = 1;

morebytes:
            ಆಗ (i < ಒಳ_ಬೈಟುಗಳು)
            {
                ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ)
                {
                    ಗಮ್ಯ_UBytes[i] = (ಅಕ್ಷರ) (ch2 = *my_ಮೂಲ);
                    ಆದರೆ (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, ಒಳ_ಬೈಟುಗಳು) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        ನಿಲ್ಲಿಸು; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++my_ಮೂಲ;
                    i++;
                }
                ಇಲ್ಲದಿದ್ದರೆ
                {
                    cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = ch;
                    cnv->ಬಹುಲಕ = ಒಳ_ಬೈಟುಗಳು;
                    cnv->ಗಮ್ಯ_ULength = (int8_t)i;
                    ಹೋಗು donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            ಆದರೆ (i == ಒಳ_ಬೈಟುಗಳು && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[ಒಳ_ಬೈಟುಗಳು];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                ಆದರೆ (ch <= ಗರಿಷ್ಠ_UCS2) 
                {
                    /* fits in 16 bits */
                    *(my_ಗುರಿ++) = (char16_t) ch;
                    *(myOffsets++) = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ;
                }
                ಇಲ್ಲದಿದ್ದರೆ
                {
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
                        cnv->UChar_Error_ಬಫರ್[0] = (char16_t) ch;
                        cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ = 1;
                        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    }
                }
                ಸ್ಥಾನಾಂತರ_ಅಂಕೆ += i;
            }
            ಇಲ್ಲದಿದ್ದರೆ
            {
                cnv->ಗಮ್ಯ_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ನಿಲ್ಲಿಸು;
            }
        }
    }

donefornow:
    ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ >= ಗುರಿ_ಎಲ್ಲೆ && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
    ವಾದಗಳು_2->ಮೂಲ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) my_ಮೂಲ;
    ವಾದಗಳು_2->offsets = myOffsets;
}
U_CDECL_ಕೊನೆ

U_CFUNC ಶೂನ್ಯ  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverter_From_Unicode_ವಾದಗಳು * ವಾದಗಳು_2,
                                    UError_ಸಂಕೇತ * err)
{
    UConverter *cnv = ವಾದಗಳು_2->converter;
    ಸ್ಥಿರಾಂಕ char16_t *my_ಮೂಲ = ವಾದಗಳು_2->ಮೂಲ;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *my_ಗುರಿ = (uint8_t *) ವಾದಗಳು_2->ಗುರಿ;
    ಸ್ಥಿರಾಂಕ uint8_t *ಗುರಿ_ಎಲ್ಲೆ = (uint8_t *) ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ;
    uint8_t *ತಾತ್ಕಾಲಿಕ_Ptr;
    UChar32 ch;
    uint8_t ತಾತ್ಕಾಲಿಕ_Buf[4];
    int32_t ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write;
    UBool isNotCESU8 = !has_CESU8_ದತ್ತಾಂಶ(cnv);

    ಆದರೆ (cnv->fromUChar32 && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        ಹೋಗು lowsurrogate;
    }

    ಆಗ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
    {
        ch = *(my_ಮೂಲ++);

        ಆದರೆ (ch < 0x80)        /* Single byte */
        {
            *(my_ಗುರಿ++) = (uint8_t) ch;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ch < 0x800)  /* Double byte */
        {
            *(my_ಗುರಿ++) = (uint8_t) ((ch >> 6) | 0xc0);
            ಆದರೆ (my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
            {
                *(my_ಗುರಿ++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            ಇಲ್ಲದಿದ್ದರೆ
            {
                cnv->char_Error_ಬಫರ್[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ = 1;
                *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            /* Check for surrogates */
            ಆದರೆ(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ) {
                    /* test both code units */
                    ಆದರೆ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*my_ಮೂಲ)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *my_ಮೂಲ);
                        ++my_ಮೂಲ;
                        /* exit this condition tree */
                    }
                    ಇಲ್ಲದಿದ್ದರೆ {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ನಿಲ್ಲಿಸು;
                    }
                }
                ಇಲ್ಲದಿದ್ದರೆ {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    ನಿಲ್ಲಿಸು;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            ತಾತ್ಕಾಲಿಕ_Ptr = (((ಗುರಿ_ಎಲ್ಲೆ - my_ಗುರಿ) >= 4) ? my_ಗುರಿ : ತಾತ್ಕಾಲಿಕ_Buf);

            ಆದರೆ (ch <= ಗರಿಷ್ಠ_UCS2) {
                ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write = 2;
                ತಾತ್ಕಾಲಿಕ_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write = 3;
                ತಾತ್ಕಾಲಿಕ_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                ತಾತ್ಕಾಲಿಕ_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            ತಾತ್ಕಾಲಿಕ_Ptr[ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            ತಾತ್ಕಾಲಿಕ_Ptr[ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            ಆದರೆ (ತಾತ್ಕಾಲಿಕ_Ptr == my_ಗುರಿ) {
                /* There was enough space to write the codepoint directly. */
                my_ಗುರಿ += (ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write + 1);
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                /* We might run out of room soon. Write it slowly. */
                ಗಾಗಿ (; ತಾತ್ಕಾಲಿಕ_Ptr <= (ತಾತ್ಕಾಲಿಕ_Buf + ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write); ತಾತ್ಕಾಲಿಕ_Ptr++) {
                    ಆದರೆ (my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ) {
                        *(my_ಗುರಿ++) = *ತಾತ್ಕಾಲಿಕ_Ptr;
                    }
                    ಇಲ್ಲದಿದ್ದರೆ {
                        cnv->char_Error_ಬಫರ್[cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] = *ತಾತ್ಕಾಲಿಕ_Ptr;
                        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    }
                }
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

U_CFUNC ಶೂನ್ಯ  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverter_From_Unicode_ವಾದಗಳು * ವಾದಗಳು_2,
                                                  UError_ಸಂಕೇತ * err)
{
    UConverter *cnv = ವಾದಗಳು_2->converter;
    ಸ್ಥಿರಾಂಕ char16_t *my_ಮೂಲ = ವಾದಗಳು_2->ಮೂಲ;
    int32_t *myOffsets = ವಾದಗಳು_2->offsets;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *my_ಗುರಿ = (uint8_t *) ವಾದಗಳು_2->ಗುರಿ;
    ಸ್ಥಿರಾಂಕ uint8_t *ಗುರಿ_ಎಲ್ಲೆ = (uint8_t *) ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ;
    uint8_t *ತಾತ್ಕಾಲಿಕ_Ptr;
    UChar32 ch;
    int32_t ಸ್ಥಾನಾಂತರ_ಅಂಕೆ, ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
    int32_t ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write;
    uint8_t ತಾತ್ಕಾಲಿಕ_Buf[4];
    UBool isNotCESU8 = !has_CESU8_ದತ್ತಾಂಶ(cnv);

    ಆದರೆ (cnv->fromUChar32 && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        ಸ್ಥಾನಾಂತರ_ಅಂಕೆ = -1;
        ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ = 0;
        ಹೋಗು lowsurrogate;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಸ್ಥಾನಾಂತರ_ಅಂಕೆ = 0;
    }

    ಆಗ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
    {
        ch = *(my_ಮೂಲ++);

        ಆದರೆ (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ++;
            *(my_ಗುರಿ++) = (ಅಕ್ಷರ) ch;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ;
            *(my_ಗುರಿ++) = (uint8_t) ((ch >> 6) | 0xc0);
            ಆದರೆ (my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
            {
                *(myOffsets++) = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ++;
                *(my_ಗುರಿ++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            ಇಲ್ಲದಿದ್ದರೆ
            {
                cnv->char_Error_ಬಫರ್[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ = 1;
                *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ
        /* Check for surrogates */
        {
            ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ + 1;

            ಆದರೆ(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ) {
                    /* test both code units */
                    ಆದರೆ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*my_ಮೂಲ)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *my_ಮೂಲ);
                        ++my_ಮೂಲ;
                        ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        /* exit this condition tree */
                    }
                    ಇಲ್ಲದಿದ್ದರೆ {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ನಿಲ್ಲಿಸು;
                    }
                }
                ಇಲ್ಲದಿದ್ದರೆ {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    ನಿಲ್ಲಿಸು;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            ತಾತ್ಕಾಲಿಕ_Ptr = (((ಗುರಿ_ಎಲ್ಲೆ - my_ಗುರಿ) >= 4) ? my_ಗುರಿ : ತಾತ್ಕಾಲಿಕ_Buf);

            ಆದರೆ (ch <= ಗರಿಷ್ಠ_UCS2) {
                ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write = 2;
                ತಾತ್ಕಾಲಿಕ_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write = 3;
                ತಾತ್ಕಾಲಿಕ_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                ತಾತ್ಕಾಲಿಕ_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            ತಾತ್ಕಾಲಿಕ_Ptr[ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            ತಾತ್ಕಾಲಿಕ_Ptr[ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            ಆದರೆ (ತಾತ್ಕಾಲಿಕ_Ptr == my_ಗುರಿ) {
                /* There was enough space to write the codepoint directly. */
                my_ಗುರಿ += (ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write + 1);
                myOffsets[0] = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ;
                myOffsets[1] = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ;
                myOffsets[2] = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ;
                ಆದರೆ (ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write >= 3) {
                    myOffsets[3] = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ;
                }
                myOffsets += (ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write + 1);
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                /* We might run out of room soon. Write it slowly. */
                ಗಾಗಿ (; ತಾತ್ಕಾಲಿಕ_Ptr <= (ತಾತ್ಕಾಲಿಕ_Buf + ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_Write); ತಾತ್ಕಾಲಿಕ_Ptr++) {
                    ಆದರೆ (my_ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ)
                    {
                        *(myOffsets++) = ಸ್ಥಾನಾಂತರ_ಅಂಕೆ;
                        *(my_ಗುರಿ++) = *ತಾತ್ಕಾಲಿಕ_Ptr;
                    }
                    ಇಲ್ಲದಿದ್ದರೆ
                    {
                        cnv->char_Error_ಬಫರ್[cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] = *ತಾತ್ಕಾಲಿಕ_Ptr;
                        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    }
                }
            }
            ಸ್ಥಾನಾಂತರ_ಅಂಕೆ = ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
        }
    }

    ಆದರೆ (my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ >= ಗುರಿ_ಎಲ್ಲೆ && U_SUCCESS(*err))
    {
        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    ವಾದಗಳು_2->ಗುರಿ = (ಅಕ್ಷರ *) my_ಗುರಿ;
    ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
    ವಾದಗಳು_2->offsets = myOffsets;
}

U_CDECL_BEGIN
ಸ್ಥಿರ UChar32 U_CALLCONV ucnv_get_ಮುಂದಿನ_UChar_UTF8(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *ವಾದಗಳು_2,
                                               UError_ಸಂಕೇತ *err) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ_Initial;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = ವಾದಗಳು_2->converter;
    ಮೂಲ_Initial = ಮೂಲ = (ಸ್ಥಿರಾಂಕ uint8_t *)ವಾದಗಳು_2->ಮೂಲ;
    ಆದರೆ (ಮೂಲ >= (ಸ್ಥಿರಾಂಕ uint8_t *)ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ)
    {
        /* no input */
        *err = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    myByte = *(ಮೂಲ++);
    ಆದರೆ (U8_IS_SINGLE(myByte))
    {
        ವಾದಗಳು_2->ಮೂಲ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
        ಹಿಂತಿರುಗಿ (UChar32)myByte;
    }

    uint16_t ಎಣಿಕೆ_Trail_ಬೈಟುಗಳು = U8_ಎಣಿಕೆ_TRAIL_ಬೈಟುಗಳು(myByte);
    ಆದರೆ (ಎಣಿಕೆ_Trail_ಬೈಟುಗಳು == 0) {
        cnv->ಗಮ್ಯ_UBytes[0] = myByte;
        cnv->ಗಮ್ಯ_ULength = 1;
        *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
        ವಾದಗಳು_2->ಮೂಲ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    ಆದರೆ (((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ + ಎಣಿಕೆ_Trail_ಬೈಟುಗಳು) > ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extra_ಬೈಟುಗಳು_ಗಮ್ಯ_Write = ಎಣಿಕೆ_Trail_ಬೈಟುಗಳು + 1;
        cnv->ಗಮ್ಯ_UBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು;
        ಆಗ(ಮೂಲ < (ಸ್ಥಿರಾಂಕ uint8_t *)ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ) {
            uint8_t b = *ಮೂಲ;
            ಆದರೆ(icu::UTF8::isValidTrail(myByte, b, i, extra_ಬೈಟುಗಳು_ಗಮ್ಯ_Write)) {
                cnv->ಗಮ್ಯ_UBytes[i++] = b;
                ++ಮೂಲ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ನಿಲ್ಲಿಸು;
            }
        }
        cnv->ಗಮ್ಯ_ULength = i;
        ವಾದಗಳು_2->ಮೂಲ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    ch = myByte << 6;
    ಆದರೆ(ಎಣಿಕೆ_Trail_ಬೈಟುಗಳು == 2) {
        uint8_t t1 = *ಮೂಲ, t2;
        ಆದರೆ(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++ಮೂಲ)) {
            ವಾದಗಳು_2->ಮೂಲ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(ಮೂಲ + 1);
            ಹಿಂತಿರುಗಿ (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಎಣಿಕೆ_Trail_ಬೈಟುಗಳು == 1) {
        uint8_t t1 = *ಮೂಲ;
        ಆದರೆ(U8_IS_TRAIL(t1)) {
            ವಾದಗಳು_2->ಮೂಲ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(ಮೂಲ + 1);
            ಹಿಂತಿರುಗಿ (ch + t1) - offsetsFromUTF8[2];
        }
    } ಇಲ್ಲದಿದ್ದರೆ {  // countTrailBytes == 3
        uint8_t t1 = *ಮೂಲ, t2, t3;
        ಆದರೆ(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++ಮೂಲ) &&
                U8_IS_TRAIL(t3 = *++ಮೂಲ)) {
            ವಾದಗಳು_2->ಮೂಲ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(ಮೂಲ + 1);
            ಹಿಂತಿರುಗಿ (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    ವಾದಗಳು_2->ಮೂಲ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;

    ಗಾಗಿ(i = 0; ಮೂಲ_Initial < ಮೂಲ; ++i) {
        cnv->ಗಮ್ಯ_UBytes[i] = *ಮೂಲ_Initial++;
    }
    cnv->ಗಮ್ಯ_ULength = i;
    *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
    ಹಿಂತಿರುಗಿ 0xffff;
} 
U_CDECL_ಕೊನೆ

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_UTF8FromUTF8(UConverter_From_Unicode_ವಾದಗಳು *pFromUArgs,
                  UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ಗಮ್ಯ_UArgs,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *utf8;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
    int32_t count;

    int8_t old_ಗಮ್ಯ_ULength, ಗಮ್ಯ_ULength, ಗಮ್ಯ_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    /* set up the local pointers */
    utf8=p_ಗಮ್ಯ_UArgs->converter;
    ಮೂಲ=(uint8_t *)p_ಗಮ್ಯ_UArgs->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=(uint8_t *)p_ಗಮ್ಯ_UArgs->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ=(uint8_t *)pFromUArgs->ಗುರಿ;
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(int32_t)(pFromUArgs->ಗುರಿ_ಎಲ್ಲೆ-pFromUArgs->ಗುರಿ);

    /* get the converter state from the UTF-8 UConverter */
    ಆದರೆ(utf8->ಗಮ್ಯ_ULength > 0) {
        ಗಮ್ಯ_ULength=old_ಗಮ್ಯ_ULength=utf8->ಗಮ್ಯ_ULength;
        ಗಮ್ಯ_ULimit=(int8_t)utf8->ಬಹುಲಕ;
        c=(UChar32)utf8->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಗಮ್ಯ_ULength=old_ಗಮ್ಯ_ULength=ಗಮ್ಯ_ULimit=0;
        c = 0;
    }

    count=(int32_t)(ಮೂಲ_ಎಲ್ಲೆ-ಮೂಲ)+old_ಗಮ್ಯ_ULength;
    ಆದರೆ(count<ಗಮ್ಯ_ULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ<ಗಮ್ಯ_ULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *p_Error_ಸಂಕೇತ=U_USING_DEFAULT_WARNING;
        ಹಿಂತಿರುಗಿ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        ಆದರೆ(count>ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
            count=ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
        }

        // The conversion loop checks count>0 only once per character.
        // If the buffer ends with a truncated sequence,
        // then we reduce the count to stop before that,
        // and collect the remaining bytes after the conversion loop.

        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length=count-ಗಮ್ಯ_ULength;
        U8_TRUNCATE_IF_INCOMPLETE(ಮೂಲ, 0, length);
        count=ಗಮ್ಯ_ULength+length;
    }

    ಆದರೆ(c!=0) {
        utf8->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=0;
        utf8->ಗಮ್ಯ_ULength=0;
        ಹೋಗು ಇನ್ನಷ್ಟು_ಬೈಟುಗಳು;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    ಆಗ(count>0) {
        b=*ಮೂಲ++;
        ಆದರೆ(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *ಗುರಿ++=b;
            --count;
            ಮುಂದುವರಿ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(b>=0xe0) {
                ಆದರೆ( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=ಮೂಲ[0]) &&
                    U8_IS_TRAIL(t2=ಮೂಲ[1])
                ) {
                    ಮೂಲ+=2;
                    *ಗುರಿ++=b;
                    *ಗುರಿ++=t1;
                    *ಗುರಿ++=t2;
                    count-=3;
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*ಮೂಲ)
                ) {
                    ++ಮೂಲ;
                    *ಗುರಿ++=b;
                    *ಗುರಿ++=t1;
                    count-=2;
                    ಮುಂದುವರಿ;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            old_ಗಮ್ಯ_ULength=0;
            ಗಮ್ಯ_ULength=1;
            ಗಮ್ಯ_ULimit=U8_ಎಣಿಕೆ_ಬೈಟುಗಳು_NON_ASCII(b);
            c=b;
ಇನ್ನಷ್ಟು_ಬೈಟುಗಳು:
            ಆಗ(ಗಮ್ಯ_ULength<ಗಮ್ಯ_ULimit) {
                ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
                    b=*ಮೂಲ;
                    ಆದರೆ(icu::UTF8::isValidTrail(c, b, ಗಮ್ಯ_ULength, ಗಮ್ಯ_ULimit)) {
                        ++ಮೂಲ;
                        ++ಗಮ್ಯ_ULength;
                        c=(c<<6)+b;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ನಿಲ್ಲಿಸು; /* sequence too short, stop with toULength<toULimit */
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    ಮೂಲ-=(ಗಮ್ಯ_ULength-old_ಗಮ್ಯ_ULength);
                    ಆಗ(old_ಗಮ್ಯ_ULength<ಗಮ್ಯ_ULength) {
                        utf8->ಗಮ್ಯ_UBytes[old_ಗಮ್ಯ_ULength++]=*ಮೂಲ++;
                    }
                    utf8->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=c;
                    utf8->ಗಮ್ಯ_ULength=ಗಮ್ಯ_ULength;
                    utf8->ಬಹುಲಕ=ಗಮ್ಯ_ULimit;
                    p_ಗಮ್ಯ_UArgs->ಮೂಲ=(ಅಕ್ಷರ *)ಮೂಲ;
                    pFromUArgs->ಗುರಿ=(ಅಕ್ಷರ *)ಗುರಿ;
                    ಹಿಂತಿರುಗಿ;
                }
            }

            ಆದರೆ(ಗಮ್ಯ_ULength!=ಗಮ್ಯ_ULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                ಮೂಲ-=(ಗಮ್ಯ_ULength-old_ಗಮ್ಯ_ULength);
                ಆಗ(old_ಗಮ್ಯ_ULength<ಗಮ್ಯ_ULength) {
                    utf8->ಗಮ್ಯ_UBytes[old_ಗಮ್ಯ_ULength++]=*ಮೂಲ++;
                }
                utf8->ಗಮ್ಯ_ULength=ಗಮ್ಯ_ULength;
                p_ಗಮ್ಯ_UArgs->ಮೂಲ=(ಅಕ್ಷರ *)ಮೂಲ;
                pFromUArgs->ಗುರಿ=(ಅಕ್ಷರ *)ಗುರಿ;
                *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ಹಿಂತಿರುಗಿ;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                ಗಾಗಿ(i=0; i<old_ಗಮ್ಯ_ULength; ++i) {
                    *ಗುರಿ++=utf8->ಗಮ್ಯ_UBytes[i];
                }
                ಮೂಲ-=(ಗಮ್ಯ_ULength-old_ಗಮ್ಯ_ULength);
                ಗಾಗಿ(; i<ಗಮ್ಯ_ULength; ++i) {
                    *ಗುರಿ++=*ಮೂಲ++;
                }
                count-=ಗಮ್ಯ_ULength;
            }
        }
    }
    U_ASSERT(count>=0);

    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ) && ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
        ಆದರೆ(ಗುರಿ==(ಸ್ಥಿರಾಂಕ uint8_t *)pFromUArgs->ಗುರಿ_ಎಲ್ಲೆ) {
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            b=*ಮೂಲ;
            ಗಮ್ಯ_ULimit=U8_ಎಣಿಕೆ_ಬೈಟುಗಳು(b);
            ಆದರೆ(ಗಮ್ಯ_ULimit>(ಮೂಲ_ಎಲ್ಲೆ-ಮೂಲ)) {
                /* collect a truncated byte sequence */
                ಗಮ್ಯ_ULength=0;
                c=b;
                ಗಾಗಿ(;;) {
                    utf8->ಗಮ್ಯ_UBytes[ಗಮ್ಯ_ULength++]=b;
                    ಆದರೆ(++ಮೂಲ==ಮೂಲ_ಎಲ್ಲೆ) {
                        /* partial byte sequence at end of source */
                        utf8->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=c;
                        utf8->ಗಮ್ಯ_ULength=ಗಮ್ಯ_ULength;
                        utf8->ಬಹುಲಕ=ಗಮ್ಯ_ULimit;
                        ನಿಲ್ಲಿಸು;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!icu::UTF8::isValidTrail(c, b=*ಮೂಲ, ಗಮ್ಯ_ULength, ಗಮ್ಯ_ULimit)) {
                        utf8->ಗಮ್ಯ_ULength=ಗಮ್ಯ_ULength;
                        *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ನಿಲ್ಲಿಸು;
                    }
                    c=(c<<6)+b;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *p_Error_ಸಂಕೇತ=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    p_ಗಮ್ಯ_UArgs->ಮೂಲ=(ಅಕ್ಷರ *)ಮೂಲ;
    pFromUArgs->ಗುರಿ=(ಅಕ್ಷರ *)ಗುರಿ;
}

U_CDECL_ಕೊನೆ

/* UTF-8 converter data ----------------------------------------------------- */

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _UTF8Impl={
    UCNV_UTF8,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    ucnv_ಗಮ್ಯ_Unicode_UTF8,
    ucnv_ಗಮ್ಯ_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    ucnv_get_ಮುಂದಿನ_UChar_UTF8,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_get_Non_Surrogate_Unicode_ಗಣ,

    ucnv_UTF8FromUTF8,
    ucnv_UTF8FromUTF8
};

/* The 1208 CCSID refers to any version of Unicode of UTF-8 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ UTF8_Static_ದತ್ತಾಂಶ={
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,ಸುಳ್ಳು,ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ UTF8_ದತ್ತಾಂಶ=
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&UTF8_Static_ದತ್ತಾಂಶ, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _CESU8Impl={
    UCNV_CESU8,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    ucnv_ಗಮ್ಯ_Unicode_UTF8,
    ucnv_ಗಮ್ಯ_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_get_Complete_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ CESU8_Static_ದತ್ತಾಂಶ={
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,ಸುಳ್ಳು,ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ CESU8_ದತ್ತಾಂಶ=
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&CESU8_Static_ದತ್ತಾಂಶ, &_CESU8Impl);

#ಆದರೆ_ಅಂತ್ಯ
