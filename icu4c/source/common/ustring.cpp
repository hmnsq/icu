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
#تعریف_کرو U_BMP_MAX 0xffff

/* Forward binary string search functions ----------------------------------- */

/*
 * Test if a substring match inside a string is at code point boundaries.
 * All pointers refer to the same buffer.
 * The limit pointer may be nullptr, all others must be real pointers.
 */
ساکن اندرونی UBool
is_مطابقت_At_CPBoundary(ثابت char16_t *آغاز, ثابت char16_t *مطابقت, ثابت char16_t *مطابقت_حد_بندی, ثابت char16_t *حد_بندی) {
    اگر(U16_IS_TRAIL(*مطابقت) && آغاز!=مطابقت && U16_IS_LEAD(*(مطابقت-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        واپس جھوٹ;
    }
    اگر(U16_IS_LEAD(*(مطابقت_حد_بندی-1)) && مطابقت_حد_بندی!=حد_بندی && U16_IS_TRAIL(*مطابقت_حد_بندی)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        واپس جھوٹ;
    }
    واپس سچ;
}

U_CAPI char16_t* U_EXPORT2
u_str_ڈھونڈو_پہلا(ثابت char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               ثابت char16_t* sub,
               int32_t sub_لمبائی) {
    ثابت char16_t *آغاز, *p, *q, *sub_حد_بندی;
    char16_t c, cs, cq;

    اگر(sub==خالی_اشاریہ || sub_لمبائی<-1) {
        واپس (char16_t *)s;
    }
    اگر(s==خالی_اشاریہ || length<-1) {
        واپس خالی_اشاریہ;
    }

    آغاز=s;

    اگر(length<0 && sub_لمبائی<0) {
        /* both strings are NUL-terminated */
        اگر((cs=*sub++)==0) {
            واپس (char16_t *)s;
        }
        اگر(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            واپس u_strchr(s, cs);
        }

        جب((c=*s++)!=0) {
            اگر(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                برائے(;;) {
                    اگر((cq=*q)==0) {
                        اگر(is_مطابقت_At_CPBoundary(آغاز, s-1, p, خالی_اشاریہ)) {
                            واپس (char16_t *)(s-1); /* well-formed match */
                        } ورنہ {
                            توڑو; /* no match because surrogate pair is split */
                        }
                    }
                    اگر((c=*p)==0) {
                        واپس خالی_اشاریہ; /* no match, and none possible after s */
                    }
                    اگر(c!=cq) {
                        توڑو; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        واپس خالی_اشاریہ;
    }

    اگر(sub_لمبائی<0) {
        sub_لمبائی=u_strlen(sub);
    }
    اگر(sub_لمبائی==0) {
        واپس (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --sub_لمبائی;
    sub_حد_بندی=sub+sub_لمبائی;

    اگر(sub_لمبائی==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        واپس length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    اگر(length<0) {
        /* s is NUL-terminated */
        جب((c=*s++)!=0) {
            اگر(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                برائے(;;) {
                    اگر(q==sub_حد_بندی) {
                        اگر(is_مطابقت_At_CPBoundary(آغاز, s-1, p, خالی_اشاریہ)) {
                            واپس (char16_t *)(s-1); /* well-formed match */
                        } ورنہ {
                            توڑو; /* no match because surrogate pair is split */
                        }
                    }
                    اگر((c=*p)==0) {
                        واپس خالی_اشاریہ; /* no match, and none possible after s */
                    }
                    اگر(c!=*q) {
                        توڑو; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } ورنہ {
        ثابت char16_t *حد_بندی, *pre_حد_بندی;

        /* subLength was decremented above */
        اگر(length<=sub_لمبائی) {
            واپس خالی_اشاریہ; /* s is shorter than sub */
        }

        حد_بندی=s+length;

        /* the substring must start before preLimit */
        pre_حد_بندی=حد_بندی-sub_لمبائی;

        جب(s!=pre_حد_بندی) {
            c=*s++;
            اگر(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                برائے(;;) {
                    اگر(q==sub_حد_بندی) {
                        اگر(is_مطابقت_At_CPBoundary(آغاز, s-1, p, حد_بندی)) {
                            واپس (char16_t *)(s-1); /* well-formed match */
                        } ورنہ {
                            توڑو; /* no match because surrogate pair is split */
                        }
                    }
                    اگر(*p!=*q) {
                        توڑو; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    واپس خالی_اشاریہ;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(ثابت char16_t* s U_LIFETIME_BOUND, ثابت char16_t* substring) {
    واپس u_str_ڈھونڈو_پہلا(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(ثابت char16_t* s U_LIFETIME_BOUND, char16_t c) {
    اگر(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        واپس u_str_ڈھونڈو_پہلا(s, -1, &c, 1);
    } ورنہ {
        char16_t cs;

        /* trivial search for a BMP code point */
        برائے(;;) {
            اگر((cs=*s)==c) {
                واپس (char16_t *)s;
            }
            اگر(cs==0) {
                واپس خالی_اشاریہ;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(ثابت char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    اگر((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        واپس u_strchr(s, (char16_t)c);
    } ورنہ اگر((uint32_t)c<=UCHAR_MAX_قدر) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        جب((cs=*s++)!=0) {
            اگر(cs==lead && *s==trail) {
                واپس (char16_t *)(s-1);
            }
        }
        واپس خالی_اشاریہ;
    } ورنہ {
        /* not a Unicode code point, not findable */
        واپس خالی_اشاریہ;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(ثابت char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    اگر(count<=0) {
        واپس خالی_اشاریہ; /* no string */
    } ورنہ اگر(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        واپس u_str_ڈھونڈو_پہلا(s, count, &c, 1);
    } ورنہ {
        /* trivial search for a BMP code point */
        ثابت char16_t *حد_بندی=s+count;
        کرو {
            اگر(*s==c) {
                واپس (char16_t *)s;
            }
        } جب(++s!=حد_بندی);
        واپس خالی_اشاریہ;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(ثابت char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    اگر((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        واپس u_memchr(s, (char16_t)c, count);
    } ورنہ اگر(count<2) {
        /* too short for a surrogate pair */
        واپس خالی_اشاریہ;
    } ورنہ اگر((uint32_t)c<=UCHAR_MAX_قدر) {
        /* find supplementary code point as surrogate pair */
        ثابت char16_t *حد_بندی=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        کرو {
            اگر(*s==lead && *(s+1)==trail) {
                واپس (char16_t *)s;
            }
        } جب(++s!=حد_بندی);
        واپس خالی_اشاریہ;
    } ورنہ {
        /* not a Unicode code point, not findable */
        واپس خالی_اشاریہ;
    }
}

/* Backward binary string search functions ---------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_str_ڈھونڈو_آخری(ثابت char16_t* s U_LIFETIME_BOUND,
              int32_t length,
              ثابت char16_t* sub,
              int32_t sub_لمبائی) {
    ثابت char16_t *آغاز, *حد_بندی, *p, *q, *sub_حد_بندی;
    char16_t c, cs;

    اگر(sub==خالی_اشاریہ || sub_لمبائی<-1) {
        واپس (char16_t *)s;
    }
    اگر(s==خالی_اشاریہ || length<-1) {
        واپس خالی_اشاریہ;
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

    اگر(sub_لمبائی<0) {
        sub_لمبائی=u_strlen(sub);
    }
    اگر(sub_لمبائی==0) {
        واپس (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    sub_حد_بندی=sub+sub_لمبائی;
    cs=*(--sub_حد_بندی);
    --sub_لمبائی;

    اگر(sub_لمبائی==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        واپس length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    اگر(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    اگر(length<=sub_لمبائی) {
        واپس خالی_اشاریہ; /* s is shorter than sub */
    }

    آغاز=s;
    حد_بندی=s+length;

    /* the substring must start no later than s+subLength */
    s+=sub_لمبائی;

    جب(s!=حد_بندی) {
        c=*(--حد_بندی);
        اگر(c==cs) {
            /* found last substring char16_t, compare rest */
            p=حد_بندی;
            q=sub_حد_بندی;
            برائے(;;) {
                اگر(q==sub) {
                    اگر(is_مطابقت_At_CPBoundary(آغاز, p, حد_بندی+1, آغاز+length)) {
                        واپس (char16_t *)p; /* well-formed match */
                    } ورنہ {
                        توڑو; /* no match because surrogate pair is split */
                    }
                }
                اگر(*(--p)!=*(--q)) {
                    توڑو; /* no match */
                }
            }
        }
    }

    /* not found */
    واپس خالی_اشاریہ;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(ثابت char16_t* s U_LIFETIME_BOUND, ثابت char16_t* substring) {
    واپس u_str_ڈھونڈو_آخری(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(ثابت char16_t* s U_LIFETIME_BOUND, char16_t c) {
    اگر(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        واپس u_str_ڈھونڈو_آخری(s, -1, &c, 1);
    } ورنہ {
        ثابت char16_t *نتیجہ=خالی_اشاریہ;
        char16_t cs;

        /* trivial search for a BMP code point */
        برائے(;;) {
            اگر((cs=*s)==c) {
                نتیجہ=s;
            }
            اگر(cs==0) {
                واپس (char16_t *)نتیجہ;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(ثابت char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    اگر((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        واپس u_strrchr(s, (char16_t)c);
    } ورنہ اگر((uint32_t)c<=UCHAR_MAX_قدر) {
        /* find supplementary code point as surrogate pair */
        ثابت char16_t *نتیجہ=خالی_اشاریہ;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        جب((cs=*s++)!=0) {
            اگر(cs==lead && *s==trail) {
                نتیجہ=s-1;
            }
        }
        واپس (char16_t *)نتیجہ;
    } ورنہ {
        /* not a Unicode code point, not findable */
        واپس خالی_اشاریہ;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(ثابت char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    اگر(count<=0) {
        واپس خالی_اشاریہ; /* no string */
    } ورنہ اگر(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        واپس u_str_ڈھونڈو_آخری(s, count, &c, 1);
    } ورنہ {
        /* trivial search for a BMP code point */
        ثابت char16_t *حد_بندی=s+count;
        کرو {
            اگر(*(--حد_بندی)==c) {
                واپس (char16_t *)حد_بندی;
            }
        } جب(s!=حد_بندی);
        واپس خالی_اشاریہ;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(ثابت char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    اگر((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        واپس u_memrchr(s, (char16_t)c, count);
    } ورنہ اگر(count<2) {
        /* too short for a surrogate pair */
        واپس خالی_اشاریہ;
    } ورنہ اگر((uint32_t)c<=UCHAR_MAX_قدر) {
        /* find supplementary code point as surrogate pair */
        ثابت char16_t *حد_بندی=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        کرو {
            اگر(*حد_بندی==trail && *(حد_بندی-1)==lead) {
                واپس (char16_t *)(حد_بندی-1);
            }
        } جب(s!=--حد_بندی);
        واپس خالی_اشاریہ;
    } ورنہ {
        /* not a Unicode code point, not findable */
        واپس خالی_اشاریہ;
    }
}

/* Tokenization functions --------------------------------------------------- */

/*
 * Match each code point in a string against each code point in the matchSet.
 * Return the index of the first string code point that
 * is (polarity==true) or is not (false) contained in the matchSet.
 * Return -(string length)-1 if there is no such code point.
 */
ساکن int32_t
مطابقت_From_سیٹ(ثابت char16_t *string, ثابت char16_t *مطابقت_سیٹ, UBool polarity) {
    int32_t مطابقت_Len, مطابقت_BMPLen, strItr, مطابقت_Itr;
    UChar32 stringCh, مطابقت_Ch;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    مطابقت_BMPLen = 0;
    جب((c = مطابقت_سیٹ[مطابقت_BMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++مطابقت_BMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    مطابقت_Len = مطابقت_BMPLen;
    جب(مطابقت_سیٹ[مطابقت_Len] != 0) {
        ++مطابقت_Len;
    }

    برائے(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        اگر(U16_IS_SINGLE(c)) {
            اگر(polarity) {
                برائے(مطابقت_Itr = 0; مطابقت_Itr < مطابقت_Len; ++مطابقت_Itr) {
                    اگر(c == مطابقت_سیٹ[مطابقت_Itr]) {
                        واپس strItr - 1; /* one matches */
                    }
                }
            } ورنہ {
                برائے(مطابقت_Itr = 0; مطابقت_Itr < مطابقت_Len; ++مطابقت_Itr) {
                    اگر(c == مطابقت_سیٹ[مطابقت_Itr]) {
                        جاؤ endloop;
                    }
                }
                واپس strItr - 1; /* none matches */
            }
        } ورنہ {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            اگر(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } ورنہ {
                stringCh = c; /* unpaired trail surrogate */
            }

            اگر(polarity) {
                برائے(مطابقت_Itr = مطابقت_BMPLen; مطابقت_Itr < مطابقت_Len;) {
                    U16_اگلا(مطابقت_سیٹ, مطابقت_Itr, مطابقت_Len, مطابقت_Ch);
                    اگر(stringCh == مطابقت_Ch) {
                        واپس strItr - U16_لمبائی(stringCh); /* one matches */
                    }
                }
            } ورنہ {
                برائے(مطابقت_Itr = مطابقت_BMPLen; مطابقت_Itr < مطابقت_Len;) {
                    U16_اگلا(مطابقت_سیٹ, مطابقت_Itr, مطابقت_Len, مطابقت_Ch);
                    اگر(stringCh == مطابقت_Ch) {
                        جاؤ endloop;
                    }
                }
                واپس strItr - U16_لمبائی(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    واپس -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(ثابت char16_t* string U_LIFETIME_BOUND, ثابت char16_t* مطابقت_سیٹ)
{
    int32_t idx = مطابقت_From_سیٹ(string, مطابقت_سیٹ, سچ);
    اگر(idx >= 0) {
        واپس (char16_t *)string + idx;
    } ورنہ {
        واپس خالی_اشاریہ;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(ثابت char16_t *string, ثابت char16_t *مطابقت_سیٹ)
{
    int32_t idx = مطابقت_From_سیٹ(string, مطابقت_سیٹ, سچ);
    اگر(idx >= 0) {
        واپس idx;
    } ورنہ {
        واپس -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(ثابت char16_t *string, ثابت char16_t *مطابقت_سیٹ)
{
    int32_t idx = مطابقت_From_سیٹ(string, مطابقت_سیٹ, جھوٹ);
    اگر(idx >= 0) {
        واپس idx;
    } ورنہ {
        واپس -idx - 1; /* == u_strlen(string) */
    }
}

/* ----- Text manipulation functions --- */

U_CAPI char16_t* U_EXPORT2
u_strtok_r(char16_t* src U_LIFETIME_BOUND, ثابت char16_t* delim, char16_t** saveState)
{
    char16_t *tok_ماخذ;
    char16_t *اگلا_Token;
    uint32_t nonDelimIdx;

    /* If saveState is nullptr, the user messed up. */
    اگر (src != خالی_اشاریہ) {
        tok_ماخذ = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    ورنہ اگر (*saveState) {
        tok_ماخذ = *saveState;
    }
    ورنہ {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        واپس خالی_اشاریہ;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tok_ماخذ, delim);
    tok_ماخذ = &tok_ماخذ[nonDelimIdx];

    اگر (*tok_ماخذ) {
        اگلا_Token = u_strpbrk(tok_ماخذ, delim);
        اگر (اگلا_Token != خالی_اشاریہ) {
            /* Create a token */
            *(اگلا_Token++) = 0;
            *saveState = اگلا_Token;
            واپس tok_ماخذ;
        }
        ورنہ اگر (*saveState) {
            /* Return the last token */
            *saveState = خالی_اشاریہ;
            واپس tok_ماخذ;
        }
    }
    ورنہ {
        /* No tokens were found. Only delimiters were left. */
        *saveState = خالی_اشاریہ;
    }
    واپس خالی_اشاریہ;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, ثابت char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    جب(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    جب((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    واپس anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, ثابت char16_t* src, int32_t n)
{
    اگر(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        جب(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        جب((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            اگر(--n == 0) {
                *dst = 0;
                توڑو;
            }
            ++src;
        }

        واپس anchor;
    } ورنہ {
        واپس dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(ثابت char16_t *s1,
    ثابت char16_t *s2)
{
    char16_t  c1, c2;

    برائے(;;) {
        c1=*s1++;
        c2=*s2++;
        اگر (c1 != c2 || c1 == 0) {
            توڑو;
        }
    }
    واپس (int32_t)c1 - (int32_t)c2;
}

U_CFUNC int32_t U_EXPORT2
uprv_strCompare(ثابت char16_t *s1, int32_t length1,
                ثابت char16_t *s2, int32_t length2,
                UBool strncmpStyle, UBool رمز_نقطہ_ترتیب) {
    ثابت char16_t *start1, *start2, *limit1, *limit2;
    char16_t c1, c2;

    /* setup for fix-up */
    start1=s1;
    start2=s2;

    /* compare identical prefixes - they do not need to be fixed up */
    اگر(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        اگر(s1==s2) {
            واپس 0;
        }

        برائے(;;) {
            c1=*s1;
            c2=*s2;
            اگر(c1!=c2) {
                توڑو;
            }
            اگر(c1==0) {
                واپس 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=خالی_اشاریہ;
    } ورنہ اگر(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        اگر(s1==s2) {
            واپس 0;
        }

        limit1=start1+length1;

        برائے(;;) {
            /* both lengths are same, check only one limit */
            اگر(s1==limit1) {
                واپس 0;
            }

            c1=*s1;
            c2=*s2;
            اگر(c1!=c2) {
                توڑو;
            }
            اگر(c1==0) {
                واپس 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } ورنہ {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t لمبائی_نتیجہ;

        اگر(length1<0) {
            length1=u_strlen(s1);
        }
        اگر(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        اگر(length1<length2) {
            لمبائی_نتیجہ=-1;
            limit1=start1+length1;
        } ورنہ اگر(length1==length2) {
            لمبائی_نتیجہ=0;
            limit1=start1+length1;
        } ورنہ /* length1>length2 */ {
            لمبائی_نتیجہ=1;
            limit1=start1+length2;
        }

        اگر(s1==s2) {
            واپس لمبائی_نتیجہ;
        }

        برائے(;;) {
            /* check pseudo-limit */
            اگر(s1==limit1) {
                واپس لمبائی_نتیجہ;
            }

            c1=*s1;
            c2=*s2;
            اگر(c1!=c2) {
                توڑو;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    اگر(c1>=0xd800 && c2>=0xd800 && رمز_نقطہ_ترتیب) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        اگر(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ورنہ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        اگر(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ورنہ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    واپس (int32_t)c1-(int32_t)c2;
}

/*
 * Compare two strings as presented by UCharIterators.
 * Use code unit or code point order.
 * When the function returns, it is undefined where the iterators
 * have stopped.
 */
U_CAPI int32_t U_EXPORT2
u_strCompareIter(UCharIterator *iter1, UCharIterator *iter2, UBool رمز_نقطہ_ترتیب) {
    UChar32 c1, c2;

    /* argument checking */
    اگر(iter1==خالی_اشاریہ || iter2==خالی_اشاریہ) {
        واپس 0; /* bad arguments */
    }
    اگر(iter1==iter2) {
        واپس 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_آغاز);
    iter2->move(iter2, 0, UITER_آغاز);

    /* compare identical prefixes - they do not need to be fixed up */
    برائے(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        اگر(c1!=c2) {
            توڑو;
        }
        اگر(c1==-1) {
            واپس 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    اگر(c1>=0xd800 && c2>=0xd800 && رمز_نقطہ_ترتیب) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        اگر(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->پچھلا_2(iter1), U16_IS_LEAD(iter1->پچھلا_2(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ورنہ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        اگر(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->پچھلا_2(iter2), U16_IS_LEAD(iter2->پچھلا_2(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ورنہ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    واپس (int32_t)c1-(int32_t)c2;
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
خالی fragment {
        /* iff a surrogate is part of a surrogate pair, leave >=d800 */
        اگر(c1<=0xdbff) {
            اگر(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } ورنہ اگر(c1<=0xdfff) {
            int32_t idx=iter1->get_اشاریہ(iter1, UITER_CURRENT);
            iter1->پچھلا_2(iter1); /* ==c1 */
            اگر(!U16_IS_LEAD(iter1->پچھلا_2(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } ورنہ /* 0xe000<=c1<=0xffff */ {
            /* BMP code point - make <d800 */
            c1-=0x2800;
        }
}
#اگر_ختم

U_CAPI int32_t U_EXPORT2
u_strCompare(ثابت char16_t *s1, int32_t length1,
             ثابت char16_t *s2, int32_t length2,
             UBool رمز_نقطہ_ترتیب) {
    /* argument checking */
    اگر(s1==خالی_اشاریہ || length1<-1 || s2==خالی_اشاریہ || length2<-1) {
        واپس 0;
    }
    واپس uprv_strCompare(s1, length1, s2, length2, جھوٹ, رمز_نقطہ_ترتیب);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmp_رمز_نقطہ_ترتیب(ثابت char16_t *s1, ثابت char16_t *s2) {
    واپس uprv_strCompare(s1, -1, s2, -1, جھوٹ, سچ);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(ثابت char16_t  *s1,
     ثابت char16_t  *s2,
     int32_t     n) 
{
    اگر(n > 0) {
        int32_t rc;
        برائے(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            اگر(rc != 0 || *s1 == 0 || --n == 0) {
                واپس rc;
            }
            ++s1;
            ++s2;
        }
    } ورنہ {
        واپس 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmp_رمز_نقطہ_ترتیب(ثابت char16_t *s1, ثابت char16_t *s2, int32_t n) {
    واپس uprv_strCompare(s1, n, s2, n, سچ, سچ);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, ثابت char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    جب((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    واپس anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, ثابت char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    جب(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    واپس anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(ثابت char16_t *s)
{
#اگر U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    واپس (int32_t)uprv_wcslen((ثابت wchar_t *)s);
#ورنہ
    ثابت char16_t *t = s;
    جب(*t != 0) {
      ++t;
    }
    واپس t - s;
#اگر_ختم
}

U_CAPI int32_t U_EXPORT2
u_گنتی_Char32(ثابت char16_t *s, int32_t length) {
    int32_t count;

    اگر(s==خالی_اشاریہ || length<-1) {
        واپس 0;
    }

    count=0;
    اگر(length>=0) {
        جب(length>0) {
            ++count;
            اگر(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } ورنہ {
                ++s;
                --length;
            }
        }
    } ورنہ /* length==-1 */ {
        char16_t c;

        برائے(;;) {
            اگر((c=*s++)==0) {
                توڑو;
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
    واپس count;
}

U_CAPI UBool U_EXPORT2
u_str_Has_مزید_Char32_Than(ثابت char16_t *s, int32_t length, int32_t number) {

    اگر(number<0) {
        واپس سچ;
    }
    اگر(s==خالی_اشاریہ || length<-1) {
        واپس جھوٹ;
    }

    اگر(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        برائے(;;) {
            اگر((c=*s++)==0) {
                واپس جھوٹ;
            }
            اگر(number==0) {
                واپس سچ;
            }
            اگر(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } ورنہ {
        /* length>=0 known */
        ثابت char16_t *حد_بندی;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        اگر(((length+1)/2)>number) {
            واپس سچ;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        اگر(maxSupplementary<=0) {
            واپس جھوٹ;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        حد_بندی=s+length;
        برائے(;;) {
            اگر(s==حد_بندی) {
                واپس جھوٹ;
            }
            اگر(number==0) {
                واپس سچ;
            }
            اگر(U16_IS_LEAD(*s++) && s!=حد_بندی && U16_IS_TRAIL(*s)) {
                ++s;
                اگر(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    واپس جھوٹ;
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
    واپس dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, ثابت char16_t* src, int32_t count) {
    اگر(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    واپس dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    اگر(count > 0) {
        char16_t *ptr = dest;
        char16_t *حد_بندی = dest + count;

        جب (ptr < حد_بندی) {
            *(ptr++) = c;
        }
    }
    واپس dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(ثابت char16_t *buf1, ثابت char16_t *buf2, int32_t count) {
    اگر(count > 0) {
        ثابت char16_t *حد_بندی = buf1 + count;
        int32_t نتیجہ;

        جب (buf1 < حد_بندی) {
            نتیجہ = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            اگر (نتیجہ != 0) {
                واپس نتیجہ;
            }
            buf1++;
            buf2++;
        }
    }
    واپس 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmp_رمز_نقطہ_ترتیب(ثابت char16_t *s1, ثابت char16_t *s2, int32_t count) {
    واپس uprv_strCompare(s1, count, s2, count, جھوٹ, سچ);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
ساکن ثابت char16_t UNESCAPE_MAP[] = {
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
شمار { UNESCAPE_MAP_لمبائی = UPRV_LENGTHOF(UNESCAPE_MAP) };

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
ساکن int32_t _digit8(char16_t c) {
    اگر (c >= u'0' && c <= u'7') {
        واپس c - u'0';
    }
    واپس -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
ساکن int32_t _digit16(char16_t c) {
    اگر (c >= u'0' && c <= u'9') {
        واپس c - u'0';
    }
    اگر (c >= u'A' && c <= u'F') {
        واپس c - (u'A' - 10);
    }
    اگر (c >= u'a' && c <= u'f') {
        واپس c - (u'a' - 10);
    }
    واپس -1;
}

/* Parse a single escape sequence.  Although this method deals in
 * UChars, it does not use C++ or UnicodeString.  This allows it to
 * be used from C contexts. */
U_CAPI UChar32 U_EXPORT2
u_unescapeAt(UNESCAPE_CHAR_AT charAt,
             int32_t *انحراف,
             int32_t length,
             خالی *context) {

    int32_t آغاز = *انحراف;
    UChar32 c;
    UChar32 نتیجہ = 0;
    int8_t n = 0;
    int8_t minDig = 0;
    int8_t maxDig = 0;
    int8_t بٹ_فی_Digit = 4; 
    int32_t dig;
    UBool braces = جھوٹ;

    /* Check that offset is in range */
    اگر (*انحراف < 0 || *انحراف >= length) {
        جاؤ err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*انحراف)++, context);

    /* Convert hexadecimal and octal escapes */
    انتخاب (c) {
    حالت u'u':
        minDig = maxDig = 4;
        توڑو;
    حالت u'U':
        minDig = maxDig = 8;
        توڑو;
    حالت u'x':
        minDig = 1;
        اگر (*انحراف < length && charAt(*انحراف, context) == u'{') {
            ++(*انحراف);
            braces = سچ;
            maxDig = 8;
        } ورنہ {
            maxDig = 2;
        }
        توڑو;
    طےشدہ:
        dig = _digit8(c);
        اگر (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            بٹ_فی_Digit = 3;
            نتیجہ = dig;
        }
        توڑو;
    }
    اگر (minDig != 0) {
        جب (*انحراف < length && n < maxDig) {
            c = charAt(*انحراف, context);
            dig = (بٹ_فی_Digit == 3) ? _digit8(c) : _digit16(c);
            اگر (dig < 0) {
                توڑو;
            }
            نتیجہ = (نتیجہ << بٹ_فی_Digit) | dig;
            ++(*انحراف);
            ++n;
        }
        اگر (n < minDig) {
            جاؤ err;
        }
        اگر (braces) {
            اگر (c != u'}') {
                جاؤ err;
            }
            ++(*انحراف);
        }
        اگر (نتیجہ < 0 || نتیجہ >= 0x110000) {
            جاؤ err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        اگر (*انحراف < length && U16_IS_LEAD(نتیجہ)) {
            int32_t ahead = *انحراف + 1;
            c = charAt(*انحراف, context);
            اگر (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t دم_حد_بندی = ahead + 11;
                اگر (دم_حد_بندی > length) {
                    دم_حد_بندی = length;
                }
                c = u_unescapeAt(charAt, &ahead, دم_حد_بندی, context);
            }
            اگر (U16_IS_TRAIL(c)) {
                *انحراف = ahead;
                نتیجہ = U16_GET_SUPPLEMENTARY(نتیجہ, c);
            }
        }
        واپس نتیجہ;
    }

    /* Convert C-style escapes in table */
    برائے (int32_t i=0; i<UNESCAPE_MAP_لمبائی; i+=2) {
        اگر (c == UNESCAPE_MAP[i]) {
            واپس UNESCAPE_MAP[i+1];
        } ورنہ اگر (c < UNESCAPE_MAP[i]) {
            توڑو;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    اگر (c == u'c' && *انحراف < length) {
        c = charAt((*انحراف)++, context);
        اگر (U16_IS_LEAD(c) && *انحراف < length) {
            char16_t c2 = charAt(*انحراف, context);
            اگر (U16_IS_TRAIL(c2)) {
                ++(*انحراف);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        واپس 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    اگر (U16_IS_LEAD(c) && *انحراف < length) {
        char16_t c2 = charAt(*انحراف, context);
        اگر (U16_IS_TRAIL(c2)) {
            ++(*انحراف);
            واپس U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    واپس c;

 err:
    /* Invalid escape sequence */
    *انحراف = آغاز; /* Reset to initial value */
    واپس (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
ساکن char16_t U_CALLCONV
_charPtr_charAt(int32_t انحراف, خالی *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_chars_مقصد_UChars(static_cast<حرف*>(context) + انحراف, &c16, 1);
    واپس c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
ساکن خالی _appendUChars(char16_t *dest, int32_t dest_استعداد,
                          ثابت حرف *src, int32_t srcLen) {
    اگر (dest_استعداد < 0) {
        dest_استعداد = 0;
    }
    اگر (srcLen > dest_استعداد) {
        srcLen = dest_استعداد;
    }
    u_chars_مقصد_UChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(ثابت حرف *src, char16_t *dest, int32_t dest_استعداد) {
    ثابت حرف *segment = src;
    int32_t i = 0;
    حرف c;

    جب ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        اگر (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            اگر (src != segment) {
                اگر (dest != خالی_اشاریہ) {
                    _appendUChars(dest + i, dest_استعداد - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<حرف*>(src));
            اگر (lenParsed == 0) {
                جاؤ err;
            }
            src += lenParsed; /* advance past escape seq. */
            اگر (dest != خالی_اشاریہ && U16_لمبائی(c32) <= (dest_استعداد - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } ورنہ {
                i += U16_لمبائی(c32);
            }
            segment = src;
        } ورنہ {
            ++src;
        }
    }
    اگر (src != segment) {
        اگر (dest != خالی_اشاریہ) {
            _appendUChars(dest + i, dest_استعداد - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    اگر (dest != خالی_اشاریہ && i < dest_استعداد) {
        dest[i] = 0;
    }
    واپس i;

 err:
    اگر (dest != خالی_اشاریہ && dest_استعداد > 0) {
        *dest = 0;
    }
    واپس 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#تعریف_کرو __TERMINATE_STRING(dest, dest_استعداد, length, p_Error_رمز) UPRV_بلاک_MACRO_BEGIN { \
    اگر(p_Error_رمز!=خالی_اشاریہ && U_SUCCESS(*p_Error_رمز)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        اگر(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } ورنہ اگر(length<dest_استعداد) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            اگر(*p_Error_رمز==U_STRING_NOT_TERMINATED_WARNING) {          \
                *p_Error_رمز=U_ZERO_ERROR;                               \
            }                                                           \
        } ورنہ اگر(length==dest_استعداد) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *p_Error_رمز=U_STRING_NOT_TERMINATED_WARNING;                \
        } ورنہ /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;                        \
        }                                                               \
    } \
} UPRV_بلاک_MACRO_انتہا

U_CAPI char16_t U_EXPORT2
u_ascii_مقصد_بالائی(char16_t c) {
    اگر (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    واپس c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t dest_استعداد, int32_t length, UError_رمز *p_Error_رمز) {
    __TERMINATE_STRING(dest, dest_استعداد, length, p_Error_رمز);
    واپس length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(حرف *dest, int32_t dest_استعداد, int32_t length, UError_رمز *p_Error_رمز) {
    __TERMINATE_STRING(dest, dest_استعداد, length, p_Error_رمز);
    واپس length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t dest_استعداد, int32_t length, UError_رمز *p_Error_رمز) {
    __TERMINATE_STRING(dest, dest_استعداد, length, p_Error_رمز);
    واپس length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t dest_استعداد, int32_t length, UError_رمز *p_Error_رمز) {
    __TERMINATE_STRING(dest, dest_استعداد, length, p_Error_رمز);
    واپس length;
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

#تعریف_کرو STRING_HASH(TYPE, STR, STRLEN, DEREF) UPRV_بلاک_MACRO_BEGIN { \
    uint32_t hash = 0;                        \
    ثابت TYPE *p = (ثابت TYPE*) STR;        \
    اگر (p != خالی_اشاریہ) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        ثابت TYPE *حد_بندی = p + len;          \
        جب (p<حد_بندی) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    واپس static_cast<int32_t>(hash);        \
} UPRV_بلاک_MACRO_انتہا

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(ثابت char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(ثابت حرف *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(ثابت حرف *str, int32_t length) {
    STRING_HASH(حرف, str, length, (uint8_t)uprv_tolower(*p));
}
