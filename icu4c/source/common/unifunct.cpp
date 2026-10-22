// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2004, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#അന്തര്ഭാവ "unicode/unifunct.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_ABSTRACT_RTTI_IMPLEMENTATION(UnicodeFunctor)

UnicodeFunctor::~UnicodeFunctor() {}

UnicodeMatcher* UnicodeFunctor::toMatcher() സ്ഥിരം U_LIFETIME_BOUND {
    തിരികെ nullptr;
}

UnicodeReplacer* UnicodeFunctor::toReplacer() സ്ഥിരം U_LIFETIME_BOUND {
    തിരികെ nullptr;
}

U_NAMESPACE_END

//eof
