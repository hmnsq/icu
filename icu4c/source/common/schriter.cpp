// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*
* File schriter.cpp
*
* Modification History:
*
*   Date        Name        Description
*  05/05/99     stephen     Cleaned up.
******************************************************************************
*/

#अन्तर्भाव "utypeinfo.h"  // for 'typeid' to work

#अन्तर्भाव "unicode/chariter.h"
#अन्तर्भाव "unicode/schriter.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(StringCharacterIterator)

StringCharacterIterator::StringCharacterIterator()
  : UCharCharacterIterator(),
    text()
{
  // NEVER DEFAULT CONSTRUCT!
}

StringCharacterIterator::StringCharacterIterator(ध्रुव UnicodeString& textStr)
  : UCharCharacterIterator(textStr.get_बफर(), textStr.length()),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = एतत्->text.get_बफर();
}

StringCharacterIterator::StringCharacterIterator(ध्रुव UnicodeString& textStr,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.get_बफर(), textStr.length(), textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = एतत्->text.get_बफर();
}

StringCharacterIterator::StringCharacterIterator(ध्रुव UnicodeString& textStr,
                                                 int32_t textBegin,
                                                 int32_t text_अन्तः,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.get_बफर(), textStr.length(), textBegin, text_अन्तः, textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = एतत्->text.get_बफर();
}

StringCharacterIterator::StringCharacterIterator(ध्रुव StringCharacterIterator& that)
  : UCharCharacterIterator(that),
    text(that.text)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = एतत्->text.get_बफर();
}

StringCharacterIterator::~StringCharacterIterator() {
}

StringCharacterIterator&
StringCharacterIterator::संक्रियकः=(ध्रुव StringCharacterIterator& that) U_LIFETIME_BOUND {
    UCharCharacterIterator::संक्रियकः=(that);
    text = that.text;
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = एतत्->text.get_बफर();
    प्रत्यागम *एतत्;
}

तार्किक
StringCharacterIterator::संक्रियकः==(ध्रुव ForwardCharacterIterator& that) ध्रुव {
    यदि (एतत् == &that) {
        प्रत्यागम सत्य;
    }

    // do not call UCharCharacterIterator::operator==()
    // because that checks for array pointer equality
    // while we compare UnicodeString objects

    यदि (typeid(*एतत्) != typeid(that)) {
        प्रत्यागम असत्य;
    }

    ध्रुव StringCharacterIterator&    realThat = static_cast<ध्रुव StringCharacterIterator&>(that);

    प्रत्यागम text == realThat.text
        && pos == realThat.pos
        && begin == realThat.begin
        && अन्तः == realThat.अन्तः;
}

StringCharacterIterator*
StringCharacterIterator::clone() ध्रुव {
    प्रत्यागम नूतन StringCharacterIterator(*एतत्);
}

शून्य
StringCharacterIterator::समुच्चयः_Text(ध्रुव UnicodeString& newText) {
    text = newText;
    UCharCharacterIterator::समुच्चयः_Text(text.get_बफर(), text.length());
}

शून्य
StringCharacterIterator::getText(UnicodeString& परिणाम) {
    परिणाम = text;
}
U_NAMESPACE_अन्तः
