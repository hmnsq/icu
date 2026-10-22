// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

//
//  UVector32 is a class implementing a vector of 32 bit integers.
//            It is similar to UVector, but holds int32_t values rather than pointers.
//            Most of the code is unchanged from UVector.
//

#నిర్వచితం_కాకపోతే UVECTOR32_H
#నిర్వచించు UVECTOR32_H

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "uassert.h"

U_NAMESPACE_BEGIN



/**
 * <p>Ultralightweight C++ implementation of a <tt>void*</tt> vector
 * that is (mostly) compatible with java.util.Vector.
 *
 * <p>This is a very simple implementation, written to satisfy an
 * immediate porting need.  As such, it is not completely fleshed out,
 * and it aims for simplicity and conformity.  Nonetheless, it serves
 * its purpose (porting code from java that uses java.util.Vector)
 * well, and it could be easily made into a more robust vector class.
 *
 * <p><b>Design notes</b>
 *
 * <p>There is index bounds checking, but little is done about it.  If
 * indices are out of bounds, either nothing happens, or zero is
 * returned.  We <em>do</em> avoid indexing off into the weeds.
 *
 * <p>There is detection of out of memory, but the handling is very
 * coarse-grained -- similar to UnicodeString's protocol, but even
 * coarser.  The class contains <em>one static flag</em> that is set
 * when any call to <tt>new</tt> returns zero.  This allows the caller
 * to use several vectors and make just one check at the end to see if
 * a memory failure occurred.  This is more efficient than making a
 * check after each call on each vector when doing many operations on
 * multiple vectors.  The single static flag works best when memory
 * failures are infrequent, and when recovery options are limited or
 * nonexistent.
 *
 * <p><b>To do</b>
 *
 * <p>Improve the handling of index out of bounds errors.
 *
 * @author Alan Liu
 */
class U_COMMON_API UVector32 : బహిరంగ UObject {
అంతరంగ:
    int32_t   లెక్క;

    int32_t   సామర్థ్యం;
    
    int32_t   max_సామర్థ్యం;   // Limit beyond which capacity is not permitted to grow.

    int32_t*  elements;

బహిరంగ:
    UVector32(UError_సంకేతం &స్థితి_2);

    UVector32(int32_t initial_సామర్థ్యం, UError_సంకేతం &స్థితి_2);

    మాయా ~UVector32();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    శూన్య కేటాయించు(స్థిరరాశి UVector32& other, UError_సంకేతం &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool క్రియాకారకం==(స్థిరరాశి UVector32& other) స్థిరరాశి;

    /**
     * Equivalent to !operator==()
     */
    అంతఃవరుస bool క్రియాకారకం!=(స్థిరరాశి UVector32& other) స్థిరరాశి;

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    అంతఃవరుస శూన్య కూడు_Element(int32_t elem, UError_సంకేతం &స్థితి_2);

    శూన్య సమితి_Element_At(int32_t elem, int32_t సూచీ);

    శూన్య చొప్పించు_Element_At(int32_t elem, int32_t సూచీ, UError_సంకేతం &స్థితి_2);
    
    అంతఃవరుస int32_t elementAti(int32_t సూచీ) స్థిరరాశి;

    UBool equals(స్థిరరాశి UVector32 &other) స్థిరరాశి;

    అంతఃవరుస int32_t చివరి_Elementi() స్థిరరాశి;

    int32_t సూచీ_యొక్క(int32_t elem, int32_t ప్రారంభం_సూచీ = 0) స్థిరరాశి;

    అంతఃవరుస UBool కలిగియుంది(int32_t elem) స్థిరరాశి;

    UBool కలిగియుంది_All(స్థిరరాశి UVector32& other) స్థిరరాశి;

    UBool removeAll(స్థిరరాశి UVector32& other);

    UBool retainAll(స్థిరరాశి UVector32& other);

    శూన్య removeElementAt(int32_t సూచీ);

    శూన్య removeAllElements();

    అంతఃవరుస int32_t కొలత_2() స్థిరరాశి;

    అంతఃవరుస UBool is_ఖాళీలు() స్థిరరాశి;

    // Inline.  Use this one for speedy size check.
    అంతఃవరుస UBool ensure_సామర్థ్యం(int32_t minimum_సామర్థ్యం, UError_సంకేతం &స్థితి_2);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expand_సామర్థ్యం(int32_t minimum_సామర్థ్యం, UError_సంకేతం &స్థితి_2);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    శూన్య సమితి_కొలత(int32_t new_కొలత);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool కలిగియుంది_None(స్థిరరాశి UVector32& other) స్థిరరాశి;


    /**
     * Insert the given integer into this vector at its sorted position.
     * The current elements are assumed to be sorted already.
     */
    శూన్య sorted_చొప్పించు(int32_t elem, UError_సంకేతం& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    అంతఃవరుస int32_t *get_బఫరు() స్థిరరాశి;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (32 bits each), not bytes.
     */
    శూన్య సమితి_Max_సామర్థ్యం(int32_t హద్దు);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    స్థిర UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    మాయా UClassID getDynamicClassID() స్థిరరాశి override;

అంతరంగ:
    శూన్య _init(int32_t initial_సామర్థ్యం, UError_సంకేతం &స్థితి_2);

    // Disallow
    UVector32(స్థిరరాశి UVector32&) = delete;

    // Disallow
    UVector32& క్రియాకారకం=(స్థిరరాశి UVector32&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector32, they are all together.
బహిరంగ:
    అంతఃవరుస UBool ఖాళీలు() స్థిరరాశి;   // TODO:  redundant, same as empty().  Remove it?

    అంతఃవరుస int32_t peeki() స్థిరరాశి;
    
    అంతఃవరుస int32_t popi();
    
    అంతఃవరుస int32_t నెట్టు(int32_t i, UError_సంకేతం &స్థితి_2);

    అంతఃవరుస int32_t *reserve_ఖండం(int32_t కొలత_2, UError_సంకేతం &స్థితి_2);
    అంతఃవరుస int32_t *తీయి_చట్రం(int32_t కొలత_2);
};


// UVector32 inlines

అంతఃవరుస UBool UVector32::ensure_సామర్థ్యం(int32_t minimum_సామర్థ్యం, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ ((minimum_సామర్థ్యం >= 0) && (సామర్థ్యం >= minimum_సామర్థ్యం)) {
        తిరిగి true;
    } లేకపోతే {
        తిరిగి expand_సామర్థ్యం(minimum_సామర్థ్యం, స్థితి_2);
    }
}

అంతఃవరుస int32_t UVector32::elementAti(int32_t సూచీ) స్థిరరాశి {
    తిరిగి (సూచీ >= 0 && లెక్క > 0 && లెక్క - సూచీ > 0) ? elements[సూచీ] : 0;
}


అంతఃవరుస శూన్య UVector32::కూడు_Element(int32_t elem, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (ensure_సామర్థ్యం(లెక్క + 1, స్థితి_2)) {
        elements[లెక్క] = elem;
        లెక్క++;
    }
}

అంతఃవరుస int32_t *UVector32::reserve_ఖండం(int32_t కొలత_2, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (ensure_సామర్థ్యం(లెక్క+కొలత_2, స్థితి_2) == false) {
        తిరిగి nullptr;
    }
    int32_t  *rp = elements+లెక్క;
    లెక్క += కొలత_2;
    తిరిగి rp;
}

అంతఃవరుస int32_t *UVector32::తీయి_చట్రం(int32_t కొలత_2) {
    U_ASSERT(లెక్క >= కొలత_2);
    లెక్క -= కొలత_2;
    ఒకవేళ (లెక్క < 0) {
        లెక్క = 0;
    }
    తిరిగి elements+లెక్క-కొలత_2;
}



అంతఃవరుస int32_t UVector32::కొలత_2() స్థిరరాశి {
    తిరిగి లెక్క;
}

అంతఃవరుస UBool UVector32::is_ఖాళీలు() స్థిరరాశి {
    తిరిగి లెక్క == 0;
}

అంతఃవరుస UBool UVector32::కలిగియుంది(int32_t obj) స్థిరరాశి {
    తిరిగి సూచీ_యొక్క(obj) >= 0;
}

అంతఃవరుస int32_t UVector32::చివరి_Elementi() స్థిరరాశి {
    తిరిగి elementAti(లెక్క-1);
}

అంతఃవరుస bool UVector32::క్రియాకారకం!=(స్థిరరాశి UVector32& other) స్థిరరాశి {
    తిరిగి !క్రియాకారకం==(other);
}

అంతఃవరుస int32_t *UVector32::get_బఫరు() స్థిరరాశి {
    తిరిగి elements;
}


// UStack inlines

అంతఃవరుస UBool UVector32::ఖాళీలు() స్థిరరాశి {
    తిరిగి is_ఖాళీలు();
}

అంతఃవరుస int32_t UVector32::peeki() స్థిరరాశి {
    తిరిగి చివరి_Elementi();
}

అంతఃవరుస int32_t UVector32::నెట్టు(int32_t i, UError_సంకేతం &స్థితి_2) {
    కూడు_Element(i, స్థితి_2);
    తిరిగి i;
}

అంతఃవరుస int32_t UVector32::popi() {
    int32_t ఫలితం = 0;
    ఒకవేళ (లెక్క > 0) {
        లెక్క--;
        ఫలితం = elements[లెక్క];
    }
    తిరిగి ఫలితం;
}

U_NAMESPACE_చివర

#అయితే_ముగింపు
