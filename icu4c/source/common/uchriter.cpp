// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#અન્તર્ભાવ "utypeinfo.h"  // for 'typeid' to work

#અન્તર્ભાવ "unicode/uchriter.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "ustr_imp.h"

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

UCharCharacterIterator::UCharCharacterIterator(અચલ UCharCharacterIterator& that)
: CharacterIterator(that),
  text(that.text)
{
}

UCharCharacterIterator&
UCharCharacterIterator::operator=(અચલ UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::operator=(that);
    text = that.text;
    પાછા *આ;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

તાર્કિક
UCharCharacterIterator::operator==(અચલ ForwardCharacterIterator& that) અચલ {
    જો (આ == &that) {
        પાછા સાચું;
    }
    જો (typeid(*આ) != typeid(that)) {
        પાછા ખોટું;
    }

    અચલ UCharCharacterIterator&    realThat = static_cast<અચલ UCharCharacterIterator&>(that);

    પાછા text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() અચલ {
    પાછા ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() અચલ {
    પાછા નવું UCharCharacterIterator(*આ);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    જો(pos < end) {
        પાછા text[pos];
    } નહીંતર {
        પાછા DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    જો(pos < end) {
        પાછા text[pos++];
    } નહીંતર {
        પાછા DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    જો(pos > begin) {
        પાછા text[--pos];
    } નહીંતર {
        પાછા DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    જો(position < begin) {
        pos = begin;
    } નહીંતર જો(position > end) {
        pos = end;
    } નહીંતર {
        pos = position;
    }
    જો(pos < end) {
        પાછા text[pos];
    } નહીંતર {
        પાછા DONE;
    }
}

char16_t
UCharCharacterIterator::current() અચલ {
    જો (pos >= begin && pos < end) {
        પાછા text[pos];
    } નહીંતર {
        પાછા DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    જો (pos + 1 < end) {
        પાછા text[++pos];
    } નહીંતર {
        /* make current() return DONE */
        pos = end;
        પાછા DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    જો (pos < end) {
        પાછા text[pos++];
    } નહીંતર {
        પાછા DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    પાછા pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    જો (pos > begin) {
        પાછા text[--pos];
    } નહીંતર {
        પાછા DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    પાછા pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    જો(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        પાછા c;
    } નહીંતર {
        પાછા DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    જો(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        પાછા c;
    } નહીંતર {
        પાછા DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    જો(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        પાછા c;
    } નહીંતર {
        પાછા DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    જો(position < begin) {
        position = begin;
    } નહીંતર જો(position > end) {
        position = end;
    }
    જો(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = આ->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        પાછા c;
    } નહીંતર {
        આ->pos = position;
        પાછા DONE;
    }
}

UChar32
UCharCharacterIterator::current32() અચલ {
    જો (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        પાછા c;
    } નહીંતર {
        પાછા DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    જો (pos < end) {
        U16_FWD_1(text, pos, end);
        જો(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            પાછા c;
        }
    }
    /* make current() return DONE */
    pos = end;
    પાછા DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    જો (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        પાછા c;
    } નહીંતર {
        પાછા DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    જો (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        પાછા c;
    } નહીંતર {
        પાછા DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    પસંદગી(origin) {
    સ્થિતિ kStart:
        pos = begin + delta;
        તોડો;
    સ્થિતિ kCurrent:
        pos += delta;
        તોડો;
    સ્થિતિ kEnd:
        pos = end + delta;
        તોડો;
    મૂળભૂત:
        તોડો;
    }

    જો(pos < begin) {
        pos = begin;
    } નહીંતર જો(pos > end) {
        pos = end;
    }

    પાછા pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    પસંદગી(origin) {
    સ્થિતિ kStart:
        pos = begin;
        જો(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        તોડો;
    સ્થિતિ kCurrent:
        જો(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } નહીંતર {
            U16_BACK_N(text, begin, pos, -delta);
        }
        તોડો;
    સ્થિતિ kEnd:
        pos = end;
        જો(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        તોડો;
    મૂળભૂત:
        તોડો;
    }

    પાછા pos;
}

શૂન્ય UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    જો (newText == nullptr || newTextLength < 0) {
        newTextLength = 0;
    }
    end = textLength = newTextLength;
    pos = begin = 0;
}

શૂન્ય
UCharCharacterIterator::getText(UnicodeString& result) {
    result = UnicodeString(text, textLength);
}

U_NAMESPACE_END
