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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି U_SHOW_CPLUSPLUS_API

#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"

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
class U_COMMON_API DateInterval : ସର୍ବସାଧାରଣ UObject {
ସର୍ବସାଧାରଣ:

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
    ଆଭାସୀ ~DateInterval();
 
    /** 
     * Get the from date.
     * @return  the from date in dateInterval.
     * @stable ICU 4.0
     */
    ଧାଡ଼ିଭିତରେ UDate getFromDate() ଅଚଳ;

    /** 
     * Get the to date.
     * @return  the to date in dateInterval.
     * @stable ICU 4.0
     */
    ଧାଡ଼ିଭିତରେ UDate getToDate() ଅଚଳ;


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
    ସ୍ଥିର UClassID U_EXPORT2 getStaticClassID();

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
    ଆଭାସୀ UClassID getDynamicClassID() ଅଚଳ override;

    /**
     * Copy constructor.
     * @stable ICU 4.0
     */
    DateInterval(ଅଚଳ DateInterval& other);

    /**
     * Default assignment operator
     * @stable ICU 4.0
     */
    DateInterval& operator=(ଅଚଳ DateInterval&) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two DateIntervals are the same
     * @stable ICU 4.0
     */
    ଆଭାସୀ bool operator==(ଅଚଳ DateInterval& other) ଅଚଳ;

    /**
     * Non-equality operator
     * @return true if the two DateIntervals are not the same
     * @stable ICU 4.0
     */
    ଧାଡ଼ିଭିତରେ bool operator!=(ଅଚଳ DateInterval& other) ଅଚଳ;


    /**
     * clone this object. 
     * The caller owns the result and should delete it when done.
     * @return a cloned DateInterval
     * @stable ICU 4.0
     */
     ଆଭାସୀ DateInterval* clone() ଅଚଳ;

ବ୍ୟକ୍ତିଗତ:
    /** 
     * Default constructor, not implemented.
     */
    DateInterval() = delete;

    UDate fromDate;
    UDate toDate;

} ;// end class DateInterval


ଧାଡ଼ିଭିତରେ UDate 
DateInterval::getFromDate() ଅଚଳ { 
    ଫେରନ୍ତୁ fromDate; 
}


ଧାଡ଼ିଭିତରେ UDate 
DateInterval::getToDate() ଅଚଳ { 
    ଫେରନ୍ତୁ toDate; 
}


ଧାଡ଼ିଭିତରେ bool
DateInterval::operator!=(ଅଚଳ DateInterval& other) ଅଚଳ { 
    ଫେରନ୍ତୁ ( !operator==(other) );
}


U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif
