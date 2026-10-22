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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/appendable.h"
#অন্তর্ভাৱ "unicode/utf16.h"

U_NAMESPACE_BEGIN

Appendable::~Appendable() {}

UBool
Appendable::appendCodePoint(UChar32 c) {
    যদি(c<=0xffff) {
        ঘূৰাই appendCodeUnit(static_cast<char16_t>(c));
    } অন্যথা {
        ঘূৰাই appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ধ্রুৱ char16_t *s, int32_t length) {
    যদি(length<0) {
        char16_t c;
        যেতিয়ালৈ((c=*s++)!=0) {
            যদি(!appendCodeUnit(c)) {
                ঘূৰাই মিছা;
            }
        }
    } অন্যথা যদি(length>0) {
        ধ্রুৱ char16_t *limit=s+length;
        কৰা {
            যদি(!appendCodeUnit(*s++)) {
                ঘূৰাই মিছা;
            }
        } যেতিয়ালৈ(s<limit);
    }
    ঘূৰাই সঁচা;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    ঘূৰাই সঁচা;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    যদি(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        ঘূৰাই nullptr;
    }
    *resultCapacity=scratchCapacity;
    ঘূৰাই scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
