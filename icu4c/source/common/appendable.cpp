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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/appendable.h"
#अन्तर्भाव "unicode/utf16.h"

U_NAMESPACE_BEGIN

Appendable::~Appendable() {}

UBool
Appendable::append_संहिता_Point(UChar32 c) {
    यदि(c<=0xffff) {
        प्रत्यागम append_संहिता_Unit(static_cast<char16_t>(c));
    } अन्यथा {
        प्रत्यागम append_संहिता_Unit(U16_LEAD(c)) && append_संहिता_Unit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ध्रुव char16_t *s, int32_t length) {
    यदि(length<0) {
        char16_t c;
        यावत्((c=*s++)!=0) {
            यदि(!append_संहिता_Unit(c)) {
                प्रत्यागम असत्य;
            }
        }
    } अन्यथा यदि(length>0) {
        ध्रुव char16_t *सीमा=s+length;
        कुरु {
            यदि(!append_संहिता_Unit(*s++)) {
                प्रत्यागम असत्य;
            }
        } यावत्(s<सीमा);
    }
    प्रत्यागम सत्य;
}

UBool
Appendable::reserve_Append_क्षमता(int32_t /*appendCapacity*/) {
    प्रत्यागम सत्य;
}

char16_t*
Appendable::get_Append_बफर(int32_t min_क्षमता,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratch_क्षमता,
                            int32_t* परिणाम_क्षमता) U_LIFETIME_BOUND {
    यदि(min_क्षमता<1 || scratch_क्षमता<min_क्षमता) {
        *परिणाम_क्षमता=0;
        प्रत्यागम शून्यसूचकः;
    }
    *परिणाम_क्षमता=scratch_क्षमता;
    प्रत्यागम scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_अन्तः
