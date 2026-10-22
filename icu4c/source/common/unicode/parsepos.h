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

#अगर_अघोषित PARSEPOS_H
#घोषणा PARSEPOS_H

#समावेश "unicode/utypes.h"

#अगर U_SHOW_CPLUSPLUS_API

#समावेश "unicode/uobject.h"

 
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

class U_COMMON_API ParsePosition : खुला UObject {
खुला:
    /**
     * Default constructor, the index starts with 0 as default.
     * @stable ICU 2.0
     */
    ParsePosition()
        : UObject(),
        सूचकांक(0),
        error_सूचकांक(-1)
      {}

    /**
     * Create a new ParsePosition with the given initial index.
     * @param newIndex the new text offset.
     * @stable ICU 2.0
     */
    ParsePosition(int32_t new_सूचकांक)
        : UObject(),
        सूचकांक(new_सूचकांक),
        error_सूचकांक(-1)
      {}

    /**
     * Copy constructor
     * @param copy the object to be copied from.
     * @stable ICU 2.0
     */
    ParsePosition(स्थिर ParsePosition& copy)
        : UObject(copy),
        सूचकांक(copy.सूचकांक),
        error_सूचकांक(copy.error_सूचकांक)
      {}

    /**
     * Destructor
     * @stable ICU 2.0
     */
    भव ~ParsePosition();

    /**
     * Assignment operator
     * @stable ICU 2.0
     */
    अंतरभूत ParsePosition& चालक=(स्थिर ParsePosition& copy) U_LIFETIME_BOUND;

    /**
     * Equality operator.
     * @return true if the two parse positions are equal, false otherwise.
     * @stable ICU 2.0
     */
    अंतरभूत bool               चालक==(स्थिर ParsePosition& that) स्थिर;

    /**
     * Equality operator.
     * @return true if the two parse positions are not equal, false otherwise.
     * @stable ICU 2.0
     */
    अंतरभूत bool               चालक!=(स्थिर ParsePosition& that) स्थिर;

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
    ParsePosition *clone() स्थिर;

    /**
     * Retrieve the current parse position.  On input to a parse method, this
     * is the index of the character at which parsing will begin; on output, it
     * is the index of the character following the last character parsed.
     * @return the current index.
     * @stable ICU 2.0
     */
    अंतरभूत int32_t get_सूचकांक() स्थिर;

    /**
     * Set the current parse position.
     * @param index the new index.
     * @stable ICU 2.0
     */
    अंतरभूत व्योम set_सूचकांक(int32_t सूचकांक);

    /**
     * Set the index at which a parse error occurred.  Formatters
     * should set this before returning an error code from their
     * parseObject method.  The default value is -1 if this is not
     * set.
     * @stable ICU 2.0
     */
    अंतरभूत व्योम set_Error_सूचकांक(int32_t ei);

    /**
     * Retrieve the index at which an error occurred, or -1 if the
     * error index has not been set.
     * @stable ICU 2.0
     */
    अंतरभूत int32_t get_Error_सूचकांक() स्थिर;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     *
     * @stable ICU 2.2
     */
    अटल UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     *
     * @stable ICU 2.2
     */
    भव UClassID getDynamicClassID() स्थिर override;

निजी:
    /**
     * Input: the place you start parsing.
     * <br>Output: position where the parse stopped.
     * This is designed to be used serially,
     * with each call setting index up for the next one.
     */
    int32_t सूचकांक;

    /**
     * The index at which a parse error occurred.
     */
    int32_t error_सूचकांक;

};

अंतरभूत ParsePosition&
ParsePosition::चालक=(स्थिर ParsePosition& copy) U_LIFETIME_BOUND
{
  सूचकांक = copy.सूचकांक;
  error_सूचकांक = copy.error_सूचकांक;
  वापस *this;
}

अंतरभूत bool
ParsePosition::चालक==(स्थिर ParsePosition& copy) स्थिर
{
  अगर(सूचकांक != copy.सूचकांक || error_सूचकांक != copy.error_सूचकांक)
  वापस false;
  अन्यथा
  वापस true;
}

अंतरभूत bool
ParsePosition::चालक!=(स्थिर ParsePosition& copy) स्थिर
{
  वापस !चालक==(copy);
}

अंतरभूत int32_t
ParsePosition::get_सूचकांक() स्थिर
{
  वापस सूचकांक;
}

अंतरभूत व्योम
ParsePosition::set_सूचकांक(int32_t offset)
{
  this->सूचकांक = offset;
}

अंतरभूत int32_t
ParsePosition::get_Error_सूचकांक() स्थिर
{
  वापस error_सूचकांक;
}

अंतरभूत व्योम
ParsePosition::set_Error_सूचकांक(int32_t ei)
{
  this->error_सूचकांक = ei;
}
U_NAMESPACE_END

#पूर्ण_अगर /* U_SHOW_CPLUSPLUS_API */

#पूर्ण_अगर
