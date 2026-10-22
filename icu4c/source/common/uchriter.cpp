// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#شامل "utypeinfo.h"  // for 'typeid' to work

#شامل "unicode/uchriter.h"
#شامل "unicode/ustring.h"
#شامل "unicode/utf16.h"
#شامل "ustr_imp.h"

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
    بازگشت *این;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

منطقی
UCharCharacterIterator::operator==(ثابت ForwardCharacterIterator& that) ثابت {
    اگر (این == &that) {
        بازگشت درست;
    }
    اگر (typeid(*این) != typeid(that)) {
        بازگشت نادرست;
    }

    ثابت UCharCharacterIterator&    realThat = static_cast<ثابت UCharCharacterIterator&>(that);

    بازگشت text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() ثابت {
    بازگشت ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ثابت {
    بازگشت جدید UCharCharacterIterator(*این);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    اگر(pos < end) {
        بازگشت text[pos];
    } وگرنه {
        بازگشت DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    اگر(pos < end) {
        بازگشت text[pos++];
    } وگرنه {
        بازگشت DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    اگر(pos > begin) {
        بازگشت text[--pos];
    } وگرنه {
        بازگشت DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    اگر(position < begin) {
        pos = begin;
    } وگرنه اگر(position > end) {
        pos = end;
    } وگرنه {
        pos = position;
    }
    اگر(pos < end) {
        بازگشت text[pos];
    } وگرنه {
        بازگشت DONE;
    }
}

char16_t
UCharCharacterIterator::current() ثابت {
    اگر (pos >= begin && pos < end) {
        بازگشت text[pos];
    } وگرنه {
        بازگشت DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    اگر (pos + 1 < end) {
        بازگشت text[++pos];
    } وگرنه {
        /* make current() return DONE */
        pos = end;
        بازگشت DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    اگر (pos < end) {
        بازگشت text[pos++];
    } وگرنه {
        بازگشت DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    بازگشت pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    اگر (pos > begin) {
        بازگشت text[--pos];
    } وگرنه {
        بازگشت DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    بازگشت pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    اگر(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        بازگشت c;
    } وگرنه {
        بازگشت DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    اگر(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        بازگشت c;
    } وگرنه {
        بازگشت DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    اگر(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        بازگشت c;
    } وگرنه {
        بازگشت DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    اگر(position < begin) {
        position = begin;
    } وگرنه اگر(position > end) {
        position = end;
    }
    اگر(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = این->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        بازگشت c;
    } وگرنه {
        این->pos = position;
        بازگشت DONE;
    }
}

UChar32
UCharCharacterIterator::current32() ثابت {
    اگر (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        بازگشت c;
    } وگرنه {
        بازگشت DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    اگر (pos < end) {
        U16_FWD_1(text, pos, end);
        اگر(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            بازگشت c;
        }
    }
    /* make current() return DONE */
    pos = end;
    بازگشت DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    اگر (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        بازگشت c;
    } وگرنه {
        بازگشت DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    اگر (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        بازگشت c;
    } وگرنه {
        بازگشت DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    انتخاب(origin) {
    حالت kStart:
        pos = begin + delta;
        بشکن;
    حالت kCurrent:
        pos += delta;
        بشکن;
    حالت kEnd:
        pos = end + delta;
        بشکن;
    پیشفرض:
        بشکن;
    }

    اگر(pos < begin) {
        pos = begin;
    } وگرنه اگر(pos > end) {
        pos = end;
    }

    بازگشت pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    انتخاب(origin) {
    حالت kStart:
        pos = begin;
        اگر(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        بشکن;
    حالت kCurrent:
        اگر(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } وگرنه {
            U16_BACK_N(text, begin, pos, -delta);
        }
        بشکن;
    حالت kEnd:
        pos = end;
        اگر(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        بشکن;
    پیشفرض:
        بشکن;
    }

    بازگشت pos;
}

تهی UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    اگر (newText == nullptr || newTextLength < 0) {
        newTextLength = 0;
    }
    end = textLength = newTextLength;
    pos = begin = 0;
}

تهی
UCharCharacterIterator::getText(UnicodeString& result) {
    result = UnicodeString(text, textLength);
}

U_NAMESPACE_END
