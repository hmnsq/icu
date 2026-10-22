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



#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "uassert.h"

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
uprv_isASCIILetter(எழுத்து c) {
#எனில் U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    திரும்பு
        ('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z') ||
        ('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z');
#இல்லையெனில்
    திரும்பு ('a'<=c && c<='z') || ('A'<=c && c<='Z');
#endif
}

U_CAPI எழுத்து U_EXPORT2
uprv_toupper(எழுத்து c) {
#எனில் U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    எனில்(('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z')) {
        c=(எழுத்து)(c+('A'-'a'));
    }
#இல்லையெனில்
    எனில்('a'<=c && c<='z') {
        c=(எழுத்து)(c+('A'-'a'));
    }
#endif
    திரும்பு c;
}


#எனில் 0
/*
 * Commented out because cstring.h defines uprv_tolower() to be
 * the same as either uprv_asciitolower() or uprv_ebcdictolower()
 * to reduce the amount of code to cover with tests.
 *
 * Note that this uprv_tolower() definition is likely to work for most
 * charset families, not just ASCII and EBCDIC, because its #else branch
 * is written generically.
 */
U_CAPI எழுத்து U_EXPORT2
uprv_tolower(எழுத்து c) {
#எனில் U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    எனில்(('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z')) {
        c=(எழுத்து)(c+('a'-'A'));
    }
#இல்லையெனில்
    எனில்('A'<=c && c<='Z') {
        c=(எழுத்து)(c+('a'-'A'));
    }
#endif
    திரும்பு c;
}
#endif

U_CAPI எழுத்து U_EXPORT2
uprv_asciitolower(எழுத்து c) {
    எனில்(0x41<=c && c<=0x5a) {
        c=(எழுத்து)(c+0x20);
    }
    திரும்பு c;
}

U_CAPI எழுத்து U_EXPORT2
uprv_ebcdictolower(எழுத்து c) {
    எனில்( (0xc1<=(uint8_t)c && (uint8_t)c<=0xc9) ||
        (0xd1<=(uint8_t)c && (uint8_t)c<=0xd9) ||
        (0xe2<=(uint8_t)c && (uint8_t)c<=0xe9)
    ) {
        c=(எழுத்து)(c-0x40);
    }
    திரும்பு c;
}


U_CAPI எழுத்து* U_EXPORT2
T_CString_toLowerCase(எழுத்து* str)
{
    எழுத்து* origPtr = str;

    எனில் (str) {
        செய்
            *str = uprv_tolower(*str);
        போது (*(str++));
    }

    திரும்பு origPtr;
}

U_CAPI எழுத்து* U_EXPORT2
T_CString_toUpperCase(எழுத்து* str)
{
    எழுத்து* origPtr = str;

    எனில் (str) {
        செய்
            *str = uprv_toupper(*str);
        போது (*(str++));
    }

    திரும்பு origPtr;
}

/*
 * Takes a int32_t and fills in  a char* string with that number "radix"-based.
 * Does not handle negative values (makes an empty string for them).
 * Writes at most 12 chars ("-2147483647" plus NUL).
 * Returns the length of the string (not including the NUL).
 */
U_CAPI int32_t U_EXPORT2
T_CString_integerToString(எழுத்து* buffer, int32_t v, int32_t radix)
{
    எழுத்து      tbuf[30];
    int32_t   tbx    = அளவு(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint32_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint32_t) v;
    எனில்(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint32_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = அளவு(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    செய் {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (எழுத்து)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } போது (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += அளவு(tbuf) - tbx -1;
    திரும்பு length;
}



/*
 * Takes a int64_t and fills in  a char* string with that number "radix"-based.
 * Writes at most 21: chars ("-9223372036854775807" plus NUL).
 * Returns the length of the string, not including the terminating NUL.
 */
U_CAPI int32_t U_EXPORT2
T_CString_int64ToString(எழுத்து* buffer, int64_t v, uint32_t radix)
{
    எழுத்து      tbuf[30];
    int32_t   tbx    = அளவு(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint64_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint64_t) v;
    எனில்(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint64_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = அளவு(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    செய் {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (எழுத்து)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } போது (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += அளவு(tbuf) - tbx -1;
    திரும்பு length;
}


U_CAPI int32_t U_EXPORT2
T_CString_stringToInteger(மாறிலி எழுத்து *integerString, int32_t radix)
{
    எழுத்து *end;
    திரும்பு uprv_strtoul(integerString, &end, radix);

}

U_CAPI முழு U_EXPORT2
uprv_stricmp(மாறிலி எழுத்து *str1, மாறிலி எழுத்து *str2) {
    எனில்(str1==nullptr) {
        எனில்(str2==nullptr) {
            திரும்பு 0;
        } இல்லையெனில் {
            திரும்பு -1;
        }
    } இல்லையெனில் எனில்(str2==nullptr) {
        திரும்பு 1;
    } இல்லையெனில் {
        /* compare non-nullptr strings lexically with lowercase */
        முழு rc;
        unsigned எழுத்து c1, c2;

        க்கு(;;) {
            c1=(unsigned எழுத்து)*str1;
            c2=(unsigned எழுத்து)*str2;
            எனில்(c1==0) {
                எனில்(c2==0) {
                    திரும்பு 0;
                } இல்லையெனில் {
                    திரும்பு -1;
                }
            } இல்லையெனில் எனில்(c2==0) {
                திரும்பு 1;
            } இல்லையெனில் {
                /* compare non-zero characters with lowercase */
                rc=(முழு)(unsigned எழுத்து)uprv_tolower(c1)-(முழு)(unsigned எழுத்து)uprv_tolower(c2);
                எனில்(rc!=0) {
                    திரும்பு rc;
                }
            }
            ++str1;
            ++str2;
        }
    }
}

U_CAPI முழு U_EXPORT2
uprv_strnicmp(மாறிலி எழுத்து *str1, மாறிலி எழுத்து *str2, uint32_t n) {
    எனில்(str1==nullptr) {
        எனில்(str2==nullptr) {
            திரும்பு 0;
        } இல்லையெனில் {
            திரும்பு -1;
        }
    } இல்லையெனில் எனில்(str2==nullptr) {
        திரும்பு 1;
    } இல்லையெனில் {
        /* compare non-nullptr strings lexically with lowercase */
        முழு rc;
        unsigned எழுத்து c1, c2;

        க்கு(; n--;) {
            c1=(unsigned எழுத்து)*str1;
            c2=(unsigned எழுத்து)*str2;
            எனில்(c1==0) {
                எனில்(c2==0) {
                    திரும்பு 0;
                } இல்லையெனில் {
                    திரும்பு -1;
                }
            } இல்லையெனில் எனில்(c2==0) {
                திரும்பு 1;
            } இல்லையெனில் {
                /* compare non-zero characters with lowercase */
                rc=(முழு)(unsigned எழுத்து)uprv_tolower(c1)-(முழு)(unsigned எழுத்து)uprv_tolower(c2);
                எனில்(rc!=0) {
                    திரும்பு rc;
                }
            }
            ++str1;
            ++str2;
        }
    }

    திரும்பு 0;
}

U_CAPI எழுத்து* U_EXPORT2
uprv_strdup(மாறிலி எழுத்து *src) {
    size_t len = uprv_strlen(src) + 1;
    எழுத்து *dup = (எழுத்து *) uprv_malloc(len);

    எனில் (dup) {
        uprv_memcpy(dup, src, len);
    }

    திரும்பு dup;
}

U_CAPI எழுத்து* U_EXPORT2
uprv_strndup(மாறிலி எழுத்து *src, int32_t n) {
    எழுத்து *dup;

    எனில்(n < 0) {
        dup = uprv_strdup(src);
    } இல்லையெனில் {
        dup = (எழுத்து*)uprv_malloc(n+1);
        எனில் (dup) { 
            uprv_memcpy(dup, src, n);
            dup[n] = 0;
        }
    }

    திரும்பு dup;
}
