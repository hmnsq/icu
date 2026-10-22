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
    ᨀᨘ(U16_IS_TRAIL(*match) && start!=match && U16_IS_LEAD(*(match-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        ᨀᨘᨒ false;
    }
    ᨀᨘ(U16_IS_LEAD(*(matchLimit-1)) && matchLimit!=limit && U16_IS_TRAIL(*matchLimit)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        ᨀᨘᨒ false;
    }
    ᨀᨘᨒ true;
}

U_CAPI char16_t* U_EXPORT2
u_strFindFirst(const char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               const char16_t* sub,
               int32_t subLength) {
    const char16_t *start, *p, *q, *subLimit;
    char16_t c, cs, cq;

    ᨀᨘ(sub==nullptr || subLength<-1) {
        ᨀᨘᨒ (char16_t *)s;
    }
    ᨀᨘ(s==nullptr || length<-1) {
        ᨀᨘᨒ nullptr;
    }

    start=s;

    ᨀᨘ(length<0 && subLength<0) {
        /* both strings are NUL-terminated */
        ᨀᨘ((cs=*sub++)==0) {
            ᨀᨘᨒ (char16_t *)s;
        }
        ᨀᨘ(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            ᨀᨘᨒ u_strchr(s, cs);
        }

        ᨀᨛ((c=*s++)!=0) {
            ᨀᨘ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                for(;;) {
                    ᨀᨘ((cq=*q)==0) {
                        ᨀᨘ(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            ᨀᨘᨒ (char16_t *)(s-1); /* well-formed match */
                        } ᨀᨘᨁ {
                            bha_nga; /* no match because surrogate pair is split */
                        }
                    }
                    ᨀᨘ((c=*p)==0) {
                        ᨀᨘᨒ nullptr; /* no match, and none possible after s */
                    }
                    ᨀᨘ(c!=cq) {
                        bha_nga; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        ᨀᨘᨒ nullptr;
    }

    ᨀᨘ(subLength<0) {
        subLength=u_strlen(sub);
    }
    ᨀᨘ(subLength==0) {
        ᨀᨘᨒ (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --subLength;
    subLimit=sub+subLength;

    ᨀᨘ(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        ᨀᨘᨒ length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    ᨀᨘ(length<0) {
        /* s is NUL-terminated */
        ᨀᨛ((c=*s++)!=0) {
            ᨀᨘ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                for(;;) {
                    ᨀᨘ(q==subLimit) {
                        ᨀᨘ(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            ᨀᨘᨒ (char16_t *)(s-1); /* well-formed match */
                        } ᨀᨘᨁ {
                            bha_nga; /* no match because surrogate pair is split */
                        }
                    }
                    ᨀᨘ((c=*p)==0) {
                        ᨀᨘᨒ nullptr; /* no match, and none possible after s */
                    }
                    ᨀᨘ(c!=*q) {
                        bha_nga; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } ᨀᨘᨁ {
        const char16_t *limit, *preLimit;

        /* subLength was decremented above */
        ᨀᨘ(length<=subLength) {
            ᨀᨘᨒ nullptr; /* s is shorter than sub */
        }

        limit=s+length;

        /* the substring must start before preLimit */
        preLimit=limit-subLength;

        ᨀᨛ(s!=preLimit) {
            c=*s++;
            ᨀᨘ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                for(;;) {
                    ᨀᨘ(q==subLimit) {
                        ᨀᨘ(isMatchAtCPBoundary(start, s-1, p, limit)) {
                            ᨀᨘᨒ (char16_t *)(s-1); /* well-formed match */
                        } ᨀᨘᨁ {
                            bha_nga; /* no match because surrogate pair is split */
                        }
                    }
                    ᨀᨘ(*p!=*q) {
                        bha_nga; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    ᨀᨘᨒ nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(const char16_t* s U_LIFETIME_BOUND, const char16_t* substring) {
    ᨀᨘᨒ u_strFindFirst(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(const char16_t* s U_LIFETIME_BOUND, char16_t c) {
    ᨀᨘ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ᨀᨘᨒ u_strFindFirst(s, -1, &c, 1);
    } ᨀᨘᨁ {
        char16_t cs;

        /* trivial search for a BMP code point */
        for(;;) {
            ᨀᨘ((cs=*s)==c) {
                ᨀᨘᨒ (char16_t *)s;
            }
            ᨀᨘ(cs==0) {
                ᨀᨘᨒ nullptr;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    ᨀᨘ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ᨀᨘᨒ u_strchr(s, (char16_t)c);
    } ᨀᨘᨁ ᨀᨘ((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ᨀᨛ((cs=*s++)!=0) {
            ᨀᨘ(cs==lead && *s==trail) {
                ᨀᨘᨒ (char16_t *)(s-1);
            }
        }
        ᨀᨘᨒ nullptr;
    } ᨀᨘᨁ {
        /* not a Unicode code point, not findable */
        ᨀᨘᨒ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(const char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ᨀᨘ(count<=0) {
        ᨀᨘᨒ nullptr; /* no string */
    } ᨀᨘᨁ ᨀᨘ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ᨀᨘᨒ u_strFindFirst(s, count, &c, 1);
    } ᨀᨘᨁ {
        /* trivial search for a BMP code point */
        const char16_t *limit=s+count;
        do {
            ᨀᨘ(*s==c) {
                ᨀᨘᨒ (char16_t *)s;
            }
        } ᨀᨛ(++s!=limit);
        ᨀᨘᨒ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    ᨀᨘ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ᨀᨘᨒ u_memchr(s, (char16_t)c, count);
    } ᨀᨘᨁ ᨀᨘ(count<2) {
        /* too short for a surrogate pair */
        ᨀᨘᨒ nullptr;
    } ᨀᨘᨁ ᨀᨘ((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        const char16_t *limit=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        do {
            ᨀᨘ(*s==lead && *(s+1)==trail) {
                ᨀᨘᨒ (char16_t *)s;
            }
        } ᨀᨛ(++s!=limit);
        ᨀᨘᨒ nullptr;
    } ᨀᨘᨁ {
        /* not a Unicode code point, not findable */
        ᨀᨘᨒ nullptr;
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

    ᨀᨘ(sub==nullptr || subLength<-1) {
        ᨀᨘᨒ (char16_t *)s;
    }
    ᨀᨘ(s==nullptr || length<-1) {
        ᨀᨘᨒ nullptr;
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

    ᨀᨘ(subLength<0) {
        subLength=u_strlen(sub);
    }
    ᨀᨘ(subLength==0) {
        ᨀᨘᨒ (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    subLimit=sub+subLength;
    cs=*(--subLimit);
    --subLength;

    ᨀᨘ(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        ᨀᨘᨒ length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    ᨀᨘ(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    ᨀᨘ(length<=subLength) {
        ᨀᨘᨒ nullptr; /* s is shorter than sub */
    }

    start=s;
    limit=s+length;

    /* the substring must start no later than s+subLength */
    s+=subLength;

    ᨀᨛ(s!=limit) {
        c=*(--limit);
        ᨀᨘ(c==cs) {
            /* found last substring char16_t, compare rest */
            p=limit;
            q=subLimit;
            for(;;) {
                ᨀᨘ(q==sub) {
                    ᨀᨘ(isMatchAtCPBoundary(start, p, limit+1, start+length)) {
                        ᨀᨘᨒ (char16_t *)p; /* well-formed match */
                    } ᨀᨘᨁ {
                        bha_nga; /* no match because surrogate pair is split */
                    }
                }
                ᨀᨘ(*(--p)!=*(--q)) {
                    bha_nga; /* no match */
                }
            }
        }
    }

    /* not found */
    ᨀᨘᨒ nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(const char16_t* s U_LIFETIME_BOUND, const char16_t* substring) {
    ᨀᨘᨒ u_strFindLast(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(const char16_t* s U_LIFETIME_BOUND, char16_t c) {
    ᨀᨘ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ᨀᨘᨒ u_strFindLast(s, -1, &c, 1);
    } ᨀᨘᨁ {
        const char16_t *result=nullptr;
        char16_t cs;

        /* trivial search for a BMP code point */
        for(;;) {
            ᨀᨘ((cs=*s)==c) {
                result=s;
            }
            ᨀᨘ(cs==0) {
                ᨀᨘᨒ (char16_t *)result;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    ᨀᨘ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ᨀᨘᨒ u_strrchr(s, (char16_t)c);
    } ᨀᨘᨁ ᨀᨘ((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        const char16_t *result=nullptr;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ᨀᨛ((cs=*s++)!=0) {
            ᨀᨘ(cs==lead && *s==trail) {
                result=s-1;
            }
        }
        ᨀᨘᨒ (char16_t *)result;
    } ᨀᨘᨁ {
        /* not a Unicode code point, not findable */
        ᨀᨘᨒ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(const char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ᨀᨘ(count<=0) {
        ᨀᨘᨒ nullptr; /* no string */
    } ᨀᨘᨁ ᨀᨘ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ᨀᨘᨒ u_strFindLast(s, count, &c, 1);
    } ᨀᨘᨁ {
        /* trivial search for a BMP code point */
        const char16_t *limit=s+count;
        do {
            ᨀᨘ(*(--limit)==c) {
                ᨀᨘᨒ (char16_t *)limit;
            }
        } ᨀᨛ(s!=limit);
        ᨀᨘᨒ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    ᨀᨘ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ᨀᨘᨒ u_memrchr(s, (char16_t)c, count);
    } ᨀᨘᨁ ᨀᨘ(count<2) {
        /* too short for a surrogate pair */
        ᨀᨘᨒ nullptr;
    } ᨀᨘᨁ ᨀᨘ((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        const char16_t *limit=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        do {
            ᨀᨘ(*limit==trail && *(limit-1)==lead) {
                ᨀᨘᨒ (char16_t *)(limit-1);
            }
        } ᨀᨛ(s!=--limit);
        ᨀᨘᨒ nullptr;
    } ᨀᨘᨁ {
        /* not a Unicode code point, not findable */
        ᨀᨘᨒ nullptr;
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
    ᨀᨛ((c = matchSet[matchBMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++matchBMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    matchLen = matchBMPLen;
    ᨀᨛ(matchSet[matchLen] != 0) {
        ++matchLen;
    }

    for(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        ᨀᨘ(U16_IS_SINGLE(c)) {
            ᨀᨘ(polarity) {
                for(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    ᨀᨘ(c == matchSet[matchItr]) {
                        ᨀᨘᨒ strItr - 1; /* one matches */
                    }
                }
            } ᨀᨘᨁ {
                for(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    ᨀᨘ(c == matchSet[matchItr]) {
                        goto endloop;
                    }
                }
                ᨀᨘᨒ strItr - 1; /* none matches */
            }
        } ᨀᨘᨁ {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            ᨀᨘ(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } ᨀᨘᨁ {
                stringCh = c; /* unpaired trail surrogate */
            }

            ᨀᨘ(polarity) {
                for(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    ᨀᨘ(stringCh == matchCh) {
                        ᨀᨘᨒ strItr - U16_LENGTH(stringCh); /* one matches */
                    }
                }
            } ᨀᨘᨁ {
                for(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    ᨀᨘ(stringCh == matchCh) {
                        goto endloop;
                    }
                }
                ᨀᨘᨒ strItr - U16_LENGTH(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    ᨀᨘᨒ -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(const char16_t* string U_LIFETIME_BOUND, const char16_t* matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, true);
    ᨀᨘ(idx >= 0) {
        ᨀᨘᨒ (char16_t *)string + idx;
    } ᨀᨘᨁ {
        ᨀᨘᨒ nullptr;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(const char16_t *string, const char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, true);
    ᨀᨘ(idx >= 0) {
        ᨀᨘᨒ idx;
    } ᨀᨘᨁ {
        ᨀᨘᨒ -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(const char16_t *string, const char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, false);
    ᨀᨘ(idx >= 0) {
        ᨀᨘᨒ idx;
    } ᨀᨘᨁ {
        ᨀᨘᨒ -idx - 1; /* == u_strlen(string) */
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
    ᨀᨘ (src != nullptr) {
        tokSource = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    ᨀᨘᨁ ᨀᨘ (*saveState) {
        tokSource = *saveState;
    }
    ᨀᨘᨁ {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        ᨀᨘᨒ nullptr;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tokSource, delim);
    tokSource = &tokSource[nonDelimIdx];

    ᨀᨘ (*tokSource) {
        nextToken = u_strpbrk(tokSource, delim);
        ᨀᨘ (nextToken != nullptr) {
            /* Create a token */
            *(nextToken++) = 0;
            *saveState = nextToken;
            ᨀᨘᨒ tokSource;
        }
        ᨀᨘᨁ ᨀᨘ (*saveState) {
            /* Return the last token */
            *saveState = nullptr;
            ᨀᨘᨒ tokSource;
        }
    }
    ᨀᨘᨁ {
        /* No tokens were found. Only delimiters were left. */
        *saveState = nullptr;
    }
    ᨀᨘᨒ nullptr;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, const char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    ᨀᨛ(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    ᨀᨛ((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    ᨀᨘᨒ anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, const char16_t* src, int32_t n)
{
    ᨀᨘ(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        ᨀᨛ(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        ᨀᨛ((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            ᨀᨘ(--n == 0) {
                *dst = 0;
                bha_nga;
            }
            ++src;
        }

        ᨀᨘᨒ anchor;
    } ᨀᨘᨁ {
        ᨀᨘᨒ dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(const char16_t *s1,
    const char16_t *s2)
{
    char16_t  c1, c2;

    for(;;) {
        c1=*s1++;
        c2=*s2++;
        ᨀᨘ (c1 != c2 || c1 == 0) {
            bha_nga;
        }
    }
    ᨀᨘᨒ (int32_t)c1 - (int32_t)c2;
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
    ᨀᨘ(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        ᨀᨘ(s1==s2) {
            ᨀᨘᨒ 0;
        }

        for(;;) {
            c1=*s1;
            c2=*s2;
            ᨀᨘ(c1!=c2) {
                bha_nga;
            }
            ᨀᨘ(c1==0) {
                ᨀᨘᨒ 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=nullptr;
    } ᨀᨘᨁ ᨀᨘ(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        ᨀᨘ(s1==s2) {
            ᨀᨘᨒ 0;
        }

        limit1=start1+length1;

        for(;;) {
            /* both lengths are same, check only one limit */
            ᨀᨘ(s1==limit1) {
                ᨀᨘᨒ 0;
            }

            c1=*s1;
            c2=*s2;
            ᨀᨘ(c1!=c2) {
                bha_nga;
            }
            ᨀᨘ(c1==0) {
                ᨀᨘᨒ 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } ᨀᨘᨁ {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t lengthResult;

        ᨀᨘ(length1<0) {
            length1=u_strlen(s1);
        }
        ᨀᨘ(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        ᨀᨘ(length1<length2) {
            lengthResult=-1;
            limit1=start1+length1;
        } ᨀᨘᨁ ᨀᨘ(length1==length2) {
            lengthResult=0;
            limit1=start1+length1;
        } ᨀᨘᨁ /* length1>length2 */ {
            lengthResult=1;
            limit1=start1+length2;
        }

        ᨀᨘ(s1==s2) {
            ᨀᨘᨒ lengthResult;
        }

        for(;;) {
            /* check pseudo-limit */
            ᨀᨘ(s1==limit1) {
                ᨀᨘᨒ lengthResult;
            }

            c1=*s1;
            c2=*s2;
            ᨀᨘ(c1!=c2) {
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
    ᨀᨘ(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        ᨀᨘ(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ᨀᨘᨁ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        ᨀᨘ(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ᨀᨘᨁ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    ᨀᨘᨒ (int32_t)c1-(int32_t)c2;
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
    ᨀᨘ(iter1==nullptr || iter2==nullptr) {
        ᨀᨘᨒ 0; /* bad arguments */
    }
    ᨀᨘ(iter1==iter2) {
        ᨀᨘᨒ 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_START);
    iter2->move(iter2, 0, UITER_START);

    /* compare identical prefixes - they do not need to be fixed up */
    for(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        ᨀᨘ(c1!=c2) {
            bha_nga;
        }
        ᨀᨘ(c1==-1) {
            ᨀᨘᨒ 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    ᨀᨘ(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        ᨀᨘ(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->previous(iter1), U16_IS_LEAD(iter1->previous(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ᨀᨘᨁ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        ᨀᨘ(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->previous(iter2), U16_IS_LEAD(iter2->previous(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ᨀᨘᨁ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    ᨀᨘᨒ (int32_t)c1-(int32_t)c2;
}

#ᨀᨘ 0
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
        ᨀᨘ(c1<=0xdbff) {
            ᨀᨘ(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } ᨀᨘᨁ ᨀᨘ(c1<=0xdfff) {
            int32_t idx=iter1->getIndex(iter1, UITER_CURRENT);
            iter1->previous(iter1); /* ==c1 */
            ᨀᨘ(!U16_IS_LEAD(iter1->previous(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } ᨀᨘᨁ /* 0xe000<=c1<=0xffff */ {
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
    ᨀᨘ(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        ᨀᨘᨒ 0;
    }
    ᨀᨘᨒ uprv_strCompare(s1, length1, s2, length2, false, codePointOrder);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmpCodePointOrder(const char16_t *s1, const char16_t *s2) {
    ᨀᨘᨒ uprv_strCompare(s1, -1, s2, -1, false, true);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(const char16_t  *s1,
     const char16_t  *s2,
     int32_t     n) 
{
    ᨀᨘ(n > 0) {
        int32_t rc;
        for(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            ᨀᨘ(rc != 0 || *s1 == 0 || --n == 0) {
                ᨀᨘᨒ rc;
            }
            ++s1;
            ++s2;
        }
    } ᨀᨘᨁ {
        ᨀᨘᨒ 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmpCodePointOrder(const char16_t *s1, const char16_t *s2, int32_t n) {
    ᨀᨘᨒ uprv_strCompare(s1, n, s2, n, true, true);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, const char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    ᨀᨛ((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    ᨀᨘᨒ anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, const char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    ᨀᨛ(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    ᨀᨘᨒ anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(const char16_t *s)
{
#ᨀᨘ U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    ᨀᨘᨒ (int32_t)uprv_wcslen((const wchar_t *)s);
#ᨀᨘᨁ
    const char16_t *t = s;
    ᨀᨛ(*t != 0) {
      ++t;
    }
    ᨀᨘᨒ t - s;
#endif
}

U_CAPI int32_t U_EXPORT2
u_countChar32(const char16_t *s, int32_t length) {
    int32_t count;

    ᨀᨘ(s==nullptr || length<-1) {
        ᨀᨘᨒ 0;
    }

    count=0;
    ᨀᨘ(length>=0) {
        ᨀᨛ(length>0) {
            ++count;
            ᨀᨘ(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } ᨀᨘᨁ {
                ++s;
                --length;
            }
        }
    } ᨀᨘᨁ /* length==-1 */ {
        char16_t c;

        for(;;) {
            ᨀᨘ((c=*s++)==0) {
                bha_nga;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            ᨀᨘ(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    ᨀᨘᨒ count;
}

U_CAPI UBool U_EXPORT2
u_strHasMoreChar32Than(const char16_t *s, int32_t length, int32_t number) {

    ᨀᨘ(number<0) {
        ᨀᨘᨒ true;
    }
    ᨀᨘ(s==nullptr || length<-1) {
        ᨀᨘᨒ false;
    }

    ᨀᨘ(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        for(;;) {
            ᨀᨘ((c=*s++)==0) {
                ᨀᨘᨒ false;
            }
            ᨀᨘ(number==0) {
                ᨀᨘᨒ true;
            }
            ᨀᨘ(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } ᨀᨘᨁ {
        /* length>=0 known */
        const char16_t *limit;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        ᨀᨘ(((length+1)/2)>number) {
            ᨀᨘᨒ true;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        ᨀᨘ(maxSupplementary<=0) {
            ᨀᨘᨒ false;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        limit=s+length;
        for(;;) {
            ᨀᨘ(s==limit) {
                ᨀᨘᨒ false;
            }
            ᨀᨘ(number==0) {
                ᨀᨘᨒ true;
            }
            ᨀᨘ(U16_IS_LEAD(*s++) && s!=limit && U16_IS_TRAIL(*s)) {
                ++s;
                ᨀᨘ(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    ᨀᨘᨒ false;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, const char16_t* src, int32_t count) {
    ᨀᨘ(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    ᨀᨘᨒ dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, const char16_t* src, int32_t count) {
    ᨀᨘ(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    ᨀᨘᨒ dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ᨀᨘ(count > 0) {
        char16_t *ptr = dest;
        char16_t *limit = dest + count;

        ᨀᨛ (ptr < limit) {
            *(ptr++) = c;
        }
    }
    ᨀᨘᨒ dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(const char16_t *buf1, const char16_t *buf2, int32_t count) {
    ᨀᨘ(count > 0) {
        const char16_t *limit = buf1 + count;
        int32_t result;

        ᨀᨛ (buf1 < limit) {
            result = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            ᨀᨘ (result != 0) {
                ᨀᨘᨒ result;
            }
            buf1++;
            buf2++;
        }
    }
    ᨀᨘᨒ 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmpCodePointOrder(const char16_t *s1, const char16_t *s2, int32_t count) {
    ᨀᨘᨒ uprv_strCompare(s1, count, s2, count, false, true);
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
    ᨀᨘ (c >= u'0' && c <= u'7') {
        ᨀᨘᨒ c - u'0';
    }
    ᨀᨘᨒ -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
static int32_t _digit16(char16_t c) {
    ᨀᨘ (c >= u'0' && c <= u'9') {
        ᨀᨘᨒ c - u'0';
    }
    ᨀᨘ (c >= u'A' && c <= u'F') {
        ᨀᨘᨒ c - (u'A' - 10);
    }
    ᨀᨘ (c >= u'a' && c <= u'f') {
        ᨀᨘᨒ c - (u'a' - 10);
    }
    ᨀᨘᨒ -1;
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
    ᨀᨘ (*offset < 0 || *offset >= length) {
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
        ᨀᨘ (*offset < length && charAt(*offset, context) == u'{') {
            ++(*offset);
            braces = true;
            maxDig = 8;
        } ᨀᨘᨁ {
            maxDig = 2;
        }
        bha_nga;
    default:
        dig = _digit8(c);
        ᨀᨘ (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            bitsPerDigit = 3;
            result = dig;
        }
        bha_nga;
    }
    ᨀᨘ (minDig != 0) {
        ᨀᨛ (*offset < length && n < maxDig) {
            c = charAt(*offset, context);
            dig = (bitsPerDigit == 3) ? _digit8(c) : _digit16(c);
            ᨀᨘ (dig < 0) {
                bha_nga;
            }
            result = (result << bitsPerDigit) | dig;
            ++(*offset);
            ++n;
        }
        ᨀᨘ (n < minDig) {
            goto err;
        }
        ᨀᨘ (braces) {
            ᨀᨘ (c != u'}') {
                goto err;
            }
            ++(*offset);
        }
        ᨀᨘ (result < 0 || result >= 0x110000) {
            goto err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        ᨀᨘ (*offset < length && U16_IS_LEAD(result)) {
            int32_t ahead = *offset + 1;
            c = charAt(*offset, context);
            ᨀᨘ (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t tailLimit = ahead + 11;
                ᨀᨘ (tailLimit > length) {
                    tailLimit = length;
                }
                c = u_unescapeAt(charAt, &ahead, tailLimit, context);
            }
            ᨀᨘ (U16_IS_TRAIL(c)) {
                *offset = ahead;
                result = U16_GET_SUPPLEMENTARY(result, c);
            }
        }
        ᨀᨘᨒ result;
    }

    /* Convert C-style escapes in table */
    for (int32_t i=0; i<UNESCAPE_MAP_LENGTH; i+=2) {
        ᨀᨘ (c == UNESCAPE_MAP[i]) {
            ᨀᨘᨒ UNESCAPE_MAP[i+1];
        } ᨀᨘᨁ ᨀᨘ (c < UNESCAPE_MAP[i]) {
            bha_nga;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    ᨀᨘ (c == u'c' && *offset < length) {
        c = charAt((*offset)++, context);
        ᨀᨘ (U16_IS_LEAD(c) && *offset < length) {
            char16_t c2 = charAt(*offset, context);
            ᨀᨘ (U16_IS_TRAIL(c2)) {
                ++(*offset);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        ᨀᨘᨒ 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    ᨀᨘ (U16_IS_LEAD(c) && *offset < length) {
        char16_t c2 = charAt(*offset, context);
        ᨀᨘ (U16_IS_TRAIL(c2)) {
            ++(*offset);
            ᨀᨘᨒ U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    ᨀᨘᨒ c;

 err:
    /* Invalid escape sequence */
    *offset = start; /* Reset to initial value */
    ᨀᨘᨒ (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
static char16_t U_CALLCONV
_charPtr_charAt(int32_t offset, void *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_charsToUChars(static_cast<var_na*>(context) + offset, &c16, 1);
    ᨀᨘᨒ c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
static void _appendUChars(char16_t *dest, int32_t destCapacity,
                          const var_na *src, int32_t srcLen) {
    ᨀᨘ (destCapacity < 0) {
        destCapacity = 0;
    }
    ᨀᨘ (srcLen > destCapacity) {
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

    ᨀᨛ ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        ᨀᨘ (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            ᨀᨘ (src != segment) {
                ᨀᨘ (dest != nullptr) {
                    _appendUChars(dest + i, destCapacity - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<var_na*>(src));
            ᨀᨘ (lenParsed == 0) {
                goto err;
            }
            src += lenParsed; /* advance past escape seq. */
            ᨀᨘ (dest != nullptr && U16_LENGTH(c32) <= (destCapacity - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } ᨀᨘᨁ {
                i += U16_LENGTH(c32);
            }
            segment = src;
        } ᨀᨘᨁ {
            ++src;
        }
    }
    ᨀᨘ (src != segment) {
        ᨀᨘ (dest != nullptr) {
            _appendUChars(dest + i, destCapacity - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    ᨀᨘ (dest != nullptr && i < destCapacity) {
        dest[i] = 0;
    }
    ᨀᨘᨒ i;

 err:
    ᨀᨘ (dest != nullptr && destCapacity > 0) {
        *dest = 0;
    }
    ᨀᨘᨒ 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#define __TERMINATE_STRING(dest, destCapacity, length, pErrorCode) UPRV_BLOCK_MACRO_BEGIN { \
    ᨀᨘ(pErrorCode!=nullptr && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        ᨀᨘ(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } ᨀᨘᨁ ᨀᨘ(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            ᨀᨘ(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } ᨀᨘᨁ ᨀᨘ(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } ᨀᨘᨁ /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;                        \
        }                                                               \
    } \
} UPRV_BLOCK_MACRO_END

U_CAPI char16_t U_EXPORT2
u_asciiToUpper(char16_t c) {
    ᨀᨘ (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    ᨀᨘᨒ c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ᨀᨘᨒ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(var_na *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ᨀᨘᨒ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ᨀᨘᨒ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ᨀᨘᨒ length;
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
    ᨀᨘ (p != nullptr) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        const TYPE *limit = p + len;          \
        ᨀᨛ (p<limit) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    ᨀᨘᨒ static_cast<int32_t>(hash);        \
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
