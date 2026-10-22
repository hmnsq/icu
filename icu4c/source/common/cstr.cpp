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
    UError_کد وضعیت = U_ZERO_ERROR;
#إذا !UCONFIG_NO_CONVERSION || U_CHARSET_IS_UTF8
    int32_t length = in.extract(0, in.length(), static_cast<حرف *>(اشاره_تهی), static_cast<uint32_t>(0));
    int32_t نتیجه_ظرفیت = 0;
    حرف *buf = s.get_Append_میانگیر(length, length, نتیجه_ظرفیت, وضعیت);
    إذا (U_SUCCESS(وضعیت)) {
        in.extract(0, in.length(), buf, نتیجه_ظرفیت);
        s.append(buf, length, وضعیت);
    }
#وإلا
    // No conversion available. Convert any invariant characters; substitute '?' for the rest.
    // Note: can't just call u_UCharsToChars() or CharString.appendInvariantChars() on the 
    //       whole string because they require that the entire input be invariant.
    حرف buf[2];
    لكل (صحيح i=0; i<in.length(); i = in.moveIndex32(i, 1)) {
        إذا (uprv_isInvariantUString(in.get_میانگیر()+i, 1)) {
            u_UChars_مقصد_Chars(in.get_میانگیر()+i, buf, 1);
        } وإلا {
            buf[0] = '?';
        }
        s.append(buf, 1, وضعیت);
    }
#پایان_اگر
}

CStr::~CStr() {
}

ثابت حرف * CStr::عملگر ()() ثابت {
    أرجع s.داده();
}

U_NAMESPACE_انتها
