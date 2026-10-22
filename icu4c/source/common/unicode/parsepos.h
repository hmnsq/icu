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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PARSEPOS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARSEPOS_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ U_SHOW_CPLUSPLUS_API

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"

 
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

class U_COMMON_API ParsePosition : ਜਨਤਕ UObject {
ਜਨਤਕ:
    /**
     * Default constructor, the index starts with 0 as default.
     * @stable ICU 2.0
     */
    ParsePosition()
        : UObject(),
        ਸੂਚਕਾਂਕ(0),
        error_ਸੂਚਕਾਂਕ(-1)
      {}

    /**
     * Create a new ParsePosition with the given initial index.
     * @param newIndex the new text offset.
     * @stable ICU 2.0
     */
    ParsePosition(int32_t new_ਸੂਚਕਾਂਕ)
        : UObject(),
        ਸੂਚਕਾਂਕ(new_ਸੂਚਕਾਂਕ),
        error_ਸੂਚਕਾਂਕ(-1)
      {}

    /**
     * Copy constructor
     * @param copy the object to be copied from.
     * @stable ICU 2.0
     */
    ParsePosition(ਅਚਲ ParsePosition& copy)
        : UObject(copy),
        ਸੂਚਕਾਂਕ(copy.ਸੂਚਕਾਂਕ),
        error_ਸੂਚਕਾਂਕ(copy.error_ਸੂਚਕਾਂਕ)
      {}

    /**
     * Destructor
     * @stable ICU 2.0
     */
    ਆਭਾਸੀ ~ParsePosition();

    /**
     * Assignment operator
     * @stable ICU 2.0
     */
    ਸਤਰਅੰਦਰ ParsePosition& ਸੰਚਾਲਕ=(ਅਚਲ ParsePosition& copy) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two parse positions are equal, false otherwise.
     * @stable ICU 2.0
     */
    ਸਤਰਅੰਦਰ bool               ਸੰਚਾਲਕ==(ਅਚਲ ParsePosition& that) ਅਚਲ;

    /**
     * Equality operator.
     * @return true if the two parse positions are not equal, false otherwise.
     * @stable ICU 2.0
     */
    ਸਤਰਅੰਦਰ bool               ਸੰਚਾਲਕ!=(ਅਚਲ ParsePosition& that) ਅਚਲ;

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
    ParsePosition *clone() ਅਚਲ;

    /**
     * Retrieve the current parse position.  On input to a parse method, this
     * is the index of the character at which parsing will begin; on output, it
     * is the index of the character following the last character parsed.
     * @return the current index.
     * @stable ICU 2.0
     */
    ਸਤਰਅੰਦਰ int32_t get_ਸੂਚਕਾਂਕ() ਅਚਲ;

    /**
     * Set the current parse position.
     * @param index the new index.
     * @stable ICU 2.0
     */
    ਸਤਰਅੰਦਰ ਖਾਲੀ ਸੈੱਟ_ਸੂਚਕਾਂਕ(int32_t ਸੂਚਕਾਂਕ);

    /**
     * Set the index at which a parse error occurred.  Formatters
     * should set this before returning an error code from their
     * parseObject method.  The default value is -1 if this is not
     * set.
     * @stable ICU 2.0
     */
    ਸਤਰਅੰਦਰ ਖਾਲੀ ਸੈੱਟ_Error_ਸੂਚਕਾਂਕ(int32_t ei);

    /**
     * Retrieve the index at which an error occurred, or -1 if the
     * error index has not been set.
     * @stable ICU 2.0
     */
    ਸਤਰਅੰਦਰ int32_t get_Error_ਸੂਚਕਾਂਕ() ਅਚਲ;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     *
     * @stable ICU 2.2
     */
    ਸਥਿਰ UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     *
     * @stable ICU 2.2
     */
    ਆਭਾਸੀ UClassID getDynamicClassID() ਅਚਲ override;

ਨਿਜੀ:
    /**
     * Input: the place you start parsing.
     * <br>Output: position where the parse stopped.
     * This is designed to be used serially,
     * with each call setting index up for the next one.
     */
    int32_t ਸੂਚਕਾਂਕ;

    /**
     * The index at which a parse error occurred.
     */
    int32_t error_ਸੂਚਕਾਂਕ;

};

ਸਤਰਅੰਦਰ ParsePosition&
ParsePosition::ਸੰਚਾਲਕ=(ਅਚਲ ParsePosition& copy) U_LIFETIME_BOUND
{
  ਸੂਚਕਾਂਕ = copy.ਸੂਚਕਾਂਕ;
  error_ਸੂਚਕਾਂਕ = copy.error_ਸੂਚਕਾਂਕ;
  ਵਾਪਸ *this;
}

ਸਤਰਅੰਦਰ bool
ParsePosition::ਸੰਚਾਲਕ==(ਅਚਲ ParsePosition& copy) ਅਚਲ
{
  ਜੇ(ਸੂਚਕਾਂਕ != copy.ਸੂਚਕਾਂਕ || error_ਸੂਚਕਾਂਕ != copy.error_ਸੂਚਕਾਂਕ)
  ਵਾਪਸ false;
  ਨਹੀਂਤਾਂ
  ਵਾਪਸ true;
}

ਸਤਰਅੰਦਰ bool
ParsePosition::ਸੰਚਾਲਕ!=(ਅਚਲ ParsePosition& copy) ਅਚਲ
{
  ਵਾਪਸ !ਸੰਚਾਲਕ==(copy);
}

ਸਤਰਅੰਦਰ int32_t
ParsePosition::get_ਸੂਚਕਾਂਕ() ਅਚਲ
{
  ਵਾਪਸ ਸੂਚਕਾਂਕ;
}

ਸਤਰਅੰਦਰ ਖਾਲੀ
ParsePosition::ਸੈੱਟ_ਸੂਚਕਾਂਕ(int32_t ਵਿੱਥ)
{
  this->ਸੂਚਕਾਂਕ = ਵਿੱਥ;
}

ਸਤਰਅੰਦਰ int32_t
ParsePosition::get_Error_ਸੂਚਕਾਂਕ() ਅਚਲ
{
  ਵਾਪਸ error_ਸੂਚਕਾਂਕ;
}

ਸਤਰਅੰਦਰ ਖਾਲੀ
ParsePosition::ਸੈੱਟ_Error_ਸੂਚਕਾਂਕ(int32_t ei)
{
  this->error_ਸੂਚਕਾਂਕ = ei;
}
U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* U_SHOW_CPLUSPLUS_API */

#ਜੇ_ਅੰਤ
