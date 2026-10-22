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
                      ध्रुव UChar32* src,
                      int32_t srcLength,
                      UChar32 subchar,
                      int32_t* p_अङ्कः_Substitutions,
                      UErrorCode* pErrorCode) {
    ध्रुव UChar32 *srcLimit;
    UChar32 ch;
    char16_t *destLimit;
    char16_t *pDest;
    int32_t reqLength;
    int32_t अङ्कः_Substitutions;

    /* args check */
    जइ(U_FAILURE(*pErrorCode)){
        प्रत्यागम nullptr;
    }
    जइ( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }

    जइ(p_अङ्कः_Substitutions != nullptr) {
        *p_अङ्कः_Substitutions = 0;
    }

    pDest = dest;
    destLimit = (dest!=nullptr)?(dest + destCapacity):nullptr;
    reqLength = 0;
    अङ्कः_Substitutions = 0;

    जइ(srcLength < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        जाव((ch=*src) != 0 &&
              ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff))) {
            ++src;
            जइ(pDest < destLimit) {
                *pDest++ = (char16_t)ch;
            } अण्णहा {
                ++reqLength;
            }
        }
        srcLimit = src;
        जइ(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            जाव(*++srcLimit != 0) {}
        }
    } अण्णहा {
      srcLimit = (src!=nullptr)?(src + srcLength):nullptr;
    }

    /* convert with length */
    जाव(src < srcLimit) {
        ch = *src++;
        करेहि {
            /* usually "loops" once; twice only for writing subchar */
            जइ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff)) {
                जइ(pDest < destLimit) {
                    *pDest++ = (char16_t)ch;
                } अण्णहा {
                    ++reqLength;
                }
                भङ्ग;
            } अण्णहा जइ(0x10000 <= ch && ch <= 0x10ffff) {
                जइ(pDest!=nullptr && ((pDest + 2) <= destLimit)) {
                    *pDest++ = U16_LEAD(ch);
                    *pDest++ = U16_TRAIL(ch);
                } अण्णहा {
                    reqLength += 2;
                }
                भङ्ग;
            } अण्णहा जइ((ch = subchar) < 0) {
                /* surrogate code point, or not a Unicode code point at all */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                प्रत्यागम nullptr;
            } अण्णहा {
                ++अङ्कः_Substitutions;
            }
        } जाव(सच्च);
    }

    reqLength += (int32_t)(pDest - dest);
    जइ(pDestLength) {
        *pDestLength = reqLength;
    }
    जइ(p_अङ्कः_Substitutions != nullptr) {
        *p_अङ्कः_Substitutions = अङ्कः_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
    
    प्रत्यागम dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32(char16_t* dest U_LIFETIME_BOUND,
               int32_t destCapacity,
               int32_t* pDestLength,
               ध्रुव UChar32* src,
               int32_t srcLength,
               UErrorCode* pErrorCode) {
    प्रत्यागम u_strFromUTF32WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI UChar32* U_EXPORT2
u_strToUTF32WithSub(UChar32* dest U_LIFETIME_BOUND,
                    int32_t destCapacity,
                    int32_t* pDestLength,
                    ध्रुव char16_t* src,
                    int32_t srcLength,
                    UChar32 subchar,
                    int32_t* p_अङ्कः_Substitutions,
                    UErrorCode* pErrorCode) {
    ध्रुव char16_t *srcLimit;
    UChar32 ch;
    char16_t ch2;
    UChar32 *destLimit;
    UChar32 *pDest;
    int32_t reqLength;
    int32_t अङ्कः_Substitutions;

    /* args check */
    जइ(U_FAILURE(*pErrorCode)){
        प्रत्यागम nullptr;
    }
    जइ( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }

    जइ(p_अङ्कः_Substitutions != nullptr) {
        *p_अङ्कः_Substitutions = 0;
    }

    pDest = dest;
    destLimit = (dest!=nullptr)?(dest + destCapacity):nullptr;
    reqLength = 0;
    अङ्कः_Substitutions = 0;

    जइ(srcLength < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        जाव((ch=*src) != 0 && !U16_IS_SURROGATE(ch)) {
            ++src;
            जइ(pDest < destLimit) {
                *pDest++ = ch;
            } अण्णहा {
                ++reqLength;
            }
        }
        srcLimit = src;
        जइ(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            जाव(*++srcLimit != 0) {}
        }
    } अण्णहा {
        srcLimit = (src!=nullptr)?(src + srcLength):nullptr;
    }

    /* convert with length */
    जाव(src < srcLimit) {
        ch = *src++;
        जइ(!U16_IS_SURROGATE(ch)) {
            /* write or count ch below */
        } अण्णहा जइ(U16_IS_SURROGATE_LEAD(ch) && src < srcLimit && U16_IS_TRAIL(ch2 = *src)) {
            ++src;
            ch = U16_GET_SUPPLEMENTARY(ch, ch2);
        } अण्णहा जइ((ch = subchar) < 0) {
            /* unpaired surrogate */
            *pErrorCode = U_INVALID_CHAR_FOUND;
            प्रत्यागम nullptr;
        } अण्णहा {
            ++अङ्कः_Substitutions;
        }
        जइ(pDest < destLimit) {
            *pDest++ = ch;
        } अण्णहा {
            ++reqLength;
        }
    }

    reqLength += (int32_t)(pDest - dest);
    जइ(pDestLength) {
        *pDestLength = reqLength;
    }
    जइ(p_अङ्कः_Substitutions != nullptr) {
        *p_अङ्कः_Substitutions = अङ्कः_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChar32s(dest, destCapacity, reqLength, pErrorCode);

    प्रत्यागम dest;
}

U_CAPI UChar32* U_EXPORT2
u_strToUTF32(UChar32* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* pDestLength,
             ध्रुव char16_t* src,
             int32_t srcLength,
             UErrorCode* pErrorCode) {
    प्रत्यागम u_strToUTF32WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                     int32_t destCapacity,
                     int32_t* pDestLength,
                     ध्रुव वर्ण* src,
                     int32_t srcLength,
                     UChar32 subchar,
                     int32_t* p_अङ्कः_Substitutions,
                     UErrorCode* pErrorCode) {
    /* args check */
    जइ(U_FAILURE(*pErrorCode)) {
        प्रत्यागम nullptr;
    }
    जइ( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }

    जइ(p_अङ्कः_Substitutions!=nullptr) {
        *p_अङ्कः_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *pDestLimit = dest+destCapacity;
    int32_t reqLength = 0;
    int32_t अङ्कः_Substitutions=0;

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

    जइ(srcLength < 0){
        /*
         * Transform a NUL-terminated string.
         * The code explicitly checks for NULs only in the lead byte position.
         * A NUL byte in the trail byte position fails the trail byte range check anyway.
         */
        int32_t i;
        UChar32 c;
        प्रति(i = 0; (c = (uint8_t)src[i]) != 0 && (pDest < pDestLimit);) {
            // modified copy of U8_NEXT()
            ++i;
            जइ(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } अण्णहा {
                uint8_t __t1, __t2;
                जइ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } अण्णहा जइ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } अण्णहा {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((ध्रुव uint8_t *)src, &(i), -1, c, -1);
                    जइ(c<0 && (++अङ्कः_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        प्रत्यागम nullptr;
                    } अण्णहा जइ(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } अण्णहा {
                        *(pDest++)=U16_LEAD(c);
                        जइ(pDest<pDestLimit) {
                            *(pDest++)=U16_TRAIL(c);
                        } अण्णहा {
                            reqLength++;
                            भङ्ग;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        जाव((c = (uint8_t)src[i]) != 0) {
            // modified copy of U8_NEXT()
            ++i;
            जइ(U8_IS_SINGLE(c)) {
                ++reqLength;
            } अण्णहा {
                uint8_t __t1, __t2;
                जइ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++reqLength;
                    i+=2;
                } अण्णहा जइ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++reqLength;
                    ++(i);
                } अण्णहा {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((ध्रुव uint8_t *)src, &(i), -1, c, -1);
                    जइ(c<0 && (++अङ्कः_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        प्रत्यागम nullptr;
                    }
                    reqLength += U16_LENGTH(c);
                }
            }
        }
    } अण्णहा /* srcLength >= 0 */ {
        /* Faster loop without ongoing checking for srcLength and pDestLimit. */
        int32_t i = 0;
        UChar32 c;
        प्रति(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            int32_t count = (int32_t)(pDestLimit - pDest);
            int32_t count2 = (srcLength - i) / 3;
            जइ(count > count2) {
                count = count2; /* min(remaining dest, remaining src/3) */
            }
            जइ(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                भङ्ग;
            }

            करेहि {
                // modified copy of U8_NEXT()
                c = (uint8_t)src[i++];
                जइ(U8_IS_SINGLE(c)) {
                    *pDest++=(char16_t)c;
                } अण्णहा {
                    uint8_t __t1, __t2;
                    जइ( /* handle U+0800..U+FFFF inline */
                            (0xe0<=(c) && (c)<0xf0) &&
                            ((i)+1)<srcLength &&
                            U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                            (__t2=src[(i)+1]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                        i+=2;
                    } अण्णहा जइ( /* handle U+0080..U+07FF inline */
                            ((c)<0xe0 && (c)>=0xc2) &&
                            ((i)!=srcLength) &&
                            (__t1=src[i]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0x1f)<<6)|__t1;
                        ++(i);
                    } अण्णहा {
                        जइ(c >= 0xf0 || subchar > 0xffff) {
                            // We may read up to four bytes and write up to two UChars,
                            // which we didn't account for with computing count,
                            // so we adjust it here.
                            जइ(--count == 0) {
                                --i;  // back out byte c
                                भङ्ग;
                            }
                        }

                        /* function call for "complicated" and error cases */
                        (c)=utf8_nextCharSafeBody((ध्रुव uint8_t *)src, &(i), srcLength, c, -1);
                        जइ(c<0 && (++अङ्कः_Substitutions, c = subchar) < 0) {
                            *pErrorCode = U_INVALID_CHAR_FOUND;
                            प्रत्यागम nullptr;
                        } अण्णहा जइ(c<=0xFFFF) {
                            *(pDest++)=(char16_t)c;
                        } अण्णहा {
                            *(pDest++)=U16_LEAD(c);
                            *(pDest++)=U16_TRAIL(c);
                        }
                    }
                }
            } जाव(--count > 0);
        }

        जाव(i < srcLength && (pDest < pDestLimit)) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            जइ(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } अण्णहा {
                uint8_t __t1, __t2;
                जइ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<srcLength &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } अण्णहा जइ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=srcLength) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } अण्णहा {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((ध्रुव uint8_t *)src, &(i), srcLength, c, -1);
                    जइ(c<0 && (++अङ्कः_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        प्रत्यागम nullptr;
                    } अण्णहा जइ(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } अण्णहा {
                        *(pDest++)=U16_LEAD(c);
                        जइ(pDest<pDestLimit) {
                            *(pDest++)=U16_TRAIL(c);
                        } अण्णहा {
                            reqLength++;
                            भङ्ग;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        जाव(i < srcLength) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            जइ(U8_IS_SINGLE(c)) {
                ++reqLength;
            } अण्णहा {
                uint8_t __t1, __t2;
                जइ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<srcLength &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++reqLength;
                    i+=2;
                } अण्णहा जइ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=srcLength) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++reqLength;
                    ++(i);
                } अण्णहा {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((ध्रुव uint8_t *)src, &(i), srcLength, c, -1);
                    जइ(c<0 && (++अङ्कः_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        प्रत्यागम nullptr;
                    }
                    reqLength += U16_LENGTH(c);
                }
            }
        }
    }

    reqLength+=(int32_t)(pDest - dest);

    जइ(p_अङ्कः_Substitutions!=nullptr) {
        *p_अङ्कः_Substitutions=अङ्कः_Substitutions;
    }

    जइ(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,reqLength,pErrorCode);

    प्रत्यागम dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8(char16_t* dest U_LIFETIME_BOUND,
              int32_t destCapacity,
              int32_t* pDestLength,
              ध्रुव वर्ण* src,
              int32_t srcLength,
              UErrorCode* pErrorCode) {
    प्रत्यागम u_strFromUTF8WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8Lenient(char16_t* dest U_LIFETIME_BOUND,
                     int32_t destCapacity,
                     int32_t* pDestLength,
                     ध्रुव वर्ण* src,
                     int32_t srcLength,
                     UErrorCode* pErrorCode) {
    char16_t *pDest = dest;
    UChar32 ch;
    int32_t reqLength = 0;
    uint8_t* pSrc = (uint8_t*) src;

    /* args check */
    जइ(U_FAILURE(*pErrorCode)){
        प्रत्यागम nullptr;
    }
        
    जइ( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }

    जइ(srcLength < 0) {
        /* Transform a NUL-terminated string. */
        char16_t *pDestLimit = (dest!=nullptr)?(dest+destCapacity):nullptr;
        uint8_t t1, t2, t3; /* trail bytes */

        जाव(((ch = *pSrc) != 0) && (pDest < pDestLimit)) {
            जइ(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ++pSrc;
                अनुवर्त;
            } अण्णहा जइ(ch < 0xe0) { /* U+0080..U+07FF */
                जइ((t1 = pSrc[1]) != 0) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + t1 - 0x3080);
                    pSrc += 2;
                    अनुवर्त;
                }
            } अण्णहा जइ(ch < 0xf0) { /* U+0800..U+FFFF */
                जइ((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 12) + (t1 << 6) + t2 - 0x2080);
                    pSrc += 3;
                    अनुवर्त;
                }
            } अण्णहा /* f0..f4 */ { /* U+10000..U+10FFFF */
                जइ((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0 && (t3 = pSrc[3]) != 0) {
                    pSrc += 4;
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (t1 << 12) + (t2 << 6) + t3 - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    जइ(pDest < pDestLimit) {
                        *(pDest++) = U16_TRAIL(ch);
                    } अण्णहा {
                        reqLength = 1;
                        भङ्ग;
                    }
                    अनुवर्त;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            जाव(*++pSrc != 0) {}
            भङ्ग;
        }

        /* Pre-flight the rest of the string. */
        जाव((ch = *pSrc) != 0) {
            जइ(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                ++reqLength;
                ++pSrc;
                अनुवर्त;
            } अण्णहा जइ(ch < 0xe0) { /* U+0080..U+07FF */
                जइ(pSrc[1] != 0) {
                    ++reqLength;
                    pSrc += 2;
                    अनुवर्त;
                }
            } अण्णहा जइ(ch < 0xf0) { /* U+0800..U+FFFF */
                जइ(pSrc[1] != 0 && pSrc[2] != 0) {
                    ++reqLength;
                    pSrc += 3;
                    अनुवर्त;
                }
            } अण्णहा /* f0..f4 */ { /* U+10000..U+10FFFF */
                जइ(pSrc[1] != 0 && pSrc[2] != 0 && pSrc[3] != 0) {
                    reqLength += 2;
                    pSrc += 4;
                    अनुवर्त;
                }
            }

            /* truncated character at the end */
            ++reqLength;
            भङ्ग;
        }
    } अण्णहा /* srcLength >= 0 */ {
      ध्रुव uint8_t *pSrcLimit = (pSrc!=nullptr)?(pSrc + srcLength):nullptr;

        /*
         * This function requires that if srcLength is given, then it must be
         * destCapatity >= srcLength so that we need not check for
         * destination buffer overflow in the loop.
         */
        जइ(destCapacity < srcLength) {
            जइ(pDestLength != nullptr) {
                *pDestLength = srcLength; /* this likely overestimates the true destLength! */
            }
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            प्रत्यागम nullptr;
        }

        जइ((pSrcLimit - pSrc) >= 4) {
            pSrcLimit -= 3; /* temporarily reduce pSrcLimit */

            /* in this loop, we can always access at least 4 bytes, up to pSrc+3 */
            करेहि {
                ch = *pSrc++;
                जइ(ch < 0xc0) {
                    /*
                     * ASCII, or a trail byte in lead position which is treated like
                     * a single-byte sequence for better character boundary
                     * resynchronization after illegal sequences.
                     */
                    *pDest++=(char16_t)ch;
                } अण्णहा जइ(ch < 0xe0) { /* U+0080..U+07FF */
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                } अण्णहा जइ(ch < 0xf0) { /* U+0800..U+FFFF */
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                } अण्णहा /* f0..f4 */ { /* U+10000..U+10FFFF */
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                }
            } जाव(pSrc < pSrcLimit);

            pSrcLimit += 3; /* restore original pSrcLimit */
        }

        जाव(pSrc < pSrcLimit) {
            ch = *pSrc++;
            जइ(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                अनुवर्त;
            } अण्णहा जइ(ch < 0xe0) { /* U+0080..U+07FF */
                जइ(pSrc < pSrcLimit) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                    अनुवर्त;
                }
            } अण्णहा जइ(ch < 0xf0) { /* U+0800..U+FFFF */
                जइ((pSrcLimit - pSrc) >= 2) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                    pSrc += 3;
                    अनुवर्त;
                }
            } अण्णहा /* f0..f4 */ { /* U+10000..U+10FFFF */
                जइ((pSrcLimit - pSrc) >= 3) {
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                    pSrc += 4;
                    अनुवर्त;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            भङ्ग;
        }
    }

    reqLength+=(int32_t)(pDest - dest);

    जइ(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,reqLength,pErrorCode);

    प्रत्यागम dest;
}

स्थिर अन्तःस्थ uint8_t *
_appendUTF8(uint8_t *pDest, UChar32 c) {
    /* it is 0<=c<=0x10ffff and not a surrogate if called by a validating function */
    जइ((c)<=0x7f) {
        *pDest++ = static_cast<uint8_t>(c);
    } अण्णहा जइ(c<=0x7ff) {
        *pDest++ = static_cast<uint8_t>((c >> 6) | 0xc0);
        *pDest++ = static_cast<uint8_t>((c & 0x3f) | 0x80);
    } अण्णहा जइ(c<=0xffff) {
        *pDest++ = static_cast<uint8_t>((c >> 12) | 0xe0);
        *pDest++ = static_cast<uint8_t>(((c >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    } अण्णहा /* if((uint32_t)(c)<=0x10ffff) */ {
        *pDest++ = static_cast<uint8_t>(((c) >> 18) | 0xf0);
        *pDest++ = static_cast<uint8_t>((((c) >> 12) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>((((c) >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    }
    प्रत्यागम pDest;
}

U_CAPI वर्ण* U_EXPORT2
u_strToUTF8WithSub(वर्ण* dest U_LIFETIME_BOUND,
                   int32_t destCapacity,
                   int32_t* pDestLength,
                   ध्रुव char16_t* pSrc,
                   int32_t srcLength,
                   UChar32 subchar,
                   int32_t* p_अङ्कः_Substitutions,
                   UErrorCode* pErrorCode) {
    int32_t reqLength=0;
    uint32_t ch=0,ch2=0;
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *pDestLimit = (pDest!=nullptr)?(pDest + destCapacity):nullptr;
    int32_t अङ्कः_Substitutions;

    /* args check */
    जइ(U_FAILURE(*pErrorCode)){
        प्रत्यागम nullptr;
    }
        
    जइ( (pSrc==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }

    जइ(p_अङ्कः_Substitutions!=nullptr) {
        *p_अङ्कः_Substitutions=0;
    }
    अङ्कः_Substitutions=0;

    जइ(srcLength==-1) {
        जाव((ch=*pSrc)!=0) {
            ++pSrc;
            जइ(ch <= 0x7f) {
                जइ(pDest<pDestLimit) {
                    *pDest++ = (uint8_t)ch;
                } अण्णहा {
                    reqLength = 1;
                    भङ्ग;
                }
            } अण्णहा जइ(ch <= 0x7ff) {
                जइ((pDestLimit - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अण्णहा {
                    reqLength = 2;
                    भङ्ग;
                }
            } अण्णहा जइ(ch <= 0xd7ff || ch >= 0xe000) {
                जइ((pDestLimit - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अण्णहा {
                    reqLength = 3;
                    भङ्ग;
                }
            } अण्णहा /* ch is a surrogate */ {
                int32_t length;

                /*need not check for NUL because NUL fails U16_IS_TRAIL() anyway*/
                जइ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } अण्णहा जइ(subchar>=0) {
                    ch=subchar;
                    ++अङ्कः_Substitutions;
                } अण्णहा {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    प्रत्यागम nullptr;
                }

                length = U8_LENGTH(ch);
                जइ((pDestLimit - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } अण्णहा {
                    reqLength = length;
                    भङ्ग;
                }
            }
        }
        जाव((ch=*pSrc++)!=0) {
            जइ(ch<=0x7f) {
                ++reqLength;
            } अण्णहा जइ(ch<=0x7ff) {
                reqLength+=2;
            } अण्णहा जइ(!U16_IS_SURROGATE(ch)) {
                reqLength+=3;
            } अण्णहा जइ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                reqLength+=4;
            } अण्णहा जइ(subchar>=0) {
                reqLength+=U8_LENGTH(subchar);
                ++अङ्कः_Substitutions;
            } अण्णहा {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                प्रत्यागम nullptr;
            }
        }
    } अण्णहा {
        ध्रुव char16_t *pSrcLimit = (pSrc!=nullptr)?(pSrc+srcLength):nullptr;
        int32_t count;

        /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
        प्रति(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            count = (int32_t)((pDestLimit - pDest) / 3);
            srcLength = (int32_t)(pSrcLimit - pSrc);
            जइ(count > srcLength) {
                count = srcLength; /* min(remaining dest/3, remaining src) */
            }
            जइ(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                भङ्ग;
            }
            करेहि {
                ch=*pSrc++;
                जइ(ch <= 0x7f) {
                    *pDest++ = (uint8_t)ch;
                } अण्णहा जइ(ch <= 0x7ff) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अण्णहा जइ(ch <= 0xd7ff || ch >= 0xe000) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अण्णहा /* ch is a surrogate */ {
                    /*
                     * We will read two UChars and probably output four bytes,
                     * which we didn't account for with computing count,
                     * so we adjust it here.
                     */
                    जइ(--count == 0) {
                        --pSrc; /* undo ch=*pSrc++ for the lead surrogate */
                        भङ्ग;  /* recompute count */
                    }

                    जइ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                        ++pSrc;
                        ch=U16_GET_SUPPLEMENTARY(ch, ch2);

                        /* writing 4 bytes per 2 UChars is ok */
                        *pDest++=(uint8_t)((ch>>18)|0xf0);
                        *pDest++=(uint8_t)(((ch>>12)&0x3f)|0x80);
                        *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                        *pDest++=(uint8_t)((ch&0x3f)|0x80);
                    } अण्णहा  {
                        /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                        जइ(subchar>=0) {
                            ch=subchar;
                            ++अङ्कः_Substitutions;
                        } अण्णहा {
                            *pErrorCode = U_INVALID_CHAR_FOUND;
                            प्रत्यागम nullptr;
                        }

                        /* convert and append*/
                        pDest=_appendUTF8(pDest, ch);
                    }
                }
            } जाव(--count > 0);
        }

        जाव(pSrc<pSrcLimit) {
            ch=*pSrc++;
            जइ(ch <= 0x7f) {
                जइ(pDest<pDestLimit) {
                    *pDest++ = (uint8_t)ch;
                } अण्णहा {
                    reqLength = 1;
                    भङ्ग;
                }
            } अण्णहा जइ(ch <= 0x7ff) {
                जइ((pDestLimit - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अण्णहा {
                    reqLength = 2;
                    भङ्ग;
                }
            } अण्णहा जइ(ch <= 0xd7ff || ch >= 0xe000) {
                जइ((pDestLimit - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अण्णहा {
                    reqLength = 3;
                    भङ्ग;
                }
            } अण्णहा /* ch is a surrogate */ {
                int32_t length;

                जइ(U16_IS_SURROGATE_LEAD(ch) && pSrc<pSrcLimit && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } अण्णहा जइ(subchar>=0) {
                    ch=subchar;
                    ++अङ्कः_Substitutions;
                } अण्णहा {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    प्रत्यागम nullptr;
                }

                length = U8_LENGTH(ch);
                जइ((pDestLimit - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } अण्णहा {
                    reqLength = length;
                    भङ्ग;
                }
            }
        }
        जाव(pSrc<pSrcLimit) {
            ch=*pSrc++;
            जइ(ch<=0x7f) {
                ++reqLength;
            } अण्णहा जइ(ch<=0x7ff) {
                reqLength+=2;
            } अण्णहा जइ(!U16_IS_SURROGATE(ch)) {
                reqLength+=3;
            } अण्णहा जइ(U16_IS_SURROGATE_LEAD(ch) && pSrc<pSrcLimit && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                reqLength+=4;
            } अण्णहा जइ(subchar>=0) {
                reqLength+=U8_LENGTH(subchar);
                ++अङ्कः_Substitutions;
            } अण्णहा {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                प्रत्यागम nullptr;
            }
        }
    }

    reqLength+=(int32_t)(pDest - (uint8_t *)dest);

    जइ(p_अङ्कः_Substitutions!=nullptr) {
        *p_अङ्कः_Substitutions=अङ्कः_Substitutions;
    }

    जइ(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
    प्रत्यागम dest;
}

U_CAPI वर्ण* U_EXPORT2
u_strToUTF8(वर्ण* dest U_LIFETIME_BOUND,
            int32_t destCapacity,
            int32_t* pDestLength,
            ध्रुव char16_t* pSrc,
            int32_t srcLength,
            UErrorCode* pErrorCode) {
    प्रत्यागम u_strToUTF8WithSub(
            dest, destCapacity, pDestLength,
            pSrc, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromJavaModifiedUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                                 int32_t destCapacity,
                                 int32_t* pDestLength,
                                 ध्रुव वर्ण* src,
                                 int32_t srcLength,
                                 UChar32 subchar,
                                 int32_t* p_अङ्कः_Substitutions,
                                 UErrorCode* pErrorCode) {
    /* args check */
    जइ(U_FAILURE(*pErrorCode)) {
        प्रत्यागम nullptr;
    }
    जइ( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (dest==nullptr && destCapacity!=0) || destCapacity<0 ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }

    जइ(p_अङ्कः_Substitutions!=nullptr) {
        *p_अङ्कः_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *pDestLimit = dest+destCapacity;
    int32_t reqLength = 0;
    int32_t अङ्कः_Substitutions=0;

    जइ(srcLength < 0) {
        /*
         * Transform a NUL-terminated ASCII string.
         * Handle non-ASCII strings with slower code.
         */
        UChar32 c;
        जाव(((c = (uint8_t)*src) != 0) && c <= 0x7f && (pDest < pDestLimit)) {
            *pDest++=(char16_t)c;
            ++src;
        }
        जइ(c == 0) {
            reqLength=(int32_t)(pDest - dest);
            जइ(pDestLength) {
                *pDestLength = reqLength;
            }

            /* Terminate the buffer */
            u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
            प्रत्यागम dest;
        }
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    /* Faster loop without ongoing checking for srcLength and pDestLimit. */
    UChar32 ch;
    uint8_t t1, t2;
    int32_t i = 0;
    प्रति(;;) {
        int32_t count = (int32_t)(pDestLimit - pDest);
        int32_t count2 = srcLength - i;
        जइ(count >= count2 && srcLength > 0 && U8_IS_SINGLE(*src)) {
            /* fast ASCII loop */
            int32_t start = i;
            uint8_t b;
            जाव(i < srcLength && U8_IS_SINGLE(b = src[i])) {
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
        जइ(subchar > 0xFFFF) {
            भङ्ग;
        }
        count2 /= 3;
        जइ(count > count2) {
            count = count2; /* min(remaining dest, remaining src/3) */
        }
        जइ(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            भङ्ग;
        }
        करेहि {
            ch = (uint8_t)src[i++];
            जइ(U8_IS_SINGLE(ch)) {
                *pDest++=(char16_t)ch;
            } अण्णहा {
                जइ(ch >= 0xe0) {
                    जइ( /* handle U+0000..U+FFFF inline */
                        ch <= 0xef &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                        (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                    ) {
                        /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                        *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                        i += 2;
                        अनुवर्त;
                    }
                } अण्णहा {
                    जइ( /* handle U+0000..U+07FF inline */
                        ch >= 0xc0 &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                    ) {
                        *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                        ++i;
                        अनुवर्त;
                    }
                }

                जइ(subchar < 0) {
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    प्रत्यागम nullptr;
                } अण्णहा जइ(subchar > 0xffff && --count == 0) {
                    /*
                     * We need to write two UChars, adjusted count for that,
                     * and ran out of space.
                     */
                    --i;  // back out byte ch
                    भङ्ग;
                } अण्णहा {
                    /* function call for error cases */
                    utf8_nextCharSafeBody((ध्रुव uint8_t *)src, &(i), srcLength, ch, -1);
                    ++अङ्कः_Substitutions;
                    *(pDest++)=(char16_t)subchar;
                }
            }
        } जाव(--count > 0);
    }

    जाव(i < srcLength && (pDest < pDestLimit)) {
        ch = (uint8_t)src[i++];
        जइ(U8_IS_SINGLE(ch)){
            *pDest++=(char16_t)ch;
        } अण्णहा {
            जइ(ch >= 0xe0) {
                जइ( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < srcLength &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                    (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                ) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                    i += 2;
                    अनुवर्त;
                }
            } अण्णहा {
                जइ( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < srcLength &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                ) {
                    *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                    ++i;
                    अनुवर्त;
                }
            }

            जइ(subchar < 0) {
                *pErrorCode = U_INVALID_CHAR_FOUND;
                प्रत्यागम nullptr;
            } अण्णहा {
                /* function call for error cases */
                utf8_nextCharSafeBody((ध्रुव uint8_t *)src, &(i), srcLength, ch, -1);
                ++अङ्कः_Substitutions;
                जइ(subchar<=0xFFFF) {
                    *(pDest++)=(char16_t)subchar;
                } अण्णहा {
                    *(pDest++)=U16_LEAD(subchar);
                    जइ(pDest<pDestLimit) {
                        *(pDest++)=U16_TRAIL(subchar);
                    } अण्णहा {
                        reqLength++;
                        भङ्ग;
                    }
                }
            }
        }
    }

    /* Pre-flight the rest of the string. */
    जाव(i < srcLength) {
        ch = (uint8_t)src[i++];
        जइ(U8_IS_SINGLE(ch)) {
            reqLength++;
        } अण्णहा {
            जइ(ch >= 0xe0) {
                जइ( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < srcLength &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f &&
                    (uint8_t)(src[i+1] - 0x80) <= 0x3f
                ) {
                    reqLength++;
                    i += 2;
                    अनुवर्त;
                }
            } अण्णहा {
                जइ( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < srcLength &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f
                ) {
                    reqLength++;
                    ++i;
                    अनुवर्त;
                }
            }

            जइ(subchar < 0) {
                *pErrorCode = U_INVALID_CHAR_FOUND;
                प्रत्यागम nullptr;
            } अण्णहा {
                /* function call for error cases */
                utf8_nextCharSafeBody((ध्रुव uint8_t *)src, &(i), srcLength, ch, -1);
                ++अङ्कः_Substitutions;
                reqLength+=U16_LENGTH(ch);
            }
        }
    }

    जइ(p_अङ्कः_Substitutions!=nullptr) {
        *p_अङ्कः_Substitutions=अङ्कः_Substitutions;
    }

    reqLength+=(int32_t)(pDest - dest);
    जइ(pDestLength) {
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
    प्रत्यागम dest;
}

U_CAPI वर्ण* U_EXPORT2
u_strToJavaModifiedUTF8(वर्ण* dest U_LIFETIME_BOUND,
                        int32_t destCapacity,
                        int32_t* pDestLength,
                        ध्रुव char16_t* src,
                        int32_t srcLength,
                        UErrorCode* pErrorCode) {
    int32_t reqLength=0;
    uint32_t ch=0;
    ध्रुव char16_t *pSrcLimit;
    int32_t count;

    /* args check */
    जइ(U_FAILURE(*pErrorCode)){
        प्रत्यागम nullptr;
    }
    जइ( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (dest==nullptr && destCapacity!=0) || destCapacity<0
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *pDestLimit = pDest + destCapacity;

    जइ(srcLength==-1) {
        /* Convert NUL-terminated ASCII, then find the string length. */
        जाव((ch=*src)<=0x7f && ch != 0 && pDest<pDestLimit) {
            *pDest++ = (uint8_t)ch;
            ++src;
        }
        जइ(ch == 0) {
            reqLength=(int32_t)(pDest - (uint8_t *)dest);
            जइ(pDestLength) {
                *pDestLength = reqLength;
            }

            /* Terminate the buffer */
            u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
            प्रत्यागम dest;
        }
        srcLength = u_strlen(src);
    }

    /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
    pSrcLimit = (src!=nullptr)?(src+srcLength):nullptr;
    प्रति(;;) {
        count = (int32_t)(pDestLimit - pDest);
        srcLength = (int32_t)(pSrcLimit - src);
        जइ(count >= srcLength && srcLength > 0 && *src <= 0x7f) {
            /* fast ASCII loop */
            ध्रुव char16_t *prevSrc = src;
            int32_t delta;
            जाव(src < pSrcLimit && (ch = *src) <= 0x7f && ch != 0) {
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
        जइ(count > srcLength) {
            count = srcLength; /* min(remaining dest/3, remaining src) */
        }
        जइ(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            भङ्ग;
        }
        करेहि {
            ch=*src++;
            जइ(ch <= 0x7f && ch != 0) {
                *pDest++ = (uint8_t)ch;
            } अण्णहा जइ(ch <= 0x7ff) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } अण्णहा {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            }
        } जाव(--count > 0);
    }

    जाव(src<pSrcLimit) {
        ch=*src++;
        जइ(ch <= 0x7f && ch != 0) {
            जइ(pDest<pDestLimit) {
                *pDest++ = (uint8_t)ch;
            } अण्णहा {
                reqLength = 1;
                भङ्ग;
            }
        } अण्णहा जइ(ch <= 0x7ff) {
            जइ((pDestLimit - pDest) >= 2) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } अण्णहा {
                reqLength = 2;
                भङ्ग;
            }
        } अण्णहा {
            जइ((pDestLimit - pDest) >= 3) {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } अण्णहा {
                reqLength = 3;
                भङ्ग;
            }
        }
    }
    जाव(src<pSrcLimit) {
        ch=*src++;
        जइ(ch <= 0x7f && ch != 0) {
            ++reqLength;
        } अण्णहा जइ(ch<=0x7ff) {
            reqLength+=2;
        } अण्णहा {
            reqLength+=3;
        }
    }

    reqLength+=(int32_t)(pDest - (uint8_t *)dest);
    जइ(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
    प्रत्यागम dest;
}
