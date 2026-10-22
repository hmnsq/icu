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
    ལོག *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    གལ་སྲིད (this == &that) {
        ལོག true;
    }
    གལ་སྲིད (typeid(*this) != typeid(that)) {
        ལོག false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    ལོག text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    ལོག ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    ལོག new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    གལ་སྲིད(pos < end) {
        ལོག text[pos];
    } དེ་མིན {
        ལོག DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    གལ་སྲིད(pos < end) {
        ལོག text[pos++];
    } དེ་མིན {
        ལོག DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    གལ་སྲིད(pos > begin) {
        ལོག text[--pos];
    } དེ་མིན {
        ལོག DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    གལ་སྲིད(position < begin) {
        pos = begin;
    } དེ་མིན གལ་སྲིད(position > end) {
        pos = end;
    } དེ་མིན {
        pos = position;
    }
    གལ་སྲིད(pos < end) {
        ལོག text[pos];
    } དེ་མིན {
        ལོག DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    གལ་སྲིད (pos >= begin && pos < end) {
        ལོག text[pos];
    } དེ་མིན {
        ལོག DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    གལ་སྲིད (pos + 1 < end) {
        ལོག text[++pos];
    } དེ་མིན {
        /* make current() return DONE */
        pos = end;
        ལོག DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    གལ་སྲིད (pos < end) {
        ལོག text[pos++];
    } དེ་མིན {
        ལོག DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ལོག pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    གལ་སྲིད (pos > begin) {
        ལོག text[--pos];
    } དེ་མིན {
        ལོག DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ལོག pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    གལ་སྲིད(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ལོག c;
    } དེ་མིན {
        ལོག DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    གལ་སྲིད(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ལོག c;
    } དེ་མིན {
        ལོག DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    གལ་སྲིད(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ལོག c;
    } དེ་མིན {
        ལོག DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    གལ་སྲིད(position < begin) {
        position = begin;
    } དེ་མིན གལ་སྲིད(position > end) {
        position = end;
    }
    གལ་སྲིད(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ལོག c;
    } དེ་མིན {
        this->pos = position;
        ལོག DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    གལ་སྲིད (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ལོག c;
    } དེ་མིན {
        ལོག DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    གལ་སྲིད (pos < end) {
        U16_FWD_1(text, pos, end);
        གལ་སྲིད(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ལོག c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ལོག DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    གལ་སྲིད (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ལོག c;
    } དེ་མིན {
        ལོག DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    གལ་སྲིད (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ལོག c;
    } དེ་མིན {
        ལོག DONE;
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

    གལ་སྲིད(pos < begin) {
        pos = begin;
    } དེ་མིན གལ་སྲིད(pos > end) {
        pos = end;
    }

    ལོག pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    pak_sa kStart:
        pos = begin;
        གལ་སྲིད(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        bha_nga;
    pak_sa kCurrent:
        གལ་སྲིད(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } དེ་མིན {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    pak_sa kEnd:
        pos = end;
        གལ་སྲིད(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ལོག pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    གལ་སྲིད (newText == nullptr || newTextLength < 0) {
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
