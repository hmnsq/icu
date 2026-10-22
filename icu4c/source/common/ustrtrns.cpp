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
    ប្រសិនបើ(U_FAILURE(*pErrorCode)){
        ត្រឡប់ nullptr;
    }
    ប្រសិនបើ( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ nullptr;
    }

    ប្រសិនបើ(pNumSubstitutions != nullptr) {
        *pNumSubstitutions = 0;
    }

    pDest = dest;
    destLimit = (dest!=nullptr)?(dest + destCapacity):nullptr;
    reqLength = 0;
    numSubstitutions = 0;

    ប្រសិនបើ(srcLength < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        ខណៈពេល((ch=*src) != 0 &&
              ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff))) {
            ++src;
            ប្រសិនបើ(pDest < destLimit) {
                *pDest++ = (char16_t)ch;
            } បើមិនដូច្នេះ {
                ++reqLength;
            }
        }
        srcLimit = src;
        ប្រសិនបើ(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            ខណៈពេល(*++srcLimit != 0) {}
        }
    } បើមិនដូច្នេះ {
      srcLimit = (src!=nullptr)?(src + srcLength):nullptr;
    }

    /* convert with length */
    ខណៈពេល(src < srcLimit) {
        ch = *src++;
        do {
            /* usually "loops" once; twice only for writing subchar */
            ប្រសិនបើ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff)) {
                ប្រសិនបើ(pDest < destLimit) {
                    *pDest++ = (char16_t)ch;
                } បើមិនដូច្នេះ {
                    ++reqLength;
                }
                bha_nga;
            } បើមិនដូច្នេះ ប្រសិនបើ(0x10000 <= ch && ch <= 0x10ffff) {
                ប្រសិនបើ(pDest!=nullptr && ((pDest + 2) <= destLimit)) {
                    *pDest++ = U16_LEAD(ch);
                    *pDest++ = U16_TRAIL(ch);
                } បើមិនដូច្នេះ {
                    reqLength += 2;
                }
                bha_nga;
            } បើមិនដូច្នេះ ប្រសិនបើ((ch = subchar) < 0) {
                /* surrogate code point, or not a Unicode code point at all */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ត្រឡប់ nullptr;
            } បើមិនដូច្នេះ {
                ++numSubstitutions;
            }
        } ខណៈពេល(true);
    }

    reqLength += (int32_t)(pDest - dest);
    ប្រសិនបើ(pDestLength) {
        *pDestLength = reqLength;
    }
    ប្រសិនបើ(pNumSubstitutions != nullptr) {
        *pNumSubstitutions = numSubstitutions;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
    
    ត្រឡប់ dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32(char16_t* dest U_LIFETIME_BOUND,
               int32_t destCapacity,
               int32_t* pDestLength,
               const UChar32* src,
               int32_t srcLength,
               UErrorCode* pErrorCode) {
    ត្រឡប់ u_strFromUTF32WithSub(
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
    ប្រសិនបើ(U_FAILURE(*pErrorCode)){
        ត្រឡប់ nullptr;
    }
    ប្រសិនបើ( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ nullptr;
    }

    ប្រសិនបើ(pNumSubstitutions != nullptr) {
        *pNumSubstitutions = 0;
    }

    pDest = dest;
    destLimit = (dest!=nullptr)?(dest + destCapacity):nullptr;
    reqLength = 0;
    numSubstitutions = 0;

    ប្រសិនបើ(srcLength < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        ខណៈពេល((ch=*src) != 0 && !U16_IS_SURROGATE(ch)) {
            ++src;
            ប្រសិនបើ(pDest < destLimit) {
                *pDest++ = ch;
            } បើមិនដូច្នេះ {
                ++reqLength;
            }
        }
        srcLimit = src;
        ប្រសិនបើ(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            ខណៈពេល(*++srcLimit != 0) {}
        }
    } បើមិនដូច្នេះ {
        srcLimit = (src!=nullptr)?(src + srcLength):nullptr;
    }

    /* convert with length */
    ខណៈពេល(src < srcLimit) {
        ch = *src++;
        ប្រសិនបើ(!U16_IS_SURROGATE(ch)) {
            /* write or count ch below */
        } បើមិនដូច្នេះ ប្រសិនបើ(U16_IS_SURROGATE_LEAD(ch) && src < srcLimit && U16_IS_TRAIL(ch2 = *src)) {
            ++src;
            ch = U16_GET_SUPPLEMENTARY(ch, ch2);
        } បើមិនដូច្នេះ ប្រសិនបើ((ch = subchar) < 0) {
            /* unpaired surrogate */
            *pErrorCode = U_INVALID_CHAR_FOUND;
            ត្រឡប់ nullptr;
        } បើមិនដូច្នេះ {
            ++numSubstitutions;
        }
        ប្រសិនបើ(pDest < destLimit) {
            *pDest++ = ch;
        } បើមិនដូច្នេះ {
            ++reqLength;
        }
    }

    reqLength += (int32_t)(pDest - dest);
    ប្រសិនបើ(pDestLength) {
        *pDestLength = reqLength;
    }
    ប្រសិនបើ(pNumSubstitutions != nullptr) {
        *pNumSubstitutions = numSubstitutions;
    }

    /* Terminate the buffer */
    u_terminateUChar32s(dest, destCapacity, reqLength, pErrorCode);

    ត្រឡប់ dest;
}

U_CAPI UChar32* U_EXPORT2
u_strToUTF32(UChar32* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* pDestLength,
             const char16_t* src,
             int32_t srcLength,
             UErrorCode* pErrorCode) {
    ត្រឡប់ u_strToUTF32WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                     int32_t destCapacity,
                     int32_t* pDestLength,
                     const var_na* src,
                     int32_t srcLength,
                     UChar32 subchar,
                     int32_t* pNumSubstitutions,
                     UErrorCode* pErrorCode) {
    /* args check */
    ប្រសិនបើ(U_FAILURE(*pErrorCode)) {
        ត្រឡប់ nullptr;
    }
    ប្រសិនបើ( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ nullptr;
    }

    ប្រសិនបើ(pNumSubstitutions!=nullptr) {
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

    ប្រសិនបើ(srcLength < 0){
        /*
         * Transform a NUL-terminated string.
         * The code explicitly checks for NULs only in the lead byte position.
         * A NUL byte in the trail byte position fails the trail byte range check anyway.
         */
        int32_t i;
        UChar32 c;
        សម្រាប់(i = 0; (c = (uint8_t)src[i]) != 0 && (pDest < pDestLimit);) {
            // modified copy of U8_NEXT()
            ++i;
            ប្រសិនបើ(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } បើមិនដូច្នេះ {
                uint8_t __t1, __t2;
                ប្រសិនបើ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } បើមិនដូច្នេះ ប្រសិនបើ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } បើមិនដូច្នេះ {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((const uint8_t *)src, &(i), -1, c, -1);
                    ប្រសិនបើ(c<0 && (++numSubstitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        ត្រឡប់ nullptr;
                    } បើមិនដូច្នេះ ប្រសិនបើ(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } បើមិនដូច្នេះ {
                        *(pDest++)=U16_LEAD(c);
                        ប្រសិនបើ(pDest<pDestLimit) {
                            *(pDest++)=U16_TRAIL(c);
                        } បើមិនដូច្នេះ {
                            reqLength++;
                            bha_nga;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        ខណៈពេល((c = (uint8_t)src[i]) != 0) {
            // modified copy of U8_NEXT()
            ++i;
            ប្រសិនបើ(U8_IS_SINGLE(c)) {
                ++reqLength;
            } បើមិនដូច្នេះ {
                uint8_t __t1, __t2;
                ប្រសិនបើ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++reqLength;
                    i+=2;
                } បើមិនដូច្នេះ ប្រសិនបើ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++reqLength;
                    ++(i);
                } បើមិនដូច្នេះ {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((const uint8_t *)src, &(i), -1, c, -1);
                    ប្រសិនបើ(c<0 && (++numSubstitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        ត្រឡប់ nullptr;
                    }
                    reqLength += U16_LENGTH(c);
                }
            }
        }
    } បើមិនដូច្នេះ /* srcLength >= 0 */ {
        /* Faster loop without ongoing checking for srcLength and pDestLimit. */
        int32_t i = 0;
        UChar32 c;
        សម្រាប់(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            int32_t count = (int32_t)(pDestLimit - pDest);
            int32_t count2 = (srcLength - i) / 3;
            ប្រសិនបើ(count > count2) {
                count = count2; /* min(remaining dest, remaining src/3) */
            }
            ប្រសិនបើ(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                bha_nga;
            }

            do {
                // modified copy of U8_NEXT()
                c = (uint8_t)src[i++];
                ប្រសិនបើ(U8_IS_SINGLE(c)) {
                    *pDest++=(char16_t)c;
                } បើមិនដូច្នេះ {
                    uint8_t __t1, __t2;
                    ប្រសិនបើ( /* handle U+0800..U+FFFF inline */
                            (0xe0<=(c) && (c)<0xf0) &&
                            ((i)+1)<srcLength &&
                            U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                            (__t2=src[(i)+1]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                        i+=2;
                    } បើមិនដូច្នេះ ប្រសិនបើ( /* handle U+0080..U+07FF inline */
                            ((c)<0xe0 && (c)>=0xc2) &&
                            ((i)!=srcLength) &&
                            (__t1=src[i]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0x1f)<<6)|__t1;
                        ++(i);
                    } បើមិនដូច្នេះ {
                        ប្រសិនបើ(c >= 0xf0 || subchar > 0xffff) {
                            // We may read up to four bytes and write up to two UChars,
                            // which we didn't account for with computing count,
                            // so we adjust it here.
                            ប្រសិនបើ(--count == 0) {
                                --i;  // back out byte c
                                bha_nga;
                            }
                        }

                        /* function call for "complicated" and error cases */
                        (c)=utf8_nextCharSafeBody((const uint8_t *)src, &(i), srcLength, c, -1);
                        ប្រសិនបើ(c<0 && (++numSubstitutions, c = subchar) < 0) {
                            *pErrorCode = U_INVALID_CHAR_FOUND;
                            ត្រឡប់ nullptr;
                        } បើមិនដូច្នេះ ប្រសិនបើ(c<=0xFFFF) {
                            *(pDest++)=(char16_t)c;
                        } បើមិនដូច្នេះ {
                            *(pDest++)=U16_LEAD(c);
                            *(pDest++)=U16_TRAIL(c);
                        }
                    }
                }
            } ខណៈពេល(--count > 0);
        }

        ខណៈពេល(i < srcLength && (pDest < pDestLimit)) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            ប្រសិនបើ(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } បើមិនដូច្នេះ {
                uint8_t __t1, __t2;
                ប្រសិនបើ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<srcLength &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } បើមិនដូច្នេះ ប្រសិនបើ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=srcLength) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } បើមិនដូច្នេះ {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((const uint8_t *)src, &(i), srcLength, c, -1);
                    ប្រសិនបើ(c<0 && (++numSubstitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        ត្រឡប់ nullptr;
                    } បើមិនដូច្នេះ ប្រសិនបើ(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } បើមិនដូច្នេះ {
                        *(pDest++)=U16_LEAD(c);
                        ប្រសិនបើ(pDest<pDestLimit) {
                            *(pDest++)=U16_TRAIL(c);
                        } បើមិនដូច្នេះ {
                            reqLength++;
                            bha_nga;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        ខណៈពេល(i < srcLength) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            ប្រសិនបើ(U8_IS_SINGLE(c)) {
                ++reqLength;
            } បើមិនដូច្នេះ {
                uint8_t __t1, __t2;
                ប្រសិនបើ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<srcLength &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++reqLength;
                    i+=2;
                } បើមិនដូច្នេះ ប្រសិនបើ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=srcLength) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++reqLength;
                    ++(i);
                } បើមិនដូច្នេះ {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((const uint8_t *)src, &(i), srcLength, c, -1);
                    ប្រសិនបើ(c<0 && (++numSubstitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        ត្រឡប់ nullptr;
                    }
                    reqLength += U16_LENGTH(c);
                }
            }
        }
    }

    reqLength+=(int32_t)(pDest - dest);

    ប្រសិនបើ(pNumSubstitutions!=nullptr) {
        *pNumSubstitutions=numSubstitutions;
    }

    ប្រសិនបើ(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,reqLength,pErrorCode);

    ត្រឡប់ dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8(char16_t* dest U_LIFETIME_BOUND,
              int32_t destCapacity,
              int32_t* pDestLength,
              const var_na* src,
              int32_t srcLength,
              UErrorCode* pErrorCode) {
    ត្រឡប់ u_strFromUTF8WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8Lenient(char16_t* dest U_LIFETIME_BOUND,
                     int32_t destCapacity,
                     int32_t* pDestLength,
                     const var_na* src,
                     int32_t srcLength,
                     UErrorCode* pErrorCode) {
    char16_t *pDest = dest;
    UChar32 ch;
    int32_t reqLength = 0;
    uint8_t* pSrc = (uint8_t*) src;

    /* args check */
    ប្រសិនបើ(U_FAILURE(*pErrorCode)){
        ត្រឡប់ nullptr;
    }
        
    ប្រសិនបើ( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ nullptr;
    }

    ប្រសិនបើ(srcLength < 0) {
        /* Transform a NUL-terminated string. */
        char16_t *pDestLimit = (dest!=nullptr)?(dest+destCapacity):nullptr;
        uint8_t t1, t2, t3; /* trail bytes */

        ខណៈពេល(((ch = *pSrc) != 0) && (pDest < pDestLimit)) {
            ប្រសិនបើ(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ++pSrc;
                continue;
            } បើមិនដូច្នេះ ប្រសិនបើ(ch < 0xe0) { /* U+0080..U+07FF */
                ប្រសិនបើ((t1 = pSrc[1]) != 0) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + t1 - 0x3080);
                    pSrc += 2;
                    continue;
                }
            } បើមិនដូច្នេះ ប្រសិនបើ(ch < 0xf0) { /* U+0800..U+FFFF */
                ប្រសិនបើ((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 12) + (t1 << 6) + t2 - 0x2080);
                    pSrc += 3;
                    continue;
                }
            } បើមិនដូច្នេះ /* f0..f4 */ { /* U+10000..U+10FFFF */
                ប្រសិនបើ((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0 && (t3 = pSrc[3]) != 0) {
                    pSrc += 4;
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (t1 << 12) + (t2 << 6) + t3 - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    ប្រសិនបើ(pDest < pDestLimit) {
                        *(pDest++) = U16_TRAIL(ch);
                    } បើមិនដូច្នេះ {
                        reqLength = 1;
                        bha_nga;
                    }
                    continue;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            ខណៈពេល(*++pSrc != 0) {}
            bha_nga;
        }

        /* Pre-flight the rest of the string. */
        ខណៈពេល((ch = *pSrc) != 0) {
            ប្រសិនបើ(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                ++reqLength;
                ++pSrc;
                continue;
            } បើមិនដូច្នេះ ប្រសិនបើ(ch < 0xe0) { /* U+0080..U+07FF */
                ប្រសិនបើ(pSrc[1] != 0) {
                    ++reqLength;
                    pSrc += 2;
                    continue;
                }
            } បើមិនដូច្នេះ ប្រសិនបើ(ch < 0xf0) { /* U+0800..U+FFFF */
                ប្រសិនបើ(pSrc[1] != 0 && pSrc[2] != 0) {
                    ++reqLength;
                    pSrc += 3;
                    continue;
                }
            } បើមិនដូច្នេះ /* f0..f4 */ { /* U+10000..U+10FFFF */
                ប្រសិនបើ(pSrc[1] != 0 && pSrc[2] != 0 && pSrc[3] != 0) {
                    reqLength += 2;
                    pSrc += 4;
                    continue;
                }
            }

            /* truncated character at the end */
            ++reqLength;
            bha_nga;
        }
    } បើមិនដូច្នេះ /* srcLength >= 0 */ {
      const uint8_t *pSrcLimit = (pSrc!=nullptr)?(pSrc + srcLength):nullptr;

        /*
         * This function requires that if srcLength is given, then it must be
         * destCapatity >= srcLength so that we need not check for
         * destination buffer overflow in the loop.
         */
        ប្រសិនបើ(destCapacity < srcLength) {
            ប្រសិនបើ(pDestLength != nullptr) {
                *pDestLength = srcLength; /* this likely overestimates the true destLength! */
            }
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            ត្រឡប់ nullptr;
        }

        ប្រសិនបើ((pSrcLimit - pSrc) >= 4) {
            pSrcLimit -= 3; /* temporarily reduce pSrcLimit */

            /* in this loop, we can always access at least 4 bytes, up to pSrc+3 */
            do {
                ch = *pSrc++;
                ប្រសិនបើ(ch < 0xc0) {
                    /*
                     * ASCII, or a trail byte in lead position which is treated like
                     * a single-byte sequence for better character boundary
                     * resynchronization after illegal sequences.
                     */
                    *pDest++=(char16_t)ch;
                } បើមិនដូច្នេះ ប្រសិនបើ(ch < 0xe0) { /* U+0080..U+07FF */
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                } បើមិនដូច្នេះ ប្រសិនបើ(ch < 0xf0) { /* U+0800..U+FFFF */
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                } បើមិនដូច្នេះ /* f0..f4 */ { /* U+10000..U+10FFFF */
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                }
            } ខណៈពេល(pSrc < pSrcLimit);

            pSrcLimit += 3; /* restore original pSrcLimit */
        }

        ខណៈពេល(pSrc < pSrcLimit) {
            ch = *pSrc++;
            ប្រសិនបើ(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                continue;
            } បើមិនដូច្នេះ ប្រសិនបើ(ch < 0xe0) { /* U+0080..U+07FF */
                ប្រសិនបើ(pSrc < pSrcLimit) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                    continue;
                }
            } បើមិនដូច្នេះ ប្រសិនបើ(ch < 0xf0) { /* U+0800..U+FFFF */
                ប្រសិនបើ((pSrcLimit - pSrc) >= 2) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                    pSrc += 3;
                    continue;
                }
            } បើមិនដូច្នេះ /* f0..f4 */ { /* U+10000..U+10FFFF */
                ប្រសិនបើ((pSrcLimit - pSrc) >= 3) {
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
            bha_nga;
        }
    }

    reqLength+=(int32_t)(pDest - dest);

    ប្រសិនបើ(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,reqLength,pErrorCode);

    ត្រឡប់ dest;
}

static anta_hstha uint8_t *
_appendUTF8(uint8_t *pDest, UChar32 c) {
    /* it is 0<=c<=0x10ffff and not a surrogate if called by a validating function */
    ប្រសិនបើ((c)<=0x7f) {
        *pDest++ = static_cast<uint8_t>(c);
    } បើមិនដូច្នេះ ប្រសិនបើ(c<=0x7ff) {
        *pDest++ = static_cast<uint8_t>((c >> 6) | 0xc0);
        *pDest++ = static_cast<uint8_t>((c & 0x3f) | 0x80);
    } បើមិនដូច្នេះ ប្រសិនបើ(c<=0xffff) {
        *pDest++ = static_cast<uint8_t>((c >> 12) | 0xe0);
        *pDest++ = static_cast<uint8_t>(((c >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    } បើមិនដូច្នេះ /* if((uint32_t)(c)<=0x10ffff) */ {
        *pDest++ = static_cast<uint8_t>(((c) >> 18) | 0xf0);
        *pDest++ = static_cast<uint8_t>((((c) >> 12) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>((((c) >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    }
    ត្រឡប់ pDest;
}

U_CAPI var_na* U_EXPORT2
u_strToUTF8WithSub(var_na* dest U_LIFETIME_BOUND,
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
    ប្រសិនបើ(U_FAILURE(*pErrorCode)){
        ត្រឡប់ nullptr;
    }
        
    ប្រសិនបើ( (pSrc==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ nullptr;
    }

    ប្រសិនបើ(pNumSubstitutions!=nullptr) {
        *pNumSubstitutions=0;
    }
    numSubstitutions=0;

    ប្រសិនបើ(srcLength==-1) {
        ខណៈពេល((ch=*pSrc)!=0) {
            ++pSrc;
            ប្រសិនបើ(ch <= 0x7f) {
                ប្រសិនបើ(pDest<pDestLimit) {
                    *pDest++ = (uint8_t)ch;
                } បើមិនដូច្នេះ {
                    reqLength = 1;
                    bha_nga;
                }
            } បើមិនដូច្នេះ ប្រសិនបើ(ch <= 0x7ff) {
                ប្រសិនបើ((pDestLimit - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } បើមិនដូច្នេះ {
                    reqLength = 2;
                    bha_nga;
                }
            } បើមិនដូច្នេះ ប្រសិនបើ(ch <= 0xd7ff || ch >= 0xe000) {
                ប្រសិនបើ((pDestLimit - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } បើមិនដូច្នេះ {
                    reqLength = 3;
                    bha_nga;
                }
            } បើមិនដូច្នេះ /* ch is a surrogate */ {
                int32_t length;

                /*need not check for NUL because NUL fails U16_IS_TRAIL() anyway*/
                ប្រសិនបើ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } បើមិនដូច្នេះ ប្រសិនបើ(subchar>=0) {
                    ch=subchar;
                    ++numSubstitutions;
                } បើមិនដូច្នេះ {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    ត្រឡប់ nullptr;
                }

                length = U8_LENGTH(ch);
                ប្រសិនបើ((pDestLimit - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } បើមិនដូច្នេះ {
                    reqLength = length;
                    bha_nga;
                }
            }
        }
        ខណៈពេល((ch=*pSrc++)!=0) {
            ប្រសិនបើ(ch<=0x7f) {
                ++reqLength;
            } បើមិនដូច្នេះ ប្រសិនបើ(ch<=0x7ff) {
                reqLength+=2;
            } បើមិនដូច្នេះ ប្រសិនបើ(!U16_IS_SURROGATE(ch)) {
                reqLength+=3;
            } បើមិនដូច្នេះ ប្រសិនបើ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                reqLength+=4;
            } បើមិនដូច្នេះ ប្រសិនបើ(subchar>=0) {
                reqLength+=U8_LENGTH(subchar);
                ++numSubstitutions;
            } បើមិនដូច្នេះ {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ត្រឡប់ nullptr;
            }
        }
    } បើមិនដូច្នេះ {
        const char16_t *pSrcLimit = (pSrc!=nullptr)?(pSrc+srcLength):nullptr;
        int32_t count;

        /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
        សម្រាប់(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            count = (int32_t)((pDestLimit - pDest) / 3);
            srcLength = (int32_t)(pSrcLimit - pSrc);
            ប្រសិនបើ(count > srcLength) {
                count = srcLength; /* min(remaining dest/3, remaining src) */
            }
            ប្រសិនបើ(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                bha_nga;
            }
            do {
                ch=*pSrc++;
                ប្រសិនបើ(ch <= 0x7f) {
                    *pDest++ = (uint8_t)ch;
                } បើមិនដូច្នេះ ប្រសិនបើ(ch <= 0x7ff) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } បើមិនដូច្នេះ ប្រសិនបើ(ch <= 0xd7ff || ch >= 0xe000) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } បើមិនដូច្នេះ /* ch is a surrogate */ {
                    /*
                     * We will read two UChars and probably output four bytes,
                     * which we didn't account for with computing count,
                     * so we adjust it here.
                     */
                    ប្រសិនបើ(--count == 0) {
                        --pSrc; /* undo ch=*pSrc++ for the lead surrogate */
                        bha_nga;  /* recompute count */
                    }

                    ប្រសិនបើ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                        ++pSrc;
                        ch=U16_GET_SUPPLEMENTARY(ch, ch2);

                        /* writing 4 bytes per 2 UChars is ok */
                        *pDest++=(uint8_t)((ch>>18)|0xf0);
                        *pDest++=(uint8_t)(((ch>>12)&0x3f)|0x80);
                        *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                        *pDest++=(uint8_t)((ch&0x3f)|0x80);
                    } បើមិនដូច្នេះ  {
                        /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                        ប្រសិនបើ(subchar>=0) {
                            ch=subchar;
                            ++numSubstitutions;
                        } បើមិនដូច្នេះ {
                            *pErrorCode = U_INVALID_CHAR_FOUND;
                            ត្រឡប់ nullptr;
                        }

                        /* convert and append*/
                        pDest=_appendUTF8(pDest, ch);
                    }
                }
            } ខណៈពេល(--count > 0);
        }

        ខណៈពេល(pSrc<pSrcLimit) {
            ch=*pSrc++;
            ប្រសិនបើ(ch <= 0x7f) {
                ប្រសិនបើ(pDest<pDestLimit) {
                    *pDest++ = (uint8_t)ch;
                } បើមិនដូច្នេះ {
                    reqLength = 1;
                    bha_nga;
                }
            } បើមិនដូច្នេះ ប្រសិនបើ(ch <= 0x7ff) {
                ប្រសិនបើ((pDestLimit - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } បើមិនដូច្នេះ {
                    reqLength = 2;
                    bha_nga;
                }
            } បើមិនដូច្នេះ ប្រសិនបើ(ch <= 0xd7ff || ch >= 0xe000) {
                ប្រសិនបើ((pDestLimit - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } បើមិនដូច្នេះ {
                    reqLength = 3;
                    bha_nga;
                }
            } បើមិនដូច្នេះ /* ch is a surrogate */ {
                int32_t length;

                ប្រសិនបើ(U16_IS_SURROGATE_LEAD(ch) && pSrc<pSrcLimit && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } បើមិនដូច្នេះ ប្រសិនបើ(subchar>=0) {
                    ch=subchar;
                    ++numSubstitutions;
                } បើមិនដូច្នេះ {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    ត្រឡប់ nullptr;
                }

                length = U8_LENGTH(ch);
                ប្រសិនបើ((pDestLimit - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } បើមិនដូច្នេះ {
                    reqLength = length;
                    bha_nga;
                }
            }
        }
        ខណៈពេល(pSrc<pSrcLimit) {
            ch=*pSrc++;
            ប្រសិនបើ(ch<=0x7f) {
                ++reqLength;
            } បើមិនដូច្នេះ ប្រសិនបើ(ch<=0x7ff) {
                reqLength+=2;
            } បើមិនដូច្នេះ ប្រសិនបើ(!U16_IS_SURROGATE(ch)) {
                reqLength+=3;
            } បើមិនដូច្នេះ ប្រសិនបើ(U16_IS_SURROGATE_LEAD(ch) && pSrc<pSrcLimit && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                reqLength+=4;
            } បើមិនដូច្នេះ ប្រសិនបើ(subchar>=0) {
                reqLength+=U8_LENGTH(subchar);
                ++numSubstitutions;
            } បើមិនដូច្នេះ {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ត្រឡប់ nullptr;
            }
        }
    }

    reqLength+=(int32_t)(pDest - (uint8_t *)dest);

    ប្រសិនបើ(pNumSubstitutions!=nullptr) {
        *pNumSubstitutions=numSubstitutions;
    }

    ប្រសិនបើ(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
    ត្រឡប់ dest;
}

U_CAPI var_na* U_EXPORT2
u_strToUTF8(var_na* dest U_LIFETIME_BOUND,
            int32_t destCapacity,
            int32_t* pDestLength,
            const char16_t* pSrc,
            int32_t srcLength,
            UErrorCode* pErrorCode) {
    ត្រឡប់ u_strToUTF8WithSub(
            dest, destCapacity, pDestLength,
            pSrc, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromJavaModifiedUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                                 int32_t destCapacity,
                                 int32_t* pDestLength,
                                 const var_na* src,
                                 int32_t srcLength,
                                 UChar32 subchar,
                                 int32_t* pNumSubstitutions,
                                 UErrorCode* pErrorCode) {
    /* args check */
    ប្រសិនបើ(U_FAILURE(*pErrorCode)) {
        ត្រឡប់ nullptr;
    }
    ប្រសិនបើ( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (dest==nullptr && destCapacity!=0) || destCapacity<0 ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ nullptr;
    }

    ប្រសិនបើ(pNumSubstitutions!=nullptr) {
        *pNumSubstitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *pDestLimit = dest+destCapacity;
    int32_t reqLength = 0;
    int32_t numSubstitutions=0;

    ប្រសិនបើ(srcLength < 0) {
        /*
         * Transform a NUL-terminated ASCII string.
         * Handle non-ASCII strings with slower code.
         */
        UChar32 c;
        ខណៈពេល(((c = (uint8_t)*src) != 0) && c <= 0x7f && (pDest < pDestLimit)) {
            *pDest++=(char16_t)c;
            ++src;
        }
        ប្រសិនបើ(c == 0) {
            reqLength=(int32_t)(pDest - dest);
            ប្រសិនបើ(pDestLength) {
                *pDestLength = reqLength;
            }

            /* Terminate the buffer */
            u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
            ត្រឡប់ dest;
        }
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    /* Faster loop without ongoing checking for srcLength and pDestLimit. */
    UChar32 ch;
    uint8_t t1, t2;
    int32_t i = 0;
    សម្រាប់(;;) {
        int32_t count = (int32_t)(pDestLimit - pDest);
        int32_t count2 = srcLength - i;
        ប្រសិនបើ(count >= count2 && srcLength > 0 && U8_IS_SINGLE(*src)) {
            /* fast ASCII loop */
            int32_t start = i;
            uint8_t b;
            ខណៈពេល(i < srcLength && U8_IS_SINGLE(b = src[i])) {
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
        ប្រសិនបើ(subchar > 0xFFFF) {
            bha_nga;
        }
        count2 /= 3;
        ប្រសិនបើ(count > count2) {
            count = count2; /* min(remaining dest, remaining src/3) */
        }
        ប្រសិនបើ(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            bha_nga;
        }
        do {
            ch = (uint8_t)src[i++];
            ប្រសិនបើ(U8_IS_SINGLE(ch)) {
                *pDest++=(char16_t)ch;
            } បើមិនដូច្នេះ {
                ប្រសិនបើ(ch >= 0xe0) {
                    ប្រសិនបើ( /* handle U+0000..U+FFFF inline */
                        ch <= 0xef &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                        (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                    ) {
                        /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                        *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                        i += 2;
                        continue;
                    }
                } បើមិនដូច្នេះ {
                    ប្រសិនបើ( /* handle U+0000..U+07FF inline */
                        ch >= 0xc0 &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                    ) {
                        *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                        ++i;
                        continue;
                    }
                }

                ប្រសិនបើ(subchar < 0) {
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    ត្រឡប់ nullptr;
                } បើមិនដូច្នេះ ប្រសិនបើ(subchar > 0xffff && --count == 0) {
                    /*
                     * We need to write two UChars, adjusted count for that,
                     * and ran out of space.
                     */
                    --i;  // back out byte ch
                    bha_nga;
                } បើមិនដូច្នេះ {
                    /* function call for error cases */
                    utf8_nextCharSafeBody((const uint8_t *)src, &(i), srcLength, ch, -1);
                    ++numSubstitutions;
                    *(pDest++)=(char16_t)subchar;
                }
            }
        } ខណៈពេល(--count > 0);
    }

    ខណៈពេល(i < srcLength && (pDest < pDestLimit)) {
        ch = (uint8_t)src[i++];
        ប្រសិនបើ(U8_IS_SINGLE(ch)){
            *pDest++=(char16_t)ch;
        } បើមិនដូច្នេះ {
            ប្រសិនបើ(ch >= 0xe0) {
                ប្រសិនបើ( /* handle U+0000..U+FFFF inline */
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
            } បើមិនដូច្នេះ {
                ប្រសិនបើ( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < srcLength &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                ) {
                    *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                    ++i;
                    continue;
                }
            }

            ប្រសិនបើ(subchar < 0) {
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ត្រឡប់ nullptr;
            } បើមិនដូច្នេះ {
                /* function call for error cases */
                utf8_nextCharSafeBody((const uint8_t *)src, &(i), srcLength, ch, -1);
                ++numSubstitutions;
                ប្រសិនបើ(subchar<=0xFFFF) {
                    *(pDest++)=(char16_t)subchar;
                } បើមិនដូច្នេះ {
                    *(pDest++)=U16_LEAD(subchar);
                    ប្រសិនបើ(pDest<pDestLimit) {
                        *(pDest++)=U16_TRAIL(subchar);
                    } បើមិនដូច្នេះ {
                        reqLength++;
                        bha_nga;
                    }
                }
            }
        }
    }

    /* Pre-flight the rest of the string. */
    ខណៈពេល(i < srcLength) {
        ch = (uint8_t)src[i++];
        ប្រសិនបើ(U8_IS_SINGLE(ch)) {
            reqLength++;
        } បើមិនដូច្នេះ {
            ប្រសិនបើ(ch >= 0xe0) {
                ប្រសិនបើ( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < srcLength &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f &&
                    (uint8_t)(src[i+1] - 0x80) <= 0x3f
                ) {
                    reqLength++;
                    i += 2;
                    continue;
                }
            } បើមិនដូច្នេះ {
                ប្រសិនបើ( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < srcLength &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f
                ) {
                    reqLength++;
                    ++i;
                    continue;
                }
            }

            ប្រសិនបើ(subchar < 0) {
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ត្រឡប់ nullptr;
            } បើមិនដូច្នេះ {
                /* function call for error cases */
                utf8_nextCharSafeBody((const uint8_t *)src, &(i), srcLength, ch, -1);
                ++numSubstitutions;
                reqLength+=U16_LENGTH(ch);
            }
        }
    }

    ប្រសិនបើ(pNumSubstitutions!=nullptr) {
        *pNumSubstitutions=numSubstitutions;
    }

    reqLength+=(int32_t)(pDest - dest);
    ប្រសិនបើ(pDestLength) {
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
    ត្រឡប់ dest;
}

U_CAPI var_na* U_EXPORT2
u_strToJavaModifiedUTF8(var_na* dest U_LIFETIME_BOUND,
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
    ប្រសិនបើ(U_FAILURE(*pErrorCode)){
        ត្រឡប់ nullptr;
    }
    ប្រសិនបើ( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (dest==nullptr && destCapacity!=0) || destCapacity<0
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ nullptr;
    }
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *pDestLimit = pDest + destCapacity;

    ប្រសិនបើ(srcLength==-1) {
        /* Convert NUL-terminated ASCII, then find the string length. */
        ខណៈពេល((ch=*src)<=0x7f && ch != 0 && pDest<pDestLimit) {
            *pDest++ = (uint8_t)ch;
            ++src;
        }
        ប្រសិនបើ(ch == 0) {
            reqLength=(int32_t)(pDest - (uint8_t *)dest);
            ប្រសិនបើ(pDestLength) {
                *pDestLength = reqLength;
            }

            /* Terminate the buffer */
            u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
            ត្រឡប់ dest;
        }
        srcLength = u_strlen(src);
    }

    /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
    pSrcLimit = (src!=nullptr)?(src+srcLength):nullptr;
    សម្រាប់(;;) {
        count = (int32_t)(pDestLimit - pDest);
        srcLength = (int32_t)(pSrcLimit - src);
        ប្រសិនបើ(count >= srcLength && srcLength > 0 && *src <= 0x7f) {
            /* fast ASCII loop */
            const char16_t *prevSrc = src;
            int32_t delta;
            ខណៈពេល(src < pSrcLimit && (ch = *src) <= 0x7f && ch != 0) {
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
        ប្រសិនបើ(count > srcLength) {
            count = srcLength; /* min(remaining dest/3, remaining src) */
        }
        ប្រសិនបើ(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            bha_nga;
        }
        do {
            ch=*src++;
            ប្រសិនបើ(ch <= 0x7f && ch != 0) {
                *pDest++ = (uint8_t)ch;
            } បើមិនដូច្នេះ ប្រសិនបើ(ch <= 0x7ff) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } បើមិនដូច្នេះ {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            }
        } ខណៈពេល(--count > 0);
    }

    ខណៈពេល(src<pSrcLimit) {
        ch=*src++;
        ប្រសិនបើ(ch <= 0x7f && ch != 0) {
            ប្រសិនបើ(pDest<pDestLimit) {
                *pDest++ = (uint8_t)ch;
            } បើមិនដូច្នេះ {
                reqLength = 1;
                bha_nga;
            }
        } បើមិនដូច្នេះ ប្រសិនបើ(ch <= 0x7ff) {
            ប្រសិនបើ((pDestLimit - pDest) >= 2) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } បើមិនដូច្នេះ {
                reqLength = 2;
                bha_nga;
            }
        } បើមិនដូច្នេះ {
            ប្រសិនបើ((pDestLimit - pDest) >= 3) {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } បើមិនដូច្នេះ {
                reqLength = 3;
                bha_nga;
            }
        }
    }
    ខណៈពេល(src<pSrcLimit) {
        ch=*src++;
        ប្រសិនបើ(ch <= 0x7f && ch != 0) {
            ++reqLength;
        } បើមិនដូច្នេះ ប្រសិនបើ(ch<=0x7ff) {
            reqLength+=2;
        } បើមិនដូច្នេះ {
            reqLength+=3;
        }
    }

    reqLength+=(int32_t)(pDest - (uint8_t *)dest);
    ប្រសិនបើ(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
    ត្រឡប់ dest;
}
