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

Error_کد::~Error_کد() {}

UError_کد Error_کد::reset() {
    UError_کد کد = error_کد;
    error_کد = U_ZERO_ERROR;
    أرجع کد;
}

فراغ Error_کد::assertSuccess() ثابت {
    إذا(isFailure()) {
        handleFailure();
    }
}

ثابت حرف* Error_کد::errorName() ثابت {
  أرجع u_errorName(error_کد);
}

U_NAMESPACE_انتها
