// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  appendable.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010dec07
*   created by: Markus W. Scherer
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/appendable.h"
#అన్తర్భావ "unicode/utf16.h"

U_NAMESPACE_BEGIN

Appendable::~Appendable() {}

UBool
Appendable::append_సంకేతం_బిందువు(UChar32 c) {
    ఒకవేళ(c<=0xffff) {
        తిరిగి append_సంకేతం_Unit(static_cast<char16_t>(c));
    } లేకపోతే {
        తిరిగి append_సంకేతం_Unit(U16_LEAD(c)) && append_సంకేతం_Unit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(స్థిరరాశి char16_t *s, int32_t length) {
    ఒకవేళ(length<0) {
        char16_t c;
        అయితే((c=*s++)!=0) {
            ఒకవేళ(!append_సంకేతం_Unit(c)) {
                తిరిగి అబద్ధం;
            }
        }
    } లేకపోతే ఒకవేళ(length>0) {
        స్థిరరాశి char16_t *హద్దు=s+length;
        చేయి {
            ఒకవేళ(!append_సంకేతం_Unit(*s++)) {
                తిరిగి అబద్ధం;
            }
        } అయితే(s<హద్దు);
    }
    తిరిగి నిజం;
}

UBool
Appendable::reserve_Append_సామర్థ్యం(int32_t /*appendCapacity*/) {
    తిరిగి నిజం;
}

char16_t*
Appendable::get_Append_బఫరు(int32_t min_సామర్థ్యం,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratch_సామర్థ్యం,
                            int32_t* ఫలితం_సామర్థ్యం) U_LIFETIME_BOUND {
    ఒకవేళ(min_సామర్థ్యం<1 || scratch_సామర్థ్యం<min_సామర్థ్యం) {
        *ఫలితం_సామర్థ్యం=0;
        తిరిగి శూన్యసూచిక;
    }
    *ఫలితం_సామర్థ్యం=scratch_సామర్థ్యం;
    తిరిగి scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_చివర
