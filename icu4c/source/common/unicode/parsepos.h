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

#నిర్వచితం_కాకపోతే PARSEPOS_H
#నిర్వచించు PARSEPOS_H

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API

#అన్తర్భావ "unicode/uobject.h"

 
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

class U_COMMON_API ParsePosition : బహిరంగ UObject {
బహిరంగ:
    /**
     * Default constructor, the index starts with 0 as default.
     * @stable ICU 2.0
     */
    ParsePosition()
        : UObject(),
        సూచీ(0),
        error_సూచీ(-1)
      {}

    /**
     * Create a new ParsePosition with the given initial index.
     * @param newIndex the new text offset.
     * @stable ICU 2.0
     */
    ParsePosition(int32_t new_సూచీ)
        : UObject(),
        సూచీ(new_సూచీ),
        error_సూచీ(-1)
      {}

    /**
     * Copy constructor
     * @param copy the object to be copied from.
     * @stable ICU 2.0
     */
    ParsePosition(స్థిరరాశి ParsePosition& copy)
        : UObject(copy),
        సూచీ(copy.సూచీ),
        error_సూచీ(copy.error_సూచీ)
      {}

    /**
     * Destructor
     * @stable ICU 2.0
     */
    మాయా ~ParsePosition();

    /**
     * Assignment operator
     * @stable ICU 2.0
     */
    అంతఃవరుస ParsePosition& క్రియాకారకం=(స్థిరరాశి ParsePosition& copy) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two parse positions are equal, false otherwise.
     * @stable ICU 2.0
     */
    అంతఃవరుస bool               క్రియాకారకం==(స్థిరరాశి ParsePosition& that) స్థిరరాశి;

    /**
     * Equality operator.
     * @return true if the two parse positions are not equal, false otherwise.
     * @stable ICU 2.0
     */
    అంతఃవరుస bool               క్రియాకారకం!=(స్థిరరాశి ParsePosition& that) స్థిరరాశి;

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
    ParsePosition *clone() స్థిరరాశి;

    /**
     * Retrieve the current parse position.  On input to a parse method, this
     * is the index of the character at which parsing will begin; on output, it
     * is the index of the character following the last character parsed.
     * @return the current index.
     * @stable ICU 2.0
     */
    అంతఃవరుస int32_t get_సూచీ() స్థిరరాశి;

    /**
     * Set the current parse position.
     * @param index the new index.
     * @stable ICU 2.0
     */
    అంతఃవరుస శూన్య సమితి_సూచీ(int32_t సూచీ);

    /**
     * Set the index at which a parse error occurred.  Formatters
     * should set this before returning an error code from their
     * parseObject method.  The default value is -1 if this is not
     * set.
     * @stable ICU 2.0
     */
    అంతఃవరుస శూన్య సమితి_Error_సూచీ(int32_t ei);

    /**
     * Retrieve the index at which an error occurred, or -1 if the
     * error index has not been set.
     * @stable ICU 2.0
     */
    అంతఃవరుస int32_t get_Error_సూచీ() స్థిరరాశి;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     *
     * @stable ICU 2.2
     */
    స్థిర UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     *
     * @stable ICU 2.2
     */
    మాయా UClassID getDynamicClassID() స్థిరరాశి override;

అంతరంగ:
    /**
     * Input: the place you start parsing.
     * <br>Output: position where the parse stopped.
     * This is designed to be used serially,
     * with each call setting index up for the next one.
     */
    int32_t సూచీ;

    /**
     * The index at which a parse error occurred.
     */
    int32_t error_సూచీ;

};

అంతఃవరుస ParsePosition&
ParsePosition::క్రియాకారకం=(స్థిరరాశి ParsePosition& copy) U_LIFETIME_BOUND
{
  సూచీ = copy.సూచీ;
  error_సూచీ = copy.error_సూచీ;
  తిరిగి *this;
}

అంతఃవరుస bool
ParsePosition::క్రియాకారకం==(స్థిరరాశి ParsePosition& copy) స్థిరరాశి
{
  ఒకవేళ(సూచీ != copy.సూచీ || error_సూచీ != copy.error_సూచీ)
  తిరిగి false;
  లేకపోతే
  తిరిగి true;
}

అంతఃవరుస bool
ParsePosition::క్రియాకారకం!=(స్థిరరాశి ParsePosition& copy) స్థిరరాశి
{
  తిరిగి !క్రియాకారకం==(copy);
}

అంతఃవరుస int32_t
ParsePosition::get_సూచీ() స్థిరరాశి
{
  తిరిగి సూచీ;
}

అంతఃవరుస శూన్య
ParsePosition::సమితి_సూచీ(int32_t స్థానభ్రంశం)
{
  this->సూచీ = స్థానభ్రంశం;
}

అంతఃవరుస int32_t
ParsePosition::get_Error_సూచీ() స్థిరరాశి
{
  తిరిగి error_సూచీ;
}

అంతఃవరుస శూన్య
ParsePosition::సమితి_Error_సూచీ(int32_t ei)
{
  this->error_సూచీ = ei;
}
U_NAMESPACE_చివర

#అయితే_ముగింపు /* U_SHOW_CPLUSPLUS_API */

#అయితే_ముగింపు
