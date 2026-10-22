// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2003-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#شامل "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStack)

UStack::UStack(UError_رمز &حالت_2) :
    UVector(حالت_2)
{
}

UStack::UStack(int32_t initial_استعداد, UError_رمز &حالت_2) :
    UVector(initial_استعداد, حالت_2)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, UError_رمز &حالت_2) :
    UVector(d, c, حالت_2)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_استعداد, UError_رمز &حالت_2) :
    UVector(d, c, initial_استعداد, حالت_2)
{
}

UStack::~UStack() {}

خالی* UStack::pop() {
    int32_t n = size() - 1;
    خالی* نتیجہ = خالی_اشاریہ;
    اگر (n >= 0) {
        نتیجہ = orphanElementAt(n);
    }
    واپس نتیجہ;
}

int32_t UStack::popi() {
    int32_t n = size() - 1;
    int32_t نتیجہ = 0;
    اگر (n >= 0) {
        نتیجہ = elementAti(n);
        removeElementAt(n);
    }
    واپس نتیجہ;
}

int32_t UStack::search(خالی* obj) ثابت {
    int32_t i = اشاریہ_کا(obj);
    واپس (i >= 0) ? size() - i : i;
}

U_NAMESPACE_انتہا
