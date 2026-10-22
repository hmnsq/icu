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
#ಅನ್ತರ್ಭಾವ "ruleiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/parsepos.h"
#ಅನ್ತರ್ಭಾವ "unicode/symtable.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#define MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

RuleCharacterIterator::RuleCharacterIterator(ಧ್ರುವ UnicodeString& theText, ಧ್ರುವ ಚಿಹ್ನೆ_Table* theSym,
                      ParsePosition& thePos) :
    text(theText),
    pos(thePos),
    sym(theSym),
    buf(nullptr),
    bufPos(0)
{}

UBool RuleCharacterIterator::atEnd() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ buf == nullptr && pos.getIndex() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    ಯದಿ (U_FAILURE(ec)) ಪ್ರತ್ಯಾಗಮ DONE;

    UChar32 c = DONE;
    isEscaped = ಅಸತ್ಯ;

    ಪ್ರತಿ (;;) {
        c = _current();
        _advance(U16_LENGTH(c));

        ಯದಿ (c == ಚಿಹ್ನೆ_Table::ಚಿಹ್ನೆ_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            ಯದಿ (name.length() == 0) {
                ಭಙ್ಗ;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            ಯದಿ (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                ಪ್ರತ್ಯಾಗಮ DONE;
            }
            // Handle empty variable value
            ಯದಿ (buf->length() == 0) {
                buf = nullptr;
            }
            ಅನುವರ್ತ;
        }

        ಯದಿ ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            ಅನುವರ್ತ;
        }

        ಯದಿ (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString tempEscape;
            int32_t offset = 0;
            c = lookahead(tempEscape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = ಸತ್ಯ;
            ಯದಿ (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                ಪ್ರತ್ಯಾಗಮ DONE;
            }
        }

        ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ c;
}

ಶೂನ್ಯ RuleCharacterIterator::getPos(RuleCharacterIterator::Pos& p) ಧ್ರುವ {
    p.buf = buf;
    p.pos = pos.getIndex();
    p.bufPos = bufPos;
}

ಶೂನ್ಯ RuleCharacterIterator::setPos(ಧ್ರುವ RuleCharacterIterator::Pos& p) {
    buf = p.buf;
    pos.setIndex(p.pos);
    bufPos = p.bufPos;
}

ಶೂನ್ಯ RuleCharacterIterator::skipIgnored(int32_t options) {
    ಯದಿ ((options & SKIP_WHITESPACE) != 0) {
        ಪ್ರತಿ (;;) {
            UChar32 a = _current();
            ಯದಿ (!PatternProps::isWhiteSpace(a)) ಭಙ್ಗ;
            _advance(U16_LENGTH(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& result, int32_t maxLookAhead) ಧ್ರುವ {
    ಯದಿ (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    ಯದಿ (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, result);
    } ಅನ್ಯಥಾ {
        text.extract(pos.getIndex(), maxLookAhead, result);
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ RuleCharacterIterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 RuleCharacterIterator::_current() ಧ್ರುವ {
    ಯದಿ (buf != nullptr) {
        ಪ್ರತ್ಯಾಗಮ buf->char32At(bufPos);
    } ಅನ್ಯಥಾ {
        ಪೂರ್ಣಾಙ್ಕ i = pos.getIndex();
        ಪ್ರತ್ಯಾಗಮ (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

ಶೂನ್ಯ RuleCharacterIterator::_advance(int32_t count) {
    ಯದಿ (buf != nullptr) {
        bufPos += count;
        ಯದಿ (bufPos == buf->length()) {
            buf = nullptr;
        }
    } ಅನ್ಯಥಾ {
        pos.setIndex(pos.getIndex() + count);
        ಯದಿ (pos.getIndex() > text.length()) {
            pos.setIndex(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
