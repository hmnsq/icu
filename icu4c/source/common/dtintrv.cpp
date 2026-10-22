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



#অন্তর্ভাব "unicode/dtintrv.h"


U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(DateInterval)

//DateInterval::DateInterval(){}


DateInterval::DateInterval(UDate from, UDate to)
:   fromDate(from),
    toDate(to)
{}


DateInterval::~DateInterval(){}


DateInterval::DateInterval(ধ্রুবক DateInterval& other)
: UObject(other) {
    *এই = other;
}   


DateInterval&
DateInterval::operator=(ধ্রুবক DateInterval& other) U_LIFETIME_BOUND {
    যদি ( এই != &other ) {
        fromDate = other.fromDate;
        toDate = other.toDate;
    }
    ফেরত *এই;
}


DateInterval* 
DateInterval::clone() ধ্রুবক {
    ফেরত নতুন DateInterval(*এই);
}


যৌক্তিক
DateInterval::operator==(ধ্রুবক DateInterval& other) ধ্রুবক { 
    ফেরত ( fromDate == other.fromDate && toDate == other.toDate );
}


U_NAMESPACE_END

