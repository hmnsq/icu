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


DateInterval::DateInterval(UDate from, UDate ಗಮ್ಯ)
:   fromDate(from),
    ಗಮ್ಯ_Date(ಗಮ್ಯ)
{}


DateInterval::~DateInterval(){}


DateInterval::DateInterval(ಸ್ಥಿರಾಂಕ DateInterval& other)
: UObject(other) {
    *ಇದು = other;
}   


DateInterval&
DateInterval::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ DateInterval& other) U_LIFETIME_BOUND {
    ಆದರೆ ( ಇದು != &other ) {
        fromDate = other.fromDate;
        ಗಮ್ಯ_Date = other.ಗಮ್ಯ_Date;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}


DateInterval* 
DateInterval::clone() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಹೊಸ DateInterval(*ಇದು);
}


ತಾರ್ಕಿಕ
DateInterval::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ DateInterval& other) ಸ್ಥಿರಾಂಕ { 
    ಹಿಂತಿರುಗಿ ( fromDate == other.fromDate && ಗಮ್ಯ_Date == other.ಗಮ್ಯ_Date );
}


U_NAMESPACE_ಕೊನೆ

