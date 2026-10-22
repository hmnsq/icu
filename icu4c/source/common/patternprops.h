// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  patternprops.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2011mar13
*   created by: Markus W. Scherer
*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __PATTERNPROPS_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __PATTERNPROPS_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * Implements the immutable Unicode properties Pattern_Syntax and Pattern_White_Space.
 * Hardcodes these properties, does not load data, does not depend on other ICU classes.
 * <p>
 * Note: Both properties include ASCII as well as non-ASCII, non-Latin-1 code points,
 * and both properties only include BMP code points (no supplementary ones).
 * Pattern_Syntax includes some unassigned code points.
 * <p>
 * [:Pattern_White_Space:] =
 *   [\u0009-\u000D\ \u0085\u200E\u200F\u2028\u2029]
 * <p>
 * [:Pattern_Syntax:] =
 *   [!-/\:-@\[-\^`\{-~\u00A1-\u00A7\u00A9\u00AB\u00AC\u00AE
 *    \u00B0\u00B1\u00B6\u00BB\u00BF\u00D7\u00F7
 *    \u2010-\u2027\u2030-\u203E\u2041-\u2053\u2055-\u205E
 *    \u2190-\u245F\u2500-\u2775\u2794-\u2BFF\u2E00-\u2E7F
 *    \u3001-\u3003\u3008-\u3020\u3030\uFD3E\uFD3F\uFE45\uFE46]
 * @author mscherer
 */
class U_COMMON_API ਨਮੂਨਾ_Props {
ਜਨਤਕ:
    /**
     * @return true if c is a Pattern_Syntax code point.
     */
    ਸਥਿਰ UBool isSyntax(UChar32 c);

    /**
     * @return true if c is a Pattern_Syntax or Pattern_White_Space code point.
     */
    ਸਥਿਰ UBool isSyntaxOrWhiteSpace(UChar32 c);

    /**
     * @return true if c is a Pattern_White_Space character.
     */
    ਸਥਿਰ UBool isWhiteSpace(UChar32 c);

    /**
     * Skips over Pattern_White_Space starting at s.
     * @return The smallest pointer at or after s with a non-white space character.
     */
    ਸਥਿਰ ਅਚਲ char16_t *skipWhiteSpace(ਅਚਲ char16_t *s, int32_t ਲੰਮਾਈ);

    /**
     * Skips over Pattern_White_Space starting at index start in s.
     * @return The smallest index at or after start with a non-white space character.
     */
    ਸਥਿਰ int32_t skipWhiteSpace(ਅਚਲ UnicodeString &s, int32_t ਸ਼ੁਰੂ);

    /**
     * @return s except with leading and trailing Pattern_White_Space removed and length adjusted.
     */
    ਸਥਿਰ ਅਚਲ char16_t *trimWhiteSpace(ਅਚਲ char16_t *s, int32_t &ਲੰਮਾਈ);

    /**
     * Tests whether the string contains a "pattern identifier", that is,
     * whether it contains only non-Pattern_White_Space, non-Pattern_Syntax characters.
     * @return true if there are no Pattern_White_Space or Pattern_Syntax characters in s.
     */
    ਸਥਿਰ UBool isIdentifier(ਅਚਲ char16_t *s, int32_t ਲੰਮਾਈ);

    /**
     * Skips over a "pattern identifier" starting at index s.
     * @return The smallest pointer at or after s with
     *         a Pattern_White_Space or Pattern_Syntax character.
     */
    ਸਥਿਰ ਅਚਲ char16_t *skipIdentifier(ਅਚਲ char16_t *s, int32_t ਲੰਮਾਈ);

ਨਿਜੀ:
    ਨਮੂਨਾ_Props() = delete;  // no constructor: all static methods
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  // __PATTERNPROPS_H__
