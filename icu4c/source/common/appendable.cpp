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
Appendable::append_ڪوڊ_نقطو(UChar32 c) {
    إذا(c<=0xffff) {
        أرجع append_ڪوڊ_Unit(static_cast<char16_t>(c));
    } وإلا {
        أرجع append_ڪوڊ_Unit(U16_LEAD(c)) && append_ڪوڊ_Unit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ثابت char16_t *s, int32_t length) {
    إذا(length<0) {
        char16_t c;
        طالما((c=*s++)!=0) {
            إذا(!append_ڪوڊ_Unit(c)) {
                أرجع خطأ;
            }
        }
    } وإلا إذا(length>0) {
        ثابت char16_t *حد_بندي=s+length;
        افعل {
            إذا(!append_ڪوڊ_Unit(*s++)) {
                أرجع خطأ;
            }
        } طالما(s<حد_بندي);
    }
    أرجع سچ;
}

UBool
Appendable::reserve_Append_صلاحيت(int32_t /*appendCapacity*/) {
    أرجع سچ;
}

char16_t*
Appendable::get_Append_بفر(int32_t min_صلاحيت,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratch_صلاحيت,
                            int32_t* نتيجو_صلاحيت) U_LIFETIME_BOUND {
    إذا(min_صلاحيت<1 || scratch_صلاحيت<min_صلاحيت) {
        *نتيجو_صلاحيت=0;
        أرجع خالي_اشارو;
    }
    *نتيجو_صلاحيت=scratch_صلاحيت;
    أرجع scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_پڇاڙي
