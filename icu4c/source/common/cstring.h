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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CSTRING_H
#ವ್ಯಾಖ್ಯಾನಿಸು CSTRING_H 1

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <ctype.h>

#ವ್ಯಾಖ್ಯಾನಿಸು uprv_strcpy(dst, src) U_STANDARD_CPP_NAMESPACE  strcpy(dst, src)
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_strlen(str) U_STANDARD_CPP_NAMESPACE ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str)
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_strcmp(s1, s2) U_STANDARD_CPP_NAMESPACE strcmp(s1, s2)
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_strcat(dst, src) U_STANDARD_CPP_NAMESPACE strcat(dst, src)
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_strchr(s, c) U_STANDARD_CPP_NAMESPACE strchr(s, c)
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_strstr(s, c) U_STANDARD_CPP_NAMESPACE ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(s, c)
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_strrchr(s, c) U_STANDARD_CPP_NAMESPACE strrchr(s, c)
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_strncpy(dst, src, ಗಾತ್ರ_3) U_STANDARD_CPP_NAMESPACE strncpy(dst, src, ಗಾತ್ರ_3)
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_strncmp(s1, s2, n) U_STANDARD_CPP_NAMESPACE ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s1, s2, n)
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_strncat(dst, src, n) U_STANDARD_CPP_NAMESPACE strncat(dst, src, n)

/**
 * Is c an ASCII-repertoire letter a-z or A-Z?
 * Note: The implementation is specific to whether ICU is compiled for
 * an ASCII-based or EBCDIC-based machine. There just does not seem to be a better name for this.
 */
U_CAPI UBool U_EXPORT2
uprv_isASCIILetter(ಅಕ್ಷರ c);

// NOTE: For u_asciiToUpper that takes a UChar, see ustr_imp.h

U_CAPI ಅಕ್ಷರ U_EXPORT2
uprv_toupper(ಅಕ್ಷರ c);


U_CAPI ಅಕ್ಷರ U_EXPORT2
uprv_asciitolower(ಅಕ್ಷರ c);

U_CAPI ಅಕ್ಷರ U_EXPORT2
uprv_ebcdictolower(ಅಕ್ಷರ c);

#ಆದರೆ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_tolower uprv_asciitolower
#ಅಥವಾಆದರೆ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_tolower uprv_ebcdictolower
#ಇಲ್ಲದಿದ್ದರೆ
#   error U_CHARSET_FAMILY is not valid
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು uprv_strtod(ಮೂಲ, ಕೊನೆ) U_STANDARD_CPP_NAMESPACE strtod(ಮೂಲ, ಕೊನೆ)
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_strtoul(str, ಕೊನೆ, ತಳ) U_STANDARD_CPP_NAMESPACE strtoul(str, ಕೊನೆ, ತಳ)
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_strtol(str, ಕೊನೆ, ತಳ) U_STANDARD_CPP_NAMESPACE strtol(str, ಕೊನೆ, ತಳ)

/* Conversion from a digit to the character with radix base from 2-19 */
/* May need to use U_UPPER_ORDINAL*/
#ವ್ಯಾಖ್ಯಾನಿಸು T_CString_itos_ಸ್ಥಾನಾಂತರ(a) ((a)<=9?('0'+(a)):('A'+(a)-10))

U_CAPI ಅಕ್ಷರ* U_EXPORT2
uprv_strdup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src);

/**
 * uprv_malloc n+1 bytes, and copy n bytes from src into the new string.
 * Terminate with a null at offset n.   If n is -1, works like uprv_strdup
 * @param src
 * @param n length of the input string, not including null.
 * @return new string (owned by caller, use uprv_free to free).
 * @internal
 */
U_CAPI ಅಕ್ಷರ* U_EXPORT2
uprv_strndup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t n);

U_CAPI ಅಕ್ಷರ* U_EXPORT2
T_CString_ಗಮ್ಯ_ಅಧೋ_Case(ಅಕ್ಷರ* str);

U_CAPI ಅಕ್ಷರ* U_EXPORT2
T_CString_ಗಮ್ಯ_ಊರ್ಧ್ವ_Case(ಅಕ್ಷರ* str);

U_CAPI int32_t U_EXPORT2
T_CString_integer_ಗಮ್ಯ_String(ಅಕ್ಷರ *ಬಫರ್, int32_t n, int32_t radix);

U_CAPI int32_t U_EXPORT2
T_CString_int64_ಗಮ್ಯ_String(ಅಕ್ಷರ *ಬಫರ್, int64_t n, uint32_t radix);

U_CAPI int32_t U_EXPORT2
T_CString_string_ಗಮ್ಯ_Integer(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *integerString, int32_t radix);

/**
 * Case-insensitive, language-independent string comparison
 * limited to the ASCII character repertoire.
 */
U_CAPI ಪೂರ್ಣಾಂಕ U_EXPORT2
uprv_stricmp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str2);

/**
 * Case-insensitive, language-independent string comparison
 * limited to the ASCII character repertoire.
 */
U_CAPI ಪೂರ್ಣಾಂಕ U_EXPORT2
uprv_strnicmp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str2, uint32_t n);

#ಆದರೆ_ಅಂತ್ಯ /* ! CSTRING_H */
