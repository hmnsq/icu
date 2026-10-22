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

#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "patternprops.h"
#ಅನ್ತರ್ಭಾವ "util.h"

U_NAMESPACE_BEGIN

/**
 * Parse an integer at pos, either of the form \d+ or of the form
 * 0x[0-9A-Fa-f]+ or 0[0-7]+, that is, in standard decimal, hex,
 * or octal format.
 * @param pos INPUT-OUTPUT parameter.  On input, the first
 * character to parse.  On output, the character after the last
 * parsed character.
 */
int32_t ICU_Utility::parseInteger(ಸ್ಥಿರಾಂಕ UnicodeString& ನಿಯಮ, int32_t& pos, int32_t ಎಲ್ಲೆ) {
    int32_t count = 0;
    int32_t value = 0;
    int32_t p = pos;
    int8_t radix = 10;

    ಆದರೆ (p < ಎಲ್ಲೆ && ನಿಯಮ.charAt(p) == 48 /*0*/) {
        ಆದರೆ (p+1 < ಎಲ್ಲೆ && (ನಿಯಮ.charAt(p+1) == 0x78 /*x*/ || ನಿಯಮ.charAt(p+1) == 0x58 /*X*/)) {
            p += 2;
            radix = 16;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            p++;
            count = 1;
            radix = 8;
        }
    }

    ಆಗ (p < ಎಲ್ಲೆ) {
        int32_t d = u_digit(ನಿಯಮ.charAt(p++), radix);
        ಆದರೆ (d < 0) {
            --p;
            ನಿಲ್ಲಿಸು;
        }
        ++count;
        int32_t v = (value * radix) + d;
        ಆದರೆ (v <= value) {
            // If there are too many input digits, at some point
            // the value will go negative, e.g., if we have seen
            // "0x8000000" already and there is another '0', when
            // we parse the next 0 the value will go negative.
            ಹಿಂತಿರುಗಿ 0;
        }
        value = v;
    }
    ಆದರೆ (count > 0) {
        pos = p;
    }
    ಹಿಂತಿರುಗಿ value;
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
int32_t ICU_Utility::parse_ನಮೂನೆ(ಸ್ಥಿರಾಂಕ UnicodeString& ನಿಯಮ, int32_t pos, int32_t ಎಲ್ಲೆ,
                              ಸ್ಥಿರಾಂಕ UnicodeString& ನಮೂನೆ, int32_t* parsedInts) {
    // TODO Update this to handle surrogates
    int32_t p;
    int32_t int_ಎಣಿಕೆ = 0; // number of integers parsed
    ಗಾಗಿ (int32_t i=0; i<ನಮೂನೆ.length(); ++i) {
        char16_t cpat = ನಮೂನೆ.charAt(i);
        char16_t c;
        ಆಯ್ಕೆ (cpat) {
        ಸ್ಥಿತಿ 32 /*' '*/:
            ಆದರೆ (pos >= ಎಲ್ಲೆ) {
                ಹಿಂತಿರುಗಿ -1;
            }
            c = ನಿಯಮ.charAt(pos++);
            ಆದರೆ (!ನಮೂನೆ_Props::isWhiteSpace(c)) {
                ಹಿಂತಿರುಗಿ -1;
            }
            // FALL THROUGH to skipWhitespace
            U_FALLTHROUGH;
        ಸ್ಥಿತಿ 126 /*'~'*/:
            pos = skipWhitespace(ನಿಯಮ, pos);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 35 /*'#'*/:
            p = pos;
            parsedInts[int_ಎಣಿಕೆ++] = parseInteger(ನಿಯಮ, p, ಎಲ್ಲೆ);
            ಆದರೆ (p == pos) {
                // Syntax error; failed to parse integer
                ಹಿಂತಿರುಗಿ -1;
            }
            pos = p;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಆದರೆ (pos >= ಎಲ್ಲೆ) {
                ಹಿಂತಿರುಗಿ -1;
            }
            c = static_cast<char16_t>(u_tolower(ನಿಯಮ.charAt(pos++)));
            ಆದರೆ (c != cpat) {
                ಹಿಂತಿರುಗಿ -1;
            }
            ನಿಲ್ಲಿಸು;
        }
    }
    ಹಿಂತಿರುಗಿ pos;
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
UnicodeString ICU_Utility::parseUnicodeIdentifier(ಸ್ಥಿರಾಂಕ UnicodeString& str, int32_t& pos) {
    // assert(pos < str.length());
    UnicodeString buf;
    ಪೂರ್ಣಾಂಕ p = pos;
    ಆಗ (p < str.length()) {
        UChar32 ch = str.char32At(p);
        ಆದರೆ (buf.length() == 0) {
            ಆದರೆ (u_isIDStart(ch)) {
                buf.append(ch);
            } ಇಲ್ಲದಿದ್ದರೆ {
                buf.truncate(0);
                ಹಿಂತಿರುಗಿ buf;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (u_isIDPart(ch)) {
                buf.append(ch);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ನಿಲ್ಲಿಸು;
            }
        }
        p += U16_ಉದ್ದಳತೆ(ch);
    }
    pos = p;
    ಹಿಂತಿರುಗಿ buf;
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
int32_t ICU_Utility::parse_ಸಂಖ್ಯೆ(ಸ್ಥಿರಾಂಕ UnicodeString& ಪಠ್ಯ,
                                 int32_t& pos, int8_t radix) {
    // assert(pos[0] >= 0);
    // assert(radix >= 2);
    // assert(radix <= 36);
    int32_t n = 0;
    int32_t p = pos;
    ಆಗ (p < ಪಠ್ಯ.length()) {
        UChar32 ch = ಪಠ್ಯ.char32At(p);
        int32_t d = u_digit(ch, radix);
        ಆದರೆ (d < 0) {
            ನಿಲ್ಲಿಸು;
        }
        int64_t update = radix*static_cast<int64_t>(n) + d;
        ಆದರೆ (update > INT32_MAX) {
            ಹಿಂತಿರುಗಿ -1;
        }
        n = static_cast<int32_t>(update);
        ++p;
    }
    ಆದರೆ (p == pos) {
        ಹಿಂತಿರುಗಿ -1;
    }
    pos = p;
    ಹಿಂತಿರುಗಿ n;
}

U_NAMESPACE_ಕೊನೆ

