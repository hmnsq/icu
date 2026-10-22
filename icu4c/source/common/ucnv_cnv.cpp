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
ucnv_get_Complete_Unicode_سېټ(ثابت UConverter *cnv,
                   ثابت USetAdder *sa,
                   UConverter_Unicode_سېټ which,
                   UError_کوډ *p_Error_کوډ) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_کوډ;
    sa->جمع_کړه_Range(sa->سېټ, 0, 0x10ffff);
}

U_CFUNC فراغ
ucnv_get_Non_Surrogate_Unicode_سېټ(ثابت UConverter *cnv,
                               ثابت USetAdder *sa,
                               UConverter_Unicode_سېټ which,
                               UError_کوډ *p_Error_کوډ) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_کوډ;
    sa->جمع_کړه_Range(sa->سېټ, 0, 0xd7ff);
    sa->جمع_کړه_Range(sa->سېټ, 0xe000, 0x10ffff);
}

U_CFUNC فراغ
ucnv_from_UWrite_بایټونه(UConverter *cnv,
                     ثابت حرف *بایټونه, int32_t length,
                     حرف **موخه, ثابت حرف *موخه_حد,
                     int32_t **offsets,
                     int32_t سرچینه_شاخص,
                     UError_کوډ *p_Error_کوډ) {
    حرف *t=*موخه;
    int32_t *o;

    /* write bytes */
    إذا(offsets==تش_اشاره || (o=*offsets)==تش_اشاره) {
        طالما(length>0 && t<موخه_حد) {
            *t++=*بایټونه++;
            --length;
        }
    } وإلا {
        /* output with offsets */
        طالما(length>0 && t<موخه_حد) {
            *t++=*بایټونه++;
            *o++=سرچینه_شاخص;
            --length;
        }
        *offsets=o;
    }
    *موخه=t;

    /* write overflow */
    إذا(length>0) {
        إذا(cnv!=تش_اشاره) {
            t=(حرف *)cnv->char_Error_بفر;
            cnv->char_Error_بفر_اوږدتیا=(int8_t)length;
            افعل {
                *t++=(uint8_t)*بایټونه++;
            } طالما(--length>0);
        }
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
    }
}

U_CFUNC فراغ
ucnv_منزل_UWrite_UChars(UConverter *cnv,
                    ثابت char16_t *uchars, int32_t length,
                    char16_t **موخه, ثابت char16_t *موخه_حد,
                    int32_t **offsets,
                    int32_t سرچینه_شاخص,
                    UError_کوډ *p_Error_کوډ) {
    char16_t *t=*موخه;
    int32_t *o;

    /* write UChars */
    إذا(offsets==تش_اشاره || (o=*offsets)==تش_اشاره) {
        طالما(length>0 && t<موخه_حد) {
            *t++=*uchars++;
            --length;
        }
    } وإلا {
        /* output with offsets */
        طالما(length>0 && t<موخه_حد) {
            *t++=*uchars++;
            *o++=سرچینه_شاخص;
            --length;
        }
        *offsets=o;
    }
    *موخه=t;

    /* write overflow */
    إذا(length>0) {
        إذا(cnv!=تش_اشاره) {
            t=cnv->UChar_Error_بفر;
            cnv->UChar_Error_بفر_اوږدتیا=(int8_t)length;
            افعل {
                *t++=*uchars++;
            } طالما(--length>0);
        }
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
    }
}

U_CFUNC فراغ
ucnv_منزل_UWrite_کوډ_ټکی(UConverter *cnv,
                       UChar32 c,
                       char16_t **موخه, ثابت char16_t *موخه_حد,
                       int32_t **offsets,
                       int32_t سرچینه_شاخص,
                       UError_کوډ *p_Error_کوډ) {
    char16_t *t;
    int32_t *o;

    t=*موخه;

    إذا(t<موخه_حد) {
        إذا(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } وإلا /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            إذا(t<موخه_حد) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        إذا(offsets!=تش_اشاره && (o=*offsets)!=تش_اشاره) {
            *o++=سرچینه_شاخص;
            إذا((*موخه+1)<t) {
                *o++=سرچینه_شاخص;
            }
            *offsets=o;
        }
    }

    *موخه=t;

    /* write overflow from c */
    إذا(c>=0) {
        إذا(cnv!=تش_اشاره) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UChar_Error_بفر, i, c);
            cnv->UChar_Error_بفر_اوږدتیا=i;
        }
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
    }
}

#د_که_پای
