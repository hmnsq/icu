// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#अन्तर्भाव "utypeinfo.h"  // for 'typeid' to work

#अन्तर्भाव "unicode/uchriter.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "ustr_imp.h"

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

UCharCharacterIterator::UCharCharacterIterator(ध्रुव UCharCharacterIterator& that)
: CharacterIterator(that),
  text(that.text)
{
}

UCharCharacterIterator&
UCharCharacterIterator::operator=(ध्रुव UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::operator=(that);
    text = that.text;
    प्रत्यागम *एतत्;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

तार्किक
UCharCharacterIterator::operator==(ध्रुव ForwardCharacterIterator& that) ध्रुव {
    जइ (एतत् == &that) {
        प्रत्यागम सच्च;
    }
    जइ (typeid(*एतत्) != typeid(that)) {
        प्रत्यागम मिच्छ;
    }

    ध्रुव UCharCharacterIterator&    realThat = static_cast<ध्रुव UCharCharacterIterator&>(that);

    प्रत्यागम text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() ध्रुव {
    प्रत्यागम ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ध्रुव {
    प्रत्यागम नूतन UCharCharacterIterator(*एतत्);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    जइ(pos < end) {
        प्रत्यागम text[pos];
    } अण्णहा {
        प्रत्यागम DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    जइ(pos < end) {
        प्रत्यागम text[pos++];
    } अण्णहा {
        प्रत्यागम DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    जइ(pos > begin) {
        प्रत्यागम text[--pos];
    } अण्णहा {
        प्रत्यागम DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    जइ(position < begin) {
        pos = begin;
    } अण्णहा जइ(position > end) {
        pos = end;
    } अण्णहा {
        pos = position;
    }
    जइ(pos < end) {
        प्रत्यागम text[pos];
    } अण्णहा {
        प्रत्यागम DONE;
    }
}

char16_t
UCharCharacterIterator::current() ध्रुव {
    जइ (pos >= begin && pos < end) {
        प्रत्यागम text[pos];
    } अण्णहा {
        प्रत्यागम DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    जइ (pos + 1 < end) {
        प्रत्यागम text[++pos];
    } अण्णहा {
        /* make current() return DONE */
        pos = end;
        प्रत्यागम DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    जइ (pos < end) {
        प्रत्यागम text[pos++];
    } अण्णहा {
        प्रत्यागम DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    प्रत्यागम pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    जइ (pos > begin) {
        प्रत्यागम text[--pos];
    } अण्णहा {
        प्रत्यागम DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    प्रत्यागम pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    जइ(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        प्रत्यागम c;
    } अण्णहा {
        प्रत्यागम DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    जइ(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        प्रत्यागम c;
    } अण्णहा {
        प्रत्यागम DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    जइ(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        प्रत्यागम c;
    } अण्णहा {
        प्रत्यागम DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    जइ(position < begin) {
        position = begin;
    } अण्णहा जइ(position > end) {
        position = end;
    }
    जइ(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = एतत्->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        प्रत्यागम c;
    } अण्णहा {
        एतत्->pos = position;
        प्रत्यागम DONE;
    }
}

UChar32
UCharCharacterIterator::current32() ध्रुव {
    जइ (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        प्रत्यागम c;
    } अण्णहा {
        प्रत्यागम DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    जइ (pos < end) {
        U16_FWD_1(text, pos, end);
        जइ(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            प्रत्यागम c;
        }
    }
    /* make current() return DONE */
    pos = end;
    प्रत्यागम DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    जइ (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        प्रत्यागम c;
    } अण्णहा {
        प्रत्यागम DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    जइ (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        प्रत्यागम c;
    } अण्णहा {
        प्रत्यागम DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    विकल्प(origin) {
    पक्ष kStart:
        pos = begin + delta;
        भङ्ग;
    पक्ष kCurrent:
        pos += delta;
        भङ्ग;
    पक्ष kEnd:
        pos = end + delta;
        भङ्ग;
    स्वभाव:
        भङ्ग;
    }

    जइ(pos < begin) {
        pos = begin;
    } अण्णहा जइ(pos > end) {
        pos = end;
    }

    प्रत्यागम pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    विकल्प(origin) {
    पक्ष kStart:
        pos = begin;
        जइ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        भङ्ग;
    पक्ष kCurrent:
        जइ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } अण्णहा {
            U16_BACK_N(text, begin, pos, -delta);
        }
        भङ्ग;
    पक्ष kEnd:
        pos = end;
        जइ(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        भङ्ग;
    स्वभाव:
        भङ्ग;
    }

    प्रत्यागम pos;
}

शून्य UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    जइ (newText == nullptr || newTextLength < 0) {
        newTextLength = 0;
    }
    end = textLength = newTextLength;
    pos = begin = 0;
}

शून्य
UCharCharacterIterator::getText(UnicodeString& result) {
    result = UnicodeString(text, textLength);
}

U_NAMESPACE_END
