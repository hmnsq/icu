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
                      int32_t dest_استعداد,
                      int32_t* p_Dest_لمبائی,
                      ثابت UChar32* src,
                      int32_t src_لمبائی,
                      UChar32 subchar,
                      int32_t* p_عدد_Substitutions,
                      UError_رمز* p_Error_رمز) {
    ثابت UChar32 *src_حد_بندی;
    UChar32 ch;
    char16_t *dest_حد_بندی;
    char16_t *pDest;
    int32_t req_لمبائی;
    int32_t عدد_Substitutions;

    /* args check */
    اگر(U_FAILURE(*p_Error_رمز)){
        واپس خالی_اشاریہ;
    }
    اگر( (src==خالی_اشاریہ && src_لمبائی!=0) || src_لمبائی < -1 ||
        (dest_استعداد<0) || (dest == خالی_اشاریہ && dest_استعداد > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    اگر(p_عدد_Substitutions != خالی_اشاریہ) {
        *p_عدد_Substitutions = 0;
    }

    pDest = dest;
    dest_حد_بندی = (dest!=خالی_اشاریہ)?(dest + dest_استعداد):خالی_اشاریہ;
    req_لمبائی = 0;
    عدد_Substitutions = 0;

    اگر(src_لمبائی < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        جب((ch=*src) != 0 &&
              ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff))) {
            ++src;
            اگر(pDest < dest_حد_بندی) {
                *pDest++ = (char16_t)ch;
            } ورنہ {
                ++req_لمبائی;
            }
        }
        src_حد_بندی = src;
        اگر(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            جب(*++src_حد_بندی != 0) {}
        }
    } ورنہ {
      src_حد_بندی = (src!=خالی_اشاریہ)?(src + src_لمبائی):خالی_اشاریہ;
    }

    /* convert with length */
    جب(src < src_حد_بندی) {
        ch = *src++;
        کرو {
            /* usually "loops" once; twice only for writing subchar */
            اگر((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff)) {
                اگر(pDest < dest_حد_بندی) {
                    *pDest++ = (char16_t)ch;
                } ورنہ {
                    ++req_لمبائی;
                }
                توڑو;
            } ورنہ اگر(0x10000 <= ch && ch <= 0x10ffff) {
                اگر(pDest!=خالی_اشاریہ && ((pDest + 2) <= dest_حد_بندی)) {
                    *pDest++ = U16_LEAD(ch);
                    *pDest++ = U16_TRAIL(ch);
                } ورنہ {
                    req_لمبائی += 2;
                }
                توڑو;
            } ورنہ اگر((ch = subchar) < 0) {
                /* surrogate code point, or not a Unicode code point at all */
                *p_Error_رمز = U_INVALID_CHAR_ملا;
                واپس خالی_اشاریہ;
            } ورنہ {
                ++عدد_Substitutions;
            }
        } جب(سچ);
    }

    req_لمبائی += (int32_t)(pDest - dest);
    اگر(p_Dest_لمبائی) {
        *p_Dest_لمبائی = req_لمبائی;
    }
    اگر(p_عدد_Substitutions != خالی_اشاریہ) {
        *p_عدد_Substitutions = عدد_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, dest_استعداد, req_لمبائی, p_Error_رمز);
    
    واپس dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32(char16_t* dest U_LIFETIME_BOUND,
               int32_t dest_استعداد,
               int32_t* p_Dest_لمبائی,
               ثابت UChar32* src,
               int32_t src_لمبائی,
               UError_رمز* p_Error_رمز) {
    واپس u_strFromUTF32WithSub(
            dest, dest_استعداد, p_Dest_لمبائی,
            src, src_لمبائی,
            U_SENTINEL, خالی_اشاریہ,
            p_Error_رمز);
}

U_CAPI UChar32* U_EXPORT2
u_str_مقصد_UTF32_With_Sub(UChar32* dest U_LIFETIME_BOUND,
                    int32_t dest_استعداد,
                    int32_t* p_Dest_لمبائی,
                    ثابت char16_t* src,
                    int32_t src_لمبائی,
                    UChar32 subchar,
                    int32_t* p_عدد_Substitutions,
                    UError_رمز* p_Error_رمز) {
    ثابت char16_t *src_حد_بندی;
    UChar32 ch;
    char16_t ch2;
    UChar32 *dest_حد_بندی;
    UChar32 *pDest;
    int32_t req_لمبائی;
    int32_t عدد_Substitutions;

    /* args check */
    اگر(U_FAILURE(*p_Error_رمز)){
        واپس خالی_اشاریہ;
    }
    اگر( (src==خالی_اشاریہ && src_لمبائی!=0) || src_لمبائی < -1 ||
        (dest_استعداد<0) || (dest == خالی_اشاریہ && dest_استعداد > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    اگر(p_عدد_Substitutions != خالی_اشاریہ) {
        *p_عدد_Substitutions = 0;
    }

    pDest = dest;
    dest_حد_بندی = (dest!=خالی_اشاریہ)?(dest + dest_استعداد):خالی_اشاریہ;
    req_لمبائی = 0;
    عدد_Substitutions = 0;

    اگر(src_لمبائی < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        جب((ch=*src) != 0 && !U16_IS_SURROGATE(ch)) {
            ++src;
            اگر(pDest < dest_حد_بندی) {
                *pDest++ = ch;
            } ورنہ {
                ++req_لمبائی;
            }
        }
        src_حد_بندی = src;
        اگر(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            جب(*++src_حد_بندی != 0) {}
        }
    } ورنہ {
        src_حد_بندی = (src!=خالی_اشاریہ)?(src + src_لمبائی):خالی_اشاریہ;
    }

    /* convert with length */
    جب(src < src_حد_بندی) {
        ch = *src++;
        اگر(!U16_IS_SURROGATE(ch)) {
            /* write or count ch below */
        } ورنہ اگر(U16_IS_SURROGATE_LEAD(ch) && src < src_حد_بندی && U16_IS_TRAIL(ch2 = *src)) {
            ++src;
            ch = U16_GET_SUPPLEMENTARY(ch, ch2);
        } ورنہ اگر((ch = subchar) < 0) {
            /* unpaired surrogate */
            *p_Error_رمز = U_INVALID_CHAR_ملا;
            واپس خالی_اشاریہ;
        } ورنہ {
            ++عدد_Substitutions;
        }
        اگر(pDest < dest_حد_بندی) {
            *pDest++ = ch;
        } ورنہ {
            ++req_لمبائی;
        }
    }

    req_لمبائی += (int32_t)(pDest - dest);
    اگر(p_Dest_لمبائی) {
        *p_Dest_لمبائی = req_لمبائی;
    }
    اگر(p_عدد_Substitutions != خالی_اشاریہ) {
        *p_عدد_Substitutions = عدد_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChar32s(dest, dest_استعداد, req_لمبائی, p_Error_رمز);

    واپس dest;
}

U_CAPI UChar32* U_EXPORT2
u_str_مقصد_UTF32(UChar32* dest U_LIFETIME_BOUND,
             int32_t dest_استعداد,
             int32_t* p_Dest_لمبائی,
             ثابت char16_t* src,
             int32_t src_لمبائی,
             UError_رمز* p_Error_رمز) {
    واپس u_str_مقصد_UTF32_With_Sub(
            dest, dest_استعداد, p_Dest_لمبائی,
            src, src_لمبائی,
            U_SENTINEL, خالی_اشاریہ,
            p_Error_رمز);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                     int32_t dest_استعداد,
                     int32_t* p_Dest_لمبائی,
                     ثابت حرف* src,
                     int32_t src_لمبائی,
                     UChar32 subchar,
                     int32_t* p_عدد_Substitutions,
                     UError_رمز* p_Error_رمز) {
    /* args check */
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    اگر( (src==خالی_اشاریہ && src_لمبائی!=0) || src_لمبائی < -1 ||
        (dest_استعداد<0) || (dest == خالی_اشاریہ && dest_استعداد > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    اگر(p_عدد_Substitutions!=خالی_اشاریہ) {
        *p_عدد_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *p_Dest_حد_بندی = dest+dest_استعداد;
    int32_t req_لمبائی = 0;
    int32_t عدد_Substitutions=0;

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

    اگر(src_لمبائی < 0){
        /*
         * Transform a NUL-terminated string.
         * The code explicitly checks for NULs only in the lead byte position.
         * A NUL byte in the trail byte position fails the trail byte range check anyway.
         */
        int32_t i;
        UChar32 c;
        برائے(i = 0; (c = (uint8_t)src[i]) != 0 && (pDest < p_Dest_حد_بندی);) {
            // modified copy of U8_NEXT()
            ++i;
            اگر(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } ورنہ {
                uint8_t __t1, __t2;
                اگر( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } ورنہ اگر( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } ورنہ {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_اگلا_Char_محفوظ_Body((ثابت uint8_t *)src, &(i), -1, c, -1);
                    اگر(c<0 && (++عدد_Substitutions, c = subchar) < 0) {
                        *p_Error_رمز = U_INVALID_CHAR_ملا;
                        واپس خالی_اشاریہ;
                    } ورنہ اگر(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } ورنہ {
                        *(pDest++)=U16_LEAD(c);
                        اگر(pDest<p_Dest_حد_بندی) {
                            *(pDest++)=U16_TRAIL(c);
                        } ورنہ {
                            req_لمبائی++;
                            توڑو;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        جب((c = (uint8_t)src[i]) != 0) {
            // modified copy of U8_NEXT()
            ++i;
            اگر(U8_IS_SINGLE(c)) {
                ++req_لمبائی;
            } ورنہ {
                uint8_t __t1, __t2;
                اگر( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++req_لمبائی;
                    i+=2;
                } ورنہ اگر( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++req_لمبائی;
                    ++(i);
                } ورنہ {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_اگلا_Char_محفوظ_Body((ثابت uint8_t *)src, &(i), -1, c, -1);
                    اگر(c<0 && (++عدد_Substitutions, c = subchar) < 0) {
                        *p_Error_رمز = U_INVALID_CHAR_ملا;
                        واپس خالی_اشاریہ;
                    }
                    req_لمبائی += U16_لمبائی(c);
                }
            }
        }
    } ورنہ /* srcLength >= 0 */ {
        /* Faster loop without ongoing checking for srcLength and pDestLimit. */
        int32_t i = 0;
        UChar32 c;
        برائے(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            int32_t count = (int32_t)(p_Dest_حد_بندی - pDest);
            int32_t count2 = (src_لمبائی - i) / 3;
            اگر(count > count2) {
                count = count2; /* min(remaining dest, remaining src/3) */
            }
            اگر(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                توڑو;
            }

            کرو {
                // modified copy of U8_NEXT()
                c = (uint8_t)src[i++];
                اگر(U8_IS_SINGLE(c)) {
                    *pDest++=(char16_t)c;
                } ورنہ {
                    uint8_t __t1, __t2;
                    اگر( /* handle U+0800..U+FFFF inline */
                            (0xe0<=(c) && (c)<0xf0) &&
                            ((i)+1)<src_لمبائی &&
                            U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                            (__t2=src[(i)+1]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                        i+=2;
                    } ورنہ اگر( /* handle U+0080..U+07FF inline */
                            ((c)<0xe0 && (c)>=0xc2) &&
                            ((i)!=src_لمبائی) &&
                            (__t1=src[i]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0x1f)<<6)|__t1;
                        ++(i);
                    } ورنہ {
                        اگر(c >= 0xf0 || subchar > 0xffff) {
                            // We may read up to four bytes and write up to two UChars,
                            // which we didn't account for with computing count,
                            // so we adjust it here.
                            اگر(--count == 0) {
                                --i;  // back out byte c
                                توڑو;
                            }
                        }

                        /* function call for "complicated" and error cases */
                        (c)=utf8_اگلا_Char_محفوظ_Body((ثابت uint8_t *)src, &(i), src_لمبائی, c, -1);
                        اگر(c<0 && (++عدد_Substitutions, c = subchar) < 0) {
                            *p_Error_رمز = U_INVALID_CHAR_ملا;
                            واپس خالی_اشاریہ;
                        } ورنہ اگر(c<=0xFFFF) {
                            *(pDest++)=(char16_t)c;
                        } ورنہ {
                            *(pDest++)=U16_LEAD(c);
                            *(pDest++)=U16_TRAIL(c);
                        }
                    }
                }
            } جب(--count > 0);
        }

        جب(i < src_لمبائی && (pDest < p_Dest_حد_بندی)) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            اگر(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } ورنہ {
                uint8_t __t1, __t2;
                اگر( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<src_لمبائی &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } ورنہ اگر( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=src_لمبائی) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } ورنہ {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_اگلا_Char_محفوظ_Body((ثابت uint8_t *)src, &(i), src_لمبائی, c, -1);
                    اگر(c<0 && (++عدد_Substitutions, c = subchar) < 0) {
                        *p_Error_رمز = U_INVALID_CHAR_ملا;
                        واپس خالی_اشاریہ;
                    } ورنہ اگر(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } ورنہ {
                        *(pDest++)=U16_LEAD(c);
                        اگر(pDest<p_Dest_حد_بندی) {
                            *(pDest++)=U16_TRAIL(c);
                        } ورنہ {
                            req_لمبائی++;
                            توڑو;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        جب(i < src_لمبائی) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            اگر(U8_IS_SINGLE(c)) {
                ++req_لمبائی;
            } ورنہ {
                uint8_t __t1, __t2;
                اگر( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<src_لمبائی &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++req_لمبائی;
                    i+=2;
                } ورنہ اگر( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=src_لمبائی) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++req_لمبائی;
                    ++(i);
                } ورنہ {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_اگلا_Char_محفوظ_Body((ثابت uint8_t *)src, &(i), src_لمبائی, c, -1);
                    اگر(c<0 && (++عدد_Substitutions, c = subchar) < 0) {
                        *p_Error_رمز = U_INVALID_CHAR_ملا;
                        واپس خالی_اشاریہ;
                    }
                    req_لمبائی += U16_لمبائی(c);
                }
            }
        }
    }

    req_لمبائی+=(int32_t)(pDest - dest);

    اگر(p_عدد_Substitutions!=خالی_اشاریہ) {
        *p_عدد_Substitutions=عدد_Substitutions;
    }

    اگر(p_Dest_لمبائی){
        *p_Dest_لمبائی = req_لمبائی;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_استعداد,req_لمبائی,p_Error_رمز);

    واپس dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8(char16_t* dest U_LIFETIME_BOUND,
              int32_t dest_استعداد,
              int32_t* p_Dest_لمبائی,
              ثابت حرف* src,
              int32_t src_لمبائی,
              UError_رمز* p_Error_رمز) {
    واپس u_strFromUTF8WithSub(
            dest, dest_استعداد, p_Dest_لمبائی,
            src, src_لمبائی,
            U_SENTINEL, خالی_اشاریہ,
            p_Error_رمز);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8Lenient(char16_t* dest U_LIFETIME_BOUND,
                     int32_t dest_استعداد,
                     int32_t* p_Dest_لمبائی,
                     ثابت حرف* src,
                     int32_t src_لمبائی,
                     UError_رمز* p_Error_رمز) {
    char16_t *pDest = dest;
    UChar32 ch;
    int32_t req_لمبائی = 0;
    uint8_t* pSrc = (uint8_t*) src;

    /* args check */
    اگر(U_FAILURE(*p_Error_رمز)){
        واپس خالی_اشاریہ;
    }
        
    اگر( (src==خالی_اشاریہ && src_لمبائی!=0) || src_لمبائی < -1 ||
        (dest_استعداد<0) || (dest == خالی_اشاریہ && dest_استعداد > 0)
    ) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    اگر(src_لمبائی < 0) {
        /* Transform a NUL-terminated string. */
        char16_t *p_Dest_حد_بندی = (dest!=خالی_اشاریہ)?(dest+dest_استعداد):خالی_اشاریہ;
        uint8_t t1, t2, t3; /* trail bytes */

        جب(((ch = *pSrc) != 0) && (pDest < p_Dest_حد_بندی)) {
            اگر(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ++pSrc;
                جاری;
            } ورنہ اگر(ch < 0xe0) { /* U+0080..U+07FF */
                اگر((t1 = pSrc[1]) != 0) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + t1 - 0x3080);
                    pSrc += 2;
                    جاری;
                }
            } ورنہ اگر(ch < 0xf0) { /* U+0800..U+FFFF */
                اگر((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 12) + (t1 << 6) + t2 - 0x2080);
                    pSrc += 3;
                    جاری;
                }
            } ورنہ /* f0..f4 */ { /* U+10000..U+10FFFF */
                اگر((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0 && (t3 = pSrc[3]) != 0) {
                    pSrc += 4;
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (t1 << 12) + (t2 << 6) + t3 - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    اگر(pDest < p_Dest_حد_بندی) {
                        *(pDest++) = U16_TRAIL(ch);
                    } ورنہ {
                        req_لمبائی = 1;
                        توڑو;
                    }
                    جاری;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            جب(*++pSrc != 0) {}
            توڑو;
        }

        /* Pre-flight the rest of the string. */
        جب((ch = *pSrc) != 0) {
            اگر(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                ++req_لمبائی;
                ++pSrc;
                جاری;
            } ورنہ اگر(ch < 0xe0) { /* U+0080..U+07FF */
                اگر(pSrc[1] != 0) {
                    ++req_لمبائی;
                    pSrc += 2;
                    جاری;
                }
            } ورنہ اگر(ch < 0xf0) { /* U+0800..U+FFFF */
                اگر(pSrc[1] != 0 && pSrc[2] != 0) {
                    ++req_لمبائی;
                    pSrc += 3;
                    جاری;
                }
            } ورنہ /* f0..f4 */ { /* U+10000..U+10FFFF */
                اگر(pSrc[1] != 0 && pSrc[2] != 0 && pSrc[3] != 0) {
                    req_لمبائی += 2;
                    pSrc += 4;
                    جاری;
                }
            }

            /* truncated character at the end */
            ++req_لمبائی;
            توڑو;
        }
    } ورنہ /* srcLength >= 0 */ {
      ثابت uint8_t *p_Src_حد_بندی = (pSrc!=خالی_اشاریہ)?(pSrc + src_لمبائی):خالی_اشاریہ;

        /*
         * This function requires that if srcLength is given, then it must be
         * destCapatity >= srcLength so that we need not check for
         * destination buffer overflow in the loop.
         */
        اگر(dest_استعداد < src_لمبائی) {
            اگر(p_Dest_لمبائی != خالی_اشاریہ) {
                *p_Dest_لمبائی = src_لمبائی; /* this likely overestimates the true destLength! */
            }
            *p_Error_رمز = U_بفر_حد_سے_تجاوز_ERROR;
            واپس خالی_اشاریہ;
        }

        اگر((p_Src_حد_بندی - pSrc) >= 4) {
            p_Src_حد_بندی -= 3; /* temporarily reduce pSrcLimit */

            /* in this loop, we can always access at least 4 bytes, up to pSrc+3 */
            کرو {
                ch = *pSrc++;
                اگر(ch < 0xc0) {
                    /*
                     * ASCII, or a trail byte in lead position which is treated like
                     * a single-byte sequence for better character boundary
                     * resynchronization after illegal sequences.
                     */
                    *pDest++=(char16_t)ch;
                } ورنہ اگر(ch < 0xe0) { /* U+0080..U+07FF */
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                } ورنہ اگر(ch < 0xf0) { /* U+0800..U+FFFF */
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                } ورنہ /* f0..f4 */ { /* U+10000..U+10FFFF */
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                }
            } جب(pSrc < p_Src_حد_بندی);

            p_Src_حد_بندی += 3; /* restore original pSrcLimit */
        }

        جب(pSrc < p_Src_حد_بندی) {
            ch = *pSrc++;
            اگر(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                جاری;
            } ورنہ اگر(ch < 0xe0) { /* U+0080..U+07FF */
                اگر(pSrc < p_Src_حد_بندی) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                    جاری;
                }
            } ورنہ اگر(ch < 0xf0) { /* U+0800..U+FFFF */
                اگر((p_Src_حد_بندی - pSrc) >= 2) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                    pSrc += 3;
                    جاری;
                }
            } ورنہ /* f0..f4 */ { /* U+10000..U+10FFFF */
                اگر((p_Src_حد_بندی - pSrc) >= 3) {
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                    pSrc += 4;
                    جاری;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            توڑو;
        }
    }

    req_لمبائی+=(int32_t)(pDest - dest);

    اگر(p_Dest_لمبائی){
        *p_Dest_لمبائی = req_لمبائی;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_استعداد,req_لمبائی,p_Error_رمز);

    واپس dest;
}

ساکن اندرونی uint8_t *
_appendUTF8(uint8_t *pDest, UChar32 c) {
    /* it is 0<=c<=0x10ffff and not a surrogate if called by a validating function */
    اگر((c)<=0x7f) {
        *pDest++ = static_cast<uint8_t>(c);
    } ورنہ اگر(c<=0x7ff) {
        *pDest++ = static_cast<uint8_t>((c >> 6) | 0xc0);
        *pDest++ = static_cast<uint8_t>((c & 0x3f) | 0x80);
    } ورنہ اگر(c<=0xffff) {
        *pDest++ = static_cast<uint8_t>((c >> 12) | 0xe0);
        *pDest++ = static_cast<uint8_t>(((c >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    } ورنہ /* if((uint32_t)(c)<=0x10ffff) */ {
        *pDest++ = static_cast<uint8_t>(((c) >> 18) | 0xf0);
        *pDest++ = static_cast<uint8_t>((((c) >> 12) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>((((c) >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    }
    واپس pDest;
}

U_CAPI حرف* U_EXPORT2
u_str_مقصد_UTF8_With_Sub(حرف* dest U_LIFETIME_BOUND,
                   int32_t dest_استعداد,
                   int32_t* p_Dest_لمبائی,
                   ثابت char16_t* pSrc,
                   int32_t src_لمبائی,
                   UChar32 subchar,
                   int32_t* p_عدد_Substitutions,
                   UError_رمز* p_Error_رمز) {
    int32_t req_لمبائی=0;
    uint32_t ch=0,ch2=0;
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *p_Dest_حد_بندی = (pDest!=خالی_اشاریہ)?(pDest + dest_استعداد):خالی_اشاریہ;
    int32_t عدد_Substitutions;

    /* args check */
    اگر(U_FAILURE(*p_Error_رمز)){
        واپس خالی_اشاریہ;
    }
        
    اگر( (pSrc==خالی_اشاریہ && src_لمبائی!=0) || src_لمبائی < -1 ||
        (dest_استعداد<0) || (dest == خالی_اشاریہ && dest_استعداد > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    اگر(p_عدد_Substitutions!=خالی_اشاریہ) {
        *p_عدد_Substitutions=0;
    }
    عدد_Substitutions=0;

    اگر(src_لمبائی==-1) {
        جب((ch=*pSrc)!=0) {
            ++pSrc;
            اگر(ch <= 0x7f) {
                اگر(pDest<p_Dest_حد_بندی) {
                    *pDest++ = (uint8_t)ch;
                } ورنہ {
                    req_لمبائی = 1;
                    توڑو;
                }
            } ورنہ اگر(ch <= 0x7ff) {
                اگر((p_Dest_حد_بندی - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ورنہ {
                    req_لمبائی = 2;
                    توڑو;
                }
            } ورنہ اگر(ch <= 0xd7ff || ch >= 0xe000) {
                اگر((p_Dest_حد_بندی - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ورنہ {
                    req_لمبائی = 3;
                    توڑو;
                }
            } ورنہ /* ch is a surrogate */ {
                int32_t length;

                /*need not check for NUL because NUL fails U16_IS_TRAIL() anyway*/
                اگر(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } ورنہ اگر(subchar>=0) {
                    ch=subchar;
                    ++عدد_Substitutions;
                } ورنہ {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *p_Error_رمز = U_INVALID_CHAR_ملا;
                    واپس خالی_اشاریہ;
                }

                length = U8_لمبائی(ch);
                اگر((p_Dest_حد_بندی - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } ورنہ {
                    req_لمبائی = length;
                    توڑو;
                }
            }
        }
        جب((ch=*pSrc++)!=0) {
            اگر(ch<=0x7f) {
                ++req_لمبائی;
            } ورنہ اگر(ch<=0x7ff) {
                req_لمبائی+=2;
            } ورنہ اگر(!U16_IS_SURROGATE(ch)) {
                req_لمبائی+=3;
            } ورنہ اگر(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                req_لمبائی+=4;
            } ورنہ اگر(subchar>=0) {
                req_لمبائی+=U8_لمبائی(subchar);
                ++عدد_Substitutions;
            } ورنہ {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *p_Error_رمز = U_INVALID_CHAR_ملا;
                واپس خالی_اشاریہ;
            }
        }
    } ورنہ {
        ثابت char16_t *p_Src_حد_بندی = (pSrc!=خالی_اشاریہ)?(pSrc+src_لمبائی):خالی_اشاریہ;
        int32_t count;

        /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
        برائے(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            count = (int32_t)((p_Dest_حد_بندی - pDest) / 3);
            src_لمبائی = (int32_t)(p_Src_حد_بندی - pSrc);
            اگر(count > src_لمبائی) {
                count = src_لمبائی; /* min(remaining dest/3, remaining src) */
            }
            اگر(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                توڑو;
            }
            کرو {
                ch=*pSrc++;
                اگر(ch <= 0x7f) {
                    *pDest++ = (uint8_t)ch;
                } ورنہ اگر(ch <= 0x7ff) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ورنہ اگر(ch <= 0xd7ff || ch >= 0xe000) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ورنہ /* ch is a surrogate */ {
                    /*
                     * We will read two UChars and probably output four bytes,
                     * which we didn't account for with computing count,
                     * so we adjust it here.
                     */
                    اگر(--count == 0) {
                        --pSrc; /* undo ch=*pSrc++ for the lead surrogate */
                        توڑو;  /* recompute count */
                    }

                    اگر(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                        ++pSrc;
                        ch=U16_GET_SUPPLEMENTARY(ch, ch2);

                        /* writing 4 bytes per 2 UChars is ok */
                        *pDest++=(uint8_t)((ch>>18)|0xf0);
                        *pDest++=(uint8_t)(((ch>>12)&0x3f)|0x80);
                        *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                        *pDest++=(uint8_t)((ch&0x3f)|0x80);
                    } ورنہ  {
                        /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                        اگر(subchar>=0) {
                            ch=subchar;
                            ++عدد_Substitutions;
                        } ورنہ {
                            *p_Error_رمز = U_INVALID_CHAR_ملا;
                            واپس خالی_اشاریہ;
                        }

                        /* convert and append*/
                        pDest=_appendUTF8(pDest, ch);
                    }
                }
            } جب(--count > 0);
        }

        جب(pSrc<p_Src_حد_بندی) {
            ch=*pSrc++;
            اگر(ch <= 0x7f) {
                اگر(pDest<p_Dest_حد_بندی) {
                    *pDest++ = (uint8_t)ch;
                } ورنہ {
                    req_لمبائی = 1;
                    توڑو;
                }
            } ورنہ اگر(ch <= 0x7ff) {
                اگر((p_Dest_حد_بندی - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ورنہ {
                    req_لمبائی = 2;
                    توڑو;
                }
            } ورنہ اگر(ch <= 0xd7ff || ch >= 0xe000) {
                اگر((p_Dest_حد_بندی - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ورنہ {
                    req_لمبائی = 3;
                    توڑو;
                }
            } ورنہ /* ch is a surrogate */ {
                int32_t length;

                اگر(U16_IS_SURROGATE_LEAD(ch) && pSrc<p_Src_حد_بندی && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } ورنہ اگر(subchar>=0) {
                    ch=subchar;
                    ++عدد_Substitutions;
                } ورنہ {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *p_Error_رمز = U_INVALID_CHAR_ملا;
                    واپس خالی_اشاریہ;
                }

                length = U8_لمبائی(ch);
                اگر((p_Dest_حد_بندی - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } ورنہ {
                    req_لمبائی = length;
                    توڑو;
                }
            }
        }
        جب(pSrc<p_Src_حد_بندی) {
            ch=*pSrc++;
            اگر(ch<=0x7f) {
                ++req_لمبائی;
            } ورنہ اگر(ch<=0x7ff) {
                req_لمبائی+=2;
            } ورنہ اگر(!U16_IS_SURROGATE(ch)) {
                req_لمبائی+=3;
            } ورنہ اگر(U16_IS_SURROGATE_LEAD(ch) && pSrc<p_Src_حد_بندی && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                req_لمبائی+=4;
            } ورنہ اگر(subchar>=0) {
                req_لمبائی+=U8_لمبائی(subchar);
                ++عدد_Substitutions;
            } ورنہ {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *p_Error_رمز = U_INVALID_CHAR_ملا;
                واپس خالی_اشاریہ;
            }
        }
    }

    req_لمبائی+=(int32_t)(pDest - (uint8_t *)dest);

    اگر(p_عدد_Substitutions!=خالی_اشاریہ) {
        *p_عدد_Substitutions=عدد_Substitutions;
    }

    اگر(p_Dest_لمبائی){
        *p_Dest_لمبائی = req_لمبائی;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, dest_استعداد, req_لمبائی, p_Error_رمز);
    واپس dest;
}

U_CAPI حرف* U_EXPORT2
u_str_مقصد_UTF8(حرف* dest U_LIFETIME_BOUND,
            int32_t dest_استعداد,
            int32_t* p_Dest_لمبائی,
            ثابت char16_t* pSrc,
            int32_t src_لمبائی,
            UError_رمز* p_Error_رمز) {
    واپس u_str_مقصد_UTF8_With_Sub(
            dest, dest_استعداد, p_Dest_لمبائی,
            pSrc, src_لمبائی,
            U_SENTINEL, خالی_اشاریہ,
            p_Error_رمز);
}

U_CAPI char16_t* U_EXPORT2
u_strFromJavaModifiedUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                                 int32_t dest_استعداد,
                                 int32_t* p_Dest_لمبائی,
                                 ثابت حرف* src,
                                 int32_t src_لمبائی,
                                 UChar32 subchar,
                                 int32_t* p_عدد_Substitutions,
                                 UError_رمز* p_Error_رمز) {
    /* args check */
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    اگر( (src==خالی_اشاریہ && src_لمبائی!=0) || src_لمبائی < -1 ||
        (dest==خالی_اشاریہ && dest_استعداد!=0) || dest_استعداد<0 ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    اگر(p_عدد_Substitutions!=خالی_اشاریہ) {
        *p_عدد_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *p_Dest_حد_بندی = dest+dest_استعداد;
    int32_t req_لمبائی = 0;
    int32_t عدد_Substitutions=0;

    اگر(src_لمبائی < 0) {
        /*
         * Transform a NUL-terminated ASCII string.
         * Handle non-ASCII strings with slower code.
         */
        UChar32 c;
        جب(((c = (uint8_t)*src) != 0) && c <= 0x7f && (pDest < p_Dest_حد_بندی)) {
            *pDest++=(char16_t)c;
            ++src;
        }
        اگر(c == 0) {
            req_لمبائی=(int32_t)(pDest - dest);
            اگر(p_Dest_لمبائی) {
                *p_Dest_لمبائی = req_لمبائی;
            }

            /* Terminate the buffer */
            u_terminateUChars(dest, dest_استعداد, req_لمبائی, p_Error_رمز);
            واپس dest;
        }
        src_لمبائی = static_cast<int32_t>(uprv_strlen(src));
    }

    /* Faster loop without ongoing checking for srcLength and pDestLimit. */
    UChar32 ch;
    uint8_t t1, t2;
    int32_t i = 0;
    برائے(;;) {
        int32_t count = (int32_t)(p_Dest_حد_بندی - pDest);
        int32_t count2 = src_لمبائی - i;
        اگر(count >= count2 && src_لمبائی > 0 && U8_IS_SINGLE(*src)) {
            /* fast ASCII loop */
            int32_t آغاز = i;
            uint8_t b;
            جب(i < src_لمبائی && U8_IS_SINGLE(b = src[i])) {
                *pDest++=b;
                ++i;
            }
            int32_t delta = i - آغاز;
            count -= delta;
            count2 -= delta;
        }
        /*
         * Each iteration of the inner loop progresses by at most 3 UTF-8
         * bytes and one char16_t.
         */
        اگر(subchar > 0xFFFF) {
            توڑو;
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
            توڑو;
        }
        کرو {
            ch = (uint8_t)src[i++];
            اگر(U8_IS_SINGLE(ch)) {
                *pDest++=(char16_t)ch;
            } ورنہ {
                اگر(ch >= 0xe0) {
                    اگر( /* handle U+0000..U+FFFF inline */
                        ch <= 0xef &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                        (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                    ) {
                        /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                        *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                        i += 2;
                        جاری;
                    }
                } ورنہ {
                    اگر( /* handle U+0000..U+07FF inline */
                        ch >= 0xc0 &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                    ) {
                        *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                        ++i;
                        جاری;
                    }
                }

                اگر(subchar < 0) {
                    *p_Error_رمز = U_INVALID_CHAR_ملا;
                    واپس خالی_اشاریہ;
                } ورنہ اگر(subchar > 0xffff && --count == 0) {
                    /*
                     * We need to write two UChars, adjusted count for that,
                     * and ran out of space.
                     */
                    --i;  // back out byte ch
                    توڑو;
                } ورنہ {
                    /* function call for error cases */
                    utf8_اگلا_Char_محفوظ_Body((ثابت uint8_t *)src, &(i), src_لمبائی, ch, -1);
                    ++عدد_Substitutions;
                    *(pDest++)=(char16_t)subchar;
                }
            }
        } جب(--count > 0);
    }

    جب(i < src_لمبائی && (pDest < p_Dest_حد_بندی)) {
        ch = (uint8_t)src[i++];
        اگر(U8_IS_SINGLE(ch)){
            *pDest++=(char16_t)ch;
        } ورنہ {
            اگر(ch >= 0xe0) {
                اگر( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < src_لمبائی &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                    (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                ) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                    i += 2;
                    جاری;
                }
            } ورنہ {
                اگر( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < src_لمبائی &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                ) {
                    *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                    ++i;
                    جاری;
                }
            }

            اگر(subchar < 0) {
                *p_Error_رمز = U_INVALID_CHAR_ملا;
                واپس خالی_اشاریہ;
            } ورنہ {
                /* function call for error cases */
                utf8_اگلا_Char_محفوظ_Body((ثابت uint8_t *)src, &(i), src_لمبائی, ch, -1);
                ++عدد_Substitutions;
                اگر(subchar<=0xFFFF) {
                    *(pDest++)=(char16_t)subchar;
                } ورنہ {
                    *(pDest++)=U16_LEAD(subchar);
                    اگر(pDest<p_Dest_حد_بندی) {
                        *(pDest++)=U16_TRAIL(subchar);
                    } ورنہ {
                        req_لمبائی++;
                        توڑو;
                    }
                }
            }
        }
    }

    /* Pre-flight the rest of the string. */
    جب(i < src_لمبائی) {
        ch = (uint8_t)src[i++];
        اگر(U8_IS_SINGLE(ch)) {
            req_لمبائی++;
        } ورنہ {
            اگر(ch >= 0xe0) {
                اگر( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < src_لمبائی &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f &&
                    (uint8_t)(src[i+1] - 0x80) <= 0x3f
                ) {
                    req_لمبائی++;
                    i += 2;
                    جاری;
                }
            } ورنہ {
                اگر( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < src_لمبائی &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f
                ) {
                    req_لمبائی++;
                    ++i;
                    جاری;
                }
            }

            اگر(subchar < 0) {
                *p_Error_رمز = U_INVALID_CHAR_ملا;
                واپس خالی_اشاریہ;
            } ورنہ {
                /* function call for error cases */
                utf8_اگلا_Char_محفوظ_Body((ثابت uint8_t *)src, &(i), src_لمبائی, ch, -1);
                ++عدد_Substitutions;
                req_لمبائی+=U16_لمبائی(ch);
            }
        }
    }

    اگر(p_عدد_Substitutions!=خالی_اشاریہ) {
        *p_عدد_Substitutions=عدد_Substitutions;
    }

    req_لمبائی+=(int32_t)(pDest - dest);
    اگر(p_Dest_لمبائی) {
        *p_Dest_لمبائی = req_لمبائی;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, dest_استعداد, req_لمبائی, p_Error_رمز);
    واپس dest;
}

U_CAPI حرف* U_EXPORT2
u_str_مقصد_Java_Modified_UTF8(حرف* dest U_LIFETIME_BOUND,
                        int32_t dest_استعداد,
                        int32_t* p_Dest_لمبائی,
                        ثابت char16_t* src,
                        int32_t src_لمبائی,
                        UError_رمز* p_Error_رمز) {
    int32_t req_لمبائی=0;
    uint32_t ch=0;
    ثابت char16_t *p_Src_حد_بندی;
    int32_t count;

    /* args check */
    اگر(U_FAILURE(*p_Error_رمز)){
        واپس خالی_اشاریہ;
    }
    اگر( (src==خالی_اشاریہ && src_لمبائی!=0) || src_لمبائی < -1 ||
        (dest==خالی_اشاریہ && dest_استعداد!=0) || dest_استعداد<0
    ) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *p_Dest_حد_بندی = pDest + dest_استعداد;

    اگر(src_لمبائی==-1) {
        /* Convert NUL-terminated ASCII, then find the string length. */
        جب((ch=*src)<=0x7f && ch != 0 && pDest<p_Dest_حد_بندی) {
            *pDest++ = (uint8_t)ch;
            ++src;
        }
        اگر(ch == 0) {
            req_لمبائی=(int32_t)(pDest - (uint8_t *)dest);
            اگر(p_Dest_لمبائی) {
                *p_Dest_لمبائی = req_لمبائی;
            }

            /* Terminate the buffer */
            u_terminateChars(dest, dest_استعداد, req_لمبائی, p_Error_رمز);
            واپس dest;
        }
        src_لمبائی = u_strlen(src);
    }

    /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
    p_Src_حد_بندی = (src!=خالی_اشاریہ)?(src+src_لمبائی):خالی_اشاریہ;
    برائے(;;) {
        count = (int32_t)(p_Dest_حد_بندی - pDest);
        src_لمبائی = (int32_t)(p_Src_حد_بندی - src);
        اگر(count >= src_لمبائی && src_لمبائی > 0 && *src <= 0x7f) {
            /* fast ASCII loop */
            ثابت char16_t *پچھلا_Src = src;
            int32_t delta;
            جب(src < p_Src_حد_بندی && (ch = *src) <= 0x7f && ch != 0) {
                *pDest++=(uint8_t)ch;
                ++src;
            }
            delta = (int32_t)(src - پچھلا_Src);
            count -= delta;
            src_لمبائی -= delta;
        }
        /*
         * Each iteration of the inner loop progresses by at most 3 UTF-8
         * bytes and one char16_t.
         */
        count /= 3;
        اگر(count > src_لمبائی) {
            count = src_لمبائی; /* min(remaining dest/3, remaining src) */
        }
        اگر(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            توڑو;
        }
        کرو {
            ch=*src++;
            اگر(ch <= 0x7f && ch != 0) {
                *pDest++ = (uint8_t)ch;
            } ورنہ اگر(ch <= 0x7ff) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } ورنہ {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            }
        } جب(--count > 0);
    }

    جب(src<p_Src_حد_بندی) {
        ch=*src++;
        اگر(ch <= 0x7f && ch != 0) {
            اگر(pDest<p_Dest_حد_بندی) {
                *pDest++ = (uint8_t)ch;
            } ورنہ {
                req_لمبائی = 1;
                توڑو;
            }
        } ورنہ اگر(ch <= 0x7ff) {
            اگر((p_Dest_حد_بندی - pDest) >= 2) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } ورنہ {
                req_لمبائی = 2;
                توڑو;
            }
        } ورنہ {
            اگر((p_Dest_حد_بندی - pDest) >= 3) {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } ورنہ {
                req_لمبائی = 3;
                توڑو;
            }
        }
    }
    جب(src<p_Src_حد_بندی) {
        ch=*src++;
        اگر(ch <= 0x7f && ch != 0) {
            ++req_لمبائی;
        } ورنہ اگر(ch<=0x7ff) {
            req_لمبائی+=2;
        } ورنہ {
            req_لمبائی+=3;
        }
    }

    req_لمبائی+=(int32_t)(pDest - (uint8_t *)dest);
    اگر(p_Dest_لمبائی){
        *p_Dest_لمبائی = req_لمبائی;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, dest_استعداد, req_لمبائی, p_Error_رمز);
    واپس dest;
}
