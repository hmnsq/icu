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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION

#समावेश "unicode/ucnv_err.h"
#समावेश "unicode/ucnv.h"
#समावेश "unicode/uset.h"
#समावेश "ucnv_cnv.h"
#समावेश "ucnv_bld.h"
#समावेश "cmemory.h"

U_CFUNC व्योम
ucnv_getCompleteUnicodeSet(स्थिर UConverter *cnv,
                   स्थिर USetAdder *sa,
                   UConverterUnicodeSet which,
                   UErrorCode *pErrorCode) {
    (व्योम)cnv;
    (व्योम)which;
    (व्योम)pErrorCode;
    sa->addRange(sa->set, 0, 0x10ffff);
}

U_CFUNC व्योम
ucnv_getNonSurrogateUnicodeSet(स्थिर UConverter *cnv,
                               स्थिर USetAdder *sa,
                               UConverterUnicodeSet which,
                               UErrorCode *pErrorCode) {
    (व्योम)cnv;
    (व्योम)which;
    (व्योम)pErrorCode;
    sa->addRange(sa->set, 0, 0xd7ff);
    sa->addRange(sa->set, 0xe000, 0x10ffff);
}

U_CFUNC व्योम
ucnv_fromUWriteBytes(UConverter *cnv,
                     स्थिर अक्षर *bytes, int32_t length,
                     अक्षर **target, स्थिर अक्षर *targetLimit,
                     int32_t **offsets,
                     int32_t source_सूचकांक,
                     UErrorCode *pErrorCode) {
    अक्षर *t=*target;
    int32_t *o;

    /* write bytes */
    अगर(offsets==nullptr || (o=*offsets)==nullptr) {
        जबतक(length>0 && t<targetLimit) {
            *t++=*bytes++;
            --length;
        }
    } अन्यथा {
        /* output with offsets */
        जबतक(length>0 && t<targetLimit) {
            *t++=*bytes++;
            *o++=source_सूचकांक;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    अगर(length>0) {
        अगर(cnv!=nullptr) {
            t=(अक्षर *)cnv->char_Error_बफर;
            cnv->char_Error_बफर_लंबाई=(int8_t)length;
            करो {
                *t++=(uint8_t)*bytes++;
            } जबतक(--length>0);
        }
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
    }
}

U_CFUNC व्योम
ucnv_toUWriteUChars(UConverter *cnv,
                    स्थिर char16_t *uchars, int32_t length,
                    char16_t **target, स्थिर char16_t *targetLimit,
                    int32_t **offsets,
                    int32_t source_सूचकांक,
                    UErrorCode *pErrorCode) {
    char16_t *t=*target;
    int32_t *o;

    /* write UChars */
    अगर(offsets==nullptr || (o=*offsets)==nullptr) {
        जबतक(length>0 && t<targetLimit) {
            *t++=*uchars++;
            --length;
        }
    } अन्यथा {
        /* output with offsets */
        जबतक(length>0 && t<targetLimit) {
            *t++=*uchars++;
            *o++=source_सूचकांक;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    अगर(length>0) {
        अगर(cnv!=nullptr) {
            t=cnv->UChar_Error_बफर;
            cnv->UChar_Error_बफर_लंबाई=(int8_t)length;
            करो {
                *t++=*uchars++;
            } जबतक(--length>0);
        }
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
    }
}

U_CFUNC व्योम
ucnv_toUWriteCodePoint(UConverter *cnv,
                       UChar32 c,
                       char16_t **target, स्थिर char16_t *targetLimit,
                       int32_t **offsets,
                       int32_t source_सूचकांक,
                       UErrorCode *pErrorCode) {
    char16_t *t;
    int32_t *o;

    t=*target;

    अगर(t<targetLimit) {
        अगर(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } अन्यथा /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            अगर(t<targetLimit) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        अगर(offsets!=nullptr && (o=*offsets)!=nullptr) {
            *o++=source_सूचकांक;
            अगर((*target+1)<t) {
                *o++=source_सूचकांक;
            }
            *offsets=o;
        }
    }

    *target=t;

    /* write overflow from c */
    अगर(c>=0) {
        अगर(cnv!=nullptr) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UChar_Error_बफर, i, c);
            cnv->UChar_Error_बफर_लंबाई=i;
        }
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
    }
}

#पूर्ण_अगर
