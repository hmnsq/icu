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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_IDNA

#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/utf.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "punycode.h"
#అన్తర్భావ "uassert.h"


/* Punycode ----------------------------------------------------------------- */

/* Punycode parameters for Bootstring */
#నిర్వచించు పునాది_2            36
#నిర్వచించు TMIN            1
#నిర్వచించు TMAX            26
#నిర్వచించు SKEW            38
#నిర్వచించు DAMP            700
#నిర్వచించు INITIAL_BIAS    72
#నిర్వచించు INITIAL_N       0x80

/* "Basic" Unicode/ASCII code points */
#నిర్వచించు _HYPHEN         0X2d
#నిర్వచించు DELIMITER       _HYPHEN

#నిర్వచించు _ZERO_          0X30
#నిర్వచించు _NINE           0x39

#నిర్వచించు _SMALL_A        0X61
#నిర్వచించు _SMALL_Z        0X7a

#నిర్వచించు _CAPITAL_A      0X41
#నిర్వచించు _CAPITAL_Z      0X5a

#నిర్వచించు IS_BASIC(c) ((c)<0x80)
#నిర్వచించు IS_BASIC_UPPERCASE(c) (_CAPITAL_A<=(c) && (c)<=_CAPITAL_Z)

/**
 * digitToBasic() returns the basic code point whose value
 * (when used for representing integers) is d, which must be in the
 * range 0 to BASE-1. The lowercase form is used unless the uppercase flag is
 * nonzero, in which case the uppercase form is used.
 */
స్థిర అంతఃవరుస అక్షరం
digit_గమ్యం_Basic(int32_t digit, UBool uppercase) {
    /*  0..25 map to ASCII a..z or A..Z */
    /* 26..35 map to ASCII 0..9         */
    ఒకవేళ(digit<26) {
        ఒకవేళ(uppercase) {
            తిరిగి static_cast<అక్షరం>(_CAPITAL_A + digit);
        } లేకపోతే {
            తిరిగి static_cast<అక్షరం>(_SMALL_A + digit);
        }
    } లేకపోతే {
        తిరిగి static_cast<అక్షరం>((_ZERO_ - 26) + digit);
    }
}

/**
 * @return the numeric value of a basic code point (for use in representing integers)
 *         in the range 0 to BASE-1, or a negative value if cp is invalid.
 */
స్థిర int32_t decodeDigit(int32_t cp) {
    ఒకవేళ(cp<=u'Z') {
        ఒకవేళ(cp<=u'9') {
            ఒకవేళ(cp<u'0') {
                తిరిగి -1;
            } లేకపోతే {
                తిరిగి cp-u'0'+26;  // 0..9 -> 26..35
            }
        } లేకపోతే {
            తిరిగి cp-u'A';  // A-Z -> 0..25
        }
    } లేకపోతే ఒకవేళ(cp<=u'z') {
        తిరిగి cp-'a';  // a..z -> 0..25
    } లేకపోతే {
        తిరిగి -1;
    }
}

స్థిర అంతఃవరుస అక్షరం
asciiCaseMap(అక్షరం b, UBool uppercase) {
    ఒకవేళ(uppercase) {
        ఒకవేళ(_SMALL_A<=b && b<=_SMALL_Z) {
            b-=(_SMALL_A-_CAPITAL_A);
        }
    } లేకపోతే {
        ఒకవేళ(_CAPITAL_A<=b && b<=_CAPITAL_Z) {
            b+=(_SMALL_A-_CAPITAL_A);
        }
    }
    తిరిగి b;
}

/* Punycode-specific Bootstring code ---------------------------------------- */

/*
 * The following code omits the {parts} of the pseudo-algorithm in the spec
 * that are not used with the Punycode parameter set.
 */

/* Bias adaptation function. */
స్థిర int32_t
adaptBias(int32_t delta, int32_t length, UBool మొదటి_సమయం) {
    int32_t count;

    ఒకవేళ(మొదటి_సమయం) {
        delta/=DAMP;
    } లేకపోతే {
        delta/=2;
    }

    delta+=delta/length;
    కొరకు(count=0; delta>((పునాది_2-TMIN)*TMAX)/2; count+=పునాది_2) {
        delta/=(పునాది_2-TMIN);
    }

    తిరిగి count+(((పునాది_2-TMIN+1)*delta)/(delta+SKEW));
}

నామాకాశ {

// ICU-13727: Limit input length for n^2 algorithm
// where well-formed strings are at most 59 characters long.
constexpr int32_t ENCODE_MAX_సంకేతం_UNITS=1000;
constexpr int32_t DECODE_MAX_CHARS=2000;

}  // namespace

// encode
U_CAPI int32_t
u_str_గమ్యం_Punycode(స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                char16_t *dest, int32_t dest_సామర్థ్యం,
                స్థిరరాశి UBool *caseFlags,
                UError_సంకేతం *p_Error_సంకేతం) {

    int32_t cp_బఫరు[ENCODE_MAX_సంకేతం_UNITS];
    int32_t n, delta, handledCPCount, basic_నిడివి, dest_నిడివి, bias, j, m, q, k, t, srcCPCount;
    char16_t c, c2;

    /* argument checking */
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    ఒకవేళ(src==శూన్యసూచిక || src_నిడివి<-1 || dest_సామర్థ్యం<0 || (dest==శూన్యసూచిక && dest_సామర్థ్యం!=0)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    ఒకవేళ (src_నిడివి>ENCODE_MAX_సంకేతం_UNITS) {
        *p_Error_సంకేతం=U_ఆదానం_TOO_LONG_ERROR;
        తిరిగి 0;
    }

    /*
     * Handle the basic code points and
     * convert extended ones to UTF-32 in cpBuffer (caseFlag in sign bit):
     */
    srcCPCount=dest_నిడివి=0;
    ఒకవేళ(src_నిడివి==-1) {
        /* NUL-terminated input */
        కొరకు(j=0; /* no condition */; ++j) {
            ఒకవేళ((c=src[j])==0) {
                ఆపు;
            }
            ఒకవేళ(j>=ENCODE_MAX_సంకేతం_UNITS) {
                *p_Error_సంకేతం=U_ఆదానం_TOO_LONG_ERROR;
                తిరిగి 0;
            }
            ఒకవేళ(IS_BASIC(c)) {
                cp_బఫరు[srcCPCount++]=0;
                ఒకవేళ(dest_నిడివి<dest_సామర్థ్యం) {
                    dest[dest_నిడివి]=
                        caseFlags!=శూన్యసూచిక ?
                            asciiCaseMap((అక్షరం)c, caseFlags[j]) :
                            (అక్షరం)c;
                }
                ++dest_నిడివి;
            } లేకపోతే {
                n=(caseFlags!=శూన్యసూచిక && caseFlags[j])<<31L;
                ఒకవేళ(U16_IS_SINGLE(c)) {
                    n|=c;
                } లేకపోతే ఒకవేళ(U16_IS_LEAD(c) && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } లేకపోతే {
                    /* error: unmatched surrogate */
                    *p_Error_సంకేతం=U_INVALID_CHAR_దొరికింది;
                    తిరిగి 0;
                }
                cp_బఫరు[srcCPCount++]=n;
            }
        }
    } లేకపోతే {
        /* length-specified input */
        కొరకు(j=0; j<src_నిడివి; ++j) {
            c=src[j];
            ఒకవేళ(IS_BASIC(c)) {
                cp_బఫరు[srcCPCount++]=0;
                ఒకవేళ(dest_నిడివి<dest_సామర్థ్యం) {
                    dest[dest_నిడివి]=
                        caseFlags!=శూన్యసూచిక ?
                            asciiCaseMap((అక్షరం)c, caseFlags[j]) :
                            (అక్షరం)c;
                }
                ++dest_నిడివి;
            } లేకపోతే {
                n=(caseFlags!=శూన్యసూచిక && caseFlags[j])<<31L;
                ఒకవేళ(U16_IS_SINGLE(c)) {
                    n|=c;
                } లేకపోతే ఒకవేళ(U16_IS_LEAD(c) && (j+1)<src_నిడివి && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } లేకపోతే {
                    /* error: unmatched surrogate */
                    *p_Error_సంకేతం=U_INVALID_CHAR_దొరికింది;
                    తిరిగి 0;
                }
                cp_బఫరు[srcCPCount++]=n;
            }
        }
    }

    /* Finish the basic string - if it is not empty - with a delimiter. */
    basic_నిడివి=dest_నిడివి;
    ఒకవేళ(basic_నిడివి>0) {
        ఒకవేళ(dest_నిడివి<dest_సామర్థ్యం) {
            dest[dest_నిడివి]=DELIMITER;
        }
        ++dest_నిడివి;
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
    కొరకు(handledCPCount=basic_నిడివి; handledCPCount<srcCPCount; /* no op */) {
        /*
         * All non-basic code points < n have been handled already.
         * Find the next larger one:
         */
        కొరకు(m=0x7fffffff, j=0; j<srcCPCount; ++j) {
            q=cp_బఫరు[j]&0x7fffffff; /* remove case flag from the sign bit */
            ఒకవేళ(n<=q && q<m) {
                m=q;
            }
        }

        /*
         * Increase delta enough to advance the decoder's
         * <n,i> state to <m,0>, but guard against overflow:
         */
        ఒకవేళ(m-n>(0x7fffffff-handledCPCount-delta)/(handledCPCount+1)) {
            *p_Error_సంకేతం=U_అంతర్గత_కార్యక్రమం_ERROR;
            తిరిగి 0;
        }
        delta+=(m-n)*(handledCPCount+1);
        n=m;

        /* Encode a sequence of same code points n */
        కొరకు(j=0; j<srcCPCount; ++j) {
            q=cp_బఫరు[j]&0x7fffffff; /* remove case flag from the sign bit */
            ఒకవేళ(q<n) {
                ++delta;
            } లేకపోతే ఒకవేళ(q==n) {
                /* Represent delta as a generalized variable-length integer: */
                కొరకు(q=delta, k=పునాది_2; /* no condition */; k+=పునాది_2) {

                    /** RAM: comment out the old code for conformance with draft-ietf-idn-punycode-03.txt

                    t=k-bias;
                    if(t<TMIN) {
                        t=TMIN;
                    } else if(t>TMAX) {
                        t=TMAX;
                    }
                    */

                    t=k-bias;
                    ఒకవేళ(t<TMIN) {
                        t=TMIN;
                    } లేకపోతే ఒకవేళ(k>=(bias+TMAX)) {
                        t=TMAX;
                    }

                    ఒకవేళ(q<t) {
                        ఆపు;
                    }

                    ఒకవేళ(dest_నిడివి<dest_సామర్థ్యం) {
                        dest[dest_నిడివి]=digit_గమ్యం_Basic(t+(q-t)%(పునాది_2-t), 0);
                    }
                    ++dest_నిడివి;
                    q=(q-t)/(పునాది_2-t);
                }

                ఒకవేళ(dest_నిడివి<dest_సామర్థ్యం) {
                    dest[dest_నిడివి] = digit_గమ్యం_Basic(q, cp_బఫరు[j] < 0);
                }
                ++dest_నిడివి;
                bias = adaptBias(delta, handledCPCount + 1, handledCPCount == basic_నిడివి);
                delta=0;
                ++handledCPCount;
            }
        }

        ++delta;
        ++n;
    }

    తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, dest_నిడివి, p_Error_సంకేతం);
}

// decode
U_CAPI int32_t
u_strFromPunycode(స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                  char16_t *dest, int32_t dest_సామర్థ్యం,
                  UBool *caseFlags,
                  UError_సంకేతం *p_Error_సంకేతం) {
    int32_t n, dest_నిడివి, i, bias, basic_నిడివి, j, in, oldi, w, k, digit, t,
            destCPCount, మొదటి_Supplementary_సూచీ, cp_నిడివి;
    char16_t b;

    /* argument checking */
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    ఒకవేళ(src==శూన్యసూచిక || src_నిడివి<-1 || (dest==శూన్యసూచిక && dest_సామర్థ్యం!=0)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ(src_నిడివి==-1) {
        src_నిడివి=u_strlen(src);
    }
    ఒకవేళ (src_నిడివి>DECODE_MAX_CHARS) {
        *p_Error_సంకేతం=U_ఆదానం_TOO_LONG_ERROR;
        తిరిగి 0;
    }

    /*
     * Handle the basic code points:
     * Let basicLength be the number of input code points
     * before the last delimiter, or 0 if there is none,
     * then copy the first basicLength code points to the output.
     *
     * The two following loops iterate backward.
     */
    కొరకు(j=src_నిడివి; j>0;) {
        ఒకవేళ(src[--j]==DELIMITER) {
            ఆపు;
        }
    }
    dest_నిడివి=basic_నిడివి=destCPCount=j;
    U_ASSERT(dest_నిడివి>=0);

    అయితే(j>0) {
        b=src[--j];
        ఒకవేళ(!IS_BASIC(b)) {
            *p_Error_సంకేతం=U_INVALID_CHAR_దొరికింది;
            తిరిగి 0;
        }

        ఒకవేళ(j<dest_సామర్థ్యం) {
            dest[j] = b;

            ఒకవేళ(caseFlags!=శూన్యసూచిక) {
                caseFlags[j]=IS_BASIC_UPPERCASE(b);
            }
        }
    }

    /* Initialize the state: */
    n=INITIAL_N;
    i=0;
    bias=INITIAL_BIAS;
    మొదటి_Supplementary_సూచీ=1000000000;

    /*
     * Main decoding loop:
     * Start just after the last delimiter if any
     * basic code points were copied; start at the beginning otherwise.
     */
    కొరకు(in=basic_నిడివి>0 ? basic_నిడివి+1 : 0; in<src_నిడివి; /* no op */) {
        /*
         * in is the index of the next character to be consumed, and
         * destCPCount is the number of code points in the output array.
         *
         * Decode a generalized variable-length integer into delta,
         * which gets added to i.  The overflow checking is easier
         * if we increase i as we go, then subtract off its starting
         * value at the end to obtain delta.
         */
        కొరకు(oldi=i, w=1, k=పునాది_2; /* no condition */; k+=పునాది_2) {
            ఒకవేళ(in>=src_నిడివి) {
                *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                తిరిగి 0;
            }

            digit=decodeDigit(src[in++]);
            ఒకవేళ(digit<0) {
                *p_Error_సంకేతం=U_INVALID_CHAR_దొరికింది;
                తిరిగి 0;
            }
            ఒకవేళ(digit>(0x7fffffff-i)/w) {
                /* integer overflow */
                *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                తిరిగి 0;
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
            ఒకవేళ(t<TMIN) {
                t=TMIN;
            } లేకపోతే ఒకవేళ(k>=(bias+TMAX)) {
                t=TMAX;
            }
            ఒకవేళ(digit<t) {
                ఆపు;
            }

            ఒకవేళ(w>0x7fffffff/(పునాది_2-t)) {
                /* integer overflow */
                *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                తిరిగి 0;
            }
            w*=పునాది_2-t;
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
        ఒకవేళ(i/destCPCount>(0x7fffffff-n)) {
            /* integer overflow */
            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
            తిరిగి 0;
        }

        n+=i/destCPCount;
        i%=destCPCount;
        /* not needed for Punycode: */
        /* if (decode_digit(n) <= BASE) return punycode_invalid_input; */

        ఒకవేళ(n>0x10ffff || U_IS_SURROGATE(n)) {
            /* Unicode code point overflow */
            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
            తిరిగి 0;
        }

        /* Insert n at position i of the output: */
        cp_నిడివి=U16_నిడివి(n);
        ఒకవేళ(dest!=శూన్యసూచిక && ((dest_నిడివి+cp_నిడివి)<=dest_సామర్థ్యం)) {
            int32_t సంకేతం_Unit_సూచీ;

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
            ఒకవేళ(i<=మొదటి_Supplementary_సూచీ) {
                సంకేతం_Unit_సూచీ=i;
                ఒకవేళ(cp_నిడివి>1) {
                    మొదటి_Supplementary_సూచీ=సంకేతం_Unit_సూచీ;
                } లేకపోతే {
                    ++మొదటి_Supplementary_సూచీ;
                }
            } లేకపోతే {
                సంకేతం_Unit_సూచీ=మొదటి_Supplementary_సూచీ;
                U16_FWD_N(dest, సంకేతం_Unit_సూచీ, dest_నిడివి, i-సంకేతం_Unit_సూచీ);
            }

            /* use the char16_t index codeUnitIndex instead of the code point index i */
            ఒకవేళ(సంకేతం_Unit_సూచీ<dest_నిడివి) {
                uprv_memmove(dest+సంకేతం_Unit_సూచీ+cp_నిడివి,
                             dest+సంకేతం_Unit_సూచీ,
                             (dest_నిడివి-సంకేతం_Unit_సూచీ)*U_SIZEOF_UCHAR);
                ఒకవేళ(caseFlags!=శూన్యసూచిక) {
                    uprv_memmove(caseFlags+సంకేతం_Unit_సూచీ+cp_నిడివి,
                                 caseFlags+సంకేతం_Unit_సూచీ,
                                 dest_నిడివి-సంకేతం_Unit_సూచీ);
                }
            }
            ఒకవేళ(cp_నిడివి==1) {
                /* BMP, insert one code unit */
                dest[సంకేతం_Unit_సూచీ]=(char16_t)n;
            } లేకపోతే {
                /* supplementary character, insert two code units */
                dest[సంకేతం_Unit_సూచీ]=U16_LEAD(n);
                dest[సంకేతం_Unit_సూచీ+1]=U16_TRAIL(n);
            }
            ఒకవేళ(caseFlags!=శూన్యసూచిక) {
                /* Case of last character determines uppercase flag: */
                caseFlags[సంకేతం_Unit_సూచీ]=IS_BASIC_UPPERCASE(src[in-1]);
                ఒకవేళ(cp_నిడివి==2) {
                    caseFlags[సంకేతం_Unit_సూచీ+1]=అబద్ధం;
                }
            }
        }
        dest_నిడివి+=cp_నిడివి;
        U_ASSERT(dest_నిడివి>=0);
        ++i;
    }

    తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, dest_నిడివి, p_Error_సంకేతం);
}

/* ### check notes on overflow handling - only necessary if not IDNA? are these Punycode functions to be public? */

#అయితే_ముగింపు /* #if !UCONFIG_NO_IDNA */
