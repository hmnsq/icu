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

#నిర్వచితం_కాకపోతే CSTRING_H
#నిర్వచించు CSTRING_H 1

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ <string.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <ctype.h>

#నిర్వచించు uprv_strcpy(dst, src) U_STANDARD_CPP_NAMESPACE  strcpy(dst, src)
#నిర్వచించు uprv_strlen(str) U_STANDARD_CPP_NAMESPACE అక్షరశ్రేణి_పొడవు(str)
#నిర్వచించు uprv_strcmp(s1, s2) U_STANDARD_CPP_NAMESPACE strcmp(s1, s2)
#నిర్వచించు uprv_strcat(dst, src) U_STANDARD_CPP_NAMESPACE strcat(dst, src)
#నిర్వచించు uprv_strchr(s, c) U_STANDARD_CPP_NAMESPACE strchr(s, c)
#నిర్వచించు uprv_strstr(s, c) U_STANDARD_CPP_NAMESPACE అక్షరశ్రేణి_శోధన(s, c)
#నిర్వచించు uprv_strrchr(s, c) U_STANDARD_CPP_NAMESPACE strrchr(s, c)
#నిర్వచించు uprv_strncpy(dst, src, కొలత_2) U_STANDARD_CPP_NAMESPACE strncpy(dst, src, కొలత_2)
#నిర్వచించు uprv_strncmp(s1, s2, n) U_STANDARD_CPP_NAMESPACE అక్షరశ్రేణి_పోలిక(s1, s2, n)
#నిర్వచించు uprv_strncat(dst, src, n) U_STANDARD_CPP_NAMESPACE strncat(dst, src, n)

/**
 * Is c an ASCII-repertoire letter a-z or A-Z?
 * Note: The implementation is specific to whether ICU is compiled for
 * an ASCII-based or EBCDIC-based machine. There just does not seem to be a better name for this.
 */
U_CAPI UBool U_EXPORT2
uprv_isASCIILetter(అక్షరం c);

// NOTE: For u_asciiToUpper that takes a UChar, see ustr_imp.h

U_CAPI అక్షరం U_EXPORT2
uprv_toupper(అక్షరం c);


U_CAPI అక్షరం U_EXPORT2
uprv_asciitolower(అక్షరం c);

U_CAPI అక్షరం U_EXPORT2
uprv_ebcdictolower(అక్షరం c);

#ఒకవేళ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_tolower uprv_asciitolower
#లేదావేళ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_tolower uprv_ebcdictolower
#లేకపోతే
#   error U_CHARSET_FAMILY is not valid
#అయితే_ముగింపు

#నిర్వచించు uprv_strtod(ఆధారం, చివర) U_STANDARD_CPP_NAMESPACE strtod(ఆధారం, చివర)
#నిర్వచించు uprv_strtoul(str, చివర, పునాది) U_STANDARD_CPP_NAMESPACE strtoul(str, చివర, పునాది)
#నిర్వచించు uprv_strtol(str, చివర, పునాది) U_STANDARD_CPP_NAMESPACE strtol(str, చివర, పునాది)

/* Conversion from a digit to the character with radix base from 2-19 */
/* May need to use U_UPPER_ORDINAL*/
#నిర్వచించు T_CString_itos_స్థానభ్రంశం(a) ((a)<=9?('0'+(a)):('A'+(a)-10))

U_CAPI అక్షరం* U_EXPORT2
uprv_strdup(స్థిరరాశి అక్షరం *src);

/**
 * uprv_malloc n+1 bytes, and copy n bytes from src into the new string.
 * Terminate with a null at offset n.   If n is -1, works like uprv_strdup
 * @param src
 * @param n length of the input string, not including null.
 * @return new string (owned by caller, use uprv_free to free).
 * @internal
 */
U_CAPI అక్షరం* U_EXPORT2
uprv_strndup(స్థిరరాశి అక్షరం *src, int32_t n);

U_CAPI అక్షరం* U_EXPORT2
T_CString_గమ్యం_క్రింది_Case(అక్షరం* str);

U_CAPI అక్షరం* U_EXPORT2
T_CString_గమ్యం_పై_Case(అక్షరం* str);

U_CAPI int32_t U_EXPORT2
T_CString_integer_గమ్యం_String(అక్షరం *బఫరు, int32_t n, int32_t radix);

U_CAPI int32_t U_EXPORT2
T_CString_int64_గమ్యం_String(అక్షరం *బఫరు, int64_t n, uint32_t radix);

U_CAPI int32_t U_EXPORT2
T_CString_string_గమ్యం_Integer(స్థిరరాశి అక్షరం *integerString, int32_t radix);

/**
 * Case-insensitive, language-independent string comparison
 * limited to the ASCII character repertoire.
 */
U_CAPI పూర్ణాంకం U_EXPORT2
uprv_stricmp(స్థిరరాశి అక్షరం *str1, స్థిరరాశి అక్షరం *str2);

/**
 * Case-insensitive, language-independent string comparison
 * limited to the ASCII character repertoire.
 */
U_CAPI పూర్ణాంకం U_EXPORT2
uprv_strnicmp(స్థిరరాశి అక్షరం *str1, స్థిరరాశి అక్షరం *str2, uint32_t n);

#అయితే_ముగింపు /* ! CSTRING_H */
