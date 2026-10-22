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
ucnv_getCompleteUnicodeSet(ثابت UConverter *cnv,
                   ثابت USetAdder *sa,
                   UConverterUnicodeSet which,
                   UErrorCode *pErrorCode) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)pErrorCode;
    sa->addRange(sa->set, 0, 0x10ffff);
}

U_CFUNC فراغ
ucnv_getNonSurrogateUnicodeSet(ثابت UConverter *cnv,
                               ثابت USetAdder *sa,
                               UConverterUnicodeSet which,
                               UErrorCode *pErrorCode) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)pErrorCode;
    sa->addRange(sa->set, 0, 0xd7ff);
    sa->addRange(sa->set, 0xe000, 0x10ffff);
}

U_CFUNC فراغ
ucnv_fromUWriteBytes(UConverter *cnv,
                     ثابت حرف *bytes, int32_t length,
                     حرف **target, ثابت حرف *targetLimit,
                     int32_t **offsets,
                     int32_t sourceIndex,
                     UErrorCode *pErrorCode) {
    حرف *t=*target;
    int32_t *o;

    /* write bytes */
    إذا(offsets==nullptr || (o=*offsets)==nullptr) {
        طالما(length>0 && t<targetLimit) {
            *t++=*bytes++;
            --length;
        }
    } وإلا {
        /* output with offsets */
        طالما(length>0 && t<targetLimit) {
            *t++=*bytes++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    إذا(length>0) {
        إذا(cnv!=nullptr) {
            t=(حرف *)cnv->charErrorBuffer;
            cnv->charErrorBufferLength=(int8_t)length;
            افعل {
                *t++=(uint8_t)*bytes++;
            } طالما(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC فراغ
ucnv_toUWriteUChars(UConverter *cnv,
                    ثابت char16_t *uchars, int32_t length,
                    char16_t **target, ثابت char16_t *targetLimit,
                    int32_t **offsets,
                    int32_t sourceIndex,
                    UErrorCode *pErrorCode) {
    char16_t *t=*target;
    int32_t *o;

    /* write UChars */
    إذا(offsets==nullptr || (o=*offsets)==nullptr) {
        طالما(length>0 && t<targetLimit) {
            *t++=*uchars++;
            --length;
        }
    } وإلا {
        /* output with offsets */
        طالما(length>0 && t<targetLimit) {
            *t++=*uchars++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    إذا(length>0) {
        إذا(cnv!=nullptr) {
            t=cnv->UCharErrorBuffer;
            cnv->UCharErrorBufferLength=(int8_t)length;
            افعل {
                *t++=*uchars++;
            } طالما(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC فراغ
ucnv_toUWriteCodePoint(UConverter *cnv,
                       UChar32 c,
                       char16_t **target, ثابت char16_t *targetLimit,
                       int32_t **offsets,
                       int32_t sourceIndex,
                       UErrorCode *pErrorCode) {
    char16_t *t;
    int32_t *o;

    t=*target;

    إذا(t<targetLimit) {
        إذا(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } وإلا /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            إذا(t<targetLimit) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        إذا(offsets!=nullptr && (o=*offsets)!=nullptr) {
            *o++=sourceIndex;
            إذا((*target+1)<t) {
                *o++=sourceIndex;
            }
            *offsets=o;
        }
    }

    *target=t;

    /* write overflow from c */
    إذا(c>=0) {
        إذا(cnv!=nullptr) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UCharErrorBuffer, i, c);
            cnv->UCharErrorBufferLength=i;
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

#endif
