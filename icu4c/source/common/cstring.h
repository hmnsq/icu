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

#अगर_अघोषित CSTRING_H
#घोषणा CSTRING_H 1

#समावेश "unicode/utypes.h"
#समावेश "cmemory.h"
#समावेश <string.h>
#समावेश <stdlib.h>
#समावेश <ctype.h>

#घोषणा uprv_strcpy(dst, src) U_STANDARD_CPP_NAMESPACE  म_नकल(dst, src)
#घोषणा uprv_strlen(str) U_STANDARD_CPP_NAMESPACE म_माप(str)
#घोषणा uprv_strcmp(s1, s2) U_STANDARD_CPP_NAMESPACE म_भेद(s1, s2)
#घोषणा uprv_strcat(dst, src) U_STANDARD_CPP_NAMESPACE म_जोड़ो(dst, src)
#घोषणा uprv_strchr(s, c) U_STANDARD_CPP_NAMESPACE म_अक्षर(s, c)
#घोषणा uprv_strstr(s, c) U_STANDARD_CPP_NAMESPACE म_माला(s, c)
#घोषणा uprv_strrchr(s, c) U_STANDARD_CPP_NAMESPACE म_खोजप(s, c)
#घोषणा uprv_strncpy(dst, src, आकार_2) U_STANDARD_CPP_NAMESPACE म_नकलन(dst, src, आकार_2)
#घोषणा uprv_strncmp(s1, s2, n) U_STANDARD_CPP_NAMESPACE म_भेदन(s1, s2, n)
#घोषणा uprv_strncat(dst, src, n) U_STANDARD_CPP_NAMESPACE म_जोड़न(dst, src, n)

/**
 * Is c an ASCII-repertoire letter a-z or A-Z?
 * Note: The implementation is specific to whether ICU is compiled for
 * an ASCII-based or EBCDIC-based machine. There just does not seem to be a better name for this.
 */
U_CAPI UBool U_EXPORT2
uprv_isASCIILetter(अक्षर c);

// NOTE: For u_asciiToUpper that takes a UChar, see ustr_imp.h

U_CAPI अक्षर U_EXPORT2
uprv_toupper(अक्षर c);


U_CAPI अक्षर U_EXPORT2
uprv_asciitolower(अक्षर c);

U_CAPI अक्षर U_EXPORT2
uprv_ebcdictolower(अक्षर c);

#अगर U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_tolower uprv_asciitolower
#या_अगर U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_tolower uprv_ebcdictolower
#अन्यथा
#   error U_CHARSET_FAMILY is not valid
#पूर्ण_अगर

#घोषणा uprv_strtod(source, end) U_STANDARD_CPP_NAMESPACE म_से_भग्न(source, end)
#घोषणा uprv_strtoul(str, end, base) U_STANDARD_CPP_NAMESPACE म_से_अदीर्घ(str, end, base)
#घोषणा uprv_strtol(str, end, base) U_STANDARD_CPP_NAMESPACE म_से_दीर्घ(str, end, base)

/* Conversion from a digit to the character with radix base from 2-19 */
/* May need to use U_UPPER_ORDINAL*/
#घोषणा T_CString_itosOffset(a) ((a)<=9?('0'+(a)):('A'+(a)-10))

U_CAPI अक्षर* U_EXPORT2
uprv_strdup(स्थिर अक्षर *src);

/**
 * uprv_malloc n+1 bytes, and copy n bytes from src into the new string.
 * Terminate with a null at offset n.   If n is -1, works like uprv_strdup
 * @param src
 * @param n length of the input string, not including null.
 * @return new string (owned by caller, use uprv_free to free).
 * @internal
 */
U_CAPI अक्षर* U_EXPORT2
uprv_strndup(स्थिर अक्षर *src, int32_t n);

U_CAPI अक्षर* U_EXPORT2
T_CString_toLowerCase(अक्षर* str);

U_CAPI अक्षर* U_EXPORT2
T_CString_toUpperCase(अक्षर* str);

U_CAPI int32_t U_EXPORT2
T_CString_integerToString(अक्षर *बफर, int32_t n, int32_t radix);

U_CAPI int32_t U_EXPORT2
T_CString_int64ToString(अक्षर *बफर, int64_t n, uint32_t radix);

U_CAPI int32_t U_EXPORT2
T_CString_stringToInteger(स्थिर अक्षर *integerString, int32_t radix);

/**
 * Case-insensitive, language-independent string comparison
 * limited to the ASCII character repertoire.
 */
U_CAPI पूर्णांक U_EXPORT2
uprv_stricmp(स्थिर अक्षर *str1, स्थिर अक्षर *str2);

/**
 * Case-insensitive, language-independent string comparison
 * limited to the ASCII character repertoire.
 */
U_CAPI पूर्णांक U_EXPORT2
uprv_strnicmp(स्थिर अक्षर *str1, स्थिर अक्षर *str2, uint32_t n);

#पूर्ण_अगर /* ! CSTRING_H */
