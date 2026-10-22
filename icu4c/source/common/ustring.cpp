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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cwchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

/* ANSI string.h - style functions ------------------------------------------ */

/* U+ffff is the highest BMP code point, the highest one that fits into a 16-bit char16_t */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_BMP_MAX 0xffff

/* Forward binary string search functions ----------------------------------- */

/*
 * Test if a substring match inside a string is at code point boundaries.
 * All pointers refer to the same buffer.
 * The limit pointer may be nullptr, all others must be real pointers.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ UBool
is_ਮੇਲ_At_CPBoundary(ਅਚਲ char16_t *ਸ਼ੁਰੂ, ਅਚਲ char16_t *ਮੇਲ, ਅਚਲ char16_t *ਮੇਲ_ਹੱਦਬੰਦੀ, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ) {
    ਜੇ(U16_IS_TRAIL(*ਮੇਲ) && ਸ਼ੁਰੂ!=ਮੇਲ && U16_IS_LEAD(*(ਮੇਲ-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ(U16_IS_LEAD(*(ਮੇਲ_ਹੱਦਬੰਦੀ-1)) && ਮੇਲ_ਹੱਦਬੰਦੀ!=ਹੱਦਬੰਦੀ && U16_IS_TRAIL(*ਮੇਲ_ਹੱਦਬੰਦੀ)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        ਵਾਪਸ ਝੂਠ;
    }
    ਵਾਪਸ ਸੱਚ;
}

U_CAPI char16_t* U_EXPORT2
u_str_ਲੱਭੋ_ਪਹਿਲਾ(ਅਚਲ char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               ਅਚਲ char16_t* sub,
               int32_t sub_ਲੰਮਾਈ) {
    ਅਚਲ char16_t *ਸ਼ੁਰੂ, *p, *q, *sub_ਹੱਦਬੰਦੀ;
    char16_t c, cs, cq;

    ਜੇ(sub==ਖਾਲੀ_ਸੂਚਕ || sub_ਲੰਮਾਈ<-1) {
        ਵਾਪਸ (char16_t *)s;
    }
    ਜੇ(s==ਖਾਲੀ_ਸੂਚਕ || length<-1) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਸ਼ੁਰੂ=s;

    ਜੇ(length<0 && sub_ਲੰਮਾਈ<0) {
        /* both strings are NUL-terminated */
        ਜੇ((cs=*sub++)==0) {
            ਵਾਪਸ (char16_t *)s;
        }
        ਜੇ(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            ਵਾਪਸ u_strchr(s, cs);
        }

        ਜਦੋਂ((c=*s++)!=0) {
            ਜੇ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                ਲਈ(;;) {
                    ਜੇ((cq=*q)==0) {
                        ਜੇ(is_ਮੇਲ_At_CPBoundary(ਸ਼ੁਰੂ, s-1, p, ਖਾਲੀ_ਸੂਚਕ)) {
                            ਵਾਪਸ (char16_t *)(s-1); /* well-formed match */
                        } ਨਹੀਂਤਾਂ {
                            ਰੋਕੋ; /* no match because surrogate pair is split */
                        }
                    }
                    ਜੇ((c=*p)==0) {
                        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; /* no match, and none possible after s */
                    }
                    ਜੇ(c!=cq) {
                        ਰੋਕੋ; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ(sub_ਲੰਮਾਈ<0) {
        sub_ਲੰਮਾਈ=u_strlen(sub);
    }
    ਜੇ(sub_ਲੰਮਾਈ==0) {
        ਵਾਪਸ (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --sub_ਲੰਮਾਈ;
    sub_ਹੱਦਬੰਦੀ=sub+sub_ਲੰਮਾਈ;

    ਜੇ(sub_ਲੰਮਾਈ==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        ਵਾਪਸ length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    ਜੇ(length<0) {
        /* s is NUL-terminated */
        ਜਦੋਂ((c=*s++)!=0) {
            ਜੇ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                ਲਈ(;;) {
                    ਜੇ(q==sub_ਹੱਦਬੰਦੀ) {
                        ਜੇ(is_ਮੇਲ_At_CPBoundary(ਸ਼ੁਰੂ, s-1, p, ਖਾਲੀ_ਸੂਚਕ)) {
                            ਵਾਪਸ (char16_t *)(s-1); /* well-formed match */
                        } ਨਹੀਂਤਾਂ {
                            ਰੋਕੋ; /* no match because surrogate pair is split */
                        }
                    }
                    ਜੇ((c=*p)==0) {
                        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; /* no match, and none possible after s */
                    }
                    ਜੇ(c!=*q) {
                        ਰੋਕੋ; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } ਨਹੀਂਤਾਂ {
        ਅਚਲ char16_t *ਹੱਦਬੰਦੀ, *pre_ਹੱਦਬੰਦੀ;

        /* subLength was decremented above */
        ਜੇ(length<=sub_ਲੰਮਾਈ) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; /* s is shorter than sub */
        }

        ਹੱਦਬੰਦੀ=s+length;

        /* the substring must start before preLimit */
        pre_ਹੱਦਬੰਦੀ=ਹੱਦਬੰਦੀ-sub_ਲੰਮਾਈ;

        ਜਦੋਂ(s!=pre_ਹੱਦਬੰਦੀ) {
            c=*s++;
            ਜੇ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                ਲਈ(;;) {
                    ਜੇ(q==sub_ਹੱਦਬੰਦੀ) {
                        ਜੇ(is_ਮੇਲ_At_CPBoundary(ਸ਼ੁਰੂ, s-1, p, ਹੱਦਬੰਦੀ)) {
                            ਵਾਪਸ (char16_t *)(s-1); /* well-formed match */
                        } ਨਹੀਂਤਾਂ {
                            ਰੋਕੋ; /* no match because surrogate pair is split */
                        }
                    }
                    ਜੇ(*p!=*q) {
                        ਰੋਕੋ; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(ਅਚਲ char16_t* s U_LIFETIME_BOUND, ਅਚਲ char16_t* substring) {
    ਵਾਪਸ u_str_ਲੱਭੋ_ਪਹਿਲਾ(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(ਅਚਲ char16_t* s U_LIFETIME_BOUND, char16_t c) {
    ਜੇ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ਵਾਪਸ u_str_ਲੱਭੋ_ਪਹਿਲਾ(s, -1, &c, 1);
    } ਨਹੀਂਤਾਂ {
        char16_t cs;

        /* trivial search for a BMP code point */
        ਲਈ(;;) {
            ਜੇ((cs=*s)==c) {
                ਵਾਪਸ (char16_t *)s;
            }
            ਜੇ(cs==0) {
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(ਅਚਲ char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    ਜੇ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ਵਾਪਸ u_strchr(s, (char16_t)c);
    } ਨਹੀਂਤਾਂ ਜੇ((uint32_t)c<=UCHAR_MAX_ਮੁੱਲ) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ਜਦੋਂ((cs=*s++)!=0) {
            ਜੇ(cs==lead && *s==trail) {
                ਵਾਪਸ (char16_t *)(s-1);
            }
        }
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
        /* not a Unicode code point, not findable */
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(ਅਚਲ char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ਜੇ(count<=0) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; /* no string */
    } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ਵਾਪਸ u_str_ਲੱਭੋ_ਪਹਿਲਾ(s, count, &c, 1);
    } ਨਹੀਂਤਾਂ {
        /* trivial search for a BMP code point */
        ਅਚਲ char16_t *ਹੱਦਬੰਦੀ=s+count;
        ਕਰੋ {
            ਜੇ(*s==c) {
                ਵਾਪਸ (char16_t *)s;
            }
        } ਜਦੋਂ(++s!=ਹੱਦਬੰਦੀ);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(ਅਚਲ char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    ਜੇ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ਵਾਪਸ u_memchr(s, (char16_t)c, count);
    } ਨਹੀਂਤਾਂ ਜੇ(count<2) {
        /* too short for a surrogate pair */
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ ਜੇ((uint32_t)c<=UCHAR_MAX_ਮੁੱਲ) {
        /* find supplementary code point as surrogate pair */
        ਅਚਲ char16_t *ਹੱਦਬੰਦੀ=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ਕਰੋ {
            ਜੇ(*s==lead && *(s+1)==trail) {
                ਵਾਪਸ (char16_t *)s;
            }
        } ਜਦੋਂ(++s!=ਹੱਦਬੰਦੀ);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
        /* not a Unicode code point, not findable */
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

/* Backward binary string search functions ---------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_str_ਲੱਭੋ_ਆਖਰੀ(ਅਚਲ char16_t* s U_LIFETIME_BOUND,
              int32_t length,
              ਅਚਲ char16_t* sub,
              int32_t sub_ਲੰਮਾਈ) {
    ਅਚਲ char16_t *ਸ਼ੁਰੂ, *ਹੱਦਬੰਦੀ, *p, *q, *sub_ਹੱਦਬੰਦੀ;
    char16_t c, cs;

    ਜੇ(sub==ਖਾਲੀ_ਸੂਚਕ || sub_ਲੰਮਾਈ<-1) {
        ਵਾਪਸ (char16_t *)s;
    }
    ਜੇ(s==ਖਾਲੀ_ਸੂਚਕ || length<-1) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
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

    ਜੇ(sub_ਲੰਮਾਈ<0) {
        sub_ਲੰਮਾਈ=u_strlen(sub);
    }
    ਜੇ(sub_ਲੰਮਾਈ==0) {
        ਵਾਪਸ (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    sub_ਹੱਦਬੰਦੀ=sub+sub_ਲੰਮਾਈ;
    cs=*(--sub_ਹੱਦਬੰਦੀ);
    --sub_ਲੰਮਾਈ;

    ਜੇ(sub_ਲੰਮਾਈ==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        ਵਾਪਸ length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    ਜੇ(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    ਜੇ(length<=sub_ਲੰਮਾਈ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; /* s is shorter than sub */
    }

    ਸ਼ੁਰੂ=s;
    ਹੱਦਬੰਦੀ=s+length;

    /* the substring must start no later than s+subLength */
    s+=sub_ਲੰਮਾਈ;

    ਜਦੋਂ(s!=ਹੱਦਬੰਦੀ) {
        c=*(--ਹੱਦਬੰਦੀ);
        ਜੇ(c==cs) {
            /* found last substring char16_t, compare rest */
            p=ਹੱਦਬੰਦੀ;
            q=sub_ਹੱਦਬੰਦੀ;
            ਲਈ(;;) {
                ਜੇ(q==sub) {
                    ਜੇ(is_ਮੇਲ_At_CPBoundary(ਸ਼ੁਰੂ, p, ਹੱਦਬੰਦੀ+1, ਸ਼ੁਰੂ+length)) {
                        ਵਾਪਸ (char16_t *)p; /* well-formed match */
                    } ਨਹੀਂਤਾਂ {
                        ਰੋਕੋ; /* no match because surrogate pair is split */
                    }
                }
                ਜੇ(*(--p)!=*(--q)) {
                    ਰੋਕੋ; /* no match */
                }
            }
        }
    }

    /* not found */
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(ਅਚਲ char16_t* s U_LIFETIME_BOUND, ਅਚਲ char16_t* substring) {
    ਵਾਪਸ u_str_ਲੱਭੋ_ਆਖਰੀ(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(ਅਚਲ char16_t* s U_LIFETIME_BOUND, char16_t c) {
    ਜੇ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ਵਾਪਸ u_str_ਲੱਭੋ_ਆਖਰੀ(s, -1, &c, 1);
    } ਨਹੀਂਤਾਂ {
        ਅਚਲ char16_t *ਨਤੀਜਾ=ਖਾਲੀ_ਸੂਚਕ;
        char16_t cs;

        /* trivial search for a BMP code point */
        ਲਈ(;;) {
            ਜੇ((cs=*s)==c) {
                ਨਤੀਜਾ=s;
            }
            ਜੇ(cs==0) {
                ਵਾਪਸ (char16_t *)ਨਤੀਜਾ;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(ਅਚਲ char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    ਜੇ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ਵਾਪਸ u_strrchr(s, (char16_t)c);
    } ਨਹੀਂਤਾਂ ਜੇ((uint32_t)c<=UCHAR_MAX_ਮੁੱਲ) {
        /* find supplementary code point as surrogate pair */
        ਅਚਲ char16_t *ਨਤੀਜਾ=ਖਾਲੀ_ਸੂਚਕ;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ਜਦੋਂ((cs=*s++)!=0) {
            ਜੇ(cs==lead && *s==trail) {
                ਨਤੀਜਾ=s-1;
            }
        }
        ਵਾਪਸ (char16_t *)ਨਤੀਜਾ;
    } ਨਹੀਂਤਾਂ {
        /* not a Unicode code point, not findable */
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(ਅਚਲ char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ਜੇ(count<=0) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; /* no string */
    } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ਵਾਪਸ u_str_ਲੱਭੋ_ਆਖਰੀ(s, count, &c, 1);
    } ਨਹੀਂਤਾਂ {
        /* trivial search for a BMP code point */
        ਅਚਲ char16_t *ਹੱਦਬੰਦੀ=s+count;
        ਕਰੋ {
            ਜੇ(*(--ਹੱਦਬੰਦੀ)==c) {
                ਵਾਪਸ (char16_t *)ਹੱਦਬੰਦੀ;
            }
        } ਜਦੋਂ(s!=ਹੱਦਬੰਦੀ);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(ਅਚਲ char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    ਜੇ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ਵਾਪਸ u_memrchr(s, (char16_t)c, count);
    } ਨਹੀਂਤਾਂ ਜੇ(count<2) {
        /* too short for a surrogate pair */
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ ਜੇ((uint32_t)c<=UCHAR_MAX_ਮੁੱਲ) {
        /* find supplementary code point as surrogate pair */
        ਅਚਲ char16_t *ਹੱਦਬੰਦੀ=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ਕਰੋ {
            ਜੇ(*ਹੱਦਬੰਦੀ==trail && *(ਹੱਦਬੰਦੀ-1)==lead) {
                ਵਾਪਸ (char16_t *)(ਹੱਦਬੰਦੀ-1);
            }
        } ਜਦੋਂ(s!=--ਹੱਦਬੰਦੀ);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
        /* not a Unicode code point, not findable */
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

/* Tokenization functions --------------------------------------------------- */

/*
 * Match each code point in a string against each code point in the matchSet.
 * Return the index of the first string code point that
 * is (polarity==true) or is not (false) contained in the matchSet.
 * Return -(string length)-1 if there is no such code point.
 */
ਸਥਿਰ int32_t
ਮੇਲ_From_ਸੈੱਟ(ਅਚਲ char16_t *string, ਅਚਲ char16_t *ਮੇਲ_ਸੈੱਟ, UBool polarity) {
    int32_t ਮੇਲ_Len, ਮੇਲ_BMPLen, strItr, ਮੇਲ_Itr;
    UChar32 stringCh, ਮੇਲ_Ch;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    ਮੇਲ_BMPLen = 0;
    ਜਦੋਂ((c = ਮੇਲ_ਸੈੱਟ[ਮੇਲ_BMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++ਮੇਲ_BMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    ਮੇਲ_Len = ਮੇਲ_BMPLen;
    ਜਦੋਂ(ਮੇਲ_ਸੈੱਟ[ਮੇਲ_Len] != 0) {
        ++ਮੇਲ_Len;
    }

    ਲਈ(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        ਜੇ(U16_IS_SINGLE(c)) {
            ਜੇ(polarity) {
                ਲਈ(ਮੇਲ_Itr = 0; ਮੇਲ_Itr < ਮੇਲ_Len; ++ਮੇਲ_Itr) {
                    ਜੇ(c == ਮੇਲ_ਸੈੱਟ[ਮੇਲ_Itr]) {
                        ਵਾਪਸ strItr - 1; /* one matches */
                    }
                }
            } ਨਹੀਂਤਾਂ {
                ਲਈ(ਮੇਲ_Itr = 0; ਮੇਲ_Itr < ਮੇਲ_Len; ++ਮੇਲ_Itr) {
                    ਜੇ(c == ਮੇਲ_ਸੈੱਟ[ਮੇਲ_Itr]) {
                        ਜਾਓ endloop;
                    }
                }
                ਵਾਪਸ strItr - 1; /* none matches */
            }
        } ਨਹੀਂਤਾਂ {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            ਜੇ(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } ਨਹੀਂਤਾਂ {
                stringCh = c; /* unpaired trail surrogate */
            }

            ਜੇ(polarity) {
                ਲਈ(ਮੇਲ_Itr = ਮੇਲ_BMPLen; ਮੇਲ_Itr < ਮੇਲ_Len;) {
                    U16_ਅਗਲਾ(ਮੇਲ_ਸੈੱਟ, ਮੇਲ_Itr, ਮੇਲ_Len, ਮੇਲ_Ch);
                    ਜੇ(stringCh == ਮੇਲ_Ch) {
                        ਵਾਪਸ strItr - U16_ਲੰਮਾਈ(stringCh); /* one matches */
                    }
                }
            } ਨਹੀਂਤਾਂ {
                ਲਈ(ਮੇਲ_Itr = ਮੇਲ_BMPLen; ਮੇਲ_Itr < ਮੇਲ_Len;) {
                    U16_ਅਗਲਾ(ਮੇਲ_ਸੈੱਟ, ਮੇਲ_Itr, ਮੇਲ_Len, ਮੇਲ_Ch);
                    ਜੇ(stringCh == ਮੇਲ_Ch) {
                        ਜਾਓ endloop;
                    }
                }
                ਵਾਪਸ strItr - U16_ਲੰਮਾਈ(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    ਵਾਪਸ -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(ਅਚਲ char16_t* string U_LIFETIME_BOUND, ਅਚਲ char16_t* ਮੇਲ_ਸੈੱਟ)
{
    int32_t idx = ਮੇਲ_From_ਸੈੱਟ(string, ਮੇਲ_ਸੈੱਟ, ਸੱਚ);
    ਜੇ(idx >= 0) {
        ਵਾਪਸ (char16_t *)string + idx;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(ਅਚਲ char16_t *string, ਅਚਲ char16_t *ਮੇਲ_ਸੈੱਟ)
{
    int32_t idx = ਮੇਲ_From_ਸੈੱਟ(string, ਮੇਲ_ਸੈੱਟ, ਸੱਚ);
    ਜੇ(idx >= 0) {
        ਵਾਪਸ idx;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(ਅਚਲ char16_t *string, ਅਚਲ char16_t *ਮੇਲ_ਸੈੱਟ)
{
    int32_t idx = ਮੇਲ_From_ਸੈੱਟ(string, ਮੇਲ_ਸੈੱਟ, ਝੂਠ);
    ਜੇ(idx >= 0) {
        ਵਾਪਸ idx;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ -idx - 1; /* == u_strlen(string) */
    }
}

/* ----- Text manipulation functions --- */

U_CAPI char16_t* U_EXPORT2
u_strtok_r(char16_t* src U_LIFETIME_BOUND, ਅਚਲ char16_t* delim, char16_t** saveState)
{
    char16_t *tok_ਸਰੋਤ;
    char16_t *ਅਗਲਾ_Token;
    uint32_t nonDelimIdx;

    /* If saveState is nullptr, the user messed up. */
    ਜੇ (src != ਖਾਲੀ_ਸੂਚਕ) {
        tok_ਸਰੋਤ = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    ਨਹੀਂਤਾਂ ਜੇ (*saveState) {
        tok_ਸਰੋਤ = *saveState;
    }
    ਨਹੀਂਤਾਂ {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tok_ਸਰੋਤ, delim);
    tok_ਸਰੋਤ = &tok_ਸਰੋਤ[nonDelimIdx];

    ਜੇ (*tok_ਸਰੋਤ) {
        ਅਗਲਾ_Token = u_strpbrk(tok_ਸਰੋਤ, delim);
        ਜੇ (ਅਗਲਾ_Token != ਖਾਲੀ_ਸੂਚਕ) {
            /* Create a token */
            *(ਅਗਲਾ_Token++) = 0;
            *saveState = ਅਗਲਾ_Token;
            ਵਾਪਸ tok_ਸਰੋਤ;
        }
        ਨਹੀਂਤਾਂ ਜੇ (*saveState) {
            /* Return the last token */
            *saveState = ਖਾਲੀ_ਸੂਚਕ;
            ਵਾਪਸ tok_ਸਰੋਤ;
        }
    }
    ਨਹੀਂਤਾਂ {
        /* No tokens were found. Only delimiters were left. */
        *saveState = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, ਅਚਲ char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    ਜਦੋਂ(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    ਜਦੋਂ((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    ਵਾਪਸ anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, ਅਚਲ char16_t* src, int32_t n)
{
    ਜੇ(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        ਜਦੋਂ(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        ਜਦੋਂ((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            ਜੇ(--n == 0) {
                *dst = 0;
                ਰੋਕੋ;
            }
            ++src;
        }

        ਵਾਪਸ anchor;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(ਅਚਲ char16_t *s1,
    ਅਚਲ char16_t *s2)
{
    char16_t  c1, c2;

    ਲਈ(;;) {
        c1=*s1++;
        c2=*s2++;
        ਜੇ (c1 != c2 || c1 == 0) {
            ਰੋਕੋ;
        }
    }
    ਵਾਪਸ (int32_t)c1 - (int32_t)c2;
}

U_CFUNC int32_t U_EXPORT2
uprv_strCompare(ਅਚਲ char16_t *s1, int32_t length1,
                ਅਚਲ char16_t *s2, int32_t length2,
                UBool strncmpStyle, UBool ਕੋਡ_ਬਿੰਦੂ_ਕ੍ਰਮ) {
    ਅਚਲ char16_t *start1, *start2, *limit1, *limit2;
    char16_t c1, c2;

    /* setup for fix-up */
    start1=s1;
    start2=s2;

    /* compare identical prefixes - they do not need to be fixed up */
    ਜੇ(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        ਜੇ(s1==s2) {
            ਵਾਪਸ 0;
        }

        ਲਈ(;;) {
            c1=*s1;
            c2=*s2;
            ਜੇ(c1!=c2) {
                ਰੋਕੋ;
            }
            ਜੇ(c1==0) {
                ਵਾਪਸ 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ ਜੇ(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        ਜੇ(s1==s2) {
            ਵਾਪਸ 0;
        }

        limit1=start1+length1;

        ਲਈ(;;) {
            /* both lengths are same, check only one limit */
            ਜੇ(s1==limit1) {
                ਵਾਪਸ 0;
            }

            c1=*s1;
            c2=*s2;
            ਜੇ(c1!=c2) {
                ਰੋਕੋ;
            }
            ਜੇ(c1==0) {
                ਵਾਪਸ 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } ਨਹੀਂਤਾਂ {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t ਲੰਮਾਈ_ਨਤੀਜਾ;

        ਜੇ(length1<0) {
            length1=u_strlen(s1);
        }
        ਜੇ(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        ਜੇ(length1<length2) {
            ਲੰਮਾਈ_ਨਤੀਜਾ=-1;
            limit1=start1+length1;
        } ਨਹੀਂਤਾਂ ਜੇ(length1==length2) {
            ਲੰਮਾਈ_ਨਤੀਜਾ=0;
            limit1=start1+length1;
        } ਨਹੀਂਤਾਂ /* length1>length2 */ {
            ਲੰਮਾਈ_ਨਤੀਜਾ=1;
            limit1=start1+length2;
        }

        ਜੇ(s1==s2) {
            ਵਾਪਸ ਲੰਮਾਈ_ਨਤੀਜਾ;
        }

        ਲਈ(;;) {
            /* check pseudo-limit */
            ਜੇ(s1==limit1) {
                ਵਾਪਸ ਲੰਮਾਈ_ਨਤੀਜਾ;
            }

            c1=*s1;
            c2=*s2;
            ਜੇ(c1!=c2) {
                ਰੋਕੋ;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    ਜੇ(c1>=0xd800 && c2>=0xd800 && ਕੋਡ_ਬਿੰਦੂ_ਕ੍ਰਮ) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        ਜੇ(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ਨਹੀਂਤਾਂ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        ਜੇ(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ਨਹੀਂਤਾਂ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    ਵਾਪਸ (int32_t)c1-(int32_t)c2;
}

/*
 * Compare two strings as presented by UCharIterators.
 * Use code unit or code point order.
 * When the function returns, it is undefined where the iterators
 * have stopped.
 */
U_CAPI int32_t U_EXPORT2
u_strCompareIter(UCharIterator *iter1, UCharIterator *iter2, UBool ਕੋਡ_ਬਿੰਦੂ_ਕ੍ਰਮ) {
    UChar32 c1, c2;

    /* argument checking */
    ਜੇ(iter1==ਖਾਲੀ_ਸੂਚਕ || iter2==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ 0; /* bad arguments */
    }
    ਜੇ(iter1==iter2) {
        ਵਾਪਸ 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_ਸ਼ੁਰੂ);
    iter2->move(iter2, 0, UITER_ਸ਼ੁਰੂ);

    /* compare identical prefixes - they do not need to be fixed up */
    ਲਈ(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        ਜੇ(c1!=c2) {
            ਰੋਕੋ;
        }
        ਜੇ(c1==-1) {
            ਵਾਪਸ 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    ਜੇ(c1>=0xd800 && c2>=0xd800 && ਕੋਡ_ਬਿੰਦੂ_ਕ੍ਰਮ) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        ਜੇ(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->ਪਿਛਲਾ(iter1), U16_IS_LEAD(iter1->ਪਿਛਲਾ(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ਨਹੀਂਤਾਂ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        ਜੇ(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->ਪਿਛਲਾ(iter2), U16_IS_LEAD(iter2->ਪਿਛਲਾ(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ਨਹੀਂਤਾਂ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    ਵਾਪਸ (int32_t)c1-(int32_t)c2;
}

#ਜੇ 0
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
ਖਾਲੀ fragment {
        /* iff a surrogate is part of a surrogate pair, leave >=d800 */
        ਜੇ(c1<=0xdbff) {
            ਜੇ(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(c1<=0xdfff) {
            int32_t idx=iter1->get_ਸੂਚਕਾਂਕ(iter1, UITER_CURRENT);
            iter1->ਪਿਛਲਾ(iter1); /* ==c1 */
            ਜੇ(!U16_IS_LEAD(iter1->ਪਿਛਲਾ(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } ਨਹੀਂਤਾਂ /* 0xe000<=c1<=0xffff */ {
            /* BMP code point - make <d800 */
            c1-=0x2800;
        }
}
#ਜੇ_ਅੰਤ

U_CAPI int32_t U_EXPORT2
u_strCompare(ਅਚਲ char16_t *s1, int32_t length1,
             ਅਚਲ char16_t *s2, int32_t length2,
             UBool ਕੋਡ_ਬਿੰਦੂ_ਕ੍ਰਮ) {
    /* argument checking */
    ਜੇ(s1==ਖਾਲੀ_ਸੂਚਕ || length1<-1 || s2==ਖਾਲੀ_ਸੂਚਕ || length2<-1) {
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ uprv_strCompare(s1, length1, s2, length2, ਝੂਠ, ਕੋਡ_ਬਿੰਦੂ_ਕ੍ਰਮ);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmp_ਕੋਡ_ਬਿੰਦੂ_ਕ੍ਰਮ(ਅਚਲ char16_t *s1, ਅਚਲ char16_t *s2) {
    ਵਾਪਸ uprv_strCompare(s1, -1, s2, -1, ਝੂਠ, ਸੱਚ);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(ਅਚਲ char16_t  *s1,
     ਅਚਲ char16_t  *s2,
     int32_t     n) 
{
    ਜੇ(n > 0) {
        int32_t rc;
        ਲਈ(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            ਜੇ(rc != 0 || *s1 == 0 || --n == 0) {
                ਵਾਪਸ rc;
            }
            ++s1;
            ++s2;
        }
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmp_ਕੋਡ_ਬਿੰਦੂ_ਕ੍ਰਮ(ਅਚਲ char16_t *s1, ਅਚਲ char16_t *s2, int32_t n) {
    ਵਾਪਸ uprv_strCompare(s1, n, s2, n, ਸੱਚ, ਸੱਚ);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, ਅਚਲ char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    ਜਦੋਂ((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    ਵਾਪਸ anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, ਅਚਲ char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    ਜਦੋਂ(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    ਵਾਪਸ anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(ਅਚਲ char16_t *s)
{
#ਜੇ U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    ਵਾਪਸ (int32_t)uprv_wcslen((ਅਚਲ wchar_t *)s);
#ਨਹੀਂਤਾਂ
    ਅਚਲ char16_t *t = s;
    ਜਦੋਂ(*t != 0) {
      ++t;
    }
    ਵਾਪਸ t - s;
#ਜੇ_ਅੰਤ
}

U_CAPI int32_t U_EXPORT2
u_ਗਿਣਤੀ_Char32(ਅਚਲ char16_t *s, int32_t length) {
    int32_t count;

    ਜੇ(s==ਖਾਲੀ_ਸੂਚਕ || length<-1) {
        ਵਾਪਸ 0;
    }

    count=0;
    ਜੇ(length>=0) {
        ਜਦੋਂ(length>0) {
            ++count;
            ਜੇ(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } ਨਹੀਂਤਾਂ {
                ++s;
                --length;
            }
        }
    } ਨਹੀਂਤਾਂ /* length==-1 */ {
        char16_t c;

        ਲਈ(;;) {
            ਜੇ((c=*s++)==0) {
                ਰੋਕੋ;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            ਜੇ(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    ਵਾਪਸ count;
}

U_CAPI UBool U_EXPORT2
u_str_Has_ਹੋਰ_Char32_Than(ਅਚਲ char16_t *s, int32_t length, int32_t number) {

    ਜੇ(number<0) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ(s==ਖਾਲੀ_ਸੂਚਕ || length<-1) {
        ਵਾਪਸ ਝੂਠ;
    }

    ਜੇ(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        ਲਈ(;;) {
            ਜੇ((c=*s++)==0) {
                ਵਾਪਸ ਝੂਠ;
            }
            ਜੇ(number==0) {
                ਵਾਪਸ ਸੱਚ;
            }
            ਜੇ(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } ਨਹੀਂਤਾਂ {
        /* length>=0 known */
        ਅਚਲ char16_t *ਹੱਦਬੰਦੀ;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        ਜੇ(((length+1)/2)>number) {
            ਵਾਪਸ ਸੱਚ;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        ਜੇ(maxSupplementary<=0) {
            ਵਾਪਸ ਝੂਠ;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        ਹੱਦਬੰਦੀ=s+length;
        ਲਈ(;;) {
            ਜੇ(s==ਹੱਦਬੰਦੀ) {
                ਵਾਪਸ ਝੂਠ;
            }
            ਜੇ(number==0) {
                ਵਾਪਸ ਸੱਚ;
            }
            ਜੇ(U16_IS_LEAD(*s++) && s!=ਹੱਦਬੰਦੀ && U16_IS_TRAIL(*s)) {
                ++s;
                ਜੇ(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    ਵਾਪਸ ਝੂਠ;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, ਅਚਲ char16_t* src, int32_t count) {
    ਜੇ(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    ਵਾਪਸ dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, ਅਚਲ char16_t* src, int32_t count) {
    ਜੇ(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    ਵਾਪਸ dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ਜੇ(count > 0) {
        char16_t *ptr = dest;
        char16_t *ਹੱਦਬੰਦੀ = dest + count;

        ਜਦੋਂ (ptr < ਹੱਦਬੰਦੀ) {
            *(ptr++) = c;
        }
    }
    ਵਾਪਸ dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(ਅਚਲ char16_t *buf1, ਅਚਲ char16_t *buf2, int32_t count) {
    ਜੇ(count > 0) {
        ਅਚਲ char16_t *ਹੱਦਬੰਦੀ = buf1 + count;
        int32_t ਨਤੀਜਾ;

        ਜਦੋਂ (buf1 < ਹੱਦਬੰਦੀ) {
            ਨਤੀਜਾ = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            ਜੇ (ਨਤੀਜਾ != 0) {
                ਵਾਪਸ ਨਤੀਜਾ;
            }
            buf1++;
            buf2++;
        }
    }
    ਵਾਪਸ 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmp_ਕੋਡ_ਬਿੰਦੂ_ਕ੍ਰਮ(ਅਚਲ char16_t *s1, ਅਚਲ char16_t *s2, int32_t count) {
    ਵਾਪਸ uprv_strCompare(s1, count, s2, count, ਝੂਠ, ਸੱਚ);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
ਸਥਿਰ ਅਚਲ char16_t UNESCAPE_MAP[] = {
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
ਗਿਣਤੀ { UNESCAPE_MAP_ਲੰਮਾਈ = UPRV_LENGTHOF(UNESCAPE_MAP) };

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
ਸਥਿਰ int32_t _digit8(char16_t c) {
    ਜੇ (c >= u'0' && c <= u'7') {
        ਵਾਪਸ c - u'0';
    }
    ਵਾਪਸ -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
ਸਥਿਰ int32_t _digit16(char16_t c) {
    ਜੇ (c >= u'0' && c <= u'9') {
        ਵਾਪਸ c - u'0';
    }
    ਜੇ (c >= u'A' && c <= u'F') {
        ਵਾਪਸ c - (u'A' - 10);
    }
    ਜੇ (c >= u'a' && c <= u'f') {
        ਵਾਪਸ c - (u'a' - 10);
    }
    ਵਾਪਸ -1;
}

/* Parse a single escape sequence.  Although this method deals in
 * UChars, it does not use C++ or UnicodeString.  This allows it to
 * be used from C contexts. */
U_CAPI UChar32 U_EXPORT2
u_unescapeAt(UNESCAPE_CHAR_AT charAt,
             int32_t *ਵਿੱਥ,
             int32_t length,
             ਖਾਲੀ *context) {

    int32_t ਸ਼ੁਰੂ = *ਵਿੱਥ;
    UChar32 c;
    UChar32 ਨਤੀਜਾ = 0;
    int8_t n = 0;
    int8_t minDig = 0;
    int8_t maxDig = 0;
    int8_t ਬਿੱਟ_ਪ੍ਰਤੀ_Digit = 4; 
    int32_t dig;
    UBool braces = ਝੂਠ;

    /* Check that offset is in range */
    ਜੇ (*ਵਿੱਥ < 0 || *ਵਿੱਥ >= length) {
        ਜਾਓ err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*ਵਿੱਥ)++, context);

    /* Convert hexadecimal and octal escapes */
    ਚੋਣ (c) {
    ਹਾਲਤ u'u':
        minDig = maxDig = 4;
        ਰੋਕੋ;
    ਹਾਲਤ u'U':
        minDig = maxDig = 8;
        ਰੋਕੋ;
    ਹਾਲਤ u'x':
        minDig = 1;
        ਜੇ (*ਵਿੱਥ < length && charAt(*ਵਿੱਥ, context) == u'{') {
            ++(*ਵਿੱਥ);
            braces = ਸੱਚ;
            maxDig = 8;
        } ਨਹੀਂਤਾਂ {
            maxDig = 2;
        }
        ਰੋਕੋ;
    ਮੂਲ:
        dig = _digit8(c);
        ਜੇ (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            ਬਿੱਟ_ਪ੍ਰਤੀ_Digit = 3;
            ਨਤੀਜਾ = dig;
        }
        ਰੋਕੋ;
    }
    ਜੇ (minDig != 0) {
        ਜਦੋਂ (*ਵਿੱਥ < length && n < maxDig) {
            c = charAt(*ਵਿੱਥ, context);
            dig = (ਬਿੱਟ_ਪ੍ਰਤੀ_Digit == 3) ? _digit8(c) : _digit16(c);
            ਜੇ (dig < 0) {
                ਰੋਕੋ;
            }
            ਨਤੀਜਾ = (ਨਤੀਜਾ << ਬਿੱਟ_ਪ੍ਰਤੀ_Digit) | dig;
            ++(*ਵਿੱਥ);
            ++n;
        }
        ਜੇ (n < minDig) {
            ਜਾਓ err;
        }
        ਜੇ (braces) {
            ਜੇ (c != u'}') {
                ਜਾਓ err;
            }
            ++(*ਵਿੱਥ);
        }
        ਜੇ (ਨਤੀਜਾ < 0 || ਨਤੀਜਾ >= 0x110000) {
            ਜਾਓ err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        ਜੇ (*ਵਿੱਥ < length && U16_IS_LEAD(ਨਤੀਜਾ)) {
            int32_t ahead = *ਵਿੱਥ + 1;
            c = charAt(*ਵਿੱਥ, context);
            ਜੇ (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t ਪੂਛ_ਹੱਦਬੰਦੀ = ahead + 11;
                ਜੇ (ਪੂਛ_ਹੱਦਬੰਦੀ > length) {
                    ਪੂਛ_ਹੱਦਬੰਦੀ = length;
                }
                c = u_unescapeAt(charAt, &ahead, ਪੂਛ_ਹੱਦਬੰਦੀ, context);
            }
            ਜੇ (U16_IS_TRAIL(c)) {
                *ਵਿੱਥ = ahead;
                ਨਤੀਜਾ = U16_GET_SUPPLEMENTARY(ਨਤੀਜਾ, c);
            }
        }
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    /* Convert C-style escapes in table */
    ਲਈ (int32_t i=0; i<UNESCAPE_MAP_ਲੰਮਾਈ; i+=2) {
        ਜੇ (c == UNESCAPE_MAP[i]) {
            ਵਾਪਸ UNESCAPE_MAP[i+1];
        } ਨਹੀਂਤਾਂ ਜੇ (c < UNESCAPE_MAP[i]) {
            ਰੋਕੋ;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    ਜੇ (c == u'c' && *ਵਿੱਥ < length) {
        c = charAt((*ਵਿੱਥ)++, context);
        ਜੇ (U16_IS_LEAD(c) && *ਵਿੱਥ < length) {
            char16_t c2 = charAt(*ਵਿੱਥ, context);
            ਜੇ (U16_IS_TRAIL(c2)) {
                ++(*ਵਿੱਥ);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        ਵਾਪਸ 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    ਜੇ (U16_IS_LEAD(c) && *ਵਿੱਥ < length) {
        char16_t c2 = charAt(*ਵਿੱਥ, context);
        ਜੇ (U16_IS_TRAIL(c2)) {
            ++(*ਵਿੱਥ);
            ਵਾਪਸ U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    ਵਾਪਸ c;

 err:
    /* Invalid escape sequence */
    *ਵਿੱਥ = ਸ਼ੁਰੂ; /* Reset to initial value */
    ਵਾਪਸ (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
ਸਥਿਰ char16_t U_CALLCONV
_charPtr_charAt(int32_t ਵਿੱਥ, ਖਾਲੀ *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_chars_ਮੰਜ਼ਿਲ_UChars(static_cast<ਅੱਖਰ*>(context) + ਵਿੱਥ, &c16, 1);
    ਵਾਪਸ c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
ਸਥਿਰ ਖਾਲੀ _appendUChars(char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                          ਅਚਲ ਅੱਖਰ *src, int32_t srcLen) {
    ਜੇ (dest_ਸਮਰੱਥਾ < 0) {
        dest_ਸਮਰੱਥਾ = 0;
    }
    ਜੇ (srcLen > dest_ਸਮਰੱਥਾ) {
        srcLen = dest_ਸਮਰੱਥਾ;
    }
    u_chars_ਮੰਜ਼ਿਲ_UChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(ਅਚਲ ਅੱਖਰ *src, char16_t *dest, int32_t dest_ਸਮਰੱਥਾ) {
    ਅਚਲ ਅੱਖਰ *segment = src;
    int32_t i = 0;
    ਅੱਖਰ c;

    ਜਦੋਂ ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        ਜੇ (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            ਜੇ (src != segment) {
                ਜੇ (dest != ਖਾਲੀ_ਸੂਚਕ) {
                    _appendUChars(dest + i, dest_ਸਮਰੱਥਾ - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<ਅੱਖਰ*>(src));
            ਜੇ (lenParsed == 0) {
                ਜਾਓ err;
            }
            src += lenParsed; /* advance past escape seq. */
            ਜੇ (dest != ਖਾਲੀ_ਸੂਚਕ && U16_ਲੰਮਾਈ(c32) <= (dest_ਸਮਰੱਥਾ - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } ਨਹੀਂਤਾਂ {
                i += U16_ਲੰਮਾਈ(c32);
            }
            segment = src;
        } ਨਹੀਂਤਾਂ {
            ++src;
        }
    }
    ਜੇ (src != segment) {
        ਜੇ (dest != ਖਾਲੀ_ਸੂਚਕ) {
            _appendUChars(dest + i, dest_ਸਮਰੱਥਾ - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    ਜੇ (dest != ਖਾਲੀ_ਸੂਚਕ && i < dest_ਸਮਰੱਥਾ) {
        dest[i] = 0;
    }
    ਵਾਪਸ i;

 err:
    ਜੇ (dest != ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ > 0) {
        *dest = 0;
    }
    ਵਾਪਸ 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __TERMINATE_STRING(dest, dest_ਸਮਰੱਥਾ, length, p_Error_ਕੋਡ) UPRV_ਖੰਡ_MACRO_BEGIN { \
    ਜੇ(p_Error_ਕੋਡ!=ਖਾਲੀ_ਸੂਚਕ && U_SUCCESS(*p_Error_ਕੋਡ)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        ਜੇ(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } ਨਹੀਂਤਾਂ ਜੇ(length<dest_ਸਮਰੱਥਾ) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            ਜੇ(*p_Error_ਕੋਡ==U_STRING_NOT_TERMINATED_WARNING) {          \
                *p_Error_ਕੋਡ=U_ZERO_ERROR;                               \
            }                                                           \
        } ਨਹੀਂਤਾਂ ਜੇ(length==dest_ਸਮਰੱਥਾ) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *p_Error_ਕੋਡ=U_STRING_NOT_TERMINATED_WARNING;                \
        } ਨਹੀਂਤਾਂ /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;                        \
        }                                                               \
    } \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ

U_CAPI char16_t U_EXPORT2
u_ascii_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(char16_t c) {
    ਜੇ (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    ਵਾਪਸ c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t dest_ਸਮਰੱਥਾ, int32_t length, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    __TERMINATE_STRING(dest, dest_ਸਮਰੱਥਾ, length, p_Error_ਕੋਡ);
    ਵਾਪਸ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ, int32_t length, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    __TERMINATE_STRING(dest, dest_ਸਮਰੱਥਾ, length, p_Error_ਕੋਡ);
    ਵਾਪਸ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t dest_ਸਮਰੱਥਾ, int32_t length, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    __TERMINATE_STRING(dest, dest_ਸਮਰੱਥਾ, length, p_Error_ਕੋਡ);
    ਵਾਪਸ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t dest_ਸਮਰੱਥਾ, int32_t length, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    __TERMINATE_STRING(dest, dest_ਸਮਰੱਥਾ, length, p_Error_ਕੋਡ);
    ਵਾਪਸ length;
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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRING_HASH(TYPE, STR, STRLEN, DEREF) UPRV_ਖੰਡ_MACRO_BEGIN { \
    uint32_t hash = 0;                        \
    ਅਚਲ TYPE *p = (ਅਚਲ TYPE*) STR;        \
    ਜੇ (p != ਖਾਲੀ_ਸੂਚਕ) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        ਅਚਲ TYPE *ਹੱਦਬੰਦੀ = p + len;          \
        ਜਦੋਂ (p<ਹੱਦਬੰਦੀ) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    ਵਾਪਸ static_cast<int32_t>(hash);        \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(ਅਚਲ char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(ਅਚਲ ਅੱਖਰ *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(ਅਚਲ ਅੱਖਰ *str, int32_t length) {
    STRING_HASH(ਅੱਖਰ, str, length, (uint8_t)uprv_tolower(*p));
}
