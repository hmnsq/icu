// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2009-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  errorcode.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2009mar10
*   created by: Markus W. Scherer
*/

#ضمن "unicode/utypes.h"
#ضمن "unicode/errorcode.h"

U_NAMESPACE_BEGIN

Error_کوڈ::~Error_کوڈ() {}

UError_کوڈ Error_کوڈ::reset() {
    UError_کوڈ کوڈ = error_کوڈ;
    error_کوڈ = U_ZERO_ERROR;
    أرجع کوڈ;
}

فراغ Error_کوڈ::assertSuccess() ثابت {
    إذا(isFailure()) {
        handleFailure();
    }
}

ثابت حرف* Error_کوڈ::errorName() ثابت {
  أرجع u_errorName(error_کوڈ);
}

U_NAMESPACE_سرا
