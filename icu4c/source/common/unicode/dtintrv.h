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

#నిర్వచితం_కాకపోతే __DTINTRV_H__
#నిర్వచించు __DTINTRV_H__

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API

#అన్తర్భావ "unicode/uobject.h"

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
class U_COMMON_API DateInterval : బహిరంగ UObject {
బహిరంగ:

    /** 
     * Construct a DateInterval given a from date and a to date.
     * @param fromDate  The from date in date interval.
     * @param toDate    The to date in date interval.
     * @stable ICU 4.0
     */
    DateInterval(UDate fromDate, UDate గమ్యం_Date);

    /**
     * destructor
     * @stable ICU 4.0
     */
    మాయా ~DateInterval();
 
    /** 
     * Get the from date.
     * @return  the from date in dateInterval.
     * @stable ICU 4.0
     */
    అంతఃవరుస UDate getFromDate() స్థిరరాశి;

    /** 
     * Get the to date.
     * @return  the to date in dateInterval.
     * @stable ICU 4.0
     */
    అంతఃవరుస UDate get_గమ్యం_Date() స్థిరరాశి;


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
    స్థిర UClassID U_EXPORT2 getStaticClassID();

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
    మాయా UClassID getDynamicClassID() స్థిరరాశి override;

    /**
     * Copy constructor.
     * @stable ICU 4.0
     */
    DateInterval(స్థిరరాశి DateInterval& other);

    /**
     * Default assignment operator
     * @stable ICU 4.0
     */
    DateInterval& క్రియాకారకం=(స్థిరరాశి DateInterval&) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two DateIntervals are the same
     * @stable ICU 4.0
     */
    మాయా bool క్రియాకారకం==(స్థిరరాశి DateInterval& other) స్థిరరాశి;

    /**
     * Non-equality operator
     * @return true if the two DateIntervals are not the same
     * @stable ICU 4.0
     */
    అంతఃవరుస bool క్రియాకారకం!=(స్థిరరాశి DateInterval& other) స్థిరరాశి;


    /**
     * clone this object. 
     * The caller owns the result and should delete it when done.
     * @return a cloned DateInterval
     * @stable ICU 4.0
     */
     మాయా DateInterval* clone() స్థిరరాశి;

అంతరంగ:
    /** 
     * Default constructor, not implemented.
     */
    DateInterval() = delete;

    UDate fromDate;
    UDate గమ్యం_Date;

} ;// end class DateInterval


అంతఃవరుస UDate 
DateInterval::getFromDate() స్థిరరాశి { 
    తిరిగి fromDate; 
}


అంతఃవరుస UDate 
DateInterval::get_గమ్యం_Date() స్థిరరాశి { 
    తిరిగి గమ్యం_Date; 
}


అంతఃవరుస bool
DateInterval::క్రియాకారకం!=(స్థిరరాశి DateInterval& other) స్థిరరాశి { 
    తిరిగి ( !క్రియాకారకం==(other) );
}


U_NAMESPACE_చివర

#అయితే_ముగింపు /* U_SHOW_CPLUSPLUS_API */

#అయితే_ముగింపు
