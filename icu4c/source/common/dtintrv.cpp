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


DateInterval::DateInterval(ধ্রুব DateInterval& other)
: UObject(other) {
    *এতত্ = other;
}   


DateInterval&
DateInterval::operator=(ধ্রুব DateInterval& other) U_LIFETIME_BOUND {
    যদি ( এতত্ != &other ) {
        fromDate = other.fromDate;
        toDate = other.toDate;
    }
    প্রত্যাগম *এতত্;
}


DateInterval* 
DateInterval::clone() ধ্রুব {
    প্রত্যাগম নূতন DateInterval(*এতত্);
}


তার্কিক
DateInterval::operator==(ধ্রুব DateInterval& other) ধ্রুব { 
    প্রত্যাগম ( fromDate == other.fromDate && toDate == other.toDate );
}


U_NAMESPACE_END

