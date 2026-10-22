// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2003-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#समावेश "uvector.h"

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

व्योम* UStack::pop() {
    int32_t n = size() - 1;
    व्योम* परिणाम = nullptr;
    अगर (n >= 0) {
        परिणाम = orphanElementAt(n);
    }
    वापस परिणाम;
}

int32_t UStack::popi() {
    int32_t n = size() - 1;
    int32_t परिणाम = 0;
    अगर (n >= 0) {
        परिणाम = elementAti(n);
        removeElementAt(n);
    }
    वापस परिणाम;
}

int32_t UStack::search(व्योम* obj) स्थिर {
    int32_t i = सूचकांक_Of(obj);
    वापस (i >= 0) ? size() - i : i;
}

U_NAMESPACE_END
