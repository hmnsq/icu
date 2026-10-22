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
#ਅਨ੍ਤਰ੍ਭਾਵ "ruleiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/parsepos.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/symtable.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

ਨਿਯਮ_Character_Iterator::ਨਿਯਮ_Character_Iterator(ਅਚਲ UnicodeString& the_ਪਾਠ, ਅਚਲ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ* theSym,
                      ParsePosition& thePos) :
    ਪਾਠ(the_ਪਾਠ),
    pos(thePos),
    sym(theSym),
    buf(ਖਾਲੀ_ਸੂਚਕ),
    bufPos(0)
{}

UBool ਨਿਯਮ_Character_Iterator::at_ਸਿਰਾ() ਅਚਲ {
    ਵਾਪਸ buf == ਖਾਲੀ_ਸੂਚਕ && pos.get_ਸੂਚਕਾਂਕ() == ਪਾਠ.length();
}

UChar32 ਨਿਯਮ_Character_Iterator::next(int32_t options, UBool& isEscaped, UError_ਕੋਡ& ec) {
    ਜੇ (U_FAILURE(ec)) ਵਾਪਸ ਪੂਰਾ_2;

    UChar32 c = ਪੂਰਾ_2;
    isEscaped = ਝੂਠ;

    ਲਈ (;;) {
        c = _current();
        _advance(U16_ਲੰਮਾਈ(c));

        ਜੇ (c == ਚਿੰਨ੍ਹ_ਸਾਰਣੀ::ਚਿੰਨ੍ਹ_REF && buf == ਖਾਲੀ_ਸੂਚਕ &&
            (options & PARSE_VARIABLES) != 0 && sym != ਖਾਲੀ_ਸੂਚਕ) {
            UnicodeString name = sym->parseReference(ਪਾਠ, pos, ਪਾਠ.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            ਜੇ (name.length() == 0) {
                ਰੋਕੋ;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            ਜੇ (buf == ਖਾਲੀ_ਸੂਚਕ) {
                ec = U_UNDEFINED_VARIABLE;
                ਵਾਪਸ ਪੂਰਾ_2;
            }
            // Handle empty variable value
            ਜੇ (buf->length() == 0) {
                buf = ਖਾਲੀ_ਸੂਚਕ;
            }
            ਜਾਰੀ;
        }

        ਜੇ ((options & SKIP_WHITESPACE) != 0 && ਨਮੂਨਾ_Props::isWhiteSpace(c)) {
            ਜਾਰੀ;
        }

        ਜੇ (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString ਆਰਜ਼ੀ_Escape;
            int32_t ਵਿੱਥ = 0;
            c = lookahead(ਆਰਜ਼ੀ_Escape, MAX_U_NOTATION_LEN).unescapeAt(ਵਿੱਥ);
            jumpahead(ਵਿੱਥ);
            isEscaped = ਸੱਚ;
            ਜੇ (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                ਵਾਪਸ ਪੂਰਾ_2;
            }
        }

        ਰੋਕੋ;
    }

    ਵਾਪਸ c;
}

ਖਾਲੀ ਨਿਯਮ_Character_Iterator::getPos(ਨਿਯਮ_Character_Iterator::Pos& p) ਅਚਲ {
    p.buf = buf;
    p.pos = pos.get_ਸੂਚਕਾਂਕ();
    p.bufPos = bufPos;
}

ਖਾਲੀ ਨਿਯਮ_Character_Iterator::ਸੈੱਟ_Pos(ਅਚਲ ਨਿਯਮ_Character_Iterator::Pos& p) {
    buf = p.buf;
    pos.ਸੈੱਟ_ਸੂਚਕਾਂਕ(p.pos);
    bufPos = p.bufPos;
}

ਖਾਲੀ ਨਿਯਮ_Character_Iterator::skipIgnored(int32_t options) {
    ਜੇ ((options & SKIP_WHITESPACE) != 0) {
        ਲਈ (;;) {
            UChar32 a = _current();
            ਜੇ (!ਨਮੂਨਾ_Props::isWhiteSpace(a)) ਰੋਕੋ;
            _advance(U16_ਲੰਮਾਈ(a));
        }
    }
}

UnicodeString& ਨਿਯਮ_Character_Iterator::lookahead(UnicodeString& ਨਤੀਜਾ, int32_t maxLookAhead) ਅਚਲ {
    ਜੇ (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    ਜੇ (buf != ਖਾਲੀ_ਸੂਚਕ) {
        buf->extract(bufPos, maxLookAhead, ਨਤੀਜਾ);
    } ਨਹੀਂਤਾਂ {
        ਪਾਠ.extract(pos.get_ਸੂਚਕਾਂਕ(), maxLookAhead, ਨਤੀਜਾ);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ ਨਿਯਮ_Character_Iterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 ਨਿਯਮ_Character_Iterator::_current() ਅਚਲ {
    ਜੇ (buf != ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ buf->char32At(bufPos);
    } ਨਹੀਂਤਾਂ {
        ਪੂਰਨ i = pos.get_ਸੂਚਕਾਂਕ();
        ਵਾਪਸ (i < ਪਾਠ.length()) ? ਪਾਠ.char32At(i) : static_cast<UChar32>(ਪੂਰਾ_2);
    }
}

ਖਾਲੀ ਨਿਯਮ_Character_Iterator::_advance(int32_t count) {
    ਜੇ (buf != ਖਾਲੀ_ਸੂਚਕ) {
        bufPos += count;
        ਜੇ (bufPos == buf->length()) {
            buf = ਖਾਲੀ_ਸੂਚਕ;
        }
    } ਨਹੀਂਤਾਂ {
        pos.ਸੈੱਟ_ਸੂਚਕਾਂਕ(pos.get_ਸੂਚਕਾਂਕ() + count);
        ਜੇ (pos.get_ਸੂਚਕਾਂਕ() > ਪਾਠ.length()) {
            pos.ਸੈੱਟ_ਸੂਚਕਾਂਕ(ਪਾਠ.length());
        }
    }
}

U_NAMESPACE_ਸਿਰਾ

//eof
