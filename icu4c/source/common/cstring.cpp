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



#شامل <stdlib.h>
#شامل <stdio.h>
#شامل "unicode/utypes.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "uassert.h"

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
uprv_isASCIILetter(نویسه c) {
#اگر U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    بازگشت
        ('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z') ||
        ('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z');
#وگرنه
    بازگشت ('a'<=c && c<='z') || ('A'<=c && c<='Z');
#endif
}

U_CAPI نویسه U_EXPORT2
uprv_toupper(نویسه c) {
#اگر U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    اگر(('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z')) {
        c=(نویسه)(c+('A'-'a'));
    }
#وگرنه
    اگر('a'<=c && c<='z') {
        c=(نویسه)(c+('A'-'a'));
    }
#endif
    بازگشت c;
}


#اگر 0
/*
 * Commented out because cstring.h defines uprv_tolower() to be
 * the same as either uprv_asciitolower() or uprv_ebcdictolower()
 * to reduce the amount of code to cover with tests.
 *
 * Note that this uprv_tolower() definition is likely to work for most
 * charset families, not just ASCII and EBCDIC, because its #else branch
 * is written generically.
 */
U_CAPI نویسه U_EXPORT2
uprv_tolower(نویسه c) {
#اگر U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    اگر(('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z')) {
        c=(نویسه)(c+('a'-'A'));
    }
#وگرنه
    اگر('A'<=c && c<='Z') {
        c=(نویسه)(c+('a'-'A'));
    }
#endif
    بازگشت c;
}
#endif

U_CAPI نویسه U_EXPORT2
uprv_asciitolower(نویسه c) {
    اگر(0x41<=c && c<=0x5a) {
        c=(نویسه)(c+0x20);
    }
    بازگشت c;
}

U_CAPI نویسه U_EXPORT2
uprv_ebcdictolower(نویسه c) {
    اگر( (0xc1<=(uint8_t)c && (uint8_t)c<=0xc9) ||
        (0xd1<=(uint8_t)c && (uint8_t)c<=0xd9) ||
        (0xe2<=(uint8_t)c && (uint8_t)c<=0xe9)
    ) {
        c=(نویسه)(c-0x40);
    }
    بازگشت c;
}


U_CAPI نویسه* U_EXPORT2
T_CString_toLowerCase(نویسه* str)
{
    نویسه* origPtr = str;

    اگر (str) {
        انجامبده
            *str = uprv_tolower(*str);
        وقتی (*(str++));
    }

    بازگشت origPtr;
}

U_CAPI نویسه* U_EXPORT2
T_CString_toUpperCase(نویسه* str)
{
    نویسه* origPtr = str;

    اگر (str) {
        انجامبده
            *str = uprv_toupper(*str);
        وقتی (*(str++));
    }

    بازگشت origPtr;
}

/*
 * Takes a int32_t and fills in  a char* string with that number "radix"-based.
 * Does not handle negative values (makes an empty string for them).
 * Writes at most 12 chars ("-2147483647" plus NUL).
 * Returns the length of the string (not including the NUL).
 */
U_CAPI int32_t U_EXPORT2
T_CString_integerToString(نویسه* buffer, int32_t v, int32_t radix)
{
    نویسه      tbuf[30];
    int32_t   tbx    = اندازه(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint32_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint32_t) v;
    اگر(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint32_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = اندازه(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    انجامبده {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (نویسه)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } وقتی (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += اندازه(tbuf) - tbx -1;
    بازگشت length;
}



/*
 * Takes a int64_t and fills in  a char* string with that number "radix"-based.
 * Writes at most 21: chars ("-9223372036854775807" plus NUL).
 * Returns the length of the string, not including the terminating NUL.
 */
U_CAPI int32_t U_EXPORT2
T_CString_int64ToString(نویسه* buffer, int64_t v, uint32_t radix)
{
    نویسه      tbuf[30];
    int32_t   tbx    = اندازه(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint64_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint64_t) v;
    اگر(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint64_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = اندازه(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    انجامبده {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (نویسه)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } وقتی (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += اندازه(tbuf) - tbx -1;
    بازگشت length;
}


U_CAPI int32_t U_EXPORT2
T_CString_stringToInteger(ثابت نویسه *integerString, int32_t radix)
{
    نویسه *end;
    بازگشت uprv_strtoul(integerString, &end, radix);

}

U_CAPI صحیح U_EXPORT2
uprv_stricmp(ثابت نویسه *str1, ثابت نویسه *str2) {
    اگر(str1==nullptr) {
        اگر(str2==nullptr) {
            بازگشت 0;
        } وگرنه {
            بازگشت -1;
        }
    } وگرنه اگر(str2==nullptr) {
        بازگشت 1;
    } وگرنه {
        /* compare non-nullptr strings lexically with lowercase */
        صحیح rc;
        unsigned نویسه c1, c2;

        برای(;;) {
            c1=(unsigned نویسه)*str1;
            c2=(unsigned نویسه)*str2;
            اگر(c1==0) {
                اگر(c2==0) {
                    بازگشت 0;
                } وگرنه {
                    بازگشت -1;
                }
            } وگرنه اگر(c2==0) {
                بازگشت 1;
            } وگرنه {
                /* compare non-zero characters with lowercase */
                rc=(صحیح)(unsigned نویسه)uprv_tolower(c1)-(صحیح)(unsigned نویسه)uprv_tolower(c2);
                اگر(rc!=0) {
                    بازگشت rc;
                }
            }
            ++str1;
            ++str2;
        }
    }
}

U_CAPI صحیح U_EXPORT2
uprv_strnicmp(ثابت نویسه *str1, ثابت نویسه *str2, uint32_t n) {
    اگر(str1==nullptr) {
        اگر(str2==nullptr) {
            بازگشت 0;
        } وگرنه {
            بازگشت -1;
        }
    } وگرنه اگر(str2==nullptr) {
        بازگشت 1;
    } وگرنه {
        /* compare non-nullptr strings lexically with lowercase */
        صحیح rc;
        unsigned نویسه c1, c2;

        برای(; n--;) {
            c1=(unsigned نویسه)*str1;
            c2=(unsigned نویسه)*str2;
            اگر(c1==0) {
                اگر(c2==0) {
                    بازگشت 0;
                } وگرنه {
                    بازگشت -1;
                }
            } وگرنه اگر(c2==0) {
                بازگشت 1;
            } وگرنه {
                /* compare non-zero characters with lowercase */
                rc=(صحیح)(unsigned نویسه)uprv_tolower(c1)-(صحیح)(unsigned نویسه)uprv_tolower(c2);
                اگر(rc!=0) {
                    بازگشت rc;
                }
            }
            ++str1;
            ++str2;
        }
    }

    بازگشت 0;
}

U_CAPI نویسه* U_EXPORT2
uprv_strdup(ثابت نویسه *src) {
    size_t len = uprv_strlen(src) + 1;
    نویسه *dup = (نویسه *) uprv_malloc(len);

    اگر (dup) {
        uprv_memcpy(dup, src, len);
    }

    بازگشت dup;
}

U_CAPI نویسه* U_EXPORT2
uprv_strndup(ثابت نویسه *src, int32_t n) {
    نویسه *dup;

    اگر(n < 0) {
        dup = uprv_strdup(src);
    } وگرنه {
        dup = (نویسه*)uprv_malloc(n+1);
        اگر (dup) { 
            uprv_memcpy(dup, src, n);
            dup[n] = 0;
        }
    }

    بازگشت dup;
}
