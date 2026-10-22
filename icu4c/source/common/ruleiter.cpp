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

RuleCharacterIterator::RuleCharacterIterator(ধ্রুবক UnicodeString& theText, ধ্রুবক প্রতীক_Table* theSym,
                      ParsePosition& thePos) :
    text(theText),
    pos(thePos),
    sym(theSym),
    buf(nullptr),
    bufPos(0)
{}

UBool RuleCharacterIterator::atEnd() ধ্রুবক {
    ফেরত buf == nullptr && pos.get_সূচক() == text.length();
}

UChar32 RuleCharacterIterator::next(int32_t options, UBool& isEscaped, UErrorCode& ec) {
    যদি (U_FAILURE(ec)) ফেরত DONE;

    UChar32 c = DONE;
    isEscaped = মিথ্যা;

    জন্য (;;) {
        c = _current();
        _advance(U16_দৈর্ঘ্য(c));

        যদি (c == প্রতীক_Table::প্রতীক_REF && buf == nullptr &&
            (options & PARSE_VARIABLES) != 0 && sym != nullptr) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            যদি (name.length() == 0) {
                বিরতি;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            যদি (buf == nullptr) {
                ec = U_UNDEFINED_VARIABLE;
                ফেরত DONE;
            }
            // Handle empty variable value
            যদি (buf->length() == 0) {
                buf = nullptr;
            }
            চালু;
        }

        যদি ((options & SKIP_WHITESPACE) != 0 && PatternProps::isWhiteSpace(c)) {
            চালু;
        }

        যদি (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString অস্থায়ী_Escape;
            int32_t offset = 0;
            c = lookahead(অস্থায়ী_Escape, MAX_U_NOTATION_LEN).unescapeAt(offset);
            jumpahead(offset);
            isEscaped = সত্য;
            যদি (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                ফেরত DONE;
            }
        }

        বিরতি;
    }

    ফেরত c;
}

শূন্য RuleCharacterIterator::getPos(RuleCharacterIterator::Pos& p) ধ্রুবক {
    p.buf = buf;
    p.pos = pos.get_সূচক();
    p.bufPos = bufPos;
}

শূন্য RuleCharacterIterator::setPos(ধ্রুবক RuleCharacterIterator::Pos& p) {
    buf = p.buf;
    pos.set_সূচক(p.pos);
    bufPos = p.bufPos;
}

শূন্য RuleCharacterIterator::skipIgnored(int32_t options) {
    যদি ((options & SKIP_WHITESPACE) != 0) {
        জন্য (;;) {
            UChar32 a = _current();
            যদি (!PatternProps::isWhiteSpace(a)) বিরতি;
            _advance(U16_দৈর্ঘ্য(a));
        }
    }
}

UnicodeString& RuleCharacterIterator::lookahead(UnicodeString& ফলাফল, int32_t maxLookAhead) ধ্রুবক {
    যদি (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    যদি (buf != nullptr) {
        buf->extract(bufPos, maxLookAhead, ফলাফল);
    } অন্যথা {
        text.extract(pos.get_সূচক(), maxLookAhead, ফলাফল);
    }
    ফেরত ফলাফল;
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

UChar32 RuleCharacterIterator::_current() ধ্রুবক {
    যদি (buf != nullptr) {
        ফেরত buf->char32At(bufPos);
    } অন্যথা {
        পূর্ণসংখ্যা i = pos.get_সূচক();
        ফেরত (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(DONE);
    }
}

শূন্য RuleCharacterIterator::_advance(int32_t count) {
    যদি (buf != nullptr) {
        bufPos += count;
        যদি (bufPos == buf->length()) {
            buf = nullptr;
        }
    } অন্যথা {
        pos.set_সূচক(pos.get_সূচক() + count);
        যদি (pos.get_সূচক() > text.length()) {
            pos.set_সূচক(text.length());
        }
    }
}

U_NAMESPACE_END

//eof
