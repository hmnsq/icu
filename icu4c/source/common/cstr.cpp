// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2015-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  charstr.cpp
*/
#شامل "unicode/utypes.h"
#شامل "unicode/putil.h"
#شامل "unicode/unistr.h"

#شامل "cstr.h"

#شامل "charstr.h"
#شامل "uinvchar.h"

U_NAMESPACE_BEGIN

CStr::CStr(ثابت UnicodeString &in) {
    UError_رمز حالت_2 = U_ZERO_ERROR;
#اگر !UCONFIG_NO_CONVERSION || U_CHARSET_IS_UTF8
    int32_t length = in.extract(0, in.length(), static_cast<حرف *>(خالی_اشاریہ), static_cast<uint32_t>(0));
    int32_t نتیجہ_استعداد = 0;
    حرف *buf = s.get_Append_بفر(length, length, نتیجہ_استعداد, حالت_2);
    اگر (U_SUCCESS(حالت_2)) {
        in.extract(0, in.length(), buf, نتیجہ_استعداد);
        s.append(buf, length, حالت_2);
    }
#ورنہ
    // No conversion available. Convert any invariant characters; substitute '?' for the rest.
    // Note: can't just call u_UCharsToChars() or CharString.appendInvariantChars() on the 
    //       whole string because they require that the entire input be invariant.
    حرف buf[2];
    برائے (صحیح i=0; i<in.length(); i = in.moveIndex32(i, 1)) {
        اگر (uprv_isInvariantUString(in.get_بفر()+i, 1)) {
            u_UChars_مقصد_Chars(in.get_بفر()+i, buf, 1);
        } ورنہ {
            buf[0] = '?';
        }
        s.append(buf, 1, حالت_2);
    }
#اگر_ختم
}

CStr::~CStr() {
}

ثابت حرف * CStr::عامل ()() ثابت {
    واپس s.ڈیٹا();
}

U_NAMESPACE_انتہا
