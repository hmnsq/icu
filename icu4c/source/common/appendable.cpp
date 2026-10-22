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
Appendable::appendCodePoint(UChar32 c) {
    ܐܢ(c<=0xffff) {
        ܦܢܐ appendCodeUnit(static_cast<char16_t>(c));
    } ܐܚܪܢܐ {
        ܦܢܐ appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ثابت char16_t *s, int32_t length) {
    ܐܢ(length<0) {
        char16_t c;
        ܟܡܐ((c=*s++)!=0) {
            ܐܢ(!appendCodeUnit(c)) {
                ܦܢܐ ܕܓܠܐ;
            }
        }
    } ܐܚܪܢܐ ܐܢ(length>0) {
        ثابت char16_t *limit=s+length;
        ܥܒܕ {
            ܐܢ(!appendCodeUnit(*s++)) {
                ܦܢܐ ܕܓܠܐ;
            }
        } ܟܡܐ(s<limit);
    }
    ܦܢܐ ܫܪܪܐ;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    ܦܢܐ ܫܪܪܐ;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    ܐܢ(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        ܦܢܐ nullptr;
    }
    *resultCapacity=scratchCapacity;
    ܦܢܐ scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
