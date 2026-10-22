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

#అన్తర్భావ "unicode/unimatch.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "patternprops.h"
#అన్తర్భావ "util.h"

// Define char16_t constants using hex for EBCDIC compatibility

స్థిర స్థిరరాశి char16_t BACKSLASH  = 0x005C; /*\*/
స్థిర స్థిరరాశి char16_t పై_U    = 0x0055; /*U*/
స్థిర స్థిరరాశి char16_t క్రింది_U    = 0x0075; /*u*/
స్థిర స్థిరరాశి char16_t APOSTROPHE = 0x0027; // '\''
స్థిర స్థిరరాశి char16_t SPACE      = 0x0020; // ' '

// "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
స్థిర స్థిరరాశి char16_t DIGITS[] = {
    48,49,50,51,52,53,54,55,56,57,
    65,66,67,68,69,70,71,72,73,74,
    75,76,77,78,79,80,81,82,83,84,
    85,86,87,88,89,90
};

U_NAMESPACE_BEGIN

UnicodeString& ICU_Utility::append_సంఖ్య(UnicodeString& ఫలితం, int32_t n,
                                     int32_t radix, int32_t minDigits) {
    ఒకవేళ (radix < 2 || radix > 36) {
        // Bogus radix
        తిరిగి ఫలితం.append(static_cast<char16_t>(63)/*?*/);
    }
    // Handle negatives
    ఒకవేళ (n < 0) {
        n = -n;
        ఫలితం.append(static_cast<char16_t>(45)/*-*/);
    }
    // First determine the number of digits
    int32_t nn = n;
    int32_t r = 1;
    అయితే (nn >= radix) {
        nn /= radix;
        r *= radix;
        --minDigits;
    }
    // Now generate the digits
    అయితే (--minDigits > 0) {
        ఫలితం.append(DIGITS[0]);
    }
    అయితే (r > 0) {
        int32_t digit = n / r;
        ఫలితం.append(DIGITS[digit]);
        n -= digit * r;
        r /= radix;
    }
    తిరిగి ఫలితం;
}

UBool ICU_Utility::isUnprintable(UChar32 c) {
    తిరిగి !(c >= 0x20 && c <= 0x7E);
}

UBool ICU_Utility::shouldAlwaysBeEscaped(UChar32 c) {
    ఒకవేళ (c < 0x20) {
        తిరిగి నిజం;  // C0 control codes
    } లేకపోతే ఒకవేళ (c <= 0x7e) {
        తిరిగి అబద్ధం;  // printable ASCII
    } లేకపోతే ఒకవేళ (c <= 0x9f) {
        తిరిగి నిజం;  // C1 control codes
    } లేకపోతే ఒకవేళ (c < 0xd800) {
        తిరిగి అబద్ధం;  // most of the BMP
    } లేకపోతే ఒకవేళ (c <= 0xdfff || (0xfdd0 <= c && c <= 0xfdef) || (c & 0xfffe) == 0xfffe) {
        తిరిగి నిజం;  // surrogate or noncharacter code points
    } లేకపోతే ఒకవేళ (c <= 0x10ffff) {
        తిరిగి అబద్ధం;  // all else
    } లేకపోతే {
        తిరిగి నిజం;  // not a code point
    }
}

UBool ICU_Utility::escapeUnprintable(UnicodeString& ఫలితం, UChar32 c) {
    ఒకవేళ (isUnprintable(c)) {
        escape(ఫలితం, c);
        తిరిగి నిజం;
    }
    తిరిగి అబద్ధం;
}

UnicodeString &ICU_Utility::escape(UnicodeString& ఫలితం, UChar32 c) {
    ఫలితం.append(BACKSLASH);
    ఒకవేళ (c & ~0xFFFF) {
        ఫలితం.append(పై_U);
        ఫలితం.append(DIGITS[0xF&(c>>28)]);
        ఫలితం.append(DIGITS[0xF&(c>>24)]);
        ఫలితం.append(DIGITS[0xF&(c>>20)]);
        ఫలితం.append(DIGITS[0xF&(c>>16)]);
    } లేకపోతే {
        ఫలితం.append(క్రింది_U);
    }
    ఫలితం.append(DIGITS[0xF&(c>>12)]);
    ఫలితం.append(DIGITS[0xF&(c>>8)]);
    ఫలితం.append(DIGITS[0xF&(c>>4)]);
    ఫలితం.append(DIGITS[0xF&c]);
    తిరిగి ఫలితం;
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
int32_t ICU_Utility::skipWhitespace(స్థిరరాశి UnicodeString& str, int32_t& pos,
                                    UBool advance) {
    int32_t p = pos;
    స్థిరరాశి char16_t* s = str.get_బఫరు();
    p = static_cast<int32_t>(నమూనా_Props::skipWhiteSpace(s + p, str.length() - p) - s);
    ఒకవేళ (advance) {
        pos = p;
    }
    తిరిగి p;
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
UBool ICU_Utility::parseChar(స్థిరరాశి UnicodeString& id, int32_t& pos, char16_t ch) {
    int32_t ప్రారంభం = pos;
    skipWhitespace(id, pos, నిజం);
    ఒకవేళ (pos == id.length() ||
        id.charAt(pos) != ch) {
        pos = ప్రారంభం;
        తిరిగి అబద్ధం;
    }
    ++pos;
    తిరిగి నిజం;
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
int32_t ICU_Utility::parse_నమూనా(స్థిరరాశి UnicodeString& pat,
                                  స్థిరరాశి Replaceable& పాఠ్యం,
                                  int32_t సూచీ,
                                  int32_t హద్దు) {
    int32_t ipat = 0;

    // empty pattern matches immediately
    ఒకవేళ (ipat == pat.length()) {
        తిరిగి సూచీ;
    }

    UChar32 cpat = pat.char32At(ipat);

    అయితే (సూచీ < హద్దు) {
        UChar32 c = పాఠ్యం.char32At(సూచీ);

        // parse \s*
        ఒకవేళ (cpat == 126 /*~*/) {
            ఒకవేళ (నమూనా_Props::isWhiteSpace(c)) {
                సూచీ += U16_నిడివి(c);
                కొనసాగు;
            } లేకపోతే {
                ఒకవేళ (++ipat == pat.length()) {
                    తిరిగి సూచీ; // success; c unparsed
                }
                // fall thru; process c again with next cpat
            }
        }

        // parse literal
        లేకపోతే ఒకవేళ (c == cpat) {
            సూచీ += U16_నిడివి(c);
            ipat += U16_నిడివి(cpat);
            ఒకవేళ (ipat == pat.length()) {
                తిరిగి సూచీ; // success; c parsed
            }
            // fall thru; get next cpat
        }

        // match failure of literal
        లేకపోతే {
            తిరిగి -1;
        }

        cpat = pat.char32At(ipat);
    }

    తిరిగి -1; // text ended before end of pat
}

int32_t ICU_Utility::parseAsciiInteger(స్థిరరాశి UnicodeString& str, int32_t& pos) {
    int32_t ఫలితం = 0;
    char16_t c;
    అయితే (pos < str.length() && (c = str.charAt(pos)) >= u'0' && c <= u'9') {
        ఫలితం = ఫలితం * 10 + (c - u'0');
        pos++;
    }
    తిరిగి ఫలితం;
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
శూన్య ICU_Utility::append_గమ్యం_నియమం(UnicodeString& నియమం,
                               UChar32 c,
                               UBool isLiteral,
                               UBool escapeUnprintable,
                               UnicodeString& quoteBuf) {
    // If we are escaping unprintables, then escape them outside
    // quotes.  \u and \U are not recognized within quotes.  The same
    // logic applies to literals, but literals are never escaped.
    ఒకవేళ (isLiteral ||
        (escapeUnprintable && ICU_Utility::isUnprintable(c))) {
        ఒకవేళ (quoteBuf.length() > 0) {
            // We prefer backslash APOSTROPHE to double APOSTROPHE
            // (more readable, less similar to ") so if there are
            // double APOSTROPHEs at the ends, we pull them outside
            // of the quote.

            // If the first thing in the quoteBuf is APOSTROPHE
            // (doubled) then pull it out.
            అయితే (quoteBuf.length() >= 2 &&
                   quoteBuf.charAt(0) == APOSTROPHE &&
                   quoteBuf.charAt(1) == APOSTROPHE) {
                నియమం.append(BACKSLASH).append(APOSTROPHE);
                quoteBuf.remove(0, 2);
            }
            // If the last thing in the quoteBuf is APOSTROPHE
            // (doubled) then remove and count it and add it after.
            int32_t trailing_లెక్క = 0;
            అయితే (quoteBuf.length() >= 2 &&
                   quoteBuf.charAt(quoteBuf.length()-2) == APOSTROPHE &&
                   quoteBuf.charAt(quoteBuf.length()-1) == APOSTROPHE) {
                quoteBuf.truncate(quoteBuf.length()-2);
                ++trailing_లెక్క;
            }
            ఒకవేళ (quoteBuf.length() > 0) {
                నియమం.append(APOSTROPHE);
                నియమం.append(quoteBuf);
                నియమం.append(APOSTROPHE);
                quoteBuf.truncate(0);
            }
            అయితే (trailing_లెక్క-- > 0) {
                నియమం.append(BACKSLASH).append(APOSTROPHE);
            }
        }
        ఒకవేళ (c != static_cast<UChar32>(-1)) {
            /* Since spaces are ignored during parsing, they are
             * emitted only for readability.  We emit one here
             * only if there isn't already one at the end of the
             * rule.
             */
            ఒకవేళ (c == SPACE) {
                int32_t len = నియమం.length();
                ఒకవేళ (len > 0 && నియమం.charAt(len-1) != c) {
                    నియమం.append(c);
                }
            } లేకపోతే ఒకవేళ (!escapeUnprintable || !ICU_Utility::escapeUnprintable(నియమం, c)) {
                నియమం.append(c);
            }
        }
    }

    // Escape ' and '\' and don't begin a quote just for them
    లేకపోతే ఒకవేళ (quoteBuf.length() == 0 &&
             (c == APOSTROPHE || c == BACKSLASH)) {
        నియమం.append(BACKSLASH);
        నియమం.append(c);
    }

    // Specials (printable ascii that isn't [0-9a-zA-Z]) and
    // whitespace need quoting.  Also append stuff to quotes if we are
    // building up a quoted substring already.
    లేకపోతే ఒకవేళ (quoteBuf.length() > 0 ||
             (c >= 0x0021 && c <= 0x007E &&
              !((c >= 0x0030/*'0'*/ && c <= 0x0039/*'9'*/) ||
                (c >= 0x0041/*'A'*/ && c <= 0x005A/*'Z'*/) ||
                (c >= 0x0061/*'a'*/ && c <= 0x007A/*'z'*/))) ||
             నమూనా_Props::isWhiteSpace(c)) {
        quoteBuf.append(c);
        // Double ' within a quote
        ఒకవేళ (c == APOSTROPHE) {
            quoteBuf.append(c);
        }
    }
    
    // Otherwise just append
    లేకపోతే {
        నియమం.append(c);
    }
}

శూన్య ICU_Utility::append_గమ్యం_నియమం(UnicodeString& నియమం,
                               స్థిరరాశి UnicodeString& పాఠ్యం,
                               UBool isLiteral,
                               UBool escapeUnprintable,
                               UnicodeString& quoteBuf) {
    కొరకు (int32_t i=0; i<పాఠ్యం.length(); ++i) {
        append_గమ్యం_నియమం(నియమం, పాఠ్యం[i], isLiteral, escapeUnprintable, quoteBuf);
    }
}

/**
 * Given a matcher reference, which may be null, append its
 * pattern as a literal to the given rule.
 */
శూన్య ICU_Utility::append_గమ్యం_నియమం(UnicodeString& నియమం,
                               స్థిరరాశి UnicodeMatcher* matcher,
                               UBool escapeUnprintable,
                               UnicodeString& quoteBuf) {
    ఒకవేళ (matcher != శూన్యసూచిక) {
        UnicodeString pat;
        append_గమ్యం_నియమం(నియమం, matcher->గమ్యం_నమూనా(pat, escapeUnprintable),
                     నిజం, escapeUnprintable, quoteBuf);
    }
}

U_NAMESPACE_చివర
