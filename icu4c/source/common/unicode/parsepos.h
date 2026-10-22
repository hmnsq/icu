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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் U_SHOW_CPLUSPLUS_API

#அந்தர்பாவ "unicode/uobject.h"

 
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

class U_COMMON_API ParsePosition : பொது UObject {
பொது:
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
    ParsePosition(மாறிலி ParsePosition& copy)
        : UObject(copy),
        index(copy.index),
        errorIndex(copy.errorIndex)
      {}

    /**
     * Destructor
     * @stable ICU 2.0
     */
    மெய்நிகர் ~ParsePosition();

    /**
     * Assignment operator
     * @stable ICU 2.0
     */
    உள்வரி ParsePosition& operator=(மாறிலி ParsePosition& copy) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two parse positions are equal, false otherwise.
     * @stable ICU 2.0
     */
    உள்வரி bool               operator==(மாறிலி ParsePosition& that) மாறிலி;

    /**
     * Equality operator.
     * @return true if the two parse positions are not equal, false otherwise.
     * @stable ICU 2.0
     */
    உள்வரி bool               operator!=(மாறிலி ParsePosition& that) மாறிலி;

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
    ParsePosition *clone() மாறிலி;

    /**
     * Retrieve the current parse position.  On input to a parse method, this
     * is the index of the character at which parsing will begin; on output, it
     * is the index of the character following the last character parsed.
     * @return the current index.
     * @stable ICU 2.0
     */
    உள்வரி int32_t getIndex() மாறிலி;

    /**
     * Set the current parse position.
     * @param index the new index.
     * @stable ICU 2.0
     */
    உள்வரி வெறுமை setIndex(int32_t index);

    /**
     * Set the index at which a parse error occurred.  Formatters
     * should set this before returning an error code from their
     * parseObject method.  The default value is -1 if this is not
     * set.
     * @stable ICU 2.0
     */
    உள்வரி வெறுமை setErrorIndex(int32_t ei);

    /**
     * Retrieve the index at which an error occurred, or -1 if the
     * error index has not been set.
     * @stable ICU 2.0
     */
    உள்வரி int32_t getErrorIndex() மாறிலி;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     *
     * @stable ICU 2.2
     */
    நிலையான UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     *
     * @stable ICU 2.2
     */
    மெய்நிகர் UClassID getDynamicClassID() மாறிலி override;

தனிப்பட்ட:
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

உள்வரி ParsePosition&
ParsePosition::operator=(மாறிலி ParsePosition& copy) U_LIFETIME_BOUND
{
  index = copy.index;
  errorIndex = copy.errorIndex;
  திரும்பு *this;
}

உள்வரி bool
ParsePosition::operator==(மாறிலி ParsePosition& copy) மாறிலி
{
  எனில்(index != copy.index || errorIndex != copy.errorIndex)
  திரும்பு false;
  இல்லையெனில்
  திரும்பு true;
}

உள்வரி bool
ParsePosition::operator!=(மாறிலி ParsePosition& copy) மாறிலி
{
  திரும்பு !operator==(copy);
}

உள்வரி int32_t
ParsePosition::getIndex() மாறிலி
{
  திரும்பு index;
}

உள்வரி வெறுமை
ParsePosition::setIndex(int32_t offset)
{
  this->index = offset;
}

உள்வரி int32_t
ParsePosition::getErrorIndex() மாறிலி
{
  திரும்பு errorIndex;
}

உள்வரி வெறுமை
ParsePosition::setErrorIndex(int32_t ei)
{
  this->errorIndex = ei;
}
U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif
