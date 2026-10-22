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

#समावेश "unicode/utypes.h"
#समावेश "unicode/putil.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/utf16.h"
#समावेश "cstring.h"
#समावेश "cwchar.h"
#समावेश "cmemory.h"
#समावेश "ustr_imp.h"

/* ANSI string.h - style functions ------------------------------------------ */

/* U+ffff is the highest BMP code point, the highest one that fits into a 16-bit char16_t */
#घोषणा U_BMP_MAX 0xffff

/* Forward binary string search functions ----------------------------------- */

/*
 * Test if a substring match inside a string is at code point boundaries.
 * All pointers refer to the same buffer.
 * The limit pointer may be nullptr, all others must be real pointers.
 */
अटल अंतरभूत UBool
isMatchAtCPBoundary(स्थिर char16_t *start, स्थिर char16_t *match, स्थिर char16_t *matchLimit, स्थिर char16_t *limit) {
    अगर(U16_IS_TRAIL(*match) && start!=match && U16_IS_LEAD(*(match-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        वापस false;
    }
    अगर(U16_IS_LEAD(*(matchLimit-1)) && matchLimit!=limit && U16_IS_TRAIL(*matchLimit)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        वापस false;
    }
    वापस true;
}

U_CAPI char16_t* U_EXPORT2
u_strFindFirst(स्थिर char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               स्थिर char16_t* sub,
               int32_t sub_लंबाई) {
    स्थिर char16_t *start, *p, *q, *subLimit;
    char16_t c, cs, cq;

    अगर(sub==nullptr || sub_लंबाई<-1) {
        वापस (char16_t *)s;
    }
    अगर(s==nullptr || length<-1) {
        वापस nullptr;
    }

    start=s;

    अगर(length<0 && sub_लंबाई<0) {
        /* both strings are NUL-terminated */
        अगर((cs=*sub++)==0) {
            वापस (char16_t *)s;
        }
        अगर(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            वापस u_strchr(s, cs);
        }

        जबतक((c=*s++)!=0) {
            अगर(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                क्रम(;;) {
                    अगर((cq=*q)==0) {
                        अगर(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            वापस (char16_t *)(s-1); /* well-formed match */
                        } अन्यथा {
                            अवरोध; /* no match because surrogate pair is split */
                        }
                    }
                    अगर((c=*p)==0) {
                        वापस nullptr; /* no match, and none possible after s */
                    }
                    अगर(c!=cq) {
                        अवरोध; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        वापस nullptr;
    }

    अगर(sub_लंबाई<0) {
        sub_लंबाई=u_strlen(sub);
    }
    अगर(sub_लंबाई==0) {
        वापस (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --sub_लंबाई;
    subLimit=sub+sub_लंबाई;

    अगर(sub_लंबाई==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        वापस length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    अगर(length<0) {
        /* s is NUL-terminated */
        जबतक((c=*s++)!=0) {
            अगर(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                क्रम(;;) {
                    अगर(q==subLimit) {
                        अगर(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            वापस (char16_t *)(s-1); /* well-formed match */
                        } अन्यथा {
                            अवरोध; /* no match because surrogate pair is split */
                        }
                    }
                    अगर((c=*p)==0) {
                        वापस nullptr; /* no match, and none possible after s */
                    }
                    अगर(c!=*q) {
                        अवरोध; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } अन्यथा {
        स्थिर char16_t *limit, *preLimit;

        /* subLength was decremented above */
        अगर(length<=sub_लंबाई) {
            वापस nullptr; /* s is shorter than sub */
        }

        limit=s+length;

        /* the substring must start before preLimit */
        preLimit=limit-sub_लंबाई;

        जबतक(s!=preLimit) {
            c=*s++;
            अगर(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                क्रम(;;) {
                    अगर(q==subLimit) {
                        अगर(isMatchAtCPBoundary(start, s-1, p, limit)) {
                            वापस (char16_t *)(s-1); /* well-formed match */
                        } अन्यथा {
                            अवरोध; /* no match because surrogate pair is split */
                        }
                    }
                    अगर(*p!=*q) {
                        अवरोध; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    वापस nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(स्थिर char16_t* s U_LIFETIME_BOUND, स्थिर char16_t* substring) {
    वापस u_strFindFirst(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(स्थिर char16_t* s U_LIFETIME_BOUND, char16_t c) {
    अगर(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        वापस u_strFindFirst(s, -1, &c, 1);
    } अन्यथा {
        char16_t cs;

        /* trivial search for a BMP code point */
        क्रम(;;) {
            अगर((cs=*s)==c) {
                वापस (char16_t *)s;
            }
            अगर(cs==0) {
                वापस nullptr;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(स्थिर char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    अगर((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        वापस u_strchr(s, (char16_t)c);
    } अन्यथा अगर((uint32_t)c<=UCHAR_MAX_मूल्य) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        जबतक((cs=*s++)!=0) {
            अगर(cs==lead && *s==trail) {
                वापस (char16_t *)(s-1);
            }
        }
        वापस nullptr;
    } अन्यथा {
        /* not a Unicode code point, not findable */
        वापस nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(स्थिर char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    अगर(count<=0) {
        वापस nullptr; /* no string */
    } अन्यथा अगर(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        वापस u_strFindFirst(s, count, &c, 1);
    } अन्यथा {
        /* trivial search for a BMP code point */
        स्थिर char16_t *limit=s+count;
        करो {
            अगर(*s==c) {
                वापस (char16_t *)s;
            }
        } जबतक(++s!=limit);
        वापस nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(स्थिर char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    अगर((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        वापस u_memchr(s, (char16_t)c, count);
    } अन्यथा अगर(count<2) {
        /* too short for a surrogate pair */
        वापस nullptr;
    } अन्यथा अगर((uint32_t)c<=UCHAR_MAX_मूल्य) {
        /* find supplementary code point as surrogate pair */
        स्थिर char16_t *limit=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        करो {
            अगर(*s==lead && *(s+1)==trail) {
                वापस (char16_t *)s;
            }
        } जबतक(++s!=limit);
        वापस nullptr;
    } अन्यथा {
        /* not a Unicode code point, not findable */
        वापस nullptr;
    }
}

/* Backward binary string search functions ---------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strFindLast(स्थिर char16_t* s U_LIFETIME_BOUND,
              int32_t length,
              स्थिर char16_t* sub,
              int32_t sub_लंबाई) {
    स्थिर char16_t *start, *limit, *p, *q, *subLimit;
    char16_t c, cs;

    अगर(sub==nullptr || sub_लंबाई<-1) {
        वापस (char16_t *)s;
    }
    अगर(s==nullptr || length<-1) {
        वापस nullptr;
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

    अगर(sub_लंबाई<0) {
        sub_लंबाई=u_strlen(sub);
    }
    अगर(sub_लंबाई==0) {
        वापस (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    subLimit=sub+sub_लंबाई;
    cs=*(--subLimit);
    --sub_लंबाई;

    अगर(sub_लंबाई==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        वापस length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    अगर(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    अगर(length<=sub_लंबाई) {
        वापस nullptr; /* s is shorter than sub */
    }

    start=s;
    limit=s+length;

    /* the substring must start no later than s+subLength */
    s+=sub_लंबाई;

    जबतक(s!=limit) {
        c=*(--limit);
        अगर(c==cs) {
            /* found last substring char16_t, compare rest */
            p=limit;
            q=subLimit;
            क्रम(;;) {
                अगर(q==sub) {
                    अगर(isMatchAtCPBoundary(start, p, limit+1, start+length)) {
                        वापस (char16_t *)p; /* well-formed match */
                    } अन्यथा {
                        अवरोध; /* no match because surrogate pair is split */
                    }
                }
                अगर(*(--p)!=*(--q)) {
                    अवरोध; /* no match */
                }
            }
        }
    }

    /* not found */
    वापस nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(स्थिर char16_t* s U_LIFETIME_BOUND, स्थिर char16_t* substring) {
    वापस u_strFindLast(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(स्थिर char16_t* s U_LIFETIME_BOUND, char16_t c) {
    अगर(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        वापस u_strFindLast(s, -1, &c, 1);
    } अन्यथा {
        स्थिर char16_t *परिणाम=nullptr;
        char16_t cs;

        /* trivial search for a BMP code point */
        क्रम(;;) {
            अगर((cs=*s)==c) {
                परिणाम=s;
            }
            अगर(cs==0) {
                वापस (char16_t *)परिणाम;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(स्थिर char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    अगर((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        वापस u_strrchr(s, (char16_t)c);
    } अन्यथा अगर((uint32_t)c<=UCHAR_MAX_मूल्य) {
        /* find supplementary code point as surrogate pair */
        स्थिर char16_t *परिणाम=nullptr;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        जबतक((cs=*s++)!=0) {
            अगर(cs==lead && *s==trail) {
                परिणाम=s-1;
            }
        }
        वापस (char16_t *)परिणाम;
    } अन्यथा {
        /* not a Unicode code point, not findable */
        वापस nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(स्थिर char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    अगर(count<=0) {
        वापस nullptr; /* no string */
    } अन्यथा अगर(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        वापस u_strFindLast(s, count, &c, 1);
    } अन्यथा {
        /* trivial search for a BMP code point */
        स्थिर char16_t *limit=s+count;
        करो {
            अगर(*(--limit)==c) {
                वापस (char16_t *)limit;
            }
        } जबतक(s!=limit);
        वापस nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(स्थिर char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    अगर((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        वापस u_memrchr(s, (char16_t)c, count);
    } अन्यथा अगर(count<2) {
        /* too short for a surrogate pair */
        वापस nullptr;
    } अन्यथा अगर((uint32_t)c<=UCHAR_MAX_मूल्य) {
        /* find supplementary code point as surrogate pair */
        स्थिर char16_t *limit=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        करो {
            अगर(*limit==trail && *(limit-1)==lead) {
                वापस (char16_t *)(limit-1);
            }
        } जबतक(s!=--limit);
        वापस nullptr;
    } अन्यथा {
        /* not a Unicode code point, not findable */
        वापस nullptr;
    }
}

/* Tokenization functions --------------------------------------------------- */

/*
 * Match each code point in a string against each code point in the matchSet.
 * Return the index of the first string code point that
 * is (polarity==true) or is not (false) contained in the matchSet.
 * Return -(string length)-1 if there is no such code point.
 */
अटल int32_t
_matchFromSet(स्थिर char16_t *string, स्थिर char16_t *matchSet, UBool polarity) {
    int32_t matchLen, matchBMPLen, strItr, matchItr;
    UChar32 stringCh, matchCh;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    matchBMPLen = 0;
    जबतक((c = matchSet[matchBMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++matchBMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    matchLen = matchBMPLen;
    जबतक(matchSet[matchLen] != 0) {
        ++matchLen;
    }

    क्रम(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        अगर(U16_IS_SINGLE(c)) {
            अगर(polarity) {
                क्रम(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    अगर(c == matchSet[matchItr]) {
                        वापस strItr - 1; /* one matches */
                    }
                }
            } अन्यथा {
                क्रम(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    अगर(c == matchSet[matchItr]) {
                        जाओ endloop;
                    }
                }
                वापस strItr - 1; /* none matches */
            }
        } अन्यथा {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            अगर(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } अन्यथा {
                stringCh = c; /* unpaired trail surrogate */
            }

            अगर(polarity) {
                क्रम(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_अग्र(matchSet, matchItr, matchLen, matchCh);
                    अगर(stringCh == matchCh) {
                        वापस strItr - U16_लंबाई(stringCh); /* one matches */
                    }
                }
            } अन्यथा {
                क्रम(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_अग्र(matchSet, matchItr, matchLen, matchCh);
                    अगर(stringCh == matchCh) {
                        जाओ endloop;
                    }
                }
                वापस strItr - U16_लंबाई(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    वापस -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(स्थिर char16_t* string U_LIFETIME_BOUND, स्थिर char16_t* matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, true);
    अगर(idx >= 0) {
        वापस (char16_t *)string + idx;
    } अन्यथा {
        वापस nullptr;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(स्थिर char16_t *string, स्थिर char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, true);
    अगर(idx >= 0) {
        वापस idx;
    } अन्यथा {
        वापस -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(स्थिर char16_t *string, स्थिर char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, false);
    अगर(idx >= 0) {
        वापस idx;
    } अन्यथा {
        वापस -idx - 1; /* == u_strlen(string) */
    }
}

/* ----- Text manipulation functions --- */

U_CAPI char16_t* U_EXPORT2
u_strtok_r(char16_t* src U_LIFETIME_BOUND, स्थिर char16_t* delim, char16_t** saveState)
{
    char16_t *tokSource;
    char16_t *अग्र_Token;
    uint32_t nonDelimIdx;

    /* If saveState is nullptr, the user messed up. */
    अगर (src != nullptr) {
        tokSource = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    अन्यथा अगर (*saveState) {
        tokSource = *saveState;
    }
    अन्यथा {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        वापस nullptr;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tokSource, delim);
    tokSource = &tokSource[nonDelimIdx];

    अगर (*tokSource) {
        अग्र_Token = u_strpbrk(tokSource, delim);
        अगर (अग्र_Token != nullptr) {
            /* Create a token */
            *(अग्र_Token++) = 0;
            *saveState = अग्र_Token;
            वापस tokSource;
        }
        अन्यथा अगर (*saveState) {
            /* Return the last token */
            *saveState = nullptr;
            वापस tokSource;
        }
    }
    अन्यथा {
        /* No tokens were found. Only delimiters were left. */
        *saveState = nullptr;
    }
    वापस nullptr;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, स्थिर char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    जबतक(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    जबतक((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    वापस anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, स्थिर char16_t* src, int32_t n)
{
    अगर(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        जबतक(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        जबतक((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            अगर(--n == 0) {
                *dst = 0;
                अवरोध;
            }
            ++src;
        }

        वापस anchor;
    } अन्यथा {
        वापस dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(स्थिर char16_t *s1,
    स्थिर char16_t *s2)
{
    char16_t  c1, c2;

    क्रम(;;) {
        c1=*s1++;
        c2=*s2++;
        अगर (c1 != c2 || c1 == 0) {
            अवरोध;
        }
    }
    वापस (int32_t)c1 - (int32_t)c2;
}

U_CFUNC int32_t U_EXPORT2
uprv_strCompare(स्थिर char16_t *s1, int32_t length1,
                स्थिर char16_t *s2, int32_t length2,
                UBool strncmpStyle, UBool codePointOrder) {
    स्थिर char16_t *start1, *start2, *limit1, *limit2;
    char16_t c1, c2;

    /* setup for fix-up */
    start1=s1;
    start2=s2;

    /* compare identical prefixes - they do not need to be fixed up */
    अगर(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        अगर(s1==s2) {
            वापस 0;
        }

        क्रम(;;) {
            c1=*s1;
            c2=*s2;
            अगर(c1!=c2) {
                अवरोध;
            }
            अगर(c1==0) {
                वापस 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=nullptr;
    } अन्यथा अगर(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        अगर(s1==s2) {
            वापस 0;
        }

        limit1=start1+length1;

        क्रम(;;) {
            /* both lengths are same, check only one limit */
            अगर(s1==limit1) {
                वापस 0;
            }

            c1=*s1;
            c2=*s2;
            अगर(c1!=c2) {
                अवरोध;
            }
            अगर(c1==0) {
                वापस 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } अन्यथा {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t लंबाई_परिणाम;

        अगर(length1<0) {
            length1=u_strlen(s1);
        }
        अगर(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        अगर(length1<length2) {
            लंबाई_परिणाम=-1;
            limit1=start1+length1;
        } अन्यथा अगर(length1==length2) {
            लंबाई_परिणाम=0;
            limit1=start1+length1;
        } अन्यथा /* length1>length2 */ {
            लंबाई_परिणाम=1;
            limit1=start1+length2;
        }

        अगर(s1==s2) {
            वापस लंबाई_परिणाम;
        }

        क्रम(;;) {
            /* check pseudo-limit */
            अगर(s1==limit1) {
                वापस लंबाई_परिणाम;
            }

            c1=*s1;
            c2=*s2;
            अगर(c1!=c2) {
                अवरोध;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    अगर(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        अगर(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } अन्यथा {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        अगर(
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
    वापस (int32_t)c1-(int32_t)c2;
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
    अगर(iter1==nullptr || iter2==nullptr) {
        वापस 0; /* bad arguments */
    }
    अगर(iter1==iter2) {
        वापस 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_START);
    iter2->move(iter2, 0, UITER_START);

    /* compare identical prefixes - they do not need to be fixed up */
    क्रम(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        अगर(c1!=c2) {
            अवरोध;
        }
        अगर(c1==-1) {
            वापस 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    अगर(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        अगर(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->previous(iter1), U16_IS_LEAD(iter1->previous(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } अन्यथा {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        अगर(
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
    वापस (int32_t)c1-(int32_t)c2;
}

#अगर 0
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
व्योम fragment {
        /* iff a surrogate is part of a surrogate pair, leave >=d800 */
        अगर(c1<=0xdbff) {
            अगर(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } अन्यथा अगर(c1<=0xdfff) {
            int32_t idx=iter1->get_सूचकांक(iter1, UITER_CURRENT);
            iter1->previous(iter1); /* ==c1 */
            अगर(!U16_IS_LEAD(iter1->previous(iter1))) {
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
#पूर्ण_अगर

U_CAPI int32_t U_EXPORT2
u_strCompare(स्थिर char16_t *s1, int32_t length1,
             स्थिर char16_t *s2, int32_t length2,
             UBool codePointOrder) {
    /* argument checking */
    अगर(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        वापस 0;
    }
    वापस uprv_strCompare(s1, length1, s2, length2, false, codePointOrder);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmpCodePointOrder(स्थिर char16_t *s1, स्थिर char16_t *s2) {
    वापस uprv_strCompare(s1, -1, s2, -1, false, true);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(स्थिर char16_t  *s1,
     स्थिर char16_t  *s2,
     int32_t     n) 
{
    अगर(n > 0) {
        int32_t rc;
        क्रम(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            अगर(rc != 0 || *s1 == 0 || --n == 0) {
                वापस rc;
            }
            ++s1;
            ++s2;
        }
    } अन्यथा {
        वापस 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmpCodePointOrder(स्थिर char16_t *s1, स्थिर char16_t *s2, int32_t n) {
    वापस uprv_strCompare(s1, n, s2, n, true, true);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, स्थिर char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    जबतक((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    वापस anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, स्थिर char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    जबतक(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    वापस anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(स्थिर char16_t *s)
{
#अगर U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    वापस (int32_t)uprv_wcslen((स्थिर ब_अक्षर_प्रकार *)s);
#अन्यथा
    स्थिर char16_t *t = s;
    जबतक(*t != 0) {
      ++t;
    }
    वापस t - s;
#पूर्ण_अगर
}

U_CAPI int32_t U_EXPORT2
u_गिनती_Char32(स्थिर char16_t *s, int32_t length) {
    int32_t count;

    अगर(s==nullptr || length<-1) {
        वापस 0;
    }

    count=0;
    अगर(length>=0) {
        जबतक(length>0) {
            ++count;
            अगर(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } अन्यथा {
                ++s;
                --length;
            }
        }
    } अन्यथा /* length==-1 */ {
        char16_t c;

        क्रम(;;) {
            अगर((c=*s++)==0) {
                अवरोध;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            अगर(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    वापस count;
}

U_CAPI UBool U_EXPORT2
u_strHasMoreChar32Than(स्थिर char16_t *s, int32_t length, int32_t number) {

    अगर(number<0) {
        वापस true;
    }
    अगर(s==nullptr || length<-1) {
        वापस false;
    }

    अगर(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        क्रम(;;) {
            अगर((c=*s++)==0) {
                वापस false;
            }
            अगर(number==0) {
                वापस true;
            }
            अगर(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } अन्यथा {
        /* length>=0 known */
        स्थिर char16_t *limit;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        अगर(((length+1)/2)>number) {
            वापस true;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        अगर(maxSupplementary<=0) {
            वापस false;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        limit=s+length;
        क्रम(;;) {
            अगर(s==limit) {
                वापस false;
            }
            अगर(number==0) {
                वापस true;
            }
            अगर(U16_IS_LEAD(*s++) && s!=limit && U16_IS_TRAIL(*s)) {
                ++s;
                अगर(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    वापस false;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, स्थिर char16_t* src, int32_t count) {
    अगर(count > 0) {
        uprv_memcpy(dest, src, (माप_प्रकार)count*U_SIZEOF_UCHAR);
    }
    वापस dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, स्थिर char16_t* src, int32_t count) {
    अगर(count > 0) {
        uprv_memmove(dest, src, (माप_प्रकार)count*U_SIZEOF_UCHAR);
    }
    वापस dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    अगर(count > 0) {
        char16_t *ptr = dest;
        char16_t *limit = dest + count;

        जबतक (ptr < limit) {
            *(ptr++) = c;
        }
    }
    वापस dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(स्थिर char16_t *buf1, स्थिर char16_t *buf2, int32_t count) {
    अगर(count > 0) {
        स्थिर char16_t *limit = buf1 + count;
        int32_t परिणाम;

        जबतक (buf1 < limit) {
            परिणाम = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            अगर (परिणाम != 0) {
                वापस परिणाम;
            }
            buf1++;
            buf2++;
        }
    }
    वापस 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmpCodePointOrder(स्थिर char16_t *s1, स्थिर char16_t *s2, int32_t count) {
    वापस uprv_strCompare(s1, count, s2, count, false, true);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
अटल स्थिर char16_t UNESCAPE_MAP[] = {
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
क्रमागत { UNESCAPE_MAP_लंबाई = UPRV_LENGTHOF(UNESCAPE_MAP) };

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
अटल int32_t _digit8(char16_t c) {
    अगर (c >= u'0' && c <= u'7') {
        वापस c - u'0';
    }
    वापस -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
अटल int32_t _digit16(char16_t c) {
    अगर (c >= u'0' && c <= u'9') {
        वापस c - u'0';
    }
    अगर (c >= u'A' && c <= u'F') {
        वापस c - (u'A' - 10);
    }
    अगर (c >= u'a' && c <= u'f') {
        वापस c - (u'a' - 10);
    }
    वापस -1;
}

/* Parse a single escape sequence.  Although this method deals in
 * UChars, it does not use C++ or UnicodeString.  This allows it to
 * be used from C contexts. */
U_CAPI UChar32 U_EXPORT2
u_unescapeAt(UNESCAPE_CHAR_AT charAt,
             int32_t *offset,
             int32_t length,
             व्योम *context) {

    int32_t start = *offset;
    UChar32 c;
    UChar32 परिणाम = 0;
    int8_t n = 0;
    int8_t minDig = 0;
    int8_t maxDig = 0;
    int8_t bitsPerDigit = 4; 
    int32_t dig;
    UBool braces = false;

    /* Check that offset is in range */
    अगर (*offset < 0 || *offset >= length) {
        जाओ err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*offset)++, context);

    /* Convert hexadecimal and octal escapes */
    चयन (c) {
    हाल u'u':
        minDig = maxDig = 4;
        अवरोध;
    हाल u'U':
        minDig = maxDig = 8;
        अवरोध;
    हाल u'x':
        minDig = 1;
        अगर (*offset < length && charAt(*offset, context) == u'{') {
            ++(*offset);
            braces = true;
            maxDig = 8;
        } अन्यथा {
            maxDig = 2;
        }
        अवरोध;
    शेष:
        dig = _digit8(c);
        अगर (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            bitsPerDigit = 3;
            परिणाम = dig;
        }
        अवरोध;
    }
    अगर (minDig != 0) {
        जबतक (*offset < length && n < maxDig) {
            c = charAt(*offset, context);
            dig = (bitsPerDigit == 3) ? _digit8(c) : _digit16(c);
            अगर (dig < 0) {
                अवरोध;
            }
            परिणाम = (परिणाम << bitsPerDigit) | dig;
            ++(*offset);
            ++n;
        }
        अगर (n < minDig) {
            जाओ err;
        }
        अगर (braces) {
            अगर (c != u'}') {
                जाओ err;
            }
            ++(*offset);
        }
        अगर (परिणाम < 0 || परिणाम >= 0x110000) {
            जाओ err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        अगर (*offset < length && U16_IS_LEAD(परिणाम)) {
            int32_t ahead = *offset + 1;
            c = charAt(*offset, context);
            अगर (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t पुच्छ_Limit = ahead + 11;
                अगर (पुच्छ_Limit > length) {
                    पुच्छ_Limit = length;
                }
                c = u_unescapeAt(charAt, &ahead, पुच्छ_Limit, context);
            }
            अगर (U16_IS_TRAIL(c)) {
                *offset = ahead;
                परिणाम = U16_GET_SUPPLEMENTARY(परिणाम, c);
            }
        }
        वापस परिणाम;
    }

    /* Convert C-style escapes in table */
    क्रम (int32_t i=0; i<UNESCAPE_MAP_लंबाई; i+=2) {
        अगर (c == UNESCAPE_MAP[i]) {
            वापस UNESCAPE_MAP[i+1];
        } अन्यथा अगर (c < UNESCAPE_MAP[i]) {
            अवरोध;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    अगर (c == u'c' && *offset < length) {
        c = charAt((*offset)++, context);
        अगर (U16_IS_LEAD(c) && *offset < length) {
            char16_t c2 = charAt(*offset, context);
            अगर (U16_IS_TRAIL(c2)) {
                ++(*offset);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        वापस 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    अगर (U16_IS_LEAD(c) && *offset < length) {
        char16_t c2 = charAt(*offset, context);
        अगर (U16_IS_TRAIL(c2)) {
            ++(*offset);
            वापस U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    वापस c;

 err:
    /* Invalid escape sequence */
    *offset = start; /* Reset to initial value */
    वापस (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
अटल char16_t U_CALLCONV
_charPtr_charAt(int32_t offset, व्योम *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_charsToUChars(static_cast<अक्षर*>(context) + offset, &c16, 1);
    वापस c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
अटल व्योम _appendUChars(char16_t *dest, int32_t destCapacity,
                          स्थिर अक्षर *src, int32_t srcLen) {
    अगर (destCapacity < 0) {
        destCapacity = 0;
    }
    अगर (srcLen > destCapacity) {
        srcLen = destCapacity;
    }
    u_charsToUChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(स्थिर अक्षर *src, char16_t *dest, int32_t destCapacity) {
    स्थिर अक्षर *segment = src;
    int32_t i = 0;
    अक्षर c;

    जबतक ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        अगर (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            अगर (src != segment) {
                अगर (dest != nullptr) {
                    _appendUChars(dest + i, destCapacity - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<अक्षर*>(src));
            अगर (lenParsed == 0) {
                जाओ err;
            }
            src += lenParsed; /* advance past escape seq. */
            अगर (dest != nullptr && U16_लंबाई(c32) <= (destCapacity - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } अन्यथा {
                i += U16_लंबाई(c32);
            }
            segment = src;
        } अन्यथा {
            ++src;
        }
    }
    अगर (src != segment) {
        अगर (dest != nullptr) {
            _appendUChars(dest + i, destCapacity - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    अगर (dest != nullptr && i < destCapacity) {
        dest[i] = 0;
    }
    वापस i;

 err:
    अगर (dest != nullptr && destCapacity > 0) {
        *dest = 0;
    }
    वापस 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#घोषणा __TERMINATE_STRING(dest, destCapacity, length, pErrorCode) UPRV_BLOCK_MACRO_BEGIN { \
    अगर(pErrorCode!=nullptr && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        अगर(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } अन्यथा अगर(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            अगर(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } अन्यथा अगर(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } अन्यथा /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_बफर_OVERFLOW_ERROR;                        \
        }                                                               \
    } \
} UPRV_BLOCK_MACRO_END

U_CAPI char16_t U_EXPORT2
u_asciiToUpper(char16_t c) {
    अगर (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    वापस c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    वापस length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(अक्षर *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    वापस length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    वापस length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(ब_अक्षर_प्रकार *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    वापस length;
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

#घोषणा STRING_HASH(TYPE, STR, STRLEN, DEREF) UPRV_BLOCK_MACRO_BEGIN { \
    uint32_t hash = 0;                        \
    स्थिर TYPE *p = (स्थिर TYPE*) STR;        \
    अगर (p != nullptr) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        स्थिर TYPE *limit = p + len;          \
        जबतक (p<limit) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    वापस static_cast<int32_t>(hash);        \
} UPRV_BLOCK_MACRO_END

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(स्थिर char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(स्थिर अक्षर *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(स्थिर अक्षर *str, int32_t length) {
    STRING_HASH(अक्षर, str, length, (uint8_t)uprv_tolower(*p));
}
