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
: متن_درازی(0), pos(0), begin(0), انتها(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: متن_درازی(length), pos(0), begin(0), انتها(length) {
    إذا(متن_درازی < 0) {
        متن_درازی = انتها = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: متن_درازی(length), pos(position), begin(0), انتها(length) {
    إذا(متن_درازی < 0) {
        متن_درازی = انتها = 0;
    }
    إذا(pos < 0) {
        pos = 0;
    } وإلا إذا(pos > انتها) {
        pos = انتها;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t متن_Begin, int32_t متن_انتها, int32_t position)
: متن_درازی(length), pos(position), begin(متن_Begin), انتها(متن_انتها) {
    إذا(متن_درازی < 0) {
        متن_درازی = 0;
    }
    إذا(begin < 0) {
        begin = 0;
    } وإلا إذا(begin > متن_درازی) {
        begin = متن_درازی;
    }
    إذا(انتها < begin) {
        انتها = begin;
    } وإلا إذا(انتها > متن_درازی) {
        انتها = متن_درازی;
    }
    إذا(pos < begin) {
        pos = begin;
    } وإلا إذا(pos > انتها) {
        pos = انتها;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(ثابت CharacterIterator &that) :
ForwardCharacterIterator(that),
متن_درازی(that.متن_درازی), pos(that.pos), begin(that.begin), انتها(that.انتها)
{
}

CharacterIterator&
CharacterIterator::عملگر=(ثابت CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::عملگر=(that);
    متن_درازی = that.متن_درازی;
    pos = that.pos;
    begin = that.begin;
    انتها = that.انتها;
    أرجع *هذا;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::اولی_Post_Inc() {
    مجموعه_مقصد_شروع();
    أرجع بعدی_Post_Inc();
}

UChar32
CharacterIterator::first32PostInc() {
    مجموعه_مقصد_شروع();
    أرجع next32PostInc();
}

U_NAMESPACE_انتها
