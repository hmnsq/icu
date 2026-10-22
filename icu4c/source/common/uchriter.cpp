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
    trả_về *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    nếu (this == &that) {
        trả_về true;
    }
    nếu (typeid(*this) != typeid(that)) {
        trả_về false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    trả_về text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    trả_về ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    trả_về new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    nếu(pos < end) {
        trả_về text[pos];
    } không_thì {
        trả_về DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    nếu(pos < end) {
        trả_về text[pos++];
    } không_thì {
        trả_về DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    nếu(pos > begin) {
        trả_về text[--pos];
    } không_thì {
        trả_về DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    nếu(position < begin) {
        pos = begin;
    } không_thì nếu(position > end) {
        pos = end;
    } không_thì {
        pos = position;
    }
    nếu(pos < end) {
        trả_về text[pos];
    } không_thì {
        trả_về DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    nếu (pos >= begin && pos < end) {
        trả_về text[pos];
    } không_thì {
        trả_về DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    nếu (pos + 1 < end) {
        trả_về text[++pos];
    } không_thì {
        /* make current() return DONE */
        pos = end;
        trả_về DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    nếu (pos < end) {
        trả_về text[pos++];
    } không_thì {
        trả_về DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    trả_về pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    nếu (pos > begin) {
        trả_về text[--pos];
    } không_thì {
        trả_về DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    trả_về pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    nếu(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        trả_về c;
    } không_thì {
        trả_về DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    nếu(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        trả_về c;
    } không_thì {
        trả_về DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    nếu(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        trả_về c;
    } không_thì {
        trả_về DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    nếu(position < begin) {
        position = begin;
    } không_thì nếu(position > end) {
        position = end;
    }
    nếu(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        trả_về c;
    } không_thì {
        this->pos = position;
        trả_về DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    nếu (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        trả_về c;
    } không_thì {
        trả_về DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    nếu (pos < end) {
        U16_FWD_1(text, pos, end);
        nếu(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            trả_về c;
        }
    }
    /* make current() return DONE */
    pos = end;
    trả_về DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    nếu (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        trả_về c;
    } không_thì {
        trả_về DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    nếu (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        trả_về c;
    } không_thì {
        trả_về DONE;
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

    nếu(pos < begin) {
        pos = begin;
    } không_thì nếu(pos > end) {
        pos = end;
    }

    trả_về pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    case kStart:
        pos = begin;
        nếu(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        break;
    case kCurrent:
        nếu(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } không_thì {
            U16_BACK_N(text, begin, pos, -delta);
        }
        break;
    case kEnd:
        pos = end;
        nếu(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        break;
    default:
        break;
    }

    trả_về pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    nếu (newText == nullptr || newTextLength < 0) {
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
