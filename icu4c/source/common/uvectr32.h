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

#ضمن "unicode/utypes.h"
#ضمن "unicode/uobject.h"
#ضمن "uhash.h"
#ضمن "uassert.h"

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
class U_COMMON_API UVector32 : عام UObject {
خاص:
    int32_t   count;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int32_t*  elements;

عام:
    UVector32(UErrorCode &status);

    UVector32(int32_t initialCapacity, UErrorCode &status);

    virtual ~UVector32();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    فراغ assign(ثابت UVector32& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(ثابت UVector32& other) ثابت;

    /**
     * Equivalent to !operator==()
     */
    مضمن bool operator!=(ثابت UVector32& other) ثابت;

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    مضمن فراغ addElement(int32_t elem, UErrorCode &status);

    فراغ setElementAt(int32_t elem, int32_t index);

    فراغ insertElementAt(int32_t elem, int32_t index, UErrorCode &status);
    
    مضمن int32_t elementAti(int32_t index) ثابت;

    UBool equals(ثابت UVector32 &other) ثابت;

    مضمن int32_t lastElementi() ثابت;

    int32_t indexOf(int32_t elem, int32_t startIndex = 0) ثابت;

    مضمن UBool contains(int32_t elem) ثابت;

    UBool containsAll(ثابت UVector32& other) ثابت;

    UBool removeAll(ثابت UVector32& other);

    UBool retainAll(ثابت UVector32& other);

    فراغ removeElementAt(int32_t index);

    فراغ removeAllElements();

    مضمن int32_t size() ثابت;

    مضمن UBool isEmpty() ثابت;

    // Inline.  Use this one for speedy size check.
    مضمن UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expandCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    فراغ setSize(int32_t newSize);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool containsNone(ثابت UVector32& other) ثابت;


    /**
     * Insert the given integer into this vector at its sorted position.
     * The current elements are assumed to be sorted already.
     */
    فراغ sortedInsert(int32_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    مضمن int32_t *getBuffer() ثابت;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (32 bits each), not bytes.
     */
    فراغ setMaxCapacity(int32_t limit);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    virtual UClassID getDynamicClassID() ثابت override;

خاص:
    فراغ _init(int32_t initialCapacity, UErrorCode &status);

    // Disallow
    UVector32(ثابت UVector32&) = delete;

    // Disallow
    UVector32& operator=(ثابت UVector32&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector32, they are all together.
عام:
    مضمن UBool empty() ثابت;   // TODO:  redundant, same as empty().  Remove it?

    مضمن int32_t peeki() ثابت;
    
    مضمن int32_t popi();
    
    مضمن int32_t push(int32_t i, UErrorCode &status);

    مضمن int32_t *reserveBlock(int32_t size, UErrorCode &status);
    مضمن int32_t *popFrame(int32_t size);
};


// UVector32 inlines

مضمن UBool UVector32::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    ܐܢ ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        ܦܢܐ true;
    } ܐܚܪܢܐ {
        ܦܢܐ expandCapacity(minimumCapacity, status);
    }
}

مضمن int32_t UVector32::elementAti(int32_t index) ثابت {
    ܦܢܐ (index >= 0 && count > 0 && count - index > 0) ? elements[index] : 0;
}


مضمن فراغ UVector32::addElement(int32_t elem, UErrorCode &status) {
    ܐܢ (ensureCapacity(count + 1, status)) {
        elements[count] = elem;
        count++;
    }
}

مضمن int32_t *UVector32::reserveBlock(int32_t size, UErrorCode &status) {
    ܐܢ (ensureCapacity(count+size, status) == false) {
        ܦܢܐ nullptr;
    }
    int32_t  *rp = elements+count;
    count += size;
    ܦܢܐ rp;
}

مضمن int32_t *UVector32::popFrame(int32_t size) {
    U_ASSERT(count >= size);
    count -= size;
    ܐܢ (count < 0) {
        count = 0;
    }
    ܦܢܐ elements+count-size;
}



مضمن int32_t UVector32::size() ثابت {
    ܦܢܐ count;
}

مضمن UBool UVector32::isEmpty() ثابت {
    ܦܢܐ count == 0;
}

مضمن UBool UVector32::contains(int32_t obj) ثابت {
    ܦܢܐ indexOf(obj) >= 0;
}

مضمن int32_t UVector32::lastElementi() ثابت {
    ܦܢܐ elementAti(count-1);
}

مضمن bool UVector32::operator!=(ثابت UVector32& other) ثابت {
    ܦܢܐ !operator==(other);
}

مضمن int32_t *UVector32::getBuffer() ثابت {
    ܦܢܐ elements;
}


// UStack inlines

مضمن UBool UVector32::empty() ثابت {
    ܦܢܐ isEmpty();
}

مضمن int32_t UVector32::peeki() ثابت {
    ܦܢܐ lastElementi();
}

مضمن int32_t UVector32::push(int32_t i, UErrorCode &status) {
    addElement(i, status);
    ܦܢܐ i;
}

مضمن int32_t UVector32::popi() {
    int32_t result = 0;
    ܐܢ (count > 0) {
        count--;
        result = elements[count];
    }
    ܦܢܐ result;
}

U_NAMESPACE_END

#endif
