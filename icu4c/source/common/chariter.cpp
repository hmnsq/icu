// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ಅನ್ತರ್ಭಾವ "unicode/chariter.h"

U_NAMESPACE_BEGIN

ForwardCharacterIterator::~ForwardCharacterIterator() {}
ForwardCharacterIterator::ForwardCharacterIterator()
: UObject()
{}
ForwardCharacterIterator::ForwardCharacterIterator(ಧ್ರುವ ForwardCharacterIterator &other)
: UObject(other)
{}


CharacterIterator::CharacterIterator()
: textLength(0), pos(0), begin(0), end(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: textLength(length), pos(0), begin(0), end(length) {
    ಯದಿ(textLength < 0) {
        textLength = end = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: textLength(length), pos(position), begin(0), end(length) {
    ಯದಿ(textLength < 0) {
        textLength = end = 0;
    }
    ಯದಿ(pos < 0) {
        pos = 0;
    } ಅನ್ಯಥಾ ಯದಿ(pos > end) {
        pos = end;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t textBegin, int32_t textEnd, int32_t position)
: textLength(length), pos(position), begin(textBegin), end(textEnd) {
    ಯದಿ(textLength < 0) {
        textLength = 0;
    }
    ಯದಿ(begin < 0) {
        begin = 0;
    } ಅನ್ಯಥಾ ಯದಿ(begin > textLength) {
        begin = textLength;
    }
    ಯದಿ(end < begin) {
        end = begin;
    } ಅನ್ಯಥಾ ಯದಿ(end > textLength) {
        end = textLength;
    }
    ಯದಿ(pos < begin) {
        pos = begin;
    } ಅನ್ಯಥಾ ಯದಿ(pos > end) {
        pos = end;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(ಧ್ರುವ CharacterIterator &that) :
ForwardCharacterIterator(that),
textLength(that.textLength), pos(that.pos), begin(that.begin), end(that.end)
{
}

CharacterIterator&
CharacterIterator::operator=(ಧ್ರುವ CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::operator=(that);
    textLength = that.textLength;
    pos = that.pos;
    begin = that.begin;
    end = that.end;
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::firstPostInc() {
    setToStart();
    ಪ್ರತ್ಯಾಗಮ nextPostInc();
}

UChar32
CharacterIterator::first32PostInc() {
    setToStart();
    ಪ್ರತ್ಯಾಗಮ next32PostInc();
}

U_NAMESPACE_END
