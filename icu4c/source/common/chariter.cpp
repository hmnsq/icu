// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#अन्तर्भाव "unicode/chariter.h"

U_NAMESPACE_BEGIN

ForwardCharacterIterator::~ForwardCharacterIterator() {}
ForwardCharacterIterator::ForwardCharacterIterator()
: UObject()
{}
ForwardCharacterIterator::ForwardCharacterIterator(ध्रुव ForwardCharacterIterator &other)
: UObject(other)
{}


CharacterIterator::CharacterIterator()
: textLength(0), pos(0), begin(0), end(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: textLength(length), pos(0), begin(0), end(length) {
    जइ(textLength < 0) {
        textLength = end = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: textLength(length), pos(position), begin(0), end(length) {
    जइ(textLength < 0) {
        textLength = end = 0;
    }
    जइ(pos < 0) {
        pos = 0;
    } अण्णहा जइ(pos > end) {
        pos = end;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t textBegin, int32_t textEnd, int32_t position)
: textLength(length), pos(position), begin(textBegin), end(textEnd) {
    जइ(textLength < 0) {
        textLength = 0;
    }
    जइ(begin < 0) {
        begin = 0;
    } अण्णहा जइ(begin > textLength) {
        begin = textLength;
    }
    जइ(end < begin) {
        end = begin;
    } अण्णहा जइ(end > textLength) {
        end = textLength;
    }
    जइ(pos < begin) {
        pos = begin;
    } अण्णहा जइ(pos > end) {
        pos = end;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(ध्रुव CharacterIterator &that) :
ForwardCharacterIterator(that),
textLength(that.textLength), pos(that.pos), begin(that.begin), end(that.end)
{
}

CharacterIterator&
CharacterIterator::operator=(ध्रुव CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::operator=(that);
    textLength = that.textLength;
    pos = that.pos;
    begin = that.begin;
    end = that.end;
    प्रत्यागम *एतत्;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::firstPostInc() {
    setToStart();
    प्रत्यागम nextPostInc();
}

UChar32
CharacterIterator::first32PostInc() {
    setToStart();
    प्रत्यागम next32PostInc();
}

U_NAMESPACE_END
