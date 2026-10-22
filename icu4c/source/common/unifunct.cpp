// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2004, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#شامل "unicode/unifunct.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_ABSTRACT_RTTI_IMPLEMENTATION(UnicodeFunctor)

UnicodeFunctor::~UnicodeFunctor() {}

UnicodeMatcher* UnicodeFunctor::مقصد_Matcher() ثابت U_LIFETIME_BOUND {
    واپس خالی_اشاریہ;
}

UnicodeReplacer* UnicodeFunctor::مقصد_Replacer() ثابت U_LIFETIME_BOUND {
    واپس خالی_اشاریہ;
}

U_NAMESPACE_انتہا

//eof
