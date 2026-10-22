// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2001-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
* File ustrtrns.cpp
*
* Modification History:
*
*   Date        Name        Description
*   9/10/2001    Ram    Creation.
******************************************************************************
*/

/*******************************************************************************
 *
 * u_strTo* and u_strFrom* APIs
 * WCS functions moved to ustr_wcs.c for better modularization
 *
 *******************************************************************************
 */


#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/utf.h"
#అన్తర్భావ "unicode/utf8.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "uassert.h"

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32WithSub(char16_t* dest U_LIFETIME_BOUND,
                      int32_t dest_సామర్థ్యం,
                      int32_t* p_Dest_నిడివి,
                      స్థిరరాశి UChar32* src,
                      int32_t src_నిడివి,
                      UChar32 subchar,
                      int32_t* p_అంకె_Substitutions,
                      UError_సంకేతం* p_Error_సంకేతం) {
    స్థిరరాశి UChar32 *src_హద్దు;
    UChar32 ch;
    char16_t *dest_హద్దు;
    char16_t *pDest;
    int32_t req_నిడివి;
    int32_t అంకె_Substitutions;

    /* args check */
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)){
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ( (src==శూన్యసూచిక && src_నిడివి!=0) || src_నిడివి < -1 ||
        (dest_సామర్థ్యం<0) || (dest == శూన్యసూచిక && dest_సామర్థ్యం > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ(p_అంకె_Substitutions != శూన్యసూచిక) {
        *p_అంకె_Substitutions = 0;
    }

    pDest = dest;
    dest_హద్దు = (dest!=శూన్యసూచిక)?(dest + dest_సామర్థ్యం):శూన్యసూచిక;
    req_నిడివి = 0;
    అంకె_Substitutions = 0;

    ఒకవేళ(src_నిడివి < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        అయితే((ch=*src) != 0 &&
              ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff))) {
            ++src;
            ఒకవేళ(pDest < dest_హద్దు) {
                *pDest++ = (char16_t)ch;
            } లేకపోతే {
                ++req_నిడివి;
            }
        }
        src_హద్దు = src;
        ఒకవేళ(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            అయితే(*++src_హద్దు != 0) {}
        }
    } లేకపోతే {
      src_హద్దు = (src!=శూన్యసూచిక)?(src + src_నిడివి):శూన్యసూచిక;
    }

    /* convert with length */
    అయితే(src < src_హద్దు) {
        ch = *src++;
        చేయి {
            /* usually "loops" once; twice only for writing subchar */
            ఒకవేళ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff)) {
                ఒకవేళ(pDest < dest_హద్దు) {
                    *pDest++ = (char16_t)ch;
                } లేకపోతే {
                    ++req_నిడివి;
                }
                ఆపు;
            } లేకపోతే ఒకవేళ(0x10000 <= ch && ch <= 0x10ffff) {
                ఒకవేళ(pDest!=శూన్యసూచిక && ((pDest + 2) <= dest_హద్దు)) {
                    *pDest++ = U16_LEAD(ch);
                    *pDest++ = U16_TRAIL(ch);
                } లేకపోతే {
                    req_నిడివి += 2;
                }
                ఆపు;
            } లేకపోతే ఒకవేళ((ch = subchar) < 0) {
                /* surrogate code point, or not a Unicode code point at all */
                *p_Error_సంకేతం = U_INVALID_CHAR_దొరికింది;
                తిరిగి శూన్యసూచిక;
            } లేకపోతే {
                ++అంకె_Substitutions;
            }
        } అయితే(నిజం);
    }

    req_నిడివి += (int32_t)(pDest - dest);
    ఒకవేళ(p_Dest_నిడివి) {
        *p_Dest_నిడివి = req_నిడివి;
    }
    ఒకవేళ(p_అంకె_Substitutions != శూన్యసూచిక) {
        *p_అంకె_Substitutions = అంకె_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, dest_సామర్థ్యం, req_నిడివి, p_Error_సంకేతం);
    
    తిరిగి dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32(char16_t* dest U_LIFETIME_BOUND,
               int32_t dest_సామర్థ్యం,
               int32_t* p_Dest_నిడివి,
               స్థిరరాశి UChar32* src,
               int32_t src_నిడివి,
               UError_సంకేతం* p_Error_సంకేతం) {
    తిరిగి u_strFromUTF32WithSub(
            dest, dest_సామర్థ్యం, p_Dest_నిడివి,
            src, src_నిడివి,
            U_SENTINEL, శూన్యసూచిక,
            p_Error_సంకేతం);
}

U_CAPI UChar32* U_EXPORT2
u_str_గమ్యం_UTF32_With_Sub(UChar32* dest U_LIFETIME_BOUND,
                    int32_t dest_సామర్థ్యం,
                    int32_t* p_Dest_నిడివి,
                    స్థిరరాశి char16_t* src,
                    int32_t src_నిడివి,
                    UChar32 subchar,
                    int32_t* p_అంకె_Substitutions,
                    UError_సంకేతం* p_Error_సంకేతం) {
    స్థిరరాశి char16_t *src_హద్దు;
    UChar32 ch;
    char16_t ch2;
    UChar32 *dest_హద్దు;
    UChar32 *pDest;
    int32_t req_నిడివి;
    int32_t అంకె_Substitutions;

    /* args check */
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)){
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ( (src==శూన్యసూచిక && src_నిడివి!=0) || src_నిడివి < -1 ||
        (dest_సామర్థ్యం<0) || (dest == శూన్యసూచిక && dest_సామర్థ్యం > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ(p_అంకె_Substitutions != శూన్యసూచిక) {
        *p_అంకె_Substitutions = 0;
    }

    pDest = dest;
    dest_హద్దు = (dest!=శూన్యసూచిక)?(dest + dest_సామర్థ్యం):శూన్యసూచిక;
    req_నిడివి = 0;
    అంకె_Substitutions = 0;

    ఒకవేళ(src_నిడివి < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        అయితే((ch=*src) != 0 && !U16_IS_SURROGATE(ch)) {
            ++src;
            ఒకవేళ(pDest < dest_హద్దు) {
                *pDest++ = ch;
            } లేకపోతే {
                ++req_నిడివి;
            }
        }
        src_హద్దు = src;
        ఒకవేళ(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            అయితే(*++src_హద్దు != 0) {}
        }
    } లేకపోతే {
        src_హద్దు = (src!=శూన్యసూచిక)?(src + src_నిడివి):శూన్యసూచిక;
    }

    /* convert with length */
    అయితే(src < src_హద్దు) {
        ch = *src++;
        ఒకవేళ(!U16_IS_SURROGATE(ch)) {
            /* write or count ch below */
        } లేకపోతే ఒకవేళ(U16_IS_SURROGATE_LEAD(ch) && src < src_హద్దు && U16_IS_TRAIL(ch2 = *src)) {
            ++src;
            ch = U16_GET_SUPPLEMENTARY(ch, ch2);
        } లేకపోతే ఒకవేళ((ch = subchar) < 0) {
            /* unpaired surrogate */
            *p_Error_సంకేతం = U_INVALID_CHAR_దొరికింది;
            తిరిగి శూన్యసూచిక;
        } లేకపోతే {
            ++అంకె_Substitutions;
        }
        ఒకవేళ(pDest < dest_హద్దు) {
            *pDest++ = ch;
        } లేకపోతే {
            ++req_నిడివి;
        }
    }

    req_నిడివి += (int32_t)(pDest - dest);
    ఒకవేళ(p_Dest_నిడివి) {
        *p_Dest_నిడివి = req_నిడివి;
    }
    ఒకవేళ(p_అంకె_Substitutions != శూన్యసూచిక) {
        *p_అంకె_Substitutions = అంకె_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChar32s(dest, dest_సామర్థ్యం, req_నిడివి, p_Error_సంకేతం);

    తిరిగి dest;
}

U_CAPI UChar32* U_EXPORT2
u_str_గమ్యం_UTF32(UChar32* dest U_LIFETIME_BOUND,
             int32_t dest_సామర్థ్యం,
             int32_t* p_Dest_నిడివి,
             స్థిరరాశి char16_t* src,
             int32_t src_నిడివి,
             UError_సంకేతం* p_Error_సంకేతం) {
    తిరిగి u_str_గమ్యం_UTF32_With_Sub(
            dest, dest_సామర్థ్యం, p_Dest_నిడివి,
            src, src_నిడివి,
            U_SENTINEL, శూన్యసూచిక,
            p_Error_సంకేతం);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                     int32_t dest_సామర్థ్యం,
                     int32_t* p_Dest_నిడివి,
                     స్థిరరాశి అక్షరం* src,
                     int32_t src_నిడివి,
                     UChar32 subchar,
                     int32_t* p_అంకె_Substitutions,
                     UError_సంకేతం* p_Error_సంకేతం) {
    /* args check */
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ( (src==శూన్యసూచిక && src_నిడివి!=0) || src_నిడివి < -1 ||
        (dest_సామర్థ్యం<0) || (dest == శూన్యసూచిక && dest_సామర్థ్యం > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ(p_అంకె_Substitutions!=శూన్యసూచిక) {
        *p_అంకె_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *p_Dest_హద్దు = dest+dest_సామర్థ్యం;
    int32_t req_నిడివి = 0;
    int32_t అంకె_Substitutions=0;

    /*
     * Inline processing of UTF-8 byte sequences:
     *
     * Byte sequences for the most common characters are handled inline in
     * the conversion loops. In order to reduce the path lengths for those
     * characters, the tests are arranged in a kind of binary search.
     * ASCII (<=0x7f) is checked first, followed by the dividing point
     * between 2- and 3-byte sequences (0xe0).
     * The 3-byte branch is tested first to speed up CJK text.
     * The compiler should combine the subtractions for the two tests for 0xe0.
     * Each branch then tests for the other end of its range.
     */

    ఒకవేళ(src_నిడివి < 0){
        /*
         * Transform a NUL-terminated string.
         * The code explicitly checks for NULs only in the lead byte position.
         * A NUL byte in the trail byte position fails the trail byte range check anyway.
         */
        int32_t i;
        UChar32 c;
        కొరకు(i = 0; (c = (uint8_t)src[i]) != 0 && (pDest < p_Dest_హద్దు);) {
            // modified copy of U8_NEXT()
            ++i;
            ఒకవేళ(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } లేకపోతే {
                uint8_t __t1, __t2;
                ఒకవేళ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } లేకపోతే ఒకవేళ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } లేకపోతే {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_తదుపరి_Char_సురక్షిత_Body((స్థిరరాశి uint8_t *)src, &(i), -1, c, -1);
                    ఒకవేళ(c<0 && (++అంకె_Substitutions, c = subchar) < 0) {
                        *p_Error_సంకేతం = U_INVALID_CHAR_దొరికింది;
                        తిరిగి శూన్యసూచిక;
                    } లేకపోతే ఒకవేళ(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } లేకపోతే {
                        *(pDest++)=U16_LEAD(c);
                        ఒకవేళ(pDest<p_Dest_హద్దు) {
                            *(pDest++)=U16_TRAIL(c);
                        } లేకపోతే {
                            req_నిడివి++;
                            ఆపు;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        అయితే((c = (uint8_t)src[i]) != 0) {
            // modified copy of U8_NEXT()
            ++i;
            ఒకవేళ(U8_IS_SINGLE(c)) {
                ++req_నిడివి;
            } లేకపోతే {
                uint8_t __t1, __t2;
                ఒకవేళ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++req_నిడివి;
                    i+=2;
                } లేకపోతే ఒకవేళ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++req_నిడివి;
                    ++(i);
                } లేకపోతే {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_తదుపరి_Char_సురక్షిత_Body((స్థిరరాశి uint8_t *)src, &(i), -1, c, -1);
                    ఒకవేళ(c<0 && (++అంకె_Substitutions, c = subchar) < 0) {
                        *p_Error_సంకేతం = U_INVALID_CHAR_దొరికింది;
                        తిరిగి శూన్యసూచిక;
                    }
                    req_నిడివి += U16_నిడివి(c);
                }
            }
        }
    } లేకపోతే /* srcLength >= 0 */ {
        /* Faster loop without ongoing checking for srcLength and pDestLimit. */
        int32_t i = 0;
        UChar32 c;
        కొరకు(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            int32_t count = (int32_t)(p_Dest_హద్దు - pDest);
            int32_t count2 = (src_నిడివి - i) / 3;
            ఒకవేళ(count > count2) {
                count = count2; /* min(remaining dest, remaining src/3) */
            }
            ఒకవేళ(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                ఆపు;
            }

            చేయి {
                // modified copy of U8_NEXT()
                c = (uint8_t)src[i++];
                ఒకవేళ(U8_IS_SINGLE(c)) {
                    *pDest++=(char16_t)c;
                } లేకపోతే {
                    uint8_t __t1, __t2;
                    ఒకవేళ( /* handle U+0800..U+FFFF inline */
                            (0xe0<=(c) && (c)<0xf0) &&
                            ((i)+1)<src_నిడివి &&
                            U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                            (__t2=src[(i)+1]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                        i+=2;
                    } లేకపోతే ఒకవేళ( /* handle U+0080..U+07FF inline */
                            ((c)<0xe0 && (c)>=0xc2) &&
                            ((i)!=src_నిడివి) &&
                            (__t1=src[i]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0x1f)<<6)|__t1;
                        ++(i);
                    } లేకపోతే {
                        ఒకవేళ(c >= 0xf0 || subchar > 0xffff) {
                            // We may read up to four bytes and write up to two UChars,
                            // which we didn't account for with computing count,
                            // so we adjust it here.
                            ఒకవేళ(--count == 0) {
                                --i;  // back out byte c
                                ఆపు;
                            }
                        }

                        /* function call for "complicated" and error cases */
                        (c)=utf8_తదుపరి_Char_సురక్షిత_Body((స్థిరరాశి uint8_t *)src, &(i), src_నిడివి, c, -1);
                        ఒకవేళ(c<0 && (++అంకె_Substitutions, c = subchar) < 0) {
                            *p_Error_సంకేతం = U_INVALID_CHAR_దొరికింది;
                            తిరిగి శూన్యసూచిక;
                        } లేకపోతే ఒకవేళ(c<=0xFFFF) {
                            *(pDest++)=(char16_t)c;
                        } లేకపోతే {
                            *(pDest++)=U16_LEAD(c);
                            *(pDest++)=U16_TRAIL(c);
                        }
                    }
                }
            } అయితే(--count > 0);
        }

        అయితే(i < src_నిడివి && (pDest < p_Dest_హద్దు)) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            ఒకవేళ(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } లేకపోతే {
                uint8_t __t1, __t2;
                ఒకవేళ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<src_నిడివి &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } లేకపోతే ఒకవేళ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=src_నిడివి) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } లేకపోతే {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_తదుపరి_Char_సురక్షిత_Body((స్థిరరాశి uint8_t *)src, &(i), src_నిడివి, c, -1);
                    ఒకవేళ(c<0 && (++అంకె_Substitutions, c = subchar) < 0) {
                        *p_Error_సంకేతం = U_INVALID_CHAR_దొరికింది;
                        తిరిగి శూన్యసూచిక;
                    } లేకపోతే ఒకవేళ(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } లేకపోతే {
                        *(pDest++)=U16_LEAD(c);
                        ఒకవేళ(pDest<p_Dest_హద్దు) {
                            *(pDest++)=U16_TRAIL(c);
                        } లేకపోతే {
                            req_నిడివి++;
                            ఆపు;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        అయితే(i < src_నిడివి) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            ఒకవేళ(U8_IS_SINGLE(c)) {
                ++req_నిడివి;
            } లేకపోతే {
                uint8_t __t1, __t2;
                ఒకవేళ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<src_నిడివి &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++req_నిడివి;
                    i+=2;
                } లేకపోతే ఒకవేళ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=src_నిడివి) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++req_నిడివి;
                    ++(i);
                } లేకపోతే {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_తదుపరి_Char_సురక్షిత_Body((స్థిరరాశి uint8_t *)src, &(i), src_నిడివి, c, -1);
                    ఒకవేళ(c<0 && (++అంకె_Substitutions, c = subchar) < 0) {
                        *p_Error_సంకేతం = U_INVALID_CHAR_దొరికింది;
                        తిరిగి శూన్యసూచిక;
                    }
                    req_నిడివి += U16_నిడివి(c);
                }
            }
        }
    }

    req_నిడివి+=(int32_t)(pDest - dest);

    ఒకవేళ(p_అంకె_Substitutions!=శూన్యసూచిక) {
        *p_అంకె_Substitutions=అంకె_Substitutions;
    }

    ఒకవేళ(p_Dest_నిడివి){
        *p_Dest_నిడివి = req_నిడివి;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_సామర్థ్యం,req_నిడివి,p_Error_సంకేతం);

    తిరిగి dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8(char16_t* dest U_LIFETIME_BOUND,
              int32_t dest_సామర్థ్యం,
              int32_t* p_Dest_నిడివి,
              స్థిరరాశి అక్షరం* src,
              int32_t src_నిడివి,
              UError_సంకేతం* p_Error_సంకేతం) {
    తిరిగి u_strFromUTF8WithSub(
            dest, dest_సామర్థ్యం, p_Dest_నిడివి,
            src, src_నిడివి,
            U_SENTINEL, శూన్యసూచిక,
            p_Error_సంకేతం);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8Lenient(char16_t* dest U_LIFETIME_BOUND,
                     int32_t dest_సామర్థ్యం,
                     int32_t* p_Dest_నిడివి,
                     స్థిరరాశి అక్షరం* src,
                     int32_t src_నిడివి,
                     UError_సంకేతం* p_Error_సంకేతం) {
    char16_t *pDest = dest;
    UChar32 ch;
    int32_t req_నిడివి = 0;
    uint8_t* pSrc = (uint8_t*) src;

    /* args check */
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)){
        తిరిగి శూన్యసూచిక;
    }
        
    ఒకవేళ( (src==శూన్యసూచిక && src_నిడివి!=0) || src_నిడివి < -1 ||
        (dest_సామర్థ్యం<0) || (dest == శూన్యసూచిక && dest_సామర్థ్యం > 0)
    ) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ(src_నిడివి < 0) {
        /* Transform a NUL-terminated string. */
        char16_t *p_Dest_హద్దు = (dest!=శూన్యసూచిక)?(dest+dest_సామర్థ్యం):శూన్యసూచిక;
        uint8_t t1, t2, t3; /* trail bytes */

        అయితే(((ch = *pSrc) != 0) && (pDest < p_Dest_హద్దు)) {
            ఒకవేళ(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ++pSrc;
                కొనసాగు;
            } లేకపోతే ఒకవేళ(ch < 0xe0) { /* U+0080..U+07FF */
                ఒకవేళ((t1 = pSrc[1]) != 0) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + t1 - 0x3080);
                    pSrc += 2;
                    కొనసాగు;
                }
            } లేకపోతే ఒకవేళ(ch < 0xf0) { /* U+0800..U+FFFF */
                ఒకవేళ((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 12) + (t1 << 6) + t2 - 0x2080);
                    pSrc += 3;
                    కొనసాగు;
                }
            } లేకపోతే /* f0..f4 */ { /* U+10000..U+10FFFF */
                ఒకవేళ((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0 && (t3 = pSrc[3]) != 0) {
                    pSrc += 4;
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (t1 << 12) + (t2 << 6) + t3 - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    ఒకవేళ(pDest < p_Dest_హద్దు) {
                        *(pDest++) = U16_TRAIL(ch);
                    } లేకపోతే {
                        req_నిడివి = 1;
                        ఆపు;
                    }
                    కొనసాగు;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            అయితే(*++pSrc != 0) {}
            ఆపు;
        }

        /* Pre-flight the rest of the string. */
        అయితే((ch = *pSrc) != 0) {
            ఒకవేళ(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                ++req_నిడివి;
                ++pSrc;
                కొనసాగు;
            } లేకపోతే ఒకవేళ(ch < 0xe0) { /* U+0080..U+07FF */
                ఒకవేళ(pSrc[1] != 0) {
                    ++req_నిడివి;
                    pSrc += 2;
                    కొనసాగు;
                }
            } లేకపోతే ఒకవేళ(ch < 0xf0) { /* U+0800..U+FFFF */
                ఒకవేళ(pSrc[1] != 0 && pSrc[2] != 0) {
                    ++req_నిడివి;
                    pSrc += 3;
                    కొనసాగు;
                }
            } లేకపోతే /* f0..f4 */ { /* U+10000..U+10FFFF */
                ఒకవేళ(pSrc[1] != 0 && pSrc[2] != 0 && pSrc[3] != 0) {
                    req_నిడివి += 2;
                    pSrc += 4;
                    కొనసాగు;
                }
            }

            /* truncated character at the end */
            ++req_నిడివి;
            ఆపు;
        }
    } లేకపోతే /* srcLength >= 0 */ {
      స్థిరరాశి uint8_t *p_Src_హద్దు = (pSrc!=శూన్యసూచిక)?(pSrc + src_నిడివి):శూన్యసూచిక;

        /*
         * This function requires that if srcLength is given, then it must be
         * destCapatity >= srcLength so that we need not check for
         * destination buffer overflow in the loop.
         */
        ఒకవేళ(dest_సామర్థ్యం < src_నిడివి) {
            ఒకవేళ(p_Dest_నిడివి != శూన్యసూచిక) {
                *p_Dest_నిడివి = src_నిడివి; /* this likely overestimates the true destLength! */
            }
            *p_Error_సంకేతం = U_బఫరు_అధికప్రవాహం_ERROR;
            తిరిగి శూన్యసూచిక;
        }

        ఒకవేళ((p_Src_హద్దు - pSrc) >= 4) {
            p_Src_హద్దు -= 3; /* temporarily reduce pSrcLimit */

            /* in this loop, we can always access at least 4 bytes, up to pSrc+3 */
            చేయి {
                ch = *pSrc++;
                ఒకవేళ(ch < 0xc0) {
                    /*
                     * ASCII, or a trail byte in lead position which is treated like
                     * a single-byte sequence for better character boundary
                     * resynchronization after illegal sequences.
                     */
                    *pDest++=(char16_t)ch;
                } లేకపోతే ఒకవేళ(ch < 0xe0) { /* U+0080..U+07FF */
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                } లేకపోతే ఒకవేళ(ch < 0xf0) { /* U+0800..U+FFFF */
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                } లేకపోతే /* f0..f4 */ { /* U+10000..U+10FFFF */
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                }
            } అయితే(pSrc < p_Src_హద్దు);

            p_Src_హద్దు += 3; /* restore original pSrcLimit */
        }

        అయితే(pSrc < p_Src_హద్దు) {
            ch = *pSrc++;
            ఒకవేళ(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                కొనసాగు;
            } లేకపోతే ఒకవేళ(ch < 0xe0) { /* U+0080..U+07FF */
                ఒకవేళ(pSrc < p_Src_హద్దు) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                    కొనసాగు;
                }
            } లేకపోతే ఒకవేళ(ch < 0xf0) { /* U+0800..U+FFFF */
                ఒకవేళ((p_Src_హద్దు - pSrc) >= 2) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                    pSrc += 3;
                    కొనసాగు;
                }
            } లేకపోతే /* f0..f4 */ { /* U+10000..U+10FFFF */
                ఒకవేళ((p_Src_హద్దు - pSrc) >= 3) {
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                    pSrc += 4;
                    కొనసాగు;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            ఆపు;
        }
    }

    req_నిడివి+=(int32_t)(pDest - dest);

    ఒకవేళ(p_Dest_నిడివి){
        *p_Dest_నిడివి = req_నిడివి;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_సామర్థ్యం,req_నిడివి,p_Error_సంకేతం);

    తిరిగి dest;
}

స్థిర అంతఃవరుస uint8_t *
_appendUTF8(uint8_t *pDest, UChar32 c) {
    /* it is 0<=c<=0x10ffff and not a surrogate if called by a validating function */
    ఒకవేళ((c)<=0x7f) {
        *pDest++ = static_cast<uint8_t>(c);
    } లేకపోతే ఒకవేళ(c<=0x7ff) {
        *pDest++ = static_cast<uint8_t>((c >> 6) | 0xc0);
        *pDest++ = static_cast<uint8_t>((c & 0x3f) | 0x80);
    } లేకపోతే ఒకవేళ(c<=0xffff) {
        *pDest++ = static_cast<uint8_t>((c >> 12) | 0xe0);
        *pDest++ = static_cast<uint8_t>(((c >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    } లేకపోతే /* if((uint32_t)(c)<=0x10ffff) */ {
        *pDest++ = static_cast<uint8_t>(((c) >> 18) | 0xf0);
        *pDest++ = static_cast<uint8_t>((((c) >> 12) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>((((c) >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    }
    తిరిగి pDest;
}

U_CAPI అక్షరం* U_EXPORT2
u_str_గమ్యం_UTF8_With_Sub(అక్షరం* dest U_LIFETIME_BOUND,
                   int32_t dest_సామర్థ్యం,
                   int32_t* p_Dest_నిడివి,
                   స్థిరరాశి char16_t* pSrc,
                   int32_t src_నిడివి,
                   UChar32 subchar,
                   int32_t* p_అంకె_Substitutions,
                   UError_సంకేతం* p_Error_సంకేతం) {
    int32_t req_నిడివి=0;
    uint32_t ch=0,ch2=0;
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *p_Dest_హద్దు = (pDest!=శూన్యసూచిక)?(pDest + dest_సామర్థ్యం):శూన్యసూచిక;
    int32_t అంకె_Substitutions;

    /* args check */
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)){
        తిరిగి శూన్యసూచిక;
    }
        
    ఒకవేళ( (pSrc==శూన్యసూచిక && src_నిడివి!=0) || src_నిడివి < -1 ||
        (dest_సామర్థ్యం<0) || (dest == శూన్యసూచిక && dest_సామర్థ్యం > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ(p_అంకె_Substitutions!=శూన్యసూచిక) {
        *p_అంకె_Substitutions=0;
    }
    అంకె_Substitutions=0;

    ఒకవేళ(src_నిడివి==-1) {
        అయితే((ch=*pSrc)!=0) {
            ++pSrc;
            ఒకవేళ(ch <= 0x7f) {
                ఒకవేళ(pDest<p_Dest_హద్దు) {
                    *pDest++ = (uint8_t)ch;
                } లేకపోతే {
                    req_నిడివి = 1;
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ(ch <= 0x7ff) {
                ఒకవేళ((p_Dest_హద్దు - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } లేకపోతే {
                    req_నిడివి = 2;
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ(ch <= 0xd7ff || ch >= 0xe000) {
                ఒకవేళ((p_Dest_హద్దు - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } లేకపోతే {
                    req_నిడివి = 3;
                    ఆపు;
                }
            } లేకపోతే /* ch is a surrogate */ {
                int32_t length;

                /*need not check for NUL because NUL fails U16_IS_TRAIL() anyway*/
                ఒకవేళ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } లేకపోతే ఒకవేళ(subchar>=0) {
                    ch=subchar;
                    ++అంకె_Substitutions;
                } లేకపోతే {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *p_Error_సంకేతం = U_INVALID_CHAR_దొరికింది;
                    తిరిగి శూన్యసూచిక;
                }

                length = U8_నిడివి(ch);
                ఒకవేళ((p_Dest_హద్దు - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } లేకపోతే {
                    req_నిడివి = length;
                    ఆపు;
                }
            }
        }
        అయితే((ch=*pSrc++)!=0) {
            ఒకవేళ(ch<=0x7f) {
                ++req_నిడివి;
            } లేకపోతే ఒకవేళ(ch<=0x7ff) {
                req_నిడివి+=2;
            } లేకపోతే ఒకవేళ(!U16_IS_SURROGATE(ch)) {
                req_నిడివి+=3;
            } లేకపోతే ఒకవేళ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                req_నిడివి+=4;
            } లేకపోతే ఒకవేళ(subchar>=0) {
                req_నిడివి+=U8_నిడివి(subchar);
                ++అంకె_Substitutions;
            } లేకపోతే {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *p_Error_సంకేతం = U_INVALID_CHAR_దొరికింది;
                తిరిగి శూన్యసూచిక;
            }
        }
    } లేకపోతే {
        స్థిరరాశి char16_t *p_Src_హద్దు = (pSrc!=శూన్యసూచిక)?(pSrc+src_నిడివి):శూన్యసూచిక;
        int32_t count;

        /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
        కొరకు(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            count = (int32_t)((p_Dest_హద్దు - pDest) / 3);
            src_నిడివి = (int32_t)(p_Src_హద్దు - pSrc);
            ఒకవేళ(count > src_నిడివి) {
                count = src_నిడివి; /* min(remaining dest/3, remaining src) */
            }
            ఒకవేళ(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                ఆపు;
            }
            చేయి {
                ch=*pSrc++;
                ఒకవేళ(ch <= 0x7f) {
                    *pDest++ = (uint8_t)ch;
                } లేకపోతే ఒకవేళ(ch <= 0x7ff) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } లేకపోతే ఒకవేళ(ch <= 0xd7ff || ch >= 0xe000) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } లేకపోతే /* ch is a surrogate */ {
                    /*
                     * We will read two UChars and probably output four bytes,
                     * which we didn't account for with computing count,
                     * so we adjust it here.
                     */
                    ఒకవేళ(--count == 0) {
                        --pSrc; /* undo ch=*pSrc++ for the lead surrogate */
                        ఆపు;  /* recompute count */
                    }

                    ఒకవేళ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                        ++pSrc;
                        ch=U16_GET_SUPPLEMENTARY(ch, ch2);

                        /* writing 4 bytes per 2 UChars is ok */
                        *pDest++=(uint8_t)((ch>>18)|0xf0);
                        *pDest++=(uint8_t)(((ch>>12)&0x3f)|0x80);
                        *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                        *pDest++=(uint8_t)((ch&0x3f)|0x80);
                    } లేకపోతే  {
                        /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                        ఒకవేళ(subchar>=0) {
                            ch=subchar;
                            ++అంకె_Substitutions;
                        } లేకపోతే {
                            *p_Error_సంకేతం = U_INVALID_CHAR_దొరికింది;
                            తిరిగి శూన్యసూచిక;
                        }

                        /* convert and append*/
                        pDest=_appendUTF8(pDest, ch);
                    }
                }
            } అయితే(--count > 0);
        }

        అయితే(pSrc<p_Src_హద్దు) {
            ch=*pSrc++;
            ఒకవేళ(ch <= 0x7f) {
                ఒకవేళ(pDest<p_Dest_హద్దు) {
                    *pDest++ = (uint8_t)ch;
                } లేకపోతే {
                    req_నిడివి = 1;
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ(ch <= 0x7ff) {
                ఒకవేళ((p_Dest_హద్దు - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } లేకపోతే {
                    req_నిడివి = 2;
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ(ch <= 0xd7ff || ch >= 0xe000) {
                ఒకవేళ((p_Dest_హద్దు - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } లేకపోతే {
                    req_నిడివి = 3;
                    ఆపు;
                }
            } లేకపోతే /* ch is a surrogate */ {
                int32_t length;

                ఒకవేళ(U16_IS_SURROGATE_LEAD(ch) && pSrc<p_Src_హద్దు && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } లేకపోతే ఒకవేళ(subchar>=0) {
                    ch=subchar;
                    ++అంకె_Substitutions;
                } లేకపోతే {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *p_Error_సంకేతం = U_INVALID_CHAR_దొరికింది;
                    తిరిగి శూన్యసూచిక;
                }

                length = U8_నిడివి(ch);
                ఒకవేళ((p_Dest_హద్దు - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } లేకపోతే {
                    req_నిడివి = length;
                    ఆపు;
                }
            }
        }
        అయితే(pSrc<p_Src_హద్దు) {
            ch=*pSrc++;
            ఒకవేళ(ch<=0x7f) {
                ++req_నిడివి;
            } లేకపోతే ఒకవేళ(ch<=0x7ff) {
                req_నిడివి+=2;
            } లేకపోతే ఒకవేళ(!U16_IS_SURROGATE(ch)) {
                req_నిడివి+=3;
            } లేకపోతే ఒకవేళ(U16_IS_SURROGATE_LEAD(ch) && pSrc<p_Src_హద్దు && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                req_నిడివి+=4;
            } లేకపోతే ఒకవేళ(subchar>=0) {
                req_నిడివి+=U8_నిడివి(subchar);
                ++అంకె_Substitutions;
            } లేకపోతే {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *p_Error_సంకేతం = U_INVALID_CHAR_దొరికింది;
                తిరిగి శూన్యసూచిక;
            }
        }
    }

    req_నిడివి+=(int32_t)(pDest - (uint8_t *)dest);

    ఒకవేళ(p_అంకె_Substitutions!=శూన్యసూచిక) {
        *p_అంకె_Substitutions=అంకె_Substitutions;
    }

    ఒకవేళ(p_Dest_నిడివి){
        *p_Dest_నిడివి = req_నిడివి;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, dest_సామర్థ్యం, req_నిడివి, p_Error_సంకేతం);
    తిరిగి dest;
}

U_CAPI అక్షరం* U_EXPORT2
u_str_గమ్యం_UTF8(అక్షరం* dest U_LIFETIME_BOUND,
            int32_t dest_సామర్థ్యం,
            int32_t* p_Dest_నిడివి,
            స్థిరరాశి char16_t* pSrc,
            int32_t src_నిడివి,
            UError_సంకేతం* p_Error_సంకేతం) {
    తిరిగి u_str_గమ్యం_UTF8_With_Sub(
            dest, dest_సామర్థ్యం, p_Dest_నిడివి,
            pSrc, src_నిడివి,
            U_SENTINEL, శూన్యసూచిక,
            p_Error_సంకేతం);
}

U_CAPI char16_t* U_EXPORT2
u_strFromJavaModifiedUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                                 int32_t dest_సామర్థ్యం,
                                 int32_t* p_Dest_నిడివి,
                                 స్థిరరాశి అక్షరం* src,
                                 int32_t src_నిడివి,
                                 UChar32 subchar,
                                 int32_t* p_అంకె_Substitutions,
                                 UError_సంకేతం* p_Error_సంకేతం) {
    /* args check */
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ( (src==శూన్యసూచిక && src_నిడివి!=0) || src_నిడివి < -1 ||
        (dest==శూన్యసూచిక && dest_సామర్థ్యం!=0) || dest_సామర్థ్యం<0 ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ(p_అంకె_Substitutions!=శూన్యసూచిక) {
        *p_అంకె_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *p_Dest_హద్దు = dest+dest_సామర్థ్యం;
    int32_t req_నిడివి = 0;
    int32_t అంకె_Substitutions=0;

    ఒకవేళ(src_నిడివి < 0) {
        /*
         * Transform a NUL-terminated ASCII string.
         * Handle non-ASCII strings with slower code.
         */
        UChar32 c;
        అయితే(((c = (uint8_t)*src) != 0) && c <= 0x7f && (pDest < p_Dest_హద్దు)) {
            *pDest++=(char16_t)c;
            ++src;
        }
        ఒకవేళ(c == 0) {
            req_నిడివి=(int32_t)(pDest - dest);
            ఒకవేళ(p_Dest_నిడివి) {
                *p_Dest_నిడివి = req_నిడివి;
            }

            /* Terminate the buffer */
            u_terminateUChars(dest, dest_సామర్థ్యం, req_నిడివి, p_Error_సంకేతం);
            తిరిగి dest;
        }
        src_నిడివి = static_cast<int32_t>(uprv_strlen(src));
    }

    /* Faster loop without ongoing checking for srcLength and pDestLimit. */
    UChar32 ch;
    uint8_t t1, t2;
    int32_t i = 0;
    కొరకు(;;) {
        int32_t count = (int32_t)(p_Dest_హద్దు - pDest);
        int32_t count2 = src_నిడివి - i;
        ఒకవేళ(count >= count2 && src_నిడివి > 0 && U8_IS_SINGLE(*src)) {
            /* fast ASCII loop */
            int32_t ప్రారంభం = i;
            uint8_t b;
            అయితే(i < src_నిడివి && U8_IS_SINGLE(b = src[i])) {
                *pDest++=b;
                ++i;
            }
            int32_t delta = i - ప్రారంభం;
            count -= delta;
            count2 -= delta;
        }
        /*
         * Each iteration of the inner loop progresses by at most 3 UTF-8
         * bytes and one char16_t.
         */
        ఒకవేళ(subchar > 0xFFFF) {
            ఆపు;
        }
        count2 /= 3;
        ఒకవేళ(count > count2) {
            count = count2; /* min(remaining dest, remaining src/3) */
        }
        ఒకవేళ(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            ఆపు;
        }
        చేయి {
            ch = (uint8_t)src[i++];
            ఒకవేళ(U8_IS_SINGLE(ch)) {
                *pDest++=(char16_t)ch;
            } లేకపోతే {
                ఒకవేళ(ch >= 0xe0) {
                    ఒకవేళ( /* handle U+0000..U+FFFF inline */
                        ch <= 0xef &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                        (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                    ) {
                        /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                        *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                        i += 2;
                        కొనసాగు;
                    }
                } లేకపోతే {
                    ఒకవేళ( /* handle U+0000..U+07FF inline */
                        ch >= 0xc0 &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                    ) {
                        *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                        ++i;
                        కొనసాగు;
                    }
                }

                ఒకవేళ(subchar < 0) {
                    *p_Error_సంకేతం = U_INVALID_CHAR_దొరికింది;
                    తిరిగి శూన్యసూచిక;
                } లేకపోతే ఒకవేళ(subchar > 0xffff && --count == 0) {
                    /*
                     * We need to write two UChars, adjusted count for that,
                     * and ran out of space.
                     */
                    --i;  // back out byte ch
                    ఆపు;
                } లేకపోతే {
                    /* function call for error cases */
                    utf8_తదుపరి_Char_సురక్షిత_Body((స్థిరరాశి uint8_t *)src, &(i), src_నిడివి, ch, -1);
                    ++అంకె_Substitutions;
                    *(pDest++)=(char16_t)subchar;
                }
            }
        } అయితే(--count > 0);
    }

    అయితే(i < src_నిడివి && (pDest < p_Dest_హద్దు)) {
        ch = (uint8_t)src[i++];
        ఒకవేళ(U8_IS_SINGLE(ch)){
            *pDest++=(char16_t)ch;
        } లేకపోతే {
            ఒకవేళ(ch >= 0xe0) {
                ఒకవేళ( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < src_నిడివి &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                    (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                ) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                    i += 2;
                    కొనసాగు;
                }
            } లేకపోతే {
                ఒకవేళ( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < src_నిడివి &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                ) {
                    *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                    ++i;
                    కొనసాగు;
                }
            }

            ఒకవేళ(subchar < 0) {
                *p_Error_సంకేతం = U_INVALID_CHAR_దొరికింది;
                తిరిగి శూన్యసూచిక;
            } లేకపోతే {
                /* function call for error cases */
                utf8_తదుపరి_Char_సురక్షిత_Body((స్థిరరాశి uint8_t *)src, &(i), src_నిడివి, ch, -1);
                ++అంకె_Substitutions;
                ఒకవేళ(subchar<=0xFFFF) {
                    *(pDest++)=(char16_t)subchar;
                } లేకపోతే {
                    *(pDest++)=U16_LEAD(subchar);
                    ఒకవేళ(pDest<p_Dest_హద్దు) {
                        *(pDest++)=U16_TRAIL(subchar);
                    } లేకపోతే {
                        req_నిడివి++;
                        ఆపు;
                    }
                }
            }
        }
    }

    /* Pre-flight the rest of the string. */
    అయితే(i < src_నిడివి) {
        ch = (uint8_t)src[i++];
        ఒకవేళ(U8_IS_SINGLE(ch)) {
            req_నిడివి++;
        } లేకపోతే {
            ఒకవేళ(ch >= 0xe0) {
                ఒకవేళ( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < src_నిడివి &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f &&
                    (uint8_t)(src[i+1] - 0x80) <= 0x3f
                ) {
                    req_నిడివి++;
                    i += 2;
                    కొనసాగు;
                }
            } లేకపోతే {
                ఒకవేళ( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < src_నిడివి &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f
                ) {
                    req_నిడివి++;
                    ++i;
                    కొనసాగు;
                }
            }

            ఒకవేళ(subchar < 0) {
                *p_Error_సంకేతం = U_INVALID_CHAR_దొరికింది;
                తిరిగి శూన్యసూచిక;
            } లేకపోతే {
                /* function call for error cases */
                utf8_తదుపరి_Char_సురక్షిత_Body((స్థిరరాశి uint8_t *)src, &(i), src_నిడివి, ch, -1);
                ++అంకె_Substitutions;
                req_నిడివి+=U16_నిడివి(ch);
            }
        }
    }

    ఒకవేళ(p_అంకె_Substitutions!=శూన్యసూచిక) {
        *p_అంకె_Substitutions=అంకె_Substitutions;
    }

    req_నిడివి+=(int32_t)(pDest - dest);
    ఒకవేళ(p_Dest_నిడివి) {
        *p_Dest_నిడివి = req_నిడివి;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, dest_సామర్థ్యం, req_నిడివి, p_Error_సంకేతం);
    తిరిగి dest;
}

U_CAPI అక్షరం* U_EXPORT2
u_str_గమ్యం_Java_Modified_UTF8(అక్షరం* dest U_LIFETIME_BOUND,
                        int32_t dest_సామర్థ్యం,
                        int32_t* p_Dest_నిడివి,
                        స్థిరరాశి char16_t* src,
                        int32_t src_నిడివి,
                        UError_సంకేతం* p_Error_సంకేతం) {
    int32_t req_నిడివి=0;
    uint32_t ch=0;
    స్థిరరాశి char16_t *p_Src_హద్దు;
    int32_t count;

    /* args check */
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)){
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ( (src==శూన్యసూచిక && src_నిడివి!=0) || src_నిడివి < -1 ||
        (dest==శూన్యసూచిక && dest_సామర్థ్యం!=0) || dest_సామర్థ్యం<0
    ) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *p_Dest_హద్దు = pDest + dest_సామర్థ్యం;

    ఒకవేళ(src_నిడివి==-1) {
        /* Convert NUL-terminated ASCII, then find the string length. */
        అయితే((ch=*src)<=0x7f && ch != 0 && pDest<p_Dest_హద్దు) {
            *pDest++ = (uint8_t)ch;
            ++src;
        }
        ఒకవేళ(ch == 0) {
            req_నిడివి=(int32_t)(pDest - (uint8_t *)dest);
            ఒకవేళ(p_Dest_నిడివి) {
                *p_Dest_నిడివి = req_నిడివి;
            }

            /* Terminate the buffer */
            u_terminateChars(dest, dest_సామర్థ్యం, req_నిడివి, p_Error_సంకేతం);
            తిరిగి dest;
        }
        src_నిడివి = u_strlen(src);
    }

    /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
    p_Src_హద్దు = (src!=శూన్యసూచిక)?(src+src_నిడివి):శూన్యసూచిక;
    కొరకు(;;) {
        count = (int32_t)(p_Dest_హద్దు - pDest);
        src_నిడివి = (int32_t)(p_Src_హద్దు - src);
        ఒకవేళ(count >= src_నిడివి && src_నిడివి > 0 && *src <= 0x7f) {
            /* fast ASCII loop */
            స్థిరరాశి char16_t *prevSrc = src;
            int32_t delta;
            అయితే(src < p_Src_హద్దు && (ch = *src) <= 0x7f && ch != 0) {
                *pDest++=(uint8_t)ch;
                ++src;
            }
            delta = (int32_t)(src - prevSrc);
            count -= delta;
            src_నిడివి -= delta;
        }
        /*
         * Each iteration of the inner loop progresses by at most 3 UTF-8
         * bytes and one char16_t.
         */
        count /= 3;
        ఒకవేళ(count > src_నిడివి) {
            count = src_నిడివి; /* min(remaining dest/3, remaining src) */
        }
        ఒకవేళ(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            ఆపు;
        }
        చేయి {
            ch=*src++;
            ఒకవేళ(ch <= 0x7f && ch != 0) {
                *pDest++ = (uint8_t)ch;
            } లేకపోతే ఒకవేళ(ch <= 0x7ff) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } లేకపోతే {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            }
        } అయితే(--count > 0);
    }

    అయితే(src<p_Src_హద్దు) {
        ch=*src++;
        ఒకవేళ(ch <= 0x7f && ch != 0) {
            ఒకవేళ(pDest<p_Dest_హద్దు) {
                *pDest++ = (uint8_t)ch;
            } లేకపోతే {
                req_నిడివి = 1;
                ఆపు;
            }
        } లేకపోతే ఒకవేళ(ch <= 0x7ff) {
            ఒకవేళ((p_Dest_హద్దు - pDest) >= 2) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } లేకపోతే {
                req_నిడివి = 2;
                ఆపు;
            }
        } లేకపోతే {
            ఒకవేళ((p_Dest_హద్దు - pDest) >= 3) {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } లేకపోతే {
                req_నిడివి = 3;
                ఆపు;
            }
        }
    }
    అయితే(src<p_Src_హద్దు) {
        ch=*src++;
        ఒకవేళ(ch <= 0x7f && ch != 0) {
            ++req_నిడివి;
        } లేకపోతే ఒకవేళ(ch<=0x7ff) {
            req_నిడివి+=2;
        } లేకపోతే {
            req_నిడివి+=3;
        }
    }

    req_నిడివి+=(int32_t)(pDest - (uint8_t *)dest);
    ఒకవేళ(p_Dest_నిడివి){
        *p_Dest_నిడివి = req_నిడివి;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, dest_సామర్థ్యం, req_నిడివి, p_Error_సంకేతం);
    తిరిగి dest;
}
