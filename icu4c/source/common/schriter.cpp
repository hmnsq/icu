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

#അന്തര്ഭാവ "utypeinfo.h"  // for 'typeid' to work

#അന്തര്ഭാവ "unicode/chariter.h"
#അന്തര്ഭാവ "unicode/schriter.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(StringCharacterIterator)

StringCharacterIterator::StringCharacterIterator()
  : UCharCharacterIterator(),
    text()
{
  // NEVER DEFAULT CONSTRUCT!
}

StringCharacterIterator::StringCharacterIterator(സ്ഥിരം UnicodeString& textStr)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length()),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = ഇത്->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(സ്ഥിരം UnicodeString& textStr,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length(), textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = ഇത്->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(സ്ഥിരം UnicodeString& textStr,
                                                 int32_t textBegin,
                                                 int32_t textEnd,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.getBuffer(), textStr.length(), textBegin, textEnd, textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = ഇത്->text.getBuffer();
}

StringCharacterIterator::StringCharacterIterator(സ്ഥിരം StringCharacterIterator& that)
  : UCharCharacterIterator(that),
    text(that.text)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = ഇത്->text.getBuffer();
}

StringCharacterIterator::~StringCharacterIterator() {
}

StringCharacterIterator&
StringCharacterIterator::operator=(സ്ഥിരം StringCharacterIterator& that) U_LIFETIME_BOUND {
    UCharCharacterIterator::operator=(that);
    text = that.text;
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = ഇത്->text.getBuffer();
    തിരികെ *ഇത്;
}

തർക്കം
StringCharacterIterator::operator==(സ്ഥിരം ForwardCharacterIterator& that) സ്ഥിരം {
    എങ്കിൽ (ഇത് == &that) {
        തിരികെ സത്യം;
    }

    // do not call UCharCharacterIterator::operator==()
    // because that checks for array pointer equality
    // while we compare UnicodeString objects

    എങ്കിൽ (typeid(*ഇത്) != typeid(that)) {
        തിരികെ അസത്യം;
    }

    സ്ഥിരം StringCharacterIterator&    realThat = static_cast<സ്ഥിരം StringCharacterIterator&>(that);

    തിരികെ text == realThat.text
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

StringCharacterIterator*
StringCharacterIterator::clone() സ്ഥിരം {
    തിരികെ പുതിയ StringCharacterIterator(*ഇത്);
}

ശൂന്യം
StringCharacterIterator::setText(സ്ഥിരം UnicodeString& newText) {
    text = newText;
    UCharCharacterIterator::setText(text.getBuffer(), text.length());
}

ശൂന്യം
StringCharacterIterator::getText(UnicodeString& result) {
    result = text;
}
U_NAMESPACE_END
