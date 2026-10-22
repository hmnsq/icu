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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/putil.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uchar.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "cwchar.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"

/* ANSI string.h - style functions ------------------------------------------ */

/* U+ffff is the highest BMP code point, the highest one that fits into a 16-bit char16_t */
#define U_BMP_MAX 0xffff

/* Forward binary string search functions ----------------------------------- */

/*
 * Test if a substring match inside a string is at code point boundaries.
 * All pointers refer to the same buffer.
 * The limit pointer may be nullptr, all others must be real pointers.
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ UBool
isMatchAtCPBoundary(ଅଚଳ char16_t *start, ଅଚଳ char16_t *match, ଅଚଳ char16_t *matchLimit, ଅଚଳ char16_t *limit) {
    ଯଦି(U16_IS_TRAIL(*match) && start!=match && U16_IS_LEAD(*(match-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି(U16_IS_LEAD(*(matchLimit-1)) && matchLimit!=limit && U16_IS_TRAIL(*matchLimit)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

U_CAPI char16_t* U_EXPORT2
u_strFindFirst(ଅଚଳ char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               ଅଚଳ char16_t* sub,
               int32_t subLength) {
    ଅଚଳ char16_t *start, *p, *q, *subLimit;
    char16_t c, cs, cq;

    ଯଦି(sub==nullptr || subLength<-1) {
        ଫେରନ୍ତୁ (char16_t *)s;
    }
    ଯଦି(s==nullptr || length<-1) {
        ଫେରନ୍ତୁ nullptr;
    }

    start=s;

    ଯଦି(length<0 && subLength<0) {
        /* both strings are NUL-terminated */
        ଯଦି((cs=*sub++)==0) {
            ଫେରନ୍ତୁ (char16_t *)s;
        }
        ଯଦି(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            ଫେରନ୍ତୁ u_strchr(s, cs);
        }

        ଯେତେବେଳେ((c=*s++)!=0) {
            ଯଦି(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                ପାଇଁ(;;) {
                    ଯଦି((cq=*q)==0) {
                        ଯଦି(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            ଫେରନ୍ତୁ (char16_t *)(s-1); /* well-formed match */
                        } ଅନ୍ୟଥା {
                            ଭାଙ୍ଗ; /* no match because surrogate pair is split */
                        }
                    }
                    ଯଦି((c=*p)==0) {
                        ଫେରନ୍ତୁ nullptr; /* no match, and none possible after s */
                    }
                    ଯଦି(c!=cq) {
                        ଭାଙ୍ଗ; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        ଫେରନ୍ତୁ nullptr;
    }

    ଯଦି(subLength<0) {
        subLength=u_strlen(sub);
    }
    ଯଦି(subLength==0) {
        ଫେରନ୍ତୁ (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --subLength;
    subLimit=sub+subLength;

    ଯଦି(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        ଫେରନ୍ତୁ length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    ଯଦି(length<0) {
        /* s is NUL-terminated */
        ଯେତେବେଳେ((c=*s++)!=0) {
            ଯଦି(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                ପାଇଁ(;;) {
                    ଯଦି(q==subLimit) {
                        ଯଦି(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            ଫେରନ୍ତୁ (char16_t *)(s-1); /* well-formed match */
                        } ଅନ୍ୟଥା {
                            ଭାଙ୍ଗ; /* no match because surrogate pair is split */
                        }
                    }
                    ଯଦି((c=*p)==0) {
                        ଫେରନ୍ତୁ nullptr; /* no match, and none possible after s */
                    }
                    ଯଦି(c!=*q) {
                        ଭାଙ୍ଗ; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } ଅନ୍ୟଥା {
        ଅଚଳ char16_t *limit, *preLimit;

        /* subLength was decremented above */
        ଯଦି(length<=subLength) {
            ଫେରନ୍ତୁ nullptr; /* s is shorter than sub */
        }

        limit=s+length;

        /* the substring must start before preLimit */
        preLimit=limit-subLength;

        ଯେତେବେଳେ(s!=preLimit) {
            c=*s++;
            ଯଦି(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                ପାଇଁ(;;) {
                    ଯଦି(q==subLimit) {
                        ଯଦି(isMatchAtCPBoundary(start, s-1, p, limit)) {
                            ଫେରନ୍ତୁ (char16_t *)(s-1); /* well-formed match */
                        } ଅନ୍ୟଥା {
                            ଭାଙ୍ଗ; /* no match because surrogate pair is split */
                        }
                    }
                    ଯଦି(*p!=*q) {
                        ଭାଙ୍ଗ; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    ଫେରନ୍ତୁ nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(ଅଚଳ char16_t* s U_LIFETIME_BOUND, ଅଚଳ char16_t* substring) {
    ଫେରନ୍ତୁ u_strFindFirst(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(ଅଚଳ char16_t* s U_LIFETIME_BOUND, char16_t c) {
    ଯଦି(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ଫେରନ୍ତୁ u_strFindFirst(s, -1, &c, 1);
    } ଅନ୍ୟଥା {
        char16_t cs;

        /* trivial search for a BMP code point */
        ପାଇଁ(;;) {
            ଯଦି((cs=*s)==c) {
                ଫେରନ୍ତୁ (char16_t *)s;
            }
            ଯଦି(cs==0) {
                ଫେରନ୍ତୁ nullptr;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(ଅଚଳ char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    ଯଦି((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ଫେରନ୍ତୁ u_strchr(s, (char16_t)c);
    } ଅନ୍ୟଥା ଯଦି((uint32_t)c<=UCHAR_MAX_ମୂଲ୍ୟ) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ଯେତେବେଳେ((cs=*s++)!=0) {
            ଯଦି(cs==lead && *s==trail) {
                ଫେରନ୍ତୁ (char16_t *)(s-1);
            }
        }
        ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା {
        /* not a Unicode code point, not findable */
        ଫେରନ୍ତୁ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(ଅଚଳ char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ଯଦି(count<=0) {
        ଫେରନ୍ତୁ nullptr; /* no string */
    } ଅନ୍ୟଥା ଯଦି(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ଫେରନ୍ତୁ u_strFindFirst(s, count, &c, 1);
    } ଅନ୍ୟଥା {
        /* trivial search for a BMP code point */
        ଅଚଳ char16_t *limit=s+count;
        କର {
            ଯଦି(*s==c) {
                ଫେରନ୍ତୁ (char16_t *)s;
            }
        } ଯେତେବେଳେ(++s!=limit);
        ଫେରନ୍ତୁ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(ଅଚଳ char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    ଯଦି((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ଫେରନ୍ତୁ u_memchr(s, (char16_t)c, count);
    } ଅନ୍ୟଥା ଯଦି(count<2) {
        /* too short for a surrogate pair */
        ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା ଯଦି((uint32_t)c<=UCHAR_MAX_ମୂଲ୍ୟ) {
        /* find supplementary code point as surrogate pair */
        ଅଚଳ char16_t *limit=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        କର {
            ଯଦି(*s==lead && *(s+1)==trail) {
                ଫେରନ୍ତୁ (char16_t *)s;
            }
        } ଯେତେବେଳେ(++s!=limit);
        ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା {
        /* not a Unicode code point, not findable */
        ଫେରନ୍ତୁ nullptr;
    }
}

/* Backward binary string search functions ---------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strFindLast(ଅଚଳ char16_t* s U_LIFETIME_BOUND,
              int32_t length,
              ଅଚଳ char16_t* sub,
              int32_t subLength) {
    ଅଚଳ char16_t *start, *limit, *p, *q, *subLimit;
    char16_t c, cs;

    ଯଦି(sub==nullptr || subLength<-1) {
        ଫେରନ୍ତୁ (char16_t *)s;
    }
    ଯଦି(s==nullptr || length<-1) {
        ଫେରନ୍ତୁ nullptr;
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

    ଯଦି(subLength<0) {
        subLength=u_strlen(sub);
    }
    ଯଦି(subLength==0) {
        ଫେରନ୍ତୁ (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    subLimit=sub+subLength;
    cs=*(--subLimit);
    --subLength;

    ଯଦି(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        ଫେରନ୍ତୁ length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    ଯଦି(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    ଯଦି(length<=subLength) {
        ଫେରନ୍ତୁ nullptr; /* s is shorter than sub */
    }

    start=s;
    limit=s+length;

    /* the substring must start no later than s+subLength */
    s+=subLength;

    ଯେତେବେଳେ(s!=limit) {
        c=*(--limit);
        ଯଦି(c==cs) {
            /* found last substring char16_t, compare rest */
            p=limit;
            q=subLimit;
            ପାଇଁ(;;) {
                ଯଦି(q==sub) {
                    ଯଦି(isMatchAtCPBoundary(start, p, limit+1, start+length)) {
                        ଫେରନ୍ତୁ (char16_t *)p; /* well-formed match */
                    } ଅନ୍ୟଥା {
                        ଭାଙ୍ଗ; /* no match because surrogate pair is split */
                    }
                }
                ଯଦି(*(--p)!=*(--q)) {
                    ଭାଙ୍ଗ; /* no match */
                }
            }
        }
    }

    /* not found */
    ଫେରନ୍ତୁ nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(ଅଚଳ char16_t* s U_LIFETIME_BOUND, ଅଚଳ char16_t* substring) {
    ଫେରନ୍ତୁ u_strFindLast(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(ଅଚଳ char16_t* s U_LIFETIME_BOUND, char16_t c) {
    ଯଦି(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ଫେରନ୍ତୁ u_strFindLast(s, -1, &c, 1);
    } ଅନ୍ୟଥା {
        ଅଚଳ char16_t *result=nullptr;
        char16_t cs;

        /* trivial search for a BMP code point */
        ପାଇଁ(;;) {
            ଯଦି((cs=*s)==c) {
                result=s;
            }
            ଯଦି(cs==0) {
                ଫେରନ୍ତୁ (char16_t *)result;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(ଅଚଳ char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    ଯଦି((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ଫେରନ୍ତୁ u_strrchr(s, (char16_t)c);
    } ଅନ୍ୟଥା ଯଦି((uint32_t)c<=UCHAR_MAX_ମୂଲ୍ୟ) {
        /* find supplementary code point as surrogate pair */
        ଅଚଳ char16_t *result=nullptr;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ଯେତେବେଳେ((cs=*s++)!=0) {
            ଯଦି(cs==lead && *s==trail) {
                result=s-1;
            }
        }
        ଫେରନ୍ତୁ (char16_t *)result;
    } ଅନ୍ୟଥା {
        /* not a Unicode code point, not findable */
        ଫେରନ୍ତୁ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(ଅଚଳ char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ଯଦି(count<=0) {
        ଫେରନ୍ତୁ nullptr; /* no string */
    } ଅନ୍ୟଥା ଯଦି(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ଫେରନ୍ତୁ u_strFindLast(s, count, &c, 1);
    } ଅନ୍ୟଥା {
        /* trivial search for a BMP code point */
        ଅଚଳ char16_t *limit=s+count;
        କର {
            ଯଦି(*(--limit)==c) {
                ଫେରନ୍ତୁ (char16_t *)limit;
            }
        } ଯେତେବେଳେ(s!=limit);
        ଫେରନ୍ତୁ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(ଅଚଳ char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    ଯଦି((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ଫେରନ୍ତୁ u_memrchr(s, (char16_t)c, count);
    } ଅନ୍ୟଥା ଯଦି(count<2) {
        /* too short for a surrogate pair */
        ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା ଯଦି((uint32_t)c<=UCHAR_MAX_ମୂଲ୍ୟ) {
        /* find supplementary code point as surrogate pair */
        ଅଚଳ char16_t *limit=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        କର {
            ଯଦି(*limit==trail && *(limit-1)==lead) {
                ଫେରନ୍ତୁ (char16_t *)(limit-1);
            }
        } ଯେତେବେଳେ(s!=--limit);
        ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା {
        /* not a Unicode code point, not findable */
        ଫେରନ୍ତୁ nullptr;
    }
}

/* Tokenization functions --------------------------------------------------- */

/*
 * Match each code point in a string against each code point in the matchSet.
 * Return the index of the first string code point that
 * is (polarity==true) or is not (false) contained in the matchSet.
 * Return -(string length)-1 if there is no such code point.
 */
ସ୍ଥିର int32_t
_matchFromSet(ଅଚଳ char16_t *string, ଅଚଳ char16_t *matchSet, UBool polarity) {
    int32_t matchLen, matchBMPLen, strItr, matchItr;
    UChar32 stringCh, matchCh;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    matchBMPLen = 0;
    ଯେତେବେଳେ((c = matchSet[matchBMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++matchBMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    matchLen = matchBMPLen;
    ଯେତେବେଳେ(matchSet[matchLen] != 0) {
        ++matchLen;
    }

    ପାଇଁ(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        ଯଦି(U16_IS_SINGLE(c)) {
            ଯଦି(polarity) {
                ପାଇଁ(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    ଯଦି(c == matchSet[matchItr]) {
                        ଫେରନ୍ତୁ strItr - 1; /* one matches */
                    }
                }
            } ଅନ୍ୟଥା {
                ପାଇଁ(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    ଯଦି(c == matchSet[matchItr]) {
                        ଯାଅ endloop;
                    }
                }
                ଫେରନ୍ତୁ strItr - 1; /* none matches */
            }
        } ଅନ୍ୟଥା {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            ଯଦି(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } ଅନ୍ୟଥା {
                stringCh = c; /* unpaired trail surrogate */
            }

            ଯଦି(polarity) {
                ପାଇଁ(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    ଯଦି(stringCh == matchCh) {
                        ଫେରନ୍ତୁ strItr - U16_LENGTH(stringCh); /* one matches */
                    }
                }
            } ଅନ୍ୟଥା {
                ପାଇଁ(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    ଯଦି(stringCh == matchCh) {
                        ଯାଅ endloop;
                    }
                }
                ଫେରନ୍ତୁ strItr - U16_LENGTH(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    ଫେରନ୍ତୁ -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(ଅଚଳ char16_t* string U_LIFETIME_BOUND, ଅଚଳ char16_t* matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, ସତ୍ୟ);
    ଯଦି(idx >= 0) {
        ଫେରନ୍ତୁ (char16_t *)string + idx;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ nullptr;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(ଅଚଳ char16_t *string, ଅଚଳ char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, ସତ୍ୟ);
    ଯଦି(idx >= 0) {
        ଫେରନ୍ତୁ idx;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(ଅଚଳ char16_t *string, ଅଚଳ char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, ମିଥ୍ୟା);
    ଯଦି(idx >= 0) {
        ଫେରନ୍ତୁ idx;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ -idx - 1; /* == u_strlen(string) */
    }
}

/* ----- Text manipulation functions --- */

U_CAPI char16_t* U_EXPORT2
u_strtok_r(char16_t* src U_LIFETIME_BOUND, ଅଚଳ char16_t* delim, char16_t** saveState)
{
    char16_t *tokSource;
    char16_t *nextToken;
    uint32_t nonDelimIdx;

    /* If saveState is nullptr, the user messed up. */
    ଯଦି (src != nullptr) {
        tokSource = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    ଅନ୍ୟଥା ଯଦି (*saveState) {
        tokSource = *saveState;
    }
    ଅନ୍ୟଥା {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        ଫେରନ୍ତୁ nullptr;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tokSource, delim);
    tokSource = &tokSource[nonDelimIdx];

    ଯଦି (*tokSource) {
        nextToken = u_strpbrk(tokSource, delim);
        ଯଦି (nextToken != nullptr) {
            /* Create a token */
            *(nextToken++) = 0;
            *saveState = nextToken;
            ଫେରନ୍ତୁ tokSource;
        }
        ଅନ୍ୟଥା ଯଦି (*saveState) {
            /* Return the last token */
            *saveState = nullptr;
            ଫେରନ୍ତୁ tokSource;
        }
    }
    ଅନ୍ୟଥା {
        /* No tokens were found. Only delimiters were left. */
        *saveState = nullptr;
    }
    ଫେରନ୍ତୁ nullptr;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, ଅଚଳ char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    ଯେତେବେଳେ(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    ଯେତେବେଳେ((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    ଫେରନ୍ତୁ anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, ଅଚଳ char16_t* src, int32_t n)
{
    ଯଦି(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        ଯେତେବେଳେ(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        ଯେତେବେଳେ((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            ଯଦି(--n == 0) {
                *dst = 0;
                ଭାଙ୍ଗ;
            }
            ++src;
        }

        ଫେରନ୍ତୁ anchor;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(ଅଚଳ char16_t *s1,
    ଅଚଳ char16_t *s2)
{
    char16_t  c1, c2;

    ପାଇଁ(;;) {
        c1=*s1++;
        c2=*s2++;
        ଯଦି (c1 != c2 || c1 == 0) {
            ଭାଙ୍ଗ;
        }
    }
    ଫେରନ୍ତୁ (int32_t)c1 - (int32_t)c2;
}

U_CFUNC int32_t U_EXPORT2
uprv_strCompare(ଅଚଳ char16_t *s1, int32_t length1,
                ଅଚଳ char16_t *s2, int32_t length2,
                UBool strncmpStyle, UBool codePointOrder) {
    ଅଚଳ char16_t *start1, *start2, *limit1, *limit2;
    char16_t c1, c2;

    /* setup for fix-up */
    start1=s1;
    start2=s2;

    /* compare identical prefixes - they do not need to be fixed up */
    ଯଦି(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        ଯଦି(s1==s2) {
            ଫେରନ୍ତୁ 0;
        }

        ପାଇଁ(;;) {
            c1=*s1;
            c2=*s2;
            ଯଦି(c1!=c2) {
                ଭାଙ୍ଗ;
            }
            ଯଦି(c1==0) {
                ଫେରନ୍ତୁ 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=nullptr;
    } ଅନ୍ୟଥା ଯଦି(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        ଯଦି(s1==s2) {
            ଫେରନ୍ତୁ 0;
        }

        limit1=start1+length1;

        ପାଇଁ(;;) {
            /* both lengths are same, check only one limit */
            ଯଦି(s1==limit1) {
                ଫେରନ୍ତୁ 0;
            }

            c1=*s1;
            c2=*s2;
            ଯଦି(c1!=c2) {
                ଭାଙ୍ଗ;
            }
            ଯଦି(c1==0) {
                ଫେରନ୍ତୁ 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } ଅନ୍ୟଥା {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t lengthResult;

        ଯଦି(length1<0) {
            length1=u_strlen(s1);
        }
        ଯଦି(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        ଯଦି(length1<length2) {
            lengthResult=-1;
            limit1=start1+length1;
        } ଅନ୍ୟଥା ଯଦି(length1==length2) {
            lengthResult=0;
            limit1=start1+length1;
        } ଅନ୍ୟଥା /* length1>length2 */ {
            lengthResult=1;
            limit1=start1+length2;
        }

        ଯଦି(s1==s2) {
            ଫେରନ୍ତୁ lengthResult;
        }

        ପାଇଁ(;;) {
            /* check pseudo-limit */
            ଯଦି(s1==limit1) {
                ଫେରନ୍ତୁ lengthResult;
            }

            c1=*s1;
            c2=*s2;
            ଯଦି(c1!=c2) {
                ଭାଙ୍ଗ;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    ଯଦି(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        ଯଦି(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ଅନ୍ୟଥା {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        ଯଦି(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ଅନ୍ୟଥା {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    ଫେରନ୍ତୁ (int32_t)c1-(int32_t)c2;
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
    ଯଦି(iter1==nullptr || iter2==nullptr) {
        ଫେରନ୍ତୁ 0; /* bad arguments */
    }
    ଯଦି(iter1==iter2) {
        ଫେରନ୍ତୁ 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_START);
    iter2->move(iter2, 0, UITER_START);

    /* compare identical prefixes - they do not need to be fixed up */
    ପାଇଁ(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        ଯଦି(c1!=c2) {
            ଭାଙ୍ଗ;
        }
        ଯଦି(c1==-1) {
            ଫେରନ୍ତୁ 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    ଯଦି(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        ଯଦି(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->previous(iter1), U16_IS_LEAD(iter1->previous(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ଅନ୍ୟଥା {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        ଯଦି(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->previous(iter2), U16_IS_LEAD(iter2->previous(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ଅନ୍ୟଥା {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    ଫେରନ୍ତୁ (int32_t)c1-(int32_t)c2;
}

#ଯଦି 0
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
ଶୂନ୍ୟ fragment {
        /* iff a surrogate is part of a surrogate pair, leave >=d800 */
        ଯଦି(c1<=0xdbff) {
            ଯଦି(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } ଅନ୍ୟଥା ଯଦି(c1<=0xdfff) {
            int32_t idx=iter1->getIndex(iter1, UITER_CURRENT);
            iter1->previous(iter1); /* ==c1 */
            ଯଦି(!U16_IS_LEAD(iter1->previous(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } ଅନ୍ୟଥା /* 0xe000<=c1<=0xffff */ {
            /* BMP code point - make <d800 */
            c1-=0x2800;
        }
}
#endif

U_CAPI int32_t U_EXPORT2
u_strCompare(ଅଚଳ char16_t *s1, int32_t length1,
             ଅଚଳ char16_t *s2, int32_t length2,
             UBool codePointOrder) {
    /* argument checking */
    ଯଦି(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ uprv_strCompare(s1, length1, s2, length2, ମିଥ୍ୟା, codePointOrder);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmpCodePointOrder(ଅଚଳ char16_t *s1, ଅଚଳ char16_t *s2) {
    ଫେରନ୍ତୁ uprv_strCompare(s1, -1, s2, -1, ମିଥ୍ୟା, ସତ୍ୟ);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(ଅଚଳ char16_t  *s1,
     ଅଚଳ char16_t  *s2,
     int32_t     n) 
{
    ଯଦି(n > 0) {
        int32_t rc;
        ପାଇଁ(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            ଯଦି(rc != 0 || *s1 == 0 || --n == 0) {
                ଫେରନ୍ତୁ rc;
            }
            ++s1;
            ++s2;
        }
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmpCodePointOrder(ଅଚଳ char16_t *s1, ଅଚଳ char16_t *s2, int32_t n) {
    ଫେରନ୍ତୁ uprv_strCompare(s1, n, s2, n, ସତ୍ୟ, ସତ୍ୟ);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, ଅଚଳ char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    ଯେତେବେଳେ((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    ଫେରନ୍ତୁ anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, ଅଚଳ char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    ଯେତେବେଳେ(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    ଫେରନ୍ତୁ anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(ଅଚଳ char16_t *s)
{
#ଯଦି U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    ଫେରନ୍ତୁ (int32_t)uprv_wcslen((ଅଚଳ wchar_t *)s);
#ଅନ୍ୟଥା
    ଅଚଳ char16_t *t = s;
    ଯେତେବେଳେ(*t != 0) {
      ++t;
    }
    ଫେରନ୍ତୁ t - s;
#endif
}

U_CAPI int32_t U_EXPORT2
u_ଗଣନା_Char32(ଅଚଳ char16_t *s, int32_t length) {
    int32_t count;

    ଯଦି(s==nullptr || length<-1) {
        ଫେରନ୍ତୁ 0;
    }

    count=0;
    ଯଦି(length>=0) {
        ଯେତେବେଳେ(length>0) {
            ++count;
            ଯଦି(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } ଅନ୍ୟଥା {
                ++s;
                --length;
            }
        }
    } ଅନ୍ୟଥା /* length==-1 */ {
        char16_t c;

        ପାଇଁ(;;) {
            ଯଦି((c=*s++)==0) {
                ଭାଙ୍ଗ;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            ଯଦି(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    ଫେରନ୍ତୁ count;
}

U_CAPI UBool U_EXPORT2
u_strHasMoreChar32Than(ଅଚଳ char16_t *s, int32_t length, int32_t number) {

    ଯଦି(number<0) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି(s==nullptr || length<-1) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    ଯଦି(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        ପାଇଁ(;;) {
            ଯଦି((c=*s++)==0) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            ଯଦି(number==0) {
                ଫେରନ୍ତୁ ସତ୍ୟ;
            }
            ଯଦି(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } ଅନ୍ୟଥା {
        /* length>=0 known */
        ଅଚଳ char16_t *limit;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        ଯଦି(((length+1)/2)>number) {
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        ଯଦି(maxSupplementary<=0) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        limit=s+length;
        ପାଇଁ(;;) {
            ଯଦି(s==limit) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            ଯଦି(number==0) {
                ଫେରନ୍ତୁ ସତ୍ୟ;
            }
            ଯଦି(U16_IS_LEAD(*s++) && s!=limit && U16_IS_TRAIL(*s)) {
                ++s;
                ଯଦି(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    ଫେରନ୍ତୁ ମିଥ୍ୟା;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, ଅଚଳ char16_t* src, int32_t count) {
    ଯଦି(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    ଫେରନ୍ତୁ dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, ଅଚଳ char16_t* src, int32_t count) {
    ଯଦି(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    ଫେରନ୍ତୁ dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ଯଦି(count > 0) {
        char16_t *ptr = dest;
        char16_t *limit = dest + count;

        ଯେତେବେଳେ (ptr < limit) {
            *(ptr++) = c;
        }
    }
    ଫେରନ୍ତୁ dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(ଅଚଳ char16_t *buf1, ଅଚଳ char16_t *buf2, int32_t count) {
    ଯଦି(count > 0) {
        ଅଚଳ char16_t *limit = buf1 + count;
        int32_t result;

        ଯେତେବେଳେ (buf1 < limit) {
            result = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            ଯଦି (result != 0) {
                ଫେରନ୍ତୁ result;
            }
            buf1++;
            buf2++;
        }
    }
    ଫେରନ୍ତୁ 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmpCodePointOrder(ଅଚଳ char16_t *s1, ଅଚଳ char16_t *s2, int32_t count) {
    ଫେରନ୍ତୁ uprv_strCompare(s1, count, s2, count, ମିଥ୍ୟା, ସତ୍ୟ);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
ସ୍ଥିର ଅଚଳ char16_t UNESCAPE_MAP[] = {
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
ଗଣନା { UNESCAPE_MAP_LENGTH = UPRV_LENGTHOF(UNESCAPE_MAP) };

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
ସ୍ଥିର int32_t _digit8(char16_t c) {
    ଯଦି (c >= u'0' && c <= u'7') {
        ଫେରନ୍ତୁ c - u'0';
    }
    ଫେରନ୍ତୁ -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
ସ୍ଥିର int32_t _digit16(char16_t c) {
    ଯଦି (c >= u'0' && c <= u'9') {
        ଫେରନ୍ତୁ c - u'0';
    }
    ଯଦି (c >= u'A' && c <= u'F') {
        ଫେରନ୍ତୁ c - (u'A' - 10);
    }
    ଯଦି (c >= u'a' && c <= u'f') {
        ଫେରନ୍ତୁ c - (u'a' - 10);
    }
    ଫେରନ୍ତୁ -1;
}

/* Parse a single escape sequence.  Although this method deals in
 * UChars, it does not use C++ or UnicodeString.  This allows it to
 * be used from C contexts. */
U_CAPI UChar32 U_EXPORT2
u_unescapeAt(UNESCAPE_CHAR_AT charAt,
             int32_t *offset,
             int32_t length,
             ଶୂନ୍ୟ *context) {

    int32_t start = *offset;
    UChar32 c;
    UChar32 result = 0;
    int8_t n = 0;
    int8_t minDig = 0;
    int8_t maxDig = 0;
    int8_t bitsPerDigit = 4; 
    int32_t dig;
    UBool braces = ମିଥ୍ୟା;

    /* Check that offset is in range */
    ଯଦି (*offset < 0 || *offset >= length) {
        ଯାଅ err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*offset)++, context);

    /* Convert hexadecimal and octal escapes */
    ବାଛ (c) {
    ସ୍ଥିତି u'u':
        minDig = maxDig = 4;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି u'U':
        minDig = maxDig = 8;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି u'x':
        minDig = 1;
        ଯଦି (*offset < length && charAt(*offset, context) == u'{') {
            ++(*offset);
            braces = ସତ୍ୟ;
            maxDig = 8;
        } ଅନ୍ୟଥା {
            maxDig = 2;
        }
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        dig = _digit8(c);
        ଯଦି (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            bitsPerDigit = 3;
            result = dig;
        }
        ଭାଙ୍ଗ;
    }
    ଯଦି (minDig != 0) {
        ଯେତେବେଳେ (*offset < length && n < maxDig) {
            c = charAt(*offset, context);
            dig = (bitsPerDigit == 3) ? _digit8(c) : _digit16(c);
            ଯଦି (dig < 0) {
                ଭାଙ୍ଗ;
            }
            result = (result << bitsPerDigit) | dig;
            ++(*offset);
            ++n;
        }
        ଯଦି (n < minDig) {
            ଯାଅ err;
        }
        ଯଦି (braces) {
            ଯଦି (c != u'}') {
                ଯାଅ err;
            }
            ++(*offset);
        }
        ଯଦି (result < 0 || result >= 0x110000) {
            ଯାଅ err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        ଯଦି (*offset < length && U16_IS_LEAD(result)) {
            int32_t ahead = *offset + 1;
            c = charAt(*offset, context);
            ଯଦି (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t tailLimit = ahead + 11;
                ଯଦି (tailLimit > length) {
                    tailLimit = length;
                }
                c = u_unescapeAt(charAt, &ahead, tailLimit, context);
            }
            ଯଦି (U16_IS_TRAIL(c)) {
                *offset = ahead;
                result = U16_GET_SUPPLEMENTARY(result, c);
            }
        }
        ଫେରନ୍ତୁ result;
    }

    /* Convert C-style escapes in table */
    ପାଇଁ (int32_t i=0; i<UNESCAPE_MAP_LENGTH; i+=2) {
        ଯଦି (c == UNESCAPE_MAP[i]) {
            ଫେରନ୍ତୁ UNESCAPE_MAP[i+1];
        } ଅନ୍ୟଥା ଯଦି (c < UNESCAPE_MAP[i]) {
            ଭାଙ୍ଗ;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    ଯଦି (c == u'c' && *offset < length) {
        c = charAt((*offset)++, context);
        ଯଦି (U16_IS_LEAD(c) && *offset < length) {
            char16_t c2 = charAt(*offset, context);
            ଯଦି (U16_IS_TRAIL(c2)) {
                ++(*offset);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        ଫେରନ୍ତୁ 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    ଯଦି (U16_IS_LEAD(c) && *offset < length) {
        char16_t c2 = charAt(*offset, context);
        ଯଦି (U16_IS_TRAIL(c2)) {
            ++(*offset);
            ଫେରନ୍ତୁ U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    ଫେରନ୍ତୁ c;

 err:
    /* Invalid escape sequence */
    *offset = start; /* Reset to initial value */
    ଫେରନ୍ତୁ (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
ସ୍ଥିର char16_t U_CALLCONV
_charPtr_charAt(int32_t offset, ଶୂନ୍ୟ *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_charsToUChars(static_cast<ଅକ୍ଷର*>(context) + offset, &c16, 1);
    ଫେରନ୍ତୁ c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
ସ୍ଥିର ଶୂନ୍ୟ _appendUChars(char16_t *dest, int32_t destCapacity,
                          ଅଚଳ ଅକ୍ଷର *src, int32_t srcLen) {
    ଯଦି (destCapacity < 0) {
        destCapacity = 0;
    }
    ଯଦି (srcLen > destCapacity) {
        srcLen = destCapacity;
    }
    u_charsToUChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(ଅଚଳ ଅକ୍ଷର *src, char16_t *dest, int32_t destCapacity) {
    ଅଚଳ ଅକ୍ଷର *segment = src;
    int32_t i = 0;
    ଅକ୍ଷର c;

    ଯେତେବେଳେ ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        ଯଦି (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            ଯଦି (src != segment) {
                ଯଦି (dest != nullptr) {
                    _appendUChars(dest + i, destCapacity - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<ଅକ୍ଷର*>(src));
            ଯଦି (lenParsed == 0) {
                ଯାଅ err;
            }
            src += lenParsed; /* advance past escape seq. */
            ଯଦି (dest != nullptr && U16_LENGTH(c32) <= (destCapacity - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } ଅନ୍ୟଥା {
                i += U16_LENGTH(c32);
            }
            segment = src;
        } ଅନ୍ୟଥା {
            ++src;
        }
    }
    ଯଦି (src != segment) {
        ଯଦି (dest != nullptr) {
            _appendUChars(dest + i, destCapacity - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    ଯଦି (dest != nullptr && i < destCapacity) {
        dest[i] = 0;
    }
    ଫେରନ୍ତୁ i;

 err:
    ଯଦି (dest != nullptr && destCapacity > 0) {
        *dest = 0;
    }
    ଫେରନ୍ତୁ 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#define __TERMINATE_STRING(dest, destCapacity, length, pErrorCode) UPRV_BLOCK_MACRO_BEGIN { \
    ଯଦି(pErrorCode!=nullptr && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        ଯଦି(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } ଅନ୍ୟଥା ଯଦି(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            ଯଦି(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } ଅନ୍ୟଥା ଯଦି(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } ଅନ୍ୟଥା /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;                        \
        }                                                               \
    } \
} UPRV_BLOCK_MACRO_END

U_CAPI char16_t U_EXPORT2
u_asciiToUpper(char16_t c) {
    ଯଦି (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    ଫେରନ୍ତୁ c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ଫେରନ୍ତୁ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(ଅକ୍ଷର *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ଫେରନ୍ତୁ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ଫେରନ୍ତୁ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ଫେରନ୍ତୁ length;
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
    ଅଚଳ TYPE *p = (ଅଚଳ TYPE*) STR;        \
    ଯଦି (p != nullptr) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        ଅଚଳ TYPE *limit = p + len;          \
        ଯେତେବେଳେ (p<limit) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    ଫେରନ୍ତୁ static_cast<int32_t>(hash);        \
} UPRV_BLOCK_MACRO_END

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(ଅଚଳ char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(ଅଚଳ ଅକ୍ଷର *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(ଅଚଳ ଅକ୍ଷର *str, int32_t length) {
    STRING_HASH(ଅକ୍ଷର, str, length, (uint8_t)uprv_tolower(*p));
}
