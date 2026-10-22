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

#که_نه_تعریف_شوی __DTINTRV_H__
#تعریف_کړه __DTINTRV_H__

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

#ضمن "unicode/uobject.h"

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
class U_COMMON_API DateInterval : عام UObject {
عام:

    /** 
     * Construct a DateInterval given a from date and a to date.
     * @param fromDate  The from date in date interval.
     * @param toDate    The to date in date interval.
     * @stable ICU 4.0
     */
    DateInterval(UDate fromDate, UDate منزل_Date);

    /**
     * destructor
     * @stable ICU 4.0
     */
    مجازي ~DateInterval();
 
    /** 
     * Get the from date.
     * @return  the from date in dateInterval.
     * @stable ICU 4.0
     */
    مضمن UDate getFromDate() ثابت;

    /** 
     * Get the to date.
     * @return  the to date in dateInterval.
     * @stable ICU 4.0
     */
    مضمن UDate get_منزل_Date() ثابت;


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
    ساكن UClassID U_EXPORT2 getStaticClassID();

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
    مجازي UClassID getDynamicClassID() ثابت override;

    /**
     * Copy constructor.
     * @stable ICU 4.0
     */
    DateInterval(ثابت DateInterval& other);

    /**
     * Default assignment operator
     * @stable ICU 4.0
     */
    DateInterval& چلوونکی=(ثابت DateInterval&) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two DateIntervals are the same
     * @stable ICU 4.0
     */
    مجازي bool چلوونکی==(ثابت DateInterval& other) ثابت;

    /**
     * Non-equality operator
     * @return true if the two DateIntervals are not the same
     * @stable ICU 4.0
     */
    مضمن bool چلوونکی!=(ثابت DateInterval& other) ثابت;


    /**
     * clone this object. 
     * The caller owns the result and should delete it when done.
     * @return a cloned DateInterval
     * @stable ICU 4.0
     */
     مجازي DateInterval* clone() ثابت;

خاص:
    /** 
     * Default constructor, not implemented.
     */
    DateInterval() = delete;

    UDate fromDate;
    UDate منزل_Date;

} ;// end class DateInterval


مضمن UDate 
DateInterval::getFromDate() ثابت { 
    أرجع fromDate; 
}


مضمن UDate 
DateInterval::get_منزل_Date() ثابت { 
    أرجع منزل_Date; 
}


مضمن bool
DateInterval::چلوونکی!=(ثابت DateInterval& other) ثابت { 
    أرجع ( !چلوونکی==(other) );
}


U_NAMESPACE_پای_ټکی

#د_که_پای /* U_SHOW_CPLUSPLUS_API */

#د_که_پای
