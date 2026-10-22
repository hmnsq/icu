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
#నిర్వచితం_కాకపోతే _USTRENUM_H_
#నిర్వచించు _USTRENUM_H_

#అన్తర్భావ "unicode/uenum.h"
#అన్తర్భావ "unicode/strenum.h"

//----------------------------------------------------------------------
U_NAMESPACE_BEGIN

/**
 * A wrapper to make a UEnumeration into a StringEnumeration.  The
 * wrapper adopts the UEnumeration is wraps.
 */
class U_COMMON_API UStringEnumeration : బహిరంగ StringEnumeration {

బహిరంగ:
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
    మాయా ~UStringEnumeration();

    /**
     * Return the number of elements that the iterator traverses.
     * @param status the error code.
     * @return number of elements in the iterator.
     */
    మాయా int32_t లెక్క(UError_సంకేతం& స్థితి_2) స్థిరరాశి override;

    మాయా స్థిరరాశి అక్షరం* తదుపరి(int32_t* ఫలితం_నిడివి, UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND override;

    /**
     * Returns the next element a UnicodeString*.  If there are no
     * more elements, returns nullptr.
     * @param status the error code.
     * @return a pointer to the string, or nullptr.
     */
    మాయా స్థిరరాశి UnicodeString* snext(UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND override;

    /**
     * Resets the iterator.
     * @param status the error code.
     */
    మాయా శూన్య reset(UError_సంకేతం& స్థితి_2) override;

    /**
     * ICU4C "poor man's RTTI", returns a UClassID for the actual ICU class.
     */
    మాయా UClassID getDynamicClassID() స్థిరరాశి override;

    /**
     * ICU4C "poor man's RTTI", returns a UClassID for this ICU class.
     */
    స్థిర UClassID U_EXPORT2 getStaticClassID();

    స్థిర UStringEnumeration * U_EXPORT2 fromUEnumeration(
            UEnumeration *enum_గమ్యం_Adopt, UError_సంకేతం &స్థితి_2);
అంతరంగ:
    UEnumeration *uenum; // owned
};

U_NAMESPACE_చివర

#అయితే_ముగింపు

