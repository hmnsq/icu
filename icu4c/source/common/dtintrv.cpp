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


DateInterval::DateInterval(UDate from, UDate منزل)
:   fromDate(from),
    منزل_Date(منزل)
{}


DateInterval::~DateInterval(){}


DateInterval::DateInterval(ثابت DateInterval& other)
: UObject(other) {
    *هذا = other;
}   


DateInterval&
DateInterval::چلوونکی=(ثابت DateInterval& other) U_LIFETIME_BOUND {
    إذا ( هذا != &other ) {
        fromDate = other.fromDate;
        منزل_Date = other.منزل_Date;
    }
    أرجع *هذا;
}


DateInterval* 
DateInterval::clone() ثابت {
    أرجع جديد DateInterval(*هذا);
}


منطقي
DateInterval::چلوونکی==(ثابت DateInterval& other) ثابت { 
    أرجع ( fromDate == other.fromDate && منزل_Date == other.منزل_Date );
}


U_NAMESPACE_پای_ټکی

