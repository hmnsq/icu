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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_IDNA

#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/utf.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "punycode.h"
#অন্তর্ভাব "uassert.h"


/* Punycode ----------------------------------------------------------------- */

/* Punycode parameters for Bootstring */
#define BASE            36
#define TMIN            1
#define TMAX            26
#define SKEW            38
#define DAMP            700
#define INITIAL_BIAS    72
#define INITIAL_N       0x80

/* "Basic" Unicode/ASCII code points */
#define _HYPHEN         0X2d
#define DELIMITER       _HYPHEN

#define _ZERO_          0X30
#define _NINE           0x39

#define _SMALL_A        0X61
#define _SMALL_Z        0X7a

#define _CAPITAL_A      0X41
#define _CAPITAL_Z      0X5a

#define IS_BASIC(c) ((c)<0x80)
#define IS_BASIC_UPPERCASE(c) (_CAPITAL_A<=(c) && (c)<=_CAPITAL_Z)

/**
 * digitToBasic() returns the basic code point whose value
 * (when used for representing integers) is d, which must be in the
 * range 0 to BASE-1. The lowercase form is used unless the uppercase flag is
 * nonzero, in which case the uppercase form is used.
 */
স্থির অন্তঃরেখ অক্ষর
digitToBasic(int32_t digit, UBool uppercase) {
    /*  0..25 map to ASCII a..z or A..Z */
    /* 26..35 map to ASCII 0..9         */
    যদি(digit<26) {
        যদি(uppercase) {
            ফেরত static_cast<অক্ষর>(_CAPITAL_A + digit);
        } অন্যথা {
            ফেরত static_cast<অক্ষর>(_SMALL_A + digit);
        }
    } অন্যথা {
        ফেরত static_cast<অক্ষর>((_ZERO_ - 26) + digit);
    }
}

/**
 * @return the numeric value of a basic code point (for use in representing integers)
 *         in the range 0 to BASE-1, or a negative value if cp is invalid.
 */
স্থির int32_t decodeDigit(int32_t cp) {
    যদি(cp<=u'Z') {
        যদি(cp<=u'9') {
            যদি(cp<u'0') {
                ফেরত -1;
            } অন্যথা {
                ফেরত cp-u'0'+26;  // 0..9 -> 26..35
            }
        } অন্যথা {
            ফেরত cp-u'A';  // A-Z -> 0..25
        }
    } অন্যথা যদি(cp<=u'z') {
        ফেরত cp-'a';  // a..z -> 0..25
    } অন্যথা {
        ফেরত -1;
    }
}

স্থির অন্তঃরেখ অক্ষর
asciiCaseMap(অক্ষর b, UBool uppercase) {
    যদি(uppercase) {
        যদি(_SMALL_A<=b && b<=_SMALL_Z) {
            b-=(_SMALL_A-_CAPITAL_A);
        }
    } অন্যথা {
        যদি(_CAPITAL_A<=b && b<=_CAPITAL_Z) {
            b+=(_SMALL_A-_CAPITAL_A);
        }
    }
    ফেরত b;
}

/* Punycode-specific Bootstring code ---------------------------------------- */

/*
 * The following code omits the {parts} of the pseudo-algorithm in the spec
 * that are not used with the Punycode parameter set.
 */

/* Bias adaptation function. */
স্থির int32_t
adaptBias(int32_t delta, int32_t length, UBool firstTime) {
    int32_t count;

    যদি(firstTime) {
        delta/=DAMP;
    } অন্যথা {
        delta/=2;
    }

    delta+=delta/length;
    জন্য(count=0; delta>((BASE-TMIN)*TMAX)/2; count+=BASE) {
        delta/=(BASE-TMIN);
    }

    ফেরত count+(((BASE-TMIN+1)*delta)/(delta+SKEW));
}

নামাকাশ {

// ICU-13727: Limit input length for n^2 algorithm
// where well-formed strings are at most 59 characters long.
constexpr int32_t ENCODE_MAX_CODE_UNITS=1000;
constexpr int32_t DECODE_MAX_CHARS=2000;

}  // namespace

// encode
U_CAPI int32_t
u_strToPunycode(ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                char16_t *dest, int32_t destCapacity,
                ধ্রুবক UBool *caseFlags,
                UErrorCode *pErrorCode) {

    int32_t cp_বাফার[ENCODE_MAX_CODE_UNITS];
    int32_t n, delta, handledCPCount, basic_দৈর্ঘ্য, dest_দৈর্ঘ্য, bias, j, m, q, k, t, srcCPCount;
    char16_t c, c2;

    /* argument checking */
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    যদি(src==nullptr || src_দৈর্ঘ্য<-1 || destCapacity<0 || (dest==nullptr && destCapacity!=0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    যদি (src_দৈর্ঘ্য>ENCODE_MAX_CODE_UNITS) {
        *pErrorCode=U_প্রবেশ_TOO_LONG_ERROR;
        ফেরত 0;
    }

    /*
     * Handle the basic code points and
     * convert extended ones to UTF-32 in cpBuffer (caseFlag in sign bit):
     */
    srcCPCount=dest_দৈর্ঘ্য=0;
    যদি(src_দৈর্ঘ্য==-1) {
        /* NUL-terminated input */
        জন্য(j=0; /* no condition */; ++j) {
            যদি((c=src[j])==0) {
                বিরতি;
            }
            যদি(j>=ENCODE_MAX_CODE_UNITS) {
                *pErrorCode=U_প্রবেশ_TOO_LONG_ERROR;
                ফেরত 0;
            }
            যদি(IS_BASIC(c)) {
                cp_বাফার[srcCPCount++]=0;
                যদি(dest_দৈর্ঘ্য<destCapacity) {
                    dest[dest_দৈর্ঘ্য]=
                        caseFlags!=nullptr ?
                            asciiCaseMap((অক্ষর)c, caseFlags[j]) :
                            (অক্ষর)c;
                }
                ++dest_দৈর্ঘ্য;
            } অন্যথা {
                n=(caseFlags!=nullptr && caseFlags[j])<<31L;
                যদি(U16_IS_SINGLE(c)) {
                    n|=c;
                } অন্যথা যদি(U16_IS_LEAD(c) && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } অন্যথা {
                    /* error: unmatched surrogate */
                    *pErrorCode=U_INVALID_CHAR_FOUND;
                    ফেরত 0;
                }
                cp_বাফার[srcCPCount++]=n;
            }
        }
    } অন্যথা {
        /* length-specified input */
        জন্য(j=0; j<src_দৈর্ঘ্য; ++j) {
            c=src[j];
            যদি(IS_BASIC(c)) {
                cp_বাফার[srcCPCount++]=0;
                যদি(dest_দৈর্ঘ্য<destCapacity) {
                    dest[dest_দৈর্ঘ্য]=
                        caseFlags!=nullptr ?
                            asciiCaseMap((অক্ষর)c, caseFlags[j]) :
                            (অক্ষর)c;
                }
                ++dest_দৈর্ঘ্য;
            } অন্যথা {
                n=(caseFlags!=nullptr && caseFlags[j])<<31L;
                যদি(U16_IS_SINGLE(c)) {
                    n|=c;
                } অন্যথা যদি(U16_IS_LEAD(c) && (j+1)<src_দৈর্ঘ্য && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } অন্যথা {
                    /* error: unmatched surrogate */
                    *pErrorCode=U_INVALID_CHAR_FOUND;
                    ফেরত 0;
                }
                cp_বাফার[srcCPCount++]=n;
            }
        }
    }

    /* Finish the basic string - if it is not empty - with a delimiter. */
    basic_দৈর্ঘ্য=dest_দৈর্ঘ্য;
    যদি(basic_দৈর্ঘ্য>0) {
        যদি(dest_দৈর্ঘ্য<destCapacity) {
            dest[dest_দৈর্ঘ্য]=DELIMITER;
        }
        ++dest_দৈর্ঘ্য;
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
    জন্য(handledCPCount=basic_দৈর্ঘ্য; handledCPCount<srcCPCount; /* no op */) {
        /*
         * All non-basic code points < n have been handled already.
         * Find the next larger one:
         */
        জন্য(m=0x7fffffff, j=0; j<srcCPCount; ++j) {
            q=cp_বাফার[j]&0x7fffffff; /* remove case flag from the sign bit */
            যদি(n<=q && q<m) {
                m=q;
            }
        }

        /*
         * Increase delta enough to advance the decoder's
         * <n,i> state to <m,0>, but guard against overflow:
         */
        যদি(m-n>(0x7fffffff-handledCPCount-delta)/(handledCPCount+1)) {
            *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
            ফেরত 0;
        }
        delta+=(m-n)*(handledCPCount+1);
        n=m;

        /* Encode a sequence of same code points n */
        জন্য(j=0; j<srcCPCount; ++j) {
            q=cp_বাফার[j]&0x7fffffff; /* remove case flag from the sign bit */
            যদি(q<n) {
                ++delta;
            } অন্যথা যদি(q==n) {
                /* Represent delta as a generalized variable-length integer: */
                জন্য(q=delta, k=BASE; /* no condition */; k+=BASE) {

                    /** RAM: comment out the old code for conformance with draft-ietf-idn-punycode-03.txt

                    t=k-bias;
                    if(t<TMIN) {
                        t=TMIN;
                    } else if(t>TMAX) {
                        t=TMAX;
                    }
                    */

                    t=k-bias;
                    যদি(t<TMIN) {
                        t=TMIN;
                    } অন্যথা যদি(k>=(bias+TMAX)) {
                        t=TMAX;
                    }

                    যদি(q<t) {
                        বিরতি;
                    }

                    যদি(dest_দৈর্ঘ্য<destCapacity) {
                        dest[dest_দৈর্ঘ্য]=digitToBasic(t+(q-t)%(BASE-t), 0);
                    }
                    ++dest_দৈর্ঘ্য;
                    q=(q-t)/(BASE-t);
                }

                যদি(dest_দৈর্ঘ্য<destCapacity) {
                    dest[dest_দৈর্ঘ্য] = digitToBasic(q, cp_বাফার[j] < 0);
                }
                ++dest_দৈর্ঘ্য;
                bias = adaptBias(delta, handledCPCount + 1, handledCPCount == basic_দৈর্ঘ্য);
                delta=0;
                ++handledCPCount;
            }
        }

        ++delta;
        ++n;
    }

    ফেরত u_terminateUChars(dest, destCapacity, dest_দৈর্ঘ্য, pErrorCode);
}

// decode
U_CAPI int32_t
u_strFromPunycode(ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                  char16_t *dest, int32_t destCapacity,
                  UBool *caseFlags,
                  UErrorCode *pErrorCode) {
    int32_t n, dest_দৈর্ঘ্য, i, bias, basic_দৈর্ঘ্য, j, in, oldi, w, k, digit, t,
            destCPCount, first_Supplementary_সূচক, cp_দৈর্ঘ্য;
    char16_t b;

    /* argument checking */
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    যদি(src==nullptr || src_দৈর্ঘ্য<-1 || (dest==nullptr && destCapacity!=0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    যদি(src_দৈর্ঘ্য==-1) {
        src_দৈর্ঘ্য=u_strlen(src);
    }
    যদি (src_দৈর্ঘ্য>DECODE_MAX_CHARS) {
        *pErrorCode=U_প্রবেশ_TOO_LONG_ERROR;
        ফেরত 0;
    }

    /*
     * Handle the basic code points:
     * Let basicLength be the number of input code points
     * before the last delimiter, or 0 if there is none,
     * then copy the first basicLength code points to the output.
     *
     * The two following loops iterate backward.
     */
    জন্য(j=src_দৈর্ঘ্য; j>0;) {
        যদি(src[--j]==DELIMITER) {
            বিরতি;
        }
    }
    dest_দৈর্ঘ্য=basic_দৈর্ঘ্য=destCPCount=j;
    U_ASSERT(dest_দৈর্ঘ্য>=0);

    যখন(j>0) {
        b=src[--j];
        যদি(!IS_BASIC(b)) {
            *pErrorCode=U_INVALID_CHAR_FOUND;
            ফেরত 0;
        }

        যদি(j<destCapacity) {
            dest[j] = b;

            যদি(caseFlags!=nullptr) {
                caseFlags[j]=IS_BASIC_UPPERCASE(b);
            }
        }
    }

    /* Initialize the state: */
    n=INITIAL_N;
    i=0;
    bias=INITIAL_BIAS;
    first_Supplementary_সূচক=1000000000;

    /*
     * Main decoding loop:
     * Start just after the last delimiter if any
     * basic code points were copied; start at the beginning otherwise.
     */
    জন্য(in=basic_দৈর্ঘ্য>0 ? basic_দৈর্ঘ্য+1 : 0; in<src_দৈর্ঘ্য; /* no op */) {
        /*
         * in is the index of the next character to be consumed, and
         * destCPCount is the number of code points in the output array.
         *
         * Decode a generalized variable-length integer into delta,
         * which gets added to i.  The overflow checking is easier
         * if we increase i as we go, then subtract off its starting
         * value at the end to obtain delta.
         */
        জন্য(oldi=i, w=1, k=BASE; /* no condition */; k+=BASE) {
            যদি(in>=src_দৈর্ঘ্য) {
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                ফেরত 0;
            }

            digit=decodeDigit(src[in++]);
            যদি(digit<0) {
                *pErrorCode=U_INVALID_CHAR_FOUND;
                ফেরত 0;
            }
            যদি(digit>(0x7fffffff-i)/w) {
                /* integer overflow */
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                ফেরত 0;
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
            যদি(t<TMIN) {
                t=TMIN;
            } অন্যথা যদি(k>=(bias+TMAX)) {
                t=TMAX;
            }
            যদি(digit<t) {
                বিরতি;
            }

            যদি(w>0x7fffffff/(BASE-t)) {
                /* integer overflow */
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                ফেরত 0;
            }
            w*=BASE-t;
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
        যদি(i/destCPCount>(0x7fffffff-n)) {
            /* integer overflow */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            ফেরত 0;
        }

        n+=i/destCPCount;
        i%=destCPCount;
        /* not needed for Punycode: */
        /* if (decode_digit(n) <= BASE) return punycode_invalid_input; */

        যদি(n>0x10ffff || U_IS_SURROGATE(n)) {
            /* Unicode code point overflow */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            ফেরত 0;
        }

        /* Insert n at position i of the output: */
        cp_দৈর্ঘ্য=U16_দৈর্ঘ্য(n);
        যদি(dest!=nullptr && ((dest_দৈর্ঘ্য+cp_দৈর্ঘ্য)<=destCapacity)) {
            int32_t code_Unit_সূচক;

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
            যদি(i<=first_Supplementary_সূচক) {
                code_Unit_সূচক=i;
                যদি(cp_দৈর্ঘ্য>1) {
                    first_Supplementary_সূচক=code_Unit_সূচক;
                } অন্যথা {
                    ++first_Supplementary_সূচক;
                }
            } অন্যথা {
                code_Unit_সূচক=first_Supplementary_সূচক;
                U16_FWD_N(dest, code_Unit_সূচক, dest_দৈর্ঘ্য, i-code_Unit_সূচক);
            }

            /* use the char16_t index codeUnitIndex instead of the code point index i */
            যদি(code_Unit_সূচক<dest_দৈর্ঘ্য) {
                uprv_memmove(dest+code_Unit_সূচক+cp_দৈর্ঘ্য,
                             dest+code_Unit_সূচক,
                             (dest_দৈর্ঘ্য-code_Unit_সূচক)*U_SIZEOF_UCHAR);
                যদি(caseFlags!=nullptr) {
                    uprv_memmove(caseFlags+code_Unit_সূচক+cp_দৈর্ঘ্য,
                                 caseFlags+code_Unit_সূচক,
                                 dest_দৈর্ঘ্য-code_Unit_সূচক);
                }
            }
            যদি(cp_দৈর্ঘ্য==1) {
                /* BMP, insert one code unit */
                dest[code_Unit_সূচক]=(char16_t)n;
            } অন্যথা {
                /* supplementary character, insert two code units */
                dest[code_Unit_সূচক]=U16_LEAD(n);
                dest[code_Unit_সূচক+1]=U16_TRAIL(n);
            }
            যদি(caseFlags!=nullptr) {
                /* Case of last character determines uppercase flag: */
                caseFlags[code_Unit_সূচক]=IS_BASIC_UPPERCASE(src[in-1]);
                যদি(cp_দৈর্ঘ্য==2) {
                    caseFlags[code_Unit_সূচক+1]=মিথ্যা;
                }
            }
        }
        dest_দৈর্ঘ্য+=cp_দৈর্ঘ্য;
        U_ASSERT(dest_দৈর্ঘ্য>=0);
        ++i;
    }

    ফেরত u_terminateUChars(dest, destCapacity, dest_দৈর্ঘ্য, pErrorCode);
}

/* ### check notes on overflow handling - only necessary if not IDNA? are these Punycode functions to be public? */

#endif /* #if !UCONFIG_NO_IDNA */
