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

#ضمن "utypeinfo.h"  // for 'typeid' to work

#ضمن "unicode/chariter.h"
#ضمن "unicode/schriter.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(StringCharacterIterator)

StringCharacterIterator::StringCharacterIterator()
  : UCharCharacterIterator(),
    متن()
{
  // NEVER DEFAULT CONSTRUCT!
}

StringCharacterIterator::StringCharacterIterator(ثابت UnicodeString& متن_Str)
  : UCharCharacterIterator(متن_Str.get_میانگیر(), متن_Str.length()),
    متن(متن_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::متن = هذا->متن.get_میانگیر();
}

StringCharacterIterator::StringCharacterIterator(ثابت UnicodeString& متن_Str,
                                                 int32_t متن_Pos)
  : UCharCharacterIterator(متن_Str.get_میانگیر(), متن_Str.length(), متن_Pos),
    متن(متن_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::متن = هذا->متن.get_میانگیر();
}

StringCharacterIterator::StringCharacterIterator(ثابت UnicodeString& متن_Str,
                                                 int32_t متن_Begin,
                                                 int32_t متن_انتها,
                                                 int32_t متن_Pos)
  : UCharCharacterIterator(متن_Str.get_میانگیر(), متن_Str.length(), متن_Begin, متن_انتها, متن_Pos),
    متن(متن_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::متن = هذا->متن.get_میانگیر();
}

StringCharacterIterator::StringCharacterIterator(ثابت StringCharacterIterator& that)
  : UCharCharacterIterator(that),
    متن(that.متن)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::متن = هذا->متن.get_میانگیر();
}

StringCharacterIterator::~StringCharacterIterator() {
}

StringCharacterIterator&
StringCharacterIterator::عملگر=(ثابت StringCharacterIterator& that) U_LIFETIME_BOUND {
    UCharCharacterIterator::عملگر=(that);
    متن = that.متن;
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::متن = هذا->متن.get_میانگیر();
    أرجع *هذا;
}

منطقي
StringCharacterIterator::عملگر==(ثابت ForwardCharacterIterator& that) ثابت {
    إذا (هذا == &that) {
        أرجع درست;
    }

    // do not call UCharCharacterIterator::operator==()
    // because that checks for array pointer equality
    // while we compare UnicodeString objects

    إذا (typeid(*هذا) != typeid(that)) {
        أرجع خطأ;
    }

    ثابت StringCharacterIterator&    realThat = static_cast<ثابت StringCharacterIterator&>(that);

    أرجع متن == realThat.متن
        && pos == realThat.pos
        && begin == realThat.begin
        && انتها == realThat.انتها;
}

StringCharacterIterator*
StringCharacterIterator::clone() ثابت {
    أرجع جديد StringCharacterIterator(*هذا);
}

فراغ
StringCharacterIterator::مجموعه_متن(ثابت UnicodeString& new_متن) {
    متن = new_متن;
    UCharCharacterIterator::مجموعه_متن(متن.get_میانگیر(), متن.length());
}

فراغ
StringCharacterIterator::get_متن(UnicodeString& نتیجه) {
    نتیجه = متن;
}
U_NAMESPACE_انتها
