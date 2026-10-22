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

RuleCharacterIterator::RuleCharacterIterator(ثابت UnicodeString& theText, ثابت SymbolTable* theSym,
                      ParsePosition& thePos) :
    text(theText),
    pos(thePos),
    sym(theSym),
    buf(nullptr),
    bufPos(0)
{}

UBool RuleCharacterIterator::atEnd() ثابت {
    ܦܢܐ buf == nullptr && pos.getIndex() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    ܐܢ (U_FAILURE(ec)) ܦܢܐ DONE;

    UChar32 c = DONE;
    isEscaped = ܕܓܠܐ;

    لكل (;;) {
        c = _current();
        _advance(U16_LENGTH(c));

        ܐܢ (c == SymbolTable::SYMBOL_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            ܐܢ (name.length() == 0) {
                اكسر;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            ܐܢ (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                ܦܢܐ DONE;
            }
            // Handle empty variable value
            ܐܢ (buf->length() == 0) {
                buf = nullptr;
            }
            استمر;
        }

        ܐܢ ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            استمر;
        }

        ܐܢ (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString tempEscape;
            int32_t offset = 0;
            c = lookahead(tempEscape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = ܫܪܪܐ;
            ܐܢ (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                ܦܢܐ DONE;
            }
        }

        اكسر;
    }

    ܦܢܐ c;
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
    ܐܢ ((options & SKIP_WHITESPACE) != 0) {
        لكل (;;) {
            UChar32 a = _current();
            ܐܢ (!PatternProps::isWhiteSpace(a)) اكسر;
            _advance(U16_LENGTH(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& result, int32_t maxLookAhead) ثابت {
    ܐܢ (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    ܐܢ (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, result);
    } ܐܚܪܢܐ {
        text.extract(pos.getIndex(), maxLookAhead, result);
    }
    ܦܢܐ result;
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
    ܐܢ (buf != nullptr) {
        ܦܢܐ buf->char32At(bufPos);
    } ܐܚܪܢܐ {
        صحيح i = pos.getIndex();
        ܦܢܐ (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

فراغ RuleCharacterIterator::_advance(int32_t count) {
    ܐܢ (buf != nullptr) {
        bufPos += count;
        ܐܢ (bufPos == buf->length()) {
            buf = nullptr;
        }
    } ܐܚܪܢܐ {
        pos.setIndex(pos.getIndex() + count);
        ܐܢ (pos.getIndex() > text.length()) {
            pos.setIndex(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
