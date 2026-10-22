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

StringCharacterIterator::StringCharacterIterator(स्थिरांक UnicodeString& textStr)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length()),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = हे->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(स्थिरांक UnicodeString& textStr,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length(), textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = हे->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(स्थिरांक UnicodeString& textStr,
                                                 int32_t textBegin,
                                                 int32_t textEnd,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length(), textBegin, textEnd, textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = हे->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(स्थिरांक StringCharacterIterator& that)
  : UCharCharacterIterator(that),
    text(that.text)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = हे->text.getBuffer();
}

StringCharacterIterator::~StringCharacterIterator() {
}

StringCharacterIterator&
StringCharacterIterator::चालक=(स्थिरांक StringCharacterIterator& that) U_LIFETIME_BOUND {
    UCharCharacterIterator::चालक=(that);
    text = that.text;
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = हे->text.getBuffer();
    परत *हे;
}

तार्किक
StringCharacterIterator::चालक==(स्थिरांक ForwardCharacterIterator& that) स्थिरांक {
    जर (हे == &that) {
        परत सत्य;
    }

    // do not call UCharCharacterIterator::operator==()
    // because that checks for array pointer equality
    // while we compare UnicodeString objects

    जर (typeid(*हे) != typeid(that)) {
        परत असत्य;
    }

    स्थिरांक StringCharacterIterator&    realThat = static_cast<स्थिरांक StringCharacterIterator&>(that);

    परत text == realThat.text
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

StringCharacterIterator*
StringCharacterIterator::clone() स्थिरांक {
    परत नवीन StringCharacterIterator(*हे);
}

शून्य
StringCharacterIterator::setText(स्थिरांक UnicodeString& newText) {
    text = newText;
    UCharCharacterIterator::setText(text.getBuffer(), text.length());
}

शून्य
StringCharacterIterator::getText(UnicodeString& result) {
    result = text;
}
U_NAMESPACE_END
