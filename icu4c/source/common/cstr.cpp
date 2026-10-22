// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2015-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  charstr.cpp
*/
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "unicode/unistr.h"

#அந்தர்பாவ "cstr.h"

#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "uinvchar.h"

U_NAMESPACE_BEGIN

CStr::CStr(மாறிலி UnicodeString &in) {
    UErrorCode status = U_ZERO_ERROR;
#எனில் !UCONFIG_NO_CONVERSION || U_CHARSET_IS_UTF8
    int32_t length = in.extract(0, in.length(), static_cast<எழுத்து *>(nullptr), static_cast<uint32_t>(0));
    int32_t resultCapacity = 0;
    எழுத்து *buf = s.getAppendBuffer(length, length, resultCapacity, status);
    எனில் (U_SUCCESS(status)) {
        in.extract(0, in.length(), buf, resultCapacity);
        s.append(buf, length, status);
    }
#இல்லையெனில்
    // No conversion available. Convert any invariant characters; substitute '?' for the rest.
    // Note: can't just call u_UCharsToChars() or CharString.appendInvariantChars() on the 
    //       whole string because they require that the entire input be invariant.
    எழுத்து buf[2];
    க்கு (முழு i=0; i<in.length(); i = in.moveIndex32(i, 1)) {
        எனில் (uprv_isInvariantUString(in.getBuffer()+i, 1)) {
            u_UCharsToChars(in.getBuffer()+i, buf, 1);
        } இல்லையெனில் {
            buf[0] = '?';
        }
        s.append(buf, 1, status);
    }
#endif
}

CStr::~CStr() {
}

மாறிலி எழுத்து * CStr::operator ()() மாறிலி {
    திரும்பு s.data();
}

U_NAMESPACE_END
