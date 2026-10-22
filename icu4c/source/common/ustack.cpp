// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2003-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ಅನ್ತರ್ಭಾವ "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStack)

UStack::UStack(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
    UVector(ಸ್ಥಿತಿ_2)
{
}

UStack::UStack(int32_t initial_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
    UVector(initial_ಸಾಮರ್ಥ್ಯ, ಸ್ಥಿತಿ_2)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
    UVector(d, c, ಸ್ಥಿತಿ_2)
{
}

UStack::UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
    UVector(d, c, initial_ಸಾಮರ್ಥ್ಯ, ಸ್ಥಿತಿ_2)
{
}

UStack::~UStack() {}

ಶೂನ್ಯ* UStack::pop() {
    int32_t n = size() - 1;
    ಶೂನ್ಯ* ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (n >= 0) {
        ಫಲಿತಾಂಶ = orphanElementAt(n);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

int32_t UStack::popi() {
    int32_t n = size() - 1;
    int32_t ಫಲಿತಾಂಶ = 0;
    ಆದರೆ (n >= 0) {
        ಫಲಿತಾಂಶ = elementAti(n);
        removeElementAt(n);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

int32_t UStack::search(ಶೂನ್ಯ* obj) ಸ್ಥಿರಾಂಕ {
    int32_t i = ಸೂಚ್ಯಂಕ_ದ(obj);
    ಹಿಂತಿರುಗಿ (i >= 0) ? size() - i : i;
}

U_NAMESPACE_ಕೊನೆ
