// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2003-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#अन्तर्भाव "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStack)

UStack::UStack(UError_संहिता &स्थितिः) :
    UVector(स्थितिः)
{
}

UStack::UStack(int32_t initial_क्षमता, UError_संहिता &स्थितिः) :
    UVector(initial_क्षमता, स्थितिः)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, UError_संहिता &स्थितिः) :
    UVector(d, c, स्थितिः)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_क्षमता, UError_संहिता &स्थितिः) :
    UVector(d, c, initial_क्षमता, स्थितिः)
{
}

UStack::~UStack() {}

शून्य* UStack::pop() {
    int32_t n = size() - 1;
    शून्य* परिणाम = शून्यसूचकः;
    यदि (n >= 0) {
        परिणाम = orphanElementAt(n);
    }
    प्रत्यागम परिणाम;
}

int32_t UStack::popi() {
    int32_t n = size() - 1;
    int32_t परिणाम = 0;
    यदि (n >= 0) {
        परिणाम = elementAti(n);
        removeElementAt(n);
    }
    प्रत्यागम परिणाम;
}

int32_t UStack::search(शून्य* obj) ध्रुव {
    int32_t i = सूचकांक_सम्बन्धि(obj);
    प्रत्यागम (i >= 0) ? size() - i : i;
}

U_NAMESPACE_अन्तः
