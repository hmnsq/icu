// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#ଅନ୍ତର୍ଭାଵ "utypeinfo.h"  // for 'typeid' to work

#ଅନ୍ତର୍ଭାଵ "unicode/uchriter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"

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

UCharCharacterIterator::UCharCharacterIterator(ଅଚଳ UCharCharacterIterator& that)
: CharacterIterator(that),
  text(that.text)
{
}

UCharCharacterIterator&
UCharCharacterIterator::operator=(ଅଚଳ UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::operator=(that);
    text = that.text;
    ଫେରନ୍ତୁ *ଏହି;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

ତାର୍କିକ
UCharCharacterIterator::operator==(ଅଚଳ ForwardCharacterIterator& that) ଅଚଳ {
    ଯଦି (ଏହି == &that) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି (typeid(*ଏହି) != typeid(that)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    ଅଚଳ UCharCharacterIterator&    realThat = static_cast<ଅଚଳ UCharCharacterIterator&>(that);

    ଫେରନ୍ତୁ text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() ଅଚଳ {
    ଫେରନ୍ତୁ ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ଅଚଳ {
    ଫେରନ୍ତୁ ନୂତନ UCharCharacterIterator(*ଏହି);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    ଯଦି(pos < end) {
        ଫେରନ୍ତୁ text[pos];
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    ଯଦି(pos < end) {
        ଫେରନ୍ତୁ text[pos++];
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    ଯଦି(pos > begin) {
        ଫେରନ୍ତୁ text[--pos];
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    ଯଦି(position < begin) {
        pos = begin;
    } ଅନ୍ୟଥା ଯଦି(position > end) {
        pos = end;
    } ଅନ୍ୟଥା {
        pos = position;
    }
    ଯଦି(pos < end) {
        ଫେରନ୍ତୁ text[pos];
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ DONE;
    }
}

char16_t
UCharCharacterIterator::current() ଅଚଳ {
    ଯଦି (pos >= begin && pos < end) {
        ଫେରନ୍ତୁ text[pos];
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    ଯଦି (pos + 1 < end) {
        ଫେରନ୍ତୁ text[++pos];
    } ଅନ୍ୟଥା {
        /* make current() return DONE */
        pos = end;
        ଫେରନ୍ତୁ DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    ଯଦି (pos < end) {
        ଫେରନ୍ତୁ text[pos++];
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ଫେରନ୍ତୁ pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    ଯଦି (pos > begin) {
        ଫେରନ୍ତୁ text[--pos];
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ଫେରନ୍ତୁ pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    ଯଦି(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ଫେରନ୍ତୁ c;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    ଯଦି(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ଫେରନ୍ତୁ c;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    ଯଦି(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ଫେରନ୍ତୁ c;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    ଯଦି(position < begin) {
        position = begin;
    } ଅନ୍ୟଥା ଯଦି(position > end) {
        position = end;
    }
    ଯଦି(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = ଏହି->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ଫେରନ୍ତୁ c;
    } ଅନ୍ୟଥା {
        ଏହି->pos = position;
        ଫେରନ୍ତୁ DONE;
    }
}

UChar32
UCharCharacterIterator::current32() ଅଚଳ {
    ଯଦି (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ଫେରନ୍ତୁ c;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    ଯଦି (pos < end) {
        U16_FWD_1(text, pos, end);
        ଯଦି(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ଫେରନ୍ତୁ c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ଫେରନ୍ତୁ DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    ଯଦି (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ଫେରନ୍ତୁ c;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    ଯଦି (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ଫେରନ୍ତୁ c;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    ବାଛ(origin) {
    ସ୍ଥିତି kStart:
        pos = begin + delta;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି kCurrent:
        pos += delta;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି kEnd:
        pos = end + delta;
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
    }

    ଯଦି(pos < begin) {
        pos = begin;
    } ଅନ୍ୟଥା ଯଦି(pos > end) {
        pos = end;
    }

    ଫେରନ୍ତୁ pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    ବାଛ(origin) {
    ସ୍ଥିତି kStart:
        pos = begin;
        ଯଦି(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି kCurrent:
        ଯଦି(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } ଅନ୍ୟଥା {
            U16_BACK_N(text, begin, pos, -delta);
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି kEnd:
        pos = end;
        ଯଦି(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ pos;
}

ଶୂନ୍ୟ UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    ଯଦି (newText == nullptr || newTextLength < 0) {
        newTextLength = 0;
    }
    end = textLength = newTextLength;
    pos = begin = 0;
}

ଶୂନ୍ୟ
UCharCharacterIterator::getText(UnicodeString& result) {
    result = UnicodeString(text, textLength);
}

U_NAMESPACE_END
