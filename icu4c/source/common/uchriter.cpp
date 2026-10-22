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
    ᬯᬮᬶ *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    ᬬᬾᬦ᭄ (this == &that) {
        ᬯᬮᬶ true;
    }
    ᬬᬾᬦ᭄ (typeid(*this) != typeid(that)) {
        ᬯᬮᬶ false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    ᬯᬮᬶ text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    ᬯᬮᬶ ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    ᬯᬮᬶ new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    ᬬᬾᬦ᭄(pos < end) {
        ᬯᬮᬶ text[pos];
    } ᬮᬶᬬ {
        ᬯᬮᬶ DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    ᬬᬾᬦ᭄(pos < end) {
        ᬯᬮᬶ text[pos++];
    } ᬮᬶᬬ {
        ᬯᬮᬶ DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    ᬬᬾᬦ᭄(pos > begin) {
        ᬯᬮᬶ text[--pos];
    } ᬮᬶᬬ {
        ᬯᬮᬶ DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    ᬬᬾᬦ᭄(position < begin) {
        pos = begin;
    } ᬮᬶᬬ ᬬᬾᬦ᭄(position > end) {
        pos = end;
    } ᬮᬶᬬ {
        pos = position;
    }
    ᬬᬾᬦ᭄(pos < end) {
        ᬯᬮᬶ text[pos];
    } ᬮᬶᬬ {
        ᬯᬮᬶ DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    ᬬᬾᬦ᭄ (pos >= begin && pos < end) {
        ᬯᬮᬶ text[pos];
    } ᬮᬶᬬ {
        ᬯᬮᬶ DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    ᬬᬾᬦ᭄ (pos + 1 < end) {
        ᬯᬮᬶ text[++pos];
    } ᬮᬶᬬ {
        /* make current() return DONE */
        pos = end;
        ᬯᬮᬶ DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    ᬬᬾᬦ᭄ (pos < end) {
        ᬯᬮᬶ text[pos++];
    } ᬮᬶᬬ {
        ᬯᬮᬶ DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ᬯᬮᬶ pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    ᬬᬾᬦ᭄ (pos > begin) {
        ᬯᬮᬶ text[--pos];
    } ᬮᬶᬬ {
        ᬯᬮᬶ DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ᬯᬮᬶ pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    ᬬᬾᬦ᭄(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ᬯᬮᬶ c;
    } ᬮᬶᬬ {
        ᬯᬮᬶ DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    ᬬᬾᬦ᭄(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ᬯᬮᬶ c;
    } ᬮᬶᬬ {
        ᬯᬮᬶ DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    ᬬᬾᬦ᭄(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ᬯᬮᬶ c;
    } ᬮᬶᬬ {
        ᬯᬮᬶ DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    ᬬᬾᬦ᭄(position < begin) {
        position = begin;
    } ᬮᬶᬬ ᬬᬾᬦ᭄(position > end) {
        position = end;
    }
    ᬬᬾᬦ᭄(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ᬯᬮᬶ c;
    } ᬮᬶᬬ {
        this->pos = position;
        ᬯᬮᬶ DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    ᬬᬾᬦ᭄ (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ᬯᬮᬶ c;
    } ᬮᬶᬬ {
        ᬯᬮᬶ DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    ᬬᬾᬦ᭄ (pos < end) {
        U16_FWD_1(text, pos, end);
        ᬬᬾᬦ᭄(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ᬯᬮᬶ c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ᬯᬮᬶ DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    ᬬᬾᬦ᭄ (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ᬯᬮᬶ c;
    } ᬮᬶᬬ {
        ᬯᬮᬶ DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    ᬬᬾᬦ᭄ (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ᬯᬮᬶ c;
    } ᬮᬶᬬ {
        ᬯᬮᬶ DONE;
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

    ᬬᬾᬦ᭄(pos < begin) {
        pos = begin;
    } ᬮᬶᬬ ᬬᬾᬦ᭄(pos > end) {
        pos = end;
    }

    ᬯᬮᬶ pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    pak_sa kStart:
        pos = begin;
        ᬬᬾᬦ᭄(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        bha_nga;
    pak_sa kCurrent:
        ᬬᬾᬦ᭄(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } ᬮᬶᬬ {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    pak_sa kEnd:
        pos = end;
        ᬬᬾᬦ᭄(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ᬯᬮᬶ pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    ᬬᬾᬦ᭄ (newText == nullptr || newTextLength < 0) {
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
