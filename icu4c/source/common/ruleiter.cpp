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
#अन्तर्भाव "ruleiter.h"
#अन्तर्भाव "unicode/parsepos.h"
#अन्तर्भाव "unicode/symtable.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#परिभाषय MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

नियमः_Character_Iterator::नियमः_Character_Iterator(ध्रुव UnicodeString& theText, ध्रुव चिह्नम्_सारणी* theSym,
                      ParsePosition& thePos) :
    text(theText),
    pos(thePos),
    sym(theSym),
    buf(शून्यसूचकः),
    bufPos(0)
{}

UBool नियमः_Character_Iterator::at_अन्तः() ध्रुव {
    प्रत्यागम buf == शून्यसूचकः && pos.get_सूचकांक() == text.length();
}

UChar32 नियमः_Character_Iterator::next(int32_t options, UBool& isEscaped, UError_संहिता& ec) {
    यदि (U_FAILURE(ec)) प्रत्यागम समाप्तम्_2;

    UChar32 c = समाप्तम्_2;
    isEscaped = असत्य;

    प्रति (;;) {
        c = _current();
        _advance(U16_दीर्घता(c));

        यदि (c == चिह्नम्_सारणी::चिह्नम्_REF && buf == शून्यसूचकः &&
            (options & PARSE_VARIABLES) != 0 && sym != शून्यसूचकः) {
            UnicodeString name = sym->parseReference(text, pos, text.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            यदि (name.length() == 0) {
                भङ्ग;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            यदि (buf == शून्यसूचकः) {
                ec = U_UNDEFINED_VARIABLE;
                प्रत्यागम समाप्तम्_2;
            }
            // Handle empty variable value
            यदि (buf->length() == 0) {
                buf = शून्यसूचकः;
            }
            अनुवर्त;
        }

        यदि ((options & SKIP_WHITESPACE) != 0 && आदर्शः_Props::isWhiteSpace(c)) {
            अनुवर्त;
        }

        यदि (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString अस्थायी_Escape;
            int32_t विस्थापनम् = 0;
            c = lookahead(अस्थायी_Escape, MAX_U_NOTATION_LEN).unescapeAt(विस्थापनम्);
            jumpahead(विस्थापनम्);
            isEscaped = सत्य;
            यदि (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                प्रत्यागम समाप्तम्_2;
            }
        }

        भङ्ग;
    }

    प्रत्यागम c;
}

शून्य नियमः_Character_Iterator::getPos(नियमः_Character_Iterator::Pos& p) ध्रुव {
    p.buf = buf;
    p.pos = pos.get_सूचकांक();
    p.bufPos = bufPos;
}

शून्य नियमः_Character_Iterator::समुच्चयः_Pos(ध्रुव नियमः_Character_Iterator::Pos& p) {
    buf = p.buf;
    pos.समुच्चयः_सूचकांक(p.pos);
    bufPos = p.bufPos;
}

शून्य नियमः_Character_Iterator::skipIgnored(int32_t options) {
    यदि ((options & SKIP_WHITESPACE) != 0) {
        प्रति (;;) {
            UChar32 a = _current();
            यदि (!आदर्शः_Props::isWhiteSpace(a)) भङ्ग;
            _advance(U16_दीर्घता(a));
        }
    }
}

UnicodeString& नियमः_Character_Iterator::lookahead(UnicodeString& परिणाम, int32_t maxLookAhead) ध्रुव {
    यदि (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    यदि (buf != शून्यसूचकः) {
        buf->extract(bufPos, maxLookAhead, परिणाम);
    } अन्यथा {
        text.extract(pos.get_सूचकांक(), maxLookAhead, परिणाम);
    }
    प्रत्यागम परिणाम;
}

शून्य नियमः_Character_Iterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 नियमः_Character_Iterator::_current() ध्रुव {
    यदि (buf != शून्यसूचकः) {
        प्रत्यागम buf->char32At(bufPos);
    } अन्यथा {
        पूर्णाङ्क i = pos.get_सूचकांक();
        प्रत्यागम (i < text.length()) ? text.char32At(i) : static_cast<UChar32>(समाप्तम्_2);
    }
}

शून्य नियमः_Character_Iterator::_advance(int32_t count) {
    यदि (buf != शून्यसूचकः) {
        bufPos += count;
        यदि (bufPos == buf->length()) {
            buf = शून्यसूचकः;
        }
    } अन्यथा {
        pos.समुच्चयः_सूचकांक(pos.get_सूचकांक() + count);
        यदि (pos.get_सूचकांक() > text.length()) {
            pos.समुच्चयः_सूचकांक(text.length());
        }
    }
}

U_NAMESPACE_अन्तः

//eof
