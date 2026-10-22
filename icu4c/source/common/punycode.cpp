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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_IDNA

#شامل "unicode/ustring.h"
#شامل "unicode/utf.h"
#شامل "unicode/utf16.h"
#شامل "ustr_imp.h"
#شامل "cstring.h"
#شامل "cmemory.h"
#شامل "punycode.h"
#شامل "uassert.h"


/* Punycode ----------------------------------------------------------------- */

/* Punycode parameters for Bootstring */
#تعریف_کرو بنیاد_2            36
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
ساکن اندرونی حرف
digit_مقصد_Basic(int32_t digit, UBool uppercase) {
    /*  0..25 map to ASCII a..z or A..Z */
    /* 26..35 map to ASCII 0..9         */
    اگر(digit<26) {
        اگر(uppercase) {
            واپس static_cast<حرف>(_CAPITAL_A + digit);
        } ورنہ {
            واپس static_cast<حرف>(_SMALL_A + digit);
        }
    } ورنہ {
        واپس static_cast<حرف>((_ZERO_ - 26) + digit);
    }
}

/**
 * @return the numeric value of a basic code point (for use in representing integers)
 *         in the range 0 to BASE-1, or a negative value if cp is invalid.
 */
ساکن int32_t decodeDigit(int32_t cp) {
    اگر(cp<=u'Z') {
        اگر(cp<=u'9') {
            اگر(cp<u'0') {
                واپس -1;
            } ورنہ {
                واپس cp-u'0'+26;  // 0..9 -> 26..35
            }
        } ورنہ {
            واپس cp-u'A';  // A-Z -> 0..25
        }
    } ورنہ اگر(cp<=u'z') {
        واپس cp-'a';  // a..z -> 0..25
    } ورنہ {
        واپس -1;
    }
}

ساکن اندرونی حرف
asciiCaseMap(حرف b, UBool uppercase) {
    اگر(uppercase) {
        اگر(_SMALL_A<=b && b<=_SMALL_Z) {
            b-=(_SMALL_A-_CAPITAL_A);
        }
    } ورنہ {
        اگر(_CAPITAL_A<=b && b<=_CAPITAL_Z) {
            b+=(_SMALL_A-_CAPITAL_A);
        }
    }
    واپس b;
}

/* Punycode-specific Bootstring code ---------------------------------------- */

/*
 * The following code omits the {parts} of the pseudo-algorithm in the spec
 * that are not used with the Punycode parameter set.
 */

/* Bias adaptation function. */
ساکن int32_t
adaptBias(int32_t delta, int32_t length, UBool پہلا_وقت) {
    int32_t count;

    اگر(پہلا_وقت) {
        delta/=DAMP;
    } ورنہ {
        delta/=2;
    }

    delta+=delta/length;
    برائے(count=0; delta>((بنیاد_2-TMIN)*TMAX)/2; count+=بنیاد_2) {
        delta/=(بنیاد_2-TMIN);
    }

    واپس count+(((بنیاد_2-TMIN+1)*delta)/(delta+SKEW));
}

نامخلا {

// ICU-13727: Limit input length for n^2 algorithm
// where well-formed strings are at most 59 characters long.
constexpr int32_t ENCODE_MAX_رمز_UNITS=1000;
constexpr int32_t DECODE_MAX_CHARS=2000;

}  // namespace

// encode
U_CAPI int32_t
u_str_مقصد_Punycode(ثابت char16_t *src, int32_t src_لمبائی,
                char16_t *dest, int32_t dest_استعداد,
                ثابت UBool *caseFlags,
                UError_رمز *p_Error_رمز) {

    int32_t cp_بفر[ENCODE_MAX_رمز_UNITS];
    int32_t n, delta, handledCPCount, basic_لمبائی, dest_لمبائی, bias, j, m, q, k, t, srcCPCount;
    char16_t c, c2;

    /* argument checking */
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    اگر(src==خالی_اشاریہ || src_لمبائی<-1 || dest_استعداد<0 || (dest==خالی_اشاریہ && dest_استعداد!=0)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    اگر (src_لمبائی>ENCODE_MAX_رمز_UNITS) {
        *p_Error_رمز=U_داخلہ_TOO_LONG_ERROR;
        واپس 0;
    }

    /*
     * Handle the basic code points and
     * convert extended ones to UTF-32 in cpBuffer (caseFlag in sign bit):
     */
    srcCPCount=dest_لمبائی=0;
    اگر(src_لمبائی==-1) {
        /* NUL-terminated input */
        برائے(j=0; /* no condition */; ++j) {
            اگر((c=src[j])==0) {
                توڑو;
            }
            اگر(j>=ENCODE_MAX_رمز_UNITS) {
                *p_Error_رمز=U_داخلہ_TOO_LONG_ERROR;
                واپس 0;
            }
            اگر(IS_BASIC(c)) {
                cp_بفر[srcCPCount++]=0;
                اگر(dest_لمبائی<dest_استعداد) {
                    dest[dest_لمبائی]=
                        caseFlags!=خالی_اشاریہ ?
                            asciiCaseMap((حرف)c, caseFlags[j]) :
                            (حرف)c;
                }
                ++dest_لمبائی;
            } ورنہ {
                n=(caseFlags!=خالی_اشاریہ && caseFlags[j])<<31L;
                اگر(U16_IS_SINGLE(c)) {
                    n|=c;
                } ورنہ اگر(U16_IS_LEAD(c) && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } ورنہ {
                    /* error: unmatched surrogate */
                    *p_Error_رمز=U_INVALID_CHAR_ملا;
                    واپس 0;
                }
                cp_بفر[srcCPCount++]=n;
            }
        }
    } ورنہ {
        /* length-specified input */
        برائے(j=0; j<src_لمبائی; ++j) {
            c=src[j];
            اگر(IS_BASIC(c)) {
                cp_بفر[srcCPCount++]=0;
                اگر(dest_لمبائی<dest_استعداد) {
                    dest[dest_لمبائی]=
                        caseFlags!=خالی_اشاریہ ?
                            asciiCaseMap((حرف)c, caseFlags[j]) :
                            (حرف)c;
                }
                ++dest_لمبائی;
            } ورنہ {
                n=(caseFlags!=خالی_اشاریہ && caseFlags[j])<<31L;
                اگر(U16_IS_SINGLE(c)) {
                    n|=c;
                } ورنہ اگر(U16_IS_LEAD(c) && (j+1)<src_لمبائی && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } ورنہ {
                    /* error: unmatched surrogate */
                    *p_Error_رمز=U_INVALID_CHAR_ملا;
                    واپس 0;
                }
                cp_بفر[srcCPCount++]=n;
            }
        }
    }

    /* Finish the basic string - if it is not empty - with a delimiter. */
    basic_لمبائی=dest_لمبائی;
    اگر(basic_لمبائی>0) {
        اگر(dest_لمبائی<dest_استعداد) {
            dest[dest_لمبائی]=DELIMITER;
        }
        ++dest_لمبائی;
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
    برائے(handledCPCount=basic_لمبائی; handledCPCount<srcCPCount; /* no op */) {
        /*
         * All non-basic code points < n have been handled already.
         * Find the next larger one:
         */
        برائے(m=0x7fffffff, j=0; j<srcCPCount; ++j) {
            q=cp_بفر[j]&0x7fffffff; /* remove case flag from the sign bit */
            اگر(n<=q && q<m) {
                m=q;
            }
        }

        /*
         * Increase delta enough to advance the decoder's
         * <n,i> state to <m,0>, but guard against overflow:
         */
        اگر(m-n>(0x7fffffff-handledCPCount-delta)/(handledCPCount+1)) {
            *p_Error_رمز=U_اندرونی_پروگرام_ERROR;
            واپس 0;
        }
        delta+=(m-n)*(handledCPCount+1);
        n=m;

        /* Encode a sequence of same code points n */
        برائے(j=0; j<srcCPCount; ++j) {
            q=cp_بفر[j]&0x7fffffff; /* remove case flag from the sign bit */
            اگر(q<n) {
                ++delta;
            } ورنہ اگر(q==n) {
                /* Represent delta as a generalized variable-length integer: */
                برائے(q=delta, k=بنیاد_2; /* no condition */; k+=بنیاد_2) {

                    /** RAM: comment out the old code for conformance with draft-ietf-idn-punycode-03.txt

                    t=k-bias;
                    if(t<TMIN) {
                        t=TMIN;
                    } else if(t>TMAX) {
                        t=TMAX;
                    }
                    */

                    t=k-bias;
                    اگر(t<TMIN) {
                        t=TMIN;
                    } ورنہ اگر(k>=(bias+TMAX)) {
                        t=TMAX;
                    }

                    اگر(q<t) {
                        توڑو;
                    }

                    اگر(dest_لمبائی<dest_استعداد) {
                        dest[dest_لمبائی]=digit_مقصد_Basic(t+(q-t)%(بنیاد_2-t), 0);
                    }
                    ++dest_لمبائی;
                    q=(q-t)/(بنیاد_2-t);
                }

                اگر(dest_لمبائی<dest_استعداد) {
                    dest[dest_لمبائی] = digit_مقصد_Basic(q, cp_بفر[j] < 0);
                }
                ++dest_لمبائی;
                bias = adaptBias(delta, handledCPCount + 1, handledCPCount == basic_لمبائی);
                delta=0;
                ++handledCPCount;
            }
        }

        ++delta;
        ++n;
    }

    واپس u_terminateUChars(dest, dest_استعداد, dest_لمبائی, p_Error_رمز);
}

// decode
U_CAPI int32_t
u_strFromPunycode(ثابت char16_t *src, int32_t src_لمبائی,
                  char16_t *dest, int32_t dest_استعداد,
                  UBool *caseFlags,
                  UError_رمز *p_Error_رمز) {
    int32_t n, dest_لمبائی, i, bias, basic_لمبائی, j, in, oldi, w, k, digit, t,
            destCPCount, پہلا_Supplementary_اشاریہ, cp_لمبائی;
    char16_t b;

    /* argument checking */
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    اگر(src==خالی_اشاریہ || src_لمبائی<-1 || (dest==خالی_اشاریہ && dest_استعداد!=0)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    اگر(src_لمبائی==-1) {
        src_لمبائی=u_strlen(src);
    }
    اگر (src_لمبائی>DECODE_MAX_CHARS) {
        *p_Error_رمز=U_داخلہ_TOO_LONG_ERROR;
        واپس 0;
    }

    /*
     * Handle the basic code points:
     * Let basicLength be the number of input code points
     * before the last delimiter, or 0 if there is none,
     * then copy the first basicLength code points to the output.
     *
     * The two following loops iterate backward.
     */
    برائے(j=src_لمبائی; j>0;) {
        اگر(src[--j]==DELIMITER) {
            توڑو;
        }
    }
    dest_لمبائی=basic_لمبائی=destCPCount=j;
    U_ASSERT(dest_لمبائی>=0);

    جب(j>0) {
        b=src[--j];
        اگر(!IS_BASIC(b)) {
            *p_Error_رمز=U_INVALID_CHAR_ملا;
            واپس 0;
        }

        اگر(j<dest_استعداد) {
            dest[j] = b;

            اگر(caseFlags!=خالی_اشاریہ) {
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
    برائے(in=basic_لمبائی>0 ? basic_لمبائی+1 : 0; in<src_لمبائی; /* no op */) {
        /*
         * in is the index of the next character to be consumed, and
         * destCPCount is the number of code points in the output array.
         *
         * Decode a generalized variable-length integer into delta,
         * which gets added to i.  The overflow checking is easier
         * if we increase i as we go, then subtract off its starting
         * value at the end to obtain delta.
         */
        برائے(oldi=i, w=1, k=بنیاد_2; /* no condition */; k+=بنیاد_2) {
            اگر(in>=src_لمبائی) {
                *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                واپس 0;
            }

            digit=decodeDigit(src[in++]);
            اگر(digit<0) {
                *p_Error_رمز=U_INVALID_CHAR_ملا;
                واپس 0;
            }
            اگر(digit>(0x7fffffff-i)/w) {
                /* integer overflow */
                *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                واپس 0;
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
            اگر(t<TMIN) {
                t=TMIN;
            } ورنہ اگر(k>=(bias+TMAX)) {
                t=TMAX;
            }
            اگر(digit<t) {
                توڑو;
            }

            اگر(w>0x7fffffff/(بنیاد_2-t)) {
                /* integer overflow */
                *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                واپس 0;
            }
            w*=بنیاد_2-t;
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
        اگر(i/destCPCount>(0x7fffffff-n)) {
            /* integer overflow */
            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
            واپس 0;
        }

        n+=i/destCPCount;
        i%=destCPCount;
        /* not needed for Punycode: */
        /* if (decode_digit(n) <= BASE) return punycode_invalid_input; */

        اگر(n>0x10ffff || U_IS_SURROGATE(n)) {
            /* Unicode code point overflow */
            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
            واپس 0;
        }

        /* Insert n at position i of the output: */
        cp_لمبائی=U16_لمبائی(n);
        اگر(dest!=خالی_اشاریہ && ((dest_لمبائی+cp_لمبائی)<=dest_استعداد)) {
            int32_t رمز_Unit_اشاریہ;

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
            اگر(i<=پہلا_Supplementary_اشاریہ) {
                رمز_Unit_اشاریہ=i;
                اگر(cp_لمبائی>1) {
                    پہلا_Supplementary_اشاریہ=رمز_Unit_اشاریہ;
                } ورنہ {
                    ++پہلا_Supplementary_اشاریہ;
                }
            } ورنہ {
                رمز_Unit_اشاریہ=پہلا_Supplementary_اشاریہ;
                U16_FWD_N(dest, رمز_Unit_اشاریہ, dest_لمبائی, i-رمز_Unit_اشاریہ);
            }

            /* use the char16_t index codeUnitIndex instead of the code point index i */
            اگر(رمز_Unit_اشاریہ<dest_لمبائی) {
                uprv_memmove(dest+رمز_Unit_اشاریہ+cp_لمبائی,
                             dest+رمز_Unit_اشاریہ,
                             (dest_لمبائی-رمز_Unit_اشاریہ)*U_SIZEOF_UCHAR);
                اگر(caseFlags!=خالی_اشاریہ) {
                    uprv_memmove(caseFlags+رمز_Unit_اشاریہ+cp_لمبائی,
                                 caseFlags+رمز_Unit_اشاریہ,
                                 dest_لمبائی-رمز_Unit_اشاریہ);
                }
            }
            اگر(cp_لمبائی==1) {
                /* BMP, insert one code unit */
                dest[رمز_Unit_اشاریہ]=(char16_t)n;
            } ورنہ {
                /* supplementary character, insert two code units */
                dest[رمز_Unit_اشاریہ]=U16_LEAD(n);
                dest[رمز_Unit_اشاریہ+1]=U16_TRAIL(n);
            }
            اگر(caseFlags!=خالی_اشاریہ) {
                /* Case of last character determines uppercase flag: */
                caseFlags[رمز_Unit_اشاریہ]=IS_BASIC_UPPERCASE(src[in-1]);
                اگر(cp_لمبائی==2) {
                    caseFlags[رمز_Unit_اشاریہ+1]=جھوٹ;
                }
            }
        }
        dest_لمبائی+=cp_لمبائی;
        U_ASSERT(dest_لمبائی>=0);
        ++i;
    }

    واپس u_terminateUChars(dest, dest_استعداد, dest_لمبائی, p_Error_رمز);
}

/* ### check notes on overflow handling - only necessary if not IDNA? are these Punycode functions to be public? */

#اگر_ختم /* #if !UCONFIG_NO_IDNA */
