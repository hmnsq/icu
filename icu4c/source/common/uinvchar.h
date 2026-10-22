// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1999-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uinvchar.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:2
*
*   created on: 2004sep14
*   created by: Markus W. Scherer
*
*   Definitions for handling invariant characters, moved here from putil.c
*   for better modularization.
*/

#నిర్వచితం_కాకపోతే __UINVCHAR_H__
#నిర్వచించు __UINVCHAR_H__

#అన్తర్భావ "unicode/utypes.h"
#నిర్వచితమైతే __cplusplus
#అన్తర్భావ "unicode/unistr.h"
#అయితే_ముగింపు

/**
 * Check if a char string only contains invariant characters.
 * See utypes.h for details.
 *
 * @param s Input string pointer.
 * @param length Length of the string, can be -1 if NUL-terminated.
 * @return true if s contains only invariant characters.
 *
 * @internal (ICU 2.8)
 */
U_CAPI UBool U_EXPORT2
uprv_isInvariantString(స్థిరరాశి అక్షరం *s, int32_t నిడివి);

/**
 * Check if a Unicode string only contains invariant characters.
 * See utypes.h for details.
 *
 * @param s Input string pointer.
 * @param length Length of the string, can be -1 if NUL-terminated.
 * @return true if s contains only invariant characters.
 *
 * @internal (ICU 2.8)
 */
U_CAPI UBool U_EXPORT2
uprv_isInvariantUString(స్థిరరాశి UChar *s, int32_t నిడివి);

/**
 * \def U_UPPER_ORDINAL
 * Get the ordinal number of an uppercase invariant character
 * @internal
 */
#ఒకవేళ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define U_పై_ORDINAL(x) ((x)-'A')
#లేదావేళ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define U_పై_ORDINAL(x) (((x) < 'J') ? ((x)-'A') : \
                              (((x) < 'S') ? ((x)-'J'+9) : \
                               ((x)-'S'+18)))
#లేకపోతే
#   error Unknown charset family!
#అయితే_ముగింపు

#నిర్వచితమైతే __cplusplus

U_NAMESPACE_BEGIN

/**
 * Like U_UPPER_ORDINAL(x) but with validation.
 * Returns 0..25 for A..Z else a value outside 0..25.
 */
అంతఃవరుస int32_t uprv_పై_Ordinal(int32_t c) {
#ఒకవేళ U_CHARSET_FAMILY==U_ASCII_FAMILY
    తిరిగి c - 'A';
#లేదావేళ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    // EBCDIC: A-Z (26 letters) is split into three ranges A-I (9 letters), J-R (9), S-Z (8).
    // https://en.wikipedia.org/wiki/EBCDIC_037#Codepage_layout
    ఒకవేళ (c <= 'I') { తిరిగి c - 'A'; }  // A-I --> 0-8
    ఒకవేళ (c < 'J') { తిరిగి -1; }
    ఒకవేళ (c <= 'R') { తిరిగి c - 'J' + 9; }  // J-R --> 9..17
    ఒకవేళ (c < 'S') { తిరిగి -1; }
    తిరిగి c - 'S' + 18;  // S-Z --> 18..25
#లేకపోతే
#   error Unknown charset family!
#అయితే_ముగింపు
}

// Like U_UPPER_ORDINAL(x) but for lowercase and with validation.
// Returns 0..25 for a..z else a value outside 0..25.
అంతఃవరుస int32_t uprv_క్రింది_Ordinal(int32_t c) {
#ఒకవేళ U_CHARSET_FAMILY==U_ASCII_FAMILY
    తిరిగి c - 'a';
#లేదావేళ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    // EBCDIC: a-z (26 letters) is split into three ranges a-i (9 letters), j-r (9), s-z (8).
    // https://en.wikipedia.org/wiki/EBCDIC_037#Codepage_layout
    ఒకవేళ (c <= 'i') { తిరిగి c - 'a'; }  // a-i --> 0-8
    ఒకవేళ (c < 'j') { తిరిగి -1; }
    ఒకవేళ (c <= 'r') { తిరిగి c - 'j' + 9; }  // j-r --> 9..17
    ఒకవేళ (c < 's') { తిరిగి -1; }
    తిరిగి c - 's' + 18;  // s-z --> 18..25
#లేకపోతే
#   error Unknown charset family!
#అయితే_ముగింపు
}

U_NAMESPACE_చివర

#అయితే_ముగింపు

/**
 * Returns true if c == '@' is possible.
 * The @ sign is variant, and the @ sign used on one
 * EBCDIC machine won't be compiled the same way on other EBCDIC based machines.
 * @internal
 */
U_CAPI UBool
uprv_isEbcdicAtSign(అక్షరం c);

/**
 * \def uprv_isAtSign
 * Returns true if c == '@' is possible.
 * For ASCII, checks for exactly '@'. For EBCDIC, calls uprv_isEbcdicAtSign().
 * @internal
 */
#ఒకవేళ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_isAtSign(c) ((c)=='@')
#లేదావేళ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_isAtSign(c) uprv_isEbcdicAtSign(c)
#లేకపోతే
#   error Unknown charset family!
#అయితే_ముగింపు

/**
 * Compare two EBCDIC invariant-character strings in ASCII order.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uprv_compareInvEbcdicAsAscii(స్థిరరాశి అక్షరం *s1, స్థిరరాశి అక్షరం *s2);

/**
 * \def uprv_compareInvCharsAsAscii
 * Compare two invariant-character strings in ASCII order.
 * @internal
 */
#ఒకవేళ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_compareInvCharsAsAscii(s1, s2) uprv_strcmp(s1, s2)
#లేదావేళ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_compareInvCharsAsAscii(s1, s2) uprv_compareInvEbcdicAsAscii(s1, s2)
#లేకపోతే
#   error Unknown charset family!
#అయితే_ముగింపు

/**
 * Converts an EBCDIC invariant character to ASCII.
 * @internal
 */
U_CAPI అక్షరం U_EXPORT2
uprv_ebcdic_గమ్యం_Ascii(అక్షరం c);

/**
 * \def uprv_invCharToAscii
 * Converts an invariant character to ASCII.
 * @internal
 */
#ఒకవేళ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_inv_Char_గమ్యం_Ascii(c) (c)
#లేదావేళ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_inv_Char_గమ్యం_Ascii(c) uprv_ebcdic_గమ్యం_Ascii(c)
#లేకపోతే
#   error Unknown charset family!
#అయితే_ముగింపు

/**
 * Converts an EBCDIC invariant character to lowercase ASCII.
 * @internal
 */
U_CAPI అక్షరం U_EXPORT2
uprv_ebcdic_గమ్యం_Lowercase_Ascii(అక్షరం c);

/**
 * \def uprv_invCharToLowercaseAscii
 * Converts an invariant character to lowercase ASCII.
 * @internal
 */
#ఒకవేళ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_inv_Char_గమ్యం_Lowercase_Ascii uprv_asciitolower
#లేదావేళ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_inv_Char_గమ్యం_Lowercase_Ascii uprv_ebcdic_గమ్యం_Lowercase_Ascii
#లేకపోతే
#   error Unknown charset family!
#అయితే_ముగింపు

/**
 * Copy EBCDIC to ASCII
 * @internal
 * @see uprv_strncpy
 */
U_CAPI uint8_t* U_EXPORT2
uprv_aestrncpy(uint8_t *dst, స్థిరరాశి uint8_t *src, int32_t n);


/**
 * Copy ASCII to EBCDIC
 * @internal
 * @see uprv_strncpy
 */
U_CAPI uint8_t* U_EXPORT2
uprv_eastrncpy(uint8_t *dst, స్థిరరాశి uint8_t *src, int32_t n);



#అయితే_ముగింపు
