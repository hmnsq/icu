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
#تعریف_کړه MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

قاعده_Character_Iterator::قاعده_Character_Iterator(ثابت UnicodeString& the_متن, ثابت نښه_جدول* theSym,
                      ParsePosition& thePos) :
    متن(the_متن),
    pos(thePos),
    sym(theSym),
    buf(تش_اشاره),
    bufPos(0)
{}

UBool قاعده_Character_Iterator::at_پای_ټکی() ثابت {
    أرجع buf == تش_اشاره && pos.get_شاخص() == متن.length();
}

UChar32 قاعده_Character_Iterator::next(int32_t options, UBool& isEscaped, UError_کوډ& ec) {
    إذا (U_FAILURE(ec)) أرجع بشپړ_2;

    UChar32 c = بشپړ_2;
    isEscaped = خطأ;

    لكل (;;) {
        c = _current();
        _advance(U16_اوږدتیا(c));

        إذا (c == نښه_جدول::نښه_REF && buf == تش_اشاره &&
            (options & PARSE_VARIABLES) != 0 && sym != تش_اشاره) {
            UnicodeString name = sym->parseReference(متن, pos, متن.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            إذا (name.length() == 0) {
                اكسر;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            إذا (buf == تش_اشاره) {
                ec = U_UNDEFINED_VARIABLE;
                أرجع بشپړ_2;
            }
            // Handle empty variable value
            إذا (buf->length() == 0) {
                buf = تش_اشاره;
            }
            استمر;
        }

        إذا ((options & SKIP_WHITESPACE) != 0 && نمونه_Props::isWhiteSpace(c)) {
            استمر;
        }

        إذا (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString لنډمهاله_Escape;
            int32_t دننه_واټن = 0;
            c = lookahead(لنډمهاله_Escape, MAX_U_NOTATION_LEN).unescapeAt(دننه_واټن);
            jumpahead(دننه_واټن);
            isEscaped = ریښتیا;
            إذا (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                أرجع بشپړ_2;
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

فراغ قاعده_Character_Iterator::سېټ_Pos(ثابت قاعده_Character_Iterator::Pos& p) {
    buf = p.buf;
    pos.سېټ_شاخص(p.pos);
    bufPos = p.bufPos;
}

فراغ قاعده_Character_Iterator::skipIgnored(int32_t options) {
    إذا ((options & SKIP_WHITESPACE) != 0) {
        لكل (;;) {
            UChar32 a = _current();
            إذا (!نمونه_Props::isWhiteSpace(a)) اكسر;
            _advance(U16_اوږدتیا(a));
        }
    }
}

UnicodeString& قاعده_Character_Iterator::lookahead(UnicodeString& پایله, int32_t maxLookAhead) ثابت {
    إذا (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    إذا (buf != تش_اشاره) {
        buf->extract(bufPos, maxLookAhead, پایله);
    } وإلا {
        متن.extract(pos.get_شاخص(), maxLookAhead, پایله);
    }
    أرجع پایله;
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
    إذا (buf != تش_اشاره) {
        أرجع buf->char32At(bufPos);
    } وإلا {
        صحيح i = pos.get_شاخص();
        أرجع (i < متن.length()) ? متن.char32At(i) : static_cast<UChar32>(بشپړ_2);
    }
}

فراغ قاعده_Character_Iterator::_advance(int32_t count) {
    إذا (buf != تش_اشاره) {
        bufPos += count;
        إذا (bufPos == buf->length()) {
            buf = تش_اشاره;
        }
    } وإلا {
        pos.سېټ_شاخص(pos.get_شاخص() + count);
        إذا (pos.get_شاخص() > متن.length()) {
            pos.سېټ_شاخص(متن.length());
        }
    }
}

U_NAMESPACE_پای_ټکی

//eof
