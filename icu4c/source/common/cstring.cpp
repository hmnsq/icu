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
uprv_isASCIILetter(वर्ण c) {
#यदि U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    प्रत्यागम
        ('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z') ||
        ('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z');
#अन्यथा
    प्रत्यागम ('a'<=c && c<='z') || ('A'<=c && c<='Z');
#यदि_अन्तः
}

U_CAPI वर्ण U_EXPORT2
uprv_toupper(वर्ण c) {
#यदि U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    यदि(('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z')) {
        c=(वर्ण)(c+('A'-'a'));
    }
#अन्यथा
    यदि('a'<=c && c<='z') {
        c=(वर्ण)(c+('A'-'a'));
    }
#यदि_अन्तः
    प्रत्यागम c;
}


#यदि 0
/*
 * Commented out because cstring.h defines uprv_tolower() to be
 * the same as either uprv_asciitolower() or uprv_ebcdictolower()
 * to reduce the amount of code to cover with tests.
 *
 * Note that this uprv_tolower() definition is likely to work for most
 * charset families, not just ASCII and EBCDIC, because its #else branch
 * is written generically.
 */
U_CAPI वर्ण U_EXPORT2
uprv_tolower(वर्ण c) {
#यदि U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    यदि(('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z')) {
        c=(वर्ण)(c+('a'-'A'));
    }
#अन्यथा
    यदि('A'<=c && c<='Z') {
        c=(वर्ण)(c+('a'-'A'));
    }
#यदि_अन्तः
    प्रत्यागम c;
}
#यदि_अन्तः

U_CAPI वर्ण U_EXPORT2
uprv_asciitolower(वर्ण c) {
    यदि(0x41<=c && c<=0x5a) {
        c=(वर्ण)(c+0x20);
    }
    प्रत्यागम c;
}

U_CAPI वर्ण U_EXPORT2
uprv_ebcdictolower(वर्ण c) {
    यदि( (0xc1<=(uint8_t)c && (uint8_t)c<=0xc9) ||
        (0xd1<=(uint8_t)c && (uint8_t)c<=0xd9) ||
        (0xe2<=(uint8_t)c && (uint8_t)c<=0xe9)
    ) {
        c=(वर्ण)(c-0x40);
    }
    प्रत्यागम c;
}


U_CAPI वर्ण* U_EXPORT2
T_CString_गन्तव्यम्_अधस्तनम्_Case(वर्ण* str)
{
    वर्ण* origPtr = str;

    यदि (str) {
        कुरु
            *str = uprv_tolower(*str);
        यावत् (*(str++));
    }

    प्रत्यागम origPtr;
}

U_CAPI वर्ण* U_EXPORT2
T_CString_गन्तव्यम्_ऊर्ध्वस्थम्_Case(वर्ण* str)
{
    वर्ण* origPtr = str;

    यदि (str) {
        कुरु
            *str = uprv_toupper(*str);
        यावत् (*(str++));
    }

    प्रत्यागम origPtr;
}

/*
 * Takes a int32_t and fills in  a char* string with that number "radix"-based.
 * Does not handle negative values (makes an empty string for them).
 * Writes at most 12 chars ("-2147483647" plus NUL).
 * Returns the length of the string (not including the NUL).
 */
U_CAPI int32_t U_EXPORT2
T_CString_integer_गन्तव्यम्_String(वर्ण* बफर, int32_t v, int32_t radix)
{
    वर्ण      tbuf[30];
    int32_t   tbx    = परिमाण(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint32_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint32_t) v;
    यदि(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint32_t)(-v); 
        बफर[length++] = '-';
    }
    
    tbx = परिमाण(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    कुरु {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (वर्ण)(T_CString_itos_विस्थापनम्(digit));
        uval  = uval / radix;
    } यावत् (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(बफर+length, tbuf+tbx);
    length += परिमाण(tbuf) - tbx -1;
    प्रत्यागम length;
}



/*
 * Takes a int64_t and fills in  a char* string with that number "radix"-based.
 * Writes at most 21: chars ("-9223372036854775807" plus NUL).
 * Returns the length of the string, not including the terminating NUL.
 */
U_CAPI int32_t U_EXPORT2
T_CString_int64_गन्तव्यम्_String(वर्ण* बफर, int64_t v, uint32_t radix)
{
    वर्ण      tbuf[30];
    int32_t   tbx    = परिमाण(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint64_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint64_t) v;
    यदि(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint64_t)(-v); 
        बफर[length++] = '-';
    }
    
    tbx = परिमाण(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    कुरु {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (वर्ण)(T_CString_itos_विस्थापनम्(digit));
        uval  = uval / radix;
    } यावत् (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(बफर+length, tbuf+tbx);
    length += परिमाण(tbuf) - tbx -1;
    प्रत्यागम length;
}


U_CAPI int32_t U_EXPORT2
T_CString_string_गन्तव्यम्_Integer(ध्रुव वर्ण *integerString, int32_t radix)
{
    वर्ण *अन्तः;
    प्रत्यागम uprv_strtoul(integerString, &अन्तः, radix);

}

U_CAPI पूर्णाङ्क U_EXPORT2
uprv_stricmp(ध्रुव वर्ण *str1, ध्रुव वर्ण *str2) {
    यदि(str1==शून्यसूचकः) {
        यदि(str2==शून्यसूचकः) {
            प्रत्यागम 0;
        } अन्यथा {
            प्रत्यागम -1;
        }
    } अन्यथा यदि(str2==शून्यसूचकः) {
        प्रत्यागम 1;
    } अन्यथा {
        /* compare non-nullptr strings lexically with lowercase */
        पूर्णाङ्क rc;
        अचिह्नित वर्ण c1, c2;

        प्रति(;;) {
            c1=(अचिह्नित वर्ण)*str1;
            c2=(अचिह्नित वर्ण)*str2;
            यदि(c1==0) {
                यदि(c2==0) {
                    प्रत्यागम 0;
                } अन्यथा {
                    प्रत्यागम -1;
                }
            } अन्यथा यदि(c2==0) {
                प्रत्यागम 1;
            } अन्यथा {
                /* compare non-zero characters with lowercase */
                rc=(पूर्णाङ्क)(अचिह्नित वर्ण)uprv_tolower(c1)-(पूर्णाङ्क)(अचिह्नित वर्ण)uprv_tolower(c2);
                यदि(rc!=0) {
                    प्रत्यागम rc;
                }
            }
            ++str1;
            ++str2;
        }
    }
}

U_CAPI पूर्णाङ्क U_EXPORT2
uprv_strnicmp(ध्रुव वर्ण *str1, ध्रुव वर्ण *str2, uint32_t n) {
    यदि(str1==शून्यसूचकः) {
        यदि(str2==शून्यसूचकः) {
            प्रत्यागम 0;
        } अन्यथा {
            प्रत्यागम -1;
        }
    } अन्यथा यदि(str2==शून्यसूचकः) {
        प्रत्यागम 1;
    } अन्यथा {
        /* compare non-nullptr strings lexically with lowercase */
        पूर्णाङ्क rc;
        अचिह्नित वर्ण c1, c2;

        प्रति(; n--;) {
            c1=(अचिह्नित वर्ण)*str1;
            c2=(अचिह्नित वर्ण)*str2;
            यदि(c1==0) {
                यदि(c2==0) {
                    प्रत्यागम 0;
                } अन्यथा {
                    प्रत्यागम -1;
                }
            } अन्यथा यदि(c2==0) {
                प्रत्यागम 1;
            } अन्यथा {
                /* compare non-zero characters with lowercase */
                rc=(पूर्णाङ्क)(अचिह्नित वर्ण)uprv_tolower(c1)-(पूर्णाङ्क)(अचिह्नित वर्ण)uprv_tolower(c2);
                यदि(rc!=0) {
                    प्रत्यागम rc;
                }
            }
            ++str1;
            ++str2;
        }
    }

    प्रत्यागम 0;
}

U_CAPI वर्ण* U_EXPORT2
uprv_strdup(ध्रुव वर्ण *src) {
    size_t len = uprv_strlen(src) + 1;
    वर्ण *dup = (वर्ण *) uprv_malloc(len);

    यदि (dup) {
        uprv_memcpy(dup, src, len);
    }

    प्रत्यागम dup;
}

U_CAPI वर्ण* U_EXPORT2
uprv_strndup(ध्रुव वर्ण *src, int32_t n) {
    वर्ण *dup;

    यदि(n < 0) {
        dup = uprv_strdup(src);
    } अन्यथा {
        dup = (वर्ण*)uprv_malloc(n+1);
        यदि (dup) { 
            uprv_memcpy(dup, src, n);
            dup[n] = 0;
        }
    }

    प्रत्यागम dup;
}
