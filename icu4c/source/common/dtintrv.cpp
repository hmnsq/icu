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
    𐤀𐤌 ( هذا != &other ) {
        fromDate = other.fromDate;
        toDate = other.toDate;
    }
    أرجع *هذا;
}


DateInterval* 
DateInterval::clone() ثابت {
    أرجع جديد DateInterval(*هذا);
}


منطقي
DateInterval::operator==(ثابت DateInterval& other) ثابت { 
    أرجع ( fromDate == other.fromDate && toDate == other.toDate );
}


U_NAMESPACE_END

