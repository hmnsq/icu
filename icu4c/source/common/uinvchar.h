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

#اگر_تعریف_نشده __UINVCHAR_H__
#تعریف_کن __UINVCHAR_H__

#ضمن "unicode/utypes.h"
#اگر_تعریف_شده __cplusplus
#ضمن "unicode/unistr.h"
#پایان_اگر

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
uprv_isInvariantString(ثابت حرف *s, int32_t درازی);

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
uprv_isInvariantUString(ثابت UChar *s, int32_t درازی);

/**
 * \def U_UPPER_ORDINAL
 * Get the ordinal number of an uppercase invariant character
 * @internal
 */
#إذا U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define U_فوقانی_ORDINAL(x) ((x)-'A')
#وإلاإذا U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define U_فوقانی_ORDINAL(x) (((x) < 'J') ? ((x)-'A') : \
                              (((x) < 'S') ? ((x)-'J'+9) : \
                               ((x)-'S'+18)))
#وإلا
#   error Unknown charset family!
#پایان_اگر

#اگر_تعریف_شده __cplusplus

U_NAMESPACE_BEGIN

/**
 * Like U_UPPER_ORDINAL(x) but with validation.
 * Returns 0..25 for A..Z else a value outside 0..25.
 */
مضمن int32_t uprv_فوقانی_Ordinal(int32_t c) {
#إذا U_CHARSET_FAMILY==U_ASCII_FAMILY
    أرجع c - 'A';
#وإلاإذا U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    // EBCDIC: A-Z (26 letters) is split into three ranges A-I (9 letters), J-R (9), S-Z (8).
    // https://en.wikipedia.org/wiki/EBCDIC_037#Codepage_layout
    إذا (c <= 'I') { أرجع c - 'A'; }  // A-I --> 0-8
    إذا (c < 'J') { أرجع -1; }
    إذا (c <= 'R') { أرجع c - 'J' + 9; }  // J-R --> 9..17
    إذا (c < 'S') { أرجع -1; }
    أرجع c - 'S' + 18;  // S-Z --> 18..25
#وإلا
#   error Unknown charset family!
#پایان_اگر
}

// Like U_UPPER_ORDINAL(x) but for lowercase and with validation.
// Returns 0..25 for a..z else a value outside 0..25.
مضمن int32_t uprv_پایینی_Ordinal(int32_t c) {
#إذا U_CHARSET_FAMILY==U_ASCII_FAMILY
    أرجع c - 'a';
#وإلاإذا U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    // EBCDIC: a-z (26 letters) is split into three ranges a-i (9 letters), j-r (9), s-z (8).
    // https://en.wikipedia.org/wiki/EBCDIC_037#Codepage_layout
    إذا (c <= 'i') { أرجع c - 'a'; }  // a-i --> 0-8
    إذا (c < 'j') { أرجع -1; }
    إذا (c <= 'r') { أرجع c - 'j' + 9; }  // j-r --> 9..17
    إذا (c < 's') { أرجع -1; }
    أرجع c - 's' + 18;  // s-z --> 18..25
#وإلا
#   error Unknown charset family!
#پایان_اگر
}

U_NAMESPACE_انتها

#پایان_اگر

/**
 * Returns true if c == '@' is possible.
 * The @ sign is variant, and the @ sign used on one
 * EBCDIC machine won't be compiled the same way on other EBCDIC based machines.
 * @internal
 */
U_CAPI UBool
uprv_isEbcdicAtSign(حرف c);

/**
 * \def uprv_isAtSign
 * Returns true if c == '@' is possible.
 * For ASCII, checks for exactly '@'. For EBCDIC, calls uprv_isEbcdicAtSign().
 * @internal
 */
#إذا U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_isAtSign(c) ((c)=='@')
#وإلاإذا U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_isAtSign(c) uprv_isEbcdicAtSign(c)
#وإلا
#   error Unknown charset family!
#پایان_اگر

/**
 * Compare two EBCDIC invariant-character strings in ASCII order.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uprv_compareInvEbcdicAsAscii(ثابت حرف *s1, ثابت حرف *s2);

/**
 * \def uprv_compareInvCharsAsAscii
 * Compare two invariant-character strings in ASCII order.
 * @internal
 */
#إذا U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_compareInvCharsAsAscii(s1, s2) uprv_strcmp(s1, s2)
#وإلاإذا U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_compareInvCharsAsAscii(s1, s2) uprv_compareInvEbcdicAsAscii(s1, s2)
#وإلا
#   error Unknown charset family!
#پایان_اگر

/**
 * Converts an EBCDIC invariant character to ASCII.
 * @internal
 */
U_CAPI حرف U_EXPORT2
uprv_ebcdic_مقصد_Ascii(حرف c);

/**
 * \def uprv_invCharToAscii
 * Converts an invariant character to ASCII.
 * @internal
 */
#إذا U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_inv_Char_مقصد_Ascii(c) (c)
#وإلاإذا U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_inv_Char_مقصد_Ascii(c) uprv_ebcdic_مقصد_Ascii(c)
#وإلا
#   error Unknown charset family!
#پایان_اگر

/**
 * Converts an EBCDIC invariant character to lowercase ASCII.
 * @internal
 */
U_CAPI حرف U_EXPORT2
uprv_ebcdic_مقصد_Lowercase_Ascii(حرف c);

/**
 * \def uprv_invCharToLowercaseAscii
 * Converts an invariant character to lowercase ASCII.
 * @internal
 */
#إذا U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_inv_Char_مقصد_Lowercase_Ascii uprv_asciitolower
#وإلاإذا U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_inv_Char_مقصد_Lowercase_Ascii uprv_ebcdic_مقصد_Lowercase_Ascii
#وإلا
#   error Unknown charset family!
#پایان_اگر

/**
 * Copy EBCDIC to ASCII
 * @internal
 * @see uprv_strncpy
 */
U_CAPI uint8_t* U_EXPORT2
uprv_aestrncpy(uint8_t *dst, ثابت uint8_t *src, int32_t n);


/**
 * Copy ASCII to EBCDIC
 * @internal
 * @see uprv_strncpy
 */
U_CAPI uint8_t* U_EXPORT2
uprv_eastrncpy(uint8_t *dst, ثابت uint8_t *src, int32_t n);



#پایان_اگر
