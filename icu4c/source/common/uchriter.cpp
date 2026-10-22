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
    ꦮꦭꦶ *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    ꦪꦼꦤ꧀ (this == &that) {
        ꦮꦭꦶ true;
    }
    ꦪꦼꦤ꧀ (typeid(*this) != typeid(that)) {
        ꦮꦭꦶ false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    ꦮꦭꦶ text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    ꦮꦭꦶ ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    ꦮꦭꦶ new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    ꦪꦼꦤ꧀(pos < end) {
        ꦮꦭꦶ text[pos];
    } ꦭꦶꦪ {
        ꦮꦭꦶ DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    ꦪꦼꦤ꧀(pos < end) {
        ꦮꦭꦶ text[pos++];
    } ꦭꦶꦪ {
        ꦮꦭꦶ DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    ꦪꦼꦤ꧀(pos > begin) {
        ꦮꦭꦶ text[--pos];
    } ꦭꦶꦪ {
        ꦮꦭꦶ DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    ꦪꦼꦤ꧀(position < begin) {
        pos = begin;
    } ꦭꦶꦪ ꦪꦼꦤ꧀(position > end) {
        pos = end;
    } ꦭꦶꦪ {
        pos = position;
    }
    ꦪꦼꦤ꧀(pos < end) {
        ꦮꦭꦶ text[pos];
    } ꦭꦶꦪ {
        ꦮꦭꦶ DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    ꦪꦼꦤ꧀ (pos >= begin && pos < end) {
        ꦮꦭꦶ text[pos];
    } ꦭꦶꦪ {
        ꦮꦭꦶ DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    ꦪꦼꦤ꧀ (pos + 1 < end) {
        ꦮꦭꦶ text[++pos];
    } ꦭꦶꦪ {
        /* make current() return DONE */
        pos = end;
        ꦮꦭꦶ DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    ꦪꦼꦤ꧀ (pos < end) {
        ꦮꦭꦶ text[pos++];
    } ꦭꦶꦪ {
        ꦮꦭꦶ DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ꦮꦭꦶ pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    ꦪꦼꦤ꧀ (pos > begin) {
        ꦮꦭꦶ text[--pos];
    } ꦭꦶꦪ {
        ꦮꦭꦶ DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ꦮꦭꦶ pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    ꦪꦼꦤ꧀(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ꦮꦭꦶ c;
    } ꦭꦶꦪ {
        ꦮꦭꦶ DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    ꦪꦼꦤ꧀(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ꦮꦭꦶ c;
    } ꦭꦶꦪ {
        ꦮꦭꦶ DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    ꦪꦼꦤ꧀(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ꦮꦭꦶ c;
    } ꦭꦶꦪ {
        ꦮꦭꦶ DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    ꦪꦼꦤ꧀(position < begin) {
        position = begin;
    } ꦭꦶꦪ ꦪꦼꦤ꧀(position > end) {
        position = end;
    }
    ꦪꦼꦤ꧀(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ꦮꦭꦶ c;
    } ꦭꦶꦪ {
        this->pos = position;
        ꦮꦭꦶ DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    ꦪꦼꦤ꧀ (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ꦮꦭꦶ c;
    } ꦭꦶꦪ {
        ꦮꦭꦶ DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    ꦪꦼꦤ꧀ (pos < end) {
        U16_FWD_1(text, pos, end);
        ꦪꦼꦤ꧀(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ꦮꦭꦶ c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ꦮꦭꦶ DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    ꦪꦼꦤ꧀ (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ꦮꦭꦶ c;
    } ꦭꦶꦪ {
        ꦮꦭꦶ DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    ꦪꦼꦤ꧀ (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ꦮꦭꦶ c;
    } ꦭꦶꦪ {
        ꦮꦭꦶ DONE;
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

    ꦪꦼꦤ꧀(pos < begin) {
        pos = begin;
    } ꦭꦶꦪ ꦪꦼꦤ꧀(pos > end) {
        pos = end;
    }

    ꦮꦭꦶ pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    pak_sa kStart:
        pos = begin;
        ꦪꦼꦤ꧀(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        bha_nga;
    pak_sa kCurrent:
        ꦪꦼꦤ꧀(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } ꦭꦶꦪ {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    pak_sa kEnd:
        pos = end;
        ꦪꦼꦤ꧀(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ꦮꦭꦶ pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    ꦪꦼꦤ꧀ (newText == nullptr || newTextLength < 0) {
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
