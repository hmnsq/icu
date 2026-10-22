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
    врати *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    ако (this == &that) {
        врати true;
    }
    ако (typeid(*this) != typeid(that)) {
        врати false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    врати text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    врати ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    врати new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    ако(pos < end) {
        врати text[pos];
    } инако {
        врати DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    ако(pos < end) {
        врати text[pos++];
    } инако {
        врати DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    ако(pos > begin) {
        врати text[--pos];
    } инако {
        врати DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    ако(position < begin) {
        pos = begin;
    } инако ако(position > end) {
        pos = end;
    } инако {
        pos = position;
    }
    ако(pos < end) {
        врати text[pos];
    } инако {
        врати DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    ако (pos >= begin && pos < end) {
        врати text[pos];
    } инако {
        врати DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    ако (pos + 1 < end) {
        врати text[++pos];
    } инако {
        /* make current() return DONE */
        pos = end;
        врати DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    ако (pos < end) {
        врати text[pos++];
    } инако {
        врати DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    врати pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    ако (pos > begin) {
        врати text[--pos];
    } инако {
        врати DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    врати pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    ако(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        врати c;
    } инако {
        врати DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    ако(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        врати c;
    } инако {
        врати DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    ако(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        врати c;
    } инако {
        врати DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    ако(position < begin) {
        position = begin;
    } инако ако(position > end) {
        position = end;
    }
    ако(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        врати c;
    } инако {
        this->pos = position;
        врати DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    ако (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        врати c;
    } инако {
        врати DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    ако (pos < end) {
        U16_FWD_1(text, pos, end);
        ако(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            врати c;
        }
    }
    /* make current() return DONE */
    pos = end;
    врати DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    ако (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        врати c;
    } инако {
        врати DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    ако (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        врати c;
    } инако {
        врати DONE;
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

    ако(pos < begin) {
        pos = begin;
    } инако ако(pos > end) {
        pos = end;
    }

    врати pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    case kStart:
        pos = begin;
        ако(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        break;
    case kCurrent:
        ако(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } инако {
            U16_BACK_N(text, begin, pos, -delta);
        }
        break;
    case kEnd:
        pos = end;
        ако(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        break;
    default:
        break;
    }

    врати pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    ако (newText == nullptr || newTextLength < 0) {
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
