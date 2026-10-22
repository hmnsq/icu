// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ଅନ୍ତର୍ଭାଵ "unicode/chariter.h"

U_NAMESPACE_BEGIN

ForwardCharacterIterator::~ForwardCharacterIterator() {}
ForwardCharacterIterator::ForwardCharacterIterator()
: UObject()
{}
ForwardCharacterIterator::ForwardCharacterIterator(ଅଚଳ ForwardCharacterIterator &other)
: UObject(other)
{}


CharacterIterator::CharacterIterator()
: textLength(0), pos(0), begin(0), end(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: textLength(length), pos(0), begin(0), end(length) {
    ଯଦି(textLength < 0) {
        textLength = end = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: textLength(length), pos(position), begin(0), end(length) {
    ଯଦି(textLength < 0) {
        textLength = end = 0;
    }
    ଯଦି(pos < 0) {
        pos = 0;
    } ଅନ୍ୟଥା ଯଦି(pos > end) {
        pos = end;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t textBegin, int32_t textEnd, int32_t position)
: textLength(length), pos(position), begin(textBegin), end(textEnd) {
    ଯଦି(textLength < 0) {
        textLength = 0;
    }
    ଯଦି(begin < 0) {
        begin = 0;
    } ଅନ୍ୟଥା ଯଦି(begin > textLength) {
        begin = textLength;
    }
    ଯଦି(end < begin) {
        end = begin;
    } ଅନ୍ୟଥା ଯଦି(end > textLength) {
        end = textLength;
    }
    ଯଦି(pos < begin) {
        pos = begin;
    } ଅନ୍ୟଥା ଯଦି(pos > end) {
        pos = end;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(ଅଚଳ CharacterIterator &that) :
ForwardCharacterIterator(that),
textLength(that.textLength), pos(that.pos), begin(that.begin), end(that.end)
{
}

CharacterIterator&
CharacterIterator::operator=(ଅଚଳ CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::operator=(that);
    textLength = that.textLength;
    pos = that.pos;
    begin = that.begin;
    end = that.end;
    ଫେରନ୍ତୁ *ଏହି;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::firstPostInc() {
    setToStart();
    ଫେରନ୍ତୁ nextPostInc();
}

UChar32
CharacterIterator::first32PostInc() {
    setToStart();
    ଫେରନ୍ତୁ next32PostInc();
}

U_NAMESPACE_END
