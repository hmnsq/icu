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
    החזר *هذا;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

منطقي
UCharCharacterIterator::operator==(ثابت ForwardCharacterIterator& that) ثابت {
    אם (هذا == &that) {
        החזר אמת;
    }
    אם (typeid(*هذا) != typeid(that)) {
        החזר שקר;
    }

    ثابت UCharCharacterIterator&    realThat = static_cast<ثابت UCharCharacterIterator&>(that);

    החזר text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() ثابت {
    החזר ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ثابت {
    החזר جديد UCharCharacterIterator(*هذا);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    אם(pos < end) {
        החזר text[pos];
    } אחרת {
        החזר DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    אם(pos < end) {
        החזר text[pos++];
    } אחרת {
        החזר DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    אם(pos > begin) {
        החזר text[--pos];
    } אחרת {
        החזר DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    אם(position < begin) {
        pos = begin;
    } אחרת אם(position > end) {
        pos = end;
    } אחרת {
        pos = position;
    }
    אם(pos < end) {
        החזר text[pos];
    } אחרת {
        החזר DONE;
    }
}

char16_t
UCharCharacterIterator::current() ثابت {
    אם (pos >= begin && pos < end) {
        החזר text[pos];
    } אחרת {
        החזר DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    אם (pos + 1 < end) {
        החזר text[++pos];
    } אחרת {
        /* make current() return DONE */
        pos = end;
        החזר DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    אם (pos < end) {
        החזר text[pos++];
    } אחרת {
        החזר DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    החזר pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    אם (pos > begin) {
        החזר text[--pos];
    } אחרת {
        החזר DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    החזר pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    אם(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        החזר c;
    } אחרת {
        החזר DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    אם(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        החזר c;
    } אחרת {
        החזר DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    אם(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        החזר c;
    } אחרת {
        החזר DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    אם(position < begin) {
        position = begin;
    } אחרת אם(position > end) {
        position = end;
    }
    אם(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = هذا->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        החזר c;
    } אחרת {
        هذا->pos = position;
        החזר DONE;
    }
}

UChar32
UCharCharacterIterator::current32() ثابت {
    אם (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        החזר c;
    } אחרת {
        החזר DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    אם (pos < end) {
        U16_FWD_1(text, pos, end);
        אם(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            החזר c;
        }
    }
    /* make current() return DONE */
    pos = end;
    החזר DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    אם (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        החזר c;
    } אחרת {
        החזר DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    אם (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        החזר c;
    } אחרת {
        החזר DONE;
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

    אם(pos < begin) {
        pos = begin;
    } אחרת אם(pos > end) {
        pos = end;
    }

    החזר pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    بدل(origin) {
    حالة kStart:
        pos = begin;
        אם(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        اكسر;
    حالة kCurrent:
        אם(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } אחרת {
            U16_BACK_N(text, begin, pos, -delta);
        }
        اكسر;
    حالة kEnd:
        pos = end;
        אם(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        اكسر;
    افتراضي:
        اكسر;
    }

    החזר pos;
}

فراغ UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    אם (newText == nullptr || newTextLength < 0) {
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
