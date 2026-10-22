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
#include "ruleiter.h"
#include "unicode/parsepos.h"
#include "unicode/symtable.h"
#include "unicode/unistr.h"
#include "unicode/utf16.h"
#include "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#define MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

RuleCharacterIterator::RuleCharacterIterator(const UnicodeString& theText, const SymbolTable* theSym,
                      ParsePosition& thePos) :
    text(theText),
    pos(thePos),
    sym(theSym),
    buf(nullptr),
    bufPos(0)
{}

UBool RuleCharacterIterator::atEnd() const {
    вернуть buf == nullptr && pos.getIndex() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    если (U_FAILURE(ec)) вернуть DONE;

    UChar32 c = DONE;
    isEscaped = false;

    для (;;) {
        c = _current();
        _advance(U16_LENGTH(c));

        если (c == SymbolTable::SYMBOL_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            если (name.length() == 0) {
                break;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            если (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                вернуть DONE;
            }
            // Handle empty variable value
            если (buf->length() == 0) {
                buf = nullptr;
            }
            continue;
        }

        если ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            continue;
        }

        если (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString tempEscape;
            int32_t offset = 0;
            c = lookahead(tempEscape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = true;
            если (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                вернуть DONE;
            }
        }

        break;
    }

    вернуть c;
}

void RuleCharacterIterator::getPos(RuleCharacterIterator::Pos& p) const {
    p.buf = buf;
    p.pos = pos.getIndex();
    p.bufPos = bufPos;
}

void RuleCharacterIterator::setPos(const RuleCharacterIterator::Pos& p) {
    buf = p.buf;
    pos.setIndex(p.pos);
    bufPos = p.bufPos;
}

void RuleCharacterIterator::skipIgnored(int32_t options) {
    если ((options & SKIP_WHITESPACE) != 0) {
        для (;;) {
            UChar32 a = _current();
            если (!PatternProps::isWhiteSpace(a)) break;
            _advance(U16_LENGTH(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& result, int32_t maxLookAhead) const {
    если (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    если (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, result);
    } иначе {
        text.extract(pos.getIndex(), maxLookAhead, result);
    }
    вернуть result;
}

void RuleCharacterIterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 RuleCharacterIterator::_current() const {
    если (buf != nullptr) {
        вернуть buf->char32At(bufPos);
    } иначе {
        целое i = pos.getIndex();
        вернуть (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

void RuleCharacterIterator::_advance(int32_t count) {
    если (buf != nullptr) {
        bufPos += count;
        если (bufPos == buf->length()) {
            buf = nullptr;
        }
    } иначе {
        pos.setIndex(pos.getIndex() + count);
        если (pos.getIndex() > text.length()) {
            pos.setIndex(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
