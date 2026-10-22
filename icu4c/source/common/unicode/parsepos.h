// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
* Copyright (C) 1997-2005, International Business Machines Corporation and others. All Rights Reserved.
*******************************************************************************
*
* File PARSEPOS.H
*
* Modification History:
*
*   Date        Name        Description
*   07/09/97    helena      Converted from java.
*   07/17/98    stephen     Added errorIndex support.
*   05/11/99    stephen     Cleaned up.
*******************************************************************************
*/

#اگر_غیر_تعریف_شدہ PARSEPOS_H
#تعریف_کرو PARSEPOS_H

#شامل "unicode/utypes.h"

#اگر U_SHOW_CPLUSPLUS_API

#شامل "unicode/uobject.h"

 
U_NAMESPACE_BEGIN

/**
 * \file
 * \brief C++ API: Canonical Iterator
 */
/** 
 * <code>ParsePosition</code> is a simple class used by <code>Format</code>
 * and its subclasses to keep track of the current position during parsing.
 * The <code>parseObject</code> method in the various <code>Format</code>
 * classes requires a <code>ParsePosition</code> object as an argument.
 *
 * <p>
 * By design, as you parse through a string with different formats,
 * you can use the same <code>ParsePosition</code>, since the index parameter
 * records the current position.
 *
 * The ParsePosition class is not suitable for subclassing.
 *
 * @version     1.3 10/30/97
 * @author      Mark Davis, Helena Shih
 * @see         java.text.Format
 */

class U_COMMON_API ParsePosition : عوامی UObject {
عوامی:
    /**
     * Default constructor, the index starts with 0 as default.
     * @stable ICU 2.0
     */
    ParsePosition()
        : UObject(),
        اشاریہ(0),
        error_اشاریہ(-1)
      {}

    /**
     * Create a new ParsePosition with the given initial index.
     * @param newIndex the new text offset.
     * @stable ICU 2.0
     */
    ParsePosition(int32_t new_اشاریہ)
        : UObject(),
        اشاریہ(new_اشاریہ),
        error_اشاریہ(-1)
      {}

    /**
     * Copy constructor
     * @param copy the object to be copied from.
     * @stable ICU 2.0
     */
    ParsePosition(ثابت ParsePosition& copy)
        : UObject(copy),
        اشاریہ(copy.اشاریہ),
        error_اشاریہ(copy.error_اشاریہ)
      {}

    /**
     * Destructor
     * @stable ICU 2.0
     */
    مجازی ~ParsePosition();

    /**
     * Assignment operator
     * @stable ICU 2.0
     */
    اندرونی ParsePosition& عامل=(ثابت ParsePosition& copy) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two parse positions are equal, false otherwise.
     * @stable ICU 2.0
     */
    اندرونی bool               عامل==(ثابت ParsePosition& that) ثابت;

    /**
     * Equality operator.
     * @return true if the two parse positions are not equal, false otherwise.
     * @stable ICU 2.0
     */
    اندرونی bool               عامل!=(ثابت ParsePosition& that) ثابت;

    /**
     * Clone this object.
     * Clones can be used concurrently in multiple threads.
     * If an error occurs, then nullptr is returned.
     * The caller must delete the clone.
     *
     * @return a clone of this object
     *
     * @see getDynamicClassID
     * @stable ICU 2.8
     */
    ParsePosition *clone() ثابت;

    /**
     * Retrieve the current parse position.  On input to a parse method, this
     * is the index of the character at which parsing will begin; on output, it
     * is the index of the character following the last character parsed.
     * @return the current index.
     * @stable ICU 2.0
     */
    اندرونی int32_t get_اشاریہ() ثابت;

    /**
     * Set the current parse position.
     * @param index the new index.
     * @stable ICU 2.0
     */
    اندرونی خالی سیٹ_اشاریہ(int32_t اشاریہ);

    /**
     * Set the index at which a parse error occurred.  Formatters
     * should set this before returning an error code from their
     * parseObject method.  The default value is -1 if this is not
     * set.
     * @stable ICU 2.0
     */
    اندرونی خالی سیٹ_Error_اشاریہ(int32_t ei);

    /**
     * Retrieve the index at which an error occurred, or -1 if the
     * error index has not been set.
     * @stable ICU 2.0
     */
    اندرونی int32_t get_Error_اشاریہ() ثابت;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     *
     * @stable ICU 2.2
     */
    ساکن UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     *
     * @stable ICU 2.2
     */
    مجازی UClassID getDynamicClassID() ثابت override;

نجی:
    /**
     * Input: the place you start parsing.
     * <br>Output: position where the parse stopped.
     * This is designed to be used serially,
     * with each call setting index up for the next one.
     */
    int32_t اشاریہ;

    /**
     * The index at which a parse error occurred.
     */
    int32_t error_اشاریہ;

};

اندرونی ParsePosition&
ParsePosition::عامل=(ثابت ParsePosition& copy) U_LIFETIME_BOUND
{
  اشاریہ = copy.اشاریہ;
  error_اشاریہ = copy.error_اشاریہ;
  واپس *this;
}

اندرونی bool
ParsePosition::عامل==(ثابت ParsePosition& copy) ثابت
{
  اگر(اشاریہ != copy.اشاریہ || error_اشاریہ != copy.error_اشاریہ)
  واپس false;
  ورنہ
  واپس true;
}

اندرونی bool
ParsePosition::عامل!=(ثابت ParsePosition& copy) ثابت
{
  واپس !عامل==(copy);
}

اندرونی int32_t
ParsePosition::get_اشاریہ() ثابت
{
  واپس اشاریہ;
}

اندرونی خالی
ParsePosition::سیٹ_اشاریہ(int32_t انحراف)
{
  this->اشاریہ = انحراف;
}

اندرونی int32_t
ParsePosition::get_Error_اشاریہ() ثابت
{
  واپس error_اشاریہ;
}

اندرونی خالی
ParsePosition::سیٹ_Error_اشاریہ(int32_t ei)
{
  this->error_اشاریہ = ei;
}
U_NAMESPACE_انتہا

#اگر_ختم /* U_SHOW_CPLUSPLUS_API */

#اگر_ختم
