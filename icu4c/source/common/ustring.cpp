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
    ᮚᮦᮔ᮪(U16_IS_TRAIL(*match) && start!=match && U16_IS_LEAD(*(match-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        ᮝᮜᮤ false;
    }
    ᮚᮦᮔ᮪(U16_IS_LEAD(*(matchLimit-1)) && matchLimit!=limit && U16_IS_TRAIL(*matchLimit)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        ᮝᮜᮤ false;
    }
    ᮝᮜᮤ true;
}

U_CAPI char16_t* U_EXPORT2
u_strFindFirst(const char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               const char16_t* sub,
               int32_t subLength) {
    const char16_t *start, *p, *q, *subLimit;
    char16_t c, cs, cq;

    ᮚᮦᮔ᮪(sub==nullptr || subLength<-1) {
        ᮝᮜᮤ (char16_t *)s;
    }
    ᮚᮦᮔ᮪(s==nullptr || length<-1) {
        ᮝᮜᮤ nullptr;
    }

    start=s;

    ᮚᮦᮔ᮪(length<0 && subLength<0) {
        /* both strings are NUL-terminated */
        ᮚᮦᮔ᮪((cs=*sub++)==0) {
            ᮝᮜᮤ (char16_t *)s;
        }
        ᮚᮦᮔ᮪(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            ᮝᮜᮤ u_strchr(s, cs);
        }

        ᮞᮨᮓᮨᮜ((c=*s++)!=0) {
            ᮚᮦᮔ᮪(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                for(;;) {
                    ᮚᮦᮔ᮪((cq=*q)==0) {
                        ᮚᮦᮔ᮪(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            ᮝᮜᮤ (char16_t *)(s-1); /* well-formed match */
                        } ᮜᮤᮚ {
                            bha_nga; /* no match because surrogate pair is split */
                        }
                    }
                    ᮚᮦᮔ᮪((c=*p)==0) {
                        ᮝᮜᮤ nullptr; /* no match, and none possible after s */
                    }
                    ᮚᮦᮔ᮪(c!=cq) {
                        bha_nga; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        ᮝᮜᮤ nullptr;
    }

    ᮚᮦᮔ᮪(subLength<0) {
        subLength=u_strlen(sub);
    }
    ᮚᮦᮔ᮪(subLength==0) {
        ᮝᮜᮤ (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --subLength;
    subLimit=sub+subLength;

    ᮚᮦᮔ᮪(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        ᮝᮜᮤ length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    ᮚᮦᮔ᮪(length<0) {
        /* s is NUL-terminated */
        ᮞᮨᮓᮨᮜ((c=*s++)!=0) {
            ᮚᮦᮔ᮪(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                for(;;) {
                    ᮚᮦᮔ᮪(q==subLimit) {
                        ᮚᮦᮔ᮪(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            ᮝᮜᮤ (char16_t *)(s-1); /* well-formed match */
                        } ᮜᮤᮚ {
                            bha_nga; /* no match because surrogate pair is split */
                        }
                    }
                    ᮚᮦᮔ᮪((c=*p)==0) {
                        ᮝᮜᮤ nullptr; /* no match, and none possible after s */
                    }
                    ᮚᮦᮔ᮪(c!=*q) {
                        bha_nga; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } ᮜᮤᮚ {
        const char16_t *limit, *preLimit;

        /* subLength was decremented above */
        ᮚᮦᮔ᮪(length<=subLength) {
            ᮝᮜᮤ nullptr; /* s is shorter than sub */
        }

        limit=s+length;

        /* the substring must start before preLimit */
        preLimit=limit-subLength;

        ᮞᮨᮓᮨᮜ(s!=preLimit) {
            c=*s++;
            ᮚᮦᮔ᮪(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                for(;;) {
                    ᮚᮦᮔ᮪(q==subLimit) {
                        ᮚᮦᮔ᮪(isMatchAtCPBoundary(start, s-1, p, limit)) {
                            ᮝᮜᮤ (char16_t *)(s-1); /* well-formed match */
                        } ᮜᮤᮚ {
                            bha_nga; /* no match because surrogate pair is split */
                        }
                    }
                    ᮚᮦᮔ᮪(*p!=*q) {
                        bha_nga; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    ᮝᮜᮤ nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(const char16_t* s U_LIFETIME_BOUND, const char16_t* substring) {
    ᮝᮜᮤ u_strFindFirst(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(const char16_t* s U_LIFETIME_BOUND, char16_t c) {
    ᮚᮦᮔ᮪(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ᮝᮜᮤ u_strFindFirst(s, -1, &c, 1);
    } ᮜᮤᮚ {
        char16_t cs;

        /* trivial search for a BMP code point */
        for(;;) {
            ᮚᮦᮔ᮪((cs=*s)==c) {
                ᮝᮜᮤ (char16_t *)s;
            }
            ᮚᮦᮔ᮪(cs==0) {
                ᮝᮜᮤ nullptr;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    ᮚᮦᮔ᮪((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ᮝᮜᮤ u_strchr(s, (char16_t)c);
    } ᮜᮤᮚ ᮚᮦᮔ᮪((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ᮞᮨᮓᮨᮜ((cs=*s++)!=0) {
            ᮚᮦᮔ᮪(cs==lead && *s==trail) {
                ᮝᮜᮤ (char16_t *)(s-1);
            }
        }
        ᮝᮜᮤ nullptr;
    } ᮜᮤᮚ {
        /* not a Unicode code point, not findable */
        ᮝᮜᮤ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(const char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ᮚᮦᮔ᮪(count<=0) {
        ᮝᮜᮤ nullptr; /* no string */
    } ᮜᮤᮚ ᮚᮦᮔ᮪(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ᮝᮜᮤ u_strFindFirst(s, count, &c, 1);
    } ᮜᮤᮚ {
        /* trivial search for a BMP code point */
        const char16_t *limit=s+count;
        do {
            ᮚᮦᮔ᮪(*s==c) {
                ᮝᮜᮤ (char16_t *)s;
            }
        } ᮞᮨᮓᮨᮜ(++s!=limit);
        ᮝᮜᮤ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    ᮚᮦᮔ᮪((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ᮝᮜᮤ u_memchr(s, (char16_t)c, count);
    } ᮜᮤᮚ ᮚᮦᮔ᮪(count<2) {
        /* too short for a surrogate pair */
        ᮝᮜᮤ nullptr;
    } ᮜᮤᮚ ᮚᮦᮔ᮪((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        const char16_t *limit=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        do {
            ᮚᮦᮔ᮪(*s==lead && *(s+1)==trail) {
                ᮝᮜᮤ (char16_t *)s;
            }
        } ᮞᮨᮓᮨᮜ(++s!=limit);
        ᮝᮜᮤ nullptr;
    } ᮜᮤᮚ {
        /* not a Unicode code point, not findable */
        ᮝᮜᮤ nullptr;
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

    ᮚᮦᮔ᮪(sub==nullptr || subLength<-1) {
        ᮝᮜᮤ (char16_t *)s;
    }
    ᮚᮦᮔ᮪(s==nullptr || length<-1) {
        ᮝᮜᮤ nullptr;
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

    ᮚᮦᮔ᮪(subLength<0) {
        subLength=u_strlen(sub);
    }
    ᮚᮦᮔ᮪(subLength==0) {
        ᮝᮜᮤ (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    subLimit=sub+subLength;
    cs=*(--subLimit);
    --subLength;

    ᮚᮦᮔ᮪(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        ᮝᮜᮤ length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    ᮚᮦᮔ᮪(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    ᮚᮦᮔ᮪(length<=subLength) {
        ᮝᮜᮤ nullptr; /* s is shorter than sub */
    }

    start=s;
    limit=s+length;

    /* the substring must start no later than s+subLength */
    s+=subLength;

    ᮞᮨᮓᮨᮜ(s!=limit) {
        c=*(--limit);
        ᮚᮦᮔ᮪(c==cs) {
            /* found last substring char16_t, compare rest */
            p=limit;
            q=subLimit;
            for(;;) {
                ᮚᮦᮔ᮪(q==sub) {
                    ᮚᮦᮔ᮪(isMatchAtCPBoundary(start, p, limit+1, start+length)) {
                        ᮝᮜᮤ (char16_t *)p; /* well-formed match */
                    } ᮜᮤᮚ {
                        bha_nga; /* no match because surrogate pair is split */
                    }
                }
                ᮚᮦᮔ᮪(*(--p)!=*(--q)) {
                    bha_nga; /* no match */
                }
            }
        }
    }

    /* not found */
    ᮝᮜᮤ nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(const char16_t* s U_LIFETIME_BOUND, const char16_t* substring) {
    ᮝᮜᮤ u_strFindLast(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(const char16_t* s U_LIFETIME_BOUND, char16_t c) {
    ᮚᮦᮔ᮪(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ᮝᮜᮤ u_strFindLast(s, -1, &c, 1);
    } ᮜᮤᮚ {
        const char16_t *result=nullptr;
        char16_t cs;

        /* trivial search for a BMP code point */
        for(;;) {
            ᮚᮦᮔ᮪((cs=*s)==c) {
                result=s;
            }
            ᮚᮦᮔ᮪(cs==0) {
                ᮝᮜᮤ (char16_t *)result;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    ᮚᮦᮔ᮪((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ᮝᮜᮤ u_strrchr(s, (char16_t)c);
    } ᮜᮤᮚ ᮚᮦᮔ᮪((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        const char16_t *result=nullptr;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ᮞᮨᮓᮨᮜ((cs=*s++)!=0) {
            ᮚᮦᮔ᮪(cs==lead && *s==trail) {
                result=s-1;
            }
        }
        ᮝᮜᮤ (char16_t *)result;
    } ᮜᮤᮚ {
        /* not a Unicode code point, not findable */
        ᮝᮜᮤ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(const char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ᮚᮦᮔ᮪(count<=0) {
        ᮝᮜᮤ nullptr; /* no string */
    } ᮜᮤᮚ ᮚᮦᮔ᮪(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ᮝᮜᮤ u_strFindLast(s, count, &c, 1);
    } ᮜᮤᮚ {
        /* trivial search for a BMP code point */
        const char16_t *limit=s+count;
        do {
            ᮚᮦᮔ᮪(*(--limit)==c) {
                ᮝᮜᮤ (char16_t *)limit;
            }
        } ᮞᮨᮓᮨᮜ(s!=limit);
        ᮝᮜᮤ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(const char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    ᮚᮦᮔ᮪((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ᮝᮜᮤ u_memrchr(s, (char16_t)c, count);
    } ᮜᮤᮚ ᮚᮦᮔ᮪(count<2) {
        /* too short for a surrogate pair */
        ᮝᮜᮤ nullptr;
    } ᮜᮤᮚ ᮚᮦᮔ᮪((uint32_t)c<=UCHAR_MAX_VALUE) {
        /* find supplementary code point as surrogate pair */
        const char16_t *limit=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        do {
            ᮚᮦᮔ᮪(*limit==trail && *(limit-1)==lead) {
                ᮝᮜᮤ (char16_t *)(limit-1);
            }
        } ᮞᮨᮓᮨᮜ(s!=--limit);
        ᮝᮜᮤ nullptr;
    } ᮜᮤᮚ {
        /* not a Unicode code point, not findable */
        ᮝᮜᮤ nullptr;
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
    ᮞᮨᮓᮨᮜ((c = matchSet[matchBMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++matchBMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    matchLen = matchBMPLen;
    ᮞᮨᮓᮨᮜ(matchSet[matchLen] != 0) {
        ++matchLen;
    }

    for(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        ᮚᮦᮔ᮪(U16_IS_SINGLE(c)) {
            ᮚᮦᮔ᮪(polarity) {
                for(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    ᮚᮦᮔ᮪(c == matchSet[matchItr]) {
                        ᮝᮜᮤ strItr - 1; /* one matches */
                    }
                }
            } ᮜᮤᮚ {
                for(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    ᮚᮦᮔ᮪(c == matchSet[matchItr]) {
                        goto endloop;
                    }
                }
                ᮝᮜᮤ strItr - 1; /* none matches */
            }
        } ᮜᮤᮚ {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            ᮚᮦᮔ᮪(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } ᮜᮤᮚ {
                stringCh = c; /* unpaired trail surrogate */
            }

            ᮚᮦᮔ᮪(polarity) {
                for(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    ᮚᮦᮔ᮪(stringCh == matchCh) {
                        ᮝᮜᮤ strItr - U16_LENGTH(stringCh); /* one matches */
                    }
                }
            } ᮜᮤᮚ {
                for(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    ᮚᮦᮔ᮪(stringCh == matchCh) {
                        goto endloop;
                    }
                }
                ᮝᮜᮤ strItr - U16_LENGTH(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    ᮝᮜᮤ -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(const char16_t* string U_LIFETIME_BOUND, const char16_t* matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, true);
    ᮚᮦᮔ᮪(idx >= 0) {
        ᮝᮜᮤ (char16_t *)string + idx;
    } ᮜᮤᮚ {
        ᮝᮜᮤ nullptr;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(const char16_t *string, const char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, true);
    ᮚᮦᮔ᮪(idx >= 0) {
        ᮝᮜᮤ idx;
    } ᮜᮤᮚ {
        ᮝᮜᮤ -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(const char16_t *string, const char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, false);
    ᮚᮦᮔ᮪(idx >= 0) {
        ᮝᮜᮤ idx;
    } ᮜᮤᮚ {
        ᮝᮜᮤ -idx - 1; /* == u_strlen(string) */
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
    ᮚᮦᮔ᮪ (src != nullptr) {
        tokSource = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (*saveState) {
        tokSource = *saveState;
    }
    ᮜᮤᮚ {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        ᮝᮜᮤ nullptr;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tokSource, delim);
    tokSource = &tokSource[nonDelimIdx];

    ᮚᮦᮔ᮪ (*tokSource) {
        nextToken = u_strpbrk(tokSource, delim);
        ᮚᮦᮔ᮪ (nextToken != nullptr) {
            /* Create a token */
            *(nextToken++) = 0;
            *saveState = nextToken;
            ᮝᮜᮤ tokSource;
        }
        ᮜᮤᮚ ᮚᮦᮔ᮪ (*saveState) {
            /* Return the last token */
            *saveState = nullptr;
            ᮝᮜᮤ tokSource;
        }
    }
    ᮜᮤᮚ {
        /* No tokens were found. Only delimiters were left. */
        *saveState = nullptr;
    }
    ᮝᮜᮤ nullptr;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, const char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    ᮞᮨᮓᮨᮜ(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    ᮞᮨᮓᮨᮜ((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    ᮝᮜᮤ anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, const char16_t* src, int32_t n)
{
    ᮚᮦᮔ᮪(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        ᮞᮨᮓᮨᮜ(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        ᮞᮨᮓᮨᮜ((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            ᮚᮦᮔ᮪(--n == 0) {
                *dst = 0;
                bha_nga;
            }
            ++src;
        }

        ᮝᮜᮤ anchor;
    } ᮜᮤᮚ {
        ᮝᮜᮤ dst;
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
        ᮚᮦᮔ᮪ (c1 != c2 || c1 == 0) {
            bha_nga;
        }
    }
    ᮝᮜᮤ (int32_t)c1 - (int32_t)c2;
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
    ᮚᮦᮔ᮪(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        ᮚᮦᮔ᮪(s1==s2) {
            ᮝᮜᮤ 0;
        }

        for(;;) {
            c1=*s1;
            c2=*s2;
            ᮚᮦᮔ᮪(c1!=c2) {
                bha_nga;
            }
            ᮚᮦᮔ᮪(c1==0) {
                ᮝᮜᮤ 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=nullptr;
    } ᮜᮤᮚ ᮚᮦᮔ᮪(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        ᮚᮦᮔ᮪(s1==s2) {
            ᮝᮜᮤ 0;
        }

        limit1=start1+length1;

        for(;;) {
            /* both lengths are same, check only one limit */
            ᮚᮦᮔ᮪(s1==limit1) {
                ᮝᮜᮤ 0;
            }

            c1=*s1;
            c2=*s2;
            ᮚᮦᮔ᮪(c1!=c2) {
                bha_nga;
            }
            ᮚᮦᮔ᮪(c1==0) {
                ᮝᮜᮤ 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } ᮜᮤᮚ {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t lengthResult;

        ᮚᮦᮔ᮪(length1<0) {
            length1=u_strlen(s1);
        }
        ᮚᮦᮔ᮪(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        ᮚᮦᮔ᮪(length1<length2) {
            lengthResult=-1;
            limit1=start1+length1;
        } ᮜᮤᮚ ᮚᮦᮔ᮪(length1==length2) {
            lengthResult=0;
            limit1=start1+length1;
        } ᮜᮤᮚ /* length1>length2 */ {
            lengthResult=1;
            limit1=start1+length2;
        }

        ᮚᮦᮔ᮪(s1==s2) {
            ᮝᮜᮤ lengthResult;
        }

        for(;;) {
            /* check pseudo-limit */
            ᮚᮦᮔ᮪(s1==limit1) {
                ᮝᮜᮤ lengthResult;
            }

            c1=*s1;
            c2=*s2;
            ᮚᮦᮔ᮪(c1!=c2) {
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
    ᮚᮦᮔ᮪(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        ᮚᮦᮔ᮪(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ᮜᮤᮚ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        ᮚᮦᮔ᮪(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ᮜᮤᮚ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    ᮝᮜᮤ (int32_t)c1-(int32_t)c2;
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
    ᮚᮦᮔ᮪(iter1==nullptr || iter2==nullptr) {
        ᮝᮜᮤ 0; /* bad arguments */
    }
    ᮚᮦᮔ᮪(iter1==iter2) {
        ᮝᮜᮤ 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_START);
    iter2->move(iter2, 0, UITER_START);

    /* compare identical prefixes - they do not need to be fixed up */
    for(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        ᮚᮦᮔ᮪(c1!=c2) {
            bha_nga;
        }
        ᮚᮦᮔ᮪(c1==-1) {
            ᮝᮜᮤ 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    ᮚᮦᮔ᮪(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        ᮚᮦᮔ᮪(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->previous(iter1), U16_IS_LEAD(iter1->previous(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ᮜᮤᮚ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        ᮚᮦᮔ᮪(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->previous(iter2), U16_IS_LEAD(iter2->previous(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ᮜᮤᮚ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    ᮝᮜᮤ (int32_t)c1-(int32_t)c2;
}

#ᮚᮦᮔ᮪ 0
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
        ᮚᮦᮔ᮪(c1<=0xdbff) {
            ᮚᮦᮔ᮪(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } ᮜᮤᮚ ᮚᮦᮔ᮪(c1<=0xdfff) {
            int32_t idx=iter1->getIndex(iter1, UITER_CURRENT);
            iter1->previous(iter1); /* ==c1 */
            ᮚᮦᮔ᮪(!U16_IS_LEAD(iter1->previous(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } ᮜᮤᮚ /* 0xe000<=c1<=0xffff */ {
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
    ᮚᮦᮔ᮪(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        ᮝᮜᮤ 0;
    }
    ᮝᮜᮤ uprv_strCompare(s1, length1, s2, length2, false, codePointOrder);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmpCodePointOrder(const char16_t *s1, const char16_t *s2) {
    ᮝᮜᮤ uprv_strCompare(s1, -1, s2, -1, false, true);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(const char16_t  *s1,
     const char16_t  *s2,
     int32_t     n) 
{
    ᮚᮦᮔ᮪(n > 0) {
        int32_t rc;
        for(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            ᮚᮦᮔ᮪(rc != 0 || *s1 == 0 || --n == 0) {
                ᮝᮜᮤ rc;
            }
            ++s1;
            ++s2;
        }
    } ᮜᮤᮚ {
        ᮝᮜᮤ 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmpCodePointOrder(const char16_t *s1, const char16_t *s2, int32_t n) {
    ᮝᮜᮤ uprv_strCompare(s1, n, s2, n, true, true);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, const char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    ᮞᮨᮓᮨᮜ((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    ᮝᮜᮤ anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, const char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    ᮞᮨᮓᮨᮜ(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    ᮝᮜᮤ anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(const char16_t *s)
{
#ᮚᮦᮔ᮪ U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    ᮝᮜᮤ (int32_t)uprv_wcslen((const wchar_t *)s);
#ᮜᮤᮚ
    const char16_t *t = s;
    ᮞᮨᮓᮨᮜ(*t != 0) {
      ++t;
    }
    ᮝᮜᮤ t - s;
#endif
}

U_CAPI int32_t U_EXPORT2
u_countChar32(const char16_t *s, int32_t length) {
    int32_t count;

    ᮚᮦᮔ᮪(s==nullptr || length<-1) {
        ᮝᮜᮤ 0;
    }

    count=0;
    ᮚᮦᮔ᮪(length>=0) {
        ᮞᮨᮓᮨᮜ(length>0) {
            ++count;
            ᮚᮦᮔ᮪(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } ᮜᮤᮚ {
                ++s;
                --length;
            }
        }
    } ᮜᮤᮚ /* length==-1 */ {
        char16_t c;

        for(;;) {
            ᮚᮦᮔ᮪((c=*s++)==0) {
                bha_nga;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            ᮚᮦᮔ᮪(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    ᮝᮜᮤ count;
}

U_CAPI UBool U_EXPORT2
u_strHasMoreChar32Than(const char16_t *s, int32_t length, int32_t number) {

    ᮚᮦᮔ᮪(number<0) {
        ᮝᮜᮤ true;
    }
    ᮚᮦᮔ᮪(s==nullptr || length<-1) {
        ᮝᮜᮤ false;
    }

    ᮚᮦᮔ᮪(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        for(;;) {
            ᮚᮦᮔ᮪((c=*s++)==0) {
                ᮝᮜᮤ false;
            }
            ᮚᮦᮔ᮪(number==0) {
                ᮝᮜᮤ true;
            }
            ᮚᮦᮔ᮪(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } ᮜᮤᮚ {
        /* length>=0 known */
        const char16_t *limit;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        ᮚᮦᮔ᮪(((length+1)/2)>number) {
            ᮝᮜᮤ true;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        ᮚᮦᮔ᮪(maxSupplementary<=0) {
            ᮝᮜᮤ false;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        limit=s+length;
        for(;;) {
            ᮚᮦᮔ᮪(s==limit) {
                ᮝᮜᮤ false;
            }
            ᮚᮦᮔ᮪(number==0) {
                ᮝᮜᮤ true;
            }
            ᮚᮦᮔ᮪(U16_IS_LEAD(*s++) && s!=limit && U16_IS_TRAIL(*s)) {
                ++s;
                ᮚᮦᮔ᮪(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    ᮝᮜᮤ false;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, const char16_t* src, int32_t count) {
    ᮚᮦᮔ᮪(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    ᮝᮜᮤ dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, const char16_t* src, int32_t count) {
    ᮚᮦᮔ᮪(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    ᮝᮜᮤ dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ᮚᮦᮔ᮪(count > 0) {
        char16_t *ptr = dest;
        char16_t *limit = dest + count;

        ᮞᮨᮓᮨᮜ (ptr < limit) {
            *(ptr++) = c;
        }
    }
    ᮝᮜᮤ dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(const char16_t *buf1, const char16_t *buf2, int32_t count) {
    ᮚᮦᮔ᮪(count > 0) {
        const char16_t *limit = buf1 + count;
        int32_t result;

        ᮞᮨᮓᮨᮜ (buf1 < limit) {
            result = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            ᮚᮦᮔ᮪ (result != 0) {
                ᮝᮜᮤ result;
            }
            buf1++;
            buf2++;
        }
    }
    ᮝᮜᮤ 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmpCodePointOrder(const char16_t *s1, const char16_t *s2, int32_t count) {
    ᮝᮜᮤ uprv_strCompare(s1, count, s2, count, false, true);
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
    ᮚᮦᮔ᮪ (c >= u'0' && c <= u'7') {
        ᮝᮜᮤ c - u'0';
    }
    ᮝᮜᮤ -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
static int32_t _digit16(char16_t c) {
    ᮚᮦᮔ᮪ (c >= u'0' && c <= u'9') {
        ᮝᮜᮤ c - u'0';
    }
    ᮚᮦᮔ᮪ (c >= u'A' && c <= u'F') {
        ᮝᮜᮤ c - (u'A' - 10);
    }
    ᮚᮦᮔ᮪ (c >= u'a' && c <= u'f') {
        ᮝᮜᮤ c - (u'a' - 10);
    }
    ᮝᮜᮤ -1;
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
    ᮚᮦᮔ᮪ (*offset < 0 || *offset >= length) {
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
        ᮚᮦᮔ᮪ (*offset < length && charAt(*offset, context) == u'{') {
            ++(*offset);
            braces = true;
            maxDig = 8;
        } ᮜᮤᮚ {
            maxDig = 2;
        }
        bha_nga;
    default:
        dig = _digit8(c);
        ᮚᮦᮔ᮪ (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            bitsPerDigit = 3;
            result = dig;
        }
        bha_nga;
    }
    ᮚᮦᮔ᮪ (minDig != 0) {
        ᮞᮨᮓᮨᮜ (*offset < length && n < maxDig) {
            c = charAt(*offset, context);
            dig = (bitsPerDigit == 3) ? _digit8(c) : _digit16(c);
            ᮚᮦᮔ᮪ (dig < 0) {
                bha_nga;
            }
            result = (result << bitsPerDigit) | dig;
            ++(*offset);
            ++n;
        }
        ᮚᮦᮔ᮪ (n < minDig) {
            goto err;
        }
        ᮚᮦᮔ᮪ (braces) {
            ᮚᮦᮔ᮪ (c != u'}') {
                goto err;
            }
            ++(*offset);
        }
        ᮚᮦᮔ᮪ (result < 0 || result >= 0x110000) {
            goto err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        ᮚᮦᮔ᮪ (*offset < length && U16_IS_LEAD(result)) {
            int32_t ahead = *offset + 1;
            c = charAt(*offset, context);
            ᮚᮦᮔ᮪ (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t tailLimit = ahead + 11;
                ᮚᮦᮔ᮪ (tailLimit > length) {
                    tailLimit = length;
                }
                c = u_unescapeAt(charAt, &ahead, tailLimit, context);
            }
            ᮚᮦᮔ᮪ (U16_IS_TRAIL(c)) {
                *offset = ahead;
                result = U16_GET_SUPPLEMENTARY(result, c);
            }
        }
        ᮝᮜᮤ result;
    }

    /* Convert C-style escapes in table */
    for (int32_t i=0; i<UNESCAPE_MAP_LENGTH; i+=2) {
        ᮚᮦᮔ᮪ (c == UNESCAPE_MAP[i]) {
            ᮝᮜᮤ UNESCAPE_MAP[i+1];
        } ᮜᮤᮚ ᮚᮦᮔ᮪ (c < UNESCAPE_MAP[i]) {
            bha_nga;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    ᮚᮦᮔ᮪ (c == u'c' && *offset < length) {
        c = charAt((*offset)++, context);
        ᮚᮦᮔ᮪ (U16_IS_LEAD(c) && *offset < length) {
            char16_t c2 = charAt(*offset, context);
            ᮚᮦᮔ᮪ (U16_IS_TRAIL(c2)) {
                ++(*offset);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        ᮝᮜᮤ 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    ᮚᮦᮔ᮪ (U16_IS_LEAD(c) && *offset < length) {
        char16_t c2 = charAt(*offset, context);
        ᮚᮦᮔ᮪ (U16_IS_TRAIL(c2)) {
            ++(*offset);
            ᮝᮜᮤ U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    ᮝᮜᮤ c;

 err:
    /* Invalid escape sequence */
    *offset = start; /* Reset to initial value */
    ᮝᮜᮤ (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
static char16_t U_CALLCONV
_charPtr_charAt(int32_t offset, void *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_charsToUChars(static_cast<var_na*>(context) + offset, &c16, 1);
    ᮝᮜᮤ c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
static void _appendUChars(char16_t *dest, int32_t destCapacity,
                          const var_na *src, int32_t srcLen) {
    ᮚᮦᮔ᮪ (destCapacity < 0) {
        destCapacity = 0;
    }
    ᮚᮦᮔ᮪ (srcLen > destCapacity) {
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

    ᮞᮨᮓᮨᮜ ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        ᮚᮦᮔ᮪ (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            ᮚᮦᮔ᮪ (src != segment) {
                ᮚᮦᮔ᮪ (dest != nullptr) {
                    _appendUChars(dest + i, destCapacity - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<var_na*>(src));
            ᮚᮦᮔ᮪ (lenParsed == 0) {
                goto err;
            }
            src += lenParsed; /* advance past escape seq. */
            ᮚᮦᮔ᮪ (dest != nullptr && U16_LENGTH(c32) <= (destCapacity - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } ᮜᮤᮚ {
                i += U16_LENGTH(c32);
            }
            segment = src;
        } ᮜᮤᮚ {
            ++src;
        }
    }
    ᮚᮦᮔ᮪ (src != segment) {
        ᮚᮦᮔ᮪ (dest != nullptr) {
            _appendUChars(dest + i, destCapacity - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    ᮚᮦᮔ᮪ (dest != nullptr && i < destCapacity) {
        dest[i] = 0;
    }
    ᮝᮜᮤ i;

 err:
    ᮚᮦᮔ᮪ (dest != nullptr && destCapacity > 0) {
        *dest = 0;
    }
    ᮝᮜᮤ 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#define __TERMINATE_STRING(dest, destCapacity, length, pErrorCode) UPRV_BLOCK_MACRO_BEGIN { \
    ᮚᮦᮔ᮪(pErrorCode!=nullptr && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        ᮚᮦᮔ᮪(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } ᮜᮤᮚ ᮚᮦᮔ᮪(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            ᮚᮦᮔ᮪(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } ᮜᮤᮚ ᮚᮦᮔ᮪(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } ᮜᮤᮚ /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;                        \
        }                                                               \
    } \
} UPRV_BLOCK_MACRO_END

U_CAPI char16_t U_EXPORT2
u_asciiToUpper(char16_t c) {
    ᮚᮦᮔ᮪ (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    ᮝᮜᮤ c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ᮝᮜᮤ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(var_na *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ᮝᮜᮤ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ᮝᮜᮤ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ᮝᮜᮤ length;
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
    ᮚᮦᮔ᮪ (p != nullptr) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        const TYPE *limit = p + len;          \
        ᮞᮨᮓᮨᮜ (p<limit) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    ᮝᮜᮤ static_cast<int32_t>(hash);        \
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
