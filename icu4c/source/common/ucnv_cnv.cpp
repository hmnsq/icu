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

#जर !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/ucnv_err.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "cmemory.h"

U_CFUNC शून्य
ucnv_getCompleteUnicodeSet(स्थिरांक UConverter *cnv,
                   स्थिरांक USetAdder *sa,
                   UConverterUnicodeSet which,
                   UErrorCode *pErrorCode) {
    (शून्य)cnv;
    (शून्य)which;
    (शून्य)pErrorCode;
    sa->addRange(sa->set, 0, 0x10ffff);
}

U_CFUNC शून्य
ucnv_getNonSurrogateUnicodeSet(स्थिरांक UConverter *cnv,
                               स्थिरांक USetAdder *sa,
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
                     स्थिरांक अक्षर *bytes, int32_t length,
                     अक्षर **target, स्थिरांक अक्षर *targetLimit,
                     int32_t **offsets,
                     int32_t sourceIndex,
                     UErrorCode *pErrorCode) {
    अक्षर *t=*target;
    int32_t *o;

    /* write bytes */
    जर(offsets==nullptr || (o=*offsets)==nullptr) {
        पर्यंत(length>0 && t<targetLimit) {
            *t++=*bytes++;
            --length;
        }
    } अन्यथा {
        /* output with offsets */
        पर्यंत(length>0 && t<targetLimit) {
            *t++=*bytes++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    जर(length>0) {
        जर(cnv!=nullptr) {
            t=(अक्षर *)cnv->charErrorBuffer;
            cnv->charErrorBufferLength=(int8_t)length;
            करा {
                *t++=(uint8_t)*bytes++;
            } पर्यंत(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC शून्य
ucnv_toUWriteUChars(UConverter *cnv,
                    स्थिरांक char16_t *uchars, int32_t length,
                    char16_t **target, स्थिरांक char16_t *targetLimit,
                    int32_t **offsets,
                    int32_t sourceIndex,
                    UErrorCode *pErrorCode) {
    char16_t *t=*target;
    int32_t *o;

    /* write UChars */
    जर(offsets==nullptr || (o=*offsets)==nullptr) {
        पर्यंत(length>0 && t<targetLimit) {
            *t++=*uchars++;
            --length;
        }
    } अन्यथा {
        /* output with offsets */
        पर्यंत(length>0 && t<targetLimit) {
            *t++=*uchars++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    जर(length>0) {
        जर(cnv!=nullptr) {
            t=cnv->UCharErrorBuffer;
            cnv->UCharErrorBufferLength=(int8_t)length;
            करा {
                *t++=*uchars++;
            } पर्यंत(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC शून्य
ucnv_toUWriteCodePoint(UConverter *cnv,
                       UChar32 c,
                       char16_t **target, स्थिरांक char16_t *targetLimit,
                       int32_t **offsets,
                       int32_t sourceIndex,
                       UErrorCode *pErrorCode) {
    char16_t *t;
    int32_t *o;

    t=*target;

    जर(t<targetLimit) {
        जर(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } अन्यथा /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            जर(t<targetLimit) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        जर(offsets!=nullptr && (o=*offsets)!=nullptr) {
            *o++=sourceIndex;
            जर((*target+1)<t) {
                *o++=sourceIndex;
            }
            *offsets=o;
        }
    }

    *target=t;

    /* write overflow from c */
    जर(c>=0) {
        जर(cnv!=nullptr) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UCharErrorBuffer, i, c);
            cnv->UCharErrorBufferLength=i;
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

#endif
