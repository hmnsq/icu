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
    ပြန်ပေး *this;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::operator==(const ForwardCharacterIterator& that) const {
    အကယ်၍ (this == &that) {
        ပြန်ပေး true;
    }
    အကယ်၍ (typeid(*this) != typeid(that)) {
        ပြန်ပေး false;
    }

    const UCharCharacterIterator&    realThat = static_cast<const UCharCharacterIterator&>(that);

    ပြန်ပေး text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() const {
    ပြန်ပေး ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() const {
    ပြန်ပေး new UCharCharacterIterator(*this);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    အကယ်၍(pos < end) {
        ပြန်ပေး text[pos];
    } မဟုတ်ပါက {
        ပြန်ပေး DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    အကယ်၍(pos < end) {
        ပြန်ပေး text[pos++];
    } မဟုတ်ပါက {
        ပြန်ပေး DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    အကယ်၍(pos > begin) {
        ပြန်ပေး text[--pos];
    } မဟုတ်ပါက {
        ပြန်ပေး DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    အကယ်၍(position < begin) {
        pos = begin;
    } မဟုတ်ပါက အကယ်၍(position > end) {
        pos = end;
    } မဟုတ်ပါက {
        pos = position;
    }
    အကယ်၍(pos < end) {
        ပြန်ပေး text[pos];
    } မဟုတ်ပါက {
        ပြန်ပေး DONE;
    }
}

char16_t
UCharCharacterIterator::current() const {
    အကယ်၍ (pos >= begin && pos < end) {
        ပြန်ပေး text[pos];
    } မဟုတ်ပါက {
        ပြန်ပေး DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    အကယ်၍ (pos + 1 < end) {
        ပြန်ပေး text[++pos];
    } မဟုတ်ပါက {
        /* make current() return DONE */
        pos = end;
        ပြန်ပေး DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    အကယ်၍ (pos < end) {
        ပြန်ပေး text[pos++];
    } မဟုတ်ပါက {
        ပြန်ပေး DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ပြန်ပေး pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    အကယ်၍ (pos > begin) {
        ပြန်ပေး text[--pos];
    } မဟုတ်ပါက {
        ပြန်ပေး DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ပြန်ပေး pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    အကယ်၍(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ပြန်ပေး c;
    } မဟုတ်ပါက {
        ပြန်ပေး DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    အကယ်၍(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ပြန်ပေး c;
    } မဟုတ်ပါက {
        ပြန်ပေး DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    အကယ်၍(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ပြန်ပေး c;
    } မဟုတ်ပါက {
        ပြန်ပေး DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    အကယ်၍(position < begin) {
        position = begin;
    } မဟုတ်ပါက အကယ်၍(position > end) {
        position = end;
    }
    အကယ်၍(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = this->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ပြန်ပေး c;
    } မဟုတ်ပါက {
        this->pos = position;
        ပြန်ပေး DONE;
    }
}

UChar32
UCharCharacterIterator::current32() const {
    အကယ်၍ (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ပြန်ပေး c;
    } မဟုတ်ပါက {
        ပြန်ပေး DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    အကယ်၍ (pos < end) {
        U16_FWD_1(text, pos, end);
        အကယ်၍(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ပြန်ပေး c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ပြန်ပေး DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    အကယ်၍ (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ပြန်ပေး c;
    } မဟုတ်ပါက {
        ပြန်ပေး DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    အကယ်၍ (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ပြန်ပေး c;
    } မဟုတ်ပါက {
        ပြန်ပေး DONE;
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

    အကယ်၍(pos < begin) {
        pos = begin;
    } မဟုတ်ပါက အကယ်၍(pos > end) {
        pos = end;
    }

    ပြန်ပေး pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    switch(origin) {
    pak_sa kStart:
        pos = begin;
        အကယ်၍(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        bha_nga;
    pak_sa kCurrent:
        အကယ်၍(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } မဟုတ်ပါက {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    pak_sa kEnd:
        pos = end;
        အကယ်၍(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ပြန်ပေး pos;
}

void UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    အကယ်၍ (newText == nullptr || newTextLength < 0) {
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
