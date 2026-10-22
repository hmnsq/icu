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



#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "uassert.h"

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
uprv_isASCIILetter(అక్షరం c) {
#ఒకవేళ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    తిరిగి
        ('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z') ||
        ('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z');
#లేకపోతే
    తిరిగి ('a'<=c && c<='z') || ('A'<=c && c<='Z');
#అయితే_ముగింపు
}

U_CAPI అక్షరం U_EXPORT2
uprv_toupper(అక్షరం c) {
#ఒకవేళ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    ఒకవేళ(('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z')) {
        c=(అక్షరం)(c+('A'-'a'));
    }
#లేకపోతే
    ఒకవేళ('a'<=c && c<='z') {
        c=(అక్షరం)(c+('A'-'a'));
    }
#అయితే_ముగింపు
    తిరిగి c;
}


#ఒకవేళ 0
/*
 * Commented out because cstring.h defines uprv_tolower() to be
 * the same as either uprv_asciitolower() or uprv_ebcdictolower()
 * to reduce the amount of code to cover with tests.
 *
 * Note that this uprv_tolower() definition is likely to work for most
 * charset families, not just ASCII and EBCDIC, because its #else branch
 * is written generically.
 */
U_CAPI అక్షరం U_EXPORT2
uprv_tolower(అక్షరం c) {
#ఒకవేళ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    ఒకవేళ(('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z')) {
        c=(అక్షరం)(c+('a'-'A'));
    }
#లేకపోతే
    ఒకవేళ('A'<=c && c<='Z') {
        c=(అక్షరం)(c+('a'-'A'));
    }
#అయితే_ముగింపు
    తిరిగి c;
}
#అయితే_ముగింపు

U_CAPI అక్షరం U_EXPORT2
uprv_asciitolower(అక్షరం c) {
    ఒకవేళ(0x41<=c && c<=0x5a) {
        c=(అక్షరం)(c+0x20);
    }
    తిరిగి c;
}

U_CAPI అక్షరం U_EXPORT2
uprv_ebcdictolower(అక్షరం c) {
    ఒకవేళ( (0xc1<=(uint8_t)c && (uint8_t)c<=0xc9) ||
        (0xd1<=(uint8_t)c && (uint8_t)c<=0xd9) ||
        (0xe2<=(uint8_t)c && (uint8_t)c<=0xe9)
    ) {
        c=(అక్షరం)(c-0x40);
    }
    తిరిగి c;
}


U_CAPI అక్షరం* U_EXPORT2
T_CString_గమ్యం_క్రింది_Case(అక్షరం* str)
{
    అక్షరం* origPtr = str;

    ఒకవేళ (str) {
        చేయి
            *str = uprv_tolower(*str);
        అయితే (*(str++));
    }

    తిరిగి origPtr;
}

U_CAPI అక్షరం* U_EXPORT2
T_CString_గమ్యం_పై_Case(అక్షరం* str)
{
    అక్షరం* origPtr = str;

    ఒకవేళ (str) {
        చేయి
            *str = uprv_toupper(*str);
        అయితే (*(str++));
    }

    తిరిగి origPtr;
}

/*
 * Takes a int32_t and fills in  a char* string with that number "radix"-based.
 * Does not handle negative values (makes an empty string for them).
 * Writes at most 12 chars ("-2147483647" plus NUL).
 * Returns the length of the string (not including the NUL).
 */
U_CAPI int32_t U_EXPORT2
T_CString_integer_గమ్యం_String(అక్షరం* బఫరు, int32_t v, int32_t radix)
{
    అక్షరం      tbuf[30];
    int32_t   tbx    = పరిమాణం(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint32_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint32_t) v;
    ఒకవేళ(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint32_t)(-v); 
        బఫరు[length++] = '-';
    }
    
    tbx = పరిమాణం(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    చేయి {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (అక్షరం)(T_CString_itos_స్థానభ్రంశం(digit));
        uval  = uval / radix;
    } అయితే (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(బఫరు+length, tbuf+tbx);
    length += పరిమాణం(tbuf) - tbx -1;
    తిరిగి length;
}



/*
 * Takes a int64_t and fills in  a char* string with that number "radix"-based.
 * Writes at most 21: chars ("-9223372036854775807" plus NUL).
 * Returns the length of the string, not including the terminating NUL.
 */
U_CAPI int32_t U_EXPORT2
T_CString_int64_గమ్యం_String(అక్షరం* బఫరు, int64_t v, uint32_t radix)
{
    అక్షరం      tbuf[30];
    int32_t   tbx    = పరిమాణం(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint64_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint64_t) v;
    ఒకవేళ(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint64_t)(-v); 
        బఫరు[length++] = '-';
    }
    
    tbx = పరిమాణం(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    చేయి {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (అక్షరం)(T_CString_itos_స్థానభ్రంశం(digit));
        uval  = uval / radix;
    } అయితే (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(బఫరు+length, tbuf+tbx);
    length += పరిమాణం(tbuf) - tbx -1;
    తిరిగి length;
}


U_CAPI int32_t U_EXPORT2
T_CString_string_గమ్యం_Integer(స్థిరరాశి అక్షరం *integerString, int32_t radix)
{
    అక్షరం *చివర;
    తిరిగి uprv_strtoul(integerString, &చివర, radix);

}

U_CAPI పూర్ణాంకం U_EXPORT2
uprv_stricmp(స్థిరరాశి అక్షరం *str1, స్థిరరాశి అక్షరం *str2) {
    ఒకవేళ(str1==శూన్యసూచిక) {
        ఒకవేళ(str2==శూన్యసూచిక) {
            తిరిగి 0;
        } లేకపోతే {
            తిరిగి -1;
        }
    } లేకపోతే ఒకవేళ(str2==శూన్యసూచిక) {
        తిరిగి 1;
    } లేకపోతే {
        /* compare non-nullptr strings lexically with lowercase */
        పూర్ణాంకం rc;
        గుర్తులేని అక్షరం c1, c2;

        కొరకు(;;) {
            c1=(గుర్తులేని అక్షరం)*str1;
            c2=(గుర్తులేని అక్షరం)*str2;
            ఒకవేళ(c1==0) {
                ఒకవేళ(c2==0) {
                    తిరిగి 0;
                } లేకపోతే {
                    తిరిగి -1;
                }
            } లేకపోతే ఒకవేళ(c2==0) {
                తిరిగి 1;
            } లేకపోతే {
                /* compare non-zero characters with lowercase */
                rc=(పూర్ణాంకం)(గుర్తులేని అక్షరం)uprv_tolower(c1)-(పూర్ణాంకం)(గుర్తులేని అక్షరం)uprv_tolower(c2);
                ఒకవేళ(rc!=0) {
                    తిరిగి rc;
                }
            }
            ++str1;
            ++str2;
        }
    }
}

U_CAPI పూర్ణాంకం U_EXPORT2
uprv_strnicmp(స్థిరరాశి అక్షరం *str1, స్థిరరాశి అక్షరం *str2, uint32_t n) {
    ఒకవేళ(str1==శూన్యసూచిక) {
        ఒకవేళ(str2==శూన్యసూచిక) {
            తిరిగి 0;
        } లేకపోతే {
            తిరిగి -1;
        }
    } లేకపోతే ఒకవేళ(str2==శూన్యసూచిక) {
        తిరిగి 1;
    } లేకపోతే {
        /* compare non-nullptr strings lexically with lowercase */
        పూర్ణాంకం rc;
        గుర్తులేని అక్షరం c1, c2;

        కొరకు(; n--;) {
            c1=(గుర్తులేని అక్షరం)*str1;
            c2=(గుర్తులేని అక్షరం)*str2;
            ఒకవేళ(c1==0) {
                ఒకవేళ(c2==0) {
                    తిరిగి 0;
                } లేకపోతే {
                    తిరిగి -1;
                }
            } లేకపోతే ఒకవేళ(c2==0) {
                తిరిగి 1;
            } లేకపోతే {
                /* compare non-zero characters with lowercase */
                rc=(పూర్ణాంకం)(గుర్తులేని అక్షరం)uprv_tolower(c1)-(పూర్ణాంకం)(గుర్తులేని అక్షరం)uprv_tolower(c2);
                ఒకవేళ(rc!=0) {
                    తిరిగి rc;
                }
            }
            ++str1;
            ++str2;
        }
    }

    తిరిగి 0;
}

U_CAPI అక్షరం* U_EXPORT2
uprv_strdup(స్థిరరాశి అక్షరం *src) {
    size_t len = uprv_strlen(src) + 1;
    అక్షరం *dup = (అక్షరం *) uprv_malloc(len);

    ఒకవేళ (dup) {
        uprv_memcpy(dup, src, len);
    }

    తిరిగి dup;
}

U_CAPI అక్షరం* U_EXPORT2
uprv_strndup(స్థిరరాశి అక్షరం *src, int32_t n) {
    అక్షరం *dup;

    ఒకవేళ(n < 0) {
        dup = uprv_strdup(src);
    } లేకపోతే {
        dup = (అక్షరం*)uprv_malloc(n+1);
        ఒకవేళ (dup) { 
            uprv_memcpy(dup, src, n);
            dup[n] = 0;
        }
    }

    తిరిగి dup;
}
