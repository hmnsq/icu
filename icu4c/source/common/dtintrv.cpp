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



#ଅନ୍ତର୍ଭାଵ "unicode/dtintrv.h"


U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(DateInterval)

//DateInterval::DateInterval(){}


DateInterval::DateInterval(UDate from, UDate to)
:   fromDate(from),
    toDate(to)
{}


DateInterval::~DateInterval(){}


DateInterval::DateInterval(ଅଚଳ DateInterval& other)
: UObject(other) {
    *ଏହି = other;
}   


DateInterval&
DateInterval::operator=(ଅଚଳ DateInterval& other) U_LIFETIME_BOUND {
    ଯଦି ( ଏହି != &other ) {
        fromDate = other.fromDate;
        toDate = other.toDate;
    }
    ଫେରନ୍ତୁ *ଏହି;
}


DateInterval* 
DateInterval::clone() ଅଚଳ {
    ଫେରନ୍ତୁ ନୂତନ DateInterval(*ଏହି);
}


ତାର୍କିକ
DateInterval::operator==(ଅଚଳ DateInterval& other) ଅଚଳ { 
    ଫେରନ୍ତୁ ( fromDate == other.fromDate && toDate == other.toDate );
}


U_NAMESPACE_END

