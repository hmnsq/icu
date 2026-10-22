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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ U_SHOW_CPLUSPLUS_API

#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"

 
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

class U_COMMON_API ParsePosition : ಸಾರ್ವಜನಿಕ UObject {
ಸಾರ್ವಜನಿಕ:
    /**
     * Default constructor, the index starts with 0 as default.
     * @stable ICU 2.0
     */
    ParsePosition()
        : UObject(),
        index(0),
        errorIndex(-1)
      {}

    /**
     * Create a new ParsePosition with the given initial index.
     * @param newIndex the new text offset.
     * @stable ICU 2.0
     */
    ParsePosition(int32_t newIndex)
        : UObject(),
        index(newIndex),
        errorIndex(-1)
      {}

    /**
     * Copy constructor
     * @param copy the object to be copied from.
     * @stable ICU 2.0
     */
    ParsePosition(ಧ್ರುವ ParsePosition& copy)
        : UObject(copy),
        index(copy.index),
        errorIndex(copy.errorIndex)
      {}

    /**
     * Destructor
     * @stable ICU 2.0
     */
    ಆಭಾಸ ~ParsePosition();

    /**
     * Assignment operator
     * @stable ICU 2.0
     */
    ಅನ್ತಃಸ್ಥ ParsePosition& operator=(ಧ್ರುವ ParsePosition& copy) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two parse positions are equal, false otherwise.
     * @stable ICU 2.0
     */
    ಅನ್ತಃಸ್ಥ bool               operator==(ಧ್ರುವ ParsePosition& that) ಧ್ರುವ;

    /**
     * Equality operator.
     * @return true if the two parse positions are not equal, false otherwise.
     * @stable ICU 2.0
     */
    ಅನ್ತಃಸ್ಥ bool               operator!=(ಧ್ರುವ ParsePosition& that) ಧ್ರುವ;

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
    ParsePosition *clone() ಧ್ರುವ;

    /**
     * Retrieve the current parse position.  On input to a parse method, this
     * is the index of the character at which parsing will begin; on output, it
     * is the index of the character following the last character parsed.
     * @return the current index.
     * @stable ICU 2.0
     */
    ಅನ್ತಃಸ್ಥ int32_t getIndex() ಧ್ರುವ;

    /**
     * Set the current parse position.
     * @param index the new index.
     * @stable ICU 2.0
     */
    ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ setIndex(int32_t index);

    /**
     * Set the index at which a parse error occurred.  Formatters
     * should set this before returning an error code from their
     * parseObject method.  The default value is -1 if this is not
     * set.
     * @stable ICU 2.0
     */
    ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ setErrorIndex(int32_t ei);

    /**
     * Retrieve the index at which an error occurred, or -1 if the
     * error index has not been set.
     * @stable ICU 2.0
     */
    ಅನ್ತಃಸ್ಥ int32_t getErrorIndex() ಧ್ರುವ;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     *
     * @stable ICU 2.2
     */
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     *
     * @stable ICU 2.2
     */
    ಆಭಾಸ UClassID getDynamicClassID() ಧ್ರುವ override;

ಗುಪ್ತ:
    /**
     * Input: the place you start parsing.
     * <br>Output: position where the parse stopped.
     * This is designed to be used serially,
     * with each call setting index up for the next one.
     */
    int32_t index;

    /**
     * The index at which a parse error occurred.
     */
    int32_t errorIndex;

};

ಅನ್ತಃಸ್ಥ ParsePosition&
ParsePosition::operator=(ಧ್ರುವ ParsePosition& copy) U_LIFETIME_BOUND
{
  index = copy.index;
  errorIndex = copy.errorIndex;
  ಪ್ರತ್ಯಾಗಮ *this;
}

ಅನ್ತಃಸ್ಥ bool
ParsePosition::operator==(ಧ್ರುವ ParsePosition& copy) ಧ್ರುವ
{
  ಯದಿ(index != copy.index || errorIndex != copy.errorIndex)
  ಪ್ರತ್ಯಾಗಮ false;
  ಅನ್ಯಥಾ
  ಪ್ರತ್ಯಾಗಮ true;
}

ಅನ್ತಃಸ್ಥ bool
ParsePosition::operator!=(ಧ್ರುವ ParsePosition& copy) ಧ್ರುವ
{
  ಪ್ರತ್ಯಾಗಮ !operator==(copy);
}

ಅನ್ತಃಸ್ಥ int32_t
ParsePosition::getIndex() ಧ್ರುವ
{
  ಪ್ರತ್ಯಾಗಮ index;
}

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ
ParsePosition::setIndex(int32_t offset)
{
  this->index = offset;
}

ಅನ್ತಃಸ್ಥ int32_t
ParsePosition::getErrorIndex() ಧ್ರುವ
{
  ಪ್ರತ್ಯಾಗಮ errorIndex;
}

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ
ParsePosition::setErrorIndex(int32_t ei)
{
  this->errorIndex = ei;
}
U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif
