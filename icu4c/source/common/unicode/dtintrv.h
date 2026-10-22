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

#यदि_अपरिभाषितम् __DTINTRV_H__
#परिभाषय __DTINTRV_H__

#अन्तर्भाव "unicode/utypes.h"

#यदि U_SHOW_CPLUSPLUS_API

#अन्तर्भाव "unicode/uobject.h"

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
class U_COMMON_API DateInterval : सार्वजनिक UObject {
सार्वजनिक:

    /** 
     * Construct a DateInterval given a from date and a to date.
     * @param fromDate  The from date in date interval.
     * @param toDate    The to date in date interval.
     * @stable ICU 4.0
     */
    DateInterval(UDate fromDate, UDate गन्तव्यम्_Date);

    /**
     * destructor
     * @stable ICU 4.0
     */
    आभास ~DateInterval();
 
    /** 
     * Get the from date.
     * @return  the from date in dateInterval.
     * @stable ICU 4.0
     */
    अन्तःस्थ UDate getFromDate() ध्रुव;

    /** 
     * Get the to date.
     * @return  the to date in dateInterval.
     * @stable ICU 4.0
     */
    अन्तःस्थ UDate get_गन्तव्यम्_Date() ध्रुव;


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
    स्थिर UClassID U_EXPORT2 getStaticClassID();

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
    आभास UClassID getDynamicClassID() ध्रुव override;

    /**
     * Copy constructor.
     * @stable ICU 4.0
     */
    DateInterval(ध्रुव DateInterval& other);

    /**
     * Default assignment operator
     * @stable ICU 4.0
     */
    DateInterval& संक्रियकः=(ध्रुव DateInterval&) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two DateIntervals are the same
     * @stable ICU 4.0
     */
    आभास bool संक्रियकः==(ध्रुव DateInterval& other) ध्रुव;

    /**
     * Non-equality operator
     * @return true if the two DateIntervals are not the same
     * @stable ICU 4.0
     */
    अन्तःस्थ bool संक्रियकः!=(ध्रुव DateInterval& other) ध्रुव;


    /**
     * clone this object. 
     * The caller owns the result and should delete it when done.
     * @return a cloned DateInterval
     * @stable ICU 4.0
     */
     आभास DateInterval* clone() ध्रुव;

गुप्त:
    /** 
     * Default constructor, not implemented.
     */
    DateInterval() = delete;

    UDate fromDate;
    UDate गन्तव्यम्_Date;

} ;// end class DateInterval


अन्तःस्थ UDate 
DateInterval::getFromDate() ध्रुव { 
    प्रत्यागम fromDate; 
}


अन्तःस्थ UDate 
DateInterval::get_गन्तव्यम्_Date() ध्रुव { 
    प्रत्यागम गन्तव्यम्_Date; 
}


अन्तःस्थ bool
DateInterval::संक्रियकः!=(ध्रुव DateInterval& other) ध्रुव { 
    प्रत्यागम ( !संक्रियकः==(other) );
}


U_NAMESPACE_अन्तः

#यदि_अन्तः /* U_SHOW_CPLUSPLUS_API */

#यदि_अन्तः
