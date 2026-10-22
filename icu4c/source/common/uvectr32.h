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

#شامل "unicode/utypes.h"
#شامل "unicode/uobject.h"
#شامل "uhash.h"
#شامل "uassert.h"

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
class U_COMMON_API UVector32 : عمومی UObject {
خصوصی:
    int32_t   شمار;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int32_t*  elements;

عمومی:
    UVector32(UErrorCode &status);

    UVector32(int32_t initialCapacity, UErrorCode &status);

    مجازی ~UVector32();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    تهی assign(ثابت UVector32& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(ثابت UVector32& other) ثابت;

    /**
     * Equivalent to !operator==()
     */
    درونخطی bool operator!=(ثابت UVector32& other) ثابت;

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    درونخطی تهی addElement(int32_t elem, UErrorCode &status);

    تهی setElementAt(int32_t elem, int32_t index);

    تهی درج_Element_At(int32_t elem, int32_t index, UErrorCode &status);
    
    درونخطی int32_t elementAti(int32_t index) ثابت;

    UBool equals(ثابت UVector32 &other) ثابت;

    درونخطی int32_t lastElementi() ثابت;

    int32_t indexOf(int32_t elem, int32_t startIndex = 0) ثابت;

    درونخطی UBool contains(int32_t elem) ثابت;

    UBool containsAll(ثابت UVector32& other) ثابت;

    UBool removeAll(ثابت UVector32& other);

    UBool retainAll(ثابت UVector32& other);

    تهی removeElementAt(int32_t index);

    تهی removeAllElements();

    درونخطی int32_t size() ثابت;

    درونخطی UBool isEmpty() ثابت;

    // Inline.  Use this one for speedy size check.
    درونخطی UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expandCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    تهی setSize(int32_t newSize);

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
    تهی sorted_درج(int32_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    درونخطی int32_t *getBuffer() ثابت;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (32 bits each), not bytes.
     */
    تهی setMaxCapacity(int32_t limit);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ایستا UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    مجازی UClassID getDynamicClassID() ثابت override;

خصوصی:
    تهی _init(int32_t initialCapacity, UErrorCode &status);

    // Disallow
    UVector32(ثابت UVector32&) = delete;

    // Disallow
    UVector32& operator=(ثابت UVector32&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector32, they are all together.
عمومی:
    درونخطی UBool empty() ثابت;   // TODO:  redundant, same as empty().  Remove it?

    درونخطی int32_t peeki() ثابت;
    
    درونخطی int32_t popi();
    
    درونخطی int32_t افزودن(int32_t i, UErrorCode &status);

    درونخطی int32_t *reserveBlock(int32_t size, UErrorCode &status);
    درونخطی int32_t *برداشت_Frame(int32_t size);
};


// UVector32 inlines

درونخطی UBool UVector32::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    اگر ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        بازگشت true;
    } وگرنه {
        بازگشت expandCapacity(minimumCapacity, status);
    }
}

درونخطی int32_t UVector32::elementAti(int32_t index) ثابت {
    بازگشت (index >= 0 && شمار > 0 && شمار - index > 0) ? elements[index] : 0;
}


درونخطی تهی UVector32::addElement(int32_t elem, UErrorCode &status) {
    اگر (ensureCapacity(شمار + 1, status)) {
        elements[شمار] = elem;
        شمار++;
    }
}

درونخطی int32_t *UVector32::reserveBlock(int32_t size, UErrorCode &status) {
    اگر (ensureCapacity(شمار+size, status) == false) {
        بازگشت nullptr;
    }
    int32_t  *rp = elements+شمار;
    شمار += size;
    بازگشت rp;
}

درونخطی int32_t *UVector32::برداشت_Frame(int32_t size) {
    U_ASSERT(شمار >= size);
    شمار -= size;
    اگر (شمار < 0) {
        شمار = 0;
    }
    بازگشت elements+شمار-size;
}



درونخطی int32_t UVector32::size() ثابت {
    بازگشت شمار;
}

درونخطی UBool UVector32::isEmpty() ثابت {
    بازگشت شمار == 0;
}

درونخطی UBool UVector32::contains(int32_t obj) ثابت {
    بازگشت indexOf(obj) >= 0;
}

درونخطی int32_t UVector32::lastElementi() ثابت {
    بازگشت elementAti(شمار-1);
}

درونخطی bool UVector32::operator!=(ثابت UVector32& other) ثابت {
    بازگشت !operator==(other);
}

درونخطی int32_t *UVector32::getBuffer() ثابت {
    بازگشت elements;
}


// UStack inlines

درونخطی UBool UVector32::empty() ثابت {
    بازگشت isEmpty();
}

درونخطی int32_t UVector32::peeki() ثابت {
    بازگشت lastElementi();
}

درونخطی int32_t UVector32::افزودن(int32_t i, UErrorCode &status) {
    addElement(i, status);
    بازگشت i;
}

درونخطی int32_t UVector32::popi() {
    int32_t result = 0;
    اگر (شمار > 0) {
        شمار--;
        result = elements[شمار];
    }
    بازگشت result;
}

U_NAMESPACE_END

#endif
