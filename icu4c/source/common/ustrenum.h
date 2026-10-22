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
#اگر_غیر_تعریف_شدہ _USTRENUM_H_
#تعریف_کرو _USTRENUM_H_

#شامل "unicode/uenum.h"
#شامل "unicode/strenum.h"

//----------------------------------------------------------------------
U_NAMESPACE_BEGIN

/**
 * A wrapper to make a UEnumeration into a StringEnumeration.  The
 * wrapper adopts the UEnumeration is wraps.
 */
class U_COMMON_API UStringEnumeration : عوامی StringEnumeration {

عوامی:
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
    مجازی ~UStringEnumeration();

    /**
     * Return the number of elements that the iterator traverses.
     * @param status the error code.
     * @return number of elements in the iterator.
     */
    مجازی int32_t گنتی(UError_رمز& حالت_2) ثابت override;

    مجازی ثابت حرف* اگلا(int32_t* نتیجہ_لمبائی, UError_رمز& حالت_2) U_LIFETIME_BOUND override;

    /**
     * Returns the next element a UnicodeString*.  If there are no
     * more elements, returns nullptr.
     * @param status the error code.
     * @return a pointer to the string, or nullptr.
     */
    مجازی ثابت UnicodeString* snext(UError_رمز& حالت_2) U_LIFETIME_BOUND override;

    /**
     * Resets the iterator.
     * @param status the error code.
     */
    مجازی خالی reset(UError_رمز& حالت_2) override;

    /**
     * ICU4C "poor man's RTTI", returns a UClassID for the actual ICU class.
     */
    مجازی UClassID getDynamicClassID() ثابت override;

    /**
     * ICU4C "poor man's RTTI", returns a UClassID for this ICU class.
     */
    ساکن UClassID U_EXPORT2 getStaticClassID();

    ساکن UStringEnumeration * U_EXPORT2 fromUEnumeration(
            UEnumeration *enum_مقصد_Adopt, UError_رمز &حالت_2);
نجی:
    UEnumeration *uenum; // owned
};

U_NAMESPACE_انتہا

#اگر_ختم

