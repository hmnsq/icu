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



#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "uassert.h"

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
uprv_isASCIILetter(અક્ષર c) {
#જો U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    પાછા
        ('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z') ||
        ('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z');
#નહીંતર
    પાછા ('a'<=c && c<='z') || ('A'<=c && c<='Z');
#endif
}

U_CAPI અક્ષર U_EXPORT2
uprv_toupper(અક્ષર c) {
#જો U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    જો(('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z')) {
        c=(અક્ષર)(c+('A'-'a'));
    }
#નહીંતર
    જો('a'<=c && c<='z') {
        c=(અક્ષર)(c+('A'-'a'));
    }
#endif
    પાછા c;
}


#જો 0
/*
 * Commented out because cstring.h defines uprv_tolower() to be
 * the same as either uprv_asciitolower() or uprv_ebcdictolower()
 * to reduce the amount of code to cover with tests.
 *
 * Note that this uprv_tolower() definition is likely to work for most
 * charset families, not just ASCII and EBCDIC, because its #else branch
 * is written generically.
 */
U_CAPI અક્ષર U_EXPORT2
uprv_tolower(અક્ષર c) {
#જો U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    જો(('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z')) {
        c=(અક્ષર)(c+('a'-'A'));
    }
#નહીંતર
    જો('A'<=c && c<='Z') {
        c=(અક્ષર)(c+('a'-'A'));
    }
#endif
    પાછા c;
}
#endif

U_CAPI અક્ષર U_EXPORT2
uprv_asciitolower(અક્ષર c) {
    જો(0x41<=c && c<=0x5a) {
        c=(અક્ષર)(c+0x20);
    }
    પાછા c;
}

U_CAPI અક્ષર U_EXPORT2
uprv_ebcdictolower(અક્ષર c) {
    જો( (0xc1<=(uint8_t)c && (uint8_t)c<=0xc9) ||
        (0xd1<=(uint8_t)c && (uint8_t)c<=0xd9) ||
        (0xe2<=(uint8_t)c && (uint8_t)c<=0xe9)
    ) {
        c=(અક્ષર)(c-0x40);
    }
    પાછા c;
}


U_CAPI અક્ષર* U_EXPORT2
T_CString_toLowerCase(અક્ષર* str)
{
    અક્ષર* origPtr = str;

    જો (str) {
        કરો
            *str = uprv_tolower(*str);
        જ્યારે (*(str++));
    }

    પાછા origPtr;
}

U_CAPI અક્ષર* U_EXPORT2
T_CString_toUpperCase(અક્ષર* str)
{
    અક્ષર* origPtr = str;

    જો (str) {
        કરો
            *str = uprv_toupper(*str);
        જ્યારે (*(str++));
    }

    પાછા origPtr;
}

/*
 * Takes a int32_t and fills in  a char* string with that number "radix"-based.
 * Does not handle negative values (makes an empty string for them).
 * Writes at most 12 chars ("-2147483647" plus NUL).
 * Returns the length of the string (not including the NUL).
 */
U_CAPI int32_t U_EXPORT2
T_CString_integerToString(અક્ષર* buffer, int32_t v, int32_t radix)
{
    અક્ષર      tbuf[30];
    int32_t   tbx    = કદ(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint32_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint32_t) v;
    જો(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint32_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = કદ(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    કરો {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (અક્ષર)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } જ્યારે (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += કદ(tbuf) - tbx -1;
    પાછા length;
}



/*
 * Takes a int64_t and fills in  a char* string with that number "radix"-based.
 * Writes at most 21: chars ("-9223372036854775807" plus NUL).
 * Returns the length of the string, not including the terminating NUL.
 */
U_CAPI int32_t U_EXPORT2
T_CString_int64ToString(અક્ષર* buffer, int64_t v, uint32_t radix)
{
    અક્ષર      tbuf[30];
    int32_t   tbx    = કદ(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint64_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint64_t) v;
    જો(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint64_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = કદ(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    કરો {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (અક્ષર)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } જ્યારે (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += કદ(tbuf) - tbx -1;
    પાછા length;
}


U_CAPI int32_t U_EXPORT2
T_CString_stringToInteger(અચલ અક્ષર *integerString, int32_t radix)
{
    અક્ષર *end;
    પાછા uprv_strtoul(integerString, &end, radix);

}

U_CAPI પૂર્ણાંક U_EXPORT2
uprv_stricmp(અચલ અક્ષર *str1, અચલ અક્ષર *str2) {
    જો(str1==nullptr) {
        જો(str2==nullptr) {
            પાછા 0;
        } નહીંતર {
            પાછા -1;
        }
    } નહીંતર જો(str2==nullptr) {
        પાછા 1;
    } નહીંતર {
        /* compare non-nullptr strings lexically with lowercase */
        પૂર્ણાંક rc;
        unsigned અક્ષર c1, c2;

        માટે(;;) {
            c1=(unsigned અક્ષર)*str1;
            c2=(unsigned અક્ષર)*str2;
            જો(c1==0) {
                જો(c2==0) {
                    પાછા 0;
                } નહીંતર {
                    પાછા -1;
                }
            } નહીંતર જો(c2==0) {
                પાછા 1;
            } નહીંતર {
                /* compare non-zero characters with lowercase */
                rc=(પૂર્ણાંક)(unsigned અક્ષર)uprv_tolower(c1)-(પૂર્ણાંક)(unsigned અક્ષર)uprv_tolower(c2);
                જો(rc!=0) {
                    પાછા rc;
                }
            }
            ++str1;
            ++str2;
        }
    }
}

U_CAPI પૂર્ણાંક U_EXPORT2
uprv_strnicmp(અચલ અક્ષર *str1, અચલ અક્ષર *str2, uint32_t n) {
    જો(str1==nullptr) {
        જો(str2==nullptr) {
            પાછા 0;
        } નહીંતર {
            પાછા -1;
        }
    } નહીંતર જો(str2==nullptr) {
        પાછા 1;
    } નહીંતર {
        /* compare non-nullptr strings lexically with lowercase */
        પૂર્ણાંક rc;
        unsigned અક્ષર c1, c2;

        માટે(; n--;) {
            c1=(unsigned અક્ષર)*str1;
            c2=(unsigned અક્ષર)*str2;
            જો(c1==0) {
                જો(c2==0) {
                    પાછા 0;
                } નહીંતર {
                    પાછા -1;
                }
            } નહીંતર જો(c2==0) {
                પાછા 1;
            } નહીંતર {
                /* compare non-zero characters with lowercase */
                rc=(પૂર્ણાંક)(unsigned અક્ષર)uprv_tolower(c1)-(પૂર્ણાંક)(unsigned અક્ષર)uprv_tolower(c2);
                જો(rc!=0) {
                    પાછા rc;
                }
            }
            ++str1;
            ++str2;
        }
    }

    પાછા 0;
}

U_CAPI અક્ષર* U_EXPORT2
uprv_strdup(અચલ અક્ષર *src) {
    size_t len = uprv_strlen(src) + 1;
    અક્ષર *dup = (અક્ષર *) uprv_malloc(len);

    જો (dup) {
        uprv_memcpy(dup, src, len);
    }

    પાછા dup;
}

U_CAPI અક્ષર* U_EXPORT2
uprv_strndup(અચલ અક્ષર *src, int32_t n) {
    અક્ષર *dup;

    જો(n < 0) {
        dup = uprv_strdup(src);
    } નહીંતર {
        dup = (અક્ષર*)uprv_malloc(n+1);
        જો (dup) { 
            uprv_memcpy(dup, src, n);
            dup[n] = 0;
        }
    }

    પાછા dup;
}
