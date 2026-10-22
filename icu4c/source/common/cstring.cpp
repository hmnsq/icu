// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1997-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
* File CSTRING.C
*
* @author       Helena Shih
*
* Modification History:
*
*   Date        Name        Description
*   6/18/98     hshih       Created
*   09/08/98    stephen     Added include for ctype, for Mac Port
*   11/15/99    helena      Integrated S/390 IEEE changes. 
******************************************************************************
*/



#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "uassert.h"

/*
 * We hardcode case conversion for invariant characters to match our expectation
 * and the compiler execution charset.
 * This prevents problems on systems
 * - with non-default casing behavior, like Turkish system locales where
 *   tolower('I') maps to dotless i and toupper('i') maps to dotted I
 * - where there are no lowercase Latin characters at all, or using different
 *   codes (some old EBCDIC codepages)
 *
 * This works because the compiler usually runs on a platform where the execution
 * charset includes all of the invariant characters at their expected
 * code positions, so that the char * string literals in ICU code match
 * the char literals here.
 *
 * Note that the set of lowercase Latin letters is discontiguous in EBCDIC
 * and the set of uppercase Latin letters is discontiguous as well.
 */

U_CAPI UBool U_EXPORT2
uprv_isASCIILetter(അക്ഷരം c) {
#എങ്കിൽ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    തിരികെ
        ('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z') ||
        ('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z');
#അല്ലെങ്കിൽ
    തിരികെ ('a'<=c && c<='z') || ('A'<=c && c<='Z');
#endif
}

U_CAPI അക്ഷരം U_EXPORT2
uprv_toupper(അക്ഷരം c) {
#എങ്കിൽ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    എങ്കിൽ(('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z')) {
        c=(അക്ഷരം)(c+('A'-'a'));
    }
#അല്ലെങ്കിൽ
    എങ്കിൽ('a'<=c && c<='z') {
        c=(അക്ഷരം)(c+('A'-'a'));
    }
#endif
    തിരികെ c;
}


#എങ്കിൽ 0
/*
 * Commented out because cstring.h defines uprv_tolower() to be
 * the same as either uprv_asciitolower() or uprv_ebcdictolower()
 * to reduce the amount of code to cover with tests.
 *
 * Note that this uprv_tolower() definition is likely to work for most
 * charset families, not just ASCII and EBCDIC, because its #else branch
 * is written generically.
 */
U_CAPI അക്ഷരം U_EXPORT2
uprv_tolower(അക്ഷരം c) {
#എങ്കിൽ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    എങ്കിൽ(('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z')) {
        c=(അക്ഷരം)(c+('a'-'A'));
    }
#അല്ലെങ്കിൽ
    എങ്കിൽ('A'<=c && c<='Z') {
        c=(അക്ഷരം)(c+('a'-'A'));
    }
#endif
    തിരികെ c;
}
#endif

U_CAPI അക്ഷരം U_EXPORT2
uprv_asciitolower(അക്ഷരം c) {
    എങ്കിൽ(0x41<=c && c<=0x5a) {
        c=(അക്ഷരം)(c+0x20);
    }
    തിരികെ c;
}

U_CAPI അക്ഷരം U_EXPORT2
uprv_ebcdictolower(അക്ഷരം c) {
    എങ്കിൽ( (0xc1<=(uint8_t)c && (uint8_t)c<=0xc9) ||
        (0xd1<=(uint8_t)c && (uint8_t)c<=0xd9) ||
        (0xe2<=(uint8_t)c && (uint8_t)c<=0xe9)
    ) {
        c=(അക്ഷരം)(c-0x40);
    }
    തിരികെ c;
}


U_CAPI അക്ഷരം* U_EXPORT2
T_CString_toLowerCase(അക്ഷരം* str)
{
    അക്ഷരം* origPtr = str;

    എങ്കിൽ (str) {
        ചെയ്യുക
            *str = uprv_tolower(*str);
        ആകുമ്പോൾ (*(str++));
    }

    തിരികെ origPtr;
}

U_CAPI അക്ഷരം* U_EXPORT2
T_CString_toUpperCase(അക്ഷരം* str)
{
    അക്ഷരം* origPtr = str;

    എങ്കിൽ (str) {
        ചെയ്യുക
            *str = uprv_toupper(*str);
        ആകുമ്പോൾ (*(str++));
    }

    തിരികെ origPtr;
}

/*
 * Takes a int32_t and fills in  a char* string with that number "radix"-based.
 * Does not handle negative values (makes an empty string for them).
 * Writes at most 12 chars ("-2147483647" plus NUL).
 * Returns the length of the string (not including the NUL).
 */
U_CAPI int32_t U_EXPORT2
T_CString_integerToString(അക്ഷരം* buffer, int32_t v, int32_t radix)
{
    അക്ഷരം      tbuf[30];
    int32_t   tbx    = വലുപ്പം(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint32_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint32_t) v;
    എങ്കിൽ(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint32_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = വലുപ്പം(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    ചെയ്യുക {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (അക്ഷരം)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } ആകുമ്പോൾ (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += വലുപ്പം(tbuf) - tbx -1;
    തിരികെ length;
}



/*
 * Takes a int64_t and fills in  a char* string with that number "radix"-based.
 * Writes at most 21: chars ("-9223372036854775807" plus NUL).
 * Returns the length of the string, not including the terminating NUL.
 */
U_CAPI int32_t U_EXPORT2
T_CString_int64ToString(അക്ഷരം* buffer, int64_t v, uint32_t radix)
{
    അക്ഷരം      tbuf[30];
    int32_t   tbx    = വലുപ്പം(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint64_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint64_t) v;
    എങ്കിൽ(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint64_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = വലുപ്പം(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    ചെയ്യുക {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (അക്ഷരം)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } ആകുമ്പോൾ (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += വലുപ്പം(tbuf) - tbx -1;
    തിരികെ length;
}


U_CAPI int32_t U_EXPORT2
T_CString_stringToInteger(സ്ഥിരം അക്ഷരം *integerString, int32_t radix)
{
    അക്ഷരം *end;
    തിരികെ uprv_strtoul(integerString, &end, radix);

}

U_CAPI പൂർണ്ണസംഖ്യ U_EXPORT2
uprv_stricmp(സ്ഥിരം അക്ഷരം *str1, സ്ഥിരം അക്ഷരം *str2) {
    എങ്കിൽ(str1==nullptr) {
        എങ്കിൽ(str2==nullptr) {
            തിരികെ 0;
        } അല്ലെങ്കിൽ {
            തിരികെ -1;
        }
    } അല്ലെങ്കിൽ എങ്കിൽ(str2==nullptr) {
        തിരികെ 1;
    } അല്ലെങ്കിൽ {
        /* compare non-nullptr strings lexically with lowercase */
        പൂർണ്ണസംഖ്യ rc;
        unsigned അക്ഷരം c1, c2;

        വേണ്ടി(;;) {
            c1=(unsigned അക്ഷരം)*str1;
            c2=(unsigned അക്ഷരം)*str2;
            എങ്കിൽ(c1==0) {
                എങ്കിൽ(c2==0) {
                    തിരികെ 0;
                } അല്ലെങ്കിൽ {
                    തിരികെ -1;
                }
            } അല്ലെങ്കിൽ എങ്കിൽ(c2==0) {
                തിരികെ 1;
            } അല്ലെങ്കിൽ {
                /* compare non-zero characters with lowercase */
                rc=(പൂർണ്ണസംഖ്യ)(unsigned അക്ഷരം)uprv_tolower(c1)-(പൂർണ്ണസംഖ്യ)(unsigned അക്ഷരം)uprv_tolower(c2);
                എങ്കിൽ(rc!=0) {
                    തിരികെ rc;
                }
            }
            ++str1;
            ++str2;
        }
    }
}

U_CAPI പൂർണ്ണസംഖ്യ U_EXPORT2
uprv_strnicmp(സ്ഥിരം അക്ഷരം *str1, സ്ഥിരം അക്ഷരം *str2, uint32_t n) {
    എങ്കിൽ(str1==nullptr) {
        എങ്കിൽ(str2==nullptr) {
            തിരികെ 0;
        } അല്ലെങ്കിൽ {
            തിരികെ -1;
        }
    } അല്ലെങ്കിൽ എങ്കിൽ(str2==nullptr) {
        തിരികെ 1;
    } അല്ലെങ്കിൽ {
        /* compare non-nullptr strings lexically with lowercase */
        പൂർണ്ണസംഖ്യ rc;
        unsigned അക്ഷരം c1, c2;

        വേണ്ടി(; n--;) {
            c1=(unsigned അക്ഷരം)*str1;
            c2=(unsigned അക്ഷരം)*str2;
            എങ്കിൽ(c1==0) {
                എങ്കിൽ(c2==0) {
                    തിരികെ 0;
                } അല്ലെങ്കിൽ {
                    തിരികെ -1;
                }
            } അല്ലെങ്കിൽ എങ്കിൽ(c2==0) {
                തിരികെ 1;
            } അല്ലെങ്കിൽ {
                /* compare non-zero characters with lowercase */
                rc=(പൂർണ്ണസംഖ്യ)(unsigned അക്ഷരം)uprv_tolower(c1)-(പൂർണ്ണസംഖ്യ)(unsigned അക്ഷരം)uprv_tolower(c2);
                എങ്കിൽ(rc!=0) {
                    തിരികെ rc;
                }
            }
            ++str1;
            ++str2;
        }
    }

    തിരികെ 0;
}

U_CAPI അക്ഷരം* U_EXPORT2
uprv_strdup(സ്ഥിരം അക്ഷരം *src) {
    size_t len = uprv_strlen(src) + 1;
    അക്ഷരം *dup = (അക്ഷരം *) uprv_malloc(len);

    എങ്കിൽ (dup) {
        uprv_memcpy(dup, src, len);
    }

    തിരികെ dup;
}

U_CAPI അക്ഷരം* U_EXPORT2
uprv_strndup(സ്ഥിരം അക്ഷരം *src, int32_t n) {
    അക്ഷരം *dup;

    എങ്കിൽ(n < 0) {
        dup = uprv_strdup(src);
    } അല്ലെങ്കിൽ {
        dup = (അക്ഷരം*)uprv_malloc(n+1);
        എങ്കിൽ (dup) { 
            uprv_memcpy(dup, src, n);
            dup[n] = 0;
        }
    }

    തിരികെ dup;
}
