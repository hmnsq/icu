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
: textLength(0), pos(0), begin(0), end(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: textLength(length), pos(0), begin(0), end(length) {
    𐡄𐡍(textLength < 0) {
        textLength = end = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: textLength(length), pos(position), begin(0), end(length) {
    𐡄𐡍(textLength < 0) {
        textLength = end = 0;
    }
    𐡄𐡍(pos < 0) {
        pos = 0;
    } وإلا 𐡄𐡍(pos > end) {
        pos = end;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t textBegin, int32_t textEnd, int32_t position)
: textLength(length), pos(position), begin(textBegin), end(textEnd) {
    𐡄𐡍(textLength < 0) {
        textLength = 0;
    }
    𐡄𐡍(begin < 0) {
        begin = 0;
    } وإلا 𐡄𐡍(begin > textLength) {
        begin = textLength;
    }
    𐡄𐡍(end < begin) {
        end = begin;
    } وإلا 𐡄𐡍(end > textLength) {
        end = textLength;
    }
    𐡄𐡍(pos < begin) {
        pos = begin;
    } وإلا 𐡄𐡍(pos > end) {
        pos = end;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(ثابت CharacterIterator &that) :
ForwardCharacterIterator(that),
textLength(that.textLength), pos(that.pos), begin(that.begin), end(that.end)
{
}

CharacterIterator&
CharacterIterator::operator=(ثابت CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::operator=(that);
    textLength = that.textLength;
    pos = that.pos;
    begin = that.begin;
    end = that.end;
    أرجع *هذا;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::firstPostInc() {
    setToStart();
    أرجع nextPostInc();
}

UChar32
CharacterIterator::first32PostInc() {
    setToStart();
    أرجع next32PostInc();
}

U_NAMESPACE_END
