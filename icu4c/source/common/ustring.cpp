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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/putil.h"
#অন্তর্ভাৱ "unicode/uchar.h"
#অন্তর্ভাৱ "unicode/ustring.h"
#অন্তর্ভাৱ "unicode/utf16.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "cwchar.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "ustr_imp.h"

/* ANSI string.h - style functions ------------------------------------------ */

/* U+ffff is the highest BMP code point, the highest one that fits into a 16-bit char16_t */
#define U_BMP_MAX 0xffff

/* Forward binary string search functions ----------------------------------- */

/*
 * Test if a substring match inside a string is at code point boundaries.
 * All pointers refer to the same buffer.
 * The limit pointer may be nullptr, all others must be real pointers.
 */
স্থির অন্তঃস্থ UBool
isMatchAtCPBoundary(ধ্রুৱ char16_t *start, ধ্রুৱ char16_t *match, ধ্রুৱ char16_t *matchLimit, ধ্রুৱ char16_t *limit) {
    যদি(U16_IS_TRAIL(*match) && start!=match && U16_IS_LEAD(*(match-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        ঘূৰাই মিছা;
    }
    যদি(U16_IS_LEAD(*(matchLimit-1)) && matchLimit!=limit && U16_IS_TRAIL(*matchLimit)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        ঘূৰাই মিছা;
    }
    ঘূৰাই সঁচা;
}

U_CAPI char16_t* U_EXPORT2
u_strFindFirst(ধ্রুৱ char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               ধ্রুৱ char16_t* sub,
               int32_t subLength) {
    ধ্রুৱ char16_t *start, *p, *q, *subLimit;
    char16_t c, cs, cq;

    যদি(sub==nullptr || subLength<-1) {
        ঘূৰাই (char16_t *)s;
    }
    যদি(s==nullptr || length<-1) {
        ঘূৰাই nullptr;
    }

    start=s;

    যদি(length<0 && subLength<0) {
        /* both strings are NUL-terminated */
        যদি((cs=*sub++)==0) {
            ঘূৰাই (char16_t *)s;
        }
        যদি(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            ঘূৰাই u_strchr(s, cs);
        }

        যেতিয়ালৈ((c=*s++)!=0) {
            যদি(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                বাবে(;;) {
                    যদি((cq=*q)==0) {
                        যদি(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            ঘূৰাই (char16_t *)(s-1); /* well-formed match */
                        } অন্যথা {
                            ভঙ্গ; /* no match because surrogate pair is split */
                        }
                    }
                    যদি((c=*p)==0) {
                        ঘূৰাই nullptr; /* no match, and none possible after s */
                    }
                    যদি(c!=cq) {
                        ভঙ্গ; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        ঘূৰাই nullptr;
    }

    যদি(subLength<0) {
        subLength=u_strlen(sub);
    }
    যদি(subLength==0) {
        ঘূৰাই (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --subLength;
    subLimit=sub+subLength;

    যদি(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        ঘূৰাই length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    যদি(length<0) {
        /* s is NUL-terminated */
        যেতিয়ালৈ((c=*s++)!=0) {
            যদি(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                বাবে(;;) {
                    যদি(q==subLimit) {
                        যদি(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            ঘূৰাই (char16_t *)(s-1); /* well-formed match */
                        } অন্যথা {
                            ভঙ্গ; /* no match because surrogate pair is split */
                        }
                    }
                    যদি((c=*p)==0) {
                        ঘূৰাই nullptr; /* no match, and none possible after s */
                    }
                    যদি(c!=*q) {
                        ভঙ্গ; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } অন্যথা {
        ধ্রুৱ char16_t *limit, *preLimit;

        /* subLength was decremented above */
        যদি(length<=subLength) {
            ঘূৰাই nullptr; /* s is shorter than sub */
        }

        limit=s+length;

        /* the substring must start before preLimit */
        preLimit=limit-subLength;

        যেতিয়ালৈ(s!=preLimit) {
            c=*s++;
            যদি(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                বাবে(;;) {
                    যদি(q==subLimit) {
                        যদি(isMatchAtCPBoundary(start, s-1, p, limit)) {
                            ঘূৰাই (char16_t *)(s-1); /* well-formed match */
                        } অন্যথা {
                            ভঙ্গ; /* no match because surrogate pair is split */
                        }
                    }
                    যদি(*p!=*q) {
                        ভঙ্গ; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    ঘূৰাই nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(ধ্রুৱ char16_t* s U_LIFETIME_BOUND, ধ্রুৱ char16_t* substring) {
    ঘূৰাই u_strFindFirst(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(ধ্রুৱ char16_t* s U_LIFETIME_BOUND, char16_t c) {
    যদি(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ঘূৰাই u_strFindFirst(s, -1, &c, 1);
    } অন্যথা {
        char16_t cs;

        /* trivial search for a BMP code point */
        বাবে(;;) {
            যদি((cs=*s)==c) {
                ঘূৰাই (char16_t *)s;
            }
            যদি(cs==0) {
                ঘূৰাই nullptr;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(ধ্রুৱ char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    যদি((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ঘূৰাই u_strchr(s, (char16_t)c);
    } অন্যথা যদি((uint32_t)c<=UCHAR_MAX_মান) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        যেতিয়ালৈ((cs=*s++)!=0) {
            যদি(cs==lead && *s==trail) {
                ঘূৰাই (char16_t *)(s-1);
            }
        }
        ঘূৰাই nullptr;
    } অন্যথা {
        /* not a Unicode code point, not findable */
        ঘূৰাই nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(ধ্রুৱ char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    যদি(count<=0) {
        ঘূৰাই nullptr; /* no string */
    } অন্যথা যদি(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ঘূৰাই u_strFindFirst(s, count, &c, 1);
    } অন্যথা {
        /* trivial search for a BMP code point */
        ধ্রুৱ char16_t *limit=s+count;
        কৰা {
            যদি(*s==c) {
                ঘূৰাই (char16_t *)s;
            }
        } যেতিয়ালৈ(++s!=limit);
        ঘূৰাই nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(ধ্রুৱ char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    যদি((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ঘূৰাই u_memchr(s, (char16_t)c, count);
    } অন্যথা যদি(count<2) {
        /* too short for a surrogate pair */
        ঘূৰাই nullptr;
    } অন্যথা যদি((uint32_t)c<=UCHAR_MAX_মান) {
        /* find supplementary code point as surrogate pair */
        ধ্রুৱ char16_t *limit=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        কৰা {
            যদি(*s==lead && *(s+1)==trail) {
                ঘূৰাই (char16_t *)s;
            }
        } যেতিয়ালৈ(++s!=limit);
        ঘূৰাই nullptr;
    } অন্যথা {
        /* not a Unicode code point, not findable */
        ঘূৰাই nullptr;
    }
}

/* Backward binary string search functions ---------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strFindLast(ধ্রুৱ char16_t* s U_LIFETIME_BOUND,
              int32_t length,
              ধ্রুৱ char16_t* sub,
              int32_t subLength) {
    ধ্রুৱ char16_t *start, *limit, *p, *q, *subLimit;
    char16_t c, cs;

    যদি(sub==nullptr || subLength<-1) {
        ঘূৰাই (char16_t *)s;
    }
    যদি(s==nullptr || length<-1) {
        ঘূৰাই nullptr;
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

    যদি(subLength<0) {
        subLength=u_strlen(sub);
    }
    যদি(subLength==0) {
        ঘূৰাই (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    subLimit=sub+subLength;
    cs=*(--subLimit);
    --subLength;

    যদি(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        ঘূৰাই length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    যদি(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    যদি(length<=subLength) {
        ঘূৰাই nullptr; /* s is shorter than sub */
    }

    start=s;
    limit=s+length;

    /* the substring must start no later than s+subLength */
    s+=subLength;

    যেতিয়ালৈ(s!=limit) {
        c=*(--limit);
        যদি(c==cs) {
            /* found last substring char16_t, compare rest */
            p=limit;
            q=subLimit;
            বাবে(;;) {
                যদি(q==sub) {
                    যদি(isMatchAtCPBoundary(start, p, limit+1, start+length)) {
                        ঘূৰাই (char16_t *)p; /* well-formed match */
                    } অন্যথা {
                        ভঙ্গ; /* no match because surrogate pair is split */
                    }
                }
                যদি(*(--p)!=*(--q)) {
                    ভঙ্গ; /* no match */
                }
            }
        }
    }

    /* not found */
    ঘূৰাই nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(ধ্রুৱ char16_t* s U_LIFETIME_BOUND, ধ্রুৱ char16_t* substring) {
    ঘূৰাই u_strFindLast(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(ধ্রুৱ char16_t* s U_LIFETIME_BOUND, char16_t c) {
    যদি(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ঘূৰাই u_strFindLast(s, -1, &c, 1);
    } অন্যথা {
        ধ্রুৱ char16_t *result=nullptr;
        char16_t cs;

        /* trivial search for a BMP code point */
        বাবে(;;) {
            যদি((cs=*s)==c) {
                result=s;
            }
            যদি(cs==0) {
                ঘূৰাই (char16_t *)result;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(ধ্রুৱ char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    যদি((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ঘূৰাই u_strrchr(s, (char16_t)c);
    } অন্যথা যদি((uint32_t)c<=UCHAR_MAX_মান) {
        /* find supplementary code point as surrogate pair */
        ধ্রুৱ char16_t *result=nullptr;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        যেতিয়ালৈ((cs=*s++)!=0) {
            যদি(cs==lead && *s==trail) {
                result=s-1;
            }
        }
        ঘূৰাই (char16_t *)result;
    } অন্যথা {
        /* not a Unicode code point, not findable */
        ঘূৰাই nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(ধ্রুৱ char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    যদি(count<=0) {
        ঘূৰাই nullptr; /* no string */
    } অন্যথা যদি(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ঘূৰাই u_strFindLast(s, count, &c, 1);
    } অন্যথা {
        /* trivial search for a BMP code point */
        ধ্রুৱ char16_t *limit=s+count;
        কৰা {
            যদি(*(--limit)==c) {
                ঘূৰাই (char16_t *)limit;
            }
        } যেতিয়ালৈ(s!=limit);
        ঘূৰাই nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(ধ্রুৱ char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    যদি((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ঘূৰাই u_memrchr(s, (char16_t)c, count);
    } অন্যথা যদি(count<2) {
        /* too short for a surrogate pair */
        ঘূৰাই nullptr;
    } অন্যথা যদি((uint32_t)c<=UCHAR_MAX_মান) {
        /* find supplementary code point as surrogate pair */
        ধ্রুৱ char16_t *limit=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        কৰা {
            যদি(*limit==trail && *(limit-1)==lead) {
                ঘূৰাই (char16_t *)(limit-1);
            }
        } যেতিয়ালৈ(s!=--limit);
        ঘূৰাই nullptr;
    } অন্যথা {
        /* not a Unicode code point, not findable */
        ঘূৰাই nullptr;
    }
}

/* Tokenization functions --------------------------------------------------- */

/*
 * Match each code point in a string against each code point in the matchSet.
 * Return the index of the first string code point that
 * is (polarity==true) or is not (false) contained in the matchSet.
 * Return -(string length)-1 if there is no such code point.
 */
স্থির int32_t
_matchFromSet(ধ্রুৱ char16_t *string, ধ্রুৱ char16_t *matchSet, UBool polarity) {
    int32_t matchLen, matchBMPLen, strItr, matchItr;
    UChar32 stringCh, matchCh;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    matchBMPLen = 0;
    যেতিয়ালৈ((c = matchSet[matchBMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++matchBMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    matchLen = matchBMPLen;
    যেতিয়ালৈ(matchSet[matchLen] != 0) {
        ++matchLen;
    }

    বাবে(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        যদি(U16_IS_SINGLE(c)) {
            যদি(polarity) {
                বাবে(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    যদি(c == matchSet[matchItr]) {
                        ঘূৰাই strItr - 1; /* one matches */
                    }
                }
            } অন্যথা {
                বাবে(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    যদি(c == matchSet[matchItr]) {
                        গচ্ছ endloop;
                    }
                }
                ঘূৰাই strItr - 1; /* none matches */
            }
        } অন্যথা {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            যদি(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } অন্যথা {
                stringCh = c; /* unpaired trail surrogate */
            }

            যদি(polarity) {
                বাবে(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    যদি(stringCh == matchCh) {
                        ঘূৰাই strItr - U16_LENGTH(stringCh); /* one matches */
                    }
                }
            } অন্যথা {
                বাবে(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    যদি(stringCh == matchCh) {
                        গচ্ছ endloop;
                    }
                }
                ঘূৰাই strItr - U16_LENGTH(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    ঘূৰাই -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(ধ্রুৱ char16_t* string U_LIFETIME_BOUND, ধ্রুৱ char16_t* matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, সঁচা);
    যদি(idx >= 0) {
        ঘূৰাই (char16_t *)string + idx;
    } অন্যথা {
        ঘূৰাই nullptr;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(ধ্রুৱ char16_t *string, ধ্রুৱ char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, সঁচা);
    যদি(idx >= 0) {
        ঘূৰাই idx;
    } অন্যথা {
        ঘূৰাই -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(ধ্রুৱ char16_t *string, ধ্রুৱ char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, মিছা);
    যদি(idx >= 0) {
        ঘূৰাই idx;
    } অন্যথা {
        ঘূৰাই -idx - 1; /* == u_strlen(string) */
    }
}

/* ----- Text manipulation functions --- */

U_CAPI char16_t* U_EXPORT2
u_strtok_r(char16_t* src U_LIFETIME_BOUND, ধ্রুৱ char16_t* delim, char16_t** saveState)
{
    char16_t *tokSource;
    char16_t *nextToken;
    uint32_t nonDelimIdx;

    /* If saveState is nullptr, the user messed up. */
    যদি (src != nullptr) {
        tokSource = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    অন্যথা যদি (*saveState) {
        tokSource = *saveState;
    }
    অন্যথা {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        ঘূৰাই nullptr;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tokSource, delim);
    tokSource = &tokSource[nonDelimIdx];

    যদি (*tokSource) {
        nextToken = u_strpbrk(tokSource, delim);
        যদি (nextToken != nullptr) {
            /* Create a token */
            *(nextToken++) = 0;
            *saveState = nextToken;
            ঘূৰাই tokSource;
        }
        অন্যথা যদি (*saveState) {
            /* Return the last token */
            *saveState = nullptr;
            ঘূৰাই tokSource;
        }
    }
    অন্যথা {
        /* No tokens were found. Only delimiters were left. */
        *saveState = nullptr;
    }
    ঘূৰাই nullptr;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, ধ্রুৱ char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    যেতিয়ালৈ(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    যেতিয়ালৈ((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    ঘূৰাই anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, ধ্রুৱ char16_t* src, int32_t n)
{
    যদি(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        যেতিয়ালৈ(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        যেতিয়ালৈ((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            যদি(--n == 0) {
                *dst = 0;
                ভঙ্গ;
            }
            ++src;
        }

        ঘূৰাই anchor;
    } অন্যথা {
        ঘূৰাই dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(ধ্রুৱ char16_t *s1,
    ধ্রুৱ char16_t *s2)
{
    char16_t  c1, c2;

    বাবে(;;) {
        c1=*s1++;
        c2=*s2++;
        যদি (c1 != c2 || c1 == 0) {
            ভঙ্গ;
        }
    }
    ঘূৰাই (int32_t)c1 - (int32_t)c2;
}

U_CFUNC int32_t U_EXPORT2
uprv_strCompare(ধ্রুৱ char16_t *s1, int32_t length1,
                ধ্রুৱ char16_t *s2, int32_t length2,
                UBool strncmpStyle, UBool codePointOrder) {
    ধ্রুৱ char16_t *start1, *start2, *limit1, *limit2;
    char16_t c1, c2;

    /* setup for fix-up */
    start1=s1;
    start2=s2;

    /* compare identical prefixes - they do not need to be fixed up */
    যদি(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        যদি(s1==s2) {
            ঘূৰাই 0;
        }

        বাবে(;;) {
            c1=*s1;
            c2=*s2;
            যদি(c1!=c2) {
                ভঙ্গ;
            }
            যদি(c1==0) {
                ঘূৰাই 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=nullptr;
    } অন্যথা যদি(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        যদি(s1==s2) {
            ঘূৰাই 0;
        }

        limit1=start1+length1;

        বাবে(;;) {
            /* both lengths are same, check only one limit */
            যদি(s1==limit1) {
                ঘূৰাই 0;
            }

            c1=*s1;
            c2=*s2;
            যদি(c1!=c2) {
                ভঙ্গ;
            }
            যদি(c1==0) {
                ঘূৰাই 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } অন্যথা {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t lengthResult;

        যদি(length1<0) {
            length1=u_strlen(s1);
        }
        যদি(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        যদি(length1<length2) {
            lengthResult=-1;
            limit1=start1+length1;
        } অন্যথা যদি(length1==length2) {
            lengthResult=0;
            limit1=start1+length1;
        } অন্যথা /* length1>length2 */ {
            lengthResult=1;
            limit1=start1+length2;
        }

        যদি(s1==s2) {
            ঘূৰাই lengthResult;
        }

        বাবে(;;) {
            /* check pseudo-limit */
            যদি(s1==limit1) {
                ঘূৰাই lengthResult;
            }

            c1=*s1;
            c2=*s2;
            যদি(c1!=c2) {
                ভঙ্গ;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    যদি(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        যদি(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } অন্যথা {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        যদি(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } অন্যথা {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    ঘূৰাই (int32_t)c1-(int32_t)c2;
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
    যদি(iter1==nullptr || iter2==nullptr) {
        ঘূৰাই 0; /* bad arguments */
    }
    যদি(iter1==iter2) {
        ঘূৰাই 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_START);
    iter2->move(iter2, 0, UITER_START);

    /* compare identical prefixes - they do not need to be fixed up */
    বাবে(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        যদি(c1!=c2) {
            ভঙ্গ;
        }
        যদি(c1==-1) {
            ঘূৰাই 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    যদি(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        যদি(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->previous(iter1), U16_IS_LEAD(iter1->previous(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } অন্যথা {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        যদি(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->previous(iter2), U16_IS_LEAD(iter2->previous(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } অন্যথা {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    ঘূৰাই (int32_t)c1-(int32_t)c2;
}

#যদি 0
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
শূন্য fragment {
        /* iff a surrogate is part of a surrogate pair, leave >=d800 */
        যদি(c1<=0xdbff) {
            যদি(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } অন্যথা যদি(c1<=0xdfff) {
            int32_t idx=iter1->getIndex(iter1, UITER_CURRENT);
            iter1->previous(iter1); /* ==c1 */
            যদি(!U16_IS_LEAD(iter1->previous(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } অন্যথা /* 0xe000<=c1<=0xffff */ {
            /* BMP code point - make <d800 */
            c1-=0x2800;
        }
}
#endif

U_CAPI int32_t U_EXPORT2
u_strCompare(ধ্রুৱ char16_t *s1, int32_t length1,
             ধ্রুৱ char16_t *s2, int32_t length2,
             UBool codePointOrder) {
    /* argument checking */
    যদি(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        ঘূৰাই 0;
    }
    ঘূৰাই uprv_strCompare(s1, length1, s2, length2, মিছা, codePointOrder);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmpCodePointOrder(ধ্রুৱ char16_t *s1, ধ্রুৱ char16_t *s2) {
    ঘূৰাই uprv_strCompare(s1, -1, s2, -1, মিছা, সঁচা);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(ধ্রুৱ char16_t  *s1,
     ধ্রুৱ char16_t  *s2,
     int32_t     n) 
{
    যদি(n > 0) {
        int32_t rc;
        বাবে(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            যদি(rc != 0 || *s1 == 0 || --n == 0) {
                ঘূৰাই rc;
            }
            ++s1;
            ++s2;
        }
    } অন্যথা {
        ঘূৰাই 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmpCodePointOrder(ধ্রুৱ char16_t *s1, ধ্রুৱ char16_t *s2, int32_t n) {
    ঘূৰাই uprv_strCompare(s1, n, s2, n, সঁচা, সঁচা);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, ধ্রুৱ char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    যেতিয়ালৈ((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    ঘূৰাই anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, ধ্রুৱ char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    যেতিয়ালৈ(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    ঘূৰাই anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(ধ্রুৱ char16_t *s)
{
#যদি U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    ঘূৰাই (int32_t)uprv_wcslen((ধ্রুৱ wchar_t *)s);
#অন্যথা
    ধ্রুৱ char16_t *t = s;
    যেতিয়ালৈ(*t != 0) {
      ++t;
    }
    ঘূৰাই t - s;
#endif
}

U_CAPI int32_t U_EXPORT2
u_গুনতি_Char32(ধ্রুৱ char16_t *s, int32_t length) {
    int32_t count;

    যদি(s==nullptr || length<-1) {
        ঘূৰাই 0;
    }

    count=0;
    যদি(length>=0) {
        যেতিয়ালৈ(length>0) {
            ++count;
            যদি(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } অন্যথা {
                ++s;
                --length;
            }
        }
    } অন্যথা /* length==-1 */ {
        char16_t c;

        বাবে(;;) {
            যদি((c=*s++)==0) {
                ভঙ্গ;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            যদি(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    ঘূৰাই count;
}

U_CAPI UBool U_EXPORT2
u_strHasMoreChar32Than(ধ্রুৱ char16_t *s, int32_t length, int32_t number) {

    যদি(number<0) {
        ঘূৰাই সঁচা;
    }
    যদি(s==nullptr || length<-1) {
        ঘূৰাই মিছা;
    }

    যদি(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        বাবে(;;) {
            যদি((c=*s++)==0) {
                ঘূৰাই মিছা;
            }
            যদি(number==0) {
                ঘূৰাই সঁচা;
            }
            যদি(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } অন্যথা {
        /* length>=0 known */
        ধ্রুৱ char16_t *limit;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        যদি(((length+1)/2)>number) {
            ঘূৰাই সঁচা;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        যদি(maxSupplementary<=0) {
            ঘূৰাই মিছা;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        limit=s+length;
        বাবে(;;) {
            যদি(s==limit) {
                ঘূৰাই মিছা;
            }
            যদি(number==0) {
                ঘূৰাই সঁচা;
            }
            যদি(U16_IS_LEAD(*s++) && s!=limit && U16_IS_TRAIL(*s)) {
                ++s;
                যদি(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    ঘূৰাই মিছা;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, ধ্রুৱ char16_t* src, int32_t count) {
    যদি(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    ঘূৰাই dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, ধ্রুৱ char16_t* src, int32_t count) {
    যদি(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    ঘূৰাই dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    যদি(count > 0) {
        char16_t *ptr = dest;
        char16_t *limit = dest + count;

        যেতিয়ালৈ (ptr < limit) {
            *(ptr++) = c;
        }
    }
    ঘূৰাই dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(ধ্রুৱ char16_t *buf1, ধ্রুৱ char16_t *buf2, int32_t count) {
    যদি(count > 0) {
        ধ্রুৱ char16_t *limit = buf1 + count;
        int32_t result;

        যেতিয়ালৈ (buf1 < limit) {
            result = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            যদি (result != 0) {
                ঘূৰাই result;
            }
            buf1++;
            buf2++;
        }
    }
    ঘূৰাই 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmpCodePointOrder(ধ্রুৱ char16_t *s1, ধ্রুৱ char16_t *s2, int32_t count) {
    ঘূৰাই uprv_strCompare(s1, count, s2, count, মিছা, সঁচা);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
স্থির ধ্রুৱ char16_t UNESCAPE_MAP[] = {
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
গণনা { UNESCAPE_MAP_LENGTH = UPRV_LENGTHOF(UNESCAPE_MAP) };

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
স্থির int32_t _digit8(char16_t c) {
    যদি (c >= u'0' && c <= u'7') {
        ঘূৰাই c - u'0';
    }
    ঘূৰাই -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
স্থির int32_t _digit16(char16_t c) {
    যদি (c >= u'0' && c <= u'9') {
        ঘূৰাই c - u'0';
    }
    যদি (c >= u'A' && c <= u'F') {
        ঘূৰাই c - (u'A' - 10);
    }
    যদি (c >= u'a' && c <= u'f') {
        ঘূৰাই c - (u'a' - 10);
    }
    ঘূৰাই -1;
}

/* Parse a single escape sequence.  Although this method deals in
 * UChars, it does not use C++ or UnicodeString.  This allows it to
 * be used from C contexts. */
U_CAPI UChar32 U_EXPORT2
u_unescapeAt(UNESCAPE_CHAR_AT charAt,
             int32_t *offset,
             int32_t length,
             শূন্য *context) {

    int32_t start = *offset;
    UChar32 c;
    UChar32 result = 0;
    int8_t n = 0;
    int8_t minDig = 0;
    int8_t maxDig = 0;
    int8_t bitsPerDigit = 4; 
    int32_t dig;
    UBool braces = মিছা;

    /* Check that offset is in range */
    যদি (*offset < 0 || *offset >= length) {
        গচ্ছ err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*offset)++, context);

    /* Convert hexadecimal and octal escapes */
    ৱিকল্প (c) {
    পক্ষ u'u':
        minDig = maxDig = 4;
        ভঙ্গ;
    পক্ষ u'U':
        minDig = maxDig = 8;
        ভঙ্গ;
    পক্ষ u'x':
        minDig = 1;
        যদি (*offset < length && charAt(*offset, context) == u'{') {
            ++(*offset);
            braces = সঁচা;
            maxDig = 8;
        } অন্যথা {
            maxDig = 2;
        }
        ভঙ্গ;
    স্ৱভাৱ:
        dig = _digit8(c);
        যদি (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            bitsPerDigit = 3;
            result = dig;
        }
        ভঙ্গ;
    }
    যদি (minDig != 0) {
        যেতিয়ালৈ (*offset < length && n < maxDig) {
            c = charAt(*offset, context);
            dig = (bitsPerDigit == 3) ? _digit8(c) : _digit16(c);
            যদি (dig < 0) {
                ভঙ্গ;
            }
            result = (result << bitsPerDigit) | dig;
            ++(*offset);
            ++n;
        }
        যদি (n < minDig) {
            গচ্ছ err;
        }
        যদি (braces) {
            যদি (c != u'}') {
                গচ্ছ err;
            }
            ++(*offset);
        }
        যদি (result < 0 || result >= 0x110000) {
            গচ্ছ err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        যদি (*offset < length && U16_IS_LEAD(result)) {
            int32_t ahead = *offset + 1;
            c = charAt(*offset, context);
            যদি (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t tailLimit = ahead + 11;
                যদি (tailLimit > length) {
                    tailLimit = length;
                }
                c = u_unescapeAt(charAt, &ahead, tailLimit, context);
            }
            যদি (U16_IS_TRAIL(c)) {
                *offset = ahead;
                result = U16_GET_SUPPLEMENTARY(result, c);
            }
        }
        ঘূৰাই result;
    }

    /* Convert C-style escapes in table */
    বাবে (int32_t i=0; i<UNESCAPE_MAP_LENGTH; i+=2) {
        যদি (c == UNESCAPE_MAP[i]) {
            ঘূৰাই UNESCAPE_MAP[i+1];
        } অন্যথা যদি (c < UNESCAPE_MAP[i]) {
            ভঙ্গ;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    যদি (c == u'c' && *offset < length) {
        c = charAt((*offset)++, context);
        যদি (U16_IS_LEAD(c) && *offset < length) {
            char16_t c2 = charAt(*offset, context);
            যদি (U16_IS_TRAIL(c2)) {
                ++(*offset);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        ঘূৰাই 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    যদি (U16_IS_LEAD(c) && *offset < length) {
        char16_t c2 = charAt(*offset, context);
        যদি (U16_IS_TRAIL(c2)) {
            ++(*offset);
            ঘূৰাই U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    ঘূৰাই c;

 err:
    /* Invalid escape sequence */
    *offset = start; /* Reset to initial value */
    ঘূৰাই (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
স্থির char16_t U_CALLCONV
_charPtr_charAt(int32_t offset, শূন্য *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_charsToUChars(static_cast<ৱর্ণ*>(context) + offset, &c16, 1);
    ঘূৰাই c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
স্থির শূন্য _appendUChars(char16_t *dest, int32_t destCapacity,
                          ধ্রুৱ ৱর্ণ *src, int32_t srcLen) {
    যদি (destCapacity < 0) {
        destCapacity = 0;
    }
    যদি (srcLen > destCapacity) {
        srcLen = destCapacity;
    }
    u_charsToUChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(ধ্রুৱ ৱর্ণ *src, char16_t *dest, int32_t destCapacity) {
    ধ্রুৱ ৱর্ণ *segment = src;
    int32_t i = 0;
    ৱর্ণ c;

    যেতিয়ালৈ ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        যদি (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            যদি (src != segment) {
                যদি (dest != nullptr) {
                    _appendUChars(dest + i, destCapacity - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<ৱর্ণ*>(src));
            যদি (lenParsed == 0) {
                গচ্ছ err;
            }
            src += lenParsed; /* advance past escape seq. */
            যদি (dest != nullptr && U16_LENGTH(c32) <= (destCapacity - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } অন্যথা {
                i += U16_LENGTH(c32);
            }
            segment = src;
        } অন্যথা {
            ++src;
        }
    }
    যদি (src != segment) {
        যদি (dest != nullptr) {
            _appendUChars(dest + i, destCapacity - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    যদি (dest != nullptr && i < destCapacity) {
        dest[i] = 0;
    }
    ঘূৰাই i;

 err:
    যদি (dest != nullptr && destCapacity > 0) {
        *dest = 0;
    }
    ঘূৰাই 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#define __TERMINATE_STRING(dest, destCapacity, length, pErrorCode) UPRV_BLOCK_MACRO_BEGIN { \
    যদি(pErrorCode!=nullptr && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        যদি(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } অন্যথা যদি(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            যদি(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } অন্যথা যদি(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } অন্যথা /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;                        \
        }                                                               \
    } \
} UPRV_BLOCK_MACRO_END

U_CAPI char16_t U_EXPORT2
u_asciiToUpper(char16_t c) {
    যদি (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    ঘূৰাই c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ঘূৰাই length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(ৱর্ণ *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ঘূৰাই length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ঘূৰাই length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    ঘূৰাই length;
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
    ধ্রুৱ TYPE *p = (ধ্রুৱ TYPE*) STR;        \
    যদি (p != nullptr) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        ধ্রুৱ TYPE *limit = p + len;          \
        যেতিয়ালৈ (p<limit) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    ঘূৰাই static_cast<int32_t>(hash);        \
} UPRV_BLOCK_MACRO_END

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(ধ্রুৱ char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(ধ্রুৱ ৱর্ণ *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(ধ্রুৱ ৱর্ণ *str, int32_t length) {
    STRING_HASH(ৱর্ণ, str, length, (uint8_t)uprv_tolower(*p));
}
