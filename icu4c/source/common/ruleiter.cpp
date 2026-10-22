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
#અન્તર્ભાવ "ruleiter.h"
#અન્તર્ભાવ "unicode/parsepos.h"
#અન્તર્ભાવ "unicode/symtable.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#define MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

RuleCharacterIterator::RuleCharacterIterator(અચલ UnicodeString& theText, અચલ પ્રતીક_Table* theSym,
                      ParsePosition& thePos) :
    text(theText),
    pos(thePos),
    sym(theSym),
    buf(nullptr),
    bufPos(0)
{}

UBool RuleCharacterIterator::atEnd() અચલ {
    પાછા buf == nullptr && pos.getIndex() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    જો (U_FAILURE(ec)) પાછા DONE;

    UChar32 c = DONE;
    isEscaped = ખોટું;

    માટે (;;) {
        c = _current();
        _advance(U16_LENGTH(c));

        જો (c == પ્રતીક_Table::પ્રતીક_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            જો (name.length() == 0) {
                તોડો;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            જો (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                પાછા DONE;
            }
            // Handle empty variable value
            જો (buf->length() == 0) {
                buf = nullptr;
            }
            ચાલુ;
        }

        જો ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            ચાલુ;
        }

        જો (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString tempEscape;
            int32_t offset = 0;
            c = lookahead(tempEscape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = સાચું;
            જો (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                પાછા DONE;
            }
        }

        તોડો;
    }

    પાછા c;
}

શૂન્ય RuleCharacterIterator::getPos(RuleCharacterIterator::Pos& p) અચલ {
    p.buf = buf;
    p.pos = pos.getIndex();
    p.bufPos = bufPos;
}

શૂન્ય RuleCharacterIterator::setPos(અચલ RuleCharacterIterator::Pos& p) {
    buf = p.buf;
    pos.setIndex(p.pos);
    bufPos = p.bufPos;
}

શૂન્ય RuleCharacterIterator::skipIgnored(int32_t options) {
    જો ((options & SKIP_WHITESPACE) != 0) {
        માટે (;;) {
            UChar32 a = _current();
            જો (!PatternProps::isWhiteSpace(a)) તોડો;
            _advance(U16_LENGTH(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& result, int32_t maxLookAhead) અચલ {
    જો (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    જો (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, result);
    } નહીંતર {
        text.extract(pos.getIndex(), maxLookAhead, result);
    }
    પાછા result;
}

શૂન્ય RuleCharacterIterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 RuleCharacterIterator::_current() અચલ {
    જો (buf != nullptr) {
        પાછા buf->char32At(bufPos);
    } નહીંતર {
        પૂર્ણાંક i = pos.getIndex();
        પાછા (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

શૂન્ય RuleCharacterIterator::_advance(int32_t count) {
    જો (buf != nullptr) {
        bufPos += count;
        જો (bufPos == buf->length()) {
            buf = nullptr;
        }
    } નહીંતર {
        pos.setIndex(pos.getIndex() + count);
        જો (pos.getIndex() > text.length()) {
            pos.setIndex(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
