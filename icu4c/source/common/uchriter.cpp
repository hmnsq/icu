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
    ආපසු *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    නම් (this == &that) {
        ආපසු true;
    }
    නම් (typeid(*this) != typeid(that)) {
        ආපසු false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    ආපසු text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    ආපසු ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    ආපසු new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    නම්(pos < end) {
        ආපසු text[pos];
    } නැතහොත් {
        ආපසු DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    නම්(pos < end) {
        ආපසු text[pos++];
    } නැතහොත් {
        ආපසු DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    නම්(pos > begin) {
        ආපසු text[--pos];
    } නැතහොත් {
        ආපසු DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    නම්(position < begin) {
        pos = begin;
    } නැතහොත් නම්(position > end) {
        pos = end;
    } නැතහොත් {
        pos = position;
    }
    නම්(pos < end) {
        ආපසු text[pos];
    } නැතහොත් {
        ආපසු DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    නම් (pos >= begin && pos < end) {
        ආපසු text[pos];
    } නැතහොත් {
        ආපසු DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    නම් (pos + 1 < end) {
        ආපසු text[++pos];
    } නැතහොත් {
        /* make current() return DONE */
        pos = end;
        ආපසු DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    නම් (pos < end) {
        ආපසු text[pos++];
    } නැතහොත් {
        ආපසු DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ආපසු pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    නම් (pos > begin) {
        ආපසු text[--pos];
    } නැතහොත් {
        ආපසු DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ආපසු pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    නම්(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ආපසු c;
    } නැතහොත් {
        ආපසු DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    නම්(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ආපසු c;
    } නැතහොත් {
        ආපසු DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    නම්(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ආපසු c;
    } නැතහොත් {
        ආපසු DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    නම්(position < begin) {
        position = begin;
    } නැතහොත් නම්(position > end) {
        position = end;
    }
    නම්(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ආපසු c;
    } නැතහොත් {
        this->pos = position;
        ආපසු DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    නම් (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ආපසු c;
    } නැතහොත් {
        ආපසු DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    නම් (pos < end) {
        U16_FWD_1(text, pos, end);
        නම්(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ආපසු c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ආපසු DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    නම් (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ආපසු c;
    } නැතහොත් {
        ආපසු DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    නම් (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ආපසු c;
    } නැතහොත් {
        ආපසු DONE;
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

    නම්(pos < begin) {
        pos = begin;
    } නැතහොත් නම්(pos > end) {
        pos = end;
    }

    ආපසු pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    pak_sa kStart:
        pos = begin;
        නම්(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        bha_nga;
    pak_sa kCurrent:
        නම්(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } නැතහොත් {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    pak_sa kEnd:
        pos = end;
        නම්(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ආපසු pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    නම් (newText == nullptr || newTextLength < 0) {
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
