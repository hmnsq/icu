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
    ᮝᮜᮤ *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    ᮚᮦᮔ᮪ (this == &that) {
        ᮝᮜᮤ true;
    }
    ᮚᮦᮔ᮪ (typeid(*this) != typeid(that)) {
        ᮝᮜᮤ false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    ᮝᮜᮤ text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    ᮝᮜᮤ ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    ᮝᮜᮤ new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    ᮚᮦᮔ᮪(pos < end) {
        ᮝᮜᮤ text[pos];
    } ᮜᮤᮚ {
        ᮝᮜᮤ DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    ᮚᮦᮔ᮪(pos < end) {
        ᮝᮜᮤ text[pos++];
    } ᮜᮤᮚ {
        ᮝᮜᮤ DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    ᮚᮦᮔ᮪(pos > begin) {
        ᮝᮜᮤ text[--pos];
    } ᮜᮤᮚ {
        ᮝᮜᮤ DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    ᮚᮦᮔ᮪(position < begin) {
        pos = begin;
    } ᮜᮤᮚ ᮚᮦᮔ᮪(position > end) {
        pos = end;
    } ᮜᮤᮚ {
        pos = position;
    }
    ᮚᮦᮔ᮪(pos < end) {
        ᮝᮜᮤ text[pos];
    } ᮜᮤᮚ {
        ᮝᮜᮤ DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    ᮚᮦᮔ᮪ (pos >= begin && pos < end) {
        ᮝᮜᮤ text[pos];
    } ᮜᮤᮚ {
        ᮝᮜᮤ DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    ᮚᮦᮔ᮪ (pos + 1 < end) {
        ᮝᮜᮤ text[++pos];
    } ᮜᮤᮚ {
        /* make current() return DONE */
        pos = end;
        ᮝᮜᮤ DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    ᮚᮦᮔ᮪ (pos < end) {
        ᮝᮜᮤ text[pos++];
    } ᮜᮤᮚ {
        ᮝᮜᮤ DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ᮝᮜᮤ pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    ᮚᮦᮔ᮪ (pos > begin) {
        ᮝᮜᮤ text[--pos];
    } ᮜᮤᮚ {
        ᮝᮜᮤ DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ᮝᮜᮤ pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    ᮚᮦᮔ᮪(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ᮝᮜᮤ c;
    } ᮜᮤᮚ {
        ᮝᮜᮤ DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    ᮚᮦᮔ᮪(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ᮝᮜᮤ c;
    } ᮜᮤᮚ {
        ᮝᮜᮤ DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    ᮚᮦᮔ᮪(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ᮝᮜᮤ c;
    } ᮜᮤᮚ {
        ᮝᮜᮤ DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    ᮚᮦᮔ᮪(position < begin) {
        position = begin;
    } ᮜᮤᮚ ᮚᮦᮔ᮪(position > end) {
        position = end;
    }
    ᮚᮦᮔ᮪(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ᮝᮜᮤ c;
    } ᮜᮤᮚ {
        this->pos = position;
        ᮝᮜᮤ DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    ᮚᮦᮔ᮪ (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ᮝᮜᮤ c;
    } ᮜᮤᮚ {
        ᮝᮜᮤ DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    ᮚᮦᮔ᮪ (pos < end) {
        U16_FWD_1(text, pos, end);
        ᮚᮦᮔ᮪(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ᮝᮜᮤ c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ᮝᮜᮤ DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    ᮚᮦᮔ᮪ (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ᮝᮜᮤ c;
    } ᮜᮤᮚ {
        ᮝᮜᮤ DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    ᮚᮦᮔ᮪ (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ᮝᮜᮤ c;
    } ᮜᮤᮚ {
        ᮝᮜᮤ DONE;
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

    ᮚᮦᮔ᮪(pos < begin) {
        pos = begin;
    } ᮜᮤᮚ ᮚᮦᮔ᮪(pos > end) {
        pos = end;
    }

    ᮝᮜᮤ pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    pak_sa kStart:
        pos = begin;
        ᮚᮦᮔ᮪(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        bha_nga;
    pak_sa kCurrent:
        ᮚᮦᮔ᮪(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } ᮜᮤᮚ {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    pak_sa kEnd:
        pos = end;
        ᮚᮦᮔ᮪(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ᮝᮜᮤ pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    ᮚᮦᮔ᮪ (newText == nullptr || newTextLength < 0) {
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
