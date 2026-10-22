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
#అన్తర్భావ "ruleiter.h"
#అన్తర్భావ "unicode/parsepos.h"
#అన్తర్భావ "unicode/symtable.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "patternprops.h"

/* \U87654321 or \ud800\udc00 */
#నిర్వచించు MAX_U_NOTATION_LEN 12

U_NAMESPACE_BEGIN

నియమం_Character_Iterator::నియమం_Character_Iterator(స్థిరరాశి UnicodeString& the_పాఠ్యం, స్థిరరాశి చిహ్నం_పట్టిక* theSym,
                      ParsePosition& thePos) :
    పాఠ్యం(the_పాఠ్యం),
    pos(thePos),
    sym(theSym),
    buf(శూన్యసూచిక),
    bufPos(0)
{}

UBool నియమం_Character_Iterator::at_చివర() స్థిరరాశి {
    తిరిగి buf == శూన్యసూచిక && pos.get_సూచీ() == పాఠ్యం.length();
}

UChar32 నియమం_Character_Iterator::next(int32_t options, UBool& isEscaped, UError_సంకేతం& ec) {
    ఒకవేళ (U_FAILURE(ec)) తిరిగి పూర్తి_2;

    UChar32 c = పూర్తి_2;
    isEscaped = అబద్ధం;

    కొరకు (;;) {
        c = _current();
        _advance(U16_నిడివి(c));

        ఒకవేళ (c == చిహ్నం_పట్టిక::చిహ్నం_REF && buf == శూన్యసూచిక &&
            (options & PARSE_VARIABLES) != 0 && sym != శూన్యసూచిక) {
            UnicodeString name = sym->parseReference(పాఠ్యం, pos, పాఠ్యం.length());
            // If name is empty there was an isolated SYMBOL_REF;
            // return it.  Caller must be prepared for this.
            ఒకవేళ (name.length() == 0) {
                ఆపు;
            }
            bufPos = 0;
            buf = sym->lookup(name);
            ఒకవేళ (buf == శూన్యసూచిక) {
                ec = U_UNDEFINED_VARIABLE;
                తిరిగి పూర్తి_2;
            }
            // Handle empty variable value
            ఒకవేళ (buf->length() == 0) {
                buf = శూన్యసూచిక;
            }
            కొనసాగు;
        }

        ఒకవేళ ((options & SKIP_WHITESPACE) != 0 && నమూనా_Props::isWhiteSpace(c)) {
            కొనసాగు;
        }

        ఒకవేళ (c == 0x5C /*'\\'*/ && (options & PARSE_ESCAPES) != 0) {
            UnicodeString తాత్కాలిక_Escape;
            int32_t స్థానభ్రంశం = 0;
            c = lookahead(తాత్కాలిక_Escape, MAX_U_NOTATION_LEN).unescapeAt(స్థానభ్రంశం);
            jumpahead(స్థానభ్రంశం);
            isEscaped = నిజం;
            ఒకవేళ (c < 0) {
                ec = U_MALFORMED_UNICODE_ESCAPE;
                తిరిగి పూర్తి_2;
            }
        }

        ఆపు;
    }

    తిరిగి c;
}

శూన్య నియమం_Character_Iterator::getPos(నియమం_Character_Iterator::Pos& p) స్థిరరాశి {
    p.buf = buf;
    p.pos = pos.get_సూచీ();
    p.bufPos = bufPos;
}

శూన్య నియమం_Character_Iterator::సమితి_Pos(స్థిరరాశి నియమం_Character_Iterator::Pos& p) {
    buf = p.buf;
    pos.సమితి_సూచీ(p.pos);
    bufPos = p.bufPos;
}

శూన్య నియమం_Character_Iterator::skipIgnored(int32_t options) {
    ఒకవేళ ((options & SKIP_WHITESPACE) != 0) {
        కొరకు (;;) {
            UChar32 a = _current();
            ఒకవేళ (!నమూనా_Props::isWhiteSpace(a)) ఆపు;
            _advance(U16_నిడివి(a));
        }
    }
}

UnicodeString& నియమం_Character_Iterator::lookahead(UnicodeString& ఫలితం, int32_t maxLookAhead) స్థిరరాశి {
    ఒకవేళ (maxLookAhead < 0) {
        maxLookAhead = 0x7FFFFFFF;
    }
    ఒకవేళ (buf != శూన్యసూచిక) {
        buf->extract(bufPos, maxLookAhead, ఫలితం);
    } లేకపోతే {
        పాఠ్యం.extract(pos.get_సూచీ(), maxLookAhead, ఫలితం);
    }
    తిరిగి ఫలితం;
}

శూన్య నియమం_Character_Iterator::jumpahead(int32_t count) {
    _advance(count);
}

/*
UnicodeString& RuleCharacterIterator::toString(UnicodeString& result) const {
    int32_t b = pos.getIndex();
    text.extract(0, b, result);
    return result.append((char16_t) 0x7C).append(text, b, 0x7FFFFFFF); // Insert '|' at index
}
*/

UChar32 నియమం_Character_Iterator::_current() స్థిరరాశి {
    ఒకవేళ (buf != శూన్యసూచిక) {
        తిరిగి buf->char32At(bufPos);
    } లేకపోతే {
        పూర్ణాంకం i = pos.get_సూచీ();
        తిరిగి (i < పాఠ్యం.length()) ? పాఠ్యం.char32At(i) : static_cast<UChar32>(పూర్తి_2);
    }
}

శూన్య నియమం_Character_Iterator::_advance(int32_t count) {
    ఒకవేళ (buf != శూన్యసూచిక) {
        bufPos += count;
        ఒకవేళ (bufPos == buf->length()) {
            buf = శూన్యసూచిక;
        }
    } లేకపోతే {
        pos.సమితి_సూచీ(pos.get_సూచీ() + count);
        ఒకవేళ (pos.get_సూచీ() > పాఠ్యం.length()) {
            pos.సమితి_సూచీ(పాఠ్యం.length());
        }
    }
}

U_NAMESPACE_చివర

//eof
