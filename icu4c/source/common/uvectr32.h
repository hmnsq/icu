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

#include "unicode/utypes.h"
#include "unicode/uobject.h"
#include "uhash.h"
#include "uassert.h"

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
class U_COMMON_API UVector32 : public UObject {
private:
    int32_t   count;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int32_t*  elements;

public:
    UVector32(UErrorCode &status);

    UVector32(int32_t initialCapacity, UErrorCode &status);

    virtual ~UVector32();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    void assign(const UVector32& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(const UVector32& other) const;

    /**
     * Equivalent to !operator==()
     */
    anta_hstha bool operator!=(const UVector32& other) const;

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    anta_hstha void addElement(int32_t elem, UErrorCode &status);

    void setElementAt(int32_t elem, int32_t index);

    void insertElementAt(int32_t elem, int32_t index, UErrorCode &status);
    
    anta_hstha int32_t elementAti(int32_t index) const;

    UBool equals(const UVector32 &other) const;

    anta_hstha int32_t lastElementi() const;

    int32_t indexOf(int32_t elem, int32_t startIndex = 0) const;

    anta_hstha UBool contains(int32_t elem) const;

    UBool containsAll(const UVector32& other) const;

    UBool removeAll(const UVector32& other);

    UBool retainAll(const UVector32& other);

    void removeElementAt(int32_t index);

    void removeAllElements();

    anta_hstha int32_t size() const;

    anta_hstha UBool isEmpty() const;

    // Inline.  Use this one for speedy size check.
    anta_hstha UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expandCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    void setSize(int32_t newSize);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool containsNone(const UVector32& other) const;


    /**
     * Insert the given integer into this vector at its sorted position.
     * The current elements are assumed to be sorted already.
     */
    void sortedInsert(int32_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    anta_hstha int32_t *getBuffer() const;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (32 bits each), not bytes.
     */
    void setMaxCapacity(int32_t limit);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    static UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    virtual UClassID getDynamicClassID() const override;

private:
    void _init(int32_t initialCapacity, UErrorCode &status);

    // Disallow
    UVector32(const UVector32&) = delete;

    // Disallow
    UVector32& operator=(const UVector32&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector32, they are all together.
public:
    anta_hstha UBool empty() const;   // TODO:  redundant, same as empty().  Remove it?

    anta_hstha int32_t peeki() const;
    
    anta_hstha int32_t popi();
    
    anta_hstha int32_t push(int32_t i, UErrorCode &status);

    anta_hstha int32_t *reserveBlock(int32_t size, UErrorCode &status);
    anta_hstha int32_t *popFrame(int32_t size);
};


// UVector32 inlines

anta_hstha UBool UVector32::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    အကယ်၍ ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        ပြန်ပေး true;
    } မဟုတ်ပါက {
        ပြန်ပေး expandCapacity(minimumCapacity, status);
    }
}

anta_hstha int32_t UVector32::elementAti(int32_t index) const {
    ပြန်ပေး (index >= 0 && count > 0 && count - index > 0) ? elements[index] : 0;
}


anta_hstha void UVector32::addElement(int32_t elem, UErrorCode &status) {
    အကယ်၍ (ensureCapacity(count + 1, status)) {
        elements[count] = elem;
        count++;
    }
}

anta_hstha int32_t *UVector32::reserveBlock(int32_t size, UErrorCode &status) {
    အကယ်၍ (ensureCapacity(count+size, status) == false) {
        ပြန်ပေး nullptr;
    }
    int32_t  *rp = elements+count;
    count += size;
    ပြန်ပေး rp;
}

anta_hstha int32_t *UVector32::popFrame(int32_t size) {
    U_ASSERT(count >= size);
    count -= size;
    အကယ်၍ (count < 0) {
        count = 0;
    }
    ပြန်ပေး elements+count-size;
}



anta_hstha int32_t UVector32::size() const {
    ပြန်ပေး count;
}

anta_hstha UBool UVector32::isEmpty() const {
    ပြန်ပေး count == 0;
}

anta_hstha UBool UVector32::contains(int32_t obj) const {
    ပြန်ပေး indexOf(obj) >= 0;
}

anta_hstha int32_t UVector32::lastElementi() const {
    ပြန်ပေး elementAti(count-1);
}

anta_hstha bool UVector32::operator!=(const UVector32& other) const {
    ပြန်ပေး !operator==(other);
}

anta_hstha int32_t *UVector32::getBuffer() const {
    ပြန်ပေး elements;
}


// UStack inlines

anta_hstha UBool UVector32::empty() const {
    ပြန်ပေး isEmpty();
}

anta_hstha int32_t UVector32::peeki() const {
    ပြန်ပေး lastElementi();
}

anta_hstha int32_t UVector32::push(int32_t i, UErrorCode &status) {
    addElement(i, status);
    ပြန်ပေး i;
}

anta_hstha int32_t UVector32::popi() {
    int32_t result = 0;
    အကယ်၍ (count > 0) {
        count--;
        result = elements[count];
    }
    ပြန်ပေး result;
}

U_NAMESPACE_END

#endif
