// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#অন্তর্ভাব "unicode/chariter.h"

U_NAMESPACE_BEGIN

ForwardCharacterIterator::~ForwardCharacterIterator() {}
ForwardCharacterIterator::ForwardCharacterIterator()
: UObject()
{}
ForwardCharacterIterator::ForwardCharacterIterator(ধ্রুবক ForwardCharacterIterator &other)
: UObject(other)
{}


CharacterIterator::CharacterIterator()
: text_দৈর্ঘ্য(0), pos(0), begin(0), end(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: text_দৈর্ঘ্য(length), pos(0), begin(0), end(length) {
    যদি(text_দৈর্ঘ্য < 0) {
        text_দৈর্ঘ্য = end = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: text_দৈর্ঘ্য(length), pos(position), begin(0), end(length) {
    যদি(text_দৈর্ঘ্য < 0) {
        text_দৈর্ঘ্য = end = 0;
    }
    যদি(pos < 0) {
        pos = 0;
    } অন্যথা যদি(pos > end) {
        pos = end;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t textBegin, int32_t textEnd, int32_t position)
: text_দৈর্ঘ্য(length), pos(position), begin(textBegin), end(textEnd) {
    যদি(text_দৈর্ঘ্য < 0) {
        text_দৈর্ঘ্য = 0;
    }
    যদি(begin < 0) {
        begin = 0;
    } অন্যথা যদি(begin > text_দৈর্ঘ্য) {
        begin = text_দৈর্ঘ্য;
    }
    যদি(end < begin) {
        end = begin;
    } অন্যথা যদি(end > text_দৈর্ঘ্য) {
        end = text_দৈর্ঘ্য;
    }
    যদি(pos < begin) {
        pos = begin;
    } অন্যথা যদি(pos > end) {
        pos = end;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(ধ্রুবক CharacterIterator &that) :
ForwardCharacterIterator(that),
text_দৈর্ঘ্য(that.text_দৈর্ঘ্য), pos(that.pos), begin(that.begin), end(that.end)
{
}

CharacterIterator&
CharacterIterator::operator=(ধ্রুবক CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::operator=(that);
    text_দৈর্ঘ্য = that.text_দৈর্ঘ্য;
    pos = that.pos;
    begin = that.begin;
    end = that.end;
    ফেরত *এই;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::firstPostInc() {
    setToStart();
    ফেরত পরবর্তী_Post_Inc();
}

UChar32
CharacterIterator::first32PostInc() {
    setToStart();
    ফেরত next32PostInc();
}

U_NAMESPACE_END
