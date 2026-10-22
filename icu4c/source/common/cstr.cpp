// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2015-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  charstr.cpp
*/
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "cstr.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uinvchar.h"

U_NAMESPACE_BEGIN

CStr::CStr(ਅਚਲ UnicodeString &in) {
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
#ਜੇ !UCONFIG_NO_CONVERSION || U_CHARSET_IS_UTF8
    int32_t length = in.extract(0, in.length(), static_cast<ਅੱਖਰ *>(ਖਾਲੀ_ਸੂਚਕ), static_cast<uint32_t>(0));
    int32_t ਨਤੀਜਾ_ਸਮਰੱਥਾ = 0;
    ਅੱਖਰ *buf = s.get_Append_ਬਫਰ(length, length, ਨਤੀਜਾ_ਸਮਰੱਥਾ, ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        in.extract(0, in.length(), buf, ਨਤੀਜਾ_ਸਮਰੱਥਾ);
        s.append(buf, length, ਸਥਿਤੀ);
    }
#ਨਹੀਂਤਾਂ
    // No conversion available. Convert any invariant characters; substitute '?' for the rest.
    // Note: can't just call u_UCharsToChars() or CharString.appendInvariantChars() on the 
    //       whole string because they require that the entire input be invariant.
    ਅੱਖਰ buf[2];
    ਲਈ (ਪੂਰਨ i=0; i<in.length(); i = in.moveIndex32(i, 1)) {
        ਜੇ (uprv_isInvariantUString(in.get_ਬਫਰ()+i, 1)) {
            u_UChars_ਮੰਜ਼ਿਲ_Chars(in.get_ਬਫਰ()+i, buf, 1);
        } ਨਹੀਂਤਾਂ {
            buf[0] = '?';
        }
        s.append(buf, 1, ਸਥਿਤੀ);
    }
#ਜੇ_ਅੰਤ
}

CStr::~CStr() {
}

ਅਚਲ ਅੱਖਰ * CStr::ਸੰਚਾਲਕ ()() ਅਚਲ {
    ਵਾਪਸ s.ਅੰਕੜੇ();
}

U_NAMESPACE_ਸਿਰਾ
