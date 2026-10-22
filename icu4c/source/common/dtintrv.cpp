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



#ضمن "unicode/dtintrv.h"


U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(DateInterval)

//DateInterval::DateInterval(){}


DateInterval::DateInterval(UDate from, UDate to)
:   fromDate(from),
    toDate(to)
{}


DateInterval::~DateInterval(){}


DateInterval::DateInterval(ثابت DateInterval& other)
: UObject(other) {
    *هذا = other;
}   


DateInterval&
DateInterval::operator=(ثابت DateInterval& other) U_LIFETIME_BOUND {
    إذا ( هذا != &other ) {
        fromDate = other.fromDate;
        toDate = other.toDate;
    }
    إرجاع *هذا;
}


DateInterval* 
DateInterval::clone() ثابت {
    إرجاع جديد DateInterval(*هذا);
}


منطقي
DateInterval::operator==(ثابت DateInterval& other) ثابت { 
    إرجاع ( fromDate == other.fromDate && toDate == other.toDate );
}


U_NAMESPACE_END

