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

#ꦪꦼꦤ꧀ !UCONFIG_NO_IDNA

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
    ꦪꦼꦤ꧀(digit<26) {
        ꦪꦼꦤ꧀(uppercase) {
            ꦮꦭꦶ static_cast<var_na>(_CAPITAL_A + digit);
        } ꦭꦶꦪ {
            ꦮꦭꦶ static_cast<var_na>(_SMALL_A + digit);
        }
    } ꦭꦶꦪ {
        ꦮꦭꦶ static_cast<var_na>((_ZERO_ - 26) + digit);
    }
}

/**
 * @return the numeric value of a basic code point (for use in representing integers)
 *         in the range 0 to BASE-1, or a negative value if cp is invalid.
 */
static int32_t decodeDigit(int32_t cp) {
    ꦪꦼꦤ꧀(cp<=u'Z') {
        ꦪꦼꦤ꧀(cp<=u'9') {
            ꦪꦼꦤ꧀(cp<u'0') {
                ꦮꦭꦶ -1;
            } ꦭꦶꦪ {
                ꦮꦭꦶ cp-u'0'+26;  // 0..9 -> 26..35
            }
        } ꦭꦶꦪ {
            ꦮꦭꦶ cp-u'A';  // A-Z -> 0..25
        }
    } ꦭꦶꦪ ꦪꦼꦤ꧀(cp<=u'z') {
        ꦮꦭꦶ cp-'a';  // a..z -> 0..25
    } ꦭꦶꦪ {
        ꦮꦭꦶ -1;
    }
}

static anta_hstha var_na
asciiCaseMap(var_na b, UBool uppercase) {
    ꦪꦼꦤ꧀(uppercase) {
        ꦪꦼꦤ꧀(_SMALL_A<=b && b<=_SMALL_Z) {
            b-=(_SMALL_A-_CAPITAL_A);
        }
    } ꦭꦶꦪ {
        ꦪꦼꦤ꧀(_CAPITAL_A<=b && b<=_CAPITAL_Z) {
            b+=(_SMALL_A-_CAPITAL_A);
        }
    }
    ꦮꦭꦶ b;
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

    ꦪꦼꦤ꧀(firstTime) {
        delta/=DAMP;
    } ꦭꦶꦪ {
        delta/=2;
    }

    delta+=delta/length;
    ꦏꦁꦒꦺꦴ(count=0; delta>((BASE-TMIN)*TMAX)/2; count+=BASE) {
        delta/=(BASE-TMIN);
    }

    ꦮꦭꦶ count+(((BASE-TMIN+1)*delta)/(delta+SKEW));
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
    ꦪꦼꦤ꧀(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ꦮꦭꦶ 0;
    }

    ꦪꦼꦤ꧀(src==nullptr || srcLength<-1 || destCapacity<0 || (dest==nullptr && destCapacity!=0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ 0;
    }
    ꦪꦼꦤ꧀ (srcLength>ENCODE_MAX_CODE_UNITS) {
        *pErrorCode=U_INPUT_TOO_LONG_ERROR;
        ꦮꦭꦶ 0;
    }

    /*
     * Handle the basic code points and
     * convert extended ones to UTF-32 in cpBuffer (caseFlag in sign bit):
     */
    srcCPCount=destLength=0;
    ꦪꦼꦤ꧀(srcLength==-1) {
        /* NUL-terminated input */
        ꦏꦁꦒꦺꦴ(j=0; /* no condition */; ++j) {
            ꦪꦼꦤ꧀((c=src[j])==0) {
                bha_nga;
            }
            ꦪꦼꦤ꧀(j>=ENCODE_MAX_CODE_UNITS) {
                *pErrorCode=U_INPUT_TOO_LONG_ERROR;
                ꦮꦭꦶ 0;
            }
            ꦪꦼꦤ꧀(IS_BASIC(c)) {
                cpBuffer[srcCPCount++]=0;
                ꦪꦼꦤ꧀(destLength<destCapacity) {
                    dest[destLength]=
                        caseFlags!=nullptr ?
                            asciiCaseMap((var_na)c, caseFlags[j]) :
                            (var_na)c;
                }
                ++destLength;
            } ꦭꦶꦪ {
                n=(caseFlags!=nullptr && caseFlags[j])<<31L;
                ꦪꦼꦤ꧀(U16_IS_SINGLE(c)) {
                    n|=c;
                } ꦭꦶꦪ ꦪꦼꦤ꧀(U16_IS_LEAD(c) && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } ꦭꦶꦪ {
                    /* error: unmatched surrogate */
                    *pErrorCode=U_INVALID_CHAR_FOUND;
                    ꦮꦭꦶ 0;
                }
                cpBuffer[srcCPCount++]=n;
            }
        }
    } ꦭꦶꦪ {
        /* length-specified input */
        ꦏꦁꦒꦺꦴ(j=0; j<srcLength; ++j) {
            c=src[j];
            ꦪꦼꦤ꧀(IS_BASIC(c)) {
                cpBuffer[srcCPCount++]=0;
                ꦪꦼꦤ꧀(destLength<destCapacity) {
                    dest[destLength]=
                        caseFlags!=nullptr ?
                            asciiCaseMap((var_na)c, caseFlags[j]) :
                            (var_na)c;
                }
                ++destLength;
            } ꦭꦶꦪ {
                n=(caseFlags!=nullptr && caseFlags[j])<<31L;
                ꦪꦼꦤ꧀(U16_IS_SINGLE(c)) {
                    n|=c;
                } ꦭꦶꦪ ꦪꦼꦤ꧀(U16_IS_LEAD(c) && (j+1)<srcLength && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } ꦭꦶꦪ {
                    /* error: unmatched surrogate */
                    *pErrorCode=U_INVALID_CHAR_FOUND;
                    ꦮꦭꦶ 0;
                }
                cpBuffer[srcCPCount++]=n;
            }
        }
    }

    /* Finish the basic string - if it is not empty - with a delimiter. */
    basicLength=destLength;
    ꦪꦼꦤ꧀(basicLength>0) {
        ꦪꦼꦤ꧀(destLength<destCapacity) {
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
    ꦏꦁꦒꦺꦴ(handledCPCount=basicLength; handledCPCount<srcCPCount; /* no op */) {
        /*
         * All non-basic code points < n have been handled already.
         * Find the next larger one:
         */
        ꦏꦁꦒꦺꦴ(m=0x7fffffff, j=0; j<srcCPCount; ++j) {
            q=cpBuffer[j]&0x7fffffff; /* remove case flag from the sign bit */
            ꦪꦼꦤ꧀(n<=q && q<m) {
                m=q;
            }
        }

        /*
         * Increase delta enough to advance the decoder's
         * <n,i> state to <m,0>, but guard against overflow:
         */
        ꦪꦼꦤ꧀(m-n>(0x7fffffff-handledCPCount-delta)/(handledCPCount+1)) {
            *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
            ꦮꦭꦶ 0;
        }
        delta+=(m-n)*(handledCPCount+1);
        n=m;

        /* Encode a sequence of same code points n */
        ꦏꦁꦒꦺꦴ(j=0; j<srcCPCount; ++j) {
            q=cpBuffer[j]&0x7fffffff; /* remove case flag from the sign bit */
            ꦪꦼꦤ꧀(q<n) {
                ++delta;
            } ꦭꦶꦪ ꦪꦼꦤ꧀(q==n) {
                /* Represent delta as a generalized variable-length integer: */
                ꦏꦁꦒꦺꦴ(q=delta, k=BASE; /* no condition */; k+=BASE) {

                    /** RAM: comment out the old code for conformance with draft-ietf-idn-punycode-03.txt

                    t=k-bias;
                    if(t<TMIN) {
                        t=TMIN;
                    } else if(t>TMAX) {
                        t=TMAX;
                    }
                    */

                    t=k-bias;
                    ꦪꦼꦤ꧀(t<TMIN) {
                        t=TMIN;
                    } ꦭꦶꦪ ꦪꦼꦤ꧀(k>=(bias+TMAX)) {
                        t=TMAX;
                    }

                    ꦪꦼꦤ꧀(q<t) {
                        bha_nga;
                    }

                    ꦪꦼꦤ꧀(destLength<destCapacity) {
                        dest[destLength]=digitToBasic(t+(q-t)%(BASE-t), 0);
                    }
                    ++destLength;
                    q=(q-t)/(BASE-t);
                }

                ꦪꦼꦤ꧀(destLength<destCapacity) {
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

    ꦮꦭꦶ u_terminateUChars(dest, destCapacity, destLength, pErrorCode);
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
    ꦪꦼꦤ꧀(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ꦮꦭꦶ 0;
    }

    ꦪꦼꦤ꧀(src==nullptr || srcLength<-1 || (dest==nullptr && destCapacity!=0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ 0;
    }

    ꦪꦼꦤ꧀(srcLength==-1) {
        srcLength=u_strlen(src);
    }
    ꦪꦼꦤ꧀ (srcLength>DECODE_MAX_CHARS) {
        *pErrorCode=U_INPUT_TOO_LONG_ERROR;
        ꦮꦭꦶ 0;
    }

    /*
     * Handle the basic code points:
     * Let basicLength be the number of input code points
     * before the last delimiter, or 0 if there is none,
     * then copy the first basicLength code points to the output.
     *
     * The two following loops iterate backward.
     */
    ꦏꦁꦒꦺꦴ(j=srcLength; j>0;) {
        ꦪꦼꦤ꧀(src[--j]==DELIMITER) {
            bha_nga;
        }
    }
    destLength=basicLength=destCPCount=j;
    U_ASSERT(destLength>=0);

    ꦱꦼꦢꦼꦭ(j>0) {
        b=src[--j];
        ꦪꦼꦤ꧀(!IS_BASIC(b)) {
            *pErrorCode=U_INVALID_CHAR_FOUND;
            ꦮꦭꦶ 0;
        }

        ꦪꦼꦤ꧀(j<destCapacity) {
            dest[j] = b;

            ꦪꦼꦤ꧀(caseFlags!=nullptr) {
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
    ꦏꦁꦒꦺꦴ(in=basicLength>0 ? basicLength+1 : 0; in<srcLength; /* no op */) {
        /*
         * in is the index of the next character to be consumed, and
         * destCPCount is the number of code points in the output array.
         *
         * Decode a generalized variable-length integer into delta,
         * which gets added to i.  The overflow checking is easier
         * if we increase i as we go, then subtract off its starting
         * value at the end to obtain delta.
         */
        ꦏꦁꦒꦺꦴ(oldi=i, w=1, k=BASE; /* no condition */; k+=BASE) {
            ꦪꦼꦤ꧀(in>=srcLength) {
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                ꦮꦭꦶ 0;
            }

            digit=decodeDigit(src[in++]);
            ꦪꦼꦤ꧀(digit<0) {
                *pErrorCode=U_INVALID_CHAR_FOUND;
                ꦮꦭꦶ 0;
            }
            ꦪꦼꦤ꧀(digit>(0x7fffffff-i)/w) {
                /* integer overflow */
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                ꦮꦭꦶ 0;
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
            ꦪꦼꦤ꧀(t<TMIN) {
                t=TMIN;
            } ꦭꦶꦪ ꦪꦼꦤ꧀(k>=(bias+TMAX)) {
                t=TMAX;
            }
            ꦪꦼꦤ꧀(digit<t) {
                bha_nga;
            }

            ꦪꦼꦤ꧀(w>0x7fffffff/(BASE-t)) {
                /* integer overflow */
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                ꦮꦭꦶ 0;
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
        ꦪꦼꦤ꧀(i/destCPCount>(0x7fffffff-n)) {
            /* integer overflow */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            ꦮꦭꦶ 0;
        }

        n+=i/destCPCount;
        i%=destCPCount;
        /* not needed for Punycode: */
        /* if (decode_digit(n) <= BASE) return punycode_invalid_input; */

        ꦪꦼꦤ꧀(n>0x10ffff || U_IS_SURROGATE(n)) {
            /* Unicode code point overflow */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            ꦮꦭꦶ 0;
        }

        /* Insert n at position i of the output: */
        cpLength=U16_LENGTH(n);
        ꦪꦼꦤ꧀(dest!=nullptr && ((destLength+cpLength)<=destCapacity)) {
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
            ꦪꦼꦤ꧀(i<=firstSupplementaryIndex) {
                codeUnitIndex=i;
                ꦪꦼꦤ꧀(cpLength>1) {
                    firstSupplementaryIndex=codeUnitIndex;
                } ꦭꦶꦪ {
                    ++firstSupplementaryIndex;
                }
            } ꦭꦶꦪ {
                codeUnitIndex=firstSupplementaryIndex;
                U16_FWD_N(dest, codeUnitIndex, destLength, i-codeUnitIndex);
            }

            /* use the char16_t index codeUnitIndex instead of the code point index i */
            ꦪꦼꦤ꧀(codeUnitIndex<destLength) {
                uprv_memmove(dest+codeUnitIndex+cpLength,
                             dest+codeUnitIndex,
                             (destLength-codeUnitIndex)*U_SIZEOF_UCHAR);
                ꦪꦼꦤ꧀(caseFlags!=nullptr) {
                    uprv_memmove(caseFlags+codeUnitIndex+cpLength,
                                 caseFlags+codeUnitIndex,
                                 destLength-codeUnitIndex);
                }
            }
            ꦪꦼꦤ꧀(cpLength==1) {
                /* BMP, insert one code unit */
                dest[codeUnitIndex]=(char16_t)n;
            } ꦭꦶꦪ {
                /* supplementary character, insert two code units */
                dest[codeUnitIndex]=U16_LEAD(n);
                dest[codeUnitIndex+1]=U16_TRAIL(n);
            }
            ꦪꦼꦤ꧀(caseFlags!=nullptr) {
                /* Case of last character determines uppercase flag: */
                caseFlags[codeUnitIndex]=IS_BASIC_UPPERCASE(src[in-1]);
                ꦪꦼꦤ꧀(cpLength==2) {
                    caseFlags[codeUnitIndex+1]=false;
                }
            }
        }
        destLength+=cpLength;
        U_ASSERT(destLength>=0);
        ++i;
    }

    ꦮꦭꦶ u_terminateUChars(dest, destCapacity, destLength, pErrorCode);
}

/* ### check notes on overflow handling - only necessary if not IDNA? are these Punycode functions to be public? */

#endif /* #if !UCONFIG_NO_IDNA */
