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

UCharCharacterIterator::UCharCharacterIterator(स्थिरांक UCharCharacterIterator& that)
: CharacterIterator(that),
  text(that.text)
{
}

UCharCharacterIterator&
UCharCharacterIterator::चालक=(स्थिरांक UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::चालक=(that);
    text = that.text;
    परत *हे;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

तार्किक
UCharCharacterIterator::चालक==(स्थिरांक ForwardCharacterIterator& that) स्थिरांक {
    जर (हे == &that) {
        परत सत्य;
    }
    जर (typeid(*हे) != typeid(that)) {
        परत असत्य;
    }

    स्थिरांक UCharCharacterIterator&    realThat = static_cast<स्थिरांक UCharCharacterIterator&>(that);

    परत text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() स्थिरांक {
    परत ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() स्थिरांक {
    परत नवीन UCharCharacterIterator(*हे);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    जर(pos < end) {
        परत text[pos];
    } अन्यथा {
        परत DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    जर(pos < end) {
        परत text[pos++];
    } अन्यथा {
        परत DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    जर(pos > begin) {
        परत text[--pos];
    } अन्यथा {
        परत DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    जर(position < begin) {
        pos = begin;
    } अन्यथा जर(position > end) {
        pos = end;
    } अन्यथा {
        pos = position;
    }
    जर(pos < end) {
        परत text[pos];
    } अन्यथा {
        परत DONE;
    }
}

char16_t
UCharCharacterIterator::current() स्थिरांक {
    जर (pos >= begin && pos < end) {
        परत text[pos];
    } अन्यथा {
        परत DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    जर (pos + 1 < end) {
        परत text[++pos];
    } अन्यथा {
        /* make current() return DONE */
        pos = end;
        परत DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    जर (pos < end) {
        परत text[pos++];
    } अन्यथा {
        परत DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    परत pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    जर (pos > begin) {
        परत text[--pos];
    } अन्यथा {
        परत DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    परत pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    जर(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        परत c;
    } अन्यथा {
        परत DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    जर(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        परत c;
    } अन्यथा {
        परत DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    जर(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        परत c;
    } अन्यथा {
        परत DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    जर(position < begin) {
        position = begin;
    } अन्यथा जर(position > end) {
        position = end;
    }
    जर(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = हे->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        परत c;
    } अन्यथा {
        हे->pos = position;
        परत DONE;
    }
}

UChar32
UCharCharacterIterator::current32() स्थिरांक {
    जर (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        परत c;
    } अन्यथा {
        परत DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    जर (pos < end) {
        U16_FWD_1(text, pos, end);
        जर(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            परत c;
        }
    }
    /* make current() return DONE */
    pos = end;
    परत DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    जर (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        परत c;
    } अन्यथा {
        परत DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    जर (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        परत c;
    } अन्यथा {
        परत DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    निवड(origin) {
    स्थिती kStart:
        pos = begin + delta;
        थांबा;
    स्थिती kCurrent:
        pos += delta;
        थांबा;
    स्थिती kEnd:
        pos = end + delta;
        थांबा;
    पूर्वनिर्धारित:
        थांबा;
    }

    जर(pos < begin) {
        pos = begin;
    } अन्यथा जर(pos > end) {
        pos = end;
    }

    परत pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    निवड(origin) {
    स्थिती kStart:
        pos = begin;
        जर(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        थांबा;
    स्थिती kCurrent:
        जर(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } अन्यथा {
            U16_BACK_N(text, begin, pos, -delta);
        }
        थांबा;
    स्थिती kEnd:
        pos = end;
        जर(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        थांबा;
    पूर्वनिर्धारित:
        थांबा;
    }

    परत pos;
}

शून्य UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    जर (newText == nullptr || newTextLength < 0) {
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
