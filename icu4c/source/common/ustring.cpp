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

#ضمن "unicode/utypes.h"
#ضمن "unicode/putil.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/utf16.h"
#ضمن "cstring.h"
#ضمن "cwchar.h"
#ضمن "cmemory.h"
#ضمن "ustr_imp.h"

/* ANSI string.h - style functions ------------------------------------------ */

/* U+ffff is the highest BMP code point, the highest one that fits into a 16-bit char16_t */
#وصف_ڪريو U_BMP_MAX 0xffff

/* Forward binary string search functions ----------------------------------- */

/*
 * Test if a substring match inside a string is at code point boundaries.
 * All pointers refer to the same buffer.
 * The limit pointer may be nullptr, all others must be real pointers.
 */
ساكن مضمن UBool
is_ميلاپ_At_CPBoundary(ثابت char16_t *شروعات, ثابت char16_t *ميلاپ, ثابت char16_t *ميلاپ_حد_بندي, ثابت char16_t *حد_بندي) {
    إذا(U16_IS_TRAIL(*ميلاپ) && شروعات!=ميلاپ && U16_IS_LEAD(*(ميلاپ-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        أرجع خطأ;
    }
    إذا(U16_IS_LEAD(*(ميلاپ_حد_بندي-1)) && ميلاپ_حد_بندي!=حد_بندي && U16_IS_TRAIL(*ميلاپ_حد_بندي)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        أرجع خطأ;
    }
    أرجع سچ;
}

U_CAPI char16_t* U_EXPORT2
u_str_ڳوليو_پهريون(ثابت char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               ثابت char16_t* sub,
               int32_t sub_ڊيگهائي) {
    ثابت char16_t *شروعات, *p, *q, *sub_حد_بندي;
    char16_t c, cs, cq;

    إذا(sub==خالي_اشارو || sub_ڊيگهائي<-1) {
        أرجع (char16_t *)s;
    }
    إذا(s==خالي_اشارو || length<-1) {
        أرجع خالي_اشارو;
    }

    شروعات=s;

    إذا(length<0 && sub_ڊيگهائي<0) {
        /* both strings are NUL-terminated */
        إذا((cs=*sub++)==0) {
            أرجع (char16_t *)s;
        }
        إذا(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            أرجع u_strchr(s, cs);
        }

        طالما((c=*s++)!=0) {
            إذا(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                لكل(;;) {
                    إذا((cq=*q)==0) {
                        إذا(is_ميلاپ_At_CPBoundary(شروعات, s-1, p, خالي_اشارو)) {
                            أرجع (char16_t *)(s-1); /* well-formed match */
                        } وإلا {
                            اكسر; /* no match because surrogate pair is split */
                        }
                    }
                    إذا((c=*p)==0) {
                        أرجع خالي_اشارو; /* no match, and none possible after s */
                    }
                    إذا(c!=cq) {
                        اكسر; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        أرجع خالي_اشارو;
    }

    إذا(sub_ڊيگهائي<0) {
        sub_ڊيگهائي=u_strlen(sub);
    }
    إذا(sub_ڊيگهائي==0) {
        أرجع (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --sub_ڊيگهائي;
    sub_حد_بندي=sub+sub_ڊيگهائي;

    إذا(sub_ڊيگهائي==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        أرجع length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    إذا(length<0) {
        /* s is NUL-terminated */
        طالما((c=*s++)!=0) {
            إذا(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                لكل(;;) {
                    إذا(q==sub_حد_بندي) {
                        إذا(is_ميلاپ_At_CPBoundary(شروعات, s-1, p, خالي_اشارو)) {
                            أرجع (char16_t *)(s-1); /* well-formed match */
                        } وإلا {
                            اكسر; /* no match because surrogate pair is split */
                        }
                    }
                    إذا((c=*p)==0) {
                        أرجع خالي_اشارو; /* no match, and none possible after s */
                    }
                    إذا(c!=*q) {
                        اكسر; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } وإلا {
        ثابت char16_t *حد_بندي, *pre_حد_بندي;

        /* subLength was decremented above */
        إذا(length<=sub_ڊيگهائي) {
            أرجع خالي_اشارو; /* s is shorter than sub */
        }

        حد_بندي=s+length;

        /* the substring must start before preLimit */
        pre_حد_بندي=حد_بندي-sub_ڊيگهائي;

        طالما(s!=pre_حد_بندي) {
            c=*s++;
            إذا(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                لكل(;;) {
                    إذا(q==sub_حد_بندي) {
                        إذا(is_ميلاپ_At_CPBoundary(شروعات, s-1, p, حد_بندي)) {
                            أرجع (char16_t *)(s-1); /* well-formed match */
                        } وإلا {
                            اكسر; /* no match because surrogate pair is split */
                        }
                    }
                    إذا(*p!=*q) {
                        اكسر; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    أرجع خالي_اشارو;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(ثابت char16_t* s U_LIFETIME_BOUND, ثابت char16_t* substring) {
    أرجع u_str_ڳوليو_پهريون(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(ثابت char16_t* s U_LIFETIME_BOUND, char16_t c) {
    إذا(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        أرجع u_str_ڳوليو_پهريون(s, -1, &c, 1);
    } وإلا {
        char16_t cs;

        /* trivial search for a BMP code point */
        لكل(;;) {
            إذا((cs=*s)==c) {
                أرجع (char16_t *)s;
            }
            إذا(cs==0) {
                أرجع خالي_اشارو;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(ثابت char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    إذا((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        أرجع u_strchr(s, (char16_t)c);
    } وإلا إذا((uint32_t)c<=UCHAR_MAX_قدر) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        طالما((cs=*s++)!=0) {
            إذا(cs==lead && *s==trail) {
                أرجع (char16_t *)(s-1);
            }
        }
        أرجع خالي_اشارو;
    } وإلا {
        /* not a Unicode code point, not findable */
        أرجع خالي_اشارو;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(ثابت char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    إذا(count<=0) {
        أرجع خالي_اشارو; /* no string */
    } وإلا إذا(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        أرجع u_str_ڳوليو_پهريون(s, count, &c, 1);
    } وإلا {
        /* trivial search for a BMP code point */
        ثابت char16_t *حد_بندي=s+count;
        افعل {
            إذا(*s==c) {
                أرجع (char16_t *)s;
            }
        } طالما(++s!=حد_بندي);
        أرجع خالي_اشارو;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(ثابت char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    إذا((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        أرجع u_memchr(s, (char16_t)c, count);
    } وإلا إذا(count<2) {
        /* too short for a surrogate pair */
        أرجع خالي_اشارو;
    } وإلا إذا((uint32_t)c<=UCHAR_MAX_قدر) {
        /* find supplementary code point as surrogate pair */
        ثابت char16_t *حد_بندي=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        افعل {
            إذا(*s==lead && *(s+1)==trail) {
                أرجع (char16_t *)s;
            }
        } طالما(++s!=حد_بندي);
        أرجع خالي_اشارو;
    } وإلا {
        /* not a Unicode code point, not findable */
        أرجع خالي_اشارو;
    }
}

/* Backward binary string search functions ---------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_str_ڳوليو_آخري(ثابت char16_t* s U_LIFETIME_BOUND,
              int32_t length,
              ثابت char16_t* sub,
              int32_t sub_ڊيگهائي) {
    ثابت char16_t *شروعات, *حد_بندي, *p, *q, *sub_حد_بندي;
    char16_t c, cs;

    إذا(sub==خالي_اشارو || sub_ڊيگهائي<-1) {
        أرجع (char16_t *)s;
    }
    إذا(s==خالي_اشارو || length<-1) {
        أرجع خالي_اشارو;
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

    إذا(sub_ڊيگهائي<0) {
        sub_ڊيگهائي=u_strlen(sub);
    }
    إذا(sub_ڊيگهائي==0) {
        أرجع (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    sub_حد_بندي=sub+sub_ڊيگهائي;
    cs=*(--sub_حد_بندي);
    --sub_ڊيگهائي;

    إذا(sub_ڊيگهائي==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        أرجع length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    إذا(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    إذا(length<=sub_ڊيگهائي) {
        أرجع خالي_اشارو; /* s is shorter than sub */
    }

    شروعات=s;
    حد_بندي=s+length;

    /* the substring must start no later than s+subLength */
    s+=sub_ڊيگهائي;

    طالما(s!=حد_بندي) {
        c=*(--حد_بندي);
        إذا(c==cs) {
            /* found last substring char16_t, compare rest */
            p=حد_بندي;
            q=sub_حد_بندي;
            لكل(;;) {
                إذا(q==sub) {
                    إذا(is_ميلاپ_At_CPBoundary(شروعات, p, حد_بندي+1, شروعات+length)) {
                        أرجع (char16_t *)p; /* well-formed match */
                    } وإلا {
                        اكسر; /* no match because surrogate pair is split */
                    }
                }
                إذا(*(--p)!=*(--q)) {
                    اكسر; /* no match */
                }
            }
        }
    }

    /* not found */
    أرجع خالي_اشارو;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(ثابت char16_t* s U_LIFETIME_BOUND, ثابت char16_t* substring) {
    أرجع u_str_ڳوليو_آخري(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(ثابت char16_t* s U_LIFETIME_BOUND, char16_t c) {
    إذا(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        أرجع u_str_ڳوليو_آخري(s, -1, &c, 1);
    } وإلا {
        ثابت char16_t *نتيجو=خالي_اشارو;
        char16_t cs;

        /* trivial search for a BMP code point */
        لكل(;;) {
            إذا((cs=*s)==c) {
                نتيجو=s;
            }
            إذا(cs==0) {
                أرجع (char16_t *)نتيجو;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(ثابت char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    إذا((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        أرجع u_strrchr(s, (char16_t)c);
    } وإلا إذا((uint32_t)c<=UCHAR_MAX_قدر) {
        /* find supplementary code point as surrogate pair */
        ثابت char16_t *نتيجو=خالي_اشارو;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        طالما((cs=*s++)!=0) {
            إذا(cs==lead && *s==trail) {
                نتيجو=s-1;
            }
        }
        أرجع (char16_t *)نتيجو;
    } وإلا {
        /* not a Unicode code point, not findable */
        أرجع خالي_اشارو;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(ثابت char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    إذا(count<=0) {
        أرجع خالي_اشارو; /* no string */
    } وإلا إذا(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        أرجع u_str_ڳوليو_آخري(s, count, &c, 1);
    } وإلا {
        /* trivial search for a BMP code point */
        ثابت char16_t *حد_بندي=s+count;
        افعل {
            إذا(*(--حد_بندي)==c) {
                أرجع (char16_t *)حد_بندي;
            }
        } طالما(s!=حد_بندي);
        أرجع خالي_اشارو;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(ثابت char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    إذا((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        أرجع u_memrchr(s, (char16_t)c, count);
    } وإلا إذا(count<2) {
        /* too short for a surrogate pair */
        أرجع خالي_اشارو;
    } وإلا إذا((uint32_t)c<=UCHAR_MAX_قدر) {
        /* find supplementary code point as surrogate pair */
        ثابت char16_t *حد_بندي=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        افعل {
            إذا(*حد_بندي==trail && *(حد_بندي-1)==lead) {
                أرجع (char16_t *)(حد_بندي-1);
            }
        } طالما(s!=--حد_بندي);
        أرجع خالي_اشارو;
    } وإلا {
        /* not a Unicode code point, not findable */
        أرجع خالي_اشارو;
    }
}

/* Tokenization functions --------------------------------------------------- */

/*
 * Match each code point in a string against each code point in the matchSet.
 * Return the index of the first string code point that
 * is (polarity==true) or is not (false) contained in the matchSet.
 * Return -(string length)-1 if there is no such code point.
 */
ساكن int32_t
ميلاپ_From_سيٽ(ثابت char16_t *string, ثابت char16_t *ميلاپ_سيٽ, UBool polarity) {
    int32_t ميلاپ_Len, ميلاپ_BMPLen, strItr, ميلاپ_Itr;
    UChar32 stringCh, ميلاپ_Ch;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    ميلاپ_BMPLen = 0;
    طالما((c = ميلاپ_سيٽ[ميلاپ_BMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++ميلاپ_BMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    ميلاپ_Len = ميلاپ_BMPLen;
    طالما(ميلاپ_سيٽ[ميلاپ_Len] != 0) {
        ++ميلاپ_Len;
    }

    لكل(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        إذا(U16_IS_SINGLE(c)) {
            إذا(polarity) {
                لكل(ميلاپ_Itr = 0; ميلاپ_Itr < ميلاپ_Len; ++ميلاپ_Itr) {
                    إذا(c == ميلاپ_سيٽ[ميلاپ_Itr]) {
                        أرجع strItr - 1; /* one matches */
                    }
                }
            } وإلا {
                لكل(ميلاپ_Itr = 0; ميلاپ_Itr < ميلاپ_Len; ++ميلاپ_Itr) {
                    إذا(c == ميلاپ_سيٽ[ميلاپ_Itr]) {
                        اذهب endloop;
                    }
                }
                أرجع strItr - 1; /* none matches */
            }
        } وإلا {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            إذا(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } وإلا {
                stringCh = c; /* unpaired trail surrogate */
            }

            إذا(polarity) {
                لكل(ميلاپ_Itr = ميلاپ_BMPLen; ميلاپ_Itr < ميلاپ_Len;) {
                    U16_ايندڙ(ميلاپ_سيٽ, ميلاپ_Itr, ميلاپ_Len, ميلاپ_Ch);
                    إذا(stringCh == ميلاپ_Ch) {
                        أرجع strItr - U16_ڊيگهائي(stringCh); /* one matches */
                    }
                }
            } وإلا {
                لكل(ميلاپ_Itr = ميلاپ_BMPLen; ميلاپ_Itr < ميلاپ_Len;) {
                    U16_ايندڙ(ميلاپ_سيٽ, ميلاپ_Itr, ميلاپ_Len, ميلاپ_Ch);
                    إذا(stringCh == ميلاپ_Ch) {
                        اذهب endloop;
                    }
                }
                أرجع strItr - U16_ڊيگهائي(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    أرجع -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(ثابت char16_t* string U_LIFETIME_BOUND, ثابت char16_t* ميلاپ_سيٽ)
{
    int32_t idx = ميلاپ_From_سيٽ(string, ميلاپ_سيٽ, سچ);
    إذا(idx >= 0) {
        أرجع (char16_t *)string + idx;
    } وإلا {
        أرجع خالي_اشارو;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(ثابت char16_t *string, ثابت char16_t *ميلاپ_سيٽ)
{
    int32_t idx = ميلاپ_From_سيٽ(string, ميلاپ_سيٽ, سچ);
    إذا(idx >= 0) {
        أرجع idx;
    } وإلا {
        أرجع -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(ثابت char16_t *string, ثابت char16_t *ميلاپ_سيٽ)
{
    int32_t idx = ميلاپ_From_سيٽ(string, ميلاپ_سيٽ, خطأ);
    إذا(idx >= 0) {
        أرجع idx;
    } وإلا {
        أرجع -idx - 1; /* == u_strlen(string) */
    }
}

/* ----- Text manipulation functions --- */

U_CAPI char16_t* U_EXPORT2
u_strtok_r(char16_t* src U_LIFETIME_BOUND, ثابت char16_t* delim, char16_t** saveState)
{
    char16_t *tok_ذريعو;
    char16_t *ايندڙ_Token;
    uint32_t nonDelimIdx;

    /* If saveState is nullptr, the user messed up. */
    إذا (src != خالي_اشارو) {
        tok_ذريعو = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    وإلا إذا (*saveState) {
        tok_ذريعو = *saveState;
    }
    وإلا {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        أرجع خالي_اشارو;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tok_ذريعو, delim);
    tok_ذريعو = &tok_ذريعو[nonDelimIdx];

    إذا (*tok_ذريعو) {
        ايندڙ_Token = u_strpbrk(tok_ذريعو, delim);
        إذا (ايندڙ_Token != خالي_اشارو) {
            /* Create a token */
            *(ايندڙ_Token++) = 0;
            *saveState = ايندڙ_Token;
            أرجع tok_ذريعو;
        }
        وإلا إذا (*saveState) {
            /* Return the last token */
            *saveState = خالي_اشارو;
            أرجع tok_ذريعو;
        }
    }
    وإلا {
        /* No tokens were found. Only delimiters were left. */
        *saveState = خالي_اشارو;
    }
    أرجع خالي_اشارو;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, ثابت char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    طالما(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    طالما((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    أرجع anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, ثابت char16_t* src, int32_t n)
{
    إذا(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        طالما(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        طالما((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            إذا(--n == 0) {
                *dst = 0;
                اكسر;
            }
            ++src;
        }

        أرجع anchor;
    } وإلا {
        أرجع dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(ثابت char16_t *s1,
    ثابت char16_t *s2)
{
    char16_t  c1, c2;

    لكل(;;) {
        c1=*s1++;
        c2=*s2++;
        إذا (c1 != c2 || c1 == 0) {
            اكسر;
        }
    }
    أرجع (int32_t)c1 - (int32_t)c2;
}

U_CFUNC int32_t U_EXPORT2
uprv_strCompare(ثابت char16_t *s1, int32_t length1,
                ثابت char16_t *s2, int32_t length2,
                UBool strncmpStyle, UBool ڪوڊ_نقطو_ترتيب) {
    ثابت char16_t *start1, *start2, *limit1, *limit2;
    char16_t c1, c2;

    /* setup for fix-up */
    start1=s1;
    start2=s2;

    /* compare identical prefixes - they do not need to be fixed up */
    إذا(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        إذا(s1==s2) {
            أرجع 0;
        }

        لكل(;;) {
            c1=*s1;
            c2=*s2;
            إذا(c1!=c2) {
                اكسر;
            }
            إذا(c1==0) {
                أرجع 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=خالي_اشارو;
    } وإلا إذا(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        إذا(s1==s2) {
            أرجع 0;
        }

        limit1=start1+length1;

        لكل(;;) {
            /* both lengths are same, check only one limit */
            إذا(s1==limit1) {
                أرجع 0;
            }

            c1=*s1;
            c2=*s2;
            إذا(c1!=c2) {
                اكسر;
            }
            إذا(c1==0) {
                أرجع 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } وإلا {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t ڊيگهائي_نتيجو;

        إذا(length1<0) {
            length1=u_strlen(s1);
        }
        إذا(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        إذا(length1<length2) {
            ڊيگهائي_نتيجو=-1;
            limit1=start1+length1;
        } وإلا إذا(length1==length2) {
            ڊيگهائي_نتيجو=0;
            limit1=start1+length1;
        } وإلا /* length1>length2 */ {
            ڊيگهائي_نتيجو=1;
            limit1=start1+length2;
        }

        إذا(s1==s2) {
            أرجع ڊيگهائي_نتيجو;
        }

        لكل(;;) {
            /* check pseudo-limit */
            إذا(s1==limit1) {
                أرجع ڊيگهائي_نتيجو;
            }

            c1=*s1;
            c2=*s2;
            إذا(c1!=c2) {
                اكسر;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    إذا(c1>=0xd800 && c2>=0xd800 && ڪوڊ_نقطو_ترتيب) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        إذا(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } وإلا {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        إذا(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } وإلا {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    أرجع (int32_t)c1-(int32_t)c2;
}

/*
 * Compare two strings as presented by UCharIterators.
 * Use code unit or code point order.
 * When the function returns, it is undefined where the iterators
 * have stopped.
 */
U_CAPI int32_t U_EXPORT2
u_strCompareIter(UCharIterator *iter1, UCharIterator *iter2, UBool ڪوڊ_نقطو_ترتيب) {
    UChar32 c1, c2;

    /* argument checking */
    إذا(iter1==خالي_اشارو || iter2==خالي_اشارو) {
        أرجع 0; /* bad arguments */
    }
    إذا(iter1==iter2) {
        أرجع 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_شروعات);
    iter2->move(iter2, 0, UITER_شروعات);

    /* compare identical prefixes - they do not need to be fixed up */
    لكل(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        إذا(c1!=c2) {
            اكسر;
        }
        إذا(c1==-1) {
            أرجع 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    إذا(c1>=0xd800 && c2>=0xd800 && ڪوڊ_نقطو_ترتيب) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        إذا(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->اڳيون(iter1), U16_IS_LEAD(iter1->اڳيون(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } وإلا {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        إذا(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->اڳيون(iter2), U16_IS_LEAD(iter2->اڳيون(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } وإلا {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    أرجع (int32_t)c1-(int32_t)c2;
}

#إذا 0
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
فراغ fragment {
        /* iff a surrogate is part of a surrogate pair, leave >=d800 */
        إذا(c1<=0xdbff) {
            إذا(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } وإلا إذا(c1<=0xdfff) {
            int32_t idx=iter1->get_اشاريو(iter1, UITER_CURRENT);
            iter1->اڳيون(iter1); /* ==c1 */
            إذا(!U16_IS_LEAD(iter1->اڳيون(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } وإلا /* 0xe000<=c1<=0xffff */ {
            /* BMP code point - make <d800 */
            c1-=0x2800;
        }
}
#جيڪڏهن_پورو

U_CAPI int32_t U_EXPORT2
u_strCompare(ثابت char16_t *s1, int32_t length1,
             ثابت char16_t *s2, int32_t length2,
             UBool ڪوڊ_نقطو_ترتيب) {
    /* argument checking */
    إذا(s1==خالي_اشارو || length1<-1 || s2==خالي_اشارو || length2<-1) {
        أرجع 0;
    }
    أرجع uprv_strCompare(s1, length1, s2, length2, خطأ, ڪوڊ_نقطو_ترتيب);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmp_ڪوڊ_نقطو_ترتيب(ثابت char16_t *s1, ثابت char16_t *s2) {
    أرجع uprv_strCompare(s1, -1, s2, -1, خطأ, سچ);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(ثابت char16_t  *s1,
     ثابت char16_t  *s2,
     int32_t     n) 
{
    إذا(n > 0) {
        int32_t rc;
        لكل(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            إذا(rc != 0 || *s1 == 0 || --n == 0) {
                أرجع rc;
            }
            ++s1;
            ++s2;
        }
    } وإلا {
        أرجع 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmp_ڪوڊ_نقطو_ترتيب(ثابت char16_t *s1, ثابت char16_t *s2, int32_t n) {
    أرجع uprv_strCompare(s1, n, s2, n, سچ, سچ);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, ثابت char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    طالما((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    أرجع anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, ثابت char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    طالما(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    أرجع anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(ثابت char16_t *s)
{
#إذا U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    أرجع (int32_t)uprv_wcslen((ثابت wchar_t *)s);
#وإلا
    ثابت char16_t *t = s;
    طالما(*t != 0) {
      ++t;
    }
    أرجع t - s;
#جيڪڏهن_پورو
}

U_CAPI int32_t U_EXPORT2
u_ڳڻپ_Char32(ثابت char16_t *s, int32_t length) {
    int32_t count;

    إذا(s==خالي_اشارو || length<-1) {
        أرجع 0;
    }

    count=0;
    إذا(length>=0) {
        طالما(length>0) {
            ++count;
            إذا(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } وإلا {
                ++s;
                --length;
            }
        }
    } وإلا /* length==-1 */ {
        char16_t c;

        لكل(;;) {
            إذا((c=*s++)==0) {
                اكسر;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            إذا(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    أرجع count;
}

U_CAPI UBool U_EXPORT2
u_str_Has_وڌيڪ_Char32_Than(ثابت char16_t *s, int32_t length, int32_t number) {

    إذا(number<0) {
        أرجع سچ;
    }
    إذا(s==خالي_اشارو || length<-1) {
        أرجع خطأ;
    }

    إذا(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        لكل(;;) {
            إذا((c=*s++)==0) {
                أرجع خطأ;
            }
            إذا(number==0) {
                أرجع سچ;
            }
            إذا(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } وإلا {
        /* length>=0 known */
        ثابت char16_t *حد_بندي;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        إذا(((length+1)/2)>number) {
            أرجع سچ;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        إذا(maxSupplementary<=0) {
            أرجع خطأ;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        حد_بندي=s+length;
        لكل(;;) {
            إذا(s==حد_بندي) {
                أرجع خطأ;
            }
            إذا(number==0) {
                أرجع سچ;
            }
            إذا(U16_IS_LEAD(*s++) && s!=حد_بندي && U16_IS_TRAIL(*s)) {
                ++s;
                إذا(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    أرجع خطأ;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, ثابت char16_t* src, int32_t count) {
    إذا(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    أرجع dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, ثابت char16_t* src, int32_t count) {
    إذا(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    أرجع dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    إذا(count > 0) {
        char16_t *ptr = dest;
        char16_t *حد_بندي = dest + count;

        طالما (ptr < حد_بندي) {
            *(ptr++) = c;
        }
    }
    أرجع dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(ثابت char16_t *buf1, ثابت char16_t *buf2, int32_t count) {
    إذا(count > 0) {
        ثابت char16_t *حد_بندي = buf1 + count;
        int32_t نتيجو;

        طالما (buf1 < حد_بندي) {
            نتيجو = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            إذا (نتيجو != 0) {
                أرجع نتيجو;
            }
            buf1++;
            buf2++;
        }
    }
    أرجع 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmp_ڪوڊ_نقطو_ترتيب(ثابت char16_t *s1, ثابت char16_t *s2, int32_t count) {
    أرجع uprv_strCompare(s1, count, s2, count, خطأ, سچ);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
ساكن ثابت char16_t UNESCAPE_MAP[] = {
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
تعداد { UNESCAPE_MAP_ڊيگهائي = UPRV_LENGTHOF(UNESCAPE_MAP) };

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
ساكن int32_t _digit8(char16_t c) {
    إذا (c >= u'0' && c <= u'7') {
        أرجع c - u'0';
    }
    أرجع -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
ساكن int32_t _digit16(char16_t c) {
    إذا (c >= u'0' && c <= u'9') {
        أرجع c - u'0';
    }
    إذا (c >= u'A' && c <= u'F') {
        أرجع c - (u'A' - 10);
    }
    إذا (c >= u'a' && c <= u'f') {
        أرجع c - (u'a' - 10);
    }
    أرجع -1;
}

/* Parse a single escape sequence.  Although this method deals in
 * UChars, it does not use C++ or UnicodeString.  This allows it to
 * be used from C contexts. */
U_CAPI UChar32 U_EXPORT2
u_unescapeAt(UNESCAPE_CHAR_AT charAt,
             int32_t *وٿي,
             int32_t length,
             فراغ *context) {

    int32_t شروعات = *وٿي;
    UChar32 c;
    UChar32 نتيجو = 0;
    int8_t n = 0;
    int8_t minDig = 0;
    int8_t maxDig = 0;
    int8_t بٽ_في_Digit = 4; 
    int32_t dig;
    UBool braces = خطأ;

    /* Check that offset is in range */
    إذا (*وٿي < 0 || *وٿي >= length) {
        اذهب err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*وٿي)++, context);

    /* Convert hexadecimal and octal escapes */
    بدل (c) {
    حالة u'u':
        minDig = maxDig = 4;
        اكسر;
    حالة u'U':
        minDig = maxDig = 8;
        اكسر;
    حالة u'x':
        minDig = 1;
        إذا (*وٿي < length && charAt(*وٿي, context) == u'{') {
            ++(*وٿي);
            braces = سچ;
            maxDig = 8;
        } وإلا {
            maxDig = 2;
        }
        اكسر;
    افتراضي:
        dig = _digit8(c);
        إذا (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            بٽ_في_Digit = 3;
            نتيجو = dig;
        }
        اكسر;
    }
    إذا (minDig != 0) {
        طالما (*وٿي < length && n < maxDig) {
            c = charAt(*وٿي, context);
            dig = (بٽ_في_Digit == 3) ? _digit8(c) : _digit16(c);
            إذا (dig < 0) {
                اكسر;
            }
            نتيجو = (نتيجو << بٽ_في_Digit) | dig;
            ++(*وٿي);
            ++n;
        }
        إذا (n < minDig) {
            اذهب err;
        }
        إذا (braces) {
            إذا (c != u'}') {
                اذهب err;
            }
            ++(*وٿي);
        }
        إذا (نتيجو < 0 || نتيجو >= 0x110000) {
            اذهب err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        إذا (*وٿي < length && U16_IS_LEAD(نتيجو)) {
            int32_t ahead = *وٿي + 1;
            c = charAt(*وٿي, context);
            إذا (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t پڇ_حد_بندي = ahead + 11;
                إذا (پڇ_حد_بندي > length) {
                    پڇ_حد_بندي = length;
                }
                c = u_unescapeAt(charAt, &ahead, پڇ_حد_بندي, context);
            }
            إذا (U16_IS_TRAIL(c)) {
                *وٿي = ahead;
                نتيجو = U16_GET_SUPPLEMENTARY(نتيجو, c);
            }
        }
        أرجع نتيجو;
    }

    /* Convert C-style escapes in table */
    لكل (int32_t i=0; i<UNESCAPE_MAP_ڊيگهائي; i+=2) {
        إذا (c == UNESCAPE_MAP[i]) {
            أرجع UNESCAPE_MAP[i+1];
        } وإلا إذا (c < UNESCAPE_MAP[i]) {
            اكسر;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    إذا (c == u'c' && *وٿي < length) {
        c = charAt((*وٿي)++, context);
        إذا (U16_IS_LEAD(c) && *وٿي < length) {
            char16_t c2 = charAt(*وٿي, context);
            إذا (U16_IS_TRAIL(c2)) {
                ++(*وٿي);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        أرجع 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    إذا (U16_IS_LEAD(c) && *وٿي < length) {
        char16_t c2 = charAt(*وٿي, context);
        إذا (U16_IS_TRAIL(c2)) {
            ++(*وٿي);
            أرجع U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    أرجع c;

 err:
    /* Invalid escape sequence */
    *وٿي = شروعات; /* Reset to initial value */
    أرجع (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
ساكن char16_t U_CALLCONV
_charPtr_charAt(int32_t وٿي, فراغ *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_chars_منزل_UChars(static_cast<حرف*>(context) + وٿي, &c16, 1);
    أرجع c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
ساكن فراغ _appendUChars(char16_t *dest, int32_t dest_صلاحيت,
                          ثابت حرف *src, int32_t srcLen) {
    إذا (dest_صلاحيت < 0) {
        dest_صلاحيت = 0;
    }
    إذا (srcLen > dest_صلاحيت) {
        srcLen = dest_صلاحيت;
    }
    u_chars_منزل_UChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(ثابت حرف *src, char16_t *dest, int32_t dest_صلاحيت) {
    ثابت حرف *segment = src;
    int32_t i = 0;
    حرف c;

    طالما ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        إذا (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            إذا (src != segment) {
                إذا (dest != خالي_اشارو) {
                    _appendUChars(dest + i, dest_صلاحيت - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<حرف*>(src));
            إذا (lenParsed == 0) {
                اذهب err;
            }
            src += lenParsed; /* advance past escape seq. */
            إذا (dest != خالي_اشارو && U16_ڊيگهائي(c32) <= (dest_صلاحيت - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } وإلا {
                i += U16_ڊيگهائي(c32);
            }
            segment = src;
        } وإلا {
            ++src;
        }
    }
    إذا (src != segment) {
        إذا (dest != خالي_اشارو) {
            _appendUChars(dest + i, dest_صلاحيت - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    إذا (dest != خالي_اشارو && i < dest_صلاحيت) {
        dest[i] = 0;
    }
    أرجع i;

 err:
    إذا (dest != خالي_اشارو && dest_صلاحيت > 0) {
        *dest = 0;
    }
    أرجع 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#وصف_ڪريو __TERMINATE_STRING(dest, dest_صلاحيت, length, p_Error_ڪوڊ) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(p_Error_ڪوڊ!=خالي_اشارو && U_SUCCESS(*p_Error_ڪوڊ)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        إذا(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } وإلا إذا(length<dest_صلاحيت) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            إذا(*p_Error_ڪوڊ==U_STRING_NOT_TERMINATED_WARNING) {          \
                *p_Error_ڪوڊ=U_ZERO_ERROR;                               \
            }                                                           \
        } وإلا إذا(length==dest_صلاحيت) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *p_Error_ڪوڊ=U_STRING_NOT_TERMINATED_WARNING;                \
        } وإلا /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;                        \
        }                                                               \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

U_CAPI char16_t U_EXPORT2
u_ascii_منزل_مٿانهون(char16_t c) {
    إذا (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    أرجع c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t dest_صلاحيت, int32_t length, UError_ڪوڊ *p_Error_ڪوڊ) {
    __TERMINATE_STRING(dest, dest_صلاحيت, length, p_Error_ڪوڊ);
    أرجع length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(حرف *dest, int32_t dest_صلاحيت, int32_t length, UError_ڪوڊ *p_Error_ڪوڊ) {
    __TERMINATE_STRING(dest, dest_صلاحيت, length, p_Error_ڪوڊ);
    أرجع length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t dest_صلاحيت, int32_t length, UError_ڪوڊ *p_Error_ڪوڊ) {
    __TERMINATE_STRING(dest, dest_صلاحيت, length, p_Error_ڪوڊ);
    أرجع length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t dest_صلاحيت, int32_t length, UError_ڪوڊ *p_Error_ڪوڊ) {
    __TERMINATE_STRING(dest, dest_صلاحيت, length, p_Error_ڪوڊ);
    أرجع length;
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

#وصف_ڪريو STRING_HASH(TYPE, STR, STRLEN, DEREF) UPRV_بلاڪ_MACRO_BEGIN { \
    uint32_t hash = 0;                        \
    ثابت TYPE *p = (ثابت TYPE*) STR;        \
    إذا (p != خالي_اشارو) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        ثابت TYPE *حد_بندي = p + len;          \
        طالما (p<حد_بندي) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    أرجع static_cast<int32_t>(hash);        \
} UPRV_بلاڪ_MACRO_پڇاڙي

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
