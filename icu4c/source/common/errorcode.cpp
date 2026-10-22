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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/errorcode.h"

U_NAMESPACE_BEGIN

ErrorCode::~ErrorCode() {}

UErrorCode ErrorCode::reset() {
    UErrorCode code = errorCode;
    errorCode = U_ZERO_ERROR;
    ଫେରନ୍ତୁ code;
}

ଶୂନ୍ୟ ErrorCode::assertSuccess() ଅଚଳ {
    ଯଦି(isFailure()) {
        handleFailure();
    }
}

ଅଚଳ ଅକ୍ଷର* ErrorCode::errorName() ଅଚଳ {
  ଫେରନ୍ତୁ u_errorName(errorCode);
}

U_NAMESPACE_END
