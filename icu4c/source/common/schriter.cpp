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
  : UCharCharacterIterator(متن_Str.get_بفر(), متن_Str.length()),
    متن(متن_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::متن = هذا->متن.get_بفر();
}

StringCharacterIterator::StringCharacterIterator(ثابت UnicodeString& متن_Str,
                                                 int32_t متن_Pos)
  : UCharCharacterIterator(متن_Str.get_بفر(), متن_Str.length(), متن_Pos),
    متن(متن_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::متن = هذا->متن.get_بفر();
}

StringCharacterIterator::StringCharacterIterator(ثابت UnicodeString& متن_Str,
                                                 int32_t متن_Begin,
                                                 int32_t متن_پای_ټکی,
                                                 int32_t متن_Pos)
  : UCharCharacterIterator(متن_Str.get_بفر(), متن_Str.length(), متن_Begin, متن_پای_ټکی, متن_Pos),
    متن(متن_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::متن = هذا->متن.get_بفر();
}

StringCharacterIterator::StringCharacterIterator(ثابت StringCharacterIterator& that)
  : UCharCharacterIterator(that),
    متن(that.متن)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::متن = هذا->متن.get_بفر();
}

StringCharacterIterator::~StringCharacterIterator() {
}

StringCharacterIterator&
StringCharacterIterator::چلوونکی=(ثابت StringCharacterIterator& that) U_LIFETIME_BOUND {
    UCharCharacterIterator::چلوونکی=(that);
    متن = that.متن;
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::متن = هذا->متن.get_بفر();
    أرجع *هذا;
}

منطقي
StringCharacterIterator::چلوونکی==(ثابت ForwardCharacterIterator& that) ثابت {
    إذا (هذا == &that) {
        أرجع ریښتیا;
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
        && پای_ټکی == realThat.پای_ټکی;
}

StringCharacterIterator*
StringCharacterIterator::clone() ثابت {
    أرجع جديد StringCharacterIterator(*هذا);
}

فراغ
StringCharacterIterator::سېټ_متن(ثابت UnicodeString& new_متن) {
    متن = new_متن;
    UCharCharacterIterator::سېټ_متن(متن.get_بفر(), متن.length());
}

فراغ
StringCharacterIterator::get_متن(UnicodeString& پایله) {
    پایله = متن;
}
U_NAMESPACE_پای_ټکی
