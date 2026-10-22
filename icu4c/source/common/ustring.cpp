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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/putil.h"
#અન્તર્ભાવ "unicode/uchar.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "cwchar.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "ustr_imp.h"

/* ANSI string.h - style functions ------------------------------------------ */

/* U+ffff is the highest BMP code point, the highest one that fits into a 16-bit char16_t */
#define U_BMP_MAX 0xffff

/* Forward binary string search functions ----------------------------------- */

/*
 * Test if a substring match inside a string is at code point boundaries.
 * All pointers refer to the same buffer.
 * The limit pointer may be nullptr, all others must be real pointers.
 */
સ્થિર અંતઃરેખ UBool
isMatchAtCPBoundary(અચલ char16_t *start, અચલ char16_t *match, અચલ char16_t *matchLimit, અચલ char16_t *limit) {
    જો(U16_IS_TRAIL(*match) && start!=match && U16_IS_LEAD(*(match-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        પાછા ખોટું;
    }
    જો(U16_IS_LEAD(*(matchLimit-1)) && matchLimit!=limit && U16_IS_TRAIL(*matchLimit)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        પાછા ખોટું;
    }
    પાછા સાચું;
}

U_CAPI char16_t* U_EXPORT2
u_strFindFirst(અચલ char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               અચલ char16_t* sub,
               int32_t subLength) {
    અચલ char16_t *start, *p, *q, *subLimit;
    char16_t c, cs, cq;

    જો(sub==nullptr || subLength<-1) {
        પાછા (char16_t *)s;
    }
    જો(s==nullptr || length<-1) {
        પાછા nullptr;
    }

    start=s;

    જો(length<0 && subLength<0) {
        /* both strings are NUL-terminated */
        જો((cs=*sub++)==0) {
            પાછા (char16_t *)s;
        }
        જો(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            પાછા u_strchr(s, cs);
        }

        જ્યારે((c=*s++)!=0) {
            જો(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                માટે(;;) {
                    જો((cq=*q)==0) {
                        જો(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            પાછા (char16_t *)(s-1); /* well-formed match */
                        } નહીંતર {
                            તોડો; /* no match because surrogate pair is split */
                        }
                    }
                    જો((c=*p)==0) {
                        પાછા nullptr; /* no match, and none possible after s */
                    }
                    જો(c!=cq) {
                        તોડો; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        પાછા nullptr;
    }

    જો(subLength<0) {
        subLength=u_strlen(sub);
    }
    જો(subLength==0) {
        પાછા (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --subLength;
    subLimit=sub+subLength;

    જો(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        પાછા length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    જો(length<0) {
        /* s is NUL-terminated */
        જ્યારે((c=*s++)!=0) {
            જો(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                માટે(;;) {
                    જો(q==subLimit) {
                        જો(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            પાછા (char16_t *)(s-1); /* well-formed match */
                        } નહીંતર {
                            તોડો; /* no match because surrogate pair is split */
                        }
                    }
                    જો((c=*p)==0) {
                        પાછા nullptr; /* no match, and none possible after s */
                    }
                    જો(c!=*q) {
                        તોડો; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } નહીંતર {
        અચલ char16_t *limit, *preLimit;

        /* subLength was decremented above */
        જો(length<=subLength) {
            પાછા nullptr; /* s is shorter than sub */
        }

        limit=s+length;

        /* the substring must start before preLimit */
        preLimit=limit-subLength;

        જ્યારે(s!=preLimit) {
            c=*s++;
            જો(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                માટે(;;) {
                    જો(q==subLimit) {
                        જો(isMatchAtCPBoundary(start, s-1, p, limit)) {
                            પાછા (char16_t *)(s-1); /* well-formed match */
                        } નહીંતર {
                            તોડો; /* no match because surrogate pair is split */
                        }
                    }
                    જો(*p!=*q) {
                        તોડો; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    પાછા nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(અચલ char16_t* s U_LIFETIME_BOUND, અચલ char16_t* substring) {
    પાછા u_strFindFirst(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(અચલ char16_t* s U_LIFETIME_BOUND, char16_t c) {
    જો(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        પાછા u_strFindFirst(s, -1, &c, 1);
    } નહીંતર {
        char16_t cs;

        /* trivial search for a BMP code point */
        માટે(;;) {
            જો((cs=*s)==c) {
                પાછા (char16_t *)s;
            }
            જો(cs==0) {
                પાછા nullptr;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(અચલ char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    જો((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        પાછા u_strchr(s, (char16_t)c);
    } નહીંતર જો((uint32_t)c<=UCHAR_MAX_કિંમત) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        જ્યારે((cs=*s++)!=0) {
            જો(cs==lead && *s==trail) {
                પાછા (char16_t *)(s-1);
            }
        }
        પાછા nullptr;
    } નહીંતર {
        /* not a Unicode code point, not findable */
        પાછા nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(અચલ char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    જો(count<=0) {
        પાછા nullptr; /* no string */
    } નહીંતર જો(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        પાછા u_strFindFirst(s, count, &c, 1);
    } નહીંતર {
        /* trivial search for a BMP code point */
        અચલ char16_t *limit=s+count;
        કરો {
            જો(*s==c) {
                પાછા (char16_t *)s;
            }
        } જ્યારે(++s!=limit);
        પાછા nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(અચલ char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    જો((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        પાછા u_memchr(s, (char16_t)c, count);
    } નહીંતર જો(count<2) {
        /* too short for a surrogate pair */
        પાછા nullptr;
    } નહીંતર જો((uint32_t)c<=UCHAR_MAX_કિંમત) {
        /* find supplementary code point as surrogate pair */
        અચલ char16_t *limit=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        કરો {
            જો(*s==lead && *(s+1)==trail) {
                પાછા (char16_t *)s;
            }
        } જ્યારે(++s!=limit);
        પાછા nullptr;
    } નહીંતર {
        /* not a Unicode code point, not findable */
        પાછા nullptr;
    }
}

/* Backward binary string search functions ---------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strFindLast(અચલ char16_t* s U_LIFETIME_BOUND,
              int32_t length,
              અચલ char16_t* sub,
              int32_t subLength) {
    અચલ char16_t *start, *limit, *p, *q, *subLimit;
    char16_t c, cs;

    જો(sub==nullptr || subLength<-1) {
        પાછા (char16_t *)s;
    }
    જો(s==nullptr || length<-1) {
        પાછા nullptr;
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

    જો(subLength<0) {
        subLength=u_strlen(sub);
    }
    જો(subLength==0) {
        પાછા (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    subLimit=sub+subLength;
    cs=*(--subLimit);
    --subLength;

    જો(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        પાછા length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    જો(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    જો(length<=subLength) {
        પાછા nullptr; /* s is shorter than sub */
    }

    start=s;
    limit=s+length;

    /* the substring must start no later than s+subLength */
    s+=subLength;

    જ્યારે(s!=limit) {
        c=*(--limit);
        જો(c==cs) {
            /* found last substring char16_t, compare rest */
            p=limit;
            q=subLimit;
            માટે(;;) {
                જો(q==sub) {
                    જો(isMatchAtCPBoundary(start, p, limit+1, start+length)) {
                        પાછા (char16_t *)p; /* well-formed match */
                    } નહીંતર {
                        તોડો; /* no match because surrogate pair is split */
                    }
                }
                જો(*(--p)!=*(--q)) {
                    તોડો; /* no match */
                }
            }
        }
    }

    /* not found */
    પાછા nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(અચલ char16_t* s U_LIFETIME_BOUND, અચલ char16_t* substring) {
    પાછા u_strFindLast(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(અચલ char16_t* s U_LIFETIME_BOUND, char16_t c) {
    જો(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        પાછા u_strFindLast(s, -1, &c, 1);
    } નહીંતર {
        અચલ char16_t *result=nullptr;
        char16_t cs;

        /* trivial search for a BMP code point */
        માટે(;;) {
            જો((cs=*s)==c) {
                result=s;
            }
            જો(cs==0) {
                પાછા (char16_t *)result;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(અચલ char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    જો((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        પાછા u_strrchr(s, (char16_t)c);
    } નહીંતર જો((uint32_t)c<=UCHAR_MAX_કિંમત) {
        /* find supplementary code point as surrogate pair */
        અચલ char16_t *result=nullptr;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        જ્યારે((cs=*s++)!=0) {
            જો(cs==lead && *s==trail) {
                result=s-1;
            }
        }
        પાછા (char16_t *)result;
    } નહીંતર {
        /* not a Unicode code point, not findable */
        પાછા nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(અચલ char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    જો(count<=0) {
        પાછા nullptr; /* no string */
    } નહીંતર જો(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        પાછા u_strFindLast(s, count, &c, 1);
    } નહીંતર {
        /* trivial search for a BMP code point */
        અચલ char16_t *limit=s+count;
        કરો {
            જો(*(--limit)==c) {
                પાછા (char16_t *)limit;
            }
        } જ્યારે(s!=limit);
        પાછા nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(અચલ char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    જો((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        પાછા u_memrchr(s, (char16_t)c, count);
    } નહીંતર જો(count<2) {
        /* too short for a surrogate pair */
        પાછા nullptr;
    } નહીંતર જો((uint32_t)c<=UCHAR_MAX_કિંમત) {
        /* find supplementary code point as surrogate pair */
        અચલ char16_t *limit=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        કરો {
            જો(*limit==trail && *(limit-1)==lead) {
                પાછા (char16_t *)(limit-1);
            }
        } જ્યારે(s!=--limit);
        પાછા nullptr;
    } નહીંતર {
        /* not a Unicode code point, not findable */
        પાછા nullptr;
    }
}

/* Tokenization functions --------------------------------------------------- */

/*
 * Match each code point in a string against each code point in the matchSet.
 * Return the index of the first string code point that
 * is (polarity==true) or is not (false) contained in the matchSet.
 * Return -(string length)-1 if there is no such code point.
 */
સ્થિર int32_t
_matchFromSet(અચલ char16_t *string, અચલ char16_t *matchSet, UBool polarity) {
    int32_t matchLen, matchBMPLen, strItr, matchItr;
    UChar32 stringCh, matchCh;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    matchBMPLen = 0;
    જ્યારે((c = matchSet[matchBMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++matchBMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    matchLen = matchBMPLen;
    જ્યારે(matchSet[matchLen] != 0) {
        ++matchLen;
    }

    માટે(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        જો(U16_IS_SINGLE(c)) {
            જો(polarity) {
                માટે(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    જો(c == matchSet[matchItr]) {
                        પાછા strItr - 1; /* one matches */
                    }
                }
            } નહીંતર {
                માટે(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    જો(c == matchSet[matchItr]) {
                        જાઓ endloop;
                    }
                }
                પાછા strItr - 1; /* none matches */
            }
        } નહીંતર {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            જો(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } નહીંતર {
                stringCh = c; /* unpaired trail surrogate */
            }

            જો(polarity) {
                માટે(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    જો(stringCh == matchCh) {
                        પાછા strItr - U16_LENGTH(stringCh); /* one matches */
                    }
                }
            } નહીંતર {
                માટે(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    જો(stringCh == matchCh) {
                        જાઓ endloop;
                    }
                }
                પાછા strItr - U16_LENGTH(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    પાછા -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(અચલ char16_t* string U_LIFETIME_BOUND, અચલ char16_t* matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, સાચું);
    જો(idx >= 0) {
        પાછા (char16_t *)string + idx;
    } નહીંતર {
        પાછા nullptr;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(અચલ char16_t *string, અચલ char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, સાચું);
    જો(idx >= 0) {
        પાછા idx;
    } નહીંતર {
        પાછા -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(અચલ char16_t *string, અચલ char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, ખોટું);
    જો(idx >= 0) {
        પાછા idx;
    } નહીંતર {
        પાછા -idx - 1; /* == u_strlen(string) */
    }
}

/* ----- Text manipulation functions --- */

U_CAPI char16_t* U_EXPORT2
u_strtok_r(char16_t* src U_LIFETIME_BOUND, અચલ char16_t* delim, char16_t** saveState)
{
    char16_t *tokSource;
    char16_t *nextToken;
    uint32_t nonDelimIdx;

    /* If saveState is nullptr, the user messed up. */
    જો (src != nullptr) {
        tokSource = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    નહીંતર જો (*saveState) {
        tokSource = *saveState;
    }
    નહીંતર {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        પાછા nullptr;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tokSource, delim);
    tokSource = &tokSource[nonDelimIdx];

    જો (*tokSource) {
        nextToken = u_strpbrk(tokSource, delim);
        જો (nextToken != nullptr) {
            /* Create a token */
            *(nextToken++) = 0;
            *saveState = nextToken;
            પાછા tokSource;
        }
        નહીંતર જો (*saveState) {
            /* Return the last token */
            *saveState = nullptr;
            પાછા tokSource;
        }
    }
    નહીંતર {
        /* No tokens were found. Only delimiters were left. */
        *saveState = nullptr;
    }
    પાછા nullptr;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, અચલ char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    જ્યારે(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    જ્યારે((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    પાછા anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, અચલ char16_t* src, int32_t n)
{
    જો(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        જ્યારે(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        જ્યારે((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            જો(--n == 0) {
                *dst = 0;
                તોડો;
            }
            ++src;
        }

        પાછા anchor;
    } નહીંતર {
        પાછા dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(અચલ char16_t *s1,
    અચલ char16_t *s2)
{
    char16_t  c1, c2;

    માટે(;;) {
        c1=*s1++;
        c2=*s2++;
        જો (c1 != c2 || c1 == 0) {
            તોડો;
        }
    }
    પાછા (int32_t)c1 - (int32_t)c2;
}

U_CFUNC int32_t U_EXPORT2
uprv_strCompare(અચલ char16_t *s1, int32_t length1,
                અચલ char16_t *s2, int32_t length2,
                UBool strncmpStyle, UBool codePointOrder) {
    અચલ char16_t *start1, *start2, *limit1, *limit2;
    char16_t c1, c2;

    /* setup for fix-up */
    start1=s1;
    start2=s2;

    /* compare identical prefixes - they do not need to be fixed up */
    જો(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        જો(s1==s2) {
            પાછા 0;
        }

        માટે(;;) {
            c1=*s1;
            c2=*s2;
            જો(c1!=c2) {
                તોડો;
            }
            જો(c1==0) {
                પાછા 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=nullptr;
    } નહીંતર જો(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        જો(s1==s2) {
            પાછા 0;
        }

        limit1=start1+length1;

        માટે(;;) {
            /* both lengths are same, check only one limit */
            જો(s1==limit1) {
                પાછા 0;
            }

            c1=*s1;
            c2=*s2;
            જો(c1!=c2) {
                તોડો;
            }
            જો(c1==0) {
                પાછા 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } નહીંતર {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t lengthResult;

        જો(length1<0) {
            length1=u_strlen(s1);
        }
        જો(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        જો(length1<length2) {
            lengthResult=-1;
            limit1=start1+length1;
        } નહીંતર જો(length1==length2) {
            lengthResult=0;
            limit1=start1+length1;
        } નહીંતર /* length1>length2 */ {
            lengthResult=1;
            limit1=start1+length2;
        }

        જો(s1==s2) {
            પાછા lengthResult;
        }

        માટે(;;) {
            /* check pseudo-limit */
            જો(s1==limit1) {
                પાછા lengthResult;
            }

            c1=*s1;
            c2=*s2;
            જો(c1!=c2) {
                તોડો;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    જો(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        જો(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } નહીંતર {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        જો(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } નહીંતર {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    પાછા (int32_t)c1-(int32_t)c2;
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
    જો(iter1==nullptr || iter2==nullptr) {
        પાછા 0; /* bad arguments */
    }
    જો(iter1==iter2) {
        પાછા 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_START);
    iter2->move(iter2, 0, UITER_START);

    /* compare identical prefixes - they do not need to be fixed up */
    માટે(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        જો(c1!=c2) {
            તોડો;
        }
        જો(c1==-1) {
            પાછા 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    જો(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        જો(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->previous(iter1), U16_IS_LEAD(iter1->previous(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } નહીંતર {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        જો(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->previous(iter2), U16_IS_LEAD(iter2->previous(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } નહીંતર {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    પાછા (int32_t)c1-(int32_t)c2;
}

#જો 0
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
શૂન્ય fragment {
        /* iff a surrogate is part of a surrogate pair, leave >=d800 */
        જો(c1<=0xdbff) {
            જો(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } નહીંતર જો(c1<=0xdfff) {
            int32_t idx=iter1->getIndex(iter1, UITER_CURRENT);
            iter1->previous(iter1); /* ==c1 */
            જો(!U16_IS_LEAD(iter1->previous(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } નહીંતર /* 0xe000<=c1<=0xffff */ {
            /* BMP code point - make <d800 */
            c1-=0x2800;
        }
}
#endif

U_CAPI int32_t U_EXPORT2
u_strCompare(અચલ char16_t *s1, int32_t length1,
             અચલ char16_t *s2, int32_t length2,
             UBool codePointOrder) {
    /* argument checking */
    જો(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        પાછા 0;
    }
    પાછા uprv_strCompare(s1, length1, s2, length2, ખોટું, codePointOrder);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmpCodePointOrder(અચલ char16_t *s1, અચલ char16_t *s2) {
    પાછા uprv_strCompare(s1, -1, s2, -1, ખોટું, સાચું);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(અચલ char16_t  *s1,
     અચલ char16_t  *s2,
     int32_t     n) 
{
    જો(n > 0) {
        int32_t rc;
        માટે(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            જો(rc != 0 || *s1 == 0 || --n == 0) {
                પાછા rc;
            }
            ++s1;
            ++s2;
        }
    } નહીંતર {
        પાછા 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmpCodePointOrder(અચલ char16_t *s1, અચલ char16_t *s2, int32_t n) {
    પાછા uprv_strCompare(s1, n, s2, n, સાચું, સાચું);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, અચલ char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    જ્યારે((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    પાછા anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, અચલ char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    જ્યારે(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    પાછા anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(અચલ char16_t *s)
{
#જો U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    પાછા (int32_t)uprv_wcslen((અચલ wchar_t *)s);
#નહીંતર
    અચલ char16_t *t = s;
    જ્યારે(*t != 0) {
      ++t;
    }
    પાછા t - s;
#endif
}

U_CAPI int32_t U_EXPORT2
u_ગણતરી_Char32(અચલ char16_t *s, int32_t length) {
    int32_t count;

    જો(s==nullptr || length<-1) {
        પાછા 0;
    }

    count=0;
    જો(length>=0) {
        જ્યારે(length>0) {
            ++count;
            જો(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } નહીંતર {
                ++s;
                --length;
            }
        }
    } નહીંતર /* length==-1 */ {
        char16_t c;

        માટે(;;) {
            જો((c=*s++)==0) {
                તોડો;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            જો(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    પાછા count;
}

U_CAPI UBool U_EXPORT2
u_strHasMoreChar32Than(અચલ char16_t *s, int32_t length, int32_t number) {

    જો(number<0) {
        પાછા સાચું;
    }
    જો(s==nullptr || length<-1) {
        પાછા ખોટું;
    }

    જો(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        માટે(;;) {
            જો((c=*s++)==0) {
                પાછા ખોટું;
            }
            જો(number==0) {
                પાછા સાચું;
            }
            જો(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } નહીંતર {
        /* length>=0 known */
        અચલ char16_t *limit;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        જો(((length+1)/2)>number) {
            પાછા સાચું;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        જો(maxSupplementary<=0) {
            પાછા ખોટું;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        limit=s+length;
        માટે(;;) {
            જો(s==limit) {
                પાછા ખોટું;
            }
            જો(number==0) {
                પાછા સાચું;
            }
            જો(U16_IS_LEAD(*s++) && s!=limit && U16_IS_TRAIL(*s)) {
                ++s;
                જો(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    પાછા ખોટું;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, અચલ char16_t* src, int32_t count) {
    જો(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    પાછા dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, અચલ char16_t* src, int32_t count) {
    જો(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    પાછા dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    જો(count > 0) {
        char16_t *ptr = dest;
        char16_t *limit = dest + count;

        જ્યારે (ptr < limit) {
            *(ptr++) = c;
        }
    }
    પાછા dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(અચલ char16_t *buf1, અચલ char16_t *buf2, int32_t count) {
    જો(count > 0) {
        અચલ char16_t *limit = buf1 + count;
        int32_t result;

        જ્યારે (buf1 < limit) {
            result = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            જો (result != 0) {
                પાછા result;
            }
            buf1++;
            buf2++;
        }
    }
    પાછા 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmpCodePointOrder(અચલ char16_t *s1, અચલ char16_t *s2, int32_t count) {
    પાછા uprv_strCompare(s1, count, s2, count, ખોટું, સાચું);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
સ્થિર અચલ char16_t UNESCAPE_MAP[] = {
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
ગણના { UNESCAPE_MAP_LENGTH = UPRV_LENGTHOF(UNESCAPE_MAP) };

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
સ્થિર int32_t _digit8(char16_t c) {
    જો (c >= u'0' && c <= u'7') {
        પાછા c - u'0';
    }
    પાછા -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
સ્થિર int32_t _digit16(char16_t c) {
    જો (c >= u'0' && c <= u'9') {
        પાછા c - u'0';
    }
    જો (c >= u'A' && c <= u'F') {
        પાછા c - (u'A' - 10);
    }
    જો (c >= u'a' && c <= u'f') {
        પાછા c - (u'a' - 10);
    }
    પાછા -1;
}

/* Parse a single escape sequence.  Although this method deals in
 * UChars, it does not use C++ or UnicodeString.  This allows it to
 * be used from C contexts. */
U_CAPI UChar32 U_EXPORT2
u_unescapeAt(UNESCAPE_CHAR_AT charAt,
             int32_t *offset,
             int32_t length,
             શૂન્ય *context) {

    int32_t start = *offset;
    UChar32 c;
    UChar32 result = 0;
    int8_t n = 0;
    int8_t minDig = 0;
    int8_t maxDig = 0;
    int8_t bitsPerDigit = 4; 
    int32_t dig;
    UBool braces = ખોટું;

    /* Check that offset is in range */
    જો (*offset < 0 || *offset >= length) {
        જાઓ err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*offset)++, context);

    /* Convert hexadecimal and octal escapes */
    પસંદગી (c) {
    સ્થિતિ u'u':
        minDig = maxDig = 4;
        તોડો;
    સ્થિતિ u'U':
        minDig = maxDig = 8;
        તોડો;
    સ્થિતિ u'x':
        minDig = 1;
        જો (*offset < length && charAt(*offset, context) == u'{') {
            ++(*offset);
            braces = સાચું;
            maxDig = 8;
        } નહીંતર {
            maxDig = 2;
        }
        તોડો;
    મૂળભૂત:
        dig = _digit8(c);
        જો (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            bitsPerDigit = 3;
            result = dig;
        }
        તોડો;
    }
    જો (minDig != 0) {
        જ્યારે (*offset < length && n < maxDig) {
            c = charAt(*offset, context);
            dig = (bitsPerDigit == 3) ? _digit8(c) : _digit16(c);
            જો (dig < 0) {
                તોડો;
            }
            result = (result << bitsPerDigit) | dig;
            ++(*offset);
            ++n;
        }
        જો (n < minDig) {
            જાઓ err;
        }
        જો (braces) {
            જો (c != u'}') {
                જાઓ err;
            }
            ++(*offset);
        }
        જો (result < 0 || result >= 0x110000) {
            જાઓ err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        જો (*offset < length && U16_IS_LEAD(result)) {
            int32_t ahead = *offset + 1;
            c = charAt(*offset, context);
            જો (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t tailLimit = ahead + 11;
                જો (tailLimit > length) {
                    tailLimit = length;
                }
                c = u_unescapeAt(charAt, &ahead, tailLimit, context);
            }
            જો (U16_IS_TRAIL(c)) {
                *offset = ahead;
                result = U16_GET_SUPPLEMENTARY(result, c);
            }
        }
        પાછા result;
    }

    /* Convert C-style escapes in table */
    માટે (int32_t i=0; i<UNESCAPE_MAP_LENGTH; i+=2) {
        જો (c == UNESCAPE_MAP[i]) {
            પાછા UNESCAPE_MAP[i+1];
        } નહીંતર જો (c < UNESCAPE_MAP[i]) {
            તોડો;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    જો (c == u'c' && *offset < length) {
        c = charAt((*offset)++, context);
        જો (U16_IS_LEAD(c) && *offset < length) {
            char16_t c2 = charAt(*offset, context);
            જો (U16_IS_TRAIL(c2)) {
                ++(*offset);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        પાછા 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    જો (U16_IS_LEAD(c) && *offset < length) {
        char16_t c2 = charAt(*offset, context);
        જો (U16_IS_TRAIL(c2)) {
            ++(*offset);
            પાછા U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    પાછા c;

 err:
    /* Invalid escape sequence */
    *offset = start; /* Reset to initial value */
    પાછા (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
સ્થિર char16_t U_CALLCONV
_charPtr_charAt(int32_t offset, શૂન્ય *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_charsToUChars(static_cast<અક્ષર*>(context) + offset, &c16, 1);
    પાછા c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
સ્થિર શૂન્ય _appendUChars(char16_t *dest, int32_t destCapacity,
                          અચલ અક્ષર *src, int32_t srcLen) {
    જો (destCapacity < 0) {
        destCapacity = 0;
    }
    જો (srcLen > destCapacity) {
        srcLen = destCapacity;
    }
    u_charsToUChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(અચલ અક્ષર *src, char16_t *dest, int32_t destCapacity) {
    અચલ અક્ષર *segment = src;
    int32_t i = 0;
    અક્ષર c;

    જ્યારે ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        જો (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            જો (src != segment) {
                જો (dest != nullptr) {
                    _appendUChars(dest + i, destCapacity - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<અક્ષર*>(src));
            જો (lenParsed == 0) {
                જાઓ err;
            }
            src += lenParsed; /* advance past escape seq. */
            જો (dest != nullptr && U16_LENGTH(c32) <= (destCapacity - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } નહીંતર {
                i += U16_LENGTH(c32);
            }
            segment = src;
        } નહીંતર {
            ++src;
        }
    }
    જો (src != segment) {
        જો (dest != nullptr) {
            _appendUChars(dest + i, destCapacity - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    જો (dest != nullptr && i < destCapacity) {
        dest[i] = 0;
    }
    પાછા i;

 err:
    જો (dest != nullptr && destCapacity > 0) {
        *dest = 0;
    }
    પાછા 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#define __TERMINATE_STRING(dest, destCapacity, length, pErrorCode) UPRV_BLOCK_MACRO_BEGIN { \
    જો(pErrorCode!=nullptr && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        જો(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } નહીંતર જો(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            જો(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } નહીંતર જો(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } નહીંતર /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;                        \
        }                                                               \
    } \
} UPRV_BLOCK_MACRO_END

U_CAPI char16_t U_EXPORT2
u_asciiToUpper(char16_t c) {
    જો (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    પાછા c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    પાછા length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(અક્ષર *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    પાછા length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    પાછા length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    પાછા length;
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
    અચલ TYPE *p = (અચલ TYPE*) STR;        \
    જો (p != nullptr) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        અચલ TYPE *limit = p + len;          \
        જ્યારે (p<limit) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    પાછા static_cast<int32_t>(hash);        \
} UPRV_BLOCK_MACRO_END

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(અચલ char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(અચલ અક્ષર *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(અચલ અક્ષર *str, int32_t length) {
    STRING_HASH(અક્ષર, str, length, (uint8_t)uprv_tolower(*p));
}
