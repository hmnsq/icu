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

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION

#অন্তর্ভাৱ "unicode/ucnv_err.h"
#অন্তর্ভাৱ "unicode/ucnv.h"
#অন্তর্ভাৱ "unicode/uset.h"
#অন্তর্ভাৱ "ucnv_cnv.h"
#অন্তর্ভাৱ "ucnv_bld.h"
#অন্তর্ভাৱ "cmemory.h"

U_CFUNC শূন্য
ucnv_getCompleteUnicodeSet(ধ্রুৱ UConverter *cnv,
                   ধ্রুৱ USetAdder *sa,
                   UConverterUnicodeSet which,
                   UErrorCode *pErrorCode) {
    (শূন্য)cnv;
    (শূন্য)which;
    (শূন্য)pErrorCode;
    sa->addRange(sa->set, 0, 0x10ffff);
}

U_CFUNC শূন্য
ucnv_getNonSurrogateUnicodeSet(ধ্রুৱ UConverter *cnv,
                               ধ্রুৱ USetAdder *sa,
                               UConverterUnicodeSet which,
                               UErrorCode *pErrorCode) {
    (শূন্য)cnv;
    (শূন্য)which;
    (শূন্য)pErrorCode;
    sa->addRange(sa->set, 0, 0xd7ff);
    sa->addRange(sa->set, 0xe000, 0x10ffff);
}

U_CFUNC শূন্য
ucnv_fromUWriteBytes(UConverter *cnv,
                     ধ্রুৱ ৱর্ণ *bytes, int32_t length,
                     ৱর্ণ **target, ধ্রুৱ ৱর্ণ *targetLimit,
                     int32_t **offsets,
                     int32_t sourceIndex,
                     UErrorCode *pErrorCode) {
    ৱর্ণ *t=*target;
    int32_t *o;

    /* write bytes */
    যদি(offsets==nullptr || (o=*offsets)==nullptr) {
        যেতিয়ালৈ(length>0 && t<targetLimit) {
            *t++=*bytes++;
            --length;
        }
    } অন্যথা {
        /* output with offsets */
        যেতিয়ালৈ(length>0 && t<targetLimit) {
            *t++=*bytes++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    যদি(length>0) {
        যদি(cnv!=nullptr) {
            t=(ৱর্ণ *)cnv->charErrorBuffer;
            cnv->charErrorBufferLength=(int8_t)length;
            কৰা {
                *t++=(uint8_t)*bytes++;
            } যেতিয়ালৈ(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC শূন্য
ucnv_toUWriteUChars(UConverter *cnv,
                    ধ্রুৱ char16_t *uchars, int32_t length,
                    char16_t **target, ধ্রুৱ char16_t *targetLimit,
                    int32_t **offsets,
                    int32_t sourceIndex,
                    UErrorCode *pErrorCode) {
    char16_t *t=*target;
    int32_t *o;

    /* write UChars */
    যদি(offsets==nullptr || (o=*offsets)==nullptr) {
        যেতিয়ালৈ(length>0 && t<targetLimit) {
            *t++=*uchars++;
            --length;
        }
    } অন্যথা {
        /* output with offsets */
        যেতিয়ালৈ(length>0 && t<targetLimit) {
            *t++=*uchars++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    যদি(length>0) {
        যদি(cnv!=nullptr) {
            t=cnv->UCharErrorBuffer;
            cnv->UCharErrorBufferLength=(int8_t)length;
            কৰা {
                *t++=*uchars++;
            } যেতিয়ালৈ(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC শূন্য
ucnv_toUWriteCodePoint(UConverter *cnv,
                       UChar32 c,
                       char16_t **target, ধ্রুৱ char16_t *targetLimit,
                       int32_t **offsets,
                       int32_t sourceIndex,
                       UErrorCode *pErrorCode) {
    char16_t *t;
    int32_t *o;

    t=*target;

    যদি(t<targetLimit) {
        যদি(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } অন্যথা /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            যদি(t<targetLimit) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        যদি(offsets!=nullptr && (o=*offsets)!=nullptr) {
            *o++=sourceIndex;
            যদি((*target+1)<t) {
                *o++=sourceIndex;
            }
            *offsets=o;
        }
    }

    *target=t;

    /* write overflow from c */
    যদি(c>=0) {
        যদি(cnv!=nullptr) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UCharErrorBuffer, i, c);
            cnv->UCharErrorBufferLength=i;
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

#endif
