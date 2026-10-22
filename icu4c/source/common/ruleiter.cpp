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
    ཕྱིར buf == nullptr && pos.getIndex() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    གལ་ཏེ (U_FAILURE(ec)) ཕྱིར DONE;

    UChar32 c = DONE;
    isEscaped = false;

    ཆེད་དུ (;;) {
        c = _current();
        _advance(U16_LENGTH(c));

        གལ་ཏེ (c == SymbolTable::SYMBOL_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            གལ་ཏེ (name.length() == 0) {
                bha_nga;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            གལ་ཏེ (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                ཕྱིར DONE;
            }
            // Handle empty variable value
            གལ་ཏེ (buf->length() == 0) {
                buf = nullptr;
            }
            continue;
        }

        གལ་ཏེ ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            continue;
        }

        གལ་ཏེ (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString tempEscape;
            int32_t offset = 0;
            c = lookahead(tempEscape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = true;
            གལ་ཏེ (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                ཕྱིར DONE;
            }
        }

        bha_nga;
    }

    ཕྱིར c;
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
    གལ་ཏེ ((options & SKIP_WHITESPACE) != 0) {
        ཆེད་དུ (;;) {
            UChar32 a = _current();
            གལ་ཏེ (!PatternProps::isWhiteSpace(a)) bha_nga;
            _advance(U16_LENGTH(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& result, int32_t maxLookAhead) const {
    གལ་ཏེ (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    གལ་ཏེ (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, result);
    } དེ་མིན {
        text.extract(pos.getIndex(), maxLookAhead, result);
    }
    ཕྱིར result;
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
    གལ་ཏེ (buf != nullptr) {
        ཕྱིར buf->char32At(bufPos);
    } དེ་མིན {
        ཧྲིལ i = pos.getIndex();
        ཕྱིར (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

void RuleCharacterIterator::_advance(int32_t count) {
    གལ་ཏེ (buf != nullptr) {
        bufPos += count;
        གལ་ཏེ (bufPos == buf->length()) {
            buf = nullptr;
        }
    } དེ་མིན {
        pos.setIndex(pos.getIndex() + count);
        གལ་ཏེ (pos.getIndex() > text.length()) {
            pos.setIndex(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
