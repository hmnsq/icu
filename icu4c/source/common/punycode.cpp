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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_IDNA

#ضمن "unicode/ustring.h"
#ضمن "unicode/utf.h"
#ضمن "unicode/utf16.h"
#ضمن "ustr_imp.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"
#ضمن "punycode.h"
#ضمن "uassert.h"


/* Punycode ----------------------------------------------------------------- */

/* Punycode parameters for Bootstring */
#تعریف_کرو ادھار_2            36
#تعریف_کرو TMIN            1
#تعریف_کرو TMAX            26
#تعریف_کرو SKEW            38
#تعریف_کرو DAMP            700
#تعریف_کرو INITIAL_BIAS    72
#تعریف_کرو INITIAL_N       0x80

/* "Basic" Unicode/ASCII code points */
#تعریف_کرو _HYPHEN         0X2d
#تعریف_کرو DELIMITER       _HYPHEN

#تعریف_کرو _ZERO_          0X30
#تعریف_کرو _NINE           0x39

#تعریف_کرو _SMALL_A        0X61
#تعریف_کرو _SMALL_Z        0X7a

#تعریف_کرو _CAPITAL_A      0X41
#تعریف_کرو _CAPITAL_Z      0X5a

#تعریف_کرو IS_BASIC(c) ((c)<0x80)
#تعریف_کرو IS_BASIC_UPPERCASE(c) (_CAPITAL_A<=(c) && (c)<=_CAPITAL_Z)

/**
 * digitToBasic() returns the basic code point whose value
 * (when used for representing integers) is d, which must be in the
 * range 0 to BASE-1. The lowercase form is used unless the uppercase flag is
 * nonzero, in which case the uppercase form is used.
 */
ساكن مضمن حرف
digit_منزل_Basic(int32_t digit, UBool uppercase) {
    /*  0..25 map to ASCII a..z or A..Z */
    /* 26..35 map to ASCII 0..9         */
    إذا(digit<26) {
        إذا(uppercase) {
            أرجع static_cast<حرف>(_CAPITAL_A + digit);
        } وإلا {
            أرجع static_cast<حرف>(_SMALL_A + digit);
        }
    } وإلا {
        أرجع static_cast<حرف>((_ZERO_ - 26) + digit);
    }
}

/**
 * @return the numeric value of a basic code point (for use in representing integers)
 *         in the range 0 to BASE-1, or a negative value if cp is invalid.
 */
ساكن int32_t decodeDigit(int32_t cp) {
    إذا(cp<=u'Z') {
        إذا(cp<=u'9') {
            إذا(cp<u'0') {
                أرجع -1;
            } وإلا {
                أرجع cp-u'0'+26;  // 0..9 -> 26..35
            }
        } وإلا {
            أرجع cp-u'A';  // A-Z -> 0..25
        }
    } وإلا إذا(cp<=u'z') {
        أرجع cp-'a';  // a..z -> 0..25
    } وإلا {
        أرجع -1;
    }
}

ساكن مضمن حرف
asciiCaseMap(حرف b, UBool uppercase) {
    إذا(uppercase) {
        إذا(_SMALL_A<=b && b<=_SMALL_Z) {
            b-=(_SMALL_A-_CAPITAL_A);
        }
    } وإلا {
        إذا(_CAPITAL_A<=b && b<=_CAPITAL_Z) {
            b+=(_SMALL_A-_CAPITAL_A);
        }
    }
    أرجع b;
}

/* Punycode-specific Bootstring code ---------------------------------------- */

/*
 * The following code omits the {parts} of the pseudo-algorithm in the spec
 * that are not used with the Punycode parameter set.
 */

/* Bias adaptation function. */
ساكن int32_t
adaptBias(int32_t delta, int32_t length, UBool پہلا_ویلا) {
    int32_t count;

    إذا(پہلا_ویلا) {
        delta/=DAMP;
    } وإلا {
        delta/=2;
    }

    delta+=delta/length;
    لكل(count=0; delta>((ادھار_2-TMIN)*TMAX)/2; count+=ادھار_2) {
        delta/=(ادھار_2-TMIN);
    }

    أرجع count+(((ادھار_2-TMIN+1)*delta)/(delta+SKEW));
}

نطاق {

// ICU-13727: Limit input length for n^2 algorithm
// where well-formed strings are at most 59 characters long.
constexpr int32_t ENCODE_MAX_کوڈ_UNITS=1000;
constexpr int32_t DECODE_MAX_CHARS=2000;

}  // namespace

// encode
U_CAPI int32_t
u_str_منزل_Punycode(ثابت char16_t *src, int32_t src_لمائی,
                char16_t *dest, int32_t dest_سمرتھا,
                ثابت UBool *caseFlags,
                UError_کوڈ *p_Error_کوڈ) {

    int32_t cp_بفر[ENCODE_MAX_کوڈ_UNITS];
    int32_t n, delta, handledCPCount, basic_لمائی, dest_لمائی, bias, j, m, q, k, t, srcCPCount;
    char16_t c, c2;

    /* argument checking */
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    إذا(src==خالی_سوچک || src_لمائی<-1 || dest_سمرتھا<0 || (dest==خالی_سوچک && dest_سمرتھا!=0)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    إذا (src_لمائی>ENCODE_MAX_کوڈ_UNITS) {
        *p_Error_کوڈ=U_داخلہ_TOO_LONG_ERROR;
        أرجع 0;
    }

    /*
     * Handle the basic code points and
     * convert extended ones to UTF-32 in cpBuffer (caseFlag in sign bit):
     */
    srcCPCount=dest_لمائی=0;
    إذا(src_لمائی==-1) {
        /* NUL-terminated input */
        لكل(j=0; /* no condition */; ++j) {
            إذا((c=src[j])==0) {
                اكسر;
            }
            إذا(j>=ENCODE_MAX_کوڈ_UNITS) {
                *p_Error_کوڈ=U_داخلہ_TOO_LONG_ERROR;
                أرجع 0;
            }
            إذا(IS_BASIC(c)) {
                cp_بفر[srcCPCount++]=0;
                إذا(dest_لمائی<dest_سمرتھا) {
                    dest[dest_لمائی]=
                        caseFlags!=خالی_سوچک ?
                            asciiCaseMap((حرف)c, caseFlags[j]) :
                            (حرف)c;
                }
                ++dest_لمائی;
            } وإلا {
                n=(caseFlags!=خالی_سوچک && caseFlags[j])<<31L;
                إذا(U16_IS_SINGLE(c)) {
                    n|=c;
                } وإلا إذا(U16_IS_LEAD(c) && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } وإلا {
                    /* error: unmatched surrogate */
                    *p_Error_کوڈ=U_INVALID_CHAR_لبھیا;
                    أرجع 0;
                }
                cp_بفر[srcCPCount++]=n;
            }
        }
    } وإلا {
        /* length-specified input */
        لكل(j=0; j<src_لمائی; ++j) {
            c=src[j];
            إذا(IS_BASIC(c)) {
                cp_بفر[srcCPCount++]=0;
                إذا(dest_لمائی<dest_سمرتھا) {
                    dest[dest_لمائی]=
                        caseFlags!=خالی_سوچک ?
                            asciiCaseMap((حرف)c, caseFlags[j]) :
                            (حرف)c;
                }
                ++dest_لمائی;
            } وإلا {
                n=(caseFlags!=خالی_سوچک && caseFlags[j])<<31L;
                إذا(U16_IS_SINGLE(c)) {
                    n|=c;
                } وإلا إذا(U16_IS_LEAD(c) && (j+1)<src_لمائی && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } وإلا {
                    /* error: unmatched surrogate */
                    *p_Error_کوڈ=U_INVALID_CHAR_لبھیا;
                    أرجع 0;
                }
                cp_بفر[srcCPCount++]=n;
            }
        }
    }

    /* Finish the basic string - if it is not empty - with a delimiter. */
    basic_لمائی=dest_لمائی;
    إذا(basic_لمائی>0) {
        إذا(dest_لمائی<dest_سمرتھا) {
            dest[dest_لمائی]=DELIMITER;
        }
        ++dest_لمائی;
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
    لكل(handledCPCount=basic_لمائی; handledCPCount<srcCPCount; /* no op */) {
        /*
         * All non-basic code points < n have been handled already.
         * Find the next larger one:
         */
        لكل(m=0x7fffffff, j=0; j<srcCPCount; ++j) {
            q=cp_بفر[j]&0x7fffffff; /* remove case flag from the sign bit */
            إذا(n<=q && q<m) {
                m=q;
            }
        }

        /*
         * Increase delta enough to advance the decoder's
         * <n,i> state to <m,0>, but guard against overflow:
         */
        إذا(m-n>(0x7fffffff-handledCPCount-delta)/(handledCPCount+1)) {
            *p_Error_کوڈ=U_اندرونی_پروگرام_ERROR;
            أرجع 0;
        }
        delta+=(m-n)*(handledCPCount+1);
        n=m;

        /* Encode a sequence of same code points n */
        لكل(j=0; j<srcCPCount; ++j) {
            q=cp_بفر[j]&0x7fffffff; /* remove case flag from the sign bit */
            إذا(q<n) {
                ++delta;
            } وإلا إذا(q==n) {
                /* Represent delta as a generalized variable-length integer: */
                لكل(q=delta, k=ادھار_2; /* no condition */; k+=ادھار_2) {

                    /** RAM: comment out the old code for conformance with draft-ietf-idn-punycode-03.txt

                    t=k-bias;
                    if(t<TMIN) {
                        t=TMIN;
                    } else if(t>TMAX) {
                        t=TMAX;
                    }
                    */

                    t=k-bias;
                    إذا(t<TMIN) {
                        t=TMIN;
                    } وإلا إذا(k>=(bias+TMAX)) {
                        t=TMAX;
                    }

                    إذا(q<t) {
                        اكسر;
                    }

                    إذا(dest_لمائی<dest_سمرتھا) {
                        dest[dest_لمائی]=digit_منزل_Basic(t+(q-t)%(ادھار_2-t), 0);
                    }
                    ++dest_لمائی;
                    q=(q-t)/(ادھار_2-t);
                }

                إذا(dest_لمائی<dest_سمرتھا) {
                    dest[dest_لمائی] = digit_منزل_Basic(q, cp_بفر[j] < 0);
                }
                ++dest_لمائی;
                bias = adaptBias(delta, handledCPCount + 1, handledCPCount == basic_لمائی);
                delta=0;
                ++handledCPCount;
            }
        }

        ++delta;
        ++n;
    }

    أرجع u_terminateUChars(dest, dest_سمرتھا, dest_لمائی, p_Error_کوڈ);
}

// decode
U_CAPI int32_t
u_strFromPunycode(ثابت char16_t *src, int32_t src_لمائی,
                  char16_t *dest, int32_t dest_سمرتھا,
                  UBool *caseFlags,
                  UError_کوڈ *p_Error_کوڈ) {
    int32_t n, dest_لمائی, i, bias, basic_لمائی, j, in, oldi, w, k, digit, t,
            destCPCount, پہلا_Supplementary_اشاریہ, cp_لمائی;
    char16_t b;

    /* argument checking */
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    إذا(src==خالی_سوچک || src_لمائی<-1 || (dest==خالی_سوچک && dest_سمرتھا!=0)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا(src_لمائی==-1) {
        src_لمائی=u_strlen(src);
    }
    إذا (src_لمائی>DECODE_MAX_CHARS) {
        *p_Error_کوڈ=U_داخلہ_TOO_LONG_ERROR;
        أرجع 0;
    }

    /*
     * Handle the basic code points:
     * Let basicLength be the number of input code points
     * before the last delimiter, or 0 if there is none,
     * then copy the first basicLength code points to the output.
     *
     * The two following loops iterate backward.
     */
    لكل(j=src_لمائی; j>0;) {
        إذا(src[--j]==DELIMITER) {
            اكسر;
        }
    }
    dest_لمائی=basic_لمائی=destCPCount=j;
    U_ASSERT(dest_لمائی>=0);

    طالما(j>0) {
        b=src[--j];
        إذا(!IS_BASIC(b)) {
            *p_Error_کوڈ=U_INVALID_CHAR_لبھیا;
            أرجع 0;
        }

        إذا(j<dest_سمرتھا) {
            dest[j] = b;

            إذا(caseFlags!=خالی_سوچک) {
                caseFlags[j]=IS_BASIC_UPPERCASE(b);
            }
        }
    }

    /* Initialize the state: */
    n=INITIAL_N;
    i=0;
    bias=INITIAL_BIAS;
    پہلا_Supplementary_اشاریہ=1000000000;

    /*
     * Main decoding loop:
     * Start just after the last delimiter if any
     * basic code points were copied; start at the beginning otherwise.
     */
    لكل(in=basic_لمائی>0 ? basic_لمائی+1 : 0; in<src_لمائی; /* no op */) {
        /*
         * in is the index of the next character to be consumed, and
         * destCPCount is the number of code points in the output array.
         *
         * Decode a generalized variable-length integer into delta,
         * which gets added to i.  The overflow checking is easier
         * if we increase i as we go, then subtract off its starting
         * value at the end to obtain delta.
         */
        لكل(oldi=i, w=1, k=ادھار_2; /* no condition */; k+=ادھار_2) {
            إذا(in>=src_لمائی) {
                *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
                أرجع 0;
            }

            digit=decodeDigit(src[in++]);
            إذا(digit<0) {
                *p_Error_کوڈ=U_INVALID_CHAR_لبھیا;
                أرجع 0;
            }
            إذا(digit>(0x7fffffff-i)/w) {
                /* integer overflow */
                *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
                أرجع 0;
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
            إذا(t<TMIN) {
                t=TMIN;
            } وإلا إذا(k>=(bias+TMAX)) {
                t=TMAX;
            }
            إذا(digit<t) {
                اكسر;
            }

            إذا(w>0x7fffffff/(ادھار_2-t)) {
                /* integer overflow */
                *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
                أرجع 0;
            }
            w*=ادھار_2-t;
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
        إذا(i/destCPCount>(0x7fffffff-n)) {
            /* integer overflow */
            *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
            أرجع 0;
        }

        n+=i/destCPCount;
        i%=destCPCount;
        /* not needed for Punycode: */
        /* if (decode_digit(n) <= BASE) return punycode_invalid_input; */

        إذا(n>0x10ffff || U_IS_SURROGATE(n)) {
            /* Unicode code point overflow */
            *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
            أرجع 0;
        }

        /* Insert n at position i of the output: */
        cp_لمائی=U16_لمائی(n);
        إذا(dest!=خالی_سوچک && ((dest_لمائی+cp_لمائی)<=dest_سمرتھا)) {
            int32_t کوڈ_Unit_اشاریہ;

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
            إذا(i<=پہلا_Supplementary_اشاریہ) {
                کوڈ_Unit_اشاریہ=i;
                إذا(cp_لمائی>1) {
                    پہلا_Supplementary_اشاریہ=کوڈ_Unit_اشاریہ;
                } وإلا {
                    ++پہلا_Supplementary_اشاریہ;
                }
            } وإلا {
                کوڈ_Unit_اشاریہ=پہلا_Supplementary_اشاریہ;
                U16_FWD_N(dest, کوڈ_Unit_اشاریہ, dest_لمائی, i-کوڈ_Unit_اشاریہ);
            }

            /* use the char16_t index codeUnitIndex instead of the code point index i */
            إذا(کوڈ_Unit_اشاریہ<dest_لمائی) {
                uprv_memmove(dest+کوڈ_Unit_اشاریہ+cp_لمائی,
                             dest+کوڈ_Unit_اشاریہ,
                             (dest_لمائی-کوڈ_Unit_اشاریہ)*U_SIZEOF_UCHAR);
                إذا(caseFlags!=خالی_سوچک) {
                    uprv_memmove(caseFlags+کوڈ_Unit_اشاریہ+cp_لمائی,
                                 caseFlags+کوڈ_Unit_اشاریہ,
                                 dest_لمائی-کوڈ_Unit_اشاریہ);
                }
            }
            إذا(cp_لمائی==1) {
                /* BMP, insert one code unit */
                dest[کوڈ_Unit_اشاریہ]=(char16_t)n;
            } وإلا {
                /* supplementary character, insert two code units */
                dest[کوڈ_Unit_اشاریہ]=U16_LEAD(n);
                dest[کوڈ_Unit_اشاریہ+1]=U16_TRAIL(n);
            }
            إذا(caseFlags!=خالی_سوچک) {
                /* Case of last character determines uppercase flag: */
                caseFlags[کوڈ_Unit_اشاریہ]=IS_BASIC_UPPERCASE(src[in-1]);
                إذا(cp_لمائی==2) {
                    caseFlags[کوڈ_Unit_اشاریہ+1]=خطأ;
                }
            }
        }
        dest_لمائی+=cp_لمائی;
        U_ASSERT(dest_لمائی>=0);
        ++i;
    }

    أرجع u_terminateUChars(dest, dest_سمرتھا, dest_لمائی, p_Error_کوڈ);
}

/* ### check notes on overflow handling - only necessary if not IDNA? are these Punycode functions to be public? */

#جے_انت /* #if !UCONFIG_NO_IDNA */
