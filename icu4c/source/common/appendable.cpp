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
    אם(c<=0xffff) {
        החזר appendCodeUnit(static_cast<char16_t>(c));
    } אחרת {
        החזר appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ثابت char16_t *s, int32_t length) {
    אם(length<0) {
        char16_t c;
        כלעוד((c=*s++)!=0) {
            אם(!appendCodeUnit(c)) {
                החזר שקר;
            }
        }
    } אחרת אם(length>0) {
        ثابت char16_t *limit=s+length;
        עשה {
            אם(!appendCodeUnit(*s++)) {
                החזר שקר;
            }
        } כלעוד(s<limit);
    }
    החזר אמת;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    החזר אמת;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    אם(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        החזר nullptr;
    }
    *resultCapacity=scratchCapacity;
    החזר scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
