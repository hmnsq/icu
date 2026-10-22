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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/errorcode.h"

U_NAMESPACE_BEGIN

ErrorCode::~ErrorCode() {}

UErrorCode ErrorCode::reset() {
    UErrorCode code = errorCode;
    errorCode = U_ZERO_ERROR;
    પાછા code;
}

શૂન્ય ErrorCode::assertSuccess() અચલ {
    જો(isFailure()) {
        handleFailure();
    }
}

અચલ અક્ષર* ErrorCode::errorName() અચલ {
  પાછા u_errorName(errorCode);
}

U_NAMESPACE_END
