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
    ᱡᱩᱫᱤ(c<=0xffff) {
        ᱨᱩᱣᱟᱹᱲ appendCodeUnit(static_cast<char16_t>(c));
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(const char16_t *s, int32_t length) {
    ᱡᱩᱫᱤ(length<0) {
        char16_t c;
        ᱡᱚᱠᱷᱚᱱ((c=*s++)!=0) {
            ᱡᱩᱫᱤ(!appendCodeUnit(c)) {
                ᱨᱩᱣᱟᱹᱲ false;
            }
        }
    } ᱵᱟᱝ ᱡᱩᱫᱤ(length>0) {
        const char16_t *limit=s+length;
        do {
            ᱡᱩᱫᱤ(!appendCodeUnit(*s++)) {
                ᱨᱩᱣᱟᱹᱲ false;
            }
        } ᱡᱚᱠᱷᱚᱱ(s<limit);
    }
    ᱨᱩᱣᱟᱹᱲ true;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    ᱨᱩᱣᱟᱹᱲ true;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    ᱡᱩᱫᱤ(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        ᱨᱩᱣᱟᱹᱲ nullptr;
    }
    *resultCapacity=scratchCapacity;
    ᱨᱩᱣᱟᱹᱲ scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
