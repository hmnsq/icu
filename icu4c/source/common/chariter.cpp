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
: textLength(0), pos(0), begin(0), end(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: textLength(length), pos(0), begin(0), end(length) {
    اگر(textLength < 0) {
        textLength = end = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: textLength(length), pos(position), begin(0), end(length) {
    اگر(textLength < 0) {
        textLength = end = 0;
    }
    اگر(pos < 0) {
        pos = 0;
    } وگرنه اگر(pos > end) {
        pos = end;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t textBegin, int32_t textEnd, int32_t position)
: textLength(length), pos(position), begin(textBegin), end(textEnd) {
    اگر(textLength < 0) {
        textLength = 0;
    }
    اگر(begin < 0) {
        begin = 0;
    } وگرنه اگر(begin > textLength) {
        begin = textLength;
    }
    اگر(end < begin) {
        end = begin;
    } وگرنه اگر(end > textLength) {
        end = textLength;
    }
    اگر(pos < begin) {
        pos = begin;
    } وگرنه اگر(pos > end) {
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
    بازگشت *این;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::firstPostInc() {
    setToStart();
    بازگشت nextPostInc();
}

UChar32
CharacterIterator::first32PostInc() {
    setToStart();
    بازگشت next32PostInc();
}

U_NAMESPACE_END
