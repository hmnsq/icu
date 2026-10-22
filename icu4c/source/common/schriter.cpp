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

#அந்தர்பாவ "utypeinfo.h"  // for 'typeid' to work

#அந்தர்பாவ "unicode/chariter.h"
#அந்தர்பாவ "unicode/schriter.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(StringCharacterIterator)

StringCharacterIterator::StringCharacterIterator()
  : UCharCharacterIterator(),
    text()
{
  // NEVER DEFAULT CONSTRUCT!
}

StringCharacterIterator::StringCharacterIterator(மாறிலி UnicodeString& textStr)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length()),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = இது->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(மாறிலி UnicodeString& textStr,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length(), textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = இது->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(மாறிலி UnicodeString& textStr,
                                                 int32_t textBegin,
                                                 int32_t textEnd,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length(), textBegin, textEnd, textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = இது->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(மாறிலி StringCharacterIterator& that)
  : UCharCharacterIterator(that),
    text(that.text)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = இது->text.getBuffer();
}

StringCharacterIterator::~StringCharacterIterator() {
}

StringCharacterIterator&
StringCharacterIterator::operator=(மாறிலி StringCharacterIterator& that) U_LIFETIME_BOUND {
    UCharCharacterIterator::operator=(that);
    text = that.text;
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = இது->text.getBuffer();
    திரும்பு *இது;
}

தருக்க
StringCharacterIterator::operator==(மாறிலி ForwardCharacterIterator& that) மாறிலி {
    எனில் (இது == &that) {
        திரும்பு உண்மை;
    }

    // do not call UCharCharacterIterator::operator==()
    // because that checks for array pointer equality
    // while we compare UnicodeString objects

    எனில் (typeid(*இது) != typeid(that)) {
        திரும்பு பொய்;
    }

    மாறிலி StringCharacterIterator&    realThat = static_cast<மாறிலி StringCharacterIterator&>(that);

    திரும்பு text == realThat.text
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

StringCharacterIterator*
StringCharacterIterator::clone() மாறிலி {
    திரும்பு புதிய StringCharacterIterator(*இது);
}

வெறுமை
StringCharacterIterator::setText(மாறிலி UnicodeString& newText) {
    text = newText;
    UCharCharacterIterator::setText(text.getBuffer(), text.length());
}

வெறுமை
StringCharacterIterator::getText(UnicodeString& result) {
    result = text;
}
U_NAMESPACE_END
