// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2015-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  charstr.cpp
*/
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/putil.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"

#ଅନ୍ତର୍ଭାଵ "cstr.h"

#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "uinvchar.h"

U_NAMESPACE_BEGIN

CStr::CStr(ଅଚଳ UnicodeString &in) {
    UErrorCode status = U_ZERO_ERROR;
#ଯଦି !UCONFIG_NO_CONVERSION || U_CHARSET_IS_UTF8
    int32_t length = in.extract(0, in.length(), static_cast<ଅକ୍ଷର *>(nullptr), static_cast<uint32_t>(0));
    int32_t resultCapacity = 0;
    ଅକ୍ଷର *buf = s.getAppendBuffer(length, length, resultCapacity, status);
    ଯଦି (U_SUCCESS(status)) {
        in.extract(0, in.length(), buf, resultCapacity);
        s.append(buf, length, status);
    }
#ଅନ୍ୟଥା
    // No conversion available. Convert any invariant characters; substitute '?' for the rest.
    // Note: can't just call u_UCharsToChars() or CharString.appendInvariantChars() on the 
    //       whole string because they require that the entire input be invariant.
    ଅକ୍ଷର buf[2];
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<in.length(); i = in.moveIndex32(i, 1)) {
        ଯଦି (uprv_isInvariantUString(in.getBuffer()+i, 1)) {
            u_UCharsToChars(in.getBuffer()+i, buf, 1);
        } ଅନ୍ୟଥା {
            buf[0] = '?';
        }
        s.append(buf, 1, status);
    }
#endif
}

CStr::~CStr() {
}

ଅଚଳ ଅକ୍ଷର * CStr::operator ()() ଅଚଳ {
    ଫେରନ୍ତୁ s.data();
}

U_NAMESPACE_END
