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
#ଅନ୍ତର୍ଭାଵ "ruleiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/parsepos.h"
#ଅନ୍ତର୍ଭାଵ "unicode/symtable.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#define MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

RuleCharacterIterator::RuleCharacterIterator(ଅଚଳ UnicodeString& theText, ଅଚଳ ପ୍ରତୀକ_Table* theSym,
                      ParsePosition& thePos) :
    text(theText),
    pos(thePos),
    sym(theSym),
    buf(nullptr),
    bufPos(0)
{}

UBool RuleCharacterIterator::atEnd() ଅଚଳ {
    ଫେରନ୍ତୁ buf == nullptr && pos.getIndex() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    ଯଦି (U_FAILURE(ec)) ଫେରନ୍ତୁ DONE;

    UChar32 c = DONE;
    isEscaped = ମିଥ୍ୟା;

    ପାଇଁ (;;) {
        c = _current();
        _advance(U16_LENGTH(c));

        ଯଦି (c == ପ୍ରତୀକ_Table::ପ୍ରତୀକ_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            ଯଦି (name.length() == 0) {
                ଭାଙ୍ଗ;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            ଯଦି (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                ଫେରନ୍ତୁ DONE;
            }
            // Handle empty variable value
            ଯଦି (buf->length() == 0) {
                buf = nullptr;
            }
            ଜାରି;
        }

        ଯଦି ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            ଜାରି;
        }

        ଯଦି (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString tempEscape;
            int32_t offset = 0;
            c = lookahead(tempEscape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = ସତ୍ୟ;
            ଯଦି (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                ଫେରନ୍ତୁ DONE;
            }
        }

        ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ c;
}

ଶୂନ୍ୟ RuleCharacterIterator::getPos(RuleCharacterIterator::Pos& p) ଅଚଳ {
    p.buf = buf;
    p.pos = pos.getIndex();
    p.bufPos = bufPos;
}

ଶୂନ୍ୟ RuleCharacterIterator::setPos(ଅଚଳ RuleCharacterIterator::Pos& p) {
    buf = p.buf;
    pos.setIndex(p.pos);
    bufPos = p.bufPos;
}

ଶୂନ୍ୟ RuleCharacterIterator::skipIgnored(int32_t options) {
    ଯଦି ((options & SKIP_WHITESPACE) != 0) {
        ପାଇଁ (;;) {
            UChar32 a = _current();
            ଯଦି (!PatternProps::isWhiteSpace(a)) ଭାଙ୍ଗ;
            _advance(U16_LENGTH(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& result, int32_t maxLookAhead) ଅଚଳ {
    ଯଦି (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    ଯଦି (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, result);
    } ଅନ୍ୟଥା {
        text.extract(pos.getIndex(), maxLookAhead, result);
    }
    ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ RuleCharacterIterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 RuleCharacterIterator::_current() ଅଚଳ {
    ଯଦି (buf != nullptr) {
        ଫେରନ୍ତୁ buf->char32At(bufPos);
    } ଅନ୍ୟଥା {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = pos.getIndex();
        ଫେରନ୍ତୁ (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

ଶୂନ୍ୟ RuleCharacterIterator::_advance(int32_t count) {
    ଯଦି (buf != nullptr) {
        bufPos += count;
        ଯଦି (bufPos == buf->length()) {
            buf = nullptr;
        }
    } ଅନ୍ୟଥା {
        pos.setIndex(pos.getIndex() + count);
        ଯଦି (pos.getIndex() > text.length()) {
            pos.setIndex(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
