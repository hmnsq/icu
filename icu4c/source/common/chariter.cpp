// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ضمن "unicode/chariter.h"

U_NAMESPACE_BEGIN

ForwardCharacterIterator::~ForwardCharacterIterator() {}
ForwardCharacterIterator::ForwardCharacterIterator()
: UObject()
{}
ForwardCharacterIterator::ForwardCharacterIterator(ثابت ForwardCharacterIterator &other)
: UObject(other)
{}


CharacterIterator::CharacterIterator()
: متن_اوږدتیا(0), pos(0), begin(0), پای_ټکی(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: متن_اوږدتیا(length), pos(0), begin(0), پای_ټکی(length) {
    إذا(متن_اوږدتیا < 0) {
        متن_اوږدتیا = پای_ټکی = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: متن_اوږدتیا(length), pos(position), begin(0), پای_ټکی(length) {
    إذا(متن_اوږدتیا < 0) {
        متن_اوږدتیا = پای_ټکی = 0;
    }
    إذا(pos < 0) {
        pos = 0;
    } وإلا إذا(pos > پای_ټکی) {
        pos = پای_ټکی;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t متن_Begin, int32_t متن_پای_ټکی, int32_t position)
: متن_اوږدتیا(length), pos(position), begin(متن_Begin), پای_ټکی(متن_پای_ټکی) {
    إذا(متن_اوږدتیا < 0) {
        متن_اوږدتیا = 0;
    }
    إذا(begin < 0) {
        begin = 0;
    } وإلا إذا(begin > متن_اوږدتیا) {
        begin = متن_اوږدتیا;
    }
    إذا(پای_ټکی < begin) {
        پای_ټکی = begin;
    } وإلا إذا(پای_ټکی > متن_اوږدتیا) {
        پای_ټکی = متن_اوږدتیا;
    }
    إذا(pos < begin) {
        pos = begin;
    } وإلا إذا(pos > پای_ټکی) {
        pos = پای_ټکی;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(ثابت CharacterIterator &that) :
ForwardCharacterIterator(that),
متن_اوږدتیا(that.متن_اوږدتیا), pos(that.pos), begin(that.begin), پای_ټکی(that.پای_ټکی)
{
}

CharacterIterator&
CharacterIterator::چلوونکی=(ثابت CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::چلوونکی=(that);
    متن_اوږدتیا = that.متن_اوږدتیا;
    pos = that.pos;
    begin = that.begin;
    پای_ټکی = that.پای_ټکی;
    أرجع *هذا;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::لومړی_Post_Inc() {
    سېټ_منزل_پیل();
    أرجع بل_Post_Inc();
}

UChar32
CharacterIterator::first32PostInc() {
    سېټ_منزل_پیل();
    أرجع next32PostInc();
}

U_NAMESPACE_پای_ټکی
