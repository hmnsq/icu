// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#شامل "unicode/chariter.h"

U_NAMESPACE_BEGIN

ForwardCharacterIterator::~ForwardCharacterIterator() {}
ForwardCharacterIterator::ForwardCharacterIterator()
: UObject()
{}
ForwardCharacterIterator::ForwardCharacterIterator(ثابت ForwardCharacterIterator &other)
: UObject(other)
{}


CharacterIterator::CharacterIterator()
: متن_لمبائی(0), pos(0), begin(0), انتہا(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: متن_لمبائی(length), pos(0), begin(0), انتہا(length) {
    اگر(متن_لمبائی < 0) {
        متن_لمبائی = انتہا = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: متن_لمبائی(length), pos(position), begin(0), انتہا(length) {
    اگر(متن_لمبائی < 0) {
        متن_لمبائی = انتہا = 0;
    }
    اگر(pos < 0) {
        pos = 0;
    } ورنہ اگر(pos > انتہا) {
        pos = انتہا;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t متن_Begin, int32_t متن_انتہا, int32_t position)
: متن_لمبائی(length), pos(position), begin(متن_Begin), انتہا(متن_انتہا) {
    اگر(متن_لمبائی < 0) {
        متن_لمبائی = 0;
    }
    اگر(begin < 0) {
        begin = 0;
    } ورنہ اگر(begin > متن_لمبائی) {
        begin = متن_لمبائی;
    }
    اگر(انتہا < begin) {
        انتہا = begin;
    } ورنہ اگر(انتہا > متن_لمبائی) {
        انتہا = متن_لمبائی;
    }
    اگر(pos < begin) {
        pos = begin;
    } ورنہ اگر(pos > انتہا) {
        pos = انتہا;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(ثابت CharacterIterator &that) :
ForwardCharacterIterator(that),
متن_لمبائی(that.متن_لمبائی), pos(that.pos), begin(that.begin), انتہا(that.انتہا)
{
}

CharacterIterator&
CharacterIterator::عامل=(ثابت CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::عامل=(that);
    متن_لمبائی = that.متن_لمبائی;
    pos = that.pos;
    begin = that.begin;
    انتہا = that.انتہا;
    واپس *یہ;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::پہلا_Post_Inc() {
    سیٹ_مقصد_آغاز();
    واپس اگلا_Post_Inc();
}

UChar32
CharacterIterator::first32PostInc() {
    سیٹ_مقصد_آغاز();
    واپس next32PostInc();
}

U_NAMESPACE_انتہا
