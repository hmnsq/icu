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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION

#شامل "unicode/ucnv_err.h"
#شامل "unicode/ucnv.h"
#شامل "unicode/uset.h"
#شامل "ucnv_cnv.h"
#شامل "ucnv_bld.h"
#شامل "cmemory.h"

U_CFUNC خالی
ucnv_get_Complete_Unicode_سیٹ(ثابت UConverter *cnv,
                   ثابت USetAdder *sa,
                   UConverter_Unicode_سیٹ which,
                   UError_رمز *p_Error_رمز) {
    (خالی)cnv;
    (خالی)which;
    (خالی)p_Error_رمز;
    sa->جوڑو_Range(sa->سیٹ, 0, 0x10ffff);
}

U_CFUNC خالی
ucnv_get_Non_Surrogate_Unicode_سیٹ(ثابت UConverter *cnv,
                               ثابت USetAdder *sa,
                               UConverter_Unicode_سیٹ which,
                               UError_رمز *p_Error_رمز) {
    (خالی)cnv;
    (خالی)which;
    (خالی)p_Error_رمز;
    sa->جوڑو_Range(sa->سیٹ, 0, 0xd7ff);
    sa->جوڑو_Range(sa->سیٹ, 0xe000, 0x10ffff);
}

U_CFUNC خالی
ucnv_from_UWrite_بائٹیں(UConverter *cnv,
                     ثابت حرف *بائٹیں, int32_t length,
                     حرف **ہدف, ثابت حرف *ہدف_حد_بندی,
                     int32_t **offsets,
                     int32_t ماخذ_اشاریہ,
                     UError_رمز *p_Error_رمز) {
    حرف *t=*ہدف;
    int32_t *o;

    /* write bytes */
    اگر(offsets==خالی_اشاریہ || (o=*offsets)==خالی_اشاریہ) {
        جب(length>0 && t<ہدف_حد_بندی) {
            *t++=*بائٹیں++;
            --length;
        }
    } ورنہ {
        /* output with offsets */
        جب(length>0 && t<ہدف_حد_بندی) {
            *t++=*بائٹیں++;
            *o++=ماخذ_اشاریہ;
            --length;
        }
        *offsets=o;
    }
    *ہدف=t;

    /* write overflow */
    اگر(length>0) {
        اگر(cnv!=خالی_اشاریہ) {
            t=(حرف *)cnv->char_Error_بفر;
            cnv->char_Error_بفر_لمبائی=(int8_t)length;
            کرو {
                *t++=(uint8_t)*بائٹیں++;
            } جب(--length>0);
        }
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
    }
}

U_CFUNC خالی
ucnv_مقصد_UWrite_UChars(UConverter *cnv,
                    ثابت char16_t *uchars, int32_t length,
                    char16_t **ہدف, ثابت char16_t *ہدف_حد_بندی,
                    int32_t **offsets,
                    int32_t ماخذ_اشاریہ,
                    UError_رمز *p_Error_رمز) {
    char16_t *t=*ہدف;
    int32_t *o;

    /* write UChars */
    اگر(offsets==خالی_اشاریہ || (o=*offsets)==خالی_اشاریہ) {
        جب(length>0 && t<ہدف_حد_بندی) {
            *t++=*uchars++;
            --length;
        }
    } ورنہ {
        /* output with offsets */
        جب(length>0 && t<ہدف_حد_بندی) {
            *t++=*uchars++;
            *o++=ماخذ_اشاریہ;
            --length;
        }
        *offsets=o;
    }
    *ہدف=t;

    /* write overflow */
    اگر(length>0) {
        اگر(cnv!=خالی_اشاریہ) {
            t=cnv->UChar_Error_بفر;
            cnv->UChar_Error_بفر_لمبائی=(int8_t)length;
            کرو {
                *t++=*uchars++;
            } جب(--length>0);
        }
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
    }
}

U_CFUNC خالی
ucnv_مقصد_UWrite_رمز_نقطہ(UConverter *cnv,
                       UChar32 c,
                       char16_t **ہدف, ثابت char16_t *ہدف_حد_بندی,
                       int32_t **offsets,
                       int32_t ماخذ_اشاریہ,
                       UError_رمز *p_Error_رمز) {
    char16_t *t;
    int32_t *o;

    t=*ہدف;

    اگر(t<ہدف_حد_بندی) {
        اگر(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } ورنہ /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            اگر(t<ہدف_حد_بندی) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        اگر(offsets!=خالی_اشاریہ && (o=*offsets)!=خالی_اشاریہ) {
            *o++=ماخذ_اشاریہ;
            اگر((*ہدف+1)<t) {
                *o++=ماخذ_اشاریہ;
            }
            *offsets=o;
        }
    }

    *ہدف=t;

    /* write overflow from c */
    اگر(c>=0) {
        اگر(cnv!=خالی_اشاریہ) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UChar_Error_بفر, i, c);
            cnv->UChar_Error_بفر_لمبائی=i;
        }
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
    }
}

#اگر_ختم
