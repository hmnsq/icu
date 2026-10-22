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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_CONVERSION

#ଅନ୍ତର୍ଭାଵ "unicode/ucnv_err.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucnv.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uset.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_cnv.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_bld.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"

U_CFUNC ଶୂନ୍ୟ
ucnv_getCompleteUnicodeSet(ଅଚଳ UConverter *cnv,
                   ଅଚଳ USetAdder *sa,
                   UConverterUnicodeSet which,
                   UErrorCode *pErrorCode) {
    (ଶୂନ୍ୟ)cnv;
    (ଶୂନ୍ୟ)which;
    (ଶୂନ୍ୟ)pErrorCode;
    sa->addRange(sa->set, 0, 0x10ffff);
}

U_CFUNC ଶୂନ୍ୟ
ucnv_getNonSurrogateUnicodeSet(ଅଚଳ UConverter *cnv,
                               ଅଚଳ USetAdder *sa,
                               UConverterUnicodeSet which,
                               UErrorCode *pErrorCode) {
    (ଶୂନ୍ୟ)cnv;
    (ଶୂନ୍ୟ)which;
    (ଶୂନ୍ୟ)pErrorCode;
    sa->addRange(sa->set, 0, 0xd7ff);
    sa->addRange(sa->set, 0xe000, 0x10ffff);
}

U_CFUNC ଶୂନ୍ୟ
ucnv_fromUWriteBytes(UConverter *cnv,
                     ଅଚଳ ଅକ୍ଷର *bytes, int32_t length,
                     ଅକ୍ଷର **target, ଅଚଳ ଅକ୍ଷର *targetLimit,
                     int32_t **offsets,
                     int32_t sourceIndex,
                     UErrorCode *pErrorCode) {
    ଅକ୍ଷର *t=*target;
    int32_t *o;

    /* write bytes */
    ଯଦି(offsets==nullptr || (o=*offsets)==nullptr) {
        ଯେତେବେଳେ(length>0 && t<targetLimit) {
            *t++=*bytes++;
            --length;
        }
    } ଅନ୍ୟଥା {
        /* output with offsets */
        ଯେତେବେଳେ(length>0 && t<targetLimit) {
            *t++=*bytes++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    ଯଦି(length>0) {
        ଯଦି(cnv!=nullptr) {
            t=(ଅକ୍ଷର *)cnv->charErrorBuffer;
            cnv->charErrorBufferLength=(int8_t)length;
            କର {
                *t++=(uint8_t)*bytes++;
            } ଯେତେବେଳେ(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC ଶୂନ୍ୟ
ucnv_toUWriteUChars(UConverter *cnv,
                    ଅଚଳ char16_t *uchars, int32_t length,
                    char16_t **target, ଅଚଳ char16_t *targetLimit,
                    int32_t **offsets,
                    int32_t sourceIndex,
                    UErrorCode *pErrorCode) {
    char16_t *t=*target;
    int32_t *o;

    /* write UChars */
    ଯଦି(offsets==nullptr || (o=*offsets)==nullptr) {
        ଯେତେବେଳେ(length>0 && t<targetLimit) {
            *t++=*uchars++;
            --length;
        }
    } ଅନ୍ୟଥା {
        /* output with offsets */
        ଯେତେବେଳେ(length>0 && t<targetLimit) {
            *t++=*uchars++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    ଯଦି(length>0) {
        ଯଦି(cnv!=nullptr) {
            t=cnv->UCharErrorBuffer;
            cnv->UCharErrorBufferLength=(int8_t)length;
            କର {
                *t++=*uchars++;
            } ଯେତେବେଳେ(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC ଶୂନ୍ୟ
ucnv_toUWriteCodePoint(UConverter *cnv,
                       UChar32 c,
                       char16_t **target, ଅଚଳ char16_t *targetLimit,
                       int32_t **offsets,
                       int32_t sourceIndex,
                       UErrorCode *pErrorCode) {
    char16_t *t;
    int32_t *o;

    t=*target;

    ଯଦି(t<targetLimit) {
        ଯଦି(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } ଅନ୍ୟଥା /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            ଯଦି(t<targetLimit) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        ଯଦି(offsets!=nullptr && (o=*offsets)!=nullptr) {
            *o++=sourceIndex;
            ଯଦି((*target+1)<t) {
                *o++=sourceIndex;
            }
            *offsets=o;
        }
    }

    *target=t;

    /* write overflow from c */
    ଯଦି(c>=0) {
        ଯଦି(cnv!=nullptr) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UCharErrorBuffer, i, c);
            cnv->UCharErrorBufferLength=i;
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

#endif
