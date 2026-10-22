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
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

ನಿಯಮ_Character_Iterator::ನಿಯಮ_Character_Iterator(ಸ್ಥಿರಾಂಕ UnicodeString& the_ಪಠ್ಯ, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ* theSym,
                      ParsePosition& thePos) :
    ಪಠ್ಯ(the_ಪಠ್ಯ),
    pos(thePos),
    sym(theSym),
    buf(ಶೂನ್ಯಸೂಚಕ),
    bufPos(0)
{}

UBool ನಿಯಮ_Character_Iterator::at_ಕೊನೆ() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ buf == ಶೂನ್ಯಸೂಚಕ && pos.get_ಸೂಚ್ಯಂಕ() == ಪಠ್ಯ.length();
}

UChar32 ನಿಯಮ_Character_Iterator::next(int32_t options, UBool& isEscaped, UError_ಸಂಕೇತ& ec) {
    ಆದರೆ (U_FAILURE(ec)) ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;

    UChar32 c = ಮುಗಿದ_2;
    isEscaped = ಸುಳ್ಳು;

    ಗಾಗಿ (;;) {
        c = _current();
        _advance(U16_ಉದ್ದಳತೆ(c));

        ಆದರೆ (c == ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ::ಚಿಹ್ನೆ_REF && buf == ಶೂನ್ಯಸೂಚಕ &&
            (options & PARSE_VARIABLES) != 0 && sym != ಶೂನ್ಯಸೂಚಕ) {
            UnicodeString name = sym->parseReference(ಪಠ್ಯ, pos, ಪಠ್ಯ.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            ಆದರೆ (name.length() == 0) {
                ನಿಲ್ಲಿಸು;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            ಆದರೆ (buf == ಶೂನ್ಯಸೂಚಕ) {
                ec = U_UNDEFINED_VARIABLE;
                ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
            }
            // Handle empty variable value
            ಆದರೆ (buf->length() == 0) {
                buf = ಶೂನ್ಯಸೂಚಕ;
            }
            ಮುಂದುವರಿ;
        }

        ಆದರೆ ((options & SKIP_WHITESPACE) != 0 && ನಮೂನೆ_Props::isWhiteSpace(c)) {
            ಮುಂದುವರಿ;
        }

        ಆದರೆ (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString ತಾತ್ಕಾಲಿಕ_Escape;
            int32_t ಸ್ಥಾನಾಂತರ = 0;
            c = lookahead(ತಾತ್ಕಾಲಿಕ_Escape, MAX_U_NOTATION_LEN).unescapeAt(ಸ್ಥಾನಾಂತರ);
            jumpahead(ಸ್ಥಾನಾಂತರ);
            isEscaped = ನಿಜ;
            ಆದರೆ (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
            }
        }

        ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ c;
}

ಶೂನ್ಯ ನಿಯಮ_Character_Iterator::getPos(ನಿಯಮ_Character_Iterator::Pos& p) ಸ್ಥಿರಾಂಕ {
    p.buf = buf;
    p.pos = pos.get_ಸೂಚ್ಯಂಕ();
    p.bufPos = bufPos;
}

ಶೂನ್ಯ ನಿಯಮ_Character_Iterator::ಗಣ_Pos(ಸ್ಥಿರಾಂಕ ನಿಯಮ_Character_Iterator::Pos& p) {
    buf = p.buf;
    pos.ಗಣ_ಸೂಚ್ಯಂಕ(p.pos);
    bufPos = p.bufPos;
}

ಶೂನ್ಯ ನಿಯಮ_Character_Iterator::skipIgnored(int32_t options) {
    ಆದರೆ ((options & SKIP_WHITESPACE) != 0) {
        ಗಾಗಿ (;;) {
            UChar32 a = _current();
            ಆದರೆ (!ನಮೂನೆ_Props::isWhiteSpace(a)) ನಿಲ್ಲಿಸು;
            _advance(U16_ಉದ್ದಳತೆ(a));
        }
    }
}

UnicodeString& ನಿಯಮ_Character_Iterator::lookahead(UnicodeString& ಫಲಿತಾಂಶ, int32_t maxLookAhead) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    ಆದರೆ (buf != ಶೂನ್ಯಸೂಚಕ) {
        buf->extract(bufPos, maxLookAhead, ಫಲಿತಾಂಶ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಪಠ್ಯ.extract(pos.get_ಸೂಚ್ಯಂಕ(), maxLookAhead, ಫಲಿತಾಂಶ);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ ನಿಯಮ_Character_Iterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 ನಿಯಮ_Character_Iterator::_current() ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (buf != ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ buf->char32At(bufPos);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಪೂರ್ಣಾಂಕ i = pos.get_ಸೂಚ್ಯಂಕ();
        ಹಿಂತಿರುಗಿ (i < ಪಠ್ಯ.length()) ? ಪಠ್ಯ.char32At(i) : static_cast<UChar32>(ಮುಗಿದ_2);
    }
}

ಶೂನ್ಯ ನಿಯಮ_Character_Iterator::_advance(int32_t count) {
    ಆದರೆ (buf != ಶೂನ್ಯಸೂಚಕ) {
        bufPos += count;
        ಆದರೆ (bufPos == buf->length()) {
            buf = ಶೂನ್ಯಸೂಚಕ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        pos.ಗಣ_ಸೂಚ್ಯಂಕ(pos.get_ಸೂಚ್ಯಂಕ() + count);
        ಆದರೆ (pos.get_ಸೂಚ್ಯಂಕ() > ಪಠ್ಯ.length()) {
            pos.ಗಣ_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ.length());
        }
    }
}

U_NAMESPACE_ಕೊನೆ

//eof
