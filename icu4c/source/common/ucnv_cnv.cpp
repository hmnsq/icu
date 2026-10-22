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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن "unicode/ucnv_err.h"
#ضمن "unicode/ucnv.h"
#ضمن "unicode/uset.h"
#ضمن "ucnv_cnv.h"
#ضمن "ucnv_bld.h"
#ضمن "cmemory.h"

U_CFUNC فراغ
ucnv_get_Complete_Unicode_سیٹ(ثابت UConverter *cnv,
                   ثابت USetAdder *sa,
                   UConverter_Unicode_سیٹ which,
                   UError_کوڈ *p_Error_کوڈ) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_کوڈ;
    sa->جوڑو_Range(sa->سیٹ, 0, 0x10ffff);
}

U_CFUNC فراغ
ucnv_get_Non_Surrogate_Unicode_سیٹ(ثابت UConverter *cnv,
                               ثابت USetAdder *sa,
                               UConverter_Unicode_سیٹ which,
                               UError_کوڈ *p_Error_کوڈ) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_کوڈ;
    sa->جوڑو_Range(sa->سیٹ, 0, 0xd7ff);
    sa->جوڑو_Range(sa->سیٹ, 0xe000, 0x10ffff);
}

U_CFUNC فراغ
ucnv_from_UWrite_بائٹاں(UConverter *cnv,
                     ثابت حرف *بائٹاں, int32_t length,
                     حرف **نشانہ, ثابت حرف *نشانہ_حد_بندی,
                     int32_t **offsets,
                     int32_t سروت_اشاریہ,
                     UError_کوڈ *p_Error_کوڈ) {
    حرف *t=*نشانہ;
    int32_t *o;

    /* write bytes */
    إذا(offsets==خالی_سوچک || (o=*offsets)==خالی_سوچک) {
        طالما(length>0 && t<نشانہ_حد_بندی) {
            *t++=*بائٹاں++;
            --length;
        }
    } وإلا {
        /* output with offsets */
        طالما(length>0 && t<نشانہ_حد_بندی) {
            *t++=*بائٹاں++;
            *o++=سروت_اشاریہ;
            --length;
        }
        *offsets=o;
    }
    *نشانہ=t;

    /* write overflow */
    إذا(length>0) {
        إذا(cnv!=خالی_سوچک) {
            t=(حرف *)cnv->char_Error_بفر;
            cnv->char_Error_بفر_لمائی=(int8_t)length;
            افعل {
                *t++=(uint8_t)*بائٹاں++;
            } طالما(--length>0);
        }
        *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
    }
}

U_CFUNC فراغ
ucnv_منزل_UWrite_UChars(UConverter *cnv,
                    ثابت char16_t *uchars, int32_t length,
                    char16_t **نشانہ, ثابت char16_t *نشانہ_حد_بندی,
                    int32_t **offsets,
                    int32_t سروت_اشاریہ,
                    UError_کوڈ *p_Error_کوڈ) {
    char16_t *t=*نشانہ;
    int32_t *o;

    /* write UChars */
    إذا(offsets==خالی_سوچک || (o=*offsets)==خالی_سوچک) {
        طالما(length>0 && t<نشانہ_حد_بندی) {
            *t++=*uchars++;
            --length;
        }
    } وإلا {
        /* output with offsets */
        طالما(length>0 && t<نشانہ_حد_بندی) {
            *t++=*uchars++;
            *o++=سروت_اشاریہ;
            --length;
        }
        *offsets=o;
    }
    *نشانہ=t;

    /* write overflow */
    إذا(length>0) {
        إذا(cnv!=خالی_سوچک) {
            t=cnv->UChar_Error_بفر;
            cnv->UChar_Error_بفر_لمائی=(int8_t)length;
            افعل {
                *t++=*uchars++;
            } طالما(--length>0);
        }
        *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
    }
}

U_CFUNC فراغ
ucnv_منزل_UWrite_کوڈ_نقطہ(UConverter *cnv,
                       UChar32 c,
                       char16_t **نشانہ, ثابت char16_t *نشانہ_حد_بندی,
                       int32_t **offsets,
                       int32_t سروت_اشاریہ,
                       UError_کوڈ *p_Error_کوڈ) {
    char16_t *t;
    int32_t *o;

    t=*نشانہ;

    إذا(t<نشانہ_حد_بندی) {
        إذا(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } وإلا /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            إذا(t<نشانہ_حد_بندی) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        إذا(offsets!=خالی_سوچک && (o=*offsets)!=خالی_سوچک) {
            *o++=سروت_اشاریہ;
            إذا((*نشانہ+1)<t) {
                *o++=سروت_اشاریہ;
            }
            *offsets=o;
        }
    }

    *نشانہ=t;

    /* write overflow from c */
    إذا(c>=0) {
        إذا(cnv!=خالی_سوچک) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UChar_Error_بفر, i, c);
            cnv->UChar_Error_بفر_لمائی=i;
        }
        *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
    }
}

#جے_انت
