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



#অন্তর্ভাৱ "unicode/dtintrv.h"


U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(DateInterval)

//DateInterval::DateInterval(){}


DateInterval::DateInterval(UDate from, UDate to)
:   fromDate(from),
    toDate(to)
{}


DateInterval::~DateInterval(){}


DateInterval::DateInterval(ধ্রুৱ DateInterval& other)
: UObject(other) {
    *এতত্ = other;
}   


DateInterval&
DateInterval::operator=(ধ্রুৱ DateInterval& other) U_LIFETIME_BOUND {
    যদি ( এতত্ != &other ) {
        fromDate = other.fromDate;
        toDate = other.toDate;
    }
    ঘূৰাই *এতত্;
}


DateInterval* 
DateInterval::clone() ধ্রুৱ {
    ঘূৰাই নূতন DateInterval(*এতত্);
}


তার্কিক
DateInterval::operator==(ধ্রুৱ DateInterval& other) ধ্রুৱ { 
    ঘূৰাই ( fromDate == other.fromDate && toDate == other.toDate );
}


U_NAMESPACE_END

