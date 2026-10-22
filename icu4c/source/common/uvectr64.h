// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

//
//  UVector64 is a class implementing a vector of 64 bit integers.
//            It is similar to UVector32, but holds int64_t values rather than int32_t.
//            Most of the code is unchanged from UVector.
//

#ifndef UVECTOR64_H
#define UVECTOR64_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

U_NAMESPACE_BEGIN



/**
 * <p>Ultralightweight C++ implementation of an <tt>int64_t</tt> vector
 * that has a subset of methods from UVector32
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
 */
class U_COMMON_API UVector64 : ಸಾರ್ವಜನಿಕ UObject {
ಗುಪ್ತ:
    int32_t   ಎಣಿಕೆ;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int64_t*  elements;

ಸಾರ್ವಜನಿಕ:
    UVector64(UErrorCode &status);

    UVector64(int32_t initialCapacity, UErrorCode &status);

    ಆಭಾಸ ~UVector64();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    ಶೂನ್ಯ assign(ಧ್ರುವ UVector64& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(ಧ್ರುವ UVector64& other);

    /**
     * Equivalent to !operator==()
     */
    ಅನ್ತಃಸ್ಥ bool operator!=(ಧ್ರುವ UVector64& other);

    //------------------------------------------------------------
    // subset of java.util.Vector API
    //------------------------------------------------------------

    ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ addElement(int64_t elem, UErrorCode &status);

    ಶೂನ್ಯ setElementAt(int64_t elem, int32_t index);

    ಶೂನ್ಯ ಸೇರಿಸು_Element_At(int64_t elem, int32_t index, UErrorCode &status);
    
    ಅನ್ತಃಸ್ಥ int64_t elementAti(int32_t index) ಧ್ರುವ;

    //UBool equals(const UVector64 &other) const;

    ಅನ್ತಃಸ್ಥ int64_t lastElementi() ಧ್ರುವ;

    //int32_t indexOf(int64_t elem, int32_t startIndex = 0) const;

    //UBool contains(int64_t elem) const;

    //UBool containsAll(const UVector64& other) const;

    //UBool removeAll(const UVector64& other);

    //UBool retainAll(const UVector64& other);

    //void removeElementAt(int32_t index);

    ಶೂನ್ಯ removeAllElements();

    ಅನ್ತಃಸ್ಥ int32_t size() ಧ್ರುವ;

    ಅನ್ತಃಸ್ಥ UBool isEmpty() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ == 0; }

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

    //UBool containsNone(const UVector64& other) const;


    //void sortedInsert(int64_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    ಅನ್ತಃಸ್ಥ int64_t *getBuffer() ಧ್ರುವ;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (64 bits each), not bytes.
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
    UVector64(ಧ್ರುವ UVector64&) = delete;

    // Disallow
    UVector64& operator=(ಧ್ರುವ UVector64&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector64, they are all together.
ಸಾರ್ವಜನಿಕ:
    //UBool empty() const;   // TODO:  redundant, same as empty().  Remove it?

    //int64_t peeki() const;
    
    ಅನ್ತಃಸ್ಥ int64_t popi();
    
    ಅನ್ತಃಸ್ಥ int64_t ತಳ್ಳು(int64_t i, UErrorCode &status);

    ಅನ್ತಃಸ್ಥ int64_t *reserveBlock(int32_t size, UErrorCode &status);
    ಅನ್ತಃಸ್ಥ int64_t *ತೆಗೆ_Frame(int32_t size);
};


// UVector64 inlines

ಅನ್ತಃಸ್ಥ UBool UVector64::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    ಯದಿ ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        ಪ್ರತ್ಯಾಗಮ true;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ expandCapacity(minimumCapacity, status);
    }
}

ಅನ್ತಃಸ್ಥ int64_t UVector64::elementAti(int32_t index) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ (0 <= index && index < ಎಣಿಕೆ) ? elements[index] : 0;
}


ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ UVector64::addElement(int64_t elem, UErrorCode &status) {
    ಯದಿ (ensureCapacity(ಎಣಿಕೆ + 1, status)) {
        elements[ಎಣಿಕೆ] = elem;
        ಎಣಿಕೆ++;
    }
}

ಅನ್ತಃಸ್ಥ int64_t *UVector64::reserveBlock(int32_t size, UErrorCode &status) {
    ಯದಿ (ensureCapacity(ಎಣಿಕೆ+size, status) == false) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    int64_t  *rp = elements+ಎಣಿಕೆ;
    ಎಣಿಕೆ += size;
    ಪ್ರತ್ಯಾಗಮ rp;
}

ಅನ್ತಃಸ್ಥ int64_t *UVector64::ತೆಗೆ_Frame(int32_t size) {
    U_ASSERT(ಎಣಿಕೆ >= size);
    ಎಣಿಕೆ -= size;
    ಯದಿ (ಎಣಿಕೆ < 0) {
        ಎಣಿಕೆ = 0;
    }
    ಪ್ರತ್ಯಾಗಮ elements+ಎಣಿಕೆ-size;
}



ಅನ್ತಃಸ್ಥ int32_t UVector64::size() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;
}

ಅನ್ತಃಸ್ಥ int64_t UVector64::lastElementi() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ elementAti(ಎಣಿಕೆ-1);
}

ಅನ್ತಃಸ್ಥ bool UVector64::operator!=(ಧ್ರುವ UVector64& other) {
    ಪ್ರತ್ಯಾಗಮ !operator==(other);
}

ಅನ್ತಃಸ್ಥ int64_t *UVector64::getBuffer() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ elements;
}


// UStack inlines

ಅನ್ತಃಸ್ಥ int64_t UVector64::ತಳ್ಳು(int64_t i, UErrorCode &status) {
    addElement(i, status);
    ಪ್ರತ್ಯಾಗಮ i;
}

ಅನ್ತಃಸ್ಥ int64_t UVector64::popi() {
    int64_t result = 0;
    ಯದಿ (ಎಣಿಕೆ > 0) {
        ಎಣಿಕೆ--;
        result = elements[ಎಣಿಕೆ];
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

U_NAMESPACE_END

#endif
