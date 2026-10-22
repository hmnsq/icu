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
        প্রত্যাগম appendCodeUnit(static_cast<char16_t>(c));
    } অন্যথা {
        প্রত্যাগম appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ধ্রুব char16_t *s, int32_t length) {
    যদি(length<0) {
        char16_t c;
        যাবত্((c=*s++)!=0) {
            যদি(!appendCodeUnit(c)) {
                প্রত্যাগম অসত্য;
            }
        }
    } অন্যথা যদি(length>0) {
        ধ্রুব char16_t *limit=s+length;
        কুরু {
            যদি(!appendCodeUnit(*s++)) {
                প্রত্যাগম অসত্য;
            }
        } যাবত্(s<limit);
    }
    প্রত্যাগম সত্য;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    প্রত্যাগম সত্য;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    যদি(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        প্রত্যাগম nullptr;
    }
    *resultCapacity=scratchCapacity;
    প্রত্যাগম scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
