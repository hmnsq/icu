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

#شامل "unicode/utypes.h"
#شامل "unicode/errorcode.h"

U_NAMESPACE_BEGIN

Error_رمز::~Error_رمز() {}

UError_رمز Error_رمز::reset() {
    UError_رمز رمز = error_رمز;
    error_رمز = U_ZERO_ERROR;
    واپس رمز;
}

خالی Error_رمز::assertSuccess() ثابت {
    اگر(isFailure()) {
        handleFailure();
    }
}

ثابت حرف* Error_رمز::errorName() ثابت {
  واپس u_errorName(error_رمز);
}

U_NAMESPACE_انتہا
