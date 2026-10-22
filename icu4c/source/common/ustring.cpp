// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1998-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
* File ustring.cpp
*
* Modification History:
*
*   Date        Name        Description
*   12/07/98    bertrand    Creation.
******************************************************************************
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cwchar.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "ustr_imp.h"

/* ANSI string.h - style functions ------------------------------------------ */

/* U+ffff is the highest BMP code point, the highest one that fits into a 16-bit char16_t */
#నిర్వచించు U_BMP_MAX 0xffff

/* Forward binary string search functions ----------------------------------- */

/*
 * Test if a substring match inside a string is at code point boundaries.
 * All pointers refer to the same buffer.
 * The limit pointer may be nullptr, all others must be real pointers.
 */
స్థిర అంతఃవరుస UBool
is_సరిపోలిక_At_CPBoundary(స్థిరరాశి char16_t *ప్రారంభం, స్థిరరాశి char16_t *సరిపోలిక, స్థిరరాశి char16_t *సరిపోలిక_హద్దు, స్థిరరాశి char16_t *హద్దు) {
    ఒకవేళ(U16_IS_TRAIL(*సరిపోలిక) && ప్రారంభం!=సరిపోలిక && U16_IS_LEAD(*(సరిపోలిక-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        తిరిగి అబద్ధం;
    }
    ఒకవేళ(U16_IS_LEAD(*(సరిపోలిక_హద్దు-1)) && సరిపోలిక_హద్దు!=హద్దు && U16_IS_TRAIL(*సరిపోలిక_హద్దు)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        తిరిగి అబద్ధం;
    }
    తిరిగి నిజం;
}

U_CAPI char16_t* U_EXPORT2
u_str_కనుగొను_మొదటి(స్థిరరాశి char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               స్థిరరాశి char16_t* sub,
               int32_t sub_నిడివి) {
    స్థిరరాశి char16_t *ప్రారంభం, *p, *q, *sub_హద్దు;
    char16_t c, cs, cq;

    ఒకవేళ(sub==శూన్యసూచిక || sub_నిడివి<-1) {
        తిరిగి (char16_t *)s;
    }
    ఒకవేళ(s==శూన్యసూచిక || length<-1) {
        తిరిగి శూన్యసూచిక;
    }

    ప్రారంభం=s;

    ఒకవేళ(length<0 && sub_నిడివి<0) {
        /* both strings are NUL-terminated */
        ఒకవేళ((cs=*sub++)==0) {
            తిరిగి (char16_t *)s;
        }
        ఒకవేళ(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            తిరిగి u_strchr(s, cs);
        }

        అయితే((c=*s++)!=0) {
            ఒకవేళ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                కొరకు(;;) {
                    ఒకవేళ((cq=*q)==0) {
                        ఒకవేళ(is_సరిపోలిక_At_CPBoundary(ప్రారంభం, s-1, p, శూన్యసూచిక)) {
                            తిరిగి (char16_t *)(s-1); /* well-formed match */
                        } లేకపోతే {
                            ఆపు; /* no match because surrogate pair is split */
                        }
                    }
                    ఒకవేళ((c=*p)==0) {
                        తిరిగి శూన్యసూచిక; /* no match, and none possible after s */
                    }
                    ఒకవేళ(c!=cq) {
                        ఆపు; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ(sub_నిడివి<0) {
        sub_నిడివి=u_strlen(sub);
    }
    ఒకవేళ(sub_నిడివి==0) {
        తిరిగి (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --sub_నిడివి;
    sub_హద్దు=sub+sub_నిడివి;

    ఒకవేళ(sub_నిడివి==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        తిరిగి length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    ఒకవేళ(length<0) {
        /* s is NUL-terminated */
        అయితే((c=*s++)!=0) {
            ఒకవేళ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                కొరకు(;;) {
                    ఒకవేళ(q==sub_హద్దు) {
                        ఒకవేళ(is_సరిపోలిక_At_CPBoundary(ప్రారంభం, s-1, p, శూన్యసూచిక)) {
                            తిరిగి (char16_t *)(s-1); /* well-formed match */
                        } లేకపోతే {
                            ఆపు; /* no match because surrogate pair is split */
                        }
                    }
                    ఒకవేళ((c=*p)==0) {
                        తిరిగి శూన్యసూచిక; /* no match, and none possible after s */
                    }
                    ఒకవేళ(c!=*q) {
                        ఆపు; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } లేకపోతే {
        స్థిరరాశి char16_t *హద్దు, *pre_హద్దు;

        /* subLength was decremented above */
        ఒకవేళ(length<=sub_నిడివి) {
            తిరిగి శూన్యసూచిక; /* s is shorter than sub */
        }

        హద్దు=s+length;

        /* the substring must start before preLimit */
        pre_హద్దు=హద్దు-sub_నిడివి;

        అయితే(s!=pre_హద్దు) {
            c=*s++;
            ఒకవేళ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                కొరకు(;;) {
                    ఒకవేళ(q==sub_హద్దు) {
                        ఒకవేళ(is_సరిపోలిక_At_CPBoundary(ప్రారంభం, s-1, p, హద్దు)) {
                            తిరిగి (char16_t *)(s-1); /* well-formed match */
                        } లేకపోతే {
                            ఆపు; /* no match because surrogate pair is split */
                        }
                    }
                    ఒకవేళ(*p!=*q) {
                        ఆపు; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    తిరిగి శూన్యసూచిక;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(స్థిరరాశి char16_t* s U_LIFETIME_BOUND, స్థిరరాశి char16_t* substring) {
    తిరిగి u_str_కనుగొను_మొదటి(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(స్థిరరాశి char16_t* s U_LIFETIME_BOUND, char16_t c) {
    ఒకవేళ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        తిరిగి u_str_కనుగొను_మొదటి(s, -1, &c, 1);
    } లేకపోతే {
        char16_t cs;

        /* trivial search for a BMP code point */
        కొరకు(;;) {
            ఒకవేళ((cs=*s)==c) {
                తిరిగి (char16_t *)s;
            }
            ఒకవేళ(cs==0) {
                తిరిగి శూన్యసూచిక;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(స్థిరరాశి char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    ఒకవేళ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        తిరిగి u_strchr(s, (char16_t)c);
    } లేకపోతే ఒకవేళ((uint32_t)c<=UCHAR_MAX_విలువ) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        అయితే((cs=*s++)!=0) {
            ఒకవేళ(cs==lead && *s==trail) {
                తిరిగి (char16_t *)(s-1);
            }
        }
        తిరిగి శూన్యసూచిక;
    } లేకపోతే {
        /* not a Unicode code point, not findable */
        తిరిగి శూన్యసూచిక;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(స్థిరరాశి char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ఒకవేళ(count<=0) {
        తిరిగి శూన్యసూచిక; /* no string */
    } లేకపోతే ఒకవేళ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        తిరిగి u_str_కనుగొను_మొదటి(s, count, &c, 1);
    } లేకపోతే {
        /* trivial search for a BMP code point */
        స్థిరరాశి char16_t *హద్దు=s+count;
        చేయి {
            ఒకవేళ(*s==c) {
                తిరిగి (char16_t *)s;
            }
        } అయితే(++s!=హద్దు);
        తిరిగి శూన్యసూచిక;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(స్థిరరాశి char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    ఒకవేళ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        తిరిగి u_memchr(s, (char16_t)c, count);
    } లేకపోతే ఒకవేళ(count<2) {
        /* too short for a surrogate pair */
        తిరిగి శూన్యసూచిక;
    } లేకపోతే ఒకవేళ((uint32_t)c<=UCHAR_MAX_విలువ) {
        /* find supplementary code point as surrogate pair */
        స్థిరరాశి char16_t *హద్దు=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        చేయి {
            ఒకవేళ(*s==lead && *(s+1)==trail) {
                తిరిగి (char16_t *)s;
            }
        } అయితే(++s!=హద్దు);
        తిరిగి శూన్యసూచిక;
    } లేకపోతే {
        /* not a Unicode code point, not findable */
        తిరిగి శూన్యసూచిక;
    }
}

/* Backward binary string search functions ---------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_str_కనుగొను_చివరి(స్థిరరాశి char16_t* s U_LIFETIME_BOUND,
              int32_t length,
              స్థిరరాశి char16_t* sub,
              int32_t sub_నిడివి) {
    స్థిరరాశి char16_t *ప్రారంభం, *హద్దు, *p, *q, *sub_హద్దు;
    char16_t c, cs;

    ఒకవేళ(sub==శూన్యసూచిక || sub_నిడివి<-1) {
        తిరిగి (char16_t *)s;
    }
    ఒకవేళ(s==శూన్యసూచిక || length<-1) {
        తిరిగి శూన్యసూచిక;
    }

    /*
     * This implementation is more lazy than the one for u_strFindFirst():
     * There is no special search code for NUL-terminated strings.
     * It does not seem to be worth it for searching substrings to
     * search forward and find all matches like in u_strrchr() and similar.
     * Therefore, we simply get both string lengths and search backward.
     *
     * markus 2002oct23
     */

    ఒకవేళ(sub_నిడివి<0) {
        sub_నిడివి=u_strlen(sub);
    }
    ఒకవేళ(sub_నిడివి==0) {
        తిరిగి (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    sub_హద్దు=sub+sub_నిడివి;
    cs=*(--sub_హద్దు);
    --sub_నిడివి;

    ఒకవేళ(sub_నిడివి==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        తిరిగి length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    ఒకవేళ(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    ఒకవేళ(length<=sub_నిడివి) {
        తిరిగి శూన్యసూచిక; /* s is shorter than sub */
    }

    ప్రారంభం=s;
    హద్దు=s+length;

    /* the substring must start no later than s+subLength */
    s+=sub_నిడివి;

    అయితే(s!=హద్దు) {
        c=*(--హద్దు);
        ఒకవేళ(c==cs) {
            /* found last substring char16_t, compare rest */
            p=హద్దు;
            q=sub_హద్దు;
            కొరకు(;;) {
                ఒకవేళ(q==sub) {
                    ఒకవేళ(is_సరిపోలిక_At_CPBoundary(ప్రారంభం, p, హద్దు+1, ప్రారంభం+length)) {
                        తిరిగి (char16_t *)p; /* well-formed match */
                    } లేకపోతే {
                        ఆపు; /* no match because surrogate pair is split */
                    }
                }
                ఒకవేళ(*(--p)!=*(--q)) {
                    ఆపు; /* no match */
                }
            }
        }
    }

    /* not found */
    తిరిగి శూన్యసూచిక;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(స్థిరరాశి char16_t* s U_LIFETIME_BOUND, స్థిరరాశి char16_t* substring) {
    తిరిగి u_str_కనుగొను_చివరి(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(స్థిరరాశి char16_t* s U_LIFETIME_BOUND, char16_t c) {
    ఒకవేళ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        తిరిగి u_str_కనుగొను_చివరి(s, -1, &c, 1);
    } లేకపోతే {
        స్థిరరాశి char16_t *ఫలితం=శూన్యసూచిక;
        char16_t cs;

        /* trivial search for a BMP code point */
        కొరకు(;;) {
            ఒకవేళ((cs=*s)==c) {
                ఫలితం=s;
            }
            ఒకవేళ(cs==0) {
                తిరిగి (char16_t *)ఫలితం;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(స్థిరరాశి char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    ఒకవేళ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        తిరిగి u_strrchr(s, (char16_t)c);
    } లేకపోతే ఒకవేళ((uint32_t)c<=UCHAR_MAX_విలువ) {
        /* find supplementary code point as surrogate pair */
        స్థిరరాశి char16_t *ఫలితం=శూన్యసూచిక;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        అయితే((cs=*s++)!=0) {
            ఒకవేళ(cs==lead && *s==trail) {
                ఫలితం=s-1;
            }
        }
        తిరిగి (char16_t *)ఫలితం;
    } లేకపోతే {
        /* not a Unicode code point, not findable */
        తిరిగి శూన్యసూచిక;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(స్థిరరాశి char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ఒకవేళ(count<=0) {
        తిరిగి శూన్యసూచిక; /* no string */
    } లేకపోతే ఒకవేళ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        తిరిగి u_str_కనుగొను_చివరి(s, count, &c, 1);
    } లేకపోతే {
        /* trivial search for a BMP code point */
        స్థిరరాశి char16_t *హద్దు=s+count;
        చేయి {
            ఒకవేళ(*(--హద్దు)==c) {
                తిరిగి (char16_t *)హద్దు;
            }
        } అయితే(s!=హద్దు);
        తిరిగి శూన్యసూచిక;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(స్థిరరాశి char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    ఒకవేళ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        తిరిగి u_memrchr(s, (char16_t)c, count);
    } లేకపోతే ఒకవేళ(count<2) {
        /* too short for a surrogate pair */
        తిరిగి శూన్యసూచిక;
    } లేకపోతే ఒకవేళ((uint32_t)c<=UCHAR_MAX_విలువ) {
        /* find supplementary code point as surrogate pair */
        స్థిరరాశి char16_t *హద్దు=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        చేయి {
            ఒకవేళ(*హద్దు==trail && *(హద్దు-1)==lead) {
                తిరిగి (char16_t *)(హద్దు-1);
            }
        } అయితే(s!=--హద్దు);
        తిరిగి శూన్యసూచిక;
    } లేకపోతే {
        /* not a Unicode code point, not findable */
        తిరిగి శూన్యసూచిక;
    }
}

/* Tokenization functions --------------------------------------------------- */

/*
 * Match each code point in a string against each code point in the matchSet.
 * Return the index of the first string code point that
 * is (polarity==true) or is not (false) contained in the matchSet.
 * Return -(string length)-1 if there is no such code point.
 */
స్థిర int32_t
సరిపోలిక_From_సమితి(స్థిరరాశి char16_t *string, స్థిరరాశి char16_t *సరిపోలిక_సమితి, UBool polarity) {
    int32_t సరిపోలిక_Len, సరిపోలిక_BMPLen, strItr, సరిపోలిక_Itr;
    UChar32 stringCh, సరిపోలిక_Ch;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    సరిపోలిక_BMPLen = 0;
    అయితే((c = సరిపోలిక_సమితి[సరిపోలిక_BMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++సరిపోలిక_BMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    సరిపోలిక_Len = సరిపోలిక_BMPLen;
    అయితే(సరిపోలిక_సమితి[సరిపోలిక_Len] != 0) {
        ++సరిపోలిక_Len;
    }

    కొరకు(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        ఒకవేళ(U16_IS_SINGLE(c)) {
            ఒకవేళ(polarity) {
                కొరకు(సరిపోలిక_Itr = 0; సరిపోలిక_Itr < సరిపోలిక_Len; ++సరిపోలిక_Itr) {
                    ఒకవేళ(c == సరిపోలిక_సమితి[సరిపోలిక_Itr]) {
                        తిరిగి strItr - 1; /* one matches */
                    }
                }
            } లేకపోతే {
                కొరకు(సరిపోలిక_Itr = 0; సరిపోలిక_Itr < సరిపోలిక_Len; ++సరిపోలిక_Itr) {
                    ఒకవేళ(c == సరిపోలిక_సమితి[సరిపోలిక_Itr]) {
                        వెళ్ళు endloop;
                    }
                }
                తిరిగి strItr - 1; /* none matches */
            }
        } లేకపోతే {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            ఒకవేళ(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } లేకపోతే {
                stringCh = c; /* unpaired trail surrogate */
            }

            ఒకవేళ(polarity) {
                కొరకు(సరిపోలిక_Itr = సరిపోలిక_BMPLen; సరిపోలిక_Itr < సరిపోలిక_Len;) {
                    U16_తదుపరి(సరిపోలిక_సమితి, సరిపోలిక_Itr, సరిపోలిక_Len, సరిపోలిక_Ch);
                    ఒకవేళ(stringCh == సరిపోలిక_Ch) {
                        తిరిగి strItr - U16_నిడివి(stringCh); /* one matches */
                    }
                }
            } లేకపోతే {
                కొరకు(సరిపోలిక_Itr = సరిపోలిక_BMPLen; సరిపోలిక_Itr < సరిపోలిక_Len;) {
                    U16_తదుపరి(సరిపోలిక_సమితి, సరిపోలిక_Itr, సరిపోలిక_Len, సరిపోలిక_Ch);
                    ఒకవేళ(stringCh == సరిపోలిక_Ch) {
                        వెళ్ళు endloop;
                    }
                }
                తిరిగి strItr - U16_నిడివి(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    తిరిగి -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(స్థిరరాశి char16_t* string U_LIFETIME_BOUND, స్థిరరాశి char16_t* సరిపోలిక_సమితి)
{
    int32_t idx = సరిపోలిక_From_సమితి(string, సరిపోలిక_సమితి, నిజం);
    ఒకవేళ(idx >= 0) {
        తిరిగి (char16_t *)string + idx;
    } లేకపోతే {
        తిరిగి శూన్యసూచిక;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(స్థిరరాశి char16_t *string, స్థిరరాశి char16_t *సరిపోలిక_సమితి)
{
    int32_t idx = సరిపోలిక_From_సమితి(string, సరిపోలిక_సమితి, నిజం);
    ఒకవేళ(idx >= 0) {
        తిరిగి idx;
    } లేకపోతే {
        తిరిగి -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(స్థిరరాశి char16_t *string, స్థిరరాశి char16_t *సరిపోలిక_సమితి)
{
    int32_t idx = సరిపోలిక_From_సమితి(string, సరిపోలిక_సమితి, అబద్ధం);
    ఒకవేళ(idx >= 0) {
        తిరిగి idx;
    } లేకపోతే {
        తిరిగి -idx - 1; /* == u_strlen(string) */
    }
}

/* ----- Text manipulation functions --- */

U_CAPI char16_t* U_EXPORT2
u_strtok_r(char16_t* src U_LIFETIME_BOUND, స్థిరరాశి char16_t* delim, char16_t** saveState)
{
    char16_t *tok_ఆధారం;
    char16_t *తదుపరి_Token;
    uint32_t nonDelimIdx;

    /* If saveState is nullptr, the user messed up. */
    ఒకవేళ (src != శూన్యసూచిక) {
        tok_ఆధారం = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    లేకపోతే ఒకవేళ (*saveState) {
        tok_ఆధారం = *saveState;
    }
    లేకపోతే {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        తిరిగి శూన్యసూచిక;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tok_ఆధారం, delim);
    tok_ఆధారం = &tok_ఆధారం[nonDelimIdx];

    ఒకవేళ (*tok_ఆధారం) {
        తదుపరి_Token = u_strpbrk(tok_ఆధారం, delim);
        ఒకవేళ (తదుపరి_Token != శూన్యసూచిక) {
            /* Create a token */
            *(తదుపరి_Token++) = 0;
            *saveState = తదుపరి_Token;
            తిరిగి tok_ఆధారం;
        }
        లేకపోతే ఒకవేళ (*saveState) {
            /* Return the last token */
            *saveState = శూన్యసూచిక;
            తిరిగి tok_ఆధారం;
        }
    }
    లేకపోతే {
        /* No tokens were found. Only delimiters were left. */
        *saveState = శూన్యసూచిక;
    }
    తిరిగి శూన్యసూచిక;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, స్థిరరాశి char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    అయితే(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    అయితే((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    తిరిగి anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, స్థిరరాశి char16_t* src, int32_t n)
{
    ఒకవేళ(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        అయితే(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        అయితే((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            ఒకవేళ(--n == 0) {
                *dst = 0;
                ఆపు;
            }
            ++src;
        }

        తిరిగి anchor;
    } లేకపోతే {
        తిరిగి dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(స్థిరరాశి char16_t *s1,
    స్థిరరాశి char16_t *s2)
{
    char16_t  c1, c2;

    కొరకు(;;) {
        c1=*s1++;
        c2=*s2++;
        ఒకవేళ (c1 != c2 || c1 == 0) {
            ఆపు;
        }
    }
    తిరిగి (int32_t)c1 - (int32_t)c2;
}

U_CFUNC int32_t U_EXPORT2
uprv_strCompare(స్థిరరాశి char16_t *s1, int32_t length1,
                స్థిరరాశి char16_t *s2, int32_t length2,
                UBool strncmpStyle, UBool సంకేతం_బిందువు_క్రమం) {
    స్థిరరాశి char16_t *start1, *start2, *limit1, *limit2;
    char16_t c1, c2;

    /* setup for fix-up */
    start1=s1;
    start2=s2;

    /* compare identical prefixes - they do not need to be fixed up */
    ఒకవేళ(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        ఒకవేళ(s1==s2) {
            తిరిగి 0;
        }

        కొరకు(;;) {
            c1=*s1;
            c2=*s2;
            ఒకవేళ(c1!=c2) {
                ఆపు;
            }
            ఒకవేళ(c1==0) {
                తిరిగి 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=శూన్యసూచిక;
    } లేకపోతే ఒకవేళ(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        ఒకవేళ(s1==s2) {
            తిరిగి 0;
        }

        limit1=start1+length1;

        కొరకు(;;) {
            /* both lengths are same, check only one limit */
            ఒకవేళ(s1==limit1) {
                తిరిగి 0;
            }

            c1=*s1;
            c2=*s2;
            ఒకవేళ(c1!=c2) {
                ఆపు;
            }
            ఒకవేళ(c1==0) {
                తిరిగి 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } లేకపోతే {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t నిడివి_ఫలితం;

        ఒకవేళ(length1<0) {
            length1=u_strlen(s1);
        }
        ఒకవేళ(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        ఒకవేళ(length1<length2) {
            నిడివి_ఫలితం=-1;
            limit1=start1+length1;
        } లేకపోతే ఒకవేళ(length1==length2) {
            నిడివి_ఫలితం=0;
            limit1=start1+length1;
        } లేకపోతే /* length1>length2 */ {
            నిడివి_ఫలితం=1;
            limit1=start1+length2;
        }

        ఒకవేళ(s1==s2) {
            తిరిగి నిడివి_ఫలితం;
        }

        కొరకు(;;) {
            /* check pseudo-limit */
            ఒకవేళ(s1==limit1) {
                తిరిగి నిడివి_ఫలితం;
            }

            c1=*s1;
            c2=*s2;
            ఒకవేళ(c1!=c2) {
                ఆపు;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    ఒకవేళ(c1>=0xd800 && c2>=0xd800 && సంకేతం_బిందువు_క్రమం) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        ఒకవేళ(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } లేకపోతే {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        ఒకవేళ(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } లేకపోతే {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    తిరిగి (int32_t)c1-(int32_t)c2;
}

/*
 * Compare two strings as presented by UCharIterators.
 * Use code unit or code point order.
 * When the function returns, it is undefined where the iterators
 * have stopped.
 */
U_CAPI int32_t U_EXPORT2
u_strCompareIter(UCharIterator *iter1, UCharIterator *iter2, UBool సంకేతం_బిందువు_క్రమం) {
    UChar32 c1, c2;

    /* argument checking */
    ఒకవేళ(iter1==శూన్యసూచిక || iter2==శూన్యసూచిక) {
        తిరిగి 0; /* bad arguments */
    }
    ఒకవేళ(iter1==iter2) {
        తిరిగి 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_ప్రారంభం);
    iter2->move(iter2, 0, UITER_ప్రారంభం);

    /* compare identical prefixes - they do not need to be fixed up */
    కొరకు(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        ఒకవేళ(c1!=c2) {
            ఆపు;
        }
        ఒకవేళ(c1==-1) {
            తిరిగి 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    ఒకవేళ(c1>=0xd800 && c2>=0xd800 && సంకేతం_బిందువు_క్రమం) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        ఒకవేళ(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->మునుపటి(iter1), U16_IS_LEAD(iter1->మునుపటి(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } లేకపోతే {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        ఒకవేళ(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->మునుపటి(iter2), U16_IS_LEAD(iter2->మునుపటి(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } లేకపోతే {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    తిరిగి (int32_t)c1-(int32_t)c2;
}

#ఒకవేళ 0
/*
 * u_strCompareIter() does not leave the iterators _on_ the different units.
 * This is possible but would cost a few extra indirect function calls to back
 * up if the last unit (c1 or c2 respectively) was >=0.
 *
 * Consistently leaving them _behind_ the different units is not an option
 * because the current "unit" is the end of the string if that is reached,
 * and in such a case the iterator does not move.
 * For example, when comparing "ab" with "abc", both iterators rest _on_ the end
 * of their strings. Calling previous() on each does not move them to where
 * the comparison fails.
 *
 * So the simplest semantics is to not define where the iterators end up.
 *
 * The following fragment is part of what would need to be done for backing up.
 */
శూన్య fragment {
        /* iff a surrogate is part of a surrogate pair, leave >=d800 */
        ఒకవేళ(c1<=0xdbff) {
            ఒకవేళ(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } లేకపోతే ఒకవేళ(c1<=0xdfff) {
            int32_t idx=iter1->get_సూచీ(iter1, UITER_CURRENT);
            iter1->మునుపటి(iter1); /* ==c1 */
            ఒకవేళ(!U16_IS_LEAD(iter1->మునుపటి(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } లేకపోతే /* 0xe000<=c1<=0xffff */ {
            /* BMP code point - make <d800 */
            c1-=0x2800;
        }
}
#అయితే_ముగింపు

U_CAPI int32_t U_EXPORT2
u_strCompare(స్థిరరాశి char16_t *s1, int32_t length1,
             స్థిరరాశి char16_t *s2, int32_t length2,
             UBool సంకేతం_బిందువు_క్రమం) {
    /* argument checking */
    ఒకవేళ(s1==శూన్యసూచిక || length1<-1 || s2==శూన్యసూచిక || length2<-1) {
        తిరిగి 0;
    }
    తిరిగి uprv_strCompare(s1, length1, s2, length2, అబద్ధం, సంకేతం_బిందువు_క్రమం);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmp_సంకేతం_బిందువు_క్రమం(స్థిరరాశి char16_t *s1, స్థిరరాశి char16_t *s2) {
    తిరిగి uprv_strCompare(s1, -1, s2, -1, అబద్ధం, నిజం);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(స్థిరరాశి char16_t  *s1,
     స్థిరరాశి char16_t  *s2,
     int32_t     n) 
{
    ఒకవేళ(n > 0) {
        int32_t rc;
        కొరకు(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            ఒకవేళ(rc != 0 || *s1 == 0 || --n == 0) {
                తిరిగి rc;
            }
            ++s1;
            ++s2;
        }
    } లేకపోతే {
        తిరిగి 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmp_సంకేతం_బిందువు_క్రమం(స్థిరరాశి char16_t *s1, స్థిరరాశి char16_t *s2, int32_t n) {
    తిరిగి uprv_strCompare(s1, n, s2, n, నిజం, నిజం);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, స్థిరరాశి char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    అయితే((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    తిరిగి anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, స్థిరరాశి char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    అయితే(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    తిరిగి anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(స్థిరరాశి char16_t *s)
{
#ఒకవేళ U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    తిరిగి (int32_t)uprv_wcslen((స్థిరరాశి wchar_t *)s);
#లేకపోతే
    స్థిరరాశి char16_t *t = s;
    అయితే(*t != 0) {
      ++t;
    }
    తిరిగి t - s;
#అయితే_ముగింపు
}

U_CAPI int32_t U_EXPORT2
u_లెక్క_Char32(స్థిరరాశి char16_t *s, int32_t length) {
    int32_t count;

    ఒకవేళ(s==శూన్యసూచిక || length<-1) {
        తిరిగి 0;
    }

    count=0;
    ఒకవేళ(length>=0) {
        అయితే(length>0) {
            ++count;
            ఒకవేళ(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } లేకపోతే {
                ++s;
                --length;
            }
        }
    } లేకపోతే /* length==-1 */ {
        char16_t c;

        కొరకు(;;) {
            ఒకవేళ((c=*s++)==0) {
                ఆపు;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            ఒకవేళ(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    తిరిగి count;
}

U_CAPI UBool U_EXPORT2
u_str_Has_మరిన్ని_Char32_Than(స్థిరరాశి char16_t *s, int32_t length, int32_t number) {

    ఒకవేళ(number<0) {
        తిరిగి నిజం;
    }
    ఒకవేళ(s==శూన్యసూచిక || length<-1) {
        తిరిగి అబద్ధం;
    }

    ఒకవేళ(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        కొరకు(;;) {
            ఒకవేళ((c=*s++)==0) {
                తిరిగి అబద్ధం;
            }
            ఒకవేళ(number==0) {
                తిరిగి నిజం;
            }
            ఒకవేళ(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } లేకపోతే {
        /* length>=0 known */
        స్థిరరాశి char16_t *హద్దు;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        ఒకవేళ(((length+1)/2)>number) {
            తిరిగి నిజం;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        ఒకవేళ(maxSupplementary<=0) {
            తిరిగి అబద్ధం;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        హద్దు=s+length;
        కొరకు(;;) {
            ఒకవేళ(s==హద్దు) {
                తిరిగి అబద్ధం;
            }
            ఒకవేళ(number==0) {
                తిరిగి నిజం;
            }
            ఒకవేళ(U16_IS_LEAD(*s++) && s!=హద్దు && U16_IS_TRAIL(*s)) {
                ++s;
                ఒకవేళ(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    తిరిగి అబద్ధం;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, స్థిరరాశి char16_t* src, int32_t count) {
    ఒకవేళ(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    తిరిగి dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, స్థిరరాశి char16_t* src, int32_t count) {
    ఒకవేళ(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    తిరిగి dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ఒకవేళ(count > 0) {
        char16_t *ptr = dest;
        char16_t *హద్దు = dest + count;

        అయితే (ptr < హద్దు) {
            *(ptr++) = c;
        }
    }
    తిరిగి dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(స్థిరరాశి char16_t *buf1, స్థిరరాశి char16_t *buf2, int32_t count) {
    ఒకవేళ(count > 0) {
        స్థిరరాశి char16_t *హద్దు = buf1 + count;
        int32_t ఫలితం;

        అయితే (buf1 < హద్దు) {
            ఫలితం = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            ఒకవేళ (ఫలితం != 0) {
                తిరిగి ఫలితం;
            }
            buf1++;
            buf2++;
        }
    }
    తిరిగి 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmp_సంకేతం_బిందువు_క్రమం(స్థిరరాశి char16_t *s1, స్థిరరాశి char16_t *s2, int32_t count) {
    తిరిగి uprv_strCompare(s1, count, s2, count, అబద్ధం, నిజం);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
స్థిర స్థిరరాశి char16_t UNESCAPE_MAP[] = {
    /*"   0x22, 0x22 */
    /*'   0x27, 0x27 */
    /*?   0x3F, 0x3F */
    /*\   0x5C, 0x5C */
    /*a*/ 0x61, 0x07,
    /*b*/ 0x62, 0x08,
    /*e*/ 0x65, 0x1b,
    /*f*/ 0x66, 0x0c,
    /*n*/ 0x6E, 0x0a,
    /*r*/ 0x72, 0x0d,
    /*t*/ 0x74, 0x09,
    /*v*/ 0x76, 0x0b
};
గణన { UNESCAPE_MAP_నిడివి = UPRV_LENGTHOF(UNESCAPE_MAP) };

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
స్థిర int32_t _digit8(char16_t c) {
    ఒకవేళ (c >= u'0' && c <= u'7') {
        తిరిగి c - u'0';
    }
    తిరిగి -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
స్థిర int32_t _digit16(char16_t c) {
    ఒకవేళ (c >= u'0' && c <= u'9') {
        తిరిగి c - u'0';
    }
    ఒకవేళ (c >= u'A' && c <= u'F') {
        తిరిగి c - (u'A' - 10);
    }
    ఒకవేళ (c >= u'a' && c <= u'f') {
        తిరిగి c - (u'a' - 10);
    }
    తిరిగి -1;
}

/* Parse a single escape sequence.  Although this method deals in
 * UChars, it does not use C++ or UnicodeString.  This allows it to
 * be used from C contexts. */
U_CAPI UChar32 U_EXPORT2
u_unescapeAt(UNESCAPE_CHAR_AT charAt,
             int32_t *స్థానభ్రంశం,
             int32_t length,
             శూన్య *context) {

    int32_t ప్రారంభం = *స్థానభ్రంశం;
    UChar32 c;
    UChar32 ఫలితం = 0;
    int8_t n = 0;
    int8_t minDig = 0;
    int8_t maxDig = 0;
    int8_t బిట్లు_చొప్పున_Digit = 4; 
    int32_t dig;
    UBool braces = అబద్ధం;

    /* Check that offset is in range */
    ఒకవేళ (*స్థానభ్రంశం < 0 || *స్థానభ్రంశం >= length) {
        వెళ్ళు err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*స్థానభ్రంశం)++, context);

    /* Convert hexadecimal and octal escapes */
    ఎంపిక (c) {
    స్థితి u'u':
        minDig = maxDig = 4;
        ఆపు;
    స్థితి u'U':
        minDig = maxDig = 8;
        ఆపు;
    స్థితి u'x':
        minDig = 1;
        ఒకవేళ (*స్థానభ్రంశం < length && charAt(*స్థానభ్రంశం, context) == u'{') {
            ++(*స్థానభ్రంశం);
            braces = నిజం;
            maxDig = 8;
        } లేకపోతే {
            maxDig = 2;
        }
        ఆపు;
    అప్రమేయ:
        dig = _digit8(c);
        ఒకవేళ (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            బిట్లు_చొప్పున_Digit = 3;
            ఫలితం = dig;
        }
        ఆపు;
    }
    ఒకవేళ (minDig != 0) {
        అయితే (*స్థానభ్రంశం < length && n < maxDig) {
            c = charAt(*స్థానభ్రంశం, context);
            dig = (బిట్లు_చొప్పున_Digit == 3) ? _digit8(c) : _digit16(c);
            ఒకవేళ (dig < 0) {
                ఆపు;
            }
            ఫలితం = (ఫలితం << బిట్లు_చొప్పున_Digit) | dig;
            ++(*స్థానభ్రంశం);
            ++n;
        }
        ఒకవేళ (n < minDig) {
            వెళ్ళు err;
        }
        ఒకవేళ (braces) {
            ఒకవేళ (c != u'}') {
                వెళ్ళు err;
            }
            ++(*స్థానభ్రంశం);
        }
        ఒకవేళ (ఫలితం < 0 || ఫలితం >= 0x110000) {
            వెళ్ళు err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        ఒకవేళ (*స్థానభ్రంశం < length && U16_IS_LEAD(ఫలితం)) {
            int32_t ahead = *స్థానభ్రంశం + 1;
            c = charAt(*స్థానభ్రంశం, context);
            ఒకవేళ (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t తోక_హద్దు = ahead + 11;
                ఒకవేళ (తోక_హద్దు > length) {
                    తోక_హద్దు = length;
                }
                c = u_unescapeAt(charAt, &ahead, తోక_హద్దు, context);
            }
            ఒకవేళ (U16_IS_TRAIL(c)) {
                *స్థానభ్రంశం = ahead;
                ఫలితం = U16_GET_SUPPLEMENTARY(ఫలితం, c);
            }
        }
        తిరిగి ఫలితం;
    }

    /* Convert C-style escapes in table */
    కొరకు (int32_t i=0; i<UNESCAPE_MAP_నిడివి; i+=2) {
        ఒకవేళ (c == UNESCAPE_MAP[i]) {
            తిరిగి UNESCAPE_MAP[i+1];
        } లేకపోతే ఒకవేళ (c < UNESCAPE_MAP[i]) {
            ఆపు;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    ఒకవేళ (c == u'c' && *స్థానభ్రంశం < length) {
        c = charAt((*స్థానభ్రంశం)++, context);
        ఒకవేళ (U16_IS_LEAD(c) && *స్థానభ్రంశం < length) {
            char16_t c2 = charAt(*స్థానభ్రంశం, context);
            ఒకవేళ (U16_IS_TRAIL(c2)) {
                ++(*స్థానభ్రంశం);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        తిరిగి 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    ఒకవేళ (U16_IS_LEAD(c) && *స్థానభ్రంశం < length) {
        char16_t c2 = charAt(*స్థానభ్రంశం, context);
        ఒకవేళ (U16_IS_TRAIL(c2)) {
            ++(*స్థానభ్రంశం);
            తిరిగి U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    తిరిగి c;

 err:
    /* Invalid escape sequence */
    *స్థానభ్రంశం = ప్రారంభం; /* Reset to initial value */
    తిరిగి (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
స్థిర char16_t U_CALLCONV
_charPtr_charAt(int32_t స్థానభ్రంశం, శూన్య *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_chars_గమ్యం_UChars(static_cast<అక్షరం*>(context) + స్థానభ్రంశం, &c16, 1);
    తిరిగి c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
స్థిర శూన్య _appendUChars(char16_t *dest, int32_t dest_సామర్థ్యం,
                          స్థిరరాశి అక్షరం *src, int32_t srcLen) {
    ఒకవేళ (dest_సామర్థ్యం < 0) {
        dest_సామర్థ్యం = 0;
    }
    ఒకవేళ (srcLen > dest_సామర్థ్యం) {
        srcLen = dest_సామర్థ్యం;
    }
    u_chars_గమ్యం_UChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(స్థిరరాశి అక్షరం *src, char16_t *dest, int32_t dest_సామర్థ్యం) {
    స్థిరరాశి అక్షరం *segment = src;
    int32_t i = 0;
    అక్షరం c;

    అయితే ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        ఒకవేళ (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            ఒకవేళ (src != segment) {
                ఒకవేళ (dest != శూన్యసూచిక) {
                    _appendUChars(dest + i, dest_సామర్థ్యం - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<అక్షరం*>(src));
            ఒకవేళ (lenParsed == 0) {
                వెళ్ళు err;
            }
            src += lenParsed; /* advance past escape seq. */
            ఒకవేళ (dest != శూన్యసూచిక && U16_నిడివి(c32) <= (dest_సామర్థ్యం - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } లేకపోతే {
                i += U16_నిడివి(c32);
            }
            segment = src;
        } లేకపోతే {
            ++src;
        }
    }
    ఒకవేళ (src != segment) {
        ఒకవేళ (dest != శూన్యసూచిక) {
            _appendUChars(dest + i, dest_సామర్థ్యం - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    ఒకవేళ (dest != శూన్యసూచిక && i < dest_సామర్థ్యం) {
        dest[i] = 0;
    }
    తిరిగి i;

 err:
    ఒకవేళ (dest != శూన్యసూచిక && dest_సామర్థ్యం > 0) {
        *dest = 0;
    }
    తిరిగి 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#నిర్వచించు __TERMINATE_STRING(dest, dest_సామర్థ్యం, length, p_Error_సంకేతం) UPRV_ఖండం_MACRO_BEGIN { \
    ఒకవేళ(p_Error_సంకేతం!=శూన్యసూచిక && U_SUCCESS(*p_Error_సంకేతం)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        ఒకవేళ(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } లేకపోతే ఒకవేళ(length<dest_సామర్థ్యం) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            ఒకవేళ(*p_Error_సంకేతం==U_STRING_NOT_TERMINATED_WARNING) {          \
                *p_Error_సంకేతం=U_ZERO_ERROR;                               \
            }                                                           \
        } లేకపోతే ఒకవేళ(length==dest_సామర్థ్యం) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *p_Error_సంకేతం=U_STRING_NOT_TERMINATED_WARNING;                \
        } లేకపోతే /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;                        \
        }                                                               \
    } \
} UPRV_ఖండం_MACRO_చివర

U_CAPI char16_t U_EXPORT2
u_ascii_గమ్యం_పై(char16_t c) {
    ఒకవేళ (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    తిరిగి c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t dest_సామర్థ్యం, int32_t length, UError_సంకేతం *p_Error_సంకేతం) {
    __TERMINATE_STRING(dest, dest_సామర్థ్యం, length, p_Error_సంకేతం);
    తిరిగి length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(అక్షరం *dest, int32_t dest_సామర్థ్యం, int32_t length, UError_సంకేతం *p_Error_సంకేతం) {
    __TERMINATE_STRING(dest, dest_సామర్థ్యం, length, p_Error_సంకేతం);
    తిరిగి length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t dest_సామర్థ్యం, int32_t length, UError_సంకేతం *p_Error_సంకేతం) {
    __TERMINATE_STRING(dest, dest_సామర్థ్యం, length, p_Error_సంకేతం);
    తిరిగి length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t dest_సామర్థ్యం, int32_t length, UError_సంకేతం *p_Error_సంకేతం) {
    __TERMINATE_STRING(dest, dest_సామర్థ్యం, length, p_Error_సంకేతం);
    తిరిగి length;
}

// Compute the hash code for a string -------------------------------------- ***

// Moved here from uhash.c so that UnicodeString::hashCode() does not depend
// on UHashtable code.

/*
  Compute the hash by iterating sparsely over about 32 (up to 63)
  characters spaced evenly through the string.  For each character,
  multiply the previous hash value by a prime number and add the new
  character in, like a linear congruential random number generator,
  producing a pseudorandom deterministic value well distributed over
  the output range. [LIU]
*/

#నిర్వచించు STRING_HASH(TYPE, STR, STRLEN, DEREF) UPRV_ఖండం_MACRO_BEGIN { \
    uint32_t hash = 0;                        \
    స్థిరరాశి TYPE *p = (స్థిరరాశి TYPE*) STR;        \
    ఒకవేళ (p != శూన్యసూచిక) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        స్థిరరాశి TYPE *హద్దు = p + len;          \
        అయితే (p<హద్దు) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    తిరిగి static_cast<int32_t>(hash);        \
} UPRV_ఖండం_MACRO_చివర

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(స్థిరరాశి char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(స్థిరరాశి అక్షరం *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(స్థిరరాశి అక్షరం *str, int32_t length) {
    STRING_HASH(అక్షరం, str, length, (uint8_t)uprv_tolower(*p));
}
