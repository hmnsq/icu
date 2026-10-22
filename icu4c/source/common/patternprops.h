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

#నిర్వచితం_కాకపోతే __PATTERNPROPS_H__
#నిర్వచించు __PATTERNPROPS_H__

#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/utypes.h"

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
class U_COMMON_API నమూనా_Props {
బహిరంగ:
    /**
     * @return true if c is a Pattern_Syntax code point.
     */
    స్థిర UBool isSyntax(UChar32 c);

    /**
     * @return true if c is a Pattern_Syntax or Pattern_White_Space code point.
     */
    స్థిర UBool isSyntaxOrWhiteSpace(UChar32 c);

    /**
     * @return true if c is a Pattern_White_Space character.
     */
    స్థిర UBool isWhiteSpace(UChar32 c);

    /**
     * Skips over Pattern_White_Space starting at s.
     * @return The smallest pointer at or after s with a non-white space character.
     */
    స్థిర స్థిరరాశి char16_t *skipWhiteSpace(స్థిరరాశి char16_t *s, int32_t నిడివి);

    /**
     * Skips over Pattern_White_Space starting at index start in s.
     * @return The smallest index at or after start with a non-white space character.
     */
    స్థిర int32_t skipWhiteSpace(స్థిరరాశి UnicodeString &s, int32_t ప్రారంభం);

    /**
     * @return s except with leading and trailing Pattern_White_Space removed and length adjusted.
     */
    స్థిర స్థిరరాశి char16_t *trimWhiteSpace(స్థిరరాశి char16_t *s, int32_t &నిడివి);

    /**
     * Tests whether the string contains a "pattern identifier", that is,
     * whether it contains only non-Pattern_White_Space, non-Pattern_Syntax characters.
     * @return true if there are no Pattern_White_Space or Pattern_Syntax characters in s.
     */
    స్థిర UBool isIdentifier(స్థిరరాశి char16_t *s, int32_t నిడివి);

    /**
     * Skips over a "pattern identifier" starting at index s.
     * @return The smallest pointer at or after s with
     *         a Pattern_White_Space or Pattern_Syntax character.
     */
    స్థిర స్థిరరాశి char16_t *skipIdentifier(స్థిరరాశి char16_t *s, int32_t నిడివి);

అంతరంగ:
    నమూనా_Props() = delete;  // no constructor: all static methods
};

U_NAMESPACE_చివర

#అయితే_ముగింపు  // __PATTERNPROPS_H__
