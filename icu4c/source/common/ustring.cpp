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

#شامل "unicode/utypes.h"
#شامل "unicode/putil.h"
#شامل "unicode/uchar.h"
#شامل "unicode/ustring.h"
#شامل "unicode/utf16.h"
#شامل "cstring.h"
#شامل "cwchar.h"
#شامل "cmemory.h"
#شامل "ustr_imp.h"

/* ANSI string.h - style functions ------------------------------------------ */

/* U+ffff is the highest BMP code point, the highest one that fits into a 16-bit char16_t */
#define U_BMP_MAX 0xffff

/* Forward binary string search functions ----------------------------------- */

/*
 * Test if a substring match inside a string is at code point boundaries.
 * All pointers refer to the same buffer.
 * The limit pointer may be nullptr, all others must be real pointers.
 */
ایستا درونخطی UBool
isMatchAtCPBoundary(ثابت char16_t *start, ثابت char16_t *match, ثابت char16_t *matchLimit, ثابت char16_t *limit) {
    اگر(U16_IS_TRAIL(*match) && start!=match && U16_IS_LEAD(*(match-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        بازگشت نادرست;
    }
    اگر(U16_IS_LEAD(*(matchLimit-1)) && matchLimit!=limit && U16_IS_TRAIL(*matchLimit)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        بازگشت نادرست;
    }
    بازگشت درست;
}

U_CAPI char16_t* U_EXPORT2
u_strFindFirst(ثابت char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               ثابت char16_t* sub,
               int32_t subLength) {
    ثابت char16_t *start, *p, *q, *subLimit;
    char16_t c, cs, cq;

    اگر(sub==nullptr || subLength<-1) {
        بازگشت (char16_t *)s;
    }
    اگر(s==nullptr || length<-1) {
        بازگشت nullptr;
    }

    start=s;

    اگر(length<0 && subLength<0) {
        /* both strings are NUL-terminated */
        اگر((cs=*sub++)==0) {
            بازگشت (char16_t *)s;
        }
        اگر(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            بازگشت u_strchr(s, cs);
        }

        وقتی((c=*s++)!=0) {
            اگر(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                برای(;;) {
                    اگر((cq=*q)==0) {
                        اگر(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            بازگشت (char16_t *)(s-1); /* well-formed match */
                        } وگرنه {
                            بشکن; /* no match because surrogate pair is split */
                        }
                    }
                    اگر((c=*p)==0) {
                        بازگشت nullptr; /* no match, and none possible after s */
                    }
                    اگر(c!=cq) {
                        بشکن; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        بازگشت nullptr;
    }

    اگر(subLength<0) {
        subLength=u_strlen(sub);
    }
    اگر(subLength==0) {
        بازگشت (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --subLength;
    subLimit=sub+subLength;

    اگر(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        بازگشت length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    اگر(length<0) {
        /* s is NUL-terminated */
        وقتی((c=*s++)!=0) {
            اگر(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                برای(;;) {
                    اگر(q==subLimit) {
                        اگر(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            بازگشت (char16_t *)(s-1); /* well-formed match */
                        } وگرنه {
                            بشکن; /* no match because surrogate pair is split */
                        }
                    }
                    اگر((c=*p)==0) {
                        بازگشت nullptr; /* no match, and none possible after s */
                    }
                    اگر(c!=*q) {
                        بشکن; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } وگرنه {
        ثابت char16_t *limit, *preLimit;

        /* subLength was decremented above */
        اگر(length<=subLength) {
            بازگشت nullptr; /* s is shorter than sub */
        }

        limit=s+length;

        /* the substring must start before preLimit */
        preLimit=limit-subLength;

        وقتی(s!=preLimit) {
            c=*s++;
            اگر(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                برای(;;) {
                    اگر(q==subLimit) {
                        اگر(isMatchAtCPBoundary(start, s-1, p, limit)) {
                            بازگشت (char16_t *)(s-1); /* well-formed match */
                        } وگرنه {
                            بشکن; /* no match because surrogate pair is split */
                        }
                    }
                    اگر(*p!=*q) {
                        بشکن; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    بازگشت nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(ثابت char16_t* s U_LIFETIME_BOUND, ثابت char16_t* substring) {
    بازگشت u_strFindFirst(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(ثابت char16_t* s U_LIFETIME_BOUND, char16_t c) {
    اگر(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        بازگشت u_strFindFirst(s, -1, &c, 1);
    } وگرنه {
        char16_t cs;

        /* trivial search for a BMP code point */
        برای(;;) {
            اگر((cs=*s)==c) {
                بازگشت (char16_t *)s;
            }
            اگر(cs==0) {
                بازگشت nullptr;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(ثابت char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    اگر((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        بازگشت u_strchr(s, (char16_t)c);
    } وگرنه اگر((uint32_t)c<=UCHAR_MAX_مقدار) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        وقتی((cs=*s++)!=0) {
            اگر(cs==lead && *s==trail) {
                بازگشت (char16_t *)(s-1);
            }
        }
        بازگشت nullptr;
    } وگرنه {
        /* not a Unicode code point, not findable */
        بازگشت nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(ثابت char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    اگر(count<=0) {
        بازگشت nullptr; /* no string */
    } وگرنه اگر(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        بازگشت u_strFindFirst(s, count, &c, 1);
    } وگرنه {
        /* trivial search for a BMP code point */
        ثابت char16_t *limit=s+count;
        انجامبده {
            اگر(*s==c) {
                بازگشت (char16_t *)s;
            }
        } وقتی(++s!=limit);
        بازگشت nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(ثابت char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    اگر((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        بازگشت u_memchr(s, (char16_t)c, count);
    } وگرنه اگر(count<2) {
        /* too short for a surrogate pair */
        بازگشت nullptr;
    } وگرنه اگر((uint32_t)c<=UCHAR_MAX_مقدار) {
        /* find supplementary code point as surrogate pair */
        ثابت char16_t *limit=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        انجامبده {
            اگر(*s==lead && *(s+1)==trail) {
                بازگشت (char16_t *)s;
            }
        } وقتی(++s!=limit);
        بازگشت nullptr;
    } وگرنه {
        /* not a Unicode code point, not findable */
        بازگشت nullptr;
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

    اگر(sub==nullptr || subLength<-1) {
        بازگشت (char16_t *)s;
    }
    اگر(s==nullptr || length<-1) {
        بازگشت nullptr;
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

    اگر(subLength<0) {
        subLength=u_strlen(sub);
    }
    اگر(subLength==0) {
        بازگشت (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    subLimit=sub+subLength;
    cs=*(--subLimit);
    --subLength;

    اگر(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        بازگشت length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    اگر(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    اگر(length<=subLength) {
        بازگشت nullptr; /* s is shorter than sub */
    }

    start=s;
    limit=s+length;

    /* the substring must start no later than s+subLength */
    s+=subLength;

    وقتی(s!=limit) {
        c=*(--limit);
        اگر(c==cs) {
            /* found last substring char16_t, compare rest */
            p=limit;
            q=subLimit;
            برای(;;) {
                اگر(q==sub) {
                    اگر(isMatchAtCPBoundary(start, p, limit+1, start+length)) {
                        بازگشت (char16_t *)p; /* well-formed match */
                    } وگرنه {
                        بشکن; /* no match because surrogate pair is split */
                    }
                }
                اگر(*(--p)!=*(--q)) {
                    بشکن; /* no match */
                }
            }
        }
    }

    /* not found */
    بازگشت nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(ثابت char16_t* s U_LIFETIME_BOUND, ثابت char16_t* substring) {
    بازگشت u_strFindLast(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(ثابت char16_t* s U_LIFETIME_BOUND, char16_t c) {
    اگر(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        بازگشت u_strFindLast(s, -1, &c, 1);
    } وگرنه {
        ثابت char16_t *result=nullptr;
        char16_t cs;

        /* trivial search for a BMP code point */
        برای(;;) {
            اگر((cs=*s)==c) {
                result=s;
            }
            اگر(cs==0) {
                بازگشت (char16_t *)result;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(ثابت char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    اگر((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        بازگشت u_strrchr(s, (char16_t)c);
    } وگرنه اگر((uint32_t)c<=UCHAR_MAX_مقدار) {
        /* find supplementary code point as surrogate pair */
        ثابت char16_t *result=nullptr;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        وقتی((cs=*s++)!=0) {
            اگر(cs==lead && *s==trail) {
                result=s-1;
            }
        }
        بازگشت (char16_t *)result;
    } وگرنه {
        /* not a Unicode code point, not findable */
        بازگشت nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(ثابت char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    اگر(count<=0) {
        بازگشت nullptr; /* no string */
    } وگرنه اگر(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        بازگشت u_strFindLast(s, count, &c, 1);
    } وگرنه {
        /* trivial search for a BMP code point */
        ثابت char16_t *limit=s+count;
        انجامبده {
            اگر(*(--limit)==c) {
                بازگشت (char16_t *)limit;
            }
        } وقتی(s!=limit);
        بازگشت nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(ثابت char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    اگر((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        بازگشت u_memrchr(s, (char16_t)c, count);
    } وگرنه اگر(count<2) {
        /* too short for a surrogate pair */
        بازگشت nullptr;
    } وگرنه اگر((uint32_t)c<=UCHAR_MAX_مقدار) {
        /* find supplementary code point as surrogate pair */
        ثابت char16_t *limit=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        انجامبده {
            اگر(*limit==trail && *(limit-1)==lead) {
                بازگشت (char16_t *)(limit-1);
            }
        } وقتی(s!=--limit);
        بازگشت nullptr;
    } وگرنه {
        /* not a Unicode code point, not findable */
        بازگشت nullptr;
    }
}

/* Tokenization functions --------------------------------------------------- */

/*
 * Match each code point in a string against each code point in the matchSet.
 * Return the index of the first string code point that
 * is (polarity==true) or is not (false) contained in the matchSet.
 * Return -(string length)-1 if there is no such code point.
 */
ایستا int32_t
_matchFromSet(ثابت char16_t *string, ثابت char16_t *matchSet, UBool polarity) {
    int32_t matchLen, matchBMPLen, strItr, matchItr;
    UChar32 stringCh, matchCh;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    matchBMPLen = 0;
    وقتی((c = matchSet[matchBMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++matchBMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    matchLen = matchBMPLen;
    وقتی(matchSet[matchLen] != 0) {
        ++matchLen;
    }

    برای(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        اگر(U16_IS_SINGLE(c)) {
            اگر(polarity) {
                برای(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    اگر(c == matchSet[matchItr]) {
                        بازگشت strItr - 1; /* one matches */
                    }
                }
            } وگرنه {
                برای(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    اگر(c == matchSet[matchItr]) {
                        برو endloop;
                    }
                }
                بازگشت strItr - 1; /* none matches */
            }
        } وگرنه {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            اگر(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } وگرنه {
                stringCh = c; /* unpaired trail surrogate */
            }

            اگر(polarity) {
                برای(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    اگر(stringCh == matchCh) {
                        بازگشت strItr - U16_LENGTH(stringCh); /* one matches */
                    }
                }
            } وگرنه {
                برای(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    اگر(stringCh == matchCh) {
                        برو endloop;
                    }
                }
                بازگشت strItr - U16_LENGTH(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    بازگشت -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(ثابت char16_t* string U_LIFETIME_BOUND, ثابت char16_t* matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, درست);
    اگر(idx >= 0) {
        بازگشت (char16_t *)string + idx;
    } وگرنه {
        بازگشت nullptr;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(ثابت char16_t *string, ثابت char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, درست);
    اگر(idx >= 0) {
        بازگشت idx;
    } وگرنه {
        بازگشت -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(ثابت char16_t *string, ثابت char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, نادرست);
    اگر(idx >= 0) {
        بازگشت idx;
    } وگرنه {
        بازگشت -idx - 1; /* == u_strlen(string) */
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
    اگر (src != nullptr) {
        tokSource = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    وگرنه اگر (*saveState) {
        tokSource = *saveState;
    }
    وگرنه {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        بازگشت nullptr;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tokSource, delim);
    tokSource = &tokSource[nonDelimIdx];

    اگر (*tokSource) {
        nextToken = u_strpbrk(tokSource, delim);
        اگر (nextToken != nullptr) {
            /* Create a token */
            *(nextToken++) = 0;
            *saveState = nextToken;
            بازگشت tokSource;
        }
        وگرنه اگر (*saveState) {
            /* Return the last token */
            *saveState = nullptr;
            بازگشت tokSource;
        }
    }
    وگرنه {
        /* No tokens were found. Only delimiters were left. */
        *saveState = nullptr;
    }
    بازگشت nullptr;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, ثابت char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    وقتی(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    وقتی((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    بازگشت anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, ثابت char16_t* src, int32_t n)
{
    اگر(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        وقتی(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        وقتی((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            اگر(--n == 0) {
                *dst = 0;
                بشکن;
            }
            ++src;
        }

        بازگشت anchor;
    } وگرنه {
        بازگشت dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(ثابت char16_t *s1,
    ثابت char16_t *s2)
{
    char16_t  c1, c2;

    برای(;;) {
        c1=*s1++;
        c2=*s2++;
        اگر (c1 != c2 || c1 == 0) {
            بشکن;
        }
    }
    بازگشت (int32_t)c1 - (int32_t)c2;
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
    اگر(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        اگر(s1==s2) {
            بازگشت 0;
        }

        برای(;;) {
            c1=*s1;
            c2=*s2;
            اگر(c1!=c2) {
                بشکن;
            }
            اگر(c1==0) {
                بازگشت 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=nullptr;
    } وگرنه اگر(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        اگر(s1==s2) {
            بازگشت 0;
        }

        limit1=start1+length1;

        برای(;;) {
            /* both lengths are same, check only one limit */
            اگر(s1==limit1) {
                بازگشت 0;
            }

            c1=*s1;
            c2=*s2;
            اگر(c1!=c2) {
                بشکن;
            }
            اگر(c1==0) {
                بازگشت 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } وگرنه {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t lengthResult;

        اگر(length1<0) {
            length1=u_strlen(s1);
        }
        اگر(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        اگر(length1<length2) {
            lengthResult=-1;
            limit1=start1+length1;
        } وگرنه اگر(length1==length2) {
            lengthResult=0;
            limit1=start1+length1;
        } وگرنه /* length1>length2 */ {
            lengthResult=1;
            limit1=start1+length2;
        }

        اگر(s1==s2) {
            بازگشت lengthResult;
        }

        برای(;;) {
            /* check pseudo-limit */
            اگر(s1==limit1) {
                بازگشت lengthResult;
            }

            c1=*s1;
            c2=*s2;
            اگر(c1!=c2) {
                بشکن;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    اگر(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        اگر(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } وگرنه {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        اگر(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } وگرنه {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    بازگشت (int32_t)c1-(int32_t)c2;
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
    اگر(iter1==nullptr || iter2==nullptr) {
        بازگشت 0; /* bad arguments */
    }
    اگر(iter1==iter2) {
        بازگشت 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_START);
    iter2->move(iter2, 0, UITER_START);

    /* compare identical prefixes - they do not need to be fixed up */
    برای(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        اگر(c1!=c2) {
            بشکن;
        }
        اگر(c1==-1) {
            بازگشت 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    اگر(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        اگر(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->previous(iter1), U16_IS_LEAD(iter1->previous(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } وگرنه {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        اگر(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->previous(iter2), U16_IS_LEAD(iter2->previous(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } وگرنه {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    بازگشت (int32_t)c1-(int32_t)c2;
}

#اگر 0
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
تهی fragment {
        /* iff a surrogate is part of a surrogate pair, leave >=d800 */
        اگر(c1<=0xdbff) {
            اگر(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } وگرنه اگر(c1<=0xdfff) {
            int32_t idx=iter1->getIndex(iter1, UITER_CURRENT);
            iter1->previous(iter1); /* ==c1 */
            اگر(!U16_IS_LEAD(iter1->previous(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } وگرنه /* 0xe000<=c1<=0xffff */ {
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
    اگر(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        بازگشت 0;
    }
    بازگشت uprv_strCompare(s1, length1, s2, length2, نادرست, codePointOrder);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmpCodePointOrder(ثابت char16_t *s1, ثابت char16_t *s2) {
    بازگشت uprv_strCompare(s1, -1, s2, -1, نادرست, درست);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(ثابت char16_t  *s1,
     ثابت char16_t  *s2,
     int32_t     n) 
{
    اگر(n > 0) {
        int32_t rc;
        برای(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            اگر(rc != 0 || *s1 == 0 || --n == 0) {
                بازگشت rc;
            }
            ++s1;
            ++s2;
        }
    } وگرنه {
        بازگشت 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmpCodePointOrder(ثابت char16_t *s1, ثابت char16_t *s2, int32_t n) {
    بازگشت uprv_strCompare(s1, n, s2, n, درست, درست);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, ثابت char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    وقتی((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    بازگشت anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, ثابت char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    وقتی(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    بازگشت anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(ثابت char16_t *s)
{
#اگر U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    بازگشت (int32_t)uprv_wcslen((ثابت wchar_t *)s);
#وگرنه
    ثابت char16_t *t = s;
    وقتی(*t != 0) {
      ++t;
    }
    بازگشت t - s;
#endif
}

U_CAPI int32_t U_EXPORT2
u_شمار_Char32(ثابت char16_t *s, int32_t length) {
    int32_t count;

    اگر(s==nullptr || length<-1) {
        بازگشت 0;
    }

    count=0;
    اگر(length>=0) {
        وقتی(length>0) {
            ++count;
            اگر(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } وگرنه {
                ++s;
                --length;
            }
        }
    } وگرنه /* length==-1 */ {
        char16_t c;

        برای(;;) {
            اگر((c=*s++)==0) {
                بشکن;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            اگر(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    بازگشت count;
}

U_CAPI UBool U_EXPORT2
u_strHasMoreChar32Than(ثابت char16_t *s, int32_t length, int32_t number) {

    اگر(number<0) {
        بازگشت درست;
    }
    اگر(s==nullptr || length<-1) {
        بازگشت نادرست;
    }

    اگر(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        برای(;;) {
            اگر((c=*s++)==0) {
                بازگشت نادرست;
            }
            اگر(number==0) {
                بازگشت درست;
            }
            اگر(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } وگرنه {
        /* length>=0 known */
        ثابت char16_t *limit;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        اگر(((length+1)/2)>number) {
            بازگشت درست;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        اگر(maxSupplementary<=0) {
            بازگشت نادرست;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        limit=s+length;
        برای(;;) {
            اگر(s==limit) {
                بازگشت نادرست;
            }
            اگر(number==0) {
                بازگشت درست;
            }
            اگر(U16_IS_LEAD(*s++) && s!=limit && U16_IS_TRAIL(*s)) {
                ++s;
                اگر(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    بازگشت نادرست;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, ثابت char16_t* src, int32_t count) {
    اگر(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    بازگشت dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, ثابت char16_t* src, int32_t count) {
    اگر(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    بازگشت dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    اگر(count > 0) {
        char16_t *ptr = dest;
        char16_t *limit = dest + count;

        وقتی (ptr < limit) {
            *(ptr++) = c;
        }
    }
    بازگشت dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(ثابت char16_t *buf1, ثابت char16_t *buf2, int32_t count) {
    اگر(count > 0) {
        ثابت char16_t *limit = buf1 + count;
        int32_t result;

        وقتی (buf1 < limit) {
            result = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            اگر (result != 0) {
                بازگشت result;
            }
            buf1++;
            buf2++;
        }
    }
    بازگشت 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmpCodePointOrder(ثابت char16_t *s1, ثابت char16_t *s2, int32_t count) {
    بازگشت uprv_strCompare(s1, count, s2, count, نادرست, درست);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
ایستا ثابت char16_t UNESCAPE_MAP[] = {
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
شمارش { UNESCAPE_MAP_LENGTH = UPRV_LENGTHOF(UNESCAPE_MAP) };

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
ایستا int32_t _digit8(char16_t c) {
    اگر (c >= u'0' && c <= u'7') {
        بازگشت c - u'0';
    }
    بازگشت -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
ایستا int32_t _digit16(char16_t c) {
    اگر (c >= u'0' && c <= u'9') {
        بازگشت c - u'0';
    }
    اگر (c >= u'A' && c <= u'F') {
        بازگشت c - (u'A' - 10);
    }
    اگر (c >= u'a' && c <= u'f') {
        بازگشت c - (u'a' - 10);
    }
    بازگشت -1;
}

/* Parse a single escape sequence.  Although this method deals in
 * UChars, it does not use C++ or UnicodeString.  This allows it to
 * be used from C contexts. */
U_CAPI UChar32 U_EXPORT2
u_unescapeAt(UNESCAPE_CHAR_AT charAt,
             int32_t *offset,
             int32_t length,
             تهی *context) {

    int32_t start = *offset;
    UChar32 c;
    UChar32 result = 0;
    int8_t n = 0;
    int8_t minDig = 0;
    int8_t maxDig = 0;
    int8_t bitsPerDigit = 4; 
    int32_t dig;
    UBool braces = نادرست;

    /* Check that offset is in range */
    اگر (*offset < 0 || *offset >= length) {
        برو err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*offset)++, context);

    /* Convert hexadecimal and octal escapes */
    انتخاب (c) {
    حالت u'u':
        minDig = maxDig = 4;
        بشکن;
    حالت u'U':
        minDig = maxDig = 8;
        بشکن;
    حالت u'x':
        minDig = 1;
        اگر (*offset < length && charAt(*offset, context) == u'{') {
            ++(*offset);
            braces = درست;
            maxDig = 8;
        } وگرنه {
            maxDig = 2;
        }
        بشکن;
    پیشفرض:
        dig = _digit8(c);
        اگر (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            bitsPerDigit = 3;
            result = dig;
        }
        بشکن;
    }
    اگر (minDig != 0) {
        وقتی (*offset < length && n < maxDig) {
            c = charAt(*offset, context);
            dig = (bitsPerDigit == 3) ? _digit8(c) : _digit16(c);
            اگر (dig < 0) {
                بشکن;
            }
            result = (result << bitsPerDigit) | dig;
            ++(*offset);
            ++n;
        }
        اگر (n < minDig) {
            برو err;
        }
        اگر (braces) {
            اگر (c != u'}') {
                برو err;
            }
            ++(*offset);
        }
        اگر (result < 0 || result >= 0x110000) {
            برو err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        اگر (*offset < length && U16_IS_LEAD(result)) {
            int32_t ahead = *offset + 1;
            c = charAt(*offset, context);
            اگر (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t tailLimit = ahead + 11;
                اگر (tailLimit > length) {
                    tailLimit = length;
                }
                c = u_unescapeAt(charAt, &ahead, tailLimit, context);
            }
            اگر (U16_IS_TRAIL(c)) {
                *offset = ahead;
                result = U16_GET_SUPPLEMENTARY(result, c);
            }
        }
        بازگشت result;
    }

    /* Convert C-style escapes in table */
    برای (int32_t i=0; i<UNESCAPE_MAP_LENGTH; i+=2) {
        اگر (c == UNESCAPE_MAP[i]) {
            بازگشت UNESCAPE_MAP[i+1];
        } وگرنه اگر (c < UNESCAPE_MAP[i]) {
            بشکن;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    اگر (c == u'c' && *offset < length) {
        c = charAt((*offset)++, context);
        اگر (U16_IS_LEAD(c) && *offset < length) {
            char16_t c2 = charAt(*offset, context);
            اگر (U16_IS_TRAIL(c2)) {
                ++(*offset);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        بازگشت 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    اگر (U16_IS_LEAD(c) && *offset < length) {
        char16_t c2 = charAt(*offset, context);
        اگر (U16_IS_TRAIL(c2)) {
            ++(*offset);
            بازگشت U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    بازگشت c;

 err:
    /* Invalid escape sequence */
    *offset = start; /* Reset to initial value */
    بازگشت (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
ایستا char16_t U_CALLCONV
_charPtr_charAt(int32_t offset, تهی *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_charsToUChars(static_cast<نویسه*>(context) + offset, &c16, 1);
    بازگشت c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
ایستا تهی _appendUChars(char16_t *dest, int32_t destCapacity,
                          ثابت نویسه *src, int32_t srcLen) {
    اگر (destCapacity < 0) {
        destCapacity = 0;
    }
    اگر (srcLen > destCapacity) {
        srcLen = destCapacity;
    }
    u_charsToUChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(ثابت نویسه *src, char16_t *dest, int32_t destCapacity) {
    ثابت نویسه *segment = src;
    int32_t i = 0;
    نویسه c;

    وقتی ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        اگر (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            اگر (src != segment) {
                اگر (dest != nullptr) {
                    _appendUChars(dest + i, destCapacity - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<نویسه*>(src));
            اگر (lenParsed == 0) {
                برو err;
            }
            src += lenParsed; /* advance past escape seq. */
            اگر (dest != nullptr && U16_LENGTH(c32) <= (destCapacity - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } وگرنه {
                i += U16_LENGTH(c32);
            }
            segment = src;
        } وگرنه {
            ++src;
        }
    }
    اگر (src != segment) {
        اگر (dest != nullptr) {
            _appendUChars(dest + i, destCapacity - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    اگر (dest != nullptr && i < destCapacity) {
        dest[i] = 0;
    }
    بازگشت i;

 err:
    اگر (dest != nullptr && destCapacity > 0) {
        *dest = 0;
    }
    بازگشت 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#define __TERMINATE_STRING(dest, destCapacity, length, pErrorCode) UPRV_BLOCK_MACRO_BEGIN { \
    اگر(pErrorCode!=nullptr && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        اگر(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } وگرنه اگر(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            اگر(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } وگرنه اگر(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } وگرنه /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;                        \
        }                                                               \
    } \
} UPRV_BLOCK_MACRO_END

U_CAPI char16_t U_EXPORT2
u_asciiToUpper(char16_t c) {
    اگر (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    بازگشت c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    بازگشت length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(نویسه *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    بازگشت length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    بازگشت length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    بازگشت length;
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
    اگر (p != nullptr) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        ثابت TYPE *limit = p + len;          \
        وقتی (p<limit) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    بازگشت static_cast<int32_t>(hash);        \
} UPRV_BLOCK_MACRO_END

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(ثابت char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(ثابت نویسه *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(ثابت نویسه *str, int32_t length) {
    STRING_HASH(نویسه, str, length, (uint8_t)uprv_tolower(*p));
}
