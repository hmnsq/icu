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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv_err.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

U_CFUNC ಶೂನ್ಯ
ucnv_getCompleteUnicodeSet(ಧ್ರುವ UConverter *cnv,
                   ಧ್ರುವ USetAdder *sa,
                   UConverterUnicodeSet which,
                   UErrorCode *pErrorCode) {
    (ಶೂನ್ಯ)cnv;
    (ಶೂನ್ಯ)which;
    (ಶೂನ್ಯ)pErrorCode;
    sa->addRange(sa->set, 0, 0x10ffff);
}

U_CFUNC ಶೂನ್ಯ
ucnv_getNonSurrogateUnicodeSet(ಧ್ರುವ UConverter *cnv,
                               ಧ್ರುವ USetAdder *sa,
                               UConverterUnicodeSet which,
                               UErrorCode *pErrorCode) {
    (ಶೂನ್ಯ)cnv;
    (ಶೂನ್ಯ)which;
    (ಶೂನ್ಯ)pErrorCode;
    sa->addRange(sa->set, 0, 0xd7ff);
    sa->addRange(sa->set, 0xe000, 0x10ffff);
}

U_CFUNC ಶೂನ್ಯ
ucnv_fromUWriteBytes(UConverter *cnv,
                     ಧ್ರುವ ವರ್ಣ *bytes, int32_t length,
                     ವರ್ಣ **target, ಧ್ರುವ ವರ್ಣ *targetLimit,
                     int32_t **offsets,
                     int32_t sourceIndex,
                     UErrorCode *pErrorCode) {
    ವರ್ಣ *t=*target;
    int32_t *o;

    /* write bytes */
    ಯದಿ(offsets==nullptr || (o=*offsets)==nullptr) {
        ಯಾವತ್(length>0 && t<targetLimit) {
            *t++=*bytes++;
            --length;
        }
    } ಅನ್ಯಥಾ {
        /* output with offsets */
        ಯಾವತ್(length>0 && t<targetLimit) {
            *t++=*bytes++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    ಯದಿ(length>0) {
        ಯದಿ(cnv!=nullptr) {
            t=(ವರ್ಣ *)cnv->charErrorBuffer;
            cnv->charErrorBufferLength=(int8_t)length;
            ಕುರು {
                *t++=(uint8_t)*bytes++;
            } ಯಾವತ್(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC ಶೂನ್ಯ
ucnv_toUWriteUChars(UConverter *cnv,
                    ಧ್ರುವ char16_t *uchars, int32_t length,
                    char16_t **target, ಧ್ರುವ char16_t *targetLimit,
                    int32_t **offsets,
                    int32_t sourceIndex,
                    UErrorCode *pErrorCode) {
    char16_t *t=*target;
    int32_t *o;

    /* write UChars */
    ಯದಿ(offsets==nullptr || (o=*offsets)==nullptr) {
        ಯಾವತ್(length>0 && t<targetLimit) {
            *t++=*uchars++;
            --length;
        }
    } ಅನ್ಯಥಾ {
        /* output with offsets */
        ಯಾವತ್(length>0 && t<targetLimit) {
            *t++=*uchars++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    ಯದಿ(length>0) {
        ಯದಿ(cnv!=nullptr) {
            t=cnv->UCharErrorBuffer;
            cnv->UCharErrorBufferLength=(int8_t)length;
            ಕುರು {
                *t++=*uchars++;
            } ಯಾವತ್(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC ಶೂನ್ಯ
ucnv_toUWriteCodePoint(UConverter *cnv,
                       UChar32 c,
                       char16_t **target, ಧ್ರುವ char16_t *targetLimit,
                       int32_t **offsets,
                       int32_t sourceIndex,
                       UErrorCode *pErrorCode) {
    char16_t *t;
    int32_t *o;

    t=*target;

    ಯದಿ(t<targetLimit) {
        ಯದಿ(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } ಅನ್ಯಥಾ /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            ಯದಿ(t<targetLimit) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        ಯದಿ(offsets!=nullptr && (o=*offsets)!=nullptr) {
            *o++=sourceIndex;
            ಯದಿ((*target+1)<t) {
                *o++=sourceIndex;
            }
            *offsets=o;
        }
    }

    *target=t;

    /* write overflow from c */
    ಯದಿ(c>=0) {
        ಯದಿ(cnv!=nullptr) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UCharErrorBuffer, i, c);
            cnv->UCharErrorBufferLength=i;
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

#endif
