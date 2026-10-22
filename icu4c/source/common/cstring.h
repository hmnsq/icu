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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CSTRING_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSTRING_H 1

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ctype.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_strcpy(dst, src) U_STANDARD_CPP_NAMESPACE  strcpy(dst, src)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_strlen(str) U_STANDARD_CPP_NAMESPACE ਲੜੀ_ਲੰਬਾਈ(str)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_strcmp(s1, s2) U_STANDARD_CPP_NAMESPACE strcmp(s1, s2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_strcat(dst, src) U_STANDARD_CPP_NAMESPACE strcat(dst, src)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_strchr(s, c) U_STANDARD_CPP_NAMESPACE strchr(s, c)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_strstr(s, c) U_STANDARD_CPP_NAMESPACE ਲੜੀ_ਖੋਜ(s, c)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_strrchr(s, c) U_STANDARD_CPP_NAMESPACE strrchr(s, c)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_strncpy(dst, src, ਮਾਪ_2) U_STANDARD_CPP_NAMESPACE strncpy(dst, src, ਮਾਪ_2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_strncmp(s1, s2, n) U_STANDARD_CPP_NAMESPACE ਲੜੀ_ਤੁਲਨਾ(s1, s2, n)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_strncat(dst, src, n) U_STANDARD_CPP_NAMESPACE strncat(dst, src, n)

/**
 * Is c an ASCII-repertoire letter a-z or A-Z?
 * Note: The implementation is specific to whether ICU is compiled for
 * an ASCII-based or EBCDIC-based machine. There just does not seem to be a better name for this.
 */
U_CAPI UBool U_EXPORT2
uprv_isASCIILetter(ਅੱਖਰ c);

// NOTE: For u_asciiToUpper that takes a UChar, see ustr_imp.h

U_CAPI ਅੱਖਰ U_EXPORT2
uprv_toupper(ਅੱਖਰ c);


U_CAPI ਅੱਖਰ U_EXPORT2
uprv_asciitolower(ਅੱਖਰ c);

U_CAPI ਅੱਖਰ U_EXPORT2
uprv_ebcdictolower(ਅੱਖਰ c);

#ਜੇ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_tolower uprv_asciitolower
#ਜਾਂਜੇ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_tolower uprv_ebcdictolower
#ਨਹੀਂਤਾਂ
#   error U_CHARSET_FAMILY is not valid
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_strtod(ਸਰੋਤ, ਸਿਰਾ) U_STANDARD_CPP_NAMESPACE strtod(ਸਰੋਤ, ਸਿਰਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_strtoul(str, ਸਿਰਾ, ਅਧਾਰ) U_STANDARD_CPP_NAMESPACE strtoul(str, ਸਿਰਾ, ਅਧਾਰ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_strtol(str, ਸਿਰਾ, ਅਧਾਰ) U_STANDARD_CPP_NAMESPACE strtol(str, ਸਿਰਾ, ਅਧਾਰ)

/* Conversion from a digit to the character with radix base from 2-19 */
/* May need to use U_UPPER_ORDINAL*/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CString_itos_ਵਿੱਥ(a) ((a)<=9?('0'+(a)):('A'+(a)-10))

U_CAPI ਅੱਖਰ* U_EXPORT2
uprv_strdup(ਅਚਲ ਅੱਖਰ *src);

/**
 * uprv_malloc n+1 bytes, and copy n bytes from src into the new string.
 * Terminate with a null at offset n.   If n is -1, works like uprv_strdup
 * @param src
 * @param n length of the input string, not including null.
 * @return new string (owned by caller, use uprv_free to free).
 * @internal
 */
U_CAPI ਅੱਖਰ* U_EXPORT2
uprv_strndup(ਅਚਲ ਅੱਖਰ *src, int32_t n);

U_CAPI ਅੱਖਰ* U_EXPORT2
T_CString_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ_Case(ਅੱਖਰ* str);

U_CAPI ਅੱਖਰ* U_EXPORT2
T_CString_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_Case(ਅੱਖਰ* str);

U_CAPI int32_t U_EXPORT2
T_CString_integer_ਮੰਜ਼ਿਲ_String(ਅੱਖਰ *ਬਫਰ, int32_t n, int32_t radix);

U_CAPI int32_t U_EXPORT2
T_CString_int64_ਮੰਜ਼ਿਲ_String(ਅੱਖਰ *ਬਫਰ, int64_t n, uint32_t radix);

U_CAPI int32_t U_EXPORT2
T_CString_string_ਮੰਜ਼ਿਲ_Integer(ਅਚਲ ਅੱਖਰ *integerString, int32_t radix);

/**
 * Case-insensitive, language-independent string comparison
 * limited to the ASCII character repertoire.
 */
U_CAPI ਪੂਰਨ U_EXPORT2
uprv_stricmp(ਅਚਲ ਅੱਖਰ *str1, ਅਚਲ ਅੱਖਰ *str2);

/**
 * Case-insensitive, language-independent string comparison
 * limited to the ASCII character repertoire.
 */
U_CAPI ਪੂਰਨ U_EXPORT2
uprv_strnicmp(ਅਚਲ ਅੱਖਰ *str1, ਅਚਲ ਅੱਖਰ *str2, uint32_t n);

#ਜੇ_ਅੰਤ /* ! CSTRING_H */
