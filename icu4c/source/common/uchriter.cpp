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

UCharCharacterIterator::UCharCharacterIterator(ধ্রুবক UCharCharacterIterator& that)
: CharacterIterator(that),
  text(that.text)
{
}

UCharCharacterIterator&
UCharCharacterIterator::operator=(ধ্রুবক UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::operator=(that);
    text = that.text;
    ফেরত *এই;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

যৌক্তিক
UCharCharacterIterator::operator==(ধ্রুবক ForwardCharacterIterator& that) ধ্রুবক {
    যদি (এই == &that) {
        ফেরত সত্য;
    }
    যদি (typeid(*এই) != typeid(that)) {
        ফেরত মিথ্যা;
    }

    ধ্রুবক UCharCharacterIterator&    realThat = static_cast<ধ্রুবক UCharCharacterIterator&>(that);

    ফেরত text == realThat.text
        && text_দৈর্ঘ্য == realThat.text_দৈর্ঘ্য
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() ধ্রুবক {
    ফেরত ustr_hashUCharsN(text, text_দৈর্ঘ্য) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ধ্রুবক {
    ফেরত নতুন UCharCharacterIterator(*এই);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    যদি(pos < end) {
        ফেরত text[pos];
    } অন্যথা {
        ফেরত DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    যদি(pos < end) {
        ফেরত text[pos++];
    } অন্যথা {
        ফেরত DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    যদি(pos > begin) {
        ফেরত text[--pos];
    } অন্যথা {
        ফেরত DONE;
    }
}

char16_t
UCharCharacterIterator::set_সূচক(int32_t position) {
    যদি(position < begin) {
        pos = begin;
    } অন্যথা যদি(position > end) {
        pos = end;
    } অন্যথা {
        pos = position;
    }
    যদি(pos < end) {
        ফেরত text[pos];
    } অন্যথা {
        ফেরত DONE;
    }
}

char16_t
UCharCharacterIterator::current() ধ্রুবক {
    যদি (pos >= begin && pos < end) {
        ফেরত text[pos];
    } অন্যথা {
        ফেরত DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    যদি (pos + 1 < end) {
        ফেরত text[++pos];
    } অন্যথা {
        /* make current() return DONE */
        pos = end;
        ফেরত DONE;
    }
}

char16_t
UCharCharacterIterator::পরবর্তী_Post_Inc() {
    যদি (pos < end) {
        ফেরত text[pos++];
    } অন্যথা {
        ফেরত DONE;
    }
}

UBool
UCharCharacterIterator::has_পরবর্তী() {
    ফেরত pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    যদি (pos > begin) {
        ফেরত text[--pos];
    } অন্যথা {
        ফেরত DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    ফেরত pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    যদি(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_পরবর্তী(text, i, end, c);
        ফেরত c;
    } অন্যথা {
        ফেরত DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    যদি(pos < end) {
        UChar32 c;
        U16_পরবর্তী(text, pos, end, c);
        ফেরত c;
    } অন্যথা {
        ফেরত DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    যদি(pos > begin) {
        UChar32 c;
        U16_পূর্ব(text, begin, pos, c);
        ফেরত c;
    } অন্যথা {
        ফেরত DONE;
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
        int32_t i = এই->pos = position;
        UChar32 c;
        U16_পরবর্তী(text, i, end, c);
        ফেরত c;
    } অন্যথা {
        এই->pos = position;
        ফেরত DONE;
    }
}

UChar32
UCharCharacterIterator::current32() ধ্রুবক {
    যদি (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        ফেরত c;
    } অন্যথা {
        ফেরত DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    যদি (pos < end) {
        U16_FWD_1(text, pos, end);
        যদি(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_পরবর্তী(text, i, end, c);
            ফেরত c;
        }
    }
    /* make current() return DONE */
    pos = end;
    ফেরত DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    যদি (pos < end) {
        UChar32 c;
        U16_পরবর্তী(text, pos, end, c);
        ফেরত c;
    } অন্যথা {
        ফেরত DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    যদি (pos > begin) {
        UChar32 c;
        U16_পূর্ব(text, begin, pos, c);
        ফেরত c;
    } অন্যথা {
        ফেরত DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    নির্বাচন(origin) {
    ক্ষেত্র kStart:
        pos = begin + delta;
        বিরতি;
    ক্ষেত্র kCurrent:
        pos += delta;
        বিরতি;
    ক্ষেত্র kEnd:
        pos = end + delta;
        বিরতি;
    পূর্বনির্ধারিত:
        বিরতি;
    }

    যদি(pos < begin) {
        pos = begin;
    } অন্যথা যদি(pos > end) {
        pos = end;
    }

    ফেরত pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    নির্বাচন(origin) {
    ক্ষেত্র kStart:
        pos = begin;
        যদি(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        বিরতি;
    ক্ষেত্র kCurrent:
        যদি(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } অন্যথা {
            U16_BACK_N(text, begin, pos, -delta);
        }
        বিরতি;
    ক্ষেত্র kEnd:
        pos = end;
        যদি(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        বিরতি;
    পূর্বনির্ধারিত:
        বিরতি;
    }

    ফেরত pos;
}

শূন্য UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      new_Text_দৈর্ঘ্য) {
    text = newText;
    যদি (newText == nullptr || new_Text_দৈর্ঘ্য < 0) {
        new_Text_দৈর্ঘ্য = 0;
    }
    end = text_দৈর্ঘ্য = new_Text_দৈর্ঘ্য;
    pos = begin = 0;
}

শূন্য
UCharCharacterIterator::getText(UnicodeString& ফলাফল) {
    ফলাফল = UnicodeString(text, text_দৈর্ঘ্য);
}

U_NAMESPACE_END
