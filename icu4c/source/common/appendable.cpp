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

#ضمن "unicode/utypes.h"
#ضمن "unicode/appendable.h"
#ضمن "unicode/utf16.h"

U_NAMESPACE_BEGIN

Appendable::~Appendable() {}

UBool
Appendable::appendCodePoint(UChar32 c) {
    𐡄𐡍(c<=0xffff) {
        أرجع appendCodeUnit(static_cast<char16_t>(c));
    } وإلا {
        أرجع appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ثابت char16_t *s, int32_t length) {
    𐡄𐡍(length<0) {
        char16_t c;
        طالما((c=*s++)!=0) {
            𐡄𐡍(!appendCodeUnit(c)) {
                أرجع خطأ;
            }
        }
    } وإلا 𐡄𐡍(length>0) {
        ثابت char16_t *limit=s+length;
        𐡏𐡁𐡃 {
            𐡄𐡍(!appendCodeUnit(*s++)) {
                أرجع خطأ;
            }
        } طالما(s<limit);
    }
    أرجع 𐡒𐡔𐡈;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    أرجع 𐡒𐡔𐡈;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    𐡄𐡍(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        أرجع nullptr;
    }
    *resultCapacity=scratchCapacity;
    أرجع scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
