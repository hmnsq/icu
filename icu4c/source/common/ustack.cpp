// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2003-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#அந்தர்பாவ "uvector.h"

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

வெறுமை* UStack::pop() {
    int32_t n = size() - 1;
    வெறுமை* result = nullptr;
    எனில் (n >= 0) {
        result = orphanElementAt(n);
    }
    திரும்பு result;
}

int32_t UStack::popi() {
    int32_t n = size() - 1;
    int32_t result = 0;
    எனில் (n >= 0) {
        result = elementAti(n);
        removeElementAt(n);
    }
    திரும்பு result;
}

int32_t UStack::search(வெறுமை* obj) மாறிலி {
    int32_t i = indexOf(obj);
    திரும்பு (i >= 0) ? size() - i : i;
}

U_NAMESPACE_END
