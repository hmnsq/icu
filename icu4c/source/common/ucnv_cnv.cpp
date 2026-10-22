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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_CONVERSION

#અન્તર્ભાવ "unicode/ucnv_err.h"
#અન્તર્ભાવ "unicode/ucnv.h"
#અન્તર્ભાવ "unicode/uset.h"
#અન્તર્ભાવ "ucnv_cnv.h"
#અન્તર્ભાવ "ucnv_bld.h"
#અન્તર્ભાવ "cmemory.h"

U_CFUNC શૂન્ય
ucnv_getCompleteUnicodeSet(અચલ UConverter *cnv,
                   અચલ USetAdder *sa,
                   UConverterUnicodeSet which,
                   UErrorCode *pErrorCode) {
    (શૂન્ય)cnv;
    (શૂન્ય)which;
    (શૂન્ય)pErrorCode;
    sa->addRange(sa->set, 0, 0x10ffff);
}

U_CFUNC શૂન્ય
ucnv_getNonSurrogateUnicodeSet(અચલ UConverter *cnv,
                               અચલ USetAdder *sa,
                               UConverterUnicodeSet which,
                               UErrorCode *pErrorCode) {
    (શૂન્ય)cnv;
    (શૂન્ય)which;
    (શૂન્ય)pErrorCode;
    sa->addRange(sa->set, 0, 0xd7ff);
    sa->addRange(sa->set, 0xe000, 0x10ffff);
}

U_CFUNC શૂન્ય
ucnv_fromUWriteBytes(UConverter *cnv,
                     અચલ અક્ષર *bytes, int32_t length,
                     અક્ષર **target, અચલ અક્ષર *targetLimit,
                     int32_t **offsets,
                     int32_t sourceIndex,
                     UErrorCode *pErrorCode) {
    અક્ષર *t=*target;
    int32_t *o;

    /* write bytes */
    જો(offsets==nullptr || (o=*offsets)==nullptr) {
        જ્યારે(length>0 && t<targetLimit) {
            *t++=*bytes++;
            --length;
        }
    } નહીંતર {
        /* output with offsets */
        જ્યારે(length>0 && t<targetLimit) {
            *t++=*bytes++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    જો(length>0) {
        જો(cnv!=nullptr) {
            t=(અક્ષર *)cnv->charErrorBuffer;
            cnv->charErrorBufferLength=(int8_t)length;
            કરો {
                *t++=(uint8_t)*bytes++;
            } જ્યારે(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC શૂન્ય
ucnv_toUWriteUChars(UConverter *cnv,
                    અચલ char16_t *uchars, int32_t length,
                    char16_t **target, અચલ char16_t *targetLimit,
                    int32_t **offsets,
                    int32_t sourceIndex,
                    UErrorCode *pErrorCode) {
    char16_t *t=*target;
    int32_t *o;

    /* write UChars */
    જો(offsets==nullptr || (o=*offsets)==nullptr) {
        જ્યારે(length>0 && t<targetLimit) {
            *t++=*uchars++;
            --length;
        }
    } નહીંતર {
        /* output with offsets */
        જ્યારે(length>0 && t<targetLimit) {
            *t++=*uchars++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    જો(length>0) {
        જો(cnv!=nullptr) {
            t=cnv->UCharErrorBuffer;
            cnv->UCharErrorBufferLength=(int8_t)length;
            કરો {
                *t++=*uchars++;
            } જ્યારે(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC શૂન્ય
ucnv_toUWriteCodePoint(UConverter *cnv,
                       UChar32 c,
                       char16_t **target, અચલ char16_t *targetLimit,
                       int32_t **offsets,
                       int32_t sourceIndex,
                       UErrorCode *pErrorCode) {
    char16_t *t;
    int32_t *o;

    t=*target;

    જો(t<targetLimit) {
        જો(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } નહીંતર /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            જો(t<targetLimit) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        જો(offsets!=nullptr && (o=*offsets)!=nullptr) {
            *o++=sourceIndex;
            જો((*target+1)<t) {
                *o++=sourceIndex;
            }
            *offsets=o;
        }
    }

    *target=t;

    /* write overflow from c */
    જો(c>=0) {
        જો(cnv!=nullptr) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UCharErrorBuffer, i, c);
            cnv->UCharErrorBufferLength=i;
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

#endif
