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

#که_نه_تعریف_شوی CSTRING_H
#تعریف_کړه CSTRING_H 1

#ضمن "unicode/utypes.h"
#ضمن "cmemory.h"
#ضمن <string.h>
#ضمن <stdlib.h>
#ضمن <ctype.h>

#تعریف_کړه uprv_strcpy(dst, src) U_STANDARD_CPP_NAMESPACE  strcpy(dst, src)
#تعریف_کړه uprv_strlen(str) U_STANDARD_CPP_NAMESPACE مزي_اوږدوالی(str)
#تعریف_کړه uprv_strcmp(s1, s2) U_STANDARD_CPP_NAMESPACE strcmp(s1, s2)
#تعریف_کړه uprv_strcat(dst, src) U_STANDARD_CPP_NAMESPACE strcat(dst, src)
#تعریف_کړه uprv_strchr(s, c) U_STANDARD_CPP_NAMESPACE strchr(s, c)
#تعریف_کړه uprv_strstr(s, c) U_STANDARD_CPP_NAMESPACE مزي_پلټنه(s, c)
#تعریف_کړه uprv_strrchr(s, c) U_STANDARD_CPP_NAMESPACE strrchr(s, c)
#تعریف_کړه uprv_strncpy(dst, src, اندازه_2) U_STANDARD_CPP_NAMESPACE strncpy(dst, src, اندازه_2)
#تعریف_کړه uprv_strncmp(s1, s2, n) U_STANDARD_CPP_NAMESPACE مزي_پرتله(s1, s2, n)
#تعریف_کړه uprv_strncat(dst, src, n) U_STANDARD_CPP_NAMESPACE strncat(dst, src, n)

/**
 * Is c an ASCII-repertoire letter a-z or A-Z?
 * Note: The implementation is specific to whether ICU is compiled for
 * an ASCII-based or EBCDIC-based machine. There just does not seem to be a better name for this.
 */
U_CAPI UBool U_EXPORT2
uprv_isASCIILetter(حرف c);

// NOTE: For u_asciiToUpper that takes a UChar, see ustr_imp.h

U_CAPI حرف U_EXPORT2
uprv_toupper(حرف c);


U_CAPI حرف U_EXPORT2
uprv_asciitolower(حرف c);

U_CAPI حرف U_EXPORT2
uprv_ebcdictolower(حرف c);

#إذا U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_tolower uprv_asciitolower
#وإلاإذا U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_tolower uprv_ebcdictolower
#وإلا
#   error U_CHARSET_FAMILY is not valid
#د_که_پای

#تعریف_کړه uprv_strtod(سرچینه, پای_ټکی) U_STANDARD_CPP_NAMESPACE strtod(سرچینه, پای_ټکی)
#تعریف_کړه uprv_strtoul(str, پای_ټکی, بنسټ) U_STANDARD_CPP_NAMESPACE strtoul(str, پای_ټکی, بنسټ)
#تعریف_کړه uprv_strtol(str, پای_ټکی, بنسټ) U_STANDARD_CPP_NAMESPACE strtol(str, پای_ټکی, بنسټ)

/* Conversion from a digit to the character with radix base from 2-19 */
/* May need to use U_UPPER_ORDINAL*/
#تعریف_کړه T_CString_itos_دننه_واټن(a) ((a)<=9?('0'+(a)):('A'+(a)-10))

U_CAPI حرف* U_EXPORT2
uprv_strdup(ثابت حرف *src);

/**
 * uprv_malloc n+1 bytes, and copy n bytes from src into the new string.
 * Terminate with a null at offset n.   If n is -1, works like uprv_strdup
 * @param src
 * @param n length of the input string, not including null.
 * @return new string (owned by caller, use uprv_free to free).
 * @internal
 */
U_CAPI حرف* U_EXPORT2
uprv_strndup(ثابت حرف *src, int32_t n);

U_CAPI حرف* U_EXPORT2
T_CString_منزل_ښکتنی_Case(حرف* str);

U_CAPI حرف* U_EXPORT2
T_CString_منزل_پاسنی_Case(حرف* str);

U_CAPI int32_t U_EXPORT2
T_CString_integer_منزل_String(حرف *بفر, int32_t n, int32_t radix);

U_CAPI int32_t U_EXPORT2
T_CString_int64_منزل_String(حرف *بفر, int64_t n, uint32_t radix);

U_CAPI int32_t U_EXPORT2
T_CString_string_منزل_Integer(ثابت حرف *integerString, int32_t radix);

/**
 * Case-insensitive, language-independent string comparison
 * limited to the ASCII character repertoire.
 */
U_CAPI صحيح U_EXPORT2
uprv_stricmp(ثابت حرف *str1, ثابت حرف *str2);

/**
 * Case-insensitive, language-independent string comparison
 * limited to the ASCII character repertoire.
 */
U_CAPI صحيح U_EXPORT2
uprv_strnicmp(ثابت حرف *str1, ثابت حرف *str2, uint32_t n);

#د_که_پای /* ! CSTRING_H */
