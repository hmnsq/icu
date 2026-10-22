// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2004, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#অন্তর্ভাব "unicode/unifunct.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_ABSTRACT_RTTI_IMPLEMENTATION(UnicodeFunctor)

UnicodeFunctor::~UnicodeFunctor() {}

UnicodeMatcher* UnicodeFunctor::toMatcher() ধ্রুব U_LIFETIME_BOUND {
    প্রত্যাগম nullptr;
}

UnicodeReplacer* UnicodeFunctor::toReplacer() ধ্রুব U_LIFETIME_BOUND {
    প্রত্যাগম nullptr;
}

U_NAMESPACE_END

//eof
