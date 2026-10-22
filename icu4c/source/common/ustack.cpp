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

UStack::UStack(UError_کد &وضعیت) :
    UVector(وضعیت)
{
}

UStack::UStack(int32_t initial_ظرفیت, UError_کد &وضعیت) :
    UVector(initial_ظرفیت, وضعیت)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, UError_کد &وضعیت) :
    UVector(d, c, وضعیت)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_ظرفیت, UError_کد &وضعیت) :
    UVector(d, c, initial_ظرفیت, وضعیت)
{
}

UStack::~UStack() {}

فراغ* UStack::pop() {
    int32_t n = size() - 1;
    فراغ* نتیجه = اشاره_تهی;
    إذا (n >= 0) {
        نتیجه = orphanElementAt(n);
    }
    أرجع نتیجه;
}

int32_t UStack::popi() {
    int32_t n = size() - 1;
    int32_t نتیجه = 0;
    إذا (n >= 0) {
        نتیجه = elementAti(n);
        removeElementAt(n);
    }
    أرجع نتیجه;
}

int32_t UStack::search(فراغ* obj) ثابت {
    int32_t i = شاخص_از(obj);
    أرجع (i >= 0) ? size() - i : i;
}

U_NAMESPACE_انتها
