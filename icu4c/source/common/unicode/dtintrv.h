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

#অন্তর্ভাব "unicode/utypes.h"

#যদি U_SHOW_CPLUSPLUS_API

#অন্তর্ভাব "unicode/uobject.h"

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
class U_COMMON_API DateInterval : সার্বজনিক UObject {
সার্বজনিক:

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
    আভাস ~DateInterval();
 
    /** 
     * Get the from date.
     * @return  the from date in dateInterval.
     * @stable ICU 4.0
     */
    অন্তঃস্থ UDate getFromDate() ধ্রুব;

    /** 
     * Get the to date.
     * @return  the to date in dateInterval.
     * @stable ICU 4.0
     */
    অন্তঃস্থ UDate getToDate() ধ্রুব;


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
    স্থির UClassID U_EXPORT2 getStaticClassID();

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
    আভাস UClassID getDynamicClassID() ধ্রুব override;

    /**
     * Copy constructor.
     * @stable ICU 4.0
     */
    DateInterval(ধ্রুব DateInterval& other);

    /**
     * Default assignment operator
     * @stable ICU 4.0
     */
    DateInterval& operator=(ধ্রুব DateInterval&) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two DateIntervals are the same
     * @stable ICU 4.0
     */
    আভাস bool operator==(ধ্রুব DateInterval& other) ধ্রুব;

    /**
     * Non-equality operator
     * @return true if the two DateIntervals are not the same
     * @stable ICU 4.0
     */
    অন্তঃস্থ bool operator!=(ধ্রুব DateInterval& other) ধ্রুব;


    /**
     * clone this object. 
     * The caller owns the result and should delete it when done.
     * @return a cloned DateInterval
     * @stable ICU 4.0
     */
     আভাস DateInterval* clone() ধ্রুব;

গুপ্ত:
    /** 
     * Default constructor, not implemented.
     */
    DateInterval() = delete;

    UDate fromDate;
    UDate toDate;

} ;// end class DateInterval


অন্তঃস্থ UDate 
DateInterval::getFromDate() ধ্রুব { 
    প্রত্যাগম fromDate; 
}


অন্তঃস্থ UDate 
DateInterval::getToDate() ধ্রুব { 
    প্রত্যাগম toDate; 
}


অন্তঃস্থ bool
DateInterval::operator!=(ধ্রুব DateInterval& other) ধ্রুব { 
    প্রত্যাগম ( !operator==(other) );
}


U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif
