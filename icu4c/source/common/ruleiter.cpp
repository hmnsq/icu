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
    ᬯᬮᬶ buf == nullptr && pos.getIndex() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    ᬬᬾᬦ᭄ (U_FAILURE(ec)) ᬯᬮᬶ DONE;

    UChar32 c = DONE;
    isEscaped = false;

    for (;;) {
        c = _current();
        _advance(U16_LENGTH(c));

        ᬬᬾᬦ᭄ (c == SymbolTable::SYMBOL_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            ᬬᬾᬦ᭄ (name.length() == 0) {
                bha_nga;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            ᬬᬾᬦ᭄ (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                ᬯᬮᬶ DONE;
            }
            // Handle empty variable value
            ᬬᬾᬦ᭄ (buf->length() == 0) {
                buf = nullptr;
            }
            continue;
        }

        ᬬᬾᬦ᭄ ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            continue;
        }

        ᬬᬾᬦ᭄ (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString tempEscape;
            int32_t offset = 0;
            c = lookahead(tempEscape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = true;
            ᬬᬾᬦ᭄ (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                ᬯᬮᬶ DONE;
            }
        }

        bha_nga;
    }

    ᬯᬮᬶ c;
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
    ᬬᬾᬦ᭄ ((options & SKIP_WHITESPACE) != 0) {
        for (;;) {
            UChar32 a = _current();
            ᬬᬾᬦ᭄ (!PatternProps::isWhiteSpace(a)) bha_nga;
            _advance(U16_LENGTH(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& result, int32_t maxLookAhead) const {
    ᬬᬾᬦ᭄ (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    ᬬᬾᬦ᭄ (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, result);
    } ᬮᬶᬬ {
        text.extract(pos.getIndex(), maxLookAhead, result);
    }
    ᬯᬮᬶ result;
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
    ᬬᬾᬦ᭄ (buf != nullptr) {
        ᬯᬮᬶ buf->char32At(bufPos);
    } ᬮᬶᬬ {
        puur_naa_nka i = pos.getIndex();
        ᬯᬮᬶ (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

void RuleCharacterIterator::_advance(int32_t count) {
    ᬬᬾᬦ᭄ (buf != nullptr) {
        bufPos += count;
        ᬬᬾᬦ᭄ (bufPos == buf->length()) {
            buf = nullptr;
        }
    } ᬮᬶᬬ {
        pos.setIndex(pos.getIndex() + count);
        ᬬᬾᬦ᭄ (pos.getIndex() > text.length()) {
            pos.setIndex(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
