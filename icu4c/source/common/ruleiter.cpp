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
#अन्तर्भाव "ruleiter.h"
#अन्तर्भाव "unicode/parsepos.h"
#अन्तर्भाव "unicode/symtable.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#define MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

RuleCharacterIterator::RuleCharacterIterator(स्थिरांक UnicodeString& theText, स्थिरांक चिन्ह_Table* theSym,
                      ParsePosition& thePos) :
    text(theText),
    pos(thePos),
    sym(theSym),
    buf(nullptr),
    bufPos(0)
{}

UBool RuleCharacterIterator::atEnd() स्थिरांक {
    परत buf == nullptr && pos.getIndex() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    जर (U_FAILURE(ec)) परत DONE;

    UChar32 c = DONE;
    isEscaped = असत्य;

    साठी (;;) {
        c = _current();
        _advance(U16_LENGTH(c));

        जर (c == चिन्ह_Table::चिन्ह_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            जर (name.length() == 0) {
                थांबा;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            जर (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                परत DONE;
            }
            // Handle empty variable value
            जर (buf->length() == 0) {
                buf = nullptr;
            }
            सुरूठेवा;
        }

        जर ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            सुरूठेवा;
        }

        जर (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString tempEscape;
            int32_t offset = 0;
            c = lookahead(tempEscape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = सत्य;
            जर (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                परत DONE;
            }
        }

        थांबा;
    }

    परत c;
}

शून्य RuleCharacterIterator::getPos(RuleCharacterIterator::Pos& p) स्थिरांक {
    p.buf = buf;
    p.pos = pos.getIndex();
    p.bufPos = bufPos;
}

शून्य RuleCharacterIterator::setPos(स्थिरांक RuleCharacterIterator::Pos& p) {
    buf = p.buf;
    pos.setIndex(p.pos);
    bufPos = p.bufPos;
}

शून्य RuleCharacterIterator::skipIgnored(int32_t options) {
    जर ((options & SKIP_WHITESPACE) != 0) {
        साठी (;;) {
            UChar32 a = _current();
            जर (!PatternProps::isWhiteSpace(a)) थांबा;
            _advance(U16_LENGTH(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& result, int32_t maxLookAhead) स्थिरांक {
    जर (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    जर (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, result);
    } अन्यथा {
        text.extract(pos.getIndex(), maxLookAhead, result);
    }
    परत result;
}

शून्य RuleCharacterIterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 RuleCharacterIterator::_current() स्थिरांक {
    जर (buf != nullptr) {
        परत buf->char32At(bufPos);
    } अन्यथा {
        पूर्णांक i = pos.getIndex();
        परत (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

शून्य RuleCharacterIterator::_advance(int32_t count) {
    जर (buf != nullptr) {
        bufPos += count;
        जर (bufPos == buf->length()) {
            buf = nullptr;
        }
    } अन्यथा {
        pos.setIndex(pos.getIndex() + count);
        जर (pos.getIndex() > text.length()) {
            pos.setIndex(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
