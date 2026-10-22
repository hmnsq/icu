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



#अन्तर्भाव "unicode/dtintrv.h"


U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(DateInterval)

//DateInterval::DateInterval(){}


DateInterval::DateInterval(UDate from, UDate to)
:   fromDate(from),
    toDate(to)
{}


DateInterval::~DateInterval(){}


DateInterval::DateInterval(ध्रुव DateInterval& other)
: UObject(other) {
    *एतत् = other;
}   


DateInterval&
DateInterval::operator=(ध्रुव DateInterval& other) U_LIFETIME_BOUND {
    जइ ( एतत् != &other ) {
        fromDate = other.fromDate;
        toDate = other.toDate;
    }
    प्रत्यागम *एतत्;
}


DateInterval* 
DateInterval::clone() ध्रुव {
    प्रत्यागम नूतन DateInterval(*एतत्);
}


तार्किक
DateInterval::operator==(ध्रुव DateInterval& other) ध्रुव { 
    प्रत्यागम ( fromDate == other.fromDate && toDate == other.toDate );
}


U_NAMESPACE_END

