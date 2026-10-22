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

UCharCharacterIterator::UCharCharacterIterator(अचर UCharCharacterIterator& that)
: CharacterIterator(that),
  text(that.text)
{
}

UCharCharacterIterator&
UCharCharacterIterator::चालक=(अचर UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::चालक=(that);
    text = that.text;
    फर्काऊ *एतत्;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

तार्किक
UCharCharacterIterator::चालक==(अचर ForwardCharacterIterator& that) अचर {
    यदि (एतत् == &that) {
        फर्काऊ सत्य;
    }
    यदि (typeid(*एतत्) != typeid(that)) {
        फर्काऊ असत्य;
    }

    अचर UCharCharacterIterator&    realThat = static_cast<अचर UCharCharacterIterator&>(that);

    फर्काऊ text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() अचर {
    फर्काऊ ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() अचर {
    फर्काऊ नूतन UCharCharacterIterator(*एतत्);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    यदि(pos < end) {
        फर्काऊ text[pos];
    } अन्यथा {
        फर्काऊ DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    यदि(pos < end) {
        फर्काऊ text[pos++];
    } अन्यथा {
        फर्काऊ DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    यदि(pos > begin) {
        फर्काऊ text[--pos];
    } अन्यथा {
        फर्काऊ DONE;
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
        फर्काऊ text[pos];
    } अन्यथा {
        फर्काऊ DONE;
    }
}

char16_t
UCharCharacterIterator::current() अचर {
    यदि (pos >= begin && pos < end) {
        फर्काऊ text[pos];
    } अन्यथा {
        फर्काऊ DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    यदि (pos + 1 < end) {
        फर्काऊ text[++pos];
    } अन्यथा {
        /* make current() return DONE */
        pos = end;
        फर्काऊ DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    यदि (pos < end) {
        फर्काऊ text[pos++];
    } अन्यथा {
        फर्काऊ DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    फर्काऊ pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    यदि (pos > begin) {
        फर्काऊ text[--pos];
    } अन्यथा {
        फर्काऊ DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    फर्काऊ pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    यदि(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        फर्काऊ c;
    } अन्यथा {
        फर्काऊ DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    यदि(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        फर्काऊ c;
    } अन्यथा {
        फर्काऊ DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    यदि(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        फर्काऊ c;
    } अन्यथा {
        फर्काऊ DONE;
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
        फर्काऊ c;
    } अन्यथा {
        एतत्->pos = position;
        फर्काऊ DONE;
    }
}

UChar32
UCharCharacterIterator::current32() अचर {
    यदि (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        फर्काऊ c;
    } अन्यथा {
        फर्काऊ DONE;
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
            फर्काऊ c;
        }
    }
    /* make current() return DONE */
    pos = end;
    फर्काऊ DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    यदि (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        फर्काऊ c;
    } अन्यथा {
        फर्काऊ DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    यदि (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        फर्काऊ c;
    } अन्यथा {
        फर्काऊ DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    विकल्प(origin) {
    पक्ष kStart:
        pos = begin + delta;
        रोक;
    पक्ष kCurrent:
        pos += delta;
        रोक;
    पक्ष kEnd:
        pos = end + delta;
        रोक;
    स्वभाव:
        रोक;
    }

    यदि(pos < begin) {
        pos = begin;
    } अन्यथा यदि(pos > end) {
        pos = end;
    }

    फर्काऊ pos;
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
        रोक;
    पक्ष kCurrent:
        यदि(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } अन्यथा {
            U16_BACK_N(text, begin, pos, -delta);
        }
        रोक;
    पक्ष kEnd:
        pos = end;
        यदि(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        रोक;
    स्वभाव:
        रोक;
    }

    फर्काऊ pos;
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
