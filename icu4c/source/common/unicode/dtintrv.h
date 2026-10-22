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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் U_SHOW_CPLUSPLUS_API

#அந்தர்பாவ "unicode/uobject.h"

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
class U_COMMON_API DateInterval : பொது UObject {
பொது:

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
    மெய்நிகர் ~DateInterval();
 
    /** 
     * Get the from date.
     * @return  the from date in dateInterval.
     * @stable ICU 4.0
     */
    உள்வரி UDate getFromDate() மாறிலி;

    /** 
     * Get the to date.
     * @return  the to date in dateInterval.
     * @stable ICU 4.0
     */
    உள்வரி UDate getToDate() மாறிலி;


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
    நிலையான UClassID U_EXPORT2 getStaticClassID();

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
    மெய்நிகர் UClassID getDynamicClassID() மாறிலி override;

    /**
     * Copy constructor.
     * @stable ICU 4.0
     */
    DateInterval(மாறிலி DateInterval& other);

    /**
     * Default assignment operator
     * @stable ICU 4.0
     */
    DateInterval& operator=(மாறிலி DateInterval&) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two DateIntervals are the same
     * @stable ICU 4.0
     */
    மெய்நிகர் bool operator==(மாறிலி DateInterval& other) மாறிலி;

    /**
     * Non-equality operator
     * @return true if the two DateIntervals are not the same
     * @stable ICU 4.0
     */
    உள்வரி bool operator!=(மாறிலி DateInterval& other) மாறிலி;


    /**
     * clone this object. 
     * The caller owns the result and should delete it when done.
     * @return a cloned DateInterval
     * @stable ICU 4.0
     */
     மெய்நிகர் DateInterval* clone() மாறிலி;

தனிப்பட்ட:
    /** 
     * Default constructor, not implemented.
     */
    DateInterval() = delete;

    UDate fromDate;
    UDate toDate;

} ;// end class DateInterval


உள்வரி UDate 
DateInterval::getFromDate() மாறிலி { 
    திரும்பு fromDate; 
}


உள்வரி UDate 
DateInterval::getToDate() மாறிலி { 
    திரும்பு toDate; 
}


உள்வரி bool
DateInterval::operator!=(மாறிலி DateInterval& other) மாறிலி { 
    திரும்பு ( !operator==(other) );
}


U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif
