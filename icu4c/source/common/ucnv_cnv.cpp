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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_CONVERSION

#അന്തര്ഭാവ "unicode/ucnv_err.h"
#അന്തര്ഭാവ "unicode/ucnv.h"
#അന്തര്ഭാവ "unicode/uset.h"
#അന്തര്ഭാവ "ucnv_cnv.h"
#അന്തര്ഭാവ "ucnv_bld.h"
#അന്തര്ഭാവ "cmemory.h"

U_CFUNC ശൂന്യം
ucnv_getCompleteUnicodeSet(സ്ഥിരം UConverter *cnv,
                   സ്ഥിരം USetAdder *sa,
                   UConverterUnicodeSet which,
                   UErrorCode *pErrorCode) {
    (ശൂന്യം)cnv;
    (ശൂന്യം)which;
    (ശൂന്യം)pErrorCode;
    sa->addRange(sa->set, 0, 0x10ffff);
}

U_CFUNC ശൂന്യം
ucnv_getNonSurrogateUnicodeSet(സ്ഥിരം UConverter *cnv,
                               സ്ഥിരം USetAdder *sa,
                               UConverterUnicodeSet which,
                               UErrorCode *pErrorCode) {
    (ശൂന്യം)cnv;
    (ശൂന്യം)which;
    (ശൂന്യം)pErrorCode;
    sa->addRange(sa->set, 0, 0xd7ff);
    sa->addRange(sa->set, 0xe000, 0x10ffff);
}

U_CFUNC ശൂന്യം
ucnv_fromUWriteBytes(UConverter *cnv,
                     സ്ഥിരം അക്ഷരം *bytes, int32_t length,
                     അക്ഷരം **target, സ്ഥിരം അക്ഷരം *targetLimit,
                     int32_t **offsets,
                     int32_t sourceIndex,
                     UErrorCode *pErrorCode) {
    അക്ഷരം *t=*target;
    int32_t *o;

    /* write bytes */
    എങ്കിൽ(offsets==nullptr || (o=*offsets)==nullptr) {
        ആകുമ്പോൾ(length>0 && t<targetLimit) {
            *t++=*bytes++;
            --length;
        }
    } അല്ലെങ്കിൽ {
        /* output with offsets */
        ആകുമ്പോൾ(length>0 && t<targetLimit) {
            *t++=*bytes++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    എങ്കിൽ(length>0) {
        എങ്കിൽ(cnv!=nullptr) {
            t=(അക്ഷരം *)cnv->charErrorBuffer;
            cnv->charErrorBufferLength=(int8_t)length;
            ചെയ്യുക {
                *t++=(uint8_t)*bytes++;
            } ആകുമ്പോൾ(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC ശൂന്യം
ucnv_toUWriteUChars(UConverter *cnv,
                    സ്ഥിരം char16_t *uchars, int32_t length,
                    char16_t **target, സ്ഥിരം char16_t *targetLimit,
                    int32_t **offsets,
                    int32_t sourceIndex,
                    UErrorCode *pErrorCode) {
    char16_t *t=*target;
    int32_t *o;

    /* write UChars */
    എങ്കിൽ(offsets==nullptr || (o=*offsets)==nullptr) {
        ആകുമ്പോൾ(length>0 && t<targetLimit) {
            *t++=*uchars++;
            --length;
        }
    } അല്ലെങ്കിൽ {
        /* output with offsets */
        ആകുമ്പോൾ(length>0 && t<targetLimit) {
            *t++=*uchars++;
            *o++=sourceIndex;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    എങ്കിൽ(length>0) {
        എങ്കിൽ(cnv!=nullptr) {
            t=cnv->UCharErrorBuffer;
            cnv->UCharErrorBufferLength=(int8_t)length;
            ചെയ്യുക {
                *t++=*uchars++;
            } ആകുമ്പോൾ(--length>0);
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

U_CFUNC ശൂന്യം
ucnv_toUWriteCodePoint(UConverter *cnv,
                       UChar32 c,
                       char16_t **target, സ്ഥിരം char16_t *targetLimit,
                       int32_t **offsets,
                       int32_t sourceIndex,
                       UErrorCode *pErrorCode) {
    char16_t *t;
    int32_t *o;

    t=*target;

    എങ്കിൽ(t<targetLimit) {
        എങ്കിൽ(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } അല്ലെങ്കിൽ /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            എങ്കിൽ(t<targetLimit) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        എങ്കിൽ(offsets!=nullptr && (o=*offsets)!=nullptr) {
            *o++=sourceIndex;
            എങ്കിൽ((*target+1)<t) {
                *o++=sourceIndex;
            }
            *offsets=o;
        }
    }

    *target=t;

    /* write overflow from c */
    എങ്കിൽ(c>=0) {
        എങ്കിൽ(cnv!=nullptr) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UCharErrorBuffer, i, c);
            cnv->UCharErrorBufferLength=i;
        }
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
}

#endif
