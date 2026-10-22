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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/errorcode.h"

U_NAMESPACE_BEGIN

Error_సంకేతం::~Error_సంకేతం() {}

UError_సంకేతం Error_సంకేతం::reset() {
    UError_సంకేతం సంకేతం_2 = error_సంకేతం;
    error_సంకేతం = U_ZERO_ERROR;
    తిరిగి సంకేతం_2;
}

శూన్య Error_సంకేతం::assertSuccess() స్థిరరాశి {
    ఒకవేళ(isFailure()) {
        handleFailure();
    }
}

స్థిరరాశి అక్షరం* Error_సంకేతం::errorName() స్థిరరాశి {
  తిరిగి u_errorName(error_సంకేతం);
}

U_NAMESPACE_చివర
