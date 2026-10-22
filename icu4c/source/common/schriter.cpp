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

#অন্তর্ভাব "utypeinfo.h"  // for 'typeid' to work

#অন্তর্ভাব "unicode/chariter.h"
#অন্তর্ভাব "unicode/schriter.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(StringCharacterIterator)

StringCharacterIterator::StringCharacterIterator()
  : UCharCharacterIterator(),
    text()
{
  // NEVER DEFAULT CONSTRUCT!
}

StringCharacterIterator::StringCharacterIterator(ধ্রুবক UnicodeString& textStr)
  : UCharCharacterIterator(textStr.get_বাফার(), textStr.length()),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = এই->text.get_বাফার();
}

StringCharacterIterator::StringCharacterIterator(ধ্রুবক UnicodeString& textStr,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.get_বাফার(), textStr.length(), textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = এই->text.get_বাফার();
}

StringCharacterIterator::StringCharacterIterator(ধ্রুবক UnicodeString& textStr,
                                                 int32_t textBegin,
                                                 int32_t textEnd,
                                                 int32_t textPos)
  : UCharCharacterIterator(textStr.get_বাফার(), textStr.length(), textBegin, textEnd, textPos),
    text(textStr)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = এই->text.get_বাফার();
}

StringCharacterIterator::StringCharacterIterator(ধ্রুবক StringCharacterIterator& that)
  : UCharCharacterIterator(that),
    text(that.text)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = এই->text.get_বাফার();
}

StringCharacterIterator::~StringCharacterIterator() {
}

StringCharacterIterator&
StringCharacterIterator::operator=(ধ্রুবক StringCharacterIterator& that) U_LIFETIME_BOUND {
    UCharCharacterIterator::operator=(that);
    text = that.text;
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::text = এই->text.get_বাফার();
    ফেরত *এই;
}

যৌক্তিক
StringCharacterIterator::operator==(ধ্রুবক ForwardCharacterIterator& that) ধ্রুবক {
    যদি (এই == &that) {
        ফেরত সত্য;
    }

    // do not call UCharCharacterIterator::operator==()
    // because that checks for array pointer equality
    // while we compare UnicodeString objects

    যদি (typeid(*এই) != typeid(that)) {
        ফেরত মিথ্যা;
    }

    ধ্রুবক StringCharacterIterator&    realThat = static_cast<ধ্রুবক StringCharacterIterator&>(that);

    ফেরত text == realThat.text
        && pos == realThat.pos
        && begin == realThat.begin
        && end == realThat.end;
}

StringCharacterIterator*
StringCharacterIterator::clone() ধ্রুবক {
    ফেরত নতুন StringCharacterIterator(*এই);
}

শূন্য
StringCharacterIterator::setText(ধ্রুবক UnicodeString& newText) {
    text = newText;
    UCharCharacterIterator::setText(text.get_বাফার(), text.length());
}

শূন্য
StringCharacterIterator::getText(UnicodeString& ফলাফল) {
    ফলাফল = text;
}
U_NAMESPACE_END
