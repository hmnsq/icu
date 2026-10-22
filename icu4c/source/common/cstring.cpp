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



#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

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
uprv_isASCIILetter(ଅକ୍ଷର c) {
#ଯଦି U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    ଫେରନ୍ତୁ
        ('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z') ||
        ('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z');
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ ('a'<=c && c<='z') || ('A'<=c && c<='Z');
#endif
}

U_CAPI ଅକ୍ଷର U_EXPORT2
uprv_toupper(ଅକ୍ଷର c) {
#ଯଦି U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    ଯଦି(('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z')) {
        c=(ଅକ୍ଷର)(c+('A'-'a'));
    }
#ଅନ୍ୟଥା
    ଯଦି('a'<=c && c<='z') {
        c=(ଅକ୍ଷର)(c+('A'-'a'));
    }
#endif
    ଫେରନ୍ତୁ c;
}


#ଯଦି 0
/*
 * Commented out because cstring.h defines uprv_tolower() to be
 * the same as either uprv_asciitolower() or uprv_ebcdictolower()
 * to reduce the amount of code to cover with tests.
 *
 * Note that this uprv_tolower() definition is likely to work for most
 * charset families, not just ASCII and EBCDIC, because its #else branch
 * is written generically.
 */
U_CAPI ଅକ୍ଷର U_EXPORT2
uprv_tolower(ଅକ୍ଷର c) {
#ଯଦି U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    ଯଦି(('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z')) {
        c=(ଅକ୍ଷର)(c+('a'-'A'));
    }
#ଅନ୍ୟଥା
    ଯଦି('A'<=c && c<='Z') {
        c=(ଅକ୍ଷର)(c+('a'-'A'));
    }
#endif
    ଫେରନ୍ତୁ c;
}
#endif

U_CAPI ଅକ୍ଷର U_EXPORT2
uprv_asciitolower(ଅକ୍ଷର c) {
    ଯଦି(0x41<=c && c<=0x5a) {
        c=(ଅକ୍ଷର)(c+0x20);
    }
    ଫେରନ୍ତୁ c;
}

U_CAPI ଅକ୍ଷର U_EXPORT2
uprv_ebcdictolower(ଅକ୍ଷର c) {
    ଯଦି( (0xc1<=(uint8_t)c && (uint8_t)c<=0xc9) ||
        (0xd1<=(uint8_t)c && (uint8_t)c<=0xd9) ||
        (0xe2<=(uint8_t)c && (uint8_t)c<=0xe9)
    ) {
        c=(ଅକ୍ଷର)(c-0x40);
    }
    ଫେରନ୍ତୁ c;
}


U_CAPI ଅକ୍ଷର* U_EXPORT2
T_CString_toLowerCase(ଅକ୍ଷର* str)
{
    ଅକ୍ଷର* origPtr = str;

    ଯଦି (str) {
        କର
            *str = uprv_tolower(*str);
        ଯେତେବେଳେ (*(str++));
    }

    ଫେରନ୍ତୁ origPtr;
}

U_CAPI ଅକ୍ଷର* U_EXPORT2
T_CString_toUpperCase(ଅକ୍ଷର* str)
{
    ଅକ୍ଷର* origPtr = str;

    ଯଦି (str) {
        କର
            *str = uprv_toupper(*str);
        ଯେତେବେଳେ (*(str++));
    }

    ଫେରନ୍ତୁ origPtr;
}

/*
 * Takes a int32_t and fills in  a char* string with that number "radix"-based.
 * Does not handle negative values (makes an empty string for them).
 * Writes at most 12 chars ("-2147483647" plus NUL).
 * Returns the length of the string (not including the NUL).
 */
U_CAPI int32_t U_EXPORT2
T_CString_integerToString(ଅକ୍ଷର* buffer, int32_t v, int32_t radix)
{
    ଅକ୍ଷର      tbuf[30];
    int32_t   tbx    = ଆକାର(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint32_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint32_t) v;
    ଯଦି(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint32_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = ଆକାର(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    କର {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (ଅକ୍ଷର)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } ଯେତେବେଳେ (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += ଆକାର(tbuf) - tbx -1;
    ଫେରନ୍ତୁ length;
}



/*
 * Takes a int64_t and fills in  a char* string with that number "radix"-based.
 * Writes at most 21: chars ("-9223372036854775807" plus NUL).
 * Returns the length of the string, not including the terminating NUL.
 */
U_CAPI int32_t U_EXPORT2
T_CString_int64ToString(ଅକ୍ଷର* buffer, int64_t v, uint32_t radix)
{
    ଅକ୍ଷର      tbuf[30];
    int32_t   tbx    = ଆକାର(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint64_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint64_t) v;
    ଯଦି(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint64_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = ଆକାର(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    କର {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (ଅକ୍ଷର)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } ଯେତେବେଳେ (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += ଆକାର(tbuf) - tbx -1;
    ଫେରନ୍ତୁ length;
}


U_CAPI int32_t U_EXPORT2
T_CString_stringToInteger(ଅଚଳ ଅକ୍ଷର *integerString, int32_t radix)
{
    ଅକ୍ଷର *end;
    ଫେରନ୍ତୁ uprv_strtoul(integerString, &end, radix);

}

U_CAPI ପୂର୍ଣ୍ଣସଂଖ୍ୟା U_EXPORT2
uprv_stricmp(ଅଚଳ ଅକ୍ଷର *str1, ଅଚଳ ଅକ୍ଷର *str2) {
    ଯଦି(str1==nullptr) {
        ଯଦି(str2==nullptr) {
            ଫେରନ୍ତୁ 0;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ -1;
        }
    } ଅନ୍ୟଥା ଯଦି(str2==nullptr) {
        ଫେରନ୍ତୁ 1;
    } ଅନ୍ୟଥା {
        /* compare non-nullptr strings lexically with lowercase */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
        unsigned ଅକ୍ଷର c1, c2;

        ପାଇଁ(;;) {
            c1=(unsigned ଅକ୍ଷର)*str1;
            c2=(unsigned ଅକ୍ଷର)*str2;
            ଯଦି(c1==0) {
                ଯଦି(c2==0) {
                    ଫେରନ୍ତୁ 0;
                } ଅନ୍ୟଥା {
                    ଫେରନ୍ତୁ -1;
                }
            } ଅନ୍ୟଥା ଯଦି(c2==0) {
                ଫେରନ୍ତୁ 1;
            } ଅନ୍ୟଥା {
                /* compare non-zero characters with lowercase */
                rc=(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(unsigned ଅକ୍ଷର)uprv_tolower(c1)-(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(unsigned ଅକ୍ଷର)uprv_tolower(c2);
                ଯଦି(rc!=0) {
                    ଫେରନ୍ତୁ rc;
                }
            }
            ++str1;
            ++str2;
        }
    }
}

U_CAPI ପୂର୍ଣ୍ଣସଂଖ୍ୟା U_EXPORT2
uprv_strnicmp(ଅଚଳ ଅକ୍ଷର *str1, ଅଚଳ ଅକ୍ଷର *str2, uint32_t n) {
    ଯଦି(str1==nullptr) {
        ଯଦି(str2==nullptr) {
            ଫେରନ୍ତୁ 0;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ -1;
        }
    } ଅନ୍ୟଥା ଯଦି(str2==nullptr) {
        ଫେରନ୍ତୁ 1;
    } ଅନ୍ୟଥା {
        /* compare non-nullptr strings lexically with lowercase */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
        unsigned ଅକ୍ଷର c1, c2;

        ପାଇଁ(; n--;) {
            c1=(unsigned ଅକ୍ଷର)*str1;
            c2=(unsigned ଅକ୍ଷର)*str2;
            ଯଦି(c1==0) {
                ଯଦି(c2==0) {
                    ଫେରନ୍ତୁ 0;
                } ଅନ୍ୟଥା {
                    ଫେରନ୍ତୁ -1;
                }
            } ଅନ୍ୟଥା ଯଦି(c2==0) {
                ଫେରନ୍ତୁ 1;
            } ଅନ୍ୟଥା {
                /* compare non-zero characters with lowercase */
                rc=(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(unsigned ଅକ୍ଷର)uprv_tolower(c1)-(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(unsigned ଅକ୍ଷର)uprv_tolower(c2);
                ଯଦି(rc!=0) {
                    ଫେରନ୍ତୁ rc;
                }
            }
            ++str1;
            ++str2;
        }
    }

    ଫେରନ୍ତୁ 0;
}

U_CAPI ଅକ୍ଷର* U_EXPORT2
uprv_strdup(ଅଚଳ ଅକ୍ଷର *src) {
    size_t len = uprv_strlen(src) + 1;
    ଅକ୍ଷର *dup = (ଅକ୍ଷର *) uprv_malloc(len);

    ଯଦି (dup) {
        uprv_memcpy(dup, src, len);
    }

    ଫେରନ୍ତୁ dup;
}

U_CAPI ଅକ୍ଷର* U_EXPORT2
uprv_strndup(ଅଚଳ ଅକ୍ଷର *src, int32_t n) {
    ଅକ୍ଷର *dup;

    ଯଦି(n < 0) {
        dup = uprv_strdup(src);
    } ଅନ୍ୟଥା {
        dup = (ଅକ୍ଷର*)uprv_malloc(n+1);
        ଯଦି (dup) { 
            uprv_memcpy(dup, src, n);
            dup[n] = 0;
        }
    }

    ଫେରନ୍ତୁ dup;
}
