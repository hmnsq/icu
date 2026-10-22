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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/appendable.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"

U_NAMESPACE_BEGIN

Appendable::~Appendable() {}

UBool
Appendable::append_ਕੋਡ_ਬਿੰਦੂ(UChar32 c) {
    ਜੇ(c<=0xffff) {
        ਵਾਪਸ append_ਕੋਡ_Unit(static_cast<char16_t>(c));
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ append_ਕੋਡ_Unit(U16_LEAD(c)) && append_ਕੋਡ_Unit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ਅਚਲ char16_t *s, int32_t length) {
    ਜੇ(length<0) {
        char16_t c;
        ਜਦੋਂ((c=*s++)!=0) {
            ਜੇ(!append_ਕੋਡ_Unit(c)) {
                ਵਾਪਸ ਝੂਠ;
            }
        }
    } ਨਹੀਂਤਾਂ ਜੇ(length>0) {
        ਅਚਲ char16_t *ਹੱਦਬੰਦੀ=s+length;
        ਕਰੋ {
            ਜੇ(!append_ਕੋਡ_Unit(*s++)) {
                ਵਾਪਸ ਝੂਠ;
            }
        } ਜਦੋਂ(s<ਹੱਦਬੰਦੀ);
    }
    ਵਾਪਸ ਸੱਚ;
}

UBool
Appendable::reserve_Append_ਸਮਰੱਥਾ(int32_t /*appendCapacity*/) {
    ਵਾਪਸ ਸੱਚ;
}

char16_t*
Appendable::get_Append_ਬਫਰ(int32_t min_ਸਮਰੱਥਾ,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratch_ਸਮਰੱਥਾ,
                            int32_t* ਨਤੀਜਾ_ਸਮਰੱਥਾ) U_LIFETIME_BOUND {
    ਜੇ(min_ਸਮਰੱਥਾ<1 || scratch_ਸਮਰੱਥਾ<min_ਸਮਰੱਥਾ) {
        *ਨਤੀਜਾ_ਸਮਰੱਥਾ=0;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    *ਨਤੀਜਾ_ਸਮਰੱਥਾ=scratch_ਸਮਰੱਥਾ;
    ਵਾਪਸ scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_ਸਿਰਾ
