// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#అన్తర్భావ "unicode/chariter.h"

U_NAMESPACE_BEGIN

ForwardCharacterIterator::~ForwardCharacterIterator() {}
ForwardCharacterIterator::ForwardCharacterIterator()
: UObject()
{}
ForwardCharacterIterator::ForwardCharacterIterator(స్థిరరాశి ForwardCharacterIterator &other)
: UObject(other)
{}


CharacterIterator::CharacterIterator()
: పాఠ్యం_నిడివి(0), pos(0), begin(0), చివర(0) {
}

CharacterIterator::CharacterIterator(int32_t length)
: పాఠ్యం_నిడివి(length), pos(0), begin(0), చివర(length) {
    ఒకవేళ(పాఠ్యం_నిడివి < 0) {
        పాఠ్యం_నిడివి = చివర = 0;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t position)
: పాఠ్యం_నిడివి(length), pos(position), begin(0), చివర(length) {
    ఒకవేళ(పాఠ్యం_నిడివి < 0) {
        పాఠ్యం_నిడివి = చివర = 0;
    }
    ఒకవేళ(pos < 0) {
        pos = 0;
    } లేకపోతే ఒకవేళ(pos > చివర) {
        pos = చివర;
    }
}

CharacterIterator::CharacterIterator(int32_t length, int32_t పాఠ్యం_Begin, int32_t పాఠ్యం_చివర, int32_t position)
: పాఠ్యం_నిడివి(length), pos(position), begin(పాఠ్యం_Begin), చివర(పాఠ్యం_చివర) {
    ఒకవేళ(పాఠ్యం_నిడివి < 0) {
        పాఠ్యం_నిడివి = 0;
    }
    ఒకవేళ(begin < 0) {
        begin = 0;
    } లేకపోతే ఒకవేళ(begin > పాఠ్యం_నిడివి) {
        begin = పాఠ్యం_నిడివి;
    }
    ఒకవేళ(చివర < begin) {
        చివర = begin;
    } లేకపోతే ఒకవేళ(చివర > పాఠ్యం_నిడివి) {
        చివర = పాఠ్యం_నిడివి;
    }
    ఒకవేళ(pos < begin) {
        pos = begin;
    } లేకపోతే ఒకవేళ(pos > చివర) {
        pos = చివర;
    }
}

CharacterIterator::~CharacterIterator() {}

CharacterIterator::CharacterIterator(స్థిరరాశి CharacterIterator &that) :
ForwardCharacterIterator(that),
పాఠ్యం_నిడివి(that.పాఠ్యం_నిడివి), pos(that.pos), begin(that.begin), చివర(that.చివర)
{
}

CharacterIterator&
CharacterIterator::క్రియాకారకం=(స్థిరరాశి CharacterIterator& that) U_LIFETIME_BOUND {
    ForwardCharacterIterator::క్రియాకారకం=(that);
    పాఠ్యం_నిడివి = that.పాఠ్యం_నిడివి;
    pos = that.pos;
    begin = that.begin;
    చివర = that.చివర;
    తిరిగి *ఇది;
}

// implementing first[32]PostInc() directly in a subclass should be faster
// but these implementations make subclassing a little easier
char16_t
CharacterIterator::మొదటి_Post_Inc() {
    సమితి_గమ్యం_ప్రారంభం();
    తిరిగి తదుపరి_Post_Inc();
}

UChar32
CharacterIterator::first32PostInc() {
    సమితి_గమ్యం_ప్రారంభం();
    తిరిగి next32PostInc();
}

U_NAMESPACE_చివర
