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
स्थिर अन्तःस्थ UBool
isMatchAtCPBoundary(ध्रुव char16_t *start, ध्रुव char16_t *match, ध्रुव char16_t *matchLimit, ध्रुव char16_t *limit) {
    सचे(U16_IS_TRAIL(*match) && start!=match && U16_IS_LEAD(*(match-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        पटिनिवत्त मिच्छा;
    }
    सचे(U16_IS_LEAD(*(matchLimit-1)) && matchLimit!=limit && U16_IS_TRAIL(*matchLimit)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        पटिनिवत्त मिच्छा;
    }
    पटिनिवत्त सच्च;
}

U_CAPI char16_t* U_EXPORT2
u_strFindFirst(ध्रुव char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               ध्रुव char16_t* sub,
               int32_t subLength) {
    ध्रुव char16_t *start, *p, *q, *subLimit;
    char16_t c, cs, cq;

    सचे(sub==nullptr || subLength<-1) {
        पटिनिवत्त (char16_t *)s;
    }
    सचे(s==nullptr || length<-1) {
        पटिनिवत्त nullptr;
    }

    start=s;

    सचे(length<0 && subLength<0) {
        /* both strings are NUL-terminated */
        सचे((cs=*sub++)==0) {
            पटिनिवत्त (char16_t *)s;
        }
        सचे(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            पटिनिवत्त u_strchr(s, cs);
        }

        याव((c=*s++)!=0) {
            सचे(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                अत्थाय(;;) {
                    सचे((cq=*q)==0) {
                        सचे(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            पटिनिवत्त (char16_t *)(s-1); /* well-formed match */
                        } अञ्ञथा {
                            भङ्ग; /* no match because surrogate pair is split */
                        }
                    }
                    सचे((c=*p)==0) {
                        पटिनिवत्त nullptr; /* no match, and none possible after s */
                    }
                    सचे(c!=cq) {
                        भङ्ग; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        पटिनिवत्त nullptr;
    }

    सचे(subLength<0) {
        subLength=u_strlen(sub);
    }
    सचे(subLength==0) {
        पटिनिवत्त (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --subLength;
    subLimit=sub+subLength;

    सचे(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        पटिनिवत्त length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    सचे(length<0) {
        /* s is NUL-terminated */
        याव((c=*s++)!=0) {
            सचे(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                अत्थाय(;;) {
                    सचे(q==subLimit) {
                        सचे(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            पटिनिवत्त (char16_t *)(s-1); /* well-formed match */
                        } अञ्ञथा {
                            भङ्ग; /* no match because surrogate pair is split */
                        }
                    }
                    सचे((c=*p)==0) {
                        पटिनिवत्त nullptr; /* no match, and none possible after s */
                    }
                    सचे(c!=*q) {
                        भङ्ग; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } अञ्ञथा {
        ध्रुव char16_t *limit, *preLimit;

        /* subLength was decremented above */
        सचे(length<=subLength) {
            पटिनिवत्त nullptr; /* s is shorter than sub */
        }

        limit=s+length;

        /* the substring must start before preLimit */
        preLimit=limit-subLength;

        याव(s!=preLimit) {
            c=*s++;
            सचे(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                अत्थाय(;;) {
                    सचे(q==subLimit) {
                        सचे(isMatchAtCPBoundary(start, s-1, p, limit)) {
                            पटिनिवत्त (char16_t *)(s-1); /* well-formed match */
                        } अञ्ञथा {
                            भङ्ग; /* no match because surrogate pair is split */
                        }
                    }
                    सचे(*p!=*q) {
                        भङ्ग; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    पटिनिवत्त nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(ध्रुव char16_t* s U_LIFETIME_BOUND, ध्रुव char16_t* substring) {
    पटिनिवत्त u_strFindFirst(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(ध्रुव char16_t* s U_LIFETIME_BOUND, char16_t c) {
    सचे(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        पटिनिवत्त u_strFindFirst(s, -1, &c, 1);
    } अञ्ञथा {
        char16_t cs;

        /* trivial search for a BMP code point */
        अत्थाय(;;) {
            सचे((cs=*s)==c) {
                पटिनिवत्त (char16_t *)s;
            }
            सचे(cs==0) {
                पटिनिवत्त nullptr;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(ध्रुव char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    सचे((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        पटिनिवत्त u_strchr(s, (char16_t)c);
    } अञ्ञथा सचे((uint32_t)c<=UCHAR_MAX_मानम्) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        याव((cs=*s++)!=0) {
            सचे(cs==lead && *s==trail) {
                पटिनिवत्त (char16_t *)(s-1);
            }
        }
        पटिनिवत्त nullptr;
    } अञ्ञथा {
        /* not a Unicode code point, not findable */
        पटिनिवत्त nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(ध्रुव char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    सचे(count<=0) {
        पटिनिवत्त nullptr; /* no string */
    } अञ्ञथा सचे(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        पटिनिवत्त u_strFindFirst(s, count, &c, 1);
    } अञ्ञथा {
        /* trivial search for a BMP code point */
        ध्रुव char16_t *limit=s+count;
        करोहि {
            सचे(*s==c) {
                पटिनिवत्त (char16_t *)s;
            }
        } याव(++s!=limit);
        पटिनिवत्त nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(ध्रुव char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    सचे((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        पटिनिवत्त u_memchr(s, (char16_t)c, count);
    } अञ्ञथा सचे(count<2) {
        /* too short for a surrogate pair */
        पटिनिवत्त nullptr;
    } अञ्ञथा सचे((uint32_t)c<=UCHAR_MAX_मानम्) {
        /* find supplementary code point as surrogate pair */
        ध्रुव char16_t *limit=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        करोहि {
            सचे(*s==lead && *(s+1)==trail) {
                पटिनिवत्त (char16_t *)s;
            }
        } याव(++s!=limit);
        पटिनिवत्त nullptr;
    } अञ्ञथा {
        /* not a Unicode code point, not findable */
        पटिनिवत्त nullptr;
    }
}

/* Backward binary string search functions ---------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strFindLast(ध्रुव char16_t* s U_LIFETIME_BOUND,
              int32_t length,
              ध्रुव char16_t* sub,
              int32_t subLength) {
    ध्रुव char16_t *start, *limit, *p, *q, *subLimit;
    char16_t c, cs;

    सचे(sub==nullptr || subLength<-1) {
        पटिनिवत्त (char16_t *)s;
    }
    सचे(s==nullptr || length<-1) {
        पटिनिवत्त nullptr;
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

    सचे(subLength<0) {
        subLength=u_strlen(sub);
    }
    सचे(subLength==0) {
        पटिनिवत्त (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    subLimit=sub+subLength;
    cs=*(--subLimit);
    --subLength;

    सचे(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        पटिनिवत्त length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    सचे(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    सचे(length<=subLength) {
        पटिनिवत्त nullptr; /* s is shorter than sub */
    }

    start=s;
    limit=s+length;

    /* the substring must start no later than s+subLength */
    s+=subLength;

    याव(s!=limit) {
        c=*(--limit);
        सचे(c==cs) {
            /* found last substring char16_t, compare rest */
            p=limit;
            q=subLimit;
            अत्थाय(;;) {
                सचे(q==sub) {
                    सचे(isMatchAtCPBoundary(start, p, limit+1, start+length)) {
                        पटिनिवत्त (char16_t *)p; /* well-formed match */
                    } अञ्ञथा {
                        भङ्ग; /* no match because surrogate pair is split */
                    }
                }
                सचे(*(--p)!=*(--q)) {
                    भङ्ग; /* no match */
                }
            }
        }
    }

    /* not found */
    पटिनिवत्त nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(ध्रुव char16_t* s U_LIFETIME_BOUND, ध्रुव char16_t* substring) {
    पटिनिवत्त u_strFindLast(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(ध्रुव char16_t* s U_LIFETIME_BOUND, char16_t c) {
    सचे(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        पटिनिवत्त u_strFindLast(s, -1, &c, 1);
    } अञ्ञथा {
        ध्रुव char16_t *result=nullptr;
        char16_t cs;

        /* trivial search for a BMP code point */
        अत्थाय(;;) {
            सचे((cs=*s)==c) {
                result=s;
            }
            सचे(cs==0) {
                पटिनिवत्त (char16_t *)result;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(ध्रुव char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    सचे((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        पटिनिवत्त u_strrchr(s, (char16_t)c);
    } अञ्ञथा सचे((uint32_t)c<=UCHAR_MAX_मानम्) {
        /* find supplementary code point as surrogate pair */
        ध्रुव char16_t *result=nullptr;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        याव((cs=*s++)!=0) {
            सचे(cs==lead && *s==trail) {
                result=s-1;
            }
        }
        पटिनिवत्त (char16_t *)result;
    } अञ्ञथा {
        /* not a Unicode code point, not findable */
        पटिनिवत्त nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(ध्रुव char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    सचे(count<=0) {
        पटिनिवत्त nullptr; /* no string */
    } अञ्ञथा सचे(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        पटिनिवत्त u_strFindLast(s, count, &c, 1);
    } अञ्ञथा {
        /* trivial search for a BMP code point */
        ध्रुव char16_t *limit=s+count;
        करोहि {
            सचे(*(--limit)==c) {
                पटिनिवत्त (char16_t *)limit;
            }
        } याव(s!=limit);
        पटिनिवत्त nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(ध्रुव char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    सचे((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        पटिनिवत्त u_memrchr(s, (char16_t)c, count);
    } अञ्ञथा सचे(count<2) {
        /* too short for a surrogate pair */
        पटिनिवत्त nullptr;
    } अञ्ञथा सचे((uint32_t)c<=UCHAR_MAX_मानम्) {
        /* find supplementary code point as surrogate pair */
        ध्रुव char16_t *limit=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        करोहि {
            सचे(*limit==trail && *(limit-1)==lead) {
                पटिनिवत्त (char16_t *)(limit-1);
            }
        } याव(s!=--limit);
        पटिनिवत्त nullptr;
    } अञ्ञथा {
        /* not a Unicode code point, not findable */
        पटिनिवत्त nullptr;
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
_matchFromSet(ध्रुव char16_t *string, ध्रुव char16_t *matchSet, UBool polarity) {
    int32_t matchLen, matchBMPLen, strItr, matchItr;
    UChar32 stringCh, matchCh;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    matchBMPLen = 0;
    याव((c = matchSet[matchBMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++matchBMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    matchLen = matchBMPLen;
    याव(matchSet[matchLen] != 0) {
        ++matchLen;
    }

    अत्थाय(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        सचे(U16_IS_SINGLE(c)) {
            सचे(polarity) {
                अत्थाय(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    सचे(c == matchSet[matchItr]) {
                        पटिनिवत्त strItr - 1; /* one matches */
                    }
                }
            } अञ्ञथा {
                अत्थाय(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    सचे(c == matchSet[matchItr]) {
                        गच्छ endloop;
                    }
                }
                पटिनिवत्त strItr - 1; /* none matches */
            }
        } अञ्ञथा {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            सचे(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } अञ्ञथा {
                stringCh = c; /* unpaired trail surrogate */
            }

            सचे(polarity) {
                अत्थाय(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    सचे(stringCh == matchCh) {
                        पटिनिवत्त strItr - U16_LENGTH(stringCh); /* one matches */
                    }
                }
            } अञ्ञथा {
                अत्थाय(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    सचे(stringCh == matchCh) {
                        गच्छ endloop;
                    }
                }
                पटिनिवत्त strItr - U16_LENGTH(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    पटिनिवत्त -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(ध्रुव char16_t* string U_LIFETIME_BOUND, ध्रुव char16_t* matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, सच्च);
    सचे(idx >= 0) {
        पटिनिवत्त (char16_t *)string + idx;
    } अञ्ञथा {
        पटिनिवत्त nullptr;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(ध्रुव char16_t *string, ध्रुव char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, सच्च);
    सचे(idx >= 0) {
        पटिनिवत्त idx;
    } अञ्ञथा {
        पटिनिवत्त -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(ध्रुव char16_t *string, ध्रुव char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, मिच्छा);
    सचे(idx >= 0) {
        पटिनिवत्त idx;
    } अञ्ञथा {
        पटिनिवत्त -idx - 1; /* == u_strlen(string) */
    }
}

/* ----- Text manipulation functions --- */

U_CAPI char16_t* U_EXPORT2
u_strtok_r(char16_t* src U_LIFETIME_BOUND, ध्रुव char16_t* delim, char16_t** saveState)
{
    char16_t *tokSource;
    char16_t *nextToken;
    uint32_t nonDelimIdx;

    /* If saveState is nullptr, the user messed up. */
    सचे (src != nullptr) {
        tokSource = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    अञ्ञथा सचे (*saveState) {
        tokSource = *saveState;
    }
    अञ्ञथा {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        पटिनिवत्त nullptr;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tokSource, delim);
    tokSource = &tokSource[nonDelimIdx];

    सचे (*tokSource) {
        nextToken = u_strpbrk(tokSource, delim);
        सचे (nextToken != nullptr) {
            /* Create a token */
            *(nextToken++) = 0;
            *saveState = nextToken;
            पटिनिवत्त tokSource;
        }
        अञ्ञथा सचे (*saveState) {
            /* Return the last token */
            *saveState = nullptr;
            पटिनिवत्त tokSource;
        }
    }
    अञ्ञथा {
        /* No tokens were found. Only delimiters were left. */
        *saveState = nullptr;
    }
    पटिनिवत्त nullptr;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, ध्रुव char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    याव(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    याव((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    पटिनिवत्त anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, ध्रुव char16_t* src, int32_t n)
{
    सचे(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        याव(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        याव((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            सचे(--n == 0) {
                *dst = 0;
                भङ्ग;
            }
            ++src;
        }

        पटिनिवत्त anchor;
    } अञ्ञथा {
        पटिनिवत्त dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(ध्रुव char16_t *s1,
    ध्रुव char16_t *s2)
{
    char16_t  c1, c2;

    अत्थाय(;;) {
        c1=*s1++;
        c2=*s2++;
        सचे (c1 != c2 || c1 == 0) {
            भङ्ग;
        }
    }
    पटिनिवत्त (int32_t)c1 - (int32_t)c2;
}

U_CFUNC int32_t U_EXPORT2
uprv_strCompare(ध्रुव char16_t *s1, int32_t length1,
                ध्रुव char16_t *s2, int32_t length2,
                UBool strncmpStyle, UBool codePointOrder) {
    ध्रुव char16_t *start1, *start2, *limit1, *limit2;
    char16_t c1, c2;

    /* setup for fix-up */
    start1=s1;
    start2=s2;

    /* compare identical prefixes - they do not need to be fixed up */
    सचे(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        सचे(s1==s2) {
            पटिनिवत्त 0;
        }

        अत्थाय(;;) {
            c1=*s1;
            c2=*s2;
            सचे(c1!=c2) {
                भङ्ग;
            }
            सचे(c1==0) {
                पटिनिवत्त 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=nullptr;
    } अञ्ञथा सचे(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        सचे(s1==s2) {
            पटिनिवत्त 0;
        }

        limit1=start1+length1;

        अत्थाय(;;) {
            /* both lengths are same, check only one limit */
            सचे(s1==limit1) {
                पटिनिवत्त 0;
            }

            c1=*s1;
            c2=*s2;
            सचे(c1!=c2) {
                भङ्ग;
            }
            सचे(c1==0) {
                पटिनिवत्त 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } अञ्ञथा {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t lengthResult;

        सचे(length1<0) {
            length1=u_strlen(s1);
        }
        सचे(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        सचे(length1<length2) {
            lengthResult=-1;
            limit1=start1+length1;
        } अञ्ञथा सचे(length1==length2) {
            lengthResult=0;
            limit1=start1+length1;
        } अञ्ञथा /* length1>length2 */ {
            lengthResult=1;
            limit1=start1+length2;
        }

        सचे(s1==s2) {
            पटिनिवत्त lengthResult;
        }

        अत्थाय(;;) {
            /* check pseudo-limit */
            सचे(s1==limit1) {
                पटिनिवत्त lengthResult;
            }

            c1=*s1;
            c2=*s2;
            सचे(c1!=c2) {
                भङ्ग;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    सचे(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        सचे(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } अञ्ञथा {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        सचे(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } अञ्ञथा {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    पटिनिवत्त (int32_t)c1-(int32_t)c2;
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
    सचे(iter1==nullptr || iter2==nullptr) {
        पटिनिवत्त 0; /* bad arguments */
    }
    सचे(iter1==iter2) {
        पटिनिवत्त 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_START);
    iter2->move(iter2, 0, UITER_START);

    /* compare identical prefixes - they do not need to be fixed up */
    अत्थाय(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        सचे(c1!=c2) {
            भङ्ग;
        }
        सचे(c1==-1) {
            पटिनिवत्त 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    सचे(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        सचे(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->previous(iter1), U16_IS_LEAD(iter1->previous(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } अञ्ञथा {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        सचे(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->previous(iter2), U16_IS_LEAD(iter2->previous(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } अञ्ञथा {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    पटिनिवत्त (int32_t)c1-(int32_t)c2;
}

#सचे 0
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
        सचे(c1<=0xdbff) {
            सचे(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } अञ्ञथा सचे(c1<=0xdfff) {
            int32_t idx=iter1->getIndex(iter1, UITER_CURRENT);
            iter1->previous(iter1); /* ==c1 */
            सचे(!U16_IS_LEAD(iter1->previous(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } अञ्ञथा /* 0xe000<=c1<=0xffff */ {
            /* BMP code point - make <d800 */
            c1-=0x2800;
        }
}
#endif

U_CAPI int32_t U_EXPORT2
u_strCompare(ध्रुव char16_t *s1, int32_t length1,
             ध्रुव char16_t *s2, int32_t length2,
             UBool codePointOrder) {
    /* argument checking */
    सचे(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        पटिनिवत्त 0;
    }
    पटिनिवत्त uprv_strCompare(s1, length1, s2, length2, मिच्छा, codePointOrder);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmpCodePointOrder(ध्रुव char16_t *s1, ध्रुव char16_t *s2) {
    पटिनिवत्त uprv_strCompare(s1, -1, s2, -1, मिच्छा, सच्च);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(ध्रुव char16_t  *s1,
     ध्रुव char16_t  *s2,
     int32_t     n) 
{
    सचे(n > 0) {
        int32_t rc;
        अत्थाय(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            सचे(rc != 0 || *s1 == 0 || --n == 0) {
                पटिनिवत्त rc;
            }
            ++s1;
            ++s2;
        }
    } अञ्ञथा {
        पटिनिवत्त 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmpCodePointOrder(ध्रुव char16_t *s1, ध्रुव char16_t *s2, int32_t n) {
    पटिनिवत्त uprv_strCompare(s1, n, s2, n, सच्च, सच्च);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, ध्रुव char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    याव((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    पटिनिवत्त anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, ध्रुव char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    याव(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    पटिनिवत्त anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(ध्रुव char16_t *s)
{
#सचे U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    पटिनिवत्त (int32_t)uprv_wcslen((ध्रुव wchar_t *)s);
#अञ्ञथा
    ध्रुव char16_t *t = s;
    याव(*t != 0) {
      ++t;
    }
    पटिनिवत्त t - s;
#endif
}

U_CAPI int32_t U_EXPORT2
u_गणनम्_Char32(ध्रुव char16_t *s, int32_t length) {
    int32_t count;

    सचे(s==nullptr || length<-1) {
        पटिनिवत्त 0;
    }

    count=0;
    सचे(length>=0) {
        याव(length>0) {
            ++count;
            सचे(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } अञ्ञथा {
                ++s;
                --length;
            }
        }
    } अञ्ञथा /* length==-1 */ {
        char16_t c;

        अत्थाय(;;) {
            सचे((c=*s++)==0) {
                भङ्ग;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            सचे(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    पटिनिवत्त count;
}

U_CAPI UBool U_EXPORT2
u_strHasMoreChar32Than(ध्रुव char16_t *s, int32_t length, int32_t number) {

    सचे(number<0) {
        पटिनिवत्त सच्च;
    }
    सचे(s==nullptr || length<-1) {
        पटिनिवत्त मिच्छा;
    }

    सचे(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        अत्थाय(;;) {
            सचे((c=*s++)==0) {
                पटिनिवत्त मिच्छा;
            }
            सचे(number==0) {
                पटिनिवत्त सच्च;
            }
            सचे(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } अञ्ञथा {
        /* length>=0 known */
        ध्रुव char16_t *limit;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        सचे(((length+1)/2)>number) {
            पटिनिवत्त सच्च;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        सचे(maxSupplementary<=0) {
            पटिनिवत्त मिच्छा;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        limit=s+length;
        अत्थाय(;;) {
            सचे(s==limit) {
                पटिनिवत्त मिच्छा;
            }
            सचे(number==0) {
                पटिनिवत्त सच्च;
            }
            सचे(U16_IS_LEAD(*s++) && s!=limit && U16_IS_TRAIL(*s)) {
                ++s;
                सचे(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    पटिनिवत्त मिच्छा;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, ध्रुव char16_t* src, int32_t count) {
    सचे(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    पटिनिवत्त dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, ध्रुव char16_t* src, int32_t count) {
    सचे(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    पटिनिवत्त dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    सचे(count > 0) {
        char16_t *ptr = dest;
        char16_t *limit = dest + count;

        याव (ptr < limit) {
            *(ptr++) = c;
        }
    }
    पटिनिवत्त dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(ध्रुव char16_t *buf1, ध्रुव char16_t *buf2, int32_t count) {
    सचे(count > 0) {
        ध्रुव char16_t *limit = buf1 + count;
        int32_t result;

        याव (buf1 < limit) {
            result = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            सचे (result != 0) {
                पटिनिवत्त result;
            }
            buf1++;
            buf2++;
        }
    }
    पटिनिवत्त 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmpCodePointOrder(ध्रुव char16_t *s1, ध्रुव char16_t *s2, int32_t count) {
    पटिनिवत्त uprv_strCompare(s1, count, s2, count, मिच्छा, सच्च);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
स्थिर ध्रुव char16_t UNESCAPE_MAP[] = {
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
    सचे (c >= u'0' && c <= u'7') {
        पटिनिवत्त c - u'0';
    }
    पटिनिवत्त -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
स्थिर int32_t _digit16(char16_t c) {
    सचे (c >= u'0' && c <= u'9') {
        पटिनिवत्त c - u'0';
    }
    सचे (c >= u'A' && c <= u'F') {
        पटिनिवत्त c - (u'A' - 10);
    }
    सचे (c >= u'a' && c <= u'f') {
        पटिनिवत्त c - (u'a' - 10);
    }
    पटिनिवत्त -1;
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
    UBool braces = मिच्छा;

    /* Check that offset is in range */
    सचे (*offset < 0 || *offset >= length) {
        गच्छ err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*offset)++, context);

    /* Convert hexadecimal and octal escapes */
    विकल्प (c) {
    पक्ष u'u':
        minDig = maxDig = 4;
        भङ्ग;
    पक्ष u'U':
        minDig = maxDig = 8;
        भङ्ग;
    पक्ष u'x':
        minDig = 1;
        सचे (*offset < length && charAt(*offset, context) == u'{') {
            ++(*offset);
            braces = सच्च;
            maxDig = 8;
        } अञ्ञथा {
            maxDig = 2;
        }
        भङ्ग;
    स्वभाव:
        dig = _digit8(c);
        सचे (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            bitsPerDigit = 3;
            result = dig;
        }
        भङ्ग;
    }
    सचे (minDig != 0) {
        याव (*offset < length && n < maxDig) {
            c = charAt(*offset, context);
            dig = (bitsPerDigit == 3) ? _digit8(c) : _digit16(c);
            सचे (dig < 0) {
                भङ्ग;
            }
            result = (result << bitsPerDigit) | dig;
            ++(*offset);
            ++n;
        }
        सचे (n < minDig) {
            गच्छ err;
        }
        सचे (braces) {
            सचे (c != u'}') {
                गच्छ err;
            }
            ++(*offset);
        }
        सचे (result < 0 || result >= 0x110000) {
            गच्छ err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        सचे (*offset < length && U16_IS_LEAD(result)) {
            int32_t ahead = *offset + 1;
            c = charAt(*offset, context);
            सचे (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t tailLimit = ahead + 11;
                सचे (tailLimit > length) {
                    tailLimit = length;
                }
                c = u_unescapeAt(charAt, &ahead, tailLimit, context);
            }
            सचे (U16_IS_TRAIL(c)) {
                *offset = ahead;
                result = U16_GET_SUPPLEMENTARY(result, c);
            }
        }
        पटिनिवत्त result;
    }

    /* Convert C-style escapes in table */
    अत्थाय (int32_t i=0; i<UNESCAPE_MAP_LENGTH; i+=2) {
        सचे (c == UNESCAPE_MAP[i]) {
            पटिनिवत्त UNESCAPE_MAP[i+1];
        } अञ्ञथा सचे (c < UNESCAPE_MAP[i]) {
            भङ्ग;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    सचे (c == u'c' && *offset < length) {
        c = charAt((*offset)++, context);
        सचे (U16_IS_LEAD(c) && *offset < length) {
            char16_t c2 = charAt(*offset, context);
            सचे (U16_IS_TRAIL(c2)) {
                ++(*offset);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        पटिनिवत्त 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    सचे (U16_IS_LEAD(c) && *offset < length) {
        char16_t c2 = charAt(*offset, context);
        सचे (U16_IS_TRAIL(c2)) {
            ++(*offset);
            पटिनिवत्त U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    पटिनिवत्त c;

 err:
    /* Invalid escape sequence */
    *offset = start; /* Reset to initial value */
    पटिनिवत्त (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
स्थिर char16_t U_CALLCONV
_charPtr_charAt(int32_t offset, शून्य *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_charsToUChars(static_cast<वर्ण*>(context) + offset, &c16, 1);
    पटिनिवत्त c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
स्थिर शून्य _appendUChars(char16_t *dest, int32_t destCapacity,
                          ध्रुव वर्ण *src, int32_t srcLen) {
    सचे (destCapacity < 0) {
        destCapacity = 0;
    }
    सचे (srcLen > destCapacity) {
        srcLen = destCapacity;
    }
    u_charsToUChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(ध्रुव वर्ण *src, char16_t *dest, int32_t destCapacity) {
    ध्रुव वर्ण *segment = src;
    int32_t i = 0;
    वर्ण c;

    याव ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        सचे (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            सचे (src != segment) {
                सचे (dest != nullptr) {
                    _appendUChars(dest + i, destCapacity - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<वर्ण*>(src));
            सचे (lenParsed == 0) {
                गच्छ err;
            }
            src += lenParsed; /* advance past escape seq. */
            सचे (dest != nullptr && U16_LENGTH(c32) <= (destCapacity - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } अञ्ञथा {
                i += U16_LENGTH(c32);
            }
            segment = src;
        } अञ्ञथा {
            ++src;
        }
    }
    सचे (src != segment) {
        सचे (dest != nullptr) {
            _appendUChars(dest + i, destCapacity - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    सचे (dest != nullptr && i < destCapacity) {
        dest[i] = 0;
    }
    पटिनिवत्त i;

 err:
    सचे (dest != nullptr && destCapacity > 0) {
        *dest = 0;
    }
    पटिनिवत्त 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#define __TERMINATE_STRING(dest, destCapacity, length, pErrorCode) UPRV_BLOCK_MACRO_BEGIN { \
    सचे(pErrorCode!=nullptr && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        सचे(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } अञ्ञथा सचे(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            सचे(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } अञ्ञथा सचे(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } अञ्ञथा /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;                        \
        }                                                               \
    } \
} UPRV_BLOCK_MACRO_END

U_CAPI char16_t U_EXPORT2
u_asciiToUpper(char16_t c) {
    सचे (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    पटिनिवत्त c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    पटिनिवत्त length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(वर्ण *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    पटिनिवत्त length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    पटिनिवत्त length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    पटिनिवत्त length;
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
    ध्रुव TYPE *p = (ध्रुव TYPE*) STR;        \
    सचे (p != nullptr) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        ध्रुव TYPE *limit = p + len;          \
        याव (p<limit) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    पटिनिवत्त static_cast<int32_t>(hash);        \
} UPRV_BLOCK_MACRO_END

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(ध्रुव char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(ध्रुव वर्ण *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(ध्रुव वर्ण *str, int32_t length) {
    STRING_HASH(वर्ण, str, length, (uint8_t)uprv_tolower(*p));
}
