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

#නම් !UCONFIG_NO_CONVERSION

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
    නම්(offsets==nullptr || (o=*offsets)==nullptr) {
        අතරතුර(length>0 && t<targetLimit) {
            *t++=*bytes++;
            --length;
        }
    } නැතහොත් {
        /* output with offsets */
        අතරතුර(length>0 && t<targetLimit) {
            *t++=*bytes++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    නම්(length>0) {
        නම්(cnv!=nullptr) {
            t=(var_na *)cnv->charErrorBuffer;
            cnv->charErrorBufferLength=(int8_t)length;
            do {
                *t++=(uint8_t)*bytes++;
            } අතරතුර(--length>0);
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
    නම්(offsets==nullptr || (o=*offsets)==nullptr) {
        අතරතුර(length>0 && t<targetLimit) {
            *t++=*uchars++;
            --length;
        }
    } නැතහොත් {
        /* output with offsets */
        අතරතුර(length>0 && t<targetLimit) {
            *t++=*uchars++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    නම්(length>0) {
        නම්(cnv!=nullptr) {
            t=cnv->UCharErrorBuffer;
            cnv->UCharErrorBufferLength=(int8_t)length;
            do {
                *t++=*uchars++;
            } අතරතුර(--length>0);
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

    නම්(t<targetLimit) {
        නම්(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } නැතහොත් /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            නම්(t<targetLimit) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        නම්(offsets!=nullptr && (o=*offsets)!=nullptr) {
            *o++=sourceIndex;
            නම්((*target+1)<t) {
                *o++=sourceIndex;
            }
            *offsets=o;
        }
    }

    *target=t;

    /* write overflow from c */
    නම්(c>=0) {
        නම්(cnv!=nullptr) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UCharErrorBuffer, i, c);
            cnv->UCharErrorBufferLength=i;
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

#endif
