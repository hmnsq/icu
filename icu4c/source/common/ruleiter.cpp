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
#அந்தர்பாவ "ruleiter.h"
#அந்தர்பாவ "unicode/parsepos.h"
#அந்தர்பாவ "unicode/symtable.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#define MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

RuleCharacterIterator::RuleCharacterIterator(மாறிலி UnicodeString& theText, மாறிலி குறியீடு_Table* theSym,
                      ParsePosition& thePos) :
    text(theText),
    pos(thePos),
    sym(theSym),
    buf(nullptr),
    bufPos(0)
{}

UBool RuleCharacterIterator::atEnd() மாறிலி {
    திரும்பு buf == nullptr && pos.getIndex() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    எனில் (U_FAILURE(ec)) திரும்பு DONE;

    UChar32 c = DONE;
    isEscaped = பொய்;

    க்கு (;;) {
        c = _current();
        _advance(U16_LENGTH(c));

        எனில் (c == குறியீடு_Table::குறியீடு_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            எனில் (name.length() == 0) {
                நிறுத்து;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            எனில் (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                திரும்பு DONE;
            }
            // Handle empty variable value
            எனில் (buf->length() == 0) {
                buf = nullptr;
            }
            தொடர்;
        }

        எனில் ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            தொடர்;
        }

        எனில் (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString tempEscape;
            int32_t offset = 0;
            c = lookahead(tempEscape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = உண்மை;
            எனில் (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                திரும்பு DONE;
            }
        }

        நிறுத்து;
    }

    திரும்பு c;
}

வெறுமை RuleCharacterIterator::getPos(RuleCharacterIterator::Pos& p) மாறிலி {
    p.buf = buf;
    p.pos = pos.getIndex();
    p.bufPos = bufPos;
}

வெறுமை RuleCharacterIterator::setPos(மாறிலி RuleCharacterIterator::Pos& p) {
    buf = p.buf;
    pos.setIndex(p.pos);
    bufPos = p.bufPos;
}

வெறுமை RuleCharacterIterator::skipIgnored(int32_t options) {
    எனில் ((options & SKIP_WHITESPACE) != 0) {
        க்கு (;;) {
            UChar32 a = _current();
            எனில் (!PatternProps::isWhiteSpace(a)) நிறுத்து;
            _advance(U16_LENGTH(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& result, int32_t maxLookAhead) மாறிலி {
    எனில் (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    எனில் (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, result);
    } இல்லையெனில் {
        text.extract(pos.getIndex(), maxLookAhead, result);
    }
    திரும்பு result;
}

வெறுமை RuleCharacterIterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 RuleCharacterIterator::_current() மாறிலி {
    எனில் (buf != nullptr) {
        திரும்பு buf->char32At(bufPos);
    } இல்லையெனில் {
        முழு i = pos.getIndex();
        திரும்பு (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

வெறுமை RuleCharacterIterator::_advance(int32_t count) {
    எனில் (buf != nullptr) {
        bufPos += count;
        எனில் (bufPos == buf->length()) {
            buf = nullptr;
        }
    } இல்லையெனில் {
        pos.setIndex(pos.getIndex() + count);
        எனில் (pos.getIndex() > text.length()) {
            pos.setIndex(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
