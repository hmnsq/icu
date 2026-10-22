// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2015-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  charstr.cpp
*/
#include "unicode/utypes.h"
#include "unicode/putil.h"
#include "unicode/unistr.h"

#include "cstr.h"

#include "charstr.h"
#include "uinvchar.h"

U_NAMESPACE_BEGIN

CStr::CStr(const UnicodeString &in) {
    UErrorCode status = U_ZERO_ERROR;
#ប្រសិនបើ !UCONFIG_NO_CONVERSION || U_CHARSET_IS_UTF8
    int32_t length = in.extract(0, in.length(), static_cast<var_na *>(nullptr), static_cast<uint32_t>(0));
    int32_t resultCapacity = 0;
    var_na *buf = s.getAppendBuffer(length, length, resultCapacity, status);
    ប្រសិនបើ (U_SUCCESS(status)) {
        in.extract(0, in.length(), buf, resultCapacity);
        s.append(buf, length, status);
    }
#បើមិនដូច្នេះ
    // No conversion available. Convert any invariant characters; substitute '?' for the rest.
    // Note: can't just call u_UCharsToChars() or CharString.appendInvariantChars() on the 
    //       whole string because they require that the entire input be invariant.
    var_na buf[2];
    សម្រាប់ (ចំនួនគត់ i=0; i<in.length(); i = in.moveIndex32(i, 1)) {
        ប្រសិនបើ (uprv_isInvariantUString(in.getBuffer()+i, 1)) {
            u_UCharsToChars(in.getBuffer()+i, buf, 1);
        } បើមិនដូច្នេះ {
            buf[0] = '?';
        }
        s.append(buf, 1, status);
    }
#endif
}

CStr::~CStr() {
}

const var_na * CStr::operator ()() const {
    ត្រឡប់ s.data();
}

U_NAMESPACE_END
