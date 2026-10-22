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



#include "unicode/dtintrv.h"


U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(DateInterval)

//DateInterval::DateInterval(){}


DateInterval::DateInterval(UDate from, UDate to)
:   fromDate(from),
    toDate(to)
{}


DateInterval::~DateInterval(){}


DateInterval::DateInterval(const DateInterval& other)
: UObject(other) {
    *this = other;
}   


DateInterval&
DateInterval::operator=(const DateInterval& other) U_LIFETIME_BOUND {
    もし ( this != &other ) {
        fromDate = other.fromDate;
        toDate = other.toDate;
    }
    戻る *this;
}


DateInterval* 
DateInterval::clone() const {
    戻る new DateInterval(*this);
}


bool
DateInterval::operator==(const DateInterval& other) const { 
    戻る ( fromDate == other.fromDate && toDate == other.toDate );
}


U_NAMESPACE_END

