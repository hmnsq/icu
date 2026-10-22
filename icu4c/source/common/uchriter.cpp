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
    ᯀᯮᯞ *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    ᯀᯮᯝ (this == &that) {
        ᯀᯮᯞ true;
    }
    ᯀᯮᯝ (typeid(*this) != typeid(that)) {
        ᯀᯮᯞ false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    ᯀᯮᯞ text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    ᯀᯮᯞ ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    ᯀᯮᯞ new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    ᯀᯮᯝ(pos < end) {
        ᯀᯮᯞ text[pos];
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    ᯀᯮᯝ(pos < end) {
        ᯀᯮᯞ text[pos++];
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    ᯀᯮᯝ(pos > begin) {
        ᯀᯮᯞ text[--pos];
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    ᯀᯮᯝ(position < begin) {
        pos = begin;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(position > end) {
        pos = end;
    } ᯀᯮᯝᯂᯪ {
        pos = position;
    }
    ᯀᯮᯝ(pos < end) {
        ᯀᯮᯞ text[pos];
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    ᯀᯮᯝ (pos >= begin && pos < end) {
        ᯀᯮᯞ text[pos];
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    ᯀᯮᯝ (pos + 1 < end) {
        ᯀᯮᯞ text[++pos];
    } ᯀᯮᯝᯂᯪ {
        /* make current() return DONE */
        pos = end;
        ᯀᯮᯞ DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    ᯀᯮᯝ (pos < end) {
        ᯀᯮᯞ text[pos++];
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ᯀᯮᯞ pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    ᯀᯮᯝ (pos > begin) {
        ᯀᯮᯞ text[--pos];
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ᯀᯮᯞ pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    ᯀᯮᯝ(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ᯀᯮᯞ c;
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    ᯀᯮᯝ(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ᯀᯮᯞ c;
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    ᯀᯮᯝ(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ᯀᯮᯞ c;
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    ᯀᯮᯝ(position < begin) {
        position = begin;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(position > end) {
        position = end;
    }
    ᯀᯮᯝ(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ᯀᯮᯞ c;
    } ᯀᯮᯝᯂᯪ {
        this->pos = position;
        ᯀᯮᯞ DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    ᯀᯮᯝ (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ᯀᯮᯞ c;
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    ᯀᯮᯝ (pos < end) {
        U16_FWD_1(text, pos, end);
        ᯀᯮᯝ(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ᯀᯮᯞ c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ᯀᯮᯞ DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    ᯀᯮᯝ (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ᯀᯮᯞ c;
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    ᯀᯮᯝ (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ᯀᯮᯞ c;
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ DONE;
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

    ᯀᯮᯝ(pos < begin) {
        pos = begin;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(pos > end) {
        pos = end;
    }

    ᯀᯮᯞ pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    pak_sa kStart:
        pos = begin;
        ᯀᯮᯝ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        bha_nga;
    pak_sa kCurrent:
        ᯀᯮᯝ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } ᯀᯮᯝᯂᯪ {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    pak_sa kEnd:
        pos = end;
        ᯀᯮᯝ(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ᯀᯮᯞ pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    ᯀᯮᯝ (newText == nullptr || newTextLength < 0) {
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
