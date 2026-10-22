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


#include "unicode/putil.h"
#include "unicode/ustring.h"
#include "unicode/utf.h"
#include "unicode/utf8.h"
#include "unicode/utf16.h"
#include "cstring.h"
#include "cmemory.h"
#include "ustr_imp.h"
#include "uassert.h"

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32WithSub(char16_t* dest U_LIFETIME_BOUND,
                      int32_t destCapacity,
                      int32_t* pDestLength,
                      const UChar32* src,
                      int32_t srcLength,
                      UChar32 subchar,
                      int32_t* pNumSubstitutions,
                      UErrorCode* pErrorCode) {
    const UChar32 *srcLimit;
    UChar32 ch;
    char16_t *destLimit;
    char16_t *pDest;
    int32_t reqLength;
    int32_t numSubstitutions;

    /* args check */
    ако(U_FAILURE(*pErrorCode)){
        върни nullptr;
    }
    ако( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        върни nullptr;
    }

    ако(pNumSubstitutions != nullptr) {
        *pNumSubstitutions = 0;
    }

    pDest = dest;
    destLimit = (dest!=nullptr)?(dest + destCapacity):nullptr;
    reqLength = 0;
    numSubstitutions = 0;

    ако(srcLength < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        докато((ch=*src) != 0 &&
              ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff))) {
            ++src;
            ако(pDest < destLimit) {
                *pDest++ = (char16_t)ch;
            } иначе {
                ++reqLength;
            }
        }
        srcLimit = src;
        ако(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            докато(*++srcLimit != 0) {}
        }
    } иначе {
      srcLimit = (src!=nullptr)?(src + srcLength):nullptr;
    }

    /* convert with length */
    докато(src < srcLimit) {
        ch = *src++;
        do {
            /* usually "loops" once; twice only for writing subchar */
            ако((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff)) {
                ако(pDest < destLimit) {
                    *pDest++ = (char16_t)ch;
                } иначе {
                    ++reqLength;
                }
                break;
            } иначе ако(0x10000 <= ch && ch <= 0x10ffff) {
                ако(pDest!=nullptr && ((pDest + 2) <= destLimit)) {
                    *pDest++ = U16_LEAD(ch);
                    *pDest++ = U16_TRAIL(ch);
                } иначе {
                    reqLength += 2;
                }
                break;
            } иначе ако((ch = subchar) < 0) {
                /* surrogate code point, or not a Unicode code point at all */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                върни nullptr;
            } иначе {
                ++numSubstitutions;
            }
        } докато(true);
    }

    reqLength += (int32_t)(pDest - dest);
    ако(pDestLength) {
        *pDestLength = reqLength;
    }
    ако(pNumSubstitutions != nullptr) {
        *pNumSubstitutions = numSubstitutions;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
    
    върни dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32(char16_t* dest U_LIFETIME_BOUND,
               int32_t destCapacity,
               int32_t* pDestLength,
               const UChar32* src,
               int32_t srcLength,
               UErrorCode* pErrorCode) {
    върни u_strFromUTF32WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI UChar32* U_EXPORT2
u_strToUTF32WithSub(UChar32* dest U_LIFETIME_BOUND,
                    int32_t destCapacity,
                    int32_t* pDestLength,
                    const char16_t* src,
                    int32_t srcLength,
                    UChar32 subchar,
                    int32_t* pNumSubstitutions,
                    UErrorCode* pErrorCode) {
    const char16_t *srcLimit;
    UChar32 ch;
    char16_t ch2;
    UChar32 *destLimit;
    UChar32 *pDest;
    int32_t reqLength;
    int32_t numSubstitutions;

    /* args check */
    ако(U_FAILURE(*pErrorCode)){
        върни nullptr;
    }
    ако( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        върни nullptr;
    }

    ако(pNumSubstitutions != nullptr) {
        *pNumSubstitutions = 0;
    }

    pDest = dest;
    destLimit = (dest!=nullptr)?(dest + destCapacity):nullptr;
    reqLength = 0;
    numSubstitutions = 0;

    ако(srcLength < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        докато((ch=*src) != 0 && !U16_IS_SURROGATE(ch)) {
            ++src;
            ако(pDest < destLimit) {
                *pDest++ = ch;
            } иначе {
                ++reqLength;
            }
        }
        srcLimit = src;
        ако(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            докато(*++srcLimit != 0) {}
        }
    } иначе {
        srcLimit = (src!=nullptr)?(src + srcLength):nullptr;
    }

    /* convert with length */
    докато(src < srcLimit) {
        ch = *src++;
        ако(!U16_IS_SURROGATE(ch)) {
            /* write or count ch below */
        } иначе ако(U16_IS_SURROGATE_LEAD(ch) && src < srcLimit && U16_IS_TRAIL(ch2 = *src)) {
            ++src;
            ch = U16_GET_SUPPLEMENTARY(ch, ch2);
        } иначе ако((ch = subchar) < 0) {
            /* unpaired surrogate */
            *pErrorCode = U_INVALID_CHAR_FOUND;
            върни nullptr;
        } иначе {
            ++numSubstitutions;
        }
        ако(pDest < destLimit) {
            *pDest++ = ch;
        } иначе {
            ++reqLength;
        }
    }

    reqLength += (int32_t)(pDest - dest);
    ако(pDestLength) {
        *pDestLength = reqLength;
    }
    ако(pNumSubstitutions != nullptr) {
        *pNumSubstitutions = numSubstitutions;
    }

    /* Terminate the buffer */
    u_terminateUChar32s(dest, destCapacity, reqLength, pErrorCode);

    върни dest;
}

U_CAPI UChar32* U_EXPORT2
u_strToUTF32(UChar32* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* pDestLength,
             const char16_t* src,
             int32_t srcLength,
             UErrorCode* pErrorCode) {
    върни u_strToUTF32WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                     int32_t destCapacity,
                     int32_t* pDestLength,
                     const char* src,
                     int32_t srcLength,
                     UChar32 subchar,
                     int32_t* pNumSubstitutions,
                     UErrorCode* pErrorCode) {
    /* args check */
    ако(U_FAILURE(*pErrorCode)) {
        върни nullptr;
    }
    ако( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        върни nullptr;
    }

    ако(pNumSubstitutions!=nullptr) {
        *pNumSubstitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *pDestLimit = dest+destCapacity;
    int32_t reqLength = 0;
    int32_t numSubstitutions=0;

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

    ако(srcLength < 0){
        /*
         * Transform a NUL-terminated string.
         * The code explicitly checks for NULs only in the lead byte position.
         * A NUL byte in the trail byte position fails the trail byte range check anyway.
         */
        int32_t i;
        UChar32 c;
        за(i = 0; (c = (uint8_t)src[i]) != 0 && (pDest < pDestLimit);) {
            // modified copy of U8_NEXT()
            ++i;
            ако(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } иначе {
                uint8_t __t1, __t2;
                ако( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } иначе ако( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } иначе {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((const uint8_t *)src, &(i), -1, c, -1);
                    ако(c<0 && (++numSubstitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        върни nullptr;
                    } иначе ако(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } иначе {
                        *(pDest++)=U16_LEAD(c);
                        ако(pDest<pDestLimit) {
                            *(pDest++)=U16_TRAIL(c);
                        } иначе {
                            reqLength++;
                            break;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        докато((c = (uint8_t)src[i]) != 0) {
            // modified copy of U8_NEXT()
            ++i;
            ако(U8_IS_SINGLE(c)) {
                ++reqLength;
            } иначе {
                uint8_t __t1, __t2;
                ако( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++reqLength;
                    i+=2;
                } иначе ако( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++reqLength;
                    ++(i);
                } иначе {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((const uint8_t *)src, &(i), -1, c, -1);
                    ако(c<0 && (++numSubstitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        върни nullptr;
                    }
                    reqLength += U16_LENGTH(c);
                }
            }
        }
    } иначе /* srcLength >= 0 */ {
        /* Faster loop without ongoing checking for srcLength and pDestLimit. */
        int32_t i = 0;
        UChar32 c;
        за(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            int32_t count = (int32_t)(pDestLimit - pDest);
            int32_t count2 = (srcLength - i) / 3;
            ако(count > count2) {
                count = count2; /* min(remaining dest, remaining src/3) */
            }
            ако(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                break;
            }

            do {
                // modified copy of U8_NEXT()
                c = (uint8_t)src[i++];
                ако(U8_IS_SINGLE(c)) {
                    *pDest++=(char16_t)c;
                } иначе {
                    uint8_t __t1, __t2;
                    ако( /* handle U+0800..U+FFFF inline */
                            (0xe0<=(c) && (c)<0xf0) &&
                            ((i)+1)<srcLength &&
                            U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                            (__t2=src[(i)+1]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                        i+=2;
                    } иначе ако( /* handle U+0080..U+07FF inline */
                            ((c)<0xe0 && (c)>=0xc2) &&
                            ((i)!=srcLength) &&
                            (__t1=src[i]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0x1f)<<6)|__t1;
                        ++(i);
                    } иначе {
                        ако(c >= 0xf0 || subchar > 0xffff) {
                            // We may read up to four bytes and write up to two UChars,
                            // which we didn't account for with computing count,
                            // so we adjust it here.
                            ако(--count == 0) {
                                --i;  // back out byte c
                                break;
                            }
                        }

                        /* function call for "complicated" and error cases */
                        (c)=utf8_nextCharSafeBody((const uint8_t *)src, &(i), srcLength, c, -1);
                        ако(c<0 && (++numSubstitutions, c = subchar) < 0) {
                            *pErrorCode = U_INVALID_CHAR_FOUND;
                            върни nullptr;
                        } иначе ако(c<=0xFFFF) {
                            *(pDest++)=(char16_t)c;
                        } иначе {
                            *(pDest++)=U16_LEAD(c);
                            *(pDest++)=U16_TRAIL(c);
                        }
                    }
                }
            } докато(--count > 0);
        }

        докато(i < srcLength && (pDest < pDestLimit)) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            ако(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } иначе {
                uint8_t __t1, __t2;
                ако( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<srcLength &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } иначе ако( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=srcLength) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } иначе {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((const uint8_t *)src, &(i), srcLength, c, -1);
                    ако(c<0 && (++numSubstitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        върни nullptr;
                    } иначе ако(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } иначе {
                        *(pDest++)=U16_LEAD(c);
                        ако(pDest<pDestLimit) {
                            *(pDest++)=U16_TRAIL(c);
                        } иначе {
                            reqLength++;
                            break;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        докато(i < srcLength) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            ако(U8_IS_SINGLE(c)) {
                ++reqLength;
            } иначе {
                uint8_t __t1, __t2;
                ако( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<srcLength &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++reqLength;
                    i+=2;
                } иначе ако( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=srcLength) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++reqLength;
                    ++(i);
                } иначе {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((const uint8_t *)src, &(i), srcLength, c, -1);
                    ако(c<0 && (++numSubstitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        върни nullptr;
                    }
                    reqLength += U16_LENGTH(c);
                }
            }
        }
    }

    reqLength+=(int32_t)(pDest - dest);

    ако(pNumSubstitutions!=nullptr) {
        *pNumSubstitutions=numSubstitutions;
    }

    ако(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,reqLength,pErrorCode);

    върни dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8(char16_t* dest U_LIFETIME_BOUND,
              int32_t destCapacity,
              int32_t* pDestLength,
              const char* src,
              int32_t srcLength,
              UErrorCode* pErrorCode) {
    върни u_strFromUTF8WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8Lenient(char16_t* dest U_LIFETIME_BOUND,
                     int32_t destCapacity,
                     int32_t* pDestLength,
                     const char* src,
                     int32_t srcLength,
                     UErrorCode* pErrorCode) {
    char16_t *pDest = dest;
    UChar32 ch;
    int32_t reqLength = 0;
    uint8_t* pSrc = (uint8_t*) src;

    /* args check */
    ако(U_FAILURE(*pErrorCode)){
        върни nullptr;
    }
        
    ако( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        върни nullptr;
    }

    ако(srcLength < 0) {
        /* Transform a NUL-terminated string. */
        char16_t *pDestLimit = (dest!=nullptr)?(dest+destCapacity):nullptr;
        uint8_t t1, t2, t3; /* trail bytes */

        докато(((ch = *pSrc) != 0) && (pDest < pDestLimit)) {
            ако(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ++pSrc;
                continue;
            } иначе ако(ch < 0xe0) { /* U+0080..U+07FF */
                ако((t1 = pSrc[1]) != 0) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + t1 - 0x3080);
                    pSrc += 2;
                    continue;
                }
            } иначе ако(ch < 0xf0) { /* U+0800..U+FFFF */
                ако((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 12) + (t1 << 6) + t2 - 0x2080);
                    pSrc += 3;
                    continue;
                }
            } иначе /* f0..f4 */ { /* U+10000..U+10FFFF */
                ако((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0 && (t3 = pSrc[3]) != 0) {
                    pSrc += 4;
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (t1 << 12) + (t2 << 6) + t3 - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    ако(pDest < pDestLimit) {
                        *(pDest++) = U16_TRAIL(ch);
                    } иначе {
                        reqLength = 1;
                        break;
                    }
                    continue;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            докато(*++pSrc != 0) {}
            break;
        }

        /* Pre-flight the rest of the string. */
        докато((ch = *pSrc) != 0) {
            ако(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                ++reqLength;
                ++pSrc;
                continue;
            } иначе ако(ch < 0xe0) { /* U+0080..U+07FF */
                ако(pSrc[1] != 0) {
                    ++reqLength;
                    pSrc += 2;
                    continue;
                }
            } иначе ако(ch < 0xf0) { /* U+0800..U+FFFF */
                ако(pSrc[1] != 0 && pSrc[2] != 0) {
                    ++reqLength;
                    pSrc += 3;
                    continue;
                }
            } иначе /* f0..f4 */ { /* U+10000..U+10FFFF */
                ако(pSrc[1] != 0 && pSrc[2] != 0 && pSrc[3] != 0) {
                    reqLength += 2;
                    pSrc += 4;
                    continue;
                }
            }

            /* truncated character at the end */
            ++reqLength;
            break;
        }
    } иначе /* srcLength >= 0 */ {
      const uint8_t *pSrcLimit = (pSrc!=nullptr)?(pSrc + srcLength):nullptr;

        /*
         * This function requires that if srcLength is given, then it must be
         * destCapatity >= srcLength so that we need not check for
         * destination buffer overflow in the loop.
         */
        ако(destCapacity < srcLength) {
            ако(pDestLength != nullptr) {
                *pDestLength = srcLength; /* this likely overestimates the true destLength! */
            }
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            върни nullptr;
        }

        ако((pSrcLimit - pSrc) >= 4) {
            pSrcLimit -= 3; /* temporarily reduce pSrcLimit */

            /* in this loop, we can always access at least 4 bytes, up to pSrc+3 */
            do {
                ch = *pSrc++;
                ако(ch < 0xc0) {
                    /*
                     * ASCII, or a trail byte in lead position which is treated like
                     * a single-byte sequence for better character boundary
                     * resynchronization after illegal sequences.
                     */
                    *pDest++=(char16_t)ch;
                } иначе ако(ch < 0xe0) { /* U+0080..U+07FF */
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                } иначе ако(ch < 0xf0) { /* U+0800..U+FFFF */
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                } иначе /* f0..f4 */ { /* U+10000..U+10FFFF */
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                }
            } докато(pSrc < pSrcLimit);

            pSrcLimit += 3; /* restore original pSrcLimit */
        }

        докато(pSrc < pSrcLimit) {
            ch = *pSrc++;
            ако(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                continue;
            } иначе ако(ch < 0xe0) { /* U+0080..U+07FF */
                ако(pSrc < pSrcLimit) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                    continue;
                }
            } иначе ако(ch < 0xf0) { /* U+0800..U+FFFF */
                ако((pSrcLimit - pSrc) >= 2) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                    pSrc += 3;
                    continue;
                }
            } иначе /* f0..f4 */ { /* U+10000..U+10FFFF */
                ако((pSrcLimit - pSrc) >= 3) {
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                    pSrc += 4;
                    continue;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            break;
        }
    }

    reqLength+=(int32_t)(pDest - dest);

    ако(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,reqLength,pErrorCode);

    върни dest;
}

static inline uint8_t *
_appendUTF8(uint8_t *pDest, UChar32 c) {
    /* it is 0<=c<=0x10ffff and not a surrogate if called by a validating function */
    ако((c)<=0x7f) {
        *pDest++ = static_cast<uint8_t>(c);
    } иначе ако(c<=0x7ff) {
        *pDest++ = static_cast<uint8_t>((c >> 6) | 0xc0);
        *pDest++ = static_cast<uint8_t>((c & 0x3f) | 0x80);
    } иначе ако(c<=0xffff) {
        *pDest++ = static_cast<uint8_t>((c >> 12) | 0xe0);
        *pDest++ = static_cast<uint8_t>(((c >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    } иначе /* if((uint32_t)(c)<=0x10ffff) */ {
        *pDest++ = static_cast<uint8_t>(((c) >> 18) | 0xf0);
        *pDest++ = static_cast<uint8_t>((((c) >> 12) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>((((c) >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    }
    върни pDest;
}

U_CAPI char* U_EXPORT2
u_strToUTF8WithSub(char* dest U_LIFETIME_BOUND,
                   int32_t destCapacity,
                   int32_t* pDestLength,
                   const char16_t* pSrc,
                   int32_t srcLength,
                   UChar32 subchar,
                   int32_t* pNumSubstitutions,
                   UErrorCode* pErrorCode) {
    int32_t reqLength=0;
    uint32_t ch=0,ch2=0;
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *pDestLimit = (pDest!=nullptr)?(pDest + destCapacity):nullptr;
    int32_t numSubstitutions;

    /* args check */
    ако(U_FAILURE(*pErrorCode)){
        върни nullptr;
    }
        
    ако( (pSrc==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        върни nullptr;
    }

    ако(pNumSubstitutions!=nullptr) {
        *pNumSubstitutions=0;
    }
    numSubstitutions=0;

    ако(srcLength==-1) {
        докато((ch=*pSrc)!=0) {
            ++pSrc;
            ако(ch <= 0x7f) {
                ако(pDest<pDestLimit) {
                    *pDest++ = (uint8_t)ch;
                } иначе {
                    reqLength = 1;
                    break;
                }
            } иначе ако(ch <= 0x7ff) {
                ако((pDestLimit - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } иначе {
                    reqLength = 2;
                    break;
                }
            } иначе ако(ch <= 0xd7ff || ch >= 0xe000) {
                ако((pDestLimit - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } иначе {
                    reqLength = 3;
                    break;
                }
            } иначе /* ch is a surrogate */ {
                int32_t length;

                /*need not check for NUL because NUL fails U16_IS_TRAIL() anyway*/
                ако(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } иначе ако(subchar>=0) {
                    ch=subchar;
                    ++numSubstitutions;
                } иначе {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    върни nullptr;
                }

                length = U8_LENGTH(ch);
                ако((pDestLimit - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } иначе {
                    reqLength = length;
                    break;
                }
            }
        }
        докато((ch=*pSrc++)!=0) {
            ако(ch<=0x7f) {
                ++reqLength;
            } иначе ако(ch<=0x7ff) {
                reqLength+=2;
            } иначе ако(!U16_IS_SURROGATE(ch)) {
                reqLength+=3;
            } иначе ако(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                reqLength+=4;
            } иначе ако(subchar>=0) {
                reqLength+=U8_LENGTH(subchar);
                ++numSubstitutions;
            } иначе {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                върни nullptr;
            }
        }
    } иначе {
        const char16_t *pSrcLimit = (pSrc!=nullptr)?(pSrc+srcLength):nullptr;
        int32_t count;

        /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
        за(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            count = (int32_t)((pDestLimit - pDest) / 3);
            srcLength = (int32_t)(pSrcLimit - pSrc);
            ако(count > srcLength) {
                count = srcLength; /* min(remaining dest/3, remaining src) */
            }
            ако(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                break;
            }
            do {
                ch=*pSrc++;
                ако(ch <= 0x7f) {
                    *pDest++ = (uint8_t)ch;
                } иначе ако(ch <= 0x7ff) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } иначе ако(ch <= 0xd7ff || ch >= 0xe000) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } иначе /* ch is a surrogate */ {
                    /*
                     * We will read two UChars and probably output four bytes,
                     * which we didn't account for with computing count,
                     * so we adjust it here.
                     */
                    ако(--count == 0) {
                        --pSrc; /* undo ch=*pSrc++ for the lead surrogate */
                        break;  /* recompute count */
                    }

                    ако(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                        ++pSrc;
                        ch=U16_GET_SUPPLEMENTARY(ch, ch2);

                        /* writing 4 bytes per 2 UChars is ok */
                        *pDest++=(uint8_t)((ch>>18)|0xf0);
                        *pDest++=(uint8_t)(((ch>>12)&0x3f)|0x80);
                        *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                        *pDest++=(uint8_t)((ch&0x3f)|0x80);
                    } иначе  {
                        /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                        ако(subchar>=0) {
                            ch=subchar;
                            ++numSubstitutions;
                        } иначе {
                            *pErrorCode = U_INVALID_CHAR_FOUND;
                            върни nullptr;
                        }

                        /* convert and append*/
                        pDest=_appendUTF8(pDest, ch);
                    }
                }
            } докато(--count > 0);
        }

        докато(pSrc<pSrcLimit) {
            ch=*pSrc++;
            ако(ch <= 0x7f) {
                ако(pDest<pDestLimit) {
                    *pDest++ = (uint8_t)ch;
                } иначе {
                    reqLength = 1;
                    break;
                }
            } иначе ако(ch <= 0x7ff) {
                ако((pDestLimit - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } иначе {
                    reqLength = 2;
                    break;
                }
            } иначе ако(ch <= 0xd7ff || ch >= 0xe000) {
                ако((pDestLimit - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } иначе {
                    reqLength = 3;
                    break;
                }
            } иначе /* ch is a surrogate */ {
                int32_t length;

                ако(U16_IS_SURROGATE_LEAD(ch) && pSrc<pSrcLimit && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } иначе ако(subchar>=0) {
                    ch=subchar;
                    ++numSubstitutions;
                } иначе {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    върни nullptr;
                }

                length = U8_LENGTH(ch);
                ако((pDestLimit - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } иначе {
                    reqLength = length;
                    break;
                }
            }
        }
        докато(pSrc<pSrcLimit) {
            ch=*pSrc++;
            ако(ch<=0x7f) {
                ++reqLength;
            } иначе ако(ch<=0x7ff) {
                reqLength+=2;
            } иначе ако(!U16_IS_SURROGATE(ch)) {
                reqLength+=3;
            } иначе ако(U16_IS_SURROGATE_LEAD(ch) && pSrc<pSrcLimit && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                reqLength+=4;
            } иначе ако(subchar>=0) {
                reqLength+=U8_LENGTH(subchar);
                ++numSubstitutions;
            } иначе {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                върни nullptr;
            }
        }
    }

    reqLength+=(int32_t)(pDest - (uint8_t *)dest);

    ако(pNumSubstitutions!=nullptr) {
        *pNumSubstitutions=numSubstitutions;
    }

    ако(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
    върни dest;
}

U_CAPI char* U_EXPORT2
u_strToUTF8(char* dest U_LIFETIME_BOUND,
            int32_t destCapacity,
            int32_t* pDestLength,
            const char16_t* pSrc,
            int32_t srcLength,
            UErrorCode* pErrorCode) {
    върни u_strToUTF8WithSub(
            dest, destCapacity, pDestLength,
            pSrc, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromJavaModifiedUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                                 int32_t destCapacity,
                                 int32_t* pDestLength,
                                 const char* src,
                                 int32_t srcLength,
                                 UChar32 subchar,
                                 int32_t* pNumSubstitutions,
                                 UErrorCode* pErrorCode) {
    /* args check */
    ако(U_FAILURE(*pErrorCode)) {
        върни nullptr;
    }
    ако( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (dest==nullptr && destCapacity!=0) || destCapacity<0 ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        върни nullptr;
    }

    ако(pNumSubstitutions!=nullptr) {
        *pNumSubstitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *pDestLimit = dest+destCapacity;
    int32_t reqLength = 0;
    int32_t numSubstitutions=0;

    ако(srcLength < 0) {
        /*
         * Transform a NUL-terminated ASCII string.
         * Handle non-ASCII strings with slower code.
         */
        UChar32 c;
        докато(((c = (uint8_t)*src) != 0) && c <= 0x7f && (pDest < pDestLimit)) {
            *pDest++=(char16_t)c;
            ++src;
        }
        ако(c == 0) {
            reqLength=(int32_t)(pDest - dest);
            ако(pDestLength) {
                *pDestLength = reqLength;
            }

            /* Terminate the buffer */
            u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
            върни dest;
        }
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    /* Faster loop without ongoing checking for srcLength and pDestLimit. */
    UChar32 ch;
    uint8_t t1, t2;
    int32_t i = 0;
    за(;;) {
        int32_t count = (int32_t)(pDestLimit - pDest);
        int32_t count2 = srcLength - i;
        ако(count >= count2 && srcLength > 0 && U8_IS_SINGLE(*src)) {
            /* fast ASCII loop */
            int32_t start = i;
            uint8_t b;
            докато(i < srcLength && U8_IS_SINGLE(b = src[i])) {
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
        ако(subchar > 0xFFFF) {
            break;
        }
        count2 /= 3;
        ако(count > count2) {
            count = count2; /* min(remaining dest, remaining src/3) */
        }
        ако(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            break;
        }
        do {
            ch = (uint8_t)src[i++];
            ако(U8_IS_SINGLE(ch)) {
                *pDest++=(char16_t)ch;
            } иначе {
                ако(ch >= 0xe0) {
                    ако( /* handle U+0000..U+FFFF inline */
                        ch <= 0xef &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                        (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                    ) {
                        /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                        *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                        i += 2;
                        continue;
                    }
                } иначе {
                    ако( /* handle U+0000..U+07FF inline */
                        ch >= 0xc0 &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                    ) {
                        *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                        ++i;
                        continue;
                    }
                }

                ако(subchar < 0) {
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    върни nullptr;
                } иначе ако(subchar > 0xffff && --count == 0) {
                    /*
                     * We need to write two UChars, adjusted count for that,
                     * and ran out of space.
                     */
                    --i;  // back out byte ch
                    break;
                } иначе {
                    /* function call for error cases */
                    utf8_nextCharSafeBody((const uint8_t *)src, &(i), srcLength, ch, -1);
                    ++numSubstitutions;
                    *(pDest++)=(char16_t)subchar;
                }
            }
        } докато(--count > 0);
    }

    докато(i < srcLength && (pDest < pDestLimit)) {
        ch = (uint8_t)src[i++];
        ако(U8_IS_SINGLE(ch)){
            *pDest++=(char16_t)ch;
        } иначе {
            ако(ch >= 0xe0) {
                ако( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < srcLength &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                    (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                ) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                    i += 2;
                    continue;
                }
            } иначе {
                ако( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < srcLength &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                ) {
                    *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                    ++i;
                    continue;
                }
            }

            ако(subchar < 0) {
                *pErrorCode = U_INVALID_CHAR_FOUND;
                върни nullptr;
            } иначе {
                /* function call for error cases */
                utf8_nextCharSafeBody((const uint8_t *)src, &(i), srcLength, ch, -1);
                ++numSubstitutions;
                ако(subchar<=0xFFFF) {
                    *(pDest++)=(char16_t)subchar;
                } иначе {
                    *(pDest++)=U16_LEAD(subchar);
                    ако(pDest<pDestLimit) {
                        *(pDest++)=U16_TRAIL(subchar);
                    } иначе {
                        reqLength++;
                        break;
                    }
                }
            }
        }
    }

    /* Pre-flight the rest of the string. */
    докато(i < srcLength) {
        ch = (uint8_t)src[i++];
        ако(U8_IS_SINGLE(ch)) {
            reqLength++;
        } иначе {
            ако(ch >= 0xe0) {
                ако( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < srcLength &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f &&
                    (uint8_t)(src[i+1] - 0x80) <= 0x3f
                ) {
                    reqLength++;
                    i += 2;
                    continue;
                }
            } иначе {
                ако( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < srcLength &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f
                ) {
                    reqLength++;
                    ++i;
                    continue;
                }
            }

            ако(subchar < 0) {
                *pErrorCode = U_INVALID_CHAR_FOUND;
                върни nullptr;
            } иначе {
                /* function call for error cases */
                utf8_nextCharSafeBody((const uint8_t *)src, &(i), srcLength, ch, -1);
                ++numSubstitutions;
                reqLength+=U16_LENGTH(ch);
            }
        }
    }

    ако(pNumSubstitutions!=nullptr) {
        *pNumSubstitutions=numSubstitutions;
    }

    reqLength+=(int32_t)(pDest - dest);
    ако(pDestLength) {
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
    върни dest;
}

U_CAPI char* U_EXPORT2
u_strToJavaModifiedUTF8(char* dest U_LIFETIME_BOUND,
                        int32_t destCapacity,
                        int32_t* pDestLength,
                        const char16_t* src,
                        int32_t srcLength,
                        UErrorCode* pErrorCode) {
    int32_t reqLength=0;
    uint32_t ch=0;
    const char16_t *pSrcLimit;
    int32_t count;

    /* args check */
    ако(U_FAILURE(*pErrorCode)){
        върни nullptr;
    }
    ако( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (dest==nullptr && destCapacity!=0) || destCapacity<0
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        върни nullptr;
    }
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *pDestLimit = pDest + destCapacity;

    ако(srcLength==-1) {
        /* Convert NUL-terminated ASCII, then find the string length. */
        докато((ch=*src)<=0x7f && ch != 0 && pDest<pDestLimit) {
            *pDest++ = (uint8_t)ch;
            ++src;
        }
        ако(ch == 0) {
            reqLength=(int32_t)(pDest - (uint8_t *)dest);
            ако(pDestLength) {
                *pDestLength = reqLength;
            }

            /* Terminate the buffer */
            u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
            върни dest;
        }
        srcLength = u_strlen(src);
    }

    /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
    pSrcLimit = (src!=nullptr)?(src+srcLength):nullptr;
    за(;;) {
        count = (int32_t)(pDestLimit - pDest);
        srcLength = (int32_t)(pSrcLimit - src);
        ако(count >= srcLength && srcLength > 0 && *src <= 0x7f) {
            /* fast ASCII loop */
            const char16_t *prevSrc = src;
            int32_t delta;
            докато(src < pSrcLimit && (ch = *src) <= 0x7f && ch != 0) {
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
        ако(count > srcLength) {
            count = srcLength; /* min(remaining dest/3, remaining src) */
        }
        ако(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            break;
        }
        do {
            ch=*src++;
            ако(ch <= 0x7f && ch != 0) {
                *pDest++ = (uint8_t)ch;
            } иначе ако(ch <= 0x7ff) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } иначе {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            }
        } докато(--count > 0);
    }

    докато(src<pSrcLimit) {
        ch=*src++;
        ако(ch <= 0x7f && ch != 0) {
            ако(pDest<pDestLimit) {
                *pDest++ = (uint8_t)ch;
            } иначе {
                reqLength = 1;
                break;
            }
        } иначе ако(ch <= 0x7ff) {
            ако((pDestLimit - pDest) >= 2) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } иначе {
                reqLength = 2;
                break;
            }
        } иначе {
            ако((pDestLimit - pDest) >= 3) {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } иначе {
                reqLength = 3;
                break;
            }
        }
    }
    докато(src<pSrcLimit) {
        ch=*src++;
        ако(ch <= 0x7f && ch != 0) {
            ++reqLength;
        } иначе ако(ch<=0x7ff) {
            reqLength+=2;
        } иначе {
            reqLength+=3;
        }
    }

    reqLength+=(int32_t)(pDest - (uint8_t *)dest);
    ако(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
    върни dest;
}
