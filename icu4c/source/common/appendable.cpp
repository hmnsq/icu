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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/appendable.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"

U_NAMESPACE_BEGIN

Appendable::~Appendable() {}

UBool
Appendable::appendCodePoint(UChar32 c) {
    ಯದಿ(c<=0xffff) {
        ಪ್ರತ್ಯಾಗಮ appendCodeUnit(static_cast<char16_t>(c));
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ಧ್ರುವ char16_t *s, int32_t length) {
    ಯದಿ(length<0) {
        char16_t c;
        ಯಾವತ್((c=*s++)!=0) {
            ಯದಿ(!appendCodeUnit(c)) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
        }
    } ಅನ್ಯಥಾ ಯದಿ(length>0) {
        ಧ್ರುವ char16_t *limit=s+length;
        ಕುರು {
            ಯದಿ(!appendCodeUnit(*s++)) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
        } ಯಾವತ್(s<limit);
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    ಯದಿ(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    *resultCapacity=scratchCapacity;
    ಪ್ರತ್ಯಾಗಮ scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
