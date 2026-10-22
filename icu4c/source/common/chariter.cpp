// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#അന്തര്ഭാവ "unicode/chariter.h"

U_NAMESPACE_BEGIN

ForwardCharacterIterator::~ForwardCharacterIterator() {}
ForwardCharacterIterator::ForwardCharacterIterator()
: UObject()
{}
ForwardCharacterIterator::ForwardCharacterIterator(സ്ഥിരം ForwardCharacterIterator &other)
: UObject(other)
{}


CharacterIterator::CharacterIterator()
: textLength(0), pos(0), begin(0), end(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: textLength(length), pos(0), begin(0), end(length) {
    എങ്കിൽ(textLength < 0) {
        textLength = end = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: textLength(length), pos(position), begin(0), end(length) {
    എങ്കിൽ(textLength < 0) {
        textLength = end = 0;
    }
    എങ്കിൽ(pos < 0) {
        pos = 0;
    } അല്ലെങ്കിൽ എങ്കിൽ(pos > end) {
        pos = end;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t textBegin, int32_t textEnd, int32_t position)
: textLength(length), pos(position), begin(textBegin), end(textEnd) {
    എങ്കിൽ(textLength < 0) {
        textLength = 0;
    }
    എങ്കിൽ(begin < 0) {
        begin = 0;
    } അല്ലെങ്കിൽ എങ്കിൽ(begin > textLength) {
        begin = textLength;
    }
    എങ്കിൽ(end < begin) {
        end = begin;
    } അല്ലെങ്കിൽ എങ്കിൽ(end > textLength) {
        end = textLength;
    }
    എങ്കിൽ(pos < begin) {
        pos = begin;
    } അല്ലെങ്കിൽ എങ്കിൽ(pos > end) {
        pos = end;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(സ്ഥിരം CharacterIterator &that) :
ForwardCharacterIterator(that),
textLength(that.textLength), pos(that.pos), begin(that.begin), end(that.end)
{
}

CharacterIterator&
CharacterIterator::operator=(സ്ഥിരം CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::operator=(that);
    textLength = that.textLength;
    pos = that.pos;
    begin = that.begin;
    end = that.end;
    തിരികെ *ഇത്;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::firstPostInc() {
    setToStart();
    തിരികെ nextPostInc();
}

UChar32
CharacterIterator::first32PostInc() {
    setToStart();
    തിരികെ next32PostInc();
}

U_NAMESPACE_END
