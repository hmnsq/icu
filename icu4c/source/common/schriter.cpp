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

#समावेश "utypeinfo.h"  // for 'typeid' to work

#समावेश "unicode/chariter.h"
#समावेश "unicode/schriter.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(StringCharacterIterator)

StringCharacterIterator::StringCharacterIterator()
  : UCharCharacterIterator(),
    text()
{
  // NEVER DEFAULT CONSTRUCT!
}

StringCharacterIterator::StringCharacterIterator(स्थिर UnicodeString& textStr)
  : UCharCharacterIterator(textStr.get_बफर(), textStr.length()),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = यह->text.get_बफर();
}

StringCharacterIterator::StringCharacterIterator(स्थिर UnicodeString& textStr,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.get_बफर(), textStr.length(), textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = यह->text.get_बफर();
}

StringCharacterIterator::StringCharacterIterator(स्थिर UnicodeString& textStr,
                                                 int32_t textBegin,
                                                 int32_t textEnd,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.get_बफर(), textStr.length(), textBegin, textEnd, textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = यह->text.get_बफर();
}

StringCharacterIterator::StringCharacterIterator(स्थिर StringCharacterIterator& that)
  : UCharCharacterIterator(that),
    text(that.text)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = यह->text.get_बफर();
}

StringCharacterIterator::~StringCharacterIterator() {
}

StringCharacterIterator&
StringCharacterIterator::चालक=(स्थिर StringCharacterIterator& that) U_LIFETIME_BOUND {
    UCharCharacterIterator::चालक=(that);
    text = that.text;
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = यह->text.get_बफर();
    वापस *यह;
}

bool
StringCharacterIterator::चालक==(स्थिर ForwardCharacterIterator& that) स्थिर {
    अगर (यह == &that) {
        वापस true;
    }

    // do not call UCharCharacterIterator::operator==()
    // because that checks for array pointer equality
    // while we compare UnicodeString objects

    अगर (typeid(*यह) != typeid(that)) {
        वापस false;
    }

    स्थिर StringCharacterIterator&    realThat = static_cast<स्थिर StringCharacterIterator&>(that);

    वापस text == realThat.text
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

StringCharacterIterator*
StringCharacterIterator::clone() स्थिर {
    वापस नया StringCharacterIterator(*यह);
}

व्योम
StringCharacterIterator::setText(स्थिर UnicodeString& newText) {
    text = newText;
    UCharCharacterIterator::setText(text.get_बफर(), text.length());
}

व्योम
StringCharacterIterator::getText(UnicodeString& परिणाम) {
    परिणाम = text;
}
U_NAMESPACE_END
