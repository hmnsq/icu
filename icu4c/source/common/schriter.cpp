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

#ಅನ್ತರ್ಭಾವ "utypeinfo.h"  // for 'typeid' to work

#ಅನ್ತರ್ಭಾವ "unicode/chariter.h"
#ಅನ್ತರ್ಭಾವ "unicode/schriter.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(StringCharacterIterator)

StringCharacterIterator::StringCharacterIterator()
  : UCharCharacterIterator(),
    text()
{
  // NEVER DEFAULT CONSTRUCT!
}

StringCharacterIterator::StringCharacterIterator(ಧ್ರುವ UnicodeString& textStr)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length()),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = ಏತತ್->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(ಧ್ರುವ UnicodeString& textStr,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length(), textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = ಏತತ್->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(ಧ್ರುವ UnicodeString& textStr,
                                                 int32_t textBegin,
                                                 int32_t textEnd,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length(), textBegin, textEnd, textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = ಏತತ್->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(ಧ್ರುವ StringCharacterIterator& that)
  : UCharCharacterIterator(that),
    text(that.text)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = ಏತತ್->text.getBuffer();
}

StringCharacterIterator::~StringCharacterIterator() {
}

StringCharacterIterator&
StringCharacterIterator::operator=(ಧ್ರುವ StringCharacterIterator& that) U_LIFETIME_BOUND {
    UCharCharacterIterator::operator=(that);
    text = that.text;
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = ಏತತ್->text.getBuffer();
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ತಾರ್ಕಿಕ
StringCharacterIterator::operator==(ಧ್ರುವ ForwardCharacterIterator& that) ಧ್ರುವ {
    ಯದಿ (ಏತತ್ == &that) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }

    // do not call UCharCharacterIterator::operator==()
    // because that checks for array pointer equality
    // while we compare UnicodeString objects

    ಯದಿ (typeid(*ಏತತ್) != typeid(that)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }

    ಧ್ರುವ StringCharacterIterator&    realThat = static_cast<ಧ್ರುವ StringCharacterIterator&>(that);

    ಪ್ರತ್ಯಾಗಮ text == realThat.text
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

StringCharacterIterator*
StringCharacterIterator::clone() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ನೂತನ StringCharacterIterator(*ಏತತ್);
}

ಶೂನ್ಯ
StringCharacterIterator::setText(ಧ್ರುವ UnicodeString& newText) {
    text = newText;
    UCharCharacterIterator::setText(text.getBuffer(), text.length());
}

ಶೂನ್ಯ
StringCharacterIterator::getText(UnicodeString& result) {
    result = text;
}
U_NAMESPACE_END
