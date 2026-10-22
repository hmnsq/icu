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



#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "uassert.h"

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
uprv_isASCIILetter(अक्षर c) {
#जर U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    परत
        ('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z') ||
        ('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z');
#अन्यथा
    परत ('a'<=c && c<='z') || ('A'<=c && c<='Z');
#endif
}

U_CAPI अक्षर U_EXPORT2
uprv_toupper(अक्षर c) {
#जर U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    जर(('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z')) {
        c=(अक्षर)(c+('A'-'a'));
    }
#अन्यथा
    जर('a'<=c && c<='z') {
        c=(अक्षर)(c+('A'-'a'));
    }
#endif
    परत c;
}


#जर 0
/*
 * Commented out because cstring.h defines uprv_tolower() to be
 * the same as either uprv_asciitolower() or uprv_ebcdictolower()
 * to reduce the amount of code to cover with tests.
 *
 * Note that this uprv_tolower() definition is likely to work for most
 * charset families, not just ASCII and EBCDIC, because its #else branch
 * is written generically.
 */
U_CAPI अक्षर U_EXPORT2
uprv_tolower(अक्षर c) {
#जर U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    जर(('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z')) {
        c=(अक्षर)(c+('a'-'A'));
    }
#अन्यथा
    जर('A'<=c && c<='Z') {
        c=(अक्षर)(c+('a'-'A'));
    }
#endif
    परत c;
}
#endif

U_CAPI अक्षर U_EXPORT2
uprv_asciitolower(अक्षर c) {
    जर(0x41<=c && c<=0x5a) {
        c=(अक्षर)(c+0x20);
    }
    परत c;
}

U_CAPI अक्षर U_EXPORT2
uprv_ebcdictolower(अक्षर c) {
    जर( (0xc1<=(uint8_t)c && (uint8_t)c<=0xc9) ||
        (0xd1<=(uint8_t)c && (uint8_t)c<=0xd9) ||
        (0xe2<=(uint8_t)c && (uint8_t)c<=0xe9)
    ) {
        c=(अक्षर)(c-0x40);
    }
    परत c;
}


U_CAPI अक्षर* U_EXPORT2
T_CString_toLowerCase(अक्षर* str)
{
    अक्षर* origPtr = str;

    जर (str) {
        करा
            *str = uprv_tolower(*str);
        पर्यंत (*(str++));
    }

    परत origPtr;
}

U_CAPI अक्षर* U_EXPORT2
T_CString_toUpperCase(अक्षर* str)
{
    अक्षर* origPtr = str;

    जर (str) {
        करा
            *str = uprv_toupper(*str);
        पर्यंत (*(str++));
    }

    परत origPtr;
}

/*
 * Takes a int32_t and fills in  a char* string with that number "radix"-based.
 * Does not handle negative values (makes an empty string for them).
 * Writes at most 12 chars ("-2147483647" plus NUL).
 * Returns the length of the string (not including the NUL).
 */
U_CAPI int32_t U_EXPORT2
T_CString_integerToString(अक्षर* buffer, int32_t v, int32_t radix)
{
    अक्षर      tbuf[30];
    int32_t   tbx    = आकार(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint32_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint32_t) v;
    जर(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint32_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = आकार(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    करा {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (अक्षर)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } पर्यंत (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += आकार(tbuf) - tbx -1;
    परत length;
}



/*
 * Takes a int64_t and fills in  a char* string with that number "radix"-based.
 * Writes at most 21: chars ("-9223372036854775807" plus NUL).
 * Returns the length of the string, not including the terminating NUL.
 */
U_CAPI int32_t U_EXPORT2
T_CString_int64ToString(अक्षर* buffer, int64_t v, uint32_t radix)
{
    अक्षर      tbuf[30];
    int32_t   tbx    = आकार(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint64_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint64_t) v;
    जर(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint64_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = आकार(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    करा {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (अक्षर)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } पर्यंत (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += आकार(tbuf) - tbx -1;
    परत length;
}


U_CAPI int32_t U_EXPORT2
T_CString_stringToInteger(स्थिरांक अक्षर *integerString, int32_t radix)
{
    अक्षर *end;
    परत uprv_strtoul(integerString, &end, radix);

}

U_CAPI पूर्णांक U_EXPORT2
uprv_stricmp(स्थिरांक अक्षर *str1, स्थिरांक अक्षर *str2) {
    जर(str1==nullptr) {
        जर(str2==nullptr) {
            परत 0;
        } अन्यथा {
            परत -1;
        }
    } अन्यथा जर(str2==nullptr) {
        परत 1;
    } अन्यथा {
        /* compare non-nullptr strings lexically with lowercase */
        पूर्णांक rc;
        अचिन्हित अक्षर c1, c2;

        साठी(;;) {
            c1=(अचिन्हित अक्षर)*str1;
            c2=(अचिन्हित अक्षर)*str2;
            जर(c1==0) {
                जर(c2==0) {
                    परत 0;
                } अन्यथा {
                    परत -1;
                }
            } अन्यथा जर(c2==0) {
                परत 1;
            } अन्यथा {
                /* compare non-zero characters with lowercase */
                rc=(पूर्णांक)(अचिन्हित अक्षर)uprv_tolower(c1)-(पूर्णांक)(अचिन्हित अक्षर)uprv_tolower(c2);
                जर(rc!=0) {
                    परत rc;
                }
            }
            ++str1;
            ++str2;
        }
    }
}

U_CAPI पूर्णांक U_EXPORT2
uprv_strnicmp(स्थिरांक अक्षर *str1, स्थिरांक अक्षर *str2, uint32_t n) {
    जर(str1==nullptr) {
        जर(str2==nullptr) {
            परत 0;
        } अन्यथा {
            परत -1;
        }
    } अन्यथा जर(str2==nullptr) {
        परत 1;
    } अन्यथा {
        /* compare non-nullptr strings lexically with lowercase */
        पूर्णांक rc;
        अचिन्हित अक्षर c1, c2;

        साठी(; n--;) {
            c1=(अचिन्हित अक्षर)*str1;
            c2=(अचिन्हित अक्षर)*str2;
            जर(c1==0) {
                जर(c2==0) {
                    परत 0;
                } अन्यथा {
                    परत -1;
                }
            } अन्यथा जर(c2==0) {
                परत 1;
            } अन्यथा {
                /* compare non-zero characters with lowercase */
                rc=(पूर्णांक)(अचिन्हित अक्षर)uprv_tolower(c1)-(पूर्णांक)(अचिन्हित अक्षर)uprv_tolower(c2);
                जर(rc!=0) {
                    परत rc;
                }
            }
            ++str1;
            ++str2;
        }
    }

    परत 0;
}

U_CAPI अक्षर* U_EXPORT2
uprv_strdup(स्थिरांक अक्षर *src) {
    माप_प्रकार len = uprv_strlen(src) + 1;
    अक्षर *dup = (अक्षर *) uprv_malloc(len);

    जर (dup) {
        uprv_memcpy(dup, src, len);
    }

    परत dup;
}

U_CAPI अक्षर* U_EXPORT2
uprv_strndup(स्थिरांक अक्षर *src, int32_t n) {
    अक्षर *dup;

    जर(n < 0) {
        dup = uprv_strdup(src);
    } अन्यथा {
        dup = (अक्षर*)uprv_malloc(n+1);
        जर (dup) { 
            uprv_memcpy(dup, src, n);
            dup[n] = 0;
        }
    }

    परत dup;
}
