// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#অন্তর্ভাৱ "utypeinfo.h"  // for 'typeid' to work

#অন্তর্ভাৱ "unicode/uchriter.h"
#অন্তর্ভাৱ "unicode/ustring.h"
#অন্তর্ভাৱ "unicode/utf16.h"
#অন্তর্ভাৱ "ustr_imp.h"

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

UCharCharacterIterator::UCharCharacterIterator(ধ্রুৱ UCharCharacterIterator& that)
: CharacterIterator(that),
  text(that.text)
{
}

UCharCharacterIterator&
UCharCharacterIterator::operator=(ধ্রুৱ UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::operator=(that);
    text = that.text;
    ঘূৰাই *এতত্;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

তার্কিক
UCharCharacterIterator::operator==(ধ্রুৱ ForwardCharacterIterator& that) ধ্রুৱ {
    যদি (এতত্ == &that) {
        ঘূৰাই সঁচা;
    }
    যদি (typeid(*এতত্) != typeid(that)) {
        ঘূৰাই মিছা;
    }

    ধ্রুৱ UCharCharacterIterator&    realThat = static_cast<ধ্রুৱ UCharCharacterIterator&>(that);

    ঘূৰাই text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() ধ্রুৱ {
    ঘূৰাই ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ধ্রুৱ {
    ঘূৰাই নূতন UCharCharacterIterator(*এতত্);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    যদি(pos < end) {
        ঘূৰাই text[pos];
    } অন্যথা {
        ঘূৰাই DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    যদি(pos < end) {
        ঘূৰাই text[pos++];
    } অন্যথা {
        ঘূৰাই DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    যদি(pos > begin) {
        ঘূৰাই text[--pos];
    } অন্যথা {
        ঘূৰাই DONE;
    }
}

char16_t
UCharCharacterIterator::setIndex(int32_t position) {
    যদি(position < begin) {
        pos = begin;
    } অন্যথা যদি(position > end) {
        pos = end;
    } অন্যথা {
        pos = position;
    }
    যদি(pos < end) {
        ঘূৰাই text[pos];
    } অন্যথা {
        ঘূৰাই DONE;
    }
}

char16_t
UCharCharacterIterator::current() ধ্রুৱ {
    যদি (pos >= begin && pos < end) {
        ঘূৰাই text[pos];
    } অন্যথা {
        ঘূৰাই DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    যদি (pos + 1 < end) {
        ঘূৰাই text[++pos];
    } অন্যথা {
        /* make current() return DONE */
        pos = end;
        ঘূৰাই DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    যদি (pos < end) {
        ঘূৰাই text[pos++];
    } অন্যথা {
        ঘূৰাই DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    ঘূৰাই pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    যদি (pos > begin) {
        ঘূৰাই text[--pos];
    } অন্যথা {
        ঘূৰাই DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ঘূৰাই pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    যদি(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ঘূৰাই c;
    } অন্যথা {
        ঘূৰাই DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    যদি(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ঘূৰাই c;
    } অন্যথা {
        ঘূৰাই DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    যদি(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ঘূৰাই c;
    } অন্যথা {
        ঘূৰাই DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    যদি(position < begin) {
        position = begin;
    } অন্যথা যদি(position > end) {
        position = end;
    }
    যদি(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = এতত্->pos = position;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        ঘূৰাই c;
    } অন্যথা {
        এতত্->pos = position;
        ঘূৰাই DONE;
    }
}

UChar32
UCharCharacterIterator::current32() ধ্রুৱ {
    যদি (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ঘূৰাই c;
    } অন্যথা {
        ঘূৰাই DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    যদি (pos < end) {
        U16_FWD_1(text, pos, end);
        যদি(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_NEXT(text, i, end, c);
            ঘূৰাই c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ঘূৰাই DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    যদি (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        ঘূৰাই c;
    } অন্যথা {
        ঘূৰাই DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    যদি (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        ঘূৰাই c;
    } অন্যথা {
        ঘূৰাই DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    ৱিকল্প(origin) {
    পক্ষ kStart:
        pos = begin + delta;
        ভঙ্গ;
    পক্ষ kCurrent:
        pos += delta;
        ভঙ্গ;
    পক্ষ kEnd:
        pos = end + delta;
        ভঙ্গ;
    স্ৱভাৱ:
        ভঙ্গ;
    }

    যদি(pos < begin) {
        pos = begin;
    } অন্যথা যদি(pos > end) {
        pos = end;
    }

    ঘূৰাই pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    ৱিকল্প(origin) {
    পক্ষ kStart:
        pos = begin;
        যদি(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        ভঙ্গ;
    পক্ষ kCurrent:
        যদি(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } অন্যথা {
            U16_BACK_N(text, begin, pos, -delta);
        }
        ভঙ্গ;
    পক্ষ kEnd:
        pos = end;
        যদি(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        ভঙ্গ;
    স্ৱভাৱ:
        ভঙ্গ;
    }

    ঘূৰাই pos;
}

শূন্য UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      newTextLength) {
    text = newText;
    যদি (newText == nullptr || newTextLength < 0) {
        newTextLength = 0;
    }
    end = textLength = newTextLength;
    pos = begin = 0;
}

শূন্য
UCharCharacterIterator::getText(UnicodeString& result) {
    result = UnicodeString(text, textLength);
}

U_NAMESPACE_END
