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

UStack::UStack(UError_کوڈ &حالت) :
    UVector(حالت)
{
}

UStack::UStack(int32_t initial_سمرتھا, UError_کوڈ &حالت) :
    UVector(initial_سمرتھا, حالت)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, UError_کوڈ &حالت) :
    UVector(d, c, حالت)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_سمرتھا, UError_کوڈ &حالت) :
    UVector(d, c, initial_سمرتھا, حالت)
{
}

UStack::~UStack() {}

فراغ* UStack::pop() {
    int32_t n = size() - 1;
    فراغ* نتیجہ = خالی_سوچک;
    إذا (n >= 0) {
        نتیجہ = orphanElementAt(n);
    }
    أرجع نتیجہ;
}

int32_t UStack::popi() {
    int32_t n = size() - 1;
    int32_t نتیجہ = 0;
    إذا (n >= 0) {
        نتیجہ = elementAti(n);
        removeElementAt(n);
    }
    أرجع نتیجہ;
}

int32_t UStack::search(فراغ* obj) ثابت {
    int32_t i = اشاریہ_دا(obj);
    أرجع (i >= 0) ? size() - i : i;
}

U_NAMESPACE_سرا
