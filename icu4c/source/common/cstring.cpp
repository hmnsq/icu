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



#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

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
uprv_isASCIILetter(ಅಕ್ಷರ c) {
#ಆದರೆ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    ಹಿಂತಿರುಗಿ
        ('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z') ||
        ('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z');
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ ('a'<=c && c<='z') || ('A'<=c && c<='Z');
#ಆದರೆ_ಅಂತ್ಯ
}

U_CAPI ಅಕ್ಷರ U_EXPORT2
uprv_toupper(ಅಕ್ಷರ c) {
#ಆದರೆ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    ಆದರೆ(('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z')) {
        c=(ಅಕ್ಷರ)(c+('A'-'a'));
    }
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ('a'<=c && c<='z') {
        c=(ಅಕ್ಷರ)(c+('A'-'a'));
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ c;
}


#ಆದರೆ 0
/*
 * Commented out because cstring.h defines uprv_tolower() to be
 * the same as either uprv_asciitolower() or uprv_ebcdictolower()
 * to reduce the amount of code to cover with tests.
 *
 * Note that this uprv_tolower() definition is likely to work for most
 * charset families, not just ASCII and EBCDIC, because its #else branch
 * is written generically.
 */
U_CAPI ಅಕ್ಷರ U_EXPORT2
uprv_tolower(ಅಕ್ಷರ c) {
#ಆದರೆ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    ಆದರೆ(('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z')) {
        c=(ಅಕ್ಷರ)(c+('a'-'A'));
    }
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ('A'<=c && c<='Z') {
        c=(ಅಕ್ಷರ)(c+('a'-'A'));
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ c;
}
#ಆದರೆ_ಅಂತ್ಯ

U_CAPI ಅಕ್ಷರ U_EXPORT2
uprv_asciitolower(ಅಕ್ಷರ c) {
    ಆದರೆ(0x41<=c && c<=0x5a) {
        c=(ಅಕ್ಷರ)(c+0x20);
    }
    ಹಿಂತಿರುಗಿ c;
}

U_CAPI ಅಕ್ಷರ U_EXPORT2
uprv_ebcdictolower(ಅಕ್ಷರ c) {
    ಆದರೆ( (0xc1<=(uint8_t)c && (uint8_t)c<=0xc9) ||
        (0xd1<=(uint8_t)c && (uint8_t)c<=0xd9) ||
        (0xe2<=(uint8_t)c && (uint8_t)c<=0xe9)
    ) {
        c=(ಅಕ್ಷರ)(c-0x40);
    }
    ಹಿಂತಿರುಗಿ c;
}


U_CAPI ಅಕ್ಷರ* U_EXPORT2
T_CString_ಗಮ್ಯ_ಅಧೋ_Case(ಅಕ್ಷರ* str)
{
    ಅಕ್ಷರ* origPtr = str;

    ಆದರೆ (str) {
        ಮಾಡು
            *str = uprv_tolower(*str);
        ಆಗ (*(str++));
    }

    ಹಿಂತಿರುಗಿ origPtr;
}

U_CAPI ಅಕ್ಷರ* U_EXPORT2
T_CString_ಗಮ್ಯ_ಊರ್ಧ್ವ_Case(ಅಕ್ಷರ* str)
{
    ಅಕ್ಷರ* origPtr = str;

    ಆದರೆ (str) {
        ಮಾಡು
            *str = uprv_toupper(*str);
        ಆಗ (*(str++));
    }

    ಹಿಂತಿರುಗಿ origPtr;
}

/*
 * Takes a int32_t and fills in  a char* string with that number "radix"-based.
 * Does not handle negative values (makes an empty string for them).
 * Writes at most 12 chars ("-2147483647" plus NUL).
 * Returns the length of the string (not including the NUL).
 */
U_CAPI int32_t U_EXPORT2
T_CString_integer_ಗಮ್ಯ_String(ಅಕ್ಷರ* ಬಫರ್, int32_t v, int32_t radix)
{
    ಅಕ್ಷರ      tbuf[30];
    int32_t   tbx    = ಗಾತ್ರ(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint32_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint32_t) v;
    ಆದರೆ(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint32_t)(-v); 
        ಬಫರ್[length++] = '-';
    }
    
    tbx = ಗಾತ್ರ(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    ಮಾಡು {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (ಅಕ್ಷರ)(T_CString_itos_ಸ್ಥಾನಾಂತರ(digit));
        uval  = uval / radix;
    } ಆಗ (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(ಬಫರ್+length, tbuf+tbx);
    length += ಗಾತ್ರ(tbuf) - tbx -1;
    ಹಿಂತಿರುಗಿ length;
}



/*
 * Takes a int64_t and fills in  a char* string with that number "radix"-based.
 * Writes at most 21: chars ("-9223372036854775807" plus NUL).
 * Returns the length of the string, not including the terminating NUL.
 */
U_CAPI int32_t U_EXPORT2
T_CString_int64_ಗಮ್ಯ_String(ಅಕ್ಷರ* ಬಫರ್, int64_t v, uint32_t radix)
{
    ಅಕ್ಷರ      tbuf[30];
    int32_t   tbx    = ಗಾತ್ರ(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint64_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint64_t) v;
    ಆದರೆ(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint64_t)(-v); 
        ಬಫರ್[length++] = '-';
    }
    
    tbx = ಗಾತ್ರ(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    ಮಾಡು {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (ಅಕ್ಷರ)(T_CString_itos_ಸ್ಥಾನಾಂತರ(digit));
        uval  = uval / radix;
    } ಆಗ (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(ಬಫರ್+length, tbuf+tbx);
    length += ಗಾತ್ರ(tbuf) - tbx -1;
    ಹಿಂತಿರುಗಿ length;
}


U_CAPI int32_t U_EXPORT2
T_CString_string_ಗಮ್ಯ_Integer(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *integerString, int32_t radix)
{
    ಅಕ್ಷರ *ಕೊನೆ;
    ಹಿಂತಿರುಗಿ uprv_strtoul(integerString, &ಕೊನೆ, radix);

}

U_CAPI ಪೂರ್ಣಾಂಕ U_EXPORT2
uprv_stricmp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str2) {
    ಆದರೆ(str1==ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(str2==ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ 0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ -1;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str2==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ 1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* compare non-nullptr strings lexically with lowercase */
        ಪೂರ್ಣಾಂಕ rc;
        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c1, c2;

        ಗಾಗಿ(;;) {
            c1=(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*str1;
            c2=(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*str2;
            ಆದರೆ(c1==0) {
                ಆದರೆ(c2==0) {
                    ಹಿಂತಿರುಗಿ 0;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಹಿಂತಿರುಗಿ -1;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c2==0) {
                ಹಿಂತಿರುಗಿ 1;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* compare non-zero characters with lowercase */
                rc=(ಪೂರ್ಣಾಂಕ)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)uprv_tolower(c1)-(ಪೂರ್ಣಾಂಕ)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)uprv_tolower(c2);
                ಆದರೆ(rc!=0) {
                    ಹಿಂತಿರುಗಿ rc;
                }
            }
            ++str1;
            ++str2;
        }
    }
}

U_CAPI ಪೂರ್ಣಾಂಕ U_EXPORT2
uprv_strnicmp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str2, uint32_t n) {
    ಆದರೆ(str1==ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(str2==ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ 0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ -1;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str2==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ 1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* compare non-nullptr strings lexically with lowercase */
        ಪೂರ್ಣಾಂಕ rc;
        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c1, c2;

        ಗಾಗಿ(; n--;) {
            c1=(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*str1;
            c2=(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*str2;
            ಆದರೆ(c1==0) {
                ಆದರೆ(c2==0) {
                    ಹಿಂತಿರುಗಿ 0;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಹಿಂತಿರುಗಿ -1;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c2==0) {
                ಹಿಂತಿರುಗಿ 1;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* compare non-zero characters with lowercase */
                rc=(ಪೂರ್ಣಾಂಕ)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)uprv_tolower(c1)-(ಪೂರ್ಣಾಂಕ)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)uprv_tolower(c2);
                ಆದರೆ(rc!=0) {
                    ಹಿಂತಿರುಗಿ rc;
                }
            }
            ++str1;
            ++str2;
        }
    }

    ಹಿಂತಿರುಗಿ 0;
}

U_CAPI ಅಕ್ಷರ* U_EXPORT2
uprv_strdup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src) {
    size_t len = uprv_strlen(src) + 1;
    ಅಕ್ಷರ *dup = (ಅಕ್ಷರ *) uprv_malloc(len);

    ಆದರೆ (dup) {
        uprv_memcpy(dup, src, len);
    }

    ಹಿಂತಿರುಗಿ dup;
}

U_CAPI ಅಕ್ಷರ* U_EXPORT2
uprv_strndup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t n) {
    ಅಕ್ಷರ *dup;

    ಆದರೆ(n < 0) {
        dup = uprv_strdup(src);
    } ಇಲ್ಲದಿದ್ದರೆ {
        dup = (ಅಕ್ಷರ*)uprv_malloc(n+1);
        ಆದರೆ (dup) { 
            uprv_memcpy(dup, src, n);
            dup[n] = 0;
        }
    }

    ಹಿಂತಿರುಗಿ dup;
}
