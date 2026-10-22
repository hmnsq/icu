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

#include "unicode/utypes.h"
#include "unicode/appendable.h"
#include "unicode/utf16.h"

U_NAMESPACE_BEGIN

Appendable::~Appendable() {}

UBool
Appendable::appendCodePoint(UChar32 c) {
    ပေႃး(c<=0xffff) {
        ပွၵ်ႈ appendCodeUnit(static_cast<char16_t>(c));
    } ပေႃးဢမ်ႇ {
        ပွၵ်ႈ appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(const char16_t *s, int32_t length) {
    ပေႃး(length<0) {
        char16_t c;
        ၶၢဝ်း((c=*s++)!=0) {
            ပေႃး(!appendCodeUnit(c)) {
                ပွၵ်ႈ false;
            }
        }
    } ပေႃးဢမ်ႇ ပေႃး(length>0) {
        const char16_t *limit=s+length;
        do {
            ပေႃး(!appendCodeUnit(*s++)) {
                ပွၵ်ႈ false;
            }
        } ၶၢဝ်း(s<limit);
    }
    ပွၵ်ႈ true;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    ပွၵ်ႈ true;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    ပေႃး(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        ပွၵ်ႈ nullptr;
    }
    *resultCapacity=scratchCapacity;
    ပွၵ်ႈ scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
