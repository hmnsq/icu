// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  punycode.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002jan31
*   created by: Markus W. Scherer
*/


/* This ICU code derived from: */
/*
punycode.c 0.4.0 (2001-Nov-17-Sat)
http://www.cs.berkeley.edu/~amc/idn/
Adam M. Costello
http://www.nicemice.net/amc/

Disclaimer and license

    Regarding this entire document or any portion of it (including
    the pseudocode and C code), the author makes no guarantees and
    is not responsible for any damage resulting from its use.  The
    author grants irrevocable permission to anyone to use, modify,
    and distribute it in any way that does not diminish the rights
    of anyone else to use, modify, and distribute it, provided that
    redistributed derivative works do not contain misleading author or
    version information.  Derivative works need not be licensed under
    similar terms.
*/
/*
 * ICU modifications:
 * - ICU data types and coding conventions
 * - ICU string buffer handling with implicit source lengths
 *   and destination preflighting
 * - UTF-16 handling
 */

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_IDNA

#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "punycode.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"


/* Punycode ----------------------------------------------------------------- */

/* Punycode parameters for Bootstring */
#ವ್ಯಾಖ್ಯಾನಿಸು ತಳ_2            36
#ವ್ಯಾಖ್ಯಾನಿಸು TMIN            1
#ವ್ಯಾಖ್ಯಾನಿಸು TMAX            26
#ವ್ಯಾಖ್ಯಾನಿಸು SKEW            38
#ವ್ಯಾಖ್ಯಾನಿಸು DAMP            700
#ವ್ಯಾಖ್ಯಾನಿಸು INITIAL_BIAS    72
#ವ್ಯಾಖ್ಯಾನಿಸು INITIAL_N       0x80

/* "Basic" Unicode/ASCII code points */
#ವ್ಯಾಖ್ಯಾನಿಸು _HYPHEN         0X2d
#ವ್ಯಾಖ್ಯಾನಿಸು DELIMITER       _HYPHEN

#ವ್ಯಾಖ್ಯಾನಿಸು _ZERO_          0X30
#ವ್ಯಾಖ್ಯಾನಿಸು _NINE           0x39

#ವ್ಯಾಖ್ಯಾನಿಸು _SMALL_A        0X61
#ವ್ಯಾಖ್ಯಾನಿಸು _SMALL_Z        0X7a

#ವ್ಯಾಖ್ಯಾನಿಸು _CAPITAL_A      0X41
#ವ್ಯಾಖ್ಯಾನಿಸು _CAPITAL_Z      0X5a

#ವ್ಯಾಖ್ಯಾನಿಸು IS_BASIC(c) ((c)<0x80)
#ವ್ಯಾಖ್ಯಾನಿಸು IS_BASIC_UPPERCASE(c) (_CAPITAL_A<=(c) && (c)<=_CAPITAL_Z)

/**
 * digitToBasic() returns the basic code point whose value
 * (when used for representing integers) is d, which must be in the
 * range 0 to BASE-1. The lowercase form is used unless the uppercase flag is
 * nonzero, in which case the uppercase form is used.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ
digit_ಗಮ್ಯ_Basic(int32_t digit, UBool uppercase) {
    /*  0..25 map to ASCII a..z or A..Z */
    /* 26..35 map to ASCII 0..9         */
    ಆದರೆ(digit<26) {
        ಆದರೆ(uppercase) {
            ಹಿಂತಿರುಗಿ static_cast<ಅಕ್ಷರ>(_CAPITAL_A + digit);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ static_cast<ಅಕ್ಷರ>(_SMALL_A + digit);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ static_cast<ಅಕ್ಷರ>((_ZERO_ - 26) + digit);
    }
}

/**
 * @return the numeric value of a basic code point (for use in representing integers)
 *         in the range 0 to BASE-1, or a negative value if cp is invalid.
 */
ಸ್ಥಿರ int32_t decodeDigit(int32_t cp) {
    ಆದರೆ(cp<=u'Z') {
        ಆದರೆ(cp<=u'9') {
            ಆದರೆ(cp<u'0') {
                ಹಿಂತಿರುಗಿ -1;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ cp-u'0'+26;  // 0..9 -> 26..35
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ cp-u'A';  // A-Z -> 0..25
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cp<=u'z') {
        ಹಿಂತಿರುಗಿ cp-'a';  // a..z -> 0..25
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ -1;
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ
asciiCaseMap(ಅಕ್ಷರ b, UBool uppercase) {
    ಆದರೆ(uppercase) {
        ಆದರೆ(_SMALL_A<=b && b<=_SMALL_Z) {
            b-=(_SMALL_A-_CAPITAL_A);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(_CAPITAL_A<=b && b<=_CAPITAL_Z) {
            b+=(_SMALL_A-_CAPITAL_A);
        }
    }
    ಹಿಂತಿರುಗಿ b;
}

/* Punycode-specific Bootstring code ---------------------------------------- */

/*
 * The following code omits the {parts} of the pseudo-algorithm in the spec
 * that are not used with the Punycode parameter set.
 */

/* Bias adaptation function. */
ಸ್ಥಿರ int32_t
adaptBias(int32_t delta, int32_t length, UBool ಮೊದಲ_ಸಮಯ) {
    int32_t count;

    ಆದರೆ(ಮೊದಲ_ಸಮಯ) {
        delta/=DAMP;
    } ಇಲ್ಲದಿದ್ದರೆ {
        delta/=2;
    }

    delta+=delta/length;
    ಗಾಗಿ(count=0; delta>((ತಳ_2-TMIN)*TMAX)/2; count+=ತಳ_2) {
        delta/=(ತಳ_2-TMIN);
    }

    ಹಿಂತಿರುಗಿ count+(((ತಳ_2-TMIN+1)*delta)/(delta+SKEW));
}

ನಾಮಾಕಾಶ {

// ICU-13727: Limit input length for n^2 algorithm
// where well-formed strings are at most 59 characters long.
constexpr int32_t ENCODE_MAX_ಸಂಕೇತ_UNITS=1000;
constexpr int32_t DECODE_MAX_CHARS=2000;

}  // namespace

// encode
U_CAPI int32_t
u_str_ಗಮ್ಯ_Punycode(ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                ಸ್ಥಿರಾಂಕ UBool *caseFlags,
                UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {

    int32_t cp_ಬಫರ್[ENCODE_MAX_ಸಂಕೇತ_UNITS];
    int32_t n, delta, handledCPCount, basic_ಉದ್ದಳತೆ, dest_ಉದ್ದಳತೆ, bias, j, m, q, k, t, srcCPCount;
    char16_t c, c2;

    /* argument checking */
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(src==ಶೂನ್ಯಸೂಚಕ || src_ಉದ್ದಳತೆ<-1 || dest_ಸಾಮರ್ಥ್ಯ<0 || (dest==ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ!=0)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (src_ಉದ್ದಳತೆ>ENCODE_MAX_ಸಂಕೇತ_UNITS) {
        *p_Error_ಸಂಕೇತ=U_ಒಳಹರಿವು_TOO_LONG_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /*
     * Handle the basic code points and
     * convert extended ones to UTF-32 in cpBuffer (caseFlag in sign bit):
     */
    srcCPCount=dest_ಉದ್ದಳತೆ=0;
    ಆದರೆ(src_ಉದ್ದಳತೆ==-1) {
        /* NUL-terminated input */
        ಗಾಗಿ(j=0; /* no condition */; ++j) {
            ಆದರೆ((c=src[j])==0) {
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ(j>=ENCODE_MAX_ಸಂಕೇತ_UNITS) {
                *p_Error_ಸಂಕೇತ=U_ಒಳಹರಿವು_TOO_LONG_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
            ಆದರೆ(IS_BASIC(c)) {
                cp_ಬಫರ್[srcCPCount++]=0;
                ಆದರೆ(dest_ಉದ್ದಳತೆ<dest_ಸಾಮರ್ಥ್ಯ) {
                    dest[dest_ಉದ್ದಳತೆ]=
                        caseFlags!=ಶೂನ್ಯಸೂಚಕ ?
                            asciiCaseMap((ಅಕ್ಷರ)c, caseFlags[j]) :
                            (ಅಕ್ಷರ)c;
                }
                ++dest_ಉದ್ದಳತೆ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                n=(caseFlags!=ಶೂನ್ಯಸೂಚಕ && caseFlags[j])<<31L;
                ಆದರೆ(U16_IS_SINGLE(c)) {
                    n|=c;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_LEAD(c) && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* error: unmatched surrogate */
                    *p_Error_ಸಂಕೇತ=U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                    ಹಿಂತಿರುಗಿ 0;
                }
                cp_ಬಫರ್[srcCPCount++]=n;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* length-specified input */
        ಗಾಗಿ(j=0; j<src_ಉದ್ದಳತೆ; ++j) {
            c=src[j];
            ಆದರೆ(IS_BASIC(c)) {
                cp_ಬಫರ್[srcCPCount++]=0;
                ಆದರೆ(dest_ಉದ್ದಳತೆ<dest_ಸಾಮರ್ಥ್ಯ) {
                    dest[dest_ಉದ್ದಳತೆ]=
                        caseFlags!=ಶೂನ್ಯಸೂಚಕ ?
                            asciiCaseMap((ಅಕ್ಷರ)c, caseFlags[j]) :
                            (ಅಕ್ಷರ)c;
                }
                ++dest_ಉದ್ದಳತೆ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                n=(caseFlags!=ಶೂನ್ಯಸೂಚಕ && caseFlags[j])<<31L;
                ಆದರೆ(U16_IS_SINGLE(c)) {
                    n|=c;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_LEAD(c) && (j+1)<src_ಉದ್ದಳತೆ && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* error: unmatched surrogate */
                    *p_Error_ಸಂಕೇತ=U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                    ಹಿಂತಿರುಗಿ 0;
                }
                cp_ಬಫರ್[srcCPCount++]=n;
            }
        }
    }

    /* Finish the basic string - if it is not empty - with a delimiter. */
    basic_ಉದ್ದಳತೆ=dest_ಉದ್ದಳತೆ;
    ಆದರೆ(basic_ಉದ್ದಳತೆ>0) {
        ಆದರೆ(dest_ಉದ್ದಳತೆ<dest_ಸಾಮರ್ಥ್ಯ) {
            dest[dest_ಉದ್ದಳತೆ]=DELIMITER;
        }
        ++dest_ಉದ್ದಳತೆ;
    }

    /*
     * handledCPCount is the number of code points that have been handled
     * basicLength is the number of basic code points
     * destLength is the number of chars that have been output
     */

    /* Initialize the state: */
    n=INITIAL_N;
    delta=0;
    bias=INITIAL_BIAS;

    /* Main encoding loop: */
    ಗಾಗಿ(handledCPCount=basic_ಉದ್ದಳತೆ; handledCPCount<srcCPCount; /* no op */) {
        /*
         * All non-basic code points < n have been handled already.
         * Find the next larger one:
         */
        ಗಾಗಿ(m=0x7fffffff, j=0; j<srcCPCount; ++j) {
            q=cp_ಬಫರ್[j]&0x7fffffff; /* remove case flag from the sign bit */
            ಆದರೆ(n<=q && q<m) {
                m=q;
            }
        }

        /*
         * Increase delta enough to advance the decoder's
         * <n,i> state to <m,0>, but guard against overflow:
         */
        ಆದರೆ(m-n>(0x7fffffff-handledCPCount-delta)/(handledCPCount+1)) {
            *p_Error_ಸಂಕೇತ=U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        delta+=(m-n)*(handledCPCount+1);
        n=m;

        /* Encode a sequence of same code points n */
        ಗಾಗಿ(j=0; j<srcCPCount; ++j) {
            q=cp_ಬಫರ್[j]&0x7fffffff; /* remove case flag from the sign bit */
            ಆದರೆ(q<n) {
                ++delta;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(q==n) {
                /* Represent delta as a generalized variable-length integer: */
                ಗಾಗಿ(q=delta, k=ತಳ_2; /* no condition */; k+=ತಳ_2) {

                    /** RAM: comment out the old code for conformance with draft-ietf-idn-punycode-03.txt

                    t=k-bias;
                    if(t<TMIN) {
                        t=TMIN;
                    } else if(t>TMAX) {
                        t=TMAX;
                    }
                    */

                    t=k-bias;
                    ಆದರೆ(t<TMIN) {
                        t=TMIN;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(k>=(bias+TMAX)) {
                        t=TMAX;
                    }

                    ಆದರೆ(q<t) {
                        ನಿಲ್ಲಿಸು;
                    }

                    ಆದರೆ(dest_ಉದ್ದಳತೆ<dest_ಸಾಮರ್ಥ್ಯ) {
                        dest[dest_ಉದ್ದಳತೆ]=digit_ಗಮ್ಯ_Basic(t+(q-t)%(ತಳ_2-t), 0);
                    }
                    ++dest_ಉದ್ದಳತೆ;
                    q=(q-t)/(ತಳ_2-t);
                }

                ಆದರೆ(dest_ಉದ್ದಳತೆ<dest_ಸಾಮರ್ಥ್ಯ) {
                    dest[dest_ಉದ್ದಳತೆ] = digit_ಗಮ್ಯ_Basic(q, cp_ಬಫರ್[j] < 0);
                }
                ++dest_ಉದ್ದಳತೆ;
                bias = adaptBias(delta, handledCPCount + 1, handledCPCount == basic_ಉದ್ದಳತೆ);
                delta=0;
                ++handledCPCount;
            }
        }

        ++delta;
        ++n;
    }

    ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, dest_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
}

// decode
U_CAPI int32_t
u_strFromPunycode(ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                  char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                  UBool *caseFlags,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    int32_t n, dest_ಉದ್ದಳತೆ, i, bias, basic_ಉದ್ದಳತೆ, j, in, oldi, w, k, digit, t,
            destCPCount, ಮೊದಲ_Supplementary_ಸೂಚ್ಯಂಕ, cp_ಉದ್ದಳತೆ;
    char16_t b;

    /* argument checking */
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(src==ಶೂನ್ಯಸೂಚಕ || src_ಉದ್ದಳತೆ<-1 || (dest==ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ!=0)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(src_ಉದ್ದಳತೆ==-1) {
        src_ಉದ್ದಳತೆ=u_strlen(src);
    }
    ಆದರೆ (src_ಉದ್ದಳತೆ>DECODE_MAX_CHARS) {
        *p_Error_ಸಂಕೇತ=U_ಒಳಹರಿವು_TOO_LONG_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /*
     * Handle the basic code points:
     * Let basicLength be the number of input code points
     * before the last delimiter, or 0 if there is none,
     * then copy the first basicLength code points to the output.
     *
     * The two following loops iterate backward.
     */
    ಗಾಗಿ(j=src_ಉದ್ದಳತೆ; j>0;) {
        ಆದರೆ(src[--j]==DELIMITER) {
            ನಿಲ್ಲಿಸು;
        }
    }
    dest_ಉದ್ದಳತೆ=basic_ಉದ್ದಳತೆ=destCPCount=j;
    U_ASSERT(dest_ಉದ್ದಳತೆ>=0);

    ಆಗ(j>0) {
        b=src[--j];
        ಆದರೆ(!IS_BASIC(b)) {
            *p_Error_ಸಂಕೇತ=U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
            ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ(j<dest_ಸಾಮರ್ಥ್ಯ) {
            dest[j] = b;

            ಆದರೆ(caseFlags!=ಶೂನ್ಯಸೂಚಕ) {
                caseFlags[j]=IS_BASIC_UPPERCASE(b);
            }
        }
    }

    /* Initialize the state: */
    n=INITIAL_N;
    i=0;
    bias=INITIAL_BIAS;
    ಮೊದಲ_Supplementary_ಸೂಚ್ಯಂಕ=1000000000;

    /*
     * Main decoding loop:
     * Start just after the last delimiter if any
     * basic code points were copied; start at the beginning otherwise.
     */
    ಗಾಗಿ(in=basic_ಉದ್ದಳತೆ>0 ? basic_ಉದ್ದಳತೆ+1 : 0; in<src_ಉದ್ದಳತೆ; /* no op */) {
        /*
         * in is the index of the next character to be consumed, and
         * destCPCount is the number of code points in the output array.
         *
         * Decode a generalized variable-length integer into delta,
         * which gets added to i.  The overflow checking is easier
         * if we increase i as we go, then subtract off its starting
         * value at the end to obtain delta.
         */
        ಗಾಗಿ(oldi=i, w=1, k=ತಳ_2; /* no condition */; k+=ತಳ_2) {
            ಆದರೆ(in>=src_ಉದ್ದಳತೆ) {
                *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ಹಿಂತಿರುಗಿ 0;
            }

            digit=decodeDigit(src[in++]);
            ಆದರೆ(digit<0) {
                *p_Error_ಸಂಕೇತ=U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                ಹಿಂತಿರುಗಿ 0;
            }
            ಆದರೆ(digit>(0x7fffffff-i)/w) {
                /* integer overflow */
                *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ಹಿಂತಿರುಗಿ 0;
            }

            i+=digit*w;
            /** RAM: comment out the old code for conformance with draft-ietf-idn-punycode-03.txt  
            t=k-bias;
            if(t<TMIN) {
                t=TMIN;
            } else if(t>TMAX) {
                t=TMAX;
            }
            */
            t=k-bias;
            ಆದರೆ(t<TMIN) {
                t=TMIN;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(k>=(bias+TMAX)) {
                t=TMAX;
            }
            ಆದರೆ(digit<t) {
                ನಿಲ್ಲಿಸು;
            }

            ಆದರೆ(w>0x7fffffff/(ತಳ_2-t)) {
                /* integer overflow */
                *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ಹಿಂತಿರುಗಿ 0;
            }
            w*=ತಳ_2-t;
        }

        /*
         * Modification from sample code:
         * Increments destCPCount here,
         * where needed instead of in for() loop tail.
         */
        ++destCPCount;
        bias = adaptBias(i - oldi, destCPCount, oldi == 0);

        /*
         * i was supposed to wrap around from (incremented) destCPCount to 0,
         * incrementing n each time, so we'll fix that now:
         */
        ಆದರೆ(i/destCPCount>(0x7fffffff-n)) {
            /* integer overflow */
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
            ಹಿಂತಿರುಗಿ 0;
        }

        n+=i/destCPCount;
        i%=destCPCount;
        /* not needed for Punycode: */
        /* if (decode_digit(n) <= BASE) return punycode_invalid_input; */

        ಆದರೆ(n>0x10ffff || U_IS_SURROGATE(n)) {
            /* Unicode code point overflow */
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
            ಹಿಂತಿರುಗಿ 0;
        }

        /* Insert n at position i of the output: */
        cp_ಉದ್ದಳತೆ=U16_ಉದ್ದಳತೆ(n);
        ಆದರೆ(dest!=ಶೂನ್ಯಸೂಚಕ && ((dest_ಉದ್ದಳತೆ+cp_ಉದ್ದಳತೆ)<=dest_ಸಾಮರ್ಥ್ಯ)) {
            int32_t ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ;

            /*
             * Handle indexes when supplementary code points are present.
             *
             * In almost all cases, there will be only BMP code points before i
             * and even in the entire string.
             * This is handled with the same efficiency as with UTF-32.
             *
             * Only the rare cases with supplementary code points are handled
             * more slowly - but not too bad since this is an insertion anyway.
             */
            ಆದರೆ(i<=ಮೊದಲ_Supplementary_ಸೂಚ್ಯಂಕ) {
                ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ=i;
                ಆದರೆ(cp_ಉದ್ದಳತೆ>1) {
                    ಮೊದಲ_Supplementary_ಸೂಚ್ಯಂಕ=ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ++ಮೊದಲ_Supplementary_ಸೂಚ್ಯಂಕ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ=ಮೊದಲ_Supplementary_ಸೂಚ್ಯಂಕ;
                U16_FWD_N(dest, ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ, dest_ಉದ್ದಳತೆ, i-ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ);
            }

            /* use the char16_t index codeUnitIndex instead of the code point index i */
            ಆದರೆ(ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ<dest_ಉದ್ದಳತೆ) {
                uprv_memmove(dest+ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ+cp_ಉದ್ದಳತೆ,
                             dest+ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ,
                             (dest_ಉದ್ದಳತೆ-ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ)*U_SIZEOF_UCHAR);
                ಆದರೆ(caseFlags!=ಶೂನ್ಯಸೂಚಕ) {
                    uprv_memmove(caseFlags+ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ+cp_ಉದ್ದಳತೆ,
                                 caseFlags+ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ,
                                 dest_ಉದ್ದಳತೆ-ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ);
                }
            }
            ಆದರೆ(cp_ಉದ್ದಳತೆ==1) {
                /* BMP, insert one code unit */
                dest[ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ]=(char16_t)n;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* supplementary character, insert two code units */
                dest[ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ]=U16_LEAD(n);
                dest[ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ+1]=U16_TRAIL(n);
            }
            ಆದರೆ(caseFlags!=ಶೂನ್ಯಸೂಚಕ) {
                /* Case of last character determines uppercase flag: */
                caseFlags[ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ]=IS_BASIC_UPPERCASE(src[in-1]);
                ಆದರೆ(cp_ಉದ್ದಳತೆ==2) {
                    caseFlags[ಸಂಕೇತ_Unit_ಸೂಚ್ಯಂಕ+1]=ಸುಳ್ಳು;
                }
            }
        }
        dest_ಉದ್ದಳತೆ+=cp_ಉದ್ದಳತೆ;
        U_ASSERT(dest_ಉದ್ದಳತೆ>=0);
        ++i;
    }

    ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, dest_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
}

/* ### check notes on overflow handling - only necessary if not IDNA? are these Punycode functions to be public? */

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_IDNA */
