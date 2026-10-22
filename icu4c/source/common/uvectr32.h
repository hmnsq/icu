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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

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
class U_COMMON_API UVector32 : ಸಾರ್ವಜನಿಕ UObject {
ಗುಪ್ತ:
    int32_t   ಎಣಿಕೆ;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int32_t*  elements;

ಸಾರ್ವಜನಿಕ:
    UVector32(UErrorCode &status);

    UVector32(int32_t initialCapacity, UErrorCode &status);

    ಆಭಾಸ ~UVector32();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    ಶೂನ್ಯ assign(ಧ್ರುವ UVector32& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(ಧ್ರುವ UVector32& other) ಧ್ರುವ;

    /**
     * Equivalent to !operator==()
     */
    ಅನ್ತಃಸ್ಥ bool operator!=(ಧ್ರುವ UVector32& other) ಧ್ರುವ;

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ addElement(int32_t elem, UErrorCode &status);

    ಶೂನ್ಯ setElementAt(int32_t elem, int32_t index);

    ಶೂನ್ಯ ಸೇರಿಸು_Element_At(int32_t elem, int32_t index, UErrorCode &status);
    
    ಅನ್ತಃಸ್ಥ int32_t elementAti(int32_t index) ಧ್ರುವ;

    UBool equals(ಧ್ರುವ UVector32 &other) ಧ್ರುವ;

    ಅನ್ತಃಸ್ಥ int32_t lastElementi() ಧ್ರುವ;

    int32_t indexOf(int32_t elem, int32_t startIndex = 0) ಧ್ರುವ;

    ಅನ್ತಃಸ್ಥ UBool contains(int32_t elem) ಧ್ರುವ;

    UBool containsAll(ಧ್ರುವ UVector32& other) ಧ್ರುವ;

    UBool removeAll(ಧ್ರುವ UVector32& other);

    UBool retainAll(ಧ್ರುವ UVector32& other);

    ಶೂನ್ಯ removeElementAt(int32_t index);

    ಶೂನ್ಯ removeAllElements();

    ಅನ್ತಃಸ್ಥ int32_t size() ಧ್ರುವ;

    ಅನ್ತಃಸ್ಥ UBool isEmpty() ಧ್ರುವ;

    // Inline.  Use this one for speedy size check.
    ಅನ್ತಃಸ್ಥ UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expandCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    ಶೂನ್ಯ setSize(int32_t newSize);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool containsNone(ಧ್ರುವ UVector32& other) ಧ್ರುವ;


    /**
     * Insert the given integer into this vector at its sorted position.
     * The current elements are assumed to be sorted already.
     */
    ಶೂನ್ಯ sorted_ಸೇರಿಸು(int32_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    ಅನ್ತಃಸ್ಥ int32_t *getBuffer() ಧ್ರುವ;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (32 bits each), not bytes.
     */
    ಶೂನ್ಯ setMaxCapacity(int32_t limit);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    ಆಭಾಸ UClassID getDynamicClassID() ಧ್ರುವ override;

ಗುಪ್ತ:
    ಶೂನ್ಯ _init(int32_t initialCapacity, UErrorCode &status);

    // Disallow
    UVector32(ಧ್ರುವ UVector32&) = delete;

    // Disallow
    UVector32& operator=(ಧ್ರುವ UVector32&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector32, they are all together.
ಸಾರ್ವಜನಿಕ:
    ಅನ್ತಃಸ್ಥ UBool empty() ಧ್ರುವ;   // TODO:  redundant, same as empty().  Remove it?

    ಅನ್ತಃಸ್ಥ int32_t peeki() ಧ್ರುವ;
    
    ಅನ್ತಃಸ್ಥ int32_t popi();
    
    ಅನ್ತಃಸ್ಥ int32_t ತಳ್ಳು(int32_t i, UErrorCode &status);

    ಅನ್ತಃಸ್ಥ int32_t *reserveBlock(int32_t size, UErrorCode &status);
    ಅನ್ತಃಸ್ಥ int32_t *ತೆಗೆ_Frame(int32_t size);
};


// UVector32 inlines

ಅನ್ತಃಸ್ಥ UBool UVector32::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    ಯದಿ ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        ಪ್ರತ್ಯಾಗಮ true;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ expandCapacity(minimumCapacity, status);
    }
}

ಅನ್ತಃಸ್ಥ int32_t UVector32::elementAti(int32_t index) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ (index >= 0 && ಎಣಿಕೆ > 0 && ಎಣಿಕೆ - index > 0) ? elements[index] : 0;
}


ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ UVector32::addElement(int32_t elem, UErrorCode &status) {
    ಯದಿ (ensureCapacity(ಎಣಿಕೆ + 1, status)) {
        elements[ಎಣಿಕೆ] = elem;
        ಎಣಿಕೆ++;
    }
}

ಅನ್ತಃಸ್ಥ int32_t *UVector32::reserveBlock(int32_t size, UErrorCode &status) {
    ಯದಿ (ensureCapacity(ಎಣಿಕೆ+size, status) == false) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    int32_t  *rp = elements+ಎಣಿಕೆ;
    ಎಣಿಕೆ += size;
    ಪ್ರತ್ಯಾಗಮ rp;
}

ಅನ್ತಃಸ್ಥ int32_t *UVector32::ತೆಗೆ_Frame(int32_t size) {
    U_ASSERT(ಎಣಿಕೆ >= size);
    ಎಣಿಕೆ -= size;
    ಯದಿ (ಎಣಿಕೆ < 0) {
        ಎಣಿಕೆ = 0;
    }
    ಪ್ರತ್ಯಾಗಮ elements+ಎಣಿಕೆ-size;
}



ಅನ್ತಃಸ್ಥ int32_t UVector32::size() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;
}

ಅನ್ತಃಸ್ಥ UBool UVector32::isEmpty() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ == 0;
}

ಅನ್ತಃಸ್ಥ UBool UVector32::contains(int32_t obj) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ indexOf(obj) >= 0;
}

ಅನ್ತಃಸ್ಥ int32_t UVector32::lastElementi() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ elementAti(ಎಣಿಕೆ-1);
}

ಅನ್ತಃಸ್ಥ bool UVector32::operator!=(ಧ್ರುವ UVector32& other) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ !operator==(other);
}

ಅನ್ತಃಸ್ಥ int32_t *UVector32::getBuffer() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ elements;
}


// UStack inlines

ಅನ್ತಃಸ್ಥ UBool UVector32::empty() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ isEmpty();
}

ಅನ್ತಃಸ್ಥ int32_t UVector32::peeki() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ lastElementi();
}

ಅನ್ತಃಸ್ಥ int32_t UVector32::ತಳ್ಳು(int32_t i, UErrorCode &status) {
    addElement(i, status);
    ಪ್ರತ್ಯಾಗಮ i;
}

ಅನ್ತಃಸ್ಥ int32_t UVector32::popi() {
    int32_t result = 0;
    ಯದಿ (ಎಣಿಕೆ > 0) {
        ಎಣಿಕೆ--;
        result = elements[ಎಣಿಕೆ];
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

U_NAMESPACE_END

#endif
