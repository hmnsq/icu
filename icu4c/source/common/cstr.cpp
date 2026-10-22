// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2015-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  charstr.cpp
*/
#ضمن "unicode/utypes.h"
#ضمن "unicode/putil.h"
#ضمن "unicode/unistr.h"

#ضمن "cstr.h"

#ضمن "charstr.h"
#ضمن "uinvchar.h"

U_NAMESPACE_BEGIN

CStr::CStr(ثابت UnicodeString &in) {
    UErrorCode status = U_ZERO_ERROR;
#ܐܢ !UCONFIG_NO_CONVERSION || U_CHARSET_IS_UTF8
    int32_t length = in.extract(0, in.length(), static_cast<حرف *>(nullptr), static_cast<uint32_t>(0));
    int32_t resultCapacity = 0;
    حرف *buf = s.getAppendBuffer(length, length, resultCapacity, status);
    ܐܢ (U_SUCCESS(status)) {
        in.extract(0, in.length(), buf, resultCapacity);
        s.append(buf, length, status);
    }
#ܐܚܪܢܐ
    // No conversion available. Convert any invariant characters; substitute '?' for the rest.
    // Note: can't just call u_UCharsToChars() or CharString.appendInvariantChars() on the 
    //       whole string because they require that the entire input be invariant.
    حرف buf[2];
    لكل (صحيح i=0; i<in.length(); i = in.moveIndex32(i, 1)) {
        ܐܢ (uprv_isInvariantUString(in.getBuffer()+i, 1)) {
            u_UCharsToChars(in.getBuffer()+i, buf, 1);
        } ܐܚܪܢܐ {
            buf[0] = '?';
        }
        s.append(buf, 1, status);
    }
#endif
}

CStr::~CStr() {
}

ثابت حرف * CStr::operator ()() ثابت {
    ܦܢܐ s.data();
}

U_NAMESPACE_END
