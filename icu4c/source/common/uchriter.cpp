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
    ᰀᰧᰌ *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    ᰀᰧ (this == &that) {
        ᰀᰧᰌ true;
    }
    ᰀᰧ (typeid(*this) != typeid(that)) {
        ᰀᰧᰌ false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    ᰀᰧᰌ text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    ᰀᰧᰌ ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    ᰀᰧᰌ new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    ᰀᰧ(pos < end) {
        ᰀᰧᰌ text[pos];
    } ᰀᰧᰀ {
        ᰀᰧᰌ DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    ᰀᰧ(pos < end) {
        ᰀᰧᰌ text[pos++];
    } ᰀᰧᰀ {
        ᰀᰧᰌ DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    ᰀᰧ(pos > begin) {
        ᰀᰧᰌ text[--pos];
    } ᰀᰧᰀ {
        ᰀᰧᰌ DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    ᰀᰧ(position < begin) {
        pos = begin;
    } ᰀᰧᰀ ᰀᰧ(position > end) {
        pos = end;
    } ᰀᰧᰀ {
        pos = position;
    }
    ᰀᰧ(pos < end) {
        ᰀᰧᰌ text[pos];
    } ᰀᰧᰀ {
        ᰀᰧᰌ DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    ᰀᰧ (pos >= begin && pos < end) {
        ᰀᰧᰌ text[pos];
    } ᰀᰧᰀ {
        ᰀᰧᰌ DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    ᰀᰧ (pos + 1 < end) {
        ᰀᰧᰌ text[++pos];
    } ᰀᰧᰀ {
        /* make current() return DONE */
        pos = end;
        ᰀᰧᰌ DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    ᰀᰧ (pos < end) {
        ᰀᰧᰌ text[pos++];
    } ᰀᰧᰀ {
        ᰀᰧᰌ DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ᰀᰧᰌ pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    ᰀᰧ (pos > begin) {
        ᰀᰧᰌ text[--pos];
    } ᰀᰧᰀ {
        ᰀᰧᰌ DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ᰀᰧᰌ pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    ᰀᰧ(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ᰀᰧᰌ c;
    } ᰀᰧᰀ {
        ᰀᰧᰌ DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    ᰀᰧ(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ᰀᰧᰌ c;
    } ᰀᰧᰀ {
        ᰀᰧᰌ DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    ᰀᰧ(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ᰀᰧᰌ c;
    } ᰀᰧᰀ {
        ᰀᰧᰌ DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    ᰀᰧ(position < begin) {
        position = begin;
    } ᰀᰧᰀ ᰀᰧ(position > end) {
        position = end;
    }
    ᰀᰧ(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ᰀᰧᰌ c;
    } ᰀᰧᰀ {
        this->pos = position;
        ᰀᰧᰌ DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    ᰀᰧ (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ᰀᰧᰌ c;
    } ᰀᰧᰀ {
        ᰀᰧᰌ DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    ᰀᰧ (pos < end) {
        U16_FWD_1(text, pos, end);
        ᰀᰧ(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ᰀᰧᰌ c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ᰀᰧᰌ DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    ᰀᰧ (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ᰀᰧᰌ c;
    } ᰀᰧᰀ {
        ᰀᰧᰌ DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    ᰀᰧ (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ᰀᰧᰌ c;
    } ᰀᰧᰀ {
        ᰀᰧᰌ DONE;
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

    ᰀᰧ(pos < begin) {
        pos = begin;
    } ᰀᰧᰀ ᰀᰧ(pos > end) {
        pos = end;
    }

    ᰀᰧᰌ pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    pak_sa kStart:
        pos = begin;
        ᰀᰧ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        bha_nga;
    pak_sa kCurrent:
        ᰀᰧ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } ᰀᰧᰀ {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    pak_sa kEnd:
        pos = end;
        ᰀᰧ(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ᰀᰧᰌ pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    ᰀᰧ (newText == nullptr || newTextLength < 0) {
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
