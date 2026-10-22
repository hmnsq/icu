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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/errorcode.h"

U_NAMESPACE_BEGIN

Error_ਕੋਡ::~Error_ਕੋਡ() {}

UError_ਕੋਡ Error_ਕੋਡ::reset() {
    UError_ਕੋਡ ਕੋਡ = error_ਕੋਡ;
    error_ਕੋਡ = U_ZERO_ERROR;
    ਵਾਪਸ ਕੋਡ;
}

ਖਾਲੀ Error_ਕੋਡ::assertSuccess() ਅਚਲ {
    ਜੇ(isFailure()) {
        handleFailure();
    }
}

ਅਚਲ ਅੱਖਰ* Error_ਕੋਡ::errorName() ਅਚਲ {
  ਵਾਪਸ u_errorName(error_ਕੋਡ);
}

U_NAMESPACE_ਸਿਰਾ
