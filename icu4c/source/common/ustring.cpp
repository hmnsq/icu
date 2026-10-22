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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cwchar.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

/* ANSI string.h - style functions ------------------------------------------ */

/* U+ffff is the highest BMP code point, the highest one that fits into a 16-bit char16_t */
#ವ್ಯಾಖ್ಯಾನಿಸು U_BMP_MAX 0xffff

/* Forward binary string search functions ----------------------------------- */

/*
 * Test if a substring match inside a string is at code point boundaries.
 * All pointers refer to the same buffer.
 * The limit pointer may be nullptr, all others must be real pointers.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool
is_ಹೊಂದಿಕೆ_At_CPBoundary(ಸ್ಥಿರಾಂಕ char16_t *ಆರಂಭ, ಸ್ಥಿರಾಂಕ char16_t *ಹೊಂದಿಕೆ, ಸ್ಥಿರಾಂಕ char16_t *ಹೊಂದಿಕೆ_ಎಲ್ಲೆ, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ) {
    ಆದರೆ(U16_IS_TRAIL(*ಹೊಂದಿಕೆ) && ಆರಂಭ!=ಹೊಂದಿಕೆ && U16_IS_LEAD(*(ಹೊಂದಿಕೆ-1))) {
        /* the leading edge of the match is in the middle of a surrogate pair */
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ(U16_IS_LEAD(*(ಹೊಂದಿಕೆ_ಎಲ್ಲೆ-1)) && ಹೊಂದಿಕೆ_ಎಲ್ಲೆ!=ಎಲ್ಲೆ && U16_IS_TRAIL(*ಹೊಂದಿಕೆ_ಎಲ್ಲೆ)) {
        /* the trailing edge of the match is in the middle of a surrogate pair */
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

U_CAPI char16_t* U_EXPORT2
u_str_ಕಂಡುಹಿಡಿ_ಮೊದಲ(ಸ್ಥಿರಾಂಕ char16_t* s U_LIFETIME_BOUND,
               int32_t length,
               ಸ್ಥಿರಾಂಕ char16_t* sub,
               int32_t sub_ಉದ್ದಳತೆ) {
    ಸ್ಥಿರಾಂಕ char16_t *ಆರಂಭ, *p, *q, *sub_ಎಲ್ಲೆ;
    char16_t c, cs, cq;

    ಆದರೆ(sub==ಶೂನ್ಯಸೂಚಕ || sub_ಉದ್ದಳತೆ<-1) {
        ಹಿಂತಿರುಗಿ (char16_t *)s;
    }
    ಆದರೆ(s==ಶೂನ್ಯಸೂಚಕ || length<-1) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆರಂಭ=s;

    ಆದರೆ(length<0 && sub_ಉದ್ದಳತೆ<0) {
        /* both strings are NUL-terminated */
        ಆದರೆ((cs=*sub++)==0) {
            ಹಿಂತಿರುಗಿ (char16_t *)s;
        }
        ಆದರೆ(*sub==0 && !U16_IS_SURROGATE(cs)) {
            /* the substring consists of a single, non-surrogate BMP code point */
            ಹಿಂತಿರುಗಿ u_strchr(s, cs);
        }

        ಆಗ((c=*s++)!=0) {
            ಆದರೆ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                ಗಾಗಿ(;;) {
                    ಆದರೆ((cq=*q)==0) {
                        ಆದರೆ(is_ಹೊಂದಿಕೆ_At_CPBoundary(ಆರಂಭ, s-1, p, ಶೂನ್ಯಸೂಚಕ)) {
                            ಹಿಂತಿರುಗಿ (char16_t *)(s-1); /* well-formed match */
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            ನಿಲ್ಲಿಸು; /* no match because surrogate pair is split */
                        }
                    }
                    ಆದರೆ((c=*p)==0) {
                        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; /* no match, and none possible after s */
                    }
                    ಆದರೆ(c!=cq) {
                        ನಿಲ್ಲಿಸು; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }

        /* not found */
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ(sub_ಉದ್ದಳತೆ<0) {
        sub_ಉದ್ದಳತೆ=u_strlen(sub);
    }
    ಆದರೆ(sub_ಉದ್ದಳತೆ==0) {
        ಹಿಂತಿರುಗಿ (char16_t *)s;
    }

    /* get sub[0] to search for it fast */
    cs=*sub++;
    --sub_ಉದ್ದಳತೆ;
    sub_ಎಲ್ಲೆ=sub+sub_ಉದ್ದಳತೆ;

    ಆದರೆ(sub_ಉದ್ದಳತೆ==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        ಹಿಂತಿರುಗಿ length<0 ? u_strchr(s, cs) : u_memchr(s, cs, length);
    }

    ಆದರೆ(length<0) {
        /* s is NUL-terminated */
        ಆಗ((c=*s++)!=0) {
            ಆದರೆ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                ಗಾಗಿ(;;) {
                    ಆದರೆ(q==sub_ಎಲ್ಲೆ) {
                        ಆದರೆ(is_ಹೊಂದಿಕೆ_At_CPBoundary(ಆರಂಭ, s-1, p, ಶೂನ್ಯಸೂಚಕ)) {
                            ಹಿಂತಿರುಗಿ (char16_t *)(s-1); /* well-formed match */
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            ನಿಲ್ಲಿಸು; /* no match because surrogate pair is split */
                        }
                    }
                    ಆದರೆ((c=*p)==0) {
                        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; /* no match, and none possible after s */
                    }
                    ಆದರೆ(c!=*q) {
                        ನಿಲ್ಲಿಸು; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ, *pre_ಎಲ್ಲೆ;

        /* subLength was decremented above */
        ಆದರೆ(length<=sub_ಉದ್ದಳತೆ) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; /* s is shorter than sub */
        }

        ಎಲ್ಲೆ=s+length;

        /* the substring must start before preLimit */
        pre_ಎಲ್ಲೆ=ಎಲ್ಲೆ-sub_ಉದ್ದಳತೆ;

        ಆಗ(s!=pre_ಎಲ್ಲೆ) {
            c=*s++;
            ಆದರೆ(c==cs) {
                /* found first substring char16_t, compare rest */
                p=s;
                q=sub;
                ಗಾಗಿ(;;) {
                    ಆದರೆ(q==sub_ಎಲ್ಲೆ) {
                        ಆದರೆ(is_ಹೊಂದಿಕೆ_At_CPBoundary(ಆರಂಭ, s-1, p, ಎಲ್ಲೆ)) {
                            ಹಿಂತಿರುಗಿ (char16_t *)(s-1); /* well-formed match */
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            ನಿಲ್ಲಿಸು; /* no match because surrogate pair is split */
                        }
                    }
                    ಆದರೆ(*p!=*q) {
                        ನಿಲ್ಲಿಸು; /* no match */
                    }
                    ++p;
                    ++q;
                }
            }
        }
    }

    /* not found */
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI char16_t* U_EXPORT2
u_strstr(ಸ್ಥಿರಾಂಕ char16_t* s U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ char16_t* substring) {
    ಹಿಂತಿರುಗಿ u_str_ಕಂಡುಹಿಡಿ_ಮೊದಲ(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strchr(ಸ್ಥಿರಾಂಕ char16_t* s U_LIFETIME_BOUND, char16_t c) {
    ಆದರೆ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ಹಿಂತಿರುಗಿ u_str_ಕಂಡುಹಿಡಿ_ಮೊದಲ(s, -1, &c, 1);
    } ಇಲ್ಲದಿದ್ದರೆ {
        char16_t cs;

        /* trivial search for a BMP code point */
        ಗಾಗಿ(;;) {
            ಆದರೆ((cs=*s)==c) {
                ಹಿಂತಿರುಗಿ (char16_t *)s;
            }
            ಆದರೆ(cs==0) {
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strchr32(ಸ್ಥಿರಾಂಕ char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    ಆದರೆ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ಹಿಂತಿರುಗಿ u_strchr(s, (char16_t)c);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((uint32_t)c<=UCHAR_MAX_ಮೌಲ್ಯ) {
        /* find supplementary code point as surrogate pair */
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ಆಗ((cs=*s++)!=0) {
            ಆದರೆ(cs==lead && *s==trail) {
                ಹಿಂತಿರುಗಿ (char16_t *)(s-1);
            }
        }
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* not a Unicode code point, not findable */
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr(ಸ್ಥಿರಾಂಕ char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ಆದರೆ(count<=0) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; /* no string */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ಹಿಂತಿರುಗಿ u_str_ಕಂಡುಹಿಡಿ_ಮೊದಲ(s, count, &c, 1);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* trivial search for a BMP code point */
        ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ=s+count;
        ಮಾಡು {
            ಆದರೆ(*s==c) {
                ಹಿಂತಿರುಗಿ (char16_t *)s;
            }
        } ಆಗ(++s!=ಎಲ್ಲೆ);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memchr32(ಸ್ಥಿರಾಂಕ char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    ಆದರೆ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ಹಿಂತಿರುಗಿ u_memchr(s, (char16_t)c, count);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(count<2) {
        /* too short for a surrogate pair */
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((uint32_t)c<=UCHAR_MAX_ಮೌಲ್ಯ) {
        /* find supplementary code point as surrogate pair */
        ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ=s+count-1; /* -1 so that we do not need a separate check for the trail unit */
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ಮಾಡು {
            ಆದರೆ(*s==lead && *(s+1)==trail) {
                ಹಿಂತಿರುಗಿ (char16_t *)s;
            }
        } ಆಗ(++s!=ಎಲ್ಲೆ);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* not a Unicode code point, not findable */
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

/* Backward binary string search functions ---------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_str_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ(ಸ್ಥಿರಾಂಕ char16_t* s U_LIFETIME_BOUND,
              int32_t length,
              ಸ್ಥಿರಾಂಕ char16_t* sub,
              int32_t sub_ಉದ್ದಳತೆ) {
    ಸ್ಥಿರಾಂಕ char16_t *ಆರಂಭ, *ಎಲ್ಲೆ, *p, *q, *sub_ಎಲ್ಲೆ;
    char16_t c, cs;

    ಆದರೆ(sub==ಶೂನ್ಯಸೂಚಕ || sub_ಉದ್ದಳತೆ<-1) {
        ಹಿಂತಿರುಗಿ (char16_t *)s;
    }
    ಆದರೆ(s==ಶೂನ್ಯಸೂಚಕ || length<-1) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
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

    ಆದರೆ(sub_ಉದ್ದಳತೆ<0) {
        sub_ಉದ್ದಳತೆ=u_strlen(sub);
    }
    ಆದರೆ(sub_ಉದ್ದಳತೆ==0) {
        ಹಿಂತಿರುಗಿ (char16_t *)s;
    }

    /* get sub[subLength-1] to search for it fast */
    sub_ಎಲ್ಲೆ=sub+sub_ಉದ್ದಳತೆ;
    cs=*(--sub_ಎಲ್ಲೆ);
    --sub_ಉದ್ದಳತೆ;

    ಆದರೆ(sub_ಉದ್ದಳತೆ==0 && !U16_IS_SURROGATE(cs)) {
        /* the substring consists of a single, non-surrogate BMP code point */
        ಹಿಂತಿರುಗಿ length<0 ? u_strrchr(s, cs) : u_memrchr(s, cs, length);
    }

    ಆದರೆ(length<0) {
        length=u_strlen(s);
    }

    /* subLength was decremented above */
    ಆದರೆ(length<=sub_ಉದ್ದಳತೆ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; /* s is shorter than sub */
    }

    ಆರಂಭ=s;
    ಎಲ್ಲೆ=s+length;

    /* the substring must start no later than s+subLength */
    s+=sub_ಉದ್ದಳತೆ;

    ಆಗ(s!=ಎಲ್ಲೆ) {
        c=*(--ಎಲ್ಲೆ);
        ಆದರೆ(c==cs) {
            /* found last substring char16_t, compare rest */
            p=ಎಲ್ಲೆ;
            q=sub_ಎಲ್ಲೆ;
            ಗಾಗಿ(;;) {
                ಆದರೆ(q==sub) {
                    ಆದರೆ(is_ಹೊಂದಿಕೆ_At_CPBoundary(ಆರಂಭ, p, ಎಲ್ಲೆ+1, ಆರಂಭ+length)) {
                        ಹಿಂತಿರುಗಿ (char16_t *)p; /* well-formed match */
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ನಿಲ್ಲಿಸು; /* no match because surrogate pair is split */
                    }
                }
                ಆದರೆ(*(--p)!=*(--q)) {
                    ನಿಲ್ಲಿಸು; /* no match */
                }
            }
        }
    }

    /* not found */
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI char16_t* U_EXPORT2
u_strrstr(ಸ್ಥಿರಾಂಕ char16_t* s U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ char16_t* substring) {
    ಹಿಂತಿರುಗಿ u_str_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ(s, -1, substring, -1);
}

U_CAPI char16_t* U_EXPORT2
u_strrchr(ಸ್ಥಿರಾಂಕ char16_t* s U_LIFETIME_BOUND, char16_t c) {
    ಆದರೆ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ಹಿಂತಿರುಗಿ u_str_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ(s, -1, &c, 1);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಸ್ಥಿರಾಂಕ char16_t *ಫಲಿತಾಂಶ=ಶೂನ್ಯಸೂಚಕ;
        char16_t cs;

        /* trivial search for a BMP code point */
        ಗಾಗಿ(;;) {
            ಆದರೆ((cs=*s)==c) {
                ಫಲಿತಾಂಶ=s;
            }
            ಆದರೆ(cs==0) {
                ಹಿಂತಿರುಗಿ (char16_t *)ಫಲಿತಾಂಶ;
            }
            ++s;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_strrchr32(ಸ್ಥಿರಾಂಕ char16_t* s U_LIFETIME_BOUND, UChar32 c) {
    ಆದರೆ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ಹಿಂತಿರುಗಿ u_strrchr(s, (char16_t)c);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((uint32_t)c<=UCHAR_MAX_ಮೌಲ್ಯ) {
        /* find supplementary code point as surrogate pair */
        ಸ್ಥಿರಾಂಕ char16_t *ಫಲಿತಾಂಶ=ಶೂನ್ಯಸೂಚಕ;
        char16_t cs, lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ಆಗ((cs=*s++)!=0) {
            ಆದರೆ(cs==lead && *s==trail) {
                ಫಲಿತಾಂಶ=s-1;
            }
        }
        ಹಿಂತಿರುಗಿ (char16_t *)ಫಲಿತಾಂಶ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* not a Unicode code point, not findable */
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr(ಸ್ಥಿರಾಂಕ char16_t* s U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ಆದರೆ(count<=0) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; /* no string */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE(c)) {
        /* make sure to not find half of a surrogate pair */
        ಹಿಂತಿರುಗಿ u_str_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ(s, count, &c, 1);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* trivial search for a BMP code point */
        ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ=s+count;
        ಮಾಡು {
            ಆದರೆ(*(--ಎಲ್ಲೆ)==c) {
                ಹಿಂತಿರುಗಿ (char16_t *)ಎಲ್ಲೆ;
            }
        } ಆಗ(s!=ಎಲ್ಲೆ);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

U_CAPI char16_t* U_EXPORT2
u_memrchr32(ಸ್ಥಿರಾಂಕ char16_t* s U_LIFETIME_BOUND, UChar32 c, int32_t count) {
    ಆದರೆ((uint32_t)c<=U_BMP_MAX) {
        /* find BMP code point */
        ಹಿಂತಿರುಗಿ u_memrchr(s, (char16_t)c, count);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(count<2) {
        /* too short for a surrogate pair */
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((uint32_t)c<=UCHAR_MAX_ಮೌಲ್ಯ) {
        /* find supplementary code point as surrogate pair */
        ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ=s+count-1;
        char16_t lead=U16_LEAD(c), trail=U16_TRAIL(c);

        ಮಾಡು {
            ಆದರೆ(*ಎಲ್ಲೆ==trail && *(ಎಲ್ಲೆ-1)==lead) {
                ಹಿಂತಿರುಗಿ (char16_t *)(ಎಲ್ಲೆ-1);
            }
        } ಆಗ(s!=--ಎಲ್ಲೆ);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* not a Unicode code point, not findable */
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

/* Tokenization functions --------------------------------------------------- */

/*
 * Match each code point in a string against each code point in the matchSet.
 * Return the index of the first string code point that
 * is (polarity==true) or is not (false) contained in the matchSet.
 * Return -(string length)-1 if there is no such code point.
 */
ಸ್ಥಿರ int32_t
ಹೊಂದಿಕೆ_From_ಗಣ(ಸ್ಥಿರಾಂಕ char16_t *string, ಸ್ಥಿರಾಂಕ char16_t *ಹೊಂದಿಕೆ_ಗಣ, UBool polarity) {
    int32_t ಹೊಂದಿಕೆ_Len, ಹೊಂದಿಕೆ_BMPLen, strItr, ಹೊಂದಿಕೆ_Itr;
    UChar32 stringCh, ಹೊಂದಿಕೆ_Ch;
    char16_t c, c2;

    /* first part of matchSet contains only BMP code points */
    ಹೊಂದಿಕೆ_BMPLen = 0;
    ಆಗ((c = ಹೊಂದಿಕೆ_ಗಣ[ಹೊಂದಿಕೆ_BMPLen]) != 0 && U16_IS_SINGLE(c)) {
        ++ಹೊಂದಿಕೆ_BMPLen;
    }

    /* second part of matchSet contains BMP and supplementary code points */
    ಹೊಂದಿಕೆ_Len = ಹೊಂದಿಕೆ_BMPLen;
    ಆಗ(ಹೊಂದಿಕೆ_ಗಣ[ಹೊಂದಿಕೆ_Len] != 0) {
        ++ಹೊಂದಿಕೆ_Len;
    }

    ಗಾಗಿ(strItr = 0; (c = string[strItr]) != 0;) {
        ++strItr;
        ಆದರೆ(U16_IS_SINGLE(c)) {
            ಆದರೆ(polarity) {
                ಗಾಗಿ(ಹೊಂದಿಕೆ_Itr = 0; ಹೊಂದಿಕೆ_Itr < ಹೊಂದಿಕೆ_Len; ++ಹೊಂದಿಕೆ_Itr) {
                    ಆದರೆ(c == ಹೊಂದಿಕೆ_ಗಣ[ಹೊಂದಿಕೆ_Itr]) {
                        ಹಿಂತಿರುಗಿ strItr - 1; /* one matches */
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಗಾಗಿ(ಹೊಂದಿಕೆ_Itr = 0; ಹೊಂದಿಕೆ_Itr < ಹೊಂದಿಕೆ_Len; ++ಹೊಂದಿಕೆ_Itr) {
                    ಆದರೆ(c == ಹೊಂದಿಕೆ_ಗಣ[ಹೊಂದಿಕೆ_Itr]) {
                        ಹೋಗು endloop;
                    }
                }
                ಹಿಂತಿರುಗಿ strItr - 1; /* none matches */
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /*
             * No need to check for string length before U16_IS_TRAIL
             * because c2 could at worst be the terminating NUL.
             */
            ಆದರೆ(U16_IS_SURROGATE_LEAD(c) && U16_IS_TRAIL(c2 = string[strItr])) {
                ++strItr;
                stringCh = U16_GET_SUPPLEMENTARY(c, c2);
            } ಇಲ್ಲದಿದ್ದರೆ {
                stringCh = c; /* unpaired trail surrogate */
            }

            ಆದರೆ(polarity) {
                ಗಾಗಿ(ಹೊಂದಿಕೆ_Itr = ಹೊಂದಿಕೆ_BMPLen; ಹೊಂದಿಕೆ_Itr < ಹೊಂದಿಕೆ_Len;) {
                    U16_ಮುಂದಿನ(ಹೊಂದಿಕೆ_ಗಣ, ಹೊಂದಿಕೆ_Itr, ಹೊಂದಿಕೆ_Len, ಹೊಂದಿಕೆ_Ch);
                    ಆದರೆ(stringCh == ಹೊಂದಿಕೆ_Ch) {
                        ಹಿಂತಿರುಗಿ strItr - U16_ಉದ್ದಳತೆ(stringCh); /* one matches */
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಗಾಗಿ(ಹೊಂದಿಕೆ_Itr = ಹೊಂದಿಕೆ_BMPLen; ಹೊಂದಿಕೆ_Itr < ಹೊಂದಿಕೆ_Len;) {
                    U16_ಮುಂದಿನ(ಹೊಂದಿಕೆ_ಗಣ, ಹೊಂದಿಕೆ_Itr, ಹೊಂದಿಕೆ_Len, ಹೊಂದಿಕೆ_Ch);
                    ಆದರೆ(stringCh == ಹೊಂದಿಕೆ_Ch) {
                        ಹೋಗು endloop;
                    }
                }
                ಹಿಂತಿರುಗಿ strItr - U16_ಉದ್ದಳತೆ(stringCh); /* none matches */
            }
        }
endloop:
        /* wish C had continue with labels like Java... */;
    }

    /* Didn't find it. */
    ಹಿಂತಿರುಗಿ -strItr-1;
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI char16_t* U_EXPORT2
u_strpbrk(ಸ್ಥಿರಾಂಕ char16_t* string U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ char16_t* ಹೊಂದಿಕೆ_ಗಣ)
{
    int32_t idx = ಹೊಂದಿಕೆ_From_ಗಣ(string, ಹೊಂದಿಕೆ_ಗಣ, ನಿಜ);
    ಆದರೆ(idx >= 0) {
        ಹಿಂತಿರುಗಿ (char16_t *)string + idx;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

/* Search for a codepoint in a string that matches one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strcspn(ಸ್ಥಿರಾಂಕ char16_t *string, ಸ್ಥಿರಾಂಕ char16_t *ಹೊಂದಿಕೆ_ಗಣ)
{
    int32_t idx = ಹೊಂದಿಕೆ_From_ಗಣ(string, ಹೊಂದಿಕೆ_ಗಣ, ನಿಜ);
    ಆದರೆ(idx >= 0) {
        ಹಿಂತಿರುಗಿ idx;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ -idx - 1; /* == u_strlen(string) */
    }
}

/* Search for a codepoint in a string that does not match one of the matchSet codepoints. */
U_CAPI int32_t U_EXPORT2
u_strspn(ಸ್ಥಿರಾಂಕ char16_t *string, ಸ್ಥಿರಾಂಕ char16_t *ಹೊಂದಿಕೆ_ಗಣ)
{
    int32_t idx = ಹೊಂದಿಕೆ_From_ಗಣ(string, ಹೊಂದಿಕೆ_ಗಣ, ಸುಳ್ಳು);
    ಆದರೆ(idx >= 0) {
        ಹಿಂತಿರುಗಿ idx;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ -idx - 1; /* == u_strlen(string) */
    }
}

/* ----- Text manipulation functions --- */

U_CAPI char16_t* U_EXPORT2
u_strtok_r(char16_t* src U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ char16_t* delim, char16_t** saveState)
{
    char16_t *tok_ಮೂಲ;
    char16_t *ಮುಂದಿನ_Token;
    uint32_t nonDelimIdx;

    /* If saveState is nullptr, the user messed up. */
    ಆದರೆ (src != ಶೂನ್ಯಸೂಚಕ) {
        tok_ಮೂಲ = src;
        *saveState = src; /* Set to "src" in case there are no delimiters */
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*saveState) {
        tok_ಮೂಲ = *saveState;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        /* src == nullptr && *saveState == nullptr */
        /* This shouldn't happen. We already finished tokenizing. */
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* Skip initial delimiters */
    nonDelimIdx = u_strspn(tok_ಮೂಲ, delim);
    tok_ಮೂಲ = &tok_ಮೂಲ[nonDelimIdx];

    ಆದರೆ (*tok_ಮೂಲ) {
        ಮುಂದಿನ_Token = u_strpbrk(tok_ಮೂಲ, delim);
        ಆದರೆ (ಮುಂದಿನ_Token != ಶೂನ್ಯಸೂಚಕ) {
            /* Create a token */
            *(ಮುಂದಿನ_Token++) = 0;
            *saveState = ಮುಂದಿನ_Token;
            ಹಿಂತಿರುಗಿ tok_ಮೂಲ;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*saveState) {
            /* Return the last token */
            *saveState = ಶೂನ್ಯಸೂಚಕ;
            ಹಿಂತಿರುಗಿ tok_ಮೂಲ;
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        /* No tokens were found. Only delimiters were left. */
        *saveState = ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

/* Miscellaneous functions -------------------------------------------------- */

U_CAPI char16_t* U_EXPORT2
u_strcat(char16_t* dst U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    ಆಗ(*dst != 0) {              /* To end of first string          */
        ++dst;
    }
    ಆಗ((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    ಹಿಂತಿರುಗಿ anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncat(char16_t* dst U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ char16_t* src, int32_t n)
{
    ಆದರೆ(n > 0) {
        char16_t *anchor = dst;            /* save a pointer to start of dst */

        ಆಗ(*dst != 0) {              /* To end of first string          */
            ++dst;
        }
        ಆಗ((*dst = *src) != 0) {     /* copy string 2 over              */
            ++dst;
            ಆದರೆ(--n == 0) {
                *dst = 0;
                ನಿಲ್ಲಿಸು;
            }
            ++src;
        }

        ಹಿಂತಿರುಗಿ anchor;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ dst;
    }
}

/* ----- Text property functions --- */

U_CAPI int32_t   U_EXPORT2
u_strcmp(ಸ್ಥಿರಾಂಕ char16_t *s1,
    ಸ್ಥಿರಾಂಕ char16_t *s2)
{
    char16_t  c1, c2;

    ಗಾಗಿ(;;) {
        c1=*s1++;
        c2=*s2++;
        ಆದರೆ (c1 != c2 || c1 == 0) {
            ನಿಲ್ಲಿಸು;
        }
    }
    ಹಿಂತಿರುಗಿ (int32_t)c1 - (int32_t)c2;
}

U_CFUNC int32_t U_EXPORT2
uprv_strCompare(ಸ್ಥಿರಾಂಕ char16_t *s1, int32_t length1,
                ಸ್ಥಿರಾಂಕ char16_t *s2, int32_t length2,
                UBool strncmpStyle, UBool ಸಂಕೇತ_ಬಿಂದು_ಕ್ರಮ) {
    ಸ್ಥಿರಾಂಕ char16_t *start1, *start2, *limit1, *limit2;
    char16_t c1, c2;

    /* setup for fix-up */
    start1=s1;
    start2=s2;

    /* compare identical prefixes - they do not need to be fixed up */
    ಆದರೆ(length1<0 && length2<0) {
        /* strcmp style, both NUL-terminated */
        ಆದರೆ(s1==s2) {
            ಹಿಂತಿರುಗಿ 0;
        }

        ಗಾಗಿ(;;) {
            c1=*s1;
            c2=*s2;
            ಆದರೆ(c1!=c2) {
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ(c1==0) {
                ಹಿಂತಿರುಗಿ 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=limit2=ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strncmpStyle) {
        /* special handling for strncmp, assume length1==length2>=0 but also check for NUL */
        ಆದರೆ(s1==s2) {
            ಹಿಂತಿರುಗಿ 0;
        }

        limit1=start1+length1;

        ಗಾಗಿ(;;) {
            /* both lengths are same, check only one limit */
            ಆದರೆ(s1==limit1) {
                ಹಿಂತಿರುಗಿ 0;
            }

            c1=*s1;
            c2=*s2;
            ಆದರೆ(c1!=c2) {
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ(c1==0) {
                ಹಿಂತಿರುಗಿ 0;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit2=start2+length1; /* use length1 here, too, to enforce assumption */
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* memcmp/UnicodeString style, both length-specified */
        int32_t ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ;

        ಆದರೆ(length1<0) {
            length1=u_strlen(s1);
        }
        ಆದರೆ(length2<0) {
            length2=u_strlen(s2);
        }

        /* limit1=start1+min(length1, length2) */
        ಆದರೆ(length1<length2) {
            ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ=-1;
            limit1=start1+length1;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(length1==length2) {
            ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ=0;
            limit1=start1+length1;
        } ಇಲ್ಲದಿದ್ದರೆ /* length1>length2 */ {
            ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ=1;
            limit1=start1+length2;
        }

        ಆದರೆ(s1==s2) {
            ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ;
        }

        ಗಾಗಿ(;;) {
            /* check pseudo-limit */
            ಆದರೆ(s1==limit1) {
                ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ;
            }

            c1=*s1;
            c2=*s2;
            ಆದರೆ(c1!=c2) {
                ನಿಲ್ಲಿಸು;
            }
            ++s1;
            ++s2;
        }

        /* setup for fix-up */
        limit1=start1+length1;
        limit2=start2+length2;
    }

    /* if both values are in or above the surrogate range, fix them up */
    ಆದರೆ(c1>=0xd800 && c2>=0xd800 && ಸಂಕೇತ_ಬಿಂದು_ಕ್ರಮ) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        ಆದರೆ(
            (c1<=0xdbff && (s1+1)!=limit1 && U16_IS_TRAIL(*(s1+1))) ||
            (U16_IS_TRAIL(c1) && start1!=s1 && U16_IS_LEAD(*(s1-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        ಆದರೆ(
            (c2<=0xdbff && (s2+1)!=limit2 && U16_IS_TRAIL(*(s2+1))) ||
            (U16_IS_TRAIL(c2) && start2!=s2 && U16_IS_LEAD(*(s2-1)))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    ಹಿಂತಿರುಗಿ (int32_t)c1-(int32_t)c2;
}

/*
 * Compare two strings as presented by UCharIterators.
 * Use code unit or code point order.
 * When the function returns, it is undefined where the iterators
 * have stopped.
 */
U_CAPI int32_t U_EXPORT2
u_strCompareIter(UCharIterator *iter1, UCharIterator *iter2, UBool ಸಂಕೇತ_ಬಿಂದು_ಕ್ರಮ) {
    UChar32 c1, c2;

    /* argument checking */
    ಆದರೆ(iter1==ಶೂನ್ಯಸೂಚಕ || iter2==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ 0; /* bad arguments */
    }
    ಆದರೆ(iter1==iter2) {
        ಹಿಂತಿರುಗಿ 0; /* identical iterators */
    }

    /* reset iterators to start? */
    iter1->move(iter1, 0, UITER_ಆರಂಭ);
    iter2->move(iter2, 0, UITER_ಆರಂಭ);

    /* compare identical prefixes - they do not need to be fixed up */
    ಗಾಗಿ(;;) {
        c1=iter1->next(iter1);
        c2=iter2->next(iter2);
        ಆದರೆ(c1!=c2) {
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ(c1==-1) {
            ಹಿಂತಿರುಗಿ 0;
        }
    }

    /* if both values are in or above the surrogate range, fix them up */
    ಆದರೆ(c1>=0xd800 && c2>=0xd800 && ಸಂಕೇತ_ಬಿಂದು_ಕ್ರಮ) {
        /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
        ಆದರೆ(
            (c1<=0xdbff && U16_IS_TRAIL(iter1->current(iter1))) ||
            (U16_IS_TRAIL(c1) && (iter1->ಹಿಂದಿನ(iter1), U16_IS_LEAD(iter1->ಹಿಂದಿನ(iter1))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c1-=0x2800;
        }

        ಆದರೆ(
            (c2<=0xdbff && U16_IS_TRAIL(iter2->current(iter2))) ||
            (U16_IS_TRAIL(c2) && (iter2->ಹಿಂದಿನ(iter2), U16_IS_LEAD(iter2->ಹಿಂದಿನ(iter2))))
        ) {
            /* part of a surrogate pair, leave >=d800 */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* BMP code point - may be surrogate code point - make <d800 */
            c2-=0x2800;
        }
    }

    /* now c1 and c2 are in the requested (code unit or code point) order */
    ಹಿಂತಿರುಗಿ (int32_t)c1-(int32_t)c2;
}

#ಆದರೆ 0
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
ಶೂನ್ಯ fragment {
        /* iff a surrogate is part of a surrogate pair, leave >=d800 */
        ಆದರೆ(c1<=0xdbff) {
            ಆದರೆ(!U16_IS_TRAIL(iter1->current(iter1))) {
                /* lead surrogate code point - make <d800 */
                c1-=0x2800;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c1<=0xdfff) {
            int32_t idx=iter1->get_ಸೂಚ್ಯಂಕ(iter1, UITER_CURRENT);
            iter1->ಹಿಂದಿನ(iter1); /* ==c1 */
            ಆದರೆ(!U16_IS_LEAD(iter1->ಹಿಂದಿನ(iter1))) {
                /* trail surrogate code point - make <d800 */
                c1-=0x2800;
            }
            /* go back to behind where the difference is */
            iter1->move(iter1, idx, UITER_ZERO);
        } ಇಲ್ಲದಿದ್ದರೆ /* 0xe000<=c1<=0xffff */ {
            /* BMP code point - make <d800 */
            c1-=0x2800;
        }
}
#ಆದರೆ_ಅಂತ್ಯ

U_CAPI int32_t U_EXPORT2
u_strCompare(ಸ್ಥಿರಾಂಕ char16_t *s1, int32_t length1,
             ಸ್ಥಿರಾಂಕ char16_t *s2, int32_t length2,
             UBool ಸಂಕೇತ_ಬಿಂದು_ಕ್ರಮ) {
    /* argument checking */
    ಆದರೆ(s1==ಶೂನ್ಯಸೂಚಕ || length1<-1 || s2==ಶೂನ್ಯಸೂಚಕ || length2<-1) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ uprv_strCompare(s1, length1, s2, length2, ಸುಳ್ಳು, ಸಂಕೇತ_ಬಿಂದು_ಕ್ರಮ);
}

/* String compare in code point order - u_strcmp() compares in code unit order. */
U_CAPI int32_t U_EXPORT2
u_strcmp_ಸಂಕೇತ_ಬಿಂದು_ಕ್ರಮ(ಸ್ಥಿರಾಂಕ char16_t *s1, ಸ್ಥಿರಾಂಕ char16_t *s2) {
    ಹಿಂತಿರುಗಿ uprv_strCompare(s1, -1, s2, -1, ಸುಳ್ಳು, ನಿಜ);
}

U_CAPI int32_t   U_EXPORT2
u_strncmp(ಸ್ಥಿರಾಂಕ char16_t  *s1,
     ಸ್ಥಿರಾಂಕ char16_t  *s2,
     int32_t     n) 
{
    ಆದರೆ(n > 0) {
        int32_t rc;
        ಗಾಗಿ(;;) {
            rc = (int32_t)*s1 - (int32_t)*s2;
            ಆದರೆ(rc != 0 || *s1 == 0 || --n == 0) {
                ಹಿಂತಿರುಗಿ rc;
            }
            ++s1;
            ++s2;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;
    }
}

U_CAPI int32_t U_EXPORT2
u_strncmp_ಸಂಕೇತ_ಬಿಂದು_ಕ್ರಮ(ಸ್ಥಿರಾಂಕ char16_t *s1, ಸ್ಥಿರಾಂಕ char16_t *s2, int32_t n) {
    ಹಿಂತಿರುಗಿ uprv_strCompare(s1, n, s2, n, ನಿಜ, ನಿಜ);
}

U_CAPI char16_t* U_EXPORT2
u_strcpy(char16_t* dst U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ char16_t* src)
{
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    ಆಗ((*(dst++) = *(src++)) != 0) {     /* copy string 2 over              */
    }

    ಹಿಂತಿರುಗಿ anchor;
}

U_CAPI char16_t* U_EXPORT2
u_strncpy(char16_t* dst U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ char16_t* src, int32_t n) {
    char16_t *anchor = dst;            /* save a pointer to start of dst */

    /* copy string 2 over */
    ಆಗ(n > 0 && (*(dst++) = *(src++)) != 0) {
        --n;
    }

    ಹಿಂತಿರುಗಿ anchor;
}

U_CAPI int32_t   U_EXPORT2
u_strlen(ಸ್ಥಿರಾಂಕ char16_t *s)
{
#ಆದರೆ U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    ಹಿಂತಿರುಗಿ (int32_t)uprv_wcslen((ಸ್ಥಿರಾಂಕ wchar_t *)s);
#ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿರಾಂಕ char16_t *t = s;
    ಆಗ(*t != 0) {
      ++t;
    }
    ಹಿಂತಿರುಗಿ t - s;
#ಆದರೆ_ಅಂತ್ಯ
}

U_CAPI int32_t U_EXPORT2
u_ಎಣಿಕೆ_Char32(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length) {
    int32_t count;

    ಆದರೆ(s==ಶೂನ್ಯಸೂಚಕ || length<-1) {
        ಹಿಂತಿರುಗಿ 0;
    }

    count=0;
    ಆದರೆ(length>=0) {
        ಆಗ(length>0) {
            ++count;
            ಆದರೆ(U16_IS_LEAD(*s) && length>=2 && U16_IS_TRAIL(*(s+1))) {
                s+=2;
                length-=2;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ++s;
                --length;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ /* length==-1 */ {
        char16_t c;

        ಗಾಗಿ(;;) {
            ಆದರೆ((c=*s++)==0) {
                ನಿಲ್ಲಿಸು;
            }
            ++count;

            /*
             * sufficient to look ahead one because of UTF-16;
             * safe to look ahead one because at worst that would be the terminating NUL
             */
            ಆದರೆ(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
        }
    }
    ಹಿಂತಿರುಗಿ count;
}

U_CAPI UBool U_EXPORT2
u_str_Has_ಇನ್ನಷ್ಟು_Char32_Than(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length, int32_t number) {

    ಆದರೆ(number<0) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ(s==ಶೂನ್ಯಸೂಚಕ || length<-1) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಆದರೆ(length==-1) {
        /* s is NUL-terminated */
        char16_t c;

        /* count code points until they exceed */
        ಗಾಗಿ(;;) {
            ಆದರೆ((c=*s++)==0) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            ಆದರೆ(number==0) {
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
            ಆದರೆ(U16_IS_LEAD(c) && U16_IS_TRAIL(*s)) {
                ++s;
            }
            --number;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* length>=0 known */
        ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ;
        int32_t maxSupplementary;

        /* s contains at least (length+1)/2 code points: <=2 UChars per cp */
        ಆದರೆ(((length+1)/2)>number) {
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }

        /* check if s does not even contain enough UChars */
        maxSupplementary=length-number;
        ಆದರೆ(maxSupplementary<=0) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        /* there are maxSupplementary=length-number more UChars than asked-for code points */

        /*
         * count code points until they exceed and also check that there are
         * no more than maxSupplementary supplementary code points (char16_t pairs)
         */
        ಎಲ್ಲೆ=s+length;
        ಗಾಗಿ(;;) {
            ಆದರೆ(s==ಎಲ್ಲೆ) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            ಆದರೆ(number==0) {
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
            ಆದರೆ(U16_IS_LEAD(*s++) && s!=ಎಲ್ಲೆ && U16_IS_TRAIL(*s)) {
                ++s;
                ಆದರೆ(--maxSupplementary<=0) {
                    /* too many pairs - too few code points */
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            }
            --number;
        }
    }
}

U_CAPI char16_t* U_EXPORT2
u_memcpy(char16_t* dest U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ char16_t* src, int32_t count) {
    ಆದರೆ(count > 0) {
        uprv_memcpy(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    ಹಿಂತಿರುಗಿ dest;
}

U_CAPI char16_t* U_EXPORT2
u_memmove(char16_t* dest U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ char16_t* src, int32_t count) {
    ಆದರೆ(count > 0) {
        uprv_memmove(dest, src, (size_t)count*U_SIZEOF_UCHAR);
    }
    ಹಿಂತಿರುಗಿ dest;
}

U_CAPI char16_t* U_EXPORT2
u_memset(char16_t* dest U_LIFETIME_BOUND, char16_t c, int32_t count) {
    ಆದರೆ(count > 0) {
        char16_t *ptr = dest;
        char16_t *ಎಲ್ಲೆ = dest + count;

        ಆಗ (ptr < ಎಲ್ಲೆ) {
            *(ptr++) = c;
        }
    }
    ಹಿಂತಿರುಗಿ dest;
}

U_CAPI int32_t U_EXPORT2
u_memcmp(ಸ್ಥಿರಾಂಕ char16_t *buf1, ಸ್ಥಿರಾಂಕ char16_t *buf2, int32_t count) {
    ಆದರೆ(count > 0) {
        ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ = buf1 + count;
        int32_t ಫಲಿತಾಂಶ;

        ಆಗ (buf1 < ಎಲ್ಲೆ) {
            ಫಲಿತಾಂಶ = (int32_t)(uint16_t)*buf1 - (int32_t)(uint16_t)*buf2;
            ಆದರೆ (ಫಲಿತಾಂಶ != 0) {
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
            }
            buf1++;
            buf2++;
        }
    }
    ಹಿಂತಿರುಗಿ 0;
}

U_CAPI int32_t U_EXPORT2
u_memcmp_ಸಂಕೇತ_ಬಿಂದು_ಕ್ರಮ(ಸ್ಥಿರಾಂಕ char16_t *s1, ಸ್ಥಿರಾಂಕ char16_t *s2, int32_t count) {
    ಹಿಂತಿರುಗಿ uprv_strCompare(s1, count, s2, count, ಸುಳ್ಳು, ನಿಜ);
}

/* u_unescape & support fns ------------------------------------------------- */

/* This map must be in ASCENDING ORDER OF THE ESCAPE CODE */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t UNESCAPE_MAP[] = {
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
ಎಣಿಕೆ { UNESCAPE_MAP_ಉದ್ದಳತೆ = UPRV_LENGTHOF(UNESCAPE_MAP) };

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
ಸ್ಥಿರ int32_t _digit8(char16_t c) {
    ಆದರೆ (c >= u'0' && c <= u'7') {
        ಹಿಂತಿರುಗಿ c - u'0';
    }
    ಹಿಂತಿರುಗಿ -1;
}

/* Convert one hex digit to a numeric value 0..F, or -1 on failure */
ಸ್ಥಿರ int32_t _digit16(char16_t c) {
    ಆದರೆ (c >= u'0' && c <= u'9') {
        ಹಿಂತಿರುಗಿ c - u'0';
    }
    ಆದರೆ (c >= u'A' && c <= u'F') {
        ಹಿಂತಿರುಗಿ c - (u'A' - 10);
    }
    ಆದರೆ (c >= u'a' && c <= u'f') {
        ಹಿಂತಿರುಗಿ c - (u'a' - 10);
    }
    ಹಿಂತಿರುಗಿ -1;
}

/* Parse a single escape sequence.  Although this method deals in
 * UChars, it does not use C++ or UnicodeString.  This allows it to
 * be used from C contexts. */
U_CAPI UChar32 U_EXPORT2
u_unescapeAt(UNESCAPE_CHAR_AT charAt,
             int32_t *ಸ್ಥಾನಾಂತರ,
             int32_t length,
             ಶೂನ್ಯ *context) {

    int32_t ಆರಂಭ = *ಸ್ಥಾನಾಂತರ;
    UChar32 c;
    UChar32 ಫಲಿತಾಂಶ = 0;
    int8_t n = 0;
    int8_t minDig = 0;
    int8_t maxDig = 0;
    int8_t ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_Digit = 4; 
    int32_t dig;
    UBool braces = ಸುಳ್ಳು;

    /* Check that offset is in range */
    ಆದರೆ (*ಸ್ಥಾನಾಂತರ < 0 || *ಸ್ಥಾನಾಂತರ >= length) {
        ಹೋಗು err;
    }

    /* Fetch first char16_t after '\\' */
    c = charAt((*ಸ್ಥಾನಾಂತರ)++, context);

    /* Convert hexadecimal and octal escapes */
    ಆಯ್ಕೆ (c) {
    ಸ್ಥಿತಿ u'u':
        minDig = maxDig = 4;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ u'U':
        minDig = maxDig = 8;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ u'x':
        minDig = 1;
        ಆದರೆ (*ಸ್ಥಾನಾಂತರ < length && charAt(*ಸ್ಥಾನಾಂತರ, context) == u'{') {
            ++(*ಸ್ಥಾನಾಂತರ);
            braces = ನಿಜ;
            maxDig = 8;
        } ಇಲ್ಲದಿದ್ದರೆ {
            maxDig = 2;
        }
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        dig = _digit8(c);
        ಆದರೆ (dig >= 0) {
            minDig = 1;
            maxDig = 3;
            n = 1; /* Already have first octal digit */
            ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_Digit = 3;
            ಫಲಿತಾಂಶ = dig;
        }
        ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (minDig != 0) {
        ಆಗ (*ಸ್ಥಾನಾಂತರ < length && n < maxDig) {
            c = charAt(*ಸ್ಥಾನಾಂತರ, context);
            dig = (ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_Digit == 3) ? _digit8(c) : _digit16(c);
            ಆದರೆ (dig < 0) {
                ನಿಲ್ಲಿಸು;
            }
            ಫಲಿತಾಂಶ = (ಫಲಿತಾಂಶ << ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_Digit) | dig;
            ++(*ಸ್ಥಾನಾಂತರ);
            ++n;
        }
        ಆದರೆ (n < minDig) {
            ಹೋಗು err;
        }
        ಆದರೆ (braces) {
            ಆದರೆ (c != u'}') {
                ಹೋಗು err;
            }
            ++(*ಸ್ಥಾನಾಂತರ);
        }
        ಆದರೆ (ಫಲಿತಾಂಶ < 0 || ಫಲಿತಾಂಶ >= 0x110000) {
            ಹೋಗು err;
        }
        /* If an escape sequence specifies a lead surrogate, see if
         * there is a trail surrogate after it, either as an escape or
         * as a literal.  If so, join them up into a supplementary.
         */
        ಆದರೆ (*ಸ್ಥಾನಾಂತರ < length && U16_IS_LEAD(ಫಲಿತಾಂಶ)) {
            int32_t ahead = *ಸ್ಥಾನಾಂತರ + 1;
            c = charAt(*ಸ್ಥಾನಾಂತರ, context);
            ಆದರೆ (c == u'\\' && ahead < length) {
                // Calling ourselves recursively may cause a stack overflow if
                // we have repeated escaped lead surrogates.
                // Limit the length to 11 ("x{0000DFFF}") after ahead.
                int32_t ಬಾಲ_ಎಲ್ಲೆ = ahead + 11;
                ಆದರೆ (ಬಾಲ_ಎಲ್ಲೆ > length) {
                    ಬಾಲ_ಎಲ್ಲೆ = length;
                }
                c = u_unescapeAt(charAt, &ahead, ಬಾಲ_ಎಲ್ಲೆ, context);
            }
            ಆದರೆ (U16_IS_TRAIL(c)) {
                *ಸ್ಥಾನಾಂತರ = ahead;
                ಫಲಿತಾಂಶ = U16_GET_SUPPLEMENTARY(ಫಲಿತಾಂಶ, c);
            }
        }
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    /* Convert C-style escapes in table */
    ಗಾಗಿ (int32_t i=0; i<UNESCAPE_MAP_ಉದ್ದಳತೆ; i+=2) {
        ಆದರೆ (c == UNESCAPE_MAP[i]) {
            ಹಿಂತಿರುಗಿ UNESCAPE_MAP[i+1];
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c < UNESCAPE_MAP[i]) {
            ನಿಲ್ಲಿಸು;
        }
    }

    /* Map \cX to control-X: X & 0x1F */
    ಆದರೆ (c == u'c' && *ಸ್ಥಾನಾಂತರ < length) {
        c = charAt((*ಸ್ಥಾನಾಂತರ)++, context);
        ಆದರೆ (U16_IS_LEAD(c) && *ಸ್ಥಾನಾಂತರ < length) {
            char16_t c2 = charAt(*ಸ್ಥಾನಾಂತರ, context);
            ಆದರೆ (U16_IS_TRAIL(c2)) {
                ++(*ಸ್ಥಾನಾಂತರ);
                c = U16_GET_SUPPLEMENTARY(c, c2);
            }
        }
        ಹಿಂತಿರುಗಿ 0x1F & c;
    }

    /* If no special forms are recognized, then consider
     * the backslash to generically escape the next character.
     * Deal with surrogate pairs. */
    ಆದರೆ (U16_IS_LEAD(c) && *ಸ್ಥಾನಾಂತರ < length) {
        char16_t c2 = charAt(*ಸ್ಥಾನಾಂತರ, context);
        ಆದರೆ (U16_IS_TRAIL(c2)) {
            ++(*ಸ್ಥಾನಾಂತರ);
            ಹಿಂತಿರುಗಿ U16_GET_SUPPLEMENTARY(c, c2);
        }
    }
    ಹಿಂತಿರುಗಿ c;

 err:
    /* Invalid escape sequence */
    *ಸ್ಥಾನಾಂತರ = ಆರಂಭ; /* Reset to initial value */
    ಹಿಂತಿರುಗಿ (UChar32)0xFFFFFFFF;
}

/* u_unescapeAt() callback to return a char16_t from a char* */
ಸ್ಥಿರ char16_t U_CALLCONV
_charPtr_charAt(int32_t ಸ್ಥಾನಾಂತರ, ಶೂನ್ಯ *context) {
    char16_t c16;
    /* It would be more efficient to access the invariant tables
     * directly but there is no API for that. */
    u_chars_ಗಮ್ಯ_UChars(static_cast<ಅಕ್ಷರ*>(context) + ಸ್ಥಾನಾಂತರ, &c16, 1);
    ಹಿಂತಿರುಗಿ c16;
}

/* Append an escape-free segment of the text; used by u_unescape() */
ಸ್ಥಿರ ಶೂನ್ಯ _appendUChars(char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t srcLen) {
    ಆದರೆ (dest_ಸಾಮರ್ಥ್ಯ < 0) {
        dest_ಸಾಮರ್ಥ್ಯ = 0;
    }
    ಆದರೆ (srcLen > dest_ಸಾಮರ್ಥ್ಯ) {
        srcLen = dest_ಸಾಮರ್ಥ್ಯ;
    }
    u_chars_ಗಮ್ಯ_UChars(src, dest, srcLen);
}

/* Do an invariant conversion of char* -> char16_t*, with escape parsing */
U_CAPI int32_t U_EXPORT2
u_unescape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *segment = src;
    int32_t i = 0;
    ಅಕ್ಷರ c;

    ಆಗ ((c=*src) != 0) {
        /* '\\' intentionally written as compiler-specific
         * character constant to correspond to compiler-specific
         * char* constants. */
        ಆದರೆ (c == '\\') {
            int32_t lenParsed = 0;
            UChar32 c32;
            ಆದರೆ (src != segment) {
                ಆದರೆ (dest != ಶೂನ್ಯಸೂಚಕ) {
                    _appendUChars(dest + i, dest_ಸಾಮರ್ಥ್ಯ - i,
                                  segment, (int32_t)(src - segment));
                }
                i += (int32_t)(src - segment);
            }
            ++src; /* advance past '\\' */
            c32 = u_unescapeAt(_charPtr_charAt, &lenParsed, (int32_t)uprv_strlen(src), const_cast<ಅಕ್ಷರ*>(src));
            ಆದರೆ (lenParsed == 0) {
                ಹೋಗು err;
            }
            src += lenParsed; /* advance past escape seq. */
            ಆದರೆ (dest != ಶೂನ್ಯಸೂಚಕ && U16_ಉದ್ದಳತೆ(c32) <= (dest_ಸಾಮರ್ಥ್ಯ - i)) {
                U16_APPEND_UNSAFE(dest, i, c32);
            } ಇಲ್ಲದಿದ್ದರೆ {
                i += U16_ಉದ್ದಳತೆ(c32);
            }
            segment = src;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ++src;
        }
    }
    ಆದರೆ (src != segment) {
        ಆದರೆ (dest != ಶೂನ್ಯಸೂಚಕ) {
            _appendUChars(dest + i, dest_ಸಾಮರ್ಥ್ಯ - i,
                          segment, (int32_t)(src - segment));
        }
        i += (int32_t)(src - segment);
    }
    ಆದರೆ (dest != ಶೂನ್ಯಸೂಚಕ && i < dest_ಸಾಮರ್ಥ್ಯ) {
        dest[i] = 0;
    }
    ಹಿಂತಿರುಗಿ i;

 err:
    ಆದರೆ (dest != ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ > 0) {
        *dest = 0;
    }
    ಹಿಂತಿರುಗಿ 0;
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು __TERMINATE_STRING(dest, dest_ಸಾಮರ್ಥ್ಯ, length, p_Error_ಸಂಕೇತ) UPRV_ಖಂಡ_MACRO_BEGIN { \
    ಆದರೆ(p_Error_ಸಂಕೇತ!=ಶೂನ್ಯಸೂಚಕ && U_SUCCESS(*p_Error_ಸಂಕೇತ)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        ಆದರೆ(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(length<dest_ಸಾಮರ್ಥ್ಯ) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            ಆದರೆ(*p_Error_ಸಂಕೇತ==U_STRING_NOT_TERMINATED_WARNING) {          \
                *p_Error_ಸಂಕೇತ=U_ZERO_ERROR;                               \
            }                                                           \
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(length==dest_ಸಾಮರ್ಥ್ಯ) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *p_Error_ಸಂಕೇತ=U_STRING_NOT_TERMINATED_WARNING;                \
        } ಇಲ್ಲದಿದ್ದರೆ /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;                        \
        }                                                               \
    } \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ

U_CAPI char16_t U_EXPORT2
u_ascii_ಗಮ್ಯ_ಊರ್ಧ್ವ(char16_t c) {
    ಆದರೆ (u'a' <= c && c <= u'z') {
        c = c + u'A' - u'a';
    }
    ಹಿಂತಿರುಗಿ c;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChars(char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ, int32_t length, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    __TERMINATE_STRING(dest, dest_ಸಾಮರ್ಥ್ಯ, length, p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(ಅಕ್ಷರ *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ, int32_t length, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    __TERMINATE_STRING(dest, dest_ಸಾಮರ್ಥ್ಯ, length, p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ, int32_t length, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    __TERMINATE_STRING(dest, dest_ಸಾಮರ್ಥ್ಯ, length, p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ length;
}

U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ, int32_t length, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    __TERMINATE_STRING(dest, dest_ಸಾಮರ್ಥ್ಯ, length, p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ length;
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

#ವ್ಯಾಖ್ಯಾನಿಸು STRING_HASH(TYPE, STR, STRLEN, DEREF) UPRV_ಖಂಡ_MACRO_BEGIN { \
    uint32_t hash = 0;                        \
    ಸ್ಥಿರಾಂಕ TYPE *p = (ಸ್ಥಿರಾಂಕ TYPE*) STR;        \
    ಆದರೆ (p != ಶೂನ್ಯಸೂಚಕ) {                          \
        int32_t len = (int32_t)(STRLEN);      \
        int32_t inc = ((len - 32) / 32) + 1;  \
        ಸ್ಥಿರಾಂಕ TYPE *ಎಲ್ಲೆ = p + len;          \
        ಆಗ (p<ಎಲ್ಲೆ) {                     \
            hash = (hash * 37) + DEREF;       \
            p += inc;                         \
        }                                     \
    }                                         \
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(hash);        \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ

/* Used by UnicodeString to compute its hashcode - Not public API. */
U_CAPI int32_t U_EXPORT2
ustr_hashUCharsN(ಸ್ಥಿರಾಂಕ char16_t *str, int32_t length) {
    STRING_HASH(char16_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashCharsN(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, int32_t length) {
    STRING_HASH(uint8_t, str, length, *p);
}

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, int32_t length) {
    STRING_HASH(ಅಕ್ಷರ, str, length, (uint8_t)uprv_tolower(*p));
}
