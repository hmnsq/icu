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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unimatch.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "patternprops.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util.h"

// Define char16_t constants using hex for EBCDIC compatibility

ਸਥਿਰ ਅਚਲ char16_t BACKSLASH  = 0x005C; /*\*/
ਸਥਿਰ ਅਚਲ char16_t ਉੱਤਲਾ_U    = 0x0055; /*U*/
ਸਥਿਰ ਅਚਲ char16_t ਹੇਠਲਾ_U    = 0x0075; /*u*/
ਸਥਿਰ ਅਚਲ char16_t APOSTROPHE = 0x0027; // '\''
ਸਥਿਰ ਅਚਲ char16_t SPACE      = 0x0020; // ' '

// "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
ਸਥਿਰ ਅਚਲ char16_t DIGITS[] = {
    48,49,50,51,52,53,54,55,56,57,
    65,66,67,68,69,70,71,72,73,74,
    75,76,77,78,79,80,81,82,83,84,
    85,86,87,88,89,90
};

U_NAMESPACE_BEGIN

UnicodeString& ICU_Utility::append_ਸੰਖਿਆ(UnicodeString& ਨਤੀਜਾ, int32_t n,
                                     int32_t radix, int32_t minDigits) {
    ਜੇ (radix < 2 || radix > 36) {
        // Bogus radix
        ਵਾਪਸ ਨਤੀਜਾ.append(static_cast<char16_t>(63)/*?*/);
    }
    // Handle negatives
    ਜੇ (n < 0) {
        n = -n;
        ਨਤੀਜਾ.append(static_cast<char16_t>(45)/*-*/);
    }
    // First determine the number of digits
    int32_t nn = n;
    int32_t r = 1;
    ਜਦੋਂ (nn >= radix) {
        nn /= radix;
        r *= radix;
        --minDigits;
    }
    // Now generate the digits
    ਜਦੋਂ (--minDigits > 0) {
        ਨਤੀਜਾ.append(DIGITS[0]);
    }
    ਜਦੋਂ (r > 0) {
        int32_t digit = n / r;
        ਨਤੀਜਾ.append(DIGITS[digit]);
        n -= digit * r;
        r /= radix;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

UBool ICU_Utility::isUnprintable(UChar32 c) {
    ਵਾਪਸ !(c >= 0x20 && c <= 0x7E);
}

UBool ICU_Utility::shouldAlwaysBeEscaped(UChar32 c) {
    ਜੇ (c < 0x20) {
        ਵਾਪਸ ਸੱਚ;  // C0 control codes
    } ਨਹੀਂਤਾਂ ਜੇ (c <= 0x7e) {
        ਵਾਪਸ ਝੂਠ;  // printable ASCII
    } ਨਹੀਂਤਾਂ ਜੇ (c <= 0x9f) {
        ਵਾਪਸ ਸੱਚ;  // C1 control codes
    } ਨਹੀਂਤਾਂ ਜੇ (c < 0xd800) {
        ਵਾਪਸ ਝੂਠ;  // most of the BMP
    } ਨਹੀਂਤਾਂ ਜੇ (c <= 0xdfff || (0xfdd0 <= c && c <= 0xfdef) || (c & 0xfffe) == 0xfffe) {
        ਵਾਪਸ ਸੱਚ;  // surrogate or noncharacter code points
    } ਨਹੀਂਤਾਂ ਜੇ (c <= 0x10ffff) {
        ਵਾਪਸ ਝੂਠ;  // all else
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਸੱਚ;  // not a code point
    }
}

UBool ICU_Utility::escapeUnprintable(UnicodeString& ਨਤੀਜਾ, UChar32 c) {
    ਜੇ (isUnprintable(c)) {
        escape(ਨਤੀਜਾ, c);
        ਵਾਪਸ ਸੱਚ;
    }
    ਵਾਪਸ ਝੂਠ;
}

UnicodeString &ICU_Utility::escape(UnicodeString& ਨਤੀਜਾ, UChar32 c) {
    ਨਤੀਜਾ.append(BACKSLASH);
    ਜੇ (c & ~0xFFFF) {
        ਨਤੀਜਾ.append(ਉੱਤਲਾ_U);
        ਨਤੀਜਾ.append(DIGITS[0xF&(c>>28)]);
        ਨਤੀਜਾ.append(DIGITS[0xF&(c>>24)]);
        ਨਤੀਜਾ.append(DIGITS[0xF&(c>>20)]);
        ਨਤੀਜਾ.append(DIGITS[0xF&(c>>16)]);
    } ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ.append(ਹੇਠਲਾ_U);
    }
    ਨਤੀਜਾ.append(DIGITS[0xF&(c>>12)]);
    ਨਤੀਜਾ.append(DIGITS[0xF&(c>>8)]);
    ਨਤੀਜਾ.append(DIGITS[0xF&(c>>4)]);
    ਨਤੀਜਾ.append(DIGITS[0xF&c]);
    ਵਾਪਸ ਨਤੀਜਾ;
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
int32_t ICU_Utility::skipWhitespace(ਅਚਲ UnicodeString& str, int32_t& pos,
                                    UBool advance) {
    int32_t p = pos;
    ਅਚਲ char16_t* s = str.get_ਬਫਰ();
    p = static_cast<int32_t>(ਨਮੂਨਾ_Props::skipWhiteSpace(s + p, str.length() - p) - s);
    ਜੇ (advance) {
        pos = p;
    }
    ਵਾਪਸ p;
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
UBool ICU_Utility::parseChar(ਅਚਲ UnicodeString& id, int32_t& pos, char16_t ch) {
    int32_t ਸ਼ੁਰੂ = pos;
    skipWhitespace(id, pos, ਸੱਚ);
    ਜੇ (pos == id.length() ||
        id.charAt(pos) != ch) {
        pos = ਸ਼ੁਰੂ;
        ਵਾਪਸ ਝੂਠ;
    }
    ++pos;
    ਵਾਪਸ ਸੱਚ;
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
int32_t ICU_Utility::parse_ਨਮੂਨਾ(ਅਚਲ UnicodeString& pat,
                                  ਅਚਲ Replaceable& ਪਾਠ,
                                  int32_t ਸੂਚਕਾਂਕ,
                                  int32_t ਹੱਦਬੰਦੀ) {
    int32_t ipat = 0;

    // empty pattern matches immediately
    ਜੇ (ipat == pat.length()) {
        ਵਾਪਸ ਸੂਚਕਾਂਕ;
    }

    UChar32 cpat = pat.char32At(ipat);

    ਜਦੋਂ (ਸੂਚਕਾਂਕ < ਹੱਦਬੰਦੀ) {
        UChar32 c = ਪਾਠ.char32At(ਸੂਚਕਾਂਕ);

        // parse \s*
        ਜੇ (cpat == 126 /*~*/) {
            ਜੇ (ਨਮੂਨਾ_Props::isWhiteSpace(c)) {
                ਸੂਚਕਾਂਕ += U16_ਲੰਮਾਈ(c);
                ਜਾਰੀ;
            } ਨਹੀਂਤਾਂ {
                ਜੇ (++ipat == pat.length()) {
                    ਵਾਪਸ ਸੂਚਕਾਂਕ; // success; c unparsed
                }
                // fall thru; process c again with next cpat
            }
        }

        // parse literal
        ਨਹੀਂਤਾਂ ਜੇ (c == cpat) {
            ਸੂਚਕਾਂਕ += U16_ਲੰਮਾਈ(c);
            ipat += U16_ਲੰਮਾਈ(cpat);
            ਜੇ (ipat == pat.length()) {
                ਵਾਪਸ ਸੂਚਕਾਂਕ; // success; c parsed
            }
            // fall thru; get next cpat
        }

        // match failure of literal
        ਨਹੀਂਤਾਂ {
            ਵਾਪਸ -1;
        }

        cpat = pat.char32At(ipat);
    }

    ਵਾਪਸ -1; // text ended before end of pat
}

int32_t ICU_Utility::parseAsciiInteger(ਅਚਲ UnicodeString& str, int32_t& pos) {
    int32_t ਨਤੀਜਾ = 0;
    char16_t c;
    ਜਦੋਂ (pos < str.length() && (c = str.charAt(pos)) >= u'0' && c <= u'9') {
        ਨਤੀਜਾ = ਨਤੀਜਾ * 10 + (c - u'0');
        pos++;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
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
ਖਾਲੀ ICU_Utility::append_ਮੰਜ਼ਿਲ_ਨਿਯਮ(UnicodeString& ਨਿਯਮ,
                               UChar32 c,
                               UBool isLiteral,
                               UBool escapeUnprintable,
                               UnicodeString& quoteBuf) {
    // If we are escaping unprintables, then escape them outside
    // quotes.  \u and \U are not recognized within quotes.  The same
    // logic applies to literals, but literals are never escaped.
    ਜੇ (isLiteral ||
        (escapeUnprintable && ICU_Utility::isUnprintable(c))) {
        ਜੇ (quoteBuf.length() > 0) {
            // We prefer backslash APOSTROPHE to double APOSTROPHE
            // (more readable, less similar to ") so if there are
            // double APOSTROPHEs at the ends, we pull them outside
            // of the quote.

            // If the first thing in the quoteBuf is APOSTROPHE
            // (doubled) then pull it out.
            ਜਦੋਂ (quoteBuf.length() >= 2 &&
                   quoteBuf.charAt(0) == APOSTROPHE &&
                   quoteBuf.charAt(1) == APOSTROPHE) {
                ਨਿਯਮ.append(BACKSLASH).append(APOSTROPHE);
                quoteBuf.remove(0, 2);
            }
            // If the last thing in the quoteBuf is APOSTROPHE
            // (doubled) then remove and count it and add it after.
            int32_t trailing_ਗਿਣਤੀ = 0;
            ਜਦੋਂ (quoteBuf.length() >= 2 &&
                   quoteBuf.charAt(quoteBuf.length()-2) == APOSTROPHE &&
                   quoteBuf.charAt(quoteBuf.length()-1) == APOSTROPHE) {
                quoteBuf.truncate(quoteBuf.length()-2);
                ++trailing_ਗਿਣਤੀ;
            }
            ਜੇ (quoteBuf.length() > 0) {
                ਨਿਯਮ.append(APOSTROPHE);
                ਨਿਯਮ.append(quoteBuf);
                ਨਿਯਮ.append(APOSTROPHE);
                quoteBuf.truncate(0);
            }
            ਜਦੋਂ (trailing_ਗਿਣਤੀ-- > 0) {
                ਨਿਯਮ.append(BACKSLASH).append(APOSTROPHE);
            }
        }
        ਜੇ (c != static_cast<UChar32>(-1)) {
            /* Since spaces are ignored during parsing, they are
             * emitted only for readability.  We emit one here
             * only if there isn't already one at the end of the
             * rule.
             */
            ਜੇ (c == SPACE) {
                int32_t len = ਨਿਯਮ.length();
                ਜੇ (len > 0 && ਨਿਯਮ.charAt(len-1) != c) {
                    ਨਿਯਮ.append(c);
                }
            } ਨਹੀਂਤਾਂ ਜੇ (!escapeUnprintable || !ICU_Utility::escapeUnprintable(ਨਿਯਮ, c)) {
                ਨਿਯਮ.append(c);
            }
        }
    }

    // Escape ' and '\' and don't begin a quote just for them
    ਨਹੀਂਤਾਂ ਜੇ (quoteBuf.length() == 0 &&
             (c == APOSTROPHE || c == BACKSLASH)) {
        ਨਿਯਮ.append(BACKSLASH);
        ਨਿਯਮ.append(c);
    }

    // Specials (printable ascii that isn't [0-9a-zA-Z]) and
    // whitespace need quoting.  Also append stuff to quotes if we are
    // building up a quoted substring already.
    ਨਹੀਂਤਾਂ ਜੇ (quoteBuf.length() > 0 ||
             (c >= 0x0021 && c <= 0x007E &&
              !((c >= 0x0030/*'0'*/ && c <= 0x0039/*'9'*/) ||
                (c >= 0x0041/*'A'*/ && c <= 0x005A/*'Z'*/) ||
                (c >= 0x0061/*'a'*/ && c <= 0x007A/*'z'*/))) ||
             ਨਮੂਨਾ_Props::isWhiteSpace(c)) {
        quoteBuf.append(c);
        // Double ' within a quote
        ਜੇ (c == APOSTROPHE) {
            quoteBuf.append(c);
        }
    }
    
    // Otherwise just append
    ਨਹੀਂਤਾਂ {
        ਨਿਯਮ.append(c);
    }
}

ਖਾਲੀ ICU_Utility::append_ਮੰਜ਼ਿਲ_ਨਿਯਮ(UnicodeString& ਨਿਯਮ,
                               ਅਚਲ UnicodeString& ਪਾਠ,
                               UBool isLiteral,
                               UBool escapeUnprintable,
                               UnicodeString& quoteBuf) {
    ਲਈ (int32_t i=0; i<ਪਾਠ.length(); ++i) {
        append_ਮੰਜ਼ਿਲ_ਨਿਯਮ(ਨਿਯਮ, ਪਾਠ[i], isLiteral, escapeUnprintable, quoteBuf);
    }
}

/**
 * Given a matcher reference, which may be null, append its
 * pattern as a literal to the given rule.
 */
ਖਾਲੀ ICU_Utility::append_ਮੰਜ਼ਿਲ_ਨਿਯਮ(UnicodeString& ਨਿਯਮ,
                               ਅਚਲ UnicodeMatcher* matcher,
                               UBool escapeUnprintable,
                               UnicodeString& quoteBuf) {
    ਜੇ (matcher != ਖਾਲੀ_ਸੂਚਕ) {
        UnicodeString pat;
        append_ਮੰਜ਼ਿਲ_ਨਿਯਮ(ਨਿਯਮ, matcher->ਮੰਜ਼ਿਲ_ਨਮੂਨਾ(pat, escapeUnprintable),
                     ਸੱਚ, escapeUnprintable, quoteBuf);
    }
}

U_NAMESPACE_ਸਿਰਾ
