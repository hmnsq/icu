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


#অন্তর্ভাব "unicode/putil.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/utf.h"
#অন্তর্ভাব "unicode/utf8.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "uassert.h"

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32WithSub(char16_t* dest U_LIFETIME_BOUND,
                      int32_t destCapacity,
                      int32_t* p_Dest_দৈর্ঘ্য,
                      ধ্রুবক UChar32* src,
                      int32_t src_দৈর্ঘ্য,
                      UChar32 subchar,
                      int32_t* p_অঙ্ক_Substitutions,
                      UErrorCode* pErrorCode) {
    ধ্রুবক UChar32 *srcLimit;
    UChar32 ch;
    char16_t *destLimit;
    char16_t *pDest;
    int32_t req_দৈর্ঘ্য;
    int32_t অঙ্ক_Substitutions;

    /* args check */
    যদি(U_FAILURE(*pErrorCode)){
        ফেরত nullptr;
    }
    যদি( (src==nullptr && src_দৈর্ঘ্য!=0) || src_দৈর্ঘ্য < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    যদি(p_অঙ্ক_Substitutions != nullptr) {
        *p_অঙ্ক_Substitutions = 0;
    }

    pDest = dest;
    destLimit = (dest!=nullptr)?(dest + destCapacity):nullptr;
    req_দৈর্ঘ্য = 0;
    অঙ্ক_Substitutions = 0;

    যদি(src_দৈর্ঘ্য < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        যখন((ch=*src) != 0 &&
              ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff))) {
            ++src;
            যদি(pDest < destLimit) {
                *pDest++ = (char16_t)ch;
            } অন্যথা {
                ++req_দৈর্ঘ্য;
            }
        }
        srcLimit = src;
        যদি(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            যখন(*++srcLimit != 0) {}
        }
    } অন্যথা {
      srcLimit = (src!=nullptr)?(src + src_দৈর্ঘ্য):nullptr;
    }

    /* convert with length */
    যখন(src < srcLimit) {
        ch = *src++;
        করো {
            /* usually "loops" once; twice only for writing subchar */
            যদি((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff)) {
                যদি(pDest < destLimit) {
                    *pDest++ = (char16_t)ch;
                } অন্যথা {
                    ++req_দৈর্ঘ্য;
                }
                বিরতি;
            } অন্যথা যদি(0x10000 <= ch && ch <= 0x10ffff) {
                যদি(pDest!=nullptr && ((pDest + 2) <= destLimit)) {
                    *pDest++ = U16_LEAD(ch);
                    *pDest++ = U16_TRAIL(ch);
                } অন্যথা {
                    req_দৈর্ঘ্য += 2;
                }
                বিরতি;
            } অন্যথা যদি((ch = subchar) < 0) {
                /* surrogate code point, or not a Unicode code point at all */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ফেরত nullptr;
            } অন্যথা {
                ++অঙ্ক_Substitutions;
            }
        } যখন(সত্য);
    }

    req_দৈর্ঘ্য += (int32_t)(pDest - dest);
    যদি(p_Dest_দৈর্ঘ্য) {
        *p_Dest_দৈর্ঘ্য = req_দৈর্ঘ্য;
    }
    যদি(p_অঙ্ক_Substitutions != nullptr) {
        *p_অঙ্ক_Substitutions = অঙ্ক_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, destCapacity, req_দৈর্ঘ্য, pErrorCode);
    
    ফেরত dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32(char16_t* dest U_LIFETIME_BOUND,
               int32_t destCapacity,
               int32_t* p_Dest_দৈর্ঘ্য,
               ধ্রুবক UChar32* src,
               int32_t src_দৈর্ঘ্য,
               UErrorCode* pErrorCode) {
    ফেরত u_strFromUTF32WithSub(
            dest, destCapacity, p_Dest_দৈর্ঘ্য,
            src, src_দৈর্ঘ্য,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI UChar32* U_EXPORT2
u_strToUTF32WithSub(UChar32* dest U_LIFETIME_BOUND,
                    int32_t destCapacity,
                    int32_t* p_Dest_দৈর্ঘ্য,
                    ধ্রুবক char16_t* src,
                    int32_t src_দৈর্ঘ্য,
                    UChar32 subchar,
                    int32_t* p_অঙ্ক_Substitutions,
                    UErrorCode* pErrorCode) {
    ধ্রুবক char16_t *srcLimit;
    UChar32 ch;
    char16_t ch2;
    UChar32 *destLimit;
    UChar32 *pDest;
    int32_t req_দৈর্ঘ্য;
    int32_t অঙ্ক_Substitutions;

    /* args check */
    যদি(U_FAILURE(*pErrorCode)){
        ফেরত nullptr;
    }
    যদি( (src==nullptr && src_দৈর্ঘ্য!=0) || src_দৈর্ঘ্য < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    যদি(p_অঙ্ক_Substitutions != nullptr) {
        *p_অঙ্ক_Substitutions = 0;
    }

    pDest = dest;
    destLimit = (dest!=nullptr)?(dest + destCapacity):nullptr;
    req_দৈর্ঘ্য = 0;
    অঙ্ক_Substitutions = 0;

    যদি(src_দৈর্ঘ্য < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        যখন((ch=*src) != 0 && !U16_IS_SURROGATE(ch)) {
            ++src;
            যদি(pDest < destLimit) {
                *pDest++ = ch;
            } অন্যথা {
                ++req_দৈর্ঘ্য;
            }
        }
        srcLimit = src;
        যদি(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            যখন(*++srcLimit != 0) {}
        }
    } অন্যথা {
        srcLimit = (src!=nullptr)?(src + src_দৈর্ঘ্য):nullptr;
    }

    /* convert with length */
    যখন(src < srcLimit) {
        ch = *src++;
        যদি(!U16_IS_SURROGATE(ch)) {
            /* write or count ch below */
        } অন্যথা যদি(U16_IS_SURROGATE_LEAD(ch) && src < srcLimit && U16_IS_TRAIL(ch2 = *src)) {
            ++src;
            ch = U16_GET_SUPPLEMENTARY(ch, ch2);
        } অন্যথা যদি((ch = subchar) < 0) {
            /* unpaired surrogate */
            *pErrorCode = U_INVALID_CHAR_FOUND;
            ফেরত nullptr;
        } অন্যথা {
            ++অঙ্ক_Substitutions;
        }
        যদি(pDest < destLimit) {
            *pDest++ = ch;
        } অন্যথা {
            ++req_দৈর্ঘ্য;
        }
    }

    req_দৈর্ঘ্য += (int32_t)(pDest - dest);
    যদি(p_Dest_দৈর্ঘ্য) {
        *p_Dest_দৈর্ঘ্য = req_দৈর্ঘ্য;
    }
    যদি(p_অঙ্ক_Substitutions != nullptr) {
        *p_অঙ্ক_Substitutions = অঙ্ক_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChar32s(dest, destCapacity, req_দৈর্ঘ্য, pErrorCode);

    ফেরত dest;
}

U_CAPI UChar32* U_EXPORT2
u_strToUTF32(UChar32* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* p_Dest_দৈর্ঘ্য,
             ধ্রুবক char16_t* src,
             int32_t src_দৈর্ঘ্য,
             UErrorCode* pErrorCode) {
    ফেরত u_strToUTF32WithSub(
            dest, destCapacity, p_Dest_দৈর্ঘ্য,
            src, src_দৈর্ঘ্য,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                     int32_t destCapacity,
                     int32_t* p_Dest_দৈর্ঘ্য,
                     ধ্রুবক অক্ষর* src,
                     int32_t src_দৈর্ঘ্য,
                     UChar32 subchar,
                     int32_t* p_অঙ্ক_Substitutions,
                     UErrorCode* pErrorCode) {
    /* args check */
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    যদি( (src==nullptr && src_দৈর্ঘ্য!=0) || src_দৈর্ঘ্য < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    যদি(p_অঙ্ক_Substitutions!=nullptr) {
        *p_অঙ্ক_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *pDestLimit = dest+destCapacity;
    int32_t req_দৈর্ঘ্য = 0;
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

    যদি(src_দৈর্ঘ্য < 0){
        /*
         * Transform a NUL-terminated string.
         * The code explicitly checks for NULs only in the lead byte position.
         * A NUL byte in the trail byte position fails the trail byte range check anyway.
         */
        int32_t i;
        UChar32 c;
        জন্য(i = 0; (c = (uint8_t)src[i]) != 0 && (pDest < pDestLimit);) {
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
                    (c)=utf8_পরবর্তী_Char_Safe_Body((ধ্রুবক uint8_t *)src, &(i), -1, c, -1);
                    যদি(c<0 && (++অঙ্ক_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        ফেরত nullptr;
                    } অন্যথা যদি(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } অন্যথা {
                        *(pDest++)=U16_LEAD(c);
                        যদি(pDest<pDestLimit) {
                            *(pDest++)=U16_TRAIL(c);
                        } অন্যথা {
                            req_দৈর্ঘ্য++;
                            বিরতি;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        যখন((c = (uint8_t)src[i]) != 0) {
            // modified copy of U8_NEXT()
            ++i;
            যদি(U8_IS_SINGLE(c)) {
                ++req_দৈর্ঘ্য;
            } অন্যথা {
                uint8_t __t1, __t2;
                যদি( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++req_দৈর্ঘ্য;
                    i+=2;
                } অন্যথা যদি( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++req_দৈর্ঘ্য;
                    ++(i);
                } অন্যথা {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_পরবর্তী_Char_Safe_Body((ধ্রুবক uint8_t *)src, &(i), -1, c, -1);
                    যদি(c<0 && (++অঙ্ক_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        ফেরত nullptr;
                    }
                    req_দৈর্ঘ্য += U16_দৈর্ঘ্য(c);
                }
            }
        }
    } অন্যথা /* srcLength >= 0 */ {
        /* Faster loop without ongoing checking for srcLength and pDestLimit. */
        int32_t i = 0;
        UChar32 c;
        জন্য(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            int32_t count = (int32_t)(pDestLimit - pDest);
            int32_t count2 = (src_দৈর্ঘ্য - i) / 3;
            যদি(count > count2) {
                count = count2; /* min(remaining dest, remaining src/3) */
            }
            যদি(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                বিরতি;
            }

            করো {
                // modified copy of U8_NEXT()
                c = (uint8_t)src[i++];
                যদি(U8_IS_SINGLE(c)) {
                    *pDest++=(char16_t)c;
                } অন্যথা {
                    uint8_t __t1, __t2;
                    যদি( /* handle U+0800..U+FFFF inline */
                            (0xe0<=(c) && (c)<0xf0) &&
                            ((i)+1)<src_দৈর্ঘ্য &&
                            U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                            (__t2=src[(i)+1]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                        i+=2;
                    } অন্যথা যদি( /* handle U+0080..U+07FF inline */
                            ((c)<0xe0 && (c)>=0xc2) &&
                            ((i)!=src_দৈর্ঘ্য) &&
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
                                বিরতি;
                            }
                        }

                        /* function call for "complicated" and error cases */
                        (c)=utf8_পরবর্তী_Char_Safe_Body((ধ্রুবক uint8_t *)src, &(i), src_দৈর্ঘ্য, c, -1);
                        যদি(c<0 && (++অঙ্ক_Substitutions, c = subchar) < 0) {
                            *pErrorCode = U_INVALID_CHAR_FOUND;
                            ফেরত nullptr;
                        } অন্যথা যদি(c<=0xFFFF) {
                            *(pDest++)=(char16_t)c;
                        } অন্যথা {
                            *(pDest++)=U16_LEAD(c);
                            *(pDest++)=U16_TRAIL(c);
                        }
                    }
                }
            } যখন(--count > 0);
        }

        যখন(i < src_দৈর্ঘ্য && (pDest < pDestLimit)) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            যদি(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } অন্যথা {
                uint8_t __t1, __t2;
                যদি( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<src_দৈর্ঘ্য &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } অন্যথা যদি( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=src_দৈর্ঘ্য) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } অন্যথা {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_পরবর্তী_Char_Safe_Body((ধ্রুবক uint8_t *)src, &(i), src_দৈর্ঘ্য, c, -1);
                    যদি(c<0 && (++অঙ্ক_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        ফেরত nullptr;
                    } অন্যথা যদি(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } অন্যথা {
                        *(pDest++)=U16_LEAD(c);
                        যদি(pDest<pDestLimit) {
                            *(pDest++)=U16_TRAIL(c);
                        } অন্যথা {
                            req_দৈর্ঘ্য++;
                            বিরতি;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        যখন(i < src_দৈর্ঘ্য) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            যদি(U8_IS_SINGLE(c)) {
                ++req_দৈর্ঘ্য;
            } অন্যথা {
                uint8_t __t1, __t2;
                যদি( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<src_দৈর্ঘ্য &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++req_দৈর্ঘ্য;
                    i+=2;
                } অন্যথা যদি( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=src_দৈর্ঘ্য) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++req_দৈর্ঘ্য;
                    ++(i);
                } অন্যথা {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_পরবর্তী_Char_Safe_Body((ধ্রুবক uint8_t *)src, &(i), src_দৈর্ঘ্য, c, -1);
                    যদি(c<0 && (++অঙ্ক_Substitutions, c = subchar) < 0) {
                        *pErrorCode = U_INVALID_CHAR_FOUND;
                        ফেরত nullptr;
                    }
                    req_দৈর্ঘ্য += U16_দৈর্ঘ্য(c);
                }
            }
        }
    }

    req_দৈর্ঘ্য+=(int32_t)(pDest - dest);

    যদি(p_অঙ্ক_Substitutions!=nullptr) {
        *p_অঙ্ক_Substitutions=অঙ্ক_Substitutions;
    }

    যদি(p_Dest_দৈর্ঘ্য){
        *p_Dest_দৈর্ঘ্য = req_দৈর্ঘ্য;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,req_দৈর্ঘ্য,pErrorCode);

    ফেরত dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8(char16_t* dest U_LIFETIME_BOUND,
              int32_t destCapacity,
              int32_t* p_Dest_দৈর্ঘ্য,
              ধ্রুবক অক্ষর* src,
              int32_t src_দৈর্ঘ্য,
              UErrorCode* pErrorCode) {
    ফেরত u_strFromUTF8WithSub(
            dest, destCapacity, p_Dest_দৈর্ঘ্য,
            src, src_দৈর্ঘ্য,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8Lenient(char16_t* dest U_LIFETIME_BOUND,
                     int32_t destCapacity,
                     int32_t* p_Dest_দৈর্ঘ্য,
                     ধ্রুবক অক্ষর* src,
                     int32_t src_দৈর্ঘ্য,
                     UErrorCode* pErrorCode) {
    char16_t *pDest = dest;
    UChar32 ch;
    int32_t req_দৈর্ঘ্য = 0;
    uint8_t* pSrc = (uint8_t*) src;

    /* args check */
    যদি(U_FAILURE(*pErrorCode)){
        ফেরত nullptr;
    }
        
    যদি( (src==nullptr && src_দৈর্ঘ্য!=0) || src_দৈর্ঘ্য < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    যদি(src_দৈর্ঘ্য < 0) {
        /* Transform a NUL-terminated string. */
        char16_t *pDestLimit = (dest!=nullptr)?(dest+destCapacity):nullptr;
        uint8_t t1, t2, t3; /* trail bytes */

        যখন(((ch = *pSrc) != 0) && (pDest < pDestLimit)) {
            যদি(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ++pSrc;
                চালু;
            } অন্যথা যদি(ch < 0xe0) { /* U+0080..U+07FF */
                যদি((t1 = pSrc[1]) != 0) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + t1 - 0x3080);
                    pSrc += 2;
                    চালু;
                }
            } অন্যথা যদি(ch < 0xf0) { /* U+0800..U+FFFF */
                যদি((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 12) + (t1 << 6) + t2 - 0x2080);
                    pSrc += 3;
                    চালু;
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
                        req_দৈর্ঘ্য = 1;
                        বিরতি;
                    }
                    চালু;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            যখন(*++pSrc != 0) {}
            বিরতি;
        }

        /* Pre-flight the rest of the string. */
        যখন((ch = *pSrc) != 0) {
            যদি(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                ++req_দৈর্ঘ্য;
                ++pSrc;
                চালু;
            } অন্যথা যদি(ch < 0xe0) { /* U+0080..U+07FF */
                যদি(pSrc[1] != 0) {
                    ++req_দৈর্ঘ্য;
                    pSrc += 2;
                    চালু;
                }
            } অন্যথা যদি(ch < 0xf0) { /* U+0800..U+FFFF */
                যদি(pSrc[1] != 0 && pSrc[2] != 0) {
                    ++req_দৈর্ঘ্য;
                    pSrc += 3;
                    চালু;
                }
            } অন্যথা /* f0..f4 */ { /* U+10000..U+10FFFF */
                যদি(pSrc[1] != 0 && pSrc[2] != 0 && pSrc[3] != 0) {
                    req_দৈর্ঘ্য += 2;
                    pSrc += 4;
                    চালু;
                }
            }

            /* truncated character at the end */
            ++req_দৈর্ঘ্য;
            বিরতি;
        }
    } অন্যথা /* srcLength >= 0 */ {
      ধ্রুবক uint8_t *pSrcLimit = (pSrc!=nullptr)?(pSrc + src_দৈর্ঘ্য):nullptr;

        /*
         * This function requires that if srcLength is given, then it must be
         * destCapatity >= srcLength so that we need not check for
         * destination buffer overflow in the loop.
         */
        যদি(destCapacity < src_দৈর্ঘ্য) {
            যদি(p_Dest_দৈর্ঘ্য != nullptr) {
                *p_Dest_দৈর্ঘ্য = src_দৈর্ঘ্য; /* this likely overestimates the true destLength! */
            }
            *pErrorCode = U_বাফার_OVERFLOW_ERROR;
            ফেরত nullptr;
        }

        যদি((pSrcLimit - pSrc) >= 4) {
            pSrcLimit -= 3; /* temporarily reduce pSrcLimit */

            /* in this loop, we can always access at least 4 bytes, up to pSrc+3 */
            করো {
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
            } যখন(pSrc < pSrcLimit);

            pSrcLimit += 3; /* restore original pSrcLimit */
        }

        যখন(pSrc < pSrcLimit) {
            ch = *pSrc++;
            যদি(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                চালু;
            } অন্যথা যদি(ch < 0xe0) { /* U+0080..U+07FF */
                যদি(pSrc < pSrcLimit) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                    চালু;
                }
            } অন্যথা যদি(ch < 0xf0) { /* U+0800..U+FFFF */
                যদি((pSrcLimit - pSrc) >= 2) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                    pSrc += 3;
                    চালু;
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
                    চালু;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            বিরতি;
        }
    }

    req_দৈর্ঘ্য+=(int32_t)(pDest - dest);

    যদি(p_Dest_দৈর্ঘ্য){
        *p_Dest_দৈর্ঘ্য = req_দৈর্ঘ্য;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,req_দৈর্ঘ্য,pErrorCode);

    ফেরত dest;
}

স্থির অন্তঃরেখ uint8_t *
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
    ফেরত pDest;
}

U_CAPI অক্ষর* U_EXPORT2
u_strToUTF8WithSub(অক্ষর* dest U_LIFETIME_BOUND,
                   int32_t destCapacity,
                   int32_t* p_Dest_দৈর্ঘ্য,
                   ধ্রুবক char16_t* pSrc,
                   int32_t src_দৈর্ঘ্য,
                   UChar32 subchar,
                   int32_t* p_অঙ্ক_Substitutions,
                   UErrorCode* pErrorCode) {
    int32_t req_দৈর্ঘ্য=0;
    uint32_t ch=0,ch2=0;
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *pDestLimit = (pDest!=nullptr)?(pDest + destCapacity):nullptr;
    int32_t অঙ্ক_Substitutions;

    /* args check */
    যদি(U_FAILURE(*pErrorCode)){
        ফেরত nullptr;
    }
        
    যদি( (pSrc==nullptr && src_দৈর্ঘ্য!=0) || src_দৈর্ঘ্য < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    যদি(p_অঙ্ক_Substitutions!=nullptr) {
        *p_অঙ্ক_Substitutions=0;
    }
    অঙ্ক_Substitutions=0;

    যদি(src_দৈর্ঘ্য==-1) {
        যখন((ch=*pSrc)!=0) {
            ++pSrc;
            যদি(ch <= 0x7f) {
                যদি(pDest<pDestLimit) {
                    *pDest++ = (uint8_t)ch;
                } অন্যথা {
                    req_দৈর্ঘ্য = 1;
                    বিরতি;
                }
            } অন্যথা যদি(ch <= 0x7ff) {
                যদি((pDestLimit - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } অন্যথা {
                    req_দৈর্ঘ্য = 2;
                    বিরতি;
                }
            } অন্যথা যদি(ch <= 0xd7ff || ch >= 0xe000) {
                যদি((pDestLimit - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } অন্যথা {
                    req_দৈর্ঘ্য = 3;
                    বিরতি;
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
                    ফেরত nullptr;
                }

                length = U8_দৈর্ঘ্য(ch);
                যদি((pDestLimit - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } অন্যথা {
                    req_দৈর্ঘ্য = length;
                    বিরতি;
                }
            }
        }
        যখন((ch=*pSrc++)!=0) {
            যদি(ch<=0x7f) {
                ++req_দৈর্ঘ্য;
            } অন্যথা যদি(ch<=0x7ff) {
                req_দৈর্ঘ্য+=2;
            } অন্যথা যদি(!U16_IS_SURROGATE(ch)) {
                req_দৈর্ঘ্য+=3;
            } অন্যথা যদি(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                req_দৈর্ঘ্য+=4;
            } অন্যথা যদি(subchar>=0) {
                req_দৈর্ঘ্য+=U8_দৈর্ঘ্য(subchar);
                ++অঙ্ক_Substitutions;
            } অন্যথা {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ফেরত nullptr;
            }
        }
    } অন্যথা {
        ধ্রুবক char16_t *pSrcLimit = (pSrc!=nullptr)?(pSrc+src_দৈর্ঘ্য):nullptr;
        int32_t count;

        /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
        জন্য(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            count = (int32_t)((pDestLimit - pDest) / 3);
            src_দৈর্ঘ্য = (int32_t)(pSrcLimit - pSrc);
            যদি(count > src_দৈর্ঘ্য) {
                count = src_দৈর্ঘ্য; /* min(remaining dest/3, remaining src) */
            }
            যদি(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                বিরতি;
            }
            করো {
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
                        বিরতি;  /* recompute count */
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
                            ফেরত nullptr;
                        }

                        /* convert and append*/
                        pDest=_appendUTF8(pDest, ch);
                    }
                }
            } যখন(--count > 0);
        }

        যখন(pSrc<pSrcLimit) {
            ch=*pSrc++;
            যদি(ch <= 0x7f) {
                যদি(pDest<pDestLimit) {
                    *pDest++ = (uint8_t)ch;
                } অন্যথা {
                    req_দৈর্ঘ্য = 1;
                    বিরতি;
                }
            } অন্যথা যদি(ch <= 0x7ff) {
                যদি((pDestLimit - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } অন্যথা {
                    req_দৈর্ঘ্য = 2;
                    বিরতি;
                }
            } অন্যথা যদি(ch <= 0xd7ff || ch >= 0xe000) {
                যদি((pDestLimit - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } অন্যথা {
                    req_দৈর্ঘ্য = 3;
                    বিরতি;
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
                    ফেরত nullptr;
                }

                length = U8_দৈর্ঘ্য(ch);
                যদি((pDestLimit - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } অন্যথা {
                    req_দৈর্ঘ্য = length;
                    বিরতি;
                }
            }
        }
        যখন(pSrc<pSrcLimit) {
            ch=*pSrc++;
            যদি(ch<=0x7f) {
                ++req_দৈর্ঘ্য;
            } অন্যথা যদি(ch<=0x7ff) {
                req_দৈর্ঘ্য+=2;
            } অন্যথা যদি(!U16_IS_SURROGATE(ch)) {
                req_দৈর্ঘ্য+=3;
            } অন্যথা যদি(U16_IS_SURROGATE_LEAD(ch) && pSrc<pSrcLimit && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                req_দৈর্ঘ্য+=4;
            } অন্যথা যদি(subchar>=0) {
                req_দৈর্ঘ্য+=U8_দৈর্ঘ্য(subchar);
                ++অঙ্ক_Substitutions;
            } অন্যথা {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ফেরত nullptr;
            }
        }
    }

    req_দৈর্ঘ্য+=(int32_t)(pDest - (uint8_t *)dest);

    যদি(p_অঙ্ক_Substitutions!=nullptr) {
        *p_অঙ্ক_Substitutions=অঙ্ক_Substitutions;
    }

    যদি(p_Dest_দৈর্ঘ্য){
        *p_Dest_দৈর্ঘ্য = req_দৈর্ঘ্য;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, destCapacity, req_দৈর্ঘ্য, pErrorCode);
    ফেরত dest;
}

U_CAPI অক্ষর* U_EXPORT2
u_strToUTF8(অক্ষর* dest U_LIFETIME_BOUND,
            int32_t destCapacity,
            int32_t* p_Dest_দৈর্ঘ্য,
            ধ্রুবক char16_t* pSrc,
            int32_t src_দৈর্ঘ্য,
            UErrorCode* pErrorCode) {
    ফেরত u_strToUTF8WithSub(
            dest, destCapacity, p_Dest_দৈর্ঘ্য,
            pSrc, src_দৈর্ঘ্য,
            U_SENTINEL, nullptr,
            pErrorCode);
}

U_CAPI char16_t* U_EXPORT2
u_strFromJavaModifiedUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                                 int32_t destCapacity,
                                 int32_t* p_Dest_দৈর্ঘ্য,
                                 ধ্রুবক অক্ষর* src,
                                 int32_t src_দৈর্ঘ্য,
                                 UChar32 subchar,
                                 int32_t* p_অঙ্ক_Substitutions,
                                 UErrorCode* pErrorCode) {
    /* args check */
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    যদি( (src==nullptr && src_দৈর্ঘ্য!=0) || src_দৈর্ঘ্য < -1 ||
        (dest==nullptr && destCapacity!=0) || destCapacity<0 ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    যদি(p_অঙ্ক_Substitutions!=nullptr) {
        *p_অঙ্ক_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *pDestLimit = dest+destCapacity;
    int32_t req_দৈর্ঘ্য = 0;
    int32_t অঙ্ক_Substitutions=0;

    যদি(src_দৈর্ঘ্য < 0) {
        /*
         * Transform a NUL-terminated ASCII string.
         * Handle non-ASCII strings with slower code.
         */
        UChar32 c;
        যখন(((c = (uint8_t)*src) != 0) && c <= 0x7f && (pDest < pDestLimit)) {
            *pDest++=(char16_t)c;
            ++src;
        }
        যদি(c == 0) {
            req_দৈর্ঘ্য=(int32_t)(pDest - dest);
            যদি(p_Dest_দৈর্ঘ্য) {
                *p_Dest_দৈর্ঘ্য = req_দৈর্ঘ্য;
            }

            /* Terminate the buffer */
            u_terminateUChars(dest, destCapacity, req_দৈর্ঘ্য, pErrorCode);
            ফেরত dest;
        }
        src_দৈর্ঘ্য = static_cast<int32_t>(uprv_strlen(src));
    }

    /* Faster loop without ongoing checking for srcLength and pDestLimit. */
    UChar32 ch;
    uint8_t t1, t2;
    int32_t i = 0;
    জন্য(;;) {
        int32_t count = (int32_t)(pDestLimit - pDest);
        int32_t count2 = src_দৈর্ঘ্য - i;
        যদি(count >= count2 && src_দৈর্ঘ্য > 0 && U8_IS_SINGLE(*src)) {
            /* fast ASCII loop */
            int32_t start = i;
            uint8_t b;
            যখন(i < src_দৈর্ঘ্য && U8_IS_SINGLE(b = src[i])) {
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
            বিরতি;
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
            বিরতি;
        }
        করো {
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
                        চালু;
                    }
                } অন্যথা {
                    যদি( /* handle U+0000..U+07FF inline */
                        ch >= 0xc0 &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                    ) {
                        *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                        ++i;
                        চালু;
                    }
                }

                যদি(subchar < 0) {
                    *pErrorCode = U_INVALID_CHAR_FOUND;
                    ফেরত nullptr;
                } অন্যথা যদি(subchar > 0xffff && --count == 0) {
                    /*
                     * We need to write two UChars, adjusted count for that,
                     * and ran out of space.
                     */
                    --i;  // back out byte ch
                    বিরতি;
                } অন্যথা {
                    /* function call for error cases */
                    utf8_পরবর্তী_Char_Safe_Body((ধ্রুবক uint8_t *)src, &(i), src_দৈর্ঘ্য, ch, -1);
                    ++অঙ্ক_Substitutions;
                    *(pDest++)=(char16_t)subchar;
                }
            }
        } যখন(--count > 0);
    }

    যখন(i < src_দৈর্ঘ্য && (pDest < pDestLimit)) {
        ch = (uint8_t)src[i++];
        যদি(U8_IS_SINGLE(ch)){
            *pDest++=(char16_t)ch;
        } অন্যথা {
            যদি(ch >= 0xe0) {
                যদি( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < src_দৈর্ঘ্য &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                    (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                ) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                    i += 2;
                    চালু;
                }
            } অন্যথা {
                যদি( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < src_দৈর্ঘ্য &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                ) {
                    *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                    ++i;
                    চালু;
                }
            }

            যদি(subchar < 0) {
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ফেরত nullptr;
            } অন্যথা {
                /* function call for error cases */
                utf8_পরবর্তী_Char_Safe_Body((ধ্রুবক uint8_t *)src, &(i), src_দৈর্ঘ্য, ch, -1);
                ++অঙ্ক_Substitutions;
                যদি(subchar<=0xFFFF) {
                    *(pDest++)=(char16_t)subchar;
                } অন্যথা {
                    *(pDest++)=U16_LEAD(subchar);
                    যদি(pDest<pDestLimit) {
                        *(pDest++)=U16_TRAIL(subchar);
                    } অন্যথা {
                        req_দৈর্ঘ্য++;
                        বিরতি;
                    }
                }
            }
        }
    }

    /* Pre-flight the rest of the string. */
    যখন(i < src_দৈর্ঘ্য) {
        ch = (uint8_t)src[i++];
        যদি(U8_IS_SINGLE(ch)) {
            req_দৈর্ঘ্য++;
        } অন্যথা {
            যদি(ch >= 0xe0) {
                যদি( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < src_দৈর্ঘ্য &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f &&
                    (uint8_t)(src[i+1] - 0x80) <= 0x3f
                ) {
                    req_দৈর্ঘ্য++;
                    i += 2;
                    চালু;
                }
            } অন্যথা {
                যদি( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < src_দৈর্ঘ্য &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f
                ) {
                    req_দৈর্ঘ্য++;
                    ++i;
                    চালু;
                }
            }

            যদি(subchar < 0) {
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ফেরত nullptr;
            } অন্যথা {
                /* function call for error cases */
                utf8_পরবর্তী_Char_Safe_Body((ধ্রুবক uint8_t *)src, &(i), src_দৈর্ঘ্য, ch, -1);
                ++অঙ্ক_Substitutions;
                req_দৈর্ঘ্য+=U16_দৈর্ঘ্য(ch);
            }
        }
    }

    যদি(p_অঙ্ক_Substitutions!=nullptr) {
        *p_অঙ্ক_Substitutions=অঙ্ক_Substitutions;
    }

    req_দৈর্ঘ্য+=(int32_t)(pDest - dest);
    যদি(p_Dest_দৈর্ঘ্য) {
        *p_Dest_দৈর্ঘ্য = req_দৈর্ঘ্য;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, destCapacity, req_দৈর্ঘ্য, pErrorCode);
    ফেরত dest;
}

U_CAPI অক্ষর* U_EXPORT2
u_strToJavaModifiedUTF8(অক্ষর* dest U_LIFETIME_BOUND,
                        int32_t destCapacity,
                        int32_t* p_Dest_দৈর্ঘ্য,
                        ধ্রুবক char16_t* src,
                        int32_t src_দৈর্ঘ্য,
                        UErrorCode* pErrorCode) {
    int32_t req_দৈর্ঘ্য=0;
    uint32_t ch=0;
    ধ্রুবক char16_t *pSrcLimit;
    int32_t count;

    /* args check */
    যদি(U_FAILURE(*pErrorCode)){
        ফেরত nullptr;
    }
    যদি( (src==nullptr && src_দৈর্ঘ্য!=0) || src_দৈর্ঘ্য < -1 ||
        (dest==nullptr && destCapacity!=0) || destCapacity<0
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *pDestLimit = pDest + destCapacity;

    যদি(src_দৈর্ঘ্য==-1) {
        /* Convert NUL-terminated ASCII, then find the string length. */
        যখন((ch=*src)<=0x7f && ch != 0 && pDest<pDestLimit) {
            *pDest++ = (uint8_t)ch;
            ++src;
        }
        যদি(ch == 0) {
            req_দৈর্ঘ্য=(int32_t)(pDest - (uint8_t *)dest);
            যদি(p_Dest_দৈর্ঘ্য) {
                *p_Dest_দৈর্ঘ্য = req_দৈর্ঘ্য;
            }

            /* Terminate the buffer */
            u_terminateChars(dest, destCapacity, req_দৈর্ঘ্য, pErrorCode);
            ফেরত dest;
        }
        src_দৈর্ঘ্য = u_strlen(src);
    }

    /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
    pSrcLimit = (src!=nullptr)?(src+src_দৈর্ঘ্য):nullptr;
    জন্য(;;) {
        count = (int32_t)(pDestLimit - pDest);
        src_দৈর্ঘ্য = (int32_t)(pSrcLimit - src);
        যদি(count >= src_দৈর্ঘ্য && src_দৈর্ঘ্য > 0 && *src <= 0x7f) {
            /* fast ASCII loop */
            ধ্রুবক char16_t *পূর্ব_Src = src;
            int32_t delta;
            যখন(src < pSrcLimit && (ch = *src) <= 0x7f && ch != 0) {
                *pDest++=(uint8_t)ch;
                ++src;
            }
            delta = (int32_t)(src - পূর্ব_Src);
            count -= delta;
            src_দৈর্ঘ্য -= delta;
        }
        /*
         * Each iteration of the inner loop progresses by at most 3 UTF-8
         * bytes and one char16_t.
         */
        count /= 3;
        যদি(count > src_দৈর্ঘ্য) {
            count = src_দৈর্ঘ্য; /* min(remaining dest/3, remaining src) */
        }
        যদি(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            বিরতি;
        }
        করো {
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
        } যখন(--count > 0);
    }

    যখন(src<pSrcLimit) {
        ch=*src++;
        যদি(ch <= 0x7f && ch != 0) {
            যদি(pDest<pDestLimit) {
                *pDest++ = (uint8_t)ch;
            } অন্যথা {
                req_দৈর্ঘ্য = 1;
                বিরতি;
            }
        } অন্যথা যদি(ch <= 0x7ff) {
            যদি((pDestLimit - pDest) >= 2) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } অন্যথা {
                req_দৈর্ঘ্য = 2;
                বিরতি;
            }
        } অন্যথা {
            যদি((pDestLimit - pDest) >= 3) {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } অন্যথা {
                req_দৈর্ঘ্য = 3;
                বিরতি;
            }
        }
    }
    যখন(src<pSrcLimit) {
        ch=*src++;
        যদি(ch <= 0x7f && ch != 0) {
            ++req_দৈর্ঘ্য;
        } অন্যথা যদি(ch<=0x7ff) {
            req_দৈর্ঘ্য+=2;
        } অন্যথা {
            req_দৈর্ঘ্য+=3;
        }
    }

    req_দৈর্ঘ্য+=(int32_t)(pDest - (uint8_t *)dest);
    যদি(p_Dest_দৈর্ঘ্য){
        *p_Dest_দৈর্ঘ্য = req_দৈর্ঘ্য;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, destCapacity, req_দৈর্ঘ্য, pErrorCode);
    ফেরত dest;
}
