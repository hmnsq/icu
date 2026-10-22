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
    सचे(c<=0xffff) {
        पटिनिवत्त appendCodeUnit(static_cast<char16_t>(c));
    } अञ्ञथा {
        पटिनिवत्त appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ध्रुव char16_t *s, int32_t length) {
    सचे(length<0) {
        char16_t c;
        याव((c=*s++)!=0) {
            सचे(!appendCodeUnit(c)) {
                पटिनिवत्त मिच्छा;
            }
        }
    } अञ्ञथा सचे(length>0) {
        ध्रुव char16_t *limit=s+length;
        करोहि {
            सचे(!appendCodeUnit(*s++)) {
                पटिनिवत्त मिच्छा;
            }
        } याव(s<limit);
    }
    पटिनिवत्त सच्च;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    पटिनिवत्त सच्च;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    सचे(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        पटिनिवत्त nullptr;
    }
    *resultCapacity=scratchCapacity;
    पटिनिवत्त scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
