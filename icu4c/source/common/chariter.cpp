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
ForwardCharacterIterator::ForwardCharacterIterator(स्थिरांक ForwardCharacterIterator &other)
: UObject(other)
{}


CharacterIterator::CharacterIterator()
: textLength(0), pos(0), begin(0), end(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: textLength(length), pos(0), begin(0), end(length) {
    जर(textLength < 0) {
        textLength = end = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: textLength(length), pos(position), begin(0), end(length) {
    जर(textLength < 0) {
        textLength = end = 0;
    }
    जर(pos < 0) {
        pos = 0;
    } अन्यथा जर(pos > end) {
        pos = end;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t textBegin, int32_t textEnd, int32_t position)
: textLength(length), pos(position), begin(textBegin), end(textEnd) {
    जर(textLength < 0) {
        textLength = 0;
    }
    जर(begin < 0) {
        begin = 0;
    } अन्यथा जर(begin > textLength) {
        begin = textLength;
    }
    जर(end < begin) {
        end = begin;
    } अन्यथा जर(end > textLength) {
        end = textLength;
    }
    जर(pos < begin) {
        pos = begin;
    } अन्यथा जर(pos > end) {
        pos = end;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(स्थिरांक CharacterIterator &that) :
ForwardCharacterIterator(that),
textLength(that.textLength), pos(that.pos), begin(that.begin), end(that.end)
{
}

CharacterIterator&
CharacterIterator::चालक=(स्थिरांक CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::चालक=(that);
    textLength = that.textLength;
    pos = that.pos;
    begin = that.begin;
    end = that.end;
    परत *हे;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::firstPostInc() {
    setToStart();
    परत nextPostInc();
}

UChar32
CharacterIterator::first32PostInc() {
    setToStart();
    परत next32PostInc();
}

U_NAMESPACE_END
