// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#ضمن "utypeinfo.h"  // for 'typeid' to work

#ضمن "unicode/uchriter.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/utf16.h"
#ضمن "ustr_imp.h"

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

UCharCharacterIterator::UCharCharacterIterator(ثابت UCharCharacterIterator& that)
: CharacterIterator(that),
  text(that.text)
{
}

UCharCharacterIterator&
UCharCharacterIterator::operator=(ثابت UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::operator=(that);
    text = that.text;
    ܦܢܐ *هذا;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

منطقي
UCharCharacterIterator::operator==(ثابت ForwardCharacterIterator& that) ثابت {
    ܐܢ (هذا == &that) {
        ܦܢܐ ܫܪܪܐ;
    }
    ܐܢ (typeid(*هذا) != typeid(that)) {
        ܦܢܐ ܕܓܠܐ;
    }

    ثابت UCharCharacterIterator&    realThat = static_cast<ثابت UCharCharacterIterator&>(that);

    ܦܢܐ text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() ثابت {
    ܦܢܐ ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ثابت {
    ܦܢܐ جديد UCharCharacterIterator(*هذا);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    ܐܢ(pos < end) {
        ܦܢܐ text[pos];
    } ܐܚܪܢܐ {
        ܦܢܐ DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    ܐܢ(pos < end) {
        ܦܢܐ text[pos++];
    } ܐܚܪܢܐ {
        ܦܢܐ DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    ܐܢ(pos > begin) {
        ܦܢܐ text[--pos];
    } ܐܚܪܢܐ {
        ܦܢܐ DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    ܐܢ(position < begin) {
        pos = begin;
    } ܐܚܪܢܐ ܐܢ(position > end) {
        pos = end;
    } ܐܚܪܢܐ {
        pos = position;
    }
    ܐܢ(pos < end) {
        ܦܢܐ text[pos];
    } ܐܚܪܢܐ {
        ܦܢܐ DONE;
    }
}

char16_t
UCharCharacterIterator::current() ثابت {
    ܐܢ (pos >= begin && pos < end) {
        ܦܢܐ text[pos];
    } ܐܚܪܢܐ {
        ܦܢܐ DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    ܐܢ (pos + 1 < end) {
        ܦܢܐ text[++pos];
    } ܐܚܪܢܐ {
        /* make current() return DONE */
        pos = end;
        ܦܢܐ DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    ܐܢ (pos < end) {
        ܦܢܐ text[pos++];
    } ܐܚܪܢܐ {
        ܦܢܐ DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ܦܢܐ pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    ܐܢ (pos > begin) {
        ܦܢܐ text[--pos];
    } ܐܚܪܢܐ {
        ܦܢܐ DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ܦܢܐ pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    ܐܢ(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ܦܢܐ c;
    } ܐܚܪܢܐ {
        ܦܢܐ DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    ܐܢ(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ܦܢܐ c;
    } ܐܚܪܢܐ {
        ܦܢܐ DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    ܐܢ(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ܦܢܐ c;
    } ܐܚܪܢܐ {
        ܦܢܐ DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    ܐܢ(position < begin) {
        position = begin;
    } ܐܚܪܢܐ ܐܢ(position > end) {
        position = end;
    }
    ܐܢ(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = هذا->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ܦܢܐ c;
    } ܐܚܪܢܐ {
        هذا->pos = position;
        ܦܢܐ DONE;
    }
}

UChar32
UCharCharacterIterator::current32() ثابت {
    ܐܢ (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ܦܢܐ c;
    } ܐܚܪܢܐ {
        ܦܢܐ DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    ܐܢ (pos < end) {
        U16_FWD_1(text, pos, end);
        ܐܢ(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ܦܢܐ c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ܦܢܐ DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    ܐܢ (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ܦܢܐ c;
    } ܐܚܪܢܐ {
        ܦܢܐ DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    ܐܢ (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ܦܢܐ c;
    } ܐܚܪܢܐ {
        ܦܢܐ DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    بدل(origin) {
    حالة kStart:
        pos = begin + delta;
        اكسر;
    حالة kCurrent:
        pos += delta;
        اكسر;
    حالة kEnd:
        pos = end + delta;
        اكسر;
    افتراضي:
        اكسر;
    }

    ܐܢ(pos < begin) {
        pos = begin;
    } ܐܚܪܢܐ ܐܢ(pos > end) {
        pos = end;
    }

    ܦܢܐ pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    بدل(origin) {
    حالة kStart:
        pos = begin;
        ܐܢ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        اكسر;
    حالة kCurrent:
        ܐܢ(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } ܐܚܪܢܐ {
            U16_BACK_N(text, begin, pos, -delta);
        }
        اكسر;
    حالة kEnd:
        pos = end;
        ܐܢ(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        اكسر;
    افتراضي:
        اكسر;
    }

    ܦܢܐ pos;
}

فراغ UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    ܐܢ (newText == nullptr || newTextLength < 0) {
        newTextLength = 0;
    }
    end = textLength = newTextLength;
    pos = begin = 0;
}

فراغ
UCharCharacterIterator::getText(UnicodeString& result) {
    result = UnicodeString(text, textLength);
}

U_NAMESPACE_END
