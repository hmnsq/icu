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
: متن_ڊيگهائي(0), pos(0), begin(0), پڇاڙي(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: متن_ڊيگهائي(length), pos(0), begin(0), پڇاڙي(length) {
    إذا(متن_ڊيگهائي < 0) {
        متن_ڊيگهائي = پڇاڙي = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: متن_ڊيگهائي(length), pos(position), begin(0), پڇاڙي(length) {
    إذا(متن_ڊيگهائي < 0) {
        متن_ڊيگهائي = پڇاڙي = 0;
    }
    إذا(pos < 0) {
        pos = 0;
    } وإلا إذا(pos > پڇاڙي) {
        pos = پڇاڙي;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t متن_Begin, int32_t متن_پڇاڙي, int32_t position)
: متن_ڊيگهائي(length), pos(position), begin(متن_Begin), پڇاڙي(متن_پڇاڙي) {
    إذا(متن_ڊيگهائي < 0) {
        متن_ڊيگهائي = 0;
    }
    إذا(begin < 0) {
        begin = 0;
    } وإلا إذا(begin > متن_ڊيگهائي) {
        begin = متن_ڊيگهائي;
    }
    إذا(پڇاڙي < begin) {
        پڇاڙي = begin;
    } وإلا إذا(پڇاڙي > متن_ڊيگهائي) {
        پڇاڙي = متن_ڊيگهائي;
    }
    إذا(pos < begin) {
        pos = begin;
    } وإلا إذا(pos > پڇاڙي) {
        pos = پڇاڙي;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(ثابت CharacterIterator &that) :
ForwardCharacterIterator(that),
متن_ڊيگهائي(that.متن_ڊيگهائي), pos(that.pos), begin(that.begin), پڇاڙي(that.پڇاڙي)
{
}

CharacterIterator&
CharacterIterator::عامل=(ثابت CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::عامل=(that);
    متن_ڊيگهائي = that.متن_ڊيگهائي;
    pos = that.pos;
    begin = that.begin;
    پڇاڙي = that.پڇاڙي;
    أرجع *هذا;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::پهريون_Post_Inc() {
    سيٽ_منزل_شروعات();
    أرجع ايندڙ_Post_Inc();
}

UChar32
CharacterIterator::first32PostInc() {
    سيٽ_منزل_شروعات();
    أرجع next32PostInc();
}

U_NAMESPACE_پڇاڙي
