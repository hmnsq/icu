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

CStr::CStr(ধ্রুবক UnicodeString &in) {
    UErrorCode status = U_ZERO_ERROR;
#যদি !UCONFIG_NO_CONVERSION || U_CHARSET_IS_UTF8
    int32_t length = in.extract(0, in.length(), static_cast<অক্ষর *>(nullptr), static_cast<uint32_t>(0));
    int32_t ফলাফল_Capacity = 0;
    অক্ষর *buf = s.get_Append_বাফার(length, length, ফলাফল_Capacity, status);
    যদি (U_SUCCESS(status)) {
        in.extract(0, in.length(), buf, ফলাফল_Capacity);
        s.append(buf, length, status);
    }
#অন্যথা
    // No conversion available. Convert any invariant characters; substitute '?' for the rest.
    // Note: can't just call u_UCharsToChars() or CharString.appendInvariantChars() on the 
    //       whole string because they require that the entire input be invariant.
    অক্ষর buf[2];
    জন্য (পূর্ণসংখ্যা i=0; i<in.length(); i = in.moveIndex32(i, 1)) {
        যদি (uprv_isInvariantUString(in.get_বাফার()+i, 1)) {
            u_UCharsToChars(in.get_বাফার()+i, buf, 1);
        } অন্যথা {
            buf[0] = '?';
        }
        s.append(buf, 1, status);
    }
#endif
}

CStr::~CStr() {
}

ধ্রুবক অক্ষর * CStr::operator ()() ধ্রুবক {
    ফেরত s.তথ্য();
}

U_NAMESPACE_END
