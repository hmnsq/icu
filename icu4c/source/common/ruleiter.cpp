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
#تعریف_کن MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

قاعده_Character_Iterator::قاعده_Character_Iterator(ثابت UnicodeString& the_متن, ثابت نماد_جدول* theSym,
                      ParsePosition& thePos) :
    متن(the_متن),
    pos(thePos),
    sym(theSym),
    buf(اشاره_تهی),
    bufPos(0)
{}

UBool قاعده_Character_Iterator::at_انتها() ثابت {
    أرجع buf == اشاره_تهی && pos.get_شاخص() == متن.length();
}

UChar32 قاعده_Character_Iterator::next(int32_t options, UBool& isEscaped, UError_کد& ec) {
    إذا (U_FAILURE(ec)) أرجع انجامشده_2;

    UChar32 c = انجامشده_2;
    isEscaped = خطأ;

    لكل (;;) {
        c = _current();
        _advance(U16_درازی(c));

        إذا (c == نماد_جدول::نماد_REF && buf == اشاره_تهی &&
            (options & PARSE_VARIABLES) != 0 && sym != اشاره_تهی) {
            UnicodeString name = sym->parseReference(متن, pos, متن.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            إذا (name.length() == 0) {
                اكسر;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            إذا (buf == اشاره_تهی) {
                ec = U_UNDEFINED_VARIABLE;
                أرجع انجامشده_2;
            }
            // Handle empty variable value
            إذا (buf->length() == 0) {
                buf = اشاره_تهی;
            }
            استمر;
        }

        إذا ((options & SKIP_WHITESPACE) != 0 && الگو_Props::isWhiteSpace(c)) {
            استمر;
        }

        إذا (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString موقت_Escape;
            int32_t جابه_جایی = 0;
            c = lookahead(موقت_Escape, MAX_U_NOTATION_LEN).unescapeAt(جابه_جایی);
            jumpahead(جابه_جایی);
            isEscaped = درست;
            إذا (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                أرجع انجامشده_2;
            }
        }

        اكسر;
    }

    أرجع c;
}

فراغ قاعده_Character_Iterator::getPos(قاعده_Character_Iterator::Pos& p) ثابت {
    p.buf = buf;
    p.pos = pos.get_شاخص();
    p.bufPos = bufPos;
}

فراغ قاعده_Character_Iterator::مجموعه_Pos(ثابت قاعده_Character_Iterator::Pos& p) {
    buf = p.buf;
    pos.مجموعه_شاخص(p.pos);
    bufPos = p.bufPos;
}

فراغ قاعده_Character_Iterator::skipIgnored(int32_t options) {
    إذا ((options & SKIP_WHITESPACE) != 0) {
        لكل (;;) {
            UChar32 a = _current();
            إذا (!الگو_Props::isWhiteSpace(a)) اكسر;
            _advance(U16_درازی(a));
        }
    }
}

UnicodeString& قاعده_Character_Iterator::lookahead(UnicodeString& نتیجه, int32_t maxLookAhead) ثابت {
    إذا (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    إذا (buf != اشاره_تهی) {
        buf->extract(bufPos, maxLookAhead, نتیجه);
    } وإلا {
        متن.extract(pos.get_شاخص(), maxLookAhead, نتیجه);
    }
    أرجع نتیجه;
}

فراغ قاعده_Character_Iterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 قاعده_Character_Iterator::_current() ثابت {
    إذا (buf != اشاره_تهی) {
        أرجع buf->char32At(bufPos);
    } وإلا {
        صحيح i = pos.get_شاخص();
        أرجع (i < متن.length()) ? متن.char32At(i) : static_cast<UChar32>(انجامشده_2);
    }
}

فراغ قاعده_Character_Iterator::_advance(int32_t count) {
    إذا (buf != اشاره_تهی) {
        bufPos += count;
        إذا (bufPos == buf->length()) {
            buf = اشاره_تهی;
        }
    } وإلا {
        pos.مجموعه_شاخص(pos.get_شاخص() + count);
        إذا (pos.get_شاخص() > متن.length()) {
            pos.مجموعه_شاخص(متن.length());
        }
    }
}

U_NAMESPACE_انتها

//eof
