// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#include "unicode/chariter.h"

U_NAMESPACE_BEGIN

ForwardCharacterIterator::~ForwardCharacterIterator() {}
ForwardCharacterIterator::ForwardCharacterIterator()
: UObject()
{}
ForwardCharacterIterator::ForwardCharacterIterator(const ForwardCharacterIterator &other)
: UObject(other)
{}


CharacterIterator::CharacterIterator()
: textLength(0), pos(0), begin(0), end(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: textLength(length), pos(0), begin(0), end(length) {
    ᬬᬾᬦ᭄(textLength < 0) {
        textLength = end = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: textLength(length), pos(position), begin(0), end(length) {
    ᬬᬾᬦ᭄(textLength < 0) {
        textLength = end = 0;
    }
    ᬬᬾᬦ᭄(pos < 0) {
        pos = 0;
    } ᬮᬶᬬ ᬬᬾᬦ᭄(pos > end) {
        pos = end;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t textBegin, int32_t textEnd, int32_t position)
: textLength(length), pos(position), begin(textBegin), end(textEnd) {
    ᬬᬾᬦ᭄(textLength < 0) {
        textLength = 0;
    }
    ᬬᬾᬦ᭄(begin < 0) {
        begin = 0;
    } ᬮᬶᬬ ᬬᬾᬦ᭄(begin > textLength) {
        begin = textLength;
    }
    ᬬᬾᬦ᭄(end < begin) {
        end = begin;
    } ᬮᬶᬬ ᬬᬾᬦ᭄(end > textLength) {
        end = textLength;
    }
    ᬬᬾᬦ᭄(pos < begin) {
        pos = begin;
    } ᬮᬶᬬ ᬬᬾᬦ᭄(pos > end) {
        pos = end;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(const CharacterIterator &that) :
ForwardCharacterIterator(that),
textLength(that.textLength), pos(that.pos), begin(that.begin), end(that.end)
{
}

CharacterIterator&
CharacterIterator::operator=(const CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::operator=(that);
    textLength = that.textLength;
    pos = that.pos;
    begin = that.begin;
    end = that.end;
    ᬯᬮᬶ *this;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::firstPostInc() {
    setToStart();
    ᬯᬮᬶ nextPostInc();
}

UChar32
CharacterIterator::first32PostInc() {
    setToStart();
    ᬯᬮᬶ next32PostInc();
}

U_NAMESPACE_END
