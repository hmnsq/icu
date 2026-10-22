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
    もし(U16_IS_TRAIL(*match) && start!=match && U16_IS_LEAD(*(match-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        戻る false;
    }
    もし(U16_IS_LEAD(*(matchLimit-1)) && matchLimit!=limit && U16_IS_TRAIL(*matchLimit)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        戻る false;
    }
    戻る true;
}

U_CAPI char16_t* U_EXPORT2
u_strFindFirst(const char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               const char16_t* sub,
               int32_t subLength) {
    const char16_t *start, *p, *q, *subLimit;
    char16_t c, cs, cq;

    もし(sub==nullptr || subLength<-1) {
        戻る (char16_t *)s;
    }
    もし(s==nullptr || length<-1) {
        戻る nullptr;
    }

    start=s;

    もし(length<0 && subLength<0) {
        /* both strings are NUL-terminated */
        もし((cs=*sub++)==0) {
            戻る (char16_t *)s;
        }
        もし(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            戻る u_strchr(s, cs);
        }

        ながら((c=*s++)!=0) {
            もし(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                ために(;;) {
                    もし((cq=*q)==0) {
                        もし(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            戻る (char16_t *)(s-1); /* well-formed match */
                        } 否则 {
                            break; /* no match because surrogate pair is split */
                        }
                    }
                    もし((c=*p)==0) {
                        戻る nullptr; /* no match, and none possible after s */
                    }
                    もし(c!=cq) {
                        break; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        戻る nullptr;
    }

    もし(subLength<0) {
        subLength=u_strlen(sub);
    }
    もし(subLength==0) {
        戻る (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --subLength;
    subLimit=sub+subLength;

    もし(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        戻る length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    もし(length<0) {
        /* s is NUL-terminated */
        ながら((c=*s++)!=0) {
            もし(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                ために(;;) {
                    もし(q==subLimit) {
                        もし(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            戻る (char16_t *)(s-1); /* well-formed match */
                        } 否则 {
                            break; /* no match because surrogate pair is split */
                        }
                    }
                    もし((c=*p)==0) {
                        戻る nullptr; /* no match, and none possible after s */
                    }
                    もし(c!=*q) {
                        break; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } 否则 {
        const char16_t *limit, *preLimit;

        /* subLength was decremented above */
        もし(length<=subLength) {
            戻る nullptr; /* s is shorter than sub */
        }

        limit=s+length;

        /* the substring must start before preLimit */
        preLimit=limit-subLength;

        ながら(s!=preLimit) {
            c=*s++;
            もし(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                ために(;;) {
                    もし(q==subLimit) {
                        もし(isMatchAtCPBoundary(start, s-1, p, limit)) {
                            戻る (char16_t *)(s-1); /* well-formed match */
                        } 否则 {
                            break; /* no match because surrogate pair is split */
                        }
                    }
                    もし(*p!=*q) {
                        break; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    戻る nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(const char16_t* s U_LIFETIME_BOUND, const char16_t* substring) {
    戻る u_strFindFirst(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(const char16_t* s U_LIFETIME_BOUND, char16_t c) {
    もし(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        戻る u_strFindFirst(s, -1, &c, 1);
    } 否则 {
        char16_t cs;

        /* trivial search for a BMP code point */
        ために(;;) {
            もし((cs=*s)==c) {
                戻る (char16_t *)s;
            }
            もし(cs==0) {
                戻る nullptr;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    もし((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        戻る u_strchr(s, (char16_t)c);
    } 否则 もし((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ながら((cs=*s++)!=0) {
            もし(cs==lead && *s==trail) {
                戻る (char16_t *)(s-1);
            }
        }
        戻る nullptr;
    } 否则 {
        /* not a Unicode code point, not findable */
        戻る nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(const char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    もし(count<=0) {
        戻る nullptr; /* no string */
    } 否则 もし(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        戻る u_strFindFirst(s, count, &c, 1);
    } 否则 {
        /* trivial search for a BMP code point */
        const char16_t *limit=s+count;
        do {
            もし(*s==c) {
                戻る (char16_t *)s;
            }
        } ながら(++s!=limit);
        戻る nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    もし((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        戻る u_memchr(s, (char16_t)c, count);
    } 否则 もし(count<2) {
        /* too short for a surrogate pair */
        戻る nullptr;
    } 否则 もし((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        const char16_t *limit=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        do {
            もし(*s==lead && *(s+1)==trail) {
                戻る (char16_t *)s;
            }
        } ながら(++s!=limit);
        戻る nullptr;
    } 否则 {
        /* not a Unicode code point, not findable */
        戻る nullptr;
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

    もし(sub==nullptr || subLength<-1) {
        戻る (char16_t *)s;
    }
    もし(s==nullptr || length<-1) {
        戻る nullptr;
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

    もし(subLength<0) {
        subLength=u_strlen(sub);
    }
    もし(subLength==0) {
        戻る (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    subLimit=sub+subLength;
    cs=*(--subLimit);
    --subLength;

    もし(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        戻る length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    もし(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    もし(length<=subLength) {
        戻る nullptr; /* s is shorter than sub */
    }

    start=s;
    limit=s+length;

    /* the substring must start no later than s+subLength */
    s+=subLength;

    ながら(s!=limit) {
        c=*(--limit);
        もし(c==cs) {
            /* found last substring char16_t, compare rest */
            p=limit;
            q=subLimit;
            ために(;;) {
                もし(q==sub) {
                    もし(isMatchAtCPBoundary(start, p, limit+1, start+length)) {
                        戻る (char16_t *)p; /* well-formed match */
                    } 否则 {
                        break; /* no match because surrogate pair is split */
                    }
                }
                もし(*(--p)!=*(--q)) {
                    break; /* no match */
                }
            }
        }
    }

    /* not found */
    戻る nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(const char16_t* s U_LIFETIME_BOUND, const char16_t* substring) {
    戻る u_strFindLast(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(const char16_t* s U_LIFETIME_BOUND, char16_t c) {
    もし(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        戻る u_strFindLast(s, -1, &c, 1);
    } 否则 {
        const char16_t *result=nullptr;
        char16_t cs;

        /* trivial search for a BMP code point */
        ために(;;) {
            もし((cs=*s)==c) {
                result=s;
            }
            もし(cs==0) {
                戻る (char16_t *)result;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    もし((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        戻る u_strrchr(s, (char16_t)c);
    } 否则 もし((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        const char16_t *result=nullptr;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ながら((cs=*s++)!=0) {
            もし(cs==lead && *s==trail) {
                result=s-1;
            }
        }
        戻る (char16_t *)result;
    } 否则 {
        /* not a Unicode code point, not findable */
        戻る nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(const char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    もし(count<=0) {
        戻る nullptr; /* no string */
    } 否则 もし(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        戻る u_strFindLast(s, count, &c, 1);
    } 否则 {
        /* trivial search for a BMP code point */
        const char16_t *limit=s+count;
        do {
            もし(*(--limit)==c) {
                戻る (char16_t *)limit;
            }
        } ながら(s!=limit);
        戻る nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    もし((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        戻る u_memrchr(s, (char16_t)c, count);
    } 否则 もし(count<2) {
        /* too short for a surrogate pair */
        戻る nullptr;
    } 否则 もし((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        const char16_t *limit=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        do {
            もし(*limit==trail && *(limit-1)==lead) {
                戻る (char16_t *)(limit-1);
            }
        } ながら(s!=--limit);
        戻る nullptr;
    } 否则 {
        /* not a Unicode code point, not findable */
        戻る nullptr;
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
    ながら((c = matchSet[matchBMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++matchBMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    matchLen = matchBMPLen;
    ながら(matchSet[matchLen] != 0) {
        ++matchLen;
    }

    ために(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        もし(U16_IS_SINGLE(c)) {
            もし(polarity) {
                ために(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    もし(c == matchSet[matchItr]) {
                        戻る strItr - 1; /* one matches */
                    }
                }
            } 否则 {
                ために(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    もし(c == matchSet[matchItr]) {
                        goto endloop;
                    }
                }
                戻る strItr - 1; /* none matches */
            }
        } 否则 {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            もし(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } 否则 {
                stringCh = c; /* unpaired trail surrogate */
            }

            もし(polarity) {
                ために(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    もし(stringCh == matchCh) {
                        戻る strItr - U16_LENGTH(stringCh); /* one matches */
                    }
                }
            } 否则 {
                ために(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    もし(stringCh == matchCh) {
                        goto endloop;
                    }
                }
                戻る strItr - U16_LENGTH(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    戻る -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(const char16_t* string U_LIFETIME_BOUND, const char16_t* matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, true);
    もし(idx >= 0) {
        戻る (char16_t *)string + idx;
    } 否则 {
        戻る nullptr;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(const char16_t *string, const char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, true);
    もし(idx >= 0) {
        戻る idx;
    } 否则 {
        戻る -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(const char16_t *string, const char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, false);
    もし(idx >= 0) {
        戻る idx;
    } 否则 {
        戻る -idx - 1; /* == u_strlen(string) */
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
    もし (src != nullptr) {
        tokSource = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    否则 もし (*saveState) {
        tokSource = *saveState;
    }
    否则 {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        戻る nullptr;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tokSource, delim);
    tokSource = &tokSource[nonDelimIdx];

    もし (*tokSource) {
        nextToken = u_strpbrk(tokSource, delim);
        もし (nextToken != nullptr) {
            /* Create a token */
            *(nextToken++) = 0;
            *saveState = nextToken;
            戻る tokSource;
        }
        否则 もし (*saveState) {
            /* Return the last token */
            *saveState = nullptr;
            戻る tokSource;
        }
    }
    否则 {
        /* No tokens were found. Only delimiters were left. */
        *saveState = nullptr;
    }
    戻る nullptr;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, const char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    ながら(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    ながら((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    戻る anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, const char16_t* src, int32_t n)
{
    もし(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        ながら(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        ながら((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            もし(--n == 0) {
                *dst = 0;
                break;
            }
            ++src;
        }

        戻る anchor;
    } 否则 {
        戻る dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(const char16_t *s1,
    const char16_t *s2)
{
    char16_t  c1, c2;

    ために(;;) {
        c1=*s1++;
        c2=*s2++;
        もし (c1 != c2 || c1 == 0) {
            break;
        }
    }
    戻る (int32_t)c1 - (int32_t)c2;
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
    もし(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        もし(s1==s2) {
            戻る 0;
        }

        ために(;;) {
            c1=*s1;
            c2=*s2;
            もし(c1!=c2) {
                break;
            }
            もし(c1==0) {
                戻る 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=nullptr;
    } 否则 もし(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        もし(s1==s2) {
            戻る 0;
        }

        limit1=start1+length1;

        ために(;;) {
            /* both lengths are same, check only one limit */
            もし(s1==limit1) {
                戻る 0;
            }

            c1=*s1;
            c2=*s2;
            もし(c1!=c2) {
                break;
            }
            もし(c1==0) {
                戻る 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } 否则 {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t lengthResult;

        もし(length1<0) {
            length1=u_strlen(s1);
        }
        もし(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        もし(length1<length2) {
            lengthResult=-1;
            limit1=start1+length1;
        } 否则 もし(length1==length2) {
            lengthResult=0;
            limit1=start1+length1;
        } 否则 /* length1>length2 */ {
            lengthResult=1;
            limit1=start1+length2;
        }

        もし(s1==s2) {
            戻る lengthResult;
        }

        ために(;;) {
            /* check pseudo-limit */
            もし(s1==limit1) {
                戻る lengthResult;
            }

            c1=*s1;
            c2=*s2;
            もし(c1!=c2) {
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
    もし(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        もし(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } 否则 {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        もし(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } 否则 {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    戻る (int32_t)c1-(int32_t)c2;
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
    もし(iter1==nullptr || iter2==nullptr) {
        戻る 0; /* bad arguments */
    }
    もし(iter1==iter2) {
        戻る 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_START);
    iter2->move(iter2, 0, UITER_START);

    /* compare identical prefixes - they do not need to be fixed up */
    ために(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        もし(c1!=c2) {
            break;
        }
        もし(c1==-1) {
            戻る 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    もし(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        もし(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->previous(iter1), U16_IS_LEAD(iter1->previous(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } 否则 {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        もし(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->previous(iter2), U16_IS_LEAD(iter2->previous(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } 否则 {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    戻る (int32_t)c1-(int32_t)c2;
}

#もし 0
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
        もし(c1<=0xdbff) {
            もし(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } 否则 もし(c1<=0xdfff) {
            int32_t idx=iter1->getIndex(iter1, UITER_CURRENT);
            iter1->previous(iter1); /* ==c1 */
            もし(!U16_IS_LEAD(iter1->previous(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } 否则 /* 0xe000<=c1<=0xffff */ {
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
    もし(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        戻る 0;
    }
    戻る uprv_strCompare(s1, length1, s2, length2, false, codePointOrder);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmpCodePointOrder(const char16_t *s1, const char16_t *s2) {
    戻る uprv_strCompare(s1, -1, s2, -1, false, true);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(const char16_t  *s1,
     const char16_t  *s2,
     int32_t     n) 
{
    もし(n > 0) {
        int32_t rc;
        ために(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            もし(rc != 0 || *s1 == 0 || --n == 0) {
                戻る rc;
            }
            ++s1;
            ++s2;
        }
    } 否则 {
        戻る 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmpCodePointOrder(const char16_t *s1, const char16_t *s2, int32_t n) {
    戻る uprv_strCompare(s1, n, s2, n, true, true);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, const char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    ながら((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    戻る anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, const char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    ながら(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    戻る anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(const char16_t *s)
{
#もし U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    戻る (int32_t)uprv_wcslen((const wchar_t *)s);
#否则
    const char16_t *t = s;
    ながら(*t != 0) {
      ++t;
    }
    戻る t - s;
#endif
}

U_CAPI int32_t U_EXPORT2
u_countChar32(const char16_t *s, int32_t length) {
    int32_t count;

    もし(s==nullptr || length<-1) {
        戻る 0;
    }

    count=0;
    もし(length>=0) {
        ながら(length>0) {
            ++count;
            もし(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } 否则 {
                ++s;
                --length;
            }
        }
    } 否则 /* length==-1 */ {
        char16_t c;

        ために(;;) {
            もし((c=*s++)==0) {
                break;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            もし(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    戻る count;
}

U_CAPI UBool U_EXPORT2
u_strHasMoreChar32Than(const char16_t *s, int32_t length, int32_t number) {

    もし(number<0) {
        戻る true;
    }
    もし(s==nullptr || length<-1) {
        戻る false;
    }

    もし(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        ために(;;) {
            もし((c=*s++)==0) {
                戻る false;
            }
            もし(number==0) {
                戻る true;
            }
            もし(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } 否则 {
        /* length>=0 known */
        const char16_t *limit;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        もし(((length+1)/2)>number) {
            戻る true;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        もし(maxSupplementary<=0) {
            戻る false;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        limit=s+length;
        ために(;;) {
            もし(s==limit) {
                戻る false;
            }
            もし(number==0) {
                戻る true;
            }
            もし(U16_IS_LEAD(*s++) && s!=limit && U16_IS_TRAIL(*s)) {
                ++s;
                もし(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    戻る false;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, const char16_t* src, int32_t count) {
    もし(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    戻る dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, const char16_t* src, int32_t count) {
    もし(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    戻る dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    もし(count > 0) {
        char16_t *ptr = dest;
        char16_t *limit = dest + count;

        ながら (ptr < limit) {
            *(ptr++) = c;
        }
    }
    戻る dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(const char16_t *buf1, const char16_t *buf2, int32_t count) {
    もし(count > 0) {
        const char16_t *limit = buf1 + count;
        int32_t result;

        ながら (buf1 < limit) {
            result = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            もし (result != 0) {
                戻る result;
            }
            buf1++;
            buf2++;
        }
    }
    戻る 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmpCodePointOrder(const char16_t *s1, const char16_t *s2, int32_t count) {
    戻る uprv_strCompare(s1, count, s2, count, false, true);
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
    もし (c >= u'0' && c <= u'7') {
        戻る c - u'0';
    }
    戻る -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
static int32_t _digit16(char16_t c) {
    もし (c >= u'0' && c <= u'9') {
        戻る c - u'0';
    }
    もし (c >= u'A' && c <= u'F') {
        戻る c - (u'A' - 10);
    }
    もし (c >= u'a' && c <= u'f') {
        戻る c - (u'a' - 10);
    }
    戻る -1;
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
    もし (*offset < 0 || *offset >= length) {
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
        もし (*offset < length && charAt(*offset, context) == u'{') {
            ++(*offset);
            braces = true;
            maxDig = 8;
        } 否则 {
            maxDig = 2;
        }
        break;
    default:
        dig = _digit8(c);
        もし (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            bitsPerDigit = 3;
            result = dig;
        }
        break;
    }
    もし (minDig != 0) {
        ながら (*offset < length && n < maxDig) {
            c = charAt(*offset, context);
            dig = (bitsPerDigit == 3) ? _digit8(c) : _digit16(c);
            もし (dig < 0) {
                break;
            }
            result = (result << bitsPerDigit) | dig;
            ++(*offset);
            ++n;
        }
        もし (n < minDig) {
            goto err;
        }
        もし (braces) {
            もし (c != u'}') {
                goto err;
            }
            ++(*offset);
        }
        もし (result < 0 || result >= 0x110000) {
            goto err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        もし (*offset < length && U16_IS_LEAD(result)) {
            int32_t ahead = *offset + 1;
            c = charAt(*offset, context);
            もし (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t tailLimit = ahead + 11;
                もし (tailLimit > length) {
                    tailLimit = length;
                }
                c = u_unescapeAt(charAt, &ahead, tailLimit, context);
            }
            もし (U16_IS_TRAIL(c)) {
                *offset = ahead;
                result = U16_GET_SUPPLEMENTARY(result, c);
            }
        }
        戻る result;
    }

    /* Convert C-style escapes in table */
    ために (int32_t i=0; i<UNESCAPE_MAP_LENGTH; i+=2) {
        もし (c == UNESCAPE_MAP[i]) {
            戻る UNESCAPE_MAP[i+1];
        } 否则 もし (c < UNESCAPE_MAP[i]) {
            break;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    もし (c == u'c' && *offset < length) {
        c = charAt((*offset)++, context);
        もし (U16_IS_LEAD(c) && *offset < length) {
            char16_t c2 = charAt(*offset, context);
            もし (U16_IS_TRAIL(c2)) {
                ++(*offset);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        戻る 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    もし (U16_IS_LEAD(c) && *offset < length) {
        char16_t c2 = charAt(*offset, context);
        もし (U16_IS_TRAIL(c2)) {
            ++(*offset);
            戻る U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    戻る c;

 err:
    /* Invalid escape sequence */
    *offset = start; /* Reset to initial value */
    戻る (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
static char16_t U_CALLCONV
_charPtr_charAt(int32_t offset, void *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_charsToUChars(static_cast<char*>(context) + offset, &c16, 1);
    戻る c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
static void _appendUChars(char16_t *dest, int32_t destCapacity,
                          const char *src, int32_t srcLen) {
    もし (destCapacity < 0) {
        destCapacity = 0;
    }
    もし (srcLen > destCapacity) {
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

    ながら ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        もし (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            もし (src != segment) {
                もし (dest != nullptr) {
                    _appendUChars(dest + i, destCapacity - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<char*>(src));
            もし (lenParsed == 0) {
                goto err;
            }
            src += lenParsed; /* advance past escape seq. */
            もし (dest != nullptr && U16_LENGTH(c32) <= (destCapacity - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } 否则 {
                i += U16_LENGTH(c32);
            }
            segment = src;
        } 否则 {
            ++src;
        }
    }
    もし (src != segment) {
        もし (dest != nullptr) {
            _appendUChars(dest + i, destCapacity - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    もし (dest != nullptr && i < destCapacity) {
        dest[i] = 0;
    }
    戻る i;

 err:
    もし (dest != nullptr && destCapacity > 0) {
        *dest = 0;
    }
    戻る 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#define __TERMINATE_STRING(dest, destCapacity, length, pErrorCode) UPRV_BLOCK_MACRO_BEGIN { \
    もし(pErrorCode!=nullptr && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        もし(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } 否则 もし(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            もし(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } 否则 もし(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } 否则 /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;                        \
        }                                                               \
    } \
} UPRV_BLOCK_MACRO_END

U_CAPI char16_t U_EXPORT2
u_asciiToUpper(char16_t c) {
    もし (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    戻る c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    戻る length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(char *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    戻る length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    戻る length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    戻る length;
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
    もし (p != nullptr) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        const TYPE *limit = p + len;          \
        ながら (p<limit) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    戻る static_cast<int32_t>(hash);        \
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
