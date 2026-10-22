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

#جيڪڏهن_وصف_نه_ٿيل PARSEPOS_H
#وصف_ڪريو PARSEPOS_H

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

#ضمن "unicode/uobject.h"

 
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

class U_COMMON_API ParsePosition : عام UObject {
عام:
    /**
     * Default constructor, the index starts with 0 as default.
     * @stable ICU 2.0
     */
    ParsePosition()
        : UObject(),
        اشاريو(0),
        error_اشاريو(-1)
      {}

    /**
     * Create a new ParsePosition with the given initial index.
     * @param newIndex the new text offset.
     * @stable ICU 2.0
     */
    ParsePosition(int32_t new_اشاريو)
        : UObject(),
        اشاريو(new_اشاريو),
        error_اشاريو(-1)
      {}

    /**
     * Copy constructor
     * @param copy the object to be copied from.
     * @stable ICU 2.0
     */
    ParsePosition(ثابت ParsePosition& copy)
        : UObject(copy),
        اشاريو(copy.اشاريو),
        error_اشاريو(copy.error_اشاريو)
      {}

    /**
     * Destructor
     * @stable ICU 2.0
     */
    مجازي ~ParsePosition();

    /**
     * Assignment operator
     * @stable ICU 2.0
     */
    مضمن ParsePosition& عامل=(ثابت ParsePosition& copy) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two parse positions are equal, false otherwise.
     * @stable ICU 2.0
     */
    مضمن bool               عامل==(ثابت ParsePosition& that) ثابت;

    /**
     * Equality operator.
     * @return true if the two parse positions are not equal, false otherwise.
     * @stable ICU 2.0
     */
    مضمن bool               عامل!=(ثابت ParsePosition& that) ثابت;

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
    مضمن int32_t get_اشاريو() ثابت;

    /**
     * Set the current parse position.
     * @param index the new index.
     * @stable ICU 2.0
     */
    مضمن فراغ سيٽ_اشاريو(int32_t اشاريو);

    /**
     * Set the index at which a parse error occurred.  Formatters
     * should set this before returning an error code from their
     * parseObject method.  The default value is -1 if this is not
     * set.
     * @stable ICU 2.0
     */
    مضمن فراغ سيٽ_Error_اشاريو(int32_t ei);

    /**
     * Retrieve the index at which an error occurred, or -1 if the
     * error index has not been set.
     * @stable ICU 2.0
     */
    مضمن int32_t get_Error_اشاريو() ثابت;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     *
     * @stable ICU 2.2
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     *
     * @stable ICU 2.2
     */
    مجازي UClassID getDynamicClassID() ثابت override;

خاص:
    /**
     * Input: the place you start parsing.
     * <br>Output: position where the parse stopped.
     * This is designed to be used serially,
     * with each call setting index up for the next one.
     */
    int32_t اشاريو;

    /**
     * The index at which a parse error occurred.
     */
    int32_t error_اشاريو;

};

مضمن ParsePosition&
ParsePosition::عامل=(ثابت ParsePosition& copy) U_LIFETIME_BOUND
{
  اشاريو = copy.اشاريو;
  error_اشاريو = copy.error_اشاريو;
  أرجع *this;
}

مضمن bool
ParsePosition::عامل==(ثابت ParsePosition& copy) ثابت
{
  إذا(اشاريو != copy.اشاريو || error_اشاريو != copy.error_اشاريو)
  أرجع false;
  وإلا
  أرجع true;
}

مضمن bool
ParsePosition::عامل!=(ثابت ParsePosition& copy) ثابت
{
  أرجع !عامل==(copy);
}

مضمن int32_t
ParsePosition::get_اشاريو() ثابت
{
  أرجع اشاريو;
}

مضمن فراغ
ParsePosition::سيٽ_اشاريو(int32_t وٿي)
{
  this->اشاريو = وٿي;
}

مضمن int32_t
ParsePosition::get_Error_اشاريو() ثابت
{
  أرجع error_اشاريو;
}

مضمن فراغ
ParsePosition::سيٽ_Error_اشاريو(int32_t ei)
{
  this->error_اشاريو = ei;
}
U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* U_SHOW_CPLUSPLUS_API */

#جيڪڏهن_پورو
