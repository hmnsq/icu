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



#అన్తర్భావ "unicode/dtintrv.h"


U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(DateInterval)

//DateInterval::DateInterval(){}


DateInterval::DateInterval(UDate from, UDate గమ్యం)
:   fromDate(from),
    గమ్యం_Date(గమ్యం)
{}


DateInterval::~DateInterval(){}


DateInterval::DateInterval(స్థిరరాశి DateInterval& other)
: UObject(other) {
    *ఇది = other;
}   


DateInterval&
DateInterval::క్రియాకారకం=(స్థిరరాశి DateInterval& other) U_LIFETIME_BOUND {
    ఒకవేళ ( ఇది != &other ) {
        fromDate = other.fromDate;
        గమ్యం_Date = other.గమ్యం_Date;
    }
    తిరిగి *ఇది;
}


DateInterval* 
DateInterval::clone() స్థిరరాశి {
    తిరిగి కొత్త DateInterval(*ఇది);
}


తార్కిక
DateInterval::క్రియాకారకం==(స్థిరరాశి DateInterval& other) స్థిరరాశి { 
    తిరిగి ( fromDate == other.fromDate && గమ్యం_Date == other.గమ్యం_Date );
}


U_NAMESPACE_చివర

