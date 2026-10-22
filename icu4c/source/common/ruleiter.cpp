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
#ضمن "ruleiter.h"
#ضمن "unicode/parsepos.h"
#ضمن "unicode/symtable.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/utf16.h"
#ضمن "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#define MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

RuleCharacterIterator::RuleCharacterIterator(ثابت UnicodeString& theText, ثابت بەلگە_Table* theSym,
                      ParsePosition& thePos) :
    text(theText),
    pos(thePos),
    sym(theSym),
    buf(nullptr),
    bufPos(0)
{}

UBool RuleCharacterIterator::atEnd() ثابت {
    أرجع buf == nullptr && pos.getIndex() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    إذا (U_FAILURE(ec)) أرجع DONE;

    UChar32 c = DONE;
    isEscaped = خطأ;

    لكل (;;) {
        c = _current();
        _advance(U16_LENGTH(c));

        إذا (c == بەلگە_Table::بەلگە_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            إذا (name.length() == 0) {
                اكسر;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            إذا (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                أرجع DONE;
            }
            // Handle empty variable value
            إذا (buf->length() == 0) {
                buf = nullptr;
            }
            استمر;
        }

        إذا ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            استمر;
        }

        إذا (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString tempEscape;
            int32_t offset = 0;
            c = lookahead(tempEscape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = true;
            إذا (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                أرجع DONE;
            }
        }

        اكسر;
    }

    أرجع c;
}

فراغ RuleCharacterIterator::getPos(RuleCharacterIterator::Pos& p) ثابت {
    p.buf = buf;
    p.pos = pos.getIndex();
    p.bufPos = bufPos;
}

فراغ RuleCharacterIterator::setPos(ثابت RuleCharacterIterator::Pos& p) {
    buf = p.buf;
    pos.setIndex(p.pos);
    bufPos = p.bufPos;
}

فراغ RuleCharacterIterator::skipIgnored(int32_t options) {
    إذا ((options & SKIP_WHITESPACE) != 0) {
        لكل (;;) {
            UChar32 a = _current();
            إذا (!PatternProps::isWhiteSpace(a)) اكسر;
            _advance(U16_LENGTH(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& result, int32_t maxLookAhead) ثابت {
    إذا (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    إذا (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, result);
    } وإلا {
        text.extract(pos.getIndex(), maxLookAhead, result);
    }
    أرجع result;
}

فراغ RuleCharacterIterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 RuleCharacterIterator::_current() ثابت {
    إذا (buf != nullptr) {
        أرجع buf->char32At(bufPos);
    } وإلا {
        صحيح i = pos.getIndex();
        أرجع (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

فراغ RuleCharacterIterator::_advance(int32_t count) {
    إذا (buf != nullptr) {
        bufPos += count;
        إذا (bufPos == buf->length()) {
            buf = nullptr;
        }
    } وإلا {
        pos.setIndex(pos.getIndex() + count);
        إذا (pos.getIndex() > text.length()) {
            pos.setIndex(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
