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

Error_ڪوڊ::~Error_ڪوڊ() {}

UError_ڪوڊ Error_ڪوڊ::reset() {
    UError_ڪوڊ ڪوڊ = error_ڪوڊ;
    error_ڪوڊ = U_ZERO_ERROR;
    أرجع ڪوڊ;
}

فراغ Error_ڪوڊ::assertSuccess() ثابت {
    إذا(isFailure()) {
        handleFailure();
    }
}

ثابت حرف* Error_ڪوڊ::errorName() ثابت {
  أرجع u_errorName(error_ڪوڊ);
}

U_NAMESPACE_پڇاڙي
