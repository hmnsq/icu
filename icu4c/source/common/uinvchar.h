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

#അന്തര്ഭാവ "unicode/utypes.h"
#ifdef __cplusplus
#അന്തര്ഭാവ "unicode/unistr.h"
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
uprv_isInvariantString(സ്ഥിരം അക്ഷരം *s, int32_t length);

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
uprv_isInvariantUString(സ്ഥിരം UChar *s, int32_t length);

/**
 * \def U_UPPER_ORDINAL
 * Get the ordinal number of an uppercase invariant character
 * @internal
 */
#എങ്കിൽ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define U_UPPER_ORDINAL(x) ((x)-'A')
#അല്ലെങ്കിലെങ്കിൽ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define U_UPPER_ORDINAL(x) (((x) < 'J') ? ((x)-'A') : \
                              (((x) < 'S') ? ((x)-'J'+9) : \
                               ((x)-'S'+18)))
#അല്ലെങ്കിൽ
#   error Unknown charset family!
#endif

#ifdef __cplusplus

U_NAMESPACE_BEGIN

/**
 * Like U_UPPER_ORDINAL(x) but with validation.
 * Returns 0..25 for A..Z else a value outside 0..25.
 */
വരിയിൽ int32_t uprv_upperOrdinal(int32_t c) {
#എങ്കിൽ U_CHARSET_FAMILY==U_ASCII_FAMILY
    തിരികെ c - 'A';
#അല്ലെങ്കിലെങ്കിൽ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    // EBCDIC: A-Z (26 letters) is split into three ranges A-I (9 letters), J-R (9), S-Z (8).
    // https://en.wikipedia.org/wiki/EBCDIC_037#Codepage_layout
    എങ്കിൽ (c <= 'I') { തിരികെ c - 'A'; }  // A-I --> 0-8
    എങ്കിൽ (c < 'J') { തിരികെ -1; }
    എങ്കിൽ (c <= 'R') { തിരികെ c - 'J' + 9; }  // J-R --> 9..17
    എങ്കിൽ (c < 'S') { തിരികെ -1; }
    തിരികെ c - 'S' + 18;  // S-Z --> 18..25
#അല്ലെങ്കിൽ
#   error Unknown charset family!
#endif
}

// Like U_UPPER_ORDINAL(x) but for lowercase and with validation.
// Returns 0..25 for a..z else a value outside 0..25.
വരിയിൽ int32_t uprv_lowerOrdinal(int32_t c) {
#എങ്കിൽ U_CHARSET_FAMILY==U_ASCII_FAMILY
    തിരികെ c - 'a';
#അല്ലെങ്കിലെങ്കിൽ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    // EBCDIC: a-z (26 letters) is split into three ranges a-i (9 letters), j-r (9), s-z (8).
    // https://en.wikipedia.org/wiki/EBCDIC_037#Codepage_layout
    എങ്കിൽ (c <= 'i') { തിരികെ c - 'a'; }  // a-i --> 0-8
    എങ്കിൽ (c < 'j') { തിരികെ -1; }
    എങ്കിൽ (c <= 'r') { തിരികെ c - 'j' + 9; }  // j-r --> 9..17
    എങ്കിൽ (c < 's') { തിരികെ -1; }
    തിരികെ c - 's' + 18;  // s-z --> 18..25
#അല്ലെങ്കിൽ
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
uprv_isEbcdicAtSign(അക്ഷരം c);

/**
 * \def uprv_isAtSign
 * Returns true if c == '@' is possible.
 * For ASCII, checks for exactly '@'. For EBCDIC, calls uprv_isEbcdicAtSign().
 * @internal
 */
#എങ്കിൽ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_isAtSign(c) ((c)=='@')
#അല്ലെങ്കിലെങ്കിൽ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_isAtSign(c) uprv_isEbcdicAtSign(c)
#അല്ലെങ്കിൽ
#   error Unknown charset family!
#endif

/**
 * Compare two EBCDIC invariant-character strings in ASCII order.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uprv_compareInvEbcdicAsAscii(സ്ഥിരം അക്ഷരം *s1, സ്ഥിരം അക്ഷരം *s2);

/**
 * \def uprv_compareInvCharsAsAscii
 * Compare two invariant-character strings in ASCII order.
 * @internal
 */
#എങ്കിൽ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_compareInvCharsAsAscii(s1, s2) uprv_strcmp(s1, s2)
#അല്ലെങ്കിലെങ്കിൽ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_compareInvCharsAsAscii(s1, s2) uprv_compareInvEbcdicAsAscii(s1, s2)
#അല്ലെങ്കിൽ
#   error Unknown charset family!
#endif

/**
 * Converts an EBCDIC invariant character to ASCII.
 * @internal
 */
U_CAPI അക്ഷരം U_EXPORT2
uprv_ebcdicToAscii(അക്ഷരം c);

/**
 * \def uprv_invCharToAscii
 * Converts an invariant character to ASCII.
 * @internal
 */
#എങ്കിൽ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_invCharToAscii(c) (c)
#അല്ലെങ്കിലെങ്കിൽ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_invCharToAscii(c) uprv_ebcdicToAscii(c)
#അല്ലെങ്കിൽ
#   error Unknown charset family!
#endif

/**
 * Converts an EBCDIC invariant character to lowercase ASCII.
 * @internal
 */
U_CAPI അക്ഷരം U_EXPORT2
uprv_ebcdicToLowercaseAscii(അക്ഷരം c);

/**
 * \def uprv_invCharToLowercaseAscii
 * Converts an invariant character to lowercase ASCII.
 * @internal
 */
#എങ്കിൽ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_invCharToLowercaseAscii uprv_asciitolower
#അല്ലെങ്കിലെങ്കിൽ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_invCharToLowercaseAscii uprv_ebcdicToLowercaseAscii
#അല്ലെങ്കിൽ
#   error Unknown charset family!
#endif

/**
 * Copy EBCDIC to ASCII
 * @internal
 * @see uprv_strncpy
 */
U_CAPI uint8_t* U_EXPORT2
uprv_aestrncpy(uint8_t *dst, സ്ഥിരം uint8_t *src, int32_t n);


/**
 * Copy ASCII to EBCDIC
 * @internal
 * @see uprv_strncpy
 */
U_CAPI uint8_t* U_EXPORT2
uprv_eastrncpy(uint8_t *dst, സ്ഥിരം uint8_t *src, int32_t n);



#endif
