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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_IDNA

#समावेश "unicode/ustring.h"
#समावेश "unicode/utf.h"
#समावेश "unicode/utf16.h"
#समावेश "ustr_imp.h"
#समावेश "cstring.h"
#समावेश "cmemory.h"
#समावेश "punycode.h"
#समावेश "uassert.h"


/* Punycode ----------------------------------------------------------------- */

/* Punycode parameters for Bootstring */
#घोषणा BASE            36
#घोषणा TMIN            1
#घोषणा TMAX            26
#घोषणा SKEW            38
#घोषणा DAMP            700
#घोषणा INITIAL_BIAS    72
#घोषणा INITIAL_N       0x80

/* "Basic" Unicode/ASCII code points */
#घोषणा _HYPHEN         0X2d
#घोषणा DELIMITER       _HYPHEN

#घोषणा _ZERO_          0X30
#घोषणा _NINE           0x39

#घोषणा _SMALL_A        0X61
#घोषणा _SMALL_Z        0X7a

#घोषणा _CAPITAL_A      0X41
#घोषणा _CAPITAL_Z      0X5a

#घोषणा IS_BASIC(c) ((c)<0x80)
#घोषणा IS_BASIC_UPPERCASE(c) (_CAPITAL_A<=(c) && (c)<=_CAPITAL_Z)

/**
 * digitToBasic() returns the basic code point whose value
 * (when used for representing integers) is d, which must be in the
 * range 0 to BASE-1. The lowercase form is used unless the uppercase flag is
 * nonzero, in which case the uppercase form is used.
 */
अटल अंतरभूत अक्षर
digitToBasic(int32_t digit, UBool बड़ा) {
    /*  0..25 map to ASCII a..z or A..Z */
    /* 26..35 map to ASCII 0..9         */
    अगर(digit<26) {
        अगर(बड़ा) {
            वापस static_cast<अक्षर>(_CAPITAL_A + digit);
        } अन्यथा {
            वापस static_cast<अक्षर>(_SMALL_A + digit);
        }
    } अन्यथा {
        वापस static_cast<अक्षर>((_ZERO_ - 26) + digit);
    }
}

/**
 * @return the numeric value of a basic code point (for use in representing integers)
 *         in the range 0 to BASE-1, or a negative value if cp is invalid.
 */
अटल int32_t decodeDigit(int32_t cp) {
    अगर(cp<=u'Z') {
        अगर(cp<=u'9') {
            अगर(cp<u'0') {
                वापस -1;
            } अन्यथा {
                वापस cp-u'0'+26;  // 0..9 -> 26..35
            }
        } अन्यथा {
            वापस cp-u'A';  // A-Z -> 0..25
        }
    } अन्यथा अगर(cp<=u'z') {
        वापस cp-'a';  // a..z -> 0..25
    } अन्यथा {
        वापस -1;
    }
}

अटल अंतरभूत अक्षर
asciiCaseMap(अक्षर b, UBool बड़ा) {
    अगर(बड़ा) {
        अगर(_SMALL_A<=b && b<=_SMALL_Z) {
            b-=(_SMALL_A-_CAPITAL_A);
        }
    } अन्यथा {
        अगर(_CAPITAL_A<=b && b<=_CAPITAL_Z) {
            b+=(_SMALL_A-_CAPITAL_A);
        }
    }
    वापस b;
}

/* Punycode-specific Bootstring code ---------------------------------------- */

/*
 * The following code omits the {parts} of the pseudo-algorithm in the spec
 * that are not used with the Punycode parameter set.
 */

/* Bias adaptation function. */
अटल int32_t
adaptBias(int32_t delta, int32_t length, UBool firstTime) {
    int32_t count;

    अगर(firstTime) {
        delta/=DAMP;
    } अन्यथा {
        delta/=2;
    }

    delta+=delta/length;
    क्रम(count=0; delta>((BASE-TMIN)*TMAX)/2; count+=BASE) {
        delta/=(BASE-TMIN);
    }

    वापस count+(((BASE-TMIN+1)*delta)/(delta+SKEW));
}

namespace {

// ICU-13727: Limit input length for n^2 algorithm
// where well-formed strings are at most 59 characters long.
constexpr int32_t ENCODE_MAX_CODE_UNITS=1000;
constexpr int32_t DECODE_MAX_CHARS=2000;

}  // namespace

// encode
U_CAPI int32_t
u_strToPunycode(स्थिर char16_t *src, int32_t src_लंबाई,
                char16_t *dest, int32_t destCapacity,
                स्थिर UBool *caseFlags,
                UErrorCode *pErrorCode) {

    int32_t cp_बफर[ENCODE_MAX_CODE_UNITS];
    int32_t n, delta, handledCPCount, basic_लंबाई, dest_लंबाई, bias, j, m, q, k, t, srcCPCount;
    char16_t c, c2;

    /* argument checking */
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    अगर(src==nullptr || src_लंबाई<-1 || destCapacity<0 || (dest==nullptr && destCapacity!=0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    अगर (src_लंबाई>ENCODE_MAX_CODE_UNITS) {
        *pErrorCode=U_निवेश_TOO_LONG_ERROR;
        वापस 0;
    }

    /*
     * Handle the basic code points and
     * convert extended ones to UTF-32 in cpBuffer (caseFlag in sign bit):
     */
    srcCPCount=dest_लंबाई=0;
    अगर(src_लंबाई==-1) {
        /* NUL-terminated input */
        क्रम(j=0; /* no condition */; ++j) {
            अगर((c=src[j])==0) {
                अवरोध;
            }
            अगर(j>=ENCODE_MAX_CODE_UNITS) {
                *pErrorCode=U_निवेश_TOO_LONG_ERROR;
                वापस 0;
            }
            अगर(IS_BASIC(c)) {
                cp_बफर[srcCPCount++]=0;
                अगर(dest_लंबाई<destCapacity) {
                    dest[dest_लंबाई]=
                        caseFlags!=nullptr ?
                            asciiCaseMap((अक्षर)c, caseFlags[j]) :
                            (अक्षर)c;
                }
                ++dest_लंबाई;
            } अन्यथा {
                n=(caseFlags!=nullptr && caseFlags[j])<<31L;
                अगर(U16_IS_SINGLE(c)) {
                    n|=c;
                } अन्यथा अगर(U16_IS_LEAD(c) && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } अन्यथा {
                    /* error: unmatched surrogate */
                    *pErrorCode=U_INVALID_CHAR_FOUND;
                    वापस 0;
                }
                cp_बफर[srcCPCount++]=n;
            }
        }
    } अन्यथा {
        /* length-specified input */
        क्रम(j=0; j<src_लंबाई; ++j) {
            c=src[j];
            अगर(IS_BASIC(c)) {
                cp_बफर[srcCPCount++]=0;
                अगर(dest_लंबाई<destCapacity) {
                    dest[dest_लंबाई]=
                        caseFlags!=nullptr ?
                            asciiCaseMap((अक्षर)c, caseFlags[j]) :
                            (अक्षर)c;
                }
                ++dest_लंबाई;
            } अन्यथा {
                n=(caseFlags!=nullptr && caseFlags[j])<<31L;
                अगर(U16_IS_SINGLE(c)) {
                    n|=c;
                } अन्यथा अगर(U16_IS_LEAD(c) && (j+1)<src_लंबाई && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } अन्यथा {
                    /* error: unmatched surrogate */
                    *pErrorCode=U_INVALID_CHAR_FOUND;
                    वापस 0;
                }
                cp_बफर[srcCPCount++]=n;
            }
        }
    }

    /* Finish the basic string - if it is not empty - with a delimiter. */
    basic_लंबाई=dest_लंबाई;
    अगर(basic_लंबाई>0) {
        अगर(dest_लंबाई<destCapacity) {
            dest[dest_लंबाई]=DELIMITER;
        }
        ++dest_लंबाई;
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
    क्रम(handledCPCount=basic_लंबाई; handledCPCount<srcCPCount; /* no op */) {
        /*
         * All non-basic code points < n have been handled already.
         * Find the next larger one:
         */
        क्रम(m=0x7fffffff, j=0; j<srcCPCount; ++j) {
            q=cp_बफर[j]&0x7fffffff; /* remove case flag from the sign bit */
            अगर(n<=q && q<m) {
                m=q;
            }
        }

        /*
         * Increase delta enough to advance the decoder's
         * <n,i> state to <m,0>, but guard against overflow:
         */
        अगर(m-n>(0x7fffffff-handledCPCount-delta)/(handledCPCount+1)) {
            *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
            वापस 0;
        }
        delta+=(m-n)*(handledCPCount+1);
        n=m;

        /* Encode a sequence of same code points n */
        क्रम(j=0; j<srcCPCount; ++j) {
            q=cp_बफर[j]&0x7fffffff; /* remove case flag from the sign bit */
            अगर(q<n) {
                ++delta;
            } अन्यथा अगर(q==n) {
                /* Represent delta as a generalized variable-length integer: */
                क्रम(q=delta, k=BASE; /* no condition */; k+=BASE) {

                    /** RAM: comment out the old code for conformance with draft-ietf-idn-punycode-03.txt

                    t=k-bias;
                    if(t<TMIN) {
                        t=TMIN;
                    } else if(t>TMAX) {
                        t=TMAX;
                    }
                    */

                    t=k-bias;
                    अगर(t<TMIN) {
                        t=TMIN;
                    } अन्यथा अगर(k>=(bias+TMAX)) {
                        t=TMAX;
                    }

                    अगर(q<t) {
                        अवरोध;
                    }

                    अगर(dest_लंबाई<destCapacity) {
                        dest[dest_लंबाई]=digitToBasic(t+(q-t)%(BASE-t), 0);
                    }
                    ++dest_लंबाई;
                    q=(q-t)/(BASE-t);
                }

                अगर(dest_लंबाई<destCapacity) {
                    dest[dest_लंबाई] = digitToBasic(q, cp_बफर[j] < 0);
                }
                ++dest_लंबाई;
                bias = adaptBias(delta, handledCPCount + 1, handledCPCount == basic_लंबाई);
                delta=0;
                ++handledCPCount;
            }
        }

        ++delta;
        ++n;
    }

    वापस u_terminateUChars(dest, destCapacity, dest_लंबाई, pErrorCode);
}

// decode
U_CAPI int32_t
u_strFromPunycode(स्थिर char16_t *src, int32_t src_लंबाई,
                  char16_t *dest, int32_t destCapacity,
                  UBool *caseFlags,
                  UErrorCode *pErrorCode) {
    int32_t n, dest_लंबाई, i, bias, basic_लंबाई, j, in, oldi, w, k, digit, t,
            destCPCount, first_Supplementary_सूचकांक, cp_लंबाई;
    char16_t b;

    /* argument checking */
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    अगर(src==nullptr || src_लंबाई<-1 || (dest==nullptr && destCapacity!=0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    अगर(src_लंबाई==-1) {
        src_लंबाई=u_strlen(src);
    }
    अगर (src_लंबाई>DECODE_MAX_CHARS) {
        *pErrorCode=U_निवेश_TOO_LONG_ERROR;
        वापस 0;
    }

    /*
     * Handle the basic code points:
     * Let basicLength be the number of input code points
     * before the last delimiter, or 0 if there is none,
     * then copy the first basicLength code points to the output.
     *
     * The two following loops iterate backward.
     */
    क्रम(j=src_लंबाई; j>0;) {
        अगर(src[--j]==DELIMITER) {
            अवरोध;
        }
    }
    dest_लंबाई=basic_लंबाई=destCPCount=j;
    U_ASSERT(dest_लंबाई>=0);

    जबतक(j>0) {
        b=src[--j];
        अगर(!IS_BASIC(b)) {
            *pErrorCode=U_INVALID_CHAR_FOUND;
            वापस 0;
        }

        अगर(j<destCapacity) {
            dest[j] = b;

            अगर(caseFlags!=nullptr) {
                caseFlags[j]=IS_BASIC_UPPERCASE(b);
            }
        }
    }

    /* Initialize the state: */
    n=INITIAL_N;
    i=0;
    bias=INITIAL_BIAS;
    first_Supplementary_सूचकांक=1000000000;

    /*
     * Main decoding loop:
     * Start just after the last delimiter if any
     * basic code points were copied; start at the beginning otherwise.
     */
    क्रम(in=basic_लंबाई>0 ? basic_लंबाई+1 : 0; in<src_लंबाई; /* no op */) {
        /*
         * in is the index of the next character to be consumed, and
         * destCPCount is the number of code points in the output array.
         *
         * Decode a generalized variable-length integer into delta,
         * which gets added to i.  The overflow checking is easier
         * if we increase i as we go, then subtract off its starting
         * value at the end to obtain delta.
         */
        क्रम(oldi=i, w=1, k=BASE; /* no condition */; k+=BASE) {
            अगर(in>=src_लंबाई) {
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                वापस 0;
            }

            digit=decodeDigit(src[in++]);
            अगर(digit<0) {
                *pErrorCode=U_INVALID_CHAR_FOUND;
                वापस 0;
            }
            अगर(digit>(0x7fffffff-i)/w) {
                /* integer overflow */
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                वापस 0;
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
            अगर(t<TMIN) {
                t=TMIN;
            } अन्यथा अगर(k>=(bias+TMAX)) {
                t=TMAX;
            }
            अगर(digit<t) {
                अवरोध;
            }

            अगर(w>0x7fffffff/(BASE-t)) {
                /* integer overflow */
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                वापस 0;
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
        अगर(i/destCPCount>(0x7fffffff-n)) {
            /* integer overflow */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            वापस 0;
        }

        n+=i/destCPCount;
        i%=destCPCount;
        /* not needed for Punycode: */
        /* if (decode_digit(n) <= BASE) return punycode_invalid_input; */

        अगर(n>0x10ffff || U_IS_SURROGATE(n)) {
            /* Unicode code point overflow */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            वापस 0;
        }

        /* Insert n at position i of the output: */
        cp_लंबाई=U16_लंबाई(n);
        अगर(dest!=nullptr && ((dest_लंबाई+cp_लंबाई)<=destCapacity)) {
            int32_t code_Unit_सूचकांक;

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
            अगर(i<=first_Supplementary_सूचकांक) {
                code_Unit_सूचकांक=i;
                अगर(cp_लंबाई>1) {
                    first_Supplementary_सूचकांक=code_Unit_सूचकांक;
                } अन्यथा {
                    ++first_Supplementary_सूचकांक;
                }
            } अन्यथा {
                code_Unit_सूचकांक=first_Supplementary_सूचकांक;
                U16_FWD_N(dest, code_Unit_सूचकांक, dest_लंबाई, i-code_Unit_सूचकांक);
            }

            /* use the char16_t index codeUnitIndex instead of the code point index i */
            अगर(code_Unit_सूचकांक<dest_लंबाई) {
                uprv_memmove(dest+code_Unit_सूचकांक+cp_लंबाई,
                             dest+code_Unit_सूचकांक,
                             (dest_लंबाई-code_Unit_सूचकांक)*U_SIZEOF_UCHAR);
                अगर(caseFlags!=nullptr) {
                    uprv_memmove(caseFlags+code_Unit_सूचकांक+cp_लंबाई,
                                 caseFlags+code_Unit_सूचकांक,
                                 dest_लंबाई-code_Unit_सूचकांक);
                }
            }
            अगर(cp_लंबाई==1) {
                /* BMP, insert one code unit */
                dest[code_Unit_सूचकांक]=(char16_t)n;
            } अन्यथा {
                /* supplementary character, insert two code units */
                dest[code_Unit_सूचकांक]=U16_LEAD(n);
                dest[code_Unit_सूचकांक+1]=U16_TRAIL(n);
            }
            अगर(caseFlags!=nullptr) {
                /* Case of last character determines uppercase flag: */
                caseFlags[code_Unit_सूचकांक]=IS_BASIC_UPPERCASE(src[in-1]);
                अगर(cp_लंबाई==2) {
                    caseFlags[code_Unit_सूचकांक+1]=false;
                }
            }
        }
        dest_लंबाई+=cp_लंबाई;
        U_ASSERT(dest_लंबाई>=0);
        ++i;
    }

    वापस u_terminateUChars(dest, destCapacity, dest_लंबाई, pErrorCode);
}

/* ### check notes on overflow handling - only necessary if not IDNA? are these Punycode functions to be public? */

#पूर्ण_अगर /* #if !UCONFIG_NO_IDNA */
