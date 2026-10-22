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


#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32WithSub(char16_t* dest U_LIFETIME_BOUND,
                      int32_t dest_ਸਮਰੱਥਾ,
                      int32_t* p_Dest_ਲੰਮਾਈ,
                      ਅਚਲ UChar32* src,
                      int32_t src_ਲੰਮਾਈ,
                      UChar32 subchar,
                      int32_t* p_ਅੰਕ_Substitutions,
                      UError_ਕੋਡ* p_Error_ਕੋਡ) {
    ਅਚਲ UChar32 *src_ਹੱਦਬੰਦੀ;
    UChar32 ch;
    char16_t *dest_ਹੱਦਬੰਦੀ;
    char16_t *pDest;
    int32_t req_ਲੰਮਾਈ;
    int32_t ਅੰਕ_Substitutions;

    /* args check */
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ( (src==ਖਾਲੀ_ਸੂਚਕ && src_ਲੰਮਾਈ!=0) || src_ਲੰਮਾਈ < -1 ||
        (dest_ਸਮਰੱਥਾ<0) || (dest == ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ(p_ਅੰਕ_Substitutions != ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਅੰਕ_Substitutions = 0;
    }

    pDest = dest;
    dest_ਹੱਦਬੰਦੀ = (dest!=ਖਾਲੀ_ਸੂਚਕ)?(dest + dest_ਸਮਰੱਥਾ):ਖਾਲੀ_ਸੂਚਕ;
    req_ਲੰਮਾਈ = 0;
    ਅੰਕ_Substitutions = 0;

    ਜੇ(src_ਲੰਮਾਈ < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        ਜਦੋਂ((ch=*src) != 0 &&
              ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff))) {
            ++src;
            ਜੇ(pDest < dest_ਹੱਦਬੰਦੀ) {
                *pDest++ = (char16_t)ch;
            } ਨਹੀਂਤਾਂ {
                ++req_ਲੰਮਾਈ;
            }
        }
        src_ਹੱਦਬੰਦੀ = src;
        ਜੇ(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            ਜਦੋਂ(*++src_ਹੱਦਬੰਦੀ != 0) {}
        }
    } ਨਹੀਂਤਾਂ {
      src_ਹੱਦਬੰਦੀ = (src!=ਖਾਲੀ_ਸੂਚਕ)?(src + src_ਲੰਮਾਈ):ਖਾਲੀ_ਸੂਚਕ;
    }

    /* convert with length */
    ਜਦੋਂ(src < src_ਹੱਦਬੰਦੀ) {
        ch = *src++;
        ਕਰੋ {
            /* usually "loops" once; twice only for writing subchar */
            ਜੇ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff)) {
                ਜੇ(pDest < dest_ਹੱਦਬੰਦੀ) {
                    *pDest++ = (char16_t)ch;
                } ਨਹੀਂਤਾਂ {
                    ++req_ਲੰਮਾਈ;
                }
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ ਜੇ(0x10000 <= ch && ch <= 0x10ffff) {
                ਜੇ(pDest!=ਖਾਲੀ_ਸੂਚਕ && ((pDest + 2) <= dest_ਹੱਦਬੰਦੀ)) {
                    *pDest++ = U16_LEAD(ch);
                    *pDest++ = U16_TRAIL(ch);
                } ਨਹੀਂਤਾਂ {
                    req_ਲੰਮਾਈ += 2;
                }
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ ਜੇ((ch = subchar) < 0) {
                /* surrogate code point, or not a Unicode code point at all */
                *p_Error_ਕੋਡ = U_INVALID_CHAR_ਮਿਲਿਆ;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            } ਨਹੀਂਤਾਂ {
                ++ਅੰਕ_Substitutions;
            }
        } ਜਦੋਂ(ਸੱਚ);
    }

    req_ਲੰਮਾਈ += (int32_t)(pDest - dest);
    ਜੇ(p_Dest_ਲੰਮਾਈ) {
        *p_Dest_ਲੰਮਾਈ = req_ਲੰਮਾਈ;
    }
    ਜੇ(p_ਅੰਕ_Substitutions != ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਅੰਕ_Substitutions = ਅੰਕ_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, req_ਲੰਮਾਈ, p_Error_ਕੋਡ);
    
    ਵਾਪਸ dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32(char16_t* dest U_LIFETIME_BOUND,
               int32_t dest_ਸਮਰੱਥਾ,
               int32_t* p_Dest_ਲੰਮਾਈ,
               ਅਚਲ UChar32* src,
               int32_t src_ਲੰਮਾਈ,
               UError_ਕੋਡ* p_Error_ਕੋਡ) {
    ਵਾਪਸ u_strFromUTF32WithSub(
            dest, dest_ਸਮਰੱਥਾ, p_Dest_ਲੰਮਾਈ,
            src, src_ਲੰਮਾਈ,
            U_SENTINEL, ਖਾਲੀ_ਸੂਚਕ,
            p_Error_ਕੋਡ);
}

U_CAPI UChar32* U_EXPORT2
u_str_ਮੰਜ਼ਿਲ_UTF32_With_Sub(UChar32* dest U_LIFETIME_BOUND,
                    int32_t dest_ਸਮਰੱਥਾ,
                    int32_t* p_Dest_ਲੰਮਾਈ,
                    ਅਚਲ char16_t* src,
                    int32_t src_ਲੰਮਾਈ,
                    UChar32 subchar,
                    int32_t* p_ਅੰਕ_Substitutions,
                    UError_ਕੋਡ* p_Error_ਕੋਡ) {
    ਅਚਲ char16_t *src_ਹੱਦਬੰਦੀ;
    UChar32 ch;
    char16_t ch2;
    UChar32 *dest_ਹੱਦਬੰਦੀ;
    UChar32 *pDest;
    int32_t req_ਲੰਮਾਈ;
    int32_t ਅੰਕ_Substitutions;

    /* args check */
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ( (src==ਖਾਲੀ_ਸੂਚਕ && src_ਲੰਮਾਈ!=0) || src_ਲੰਮਾਈ < -1 ||
        (dest_ਸਮਰੱਥਾ<0) || (dest == ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ(p_ਅੰਕ_Substitutions != ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਅੰਕ_Substitutions = 0;
    }

    pDest = dest;
    dest_ਹੱਦਬੰਦੀ = (dest!=ਖਾਲੀ_ਸੂਚਕ)?(dest + dest_ਸਮਰੱਥਾ):ਖਾਲੀ_ਸੂਚਕ;
    req_ਲੰਮਾਈ = 0;
    ਅੰਕ_Substitutions = 0;

    ਜੇ(src_ਲੰਮਾਈ < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        ਜਦੋਂ((ch=*src) != 0 && !U16_IS_SURROGATE(ch)) {
            ++src;
            ਜੇ(pDest < dest_ਹੱਦਬੰਦੀ) {
                *pDest++ = ch;
            } ਨਹੀਂਤਾਂ {
                ++req_ਲੰਮਾਈ;
            }
        }
        src_ਹੱਦਬੰਦੀ = src;
        ਜੇ(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            ਜਦੋਂ(*++src_ਹੱਦਬੰਦੀ != 0) {}
        }
    } ਨਹੀਂਤਾਂ {
        src_ਹੱਦਬੰਦੀ = (src!=ਖਾਲੀ_ਸੂਚਕ)?(src + src_ਲੰਮਾਈ):ਖਾਲੀ_ਸੂਚਕ;
    }

    /* convert with length */
    ਜਦੋਂ(src < src_ਹੱਦਬੰਦੀ) {
        ch = *src++;
        ਜੇ(!U16_IS_SURROGATE(ch)) {
            /* write or count ch below */
        } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE_LEAD(ch) && src < src_ਹੱਦਬੰਦੀ && U16_IS_TRAIL(ch2 = *src)) {
            ++src;
            ch = U16_GET_SUPPLEMENTARY(ch, ch2);
        } ਨਹੀਂਤਾਂ ਜੇ((ch = subchar) < 0) {
            /* unpaired surrogate */
            *p_Error_ਕੋਡ = U_INVALID_CHAR_ਮਿਲਿਆ;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        } ਨਹੀਂਤਾਂ {
            ++ਅੰਕ_Substitutions;
        }
        ਜੇ(pDest < dest_ਹੱਦਬੰਦੀ) {
            *pDest++ = ch;
        } ਨਹੀਂਤਾਂ {
            ++req_ਲੰਮਾਈ;
        }
    }

    req_ਲੰਮਾਈ += (int32_t)(pDest - dest);
    ਜੇ(p_Dest_ਲੰਮਾਈ) {
        *p_Dest_ਲੰਮਾਈ = req_ਲੰਮਾਈ;
    }
    ਜੇ(p_ਅੰਕ_Substitutions != ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਅੰਕ_Substitutions = ਅੰਕ_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChar32s(dest, dest_ਸਮਰੱਥਾ, req_ਲੰਮਾਈ, p_Error_ਕੋਡ);

    ਵਾਪਸ dest;
}

U_CAPI UChar32* U_EXPORT2
u_str_ਮੰਜ਼ਿਲ_UTF32(UChar32* dest U_LIFETIME_BOUND,
             int32_t dest_ਸਮਰੱਥਾ,
             int32_t* p_Dest_ਲੰਮਾਈ,
             ਅਚਲ char16_t* src,
             int32_t src_ਲੰਮਾਈ,
             UError_ਕੋਡ* p_Error_ਕੋਡ) {
    ਵਾਪਸ u_str_ਮੰਜ਼ਿਲ_UTF32_With_Sub(
            dest, dest_ਸਮਰੱਥਾ, p_Dest_ਲੰਮਾਈ,
            src, src_ਲੰਮਾਈ,
            U_SENTINEL, ਖਾਲੀ_ਸੂਚਕ,
            p_Error_ਕੋਡ);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                     int32_t dest_ਸਮਰੱਥਾ,
                     int32_t* p_Dest_ਲੰਮਾਈ,
                     ਅਚਲ ਅੱਖਰ* src,
                     int32_t src_ਲੰਮਾਈ,
                     UChar32 subchar,
                     int32_t* p_ਅੰਕ_Substitutions,
                     UError_ਕੋਡ* p_Error_ਕੋਡ) {
    /* args check */
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ( (src==ਖਾਲੀ_ਸੂਚਕ && src_ਲੰਮਾਈ!=0) || src_ਲੰਮਾਈ < -1 ||
        (dest_ਸਮਰੱਥਾ<0) || (dest == ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ(p_ਅੰਕ_Substitutions!=ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਅੰਕ_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *p_Dest_ਹੱਦਬੰਦੀ = dest+dest_ਸਮਰੱਥਾ;
    int32_t req_ਲੰਮਾਈ = 0;
    int32_t ਅੰਕ_Substitutions=0;

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

    ਜੇ(src_ਲੰਮਾਈ < 0){
        /*
         * Transform a NUL-terminated string.
         * The code explicitly checks for NULs only in the lead byte position.
         * A NUL byte in the trail byte position fails the trail byte range check anyway.
         */
        int32_t i;
        UChar32 c;
        ਲਈ(i = 0; (c = (uint8_t)src[i]) != 0 && (pDest < p_Dest_ਹੱਦਬੰਦੀ);) {
            // modified copy of U8_NEXT()
            ++i;
            ਜੇ(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } ਨਹੀਂਤਾਂ {
                uint8_t __t1, __t2;
                ਜੇ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } ਨਹੀਂਤਾਂ ਜੇ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } ਨਹੀਂਤਾਂ {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_ਅਗਲਾ_Char_ਸੁਰੱਖਿਅਤ_Body((ਅਚਲ uint8_t *)src, &(i), -1, c, -1);
                    ਜੇ(c<0 && (++ਅੰਕ_Substitutions, c = subchar) < 0) {
                        *p_Error_ਕੋਡ = U_INVALID_CHAR_ਮਿਲਿਆ;
                        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                    } ਨਹੀਂਤਾਂ ਜੇ(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } ਨਹੀਂਤਾਂ {
                        *(pDest++)=U16_LEAD(c);
                        ਜੇ(pDest<p_Dest_ਹੱਦਬੰਦੀ) {
                            *(pDest++)=U16_TRAIL(c);
                        } ਨਹੀਂਤਾਂ {
                            req_ਲੰਮਾਈ++;
                            ਰੋਕੋ;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        ਜਦੋਂ((c = (uint8_t)src[i]) != 0) {
            // modified copy of U8_NEXT()
            ++i;
            ਜੇ(U8_IS_SINGLE(c)) {
                ++req_ਲੰਮਾਈ;
            } ਨਹੀਂਤਾਂ {
                uint8_t __t1, __t2;
                ਜੇ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++req_ਲੰਮਾਈ;
                    i+=2;
                } ਨਹੀਂਤਾਂ ਜੇ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++req_ਲੰਮਾਈ;
                    ++(i);
                } ਨਹੀਂਤਾਂ {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_ਅਗਲਾ_Char_ਸੁਰੱਖਿਅਤ_Body((ਅਚਲ uint8_t *)src, &(i), -1, c, -1);
                    ਜੇ(c<0 && (++ਅੰਕ_Substitutions, c = subchar) < 0) {
                        *p_Error_ਕੋਡ = U_INVALID_CHAR_ਮਿਲਿਆ;
                        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                    }
                    req_ਲੰਮਾਈ += U16_ਲੰਮਾਈ(c);
                }
            }
        }
    } ਨਹੀਂਤਾਂ /* srcLength >= 0 */ {
        /* Faster loop without ongoing checking for srcLength and pDestLimit. */
        int32_t i = 0;
        UChar32 c;
        ਲਈ(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            int32_t count = (int32_t)(p_Dest_ਹੱਦਬੰਦੀ - pDest);
            int32_t count2 = (src_ਲੰਮਾਈ - i) / 3;
            ਜੇ(count > count2) {
                count = count2; /* min(remaining dest, remaining src/3) */
            }
            ਜੇ(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                ਰੋਕੋ;
            }

            ਕਰੋ {
                // modified copy of U8_NEXT()
                c = (uint8_t)src[i++];
                ਜੇ(U8_IS_SINGLE(c)) {
                    *pDest++=(char16_t)c;
                } ਨਹੀਂਤਾਂ {
                    uint8_t __t1, __t2;
                    ਜੇ( /* handle U+0800..U+FFFF inline */
                            (0xe0<=(c) && (c)<0xf0) &&
                            ((i)+1)<src_ਲੰਮਾਈ &&
                            U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                            (__t2=src[(i)+1]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                        i+=2;
                    } ਨਹੀਂਤਾਂ ਜੇ( /* handle U+0080..U+07FF inline */
                            ((c)<0xe0 && (c)>=0xc2) &&
                            ((i)!=src_ਲੰਮਾਈ) &&
                            (__t1=src[i]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0x1f)<<6)|__t1;
                        ++(i);
                    } ਨਹੀਂਤਾਂ {
                        ਜੇ(c >= 0xf0 || subchar > 0xffff) {
                            // We may read up to four bytes and write up to two UChars,
                            // which we didn't account for with computing count,
                            // so we adjust it here.
                            ਜੇ(--count == 0) {
                                --i;  // back out byte c
                                ਰੋਕੋ;
                            }
                        }

                        /* function call for "complicated" and error cases */
                        (c)=utf8_ਅਗਲਾ_Char_ਸੁਰੱਖਿਅਤ_Body((ਅਚਲ uint8_t *)src, &(i), src_ਲੰਮਾਈ, c, -1);
                        ਜੇ(c<0 && (++ਅੰਕ_Substitutions, c = subchar) < 0) {
                            *p_Error_ਕੋਡ = U_INVALID_CHAR_ਮਿਲਿਆ;
                            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                        } ਨਹੀਂਤਾਂ ਜੇ(c<=0xFFFF) {
                            *(pDest++)=(char16_t)c;
                        } ਨਹੀਂਤਾਂ {
                            *(pDest++)=U16_LEAD(c);
                            *(pDest++)=U16_TRAIL(c);
                        }
                    }
                }
            } ਜਦੋਂ(--count > 0);
        }

        ਜਦੋਂ(i < src_ਲੰਮਾਈ && (pDest < p_Dest_ਹੱਦਬੰਦੀ)) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            ਜੇ(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } ਨਹੀਂਤਾਂ {
                uint8_t __t1, __t2;
                ਜੇ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<src_ਲੰਮਾਈ &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } ਨਹੀਂਤਾਂ ਜੇ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=src_ਲੰਮਾਈ) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } ਨਹੀਂਤਾਂ {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_ਅਗਲਾ_Char_ਸੁਰੱਖਿਅਤ_Body((ਅਚਲ uint8_t *)src, &(i), src_ਲੰਮਾਈ, c, -1);
                    ਜੇ(c<0 && (++ਅੰਕ_Substitutions, c = subchar) < 0) {
                        *p_Error_ਕੋਡ = U_INVALID_CHAR_ਮਿਲਿਆ;
                        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                    } ਨਹੀਂਤਾਂ ਜੇ(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } ਨਹੀਂਤਾਂ {
                        *(pDest++)=U16_LEAD(c);
                        ਜੇ(pDest<p_Dest_ਹੱਦਬੰਦੀ) {
                            *(pDest++)=U16_TRAIL(c);
                        } ਨਹੀਂਤਾਂ {
                            req_ਲੰਮਾਈ++;
                            ਰੋਕੋ;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        ਜਦੋਂ(i < src_ਲੰਮਾਈ) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            ਜੇ(U8_IS_SINGLE(c)) {
                ++req_ਲੰਮਾਈ;
            } ਨਹੀਂਤਾਂ {
                uint8_t __t1, __t2;
                ਜੇ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<src_ਲੰਮਾਈ &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++req_ਲੰਮਾਈ;
                    i+=2;
                } ਨਹੀਂਤਾਂ ਜੇ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=src_ਲੰਮਾਈ) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++req_ਲੰਮਾਈ;
                    ++(i);
                } ਨਹੀਂਤਾਂ {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_ਅਗਲਾ_Char_ਸੁਰੱਖਿਅਤ_Body((ਅਚਲ uint8_t *)src, &(i), src_ਲੰਮਾਈ, c, -1);
                    ਜੇ(c<0 && (++ਅੰਕ_Substitutions, c = subchar) < 0) {
                        *p_Error_ਕੋਡ = U_INVALID_CHAR_ਮਿਲਿਆ;
                        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                    }
                    req_ਲੰਮਾਈ += U16_ਲੰਮਾਈ(c);
                }
            }
        }
    }

    req_ਲੰਮਾਈ+=(int32_t)(pDest - dest);

    ਜੇ(p_ਅੰਕ_Substitutions!=ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਅੰਕ_Substitutions=ਅੰਕ_Substitutions;
    }

    ਜੇ(p_Dest_ਲੰਮਾਈ){
        *p_Dest_ਲੰਮਾਈ = req_ਲੰਮਾਈ;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_ਸਮਰੱਥਾ,req_ਲੰਮਾਈ,p_Error_ਕੋਡ);

    ਵਾਪਸ dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8(char16_t* dest U_LIFETIME_BOUND,
              int32_t dest_ਸਮਰੱਥਾ,
              int32_t* p_Dest_ਲੰਮਾਈ,
              ਅਚਲ ਅੱਖਰ* src,
              int32_t src_ਲੰਮਾਈ,
              UError_ਕੋਡ* p_Error_ਕੋਡ) {
    ਵਾਪਸ u_strFromUTF8WithSub(
            dest, dest_ਸਮਰੱਥਾ, p_Dest_ਲੰਮਾਈ,
            src, src_ਲੰਮਾਈ,
            U_SENTINEL, ਖਾਲੀ_ਸੂਚਕ,
            p_Error_ਕੋਡ);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8Lenient(char16_t* dest U_LIFETIME_BOUND,
                     int32_t dest_ਸਮਰੱਥਾ,
                     int32_t* p_Dest_ਲੰਮਾਈ,
                     ਅਚਲ ਅੱਖਰ* src,
                     int32_t src_ਲੰਮਾਈ,
                     UError_ਕੋਡ* p_Error_ਕੋਡ) {
    char16_t *pDest = dest;
    UChar32 ch;
    int32_t req_ਲੰਮਾਈ = 0;
    uint8_t* pSrc = (uint8_t*) src;

    /* args check */
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
        
    ਜੇ( (src==ਖਾਲੀ_ਸੂਚਕ && src_ਲੰਮਾਈ!=0) || src_ਲੰਮਾਈ < -1 ||
        (dest_ਸਮਰੱਥਾ<0) || (dest == ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ > 0)
    ) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ(src_ਲੰਮਾਈ < 0) {
        /* Transform a NUL-terminated string. */
        char16_t *p_Dest_ਹੱਦਬੰਦੀ = (dest!=ਖਾਲੀ_ਸੂਚਕ)?(dest+dest_ਸਮਰੱਥਾ):ਖਾਲੀ_ਸੂਚਕ;
        uint8_t t1, t2, t3; /* trail bytes */

        ਜਦੋਂ(((ch = *pSrc) != 0) && (pDest < p_Dest_ਹੱਦਬੰਦੀ)) {
            ਜੇ(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ++pSrc;
                ਜਾਰੀ;
            } ਨਹੀਂਤਾਂ ਜੇ(ch < 0xe0) { /* U+0080..U+07FF */
                ਜੇ((t1 = pSrc[1]) != 0) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + t1 - 0x3080);
                    pSrc += 2;
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(ch < 0xf0) { /* U+0800..U+FFFF */
                ਜੇ((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 12) + (t1 << 6) + t2 - 0x2080);
                    pSrc += 3;
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ /* f0..f4 */ { /* U+10000..U+10FFFF */
                ਜੇ((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0 && (t3 = pSrc[3]) != 0) {
                    pSrc += 4;
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (t1 << 12) + (t2 << 6) + t3 - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    ਜੇ(pDest < p_Dest_ਹੱਦਬੰਦੀ) {
                        *(pDest++) = U16_TRAIL(ch);
                    } ਨਹੀਂਤਾਂ {
                        req_ਲੰਮਾਈ = 1;
                        ਰੋਕੋ;
                    }
                    ਜਾਰੀ;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            ਜਦੋਂ(*++pSrc != 0) {}
            ਰੋਕੋ;
        }

        /* Pre-flight the rest of the string. */
        ਜਦੋਂ((ch = *pSrc) != 0) {
            ਜੇ(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                ++req_ਲੰਮਾਈ;
                ++pSrc;
                ਜਾਰੀ;
            } ਨਹੀਂਤਾਂ ਜੇ(ch < 0xe0) { /* U+0080..U+07FF */
                ਜੇ(pSrc[1] != 0) {
                    ++req_ਲੰਮਾਈ;
                    pSrc += 2;
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(ch < 0xf0) { /* U+0800..U+FFFF */
                ਜੇ(pSrc[1] != 0 && pSrc[2] != 0) {
                    ++req_ਲੰਮਾਈ;
                    pSrc += 3;
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ /* f0..f4 */ { /* U+10000..U+10FFFF */
                ਜੇ(pSrc[1] != 0 && pSrc[2] != 0 && pSrc[3] != 0) {
                    req_ਲੰਮਾਈ += 2;
                    pSrc += 4;
                    ਜਾਰੀ;
                }
            }

            /* truncated character at the end */
            ++req_ਲੰਮਾਈ;
            ਰੋਕੋ;
        }
    } ਨਹੀਂਤਾਂ /* srcLength >= 0 */ {
      ਅਚਲ uint8_t *p_Src_ਹੱਦਬੰਦੀ = (pSrc!=ਖਾਲੀ_ਸੂਚਕ)?(pSrc + src_ਲੰਮਾਈ):ਖਾਲੀ_ਸੂਚਕ;

        /*
         * This function requires that if srcLength is given, then it must be
         * destCapatity >= srcLength so that we need not check for
         * destination buffer overflow in the loop.
         */
        ਜੇ(dest_ਸਮਰੱਥਾ < src_ਲੰਮਾਈ) {
            ਜੇ(p_Dest_ਲੰਮਾਈ != ਖਾਲੀ_ਸੂਚਕ) {
                *p_Dest_ਲੰਮਾਈ = src_ਲੰਮਾਈ; /* this likely overestimates the true destLength! */
            }
            *p_Error_ਕੋਡ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }

        ਜੇ((p_Src_ਹੱਦਬੰਦੀ - pSrc) >= 4) {
            p_Src_ਹੱਦਬੰਦੀ -= 3; /* temporarily reduce pSrcLimit */

            /* in this loop, we can always access at least 4 bytes, up to pSrc+3 */
            ਕਰੋ {
                ch = *pSrc++;
                ਜੇ(ch < 0xc0) {
                    /*
                     * ASCII, or a trail byte in lead position which is treated like
                     * a single-byte sequence for better character boundary
                     * resynchronization after illegal sequences.
                     */
                    *pDest++=(char16_t)ch;
                } ਨਹੀਂਤਾਂ ਜੇ(ch < 0xe0) { /* U+0080..U+07FF */
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                } ਨਹੀਂਤਾਂ ਜੇ(ch < 0xf0) { /* U+0800..U+FFFF */
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                } ਨਹੀਂਤਾਂ /* f0..f4 */ { /* U+10000..U+10FFFF */
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                }
            } ਜਦੋਂ(pSrc < p_Src_ਹੱਦਬੰਦੀ);

            p_Src_ਹੱਦਬੰਦੀ += 3; /* restore original pSrcLimit */
        }

        ਜਦੋਂ(pSrc < p_Src_ਹੱਦਬੰਦੀ) {
            ch = *pSrc++;
            ਜੇ(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ਜਾਰੀ;
            } ਨਹੀਂਤਾਂ ਜੇ(ch < 0xe0) { /* U+0080..U+07FF */
                ਜੇ(pSrc < p_Src_ਹੱਦਬੰਦੀ) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(ch < 0xf0) { /* U+0800..U+FFFF */
                ਜੇ((p_Src_ਹੱਦਬੰਦੀ - pSrc) >= 2) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                    pSrc += 3;
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ /* f0..f4 */ { /* U+10000..U+10FFFF */
                ਜੇ((p_Src_ਹੱਦਬੰਦੀ - pSrc) >= 3) {
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                    pSrc += 4;
                    ਜਾਰੀ;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            ਰੋਕੋ;
        }
    }

    req_ਲੰਮਾਈ+=(int32_t)(pDest - dest);

    ਜੇ(p_Dest_ਲੰਮਾਈ){
        *p_Dest_ਲੰਮਾਈ = req_ਲੰਮਾਈ;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_ਸਮਰੱਥਾ,req_ਲੰਮਾਈ,p_Error_ਕੋਡ);

    ਵਾਪਸ dest;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint8_t *
_appendUTF8(uint8_t *pDest, UChar32 c) {
    /* it is 0<=c<=0x10ffff and not a surrogate if called by a validating function */
    ਜੇ((c)<=0x7f) {
        *pDest++ = static_cast<uint8_t>(c);
    } ਨਹੀਂਤਾਂ ਜੇ(c<=0x7ff) {
        *pDest++ = static_cast<uint8_t>((c >> 6) | 0xc0);
        *pDest++ = static_cast<uint8_t>((c & 0x3f) | 0x80);
    } ਨਹੀਂਤਾਂ ਜੇ(c<=0xffff) {
        *pDest++ = static_cast<uint8_t>((c >> 12) | 0xe0);
        *pDest++ = static_cast<uint8_t>(((c >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    } ਨਹੀਂਤਾਂ /* if((uint32_t)(c)<=0x10ffff) */ {
        *pDest++ = static_cast<uint8_t>(((c) >> 18) | 0xf0);
        *pDest++ = static_cast<uint8_t>((((c) >> 12) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>((((c) >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    }
    ਵਾਪਸ pDest;
}

U_CAPI ਅੱਖਰ* U_EXPORT2
u_str_ਮੰਜ਼ਿਲ_UTF8_With_Sub(ਅੱਖਰ* dest U_LIFETIME_BOUND,
                   int32_t dest_ਸਮਰੱਥਾ,
                   int32_t* p_Dest_ਲੰਮਾਈ,
                   ਅਚਲ char16_t* pSrc,
                   int32_t src_ਲੰਮਾਈ,
                   UChar32 subchar,
                   int32_t* p_ਅੰਕ_Substitutions,
                   UError_ਕੋਡ* p_Error_ਕੋਡ) {
    int32_t req_ਲੰਮਾਈ=0;
    uint32_t ch=0,ch2=0;
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *p_Dest_ਹੱਦਬੰਦੀ = (pDest!=ਖਾਲੀ_ਸੂਚਕ)?(pDest + dest_ਸਮਰੱਥਾ):ਖਾਲੀ_ਸੂਚਕ;
    int32_t ਅੰਕ_Substitutions;

    /* args check */
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
        
    ਜੇ( (pSrc==ਖਾਲੀ_ਸੂਚਕ && src_ਲੰਮਾਈ!=0) || src_ਲੰਮਾਈ < -1 ||
        (dest_ਸਮਰੱਥਾ<0) || (dest == ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ(p_ਅੰਕ_Substitutions!=ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਅੰਕ_Substitutions=0;
    }
    ਅੰਕ_Substitutions=0;

    ਜੇ(src_ਲੰਮਾਈ==-1) {
        ਜਦੋਂ((ch=*pSrc)!=0) {
            ++pSrc;
            ਜੇ(ch <= 0x7f) {
                ਜੇ(pDest<p_Dest_ਹੱਦਬੰਦੀ) {
                    *pDest++ = (uint8_t)ch;
                } ਨਹੀਂਤਾਂ {
                    req_ਲੰਮਾਈ = 1;
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(ch <= 0x7ff) {
                ਜੇ((p_Dest_ਹੱਦਬੰਦੀ - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ਨਹੀਂਤਾਂ {
                    req_ਲੰਮਾਈ = 2;
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(ch <= 0xd7ff || ch >= 0xe000) {
                ਜੇ((p_Dest_ਹੱਦਬੰਦੀ - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ਨਹੀਂਤਾਂ {
                    req_ਲੰਮਾਈ = 3;
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ /* ch is a surrogate */ {
                int32_t length;

                /*need not check for NUL because NUL fails U16_IS_TRAIL() anyway*/
                ਜੇ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } ਨਹੀਂਤਾਂ ਜੇ(subchar>=0) {
                    ch=subchar;
                    ++ਅੰਕ_Substitutions;
                } ਨਹੀਂਤਾਂ {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *p_Error_ਕੋਡ = U_INVALID_CHAR_ਮਿਲਿਆ;
                    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                }

                length = U8_ਲੰਮਾਈ(ch);
                ਜੇ((p_Dest_ਹੱਦਬੰਦੀ - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } ਨਹੀਂਤਾਂ {
                    req_ਲੰਮਾਈ = length;
                    ਰੋਕੋ;
                }
            }
        }
        ਜਦੋਂ((ch=*pSrc++)!=0) {
            ਜੇ(ch<=0x7f) {
                ++req_ਲੰਮਾਈ;
            } ਨਹੀਂਤਾਂ ਜੇ(ch<=0x7ff) {
                req_ਲੰਮਾਈ+=2;
            } ਨਹੀਂਤਾਂ ਜੇ(!U16_IS_SURROGATE(ch)) {
                req_ਲੰਮਾਈ+=3;
            } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                req_ਲੰਮਾਈ+=4;
            } ਨਹੀਂਤਾਂ ਜੇ(subchar>=0) {
                req_ਲੰਮਾਈ+=U8_ਲੰਮਾਈ(subchar);
                ++ਅੰਕ_Substitutions;
            } ਨਹੀਂਤਾਂ {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *p_Error_ਕੋਡ = U_INVALID_CHAR_ਮਿਲਿਆ;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
        }
    } ਨਹੀਂਤਾਂ {
        ਅਚਲ char16_t *p_Src_ਹੱਦਬੰਦੀ = (pSrc!=ਖਾਲੀ_ਸੂਚਕ)?(pSrc+src_ਲੰਮਾਈ):ਖਾਲੀ_ਸੂਚਕ;
        int32_t count;

        /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
        ਲਈ(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            count = (int32_t)((p_Dest_ਹੱਦਬੰਦੀ - pDest) / 3);
            src_ਲੰਮਾਈ = (int32_t)(p_Src_ਹੱਦਬੰਦੀ - pSrc);
            ਜੇ(count > src_ਲੰਮਾਈ) {
                count = src_ਲੰਮਾਈ; /* min(remaining dest/3, remaining src) */
            }
            ਜੇ(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                ਰੋਕੋ;
            }
            ਕਰੋ {
                ch=*pSrc++;
                ਜੇ(ch <= 0x7f) {
                    *pDest++ = (uint8_t)ch;
                } ਨਹੀਂਤਾਂ ਜੇ(ch <= 0x7ff) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ਨਹੀਂਤਾਂ ਜੇ(ch <= 0xd7ff || ch >= 0xe000) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ਨਹੀਂਤਾਂ /* ch is a surrogate */ {
                    /*
                     * We will read two UChars and probably output four bytes,
                     * which we didn't account for with computing count,
                     * so we adjust it here.
                     */
                    ਜੇ(--count == 0) {
                        --pSrc; /* undo ch=*pSrc++ for the lead surrogate */
                        ਰੋਕੋ;  /* recompute count */
                    }

                    ਜੇ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                        ++pSrc;
                        ch=U16_GET_SUPPLEMENTARY(ch, ch2);

                        /* writing 4 bytes per 2 UChars is ok */
                        *pDest++=(uint8_t)((ch>>18)|0xf0);
                        *pDest++=(uint8_t)(((ch>>12)&0x3f)|0x80);
                        *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                        *pDest++=(uint8_t)((ch&0x3f)|0x80);
                    } ਨਹੀਂਤਾਂ  {
                        /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                        ਜੇ(subchar>=0) {
                            ch=subchar;
                            ++ਅੰਕ_Substitutions;
                        } ਨਹੀਂਤਾਂ {
                            *p_Error_ਕੋਡ = U_INVALID_CHAR_ਮਿਲਿਆ;
                            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                        }

                        /* convert and append*/
                        pDest=_appendUTF8(pDest, ch);
                    }
                }
            } ਜਦੋਂ(--count > 0);
        }

        ਜਦੋਂ(pSrc<p_Src_ਹੱਦਬੰਦੀ) {
            ch=*pSrc++;
            ਜੇ(ch <= 0x7f) {
                ਜੇ(pDest<p_Dest_ਹੱਦਬੰਦੀ) {
                    *pDest++ = (uint8_t)ch;
                } ਨਹੀਂਤਾਂ {
                    req_ਲੰਮਾਈ = 1;
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(ch <= 0x7ff) {
                ਜੇ((p_Dest_ਹੱਦਬੰਦੀ - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ਨਹੀਂਤਾਂ {
                    req_ਲੰਮਾਈ = 2;
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(ch <= 0xd7ff || ch >= 0xe000) {
                ਜੇ((p_Dest_ਹੱਦਬੰਦੀ - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ਨਹੀਂਤਾਂ {
                    req_ਲੰਮਾਈ = 3;
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ /* ch is a surrogate */ {
                int32_t length;

                ਜੇ(U16_IS_SURROGATE_LEAD(ch) && pSrc<p_Src_ਹੱਦਬੰਦੀ && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } ਨਹੀਂਤਾਂ ਜੇ(subchar>=0) {
                    ch=subchar;
                    ++ਅੰਕ_Substitutions;
                } ਨਹੀਂਤਾਂ {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *p_Error_ਕੋਡ = U_INVALID_CHAR_ਮਿਲਿਆ;
                    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                }

                length = U8_ਲੰਮਾਈ(ch);
                ਜੇ((p_Dest_ਹੱਦਬੰਦੀ - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } ਨਹੀਂਤਾਂ {
                    req_ਲੰਮਾਈ = length;
                    ਰੋਕੋ;
                }
            }
        }
        ਜਦੋਂ(pSrc<p_Src_ਹੱਦਬੰਦੀ) {
            ch=*pSrc++;
            ਜੇ(ch<=0x7f) {
                ++req_ਲੰਮਾਈ;
            } ਨਹੀਂਤਾਂ ਜੇ(ch<=0x7ff) {
                req_ਲੰਮਾਈ+=2;
            } ਨਹੀਂਤਾਂ ਜੇ(!U16_IS_SURROGATE(ch)) {
                req_ਲੰਮਾਈ+=3;
            } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE_LEAD(ch) && pSrc<p_Src_ਹੱਦਬੰਦੀ && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                req_ਲੰਮਾਈ+=4;
            } ਨਹੀਂਤਾਂ ਜੇ(subchar>=0) {
                req_ਲੰਮਾਈ+=U8_ਲੰਮਾਈ(subchar);
                ++ਅੰਕ_Substitutions;
            } ਨਹੀਂਤਾਂ {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *p_Error_ਕੋਡ = U_INVALID_CHAR_ਮਿਲਿਆ;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
        }
    }

    req_ਲੰਮਾਈ+=(int32_t)(pDest - (uint8_t *)dest);

    ਜੇ(p_ਅੰਕ_Substitutions!=ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਅੰਕ_Substitutions=ਅੰਕ_Substitutions;
    }

    ਜੇ(p_Dest_ਲੰਮਾਈ){
        *p_Dest_ਲੰਮਾਈ = req_ਲੰਮਾਈ;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, dest_ਸਮਰੱਥਾ, req_ਲੰਮਾਈ, p_Error_ਕੋਡ);
    ਵਾਪਸ dest;
}

U_CAPI ਅੱਖਰ* U_EXPORT2
u_str_ਮੰਜ਼ਿਲ_UTF8(ਅੱਖਰ* dest U_LIFETIME_BOUND,
            int32_t dest_ਸਮਰੱਥਾ,
            int32_t* p_Dest_ਲੰਮਾਈ,
            ਅਚਲ char16_t* pSrc,
            int32_t src_ਲੰਮਾਈ,
            UError_ਕੋਡ* p_Error_ਕੋਡ) {
    ਵਾਪਸ u_str_ਮੰਜ਼ਿਲ_UTF8_With_Sub(
            dest, dest_ਸਮਰੱਥਾ, p_Dest_ਲੰਮਾਈ,
            pSrc, src_ਲੰਮਾਈ,
            U_SENTINEL, ਖਾਲੀ_ਸੂਚਕ,
            p_Error_ਕੋਡ);
}

U_CAPI char16_t* U_EXPORT2
u_strFromJavaModifiedUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                                 int32_t dest_ਸਮਰੱਥਾ,
                                 int32_t* p_Dest_ਲੰਮਾਈ,
                                 ਅਚਲ ਅੱਖਰ* src,
                                 int32_t src_ਲੰਮਾਈ,
                                 UChar32 subchar,
                                 int32_t* p_ਅੰਕ_Substitutions,
                                 UError_ਕੋਡ* p_Error_ਕੋਡ) {
    /* args check */
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ( (src==ਖਾਲੀ_ਸੂਚਕ && src_ਲੰਮਾਈ!=0) || src_ਲੰਮਾਈ < -1 ||
        (dest==ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ!=0) || dest_ਸਮਰੱਥਾ<0 ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ(p_ਅੰਕ_Substitutions!=ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਅੰਕ_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *p_Dest_ਹੱਦਬੰਦੀ = dest+dest_ਸਮਰੱਥਾ;
    int32_t req_ਲੰਮਾਈ = 0;
    int32_t ਅੰਕ_Substitutions=0;

    ਜੇ(src_ਲੰਮਾਈ < 0) {
        /*
         * Transform a NUL-terminated ASCII string.
         * Handle non-ASCII strings with slower code.
         */
        UChar32 c;
        ਜਦੋਂ(((c = (uint8_t)*src) != 0) && c <= 0x7f && (pDest < p_Dest_ਹੱਦਬੰਦੀ)) {
            *pDest++=(char16_t)c;
            ++src;
        }
        ਜੇ(c == 0) {
            req_ਲੰਮਾਈ=(int32_t)(pDest - dest);
            ਜੇ(p_Dest_ਲੰਮਾਈ) {
                *p_Dest_ਲੰਮਾਈ = req_ਲੰਮਾਈ;
            }

            /* Terminate the buffer */
            u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, req_ਲੰਮਾਈ, p_Error_ਕੋਡ);
            ਵਾਪਸ dest;
        }
        src_ਲੰਮਾਈ = static_cast<int32_t>(uprv_strlen(src));
    }

    /* Faster loop without ongoing checking for srcLength and pDestLimit. */
    UChar32 ch;
    uint8_t t1, t2;
    int32_t i = 0;
    ਲਈ(;;) {
        int32_t count = (int32_t)(p_Dest_ਹੱਦਬੰਦੀ - pDest);
        int32_t count2 = src_ਲੰਮਾਈ - i;
        ਜੇ(count >= count2 && src_ਲੰਮਾਈ > 0 && U8_IS_SINGLE(*src)) {
            /* fast ASCII loop */
            int32_t ਸ਼ੁਰੂ = i;
            uint8_t b;
            ਜਦੋਂ(i < src_ਲੰਮਾਈ && U8_IS_SINGLE(b = src[i])) {
                *pDest++=b;
                ++i;
            }
            int32_t delta = i - ਸ਼ੁਰੂ;
            count -= delta;
            count2 -= delta;
        }
        /*
         * Each iteration of the inner loop progresses by at most 3 UTF-8
         * bytes and one char16_t.
         */
        ਜੇ(subchar > 0xFFFF) {
            ਰੋਕੋ;
        }
        count2 /= 3;
        ਜੇ(count > count2) {
            count = count2; /* min(remaining dest, remaining src/3) */
        }
        ਜੇ(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            ਰੋਕੋ;
        }
        ਕਰੋ {
            ch = (uint8_t)src[i++];
            ਜੇ(U8_IS_SINGLE(ch)) {
                *pDest++=(char16_t)ch;
            } ਨਹੀਂਤਾਂ {
                ਜੇ(ch >= 0xe0) {
                    ਜੇ( /* handle U+0000..U+FFFF inline */
                        ch <= 0xef &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                        (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                    ) {
                        /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                        *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                        i += 2;
                        ਜਾਰੀ;
                    }
                } ਨਹੀਂਤਾਂ {
                    ਜੇ( /* handle U+0000..U+07FF inline */
                        ch >= 0xc0 &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                    ) {
                        *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                        ++i;
                        ਜਾਰੀ;
                    }
                }

                ਜੇ(subchar < 0) {
                    *p_Error_ਕੋਡ = U_INVALID_CHAR_ਮਿਲਿਆ;
                    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                } ਨਹੀਂਤਾਂ ਜੇ(subchar > 0xffff && --count == 0) {
                    /*
                     * We need to write two UChars, adjusted count for that,
                     * and ran out of space.
                     */
                    --i;  // back out byte ch
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ {
                    /* function call for error cases */
                    utf8_ਅਗਲਾ_Char_ਸੁਰੱਖਿਅਤ_Body((ਅਚਲ uint8_t *)src, &(i), src_ਲੰਮਾਈ, ch, -1);
                    ++ਅੰਕ_Substitutions;
                    *(pDest++)=(char16_t)subchar;
                }
            }
        } ਜਦੋਂ(--count > 0);
    }

    ਜਦੋਂ(i < src_ਲੰਮਾਈ && (pDest < p_Dest_ਹੱਦਬੰਦੀ)) {
        ch = (uint8_t)src[i++];
        ਜੇ(U8_IS_SINGLE(ch)){
            *pDest++=(char16_t)ch;
        } ਨਹੀਂਤਾਂ {
            ਜੇ(ch >= 0xe0) {
                ਜੇ( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < src_ਲੰਮਾਈ &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                    (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                ) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                    i += 2;
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ {
                ਜੇ( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < src_ਲੰਮਾਈ &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                ) {
                    *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                    ++i;
                    ਜਾਰੀ;
                }
            }

            ਜੇ(subchar < 0) {
                *p_Error_ਕੋਡ = U_INVALID_CHAR_ਮਿਲਿਆ;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            } ਨਹੀਂਤਾਂ {
                /* function call for error cases */
                utf8_ਅਗਲਾ_Char_ਸੁਰੱਖਿਅਤ_Body((ਅਚਲ uint8_t *)src, &(i), src_ਲੰਮਾਈ, ch, -1);
                ++ਅੰਕ_Substitutions;
                ਜੇ(subchar<=0xFFFF) {
                    *(pDest++)=(char16_t)subchar;
                } ਨਹੀਂਤਾਂ {
                    *(pDest++)=U16_LEAD(subchar);
                    ਜੇ(pDest<p_Dest_ਹੱਦਬੰਦੀ) {
                        *(pDest++)=U16_TRAIL(subchar);
                    } ਨਹੀਂਤਾਂ {
                        req_ਲੰਮਾਈ++;
                        ਰੋਕੋ;
                    }
                }
            }
        }
    }

    /* Pre-flight the rest of the string. */
    ਜਦੋਂ(i < src_ਲੰਮਾਈ) {
        ch = (uint8_t)src[i++];
        ਜੇ(U8_IS_SINGLE(ch)) {
            req_ਲੰਮਾਈ++;
        } ਨਹੀਂਤਾਂ {
            ਜੇ(ch >= 0xe0) {
                ਜੇ( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < src_ਲੰਮਾਈ &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f &&
                    (uint8_t)(src[i+1] - 0x80) <= 0x3f
                ) {
                    req_ਲੰਮਾਈ++;
                    i += 2;
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ {
                ਜੇ( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < src_ਲੰਮਾਈ &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f
                ) {
                    req_ਲੰਮਾਈ++;
                    ++i;
                    ਜਾਰੀ;
                }
            }

            ਜੇ(subchar < 0) {
                *p_Error_ਕੋਡ = U_INVALID_CHAR_ਮਿਲਿਆ;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            } ਨਹੀਂਤਾਂ {
                /* function call for error cases */
                utf8_ਅਗਲਾ_Char_ਸੁਰੱਖਿਅਤ_Body((ਅਚਲ uint8_t *)src, &(i), src_ਲੰਮਾਈ, ch, -1);
                ++ਅੰਕ_Substitutions;
                req_ਲੰਮਾਈ+=U16_ਲੰਮਾਈ(ch);
            }
        }
    }

    ਜੇ(p_ਅੰਕ_Substitutions!=ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਅੰਕ_Substitutions=ਅੰਕ_Substitutions;
    }

    req_ਲੰਮਾਈ+=(int32_t)(pDest - dest);
    ਜੇ(p_Dest_ਲੰਮਾਈ) {
        *p_Dest_ਲੰਮਾਈ = req_ਲੰਮਾਈ;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, req_ਲੰਮਾਈ, p_Error_ਕੋਡ);
    ਵਾਪਸ dest;
}

U_CAPI ਅੱਖਰ* U_EXPORT2
u_str_ਮੰਜ਼ਿਲ_Java_Modified_UTF8(ਅੱਖਰ* dest U_LIFETIME_BOUND,
                        int32_t dest_ਸਮਰੱਥਾ,
                        int32_t* p_Dest_ਲੰਮਾਈ,
                        ਅਚਲ char16_t* src,
                        int32_t src_ਲੰਮਾਈ,
                        UError_ਕੋਡ* p_Error_ਕੋਡ) {
    int32_t req_ਲੰਮਾਈ=0;
    uint32_t ch=0;
    ਅਚਲ char16_t *p_Src_ਹੱਦਬੰਦੀ;
    int32_t count;

    /* args check */
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ( (src==ਖਾਲੀ_ਸੂਚਕ && src_ਲੰਮਾਈ!=0) || src_ਲੰਮਾਈ < -1 ||
        (dest==ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ!=0) || dest_ਸਮਰੱਥਾ<0
    ) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *p_Dest_ਹੱਦਬੰਦੀ = pDest + dest_ਸਮਰੱਥਾ;

    ਜੇ(src_ਲੰਮਾਈ==-1) {
        /* Convert NUL-terminated ASCII, then find the string length. */
        ਜਦੋਂ((ch=*src)<=0x7f && ch != 0 && pDest<p_Dest_ਹੱਦਬੰਦੀ) {
            *pDest++ = (uint8_t)ch;
            ++src;
        }
        ਜੇ(ch == 0) {
            req_ਲੰਮਾਈ=(int32_t)(pDest - (uint8_t *)dest);
            ਜੇ(p_Dest_ਲੰਮਾਈ) {
                *p_Dest_ਲੰਮਾਈ = req_ਲੰਮਾਈ;
            }

            /* Terminate the buffer */
            u_terminateChars(dest, dest_ਸਮਰੱਥਾ, req_ਲੰਮਾਈ, p_Error_ਕੋਡ);
            ਵਾਪਸ dest;
        }
        src_ਲੰਮਾਈ = u_strlen(src);
    }

    /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
    p_Src_ਹੱਦਬੰਦੀ = (src!=ਖਾਲੀ_ਸੂਚਕ)?(src+src_ਲੰਮਾਈ):ਖਾਲੀ_ਸੂਚਕ;
    ਲਈ(;;) {
        count = (int32_t)(p_Dest_ਹੱਦਬੰਦੀ - pDest);
        src_ਲੰਮਾਈ = (int32_t)(p_Src_ਹੱਦਬੰਦੀ - src);
        ਜੇ(count >= src_ਲੰਮਾਈ && src_ਲੰਮਾਈ > 0 && *src <= 0x7f) {
            /* fast ASCII loop */
            ਅਚਲ char16_t *prevSrc = src;
            int32_t delta;
            ਜਦੋਂ(src < p_Src_ਹੱਦਬੰਦੀ && (ch = *src) <= 0x7f && ch != 0) {
                *pDest++=(uint8_t)ch;
                ++src;
            }
            delta = (int32_t)(src - prevSrc);
            count -= delta;
            src_ਲੰਮਾਈ -= delta;
        }
        /*
         * Each iteration of the inner loop progresses by at most 3 UTF-8
         * bytes and one char16_t.
         */
        count /= 3;
        ਜੇ(count > src_ਲੰਮਾਈ) {
            count = src_ਲੰਮਾਈ; /* min(remaining dest/3, remaining src) */
        }
        ਜੇ(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            ਰੋਕੋ;
        }
        ਕਰੋ {
            ch=*src++;
            ਜੇ(ch <= 0x7f && ch != 0) {
                *pDest++ = (uint8_t)ch;
            } ਨਹੀਂਤਾਂ ਜੇ(ch <= 0x7ff) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } ਨਹੀਂਤਾਂ {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            }
        } ਜਦੋਂ(--count > 0);
    }

    ਜਦੋਂ(src<p_Src_ਹੱਦਬੰਦੀ) {
        ch=*src++;
        ਜੇ(ch <= 0x7f && ch != 0) {
            ਜੇ(pDest<p_Dest_ਹੱਦਬੰਦੀ) {
                *pDest++ = (uint8_t)ch;
            } ਨਹੀਂਤਾਂ {
                req_ਲੰਮਾਈ = 1;
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(ch <= 0x7ff) {
            ਜੇ((p_Dest_ਹੱਦਬੰਦੀ - pDest) >= 2) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } ਨਹੀਂਤਾਂ {
                req_ਲੰਮਾਈ = 2;
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ {
            ਜੇ((p_Dest_ਹੱਦਬੰਦੀ - pDest) >= 3) {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } ਨਹੀਂਤਾਂ {
                req_ਲੰਮਾਈ = 3;
                ਰੋਕੋ;
            }
        }
    }
    ਜਦੋਂ(src<p_Src_ਹੱਦਬੰਦੀ) {
        ch=*src++;
        ਜੇ(ch <= 0x7f && ch != 0) {
            ++req_ਲੰਮਾਈ;
        } ਨਹੀਂਤਾਂ ਜੇ(ch<=0x7ff) {
            req_ਲੰਮਾਈ+=2;
        } ਨਹੀਂਤਾਂ {
            req_ਲੰਮਾਈ+=3;
        }
    }

    req_ਲੰਮਾਈ+=(int32_t)(pDest - (uint8_t *)dest);
    ਜੇ(p_Dest_ਲੰਮਾਈ){
        *p_Dest_ਲੰਮਾਈ = req_ਲੰਮਾਈ;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, dest_ਸਮਰੱਥਾ, req_ਲੰਮਾਈ, p_Error_ਕੋਡ);
    ਵਾਪਸ dest;
}
