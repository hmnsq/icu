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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/ucnv_err.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "cmemory.h"

U_CFUNC शून्य
ucnv_getCompleteUnicodeSet(ध्रुव UConverter *cnv,
                   ध्रुव USetAdder *sa,
                   UConverterUnicodeSet which,
                   UErrorCode *pErrorCode) {
    (शून्य)cnv;
    (शून्य)which;
    (शून्य)pErrorCode;
    sa->addRange(sa->set, 0, 0x10ffff);
}

U_CFUNC शून्य
ucnv_getNonSurrogateUnicodeSet(ध्रुव UConverter *cnv,
                               ध्रुव USetAdder *sa,
                               UConverterUnicodeSet which,
                               UErrorCode *pErrorCode) {
    (शून्य)cnv;
    (शून्य)which;
    (शून्य)pErrorCode;
    sa->addRange(sa->set, 0, 0xd7ff);
    sa->addRange(sa->set, 0xe000, 0x10ffff);
}

U_CFUNC शून्य
ucnv_fromUWriteBytes(UConverter *cnv,
                     ध्रुव वर्ण *bytes, int32_t length,
                     वर्ण **target, ध्रुव वर्ण *targetLimit,
                     int32_t **offsets,
                     int32_t sourceIndex,
                     UErrorCode *pErrorCode) {
    वर्ण *t=*target;
    int32_t *o;

    /* write bytes */
    यदि(offsets==nullptr || (o=*offsets)==nullptr) {
        यावत्(length>0 && t<targetLimit) {
            *t++=*bytes++;
            --length;
        }
    } अन्यथा {
        /* output with offsets */
        यावत्(length>0 && t<targetLimit) {
            *t++=*bytes++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    यदि(length>0) {
        यदि(cnv!=nullptr) {
            t=(वर्ण *)cnv->charErrorBuffer;
            cnv->charErrorBufferLength=(int8_t)length;
            कुरु {
                *t++=(uint8_t)*bytes++;
            } यावत्(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC शून्य
ucnv_toUWriteUChars(UConverter *cnv,
                    ध्रुव char16_t *uchars, int32_t length,
                    char16_t **target, ध्रुव char16_t *targetLimit,
                    int32_t **offsets,
                    int32_t sourceIndex,
                    UErrorCode *pErrorCode) {
    char16_t *t=*target;
    int32_t *o;

    /* write UChars */
    यदि(offsets==nullptr || (o=*offsets)==nullptr) {
        यावत्(length>0 && t<targetLimit) {
            *t++=*uchars++;
            --length;
        }
    } अन्यथा {
        /* output with offsets */
        यावत्(length>0 && t<targetLimit) {
            *t++=*uchars++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    यदि(length>0) {
        यदि(cnv!=nullptr) {
            t=cnv->UCharErrorBuffer;
            cnv->UCharErrorBufferLength=(int8_t)length;
            कुरु {
                *t++=*uchars++;
            } यावत्(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC शून्य
ucnv_toUWriteCodePoint(UConverter *cnv,
                       UChar32 c,
                       char16_t **target, ध्रुव char16_t *targetLimit,
                       int32_t **offsets,
                       int32_t sourceIndex,
                       UErrorCode *pErrorCode) {
    char16_t *t;
    int32_t *o;

    t=*target;

    यदि(t<targetLimit) {
        यदि(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } अन्यथा /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            यदि(t<targetLimit) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        यदि(offsets!=nullptr && (o=*offsets)!=nullptr) {
            *o++=sourceIndex;
            यदि((*target+1)<t) {
                *o++=sourceIndex;
            }
            *offsets=o;
        }
    }

    *target=t;

    /* write overflow from c */
    यदि(c>=0) {
        यदि(cnv!=nullptr) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UCharErrorBuffer, i, c);
            cnv->UCharErrorBufferLength=i;
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

#endif
