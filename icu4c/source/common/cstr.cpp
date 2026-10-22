// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2015-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  charstr.cpp
*/
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "unicode/unistr.h"

#అన్తర్భావ "cstr.h"

#అన్తర్భావ "charstr.h"
#అన్తర్భావ "uinvchar.h"

U_NAMESPACE_BEGIN

CStr::CStr(స్థిరరాశి UnicodeString &in) {
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
#ఒకవేళ !UCONFIG_NO_CONVERSION || U_CHARSET_IS_UTF8
    int32_t length = in.extract(0, in.length(), static_cast<అక్షరం *>(శూన్యసూచిక), static_cast<uint32_t>(0));
    int32_t ఫలితం_సామర్థ్యం = 0;
    అక్షరం *buf = s.get_Append_బఫరు(length, length, ఫలితం_సామర్థ్యం, స్థితి_2);
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        in.extract(0, in.length(), buf, ఫలితం_సామర్థ్యం);
        s.append(buf, length, స్థితి_2);
    }
#లేకపోతే
    // No conversion available. Convert any invariant characters; substitute '?' for the rest.
    // Note: can't just call u_UCharsToChars() or CharString.appendInvariantChars() on the 
    //       whole string because they require that the entire input be invariant.
    అక్షరం buf[2];
    కొరకు (పూర్ణాంకం i=0; i<in.length(); i = in.moveIndex32(i, 1)) {
        ఒకవేళ (uprv_isInvariantUString(in.get_బఫరు()+i, 1)) {
            u_UChars_గమ్యం_Chars(in.get_బఫరు()+i, buf, 1);
        } లేకపోతే {
            buf[0] = '?';
        }
        s.append(buf, 1, స్థితి_2);
    }
#అయితే_ముగింపు
}

CStr::~CStr() {
}

స్థిరరాశి అక్షరం * CStr::క్రియాకారకం ()() స్థిరరాశి {
    తిరిగి s.దత్తాంశం();
}

U_NAMESPACE_చివర
