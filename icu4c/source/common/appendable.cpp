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
Appendable::append_رمز_نقطہ(UChar32 c) {
    اگر(c<=0xffff) {
        واپس append_رمز_Unit(static_cast<char16_t>(c));
    } ورنہ {
        واپس append_رمز_Unit(U16_LEAD(c)) && append_رمز_Unit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ثابت char16_t *s, int32_t length) {
    اگر(length<0) {
        char16_t c;
        جب((c=*s++)!=0) {
            اگر(!append_رمز_Unit(c)) {
                واپس جھوٹ;
            }
        }
    } ورنہ اگر(length>0) {
        ثابت char16_t *حد_بندی=s+length;
        کرو {
            اگر(!append_رمز_Unit(*s++)) {
                واپس جھوٹ;
            }
        } جب(s<حد_بندی);
    }
    واپس سچ;
}

UBool
Appendable::reserve_Append_استعداد(int32_t /*appendCapacity*/) {
    واپس سچ;
}

char16_t*
Appendable::get_Append_بفر(int32_t min_استعداد,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratch_استعداد,
                            int32_t* نتیجہ_استعداد) U_LIFETIME_BOUND {
    اگر(min_استعداد<1 || scratch_استعداد<min_استعداد) {
        *نتیجہ_استعداد=0;
        واپس خالی_اشاریہ;
    }
    *نتیجہ_استعداد=scratch_استعداد;
    واپس scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_انتہا
