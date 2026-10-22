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

#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "patternprops.h"
#अन्तर्भाव "util.h"

U_NAMESPACE_BEGIN

/**
 * Parse an integer at pos, either of the form \d+ or of the form
 * 0x[0-9A-Fa-f]+ or 0[0-7]+, that is, in standard decimal, hex,
 * or octal format.
 * @param pos INPUT-OUTPUT parameter.  On input, the first
 * character to parse.  On output, the character after the last
 * parsed character.
 */
int32_t ICU_Utility::parseInteger(ध्रुव UnicodeString& नियमः, int32_t& pos, int32_t सीमा) {
    int32_t count = 0;
    int32_t value = 0;
    int32_t p = pos;
    int8_t radix = 10;

    यदि (p < सीमा && नियमः.charAt(p) == 48 /*0*/) {
        यदि (p+1 < सीमा && (नियमः.charAt(p+1) == 0x78 /*x*/ || नियमः.charAt(p+1) == 0x58 /*X*/)) {
            p += 2;
            radix = 16;
        }
        अन्यथा {
            p++;
            count = 1;
            radix = 8;
        }
    }

    यावत् (p < सीमा) {
        int32_t d = u_digit(नियमः.charAt(p++), radix);
        यदि (d < 0) {
            --p;
            भङ्ग;
        }
        ++count;
        int32_t v = (value * radix) + d;
        यदि (v <= value) {
            // If there are too many input digits, at some point
            // the value will go negative, e.g., if we have seen
            // "0x8000000" already and there is another '0', when
            // we parse the next 0 the value will go negative.
            प्रत्यागम 0;
        }
        value = v;
    }
    यदि (count > 0) {
        pos = p;
    }
    प्रत्यागम value;
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
int32_t ICU_Utility::parse_आदर्शः(ध्रुव UnicodeString& नियमः, int32_t pos, int32_t सीमा,
                              ध्रुव UnicodeString& आदर्शः, int32_t* parsedInts) {
    // TODO Update this to handle surrogates
    int32_t p;
    int32_t int_गणनम् = 0; // number of integers parsed
    प्रति (int32_t i=0; i<आदर्शः.length(); ++i) {
        char16_t cpat = आदर्शः.charAt(i);
        char16_t c;
        विकल्प (cpat) {
        पक्ष 32 /*' '*/:
            यदि (pos >= सीमा) {
                प्रत्यागम -1;
            }
            c = नियमः.charAt(pos++);
            यदि (!आदर्शः_Props::isWhiteSpace(c)) {
                प्रत्यागम -1;
            }
            // FALL THROUGH to skipWhitespace
            U_FALLTHROUGH;
        पक्ष 126 /*'~'*/:
            pos = skipWhitespace(नियमः, pos);
            भङ्ग;
        पक्ष 35 /*'#'*/:
            p = pos;
            parsedInts[int_गणनम्++] = parseInteger(नियमः, p, सीमा);
            यदि (p == pos) {
                // Syntax error; failed to parse integer
                प्रत्यागम -1;
            }
            pos = p;
            भङ्ग;
        स्वभाव:
            यदि (pos >= सीमा) {
                प्रत्यागम -1;
            }
            c = static_cast<char16_t>(u_tolower(नियमः.charAt(pos++)));
            यदि (c != cpat) {
                प्रत्यागम -1;
            }
            भङ्ग;
        }
    }
    प्रत्यागम pos;
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
UnicodeString ICU_Utility::parseUnicodeIdentifier(ध्रुव UnicodeString& str, int32_t& pos) {
    // assert(pos < str.length());
    UnicodeString buf;
    पूर्णाङ्क p = pos;
    यावत् (p < str.length()) {
        UChar32 ch = str.char32At(p);
        यदि (buf.length() == 0) {
            यदि (u_isIDStart(ch)) {
                buf.append(ch);
            } अन्यथा {
                buf.truncate(0);
                प्रत्यागम buf;
            }
        } अन्यथा {
            यदि (u_isIDPart(ch)) {
                buf.append(ch);
            } अन्यथा {
                भङ्ग;
            }
        }
        p += U16_दीर्घता(ch);
    }
    pos = p;
    प्रत्यागम buf;
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
int32_t ICU_Utility::parse_सङ्ख्या(ध्रुव UnicodeString& text,
                                 int32_t& pos, int8_t radix) {
    // assert(pos[0] >= 0);
    // assert(radix >= 2);
    // assert(radix <= 36);
    int32_t n = 0;
    int32_t p = pos;
    यावत् (p < text.length()) {
        UChar32 ch = text.char32At(p);
        int32_t d = u_digit(ch, radix);
        यदि (d < 0) {
            भङ्ग;
        }
        int64_t update = radix*static_cast<int64_t>(n) + d;
        यदि (update > INT32_MAX) {
            प्रत्यागम -1;
        }
        n = static_cast<int32_t>(update);
        ++p;
    }
    यदि (p == pos) {
        प्रत्यागम -1;
    }
    pos = p;
    प्रत्यागम n;
}

U_NAMESPACE_अन्तः

