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



#شامل "unicode/dtintrv.h"


U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(DateInterval)

//DateInterval::DateInterval(){}


DateInterval::DateInterval(UDate from, UDate مقصد)
:   fromDate(from),
    مقصد_Date(مقصد)
{}


DateInterval::~DateInterval(){}


DateInterval::DateInterval(ثابت DateInterval& other)
: UObject(other) {
    *یہ = other;
}   


DateInterval&
DateInterval::عامل=(ثابت DateInterval& other) U_LIFETIME_BOUND {
    اگر ( یہ != &other ) {
        fromDate = other.fromDate;
        مقصد_Date = other.مقصد_Date;
    }
    واپس *یہ;
}


DateInterval* 
DateInterval::clone() ثابت {
    واپس نیا DateInterval(*یہ);
}


منطقی
DateInterval::عامل==(ثابت DateInterval& other) ثابت { 
    واپس ( fromDate == other.fromDate && مقصد_Date == other.مقصد_Date );
}


U_NAMESPACE_انتہا

