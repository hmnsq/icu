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

#समावेश "unicode/utypes.h"
#समावेश "unicode/appendable.h"
#समावेश "unicode/utf16.h"

U_NAMESPACE_BEGIN

Appendable::~Appendable() {}

UBool
Appendable::appendCodePoint(UChar32 c) {
    अगर(c<=0xffff) {
        वापस appendCodeUnit(static_cast<char16_t>(c));
    } अन्यथा {
        वापस appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(स्थिर char16_t *s, int32_t length) {
    अगर(length<0) {
        char16_t c;
        जबतक((c=*s++)!=0) {
            अगर(!appendCodeUnit(c)) {
                वापस false;
            }
        }
    } अन्यथा अगर(length>0) {
        स्थिर char16_t *limit=s+length;
        करो {
            अगर(!appendCodeUnit(*s++)) {
                वापस false;
            }
        } जबतक(s<limit);
    }
    वापस true;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    वापस true;
}

char16_t*
Appendable::get_Append_बफर(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* परिणाम_Capacity) U_LIFETIME_BOUND {
    अगर(minCapacity<1 || scratchCapacity<minCapacity) {
        *परिणाम_Capacity=0;
        वापस nullptr;
    }
    *परिणाम_Capacity=scratchCapacity;
    वापस scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
