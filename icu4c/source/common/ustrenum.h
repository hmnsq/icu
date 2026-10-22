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
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _USTRENUM_H_
#ವ್ಯಾಖ್ಯಾನಿಸು _USTRENUM_H_

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
    ವಾಸ್ತವಿಕ ~UStringEnumeration();

    /**
     * Return the number of elements that the iterator traverses.
     * @param status the error code.
     * @return number of elements in the iterator.
     */
    ವಾಸ್ತವಿಕ int32_t ಎಣಿಕೆ_2(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;

    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮುಂದಿನ(int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND override;

    /**
     * Returns the next element a UnicodeString*.  If there are no
     * more elements, returns nullptr.
     * @param status the error code.
     * @return a pointer to the string, or nullptr.
     */
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ UnicodeString* snext(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND override;

    /**
     * Resets the iterator.
     * @param status the error code.
     */
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ reset(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) override;

    /**
     * ICU4C "poor man's RTTI", returns a UClassID for the actual ICU class.
     */
    ವಾಸ್ತವಿಕ UClassID getDynamicClassID() ಸ್ಥಿರಾಂಕ override;

    /**
     * ICU4C "poor man's RTTI", returns a UClassID for this ICU class.
     */
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();

    ಸ್ಥಿರ UStringEnumeration * U_EXPORT2 fromUEnumeration(
            UEnumeration *enum_ಗಮ್ಯ_Adopt, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
ಖಾಸಗಿ:
    UEnumeration *uenum; // owned
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ

