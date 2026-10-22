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
#অন্তর্ভাৱ "ruleiter.h"
#অন্তর্ভাৱ "unicode/parsepos.h"
#অন্তর্ভাৱ "unicode/symtable.h"
#অন্তর্ভাৱ "unicode/unistr.h"
#অন্তর্ভাৱ "unicode/utf16.h"
#অন্তর্ভাৱ "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#define MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

RuleCharacterIterator::RuleCharacterIterator(ধ্রুৱ UnicodeString& theText, ধ্রুৱ প্ৰতীক_Table* theSym,
                      ParsePosition& thePos) :
    text(theText),
    pos(thePos),
    sym(theSym),
    buf(nullptr),
    bufPos(0)
{}

UBool RuleCharacterIterator::atEnd() ধ্রুৱ {
    ঘূৰাই buf == nullptr && pos.getIndex() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    যদি (U_FAILURE(ec)) ঘূৰাই DONE;

    UChar32 c = DONE;
    isEscaped = মিছা;

    বাবে (;;) {
        c = _current();
        _advance(U16_LENGTH(c));

        যদি (c == প্ৰতীক_Table::প্ৰতীক_REF && buf == nullptr &&
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
                ঘূৰাই DONE;
            }
            // Handle empty variable value
            যদি (buf->length() == 0) {
                buf = nullptr;
            }
            অনুৱর্ত;
        }

        যদি ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            অনুৱর্ত;
        }

        যদি (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString tempEscape;
            int32_t offset = 0;
            c = lookahead(tempEscape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = সঁচা;
            যদি (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                ঘূৰাই DONE;
            }
        }

        ভঙ্গ;
    }

    ঘূৰাই c;
}

শূন্য RuleCharacterIterator::getPos(RuleCharacterIterator::Pos& p) ধ্রুৱ {
    p.buf = buf;
    p.pos = pos.getIndex();
    p.bufPos = bufPos;
}

শূন্য RuleCharacterIterator::setPos(ধ্রুৱ RuleCharacterIterator::Pos& p) {
    buf = p.buf;
    pos.setIndex(p.pos);
    bufPos = p.bufPos;
}

শূন্য RuleCharacterIterator::skipIgnored(int32_t options) {
    যদি ((options & SKIP_WHITESPACE) != 0) {
        বাবে (;;) {
            UChar32 a = _current();
            যদি (!PatternProps::isWhiteSpace(a)) ভঙ্গ;
            _advance(U16_LENGTH(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& result, int32_t maxLookAhead) ধ্রুৱ {
    যদি (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    যদি (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, result);
    } অন্যথা {
        text.extract(pos.getIndex(), maxLookAhead, result);
    }
    ঘূৰাই result;
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

UChar32 RuleCharacterIterator::_current() ধ্রুৱ {
    যদি (buf != nullptr) {
        ঘূৰাই buf->char32At(bufPos);
    } অন্যথা {
        পূর্ণাঙ্ক i = pos.getIndex();
        ঘূৰাই (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
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
