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
#شامل "ruleiter.h"
#شامل "unicode/parsepos.h"
#شامل "unicode/symtable.h"
#شامل "unicode/unistr.h"
#شامل "unicode/utf16.h"
#شامل "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#define MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

RuleCharacterIterator::RuleCharacterIterator(ثابت UnicodeString& theText, ثابت نماد_Table* theSym,
                      ParsePosition& thePos) :
    text(theText),
    pos(thePos),
    sym(theSym),
    buf(nullptr),
    bufPos(0)
{}

UBool RuleCharacterIterator::atEnd() ثابت {
    بازگشت buf == nullptr && pos.getIndex() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    اگر (U_FAILURE(ec)) بازگشت DONE;

    UChar32 c = DONE;
    isEscaped = نادرست;

    برای (;;) {
        c = _current();
        _advance(U16_LENGTH(c));

        اگر (c == نماد_Table::نماد_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            اگر (name.length() == 0) {
                بشکن;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            اگر (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                بازگشت DONE;
            }
            // Handle empty variable value
            اگر (buf->length() == 0) {
                buf = nullptr;
            }
            ادامه;
        }

        اگر ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            ادامه;
        }

        اگر (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString tempEscape;
            int32_t offset = 0;
            c = lookahead(tempEscape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = درست;
            اگر (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                بازگشت DONE;
            }
        }

        بشکن;
    }

    بازگشت c;
}

تهی RuleCharacterIterator::getPos(RuleCharacterIterator::Pos& p) ثابت {
    p.buf = buf;
    p.pos = pos.getIndex();
    p.bufPos = bufPos;
}

تهی RuleCharacterIterator::setPos(ثابت RuleCharacterIterator::Pos& p) {
    buf = p.buf;
    pos.setIndex(p.pos);
    bufPos = p.bufPos;
}

تهی RuleCharacterIterator::skipIgnored(int32_t options) {
    اگر ((options & SKIP_WHITESPACE) != 0) {
        برای (;;) {
            UChar32 a = _current();
            اگر (!PatternProps::isWhiteSpace(a)) بشکن;
            _advance(U16_LENGTH(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& result, int32_t maxLookAhead) ثابت {
    اگر (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    اگر (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, result);
    } وگرنه {
        text.extract(pos.getIndex(), maxLookAhead, result);
    }
    بازگشت result;
}

تهی RuleCharacterIterator::jumpahead(int32_t count) {
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
    اگر (buf != nullptr) {
        بازگشت buf->char32At(bufPos);
    } وگرنه {
        صحیح i = pos.getIndex();
        بازگشت (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

تهی RuleCharacterIterator::_advance(int32_t count) {
    اگر (buf != nullptr) {
        bufPos += count;
        اگر (bufPos == buf->length()) {
            buf = nullptr;
        }
    } وگرنه {
        pos.setIndex(pos.getIndex() + count);
        اگر (pos.getIndex() > text.length()) {
            pos.setIndex(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
