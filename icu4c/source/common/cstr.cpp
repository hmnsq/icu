// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2015-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  charstr.cpp
*/
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "unicode/unistr.h"

#അന്തര്ഭാവ "cstr.h"

#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "uinvchar.h"

U_NAMESPACE_BEGIN

CStr::CStr(സ്ഥിരം UnicodeString &in) {
    UErrorCode status = U_ZERO_ERROR;
#എങ്കിൽ !UCONFIG_NO_CONVERSION || U_CHARSET_IS_UTF8
    int32_t length = in.extract(0, in.length(), static_cast<അക്ഷരം *>(nullptr), static_cast<uint32_t>(0));
    int32_t resultCapacity = 0;
    അക്ഷരം *buf = s.getAppendBuffer(length, length, resultCapacity, status);
    എങ്കിൽ (U_SUCCESS(status)) {
        in.extract(0, in.length(), buf, resultCapacity);
        s.append(buf, length, status);
    }
#അല്ലെങ്കിൽ
    // No conversion available. Convert any invariant characters; substitute '?' for the rest.
    // Note: can't just call u_UCharsToChars() or CharString.appendInvariantChars() on the 
    //       whole string because they require that the entire input be invariant.
    അക്ഷരം buf[2];
    വേണ്ടി (പൂർണ്ണസംഖ്യ i=0; i<in.length(); i = in.moveIndex32(i, 1)) {
        എങ്കിൽ (uprv_isInvariantUString(in.getBuffer()+i, 1)) {
            u_UCharsToChars(in.getBuffer()+i, buf, 1);
        } അല്ലെങ്കിൽ {
            buf[0] = '?';
        }
        s.append(buf, 1, status);
    }
#endif
}

CStr::~CStr() {
}

സ്ഥിരം അക്ഷരം * CStr::operator ()() സ്ഥിരം {
    തിരികെ s.data();
}

U_NAMESPACE_END
