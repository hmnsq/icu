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
#অন্তর্ভাব "ruleiter.h"
#অন্তর্ভাব "unicode/parsepos.h"
#অন্তর্ভাব "unicode/symtable.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#define MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

RuleCharacterIterator::RuleCharacterIterator(ধ্রুব UnicodeString& theText, ধ্রুব প্রতীক_Table* theSym,
                      ParsePosition& thePos) :
    text(theText),
    pos(thePos),
    sym(theSym),
    buf(nullptr),
    bufPos(0)
{}

UBool RuleCharacterIterator::atEnd() ধ্রুব {
    প্রত্যাগম buf == nullptr && pos.getIndex() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    যদি (U_FAILURE(ec)) প্রত্যাগম DONE;

    UChar32 c = DONE;
    isEscaped = অসত্য;

    প্রতি (;;) {
        c = _current();
        _advance(U16_LENGTH(c));

        যদি (c == প্রতীক_Table::প্রতীক_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            যদি (name.length() == 0) {
                ভঙ্গ;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            যদি (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                প্রত্যাগম DONE;
            }
            // Handle empty variable value
            যদি (buf->length() == 0) {
                buf = nullptr;
            }
            অনুবর্ত;
        }

        যদি ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            অনুবর্ত;
        }

        যদি (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString tempEscape;
            int32_t offset = 0;
            c = lookahead(tempEscape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = সত্য;
            যদি (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                প্রত্যাগম DONE;
            }
        }

        ভঙ্গ;
    }

    প্রত্যাগম c;
}

শূন্য RuleCharacterIterator::getPos(RuleCharacterIterator::Pos& p) ধ্রুব {
    p.buf = buf;
    p.pos = pos.getIndex();
    p.bufPos = bufPos;
}

শূন্য RuleCharacterIterator::setPos(ধ্রুব RuleCharacterIterator::Pos& p) {
    buf = p.buf;
    pos.setIndex(p.pos);
    bufPos = p.bufPos;
}

শূন্য RuleCharacterIterator::skipIgnored(int32_t options) {
    যদি ((options & SKIP_WHITESPACE) != 0) {
        প্রতি (;;) {
            UChar32 a = _current();
            যদি (!PatternProps::isWhiteSpace(a)) ভঙ্গ;
            _advance(U16_LENGTH(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& result, int32_t maxLookAhead) ধ্রুব {
    যদি (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    যদি (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, result);
    } অন্যথা {
        text.extract(pos.getIndex(), maxLookAhead, result);
    }
    প্রত্যাগম result;
}

শূন্য RuleCharacterIterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 RuleCharacterIterator::_current() ধ্রুব {
    যদি (buf != nullptr) {
        প্রত্যাগম buf->char32At(bufPos);
    } অন্যথা {
        পূর্ণাঙ্ক i = pos.getIndex();
        প্রত্যাগম (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

শূন্য RuleCharacterIterator::_advance(int32_t count) {
    যদি (buf != nullptr) {
        bufPos += count;
        যদি (bufPos == buf->length()) {
            buf = nullptr;
        }
    } অন্যথা {
        pos.setIndex(pos.getIndex() + count);
        যদি (pos.getIndex() > text.length()) {
            pos.setIndex(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
