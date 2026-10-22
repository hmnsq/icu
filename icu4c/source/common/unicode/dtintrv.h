// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2008-2009, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*
* File DTINTRV.H 
*
*******************************************************************************
*/

#ifndef __DTINTRV_H__
#define __DTINTRV_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ U_SHOW_CPLUSPLUS_API

#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: Date Interval data type
 */

U_NAMESPACE_BEGIN


/**
 * This class represents a date interval.
 * It is a pair of UDate representing from UDate 1 to UDate 2.
 * @stable ICU 4.0
**/
class U_COMMON_API DateInterval : ಸಾರ್ವಜನಿಕ UObject {
ಸಾರ್ವಜನಿಕ:

    /** 
     * Construct a DateInterval given a from date and a to date.
     * @param fromDate  The from date in date interval.
     * @param toDate    The to date in date interval.
     * @stable ICU 4.0
     */
    DateInterval(UDate fromDate, UDate toDate);

    /**
     * destructor
     * @stable ICU 4.0
     */
    ಆಭಾಸ ~DateInterval();
 
    /** 
     * Get the from date.
     * @return  the from date in dateInterval.
     * @stable ICU 4.0
     */
    ಅನ್ತಃಸ್ಥ UDate getFromDate() ಧ್ರುವ;

    /** 
     * Get the to date.
     * @return  the to date in dateInterval.
     * @stable ICU 4.0
     */
    ಅನ್ತಃಸ್ಥ UDate getToDate() ಧ್ರುವ;


    /**
     * Return the class ID for this class. This is useful only for comparing to
     * a return value from getDynamicClassID(). For example:
     * <pre>
     * .   Base* polymorphic_pointer = createPolymorphicObject();
     * .   if (polymorphic_pointer->getDynamicClassID() ==
     * .       derived::getStaticClassID()) ...
     * </pre>
     * @return          The class ID for all objects of this class.
     * @stable ICU 4.0
     */
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();

    /**
     * Returns a unique class ID POLYMORPHICALLY. Pure virtual override. This
     * method is to implement a simple version of RTTI, since not all C++
     * compilers support genuine RTTI. Polymorphic operator==() and clone()
     * methods call this method.
     *
     * @return          The class ID for this object. All objects of a
     *                  given class have the same class ID.  Objects of
     *                  other classes have different class IDs.
     * @stable ICU 4.0
     */
    ಆಭಾಸ UClassID getDynamicClassID() ಧ್ರುವ override;

    /**
     * Copy constructor.
     * @stable ICU 4.0
     */
    DateInterval(ಧ್ರುವ DateInterval& other);

    /**
     * Default assignment operator
     * @stable ICU 4.0
     */
    DateInterval& operator=(ಧ್ರುವ DateInterval&) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two DateIntervals are the same
     * @stable ICU 4.0
     */
    ಆಭಾಸ bool operator==(ಧ್ರುವ DateInterval& other) ಧ್ರುವ;

    /**
     * Non-equality operator
     * @return true if the two DateIntervals are not the same
     * @stable ICU 4.0
     */
    ಅನ್ತಃಸ್ಥ bool operator!=(ಧ್ರುವ DateInterval& other) ಧ್ರುವ;


    /**
     * clone this object. 
     * The caller owns the result and should delete it when done.
     * @return a cloned DateInterval
     * @stable ICU 4.0
     */
     ಆಭಾಸ DateInterval* clone() ಧ್ರುವ;

ಗುಪ್ತ:
    /** 
     * Default constructor, not implemented.
     */
    DateInterval() = delete;

    UDate fromDate;
    UDate toDate;

} ;// end class DateInterval


ಅನ್ತಃಸ್ಥ UDate 
DateInterval::getFromDate() ಧ್ರುವ { 
    ಪ್ರತ್ಯಾಗಮ fromDate; 
}


ಅನ್ತಃಸ್ಥ UDate 
DateInterval::getToDate() ಧ್ರುವ { 
    ಪ್ರತ್ಯಾಗಮ toDate; 
}


ಅನ್ತಃಸ್ಥ bool
DateInterval::operator!=(ಧ್ರುವ DateInterval& other) ಧ್ರುವ { 
    ಪ್ರತ್ಯಾಗಮ ( !operator==(other) );
}


U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif
