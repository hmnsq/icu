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
ForwardCharacterIterator::ForwardCharacterIterator(अचर ForwardCharacterIterator &other)
: UObject(other)
{}


CharacterIterator::CharacterIterator()
: textLength(0), pos(0), begin(0), end(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: textLength(length), pos(0), begin(0), end(length) {
    यदि(textLength < 0) {
        textLength = end = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: textLength(length), pos(position), begin(0), end(length) {
    यदि(textLength < 0) {
        textLength = end = 0;
    }
    यदि(pos < 0) {
        pos = 0;
    } अन्यथा यदि(pos > end) {
        pos = end;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t textBegin, int32_t textEnd, int32_t position)
: textLength(length), pos(position), begin(textBegin), end(textEnd) {
    यदि(textLength < 0) {
        textLength = 0;
    }
    यदि(begin < 0) {
        begin = 0;
    } अन्यथा यदि(begin > textLength) {
        begin = textLength;
    }
    यदि(end < begin) {
        end = begin;
    } अन्यथा यदि(end > textLength) {
        end = textLength;
    }
    यदि(pos < begin) {
        pos = begin;
    } अन्यथा यदि(pos > end) {
        pos = end;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(अचर CharacterIterator &that) :
ForwardCharacterIterator(that),
textLength(that.textLength), pos(that.pos), begin(that.begin), end(that.end)
{
}

CharacterIterator&
CharacterIterator::चालक=(अचर CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::चालक=(that);
    textLength = that.textLength;
    pos = that.pos;
    begin = that.begin;
    end = that.end;
    फर्काऊ *एतत्;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::firstPostInc() {
    setToStart();
    फर्काऊ nextPostInc();
}

UChar32
CharacterIterator::first32PostInc() {
    setToStart();
    फर्काऊ next32PostInc();
}

U_NAMESPACE_END
