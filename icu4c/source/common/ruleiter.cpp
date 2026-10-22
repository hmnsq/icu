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
#تعریف_کرو MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

قاعدہ_Character_Iterator::قاعدہ_Character_Iterator(ثابت UnicodeString& the_متن, ثابت علامت_جدول* theSym,
                      ParsePosition& thePos) :
    متن(the_متن),
    pos(thePos),
    sym(theSym),
    buf(خالی_اشاریہ),
    bufPos(0)
{}

UBool قاعدہ_Character_Iterator::at_انتہا() ثابت {
    واپس buf == خالی_اشاریہ && pos.get_اشاریہ() == متن.length();
}

UChar32 قاعدہ_Character_Iterator::next(int32_t options, UBool& isEscaped, UError_رمز& ec) {
    اگر (U_FAILURE(ec)) واپس مکمل_2;

    UChar32 c = مکمل_2;
    isEscaped = جھوٹ;

    برائے (;;) {
        c = _current();
        _advance(U16_لمبائی(c));

        اگر (c == علامت_جدول::علامت_REF && buf == خالی_اشاریہ &&
            (options & PARSE_VARIABLES) != 0 && sym != خالی_اشاریہ) {
            UnicodeString name = sym->parseReference(متن, pos, متن.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            اگر (name.length() == 0) {
                توڑو;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            اگر (buf == خالی_اشاریہ) {
                ec = U_UNDEFINED_VARIABLE;
                واپس مکمل_2;
            }
            // Handle empty variable value
            اگر (buf->length() == 0) {
                buf = خالی_اشاریہ;
            }
            جاری;
        }

        اگر ((options & SKIP_WHITESPACE) != 0 && نمونہ_Props::isWhiteSpace(c)) {
            جاری;
        }

        اگر (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString عارضی_Escape;
            int32_t انحراف = 0;
            c = lookahead(عارضی_Escape, MAX_U_NOTATION_LEN).unescapeAt(انحراف);
            jumpahead(انحراف);
            isEscaped = سچ;
            اگر (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                واپس مکمل_2;
            }
        }

        توڑو;
    }

    واپس c;
}

خالی قاعدہ_Character_Iterator::getPos(قاعدہ_Character_Iterator::Pos& p) ثابت {
    p.buf = buf;
    p.pos = pos.get_اشاریہ();
    p.bufPos = bufPos;
}

خالی قاعدہ_Character_Iterator::سیٹ_Pos(ثابت قاعدہ_Character_Iterator::Pos& p) {
    buf = p.buf;
    pos.سیٹ_اشاریہ(p.pos);
    bufPos = p.bufPos;
}

خالی قاعدہ_Character_Iterator::skipIgnored(int32_t options) {
    اگر ((options & SKIP_WHITESPACE) != 0) {
        برائے (;;) {
            UChar32 a = _current();
            اگر (!نمونہ_Props::isWhiteSpace(a)) توڑو;
            _advance(U16_لمبائی(a));
        }
    }
}

UnicodeString& قاعدہ_Character_Iterator::lookahead(UnicodeString& نتیجہ, int32_t maxLookAhead) ثابت {
    اگر (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    اگر (buf != خالی_اشاریہ) {
        buf->extract(bufPos, maxLookAhead, نتیجہ);
    } ورنہ {
        متن.extract(pos.get_اشاریہ(), maxLookAhead, نتیجہ);
    }
    واپس نتیجہ;
}

خالی قاعدہ_Character_Iterator::jumpahead(int32_t count) {
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
    اگر (buf != خالی_اشاریہ) {
        واپس buf->char32At(bufPos);
    } ورنہ {
        صحیح i = pos.get_اشاریہ();
        واپس (i < متن.length()) ? متن.char32At(i) : static_cast<UChar32>(مکمل_2);
    }
}

خالی قاعدہ_Character_Iterator::_advance(int32_t count) {
    اگر (buf != خالی_اشاریہ) {
        bufPos += count;
        اگر (bufPos == buf->length()) {
            buf = خالی_اشاریہ;
        }
    } ورنہ {
        pos.سیٹ_اشاریہ(pos.get_اشاریہ() + count);
        اگر (pos.get_اشاریہ() > متن.length()) {
            pos.سیٹ_اشاریہ(متن.length());
        }
    }
}

U_NAMESPACE_انتہا

//eof
