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



#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "uassert.h"

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
uprv_isASCIILetter(ৱর্ণ c) {
#যদি U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    ঘূৰাই
        ('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z') ||
        ('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z');
#অন্যথা
    ঘূৰাই ('a'<=c && c<='z') || ('A'<=c && c<='Z');
#endif
}

U_CAPI ৱর্ণ U_EXPORT2
uprv_toupper(ৱর্ণ c) {
#যদি U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    যদি(('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z')) {
        c=(ৱর্ণ)(c+('A'-'a'));
    }
#অন্যথা
    যদি('a'<=c && c<='z') {
        c=(ৱর্ণ)(c+('A'-'a'));
    }
#endif
    ঘূৰাই c;
}


#যদি 0
/*
 * Commented out because cstring.h defines uprv_tolower() to be
 * the same as either uprv_asciitolower() or uprv_ebcdictolower()
 * to reduce the amount of code to cover with tests.
 *
 * Note that this uprv_tolower() definition is likely to work for most
 * charset families, not just ASCII and EBCDIC, because its #else branch
 * is written generically.
 */
U_CAPI ৱর্ণ U_EXPORT2
uprv_tolower(ৱর্ণ c) {
#যদি U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    যদি(('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z')) {
        c=(ৱর্ণ)(c+('a'-'A'));
    }
#অন্যথা
    যদি('A'<=c && c<='Z') {
        c=(ৱর্ণ)(c+('a'-'A'));
    }
#endif
    ঘূৰাই c;
}
#endif

U_CAPI ৱর্ণ U_EXPORT2
uprv_asciitolower(ৱর্ণ c) {
    যদি(0x41<=c && c<=0x5a) {
        c=(ৱর্ণ)(c+0x20);
    }
    ঘূৰাই c;
}

U_CAPI ৱর্ণ U_EXPORT2
uprv_ebcdictolower(ৱর্ণ c) {
    যদি( (0xc1<=(uint8_t)c && (uint8_t)c<=0xc9) ||
        (0xd1<=(uint8_t)c && (uint8_t)c<=0xd9) ||
        (0xe2<=(uint8_t)c && (uint8_t)c<=0xe9)
    ) {
        c=(ৱর্ণ)(c-0x40);
    }
    ঘূৰাই c;
}


U_CAPI ৱর্ণ* U_EXPORT2
T_CString_toLowerCase(ৱর্ণ* str)
{
    ৱর্ণ* origPtr = str;

    যদি (str) {
        কৰা
            *str = uprv_tolower(*str);
        যেতিয়ালৈ (*(str++));
    }

    ঘূৰাই origPtr;
}

U_CAPI ৱর্ণ* U_EXPORT2
T_CString_toUpperCase(ৱর্ণ* str)
{
    ৱর্ণ* origPtr = str;

    যদি (str) {
        কৰা
            *str = uprv_toupper(*str);
        যেতিয়ালৈ (*(str++));
    }

    ঘূৰাই origPtr;
}

/*
 * Takes a int32_t and fills in  a char* string with that number "radix"-based.
 * Does not handle negative values (makes an empty string for them).
 * Writes at most 12 chars ("-2147483647" plus NUL).
 * Returns the length of the string (not including the NUL).
 */
U_CAPI int32_t U_EXPORT2
T_CString_integerToString(ৱর্ণ* buffer, int32_t v, int32_t radix)
{
    ৱর্ণ      tbuf[30];
    int32_t   tbx    = পরিমাণ(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint32_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint32_t) v;
    যদি(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint32_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = পরিমাণ(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    কৰা {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (ৱর্ণ)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } যেতিয়ালৈ (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += পরিমাণ(tbuf) - tbx -1;
    ঘূৰাই length;
}



/*
 * Takes a int64_t and fills in  a char* string with that number "radix"-based.
 * Writes at most 21: chars ("-9223372036854775807" plus NUL).
 * Returns the length of the string, not including the terminating NUL.
 */
U_CAPI int32_t U_EXPORT2
T_CString_int64ToString(ৱর্ণ* buffer, int64_t v, uint32_t radix)
{
    ৱর্ণ      tbuf[30];
    int32_t   tbx    = পরিমাণ(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint64_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint64_t) v;
    যদি(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint64_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = পরিমাণ(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    কৰা {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (ৱর্ণ)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } যেতিয়ালৈ (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += পরিমাণ(tbuf) - tbx -1;
    ঘূৰাই length;
}


U_CAPI int32_t U_EXPORT2
T_CString_stringToInteger(ধ্রুৱ ৱর্ণ *integerString, int32_t radix)
{
    ৱর্ণ *end;
    ঘূৰাই uprv_strtoul(integerString, &end, radix);

}

U_CAPI পূর্ণাঙ্ক U_EXPORT2
uprv_stricmp(ধ্রুৱ ৱর্ণ *str1, ধ্রুৱ ৱর্ণ *str2) {
    যদি(str1==nullptr) {
        যদি(str2==nullptr) {
            ঘূৰাই 0;
        } অন্যথা {
            ঘূৰাই -1;
        }
    } অন্যথা যদি(str2==nullptr) {
        ঘূৰাই 1;
    } অন্যথা {
        /* compare non-nullptr strings lexically with lowercase */
        পূর্ণাঙ্ক rc;
        unsigned ৱর্ণ c1, c2;

        বাবে(;;) {
            c1=(unsigned ৱর্ণ)*str1;
            c2=(unsigned ৱর্ণ)*str2;
            যদি(c1==0) {
                যদি(c2==0) {
                    ঘূৰাই 0;
                } অন্যথা {
                    ঘূৰাই -1;
                }
            } অন্যথা যদি(c2==0) {
                ঘূৰাই 1;
            } অন্যথা {
                /* compare non-zero characters with lowercase */
                rc=(পূর্ণাঙ্ক)(unsigned ৱর্ণ)uprv_tolower(c1)-(পূর্ণাঙ্ক)(unsigned ৱর্ণ)uprv_tolower(c2);
                যদি(rc!=0) {
                    ঘূৰাই rc;
                }
            }
            ++str1;
            ++str2;
        }
    }
}

U_CAPI পূর্ণাঙ্ক U_EXPORT2
uprv_strnicmp(ধ্রুৱ ৱর্ণ *str1, ধ্রুৱ ৱর্ণ *str2, uint32_t n) {
    যদি(str1==nullptr) {
        যদি(str2==nullptr) {
            ঘূৰাই 0;
        } অন্যথা {
            ঘূৰাই -1;
        }
    } অন্যথা যদি(str2==nullptr) {
        ঘূৰাই 1;
    } অন্যথা {
        /* compare non-nullptr strings lexically with lowercase */
        পূর্ণাঙ্ক rc;
        unsigned ৱর্ণ c1, c2;

        বাবে(; n--;) {
            c1=(unsigned ৱর্ণ)*str1;
            c2=(unsigned ৱর্ণ)*str2;
            যদি(c1==0) {
                যদি(c2==0) {
                    ঘূৰাই 0;
                } অন্যথা {
                    ঘূৰাই -1;
                }
            } অন্যথা যদি(c2==0) {
                ঘূৰাই 1;
            } অন্যথা {
                /* compare non-zero characters with lowercase */
                rc=(পূর্ণাঙ্ক)(unsigned ৱর্ণ)uprv_tolower(c1)-(পূর্ণাঙ্ক)(unsigned ৱর্ণ)uprv_tolower(c2);
                যদি(rc!=0) {
                    ঘূৰাই rc;
                }
            }
            ++str1;
            ++str2;
        }
    }

    ঘূৰাই 0;
}

U_CAPI ৱর্ণ* U_EXPORT2
uprv_strdup(ধ্রুৱ ৱর্ণ *src) {
    size_t len = uprv_strlen(src) + 1;
    ৱর্ণ *dup = (ৱর্ণ *) uprv_malloc(len);

    যদি (dup) {
        uprv_memcpy(dup, src, len);
    }

    ঘূৰাই dup;
}

U_CAPI ৱর্ণ* U_EXPORT2
uprv_strndup(ধ্রুৱ ৱর্ণ *src, int32_t n) {
    ৱর্ণ *dup;

    যদি(n < 0) {
        dup = uprv_strdup(src);
    } অন্যথা {
        dup = (ৱর্ণ*)uprv_malloc(n+1);
        যদি (dup) { 
            uprv_memcpy(dup, src, n);
            dup[n] = 0;
        }
    }

    ঘূৰাই dup;
}
