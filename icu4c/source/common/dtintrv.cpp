// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*******************************************************************************
* Copyright (C) 2008, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*
* File DTINTRV.CPP 
*
*******************************************************************************
*/



#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/dtintrv.h"


U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(DateInterval)

//DateInterval::DateInterval(){}


DateInterval::DateInterval(UDate from, UDate ਮੰਜ਼ਿਲ)
:   fromDate(from),
    ਮੰਜ਼ਿਲ_Date(ਮੰਜ਼ਿਲ)
{}


DateInterval::~DateInterval(){}


DateInterval::DateInterval(ਅਚਲ DateInterval& other)
: UObject(other) {
    *ਇਹ = other;
}   


DateInterval&
DateInterval::ਸੰਚਾਲਕ=(ਅਚਲ DateInterval& other) U_LIFETIME_BOUND {
    ਜੇ ( ਇਹ != &other ) {
        fromDate = other.fromDate;
        ਮੰਜ਼ਿਲ_Date = other.ਮੰਜ਼ਿਲ_Date;
    }
    ਵਾਪਸ *ਇਹ;
}


DateInterval* 
DateInterval::clone() ਅਚਲ {
    ਵਾਪਸ ਨਵਾਂ DateInterval(*ਇਹ);
}


ਤਾਰਕਿਕ
DateInterval::ਸੰਚਾਲਕ==(ਅਚਲ DateInterval& other) ਅਚਲ { 
    ਵਾਪਸ ( fromDate == other.fromDate && ਮੰਜ਼ਿਲ_Date == other.ਮੰਜ਼ਿਲ_Date );
}


U_NAMESPACE_ਸਿਰਾ

