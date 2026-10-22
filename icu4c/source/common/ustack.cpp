// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2003-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#అన్తర్భావ "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStack)

UStack::UStack(UError_సంకేతం &స్థితి_2) :
    UVector(స్థితి_2)
{
}

UStack::UStack(int32_t initial_సామర్థ్యం, UError_సంకేతం &స్థితి_2) :
    UVector(initial_సామర్థ్యం, స్థితి_2)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, UError_సంకేతం &స్థితి_2) :
    UVector(d, c, స్థితి_2)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_సామర్థ్యం, UError_సంకేతం &స్థితి_2) :
    UVector(d, c, initial_సామర్థ్యం, స్థితి_2)
{
}

UStack::~UStack() {}

శూన్య* UStack::pop() {
    int32_t n = size() - 1;
    శూన్య* ఫలితం = శూన్యసూచిక;
    ఒకవేళ (n >= 0) {
        ఫలితం = orphanElementAt(n);
    }
    తిరిగి ఫలితం;
}

int32_t UStack::popi() {
    int32_t n = size() - 1;
    int32_t ఫలితం = 0;
    ఒకవేళ (n >= 0) {
        ఫలితం = elementAti(n);
        removeElementAt(n);
    }
    తిరిగి ఫలితం;
}

int32_t UStack::search(శూన్య* obj) స్థిరరాశి {
    int32_t i = సూచీ_యొక్క(obj);
    తిరిగి (i >= 0) ? size() - i : i;
}

U_NAMESPACE_చివర
