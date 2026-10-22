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
    ᤀᤡᤴ *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    ᤀᤡ᤺ (this == &that) {
        ᤀᤡᤴ true;
    }
    ᤀᤡ᤺ (typeid(*this) != typeid(that)) {
        ᤀᤡᤴ false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    ᤀᤡᤴ text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    ᤀᤡᤴ ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    ᤀᤡᤴ new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    ᤀᤡ᤺(pos < end) {
        ᤀᤡᤴ text[pos];
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡᤴ DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    ᤀᤡ᤺(pos < end) {
        ᤀᤡᤴ text[pos++];
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡᤴ DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    ᤀᤡ᤺(pos > begin) {
        ᤀᤡᤴ text[--pos];
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡᤴ DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    ᤀᤡ᤺(position < begin) {
        pos = begin;
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(position > end) {
        pos = end;
    } ᤀᤡ᤺ᤆᤡ {
        pos = position;
    }
    ᤀᤡ᤺(pos < end) {
        ᤀᤡᤴ text[pos];
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡᤴ DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    ᤀᤡ᤺ (pos >= begin && pos < end) {
        ᤀᤡᤴ text[pos];
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡᤴ DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    ᤀᤡ᤺ (pos + 1 < end) {
        ᤀᤡᤴ text[++pos];
    } ᤀᤡ᤺ᤆᤡ {
        /* make current() return DONE */
        pos = end;
        ᤀᤡᤴ DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    ᤀᤡ᤺ (pos < end) {
        ᤀᤡᤴ text[pos++];
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡᤴ DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ᤀᤡᤴ pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    ᤀᤡ᤺ (pos > begin) {
        ᤀᤡᤴ text[--pos];
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡᤴ DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ᤀᤡᤴ pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    ᤀᤡ᤺(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ᤀᤡᤴ c;
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡᤴ DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    ᤀᤡ᤺(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ᤀᤡᤴ c;
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡᤴ DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    ᤀᤡ᤺(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ᤀᤡᤴ c;
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡᤴ DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    ᤀᤡ᤺(position < begin) {
        position = begin;
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(position > end) {
        position = end;
    }
    ᤀᤡ᤺(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ᤀᤡᤴ c;
    } ᤀᤡ᤺ᤆᤡ {
        this->pos = position;
        ᤀᤡᤴ DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    ᤀᤡ᤺ (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ᤀᤡᤴ c;
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡᤴ DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    ᤀᤡ᤺ (pos < end) {
        U16_FWD_1(text, pos, end);
        ᤀᤡ᤺(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ᤀᤡᤴ c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ᤀᤡᤴ DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    ᤀᤡ᤺ (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ᤀᤡᤴ c;
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡᤴ DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    ᤀᤡ᤺ (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ᤀᤡᤴ c;
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡᤴ DONE;
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

    ᤀᤡ᤺(pos < begin) {
        pos = begin;
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(pos > end) {
        pos = end;
    }

    ᤀᤡᤴ pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    pak_sa kStart:
        pos = begin;
        ᤀᤡ᤺(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        bha_nga;
    pak_sa kCurrent:
        ᤀᤡ᤺(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } ᤀᤡ᤺ᤆᤡ {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    pak_sa kEnd:
        pos = end;
        ᤀᤡ᤺(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ᤀᤡᤴ pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    ᤀᤡ᤺ (newText == nullptr || newTextLength < 0) {
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
