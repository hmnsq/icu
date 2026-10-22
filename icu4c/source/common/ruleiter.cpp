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
    ᯀᯮᯞ buf == nullptr && pos.getIndex() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    ᯀᯮᯝ (U_FAILURE(ec)) ᯀᯮᯞ DONE;

    UChar32 c = DONE;
    isEscaped = false;

    for (;;) {
        c = _current();
        _advance(U16_LENGTH(c));

        ᯀᯮᯝ (c == SymbolTable::SYMBOL_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            ᯀᯮᯝ (name.length() == 0) {
                bha_nga;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            ᯀᯮᯝ (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                ᯀᯮᯞ DONE;
            }
            // Handle empty variable value
            ᯀᯮᯝ (buf->length() == 0) {
                buf = nullptr;
            }
            continue;
        }

        ᯀᯮᯝ ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            continue;
        }

        ᯀᯮᯝ (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString tempEscape;
            int32_t offset = 0;
            c = lookahead(tempEscape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = true;
            ᯀᯮᯝ (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                ᯀᯮᯞ DONE;
            }
        }

        bha_nga;
    }

    ᯀᯮᯞ c;
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
    ᯀᯮᯝ ((options & SKIP_WHITESPACE) != 0) {
        for (;;) {
            UChar32 a = _current();
            ᯀᯮᯝ (!PatternProps::isWhiteSpace(a)) bha_nga;
            _advance(U16_LENGTH(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& result, int32_t maxLookAhead) const {
    ᯀᯮᯝ (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    ᯀᯮᯝ (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, result);
    } ᯀᯮᯝᯂᯪ {
        text.extract(pos.getIndex(), maxLookAhead, result);
    }
    ᯀᯮᯞ result;
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
    ᯀᯮᯝ (buf != nullptr) {
        ᯀᯮᯞ buf->char32At(bufPos);
    } ᯀᯮᯝᯂᯪ {
        puur_naa_nka i = pos.getIndex();
        ᯀᯮᯞ (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

void RuleCharacterIterator::_advance(int32_t count) {
    ᯀᯮᯝ (buf != nullptr) {
        bufPos += count;
        ᯀᯮᯝ (bufPos == buf->length()) {
            buf = nullptr;
        }
    } ᯀᯮᯝᯂᯪ {
        pos.setIndex(pos.getIndex() + count);
        ᯀᯮᯝ (pos.getIndex() > text.length()) {
            pos.setIndex(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
