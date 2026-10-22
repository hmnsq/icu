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
    ಪಠ್ಯ()
{
  // NEVER DEFAULT CONSTRUCT!
}

StringCharacterIterator::StringCharacterIterator(ಸ್ಥಿರಾಂಕ UnicodeString& ಪಠ್ಯ_Str)
  : UCharCharacterIterator(ಪಠ್ಯ_Str.get_ಬಫರ್(), ಪಠ್ಯ_Str.length()),
    ಪಠ್ಯ(ಪಠ್ಯ_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::ಪಠ್ಯ = ಇದು->ಪಠ್ಯ.get_ಬಫರ್();
}

StringCharacterIterator::StringCharacterIterator(ಸ್ಥಿರಾಂಕ UnicodeString& ಪಠ್ಯ_Str,
                                                 int32_t ಪಠ್ಯ_Pos)
  : UCharCharacterIterator(ಪಠ್ಯ_Str.get_ಬಫರ್(), ಪಠ್ಯ_Str.length(), ಪಠ್ಯ_Pos),
    ಪಠ್ಯ(ಪಠ್ಯ_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::ಪಠ್ಯ = ಇದು->ಪಠ್ಯ.get_ಬಫರ್();
}

StringCharacterIterator::StringCharacterIterator(ಸ್ಥಿರಾಂಕ UnicodeString& ಪಠ್ಯ_Str,
                                                 int32_t ಪಠ್ಯ_Begin,
                                                 int32_t ಪಠ್ಯ_ಕೊನೆ,
                                                 int32_t ಪಠ್ಯ_Pos)
  : UCharCharacterIterator(ಪಠ್ಯ_Str.get_ಬಫರ್(), ಪಠ್ಯ_Str.length(), ಪಠ್ಯ_Begin, ಪಠ್ಯ_ಕೊನೆ, ಪಠ್ಯ_Pos),
    ಪಠ್ಯ(ಪಠ್ಯ_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::ಪಠ್ಯ = ಇದು->ಪಠ್ಯ.get_ಬಫರ್();
}

StringCharacterIterator::StringCharacterIterator(ಸ್ಥಿರಾಂಕ StringCharacterIterator& that)
  : UCharCharacterIterator(that),
    ಪಠ್ಯ(that.ಪಠ್ಯ)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::ಪಠ್ಯ = ಇದು->ಪಠ್ಯ.get_ಬಫರ್();
}

StringCharacterIterator::~StringCharacterIterator() {
}

StringCharacterIterator&
StringCharacterIterator::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ StringCharacterIterator& that) U_LIFETIME_BOUND {
    UCharCharacterIterator::ಕ್ರಿಯಾಕಾರಕ=(that);
    ಪಠ್ಯ = that.ಪಠ್ಯ;
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::ಪಠ್ಯ = ಇದು->ಪಠ್ಯ.get_ಬಫರ್();
    ಹಿಂತಿರುಗಿ *ಇದು;
}

ತಾರ್ಕಿಕ
StringCharacterIterator::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ForwardCharacterIterator& that) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (ಇದು == &that) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    // do not call UCharCharacterIterator::operator==()
    // because that checks for array pointer equality
    // while we compare UnicodeString objects

    ಆದರೆ (typeid(*ಇದು) != typeid(that)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಸ್ಥಿರಾಂಕ StringCharacterIterator&    realThat = static_cast<ಸ್ಥಿರಾಂಕ StringCharacterIterator&>(that);

    ಹಿಂತಿರುಗಿ ಪಠ್ಯ == realThat.ಪಠ್ಯ
        && pos == realThat.pos
        && begin == realThat.begin
        && ಕೊನೆ == realThat.ಕೊನೆ;
}

StringCharacterIterator*
StringCharacterIterator::clone() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಹೊಸ StringCharacterIterator(*ಇದು);
}

ಶೂನ್ಯ
StringCharacterIterator::ಗಣ_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ UnicodeString& new_ಪಠ್ಯ) {
    ಪಠ್ಯ = new_ಪಠ್ಯ;
    UCharCharacterIterator::ಗಣ_ಪಠ್ಯ(ಪಠ್ಯ.get_ಬಫರ್(), ಪಠ್ಯ.length());
}

ಶೂನ್ಯ
StringCharacterIterator::get_ಪಠ್ಯ(UnicodeString& ಫಲಿತಾಂಶ) {
    ಫಲಿತಾಂಶ = ಪಠ್ಯ;
}
U_NAMESPACE_ಕೊನೆ
