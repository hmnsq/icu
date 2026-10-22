// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#அந்தர்பாவ "unicode/chariter.h"

U_NAMESPACE_BEGIN

ForwardCharacterIterator::~ForwardCharacterIterator() {}
ForwardCharacterIterator::ForwardCharacterIterator()
: UObject()
{}
ForwardCharacterIterator::ForwardCharacterIterator(மாறிலி ForwardCharacterIterator &other)
: UObject(other)
{}


CharacterIterator::CharacterIterator()
: textLength(0), pos(0), begin(0), end(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: textLength(length), pos(0), begin(0), end(length) {
    எனில்(textLength < 0) {
        textLength = end = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: textLength(length), pos(position), begin(0), end(length) {
    எனில்(textLength < 0) {
        textLength = end = 0;
    }
    எனில்(pos < 0) {
        pos = 0;
    } இல்லையெனில் எனில்(pos > end) {
        pos = end;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t textBegin, int32_t textEnd, int32_t position)
: textLength(length), pos(position), begin(textBegin), end(textEnd) {
    எனில்(textLength < 0) {
        textLength = 0;
    }
    எனில்(begin < 0) {
        begin = 0;
    } இல்லையெனில் எனில்(begin > textLength) {
        begin = textLength;
    }
    எனில்(end < begin) {
        end = begin;
    } இல்லையெனில் எனில்(end > textLength) {
        end = textLength;
    }
    எனில்(pos < begin) {
        pos = begin;
    } இல்லையெனில் எனில்(pos > end) {
        pos = end;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(மாறிலி CharacterIterator &that) :
ForwardCharacterIterator(that),
textLength(that.textLength), pos(that.pos), begin(that.begin), end(that.end)
{
}

CharacterIterator&
CharacterIterator::operator=(மாறிலி CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::operator=(that);
    textLength = that.textLength;
    pos = that.pos;
    begin = that.begin;
    end = that.end;
    திரும்பு *இது;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::firstPostInc() {
    setToStart();
    திரும்பு nextPostInc();
}

UChar32
CharacterIterator::first32PostInc() {
    setToStart();
    திரும்பு next32PostInc();
}

U_NAMESPACE_END
