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
ucnv_get_Complete_Unicode_سيٽ(ثابت UConverter *cnv,
                   ثابت USetAdder *sa,
                   UConverter_Unicode_سيٽ which,
                   UError_ڪوڊ *p_Error_ڪوڊ) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_ڪوڊ;
    sa->جوڙيو_Range(sa->سيٽ, 0, 0x10ffff);
}

U_CFUNC فراغ
ucnv_get_Non_Surrogate_Unicode_سيٽ(ثابت UConverter *cnv,
                               ثابت USetAdder *sa,
                               UConverter_Unicode_سيٽ which,
                               UError_ڪوڊ *p_Error_ڪوڊ) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_ڪوڊ;
    sa->جوڙيو_Range(sa->سيٽ, 0, 0xd7ff);
    sa->جوڙيو_Range(sa->سيٽ, 0xe000, 0x10ffff);
}

U_CFUNC فراغ
ucnv_from_UWrite_بائيٽون(UConverter *cnv,
                     ثابت حرف *بائيٽون, int32_t length,
                     حرف **هدف, ثابت حرف *هدف_حد_بندي,
                     int32_t **offsets,
                     int32_t ذريعو_اشاريو,
                     UError_ڪوڊ *p_Error_ڪوڊ) {
    حرف *t=*هدف;
    int32_t *o;

    /* write bytes */
    إذا(offsets==خالي_اشارو || (o=*offsets)==خالي_اشارو) {
        طالما(length>0 && t<هدف_حد_بندي) {
            *t++=*بائيٽون++;
            --length;
        }
    } وإلا {
        /* output with offsets */
        طالما(length>0 && t<هدف_حد_بندي) {
            *t++=*بائيٽون++;
            *o++=ذريعو_اشاريو;
            --length;
        }
        *offsets=o;
    }
    *هدف=t;

    /* write overflow */
    إذا(length>0) {
        إذا(cnv!=خالي_اشارو) {
            t=(حرف *)cnv->char_Error_بفر;
            cnv->char_Error_بفر_ڊيگهائي=(int8_t)length;
            افعل {
                *t++=(uint8_t)*بائيٽون++;
            } طالما(--length>0);
        }
        *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
    }
}

U_CFUNC فراغ
ucnv_منزل_UWrite_UChars(UConverter *cnv,
                    ثابت char16_t *uchars, int32_t length,
                    char16_t **هدف, ثابت char16_t *هدف_حد_بندي,
                    int32_t **offsets,
                    int32_t ذريعو_اشاريو,
                    UError_ڪوڊ *p_Error_ڪوڊ) {
    char16_t *t=*هدف;
    int32_t *o;

    /* write UChars */
    إذا(offsets==خالي_اشارو || (o=*offsets)==خالي_اشارو) {
        طالما(length>0 && t<هدف_حد_بندي) {
            *t++=*uchars++;
            --length;
        }
    } وإلا {
        /* output with offsets */
        طالما(length>0 && t<هدف_حد_بندي) {
            *t++=*uchars++;
            *o++=ذريعو_اشاريو;
            --length;
        }
        *offsets=o;
    }
    *هدف=t;

    /* write overflow */
    إذا(length>0) {
        إذا(cnv!=خالي_اشارو) {
            t=cnv->UChar_Error_بفر;
            cnv->UChar_Error_بفر_ڊيگهائي=(int8_t)length;
            افعل {
                *t++=*uchars++;
            } طالما(--length>0);
        }
        *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
    }
}

U_CFUNC فراغ
ucnv_منزل_UWrite_ڪوڊ_نقطو(UConverter *cnv,
                       UChar32 c,
                       char16_t **هدف, ثابت char16_t *هدف_حد_بندي,
                       int32_t **offsets,
                       int32_t ذريعو_اشاريو,
                       UError_ڪوڊ *p_Error_ڪوڊ) {
    char16_t *t;
    int32_t *o;

    t=*هدف;

    إذا(t<هدف_حد_بندي) {
        إذا(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } وإلا /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            إذا(t<هدف_حد_بندي) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        إذا(offsets!=خالي_اشارو && (o=*offsets)!=خالي_اشارو) {
            *o++=ذريعو_اشاريو;
            إذا((*هدف+1)<t) {
                *o++=ذريعو_اشاريو;
            }
            *offsets=o;
        }
    }

    *هدف=t;

    /* write overflow from c */
    إذا(c>=0) {
        إذا(cnv!=خالي_اشارو) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UChar_Error_بفر, i, c);
            cnv->UChar_Error_بفر_ڊيگهائي=i;
        }
        *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
    }
}

#جيڪڏهن_پورو
