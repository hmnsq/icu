// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#समावेश "utypeinfo.h"  // for 'typeid' to work

#समावेश "unicode/uchriter.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/utf16.h"
#समावेश "ustr_imp.h"

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

UCharCharacterIterator::UCharCharacterIterator(स्थिर UCharCharacterIterator& that)
: CharacterIterator(that),
  text(that.text)
{
}

UCharCharacterIterator&
UCharCharacterIterator::चालक=(स्थिर UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::चालक=(that);
    text = that.text;
    वापस *यह;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

bool
UCharCharacterIterator::चालक==(स्थिर ForwardCharacterIterator& that) स्थिर {
    अगर (यह == &that) {
        वापस true;
    }
    अगर (typeid(*यह) != typeid(that)) {
        वापस false;
    }

    स्थिर UCharCharacterIterator&    realThat = static_cast<स्थिर UCharCharacterIterator&>(that);

    वापस text == realThat.text
        && text_लंबाई == realThat.text_लंबाई
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

int32_t
UCharCharacterIterator::hashCode() स्थिर {
    वापस ustr_hashUCharsN(text, text_लंबाई) ^ pos ^ begin ^ end;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() स्थिर {
    वापस नया UCharCharacterIterator(*यह);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    अगर(pos < end) {
        वापस text[pos];
    } अन्यथा {
        वापस DONE;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    अगर(pos < end) {
        वापस text[pos++];
    } अन्यथा {
        वापस DONE;
    }
}

char16_t
UCharCharacterIterator::last() {
    pos = end;
    अगर(pos > begin) {
        वापस text[--pos];
    } अन्यथा {
        वापस DONE;
    }
}

char16_t
UCharCharacterIterator::set_सूचकांक(int32_t position) {
    अगर(position < begin) {
        pos = begin;
    } अन्यथा अगर(position > end) {
        pos = end;
    } अन्यथा {
        pos = position;
    }
    अगर(pos < end) {
        वापस text[pos];
    } अन्यथा {
        वापस DONE;
    }
}

char16_t
UCharCharacterIterator::current() स्थिर {
    अगर (pos >= begin && pos < end) {
        वापस text[pos];
    } अन्यथा {
        वापस DONE;
    }
}

char16_t
UCharCharacterIterator::next() {
    अगर (pos + 1 < end) {
        वापस text[++pos];
    } अन्यथा {
        /* make current() return DONE */
        pos = end;
        वापस DONE;
    }
}

char16_t
UCharCharacterIterator::अग्र_Post_Inc() {
    अगर (pos < end) {
        वापस text[pos++];
    } अन्यथा {
        वापस DONE;
    }
}

UBool
UCharCharacterIterator::has_अग्र() {
    वापस pos < end;
}

char16_t
UCharCharacterIterator::previous() {
    अगर (pos > begin) {
        वापस text[--pos];
    } अन्यथा {
        वापस DONE;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    वापस pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    अगर(pos < end) {
        int32_t i = pos;
        UChar32 c;
        U16_अग्र(text, i, end, c);
        वापस c;
    } अन्यथा {
        वापस DONE;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    अगर(pos < end) {
        UChar32 c;
        U16_अग्र(text, pos, end, c);
        वापस c;
    } अन्यथा {
        वापस DONE;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = end;
    अगर(pos > begin) {
        UChar32 c;
        U16_पूर्व(text, begin, pos, c);
        वापस c;
    } अन्यथा {
        वापस DONE;
    }
}

UChar32
UCharCharacterIterator::setIndex32(int32_t position) {
    अगर(position < begin) {
        position = begin;
    } अन्यथा अगर(position > end) {
        position = end;
    }
    अगर(position < end) {
        U16_SET_CP_START(text, begin, position);
        int32_t i = यह->pos = position;
        UChar32 c;
        U16_अग्र(text, i, end, c);
        वापस c;
    } अन्यथा {
        यह->pos = position;
        वापस DONE;
    }
}

UChar32
UCharCharacterIterator::current32() स्थिर {
    अगर (pos >= begin && pos < end) {
        UChar32 c;
        U16_GET(text, begin, pos, end, c);
        वापस c;
    } अन्यथा {
        वापस DONE;
    }
}

UChar32
UCharCharacterIterator::next32() {
    अगर (pos < end) {
        U16_FWD_1(text, pos, end);
        अगर(pos < end) {
            int32_t i = pos;
            UChar32 c;
            U16_अग्र(text, i, end, c);
            वापस c;
        }
    }
    /* make current() return DONE */
    pos = end;
    वापस DONE;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    अगर (pos < end) {
        UChar32 c;
        U16_अग्र(text, pos, end, c);
        वापस c;
    } अन्यथा {
        वापस DONE;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    अगर (pos > begin) {
        UChar32 c;
        U16_पूर्व(text, begin, pos, c);
        वापस c;
    } अन्यथा {
        वापस DONE;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    चयन(origin) {
    हाल kStart:
        pos = begin + delta;
        अवरोध;
    हाल kCurrent:
        pos += delta;
        अवरोध;
    हाल kEnd:
        pos = end + delta;
        अवरोध;
    शेष:
        अवरोध;
    }

    अगर(pos < begin) {
        pos = begin;
    } अन्यथा अगर(pos > end) {
        pos = end;
    }

    वापस pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    चयन(origin) {
    हाल kStart:
        pos = begin;
        अगर(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        }
        अवरोध;
    हाल kCurrent:
        अगर(delta > 0) {
            U16_FWD_N(text, pos, end, delta);
        } अन्यथा {
            U16_BACK_N(text, begin, pos, -delta);
        }
        अवरोध;
    हाल kEnd:
        pos = end;
        अगर(delta < 0) {
            U16_BACK_N(text, begin, pos, -delta);
        }
        अवरोध;
    शेष:
        अवरोध;
    }

    वापस pos;
}

व्योम UCharCharacterIterator::setText(ConstChar16Ptr newText,
                                     int32_t      new_Text_लंबाई) {
    text = newText;
    अगर (newText == nullptr || new_Text_लंबाई < 0) {
        new_Text_लंबाई = 0;
    }
    end = text_लंबाई = new_Text_लंबाई;
    pos = begin = 0;
}

व्योम
UCharCharacterIterator::getText(UnicodeString& परिणाम) {
    परिणाम = UnicodeString(text, text_लंबाई);
}

U_NAMESPACE_END
