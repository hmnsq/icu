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

#شامل "utypeinfo.h"  // for 'typeid' to work

#شامل "unicode/chariter.h"
#شامل "unicode/schriter.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(StringCharacterIterator)

StringCharacterIterator::StringCharacterIterator()
  : UCharCharacterIterator(),
    text()
{
  // NEVER DEFAULT CONSTRUCT!
}

StringCharacterIterator::StringCharacterIterator(ثابت UnicodeString& textStr)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length()),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = این->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(ثابت UnicodeString& textStr,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length(), textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = این->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(ثابت UnicodeString& textStr,
                                                 int32_t textBegin,
                                                 int32_t textEnd,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length(), textBegin, textEnd, textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = این->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(ثابت StringCharacterIterator& that)
  : UCharCharacterIterator(that),
    text(that.text)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = این->text.getBuffer();
}

StringCharacterIterator::~StringCharacterIterator() {
}

StringCharacterIterator&
StringCharacterIterator::operator=(ثابت StringCharacterIterator& that) U_LIFETIME_BOUND {
    UCharCharacterIterator::operator=(that);
    text = that.text;
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = این->text.getBuffer();
    بازگشت *این;
}

منطقی
StringCharacterIterator::operator==(ثابت ForwardCharacterIterator& that) ثابت {
    اگر (این == &that) {
        بازگشت درست;
    }

    // do not call UCharCharacterIterator::operator==()
    // because that checks for array pointer equality
    // while we compare UnicodeString objects

    اگر (typeid(*این) != typeid(that)) {
        بازگشت نادرست;
    }

    ثابت StringCharacterIterator&    realThat = static_cast<ثابت StringCharacterIterator&>(that);

    بازگشت text == realThat.text
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

StringCharacterIterator*
StringCharacterIterator::clone() ثابت {
    بازگشت جدید StringCharacterIterator(*این);
}

تهی
StringCharacterIterator::setText(ثابت UnicodeString& newText) {
    text = newText;
    UCharCharacterIterator::setText(text.getBuffer(), text.length());
}

تهی
StringCharacterIterator::getText(UnicodeString& result) {
    result = text;
}
U_NAMESPACE_END
