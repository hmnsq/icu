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
#جيڪڏهن_وصف_نه_ٿيل _USTRENUM_H_
#وصف_ڪريو _USTRENUM_H_

#ضمن "unicode/uenum.h"
#ضمن "unicode/strenum.h"

//----------------------------------------------------------------------
U_NAMESPACE_BEGIN

/**
 * A wrapper to make a UEnumeration into a StringEnumeration.  The
 * wrapper adopts the UEnumeration is wraps.
 */
class U_COMMON_API UStringEnumeration : عام StringEnumeration {

عام:
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
    مجازي ~UStringEnumeration();

    /**
     * Return the number of elements that the iterator traverses.
     * @param status the error code.
     * @return number of elements in the iterator.
     */
    مجازي int32_t ڳڻپ(UError_ڪوڊ& حالت) ثابت override;

    مجازي ثابت حرف* ايندڙ(int32_t* نتيجو_ڊيگهائي, UError_ڪوڊ& حالت) U_LIFETIME_BOUND override;

    /**
     * Returns the next element a UnicodeString*.  If there are no
     * more elements, returns nullptr.
     * @param status the error code.
     * @return a pointer to the string, or nullptr.
     */
    مجازي ثابت UnicodeString* snext(UError_ڪوڊ& حالت) U_LIFETIME_BOUND override;

    /**
     * Resets the iterator.
     * @param status the error code.
     */
    مجازي فراغ reset(UError_ڪوڊ& حالت) override;

    /**
     * ICU4C "poor man's RTTI", returns a UClassID for the actual ICU class.
     */
    مجازي UClassID getDynamicClassID() ثابت override;

    /**
     * ICU4C "poor man's RTTI", returns a UClassID for this ICU class.
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();

    ساكن UStringEnumeration * U_EXPORT2 fromUEnumeration(
            UEnumeration *enum_منزل_Adopt, UError_ڪوڊ &حالت);
خاص:
    UEnumeration *uenum; // owned
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو

