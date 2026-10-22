// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2003-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStack)

UStack::UStack(UError_ਕੋਡ &ਸਥਿਤੀ) :
    UVector(ਸਥਿਤੀ)
{
}

UStack::UStack(int32_t initial_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ) :
    UVector(initial_ਸਮਰੱਥਾ, ਸਥਿਤੀ)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, UError_ਕੋਡ &ਸਥਿਤੀ) :
    UVector(d, c, ਸਥਿਤੀ)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ) :
    UVector(d, c, initial_ਸਮਰੱਥਾ, ਸਥਿਤੀ)
{
}

UStack::~UStack() {}

ਖਾਲੀ* UStack::pop() {
    int32_t n = size() - 1;
    ਖਾਲੀ* ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (n >= 0) {
        ਨਤੀਜਾ = orphanElementAt(n);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

int32_t UStack::popi() {
    int32_t n = size() - 1;
    int32_t ਨਤੀਜਾ = 0;
    ਜੇ (n >= 0) {
        ਨਤੀਜਾ = elementAti(n);
        removeElementAt(n);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

int32_t UStack::search(ਖਾਲੀ* obj) ਅਚਲ {
    int32_t i = ਸੂਚਕਾਂਕ_ਦਾ(obj);
    ਵਾਪਸ (i >= 0) ? size() - i : i;
}

U_NAMESPACE_ਸਿਰਾ
