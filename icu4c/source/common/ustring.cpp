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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "unicode/uchar.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "cwchar.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "ustr_imp.h"

/* ANSI string.h - style functions ------------------------------------------ */

/* U+ffff is the highest BMP code point, the highest one that fits into a 16-bit char16_t */
#define U_BMP_MAX 0xffff

/* Forward binary string search functions ----------------------------------- */

/*
 * Test if a substring match inside a string is at code point boundaries.
 * All pointers refer to the same buffer.
 * The limit pointer may be nullptr, all others must be real pointers.
 */
நிலையான உள்வரி UBool
isMatchAtCPBoundary(மாறிலி char16_t *start, மாறிலி char16_t *match, மாறிலி char16_t *matchLimit, மாறிலி char16_t *limit) {
    எனில்(U16_IS_TRAIL(*match) && start!=match && U16_IS_LEAD(*(match-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        திரும்பு பொய்;
    }
    எனில்(U16_IS_LEAD(*(matchLimit-1)) && matchLimit!=limit && U16_IS_TRAIL(*matchLimit)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        திரும்பு பொய்;
    }
    திரும்பு உண்மை;
}

U_CAPI char16_t* U_EXPORT2
u_strFindFirst(மாறிலி char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               மாறிலி char16_t* sub,
               int32_t subLength) {
    மாறிலி char16_t *start, *p, *q, *subLimit;
    char16_t c, cs, cq;

    எனில்(sub==nullptr || subLength<-1) {
        திரும்பு (char16_t *)s;
    }
    எனில்(s==nullptr || length<-1) {
        திரும்பு nullptr;
    }

    start=s;

    எனில்(length<0 && subLength<0) {
        /* both strings are NUL-terminated */
        எனில்((cs=*sub++)==0) {
            திரும்பு (char16_t *)s;
        }
        எனில்(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            திரும்பு u_strchr(s, cs);
        }

        போது((c=*s++)!=0) {
            எனில்(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                க்கு(;;) {
                    எனில்((cq=*q)==0) {
                        எனில்(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            திரும்பு (char16_t *)(s-1); /* well-formed match */
                        } இல்லையெனில் {
                            நிறுத்து; /* no match because surrogate pair is split */
                        }
                    }
                    எனில்((c=*p)==0) {
                        திரும்பு nullptr; /* no match, and none possible after s */
                    }
                    எனில்(c!=cq) {
                        நிறுத்து; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        திரும்பு nullptr;
    }

    எனில்(subLength<0) {
        subLength=u_strlen(sub);
    }
    எனில்(subLength==0) {
        திரும்பு (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --subLength;
    subLimit=sub+subLength;

    எனில்(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        திரும்பு length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    எனில்(length<0) {
        /* s is NUL-terminated */
        போது((c=*s++)!=0) {
            எனில்(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                க்கு(;;) {
                    எனில்(q==subLimit) {
                        எனில்(isMatchAtCPBoundary(start, s-1, p, nullptr)) {
                            திரும்பு (char16_t *)(s-1); /* well-formed match */
                        } இல்லையெனில் {
                            நிறுத்து; /* no match because surrogate pair is split */
                        }
                    }
                    எனில்((c=*p)==0) {
                        திரும்பு nullptr; /* no match, and none possible after s */
                    }
                    எனில்(c!=*q) {
                        நிறுத்து; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } இல்லையெனில் {
        மாறிலி char16_t *limit, *preLimit;

        /* subLength was decremented above */
        எனில்(length<=subLength) {
            திரும்பு nullptr; /* s is shorter than sub */
        }

        limit=s+length;

        /* the substring must start before preLimit */
        preLimit=limit-subLength;

        போது(s!=preLimit) {
            c=*s++;
            எனில்(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                க்கு(;;) {
                    எனில்(q==subLimit) {
                        எனில்(isMatchAtCPBoundary(start, s-1, p, limit)) {
                            திரும்பு (char16_t *)(s-1); /* well-formed match */
                        } இல்லையெனில் {
                            நிறுத்து; /* no match because surrogate pair is split */
                        }
                    }
                    எனில்(*p!=*q) {
                        நிறுத்து; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    திரும்பு nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(மாறிலி char16_t* s U_LIFETIME_BOUND, மாறிலி char16_t* substring) {
    திரும்பு u_strFindFirst(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(மாறிலி char16_t* s U_LIFETIME_BOUND, char16_t c) {
    எனில்(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        திரும்பு u_strFindFirst(s, -1, &c, 1);
    } இல்லையெனில் {
        char16_t cs;

        /* trivial search for a BMP code point */
        க்கு(;;) {
            எனில்((cs=*s)==c) {
                திரும்பு (char16_t *)s;
            }
            எனில்(cs==0) {
                திரும்பு nullptr;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(மாறிலி char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    எனில்((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        திரும்பு u_strchr(s, (char16_t)c);
    } இல்லையெனில் எனில்((uint32_t)c<=UCHAR_MAX_மதிப்பு) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        போது((cs=*s++)!=0) {
            எனில்(cs==lead && *s==trail) {
                திரும்பு (char16_t *)(s-1);
            }
        }
        திரும்பு nullptr;
    } இல்லையெனில் {
        /* not a Unicode code point, not findable */
        திரும்பு nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(மாறிலி char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    எனில்(count<=0) {
        திரும்பு nullptr; /* no string */
    } இல்லையெனில் எனில்(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        திரும்பு u_strFindFirst(s, count, &c, 1);
    } இல்லையெனில் {
        /* trivial search for a BMP code point */
        மாறிலி char16_t *limit=s+count;
        செய் {
            எனில்(*s==c) {
                திரும்பு (char16_t *)s;
            }
        } போது(++s!=limit);
        திரும்பு nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(மாறிலி char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    எனில்((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        திரும்பு u_memchr(s, (char16_t)c, count);
    } இல்லையெனில் எனில்(count<2) {
        /* too short for a surrogate pair */
        திரும்பு nullptr;
    } இல்லையெனில் எனில்((uint32_t)c<=UCHAR_MAX_மதிப்பு) {
        /* find supplementary code point as surrogate pair */
        மாறிலி char16_t *limit=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        செய் {
            எனில்(*s==lead && *(s+1)==trail) {
                திரும்பு (char16_t *)s;
            }
        } போது(++s!=limit);
        திரும்பு nullptr;
    } இல்லையெனில் {
        /* not a Unicode code point, not findable */
        திரும்பு nullptr;
    }
}

/* Backward binary string search functions ---------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strFindLast(மாறிலி char16_t* s U_LIFETIME_BOUND,
              int32_t length,
              மாறிலி char16_t* sub,
              int32_t subLength) {
    மாறிலி char16_t *start, *limit, *p, *q, *subLimit;
    char16_t c, cs;

    எனில்(sub==nullptr || subLength<-1) {
        திரும்பு (char16_t *)s;
    }
    எனில்(s==nullptr || length<-1) {
        திரும்பு nullptr;
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

    எனில்(subLength<0) {
        subLength=u_strlen(sub);
    }
    எனில்(subLength==0) {
        திரும்பு (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    subLimit=sub+subLength;
    cs=*(--subLimit);
    --subLength;

    எனில்(subLength==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        திரும்பு length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    எனில்(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    எனில்(length<=subLength) {
        திரும்பு nullptr; /* s is shorter than sub */
    }

    start=s;
    limit=s+length;

    /* the substring must start no later than s+subLength */
    s+=subLength;

    போது(s!=limit) {
        c=*(--limit);
        எனில்(c==cs) {
            /* found last substring char16_t, compare rest */
            p=limit;
            q=subLimit;
            க்கு(;;) {
                எனில்(q==sub) {
                    எனில்(isMatchAtCPBoundary(start, p, limit+1, start+length)) {
                        திரும்பு (char16_t *)p; /* well-formed match */
                    } இல்லையெனில் {
                        நிறுத்து; /* no match because surrogate pair is split */
                    }
                }
                எனில்(*(--p)!=*(--q)) {
                    நிறுத்து; /* no match */
                }
            }
        }
    }

    /* not found */
    திரும்பு nullptr;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(மாறிலி char16_t* s U_LIFETIME_BOUND, மாறிலி char16_t* substring) {
    திரும்பு u_strFindLast(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(மாறிலி char16_t* s U_LIFETIME_BOUND, char16_t c) {
    எனில்(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        திரும்பு u_strFindLast(s, -1, &c, 1);
    } இல்லையெனில் {
        மாறிலி char16_t *result=nullptr;
        char16_t cs;

        /* trivial search for a BMP code point */
        க்கு(;;) {
            எனில்((cs=*s)==c) {
                result=s;
            }
            எனில்(cs==0) {
                திரும்பு (char16_t *)result;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(மாறிலி char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    எனில்((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        திரும்பு u_strrchr(s, (char16_t)c);
    } இல்லையெனில் எனில்((uint32_t)c<=UCHAR_MAX_மதிப்பு) {
        /* find supplementary code point as surrogate pair */
        மாறிலி char16_t *result=nullptr;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        போது((cs=*s++)!=0) {
            எனில்(cs==lead && *s==trail) {
                result=s-1;
            }
        }
        திரும்பு (char16_t *)result;
    } இல்லையெனில் {
        /* not a Unicode code point, not findable */
        திரும்பு nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(மாறிலி char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    எனில்(count<=0) {
        திரும்பு nullptr; /* no string */
    } இல்லையெனில் எனில்(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        திரும்பு u_strFindLast(s, count, &c, 1);
    } இல்லையெனில் {
        /* trivial search for a BMP code point */
        மாறிலி char16_t *limit=s+count;
        செய் {
            எனில்(*(--limit)==c) {
                திரும்பு (char16_t *)limit;
            }
        } போது(s!=limit);
        திரும்பு nullptr;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(மாறிலி char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    எனில்((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        திரும்பு u_memrchr(s, (char16_t)c, count);
    } இல்லையெனில் எனில்(count<2) {
        /* too short for a surrogate pair */
        திரும்பு nullptr;
    } இல்லையெனில் எனில்((uint32_t)c<=UCHAR_MAX_மதிப்பு) {
        /* find supplementary code point as surrogate pair */
        மாறிலி char16_t *limit=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        செய் {
            எனில்(*limit==trail && *(limit-1)==lead) {
                திரும்பு (char16_t *)(limit-1);
            }
        } போது(s!=--limit);
        திரும்பு nullptr;
    } இல்லையெனில் {
        /* not a Unicode code point, not findable */
        திரும்பு nullptr;
    }
}

/* Tokenization functions --------------------------------------------------- */

/*
 * Match each code point in a string against each code point in the matchSet.
 * Return the index of the first string code point that
 * is (polarity==true) or is not (false) contained in the matchSet.
 * Return -(string length)-1 if there is no such code point.
 */
நிலையான int32_t
_matchFromSet(மாறிலி char16_t *string, மாறிலி char16_t *matchSet, UBool polarity) {
    int32_t matchLen, matchBMPLen, strItr, matchItr;
    UChar32 stringCh, matchCh;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    matchBMPLen = 0;
    போது((c = matchSet[matchBMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++matchBMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    matchLen = matchBMPLen;
    போது(matchSet[matchLen] != 0) {
        ++matchLen;
    }

    க்கு(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        எனில்(U16_IS_SINGLE(c)) {
            எனில்(polarity) {
                க்கு(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    எனில்(c == matchSet[matchItr]) {
                        திரும்பு strItr - 1; /* one matches */
                    }
                }
            } இல்லையெனில் {
                க்கு(matchItr = 0; matchItr < matchLen; ++matchItr) {
                    எனில்(c == matchSet[matchItr]) {
                        செல் endloop;
                    }
                }
                திரும்பு strItr - 1; /* none matches */
            }
        } இல்லையெனில் {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            எனில்(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } இல்லையெனில் {
                stringCh = c; /* unpaired trail surrogate */
            }

            எனில்(polarity) {
                க்கு(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    எனில்(stringCh == matchCh) {
                        திரும்பு strItr - U16_LENGTH(stringCh); /* one matches */
                    }
                }
            } இல்லையெனில் {
                க்கு(matchItr = matchBMPLen; matchItr < matchLen;) {
                    U16_NEXT(matchSet, matchItr, matchLen, matchCh);
                    எனில்(stringCh == matchCh) {
                        செல் endloop;
                    }
                }
                திரும்பு strItr - U16_LENGTH(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    திரும்பு -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(மாறிலி char16_t* string U_LIFETIME_BOUND, மாறிலி char16_t* matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, உண்மை);
    எனில்(idx >= 0) {
        திரும்பு (char16_t *)string + idx;
    } இல்லையெனில் {
        திரும்பு nullptr;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(மாறிலி char16_t *string, மாறிலி char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, உண்மை);
    எனில்(idx >= 0) {
        திரும்பு idx;
    } இல்லையெனில் {
        திரும்பு -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(மாறிலி char16_t *string, மாறிலி char16_t *matchSet)
{
    int32_t idx = _matchFromSet(string, matchSet, பொய்);
    எனில்(idx >= 0) {
        திரும்பு idx;
    } இல்லையெனில் {
        திரும்பு -idx - 1; /* == u_strlen(string) */
    }
}

/* ----- Text manipulation functions --- */

U_CAPI char16_t* U_EXPORT2
u_strtok_r(char16_t* src U_LIFETIME_BOUND, மாறிலி char16_t* delim, char16_t** saveState)
{
    char16_t *tokSource;
    char16_t *nextToken;
    uint32_t nonDelimIdx;

    /* If saveState is nullptr, the user messed up. */
    எனில் (src != nullptr) {
        tokSource = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    இல்லையெனில் எனில் (*saveState) {
        tokSource = *saveState;
    }
    இல்லையெனில் {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        திரும்பு nullptr;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tokSource, delim);
    tokSource = &tokSource[nonDelimIdx];

    எனில் (*tokSource) {
        nextToken = u_strpbrk(tokSource, delim);
        எனில் (nextToken != nullptr) {
            /* Create a token */
            *(nextToken++) = 0;
            *saveState = nextToken;
            திரும்பு tokSource;
        }
        இல்லையெனில் எனில் (*saveState) {
            /* Return the last token */
            *saveState = nullptr;
            திரும்பு tokSource;
        }
    }
    இல்லையெனில் {
        /* No tokens were found. Only delimiters were left. */
        *saveState = nullptr;
    }
    திரும்பு nullptr;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, மாறிலி char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    போது(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    போது((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    திரும்பு anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, மாறிலி char16_t* src, int32_t n)
{
    எனில்(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        போது(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        போது((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            எனில்(--n == 0) {
                *dst = 0;
                நிறுத்து;
            }
            ++src;
        }

        திரும்பு anchor;
    } இல்லையெனில் {
        திரும்பு dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(மாறிலி char16_t *s1,
    மாறிலி char16_t *s2)
{
    char16_t  c1, c2;

    க்கு(;;) {
        c1=*s1++;
        c2=*s2++;
        எனில் (c1 != c2 || c1 == 0) {
            நிறுத்து;
        }
    }
    திரும்பு (int32_t)c1 - (int32_t)c2;
}

U_CFUNC int32_t U_EXPORT2
uprv_strCompare(மாறிலி char16_t *s1, int32_t length1,
                மாறிலி char16_t *s2, int32_t length2,
                UBool strncmpStyle, UBool codePointOrder) {
    மாறிலி char16_t *start1, *start2, *limit1, *limit2;
    char16_t c1, c2;

    /* setup for fix-up */
    start1=s1;
    start2=s2;

    /* compare identical prefixes - they do not need to be fixed up */
    எனில்(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        எனில்(s1==s2) {
            திரும்பு 0;
        }

        க்கு(;;) {
            c1=*s1;
            c2=*s2;
            எனில்(c1!=c2) {
                நிறுத்து;
            }
            எனில்(c1==0) {
                திரும்பு 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=nullptr;
    } இல்லையெனில் எனில்(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        எனில்(s1==s2) {
            திரும்பு 0;
        }

        limit1=start1+length1;

        க்கு(;;) {
            /* both lengths are same, check only one limit */
            எனில்(s1==limit1) {
                திரும்பு 0;
            }

            c1=*s1;
            c2=*s2;
            எனில்(c1!=c2) {
                நிறுத்து;
            }
            எனில்(c1==0) {
                திரும்பு 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } இல்லையெனில் {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t lengthResult;

        எனில்(length1<0) {
            length1=u_strlen(s1);
        }
        எனில்(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        எனில்(length1<length2) {
            lengthResult=-1;
            limit1=start1+length1;
        } இல்லையெனில் எனில்(length1==length2) {
            lengthResult=0;
            limit1=start1+length1;
        } இல்லையெனில் /* length1>length2 */ {
            lengthResult=1;
            limit1=start1+length2;
        }

        எனில்(s1==s2) {
            திரும்பு lengthResult;
        }

        க்கு(;;) {
            /* check pseudo-limit */
            எனில்(s1==limit1) {
                திரும்பு lengthResult;
            }

            c1=*s1;
            c2=*s2;
            எனில்(c1!=c2) {
                நிறுத்து;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    எனில்(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        எனில்(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } இல்லையெனில் {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        எனில்(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } இல்லையெனில் {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    திரும்பு (int32_t)c1-(int32_t)c2;
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
    எனில்(iter1==nullptr || iter2==nullptr) {
        திரும்பு 0; /* bad arguments */
    }
    எனில்(iter1==iter2) {
        திரும்பு 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_START);
    iter2->move(iter2, 0, UITER_START);

    /* compare identical prefixes - they do not need to be fixed up */
    க்கு(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        எனில்(c1!=c2) {
            நிறுத்து;
        }
        எனில்(c1==-1) {
            திரும்பு 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    எனில்(c1>=0xd800 && c2>=0xd800 && codePointOrder) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        எனில்(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->previous(iter1), U16_IS_LEAD(iter1->previous(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } இல்லையெனில் {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        எனில்(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->previous(iter2), U16_IS_LEAD(iter2->previous(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } இல்லையெனில் {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    திரும்பு (int32_t)c1-(int32_t)c2;
}

#எனில் 0
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
வெறுமை fragment {
        /* iff a surrogate is part of a surrogate pair, leave >=d800 */
        எனில்(c1<=0xdbff) {
            எனில்(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } இல்லையெனில் எனில்(c1<=0xdfff) {
            int32_t idx=iter1->getIndex(iter1, UITER_CURRENT);
            iter1->previous(iter1); /* ==c1 */
            எனில்(!U16_IS_LEAD(iter1->previous(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } இல்லையெனில் /* 0xe000<=c1<=0xffff */ {
            /* BMP code point - make <d800 */
            c1-=0x2800;
        }
}
#endif

U_CAPI int32_t U_EXPORT2
u_strCompare(மாறிலி char16_t *s1, int32_t length1,
             மாறிலி char16_t *s2, int32_t length2,
             UBool codePointOrder) {
    /* argument checking */
    எனில்(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        திரும்பு 0;
    }
    திரும்பு uprv_strCompare(s1, length1, s2, length2, பொய், codePointOrder);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmpCodePointOrder(மாறிலி char16_t *s1, மாறிலி char16_t *s2) {
    திரும்பு uprv_strCompare(s1, -1, s2, -1, பொய், உண்மை);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(மாறிலி char16_t  *s1,
     மாறிலி char16_t  *s2,
     int32_t     n) 
{
    எனில்(n > 0) {
        int32_t rc;
        க்கு(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            எனில்(rc != 0 || *s1 == 0 || --n == 0) {
                திரும்பு rc;
            }
            ++s1;
            ++s2;
        }
    } இல்லையெனில் {
        திரும்பு 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmpCodePointOrder(மாறிலி char16_t *s1, மாறிலி char16_t *s2, int32_t n) {
    திரும்பு uprv_strCompare(s1, n, s2, n, உண்மை, உண்மை);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, மாறிலி char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    போது((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    திரும்பு anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, மாறிலி char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    போது(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    திரும்பு anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(மாறிலி char16_t *s)
{
#எனில் U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    திரும்பு (int32_t)uprv_wcslen((மாறிலி wchar_t *)s);
#இல்லையெனில்
    மாறிலி char16_t *t = s;
    போது(*t != 0) {
      ++t;
    }
    திரும்பு t - s;
#endif
}

U_CAPI int32_t U_EXPORT2
u_மொத்தம்_Char32(மாறிலி char16_t *s, int32_t length) {
    int32_t count;

    எனில்(s==nullptr || length<-1) {
        திரும்பு 0;
    }

    count=0;
    எனில்(length>=0) {
        போது(length>0) {
            ++count;
            எனில்(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } இல்லையெனில் {
                ++s;
                --length;
            }
        }
    } இல்லையெனில் /* length==-1 */ {
        char16_t c;

        க்கு(;;) {
            எனில்((c=*s++)==0) {
                நிறுத்து;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            எனில்(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    திரும்பு count;
}

U_CAPI UBool U_EXPORT2
u_strHasMoreChar32Than(மாறிலி char16_t *s, int32_t length, int32_t number) {

    எனில்(number<0) {
        திரும்பு உண்மை;
    }
    எனில்(s==nullptr || length<-1) {
        திரும்பு பொய்;
    }

    எனில்(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        க்கு(;;) {
            எனில்((c=*s++)==0) {
                திரும்பு பொய்;
            }
            எனில்(number==0) {
                திரும்பு உண்மை;
            }
            எனில்(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } இல்லையெனில் {
        /* length>=0 known */
        மாறிலி char16_t *limit;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        எனில்(((length+1)/2)>number) {
            திரும்பு உண்மை;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        எனில்(maxSupplementary<=0) {
            திரும்பு பொய்;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        limit=s+length;
        க்கு(;;) {
            எனில்(s==limit) {
                திரும்பு பொய்;
            }
            எனில்(number==0) {
                திரும்பு உண்மை;
            }
            எனில்(U16_IS_LEAD(*s++) && s!=limit && U16_IS_TRAIL(*s)) {
                ++s;
                எனில்(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    திரும்பு பொய்;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, மாறிலி char16_t* src, int32_t count) {
    எனில்(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    திரும்பு dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, மாறிலி char16_t* src, int32_t count) {
    எனில்(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    திரும்பு dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    எனில்(count > 0) {
        char16_t *ptr = dest;
        char16_t *limit = dest + count;

        போது (ptr < limit) {
            *(ptr++) = c;
        }
    }
    திரும்பு dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(மாறிலி char16_t *buf1, மாறிலி char16_t *buf2, int32_t count) {
    எனில்(count > 0) {
        மாறிலி char16_t *limit = buf1 + count;
        int32_t result;

        போது (buf1 < limit) {
            result = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            எனில் (result != 0) {
                திரும்பு result;
            }
            buf1++;
            buf2++;
        }
    }
    திரும்பு 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmpCodePointOrder(மாறிலி char16_t *s1, மாறிலி char16_t *s2, int32_t count) {
    திரும்பு uprv_strCompare(s1, count, s2, count, பொய், உண்மை);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
நிலையான மாறிலி char16_t UNESCAPE_MAP[] = {
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
எண்ணிக்கை { UNESCAPE_MAP_LENGTH = UPRV_LENGTHOF(UNESCAPE_MAP) };

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
நிலையான int32_t _digit8(char16_t c) {
    எனில் (c >= u'0' && c <= u'7') {
        திரும்பு c - u'0';
    }
    திரும்பு -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
நிலையான int32_t _digit16(char16_t c) {
    எனில் (c >= u'0' && c <= u'9') {
        திரும்பு c - u'0';
    }
    எனில் (c >= u'A' && c <= u'F') {
        திரும்பு c - (u'A' - 10);
    }
    எனில் (c >= u'a' && c <= u'f') {
        திரும்பு c - (u'a' - 10);
    }
    திரும்பு -1;
}

/* Parse a single escape sequence.  Although this method deals in
 * UChars, it does not use C++ or UnicodeString.  This allows it to
 * be used from C contexts. */
U_CAPI UChar32 U_EXPORT2
u_unescapeAt(UNESCAPE_CHAR_AT charAt,
             int32_t *offset,
             int32_t length,
             வெறுமை *context) {

    int32_t start = *offset;
    UChar32 c;
    UChar32 result = 0;
    int8_t n = 0;
    int8_t minDig = 0;
    int8_t maxDig = 0;
    int8_t bitsPerDigit = 4; 
    int32_t dig;
    UBool braces = பொய்;

    /* Check that offset is in range */
    எனில் (*offset < 0 || *offset >= length) {
        செல் err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*offset)++, context);

    /* Convert hexadecimal and octal escapes */
    தேர்வு (c) {
    நிலை u'u':
        minDig = maxDig = 4;
        நிறுத்து;
    நிலை u'U':
        minDig = maxDig = 8;
        நிறுத்து;
    நிலை u'x':
        minDig = 1;
        எனில் (*offset < length && charAt(*offset, context) == u'{') {
            ++(*offset);
            braces = உண்மை;
            maxDig = 8;
        } இல்லையெனில் {
            maxDig = 2;
        }
        நிறுத்து;
    இயல்பு:
        dig = _digit8(c);
        எனில் (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            bitsPerDigit = 3;
            result = dig;
        }
        நிறுத்து;
    }
    எனில் (minDig != 0) {
        போது (*offset < length && n < maxDig) {
            c = charAt(*offset, context);
            dig = (bitsPerDigit == 3) ? _digit8(c) : _digit16(c);
            எனில் (dig < 0) {
                நிறுத்து;
            }
            result = (result << bitsPerDigit) | dig;
            ++(*offset);
            ++n;
        }
        எனில் (n < minDig) {
            செல் err;
        }
        எனில் (braces) {
            எனில் (c != u'}') {
                செல் err;
            }
            ++(*offset);
        }
        எனில் (result < 0 || result >= 0x110000) {
            செல் err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        எனில் (*offset < length && U16_IS_LEAD(result)) {
            int32_t ahead = *offset + 1;
            c = charAt(*offset, context);
            எனில் (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t tailLimit = ahead + 11;
                எனில் (tailLimit > length) {
                    tailLimit = length;
                }
                c = u_unescapeAt(charAt, &ahead, tailLimit, context);
            }
            எனில் (U16_IS_TRAIL(c)) {
                *offset = ahead;
                result = U16_GET_SUPPLEMENTARY(result, c);
            }
        }
        திரும்பு result;
    }

    /* Convert C-style escapes in table */
    க்கு (int32_t i=0; i<UNESCAPE_MAP_LENGTH; i+=2) {
        எனில் (c == UNESCAPE_MAP[i]) {
            திரும்பு UNESCAPE_MAP[i+1];
        } இல்லையெனில் எனில் (c < UNESCAPE_MAP[i]) {
            நிறுத்து;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    எனில் (c == u'c' && *offset < length) {
        c = charAt((*offset)++, context);
        எனில் (U16_IS_LEAD(c) && *offset < length) {
            char16_t c2 = charAt(*offset, context);
            எனில் (U16_IS_TRAIL(c2)) {
                ++(*offset);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        திரும்பு 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    எனில் (U16_IS_LEAD(c) && *offset < length) {
        char16_t c2 = charAt(*offset, context);
        எனில் (U16_IS_TRAIL(c2)) {
            ++(*offset);
            திரும்பு U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    திரும்பு c;

 err:
    /* Invalid escape sequence */
    *offset = start; /* Reset to initial value */
    திரும்பு (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
நிலையான char16_t U_CALLCONV
_charPtr_charAt(int32_t offset, வெறுமை *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_charsToUChars(static_cast<எழுத்து*>(context) + offset, &c16, 1);
    திரும்பு c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
நிலையான வெறுமை _appendUChars(char16_t *dest, int32_t destCapacity,
                          மாறிலி எழுத்து *src, int32_t srcLen) {
    எனில் (destCapacity < 0) {
        destCapacity = 0;
    }
    எனில் (srcLen > destCapacity) {
        srcLen = destCapacity;
    }
    u_charsToUChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(மாறிலி எழுத்து *src, char16_t *dest, int32_t destCapacity) {
    மாறிலி எழுத்து *segment = src;
    int32_t i = 0;
    எழுத்து c;

    போது ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        எனில் (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            எனில் (src != segment) {
                எனில் (dest != nullptr) {
                    _appendUChars(dest + i, destCapacity - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<எழுத்து*>(src));
            எனில் (lenParsed == 0) {
                செல் err;
            }
            src += lenParsed; /* advance past escape seq. */
            எனில் (dest != nullptr && U16_LENGTH(c32) <= (destCapacity - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } இல்லையெனில் {
                i += U16_LENGTH(c32);
            }
            segment = src;
        } இல்லையெனில் {
            ++src;
        }
    }
    எனில் (src != segment) {
        எனில் (dest != nullptr) {
            _appendUChars(dest + i, destCapacity - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    எனில் (dest != nullptr && i < destCapacity) {
        dest[i] = 0;
    }
    திரும்பு i;

 err:
    எனில் (dest != nullptr && destCapacity > 0) {
        *dest = 0;
    }
    திரும்பு 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#define __TERMINATE_STRING(dest, destCapacity, length, pErrorCode) UPRV_BLOCK_MACRO_BEGIN { \
    எனில்(pErrorCode!=nullptr && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        எனில்(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } இல்லையெனில் எனில்(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            எனில்(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } இல்லையெனில் எனில்(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } இல்லையெனில் /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;                        \
        }                                                               \
    } \
} UPRV_BLOCK_MACRO_END

U_CAPI char16_t U_EXPORT2
u_asciiToUpper(char16_t c) {
    எனில் (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    திரும்பு c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    திரும்பு length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(எழுத்து *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    திரும்பு length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    திரும்பு length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    திரும்பு length;
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
    மாறிலி TYPE *p = (மாறிலி TYPE*) STR;        \
    எனில் (p != nullptr) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        மாறிலி TYPE *limit = p + len;          \
        போது (p<limit) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    திரும்பு static_cast<int32_t>(hash);        \
} UPRV_BLOCK_MACRO_END

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(மாறிலி char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(மாறிலி எழுத்து *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(மாறிலி எழுத்து *str, int32_t length) {
    STRING_HASH(எழுத்து, str, length, (uint8_t)uprv_tolower(*p));
}
