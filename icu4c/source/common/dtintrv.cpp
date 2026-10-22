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



#அந்தர்பாவ "unicode/dtintrv.h"


U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(DateInterval)

//DateInterval::DateInterval(){}


DateInterval::DateInterval(UDate from, UDate to)
:   fromDate(from),
    toDate(to)
{}


DateInterval::~DateInterval(){}


DateInterval::DateInterval(மாறிலி DateInterval& other)
: UObject(other) {
    *இது = other;
}   


DateInterval&
DateInterval::operator=(மாறிலி DateInterval& other) U_LIFETIME_BOUND {
    எனில் ( இது != &other ) {
        fromDate = other.fromDate;
        toDate = other.toDate;
    }
    திரும்பு *இது;
}


DateInterval* 
DateInterval::clone() மாறிலி {
    திரும்பு புதிய DateInterval(*இது);
}


தருக்க
DateInterval::operator==(மாறிலி DateInterval& other) மாறிலி { 
    திரும்பு ( fromDate == other.fromDate && toDate == other.toDate );
}


U_NAMESPACE_END

