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
    반환 *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    만약 (this == &that) {
        반환 true;
    }
    만약 (typeid(*this) != typeid(that)) {
        반환 false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    반환 text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    반환 ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    반환 new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    만약(pos < end) {
        반환 text[pos];
    } 아니면 {
        반환 DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    만약(pos < end) {
        반환 text[pos++];
    } 아니면 {
        반환 DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    만약(pos > begin) {
        반환 text[--pos];
    } 아니면 {
        반환 DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    만약(position < begin) {
        pos = begin;
    } 아니면 만약(position > end) {
        pos = end;
    } 아니면 {
        pos = position;
    }
    만약(pos < end) {
        반환 text[pos];
    } 아니면 {
        반환 DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    만약 (pos >= begin && pos < end) {
        반환 text[pos];
    } 아니면 {
        반환 DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    만약 (pos + 1 < end) {
        반환 text[++pos];
    } 아니면 {
        /* make current() return DONE */
        pos = end;
        반환 DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    만약 (pos < end) {
        반환 text[pos++];
    } 아니면 {
        반환 DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    반환 pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    만약 (pos > begin) {
        반환 text[--pos];
    } 아니면 {
        반환 DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    반환 pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    만약(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        반환 c;
    } 아니면 {
        반환 DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    만약(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        반환 c;
    } 아니면 {
        반환 DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    만약(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        반환 c;
    } 아니면 {
        반환 DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    만약(position < begin) {
        position = begin;
    } 아니면 만약(position > end) {
        position = end;
    }
    만약(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        반환 c;
    } 아니면 {
        this->pos = position;
        반환 DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    만약 (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        반환 c;
    } 아니면 {
        반환 DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    만약 (pos < end) {
        U16_FWD_1(text, pos, end);
        만약(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            반환 c;
        }
    }
    /* make current() return DONE */
    pos = end;
    반환 DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    만약 (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        반환 c;
    } 아니면 {
        반환 DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    만약 (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        반환 c;
    } 아니면 {
        반환 DONE;
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

    만약(pos < begin) {
        pos = begin;
    } 아니면 만약(pos > end) {
        pos = end;
    }

    반환 pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    case kStart:
        pos = begin;
        만약(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        break;
    case kCurrent:
        만약(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } 아니면 {
            U16_BACK_N(text, begin, pos, -delta);
        }
        break;
    case kEnd:
        pos = end;
        만약(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        break;
    default:
        break;
    }

    반환 pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    만약 (newText == nullptr || newTextLength < 0) {
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
