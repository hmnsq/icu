// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (c) 2001-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   Date        Name        Description
*   11/19/2001  aliu        Creation.
**********************************************************************
*/

#ಅನ್ತರ್ಭಾವ "unicode/unimatch.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "patternprops.h"
#ಅನ್ತರ್ಭಾವ "util.h"

// Define char16_t constants using hex for EBCDIC compatibility

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t BACKSLASH  = 0x005C; /*\*/
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t ಊರ್ಧ್ವ_U    = 0x0055; /*U*/
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t ಅಧೋ_U    = 0x0075; /*u*/
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t APOSTROPHE = 0x0027; // '\''
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t SPACE      = 0x0020; // ' '

// "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t DIGITS[] = {
    48,49,50,51,52,53,54,55,56,57,
    65,66,67,68,69,70,71,72,73,74,
    75,76,77,78,79,80,81,82,83,84,
    85,86,87,88,89,90
};

U_NAMESPACE_BEGIN

UnicodeString& ICU_Utility::append_ಸಂಖ್ಯೆ(UnicodeString& ಫಲಿತಾಂಶ, int32_t n,
                                     int32_t radix, int32_t minDigits) {
    ಆದರೆ (radix < 2 || radix > 36) {
        // Bogus radix
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.append(static_cast<char16_t>(63)/*?*/);
    }
    // Handle negatives
    ಆದರೆ (n < 0) {
        n = -n;
        ಫಲಿತಾಂಶ.append(static_cast<char16_t>(45)/*-*/);
    }
    // First determine the number of digits
    int32_t nn = n;
    int32_t r = 1;
    ಆಗ (nn >= radix) {
        nn /= radix;
        r *= radix;
        --minDigits;
    }
    // Now generate the digits
    ಆಗ (--minDigits > 0) {
        ಫಲಿತಾಂಶ.append(DIGITS[0]);
    }
    ಆಗ (r > 0) {
        int32_t digit = n / r;
        ಫಲಿತಾಂಶ.append(DIGITS[digit]);
        n -= digit * r;
        r /= radix;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UBool ICU_Utility::isUnprintable(UChar32 c) {
    ಹಿಂತಿರುಗಿ !(c >= 0x20 && c <= 0x7E);
}

UBool ICU_Utility::shouldAlwaysBeEscaped(UChar32 c) {
    ಆದರೆ (c < 0x20) {
        ಹಿಂತಿರುಗಿ ನಿಜ;  // C0 control codes
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c <= 0x7e) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;  // printable ASCII
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c <= 0x9f) {
        ಹಿಂತಿರುಗಿ ನಿಜ;  // C1 control codes
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c < 0xd800) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;  // most of the BMP
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c <= 0xdfff || (0xfdd0 <= c && c <= 0xfdef) || (c & 0xfffe) == 0xfffe) {
        ಹಿಂತಿರುಗಿ ನಿಜ;  // surrogate or noncharacter code points
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c <= 0x10ffff) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;  // all else
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ನಿಜ;  // not a code point
    }
}

UBool ICU_Utility::escapeUnprintable(UnicodeString& ಫಲಿತಾಂಶ, UChar32 c) {
    ಆದರೆ (isUnprintable(c)) {
        escape(ಫಲಿತಾಂಶ, c);
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

UnicodeString &ICU_Utility::escape(UnicodeString& ಫಲಿತಾಂಶ, UChar32 c) {
    ಫಲಿತಾಂಶ.append(BACKSLASH);
    ಆದರೆ (c & ~0xFFFF) {
        ಫಲಿತಾಂಶ.append(ಊರ್ಧ್ವ_U);
        ಫಲಿತಾಂಶ.append(DIGITS[0xF&(c>>28)]);
        ಫಲಿತಾಂಶ.append(DIGITS[0xF&(c>>24)]);
        ಫಲಿತಾಂಶ.append(DIGITS[0xF&(c>>20)]);
        ಫಲಿತಾಂಶ.append(DIGITS[0xF&(c>>16)]);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ.append(ಅಧೋ_U);
    }
    ಫಲಿತಾಂಶ.append(DIGITS[0xF&(c>>12)]);
    ಫಲಿತಾಂಶ.append(DIGITS[0xF&(c>>8)]);
    ಫಲಿತಾಂಶ.append(DIGITS[0xF&(c>>4)]);
    ಫಲಿತಾಂಶ.append(DIGITS[0xF&c]);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/**
 * Returns the index of a character, ignoring quoted text.
 * For example, in the string "abc'hide'h", the 'h' in "hide" will not be
 * found by a search for 'h'.
 */
// FOR FUTURE USE.  DISABLE FOR NOW for coverage reasons.
/*
int32_t ICU_Utility::quotedIndexOf(const UnicodeString& text,
                               int32_t start, int32_t limit,
                               char16_t charToFind) {
    for (int32_t i=start; i<limit; ++i) {
        char16_t c = text.charAt(i);
        if (c == BACKSLASH) {
            ++i;
        } else if (c == APOSTROPHE) {
            while (++i < limit
                   && text.charAt(i) != APOSTROPHE) {}
        } else if (c == charToFind) {
            return i;
        }
    }
    return -1;
}
*/

/**
 * Skip over a sequence of zero or more white space characters at pos.
 * @param advance if true, advance pos to the first non-white-space
 * character at or after pos, or str.length(), if there is none.
 * Otherwise leave pos unchanged.
 * @return the index of the first non-white-space character at or
 * after pos, or str.length(), if there is none.
 */
int32_t ICU_Utility::skipWhitespace(ಸ್ಥಿರಾಂಕ UnicodeString& str, int32_t& pos,
                                    UBool advance) {
    int32_t p = pos;
    ಸ್ಥಿರಾಂಕ char16_t* s = str.get_ಬಫರ್();
    p = static_cast<int32_t>(ನಮೂನೆ_Props::skipWhiteSpace(s + p, str.length() - p) - s);
    ಆದರೆ (advance) {
        pos = p;
    }
    ಹಿಂತಿರುಗಿ p;
}

/**
 * Skip over Pattern_White_Space in a Replaceable.
 * Skipping may be done in the forward or
 * reverse direction.  In either case, the leftmost index will be
 * inclusive, and the rightmost index will be exclusive.  That is,
 * given a range defined as [start, limit), the call
 * skipWhitespace(text, start, limit) will advance start past leading
 * whitespace, whereas the call skipWhitespace(text, limit, start),
 * will back up limit past trailing whitespace.
 * @param text the text to be analyzed
 * @param pos either the start or limit of a range of 'text', to skip
 * leading or trailing whitespace, respectively
 * @param stop either the limit or start of a range of 'text', to skip
 * leading or trailing whitespace, respectively
 * @return the new start or limit, depending on what was passed in to
 * 'pos'
 */
//?FOR FUTURE USE.  DISABLE FOR NOW for coverage reasons.
//?int32_t ICU_Utility::skipWhitespace(const Replaceable& text,
//?                                    int32_t pos, int32_t stop) {
//?    UChar32 c;
//?    UBool isForward = (stop >= pos);
//?
//?    if (!isForward) {
//?        --pos; // pos is a limit, so back up by one
//?    }
//?    
//?    while (pos != stop &&
//?           PatternProps::isWhiteSpace(c = text.char32At(pos))) {
//?        if (isForward) {
//?            pos += U16_LENGTH(c);
//?        } else {
//?            pos -= U16_LENGTH(c);
//?        }
//?    }
//?
//?    if (!isForward) {
//?        ++pos; // make pos back into a limit
//?    }
//?
//?    return pos;
//?}

/**
 * Parse a single non-whitespace character 'ch', optionally
 * preceded by whitespace.
 * @param id the string to be parsed
 * @param pos INPUT-OUTPUT parameter.  On input, pos[0] is the
 * offset of the first character to be parsed.  On output, pos[0]
 * is the index after the last parsed character.  If the parse
 * fails, pos[0] will be unchanged.
 * @param ch the non-whitespace character to be parsed.
 * @return true if 'ch' is seen preceded by zero or more
 * whitespace characters.
 */
UBool ICU_Utility::parseChar(ಸ್ಥಿರಾಂಕ UnicodeString& id, int32_t& pos, char16_t ch) {
    int32_t ಆರಂಭ = pos;
    skipWhitespace(id, pos, ನಿಜ);
    ಆದರೆ (pos == id.length() ||
        id.charAt(pos) != ch) {
        pos = ಆರಂಭ;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ++pos;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/**
 * Parse a pattern string within the given Replaceable and a parsing
 * pattern.  Characters are matched literally and case-sensitively
 * except for the following special characters:
 *
 * ~  zero or more Pattern_White_Space chars
 *
 * If end of pattern is reached with all matches along the way,
 * pos is advanced to the first unparsed index and returned.
 * Otherwise -1 is returned.
 * @param pat pattern that controls parsing
 * @param text text to be parsed, starting at index
 * @param index offset to first character to parse
 * @param limit offset after last character to parse
 * @return index after last parsed character, or -1 on parse failure.
 */
int32_t ICU_Utility::parse_ನಮೂನೆ(ಸ್ಥಿರಾಂಕ UnicodeString& pat,
                                  ಸ್ಥಿರಾಂಕ Replaceable& ಪಠ್ಯ,
                                  int32_t ಸೂಚ್ಯಂಕ,
                                  int32_t ಎಲ್ಲೆ) {
    int32_t ipat = 0;

    // empty pattern matches immediately
    ಆದರೆ (ipat == pat.length()) {
        ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
    }

    UChar32 cpat = pat.char32At(ipat);

    ಆಗ (ಸೂಚ್ಯಂಕ < ಎಲ್ಲೆ) {
        UChar32 c = ಪಠ್ಯ.char32At(ಸೂಚ್ಯಂಕ);

        // parse \s*
        ಆದರೆ (cpat == 126 /*~*/) {
            ಆದರೆ (ನಮೂನೆ_Props::isWhiteSpace(c)) {
                ಸೂಚ್ಯಂಕ += U16_ಉದ್ದಳತೆ(c);
                ಮುಂದುವರಿ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (++ipat == pat.length()) {
                    ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ; // success; c unparsed
                }
                // fall thru; process c again with next cpat
            }
        }

        // parse literal
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == cpat) {
            ಸೂಚ್ಯಂಕ += U16_ಉದ್ದಳತೆ(c);
            ipat += U16_ಉದ್ದಳತೆ(cpat);
            ಆದರೆ (ipat == pat.length()) {
                ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ; // success; c parsed
            }
            // fall thru; get next cpat
        }

        // match failure of literal
        ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ -1;
        }

        cpat = pat.char32At(ipat);
    }

    ಹಿಂತಿರುಗಿ -1; // text ended before end of pat
}

int32_t ICU_Utility::parseAsciiInteger(ಸ್ಥಿರಾಂಕ UnicodeString& str, int32_t& pos) {
    int32_t ಫಲಿತಾಂಶ = 0;
    char16_t c;
    ಆಗ (pos < str.length() && (c = str.charAt(pos)) >= u'0' && c <= u'9') {
        ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ * 10 + (c - u'0');
        pos++;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/**
 * Append a character to a rule that is being built up.  To flush
 * the quoteBuf to rule, make one final call with isLiteral == true.
 * If there is no final character, pass in (UChar32)-1 as c.
 * @param rule the string to append the character to
 * @param c the character to append, or (UChar32)-1 if none.
 * @param isLiteral if true, then the given character should not be
 * quoted or escaped.  Usually this means it is a syntactic element
 * such as > or $
 * @param escapeUnprintable if true, then unprintable characters
 * should be escaped using \uxxxx or \Uxxxxxxxx.  These escapes will
 * appear outside of quotes.
 * @param quoteBuf a buffer which is used to build up quoted
 * substrings.  The caller should initially supply an empty buffer,
 * and thereafter should not modify the buffer.  The buffer should be
 * cleared out by, at the end, calling this method with a literal
 * character.
 */
ಶೂನ್ಯ ICU_Utility::append_ಗಮ್ಯ_ನಿಯಮ(UnicodeString& ನಿಯಮ,
                               UChar32 c,
                               UBool isLiteral,
                               UBool escapeUnprintable,
                               UnicodeString& quoteBuf) {
    // If we are escaping unprintables, then escape them outside
    // quotes.  \u and \U are not recognized within quotes.  The same
    // logic applies to literals, but literals are never escaped.
    ಆದರೆ (isLiteral ||
        (escapeUnprintable && ICU_Utility::isUnprintable(c))) {
        ಆದರೆ (quoteBuf.length() > 0) {
            // We prefer backslash APOSTROPHE to double APOSTROPHE
            // (more readable, less similar to ") so if there are
            // double APOSTROPHEs at the ends, we pull them outside
            // of the quote.

            // If the first thing in the quoteBuf is APOSTROPHE
            // (doubled) then pull it out.
            ಆಗ (quoteBuf.length() >= 2 &&
                   quoteBuf.charAt(0) == APOSTROPHE &&
                   quoteBuf.charAt(1) == APOSTROPHE) {
                ನಿಯಮ.append(BACKSLASH).append(APOSTROPHE);
                quoteBuf.remove(0, 2);
            }
            // If the last thing in the quoteBuf is APOSTROPHE
            // (doubled) then remove and count it and add it after.
            int32_t trailing_ಎಣಿಕೆ = 0;
            ಆಗ (quoteBuf.length() >= 2 &&
                   quoteBuf.charAt(quoteBuf.length()-2) == APOSTROPHE &&
                   quoteBuf.charAt(quoteBuf.length()-1) == APOSTROPHE) {
                quoteBuf.truncate(quoteBuf.length()-2);
                ++trailing_ಎಣಿಕೆ;
            }
            ಆದರೆ (quoteBuf.length() > 0) {
                ನಿಯಮ.append(APOSTROPHE);
                ನಿಯಮ.append(quoteBuf);
                ನಿಯಮ.append(APOSTROPHE);
                quoteBuf.truncate(0);
            }
            ಆಗ (trailing_ಎಣಿಕೆ-- > 0) {
                ನಿಯಮ.append(BACKSLASH).append(APOSTROPHE);
            }
        }
        ಆದರೆ (c != static_cast<UChar32>(-1)) {
            /* Since spaces are ignored during parsing, they are
             * emitted only for readability.  We emit one here
             * only if there isn't already one at the end of the
             * rule.
             */
            ಆದರೆ (c == SPACE) {
                int32_t len = ನಿಯಮ.length();
                ಆದರೆ (len > 0 && ನಿಯಮ.charAt(len-1) != c) {
                    ನಿಯಮ.append(c);
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!escapeUnprintable || !ICU_Utility::escapeUnprintable(ನಿಯಮ, c)) {
                ನಿಯಮ.append(c);
            }
        }
    }

    // Escape ' and '\' and don't begin a quote just for them
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (quoteBuf.length() == 0 &&
             (c == APOSTROPHE || c == BACKSLASH)) {
        ನಿಯಮ.append(BACKSLASH);
        ನಿಯಮ.append(c);
    }

    // Specials (printable ascii that isn't [0-9a-zA-Z]) and
    // whitespace need quoting.  Also append stuff to quotes if we are
    // building up a quoted substring already.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (quoteBuf.length() > 0 ||
             (c >= 0x0021 && c <= 0x007E &&
              !((c >= 0x0030/*'0'*/ && c <= 0x0039/*'9'*/) ||
                (c >= 0x0041/*'A'*/ && c <= 0x005A/*'Z'*/) ||
                (c >= 0x0061/*'a'*/ && c <= 0x007A/*'z'*/))) ||
             ನಮೂನೆ_Props::isWhiteSpace(c)) {
        quoteBuf.append(c);
        // Double ' within a quote
        ಆದರೆ (c == APOSTROPHE) {
            quoteBuf.append(c);
        }
    }
    
    // Otherwise just append
    ಇಲ್ಲದಿದ್ದರೆ {
        ನಿಯಮ.append(c);
    }
}

ಶೂನ್ಯ ICU_Utility::append_ಗಮ್ಯ_ನಿಯಮ(UnicodeString& ನಿಯಮ,
                               ಸ್ಥಿರಾಂಕ UnicodeString& ಪಠ್ಯ,
                               UBool isLiteral,
                               UBool escapeUnprintable,
                               UnicodeString& quoteBuf) {
    ಗಾಗಿ (int32_t i=0; i<ಪಠ್ಯ.length(); ++i) {
        append_ಗಮ್ಯ_ನಿಯಮ(ನಿಯಮ, ಪಠ್ಯ[i], isLiteral, escapeUnprintable, quoteBuf);
    }
}

/**
 * Given a matcher reference, which may be null, append its
 * pattern as a literal to the given rule.
 */
ಶೂನ್ಯ ICU_Utility::append_ಗಮ್ಯ_ನಿಯಮ(UnicodeString& ನಿಯಮ,
                               ಸ್ಥಿರಾಂಕ UnicodeMatcher* matcher,
                               UBool escapeUnprintable,
                               UnicodeString& quoteBuf) {
    ಆದರೆ (matcher != ಶೂನ್ಯಸೂಚಕ) {
        UnicodeString pat;
        append_ಗಮ್ಯ_ನಿಯಮ(ನಿಯಮ, matcher->ಗಮ್ಯ_ನಮೂನೆ(pat, escapeUnprintable),
                     ನಿಜ, escapeUnprintable, quoteBuf);
    }
}

U_NAMESPACE_ಕೊನೆ
