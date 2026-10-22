// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2004, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ಅನ್ತರ್ಭಾವ "unicode/unifunct.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_ABSTRACT_RTTI_IMPLEMENTATION(UnicodeFunctor)

UnicodeFunctor::~UnicodeFunctor() {}

UnicodeMatcher* UnicodeFunctor::ಗಮ್ಯ_Matcher() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

UnicodeReplacer* UnicodeFunctor::ಗಮ್ಯ_Replacer() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_NAMESPACE_ಕೊನೆ

//eof
