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
ucnv_get_Complete_Unicode_مجموعه(ثابت UConverter *cnv,
                   ثابت USetAdder *sa,
                   UConverter_Unicode_مجموعه which,
                   UError_کد *p_Error_کد) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_کد;
    sa->جمع_کن_Range(sa->مجموعه, 0, 0x10ffff);
}

U_CFUNC فراغ
ucnv_get_Non_Surrogate_Unicode_مجموعه(ثابت UConverter *cnv,
                               ثابت USetAdder *sa,
                               UConverter_Unicode_مجموعه which,
                               UError_کد *p_Error_کد) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_کد;
    sa->جمع_کن_Range(sa->مجموعه, 0, 0xd7ff);
    sa->جمع_کن_Range(sa->مجموعه, 0xe000, 0x10ffff);
}

U_CFUNC فراغ
ucnv_from_UWrite_بایتها(UConverter *cnv,
                     ثابت حرف *بایتها, int32_t length,
                     حرف **هدف, ثابت حرف *هدف_حد,
                     int32_t **offsets,
                     int32_t منبع_شاخص,
                     UError_کد *p_Error_کد) {
    حرف *t=*هدف;
    int32_t *o;

    /* write bytes */
    إذا(offsets==اشاره_تهی || (o=*offsets)==اشاره_تهی) {
        طالما(length>0 && t<هدف_حد) {
            *t++=*بایتها++;
            --length;
        }
    } وإلا {
        /* output with offsets */
        طالما(length>0 && t<هدف_حد) {
            *t++=*بایتها++;
            *o++=منبع_شاخص;
            --length;
        }
        *offsets=o;
    }
    *هدف=t;

    /* write overflow */
    إذا(length>0) {
        إذا(cnv!=اشاره_تهی) {
            t=(حرف *)cnv->char_Error_میانگیر;
            cnv->char_Error_میانگیر_درازی=(int8_t)length;
            افعل {
                *t++=(uint8_t)*بایتها++;
            } طالما(--length>0);
        }
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
    }
}

U_CFUNC فراغ
ucnv_مقصد_UWrite_UChars(UConverter *cnv,
                    ثابت char16_t *uchars, int32_t length,
                    char16_t **هدف, ثابت char16_t *هدف_حد,
                    int32_t **offsets,
                    int32_t منبع_شاخص,
                    UError_کد *p_Error_کد) {
    char16_t *t=*هدف;
    int32_t *o;

    /* write UChars */
    إذا(offsets==اشاره_تهی || (o=*offsets)==اشاره_تهی) {
        طالما(length>0 && t<هدف_حد) {
            *t++=*uchars++;
            --length;
        }
    } وإلا {
        /* output with offsets */
        طالما(length>0 && t<هدف_حد) {
            *t++=*uchars++;
            *o++=منبع_شاخص;
            --length;
        }
        *offsets=o;
    }
    *هدف=t;

    /* write overflow */
    إذا(length>0) {
        إذا(cnv!=اشاره_تهی) {
            t=cnv->UChar_Error_میانگیر;
            cnv->UChar_Error_میانگیر_درازی=(int8_t)length;
            افعل {
                *t++=*uchars++;
            } طالما(--length>0);
        }
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
    }
}

U_CFUNC فراغ
ucnv_مقصد_UWrite_کد_نقطه(UConverter *cnv,
                       UChar32 c,
                       char16_t **هدف, ثابت char16_t *هدف_حد,
                       int32_t **offsets,
                       int32_t منبع_شاخص,
                       UError_کد *p_Error_کد) {
    char16_t *t;
    int32_t *o;

    t=*هدف;

    إذا(t<هدف_حد) {
        إذا(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } وإلا /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            إذا(t<هدف_حد) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        إذا(offsets!=اشاره_تهی && (o=*offsets)!=اشاره_تهی) {
            *o++=منبع_شاخص;
            إذا((*هدف+1)<t) {
                *o++=منبع_شاخص;
            }
            *offsets=o;
        }
    }

    *هدف=t;

    /* write overflow from c */
    إذا(c>=0) {
        إذا(cnv!=اشاره_تهی) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UChar_Error_میانگیر, i, c);
            cnv->UChar_Error_میانگیر_درازی=i;
        }
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
    }
}

#پایان_اگر
