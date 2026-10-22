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
static anta_hstha UBool
isMatchAtCPBoundary(const char16_t *start, const char16_t *match, const char16_t *matchLimit, const char16_t *limit) {
    གལ་ཏེ(U16_IS_TRAIL(*match) && start!=match && U16_IS_LEAD(*(match-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        ཕྱིར false;
    }
    གལ་ཏེ(U16_IS_LEAD(*(matchLimit-1)) && matchLimit!=limit && U16_IS_TRAIL(*matchLimit)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        ཕྱིར false;
    }
    ཕྱིར true;
}

U_CAPI char16_t* U_EXPORT2
u_strFindFirst(const char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               const char16_t* sub,
               int32_t subLength) {
    const char16_t *start, *p, *q, *subLimit;
    char16_t c, cs, cq;

    གལ་ཏེ(sub==nullptr || subLength<-1) {
        ཕྱིར (char16_t *)s;
    }
    གལ་ཏེ(s==nullptr || length<-1) {
        ཕྱིར nullptr;
    }

    start=s;

    གལ་ཏེ(length<0 && subLength<0) {
        /* both strings are NUL-terminated */
        གལ་ཏེ((cs=*sub++)==0) {
            ཕྱིར (char16_t *)s;
        }
        གལ་ཏེ(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            ཕྱིར u_strchr(s, cs);
        }

        སྐབས((c=*s++)!=0) {
            གལ་ཏེ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                ཆེད་དུ(;;) {
                    གལ་ཏེ((cq=*q)==0) {
                        གལ་ཏེ(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            ཕྱིར (char16_t *)(s-1); /* well-formed match */
                        } དེ་མིན {
                            bha_nga; /* no match because surrogate pair is split */
                        }
                    }
                    གལ་ཏེ((c=*p)==0) {
                        ཕྱིར nullptr; /* no match, and none possible after s */
                    }
                    གལ་ཏེ(c!=cq) {
                        bha_nga; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        ཕྱིར nullptr;
    }

    གལ་ཏེ(subLength<0) {
        subLength=u_strlen(sub);
    }
    གལ་ཏེ(subLength==0) {
        ཕྱིར (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --subLength;
    subLimit=sub+subLength;

    གལ་ཏེ(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        ཕྱིར length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    གལ་ཏེ(length<0) {
        /* s is NUL-terminated */
        སྐབས((c=*s++)!=0) {
            གལ་ཏེ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                ཆེད་དུ(;;) {
                    གལ་ཏེ(q==subLimit) {
                        གལ་ཏེ(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            ཕྱིར (char16_t *)(s-1); /* well-formed match */
                        } དེ་མིན {
                            bha_nga; /* no match because surrogate pair is split */
                        }
                    }
                    གལ་ཏེ((c=*p)==0) {
                        ཕྱིར nullptr; /* no match, and none possible after s */
                    }
                    གལ་ཏེ(c!=*q) {
                        bha_nga; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } དེ་མིན {
        const char16_t *limit, *preLimit;

        /* subLength was decremented above */
        གལ་ཏེ(length<=subLength) {
            ཕྱིར nullptr; /* s is shorter than sub */
        }

        limit=s+length;

        /* the substring must start before preLimit */
        preLimit=limit-subLength;

        སྐབས(s!=preLimit) {
            c=*s++;
            གལ་ཏེ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                ཆེད་དུ(;;) {
                    གལ་ཏེ(q==subLimit) {
                        གལ་ཏེ(isMatchAtCPBoundary(start, s-1, p, limit)) {
                            ཕྱིར (char16_t *)(s-1); /* well-formed match */
                        } དེ་མིན {
                            bha_nga; /* no match because surrogate pair is split */
                        }
                    }
                    གལ་ཏེ(*p!=*q) {
                        bha_nga; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    ཕྱིར nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(const char16_t* s U_LIFETIME_BOUND, const char16_t* substring) {
    ཕྱིར u_strFindFirst(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(const char16_t* s U_LIFETIME_BOUND, char16_t c) {
    གལ་ཏེ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ཕྱིར u_strFindFirst(s, -1, &c, 1);
    } དེ་མིན {
        char16_t cs;

        /* trivial search for a BMP code point */
        ཆེད་དུ(;;) {
            གལ་ཏེ((cs=*s)==c) {
                ཕྱིར (char16_t *)s;
            }
            གལ་ཏེ(cs==0) {
                ཕྱིར nullptr;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    གལ་ཏེ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ཕྱིར u_strchr(s, (char16_t)c);
    } དེ་མིན གལ་ཏེ((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        སྐབས((cs=*s++)!=0) {
            གལ་ཏེ(cs==lead && *s==trail) {
                ཕྱིར (char16_t *)(s-1);
            }
        }
        ཕྱིར nullptr;
    } དེ་མིན {
        /* not a Unicode code point, not findable */
        ཕྱིར nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(const char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    གལ་ཏེ(count<=0) {
        ཕྱིར nullptr; /* no string */
    } དེ་མིན གལ་ཏེ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ཕྱིར u_strFindFirst(s, count, &c, 1);
    } དེ་མིན {
        /* trivial search for a BMP code point */
        const char16_t *limit=s+count;
        do {
            གལ་ཏེ(*s==c) {
                ཕྱིར (char16_t *)s;
            }
        } སྐབས(++s!=limit);
        ཕྱིར nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    གལ་ཏེ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ཕྱིར u_memchr(s, (char16_t)c, count);
    } དེ་མིན གལ་ཏེ(count<2) {
        /* too short for a surrogate pair */
        ཕྱིར nullptr;
    } དེ་མིན གལ་ཏེ((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        const char16_t *limit=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        do {
            གལ་ཏེ(*s==lead && *(s+1)==trail) {
                ཕྱིར (char16_t *)s;
            }
        } སྐབས(++s!=limit);
        ཕྱིར nullptr;
    } དེ་མིན {
        /* not a Unicode code point, not findable */
        ཕྱིར nullptr;
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

    གལ་ཏེ(sub==nullptr || subLength<-1) {
        ཕྱིར (char16_t *)s;
    }
    གལ་ཏེ(s==nullptr || length<-1) {
        ཕྱིར nullptr;
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

    གལ་ཏེ(subLength<0) {
        subLength=u_strlen(sub);
    }
    གལ་ཏེ(subLength==0) {
        ཕྱིར (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    subLimit=sub+subLength;
    cs=*(--subLimit);
    --subLength;

    གལ་ཏེ(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        ཕྱིར length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    གལ་ཏེ(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    གལ་ཏེ(length<=subLength) {
        ཕྱིར nullptr; /* s is shorter than sub */
    }

    start=s;
    limit=s+length;

    /* the substring must start no later than s+subLength */
    s+=subLength;

    སྐབས(s!=limit) {
        c=*(--limit);
        གལ་ཏེ(c==cs) {
            /* found last substring char16_t, compare rest */
            p=limit;
            q=subLimit;
            ཆེད་དུ(;;) {
                གལ་ཏེ(q==sub) {
                    གལ་ཏེ(isMatchAtCPBoundary(start, p, limit+1, start+length)) {
                        ཕྱིར (char16_t *)p; /* well-formed match */
                    } དེ་མིན {
                        bha_nga; /* no match because surrogate pair is split */
                    }
                }
                གལ་ཏེ(*(--p)!=*(--q)) {
                    bha_nga; /* no match */
                }
            }
        }
    }

    /* not found */
    ཕྱིར nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(const char16_t* s U_LIFETIME_BOUND, const char16_t* substring) {
    ཕྱིར u_strFindLast(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(const char16_t* s U_LIFETIME_BOUND, char16_t c) {
    གལ་ཏེ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ཕྱིར u_strFindLast(s, -1, &c, 1);
    } དེ་མིན {
        const char16_t *result=nullptr;
        char16_t cs;

        /* trivial search for a BMP code point */
        ཆེད་དུ(;;) {
            གལ་ཏེ((cs=*s)==c) {
                result=s;
            }
            གལ་ཏེ(cs==0) {
                ཕྱིར (char16_t *)result;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    གལ་ཏེ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ཕྱིར u_strrchr(s, (char16_t)c);
    } དེ་མིན གལ་ཏེ((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        const char16_t *result=nullptr;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        སྐབས((cs=*s++)!=0) {
            གལ་ཏེ(cs==lead && *s==trail) {
                result=s-1;
            }
        }
        ཕྱིར (char16_t *)result;
    } དེ་མིན {
        /* not a Unicode code point, not findable */
        ཕྱིར nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(const char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    གལ་ཏེ(count<=0) {
        ཕྱིར nullptr; /* no string */
    } དེ་མིན གལ་ཏེ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ཕྱིར u_strFindLast(s, count, &c, 1);
    } དེ་མིན {
        /* trivial search for a BMP code point */
        const char16_t *limit=s+count;
        do {
            གལ་ཏེ(*(--limit)==c) {
                ཕྱིར (char16_t *)limit;
            }
        } སྐབས(s!=limit);
        ཕྱིར nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    གལ་ཏེ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ཕྱིར u_memrchr(s, (char16_t)c, count);
    } དེ་མིན གལ་ཏེ(count<2) {
        /* too short for a surrogate pair */
        ཕྱིར nullptr;
    } དེ་མིན གལ་ཏེ((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        const char16_t *limit=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        do {
            གལ་ཏེ(*limit==trail && *(limit-1)==lead) {
                ཕྱིར (char16_t *)(limit-1);
            }
        } སྐབས(s!=--limit);
        ཕྱིར nullptr;
    } དེ་མིན {
        /* not a Unicode code point, not findable */
        ཕྱིར nullptr;
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
    སྐབས((c = matchSet[matchBMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++matchBMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    matchLen = matchBMPLen;
    སྐབས(matchSet[matchLen] != 0) {
        ++matchLen;
    }

    ཆེད་དུ(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        གལ་ཏེ(U16_IS_SINGLE(c)) {
            གལ་ཏེ(polarity) {
                ཆེད་དུ(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    གལ་ཏེ(c == matchSet[matchItr]) {
                        ཕྱིར strItr - 1; /* one matches */
                    }
                }
            } དེ་མིན {
                ཆེད་དུ(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    གལ་ཏེ(c == matchSet[matchItr]) {
                        goto endloop;
                    }
                }
                ཕྱིར strItr - 1; /* none matches */
            }
        } དེ་མིན {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            གལ་ཏེ(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } དེ་མིན {
                stringCh = c; /* unpaired trail surrogate */
            }

            གལ་ཏེ(polarity) {
                ཆེད་དུ(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    གལ་ཏེ(stringCh == matchCh) {
                        ཕྱིར strItr - U16_LENGTH(stringCh); /* one matches */
                    }
                }
            } དེ་མིན {
                ཆེད་དུ(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    གལ་ཏེ(stringCh == matchCh) {
                        goto endloop;
                    }
                }
                ཕྱིར strItr - U16_LENGTH(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    ཕྱིར -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(const char16_t* string U_LIFETIME_BOUND, const char16_t* matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, true);
    གལ་ཏེ(idx >= 0) {
        ཕྱིར (char16_t *)string + idx;
    } དེ་མིན {
        ཕྱིར nullptr;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(const char16_t *string, const char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, true);
    གལ་ཏེ(idx >= 0) {
        ཕྱིར idx;
    } དེ་མིན {
        ཕྱིར -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(const char16_t *string, const char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, false);
    གལ་ཏེ(idx >= 0) {
        ཕྱིར idx;
    } དེ་མིན {
        ཕྱིར -idx - 1; /* == u_strlen(string) */
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
    གལ་ཏེ (src != nullptr) {
        tokSource = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    དེ་མིན གལ་ཏེ (*saveState) {
        tokSource = *saveState;
    }
    དེ་མིན {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        ཕྱིར nullptr;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tokSource, delim);
    tokSource = &tokSource[nonDelimIdx];

    གལ་ཏེ (*tokSource) {
        nextToken = u_strpbrk(tokSource, delim);
        གལ་ཏེ (nextToken != nullptr) {
            /* Create a token */
            *(nextToken++) = 0;
            *saveState = nextToken;
            ཕྱིར tokSource;
        }
        དེ་མིན གལ་ཏེ (*saveState) {
            /* Return the last token */
            *saveState = nullptr;
            ཕྱིར tokSource;
        }
    }
    དེ་མིན {
        /* No tokens were found. Only delimiters were left. */
        *saveState = nullptr;
    }
    ཕྱིར nullptr;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, const char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    སྐབས(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    སྐབས((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    ཕྱིར anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, const char16_t* src, int32_t n)
{
    གལ་ཏེ(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        སྐབས(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        སྐབས((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            གལ་ཏེ(--n == 0) {
                *dst = 0;
                bha_nga;
            }
            ++src;
        }

        ཕྱིར anchor;
    } དེ་མིན {
        ཕྱིར dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(const char16_t *s1,
    const char16_t *s2)
{
    char16_t  c1, c2;

    ཆེད་དུ(;;) {
        c1=*s1++;
        c2=*s2++;
        གལ་ཏེ (c1 != c2 || c1 == 0) {
            bha_nga;
        }
    }
    ཕྱིར (int32_t)c1 - (int32_t)c2;
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
    གལ་ཏེ(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        གལ་ཏེ(s1==s2) {
            ཕྱིར 0;
        }

        ཆེད་དུ(;;) {
            c1=*s1;
            c2=*s2;
            གལ་ཏེ(c1!=c2) {
                bha_nga;
            }
            གལ་ཏེ(c1==0) {
                ཕྱིར 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=nullptr;
    } དེ་མིན གལ་ཏེ(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        གལ་ཏེ(s1==s2) {
            ཕྱིར 0;
        }

        limit1=start1+length1;

        ཆེད་དུ(;;) {
            /* both lengths are same, check only one limit */
            གལ་ཏེ(s1==limit1) {
                ཕྱིར 0;
            }

            c1=*s1;
            c2=*s2;
            གལ་ཏེ(c1!=c2) {
                bha_nga;
            }
            གལ་ཏེ(c1==0) {
                ཕྱིར 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } དེ་མིན {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t lengthResult;

        གལ་ཏེ(length1<0) {
            length1=u_strlen(s1);
        }
        གལ་ཏེ(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        གལ་ཏེ(length1<length2) {
            lengthResult=-1;
            limit1=start1+length1;
        } དེ་མིན གལ་ཏེ(length1==length2) {
            lengthResult=0;
            limit1=start1+length1;
        } དེ་མིན /* length1>length2 */ {
            lengthResult=1;
            limit1=start1+length2;
        }

        གལ་ཏེ(s1==s2) {
            ཕྱིར lengthResult;
        }

        ཆེད་དུ(;;) {
            /* check pseudo-limit */
            གལ་ཏེ(s1==limit1) {
                ཕྱིར lengthResult;
            }

            c1=*s1;
            c2=*s2;
            གལ་ཏེ(c1!=c2) {
                bha_nga;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    གལ་ཏེ(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        གལ་ཏེ(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } དེ་མིན {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        གལ་ཏེ(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } དེ་མིན {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    ཕྱིར (int32_t)c1-(int32_t)c2;
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
    གལ་ཏེ(iter1==nullptr || iter2==nullptr) {
        ཕྱིར 0; /* bad arguments */
    }
    གལ་ཏེ(iter1==iter2) {
        ཕྱིར 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_START);
    iter2->move(iter2, 0, UITER_START);

    /* compare identical prefixes - they do not need to be fixed up */
    ཆེད་དུ(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        གལ་ཏེ(c1!=c2) {
            bha_nga;
        }
        གལ་ཏེ(c1==-1) {
            ཕྱིར 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    གལ་ཏེ(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        གལ་ཏེ(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->previous(iter1), U16_IS_LEAD(iter1->previous(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } དེ་མིན {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        གལ་ཏེ(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->previous(iter2), U16_IS_LEAD(iter2->previous(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } དེ་མིན {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    ཕྱིར (int32_t)c1-(int32_t)c2;
}

#གལ་ཏེ 0
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
        གལ་ཏེ(c1<=0xdbff) {
            གལ་ཏེ(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } དེ་མིན གལ་ཏེ(c1<=0xdfff) {
            int32_t idx=iter1->getIndex(iter1, UITER_CURRENT);
            iter1->previous(iter1); /* ==c1 */
            གལ་ཏེ(!U16_IS_LEAD(iter1->previous(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } དེ་མིན /* 0xe000<=c1<=0xffff */ {
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
    གལ་ཏེ(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        ཕྱིར 0;
    }
    ཕྱིར uprv_strCompare(s1, length1, s2, length2, false, codePointOrder);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmpCodePointOrder(const char16_t *s1, const char16_t *s2) {
    ཕྱིར uprv_strCompare(s1, -1, s2, -1, false, true);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(const char16_t  *s1,
     const char16_t  *s2,
     int32_t     n) 
{
    གལ་ཏེ(n > 0) {
        int32_t rc;
        ཆེད་དུ(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            གལ་ཏེ(rc != 0 || *s1 == 0 || --n == 0) {
                ཕྱིར rc;
            }
            ++s1;
            ++s2;
        }
    } དེ་མིན {
        ཕྱིར 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmpCodePointOrder(const char16_t *s1, const char16_t *s2, int32_t n) {
    ཕྱིར uprv_strCompare(s1, n, s2, n, true, true);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, const char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    སྐབས((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    ཕྱིར anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, const char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    སྐབས(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    ཕྱིར anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(const char16_t *s)
{
#གལ་ཏེ U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    ཕྱིར (int32_t)uprv_wcslen((const wchar_t *)s);
#དེ་མིན
    const char16_t *t = s;
    སྐབས(*t != 0) {
      ++t;
    }
    ཕྱིར t - s;
#endif
}

U_CAPI int32_t U_EXPORT2
u_countChar32(const char16_t *s, int32_t length) {
    int32_t count;

    གལ་ཏེ(s==nullptr || length<-1) {
        ཕྱིར 0;
    }

    count=0;
    གལ་ཏེ(length>=0) {
        སྐབས(length>0) {
            ++count;
            གལ་ཏེ(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } དེ་མིན {
                ++s;
                --length;
            }
        }
    } དེ་མིན /* length==-1 */ {
        char16_t c;

        ཆེད་དུ(;;) {
            གལ་ཏེ((c=*s++)==0) {
                bha_nga;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            གལ་ཏེ(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    ཕྱིར count;
}

U_CAPI UBool U_EXPORT2
u_strHasMoreChar32Than(const char16_t *s, int32_t length, int32_t number) {

    གལ་ཏེ(number<0) {
        ཕྱིར true;
    }
    གལ་ཏེ(s==nullptr || length<-1) {
        ཕྱིར false;
    }

    གལ་ཏེ(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        ཆེད་དུ(;;) {
            གལ་ཏེ((c=*s++)==0) {
                ཕྱིར false;
            }
            གལ་ཏེ(number==0) {
                ཕྱིར true;
            }
            གལ་ཏེ(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } དེ་མིན {
        /* length>=0 known */
        const char16_t *limit;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        གལ་ཏེ(((length+1)/2)>number) {
            ཕྱིར true;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        གལ་ཏེ(maxSupplementary<=0) {
            ཕྱིར false;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        limit=s+length;
        ཆེད་དུ(;;) {
            གལ་ཏེ(s==limit) {
                ཕྱིར false;
            }
            གལ་ཏེ(number==0) {
                ཕྱིར true;
            }
            གལ་ཏེ(U16_IS_LEAD(*s++) && s!=limit && U16_IS_TRAIL(*s)) {
                ++s;
                གལ་ཏེ(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    ཕྱིར false;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, const char16_t* src, int32_t count) {
    གལ་ཏེ(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    ཕྱིར dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, const char16_t* src, int32_t count) {
    གལ་ཏེ(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    ཕྱིར dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    གལ་ཏེ(count > 0) {
        char16_t *ptr = dest;
        char16_t *limit = dest + count;

        སྐབས (ptr < limit) {
            *(ptr++) = c;
        }
    }
    ཕྱིར dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(const char16_t *buf1, const char16_t *buf2, int32_t count) {
    གལ་ཏེ(count > 0) {
        const char16_t *limit = buf1 + count;
        int32_t result;

        སྐབས (buf1 < limit) {
            result = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            གལ་ཏེ (result != 0) {
                ཕྱིར result;
            }
            buf1++;
            buf2++;
        }
    }
    ཕྱིར 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmpCodePointOrder(const char16_t *s1, const char16_t *s2, int32_t count) {
    ཕྱིར uprv_strCompare(s1, count, s2, count, false, true);
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
ga_nanaa { UNESCAPE_MAP_LENGTH = UPRV_LENGTHOF(UNESCAPE_MAP) };

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
static int32_t _digit8(char16_t c) {
    གལ་ཏེ (c >= u'0' && c <= u'7') {
        ཕྱིར c - u'0';
    }
    ཕྱིར -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
static int32_t _digit16(char16_t c) {
    གལ་ཏེ (c >= u'0' && c <= u'9') {
        ཕྱིར c - u'0';
    }
    གལ་ཏེ (c >= u'A' && c <= u'F') {
        ཕྱིར c - (u'A' - 10);
    }
    གལ་ཏེ (c >= u'a' && c <= u'f') {
        ཕྱིར c - (u'a' - 10);
    }
    ཕྱིར -1;
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
    གལ་ཏེ (*offset < 0 || *offset >= length) {
        goto err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*offset)++, context);

    /* Convert hexadecimal and octal escapes */
    switch (c) {
    pak_sa u'u':
        minDig = maxDig = 4;
        bha_nga;
    pak_sa u'U':
        minDig = maxDig = 8;
        bha_nga;
    pak_sa u'x':
        minDig = 1;
        གལ་ཏེ (*offset < length && charAt(*offset, context) == u'{') {
            ++(*offset);
            braces = true;
            maxDig = 8;
        } དེ་མིན {
            maxDig = 2;
        }
        bha_nga;
    default:
        dig = _digit8(c);
        གལ་ཏེ (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            bitsPerDigit = 3;
            result = dig;
        }
        bha_nga;
    }
    གལ་ཏེ (minDig != 0) {
        སྐབས (*offset < length && n < maxDig) {
            c = charAt(*offset, context);
            dig = (bitsPerDigit == 3) ? _digit8(c) : _digit16(c);
            གལ་ཏེ (dig < 0) {
                bha_nga;
            }
            result = (result << bitsPerDigit) | dig;
            ++(*offset);
            ++n;
        }
        གལ་ཏེ (n < minDig) {
            goto err;
        }
        གལ་ཏེ (braces) {
            གལ་ཏེ (c != u'}') {
                goto err;
            }
            ++(*offset);
        }
        གལ་ཏེ (result < 0 || result >= 0x110000) {
            goto err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        གལ་ཏེ (*offset < length && U16_IS_LEAD(result)) {
            int32_t ahead = *offset + 1;
            c = charAt(*offset, context);
            གལ་ཏེ (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t tailLimit = ahead + 11;
                གལ་ཏེ (tailLimit > length) {
                    tailLimit = length;
                }
                c = u_unescapeAt(charAt, &ahead, tailLimit, context);
            }
            གལ་ཏེ (U16_IS_TRAIL(c)) {
                *offset = ahead;
                result = U16_GET_SUPPLEMENTARY(result, c);
            }
        }
        ཕྱིར result;
    }

    /* Convert C-style escapes in table */
    ཆེད་དུ (int32_t i=0; i<UNESCAPE_MAP_LENGTH; i+=2) {
        གལ་ཏེ (c == UNESCAPE_MAP[i]) {
            ཕྱིར UNESCAPE_MAP[i+1];
        } དེ་མིན གལ་ཏེ (c < UNESCAPE_MAP[i]) {
            bha_nga;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    གལ་ཏེ (c == u'c' && *offset < length) {
        c = charAt((*offset)++, context);
        གལ་ཏེ (U16_IS_LEAD(c) && *offset < length) {
            char16_t c2 = charAt(*offset, context);
            གལ་ཏེ (U16_IS_TRAIL(c2)) {
                ++(*offset);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        ཕྱིར 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    གལ་ཏེ (U16_IS_LEAD(c) && *offset < length) {
        char16_t c2 = charAt(*offset, context);
        གལ་ཏེ (U16_IS_TRAIL(c2)) {
            ++(*offset);
            ཕྱིར U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    ཕྱིར c;

 err:
    /* Invalid escape sequence */
    *offset = start; /* Reset to initial value */
    ཕྱིར (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
static char16_t U_CALLCONV
_charPtr_charAt(int32_t offset, void *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_charsToUChars(static_cast<var_na*>(context) + offset, &c16, 1);
    ཕྱིར c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
static void _appendUChars(char16_t *dest, int32_t destCapacity,
                          const var_na *src, int32_t srcLen) {
    གལ་ཏེ (destCapacity < 0) {
        destCapacity = 0;
    }
    གལ་ཏེ (srcLen > destCapacity) {
        srcLen = destCapacity;
    }
    u_charsToUChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(const var_na *src, char16_t *dest, int32_t destCapacity) {
    const var_na *segment = src;
    int32_t i = 0;
    var_na c;

    སྐབས ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        གལ་ཏེ (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            གལ་ཏེ (src != segment) {
                གལ་ཏེ (dest != nullptr) {
                    _appendUChars(dest + i, destCapacity - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<var_na*>(src));
            གལ་ཏེ (lenParsed == 0) {
                goto err;
            }
            src += lenParsed; /* advance past escape seq. */
            གལ་ཏེ (dest != nullptr && U16_LENGTH(c32) <= (destCapacity - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } དེ་མིན {
                i += U16_LENGTH(c32);
            }
            segment = src;
        } དེ་མིན {
            ++src;
        }
    }
    གལ་ཏེ (src != segment) {
        གལ་ཏེ (dest != nullptr) {
            _appendUChars(dest + i, destCapacity - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    གལ་ཏེ (dest != nullptr && i < destCapacity) {
        dest[i] = 0;
    }
    ཕྱིར i;

 err:
    གལ་ཏེ (dest != nullptr && destCapacity > 0) {
        *dest = 0;
    }
    ཕྱིར 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#define __TERMINATE_STRING(dest, destCapacity, length, pErrorCode) UPRV_BLOCK_MACRO_BEGIN { \
    གལ་ཏེ(pErrorCode!=nullptr && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        གལ་ཏེ(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } དེ་མིན གལ་ཏེ(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            གལ་ཏེ(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } དེ་མིན གལ་ཏེ(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } དེ་མིན /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;                        \
        }                                                               \
    } \
} UPRV_BLOCK_MACRO_END

U_CAPI char16_t U_EXPORT2
u_asciiToUpper(char16_t c) {
    གལ་ཏེ (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    ཕྱིར c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ཕྱིར length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(var_na *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ཕྱིར length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ཕྱིར length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ཕྱིར length;
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
    གལ་ཏེ (p != nullptr) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        const TYPE *limit = p + len;          \
        སྐབས (p<limit) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    ཕྱིར static_cast<int32_t>(hash);        \
} UPRV_BLOCK_MACRO_END

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(const char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(const var_na *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(const var_na *str, int32_t length) {
    STRING_HASH(var_na, str, length, (uint8_t)uprv_tolower(*p));
}
