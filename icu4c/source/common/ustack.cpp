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

UStack::UStack(UError_ڪوڊ &حالت) :
    UVector(حالت)
{
}

UStack::UStack(int32_t initial_صلاحيت, UError_ڪوڊ &حالت) :
    UVector(initial_صلاحيت, حالت)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, UError_ڪوڊ &حالت) :
    UVector(d, c, حالت)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_صلاحيت, UError_ڪوڊ &حالت) :
    UVector(d, c, initial_صلاحيت, حالت)
{
}

UStack::~UStack() {}

فراغ* UStack::pop() {
    int32_t n = size() - 1;
    فراغ* نتيجو = خالي_اشارو;
    إذا (n >= 0) {
        نتيجو = orphanElementAt(n);
    }
    أرجع نتيجو;
}

int32_t UStack::popi() {
    int32_t n = size() - 1;
    int32_t نتيجو = 0;
    إذا (n >= 0) {
        نتيجو = elementAti(n);
        removeElementAt(n);
    }
    أرجع نتيجو;
}

int32_t UStack::search(فراغ* obj) ثابت {
    int32_t i = اشاريو_جو(obj);
    أرجع (i >= 0) ? size() - i : i;
}

U_NAMESPACE_پڇاڙي
