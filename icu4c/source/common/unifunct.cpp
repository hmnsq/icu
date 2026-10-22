// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2004, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unifunct.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_ABSTRACT_RTTI_IMPLEMENTATION(UnicodeFunctor)

UnicodeFunctor::~UnicodeFunctor() {}

UnicodeMatcher* UnicodeFunctor::ਮੰਜ਼ਿਲ_Matcher() ਅਚਲ U_LIFETIME_BOUND {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

UnicodeReplacer* UnicodeFunctor::ਮੰਜ਼ਿਲ_Replacer() ਅਚਲ U_LIFETIME_BOUND {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_NAMESPACE_ਸਿਰਾ

//eof
