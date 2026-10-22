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


#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32WithSub(char16_t* dest U_LIFETIME_BOUND,
                      int32_t dest_ಸಾಮರ್ಥ್ಯ,
                      int32_t* p_Dest_ಉದ್ದಳತೆ,
                      ಸ್ಥಿರಾಂಕ UChar32* src,
                      int32_t src_ಉದ್ದಳತೆ,
                      UChar32 subchar,
                      int32_t* p_ಅಂಕೆ_Substitutions,
                      UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ UChar32 *src_ಎಲ್ಲೆ;
    UChar32 ch;
    char16_t *dest_ಎಲ್ಲೆ;
    char16_t *pDest;
    int32_t req_ಉದ್ದಳತೆ;
    int32_t ಅಂಕೆ_Substitutions;

    /* args check */
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ( (src==ಶೂನ್ಯಸೂಚಕ && src_ಉದ್ದಳತೆ!=0) || src_ಉದ್ದಳತೆ < -1 ||
        (dest_ಸಾಮರ್ಥ್ಯ<0) || (dest == ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ(p_ಅಂಕೆ_Substitutions != ಶೂನ್ಯಸೂಚಕ) {
        *p_ಅಂಕೆ_Substitutions = 0;
    }

    pDest = dest;
    dest_ಎಲ್ಲೆ = (dest!=ಶೂನ್ಯಸೂಚಕ)?(dest + dest_ಸಾಮರ್ಥ್ಯ):ಶೂನ್ಯಸೂಚಕ;
    req_ಉದ್ದಳತೆ = 0;
    ಅಂಕೆ_Substitutions = 0;

    ಆದರೆ(src_ಉದ್ದಳತೆ < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        ಆಗ((ch=*src) != 0 &&
              ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff))) {
            ++src;
            ಆದರೆ(pDest < dest_ಎಲ್ಲೆ) {
                *pDest++ = (char16_t)ch;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ++req_ಉದ್ದಳತೆ;
            }
        }
        src_ಎಲ್ಲೆ = src;
        ಆದರೆ(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            ಆಗ(*++src_ಎಲ್ಲೆ != 0) {}
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
      src_ಎಲ್ಲೆ = (src!=ಶೂನ್ಯಸೂಚಕ)?(src + src_ಉದ್ದಳತೆ):ಶೂನ್ಯಸೂಚಕ;
    }

    /* convert with length */
    ಆಗ(src < src_ಎಲ್ಲೆ) {
        ch = *src++;
        ಮಾಡು {
            /* usually "loops" once; twice only for writing subchar */
            ಆದರೆ((uint32_t)ch < 0xd800 || (0xe000 <= ch && ch <= 0xffff)) {
                ಆದರೆ(pDest < dest_ಎಲ್ಲೆ) {
                    *pDest++ = (char16_t)ch;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ++req_ಉದ್ದಳತೆ;
                }
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(0x10000 <= ch && ch <= 0x10ffff) {
                ಆದರೆ(pDest!=ಶೂನ್ಯಸೂಚಕ && ((pDest + 2) <= dest_ಎಲ್ಲೆ)) {
                    *pDest++ = U16_LEAD(ch);
                    *pDest++ = U16_TRAIL(ch);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    req_ಉದ್ದಳತೆ += 2;
                }
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ch = subchar) < 0) {
                /* surrogate code point, or not a Unicode code point at all */
                *p_Error_ಸಂಕೇತ = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ++ಅಂಕೆ_Substitutions;
            }
        } ಆಗ(ನಿಜ);
    }

    req_ಉದ್ದಳತೆ += (int32_t)(pDest - dest);
    ಆದರೆ(p_Dest_ಉದ್ದಳತೆ) {
        *p_Dest_ಉದ್ದಳತೆ = req_ಉದ್ದಳತೆ;
    }
    ಆದರೆ(p_ಅಂಕೆ_Substitutions != ಶೂನ್ಯಸೂಚಕ) {
        *p_ಅಂಕೆ_Substitutions = ಅಂಕೆ_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, req_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
    
    ಹಿಂತಿರುಗಿ dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF32(char16_t* dest U_LIFETIME_BOUND,
               int32_t dest_ಸಾಮರ್ಥ್ಯ,
               int32_t* p_Dest_ಉದ್ದಳತೆ,
               ಸ್ಥಿರಾಂಕ UChar32* src,
               int32_t src_ಉದ್ದಳತೆ,
               UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ u_strFromUTF32WithSub(
            dest, dest_ಸಾಮರ್ಥ್ಯ, p_Dest_ಉದ್ದಳತೆ,
            src, src_ಉದ್ದಳತೆ,
            U_SENTINEL, ಶೂನ್ಯಸೂಚಕ,
            p_Error_ಸಂಕೇತ);
}

U_CAPI UChar32* U_EXPORT2
u_str_ಗಮ್ಯ_UTF32_With_Sub(UChar32* dest U_LIFETIME_BOUND,
                    int32_t dest_ಸಾಮರ್ಥ್ಯ,
                    int32_t* p_Dest_ಉದ್ದಳತೆ,
                    ಸ್ಥಿರಾಂಕ char16_t* src,
                    int32_t src_ಉದ್ದಳತೆ,
                    UChar32 subchar,
                    int32_t* p_ಅಂಕೆ_Substitutions,
                    UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ char16_t *src_ಎಲ್ಲೆ;
    UChar32 ch;
    char16_t ch2;
    UChar32 *dest_ಎಲ್ಲೆ;
    UChar32 *pDest;
    int32_t req_ಉದ್ದಳತೆ;
    int32_t ಅಂಕೆ_Substitutions;

    /* args check */
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ( (src==ಶೂನ್ಯಸೂಚಕ && src_ಉದ್ದಳತೆ!=0) || src_ಉದ್ದಳತೆ < -1 ||
        (dest_ಸಾಮರ್ಥ್ಯ<0) || (dest == ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ(p_ಅಂಕೆ_Substitutions != ಶೂನ್ಯಸೂಚಕ) {
        *p_ಅಂಕೆ_Substitutions = 0;
    }

    pDest = dest;
    dest_ಎಲ್ಲೆ = (dest!=ಶೂನ್ಯಸೂಚಕ)?(dest + dest_ಸಾಮರ್ಥ್ಯ):ಶೂನ್ಯಸೂಚಕ;
    req_ಉದ್ದಳತೆ = 0;
    ಅಂಕೆ_Substitutions = 0;

    ಆದರೆ(src_ಉದ್ದಳತೆ < 0) {
        /* simple loop for conversion of a NUL-terminated BMP string */
        ಆಗ((ch=*src) != 0 && !U16_IS_SURROGATE(ch)) {
            ++src;
            ಆದರೆ(pDest < dest_ಎಲ್ಲೆ) {
                *pDest++ = ch;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ++req_ಉದ್ದಳತೆ;
            }
        }
        src_ಎಲ್ಲೆ = src;
        ಆದರೆ(ch != 0) {
            /* "complicated" case, find the end of the remaining string */
            ಆಗ(*++src_ಎಲ್ಲೆ != 0) {}
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        src_ಎಲ್ಲೆ = (src!=ಶೂನ್ಯಸೂಚಕ)?(src + src_ಉದ್ದಳತೆ):ಶೂನ್ಯಸೂಚಕ;
    }

    /* convert with length */
    ಆಗ(src < src_ಎಲ್ಲೆ) {
        ch = *src++;
        ಆದರೆ(!U16_IS_SURROGATE(ch)) {
            /* write or count ch below */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE_LEAD(ch) && src < src_ಎಲ್ಲೆ && U16_IS_TRAIL(ch2 = *src)) {
            ++src;
            ch = U16_GET_SUPPLEMENTARY(ch, ch2);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ch = subchar) < 0) {
            /* unpaired surrogate */
            *p_Error_ಸಂಕೇತ = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ++ಅಂಕೆ_Substitutions;
        }
        ಆದರೆ(pDest < dest_ಎಲ್ಲೆ) {
            *pDest++ = ch;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ++req_ಉದ್ದಳತೆ;
        }
    }

    req_ಉದ್ದಳತೆ += (int32_t)(pDest - dest);
    ಆದರೆ(p_Dest_ಉದ್ದಳತೆ) {
        *p_Dest_ಉದ್ದಳತೆ = req_ಉದ್ದಳತೆ;
    }
    ಆದರೆ(p_ಅಂಕೆ_Substitutions != ಶೂನ್ಯಸೂಚಕ) {
        *p_ಅಂಕೆ_Substitutions = ಅಂಕೆ_Substitutions;
    }

    /* Terminate the buffer */
    u_terminateUChar32s(dest, dest_ಸಾಮರ್ಥ್ಯ, req_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);

    ಹಿಂತಿರುಗಿ dest;
}

U_CAPI UChar32* U_EXPORT2
u_str_ಗಮ್ಯ_UTF32(UChar32* dest U_LIFETIME_BOUND,
             int32_t dest_ಸಾಮರ್ಥ್ಯ,
             int32_t* p_Dest_ಉದ್ದಳತೆ,
             ಸ್ಥಿರಾಂಕ char16_t* src,
             int32_t src_ಉದ್ದಳತೆ,
             UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ u_str_ಗಮ್ಯ_UTF32_With_Sub(
            dest, dest_ಸಾಮರ್ಥ್ಯ, p_Dest_ಉದ್ದಳತೆ,
            src, src_ಉದ್ದಳತೆ,
            U_SENTINEL, ಶೂನ್ಯಸೂಚಕ,
            p_Error_ಸಂಕೇತ);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                     int32_t dest_ಸಾಮರ್ಥ್ಯ,
                     int32_t* p_Dest_ಉದ್ದಳತೆ,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* src,
                     int32_t src_ಉದ್ದಳತೆ,
                     UChar32 subchar,
                     int32_t* p_ಅಂಕೆ_Substitutions,
                     UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {
    /* args check */
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ( (src==ಶೂನ್ಯಸೂಚಕ && src_ಉದ್ದಳತೆ!=0) || src_ಉದ್ದಳತೆ < -1 ||
        (dest_ಸಾಮರ್ಥ್ಯ<0) || (dest == ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ(p_ಅಂಕೆ_Substitutions!=ಶೂನ್ಯಸೂಚಕ) {
        *p_ಅಂಕೆ_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *p_Dest_ಎಲ್ಲೆ = dest+dest_ಸಾಮರ್ಥ್ಯ;
    int32_t req_ಉದ್ದಳತೆ = 0;
    int32_t ಅಂಕೆ_Substitutions=0;

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

    ಆದರೆ(src_ಉದ್ದಳತೆ < 0){
        /*
         * Transform a NUL-terminated string.
         * The code explicitly checks for NULs only in the lead byte position.
         * A NUL byte in the trail byte position fails the trail byte range check anyway.
         */
        int32_t i;
        UChar32 c;
        ಗಾಗಿ(i = 0; (c = (uint8_t)src[i]) != 0 && (pDest < p_Dest_ಎಲ್ಲೆ);) {
            // modified copy of U8_NEXT()
            ++i;
            ಆದರೆ(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } ಇಲ್ಲದಿದ್ದರೆ {
                uint8_t __t1, __t2;
                ಆದರೆ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_ಮುಂದಿನ_Char_ಸುರಕ್ಷಿತ_Body((ಸ್ಥಿರಾಂಕ uint8_t *)src, &(i), -1, c, -1);
                    ಆದರೆ(c<0 && (++ಅಂಕೆ_Substitutions, c = subchar) < 0) {
                        *p_Error_ಸಂಕೇತ = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        *(pDest++)=U16_LEAD(c);
                        ಆದರೆ(pDest<p_Dest_ಎಲ್ಲೆ) {
                            *(pDest++)=U16_TRAIL(c);
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            req_ಉದ್ದಳತೆ++;
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        ಆಗ((c = (uint8_t)src[i]) != 0) {
            // modified copy of U8_NEXT()
            ++i;
            ಆದರೆ(U8_IS_SINGLE(c)) {
                ++req_ಉದ್ದಳತೆ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                uint8_t __t1, __t2;
                ಆದರೆ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++req_ಉದ್ದಳತೆ;
                    i+=2;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++req_ಉದ್ದಳತೆ;
                    ++(i);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_ಮುಂದಿನ_Char_ಸುರಕ್ಷಿತ_Body((ಸ್ಥಿರಾಂಕ uint8_t *)src, &(i), -1, c, -1);
                    ಆದರೆ(c<0 && (++ಅಂಕೆ_Substitutions, c = subchar) < 0) {
                        *p_Error_ಸಂಕೇತ = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                    }
                    req_ಉದ್ದಳತೆ += U16_ಉದ್ದಳತೆ(c);
                }
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ /* srcLength >= 0 */ {
        /* Faster loop without ongoing checking for srcLength and pDestLimit. */
        int32_t i = 0;
        UChar32 c;
        ಗಾಗಿ(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            int32_t count = (int32_t)(p_Dest_ಎಲ್ಲೆ - pDest);
            int32_t count2 = (src_ಉದ್ದಳತೆ - i) / 3;
            ಆದರೆ(count > count2) {
                count = count2; /* min(remaining dest, remaining src/3) */
            }
            ಆದರೆ(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                ನಿಲ್ಲಿಸು;
            }

            ಮಾಡು {
                // modified copy of U8_NEXT()
                c = (uint8_t)src[i++];
                ಆದರೆ(U8_IS_SINGLE(c)) {
                    *pDest++=(char16_t)c;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    uint8_t __t1, __t2;
                    ಆದರೆ( /* handle U+0800..U+FFFF inline */
                            (0xe0<=(c) && (c)<0xf0) &&
                            ((i)+1)<src_ಉದ್ದಳತೆ &&
                            U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                            (__t2=src[(i)+1]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                        i+=2;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ( /* handle U+0080..U+07FF inline */
                            ((c)<0xe0 && (c)>=0xc2) &&
                            ((i)!=src_ಉದ್ದಳತೆ) &&
                            (__t1=src[i]-0x80)<=0x3f) {
                        *pDest++ = (((c)&0x1f)<<6)|__t1;
                        ++(i);
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ಆದರೆ(c >= 0xf0 || subchar > 0xffff) {
                            // We may read up to four bytes and write up to two UChars,
                            // which we didn't account for with computing count,
                            // so we adjust it here.
                            ಆದರೆ(--count == 0) {
                                --i;  // back out byte c
                                ನಿಲ್ಲಿಸು;
                            }
                        }

                        /* function call for "complicated" and error cases */
                        (c)=utf8_ಮುಂದಿನ_Char_ಸುರಕ್ಷಿತ_Body((ಸ್ಥಿರಾಂಕ uint8_t *)src, &(i), src_ಉದ್ದಳತೆ, c, -1);
                        ಆದರೆ(c<0 && (++ಅಂಕೆ_Substitutions, c = subchar) < 0) {
                            *p_Error_ಸಂಕೇತ = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0xFFFF) {
                            *(pDest++)=(char16_t)c;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            *(pDest++)=U16_LEAD(c);
                            *(pDest++)=U16_TRAIL(c);
                        }
                    }
                }
            } ಆಗ(--count > 0);
        }

        ಆಗ(i < src_ಉದ್ದಳತೆ && (pDest < p_Dest_ಎಲ್ಲೆ)) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            ಆದರೆ(U8_IS_SINGLE(c)) {
                *pDest++=(char16_t)c;
            } ಇಲ್ಲದಿದ್ದರೆ {
                uint8_t __t1, __t2;
                ಆದರೆ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<src_ಉದ್ದಳತೆ &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0xf)<<12)|((src[i]&0x3f)<<6)|__t2;
                    i+=2;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=src_ಉದ್ದಳತೆ) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    *pDest++ = (((c)&0x1f)<<6)|__t1;
                    ++(i);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_ಮುಂದಿನ_Char_ಸುರಕ್ಷಿತ_Body((ಸ್ಥಿರಾಂಕ uint8_t *)src, &(i), src_ಉದ್ದಳತೆ, c, -1);
                    ಆದರೆ(c<0 && (++ಅಂಕೆ_Substitutions, c = subchar) < 0) {
                        *p_Error_ಸಂಕೇತ = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0xFFFF) {
                        *(pDest++)=(char16_t)c;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        *(pDest++)=U16_LEAD(c);
                        ಆದರೆ(pDest<p_Dest_ಎಲ್ಲೆ) {
                            *(pDest++)=U16_TRAIL(c);
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            req_ಉದ್ದಳತೆ++;
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                }
            }
        }

        /* Pre-flight the rest of the string. */
        ಆಗ(i < src_ಉದ್ದಳತೆ) {
            // modified copy of U8_NEXT()
            c = (uint8_t)src[i++];
            ಆದರೆ(U8_IS_SINGLE(c)) {
                ++req_ಉದ್ದಳತೆ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                uint8_t __t1, __t2;
                ಆದರೆ( /* handle U+0800..U+FFFF inline */
                        (0xe0<=(c) && (c)<0xf0) &&
                        ((i)+1)<src_ಉದ್ದಳತೆ &&
                        U8_IS_VALID_LEAD3_AND_T1((c), src[i]) &&
                        (__t2=src[(i)+1]-0x80)<=0x3f) {
                    ++req_ಉದ್ದಳತೆ;
                    i+=2;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ( /* handle U+0080..U+07FF inline */
                        ((c)<0xe0 && (c)>=0xc2) &&
                        ((i)!=src_ಉದ್ದಳತೆ) &&
                        (__t1=src[i]-0x80)<=0x3f) {
                    ++req_ಉದ್ದಳತೆ;
                    ++(i);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* function call for "complicated" and error cases */
                    (c)=utf8_ಮುಂದಿನ_Char_ಸುರಕ್ಷಿತ_Body((ಸ್ಥಿರಾಂಕ uint8_t *)src, &(i), src_ಉದ್ದಳತೆ, c, -1);
                    ಆದರೆ(c<0 && (++ಅಂಕೆ_Substitutions, c = subchar) < 0) {
                        *p_Error_ಸಂಕೇತ = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                    }
                    req_ಉದ್ದಳತೆ += U16_ಉದ್ದಳತೆ(c);
                }
            }
        }
    }

    req_ಉದ್ದಳತೆ+=(int32_t)(pDest - dest);

    ಆದರೆ(p_ಅಂಕೆ_Substitutions!=ಶೂನ್ಯಸೂಚಕ) {
        *p_ಅಂಕೆ_Substitutions=ಅಂಕೆ_Substitutions;
    }

    ಆದರೆ(p_Dest_ಉದ್ದಳತೆ){
        *p_Dest_ಉದ್ದಳತೆ = req_ಉದ್ದಳತೆ;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_ಸಾಮರ್ಥ್ಯ,req_ಉದ್ದಳತೆ,p_Error_ಸಂಕೇತ);

    ಹಿಂತಿರುಗಿ dest;
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8(char16_t* dest U_LIFETIME_BOUND,
              int32_t dest_ಸಾಮರ್ಥ್ಯ,
              int32_t* p_Dest_ಉದ್ದಳತೆ,
              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* src,
              int32_t src_ಉದ್ದಳತೆ,
              UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ u_strFromUTF8WithSub(
            dest, dest_ಸಾಮರ್ಥ್ಯ, p_Dest_ಉದ್ದಳತೆ,
            src, src_ಉದ್ದಳತೆ,
            U_SENTINEL, ಶೂನ್ಯಸೂಚಕ,
            p_Error_ಸಂಕೇತ);
}

U_CAPI char16_t* U_EXPORT2
u_strFromUTF8Lenient(char16_t* dest U_LIFETIME_BOUND,
                     int32_t dest_ಸಾಮರ್ಥ್ಯ,
                     int32_t* p_Dest_ಉದ್ದಳತೆ,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* src,
                     int32_t src_ಉದ್ದಳತೆ,
                     UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {
    char16_t *pDest = dest;
    UChar32 ch;
    int32_t req_ಉದ್ದಳತೆ = 0;
    uint8_t* pSrc = (uint8_t*) src;

    /* args check */
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
        
    ಆದರೆ( (src==ಶೂನ್ಯಸೂಚಕ && src_ಉದ್ದಳತೆ!=0) || src_ಉದ್ದಳತೆ < -1 ||
        (dest_ಸಾಮರ್ಥ್ಯ<0) || (dest == ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ > 0)
    ) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ(src_ಉದ್ದಳತೆ < 0) {
        /* Transform a NUL-terminated string. */
        char16_t *p_Dest_ಎಲ್ಲೆ = (dest!=ಶೂನ್ಯಸೂಚಕ)?(dest+dest_ಸಾಮರ್ಥ್ಯ):ಶೂನ್ಯಸೂಚಕ;
        uint8_t t1, t2, t3; /* trail bytes */

        ಆಗ(((ch = *pSrc) != 0) && (pDest < p_Dest_ಎಲ್ಲೆ)) {
            ಆದರೆ(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ++pSrc;
                ಮುಂದುವರಿ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch < 0xe0) { /* U+0080..U+07FF */
                ಆದರೆ((t1 = pSrc[1]) != 0) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + t1 - 0x3080);
                    pSrc += 2;
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch < 0xf0) { /* U+0800..U+FFFF */
                ಆದರೆ((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 12) + (t1 << 6) + t2 - 0x2080);
                    pSrc += 3;
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ /* f0..f4 */ { /* U+10000..U+10FFFF */
                ಆದರೆ((t1 = pSrc[1]) != 0 && (t2 = pSrc[2]) != 0 && (t3 = pSrc[3]) != 0) {
                    pSrc += 4;
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (t1 << 12) + (t2 << 6) + t3 - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    ಆದರೆ(pDest < p_Dest_ಎಲ್ಲೆ) {
                        *(pDest++) = U16_TRAIL(ch);
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        req_ಉದ್ದಳತೆ = 1;
                        ನಿಲ್ಲಿಸು;
                    }
                    ಮುಂದುವರಿ;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            ಆಗ(*++pSrc != 0) {}
            ನಿಲ್ಲಿಸು;
        }

        /* Pre-flight the rest of the string. */
        ಆಗ((ch = *pSrc) != 0) {
            ಆದರೆ(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                ++req_ಉದ್ದಳತೆ;
                ++pSrc;
                ಮುಂದುವರಿ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch < 0xe0) { /* U+0080..U+07FF */
                ಆದರೆ(pSrc[1] != 0) {
                    ++req_ಉದ್ದಳತೆ;
                    pSrc += 2;
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch < 0xf0) { /* U+0800..U+FFFF */
                ಆದರೆ(pSrc[1] != 0 && pSrc[2] != 0) {
                    ++req_ಉದ್ದಳತೆ;
                    pSrc += 3;
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ /* f0..f4 */ { /* U+10000..U+10FFFF */
                ಆದರೆ(pSrc[1] != 0 && pSrc[2] != 0 && pSrc[3] != 0) {
                    req_ಉದ್ದಳತೆ += 2;
                    pSrc += 4;
                    ಮುಂದುವರಿ;
                }
            }

            /* truncated character at the end */
            ++req_ಉದ್ದಳತೆ;
            ನಿಲ್ಲಿಸು;
        }
    } ಇಲ್ಲದಿದ್ದರೆ /* srcLength >= 0 */ {
      ಸ್ಥಿರಾಂಕ uint8_t *p_Src_ಎಲ್ಲೆ = (pSrc!=ಶೂನ್ಯಸೂಚಕ)?(pSrc + src_ಉದ್ದಳತೆ):ಶೂನ್ಯಸೂಚಕ;

        /*
         * This function requires that if srcLength is given, then it must be
         * destCapatity >= srcLength so that we need not check for
         * destination buffer overflow in the loop.
         */
        ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ < src_ಉದ್ದಳತೆ) {
            ಆದರೆ(p_Dest_ಉದ್ದಳತೆ != ಶೂನ್ಯಸೂಚಕ) {
                *p_Dest_ಉದ್ದಳತೆ = src_ಉದ್ದಳತೆ; /* this likely overestimates the true destLength! */
            }
            *p_Error_ಸಂಕೇತ = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }

        ಆದರೆ((p_Src_ಎಲ್ಲೆ - pSrc) >= 4) {
            p_Src_ಎಲ್ಲೆ -= 3; /* temporarily reduce pSrcLimit */

            /* in this loop, we can always access at least 4 bytes, up to pSrc+3 */
            ಮಾಡು {
                ch = *pSrc++;
                ಆದರೆ(ch < 0xc0) {
                    /*
                     * ASCII, or a trail byte in lead position which is treated like
                     * a single-byte sequence for better character boundary
                     * resynchronization after illegal sequences.
                     */
                    *pDest++=(char16_t)ch;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch < 0xe0) { /* U+0080..U+07FF */
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch < 0xf0) { /* U+0800..U+FFFF */
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                } ಇಲ್ಲದಿದ್ದರೆ /* f0..f4 */ { /* U+10000..U+10FFFF */
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                }
            } ಆಗ(pSrc < p_Src_ಎಲ್ಲೆ);

            p_Src_ಎಲ್ಲೆ += 3; /* restore original pSrcLimit */
        }

        ಆಗ(pSrc < p_Src_ಎಲ್ಲೆ) {
            ch = *pSrc++;
            ಆದರೆ(ch < 0xc0) {
                /*
                 * ASCII, or a trail byte in lead position which is treated like
                 * a single-byte sequence for better character boundary
                 * resynchronization after illegal sequences.
                 */
                *pDest++=(char16_t)ch;
                ಮುಂದುವರಿ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch < 0xe0) { /* U+0080..U+07FF */
                ಆದರೆ(pSrc < p_Src_ಎಲ್ಲೆ) {
                    /* 0x3080 = (0xc0 << 6) + 0x80 */
                    *pDest++ = (char16_t)((ch << 6) + *pSrc++ - 0x3080);
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch < 0xf0) { /* U+0800..U+FFFF */
                ಆದರೆ((p_Src_ಎಲ್ಲೆ - pSrc) >= 2) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    /* 0x2080 = (0x80 << 6) + 0x80 */
                    ch = (ch << 12) + (*pSrc++ << 6);
                    *pDest++ = (char16_t)(ch + *pSrc++ - 0x2080);
                    pSrc += 3;
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ /* f0..f4 */ { /* U+10000..U+10FFFF */
                ಆದರೆ((p_Src_ಎಲ್ಲೆ - pSrc) >= 3) {
                    /* 0x3c82080 = (0xf0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
                    ch = (ch << 18) + (*pSrc++ << 12);
                    ch += *pSrc++ << 6;
                    ch += *pSrc++ - 0x3c82080;
                    *(pDest++) = U16_LEAD(ch);
                    *(pDest++) = U16_TRAIL(ch);
                    pSrc += 4;
                    ಮುಂದುವರಿ;
                }
            }

            /* truncated character at the end */
            *pDest++ = 0xfffd;
            ನಿಲ್ಲಿಸು;
        }
    }

    req_ಉದ್ದಳತೆ+=(int32_t)(pDest - dest);

    ಆದರೆ(p_Dest_ಉದ್ದಳತೆ){
        *p_Dest_ಉದ್ದಳತೆ = req_ಉದ್ದಳತೆ;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_ಸಾಮರ್ಥ್ಯ,req_ಉದ್ದಳತೆ,p_Error_ಸಂಕೇತ);

    ಹಿಂತಿರುಗಿ dest;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint8_t *
_appendUTF8(uint8_t *pDest, UChar32 c) {
    /* it is 0<=c<=0x10ffff and not a surrogate if called by a validating function */
    ಆದರೆ((c)<=0x7f) {
        *pDest++ = static_cast<uint8_t>(c);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0x7ff) {
        *pDest++ = static_cast<uint8_t>((c >> 6) | 0xc0);
        *pDest++ = static_cast<uint8_t>((c & 0x3f) | 0x80);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0xffff) {
        *pDest++ = static_cast<uint8_t>((c >> 12) | 0xe0);
        *pDest++ = static_cast<uint8_t>(((c >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    } ಇಲ್ಲದಿದ್ದರೆ /* if((uint32_t)(c)<=0x10ffff) */ {
        *pDest++ = static_cast<uint8_t>(((c) >> 18) | 0xf0);
        *pDest++ = static_cast<uint8_t>((((c) >> 12) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>((((c) >> 6) & 0x3f) | 0x80);
        *pDest++ = static_cast<uint8_t>(((c) & 0x3f) | 0x80);
    }
    ಹಿಂತಿರುಗಿ pDest;
}

U_CAPI ಅಕ್ಷರ* U_EXPORT2
u_str_ಗಮ್ಯ_UTF8_With_Sub(ಅಕ್ಷರ* dest U_LIFETIME_BOUND,
                   int32_t dest_ಸಾಮರ್ಥ್ಯ,
                   int32_t* p_Dest_ಉದ್ದಳತೆ,
                   ಸ್ಥಿರಾಂಕ char16_t* pSrc,
                   int32_t src_ಉದ್ದಳತೆ,
                   UChar32 subchar,
                   int32_t* p_ಅಂಕೆ_Substitutions,
                   UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {
    int32_t req_ಉದ್ದಳತೆ=0;
    uint32_t ch=0,ch2=0;
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *p_Dest_ಎಲ್ಲೆ = (pDest!=ಶೂನ್ಯಸೂಚಕ)?(pDest + dest_ಸಾಮರ್ಥ್ಯ):ಶೂನ್ಯಸೂಚಕ;
    int32_t ಅಂಕೆ_Substitutions;

    /* args check */
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
        
    ಆದರೆ( (pSrc==ಶೂನ್ಯಸೂಚಕ && src_ಉದ್ದಳತೆ!=0) || src_ಉದ್ದಳತೆ < -1 ||
        (dest_ಸಾಮರ್ಥ್ಯ<0) || (dest == ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ > 0) ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ(p_ಅಂಕೆ_Substitutions!=ಶೂನ್ಯಸೂಚಕ) {
        *p_ಅಂಕೆ_Substitutions=0;
    }
    ಅಂಕೆ_Substitutions=0;

    ಆದರೆ(src_ಉದ್ದಳತೆ==-1) {
        ಆಗ((ch=*pSrc)!=0) {
            ++pSrc;
            ಆದರೆ(ch <= 0x7f) {
                ಆದರೆ(pDest<p_Dest_ಎಲ್ಲೆ) {
                    *pDest++ = (uint8_t)ch;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    req_ಉದ್ದಳತೆ = 1;
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch <= 0x7ff) {
                ಆದರೆ((p_Dest_ಎಲ್ಲೆ - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    req_ಉದ್ದಳತೆ = 2;
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch <= 0xd7ff || ch >= 0xe000) {
                ಆದರೆ((p_Dest_ಎಲ್ಲೆ - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    req_ಉದ್ದಳತೆ = 3;
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ /* ch is a surrogate */ {
                int32_t length;

                /*need not check for NUL because NUL fails U16_IS_TRAIL() anyway*/
                ಆದರೆ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(subchar>=0) {
                    ch=subchar;
                    ++ಅಂಕೆ_Substitutions;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *p_Error_ಸಂಕೇತ = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                }

                length = U8_ಉದ್ದಳತೆ(ch);
                ಆದರೆ((p_Dest_ಎಲ್ಲೆ - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    req_ಉದ್ದಳತೆ = length;
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
        ಆಗ((ch=*pSrc++)!=0) {
            ಆದರೆ(ch<=0x7f) {
                ++req_ಉದ್ದಳತೆ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch<=0x7ff) {
                req_ಉದ್ದಳತೆ+=2;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!U16_IS_SURROGATE(ch)) {
                req_ಉದ್ದಳತೆ+=3;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                req_ಉದ್ದಳತೆ+=4;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(subchar>=0) {
                req_ಉದ್ದಳತೆ+=U8_ಉದ್ದಳತೆ(subchar);
                ++ಅಂಕೆ_Substitutions;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *p_Error_ಸಂಕೇತ = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಸ್ಥಿರಾಂಕ char16_t *p_Src_ಎಲ್ಲೆ = (pSrc!=ಶೂನ್ಯಸೂಚಕ)?(pSrc+src_ಉದ್ದಳತೆ):ಶೂನ್ಯಸೂಚಕ;
        int32_t count;

        /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
        ಗಾಗಿ(;;) {
            /*
             * Each iteration of the inner loop progresses by at most 3 UTF-8
             * bytes and one char16_t, for most characters.
             * For supplementary code points (4 & 2), which are rare,
             * there is an additional adjustment.
             */
            count = (int32_t)((p_Dest_ಎಲ್ಲೆ - pDest) / 3);
            src_ಉದ್ದಳತೆ = (int32_t)(p_Src_ಎಲ್ಲೆ - pSrc);
            ಆದರೆ(count > src_ಉದ್ದಳತೆ) {
                count = src_ಉದ್ದಳತೆ; /* min(remaining dest/3, remaining src) */
            }
            ಆದರೆ(count < 3) {
                /*
                 * Too much overhead if we get near the end of the string,
                 * continue with the next loop.
                 */
                ನಿಲ್ಲಿಸು;
            }
            ಮಾಡು {
                ch=*pSrc++;
                ಆದರೆ(ch <= 0x7f) {
                    *pDest++ = (uint8_t)ch;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch <= 0x7ff) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch <= 0xd7ff || ch >= 0xe000) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ಇಲ್ಲದಿದ್ದರೆ /* ch is a surrogate */ {
                    /*
                     * We will read two UChars and probably output four bytes,
                     * which we didn't account for with computing count,
                     * so we adjust it here.
                     */
                    ಆದರೆ(--count == 0) {
                        --pSrc; /* undo ch=*pSrc++ for the lead surrogate */
                        ನಿಲ್ಲಿಸು;  /* recompute count */
                    }

                    ಆದರೆ(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(ch2=*pSrc)) { 
                        ++pSrc;
                        ch=U16_GET_SUPPLEMENTARY(ch, ch2);

                        /* writing 4 bytes per 2 UChars is ok */
                        *pDest++=(uint8_t)((ch>>18)|0xf0);
                        *pDest++=(uint8_t)(((ch>>12)&0x3f)|0x80);
                        *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                        *pDest++=(uint8_t)((ch&0x3f)|0x80);
                    } ಇಲ್ಲದಿದ್ದರೆ  {
                        /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                        ಆದರೆ(subchar>=0) {
                            ch=subchar;
                            ++ಅಂಕೆ_Substitutions;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            *p_Error_ಸಂಕೇತ = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                        }

                        /* convert and append*/
                        pDest=_appendUTF8(pDest, ch);
                    }
                }
            } ಆಗ(--count > 0);
        }

        ಆಗ(pSrc<p_Src_ಎಲ್ಲೆ) {
            ch=*pSrc++;
            ಆದರೆ(ch <= 0x7f) {
                ಆದರೆ(pDest<p_Dest_ಎಲ್ಲೆ) {
                    *pDest++ = (uint8_t)ch;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    req_ಉದ್ದಳತೆ = 1;
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch <= 0x7ff) {
                ಆದರೆ((p_Dest_ಎಲ್ಲೆ - pDest) >= 2) {
                    *pDest++=(uint8_t)((ch>>6)|0xc0);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    req_ಉದ್ದಳತೆ = 2;
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch <= 0xd7ff || ch >= 0xe000) {
                ಆದರೆ((p_Dest_ಎಲ್ಲೆ - pDest) >= 3) {
                    *pDest++=(uint8_t)((ch>>12)|0xe0);
                    *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                    *pDest++=(uint8_t)((ch&0x3f)|0x80);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    req_ಉದ್ದಳತೆ = 3;
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ /* ch is a surrogate */ {
                int32_t length;

                ಆದರೆ(U16_IS_SURROGATE_LEAD(ch) && pSrc<p_Src_ಎಲ್ಲೆ && U16_IS_TRAIL(ch2=*pSrc)) { 
                    ++pSrc;
                    ch=U16_GET_SUPPLEMENTARY(ch, ch2);
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(subchar>=0) {
                    ch=subchar;
                    ++ಅಂಕೆ_Substitutions;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                    *p_Error_ಸಂಕೇತ = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                }

                length = U8_ಉದ್ದಳತೆ(ch);
                ಆದರೆ((p_Dest_ಎಲ್ಲೆ - pDest) >= length) {
                    /* convert and append*/
                    pDest=_appendUTF8(pDest, ch);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    req_ಉದ್ದಳತೆ = length;
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
        ಆಗ(pSrc<p_Src_ಎಲ್ಲೆ) {
            ch=*pSrc++;
            ಆದರೆ(ch<=0x7f) {
                ++req_ಉದ್ದಳತೆ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch<=0x7ff) {
                req_ಉದ್ದಳತೆ+=2;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!U16_IS_SURROGATE(ch)) {
                req_ಉದ್ದಳತೆ+=3;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE_LEAD(ch) && pSrc<p_Src_ಎಲ್ಲೆ && U16_IS_TRAIL(ch2=*pSrc)) {
                ++pSrc;
                req_ಉದ್ದಳತೆ+=4;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(subchar>=0) {
                req_ಉದ್ದಳತೆ+=U8_ಉದ್ದಳತೆ(subchar);
                ++ಅಂಕೆ_Substitutions;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* Unicode 3.2 forbids surrogate code points in UTF-8 */
                *p_Error_ಸಂಕೇತ = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
        }
    }

    req_ಉದ್ದಳತೆ+=(int32_t)(pDest - (uint8_t *)dest);

    ಆದರೆ(p_ಅಂಕೆ_Substitutions!=ಶೂನ್ಯಸೂಚಕ) {
        *p_ಅಂಕೆ_Substitutions=ಅಂಕೆ_Substitutions;
    }

    ಆದರೆ(p_Dest_ಉದ್ದಳತೆ){
        *p_Dest_ಉದ್ದಳತೆ = req_ಉದ್ದಳತೆ;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, dest_ಸಾಮರ್ಥ್ಯ, req_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ dest;
}

U_CAPI ಅಕ್ಷರ* U_EXPORT2
u_str_ಗಮ್ಯ_UTF8(ಅಕ್ಷರ* dest U_LIFETIME_BOUND,
            int32_t dest_ಸಾಮರ್ಥ್ಯ,
            int32_t* p_Dest_ಉದ್ದಳತೆ,
            ಸ್ಥಿರಾಂಕ char16_t* pSrc,
            int32_t src_ಉದ್ದಳತೆ,
            UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ u_str_ಗಮ್ಯ_UTF8_With_Sub(
            dest, dest_ಸಾಮರ್ಥ್ಯ, p_Dest_ಉದ್ದಳತೆ,
            pSrc, src_ಉದ್ದಳತೆ,
            U_SENTINEL, ಶೂನ್ಯಸೂಚಕ,
            p_Error_ಸಂಕೇತ);
}

U_CAPI char16_t* U_EXPORT2
u_strFromJavaModifiedUTF8WithSub(char16_t* dest U_LIFETIME_BOUND,
                                 int32_t dest_ಸಾಮರ್ಥ್ಯ,
                                 int32_t* p_Dest_ಉದ್ದಳತೆ,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* src,
                                 int32_t src_ಉದ್ದಳತೆ,
                                 UChar32 subchar,
                                 int32_t* p_ಅಂಕೆ_Substitutions,
                                 UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {
    /* args check */
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ( (src==ಶೂನ್ಯಸೂಚಕ && src_ಉದ್ದಳತೆ!=0) || src_ಉದ್ದಳತೆ < -1 ||
        (dest==ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ!=0) || dest_ಸಾಮರ್ಥ್ಯ<0 ||
        subchar > 0x10ffff || U_IS_SURROGATE(subchar)
    ) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ(p_ಅಂಕೆ_Substitutions!=ಶೂನ್ಯಸೂಚಕ) {
        *p_ಅಂಕೆ_Substitutions=0;
    }
    char16_t *pDest = dest;
    char16_t *p_Dest_ಎಲ್ಲೆ = dest+dest_ಸಾಮರ್ಥ್ಯ;
    int32_t req_ಉದ್ದಳತೆ = 0;
    int32_t ಅಂಕೆ_Substitutions=0;

    ಆದರೆ(src_ಉದ್ದಳತೆ < 0) {
        /*
         * Transform a NUL-terminated ASCII string.
         * Handle non-ASCII strings with slower code.
         */
        UChar32 c;
        ಆಗ(((c = (uint8_t)*src) != 0) && c <= 0x7f && (pDest < p_Dest_ಎಲ್ಲೆ)) {
            *pDest++=(char16_t)c;
            ++src;
        }
        ಆದರೆ(c == 0) {
            req_ಉದ್ದಳತೆ=(int32_t)(pDest - dest);
            ಆದರೆ(p_Dest_ಉದ್ದಳತೆ) {
                *p_Dest_ಉದ್ದಳತೆ = req_ಉದ್ದಳತೆ;
            }

            /* Terminate the buffer */
            u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, req_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
            ಹಿಂತಿರುಗಿ dest;
        }
        src_ಉದ್ದಳತೆ = static_cast<int32_t>(uprv_strlen(src));
    }

    /* Faster loop without ongoing checking for srcLength and pDestLimit. */
    UChar32 ch;
    uint8_t t1, t2;
    int32_t i = 0;
    ಗಾಗಿ(;;) {
        int32_t count = (int32_t)(p_Dest_ಎಲ್ಲೆ - pDest);
        int32_t count2 = src_ಉದ್ದಳತೆ - i;
        ಆದರೆ(count >= count2 && src_ಉದ್ದಳತೆ > 0 && U8_IS_SINGLE(*src)) {
            /* fast ASCII loop */
            int32_t ಆರಂಭ = i;
            uint8_t b;
            ಆಗ(i < src_ಉದ್ದಳತೆ && U8_IS_SINGLE(b = src[i])) {
                *pDest++=b;
                ++i;
            }
            int32_t delta = i - ಆರಂಭ;
            count -= delta;
            count2 -= delta;
        }
        /*
         * Each iteration of the inner loop progresses by at most 3 UTF-8
         * bytes and one char16_t.
         */
        ಆದರೆ(subchar > 0xFFFF) {
            ನಿಲ್ಲಿಸು;
        }
        count2 /= 3;
        ಆದರೆ(count > count2) {
            count = count2; /* min(remaining dest, remaining src/3) */
        }
        ಆದರೆ(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            ನಿಲ್ಲಿಸು;
        }
        ಮಾಡು {
            ch = (uint8_t)src[i++];
            ಆದರೆ(U8_IS_SINGLE(ch)) {
                *pDest++=(char16_t)ch;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ(ch >= 0xe0) {
                    ಆದರೆ( /* handle U+0000..U+FFFF inline */
                        ch <= 0xef &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                        (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                    ) {
                        /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                        *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                        i += 2;
                        ಮುಂದುವರಿ;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಆದರೆ( /* handle U+0000..U+07FF inline */
                        ch >= 0xc0 &&
                        (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                    ) {
                        *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                        ++i;
                        ಮುಂದುವರಿ;
                    }
                }

                ಆದರೆ(subchar < 0) {
                    *p_Error_ಸಂಕೇತ = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(subchar > 0xffff && --count == 0) {
                    /*
                     * We need to write two UChars, adjusted count for that,
                     * and ran out of space.
                     */
                    --i;  // back out byte ch
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* function call for error cases */
                    utf8_ಮುಂದಿನ_Char_ಸುರಕ್ಷಿತ_Body((ಸ್ಥಿರಾಂಕ uint8_t *)src, &(i), src_ಉದ್ದಳತೆ, ch, -1);
                    ++ಅಂಕೆ_Substitutions;
                    *(pDest++)=(char16_t)subchar;
                }
            }
        } ಆಗ(--count > 0);
    }

    ಆಗ(i < src_ಉದ್ದಳತೆ && (pDest < p_Dest_ಎಲ್ಲೆ)) {
        ch = (uint8_t)src[i++];
        ಆದರೆ(U8_IS_SINGLE(ch)){
            *pDest++=(char16_t)ch;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(ch >= 0xe0) {
                ಆದರೆ( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < src_ಉದ್ದಳತೆ &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f &&
                    (t2 = (uint8_t)(src[i+1] - 0x80)) <= 0x3f
                ) {
                    /* no need for (ch & 0xf) because the upper bits are truncated after <<12 in the cast to (char16_t) */
                    *pDest++ = (char16_t)((ch << 12) | (t1 << 6) | t2);
                    i += 2;
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < src_ಉದ್ದಳತೆ &&
                    (t1 = (uint8_t)(src[i] - 0x80)) <= 0x3f
                ) {
                    *pDest++ = (char16_t)(((ch & 0x1f) << 6) | t1);
                    ++i;
                    ಮುಂದುವರಿ;
                }
            }

            ಆದರೆ(subchar < 0) {
                *p_Error_ಸಂಕೇತ = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* function call for error cases */
                utf8_ಮುಂದಿನ_Char_ಸುರಕ್ಷಿತ_Body((ಸ್ಥಿರಾಂಕ uint8_t *)src, &(i), src_ಉದ್ದಳತೆ, ch, -1);
                ++ಅಂಕೆ_Substitutions;
                ಆದರೆ(subchar<=0xFFFF) {
                    *(pDest++)=(char16_t)subchar;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    *(pDest++)=U16_LEAD(subchar);
                    ಆದರೆ(pDest<p_Dest_ಎಲ್ಲೆ) {
                        *(pDest++)=U16_TRAIL(subchar);
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        req_ಉದ್ದಳತೆ++;
                        ನಿಲ್ಲಿಸು;
                    }
                }
            }
        }
    }

    /* Pre-flight the rest of the string. */
    ಆಗ(i < src_ಉದ್ದಳತೆ) {
        ch = (uint8_t)src[i++];
        ಆದರೆ(U8_IS_SINGLE(ch)) {
            req_ಉದ್ದಳತೆ++;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(ch >= 0xe0) {
                ಆದರೆ( /* handle U+0000..U+FFFF inline */
                    ch <= 0xef &&
                    (i+1) < src_ಉದ್ದಳತೆ &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f &&
                    (uint8_t)(src[i+1] - 0x80) <= 0x3f
                ) {
                    req_ಉದ್ದಳತೆ++;
                    i += 2;
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ( /* handle U+0000..U+07FF inline */
                    ch >= 0xc0 &&
                    i < src_ಉದ್ದಳತೆ &&
                    (uint8_t)(src[i] - 0x80) <= 0x3f
                ) {
                    req_ಉದ್ದಳತೆ++;
                    ++i;
                    ಮುಂದುವರಿ;
                }
            }

            ಆದರೆ(subchar < 0) {
                *p_Error_ಸಂಕೇತ = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* function call for error cases */
                utf8_ಮುಂದಿನ_Char_ಸುರಕ್ಷಿತ_Body((ಸ್ಥಿರಾಂಕ uint8_t *)src, &(i), src_ಉದ್ದಳತೆ, ch, -1);
                ++ಅಂಕೆ_Substitutions;
                req_ಉದ್ದಳತೆ+=U16_ಉದ್ದಳತೆ(ch);
            }
        }
    }

    ಆದರೆ(p_ಅಂಕೆ_Substitutions!=ಶೂನ್ಯಸೂಚಕ) {
        *p_ಅಂಕೆ_Substitutions=ಅಂಕೆ_Substitutions;
    }

    req_ಉದ್ದಳತೆ+=(int32_t)(pDest - dest);
    ಆದರೆ(p_Dest_ಉದ್ದಳತೆ) {
        *p_Dest_ಉದ್ದಳತೆ = req_ಉದ್ದಳತೆ;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, req_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ dest;
}

U_CAPI ಅಕ್ಷರ* U_EXPORT2
u_str_ಗಮ್ಯ_Java_Modified_UTF8(ಅಕ್ಷರ* dest U_LIFETIME_BOUND,
                        int32_t dest_ಸಾಮರ್ಥ್ಯ,
                        int32_t* p_Dest_ಉದ್ದಳತೆ,
                        ಸ್ಥಿರಾಂಕ char16_t* src,
                        int32_t src_ಉದ್ದಳತೆ,
                        UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {
    int32_t req_ಉದ್ದಳತೆ=0;
    uint32_t ch=0;
    ಸ್ಥಿರಾಂಕ char16_t *p_Src_ಎಲ್ಲೆ;
    int32_t count;

    /* args check */
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ( (src==ಶೂನ್ಯಸೂಚಕ && src_ಉದ್ದಳತೆ!=0) || src_ಉದ್ದಳತೆ < -1 ||
        (dest==ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ!=0) || dest_ಸಾಮರ್ಥ್ಯ<0
    ) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t *p_Dest_ಎಲ್ಲೆ = pDest + dest_ಸಾಮರ್ಥ್ಯ;

    ಆದರೆ(src_ಉದ್ದಳತೆ==-1) {
        /* Convert NUL-terminated ASCII, then find the string length. */
        ಆಗ((ch=*src)<=0x7f && ch != 0 && pDest<p_Dest_ಎಲ್ಲೆ) {
            *pDest++ = (uint8_t)ch;
            ++src;
        }
        ಆದರೆ(ch == 0) {
            req_ಉದ್ದಳತೆ=(int32_t)(pDest - (uint8_t *)dest);
            ಆದರೆ(p_Dest_ಉದ್ದಳತೆ) {
                *p_Dest_ಉದ್ದಳತೆ = req_ಉದ್ದಳತೆ;
            }

            /* Terminate the buffer */
            u_terminateChars(dest, dest_ಸಾಮರ್ಥ್ಯ, req_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
            ಹಿಂತಿರುಗಿ dest;
        }
        src_ಉದ್ದಳತೆ = u_strlen(src);
    }

    /* Faster loop without ongoing checking for pSrcLimit and pDestLimit. */
    p_Src_ಎಲ್ಲೆ = (src!=ಶೂನ್ಯಸೂಚಕ)?(src+src_ಉದ್ದಳತೆ):ಶೂನ್ಯಸೂಚಕ;
    ಗಾಗಿ(;;) {
        count = (int32_t)(p_Dest_ಎಲ್ಲೆ - pDest);
        src_ಉದ್ದಳತೆ = (int32_t)(p_Src_ಎಲ್ಲೆ - src);
        ಆದರೆ(count >= src_ಉದ್ದಳತೆ && src_ಉದ್ದಳತೆ > 0 && *src <= 0x7f) {
            /* fast ASCII loop */
            ಸ್ಥಿರಾಂಕ char16_t *prevSrc = src;
            int32_t delta;
            ಆಗ(src < p_Src_ಎಲ್ಲೆ && (ch = *src) <= 0x7f && ch != 0) {
                *pDest++=(uint8_t)ch;
                ++src;
            }
            delta = (int32_t)(src - prevSrc);
            count -= delta;
            src_ಉದ್ದಳತೆ -= delta;
        }
        /*
         * Each iteration of the inner loop progresses by at most 3 UTF-8
         * bytes and one char16_t.
         */
        count /= 3;
        ಆದರೆ(count > src_ಉದ್ದಳತೆ) {
            count = src_ಉದ್ದಳತೆ; /* min(remaining dest/3, remaining src) */
        }
        ಆದರೆ(count < 3) {
            /*
             * Too much overhead if we get near the end of the string,
             * continue with the next loop.
             */
            ನಿಲ್ಲಿಸು;
        }
        ಮಾಡು {
            ch=*src++;
            ಆದರೆ(ch <= 0x7f && ch != 0) {
                *pDest++ = (uint8_t)ch;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch <= 0x7ff) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } ಇಲ್ಲದಿದ್ದರೆ {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            }
        } ಆಗ(--count > 0);
    }

    ಆಗ(src<p_Src_ಎಲ್ಲೆ) {
        ch=*src++;
        ಆದರೆ(ch <= 0x7f && ch != 0) {
            ಆದರೆ(pDest<p_Dest_ಎಲ್ಲೆ) {
                *pDest++ = (uint8_t)ch;
            } ಇಲ್ಲದಿದ್ದರೆ {
                req_ಉದ್ದಳತೆ = 1;
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch <= 0x7ff) {
            ಆದರೆ((p_Dest_ಎಲ್ಲೆ - pDest) >= 2) {
                *pDest++=(uint8_t)((ch>>6)|0xc0);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } ಇಲ್ಲದಿದ್ದರೆ {
                req_ಉದ್ದಳತೆ = 2;
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ((p_Dest_ಎಲ್ಲೆ - pDest) >= 3) {
                *pDest++=(uint8_t)((ch>>12)|0xe0);
                *pDest++=(uint8_t)(((ch>>6)&0x3f)|0x80);
                *pDest++=(uint8_t)((ch&0x3f)|0x80);
            } ಇಲ್ಲದಿದ್ದರೆ {
                req_ಉದ್ದಳತೆ = 3;
                ನಿಲ್ಲಿಸು;
            }
        }
    }
    ಆಗ(src<p_Src_ಎಲ್ಲೆ) {
        ch=*src++;
        ಆದರೆ(ch <= 0x7f && ch != 0) {
            ++req_ಉದ್ದಳತೆ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch<=0x7ff) {
            req_ಉದ್ದಳತೆ+=2;
        } ಇಲ್ಲದಿದ್ದರೆ {
            req_ಉದ್ದಳತೆ+=3;
        }
    }

    req_ಉದ್ದಳತೆ+=(int32_t)(pDest - (uint8_t *)dest);
    ಆದರೆ(p_Dest_ಉದ್ದಳತೆ){
        *p_Dest_ಉದ್ದಳತೆ = req_ಉದ್ದಳತೆ;
    }

    /* Terminate the buffer */
    u_terminateChars(dest, dest_ಸಾಮರ್ಥ್ಯ, req_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ dest;
}
