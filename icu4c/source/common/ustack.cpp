// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2003-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ضمن "uvector.h"

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

فراغ* UStack::pop() {
    int32_t n = size() - 1;
    فراغ* result = nullptr;
    𐤀𐤌 (n >= 0) {
        result = orphanElementAt(n);
    }
    أرجع result;
}

int32_t UStack::popi() {
    int32_t n = size() - 1;
    int32_t result = 0;
    𐤀𐤌 (n >= 0) {
        result = elementAti(n);
        removeElementAt(n);
    }
    أرجع result;
}

int32_t UStack::search(فراغ* obj) ثابت {
    int32_t i = indexOf(obj);
    أرجع (i >= 0) ? size() - i : i;
}

U_NAMESPACE_END
