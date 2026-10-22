// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2015-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  charstr.cpp
*/
#समावेश "unicode/utypes.h"
#समावेश "unicode/putil.h"
#समावेश "unicode/unistr.h"

#समावेश "cstr.h"

#समावेश "charstr.h"
#समावेश "uinvchar.h"

U_NAMESPACE_BEGIN

CStr::CStr(स्थिर UnicodeString &in) {
    UErrorCode status = U_ZERO_ERROR;
#अगर !UCONFIG_NO_CONVERSION || U_CHARSET_IS_UTF8
    int32_t length = in.extract(0, in.length(), static_cast<अक्षर *>(nullptr), static_cast<uint32_t>(0));
    int32_t परिणाम_Capacity = 0;
    अक्षर *buf = s.get_Append_बफर(length, length, परिणाम_Capacity, status);
    अगर (U_SUCCESS(status)) {
        in.extract(0, in.length(), buf, परिणाम_Capacity);
        s.append(buf, length, status);
    }
#अन्यथा
    // No conversion available. Convert any invariant characters; substitute '?' for the rest.
    // Note: can't just call u_UCharsToChars() or CharString.appendInvariantChars() on the 
    //       whole string because they require that the entire input be invariant.
    अक्षर buf[2];
    क्रम (पूर्णांक i=0; i<in.length(); i = in.moveIndex32(i, 1)) {
        अगर (uprv_isInvariantUString(in.get_बफर()+i, 1)) {
            u_UCharsToChars(in.get_बफर()+i, buf, 1);
        } अन्यथा {
            buf[0] = '?';
        }
        s.append(buf, 1, status);
    }
#पूर्ण_अगर
}

CStr::~CStr() {
}

स्थिर अक्षर * CStr::चालक ()() स्थिर {
    वापस s.तथ्य();
}

U_NAMESPACE_END
