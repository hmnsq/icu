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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/appendable.h"
#അന്തര്ഭാവ "unicode/utf16.h"

U_NAMESPACE_BEGIN

Appendable::~Appendable() {}

UBool
Appendable::appendCodePoint(UChar32 c) {
    എങ്കിൽ(c<=0xffff) {
        തിരികെ appendCodeUnit(static_cast<char16_t>(c));
    } അല്ലെങ്കിൽ {
        തിരികെ appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(സ്ഥിരം char16_t *s, int32_t length) {
    എങ്കിൽ(length<0) {
        char16_t c;
        ആകുമ്പോൾ((c=*s++)!=0) {
            എങ്കിൽ(!appendCodeUnit(c)) {
                തിരികെ അസത്യം;
            }
        }
    } അല്ലെങ്കിൽ എങ്കിൽ(length>0) {
        സ്ഥിരം char16_t *limit=s+length;
        ചെയ്യുക {
            എങ്കിൽ(!appendCodeUnit(*s++)) {
                തിരികെ അസത്യം;
            }
        } ആകുമ്പോൾ(s<limit);
    }
    തിരികെ സത്യം;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    തിരികെ സത്യം;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    എങ്കിൽ(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        തിരികെ nullptr;
    }
    *resultCapacity=scratchCapacity;
    തിരികെ scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
