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
#ifndef _USTRENUM_H_
#define _USTRENUM_H_

#ಅನ್ತರ್ಭಾವ "unicode/uenum.h"
#ಅನ್ತರ್ಭಾವ "unicode/strenum.h"

//----------------------------------------------------------------------
U_NAMESPACE_BEGIN

/**
 * A wrapper to make a UEnumeration into a StringEnumeration.  The
 * wrapper adopts the UEnumeration is wraps.
 */
class U_COMMON_API UStringEnumeration : ಸಾರ್ವಜನಿಕ StringEnumeration {

ಸಾರ್ವಜನಿಕ:
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
    ಆಭಾಸ ~UStringEnumeration();

    /**
     * Return the number of elements that the iterator traverses.
     * @param status the error code.
     * @return number of elements in the iterator.
     */
    ಆಭಾಸ int32_t ಎಣಿಕೆ(UErrorCode& status) ಧ್ರುವ override;

    ಆಭಾಸ ಧ್ರುವ ವರ್ಣ* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override;

    /**
     * Returns the next element a UnicodeString*.  If there are no
     * more elements, returns nullptr.
     * @param status the error code.
     * @return a pointer to the string, or nullptr.
     */
    ಆಭಾಸ ಧ್ರುವ UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override;

    /**
     * Resets the iterator.
     * @param status the error code.
     */
    ಆಭಾಸ ಶೂನ್ಯ reset(UErrorCode& status) override;

    /**
     * ICU4C "poor man's RTTI", returns a UClassID for the actual ICU class.
     */
    ಆಭಾಸ UClassID getDynamicClassID() ಧ್ರುವ override;

    /**
     * ICU4C "poor man's RTTI", returns a UClassID for this ICU class.
     */
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();

    ಸ್ಥಿರ UStringEnumeration * U_EXPORT2 fromUEnumeration(
            UEnumeration *enumToAdopt, UErrorCode &status);
ಗುಪ್ತ:
    UEnumeration *uenum; // owned
};

U_NAMESPACE_END

#endif

