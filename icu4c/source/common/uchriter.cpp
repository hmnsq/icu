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
    ສົ່ງຄືນ *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    ຖ້າ (this == &that) {
        ສົ່ງຄືນ true;
    }
    ຖ້າ (typeid(*this) != typeid(that)) {
        ສົ່ງຄືນ false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    ສົ່ງຄືນ text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    ສົ່ງຄືນ ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    ສົ່ງຄືນ new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    ຖ້າ(pos < end) {
        ສົ່ງຄືນ text[pos];
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    ຖ້າ(pos < end) {
        ສົ່ງຄືນ text[pos++];
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    ຖ້າ(pos > begin) {
        ສົ່ງຄືນ text[--pos];
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    ຖ້າ(position < begin) {
        pos = begin;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ(position > end) {
        pos = end;
    } ບໍ່ດັ່ງນັ້ນ {
        pos = position;
    }
    ຖ້າ(pos < end) {
        ສົ່ງຄືນ text[pos];
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    ຖ້າ (pos >= begin && pos < end) {
        ສົ່ງຄືນ text[pos];
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    ຖ້າ (pos + 1 < end) {
        ສົ່ງຄືນ text[++pos];
    } ບໍ່ດັ່ງນັ້ນ {
        /* make current() return DONE */
        pos = end;
        ສົ່ງຄືນ DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    ຖ້າ (pos < end) {
        ສົ່ງຄືນ text[pos++];
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ສົ່ງຄືນ pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    ຖ້າ (pos > begin) {
        ສົ່ງຄືນ text[--pos];
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ສົ່ງຄືນ pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    ຖ້າ(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ສົ່ງຄືນ c;
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    ຖ້າ(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ສົ່ງຄືນ c;
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    ຖ້າ(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ສົ່ງຄືນ c;
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    ຖ້າ(position < begin) {
        position = begin;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ(position > end) {
        position = end;
    }
    ຖ້າ(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ສົ່ງຄືນ c;
    } ບໍ່ດັ່ງນັ້ນ {
        this->pos = position;
        ສົ່ງຄືນ DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    ຖ້າ (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ສົ່ງຄືນ c;
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    ຖ້າ (pos < end) {
        U16_FWD_1(text, pos, end);
        ຖ້າ(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ສົ່ງຄືນ c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ສົ່ງຄືນ DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    ຖ້າ (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ສົ່ງຄືນ c;
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    ຖ້າ (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ສົ່ງຄືນ c;
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ DONE;
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

    ຖ້າ(pos < begin) {
        pos = begin;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ(pos > end) {
        pos = end;
    }

    ສົ່ງຄືນ pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    pak_sa kStart:
        pos = begin;
        ຖ້າ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        bha_nga;
    pak_sa kCurrent:
        ຖ້າ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } ບໍ່ດັ່ງນັ້ນ {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    pak_sa kEnd:
        pos = end;
        ຖ້າ(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ສົ່ງຄືນ pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    ຖ້າ (newText == nullptr || newTextLength < 0) {
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
