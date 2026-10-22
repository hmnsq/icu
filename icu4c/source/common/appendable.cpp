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
Appendable::append_کوڈ_نقطہ(UChar32 c) {
    إذا(c<=0xffff) {
        أرجع append_کوڈ_Unit(static_cast<char16_t>(c));
    } وإلا {
        أرجع append_کوڈ_Unit(U16_LEAD(c)) && append_کوڈ_Unit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ثابت char16_t *s, int32_t length) {
    إذا(length<0) {
        char16_t c;
        طالما((c=*s++)!=0) {
            إذا(!append_کوڈ_Unit(c)) {
                أرجع خطأ;
            }
        }
    } وإلا إذا(length>0) {
        ثابت char16_t *حد_بندی=s+length;
        افعل {
            إذا(!append_کوڈ_Unit(*s++)) {
                أرجع خطأ;
            }
        } طالما(s<حد_بندی);
    }
    أرجع سچ;
}

UBool
Appendable::reserve_Append_سمرتھا(int32_t /*appendCapacity*/) {
    أرجع سچ;
}

char16_t*
Appendable::get_Append_بفر(int32_t min_سمرتھا,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratch_سمرتھا,
                            int32_t* نتیجہ_سمرتھا) U_LIFETIME_BOUND {
    إذا(min_سمرتھا<1 || scratch_سمرتھا<min_سمرتھا) {
        *نتیجہ_سمرتھا=0;
        أرجع خالی_سوچک;
    }
    *نتیجہ_سمرتھا=scratch_سمرتھا;
    أرجع scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_سرا
