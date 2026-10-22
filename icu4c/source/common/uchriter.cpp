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
    ត្រឡប់ *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    ប្រសិនបើ (this == &that) {
        ត្រឡប់ true;
    }
    ប្រសិនបើ (typeid(*this) != typeid(that)) {
        ត្រឡប់ false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    ត្រឡប់ text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    ត្រឡប់ ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    ត្រឡប់ new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    ប្រសិនបើ(pos < end) {
        ត្រឡប់ text[pos];
    } បើមិនដូច្នេះ {
        ត្រឡប់ DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    ប្រសិនបើ(pos < end) {
        ត្រឡប់ text[pos++];
    } បើមិនដូច្នេះ {
        ត្រឡប់ DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    ប្រសិនបើ(pos > begin) {
        ត្រឡប់ text[--pos];
    } បើមិនដូច្នេះ {
        ត្រឡប់ DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    ប្រសិនបើ(position < begin) {
        pos = begin;
    } បើមិនដូច្នេះ ប្រសិនបើ(position > end) {
        pos = end;
    } បើមិនដូច្នេះ {
        pos = position;
    }
    ប្រសិនបើ(pos < end) {
        ត្រឡប់ text[pos];
    } បើមិនដូច្នេះ {
        ត្រឡប់ DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    ប្រសិនបើ (pos >= begin && pos < end) {
        ត្រឡប់ text[pos];
    } បើមិនដូច្នេះ {
        ត្រឡប់ DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    ប្រសិនបើ (pos + 1 < end) {
        ត្រឡប់ text[++pos];
    } បើមិនដូច្នេះ {
        /* make current() return DONE */
        pos = end;
        ត្រឡប់ DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    ប្រសិនបើ (pos < end) {
        ត្រឡប់ text[pos++];
    } បើមិនដូច្នេះ {
        ត្រឡប់ DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ត្រឡប់ pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    ប្រសិនបើ (pos > begin) {
        ត្រឡប់ text[--pos];
    } បើមិនដូច្នេះ {
        ត្រឡប់ DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ត្រឡប់ pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    ប្រសិនបើ(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ត្រឡប់ c;
    } បើមិនដូច្នេះ {
        ត្រឡប់ DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    ប្រសិនបើ(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ត្រឡប់ c;
    } បើមិនដូច្នេះ {
        ត្រឡប់ DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    ប្រសិនបើ(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ត្រឡប់ c;
    } បើមិនដូច្នេះ {
        ត្រឡប់ DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    ប្រសិនបើ(position < begin) {
        position = begin;
    } បើមិនដូច្នេះ ប្រសិនបើ(position > end) {
        position = end;
    }
    ប្រសិនបើ(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ត្រឡប់ c;
    } បើមិនដូច្នេះ {
        this->pos = position;
        ត្រឡប់ DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    ប្រសិនបើ (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ត្រឡប់ c;
    } បើមិនដូច្នេះ {
        ត្រឡប់ DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    ប្រសិនបើ (pos < end) {
        U16_FWD_1(text, pos, end);
        ប្រសិនបើ(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ត្រឡប់ c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ត្រឡប់ DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    ប្រសិនបើ (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ត្រឡប់ c;
    } បើមិនដូច្នេះ {
        ត្រឡប់ DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    ប្រសិនបើ (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ត្រឡប់ c;
    } បើមិនដូច្នេះ {
        ត្រឡប់ DONE;
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

    ប្រសិនបើ(pos < begin) {
        pos = begin;
    } បើមិនដូច្នេះ ប្រសិនបើ(pos > end) {
        pos = end;
    }

    ត្រឡប់ pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    pak_sa kStart:
        pos = begin;
        ប្រសិនបើ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        bha_nga;
    pak_sa kCurrent:
        ប្រសិនបើ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } បើមិនដូច្នេះ {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    pak_sa kEnd:
        pos = end;
        ប្រសិនបើ(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ត្រឡប់ pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    ប្រសិនបើ (newText == nullptr || newTextLength < 0) {
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
