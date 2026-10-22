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

#ifndef PARSEPOS_H
#define PARSEPOS_H

#অন্তর্ভাব "unicode/utypes.h"

#যদি U_SHOW_CPLUSPLUS_API

#অন্তর্ভাব "unicode/uobject.h"

 
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

class U_COMMON_API ParsePosition : সর্বজনীন UObject {
সর্বজনীন:
    /**
     * Default constructor, the index starts with 0 as default.
     * @stable ICU 2.0
     */
    ParsePosition()
        : UObject(),
        সূচক(0),
        error_সূচক(-1)
      {}

    /**
     * Create a new ParsePosition with the given initial index.
     * @param newIndex the new text offset.
     * @stable ICU 2.0
     */
    ParsePosition(int32_t new_সূচক)
        : UObject(),
        সূচক(new_সূচক),
        error_সূচক(-1)
      {}

    /**
     * Copy constructor
     * @param copy the object to be copied from.
     * @stable ICU 2.0
     */
    ParsePosition(ধ্রুবক ParsePosition& copy)
        : UObject(copy),
        সূচক(copy.সূচক),
        error_সূচক(copy.error_সূচক)
      {}

    /**
     * Destructor
     * @stable ICU 2.0
     */
    আভাসী ~ParsePosition();

    /**
     * Assignment operator
     * @stable ICU 2.0
     */
    অন্তঃরেখ ParsePosition& operator=(ধ্রুবক ParsePosition& copy) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two parse positions are equal, false otherwise.
     * @stable ICU 2.0
     */
    অন্তঃরেখ bool               operator==(ধ্রুবক ParsePosition& that) ধ্রুবক;

    /**
     * Equality operator.
     * @return true if the two parse positions are not equal, false otherwise.
     * @stable ICU 2.0
     */
    অন্তঃরেখ bool               operator!=(ধ্রুবক ParsePosition& that) ধ্রুবক;

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
    ParsePosition *clone() ধ্রুবক;

    /**
     * Retrieve the current parse position.  On input to a parse method, this
     * is the index of the character at which parsing will begin; on output, it
     * is the index of the character following the last character parsed.
     * @return the current index.
     * @stable ICU 2.0
     */
    অন্তঃরেখ int32_t get_সূচক() ধ্রুবক;

    /**
     * Set the current parse position.
     * @param index the new index.
     * @stable ICU 2.0
     */
    অন্তঃরেখ শূন্য set_সূচক(int32_t সূচক);

    /**
     * Set the index at which a parse error occurred.  Formatters
     * should set this before returning an error code from their
     * parseObject method.  The default value is -1 if this is not
     * set.
     * @stable ICU 2.0
     */
    অন্তঃরেখ শূন্য set_Error_সূচক(int32_t ei);

    /**
     * Retrieve the index at which an error occurred, or -1 if the
     * error index has not been set.
     * @stable ICU 2.0
     */
    অন্তঃরেখ int32_t get_Error_সূচক() ধ্রুবক;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     *
     * @stable ICU 2.2
     */
    স্থির UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     *
     * @stable ICU 2.2
     */
    আভাসী UClassID getDynamicClassID() ধ্রুবক override;

ব্যক্তিগত:
    /**
     * Input: the place you start parsing.
     * <br>Output: position where the parse stopped.
     * This is designed to be used serially,
     * with each call setting index up for the next one.
     */
    int32_t সূচক;

    /**
     * The index at which a parse error occurred.
     */
    int32_t error_সূচক;

};

অন্তঃরেখ ParsePosition&
ParsePosition::operator=(ধ্রুবক ParsePosition& copy) U_LIFETIME_BOUND
{
  সূচক = copy.সূচক;
  error_সূচক = copy.error_সূচক;
  ফেরত *this;
}

অন্তঃরেখ bool
ParsePosition::operator==(ধ্রুবক ParsePosition& copy) ধ্রুবক
{
  যদি(সূচক != copy.সূচক || error_সূচক != copy.error_সূচক)
  ফেরত false;
  অন্যথা
  ফেরত true;
}

অন্তঃরেখ bool
ParsePosition::operator!=(ধ্রুবক ParsePosition& copy) ধ্রুবক
{
  ফেরত !operator==(copy);
}

অন্তঃরেখ int32_t
ParsePosition::get_সূচক() ধ্রুবক
{
  ফেরত সূচক;
}

অন্তঃরেখ শূন্য
ParsePosition::set_সূচক(int32_t offset)
{
  this->সূচক = offset;
}

অন্তঃরেখ int32_t
ParsePosition::get_Error_সূচক() ধ্রুবক
{
  ফেরত error_সূচক;
}

অন্তঃরেখ শূন্য
ParsePosition::set_Error_সূচক(int32_t ei)
{
  this->error_সূচক = ei;
}
U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif
