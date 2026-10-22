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
                      int32_t dest_क्षमता,
                      int32_t* p_Dest_दीर्घता,
                      ध्रुव UChar32* src,
                      int32_t src_दीर्घता,
                      UChar32 subchar,
                      int32_t* p_अङ्कः_Substitutions,
                      UError_संहिता* p_Error_संहिता) {
    ध्रुव UChar32 *src_सीमा;
    UChar32 ch;
    char16_t *dest_सीमा;
    char16_t *pDest;
    int32_t req_दीर्घता;
    int32_t अङ्कः_Substitutions;

    /* args check */
    यदि(U_FAILURE(*p_Error_संहिता)){
        प्रत्यागम शून्यसूचकः;
    }
    यदि( (src==शून्यसूचकः && src_दीर्घता!=0) || src_दीर्घता < -1 ||
        (dest_क्षमता<0) || (dest == शून्यसूचकः && dest_क्षमता > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    यदि(p_अङ्कः_Substitutions != शून्यसूचकः) {
        *p_अङ्कः_Substitutions = 0;
    }

    pDest = dest;
    dest_सीमा = (dest!=शून्यसूचकः)?(dest + dest_क्षमता):शून्यसूचकः;
    req_दीर्घता = 0;
    अङ्कः_Substitutions = 0;

    यदि(src_दीर्घता < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        यावत्((ch=*src) != 0 &&
              ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff))) {
            ++src;
            यदि(pDest < dest_सीमा) {
                *pDest++ = (char16_t)ch;
            } अन्यथा {
                ++req_दीर्घता;
            }
        }
        src_सीमा = src;
        यदि(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            यावत्(*++src_सीमा != 0) {}
        }
    } अन्यथा {
      src_सीमा = (src!=शून्यसूचकः)?(src + src_दीर्घता):शून्यसूचकः;
    }

    /* convert with length */
    यावत्(src < src_सीमा) {
        ch = *src++;
        कुरु {
            /* usually "loops" once; twice only for writing subchar */
            यदि((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff)) {
                यदि(pDest < dest_सीमा) {
                    *pDest++ = (char16_t)ch;
                } अन्यथा {
                    ++req_दीर्घता;
                }
                भङ्ग;
            } अन्यथा यदि(0x10000 <= ch && ch <= 0x10ffff) {
                यदि(pDest!=शून्यसूचकः && ((pDest + 2) <= dest_सीमा)) {
                    *pDest++ = U16_LEAD(ch);
                    *pDest++ = U16_TRAIL(ch);
                } अन्यथा {
                    req_दीर्घता += 2;
                }
                भङ्ग;
            } अन्यथा यदि((ch = subchar) < 0) {
                /* surrogate code point, or not a Unicode code point at all */
                *p_Error_संहिता = U_INVALID_CHAR_प्राप्तम्;
                प्रत्यागम शून्यसूचकः;
            } अन्यथा {
                ++अङ्कः_Substitutions;
            }
        } यावत्(सत्य);
    }

    req_दीर्घता += (int32_t)(pDest - dest);
    यदि(p_Dest_दीर्घता) {
        *p_Dest_दीर्घता = req_दीर्घता;
    }
    यदि(p_अङ्कः_Substitutions != शून्यसूचकः) {
        *p_अङ्कः_Substitutions = अङ्कः_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, dest_क्षमता, req_दीर्घता, p_Error_संहिता);
    
    प्रत्यागम dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32(char16_t* dest U_LIFETIME_BOUND,
               int32_t dest_क्षमता,
               int32_t* p_Dest_दीर्घता,
               ध्रुव UChar32* src,
               int32_t src_दीर्घता,
               UError_संहिता* p_Error_संहिता) {
    प्रत्यागम u_strFromUTF32WithSub(
            dest, dest_क्षमता, p_Dest_दीर्घता,
            src, src_दीर्घता,
            U_SENTINEL, शून्यसूचकः,
            p_Error_संहिता);
}

U_CAPI UChar32* U_EXPORT2
u_str_गन्तव्यम्_UTF32_With_Sub(UChar32* dest U_LIFETIME_BOUND,
                    int32_t dest_क्षमता,
                    int32_t* p_Dest_दीर्घता,
                    ध्रुव char16_t* src,
                    int32_t src_दीर्घता,
                    UChar32 subchar,
                    int32_t* p_अङ्कः_Substitutions,
                    UError_संहिता* p_Error_संहिता) {
    ध्रुव char16_t *src_सीमा;
    UChar32 ch;
    char16_t ch2;
    UChar32 *dest_सीमा;
    UChar32 *pDest;
    int32_t req_दीर्घता;
    int32_t अङ्कः_Substitutions;

    /* args check */
    यदि(U_FAILURE(*p_Error_संहिता)){
        प्रत्यागम शून्यसूचकः;
    }
    यदि( (src==शून्यसूचकः && src_दीर्घता!=0) || src_दीर्घता < -1 ||
        (dest_क्षमता<0) || (dest == शून्यसूचकः && dest_क्षमता > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    यदि(p_अङ्कः_Substitutions != शून्यसूचकः) {
        *p_अङ्कः_Substitutions = 0;
    }

    pDest = dest;
    dest_सीमा = (dest!=शून्यसूचकः)?(dest + dest_क्षमता):शून्यसूचकः;
    req_दीर्घता = 0;
    अङ्कः_Substitutions = 0;

    यदि(src_दीर्घता < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        यावत्((ch=*src) != 0 && !U16_IS_SURROGATE(ch)) {
            ++src;
            यदि(pDest < dest_सीमा) {
                *pDest++ = ch;
            } अन्यथा {
                ++req_दीर्घता;
            }
        }
        src_सीमा = src;
        यदि(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            यावत्(*++src_सीमा != 0) {}
        }
    } अन्यथा {
        src_सीमा = (src!=शून्यसूचकः)?(src + src_दीर्घता):शून्यसूचकः;
    }

    /* convert with length */
    यावत्(src < src_सीमा) {
        ch = *src++;
        यदि(!U16_IS_SURROGATE(ch)) {
            /* write or count ch below */
        } अन्यथा यदि(U16_IS_SURROGATE_LEAD(ch) && src < src_सीमा && U16_IS_TRAIL(ch2 = *src)) {
            ++src;
            ch = U16_GET_SUPPLEMENTARY(ch, ch2);
        } अन्यथा यदि((ch = subchar) < 0) {
            /* unpaired surrogate */
            *p_Error_संहिता = U_INVALID_CHAR_प्राप्तम्;
            प्रत्यागम शून्यसूचकः;
        } अन्यथा {
            ++अङ्कः_Substitutions;
        }
        यदि(pDest < dest_सीमा) {
            *pDest++ = ch;
        } अन्यथा {
            ++req_दीर्घता;
        }
    }

    req_दीर्घता += (int32_t)(pDest - dest);
    यदि(p_Dest_दीर्घता) {
        *p_Dest_दीर्घता = req_दीर्घता;
    }
    यदि(p_अङ्कः_Substitutions != शून्यसूचकः) {
        *p_अङ्कः_Substitutions = अङ्कः_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChar32s(dest, dest_क्षमता, req_दीर्घता, p_Error_संहिता);

    प्रत्यागम dest;
}

U_CAPI UChar32* U_EXPORT2
u_str_गन्तव्यम्_UTF32(UChar32* dest U_LIFETIME_BOUND,
             int32_t dest_क्षमता,
             int32_t* p_Dest_दीर्घता,
             ध्रुव char16_t* src,
             int32_t src_दीर्घता,
             UError_संहिता* p_Error_संहिता) {
    प्रत्यागम u_str_गन्तव्यम्_UTF32_With_Sub(
            dest, dest_क्षमता, p_Dest_दीर्घता,
            src, src_दीर्घता,
            U_SENTINEL, शून्यसूचकः,
            p_Error_संहिता);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                     int32_t dest_क्षमता,
                     int32_t* p_Dest_दीर्घता,
                     ध्रुव वर्ण* src,
                     int32_t src_दीर्घता,
                     UChar32 subchar,
                     int32_t* p_अङ्कः_Substitutions,
                     UError_संहिता* p_Error_संहिता) {
    /* args check */
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि( (src==शून्यसूचकः && src_दीर्घता!=0) || src_दीर्घता < -1 ||
        (dest_क्षमता<0) || (dest == शून्यसूचकः && dest_क्षमता > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    यदि(p_अङ्कः_Substitutions!=शून्यसूचकः) {
        *p_अङ्कः_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *p_Dest_सीमा = dest+dest_क्षमता;
    int32_t req_दीर्घता = 0;
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

    यदि(src_दीर्घता < 0){
        /*
         * Transform a NUL-terminated string.
         * The code explicitly checks for NULs only in the lead byte position.
         * A NUL byte in the trail byte position fails the trail byte range check anyway.
         */
        int32_t i;
        UChar32 c;
        प्रति(i = 0; (c = (uint8_t)src[i]) != 0 && (pDest < p_Dest_सीमा);) {
            // modified copy of U8_NEXT()
            ++i;
            यदि(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } अन्यथा {
                uint8_t __t1, __t2;
                यदि( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } अन्यथा यदि( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } अन्यथा {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_अग्र_Char_Safe_Body((ध्रुव uint8_t *)src, &(i), -1, c, -1);
                    यदि(c<0 && (++अङ्कः_Substitutions, c = subchar) < 0) {
                        *p_Error_संहिता = U_INVALID_CHAR_प्राप्तम्;
                        प्रत्यागम शून्यसूचकः;
                    } अन्यथा यदि(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } अन्यथा {
                        *(pDest++)=U16_LEAD(c);
                        यदि(pDest<p_Dest_सीमा) {
                            *(pDest++)=U16_TRAIL(c);
                        } अन्यथा {
                            req_दीर्घता++;
                            भङ्ग;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        यावत्((c = (uint8_t)src[i]) != 0) {
            // modified copy of U8_NEXT()
            ++i;
            यदि(U8_IS_SINGLE(c)) {
                ++req_दीर्घता;
            } अन्यथा {
                uint8_t __t1, __t2;
                यदि( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++req_दीर्घता;
                    i+=2;
                } अन्यथा यदि( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++req_दीर्घता;
                    ++(i);
                } अन्यथा {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_अग्र_Char_Safe_Body((ध्रुव uint8_t *)src, &(i), -1, c, -1);
                    यदि(c<0 && (++अङ्कः_Substitutions, c = subchar) < 0) {
                        *p_Error_संहिता = U_INVALID_CHAR_प्राप्तम्;
                        प्रत्यागम शून्यसूचकः;
                    }
                    req_दीर्घता += U16_दीर्घता(c);
                }
            }
        }
    } अन्यथा /* srcLength >= 0 */ {
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
            int32_t count = (int32_t)(p_Dest_सीमा - pDest);
            int32_t count2 = (src_दीर्घता - i) / 3;
            यदि(count > count2) {
                count = count2; /* min(remaining dest, remaining src/3) */
            }
            यदि(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                भङ्ग;
            }

            कुरु {
                // modified copy of U8_NEXT()
                c = (uint8_t)src[i++];
                यदि(U8_IS_SINGLE(c)) {
                    *pDest++=(char16_t)c;
                } अन्यथा {
                    uint8_t __t1, __t2;
                    यदि( /* handle U+0800..U+FFFF inline */
                            (0xe0<=(c) && (c)<0xf0) &&
                            ((i)+1)<src_दीर्घता &&
                            U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                            (__t2=src[(i)+1]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                        i+=2;
                    } अन्यथा यदि( /* handle U+0080..U+07FF inline */
                            ((c)<0xe0 && (c)>=0xc2) &&
                            ((i)!=src_दीर्घता) &&
                            (__t1=src[i]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0x1f)<<6)|__t1;
                        ++(i);
                    } अन्यथा {
                        यदि(c >= 0xf0 || subchar > 0xffff) {
                            // We may read up to four bytes and write up to two UChars,
                            // which we didn't account for with computing count,
                            // so we adjust it here.
                            यदि(--count == 0) {
                                --i;  // back out byte c
                                भङ्ग;
                            }
                        }

                        /* function call for "complicated" and error cases */
                        (c)=utf8_अग्र_Char_Safe_Body((ध्रुव uint8_t *)src, &(i), src_दीर्घता, c, -1);
                        यदि(c<0 && (++अङ्कः_Substitutions, c = subchar) < 0) {
                            *p_Error_संहिता = U_INVALID_CHAR_प्राप्तम्;
                            प्रत्यागम शून्यसूचकः;
                        } अन्यथा यदि(c<=0xFFFF) {
                            *(pDest++)=(char16_t)c;
                        } अन्यथा {
                            *(pDest++)=U16_LEAD(c);
                            *(pDest++)=U16_TRAIL(c);
                        }
                    }
                }
            } यावत्(--count > 0);
        }

        यावत्(i < src_दीर्घता && (pDest < p_Dest_सीमा)) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            यदि(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } अन्यथा {
                uint8_t __t1, __t2;
                यदि( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<src_दीर्घता &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } अन्यथा यदि( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=src_दीर्घता) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } अन्यथा {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_अग्र_Char_Safe_Body((ध्रुव uint8_t *)src, &(i), src_दीर्घता, c, -1);
                    यदि(c<0 && (++अङ्कः_Substitutions, c = subchar) < 0) {
                        *p_Error_संहिता = U_INVALID_CHAR_प्राप्तम्;
                        प्रत्यागम शून्यसूचकः;
                    } अन्यथा यदि(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } अन्यथा {
                        *(pDest++)=U16_LEAD(c);
                        यदि(pDest<p_Dest_सीमा) {
                            *(pDest++)=U16_TRAIL(c);
                        } अन्यथा {
                            req_दीर्घता++;
                            भङ्ग;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        यावत्(i < src_दीर्घता) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            यदि(U8_IS_SINGLE(c)) {
                ++req_दीर्घता;
            } अन्यथा {
                uint8_t __t1, __t2;
                यदि( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<src_दीर्घता &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++req_दीर्घता;
                    i+=2;
                } अन्यथा यदि( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=src_दीर्घता) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++req_दीर्घता;
                    ++(i);
                } अन्यथा {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_अग्र_Char_Safe_Body((ध्रुव uint8_t *)src, &(i), src_दीर्घता, c, -1);
                    यदि(c<0 && (++अङ्कः_Substitutions, c = subchar) < 0) {
                        *p_Error_संहिता = U_INVALID_CHAR_प्राप्तम्;
                        प्रत्यागम शून्यसूचकः;
                    }
                    req_दीर्घता += U16_दीर्घता(c);
                }
            }
        }
    }

    req_दीर्घता+=(int32_t)(pDest - dest);

    यदि(p_अङ्कः_Substitutions!=शून्यसूचकः) {
        *p_अङ्कः_Substitutions=अङ्कः_Substitutions;
    }

    यदि(p_Dest_दीर्घता){
        *p_Dest_दीर्घता = req_दीर्घता;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_क्षमता,req_दीर्घता,p_Error_संहिता);

    प्रत्यागम dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8(char16_t* dest U_LIFETIME_BOUND,
              int32_t dest_क्षमता,
              int32_t* p_Dest_दीर्घता,
              ध्रुव वर्ण* src,
              int32_t src_दीर्घता,
              UError_संहिता* p_Error_संहिता) {
    प्रत्यागम u_strFromUTF8WithSub(
            dest, dest_क्षमता, p_Dest_दीर्घता,
            src, src_दीर्घता,
            U_SENTINEL, शून्यसूचकः,
            p_Error_संहिता);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8Lenient(char16_t* dest U_LIFETIME_BOUND,
                     int32_t dest_क्षमता,
                     int32_t* p_Dest_दीर्घता,
                     ध्रुव वर्ण* src,
                     int32_t src_दीर्घता,
                     UError_संहिता* p_Error_संहिता) {
    char16_t *pDest = dest;
    UChar32 ch;
    int32_t req_दीर्घता = 0;
    uint8_t* pSrc = (uint8_t*) src;

    /* args check */
    यदि(U_FAILURE(*p_Error_संहिता)){
        प्रत्यागम शून्यसूचकः;
    }
        
    यदि( (src==शून्यसूचकः && src_दीर्घता!=0) || src_दीर्घता < -1 ||
        (dest_क्षमता<0) || (dest == शून्यसूचकः && dest_क्षमता > 0)
    ) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    यदि(src_दीर्घता < 0) {
        /* Transform a NUL-terminated string. */
        char16_t *p_Dest_सीमा = (dest!=शून्यसूचकः)?(dest+dest_क्षमता):शून्यसूचकः;
        uint8_t t1, t2, t3; /* trail bytes */

        यावत्(((ch = *pSrc) != 0) && (pDest < p_Dest_सीमा)) {
            यदि(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ++pSrc;
                अनुवर्त;
            } अन्यथा यदि(ch < 0xe0) { /* U+0080..U+07FF */
                यदि((t1 = pSrc[1]) != 0) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + t1 - 0x3080);
                    pSrc += 2;
                    अनुवर्त;
                }
            } अन्यथा यदि(ch < 0xf0) { /* U+0800..U+FFFF */
                यदि((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 12) + (t1 << 6) + t2 - 0x2080);
                    pSrc += 3;
                    अनुवर्त;
                }
            } अन्यथा /* f0..f4 */ { /* U+10000..U+10FFFF */
                यदि((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0 && (t3 = pSrc[3]) != 0) {
                    pSrc += 4;
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (t1 << 12) + (t2 << 6) + t3 - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    यदि(pDest < p_Dest_सीमा) {
                        *(pDest++) = U16_TRAIL(ch);
                    } अन्यथा {
                        req_दीर्घता = 1;
                        भङ्ग;
                    }
                    अनुवर्त;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            यावत्(*++pSrc != 0) {}
            भङ्ग;
        }

        /* Pre-flight the rest of the string. */
        यावत्((ch = *pSrc) != 0) {
            यदि(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                ++req_दीर्घता;
                ++pSrc;
                अनुवर्त;
            } अन्यथा यदि(ch < 0xe0) { /* U+0080..U+07FF */
                यदि(pSrc[1] != 0) {
                    ++req_दीर्घता;
                    pSrc += 2;
                    अनुवर्त;
                }
            } अन्यथा यदि(ch < 0xf0) { /* U+0800..U+FFFF */
                यदि(pSrc[1] != 0 && pSrc[2] != 0) {
                    ++req_दीर्घता;
                    pSrc += 3;
                    अनुवर्त;
                }
            } अन्यथा /* f0..f4 */ { /* U+10000..U+10FFFF */
                यदि(pSrc[1] != 0 && pSrc[2] != 0 && pSrc[3] != 0) {
                    req_दीर्घता += 2;
                    pSrc += 4;
                    अनुवर्त;
                }
            }

            /* truncated character at the end */
            ++req_दीर्घता;
            भङ्ग;
        }
    } अन्यथा /* srcLength >= 0 */ {
      ध्रुव uint8_t *p_Src_सीमा = (pSrc!=शून्यसूचकः)?(pSrc + src_दीर्घता):शून्यसूचकः;

        /*
         * This function requires that if srcLength is given, then it must be
         * destCapatity >= srcLength so that we need not check for
         * destination buffer overflow in the loop.
         */
        यदि(dest_क्षमता < src_दीर्घता) {
            यदि(p_Dest_दीर्घता != शून्यसूचकः) {
                *p_Dest_दीर्घता = src_दीर्घता; /* this likely overestimates the true destLength! */
            }
            *p_Error_संहिता = U_बफर_अतिप्रवाहः_ERROR;
            प्रत्यागम शून्यसूचकः;
        }

        यदि((p_Src_सीमा - pSrc) >= 4) {
            p_Src_सीमा -= 3; /* temporarily reduce pSrcLimit */

            /* in this loop, we can always access at least 4 bytes, up to pSrc+3 */
            कुरु {
                ch = *pSrc++;
                यदि(ch < 0xc0) {
                    /*
                     * ASCII, or a trail byte in lead position which is treated like
                     * a single-byte sequence for better character boundary
                     * resynchronization after illegal sequences.
                     */
                    *pDest++=(char16_t)ch;
                } अन्यथा यदि(ch < 0xe0) { /* U+0080..U+07FF */
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                } अन्यथा यदि(ch < 0xf0) { /* U+0800..U+FFFF */
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                } अन्यथा /* f0..f4 */ { /* U+10000..U+10FFFF */
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                }
            } यावत्(pSrc < p_Src_सीमा);

            p_Src_सीमा += 3; /* restore original pSrcLimit */
        }

        यावत्(pSrc < p_Src_सीमा) {
            ch = *pSrc++;
            यदि(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                अनुवर्त;
            } अन्यथा यदि(ch < 0xe0) { /* U+0080..U+07FF */
                यदि(pSrc < p_Src_सीमा) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                    अनुवर्त;
                }
            } अन्यथा यदि(ch < 0xf0) { /* U+0800..U+FFFF */
                यदि((p_Src_सीमा - pSrc) >= 2) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                    pSrc += 3;
                    अनुवर्त;
                }
            } अन्यथा /* f0..f4 */ { /* U+10000..U+10FFFF */
                यदि((p_Src_सीमा - pSrc) >= 3) {
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

    req_दीर्घता+=(int32_t)(pDest - dest);

    यदि(p_Dest_दीर्घता){
        *p_Dest_दीर्घता = req_दीर्घता;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_क्षमता,req_दीर्घता,p_Error_संहिता);

    प्रत्यागम dest;
}

स्थिर अन्तःस्थ uint8_t *
_appendUTF8(uint8_t *pDest, UChar32 c) {
    /* it is 0<=c<=0x10ffff and not a surrogate if called by a validating function */
    यदि((c)<=0x7f) {
        *pDest++ = static_cast<uint8_t>(c);
    } अन्यथा यदि(c<=0x7ff) {
        *pDest++ = static_cast<uint8_t>((c >> 6) | 0xc0);
        *pDest++ = static_cast<uint8_t>((c & 0x3f) | 0x80);
    } अन्यथा यदि(c<=0xffff) {
        *pDest++ = static_cast<uint8_t>((c >> 12) | 0xe0);
        *pDest++ = static_cast<uint8_t>(((c >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    } अन्यथा /* if((uint32_t)(c)<=0x10ffff) */ {
        *pDest++ = static_cast<uint8_t>(((c) >> 18) | 0xf0);
        *pDest++ = static_cast<uint8_t>((((c) >> 12) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>((((c) >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    }
    प्रत्यागम pDest;
}

U_CAPI वर्ण* U_EXPORT2
u_str_गन्तव्यम्_UTF8_With_Sub(वर्ण* dest U_LIFETIME_BOUND,
                   int32_t dest_क्षमता,
                   int32_t* p_Dest_दीर्घता,
                   ध्रुव char16_t* pSrc,
                   int32_t src_दीर्घता,
                   UChar32 subchar,
                   int32_t* p_अङ्कः_Substitutions,
                   UError_संहिता* p_Error_संहिता) {
    int32_t req_दीर्घता=0;
    uint32_t ch=0,ch2=0;
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *p_Dest_सीमा = (pDest!=शून्यसूचकः)?(pDest + dest_क्षमता):शून्यसूचकः;
    int32_t अङ्कः_Substitutions;

    /* args check */
    यदि(U_FAILURE(*p_Error_संहिता)){
        प्रत्यागम शून्यसूचकः;
    }
        
    यदि( (pSrc==शून्यसूचकः && src_दीर्घता!=0) || src_दीर्घता < -1 ||
        (dest_क्षमता<0) || (dest == शून्यसूचकः && dest_क्षमता > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    यदि(p_अङ्कः_Substitutions!=शून्यसूचकः) {
        *p_अङ्कः_Substitutions=0;
    }
    अङ्कः_Substitutions=0;

    यदि(src_दीर्घता==-1) {
        यावत्((ch=*pSrc)!=0) {
            ++pSrc;
            यदि(ch <= 0x7f) {
                यदि(pDest<p_Dest_सीमा) {
                    *pDest++ = (uint8_t)ch;
                } अन्यथा {
                    req_दीर्घता = 1;
                    भङ्ग;
                }
            } अन्यथा यदि(ch <= 0x7ff) {
                यदि((p_Dest_सीमा - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अन्यथा {
                    req_दीर्घता = 2;
                    भङ्ग;
                }
            } अन्यथा यदि(ch <= 0xd7ff || ch >= 0xe000) {
                यदि((p_Dest_सीमा - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अन्यथा {
                    req_दीर्घता = 3;
                    भङ्ग;
                }
            } अन्यथा /* ch is a surrogate */ {
                int32_t length;

                /*need not check for NUL because NUL fails U16_IS_TRAIL() anyway*/
                यदि(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } अन्यथा यदि(subchar>=0) {
                    ch=subchar;
                    ++अङ्कः_Substitutions;
                } अन्यथा {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *p_Error_संहिता = U_INVALID_CHAR_प्राप्तम्;
                    प्रत्यागम शून्यसूचकः;
                }

                length = U8_दीर्घता(ch);
                यदि((p_Dest_सीमा - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } अन्यथा {
                    req_दीर्घता = length;
                    भङ्ग;
                }
            }
        }
        यावत्((ch=*pSrc++)!=0) {
            यदि(ch<=0x7f) {
                ++req_दीर्घता;
            } अन्यथा यदि(ch<=0x7ff) {
                req_दीर्घता+=2;
            } अन्यथा यदि(!U16_IS_SURROGATE(ch)) {
                req_दीर्घता+=3;
            } अन्यथा यदि(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                req_दीर्घता+=4;
            } अन्यथा यदि(subchar>=0) {
                req_दीर्घता+=U8_दीर्घता(subchar);
                ++अङ्कः_Substitutions;
            } अन्यथा {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *p_Error_संहिता = U_INVALID_CHAR_प्राप्तम्;
                प्रत्यागम शून्यसूचकः;
            }
        }
    } अन्यथा {
        ध्रुव char16_t *p_Src_सीमा = (pSrc!=शून्यसूचकः)?(pSrc+src_दीर्घता):शून्यसूचकः;
        int32_t count;

        /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
        प्रति(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            count = (int32_t)((p_Dest_सीमा - pDest) / 3);
            src_दीर्घता = (int32_t)(p_Src_सीमा - pSrc);
            यदि(count > src_दीर्घता) {
                count = src_दीर्घता; /* min(remaining dest/3, remaining src) */
            }
            यदि(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                भङ्ग;
            }
            कुरु {
                ch=*pSrc++;
                यदि(ch <= 0x7f) {
                    *pDest++ = (uint8_t)ch;
                } अन्यथा यदि(ch <= 0x7ff) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अन्यथा यदि(ch <= 0xd7ff || ch >= 0xe000) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अन्यथा /* ch is a surrogate */ {
                    /*
                     * We will read two UChars and probably output four bytes,
                     * which we didn't account for with computing count,
                     * so we adjust it here.
                     */
                    यदि(--count == 0) {
                        --pSrc; /* undo ch=*pSrc++ for the lead surrogate */
                        भङ्ग;  /* recompute count */
                    }

                    यदि(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                        ++pSrc;
                        ch=U16_GET_SUPPLEMENTARY(ch, ch2);

                        /* writing 4 bytes per 2 UChars is ok */
                        *pDest++=(uint8_t)((ch>>18)|0xf0);
                        *pDest++=(uint8_t)(((ch>>12)&0x3f)|0x80);
                        *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                        *pDest++=(uint8_t)((ch&0x3f)|0x80);
                    } अन्यथा  {
                        /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                        यदि(subchar>=0) {
                            ch=subchar;
                            ++अङ्कः_Substitutions;
                        } अन्यथा {
                            *p_Error_संहिता = U_INVALID_CHAR_प्राप्तम्;
                            प्रत्यागम शून्यसूचकः;
                        }

                        /* convert and append*/
                        pDest=_appendUTF8(pDest, ch);
                    }
                }
            } यावत्(--count > 0);
        }

        यावत्(pSrc<p_Src_सीमा) {
            ch=*pSrc++;
            यदि(ch <= 0x7f) {
                यदि(pDest<p_Dest_सीमा) {
                    *pDest++ = (uint8_t)ch;
                } अन्यथा {
                    req_दीर्घता = 1;
                    भङ्ग;
                }
            } अन्यथा यदि(ch <= 0x7ff) {
                यदि((p_Dest_सीमा - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अन्यथा {
                    req_दीर्घता = 2;
                    भङ्ग;
                }
            } अन्यथा यदि(ch <= 0xd7ff || ch >= 0xe000) {
                यदि((p_Dest_सीमा - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } अन्यथा {
                    req_दीर्घता = 3;
                    भङ्ग;
                }
            } अन्यथा /* ch is a surrogate */ {
                int32_t length;

                यदि(U16_IS_SURROGATE_LEAD(ch) && pSrc<p_Src_सीमा && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } अन्यथा यदि(subchar>=0) {
                    ch=subchar;
                    ++अङ्कः_Substitutions;
                } अन्यथा {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *p_Error_संहिता = U_INVALID_CHAR_प्राप्तम्;
                    प्रत्यागम शून्यसूचकः;
                }

                length = U8_दीर्घता(ch);
                यदि((p_Dest_सीमा - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } अन्यथा {
                    req_दीर्घता = length;
                    भङ्ग;
                }
            }
        }
        यावत्(pSrc<p_Src_सीमा) {
            ch=*pSrc++;
            यदि(ch<=0x7f) {
                ++req_दीर्घता;
            } अन्यथा यदि(ch<=0x7ff) {
                req_दीर्घता+=2;
            } अन्यथा यदि(!U16_IS_SURROGATE(ch)) {
                req_दीर्घता+=3;
            } अन्यथा यदि(U16_IS_SURROGATE_LEAD(ch) && pSrc<p_Src_सीमा && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                req_दीर्घता+=4;
            } अन्यथा यदि(subchar>=0) {
                req_दीर्घता+=U8_दीर्घता(subchar);
                ++अङ्कः_Substitutions;
            } अन्यथा {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *p_Error_संहिता = U_INVALID_CHAR_प्राप्तम्;
                प्रत्यागम शून्यसूचकः;
            }
        }
    }

    req_दीर्घता+=(int32_t)(pDest - (uint8_t *)dest);

    यदि(p_अङ्कः_Substitutions!=शून्यसूचकः) {
        *p_अङ्कः_Substitutions=अङ्कः_Substitutions;
    }

    यदि(p_Dest_दीर्घता){
        *p_Dest_दीर्घता = req_दीर्घता;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, dest_क्षमता, req_दीर्घता, p_Error_संहिता);
    प्रत्यागम dest;
}

U_CAPI वर्ण* U_EXPORT2
u_str_गन्तव्यम्_UTF8(वर्ण* dest U_LIFETIME_BOUND,
            int32_t dest_क्षमता,
            int32_t* p_Dest_दीर्घता,
            ध्रुव char16_t* pSrc,
            int32_t src_दीर्घता,
            UError_संहिता* p_Error_संहिता) {
    प्रत्यागम u_str_गन्तव्यम्_UTF8_With_Sub(
            dest, dest_क्षमता, p_Dest_दीर्घता,
            pSrc, src_दीर्घता,
            U_SENTINEL, शून्यसूचकः,
            p_Error_संहिता);
}

U_CAPI char16_t* U_EXPORT2
u_strFromJavaModifiedUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                                 int32_t dest_क्षमता,
                                 int32_t* p_Dest_दीर्घता,
                                 ध्रुव वर्ण* src,
                                 int32_t src_दीर्घता,
                                 UChar32 subchar,
                                 int32_t* p_अङ्कः_Substitutions,
                                 UError_संहिता* p_Error_संहिता) {
    /* args check */
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि( (src==शून्यसूचकः && src_दीर्घता!=0) || src_दीर्घता < -1 ||
        (dest==शून्यसूचकः && dest_क्षमता!=0) || dest_क्षमता<0 ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    यदि(p_अङ्कः_Substitutions!=शून्यसूचकः) {
        *p_अङ्कः_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *p_Dest_सीमा = dest+dest_क्षमता;
    int32_t req_दीर्घता = 0;
    int32_t अङ्कः_Substitutions=0;

    यदि(src_दीर्घता < 0) {
        /*
         * Transform a NUL-terminated ASCII string.
         * Handle non-ASCII strings with slower code.
         */
        UChar32 c;
        यावत्(((c = (uint8_t)*src) != 0) && c <= 0x7f && (pDest < p_Dest_सीमा)) {
            *pDest++=(char16_t)c;
            ++src;
        }
        यदि(c == 0) {
            req_दीर्घता=(int32_t)(pDest - dest);
            यदि(p_Dest_दीर्घता) {
                *p_Dest_दीर्घता = req_दीर्घता;
            }

            /* Terminate the buffer */
            u_terminateUChars(dest, dest_क्षमता, req_दीर्घता, p_Error_संहिता);
            प्रत्यागम dest;
        }
        src_दीर्घता = static_cast<int32_t>(uprv_strlen(src));
    }

    /* Faster loop without ongoing checking for srcLength and pDestLimit. */
    UChar32 ch;
    uint8_t t1, t2;
    int32_t i = 0;
    प्रति(;;) {
        int32_t count = (int32_t)(p_Dest_सीमा - pDest);
        int32_t count2 = src_दीर्घता - i;
        यदि(count >= count2 && src_दीर्घता > 0 && U8_IS_SINGLE(*src)) {
            /* fast ASCII loop */
            int32_t start = i;
            uint8_t b;
            यावत्(i < src_दीर्घता && U8_IS_SINGLE(b = src[i])) {
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
        यदि(subchar > 0xFFFF) {
            भङ्ग;
        }
        count2 /= 3;
        यदि(count > count2) {
            count = count2; /* min(remaining dest, remaining src/3) */
        }
        यदि(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            भङ्ग;
        }
        कुरु {
            ch = (uint8_t)src[i++];
            यदि(U8_IS_SINGLE(ch)) {
                *pDest++=(char16_t)ch;
            } अन्यथा {
                यदि(ch >= 0xe0) {
                    यदि( /* handle U+0000..U+FFFF inline */
                        ch <= 0xef &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                        (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                    ) {
                        /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                        *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                        i += 2;
                        अनुवर्त;
                    }
                } अन्यथा {
                    यदि( /* handle U+0000..U+07FF inline */
                        ch >= 0xc0 &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                    ) {
                        *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                        ++i;
                        अनुवर्त;
                    }
                }

                यदि(subchar < 0) {
                    *p_Error_संहिता = U_INVALID_CHAR_प्राप्तम्;
                    प्रत्यागम शून्यसूचकः;
                } अन्यथा यदि(subchar > 0xffff && --count == 0) {
                    /*
                     * We need to write two UChars, adjusted count for that,
                     * and ran out of space.
                     */
                    --i;  // back out byte ch
                    भङ्ग;
                } अन्यथा {
                    /* function call for error cases */
                    utf8_अग्र_Char_Safe_Body((ध्रुव uint8_t *)src, &(i), src_दीर्घता, ch, -1);
                    ++अङ्कः_Substitutions;
                    *(pDest++)=(char16_t)subchar;
                }
            }
        } यावत्(--count > 0);
    }

    यावत्(i < src_दीर्घता && (pDest < p_Dest_सीमा)) {
        ch = (uint8_t)src[i++];
        यदि(U8_IS_SINGLE(ch)){
            *pDest++=(char16_t)ch;
        } अन्यथा {
            यदि(ch >= 0xe0) {
                यदि( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < src_दीर्घता &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                    (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                ) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                    i += 2;
                    अनुवर्त;
                }
            } अन्यथा {
                यदि( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < src_दीर्घता &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                ) {
                    *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                    ++i;
                    अनुवर्त;
                }
            }

            यदि(subchar < 0) {
                *p_Error_संहिता = U_INVALID_CHAR_प्राप्तम्;
                प्रत्यागम शून्यसूचकः;
            } अन्यथा {
                /* function call for error cases */
                utf8_अग्र_Char_Safe_Body((ध्रुव uint8_t *)src, &(i), src_दीर्घता, ch, -1);
                ++अङ्कः_Substitutions;
                यदि(subchar<=0xFFFF) {
                    *(pDest++)=(char16_t)subchar;
                } अन्यथा {
                    *(pDest++)=U16_LEAD(subchar);
                    यदि(pDest<p_Dest_सीमा) {
                        *(pDest++)=U16_TRAIL(subchar);
                    } अन्यथा {
                        req_दीर्घता++;
                        भङ्ग;
                    }
                }
            }
        }
    }

    /* Pre-flight the rest of the string. */
    यावत्(i < src_दीर्घता) {
        ch = (uint8_t)src[i++];
        यदि(U8_IS_SINGLE(ch)) {
            req_दीर्घता++;
        } अन्यथा {
            यदि(ch >= 0xe0) {
                यदि( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < src_दीर्घता &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f &&
                    (uint8_t)(src[i+1] - 0x80) <= 0x3f
                ) {
                    req_दीर्घता++;
                    i += 2;
                    अनुवर्त;
                }
            } अन्यथा {
                यदि( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < src_दीर्घता &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f
                ) {
                    req_दीर्घता++;
                    ++i;
                    अनुवर्त;
                }
            }

            यदि(subchar < 0) {
                *p_Error_संहिता = U_INVALID_CHAR_प्राप्तम्;
                प्रत्यागम शून्यसूचकः;
            } अन्यथा {
                /* function call for error cases */
                utf8_अग्र_Char_Safe_Body((ध्रुव uint8_t *)src, &(i), src_दीर्घता, ch, -1);
                ++अङ्कः_Substitutions;
                req_दीर्घता+=U16_दीर्घता(ch);
            }
        }
    }

    यदि(p_अङ्कः_Substitutions!=शून्यसूचकः) {
        *p_अङ्कः_Substitutions=अङ्कः_Substitutions;
    }

    req_दीर्घता+=(int32_t)(pDest - dest);
    यदि(p_Dest_दीर्घता) {
        *p_Dest_दीर्घता = req_दीर्घता;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, dest_क्षमता, req_दीर्घता, p_Error_संहिता);
    प्रत्यागम dest;
}

U_CAPI वर्ण* U_EXPORT2
u_str_गन्तव्यम्_Java_Modified_UTF8(वर्ण* dest U_LIFETIME_BOUND,
                        int32_t dest_क्षमता,
                        int32_t* p_Dest_दीर्घता,
                        ध्रुव char16_t* src,
                        int32_t src_दीर्घता,
                        UError_संहिता* p_Error_संहिता) {
    int32_t req_दीर्घता=0;
    uint32_t ch=0;
    ध्रुव char16_t *p_Src_सीमा;
    int32_t count;

    /* args check */
    यदि(U_FAILURE(*p_Error_संहिता)){
        प्रत्यागम शून्यसूचकः;
    }
    यदि( (src==शून्यसूचकः && src_दीर्घता!=0) || src_दीर्घता < -1 ||
        (dest==शून्यसूचकः && dest_क्षमता!=0) || dest_क्षमता<0
    ) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *p_Dest_सीमा = pDest + dest_क्षमता;

    यदि(src_दीर्घता==-1) {
        /* Convert NUL-terminated ASCII, then find the string length. */
        यावत्((ch=*src)<=0x7f && ch != 0 && pDest<p_Dest_सीमा) {
            *pDest++ = (uint8_t)ch;
            ++src;
        }
        यदि(ch == 0) {
            req_दीर्घता=(int32_t)(pDest - (uint8_t *)dest);
            यदि(p_Dest_दीर्घता) {
                *p_Dest_दीर्घता = req_दीर्घता;
            }

            /* Terminate the buffer */
            u_terminateChars(dest, dest_क्षमता, req_दीर्घता, p_Error_संहिता);
            प्रत्यागम dest;
        }
        src_दीर्घता = u_strlen(src);
    }

    /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
    p_Src_सीमा = (src!=शून्यसूचकः)?(src+src_दीर्घता):शून्यसूचकः;
    प्रति(;;) {
        count = (int32_t)(p_Dest_सीमा - pDest);
        src_दीर्घता = (int32_t)(p_Src_सीमा - src);
        यदि(count >= src_दीर्घता && src_दीर्घता > 0 && *src <= 0x7f) {
            /* fast ASCII loop */
            ध्रुव char16_t *पूर्व_Src = src;
            int32_t delta;
            यावत्(src < p_Src_सीमा && (ch = *src) <= 0x7f && ch != 0) {
                *pDest++=(uint8_t)ch;
                ++src;
            }
            delta = (int32_t)(src - पूर्व_Src);
            count -= delta;
            src_दीर्घता -= delta;
        }
        /*
         * Each iteration of the inner loop progresses by at most 3 UTF-8
         * bytes and one char16_t.
         */
        count /= 3;
        यदि(count > src_दीर्घता) {
            count = src_दीर्घता; /* min(remaining dest/3, remaining src) */
        }
        यदि(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            भङ्ग;
        }
        कुरु {
            ch=*src++;
            यदि(ch <= 0x7f && ch != 0) {
                *pDest++ = (uint8_t)ch;
            } अन्यथा यदि(ch <= 0x7ff) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } अन्यथा {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            }
        } यावत्(--count > 0);
    }

    यावत्(src<p_Src_सीमा) {
        ch=*src++;
        यदि(ch <= 0x7f && ch != 0) {
            यदि(pDest<p_Dest_सीमा) {
                *pDest++ = (uint8_t)ch;
            } अन्यथा {
                req_दीर्घता = 1;
                भङ्ग;
            }
        } अन्यथा यदि(ch <= 0x7ff) {
            यदि((p_Dest_सीमा - pDest) >= 2) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } अन्यथा {
                req_दीर्घता = 2;
                भङ्ग;
            }
        } अन्यथा {
            यदि((p_Dest_सीमा - pDest) >= 3) {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } अन्यथा {
                req_दीर्घता = 3;
                भङ्ग;
            }
        }
    }
    यावत्(src<p_Src_सीमा) {
        ch=*src++;
        यदि(ch <= 0x7f && ch != 0) {
            ++req_दीर्घता;
        } अन्यथा यदि(ch<=0x7ff) {
            req_दीर्घता+=2;
        } अन्यथा {
            req_दीर्घता+=3;
        }
    }

    req_दीर्घता+=(int32_t)(pDest - (uint8_t *)dest);
    यदि(p_Dest_दीर्घता){
        *p_Dest_दीर्घता = req_दीर्घता;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, dest_क्षमता, req_दीर्घता, p_Error_संहिता);
    प्रत्यागम dest;
}
