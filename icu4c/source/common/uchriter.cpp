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
    यदि (एतत् == &that) {
        प्रत्यागम सत्य;
    }
    यदि (typeid(*एतत्) != typeid(that)) {
        प्रत्यागम असत्य;
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
    यदि(pos < end) {
        प्रत्यागम text[pos];
    } अन्यथा {
        प्रत्यागम DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    यदि(pos < end) {
        प्रत्यागम text[pos++];
    } अन्यथा {
        प्रत्यागम DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    यदि(pos > begin) {
        प्रत्यागम text[--pos];
    } अन्यथा {
        प्रत्यागम DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    यदि(position < begin) {
        pos = begin;
    } अन्यथा यदि(position > end) {
        pos = end;
    } अन्यथा {
        pos = position;
    }
    यदि(pos < end) {
        प्रत्यागम text[pos];
    } अन्यथा {
        प्रत्यागम DONE;
    }
}

char16_t
UCharCharacterIterator::current() ध्रुव {
    यदि (pos >= begin && pos < end) {
        प्रत्यागम text[pos];
    } अन्यथा {
        प्रत्यागम DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    यदि (pos + 1 < end) {
        प्रत्यागम text[++pos];
    } अन्यथा {
        /* make current() return DONE */
        pos = end;
        प्रत्यागम DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    यदि (pos < end) {
        प्रत्यागम text[pos++];
    } अन्यथा {
        प्रत्यागम DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    प्रत्यागम pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    यदि (pos > begin) {
        प्रत्यागम text[--pos];
    } अन्यथा {
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
    यदि(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        प्रत्यागम c;
    } अन्यथा {
        प्रत्यागम DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    यदि(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        प्रत्यागम c;
    } अन्यथा {
        प्रत्यागम DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    यदि(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        प्रत्यागम c;
    } अन्यथा {
        प्रत्यागम DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    यदि(position < begin) {
        position = begin;
    } अन्यथा यदि(position > end) {
        position = end;
    }
    यदि(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = एतत्->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        प्रत्यागम c;
    } अन्यथा {
        एतत्->pos = position;
        प्रत्यागम DONE;
    }
}

UChar32
UCharCharacterIterator::current32() ध्रुव {
    यदि (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        प्रत्यागम c;
    } अन्यथा {
        प्रत्यागम DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    यदि (pos < end) {
        U16_FWD_1(text, pos, end);
        यदि(pos < end) {
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
    यदि (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        प्रत्यागम c;
    } अन्यथा {
        प्रत्यागम DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    यदि (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        प्रत्यागम c;
    } अन्यथा {
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

    यदि(pos < begin) {
        pos = begin;
    } अन्यथा यदि(pos > end) {
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
        यदि(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        भङ्ग;
    पक्ष kCurrent:
        यदि(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } अन्यथा {
            U16_BACK_N(text, begin, pos, -delta);
        }
        भङ्ग;
    पक्ष kEnd:
        pos = end;
        यदि(delta < 0) {
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
    यदि (newText == nullptr || newTextLength < 0) {
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
