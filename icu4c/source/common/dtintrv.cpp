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



#અન્તર્ભાવ "unicode/dtintrv.h"


U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(DateInterval)

//DateInterval::DateInterval(){}


DateInterval::DateInterval(UDate from, UDate to)
:   fromDate(from),
    toDate(to)
{}


DateInterval::~DateInterval(){}


DateInterval::DateInterval(અચલ DateInterval& other)
: UObject(other) {
    *આ = other;
}   


DateInterval&
DateInterval::operator=(અચલ DateInterval& other) U_LIFETIME_BOUND {
    જો ( આ != &other ) {
        fromDate = other.fromDate;
        toDate = other.toDate;
    }
    પાછા *આ;
}


DateInterval* 
DateInterval::clone() અચલ {
    પાછા નવું DateInterval(*આ);
}


તાર્કિક
DateInterval::operator==(અચલ DateInterval& other) અચલ { 
    પાછા ( fromDate == other.fromDate && toDate == other.toDate );
}


U_NAMESPACE_END

