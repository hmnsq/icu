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



#ಅನ್ತರ್ಭಾವ "unicode/dtintrv.h"


U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(DateInterval)

//DateInterval::DateInterval(){}


DateInterval::DateInterval(UDate from, UDate to)
:   fromDate(from),
    toDate(to)
{}


DateInterval::~DateInterval(){}


DateInterval::DateInterval(ಧ್ರುವ DateInterval& other)
: UObject(other) {
    *ಏತತ್ = other;
}   


DateInterval&
DateInterval::operator=(ಧ್ರುವ DateInterval& other) U_LIFETIME_BOUND {
    ಯದಿ ( ಏತತ್ != &other ) {
        fromDate = other.fromDate;
        toDate = other.toDate;
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}


DateInterval* 
DateInterval::clone() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ನೂತನ DateInterval(*ಏತತ್);
}


ತಾರ್ಕಿಕ
DateInterval::operator==(ಧ್ರುವ DateInterval& other) ಧ್ರುವ { 
    ಪ್ರತ್ಯಾಗಮ ( fromDate == other.fromDate && toDate == other.toDate );
}


U_NAMESPACE_END

