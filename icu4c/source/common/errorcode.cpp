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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/errorcode.h"

U_NAMESPACE_BEGIN

ErrorCode::~ErrorCode() {}

UErrorCode ErrorCode::reset() {
    UErrorCode code = errorCode;
    errorCode = U_ZERO_ERROR;
    ಪ್ರತ್ಯಾಗಮ code;
}

ಶೂನ್ಯ ErrorCode::assertSuccess() ಧ್ರುವ {
    ಯದಿ(isFailure()) {
        handleFailure();
    }
}

ಧ್ರುವ ವರ್ಣ* ErrorCode::errorName() ಧ್ರುವ {
  ಪ್ರತ್ಯಾಗಮ u_errorName(errorCode);
}

U_NAMESPACE_END
