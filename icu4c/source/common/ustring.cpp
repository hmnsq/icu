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
isMatchAtCPBoundary(अचर char16_t *start, अचर char16_t *match, अचर char16_t *matchLimit, अचर char16_t *limit) {
    यदि(U16_IS_TRAIL(*match) && start!=match && U16_IS_LEAD(*(match-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        फर्काऊ असत्य;
    }
    यदि(U16_IS_LEAD(*(matchLimit-1)) && matchLimit!=limit && U16_IS_TRAIL(*matchLimit)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        फर्काऊ असत्य;
    }
    फर्काऊ सत्य;
}

U_CAPI char16_t* U_EXPORT2
u_strFindFirst(अचर char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               अचर char16_t* sub,
               int32_t subLength) {
    अचर char16_t *start, *p, *q, *subLimit;
    char16_t c, cs, cq;

    यदि(sub==nullptr || subLength<-1) {
        फर्काऊ (char16_t *)s;
    }
    यदि(s==nullptr || length<-1) {
        फर्काऊ nullptr;
    }

    start=s;

    यदि(length<0 && subLength<0) {
        /* both strings are NUL-terminated */
        यदि((cs=*sub++)==0) {
            फर्काऊ (char16_t *)s;
        }
        यदि(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            फर्काऊ u_strchr(s, cs);
        }

        जबसम्म((c=*s++)!=0) {
            यदि(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                लागि(;;) {
                    यदि((cq=*q)==0) {
                        यदि(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            फर्काऊ (char16_t *)(s-1); /* well-formed match */
                        } अन्यथा {
                            रोक; /* no match because surrogate pair is split */
                        }
                    }
                    यदि((c=*p)==0) {
                        फर्काऊ nullptr; /* no match, and none possible after s */
                    }
                    यदि(c!=cq) {
                        रोक; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        फर्काऊ nullptr;
    }

    यदि(subLength<0) {
        subLength=u_strlen(sub);
    }
    यदि(subLength==0) {
        फर्काऊ (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --subLength;
    subLimit=sub+subLength;

    यदि(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        फर्काऊ length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    यदि(length<0) {
        /* s is NUL-terminated */
        जबसम्म((c=*s++)!=0) {
            यदि(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                लागि(;;) {
                    यदि(q==subLimit) {
                        यदि(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            फर्काऊ (char16_t *)(s-1); /* well-formed match */
                        } अन्यथा {
                            रोक; /* no match because surrogate pair is split */
                        }
                    }
                    यदि((c=*p)==0) {
                        फर्काऊ nullptr; /* no match, and none possible after s */
                    }
                    यदि(c!=*q) {
                        रोक; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } अन्यथा {
        अचर char16_t *limit, *preLimit;

        /* subLength was decremented above */
        यदि(length<=subLength) {
            फर्काऊ nullptr; /* s is shorter than sub */
        }

        limit=s+length;

        /* the substring must start before preLimit */
        preLimit=limit-subLength;

        जबसम्म(s!=preLimit) {
            c=*s++;
            यदि(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                लागि(;;) {
                    यदि(q==subLimit) {
                        यदि(isMatchAtCPBoundary(start, s-1, p, limit)) {
                            फर्काऊ (char16_t *)(s-1); /* well-formed match */
                        } अन्यथा {
                            रोक; /* no match because surrogate pair is split */
                        }
                    }
                    यदि(*p!=*q) {
                        रोक; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    फर्काऊ nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(अचर char16_t* s U_LIFETIME_BOUND, अचर char16_t* substring) {
    फर्काऊ u_strFindFirst(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(अचर char16_t* s U_LIFETIME_BOUND, char16_t c) {
    यदि(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        फर्काऊ u_strFindFirst(s, -1, &c, 1);
    } अन्यथा {
        char16_t cs;

        /* trivial search for a BMP code point */
        लागि(;;) {
            यदि((cs=*s)==c) {
                फर्काऊ (char16_t *)s;
            }
            यदि(cs==0) {
                फर्काऊ nullptr;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(अचर char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    यदि((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        फर्काऊ u_strchr(s, (char16_t)c);
    } अन्यथा यदि((uint32_t)c<=UCHAR_MAX_मान) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        जबसम्म((cs=*s++)!=0) {
            यदि(cs==lead && *s==trail) {
                फर्काऊ (char16_t *)(s-1);
            }
        }
        फर्काऊ nullptr;
    } अन्यथा {
        /* not a Unicode code point, not findable */
        फर्काऊ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(अचर char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    यदि(count<=0) {
        फर्काऊ nullptr; /* no string */
    } अन्यथा यदि(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        फर्काऊ u_strFindFirst(s, count, &c, 1);
    } अन्यथा {
        /* trivial search for a BMP code point */
        अचर char16_t *limit=s+count;
        गर {
            यदि(*s==c) {
                फर्काऊ (char16_t *)s;
            }
        } जबसम्म(++s!=limit);
        फर्काऊ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(अचर char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    यदि((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        फर्काऊ u_memchr(s, (char16_t)c, count);
    } अन्यथा यदि(count<2) {
        /* too short for a surrogate pair */
        फर्काऊ nullptr;
    } अन्यथा यदि((uint32_t)c<=UCHAR_MAX_मान) {
        /* find supplementary code point as surrogate pair */
        अचर char16_t *limit=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        गर {
            यदि(*s==lead && *(s+1)==trail) {
                फर्काऊ (char16_t *)s;
            }
        } जबसम्म(++s!=limit);
        फर्काऊ nullptr;
    } अन्यथा {
        /* not a Unicode code point, not findable */
        फर्काऊ nullptr;
    }
}

/* Backward binary string search functions ---------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strFindLast(अचर char16_t* s U_LIFETIME_BOUND,
              int32_t length,
              अचर char16_t* sub,
              int32_t subLength) {
    अचर char16_t *start, *limit, *p, *q, *subLimit;
    char16_t c, cs;

    यदि(sub==nullptr || subLength<-1) {
        फर्काऊ (char16_t *)s;
    }
    यदि(s==nullptr || length<-1) {
        फर्काऊ nullptr;
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

    यदि(subLength<0) {
        subLength=u_strlen(sub);
    }
    यदि(subLength==0) {
        फर्काऊ (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    subLimit=sub+subLength;
    cs=*(--subLimit);
    --subLength;

    यदि(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        फर्काऊ length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    यदि(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    यदि(length<=subLength) {
        फर्काऊ nullptr; /* s is shorter than sub */
    }

    start=s;
    limit=s+length;

    /* the substring must start no later than s+subLength */
    s+=subLength;

    जबसम्म(s!=limit) {
        c=*(--limit);
        यदि(c==cs) {
            /* found last substring char16_t, compare rest */
            p=limit;
            q=subLimit;
            लागि(;;) {
                यदि(q==sub) {
                    यदि(isMatchAtCPBoundary(start, p, limit+1, start+length)) {
                        फर्काऊ (char16_t *)p; /* well-formed match */
                    } अन्यथा {
                        रोक; /* no match because surrogate pair is split */
                    }
                }
                यदि(*(--p)!=*(--q)) {
                    रोक; /* no match */
                }
            }
        }
    }

    /* not found */
    फर्काऊ nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(अचर char16_t* s U_LIFETIME_BOUND, अचर char16_t* substring) {
    फर्काऊ u_strFindLast(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(अचर char16_t* s U_LIFETIME_BOUND, char16_t c) {
    यदि(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        फर्काऊ u_strFindLast(s, -1, &c, 1);
    } अन्यथा {
        अचर char16_t *result=nullptr;
        char16_t cs;

        /* trivial search for a BMP code point */
        लागि(;;) {
            यदि((cs=*s)==c) {
                result=s;
            }
            यदि(cs==0) {
                फर्काऊ (char16_t *)result;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(अचर char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    यदि((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        फर्काऊ u_strrchr(s, (char16_t)c);
    } अन्यथा यदि((uint32_t)c<=UCHAR_MAX_मान) {
        /* find supplementary code point as surrogate pair */
        अचर char16_t *result=nullptr;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        जबसम्म((cs=*s++)!=0) {
            यदि(cs==lead && *s==trail) {
                result=s-1;
            }
        }
        फर्काऊ (char16_t *)result;
    } अन्यथा {
        /* not a Unicode code point, not findable */
        फर्काऊ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(अचर char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    यदि(count<=0) {
        फर्काऊ nullptr; /* no string */
    } अन्यथा यदि(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        फर्काऊ u_strFindLast(s, count, &c, 1);
    } अन्यथा {
        /* trivial search for a BMP code point */
        अचर char16_t *limit=s+count;
        गर {
            यदि(*(--limit)==c) {
                फर्काऊ (char16_t *)limit;
            }
        } जबसम्म(s!=limit);
        फर्काऊ nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(अचर char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    यदि((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        फर्काऊ u_memrchr(s, (char16_t)c, count);
    } अन्यथा यदि(count<2) {
        /* too short for a surrogate pair */
        फर्काऊ nullptr;
    } अन्यथा यदि((uint32_t)c<=UCHAR_MAX_मान) {
        /* find supplementary code point as surrogate pair */
        अचर char16_t *limit=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        गर {
            यदि(*limit==trail && *(limit-1)==lead) {
                फर्काऊ (char16_t *)(limit-1);
            }
        } जबसम्म(s!=--limit);
        फर्काऊ nullptr;
    } अन्यथा {
        /* not a Unicode code point, not findable */
        फर्काऊ nullptr;
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
_matchFromSet(अचर char16_t *string, अचर char16_t *matchSet, UBool polarity) {
    int32_t matchLen, matchBMPLen, strItr, matchItr;
    UChar32 stringCh, matchCh;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    matchBMPLen = 0;
    जबसम्म((c = matchSet[matchBMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++matchBMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    matchLen = matchBMPLen;
    जबसम्म(matchSet[matchLen] != 0) {
        ++matchLen;
    }

    लागि(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        यदि(U16_IS_SINGLE(c)) {
            यदि(polarity) {
                लागि(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    यदि(c == matchSet[matchItr]) {
                        फर्काऊ strItr - 1; /* one matches */
                    }
                }
            } अन्यथा {
                लागि(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    यदि(c == matchSet[matchItr]) {
                        गच्छ endloop;
                    }
                }
                फर्काऊ strItr - 1; /* none matches */
            }
        } अन्यथा {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            यदि(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } अन्यथा {
                stringCh = c; /* unpaired trail surrogate */
            }

            यदि(polarity) {
                लागि(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    यदि(stringCh == matchCh) {
                        फर्काऊ strItr - U16_LENGTH(stringCh); /* one matches */
                    }
                }
            } अन्यथा {
                लागि(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    यदि(stringCh == matchCh) {
                        गच्छ endloop;
                    }
                }
                फर्काऊ strItr - U16_LENGTH(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    फर्काऊ -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(अचर char16_t* string U_LIFETIME_BOUND, अचर char16_t* matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, सत्य);
    यदि(idx >= 0) {
        फर्काऊ (char16_t *)string + idx;
    } अन्यथा {
        फर्काऊ nullptr;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(अचर char16_t *string, अचर char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, सत्य);
    यदि(idx >= 0) {
        फर्काऊ idx;
    } अन्यथा {
        फर्काऊ -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(अचर char16_t *string, अचर char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, असत्य);
    यदि(idx >= 0) {
        फर्काऊ idx;
    } अन्यथा {
        फर्काऊ -idx - 1; /* == u_strlen(string) */
    }
}

/* ----- Text manipulation functions --- */

U_CAPI char16_t* U_EXPORT2
u_strtok_r(char16_t* src U_LIFETIME_BOUND, अचर char16_t* delim, char16_t** saveState)
{
    char16_t *tokSource;
    char16_t *nextToken;
    uint32_t nonDelimIdx;

    /* If saveState is nullptr, the user messed up. */
    यदि (src != nullptr) {
        tokSource = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    अन्यथा यदि (*saveState) {
        tokSource = *saveState;
    }
    अन्यथा {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        फर्काऊ nullptr;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tokSource, delim);
    tokSource = &tokSource[nonDelimIdx];

    यदि (*tokSource) {
        nextToken = u_strpbrk(tokSource, delim);
        यदि (nextToken != nullptr) {
            /* Create a token */
            *(nextToken++) = 0;
            *saveState = nextToken;
            फर्काऊ tokSource;
        }
        अन्यथा यदि (*saveState) {
            /* Return the last token */
            *saveState = nullptr;
            फर्काऊ tokSource;
        }
    }
    अन्यथा {
        /* No tokens were found. Only delimiters were left. */
        *saveState = nullptr;
    }
    फर्काऊ nullptr;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, अचर char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    जबसम्म(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    जबसम्म((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    फर्काऊ anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, अचर char16_t* src, int32_t n)
{
    यदि(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        जबसम्म(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        जबसम्म((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            यदि(--n == 0) {
                *dst = 0;
                रोक;
            }
            ++src;
        }

        फर्काऊ anchor;
    } अन्यथा {
        फर्काऊ dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(अचर char16_t *s1,
    अचर char16_t *s2)
{
    char16_t  c1, c2;

    लागि(;;) {
        c1=*s1++;
        c2=*s2++;
        यदि (c1 != c2 || c1 == 0) {
            रोक;
        }
    }
    फर्काऊ (int32_t)c1 - (int32_t)c2;
}

U_CFUNC int32_t U_EXPORT2
uprv_strCompare(अचर char16_t *s1, int32_t length1,
                अचर char16_t *s2, int32_t length2,
                UBool strncmpStyle, UBool codePointOrder) {
    अचर char16_t *start1, *start2, *limit1, *limit2;
    char16_t c1, c2;

    /* setup for fix-up */
    start1=s1;
    start2=s2;

    /* compare identical prefixes - they do not need to be fixed up */
    यदि(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        यदि(s1==s2) {
            फर्काऊ 0;
        }

        लागि(;;) {
            c1=*s1;
            c2=*s2;
            यदि(c1!=c2) {
                रोक;
            }
            यदि(c1==0) {
                फर्काऊ 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=nullptr;
    } अन्यथा यदि(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        यदि(s1==s2) {
            फर्काऊ 0;
        }

        limit1=start1+length1;

        लागि(;;) {
            /* both lengths are same, check only one limit */
            यदि(s1==limit1) {
                फर्काऊ 0;
            }

            c1=*s1;
            c2=*s2;
            यदि(c1!=c2) {
                रोक;
            }
            यदि(c1==0) {
                फर्काऊ 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } अन्यथा {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t lengthResult;

        यदि(length1<0) {
            length1=u_strlen(s1);
        }
        यदि(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        यदि(length1<length2) {
            lengthResult=-1;
            limit1=start1+length1;
        } अन्यथा यदि(length1==length2) {
            lengthResult=0;
            limit1=start1+length1;
        } अन्यथा /* length1>length2 */ {
            lengthResult=1;
            limit1=start1+length2;
        }

        यदि(s1==s2) {
            फर्काऊ lengthResult;
        }

        लागि(;;) {
            /* check pseudo-limit */
            यदि(s1==limit1) {
                फर्काऊ lengthResult;
            }

            c1=*s1;
            c2=*s2;
            यदि(c1!=c2) {
                रोक;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    यदि(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        यदि(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } अन्यथा {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        यदि(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } अन्यथा {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    फर्काऊ (int32_t)c1-(int32_t)c2;
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
    यदि(iter1==nullptr || iter2==nullptr) {
        फर्काऊ 0; /* bad arguments */
    }
    यदि(iter1==iter2) {
        फर्काऊ 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_START);
    iter2->move(iter2, 0, UITER_START);

    /* compare identical prefixes - they do not need to be fixed up */
    लागि(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        यदि(c1!=c2) {
            रोक;
        }
        यदि(c1==-1) {
            फर्काऊ 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    यदि(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        यदि(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->previous(iter1), U16_IS_LEAD(iter1->previous(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } अन्यथा {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        यदि(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->previous(iter2), U16_IS_LEAD(iter2->previous(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } अन्यथा {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    फर्काऊ (int32_t)c1-(int32_t)c2;
}

#यदि 0
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
        यदि(c1<=0xdbff) {
            यदि(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } अन्यथा यदि(c1<=0xdfff) {
            int32_t idx=iter1->getIndex(iter1, UITER_CURRENT);
            iter1->previous(iter1); /* ==c1 */
            यदि(!U16_IS_LEAD(iter1->previous(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } अन्यथा /* 0xe000<=c1<=0xffff */ {
            /* BMP code point - make <d800 */
            c1-=0x2800;
        }
}
#endif

U_CAPI int32_t U_EXPORT2
u_strCompare(अचर char16_t *s1, int32_t length1,
             अचर char16_t *s2, int32_t length2,
             UBool codePointOrder) {
    /* argument checking */
    यदि(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        फर्काऊ 0;
    }
    फर्काऊ uprv_strCompare(s1, length1, s2, length2, असत्य, codePointOrder);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmpCodePointOrder(अचर char16_t *s1, अचर char16_t *s2) {
    फर्काऊ uprv_strCompare(s1, -1, s2, -1, असत्य, सत्य);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(अचर char16_t  *s1,
     अचर char16_t  *s2,
     int32_t     n) 
{
    यदि(n > 0) {
        int32_t rc;
        लागि(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            यदि(rc != 0 || *s1 == 0 || --n == 0) {
                फर्काऊ rc;
            }
            ++s1;
            ++s2;
        }
    } अन्यथा {
        फर्काऊ 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmpCodePointOrder(अचर char16_t *s1, अचर char16_t *s2, int32_t n) {
    फर्काऊ uprv_strCompare(s1, n, s2, n, सत्य, सत्य);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, अचर char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    जबसम्म((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    फर्काऊ anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, अचर char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    जबसम्म(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    फर्काऊ anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(अचर char16_t *s)
{
#यदि U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    फर्काऊ (int32_t)uprv_wcslen((अचर ब_अक्षर_प्रकार *)s);
#अन्यथा
    अचर char16_t *t = s;
    जबसम्म(*t != 0) {
      ++t;
    }
    फर्काऊ t - s;
#endif
}

U_CAPI int32_t U_EXPORT2
u_गन्ती_Char32(अचर char16_t *s, int32_t length) {
    int32_t count;

    यदि(s==nullptr || length<-1) {
        फर्काऊ 0;
    }

    count=0;
    यदि(length>=0) {
        जबसम्म(length>0) {
            ++count;
            यदि(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } अन्यथा {
                ++s;
                --length;
            }
        }
    } अन्यथा /* length==-1 */ {
        char16_t c;

        लागि(;;) {
            यदि((c=*s++)==0) {
                रोक;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            यदि(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    फर्काऊ count;
}

U_CAPI UBool U_EXPORT2
u_strHasMoreChar32Than(अचर char16_t *s, int32_t length, int32_t number) {

    यदि(number<0) {
        फर्काऊ सत्य;
    }
    यदि(s==nullptr || length<-1) {
        फर्काऊ असत्य;
    }

    यदि(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        लागि(;;) {
            यदि((c=*s++)==0) {
                फर्काऊ असत्य;
            }
            यदि(number==0) {
                फर्काऊ सत्य;
            }
            यदि(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } अन्यथा {
        /* length>=0 known */
        अचर char16_t *limit;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        यदि(((length+1)/2)>number) {
            फर्काऊ सत्य;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        यदि(maxSupplementary<=0) {
            फर्काऊ असत्य;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        limit=s+length;
        लागि(;;) {
            यदि(s==limit) {
                फर्काऊ असत्य;
            }
            यदि(number==0) {
                फर्काऊ सत्य;
            }
            यदि(U16_IS_LEAD(*s++) && s!=limit && U16_IS_TRAIL(*s)) {
                ++s;
                यदि(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    फर्काऊ असत्य;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, अचर char16_t* src, int32_t count) {
    यदि(count > 0) {
        uprv_memcpy(dest, src, (माप_प्रकार)count*U_SIZEOF_UCHAR);
    }
    फर्काऊ dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, अचर char16_t* src, int32_t count) {
    यदि(count > 0) {
        uprv_memmove(dest, src, (माप_प्रकार)count*U_SIZEOF_UCHAR);
    }
    फर्काऊ dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    यदि(count > 0) {
        char16_t *ptr = dest;
        char16_t *limit = dest + count;

        जबसम्म (ptr < limit) {
            *(ptr++) = c;
        }
    }
    फर्काऊ dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(अचर char16_t *buf1, अचर char16_t *buf2, int32_t count) {
    यदि(count > 0) {
        अचर char16_t *limit = buf1 + count;
        int32_t result;

        जबसम्म (buf1 < limit) {
            result = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            यदि (result != 0) {
                फर्काऊ result;
            }
            buf1++;
            buf2++;
        }
    }
    फर्काऊ 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmpCodePointOrder(अचर char16_t *s1, अचर char16_t *s2, int32_t count) {
    फर्काऊ uprv_strCompare(s1, count, s2, count, असत्य, सत्य);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
स्थिर अचर char16_t UNESCAPE_MAP[] = {
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
    यदि (c >= u'0' && c <= u'7') {
        फर्काऊ c - u'0';
    }
    फर्काऊ -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
स्थिर int32_t _digit16(char16_t c) {
    यदि (c >= u'0' && c <= u'9') {
        फर्काऊ c - u'0';
    }
    यदि (c >= u'A' && c <= u'F') {
        फर्काऊ c - (u'A' - 10);
    }
    यदि (c >= u'a' && c <= u'f') {
        फर्काऊ c - (u'a' - 10);
    }
    फर्काऊ -1;
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
    UBool braces = असत्य;

    /* Check that offset is in range */
    यदि (*offset < 0 || *offset >= length) {
        गच्छ err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*offset)++, context);

    /* Convert hexadecimal and octal escapes */
    विकल्प (c) {
    पक्ष u'u':
        minDig = maxDig = 4;
        रोक;
    पक्ष u'U':
        minDig = maxDig = 8;
        रोक;
    पक्ष u'x':
        minDig = 1;
        यदि (*offset < length && charAt(*offset, context) == u'{') {
            ++(*offset);
            braces = सत्य;
            maxDig = 8;
        } अन्यथा {
            maxDig = 2;
        }
        रोक;
    स्वभाव:
        dig = _digit8(c);
        यदि (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            bitsPerDigit = 3;
            result = dig;
        }
        रोक;
    }
    यदि (minDig != 0) {
        जबसम्म (*offset < length && n < maxDig) {
            c = charAt(*offset, context);
            dig = (bitsPerDigit == 3) ? _digit8(c) : _digit16(c);
            यदि (dig < 0) {
                रोक;
            }
            result = (result << bitsPerDigit) | dig;
            ++(*offset);
            ++n;
        }
        यदि (n < minDig) {
            गच्छ err;
        }
        यदि (braces) {
            यदि (c != u'}') {
                गच्छ err;
            }
            ++(*offset);
        }
        यदि (result < 0 || result >= 0x110000) {
            गच्छ err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        यदि (*offset < length && U16_IS_LEAD(result)) {
            int32_t ahead = *offset + 1;
            c = charAt(*offset, context);
            यदि (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t tailLimit = ahead + 11;
                यदि (tailLimit > length) {
                    tailLimit = length;
                }
                c = u_unescapeAt(charAt, &ahead, tailLimit, context);
            }
            यदि (U16_IS_TRAIL(c)) {
                *offset = ahead;
                result = U16_GET_SUPPLEMENTARY(result, c);
            }
        }
        फर्काऊ result;
    }

    /* Convert C-style escapes in table */
    लागि (int32_t i=0; i<UNESCAPE_MAP_LENGTH; i+=2) {
        यदि (c == UNESCAPE_MAP[i]) {
            फर्काऊ UNESCAPE_MAP[i+1];
        } अन्यथा यदि (c < UNESCAPE_MAP[i]) {
            रोक;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    यदि (c == u'c' && *offset < length) {
        c = charAt((*offset)++, context);
        यदि (U16_IS_LEAD(c) && *offset < length) {
            char16_t c2 = charAt(*offset, context);
            यदि (U16_IS_TRAIL(c2)) {
                ++(*offset);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        फर्काऊ 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    यदि (U16_IS_LEAD(c) && *offset < length) {
        char16_t c2 = charAt(*offset, context);
        यदि (U16_IS_TRAIL(c2)) {
            ++(*offset);
            फर्काऊ U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    फर्काऊ c;

 err:
    /* Invalid escape sequence */
    *offset = start; /* Reset to initial value */
    फर्काऊ (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
स्थिर char16_t U_CALLCONV
_charPtr_charAt(int32_t offset, शून्य *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_charsToUChars(static_cast<वर्ण*>(context) + offset, &c16, 1);
    फर्काऊ c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
स्थिर शून्य _appendUChars(char16_t *dest, int32_t destCapacity,
                          अचर वर्ण *src, int32_t srcLen) {
    यदि (destCapacity < 0) {
        destCapacity = 0;
    }
    यदि (srcLen > destCapacity) {
        srcLen = destCapacity;
    }
    u_charsToUChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(अचर वर्ण *src, char16_t *dest, int32_t destCapacity) {
    अचर वर्ण *segment = src;
    int32_t i = 0;
    वर्ण c;

    जबसम्म ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        यदि (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            यदि (src != segment) {
                यदि (dest != nullptr) {
                    _appendUChars(dest + i, destCapacity - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<वर्ण*>(src));
            यदि (lenParsed == 0) {
                गच्छ err;
            }
            src += lenParsed; /* advance past escape seq. */
            यदि (dest != nullptr && U16_LENGTH(c32) <= (destCapacity - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } अन्यथा {
                i += U16_LENGTH(c32);
            }
            segment = src;
        } अन्यथा {
            ++src;
        }
    }
    यदि (src != segment) {
        यदि (dest != nullptr) {
            _appendUChars(dest + i, destCapacity - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    यदि (dest != nullptr && i < destCapacity) {
        dest[i] = 0;
    }
    फर्काऊ i;

 err:
    यदि (dest != nullptr && destCapacity > 0) {
        *dest = 0;
    }
    फर्काऊ 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#define __TERMINATE_STRING(dest, destCapacity, length, pErrorCode) UPRV_BLOCK_MACRO_BEGIN { \
    यदि(pErrorCode!=nullptr && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        यदि(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } अन्यथा यदि(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            यदि(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } अन्यथा यदि(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } अन्यथा /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;                        \
        }                                                               \
    } \
} UPRV_BLOCK_MACRO_END

U_CAPI char16_t U_EXPORT2
u_asciiToUpper(char16_t c) {
    यदि (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    फर्काऊ c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    फर्काऊ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(वर्ण *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    फर्काऊ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    फर्काऊ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(ब_अक्षर_प्रकार *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    फर्काऊ length;
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
    अचर TYPE *p = (अचर TYPE*) STR;        \
    यदि (p != nullptr) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        अचर TYPE *limit = p + len;          \
        जबसम्म (p<limit) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    फर्काऊ static_cast<int32_t>(hash);        \
} UPRV_BLOCK_MACRO_END

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(अचर char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(अचर वर्ण *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(अचर वर्ण *str, int32_t length) {
    STRING_HASH(वर्ण, str, length, (uint8_t)uprv_tolower(*p));
}
