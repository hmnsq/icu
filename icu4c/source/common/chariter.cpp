// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#समावेश "unicode/chariter.h"

U_NAMESPACE_BEGIN

ForwardCharacterIterator::~ForwardCharacterIterator() {}
ForwardCharacterIterator::ForwardCharacterIterator()
: UObject()
{}
ForwardCharacterIterator::ForwardCharacterIterator(स्थिर ForwardCharacterIterator &other)
: UObject(other)
{}


CharacterIterator::CharacterIterator()
: text_लंबाई(0), pos(0), begin(0), end(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: text_लंबाई(length), pos(0), begin(0), end(length) {
    अगर(text_लंबाई < 0) {
        text_लंबाई = end = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: text_लंबाई(length), pos(position), begin(0), end(length) {
    अगर(text_लंबाई < 0) {
        text_लंबाई = end = 0;
    }
    अगर(pos < 0) {
        pos = 0;
    } अन्यथा अगर(pos > end) {
        pos = end;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t textBegin, int32_t textEnd, int32_t position)
: text_लंबाई(length), pos(position), begin(textBegin), end(textEnd) {
    अगर(text_लंबाई < 0) {
        text_लंबाई = 0;
    }
    अगर(begin < 0) {
        begin = 0;
    } अन्यथा अगर(begin > text_लंबाई) {
        begin = text_लंबाई;
    }
    अगर(end < begin) {
        end = begin;
    } अन्यथा अगर(end > text_लंबाई) {
        end = text_लंबाई;
    }
    अगर(pos < begin) {
        pos = begin;
    } अन्यथा अगर(pos > end) {
        pos = end;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(स्थिर CharacterIterator &that) :
ForwardCharacterIterator(that),
text_लंबाई(that.text_लंबाई), pos(that.pos), begin(that.begin), end(that.end)
{
}

CharacterIterator&
CharacterIterator::चालक=(स्थिर CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::चालक=(that);
    text_लंबाई = that.text_लंबाई;
    pos = that.pos;
    begin = that.begin;
    end = that.end;
    वापस *यह;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::firstPostInc() {
    setToStart();
    वापस अग्र_Post_Inc();
}

UChar32
CharacterIterator::first32PostInc() {
    setToStart();
    वापस next32PostInc();
}

U_NAMESPACE_END
