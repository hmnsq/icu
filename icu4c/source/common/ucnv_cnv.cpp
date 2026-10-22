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

U_CFUNC تهی
ucnv_getCompleteUnicodeSet(ثابت UConverter *cnv,
                   ثابت USetAdder *sa,
                   UConverterUnicodeSet which,
                   UErrorCode *pErrorCode) {
    (تهی)cnv;
    (تهی)which;
    (تهی)pErrorCode;
    sa->addRange(sa->set, 0, 0x10ffff);
}

U_CFUNC تهی
ucnv_getNonSurrogateUnicodeSet(ثابت UConverter *cnv,
                               ثابت USetAdder *sa,
                               UConverterUnicodeSet which,
                               UErrorCode *pErrorCode) {
    (تهی)cnv;
    (تهی)which;
    (تهی)pErrorCode;
    sa->addRange(sa->set, 0, 0xd7ff);
    sa->addRange(sa->set, 0xe000, 0x10ffff);
}

U_CFUNC تهی
ucnv_fromUWriteBytes(UConverter *cnv,
                     ثابت نویسه *bytes, int32_t length,
                     نویسه **target, ثابت نویسه *targetLimit,
                     int32_t **offsets,
                     int32_t sourceIndex,
                     UErrorCode *pErrorCode) {
    نویسه *t=*target;
    int32_t *o;

    /* write bytes */
    اگر(offsets==nullptr || (o=*offsets)==nullptr) {
        وقتی(length>0 && t<targetLimit) {
            *t++=*bytes++;
            --length;
        }
    } وگرنه {
        /* output with offsets */
        وقتی(length>0 && t<targetLimit) {
            *t++=*bytes++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    اگر(length>0) {
        اگر(cnv!=nullptr) {
            t=(نویسه *)cnv->charErrorBuffer;
            cnv->charErrorBufferLength=(int8_t)length;
            انجامبده {
                *t++=(uint8_t)*bytes++;
            } وقتی(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC تهی
ucnv_toUWriteUChars(UConverter *cnv,
                    ثابت char16_t *uchars, int32_t length,
                    char16_t **target, ثابت char16_t *targetLimit,
                    int32_t **offsets,
                    int32_t sourceIndex,
                    UErrorCode *pErrorCode) {
    char16_t *t=*target;
    int32_t *o;

    /* write UChars */
    اگر(offsets==nullptr || (o=*offsets)==nullptr) {
        وقتی(length>0 && t<targetLimit) {
            *t++=*uchars++;
            --length;
        }
    } وگرنه {
        /* output with offsets */
        وقتی(length>0 && t<targetLimit) {
            *t++=*uchars++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    اگر(length>0) {
        اگر(cnv!=nullptr) {
            t=cnv->UCharErrorBuffer;
            cnv->UCharErrorBufferLength=(int8_t)length;
            انجامبده {
                *t++=*uchars++;
            } وقتی(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC تهی
ucnv_toUWriteCodePoint(UConverter *cnv,
                       UChar32 c,
                       char16_t **target, ثابت char16_t *targetLimit,
                       int32_t **offsets,
                       int32_t sourceIndex,
                       UErrorCode *pErrorCode) {
    char16_t *t;
    int32_t *o;

    t=*target;

    اگر(t<targetLimit) {
        اگر(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } وگرنه /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            اگر(t<targetLimit) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        اگر(offsets!=nullptr && (o=*offsets)!=nullptr) {
            *o++=sourceIndex;
            اگر((*target+1)<t) {
                *o++=sourceIndex;
            }
            *offsets=o;
        }
    }

    *target=t;

    /* write overflow from c */
    اگر(c>=0) {
        اگر(cnv!=nullptr) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UCharErrorBuffer, i, c);
            cnv->UCharErrorBufferLength=i;
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

#endif
