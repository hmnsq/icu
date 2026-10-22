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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/appendable.h"
#અન્તર્ભાવ "unicode/utf16.h"

U_NAMESPACE_BEGIN

Appendable::~Appendable() {}

UBool
Appendable::appendCodePoint(UChar32 c) {
    જો(c<=0xffff) {
        પાછા appendCodeUnit(static_cast<char16_t>(c));
    } નહીંતર {
        પાછા appendCodeUnit(U16_LEAD(c)) && appendCodeUnit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(અચલ char16_t *s, int32_t length) {
    જો(length<0) {
        char16_t c;
        જ્યારે((c=*s++)!=0) {
            જો(!appendCodeUnit(c)) {
                પાછા ખોટું;
            }
        }
    } નહીંતર જો(length>0) {
        અચલ char16_t *limit=s+length;
        કરો {
            જો(!appendCodeUnit(*s++)) {
                પાછા ખોટું;
            }
        } જ્યારે(s<limit);
    }
    પાછા સાચું;
}

UBool
Appendable::reserveAppendCapacity(int32_t /*appendCapacity*/) {
    પાછા સાચું;
}

char16_t*
Appendable::getAppendBuffer(int32_t minCapacity,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratchCapacity,
                            int32_t* resultCapacity) U_LIFETIME_BOUND {
    જો(minCapacity<1 || scratchCapacity<minCapacity) {
        *resultCapacity=0;
        પાછા nullptr;
    }
    *resultCapacity=scratchCapacity;
    પાછા scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_END
