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

#include "unicode/utypes.h"

#ᰀᰧ !UCONFIG_NO_CONVERSION

#include "unicode/ucnv_err.h"
#include "unicode/ucnv.h"
#include "unicode/uset.h"
#include "ucnv_cnv.h"
#include "ucnv_bld.h"
#include "cmemory.h"

U_CFUNC void
ucnv_getCompleteUnicodeSet(const UConverter *cnv,
                   const USetAdder *sa,
                   UConverterUnicodeSet which,
                   UErrorCode *pErrorCode) {
    (void)cnv;
    (void)which;
    (void)pErrorCode;
    sa->addRange(sa->set, 0, 0x10ffff);
}

U_CFUNC void
ucnv_getNonSurrogateUnicodeSet(const UConverter *cnv,
                               const USetAdder *sa,
                               UConverterUnicodeSet which,
                               UErrorCode *pErrorCode) {
    (void)cnv;
    (void)which;
    (void)pErrorCode;
    sa->addRange(sa->set, 0, 0xd7ff);
    sa->addRange(sa->set, 0xe000, 0x10ffff);
}

U_CFUNC void
ucnv_fromUWriteBytes(UConverter *cnv,
                     const var_na *bytes, int32_t length,
                     var_na **target, const var_na *targetLimit,
                     int32_t **offsets,
                     int32_t sourceIndex,
                     UErrorCode *pErrorCode) {
    var_na *t=*target;
    int32_t *o;

    /* write bytes */
    ᰀᰧ(offsets==nullptr || (o=*offsets)==nullptr) {
        ᰀᰬ(length>0 && t<targetLimit) {
            *t++=*bytes++;
            --length;
        }
    } ᰀᰧᰀ {
        /* output with offsets */
        ᰀᰬ(length>0 && t<targetLimit) {
            *t++=*bytes++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    ᰀᰧ(length>0) {
        ᰀᰧ(cnv!=nullptr) {
            t=(var_na *)cnv->charErrorBuffer;
            cnv->charErrorBufferLength=(int8_t)length;
            do {
                *t++=(uint8_t)*bytes++;
            } ᰀᰬ(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC void
ucnv_toUWriteUChars(UConverter *cnv,
                    const char16_t *uchars, int32_t length,
                    char16_t **target, const char16_t *targetLimit,
                    int32_t **offsets,
                    int32_t sourceIndex,
                    UErrorCode *pErrorCode) {
    char16_t *t=*target;
    int32_t *o;

    /* write UChars */
    ᰀᰧ(offsets==nullptr || (o=*offsets)==nullptr) {
        ᰀᰬ(length>0 && t<targetLimit) {
            *t++=*uchars++;
            --length;
        }
    } ᰀᰧᰀ {
        /* output with offsets */
        ᰀᰬ(length>0 && t<targetLimit) {
            *t++=*uchars++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    ᰀᰧ(length>0) {
        ᰀᰧ(cnv!=nullptr) {
            t=cnv->UCharErrorBuffer;
            cnv->UCharErrorBufferLength=(int8_t)length;
            do {
                *t++=*uchars++;
            } ᰀᰬ(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC void
ucnv_toUWriteCodePoint(UConverter *cnv,
                       UChar32 c,
                       char16_t **target, const char16_t *targetLimit,
                       int32_t **offsets,
                       int32_t sourceIndex,
                       UErrorCode *pErrorCode) {
    char16_t *t;
    int32_t *o;

    t=*target;

    ᰀᰧ(t<targetLimit) {
        ᰀᰧ(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } ᰀᰧᰀ /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            ᰀᰧ(t<targetLimit) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        ᰀᰧ(offsets!=nullptr && (o=*offsets)!=nullptr) {
            *o++=sourceIndex;
            ᰀᰧ((*target+1)<t) {
                *o++=sourceIndex;
            }
            *offsets=o;
        }
    }

    *target=t;

    /* write overflow from c */
    ᰀᰧ(c>=0) {
        ᰀᰧ(cnv!=nullptr) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UCharErrorBuffer, i, c);
            cnv->UCharErrorBufferLength=i;
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

#endif
