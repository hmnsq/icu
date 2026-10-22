// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2004, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#అన్తర్భావ "unicode/unifunct.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_ABSTRACT_RTTI_IMPLEMENTATION(UnicodeFunctor)

UnicodeFunctor::~UnicodeFunctor() {}

UnicodeMatcher* UnicodeFunctor::గమ్యం_Matcher() స్థిరరాశి U_LIFETIME_BOUND {
    తిరిగి శూన్యసూచిక;
}

UnicodeReplacer* UnicodeFunctor::గమ్యం_Replacer() స్థిరరాశి U_LIFETIME_BOUND {
    తిరిగి శూన్యసూచిక;
}

U_NAMESPACE_చివర

//eof
