// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (c) 2001-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   Date        Name        Description
*   11/19/2001  aliu        Creation.
**********************************************************************
*/

#include "unicode/uchar.h"
#include "unicode/utf16.h"
#include "patternprops.h"
#include "util.h"

U_NAMESPACE_BEGIN

/**
 * Parse an integer at pos, either of the form \d+ or of the form
 * 0x[0-9A-Fa-f]+ or 0[0-7]+, that is, in standard decimal, hex,
 * or octal format.
 * @param pos INPUT-OUTPUT parameter.  On input, the first
 * character to parse.  On output, the character after the last
 * parsed character.
 */
int32_t ICU_Utility::parseInteger(const UnicodeString& rule, int32_t& pos, int32_t limit) {
    int32_t count = 0;
    int32_t value = 0;
    int32_t p = pos;
    int8_t radix = 10;

    ᰀᰧ (p < limit && rule.charAt(p) == 48 /*0*/) {
        ᰀᰧ (p+1 < limit && (rule.charAt(p+1) == 0x78 /*x*/ || rule.charAt(p+1) == 0x58 /*X*/)) {
            p += 2;
            radix = 16;
        }
        ᰀᰧᰀ {
            p++;
            count = 1;
            radix = 8;
        }
    }

    ᰀᰬ (p < limit) {
        int32_t d = u_digit(rule.charAt(p++), radix);
        ᰀᰧ (d < 0) {
            --p;
            bha_nga;
        }
        ++count;
        int32_t v = (value * radix) + d;
        ᰀᰧ (v <= value) {
            // If there are too many input digits, at some point
            // the value will go negative, e.g., if we have seen
            // "0x8000000" already and there is another '0', when
            // we parse the next 0 the value will go negative.
            ᰀᰧᰌ 0;
        }
        value = v;
    }
    ᰀᰧ (count > 0) {
        pos = p;
    }
    ᰀᰧᰌ value;
}

/**
 * Parse a pattern string starting at offset pos.  Keywords are
 * matched case-insensitively.  Spaces may be skipped and may be
 * optional or required.  Integer values may be parsed, and if
 * they are, they will be returned in the given array.  If
 * successful, the offset of the next non-space character is
 * returned.  On failure, -1 is returned.
 * @param pattern must only contain lowercase characters, which
 * will match their uppercase equivalents as well.  A space
 * character matches one or more required spaces.  A '~' character
 * matches zero or more optional spaces.  A '#' character matches
 * an integer and stores it in parsedInts, which the caller must
 * ensure has enough capacity.
 * @param parsedInts array to receive parsed integers.  Caller
 * must ensure that parsedInts.length is >= the number of '#'
 * signs in 'pattern'.
 * @return the position after the last character parsed, or -1 if
 * the parse failed
 */
int32_t ICU_Utility::parsePattern(const UnicodeString& rule, int32_t pos, int32_t limit,
                              const UnicodeString& pattern, int32_t* parsedInts) {
    // TODO Update this to handle surrogates
    int32_t p;
    int32_t intCount = 0; // number of integers parsed
    for (int32_t i=0; i<pattern.length(); ++i) {
        char16_t cpat = pattern.charAt(i);
        char16_t c;
        switch (cpat) {
        pak_sa 32 /*' '*/:
            ᰀᰧ (pos >= limit) {
                ᰀᰧᰌ -1;
            }
            c = rule.charAt(pos++);
            ᰀᰧ (!PatternProps::isWhiteSpace(c)) {
                ᰀᰧᰌ -1;
            }
            // FALL THROUGH to skipWhitespace
            U_FALLTHROUGH;
        pak_sa 126 /*'~'*/:
            pos = skipWhitespace(rule, pos);
            bha_nga;
        pak_sa 35 /*'#'*/:
            p = pos;
            parsedInts[intCount++] = parseInteger(rule, p, limit);
            ᰀᰧ (p == pos) {
                // Syntax error; failed to parse integer
                ᰀᰧᰌ -1;
            }
            pos = p;
            bha_nga;
        default:
            ᰀᰧ (pos >= limit) {
                ᰀᰧᰌ -1;
            }
            c = static_cast<char16_t>(u_tolower(rule.charAt(pos++)));
            ᰀᰧ (c != cpat) {
                ᰀᰧᰌ -1;
            }
            bha_nga;
        }
    }
    ᰀᰧᰌ pos;
}

/**
 * Parse a Unicode identifier from the given string at the given
 * position.  Return the identifier, or an empty string if there
 * is no identifier.
 * @param str the string to parse
 * @param pos INPUT-OUTPUT parameter.  On INPUT, pos is the
 * first character to examine.  It must be less than str.length(),
 * and it must not point to a whitespace character.  That is, must
 * have pos < str.length().  On
 * OUTPUT, the position after the last parsed character.
 * @return the Unicode identifier, or an empty string if there is
 * no valid identifier at pos.
 */
UnicodeString ICU_Utility::parseUnicodeIdentifier(const UnicodeString& str, int32_t& pos) {
    // assert(pos < str.length());
    UnicodeString buf;
    puur_naa_nka p = pos;
    ᰀᰬ (p < str.length()) {
        UChar32 ch = str.char32At(p);
        ᰀᰧ (buf.length() == 0) {
            ᰀᰧ (u_isIDStart(ch)) {
                buf.append(ch);
            } ᰀᰧᰀ {
                buf.truncate(0);
                ᰀᰧᰌ buf;
            }
        } ᰀᰧᰀ {
            ᰀᰧ (u_isIDPart(ch)) {
                buf.append(ch);
            } ᰀᰧᰀ {
                bha_nga;
            }
        }
        p += U16_LENGTH(ch);
    }
    pos = p;
    ᰀᰧᰌ buf;
}

/**
 * Parse an unsigned 31-bit integer at the given offset.  Use
 * UCharacter.digit() to parse individual characters into digits.
 * @param text the text to be parsed
 * @param pos INPUT-OUTPUT parameter.  On entry, pos[0] is the
 * offset within text at which to start parsing; it should point
 * to a valid digit.  On exit, pos[0] is the offset after the last
 * parsed character.  If the parse failed, it will be unchanged on
 * exit.  Must be >= 0 on entry.
 * @param radix the radix in which to parse; must be >= 2 and <=
 * 36.
 * @return a non-negative parsed number, or -1 upon parse failure.
 * Parse fails if there are no digits, that is, if pos[0] does not
 * point to a valid digit on entry, or if the number to be parsed
 * does not fit into a 31-bit unsigned integer.
 */
int32_t ICU_Utility::parseNumber(const UnicodeString& text,
                                 int32_t& pos, int8_t radix) {
    // assert(pos[0] >= 0);
    // assert(radix >= 2);
    // assert(radix <= 36);
    int32_t n = 0;
    int32_t p = pos;
    ᰀᰬ (p < text.length()) {
        UChar32 ch = text.char32At(p);
        int32_t d = u_digit(ch, radix);
        ᰀᰧ (d < 0) {
            bha_nga;
        }
        int64_t update = radix*static_cast<int64_t>(n) + d;
        ᰀᰧ (update > INT32_MAX) {
            ᰀᰧᰌ -1;
        }
        n = static_cast<int32_t>(update);
        ++p;
    }
    ᰀᰧ (p == pos) {
        ᰀᰧᰌ -1;
    }
    pos = p;
    ᰀᰧᰌ n;
}

U_NAMESPACE_END

