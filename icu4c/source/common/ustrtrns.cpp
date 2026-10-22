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


#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "uassert.h"

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32WithSub(char16_t* dest U_LIFETIME_BOUND,
                      int32_t destCapacity,
                      int32_t* pDestLength,
                      स्थिरांक UChar32* src,
                      int32_t srcLength,
                      UChar32 subchar,
                      int32_t* p_अंक_Substitutions,
                      UErrorCode* pErrorCode) {
    स्थिरांक UChar32 *srcLimit;
    UChar32 ch;
    char16_t *destLimit;
    char16_t *pDest;
    int32_t reqLength;
    int32_t अंक_Substitutions;

    /* args check */
    जर(U_FAILURE(*pErrorCode)){
        परत nullptr;
    }
    जर( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }

    जर(p_अंक_Substitutions != nullptr) {
        *p_अंक_Substitutions = 0;
    }

    pDest = dest;
    destLimit = (dest!=nullptr)?(dest + destCapacity):nullptr;
    reqLength = 0;
    अंक_Substitutions = 0;

    जर(srcLength < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        पर्यंत((ch=*src) != 0 &&
              ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff))) {
            ++src;
            जर(pDest < destLimit) {
                *pDest++ = (char16_t)ch;
            } अन्यथा {
                ++reqLength;
            }
        }
        srcLimit = src;
        जर(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            पर्यंत(*++srcLimit != 0) {}
        }
    } अन्यथा {
      srcLimit = (src!=nullptr)?(src + srcLength):nullptr;
    }

    /* convert with length */
    पर्यंत(src < srcLimit) {
        ch = *src++;
        करा {
            /* usually "loops" once; twice only for writing subchar */
            जर((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff)) {
                जर(pDest < destLimit) {
                    *pDest++ = (char16_t)ch;
                } अन्यथा {
                    ++reqLength;
                }
                थांबा;
            } अन्यथा जर(0x10000 <= ch && ch <= 0x10ffff) {
                जर(pDest!=nullptr && ((pDest + 2) <= destLimit)) {
                    *pDest++ = U16_LEAD(ch);
                    *pDest++ = U16_TRAIL(ch);
                } अन्यथा {
                    reqLength += 2;
                }
                थांबा;
            } अन्यथा जर((ch = subchar) < 0) {
                /* surrogate code point, or not a Unicode code point at all */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                परत nullptr;
            } अन्यथा {
                ++अंक_Substitutions;
            }
        } पर्यंत(सत्य);
    }

    reqLength += (int32_t)(pDest - dest);
    जर(pDestLength) {
        *pDestLength = reqLength;
    }
    जर(p_अंक_Substitutions != nullptr) {
        *p_अंक_Substitutions = अंक_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
    
    परत dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32(char16_t* dest U_LIFETIME_BOUND,
               int32_t destCapacity,
               int32_t* pDestLength,
               स्थिरांक UChar32* src,
               int32_t srcLength,
               UErrorCode* pErrorCode) {
    परत u_strFromUTF32WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI UChar32* U_EXPORT2
u_strToUTF32WithSub(UChar32* dest U_LIFETIME_BOUND,
                    int32_t destCapacity,
                    int32_t* pDestLength,
                    स्थिरांक char16_t* src,
                    int32_t srcLength,
                    UChar32 subchar,
                    int32_t* p_अंक_Substitutions,
                    UErrorCode* pErrorCode) {
    स्थिरांक char16_t *srcLimit;
    UChar32 ch;
    char16_t ch2;
    UChar32 *destLimit;
    UChar32 *pDest;
    int32_t reqLength;
    int32_t अंक_Substitutions;

    /* args check */
    जर(U_FAILURE(*pErrorCode)){
        परत nullptr;
    }
    जर( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }

    जर(p_अंक_Substitutions != nullptr) {
        *p_अंक_Substitutions = 0;
    }

    pDest = dest;
    destLimit = (dest!=nullptr)?(dest + destCapacity):nullptr;
    reqLength = 0;
    अंक_Substitutions = 0;

    जर(srcLength < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        पर्यंत((ch=*src) != 0 && !U16_IS_SURROGATE(ch)) {
            ++src;
            जर(pDest < destLimit) {
                *pDest++ = ch;
            } अन्यथा {
                ++reqLength;
            }
        }
        srcLimit = src;
        जर(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            पर्यंत(*++srcLimit != 0) {}
        }
    } अन्यथा {
        srcLimit = (src!=nullptr)?(src + srcLength):nullptr;
    }

    /* convert with length */
    पर्यंत(src < srcLimit) {
        ch = *src++;
        जर(!U16_IS_SURROGATE(ch)) {
            /* write or count ch below */
        } अन्यथा जर(U16_IS_SURROGATE_LEAD(ch) && src < srcLimit && U16_IS_TRAIL(ch2 = *src)) {
            ++src;
            ch = U16_GET_SUPPLEMENTARY(ch, ch2);
        } अन्यथा जर((ch = subchar) < 0) {
            /* unpaired surrogate */
            *pErrorCode = U_INVALID_CHAR_FOUND;
            परत nullptr;
        } अन्यथा {
            ++अंक_Substitutions;
        }
        जर(pDest < destLimit) {
            *pDest++ = ch;
        } अन्यथा {
            ++reqLength;
        }
    }

    reqLength += (int32_t)(pDest - dest);
    जर(pDestLength) {
        *pDestLength = reqLength;
    }
    जर(p_अंक_Substitutions != nullptr) {
        *p_अंक_Substitutions = अंक_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChar32s(dest, destCapacity, reqLength, pErrorCode);

    परत dest;
}

U_CAPI UChar32* U_EXPORT2
u_strToUTF32(UChar32* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* pDestLength,
             स्थिरांक char16_t* src,
             int32_t srcLength,
             UErrorCode* pErrorCode) {
    परत u_strToUTF32WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                     int32_t destCapacity,
                     int32_t* pDestLength,
                     स्थिरांक अक्षर* src,
                     int32_t srcLength,
                     UChar32 subchar,
                     int32_t* p_अंक_Substitutions,
                     UErrorCode* pErrorCode) {
    /* args check */
    जर(U_FAILURE(*pErrorCode)) {
        परत nullptr;
    }
    जर( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }

    जर(p_अंक_Substitutions!=nullptr) {
        *p_अंक_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *pDestLimit = dest+destCapacity;
    int32_t reqLength = 0;
    int32_t अंक_Substitutions=0;

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

    जर(srcLength < 0){
        /*
         * Transform a NUL-terminated string.
         * The code explicitly checks for NULs only in the lead byte position.
         * A NUL byte in the trail byte position fails the trail byte range check anyway.
         */
        int32_t i;
        UChar32 c;
        साठी(i = 0; (c = (uint8_t)src[i]) != 0 && (pDest < pDestLimit);) {
            // modified copy of U8_NEXT()
            ++i;
            जर(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } अन्यथा {
                uint8_t __t1, __t2;
                जर( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } अन्यथा जर( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } अन्यथा {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((स्थिरांक uint8_t *)src, &(i), -1, c, -1);
                    जर(c<0 && (++अंक_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        परत nullptr;
                    } अन्यथा जर(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } अन्यथा {
                        *(pDest++)=U16_LEAD(c);
                        जर(pDest<pDestLimit) {
                            *(pDest++)=U16_TRAIL(c);
                        } अन्यथा {
                            reqLength++;
                            थांबा;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        पर्यंत((c = (uint8_t)src[i]) != 0) {
            // modified copy of U8_NEXT()
            ++i;
            जर(U8_IS_SINGLE(c)) {
                ++reqLength;
            } अन्यथा {
                uint8_t __t1, __t2;
                जर( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++reqLength;
                    i+=2;
                } अन्यथा जर( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++reqLength;
                    ++(i);
                } अन्यथा {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((स्थिरांक uint8_t *)src, &(i), -1, c, -1);
                    जर(c<0 && (++अंक_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        परत nullptr;
                    }
                    reqLength += U16_LENGTH(c);
                }
            }
        }
    } अन्यथा /* srcLength >= 0 */ {
        /* Faster loop without ongoing checking for srcLength and pDestLimit. */
        int32_t i = 0;
        UChar32 c;
        साठी(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            int32_t count = (int32_t)(pDestLimit - pDest);
            int32_t count2 = (srcLength - i) / 3;
            जर(count > count2) {
                count = count2; /* min(remaining dest, remaining src/3) */
            }
            जर(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                थांबा;
            }

            करा {
                // modified copy of U8_NEXT()
                c = (uint8_t)src[i++];
                जर(U8_IS_SINGLE(c)) {
                    *pDest++=(char16_t)c;
                } अन्यथा {
                    uint8_t __t1, __t2;
                    जर( /* handle U+0800..U+FFFF inline */
                            (0xe0<=(c) && (c)<0xf0) &&
                            ((i)+1)<srcLength &&
                            U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                            (__t2=src[(i)+1]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                        i+=2;
                    } अन्यथा जर( /* handle U+0080..U+07FF inline */
                            ((c)<0xe0 && (c)>=0xc2) &&
                            ((i)!=srcLength) &&
                            (__t1=src[i]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0x1f)<<6)|__t1;
                        ++(i);
                    } अन्यथा {
                        जर(c >= 0xf0 || subchar > 0xffff) {
                            // We may read up to four bytes and write up to two UChars,
                            // which we didn't account for with computing count,
                            // so we adjust it here.
                            जर(--count == 0) {
                                --i;  // back out byte c
                                थांबा;
                            }
                        }

                        /* function call for "complicated" and error cases */
                        (c)=utf8_nextCharSafeBody((स्थिरांक uint8_t *)src, &(i), srcLength, c, -1);
                        जर(c<0 && (++अंक_Substitutions, c = subchar) < 0) {
                            *pErrorCode = U_INVALID_CHAR_FOUND;
                            परत nullptr;
                        } अन्यथा जर(c<=0xFFFF) {
                            *(pDest++)=(char16_t)c;
                        } अन्यथा {
                            *(pDest++)=U16_LEAD(c);
                            *(pDest++)=U16_TRAIL(c);
                        }
                    }
                }
            } पर्यंत(--count > 0);
        }

        पर्यंत(i < srcLength && (pDest < pDestLimit)) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            जर(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } अन्यथा {
                uint8_t __t1, __t2;
                जर( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<srcLength &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } अन्यथा जर( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=srcLength) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } अन्यथा {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((स्थिरांक uint8_t *)src, &(i), srcLength, c, -1);
                    जर(c<0 && (++अंक_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        परत nullptr;
                    } अन्यथा जर(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } अन्यथा {
                        *(pDest++)=U16_LEAD(c);
                        जर(pDest<pDestLimit) {
                            *(pDest++)=U16_TRAIL(c);
                        } अन्यथा {
                            reqLength++;
                            थांबा;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        पर्यंत(i < srcLength) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            जर(U8_IS_SINGLE(c)) {
                ++reqLength;
            } अन्यथा {
                uint8_t __t1, __t2;
                जर( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<srcLength &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++reqLength;
                    i+=2;
                } अन्यथा जर( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=srcLength) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++reqLength;
                    ++(i);
                } अन्यथा {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((स्थिरांक uint8_t *)src, &(i), srcLength, c, -1);
                    जर(c<0 && (++अंक_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        परत nullptr;
                    }
                    reqLength += U16_LENGTH(c);
                }
            }
        }
    }

    reqLength+=(int32_t)(pDest - dest);

    जर(p_अंक_Substitutions!=nullptr) {
        *p_अंक_Substitutions=अंक_Substitutions;
    }

    जर(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,reqLength,pErrorCode);

    परत dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8(char16_t* dest U_LIFETIME_BOUND,
              int32_t destCapacity,
              int32_t* pDestLength,
              स्थिरांक अक्षर* src,
              int32_t srcLength,
              UErrorCode* pErrorCode) {
    परत u_strFromUTF8WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8Lenient(char16_t* dest U_LIFETIME_BOUND,
                     int32_t destCapacity,
                     int32_t* pDestLength,
                     स्थिरांक अक्षर* src,
                     int32_t srcLength,
                     UErrorCode* pErrorCode) {
    char16_t *pDest = dest;
    UChar32 ch;
    int32_t reqLength = 0;
    uint8_t* pSrc = (uint8_t*) src;

    /* args check */
    जर(U_FAILURE(*pErrorCode)){
        परत nullptr;
    }
        
    जर( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }

    जर(srcLength < 0) {
        /* Transform a NUL-terminated string. */
        char16_t *pDestLimit = (dest!=nullptr)?(dest+destCapacity):nullptr;
        uint8_t t1, t2, t3; /* trail bytes */

        पर्यंत(((ch = *pSrc) != 0) && (pDest < pDestLimit)) {
            जर(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ++pSrc;
                सुरूठेवा;
            } अन्यथा जर(ch < 0xe0) { /* U+0080..U+07FF */
                जर((t1 = pSrc[1]) != 0) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + t1 - 0x3080);
                    pSrc += 2;
                    सुरूठेवा;
                }
            } अन्यथा जर(ch < 0xf0) { /* U+0800..U+FFFF */
                जर((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 12) + (t1 << 6) + t2 - 0x2080);
                    pSrc += 3;
                    सुरूठेवा;
                }
            } अन्यथा /* f0..f4 */ { /* U+10000..U+10FFFF */
                जर((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0 && (t3 = pSrc[3]) != 0) {
                    pSrc += 4;
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (t1 << 12) + (t2 << 6) + t3 - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    जर(pDest < pDestLimit) {
                        *(pDest++) = U16_TRAIL(ch);
                    } अन्यथा {
                        reqLength = 1;
                        थांबा;
                    }
                    सुरूठेवा;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            पर्यंत(*++pSrc != 0) {}
            थांबा;
        }

        /* Pre-flight the rest of the string. */
        पर्यंत((ch = *pSrc) != 0) {
            जर(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                ++reqLength;
                ++pSrc;
                सुरूठेवा;
            } अन्यथा जर(ch < 0xe0) { /* U+0080..U+07FF */
                जर(pSrc[1] != 0) {
                    ++reqLength;
                    pSrc += 2;
                    सुरूठेवा;
                }
            } अन्यथा जर(ch < 0xf0) { /* U+0800..U+FFFF */
                जर(pSrc[1] != 0 && pSrc[2] != 0) {
                    ++reqLength;
                    pSrc += 3;
                    सुरूठेवा;
                }
            } अन्यथा /* f0..f4 */ { /* U+10000..U+10FFFF */
                जर(pSrc[1] != 0 && pSrc[2] != 0 && pSrc[3] != 0) {
                    reqLength += 2;
                    pSrc += 4;
                    सुरूठेवा;
                }
            }

            /* truncated character at the end */
            ++reqLength;
            थांबा;
        }
    } अन्यथा /* srcLength >= 0 */ {
      स्थिरांक uint8_t *pSrcLimit = (pSrc!=nullptr)?(pSrc + srcLength):nullptr;

        /*
         * This function requires that if srcLength is given, then it must be
         * destCapatity >= srcLength so that we need not check for
         * destination buffer overflow in the loop.
         */
        जर(destCapacity < srcLength) {
            जर(pDestLength != nullptr) {
                *pDestLength = srcLength; /* this likely overestimates the true destLength! */
            }
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            परत nullptr;
        }

        जर((pSrcLimit - pSrc) >= 4) {
            pSrcLimit -= 3; /* temporarily reduce pSrcLimit */

            /* in this loop, we can always access at least 4 bytes, up to pSrc+3 */
            करा {
                ch = *pSrc++;
                जर(ch < 0xc0) {
                    /*
                     * ASCII, or a trail byte in lead position which is treated like
                     * a single-byte sequence for better character boundary
                     * resynchronization after illegal sequences.
                     */
                    *pDest++=(char16_t)ch;
                } अन्यथा जर(ch < 0xe0) { /* U+0080..U+07FF */
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                } अन्यथा जर(ch < 0xf0) { /* U+0800..U+FFFF */
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                } अन्यथा /* f0..f4 */ { /* U+10000..U+10FFFF */
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                }
            } पर्यंत(pSrc < pSrcLimit);

            pSrcLimit += 3; /* restore original pSrcLimit */
        }

        पर्यंत(pSrc < pSrcLimit) {
            ch = *pSrc++;
            जर(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                सुरूठेवा;
            } अन्यथा जर(ch < 0xe0) { /* U+0080..U+07FF */
                जर(pSrc < pSrcLimit) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                    सुरूठेवा;
                }
            } अन्यथा जर(ch < 0xf0) { /* U+0800..U+FFFF */
                जर((pSrcLimit - pSrc) >= 2) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                    pSrc += 3;
                    सुरूठेवा;
                }
            } अन्यथा /* f0..f4 */ { /* U+10000..U+10FFFF */
                जर((pSrcLimit - pSrc) >= 3) {
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                    pSrc += 4;
                    सुरूठेवा;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            थांबा;
        }
    }

    reqLength+=(int32_t)(pDest - dest);

    जर(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,reqLength,pErrorCode);

    परत dest;
}

स्थिर ओळीत uint8_t *
_appendUTF8(uint8_t *pDest, UChar32 c) {
    /* it is 0<=c<=0x10ffff and not a surrogate if called by a validating function */
    जर((c)<=0x7f) {
        *pDest++ = static_cast<uint8_t>(c);
    } अन्यथा जर(c<=0x7ff) {
        *pDest++ = static_cast<uint8_t>((c >> 6) | 0xc0);
        *pDest++ = static_cast<uint8_t>((c & 0x3f) | 0x80);
    } अन्यथा जर(c<=0xffff) {
        *pDest++ = static_cast<uint8_t>((c >> 12) | 0xe0);
        *pDest++ = static_cast<uint8_t>(((c >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    } अन्यथा /* if((uint32_t)(c)<=0x10ffff) */ {
        *pDest++ = static_cast<uint8_t>(((c) >> 18) | 0xf0);
        *pDest++ = static_cast<uint8_t>((((c) >> 12) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>((((c) >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    }
    परत pDest;
}

U_CAPI अक्षर* U_EXPORT2
u_strToUTF8WithSub(अक्षर* dest U_LIFETIME_BOUND,
                   int32_t destCapacity,
                   int32_t* pDestLength,
                   स्थिरांक char16_t* pSrc,
                   int32_t srcLength,
                   UChar32 subchar,
                   int32_t* p_अंक_Substitutions,
                   UErrorCode* pErrorCode) {
    int32_t reqLength=0;
    uint32_t ch=0,ch2=0;
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *pDestLimit = (pDest!=nullptr)?(pDest + destCapacity):nullptr;
    int32_t अंक_Substitutions;

    /* args check */
    जर(U_FAILURE(*pErrorCode)){
        परत nullptr;
    }
        
    जर( (pSrc==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }

    जर(p_अंक_Substitutions!=nullptr) {
        *p_अंक_Substitutions=0;
    }
    अंक_Substitutions=0;

    जर(srcLength==-1) {
        पर्यंत((ch=*pSrc)!=0) {
            ++pSrc;
            जर(ch <= 0x7f) {
                जर(pDest<pDestLimit) {
                    *pDest++ = (uint8_t)ch;
                } अन्यथा {
                    reqLength = 1;
                    थांबा;
                }
            } अन्यथा जर(ch <= 0x7ff) {
                जर((pDestLimit - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अन्यथा {
                    reqLength = 2;
                    थांबा;
                }
            } अन्यथा जर(ch <= 0xd7ff || ch >= 0xe000) {
                जर((pDestLimit - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अन्यथा {
                    reqLength = 3;
                    थांबा;
                }
            } अन्यथा /* ch is a surrogate */ {
                int32_t length;

                /*need not check for NUL because NUL fails U16_IS_TRAIL() anyway*/
                जर(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } अन्यथा जर(subchar>=0) {
                    ch=subchar;
                    ++अंक_Substitutions;
                } अन्यथा {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    परत nullptr;
                }

                length = U8_LENGTH(ch);
                जर((pDestLimit - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } अन्यथा {
                    reqLength = length;
                    थांबा;
                }
            }
        }
        पर्यंत((ch=*pSrc++)!=0) {
            जर(ch<=0x7f) {
                ++reqLength;
            } अन्यथा जर(ch<=0x7ff) {
                reqLength+=2;
            } अन्यथा जर(!U16_IS_SURROGATE(ch)) {
                reqLength+=3;
            } अन्यथा जर(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                reqLength+=4;
            } अन्यथा जर(subchar>=0) {
                reqLength+=U8_LENGTH(subchar);
                ++अंक_Substitutions;
            } अन्यथा {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                परत nullptr;
            }
        }
    } अन्यथा {
        स्थिरांक char16_t *pSrcLimit = (pSrc!=nullptr)?(pSrc+srcLength):nullptr;
        int32_t count;

        /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
        साठी(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            count = (int32_t)((pDestLimit - pDest) / 3);
            srcLength = (int32_t)(pSrcLimit - pSrc);
            जर(count > srcLength) {
                count = srcLength; /* min(remaining dest/3, remaining src) */
            }
            जर(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                थांबा;
            }
            करा {
                ch=*pSrc++;
                जर(ch <= 0x7f) {
                    *pDest++ = (uint8_t)ch;
                } अन्यथा जर(ch <= 0x7ff) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अन्यथा जर(ch <= 0xd7ff || ch >= 0xe000) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अन्यथा /* ch is a surrogate */ {
                    /*
                     * We will read two UChars and probably output four bytes,
                     * which we didn't account for with computing count,
                     * so we adjust it here.
                     */
                    जर(--count == 0) {
                        --pSrc; /* undo ch=*pSrc++ for the lead surrogate */
                        थांबा;  /* recompute count */
                    }

                    जर(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                        ++pSrc;
                        ch=U16_GET_SUPPLEMENTARY(ch, ch2);

                        /* writing 4 bytes per 2 UChars is ok */
                        *pDest++=(uint8_t)((ch>>18)|0xf0);
                        *pDest++=(uint8_t)(((ch>>12)&0x3f)|0x80);
                        *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                        *pDest++=(uint8_t)((ch&0x3f)|0x80);
                    } अन्यथा  {
                        /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                        जर(subchar>=0) {
                            ch=subchar;
                            ++अंक_Substitutions;
                        } अन्यथा {
                            *pErrorCode = U_INVALID_CHAR_FOUND;
                            परत nullptr;
                        }

                        /* convert and append*/
                        pDest=_appendUTF8(pDest, ch);
                    }
                }
            } पर्यंत(--count > 0);
        }

        पर्यंत(pSrc<pSrcLimit) {
            ch=*pSrc++;
            जर(ch <= 0x7f) {
                जर(pDest<pDestLimit) {
                    *pDest++ = (uint8_t)ch;
                } अन्यथा {
                    reqLength = 1;
                    थांबा;
                }
            } अन्यथा जर(ch <= 0x7ff) {
                जर((pDestLimit - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अन्यथा {
                    reqLength = 2;
                    थांबा;
                }
            } अन्यथा जर(ch <= 0xd7ff || ch >= 0xe000) {
                जर((pDestLimit - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अन्यथा {
                    reqLength = 3;
                    थांबा;
                }
            } अन्यथा /* ch is a surrogate */ {
                int32_t length;

                जर(U16_IS_SURROGATE_LEAD(ch) && pSrc<pSrcLimit && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } अन्यथा जर(subchar>=0) {
                    ch=subchar;
                    ++अंक_Substitutions;
                } अन्यथा {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    परत nullptr;
                }

                length = U8_LENGTH(ch);
                जर((pDestLimit - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } अन्यथा {
                    reqLength = length;
                    थांबा;
                }
            }
        }
        पर्यंत(pSrc<pSrcLimit) {
            ch=*pSrc++;
            जर(ch<=0x7f) {
                ++reqLength;
            } अन्यथा जर(ch<=0x7ff) {
                reqLength+=2;
            } अन्यथा जर(!U16_IS_SURROGATE(ch)) {
                reqLength+=3;
            } अन्यथा जर(U16_IS_SURROGATE_LEAD(ch) && pSrc<pSrcLimit && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                reqLength+=4;
            } अन्यथा जर(subchar>=0) {
                reqLength+=U8_LENGTH(subchar);
                ++अंक_Substitutions;
            } अन्यथा {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                परत nullptr;
            }
        }
    }

    reqLength+=(int32_t)(pDest - (uint8_t *)dest);

    जर(p_अंक_Substitutions!=nullptr) {
        *p_अंक_Substitutions=अंक_Substitutions;
    }

    जर(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
    परत dest;
}

U_CAPI अक्षर* U_EXPORT2
u_strToUTF8(अक्षर* dest U_LIFETIME_BOUND,
            int32_t destCapacity,
            int32_t* pDestLength,
            स्थिरांक char16_t* pSrc,
            int32_t srcLength,
            UErrorCode* pErrorCode) {
    परत u_strToUTF8WithSub(
            dest, destCapacity, pDestLength,
            pSrc, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromJavaModifiedUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                                 int32_t destCapacity,
                                 int32_t* pDestLength,
                                 स्थिरांक अक्षर* src,
                                 int32_t srcLength,
                                 UChar32 subchar,
                                 int32_t* p_अंक_Substitutions,
                                 UErrorCode* pErrorCode) {
    /* args check */
    जर(U_FAILURE(*pErrorCode)) {
        परत nullptr;
    }
    जर( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (dest==nullptr && destCapacity!=0) || destCapacity<0 ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }

    जर(p_अंक_Substitutions!=nullptr) {
        *p_अंक_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *pDestLimit = dest+destCapacity;
    int32_t reqLength = 0;
    int32_t अंक_Substitutions=0;

    जर(srcLength < 0) {
        /*
         * Transform a NUL-terminated ASCII string.
         * Handle non-ASCII strings with slower code.
         */
        UChar32 c;
        पर्यंत(((c = (uint8_t)*src) != 0) && c <= 0x7f && (pDest < pDestLimit)) {
            *pDest++=(char16_t)c;
            ++src;
        }
        जर(c == 0) {
            reqLength=(int32_t)(pDest - dest);
            जर(pDestLength) {
                *pDestLength = reqLength;
            }

            /* Terminate the buffer */
            u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
            परत dest;
        }
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    /* Faster loop without ongoing checking for srcLength and pDestLimit. */
    UChar32 ch;
    uint8_t t1, t2;
    int32_t i = 0;
    साठी(;;) {
        int32_t count = (int32_t)(pDestLimit - pDest);
        int32_t count2 = srcLength - i;
        जर(count >= count2 && srcLength > 0 && U8_IS_SINGLE(*src)) {
            /* fast ASCII loop */
            int32_t start = i;
            uint8_t b;
            पर्यंत(i < srcLength && U8_IS_SINGLE(b = src[i])) {
                *pDest++=b;
                ++i;
            }
            int32_t delta = i - start;
            count -= delta;
            count2 -= delta;
        }
        /*
         * Each iteration of the inner loop progresses by at most 3 UTF-8
         * bytes and one char16_t.
         */
        जर(subchar > 0xFFFF) {
            थांबा;
        }
        count2 /= 3;
        जर(count > count2) {
            count = count2; /* min(remaining dest, remaining src/3) */
        }
        जर(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            थांबा;
        }
        करा {
            ch = (uint8_t)src[i++];
            जर(U8_IS_SINGLE(ch)) {
                *pDest++=(char16_t)ch;
            } अन्यथा {
                जर(ch >= 0xe0) {
                    जर( /* handle U+0000..U+FFFF inline */
                        ch <= 0xef &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                        (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                    ) {
                        /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                        *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                        i += 2;
                        सुरूठेवा;
                    }
                } अन्यथा {
                    जर( /* handle U+0000..U+07FF inline */
                        ch >= 0xc0 &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                    ) {
                        *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                        ++i;
                        सुरूठेवा;
                    }
                }

                जर(subchar < 0) {
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    परत nullptr;
                } अन्यथा जर(subchar > 0xffff && --count == 0) {
                    /*
                     * We need to write two UChars, adjusted count for that,
                     * and ran out of space.
                     */
                    --i;  // back out byte ch
                    थांबा;
                } अन्यथा {
                    /* function call for error cases */
                    utf8_nextCharSafeBody((स्थिरांक uint8_t *)src, &(i), srcLength, ch, -1);
                    ++अंक_Substitutions;
                    *(pDest++)=(char16_t)subchar;
                }
            }
        } पर्यंत(--count > 0);
    }

    पर्यंत(i < srcLength && (pDest < pDestLimit)) {
        ch = (uint8_t)src[i++];
        जर(U8_IS_SINGLE(ch)){
            *pDest++=(char16_t)ch;
        } अन्यथा {
            जर(ch >= 0xe0) {
                जर( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < srcLength &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                    (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                ) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                    i += 2;
                    सुरूठेवा;
                }
            } अन्यथा {
                जर( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < srcLength &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                ) {
                    *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                    ++i;
                    सुरूठेवा;
                }
            }

            जर(subchar < 0) {
                *pErrorCode = U_INVALID_CHAR_FOUND;
                परत nullptr;
            } अन्यथा {
                /* function call for error cases */
                utf8_nextCharSafeBody((स्थिरांक uint8_t *)src, &(i), srcLength, ch, -1);
                ++अंक_Substitutions;
                जर(subchar<=0xFFFF) {
                    *(pDest++)=(char16_t)subchar;
                } अन्यथा {
                    *(pDest++)=U16_LEAD(subchar);
                    जर(pDest<pDestLimit) {
                        *(pDest++)=U16_TRAIL(subchar);
                    } अन्यथा {
                        reqLength++;
                        थांबा;
                    }
                }
            }
        }
    }

    /* Pre-flight the rest of the string. */
    पर्यंत(i < srcLength) {
        ch = (uint8_t)src[i++];
        जर(U8_IS_SINGLE(ch)) {
            reqLength++;
        } अन्यथा {
            जर(ch >= 0xe0) {
                जर( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < srcLength &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f &&
                    (uint8_t)(src[i+1] - 0x80) <= 0x3f
                ) {
                    reqLength++;
                    i += 2;
                    सुरूठेवा;
                }
            } अन्यथा {
                जर( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < srcLength &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f
                ) {
                    reqLength++;
                    ++i;
                    सुरूठेवा;
                }
            }

            जर(subchar < 0) {
                *pErrorCode = U_INVALID_CHAR_FOUND;
                परत nullptr;
            } अन्यथा {
                /* function call for error cases */
                utf8_nextCharSafeBody((स्थिरांक uint8_t *)src, &(i), srcLength, ch, -1);
                ++अंक_Substitutions;
                reqLength+=U16_LENGTH(ch);
            }
        }
    }

    जर(p_अंक_Substitutions!=nullptr) {
        *p_अंक_Substitutions=अंक_Substitutions;
    }

    reqLength+=(int32_t)(pDest - dest);
    जर(pDestLength) {
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
    परत dest;
}

U_CAPI अक्षर* U_EXPORT2
u_strToJavaModifiedUTF8(अक्षर* dest U_LIFETIME_BOUND,
                        int32_t destCapacity,
                        int32_t* pDestLength,
                        स्थिरांक char16_t* src,
                        int32_t srcLength,
                        UErrorCode* pErrorCode) {
    int32_t reqLength=0;
    uint32_t ch=0;
    स्थिरांक char16_t *pSrcLimit;
    int32_t count;

    /* args check */
    जर(U_FAILURE(*pErrorCode)){
        परत nullptr;
    }
    जर( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (dest==nullptr && destCapacity!=0) || destCapacity<0
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *pDestLimit = pDest + destCapacity;

    जर(srcLength==-1) {
        /* Convert NUL-terminated ASCII, then find the string length. */
        पर्यंत((ch=*src)<=0x7f && ch != 0 && pDest<pDestLimit) {
            *pDest++ = (uint8_t)ch;
            ++src;
        }
        जर(ch == 0) {
            reqLength=(int32_t)(pDest - (uint8_t *)dest);
            जर(pDestLength) {
                *pDestLength = reqLength;
            }

            /* Terminate the buffer */
            u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
            परत dest;
        }
        srcLength = u_strlen(src);
    }

    /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
    pSrcLimit = (src!=nullptr)?(src+srcLength):nullptr;
    साठी(;;) {
        count = (int32_t)(pDestLimit - pDest);
        srcLength = (int32_t)(pSrcLimit - src);
        जर(count >= srcLength && srcLength > 0 && *src <= 0x7f) {
            /* fast ASCII loop */
            स्थिरांक char16_t *prevSrc = src;
            int32_t delta;
            पर्यंत(src < pSrcLimit && (ch = *src) <= 0x7f && ch != 0) {
                *pDest++=(uint8_t)ch;
                ++src;
            }
            delta = (int32_t)(src - prevSrc);
            count -= delta;
            srcLength -= delta;
        }
        /*
         * Each iteration of the inner loop progresses by at most 3 UTF-8
         * bytes and one char16_t.
         */
        count /= 3;
        जर(count > srcLength) {
            count = srcLength; /* min(remaining dest/3, remaining src) */
        }
        जर(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            थांबा;
        }
        करा {
            ch=*src++;
            जर(ch <= 0x7f && ch != 0) {
                *pDest++ = (uint8_t)ch;
            } अन्यथा जर(ch <= 0x7ff) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } अन्यथा {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            }
        } पर्यंत(--count > 0);
    }

    पर्यंत(src<pSrcLimit) {
        ch=*src++;
        जर(ch <= 0x7f && ch != 0) {
            जर(pDest<pDestLimit) {
                *pDest++ = (uint8_t)ch;
            } अन्यथा {
                reqLength = 1;
                थांबा;
            }
        } अन्यथा जर(ch <= 0x7ff) {
            जर((pDestLimit - pDest) >= 2) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } अन्यथा {
                reqLength = 2;
                थांबा;
            }
        } अन्यथा {
            जर((pDestLimit - pDest) >= 3) {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } अन्यथा {
                reqLength = 3;
                थांबा;
            }
        }
    }
    पर्यंत(src<pSrcLimit) {
        ch=*src++;
        जर(ch <= 0x7f && ch != 0) {
            ++reqLength;
        } अन्यथा जर(ch<=0x7ff) {
            reqLength+=2;
        } अन्यथा {
            reqLength+=3;
        }
    }

    reqLength+=(int32_t)(pDest - (uint8_t *)dest);
    जर(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
    परत dest;
}
