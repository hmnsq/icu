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

#ضمن "unicode/utypes.h"
#ضمن "unicode/putil.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/utf16.h"
#ضمن "cstring.h"
#ضمن "cwchar.h"
#ضمن "cmemory.h"
#ضمن "ustr_imp.h"

/* ANSI string.h - style functions ------------------------------------------ */

/* U+ffff is the highest BMP code point, the highest one that fits into a 16-bit char16_t */
#define U_BMP_MAX 0xffff

/* Forward binary string search functions ----------------------------------- */

/*
 * Test if a substring match inside a string is at code point boundaries.
 * All pointers refer to the same buffer.
 * The limit pointer may be nullptr, all others must be real pointers.
 */
ساكن مضمن UBool
isMatchAtCPBoundary(ثابت char16_t *start, ثابت char16_t *match, ثابت char16_t *matchLimit, ثابت char16_t *limit) {
    ܐܢ(U16_IS_TRAIL(*match) && start!=match && U16_IS_LEAD(*(match-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        ܦܢܐ ܕܓܠܐ;
    }
    ܐܢ(U16_IS_LEAD(*(matchLimit-1)) && matchLimit!=limit && U16_IS_TRAIL(*matchLimit)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        ܦܢܐ ܕܓܠܐ;
    }
    ܦܢܐ ܫܪܪܐ;
}

U_CAPI char16_t* U_EXPORT2
u_strFindFirst(ثابت char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               ثابت char16_t* sub,
               int32_t subLength) {
    ثابت char16_t *start, *p, *q, *subLimit;
    char16_t c, cs, cq;

    ܐܢ(sub==nullptr || subLength<-1) {
        ܦܢܐ (char16_t *)s;
    }
    ܐܢ(s==nullptr || length<-1) {
        ܦܢܐ nullptr;
    }

    start=s;

    ܐܢ(length<0 && subLength<0) {
        /* both strings are NUL-terminated */
        ܐܢ((cs=*sub++)==0) {
            ܦܢܐ (char16_t *)s;
        }
        ܐܢ(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            ܦܢܐ u_strchr(s, cs);
        }

        ܟܡܐ((c=*s++)!=0) {
            ܐܢ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                لكل(;;) {
                    ܐܢ((cq=*q)==0) {
                        ܐܢ(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            ܦܢܐ (char16_t *)(s-1); /* well-formed match */
                        } ܐܚܪܢܐ {
                            اكسر; /* no match because surrogate pair is split */
                        }
                    }
                    ܐܢ((c=*p)==0) {
                        ܦܢܐ nullptr; /* no match, and none possible after s */
                    }
                    ܐܢ(c!=cq) {
                        اكسر; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        ܦܢܐ nullptr;
    }

    ܐܢ(subLength<0) {
        subLength=u_strlen(sub);
    }
    ܐܢ(subLength==0) {
        ܦܢܐ (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --subLength;
    subLimit=sub+subLength;

    ܐܢ(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        ܦܢܐ length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    ܐܢ(length<0) {
        /* s is NUL-terminated */
        ܟܡܐ((c=*s++)!=0) {
            ܐܢ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                لكل(;;) {
                    ܐܢ(q==subLimit) {
                        ܐܢ(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            ܦܢܐ (char16_t *)(s-1); /* well-formed match */
                        } ܐܚܪܢܐ {
                            اكسر; /* no match because surrogate pair is split */
                        }
                    }
                    ܐܢ((c=*p)==0) {
                        ܦܢܐ nullptr; /* no match, and none possible after s */
                    }
                    ܐܢ(c!=*q) {
                        اكسر; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } ܐܚܪܢܐ {
        ثابت char16_t *limit, *preLimit;

        /* subLength was decremented above */
        ܐܢ(length<=subLength) {
            ܦܢܐ nullptr; /* s is shorter than sub */
        }

        limit=s+length;

        /* the substring must start before preLimit */
        preLimit=limit-subLength;

        ܟܡܐ(s!=preLimit) {
            c=*s++;
            ܐܢ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                لكل(;;) {
                    ܐܢ(q==subLimit) {
                        ܐܢ(isMatchAtCPBoundary(start, s-1, p, limit)) {
                            ܦܢܐ (char16_t *)(s-1); /* well-formed match */
                        } ܐܚܪܢܐ {
                            اكسر; /* no match because surrogate pair is split */
                        }
                    }
                    ܐܢ(*p!=*q) {
                        اكسر; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    ܦܢܐ nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(ثابت char16_t* s U_LIFETIME_BOUND, ثابت char16_t* substring) {
    ܦܢܐ u_strFindFirst(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(ثابت char16_t* s U_LIFETIME_BOUND, char16_t c) {
    ܐܢ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ܦܢܐ u_strFindFirst(s, -1, &c, 1);
    } ܐܚܪܢܐ {
        char16_t cs;

        /* trivial search for a BMP code point */
        لكل(;;) {
            ܐܢ((cs=*s)==c) {
                ܦܢܐ (char16_t *)s;
            }
            ܐܢ(cs==0) {
                ܦܢܐ nullptr;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(ثابت char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    ܐܢ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ܦܢܐ u_strchr(s, (char16_t)c);
    } ܐܚܪܢܐ ܐܢ((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ܟܡܐ((cs=*s++)!=0) {
            ܐܢ(cs==lead && *s==trail) {
                ܦܢܐ (char16_t *)(s-1);
            }
        }
        ܦܢܐ nullptr;
    } ܐܚܪܢܐ {
        /* not a Unicode code point, not findable */
        ܦܢܐ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(ثابت char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ܐܢ(count<=0) {
        ܦܢܐ nullptr; /* no string */
    } ܐܚܪܢܐ ܐܢ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ܦܢܐ u_strFindFirst(s, count, &c, 1);
    } ܐܚܪܢܐ {
        /* trivial search for a BMP code point */
        ثابت char16_t *limit=s+count;
        ܥܒܕ {
            ܐܢ(*s==c) {
                ܦܢܐ (char16_t *)s;
            }
        } ܟܡܐ(++s!=limit);
        ܦܢܐ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(ثابت char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    ܐܢ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ܦܢܐ u_memchr(s, (char16_t)c, count);
    } ܐܚܪܢܐ ܐܢ(count<2) {
        /* too short for a surrogate pair */
        ܦܢܐ nullptr;
    } ܐܚܪܢܐ ܐܢ((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        ثابت char16_t *limit=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ܥܒܕ {
            ܐܢ(*s==lead && *(s+1)==trail) {
                ܦܢܐ (char16_t *)s;
            }
        } ܟܡܐ(++s!=limit);
        ܦܢܐ nullptr;
    } ܐܚܪܢܐ {
        /* not a Unicode code point, not findable */
        ܦܢܐ nullptr;
    }
}

/* Backward binary string search functions ---------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strFindLast(ثابت char16_t* s U_LIFETIME_BOUND,
              int32_t length,
              ثابت char16_t* sub,
              int32_t subLength) {
    ثابت char16_t *start, *limit, *p, *q, *subLimit;
    char16_t c, cs;

    ܐܢ(sub==nullptr || subLength<-1) {
        ܦܢܐ (char16_t *)s;
    }
    ܐܢ(s==nullptr || length<-1) {
        ܦܢܐ nullptr;
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

    ܐܢ(subLength<0) {
        subLength=u_strlen(sub);
    }
    ܐܢ(subLength==0) {
        ܦܢܐ (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    subLimit=sub+subLength;
    cs=*(--subLimit);
    --subLength;

    ܐܢ(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        ܦܢܐ length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    ܐܢ(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    ܐܢ(length<=subLength) {
        ܦܢܐ nullptr; /* s is shorter than sub */
    }

    start=s;
    limit=s+length;

    /* the substring must start no later than s+subLength */
    s+=subLength;

    ܟܡܐ(s!=limit) {
        c=*(--limit);
        ܐܢ(c==cs) {
            /* found last substring char16_t, compare rest */
            p=limit;
            q=subLimit;
            لكل(;;) {
                ܐܢ(q==sub) {
                    ܐܢ(isMatchAtCPBoundary(start, p, limit+1, start+length)) {
                        ܦܢܐ (char16_t *)p; /* well-formed match */
                    } ܐܚܪܢܐ {
                        اكسر; /* no match because surrogate pair is split */
                    }
                }
                ܐܢ(*(--p)!=*(--q)) {
                    اكسر; /* no match */
                }
            }
        }
    }

    /* not found */
    ܦܢܐ nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(ثابت char16_t* s U_LIFETIME_BOUND, ثابت char16_t* substring) {
    ܦܢܐ u_strFindLast(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(ثابت char16_t* s U_LIFETIME_BOUND, char16_t c) {
    ܐܢ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ܦܢܐ u_strFindLast(s, -1, &c, 1);
    } ܐܚܪܢܐ {
        ثابت char16_t *result=nullptr;
        char16_t cs;

        /* trivial search for a BMP code point */
        لكل(;;) {
            ܐܢ((cs=*s)==c) {
                result=s;
            }
            ܐܢ(cs==0) {
                ܦܢܐ (char16_t *)result;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(ثابت char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    ܐܢ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ܦܢܐ u_strrchr(s, (char16_t)c);
    } ܐܚܪܢܐ ܐܢ((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        ثابت char16_t *result=nullptr;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ܟܡܐ((cs=*s++)!=0) {
            ܐܢ(cs==lead && *s==trail) {
                result=s-1;
            }
        }
        ܦܢܐ (char16_t *)result;
    } ܐܚܪܢܐ {
        /* not a Unicode code point, not findable */
        ܦܢܐ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(ثابت char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ܐܢ(count<=0) {
        ܦܢܐ nullptr; /* no string */
    } ܐܚܪܢܐ ܐܢ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ܦܢܐ u_strFindLast(s, count, &c, 1);
    } ܐܚܪܢܐ {
        /* trivial search for a BMP code point */
        ثابت char16_t *limit=s+count;
        ܥܒܕ {
            ܐܢ(*(--limit)==c) {
                ܦܢܐ (char16_t *)limit;
            }
        } ܟܡܐ(s!=limit);
        ܦܢܐ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(ثابت char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    ܐܢ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ܦܢܐ u_memrchr(s, (char16_t)c, count);
    } ܐܚܪܢܐ ܐܢ(count<2) {
        /* too short for a surrogate pair */
        ܦܢܐ nullptr;
    } ܐܚܪܢܐ ܐܢ((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        ثابت char16_t *limit=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ܥܒܕ {
            ܐܢ(*limit==trail && *(limit-1)==lead) {
                ܦܢܐ (char16_t *)(limit-1);
            }
        } ܟܡܐ(s!=--limit);
        ܦܢܐ nullptr;
    } ܐܚܪܢܐ {
        /* not a Unicode code point, not findable */
        ܦܢܐ nullptr;
    }
}

/* Tokenization functions --------------------------------------------------- */

/*
 * Match each code point in a string against each code point in the matchSet.
 * Return the index of the first string code point that
 * is (polarity==true) or is not (false) contained in the matchSet.
 * Return -(string length)-1 if there is no such code point.
 */
ساكن int32_t
_matchFromSet(ثابت char16_t *string, ثابت char16_t *matchSet, UBool polarity) {
    int32_t matchLen, matchBMPLen, strItr, matchItr;
    UChar32 stringCh, matchCh;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    matchBMPLen = 0;
    ܟܡܐ((c = matchSet[matchBMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++matchBMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    matchLen = matchBMPLen;
    ܟܡܐ(matchSet[matchLen] != 0) {
        ++matchLen;
    }

    لكل(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        ܐܢ(U16_IS_SINGLE(c)) {
            ܐܢ(polarity) {
                لكل(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    ܐܢ(c == matchSet[matchItr]) {
                        ܦܢܐ strItr - 1; /* one matches */
                    }
                }
            } ܐܚܪܢܐ {
                لكل(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    ܐܢ(c == matchSet[matchItr]) {
                        اذهب endloop;
                    }
                }
                ܦܢܐ strItr - 1; /* none matches */
            }
        } ܐܚܪܢܐ {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            ܐܢ(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } ܐܚܪܢܐ {
                stringCh = c; /* unpaired trail surrogate */
            }

            ܐܢ(polarity) {
                لكل(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    ܐܢ(stringCh == matchCh) {
                        ܦܢܐ strItr - U16_LENGTH(stringCh); /* one matches */
                    }
                }
            } ܐܚܪܢܐ {
                لكل(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    ܐܢ(stringCh == matchCh) {
                        اذهب endloop;
                    }
                }
                ܦܢܐ strItr - U16_LENGTH(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    ܦܢܐ -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(ثابت char16_t* string U_LIFETIME_BOUND, ثابت char16_t* matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, ܫܪܪܐ);
    ܐܢ(idx >= 0) {
        ܦܢܐ (char16_t *)string + idx;
    } ܐܚܪܢܐ {
        ܦܢܐ nullptr;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(ثابت char16_t *string, ثابت char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, ܫܪܪܐ);
    ܐܢ(idx >= 0) {
        ܦܢܐ idx;
    } ܐܚܪܢܐ {
        ܦܢܐ -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(ثابت char16_t *string, ثابت char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, ܕܓܠܐ);
    ܐܢ(idx >= 0) {
        ܦܢܐ idx;
    } ܐܚܪܢܐ {
        ܦܢܐ -idx - 1; /* == u_strlen(string) */
    }
}

/* ----- Text manipulation functions --- */

U_CAPI char16_t* U_EXPORT2
u_strtok_r(char16_t* src U_LIFETIME_BOUND, ثابت char16_t* delim, char16_t** saveState)
{
    char16_t *tokSource;
    char16_t *nextToken;
    uint32_t nonDelimIdx;

    /* If saveState is nullptr, the user messed up. */
    ܐܢ (src != nullptr) {
        tokSource = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    ܐܚܪܢܐ ܐܢ (*saveState) {
        tokSource = *saveState;
    }
    ܐܚܪܢܐ {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        ܦܢܐ nullptr;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tokSource, delim);
    tokSource = &tokSource[nonDelimIdx];

    ܐܢ (*tokSource) {
        nextToken = u_strpbrk(tokSource, delim);
        ܐܢ (nextToken != nullptr) {
            /* Create a token */
            *(nextToken++) = 0;
            *saveState = nextToken;
            ܦܢܐ tokSource;
        }
        ܐܚܪܢܐ ܐܢ (*saveState) {
            /* Return the last token */
            *saveState = nullptr;
            ܦܢܐ tokSource;
        }
    }
    ܐܚܪܢܐ {
        /* No tokens were found. Only delimiters were left. */
        *saveState = nullptr;
    }
    ܦܢܐ nullptr;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, ثابت char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    ܟܡܐ(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    ܟܡܐ((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    ܦܢܐ anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, ثابت char16_t* src, int32_t n)
{
    ܐܢ(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        ܟܡܐ(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        ܟܡܐ((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            ܐܢ(--n == 0) {
                *dst = 0;
                اكسر;
            }
            ++src;
        }

        ܦܢܐ anchor;
    } ܐܚܪܢܐ {
        ܦܢܐ dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(ثابت char16_t *s1,
    ثابت char16_t *s2)
{
    char16_t  c1, c2;

    لكل(;;) {
        c1=*s1++;
        c2=*s2++;
        ܐܢ (c1 != c2 || c1 == 0) {
            اكسر;
        }
    }
    ܦܢܐ (int32_t)c1 - (int32_t)c2;
}

U_CFUNC int32_t U_EXPORT2
uprv_strCompare(ثابت char16_t *s1, int32_t length1,
                ثابت char16_t *s2, int32_t length2,
                UBool strncmpStyle, UBool codePointOrder) {
    ثابت char16_t *start1, *start2, *limit1, *limit2;
    char16_t c1, c2;

    /* setup for fix-up */
    start1=s1;
    start2=s2;

    /* compare identical prefixes - they do not need to be fixed up */
    ܐܢ(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        ܐܢ(s1==s2) {
            ܦܢܐ 0;
        }

        لكل(;;) {
            c1=*s1;
            c2=*s2;
            ܐܢ(c1!=c2) {
                اكسر;
            }
            ܐܢ(c1==0) {
                ܦܢܐ 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=nullptr;
    } ܐܚܪܢܐ ܐܢ(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        ܐܢ(s1==s2) {
            ܦܢܐ 0;
        }

        limit1=start1+length1;

        لكل(;;) {
            /* both lengths are same, check only one limit */
            ܐܢ(s1==limit1) {
                ܦܢܐ 0;
            }

            c1=*s1;
            c2=*s2;
            ܐܢ(c1!=c2) {
                اكسر;
            }
            ܐܢ(c1==0) {
                ܦܢܐ 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } ܐܚܪܢܐ {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t lengthResult;

        ܐܢ(length1<0) {
            length1=u_strlen(s1);
        }
        ܐܢ(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        ܐܢ(length1<length2) {
            lengthResult=-1;
            limit1=start1+length1;
        } ܐܚܪܢܐ ܐܢ(length1==length2) {
            lengthResult=0;
            limit1=start1+length1;
        } ܐܚܪܢܐ /* length1>length2 */ {
            lengthResult=1;
            limit1=start1+length2;
        }

        ܐܢ(s1==s2) {
            ܦܢܐ lengthResult;
        }

        لكل(;;) {
            /* check pseudo-limit */
            ܐܢ(s1==limit1) {
                ܦܢܐ lengthResult;
            }

            c1=*s1;
            c2=*s2;
            ܐܢ(c1!=c2) {
                اكسر;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    ܐܢ(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        ܐܢ(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ܐܚܪܢܐ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        ܐܢ(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ܐܚܪܢܐ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    ܦܢܐ (int32_t)c1-(int32_t)c2;
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
    ܐܢ(iter1==nullptr || iter2==nullptr) {
        ܦܢܐ 0; /* bad arguments */
    }
    ܐܢ(iter1==iter2) {
        ܦܢܐ 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_START);
    iter2->move(iter2, 0, UITER_START);

    /* compare identical prefixes - they do not need to be fixed up */
    لكل(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        ܐܢ(c1!=c2) {
            اكسر;
        }
        ܐܢ(c1==-1) {
            ܦܢܐ 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    ܐܢ(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        ܐܢ(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->previous(iter1), U16_IS_LEAD(iter1->previous(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ܐܚܪܢܐ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        ܐܢ(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->previous(iter2), U16_IS_LEAD(iter2->previous(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ܐܚܪܢܐ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    ܦܢܐ (int32_t)c1-(int32_t)c2;
}

#ܐܢ 0
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
فراغ fragment {
        /* iff a surrogate is part of a surrogate pair, leave >=d800 */
        ܐܢ(c1<=0xdbff) {
            ܐܢ(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } ܐܚܪܢܐ ܐܢ(c1<=0xdfff) {
            int32_t idx=iter1->getIndex(iter1, UITER_CURRENT);
            iter1->previous(iter1); /* ==c1 */
            ܐܢ(!U16_IS_LEAD(iter1->previous(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } ܐܚܪܢܐ /* 0xe000<=c1<=0xffff */ {
            /* BMP code point - make <d800 */
            c1-=0x2800;
        }
}
#endif

U_CAPI int32_t U_EXPORT2
u_strCompare(ثابت char16_t *s1, int32_t length1,
             ثابت char16_t *s2, int32_t length2,
             UBool codePointOrder) {
    /* argument checking */
    ܐܢ(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        ܦܢܐ 0;
    }
    ܦܢܐ uprv_strCompare(s1, length1, s2, length2, ܕܓܠܐ, codePointOrder);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmpCodePointOrder(ثابت char16_t *s1, ثابت char16_t *s2) {
    ܦܢܐ uprv_strCompare(s1, -1, s2, -1, ܕܓܠܐ, ܫܪܪܐ);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(ثابت char16_t  *s1,
     ثابت char16_t  *s2,
     int32_t     n) 
{
    ܐܢ(n > 0) {
        int32_t rc;
        لكل(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            ܐܢ(rc != 0 || *s1 == 0 || --n == 0) {
                ܦܢܐ rc;
            }
            ++s1;
            ++s2;
        }
    } ܐܚܪܢܐ {
        ܦܢܐ 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmpCodePointOrder(ثابت char16_t *s1, ثابت char16_t *s2, int32_t n) {
    ܦܢܐ uprv_strCompare(s1, n, s2, n, ܫܪܪܐ, ܫܪܪܐ);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, ثابت char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    ܟܡܐ((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    ܦܢܐ anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, ثابت char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    ܟܡܐ(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    ܦܢܐ anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(ثابت char16_t *s)
{
#ܐܢ U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    ܦܢܐ (int32_t)uprv_wcslen((ثابت wchar_t *)s);
#ܐܚܪܢܐ
    ثابت char16_t *t = s;
    ܟܡܐ(*t != 0) {
      ++t;
    }
    ܦܢܐ t - s;
#endif
}

U_CAPI int32_t U_EXPORT2
u_countChar32(ثابت char16_t *s, int32_t length) {
    int32_t count;

    ܐܢ(s==nullptr || length<-1) {
        ܦܢܐ 0;
    }

    count=0;
    ܐܢ(length>=0) {
        ܟܡܐ(length>0) {
            ++count;
            ܐܢ(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } ܐܚܪܢܐ {
                ++s;
                --length;
            }
        }
    } ܐܚܪܢܐ /* length==-1 */ {
        char16_t c;

        لكل(;;) {
            ܐܢ((c=*s++)==0) {
                اكسر;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            ܐܢ(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    ܦܢܐ count;
}

U_CAPI UBool U_EXPORT2
u_strHasMoreChar32Than(ثابت char16_t *s, int32_t length, int32_t number) {

    ܐܢ(number<0) {
        ܦܢܐ ܫܪܪܐ;
    }
    ܐܢ(s==nullptr || length<-1) {
        ܦܢܐ ܕܓܠܐ;
    }

    ܐܢ(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        لكل(;;) {
            ܐܢ((c=*s++)==0) {
                ܦܢܐ ܕܓܠܐ;
            }
            ܐܢ(number==0) {
                ܦܢܐ ܫܪܪܐ;
            }
            ܐܢ(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } ܐܚܪܢܐ {
        /* length>=0 known */
        ثابت char16_t *limit;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        ܐܢ(((length+1)/2)>number) {
            ܦܢܐ ܫܪܪܐ;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        ܐܢ(maxSupplementary<=0) {
            ܦܢܐ ܕܓܠܐ;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        limit=s+length;
        لكل(;;) {
            ܐܢ(s==limit) {
                ܦܢܐ ܕܓܠܐ;
            }
            ܐܢ(number==0) {
                ܦܢܐ ܫܪܪܐ;
            }
            ܐܢ(U16_IS_LEAD(*s++) && s!=limit && U16_IS_TRAIL(*s)) {
                ++s;
                ܐܢ(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    ܦܢܐ ܕܓܠܐ;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, ثابت char16_t* src, int32_t count) {
    ܐܢ(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    ܦܢܐ dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, ثابت char16_t* src, int32_t count) {
    ܐܢ(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    ܦܢܐ dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ܐܢ(count > 0) {
        char16_t *ptr = dest;
        char16_t *limit = dest + count;

        ܟܡܐ (ptr < limit) {
            *(ptr++) = c;
        }
    }
    ܦܢܐ dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(ثابت char16_t *buf1, ثابت char16_t *buf2, int32_t count) {
    ܐܢ(count > 0) {
        ثابت char16_t *limit = buf1 + count;
        int32_t result;

        ܟܡܐ (buf1 < limit) {
            result = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            ܐܢ (result != 0) {
                ܦܢܐ result;
            }
            buf1++;
            buf2++;
        }
    }
    ܦܢܐ 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmpCodePointOrder(ثابت char16_t *s1, ثابت char16_t *s2, int32_t count) {
    ܦܢܐ uprv_strCompare(s1, count, s2, count, ܕܓܠܐ, ܫܪܪܐ);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
ساكن ثابت char16_t UNESCAPE_MAP[] = {
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
تعداد { UNESCAPE_MAP_LENGTH = UPRV_LENGTHOF(UNESCAPE_MAP) };

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
ساكن int32_t _digit8(char16_t c) {
    ܐܢ (c >= u'0' && c <= u'7') {
        ܦܢܐ c - u'0';
    }
    ܦܢܐ -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
ساكن int32_t _digit16(char16_t c) {
    ܐܢ (c >= u'0' && c <= u'9') {
        ܦܢܐ c - u'0';
    }
    ܐܢ (c >= u'A' && c <= u'F') {
        ܦܢܐ c - (u'A' - 10);
    }
    ܐܢ (c >= u'a' && c <= u'f') {
        ܦܢܐ c - (u'a' - 10);
    }
    ܦܢܐ -1;
}

/* Parse a single escape sequence.  Although this method deals in
 * UChars, it does not use C++ or UnicodeString.  This allows it to
 * be used from C contexts. */
U_CAPI UChar32 U_EXPORT2
u_unescapeAt(UNESCAPE_CHAR_AT charAt,
             int32_t *offset,
             int32_t length,
             فراغ *context) {

    int32_t start = *offset;
    UChar32 c;
    UChar32 result = 0;
    int8_t n = 0;
    int8_t minDig = 0;
    int8_t maxDig = 0;
    int8_t bitsPerDigit = 4; 
    int32_t dig;
    UBool braces = ܕܓܠܐ;

    /* Check that offset is in range */
    ܐܢ (*offset < 0 || *offset >= length) {
        اذهب err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*offset)++, context);

    /* Convert hexadecimal and octal escapes */
    بدل (c) {
    حالة u'u':
        minDig = maxDig = 4;
        اكسر;
    حالة u'U':
        minDig = maxDig = 8;
        اكسر;
    حالة u'x':
        minDig = 1;
        ܐܢ (*offset < length && charAt(*offset, context) == u'{') {
            ++(*offset);
            braces = ܫܪܪܐ;
            maxDig = 8;
        } ܐܚܪܢܐ {
            maxDig = 2;
        }
        اكسر;
    افتراضي:
        dig = _digit8(c);
        ܐܢ (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            bitsPerDigit = 3;
            result = dig;
        }
        اكسر;
    }
    ܐܢ (minDig != 0) {
        ܟܡܐ (*offset < length && n < maxDig) {
            c = charAt(*offset, context);
            dig = (bitsPerDigit == 3) ? _digit8(c) : _digit16(c);
            ܐܢ (dig < 0) {
                اكسر;
            }
            result = (result << bitsPerDigit) | dig;
            ++(*offset);
            ++n;
        }
        ܐܢ (n < minDig) {
            اذهب err;
        }
        ܐܢ (braces) {
            ܐܢ (c != u'}') {
                اذهب err;
            }
            ++(*offset);
        }
        ܐܢ (result < 0 || result >= 0x110000) {
            اذهب err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        ܐܢ (*offset < length && U16_IS_LEAD(result)) {
            int32_t ahead = *offset + 1;
            c = charAt(*offset, context);
            ܐܢ (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t tailLimit = ahead + 11;
                ܐܢ (tailLimit > length) {
                    tailLimit = length;
                }
                c = u_unescapeAt(charAt, &ahead, tailLimit, context);
            }
            ܐܢ (U16_IS_TRAIL(c)) {
                *offset = ahead;
                result = U16_GET_SUPPLEMENTARY(result, c);
            }
        }
        ܦܢܐ result;
    }

    /* Convert C-style escapes in table */
    لكل (int32_t i=0; i<UNESCAPE_MAP_LENGTH; i+=2) {
        ܐܢ (c == UNESCAPE_MAP[i]) {
            ܦܢܐ UNESCAPE_MAP[i+1];
        } ܐܚܪܢܐ ܐܢ (c < UNESCAPE_MAP[i]) {
            اكسر;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    ܐܢ (c == u'c' && *offset < length) {
        c = charAt((*offset)++, context);
        ܐܢ (U16_IS_LEAD(c) && *offset < length) {
            char16_t c2 = charAt(*offset, context);
            ܐܢ (U16_IS_TRAIL(c2)) {
                ++(*offset);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        ܦܢܐ 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    ܐܢ (U16_IS_LEAD(c) && *offset < length) {
        char16_t c2 = charAt(*offset, context);
        ܐܢ (U16_IS_TRAIL(c2)) {
            ++(*offset);
            ܦܢܐ U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    ܦܢܐ c;

 err:
    /* Invalid escape sequence */
    *offset = start; /* Reset to initial value */
    ܦܢܐ (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
ساكن char16_t U_CALLCONV
_charPtr_charAt(int32_t offset, فراغ *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_charsToUChars(static_cast<حرف*>(context) + offset, &c16, 1);
    ܦܢܐ c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
ساكن فراغ _appendUChars(char16_t *dest, int32_t destCapacity,
                          ثابت حرف *src, int32_t srcLen) {
    ܐܢ (destCapacity < 0) {
        destCapacity = 0;
    }
    ܐܢ (srcLen > destCapacity) {
        srcLen = destCapacity;
    }
    u_charsToUChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(ثابت حرف *src, char16_t *dest, int32_t destCapacity) {
    ثابت حرف *segment = src;
    int32_t i = 0;
    حرف c;

    ܟܡܐ ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        ܐܢ (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            ܐܢ (src != segment) {
                ܐܢ (dest != nullptr) {
                    _appendUChars(dest + i, destCapacity - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<حرف*>(src));
            ܐܢ (lenParsed == 0) {
                اذهب err;
            }
            src += lenParsed; /* advance past escape seq. */
            ܐܢ (dest != nullptr && U16_LENGTH(c32) <= (destCapacity - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } ܐܚܪܢܐ {
                i += U16_LENGTH(c32);
            }
            segment = src;
        } ܐܚܪܢܐ {
            ++src;
        }
    }
    ܐܢ (src != segment) {
        ܐܢ (dest != nullptr) {
            _appendUChars(dest + i, destCapacity - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    ܐܢ (dest != nullptr && i < destCapacity) {
        dest[i] = 0;
    }
    ܦܢܐ i;

 err:
    ܐܢ (dest != nullptr && destCapacity > 0) {
        *dest = 0;
    }
    ܦܢܐ 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#define __TERMINATE_STRING(dest, destCapacity, length, pErrorCode) UPRV_BLOCK_MACRO_BEGIN { \
    ܐܢ(pErrorCode!=nullptr && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        ܐܢ(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } ܐܚܪܢܐ ܐܢ(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            ܐܢ(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } ܐܚܪܢܐ ܐܢ(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } ܐܚܪܢܐ /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;                        \
        }                                                               \
    } \
} UPRV_BLOCK_MACRO_END

U_CAPI char16_t U_EXPORT2
u_asciiToUpper(char16_t c) {
    ܐܢ (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    ܦܢܐ c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ܦܢܐ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(حرف *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ܦܢܐ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ܦܢܐ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ܦܢܐ length;
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
    ثابت TYPE *p = (ثابت TYPE*) STR;        \
    ܐܢ (p != nullptr) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        ثابت TYPE *limit = p + len;          \
        ܟܡܐ (p<limit) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    ܦܢܐ static_cast<int32_t>(hash);        \
} UPRV_BLOCK_MACRO_END

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(ثابت char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(ثابت حرف *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(ثابت حرف *str, int32_t length) {
    STRING_HASH(حرف, str, length, (uint8_t)uprv_tolower(*p));
}
