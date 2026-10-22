// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#അന്തര്ഭാവ "utypeinfo.h"  // for 'typeid' to work

#അന്തര്ഭാവ "unicode/uchriter.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "ustr_imp.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UCharCharacterIterator)

UCharCharacterIterator::UCharCharacterIterator()
  : CharacterIterator(),
  text(nullptr)
{
    // never default construct!
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                                               int32_t length)
  : CharacterIterator(textPtr != nullptr ? (length >= 0 ? length : u_strlen(textPtr)) : 0),
  text(textPtr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t position)
  : CharacterIterator(textPtr != nullptr ? (length >= 0 ? length : u_strlen(textPtr)) : 0, position),
  text(textPtr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t textBegin,
                                               int32_t textEnd,
                                               int32_t position)
  : CharacterIterator(textPtr != nullptr ? (length >= 0 ? length : u_strlen(textPtr)) : 0,
                      textBegin, textEnd, position),
  text(textPtr)
{
}

UCharCharacterIterator::UCharCharacterIterator(സ്ഥിരം UCharCharacterIterator& that)
: CharacterIterator(that),
  text(that.text)
{
}

UCharCharacterIterator&
UCharCharacterIterator::operator=(സ്ഥിരം UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::operator=(that);
    text = that.text;
    തിരികെ *ഇത്;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

തർക്കം
UCharCharacterIterator::operator==(സ്ഥിരം ForwardCharacterIterator& that) സ്ഥിരം {
    എങ്കിൽ (ഇത് == &that) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ (typeid(*ഇത്) != typeid(that)) {
        തിരികെ അസത്യം;
    }

    സ്ഥിരം UCharCharacterIterator&    realThat = static_cast<സ്ഥിരം UCharCharacterIterator&>(that);

    തിരികെ text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() സ്ഥിരം {
    തിരികെ ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() സ്ഥിരം {
    തിരികെ പുതിയ UCharCharacterIterator(*ഇത്);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    എങ്കിൽ(pos < end) {
        തിരികെ text[pos];
    } അല്ലെങ്കിൽ {
        തിരികെ DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    എങ്കിൽ(pos < end) {
        തിരികെ text[pos++];
    } അല്ലെങ്കിൽ {
        തിരികെ DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    എങ്കിൽ(pos > begin) {
        തിരികെ text[--pos];
    } അല്ലെങ്കിൽ {
        തിരികെ DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    എങ്കിൽ(position < begin) {
        pos = begin;
    } അല്ലെങ്കിൽ എങ്കിൽ(position > end) {
        pos = end;
    } അല്ലെങ്കിൽ {
        pos = position;
    }
    എങ്കിൽ(pos < end) {
        തിരികെ text[pos];
    } അല്ലെങ്കിൽ {
        തിരികെ DONE;
    }
}

char16_t
UCharCharacterIterator::current() സ്ഥിരം {
    എങ്കിൽ (pos >= begin && pos < end) {
        തിരികെ text[pos];
    } അല്ലെങ്കിൽ {
        തിരികെ DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    എങ്കിൽ (pos + 1 < end) {
        തിരികെ text[++pos];
    } അല്ലെങ്കിൽ {
        /* make current() return DONE */
        pos = end;
        തിരികെ DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    എങ്കിൽ (pos < end) {
        തിരികെ text[pos++];
    } അല്ലെങ്കിൽ {
        തിരികെ DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    തിരികെ pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    എങ്കിൽ (pos > begin) {
        തിരികെ text[--pos];
    } അല്ലെങ്കിൽ {
        തിരികെ DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    തിരികെ pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    എങ്കിൽ(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        തിരികെ c;
    } അല്ലെങ്കിൽ {
        തിരികെ DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    എങ്കിൽ(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        തിരികെ c;
    } അല്ലെങ്കിൽ {
        തിരികെ DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    എങ്കിൽ(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        തിരികെ c;
    } അല്ലെങ്കിൽ {
        തിരികെ DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    എങ്കിൽ(position < begin) {
        position = begin;
    } അല്ലെങ്കിൽ എങ്കിൽ(position > end) {
        position = end;
    }
    എങ്കിൽ(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = ഇത്->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        തിരികെ c;
    } അല്ലെങ്കിൽ {
        ഇത്->pos = position;
        തിരികെ DONE;
    }
}

UChar32
UCharCharacterIterator::current32() സ്ഥിരം {
    എങ്കിൽ (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        തിരികെ c;
    } അല്ലെങ്കിൽ {
        തിരികെ DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    എങ്കിൽ (pos < end) {
        U16_FWD_1(text, pos, end);
        എങ്കിൽ(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            തിരികെ c;
        }
    }
    /* make current() return DONE */
    pos = end;
    തിരികെ DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    എങ്കിൽ (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        തിരികെ c;
    } അല്ലെങ്കിൽ {
        തിരികെ DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    എങ്കിൽ (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        തിരികെ c;
    } അല്ലെങ്കിൽ {
        തിരികെ DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    തിരഞ്ഞെടുപ്പ്(origin) {
    അവസ്ഥ kStart:
        pos = begin + delta;
        നിർത്തുക;
    അവസ്ഥ kCurrent:
        pos += delta;
        നിർത്തുക;
    അവസ്ഥ kEnd:
        pos = end + delta;
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        നിർത്തുക;
    }

    എങ്കിൽ(pos < begin) {
        pos = begin;
    } അല്ലെങ്കിൽ എങ്കിൽ(pos > end) {
        pos = end;
    }

    തിരികെ pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    തിരഞ്ഞെടുപ്പ്(origin) {
    അവസ്ഥ kStart:
        pos = begin;
        എങ്കിൽ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        നിർത്തുക;
    അവസ്ഥ kCurrent:
        എങ്കിൽ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } അല്ലെങ്കിൽ {
            U16_BACK_N(text, begin, pos, -delta);
        }
        നിർത്തുക;
    അവസ്ഥ kEnd:
        pos = end;
        എങ്കിൽ(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        നിർത്തുക;
    }

    തിരികെ pos;
}

ശൂന്യം UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    എങ്കിൽ (newText == nullptr || newTextLength < 0) {
        newTextLength = 0;
    }
    end = textLength = newTextLength;
    pos = begin = 0;
}

ശൂന്യം
UCharCharacterIterator::getText(UnicodeString& result) {
    result = UnicodeString(text, textLength);
}

U_NAMESPACE_END
