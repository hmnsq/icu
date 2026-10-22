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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __DTINTRV_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __DTINTRV_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ U_SHOW_CPLUSPLUS_API

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"

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
class U_COMMON_API DateInterval : ਜਨਤਕ UObject {
ਜਨਤਕ:

    /** 
     * Construct a DateInterval given a from date and a to date.
     * @param fromDate  The from date in date interval.
     * @param toDate    The to date in date interval.
     * @stable ICU 4.0
     */
    DateInterval(UDate fromDate, UDate ਮੰਜ਼ਿਲ_Date);

    /**
     * destructor
     * @stable ICU 4.0
     */
    ਆਭਾਸੀ ~DateInterval();
 
    /** 
     * Get the from date.
     * @return  the from date in dateInterval.
     * @stable ICU 4.0
     */
    ਸਤਰਅੰਦਰ UDate getFromDate() ਅਚਲ;

    /** 
     * Get the to date.
     * @return  the to date in dateInterval.
     * @stable ICU 4.0
     */
    ਸਤਰਅੰਦਰ UDate get_ਮੰਜ਼ਿਲ_Date() ਅਚਲ;


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
    ਸਥਿਰ UClassID U_EXPORT2 getStaticClassID();

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
    ਆਭਾਸੀ UClassID getDynamicClassID() ਅਚਲ override;

    /**
     * Copy constructor.
     * @stable ICU 4.0
     */
    DateInterval(ਅਚਲ DateInterval& other);

    /**
     * Default assignment operator
     * @stable ICU 4.0
     */
    DateInterval& ਸੰਚਾਲਕ=(ਅਚਲ DateInterval&) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two DateIntervals are the same
     * @stable ICU 4.0
     */
    ਆਭਾਸੀ bool ਸੰਚਾਲਕ==(ਅਚਲ DateInterval& other) ਅਚਲ;

    /**
     * Non-equality operator
     * @return true if the two DateIntervals are not the same
     * @stable ICU 4.0
     */
    ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ!=(ਅਚਲ DateInterval& other) ਅਚਲ;


    /**
     * clone this object. 
     * The caller owns the result and should delete it when done.
     * @return a cloned DateInterval
     * @stable ICU 4.0
     */
     ਆਭਾਸੀ DateInterval* clone() ਅਚਲ;

ਨਿਜੀ:
    /** 
     * Default constructor, not implemented.
     */
    DateInterval() = delete;

    UDate fromDate;
    UDate ਮੰਜ਼ਿਲ_Date;

} ;// end class DateInterval


ਸਤਰਅੰਦਰ UDate 
DateInterval::getFromDate() ਅਚਲ { 
    ਵਾਪਸ fromDate; 
}


ਸਤਰਅੰਦਰ UDate 
DateInterval::get_ਮੰਜ਼ਿਲ_Date() ਅਚਲ { 
    ਵਾਪਸ ਮੰਜ਼ਿਲ_Date; 
}


ਸਤਰਅੰਦਰ bool
DateInterval::ਸੰਚਾਲਕ!=(ਅਚਲ DateInterval& other) ਅਚਲ { 
    ਵਾਪਸ ( !ਸੰਚਾਲਕ==(other) );
}


U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* U_SHOW_CPLUSPLUS_API */

#ਜੇ_ਅੰਤ
