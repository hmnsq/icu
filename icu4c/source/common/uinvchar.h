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

#ifndef __UINVCHAR_H__
#define __UINVCHAR_H__

#شامل "unicode/utypes.h"
#ifdef __cplusplus
#شامل "unicode/unistr.h"
#endif

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
uprv_isInvariantString(ثابت نویسه *s, int32_t length);

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
uprv_isInvariantUString(ثابت UChar *s, int32_t length);

/**
 * \def U_UPPER_ORDINAL
 * Get the ordinal number of an uppercase invariant character
 * @internal
 */
#اگر U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define U_UPPER_ORDINAL(x) ((x)-'A')
#وگرنهاگر U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define U_UPPER_ORDINAL(x) (((x) < 'J') ? ((x)-'A') : \
                              (((x) < 'S') ? ((x)-'J'+9) : \
                               ((x)-'S'+18)))
#وگرنه
#   error Unknown charset family!
#endif

#ifdef __cplusplus

U_NAMESPACE_BEGIN

/**
 * Like U_UPPER_ORDINAL(x) but with validation.
 * Returns 0..25 for A..Z else a value outside 0..25.
 */
درونخطی int32_t uprv_upperOrdinal(int32_t c) {
#اگر U_CHARSET_FAMILY==U_ASCII_FAMILY
    بازگشت c - 'A';
#وگرنهاگر U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    // EBCDIC: A-Z (26 letters) is split into three ranges A-I (9 letters), J-R (9), S-Z (8).
    // https://en.wikipedia.org/wiki/EBCDIC_037#Codepage_layout
    اگر (c <= 'I') { بازگشت c - 'A'; }  // A-I --> 0-8
    اگر (c < 'J') { بازگشت -1; }
    اگر (c <= 'R') { بازگشت c - 'J' + 9; }  // J-R --> 9..17
    اگر (c < 'S') { بازگشت -1; }
    بازگشت c - 'S' + 18;  // S-Z --> 18..25
#وگرنه
#   error Unknown charset family!
#endif
}

// Like U_UPPER_ORDINAL(x) but for lowercase and with validation.
// Returns 0..25 for a..z else a value outside 0..25.
درونخطی int32_t uprv_lowerOrdinal(int32_t c) {
#اگر U_CHARSET_FAMILY==U_ASCII_FAMILY
    بازگشت c - 'a';
#وگرنهاگر U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    // EBCDIC: a-z (26 letters) is split into three ranges a-i (9 letters), j-r (9), s-z (8).
    // https://en.wikipedia.org/wiki/EBCDIC_037#Codepage_layout
    اگر (c <= 'i') { بازگشت c - 'a'; }  // a-i --> 0-8
    اگر (c < 'j') { بازگشت -1; }
    اگر (c <= 'r') { بازگشت c - 'j' + 9; }  // j-r --> 9..17
    اگر (c < 's') { بازگشت -1; }
    بازگشت c - 's' + 18;  // s-z --> 18..25
#وگرنه
#   error Unknown charset family!
#endif
}

U_NAMESPACE_END

#endif

/**
 * Returns true if c == '@' is possible.
 * The @ sign is variant, and the @ sign used on one
 * EBCDIC machine won't be compiled the same way on other EBCDIC based machines.
 * @internal
 */
U_CAPI UBool
uprv_isEbcdicAtSign(نویسه c);

/**
 * \def uprv_isAtSign
 * Returns true if c == '@' is possible.
 * For ASCII, checks for exactly '@'. For EBCDIC, calls uprv_isEbcdicAtSign().
 * @internal
 */
#اگر U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_isAtSign(c) ((c)=='@')
#وگرنهاگر U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_isAtSign(c) uprv_isEbcdicAtSign(c)
#وگرنه
#   error Unknown charset family!
#endif

/**
 * Compare two EBCDIC invariant-character strings in ASCII order.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uprv_compareInvEbcdicAsAscii(ثابت نویسه *s1, ثابت نویسه *s2);

/**
 * \def uprv_compareInvCharsAsAscii
 * Compare two invariant-character strings in ASCII order.
 * @internal
 */
#اگر U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_compareInvCharsAsAscii(s1, s2) uprv_strcmp(s1, s2)
#وگرنهاگر U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_compareInvCharsAsAscii(s1, s2) uprv_compareInvEbcdicAsAscii(s1, s2)
#وگرنه
#   error Unknown charset family!
#endif

/**
 * Converts an EBCDIC invariant character to ASCII.
 * @internal
 */
U_CAPI نویسه U_EXPORT2
uprv_ebcdicToAscii(نویسه c);

/**
 * \def uprv_invCharToAscii
 * Converts an invariant character to ASCII.
 * @internal
 */
#اگر U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_invCharToAscii(c) (c)
#وگرنهاگر U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_invCharToAscii(c) uprv_ebcdicToAscii(c)
#وگرنه
#   error Unknown charset family!
#endif

/**
 * Converts an EBCDIC invariant character to lowercase ASCII.
 * @internal
 */
U_CAPI نویسه U_EXPORT2
uprv_ebcdicToLowercaseAscii(نویسه c);

/**
 * \def uprv_invCharToLowercaseAscii
 * Converts an invariant character to lowercase ASCII.
 * @internal
 */
#اگر U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_invCharToLowercaseAscii uprv_asciitolower
#وگرنهاگر U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_invCharToLowercaseAscii uprv_ebcdicToLowercaseAscii
#وگرنه
#   error Unknown charset family!
#endif

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



#endif
