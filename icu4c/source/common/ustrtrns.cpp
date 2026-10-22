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


#ضمن "unicode/putil.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/utf.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"
#ضمن "ustr_imp.h"
#ضمن "uassert.h"

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32WithSub(char16_t* dest U_LIFETIME_BOUND,
                      int32_t dest_ظرفیت,
                      int32_t* p_Dest_اوږدتیا,
                      ثابت UChar32* src,
                      int32_t src_اوږدتیا,
                      UChar32 subchar,
                      int32_t* p_عدد_Substitutions,
                      UError_کوډ* p_Error_کوډ) {
    ثابت UChar32 *src_حد;
    UChar32 ch;
    char16_t *dest_حد;
    char16_t *pDest;
    int32_t req_اوږدتیا;
    int32_t عدد_Substitutions;

    /* args check */
    إذا(U_FAILURE(*p_Error_کوډ)){
        أرجع تش_اشاره;
    }
    إذا( (src==تش_اشاره && src_اوږدتیا!=0) || src_اوږدتیا < -1 ||
        (dest_ظرفیت<0) || (dest == تش_اشاره && dest_ظرفیت > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    إذا(p_عدد_Substitutions != تش_اشاره) {
        *p_عدد_Substitutions = 0;
    }

    pDest = dest;
    dest_حد = (dest!=تش_اشاره)?(dest + dest_ظرفیت):تش_اشاره;
    req_اوږدتیا = 0;
    عدد_Substitutions = 0;

    إذا(src_اوږدتیا < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        طالما((ch=*src) != 0 &&
              ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff))) {
            ++src;
            إذا(pDest < dest_حد) {
                *pDest++ = (char16_t)ch;
            } وإلا {
                ++req_اوږدتیا;
            }
        }
        src_حد = src;
        إذا(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            طالما(*++src_حد != 0) {}
        }
    } وإلا {
      src_حد = (src!=تش_اشاره)?(src + src_اوږدتیا):تش_اشاره;
    }

    /* convert with length */
    طالما(src < src_حد) {
        ch = *src++;
        افعل {
            /* usually "loops" once; twice only for writing subchar */
            إذا((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff)) {
                إذا(pDest < dest_حد) {
                    *pDest++ = (char16_t)ch;
                } وإلا {
                    ++req_اوږدتیا;
                }
                اكسر;
            } وإلا إذا(0x10000 <= ch && ch <= 0x10ffff) {
                إذا(pDest!=تش_اشاره && ((pDest + 2) <= dest_حد)) {
                    *pDest++ = U16_LEAD(ch);
                    *pDest++ = U16_TRAIL(ch);
                } وإلا {
                    req_اوږدتیا += 2;
                }
                اكسر;
            } وإلا إذا((ch = subchar) < 0) {
                /* surrogate code point, or not a Unicode code point at all */
                *p_Error_کوډ = U_INVALID_CHAR_وموندل;
                أرجع تش_اشاره;
            } وإلا {
                ++عدد_Substitutions;
            }
        } طالما(ریښتیا);
    }

    req_اوږدتیا += (int32_t)(pDest - dest);
    إذا(p_Dest_اوږدتیا) {
        *p_Dest_اوږدتیا = req_اوږدتیا;
    }
    إذا(p_عدد_Substitutions != تش_اشاره) {
        *p_عدد_Substitutions = عدد_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, dest_ظرفیت, req_اوږدتیا, p_Error_کوډ);
    
    أرجع dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32(char16_t* dest U_LIFETIME_BOUND,
               int32_t dest_ظرفیت,
               int32_t* p_Dest_اوږدتیا,
               ثابت UChar32* src,
               int32_t src_اوږدتیا,
               UError_کوډ* p_Error_کوډ) {
    أرجع u_strFromUTF32WithSub(
            dest, dest_ظرفیت, p_Dest_اوږدتیا,
            src, src_اوږدتیا,
            U_SENTINEL, تش_اشاره,
            p_Error_کوډ);
}

U_CAPI UChar32* U_EXPORT2
u_str_منزل_UTF32_With_Sub(UChar32* dest U_LIFETIME_BOUND,
                    int32_t dest_ظرفیت,
                    int32_t* p_Dest_اوږدتیا,
                    ثابت char16_t* src,
                    int32_t src_اوږدتیا,
                    UChar32 subchar,
                    int32_t* p_عدد_Substitutions,
                    UError_کوډ* p_Error_کوډ) {
    ثابت char16_t *src_حد;
    UChar32 ch;
    char16_t ch2;
    UChar32 *dest_حد;
    UChar32 *pDest;
    int32_t req_اوږدتیا;
    int32_t عدد_Substitutions;

    /* args check */
    إذا(U_FAILURE(*p_Error_کوډ)){
        أرجع تش_اشاره;
    }
    إذا( (src==تش_اشاره && src_اوږدتیا!=0) || src_اوږدتیا < -1 ||
        (dest_ظرفیت<0) || (dest == تش_اشاره && dest_ظرفیت > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    إذا(p_عدد_Substitutions != تش_اشاره) {
        *p_عدد_Substitutions = 0;
    }

    pDest = dest;
    dest_حد = (dest!=تش_اشاره)?(dest + dest_ظرفیت):تش_اشاره;
    req_اوږدتیا = 0;
    عدد_Substitutions = 0;

    إذا(src_اوږدتیا < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        طالما((ch=*src) != 0 && !U16_IS_SURROGATE(ch)) {
            ++src;
            إذا(pDest < dest_حد) {
                *pDest++ = ch;
            } وإلا {
                ++req_اوږدتیا;
            }
        }
        src_حد = src;
        إذا(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            طالما(*++src_حد != 0) {}
        }
    } وإلا {
        src_حد = (src!=تش_اشاره)?(src + src_اوږدتیا):تش_اشاره;
    }

    /* convert with length */
    طالما(src < src_حد) {
        ch = *src++;
        إذا(!U16_IS_SURROGATE(ch)) {
            /* write or count ch below */
        } وإلا إذا(U16_IS_SURROGATE_LEAD(ch) && src < src_حد && U16_IS_TRAIL(ch2 = *src)) {
            ++src;
            ch = U16_GET_SUPPLEMENTARY(ch, ch2);
        } وإلا إذا((ch = subchar) < 0) {
            /* unpaired surrogate */
            *p_Error_کوډ = U_INVALID_CHAR_وموندل;
            أرجع تش_اشاره;
        } وإلا {
            ++عدد_Substitutions;
        }
        إذا(pDest < dest_حد) {
            *pDest++ = ch;
        } وإلا {
            ++req_اوږدتیا;
        }
    }

    req_اوږدتیا += (int32_t)(pDest - dest);
    إذا(p_Dest_اوږدتیا) {
        *p_Dest_اوږدتیا = req_اوږدتیا;
    }
    إذا(p_عدد_Substitutions != تش_اشاره) {
        *p_عدد_Substitutions = عدد_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChar32s(dest, dest_ظرفیت, req_اوږدتیا, p_Error_کوډ);

    أرجع dest;
}

U_CAPI UChar32* U_EXPORT2
u_str_منزل_UTF32(UChar32* dest U_LIFETIME_BOUND,
             int32_t dest_ظرفیت,
             int32_t* p_Dest_اوږدتیا,
             ثابت char16_t* src,
             int32_t src_اوږدتیا,
             UError_کوډ* p_Error_کوډ) {
    أرجع u_str_منزل_UTF32_With_Sub(
            dest, dest_ظرفیت, p_Dest_اوږدتیا,
            src, src_اوږدتیا,
            U_SENTINEL, تش_اشاره,
            p_Error_کوډ);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                     int32_t dest_ظرفیت,
                     int32_t* p_Dest_اوږدتیا,
                     ثابت حرف* src,
                     int32_t src_اوږدتیا,
                     UChar32 subchar,
                     int32_t* p_عدد_Substitutions,
                     UError_کوډ* p_Error_کوډ) {
    /* args check */
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    إذا( (src==تش_اشاره && src_اوږدتیا!=0) || src_اوږدتیا < -1 ||
        (dest_ظرفیت<0) || (dest == تش_اشاره && dest_ظرفیت > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    إذا(p_عدد_Substitutions!=تش_اشاره) {
        *p_عدد_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *p_Dest_حد = dest+dest_ظرفیت;
    int32_t req_اوږدتیا = 0;
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

    إذا(src_اوږدتیا < 0){
        /*
         * Transform a NUL-terminated string.
         * The code explicitly checks for NULs only in the lead byte position.
         * A NUL byte in the trail byte position fails the trail byte range check anyway.
         */
        int32_t i;
        UChar32 c;
        لكل(i = 0; (c = (uint8_t)src[i]) != 0 && (pDest < p_Dest_حد);) {
            // modified copy of U8_NEXT()
            ++i;
            إذا(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } وإلا {
                uint8_t __t1, __t2;
                إذا( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } وإلا إذا( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } وإلا {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_بل_Char_خوندي_Body((ثابت uint8_t *)src, &(i), -1, c, -1);
                    إذا(c<0 && (++عدد_Substitutions, c = subchar) < 0) {
                        *p_Error_کوډ = U_INVALID_CHAR_وموندل;
                        أرجع تش_اشاره;
                    } وإلا إذا(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } وإلا {
                        *(pDest++)=U16_LEAD(c);
                        إذا(pDest<p_Dest_حد) {
                            *(pDest++)=U16_TRAIL(c);
                        } وإلا {
                            req_اوږدتیا++;
                            اكسر;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        طالما((c = (uint8_t)src[i]) != 0) {
            // modified copy of U8_NEXT()
            ++i;
            إذا(U8_IS_SINGLE(c)) {
                ++req_اوږدتیا;
            } وإلا {
                uint8_t __t1, __t2;
                إذا( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++req_اوږدتیا;
                    i+=2;
                } وإلا إذا( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++req_اوږدتیا;
                    ++(i);
                } وإلا {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_بل_Char_خوندي_Body((ثابت uint8_t *)src, &(i), -1, c, -1);
                    إذا(c<0 && (++عدد_Substitutions, c = subchar) < 0) {
                        *p_Error_کوډ = U_INVALID_CHAR_وموندل;
                        أرجع تش_اشاره;
                    }
                    req_اوږدتیا += U16_اوږدتیا(c);
                }
            }
        }
    } وإلا /* srcLength >= 0 */ {
        /* Faster loop without ongoing checking for srcLength and pDestLimit. */
        int32_t i = 0;
        UChar32 c;
        لكل(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            int32_t count = (int32_t)(p_Dest_حد - pDest);
            int32_t count2 = (src_اوږدتیا - i) / 3;
            إذا(count > count2) {
                count = count2; /* min(remaining dest, remaining src/3) */
            }
            إذا(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                اكسر;
            }

            افعل {
                // modified copy of U8_NEXT()
                c = (uint8_t)src[i++];
                إذا(U8_IS_SINGLE(c)) {
                    *pDest++=(char16_t)c;
                } وإلا {
                    uint8_t __t1, __t2;
                    إذا( /* handle U+0800..U+FFFF inline */
                            (0xe0<=(c) && (c)<0xf0) &&
                            ((i)+1)<src_اوږدتیا &&
                            U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                            (__t2=src[(i)+1]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                        i+=2;
                    } وإلا إذا( /* handle U+0080..U+07FF inline */
                            ((c)<0xe0 && (c)>=0xc2) &&
                            ((i)!=src_اوږدتیا) &&
                            (__t1=src[i]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0x1f)<<6)|__t1;
                        ++(i);
                    } وإلا {
                        إذا(c >= 0xf0 || subchar > 0xffff) {
                            // We may read up to four bytes and write up to two UChars,
                            // which we didn't account for with computing count,
                            // so we adjust it here.
                            إذا(--count == 0) {
                                --i;  // back out byte c
                                اكسر;
                            }
                        }

                        /* function call for "complicated" and error cases */
                        (c)=utf8_بل_Char_خوندي_Body((ثابت uint8_t *)src, &(i), src_اوږدتیا, c, -1);
                        إذا(c<0 && (++عدد_Substitutions, c = subchar) < 0) {
                            *p_Error_کوډ = U_INVALID_CHAR_وموندل;
                            أرجع تش_اشاره;
                        } وإلا إذا(c<=0xFFFF) {
                            *(pDest++)=(char16_t)c;
                        } وإلا {
                            *(pDest++)=U16_LEAD(c);
                            *(pDest++)=U16_TRAIL(c);
                        }
                    }
                }
            } طالما(--count > 0);
        }

        طالما(i < src_اوږدتیا && (pDest < p_Dest_حد)) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            إذا(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } وإلا {
                uint8_t __t1, __t2;
                إذا( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<src_اوږدتیا &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } وإلا إذا( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=src_اوږدتیا) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } وإلا {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_بل_Char_خوندي_Body((ثابت uint8_t *)src, &(i), src_اوږدتیا, c, -1);
                    إذا(c<0 && (++عدد_Substitutions, c = subchar) < 0) {
                        *p_Error_کوډ = U_INVALID_CHAR_وموندل;
                        أرجع تش_اشاره;
                    } وإلا إذا(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } وإلا {
                        *(pDest++)=U16_LEAD(c);
                        إذا(pDest<p_Dest_حد) {
                            *(pDest++)=U16_TRAIL(c);
                        } وإلا {
                            req_اوږدتیا++;
                            اكسر;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        طالما(i < src_اوږدتیا) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            إذا(U8_IS_SINGLE(c)) {
                ++req_اوږدتیا;
            } وإلا {
                uint8_t __t1, __t2;
                إذا( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<src_اوږدتیا &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++req_اوږدتیا;
                    i+=2;
                } وإلا إذا( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=src_اوږدتیا) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++req_اوږدتیا;
                    ++(i);
                } وإلا {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_بل_Char_خوندي_Body((ثابت uint8_t *)src, &(i), src_اوږدتیا, c, -1);
                    إذا(c<0 && (++عدد_Substitutions, c = subchar) < 0) {
                        *p_Error_کوډ = U_INVALID_CHAR_وموندل;
                        أرجع تش_اشاره;
                    }
                    req_اوږدتیا += U16_اوږدتیا(c);
                }
            }
        }
    }

    req_اوږدتیا+=(int32_t)(pDest - dest);

    إذا(p_عدد_Substitutions!=تش_اشاره) {
        *p_عدد_Substitutions=عدد_Substitutions;
    }

    إذا(p_Dest_اوږدتیا){
        *p_Dest_اوږدتیا = req_اوږدتیا;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_ظرفیت,req_اوږدتیا,p_Error_کوډ);

    أرجع dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8(char16_t* dest U_LIFETIME_BOUND,
              int32_t dest_ظرفیت,
              int32_t* p_Dest_اوږدتیا,
              ثابت حرف* src,
              int32_t src_اوږدتیا,
              UError_کوډ* p_Error_کوډ) {
    أرجع u_strFromUTF8WithSub(
            dest, dest_ظرفیت, p_Dest_اوږدتیا,
            src, src_اوږدتیا,
            U_SENTINEL, تش_اشاره,
            p_Error_کوډ);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8Lenient(char16_t* dest U_LIFETIME_BOUND,
                     int32_t dest_ظرفیت,
                     int32_t* p_Dest_اوږدتیا,
                     ثابت حرف* src,
                     int32_t src_اوږدتیا,
                     UError_کوډ* p_Error_کوډ) {
    char16_t *pDest = dest;
    UChar32 ch;
    int32_t req_اوږدتیا = 0;
    uint8_t* pSrc = (uint8_t*) src;

    /* args check */
    إذا(U_FAILURE(*p_Error_کوډ)){
        أرجع تش_اشاره;
    }
        
    إذا( (src==تش_اشاره && src_اوږدتیا!=0) || src_اوږدتیا < -1 ||
        (dest_ظرفیت<0) || (dest == تش_اشاره && dest_ظرفیت > 0)
    ) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    إذا(src_اوږدتیا < 0) {
        /* Transform a NUL-terminated string. */
        char16_t *p_Dest_حد = (dest!=تش_اشاره)?(dest+dest_ظرفیت):تش_اشاره;
        uint8_t t1, t2, t3; /* trail bytes */

        طالما(((ch = *pSrc) != 0) && (pDest < p_Dest_حد)) {
            إذا(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ++pSrc;
                استمر;
            } وإلا إذا(ch < 0xe0) { /* U+0080..U+07FF */
                إذا((t1 = pSrc[1]) != 0) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + t1 - 0x3080);
                    pSrc += 2;
                    استمر;
                }
            } وإلا إذا(ch < 0xf0) { /* U+0800..U+FFFF */
                إذا((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 12) + (t1 << 6) + t2 - 0x2080);
                    pSrc += 3;
                    استمر;
                }
            } وإلا /* f0..f4 */ { /* U+10000..U+10FFFF */
                إذا((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0 && (t3 = pSrc[3]) != 0) {
                    pSrc += 4;
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (t1 << 12) + (t2 << 6) + t3 - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    إذا(pDest < p_Dest_حد) {
                        *(pDest++) = U16_TRAIL(ch);
                    } وإلا {
                        req_اوږدتیا = 1;
                        اكسر;
                    }
                    استمر;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            طالما(*++pSrc != 0) {}
            اكسر;
        }

        /* Pre-flight the rest of the string. */
        طالما((ch = *pSrc) != 0) {
            إذا(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                ++req_اوږدتیا;
                ++pSrc;
                استمر;
            } وإلا إذا(ch < 0xe0) { /* U+0080..U+07FF */
                إذا(pSrc[1] != 0) {
                    ++req_اوږدتیا;
                    pSrc += 2;
                    استمر;
                }
            } وإلا إذا(ch < 0xf0) { /* U+0800..U+FFFF */
                إذا(pSrc[1] != 0 && pSrc[2] != 0) {
                    ++req_اوږدتیا;
                    pSrc += 3;
                    استمر;
                }
            } وإلا /* f0..f4 */ { /* U+10000..U+10FFFF */
                إذا(pSrc[1] != 0 && pSrc[2] != 0 && pSrc[3] != 0) {
                    req_اوږدتیا += 2;
                    pSrc += 4;
                    استمر;
                }
            }

            /* truncated character at the end */
            ++req_اوږدتیا;
            اكسر;
        }
    } وإلا /* srcLength >= 0 */ {
      ثابت uint8_t *p_Src_حد = (pSrc!=تش_اشاره)?(pSrc + src_اوږدتیا):تش_اشاره;

        /*
         * This function requires that if srcLength is given, then it must be
         * destCapatity >= srcLength so that we need not check for
         * destination buffer overflow in the loop.
         */
        إذا(dest_ظرفیت < src_اوږدتیا) {
            إذا(p_Dest_اوږدتیا != تش_اشاره) {
                *p_Dest_اوږدتیا = src_اوږدتیا; /* this likely overestimates the true destLength! */
            }
            *p_Error_کوډ = U_بفر_ډکېدنه_ERROR;
            أرجع تش_اشاره;
        }

        إذا((p_Src_حد - pSrc) >= 4) {
            p_Src_حد -= 3; /* temporarily reduce pSrcLimit */

            /* in this loop, we can always access at least 4 bytes, up to pSrc+3 */
            افعل {
                ch = *pSrc++;
                إذا(ch < 0xc0) {
                    /*
                     * ASCII, or a trail byte in lead position which is treated like
                     * a single-byte sequence for better character boundary
                     * resynchronization after illegal sequences.
                     */
                    *pDest++=(char16_t)ch;
                } وإلا إذا(ch < 0xe0) { /* U+0080..U+07FF */
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                } وإلا إذا(ch < 0xf0) { /* U+0800..U+FFFF */
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                } وإلا /* f0..f4 */ { /* U+10000..U+10FFFF */
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                }
            } طالما(pSrc < p_Src_حد);

            p_Src_حد += 3; /* restore original pSrcLimit */
        }

        طالما(pSrc < p_Src_حد) {
            ch = *pSrc++;
            إذا(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                استمر;
            } وإلا إذا(ch < 0xe0) { /* U+0080..U+07FF */
                إذا(pSrc < p_Src_حد) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                    استمر;
                }
            } وإلا إذا(ch < 0xf0) { /* U+0800..U+FFFF */
                إذا((p_Src_حد - pSrc) >= 2) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                    pSrc += 3;
                    استمر;
                }
            } وإلا /* f0..f4 */ { /* U+10000..U+10FFFF */
                إذا((p_Src_حد - pSrc) >= 3) {
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                    pSrc += 4;
                    استمر;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            اكسر;
        }
    }

    req_اوږدتیا+=(int32_t)(pDest - dest);

    إذا(p_Dest_اوږدتیا){
        *p_Dest_اوږدتیا = req_اوږدتیا;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_ظرفیت,req_اوږدتیا,p_Error_کوډ);

    أرجع dest;
}

ساكن مضمن uint8_t *
_appendUTF8(uint8_t *pDest, UChar32 c) {
    /* it is 0<=c<=0x10ffff and not a surrogate if called by a validating function */
    إذا((c)<=0x7f) {
        *pDest++ = static_cast<uint8_t>(c);
    } وإلا إذا(c<=0x7ff) {
        *pDest++ = static_cast<uint8_t>((c >> 6) | 0xc0);
        *pDest++ = static_cast<uint8_t>((c & 0x3f) | 0x80);
    } وإلا إذا(c<=0xffff) {
        *pDest++ = static_cast<uint8_t>((c >> 12) | 0xe0);
        *pDest++ = static_cast<uint8_t>(((c >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    } وإلا /* if((uint32_t)(c)<=0x10ffff) */ {
        *pDest++ = static_cast<uint8_t>(((c) >> 18) | 0xf0);
        *pDest++ = static_cast<uint8_t>((((c) >> 12) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>((((c) >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    }
    أرجع pDest;
}

U_CAPI حرف* U_EXPORT2
u_str_منزل_UTF8_With_Sub(حرف* dest U_LIFETIME_BOUND,
                   int32_t dest_ظرفیت,
                   int32_t* p_Dest_اوږدتیا,
                   ثابت char16_t* pSrc,
                   int32_t src_اوږدتیا,
                   UChar32 subchar,
                   int32_t* p_عدد_Substitutions,
                   UError_کوډ* p_Error_کوډ) {
    int32_t req_اوږدتیا=0;
    uint32_t ch=0,ch2=0;
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *p_Dest_حد = (pDest!=تش_اشاره)?(pDest + dest_ظرفیت):تش_اشاره;
    int32_t عدد_Substitutions;

    /* args check */
    إذا(U_FAILURE(*p_Error_کوډ)){
        أرجع تش_اشاره;
    }
        
    إذا( (pSrc==تش_اشاره && src_اوږدتیا!=0) || src_اوږدتیا < -1 ||
        (dest_ظرفیت<0) || (dest == تش_اشاره && dest_ظرفیت > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    إذا(p_عدد_Substitutions!=تش_اشاره) {
        *p_عدد_Substitutions=0;
    }
    عدد_Substitutions=0;

    إذا(src_اوږدتیا==-1) {
        طالما((ch=*pSrc)!=0) {
            ++pSrc;
            إذا(ch <= 0x7f) {
                إذا(pDest<p_Dest_حد) {
                    *pDest++ = (uint8_t)ch;
                } وإلا {
                    req_اوږدتیا = 1;
                    اكسر;
                }
            } وإلا إذا(ch <= 0x7ff) {
                إذا((p_Dest_حد - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } وإلا {
                    req_اوږدتیا = 2;
                    اكسر;
                }
            } وإلا إذا(ch <= 0xd7ff || ch >= 0xe000) {
                إذا((p_Dest_حد - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } وإلا {
                    req_اوږدتیا = 3;
                    اكسر;
                }
            } وإلا /* ch is a surrogate */ {
                int32_t length;

                /*need not check for NUL because NUL fails U16_IS_TRAIL() anyway*/
                إذا(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } وإلا إذا(subchar>=0) {
                    ch=subchar;
                    ++عدد_Substitutions;
                } وإلا {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *p_Error_کوډ = U_INVALID_CHAR_وموندل;
                    أرجع تش_اشاره;
                }

                length = U8_اوږدتیا(ch);
                إذا((p_Dest_حد - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } وإلا {
                    req_اوږدتیا = length;
                    اكسر;
                }
            }
        }
        طالما((ch=*pSrc++)!=0) {
            إذا(ch<=0x7f) {
                ++req_اوږدتیا;
            } وإلا إذا(ch<=0x7ff) {
                req_اوږدتیا+=2;
            } وإلا إذا(!U16_IS_SURROGATE(ch)) {
                req_اوږدتیا+=3;
            } وإلا إذا(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                req_اوږدتیا+=4;
            } وإلا إذا(subchar>=0) {
                req_اوږدتیا+=U8_اوږدتیا(subchar);
                ++عدد_Substitutions;
            } وإلا {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *p_Error_کوډ = U_INVALID_CHAR_وموندل;
                أرجع تش_اشاره;
            }
        }
    } وإلا {
        ثابت char16_t *p_Src_حد = (pSrc!=تش_اشاره)?(pSrc+src_اوږدتیا):تش_اشاره;
        int32_t count;

        /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
        لكل(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            count = (int32_t)((p_Dest_حد - pDest) / 3);
            src_اوږدتیا = (int32_t)(p_Src_حد - pSrc);
            إذا(count > src_اوږدتیا) {
                count = src_اوږدتیا; /* min(remaining dest/3, remaining src) */
            }
            إذا(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                اكسر;
            }
            افعل {
                ch=*pSrc++;
                إذا(ch <= 0x7f) {
                    *pDest++ = (uint8_t)ch;
                } وإلا إذا(ch <= 0x7ff) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } وإلا إذا(ch <= 0xd7ff || ch >= 0xe000) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } وإلا /* ch is a surrogate */ {
                    /*
                     * We will read two UChars and probably output four bytes,
                     * which we didn't account for with computing count,
                     * so we adjust it here.
                     */
                    إذا(--count == 0) {
                        --pSrc; /* undo ch=*pSrc++ for the lead surrogate */
                        اكسر;  /* recompute count */
                    }

                    إذا(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                        ++pSrc;
                        ch=U16_GET_SUPPLEMENTARY(ch, ch2);

                        /* writing 4 bytes per 2 UChars is ok */
                        *pDest++=(uint8_t)((ch>>18)|0xf0);
                        *pDest++=(uint8_t)(((ch>>12)&0x3f)|0x80);
                        *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                        *pDest++=(uint8_t)((ch&0x3f)|0x80);
                    } وإلا  {
                        /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                        إذا(subchar>=0) {
                            ch=subchar;
                            ++عدد_Substitutions;
                        } وإلا {
                            *p_Error_کوډ = U_INVALID_CHAR_وموندل;
                            أرجع تش_اشاره;
                        }

                        /* convert and append*/
                        pDest=_appendUTF8(pDest, ch);
                    }
                }
            } طالما(--count > 0);
        }

        طالما(pSrc<p_Src_حد) {
            ch=*pSrc++;
            إذا(ch <= 0x7f) {
                إذا(pDest<p_Dest_حد) {
                    *pDest++ = (uint8_t)ch;
                } وإلا {
                    req_اوږدتیا = 1;
                    اكسر;
                }
            } وإلا إذا(ch <= 0x7ff) {
                إذا((p_Dest_حد - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } وإلا {
                    req_اوږدتیا = 2;
                    اكسر;
                }
            } وإلا إذا(ch <= 0xd7ff || ch >= 0xe000) {
                إذا((p_Dest_حد - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } وإلا {
                    req_اوږدتیا = 3;
                    اكسر;
                }
            } وإلا /* ch is a surrogate */ {
                int32_t length;

                إذا(U16_IS_SURROGATE_LEAD(ch) && pSrc<p_Src_حد && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } وإلا إذا(subchar>=0) {
                    ch=subchar;
                    ++عدد_Substitutions;
                } وإلا {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *p_Error_کوډ = U_INVALID_CHAR_وموندل;
                    أرجع تش_اشاره;
                }

                length = U8_اوږدتیا(ch);
                إذا((p_Dest_حد - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } وإلا {
                    req_اوږدتیا = length;
                    اكسر;
                }
            }
        }
        طالما(pSrc<p_Src_حد) {
            ch=*pSrc++;
            إذا(ch<=0x7f) {
                ++req_اوږدتیا;
            } وإلا إذا(ch<=0x7ff) {
                req_اوږدتیا+=2;
            } وإلا إذا(!U16_IS_SURROGATE(ch)) {
                req_اوږدتیا+=3;
            } وإلا إذا(U16_IS_SURROGATE_LEAD(ch) && pSrc<p_Src_حد && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                req_اوږدتیا+=4;
            } وإلا إذا(subchar>=0) {
                req_اوږدتیا+=U8_اوږدتیا(subchar);
                ++عدد_Substitutions;
            } وإلا {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *p_Error_کوډ = U_INVALID_CHAR_وموندل;
                أرجع تش_اشاره;
            }
        }
    }

    req_اوږدتیا+=(int32_t)(pDest - (uint8_t *)dest);

    إذا(p_عدد_Substitutions!=تش_اشاره) {
        *p_عدد_Substitutions=عدد_Substitutions;
    }

    إذا(p_Dest_اوږدتیا){
        *p_Dest_اوږدتیا = req_اوږدتیا;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, dest_ظرفیت, req_اوږدتیا, p_Error_کوډ);
    أرجع dest;
}

U_CAPI حرف* U_EXPORT2
u_str_منزل_UTF8(حرف* dest U_LIFETIME_BOUND,
            int32_t dest_ظرفیت,
            int32_t* p_Dest_اوږدتیا,
            ثابت char16_t* pSrc,
            int32_t src_اوږدتیا,
            UError_کوډ* p_Error_کوډ) {
    أرجع u_str_منزل_UTF8_With_Sub(
            dest, dest_ظرفیت, p_Dest_اوږدتیا,
            pSrc, src_اوږدتیا,
            U_SENTINEL, تش_اشاره,
            p_Error_کوډ);
}

U_CAPI char16_t* U_EXPORT2
u_strFromJavaModifiedUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                                 int32_t dest_ظرفیت,
                                 int32_t* p_Dest_اوږدتیا,
                                 ثابت حرف* src,
                                 int32_t src_اوږدتیا,
                                 UChar32 subchar,
                                 int32_t* p_عدد_Substitutions,
                                 UError_کوډ* p_Error_کوډ) {
    /* args check */
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    إذا( (src==تش_اشاره && src_اوږدتیا!=0) || src_اوږدتیا < -1 ||
        (dest==تش_اشاره && dest_ظرفیت!=0) || dest_ظرفیت<0 ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    إذا(p_عدد_Substitutions!=تش_اشاره) {
        *p_عدد_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *p_Dest_حد = dest+dest_ظرفیت;
    int32_t req_اوږدتیا = 0;
    int32_t عدد_Substitutions=0;

    إذا(src_اوږدتیا < 0) {
        /*
         * Transform a NUL-terminated ASCII string.
         * Handle non-ASCII strings with slower code.
         */
        UChar32 c;
        طالما(((c = (uint8_t)*src) != 0) && c <= 0x7f && (pDest < p_Dest_حد)) {
            *pDest++=(char16_t)c;
            ++src;
        }
        إذا(c == 0) {
            req_اوږدتیا=(int32_t)(pDest - dest);
            إذا(p_Dest_اوږدتیا) {
                *p_Dest_اوږدتیا = req_اوږدتیا;
            }

            /* Terminate the buffer */
            u_terminateUChars(dest, dest_ظرفیت, req_اوږدتیا, p_Error_کوډ);
            أرجع dest;
        }
        src_اوږدتیا = static_cast<int32_t>(uprv_strlen(src));
    }

    /* Faster loop without ongoing checking for srcLength and pDestLimit. */
    UChar32 ch;
    uint8_t t1, t2;
    int32_t i = 0;
    لكل(;;) {
        int32_t count = (int32_t)(p_Dest_حد - pDest);
        int32_t count2 = src_اوږدتیا - i;
        إذا(count >= count2 && src_اوږدتیا > 0 && U8_IS_SINGLE(*src)) {
            /* fast ASCII loop */
            int32_t پیل = i;
            uint8_t b;
            طالما(i < src_اوږدتیا && U8_IS_SINGLE(b = src[i])) {
                *pDest++=b;
                ++i;
            }
            int32_t delta = i - پیل;
            count -= delta;
            count2 -= delta;
        }
        /*
         * Each iteration of the inner loop progresses by at most 3 UTF-8
         * bytes and one char16_t.
         */
        إذا(subchar > 0xFFFF) {
            اكسر;
        }
        count2 /= 3;
        إذا(count > count2) {
            count = count2; /* min(remaining dest, remaining src/3) */
        }
        إذا(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            اكسر;
        }
        افعل {
            ch = (uint8_t)src[i++];
            إذا(U8_IS_SINGLE(ch)) {
                *pDest++=(char16_t)ch;
            } وإلا {
                إذا(ch >= 0xe0) {
                    إذا( /* handle U+0000..U+FFFF inline */
                        ch <= 0xef &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                        (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                    ) {
                        /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                        *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                        i += 2;
                        استمر;
                    }
                } وإلا {
                    إذا( /* handle U+0000..U+07FF inline */
                        ch >= 0xc0 &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                    ) {
                        *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                        ++i;
                        استمر;
                    }
                }

                إذا(subchar < 0) {
                    *p_Error_کوډ = U_INVALID_CHAR_وموندل;
                    أرجع تش_اشاره;
                } وإلا إذا(subchar > 0xffff && --count == 0) {
                    /*
                     * We need to write two UChars, adjusted count for that,
                     * and ran out of space.
                     */
                    --i;  // back out byte ch
                    اكسر;
                } وإلا {
                    /* function call for error cases */
                    utf8_بل_Char_خوندي_Body((ثابت uint8_t *)src, &(i), src_اوږدتیا, ch, -1);
                    ++عدد_Substitutions;
                    *(pDest++)=(char16_t)subchar;
                }
            }
        } طالما(--count > 0);
    }

    طالما(i < src_اوږدتیا && (pDest < p_Dest_حد)) {
        ch = (uint8_t)src[i++];
        إذا(U8_IS_SINGLE(ch)){
            *pDest++=(char16_t)ch;
        } وإلا {
            إذا(ch >= 0xe0) {
                إذا( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < src_اوږدتیا &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                    (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                ) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                    i += 2;
                    استمر;
                }
            } وإلا {
                إذا( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < src_اوږدتیا &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                ) {
                    *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                    ++i;
                    استمر;
                }
            }

            إذا(subchar < 0) {
                *p_Error_کوډ = U_INVALID_CHAR_وموندل;
                أرجع تش_اشاره;
            } وإلا {
                /* function call for error cases */
                utf8_بل_Char_خوندي_Body((ثابت uint8_t *)src, &(i), src_اوږدتیا, ch, -1);
                ++عدد_Substitutions;
                إذا(subchar<=0xFFFF) {
                    *(pDest++)=(char16_t)subchar;
                } وإلا {
                    *(pDest++)=U16_LEAD(subchar);
                    إذا(pDest<p_Dest_حد) {
                        *(pDest++)=U16_TRAIL(subchar);
                    } وإلا {
                        req_اوږدتیا++;
                        اكسر;
                    }
                }
            }
        }
    }

    /* Pre-flight the rest of the string. */
    طالما(i < src_اوږدتیا) {
        ch = (uint8_t)src[i++];
        إذا(U8_IS_SINGLE(ch)) {
            req_اوږدتیا++;
        } وإلا {
            إذا(ch >= 0xe0) {
                إذا( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < src_اوږدتیا &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f &&
                    (uint8_t)(src[i+1] - 0x80) <= 0x3f
                ) {
                    req_اوږدتیا++;
                    i += 2;
                    استمر;
                }
            } وإلا {
                إذا( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < src_اوږدتیا &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f
                ) {
                    req_اوږدتیا++;
                    ++i;
                    استمر;
                }
            }

            إذا(subchar < 0) {
                *p_Error_کوډ = U_INVALID_CHAR_وموندل;
                أرجع تش_اشاره;
            } وإلا {
                /* function call for error cases */
                utf8_بل_Char_خوندي_Body((ثابت uint8_t *)src, &(i), src_اوږدتیا, ch, -1);
                ++عدد_Substitutions;
                req_اوږدتیا+=U16_اوږدتیا(ch);
            }
        }
    }

    إذا(p_عدد_Substitutions!=تش_اشاره) {
        *p_عدد_Substitutions=عدد_Substitutions;
    }

    req_اوږدتیا+=(int32_t)(pDest - dest);
    إذا(p_Dest_اوږدتیا) {
        *p_Dest_اوږدتیا = req_اوږدتیا;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, dest_ظرفیت, req_اوږدتیا, p_Error_کوډ);
    أرجع dest;
}

U_CAPI حرف* U_EXPORT2
u_str_منزل_Java_Modified_UTF8(حرف* dest U_LIFETIME_BOUND,
                        int32_t dest_ظرفیت,
                        int32_t* p_Dest_اوږدتیا,
                        ثابت char16_t* src,
                        int32_t src_اوږدتیا,
                        UError_کوډ* p_Error_کوډ) {
    int32_t req_اوږدتیا=0;
    uint32_t ch=0;
    ثابت char16_t *p_Src_حد;
    int32_t count;

    /* args check */
    إذا(U_FAILURE(*p_Error_کوډ)){
        أرجع تش_اشاره;
    }
    إذا( (src==تش_اشاره && src_اوږدتیا!=0) || src_اوږدتیا < -1 ||
        (dest==تش_اشاره && dest_ظرفیت!=0) || dest_ظرفیت<0
    ) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *p_Dest_حد = pDest + dest_ظرفیت;

    إذا(src_اوږدتیا==-1) {
        /* Convert NUL-terminated ASCII, then find the string length. */
        طالما((ch=*src)<=0x7f && ch != 0 && pDest<p_Dest_حد) {
            *pDest++ = (uint8_t)ch;
            ++src;
        }
        إذا(ch == 0) {
            req_اوږدتیا=(int32_t)(pDest - (uint8_t *)dest);
            إذا(p_Dest_اوږدتیا) {
                *p_Dest_اوږدتیا = req_اوږدتیا;
            }

            /* Terminate the buffer */
            u_terminateChars(dest, dest_ظرفیت, req_اوږدتیا, p_Error_کوډ);
            أرجع dest;
        }
        src_اوږدتیا = u_strlen(src);
    }

    /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
    p_Src_حد = (src!=تش_اشاره)?(src+src_اوږدتیا):تش_اشاره;
    لكل(;;) {
        count = (int32_t)(p_Dest_حد - pDest);
        src_اوږدتیا = (int32_t)(p_Src_حد - src);
        إذا(count >= src_اوږدتیا && src_اوږدتیا > 0 && *src <= 0x7f) {
            /* fast ASCII loop */
            ثابت char16_t *prevSrc = src;
            int32_t delta;
            طالما(src < p_Src_حد && (ch = *src) <= 0x7f && ch != 0) {
                *pDest++=(uint8_t)ch;
                ++src;
            }
            delta = (int32_t)(src - prevSrc);
            count -= delta;
            src_اوږدتیا -= delta;
        }
        /*
         * Each iteration of the inner loop progresses by at most 3 UTF-8
         * bytes and one char16_t.
         */
        count /= 3;
        إذا(count > src_اوږدتیا) {
            count = src_اوږدتیا; /* min(remaining dest/3, remaining src) */
        }
        إذا(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            اكسر;
        }
        افعل {
            ch=*src++;
            إذا(ch <= 0x7f && ch != 0) {
                *pDest++ = (uint8_t)ch;
            } وإلا إذا(ch <= 0x7ff) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } وإلا {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            }
        } طالما(--count > 0);
    }

    طالما(src<p_Src_حد) {
        ch=*src++;
        إذا(ch <= 0x7f && ch != 0) {
            إذا(pDest<p_Dest_حد) {
                *pDest++ = (uint8_t)ch;
            } وإلا {
                req_اوږدتیا = 1;
                اكسر;
            }
        } وإلا إذا(ch <= 0x7ff) {
            إذا((p_Dest_حد - pDest) >= 2) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } وإلا {
                req_اوږدتیا = 2;
                اكسر;
            }
        } وإلا {
            إذا((p_Dest_حد - pDest) >= 3) {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } وإلا {
                req_اوږدتیا = 3;
                اكسر;
            }
        }
    }
    طالما(src<p_Src_حد) {
        ch=*src++;
        إذا(ch <= 0x7f && ch != 0) {
            ++req_اوږدتیا;
        } وإلا إذا(ch<=0x7ff) {
            req_اوږدتیا+=2;
        } وإلا {
            req_اوږدتیا+=3;
        }
    }

    req_اوږدتیا+=(int32_t)(pDest - (uint8_t *)dest);
    إذا(p_Dest_اوږدتیا){
        *p_Dest_اوږدتیا = req_اوږدتیا;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, dest_ظرفیت, req_اوږدتیا, p_Error_کوډ);
    أرجع dest;
}
