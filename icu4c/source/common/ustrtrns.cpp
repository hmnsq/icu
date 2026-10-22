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


#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/utf.h"
#அந்தர்பாவ "unicode/utf8.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "ustr_imp.h"
#அந்தர்பாவ "uassert.h"

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32WithSub(char16_t* dest U_LIFETIME_BOUND,
                      int32_t destCapacity,
                      int32_t* pDestLength,
                      மாறிலி UChar32* src,
                      int32_t srcLength,
                      UChar32 subchar,
                      int32_t* p_இலக்கம்_Substitutions,
                      UErrorCode* pErrorCode) {
    மாறிலி UChar32 *srcLimit;
    UChar32 ch;
    char16_t *destLimit;
    char16_t *pDest;
    int32_t reqLength;
    int32_t இலக்கம்_Substitutions;

    /* args check */
    எனில்(U_FAILURE(*pErrorCode)){
        திரும்பு nullptr;
    }
    எனில்( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    எனில்(p_இலக்கம்_Substitutions != nullptr) {
        *p_இலக்கம்_Substitutions = 0;
    }

    pDest = dest;
    destLimit = (dest!=nullptr)?(dest + destCapacity):nullptr;
    reqLength = 0;
    இலக்கம்_Substitutions = 0;

    எனில்(srcLength < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        போது((ch=*src) != 0 &&
              ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff))) {
            ++src;
            எனில்(pDest < destLimit) {
                *pDest++ = (char16_t)ch;
            } இல்லையெனில் {
                ++reqLength;
            }
        }
        srcLimit = src;
        எனில்(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            போது(*++srcLimit != 0) {}
        }
    } இல்லையெனில் {
      srcLimit = (src!=nullptr)?(src + srcLength):nullptr;
    }

    /* convert with length */
    போது(src < srcLimit) {
        ch = *src++;
        செய் {
            /* usually "loops" once; twice only for writing subchar */
            எனில்((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff)) {
                எனில்(pDest < destLimit) {
                    *pDest++ = (char16_t)ch;
                } இல்லையெனில் {
                    ++reqLength;
                }
                நிறுத்து;
            } இல்லையெனில் எனில்(0x10000 <= ch && ch <= 0x10ffff) {
                எனில்(pDest!=nullptr && ((pDest + 2) <= destLimit)) {
                    *pDest++ = U16_LEAD(ch);
                    *pDest++ = U16_TRAIL(ch);
                } இல்லையெனில் {
                    reqLength += 2;
                }
                நிறுத்து;
            } இல்லையெனில் எனில்((ch = subchar) < 0) {
                /* surrogate code point, or not a Unicode code point at all */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                திரும்பு nullptr;
            } இல்லையெனில் {
                ++இலக்கம்_Substitutions;
            }
        } போது(உண்மை);
    }

    reqLength += (int32_t)(pDest - dest);
    எனில்(pDestLength) {
        *pDestLength = reqLength;
    }
    எனில்(p_இலக்கம்_Substitutions != nullptr) {
        *p_இலக்கம்_Substitutions = இலக்கம்_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
    
    திரும்பு dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32(char16_t* dest U_LIFETIME_BOUND,
               int32_t destCapacity,
               int32_t* pDestLength,
               மாறிலி UChar32* src,
               int32_t srcLength,
               UErrorCode* pErrorCode) {
    திரும்பு u_strFromUTF32WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI UChar32* U_EXPORT2
u_strToUTF32WithSub(UChar32* dest U_LIFETIME_BOUND,
                    int32_t destCapacity,
                    int32_t* pDestLength,
                    மாறிலி char16_t* src,
                    int32_t srcLength,
                    UChar32 subchar,
                    int32_t* p_இலக்கம்_Substitutions,
                    UErrorCode* pErrorCode) {
    மாறிலி char16_t *srcLimit;
    UChar32 ch;
    char16_t ch2;
    UChar32 *destLimit;
    UChar32 *pDest;
    int32_t reqLength;
    int32_t இலக்கம்_Substitutions;

    /* args check */
    எனில்(U_FAILURE(*pErrorCode)){
        திரும்பு nullptr;
    }
    எனில்( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    எனில்(p_இலக்கம்_Substitutions != nullptr) {
        *p_இலக்கம்_Substitutions = 0;
    }

    pDest = dest;
    destLimit = (dest!=nullptr)?(dest + destCapacity):nullptr;
    reqLength = 0;
    இலக்கம்_Substitutions = 0;

    எனில்(srcLength < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        போது((ch=*src) != 0 && !U16_IS_SURROGATE(ch)) {
            ++src;
            எனில்(pDest < destLimit) {
                *pDest++ = ch;
            } இல்லையெனில் {
                ++reqLength;
            }
        }
        srcLimit = src;
        எனில்(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            போது(*++srcLimit != 0) {}
        }
    } இல்லையெனில் {
        srcLimit = (src!=nullptr)?(src + srcLength):nullptr;
    }

    /* convert with length */
    போது(src < srcLimit) {
        ch = *src++;
        எனில்(!U16_IS_SURROGATE(ch)) {
            /* write or count ch below */
        } இல்லையெனில் எனில்(U16_IS_SURROGATE_LEAD(ch) && src < srcLimit && U16_IS_TRAIL(ch2 = *src)) {
            ++src;
            ch = U16_GET_SUPPLEMENTARY(ch, ch2);
        } இல்லையெனில் எனில்((ch = subchar) < 0) {
            /* unpaired surrogate */
            *pErrorCode = U_INVALID_CHAR_FOUND;
            திரும்பு nullptr;
        } இல்லையெனில் {
            ++இலக்கம்_Substitutions;
        }
        எனில்(pDest < destLimit) {
            *pDest++ = ch;
        } இல்லையெனில் {
            ++reqLength;
        }
    }

    reqLength += (int32_t)(pDest - dest);
    எனில்(pDestLength) {
        *pDestLength = reqLength;
    }
    எனில்(p_இலக்கம்_Substitutions != nullptr) {
        *p_இலக்கம்_Substitutions = இலக்கம்_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChar32s(dest, destCapacity, reqLength, pErrorCode);

    திரும்பு dest;
}

U_CAPI UChar32* U_EXPORT2
u_strToUTF32(UChar32* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* pDestLength,
             மாறிலி char16_t* src,
             int32_t srcLength,
             UErrorCode* pErrorCode) {
    திரும்பு u_strToUTF32WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                     int32_t destCapacity,
                     int32_t* pDestLength,
                     மாறிலி எழுத்து* src,
                     int32_t srcLength,
                     UChar32 subchar,
                     int32_t* p_இலக்கம்_Substitutions,
                     UErrorCode* pErrorCode) {
    /* args check */
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }
    எனில்( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    எனில்(p_இலக்கம்_Substitutions!=nullptr) {
        *p_இலக்கம்_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *pDestLimit = dest+destCapacity;
    int32_t reqLength = 0;
    int32_t இலக்கம்_Substitutions=0;

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

    எனில்(srcLength < 0){
        /*
         * Transform a NUL-terminated string.
         * The code explicitly checks for NULs only in the lead byte position.
         * A NUL byte in the trail byte position fails the trail byte range check anyway.
         */
        int32_t i;
        UChar32 c;
        க்கு(i = 0; (c = (uint8_t)src[i]) != 0 && (pDest < pDestLimit);) {
            // modified copy of U8_NEXT()
            ++i;
            எனில்(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } இல்லையெனில் {
                uint8_t __t1, __t2;
                எனில்( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } இல்லையெனில் எனில்( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } இல்லையெனில் {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((மாறிலி uint8_t *)src, &(i), -1, c, -1);
                    எனில்(c<0 && (++இலக்கம்_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        திரும்பு nullptr;
                    } இல்லையெனில் எனில்(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } இல்லையெனில் {
                        *(pDest++)=U16_LEAD(c);
                        எனில்(pDest<pDestLimit) {
                            *(pDest++)=U16_TRAIL(c);
                        } இல்லையெனில் {
                            reqLength++;
                            நிறுத்து;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        போது((c = (uint8_t)src[i]) != 0) {
            // modified copy of U8_NEXT()
            ++i;
            எனில்(U8_IS_SINGLE(c)) {
                ++reqLength;
            } இல்லையெனில் {
                uint8_t __t1, __t2;
                எனில்( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++reqLength;
                    i+=2;
                } இல்லையெனில் எனில்( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++reqLength;
                    ++(i);
                } இல்லையெனில் {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((மாறிலி uint8_t *)src, &(i), -1, c, -1);
                    எனில்(c<0 && (++இலக்கம்_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        திரும்பு nullptr;
                    }
                    reqLength += U16_LENGTH(c);
                }
            }
        }
    } இல்லையெனில் /* srcLength >= 0 */ {
        /* Faster loop without ongoing checking for srcLength and pDestLimit. */
        int32_t i = 0;
        UChar32 c;
        க்கு(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            int32_t count = (int32_t)(pDestLimit - pDest);
            int32_t count2 = (srcLength - i) / 3;
            எனில்(count > count2) {
                count = count2; /* min(remaining dest, remaining src/3) */
            }
            எனில்(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                நிறுத்து;
            }

            செய் {
                // modified copy of U8_NEXT()
                c = (uint8_t)src[i++];
                எனில்(U8_IS_SINGLE(c)) {
                    *pDest++=(char16_t)c;
                } இல்லையெனில் {
                    uint8_t __t1, __t2;
                    எனில்( /* handle U+0800..U+FFFF inline */
                            (0xe0<=(c) && (c)<0xf0) &&
                            ((i)+1)<srcLength &&
                            U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                            (__t2=src[(i)+1]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                        i+=2;
                    } இல்லையெனில் எனில்( /* handle U+0080..U+07FF inline */
                            ((c)<0xe0 && (c)>=0xc2) &&
                            ((i)!=srcLength) &&
                            (__t1=src[i]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0x1f)<<6)|__t1;
                        ++(i);
                    } இல்லையெனில் {
                        எனில்(c >= 0xf0 || subchar > 0xffff) {
                            // We may read up to four bytes and write up to two UChars,
                            // which we didn't account for with computing count,
                            // so we adjust it here.
                            எனில்(--count == 0) {
                                --i;  // back out byte c
                                நிறுத்து;
                            }
                        }

                        /* function call for "complicated" and error cases */
                        (c)=utf8_nextCharSafeBody((மாறிலி uint8_t *)src, &(i), srcLength, c, -1);
                        எனில்(c<0 && (++இலக்கம்_Substitutions, c = subchar) < 0) {
                            *pErrorCode = U_INVALID_CHAR_FOUND;
                            திரும்பு nullptr;
                        } இல்லையெனில் எனில்(c<=0xFFFF) {
                            *(pDest++)=(char16_t)c;
                        } இல்லையெனில் {
                            *(pDest++)=U16_LEAD(c);
                            *(pDest++)=U16_TRAIL(c);
                        }
                    }
                }
            } போது(--count > 0);
        }

        போது(i < srcLength && (pDest < pDestLimit)) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            எனில்(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } இல்லையெனில் {
                uint8_t __t1, __t2;
                எனில்( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<srcLength &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } இல்லையெனில் எனில்( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=srcLength) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } இல்லையெனில் {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((மாறிலி uint8_t *)src, &(i), srcLength, c, -1);
                    எனில்(c<0 && (++இலக்கம்_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        திரும்பு nullptr;
                    } இல்லையெனில் எனில்(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } இல்லையெனில் {
                        *(pDest++)=U16_LEAD(c);
                        எனில்(pDest<pDestLimit) {
                            *(pDest++)=U16_TRAIL(c);
                        } இல்லையெனில் {
                            reqLength++;
                            நிறுத்து;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        போது(i < srcLength) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            எனில்(U8_IS_SINGLE(c)) {
                ++reqLength;
            } இல்லையெனில் {
                uint8_t __t1, __t2;
                எனில்( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<srcLength &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++reqLength;
                    i+=2;
                } இல்லையெனில் எனில்( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=srcLength) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++reqLength;
                    ++(i);
                } இல்லையெனில் {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_nextCharSafeBody((மாறிலி uint8_t *)src, &(i), srcLength, c, -1);
                    எனில்(c<0 && (++இலக்கம்_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        திரும்பு nullptr;
                    }
                    reqLength += U16_LENGTH(c);
                }
            }
        }
    }

    reqLength+=(int32_t)(pDest - dest);

    எனில்(p_இலக்கம்_Substitutions!=nullptr) {
        *p_இலக்கம்_Substitutions=இலக்கம்_Substitutions;
    }

    எனில்(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,reqLength,pErrorCode);

    திரும்பு dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8(char16_t* dest U_LIFETIME_BOUND,
              int32_t destCapacity,
              int32_t* pDestLength,
              மாறிலி எழுத்து* src,
              int32_t srcLength,
              UErrorCode* pErrorCode) {
    திரும்பு u_strFromUTF8WithSub(
            dest, destCapacity, pDestLength,
            src, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8Lenient(char16_t* dest U_LIFETIME_BOUND,
                     int32_t destCapacity,
                     int32_t* pDestLength,
                     மாறிலி எழுத்து* src,
                     int32_t srcLength,
                     UErrorCode* pErrorCode) {
    char16_t *pDest = dest;
    UChar32 ch;
    int32_t reqLength = 0;
    uint8_t* pSrc = (uint8_t*) src;

    /* args check */
    எனில்(U_FAILURE(*pErrorCode)){
        திரும்பு nullptr;
    }
        
    எனில்( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    எனில்(srcLength < 0) {
        /* Transform a NUL-terminated string. */
        char16_t *pDestLimit = (dest!=nullptr)?(dest+destCapacity):nullptr;
        uint8_t t1, t2, t3; /* trail bytes */

        போது(((ch = *pSrc) != 0) && (pDest < pDestLimit)) {
            எனில்(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ++pSrc;
                தொடர்;
            } இல்லையெனில் எனில்(ch < 0xe0) { /* U+0080..U+07FF */
                எனில்((t1 = pSrc[1]) != 0) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + t1 - 0x3080);
                    pSrc += 2;
                    தொடர்;
                }
            } இல்லையெனில் எனில்(ch < 0xf0) { /* U+0800..U+FFFF */
                எனில்((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 12) + (t1 << 6) + t2 - 0x2080);
                    pSrc += 3;
                    தொடர்;
                }
            } இல்லையெனில் /* f0..f4 */ { /* U+10000..U+10FFFF */
                எனில்((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0 && (t3 = pSrc[3]) != 0) {
                    pSrc += 4;
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (t1 << 12) + (t2 << 6) + t3 - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    எனில்(pDest < pDestLimit) {
                        *(pDest++) = U16_TRAIL(ch);
                    } இல்லையெனில் {
                        reqLength = 1;
                        நிறுத்து;
                    }
                    தொடர்;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            போது(*++pSrc != 0) {}
            நிறுத்து;
        }

        /* Pre-flight the rest of the string. */
        போது((ch = *pSrc) != 0) {
            எனில்(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                ++reqLength;
                ++pSrc;
                தொடர்;
            } இல்லையெனில் எனில்(ch < 0xe0) { /* U+0080..U+07FF */
                எனில்(pSrc[1] != 0) {
                    ++reqLength;
                    pSrc += 2;
                    தொடர்;
                }
            } இல்லையெனில் எனில்(ch < 0xf0) { /* U+0800..U+FFFF */
                எனில்(pSrc[1] != 0 && pSrc[2] != 0) {
                    ++reqLength;
                    pSrc += 3;
                    தொடர்;
                }
            } இல்லையெனில் /* f0..f4 */ { /* U+10000..U+10FFFF */
                எனில்(pSrc[1] != 0 && pSrc[2] != 0 && pSrc[3] != 0) {
                    reqLength += 2;
                    pSrc += 4;
                    தொடர்;
                }
            }

            /* truncated character at the end */
            ++reqLength;
            நிறுத்து;
        }
    } இல்லையெனில் /* srcLength >= 0 */ {
      மாறிலி uint8_t *pSrcLimit = (pSrc!=nullptr)?(pSrc + srcLength):nullptr;

        /*
         * This function requires that if srcLength is given, then it must be
         * destCapatity >= srcLength so that we need not check for
         * destination buffer overflow in the loop.
         */
        எனில்(destCapacity < srcLength) {
            எனில்(pDestLength != nullptr) {
                *pDestLength = srcLength; /* this likely overestimates the true destLength! */
            }
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            திரும்பு nullptr;
        }

        எனில்((pSrcLimit - pSrc) >= 4) {
            pSrcLimit -= 3; /* temporarily reduce pSrcLimit */

            /* in this loop, we can always access at least 4 bytes, up to pSrc+3 */
            செய் {
                ch = *pSrc++;
                எனில்(ch < 0xc0) {
                    /*
                     * ASCII, or a trail byte in lead position which is treated like
                     * a single-byte sequence for better character boundary
                     * resynchronization after illegal sequences.
                     */
                    *pDest++=(char16_t)ch;
                } இல்லையெனில் எனில்(ch < 0xe0) { /* U+0080..U+07FF */
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                } இல்லையெனில் எனில்(ch < 0xf0) { /* U+0800..U+FFFF */
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                } இல்லையெனில் /* f0..f4 */ { /* U+10000..U+10FFFF */
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                }
            } போது(pSrc < pSrcLimit);

            pSrcLimit += 3; /* restore original pSrcLimit */
        }

        போது(pSrc < pSrcLimit) {
            ch = *pSrc++;
            எனில்(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                தொடர்;
            } இல்லையெனில் எனில்(ch < 0xe0) { /* U+0080..U+07FF */
                எனில்(pSrc < pSrcLimit) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                    தொடர்;
                }
            } இல்லையெனில் எனில்(ch < 0xf0) { /* U+0800..U+FFFF */
                எனில்((pSrcLimit - pSrc) >= 2) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                    pSrc += 3;
                    தொடர்;
                }
            } இல்லையெனில் /* f0..f4 */ { /* U+10000..U+10FFFF */
                எனில்((pSrcLimit - pSrc) >= 3) {
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                    pSrc += 4;
                    தொடர்;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            நிறுத்து;
        }
    }

    reqLength+=(int32_t)(pDest - dest);

    எனில்(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,reqLength,pErrorCode);

    திரும்பு dest;
}

நிலையான உள்வரி uint8_t *
_appendUTF8(uint8_t *pDest, UChar32 c) {
    /* it is 0<=c<=0x10ffff and not a surrogate if called by a validating function */
    எனில்((c)<=0x7f) {
        *pDest++ = static_cast<uint8_t>(c);
    } இல்லையெனில் எனில்(c<=0x7ff) {
        *pDest++ = static_cast<uint8_t>((c >> 6) | 0xc0);
        *pDest++ = static_cast<uint8_t>((c & 0x3f) | 0x80);
    } இல்லையெனில் எனில்(c<=0xffff) {
        *pDest++ = static_cast<uint8_t>((c >> 12) | 0xe0);
        *pDest++ = static_cast<uint8_t>(((c >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    } இல்லையெனில் /* if((uint32_t)(c)<=0x10ffff) */ {
        *pDest++ = static_cast<uint8_t>(((c) >> 18) | 0xf0);
        *pDest++ = static_cast<uint8_t>((((c) >> 12) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>((((c) >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    }
    திரும்பு pDest;
}

U_CAPI எழுத்து* U_EXPORT2
u_strToUTF8WithSub(எழுத்து* dest U_LIFETIME_BOUND,
                   int32_t destCapacity,
                   int32_t* pDestLength,
                   மாறிலி char16_t* pSrc,
                   int32_t srcLength,
                   UChar32 subchar,
                   int32_t* p_இலக்கம்_Substitutions,
                   UErrorCode* pErrorCode) {
    int32_t reqLength=0;
    uint32_t ch=0,ch2=0;
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *pDestLimit = (pDest!=nullptr)?(pDest + destCapacity):nullptr;
    int32_t இலக்கம்_Substitutions;

    /* args check */
    எனில்(U_FAILURE(*pErrorCode)){
        திரும்பு nullptr;
    }
        
    எனில்( (pSrc==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    எனில்(p_இலக்கம்_Substitutions!=nullptr) {
        *p_இலக்கம்_Substitutions=0;
    }
    இலக்கம்_Substitutions=0;

    எனில்(srcLength==-1) {
        போது((ch=*pSrc)!=0) {
            ++pSrc;
            எனில்(ch <= 0x7f) {
                எனில்(pDest<pDestLimit) {
                    *pDest++ = (uint8_t)ch;
                } இல்லையெனில் {
                    reqLength = 1;
                    நிறுத்து;
                }
            } இல்லையெனில் எனில்(ch <= 0x7ff) {
                எனில்((pDestLimit - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } இல்லையெனில் {
                    reqLength = 2;
                    நிறுத்து;
                }
            } இல்லையெனில் எனில்(ch <= 0xd7ff || ch >= 0xe000) {
                எனில்((pDestLimit - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } இல்லையெனில் {
                    reqLength = 3;
                    நிறுத்து;
                }
            } இல்லையெனில் /* ch is a surrogate */ {
                int32_t length;

                /*need not check for NUL because NUL fails U16_IS_TRAIL() anyway*/
                எனில்(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } இல்லையெனில் எனில்(subchar>=0) {
                    ch=subchar;
                    ++இலக்கம்_Substitutions;
                } இல்லையெனில் {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    திரும்பு nullptr;
                }

                length = U8_LENGTH(ch);
                எனில்((pDestLimit - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } இல்லையெனில் {
                    reqLength = length;
                    நிறுத்து;
                }
            }
        }
        போது((ch=*pSrc++)!=0) {
            எனில்(ch<=0x7f) {
                ++reqLength;
            } இல்லையெனில் எனில்(ch<=0x7ff) {
                reqLength+=2;
            } இல்லையெனில் எனில்(!U16_IS_SURROGATE(ch)) {
                reqLength+=3;
            } இல்லையெனில் எனில்(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                reqLength+=4;
            } இல்லையெனில் எனில்(subchar>=0) {
                reqLength+=U8_LENGTH(subchar);
                ++இலக்கம்_Substitutions;
            } இல்லையெனில் {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                திரும்பு nullptr;
            }
        }
    } இல்லையெனில் {
        மாறிலி char16_t *pSrcLimit = (pSrc!=nullptr)?(pSrc+srcLength):nullptr;
        int32_t count;

        /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
        க்கு(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            count = (int32_t)((pDestLimit - pDest) / 3);
            srcLength = (int32_t)(pSrcLimit - pSrc);
            எனில்(count > srcLength) {
                count = srcLength; /* min(remaining dest/3, remaining src) */
            }
            எனில்(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                நிறுத்து;
            }
            செய் {
                ch=*pSrc++;
                எனில்(ch <= 0x7f) {
                    *pDest++ = (uint8_t)ch;
                } இல்லையெனில் எனில்(ch <= 0x7ff) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } இல்லையெனில் எனில்(ch <= 0xd7ff || ch >= 0xe000) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } இல்லையெனில் /* ch is a surrogate */ {
                    /*
                     * We will read two UChars and probably output four bytes,
                     * which we didn't account for with computing count,
                     * so we adjust it here.
                     */
                    எனில்(--count == 0) {
                        --pSrc; /* undo ch=*pSrc++ for the lead surrogate */
                        நிறுத்து;  /* recompute count */
                    }

                    எனில்(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                        ++pSrc;
                        ch=U16_GET_SUPPLEMENTARY(ch, ch2);

                        /* writing 4 bytes per 2 UChars is ok */
                        *pDest++=(uint8_t)((ch>>18)|0xf0);
                        *pDest++=(uint8_t)(((ch>>12)&0x3f)|0x80);
                        *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                        *pDest++=(uint8_t)((ch&0x3f)|0x80);
                    } இல்லையெனில்  {
                        /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                        எனில்(subchar>=0) {
                            ch=subchar;
                            ++இலக்கம்_Substitutions;
                        } இல்லையெனில் {
                            *pErrorCode = U_INVALID_CHAR_FOUND;
                            திரும்பு nullptr;
                        }

                        /* convert and append*/
                        pDest=_appendUTF8(pDest, ch);
                    }
                }
            } போது(--count > 0);
        }

        போது(pSrc<pSrcLimit) {
            ch=*pSrc++;
            எனில்(ch <= 0x7f) {
                எனில்(pDest<pDestLimit) {
                    *pDest++ = (uint8_t)ch;
                } இல்லையெனில் {
                    reqLength = 1;
                    நிறுத்து;
                }
            } இல்லையெனில் எனில்(ch <= 0x7ff) {
                எனில்((pDestLimit - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } இல்லையெனில் {
                    reqLength = 2;
                    நிறுத்து;
                }
            } இல்லையெனில் எனில்(ch <= 0xd7ff || ch >= 0xe000) {
                எனில்((pDestLimit - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } இல்லையெனில் {
                    reqLength = 3;
                    நிறுத்து;
                }
            } இல்லையெனில் /* ch is a surrogate */ {
                int32_t length;

                எனில்(U16_IS_SURROGATE_LEAD(ch) && pSrc<pSrcLimit && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } இல்லையெனில் எனில்(subchar>=0) {
                    ch=subchar;
                    ++இலக்கம்_Substitutions;
                } இல்லையெனில் {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    திரும்பு nullptr;
                }

                length = U8_LENGTH(ch);
                எனில்((pDestLimit - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } இல்லையெனில் {
                    reqLength = length;
                    நிறுத்து;
                }
            }
        }
        போது(pSrc<pSrcLimit) {
            ch=*pSrc++;
            எனில்(ch<=0x7f) {
                ++reqLength;
            } இல்லையெனில் எனில்(ch<=0x7ff) {
                reqLength+=2;
            } இல்லையெனில் எனில்(!U16_IS_SURROGATE(ch)) {
                reqLength+=3;
            } இல்லையெனில் எனில்(U16_IS_SURROGATE_LEAD(ch) && pSrc<pSrcLimit && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                reqLength+=4;
            } இல்லையெனில் எனில்(subchar>=0) {
                reqLength+=U8_LENGTH(subchar);
                ++இலக்கம்_Substitutions;
            } இல்லையெனில் {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                திரும்பு nullptr;
            }
        }
    }

    reqLength+=(int32_t)(pDest - (uint8_t *)dest);

    எனில்(p_இலக்கம்_Substitutions!=nullptr) {
        *p_இலக்கம்_Substitutions=இலக்கம்_Substitutions;
    }

    எனில்(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
    திரும்பு dest;
}

U_CAPI எழுத்து* U_EXPORT2
u_strToUTF8(எழுத்து* dest U_LIFETIME_BOUND,
            int32_t destCapacity,
            int32_t* pDestLength,
            மாறிலி char16_t* pSrc,
            int32_t srcLength,
            UErrorCode* pErrorCode) {
    திரும்பு u_strToUTF8WithSub(
            dest, destCapacity, pDestLength,
            pSrc, srcLength,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromJavaModifiedUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                                 int32_t destCapacity,
                                 int32_t* pDestLength,
                                 மாறிலி எழுத்து* src,
                                 int32_t srcLength,
                                 UChar32 subchar,
                                 int32_t* p_இலக்கம்_Substitutions,
                                 UErrorCode* pErrorCode) {
    /* args check */
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }
    எனில்( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (dest==nullptr && destCapacity!=0) || destCapacity<0 ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    எனில்(p_இலக்கம்_Substitutions!=nullptr) {
        *p_இலக்கம்_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *pDestLimit = dest+destCapacity;
    int32_t reqLength = 0;
    int32_t இலக்கம்_Substitutions=0;

    எனில்(srcLength < 0) {
        /*
         * Transform a NUL-terminated ASCII string.
         * Handle non-ASCII strings with slower code.
         */
        UChar32 c;
        போது(((c = (uint8_t)*src) != 0) && c <= 0x7f && (pDest < pDestLimit)) {
            *pDest++=(char16_t)c;
            ++src;
        }
        எனில்(c == 0) {
            reqLength=(int32_t)(pDest - dest);
            எனில்(pDestLength) {
                *pDestLength = reqLength;
            }

            /* Terminate the buffer */
            u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
            திரும்பு dest;
        }
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    /* Faster loop without ongoing checking for srcLength and pDestLimit. */
    UChar32 ch;
    uint8_t t1, t2;
    int32_t i = 0;
    க்கு(;;) {
        int32_t count = (int32_t)(pDestLimit - pDest);
        int32_t count2 = srcLength - i;
        எனில்(count >= count2 && srcLength > 0 && U8_IS_SINGLE(*src)) {
            /* fast ASCII loop */
            int32_t start = i;
            uint8_t b;
            போது(i < srcLength && U8_IS_SINGLE(b = src[i])) {
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
        எனில்(subchar > 0xFFFF) {
            நிறுத்து;
        }
        count2 /= 3;
        எனில்(count > count2) {
            count = count2; /* min(remaining dest, remaining src/3) */
        }
        எனில்(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            நிறுத்து;
        }
        செய் {
            ch = (uint8_t)src[i++];
            எனில்(U8_IS_SINGLE(ch)) {
                *pDest++=(char16_t)ch;
            } இல்லையெனில் {
                எனில்(ch >= 0xe0) {
                    எனில்( /* handle U+0000..U+FFFF inline */
                        ch <= 0xef &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                        (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                    ) {
                        /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                        *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                        i += 2;
                        தொடர்;
                    }
                } இல்லையெனில் {
                    எனில்( /* handle U+0000..U+07FF inline */
                        ch >= 0xc0 &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                    ) {
                        *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                        ++i;
                        தொடர்;
                    }
                }

                எனில்(subchar < 0) {
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    திரும்பு nullptr;
                } இல்லையெனில் எனில்(subchar > 0xffff && --count == 0) {
                    /*
                     * We need to write two UChars, adjusted count for that,
                     * and ran out of space.
                     */
                    --i;  // back out byte ch
                    நிறுத்து;
                } இல்லையெனில் {
                    /* function call for error cases */
                    utf8_nextCharSafeBody((மாறிலி uint8_t *)src, &(i), srcLength, ch, -1);
                    ++இலக்கம்_Substitutions;
                    *(pDest++)=(char16_t)subchar;
                }
            }
        } போது(--count > 0);
    }

    போது(i < srcLength && (pDest < pDestLimit)) {
        ch = (uint8_t)src[i++];
        எனில்(U8_IS_SINGLE(ch)){
            *pDest++=(char16_t)ch;
        } இல்லையெனில் {
            எனில்(ch >= 0xe0) {
                எனில்( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < srcLength &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                    (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                ) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                    i += 2;
                    தொடர்;
                }
            } இல்லையெனில் {
                எனில்( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < srcLength &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                ) {
                    *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                    ++i;
                    தொடர்;
                }
            }

            எனில்(subchar < 0) {
                *pErrorCode = U_INVALID_CHAR_FOUND;
                திரும்பு nullptr;
            } இல்லையெனில் {
                /* function call for error cases */
                utf8_nextCharSafeBody((மாறிலி uint8_t *)src, &(i), srcLength, ch, -1);
                ++இலக்கம்_Substitutions;
                எனில்(subchar<=0xFFFF) {
                    *(pDest++)=(char16_t)subchar;
                } இல்லையெனில் {
                    *(pDest++)=U16_LEAD(subchar);
                    எனில்(pDest<pDestLimit) {
                        *(pDest++)=U16_TRAIL(subchar);
                    } இல்லையெனில் {
                        reqLength++;
                        நிறுத்து;
                    }
                }
            }
        }
    }

    /* Pre-flight the rest of the string. */
    போது(i < srcLength) {
        ch = (uint8_t)src[i++];
        எனில்(U8_IS_SINGLE(ch)) {
            reqLength++;
        } இல்லையெனில் {
            எனில்(ch >= 0xe0) {
                எனில்( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < srcLength &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f &&
                    (uint8_t)(src[i+1] - 0x80) <= 0x3f
                ) {
                    reqLength++;
                    i += 2;
                    தொடர்;
                }
            } இல்லையெனில் {
                எனில்( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < srcLength &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f
                ) {
                    reqLength++;
                    ++i;
                    தொடர்;
                }
            }

            எனில்(subchar < 0) {
                *pErrorCode = U_INVALID_CHAR_FOUND;
                திரும்பு nullptr;
            } இல்லையெனில் {
                /* function call for error cases */
                utf8_nextCharSafeBody((மாறிலி uint8_t *)src, &(i), srcLength, ch, -1);
                ++இலக்கம்_Substitutions;
                reqLength+=U16_LENGTH(ch);
            }
        }
    }

    எனில்(p_இலக்கம்_Substitutions!=nullptr) {
        *p_இலக்கம்_Substitutions=இலக்கம்_Substitutions;
    }

    reqLength+=(int32_t)(pDest - dest);
    எனில்(pDestLength) {
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, destCapacity, reqLength, pErrorCode);
    திரும்பு dest;
}

U_CAPI எழுத்து* U_EXPORT2
u_strToJavaModifiedUTF8(எழுத்து* dest U_LIFETIME_BOUND,
                        int32_t destCapacity,
                        int32_t* pDestLength,
                        மாறிலி char16_t* src,
                        int32_t srcLength,
                        UErrorCode* pErrorCode) {
    int32_t reqLength=0;
    uint32_t ch=0;
    மாறிலி char16_t *pSrcLimit;
    int32_t count;

    /* args check */
    எனில்(U_FAILURE(*pErrorCode)){
        திரும்பு nullptr;
    }
    எனில்( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (dest==nullptr && destCapacity!=0) || destCapacity<0
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *pDestLimit = pDest + destCapacity;

    எனில்(srcLength==-1) {
        /* Convert NUL-terminated ASCII, then find the string length. */
        போது((ch=*src)<=0x7f && ch != 0 && pDest<pDestLimit) {
            *pDest++ = (uint8_t)ch;
            ++src;
        }
        எனில்(ch == 0) {
            reqLength=(int32_t)(pDest - (uint8_t *)dest);
            எனில்(pDestLength) {
                *pDestLength = reqLength;
            }

            /* Terminate the buffer */
            u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
            திரும்பு dest;
        }
        srcLength = u_strlen(src);
    }

    /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
    pSrcLimit = (src!=nullptr)?(src+srcLength):nullptr;
    க்கு(;;) {
        count = (int32_t)(pDestLimit - pDest);
        srcLength = (int32_t)(pSrcLimit - src);
        எனில்(count >= srcLength && srcLength > 0 && *src <= 0x7f) {
            /* fast ASCII loop */
            மாறிலி char16_t *prevSrc = src;
            int32_t delta;
            போது(src < pSrcLimit && (ch = *src) <= 0x7f && ch != 0) {
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
        எனில்(count > srcLength) {
            count = srcLength; /* min(remaining dest/3, remaining src) */
        }
        எனில்(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            நிறுத்து;
        }
        செய் {
            ch=*src++;
            எனில்(ch <= 0x7f && ch != 0) {
                *pDest++ = (uint8_t)ch;
            } இல்லையெனில் எனில்(ch <= 0x7ff) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } இல்லையெனில் {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            }
        } போது(--count > 0);
    }

    போது(src<pSrcLimit) {
        ch=*src++;
        எனில்(ch <= 0x7f && ch != 0) {
            எனில்(pDest<pDestLimit) {
                *pDest++ = (uint8_t)ch;
            } இல்லையெனில் {
                reqLength = 1;
                நிறுத்து;
            }
        } இல்லையெனில் எனில்(ch <= 0x7ff) {
            எனில்((pDestLimit - pDest) >= 2) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } இல்லையெனில் {
                reqLength = 2;
                நிறுத்து;
            }
        } இல்லையெனில் {
            எனில்((pDestLimit - pDest) >= 3) {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } இல்லையெனில் {
                reqLength = 3;
                நிறுத்து;
            }
        }
    }
    போது(src<pSrcLimit) {
        ch=*src++;
        எனில்(ch <= 0x7f && ch != 0) {
            ++reqLength;
        } இல்லையெனில் எனில்(ch<=0x7ff) {
            reqLength+=2;
        } இல்லையெனில் {
            reqLength+=3;
        }
    }

    reqLength+=(int32_t)(pDest - (uint8_t *)dest);
    எனில்(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, destCapacity, reqLength, pErrorCode);
    திரும்பு dest;
}
