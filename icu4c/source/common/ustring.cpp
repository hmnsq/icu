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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cwchar.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ustr_imp.h"

/* ANSI string.h - style functions ------------------------------------------ */

/* U+ffff is the highest BMP code point, the highest one that fits into a 16-bit char16_t */
#define U_BMP_MAX 0xffff

/* Forward binary string search functions ----------------------------------- */

/*
 * Test if a substring match inside a string is at code point boundaries.
 * All pointers refer to the same buffer.
 * The limit pointer may be nullptr, all others must be real pointers.
 */
स्थिर ओळीत UBool
isMatchAtCPBoundary(स्थिरांक char16_t *start, स्थिरांक char16_t *match, स्थिरांक char16_t *matchLimit, स्थिरांक char16_t *limit) {
    जर(U16_IS_TRAIL(*match) && start!=match && U16_IS_LEAD(*(match-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        परत असत्य;
    }
    जर(U16_IS_LEAD(*(matchLimit-1)) && matchLimit!=limit && U16_IS_TRAIL(*matchLimit)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        परत असत्य;
    }
    परत सत्य;
}

U_CAPI char16_t* U_EXPORT2
u_strFindFirst(स्थिरांक char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               स्थिरांक char16_t* sub,
               int32_t subLength) {
    स्थिरांक char16_t *start, *p, *q, *subLimit;
    char16_t c, cs, cq;

    जर(sub==nullptr || subLength<-1) {
        परत (char16_t *)s;
    }
    जर(s==nullptr || length<-1) {
        परत nullptr;
    }

    start=s;

    जर(length<0 && subLength<0) {
        /* both strings are NUL-terminated */
        जर((cs=*sub++)==0) {
            परत (char16_t *)s;
        }
        जर(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            परत u_strchr(s, cs);
        }

        पर्यंत((c=*s++)!=0) {
            जर(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                साठी(;;) {
                    जर((cq=*q)==0) {
                        जर(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            परत (char16_t *)(s-1); /* well-formed match */
                        } अन्यथा {
                            थांबा; /* no match because surrogate pair is split */
                        }
                    }
                    जर((c=*p)==0) {
                        परत nullptr; /* no match, and none possible after s */
                    }
                    जर(c!=cq) {
                        थांबा; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        परत nullptr;
    }

    जर(subLength<0) {
        subLength=u_strlen(sub);
    }
    जर(subLength==0) {
        परत (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --subLength;
    subLimit=sub+subLength;

    जर(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        परत length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    जर(length<0) {
        /* s is NUL-terminated */
        पर्यंत((c=*s++)!=0) {
            जर(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                साठी(;;) {
                    जर(q==subLimit) {
                        जर(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            परत (char16_t *)(s-1); /* well-formed match */
                        } अन्यथा {
                            थांबा; /* no match because surrogate pair is split */
                        }
                    }
                    जर((c=*p)==0) {
                        परत nullptr; /* no match, and none possible after s */
                    }
                    जर(c!=*q) {
                        थांबा; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } अन्यथा {
        स्थिरांक char16_t *limit, *preLimit;

        /* subLength was decremented above */
        जर(length<=subLength) {
            परत nullptr; /* s is shorter than sub */
        }

        limit=s+length;

        /* the substring must start before preLimit */
        preLimit=limit-subLength;

        पर्यंत(s!=preLimit) {
            c=*s++;
            जर(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                साठी(;;) {
                    जर(q==subLimit) {
                        जर(isMatchAtCPBoundary(start, s-1, p, limit)) {
                            परत (char16_t *)(s-1); /* well-formed match */
                        } अन्यथा {
                            थांबा; /* no match because surrogate pair is split */
                        }
                    }
                    जर(*p!=*q) {
                        थांबा; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    परत nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(स्थिरांक char16_t* s U_LIFETIME_BOUND, स्थिरांक char16_t* substring) {
    परत u_strFindFirst(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(स्थिरांक char16_t* s U_LIFETIME_BOUND, char16_t c) {
    जर(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        परत u_strFindFirst(s, -1, &c, 1);
    } अन्यथा {
        char16_t cs;

        /* trivial search for a BMP code point */
        साठी(;;) {
            जर((cs=*s)==c) {
                परत (char16_t *)s;
            }
            जर(cs==0) {
                परत nullptr;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(स्थिरांक char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    जर((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        परत u_strchr(s, (char16_t)c);
    } अन्यथा जर((uint32_t)c<=UCHAR_MAX_मूल्य) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        पर्यंत((cs=*s++)!=0) {
            जर(cs==lead && *s==trail) {
                परत (char16_t *)(s-1);
            }
        }
        परत nullptr;
    } अन्यथा {
        /* not a Unicode code point, not findable */
        परत nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(स्थिरांक char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    जर(count<=0) {
        परत nullptr; /* no string */
    } अन्यथा जर(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        परत u_strFindFirst(s, count, &c, 1);
    } अन्यथा {
        /* trivial search for a BMP code point */
        स्थिरांक char16_t *limit=s+count;
        करा {
            जर(*s==c) {
                परत (char16_t *)s;
            }
        } पर्यंत(++s!=limit);
        परत nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(स्थिरांक char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    जर((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        परत u_memchr(s, (char16_t)c, count);
    } अन्यथा जर(count<2) {
        /* too short for a surrogate pair */
        परत nullptr;
    } अन्यथा जर((uint32_t)c<=UCHAR_MAX_मूल्य) {
        /* find supplementary code point as surrogate pair */
        स्थिरांक char16_t *limit=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        करा {
            जर(*s==lead && *(s+1)==trail) {
                परत (char16_t *)s;
            }
        } पर्यंत(++s!=limit);
        परत nullptr;
    } अन्यथा {
        /* not a Unicode code point, not findable */
        परत nullptr;
    }
}

/* Backward binary string search functions ---------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strFindLast(स्थिरांक char16_t* s U_LIFETIME_BOUND,
              int32_t length,
              स्थिरांक char16_t* sub,
              int32_t subLength) {
    स्थिरांक char16_t *start, *limit, *p, *q, *subLimit;
    char16_t c, cs;

    जर(sub==nullptr || subLength<-1) {
        परत (char16_t *)s;
    }
    जर(s==nullptr || length<-1) {
        परत nullptr;
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

    जर(subLength<0) {
        subLength=u_strlen(sub);
    }
    जर(subLength==0) {
        परत (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    subLimit=sub+subLength;
    cs=*(--subLimit);
    --subLength;

    जर(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        परत length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    जर(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    जर(length<=subLength) {
        परत nullptr; /* s is shorter than sub */
    }

    start=s;
    limit=s+length;

    /* the substring must start no later than s+subLength */
    s+=subLength;

    पर्यंत(s!=limit) {
        c=*(--limit);
        जर(c==cs) {
            /* found last substring char16_t, compare rest */
            p=limit;
            q=subLimit;
            साठी(;;) {
                जर(q==sub) {
                    जर(isMatchAtCPBoundary(start, p, limit+1, start+length)) {
                        परत (char16_t *)p; /* well-formed match */
                    } अन्यथा {
                        थांबा; /* no match because surrogate pair is split */
                    }
                }
                जर(*(--p)!=*(--q)) {
                    थांबा; /* no match */
                }
            }
        }
    }

    /* not found */
    परत nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(स्थिरांक char16_t* s U_LIFETIME_BOUND, स्थिरांक char16_t* substring) {
    परत u_strFindLast(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(स्थिरांक char16_t* s U_LIFETIME_BOUND, char16_t c) {
    जर(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        परत u_strFindLast(s, -1, &c, 1);
    } अन्यथा {
        स्थिरांक char16_t *result=nullptr;
        char16_t cs;

        /* trivial search for a BMP code point */
        साठी(;;) {
            जर((cs=*s)==c) {
                result=s;
            }
            जर(cs==0) {
                परत (char16_t *)result;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(स्थिरांक char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    जर((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        परत u_strrchr(s, (char16_t)c);
    } अन्यथा जर((uint32_t)c<=UCHAR_MAX_मूल्य) {
        /* find supplementary code point as surrogate pair */
        स्थिरांक char16_t *result=nullptr;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        पर्यंत((cs=*s++)!=0) {
            जर(cs==lead && *s==trail) {
                result=s-1;
            }
        }
        परत (char16_t *)result;
    } अन्यथा {
        /* not a Unicode code point, not findable */
        परत nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(स्थिरांक char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    जर(count<=0) {
        परत nullptr; /* no string */
    } अन्यथा जर(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        परत u_strFindLast(s, count, &c, 1);
    } अन्यथा {
        /* trivial search for a BMP code point */
        स्थिरांक char16_t *limit=s+count;
        करा {
            जर(*(--limit)==c) {
                परत (char16_t *)limit;
            }
        } पर्यंत(s!=limit);
        परत nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(स्थिरांक char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    जर((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        परत u_memrchr(s, (char16_t)c, count);
    } अन्यथा जर(count<2) {
        /* too short for a surrogate pair */
        परत nullptr;
    } अन्यथा जर((uint32_t)c<=UCHAR_MAX_मूल्य) {
        /* find supplementary code point as surrogate pair */
        स्थिरांक char16_t *limit=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        करा {
            जर(*limit==trail && *(limit-1)==lead) {
                परत (char16_t *)(limit-1);
            }
        } पर्यंत(s!=--limit);
        परत nullptr;
    } अन्यथा {
        /* not a Unicode code point, not findable */
        परत nullptr;
    }
}

/* Tokenization functions --------------------------------------------------- */

/*
 * Match each code point in a string against each code point in the matchSet.
 * Return the index of the first string code point that
 * is (polarity==true) or is not (false) contained in the matchSet.
 * Return -(string length)-1 if there is no such code point.
 */
स्थिर int32_t
_matchFromSet(स्थिरांक char16_t *string, स्थिरांक char16_t *matchSet, UBool polarity) {
    int32_t matchLen, matchBMPLen, strItr, matchItr;
    UChar32 stringCh, matchCh;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    matchBMPLen = 0;
    पर्यंत((c = matchSet[matchBMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++matchBMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    matchLen = matchBMPLen;
    पर्यंत(matchSet[matchLen] != 0) {
        ++matchLen;
    }

    साठी(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        जर(U16_IS_SINGLE(c)) {
            जर(polarity) {
                साठी(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    जर(c == matchSet[matchItr]) {
                        परत strItr - 1; /* one matches */
                    }
                }
            } अन्यथा {
                साठी(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    जर(c == matchSet[matchItr]) {
                        जा endloop;
                    }
                }
                परत strItr - 1; /* none matches */
            }
        } अन्यथा {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            जर(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } अन्यथा {
                stringCh = c; /* unpaired trail surrogate */
            }

            जर(polarity) {
                साठी(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    जर(stringCh == matchCh) {
                        परत strItr - U16_LENGTH(stringCh); /* one matches */
                    }
                }
            } अन्यथा {
                साठी(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    जर(stringCh == matchCh) {
                        जा endloop;
                    }
                }
                परत strItr - U16_LENGTH(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    परत -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(स्थिरांक char16_t* string U_LIFETIME_BOUND, स्थिरांक char16_t* matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, सत्य);
    जर(idx >= 0) {
        परत (char16_t *)string + idx;
    } अन्यथा {
        परत nullptr;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(स्थिरांक char16_t *string, स्थिरांक char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, सत्य);
    जर(idx >= 0) {
        परत idx;
    } अन्यथा {
        परत -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(स्थिरांक char16_t *string, स्थिरांक char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, असत्य);
    जर(idx >= 0) {
        परत idx;
    } अन्यथा {
        परत -idx - 1; /* == u_strlen(string) */
    }
}

/* ----- Text manipulation functions --- */

U_CAPI char16_t* U_EXPORT2
u_strtok_r(char16_t* src U_LIFETIME_BOUND, स्थिरांक char16_t* delim, char16_t** saveState)
{
    char16_t *tokSource;
    char16_t *nextToken;
    uint32_t nonDelimIdx;

    /* If saveState is nullptr, the user messed up. */
    जर (src != nullptr) {
        tokSource = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    अन्यथा जर (*saveState) {
        tokSource = *saveState;
    }
    अन्यथा {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        परत nullptr;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tokSource, delim);
    tokSource = &tokSource[nonDelimIdx];

    जर (*tokSource) {
        nextToken = u_strpbrk(tokSource, delim);
        जर (nextToken != nullptr) {
            /* Create a token */
            *(nextToken++) = 0;
            *saveState = nextToken;
            परत tokSource;
        }
        अन्यथा जर (*saveState) {
            /* Return the last token */
            *saveState = nullptr;
            परत tokSource;
        }
    }
    अन्यथा {
        /* No tokens were found. Only delimiters were left. */
        *saveState = nullptr;
    }
    परत nullptr;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, स्थिरांक char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    पर्यंत(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    पर्यंत((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    परत anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, स्थिरांक char16_t* src, int32_t n)
{
    जर(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        पर्यंत(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        पर्यंत((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            जर(--n == 0) {
                *dst = 0;
                थांबा;
            }
            ++src;
        }

        परत anchor;
    } अन्यथा {
        परत dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(स्थिरांक char16_t *s1,
    स्थिरांक char16_t *s2)
{
    char16_t  c1, c2;

    साठी(;;) {
        c1=*s1++;
        c2=*s2++;
        जर (c1 != c2 || c1 == 0) {
            थांबा;
        }
    }
    परत (int32_t)c1 - (int32_t)c2;
}

U_CFUNC int32_t U_EXPORT2
uprv_strCompare(स्थिरांक char16_t *s1, int32_t length1,
                स्थिरांक char16_t *s2, int32_t length2,
                UBool strncmpStyle, UBool codePointOrder) {
    स्थिरांक char16_t *start1, *start2, *limit1, *limit2;
    char16_t c1, c2;

    /* setup for fix-up */
    start1=s1;
    start2=s2;

    /* compare identical prefixes - they do not need to be fixed up */
    जर(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        जर(s1==s2) {
            परत 0;
        }

        साठी(;;) {
            c1=*s1;
            c2=*s2;
            जर(c1!=c2) {
                थांबा;
            }
            जर(c1==0) {
                परत 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=nullptr;
    } अन्यथा जर(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        जर(s1==s2) {
            परत 0;
        }

        limit1=start1+length1;

        साठी(;;) {
            /* both lengths are same, check only one limit */
            जर(s1==limit1) {
                परत 0;
            }

            c1=*s1;
            c2=*s2;
            जर(c1!=c2) {
                थांबा;
            }
            जर(c1==0) {
                परत 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } अन्यथा {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t lengthResult;

        जर(length1<0) {
            length1=u_strlen(s1);
        }
        जर(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        जर(length1<length2) {
            lengthResult=-1;
            limit1=start1+length1;
        } अन्यथा जर(length1==length2) {
            lengthResult=0;
            limit1=start1+length1;
        } अन्यथा /* length1>length2 */ {
            lengthResult=1;
            limit1=start1+length2;
        }

        जर(s1==s2) {
            परत lengthResult;
        }

        साठी(;;) {
            /* check pseudo-limit */
            जर(s1==limit1) {
                परत lengthResult;
            }

            c1=*s1;
            c2=*s2;
            जर(c1!=c2) {
                थांबा;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    जर(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        जर(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } अन्यथा {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        जर(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } अन्यथा {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    परत (int32_t)c1-(int32_t)c2;
}

/*
 * Compare two strings as presented by UCharIterators.
 * Use code unit or code point order.
 * When the function returns, it is undefined where the iterators
 * have stopped.
 */
U_CAPI int32_t U_EXPORT2
u_strCompareIter(UCharIterator *iter1, UCharIterator *iter2, UBool codePointOrder) {
    UChar32 c1, c2;

    /* argument checking */
    जर(iter1==nullptr || iter2==nullptr) {
        परत 0; /* bad arguments */
    }
    जर(iter1==iter2) {
        परत 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_START);
    iter2->move(iter2, 0, UITER_START);

    /* compare identical prefixes - they do not need to be fixed up */
    साठी(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        जर(c1!=c2) {
            थांबा;
        }
        जर(c1==-1) {
            परत 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    जर(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        जर(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->previous(iter1), U16_IS_LEAD(iter1->previous(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } अन्यथा {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        जर(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->previous(iter2), U16_IS_LEAD(iter2->previous(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } अन्यथा {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    परत (int32_t)c1-(int32_t)c2;
}

#जर 0
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
शून्य fragment {
        /* iff a surrogate is part of a surrogate pair, leave >=d800 */
        जर(c1<=0xdbff) {
            जर(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } अन्यथा जर(c1<=0xdfff) {
            int32_t idx=iter1->getIndex(iter1, UITER_CURRENT);
            iter1->previous(iter1); /* ==c1 */
            जर(!U16_IS_LEAD(iter1->previous(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } अन्यथा /* 0xe000<=c1<=0xffff */ {
            /* BMP code point - make <d800 */
            c1-=0x2800;
        }
}
#endif

U_CAPI int32_t U_EXPORT2
u_strCompare(स्थिरांक char16_t *s1, int32_t length1,
             स्थिरांक char16_t *s2, int32_t length2,
             UBool codePointOrder) {
    /* argument checking */
    जर(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        परत 0;
    }
    परत uprv_strCompare(s1, length1, s2, length2, असत्य, codePointOrder);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmpCodePointOrder(स्थिरांक char16_t *s1, स्थिरांक char16_t *s2) {
    परत uprv_strCompare(s1, -1, s2, -1, असत्य, सत्य);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(स्थिरांक char16_t  *s1,
     स्थिरांक char16_t  *s2,
     int32_t     n) 
{
    जर(n > 0) {
        int32_t rc;
        साठी(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            जर(rc != 0 || *s1 == 0 || --n == 0) {
                परत rc;
            }
            ++s1;
            ++s2;
        }
    } अन्यथा {
        परत 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmpCodePointOrder(स्थिरांक char16_t *s1, स्थिरांक char16_t *s2, int32_t n) {
    परत uprv_strCompare(s1, n, s2, n, सत्य, सत्य);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, स्थिरांक char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    पर्यंत((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    परत anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, स्थिरांक char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    पर्यंत(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    परत anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(स्थिरांक char16_t *s)
{
#जर U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    परत (int32_t)uprv_wcslen((स्थिरांक ब_अक्षर_प्रकार *)s);
#अन्यथा
    स्थिरांक char16_t *t = s;
    पर्यंत(*t != 0) {
      ++t;
    }
    परत t - s;
#endif
}

U_CAPI int32_t U_EXPORT2
u_मोजणी_Char32(स्थिरांक char16_t *s, int32_t length) {
    int32_t count;

    जर(s==nullptr || length<-1) {
        परत 0;
    }

    count=0;
    जर(length>=0) {
        पर्यंत(length>0) {
            ++count;
            जर(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } अन्यथा {
                ++s;
                --length;
            }
        }
    } अन्यथा /* length==-1 */ {
        char16_t c;

        साठी(;;) {
            जर((c=*s++)==0) {
                थांबा;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            जर(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    परत count;
}

U_CAPI UBool U_EXPORT2
u_strHasMoreChar32Than(स्थिरांक char16_t *s, int32_t length, int32_t number) {

    जर(number<0) {
        परत सत्य;
    }
    जर(s==nullptr || length<-1) {
        परत असत्य;
    }

    जर(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        साठी(;;) {
            जर((c=*s++)==0) {
                परत असत्य;
            }
            जर(number==0) {
                परत सत्य;
            }
            जर(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } अन्यथा {
        /* length>=0 known */
        स्थिरांक char16_t *limit;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        जर(((length+1)/2)>number) {
            परत सत्य;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        जर(maxSupplementary<=0) {
            परत असत्य;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        limit=s+length;
        साठी(;;) {
            जर(s==limit) {
                परत असत्य;
            }
            जर(number==0) {
                परत सत्य;
            }
            जर(U16_IS_LEAD(*s++) && s!=limit && U16_IS_TRAIL(*s)) {
                ++s;
                जर(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    परत असत्य;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, स्थिरांक char16_t* src, int32_t count) {
    जर(count > 0) {
        uprv_memcpy(dest, src, (माप_प्रकार)count*U_SIZEOF_UCHAR);
    }
    परत dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, स्थिरांक char16_t* src, int32_t count) {
    जर(count > 0) {
        uprv_memmove(dest, src, (माप_प्रकार)count*U_SIZEOF_UCHAR);
    }
    परत dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    जर(count > 0) {
        char16_t *ptr = dest;
        char16_t *limit = dest + count;

        पर्यंत (ptr < limit) {
            *(ptr++) = c;
        }
    }
    परत dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(स्थिरांक char16_t *buf1, स्थिरांक char16_t *buf2, int32_t count) {
    जर(count > 0) {
        स्थिरांक char16_t *limit = buf1 + count;
        int32_t result;

        पर्यंत (buf1 < limit) {
            result = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            जर (result != 0) {
                परत result;
            }
            buf1++;
            buf2++;
        }
    }
    परत 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmpCodePointOrder(स्थिरांक char16_t *s1, स्थिरांक char16_t *s2, int32_t count) {
    परत uprv_strCompare(s1, count, s2, count, असत्य, सत्य);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
स्थिर स्थिरांक char16_t UNESCAPE_MAP[] = {
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
गणना { UNESCAPE_MAP_LENGTH = UPRV_LENGTHOF(UNESCAPE_MAP) };

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
स्थिर int32_t _digit8(char16_t c) {
    जर (c >= u'0' && c <= u'7') {
        परत c - u'0';
    }
    परत -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
स्थिर int32_t _digit16(char16_t c) {
    जर (c >= u'0' && c <= u'9') {
        परत c - u'0';
    }
    जर (c >= u'A' && c <= u'F') {
        परत c - (u'A' - 10);
    }
    जर (c >= u'a' && c <= u'f') {
        परत c - (u'a' - 10);
    }
    परत -1;
}

/* Parse a single escape sequence.  Although this method deals in
 * UChars, it does not use C++ or UnicodeString.  This allows it to
 * be used from C contexts. */
U_CAPI UChar32 U_EXPORT2
u_unescapeAt(UNESCAPE_CHAR_AT charAt,
             int32_t *offset,
             int32_t length,
             शून्य *context) {

    int32_t start = *offset;
    UChar32 c;
    UChar32 result = 0;
    int8_t n = 0;
    int8_t minDig = 0;
    int8_t maxDig = 0;
    int8_t bitsPerDigit = 4; 
    int32_t dig;
    UBool braces = असत्य;

    /* Check that offset is in range */
    जर (*offset < 0 || *offset >= length) {
        जा err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*offset)++, context);

    /* Convert hexadecimal and octal escapes */
    निवड (c) {
    स्थिती u'u':
        minDig = maxDig = 4;
        थांबा;
    स्थिती u'U':
        minDig = maxDig = 8;
        थांबा;
    स्थिती u'x':
        minDig = 1;
        जर (*offset < length && charAt(*offset, context) == u'{') {
            ++(*offset);
            braces = सत्य;
            maxDig = 8;
        } अन्यथा {
            maxDig = 2;
        }
        थांबा;
    पूर्वनिर्धारित:
        dig = _digit8(c);
        जर (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            bitsPerDigit = 3;
            result = dig;
        }
        थांबा;
    }
    जर (minDig != 0) {
        पर्यंत (*offset < length && n < maxDig) {
            c = charAt(*offset, context);
            dig = (bitsPerDigit == 3) ? _digit8(c) : _digit16(c);
            जर (dig < 0) {
                थांबा;
            }
            result = (result << bitsPerDigit) | dig;
            ++(*offset);
            ++n;
        }
        जर (n < minDig) {
            जा err;
        }
        जर (braces) {
            जर (c != u'}') {
                जा err;
            }
            ++(*offset);
        }
        जर (result < 0 || result >= 0x110000) {
            जा err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        जर (*offset < length && U16_IS_LEAD(result)) {
            int32_t ahead = *offset + 1;
            c = charAt(*offset, context);
            जर (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t tailLimit = ahead + 11;
                जर (tailLimit > length) {
                    tailLimit = length;
                }
                c = u_unescapeAt(charAt, &ahead, tailLimit, context);
            }
            जर (U16_IS_TRAIL(c)) {
                *offset = ahead;
                result = U16_GET_SUPPLEMENTARY(result, c);
            }
        }
        परत result;
    }

    /* Convert C-style escapes in table */
    साठी (int32_t i=0; i<UNESCAPE_MAP_LENGTH; i+=2) {
        जर (c == UNESCAPE_MAP[i]) {
            परत UNESCAPE_MAP[i+1];
        } अन्यथा जर (c < UNESCAPE_MAP[i]) {
            थांबा;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    जर (c == u'c' && *offset < length) {
        c = charAt((*offset)++, context);
        जर (U16_IS_LEAD(c) && *offset < length) {
            char16_t c2 = charAt(*offset, context);
            जर (U16_IS_TRAIL(c2)) {
                ++(*offset);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        परत 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    जर (U16_IS_LEAD(c) && *offset < length) {
        char16_t c2 = charAt(*offset, context);
        जर (U16_IS_TRAIL(c2)) {
            ++(*offset);
            परत U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    परत c;

 err:
    /* Invalid escape sequence */
    *offset = start; /* Reset to initial value */
    परत (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
स्थिर char16_t U_CALLCONV
_charPtr_charAt(int32_t offset, शून्य *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_charsToUChars(static_cast<अक्षर*>(context) + offset, &c16, 1);
    परत c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
स्थिर शून्य _appendUChars(char16_t *dest, int32_t destCapacity,
                          स्थिरांक अक्षर *src, int32_t srcLen) {
    जर (destCapacity < 0) {
        destCapacity = 0;
    }
    जर (srcLen > destCapacity) {
        srcLen = destCapacity;
    }
    u_charsToUChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(स्थिरांक अक्षर *src, char16_t *dest, int32_t destCapacity) {
    स्थिरांक अक्षर *segment = src;
    int32_t i = 0;
    अक्षर c;

    पर्यंत ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        जर (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            जर (src != segment) {
                जर (dest != nullptr) {
                    _appendUChars(dest + i, destCapacity - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<अक्षर*>(src));
            जर (lenParsed == 0) {
                जा err;
            }
            src += lenParsed; /* advance past escape seq. */
            जर (dest != nullptr && U16_LENGTH(c32) <= (destCapacity - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } अन्यथा {
                i += U16_LENGTH(c32);
            }
            segment = src;
        } अन्यथा {
            ++src;
        }
    }
    जर (src != segment) {
        जर (dest != nullptr) {
            _appendUChars(dest + i, destCapacity - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    जर (dest != nullptr && i < destCapacity) {
        dest[i] = 0;
    }
    परत i;

 err:
    जर (dest != nullptr && destCapacity > 0) {
        *dest = 0;
    }
    परत 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#define __TERMINATE_STRING(dest, destCapacity, length, pErrorCode) UPRV_BLOCK_MACRO_BEGIN { \
    जर(pErrorCode!=nullptr && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        जर(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } अन्यथा जर(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            जर(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } अन्यथा जर(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } अन्यथा /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;                        \
        }                                                               \
    } \
} UPRV_BLOCK_MACRO_END

U_CAPI char16_t U_EXPORT2
u_asciiToUpper(char16_t c) {
    जर (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    परत c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    परत length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(अक्षर *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    परत length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    परत length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(ब_अक्षर_प्रकार *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    परत length;
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

#define STRING_HASH(TYPE, STR, STRLEN, DEREF) UPRV_BLOCK_MACRO_BEGIN { \
    uint32_t hash = 0;                        \
    स्थिरांक TYPE *p = (स्थिरांक TYPE*) STR;        \
    जर (p != nullptr) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        स्थिरांक TYPE *limit = p + len;          \
        पर्यंत (p<limit) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    परत static_cast<int32_t>(hash);        \
} UPRV_BLOCK_MACRO_END

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(स्थिरांक char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(स्थिरांक अक्षर *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(स्थिरांक अक्षर *str, int32_t length) {
    STRING_HASH(अक्षर, str, length, (uint8_t)uprv_tolower(*p));
}
