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

#شامل "unicode/unimatch.h"
#شامل "unicode/utf16.h"
#شامل "patternprops.h"
#شامل "util.h"

// Define char16_t constants using hex for EBCDIC compatibility

ساکن ثابت char16_t BACKSLASH  = 0x005C; /*\*/
ساکن ثابت char16_t بالائی_U    = 0x0055; /*U*/
ساکن ثابت char16_t زیریں_U    = 0x0075; /*u*/
ساکن ثابت char16_t APOSTROPHE = 0x0027; // '\''
ساکن ثابت char16_t SPACE      = 0x0020; // ' '

// "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
ساکن ثابت char16_t DIGITS[] = {
    48,49,50,51,52,53,54,55,56,57,
    65,66,67,68,69,70,71,72,73,74,
    75,76,77,78,79,80,81,82,83,84,
    85,86,87,88,89,90
};

U_NAMESPACE_BEGIN

UnicodeString& ICU_Utility::append_نمبر(UnicodeString& نتیجہ, int32_t n,
                                     int32_t radix, int32_t minDigits) {
    اگر (radix < 2 || radix > 36) {
        // Bogus radix
        واپس نتیجہ.append(static_cast<char16_t>(63)/*?*/);
    }
    // Handle negatives
    اگر (n < 0) {
        n = -n;
        نتیجہ.append(static_cast<char16_t>(45)/*-*/);
    }
    // First determine the number of digits
    int32_t nn = n;
    int32_t r = 1;
    جب (nn >= radix) {
        nn /= radix;
        r *= radix;
        --minDigits;
    }
    // Now generate the digits
    جب (--minDigits > 0) {
        نتیجہ.append(DIGITS[0]);
    }
    جب (r > 0) {
        int32_t digit = n / r;
        نتیجہ.append(DIGITS[digit]);
        n -= digit * r;
        r /= radix;
    }
    واپس نتیجہ;
}

UBool ICU_Utility::isUnprintable(UChar32 c) {
    واپس !(c >= 0x20 && c <= 0x7E);
}

UBool ICU_Utility::shouldAlwaysBeEscaped(UChar32 c) {
    اگر (c < 0x20) {
        واپس سچ;  // C0 control codes
    } ورنہ اگر (c <= 0x7e) {
        واپس جھوٹ;  // printable ASCII
    } ورنہ اگر (c <= 0x9f) {
        واپس سچ;  // C1 control codes
    } ورنہ اگر (c < 0xd800) {
        واپس جھوٹ;  // most of the BMP
    } ورنہ اگر (c <= 0xdfff || (0xfdd0 <= c && c <= 0xfdef) || (c & 0xfffe) == 0xfffe) {
        واپس سچ;  // surrogate or noncharacter code points
    } ورنہ اگر (c <= 0x10ffff) {
        واپس جھوٹ;  // all else
    } ورنہ {
        واپس سچ;  // not a code point
    }
}

UBool ICU_Utility::escapeUnprintable(UnicodeString& نتیجہ, UChar32 c) {
    اگر (isUnprintable(c)) {
        escape(نتیجہ, c);
        واپس سچ;
    }
    واپس جھوٹ;
}

UnicodeString &ICU_Utility::escape(UnicodeString& نتیجہ, UChar32 c) {
    نتیجہ.append(BACKSLASH);
    اگر (c & ~0xFFFF) {
        نتیجہ.append(بالائی_U);
        نتیجہ.append(DIGITS[0xF&(c>>28)]);
        نتیجہ.append(DIGITS[0xF&(c>>24)]);
        نتیجہ.append(DIGITS[0xF&(c>>20)]);
        نتیجہ.append(DIGITS[0xF&(c>>16)]);
    } ورنہ {
        نتیجہ.append(زیریں_U);
    }
    نتیجہ.append(DIGITS[0xF&(c>>12)]);
    نتیجہ.append(DIGITS[0xF&(c>>8)]);
    نتیجہ.append(DIGITS[0xF&(c>>4)]);
    نتیجہ.append(DIGITS[0xF&c]);
    واپس نتیجہ;
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
int32_t ICU_Utility::skipWhitespace(ثابت UnicodeString& str, int32_t& pos,
                                    UBool advance) {
    int32_t p = pos;
    ثابت char16_t* s = str.get_بفر();
    p = static_cast<int32_t>(نمونہ_Props::skipWhiteSpace(s + p, str.length() - p) - s);
    اگر (advance) {
        pos = p;
    }
    واپس p;
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
UBool ICU_Utility::parseChar(ثابت UnicodeString& id, int32_t& pos, char16_t ch) {
    int32_t آغاز = pos;
    skipWhitespace(id, pos, سچ);
    اگر (pos == id.length() ||
        id.charAt(pos) != ch) {
        pos = آغاز;
        واپس جھوٹ;
    }
    ++pos;
    واپس سچ;
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
int32_t ICU_Utility::parse_نمونہ(ثابت UnicodeString& pat,
                                  ثابت Replaceable& متن,
                                  int32_t اشاریہ,
                                  int32_t حد_بندی) {
    int32_t ipat = 0;

    // empty pattern matches immediately
    اگر (ipat == pat.length()) {
        واپس اشاریہ;
    }

    UChar32 cpat = pat.char32At(ipat);

    جب (اشاریہ < حد_بندی) {
        UChar32 c = متن.char32At(اشاریہ);

        // parse \s*
        اگر (cpat == 126 /*~*/) {
            اگر (نمونہ_Props::isWhiteSpace(c)) {
                اشاریہ += U16_لمبائی(c);
                جاری;
            } ورنہ {
                اگر (++ipat == pat.length()) {
                    واپس اشاریہ; // success; c unparsed
                }
                // fall thru; process c again with next cpat
            }
        }

        // parse literal
        ورنہ اگر (c == cpat) {
            اشاریہ += U16_لمبائی(c);
            ipat += U16_لمبائی(cpat);
            اگر (ipat == pat.length()) {
                واپس اشاریہ; // success; c parsed
            }
            // fall thru; get next cpat
        }

        // match failure of literal
        ورنہ {
            واپس -1;
        }

        cpat = pat.char32At(ipat);
    }

    واپس -1; // text ended before end of pat
}

int32_t ICU_Utility::parseAsciiInteger(ثابت UnicodeString& str, int32_t& pos) {
    int32_t نتیجہ = 0;
    char16_t c;
    جب (pos < str.length() && (c = str.charAt(pos)) >= u'0' && c <= u'9') {
        نتیجہ = نتیجہ * 10 + (c - u'0');
        pos++;
    }
    واپس نتیجہ;
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
خالی ICU_Utility::append_مقصد_قاعدہ(UnicodeString& قاعدہ,
                               UChar32 c,
                               UBool isLiteral,
                               UBool escapeUnprintable,
                               UnicodeString& quoteBuf) {
    // If we are escaping unprintables, then escape them outside
    // quotes.  \u and \U are not recognized within quotes.  The same
    // logic applies to literals, but literals are never escaped.
    اگر (isLiteral ||
        (escapeUnprintable && ICU_Utility::isUnprintable(c))) {
        اگر (quoteBuf.length() > 0) {
            // We prefer backslash APOSTROPHE to double APOSTROPHE
            // (more readable, less similar to ") so if there are
            // double APOSTROPHEs at the ends, we pull them outside
            // of the quote.

            // If the first thing in the quoteBuf is APOSTROPHE
            // (doubled) then pull it out.
            جب (quoteBuf.length() >= 2 &&
                   quoteBuf.charAt(0) == APOSTROPHE &&
                   quoteBuf.charAt(1) == APOSTROPHE) {
                قاعدہ.append(BACKSLASH).append(APOSTROPHE);
                quoteBuf.remove(0, 2);
            }
            // If the last thing in the quoteBuf is APOSTROPHE
            // (doubled) then remove and count it and add it after.
            int32_t trailing_گنتی = 0;
            جب (quoteBuf.length() >= 2 &&
                   quoteBuf.charAt(quoteBuf.length()-2) == APOSTROPHE &&
                   quoteBuf.charAt(quoteBuf.length()-1) == APOSTROPHE) {
                quoteBuf.truncate(quoteBuf.length()-2);
                ++trailing_گنتی;
            }
            اگر (quoteBuf.length() > 0) {
                قاعدہ.append(APOSTROPHE);
                قاعدہ.append(quoteBuf);
                قاعدہ.append(APOSTROPHE);
                quoteBuf.truncate(0);
            }
            جب (trailing_گنتی-- > 0) {
                قاعدہ.append(BACKSLASH).append(APOSTROPHE);
            }
        }
        اگر (c != static_cast<UChar32>(-1)) {
            /* Since spaces are ignored during parsing, they are
             * emitted only for readability.  We emit one here
             * only if there isn't already one at the end of the
             * rule.
             */
            اگر (c == SPACE) {
                int32_t len = قاعدہ.length();
                اگر (len > 0 && قاعدہ.charAt(len-1) != c) {
                    قاعدہ.append(c);
                }
            } ورنہ اگر (!escapeUnprintable || !ICU_Utility::escapeUnprintable(قاعدہ, c)) {
                قاعدہ.append(c);
            }
        }
    }

    // Escape ' and '\' and don't begin a quote just for them
    ورنہ اگر (quoteBuf.length() == 0 &&
             (c == APOSTROPHE || c == BACKSLASH)) {
        قاعدہ.append(BACKSLASH);
        قاعدہ.append(c);
    }

    // Specials (printable ascii that isn't [0-9a-zA-Z]) and
    // whitespace need quoting.  Also append stuff to quotes if we are
    // building up a quoted substring already.
    ورنہ اگر (quoteBuf.length() > 0 ||
             (c >= 0x0021 && c <= 0x007E &&
              !((c >= 0x0030/*'0'*/ && c <= 0x0039/*'9'*/) ||
                (c >= 0x0041/*'A'*/ && c <= 0x005A/*'Z'*/) ||
                (c >= 0x0061/*'a'*/ && c <= 0x007A/*'z'*/))) ||
             نمونہ_Props::isWhiteSpace(c)) {
        quoteBuf.append(c);
        // Double ' within a quote
        اگر (c == APOSTROPHE) {
            quoteBuf.append(c);
        }
    }
    
    // Otherwise just append
    ورنہ {
        قاعدہ.append(c);
    }
}

خالی ICU_Utility::append_مقصد_قاعدہ(UnicodeString& قاعدہ,
                               ثابت UnicodeString& متن,
                               UBool isLiteral,
                               UBool escapeUnprintable,
                               UnicodeString& quoteBuf) {
    برائے (int32_t i=0; i<متن.length(); ++i) {
        append_مقصد_قاعدہ(قاعدہ, متن[i], isLiteral, escapeUnprintable, quoteBuf);
    }
}

/**
 * Given a matcher reference, which may be null, append its
 * pattern as a literal to the given rule.
 */
خالی ICU_Utility::append_مقصد_قاعدہ(UnicodeString& قاعدہ,
                               ثابت UnicodeMatcher* matcher,
                               UBool escapeUnprintable,
                               UnicodeString& quoteBuf) {
    اگر (matcher != خالی_اشاریہ) {
        UnicodeString pat;
        append_مقصد_قاعدہ(قاعدہ, matcher->مقصد_نمونہ(pat, escapeUnprintable),
                     سچ, escapeUnprintable, quoteBuf);
    }
}

U_NAMESPACE_انتہا
