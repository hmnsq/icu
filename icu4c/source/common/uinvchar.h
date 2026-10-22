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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __UINVCHAR_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __UINVCHAR_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಆದರೆ_ಅಂತ್ಯ

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
uprv_isInvariantString(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int32_t ಉದ್ದಳತೆ);

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
uprv_isInvariantUString(ಸ್ಥಿರಾಂಕ UChar *s, int32_t ಉದ್ದಳತೆ);

/**
 * \def U_UPPER_ORDINAL
 * Get the ordinal number of an uppercase invariant character
 * @internal
 */
#ಆದರೆ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define U_ಊರ್ಧ್ವ_ORDINAL(x) ((x)-'A')
#ಅಥವಾಆದರೆ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define U_ಊರ್ಧ್ವ_ORDINAL(x) (((x) < 'J') ? ((x)-'A') : \
                              (((x) < 'S') ? ((x)-'J'+9) : \
                               ((x)-'S'+18)))
#ಇಲ್ಲದಿದ್ದರೆ
#   error Unknown charset family!
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus

U_NAMESPACE_BEGIN

/**
 * Like U_UPPER_ORDINAL(x) but with validation.
 * Returns 0..25 for A..Z else a value outside 0..25.
 */
ಸಾಲೊಳಗೆ int32_t uprv_ಊರ್ಧ್ವ_Ordinal(int32_t c) {
#ಆದರೆ U_CHARSET_FAMILY==U_ASCII_FAMILY
    ಹಿಂತಿರುಗಿ c - 'A';
#ಅಥವಾಆದರೆ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    // EBCDIC: A-Z (26 letters) is split into three ranges A-I (9 letters), J-R (9), S-Z (8).
    // https://en.wikipedia.org/wiki/EBCDIC_037#Codepage_layout
    ಆದರೆ (c <= 'I') { ಹಿಂತಿರುಗಿ c - 'A'; }  // A-I --> 0-8
    ಆದರೆ (c < 'J') { ಹಿಂತಿರುಗಿ -1; }
    ಆದರೆ (c <= 'R') { ಹಿಂತಿರುಗಿ c - 'J' + 9; }  // J-R --> 9..17
    ಆದರೆ (c < 'S') { ಹಿಂತಿರುಗಿ -1; }
    ಹಿಂತಿರುಗಿ c - 'S' + 18;  // S-Z --> 18..25
#ಇಲ್ಲದಿದ್ದರೆ
#   error Unknown charset family!
#ಆದರೆ_ಅಂತ್ಯ
}

// Like U_UPPER_ORDINAL(x) but for lowercase and with validation.
// Returns 0..25 for a..z else a value outside 0..25.
ಸಾಲೊಳಗೆ int32_t uprv_ಅಧೋ_Ordinal(int32_t c) {
#ಆದರೆ U_CHARSET_FAMILY==U_ASCII_FAMILY
    ಹಿಂತಿರುಗಿ c - 'a';
#ಅಥವಾಆದರೆ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    // EBCDIC: a-z (26 letters) is split into three ranges a-i (9 letters), j-r (9), s-z (8).
    // https://en.wikipedia.org/wiki/EBCDIC_037#Codepage_layout
    ಆದರೆ (c <= 'i') { ಹಿಂತಿರುಗಿ c - 'a'; }  // a-i --> 0-8
    ಆದರೆ (c < 'j') { ಹಿಂತಿರುಗಿ -1; }
    ಆದರೆ (c <= 'r') { ಹಿಂತಿರುಗಿ c - 'j' + 9; }  // j-r --> 9..17
    ಆದರೆ (c < 's') { ಹಿಂತಿರುಗಿ -1; }
    ಹಿಂತಿರುಗಿ c - 's' + 18;  // s-z --> 18..25
#ಇಲ್ಲದಿದ್ದರೆ
#   error Unknown charset family!
#ಆದರೆ_ಅಂತ್ಯ
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ

/**
 * Returns true if c == '@' is possible.
 * The @ sign is variant, and the @ sign used on one
 * EBCDIC machine won't be compiled the same way on other EBCDIC based machines.
 * @internal
 */
U_CAPI UBool
uprv_isEbcdicAtSign(ಅಕ್ಷರ c);

/**
 * \def uprv_isAtSign
 * Returns true if c == '@' is possible.
 * For ASCII, checks for exactly '@'. For EBCDIC, calls uprv_isEbcdicAtSign().
 * @internal
 */
#ಆದರೆ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_isAtSign(c) ((c)=='@')
#ಅಥವಾಆದರೆ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_isAtSign(c) uprv_isEbcdicAtSign(c)
#ಇಲ್ಲದಿದ್ದರೆ
#   error Unknown charset family!
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Compare two EBCDIC invariant-character strings in ASCII order.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uprv_compareInvEbcdicAsAscii(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s2);

/**
 * \def uprv_compareInvCharsAsAscii
 * Compare two invariant-character strings in ASCII order.
 * @internal
 */
#ಆದರೆ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_compareInvCharsAsAscii(s1, s2) uprv_strcmp(s1, s2)
#ಅಥವಾಆದರೆ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_compareInvCharsAsAscii(s1, s2) uprv_compareInvEbcdicAsAscii(s1, s2)
#ಇಲ್ಲದಿದ್ದರೆ
#   error Unknown charset family!
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Converts an EBCDIC invariant character to ASCII.
 * @internal
 */
U_CAPI ಅಕ್ಷರ U_EXPORT2
uprv_ebcdic_ಗಮ್ಯ_Ascii(ಅಕ್ಷರ c);

/**
 * \def uprv_invCharToAscii
 * Converts an invariant character to ASCII.
 * @internal
 */
#ಆದರೆ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_inv_Char_ಗಮ್ಯ_Ascii(c) (c)
#ಅಥವಾಆದರೆ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_inv_Char_ಗಮ್ಯ_Ascii(c) uprv_ebcdic_ಗಮ್ಯ_Ascii(c)
#ಇಲ್ಲದಿದ್ದರೆ
#   error Unknown charset family!
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Converts an EBCDIC invariant character to lowercase ASCII.
 * @internal
 */
U_CAPI ಅಕ್ಷರ U_EXPORT2
uprv_ebcdic_ಗಮ್ಯ_Lowercase_Ascii(ಅಕ್ಷರ c);

/**
 * \def uprv_invCharToLowercaseAscii
 * Converts an invariant character to lowercase ASCII.
 * @internal
 */
#ಆದರೆ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_inv_Char_ಗಮ್ಯ_Lowercase_Ascii uprv_asciitolower
#ಅಥವಾಆದರೆ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_inv_Char_ಗಮ್ಯ_Lowercase_Ascii uprv_ebcdic_ಗಮ್ಯ_Lowercase_Ascii
#ಇಲ್ಲದಿದ್ದರೆ
#   error Unknown charset family!
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Copy EBCDIC to ASCII
 * @internal
 * @see uprv_strncpy
 */
U_CAPI uint8_t* U_EXPORT2
uprv_aestrncpy(uint8_t *dst, ಸ್ಥಿರಾಂಕ uint8_t *src, int32_t n);


/**
 * Copy ASCII to EBCDIC
 * @internal
 * @see uprv_strncpy
 */
U_CAPI uint8_t* U_EXPORT2
uprv_eastrncpy(uint8_t *dst, ಸ್ಥಿರಾಂಕ uint8_t *src, int32_t n);



#ಆದರೆ_ಅಂತ್ಯ
