// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2003-2011, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
* Author: Alan Liu
* Created: September 24 2003
* Since: ICU 2.8
**********************************************************************
*/
#अन्तर्भाव "ruleiter.h"
#अन्तर्भाव "unicode/parsepos.h"
#अन्तर्भाव "unicode/symtable.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#define MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

RuleCharacterIterator::RuleCharacterIterator(ध्रुव UnicodeString& theText, ध्रुव चिह्नम्_Table* theSym,
                      ParsePosition& thePos) :
    text(theText),
    pos(thePos),
    sym(theSym),
    buf(nullptr),
    bufPos(0)
{}

UBool RuleCharacterIterator::atEnd() ध्रुव {
    प्रत्यागम buf == nullptr && pos.getIndex() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    जइ (U_FAILURE(ec)) प्रत्यागम DONE;

    UChar32 c = DONE;
    isEscaped = मिच्छ;

    प्रति (;;) {
        c = _current();
        _advance(U16_LENGTH(c));

        जइ (c == चिह्नम्_Table::चिह्नम्_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            जइ (name.length() == 0) {
                भङ्ग;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            जइ (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                प्रत्यागम DONE;
            }
            // Handle empty variable value
            जइ (buf->length() == 0) {
                buf = nullptr;
            }
            अनुवर्त;
        }

        जइ ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            अनुवर्त;
        }

        जइ (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString tempEscape;
            int32_t offset = 0;
            c = lookahead(tempEscape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = सच्च;
            जइ (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                प्रत्यागम DONE;
            }
        }

        भङ्ग;
    }

    प्रत्यागम c;
}

शून्य RuleCharacterIterator::getPos(RuleCharacterIterator::Pos& p) ध्रुव {
    p.buf = buf;
    p.pos = pos.getIndex();
    p.bufPos = bufPos;
}

शून्य RuleCharacterIterator::setPos(ध्रुव RuleCharacterIterator::Pos& p) {
    buf = p.buf;
    pos.setIndex(p.pos);
    bufPos = p.bufPos;
}

शून्य RuleCharacterIterator::skipIgnored(int32_t options) {
    जइ ((options & SKIP_WHITESPACE) != 0) {
        प्रति (;;) {
            UChar32 a = _current();
            जइ (!PatternProps::isWhiteSpace(a)) भङ्ग;
            _advance(U16_LENGTH(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& result, int32_t maxLookAhead) ध्रुव {
    जइ (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    जइ (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, result);
    } अण्णहा {
        text.extract(pos.getIndex(), maxLookAhead, result);
    }
    प्रत्यागम result;
}

शून्य RuleCharacterIterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 RuleCharacterIterator::_current() ध्रुव {
    जइ (buf != nullptr) {
        प्रत्यागम buf->char32At(bufPos);
    } अण्णहा {
        पूर्णाङ्क i = pos.getIndex();
        प्रत्यागम (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

शून्य RuleCharacterIterator::_advance(int32_t count) {
    जइ (buf != nullptr) {
        bufPos += count;
        जइ (bufPos == buf->length()) {
            buf = nullptr;
        }
    } अण्णहा {
        pos.setIndex(pos.getIndex() + count);
        जइ (pos.getIndex() > text.length()) {
            pos.setIndex(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
