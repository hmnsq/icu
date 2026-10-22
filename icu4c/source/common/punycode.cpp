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

#यदि !UCONFIG_NO_IDNA

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
#परिभाषय आधारः_2            36
#परिभाषय TMIN            1
#परिभाषय TMAX            26
#परिभाषय SKEW            38
#परिभाषय DAMP            700
#परिभाषय INITIAL_BIAS    72
#परिभाषय INITIAL_N       0x80

/* "Basic" Unicode/ASCII code points */
#परिभाषय _HYPHEN         0X2d
#परिभाषय DELIMITER       _HYPHEN

#परिभाषय _ZERO_          0X30
#परिभाषय _NINE           0x39

#परिभाषय _SMALL_A        0X61
#परिभाषय _SMALL_Z        0X7a

#परिभाषय _CAPITAL_A      0X41
#परिभाषय _CAPITAL_Z      0X5a

#परिभाषय IS_BASIC(c) ((c)<0x80)
#परिभाषय IS_BASIC_UPPERCASE(c) (_CAPITAL_A<=(c) && (c)<=_CAPITAL_Z)

/**
 * digitToBasic() returns the basic code point whose value
 * (when used for representing integers) is d, which must be in the
 * range 0 to BASE-1. The lowercase form is used unless the uppercase flag is
 * nonzero, in which case the uppercase form is used.
 */
स्थिर अन्तःस्थ वर्ण
digit_गन्तव्यम्_Basic(int32_t digit, UBool uppercase) {
    /*  0..25 map to ASCII a..z or A..Z */
    /* 26..35 map to ASCII 0..9         */
    यदि(digit<26) {
        यदि(uppercase) {
            प्रत्यागम static_cast<वर्ण>(_CAPITAL_A + digit);
        } अन्यथा {
            प्रत्यागम static_cast<वर्ण>(_SMALL_A + digit);
        }
    } अन्यथा {
        प्रत्यागम static_cast<वर्ण>((_ZERO_ - 26) + digit);
    }
}

/**
 * @return the numeric value of a basic code point (for use in representing integers)
 *         in the range 0 to BASE-1, or a negative value if cp is invalid.
 */
स्थिर int32_t decodeDigit(int32_t cp) {
    यदि(cp<=u'Z') {
        यदि(cp<=u'9') {
            यदि(cp<u'0') {
                प्रत्यागम -1;
            } अन्यथा {
                प्रत्यागम cp-u'0'+26;  // 0..9 -> 26..35
            }
        } अन्यथा {
            प्रत्यागम cp-u'A';  // A-Z -> 0..25
        }
    } अन्यथा यदि(cp<=u'z') {
        प्रत्यागम cp-'a';  // a..z -> 0..25
    } अन्यथा {
        प्रत्यागम -1;
    }
}

स्थिर अन्तःस्थ वर्ण
asciiCaseMap(वर्ण b, UBool uppercase) {
    यदि(uppercase) {
        यदि(_SMALL_A<=b && b<=_SMALL_Z) {
            b-=(_SMALL_A-_CAPITAL_A);
        }
    } अन्यथा {
        यदि(_CAPITAL_A<=b && b<=_CAPITAL_Z) {
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

    यदि(firstTime) {
        delta/=DAMP;
    } अन्यथा {
        delta/=2;
    }

    delta+=delta/length;
    प्रति(count=0; delta>((आधारः_2-TMIN)*TMAX)/2; count+=आधारः_2) {
        delta/=(आधारः_2-TMIN);
    }

    प्रत्यागम count+(((आधारः_2-TMIN+1)*delta)/(delta+SKEW));
}

नामाकाश {

// ICU-13727: Limit input length for n^2 algorithm
// where well-formed strings are at most 59 characters long.
constexpr int32_t ENCODE_MAX_संहिता_UNITS=1000;
constexpr int32_t DECODE_MAX_CHARS=2000;

}  // namespace

// encode
U_CAPI int32_t
u_str_गन्तव्यम्_Punycode(ध्रुव char16_t *src, int32_t src_दीर्घता,
                char16_t *dest, int32_t dest_क्षमता,
                ध्रुव UBool *caseFlags,
                UError_संहिता *p_Error_संहिता) {

    int32_t cp_बफर[ENCODE_MAX_संहिता_UNITS];
    int32_t n, delta, handledCPCount, basic_दीर्घता, dest_दीर्घता, bias, j, m, q, k, t, srcCPCount;
    char16_t c, c2;

    /* argument checking */
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    यदि(src==शून्यसूचकः || src_दीर्घता<-1 || dest_क्षमता<0 || (dest==शून्यसूचकः && dest_क्षमता!=0)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    यदि (src_दीर्घता>ENCODE_MAX_संहिता_UNITS) {
        *p_Error_संहिता=U_निवेशः_TOO_LONG_ERROR;
        प्रत्यागम 0;
    }

    /*
     * Handle the basic code points and
     * convert extended ones to UTF-32 in cpBuffer (caseFlag in sign bit):
     */
    srcCPCount=dest_दीर्घता=0;
    यदि(src_दीर्घता==-1) {
        /* NUL-terminated input */
        प्रति(j=0; /* no condition */; ++j) {
            यदि((c=src[j])==0) {
                भङ्ग;
            }
            यदि(j>=ENCODE_MAX_संहिता_UNITS) {
                *p_Error_संहिता=U_निवेशः_TOO_LONG_ERROR;
                प्रत्यागम 0;
            }
            यदि(IS_BASIC(c)) {
                cp_बफर[srcCPCount++]=0;
                यदि(dest_दीर्घता<dest_क्षमता) {
                    dest[dest_दीर्घता]=
                        caseFlags!=शून्यसूचकः ?
                            asciiCaseMap((वर्ण)c, caseFlags[j]) :
                            (वर्ण)c;
                }
                ++dest_दीर्घता;
            } अन्यथा {
                n=(caseFlags!=शून्यसूचकः && caseFlags[j])<<31L;
                यदि(U16_IS_SINGLE(c)) {
                    n|=c;
                } अन्यथा यदि(U16_IS_LEAD(c) && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } अन्यथा {
                    /* error: unmatched surrogate */
                    *p_Error_संहिता=U_INVALID_CHAR_प्राप्तम्;
                    प्रत्यागम 0;
                }
                cp_बफर[srcCPCount++]=n;
            }
        }
    } अन्यथा {
        /* length-specified input */
        प्रति(j=0; j<src_दीर्घता; ++j) {
            c=src[j];
            यदि(IS_BASIC(c)) {
                cp_बफर[srcCPCount++]=0;
                यदि(dest_दीर्घता<dest_क्षमता) {
                    dest[dest_दीर्घता]=
                        caseFlags!=शून्यसूचकः ?
                            asciiCaseMap((वर्ण)c, caseFlags[j]) :
                            (वर्ण)c;
                }
                ++dest_दीर्घता;
            } अन्यथा {
                n=(caseFlags!=शून्यसूचकः && caseFlags[j])<<31L;
                यदि(U16_IS_SINGLE(c)) {
                    n|=c;
                } अन्यथा यदि(U16_IS_LEAD(c) && (j+1)<src_दीर्घता && U16_IS_TRAIL(c2=src[j+1])) {
                    ++j;
                    n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                } अन्यथा {
                    /* error: unmatched surrogate */
                    *p_Error_संहिता=U_INVALID_CHAR_प्राप्तम्;
                    प्रत्यागम 0;
                }
                cp_बफर[srcCPCount++]=n;
            }
        }
    }

    /* Finish the basic string - if it is not empty - with a delimiter. */
    basic_दीर्घता=dest_दीर्घता;
    यदि(basic_दीर्घता>0) {
        यदि(dest_दीर्घता<dest_क्षमता) {
            dest[dest_दीर्घता]=DELIMITER;
        }
        ++dest_दीर्घता;
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
    प्रति(handledCPCount=basic_दीर्घता; handledCPCount<srcCPCount; /* no op */) {
        /*
         * All non-basic code points < n have been handled already.
         * Find the next larger one:
         */
        प्रति(m=0x7fffffff, j=0; j<srcCPCount; ++j) {
            q=cp_बफर[j]&0x7fffffff; /* remove case flag from the sign bit */
            यदि(n<=q && q<m) {
                m=q;
            }
        }

        /*
         * Increase delta enough to advance the decoder's
         * <n,i> state to <m,0>, but guard against overflow:
         */
        यदि(m-n>(0x7fffffff-handledCPCount-delta)/(handledCPCount+1)) {
            *p_Error_संहिता=U_आन्तरिकम्_क्रमादेशः_ERROR;
            प्रत्यागम 0;
        }
        delta+=(m-n)*(handledCPCount+1);
        n=m;

        /* Encode a sequence of same code points n */
        प्रति(j=0; j<srcCPCount; ++j) {
            q=cp_बफर[j]&0x7fffffff; /* remove case flag from the sign bit */
            यदि(q<n) {
                ++delta;
            } अन्यथा यदि(q==n) {
                /* Represent delta as a generalized variable-length integer: */
                प्रति(q=delta, k=आधारः_2; /* no condition */; k+=आधारः_2) {

                    /** RAM: comment out the old code for conformance with draft-ietf-idn-punycode-03.txt

                    t=k-bias;
                    if(t<TMIN) {
                        t=TMIN;
                    } else if(t>TMAX) {
                        t=TMAX;
                    }
                    */

                    t=k-bias;
                    यदि(t<TMIN) {
                        t=TMIN;
                    } अन्यथा यदि(k>=(bias+TMAX)) {
                        t=TMAX;
                    }

                    यदि(q<t) {
                        भङ्ग;
                    }

                    यदि(dest_दीर्घता<dest_क्षमता) {
                        dest[dest_दीर्घता]=digit_गन्तव्यम्_Basic(t+(q-t)%(आधारः_2-t), 0);
                    }
                    ++dest_दीर्घता;
                    q=(q-t)/(आधारः_2-t);
                }

                यदि(dest_दीर्घता<dest_क्षमता) {
                    dest[dest_दीर्घता] = digit_गन्तव्यम्_Basic(q, cp_बफर[j] < 0);
                }
                ++dest_दीर्घता;
                bias = adaptBias(delta, handledCPCount + 1, handledCPCount == basic_दीर्घता);
                delta=0;
                ++handledCPCount;
            }
        }

        ++delta;
        ++n;
    }

    प्रत्यागम u_terminateUChars(dest, dest_क्षमता, dest_दीर्घता, p_Error_संहिता);
}

// decode
U_CAPI int32_t
u_strFromPunycode(ध्रुव char16_t *src, int32_t src_दीर्घता,
                  char16_t *dest, int32_t dest_क्षमता,
                  UBool *caseFlags,
                  UError_संहिता *p_Error_संहिता) {
    int32_t n, dest_दीर्घता, i, bias, basic_दीर्घता, j, in, oldi, w, k, digit, t,
            destCPCount, first_Supplementary_सूचकांक, cp_दीर्घता;
    char16_t b;

    /* argument checking */
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    यदि(src==शून्यसूचकः || src_दीर्घता<-1 || (dest==शून्यसूचकः && dest_क्षमता!=0)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    यदि(src_दीर्घता==-1) {
        src_दीर्घता=u_strlen(src);
    }
    यदि (src_दीर्घता>DECODE_MAX_CHARS) {
        *p_Error_संहिता=U_निवेशः_TOO_LONG_ERROR;
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
    प्रति(j=src_दीर्घता; j>0;) {
        यदि(src[--j]==DELIMITER) {
            भङ्ग;
        }
    }
    dest_दीर्घता=basic_दीर्घता=destCPCount=j;
    U_ASSERT(dest_दीर्घता>=0);

    यावत्(j>0) {
        b=src[--j];
        यदि(!IS_BASIC(b)) {
            *p_Error_संहिता=U_INVALID_CHAR_प्राप्तम्;
            प्रत्यागम 0;
        }

        यदि(j<dest_क्षमता) {
            dest[j] = b;

            यदि(caseFlags!=शून्यसूचकः) {
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
    प्रति(in=basic_दीर्घता>0 ? basic_दीर्घता+1 : 0; in<src_दीर्घता; /* no op */) {
        /*
         * in is the index of the next character to be consumed, and
         * destCPCount is the number of code points in the output array.
         *
         * Decode a generalized variable-length integer into delta,
         * which gets added to i.  The overflow checking is easier
         * if we increase i as we go, then subtract off its starting
         * value at the end to obtain delta.
         */
        प्रति(oldi=i, w=1, k=आधारः_2; /* no condition */; k+=आधारः_2) {
            यदि(in>=src_दीर्घता) {
                *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                प्रत्यागम 0;
            }

            digit=decodeDigit(src[in++]);
            यदि(digit<0) {
                *p_Error_संहिता=U_INVALID_CHAR_प्राप्तम्;
                प्रत्यागम 0;
            }
            यदि(digit>(0x7fffffff-i)/w) {
                /* integer overflow */
                *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
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
            यदि(t<TMIN) {
                t=TMIN;
            } अन्यथा यदि(k>=(bias+TMAX)) {
                t=TMAX;
            }
            यदि(digit<t) {
                भङ्ग;
            }

            यदि(w>0x7fffffff/(आधारः_2-t)) {
                /* integer overflow */
                *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                प्रत्यागम 0;
            }
            w*=आधारः_2-t;
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
        यदि(i/destCPCount>(0x7fffffff-n)) {
            /* integer overflow */
            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
            प्रत्यागम 0;
        }

        n+=i/destCPCount;
        i%=destCPCount;
        /* not needed for Punycode: */
        /* if (decode_digit(n) <= BASE) return punycode_invalid_input; */

        यदि(n>0x10ffff || U_IS_SURROGATE(n)) {
            /* Unicode code point overflow */
            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
            प्रत्यागम 0;
        }

        /* Insert n at position i of the output: */
        cp_दीर्घता=U16_दीर्घता(n);
        यदि(dest!=शून्यसूचकः && ((dest_दीर्घता+cp_दीर्घता)<=dest_क्षमता)) {
            int32_t संहिता_Unit_सूचकांक;

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
            यदि(i<=first_Supplementary_सूचकांक) {
                संहिता_Unit_सूचकांक=i;
                यदि(cp_दीर्घता>1) {
                    first_Supplementary_सूचकांक=संहिता_Unit_सूचकांक;
                } अन्यथा {
                    ++first_Supplementary_सूचकांक;
                }
            } अन्यथा {
                संहिता_Unit_सूचकांक=first_Supplementary_सूचकांक;
                U16_FWD_N(dest, संहिता_Unit_सूचकांक, dest_दीर्घता, i-संहिता_Unit_सूचकांक);
            }

            /* use the char16_t index codeUnitIndex instead of the code point index i */
            यदि(संहिता_Unit_सूचकांक<dest_दीर्घता) {
                uprv_memmove(dest+संहिता_Unit_सूचकांक+cp_दीर्घता,
                             dest+संहिता_Unit_सूचकांक,
                             (dest_दीर्घता-संहिता_Unit_सूचकांक)*U_SIZEOF_UCHAR);
                यदि(caseFlags!=शून्यसूचकः) {
                    uprv_memmove(caseFlags+संहिता_Unit_सूचकांक+cp_दीर्घता,
                                 caseFlags+संहिता_Unit_सूचकांक,
                                 dest_दीर्घता-संहिता_Unit_सूचकांक);
                }
            }
            यदि(cp_दीर्घता==1) {
                /* BMP, insert one code unit */
                dest[संहिता_Unit_सूचकांक]=(char16_t)n;
            } अन्यथा {
                /* supplementary character, insert two code units */
                dest[संहिता_Unit_सूचकांक]=U16_LEAD(n);
                dest[संहिता_Unit_सूचकांक+1]=U16_TRAIL(n);
            }
            यदि(caseFlags!=शून्यसूचकः) {
                /* Case of last character determines uppercase flag: */
                caseFlags[संहिता_Unit_सूचकांक]=IS_BASIC_UPPERCASE(src[in-1]);
                यदि(cp_दीर्घता==2) {
                    caseFlags[संहिता_Unit_सूचकांक+1]=असत्य;
                }
            }
        }
        dest_दीर्घता+=cp_दीर्घता;
        U_ASSERT(dest_दीर्घता>=0);
        ++i;
    }

    प्रत्यागम u_terminateUChars(dest, dest_क्षमता, dest_दीर्घता, p_Error_संहिता);
}

/* ### check notes on overflow handling - only necessary if not IDNA? are these Punycode functions to be public? */

#यदि_अन्तः /* #if !UCONFIG_NO_IDNA */
