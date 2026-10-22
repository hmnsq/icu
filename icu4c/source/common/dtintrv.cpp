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


DateInterval::DateInterval(अचर DateInterval& other)
: UObject(other) {
    *एतत् = other;
}   


DateInterval&
DateInterval::चालक=(अचर DateInterval& other) U_LIFETIME_BOUND {
    यदि ( एतत् != &other ) {
        fromDate = other.fromDate;
        toDate = other.toDate;
    }
    फर्काऊ *एतत्;
}


DateInterval* 
DateInterval::clone() अचर {
    फर्काऊ नूतन DateInterval(*एतत्);
}


तार्किक
DateInterval::चालक==(अचर DateInterval& other) अचर { 
    फर्काऊ ( fromDate == other.fromDate && toDate == other.toDate );
}


U_NAMESPACE_END

