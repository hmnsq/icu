// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1997-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
* File CSTRING.H
*
* Contains CString interface
*
* @author       Helena Shih
*
* Modification History:
*
*   Date        Name        Description
*   6/17/98     hshih       Created.
*  05/03/99     stephen     Changed from functions to macros.
*  06/14/99     stephen     Added icu_strncat, icu_strncmp, icu_tolower
*
******************************************************************************
*/

#ifndef CSTRING_H
#define CSTRING_H 1

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव <string.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <ctype.h>

#define uprv_strcpy(dst, src) U_STANDARD_CPP_NAMESPACE  म_नकल(dst, src)
#define uprv_strlen(str) U_STANDARD_CPP_NAMESPACE म_माप(str)
#define uprv_strcmp(s1, s2) U_STANDARD_CPP_NAMESPACE म_भेद(s1, s2)
#define uprv_strcat(dst, src) U_STANDARD_CPP_NAMESPACE म_जोड़ो(dst, src)
#define uprv_strchr(s, c) U_STANDARD_CPP_NAMESPACE म_अक्षर(s, c)
#define uprv_strstr(s, c) U_STANDARD_CPP_NAMESPACE म_माला(s, c)
#define uprv_strrchr(s, c) U_STANDARD_CPP_NAMESPACE म_खोजप(s, c)
#define uprv_strncpy(dst, src, size) U_STANDARD_CPP_NAMESPACE म_नकलन(dst, src, size)
#define uprv_strncmp(s1, s2, n) U_STANDARD_CPP_NAMESPACE म_भेदन(s1, s2, n)
#define uprv_strncat(dst, src, n) U_STANDARD_CPP_NAMESPACE म_जोड़न(dst, src, n)

/**
 * Is c an ASCII-repertoire letter a-z or A-Z?
 * Note: The implementation is specific to whether ICU is compiled for
 * an ASCII-based or EBCDIC-based machine. There just does not seem to be a better name for this.
 */
U_CAPI UBool U_EXPORT2
uprv_isASCIILetter(वर्ण c);

// NOTE: For u_asciiToUpper that takes a UChar, see ustr_imp.h

U_CAPI वर्ण U_EXPORT2
uprv_toupper(वर्ण c);


U_CAPI वर्ण U_EXPORT2
uprv_asciitolower(वर्ण c);

U_CAPI वर्ण U_EXPORT2
uprv_ebcdictolower(वर्ण c);

#यदि U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_tolower uprv_asciitolower
#अथवायदि U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_tolower uprv_ebcdictolower
#अन्यथा
#   error U_CHARSET_FAMILY is not valid
#endif

#define uprv_strtod(source, end) U_STANDARD_CPP_NAMESPACE म_से_भग्न(source, end)
#define uprv_strtoul(str, end, base) U_STANDARD_CPP_NAMESPACE म_से_अदीर्घ(str, end, base)
#define uprv_strtol(str, end, base) U_STANDARD_CPP_NAMESPACE म_से_दीर्घ(str, end, base)

/* Conversion from a digit to the character with radix base from 2-19 */
/* May need to use U_UPPER_ORDINAL*/
#define T_CString_itosOffset(a) ((a)<=9?('0'+(a)):('A'+(a)-10))

U_CAPI वर्ण* U_EXPORT2
uprv_strdup(अचर वर्ण *src);

/**
 * uprv_malloc n+1 bytes, and copy n bytes from src into the new string.
 * Terminate with a null at offset n.   If n is -1, works like uprv_strdup
 * @param src
 * @param n length of the input string, not including null.
 * @return new string (owned by caller, use uprv_free to free).
 * @internal
 */
U_CAPI वर्ण* U_EXPORT2
uprv_strndup(अचर वर्ण *src, int32_t n);

U_CAPI वर्ण* U_EXPORT2
T_CString_toLowerCase(वर्ण* str);

U_CAPI वर्ण* U_EXPORT2
T_CString_toUpperCase(वर्ण* str);

U_CAPI int32_t U_EXPORT2
T_CString_integerToString(वर्ण *buffer, int32_t n, int32_t radix);

U_CAPI int32_t U_EXPORT2
T_CString_int64ToString(वर्ण *buffer, int64_t n, uint32_t radix);

U_CAPI int32_t U_EXPORT2
T_CString_stringToInteger(अचर वर्ण *integerString, int32_t radix);

/**
 * Case-insensitive, language-independent string comparison
 * limited to the ASCII character repertoire.
 */
U_CAPI पूर्णाङ्क U_EXPORT2
uprv_stricmp(अचर वर्ण *str1, अचर वर्ण *str2);

/**
 * Case-insensitive, language-independent string comparison
 * limited to the ASCII character repertoire.
 */
U_CAPI पूर्णाङ्क U_EXPORT2
uprv_strnicmp(अचर वर्ण *str1, अचर वर्ण *str2, uint32_t n);

#endif /* ! CSTRING_H */
