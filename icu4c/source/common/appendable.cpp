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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/appendable.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"

U_NAMESPACE_BEGIN

Appendable::~Appendable() {}

UBool
Appendable::appendCodePoint(UChar32 c) {
    ଯଦି(c<=0xffff) {
        ଫେରନ୍ତୁ appendCodeUnit(static_cast<char16_t>(c));
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ଅଚଳ char16_t *s, int32_t length) {
    ଯଦି(length<0) {
        char16_t c;
        ଯେତେବେଳେ((c=*s++)!=0) {
            ଯଦି(!appendCodeUnit(c)) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
        }
    } ଅନ୍ୟଥା ଯଦି(length>0) {
        ଅଚଳ char16_t *limit=s+length;
        କର {
            ଯଦି(!appendCodeUnit(*s++)) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
        } ଯେତେବେଳେ(s<limit);
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    ଯଦି(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        ଫେରନ୍ତୁ nullptr;
    }
    *resultCapacity=scratchCapacity;
    ଫେରନ୍ତୁ scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
