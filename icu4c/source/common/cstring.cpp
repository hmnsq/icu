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



#ضمن <stdlib.h>
#ضمن <stdio.h>
#ضمن "unicode/utypes.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "uassert.h"

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
uprv_isASCIILetter(حرف c) {
#إذا U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    أرجع
        ('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z') ||
        ('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z');
#وإلا
    أرجع ('a'<=c && c<='z') || ('A'<=c && c<='Z');
#جيڪڏهن_پورو
}

U_CAPI حرف U_EXPORT2
uprv_toupper(حرف c) {
#إذا U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    إذا(('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z')) {
        c=(حرف)(c+('A'-'a'));
    }
#وإلا
    إذا('a'<=c && c<='z') {
        c=(حرف)(c+('A'-'a'));
    }
#جيڪڏهن_پورو
    أرجع c;
}


#إذا 0
/*
 * Commented out because cstring.h defines uprv_tolower() to be
 * the same as either uprv_asciitolower() or uprv_ebcdictolower()
 * to reduce the amount of code to cover with tests.
 *
 * Note that this uprv_tolower() definition is likely to work for most
 * charset families, not just ASCII and EBCDIC, because its #else branch
 * is written generically.
 */
U_CAPI حرف U_EXPORT2
uprv_tolower(حرف c) {
#إذا U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    إذا(('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z')) {
        c=(حرف)(c+('a'-'A'));
    }
#وإلا
    إذا('A'<=c && c<='Z') {
        c=(حرف)(c+('a'-'A'));
    }
#جيڪڏهن_پورو
    أرجع c;
}
#جيڪڏهن_پورو

U_CAPI حرف U_EXPORT2
uprv_asciitolower(حرف c) {
    إذا(0x41<=c && c<=0x5a) {
        c=(حرف)(c+0x20);
    }
    أرجع c;
}

U_CAPI حرف U_EXPORT2
uprv_ebcdictolower(حرف c) {
    إذا( (0xc1<=(uint8_t)c && (uint8_t)c<=0xc9) ||
        (0xd1<=(uint8_t)c && (uint8_t)c<=0xd9) ||
        (0xe2<=(uint8_t)c && (uint8_t)c<=0xe9)
    ) {
        c=(حرف)(c-0x40);
    }
    أرجع c;
}


U_CAPI حرف* U_EXPORT2
T_CString_منزل_هيٺاهون_Case(حرف* str)
{
    حرف* origPtr = str;

    إذا (str) {
        افعل
            *str = uprv_tolower(*str);
        طالما (*(str++));
    }

    أرجع origPtr;
}

U_CAPI حرف* U_EXPORT2
T_CString_منزل_مٿانهون_Case(حرف* str)
{
    حرف* origPtr = str;

    إذا (str) {
        افعل
            *str = uprv_toupper(*str);
        طالما (*(str++));
    }

    أرجع origPtr;
}

/*
 * Takes a int32_t and fills in  a char* string with that number "radix"-based.
 * Does not handle negative values (makes an empty string for them).
 * Writes at most 12 chars ("-2147483647" plus NUL).
 * Returns the length of the string (not including the NUL).
 */
U_CAPI int32_t U_EXPORT2
T_CString_integer_منزل_String(حرف* بفر, int32_t v, int32_t radix)
{
    حرف      tbuf[30];
    int32_t   tbx    = حجم(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint32_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint32_t) v;
    إذا(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint32_t)(-v); 
        بفر[length++] = '-';
    }
    
    tbx = حجم(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    افعل {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (حرف)(T_CString_itos_وٿي(digit));
        uval  = uval / radix;
    } طالما (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(بفر+length, tbuf+tbx);
    length += حجم(tbuf) - tbx -1;
    أرجع length;
}



/*
 * Takes a int64_t and fills in  a char* string with that number "radix"-based.
 * Writes at most 21: chars ("-9223372036854775807" plus NUL).
 * Returns the length of the string, not including the terminating NUL.
 */
U_CAPI int32_t U_EXPORT2
T_CString_int64_منزل_String(حرف* بفر, int64_t v, uint32_t radix)
{
    حرف      tbuf[30];
    int32_t   tbx    = حجم(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint64_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint64_t) v;
    إذا(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint64_t)(-v); 
        بفر[length++] = '-';
    }
    
    tbx = حجم(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    افعل {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (حرف)(T_CString_itos_وٿي(digit));
        uval  = uval / radix;
    } طالما (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(بفر+length, tbuf+tbx);
    length += حجم(tbuf) - tbx -1;
    أرجع length;
}


U_CAPI int32_t U_EXPORT2
T_CString_string_منزل_Integer(ثابت حرف *integerString, int32_t radix)
{
    حرف *پڇاڙي;
    أرجع uprv_strtoul(integerString, &پڇاڙي, radix);

}

U_CAPI صحيح U_EXPORT2
uprv_stricmp(ثابت حرف *str1, ثابت حرف *str2) {
    إذا(str1==خالي_اشارو) {
        إذا(str2==خالي_اشارو) {
            أرجع 0;
        } وإلا {
            أرجع -1;
        }
    } وإلا إذا(str2==خالي_اشارو) {
        أرجع 1;
    } وإلا {
        /* compare non-nullptr strings lexically with lowercase */
        صحيح rc;
        بنانشان حرف c1, c2;

        لكل(;;) {
            c1=(بنانشان حرف)*str1;
            c2=(بنانشان حرف)*str2;
            إذا(c1==0) {
                إذا(c2==0) {
                    أرجع 0;
                } وإلا {
                    أرجع -1;
                }
            } وإلا إذا(c2==0) {
                أرجع 1;
            } وإلا {
                /* compare non-zero characters with lowercase */
                rc=(صحيح)(بنانشان حرف)uprv_tolower(c1)-(صحيح)(بنانشان حرف)uprv_tolower(c2);
                إذا(rc!=0) {
                    أرجع rc;
                }
            }
            ++str1;
            ++str2;
        }
    }
}

U_CAPI صحيح U_EXPORT2
uprv_strnicmp(ثابت حرف *str1, ثابت حرف *str2, uint32_t n) {
    إذا(str1==خالي_اشارو) {
        إذا(str2==خالي_اشارو) {
            أرجع 0;
        } وإلا {
            أرجع -1;
        }
    } وإلا إذا(str2==خالي_اشارو) {
        أرجع 1;
    } وإلا {
        /* compare non-nullptr strings lexically with lowercase */
        صحيح rc;
        بنانشان حرف c1, c2;

        لكل(; n--;) {
            c1=(بنانشان حرف)*str1;
            c2=(بنانشان حرف)*str2;
            إذا(c1==0) {
                إذا(c2==0) {
                    أرجع 0;
                } وإلا {
                    أرجع -1;
                }
            } وإلا إذا(c2==0) {
                أرجع 1;
            } وإلا {
                /* compare non-zero characters with lowercase */
                rc=(صحيح)(بنانشان حرف)uprv_tolower(c1)-(صحيح)(بنانشان حرف)uprv_tolower(c2);
                إذا(rc!=0) {
                    أرجع rc;
                }
            }
            ++str1;
            ++str2;
        }
    }

    أرجع 0;
}

U_CAPI حرف* U_EXPORT2
uprv_strdup(ثابت حرف *src) {
    size_t len = uprv_strlen(src) + 1;
    حرف *dup = (حرف *) uprv_malloc(len);

    إذا (dup) {
        uprv_memcpy(dup, src, len);
    }

    أرجع dup;
}

U_CAPI حرف* U_EXPORT2
uprv_strndup(ثابت حرف *src, int32_t n) {
    حرف *dup;

    إذا(n < 0) {
        dup = uprv_strdup(src);
    } وإلا {
        dup = (حرف*)uprv_malloc(n+1);
        إذا (dup) { 
            uprv_memcpy(dup, src, n);
            dup[n] = 0;
        }
    }

    أرجع dup;
}
