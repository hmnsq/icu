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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/appendable.h"
#अन्तर्भाव "unicode/utf16.h"

U_NAMESPACE_BEGIN

Appendable::~Appendable() {}

UBool
Appendable::appendCodePoint(UChar32 c) {
    यदि(c<=0xffff) {
        प्रत्यागम appendCodeUnit(static_cast<char16_t>(c));
    } अन्यथा {
        प्रत्यागम appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ध्रुव char16_t *s, int32_t length) {
    यदि(length<0) {
        char16_t c;
        यावत्((c=*s++)!=0) {
            यदि(!appendCodeUnit(c)) {
                प्रत्यागम असत्य;
            }
        }
    } अन्यथा यदि(length>0) {
        ध्रुव char16_t *limit=s+length;
        कुरु {
            यदि(!appendCodeUnit(*s++)) {
                प्रत्यागम असत्य;
            }
        } यावत्(s<limit);
    }
    प्रत्यागम सत्य;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    प्रत्यागम सत्य;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    यदि(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        प्रत्यागम nullptr;
    }
    *resultCapacity=scratchCapacity;
    प्रत्यागम scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
