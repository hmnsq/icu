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
: متن_لمائی(0), pos(0), begin(0), سرا(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: متن_لمائی(length), pos(0), begin(0), سرا(length) {
    إذا(متن_لمائی < 0) {
        متن_لمائی = سرا = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: متن_لمائی(length), pos(position), begin(0), سرا(length) {
    إذا(متن_لمائی < 0) {
        متن_لمائی = سرا = 0;
    }
    إذا(pos < 0) {
        pos = 0;
    } وإلا إذا(pos > سرا) {
        pos = سرا;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t متن_Begin, int32_t متن_سرا, int32_t position)
: متن_لمائی(length), pos(position), begin(متن_Begin), سرا(متن_سرا) {
    إذا(متن_لمائی < 0) {
        متن_لمائی = 0;
    }
    إذا(begin < 0) {
        begin = 0;
    } وإلا إذا(begin > متن_لمائی) {
        begin = متن_لمائی;
    }
    إذا(سرا < begin) {
        سرا = begin;
    } وإلا إذا(سرا > متن_لمائی) {
        سرا = متن_لمائی;
    }
    إذا(pos < begin) {
        pos = begin;
    } وإلا إذا(pos > سرا) {
        pos = سرا;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(ثابت CharacterIterator &that) :
ForwardCharacterIterator(that),
متن_لمائی(that.متن_لمائی), pos(that.pos), begin(that.begin), سرا(that.سرا)
{
}

CharacterIterator&
CharacterIterator::عامل=(ثابت CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::عامل=(that);
    متن_لمائی = that.متن_لمائی;
    pos = that.pos;
    begin = that.begin;
    سرا = that.سرا;
    أرجع *هذا;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::پہلا_Post_Inc() {
    سیٹ_منزل_شروع();
    أرجع اگلا_Post_Inc();
}

UChar32
CharacterIterator::first32PostInc() {
    سیٹ_منزل_شروع();
    أرجع next32PostInc();
}

U_NAMESPACE_سرا
