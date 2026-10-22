// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#ಅನ್ತರ್ಭಾವ "utypeinfo.h"  // for 'typeid' to work

#ಅನ್ತರ್ಭಾವ "unicode/uchriter.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

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

UCharCharacterIterator::UCharCharacterIterator(ಧ್ರುವ UCharCharacterIterator& that)
: CharacterIterator(that),
  text(that.text)
{
}

UCharCharacterIterator&
UCharCharacterIterator::operator=(ಧ್ರುವ UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::operator=(that);
    text = that.text;
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

ತಾರ್ಕಿಕ
UCharCharacterIterator::operator==(ಧ್ರುವ ForwardCharacterIterator& that) ಧ್ರುವ {
    ಯದಿ (ಏತತ್ == &that) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ (typeid(*ಏತತ್) != typeid(that)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }

    ಧ್ರುವ UCharCharacterIterator&    realThat = static_cast<ಧ್ರುವ UCharCharacterIterator&>(that);

    ಪ್ರತ್ಯಾಗಮ text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ನೂತನ UCharCharacterIterator(*ಏತತ್);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    ಯದಿ(pos < end) {
        ಪ್ರತ್ಯಾಗಮ text[pos];
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    ಯದಿ(pos < end) {
        ಪ್ರತ್ಯಾಗಮ text[pos++];
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    ಯದಿ(pos > begin) {
        ಪ್ರತ್ಯಾಗಮ text[--pos];
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    ಯದಿ(position < begin) {
        pos = begin;
    } ಅನ್ಯಥಾ ಯದಿ(position > end) {
        pos = end;
    } ಅನ್ಯಥಾ {
        pos = position;
    }
    ಯದಿ(pos < end) {
        ಪ್ರತ್ಯಾಗಮ text[pos];
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

char16_t
UCharCharacterIterator::current() ಧ್ರುವ {
    ಯದಿ (pos >= begin && pos < end) {
        ಪ್ರತ್ಯಾಗಮ text[pos];
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    ಯದಿ (pos + 1 < end) {
        ಪ್ರತ್ಯಾಗಮ text[++pos];
    } ಅನ್ಯಥಾ {
        /* make current() return DONE */
        pos = end;
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    ಯದಿ (pos < end) {
        ಪ್ರತ್ಯಾಗಮ text[pos++];
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ಪ್ರತ್ಯಾಗಮ pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    ಯದಿ (pos > begin) {
        ಪ್ರತ್ಯಾಗಮ text[--pos];
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ಪ್ರತ್ಯಾಗಮ pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    ಯದಿ(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ಪ್ರತ್ಯಾಗಮ c;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    ಯದಿ(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ಪ್ರತ್ಯಾಗಮ c;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    ಯದಿ(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ಪ್ರತ್ಯಾಗಮ c;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    ಯದಿ(position < begin) {
        position = begin;
    } ಅನ್ಯಥಾ ಯದಿ(position > end) {
        position = end;
    }
    ಯದಿ(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = ಏತತ್->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ಪ್ರತ್ಯಾಗಮ c;
    } ಅನ್ಯಥಾ {
        ಏತತ್->pos = position;
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

UChar32
UCharCharacterIterator::current32() ಧ್ರುವ {
    ಯದಿ (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ಪ್ರತ್ಯಾಗಮ c;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    ಯದಿ (pos < end) {
        U16_FWD_1(text, pos, end);
        ಯದಿ(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ಪ್ರತ್ಯಾಗಮ c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ಪ್ರತ್ಯಾಗಮ DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    ಯದಿ (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ಪ್ರತ್ಯಾಗಮ c;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    ಯದಿ (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ಪ್ರತ್ಯಾಗಮ c;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    ವಿಕಲ್ಪ(origin) {
    ಪಕ್ಷ kStart:
        pos = begin + delta;
        ಭಙ್ಗ;
    ಪಕ್ಷ kCurrent:
        pos += delta;
        ಭಙ್ಗ;
    ಪಕ್ಷ kEnd:
        pos = end + delta;
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        ಭಙ್ಗ;
    }

    ಯದಿ(pos < begin) {
        pos = begin;
    } ಅನ್ಯಥಾ ಯದಿ(pos > end) {
        pos = end;
    }

    ಪ್ರತ್ಯಾಗಮ pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    ವಿಕಲ್ಪ(origin) {
    ಪಕ್ಷ kStart:
        pos = begin;
        ಯದಿ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ kCurrent:
        ಯದಿ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } ಅನ್ಯಥಾ {
            U16_BACK_N(text, begin, pos, -delta);
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ kEnd:
        pos = end;
        ಯದಿ(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ pos;
}

ಶೂನ್ಯ UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    ಯದಿ (newText == nullptr || newTextLength < 0) {
        newTextLength = 0;
    }
    end = textLength = newTextLength;
    pos = begin = 0;
}

ಶೂನ್ಯ
UCharCharacterIterator::getText(UnicodeString& result) {
    result = UnicodeString(text, textLength);
}

U_NAMESPACE_END
