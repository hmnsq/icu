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
uprv_isASCIILetter(var_na c) {
#ຖ້າ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    ສົ່ງຄືນ
        ('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z') ||
        ('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z');
#ບໍ່ດັ່ງນັ້ນ
    ສົ່ງຄືນ ('a'<=c && c<='z') || ('A'<=c && c<='Z');
#endif
}

U_CAPI var_na U_EXPORT2
uprv_toupper(var_na c) {
#ຖ້າ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    ຖ້າ(('a'<=c && c<='i') || ('j'<=c && c<='r') || ('s'<=c && c<='z')) {
        c=(var_na)(c+('A'-'a'));
    }
#ບໍ່ດັ່ງນັ້ນ
    ຖ້າ('a'<=c && c<='z') {
        c=(var_na)(c+('A'-'a'));
    }
#endif
    ສົ່ງຄືນ c;
}


#ຖ້າ 0
/*
 * Commented out because cstring.h defines uprv_tolower() to be
 * the same as either uprv_asciitolower() or uprv_ebcdictolower()
 * to reduce the amount of code to cover with tests.
 *
 * Note that this uprv_tolower() definition is likely to work for most
 * charset families, not just ASCII and EBCDIC, because its #else branch
 * is written generically.
 */
U_CAPI var_na U_EXPORT2
uprv_tolower(var_na c) {
#ຖ້າ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    ຖ້າ(('A'<=c && c<='I') || ('J'<=c && c<='R') || ('S'<=c && c<='Z')) {
        c=(var_na)(c+('a'-'A'));
    }
#ບໍ່ດັ່ງນັ້ນ
    ຖ້າ('A'<=c && c<='Z') {
        c=(var_na)(c+('a'-'A'));
    }
#endif
    ສົ່ງຄືນ c;
}
#endif

U_CAPI var_na U_EXPORT2
uprv_asciitolower(var_na c) {
    ຖ້າ(0x41<=c && c<=0x5a) {
        c=(var_na)(c+0x20);
    }
    ສົ່ງຄືນ c;
}

U_CAPI var_na U_EXPORT2
uprv_ebcdictolower(var_na c) {
    ຖ້າ( (0xc1<=(uint8_t)c && (uint8_t)c<=0xc9) ||
        (0xd1<=(uint8_t)c && (uint8_t)c<=0xd9) ||
        (0xe2<=(uint8_t)c && (uint8_t)c<=0xe9)
    ) {
        c=(var_na)(c-0x40);
    }
    ສົ່ງຄືນ c;
}


U_CAPI var_na* U_EXPORT2
T_CString_toLowerCase(var_na* str)
{
    var_na* origPtr = str;

    ຖ້າ (str) {
        do
            *str = uprv_tolower(*str);
        ໃນຂະນະທີ່ (*(str++));
    }

    ສົ່ງຄືນ origPtr;
}

U_CAPI var_na* U_EXPORT2
T_CString_toUpperCase(var_na* str)
{
    var_na* origPtr = str;

    ຖ້າ (str) {
        do
            *str = uprv_toupper(*str);
        ໃນຂະນະທີ່ (*(str++));
    }

    ສົ່ງຄືນ origPtr;
}

/*
 * Takes a int32_t and fills in  a char* string with that number "radix"-based.
 * Does not handle negative values (makes an empty string for them).
 * Writes at most 12 chars ("-2147483647" plus NUL).
 * Returns the length of the string (not including the NUL).
 */
U_CAPI int32_t U_EXPORT2
T_CString_integerToString(var_na* buffer, int32_t v, int32_t radix)
{
    var_na      tbuf[30];
    int32_t   tbx    = parimaa_na(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint32_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint32_t) v;
    ຖ້າ(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint32_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = parimaa_na(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    do {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (var_na)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } ໃນຂະນະທີ່ (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += parimaa_na(tbuf) - tbx -1;
    ສົ່ງຄືນ length;
}



/*
 * Takes a int64_t and fills in  a char* string with that number "radix"-based.
 * Writes at most 21: chars ("-9223372036854775807" plus NUL).
 * Returns the length of the string, not including the terminating NUL.
 */
U_CAPI int32_t U_EXPORT2
T_CString_int64ToString(var_na* buffer, int64_t v, uint32_t radix)
{
    var_na      tbuf[30];
    int32_t   tbx    = parimaa_na(tbuf);
    uint8_t   digit;
    int32_t   length = 0;
    uint64_t  uval;
    
    U_ASSERT(radix>=2 && radix<=16);
    uval = (uint64_t) v;
    ຖ້າ(v<0 && radix == 10) {
        /* Only in base 10 do we conside numbers to be signed. */
        uval = (uint64_t)(-v); 
        buffer[length++] = '-';
    }
    
    tbx = parimaa_na(tbuf)-1;
    tbuf[tbx] = 0;   /* We are generating the digits backwards.  Null term the end. */
    do {
        digit = (uint8_t)(uval % radix);
        tbuf[--tbx] = (var_na)(T_CString_itosOffset(digit));
        uval  = uval / radix;
    } ໃນຂະນະທີ່ (uval != 0);
    
    /* copy converted number into user buffer  */
    uprv_strcpy(buffer+length, tbuf+tbx);
    length += parimaa_na(tbuf) - tbx -1;
    ສົ່ງຄືນ length;
}


U_CAPI int32_t U_EXPORT2
T_CString_stringToInteger(const var_na *integerString, int32_t radix)
{
    var_na *end;
    ສົ່ງຄືນ uprv_strtoul(integerString, &end, radix);

}

U_CAPI ຈຳນວນເຕັມ U_EXPORT2
uprv_stricmp(const var_na *str1, const var_na *str2) {
    ຖ້າ(str1==nullptr) {
        ຖ້າ(str2==nullptr) {
            ສົ່ງຄືນ 0;
        } ບໍ່ດັ່ງນັ້ນ {
            ສົ່ງຄືນ -1;
        }
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ(str2==nullptr) {
        ສົ່ງຄືນ 1;
    } ບໍ່ດັ່ງນັ້ນ {
        /* compare non-nullptr strings lexically with lowercase */
        ຈຳນວນເຕັມ rc;
        unsigned var_na c1, c2;

        ສຳລັບ(;;) {
            c1=(unsigned var_na)*str1;
            c2=(unsigned var_na)*str2;
            ຖ້າ(c1==0) {
                ຖ້າ(c2==0) {
                    ສົ່ງຄືນ 0;
                } ບໍ່ດັ່ງນັ້ນ {
                    ສົ່ງຄືນ -1;
                }
            } ບໍ່ດັ່ງນັ້ນ ຖ້າ(c2==0) {
                ສົ່ງຄືນ 1;
            } ບໍ່ດັ່ງນັ້ນ {
                /* compare non-zero characters with lowercase */
                rc=(ຈຳນວນເຕັມ)(unsigned var_na)uprv_tolower(c1)-(ຈຳນວນເຕັມ)(unsigned var_na)uprv_tolower(c2);
                ຖ້າ(rc!=0) {
                    ສົ່ງຄືນ rc;
                }
            }
            ++str1;
            ++str2;
        }
    }
}

U_CAPI ຈຳນວນເຕັມ U_EXPORT2
uprv_strnicmp(const var_na *str1, const var_na *str2, uint32_t n) {
    ຖ້າ(str1==nullptr) {
        ຖ້າ(str2==nullptr) {
            ສົ່ງຄືນ 0;
        } ບໍ່ດັ່ງນັ້ນ {
            ສົ່ງຄືນ -1;
        }
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ(str2==nullptr) {
        ສົ່ງຄືນ 1;
    } ບໍ່ດັ່ງນັ້ນ {
        /* compare non-nullptr strings lexically with lowercase */
        ຈຳນວນເຕັມ rc;
        unsigned var_na c1, c2;

        ສຳລັບ(; n--;) {
            c1=(unsigned var_na)*str1;
            c2=(unsigned var_na)*str2;
            ຖ້າ(c1==0) {
                ຖ້າ(c2==0) {
                    ສົ່ງຄືນ 0;
                } ບໍ່ດັ່ງນັ້ນ {
                    ສົ່ງຄືນ -1;
                }
            } ບໍ່ດັ່ງນັ້ນ ຖ້າ(c2==0) {
                ສົ່ງຄືນ 1;
            } ບໍ່ດັ່ງນັ້ນ {
                /* compare non-zero characters with lowercase */
                rc=(ຈຳນວນເຕັມ)(unsigned var_na)uprv_tolower(c1)-(ຈຳນວນເຕັມ)(unsigned var_na)uprv_tolower(c2);
                ຖ້າ(rc!=0) {
                    ສົ່ງຄືນ rc;
                }
            }
            ++str1;
            ++str2;
        }
    }

    ສົ່ງຄືນ 0;
}

U_CAPI var_na* U_EXPORT2
uprv_strdup(const var_na *src) {
    size_t len = uprv_strlen(src) + 1;
    var_na *dup = (var_na *) uprv_malloc(len);

    ຖ້າ (dup) {
        uprv_memcpy(dup, src, len);
    }

    ສົ່ງຄືນ dup;
}

U_CAPI var_na* U_EXPORT2
uprv_strndup(const var_na *src, int32_t n) {
    var_na *dup;

    ຖ້າ(n < 0) {
        dup = uprv_strdup(src);
    } ບໍ່ດັ່ງນັ້ນ {
        dup = (var_na*)uprv_malloc(n+1);
        ຖ້າ (dup) { 
            uprv_memcpy(dup, src, n);
            dup[n] = 0;
        }
    }

    ສົ່ງຄືນ dup;
}
