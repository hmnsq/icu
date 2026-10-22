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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/appendable.h"
#அந்தர்பாவ "unicode/utf16.h"

U_NAMESPACE_BEGIN

Appendable::~Appendable() {}

UBool
Appendable::appendCodePoint(UChar32 c) {
    எனில்(c<=0xffff) {
        திரும்பு appendCodeUnit(static_cast<char16_t>(c));
    } இல்லையெனில் {
        திரும்பு appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(மாறிலி char16_t *s, int32_t length) {
    எனில்(length<0) {
        char16_t c;
        போது((c=*s++)!=0) {
            எனில்(!appendCodeUnit(c)) {
                திரும்பு பொய்;
            }
        }
    } இல்லையெனில் எனில்(length>0) {
        மாறிலி char16_t *limit=s+length;
        செய் {
            எனில்(!appendCodeUnit(*s++)) {
                திரும்பு பொய்;
            }
        } போது(s<limit);
    }
    திரும்பு உண்மை;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    திரும்பு உண்மை;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    எனில்(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        திரும்பு nullptr;
    }
    *resultCapacity=scratchCapacity;
    திரும்பு scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
