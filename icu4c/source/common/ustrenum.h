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
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _USTRENUM_H_
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _USTRENUM_H_

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uenum.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/strenum.h"

//----------------------------------------------------------------------
U_NAMESPACE_BEGIN

/**
 * A wrapper to make a UEnumeration into a StringEnumeration.  The
 * wrapper adopts the UEnumeration is wraps.
 */
class U_COMMON_API UStringEnumeration : ਜਨਤਕ StringEnumeration {

ਜਨਤਕ:
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
    ਆਭਾਸੀ ~UStringEnumeration();

    /**
     * Return the number of elements that the iterator traverses.
     * @param status the error code.
     * @return number of elements in the iterator.
     */
    ਆਭਾਸੀ int32_t ਗਿਣਤੀ_2(UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override;

    ਆਭਾਸੀ ਅਚਲ ਅੱਖਰ* ਅਗਲਾ(int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ, UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND override;

    /**
     * Returns the next element a UnicodeString*.  If there are no
     * more elements, returns nullptr.
     * @param status the error code.
     * @return a pointer to the string, or nullptr.
     */
    ਆਭਾਸੀ ਅਚਲ UnicodeString* snext(UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND override;

    /**
     * Resets the iterator.
     * @param status the error code.
     */
    ਆਭਾਸੀ ਖਾਲੀ reset(UError_ਕੋਡ& ਸਥਿਤੀ) override;

    /**
     * ICU4C "poor man's RTTI", returns a UClassID for the actual ICU class.
     */
    ਆਭਾਸੀ UClassID getDynamicClassID() ਅਚਲ override;

    /**
     * ICU4C "poor man's RTTI", returns a UClassID for this ICU class.
     */
    ਸਥਿਰ UClassID U_EXPORT2 getStaticClassID();

    ਸਥਿਰ UStringEnumeration * U_EXPORT2 fromUEnumeration(
            UEnumeration *enum_ਮੰਜ਼ਿਲ_Adopt, UError_ਕੋਡ &ਸਥਿਤੀ);
ਨਿਜੀ:
    UEnumeration *uenum; // owned
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ

