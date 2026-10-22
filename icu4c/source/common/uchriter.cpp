// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#अन्तर्भाव "utypeinfo.h"  // for 'typeid' to work

#अन्तर्भाव "unicode/uchriter.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "ustr_imp.h"

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

UCharCharacterIterator::UCharCharacterIterator(ध्रुव UCharCharacterIterator& that)
: CharacterIterator(that),
  text(that.text)
{
}

UCharCharacterIterator&
UCharCharacterIterator::operator=(ध्रुव UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::operator=(that);
    text = that.text;
    पटिनिवत्त *एतत्;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

तार्किक
UCharCharacterIterator::operator==(ध्रुव ForwardCharacterIterator& that) ध्रुव {
    सचे (एतत् == &that) {
        पटिनिवत्त सच्च;
    }
    सचे (typeid(*एतत्) != typeid(that)) {
        पटिनिवत्त मिच्छा;
    }

    ध्रुव UCharCharacterIterator&    realThat = static_cast<ध्रुव UCharCharacterIterator&>(that);

    पटिनिवत्त text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() ध्रुव {
    पटिनिवत्त ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ध्रुव {
    पटिनिवत्त नूतन UCharCharacterIterator(*एतत्);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    सचे(pos < end) {
        पटिनिवत्त text[pos];
    } अञ्ञथा {
        पटिनिवत्त DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    सचे(pos < end) {
        पटिनिवत्त text[pos++];
    } अञ्ञथा {
        पटिनिवत्त DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    सचे(pos > begin) {
        पटिनिवत्त text[--pos];
    } अञ्ञथा {
        पटिनिवत्त DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    सचे(position < begin) {
        pos = begin;
    } अञ्ञथा सचे(position > end) {
        pos = end;
    } अञ्ञथा {
        pos = position;
    }
    सचे(pos < end) {
        पटिनिवत्त text[pos];
    } अञ्ञथा {
        पटिनिवत्त DONE;
    }
}

char16_t
UCharCharacterIterator::current() ध्रुव {
    सचे (pos >= begin && pos < end) {
        पटिनिवत्त text[pos];
    } अञ्ञथा {
        पटिनिवत्त DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    सचे (pos + 1 < end) {
        पटिनिवत्त text[++pos];
    } अञ्ञथा {
        /* make current() return DONE */
        pos = end;
        पटिनिवत्त DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    सचे (pos < end) {
        पटिनिवत्त text[pos++];
    } अञ्ञथा {
        पटिनिवत्त DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    पटिनिवत्त pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    सचे (pos > begin) {
        पटिनिवत्त text[--pos];
    } अञ्ञथा {
        पटिनिवत्त DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    पटिनिवत्त pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    सचे(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        पटिनिवत्त c;
    } अञ्ञथा {
        पटिनिवत्त DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    सचे(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        पटिनिवत्त c;
    } अञ्ञथा {
        पटिनिवत्त DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    सचे(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        पटिनिवत्त c;
    } अञ्ञथा {
        पटिनिवत्त DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    सचे(position < begin) {
        position = begin;
    } अञ्ञथा सचे(position > end) {
        position = end;
    }
    सचे(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = एतत्->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        पटिनिवत्त c;
    } अञ्ञथा {
        एतत्->pos = position;
        पटिनिवत्त DONE;
    }
}

UChar32
UCharCharacterIterator::current32() ध्रुव {
    सचे (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        पटिनिवत्त c;
    } अञ्ञथा {
        पटिनिवत्त DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    सचे (pos < end) {
        U16_FWD_1(text, pos, end);
        सचे(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            पटिनिवत्त c;
        }
    }
    /* make current() return DONE */
    pos = end;
    पटिनिवत्त DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    सचे (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        पटिनिवत्त c;
    } अञ्ञथा {
        पटिनिवत्त DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    सचे (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        पटिनिवत्त c;
    } अञ्ञथा {
        पटिनिवत्त DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    विकल्प(origin) {
    पक्ष kStart:
        pos = begin + delta;
        भङ्ग;
    पक्ष kCurrent:
        pos += delta;
        भङ्ग;
    पक्ष kEnd:
        pos = end + delta;
        भङ्ग;
    स्वभाव:
        भङ्ग;
    }

    सचे(pos < begin) {
        pos = begin;
    } अञ्ञथा सचे(pos > end) {
        pos = end;
    }

    पटिनिवत्त pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    विकल्प(origin) {
    पक्ष kStart:
        pos = begin;
        सचे(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        भङ्ग;
    पक्ष kCurrent:
        सचे(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } अञ्ञथा {
            U16_BACK_N(text, begin, pos, -delta);
        }
        भङ्ग;
    पक्ष kEnd:
        pos = end;
        सचे(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        भङ्ग;
    स्वभाव:
        भङ्ग;
    }

    पटिनिवत्त pos;
}

शून्य UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    सचे (newText == nullptr || newTextLength < 0) {
        newTextLength = 0;
    }
    end = textLength = newTextLength;
    pos = begin = 0;
}

शून्य
UCharCharacterIterator::getText(UnicodeString& result) {
    result = UnicodeString(text, textLength);
}

U_NAMESPACE_END
