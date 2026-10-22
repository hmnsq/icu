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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_IDNA

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "punycode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"


/* Punycode ----------------------------------------------------------------- */

/* Punycode parameters for Bootstring */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅਧਾਰ_2            36
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMIN            1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMAX            26
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SKEW            38
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DAMP            700
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INITIAL_BIAS    72
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INITIAL_N       0x80

/* "Basic" Unicode/ASCII code points */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _HYPHEN         0X2d
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DELIMITER       _HYPHEN

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _ZERO_          0X30
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _NINE           0x39

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _SMALL_A        0X61
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _SMALL_Z        0X7a

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _CAPITAL_A      0X41
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _CAPITAL_Z      0X5a

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_BASIC(c) ((c)<0x80)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_BASIC_UPPERCASE(c) (_CAPITAL_A<=(c) && (c)<=_CAPITAL_Z)

/**
 * digitToBasic() returns the basic code point whose value
 * (when used for representing integers) is d, which must be in the
 * range 0 to BASE-1. The lowercase form is used unless the uppercase flag is
 * nonzero, in which case the uppercase form is used.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ
digit_ਮੰਜ਼ਿਲ_Basic(int32_t digit, UBool uppercase) {
    /*  0..25 map to ASCII a..z or A..Z */
    /* 26..35 map to ASCII 0..9         */
    ਜੇ(digit<26) {
        ਜੇ(uppercase) {
            ਵਾਪਸ static_cast<ਅੱਖਰ>(_CAPITAL_A + digit);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ static_cast<ਅੱਖਰ>(_SMALL_A + digit);
        }
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ static_cast<ਅੱਖਰ>((_ZERO_ - 26) + digit);
    }
}

/**
 * @return the numeric value of a basic code point (for use in representing integers)
 *         in the range 0 to BASE-1, or a negative value if cp is invalid.
 */
ਸਥਿਰ int32_t decodeDigit(int32_t cp) {
    ਜੇ(cp<=u'Z') {
        ਜੇ(cp<=u'9') {
            ਜੇ(cp<u'0') {
                ਵਾਪਸ -1;
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ cp-u'0'+26;  // 0..9 -> 26..35
            }
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ cp-u'A';  // A-Z -> 0..25
        }
    } ਨਹੀਂਤਾਂ ਜੇ(cp<=u'z') {
        ਵਾਪਸ cp-'a';  // a..z -> 0..25
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ -1;
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ
asciiCaseMap(ਅੱਖਰ b, UBool uppercase) {
    ਜੇ(uppercase) {
        ਜੇ(_SMALL_A<=b && b<=_SMALL_Z) {
            b-=(_SMALL_A-_CAPITAL_A);
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ(_CAPITAL_A<=b && b<=_CAPITAL_Z) {
            b+=(_SMALL_A-_CAPITAL_A);
        }
    }
    ਵਾਪਸ b;
}

/* Punycode-specific Bootstring code ---------------------------------------- */

/*
 * The following code omits the {parts} of the pseudo-algorithm in the spec
 * that are not used with the Punycode parameter set.
 */

/* Bias adaptation function. */
ਸਥਿਰ int32_t
adaptBias(int32_t delta, int32_t length, UBool ਪਹਿਲਾ_ਸਮਾਂ) {
    int32_t count;

    ਜੇ(ਪਹਿਲਾ_ਸਮਾਂ) {
        delta/=DAMP;
    } ਨਹੀਂਤਾਂ {
        delta/=2;
    }

    delta+=delta/length;
    ਲਈ(count=0; delta>((ਅਧਾਰ_2-TMIN)*TMAX)/2; count+=ਅਧਾਰ_2) {
        delta/=(ਅਧਾਰ_2-TMIN);
    }

    ਵਾਪਸ count+(((ਅਧਾਰ_2-TMIN+1)*delta)/(delta+SKEW));
}

ਨਾਮਾਕਾਸ਼ {

// ICU-13727: Limit input length for n^2 algorithm
// where well-formed strings are at most 59 characters long.
constexpr int32_t ENCODE_MAX_ਕੋਡ_UNITS=1000;
constexpr int32_t DECODE_MAX_CHARS=2000;

}  // namespace

// encode
U_CAPI int32_t
u_str_ਮੰਜ਼ਿਲ_Punycode(ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                ਅਚਲ UBool *caseFlags,
                UError_ਕੋਡ *p_Error_ਕੋਡ) {

    int32_t cp_ਬਫਰ[ENCODE_MAX_ਕੋਡ_UNITS];
    int32_t n, delta, handledCPCount, basic_ਲੰਮਾਈ, dest_ਲੰਮਾਈ, bias, j, m, q, k, t, srcCPCount;
    char16_t c, c2;

    /* argument checking */
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    ਜੇ(src==ਖਾਲੀ_ਸੂਚਕ || src_ਲੰਮਾਈ<-1 || dest_ਸਮਰੱਥਾ<0 || (dest==ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ!=0)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    ਜੇ (src_ਲੰਮਾਈ>ENCODE_MAX_ਕੋਡ_UNITS) {
        *p_Error_ਕੋਡ=U_ਨਿਵੇਸ਼_TOO_LONG_ERROR;
        ਵਾਪਸ 0;
    }

    /*
     * Handle the basic code points and
     * convert extended ones to UTF-32 in cpBuffer (caseFlag in sign bit):
     */
    srcCPCount=dest_ਲੰਮਾਈ=0;
    ਜੇ(src_ਲੰਮਾਈ==-1) {
        /* NUL-terminated input */
        ਲਈ(j=0; /* no condition */; ++j) {
            ਜੇ((c=src[j])==0) {
                ਰੋਕੋ;
            }
            ਜੇ(j>=ENCODE_MAX_ਕੋਡ_UNITS) {
                *p_Error_ਕੋਡ=U_ਨਿਵੇਸ਼_TOO_LONG_ERROR;
                ਵਾਪਸ 0;
            }
            ਜੇ(IS_BASIC(c)) {
                cp_ਬਫਰ[srcCPCount++]=0;
                ਜੇ(dest_ਲੰਮਾਈ<dest_ਸਮਰੱਥਾ) {
                    dest[dest_ਲੰਮਾਈ]=
                        caseFlags!=ਖਾਲੀ_ਸੂਚਕ ?
                            asciiCaseMap((ਅੱਖਰ)c, caseFlags[j]) :
                            (ਅੱਖਰ)c;
                }
                ++dest_ਲੰਮਾਈ;
            } ਨਹੀਂਤਾਂ {
                n=(caseFlags!=ਖਾਲੀ_ਸੂਚਕ && caseFlags[j])<<31L;
                ਜੇ(U16_IS_SINGLE(c)) {
                    n|=c;
                } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_LEAD(c) && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } ਨਹੀਂਤਾਂ {
                    /* error: unmatched surrogate */
                    *p_Error_ਕੋਡ=U_INVALID_CHAR_ਮਿਲਿਆ;
                    ਵਾਪਸ 0;
                }
                cp_ਬਫਰ[srcCPCount++]=n;
            }
        }
    } ਨਹੀਂਤਾਂ {
        /* length-specified input */
        ਲਈ(j=0; j<src_ਲੰਮਾਈ; ++j) {
            c=src[j];
            ਜੇ(IS_BASIC(c)) {
                cp_ਬਫਰ[srcCPCount++]=0;
                ਜੇ(dest_ਲੰਮਾਈ<dest_ਸਮਰੱਥਾ) {
                    dest[dest_ਲੰਮਾਈ]=
                        caseFlags!=ਖਾਲੀ_ਸੂਚਕ ?
                            asciiCaseMap((ਅੱਖਰ)c, caseFlags[j]) :
                            (ਅੱਖਰ)c;
                }
                ++dest_ਲੰਮਾਈ;
            } ਨਹੀਂਤਾਂ {
                n=(caseFlags!=ਖਾਲੀ_ਸੂਚਕ && caseFlags[j])<<31L;
                ਜੇ(U16_IS_SINGLE(c)) {
                    n|=c;
                } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_LEAD(c) && (j+1)<src_ਲੰਮਾਈ && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } ਨਹੀਂਤਾਂ {
                    /* error: unmatched surrogate */
                    *p_Error_ਕੋਡ=U_INVALID_CHAR_ਮਿਲਿਆ;
                    ਵਾਪਸ 0;
                }
                cp_ਬਫਰ[srcCPCount++]=n;
            }
        }
    }

    /* Finish the basic string - if it is not empty - with a delimiter. */
    basic_ਲੰਮਾਈ=dest_ਲੰਮਾਈ;
    ਜੇ(basic_ਲੰਮਾਈ>0) {
        ਜੇ(dest_ਲੰਮਾਈ<dest_ਸਮਰੱਥਾ) {
            dest[dest_ਲੰਮਾਈ]=DELIMITER;
        }
        ++dest_ਲੰਮਾਈ;
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
    ਲਈ(handledCPCount=basic_ਲੰਮਾਈ; handledCPCount<srcCPCount; /* no op */) {
        /*
         * All non-basic code points < n have been handled already.
         * Find the next larger one:
         */
        ਲਈ(m=0x7fffffff, j=0; j<srcCPCount; ++j) {
            q=cp_ਬਫਰ[j]&0x7fffffff; /* remove case flag from the sign bit */
            ਜੇ(n<=q && q<m) {
                m=q;
            }
        }

        /*
         * Increase delta enough to advance the decoder's
         * <n,i> state to <m,0>, but guard against overflow:
         */
        ਜੇ(m-n>(0x7fffffff-handledCPCount-delta)/(handledCPCount+1)) {
            *p_Error_ਕੋਡ=U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
            ਵਾਪਸ 0;
        }
        delta+=(m-n)*(handledCPCount+1);
        n=m;

        /* Encode a sequence of same code points n */
        ਲਈ(j=0; j<srcCPCount; ++j) {
            q=cp_ਬਫਰ[j]&0x7fffffff; /* remove case flag from the sign bit */
            ਜੇ(q<n) {
                ++delta;
            } ਨਹੀਂਤਾਂ ਜੇ(q==n) {
                /* Represent delta as a generalized variable-length integer: */
                ਲਈ(q=delta, k=ਅਧਾਰ_2; /* no condition */; k+=ਅਧਾਰ_2) {

                    /** RAM: comment out the old code for conformance with draft-ietf-idn-punycode-03.txt

                    t=k-bias;
                    if(t<TMIN) {
                        t=TMIN;
                    } else if(t>TMAX) {
                        t=TMAX;
                    }
                    */

                    t=k-bias;
                    ਜੇ(t<TMIN) {
                        t=TMIN;
                    } ਨਹੀਂਤਾਂ ਜੇ(k>=(bias+TMAX)) {
                        t=TMAX;
                    }

                    ਜੇ(q<t) {
                        ਰੋਕੋ;
                    }

                    ਜੇ(dest_ਲੰਮਾਈ<dest_ਸਮਰੱਥਾ) {
                        dest[dest_ਲੰਮਾਈ]=digit_ਮੰਜ਼ਿਲ_Basic(t+(q-t)%(ਅਧਾਰ_2-t), 0);
                    }
                    ++dest_ਲੰਮਾਈ;
                    q=(q-t)/(ਅਧਾਰ_2-t);
                }

                ਜੇ(dest_ਲੰਮਾਈ<dest_ਸਮਰੱਥਾ) {
                    dest[dest_ਲੰਮਾਈ] = digit_ਮੰਜ਼ਿਲ_Basic(q, cp_ਬਫਰ[j] < 0);
                }
                ++dest_ਲੰਮਾਈ;
                bias = adaptBias(delta, handledCPCount + 1, handledCPCount == basic_ਲੰਮਾਈ);
                delta=0;
                ++handledCPCount;
            }
        }

        ++delta;
        ++n;
    }

    ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, dest_ਲੰਮਾਈ, p_Error_ਕੋਡ);
}

// decode
U_CAPI int32_t
u_strFromPunycode(ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                  char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                  UBool *caseFlags,
                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    int32_t n, dest_ਲੰਮਾਈ, i, bias, basic_ਲੰਮਾਈ, j, in, oldi, w, k, digit, t,
            destCPCount, ਪਹਿਲਾ_Supplementary_ਸੂਚਕਾਂਕ, cp_ਲੰਮਾਈ;
    char16_t b;

    /* argument checking */
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    ਜੇ(src==ਖਾਲੀ_ਸੂਚਕ || src_ਲੰਮਾਈ<-1 || (dest==ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ!=0)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ(src_ਲੰਮਾਈ==-1) {
        src_ਲੰਮਾਈ=u_strlen(src);
    }
    ਜੇ (src_ਲੰਮਾਈ>DECODE_MAX_CHARS) {
        *p_Error_ਕੋਡ=U_ਨਿਵੇਸ਼_TOO_LONG_ERROR;
        ਵਾਪਸ 0;
    }

    /*
     * Handle the basic code points:
     * Let basicLength be the number of input code points
     * before the last delimiter, or 0 if there is none,
     * then copy the first basicLength code points to the output.
     *
     * The two following loops iterate backward.
     */
    ਲਈ(j=src_ਲੰਮਾਈ; j>0;) {
        ਜੇ(src[--j]==DELIMITER) {
            ਰੋਕੋ;
        }
    }
    dest_ਲੰਮਾਈ=basic_ਲੰਮਾਈ=destCPCount=j;
    U_ASSERT(dest_ਲੰਮਾਈ>=0);

    ਜਦੋਂ(j>0) {
        b=src[--j];
        ਜੇ(!IS_BASIC(b)) {
            *p_Error_ਕੋਡ=U_INVALID_CHAR_ਮਿਲਿਆ;
            ਵਾਪਸ 0;
        }

        ਜੇ(j<dest_ਸਮਰੱਥਾ) {
            dest[j] = b;

            ਜੇ(caseFlags!=ਖਾਲੀ_ਸੂਚਕ) {
                caseFlags[j]=IS_BASIC_UPPERCASE(b);
            }
        }
    }

    /* Initialize the state: */
    n=INITIAL_N;
    i=0;
    bias=INITIAL_BIAS;
    ਪਹਿਲਾ_Supplementary_ਸੂਚਕਾਂਕ=1000000000;

    /*
     * Main decoding loop:
     * Start just after the last delimiter if any
     * basic code points were copied; start at the beginning otherwise.
     */
    ਲਈ(in=basic_ਲੰਮਾਈ>0 ? basic_ਲੰਮਾਈ+1 : 0; in<src_ਲੰਮਾਈ; /* no op */) {
        /*
         * in is the index of the next character to be consumed, and
         * destCPCount is the number of code points in the output array.
         *
         * Decode a generalized variable-length integer into delta,
         * which gets added to i.  The overflow checking is easier
         * if we increase i as we go, then subtract off its starting
         * value at the end to obtain delta.
         */
        ਲਈ(oldi=i, w=1, k=ਅਧਾਰ_2; /* no condition */; k+=ਅਧਾਰ_2) {
            ਜੇ(in>=src_ਲੰਮਾਈ) {
                *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਵਾਪਸ 0;
            }

            digit=decodeDigit(src[in++]);
            ਜੇ(digit<0) {
                *p_Error_ਕੋਡ=U_INVALID_CHAR_ਮਿਲਿਆ;
                ਵਾਪਸ 0;
            }
            ਜੇ(digit>(0x7fffffff-i)/w) {
                /* integer overflow */
                *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਵਾਪਸ 0;
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
            ਜੇ(t<TMIN) {
                t=TMIN;
            } ਨਹੀਂਤਾਂ ਜੇ(k>=(bias+TMAX)) {
                t=TMAX;
            }
            ਜੇ(digit<t) {
                ਰੋਕੋ;
            }

            ਜੇ(w>0x7fffffff/(ਅਧਾਰ_2-t)) {
                /* integer overflow */
                *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਵਾਪਸ 0;
            }
            w*=ਅਧਾਰ_2-t;
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
        ਜੇ(i/destCPCount>(0x7fffffff-n)) {
            /* integer overflow */
            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
            ਵਾਪਸ 0;
        }

        n+=i/destCPCount;
        i%=destCPCount;
        /* not needed for Punycode: */
        /* if (decode_digit(n) <= BASE) return punycode_invalid_input; */

        ਜੇ(n>0x10ffff || U_IS_SURROGATE(n)) {
            /* Unicode code point overflow */
            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
            ਵਾਪਸ 0;
        }

        /* Insert n at position i of the output: */
        cp_ਲੰਮਾਈ=U16_ਲੰਮਾਈ(n);
        ਜੇ(dest!=ਖਾਲੀ_ਸੂਚਕ && ((dest_ਲੰਮਾਈ+cp_ਲੰਮਾਈ)<=dest_ਸਮਰੱਥਾ)) {
            int32_t ਕੋਡ_Unit_ਸੂਚਕਾਂਕ;

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
            ਜੇ(i<=ਪਹਿਲਾ_Supplementary_ਸੂਚਕਾਂਕ) {
                ਕੋਡ_Unit_ਸੂਚਕਾਂਕ=i;
                ਜੇ(cp_ਲੰਮਾਈ>1) {
                    ਪਹਿਲਾ_Supplementary_ਸੂਚਕਾਂਕ=ਕੋਡ_Unit_ਸੂਚਕਾਂਕ;
                } ਨਹੀਂਤਾਂ {
                    ++ਪਹਿਲਾ_Supplementary_ਸੂਚਕਾਂਕ;
                }
            } ਨਹੀਂਤਾਂ {
                ਕੋਡ_Unit_ਸੂਚਕਾਂਕ=ਪਹਿਲਾ_Supplementary_ਸੂਚਕਾਂਕ;
                U16_FWD_N(dest, ਕੋਡ_Unit_ਸੂਚਕਾਂਕ, dest_ਲੰਮਾਈ, i-ਕੋਡ_Unit_ਸੂਚਕਾਂਕ);
            }

            /* use the char16_t index codeUnitIndex instead of the code point index i */
            ਜੇ(ਕੋਡ_Unit_ਸੂਚਕਾਂਕ<dest_ਲੰਮਾਈ) {
                uprv_memmove(dest+ਕੋਡ_Unit_ਸੂਚਕਾਂਕ+cp_ਲੰਮਾਈ,
                             dest+ਕੋਡ_Unit_ਸੂਚਕਾਂਕ,
                             (dest_ਲੰਮਾਈ-ਕੋਡ_Unit_ਸੂਚਕਾਂਕ)*U_SIZEOF_UCHAR);
                ਜੇ(caseFlags!=ਖਾਲੀ_ਸੂਚਕ) {
                    uprv_memmove(caseFlags+ਕੋਡ_Unit_ਸੂਚਕਾਂਕ+cp_ਲੰਮਾਈ,
                                 caseFlags+ਕੋਡ_Unit_ਸੂਚਕਾਂਕ,
                                 dest_ਲੰਮਾਈ-ਕੋਡ_Unit_ਸੂਚਕਾਂਕ);
                }
            }
            ਜੇ(cp_ਲੰਮਾਈ==1) {
                /* BMP, insert one code unit */
                dest[ਕੋਡ_Unit_ਸੂਚਕਾਂਕ]=(char16_t)n;
            } ਨਹੀਂਤਾਂ {
                /* supplementary character, insert two code units */
                dest[ਕੋਡ_Unit_ਸੂਚਕਾਂਕ]=U16_LEAD(n);
                dest[ਕੋਡ_Unit_ਸੂਚਕਾਂਕ+1]=U16_TRAIL(n);
            }
            ਜੇ(caseFlags!=ਖਾਲੀ_ਸੂਚਕ) {
                /* Case of last character determines uppercase flag: */
                caseFlags[ਕੋਡ_Unit_ਸੂਚਕਾਂਕ]=IS_BASIC_UPPERCASE(src[in-1]);
                ਜੇ(cp_ਲੰਮਾਈ==2) {
                    caseFlags[ਕੋਡ_Unit_ਸੂਚਕਾਂਕ+1]=ਝੂਠ;
                }
            }
        }
        dest_ਲੰਮਾਈ+=cp_ਲੰਮਾਈ;
        U_ASSERT(dest_ਲੰਮਾਈ>=0);
        ++i;
    }

    ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, dest_ਲੰਮਾਈ, p_Error_ਕੋਡ);
}

/* ### check notes on overflow handling - only necessary if not IDNA? are these Punycode functions to be public? */

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_IDNA */
