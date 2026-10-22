// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2015-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  charstr.cpp
*/
#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/putil.h"
#অন্তর্ভাব "unicode/unistr.h"

#অন্তর্ভাব "cstr.h"

#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "uinvchar.h"

U_NAMESPACE_BEGIN

CStr::CStr(ধ্রুব UnicodeString &in) {
    UErrorCode status = U_ZERO_ERROR;
#যদি !UCONFIG_NO_CONVERSION || U_CHARSET_IS_UTF8
    int32_t length = in.extract(0, in.length(), static_cast<বর্ণ *>(nullptr), static_cast<uint32_t>(0));
    int32_t resultCapacity = 0;
    বর্ণ *buf = s.getAppendBuffer(length, length, resultCapacity, status);
    যদি (U_SUCCESS(status)) {
        in.extract(0, in.length(), buf, resultCapacity);
        s.append(buf, length, status);
    }
#অন্যথা
    // No conversion available. Convert any invariant characters; substitute '?' for the rest.
    // Note: can't just call u_UCharsToChars() or CharString.appendInvariantChars() on the 
    //       whole string because they require that the entire input be invariant.
    বর্ণ buf[2];
    প্রতি (পূর্ণাঙ্ক i=0; i<in.length(); i = in.moveIndex32(i, 1)) {
        যদি (uprv_isInvariantUString(in.getBuffer()+i, 1)) {
            u_UCharsToChars(in.getBuffer()+i, buf, 1);
        } অন্যথা {
            buf[0] = '?';
        }
        s.append(buf, 1, status);
    }
#endif
}

CStr::~CStr() {
}

ধ্রুব বর্ণ * CStr::operator ()() ধ্রুব {
    প্রত্যাগম s.data();
}

U_NAMESPACE_END
