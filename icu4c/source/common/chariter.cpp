// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ಅನ್ತರ್ಭಾವ "unicode/chariter.h"

U_NAMESPACE_BEGIN

ForwardCharacterIterator::~ForwardCharacterIterator() {}
ForwardCharacterIterator::ForwardCharacterIterator()
: UObject()
{}
ForwardCharacterIterator::ForwardCharacterIterator(ಸ್ಥಿರಾಂಕ ForwardCharacterIterator &other)
: UObject(other)
{}


CharacterIterator::CharacterIterator()
: ಪಠ್ಯ_ಉದ್ದಳತೆ(0), pos(0), begin(0), ಕೊನೆ(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: ಪಠ್ಯ_ಉದ್ದಳತೆ(length), pos(0), begin(0), ಕೊನೆ(length) {
    ಆದರೆ(ಪಠ್ಯ_ಉದ್ದಳತೆ < 0) {
        ಪಠ್ಯ_ಉದ್ದಳತೆ = ಕೊನೆ = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: ಪಠ್ಯ_ಉದ್ದಳತೆ(length), pos(position), begin(0), ಕೊನೆ(length) {
    ಆದರೆ(ಪಠ್ಯ_ಉದ್ದಳತೆ < 0) {
        ಪಠ್ಯ_ಉದ್ದಳತೆ = ಕೊನೆ = 0;
    }
    ಆದರೆ(pos < 0) {
        pos = 0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pos > ಕೊನೆ) {
        pos = ಕೊನೆ;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t ಪಠ್ಯ_Begin, int32_t ಪಠ್ಯ_ಕೊನೆ, int32_t position)
: ಪಠ್ಯ_ಉದ್ದಳತೆ(length), pos(position), begin(ಪಠ್ಯ_Begin), ಕೊನೆ(ಪಠ್ಯ_ಕೊನೆ) {
    ಆದರೆ(ಪಠ್ಯ_ಉದ್ದಳತೆ < 0) {
        ಪಠ್ಯ_ಉದ್ದಳತೆ = 0;
    }
    ಆದರೆ(begin < 0) {
        begin = 0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(begin > ಪಠ್ಯ_ಉದ್ದಳತೆ) {
        begin = ಪಠ್ಯ_ಉದ್ದಳತೆ;
    }
    ಆದರೆ(ಕೊನೆ < begin) {
        ಕೊನೆ = begin;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಕೊನೆ > ಪಠ್ಯ_ಉದ್ದಳತೆ) {
        ಕೊನೆ = ಪಠ್ಯ_ಉದ್ದಳತೆ;
    }
    ಆದರೆ(pos < begin) {
        pos = begin;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pos > ಕೊನೆ) {
        pos = ಕೊನೆ;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(ಸ್ಥಿರಾಂಕ CharacterIterator &that) :
ForwardCharacterIterator(that),
ಪಠ್ಯ_ಉದ್ದಳತೆ(that.ಪಠ್ಯ_ಉದ್ದಳತೆ), pos(that.pos), begin(that.begin), ಕೊನೆ(that.ಕೊನೆ)
{
}

CharacterIterator&
CharacterIterator::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::ಕ್ರಿಯಾಕಾರಕ=(that);
    ಪಠ್ಯ_ಉದ್ದಳತೆ = that.ಪಠ್ಯ_ಉದ್ದಳತೆ;
    pos = that.pos;
    begin = that.begin;
    ಕೊನೆ = that.ಕೊನೆ;
    ಹಿಂತಿರುಗಿ *ಇದು;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::ಮೊದಲ_Post_Inc() {
    ಗಣ_ಗಮ್ಯ_ಆರಂಭ();
    ಹಿಂತಿರುಗಿ ಮುಂದಿನ_Post_Inc();
}

UChar32
CharacterIterator::first32PostInc() {
    ಗಣ_ಗಮ್ಯ_ಆರಂಭ();
    ಹಿಂತಿರುಗಿ next32PostInc();
}

U_NAMESPACE_ಕೊನೆ
