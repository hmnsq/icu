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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PARSEPOS_H
#ವ್ಯಾಖ್ಯಾನಿಸು PARSEPOS_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API

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
        ಸೂಚ್ಯಂಕ(0),
        error_ಸೂಚ್ಯಂಕ(-1)
      {}

    /**
     * Create a new ParsePosition with the given initial index.
     * @param newIndex the new text offset.
     * @stable ICU 2.0
     */
    ParsePosition(int32_t new_ಸೂಚ್ಯಂಕ)
        : UObject(),
        ಸೂಚ್ಯಂಕ(new_ಸೂಚ್ಯಂಕ),
        error_ಸೂಚ್ಯಂಕ(-1)
      {}

    /**
     * Copy constructor
     * @param copy the object to be copied from.
     * @stable ICU 2.0
     */
    ParsePosition(ಸ್ಥಿರಾಂಕ ParsePosition& copy)
        : UObject(copy),
        ಸೂಚ್ಯಂಕ(copy.ಸೂಚ್ಯಂಕ),
        error_ಸೂಚ್ಯಂಕ(copy.error_ಸೂಚ್ಯಂಕ)
      {}

    /**
     * Destructor
     * @stable ICU 2.0
     */
    ವಾಸ್ತವಿಕ ~ParsePosition();

    /**
     * Assignment operator
     * @stable ICU 2.0
     */
    ಸಾಲೊಳಗೆ ParsePosition& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ParsePosition& copy) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two parse positions are equal, false otherwise.
     * @stable ICU 2.0
     */
    ಸಾಲೊಳಗೆ bool               ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ParsePosition& that) ಸ್ಥಿರಾಂಕ;

    /**
     * Equality operator.
     * @return true if the two parse positions are not equal, false otherwise.
     * @stable ICU 2.0
     */
    ಸಾಲೊಳಗೆ bool               ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ ParsePosition& that) ಸ್ಥಿರಾಂಕ;

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
    ParsePosition *clone() ಸ್ಥಿರಾಂಕ;

    /**
     * Retrieve the current parse position.  On input to a parse method, this
     * is the index of the character at which parsing will begin; on output, it
     * is the index of the character following the last character parsed.
     * @return the current index.
     * @stable ICU 2.0
     */
    ಸಾಲೊಳಗೆ int32_t get_ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ;

    /**
     * Set the current parse position.
     * @param index the new index.
     * @stable ICU 2.0
     */
    ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಗಣ_ಸೂಚ್ಯಂಕ(int32_t ಸೂಚ್ಯಂಕ);

    /**
     * Set the index at which a parse error occurred.  Formatters
     * should set this before returning an error code from their
     * parseObject method.  The default value is -1 if this is not
     * set.
     * @stable ICU 2.0
     */
    ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಗಣ_Error_ಸೂಚ್ಯಂಕ(int32_t ei);

    /**
     * Retrieve the index at which an error occurred, or -1 if the
     * error index has not been set.
     * @stable ICU 2.0
     */
    ಸಾಲೊಳಗೆ int32_t get_Error_ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ;

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
    ವಾಸ್ತವಿಕ UClassID getDynamicClassID() ಸ್ಥಿರಾಂಕ override;

ಖಾಸಗಿ:
    /**
     * Input: the place you start parsing.
     * <br>Output: position where the parse stopped.
     * This is designed to be used serially,
     * with each call setting index up for the next one.
     */
    int32_t ಸೂಚ್ಯಂಕ;

    /**
     * The index at which a parse error occurred.
     */
    int32_t error_ಸೂಚ್ಯಂಕ;

};

ಸಾಲೊಳಗೆ ParsePosition&
ParsePosition::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ParsePosition& copy) U_LIFETIME_BOUND
{
  ಸೂಚ್ಯಂಕ = copy.ಸೂಚ್ಯಂಕ;
  error_ಸೂಚ್ಯಂಕ = copy.error_ಸೂಚ್ಯಂಕ;
  ಹಿಂತಿರುಗಿ *this;
}

ಸಾಲೊಳಗೆ bool
ParsePosition::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ParsePosition& copy) ಸ್ಥಿರಾಂಕ
{
  ಆದರೆ(ಸೂಚ್ಯಂಕ != copy.ಸೂಚ್ಯಂಕ || error_ಸೂಚ್ಯಂಕ != copy.error_ಸೂಚ್ಯಂಕ)
  ಹಿಂತಿರುಗಿ false;
  ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ true;
}

ಸಾಲೊಳಗೆ bool
ParsePosition::ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ ParsePosition& copy) ಸ್ಥಿರಾಂಕ
{
  ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(copy);
}

ಸಾಲೊಳಗೆ int32_t
ParsePosition::get_ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ
{
  ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ
ParsePosition::ಗಣ_ಸೂಚ್ಯಂಕ(int32_t ಸ್ಥಾನಾಂತರ)
{
  this->ಸೂಚ್ಯಂಕ = ಸ್ಥಾನಾಂತರ;
}

ಸಾಲೊಳಗೆ int32_t
ParsePosition::get_Error_ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ
{
  ಹಿಂತಿರುಗಿ error_ಸೂಚ್ಯಂಕ;
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ
ParsePosition::ಗಣ_Error_ಸೂಚ್ಯಂಕ(int32_t ei)
{
  this->error_ಸೂಚ್ಯಂಕ = ei;
}
U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* U_SHOW_CPLUSPLUS_API */

#ಆದರೆ_ಅಂತ್ಯ
