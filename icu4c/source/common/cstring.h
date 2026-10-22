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

#यदि_अपरिभाषितम् CSTRING_H
#परिभाषय CSTRING_H 1

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव <string.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <ctype.h>

#परिभाषय uprv_strcpy(dst, src) U_STANDARD_CPP_NAMESPACE  वाक्यं_प्रतिलिख(dst, src)
#परिभाषय uprv_strlen(str) U_STANDARD_CPP_NAMESPACE वाक्यदैर्घ्यम्(str)
#परिभाषय uprv_strcmp(s1, s2) U_STANDARD_CPP_NAMESPACE वाक्यं_तुलय(s1, s2)
#परिभाषय uprv_strcat(dst, src) U_STANDARD_CPP_NAMESPACE वाक्यं_योजय(dst, src)
#परिभाषय uprv_strchr(s, c) U_STANDARD_CPP_NAMESPACE strchr(s, c)
#परिभाषय uprv_strstr(s, c) U_STANDARD_CPP_NAMESPACE वाक्ये_अन्विष(s, c)
#परिभाषय uprv_strrchr(s, c) U_STANDARD_CPP_NAMESPACE strrchr(s, c)
#परिभाषय uprv_strncpy(dst, src, आकार_2) U_STANDARD_CPP_NAMESPACE strncpy(dst, src, आकार_2)
#परिभाषय uprv_strncmp(s1, s2, n) U_STANDARD_CPP_NAMESPACE वाक्ये_तुलय(s1, s2, n)
#परिभाषय uprv_strncat(dst, src, n) U_STANDARD_CPP_NAMESPACE strncat(dst, src, n)

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
#नोचेद्यदि U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_tolower uprv_ebcdictolower
#अन्यथा
#   error U_CHARSET_FAMILY is not valid
#यदि_अन्तः

#परिभाषय uprv_strtod(source, अन्तः) U_STANDARD_CPP_NAMESPACE strtod(source, अन्तः)
#परिभाषय uprv_strtoul(str, अन्तः, आधारः) U_STANDARD_CPP_NAMESPACE strtoul(str, अन्तः, आधारः)
#परिभाषय uprv_strtol(str, अन्तः, आधारः) U_STANDARD_CPP_NAMESPACE strtol(str, अन्तः, आधारः)

/* Conversion from a digit to the character with radix base from 2-19 */
/* May need to use U_UPPER_ORDINAL*/
#परिभाषय T_CString_itos_विस्थापनम्(a) ((a)<=9?('0'+(a)):('A'+(a)-10))

U_CAPI वर्ण* U_EXPORT2
uprv_strdup(ध्रुव वर्ण *src);

/**
 * uprv_malloc n+1 bytes, and copy n bytes from src into the new string.
 * Terminate with a null at offset n.   If n is -1, works like uprv_strdup
 * @param src
 * @param n length of the input string, not including null.
 * @return new string (owned by caller, use uprv_free to free).
 * @internal
 */
U_CAPI वर्ण* U_EXPORT2
uprv_strndup(ध्रुव वर्ण *src, int32_t n);

U_CAPI वर्ण* U_EXPORT2
T_CString_गन्तव्यम्_अधस्तनम्_Case(वर्ण* str);

U_CAPI वर्ण* U_EXPORT2
T_CString_गन्तव्यम्_ऊर्ध्वस्थम्_Case(वर्ण* str);

U_CAPI int32_t U_EXPORT2
T_CString_integer_गन्तव्यम्_String(वर्ण *बफर, int32_t n, int32_t radix);

U_CAPI int32_t U_EXPORT2
T_CString_int64_गन्तव्यम्_String(वर्ण *बफर, int64_t n, uint32_t radix);

U_CAPI int32_t U_EXPORT2
T_CString_string_गन्तव्यम्_Integer(ध्रुव वर्ण *integerString, int32_t radix);

/**
 * Case-insensitive, language-independent string comparison
 * limited to the ASCII character repertoire.
 */
U_CAPI पूर्णाङ्क U_EXPORT2
uprv_stricmp(ध्रुव वर्ण *str1, ध्रुव वर्ण *str2);

/**
 * Case-insensitive, language-independent string comparison
 * limited to the ASCII character repertoire.
 */
U_CAPI पूर्णाङ्क U_EXPORT2
uprv_strnicmp(ध्रुव वर्ण *str1, ध्रुव वर्ण *str2, uint32_t n);

#यदि_अन्तः /* ! CSTRING_H */
