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

#अगर_अघोषित __UINVCHAR_H__
#घोषणा __UINVCHAR_H__

#समावेश "unicode/utypes.h"
#अगर_घोषित __cplusplus
#समावेश "unicode/unistr.h"
#पूर्ण_अगर

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
uprv_isInvariantString(स्थिर अक्षर *s, int32_t लंबाई);

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
uprv_isInvariantUString(स्थिर UChar *s, int32_t लंबाई);

/**
 * \def U_UPPER_ORDINAL
 * Get the ordinal number of an uppercase invariant character
 * @internal
 */
#अगर U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define U_UPPER_ORDINAL(x) ((x)-'A')
#या_अगर U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define U_UPPER_ORDINAL(x) (((x) < 'J') ? ((x)-'A') : \
                              (((x) < 'S') ? ((x)-'J'+9) : \
                               ((x)-'S'+18)))
#अन्यथा
#   error Unknown charset family!
#पूर्ण_अगर

#अगर_घोषित __cplusplus

U_NAMESPACE_BEGIN

/**
 * Like U_UPPER_ORDINAL(x) but with validation.
 * Returns 0..25 for A..Z else a value outside 0..25.
 */
अंतरभूत int32_t uprv_upperOrdinal(int32_t c) {
#अगर U_CHARSET_FAMILY==U_ASCII_FAMILY
    वापस c - 'A';
#या_अगर U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    // EBCDIC: A-Z (26 letters) is split into three ranges A-I (9 letters), J-R (9), S-Z (8).
    // https://en.wikipedia.org/wiki/EBCDIC_037#Codepage_layout
    अगर (c <= 'I') { वापस c - 'A'; }  // A-I --> 0-8
    अगर (c < 'J') { वापस -1; }
    अगर (c <= 'R') { वापस c - 'J' + 9; }  // J-R --> 9..17
    अगर (c < 'S') { वापस -1; }
    वापस c - 'S' + 18;  // S-Z --> 18..25
#अन्यथा
#   error Unknown charset family!
#पूर्ण_अगर
}

// Like U_UPPER_ORDINAL(x) but for lowercase and with validation.
// Returns 0..25 for a..z else a value outside 0..25.
अंतरभूत int32_t uprv_lowerOrdinal(int32_t c) {
#अगर U_CHARSET_FAMILY==U_ASCII_FAMILY
    वापस c - 'a';
#या_अगर U_CHARSET_FAMILY==U_EBCDIC_FAMILY
    // EBCDIC: a-z (26 letters) is split into three ranges a-i (9 letters), j-r (9), s-z (8).
    // https://en.wikipedia.org/wiki/EBCDIC_037#Codepage_layout
    अगर (c <= 'i') { वापस c - 'a'; }  // a-i --> 0-8
    अगर (c < 'j') { वापस -1; }
    अगर (c <= 'r') { वापस c - 'j' + 9; }  // j-r --> 9..17
    अगर (c < 's') { वापस -1; }
    वापस c - 's' + 18;  // s-z --> 18..25
#अन्यथा
#   error Unknown charset family!
#पूर्ण_अगर
}

U_NAMESPACE_END

#पूर्ण_अगर

/**
 * Returns true if c == '@' is possible.
 * The @ sign is variant, and the @ sign used on one
 * EBCDIC machine won't be compiled the same way on other EBCDIC based machines.
 * @internal
 */
U_CAPI UBool
uprv_isEbcdicAtSign(अक्षर c);

/**
 * \def uprv_isAtSign
 * Returns true if c == '@' is possible.
 * For ASCII, checks for exactly '@'. For EBCDIC, calls uprv_isEbcdicAtSign().
 * @internal
 */
#अगर U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_isAtSign(c) ((c)=='@')
#या_अगर U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_isAtSign(c) uprv_isEbcdicAtSign(c)
#अन्यथा
#   error Unknown charset family!
#पूर्ण_अगर

/**
 * Compare two EBCDIC invariant-character strings in ASCII order.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uprv_compareInvEbcdicAsAscii(स्थिर अक्षर *s1, स्थिर अक्षर *s2);

/**
 * \def uprv_compareInvCharsAsAscii
 * Compare two invariant-character strings in ASCII order.
 * @internal
 */
#अगर U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_compareInvCharsAsAscii(s1, s2) uprv_strcmp(s1, s2)
#या_अगर U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_compareInvCharsAsAscii(s1, s2) uprv_compareInvEbcdicAsAscii(s1, s2)
#अन्यथा
#   error Unknown charset family!
#पूर्ण_अगर

/**
 * Converts an EBCDIC invariant character to ASCII.
 * @internal
 */
U_CAPI अक्षर U_EXPORT2
uprv_ebcdicToAscii(अक्षर c);

/**
 * \def uprv_invCharToAscii
 * Converts an invariant character to ASCII.
 * @internal
 */
#अगर U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_invCharToAscii(c) (c)
#या_अगर U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_invCharToAscii(c) uprv_ebcdicToAscii(c)
#अन्यथा
#   error Unknown charset family!
#पूर्ण_अगर

/**
 * Converts an EBCDIC invariant character to lowercase ASCII.
 * @internal
 */
U_CAPI अक्षर U_EXPORT2
uprv_ebcdicToLowercaseAscii(अक्षर c);

/**
 * \def uprv_invCharToLowercaseAscii
 * Converts an invariant character to lowercase ASCII.
 * @internal
 */
#अगर U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_invCharToLowercaseAscii uprv_asciitolower
#या_अगर U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_invCharToLowercaseAscii uprv_ebcdicToLowercaseAscii
#अन्यथा
#   error Unknown charset family!
#पूर्ण_अगर

/**
 * Copy EBCDIC to ASCII
 * @internal
 * @see uprv_strncpy
 */
U_CAPI uint8_t* U_EXPORT2
uprv_aestrncpy(uint8_t *dst, स्थिर uint8_t *src, int32_t n);


/**
 * Copy ASCII to EBCDIC
 * @internal
 * @see uprv_strncpy
 */
U_CAPI uint8_t* U_EXPORT2
uprv_eastrncpy(uint8_t *dst, स्थिर uint8_t *src, int32_t n);



#पूर्ण_अगर
