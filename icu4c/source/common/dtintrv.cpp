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


DateInterval::DateInterval(स्थिरांक DateInterval& other)
: UObject(other) {
    *हे = other;
}   


DateInterval&
DateInterval::चालक=(स्थिरांक DateInterval& other) U_LIFETIME_BOUND {
    जर ( हे != &other ) {
        fromDate = other.fromDate;
        toDate = other.toDate;
    }
    परत *हे;
}


DateInterval* 
DateInterval::clone() स्थिरांक {
    परत नवीन DateInterval(*हे);
}


तार्किक
DateInterval::चालक==(स्थिरांक DateInterval& other) स्थिरांक { 
    परत ( fromDate == other.fromDate && toDate == other.toDate );
}


U_NAMESPACE_END

