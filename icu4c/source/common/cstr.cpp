// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2015-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  charstr.cpp
*/
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "unicode/unistr.h"

#अन्तर्भाव "cstr.h"

#अन्तर्भाव "charstr.h"
#अन्तर्भाव "uinvchar.h"

U_NAMESPACE_BEGIN

CStr::CStr(ध्रुव UnicodeString &in) {
    UErrorCode status = U_ZERO_ERROR;
#यदि !UCONFIG_NO_CONVERSION || U_CHARSET_IS_UTF8
    int32_t length = in.extract(0, in.length(), static_cast<वर्ण *>(nullptr), static_cast<uint32_t>(0));
    int32_t resultCapacity = 0;
    वर्ण *buf = s.getAppendBuffer(length, length, resultCapacity, status);
    यदि (U_SUCCESS(status)) {
        in.extract(0, in.length(), buf, resultCapacity);
        s.append(buf, length, status);
    }
#अन्यथा
    // No conversion available. Convert any invariant characters; substitute '?' for the rest.
    // Note: can't just call u_UCharsToChars() or CharString.appendInvariantChars() on the 
    //       whole string because they require that the entire input be invariant.
    वर्ण buf[2];
    प्रति (पूर्णाङ्क i=0; i<in.length(); i = in.moveIndex32(i, 1)) {
        यदि (uprv_isInvariantUString(in.getBuffer()+i, 1)) {
            u_UCharsToChars(in.getBuffer()+i, buf, 1);
        } अन्यथा {
            buf[0] = '?';
        }
        s.append(buf, 1, status);
    }
#endif
}

CStr::~CStr() {
}

ध्रुव वर्ण * CStr::operator ()() ध्रुव {
    प्रत्यागम s.data();
}

U_NAMESPACE_END
