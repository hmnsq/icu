// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2015-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  charstr.cpp
*/
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"

#ಅನ್ತರ್ಭಾವ "cstr.h"

#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "uinvchar.h"

U_NAMESPACE_BEGIN

CStr::CStr(ಧ್ರುವ UnicodeString &in) {
    UErrorCode status = U_ZERO_ERROR;
#ಯದಿ !UCONFIG_NO_CONVERSION || U_CHARSET_IS_UTF8
    int32_t length = in.extract(0, in.length(), static_cast<ವರ್ಣ *>(nullptr), static_cast<uint32_t>(0));
    int32_t resultCapacity = 0;
    ವರ್ಣ *buf = s.getAppendBuffer(length, length, resultCapacity, status);
    ಯದಿ (U_SUCCESS(status)) {
        in.extract(0, in.length(), buf, resultCapacity);
        s.append(buf, length, status);
    }
#ಅನ್ಯಥಾ
    // No conversion available. Convert any invariant characters; substitute '?' for the rest.
    // Note: can't just call u_UCharsToChars() or CharString.appendInvariantChars() on the 
    //       whole string because they require that the entire input be invariant.
    ವರ್ಣ buf[2];
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i=0; i<in.length(); i = in.moveIndex32(i, 1)) {
        ಯದಿ (uprv_isInvariantUString(in.getBuffer()+i, 1)) {
            u_UCharsToChars(in.getBuffer()+i, buf, 1);
        } ಅನ್ಯಥಾ {
            buf[0] = '?';
        }
        s.append(buf, 1, status);
    }
#endif
}

CStr::~CStr() {
}

ಧ್ರುವ ವರ್ಣ * CStr::operator ()() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ s.data();
}

U_NAMESPACE_END
