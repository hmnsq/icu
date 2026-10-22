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

#include "unicode/utypes.h"
#include "unicode/putil.h"
#include "unicode/uchar.h"
#include "unicode/ustring.h"
#include "unicode/utf16.h"
#include "cstring.h"
#include "cwchar.h"
#include "cmemory.h"
#include "ustr_imp.h"

/* ANSI string.h - style functions ------------------------------------------ */

/* U+ffff is the highest BMP code point, the highest one that fits into a 16-bit char16_t */
#define U_BMP_MAX 0xffff

/* Forward binary string search functions ----------------------------------- */

/*
 * Test if a substring match inside a string is at code point boundaries.
 * All pointers refer to the same buffer.
 * The limit pointer may be nullptr, all others must be real pointers.
 */
static inline UBool
isMatchAtCPBoundary(const char16_t *start, const char16_t *match, const char16_t *matchLimit, const char16_t *limit) {
    ако(U16_IS_TRAIL(*match) && start!=match && U16_IS_LEAD(*(match-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        врати false;
    }
    ако(U16_IS_LEAD(*(matchLimit-1)) && matchLimit!=limit && U16_IS_TRAIL(*matchLimit)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        врати false;
    }
    врати true;
}

U_CAPI char16_t* U_EXPORT2
u_strFindFirst(const char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               const char16_t* sub,
               int32_t subLength) {
    const char16_t *start, *p, *q, *subLimit;
    char16_t c, cs, cq;

    ако(sub==nullptr || subLength<-1) {
        врати (char16_t *)s;
    }
    ако(s==nullptr || length<-1) {
        врати nullptr;
    }

    start=s;

    ако(length<0 && subLength<0) {
        /* both strings are NUL-terminated */
        ако((cs=*sub++)==0) {
            врати (char16_t *)s;
        }
        ако(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            врати u_strchr(s, cs);
        }

        док((c=*s++)!=0) {
            ако(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                за(;;) {
                    ако((cq=*q)==0) {
                        ако(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            врати (char16_t *)(s-1); /* well-formed match */
                        } инако {
                            break; /* no match because surrogate pair is split */
                        }
                    }
                    ако((c=*p)==0) {
                        врати nullptr; /* no match, and none possible after s */
                    }
                    ако(c!=cq) {
                        break; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        врати nullptr;
    }

    ако(subLength<0) {
        subLength=u_strlen(sub);
    }
    ако(subLength==0) {
        врати (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --subLength;
    subLimit=sub+subLength;

    ако(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        врати length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    ако(length<0) {
        /* s is NUL-terminated */
        док((c=*s++)!=0) {
            ако(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                за(;;) {
                    ако(q==subLimit) {
                        ако(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            врати (char16_t *)(s-1); /* well-formed match */
                        } инако {
                            break; /* no match because surrogate pair is split */
                        }
                    }
                    ако((c=*p)==0) {
                        врати nullptr; /* no match, and none possible after s */
                    }
                    ако(c!=*q) {
                        break; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } инако {
        const char16_t *limit, *preLimit;

        /* subLength was decremented above */
        ако(length<=subLength) {
            врати nullptr; /* s is shorter than sub */
        }

        limit=s+length;

        /* the substring must start before preLimit */
        preLimit=limit-subLength;

        док(s!=preLimit) {
            c=*s++;
            ако(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                за(;;) {
                    ако(q==subLimit) {
                        ако(isMatchAtCPBoundary(start, s-1, p, limit)) {
                            врати (char16_t *)(s-1); /* well-formed match */
                        } инако {
                            break; /* no match because surrogate pair is split */
                        }
                    }
                    ако(*p!=*q) {
                        break; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    врати nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(const char16_t* s U_LIFETIME_BOUND, const char16_t* substring) {
    врати u_strFindFirst(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(const char16_t* s U_LIFETIME_BOUND, char16_t c) {
    ако(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        врати u_strFindFirst(s, -1, &c, 1);
    } инако {
        char16_t cs;

        /* trivial search for a BMP code point */
        за(;;) {
            ако((cs=*s)==c) {
                врати (char16_t *)s;
            }
            ако(cs==0) {
                врати nullptr;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    ако((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        врати u_strchr(s, (char16_t)c);
    } инако ако((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        док((cs=*s++)!=0) {
            ако(cs==lead && *s==trail) {
                врати (char16_t *)(s-1);
            }
        }
        врати nullptr;
    } инако {
        /* not a Unicode code point, not findable */
        врати nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(const char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ако(count<=0) {
        врати nullptr; /* no string */
    } инако ако(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        врати u_strFindFirst(s, count, &c, 1);
    } инако {
        /* trivial search for a BMP code point */
        const char16_t *limit=s+count;
        do {
            ако(*s==c) {
                врати (char16_t *)s;
            }
        } док(++s!=limit);
        врати nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    ако((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        врати u_memchr(s, (char16_t)c, count);
    } инако ако(count<2) {
        /* too short for a surrogate pair */
        врати nullptr;
    } инако ако((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        const char16_t *limit=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        do {
            ако(*s==lead && *(s+1)==trail) {
                врати (char16_t *)s;
            }
        } док(++s!=limit);
        врати nullptr;
    } инако {
        /* not a Unicode code point, not findable */
        врати nullptr;
    }
}

/* Backward binary string search functions ---------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strFindLast(const char16_t* s U_LIFETIME_BOUND,
              int32_t length,
              const char16_t* sub,
              int32_t subLength) {
    const char16_t *start, *limit, *p, *q, *subLimit;
    char16_t c, cs;

    ако(sub==nullptr || subLength<-1) {
        врати (char16_t *)s;
    }
    ако(s==nullptr || length<-1) {
        врати nullptr;
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

    ако(subLength<0) {
        subLength=u_strlen(sub);
    }
    ако(subLength==0) {
        врати (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    subLimit=sub+subLength;
    cs=*(--subLimit);
    --subLength;

    ако(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        врати length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    ако(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    ако(length<=subLength) {
        врати nullptr; /* s is shorter than sub */
    }

    start=s;
    limit=s+length;

    /* the substring must start no later than s+subLength */
    s+=subLength;

    док(s!=limit) {
        c=*(--limit);
        ако(c==cs) {
            /* found last substring char16_t, compare rest */
            p=limit;
            q=subLimit;
            за(;;) {
                ако(q==sub) {
                    ако(isMatchAtCPBoundary(start, p, limit+1, start+length)) {
                        врати (char16_t *)p; /* well-formed match */
                    } инако {
                        break; /* no match because surrogate pair is split */
                    }
                }
                ако(*(--p)!=*(--q)) {
                    break; /* no match */
                }
            }
        }
    }

    /* not found */
    врати nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(const char16_t* s U_LIFETIME_BOUND, const char16_t* substring) {
    врати u_strFindLast(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(const char16_t* s U_LIFETIME_BOUND, char16_t c) {
    ако(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        врати u_strFindLast(s, -1, &c, 1);
    } инако {
        const char16_t *result=nullptr;
        char16_t cs;

        /* trivial search for a BMP code point */
        за(;;) {
            ако((cs=*s)==c) {
                result=s;
            }
            ако(cs==0) {
                врати (char16_t *)result;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    ако((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        врати u_strrchr(s, (char16_t)c);
    } инако ако((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        const char16_t *result=nullptr;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        док((cs=*s++)!=0) {
            ако(cs==lead && *s==trail) {
                result=s-1;
            }
        }
        врати (char16_t *)result;
    } инако {
        /* not a Unicode code point, not findable */
        врати nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(const char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ако(count<=0) {
        врати nullptr; /* no string */
    } инако ако(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        врати u_strFindLast(s, count, &c, 1);
    } инако {
        /* trivial search for a BMP code point */
        const char16_t *limit=s+count;
        do {
            ако(*(--limit)==c) {
                врати (char16_t *)limit;
            }
        } док(s!=limit);
        врати nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    ако((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        врати u_memrchr(s, (char16_t)c, count);
    } инако ако(count<2) {
        /* too short for a surrogate pair */
        врати nullptr;
    } инако ако((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        const char16_t *limit=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        do {
            ако(*limit==trail && *(limit-1)==lead) {
                врати (char16_t *)(limit-1);
            }
        } док(s!=--limit);
        врати nullptr;
    } инако {
        /* not a Unicode code point, not findable */
        врати nullptr;
    }
}

/* Tokenization functions --------------------------------------------------- */

/*
 * Match each code point in a string against each code point in the matchSet.
 * Return the index of the first string code point that
 * is (polarity==true) or is not (false) contained in the matchSet.
 * Return -(string length)-1 if there is no such code point.
 */
static int32_t
_matchFromSet(const char16_t *string, const char16_t *matchSet, UBool polarity) {
    int32_t matchLen, matchBMPLen, strItr, matchItr;
    UChar32 stringCh, matchCh;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    matchBMPLen = 0;
    док((c = matchSet[matchBMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++matchBMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    matchLen = matchBMPLen;
    док(matchSet[matchLen] != 0) {
        ++matchLen;
    }

    за(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        ако(U16_IS_SINGLE(c)) {
            ако(polarity) {
                за(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    ако(c == matchSet[matchItr]) {
                        врати strItr - 1; /* one matches */
                    }
                }
            } инако {
                за(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    ако(c == matchSet[matchItr]) {
                        goto endloop;
                    }
                }
                врати strItr - 1; /* none matches */
            }
        } инако {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            ако(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } инако {
                stringCh = c; /* unpaired trail surrogate */
            }

            ако(polarity) {
                за(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    ако(stringCh == matchCh) {
                        врати strItr - U16_LENGTH(stringCh); /* one matches */
                    }
                }
            } инако {
                за(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    ако(stringCh == matchCh) {
                        goto endloop;
                    }
                }
                врати strItr - U16_LENGTH(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    врати -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(const char16_t* string U_LIFETIME_BOUND, const char16_t* matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, true);
    ако(idx >= 0) {
        врати (char16_t *)string + idx;
    } инако {
        врати nullptr;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(const char16_t *string, const char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, true);
    ако(idx >= 0) {
        врати idx;
    } инако {
        врати -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(const char16_t *string, const char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, false);
    ако(idx >= 0) {
        врати idx;
    } инако {
        врати -idx - 1; /* == u_strlen(string) */
    }
}

/* ----- Text manipulation functions --- */

U_CAPI char16_t* U_EXPORT2
u_strtok_r(char16_t* src U_LIFETIME_BOUND, const char16_t* delim, char16_t** saveState)
{
    char16_t *tokSource;
    char16_t *nextToken;
    uint32_t nonDelimIdx;

    /* If saveState is nullptr, the user messed up. */
    ако (src != nullptr) {
        tokSource = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    инако ако (*saveState) {
        tokSource = *saveState;
    }
    инако {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        врати nullptr;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tokSource, delim);
    tokSource = &tokSource[nonDelimIdx];

    ако (*tokSource) {
        nextToken = u_strpbrk(tokSource, delim);
        ако (nextToken != nullptr) {
            /* Create a token */
            *(nextToken++) = 0;
            *saveState = nextToken;
            врати tokSource;
        }
        инако ако (*saveState) {
            /* Return the last token */
            *saveState = nullptr;
            врати tokSource;
        }
    }
    инако {
        /* No tokens were found. Only delimiters were left. */
        *saveState = nullptr;
    }
    врати nullptr;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, const char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    док(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    док((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    врати anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, const char16_t* src, int32_t n)
{
    ако(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        док(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        док((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            ако(--n == 0) {
                *dst = 0;
                break;
            }
            ++src;
        }

        врати anchor;
    } инако {
        врати dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(const char16_t *s1,
    const char16_t *s2)
{
    char16_t  c1, c2;

    за(;;) {
        c1=*s1++;
        c2=*s2++;
        ако (c1 != c2 || c1 == 0) {
            break;
        }
    }
    врати (int32_t)c1 - (int32_t)c2;
}

U_CFUNC int32_t U_EXPORT2
uprv_strCompare(const char16_t *s1, int32_t length1,
                const char16_t *s2, int32_t length2,
                UBool strncmpStyle, UBool codePointOrder) {
    const char16_t *start1, *start2, *limit1, *limit2;
    char16_t c1, c2;

    /* setup for fix-up */
    start1=s1;
    start2=s2;

    /* compare identical prefixes - they do not need to be fixed up */
    ако(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        ако(s1==s2) {
            врати 0;
        }

        за(;;) {
            c1=*s1;
            c2=*s2;
            ако(c1!=c2) {
                break;
            }
            ако(c1==0) {
                врати 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=nullptr;
    } инако ако(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        ако(s1==s2) {
            врати 0;
        }

        limit1=start1+length1;

        за(;;) {
            /* both lengths are same, check only one limit */
            ако(s1==limit1) {
                врати 0;
            }

            c1=*s1;
            c2=*s2;
            ако(c1!=c2) {
                break;
            }
            ако(c1==0) {
                врати 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } инако {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t lengthResult;

        ако(length1<0) {
            length1=u_strlen(s1);
        }
        ако(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        ако(length1<length2) {
            lengthResult=-1;
            limit1=start1+length1;
        } инако ако(length1==length2) {
            lengthResult=0;
            limit1=start1+length1;
        } инако /* length1>length2 */ {
            lengthResult=1;
            limit1=start1+length2;
        }

        ако(s1==s2) {
            врати lengthResult;
        }

        за(;;) {
            /* check pseudo-limit */
            ако(s1==limit1) {
                врати lengthResult;
            }

            c1=*s1;
            c2=*s2;
            ако(c1!=c2) {
                break;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    ако(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        ако(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } инако {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        ако(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } инако {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    врати (int32_t)c1-(int32_t)c2;
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
    ако(iter1==nullptr || iter2==nullptr) {
        врати 0; /* bad arguments */
    }
    ако(iter1==iter2) {
        врати 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_START);
    iter2->move(iter2, 0, UITER_START);

    /* compare identical prefixes - they do not need to be fixed up */
    за(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        ако(c1!=c2) {
            break;
        }
        ако(c1==-1) {
            врати 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    ако(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        ако(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->previous(iter1), U16_IS_LEAD(iter1->previous(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } инако {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        ако(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->previous(iter2), U16_IS_LEAD(iter2->previous(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } инако {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    врати (int32_t)c1-(int32_t)c2;
}

#ако 0
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
void fragment {
        /* iff a surrogate is part of a surrogate pair, leave >=d800 */
        ако(c1<=0xdbff) {
            ако(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } инако ако(c1<=0xdfff) {
            int32_t idx=iter1->getIndex(iter1, UITER_CURRENT);
            iter1->previous(iter1); /* ==c1 */
            ако(!U16_IS_LEAD(iter1->previous(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } инако /* 0xe000<=c1<=0xffff */ {
            /* BMP code point - make <d800 */
            c1-=0x2800;
        }
}
#endif

U_CAPI int32_t U_EXPORT2
u_strCompare(const char16_t *s1, int32_t length1,
             const char16_t *s2, int32_t length2,
             UBool codePointOrder) {
    /* argument checking */
    ако(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        врати 0;
    }
    врати uprv_strCompare(s1, length1, s2, length2, false, codePointOrder);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmpCodePointOrder(const char16_t *s1, const char16_t *s2) {
    врати uprv_strCompare(s1, -1, s2, -1, false, true);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(const char16_t  *s1,
     const char16_t  *s2,
     int32_t     n) 
{
    ако(n > 0) {
        int32_t rc;
        за(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            ако(rc != 0 || *s1 == 0 || --n == 0) {
                врати rc;
            }
            ++s1;
            ++s2;
        }
    } инако {
        врати 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmpCodePointOrder(const char16_t *s1, const char16_t *s2, int32_t n) {
    врати uprv_strCompare(s1, n, s2, n, true, true);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, const char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    док((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    врати anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, const char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    док(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    врати anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(const char16_t *s)
{
#ако U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    врати (int32_t)uprv_wcslen((const wchar_t *)s);
#инако
    const char16_t *t = s;
    док(*t != 0) {
      ++t;
    }
    врати t - s;
#endif
}

U_CAPI int32_t U_EXPORT2
u_countChar32(const char16_t *s, int32_t length) {
    int32_t count;

    ако(s==nullptr || length<-1) {
        врати 0;
    }

    count=0;
    ако(length>=0) {
        док(length>0) {
            ++count;
            ако(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } инако {
                ++s;
                --length;
            }
        }
    } инако /* length==-1 */ {
        char16_t c;

        за(;;) {
            ако((c=*s++)==0) {
                break;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            ако(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    врати count;
}

U_CAPI UBool U_EXPORT2
u_strHasMoreChar32Than(const char16_t *s, int32_t length, int32_t number) {

    ако(number<0) {
        врати true;
    }
    ако(s==nullptr || length<-1) {
        врати false;
    }

    ако(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        за(;;) {
            ако((c=*s++)==0) {
                врати false;
            }
            ако(number==0) {
                врати true;
            }
            ако(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } инако {
        /* length>=0 known */
        const char16_t *limit;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        ако(((length+1)/2)>number) {
            врати true;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        ако(maxSupplementary<=0) {
            врати false;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        limit=s+length;
        за(;;) {
            ако(s==limit) {
                врати false;
            }
            ако(number==0) {
                врати true;
            }
            ако(U16_IS_LEAD(*s++) && s!=limit && U16_IS_TRAIL(*s)) {
                ++s;
                ако(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    врати false;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, const char16_t* src, int32_t count) {
    ако(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    врати dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, const char16_t* src, int32_t count) {
    ако(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    врати dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ако(count > 0) {
        char16_t *ptr = dest;
        char16_t *limit = dest + count;

        док (ptr < limit) {
            *(ptr++) = c;
        }
    }
    врати dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(const char16_t *buf1, const char16_t *buf2, int32_t count) {
    ако(count > 0) {
        const char16_t *limit = buf1 + count;
        int32_t result;

        док (buf1 < limit) {
            result = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            ако (result != 0) {
                врати result;
            }
            buf1++;
            buf2++;
        }
    }
    врати 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmpCodePointOrder(const char16_t *s1, const char16_t *s2, int32_t count) {
    врати uprv_strCompare(s1, count, s2, count, false, true);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
static const char16_t UNESCAPE_MAP[] = {
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
enum { UNESCAPE_MAP_LENGTH = UPRV_LENGTHOF(UNESCAPE_MAP) };

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
static int32_t _digit8(char16_t c) {
    ако (c >= u'0' && c <= u'7') {
        врати c - u'0';
    }
    врати -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
static int32_t _digit16(char16_t c) {
    ако (c >= u'0' && c <= u'9') {
        врати c - u'0';
    }
    ако (c >= u'A' && c <= u'F') {
        врати c - (u'A' - 10);
    }
    ако (c >= u'a' && c <= u'f') {
        врати c - (u'a' - 10);
    }
    врати -1;
}

/* Parse a single escape sequence.  Although this method deals in
 * UChars, it does not use C++ or UnicodeString.  This allows it to
 * be used from C contexts. */
U_CAPI UChar32 U_EXPORT2
u_unescapeAt(UNESCAPE_CHAR_AT charAt,
             int32_t *offset,
             int32_t length,
             void *context) {

    int32_t start = *offset;
    UChar32 c;
    UChar32 result = 0;
    int8_t n = 0;
    int8_t minDig = 0;
    int8_t maxDig = 0;
    int8_t bitsPerDigit = 4; 
    int32_t dig;
    UBool braces = false;

    /* Check that offset is in range */
    ако (*offset < 0 || *offset >= length) {
        goto err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*offset)++, context);

    /* Convert hexadecimal and octal escapes */
    switch (c) {
    case u'u':
        minDig = maxDig = 4;
        break;
    case u'U':
        minDig = maxDig = 8;
        break;
    case u'x':
        minDig = 1;
        ако (*offset < length && charAt(*offset, context) == u'{') {
            ++(*offset);
            braces = true;
            maxDig = 8;
        } инако {
            maxDig = 2;
        }
        break;
    default:
        dig = _digit8(c);
        ако (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            bitsPerDigit = 3;
            result = dig;
        }
        break;
    }
    ако (minDig != 0) {
        док (*offset < length && n < maxDig) {
            c = charAt(*offset, context);
            dig = (bitsPerDigit == 3) ? _digit8(c) : _digit16(c);
            ако (dig < 0) {
                break;
            }
            result = (result << bitsPerDigit) | dig;
            ++(*offset);
            ++n;
        }
        ако (n < minDig) {
            goto err;
        }
        ако (braces) {
            ако (c != u'}') {
                goto err;
            }
            ++(*offset);
        }
        ако (result < 0 || result >= 0x110000) {
            goto err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        ако (*offset < length && U16_IS_LEAD(result)) {
            int32_t ahead = *offset + 1;
            c = charAt(*offset, context);
            ако (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t tailLimit = ahead + 11;
                ако (tailLimit > length) {
                    tailLimit = length;
                }
                c = u_unescapeAt(charAt, &ahead, tailLimit, context);
            }
            ако (U16_IS_TRAIL(c)) {
                *offset = ahead;
                result = U16_GET_SUPPLEMENTARY(result, c);
            }
        }
        врати result;
    }

    /* Convert C-style escapes in table */
    за (int32_t i=0; i<UNESCAPE_MAP_LENGTH; i+=2) {
        ако (c == UNESCAPE_MAP[i]) {
            врати UNESCAPE_MAP[i+1];
        } инако ако (c < UNESCAPE_MAP[i]) {
            break;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    ако (c == u'c' && *offset < length) {
        c = charAt((*offset)++, context);
        ако (U16_IS_LEAD(c) && *offset < length) {
            char16_t c2 = charAt(*offset, context);
            ако (U16_IS_TRAIL(c2)) {
                ++(*offset);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        врати 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    ако (U16_IS_LEAD(c) && *offset < length) {
        char16_t c2 = charAt(*offset, context);
        ако (U16_IS_TRAIL(c2)) {
            ++(*offset);
            врати U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    врати c;

 err:
    /* Invalid escape sequence */
    *offset = start; /* Reset to initial value */
    врати (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
static char16_t U_CALLCONV
_charPtr_charAt(int32_t offset, void *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_charsToUChars(static_cast<char*>(context) + offset, &c16, 1);
    врати c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
static void _appendUChars(char16_t *dest, int32_t destCapacity,
                          const char *src, int32_t srcLen) {
    ако (destCapacity < 0) {
        destCapacity = 0;
    }
    ако (srcLen > destCapacity) {
        srcLen = destCapacity;
    }
    u_charsToUChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(const char *src, char16_t *dest, int32_t destCapacity) {
    const char *segment = src;
    int32_t i = 0;
    char c;

    док ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        ако (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            ако (src != segment) {
                ако (dest != nullptr) {
                    _appendUChars(dest + i, destCapacity - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<char*>(src));
            ако (lenParsed == 0) {
                goto err;
            }
            src += lenParsed; /* advance past escape seq. */
            ако (dest != nullptr && U16_LENGTH(c32) <= (destCapacity - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } инако {
                i += U16_LENGTH(c32);
            }
            segment = src;
        } инако {
            ++src;
        }
    }
    ако (src != segment) {
        ако (dest != nullptr) {
            _appendUChars(dest + i, destCapacity - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    ако (dest != nullptr && i < destCapacity) {
        dest[i] = 0;
    }
    врати i;

 err:
    ако (dest != nullptr && destCapacity > 0) {
        *dest = 0;
    }
    врати 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#define __TERMINATE_STRING(dest, destCapacity, length, pErrorCode) UPRV_BLOCK_MACRO_BEGIN { \
    ако(pErrorCode!=nullptr && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        ако(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } инако ако(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            ако(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } инако ако(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } инако /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;                        \
        }                                                               \
    } \
} UPRV_BLOCK_MACRO_END

U_CAPI char16_t U_EXPORT2
u_asciiToUpper(char16_t c) {
    ако (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    врати c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    врати length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(char *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    врати length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    врати length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    врати length;
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
    const TYPE *p = (const TYPE*) STR;        \
    ако (p != nullptr) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        const TYPE *limit = p + len;          \
        док (p<limit) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    врати static_cast<int32_t>(hash);        \
} UPRV_BLOCK_MACRO_END

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(const char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(const char *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(const char *str, int32_t length) {
    STRING_HASH(char, str, length, (uint8_t)uprv_tolower(*p));
}
