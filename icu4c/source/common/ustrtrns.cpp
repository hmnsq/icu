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


#অন্তর্ভাৱ "unicode/putil.h"
#অন্তর্ভাৱ "unicode/ustring.h"
#অন্তর্ভাৱ "unicode/utf.h"
#অন্তর্ভাৱ "unicode/utf8.h"
#অন্তর্ভাৱ "unicode/utf16.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "ustr_imp.h"
#অন্তর্ভাৱ "uassert.h"

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32WithSub(char16_t* dest U_LIFETIME_BOUND,
                      int32_t destCapacity,
                      int32_t* pDestLength,
                      ধ্রুৱ UChar32* src,
                      int32_t srcLength,
                      UChar32 subchar,
                      int32_t* p_অঙ্ক_Substitutions,
                      UErrorCode* pErrorCode) {
    ধ্রুৱ UChar32 *srcLimit;
    UChar32 ch;
    char16_t *destLimit;
    char16_t *pDest;
    int32_t reqLength;
    int32_t অঙ্ক_Substitutions;

    /* args check */
    যদি(U_FAILURE(*pErrorCode)){
        ঘূৰাই nullptr;
    }
    যদি( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }

    যদি(p_অঙ্ক_Substitutions != nullptr) {
        *p_অঙ্ক_Substitutions = 0;
    }

    pDest = dest;
    destLimit = (dest!=nullptr)?(dest + destCapacity):nullptr;
    reqLength = 0;
    অঙ্ক_Substitutions = 0;

    যদি(srcLength < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        যেতিয়ালৈ((ch=*src) != 0 &&
              ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff))) {
            ++src;
            যদি(pDest < destLimit) {
                *pDest++ = (char16_t)ch;
            } অন্যথা {
                ++reqLength;
            }
        }
        srcLimit = src;
        যদি(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            যেতিয়ালৈ(*++srcLimit != 0) {}
        }
    } অন্যথা {
      srcLimit = (src!=nullptr)?(src + srcLength):nullptr;
    }

    /* convert with length */
    যেতিয়ালৈ(src < srcLimit) {
        ch = *src++;
        কৰা {
            /* usually "loops" once; twice only for writing subchar */
            যদি((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff)) {
                যদি(pDest < destLimit) {
                    *pDest++ = (char16_t)ch;
                } অন্যথা {
                    ++reqLength;
                }
                ভঙ্গ;
            } অন্যথা যদি(0x10000 <= ch && ch <= 0x10ffff) {
                যদি(pDest!=nullptr && ((pDest + 2) <= destLimit)) {
                    *pDest++ = U16_LEAD(ch);
                    *pDest++ = U16_TRAIL(ch);
                } অন্যথা {
                    reqLength += 2;
                }
                ভঙ্গ;
            } অন্যথা যদি((ch = subchar) < 0) {
                /* surrogate code point, or not a Unicode code point at all */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ঘূৰাই nullptr;
            } অন্যথা {
                ++অঙ্ক_Substitutions;
            }
        } যেতিয়ালৈ(সঁচা);
    }

    reqLength += (int32_t)(pDest - dest);
    যদি(pDestLength) {
        *pDestLength = reqLength;
    }
    যদি(p_অঙ্ক_Substitutions != nullptr) {
        *p_অঙ্ক_Substitutions = অঙ্ক_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
    
    ঘূৰাই dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32(char16_t* dest U_LIFETIME_BOUND,
               int32_t destCapacity,
               int32_t* pDestLength,
               ধ্রুৱ UChar32* src,
               int32_t srcLength,
               UErrorCode* pErrorCode) {
    ঘূৰাই u_strFromUTF32WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI UChar32* U_EXPORT2
u_strToUTF32WithSub(UChar32* dest U_LIFETIME_BOUND,
                    int32_t destCapacity,
                    int32_t* pDestLength,
                    ধ্রুৱ char16_t* src,
                    int32_t srcLength,
                    UChar32 subchar,
                    int32_t* p_অঙ্ক_Substitutions,
                    UErrorCode* pErrorCode) {
    ধ্রুৱ char16_t *srcLimit;
    UChar32 ch;
    char16_t ch2;
    UChar32 *destLimit;
    UChar32 *pDest;
    int32_t reqLength;
    int32_t অঙ্ক_Substitutions;

    /* args check */
    যদি(U_FAILURE(*pErrorCode)){
        ঘূৰাই nullptr;
    }
    যদি( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }

    যদি(p_অঙ্ক_Substitutions != nullptr) {
        *p_অঙ্ক_Substitutions = 0;
    }

    pDest = dest;
    destLimit = (dest!=nullptr)?(dest + destCapacity):nullptr;
    reqLength = 0;
    অঙ্ক_Substitutions = 0;

    যদি(srcLength < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        যেতিয়ালৈ((ch=*src) != 0 && !U16_IS_SURROGATE(ch)) {
            ++src;
            যদি(pDest < destLimit) {
                *pDest++ = ch;
            } অন্যথা {
                ++reqLength;
            }
        }
        srcLimit = src;
        যদি(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            যেতিয়ালৈ(*++srcLimit != 0) {}
        }
    } অন্যথা {
        srcLimit = (src!=nullptr)?(src + srcLength):nullptr;
    }

    /* convert with length */
    যেতিয়ালৈ(src < srcLimit) {
        ch = *src++;
        যদি(!U16_IS_SURROGATE(ch)) {
            /* write or count ch below */
        } অন্যথা যদি(U16_IS_SURROGATE_LEAD(ch) && src < srcLimit && U16_IS_TRAIL(ch2 = *src)) {
            ++src;
            ch = U16_GET_SUPPLEMENTARY(ch, ch2);
        } অন্যথা যদি((ch = subchar) < 0) {
            /* unpaired surrogate */
            *pErrorCode = U_INVALID_CHAR_FOUND;
            ঘূৰাই nullptr;
        } অন্যথা {
            ++অঙ্ক_Substitutions;
        }
        যদি(pDest < destLimit) {
            *pDest++ = ch;
        } অন্যথা {
            ++reqLength;
        }
    }

    reqLength += (int32_t)(pDest - dest);
    যদি(pDestLength) {
        *pDestLength = reqLength;
    }
    যদি(p_অঙ্ক_Substitutions != nullptr) {
        *p_অঙ্ক_Substitutions = অঙ্ক_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChar32s(dest, destCapacity, reqLength, pErrorCode);

    ঘূৰাই dest;
}

U_CAPI UChar32* U_EXPORT2
u_strToUTF32(UChar32* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* pDestLength,
             ধ্রুৱ char16_t* src,
             int32_t srcLength,
             UErrorCode* pErrorCode) {
    ঘূৰাই u_strToUTF32WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                     int32_t destCapacity,
                     int32_t* pDestLength,
                     ধ্রুৱ ৱর্ণ* src,
                     int32_t srcLength,
                     UChar32 subchar,
                     int32_t* p_অঙ্ক_Substitutions,
                     UErrorCode* pErrorCode) {
    /* args check */
    যদি(U_FAILURE(*pErrorCode)) {
        ঘূৰাই nullptr;
    }
    যদি( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }

    যদি(p_অঙ্ক_Substitutions!=nullptr) {
        *p_অঙ্ক_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *pDestLimit = dest+destCapacity;
    int32_t reqLength = 0;
    int32_t অঙ্ক_Substitutions=0;

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

    যদি(srcLength < 0){
        /*
         * Transform a NUL-terminated string.
         * The code explicitly checks for NULs only in the lead byte position.
         * A NUL byte in the trail byte position fails the trail byte range check anyway.
         */
        int32_t i;
        UChar32 c;
        বাবে(i = 0; (c = (uint8_t)src[i]) != 0 && (pDest < pDestLimit);) {
            // modified copy of U8_NEXT()
            ++i;
            যদি(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } অন্যথা {
                uint8_t __t1, __t2;
                যদি( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } অন্যথা যদি( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } অন্যথা {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((ধ্রুৱ uint8_t *)src, &(i), -1, c, -1);
                    যদি(c<0 && (++অঙ্ক_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        ঘূৰাই nullptr;
                    } অন্যথা যদি(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } অন্যথা {
                        *(pDest++)=U16_LEAD(c);
                        যদি(pDest<pDestLimit) {
                            *(pDest++)=U16_TRAIL(c);
                        } অন্যথা {
                            reqLength++;
                            ভঙ্গ;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        যেতিয়ালৈ((c = (uint8_t)src[i]) != 0) {
            // modified copy of U8_NEXT()
            ++i;
            যদি(U8_IS_SINGLE(c)) {
                ++reqLength;
            } অন্যথা {
                uint8_t __t1, __t2;
                যদি( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++reqLength;
                    i+=2;
                } অন্যথা যদি( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++reqLength;
                    ++(i);
                } অন্যথা {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((ধ্রুৱ uint8_t *)src, &(i), -1, c, -1);
                    যদি(c<0 && (++অঙ্ক_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        ঘূৰাই nullptr;
                    }
                    reqLength += U16_LENGTH(c);
                }
            }
        }
    } অন্যথা /* srcLength >= 0 */ {
        /* Faster loop without ongoing checking for srcLength and pDestLimit. */
        int32_t i = 0;
        UChar32 c;
        বাবে(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            int32_t count = (int32_t)(pDestLimit - pDest);
            int32_t count2 = (srcLength - i) / 3;
            যদি(count > count2) {
                count = count2; /* min(remaining dest, remaining src/3) */
            }
            যদি(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                ভঙ্গ;
            }

            কৰা {
                // modified copy of U8_NEXT()
                c = (uint8_t)src[i++];
                যদি(U8_IS_SINGLE(c)) {
                    *pDest++=(char16_t)c;
                } অন্যথা {
                    uint8_t __t1, __t2;
                    যদি( /* handle U+0800..U+FFFF inline */
                            (0xe0<=(c) && (c)<0xf0) &&
                            ((i)+1)<srcLength &&
                            U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                            (__t2=src[(i)+1]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                        i+=2;
                    } অন্যথা যদি( /* handle U+0080..U+07FF inline */
                            ((c)<0xe0 && (c)>=0xc2) &&
                            ((i)!=srcLength) &&
                            (__t1=src[i]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0x1f)<<6)|__t1;
                        ++(i);
                    } অন্যথা {
                        যদি(c >= 0xf0 || subchar > 0xffff) {
                            // We may read up to four bytes and write up to two UChars,
                            // which we didn't account for with computing count,
                            // so we adjust it here.
                            যদি(--count == 0) {
                                --i;  // back out byte c
                                ভঙ্গ;
                            }
                        }

                        /* function call for "complicated" and error cases */
                        (c)=utf8_nextCharSafeBody((ধ্রুৱ uint8_t *)src, &(i), srcLength, c, -1);
                        যদি(c<0 && (++অঙ্ক_Substitutions, c = subchar) < 0) {
                            *pErrorCode = U_INVALID_CHAR_FOUND;
                            ঘূৰাই nullptr;
                        } অন্যথা যদি(c<=0xFFFF) {
                            *(pDest++)=(char16_t)c;
                        } অন্যথা {
                            *(pDest++)=U16_LEAD(c);
                            *(pDest++)=U16_TRAIL(c);
                        }
                    }
                }
            } যেতিয়ালৈ(--count > 0);
        }

        যেতিয়ালৈ(i < srcLength && (pDest < pDestLimit)) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            যদি(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } অন্যথা {
                uint8_t __t1, __t2;
                যদি( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<srcLength &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } অন্যথা যদি( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=srcLength) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } অন্যথা {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((ধ্রুৱ uint8_t *)src, &(i), srcLength, c, -1);
                    যদি(c<0 && (++অঙ্ক_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        ঘূৰাই nullptr;
                    } অন্যথা যদি(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } অন্যথা {
                        *(pDest++)=U16_LEAD(c);
                        যদি(pDest<pDestLimit) {
                            *(pDest++)=U16_TRAIL(c);
                        } অন্যথা {
                            reqLength++;
                            ভঙ্গ;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        যেতিয়ালৈ(i < srcLength) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            যদি(U8_IS_SINGLE(c)) {
                ++reqLength;
            } অন্যথা {
                uint8_t __t1, __t2;
                যদি( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<srcLength &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++reqLength;
                    i+=2;
                } অন্যথা যদি( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=srcLength) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++reqLength;
                    ++(i);
                } অন্যথা {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((ধ্রুৱ uint8_t *)src, &(i), srcLength, c, -1);
                    যদি(c<0 && (++অঙ্ক_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        ঘূৰাই nullptr;
                    }
                    reqLength += U16_LENGTH(c);
                }
            }
        }
    }

    reqLength+=(int32_t)(pDest - dest);

    যদি(p_অঙ্ক_Substitutions!=nullptr) {
        *p_অঙ্ক_Substitutions=অঙ্ক_Substitutions;
    }

    যদি(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,reqLength,pErrorCode);

    ঘূৰাই dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8(char16_t* dest U_LIFETIME_BOUND,
              int32_t destCapacity,
              int32_t* pDestLength,
              ধ্রুৱ ৱর্ণ* src,
              int32_t srcLength,
              UErrorCode* pErrorCode) {
    ঘূৰাই u_strFromUTF8WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8Lenient(char16_t* dest U_LIFETIME_BOUND,
                     int32_t destCapacity,
                     int32_t* pDestLength,
                     ধ্রুৱ ৱর্ণ* src,
                     int32_t srcLength,
                     UErrorCode* pErrorCode) {
    char16_t *pDest = dest;
    UChar32 ch;
    int32_t reqLength = 0;
    uint8_t* pSrc = (uint8_t*) src;

    /* args check */
    যদি(U_FAILURE(*pErrorCode)){
        ঘূৰাই nullptr;
    }
        
    যদি( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }

    যদি(srcLength < 0) {
        /* Transform a NUL-terminated string. */
        char16_t *pDestLimit = (dest!=nullptr)?(dest+destCapacity):nullptr;
        uint8_t t1, t2, t3; /* trail bytes */

        যেতিয়ালৈ(((ch = *pSrc) != 0) && (pDest < pDestLimit)) {
            যদি(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ++pSrc;
                অনুৱর্ত;
            } অন্যথা যদি(ch < 0xe0) { /* U+0080..U+07FF */
                যদি((t1 = pSrc[1]) != 0) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + t1 - 0x3080);
                    pSrc += 2;
                    অনুৱর্ত;
                }
            } অন্যথা যদি(ch < 0xf0) { /* U+0800..U+FFFF */
                যদি((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 12) + (t1 << 6) + t2 - 0x2080);
                    pSrc += 3;
                    অনুৱর্ত;
                }
            } অন্যথা /* f0..f4 */ { /* U+10000..U+10FFFF */
                যদি((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0 && (t3 = pSrc[3]) != 0) {
                    pSrc += 4;
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (t1 << 12) + (t2 << 6) + t3 - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    যদি(pDest < pDestLimit) {
                        *(pDest++) = U16_TRAIL(ch);
                    } অন্যথা {
                        reqLength = 1;
                        ভঙ্গ;
                    }
                    অনুৱর্ত;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            যেতিয়ালৈ(*++pSrc != 0) {}
            ভঙ্গ;
        }

        /* Pre-flight the rest of the string. */
        যেতিয়ালৈ((ch = *pSrc) != 0) {
            যদি(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                ++reqLength;
                ++pSrc;
                অনুৱর্ত;
            } অন্যথা যদি(ch < 0xe0) { /* U+0080..U+07FF */
                যদি(pSrc[1] != 0) {
                    ++reqLength;
                    pSrc += 2;
                    অনুৱর্ত;
                }
            } অন্যথা যদি(ch < 0xf0) { /* U+0800..U+FFFF */
                যদি(pSrc[1] != 0 && pSrc[2] != 0) {
                    ++reqLength;
                    pSrc += 3;
                    অনুৱর্ত;
                }
            } অন্যথা /* f0..f4 */ { /* U+10000..U+10FFFF */
                যদি(pSrc[1] != 0 && pSrc[2] != 0 && pSrc[3] != 0) {
                    reqLength += 2;
                    pSrc += 4;
                    অনুৱর্ত;
                }
            }

            /* truncated character at the end */
            ++reqLength;
            ভঙ্গ;
        }
    } অন্যথা /* srcLength >= 0 */ {
      ধ্রুৱ uint8_t *pSrcLimit = (pSrc!=nullptr)?(pSrc + srcLength):nullptr;

        /*
         * This function requires that if srcLength is given, then it must be
         * destCapatity >= srcLength so that we need not check for
         * destination buffer overflow in the loop.
         */
        যদি(destCapacity < srcLength) {
            যদি(pDestLength != nullptr) {
                *pDestLength = srcLength; /* this likely overestimates the true destLength! */
            }
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            ঘূৰাই nullptr;
        }

        যদি((pSrcLimit - pSrc) >= 4) {
            pSrcLimit -= 3; /* temporarily reduce pSrcLimit */

            /* in this loop, we can always access at least 4 bytes, up to pSrc+3 */
            কৰা {
                ch = *pSrc++;
                যদি(ch < 0xc0) {
                    /*
                     * ASCII, or a trail byte in lead position which is treated like
                     * a single-byte sequence for better character boundary
                     * resynchronization after illegal sequences.
                     */
                    *pDest++=(char16_t)ch;
                } অন্যথা যদি(ch < 0xe0) { /* U+0080..U+07FF */
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                } অন্যথা যদি(ch < 0xf0) { /* U+0800..U+FFFF */
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                } অন্যথা /* f0..f4 */ { /* U+10000..U+10FFFF */
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                }
            } যেতিয়ালৈ(pSrc < pSrcLimit);

            pSrcLimit += 3; /* restore original pSrcLimit */
        }

        যেতিয়ালৈ(pSrc < pSrcLimit) {
            ch = *pSrc++;
            যদি(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                অনুৱর্ত;
            } অন্যথা যদি(ch < 0xe0) { /* U+0080..U+07FF */
                যদি(pSrc < pSrcLimit) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                    অনুৱর্ত;
                }
            } অন্যথা যদি(ch < 0xf0) { /* U+0800..U+FFFF */
                যদি((pSrcLimit - pSrc) >= 2) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                    pSrc += 3;
                    অনুৱর্ত;
                }
            } অন্যথা /* f0..f4 */ { /* U+10000..U+10FFFF */
                যদি((pSrcLimit - pSrc) >= 3) {
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                    pSrc += 4;
                    অনুৱর্ত;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            ভঙ্গ;
        }
    }

    reqLength+=(int32_t)(pDest - dest);

    যদি(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,reqLength,pErrorCode);

    ঘূৰাই dest;
}

স্থির অন্তঃস্থ uint8_t *
_appendUTF8(uint8_t *pDest, UChar32 c) {
    /* it is 0<=c<=0x10ffff and not a surrogate if called by a validating function */
    যদি((c)<=0x7f) {
        *pDest++ = static_cast<uint8_t>(c);
    } অন্যথা যদি(c<=0x7ff) {
        *pDest++ = static_cast<uint8_t>((c >> 6) | 0xc0);
        *pDest++ = static_cast<uint8_t>((c & 0x3f) | 0x80);
    } অন্যথা যদি(c<=0xffff) {
        *pDest++ = static_cast<uint8_t>((c >> 12) | 0xe0);
        *pDest++ = static_cast<uint8_t>(((c >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    } অন্যথা /* if((uint32_t)(c)<=0x10ffff) */ {
        *pDest++ = static_cast<uint8_t>(((c) >> 18) | 0xf0);
        *pDest++ = static_cast<uint8_t>((((c) >> 12) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>((((c) >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    }
    ঘূৰাই pDest;
}

U_CAPI ৱর্ণ* U_EXPORT2
u_strToUTF8WithSub(ৱর্ণ* dest U_LIFETIME_BOUND,
                   int32_t destCapacity,
                   int32_t* pDestLength,
                   ধ্রুৱ char16_t* pSrc,
                   int32_t srcLength,
                   UChar32 subchar,
                   int32_t* p_অঙ্ক_Substitutions,
                   UErrorCode* pErrorCode) {
    int32_t reqLength=0;
    uint32_t ch=0,ch2=0;
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *pDestLimit = (pDest!=nullptr)?(pDest + destCapacity):nullptr;
    int32_t অঙ্ক_Substitutions;

    /* args check */
    যদি(U_FAILURE(*pErrorCode)){
        ঘূৰাই nullptr;
    }
        
    যদি( (pSrc==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }

    যদি(p_অঙ্ক_Substitutions!=nullptr) {
        *p_অঙ্ক_Substitutions=0;
    }
    অঙ্ক_Substitutions=0;

    যদি(srcLength==-1) {
        যেতিয়ালৈ((ch=*pSrc)!=0) {
            ++pSrc;
            যদি(ch <= 0x7f) {
                যদি(pDest<pDestLimit) {
                    *pDest++ = (uint8_t)ch;
                } অন্যথা {
                    reqLength = 1;
                    ভঙ্গ;
                }
            } অন্যথা যদি(ch <= 0x7ff) {
                যদি((pDestLimit - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } অন্যথা {
                    reqLength = 2;
                    ভঙ্গ;
                }
            } অন্যথা যদি(ch <= 0xd7ff || ch >= 0xe000) {
                যদি((pDestLimit - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } অন্যথা {
                    reqLength = 3;
                    ভঙ্গ;
                }
            } অন্যথা /* ch is a surrogate */ {
                int32_t length;

                /*need not check for NUL because NUL fails U16_IS_TRAIL() anyway*/
                যদি(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } অন্যথা যদি(subchar>=0) {
                    ch=subchar;
                    ++অঙ্ক_Substitutions;
                } অন্যথা {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    ঘূৰাই nullptr;
                }

                length = U8_LENGTH(ch);
                যদি((pDestLimit - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } অন্যথা {
                    reqLength = length;
                    ভঙ্গ;
                }
            }
        }
        যেতিয়ালৈ((ch=*pSrc++)!=0) {
            যদি(ch<=0x7f) {
                ++reqLength;
            } অন্যথা যদি(ch<=0x7ff) {
                reqLength+=2;
            } অন্যথা যদি(!U16_IS_SURROGATE(ch)) {
                reqLength+=3;
            } অন্যথা যদি(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                reqLength+=4;
            } অন্যথা যদি(subchar>=0) {
                reqLength+=U8_LENGTH(subchar);
                ++অঙ্ক_Substitutions;
            } অন্যথা {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ঘূৰাই nullptr;
            }
        }
    } অন্যথা {
        ধ্রুৱ char16_t *pSrcLimit = (pSrc!=nullptr)?(pSrc+srcLength):nullptr;
        int32_t count;

        /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
        বাবে(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            count = (int32_t)((pDestLimit - pDest) / 3);
            srcLength = (int32_t)(pSrcLimit - pSrc);
            যদি(count > srcLength) {
                count = srcLength; /* min(remaining dest/3, remaining src) */
            }
            যদি(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                ভঙ্গ;
            }
            কৰা {
                ch=*pSrc++;
                যদি(ch <= 0x7f) {
                    *pDest++ = (uint8_t)ch;
                } অন্যথা যদি(ch <= 0x7ff) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } অন্যথা যদি(ch <= 0xd7ff || ch >= 0xe000) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } অন্যথা /* ch is a surrogate */ {
                    /*
                     * We will read two UChars and probably output four bytes,
                     * which we didn't account for with computing count,
                     * so we adjust it here.
                     */
                    যদি(--count == 0) {
                        --pSrc; /* undo ch=*pSrc++ for the lead surrogate */
                        ভঙ্গ;  /* recompute count */
                    }

                    যদি(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                        ++pSrc;
                        ch=U16_GET_SUPPLEMENTARY(ch, ch2);

                        /* writing 4 bytes per 2 UChars is ok */
                        *pDest++=(uint8_t)((ch>>18)|0xf0);
                        *pDest++=(uint8_t)(((ch>>12)&0x3f)|0x80);
                        *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                        *pDest++=(uint8_t)((ch&0x3f)|0x80);
                    } অন্যথা  {
                        /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                        যদি(subchar>=0) {
                            ch=subchar;
                            ++অঙ্ক_Substitutions;
                        } অন্যথা {
                            *pErrorCode = U_INVALID_CHAR_FOUND;
                            ঘূৰাই nullptr;
                        }

                        /* convert and append*/
                        pDest=_appendUTF8(pDest, ch);
                    }
                }
            } যেতিয়ালৈ(--count > 0);
        }

        যেতিয়ালৈ(pSrc<pSrcLimit) {
            ch=*pSrc++;
            যদি(ch <= 0x7f) {
                যদি(pDest<pDestLimit) {
                    *pDest++ = (uint8_t)ch;
                } অন্যথা {
                    reqLength = 1;
                    ভঙ্গ;
                }
            } অন্যথা যদি(ch <= 0x7ff) {
                যদি((pDestLimit - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } অন্যথা {
                    reqLength = 2;
                    ভঙ্গ;
                }
            } অন্যথা যদি(ch <= 0xd7ff || ch >= 0xe000) {
                যদি((pDestLimit - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } অন্যথা {
                    reqLength = 3;
                    ভঙ্গ;
                }
            } অন্যথা /* ch is a surrogate */ {
                int32_t length;

                যদি(U16_IS_SURROGATE_LEAD(ch) && pSrc<pSrcLimit && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } অন্যথা যদি(subchar>=0) {
                    ch=subchar;
                    ++অঙ্ক_Substitutions;
                } অন্যথা {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    ঘূৰাই nullptr;
                }

                length = U8_LENGTH(ch);
                যদি((pDestLimit - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } অন্যথা {
                    reqLength = length;
                    ভঙ্গ;
                }
            }
        }
        যেতিয়ালৈ(pSrc<pSrcLimit) {
            ch=*pSrc++;
            যদি(ch<=0x7f) {
                ++reqLength;
            } অন্যথা যদি(ch<=0x7ff) {
                reqLength+=2;
            } অন্যথা যদি(!U16_IS_SURROGATE(ch)) {
                reqLength+=3;
            } অন্যথা যদি(U16_IS_SURROGATE_LEAD(ch) && pSrc<pSrcLimit && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                reqLength+=4;
            } অন্যথা যদি(subchar>=0) {
                reqLength+=U8_LENGTH(subchar);
                ++অঙ্ক_Substitutions;
            } অন্যথা {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ঘূৰাই nullptr;
            }
        }
    }

    reqLength+=(int32_t)(pDest - (uint8_t *)dest);

    যদি(p_অঙ্ক_Substitutions!=nullptr) {
        *p_অঙ্ক_Substitutions=অঙ্ক_Substitutions;
    }

    যদি(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
    ঘূৰাই dest;
}

U_CAPI ৱর্ণ* U_EXPORT2
u_strToUTF8(ৱর্ণ* dest U_LIFETIME_BOUND,
            int32_t destCapacity,
            int32_t* pDestLength,
            ধ্রুৱ char16_t* pSrc,
            int32_t srcLength,
            UErrorCode* pErrorCode) {
    ঘূৰাই u_strToUTF8WithSub(
            dest, destCapacity, pDestLength,
            pSrc, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromJavaModifiedUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                                 int32_t destCapacity,
                                 int32_t* pDestLength,
                                 ধ্রুৱ ৱর্ণ* src,
                                 int32_t srcLength,
                                 UChar32 subchar,
                                 int32_t* p_অঙ্ক_Substitutions,
                                 UErrorCode* pErrorCode) {
    /* args check */
    যদি(U_FAILURE(*pErrorCode)) {
        ঘূৰাই nullptr;
    }
    যদি( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (dest==nullptr && destCapacity!=0) || destCapacity<0 ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }

    যদি(p_অঙ্ক_Substitutions!=nullptr) {
        *p_অঙ্ক_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *pDestLimit = dest+destCapacity;
    int32_t reqLength = 0;
    int32_t অঙ্ক_Substitutions=0;

    যদি(srcLength < 0) {
        /*
         * Transform a NUL-terminated ASCII string.
         * Handle non-ASCII strings with slower code.
         */
        UChar32 c;
        যেতিয়ালৈ(((c = (uint8_t)*src) != 0) && c <= 0x7f && (pDest < pDestLimit)) {
            *pDest++=(char16_t)c;
            ++src;
        }
        যদি(c == 0) {
            reqLength=(int32_t)(pDest - dest);
            যদি(pDestLength) {
                *pDestLength = reqLength;
            }

            /* Terminate the buffer */
            u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
            ঘূৰাই dest;
        }
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    /* Faster loop without ongoing checking for srcLength and pDestLimit. */
    UChar32 ch;
    uint8_t t1, t2;
    int32_t i = 0;
    বাবে(;;) {
        int32_t count = (int32_t)(pDestLimit - pDest);
        int32_t count2 = srcLength - i;
        যদি(count >= count2 && srcLength > 0 && U8_IS_SINGLE(*src)) {
            /* fast ASCII loop */
            int32_t start = i;
            uint8_t b;
            যেতিয়ালৈ(i < srcLength && U8_IS_SINGLE(b = src[i])) {
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
        যদি(subchar > 0xFFFF) {
            ভঙ্গ;
        }
        count2 /= 3;
        যদি(count > count2) {
            count = count2; /* min(remaining dest, remaining src/3) */
        }
        যদি(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            ভঙ্গ;
        }
        কৰা {
            ch = (uint8_t)src[i++];
            যদি(U8_IS_SINGLE(ch)) {
                *pDest++=(char16_t)ch;
            } অন্যথা {
                যদি(ch >= 0xe0) {
                    যদি( /* handle U+0000..U+FFFF inline */
                        ch <= 0xef &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                        (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                    ) {
                        /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                        *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                        i += 2;
                        অনুৱর্ত;
                    }
                } অন্যথা {
                    যদি( /* handle U+0000..U+07FF inline */
                        ch >= 0xc0 &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                    ) {
                        *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                        ++i;
                        অনুৱর্ত;
                    }
                }

                যদি(subchar < 0) {
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    ঘূৰাই nullptr;
                } অন্যথা যদি(subchar > 0xffff && --count == 0) {
                    /*
                     * We need to write two UChars, adjusted count for that,
                     * and ran out of space.
                     */
                    --i;  // back out byte ch
                    ভঙ্গ;
                } অন্যথা {
                    /* function call for error cases */
                    utf8_nextCharSafeBody((ধ্রুৱ uint8_t *)src, &(i), srcLength, ch, -1);
                    ++অঙ্ক_Substitutions;
                    *(pDest++)=(char16_t)subchar;
                }
            }
        } যেতিয়ালৈ(--count > 0);
    }

    যেতিয়ালৈ(i < srcLength && (pDest < pDestLimit)) {
        ch = (uint8_t)src[i++];
        যদি(U8_IS_SINGLE(ch)){
            *pDest++=(char16_t)ch;
        } অন্যথা {
            যদি(ch >= 0xe0) {
                যদি( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < srcLength &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                    (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                ) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                    i += 2;
                    অনুৱর্ত;
                }
            } অন্যথা {
                যদি( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < srcLength &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                ) {
                    *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                    ++i;
                    অনুৱর্ত;
                }
            }

            যদি(subchar < 0) {
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ঘূৰাই nullptr;
            } অন্যথা {
                /* function call for error cases */
                utf8_nextCharSafeBody((ধ্রুৱ uint8_t *)src, &(i), srcLength, ch, -1);
                ++অঙ্ক_Substitutions;
                যদি(subchar<=0xFFFF) {
                    *(pDest++)=(char16_t)subchar;
                } অন্যথা {
                    *(pDest++)=U16_LEAD(subchar);
                    যদি(pDest<pDestLimit) {
                        *(pDest++)=U16_TRAIL(subchar);
                    } অন্যথা {
                        reqLength++;
                        ভঙ্গ;
                    }
                }
            }
        }
    }

    /* Pre-flight the rest of the string. */
    যেতিয়ালৈ(i < srcLength) {
        ch = (uint8_t)src[i++];
        যদি(U8_IS_SINGLE(ch)) {
            reqLength++;
        } অন্যথা {
            যদি(ch >= 0xe0) {
                যদি( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < srcLength &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f &&
                    (uint8_t)(src[i+1] - 0x80) <= 0x3f
                ) {
                    reqLength++;
                    i += 2;
                    অনুৱর্ত;
                }
            } অন্যথা {
                যদি( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < srcLength &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f
                ) {
                    reqLength++;
                    ++i;
                    অনুৱর্ত;
                }
            }

            যদি(subchar < 0) {
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ঘূৰাই nullptr;
            } অন্যথা {
                /* function call for error cases */
                utf8_nextCharSafeBody((ধ্রুৱ uint8_t *)src, &(i), srcLength, ch, -1);
                ++অঙ্ক_Substitutions;
                reqLength+=U16_LENGTH(ch);
            }
        }
    }

    যদি(p_অঙ্ক_Substitutions!=nullptr) {
        *p_অঙ্ক_Substitutions=অঙ্ক_Substitutions;
    }

    reqLength+=(int32_t)(pDest - dest);
    যদি(pDestLength) {
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
    ঘূৰাই dest;
}

U_CAPI ৱর্ণ* U_EXPORT2
u_strToJavaModifiedUTF8(ৱর্ণ* dest U_LIFETIME_BOUND,
                        int32_t destCapacity,
                        int32_t* pDestLength,
                        ধ্রুৱ char16_t* src,
                        int32_t srcLength,
                        UErrorCode* pErrorCode) {
    int32_t reqLength=0;
    uint32_t ch=0;
    ধ্রুৱ char16_t *pSrcLimit;
    int32_t count;

    /* args check */
    যদি(U_FAILURE(*pErrorCode)){
        ঘূৰাই nullptr;
    }
    যদি( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (dest==nullptr && destCapacity!=0) || destCapacity<0
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *pDestLimit = pDest + destCapacity;

    যদি(srcLength==-1) {
        /* Convert NUL-terminated ASCII, then find the string length. */
        যেতিয়ালৈ((ch=*src)<=0x7f && ch != 0 && pDest<pDestLimit) {
            *pDest++ = (uint8_t)ch;
            ++src;
        }
        যদি(ch == 0) {
            reqLength=(int32_t)(pDest - (uint8_t *)dest);
            যদি(pDestLength) {
                *pDestLength = reqLength;
            }

            /* Terminate the buffer */
            u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
            ঘূৰাই dest;
        }
        srcLength = u_strlen(src);
    }

    /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
    pSrcLimit = (src!=nullptr)?(src+srcLength):nullptr;
    বাবে(;;) {
        count = (int32_t)(pDestLimit - pDest);
        srcLength = (int32_t)(pSrcLimit - src);
        যদি(count >= srcLength && srcLength > 0 && *src <= 0x7f) {
            /* fast ASCII loop */
            ধ্রুৱ char16_t *prevSrc = src;
            int32_t delta;
            যেতিয়ালৈ(src < pSrcLimit && (ch = *src) <= 0x7f && ch != 0) {
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
        যদি(count > srcLength) {
            count = srcLength; /* min(remaining dest/3, remaining src) */
        }
        যদি(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            ভঙ্গ;
        }
        কৰা {
            ch=*src++;
            যদি(ch <= 0x7f && ch != 0) {
                *pDest++ = (uint8_t)ch;
            } অন্যথা যদি(ch <= 0x7ff) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } অন্যথা {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            }
        } যেতিয়ালৈ(--count > 0);
    }

    যেতিয়ালৈ(src<pSrcLimit) {
        ch=*src++;
        যদি(ch <= 0x7f && ch != 0) {
            যদি(pDest<pDestLimit) {
                *pDest++ = (uint8_t)ch;
            } অন্যথা {
                reqLength = 1;
                ভঙ্গ;
            }
        } অন্যথা যদি(ch <= 0x7ff) {
            যদি((pDestLimit - pDest) >= 2) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } অন্যথা {
                reqLength = 2;
                ভঙ্গ;
            }
        } অন্যথা {
            যদি((pDestLimit - pDest) >= 3) {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } অন্যথা {
                reqLength = 3;
                ভঙ্গ;
            }
        }
    }
    যেতিয়ালৈ(src<pSrcLimit) {
        ch=*src++;
        যদি(ch <= 0x7f && ch != 0) {
            ++reqLength;
        } অন্যথা যদি(ch<=0x7ff) {
            reqLength+=2;
        } অন্যথা {
            reqLength+=3;
        }
    }

    reqLength+=(int32_t)(pDest - (uint8_t *)dest);
    যদি(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
    ঘূৰাই dest;
}
