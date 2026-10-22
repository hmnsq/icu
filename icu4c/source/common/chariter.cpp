// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/chariter.h"

U_NAMESPACE_BEGIN

ForwardCharacterIterator::~ForwardCharacterIterator() {}
ForwardCharacterIterator::ForwardCharacterIterator()
: UObject()
{}
ForwardCharacterIterator::ForwardCharacterIterator(ਅਚਲ ForwardCharacterIterator &other)
: UObject(other)
{}


CharacterIterator::CharacterIterator()
: ਪਾਠ_ਲੰਮਾਈ(0), pos(0), begin(0), ਸਿਰਾ(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: ਪਾਠ_ਲੰਮਾਈ(length), pos(0), begin(0), ਸਿਰਾ(length) {
    ਜੇ(ਪਾਠ_ਲੰਮਾਈ < 0) {
        ਪਾਠ_ਲੰਮਾਈ = ਸਿਰਾ = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: ਪਾਠ_ਲੰਮਾਈ(length), pos(position), begin(0), ਸਿਰਾ(length) {
    ਜੇ(ਪਾਠ_ਲੰਮਾਈ < 0) {
        ਪਾਠ_ਲੰਮਾਈ = ਸਿਰਾ = 0;
    }
    ਜੇ(pos < 0) {
        pos = 0;
    } ਨਹੀਂਤਾਂ ਜੇ(pos > ਸਿਰਾ) {
        pos = ਸਿਰਾ;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t ਪਾਠ_Begin, int32_t ਪਾਠ_ਸਿਰਾ, int32_t position)
: ਪਾਠ_ਲੰਮਾਈ(length), pos(position), begin(ਪਾਠ_Begin), ਸਿਰਾ(ਪਾਠ_ਸਿਰਾ) {
    ਜੇ(ਪਾਠ_ਲੰਮਾਈ < 0) {
        ਪਾਠ_ਲੰਮਾਈ = 0;
    }
    ਜੇ(begin < 0) {
        begin = 0;
    } ਨਹੀਂਤਾਂ ਜੇ(begin > ਪਾਠ_ਲੰਮਾਈ) {
        begin = ਪਾਠ_ਲੰਮਾਈ;
    }
    ਜੇ(ਸਿਰਾ < begin) {
        ਸਿਰਾ = begin;
    } ਨਹੀਂਤਾਂ ਜੇ(ਸਿਰਾ > ਪਾਠ_ਲੰਮਾਈ) {
        ਸਿਰਾ = ਪਾਠ_ਲੰਮਾਈ;
    }
    ਜੇ(pos < begin) {
        pos = begin;
    } ਨਹੀਂਤਾਂ ਜੇ(pos > ਸਿਰਾ) {
        pos = ਸਿਰਾ;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(ਅਚਲ CharacterIterator &that) :
ForwardCharacterIterator(that),
ਪਾਠ_ਲੰਮਾਈ(that.ਪਾਠ_ਲੰਮਾਈ), pos(that.pos), begin(that.begin), ਸਿਰਾ(that.ਸਿਰਾ)
{
}

CharacterIterator&
CharacterIterator::ਸੰਚਾਲਕ=(ਅਚਲ CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::ਸੰਚਾਲਕ=(that);
    ਪਾਠ_ਲੰਮਾਈ = that.ਪਾਠ_ਲੰਮਾਈ;
    pos = that.pos;
    begin = that.begin;
    ਸਿਰਾ = that.ਸਿਰਾ;
    ਵਾਪਸ *ਇਹ;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::ਪਹਿਲਾ_Post_Inc() {
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ();
    ਵਾਪਸ ਅਗਲਾ_Post_Inc();
}

UChar32
CharacterIterator::first32PostInc() {
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ();
    ਵਾਪਸ next32PostInc();
}

U_NAMESPACE_ਸਿਰਾ
