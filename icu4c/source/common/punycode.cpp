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

#include "unicode/utypes.h"

#ပေႃး !UCONFIG_NO_IDNA

#include "unicode/ustring.h"
#include "unicode/utf.h"
#include "unicode/utf16.h"
#include "ustr_imp.h"
#include "cstring.h"
#include "cmemory.h"
#include "punycode.h"
#include "uassert.h"


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
static anta_hstha var_na
digitToBasic(int32_t digit, UBool uppercase) {
    /*  0..25 map to ASCII a..z or A..Z */
    /* 26..35 map to ASCII 0..9         */
    ပေႃး(digit<26) {
        ပေႃး(uppercase) {
            ပွၵ်ႈ static_cast<var_na>(_CAPITAL_A + digit);
        } ပေႃးဢမ်ႇ {
            ပွၵ်ႈ static_cast<var_na>(_SMALL_A + digit);
        }
    } ပေႃးဢမ်ႇ {
        ပွၵ်ႈ static_cast<var_na>((_ZERO_ - 26) + digit);
    }
}

/**
 * @return the numeric value of a basic code point (for use in representing integers)
 *         in the range 0 to BASE-1, or a negative value if cp is invalid.
 */
static int32_t decodeDigit(int32_t cp) {
    ပေႃး(cp<=u'Z') {
        ပေႃး(cp<=u'9') {
            ပေႃး(cp<u'0') {
                ပွၵ်ႈ -1;
            } ပေႃးဢမ်ႇ {
                ပွၵ်ႈ cp-u'0'+26;  // 0..9 -> 26..35
            }
        } ပေႃးဢမ်ႇ {
            ပွၵ်ႈ cp-u'A';  // A-Z -> 0..25
        }
    } ပေႃးဢမ်ႇ ပေႃး(cp<=u'z') {
        ပွၵ်ႈ cp-'a';  // a..z -> 0..25
    } ပေႃးဢမ်ႇ {
        ပွၵ်ႈ -1;
    }
}

static anta_hstha var_na
asciiCaseMap(var_na b, UBool uppercase) {
    ပေႃး(uppercase) {
        ပေႃး(_SMALL_A<=b && b<=_SMALL_Z) {
            b-=(_SMALL_A-_CAPITAL_A);
        }
    } ပေႃးဢမ်ႇ {
        ပေႃး(_CAPITAL_A<=b && b<=_CAPITAL_Z) {
            b+=(_SMALL_A-_CAPITAL_A);
        }
    }
    ပွၵ်ႈ b;
}

/* Punycode-specific Bootstring code ---------------------------------------- */

/*
 * The following code omits the {parts} of the pseudo-algorithm in the spec
 * that are not used with the Punycode parameter set.
 */

/* Bias adaptation function. */
static int32_t
adaptBias(int32_t delta, int32_t length, UBool firstTime) {
    int32_t count;

    ပေႃး(firstTime) {
        delta/=DAMP;
    } ပေႃးဢမ်ႇ {
        delta/=2;
    }

    delta+=delta/length;
    for(count=0; delta>((BASE-TMIN)*TMAX)/2; count+=BASE) {
        delta/=(BASE-TMIN);
    }

    ပွၵ်ႈ count+(((BASE-TMIN+1)*delta)/(delta+SKEW));
}

namespace {

// ICU-13727: Limit input length for n^2 algorithm
// where well-formed strings are at most 59 characters long.
constexpr int32_t ENCODE_MAX_CODE_UNITS=1000;
constexpr int32_t DECODE_MAX_CHARS=2000;

}  // namespace

// encode
U_CAPI int32_t
u_strToPunycode(const char16_t *src, int32_t srcLength,
                char16_t *dest, int32_t destCapacity,
                const UBool *caseFlags,
                UErrorCode *pErrorCode) {

    int32_t cpBuffer[ENCODE_MAX_CODE_UNITS];
    int32_t n, delta, handledCPCount, basicLength, destLength, bias, j, m, q, k, t, srcCPCount;
    char16_t c, c2;

    /* argument checking */
    ပေႃး(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ပွၵ်ႈ 0;
    }

    ပေႃး(src==nullptr || srcLength<-1 || destCapacity<0 || (dest==nullptr && destCapacity!=0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ 0;
    }
    ပေႃး (srcLength>ENCODE_MAX_CODE_UNITS) {
        *pErrorCode=U_INPUT_TOO_LONG_ERROR;
        ပွၵ်ႈ 0;
    }

    /*
     * Handle the basic code points and
     * convert extended ones to UTF-32 in cpBuffer (caseFlag in sign bit):
     */
    srcCPCount=destLength=0;
    ပေႃး(srcLength==-1) {
        /* NUL-terminated input */
        for(j=0; /* no condition */; ++j) {
            ပေႃး((c=src[j])==0) {
                bha_nga;
            }
            ပေႃး(j>=ENCODE_MAX_CODE_UNITS) {
                *pErrorCode=U_INPUT_TOO_LONG_ERROR;
                ပွၵ်ႈ 0;
            }
            ပေႃး(IS_BASIC(c)) {
                cpBuffer[srcCPCount++]=0;
                ပေႃး(destLength<destCapacity) {
                    dest[destLength]=
                        caseFlags!=nullptr ?
                            asciiCaseMap((var_na)c, caseFlags[j]) :
                            (var_na)c;
                }
                ++destLength;
            } ပေႃးဢမ်ႇ {
                n=(caseFlags!=nullptr && caseFlags[j])<<31L;
                ပေႃး(U16_IS_SINGLE(c)) {
                    n|=c;
                } ပေႃးဢမ်ႇ ပေႃး(U16_IS_LEAD(c) && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } ပေႃးဢမ်ႇ {
                    /* error: unmatched surrogate */
                    *pErrorCode=U_INVALID_CHAR_FOUND;
                    ပွၵ်ႈ 0;
                }
                cpBuffer[srcCPCount++]=n;
            }
        }
    } ပေႃးဢမ်ႇ {
        /* length-specified input */
        for(j=0; j<srcLength; ++j) {
            c=src[j];
            ပေႃး(IS_BASIC(c)) {
                cpBuffer[srcCPCount++]=0;
                ပေႃး(destLength<destCapacity) {
                    dest[destLength]=
                        caseFlags!=nullptr ?
                            asciiCaseMap((var_na)c, caseFlags[j]) :
                            (var_na)c;
                }
                ++destLength;
            } ပေႃးဢမ်ႇ {
                n=(caseFlags!=nullptr && caseFlags[j])<<31L;
                ပေႃး(U16_IS_SINGLE(c)) {
                    n|=c;
                } ပေႃးဢမ်ႇ ပေႃး(U16_IS_LEAD(c) && (j+1)<srcLength && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } ပေႃးဢမ်ႇ {
                    /* error: unmatched surrogate */
                    *pErrorCode=U_INVALID_CHAR_FOUND;
                    ပွၵ်ႈ 0;
                }
                cpBuffer[srcCPCount++]=n;
            }
        }
    }

    /* Finish the basic string - if it is not empty - with a delimiter. */
    basicLength=destLength;
    ပေႃး(basicLength>0) {
        ပေႃး(destLength<destCapacity) {
            dest[destLength]=DELIMITER;
        }
        ++destLength;
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
    for(handledCPCount=basicLength; handledCPCount<srcCPCount; /* no op */) {
        /*
         * All non-basic code points < n have been handled already.
         * Find the next larger one:
         */
        for(m=0x7fffffff, j=0; j<srcCPCount; ++j) {
            q=cpBuffer[j]&0x7fffffff; /* remove case flag from the sign bit */
            ပေႃး(n<=q && q<m) {
                m=q;
            }
        }

        /*
         * Increase delta enough to advance the decoder's
         * <n,i> state to <m,0>, but guard against overflow:
         */
        ပေႃး(m-n>(0x7fffffff-handledCPCount-delta)/(handledCPCount+1)) {
            *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
            ပွၵ်ႈ 0;
        }
        delta+=(m-n)*(handledCPCount+1);
        n=m;

        /* Encode a sequence of same code points n */
        for(j=0; j<srcCPCount; ++j) {
            q=cpBuffer[j]&0x7fffffff; /* remove case flag from the sign bit */
            ပေႃး(q<n) {
                ++delta;
            } ပေႃးဢမ်ႇ ပေႃး(q==n) {
                /* Represent delta as a generalized variable-length integer: */
                for(q=delta, k=BASE; /* no condition */; k+=BASE) {

                    /** RAM: comment out the old code for conformance with draft-ietf-idn-punycode-03.txt

                    t=k-bias;
                    if(t<TMIN) {
                        t=TMIN;
                    } else if(t>TMAX) {
                        t=TMAX;
                    }
                    */

                    t=k-bias;
                    ပေႃး(t<TMIN) {
                        t=TMIN;
                    } ပေႃးဢမ်ႇ ပေႃး(k>=(bias+TMAX)) {
                        t=TMAX;
                    }

                    ပေႃး(q<t) {
                        bha_nga;
                    }

                    ပေႃး(destLength<destCapacity) {
                        dest[destLength]=digitToBasic(t+(q-t)%(BASE-t), 0);
                    }
                    ++destLength;
                    q=(q-t)/(BASE-t);
                }

                ပေႃး(destLength<destCapacity) {
                    dest[destLength] = digitToBasic(q, cpBuffer[j] < 0);
                }
                ++destLength;
                bias = adaptBias(delta, handledCPCount + 1, handledCPCount == basicLength);
                delta=0;
                ++handledCPCount;
            }
        }

        ++delta;
        ++n;
    }

    ပွၵ်ႈ u_terminateUChars(dest, destCapacity, destLength, pErrorCode);
}

// decode
U_CAPI int32_t
u_strFromPunycode(const char16_t *src, int32_t srcLength,
                  char16_t *dest, int32_t destCapacity,
                  UBool *caseFlags,
                  UErrorCode *pErrorCode) {
    int32_t n, destLength, i, bias, basicLength, j, in, oldi, w, k, digit, t,
            destCPCount, firstSupplementaryIndex, cpLength;
    char16_t b;

    /* argument checking */
    ပေႃး(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ပွၵ်ႈ 0;
    }

    ပေႃး(src==nullptr || srcLength<-1 || (dest==nullptr && destCapacity!=0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ 0;
    }

    ပေႃး(srcLength==-1) {
        srcLength=u_strlen(src);
    }
    ပေႃး (srcLength>DECODE_MAX_CHARS) {
        *pErrorCode=U_INPUT_TOO_LONG_ERROR;
        ပွၵ်ႈ 0;
    }

    /*
     * Handle the basic code points:
     * Let basicLength be the number of input code points
     * before the last delimiter, or 0 if there is none,
     * then copy the first basicLength code points to the output.
     *
     * The two following loops iterate backward.
     */
    for(j=srcLength; j>0;) {
        ပေႃး(src[--j]==DELIMITER) {
            bha_nga;
        }
    }
    destLength=basicLength=destCPCount=j;
    U_ASSERT(destLength>=0);

    ၶၢဝ်း(j>0) {
        b=src[--j];
        ပေႃး(!IS_BASIC(b)) {
            *pErrorCode=U_INVALID_CHAR_FOUND;
            ပွၵ်ႈ 0;
        }

        ပေႃး(j<destCapacity) {
            dest[j] = b;

            ပေႃး(caseFlags!=nullptr) {
                caseFlags[j]=IS_BASIC_UPPERCASE(b);
            }
        }
    }

    /* Initialize the state: */
    n=INITIAL_N;
    i=0;
    bias=INITIAL_BIAS;
    firstSupplementaryIndex=1000000000;

    /*
     * Main decoding loop:
     * Start just after the last delimiter if any
     * basic code points were copied; start at the beginning otherwise.
     */
    for(in=basicLength>0 ? basicLength+1 : 0; in<srcLength; /* no op */) {
        /*
         * in is the index of the next character to be consumed, and
         * destCPCount is the number of code points in the output array.
         *
         * Decode a generalized variable-length integer into delta,
         * which gets added to i.  The overflow checking is easier
         * if we increase i as we go, then subtract off its starting
         * value at the end to obtain delta.
         */
        for(oldi=i, w=1, k=BASE; /* no condition */; k+=BASE) {
            ပေႃး(in>=srcLength) {
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                ပွၵ်ႈ 0;
            }

            digit=decodeDigit(src[in++]);
            ပေႃး(digit<0) {
                *pErrorCode=U_INVALID_CHAR_FOUND;
                ပွၵ်ႈ 0;
            }
            ပေႃး(digit>(0x7fffffff-i)/w) {
                /* integer overflow */
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                ပွၵ်ႈ 0;
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
            ပေႃး(t<TMIN) {
                t=TMIN;
            } ပေႃးဢမ်ႇ ပေႃး(k>=(bias+TMAX)) {
                t=TMAX;
            }
            ပေႃး(digit<t) {
                bha_nga;
            }

            ပေႃး(w>0x7fffffff/(BASE-t)) {
                /* integer overflow */
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                ပွၵ်ႈ 0;
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
        ပေႃး(i/destCPCount>(0x7fffffff-n)) {
            /* integer overflow */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            ပွၵ်ႈ 0;
        }

        n+=i/destCPCount;
        i%=destCPCount;
        /* not needed for Punycode: */
        /* if (decode_digit(n) <= BASE) return punycode_invalid_input; */

        ပေႃး(n>0x10ffff || U_IS_SURROGATE(n)) {
            /* Unicode code point overflow */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            ပွၵ်ႈ 0;
        }

        /* Insert n at position i of the output: */
        cpLength=U16_LENGTH(n);
        ပေႃး(dest!=nullptr && ((destLength+cpLength)<=destCapacity)) {
            int32_t codeUnitIndex;

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
            ပေႃး(i<=firstSupplementaryIndex) {
                codeUnitIndex=i;
                ပေႃး(cpLength>1) {
                    firstSupplementaryIndex=codeUnitIndex;
                } ပေႃးဢမ်ႇ {
                    ++firstSupplementaryIndex;
                }
            } ပေႃးဢမ်ႇ {
                codeUnitIndex=firstSupplementaryIndex;
                U16_FWD_N(dest, codeUnitIndex, destLength, i-codeUnitIndex);
            }

            /* use the char16_t index codeUnitIndex instead of the code point index i */
            ပေႃး(codeUnitIndex<destLength) {
                uprv_memmove(dest+codeUnitIndex+cpLength,
                             dest+codeUnitIndex,
                             (destLength-codeUnitIndex)*U_SIZEOF_UCHAR);
                ပေႃး(caseFlags!=nullptr) {
                    uprv_memmove(caseFlags+codeUnitIndex+cpLength,
                                 caseFlags+codeUnitIndex,
                                 destLength-codeUnitIndex);
                }
            }
            ပေႃး(cpLength==1) {
                /* BMP, insert one code unit */
                dest[codeUnitIndex]=(char16_t)n;
            } ပေႃးဢမ်ႇ {
                /* supplementary character, insert two code units */
                dest[codeUnitIndex]=U16_LEAD(n);
                dest[codeUnitIndex+1]=U16_TRAIL(n);
            }
            ပေႃး(caseFlags!=nullptr) {
                /* Case of last character determines uppercase flag: */
                caseFlags[codeUnitIndex]=IS_BASIC_UPPERCASE(src[in-1]);
                ပေႃး(cpLength==2) {
                    caseFlags[codeUnitIndex+1]=false;
                }
            }
        }
        destLength+=cpLength;
        U_ASSERT(destLength>=0);
        ++i;
    }

    ပွၵ်ႈ u_terminateUChars(dest, destCapacity, destLength, pErrorCode);
}

/* ### check notes on overflow handling - only necessary if not IDNA? are these Punycode functions to be public? */

#endif /* #if !UCONFIG_NO_IDNA */
