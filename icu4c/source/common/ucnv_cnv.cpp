// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2004, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
*   uconv_cnv.c:
*   Implements all the low level conversion functions
*   T_UnicodeConverter_{to,from}Unicode_$ConversionType
*
*   Change history:
*
*   06/29/2000  helena      Major rewrite of the callback APIs.
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv_err.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

U_CFUNC ಶೂನ್ಯ
ucnv_get_Complete_Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UConverter *cnv,
                   ಸ್ಥಿರಾಂಕ USetAdder *sa,
                   UConverter_Unicode_ಗಣ which,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    (ಶೂನ್ಯ)cnv;
    (ಶೂನ್ಯ)which;
    (ಶೂನ್ಯ)p_Error_ಸಂಕೇತ;
    sa->ಕೂಡಿಸು_Range(sa->ಗಣ, 0, 0x10ffff);
}

U_CFUNC ಶೂನ್ಯ
ucnv_get_Non_Surrogate_Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UConverter *cnv,
                               ಸ್ಥಿರಾಂಕ USetAdder *sa,
                               UConverter_Unicode_ಗಣ which,
                               UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    (ಶೂನ್ಯ)cnv;
    (ಶೂನ್ಯ)which;
    (ಶೂನ್ಯ)p_Error_ಸಂಕೇತ;
    sa->ಕೂಡಿಸು_Range(sa->ಗಣ, 0, 0xd7ff);
    sa->ಕೂಡಿಸು_Range(sa->ಗಣ, 0xe000, 0x10ffff);
}

U_CFUNC ಶೂನ್ಯ
ucnv_from_UWrite_ಬೈಟುಗಳು(UConverter *cnv,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು, int32_t length,
                     ಅಕ್ಷರ **ಗುರಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ_ಎಲ್ಲೆ,
                     int32_t **offsets,
                     int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ,
                     UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಅಕ್ಷರ *t=*ಗುರಿ;
    int32_t *o;

    /* write bytes */
    ಆದರೆ(offsets==ಶೂನ್ಯಸೂಚಕ || (o=*offsets)==ಶೂನ್ಯಸೂಚಕ) {
        ಆಗ(length>0 && t<ಗುರಿ_ಎಲ್ಲೆ) {
            *t++=*ಬೈಟುಗಳು++;
            --length;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* output with offsets */
        ಆಗ(length>0 && t<ಗುರಿ_ಎಲ್ಲೆ) {
            *t++=*ಬೈಟುಗಳು++;
            *o++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
            --length;
        }
        *offsets=o;
    }
    *ಗುರಿ=t;

    /* write overflow */
    ಆದರೆ(length>0) {
        ಆದರೆ(cnv!=ಶೂನ್ಯಸೂಚಕ) {
            t=(ಅಕ್ಷರ *)cnv->char_Error_ಬಫರ್;
            cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=(int8_t)length;
            ಮಾಡು {
                *t++=(uint8_t)*ಬೈಟುಗಳು++;
            } ಆಗ(--length>0);
        }
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }
}

U_CFUNC ಶೂನ್ಯ
ucnv_ಗಮ್ಯ_UWrite_UChars(UConverter *cnv,
                    ಸ್ಥಿರಾಂಕ char16_t *uchars, int32_t length,
                    char16_t **ಗುರಿ, ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ,
                    int32_t **offsets,
                    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ,
                    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    char16_t *t=*ಗುರಿ;
    int32_t *o;

    /* write UChars */
    ಆದರೆ(offsets==ಶೂನ್ಯಸೂಚಕ || (o=*offsets)==ಶೂನ್ಯಸೂಚಕ) {
        ಆಗ(length>0 && t<ಗುರಿ_ಎಲ್ಲೆ) {
            *t++=*uchars++;
            --length;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* output with offsets */
        ಆಗ(length>0 && t<ಗುರಿ_ಎಲ್ಲೆ) {
            *t++=*uchars++;
            *o++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
            --length;
        }
        *offsets=o;
    }
    *ಗುರಿ=t;

    /* write overflow */
    ಆದರೆ(length>0) {
        ಆದರೆ(cnv!=ಶೂನ್ಯಸೂಚಕ) {
            t=cnv->UChar_Error_ಬಫರ್;
            cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=(int8_t)length;
            ಮಾಡು {
                *t++=*uchars++;
            } ಆಗ(--length>0);
        }
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }
}

U_CFUNC ಶೂನ್ಯ
ucnv_ಗಮ್ಯ_UWrite_ಸಂಕೇತ_ಬಿಂದು(UConverter *cnv,
                       UChar32 c,
                       char16_t **ಗುರಿ, ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ,
                       int32_t **offsets,
                       int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ,
                       UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    char16_t *t;
    int32_t *o;

    t=*ಗುರಿ;

    ಆದರೆ(t<ಗುರಿ_ಎಲ್ಲೆ) {
        ಆದರೆ(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } ಇಲ್ಲದಿದ್ದರೆ /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            ಆದರೆ(t<ಗುರಿ_ಎಲ್ಲೆ) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ && (o=*offsets)!=ಶೂನ್ಯಸೂಚಕ) {
            *o++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಆದರೆ((*ಗುರಿ+1)<t) {
                *o++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
            }
            *offsets=o;
        }
    }

    *ಗುರಿ=t;

    /* write overflow from c */
    ಆದರೆ(c>=0) {
        ಆದರೆ(cnv!=ಶೂನ್ಯಸೂಚಕ) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UChar_Error_ಬಫರ್, i, c);
            cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=i;
        }
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }
}

#ಆದರೆ_ಅಂತ್ಯ
