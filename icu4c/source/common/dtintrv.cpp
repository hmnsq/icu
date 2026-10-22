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



#അന്തര്ഭാവ "unicode/dtintrv.h"


U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(DateInterval)

//DateInterval::DateInterval(){}


DateInterval::DateInterval(UDate from, UDate to)
:   fromDate(from),
    toDate(to)
{}


DateInterval::~DateInterval(){}


DateInterval::DateInterval(സ്ഥിരം DateInterval& other)
: UObject(other) {
    *ഇത് = other;
}   


DateInterval&
DateInterval::operator=(സ്ഥിരം DateInterval& other) U_LIFETIME_BOUND {
    എങ്കിൽ ( ഇത് != &other ) {
        fromDate = other.fromDate;
        toDate = other.toDate;
    }
    തിരികെ *ഇത്;
}


DateInterval* 
DateInterval::clone() സ്ഥിരം {
    തിരികെ പുതിയ DateInterval(*ഇത്);
}


തർക്കം
DateInterval::operator==(സ്ഥിരം DateInterval& other) സ്ഥിരം { 
    തിരികെ ( fromDate == other.fromDate && toDate == other.toDate );
}


U_NAMESPACE_END

