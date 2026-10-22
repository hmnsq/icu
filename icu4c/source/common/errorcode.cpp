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

Error_ಸಂಕೇತ::~Error_ಸಂಕೇತ() {}

UError_ಸಂಕೇತ Error_ಸಂಕೇತ::reset() {
    UError_ಸಂಕೇತ ಸಂಕೇತ_2 = error_ಸಂಕೇತ;
    error_ಸಂಕೇತ = U_ZERO_ERROR;
    ಹಿಂತಿರುಗಿ ಸಂಕೇತ_2;
}

ಶೂನ್ಯ Error_ಸಂಕೇತ::assertSuccess() ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(isFailure()) {
        handleFailure();
    }
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* Error_ಸಂಕೇತ::errorName() ಸ್ಥಿರಾಂಕ {
  ಹಿಂತಿರುಗಿ u_errorName(error_ಸಂಕೇತ);
}

U_NAMESPACE_ಕೊನೆ
