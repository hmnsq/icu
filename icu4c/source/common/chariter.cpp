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
    ຖ້າ(textLength < 0) {
        textLength = end = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: textLength(length), pos(position), begin(0), end(length) {
    ຖ້າ(textLength < 0) {
        textLength = end = 0;
    }
    ຖ້າ(pos < 0) {
        pos = 0;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ(pos > end) {
        pos = end;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t textBegin, int32_t textEnd, int32_t position)
: textLength(length), pos(position), begin(textBegin), end(textEnd) {
    ຖ້າ(textLength < 0) {
        textLength = 0;
    }
    ຖ້າ(begin < 0) {
        begin = 0;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ(begin > textLength) {
        begin = textLength;
    }
    ຖ້າ(end < begin) {
        end = begin;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ(end > textLength) {
        end = textLength;
    }
    ຖ້າ(pos < begin) {
        pos = begin;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ(pos > end) {
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
    ສົ່ງຄືນ *this;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::firstPostInc() {
    setToStart();
    ສົ່ງຄືນ nextPostInc();
}

UChar32
CharacterIterator::first32PostInc() {
    setToStart();
    ສົ່ງຄືນ next32PostInc();
}

U_NAMESPACE_END
