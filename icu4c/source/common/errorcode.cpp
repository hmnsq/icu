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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/errorcode.h"

U_NAMESPACE_BEGIN

ErrorCode::~ErrorCode() {}

UErrorCode ErrorCode::reset() {
    UErrorCode code = errorCode;
    errorCode = U_ZERO_ERROR;
    തിരികെ code;
}

ശൂന്യം ErrorCode::assertSuccess() സ്ഥിരം {
    എങ്കിൽ(isFailure()) {
        handleFailure();
    }
}

സ്ഥിരം അക്ഷരം* ErrorCode::errorName() സ്ഥിരം {
  തിരികെ u_errorName(errorCode);
}

U_NAMESPACE_END
