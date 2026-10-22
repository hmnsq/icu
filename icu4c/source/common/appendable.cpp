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
        फर्काऊ appendCodeUnit(static_cast<char16_t>(c));
    } अन्यथा {
        फर्काऊ appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(अचर char16_t *s, int32_t length) {
    यदि(length<0) {
        char16_t c;
        जबसम्म((c=*s++)!=0) {
            यदि(!appendCodeUnit(c)) {
                फर्काऊ असत्य;
            }
        }
    } अन्यथा यदि(length>0) {
        अचर char16_t *limit=s+length;
        गर {
            यदि(!appendCodeUnit(*s++)) {
                फर्काऊ असत्य;
            }
        } जबसम्म(s<limit);
    }
    फर्काऊ सत्य;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    फर्काऊ सत्य;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    यदि(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        फर्काऊ nullptr;
    }
    *resultCapacity=scratchCapacity;
    फर्काऊ scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
