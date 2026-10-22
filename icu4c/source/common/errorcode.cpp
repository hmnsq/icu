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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/errorcode.h"

U_NAMESPACE_BEGIN

ErrorCode::~ErrorCode() {}

UErrorCode ErrorCode::reset() {
    UErrorCode code = errorCode;
    errorCode = U_ZERO_ERROR;
    परत code;
}

शून्य ErrorCode::assertSuccess() स्थिरांक {
    जर(isFailure()) {
        handleFailure();
    }
}

स्थिरांक अक्षर* ErrorCode::errorName() स्थिरांक {
  परत u_errorName(errorCode);
}

U_NAMESPACE_END
