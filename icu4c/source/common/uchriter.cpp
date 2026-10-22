// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#include "utypeinfo.h"  // for 'typeid' to work

#include "unicode/uchriter.h"
#include "unicode/ustring.h"
#include "unicode/utf16.h"
#include "ustr_imp.h"

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

UCharCharacterIterator::UCharCharacterIterator(const UCharCharacterIterator& that)
: CharacterIterator(that),
  text(that.text)
{
}

UCharCharacterIterator&
UCharCharacterIterator::operator=(const UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::operator=(that);
    text = that.text;
    повернути *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    якщо (this == &that) {
        повернути true;
    }
    якщо (typeid(*this) != typeid(that)) {
        повернути false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    повернути text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    повернути ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    повернути new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    якщо(pos < end) {
        повернути text[pos];
    } інакше {
        повернути DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    якщо(pos < end) {
        повернути text[pos++];
    } інакше {
        повернути DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    якщо(pos > begin) {
        повернути text[--pos];
    } інакше {
        повернути DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    якщо(position < begin) {
        pos = begin;
    } інакше якщо(position > end) {
        pos = end;
    } інакше {
        pos = position;
    }
    якщо(pos < end) {
        повернути text[pos];
    } інакше {
        повернути DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    якщо (pos >= begin && pos < end) {
        повернути text[pos];
    } інакше {
        повернути DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    якщо (pos + 1 < end) {
        повернути text[++pos];
    } інакше {
        /* make current() return DONE */
        pos = end;
        повернути DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    якщо (pos < end) {
        повернути text[pos++];
    } інакше {
        повернути DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    повернути pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    якщо (pos > begin) {
        повернути text[--pos];
    } інакше {
        повернути DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    повернути pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    якщо(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        повернути c;
    } інакше {
        повернути DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    якщо(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        повернути c;
    } інакше {
        повернути DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    якщо(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        повернути c;
    } інакше {
        повернути DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    якщо(position < begin) {
        position = begin;
    } інакше якщо(position > end) {
        position = end;
    }
    якщо(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        повернути c;
    } інакше {
        this->pos = position;
        повернути DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    якщо (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        повернути c;
    } інакше {
        повернути DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    якщо (pos < end) {
        U16_FWD_1(text, pos, end);
        якщо(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            повернути c;
        }
    }
    /* make current() return DONE */
    pos = end;
    повернути DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    якщо (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        повернути c;
    } інакше {
        повернути DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    якщо (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        повернути c;
    } інакше {
        повернути DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    switch(origin) {
    case kStart:
        pos = begin + delta;
        break;
    case kCurrent:
        pos += delta;
        break;
    case kEnd:
        pos = end + delta;
        break;
    default:
        break;
    }

    якщо(pos < begin) {
        pos = begin;
    } інакше якщо(pos > end) {
        pos = end;
    }

    повернути pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    case kStart:
        pos = begin;
        якщо(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        break;
    case kCurrent:
        якщо(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } інакше {
            U16_BACK_N(text, begin, pos, -delta);
        }
        break;
    case kEnd:
        pos = end;
        якщо(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        break;
    default:
        break;
    }

    повернути pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    якщо (newText == nullptr || newTextLength < 0) {
        newTextLength = 0;
    }
    end = textLength = newTextLength;
    pos = begin = 0;
}

void
UCharCharacterIterator::getText(UnicodeString& result) {
    result = UnicodeString(text, textLength);
}

U_NAMESPACE_END
