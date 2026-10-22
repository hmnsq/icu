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
  text(शून्यसूचकः)
{
    // never default construct!
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                                               int32_t length)
  : CharacterIterator(textPtr != शून्यसूचकः ? (length >= 0 ? length : u_strlen(textPtr)) : 0),
  text(textPtr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t position)
  : CharacterIterator(textPtr != शून्यसूचकः ? (length >= 0 ? length : u_strlen(textPtr)) : 0, position),
  text(textPtr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t textBegin,
                                               int32_t text_अन्तः,
                                               int32_t position)
  : CharacterIterator(textPtr != शून्यसूचकः ? (length >= 0 ? length : u_strlen(textPtr)) : 0,
                      textBegin, text_अन्तः, position),
  text(textPtr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ध्रुव UCharCharacterIterator& that)
: CharacterIterator(that),
  text(that.text)
{
}

UCharCharacterIterator&
UCharCharacterIterator::संक्रियकः=(ध्रुव UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::संक्रियकः=(that);
    text = that.text;
    प्रत्यागम *एतत्;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

तार्किक
UCharCharacterIterator::संक्रियकः==(ध्रुव ForwardCharacterIterator& that) ध्रुव {
    यदि (एतत् == &that) {
        प्रत्यागम सत्य;
    }
    यदि (typeid(*एतत्) != typeid(that)) {
        प्रत्यागम असत्य;
    }

    ध्रुव UCharCharacterIterator&    realThat = static_cast<ध्रुव UCharCharacterIterator&>(that);

    प्रत्यागम text == realThat.text
        && text_दीर्घता == realThat.text_दीर्घता
        && pos == realThat.pos
        && begin == realThat.begin
        && अन्तः == realThat.अन्तः;
}

int32_t
UCharCharacterIterator::hash_संहिता() ध्रुव {
    प्रत्यागम ustr_hashUCharsN(text, text_दीर्घता) ^ pos ^ begin ^ अन्तः;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ध्रुव {
    प्रत्यागम नूतन UCharCharacterIterator(*एतत्);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    यदि(pos < अन्तः) {
        प्रत्यागम text[pos];
    } अन्यथा {
        प्रत्यागम समाप्तम्_2;
    }
}

char16_t
UCharCharacterIterator::firstPostInc() {
    pos = begin;
    यदि(pos < अन्तः) {
        प्रत्यागम text[pos++];
    } अन्यथा {
        प्रत्यागम समाप्तम्_2;
    }
}

char16_t
UCharCharacterIterator::चरमः() {
    pos = अन्तः;
    यदि(pos > begin) {
        प्रत्यागम text[--pos];
    } अन्यथा {
        प्रत्यागम समाप्तम्_2;
    }
}

char16_t
UCharCharacterIterator::समुच्चयः_सूचकांक(int32_t position) {
    यदि(position < begin) {
        pos = begin;
    } अन्यथा यदि(position > अन्तः) {
        pos = अन्तः;
    } अन्यथा {
        pos = position;
    }
    यदि(pos < अन्तः) {
        प्रत्यागम text[pos];
    } अन्यथा {
        प्रत्यागम समाप्तम्_2;
    }
}

char16_t
UCharCharacterIterator::current() ध्रुव {
    यदि (pos >= begin && pos < अन्तः) {
        प्रत्यागम text[pos];
    } अन्यथा {
        प्रत्यागम समाप्तम्_2;
    }
}

char16_t
UCharCharacterIterator::next() {
    यदि (pos + 1 < अन्तः) {
        प्रत्यागम text[++pos];
    } अन्यथा {
        /* make current() return DONE */
        pos = अन्तः;
        प्रत्यागम समाप्तम्_2;
    }
}

char16_t
UCharCharacterIterator::अग्र_Post_Inc() {
    यदि (pos < अन्तः) {
        प्रत्यागम text[pos++];
    } अन्यथा {
        प्रत्यागम समाप्तम्_2;
    }
}

UBool
UCharCharacterIterator::has_अग्र() {
    प्रत्यागम pos < अन्तः;
}

char16_t
UCharCharacterIterator::previous() {
    यदि (pos > begin) {
        प्रत्यागम text[--pos];
    } अन्यथा {
        प्रत्यागम समाप्तम्_2;
    }
}

UBool
UCharCharacterIterator::hasPrevious() {
    प्रत्यागम pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    यदि(pos < अन्तः) {
        int32_t i = pos;
        UChar32 c;
        U16_अग्र(text, i, अन्तः, c);
        प्रत्यागम c;
    } अन्यथा {
        प्रत्यागम समाप्तम्_2;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    यदि(pos < अन्तः) {
        UChar32 c;
        U16_अग्र(text, pos, अन्तः, c);
        प्रत्यागम c;
    } अन्यथा {
        प्रत्यागम समाप्तम्_2;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = अन्तः;
    यदि(pos > begin) {
        UChar32 c;
        U16_पूर्व(text, begin, pos, c);
        प्रत्यागम c;
    } अन्यथा {
        प्रत्यागम समाप्तम्_2;
    }
}

UChar32
UCharCharacterIterator::समुच्चयः_Index32(int32_t position) {
    यदि(position < begin) {
        position = begin;
    } अन्यथा यदि(position > अन्तः) {
        position = अन्तः;
    }
    यदि(position < अन्तः) {
        U16_समुच्चयः_CP_START(text, begin, position);
        int32_t i = एतत्->pos = position;
        UChar32 c;
        U16_अग्र(text, i, अन्तः, c);
        प्रत्यागम c;
    } अन्यथा {
        एतत्->pos = position;
        प्रत्यागम समाप्तम्_2;
    }
}

UChar32
UCharCharacterIterator::current32() ध्रुव {
    यदि (pos >= begin && pos < अन्तः) {
        UChar32 c;
        U16_GET(text, begin, pos, अन्तः, c);
        प्रत्यागम c;
    } अन्यथा {
        प्रत्यागम समाप्तम्_2;
    }
}

UChar32
UCharCharacterIterator::next32() {
    यदि (pos < अन्तः) {
        U16_FWD_1(text, pos, अन्तः);
        यदि(pos < अन्तः) {
            int32_t i = pos;
            UChar32 c;
            U16_अग्र(text, i, अन्तः, c);
            प्रत्यागम c;
        }
    }
    /* make current() return DONE */
    pos = अन्तः;
    प्रत्यागम समाप्तम्_2;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    यदि (pos < अन्तः) {
        UChar32 c;
        U16_अग्र(text, pos, अन्तः, c);
        प्रत्यागम c;
    } अन्यथा {
        प्रत्यागम समाप्तम्_2;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    यदि (pos > begin) {
        UChar32 c;
        U16_पूर्व(text, begin, pos, c);
        प्रत्यागम c;
    } अन्यथा {
        प्रत्यागम समाप्तम्_2;
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
    पक्ष k_अन्तः:
        pos = अन्तः + delta;
        भङ्ग;
    स्वभाव:
        भङ्ग;
    }

    यदि(pos < begin) {
        pos = begin;
    } अन्यथा यदि(pos > अन्तः) {
        pos = अन्तः;
    }

    प्रत्यागम pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    विकल्प(origin) {
    पक्ष kStart:
        pos = begin;
        यदि(delta > 0) {
            U16_FWD_N(text, pos, अन्तः, delta);
        }
        भङ्ग;
    पक्ष kCurrent:
        यदि(delta > 0) {
            U16_FWD_N(text, pos, अन्तः, delta);
        } अन्यथा {
            U16_पृष्ठभागः_N(text, begin, pos, -delta);
        }
        भङ्ग;
    पक्ष k_अन्तः:
        pos = अन्तः;
        यदि(delta < 0) {
            U16_पृष्ठभागः_N(text, begin, pos, -delta);
        }
        भङ्ग;
    स्वभाव:
        भङ्ग;
    }

    प्रत्यागम pos;
}

शून्य UCharCharacterIterator::समुच्चयः_Text(ConstChar16Ptr newText,
                                     int32_t      new_Text_दीर्घता) {
    text = newText;
    यदि (newText == शून्यसूचकः || new_Text_दीर्घता < 0) {
        new_Text_दीर्घता = 0;
    }
    अन्तः = text_दीर्घता = new_Text_दीर्घता;
    pos = begin = 0;
}

शून्य
UCharCharacterIterator::getText(UnicodeString& परिणाम) {
    परिणाम = UnicodeString(text, text_दीर्घता);
}

U_NAMESPACE_अन्तः
