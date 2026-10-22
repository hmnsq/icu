// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  utf_impl.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999sep13
*   created by: Markus W. Scherer
*
*   This file provides implementation functions for macros in the utfXX.h
*   that would otherwise be too long as macros.
*/

/* set import/export definitions */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_UTF8_IMPL
#   define U_UTF8_IMPL
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

/*
 * Table of the number of utf8 trail bytes, indexed by the lead byte.
 * Used by the deprecated macro UTF8_COUNT_TRAIL_BYTES, defined in utf_old.h
 *
 * The current macro, U8_COUNT_TRAIL_BYTES, does _not_ use this table.
 *
 * Note that this table cannot be removed, even if UTF8_COUNT_TRAIL_BYTES were
 * changed to no longer use it. References to the table from expansions of UTF8_COUNT_TRAIL_BYTES
 * may exist in old client code that must continue to run with newer icu library versions.
 *
 * This table could be replaced on many machines by
 * a few lines of assembler code using an
 * "index of first 0-bit from msb" instruction and
 * one or two more integer instructions.
 *
 * For example, on an i386, do something like
 * - MOV AL, leadByte
 * - NOT AL         (8-bit, leave b15..b8==0..0, reverse only b7..b0)
 * - MOV AH, 0
 * - BSR BX, AX     (16-bit)
 * - MOV AX, 6      (result)
 * - JZ finish      (ZF==1 if leadByte==0xff)
 * - SUB AX, BX (result)
 * -finish:
 * (BSR: Bit Scan Reverse, scans for a 1-bit, starting from the MSB)
 */
U_CAPI ಸ್ಥಿರಾಂಕ uint8_t
utf8_ಎಣಿಕೆ_Trail_ಬೈಟುಗಳು[256]={
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    // illegal C0 & C1
    // 2-byte lead bytes C2..DF
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

    // 3-byte lead bytes E0..EF
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    // 4-byte lead bytes F0..F4
    // illegal F5..FF
    3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UChar32
utf8_error_ಮೌಲ್ಯ[6]={
    // Same values as UTF8_ERROR_VALUE_1, UTF8_ERROR_VALUE_2, UTF_ERROR_VALUE,
    // but without relying on the obsolete unicode/utf_old.h.
    0x15, 0x9f, 0xffff,
    0x10ffff
};

ಸ್ಥಿರ UChar32
error_ಮೌಲ್ಯ(int32_t count, int8_t strict) {
    ಆದರೆ(strict>=0) {
        ಹಿಂತಿರುಗಿ utf8_error_ಮೌಲ್ಯ[count];
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strict==-3) {
        ಹಿಂತಿರುಗಿ 0xfffd;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
}

/*
 * Handle the non-inline part of the U8_NEXT() and U8_NEXT_FFFD() macros
 * and their obsolete sibling UTF8_NEXT_CHAR_SAFE().
 *
 * U8_NEXT() supports NUL-terminated strings indicated via length<0.
 *
 * The "strict" parameter controls the error behavior:
 * <0  "Safe" behavior of U8_NEXT():
 *     -1: All illegal byte sequences yield U_SENTINEL=-1.
 *     -2: Same as -1, except for lenient treatment of surrogate code points as legal.
 *         Some implementations use this for roundtripping of
 *         Unicode 16-bit strings that are not well-formed UTF-16, that is, they
 *         contain unpaired surrogates.
 *     -3: All illegal byte sequences yield U+FFFD.
 *  0  Obsolete "safe" behavior of UTF8_NEXT_CHAR_SAFE(..., false):
 *     All illegal byte sequences yield a positive code point such that this
 *     result code point would be encoded with the same number of bytes as
 *     the illegal sequence.
 * >0  Obsolete "strict" behavior of UTF8_NEXT_CHAR_SAFE(..., true):
 *     Same as the obsolete "safe" behavior, but non-characters are also treated
 *     like illegal sequences.
 */
U_CAPI UChar32 U_EXPORT2
utf8_ಮುಂದಿನ_Char_ಸುರಕ್ಷಿತ_Body(ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t *pi, int32_t length, UChar32 c, int8_t strict) {
    // *pi is one after byte c.
    int32_t i=*pi;
    // length can be negative for NUL-terminated strings: Read and validate one byte at a time.
    ಆದರೆ(i==length || c>0xf4) {
        // end of string, or not a lead byte
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c>=0xf0) {
        // Test for 4-byte sequences first because
        // U8_NEXT() handles shorter valid sequences inline.
        uint8_t t1=s[i], t2, t3;
        c&=7;
        ಆದರೆ(U8_IS_VALID_LEAD4_AND_T1(c, t1) &&
                ++i!=length && (t2=s[i]-0x80)<=0x3f &&
                ++i!=length && (t3=s[i]-0x80)<=0x3f) {
            ++i;
            c=(c<<18)|((t1&0x3f)<<12)|(t2<<6)|t3;
            // strict: forbid non-characters like U+fffe
            ಆದರೆ(strict<=0 || !U_IS_UNICODE_NONCHAR(c)) {
                *pi=i;
                ಹಿಂತಿರುಗಿ c;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c>=0xe0) {
        c&=0xf;
        ಆದರೆ(strict!=-2) {
            uint8_t t1=s[i], t2;
            ಆದರೆ(U8_IS_VALID_LEAD3_AND_T1(c, t1) &&
                    ++i!=length && (t2=s[i]-0x80)<=0x3f) {
                ++i;
                c=(c<<12)|((t1&0x3f)<<6)|t2;
                // strict: forbid non-characters like U+fffe
                ಆದರೆ(strict<=0 || !U_IS_UNICODE_NONCHAR(c)) {
                    *pi=i;
                    ಹಿಂತಿರುಗಿ c;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            // strict=-2 -> lenient: allow surrogates
            uint8_t t1=s[i]-0x80, t2;
            ಆದರೆ(t1<=0x3f && (c>0 || t1>=0x20) &&
                    ++i!=length && (t2=s[i]-0x80)<=0x3f) {
                *pi=i+1;
                ಹಿಂತಿರುಗಿ (c<<12)|(t1<<6)|t2;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c>=0xc2) {
        uint8_t t1=s[i]-0x80;
        ಆದರೆ(t1<=0x3f) {
            *pi=i+1;
            ಹಿಂತಿರುಗಿ ((c-0xc0)<<6)|t1;
        }
    }  // else 0x80<=c<0xc2 is not a lead byte

    /* error handling */
    c=error_ಮೌಲ್ಯ(i-*pi, strict);
    *pi=i;
    ಹಿಂತಿರುಗಿ c;
}

U_CAPI int32_t U_EXPORT2
utf8_append_Char_ಸುರಕ್ಷಿತ_Body(uint8_t *s, int32_t i, int32_t length, UChar32 c, UBool *pIsError) {
    ಆದರೆ((uint32_t)(c)<=0x7ff) {
        ಆದರೆ((i)+1<(length)) {
            (s)[(i)++]=(uint8_t)(((c)>>6)|0xc0);
            (s)[(i)++]=(uint8_t)(((c)&0x3f)|0x80);
            ಹಿಂತಿರುಗಿ i;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((uint32_t)(c)<=0xffff) {
        /* Starting with Unicode 3.2, surrogate code points must not be encoded in UTF-8. */
        ಆದರೆ((i)+2<(length) && !U_IS_SURROGATE(c)) {
            (s)[(i)++]=(uint8_t)(((c)>>12)|0xe0);
            (s)[(i)++]=(uint8_t)((((c)>>6)&0x3f)|0x80);
            (s)[(i)++]=(uint8_t)(((c)&0x3f)|0x80);
            ಹಿಂತಿರುಗಿ i;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((uint32_t)(c)<=0x10ffff) {
        ಆದರೆ((i)+3<(length)) {
            (s)[(i)++]=(uint8_t)(((c)>>18)|0xf0);
            (s)[(i)++]=(uint8_t)((((c)>>12)&0x3f)|0x80);
            (s)[(i)++]=(uint8_t)((((c)>>6)&0x3f)|0x80);
            (s)[(i)++]=(uint8_t)(((c)&0x3f)|0x80);
            ಹಿಂತಿರುಗಿ i;
        }
    }
    /* c>0x10ffff or not enough space, write an error value */
    ಆದರೆ(pIsError!=ಶೂನ್ಯಸೂಚಕ) {
        *pIsError=ನಿಜ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        length-=i;
        ಆದರೆ(length>0) {
            int32_t ಸ್ಥಾನಾಂತರ;
            ಆದರೆ(length>3) {
                length=3;
            }
            s+=i;
            ಸ್ಥಾನಾಂತರ=0;
            c=utf8_error_ಮೌಲ್ಯ[length-1];
            U8_APPEND_UNSAFE(s, ಸ್ಥಾನಾಂತರ, c);
            i=i+ಸ್ಥಾನಾಂತರ;
        }
    }
    ಹಿಂತಿರುಗಿ i;
}

U_CAPI UChar32 U_EXPORT2
utf8_prev_Char_ಸುರಕ್ಷಿತ_Body(ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t ಆರಂಭ, int32_t *pi, UChar32 c, int8_t strict) {
    // *pi is the index of byte c.
    int32_t i=*pi;
    ಆದರೆ(U8_IS_TRAIL(c) && i>ಆರಂಭ) {
        uint8_t b1=s[--i];
        ಆದರೆ(U8_IS_LEAD(b1)) {
            ಆದರೆ(b1<0xe0) {
                *pi=i;
                ಹಿಂತಿರುಗಿ ((b1-0xc0)<<6)|(c&0x3f);
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b1<0xf0 ? U8_IS_VALID_LEAD3_AND_T1(b1, c) : U8_IS_VALID_LEAD4_AND_T1(b1, c)) {
                // Truncated 3- or 4-byte sequence.
                *pi=i;
                ಹಿಂತಿರುಗಿ error_ಮೌಲ್ಯ(1, strict);
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U8_IS_TRAIL(b1) && i>ಆರಂಭ) {
            // Extract the value bits from the last trail byte.
            c&=0x3f;
            uint8_t b2=s[--i];
            ಆದರೆ(0xe0<=b2 && b2<=0xf4) {
                ಆದರೆ(b2<0xf0) {
                    b2&=0xf;
                    ಆದರೆ(strict!=-2) {
                        ಆದರೆ(U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                            *pi=i;
                            c=(b2<<12)|((b1&0x3f)<<6)|c;
                            ಆದರೆ(strict<=0 || !U_IS_UNICODE_NONCHAR(c)) {
                                ಹಿಂತಿರುಗಿ c;
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                // strict: forbid non-characters like U+fffe
                                ಹಿಂತಿರುಗಿ error_ಮೌಲ್ಯ(2, strict);
                            }
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        // strict=-2 -> lenient: allow surrogates
                        b1-=0x80;
                        ಆದರೆ((b2>0 || b1>=0x20)) {
                            *pi=i;
                            ಹಿಂತಿರುಗಿ (b2<<12)|(b1<<6)|c;
                        }
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U8_IS_VALID_LEAD4_AND_T1(b2, b1)) {
                    // Truncated 4-byte sequence.
                    *pi=i;
                    ಹಿಂತಿರುಗಿ error_ಮೌಲ್ಯ(2, strict);
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U8_IS_TRAIL(b2) && i>ಆರಂಭ) {
                uint8_t b3=s[--i];
                ಆದರೆ(0xf0<=b3 && b3<=0xf4) {
                    b3&=7;
                    ಆದರೆ(U8_IS_VALID_LEAD4_AND_T1(b3, b2)) {
                        *pi=i;
                        c=(b3<<18)|((b2&0x3f)<<12)|((b1&0x3f)<<6)|c;
                        ಆದರೆ(strict<=0 || !U_IS_UNICODE_NONCHAR(c)) {
                            ಹಿಂತಿರುಗಿ c;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            // strict: forbid non-characters like U+fffe
                            ಹಿಂತಿರುಗಿ error_ಮೌಲ್ಯ(3, strict);
                        }
                    }
                }
            }
        }
    }
    ಹಿಂತಿರುಗಿ error_ಮೌಲ್ಯ(0, strict);
}

U_CAPI int32_t U_EXPORT2
utf8_back1_ಸುರಕ್ಷಿತ_Body(ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t ಆರಂಭ, int32_t i) {
    // Same as utf8_prevCharSafeBody(..., strict=-1) minus assembling code points.
    int32_t orig_i=i;
    uint8_t c=s[i];
    ಆದರೆ(U8_IS_TRAIL(c) && i>ಆರಂಭ) {
        uint8_t b1=s[--i];
        ಆದರೆ(U8_IS_LEAD(b1)) {
            ಆದರೆ(b1<0xe0 ||
                    (b1<0xf0 ? U8_IS_VALID_LEAD3_AND_T1(b1, c) : U8_IS_VALID_LEAD4_AND_T1(b1, c))) {
                ಹಿಂತಿರುಗಿ i;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U8_IS_TRAIL(b1) && i>ಆರಂಭ) {
            uint8_t b2=s[--i];
            ಆದರೆ(0xe0<=b2 && b2<=0xf4) {
                ಆದರೆ(b2<0xf0 ? U8_IS_VALID_LEAD3_AND_T1(b2, b1) : U8_IS_VALID_LEAD4_AND_T1(b2, b1)) {
                    ಹಿಂತಿರುಗಿ i;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U8_IS_TRAIL(b2) && i>ಆರಂಭ) {
                uint8_t b3=s[--i];
                ಆದರೆ(0xf0<=b3 && b3<=0xf4 && U8_IS_VALID_LEAD4_AND_T1(b3, b2)) {
                    ಹಿಂತಿರುಗಿ i;
                }
            }
        }
    }
    ಹಿಂತಿರುಗಿ orig_i;
}
