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

#समावेश "unicode/uchar.h"
#समावेश "unicode/utf16.h"
#समावेश "patternprops.h"
#समावेश "util.h"

U_NAMESPACE_BEGIN

/**
 * Parse an integer at pos, either of the form \d+ or of the form
 * 0x[0-9A-Fa-f]+ or 0[0-7]+, that is, in standard decimal, hex,
 * or octal format.
 * @param pos INPUT-OUTPUT parameter.  On input, the first
 * character to parse.  On output, the character after the last
 * parsed character.
 */
int32_t ICU_Utility::parseInteger(स्थिर UnicodeString& rule, int32_t& pos, int32_t limit) {
    int32_t count = 0;
    int32_t value = 0;
    int32_t p = pos;
    int8_t radix = 10;

    अगर (p < limit && rule.charAt(p) == 48 /*0*/) {
        अगर (p+1 < limit && (rule.charAt(p+1) == 0x78 /*x*/ || rule.charAt(p+1) == 0x58 /*X*/)) {
            p += 2;
            radix = 16;
        }
        अन्यथा {
            p++;
            count = 1;
            radix = 8;
        }
    }

    जबतक (p < limit) {
        int32_t d = u_digit(rule.charAt(p++), radix);
        अगर (d < 0) {
            --p;
            अवरोध;
        }
        ++count;
        int32_t v = (value * radix) + d;
        अगर (v <= value) {
            // If there are too many input digits, at some point
            // the value will go negative, e.g., if we have seen
            // "0x8000000" already and there is another '0', when
            // we parse the next 0 the value will go negative.
            वापस 0;
        }
        value = v;
    }
    अगर (count > 0) {
        pos = p;
    }
    वापस value;
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
int32_t ICU_Utility::parsePattern(स्थिर UnicodeString& rule, int32_t pos, int32_t limit,
                              स्थिर UnicodeString& pattern, int32_t* parsedInts) {
    // TODO Update this to handle surrogates
    int32_t p;
    int32_t int_गिनती = 0; // number of integers parsed
    क्रम (int32_t i=0; i<pattern.length(); ++i) {
        char16_t cpat = pattern.charAt(i);
        char16_t c;
        चयन (cpat) {
        हाल 32 /*' '*/:
            अगर (pos >= limit) {
                वापस -1;
            }
            c = rule.charAt(pos++);
            अगर (!PatternProps::isWhiteSpace(c)) {
                वापस -1;
            }
            // FALL THROUGH to skipWhitespace
            U_FALLTHROUGH;
        हाल 126 /*'~'*/:
            pos = skipWhitespace(rule, pos);
            अवरोध;
        हाल 35 /*'#'*/:
            p = pos;
            parsedInts[int_गिनती++] = parseInteger(rule, p, limit);
            अगर (p == pos) {
                // Syntax error; failed to parse integer
                वापस -1;
            }
            pos = p;
            अवरोध;
        शेष:
            अगर (pos >= limit) {
                वापस -1;
            }
            c = static_cast<char16_t>(u_tolower(rule.charAt(pos++)));
            अगर (c != cpat) {
                वापस -1;
            }
            अवरोध;
        }
    }
    वापस pos;
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
UnicodeString ICU_Utility::parseUnicodeIdentifier(स्थिर UnicodeString& str, int32_t& pos) {
    // assert(pos < str.length());
    UnicodeString buf;
    पूर्णांक p = pos;
    जबतक (p < str.length()) {
        UChar32 ch = str.char32At(p);
        अगर (buf.length() == 0) {
            अगर (u_isIDStart(ch)) {
                buf.append(ch);
            } अन्यथा {
                buf.truncate(0);
                वापस buf;
            }
        } अन्यथा {
            अगर (u_isIDPart(ch)) {
                buf.append(ch);
            } अन्यथा {
                अवरोध;
            }
        }
        p += U16_लंबाई(ch);
    }
    pos = p;
    वापस buf;
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
int32_t ICU_Utility::parse_संख्या(स्थिर UnicodeString& text,
                                 int32_t& pos, int8_t radix) {
    // assert(pos[0] >= 0);
    // assert(radix >= 2);
    // assert(radix <= 36);
    int32_t n = 0;
    int32_t p = pos;
    जबतक (p < text.length()) {
        UChar32 ch = text.char32At(p);
        int32_t d = u_digit(ch, radix);
        अगर (d < 0) {
            अवरोध;
        }
        int64_t update = radix*static_cast<int64_t>(n) + d;
        अगर (update > INT32_MAX) {
            वापस -1;
        }
        n = static_cast<int32_t>(update);
        ++p;
    }
    अगर (p == pos) {
        वापस -1;
    }
    pos = p;
    वापस n;
}

U_NAMESPACE_END

