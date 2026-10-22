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

#समावेश "unicode/unimatch.h"
#समावेश "unicode/utf16.h"
#समावेश "patternprops.h"
#समावेश "util.h"

// Define char16_t constants using hex for EBCDIC compatibility

अटल स्थिर char16_t BACKSLASH  = 0x005C; /*\*/
अटल स्थिर char16_t UPPER_U    = 0x0055; /*U*/
अटल स्थिर char16_t LOWER_U    = 0x0075; /*u*/
अटल स्थिर char16_t APOSTROPHE = 0x0027; // '\''
अटल स्थिर char16_t SPACE      = 0x0020; // ' '

// "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
अटल स्थिर char16_t DIGITS[] = {
    48,49,50,51,52,53,54,55,56,57,
    65,66,67,68,69,70,71,72,73,74,
    75,76,77,78,79,80,81,82,83,84,
    85,86,87,88,89,90
};

U_NAMESPACE_BEGIN

UnicodeString& ICU_Utility::append_संख्या(UnicodeString& परिणाम, int32_t n,
                                     int32_t radix, int32_t minDigits) {
    अगर (radix < 2 || radix > 36) {
        // Bogus radix
        वापस परिणाम.append(static_cast<char16_t>(63)/*?*/);
    }
    // Handle negatives
    अगर (n < 0) {
        n = -n;
        परिणाम.append(static_cast<char16_t>(45)/*-*/);
    }
    // First determine the number of digits
    int32_t nn = n;
    int32_t r = 1;
    जबतक (nn >= radix) {
        nn /= radix;
        r *= radix;
        --minDigits;
    }
    // Now generate the digits
    जबतक (--minDigits > 0) {
        परिणाम.append(DIGITS[0]);
    }
    जबतक (r > 0) {
        int32_t digit = n / r;
        परिणाम.append(DIGITS[digit]);
        n -= digit * r;
        r /= radix;
    }
    वापस परिणाम;
}

UBool ICU_Utility::isUnprintable(UChar32 c) {
    वापस !(c >= 0x20 && c <= 0x7E);
}

UBool ICU_Utility::shouldAlwaysBeEscaped(UChar32 c) {
    अगर (c < 0x20) {
        वापस true;  // C0 control codes
    } अन्यथा अगर (c <= 0x7e) {
        वापस false;  // printable ASCII
    } अन्यथा अगर (c <= 0x9f) {
        वापस true;  // C1 control codes
    } अन्यथा अगर (c < 0xd800) {
        वापस false;  // most of the BMP
    } अन्यथा अगर (c <= 0xdfff || (0xfdd0 <= c && c <= 0xfdef) || (c & 0xfffe) == 0xfffe) {
        वापस true;  // surrogate or noncharacter code points
    } अन्यथा अगर (c <= 0x10ffff) {
        वापस false;  // all else
    } अन्यथा {
        वापस true;  // not a code point
    }
}

UBool ICU_Utility::escapeUnprintable(UnicodeString& परिणाम, UChar32 c) {
    अगर (isUnprintable(c)) {
        escape(परिणाम, c);
        वापस true;
    }
    वापस false;
}

UnicodeString &ICU_Utility::escape(UnicodeString& परिणाम, UChar32 c) {
    परिणाम.append(BACKSLASH);
    अगर (c & ~0xFFFF) {
        परिणाम.append(UPPER_U);
        परिणाम.append(DIGITS[0xF&(c>>28)]);
        परिणाम.append(DIGITS[0xF&(c>>24)]);
        परिणाम.append(DIGITS[0xF&(c>>20)]);
        परिणाम.append(DIGITS[0xF&(c>>16)]);
    } अन्यथा {
        परिणाम.append(LOWER_U);
    }
    परिणाम.append(DIGITS[0xF&(c>>12)]);
    परिणाम.append(DIGITS[0xF&(c>>8)]);
    परिणाम.append(DIGITS[0xF&(c>>4)]);
    परिणाम.append(DIGITS[0xF&c]);
    वापस परिणाम;
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
int32_t ICU_Utility::skipWhitespace(स्थिर UnicodeString& str, int32_t& pos,
                                    UBool advance) {
    int32_t p = pos;
    स्थिर char16_t* s = str.get_बफर();
    p = static_cast<int32_t>(PatternProps::skipWhiteSpace(s + p, str.length() - p) - s);
    अगर (advance) {
        pos = p;
    }
    वापस p;
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
UBool ICU_Utility::parseChar(स्थिर UnicodeString& id, int32_t& pos, char16_t ch) {
    int32_t start = pos;
    skipWhitespace(id, pos, true);
    अगर (pos == id.length() ||
        id.charAt(pos) != ch) {
        pos = start;
        वापस false;
    }
    ++pos;
    वापस true;
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
int32_t ICU_Utility::parsePattern(स्थिर UnicodeString& pat,
                                  स्थिर Replaceable& text,
                                  int32_t सूचकांक,
                                  int32_t limit) {
    int32_t ipat = 0;

    // empty pattern matches immediately
    अगर (ipat == pat.length()) {
        वापस सूचकांक;
    }

    UChar32 cpat = pat.char32At(ipat);

    जबतक (सूचकांक < limit) {
        UChar32 c = text.char32At(सूचकांक);

        // parse \s*
        अगर (cpat == 126 /*~*/) {
            अगर (PatternProps::isWhiteSpace(c)) {
                सूचकांक += U16_लंबाई(c);
                जारी;
            } अन्यथा {
                अगर (++ipat == pat.length()) {
                    वापस सूचकांक; // success; c unparsed
                }
                // fall thru; process c again with next cpat
            }
        }

        // parse literal
        अन्यथा अगर (c == cpat) {
            सूचकांक += U16_लंबाई(c);
            ipat += U16_लंबाई(cpat);
            अगर (ipat == pat.length()) {
                वापस सूचकांक; // success; c parsed
            }
            // fall thru; get next cpat
        }

        // match failure of literal
        अन्यथा {
            वापस -1;
        }

        cpat = pat.char32At(ipat);
    }

    वापस -1; // text ended before end of pat
}

int32_t ICU_Utility::parseAsciiInteger(स्थिर UnicodeString& str, int32_t& pos) {
    int32_t परिणाम = 0;
    char16_t c;
    जबतक (pos < str.length() && (c = str.charAt(pos)) >= u'0' && c <= u'9') {
        परिणाम = परिणाम * 10 + (c - u'0');
        pos++;
    }
    वापस परिणाम;
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
व्योम ICU_Utility::appendToRule(UnicodeString& rule,
                               UChar32 c,
                               UBool isLiteral,
                               UBool escapeUnprintable,
                               UnicodeString& quoteBuf) {
    // If we are escaping unprintables, then escape them outside
    // quotes.  \u and \U are not recognized within quotes.  The same
    // logic applies to literals, but literals are never escaped.
    अगर (isLiteral ||
        (escapeUnprintable && ICU_Utility::isUnprintable(c))) {
        अगर (quoteBuf.length() > 0) {
            // We prefer backslash APOSTROPHE to double APOSTROPHE
            // (more readable, less similar to ") so if there are
            // double APOSTROPHEs at the ends, we pull them outside
            // of the quote.

            // If the first thing in the quoteBuf is APOSTROPHE
            // (doubled) then pull it out.
            जबतक (quoteBuf.length() >= 2 &&
                   quoteBuf.charAt(0) == APOSTROPHE &&
                   quoteBuf.charAt(1) == APOSTROPHE) {
                rule.append(BACKSLASH).append(APOSTROPHE);
                quoteBuf.हटाओ(0, 2);
            }
            // If the last thing in the quoteBuf is APOSTROPHE
            // (doubled) then remove and count it and add it after.
            int32_t trailing_गिनती = 0;
            जबतक (quoteBuf.length() >= 2 &&
                   quoteBuf.charAt(quoteBuf.length()-2) == APOSTROPHE &&
                   quoteBuf.charAt(quoteBuf.length()-1) == APOSTROPHE) {
                quoteBuf.truncate(quoteBuf.length()-2);
                ++trailing_गिनती;
            }
            अगर (quoteBuf.length() > 0) {
                rule.append(APOSTROPHE);
                rule.append(quoteBuf);
                rule.append(APOSTROPHE);
                quoteBuf.truncate(0);
            }
            जबतक (trailing_गिनती-- > 0) {
                rule.append(BACKSLASH).append(APOSTROPHE);
            }
        }
        अगर (c != static_cast<UChar32>(-1)) {
            /* Since spaces are ignored during parsing, they are
             * emitted only for readability.  We emit one here
             * only if there isn't already one at the end of the
             * rule.
             */
            अगर (c == SPACE) {
                int32_t len = rule.length();
                अगर (len > 0 && rule.charAt(len-1) != c) {
                    rule.append(c);
                }
            } अन्यथा अगर (!escapeUnprintable || !ICU_Utility::escapeUnprintable(rule, c)) {
                rule.append(c);
            }
        }
    }

    // Escape ' and '\' and don't begin a quote just for them
    अन्यथा अगर (quoteBuf.length() == 0 &&
             (c == APOSTROPHE || c == BACKSLASH)) {
        rule.append(BACKSLASH);
        rule.append(c);
    }

    // Specials (printable ascii that isn't [0-9a-zA-Z]) and
    // whitespace need quoting.  Also append stuff to quotes if we are
    // building up a quoted substring already.
    अन्यथा अगर (quoteBuf.length() > 0 ||
             (c >= 0x0021 && c <= 0x007E &&
              !((c >= 0x0030/*'0'*/ && c <= 0x0039/*'9'*/) ||
                (c >= 0x0041/*'A'*/ && c <= 0x005A/*'Z'*/) ||
                (c >= 0x0061/*'a'*/ && c <= 0x007A/*'z'*/))) ||
             PatternProps::isWhiteSpace(c)) {
        quoteBuf.append(c);
        // Double ' within a quote
        अगर (c == APOSTROPHE) {
            quoteBuf.append(c);
        }
    }
    
    // Otherwise just append
    अन्यथा {
        rule.append(c);
    }
}

व्योम ICU_Utility::appendToRule(UnicodeString& rule,
                               स्थिर UnicodeString& text,
                               UBool isLiteral,
                               UBool escapeUnprintable,
                               UnicodeString& quoteBuf) {
    क्रम (int32_t i=0; i<text.length(); ++i) {
        appendToRule(rule, text[i], isLiteral, escapeUnprintable, quoteBuf);
    }
}

/**
 * Given a matcher reference, which may be null, append its
 * pattern as a literal to the given rule.
 */
व्योम ICU_Utility::appendToRule(UnicodeString& rule,
                               स्थिर UnicodeMatcher* matcher,
                               UBool escapeUnprintable,
                               UnicodeString& quoteBuf) {
    अगर (matcher != nullptr) {
        UnicodeString pat;
        appendToRule(rule, matcher->toPattern(pat, escapeUnprintable),
                     true, escapeUnprintable, quoteBuf);
    }
}

U_NAMESPACE_END
