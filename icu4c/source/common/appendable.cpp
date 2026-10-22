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
    もし(c<=0xffff) {
        戻る appendCodeUnit(static_cast<char16_t>(c));
    } 否则 {
        戻る appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(const char16_t *s, int32_t length) {
    もし(length<0) {
        char16_t c;
        ながら((c=*s++)!=0) {
            もし(!appendCodeUnit(c)) {
                戻る false;
            }
        }
    } 否则 もし(length>0) {
        const char16_t *limit=s+length;
        do {
            もし(!appendCodeUnit(*s++)) {
                戻る false;
            }
        } ながら(s<limit);
    }
    戻る true;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    戻る true;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    もし(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        戻る nullptr;
    }
    *resultCapacity=scratchCapacity;
    戻る scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
