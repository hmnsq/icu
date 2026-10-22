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

#شامل "unicode/utypes.h"
#شامل "unicode/appendable.h"
#شامل "unicode/utf16.h"

U_NAMESPACE_BEGIN

Appendable::~Appendable() {}

UBool
Appendable::appendCodePoint(UChar32 c) {
    اگر(c<=0xffff) {
        بازگشت appendCodeUnit(static_cast<char16_t>(c));
    } وگرنه {
        بازگشت appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ثابت char16_t *s, int32_t length) {
    اگر(length<0) {
        char16_t c;
        وقتی((c=*s++)!=0) {
            اگر(!appendCodeUnit(c)) {
                بازگشت نادرست;
            }
        }
    } وگرنه اگر(length>0) {
        ثابت char16_t *limit=s+length;
        انجامبده {
            اگر(!appendCodeUnit(*s++)) {
                بازگشت نادرست;
            }
        } وقتی(s<limit);
    }
    بازگشت درست;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    بازگشت درست;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    اگر(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        بازگشت nullptr;
    }
    *resultCapacity=scratchCapacity;
    بازگشت scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
