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

#ਅਨ੍ਤਰ੍ਭਾਵ "utypeinfo.h"  // for 'typeid' to work

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/chariter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/schriter.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(StringCharacterIterator)

StringCharacterIterator::StringCharacterIterator()
  : UCharCharacterIterator(),
    ਪਾਠ()
{
  // NEVER DEFAULT CONSTRUCT!
}

StringCharacterIterator::StringCharacterIterator(ਅਚਲ UnicodeString& ਪਾਠ_Str)
  : UCharCharacterIterator(ਪਾਠ_Str.get_ਬਫਰ(), ਪਾਠ_Str.length()),
    ਪਾਠ(ਪਾਠ_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::ਪਾਠ = ਇਹ->ਪਾਠ.get_ਬਫਰ();
}

StringCharacterIterator::StringCharacterIterator(ਅਚਲ UnicodeString& ਪਾਠ_Str,
                                                 int32_t ਪਾਠ_Pos)
  : UCharCharacterIterator(ਪਾਠ_Str.get_ਬਫਰ(), ਪਾਠ_Str.length(), ਪਾਠ_Pos),
    ਪਾਠ(ਪਾਠ_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::ਪਾਠ = ਇਹ->ਪਾਠ.get_ਬਫਰ();
}

StringCharacterIterator::StringCharacterIterator(ਅਚਲ UnicodeString& ਪਾਠ_Str,
                                                 int32_t ਪਾਠ_Begin,
                                                 int32_t ਪਾਠ_ਸਿਰਾ,
                                                 int32_t ਪਾਠ_Pos)
  : UCharCharacterIterator(ਪਾਠ_Str.get_ਬਫਰ(), ਪਾਠ_Str.length(), ਪਾਠ_Begin, ਪਾਠ_ਸਿਰਾ, ਪਾਠ_Pos),
    ਪਾਠ(ਪਾਠ_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::ਪਾਠ = ਇਹ->ਪਾਠ.get_ਬਫਰ();
}

StringCharacterIterator::StringCharacterIterator(ਅਚਲ StringCharacterIterator& that)
  : UCharCharacterIterator(that),
    ਪਾਠ(that.ਪਾਠ)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::ਪਾਠ = ਇਹ->ਪਾਠ.get_ਬਫਰ();
}

StringCharacterIterator::~StringCharacterIterator() {
}

StringCharacterIterator&
StringCharacterIterator::ਸੰਚਾਲਕ=(ਅਚਲ StringCharacterIterator& that) U_LIFETIME_BOUND {
    UCharCharacterIterator::ਸੰਚਾਲਕ=(that);
    ਪਾਠ = that.ਪਾਠ;
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::ਪਾਠ = ਇਹ->ਪਾਠ.get_ਬਫਰ();
    ਵਾਪਸ *ਇਹ;
}

ਤਾਰਕਿਕ
StringCharacterIterator::ਸੰਚਾਲਕ==(ਅਚਲ ForwardCharacterIterator& that) ਅਚਲ {
    ਜੇ (ਇਹ == &that) {
        ਵਾਪਸ ਸੱਚ;
    }

    // do not call UCharCharacterIterator::operator==()
    // because that checks for array pointer equality
    // while we compare UnicodeString objects

    ਜੇ (typeid(*ਇਹ) != typeid(that)) {
        ਵਾਪਸ ਝੂਠ;
    }

    ਅਚਲ StringCharacterIterator&    realThat = static_cast<ਅਚਲ StringCharacterIterator&>(that);

    ਵਾਪਸ ਪਾਠ == realThat.ਪਾਠ
        && pos == realThat.pos
        && begin == realThat.begin
        && ਸਿਰਾ == realThat.ਸਿਰਾ;
}

StringCharacterIterator*
StringCharacterIterator::clone() ਅਚਲ {
    ਵਾਪਸ ਨਵਾਂ StringCharacterIterator(*ਇਹ);
}

ਖਾਲੀ
StringCharacterIterator::ਸੈੱਟ_ਪਾਠ(ਅਚਲ UnicodeString& new_ਪਾਠ) {
    ਪਾਠ = new_ਪਾਠ;
    UCharCharacterIterator::ਸੈੱਟ_ਪਾਠ(ਪਾਠ.get_ਬਫਰ(), ਪਾਠ.length());
}

ਖਾਲੀ
StringCharacterIterator::get_ਪਾਠ(UnicodeString& ਨਤੀਜਾ) {
    ਨਤੀਜਾ = ਪਾਠ;
}
U_NAMESPACE_ਸਿਰਾ
