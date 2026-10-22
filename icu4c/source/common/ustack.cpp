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

UStack::UStack(UError_کوډ &حالت) :
    UVector(حالت)
{
}

UStack::UStack(int32_t initial_ظرفیت, UError_کوډ &حالت) :
    UVector(initial_ظرفیت, حالت)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, UError_کوډ &حالت) :
    UVector(d, c, حالت)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_ظرفیت, UError_کوډ &حالت) :
    UVector(d, c, initial_ظرفیت, حالت)
{
}

UStack::~UStack() {}

فراغ* UStack::pop() {
    int32_t n = size() - 1;
    فراغ* پایله = تش_اشاره;
    إذا (n >= 0) {
        پایله = orphanElementAt(n);
    }
    أرجع پایله;
}

int32_t UStack::popi() {
    int32_t n = size() - 1;
    int32_t پایله = 0;
    إذا (n >= 0) {
        پایله = elementAti(n);
        removeElementAt(n);
    }
    أرجع پایله;
}

int32_t UStack::search(فراغ* obj) ثابت {
    int32_t i = شاخص_د(obj);
    أرجع (i >= 0) ? size() - i : i;
}

U_NAMESPACE_پای_ټکی
