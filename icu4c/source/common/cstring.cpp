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



#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

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
uprv_isASCIILetter(ਅੱਖਰ c) {
#ਜੇ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    ਵਾਪਸ
        ('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z') ||
        ('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z');
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ ('a'<=c && c<='z') || ('A'<=c && c<='Z');
#ਜੇ_ਅੰਤ
}

U_CAPI ਅੱਖਰ U_EXPORT2
uprv_toupper(ਅੱਖਰ c) {
#ਜੇ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    ਜੇ(('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z')) {
        c=(ਅੱਖਰ)(c+('A'-'a'));
    }
#ਨਹੀਂਤਾਂ
    ਜੇ('a'<=c && c<='z') {
        c=(ਅੱਖਰ)(c+('A'-'a'));
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ c;
}


#ਜੇ 0
/*
 * Commented out because cstring.h defines uprv_tolower() to be
 * the same as either uprv_asciitolower() or uprv_ebcdictolower()
 * to reduce the amount of code to cover with tests.
 *
 * Note that this uprv_tolower() definition is likely to work for most
 * charset families, not just ASCII and EBCDIC, because its #else branch
 * is written generically.
 */
U_CAPI ਅੱਖਰ U_EXPORT2
uprv_tolower(ਅੱਖਰ c) {
#ਜੇ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    ਜੇ(('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z')) {
        c=(ਅੱਖਰ)(c+('a'-'A'));
    }
#ਨਹੀਂਤਾਂ
    ਜੇ('A'<=c && c<='Z') {
        c=(ਅੱਖਰ)(c+('a'-'A'));
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ c;
}
#ਜੇ_ਅੰਤ

U_CAPI ਅੱਖਰ U_EXPORT2
uprv_asciitolower(ਅੱਖਰ c) {
    ਜੇ(0x41<=c && c<=0x5a) {
        c=(ਅੱਖਰ)(c+0x20);
    }
    ਵਾਪਸ c;
}

U_CAPI ਅੱਖਰ U_EXPORT2
uprv_ebcdictolower(ਅੱਖਰ c) {
    ਜੇ( (0xc1<=(uint8_t)c && (uint8_t)c<=0xc9) ||
        (0xd1<=(uint8_t)c && (uint8_t)c<=0xd9) ||
        (0xe2<=(uint8_t)c && (uint8_t)c<=0xe9)
    ) {
        c=(ਅੱਖਰ)(c-0x40);
    }
    ਵਾਪਸ c;
}


U_CAPI ਅੱਖਰ* U_EXPORT2
T_CString_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ_Case(ਅੱਖਰ* str)
{
    ਅੱਖਰ* origPtr = str;

    ਜੇ (str) {
        ਕਰੋ
            *str = uprv_tolower(*str);
        ਜਦੋਂ (*(str++));
    }

    ਵਾਪਸ origPtr;
}

U_CAPI ਅੱਖਰ* U_EXPORT2
T_CString_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_Case(ਅੱਖਰ* str)
{
    ਅੱਖਰ* origPtr = str;

    ਜੇ (str) {
        ਕਰੋ
            *str = uprv_toupper(*str);
        ਜਦੋਂ (*(str++));
    }

    ਵਾਪਸ origPtr;
}

/*
 * Takes a int32_t and fills in  a char* string with that number "radix"-based.
 * Does not handle negative values (makes an empty string for them).
 * Writes at most 12 chars ("-2147483647" plus NUL).
 * Returns the length of the string (not including the NUL).
 */
U_CAPI int32_t U_EXPORT2
T_CString_integer_ਮੰਜ਼ਿਲ_String(ਅੱਖਰ* ਬਫਰ, int32_t v, int32_t radix)
{
    ਅੱਖਰ      tbuf[30];
    int32_t   tbx    = ਆਕਾਰ(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint32_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint32_t) v;
    ਜੇ(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint32_t)(-v); 
        ਬਫਰ[length++] = '-';
    }
    
    tbx = ਆਕਾਰ(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    ਕਰੋ {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (ਅੱਖਰ)(T_CString_itos_ਵਿੱਥ(digit));
        uval  = uval / radix;
    } ਜਦੋਂ (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(ਬਫਰ+length, tbuf+tbx);
    length += ਆਕਾਰ(tbuf) - tbx -1;
    ਵਾਪਸ length;
}



/*
 * Takes a int64_t and fills in  a char* string with that number "radix"-based.
 * Writes at most 21: chars ("-9223372036854775807" plus NUL).
 * Returns the length of the string, not including the terminating NUL.
 */
U_CAPI int32_t U_EXPORT2
T_CString_int64_ਮੰਜ਼ਿਲ_String(ਅੱਖਰ* ਬਫਰ, int64_t v, uint32_t radix)
{
    ਅੱਖਰ      tbuf[30];
    int32_t   tbx    = ਆਕਾਰ(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint64_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint64_t) v;
    ਜੇ(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint64_t)(-v); 
        ਬਫਰ[length++] = '-';
    }
    
    tbx = ਆਕਾਰ(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    ਕਰੋ {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (ਅੱਖਰ)(T_CString_itos_ਵਿੱਥ(digit));
        uval  = uval / radix;
    } ਜਦੋਂ (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(ਬਫਰ+length, tbuf+tbx);
    length += ਆਕਾਰ(tbuf) - tbx -1;
    ਵਾਪਸ length;
}


U_CAPI int32_t U_EXPORT2
T_CString_string_ਮੰਜ਼ਿਲ_Integer(ਅਚਲ ਅੱਖਰ *integerString, int32_t radix)
{
    ਅੱਖਰ *ਸਿਰਾ;
    ਵਾਪਸ uprv_strtoul(integerString, &ਸਿਰਾ, radix);

}

U_CAPI ਪੂਰਨ U_EXPORT2
uprv_stricmp(ਅਚਲ ਅੱਖਰ *str1, ਅਚਲ ਅੱਖਰ *str2) {
    ਜੇ(str1==ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(str2==ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ 0;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ -1;
        }
    } ਨਹੀਂਤਾਂ ਜੇ(str2==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ 1;
    } ਨਹੀਂਤਾਂ {
        /* compare non-nullptr strings lexically with lowercase */
        ਪੂਰਨ rc;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c1, c2;

        ਲਈ(;;) {
            c1=(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*str1;
            c2=(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*str2;
            ਜੇ(c1==0) {
                ਜੇ(c2==0) {
                    ਵਾਪਸ 0;
                } ਨਹੀਂਤਾਂ {
                    ਵਾਪਸ -1;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c2==0) {
                ਵਾਪਸ 1;
            } ਨਹੀਂਤਾਂ {
                /* compare non-zero characters with lowercase */
                rc=(ਪੂਰਨ)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)uprv_tolower(c1)-(ਪੂਰਨ)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)uprv_tolower(c2);
                ਜੇ(rc!=0) {
                    ਵਾਪਸ rc;
                }
            }
            ++str1;
            ++str2;
        }
    }
}

U_CAPI ਪੂਰਨ U_EXPORT2
uprv_strnicmp(ਅਚਲ ਅੱਖਰ *str1, ਅਚਲ ਅੱਖਰ *str2, uint32_t n) {
    ਜੇ(str1==ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(str2==ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ 0;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ -1;
        }
    } ਨਹੀਂਤਾਂ ਜੇ(str2==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ 1;
    } ਨਹੀਂਤਾਂ {
        /* compare non-nullptr strings lexically with lowercase */
        ਪੂਰਨ rc;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c1, c2;

        ਲਈ(; n--;) {
            c1=(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*str1;
            c2=(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*str2;
            ਜੇ(c1==0) {
                ਜੇ(c2==0) {
                    ਵਾਪਸ 0;
                } ਨਹੀਂਤਾਂ {
                    ਵਾਪਸ -1;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c2==0) {
                ਵਾਪਸ 1;
            } ਨਹੀਂਤਾਂ {
                /* compare non-zero characters with lowercase */
                rc=(ਪੂਰਨ)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)uprv_tolower(c1)-(ਪੂਰਨ)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)uprv_tolower(c2);
                ਜੇ(rc!=0) {
                    ਵਾਪਸ rc;
                }
            }
            ++str1;
            ++str2;
        }
    }

    ਵਾਪਸ 0;
}

U_CAPI ਅੱਖਰ* U_EXPORT2
uprv_strdup(ਅਚਲ ਅੱਖਰ *src) {
    size_t len = uprv_strlen(src) + 1;
    ਅੱਖਰ *dup = (ਅੱਖਰ *) uprv_malloc(len);

    ਜੇ (dup) {
        uprv_memcpy(dup, src, len);
    }

    ਵਾਪਸ dup;
}

U_CAPI ਅੱਖਰ* U_EXPORT2
uprv_strndup(ਅਚਲ ਅੱਖਰ *src, int32_t n) {
    ਅੱਖਰ *dup;

    ਜੇ(n < 0) {
        dup = uprv_strdup(src);
    } ਨਹੀਂਤਾਂ {
        dup = (ਅੱਖਰ*)uprv_malloc(n+1);
        ਜੇ (dup) { 
            uprv_memcpy(dup, src, n);
            dup[n] = 0;
        }
    }

    ਵਾਪਸ dup;
}
