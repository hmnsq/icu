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
    вернуть *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    если (this == &that) {
        вернуть true;
    }
    если (typeid(*this) != typeid(that)) {
        вернуть false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    вернуть text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    вернуть ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    вернуть new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    если(pos < end) {
        вернуть text[pos];
    } иначе {
        вернуть DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    если(pos < end) {
        вернуть text[pos++];
    } иначе {
        вернуть DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    если(pos > begin) {
        вернуть text[--pos];
    } иначе {
        вернуть DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    если(position < begin) {
        pos = begin;
    } иначе если(position > end) {
        pos = end;
    } иначе {
        pos = position;
    }
    если(pos < end) {
        вернуть text[pos];
    } иначе {
        вернуть DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    если (pos >= begin && pos < end) {
        вернуть text[pos];
    } иначе {
        вернуть DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    если (pos + 1 < end) {
        вернуть text[++pos];
    } иначе {
        /* make current() return DONE */
        pos = end;
        вернуть DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    если (pos < end) {
        вернуть text[pos++];
    } иначе {
        вернуть DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    вернуть pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    если (pos > begin) {
        вернуть text[--pos];
    } иначе {
        вернуть DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    вернуть pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    если(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        вернуть c;
    } иначе {
        вернуть DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    если(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        вернуть c;
    } иначе {
        вернуть DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    если(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        вернуть c;
    } иначе {
        вернуть DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    если(position < begin) {
        position = begin;
    } иначе если(position > end) {
        position = end;
    }
    если(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        вернуть c;
    } иначе {
        this->pos = position;
        вернуть DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    если (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        вернуть c;
    } иначе {
        вернуть DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    если (pos < end) {
        U16_FWD_1(text, pos, end);
        если(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            вернуть c;
        }
    }
    /* make current() return DONE */
    pos = end;
    вернуть DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    если (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        вернуть c;
    } иначе {
        вернуть DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    если (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        вернуть c;
    } иначе {
        вернуть DONE;
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

    если(pos < begin) {
        pos = begin;
    } иначе если(pos > end) {
        pos = end;
    }

    вернуть pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    case kStart:
        pos = begin;
        если(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        break;
    case kCurrent:
        если(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } иначе {
            U16_BACK_N(text, begin, pos, -delta);
        }
        break;
    case kEnd:
        pos = end;
        если(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        break;
    default:
        break;
    }

    вернуть pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    если (newText == nullptr || newTextLength < 0) {
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
