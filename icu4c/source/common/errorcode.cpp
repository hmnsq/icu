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

Error_संहिता::~Error_संहिता() {}

UError_संहिता Error_संहिता::reset() {
    UError_संहिता संहिता = error_संहिता;
    error_संहिता = U_ZERO_ERROR;
    प्रत्यागम संहिता;
}

शून्य Error_संहिता::assertSuccess() ध्रुव {
    यदि(isFailure()) {
        handleFailure();
    }
}

ध्रुव वर्ण* Error_संहिता::errorName() ध्रुव {
  प्रत्यागम u_errorName(error_संहिता);
}

U_NAMESPACE_अन्तः
