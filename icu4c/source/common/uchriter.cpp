// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#அந்தர்பாவ "utypeinfo.h"  // for 'typeid' to work

#அந்தர்பாவ "unicode/uchriter.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "ustr_imp.h"

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

UCharCharacterIterator::UCharCharacterIterator(மாறிலி UCharCharacterIterator& that)
: CharacterIterator(that),
  text(that.text)
{
}

UCharCharacterIterator&
UCharCharacterIterator::operator=(மாறிலி UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::operator=(that);
    text = that.text;
    திரும்பு *இது;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

தருக்க
UCharCharacterIterator::operator==(மாறிலி ForwardCharacterIterator& that) மாறிலி {
    எனில் (இது == &that) {
        திரும்பு உண்மை;
    }
    எனில் (typeid(*இது) != typeid(that)) {
        திரும்பு பொய்;
    }

    மாறிலி UCharCharacterIterator&    realThat = static_cast<மாறிலி UCharCharacterIterator&>(that);

    திரும்பு text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() மாறிலி {
    திரும்பு ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() மாறிலி {
    திரும்பு புதிய UCharCharacterIterator(*இது);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    எனில்(pos < end) {
        திரும்பு text[pos];
    } இல்லையெனில் {
        திரும்பு DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    எனில்(pos < end) {
        திரும்பு text[pos++];
    } இல்லையெனில் {
        திரும்பு DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    எனில்(pos > begin) {
        திரும்பு text[--pos];
    } இல்லையெனில் {
        திரும்பு DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    எனில்(position < begin) {
        pos = begin;
    } இல்லையெனில் எனில்(position > end) {
        pos = end;
    } இல்லையெனில் {
        pos = position;
    }
    எனில்(pos < end) {
        திரும்பு text[pos];
    } இல்லையெனில் {
        திரும்பு DONE;
    }
}

char16_t
UCharCharacterIterator::current() மாறிலி {
    எனில் (pos >= begin && pos < end) {
        திரும்பு text[pos];
    } இல்லையெனில் {
        திரும்பு DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    எனில் (pos + 1 < end) {
        திரும்பு text[++pos];
    } இல்லையெனில் {
        /* make current() return DONE */
        pos = end;
        திரும்பு DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    எனில் (pos < end) {
        திரும்பு text[pos++];
    } இல்லையெனில் {
        திரும்பு DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    திரும்பு pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    எனில் (pos > begin) {
        திரும்பு text[--pos];
    } இல்லையெனில் {
        திரும்பு DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    திரும்பு pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    எனில்(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        திரும்பு c;
    } இல்லையெனில் {
        திரும்பு DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    எனில்(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        திரும்பு c;
    } இல்லையெனில் {
        திரும்பு DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    எனில்(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        திரும்பு c;
    } இல்லையெனில் {
        திரும்பு DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    எனில்(position < begin) {
        position = begin;
    } இல்லையெனில் எனில்(position > end) {
        position = end;
    }
    எனில்(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = இது->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        திரும்பு c;
    } இல்லையெனில் {
        இது->pos = position;
        திரும்பு DONE;
    }
}

UChar32
UCharCharacterIterator::current32() மாறிலி {
    எனில் (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        திரும்பு c;
    } இல்லையெனில் {
        திரும்பு DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    எனில் (pos < end) {
        U16_FWD_1(text, pos, end);
        எனில்(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            திரும்பு c;
        }
    }
    /* make current() return DONE */
    pos = end;
    திரும்பு DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    எனில் (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        திரும்பு c;
    } இல்லையெனில் {
        திரும்பு DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    எனில் (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        திரும்பு c;
    } இல்லையெனில் {
        திரும்பு DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    தேர்வு(origin) {
    நிலை kStart:
        pos = begin + delta;
        நிறுத்து;
    நிலை kCurrent:
        pos += delta;
        நிறுத்து;
    நிலை kEnd:
        pos = end + delta;
        நிறுத்து;
    இயல்பு:
        நிறுத்து;
    }

    எனில்(pos < begin) {
        pos = begin;
    } இல்லையெனில் எனில்(pos > end) {
        pos = end;
    }

    திரும்பு pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    தேர்வு(origin) {
    நிலை kStart:
        pos = begin;
        எனில்(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        நிறுத்து;
    நிலை kCurrent:
        எனில்(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } இல்லையெனில் {
            U16_BACK_N(text, begin, pos, -delta);
        }
        நிறுத்து;
    நிலை kEnd:
        pos = end;
        எனில்(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        நிறுத்து;
    இயல்பு:
        நிறுத்து;
    }

    திரும்பு pos;
}

வெறுமை UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    எனில் (newText == nullptr || newTextLength < 0) {
        newTextLength = 0;
    }
    end = textLength = newTextLength;
    pos = begin = 0;
}

வெறுமை
UCharCharacterIterator::getText(UnicodeString& result) {
    result = UnicodeString(text, textLength);
}

U_NAMESPACE_END
