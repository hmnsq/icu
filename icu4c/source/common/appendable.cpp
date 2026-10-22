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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/appendable.h"
#অন্তর্ভাব "unicode/utf16.h"

U_NAMESPACE_BEGIN

Appendable::~Appendable() {}

UBool
Appendable::appendCodePoint(UChar32 c) {
    যদি(c<=0xffff) {
        ফেরত appendCodeUnit(static_cast<char16_t>(c));
    } অন্যথা {
        ফেরত appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ধ্রুবক char16_t *s, int32_t length) {
    যদি(length<0) {
        char16_t c;
        যখন((c=*s++)!=0) {
            যদি(!appendCodeUnit(c)) {
                ফেরত মিথ্যা;
            }
        }
    } অন্যথা যদি(length>0) {
        ধ্রুবক char16_t *limit=s+length;
        করো {
            যদি(!appendCodeUnit(*s++)) {
                ফেরত মিথ্যা;
            }
        } যখন(s<limit);
    }
    ফেরত সত্য;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    ফেরত সত্য;
}

char16_t*
Appendable::get_Append_বাফার(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* ফলাফল_Capacity) U_LIFETIME_BOUND {
    যদি(minCapacity<1 || scratchCapacity<minCapacity) {
        *ফলাফল_Capacity=0;
        ফেরত nullptr;
    }
    *ফলাফল_Capacity=scratchCapacity;
    ফেরত scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
