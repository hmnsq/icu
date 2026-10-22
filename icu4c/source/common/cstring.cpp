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



#include <stdlib.h>
#include <stdio.h>
#include "unicode/utypes.h"
#include "cmemory.h"
#include "cstring.h"
#include "uassert.h"

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
uprv_isASCIILetter(char c) {
#もし U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    戻る
        ('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z') ||
        ('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z');
#否则
    戻る ('a'<=c && c<='z') || ('A'<=c && c<='Z');
#endif
}

U_CAPI char U_EXPORT2
uprv_toupper(char c) {
#もし U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    もし(('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z')) {
        c=(char)(c+('A'-'a'));
    }
#否则
    もし('a'<=c && c<='z') {
        c=(char)(c+('A'-'a'));
    }
#endif
    戻る c;
}


#もし 0
/*
 * Commented out because cstring.h defines uprv_tolower() to be
 * the same as either uprv_asciitolower() or uprv_ebcdictolower()
 * to reduce the amount of code to cover with tests.
 *
 * Note that this uprv_tolower() definition is likely to work for most
 * charset families, not just ASCII and EBCDIC, because its #else branch
 * is written generically.
 */
U_CAPI char U_EXPORT2
uprv_tolower(char c) {
#もし U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    もし(('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z')) {
        c=(char)(c+('a'-'A'));
    }
#否则
    もし('A'<=c && c<='Z') {
        c=(char)(c+('a'-'A'));
    }
#endif
    戻る c;
}
#endif

U_CAPI char U_EXPORT2
uprv_asciitolower(char c) {
    もし(0x41<=c && c<=0x5a) {
        c=(char)(c+0x20);
    }
    戻る c;
}

U_CAPI char U_EXPORT2
uprv_ebcdictolower(char c) {
    もし( (0xc1<=(uint8_t)c && (uint8_t)c<=0xc9) ||
        (0xd1<=(uint8_t)c && (uint8_t)c<=0xd9) ||
        (0xe2<=(uint8_t)c && (uint8_t)c<=0xe9)
    ) {
        c=(char)(c-0x40);
    }
    戻る c;
}


U_CAPI char* U_EXPORT2
T_CString_toLowerCase(char* str)
{
    char* origPtr = str;

    もし (str) {
        do
            *str = uprv_tolower(*str);
        ながら (*(str++));
    }

    戻る origPtr;
}

U_CAPI char* U_EXPORT2
T_CString_toUpperCase(char* str)
{
    char* origPtr = str;

    もし (str) {
        do
            *str = uprv_toupper(*str);
        ながら (*(str++));
    }

    戻る origPtr;
}

/*
 * Takes a int32_t and fills in  a char* string with that number "radix"-based.
 * Does not handle negative values (makes an empty string for them).
 * Writes at most 12 chars ("-2147483647" plus NUL).
 * Returns the length of the string (not including the NUL).
 */
U_CAPI int32_t U_EXPORT2
T_CString_integerToString(char* buffer, int32_t v, int32_t radix)
{
    char      tbuf[30];
    int32_t   tbx    = sizeof(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint32_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint32_t) v;
    もし(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint32_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = sizeof(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    do {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (char)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } ながら (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += sizeof(tbuf) - tbx -1;
    戻る length;
}



/*
 * Takes a int64_t and fills in  a char* string with that number "radix"-based.
 * Writes at most 21: chars ("-9223372036854775807" plus NUL).
 * Returns the length of the string, not including the terminating NUL.
 */
U_CAPI int32_t U_EXPORT2
T_CString_int64ToString(char* buffer, int64_t v, uint32_t radix)
{
    char      tbuf[30];
    int32_t   tbx    = sizeof(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint64_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint64_t) v;
    もし(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint64_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = sizeof(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    do {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (char)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } ながら (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += sizeof(tbuf) - tbx -1;
    戻る length;
}


U_CAPI int32_t U_EXPORT2
T_CString_stringToInteger(const char *integerString, int32_t radix)
{
    char *end;
    戻る uprv_strtoul(integerString, &end, radix);

}

U_CAPI 整数 U_EXPORT2
uprv_stricmp(const char *str1, const char *str2) {
    もし(str1==nullptr) {
        もし(str2==nullptr) {
            戻る 0;
        } 否则 {
            戻る -1;
        }
    } 否则 もし(str2==nullptr) {
        戻る 1;
    } 否则 {
        /* compare non-nullptr strings lexically with lowercase */
        整数 rc;
        unsigned char c1, c2;

        ために(;;) {
            c1=(unsigned char)*str1;
            c2=(unsigned char)*str2;
            もし(c1==0) {
                もし(c2==0) {
                    戻る 0;
                } 否则 {
                    戻る -1;
                }
            } 否则 もし(c2==0) {
                戻る 1;
            } 否则 {
                /* compare non-zero characters with lowercase */
                rc=(整数)(unsigned char)uprv_tolower(c1)-(整数)(unsigned char)uprv_tolower(c2);
                もし(rc!=0) {
                    戻る rc;
                }
            }
            ++str1;
            ++str2;
        }
    }
}

U_CAPI 整数 U_EXPORT2
uprv_strnicmp(const char *str1, const char *str2, uint32_t n) {
    もし(str1==nullptr) {
        もし(str2==nullptr) {
            戻る 0;
        } 否则 {
            戻る -1;
        }
    } 否则 もし(str2==nullptr) {
        戻る 1;
    } 否则 {
        /* compare non-nullptr strings lexically with lowercase */
        整数 rc;
        unsigned char c1, c2;

        ために(; n--;) {
            c1=(unsigned char)*str1;
            c2=(unsigned char)*str2;
            もし(c1==0) {
                もし(c2==0) {
                    戻る 0;
                } 否则 {
                    戻る -1;
                }
            } 否则 もし(c2==0) {
                戻る 1;
            } 否则 {
                /* compare non-zero characters with lowercase */
                rc=(整数)(unsigned char)uprv_tolower(c1)-(整数)(unsigned char)uprv_tolower(c2);
                もし(rc!=0) {
                    戻る rc;
                }
            }
            ++str1;
            ++str2;
        }
    }

    戻る 0;
}

U_CAPI char* U_EXPORT2
uprv_strdup(const char *src) {
    size_t len = uprv_strlen(src) + 1;
    char *dup = (char *) uprv_malloc(len);

    もし (dup) {
        uprv_memcpy(dup, src, len);
    }

    戻る dup;
}

U_CAPI char* U_EXPORT2
uprv_strndup(const char *src, int32_t n) {
    char *dup;

    もし(n < 0) {
        dup = uprv_strdup(src);
    } 否则 {
        dup = (char*)uprv_malloc(n+1);
        もし (dup) { 
            uprv_memcpy(dup, src, n);
            dup[n] = 0;
        }
    }

    戻る dup;
}
