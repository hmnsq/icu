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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION

#অন্তর্ভাব "unicode/ucnv_err.h"
#অন্তর্ভাব "unicode/ucnv.h"
#অন্তর্ভাব "unicode/uset.h"
#অন্তর্ভাব "ucnv_cnv.h"
#অন্তর্ভাব "ucnv_bld.h"
#অন্তর্ভাব "cmemory.h"

U_CFUNC শূন্য
ucnv_getCompleteUnicodeSet(ধ্রুবক UConverter *cnv,
                   ধ্রুবক USetAdder *sa,
                   UConverterUnicodeSet which,
                   UErrorCode *pErrorCode) {
    (শূন্য)cnv;
    (শূন্য)which;
    (শূন্য)pErrorCode;
    sa->addRange(sa->set, 0, 0x10ffff);
}

U_CFUNC শূন্য
ucnv_getNonSurrogateUnicodeSet(ধ্রুবক UConverter *cnv,
                               ধ্রুবক USetAdder *sa,
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
                     ধ্রুবক অক্ষর *bytes, int32_t length,
                     অক্ষর **target, ধ্রুবক অক্ষর *targetLimit,
                     int32_t **offsets,
                     int32_t source_সূচক,
                     UErrorCode *pErrorCode) {
    অক্ষর *t=*target;
    int32_t *o;

    /* write bytes */
    যদি(offsets==nullptr || (o=*offsets)==nullptr) {
        যখন(length>0 && t<targetLimit) {
            *t++=*bytes++;
            --length;
        }
    } অন্যথা {
        /* output with offsets */
        যখন(length>0 && t<targetLimit) {
            *t++=*bytes++;
            *o++=source_সূচক;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    যদি(length>0) {
        যদি(cnv!=nullptr) {
            t=(অক্ষর *)cnv->char_Error_বাফার;
            cnv->char_Error_বাফার_দৈর্ঘ্য=(int8_t)length;
            করো {
                *t++=(uint8_t)*bytes++;
            } যখন(--length>0);
        }
        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
    }
}

U_CFUNC শূন্য
ucnv_toUWriteUChars(UConverter *cnv,
                    ধ্রুবক char16_t *uchars, int32_t length,
                    char16_t **target, ধ্রুবক char16_t *targetLimit,
                    int32_t **offsets,
                    int32_t source_সূচক,
                    UErrorCode *pErrorCode) {
    char16_t *t=*target;
    int32_t *o;

    /* write UChars */
    যদি(offsets==nullptr || (o=*offsets)==nullptr) {
        যখন(length>0 && t<targetLimit) {
            *t++=*uchars++;
            --length;
        }
    } অন্যথা {
        /* output with offsets */
        যখন(length>0 && t<targetLimit) {
            *t++=*uchars++;
            *o++=source_সূচক;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    যদি(length>0) {
        যদি(cnv!=nullptr) {
            t=cnv->UChar_Error_বাফার;
            cnv->UChar_Error_বাফার_দৈর্ঘ্য=(int8_t)length;
            করো {
                *t++=*uchars++;
            } যখন(--length>0);
        }
        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
    }
}

U_CFUNC শূন্য
ucnv_toUWriteCodePoint(UConverter *cnv,
                       UChar32 c,
                       char16_t **target, ধ্রুবক char16_t *targetLimit,
                       int32_t **offsets,
                       int32_t source_সূচক,
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
            *o++=source_সূচক;
            যদি((*target+1)<t) {
                *o++=source_সূচক;
            }
            *offsets=o;
        }
    }

    *target=t;

    /* write overflow from c */
    যদি(c>=0) {
        যদি(cnv!=nullptr) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UChar_Error_বাফার, i, c);
            cnv->UChar_Error_বাফার_দৈর্ঘ্য=i;
        }
        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
    }
}

#endif
