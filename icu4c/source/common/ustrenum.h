// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2014, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
* Author: Alan Liu
* Created: November 11 2002
* Since: ICU 2.4
**********************************************************************
*/
#अगर_अघोषित _USTRENUM_H_
#घोषणा _USTRENUM_H_

#समावेश "unicode/uenum.h"
#समावेश "unicode/strenum.h"

//----------------------------------------------------------------------
U_NAMESPACE_BEGIN

/**
 * A wrapper to make a UEnumeration into a StringEnumeration.  The
 * wrapper adopts the UEnumeration is wraps.
 */
class U_COMMON_API UStringEnumeration : खुला StringEnumeration {

खुला:
    /**
     * Constructor.  This constructor adopts its UEnumeration
     * argument.
     * @param uenum a UEnumeration object.  This object takes
     * ownership of 'uenum' and will close it in its destructor.  The
     * caller must not call uenum_close on 'uenum' after calling this
     * constructor.
     */
    UStringEnumeration(UEnumeration* uenum);

    /**
     * Destructor.  This closes the UEnumeration passed in to the
     * constructor.
     */
    भव ~UStringEnumeration();

    /**
     * Return the number of elements that the iterator traverses.
     * @param status the error code.
     * @return number of elements in the iterator.
     */
    भव int32_t गिनती(UErrorCode& status) स्थिर override;

    भव स्थिर अक्षर* अग्र(int32_t* परिणाम_लंबाई, UErrorCode& status) U_LIFETIME_BOUND override;

    /**
     * Returns the next element a UnicodeString*.  If there are no
     * more elements, returns nullptr.
     * @param status the error code.
     * @return a pointer to the string, or nullptr.
     */
    भव स्थिर UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override;

    /**
     * Resets the iterator.
     * @param status the error code.
     */
    भव व्योम reset(UErrorCode& status) override;

    /**
     * ICU4C "poor man's RTTI", returns a UClassID for the actual ICU class.
     */
    भव UClassID getDynamicClassID() स्थिर override;

    /**
     * ICU4C "poor man's RTTI", returns a UClassID for this ICU class.
     */
    अटल UClassID U_EXPORT2 getStaticClassID();

    अटल UStringEnumeration * U_EXPORT2 fromUEnumeration(
            UEnumeration *enumToAdopt, UErrorCode &status);
निजी:
    UEnumeration *uenum; // owned
};

U_NAMESPACE_END

#पूर्ण_अगर

