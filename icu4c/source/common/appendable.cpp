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
Appendable::append_کد_نقطه(UChar32 c) {
    إذا(c<=0xffff) {
        أرجع append_کد_Unit(static_cast<char16_t>(c));
    } وإلا {
        أرجع append_کد_Unit(U16_LEAD(c)) && append_کد_Unit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ثابت char16_t *s, int32_t length) {
    إذا(length<0) {
        char16_t c;
        طالما((c=*s++)!=0) {
            إذا(!append_کد_Unit(c)) {
                أرجع خطأ;
            }
        }
    } وإلا إذا(length>0) {
        ثابت char16_t *حد=s+length;
        افعل {
            إذا(!append_کد_Unit(*s++)) {
                أرجع خطأ;
            }
        } طالما(s<حد);
    }
    أرجع درست;
}

UBool
Appendable::reserve_Append_ظرفیت(int32_t /*appendCapacity*/) {
    أرجع درست;
}

char16_t*
Appendable::get_Append_میانگیر(int32_t min_ظرفیت,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratch_ظرفیت,
                            int32_t* نتیجه_ظرفیت) U_LIFETIME_BOUND {
    إذا(min_ظرفیت<1 || scratch_ظرفیت<min_ظرفیت) {
        *نتیجه_ظرفیت=0;
        أرجع اشاره_تهی;
    }
    *نتیجه_ظرفیت=scratch_ظرفیت;
    أرجع scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_انتها
