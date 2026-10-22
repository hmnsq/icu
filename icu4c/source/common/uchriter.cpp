// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#অন্তর্ভাব "utypeinfo.h"  // for 'typeid' to work

#অন্তর্ভাব "unicode/uchriter.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "ustr_imp.h"

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

UCharCharacterIterator::UCharCharacterIterator(ধ্রুব UCharCharacterIterator& that)
: CharacterIterator(that),
  text(that.text)
{
}

UCharCharacterIterator&
UCharCharacterIterator::operator=(ধ্রুব UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::operator=(that);
    text = that.text;
    প্রত্যাগম *এতত্;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

তার্কিক
UCharCharacterIterator::operator==(ধ্রুব ForwardCharacterIterator& that) ধ্রুব {
    যদি (এতত্ == &that) {
        প্রত্যাগম সত্য;
    }
    যদি (typeid(*এতত্) != typeid(that)) {
        প্রত্যাগম অসত্য;
    }

    ধ্রুব UCharCharacterIterator&    realThat = static_cast<ধ্রুব UCharCharacterIterator&>(that);

    প্রত্যাগম text == realThat.text
        && textLength == realThat.textLength
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() ধ্রুব {
    প্রত্যাগম ustr_hashUCharsN(text, textLength) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ধ্রুব {
    প্রত্যাগম নূতন UCharCharacterIterator(*এতত্);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    যদি(pos < end) {
        প্রত্যাগম text[pos];
    } অন্যথা {
        প্রত্যাগম DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    যদি(pos < end) {
        প্রত্যাগম text[pos++];
    } অন্যথা {
        প্রত্যাগম DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    যদি(pos > begin) {
        প্রত্যাগম text[--pos];
    } অন্যথা {
        প্রত্যাগম DONE;
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
        প্রত্যাগম text[pos];
    } অন্যথা {
        প্রত্যাগম DONE;
    }
}

char16_t
UCharCharacterIterator::current() ধ্রুব {
    যদি (pos >= begin && pos < end) {
        প্রত্যাগম text[pos];
    } অন্যথা {
        প্রত্যাগম DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    যদি (pos + 1 < end) {
        প্রত্যাগম text[++pos];
    } অন্যথা {
        /* make current() return DONE */
        pos = end;
        প্রত্যাগম DONE;
    }
}

char16_t
UCharCharacterIterator::nextPostInc() {
    যদি (pos < end) {
        প্রত্যাগম text[pos++];
    } অন্যথা {
        প্রত্যাগম DONE;
    }
}

UBool
UCharCharacterIterator::hasNext() {
    প্রত্যাগম pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    যদি (pos > begin) {
        প্রত্যাগম text[--pos];
    } অন্যথা {
        প্রত্যাগম DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    প্রত্যাগম pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    যদি(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_NEXT(text, i, end, c);
        প্রত্যাগম c;
    } অন্যথা {
        প্রত্যাগম DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    যদি(pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        প্রত্যাগম c;
    } অন্যথা {
        প্রত্যাগম DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    যদি(pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        প্রত্যাগম c;
    } অন্যথা {
        প্রত্যাগম DONE;
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
        প্রত্যাগম c;
    } অন্যথা {
        এতত্->pos = position;
        প্রত্যাগম DONE;
    }
}

UChar32
UCharCharacterIterator::current32() ধ্রুব {
    যদি (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        প্রত্যাগম c;
    } অন্যথা {
        প্রত্যাগম DONE;
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
            প্রত্যাগম c;
        }
    }
    /* make current() return DONE */
    pos = end;
    প্রত্যাগম DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    যদি (pos < end) {
        UChar32 c;
        U16_NEXT(text, pos, end, c);
        প্রত্যাগম c;
    } অন্যথা {
        প্রত্যাগম DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    যদি (pos > begin) {
        UChar32 c;
        U16_PREV(text, begin, pos, c);
        প্রত্যাগম c;
    } অন্যথা {
        প্রত্যাগম DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    বিকল্প(origin) {
    পক্ষ kStart:
        pos = begin + delta;
        ভঙ্গ;
    পক্ষ kCurrent:
        pos += delta;
        ভঙ্গ;
    পক্ষ kEnd:
        pos = end + delta;
        ভঙ্গ;
    স্বভাব:
        ভঙ্গ;
    }

    যদি(pos < begin) {
        pos = begin;
    } অন্যথা যদি(pos > end) {
        pos = end;
    }

    প্রত্যাগম pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    বিকল্প(origin) {
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
    স্বভাব:
        ভঙ্গ;
    }

    প্রত্যাগম pos;
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
