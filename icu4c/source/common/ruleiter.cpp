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
#അന്തര്ഭാവ "ruleiter.h"
#അന്തര്ഭാവ "unicode/parsepos.h"
#അന്തര്ഭാവ "unicode/symtable.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#define MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

RuleCharacterIterator::RuleCharacterIterator(സ്ഥിരം UnicodeString& theText, സ്ഥിരം ചിഹ്നം_Table* theSym,
                      ParsePosition& thePos) :
    text(theText),
    pos(thePos),
    sym(theSym),
    buf(nullptr),
    bufPos(0)
{}

UBool RuleCharacterIterator::atEnd() സ്ഥിരം {
    തിരികെ buf == nullptr && pos.getIndex() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    എങ്കിൽ (U_FAILURE(ec)) തിരികെ DONE;

    UChar32 c = DONE;
    isEscaped = അസത്യം;

    വേണ്ടി (;;) {
        c = _current();
        _advance(U16_LENGTH(c));

        എങ്കിൽ (c == ചിഹ്നം_Table::ചിഹ്നം_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            എങ്കിൽ (name.length() == 0) {
                നിർത്തുക;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            എങ്കിൽ (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                തിരികെ DONE;
            }
            // Handle empty variable value
            എങ്കിൽ (buf->length() == 0) {
                buf = nullptr;
            }
            തുടരുക;
        }

        എങ്കിൽ ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            തുടരുക;
        }

        എങ്കിൽ (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString tempEscape;
            int32_t offset = 0;
            c = lookahead(tempEscape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = സത്യം;
            എങ്കിൽ (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                തിരികെ DONE;
            }
        }

        നിർത്തുക;
    }

    തിരികെ c;
}

ശൂന്യം RuleCharacterIterator::getPos(RuleCharacterIterator::Pos& p) സ്ഥിരം {
    p.buf = buf;
    p.pos = pos.getIndex();
    p.bufPos = bufPos;
}

ശൂന്യം RuleCharacterIterator::setPos(സ്ഥിരം RuleCharacterIterator::Pos& p) {
    buf = p.buf;
    pos.setIndex(p.pos);
    bufPos = p.bufPos;
}

ശൂന്യം RuleCharacterIterator::skipIgnored(int32_t options) {
    എങ്കിൽ ((options & SKIP_WHITESPACE) != 0) {
        വേണ്ടി (;;) {
            UChar32 a = _current();
            എങ്കിൽ (!PatternProps::isWhiteSpace(a)) നിർത്തുക;
            _advance(U16_LENGTH(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& result, int32_t maxLookAhead) സ്ഥിരം {
    എങ്കിൽ (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    എങ്കിൽ (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, result);
    } അല്ലെങ്കിൽ {
        text.extract(pos.getIndex(), maxLookAhead, result);
    }
    തിരികെ result;
}

ശൂന്യം RuleCharacterIterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 RuleCharacterIterator::_current() സ്ഥിരം {
    എങ്കിൽ (buf != nullptr) {
        തിരികെ buf->char32At(bufPos);
    } അല്ലെങ്കിൽ {
        പൂർണ്ണസംഖ്യ i = pos.getIndex();
        തിരികെ (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

ശൂന്യം RuleCharacterIterator::_advance(int32_t count) {
    എങ്കിൽ (buf != nullptr) {
        bufPos += count;
        എങ്കിൽ (bufPos == buf->length()) {
            buf = nullptr;
        }
    } അല്ലെങ്കിൽ {
        pos.setIndex(pos.getIndex() + count);
        എങ്കിൽ (pos.getIndex() > text.length()) {
            pos.setIndex(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
