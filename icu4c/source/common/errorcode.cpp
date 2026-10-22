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

Error_کوډ::~Error_کوډ() {}

UError_کوډ Error_کوډ::reset() {
    UError_کوډ کوډ = error_کوډ;
    error_کوډ = U_ZERO_ERROR;
    أرجع کوډ;
}

فراغ Error_کوډ::assertSuccess() ثابت {
    إذا(isFailure()) {
        handleFailure();
    }
}

ثابت حرف* Error_کوډ::errorName() ثابت {
  أرجع u_errorName(error_کوډ);
}

U_NAMESPACE_پای_ټکی
