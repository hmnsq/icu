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
#وصف_ڪريو MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

قاعدو_Character_Iterator::قاعدو_Character_Iterator(ثابت UnicodeString& the_متن, ثابت نشان_جدول* theSym,
                      ParsePosition& thePos) :
    متن(the_متن),
    pos(thePos),
    sym(theSym),
    buf(خالي_اشارو),
    bufPos(0)
{}

UBool قاعدو_Character_Iterator::at_پڇاڙي() ثابت {
    أرجع buf == خالي_اشارو && pos.get_اشاريو() == متن.length();
}

UChar32 قاعدو_Character_Iterator::next(int32_t options, UBool& isEscaped, UError_ڪوڊ& ec) {
    إذا (U_FAILURE(ec)) أرجع پورو_2;

    UChar32 c = پورو_2;
    isEscaped = خطأ;

    لكل (;;) {
        c = _current();
        _advance(U16_ڊيگهائي(c));

        إذا (c == نشان_جدول::نشان_REF && buf == خالي_اشارو &&
            (options & PARSE_VARIABLES) != 0 && sym != خالي_اشارو) {
            UnicodeString name = sym->parseReference(متن, pos, متن.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            إذا (name.length() == 0) {
                اكسر;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            إذا (buf == خالي_اشارو) {
                ec = U_UNDEFINED_VARIABLE;
                أرجع پورو_2;
            }
            // Handle empty variable value
            إذا (buf->length() == 0) {
                buf = خالي_اشارو;
            }
            استمر;
        }

        إذا ((options & SKIP_WHITESPACE) != 0 && نمونو_Props::isWhiteSpace(c)) {
            استمر;
        }

        إذا (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString عارضي_Escape;
            int32_t وٿي = 0;
            c = lookahead(عارضي_Escape, MAX_U_NOTATION_LEN).unescapeAt(وٿي);
            jumpahead(وٿي);
            isEscaped = سچ;
            إذا (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                أرجع پورو_2;
            }
        }

        اكسر;
    }

    أرجع c;
}

فراغ قاعدو_Character_Iterator::getPos(قاعدو_Character_Iterator::Pos& p) ثابت {
    p.buf = buf;
    p.pos = pos.get_اشاريو();
    p.bufPos = bufPos;
}

فراغ قاعدو_Character_Iterator::سيٽ_Pos(ثابت قاعدو_Character_Iterator::Pos& p) {
    buf = p.buf;
    pos.سيٽ_اشاريو(p.pos);
    bufPos = p.bufPos;
}

فراغ قاعدو_Character_Iterator::skipIgnored(int32_t options) {
    إذا ((options & SKIP_WHITESPACE) != 0) {
        لكل (;;) {
            UChar32 a = _current();
            إذا (!نمونو_Props::isWhiteSpace(a)) اكسر;
            _advance(U16_ڊيگهائي(a));
        }
    }
}

UnicodeString& قاعدو_Character_Iterator::lookahead(UnicodeString& نتيجو, int32_t maxLookAhead) ثابت {
    إذا (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    إذا (buf != خالي_اشارو) {
        buf->extract(bufPos, maxLookAhead, نتيجو);
    } وإلا {
        متن.extract(pos.get_اشاريو(), maxLookAhead, نتيجو);
    }
    أرجع نتيجو;
}

فراغ قاعدو_Character_Iterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 قاعدو_Character_Iterator::_current() ثابت {
    إذا (buf != خالي_اشارو) {
        أرجع buf->char32At(bufPos);
    } وإلا {
        صحيح i = pos.get_اشاريو();
        أرجع (i < متن.length()) ? متن.char32At(i) : static_cast<UChar32>(پورو_2);
    }
}

فراغ قاعدو_Character_Iterator::_advance(int32_t count) {
    إذا (buf != خالي_اشارو) {
        bufPos += count;
        إذا (bufPos == buf->length()) {
            buf = خالي_اشارو;
        }
    } وإلا {
        pos.سيٽ_اشاريو(pos.get_اشاريو() + count);
        إذا (pos.get_اشاريو() > متن.length()) {
            pos.سيٽ_اشاريو(متن.length());
        }
    }
}

U_NAMESPACE_پڇاڙي

//eof
