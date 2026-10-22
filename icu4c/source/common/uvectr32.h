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

#ifndef UVECTOR32_H
#define UVECTOR32_H

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "uassert.h"

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
class U_COMMON_API UVector32 : सार्वजनिक UObject {
गुप्त:
    int32_t   गणनम्;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int32_t*  elements;

सार्वजनिक:
    UVector32(UErrorCode &status);

    UVector32(int32_t initialCapacity, UErrorCode &status);

    आभास ~UVector32();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    शून्य assign(ध्रुव UVector32& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(ध्रुव UVector32& other) ध्रुव;

    /**
     * Equivalent to !operator==()
     */
    अन्तःस्थ bool operator!=(ध्रुव UVector32& other) ध्रुव;

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    अन्तःस्थ शून्य addElement(int32_t elem, UErrorCode &status);

    शून्य setElementAt(int32_t elem, int32_t index);

    शून्य निवेशय_Element_At(int32_t elem, int32_t index, UErrorCode &status);
    
    अन्तःस्थ int32_t elementAti(int32_t index) ध्रुव;

    UBool equals(ध्रुव UVector32 &other) ध्रुव;

    अन्तःस्थ int32_t lastElementi() ध्रुव;

    int32_t indexOf(int32_t elem, int32_t startIndex = 0) ध्रुव;

    अन्तःस्थ UBool contains(int32_t elem) ध्रुव;

    UBool containsAll(ध्रुव UVector32& other) ध्रुव;

    UBool removeAll(ध्रुव UVector32& other);

    UBool retainAll(ध्रुव UVector32& other);

    शून्य removeElementAt(int32_t index);

    शून्य removeAllElements();

    अन्तःस्थ int32_t size() ध्रुव;

    अन्तःस्थ UBool isEmpty() ध्रुव;

    // Inline.  Use this one for speedy size check.
    अन्तःस्थ UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expandCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    शून्य setSize(int32_t newSize);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool containsNone(ध्रुव UVector32& other) ध्रुव;


    /**
     * Insert the given integer into this vector at its sorted position.
     * The current elements are assumed to be sorted already.
     */
    शून्य sorted_निवेशय(int32_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    अन्तःस्थ int32_t *getBuffer() ध्रुव;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (32 bits each), not bytes.
     */
    शून्य setMaxCapacity(int32_t limit);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    आभास UClassID getDynamicClassID() ध्रुव override;

गुप्त:
    शून्य _init(int32_t initialCapacity, UErrorCode &status);

    // Disallow
    UVector32(ध्रुव UVector32&) = delete;

    // Disallow
    UVector32& operator=(ध्रुव UVector32&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector32, they are all together.
सार्वजनिक:
    अन्तःस्थ UBool empty() ध्रुव;   // TODO:  redundant, same as empty().  Remove it?

    अन्तःस्थ int32_t peeki() ध्रुव;
    
    अन्तःस्थ int32_t popi();
    
    अन्तःस्थ int32_t प्रक्षिप(int32_t i, UErrorCode &status);

    अन्तःस्थ int32_t *reserveBlock(int32_t size, UErrorCode &status);
    अन्तःस्थ int32_t *निष्कासय_Frame(int32_t size);
};


// UVector32 inlines

अन्तःस्थ UBool UVector32::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    जइ ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        प्रत्यागम true;
    } अण्णहा {
        प्रत्यागम expandCapacity(minimumCapacity, status);
    }
}

अन्तःस्थ int32_t UVector32::elementAti(int32_t index) ध्रुव {
    प्रत्यागम (index >= 0 && गणनम् > 0 && गणनम् - index > 0) ? elements[index] : 0;
}


अन्तःस्थ शून्य UVector32::addElement(int32_t elem, UErrorCode &status) {
    जइ (ensureCapacity(गणनम् + 1, status)) {
        elements[गणनम्] = elem;
        गणनम्++;
    }
}

अन्तःस्थ int32_t *UVector32::reserveBlock(int32_t size, UErrorCode &status) {
    जइ (ensureCapacity(गणनम्+size, status) == false) {
        प्रत्यागम nullptr;
    }
    int32_t  *rp = elements+गणनम्;
    गणनम् += size;
    प्रत्यागम rp;
}

अन्तःस्थ int32_t *UVector32::निष्कासय_Frame(int32_t size) {
    U_ASSERT(गणनम् >= size);
    गणनम् -= size;
    जइ (गणनम् < 0) {
        गणनम् = 0;
    }
    प्रत्यागम elements+गणनम्-size;
}



अन्तःस्थ int32_t UVector32::size() ध्रुव {
    प्रत्यागम गणनम्;
}

अन्तःस्थ UBool UVector32::isEmpty() ध्रुव {
    प्रत्यागम गणनम् == 0;
}

अन्तःस्थ UBool UVector32::contains(int32_t obj) ध्रुव {
    प्रत्यागम indexOf(obj) >= 0;
}

अन्तःस्थ int32_t UVector32::lastElementi() ध्रुव {
    प्रत्यागम elementAti(गणनम्-1);
}

अन्तःस्थ bool UVector32::operator!=(ध्रुव UVector32& other) ध्रुव {
    प्रत्यागम !operator==(other);
}

अन्तःस्थ int32_t *UVector32::getBuffer() ध्रुव {
    प्रत्यागम elements;
}


// UStack inlines

अन्तःस्थ UBool UVector32::empty() ध्रुव {
    प्रत्यागम isEmpty();
}

अन्तःस्थ int32_t UVector32::peeki() ध्रुव {
    प्रत्यागम lastElementi();
}

अन्तःस्थ int32_t UVector32::प्रक्षिप(int32_t i, UErrorCode &status) {
    addElement(i, status);
    प्रत्यागम i;
}

अन्तःस्थ int32_t UVector32::popi() {
    int32_t result = 0;
    जइ (गणनम् > 0) {
        गणनम्--;
        result = elements[गणनम्];
    }
    प्रत्यागम result;
}

U_NAMESPACE_END

#endif
