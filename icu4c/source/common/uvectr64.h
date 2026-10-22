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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "uassert.h"

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
class U_COMMON_API UVector64 : সার্বজনিক UObject {
গুপ্ত:
    int32_t   গুনতি;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int64_t*  elements;

সার্বজনিক:
    UVector64(UErrorCode &status);

    UVector64(int32_t initialCapacity, UErrorCode &status);

    আভাস ~UVector64();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    শূন্য assign(ধ্রুব UVector64& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(ধ্রুব UVector64& other);

    /**
     * Equivalent to !operator==()
     */
    অন্তঃস্থ bool operator!=(ধ্রুব UVector64& other);

    //------------------------------------------------------------
    // subset of java.util.Vector API
    //------------------------------------------------------------

    অন্তঃস্থ শূন্য addElement(int64_t elem, UErrorCode &status);

    শূন্য setElementAt(int64_t elem, int32_t index);

    শূন্য ঢোকাও_Element_At(int64_t elem, int32_t index, UErrorCode &status);
    
    অন্তঃস্থ int64_t elementAti(int32_t index) ধ্রুব;

    //UBool equals(const UVector64 &other) const;

    অন্তঃস্থ int64_t lastElementi() ধ্রুব;

    //int32_t indexOf(int64_t elem, int32_t startIndex = 0) const;

    //UBool contains(int64_t elem) const;

    //UBool containsAll(const UVector64& other) const;

    //UBool removeAll(const UVector64& other);

    //UBool retainAll(const UVector64& other);

    //void removeElementAt(int32_t index);

    শূন্য removeAllElements();

    অন্তঃস্থ int32_t size() ধ্রুব;

    অন্তঃস্থ UBool isEmpty() ধ্রুব { প্রত্যাগম গুনতি == 0; }

    // Inline.  Use this one for speedy size check.
    অন্তঃস্থ UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expandCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    শূন্য setSize(int32_t newSize);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    //UBool containsNone(const UVector64& other) const;


    //void sortedInsert(int64_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    অন্তঃস্থ int64_t *getBuffer() ধ্রুব;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (64 bits each), not bytes.
     */
    শূন্য setMaxCapacity(int32_t limit);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    স্থির UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    আভাস UClassID getDynamicClassID() ধ্রুব override;

গুপ্ত:
    শূন্য _init(int32_t initialCapacity, UErrorCode &status);

    // Disallow
    UVector64(ধ্রুব UVector64&) = delete;

    // Disallow
    UVector64& operator=(ধ্রুব UVector64&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector64, they are all together.
সার্বজনিক:
    //UBool empty() const;   // TODO:  redundant, same as empty().  Remove it?

    //int64_t peeki() const;
    
    অন্তঃস্থ int64_t popi();
    
    অন্তঃস্থ int64_t ঠেলো(int64_t i, UErrorCode &status);

    অন্তঃস্থ int64_t *reserveBlock(int32_t size, UErrorCode &status);
    অন্তঃস্থ int64_t *বের_করো_Frame(int32_t size);
};


// UVector64 inlines

অন্তঃস্থ UBool UVector64::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    যদি ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        প্রত্যাগম true;
    } অন্যথা {
        প্রত্যাগম expandCapacity(minimumCapacity, status);
    }
}

অন্তঃস্থ int64_t UVector64::elementAti(int32_t index) ধ্রুব {
    প্রত্যাগম (0 <= index && index < গুনতি) ? elements[index] : 0;
}


অন্তঃস্থ শূন্য UVector64::addElement(int64_t elem, UErrorCode &status) {
    যদি (ensureCapacity(গুনতি + 1, status)) {
        elements[গুনতি] = elem;
        গুনতি++;
    }
}

অন্তঃস্থ int64_t *UVector64::reserveBlock(int32_t size, UErrorCode &status) {
    যদি (ensureCapacity(গুনতি+size, status) == false) {
        প্রত্যাগম nullptr;
    }
    int64_t  *rp = elements+গুনতি;
    গুনতি += size;
    প্রত্যাগম rp;
}

অন্তঃস্থ int64_t *UVector64::বের_করো_Frame(int32_t size) {
    U_ASSERT(গুনতি >= size);
    গুনতি -= size;
    যদি (গুনতি < 0) {
        গুনতি = 0;
    }
    প্রত্যাগম elements+গুনতি-size;
}



অন্তঃস্থ int32_t UVector64::size() ধ্রুব {
    প্রত্যাগম গুনতি;
}

অন্তঃস্থ int64_t UVector64::lastElementi() ধ্রুব {
    প্রত্যাগম elementAti(গুনতি-1);
}

অন্তঃস্থ bool UVector64::operator!=(ধ্রুব UVector64& other) {
    প্রত্যাগম !operator==(other);
}

অন্তঃস্থ int64_t *UVector64::getBuffer() ধ্রুব {
    প্রত্যাগম elements;
}


// UStack inlines

অন্তঃস্থ int64_t UVector64::ঠেলো(int64_t i, UErrorCode &status) {
    addElement(i, status);
    প্রত্যাগম i;
}

অন্তঃস্থ int64_t UVector64::popi() {
    int64_t result = 0;
    যদি (গুনতি > 0) {
        গুনতি--;
        result = elements[গুনতি];
    }
    প্রত্যাগম result;
}

U_NAMESPACE_END

#endif
