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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_CONVERSION

#அந்தர்பாவ "unicode/ucnv_err.h"
#அந்தர்பாவ "unicode/ucnv.h"
#அந்தர்பாவ "unicode/uset.h"
#அந்தர்பாவ "ucnv_cnv.h"
#அந்தர்பாவ "ucnv_bld.h"
#அந்தர்பாவ "cmemory.h"

U_CFUNC வெறுமை
ucnv_getCompleteUnicodeSet(மாறிலி UConverter *cnv,
                   மாறிலி USetAdder *sa,
                   UConverterUnicodeSet which,
                   UErrorCode *pErrorCode) {
    (வெறுமை)cnv;
    (வெறுமை)which;
    (வெறுமை)pErrorCode;
    sa->addRange(sa->set, 0, 0x10ffff);
}

U_CFUNC வெறுமை
ucnv_getNonSurrogateUnicodeSet(மாறிலி UConverter *cnv,
                               மாறிலி USetAdder *sa,
                               UConverterUnicodeSet which,
                               UErrorCode *pErrorCode) {
    (வெறுமை)cnv;
    (வெறுமை)which;
    (வெறுமை)pErrorCode;
    sa->addRange(sa->set, 0, 0xd7ff);
    sa->addRange(sa->set, 0xe000, 0x10ffff);
}

U_CFUNC வெறுமை
ucnv_fromUWriteBytes(UConverter *cnv,
                     மாறிலி எழுத்து *bytes, int32_t length,
                     எழுத்து **target, மாறிலி எழுத்து *targetLimit,
                     int32_t **offsets,
                     int32_t sourceIndex,
                     UErrorCode *pErrorCode) {
    எழுத்து *t=*target;
    int32_t *o;

    /* write bytes */
    எனில்(offsets==nullptr || (o=*offsets)==nullptr) {
        போது(length>0 && t<targetLimit) {
            *t++=*bytes++;
            --length;
        }
    } இல்லையெனில் {
        /* output with offsets */
        போது(length>0 && t<targetLimit) {
            *t++=*bytes++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    எனில்(length>0) {
        எனில்(cnv!=nullptr) {
            t=(எழுத்து *)cnv->charErrorBuffer;
            cnv->charErrorBufferLength=(int8_t)length;
            செய் {
                *t++=(uint8_t)*bytes++;
            } போது(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC வெறுமை
ucnv_toUWriteUChars(UConverter *cnv,
                    மாறிலி char16_t *uchars, int32_t length,
                    char16_t **target, மாறிலி char16_t *targetLimit,
                    int32_t **offsets,
                    int32_t sourceIndex,
                    UErrorCode *pErrorCode) {
    char16_t *t=*target;
    int32_t *o;

    /* write UChars */
    எனில்(offsets==nullptr || (o=*offsets)==nullptr) {
        போது(length>0 && t<targetLimit) {
            *t++=*uchars++;
            --length;
        }
    } இல்லையெனில் {
        /* output with offsets */
        போது(length>0 && t<targetLimit) {
            *t++=*uchars++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    எனில்(length>0) {
        எனில்(cnv!=nullptr) {
            t=cnv->UCharErrorBuffer;
            cnv->UCharErrorBufferLength=(int8_t)length;
            செய் {
                *t++=*uchars++;
            } போது(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC வெறுமை
ucnv_toUWriteCodePoint(UConverter *cnv,
                       UChar32 c,
                       char16_t **target, மாறிலி char16_t *targetLimit,
                       int32_t **offsets,
                       int32_t sourceIndex,
                       UErrorCode *pErrorCode) {
    char16_t *t;
    int32_t *o;

    t=*target;

    எனில்(t<targetLimit) {
        எனில்(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } இல்லையெனில் /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            எனில்(t<targetLimit) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        எனில்(offsets!=nullptr && (o=*offsets)!=nullptr) {
            *o++=sourceIndex;
            எனில்((*target+1)<t) {
                *o++=sourceIndex;
            }
            *offsets=o;
        }
    }

    *target=t;

    /* write overflow from c */
    எனில்(c>=0) {
        எனில்(cnv!=nullptr) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UCharErrorBuffer, i, c);
            cnv->UCharErrorBufferLength=i;
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

#endif
