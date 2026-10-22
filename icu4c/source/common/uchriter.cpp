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
    ᱨᱩᱣᱟᱹᱲ *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    ᱡᱩᱫᱤ (this == &that) {
        ᱨᱩᱣᱟᱹᱲ true;
    }
    ᱡᱩᱫᱤ (typeid(*this) != typeid(that)) {
        ᱨᱩᱣᱟᱹᱲ false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    ᱨᱩᱣᱟᱹᱲ text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    ᱨᱩᱣᱟᱹᱲ ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    ᱨᱩᱣᱟᱹᱲ new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    ᱡᱩᱫᱤ(pos < end) {
        ᱨᱩᱣᱟᱹᱲ text[pos];
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    ᱡᱩᱫᱤ(pos < end) {
        ᱨᱩᱣᱟᱹᱲ text[pos++];
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    ᱡᱩᱫᱤ(pos > begin) {
        ᱨᱩᱣᱟᱹᱲ text[--pos];
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    ᱡᱩᱫᱤ(position < begin) {
        pos = begin;
    } ᱵᱟᱝ ᱡᱩᱫᱤ(position > end) {
        pos = end;
    } ᱵᱟᱝ {
        pos = position;
    }
    ᱡᱩᱫᱤ(pos < end) {
        ᱨᱩᱣᱟᱹᱲ text[pos];
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    ᱡᱩᱫᱤ (pos >= begin && pos < end) {
        ᱨᱩᱣᱟᱹᱲ text[pos];
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    ᱡᱩᱫᱤ (pos + 1 < end) {
        ᱨᱩᱣᱟᱹᱲ text[++pos];
    } ᱵᱟᱝ {
        /* make current() return DONE */
        pos = end;
        ᱨᱩᱣᱟᱹᱲ DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    ᱡᱩᱫᱤ (pos < end) {
        ᱨᱩᱣᱟᱹᱲ text[pos++];
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ᱨᱩᱣᱟᱹᱲ pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    ᱡᱩᱫᱤ (pos > begin) {
        ᱨᱩᱣᱟᱹᱲ text[--pos];
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ᱨᱩᱣᱟᱹᱲ pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    ᱡᱩᱫᱤ(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ᱨᱩᱣᱟᱹᱲ c;
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    ᱡᱩᱫᱤ(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ᱨᱩᱣᱟᱹᱲ c;
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    ᱡᱩᱫᱤ(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ᱨᱩᱣᱟᱹᱲ c;
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    ᱡᱩᱫᱤ(position < begin) {
        position = begin;
    } ᱵᱟᱝ ᱡᱩᱫᱤ(position > end) {
        position = end;
    }
    ᱡᱩᱫᱤ(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ᱨᱩᱣᱟᱹᱲ c;
    } ᱵᱟᱝ {
        this->pos = position;
        ᱨᱩᱣᱟᱹᱲ DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    ᱡᱩᱫᱤ (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ᱨᱩᱣᱟᱹᱲ c;
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    ᱡᱩᱫᱤ (pos < end) {
        U16_FWD_1(text, pos, end);
        ᱡᱩᱫᱤ(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ᱨᱩᱣᱟᱹᱲ c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ᱨᱩᱣᱟᱹᱲ DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    ᱡᱩᱫᱤ (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ᱨᱩᱣᱟᱹᱲ c;
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    ᱡᱩᱫᱤ (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ᱨᱩᱣᱟᱹᱲ c;
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ DONE;
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

    ᱡᱩᱫᱤ(pos < begin) {
        pos = begin;
    } ᱵᱟᱝ ᱡᱩᱫᱤ(pos > end) {
        pos = end;
    }

    ᱨᱩᱣᱟᱹᱲ pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    pak_sa kStart:
        pos = begin;
        ᱡᱩᱫᱤ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        bha_nga;
    pak_sa kCurrent:
        ᱡᱩᱫᱤ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } ᱵᱟᱝ {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    pak_sa kEnd:
        pos = end;
        ᱡᱩᱫᱤ(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ᱨᱩᱣᱟᱹᱲ pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    ᱡᱩᱫᱤ (newText == nullptr || newTextLength < 0) {
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
