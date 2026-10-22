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
    متن()
{
  // NEVER DEFAULT CONSTRUCT!
}

StringCharacterIterator::StringCharacterIterator(ثابت UnicodeString& متن_Str)
  : UCharCharacterIterator(متن_Str.get_بفر(), متن_Str.length()),
    متن(متن_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::متن = یہ->متن.get_بفر();
}

StringCharacterIterator::StringCharacterIterator(ثابت UnicodeString& متن_Str,
                                                 int32_t متن_Pos)
  : UCharCharacterIterator(متن_Str.get_بفر(), متن_Str.length(), متن_Pos),
    متن(متن_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::متن = یہ->متن.get_بفر();
}

StringCharacterIterator::StringCharacterIterator(ثابت UnicodeString& متن_Str,
                                                 int32_t متن_Begin,
                                                 int32_t متن_انتہا,
                                                 int32_t متن_Pos)
  : UCharCharacterIterator(متن_Str.get_بفر(), متن_Str.length(), متن_Begin, متن_انتہا, متن_Pos),
    متن(متن_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::متن = یہ->متن.get_بفر();
}

StringCharacterIterator::StringCharacterIterator(ثابت StringCharacterIterator& that)
  : UCharCharacterIterator(that),
    متن(that.متن)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::متن = یہ->متن.get_بفر();
}

StringCharacterIterator::~StringCharacterIterator() {
}

StringCharacterIterator&
StringCharacterIterator::عامل=(ثابت StringCharacterIterator& that) U_LIFETIME_BOUND {
    UCharCharacterIterator::عامل=(that);
    متن = that.متن;
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::متن = یہ->متن.get_بفر();
    واپس *یہ;
}

منطقی
StringCharacterIterator::عامل==(ثابت ForwardCharacterIterator& that) ثابت {
    اگر (یہ == &that) {
        واپس سچ;
    }

    // do not call UCharCharacterIterator::operator==()
    // because that checks for array pointer equality
    // while we compare UnicodeString objects

    اگر (typeid(*یہ) != typeid(that)) {
        واپس جھوٹ;
    }

    ثابت StringCharacterIterator&    realThat = static_cast<ثابت StringCharacterIterator&>(that);

    واپس متن == realThat.متن
        && pos == realThat.pos
        && begin == realThat.begin
        && انتہا == realThat.انتہا;
}

StringCharacterIterator*
StringCharacterIterator::clone() ثابت {
    واپس نیا StringCharacterIterator(*یہ);
}

خالی
StringCharacterIterator::سیٹ_متن(ثابت UnicodeString& new_متن) {
    متن = new_متن;
    UCharCharacterIterator::سیٹ_متن(متن.get_بفر(), متن.length());
}

خالی
StringCharacterIterator::get_متن(UnicodeString& نتیجہ) {
    نتیجہ = متن;
}
U_NAMESPACE_انتہا
