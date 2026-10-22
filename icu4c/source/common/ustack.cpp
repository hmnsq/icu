// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2003-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ଅନ୍ତର୍ଭାଵ "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStack)

UStack::UStack(UErrorCode &status) :
    UVector(status)
{
}

UStack::UStack(int32_t initialCapacity, UErrorCode &status) :
    UVector(initialCapacity, status)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, UErrorCode &status) :
    UVector(d, c, status)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initialCapacity, UErrorCode &status) :
    UVector(d, c, initialCapacity, status)
{
}

UStack::~UStack() {}

ଶୂନ୍ୟ* UStack::pop() {
    int32_t n = size() - 1;
    ଶୂନ୍ୟ* result = nullptr;
    ଯଦି (n >= 0) {
        result = orphanElementAt(n);
    }
    ଫେରନ୍ତୁ result;
}

int32_t UStack::popi() {
    int32_t n = size() - 1;
    int32_t result = 0;
    ଯଦି (n >= 0) {
        result = elementAti(n);
        removeElementAt(n);
    }
    ଫେରନ୍ତୁ result;
}

int32_t UStack::search(ଶୂନ୍ୟ* obj) ଅଚଳ {
    int32_t i = indexOf(obj);
    ଫେରନ୍ତୁ (i >= 0) ? size() - i : i;
}

U_NAMESPACE_END
