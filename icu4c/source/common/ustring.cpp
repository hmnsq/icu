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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "cwchar.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "ustr_imp.h"

/* ANSI string.h - style functions ------------------------------------------ */

/* U+ffff is the highest BMP code point, the highest one that fits into a 16-bit char16_t */
#define U_BMP_MAX 0xffff

/* Forward binary string search functions ----------------------------------- */

/*
 * Test if a substring match inside a string is at code point boundaries.
 * All pointers refer to the same buffer.
 * The limit pointer may be nullptr, all others must be real pointers.
 */
സ്ഥിതം വരിയിൽ UBool
isMatchAtCPBoundary(സ്ഥിരം char16_t *start, സ്ഥിരം char16_t *match, സ്ഥിരം char16_t *matchLimit, സ്ഥിരം char16_t *limit) {
    എങ്കിൽ(U16_IS_TRAIL(*match) && start!=match && U16_IS_LEAD(*(match-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        തിരികെ അസത്യം;
    }
    എങ്കിൽ(U16_IS_LEAD(*(matchLimit-1)) && matchLimit!=limit && U16_IS_TRAIL(*matchLimit)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        തിരികെ അസത്യം;
    }
    തിരികെ സത്യം;
}

U_CAPI char16_t* U_EXPORT2
u_strFindFirst(സ്ഥിരം char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               സ്ഥിരം char16_t* sub,
               int32_t subLength) {
    സ്ഥിരം char16_t *start, *p, *q, *subLimit;
    char16_t c, cs, cq;

    എങ്കിൽ(sub==nullptr || subLength<-1) {
        തിരികെ (char16_t *)s;
    }
    എങ്കിൽ(s==nullptr || length<-1) {
        തിരികെ nullptr;
    }

    start=s;

    എങ്കിൽ(length<0 && subLength<0) {
        /* both strings are NUL-terminated */
        എങ്കിൽ((cs=*sub++)==0) {
            തിരികെ (char16_t *)s;
        }
        എങ്കിൽ(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            തിരികെ u_strchr(s, cs);
        }

        ആകുമ്പോൾ((c=*s++)!=0) {
            എങ്കിൽ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                വേണ്ടി(;;) {
                    എങ്കിൽ((cq=*q)==0) {
                        എങ്കിൽ(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            തിരികെ (char16_t *)(s-1); /* well-formed match */
                        } അല്ലെങ്കിൽ {
                            നിർത്തുക; /* no match because surrogate pair is split */
                        }
                    }
                    എങ്കിൽ((c=*p)==0) {
                        തിരികെ nullptr; /* no match, and none possible after s */
                    }
                    എങ്കിൽ(c!=cq) {
                        നിർത്തുക; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        തിരികെ nullptr;
    }

    എങ്കിൽ(subLength<0) {
        subLength=u_strlen(sub);
    }
    എങ്കിൽ(subLength==0) {
        തിരികെ (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --subLength;
    subLimit=sub+subLength;

    എങ്കിൽ(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        തിരികെ length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    എങ്കിൽ(length<0) {
        /* s is NUL-terminated */
        ആകുമ്പോൾ((c=*s++)!=0) {
            എങ്കിൽ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                വേണ്ടി(;;) {
                    എങ്കിൽ(q==subLimit) {
                        എങ്കിൽ(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            തിരികെ (char16_t *)(s-1); /* well-formed match */
                        } അല്ലെങ്കിൽ {
                            നിർത്തുക; /* no match because surrogate pair is split */
                        }
                    }
                    എങ്കിൽ((c=*p)==0) {
                        തിരികെ nullptr; /* no match, and none possible after s */
                    }
                    എങ്കിൽ(c!=*q) {
                        നിർത്തുക; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } അല്ലെങ്കിൽ {
        സ്ഥിരം char16_t *limit, *preLimit;

        /* subLength was decremented above */
        എങ്കിൽ(length<=subLength) {
            തിരികെ nullptr; /* s is shorter than sub */
        }

        limit=s+length;

        /* the substring must start before preLimit */
        preLimit=limit-subLength;

        ആകുമ്പോൾ(s!=preLimit) {
            c=*s++;
            എങ്കിൽ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                വേണ്ടി(;;) {
                    എങ്കിൽ(q==subLimit) {
                        എങ്കിൽ(isMatchAtCPBoundary(start, s-1, p, limit)) {
                            തിരികെ (char16_t *)(s-1); /* well-formed match */
                        } അല്ലെങ്കിൽ {
                            നിർത്തുക; /* no match because surrogate pair is split */
                        }
                    }
                    എങ്കിൽ(*p!=*q) {
                        നിർത്തുക; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    തിരികെ nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(സ്ഥിരം char16_t* s U_LIFETIME_BOUND, സ്ഥിരം char16_t* substring) {
    തിരികെ u_strFindFirst(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(സ്ഥിരം char16_t* s U_LIFETIME_BOUND, char16_t c) {
    എങ്കിൽ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        തിരികെ u_strFindFirst(s, -1, &c, 1);
    } അല്ലെങ്കിൽ {
        char16_t cs;

        /* trivial search for a BMP code point */
        വേണ്ടി(;;) {
            എങ്കിൽ((cs=*s)==c) {
                തിരികെ (char16_t *)s;
            }
            എങ്കിൽ(cs==0) {
                തിരികെ nullptr;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(സ്ഥിരം char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    എങ്കിൽ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        തിരികെ u_strchr(s, (char16_t)c);
    } അല്ലെങ്കിൽ എങ്കിൽ((uint32_t)c<=UCHAR_MAX_മൂല്യം) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ആകുമ്പോൾ((cs=*s++)!=0) {
            എങ്കിൽ(cs==lead && *s==trail) {
                തിരികെ (char16_t *)(s-1);
            }
        }
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ {
        /* not a Unicode code point, not findable */
        തിരികെ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(സ്ഥിരം char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    എങ്കിൽ(count<=0) {
        തിരികെ nullptr; /* no string */
    } അല്ലെങ്കിൽ എങ്കിൽ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        തിരികെ u_strFindFirst(s, count, &c, 1);
    } അല്ലെങ്കിൽ {
        /* trivial search for a BMP code point */
        സ്ഥിരം char16_t *limit=s+count;
        ചെയ്യുക {
            എങ്കിൽ(*s==c) {
                തിരികെ (char16_t *)s;
            }
        } ആകുമ്പോൾ(++s!=limit);
        തിരികെ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(സ്ഥിരം char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    എങ്കിൽ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        തിരികെ u_memchr(s, (char16_t)c, count);
    } അല്ലെങ്കിൽ എങ്കിൽ(count<2) {
        /* too short for a surrogate pair */
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ എങ്കിൽ((uint32_t)c<=UCHAR_MAX_മൂല്യം) {
        /* find supplementary code point as surrogate pair */
        സ്ഥിരം char16_t *limit=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ചെയ്യുക {
            എങ്കിൽ(*s==lead && *(s+1)==trail) {
                തിരികെ (char16_t *)s;
            }
        } ആകുമ്പോൾ(++s!=limit);
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ {
        /* not a Unicode code point, not findable */
        തിരികെ nullptr;
    }
}

/* Backward binary string search functions ---------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strFindLast(സ്ഥിരം char16_t* s U_LIFETIME_BOUND,
              int32_t length,
              സ്ഥിരം char16_t* sub,
              int32_t subLength) {
    സ്ഥിരം char16_t *start, *limit, *p, *q, *subLimit;
    char16_t c, cs;

    എങ്കിൽ(sub==nullptr || subLength<-1) {
        തിരികെ (char16_t *)s;
    }
    എങ്കിൽ(s==nullptr || length<-1) {
        തിരികെ nullptr;
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

    എങ്കിൽ(subLength<0) {
        subLength=u_strlen(sub);
    }
    എങ്കിൽ(subLength==0) {
        തിരികെ (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    subLimit=sub+subLength;
    cs=*(--subLimit);
    --subLength;

    എങ്കിൽ(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        തിരികെ length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    എങ്കിൽ(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    എങ്കിൽ(length<=subLength) {
        തിരികെ nullptr; /* s is shorter than sub */
    }

    start=s;
    limit=s+length;

    /* the substring must start no later than s+subLength */
    s+=subLength;

    ആകുമ്പോൾ(s!=limit) {
        c=*(--limit);
        എങ്കിൽ(c==cs) {
            /* found last substring char16_t, compare rest */
            p=limit;
            q=subLimit;
            വേണ്ടി(;;) {
                എങ്കിൽ(q==sub) {
                    എങ്കിൽ(isMatchAtCPBoundary(start, p, limit+1, start+length)) {
                        തിരികെ (char16_t *)p; /* well-formed match */
                    } അല്ലെങ്കിൽ {
                        നിർത്തുക; /* no match because surrogate pair is split */
                    }
                }
                എങ്കിൽ(*(--p)!=*(--q)) {
                    നിർത്തുക; /* no match */
                }
            }
        }
    }

    /* not found */
    തിരികെ nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(സ്ഥിരം char16_t* s U_LIFETIME_BOUND, സ്ഥിരം char16_t* substring) {
    തിരികെ u_strFindLast(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(സ്ഥിരം char16_t* s U_LIFETIME_BOUND, char16_t c) {
    എങ്കിൽ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        തിരികെ u_strFindLast(s, -1, &c, 1);
    } അല്ലെങ്കിൽ {
        സ്ഥിരം char16_t *result=nullptr;
        char16_t cs;

        /* trivial search for a BMP code point */
        വേണ്ടി(;;) {
            എങ്കിൽ((cs=*s)==c) {
                result=s;
            }
            എങ്കിൽ(cs==0) {
                തിരികെ (char16_t *)result;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(സ്ഥിരം char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    എങ്കിൽ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        തിരികെ u_strrchr(s, (char16_t)c);
    } അല്ലെങ്കിൽ എങ്കിൽ((uint32_t)c<=UCHAR_MAX_മൂല്യം) {
        /* find supplementary code point as surrogate pair */
        സ്ഥിരം char16_t *result=nullptr;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ആകുമ്പോൾ((cs=*s++)!=0) {
            എങ്കിൽ(cs==lead && *s==trail) {
                result=s-1;
            }
        }
        തിരികെ (char16_t *)result;
    } അല്ലെങ്കിൽ {
        /* not a Unicode code point, not findable */
        തിരികെ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(സ്ഥിരം char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    എങ്കിൽ(count<=0) {
        തിരികെ nullptr; /* no string */
    } അല്ലെങ്കിൽ എങ്കിൽ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        തിരികെ u_strFindLast(s, count, &c, 1);
    } അല്ലെങ്കിൽ {
        /* trivial search for a BMP code point */
        സ്ഥിരം char16_t *limit=s+count;
        ചെയ്യുക {
            എങ്കിൽ(*(--limit)==c) {
                തിരികെ (char16_t *)limit;
            }
        } ആകുമ്പോൾ(s!=limit);
        തിരികെ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(സ്ഥിരം char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    എങ്കിൽ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        തിരികെ u_memrchr(s, (char16_t)c, count);
    } അല്ലെങ്കിൽ എങ്കിൽ(count<2) {
        /* too short for a surrogate pair */
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ എങ്കിൽ((uint32_t)c<=UCHAR_MAX_മൂല്യം) {
        /* find supplementary code point as surrogate pair */
        സ്ഥിരം char16_t *limit=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ചെയ്യുക {
            എങ്കിൽ(*limit==trail && *(limit-1)==lead) {
                തിരികെ (char16_t *)(limit-1);
            }
        } ആകുമ്പോൾ(s!=--limit);
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ {
        /* not a Unicode code point, not findable */
        തിരികെ nullptr;
    }
}

/* Tokenization functions --------------------------------------------------- */

/*
 * Match each code point in a string against each code point in the matchSet.
 * Return the index of the first string code point that
 * is (polarity==true) or is not (false) contained in the matchSet.
 * Return -(string length)-1 if there is no such code point.
 */
സ്ഥിതം int32_t
_matchFromSet(സ്ഥിരം char16_t *string, സ്ഥിരം char16_t *matchSet, UBool polarity) {
    int32_t matchLen, matchBMPLen, strItr, matchItr;
    UChar32 stringCh, matchCh;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    matchBMPLen = 0;
    ആകുമ്പോൾ((c = matchSet[matchBMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++matchBMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    matchLen = matchBMPLen;
    ആകുമ്പോൾ(matchSet[matchLen] != 0) {
        ++matchLen;
    }

    വേണ്ടി(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        എങ്കിൽ(U16_IS_SINGLE(c)) {
            എങ്കിൽ(polarity) {
                വേണ്ടി(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    എങ്കിൽ(c == matchSet[matchItr]) {
                        തിരികെ strItr - 1; /* one matches */
                    }
                }
            } അല്ലെങ്കിൽ {
                വേണ്ടി(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    എങ്കിൽ(c == matchSet[matchItr]) {
                        പോകുക endloop;
                    }
                }
                തിരികെ strItr - 1; /* none matches */
            }
        } അല്ലെങ്കിൽ {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            എങ്കിൽ(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } അല്ലെങ്കിൽ {
                stringCh = c; /* unpaired trail surrogate */
            }

            എങ്കിൽ(polarity) {
                വേണ്ടി(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    എങ്കിൽ(stringCh == matchCh) {
                        തിരികെ strItr - U16_LENGTH(stringCh); /* one matches */
                    }
                }
            } അല്ലെങ്കിൽ {
                വേണ്ടി(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    എങ്കിൽ(stringCh == matchCh) {
                        പോകുക endloop;
                    }
                }
                തിരികെ strItr - U16_LENGTH(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    തിരികെ -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(സ്ഥിരം char16_t* string U_LIFETIME_BOUND, സ്ഥിരം char16_t* matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, സത്യം);
    എങ്കിൽ(idx >= 0) {
        തിരികെ (char16_t *)string + idx;
    } അല്ലെങ്കിൽ {
        തിരികെ nullptr;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(സ്ഥിരം char16_t *string, സ്ഥിരം char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, സത്യം);
    എങ്കിൽ(idx >= 0) {
        തിരികെ idx;
    } അല്ലെങ്കിൽ {
        തിരികെ -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(സ്ഥിരം char16_t *string, സ്ഥിരം char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, അസത്യം);
    എങ്കിൽ(idx >= 0) {
        തിരികെ idx;
    } അല്ലെങ്കിൽ {
        തിരികെ -idx - 1; /* == u_strlen(string) */
    }
}

/* ----- Text manipulation functions --- */

U_CAPI char16_t* U_EXPORT2
u_strtok_r(char16_t* src U_LIFETIME_BOUND, സ്ഥിരം char16_t* delim, char16_t** saveState)
{
    char16_t *tokSource;
    char16_t *nextToken;
    uint32_t nonDelimIdx;

    /* If saveState is nullptr, the user messed up. */
    എങ്കിൽ (src != nullptr) {
        tokSource = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*saveState) {
        tokSource = *saveState;
    }
    അല്ലെങ്കിൽ {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        തിരികെ nullptr;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tokSource, delim);
    tokSource = &tokSource[nonDelimIdx];

    എങ്കിൽ (*tokSource) {
        nextToken = u_strpbrk(tokSource, delim);
        എങ്കിൽ (nextToken != nullptr) {
            /* Create a token */
            *(nextToken++) = 0;
            *saveState = nextToken;
            തിരികെ tokSource;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (*saveState) {
            /* Return the last token */
            *saveState = nullptr;
            തിരികെ tokSource;
        }
    }
    അല്ലെങ്കിൽ {
        /* No tokens were found. Only delimiters were left. */
        *saveState = nullptr;
    }
    തിരികെ nullptr;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, സ്ഥിരം char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    ആകുമ്പോൾ(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    ആകുമ്പോൾ((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    തിരികെ anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, സ്ഥിരം char16_t* src, int32_t n)
{
    എങ്കിൽ(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        ആകുമ്പോൾ(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        ആകുമ്പോൾ((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            എങ്കിൽ(--n == 0) {
                *dst = 0;
                നിർത്തുക;
            }
            ++src;
        }

        തിരികെ anchor;
    } അല്ലെങ്കിൽ {
        തിരികെ dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(സ്ഥിരം char16_t *s1,
    സ്ഥിരം char16_t *s2)
{
    char16_t  c1, c2;

    വേണ്ടി(;;) {
        c1=*s1++;
        c2=*s2++;
        എങ്കിൽ (c1 != c2 || c1 == 0) {
            നിർത്തുക;
        }
    }
    തിരികെ (int32_t)c1 - (int32_t)c2;
}

U_CFUNC int32_t U_EXPORT2
uprv_strCompare(സ്ഥിരം char16_t *s1, int32_t length1,
                സ്ഥിരം char16_t *s2, int32_t length2,
                UBool strncmpStyle, UBool codePointOrder) {
    സ്ഥിരം char16_t *start1, *start2, *limit1, *limit2;
    char16_t c1, c2;

    /* setup for fix-up */
    start1=s1;
    start2=s2;

    /* compare identical prefixes - they do not need to be fixed up */
    എങ്കിൽ(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        എങ്കിൽ(s1==s2) {
            തിരികെ 0;
        }

        വേണ്ടി(;;) {
            c1=*s1;
            c2=*s2;
            എങ്കിൽ(c1!=c2) {
                നിർത്തുക;
            }
            എങ്കിൽ(c1==0) {
                തിരികെ 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=nullptr;
    } അല്ലെങ്കിൽ എങ്കിൽ(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        എങ്കിൽ(s1==s2) {
            തിരികെ 0;
        }

        limit1=start1+length1;

        വേണ്ടി(;;) {
            /* both lengths are same, check only one limit */
            എങ്കിൽ(s1==limit1) {
                തിരികെ 0;
            }

            c1=*s1;
            c2=*s2;
            എങ്കിൽ(c1!=c2) {
                നിർത്തുക;
            }
            എങ്കിൽ(c1==0) {
                തിരികെ 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } അല്ലെങ്കിൽ {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t lengthResult;

        എങ്കിൽ(length1<0) {
            length1=u_strlen(s1);
        }
        എങ്കിൽ(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        എങ്കിൽ(length1<length2) {
            lengthResult=-1;
            limit1=start1+length1;
        } അല്ലെങ്കിൽ എങ്കിൽ(length1==length2) {
            lengthResult=0;
            limit1=start1+length1;
        } അല്ലെങ്കിൽ /* length1>length2 */ {
            lengthResult=1;
            limit1=start1+length2;
        }

        എങ്കിൽ(s1==s2) {
            തിരികെ lengthResult;
        }

        വേണ്ടി(;;) {
            /* check pseudo-limit */
            എങ്കിൽ(s1==limit1) {
                തിരികെ lengthResult;
            }

            c1=*s1;
            c2=*s2;
            എങ്കിൽ(c1!=c2) {
                നിർത്തുക;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    എങ്കിൽ(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        എങ്കിൽ(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } അല്ലെങ്കിൽ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        എങ്കിൽ(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } അല്ലെങ്കിൽ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    തിരികെ (int32_t)c1-(int32_t)c2;
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
    എങ്കിൽ(iter1==nullptr || iter2==nullptr) {
        തിരികെ 0; /* bad arguments */
    }
    എങ്കിൽ(iter1==iter2) {
        തിരികെ 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_START);
    iter2->move(iter2, 0, UITER_START);

    /* compare identical prefixes - they do not need to be fixed up */
    വേണ്ടി(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        എങ്കിൽ(c1!=c2) {
            നിർത്തുക;
        }
        എങ്കിൽ(c1==-1) {
            തിരികെ 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    എങ്കിൽ(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        എങ്കിൽ(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->previous(iter1), U16_IS_LEAD(iter1->previous(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } അല്ലെങ്കിൽ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        എങ്കിൽ(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->previous(iter2), U16_IS_LEAD(iter2->previous(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } അല്ലെങ്കിൽ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    തിരികെ (int32_t)c1-(int32_t)c2;
}

#എങ്കിൽ 0
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
ശൂന്യം fragment {
        /* iff a surrogate is part of a surrogate pair, leave >=d800 */
        എങ്കിൽ(c1<=0xdbff) {
            എങ്കിൽ(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } അല്ലെങ്കിൽ എങ്കിൽ(c1<=0xdfff) {
            int32_t idx=iter1->getIndex(iter1, UITER_CURRENT);
            iter1->previous(iter1); /* ==c1 */
            എങ്കിൽ(!U16_IS_LEAD(iter1->previous(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } അല്ലെങ്കിൽ /* 0xe000<=c1<=0xffff */ {
            /* BMP code point - make <d800 */
            c1-=0x2800;
        }
}
#endif

U_CAPI int32_t U_EXPORT2
u_strCompare(സ്ഥിരം char16_t *s1, int32_t length1,
             സ്ഥിരം char16_t *s2, int32_t length2,
             UBool codePointOrder) {
    /* argument checking */
    എങ്കിൽ(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        തിരികെ 0;
    }
    തിരികെ uprv_strCompare(s1, length1, s2, length2, അസത്യം, codePointOrder);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmpCodePointOrder(സ്ഥിരം char16_t *s1, സ്ഥിരം char16_t *s2) {
    തിരികെ uprv_strCompare(s1, -1, s2, -1, അസത്യം, സത്യം);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(സ്ഥിരം char16_t  *s1,
     സ്ഥിരം char16_t  *s2,
     int32_t     n) 
{
    എങ്കിൽ(n > 0) {
        int32_t rc;
        വേണ്ടി(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            എങ്കിൽ(rc != 0 || *s1 == 0 || --n == 0) {
                തിരികെ rc;
            }
            ++s1;
            ++s2;
        }
    } അല്ലെങ്കിൽ {
        തിരികെ 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmpCodePointOrder(സ്ഥിരം char16_t *s1, സ്ഥിരം char16_t *s2, int32_t n) {
    തിരികെ uprv_strCompare(s1, n, s2, n, സത്യം, സത്യം);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, സ്ഥിരം char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    ആകുമ്പോൾ((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    തിരികെ anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, സ്ഥിരം char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    ആകുമ്പോൾ(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    തിരികെ anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(സ്ഥിരം char16_t *s)
{
#എങ്കിൽ U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    തിരികെ (int32_t)uprv_wcslen((സ്ഥിരം wchar_t *)s);
#അല്ലെങ്കിൽ
    സ്ഥിരം char16_t *t = s;
    ആകുമ്പോൾ(*t != 0) {
      ++t;
    }
    തിരികെ t - s;
#endif
}

U_CAPI int32_t U_EXPORT2
u_എണ്ണം_Char32(സ്ഥിരം char16_t *s, int32_t length) {
    int32_t count;

    എങ്കിൽ(s==nullptr || length<-1) {
        തിരികെ 0;
    }

    count=0;
    എങ്കിൽ(length>=0) {
        ആകുമ്പോൾ(length>0) {
            ++count;
            എങ്കിൽ(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } അല്ലെങ്കിൽ {
                ++s;
                --length;
            }
        }
    } അല്ലെങ്കിൽ /* length==-1 */ {
        char16_t c;

        വേണ്ടി(;;) {
            എങ്കിൽ((c=*s++)==0) {
                നിർത്തുക;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            എങ്കിൽ(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    തിരികെ count;
}

U_CAPI UBool U_EXPORT2
u_strHasMoreChar32Than(സ്ഥിരം char16_t *s, int32_t length, int32_t number) {

    എങ്കിൽ(number<0) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ(s==nullptr || length<-1) {
        തിരികെ അസത്യം;
    }

    എങ്കിൽ(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        വേണ്ടി(;;) {
            എങ്കിൽ((c=*s++)==0) {
                തിരികെ അസത്യം;
            }
            എങ്കിൽ(number==0) {
                തിരികെ സത്യം;
            }
            എങ്കിൽ(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } അല്ലെങ്കിൽ {
        /* length>=0 known */
        സ്ഥിരം char16_t *limit;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        എങ്കിൽ(((length+1)/2)>number) {
            തിരികെ സത്യം;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        എങ്കിൽ(maxSupplementary<=0) {
            തിരികെ അസത്യം;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        limit=s+length;
        വേണ്ടി(;;) {
            എങ്കിൽ(s==limit) {
                തിരികെ അസത്യം;
            }
            എങ്കിൽ(number==0) {
                തിരികെ സത്യം;
            }
            എങ്കിൽ(U16_IS_LEAD(*s++) && s!=limit && U16_IS_TRAIL(*s)) {
                ++s;
                എങ്കിൽ(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    തിരികെ അസത്യം;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, സ്ഥിരം char16_t* src, int32_t count) {
    എങ്കിൽ(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    തിരികെ dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, സ്ഥിരം char16_t* src, int32_t count) {
    എങ്കിൽ(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    തിരികെ dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    എങ്കിൽ(count > 0) {
        char16_t *ptr = dest;
        char16_t *limit = dest + count;

        ആകുമ്പോൾ (ptr < limit) {
            *(ptr++) = c;
        }
    }
    തിരികെ dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(സ്ഥിരം char16_t *buf1, സ്ഥിരം char16_t *buf2, int32_t count) {
    എങ്കിൽ(count > 0) {
        സ്ഥിരം char16_t *limit = buf1 + count;
        int32_t result;

        ആകുമ്പോൾ (buf1 < limit) {
            result = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            എങ്കിൽ (result != 0) {
                തിരികെ result;
            }
            buf1++;
            buf2++;
        }
    }
    തിരികെ 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmpCodePointOrder(സ്ഥിരം char16_t *s1, സ്ഥിരം char16_t *s2, int32_t count) {
    തിരികെ uprv_strCompare(s1, count, s2, count, അസത്യം, സത്യം);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
സ്ഥിതം സ്ഥിരം char16_t UNESCAPE_MAP[] = {
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
എണ്ണൽ { UNESCAPE_MAP_LENGTH = UPRV_LENGTHOF(UNESCAPE_MAP) };

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
സ്ഥിതം int32_t _digit8(char16_t c) {
    എങ്കിൽ (c >= u'0' && c <= u'7') {
        തിരികെ c - u'0';
    }
    തിരികെ -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
സ്ഥിതം int32_t _digit16(char16_t c) {
    എങ്കിൽ (c >= u'0' && c <= u'9') {
        തിരികെ c - u'0';
    }
    എങ്കിൽ (c >= u'A' && c <= u'F') {
        തിരികെ c - (u'A' - 10);
    }
    എങ്കിൽ (c >= u'a' && c <= u'f') {
        തിരികെ c - (u'a' - 10);
    }
    തിരികെ -1;
}

/* Parse a single escape sequence.  Although this method deals in
 * UChars, it does not use C++ or UnicodeString.  This allows it to
 * be used from C contexts. */
U_CAPI UChar32 U_EXPORT2
u_unescapeAt(UNESCAPE_CHAR_AT charAt,
             int32_t *offset,
             int32_t length,
             ശൂന്യം *context) {

    int32_t start = *offset;
    UChar32 c;
    UChar32 result = 0;
    int8_t n = 0;
    int8_t minDig = 0;
    int8_t maxDig = 0;
    int8_t bitsPerDigit = 4; 
    int32_t dig;
    UBool braces = അസത്യം;

    /* Check that offset is in range */
    എങ്കിൽ (*offset < 0 || *offset >= length) {
        പോകുക err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*offset)++, context);

    /* Convert hexadecimal and octal escapes */
    തിരഞ്ഞെടുപ്പ് (c) {
    അവസ്ഥ u'u':
        minDig = maxDig = 4;
        നിർത്തുക;
    അവസ്ഥ u'U':
        minDig = maxDig = 8;
        നിർത്തുക;
    അവസ്ഥ u'x':
        minDig = 1;
        എങ്കിൽ (*offset < length && charAt(*offset, context) == u'{') {
            ++(*offset);
            braces = സത്യം;
            maxDig = 8;
        } അല്ലെങ്കിൽ {
            maxDig = 2;
        }
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        dig = _digit8(c);
        എങ്കിൽ (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            bitsPerDigit = 3;
            result = dig;
        }
        നിർത്തുക;
    }
    എങ്കിൽ (minDig != 0) {
        ആകുമ്പോൾ (*offset < length && n < maxDig) {
            c = charAt(*offset, context);
            dig = (bitsPerDigit == 3) ? _digit8(c) : _digit16(c);
            എങ്കിൽ (dig < 0) {
                നിർത്തുക;
            }
            result = (result << bitsPerDigit) | dig;
            ++(*offset);
            ++n;
        }
        എങ്കിൽ (n < minDig) {
            പോകുക err;
        }
        എങ്കിൽ (braces) {
            എങ്കിൽ (c != u'}') {
                പോകുക err;
            }
            ++(*offset);
        }
        എങ്കിൽ (result < 0 || result >= 0x110000) {
            പോകുക err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        എങ്കിൽ (*offset < length && U16_IS_LEAD(result)) {
            int32_t ahead = *offset + 1;
            c = charAt(*offset, context);
            എങ്കിൽ (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t tailLimit = ahead + 11;
                എങ്കിൽ (tailLimit > length) {
                    tailLimit = length;
                }
                c = u_unescapeAt(charAt, &ahead, tailLimit, context);
            }
            എങ്കിൽ (U16_IS_TRAIL(c)) {
                *offset = ahead;
                result = U16_GET_SUPPLEMENTARY(result, c);
            }
        }
        തിരികെ result;
    }

    /* Convert C-style escapes in table */
    വേണ്ടി (int32_t i=0; i<UNESCAPE_MAP_LENGTH; i+=2) {
        എങ്കിൽ (c == UNESCAPE_MAP[i]) {
            തിരികെ UNESCAPE_MAP[i+1];
        } അല്ലെങ്കിൽ എങ്കിൽ (c < UNESCAPE_MAP[i]) {
            നിർത്തുക;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    എങ്കിൽ (c == u'c' && *offset < length) {
        c = charAt((*offset)++, context);
        എങ്കിൽ (U16_IS_LEAD(c) && *offset < length) {
            char16_t c2 = charAt(*offset, context);
            എങ്കിൽ (U16_IS_TRAIL(c2)) {
                ++(*offset);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        തിരികെ 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    എങ്കിൽ (U16_IS_LEAD(c) && *offset < length) {
        char16_t c2 = charAt(*offset, context);
        എങ്കിൽ (U16_IS_TRAIL(c2)) {
            ++(*offset);
            തിരികെ U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    തിരികെ c;

 err:
    /* Invalid escape sequence */
    *offset = start; /* Reset to initial value */
    തിരികെ (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
സ്ഥിതം char16_t U_CALLCONV
_charPtr_charAt(int32_t offset, ശൂന്യം *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_charsToUChars(static_cast<അക്ഷരം*>(context) + offset, &c16, 1);
    തിരികെ c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
സ്ഥിതം ശൂന്യം _appendUChars(char16_t *dest, int32_t destCapacity,
                          സ്ഥിരം അക്ഷരം *src, int32_t srcLen) {
    എങ്കിൽ (destCapacity < 0) {
        destCapacity = 0;
    }
    എങ്കിൽ (srcLen > destCapacity) {
        srcLen = destCapacity;
    }
    u_charsToUChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(സ്ഥിരം അക്ഷരം *src, char16_t *dest, int32_t destCapacity) {
    സ്ഥിരം അക്ഷരം *segment = src;
    int32_t i = 0;
    അക്ഷരം c;

    ആകുമ്പോൾ ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        എങ്കിൽ (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            എങ്കിൽ (src != segment) {
                എങ്കിൽ (dest != nullptr) {
                    _appendUChars(dest + i, destCapacity - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<അക്ഷരം*>(src));
            എങ്കിൽ (lenParsed == 0) {
                പോകുക err;
            }
            src += lenParsed; /* advance past escape seq. */
            എങ്കിൽ (dest != nullptr && U16_LENGTH(c32) <= (destCapacity - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } അല്ലെങ്കിൽ {
                i += U16_LENGTH(c32);
            }
            segment = src;
        } അല്ലെങ്കിൽ {
            ++src;
        }
    }
    എങ്കിൽ (src != segment) {
        എങ്കിൽ (dest != nullptr) {
            _appendUChars(dest + i, destCapacity - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    എങ്കിൽ (dest != nullptr && i < destCapacity) {
        dest[i] = 0;
    }
    തിരികെ i;

 err:
    എങ്കിൽ (dest != nullptr && destCapacity > 0) {
        *dest = 0;
    }
    തിരികെ 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#define __TERMINATE_STRING(dest, destCapacity, length, pErrorCode) UPRV_BLOCK_MACRO_BEGIN { \
    എങ്കിൽ(pErrorCode!=nullptr && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        എങ്കിൽ(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } അല്ലെങ്കിൽ എങ്കിൽ(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            എങ്കിൽ(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } അല്ലെങ്കിൽ എങ്കിൽ(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } അല്ലെങ്കിൽ /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;                        \
        }                                                               \
    } \
} UPRV_BLOCK_MACRO_END

U_CAPI char16_t U_EXPORT2
u_asciiToUpper(char16_t c) {
    എങ്കിൽ (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    തിരികെ c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    തിരികെ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(അക്ഷരം *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    തിരികെ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    തിരികെ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    തിരികെ length;
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
    സ്ഥിരം TYPE *p = (സ്ഥിരം TYPE*) STR;        \
    എങ്കിൽ (p != nullptr) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        സ്ഥിരം TYPE *limit = p + len;          \
        ആകുമ്പോൾ (p<limit) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    തിരികെ static_cast<int32_t>(hash);        \
} UPRV_BLOCK_MACRO_END

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(സ്ഥിരം char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(സ്ഥിരം അക്ഷരം *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(സ്ഥിരം അക്ഷരം *str, int32_t length) {
    STRING_HASH(അക്ഷരം, str, length, (uint8_t)uprv_tolower(*p));
}
