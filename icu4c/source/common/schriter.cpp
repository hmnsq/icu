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

#অন্তর্ভাৱ "utypeinfo.h"  // for 'typeid' to work

#অন্তর্ভাৱ "unicode/chariter.h"
#অন্তর্ভাৱ "unicode/schriter.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(StringCharacterIterator)

StringCharacterIterator::StringCharacterIterator()
  : UCharCharacterIterator(),
    text()
{
  // NEVER DEFAULT CONSTRUCT!
}

StringCharacterIterator::StringCharacterIterator(ধ্রুৱ UnicodeString& textStr)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length()),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = এতত্->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(ধ্রুৱ UnicodeString& textStr,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length(), textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = এতত্->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(ধ্রুৱ UnicodeString& textStr,
                                                 int32_t textBegin,
                                                 int32_t textEnd,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length(), textBegin, textEnd, textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = এতত্->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(ধ্রুৱ StringCharacterIterator& that)
  : UCharCharacterIterator(that),
    text(that.text)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = এতত্->text.getBuffer();
}

StringCharacterIterator::~StringCharacterIterator() {
}

StringCharacterIterator&
StringCharacterIterator::operator=(ধ্রুৱ StringCharacterIterator& that) U_LIFETIME_BOUND {
    UCharCharacterIterator::operator=(that);
    text = that.text;
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = এতত্->text.getBuffer();
    ঘূৰাই *এতত্;
}

তার্কিক
StringCharacterIterator::operator==(ধ্রুৱ ForwardCharacterIterator& that) ধ্রুৱ {
    যদি (এতত্ == &that) {
        ঘূৰাই সঁচা;
    }

    // do not call UCharCharacterIterator::operator==()
    // because that checks for array pointer equality
    // while we compare UnicodeString objects

    যদি (typeid(*এতত্) != typeid(that)) {
        ঘূৰাই মিছা;
    }

    ধ্রুৱ StringCharacterIterator&    realThat = static_cast<ধ্রুৱ StringCharacterIterator&>(that);

    ঘূৰাই text == realThat.text
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

StringCharacterIterator*
StringCharacterIterator::clone() ধ্রুৱ {
    ঘূৰাই নূতন StringCharacterIterator(*এতত্);
}

শূন্য
StringCharacterIterator::setText(ধ্রুৱ UnicodeString& newText) {
    text = newText;
    UCharCharacterIterator::setText(text.getBuffer(), text.length());
}

শূন্য
StringCharacterIterator::getText(UnicodeString& result) {
    result = text;
}
U_NAMESPACE_END
