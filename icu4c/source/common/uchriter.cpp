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
    أرجع *هذا;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

منطقي
UCharCharacterIterator::operator==(ثابت ForwardCharacterIterator& that) ثابت {
    𐤀𐤌 (هذا == &that) {
        أرجع true;
    }
    𐤀𐤌 (typeid(*هذا) != typeid(that)) {
        أرجع خطأ;
    }

    ثابت UCharCharacterIterator&    realThat = static_cast<ثابت UCharCharacterIterator&>(that);

    أرجع text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() ثابت {
    أرجع ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ثابت {
    أرجع جديد UCharCharacterIterator(*هذا);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    𐤀𐤌(pos < end) {
        أرجع text[pos];
    } وإلا {
        أرجع DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    𐤀𐤌(pos < end) {
        أرجع text[pos++];
    } وإلا {
        أرجع DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    𐤀𐤌(pos > begin) {
        أرجع text[--pos];
    } وإلا {
        أرجع DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    𐤀𐤌(position < begin) {
        pos = begin;
    } وإلا 𐤀𐤌(position > end) {
        pos = end;
    } وإلا {
        pos = position;
    }
    𐤀𐤌(pos < end) {
        أرجع text[pos];
    } وإلا {
        أرجع DONE;
    }
}

char16_t
UCharCharacterIterator::current() ثابت {
    𐤀𐤌 (pos >= begin && pos < end) {
        أرجع text[pos];
    } وإلا {
        أرجع DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    𐤀𐤌 (pos + 1 < end) {
        أرجع text[++pos];
    } وإلا {
        /* make current() return DONE */
        pos = end;
        أرجع DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    𐤀𐤌 (pos < end) {
        أرجع text[pos++];
    } وإلا {
        أرجع DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    أرجع pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    𐤀𐤌 (pos > begin) {
        أرجع text[--pos];
    } وإلا {
        أرجع DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    أرجع pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    𐤀𐤌(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        أرجع c;
    } وإلا {
        أرجع DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    𐤀𐤌(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        أرجع c;
    } وإلا {
        أرجع DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    𐤀𐤌(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        أرجع c;
    } وإلا {
        أرجع DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    𐤀𐤌(position < begin) {
        position = begin;
    } وإلا 𐤀𐤌(position > end) {
        position = end;
    }
    𐤀𐤌(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = هذا->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        أرجع c;
    } وإلا {
        هذا->pos = position;
        أرجع DONE;
    }
}

UChar32
UCharCharacterIterator::current32() ثابت {
    𐤀𐤌 (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        أرجع c;
    } وإلا {
        أرجع DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    𐤀𐤌 (pos < end) {
        U16_FWD_1(text, pos, end);
        𐤀𐤌(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            أرجع c;
        }
    }
    /* make current() return DONE */
    pos = end;
    أرجع DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    𐤀𐤌 (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        أرجع c;
    } وإلا {
        أرجع DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    𐤀𐤌 (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        أرجع c;
    } وإلا {
        أرجع DONE;
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

    𐤀𐤌(pos < begin) {
        pos = begin;
    } وإلا 𐤀𐤌(pos > end) {
        pos = end;
    }

    أرجع pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    بدل(origin) {
    حالة kStart:
        pos = begin;
        𐤀𐤌(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        اكسر;
    حالة kCurrent:
        𐤀𐤌(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } وإلا {
            U16_BACK_N(text, begin, pos, -delta);
        }
        اكسر;
    حالة kEnd:
        pos = end;
        𐤀𐤌(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        اكسر;
    افتراضي:
        اكسر;
    }

    أرجع pos;
}

فراغ UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    𐤀𐤌 (newText == nullptr || newTextLength < 0) {
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
