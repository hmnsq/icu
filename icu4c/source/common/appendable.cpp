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
    အကယ်၍(c<=0xffff) {
        ပြန်ပေး appendCodeUnit(static_cast<char16_t>(c));
    } မဟုတ်ပါက {
        ပြန်ပေး appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(const char16_t *s, int32_t length) {
    အကယ်၍(length<0) {
        char16_t c;
        နေစဉ်((c=*s++)!=0) {
            အကယ်၍(!appendCodeUnit(c)) {
                ပြန်ပေး false;
            }
        }
    } မဟုတ်ပါက အကယ်၍(length>0) {
        const char16_t *limit=s+length;
        do {
            အကယ်၍(!appendCodeUnit(*s++)) {
                ပြန်ပေး false;
            }
        } နေစဉ်(s<limit);
    }
    ပြန်ပေး true;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    ပြန်ပေး true;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    အကယ်၍(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        ပြန်ပေး nullptr;
    }
    *resultCapacity=scratchCapacity;
    ပြန်ပေး scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
