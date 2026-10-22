// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2003-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#অন্তর্ভাব "uvector.h"

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

শূন্য* UStack::pop() {
    int32_t n = size() - 1;
    শূন্য* ফলাফল = nullptr;
    যদি (n >= 0) {
        ফলাফল = orphanElementAt(n);
    }
    ফেরত ফলাফল;
}

int32_t UStack::popi() {
    int32_t n = size() - 1;
    int32_t ফলাফল = 0;
    যদি (n >= 0) {
        ফলাফল = elementAti(n);
        removeElementAt(n);
    }
    ফেরত ফলাফল;
}

int32_t UStack::search(শূন্য* obj) ধ্রুবক {
    int32_t i = সূচক_Of(obj);
    ফেরত (i >= 0) ? size() - i : i;
}

U_NAMESPACE_END
