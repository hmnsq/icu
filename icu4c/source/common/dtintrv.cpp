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



#समावेश "unicode/dtintrv.h"


U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(DateInterval)

//DateInterval::DateInterval(){}


DateInterval::DateInterval(UDate from, UDate to)
:   fromDate(from),
    toDate(to)
{}


DateInterval::~DateInterval(){}


DateInterval::DateInterval(स्थिर DateInterval& other)
: UObject(other) {
    *यह = other;
}   


DateInterval&
DateInterval::चालक=(स्थिर DateInterval& other) U_LIFETIME_BOUND {
    अगर ( यह != &other ) {
        fromDate = other.fromDate;
        toDate = other.toDate;
    }
    वापस *यह;
}


DateInterval* 
DateInterval::clone() स्थिर {
    वापस नया DateInterval(*यह);
}


bool
DateInterval::चालक==(स्थिर DateInterval& other) स्थिर { 
    वापस ( fromDate == other.fromDate && toDate == other.toDate );
}


U_NAMESPACE_END

