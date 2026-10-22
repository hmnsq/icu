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

#अन्तर्भाव "unicode/utypes.h"

#जइ !UCONFIG_NO_IDNA

#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "punycode.h"
#अन्तर्भाव "uassert.h"


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
स्थिर अन्तःस्थ वर्ण
digitToBasic(int32_t digit, UBool uppercase) {
    /*  0..25 map to ASCII a..z or A..Z */
    /* 26..35 map to ASCII 0..9         */
    जइ(digit<26) {
        जइ(uppercase) {
            प्रत्यागम static_cast<वर्ण>(_CAPITAL_A + digit);
        } अण्णहा {
            प्रत्यागम static_cast<वर्ण>(_SMALL_A + digit);
        }
    } अण्णहा {
        प्रत्यागम static_cast<वर्ण>((_ZERO_ - 26) + digit);
    }
}

/**
 * @return the numeric value of a basic code point (for use in representing integers)
 *         in the range 0 to BASE-1, or a negative value if cp is invalid.
 */
स्थिर int32_t decodeDigit(int32_t cp) {
    जइ(cp<=u'Z') {
        जइ(cp<=u'9') {
            जइ(cp<u'0') {
                प्रत्यागम -1;
            } अण्णहा {
                प्रत्यागम cp-u'0'+26;  // 0..9 -> 26..35
            }
        } अण्णहा {
            प्रत्यागम cp-u'A';  // A-Z -> 0..25
        }
    } अण्णहा जइ(cp<=u'z') {
        प्रत्यागम cp-'a';  // a..z -> 0..25
    } अण्णहा {
        प्रत्यागम -1;
    }
}

स्थिर अन्तःस्थ वर्ण
asciiCaseMap(वर्ण b, UBool uppercase) {
    जइ(uppercase) {
        जइ(_SMALL_A<=b && b<=_SMALL_Z) {
            b-=(_SMALL_A-_CAPITAL_A);
        }
    } अण्णहा {
        जइ(_CAPITAL_A<=b && b<=_CAPITAL_Z) {
            b+=(_SMALL_A-_CAPITAL_A);
        }
    }
    प्रत्यागम b;
}

/* Punycode-specific Bootstring code ---------------------------------------- */

/*
 * The following code omits the {parts} of the pseudo-algorithm in the spec
 * that are not used with the Punycode parameter set.
 */

/* Bias adaptation function. */
स्थिर int32_t
adaptBias(int32_t delta, int32_t length, UBool firstTime) {
    int32_t count;

    जइ(firstTime) {
        delta/=DAMP;
    } अण्णहा {
        delta/=2;
    }

    delta+=delta/length;
    प्रति(count=0; delta>((BASE-TMIN)*TMAX)/2; count+=BASE) {
        delta/=(BASE-TMIN);
    }

    प्रत्यागम count+(((BASE-TMIN+1)*delta)/(delta+SKEW));
}

नामाकाश {

// ICU-13727: Limit input length for n^2 algorithm
// where well-formed strings are at most 59 characters long.
constexpr int32_t ENCODE_MAX_CODE_UNITS=1000;
constexpr int32_t DECODE_MAX_CHARS=2000;

}  // namespace

// encode
U_CAPI int32_t
u_strToPunycode(ध्रुव char16_t *src, int32_t srcLength,
                char16_t *dest, int32_t destCapacity,
                ध्रुव UBool *caseFlags,
                UErrorCode *pErrorCode) {

    int32_t cpBuffer[ENCODE_MAX_CODE_UNITS];
    int32_t n, delta, handledCPCount, basicLength, destLength, bias, j, m, q, k, t, srcCPCount;
    char16_t c, c2;

    /* argument checking */
    जइ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    जइ(src==nullptr || srcLength<-1 || destCapacity<0 || (dest==nullptr && destCapacity!=0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }
    जइ (srcLength>ENCODE_MAX_CODE_UNITS) {
        *pErrorCode=U_निवेशः_TOO_LONG_ERROR;
        प्रत्यागम 0;
    }

    /*
     * Handle the basic code points and
     * convert extended ones to UTF-32 in cpBuffer (caseFlag in sign bit):
     */
    srcCPCount=destLength=0;
    जइ(srcLength==-1) {
        /* NUL-terminated input */
        प्रति(j=0; /* no condition */; ++j) {
            जइ((c=src[j])==0) {
                भङ्ग;
            }
            जइ(j>=ENCODE_MAX_CODE_UNITS) {
                *pErrorCode=U_निवेशः_TOO_LONG_ERROR;
                प्रत्यागम 0;
            }
            जइ(IS_BASIC(c)) {
                cpBuffer[srcCPCount++]=0;
                जइ(destLength<destCapacity) {
                    dest[destLength]=
                        caseFlags!=nullptr ?
                            asciiCaseMap((वर्ण)c, caseFlags[j]) :
                            (वर्ण)c;
                }
                ++destLength;
            } अण्णहा {
                n=(caseFlags!=nullptr && caseFlags[j])<<31L;
                जइ(U16_IS_SINGLE(c)) {
                    n|=c;
                } अण्णहा जइ(U16_IS_LEAD(c) && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } अण्णहा {
                    /* error: unmatched surrogate */
                    *pErrorCode=U_INVALID_CHAR_FOUND;
                    प्रत्यागम 0;
                }
                cpBuffer[srcCPCount++]=n;
            }
        }
    } अण्णहा {
        /* length-specified input */
        प्रति(j=0; j<srcLength; ++j) {
            c=src[j];
            जइ(IS_BASIC(c)) {
                cpBuffer[srcCPCount++]=0;
                जइ(destLength<destCapacity) {
                    dest[destLength]=
                        caseFlags!=nullptr ?
                            asciiCaseMap((वर्ण)c, caseFlags[j]) :
                            (वर्ण)c;
                }
                ++destLength;
            } अण्णहा {
                n=(caseFlags!=nullptr && caseFlags[j])<<31L;
                जइ(U16_IS_SINGLE(c)) {
                    n|=c;
                } अण्णहा जइ(U16_IS_LEAD(c) && (j+1)<srcLength && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } अण्णहा {
                    /* error: unmatched surrogate */
                    *pErrorCode=U_INVALID_CHAR_FOUND;
                    प्रत्यागम 0;
                }
                cpBuffer[srcCPCount++]=n;
            }
        }
    }

    /* Finish the basic string - if it is not empty - with a delimiter. */
    basicLength=destLength;
    जइ(basicLength>0) {
        जइ(destLength<destCapacity) {
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
    प्रति(handledCPCount=basicLength; handledCPCount<srcCPCount; /* no op */) {
        /*
         * All non-basic code points < n have been handled already.
         * Find the next larger one:
         */
        प्रति(m=0x7fffffff, j=0; j<srcCPCount; ++j) {
            q=cpBuffer[j]&0x7fffffff; /* remove case flag from the sign bit */
            जइ(n<=q && q<m) {
                m=q;
            }
        }

        /*
         * Increase delta enough to advance the decoder's
         * <n,i> state to <m,0>, but guard against overflow:
         */
        जइ(m-n>(0x7fffffff-handledCPCount-delta)/(handledCPCount+1)) {
            *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
            प्रत्यागम 0;
        }
        delta+=(m-n)*(handledCPCount+1);
        n=m;

        /* Encode a sequence of same code points n */
        प्रति(j=0; j<srcCPCount; ++j) {
            q=cpBuffer[j]&0x7fffffff; /* remove case flag from the sign bit */
            जइ(q<n) {
                ++delta;
            } अण्णहा जइ(q==n) {
                /* Represent delta as a generalized variable-length integer: */
                प्रति(q=delta, k=BASE; /* no condition */; k+=BASE) {

                    /** RAM: comment out the old code for conformance with draft-ietf-idn-punycode-03.txt

                    t=k-bias;
                    if(t<TMIN) {
                        t=TMIN;
                    } else if(t>TMAX) {
                        t=TMAX;
                    }
                    */

                    t=k-bias;
                    जइ(t<TMIN) {
                        t=TMIN;
                    } अण्णहा जइ(k>=(bias+TMAX)) {
                        t=TMAX;
                    }

                    जइ(q<t) {
                        भङ्ग;
                    }

                    जइ(destLength<destCapacity) {
                        dest[destLength]=digitToBasic(t+(q-t)%(BASE-t), 0);
                    }
                    ++destLength;
                    q=(q-t)/(BASE-t);
                }

                जइ(destLength<destCapacity) {
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

    प्रत्यागम u_terminateUChars(dest, destCapacity, destLength, pErrorCode);
}

// decode
U_CAPI int32_t
u_strFromPunycode(ध्रुव char16_t *src, int32_t srcLength,
                  char16_t *dest, int32_t destCapacity,
                  UBool *caseFlags,
                  UErrorCode *pErrorCode) {
    int32_t n, destLength, i, bias, basicLength, j, in, oldi, w, k, digit, t,
            destCPCount, firstSupplementaryIndex, cpLength;
    char16_t b;

    /* argument checking */
    जइ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    जइ(src==nullptr || srcLength<-1 || (dest==nullptr && destCapacity!=0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    जइ(srcLength==-1) {
        srcLength=u_strlen(src);
    }
    जइ (srcLength>DECODE_MAX_CHARS) {
        *pErrorCode=U_निवेशः_TOO_LONG_ERROR;
        प्रत्यागम 0;
    }

    /*
     * Handle the basic code points:
     * Let basicLength be the number of input code points
     * before the last delimiter, or 0 if there is none,
     * then copy the first basicLength code points to the output.
     *
     * The two following loops iterate backward.
     */
    प्रति(j=srcLength; j>0;) {
        जइ(src[--j]==DELIMITER) {
            भङ्ग;
        }
    }
    destLength=basicLength=destCPCount=j;
    U_ASSERT(destLength>=0);

    जाव(j>0) {
        b=src[--j];
        जइ(!IS_BASIC(b)) {
            *pErrorCode=U_INVALID_CHAR_FOUND;
            प्रत्यागम 0;
        }

        जइ(j<destCapacity) {
            dest[j] = b;

            जइ(caseFlags!=nullptr) {
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
    प्रति(in=basicLength>0 ? basicLength+1 : 0; in<srcLength; /* no op */) {
        /*
         * in is the index of the next character to be consumed, and
         * destCPCount is the number of code points in the output array.
         *
         * Decode a generalized variable-length integer into delta,
         * which gets added to i.  The overflow checking is easier
         * if we increase i as we go, then subtract off its starting
         * value at the end to obtain delta.
         */
        प्रति(oldi=i, w=1, k=BASE; /* no condition */; k+=BASE) {
            जइ(in>=srcLength) {
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                प्रत्यागम 0;
            }

            digit=decodeDigit(src[in++]);
            जइ(digit<0) {
                *pErrorCode=U_INVALID_CHAR_FOUND;
                प्रत्यागम 0;
            }
            जइ(digit>(0x7fffffff-i)/w) {
                /* integer overflow */
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                प्रत्यागम 0;
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
            जइ(t<TMIN) {
                t=TMIN;
            } अण्णहा जइ(k>=(bias+TMAX)) {
                t=TMAX;
            }
            जइ(digit<t) {
                भङ्ग;
            }

            जइ(w>0x7fffffff/(BASE-t)) {
                /* integer overflow */
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                प्रत्यागम 0;
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
        जइ(i/destCPCount>(0x7fffffff-n)) {
            /* integer overflow */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            प्रत्यागम 0;
        }

        n+=i/destCPCount;
        i%=destCPCount;
        /* not needed for Punycode: */
        /* if (decode_digit(n) <= BASE) return punycode_invalid_input; */

        जइ(n>0x10ffff || U_IS_SURROGATE(n)) {
            /* Unicode code point overflow */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            प्रत्यागम 0;
        }

        /* Insert n at position i of the output: */
        cpLength=U16_LENGTH(n);
        जइ(dest!=nullptr && ((destLength+cpLength)<=destCapacity)) {
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
            जइ(i<=firstSupplementaryIndex) {
                codeUnitIndex=i;
                जइ(cpLength>1) {
                    firstSupplementaryIndex=codeUnitIndex;
                } अण्णहा {
                    ++firstSupplementaryIndex;
                }
            } अण्णहा {
                codeUnitIndex=firstSupplementaryIndex;
                U16_FWD_N(dest, codeUnitIndex, destLength, i-codeUnitIndex);
            }

            /* use the char16_t index codeUnitIndex instead of the code point index i */
            जइ(codeUnitIndex<destLength) {
                uprv_memmove(dest+codeUnitIndex+cpLength,
                             dest+codeUnitIndex,
                             (destLength-codeUnitIndex)*U_SIZEOF_UCHAR);
                जइ(caseFlags!=nullptr) {
                    uprv_memmove(caseFlags+codeUnitIndex+cpLength,
                                 caseFlags+codeUnitIndex,
                                 destLength-codeUnitIndex);
                }
            }
            जइ(cpLength==1) {
                /* BMP, insert one code unit */
                dest[codeUnitIndex]=(char16_t)n;
            } अण्णहा {
                /* supplementary character, insert two code units */
                dest[codeUnitIndex]=U16_LEAD(n);
                dest[codeUnitIndex+1]=U16_TRAIL(n);
            }
            जइ(caseFlags!=nullptr) {
                /* Case of last character determines uppercase flag: */
                caseFlags[codeUnitIndex]=IS_BASIC_UPPERCASE(src[in-1]);
                जइ(cpLength==2) {
                    caseFlags[codeUnitIndex+1]=मिच्छ;
                }
            }
        }
        destLength+=cpLength;
        U_ASSERT(destLength>=0);
        ++i;
    }

    प्रत्यागम u_terminateUChars(dest, destCapacity, destLength, pErrorCode);
}

/* ### check notes on overflow handling - only necessary if not IDNA? are these Punycode functions to be public? */

#endif /* #if !UCONFIG_NO_IDNA */
