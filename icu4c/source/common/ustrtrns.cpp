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


#شامل "unicode/putil.h"
#شامل "unicode/ustring.h"
#شامل "unicode/utf.h"
#شامل "unicode/utf8.h"
#شامل "unicode/utf16.h"
#شامل "cstring.h"
#شامل "cmemory.h"
#شامل "ustr_imp.h"
#شامل "uassert.h"

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32WithSub(char16_t* dest U_LIFETIME_BOUND,
                      int32_t destCapacity,
                      int32_t* pDestLength,
                      ثابت UChar32* src,
                      int32_t srcLength,
                      UChar32 subchar,
                      int32_t* p_رقم_Substitutions,
                      UErrorCode* pErrorCode) {
    ثابت UChar32 *srcLimit;
    UChar32 ch;
    char16_t *destLimit;
    char16_t *pDest;
    int32_t reqLength;
    int32_t رقم_Substitutions;

    /* args check */
    اگر(U_FAILURE(*pErrorCode)){
        بازگشت nullptr;
    }
    اگر( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }

    اگر(p_رقم_Substitutions != nullptr) {
        *p_رقم_Substitutions = 0;
    }

    pDest = dest;
    destLimit = (dest!=nullptr)?(dest + destCapacity):nullptr;
    reqLength = 0;
    رقم_Substitutions = 0;

    اگر(srcLength < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        وقتی((ch=*src) != 0 &&
              ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff))) {
            ++src;
            اگر(pDest < destLimit) {
                *pDest++ = (char16_t)ch;
            } وگرنه {
                ++reqLength;
            }
        }
        srcLimit = src;
        اگر(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            وقتی(*++srcLimit != 0) {}
        }
    } وگرنه {
      srcLimit = (src!=nullptr)?(src + srcLength):nullptr;
    }

    /* convert with length */
    وقتی(src < srcLimit) {
        ch = *src++;
        انجامبده {
            /* usually "loops" once; twice only for writing subchar */
            اگر((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff)) {
                اگر(pDest < destLimit) {
                    *pDest++ = (char16_t)ch;
                } وگرنه {
                    ++reqLength;
                }
                بشکن;
            } وگرنه اگر(0x10000 <= ch && ch <= 0x10ffff) {
                اگر(pDest!=nullptr && ((pDest + 2) <= destLimit)) {
                    *pDest++ = U16_LEAD(ch);
                    *pDest++ = U16_TRAIL(ch);
                } وگرنه {
                    reqLength += 2;
                }
                بشکن;
            } وگرنه اگر((ch = subchar) < 0) {
                /* surrogate code point, or not a Unicode code point at all */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                بازگشت nullptr;
            } وگرنه {
                ++رقم_Substitutions;
            }
        } وقتی(درست);
    }

    reqLength += (int32_t)(pDest - dest);
    اگر(pDestLength) {
        *pDestLength = reqLength;
    }
    اگر(p_رقم_Substitutions != nullptr) {
        *p_رقم_Substitutions = رقم_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
    
    بازگشت dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32(char16_t* dest U_LIFETIME_BOUND,
               int32_t destCapacity,
               int32_t* pDestLength,
               ثابت UChar32* src,
               int32_t srcLength,
               UErrorCode* pErrorCode) {
    بازگشت u_strFromUTF32WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI UChar32* U_EXPORT2
u_strToUTF32WithSub(UChar32* dest U_LIFETIME_BOUND,
                    int32_t destCapacity,
                    int32_t* pDestLength,
                    ثابت char16_t* src,
                    int32_t srcLength,
                    UChar32 subchar,
                    int32_t* p_رقم_Substitutions,
                    UErrorCode* pErrorCode) {
    ثابت char16_t *srcLimit;
    UChar32 ch;
    char16_t ch2;
    UChar32 *destLimit;
    UChar32 *pDest;
    int32_t reqLength;
    int32_t رقم_Substitutions;

    /* args check */
    اگر(U_FAILURE(*pErrorCode)){
        بازگشت nullptr;
    }
    اگر( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }

    اگر(p_رقم_Substitutions != nullptr) {
        *p_رقم_Substitutions = 0;
    }

    pDest = dest;
    destLimit = (dest!=nullptr)?(dest + destCapacity):nullptr;
    reqLength = 0;
    رقم_Substitutions = 0;

    اگر(srcLength < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        وقتی((ch=*src) != 0 && !U16_IS_SURROGATE(ch)) {
            ++src;
            اگر(pDest < destLimit) {
                *pDest++ = ch;
            } وگرنه {
                ++reqLength;
            }
        }
        srcLimit = src;
        اگر(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            وقتی(*++srcLimit != 0) {}
        }
    } وگرنه {
        srcLimit = (src!=nullptr)?(src + srcLength):nullptr;
    }

    /* convert with length */
    وقتی(src < srcLimit) {
        ch = *src++;
        اگر(!U16_IS_SURROGATE(ch)) {
            /* write or count ch below */
        } وگرنه اگر(U16_IS_SURROGATE_LEAD(ch) && src < srcLimit && U16_IS_TRAIL(ch2 = *src)) {
            ++src;
            ch = U16_GET_SUPPLEMENTARY(ch, ch2);
        } وگرنه اگر((ch = subchar) < 0) {
            /* unpaired surrogate */
            *pErrorCode = U_INVALID_CHAR_FOUND;
            بازگشت nullptr;
        } وگرنه {
            ++رقم_Substitutions;
        }
        اگر(pDest < destLimit) {
            *pDest++ = ch;
        } وگرنه {
            ++reqLength;
        }
    }

    reqLength += (int32_t)(pDest - dest);
    اگر(pDestLength) {
        *pDestLength = reqLength;
    }
    اگر(p_رقم_Substitutions != nullptr) {
        *p_رقم_Substitutions = رقم_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChar32s(dest, destCapacity, reqLength, pErrorCode);

    بازگشت dest;
}

U_CAPI UChar32* U_EXPORT2
u_strToUTF32(UChar32* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* pDestLength,
             ثابت char16_t* src,
             int32_t srcLength,
             UErrorCode* pErrorCode) {
    بازگشت u_strToUTF32WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                     int32_t destCapacity,
                     int32_t* pDestLength,
                     ثابت نویسه* src,
                     int32_t srcLength,
                     UChar32 subchar,
                     int32_t* p_رقم_Substitutions,
                     UErrorCode* pErrorCode) {
    /* args check */
    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت nullptr;
    }
    اگر( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }

    اگر(p_رقم_Substitutions!=nullptr) {
        *p_رقم_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *pDestLimit = dest+destCapacity;
    int32_t reqLength = 0;
    int32_t رقم_Substitutions=0;

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

    اگر(srcLength < 0){
        /*
         * Transform a NUL-terminated string.
         * The code explicitly checks for NULs only in the lead byte position.
         * A NUL byte in the trail byte position fails the trail byte range check anyway.
         */
        int32_t i;
        UChar32 c;
        برای(i = 0; (c = (uint8_t)src[i]) != 0 && (pDest < pDestLimit);) {
            // modified copy of U8_NEXT()
            ++i;
            اگر(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } وگرنه {
                uint8_t __t1, __t2;
                اگر( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } وگرنه اگر( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } وگرنه {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((ثابت uint8_t *)src, &(i), -1, c, -1);
                    اگر(c<0 && (++رقم_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        بازگشت nullptr;
                    } وگرنه اگر(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } وگرنه {
                        *(pDest++)=U16_LEAD(c);
                        اگر(pDest<pDestLimit) {
                            *(pDest++)=U16_TRAIL(c);
                        } وگرنه {
                            reqLength++;
                            بشکن;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        وقتی((c = (uint8_t)src[i]) != 0) {
            // modified copy of U8_NEXT()
            ++i;
            اگر(U8_IS_SINGLE(c)) {
                ++reqLength;
            } وگرنه {
                uint8_t __t1, __t2;
                اگر( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++reqLength;
                    i+=2;
                } وگرنه اگر( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++reqLength;
                    ++(i);
                } وگرنه {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((ثابت uint8_t *)src, &(i), -1, c, -1);
                    اگر(c<0 && (++رقم_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        بازگشت nullptr;
                    }
                    reqLength += U16_LENGTH(c);
                }
            }
        }
    } وگرنه /* srcLength >= 0 */ {
        /* Faster loop without ongoing checking for srcLength and pDestLimit. */
        int32_t i = 0;
        UChar32 c;
        برای(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            int32_t count = (int32_t)(pDestLimit - pDest);
            int32_t count2 = (srcLength - i) / 3;
            اگر(count > count2) {
                count = count2; /* min(remaining dest, remaining src/3) */
            }
            اگر(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                بشکن;
            }

            انجامبده {
                // modified copy of U8_NEXT()
                c = (uint8_t)src[i++];
                اگر(U8_IS_SINGLE(c)) {
                    *pDest++=(char16_t)c;
                } وگرنه {
                    uint8_t __t1, __t2;
                    اگر( /* handle U+0800..U+FFFF inline */
                            (0xe0<=(c) && (c)<0xf0) &&
                            ((i)+1)<srcLength &&
                            U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                            (__t2=src[(i)+1]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                        i+=2;
                    } وگرنه اگر( /* handle U+0080..U+07FF inline */
                            ((c)<0xe0 && (c)>=0xc2) &&
                            ((i)!=srcLength) &&
                            (__t1=src[i]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0x1f)<<6)|__t1;
                        ++(i);
                    } وگرنه {
                        اگر(c >= 0xf0 || subchar > 0xffff) {
                            // We may read up to four bytes and write up to two UChars,
                            // which we didn't account for with computing count,
                            // so we adjust it here.
                            اگر(--count == 0) {
                                --i;  // back out byte c
                                بشکن;
                            }
                        }

                        /* function call for "complicated" and error cases */
                        (c)=utf8_nextCharSafeBody((ثابت uint8_t *)src, &(i), srcLength, c, -1);
                        اگر(c<0 && (++رقم_Substitutions, c = subchar) < 0) {
                            *pErrorCode = U_INVALID_CHAR_FOUND;
                            بازگشت nullptr;
                        } وگرنه اگر(c<=0xFFFF) {
                            *(pDest++)=(char16_t)c;
                        } وگرنه {
                            *(pDest++)=U16_LEAD(c);
                            *(pDest++)=U16_TRAIL(c);
                        }
                    }
                }
            } وقتی(--count > 0);
        }

        وقتی(i < srcLength && (pDest < pDestLimit)) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            اگر(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } وگرنه {
                uint8_t __t1, __t2;
                اگر( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<srcLength &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } وگرنه اگر( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=srcLength) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } وگرنه {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((ثابت uint8_t *)src, &(i), srcLength, c, -1);
                    اگر(c<0 && (++رقم_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        بازگشت nullptr;
                    } وگرنه اگر(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } وگرنه {
                        *(pDest++)=U16_LEAD(c);
                        اگر(pDest<pDestLimit) {
                            *(pDest++)=U16_TRAIL(c);
                        } وگرنه {
                            reqLength++;
                            بشکن;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        وقتی(i < srcLength) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            اگر(U8_IS_SINGLE(c)) {
                ++reqLength;
            } وگرنه {
                uint8_t __t1, __t2;
                اگر( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<srcLength &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++reqLength;
                    i+=2;
                } وگرنه اگر( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=srcLength) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++reqLength;
                    ++(i);
                } وگرنه {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((ثابت uint8_t *)src, &(i), srcLength, c, -1);
                    اگر(c<0 && (++رقم_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        بازگشت nullptr;
                    }
                    reqLength += U16_LENGTH(c);
                }
            }
        }
    }

    reqLength+=(int32_t)(pDest - dest);

    اگر(p_رقم_Substitutions!=nullptr) {
        *p_رقم_Substitutions=رقم_Substitutions;
    }

    اگر(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,reqLength,pErrorCode);

    بازگشت dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8(char16_t* dest U_LIFETIME_BOUND,
              int32_t destCapacity,
              int32_t* pDestLength,
              ثابت نویسه* src,
              int32_t srcLength,
              UErrorCode* pErrorCode) {
    بازگشت u_strFromUTF8WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8Lenient(char16_t* dest U_LIFETIME_BOUND,
                     int32_t destCapacity,
                     int32_t* pDestLength,
                     ثابت نویسه* src,
                     int32_t srcLength,
                     UErrorCode* pErrorCode) {
    char16_t *pDest = dest;
    UChar32 ch;
    int32_t reqLength = 0;
    uint8_t* pSrc = (uint8_t*) src;

    /* args check */
    اگر(U_FAILURE(*pErrorCode)){
        بازگشت nullptr;
    }
        
    اگر( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }

    اگر(srcLength < 0) {
        /* Transform a NUL-terminated string. */
        char16_t *pDestLimit = (dest!=nullptr)?(dest+destCapacity):nullptr;
        uint8_t t1, t2, t3; /* trail bytes */

        وقتی(((ch = *pSrc) != 0) && (pDest < pDestLimit)) {
            اگر(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ++pSrc;
                ادامه;
            } وگرنه اگر(ch < 0xe0) { /* U+0080..U+07FF */
                اگر((t1 = pSrc[1]) != 0) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + t1 - 0x3080);
                    pSrc += 2;
                    ادامه;
                }
            } وگرنه اگر(ch < 0xf0) { /* U+0800..U+FFFF */
                اگر((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 12) + (t1 << 6) + t2 - 0x2080);
                    pSrc += 3;
                    ادامه;
                }
            } وگرنه /* f0..f4 */ { /* U+10000..U+10FFFF */
                اگر((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0 && (t3 = pSrc[3]) != 0) {
                    pSrc += 4;
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (t1 << 12) + (t2 << 6) + t3 - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    اگر(pDest < pDestLimit) {
                        *(pDest++) = U16_TRAIL(ch);
                    } وگرنه {
                        reqLength = 1;
                        بشکن;
                    }
                    ادامه;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            وقتی(*++pSrc != 0) {}
            بشکن;
        }

        /* Pre-flight the rest of the string. */
        وقتی((ch = *pSrc) != 0) {
            اگر(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                ++reqLength;
                ++pSrc;
                ادامه;
            } وگرنه اگر(ch < 0xe0) { /* U+0080..U+07FF */
                اگر(pSrc[1] != 0) {
                    ++reqLength;
                    pSrc += 2;
                    ادامه;
                }
            } وگرنه اگر(ch < 0xf0) { /* U+0800..U+FFFF */
                اگر(pSrc[1] != 0 && pSrc[2] != 0) {
                    ++reqLength;
                    pSrc += 3;
                    ادامه;
                }
            } وگرنه /* f0..f4 */ { /* U+10000..U+10FFFF */
                اگر(pSrc[1] != 0 && pSrc[2] != 0 && pSrc[3] != 0) {
                    reqLength += 2;
                    pSrc += 4;
                    ادامه;
                }
            }

            /* truncated character at the end */
            ++reqLength;
            بشکن;
        }
    } وگرنه /* srcLength >= 0 */ {
      ثابت uint8_t *pSrcLimit = (pSrc!=nullptr)?(pSrc + srcLength):nullptr;

        /*
         * This function requires that if srcLength is given, then it must be
         * destCapatity >= srcLength so that we need not check for
         * destination buffer overflow in the loop.
         */
        اگر(destCapacity < srcLength) {
            اگر(pDestLength != nullptr) {
                *pDestLength = srcLength; /* this likely overestimates the true destLength! */
            }
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            بازگشت nullptr;
        }

        اگر((pSrcLimit - pSrc) >= 4) {
            pSrcLimit -= 3; /* temporarily reduce pSrcLimit */

            /* in this loop, we can always access at least 4 bytes, up to pSrc+3 */
            انجامبده {
                ch = *pSrc++;
                اگر(ch < 0xc0) {
                    /*
                     * ASCII, or a trail byte in lead position which is treated like
                     * a single-byte sequence for better character boundary
                     * resynchronization after illegal sequences.
                     */
                    *pDest++=(char16_t)ch;
                } وگرنه اگر(ch < 0xe0) { /* U+0080..U+07FF */
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                } وگرنه اگر(ch < 0xf0) { /* U+0800..U+FFFF */
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                } وگرنه /* f0..f4 */ { /* U+10000..U+10FFFF */
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                }
            } وقتی(pSrc < pSrcLimit);

            pSrcLimit += 3; /* restore original pSrcLimit */
        }

        وقتی(pSrc < pSrcLimit) {
            ch = *pSrc++;
            اگر(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ادامه;
            } وگرنه اگر(ch < 0xe0) { /* U+0080..U+07FF */
                اگر(pSrc < pSrcLimit) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                    ادامه;
                }
            } وگرنه اگر(ch < 0xf0) { /* U+0800..U+FFFF */
                اگر((pSrcLimit - pSrc) >= 2) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                    pSrc += 3;
                    ادامه;
                }
            } وگرنه /* f0..f4 */ { /* U+10000..U+10FFFF */
                اگر((pSrcLimit - pSrc) >= 3) {
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                    pSrc += 4;
                    ادامه;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            بشکن;
        }
    }

    reqLength+=(int32_t)(pDest - dest);

    اگر(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,reqLength,pErrorCode);

    بازگشت dest;
}

ایستا درونخطی uint8_t *
_appendUTF8(uint8_t *pDest, UChar32 c) {
    /* it is 0<=c<=0x10ffff and not a surrogate if called by a validating function */
    اگر((c)<=0x7f) {
        *pDest++ = static_cast<uint8_t>(c);
    } وگرنه اگر(c<=0x7ff) {
        *pDest++ = static_cast<uint8_t>((c >> 6) | 0xc0);
        *pDest++ = static_cast<uint8_t>((c & 0x3f) | 0x80);
    } وگرنه اگر(c<=0xffff) {
        *pDest++ = static_cast<uint8_t>((c >> 12) | 0xe0);
        *pDest++ = static_cast<uint8_t>(((c >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    } وگرنه /* if((uint32_t)(c)<=0x10ffff) */ {
        *pDest++ = static_cast<uint8_t>(((c) >> 18) | 0xf0);
        *pDest++ = static_cast<uint8_t>((((c) >> 12) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>((((c) >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    }
    بازگشت pDest;
}

U_CAPI نویسه* U_EXPORT2
u_strToUTF8WithSub(نویسه* dest U_LIFETIME_BOUND,
                   int32_t destCapacity,
                   int32_t* pDestLength,
                   ثابت char16_t* pSrc,
                   int32_t srcLength,
                   UChar32 subchar,
                   int32_t* p_رقم_Substitutions,
                   UErrorCode* pErrorCode) {
    int32_t reqLength=0;
    uint32_t ch=0,ch2=0;
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *pDestLimit = (pDest!=nullptr)?(pDest + destCapacity):nullptr;
    int32_t رقم_Substitutions;

    /* args check */
    اگر(U_FAILURE(*pErrorCode)){
        بازگشت nullptr;
    }
        
    اگر( (pSrc==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }

    اگر(p_رقم_Substitutions!=nullptr) {
        *p_رقم_Substitutions=0;
    }
    رقم_Substitutions=0;

    اگر(srcLength==-1) {
        وقتی((ch=*pSrc)!=0) {
            ++pSrc;
            اگر(ch <= 0x7f) {
                اگر(pDest<pDestLimit) {
                    *pDest++ = (uint8_t)ch;
                } وگرنه {
                    reqLength = 1;
                    بشکن;
                }
            } وگرنه اگر(ch <= 0x7ff) {
                اگر((pDestLimit - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } وگرنه {
                    reqLength = 2;
                    بشکن;
                }
            } وگرنه اگر(ch <= 0xd7ff || ch >= 0xe000) {
                اگر((pDestLimit - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } وگرنه {
                    reqLength = 3;
                    بشکن;
                }
            } وگرنه /* ch is a surrogate */ {
                int32_t length;

                /*need not check for NUL because NUL fails U16_IS_TRAIL() anyway*/
                اگر(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } وگرنه اگر(subchar>=0) {
                    ch=subchar;
                    ++رقم_Substitutions;
                } وگرنه {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    بازگشت nullptr;
                }

                length = U8_LENGTH(ch);
                اگر((pDestLimit - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } وگرنه {
                    reqLength = length;
                    بشکن;
                }
            }
        }
        وقتی((ch=*pSrc++)!=0) {
            اگر(ch<=0x7f) {
                ++reqLength;
            } وگرنه اگر(ch<=0x7ff) {
                reqLength+=2;
            } وگرنه اگر(!U16_IS_SURROGATE(ch)) {
                reqLength+=3;
            } وگرنه اگر(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                reqLength+=4;
            } وگرنه اگر(subchar>=0) {
                reqLength+=U8_LENGTH(subchar);
                ++رقم_Substitutions;
            } وگرنه {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                بازگشت nullptr;
            }
        }
    } وگرنه {
        ثابت char16_t *pSrcLimit = (pSrc!=nullptr)?(pSrc+srcLength):nullptr;
        int32_t count;

        /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
        برای(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            count = (int32_t)((pDestLimit - pDest) / 3);
            srcLength = (int32_t)(pSrcLimit - pSrc);
            اگر(count > srcLength) {
                count = srcLength; /* min(remaining dest/3, remaining src) */
            }
            اگر(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                بشکن;
            }
            انجامبده {
                ch=*pSrc++;
                اگر(ch <= 0x7f) {
                    *pDest++ = (uint8_t)ch;
                } وگرنه اگر(ch <= 0x7ff) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } وگرنه اگر(ch <= 0xd7ff || ch >= 0xe000) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } وگرنه /* ch is a surrogate */ {
                    /*
                     * We will read two UChars and probably output four bytes,
                     * which we didn't account for with computing count,
                     * so we adjust it here.
                     */
                    اگر(--count == 0) {
                        --pSrc; /* undo ch=*pSrc++ for the lead surrogate */
                        بشکن;  /* recompute count */
                    }

                    اگر(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                        ++pSrc;
                        ch=U16_GET_SUPPLEMENTARY(ch, ch2);

                        /* writing 4 bytes per 2 UChars is ok */
                        *pDest++=(uint8_t)((ch>>18)|0xf0);
                        *pDest++=(uint8_t)(((ch>>12)&0x3f)|0x80);
                        *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                        *pDest++=(uint8_t)((ch&0x3f)|0x80);
                    } وگرنه  {
                        /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                        اگر(subchar>=0) {
                            ch=subchar;
                            ++رقم_Substitutions;
                        } وگرنه {
                            *pErrorCode = U_INVALID_CHAR_FOUND;
                            بازگشت nullptr;
                        }

                        /* convert and append*/
                        pDest=_appendUTF8(pDest, ch);
                    }
                }
            } وقتی(--count > 0);
        }

        وقتی(pSrc<pSrcLimit) {
            ch=*pSrc++;
            اگر(ch <= 0x7f) {
                اگر(pDest<pDestLimit) {
                    *pDest++ = (uint8_t)ch;
                } وگرنه {
                    reqLength = 1;
                    بشکن;
                }
            } وگرنه اگر(ch <= 0x7ff) {
                اگر((pDestLimit - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } وگرنه {
                    reqLength = 2;
                    بشکن;
                }
            } وگرنه اگر(ch <= 0xd7ff || ch >= 0xe000) {
                اگر((pDestLimit - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } وگرنه {
                    reqLength = 3;
                    بشکن;
                }
            } وگرنه /* ch is a surrogate */ {
                int32_t length;

                اگر(U16_IS_SURROGATE_LEAD(ch) && pSrc<pSrcLimit && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } وگرنه اگر(subchar>=0) {
                    ch=subchar;
                    ++رقم_Substitutions;
                } وگرنه {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    بازگشت nullptr;
                }

                length = U8_LENGTH(ch);
                اگر((pDestLimit - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } وگرنه {
                    reqLength = length;
                    بشکن;
                }
            }
        }
        وقتی(pSrc<pSrcLimit) {
            ch=*pSrc++;
            اگر(ch<=0x7f) {
                ++reqLength;
            } وگرنه اگر(ch<=0x7ff) {
                reqLength+=2;
            } وگرنه اگر(!U16_IS_SURROGATE(ch)) {
                reqLength+=3;
            } وگرنه اگر(U16_IS_SURROGATE_LEAD(ch) && pSrc<pSrcLimit && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                reqLength+=4;
            } وگرنه اگر(subchar>=0) {
                reqLength+=U8_LENGTH(subchar);
                ++رقم_Substitutions;
            } وگرنه {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                بازگشت nullptr;
            }
        }
    }

    reqLength+=(int32_t)(pDest - (uint8_t *)dest);

    اگر(p_رقم_Substitutions!=nullptr) {
        *p_رقم_Substitutions=رقم_Substitutions;
    }

    اگر(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
    بازگشت dest;
}

U_CAPI نویسه* U_EXPORT2
u_strToUTF8(نویسه* dest U_LIFETIME_BOUND,
            int32_t destCapacity,
            int32_t* pDestLength,
            ثابت char16_t* pSrc,
            int32_t srcLength,
            UErrorCode* pErrorCode) {
    بازگشت u_strToUTF8WithSub(
            dest, destCapacity, pDestLength,
            pSrc, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromJavaModifiedUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                                 int32_t destCapacity,
                                 int32_t* pDestLength,
                                 ثابت نویسه* src,
                                 int32_t srcLength,
                                 UChar32 subchar,
                                 int32_t* p_رقم_Substitutions,
                                 UErrorCode* pErrorCode) {
    /* args check */
    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت nullptr;
    }
    اگر( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (dest==nullptr && destCapacity!=0) || destCapacity<0 ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }

    اگر(p_رقم_Substitutions!=nullptr) {
        *p_رقم_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *pDestLimit = dest+destCapacity;
    int32_t reqLength = 0;
    int32_t رقم_Substitutions=0;

    اگر(srcLength < 0) {
        /*
         * Transform a NUL-terminated ASCII string.
         * Handle non-ASCII strings with slower code.
         */
        UChar32 c;
        وقتی(((c = (uint8_t)*src) != 0) && c <= 0x7f && (pDest < pDestLimit)) {
            *pDest++=(char16_t)c;
            ++src;
        }
        اگر(c == 0) {
            reqLength=(int32_t)(pDest - dest);
            اگر(pDestLength) {
                *pDestLength = reqLength;
            }

            /* Terminate the buffer */
            u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
            بازگشت dest;
        }
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    /* Faster loop without ongoing checking for srcLength and pDestLimit. */
    UChar32 ch;
    uint8_t t1, t2;
    int32_t i = 0;
    برای(;;) {
        int32_t count = (int32_t)(pDestLimit - pDest);
        int32_t count2 = srcLength - i;
        اگر(count >= count2 && srcLength > 0 && U8_IS_SINGLE(*src)) {
            /* fast ASCII loop */
            int32_t start = i;
            uint8_t b;
            وقتی(i < srcLength && U8_IS_SINGLE(b = src[i])) {
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
        اگر(subchar > 0xFFFF) {
            بشکن;
        }
        count2 /= 3;
        اگر(count > count2) {
            count = count2; /* min(remaining dest, remaining src/3) */
        }
        اگر(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            بشکن;
        }
        انجامبده {
            ch = (uint8_t)src[i++];
            اگر(U8_IS_SINGLE(ch)) {
                *pDest++=(char16_t)ch;
            } وگرنه {
                اگر(ch >= 0xe0) {
                    اگر( /* handle U+0000..U+FFFF inline */
                        ch <= 0xef &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                        (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                    ) {
                        /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                        *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                        i += 2;
                        ادامه;
                    }
                } وگرنه {
                    اگر( /* handle U+0000..U+07FF inline */
                        ch >= 0xc0 &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                    ) {
                        *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                        ++i;
                        ادامه;
                    }
                }

                اگر(subchar < 0) {
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    بازگشت nullptr;
                } وگرنه اگر(subchar > 0xffff && --count == 0) {
                    /*
                     * We need to write two UChars, adjusted count for that,
                     * and ran out of space.
                     */
                    --i;  // back out byte ch
                    بشکن;
                } وگرنه {
                    /* function call for error cases */
                    utf8_nextCharSafeBody((ثابت uint8_t *)src, &(i), srcLength, ch, -1);
                    ++رقم_Substitutions;
                    *(pDest++)=(char16_t)subchar;
                }
            }
        } وقتی(--count > 0);
    }

    وقتی(i < srcLength && (pDest < pDestLimit)) {
        ch = (uint8_t)src[i++];
        اگر(U8_IS_SINGLE(ch)){
            *pDest++=(char16_t)ch;
        } وگرنه {
            اگر(ch >= 0xe0) {
                اگر( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < srcLength &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                    (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                ) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                    i += 2;
                    ادامه;
                }
            } وگرنه {
                اگر( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < srcLength &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                ) {
                    *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                    ++i;
                    ادامه;
                }
            }

            اگر(subchar < 0) {
                *pErrorCode = U_INVALID_CHAR_FOUND;
                بازگشت nullptr;
            } وگرنه {
                /* function call for error cases */
                utf8_nextCharSafeBody((ثابت uint8_t *)src, &(i), srcLength, ch, -1);
                ++رقم_Substitutions;
                اگر(subchar<=0xFFFF) {
                    *(pDest++)=(char16_t)subchar;
                } وگرنه {
                    *(pDest++)=U16_LEAD(subchar);
                    اگر(pDest<pDestLimit) {
                        *(pDest++)=U16_TRAIL(subchar);
                    } وگرنه {
                        reqLength++;
                        بشکن;
                    }
                }
            }
        }
    }

    /* Pre-flight the rest of the string. */
    وقتی(i < srcLength) {
        ch = (uint8_t)src[i++];
        اگر(U8_IS_SINGLE(ch)) {
            reqLength++;
        } وگرنه {
            اگر(ch >= 0xe0) {
                اگر( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < srcLength &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f &&
                    (uint8_t)(src[i+1] - 0x80) <= 0x3f
                ) {
                    reqLength++;
                    i += 2;
                    ادامه;
                }
            } وگرنه {
                اگر( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < srcLength &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f
                ) {
                    reqLength++;
                    ++i;
                    ادامه;
                }
            }

            اگر(subchar < 0) {
                *pErrorCode = U_INVALID_CHAR_FOUND;
                بازگشت nullptr;
            } وگرنه {
                /* function call for error cases */
                utf8_nextCharSafeBody((ثابت uint8_t *)src, &(i), srcLength, ch, -1);
                ++رقم_Substitutions;
                reqLength+=U16_LENGTH(ch);
            }
        }
    }

    اگر(p_رقم_Substitutions!=nullptr) {
        *p_رقم_Substitutions=رقم_Substitutions;
    }

    reqLength+=(int32_t)(pDest - dest);
    اگر(pDestLength) {
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
    بازگشت dest;
}

U_CAPI نویسه* U_EXPORT2
u_strToJavaModifiedUTF8(نویسه* dest U_LIFETIME_BOUND,
                        int32_t destCapacity,
                        int32_t* pDestLength,
                        ثابت char16_t* src,
                        int32_t srcLength,
                        UErrorCode* pErrorCode) {
    int32_t reqLength=0;
    uint32_t ch=0;
    ثابت char16_t *pSrcLimit;
    int32_t count;

    /* args check */
    اگر(U_FAILURE(*pErrorCode)){
        بازگشت nullptr;
    }
    اگر( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (dest==nullptr && destCapacity!=0) || destCapacity<0
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *pDestLimit = pDest + destCapacity;

    اگر(srcLength==-1) {
        /* Convert NUL-terminated ASCII, then find the string length. */
        وقتی((ch=*src)<=0x7f && ch != 0 && pDest<pDestLimit) {
            *pDest++ = (uint8_t)ch;
            ++src;
        }
        اگر(ch == 0) {
            reqLength=(int32_t)(pDest - (uint8_t *)dest);
            اگر(pDestLength) {
                *pDestLength = reqLength;
            }

            /* Terminate the buffer */
            u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
            بازگشت dest;
        }
        srcLength = u_strlen(src);
    }

    /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
    pSrcLimit = (src!=nullptr)?(src+srcLength):nullptr;
    برای(;;) {
        count = (int32_t)(pDestLimit - pDest);
        srcLength = (int32_t)(pSrcLimit - src);
        اگر(count >= srcLength && srcLength > 0 && *src <= 0x7f) {
            /* fast ASCII loop */
            ثابت char16_t *prevSrc = src;
            int32_t delta;
            وقتی(src < pSrcLimit && (ch = *src) <= 0x7f && ch != 0) {
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
        اگر(count > srcLength) {
            count = srcLength; /* min(remaining dest/3, remaining src) */
        }
        اگر(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            بشکن;
        }
        انجامبده {
            ch=*src++;
            اگر(ch <= 0x7f && ch != 0) {
                *pDest++ = (uint8_t)ch;
            } وگرنه اگر(ch <= 0x7ff) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } وگرنه {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            }
        } وقتی(--count > 0);
    }

    وقتی(src<pSrcLimit) {
        ch=*src++;
        اگر(ch <= 0x7f && ch != 0) {
            اگر(pDest<pDestLimit) {
                *pDest++ = (uint8_t)ch;
            } وگرنه {
                reqLength = 1;
                بشکن;
            }
        } وگرنه اگر(ch <= 0x7ff) {
            اگر((pDestLimit - pDest) >= 2) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } وگرنه {
                reqLength = 2;
                بشکن;
            }
        } وگرنه {
            اگر((pDestLimit - pDest) >= 3) {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } وگرنه {
                reqLength = 3;
                بشکن;
            }
        }
    }
    وقتی(src<pSrcLimit) {
        ch=*src++;
        اگر(ch <= 0x7f && ch != 0) {
            ++reqLength;
        } وگرنه اگر(ch<=0x7ff) {
            reqLength+=2;
        } وگرنه {
            reqLength+=3;
        }
    }

    reqLength+=(int32_t)(pDest - (uint8_t *)dest);
    اگر(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
    بازگشت dest;
}
