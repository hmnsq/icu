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
    ཕྱིར *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    གལ་ཏེ (this == &that) {
        ཕྱིར true;
    }
    གལ་ཏེ (typeid(*this) != typeid(that)) {
        ཕྱིར false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    ཕྱིར text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    ཕྱིར ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    ཕྱིར new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    གལ་ཏེ(pos < end) {
        ཕྱིར text[pos];
    } དེ་མིན {
        ཕྱིར DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    གལ་ཏེ(pos < end) {
        ཕྱིར text[pos++];
    } དེ་མིན {
        ཕྱིར DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    གལ་ཏེ(pos > begin) {
        ཕྱིར text[--pos];
    } དེ་མིན {
        ཕྱིར DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    གལ་ཏེ(position < begin) {
        pos = begin;
    } དེ་མིན གལ་ཏེ(position > end) {
        pos = end;
    } དེ་མིན {
        pos = position;
    }
    གལ་ཏེ(pos < end) {
        ཕྱིར text[pos];
    } དེ་མིན {
        ཕྱིར DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    གལ་ཏེ (pos >= begin && pos < end) {
        ཕྱིར text[pos];
    } དེ་མིན {
        ཕྱིར DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    གལ་ཏེ (pos + 1 < end) {
        ཕྱིར text[++pos];
    } དེ་མིན {
        /* make current() return DONE */
        pos = end;
        ཕྱིར DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    གལ་ཏེ (pos < end) {
        ཕྱིར text[pos++];
    } དེ་མིན {
        ཕྱིར DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ཕྱིར pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    གལ་ཏེ (pos > begin) {
        ཕྱིར text[--pos];
    } དེ་མིན {
        ཕྱིར DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ཕྱིར pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    གལ་ཏེ(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ཕྱིར c;
    } དེ་མིན {
        ཕྱིར DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    གལ་ཏེ(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ཕྱིར c;
    } དེ་མིན {
        ཕྱིར DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    གལ་ཏེ(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ཕྱིར c;
    } དེ་མིན {
        ཕྱིར DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    གལ་ཏེ(position < begin) {
        position = begin;
    } དེ་མིན གལ་ཏེ(position > end) {
        position = end;
    }
    གལ་ཏེ(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ཕྱིར c;
    } དེ་མིན {
        this->pos = position;
        ཕྱིར DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    གལ་ཏེ (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ཕྱིར c;
    } དེ་མིན {
        ཕྱིར DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    གལ་ཏེ (pos < end) {
        U16_FWD_1(text, pos, end);
        གལ་ཏེ(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ཕྱིར c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ཕྱིར DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    གལ་ཏེ (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ཕྱིར c;
    } དེ་མིན {
        ཕྱིར DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    གལ་ཏེ (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ཕྱིར c;
    } དེ་མིན {
        ཕྱིར DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    switch(origin) {
    pak_sa kStart:
        pos = begin + delta;
        bha_nga;
    pak_sa kCurrent:
        pos += delta;
        bha_nga;
    pak_sa kEnd:
        pos = end + delta;
        bha_nga;
    default:
        bha_nga;
    }

    གལ་ཏེ(pos < begin) {
        pos = begin;
    } དེ་མིན གལ་ཏེ(pos > end) {
        pos = end;
    }

    ཕྱིར pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    pak_sa kStart:
        pos = begin;
        གལ་ཏེ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        bha_nga;
    pak_sa kCurrent:
        གལ་ཏེ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } དེ་མིན {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    pak_sa kEnd:
        pos = end;
        གལ་ཏེ(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ཕྱིར pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    གལ་ཏེ (newText == nullptr || newTextLength < 0) {
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
