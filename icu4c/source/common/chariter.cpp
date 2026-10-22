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
ForwardCharacterIterator::ForwardCharacterIterator(ध्रुव ForwardCharacterIterator &other)
: UObject(other)
{}


CharacterIterator::CharacterIterator()
: text_दीर्घता(0), pos(0), begin(0), अन्तः(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: text_दीर्घता(length), pos(0), begin(0), अन्तः(length) {
    यदि(text_दीर्घता < 0) {
        text_दीर्घता = अन्तः = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: text_दीर्घता(length), pos(position), begin(0), अन्तः(length) {
    यदि(text_दीर्घता < 0) {
        text_दीर्घता = अन्तः = 0;
    }
    यदि(pos < 0) {
        pos = 0;
    } अन्यथा यदि(pos > अन्तः) {
        pos = अन्तः;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t textBegin, int32_t text_अन्तः, int32_t position)
: text_दीर्घता(length), pos(position), begin(textBegin), अन्तः(text_अन्तः) {
    यदि(text_दीर्घता < 0) {
        text_दीर्घता = 0;
    }
    यदि(begin < 0) {
        begin = 0;
    } अन्यथा यदि(begin > text_दीर्घता) {
        begin = text_दीर्घता;
    }
    यदि(अन्तः < begin) {
        अन्तः = begin;
    } अन्यथा यदि(अन्तः > text_दीर्घता) {
        अन्तः = text_दीर्घता;
    }
    यदि(pos < begin) {
        pos = begin;
    } अन्यथा यदि(pos > अन्तः) {
        pos = अन्तः;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(ध्रुव CharacterIterator &that) :
ForwardCharacterIterator(that),
text_दीर्घता(that.text_दीर्घता), pos(that.pos), begin(that.begin), अन्तः(that.अन्तः)
{
}

CharacterIterator&
CharacterIterator::संक्रियकः=(ध्रुव CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::संक्रियकः=(that);
    text_दीर्घता = that.text_दीर्घता;
    pos = that.pos;
    begin = that.begin;
    अन्तः = that.अन्तः;
    प्रत्यागम *एतत्;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::firstPostInc() {
    समुच्चयः_गन्तव्यम्_Start();
    प्रत्यागम अग्र_Post_Inc();
}

UChar32
CharacterIterator::first32PostInc() {
    समुच्चयः_गन्तव्यम्_Start();
    प्रत्यागम next32PostInc();
}

U_NAMESPACE_अन्तः
