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
#ضمن "ruleiter.h"
#ضمن "unicode/parsepos.h"
#ضمن "unicode/symtable.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/utf16.h"
#ضمن "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#تعریف_کرو MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

قاعدہ_Character_Iterator::قاعدہ_Character_Iterator(ثابت UnicodeString& the_متن, ثابت علامت_جدول* theSym,
                      ParsePosition& thePos) :
    متن(the_متن),
    pos(thePos),
    sym(theSym),
    buf(خالی_سوچک),
    bufPos(0)
{}

UBool قاعدہ_Character_Iterator::at_سرا() ثابت {
    أرجع buf == خالی_سوچک && pos.get_اشاریہ() == متن.length();
}

UChar32 قاعدہ_Character_Iterator::next(int32_t options, UBool& isEscaped, UError_کوڈ& ec) {
    إذا (U_FAILURE(ec)) أرجع پورا_2;

    UChar32 c = پورا_2;
    isEscaped = خطأ;

    لكل (;;) {
        c = _current();
        _advance(U16_لمائی(c));

        إذا (c == علامت_جدول::علامت_REF && buf == خالی_سوچک &&
            (options & PARSE_VARIABLES) != 0 && sym != خالی_سوچک) {
            UnicodeString name = sym->parseReference(متن, pos, متن.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            إذا (name.length() == 0) {
                اكسر;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            إذا (buf == خالی_سوچک) {
                ec = U_UNDEFINED_VARIABLE;
                أرجع پورا_2;
            }
            // Handle empty variable value
            إذا (buf->length() == 0) {
                buf = خالی_سوچک;
            }
            استمر;
        }

        إذا ((options & SKIP_WHITESPACE) != 0 && نمونہ_Props::isWhiteSpace(c)) {
            استمر;
        }

        إذا (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString عارضی_Escape;
            int32_t وتھ = 0;
            c = lookahead(عارضی_Escape, MAX_U_NOTATION_LEN).unescapeAt(وتھ);
            jumpahead(وتھ);
            isEscaped = سچ;
            إذا (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                أرجع پورا_2;
            }
        }

        اكسر;
    }

    أرجع c;
}

فراغ قاعدہ_Character_Iterator::getPos(قاعدہ_Character_Iterator::Pos& p) ثابت {
    p.buf = buf;
    p.pos = pos.get_اشاریہ();
    p.bufPos = bufPos;
}

فراغ قاعدہ_Character_Iterator::سیٹ_Pos(ثابت قاعدہ_Character_Iterator::Pos& p) {
    buf = p.buf;
    pos.سیٹ_اشاریہ(p.pos);
    bufPos = p.bufPos;
}

فراغ قاعدہ_Character_Iterator::skipIgnored(int32_t options) {
    إذا ((options & SKIP_WHITESPACE) != 0) {
        لكل (;;) {
            UChar32 a = _current();
            إذا (!نمونہ_Props::isWhiteSpace(a)) اكسر;
            _advance(U16_لمائی(a));
        }
    }
}

UnicodeString& قاعدہ_Character_Iterator::lookahead(UnicodeString& نتیجہ, int32_t maxLookAhead) ثابت {
    إذا (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    إذا (buf != خالی_سوچک) {
        buf->extract(bufPos, maxLookAhead, نتیجہ);
    } وإلا {
        متن.extract(pos.get_اشاریہ(), maxLookAhead, نتیجہ);
    }
    أرجع نتیجہ;
}

فراغ قاعدہ_Character_Iterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 قاعدہ_Character_Iterator::_current() ثابت {
    إذا (buf != خالی_سوچک) {
        أرجع buf->char32At(bufPos);
    } وإلا {
        صحيح i = pos.get_اشاریہ();
        أرجع (i < متن.length()) ? متن.char32At(i) : static_cast<UChar32>(پورا_2);
    }
}

فراغ قاعدہ_Character_Iterator::_advance(int32_t count) {
    إذا (buf != خالی_سوچک) {
        bufPos += count;
        إذا (bufPos == buf->length()) {
            buf = خالی_سوچک;
        }
    } وإلا {
        pos.سیٹ_اشاریہ(pos.get_اشاریہ() + count);
        إذا (pos.get_اشاریہ() > متن.length()) {
            pos.سیٹ_اشاریہ(متن.length());
        }
    }
}

U_NAMESPACE_سرا

//eof
