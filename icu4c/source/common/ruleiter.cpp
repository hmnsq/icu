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
#समावेश "ruleiter.h"
#समावेश "unicode/parsepos.h"
#समावेश "unicode/symtable.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/utf16.h"
#समावेश "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#घोषणा MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

RuleCharacterIterator::RuleCharacterIterator(स्थिर UnicodeString& theText, स्थिर प्रतीक_Table* theSym,
                      ParsePosition& thePos) :
    text(theText),
    pos(thePos),
    sym(theSym),
    buf(nullptr),
    bufPos(0)
{}

UBool RuleCharacterIterator::atEnd() स्थिर {
    वापस buf == nullptr && pos.get_सूचकांक() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    अगर (U_FAILURE(ec)) वापस DONE;

    UChar32 c = DONE;
    isEscaped = false;

    क्रम (;;) {
        c = _current();
        _advance(U16_लंबाई(c));

        अगर (c == प्रतीक_Table::प्रतीक_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            अगर (name.length() == 0) {
                अवरोध;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            अगर (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                वापस DONE;
            }
            // Handle empty variable value
            अगर (buf->length() == 0) {
                buf = nullptr;
            }
            जारी;
        }

        अगर ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            जारी;
        }

        अगर (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString अस्थायी_Escape;
            int32_t offset = 0;
            c = lookahead(अस्थायी_Escape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = true;
            अगर (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                वापस DONE;
            }
        }

        अवरोध;
    }

    वापस c;
}

व्योम RuleCharacterIterator::getPos(RuleCharacterIterator::Pos& p) स्थिर {
    p.buf = buf;
    p.pos = pos.get_सूचकांक();
    p.bufPos = bufPos;
}

व्योम RuleCharacterIterator::setPos(स्थिर RuleCharacterIterator::Pos& p) {
    buf = p.buf;
    pos.set_सूचकांक(p.pos);
    bufPos = p.bufPos;
}

व्योम RuleCharacterIterator::skipIgnored(int32_t options) {
    अगर ((options & SKIP_WHITESPACE) != 0) {
        क्रम (;;) {
            UChar32 a = _current();
            अगर (!PatternProps::isWhiteSpace(a)) अवरोध;
            _advance(U16_लंबाई(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& परिणाम, int32_t maxLookAhead) स्थिर {
    अगर (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    अगर (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, परिणाम);
    } अन्यथा {
        text.extract(pos.get_सूचकांक(), maxLookAhead, परिणाम);
    }
    वापस परिणाम;
}

व्योम RuleCharacterIterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 RuleCharacterIterator::_current() स्थिर {
    अगर (buf != nullptr) {
        वापस buf->char32At(bufPos);
    } अन्यथा {
        पूर्णांक i = pos.get_सूचकांक();
        वापस (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

व्योम RuleCharacterIterator::_advance(int32_t count) {
    अगर (buf != nullptr) {
        bufPos += count;
        अगर (bufPos == buf->length()) {
            buf = nullptr;
        }
    } अन्यथा {
        pos.set_सूचकांक(pos.get_सूचकांक() + count);
        अगर (pos.get_सूचकांक() > text.length()) {
            pos.set_सूचकांक(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
