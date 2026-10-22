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

#ଅନ୍ତର୍ଭାଵ "utypeinfo.h"  // for 'typeid' to work

#ଅନ୍ତର୍ଭାଵ "unicode/chariter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/schriter.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(StringCharacterIterator)

StringCharacterIterator::StringCharacterIterator()
  : UCharCharacterIterator(),
    text()
{
  // NEVER DEFAULT CONSTRUCT!
}

StringCharacterIterator::StringCharacterIterator(ଅଚଳ UnicodeString& textStr)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length()),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = ଏହି->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(ଅଚଳ UnicodeString& textStr,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length(), textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = ଏହି->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(ଅଚଳ UnicodeString& textStr,
                                                 int32_t textBegin,
                                                 int32_t textEnd,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length(), textBegin, textEnd, textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = ଏହି->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(ଅଚଳ StringCharacterIterator& that)
  : UCharCharacterIterator(that),
    text(that.text)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = ଏହି->text.getBuffer();
}

StringCharacterIterator::~StringCharacterIterator() {
}

StringCharacterIterator&
StringCharacterIterator::operator=(ଅଚଳ StringCharacterIterator& that) U_LIFETIME_BOUND {
    UCharCharacterIterator::operator=(that);
    text = that.text;
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = ଏହି->text.getBuffer();
    ଫେରନ୍ତୁ *ଏହି;
}

ତାର୍କିକ
StringCharacterIterator::operator==(ଅଚଳ ForwardCharacterIterator& that) ଅଚଳ {
    ଯଦି (ଏହି == &that) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }

    // do not call UCharCharacterIterator::operator==()
    // because that checks for array pointer equality
    // while we compare UnicodeString objects

    ଯଦି (typeid(*ଏହି) != typeid(that)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    ଅଚଳ StringCharacterIterator&    realThat = static_cast<ଅଚଳ StringCharacterIterator&>(that);

    ଫେରନ୍ତୁ text == realThat.text
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

StringCharacterIterator*
StringCharacterIterator::clone() ଅଚଳ {
    ଫେରନ୍ତୁ ନୂତନ StringCharacterIterator(*ଏହି);
}

ଶୂନ୍ୟ
StringCharacterIterator::setText(ଅଚଳ UnicodeString& newText) {
    text = newText;
    UCharCharacterIterator::setText(text.getBuffer(), text.length());
}

ଶୂନ୍ୟ
StringCharacterIterator::getText(UnicodeString& result) {
    result = text;
}
U_NAMESPACE_END
